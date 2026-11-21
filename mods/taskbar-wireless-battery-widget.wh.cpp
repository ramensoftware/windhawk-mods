// ==WindhawkMod==
// @id              taskbar-wireless-battery-widget
// @name            Taskbar Wireless Battery Widget
// @description     Show wireless device battery levels as part of the taskbar itself, or as tray icons. Based on BatteryHub's device support.
// @version         1.0
// @author          TotallyWireless
// @github          https://github.com/TotallyWireless
// @include         windhawk.exe
// @include         explorer.exe
// @license         GPL-3.0-or-later
// @compilerOptions -lhid -lsetupapi -lcfgmgr32 -lole32 -loleaut32 -lcomctl32 -lgdi32 -lruntimeobject -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Wireless Battery Widget

![Screenshot](https://i.imgur.com/Q1EWb9n.png)

Display battery levels for supported wireless mice, keyboards, headsets and
controllers in the Windows 11 taskbar, or as one system tray icon per device.
Click the battery area to refresh or show/hide individual devices. Hover for details.
Configure the display mode, refresh interval, low-battery threshold, font size and
device spacing in Windhawk. Windhawk manages startup and enabling/disabling the mod.

In taskbar mode, the battery levels are part of the primary taskbar, at the left
of its notification area. On Windows 10, or with a taskbar that isn't the
Windows 11 one, tray icons are shown instead.

The devices are queried in a dedicated Windhawk process, and Explorer only shows
the battery levels in the taskbar. The mod doesn't run together with the
standalone BatteryHub app (formerly Wireless Battery) or its Chinese adaptation,
so that the devices aren't queried twice. Exit the standalone app before
enabling the mod. If the display does not start, exit that app and
disable/re-enable the mod.

## Device support and limitations

- The device drivers are ported from [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor),
  reviewed up to its commit [fd2267d](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor/commit/fd2267d770c9dafc46047f715e79da525e3fecf4)
  (November 20, 2026). Brand support does not mean every model is supported,
  and not every BatteryHub driver is included (see below).
- Vendor-specific battery queries send HID output/feature reports, or serial
  commands for supported ATK headsets, to peripherals and read their replies.
  These community-derived protocols have not been verified on every supported
  model. Queries can conflict with vendor utilities that access a device
  exclusively.
- Periodic queries can wake a device right after it goes to sleep, or keep its
  lights from turning off. Add such a device to the **Devices to exclude**
  setting to stop querying it. Hiding a device in the menu only hides it from
  the display.
- Drawing tablets are never queried for a standard HID battery level.
- Bluetooth PlayStation controllers are only read after another app (e.g. Steam)
  has switched them to full reports. The mod doesn't do that itself, because it
  breaks DirectInput for other apps until the controller is turned off and on.
- Standard Bluetooth battery readings depend on what Windows exposes.
- A phone's battery is read from the status file of Microsoft Phone Link, when
  Phone Link is set up. The phone itself isn't contacted.
- In taskbar mode low battery is indicated by red text; notification balloons
  are available in tray mode. Sleep/last-known readings are dimmed.

## Differences from BatteryHub

- Battery queries are only sent to product IDs known to use each protocol,
  including on vendor IDs that several brands share and that BatteryHub queries
  as a whole (Glorious, Hitscan, G-Wolves, Kysona and its partner brands,
  LAMZU, ZOWIE).
- Not included: drivers that switch a device to another mode or need
  keep-alive traffic (e.g. Razer Barracuda X, Razer BlackShark V2 Pro (2023),
  Logitech G435, Turtle Beach), drivers that hold Bluetooth connections (Galaxy
  Buds, Nothing Ear, Steam Controller, GameSir Cyclone Pro), Google Fast Pair
  earbud details, the HeadsetControl integration, and drivers that query many
  products of a shared vendor ID or haven't been reviewed for the mod yet (e.g.
  Keychron mice, Jabra, 8BitDo, NuPhy, Royal Kludge, VGN).
- Bluetooth PlayStation controllers aren't switched to full reports (see above).
- Some devices are asked less often: the Razer BlackShark V3 / V3 X rate limit
  (one query per 5 minutes) also counts failed queries, and the ATK Z87
  receiver is asked every 10 minutes, also after a failed query.
- AirPods are read by the mod's own receiver, which is off by default (see
  below).
- The taskbar widget is part of the taskbar's layout, not a window placed over
  the taskbar, so the task buttons and the tray make room for it.
- BatteryHub's app features, such as battery history, alert rules, renaming
  and updates, aren't included.

## Optional AirPods reception

AirPods BLE reception is **off by default**. Enable it in settings if needed.
It passively receives nearby Apple proximity broadcasts while enabled, without
changing pairing or audio settings. Battery levels are approximate, in steps
of about 10%; open the charging case near the computer to obtain case readings.

Only AirPods and Beats models that are paired with this PC are shown, with the
strongest recent broadcast of each model. Broadcast ownership cannot be
verified, so a nearby pair of the same model that belongs to someone else can
be shown instead. Readings older than 30 seconds are marked as historical at
the next refresh, and entries expire after about two minutes without a
broadcast. Raw Bluetooth addresses and encrypted payloads are not written to
disk.

## Credits and license

GNU GPL v3.0 or later. Based on [BatteryHub](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor) (formerly Wireless
Battery) and the [Chinese / G87 adaptation](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor/pull/43).
The original port to a Windhawk mod was done by [yaoiyiXu](https://github.com/yaoiyiXu)
in [windhawk-mods#7564](https://github.com/ramensoftware/windhawk-mods/pull/7564).
AirPods protocol reference: [SpriteOvO/AirPodsDesktop](https://github.com/SpriteOvO/AirPodsDesktop)
(GPL-3.0-or-later). The dedicated host follows the official Windhawk "Mods as tools" example.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayMode: taskbar
  $name: Display mode
  $options:
  - taskbar: Taskbar icons and battery levels
  - tray: One tray icon per device
- pollSeconds: 60
  $name: Device refresh interval (seconds, 10-600)
- warningPercent: 15
  $name: Low-battery threshold (0-50)
- fontSize: 13
  $name: Battery font size (10-24)
- deviceGap: 14
  $name: Device spacing (4-40)
- hideSleeping: false
  $name: Hide sleeping devices and last-known readings
- enableAirPods: false
  $name: Receive nearby AirPods battery broadcasts
  $description: >-
    Approximate readings for AirPods paired with this PC; a nearby pair of the
    same model can be shown instead.
- ignoredDevices: [""]
  $name: Devices to exclude
  $description: >-
    Battery queries are never sent to these devices. Each entry is a hex
    VID:PID pair, for example 28BD:0947 for a device with the hardware ID
    VID_28BD&PID_0947 (Device Manager, device properties, Details tab).
*/
// ==/WindhawkModSettings==

// The device drivers are ported from BatteryHub (github.com/krisgfx9/Wireless-Device-Battery-Monitor),
// reviewed up to commit fd2267d770c9dafc46047f715e79da525e3fecf4 (2026-11-20). Differences from upstream:
//  * Vendor protocols are only sent to known product IDs (Glorious, Hitscan, G-Wolves, Kysona, LAMZU,
//    ZOWIE DW, AJ179, AJ-series), and the Darmoshark Nordic receiver also needs its product name.
//  * Upstream drivers that are not included:
//    - mode switches or keep-alive traffic: barracuda_pro, logitech_g435_0adf, turtlebeach, plantronics,
//      finalmouse, eightbitdo; in razer_headset the Barracuda X, BlackShark V2 Pro (2023) and V2
//      HyperSpeed; in gamesir the Cyclone 2; in misc_mice the Wraith W1 (and Incott, which shares its ids);
//    - many products of a shared vendor id: keychron, jabra, mchose (5253), rawm, raigor, sonix, via_dongle;
//    - Bluetooth connections or BLE infrastructure: galaxy_buds, nothing_ear, steam_controller, gamesir_ble,
//      fastpair / ble_adverts, and headsetcontrol (external tool);
//    - not reviewed yet: ajazz_ak992, beitong, chimera, darkrim, eweadn, fantech, flydigi, gamesir_sapjoy,
//      maono, moondrop, niceboy, nuphy, pressplay_icarus, redragon_evision, royal_kludge, scyrox, semico,
//      sinowealth_receiver, vgn, zenith; in razer_headset the BlackShark V2 Pro (2020); in misc_mice the
//      AJ139, Delux M800 and ARDOR Fury V2.
//  * Bluetooth PlayStation controllers are not switched to full reports.
//  * The BlackShark V3 / V3 X rate limit also applies to queries that fail, and the ATK Z87 receiver is
//    not asked again 30 s after a failed query.
//  * The full-charge state, the per-bud Fast Pair details and the app features (history, alerts, renaming,
//    updates) are not included; the display and the AirPods reader are the mod's own.

// Driver namespaces isolate file-local helpers from the original translation units.

#include <windows.h>
#include <windhawk_utils.h>
#include <initguid.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <algorithm>
#include <atomic>
#include <cfgmgr32.h>
#include <cmath>
#include <commctrl.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <dbt.h>
#include <devguid.h>
#include <devpkey.h>
#include <hidclass.h>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <roapi.h>
#include <set>
#include <setupapi.h>
#include <shellapi.h>
#include <string>
#include <utility>
#include <vector>
#include <winstring.h>
#include <xinput.h>

#ifdef __MINGW32__
#define boolean bool
#endif
#include <windows.system.power.h>
namespace ABI { namespace Windows { namespace Devices { namespace Power {
using ABI::Windows::System::Power::BatteryStatus;
} } } }
#include <windows.devices.power.h>
#include <windows.gaming.input.h>
#include <windows.storage.streams.h>
#include <robuffer.h>
#ifdef __MINGW32__
#undef boolean
#endif

#undef GetCurrentTime  // a windows.h macro, which breaks Storyboard::GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

// Minimal declarations for the stable Windows Bluetooth advertisement ABI.
// Windhawk 1.7.3's SDK lacks windows.devices.bluetooth.advertisement.h.
// Interface order and IDs match the Windows SDK / mingw-w64 public WinRT headers.
// Unused method signatures below keep the original vtable slots; they are never invoked.
namespace WindhawkAirPodsAbi {
enum BluetoothLEAdvertisementWatcherStatus { BluetoothLEAdvertisementWatcherStatus_Created, BluetoothLEAdvertisementWatcherStatus_Started,
    BluetoothLEAdvertisementWatcherStatus_Stopping, BluetoothLEAdvertisementWatcherStatus_Stopped, BluetoothLEAdvertisementWatcherStatus_Aborted };
enum BluetoothLEScanningMode { BluetoothLEScanningMode_Passive, BluetoothLEScanningMode_Active };
struct IBluetoothLEAdvertisementWatcher;
struct IBluetoothLEAdvertisementReceivedEventArgs;
struct IBluetoothLEManufacturerData;

MIDL_INTERFACE("90eb4eca-d465-5ea0-a61c-033c8c5ecef2") Received : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke(IBluetoothLEAdvertisementWatcher*, IBluetoothLEAdvertisementReceivedEventArgs*) = 0;
};
MIDL_INTERFACE("78ab070e-ad7e-5912-a4f1-7be33e4560af") DataList : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT32, IBluetoothLEManufacturerData**) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(UINT32*) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedIndexOf() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetMany() = 0;
};
MIDL_INTERFACE("912dba18-6963-4533-b061-4694dafb34e5") IBluetoothLEManufacturerData : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_CompanyId(UINT16*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CompanyId(UINT16) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Data(ABI::Windows::Storage::Streams::IBuffer**) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Data(ABI::Windows::Storage::Streams::IBuffer*) = 0;
};
MIDL_INTERFACE("066fb2b7-33d1-4e7d-8367-cf81d0f79653") IBluetoothLEAdvertisement : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE UnusedGetFlags() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedPutFlags() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetLocalName() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedPutLocalName() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetServiceUuids() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetManufacturerData() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetDataSections() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetManufacturerDataByCompanyId(UINT16, DataList**) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetSectionsByType() = 0;
};
MIDL_INTERFACE("27987ddf-e596-41be-8d43-9e6731d4a913") IBluetoothLEAdvertisementReceivedEventArgs : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_RawSignalStrengthInDBm(INT16*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BluetoothAddress(UINT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetAdvertisementType() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetTimestamp() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Advertisement(IBluetoothLEAdvertisement**) = 0;
};
MIDL_INTERFACE("a6ac336f-f3d3-4297-8d6c-c81ea6623f40") IBluetoothLEAdvertisementWatcher : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE UnusedGetMinSamplingInterval() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetMaxSamplingInterval() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetMinOutOfRangeTimeout() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetMaxOutOfRangeTimeout() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Status(BluetoothLEAdvertisementWatcherStatus*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ScanningMode(BluetoothLEScanningMode*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ScanningMode(BluetoothLEScanningMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetSignalStrengthFilter() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedPutSignalStrengthFilter() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedGetAdvertisementFilter() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedPutAdvertisementFilter() = 0;
    virtual HRESULT STDMETHODCALLTYPE Start() = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop() = 0;
    virtual HRESULT STDMETHODCALLTYPE add_Received(Received*, EventRegistrationToken*) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_Received(EventRegistrationToken) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedAddStopped() = 0;
    virtual HRESULT STDMETHODCALLTYPE UnusedRemoveStopped() = 0;
};
}
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(WindhawkAirPodsAbi::Received, 0x90eb4eca, 0xd465, 0x5ea0, 0xa6, 0x1c, 0x03, 0x3c, 0x8c, 0x5e, 0xce, 0xf2)
__CRT_UUID_DECL(WindhawkAirPodsAbi::IBluetoothLEAdvertisementWatcher, 0xa6ac336f, 0xf3d3, 0x4297, 0x8d, 0x6c, 0xc8, 0x1e, 0xa6, 0x62, 0x3f, 0x40)
#endif

enum class DeviceKind { Other, Mouse, Headset, Keyboard, Gamepad, Phone, Microphone };

// One battery-powered device, as seen during a single poll.
struct DeviceStatus {
    std::wstring key;          // stable identity across polls (used for the tray icon id)
    std::wstring name;         // human readable name
    std::wstring via;          // "2.4 GHz receiver", "Bluetooth", "USB cable", ...
    DeviceKind kind = DeviceKind::Other;
    int  percent    = -1;      // 0..100, or -1 when unknown right now
    std::wstring levelText;    // set when the device only reports coarse levels ("Low", "Full"...)
    std::wstring detailText;   // optional per-component battery information for tooltips/console
    bool charging   = false;
    bool asleep     = false;   // receiver answered but the device is sleeping (keep last value)
    bool connected  = true;    // false: receiver present, device switched off (hidden)
    int  millivolts = 0;       // battery voltage when reported (0 = unknown)
    int  graceSec   = 0;       // keep showing the last value this long if the device stops answering
};

// Queries every supported device. Blocking (can take a second or two); call from a worker thread.
std::vector<DeviceStatus> PollAllDevices(HANDLE stopEvent = nullptr);

// Thin Win32 HID layer shared by all drivers.
//
// Report buffers follow hidapi conventions so protocol code from other projects maps 1:1:
//  * Write/SetFeature: data[0] is the report ID (0 when the device does not number its reports);
//    the buffer is zero-padded to the report length.
//  * Read: a leading 0 report ID is stripped, so data[0] is the first payload byte for
//    unnumbered reports and the report ID for numbered ones.
struct HidColl {
    std::wstring path;
    USHORT vid = 0, pid = 0;
    int iface = -1;  // USB interface number (MI_xx in the path), -1 when unknown
    HIDP_CAPS caps{};
    USHORT page() const { return caps.UsagePage; }
    USHORT usage() const { return caps.Usage; }
    USHORT inLen() const { return caps.InputReportByteLength; }
    USHORT outLen() const { return caps.OutputReportByteLength; }
    USHORT featLen() const { return caps.FeatureReportByteLength; }
};

std::vector<HidColl> EnumHid();

class HidDev {
public:
    HidDev() = default;
    ~HidDev() { Close(); }
    HidDev(const HidDev&) = delete;
    HidDev& operator=(const HidDev&) = delete;

    bool Open(const HidColl& c);            // read/write, overlapped
    bool OpenReadOnly(const HidColl& c);    // read only, overlapped: for push-only collections another app may hold
    bool OpenFeatureOnly(const HidColl& c);  // zero access: enough for feature reports, works on mice/keyboards
    void Close();
    bool ok() const { return h_ != INVALID_HANDLE_VALUE; }
    const HidColl& coll() const { return c_; }

    bool Write(const std::vector<BYTE>& data, int timeoutMs = 500);
    bool Write(std::initializer_list<BYTE> data, int timeoutMs = 500) { return Write(std::vector<BYTE>(data), timeoutMs); }
    // Reads one input report into `buf` (resized to fit). Returns the byte count, or -1 on timeout/error.
    int Read(std::vector<BYTE>& buf, int timeoutMs);
    // For collections without an interrupt OUT pipe (WriteFile fails there): arms a read, sends
    // `data` with HidD_SetOutputReport, and returns the first input report like Read() does.
    int SetOutputAndRead(const std::vector<BYTE>& data, std::vector<BYTE>& buf, int timeoutMs);
    void Flush();  // discard queued input reports

    bool SetFeature(const std::vector<BYTE>& data);
    bool GetFeature(BYTE reportId, std::vector<BYTE>& out);  // out[0] = report ID
    bool GetFeatureByte(BYTE reportId, BYTE& value);  // numbered 2-byte report on a read handle
    bool GetInputReport(BYTE reportId, std::vector<BYTE>& out);  // current input report via the control pipe; out[0] = report ID

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
    bool overlapped_ = false;
    HidColl c_;
};

std::wstring HidProduct(const HidColl& c);
std::wstring Hex4(USHORT v);
int Clamp100(int v);
int MapTo100(int v, int lo, int hi);  // linear map lo..hi -> 0..100, clamped
// Piecewise-linear interpolation through (mv[i], pct[i]); tables sorted by descending voltage.
int Interpolate(int mv, const int* mvTable, const int* pctTable, int n);
DeviceKind GuessKind(const std::wstring& name);
// Device type from the top-level collections a device exposes (mouse > gamepad > keyboard).
DeviceKind KindFromCollections(const std::vector<HidColl>& all, USHORT vid, USHORT pid);

// Each driver looks at the enumerated HID collections, talks to the devices it recognises and
// appends what it finds. A driver claims a device (vendor/product id) once it has handled it so
// that the generic fallbacks further down the chain do not report it twice.
struct PollContext {
    std::vector<HidColl> colls;
    std::set<std::pair<USHORT, USHORT>> claimed;
    std::vector<DeviceStatus> out;
    std::set<std::wstring> btContainers;  // container ids PollBluetooth reported (skipped by PollGenericHid)

    bool IsClaimed(USHORT vid, USHORT pid) const { return claimed.count({ vid, pid }) != 0; }
    void Claim(USHORT vid, USHORT pid) { claimed.insert({ vid, pid }); }
    void Add(const DeviceStatus& s, USHORT vid, USHORT pid) {
        out.push_back(s);
        Claim(vid, pid);
    }
    // `base`, or `base#2`, `base#3`, ... when a connected device already uses it (identical receivers).
    std::wstring StableKey(const std::wstring& base) const {
        auto taken = [&](const std::wstring& k) {
            for (const auto& s : out)
                if (s.key == k && s.connected) return true;
            return false;
        };
        if (!taken(base)) return base;
        for (int n = 2;; ++n)
            if (!taken(base + L"#" + std::to_wstring(n))) return base + L"#" + std::to_wstring(n);
    }
    // Distinct (vid, pid) pairs for a vendor that are not claimed yet.
    std::vector<std::pair<USHORT, USHORT>> Products(USHORT vid) const {
        std::set<std::pair<USHORT, USHORT>> s;
        for (const auto& c : colls)
            if (c.vid == vid && !IsClaimed(c.vid, c.pid)) s.insert({ c.vid, c.pid });
        return { s.begin(), s.end() };
    }
    // Collections of one product, optionally filtered.
    template <class Pred>
    std::vector<const HidColl*> Find(USHORT vid, USHORT pid, Pred pred) const {
        std::vector<const HidColl*> r;
        for (const auto& c : colls)
            if (c.vid == vid && c.pid == pid && pred(c)) r.push_back(&c);
        return r;
    }
};

// G-Wolves model names (shared by the Glorious- and Kysona-protocol drivers).
const wchar_t* GWolvesModelName(USHORT pid);
bool GWolvesIsCable(USHORT pid);

// Vendor drivers (run in this order).
void PollGlorious(PollContext& ctx);          // also Hitscan, G-Wolves XVI, WLMouse YING
void PollAula(PollContext& ctx);              // AULA / MCHOSE / Redragon keyboards, AULA SC580 (before Kysona, same vendor id)
void PollAulaMouse(PollContext& ctx);         // AULA X9 Ultra and V9 mice (320F:22A0)
void PollDareu(PollContext& ctx);             // DAREU EM901X / A84, Togran, Sharkoon SGM50W
void PollDarmoshark(PollContext& ctx);        // Darmoshark M3 / M3S
void PollDarmosharkNrf(PollContext& ctx);     // Darmoshark M3S Pro (Nordic receiver)
void PollAtkSerial(PollContext& ctx);         // ATK N9 Pro / N9 Ultra headsets (virtual COM port)
void PollAtkKeyboard(PollContext& ctx);       // ATK / VGN / VXE Evision keyboards, ATK Z87
void PollPulsar(PollContext& ctx);            // Pulsar X2 V3 and other Sonix-based Pulsar mice
void PollAtkBitmouse(PollContext& ctx);       // ATK / VXE NearLink and 8K Nano dongles
void PollKysona(PollContext& ctx);            // also Pulsar, ATK / VXE, Hitscan Hyperlight, HECATE
void PollCherry(PollContext& ctx);            // CHERRY MX 3.0S Wireless, CHERRY XTRFY
void PollArbiter(PollContext& ctx);           // Arbiter Studio Akitsu (373E, before LAMZU)
void PollLamzu(PollContext& ctx);             // LAMZU mice on vendor ids 373E / 37B0
void PollEndgame(PollContext& ctx);           // Endgame Gear 4K mice
void PollEndgameWe(PollContext& ctx);         // Endgame Gear XM2we
void PollVaxee(PollContext& ctx);             // VAXEE receivers, ZYGEN NP-01S V2
void PollWlmouse(PollContext& ctx);           // WLMouse Beast / Sword
void PollNinjutso(PollContext& ctx);          // Ninjutso Sora V2 / Ten
void PollSprime(PollContext& ctx);            // Sprime PM1
void PollAkko(PollContext& ctx);              // Akko V9 Dash Pro / Dash V9 Ultra
void PollRongyuan(PollContext& ctx);          // Rongyuan receivers (Attack Shark, Ajazz keyboards, ...)
void PollSonyInzone(PollContext& ctx);        // Sony INZONE H9 II / H5 / Buds
void PollFractal(PollContext& ctx);           // Fractal Design Scape
void PollJbl(PollContext& ctx);               // JBL Quantum headsets
void PollRog(PollContext& ctx);               // ASUS ROG keyboards, mice and headsets
void PollZowie(PollContext& ctx);             // BenQ ZOWIE EC-CW / DW / U2 mice
void PollMchoseA5(PollContext& ctx);          // MCHOSE A5 Pro / Pro Max
void PollRapoo(PollContext& ctx);             // Rapoo VT gaming mice (passive battery reports)
void PollAreson(PollContext& ctx);            // Redragon mice on Areson receivers (M913, M915RGB-WL)
void PollMchoseG3(PollContext& ctx);          // MCHOSE G3 V2
void PollMsi(PollContext& ctx);               // MSI wireless mice
void PollMachenike(PollContext& ctx);         // Machenike L7 Arctic / L7 Mix
void PollMchoseModern(PollContext& ctx);      // MCHOSE K7 V2 Ultra+ and other Nordic mice
void PollMchoseHeadset(PollContext& ctx);     // MCHOSE V9 Pro 2 headset
void PollMchoseKeyboard(PollContext& ctx);    // MCHOSE keyboards on the 3837:3033 receiver
void PollMiscMice(PollContext& ctx);          // Motospeed X6, Xenta receivers, Ajazz AJ179 and AJ159/AJ-series
void PollRazerHeadsets(PollContext& ctx);     // Razer BlackShark V3 / V3 Pro / V3 X
void PollHyperX(PollContext& ctx);
void PollAstro(PollContext& ctx);             // ASTRO / Logitech A50 and A20 headsets
void PollLogitech(PollContext& ctx);
void PollRazer(PollContext& ctx);
void PollSteelSeriesElite(PollContext& ctx);  // SteelSeries Arctis Nova Elite
void PollSteelSeries(PollContext& ctx);
void PollAudeze(PollContext& ctx);            // Audeze Maxwell
void PollEndorfy(PollContext& ctx);           // ENDORFY Viro Wireless
void PollAkg(PollContext& ctx);               // AKG N9 Hybrid
void PollNzxt(PollContext& ctx);              // NZXT Lift Elite Wireless
void PollCorsair(PollContext& ctx);
void PollCoolerMaster(PollContext& ctx);
void PollPlayStation(PollContext& ctx);
void PollSwitchPro(PollContext& ctx);         // Switch Pro-compatible controllers (passive)
void PollGameSir(PollContext& ctx);           // Nova Lite / Nova 2 / Nova 2 Lite / Nova Lite 2
void PollPhoneLink(PollContext& ctx);         // phone battery from Microsoft Phone Link
void PollAlienware(PollContext& ctx);         // Alienware AW920H headset
// Fallbacks.
void PollBluetooth(PollContext& ctx);
void PollGenericHid(PollContext& ctx);
void PollAirPods(PollContext& ctx);
void PollXInput(PollContext& ctx);

// The taskbar widget, "[icon] 30%  [icon] 93%", is part of Explorer's taskbar XAML, while the devices
// are polled in the dedicated Windhawk process, so that a stuck device query never blocks Explorer.
// The polling process sends the widget's content with WM_COPYDATA to a message-only window, which
// Explorer creates while the widget is attached. Explorer posts clicks on the widget back to the
// polling process's window, and asks it to send the content again while it has none.
namespace WidgetIpc {
constexpr const wchar_t* kToolWindowClass = L"TaskbarWirelessBatteryWnd";
constexpr const wchar_t* kReceiverClass = L"TaskbarWirelessBatteryXaml";
constexpr ULONG_PTR kCopyDataId = 0x57424D31;  // "WBM1"
constexpr UINT kClickMessage = WM_APP + 3;
constexpr UINT kResendMessage = WM_APP + 4;

enum ItemFlags : UINT32 { kCharging = 1, kDim = 2, kLow = 4, kBluetooth = 8 };

struct Item {
    INT32 kind;             // DeviceKind
    INT32 percent;          // -1 = unknown
    UINT32 flags;           // ItemFlags; kBluetooth picks the Bluetooth icon for an unknown device type
    wchar_t text[24];       // shown instead of the percentage when set (coarse levels like "Low")
    wchar_t tooltip[256];
};

// Sent without the unused items.
struct Payload {
    UINT32 visible;         // 0 when tray icons are shown instead
    INT32 fontSize, gap;
    UINT32 count;
    Item items[32];
};
}  // namespace WidgetIpc

// Live, read-only BLE reception; no pairing, settings or audio changes.
void StartAirPods();
void StopAirPods();
void SetAirPodsRefreshEvent(void* event);
void RefreshAirPods(std::vector<DeviceStatus>& devices);

// Independent decoder of Apple proximity advertisements. Layout reference:
// SpriteOvO/AirPodsDesktop, Source/Core/AppleCP.h and AppleCP.cpp (GPL-3.0-or-later).
namespace airpods {
struct Battery {
    int left = -1, right = -1, box = -1;  // single-battery models use `left`
    bool leftCharging = false, rightCharging = false, boxCharging = false;
    bool single = false;
    uint16_t model = 0;
};

struct Model { uint16_t id; const wchar_t* name; bool single; };  // single: headphones and neckbands

inline const Model* FindModel(uint16_t id) {
    static const Model kModels[] = {
        { 0x0220, L"AirPods 1", false },
        { 0x0F20, L"AirPods 2", false },
        { 0x1320, L"AirPods 3", false },
        { 0x1920, L"AirPods 4", false },
        { 0x1B20, L"AirPods 4 ANC", false },
        { 0x0E20, L"AirPods Pro", false },
        { 0x1420, L"AirPods Pro 2", false },
        { 0x2420, L"AirPods Pro 2 USB-C", false },
        { 0x2720, L"AirPods Pro 3", false },
        { 0x0A20, L"AirPods Max", true },
        { 0x1F20, L"AirPods Max USB-C", true },
        { 0x2D20, L"AirPods Max 2", true },
        { 0x0320, L"Powerbeats 3", true },
        { 0x0520, L"BeatsX", true },
        { 0x0620, L"Beats Solo 3", true },
        { 0x0920, L"Beats Studio 3", true },
        { 0x0C20, L"Beats Solo Pro", true },
        { 0x0D20, L"Powerbeats 4", true },
        { 0x1020, L"Beats Flex", true },
        { 0x1720, L"Beats Studio Pro", true },
        { 0x2520, L"Beats Solo 4", true },
    };
    for (const auto& m : kModels)
        if (m.id == id) return &m;
    return nullptr;
}

inline const wchar_t* ModelName(uint16_t model) {
    const Model* m = FindModel(model);
    return m ? m->name : nullptr;
}

inline int Percent(unsigned nibble) { return nibble <= 10 ? int(nibble * 10) : -1; }

inline bool DecodeProximity(const uint8_t* data, size_t size, Battery& result) {
    if (size != 27 || data[0] != 0x07 || data[1] != 0x19 || data[2] != 0x01) return false;
    Battery next;
    next.model = (uint16_t(data[3]) << 8) | data[4];
    const Model* model = FindModel(next.model);
    if (!model) return false;
    if (model->single) {
        // The battery is in the low nibble of byte 6; the unused nibbles are 0, not 15.
        unsigned level = data[6] & 0x0F;
        bool charging = (data[7] & 0x10) != 0;
        if (level == 0 && (data[6] >> 4) >= 1 && (data[6] >> 4) <= 10) {
            level = data[6] >> 4;
            charging = (data[7] & 0x20) != 0;
        }
        next.single = true;
        next.left = Percent(level);
        next.leftCharging = charging;
        if (next.left < 0) return false;
        result = next;
        return true;
    }
    // Low nibble and bit 4 describe the broadcasting bud; status bit 5 identifies its side.
    const bool fromLeft = (data[5] & 0x20) != 0;
    const int current = Percent(data[6] & 0x0F), other = Percent(data[6] >> 4);
    next.left = fromLeft ? current : other;
    next.right = fromLeft ? other : current;
    next.box = Percent(data[7] & 0x0F);
    next.leftCharging = next.left >= 0 && (data[7] & (fromLeft ? 0x10 : 0x20)) != 0;
    next.rightCharging = next.right >= 0 && (data[7] & (fromLeft ? 0x20 : 0x10)) != 0;
    next.boxCharging = next.box >= 0 && (data[7] & 0x40) != 0;
    if (next.left < 0 && next.right < 0 && next.box < 0) return false;
    result = next;
    return true;
}

// Apple manufacturer data is a sequence of type-length-value messages; the proximity message (type 07)
// can follow another one.
inline bool Decode(const uint8_t* data, size_t size, Battery& result) {
    for (size_t i = 0; data && i + 1 < size; i += 2 + size_t(data[i + 1])) {
        const size_t length = 2 + size_t(data[i + 1]);
        if (i + length > size) break;
        if (data[i] == 0x07 && DecodeProximity(data + i, length, result)) return true;
    }
    return false;
}
} // namespace airpods

// Passive Rapoo status reports, including the report ID (hidapi/Win32 convention).
// Kept independent of Win32 so captured frames can be checked without a mouse.
namespace Rapoo {

enum class Connection { Unknown, Receiver, Bluetooth, Cable };
enum class Profile { Nordic54L, LegacyVT3Pro };

struct Battery {
    int percent = -1;
    bool charging = false;
    Connection connection = Connection::Unknown;
    std::uint16_t mousePid = 0;
};

inline bool DecodeBattery(const std::uint8_t* data, std::size_t size, Battery& out,
                          Profile profile = Profile::Nordic54L) {
    if (!data || !size) return false;
    Battery result;
    unsigned state = 0, percent = 0;
    if (profile == Profile::Nordic54L && data[0] == 0x07) {
        // Nordic 54L: mode, DPI gear, X/Y DPI, battery state, percentage.
        if (size < 9 || (data[1] & 0x0F) > 2) return false;
        switch (data[1] & 0x0F) {
        case 0: result.connection = Connection::Receiver; break;
        case 1: result.connection = Connection::Bluetooth; break;
        case 2: result.connection = Connection::Cable; break;
        }
        state = data[7];
        percent = data[8];
        if (size >= 15) result.mousePid = (std::uint16_t)(data[13] | (data[14] << 8));
    } else if (profile == Profile::LegacyVT3Pro && data[0] == 0xBB) {
        // VT3 Pro firmware: BB B0 mode DPI-lo DPI-hi battery-state percentage.
        if (size < 7 || data[1] != 0xB0) return false;
        state = data[5];
        percent = data[6];
    } else {
        return false;
    }
    // 0 = invalid/transition. Some older firmware uses 7F instead of a level while charging.
    if (state != 1 && state != 2) return false;
    if (percent > 100 && !(profile == Profile::LegacyVT3Pro && state == 2 && percent == 0x7F)) return false;
    result.charging = state == 2;
    if (percent <= 100) result.percent = (int)percent;
    out = result;
    return true;
}

}  // namespace Rapoo

namespace ModConfig {
inline std::atomic<int> pollMs{60000}, warning{15}, fontSize{13}, gap{14};
inline std::atomic<bool> taskbar{true}, hideSleeping{false}, airpodsEnabled{false};
inline void* bleEvent = nullptr;
inline std::mutex ignoredMutex;
inline std::set<std::pair<USHORT, USHORT>> ignored;  // vendor and product ids, guarded by ignoredMutex
inline void Load() {
    pollMs = std::clamp(Wh_GetIntSetting(L"pollSeconds"), 10, 600) * 1000;
    warning = std::clamp(Wh_GetIntSetting(L"warningPercent"), 0, 50);
    fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 10, 24);
    gap = std::clamp(Wh_GetIntSetting(L"deviceGap"), 4, 40);
    hideSleeping = !!Wh_GetIntSetting(L"hideSleeping");
    airpodsEnabled = !!Wh_GetIntSetting(L"enableAirPods");
    auto mode = WindhawkUtils::StringSetting::make(L"displayMode");
    taskbar = wcscmp(mode, L"tray") != 0;
    std::set<std::pair<USHORT, USHORT>> devices;
    for (int i = 0;; i++) {
        auto entry = WindhawkUtils::StringSetting::make(L"ignoredDevices[%d]", i);
        if (!*entry.get()) break;
        unsigned vid = 0, pid = 0;
        wchar_t extra;  // matched only when there are trailing characters
        if (swscanf(entry.get(), L" %x : %x %lc", &vid, &pid, &extra) == 2 && vid <= 0xFFFF && pid <= 0xFFFF)
            devices.insert({ (USHORT)vid, (USHORT)pid });
        else
            Wh_Log(L"Invalid device to exclude: %s", entry.get());
    }
    std::lock_guard<std::mutex> lock(ignoredMutex);
    ignored = std::move(devices);
}
inline bool IsIgnored(USHORT vid, USHORT pid) {
    std::lock_guard<std::mutex> lock(ignoredMutex);
    return ignored.count({ vid, pid }) != 0;
}
}
namespace ModHost {
extern std::atomic<bool> stopRequested;
extern std::atomic<HWND> window;
// Single-instance mutexes of the standalone apps: upstream BatteryHub and the Chinese adaptation.
inline const wchar_t* const kStandaloneMutexes[] = { L"Local\\WirelessBatteryTray", L"Local\\TaskbarWirelessBatteryTray" };
}

std::vector<HidColl> EnumHid() {
    std::vector<HidColl> out;
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO di = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (di == INVALID_HANDLE_VALUE) return out;
    SP_DEVICE_INTERFACE_DATA ifd{ sizeof(ifd) };
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(di, nullptr, &hidGuid, i, &ifd); i++) {
        DWORD need = 0;
        SetupDiGetDeviceInterfaceDetailW(di, &ifd, nullptr, 0, &need, nullptr);
        if (!need) continue;
        std::vector<BYTE> buf(need);
        auto det = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buf.data());
        det->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (!SetupDiGetDeviceInterfaceDetailW(di, &ifd, det, need, nullptr, nullptr)) continue;
        HANDLE h = CreateFileW(det->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE) continue;
        HidColl c;
        c.path = det->DevicePath;
        HIDD_ATTRIBUTES a{ sizeof(a) };
        if (HidD_GetAttributes(h, &a)) { c.vid = a.VendorID; c.pid = a.ProductID; }
        PHIDP_PREPARSED_DATA pp;
        if (HidD_GetPreparsedData(h, &pp)) { HidP_GetCaps(pp, &c.caps); HidD_FreePreparsedData(pp); }
        CloseHandle(h);
        std::wstring lower = c.path;
        for (auto& ch : lower) ch = towlower(ch);
        size_t mi = lower.find(L"&mi_");
        if (mi != std::wstring::npos) c.iface = (int)wcstol(lower.c_str() + mi + 4, nullptr, 16);
        out.push_back(std::move(c));
    }
    SetupDiDestroyDeviceInfoList(di);
    return out;
}

// ---- HidDev -----------------------------------------------------------------------------------

bool HidDev::Open(const HidColl& c) {
    Close();
    c_ = c;
    h_ = CreateFileW(c.path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                     FILE_FLAG_OVERLAPPED, nullptr);
    overlapped_ = true;
    return ok();
}

bool HidDev::OpenReadOnly(const HidColl& c) {
    Close();
    c_ = c;
    h_ = CreateFileW(c.path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
    overlapped_ = true;
    return ok();
}

bool HidDev::OpenFeatureOnly(const HidColl& c) {
    Close();
    c_ = c;
    h_ = CreateFileW(c.path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    overlapped_ = false;
    return ok();
}

void HidDev::Close() {
    if (h_ != INVALID_HANDLE_VALUE) CloseHandle(h_);
    h_ = INVALID_HANDLE_VALUE;
}

bool HidDev::Write(const std::vector<BYTE>& data, int timeoutMs) {
    if (!ok() || !overlapped_ || !c_.outLen() || data.empty()) return false;
    std::vector<BYTE> buf(c_.outLen(), 0);
    memcpy(buf.data(), data.data(), std::min(buf.size(), data.size()));
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD n = 0;
    bool done = false;
    if (WriteFile(h_, buf.data(), (DWORD)buf.size(), nullptr, &ov) || GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(ov.hEvent, (DWORD)timeoutMs) == WAIT_OBJECT_0) {
            done = GetOverlappedResult(h_, &ov, &n, FALSE) != FALSE;
        } else {
            CancelIo(h_);
            done = GetOverlappedResult(h_, &ov, &n, TRUE) != FALSE;
        }
    }
    CloseHandle(ov.hEvent);
    return done;
}

int HidDev::Read(std::vector<BYTE>& buf, int timeoutMs) {
    if (!ok() || !overlapped_ || !c_.inLen()) return -1;
    std::vector<BYTE> tmp(c_.inLen(), 0);
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD got = 0;
    bool done = false;
    if (ReadFile(h_, tmp.data(), (DWORD)tmp.size(), nullptr, &ov) || GetLastError() == ERROR_IO_PENDING) {
        if (timeoutMs > 0 && WaitForSingleObject(ov.hEvent, (DWORD)timeoutMs) == WAIT_OBJECT_0) {
            done = GetOverlappedResult(h_, &ov, &got, FALSE) != FALSE;
        } else {
            CancelIo(h_);
            done = GetOverlappedResult(h_, &ov, &got, TRUE) != FALSE;  // it may have completed meanwhile
        }
    }
    CloseHandle(ov.hEvent);
    if (!done || got == 0) return -1;
    size_t off = tmp[0] == 0 ? 1 : 0;  // hidapi semantics: drop the "no report ID" byte
    buf.assign(tmp.begin() + off, tmp.begin() + got);
    return (int)buf.size();
}

int HidDev::SetOutputAndRead(const std::vector<BYTE>& data, std::vector<BYTE>& buf, int timeoutMs) {
    if (!ok() || !overlapped_ || !c_.inLen() || !c_.outLen() || data.empty()) return -1;
    std::vector<BYTE> out(c_.outLen(), 0);
    memcpy(out.data(), data.data(), std::min(out.size(), data.size()));
    std::vector<BYTE> tmp(c_.inLen(), 0);
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD got = 0;
    bool done = false;
    if (ReadFile(h_, tmp.data(), (DWORD)tmp.size(), nullptr, &ov) || GetLastError() == ERROR_IO_PENDING) {
        bool sent = HidD_SetOutputReport(h_, out.data(), (ULONG)out.size()) != FALSE;
        if (sent && timeoutMs > 0 && WaitForSingleObject(ov.hEvent, (DWORD)timeoutMs) == WAIT_OBJECT_0) {
            done = GetOverlappedResult(h_, &ov, &got, FALSE) != FALSE;
        } else {
            CancelIo(h_);
            done = GetOverlappedResult(h_, &ov, &got, TRUE) != FALSE && sent;
        }
    }
    CloseHandle(ov.hEvent);
    if (!done || got == 0) return -1;
    size_t off = tmp[0] == 0 ? 1 : 0;
    buf.assign(tmp.begin() + off, tmp.begin() + got);
    return (int)buf.size();
}

void HidDev::Flush() {
    if (ok()) HidD_FlushQueue(h_);
}

bool HidDev::SetFeature(const std::vector<BYTE>& data) {
    if (!ok() || !c_.featLen()) return false;
    std::vector<BYTE> buf(c_.featLen(), 0);
    memcpy(buf.data(), data.data(), std::min(buf.size(), data.size()));
    return HidD_SetFeature(h_, buf.data(), (ULONG)buf.size()) != FALSE;
}

bool HidDev::GetFeature(BYTE reportId, std::vector<BYTE>& out) {
    if (!ok() || !c_.featLen()) return false;
    out.assign(c_.featLen(), 0);
    out[0] = reportId;
    return HidD_GetFeature(h_, out.data(), (ULONG)out.size()) != FALSE;
}

bool HidDev::GetInputReport(BYTE reportId, std::vector<BYTE>& out) {
    if (!ok() || !c_.inLen()) return false;
    out.assign(c_.inLen(), 0);
    out[0] = reportId;
    return HidD_GetInputReport(h_, out.data(), (ULONG)out.size()) != FALSE;
}

bool HidDev::GetFeatureByte(BYTE reportId, BYTE& value) {
    if (!ok() || !overlapped_ || !reportId || c_.featLen() < 2) return false;
    BYTE report[] = { reportId, 0xFF };
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) return false;
    DWORD received = 0;
    BOOL done = DeviceIoControl(h_, IOCTL_HID_GET_FEATURE, report, sizeof(report), report, sizeof(report), &received, &ov);
    if (!done && GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(ov.hEvent, 200) != WAIT_OBJECT_0) CancelIoEx(h_, &ov);
        done = GetOverlappedResult(h_, &ov, &received, TRUE);
    }
    CloseHandle(ov.hEvent);
    if (!done || received != sizeof(report) || report[0] != reportId) return false;
    value = report[1];
    return true;
}

// ---- helpers ----------------------------------------------------------------------------------

std::wstring HidProduct(const HidColl& c) {
    HANDLE h = CreateFileW(c.path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return L"";
    wchar_t name[128] = {};
    if (!HidD_GetProductString(h, name, sizeof(name) - sizeof(wchar_t))) name[0] = 0;
    CloseHandle(h);
    std::wstring s = name;
    while (!s.empty() && iswspace(s.back())) s.pop_back();
    return s;
}

std::wstring Hex4(USHORT v) {
    wchar_t b[8];
    swprintf(b, 8, L"%04X", v);
    return b;
}

int Clamp100(int v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }

int MapTo100(int v, int lo, int hi) {
    if (hi <= lo) return Clamp100(v);
    return Clamp100((v - lo) * 100 / (hi - lo));
}

int Interpolate(int mv, const int* mvTable, const int* pctTable, int n) {
    if (mv >= mvTable[0]) return pctTable[0];
    for (int i = 1; i < n; i++) {
        if (mv >= mvTable[i]) {
            int span = mvTable[i - 1] - mvTable[i];
            return pctTable[i] + (pctTable[i - 1] - pctTable[i]) * (mv - mvTable[i]) / (span ? span : 1);
        }
    }
    return pctTable[n - 1];
}

DeviceKind GuessKind(const std::wstring& name) {
    std::wstring n = name;
    for (auto& ch : n) ch = towlower(ch);
    auto has = [&](const wchar_t* s) { return n.find(s) != std::wstring::npos; };
    if (has(L"mouse") || has(L"mice") || has(L"trackball")) return DeviceKind::Mouse;
    if (has(L"keyboard") || has(L"keys")) return DeviceKind::Keyboard;
    if (has(L"controller") || has(L"gamepad") || has(L"xbox") || has(L"dualsense") || has(L"dualshock") || has(L"joy-con"))
        return DeviceKind::Gamepad;
    if (has(L"head") || has(L"buds") || has(L"pods") || has(L"ear") || has(L"wh-") || has(L"wf-") || has(L"audio") ||
        has(L"speaker") || has(L"arctis") || has(L"cloud") || has(L"void") || has(L"virtuoso"))
        return DeviceKind::Headset;
    return DeviceKind::Other;
}

DeviceKind KindFromCollections(const std::vector<HidColl>& all, USHORT vid, USHORT pid) {
    bool mouse = false, pad = false, kbd = false;
    for (const auto& c : all) {
        if (c.vid != vid || c.pid != pid || c.page() != 0x01) continue;
        mouse |= c.usage() == 0x02;
        pad |= c.usage() == 0x04 || c.usage() == 0x05;
        kbd |= c.usage() == 0x06;
    }
    if (mouse) return DeviceKind::Mouse;
    if (pad) return DeviceKind::Gamepad;
    if (kbd) return DeviceKind::Keyboard;
    return DeviceKind::Other;
}

namespace wb_driver_airpods {
namespace {
using namespace WindhawkAirPodsAbi;

template<class T> struct Com {
    T* p = nullptr;
    ~Com() { if (p) p->Release(); }
    T** out() { return &p; }
    T* operator->() const { return p; }
};

struct Seen { airpods::Battery battery; ULONGLONG when = 0; INT16 rssi = -127; };
struct State {
    std::mutex mutex;
    std::map<UINT64, Seen> seen;
    HANDLE refresh = nullptr;
    bool pending = false;
    ULONGLONG signalled = 0;
    unsigned events = 0, apple = 0, decoded = 0;
};

class Handler final : public Received {
    std::atomic<ULONG> refs{1};
    std::shared_ptr<State> state;
public:
    explicit Handler(std::shared_ptr<State> s) : state(std::move(s)) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(Received) || iid == __uuidof(IAgileObject)) {
            *out = static_cast<Received*>(this); AddRef(); return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs; }
    ULONG STDMETHODCALLTYPE Release() override { ULONG n = --refs; if (!n) delete this; return n; }
    HRESULT STDMETHODCALLTYPE Invoke(IBluetoothLEAdvertisementWatcher*, IBluetoothLEAdvertisementReceivedEventArgs* args) override {
        Com<IBluetoothLEAdvertisement> ad;
        Com<DataList> list;
        if (FAILED(args->get_Advertisement(ad.out())) || !ad.p ||
            FAILED(ad->GetManufacturerDataByCompanyId(0x004C, list.out())) || !list.p) return S_OK;
        UINT32 count = 0; list->get_Size(&count);
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            ++state->events; if (count) ++state->apple;
        }
        for (UINT32 i = 0; i < count; ++i) {
            Com<IBluetoothLEManufacturerData> item;
            Com<ABI::Windows::Storage::Streams::IBuffer> buffer;
            Com<::Windows::Storage::Streams::IBufferByteAccess> access;
            if (FAILED(list->GetAt(i, item.out())) || !item.p || FAILED(item->get_Data(buffer.out())) || !buffer.p) continue;
            UINT32 length = 0;
            if (FAILED(buffer->get_Length(&length)) || length < 27 ||
                FAILED(buffer->QueryInterface(__uuidof(::Windows::Storage::Streams::IBufferByteAccess), (void**)access.out()))) continue;
            BYTE* bytes = nullptr;
            if (FAILED(access->Buffer(&bytes))) continue;
            airpods::Battery battery;
            if (!airpods::Decode(bytes, length, battery)) continue;
            UINT64 address = 0; INT16 rssi = -127;
            if (FAILED(args->get_BluetoothAddress(&address)) || FAILED(args->get_RawSignalStrengthInDBm(&rssi)) || rssi < -85) continue;
            const auto now = GetTickCount64();
            std::lock_guard<std::mutex> lock(state->mutex);
            ++state->decoded;
            for (auto it = state->seen.begin(); it != state->seen.end();)
                if (now - it->second.when > 120000) it = state->seen.erase(it); else ++it;
            if (state->seen.size() >= 32 && !state->seen.count(address)) continue;
            auto previous = state->seen.find(address);
            bool changed = previous == state->seen.end();
            if (!changed) {
                const auto& old = previous->second.battery;
                changed = old.model != battery.model || old.left != battery.left || old.right != battery.right ||
                    old.box != battery.box || old.leftCharging != battery.leftCharging ||
                    old.rightCharging != battery.rightCharging || old.boxCharging != battery.boxCharging ||
                    now - previous->second.when > 30000;
            }
            state->seen[address] = {battery, now, rssi};
            state->pending = state->pending || changed;
            // One UI refresh at most every 3 seconds; no disk logs or raw packet retention.
            if (state->pending && state->refresh && now - state->signalled >= 3000) {
                SetEvent(state->refresh); state->signalled = now; state->pending = false;
            }
        }
        return S_OK;
    }
};

struct Receiver {
    std::shared_ptr<State> state = std::make_shared<State>();
    IBluetoothLEAdvertisementWatcher* watcher = nullptr;
    EventRegistrationToken token{};
    HRESULT error = S_OK;
    ULONGLONG attempted = 0;
    bool initialized = false;

    void stop() {
        if (watcher) {
            watcher->Stop(); watcher->remove_Received(token); watcher->Release(); watcher = nullptr;
        }
        if (initialized) { RoUninitialize(); initialized = false; }
    }
    void start() {
        if (watcher) {
            BluetoothLEAdvertisementWatcherStatus status;
            if (SUCCEEDED(watcher->get_Status(&status)) && status == BluetoothLEAdvertisementWatcherStatus_Started) return;
            stop();
        }
        const auto now = GetTickCount64();
        if (attempted && now - attempted < 10000) return;
        attempted = now;
        error = RoInitialize(RO_INIT_MULTITHREADED);
        if (FAILED(error)) return;
        initialized = true;
        const wchar_t* cls = L"Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher";
        HSTRING name = nullptr; IInspectable* instance = nullptr;
        error = WindowsCreateString(cls, (UINT32)wcslen(cls), &name);
        if (SUCCEEDED(error)) error = RoActivateInstance(name, &instance);
        if (name) WindowsDeleteString(name);
        if (SUCCEEDED(error)) {
            error = instance->QueryInterface(__uuidof(IBluetoothLEAdvertisementWatcher), (void**)&watcher);
            instance->Release();
        }
        if (FAILED(error)) { stop(); return; }
        error = watcher->put_ScanningMode(BluetoothLEScanningMode_Passive);
        auto* handler = new Handler(state);
        if (SUCCEEDED(error)) error = watcher->add_Received(handler, &token);
        handler->Release();
        if (SUCCEEDED(error)) error = watcher->Start();
        if (FAILED(error)) stop();
    }
};

Receiver& GetReceiver() { static auto* r = new Receiver; return *r; }

std::wstring Level(int pct, bool charging) {
    if (pct < 0) return L"Unknown";
    return L"~" + std::to_wstring(pct) + L"%" + (charging ? L" (charging)" : L"");
}

struct Paired { std::wstring name; bool connected = false; };

// Apple audio models paired with this PC, by advertisement model id. Their Bluetooth service nodes
// carry the model as the PID: "BTHENUM\{uuid}_VID&0001004C_PID&2014\..." (AirPods Pro 2).
std::map<uint16_t, Paired> PairedAppleModels() {
    static const DEVPROPKEY kIsConnected = { { 0x83DA6326, 0x97A6, 0x4088, { 0x94, 0x53, 0xA1, 0x92, 0x3F, 0x57, 0x3B, 0x29 } }, 15 };
    struct Node { std::wstring name; bool connected = false; uint16_t model = 0; };
    std::map<std::wstring, Node> byContainer;
    HDEVINFO di = SetupDiGetClassDevsW(nullptr, L"BTHENUM", nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (di == INVALID_HANDLE_VALUE) return {};
    SP_DEVINFO_DATA dd{ sizeof(dd) };
    for (DWORD i = 0; SetupDiEnumDeviceInfo(di, i, &dd); i++) {
        wchar_t inst[512];
        if (!SetupDiGetDeviceInstanceIdW(di, &dd, inst, 512, nullptr)) continue;
        GUID container{};
        DEVPROPTYPE t;
        if (!SetupDiGetDevicePropertyW(di, &dd, &DEVPKEY_Device_ContainerId, &t, (PBYTE)&container, sizeof(container), nullptr, 0)) continue;
        wchar_t g[64];
        StringFromGUID2(container, g, 64);
        Node& n = byContainer[g];
        std::wstring id = inst;
        for (auto& ch : id) ch = towupper(ch);
        size_t v = id.find(L"_VID&0001004C_PID&");
        if (v != std::wstring::npos) {
            unsigned pid = wcstoul(id.c_str() + v + 18, nullptr, 16);
            n.model = (uint16_t)(((pid & 0xFF) << 8) | ((pid >> 8) & 0xFF));  // advertisement byte order
        }
        if (!wcsncmp(inst, L"BTHENUM\\DEV_", 12)) {
            wchar_t buf[256] = {};
            if (SetupDiGetDevicePropertyW(di, &dd, &DEVPKEY_Device_FriendlyName, &t, (PBYTE)buf, sizeof(buf) - sizeof(wchar_t), nullptr, 0) &&
                t == DEVPROP_TYPE_STRING)
                n.name = buf;
            DEVPROP_BOOLEAN conn = DEVPROP_FALSE;
            if (SetupDiGetDevicePropertyW(di, &dd, &kIsConnected, &t, (PBYTE)&conn, sizeof(conn), nullptr, 0) && t == DEVPROP_TYPE_BOOLEAN)
                n.connected = conn != DEVPROP_FALSE;
        }
    }
    SetupDiDestroyDeviceInfoList(di);
    std::map<uint16_t, Paired> out;
    for (const auto& [cid, n] : byContainer) {
        if (!n.model || !airpods::FindModel(n.model)) continue;
        auto it = out.find(n.model);
        if (it == out.end() || (n.connected && !it->second.connected)) out[n.model] = { n.name, n.connected };
    }
    return out;
}

std::map<uint16_t, Paired> g_paired;  // worker thread only, refreshed by every full poll
} // namespace

void SetAirPodsRefreshEvent(void* event) {
    auto s = GetReceiver().state;
    std::lock_guard<std::mutex> lock(s->mutex); s->refresh = (HANDLE)event;
}

void StartAirPods() { GetReceiver().start(); }
void StopAirPods() {
    SetAirPodsRefreshEvent(nullptr);
    GetReceiver().stop();
}

void PollAirPods(PollContext& ctx, bool refreshPaired) {
    StartAirPods();
    if (refreshPaired) g_paired = PairedAppleModels();
    if (g_paired.empty()) return;
    auto s = GetReceiver().state;
    std::lock_guard<std::mutex> lock(s->mutex);
    const auto now = GetTickCount64();
    // Rotating BLE addresses cannot prove ownership. Only models paired with this PC are used, with
    // the strongest recent broadcast of each, labelled as unverified (it can come from someone else's
    // AirPods of the same model).
    std::map<uint16_t, Seen> strongest;
    for (const auto& [address, seen] : s->seen) {
        if (now - seen.when > 120000 || !g_paired.count(seen.battery.model)) continue;
        auto it = strongest.find(seen.battery.model);
        if (it == strongest.end() || seen.rssi > it->second.rssi || now - it->second.when > 15000)
            strongest[seen.battery.model] = seen;
    }
    for (const auto& [model, seen] : strongest) {
        const auto& b = seen.battery;
        const Paired& paired = g_paired[model];
        DeviceStatus status;
        status.key = L"airpods-nearby:" + Hex4(model);
        status.name = paired.name.empty() ? airpods::ModelName(model) : paired.name;
        status.kind = DeviceKind::Headset;
        status.via = L"Apple BLE";
        bool onlyBox = false;
        if (b.single) {
            status.percent = b.left;
            status.charging = b.leftCharging;
            status.detailText = L"Nearby broadcast, ownership unverified";
        } else {
            // The lower discharging bud is the one in use; charging only when every bud with a reading is.
            int discharging = -1, charging = -1;
            for (auto [pct, chg] : { std::pair{ b.left, b.leftCharging }, std::pair{ b.right, b.rightCharging } }) {
                int& lowest = chg ? charging : discharging;
                if (pct >= 0 && (lowest < 0 || pct < lowest)) lowest = pct;
            }
            onlyBox = discharging < 0 && charging < 0;
            status.percent = discharging >= 0 ? discharging : charging >= 0 ? charging : b.box;
            status.charging = discharging < 0 && (charging >= 0 || b.boxCharging);
            status.detailText = L"Left: " + Level(b.left, b.leftCharging) + L"; right: " + Level(b.right, b.rightCharging) +
                L"; case: " + Level(b.box, b.boxCharging) + L"; nearby broadcast, ownership unverified";
        }
        status.levelText = (onlyBox ? L"Case ~" : L"~") + std::to_wstring(status.percent) + L"%";
        if (now - seen.when > 30000) {
            status.asleep = true; status.percent = -1;
            status.levelText.clear(); status.detailText += L"; last known";
        }
        ctx.out.push_back(status);
    }
}

void RefreshAirPods(std::vector<DeviceStatus>& devices) {
    devices.erase(std::remove_if(devices.begin(), devices.end(), [](const DeviceStatus& s) {
        return s.key.compare(0, 15, L"airpods-nearby:") == 0;
    }), devices.end());
    PollContext ctx;
    wb_driver_airpods::PollAirPods(ctx, false);
    devices.insert(devices.end(), ctx.out.begin(), ctx.out.end());
}

}

void PollAirPods(PollContext& ctx) { if (ModConfig::airpodsEnabled) wb_driver_airpods::PollAirPods(ctx, true); }

void StartAirPods() {
    if (ModConfig::airpodsEnabled) {
        wb_driver_airpods::SetAirPodsRefreshEvent(ModConfig::bleEvent); wb_driver_airpods::StartAirPods();
    } else { wb_driver_airpods::StopAirPods(); }
}
void StopAirPods() { wb_driver_airpods::StopAirPods(); }
void SetAirPodsRefreshEvent(void* e) { ModConfig::bleEvent = e; wb_driver_airpods::SetAirPodsRefreshEvent(e); }
void RefreshAirPods(std::vector<DeviceStatus>& v) {
    if (ModConfig::airpodsEnabled) wb_driver_airpods::RefreshAirPods(v);
    else {
        wb_driver_airpods::StopAirPods();
        v.erase(std::remove_if(v.begin(), v.end(), [](const DeviceStatus& d) { return d.key.rfind(L"airpods-nearby:", 0) == 0; }), v.end());
    }
}

namespace wb_driver_akg {
// AKG N9 Hybrid on its USB-C dongle (0ECB:20D9, Harman vendor id).
// The dongle is an Airoha part and speaks Airoha's RACE protocol on the vendor collection FF13:0001
// (interface 3), wrapped the same way as the Audeze Maxwell:
//   request:  62-byte output report 06: [06][07][80][05 5A 03 00 D6 0C 00]
//             = RACE command (05 5A), length 3, id 0x0CD6 (battery), agent 00
//   answer:   62-byte input report 07, fetched with GetInputReport: [07][valid length][80] followed by recent
//             RACE frames; the battery reply is [05 5D 05 00 D6 0C][status 00][agent][percent].
//             With the headset off there is no battery reply in the valid part (only notifications 2C80 / 2CB1).
// Checked on an AKG N9 Hybrid playing music on the dongle: 05 5D 05 00 D6 0C 00 00 32 = 50 %, the level the
// AKG Headphones app showed at the same time. The dongle also acknowledges with 05 5B ... D6 0C 00 and keeps
// sending other RACE notifications (ids 0901, 2C80, 2C82, 2CB1) that are not needed here.
// Only this single read-only query is sent. No charging flag is known.
namespace {

constexpr USHORT kVid = 0x0ECB, kN9HybridDongle = 0x20D9;

// The newest battery reply in a report 07 buffer, or -1. Byte 1 is the number of valid bytes from byte 3 on;
// the rest of the report is left over from earlier answers. With the headset off the dongle stops answering
// the query, but an old battery frame can still sit in that leftover tail, so it must not be read.
int FindLevel(const std::vector<BYTE>& b) {
    if (b.size() < 4) return -1;
    size_t end = (std::min)(b.size(), (size_t)3 + b[1]);
    int level = -1;
    for (size_t i = 3; i + 8 < end; i++) {
        if (b[i] != 0x05 || b[i + 1] != 0x5D || b[i + 2] != 0x05 || b[i + 3] != 0x00) continue;
        if (b[i + 4] != 0xD6 || b[i + 5] != 0x0C || b[i + 6] != 0x00) continue;  // id 0x0CD6, status ok
        if (b[i + 8] <= 100) level = b[i + 8];
    }
    return level;
}

int ReadLevel(const HidColl& c) {
    HidDev d;
    if (!d.Open(c)) return -1;
    std::vector<BYTE> req(c.outLen(), 0);
    const BYTE q[] = { 0x06, 0x07, 0x80, 0x05, 0x5A, 0x03, 0x00, 0xD6, 0x0C, 0x00 };
    std::copy(std::begin(q), std::end(q), req.begin());
    if (!d.Write(req)) return -1;
    int level = -1;
    ULONGLONG end = GetTickCount64() + 600;  // the reply shows up within ~100 ms while the headset is on
    while (GetTickCount64() < end) {
        Sleep(15);
        std::vector<BYTE> r;
        if (!d.GetInputReport(0x07, r)) continue;
        int l = FindLevel(r);
        if (l >= 0) level = l;
        if (level >= 0) break;
    }
    return level;
}

}  // namespace

void PollAkg(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kN9HybridDongle)) return;
    for (const HidColl* c : ctx.Find(kVid, kN9HybridDongle, [](const HidColl& c) {
             return c.iface == 3 && c.page() == 0xFF13 && c.usage() == 0x0001 && c.inLen() == 62 && c.outLen() == 62;
         })) {
        int level = ReadLevel(*c);
        if (level < 0) continue;
        DeviceStatus s;
        s.key = L"akg:" + Hex4(kN9HybridDongle);
        s.name = L"AKG N9 Hybrid";
        s.kind = DeviceKind::Headset;
        s.via = L"USB dongle";
        s.percent = level;
        ctx.Add(s, kVid, kN9HybridDongle);
        break;
    }
}

}

void PollAkg(PollContext& ctx) { wb_driver_akg::PollAkg(ctx); }
namespace wb_driver_akko {
// Akko V9 Dash Pro (38EE:000C) and Akko Dash V9 Ultra (38EE:000F, "Akko 8K Receiver") over 2.4 GHz.
// Uses the same 64-byte vendor channel on usage page FF1C, usage 0092 as Evision / CHERRY / ATK devices
// (protocol originally from B1Chat/BatteryWatch, MIT; verified with both receivers):
//   frame:  [0] report ID 4, [1..2] checksum (LE) = sum(bytes[3..end]) & 0xFFFF, [3] type, [4..] payload
//   query:  type 0x1A, payload 03 00 00 55 (read-only status request)
//   reply:  [0] 4, [3] 0x1A, [8] battery percent (0..100), [9] non-zero while charging
namespace {

constexpr USHORT kVid = 0x38EE;
constexpr USHORT kPage = 0xFF1C;
constexpr USHORT kUsage = 0x0092;
constexpr size_t kLen = 64;
constexpr BYTE kReportId = 0x04;
constexpr BYTE kStatusType = 0x1A;

struct Model {
    USHORT pid;
    const wchar_t* name;
    const wchar_t* key;
};

constexpr Model kModels[] = {
    { 0x000C, L"Akko V9 Dash Pro", L"akko:38ee:000c" },
    { 0x000F, L"Akko Dash V9 Ultra", L"akko:38ee:000f" },
};

std::vector<BYTE> BuildQuery() {
    std::vector<BYTE> f(kLen, 0);
    f[0] = kReportId;
    f[3] = kStatusType;
    f[4] = 0x03;
    f[7] = 0x55;
    unsigned sum = 0;
    for (size_t i = 3; i < f.size(); ++i) sum += f[i];
    f[1] = (BYTE)(sum & 0xFF);
    f[2] = (BYTE)((sum >> 8) & 0xFF);
    return f;
}

bool IsStatusReply(const std::vector<BYTE>& r) {
    return r.size() >= 10 && r[0] == kReportId && r[3] == kStatusType && r[8] <= 100;
}

bool Query(const HidColl& c, std::vector<BYTE>& reply) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    const std::vector<BYTE> query = BuildQuery();
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (!d.Write(query, 300)) return false;
        const ULONGLONG end = GetTickCount64() + 400;
        for (;;) {
            const LONGLONG left = (LONGLONG)end - (LONGLONG)GetTickCount64();
            if (left <= 0) break;
            std::vector<BYTE> r;
            if (d.Read(r, (int)left) < 0) break;
            if (IsStatusReply(r)) {
                reply = std::move(r);
                return true;
            }
        }
    }
    return false;
}

}  // namespace

void PollAkko(PollContext& ctx) {
    for (const Model& model : kModels) {
        if (ctx.IsClaimed(kVid, model.pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, model.pid, [](const HidColl& c) {
                 return c.page() == kPage && c.usage() == kUsage && c.inLen() == kLen && c.outLen() == kLen;
             })) {
            std::vector<BYTE> reply;
            if (!Query(*c, reply)) continue;

            DeviceStatus s;
            s.key = model.key;
            s.name = model.name;
            s.kind = DeviceKind::Mouse;
            s.via = L"2.4 GHz receiver";
            s.percent = reply[8];
            s.charging = reply[9] != 0;
            s.graceSec = 1800;
            ctx.Add(s, kVid, model.pid);
            break;
        }
    }
}

}

void PollAkko(PollContext& ctx) { wb_driver_akko::PollAkko(ctx); }
namespace wb_driver_alienware {
// Alienware AW920H Tri-Mode Wireless Gaming Headset on its 2.4 GHz dongle (413C:A515).
// Protocol from Alienware Command Center: the packet log its AlienFX SubAgent writes and the
// GetRFBatteryStatus / IsChecksumOk methods of its AW920H plugin (HeadsetDevice.cs in
// AW920H_VID_413C_PID_A515.dll; it sends the same query once a minute). Checked on an AW920H:
//  * Collection: interface 5, usage page FF13:0001; output report 06 / input report 07, 62 bytes each.
//  * Request: HidD_SetOutputReport [06][C0][0A][00 ...].
//  * Reply, read with HidD_GetInputReport 07 every ~15 ms: [07][C0][0A][09][00][linked][percent]
//    [charging][critical][..][..][critical percent][charge mode][checksum]:
//    [13] = XOR of bytes 3..12, and Command Center also rejects a reply whose [6] is 0.
//    [7] = 1 while charging ([12] = 0 normal, 1 / 2 quick charge). [8] = 1 when the battery is
//    critical; the level is then taken from [11] instead of [6].
//    [07][00][80] means "no answer yet"; the dongle answers after 2-3 reads (30-100 ms) while the
//    headset is on, and keeps answering 00 80 while it is off.
//    Example: 07 C0 0A 09 00 01 5F 00 00 1C 10 5F 02 06 = 95%, not charging.
//  * About 1 query in 100 is dropped and never answered (measured: none of those answered within 3 s),
//    so the query is resent; Command Center likewise retries. A headset that still does not answer is
//    left out of the results so the tray keeps its last level for graceSec instead of flickering.
// In Bluetooth mode (413C:A514) Windows keeps the level itself, so PollBluetooth covers it.
namespace {
constexpr USHORT kVendor = 0x413C, kAw920hDongle = 0xA515;
constexpr BYTE kRequestId = 0x06, kReplyId = 0x07;
constexpr BYTE kBatteryCmd0 = 0xC0, kBatteryCmd1 = 0x0A;
constexpr int kAttempts = 3;
constexpr int kAttemptMs = 180;     // a normal reply takes 30-100 ms
constexpr int kTotalTimeoutMs = 600;

bool IsVendorCollection(const HidColl& c) {
    return c.iface == 5 && c.page() == 0xFF13 && c.usage() == 0x0001 && c.inLen() == 62 && c.outLen() == 62;
}

struct Battery {
    int percent = -1;  // -1: no valid reply (headset off or out of range)
    bool charging = false;
};

// Decodes a battery reply the way Command Center does; false when `r` is not a valid one.
bool DecodeBattery(const std::vector<BYTE>& r, Battery& b) {
    if (r.size() < 14 || r[0] != kReplyId || r[1] != kBatteryCmd0 || r[2] != kBatteryCmd1 || r[3] != 0x09) return false;
    BYTE x = 0;
    for (int i = 3; i <= 12; i++) x ^= r[i];
    if (x != r[13] || r[6] == 0) return false;
    int pct = r[8] == 1 ? r[11] : r[6];
    if (pct > 100) return false;
    b.percent = pct;
    b.charging = r[7] == 1;
    return true;
}

Battery QueryBattery(HidDev& d) {
    std::vector<BYTE> req(62, 0);
    req[0] = kRequestId;
    req[1] = kBatteryCmd0;
    req[2] = kBatteryCmd1;
    // Sends the output report the way Command Center does (HidD_SetOutputReport). The reply is
    // normally fetched over the control pipe below, but take it if it also arrives as an interrupt report.
    std::vector<BYTE> r;
    Battery b;
    ULONGLONG deadline = GetTickCount64() + kTotalTimeoutMs;
    for (int attempt = 0; attempt < kAttempts && GetTickCount64() < deadline; attempt++) {
        if (d.SetOutputAndRead(req, r, 20) > 0 && DecodeBattery(r, b)) return b;
        ULONGLONG attemptEnd = (std::min)(deadline, GetTickCount64() + kAttemptMs);
        do {
            Sleep(15);
            if (d.GetInputReport(kReplyId, r) && DecodeBattery(r, b)) return b;
        } while (GetTickCount64() < attemptEnd);
    }
    return Battery{};
}
}  // namespace

void PollAlienware(PollContext& ctx) {
    if (ctx.IsClaimed(kVendor, kAw920hDongle)) return;
    for (const HidColl* c : ctx.Find(kVendor, kAw920hDongle, IsVendorCollection)) {
        HidDev d;
        if (!d.Open(*c)) continue;
        Battery b = QueryBattery(d);
        if (b.percent < 0) {
            // Headset off, out of range, or every query dropped: leave it out (the tray keeps the last
            // level for graceSec, then removes it) and keep the generic fallbacks off this dongle.
            ctx.Claim(kVendor, kAw920hDongle);
            break;
        }
        DeviceStatus s;
        s.key = L"alienware:" + Hex4(kAw920hDongle);
        s.name = L"Alienware AW920H";
        s.kind = DeviceKind::Headset;
        s.via = L"2.4 GHz dongle";
        s.percent = b.percent;
        s.charging = b.charging;
        s.graceSec = 180;
        ctx.Add(s, kVendor, kAw920hDongle);
        break;
    }
}

}

void PollAlienware(PollContext& ctx) { wb_driver_alienware::PollAlienware(ctx); }
namespace wb_driver_arbiter {
// Arbiter Studio Akitsu wireless mice on the shared DriverCore vendor id 373E:
//   * Akitsu (PAW3395):        0009 USB cable, 000A 8K receiver
//   * Akitsu 3950 (PAW3950):   0038 USB cable, 0039 8K receiver
//   * Akitsu Medium (PAW3950): 0066 USB cable, 0067 8K receiver (both verified on hardware)
// LAMZU mice share this vendor id (LAMZU driver), so only these product ids are queried.
// Protocol and model ids from Arbiter Web HUB (webapp.arbiterstudio.com/Config/env-models.json,
// "IsNewProtocol" getBatPer() in the DriverCore bundle; the LAMZU and WLMouse drivers send the same query):
//   Vendor collection FFFF:0000 (65-byte feature report 0):
//   SET_FEATURE [00][00][00][02][02][00][83][00 ...], wait ~100 ms, GET_FEATURE report 0 ->
//   [00][status][..][..][02][..][83][charging][percent] (or without the leading report id byte).
//   status A1 / A2 = answered, A0 = mouse asleep; charging 1 = on the cable; percent 0 on a receiver
//   means the mouse is switched off or not linked. Read-only: no settings or mode writes are sent.
namespace {

constexpr USHORT kVid = 0x373E;

// Cables come before receivers: when both the USB cable and the 8K receiver are plugged in, an
// answering cable claims its paired receiver so the mouse is listed once.
struct Model { USHORT pid; bool receiver; const wchar_t* name; USHORT receiverPid; };
const Model kModels[] = {
    { 0x0009, false, L"Arbiter Studio Akitsu", 0x000A },
    { 0x0038, false, L"Arbiter Studio Akitsu 3950", 0x0039 },
    { 0x0066, false, L"Arbiter Studio Akitsu Medium", 0x0067 },
    { 0x000A, true, L"Arbiter Studio Akitsu", 0 },
    { 0x0039, true, L"Arbiter Studio Akitsu 3950", 0 },
    { 0x0067, true, L"Arbiter Studio Akitsu Medium", 0 },
};

enum class Reply { Battery, Asleep, None };

Reply Decode(const std::vector<BYTE>& r, int& pct, bool& charging) {
    BYTE status = 0, chg = 0, level = 0;
    if (r.size() >= 9 && r[4] == 0x02 && r[6] == 0x83) {
        status = r[1];
        chg = r[7];
        level = r[8];
    } else if (r.size() >= 8 && r[3] == 0x02 && r[5] == 0x83) {
        status = r[0];
        chg = r[6];
        level = r[7];
    } else {
        return Reply::None;
    }
    if (status == 0xA0) return Reply::Asleep;
    if (status != 0xA1 && status != 0xA2) return Reply::None;
    if (level > 100) return Reply::None;
    pct = level;
    charging = chg == 0x01;
    return Reply::Battery;
}

Reply Query(HidDev& d, int& pct, bool& charging) {
    std::vector<BYTE> req(65, 0);  // [0] = report id 0
    req[3] = 0x02;
    req[4] = 0x02;
    req[6] = 0x83;
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (!d.SetFeature(req)) return Reply::None;
        for (int read = 0; read < 3; ++read) {
            Sleep(read == 0 ? 100 : 60);
            std::vector<BYTE> r;
            if (!d.GetFeature(0x00, r)) return Reply::None;
            Reply rep = Decode(r, pct, charging);
            if (rep != Reply::None) return rep;
        }
    }
    return Reply::None;
}

}  // namespace

void PollArbiter(PollContext& ctx) {
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        auto colls = ctx.Find(kVid, m.pid, [](const HidColl& c) { return c.page() == 0xFFFF && c.featLen() == 65; });
        if (colls.empty()) continue;
        std::stable_sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) {
            return (a->usage() == 0) > (b->usage() == 0);
        });
        bool handled = false;
        for (const HidColl* c : colls) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            int pct = -1;
            bool charging = false;
            Reply rep = Query(d, pct, charging);
            if (rep == Reply::None) continue;
            DeviceStatus s;
            // Share one key between cable and receiver so plugging in the cable updates the existing
            // entry in place instead of leaving the receiver entry dimmed for graceSec.
            USHORT keyPid = m.receiverPid ? m.receiverPid : m.pid;
            s.key = L"arbiter:" + Hex4(kVid) + L":" + Hex4(keyPid);
            s.name = m.name;
            s.kind = DeviceKind::Mouse;
            s.via = m.receiver ? L"2.4 GHz receiver" : L"USB cable";
            if (m.receiver || !ctx.Find(kVid, m.receiverPid, [](const HidColl& col) {
                    return col.page() == 0xFFFF && col.featLen() == 65;
                }).empty()) {
                s.graceSec = 1800;
            }
            if (rep == Reply::Asleep) {
                s.asleep = true;
            } else {
                s.connected = pct > 0 || !m.receiver;  // 0 from a dongle = mouse off or out of range
                if (s.connected) s.percent = pct;
                s.charging = charging;
            }
            ctx.Add(s, kVid, m.pid);
            if (m.receiverPid) ctx.Claim(kVid, m.receiverPid);
            handled = true;
            break;
        }
        // Claim present Akitsu hardware even when silent so no later driver reports it under another
        // name, and so the app keeps the last reading across sleep via graceSec.
        if (!handled) ctx.Claim(kVid, m.pid);
    }
}

}

void PollArbiter(PollContext& ctx) { wb_driver_arbiter::PollArbiter(ctx); }
namespace wb_driver_areson {
// Redragon wireless mice on Areson/CompX 2.4 GHz receivers (vendor 25A7), e.g. M913 Impact Elite
// and M915RGB-WL. The battery is not pushed; it has to be asked for with a read-only "GetPower"
// command, so nothing is ever written to the mouse (no LED, DPI or configuration report):
//   request  SET_FEATURE on the FF02:0002 collection of interface 1, 17 bytes:
//            [08][04][00 ...][checksum]  checksum = 0x55 - sum(bytes 0..15)
//   reply    input report on the FF01:0000 collection of interface 1:
//            [09][04][00][..][..][..][level][charging][00 ...][checksum]
//            the whole frame sums to 0x55 (mod 256); level is 0..10 -> 0..100%
//            (the official software showed 80% for level 8); charging is any non-zero byte.
// Sources:
//  * https://github.com/Qehbr/m913-ctl (M913 Linux driver: 17-byte frames, 0x55 checksum)
//  * https://github.com/Doomguy-BFG9000/redragon-m913-battery-tray (M913 battery reader)
//  * BatteryHub's capture of the official Redragon software against an M915RGB-WL on the 25A7:FA07
//    receiver, which is what pinned down the two collections and the reply layout above.
// Only the FA07/FA08 product ids are handled: the FA7B/FA7C family reports the level on a
// different scale, and reporting it here would show a wrong percentage.
namespace {

// One GetPower exchange. percent is 0..100, or -1 when the mouse never answered.
struct Power {
    int percent = -1;
    bool charging = false;
};

bool QueryPower(HidDev& cmd, HidDev& reply, Power& out) {
    std::vector<BYTE> req(17, 0);
    req[0] = 0x08;  // report id
    req[1] = 0x04;  // GetPower
    int sum = 0;
    for (int i = 0; i < 16; i++) sum += req[i];
    req[16] = (BYTE)(0x55 - sum);
    for (int attempt = 0; attempt < 2; attempt++) {
        if (attempt) Sleep(200);
        reply.Flush();  // a report queued before the request would be mistaken for the answer
        if (!cmd.SetFeature(req)) return false;  // receiver gone; retrying will not help
        std::vector<BYTE> r;
        if (reply.Read(r, 1800) < 17) continue;  // asleep or out of range
        unsigned total = 0;
        for (BYTE b : r) total += b;
        if ((total & 0xFF) != 0x55 || r[0] != 0x09 || r[1] != 0x04 || r[2] != 0x00 || r[6] > 10) continue;
        out.percent = Clamp100(r[6] * 10);
        out.charging = r[7] != 0;
        return true;
    }
    return false;
}

// Appends the mouse behind one receiver. Returns false (claiming nothing) when it does not answer,
// so a sleeping mouse simply keeps its last reading instead of clearing it.
bool Probe(PollContext& ctx, const HidColl& cmdColl, const HidColl& replyColl, USHORT vid, USHORT pid) {
    HidDev cmd;
    if (!cmd.OpenFeatureOnly(cmdColl)) return false;  // zero access is enough for feature reports
    HidDev reply;
    if (!reply.Open(replyColl)) return false;
    Power p;
    if (!QueryPower(cmd, reply, p)) return false;

    DeviceStatus s;
    s.key = L"areson:" + Hex4(vid) + L":" + Hex4(pid);
    s.name = L"Redragon wireless mouse";
    s.kind = DeviceKind::Mouse;
    s.via = L"2.4 GHz receiver";
    s.percent = p.percent;
    s.charging = p.charging;
    s.graceSec = 1800;  // existing UI keeps the last reading, dimmed, while the mouse sleeps
    ctx.Add(s, vid, pid);
    return true;
}

}  // namespace

void PollAreson(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x25A7)) {
        if (pid != 0xFA07 && pid != 0xFA08) continue;  // FA7B/FA7C report the level on another scale
        bool done = false;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.iface == 1 && c.page() == 0xFF02 && c.usage() == 0x0002 && c.featLen() >= 17;
             })) {
            for (const HidColl* i : ctx.Find(vid, pid, [](const HidColl& c) {
                     return c.iface == 1 && c.page() == 0xFF01 && c.usage() == 0x0000 && c.inLen() >= 17;
                 })) {
                if (Probe(ctx, *c, *i, vid, pid)) done = true;
                if (done) break;
            }
            if (done) break;
        }
    }
}

}

void PollAreson(PollContext& ctx) { wb_driver_areson::PollAreson(ctx); }
namespace wb_driver_astro {
// Logitech / ASTRO wireless headsets (A50 X, A50 Gen 5, A50 Gen 4, A50 Gen 3, A20 Gen 1/2).
//
// 1. ASTRO A50 X (046D:0B0B) and A50 Gen 5 (046D:0B1C):
//    Vendor HID collection on usage page FF32 / usage 0074 (interface 8), 64-byte reports, ID 02:
//      request: [02][0C][03][00][06][0C][00...]
//      reply:   [02][0C][len][00][06][handle][percent][...][charging]
//    (Protocol from Sapd/HeadsetControl logitech_astro_a50.hpp.)
//
// 2. ASTRO A50 Gen 4 ("Mimas"), A50 Gen 3 ("Europa") and A20 ("Aquarius"), VID 9886:
//    * Gen 4 base stations: 002C (PlayStation/PC), 002D / 002A (Xbox/PC)
//    * Gen 3 base stations: 0015, 0014, 0018, 001D, 0026
//    * A20 transmitters:    0037, 0038, 0039, 0043, 0044, 0045
//    Vendor HID collection on usage page FF32 or FF31, usage 0074 (interface 6), 64-byte reports,
//    report ID 02:
//      request: [02][cmd]
//      reply:   [02][status][len][payload...]   (status 02 = OK, 00 = accepted, 01 = error)
//    cmd 0x54 (headset status) -> payload[0]: bit 0 (0x01) = docked, bit 1 (0x02) = live 2.4 GHz link
//    cmd 0x7C (battery status) -> payload[0]: bit 7 (0x80) = charging, bits 0..6 = battery % (0..100)
//    (Protocol from tdryer/eh-fifty, nichtlegacy/AstroBar, Sapd/HeadsetControl logitech_astro_a50_gen4.hpp.)
//
//    A50 Gen 3 (9886:0015, base TXDP01) answers 0x54 but rejects 0x7C (status 01, "HID_E..." text).
//    Its battery is in the "current UI" read instead: request [02][72][01][01], reply data is four
//    [02][field][value][00] groups; the field-04 group (data[12..15]) holds battery % in bits 0..6 and
//    charging in bit 7. On Gen 4, 0x72 is a different read (the game/chat balance in eh-fifty), so it
//    is only used after 0x7C fails.
//    (Protocol from BenStein1/OpenAstroCC A50-PROTOCOL.md, tested on 9886:0015.)
//
//    Coexistence with Astro Command Center (Microsoft Store / UWP):
//    The UWP app opens FF32:0074 with FileAccessMode.ReadWrite, placing an exclusive kernel lock
//    (ShareMode = 0) on the collection while open. Open() then fails, so PollAstro keeps the headset
//    visible (with its last cached reading) and resumes live polling as soon as Astro Command Center
//    is closed.
namespace {

constexpr USHORT kVidAstro = 0x9886;
constexpr USHORT kVidLogitech = 0x046D;

struct LegacyModel { USHORT pid; const wchar_t* name; };
constexpr LegacyModel kLegacyModels[] = {
    // A50 Gen 4 (Mimas)
    { 0x002C, L"ASTRO A50 Gen 4" },
    { 0x002D, L"ASTRO A50 Gen 4" },
    { 0x002A, L"ASTRO A50 Gen 4" },
    // A50 Gen 3 (Europa)
    { 0x0015, L"ASTRO A50 Gen 3" },
    { 0x0014, L"ASTRO A50 Gen 3" },
    { 0x0018, L"ASTRO A50 Gen 3" },
    { 0x001D, L"ASTRO A50 Gen 3" },
    { 0x0026, L"ASTRO A50 Gen 3" },
    // A20 Wireless (Aquarius)
    { 0x0037, L"ASTRO A20 Wireless" },
    { 0x0038, L"ASTRO A20 Wireless" },
    { 0x0039, L"ASTRO A20 Wireless" },
    { 0x0043, L"ASTRO A20 Wireless" },
    { 0x0044, L"ASTRO A20 Wireless" },
    { 0x0045, L"ASTRO A20 Wireless" },
};

struct CachedState {
    int percent = -1;
    bool charging = false;
    bool docked = false;
};
std::map<USHORT, CachedState> g_cache;

// Gen 4 bases declare report ID 02, so the frame [02][cmd] is written as-is. Some Gen 3 base
// firmware (9886:0015, TXDP01) declares no report ID: Windows then rejects a buffer starting with
// 02, and the same frame has to be sent behind a 00 report-ID byte. Remember what works per PID.
enum class Framing { Unknown, ReportId02, NoReportId };
std::map<USHORT, Framing> g_framing;

enum class QResult { Ok, Error, NoReply, WriteFailed };

std::vector<BYTE> LegacyFrame(Framing f, BYTE cmd, const std::vector<BYTE>& payload = {}) {
    std::vector<BYTE> v;
    if (f == Framing::NoReportId) v.push_back(0x00);
    v.push_back(0x02);
    v.push_back(cmd);
    if (!payload.empty()) {
        v.push_back((BYTE)payload.size());
        v.insert(v.end(), payload.begin(), payload.end());
    }
    return v;
}

// reply: the whole frame starting at [02][status][len]...
QResult QueryOnce(HidDev& d, Framing f, BYTE cmd, const std::vector<BYTE>& payload, std::vector<BYTE>& reply) {
    d.Flush();
    if (!d.Write(LegacyFrame(f, cmd, payload))) return QResult::WriteFailed;
    ULONGLONG end = GetTickCount64() + 500;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 4) break;
        if (r[0] != 0x02) continue;
        if (r[1] == 0x01) return QResult::Error;  // understood, but refused (unknown command / not linked)
        if (r[1] == 0x02) {
            reply = r;
            return QResult::Ok;
        }
    }
    return QResult::NoReply;
}

QResult QueryLegacy(HidDev& d, USHORT pid, BYTE cmd, const std::vector<BYTE>& payload, std::vector<BYTE>& reply) {
    Framing known = g_framing.count(pid) ? g_framing[pid] : Framing::Unknown;
    Framing order[2] = { Framing::ReportId02, Framing::NoReportId };
    if (known == Framing::NoReportId) std::swap(order[0], order[1]);
    QResult last = QResult::NoReply;
    for (Framing f : order) {
        last = QueryOnce(d, f, cmd, payload, reply);
        if (last == QResult::Ok || last == QResult::Error) {
            g_framing[pid] = f;
            return last;
        }
        if (known != Framing::Unknown) break;  // framing already proven: a miss is a real miss
    }
    return last;
}

QResult QueryLegacy(HidDev& d, USHORT pid, BYTE cmd, BYTE& value) {
    std::vector<BYTE> r;
    QResult res = QueryLegacy(d, pid, cmd, {}, r);
    if (res == QResult::Ok) value = r[3];
    return res;
}

// Gen 3 battery from the 0x72 "current UI" read: data[12..15] = [02][04][battery][00].
bool QueryGen3Battery(HidDev& d, USHORT pid, BYTE& value) {
    std::vector<BYTE> r;
    if (QueryLegacy(d, pid, 0x72, { 0x01 }, r) != QResult::Ok) return false;
    if ((r[2] & 0x3F) < 16 || r.size() < 3 + 16) return false;
    const BYTE* data = r.data() + 3;
    if (data[12] != 0x02 || data[13] != 0x04 || (data[14] & 0x7F) > 100) return false;
    value = data[14];
    return true;
}

bool PollLegacyCollection(const HidColl& c, const wchar_t* name, DeviceStatus& s) {
    s.key = L"astro:" + Hex4(c.pid);
    s.name = name;
    s.kind = DeviceKind::Headset;
    s.via = L"USB base station";
    s.graceSec = 1800;

    HidDev d;
    if (d.Open(c)) {
        // 0x54 (dock / link status) is optional: some Gen 3 firmware may not answer it.
        BYTE st = 0;
        bool haveStatus = QueryLegacy(d, c.pid, 0x54, st) == QResult::Ok;
        bool docked = haveStatus && (st & 0x01) != 0;
        if (haveStatus) {
            s.connected = docked || (st & 0x02) != 0;
            if (!s.connected) return true;
        }

        BYTE bat = 0;
        QResult rb = QueryLegacy(d, c.pid, 0x7C, bat);
        if (rb == QResult::Error && haveStatus && QueryGen3Battery(d, c.pid, bat)) rb = QResult::Ok;  // A50 Gen 3
        if (rb == QResult::Ok && (bat & 0x7F) <= 100) {
            int pct = bat & 0x7F;
            s.connected = true;
            s.percent = pct;
            s.charging = (bat & 0x80) != 0 || (docked && pct < 100);
            if (docked) s.detailText = L"docked";
            g_cache[c.pid] = { s.percent, s.charging, docked };
            return true;
        }
        if (rb == QResult::Error && !haveStatus) {  // base answered: headset off or out of range
            s.connected = false;
            return true;
        }
        return haveStatus;
    }

    // Exclusive lock held by Astro Command Center (UWP): keep the headset visible with cached reading.
    s.connected = true;
    auto it = g_cache.find(c.pid);
    if (it != g_cache.end() && it->second.percent >= 0) {
        s.percent = it->second.percent;
        s.charging = it->second.charging;
        s.detailText = it->second.docked ? L"docked, Astro Command Center" : L"Astro Command Center";
    } else {
        s.detailText = L"Astro Command Center";
    }
    return true;
}

bool IsLegacyVendorColl(const HidColl& c) {
    return (c.page() == 0xFF32 || c.page() == 0xFF31 || c.page() == 0xFF00) && c.inLen() >= 64 && c.outLen() >= 64;
}

}  // namespace

void PollAstro(PollContext& ctx) {
    // 1. ASTRO A50 X (046D:0B0B) and A50 Gen 5 (046D:0B1C)
    for (const auto& c : ctx.colls) {
        if (c.vid != kVidLogitech || (c.pid != 0x0B0B && c.pid != 0x0B1C) ||
            ctx.IsClaimed(c.vid, c.pid) || c.page() != 0xFF32 || c.usage() != 0x0074 ||
            c.inLen() != 64 || c.outLen() != 64) continue;
        HidDev d;
        if (!d.Open(c)) continue;
        d.Flush();
        // No payload: read battery only; never change audio or headset settings.
        std::vector<BYTE> request(64, 0);
        request[0] = 0x02;
        request[1] = 0x0C;
        request[2] = 3;
        request[4] = 0x06;
        request[5] = 0x0C;
        if (!d.Write(request)) continue;
        const auto deadline = GetTickCount64() + 1000;
        for (int attempt = 0; attempt < 8; ++attempt) {
            const auto now = GetTickCount64();
            if (now >= deadline) break;
            std::vector<BYTE> reply;
            int n = d.Read(reply, (int)(deadline - now));
            if (n < 0) break;
            // Ignore unsolicited volume/mix events and malformed or partial frames.
            if (n < 9 || reply.size() < 9 || reply[0] != 0x02 || reply[1] != 0x0C ||
                reply[2] < 6 || (size_t)reply[2] + 3 > reply.size() ||
                (int)reply[2] + 3 > n || reply[3] != 0 || reply[4] != 0x06 ||
                (reply[5] != 0 && reply[5] != 0x0C) || reply[6] > 100) continue;
            DeviceStatus s;
            s.key = ctx.StableKey(L"astro:" + Hex4(c.pid));
            s.name = c.pid == 0x0B0B ? L"ASTRO A50 X" : L"ASTRO A50 Gen 5";
            s.kind = DeviceKind::Headset;
            s.via = L"USB base station";
            s.percent = reply[6];
            s.charging = reply[8] != 0;
            ctx.Add(s, c.vid, c.pid);
            break;
        }
    }

    // 2. ASTRO A50 Gen 4 / Gen 3 and A20 Wireless (VID 9886)
    for (const LegacyModel& m : kLegacyModels) {
        if (ctx.IsClaimed(kVidAstro, m.pid)) continue;
        for (const HidColl* c : ctx.Find(kVidAstro, m.pid, [](const HidColl& c) {
                 bool validPage = IsLegacyVendorColl(c);
                 bool lockedVendorIf = c.page() == 0 && (c.iface == 6 || c.iface == -1);
                 return validPage || lockedVendorIf;
             })) {
            DeviceStatus s;
            if (PollLegacyCollection(*c, m.name, s)) {
                ctx.Add(s, kVidAstro, m.pid);
                break;
            }
        }
    }
}

}

void PollAstro(PollContext& ctx) { wb_driver_astro::PollAstro(ctx); }
namespace wb_driver_atk_bitmouse {
// ATK / VXE NearLink mice on the "NK mouse NANO dongle" 373B:1216 (VXE R1 NK and siblings). This dongle
// has no 17-byte report 08 collection for the Kysona protocol; it speaks ATK's BITMOUSE protocol on
// vendor collection FF05:0001 instead.
//   Output report 08, 64 bytes: [08][checksum][72][02][00][01][cmd][01][00 ...],
//   checksum = sum(bytes 2..63) & 0xFF; target 01 = the mouse behind the dongle.
//   cmd 07 = battery level, cmd 0F = charging flag.
//   Reply on the same collection: [08][72][status][seq][00][cmd][01][value]...; status FF = error.
//   No reply = the mouse is asleep or switched off (moving it brings the reading back).
// From github.com/HeyOkay/HaloBattery PR #350 (providers/atk_bitmouse.py; verified by a VXE R1 owner on
// Windows: the level matches ATK's app, charging follows the cable) and github.com/qwr79/mouse-battery.
//
// ATK Zero uses the same 64-byte framing on FF05:0001, with target 00 for the 1154 cable or 01 for
// the 124F / 1155 8K receivers. Only status 00 is fresh. Command 07 reads the level on either link,
// and command 08 reports charging (02) on the cable. The receivers only relay the battery query.
// Reference: github.com/Fan4Metal/mouse_tray/blob/master/mouse_tray/drivers/chipset/nordic54.py
//
// ATK "8K Nano Dongle" 373B:1178 (NK mouse 8K NANO dongle) is a Holtek receiver with a different protocol on
// vendor collection FF10:0001 (65-byte input/output, 9-byte feature report, no report ids):
//   Feature report 00, 8 bytes: [82][00 x7] (GetConfigData).
//   Reply as an input report: [82][rate][dpi stage][light mode][brightness][speed][color][reserved]
//   [battery][link]...; battery bit 7 = charging, bits 0..6 = percent; link 0 = no mouse paired/awake.
// From ATK HUB (v3-hub.atkgear.com 3.2.33, static/AtkRadioGroup-*.js): the Holtek mouse controller lists
// productId 4472 (0x1178), usagePage 0xFF10 usage 1, "ATK 8K Nano Dongle", and reads battery and link state
// with GetConfigData (0x82) sent as feature report 0, the reply matched on its first byte.

namespace {

const USHORT kVid = 0x373B;
const USHORT kNanoDongle = 0x1216;
const USHORT kNano8kDongle = 0x1178;
const USHORT kZeroCable = 0x1154;

enum class BitmouseLink { NearLink, ZeroCable, ZeroReceiver };

// Sends command `cmd` and returns its one-byte payload, or -1 when there is no valid reply.
int Query(HidDev& d, BYTE cmd, BitmouseLink link = BitmouseLink::NearLink) {
    const bool zero = link != BitmouseLink::NearLink;
    std::vector<BYTE> req(64, 0);
    req[0] = 0x08;
    req[2] = 0x72;
    req[3] = 0x02;  // request length
    req[5] = link == BitmouseLink::ZeroCable ? 0x00 : 0x01;
    req[6] = cmd;
    req[7] = 0x01;  // expected reply length
    unsigned sum = 0;
    for (size_t i = 2; i < req.size(); i++) sum += req[i];
    req[1] = zero ? 0x7D : (BYTE)(sum & 0xFF);
    d.Flush();
    ULONGLONG end = GetTickCount64() + 500;
    if (!d.Write(req)) return -1;
    if (!zero) end = GetTickCount64() + 700;  // retain the NearLink reply window
    std::vector<BYTE> r;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) return -1;
        if (d.Read(r, left) < 0) return -1;
        if (r.size() < 8 || r[0] != 0x08 || r[1] != 0x72) continue;
        if (r[5] != cmd || (zero && r[6] != 0x01)) continue;  // a reply to another request
        if (zero ? r[2] != 0x00 : r[2] == 0xFF) return -1;
        return r[7];
    }
}

void PollZero(PollContext& ctx) {
    bool reported = false;
    // Prefer the cable for a fresh reading while charging and use the same battery history on either link.
    for (USHORT pid : { kZeroCable, (USHORT)0x124F, (USHORT)0x1155 }) {
        if (ctx.IsClaimed(kVid, pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF05 && c.usage() == 0x0001 && c.outLen() == 64 && c.inLen() == 64;
             })) {
            ctx.Claim(kVid, pid);  // keep later drivers off this collection, even when asleep
            if (reported) break;
            HidDev d;
            if (!d.Open(*c)) continue;

            const bool wired = pid == kZeroCable;
            const BitmouseLink link = wired ? BitmouseLink::ZeroCable : BitmouseLink::ZeroReceiver;
            const int level = Query(d, 0x07, link);
            if (level < 0 || level > 100) continue;

            DeviceStatus s;
            s.key = L"atkzero:" + Hex4(kZeroCable);
            s.name = L"ATK Zero";
            s.kind = DeviceKind::Mouse;
            s.via = wired ? L"USB cable" : L"2.4 GHz receiver";
            s.graceSec = 1800;
            s.percent = level;
            s.charging = wired && Query(d, 0x08, link) == 0x02;
            ctx.Add(s, kVid, pid);
            reported = true;
            break;
        }
    }
}

// 8K Nano dongle: GetConfigData (0x82) as feature report 0; the reply arrives as an input report
// starting with 0x82. Returns false when nothing valid came back.
bool QueryNano8kConfig(HidDev& d, std::vector<BYTE>& r) {
    d.Flush();
    if (!d.SetFeature({0x00, 0x82})) return false;
    ULONGLONG end = GetTickCount64() + 700;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) return false;
        if (d.Read(r, left) < 10) return false;
        if (r[0] == 0x82) return true;  // other input reports (e.g. 0x12 DPI / rate notifications) are skipped
    }
}

void PollNano8k(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kNano8kDongle)) return;
    for (const HidColl* c : ctx.Find(kVid, kNano8kDongle, [](const HidColl& c) {
             return c.page() == 0xFF10 && c.usage() == 0x0001 && c.featLen() >= 9 && c.inLen() >= 11;
         })) {
        HidDev d;
        if (!d.Open(*c)) continue;
        std::vector<BYTE> r;
        if (!QueryNano8kConfig(d, r)) continue;
        // The dongle speaks this protocol: keep later drivers from probing it with other frames.
        if (r[9] == 0) {
            ctx.Claim(kVid, kNano8kDongle);  // no mouse linked (off or asleep)
            return;
        }
        int level = r[8] & 0x7F;
        if (level < 1 || level > 100) {
            ctx.Claim(kVid, kNano8kDongle);
            return;
        }
        DeviceStatus s;
        s.key = L"atknano8k:" + Hex4(kNano8kDongle);
        s.name = L"ATK 8K Nano mouse";  // the dongle is shared by several ATK / VXE models
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.graceSec = 1800;
        s.percent = level;
        s.charging = (r[8] & 0x80) != 0;
        ctx.Add(s, kVid, kNano8kDongle);
        return;
    }
}

}  // namespace

void PollAtkBitmouse(PollContext& ctx) {
    PollZero(ctx);
    PollNano8k(ctx);
    if (ctx.IsClaimed(kVid, kNanoDongle)) return;
    for (const HidColl* c : ctx.Find(kVid, kNanoDongle, [](const HidColl& c) {
             return c.page() == 0xFF05 && c.usage() == 0x0001 && c.outLen() >= 64 && c.inLen() >= 8;
         })) {
        HidDev d;
        if (!d.Open(*c)) continue;
        int level = Query(d, 0x07);
        if (level < 1 || level > 100) continue;  // asleep / off, or no valid reading
        DeviceStatus s;
        s.key = L"atknk:" + Hex4(kNanoDongle);
        s.name = L"ATK / VXE NK mouse";  // the dongle is shared by several NearLink models
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.graceSec = 1800;  // the dongle stops answering while the mouse sleeps: keep the last value
        s.percent = level;
        s.charging = Query(d, 0x0F) == 1;
        ctx.Add(s, kVid, kNanoDongle);
        break;
    }
}

}

void PollAtkBitmouse(PollContext& ctx) { wb_driver_atk_bitmouse::PollAtkBitmouse(ctx); }
namespace wb_driver_atk_keyboard {
// Evision keyboards on the shared 2.4 GHz receiver (320F:5088) or USB cable (320F:5055): ATK V75X and the
// VGN / VXE keyboards that VGN HUB drives the same way.
// ATK V75X: ported from HaloBattery (github.com/HeyOkay/HaloBattery, providers/atk_keyboard.py, MIT; confirmed
// there by its author on both), from ATK HUB 3.2.29's Evision keyboard driver (keyboard-info.util-*.js).
// VGN / VXE: VGN HUB (drive.vgnlab.com.cn, assets/index-*.js) lists these product names on 320F:5088 / 5055,
// FF1C:0092, report 4, type "weisheng"; its getElectricQuantity() sends [04][00 00][1A][06] and reads the
// level from the same byte, and it checks that the reply echoes the first bytes of the request. myfinder-device-
// battery (engine/upstream-protocols.md, "VGN 2.4G keyboard receiver") read 93% this way on a VGN receiver
// (VGN HUB showed 94%) and saw the reply arrive about 1 s after the request, so up to 2.5 s is waited.
//   Output report 04 on FF1C:0092, 31 payload bytes: [04][seq lo][seq hi][1A][00, or 06 for VGN][00 ...]
//   Input report 04: [04][seq lo][seq hi][1A][..][00 00][..][percent][state: 0 on battery, 1/2 charging]
// These ids are shared by many Evision keyboards, so the product name is checked as well, and the keyboard is
// named after it (the receiver's name without "2.4G Dongle").
// The receiver is also sold under the generic name "Wireless Gaming Keyboard" (Feker Alice80 and others); the same
// request and reply were confirmed on a Feker Alice80 on that receiver (seq echoed, command 1A, level at byte 8),
// and its backlight still timed out while it was asked every minute. Such keyboards are shown under that name.
// HaloBattery only asks over the receiver while the keyboard is being typed on, so a sleeping keyboard is
// not woken. This app does not watch keyboard input, so it asks at most every 10 minutes over the receiver
// instead (every poll on the cable, which powers the keyboard anyway).
// The ATK Z87 receiver (373B:1012, "ATK Z87 Dongle") answers the same request without the VGN byte; a reply
// with FF in place of 1A means the keyboard is asleep.

namespace {

constexpr USHORT kVid = 0x320F, kReceiver = 0x5088, kCable = 0x5055;
constexpr ULONGLONG kReceiverIntervalMs = 10 * 60 * 1000;

struct Model {
    const wchar_t* product;  // product string of the receiver or the cable
    bool vgn;                // asked the way VGN HUB asks
};
const Model kModels[] = {
    { L"ATK V75X 2.4G Dongle", false },     { L"ATK V75X", false },
    { L"VGN S99 2.4G Dongle", true },       { L"VGN S99", true },
    { L"VGN V98pro V2 2.4G Dongle", true }, { L"VGN V98pro V2", true },
    { L"VGN V98 PRO 2.4G Dongle", true },   { L"VGN V98pro", true },
    { L"VGN N75 PRO 2.4G Dongle", true },   { L"VGN N75 PRO", true },
    { L"VGN VXE75 V2 2.4G Dongle", true },  { L"VGN VXE75 V2", true },
    { L"VGN VXE75 2.4G Dongle", true },     { L"VGN VXE75", true },
    { L"VXE V87 2.4G Dongle", true },       { L"VXE V87", true },
    { L"VXE V87 RGB", true },
    { L"Wireless Gaming Keyboard", false },  // generic receiver name (Feker Alice80 and others)
};

const Model* FindModel(std::wstring& name) {
    while (!name.empty() && name.back() == L' ') name.pop_back();
    for (const Model& m : kModels)
        if (_wcsicmp(name.c_str(), m.product) == 0) return &m;
    return nullptr;
}

std::wstring KeyboardName(const std::wstring& product) {
    const size_t n = wcslen(L" 2.4G Dongle");
    if (product.size() > n && _wcsicmp(product.c_str() + product.size() - n, L" 2.4G Dongle") == 0)
        return product.substr(0, product.size() - n);
    return product;
}

bool ReadBattery(const HidColl& c, bool vgn, int& percent, bool& charging) {
    static USHORT seq = 0;
    seq = (USHORT)(seq % 0xFFFF + 1);
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    std::vector<BYTE> req(32, 0);
    req[0] = 0x04;
    req[1] = (BYTE)(seq & 0xFF);
    req[2] = (BYTE)(seq >> 8);
    req[3] = 0x1A;
    req[4] = vgn ? 0x06 : 0x00;
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + 2500;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int n = d.Read(r, 100);
        if (n < 10 || r[0] != 0x04 || r[1] != (BYTE)(seq & 0xFF) || r[2] != (BYTE)(seq >> 8) || r[3] != 0x1A || r[5] || r[6])
            continue;
        if (r[8] > 100 || r[9] > 2) return false;
        percent = r[8];
        charging = r[9] == 1 || r[9] == 2;
        return true;
    }
    return false;
}

constexpr USHORT kZ87Vid = 0x373B, kZ87Receiver = 0x1012;
bool ReadZ87Battery(const HidColl& c, USHORT& seq, int& percent, bool& charging) {
    seq = (USHORT)(seq % 0xFFFF + 1);
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    std::vector<BYTE> req(32, 0);  // HidDev pads to the output report length (64 on the Z87).
    req[0] = 0x04;
    req[1] = (BYTE)(seq & 0xFF);
    req[2] = (BYTE)(seq >> 8);
    req[3] = 0x1A;
    ULONGLONG end = GetTickCount64() + 1000;
    if (!d.Write(req)) return false;
    std::vector<BYTE> r;
    for (;;) {
        ULONGLONG now = GetTickCount64();
        if (now >= end) return false;
        int n = d.Read(r, (int)(end - now));
        if (n < 0) return false;
        if (n < 4 || r[0] != 0x04 || r[1] != req[1] || r[2] != req[2]) continue;
        if (c.vid == kZ87Vid && r[3] == 0xFF) return false;  // sleeping keyboard
        if (n < 10 || r[3] != 0x1A || r[5] || r[6]) continue;
        if (r[8] > 100 || r[9] > 2) return false;
        percent = r[8];
        charging = r[9] == 1 || r[9] == 2;
        return true;
    }
}

// The Z87 keeps its own cache, separate from the VGN / VXE / Feker / V75X path.
void PollZ87(PollContext& ctx) {
    struct Cache { USHORT sequence=0; ULONGLONG nextAsk=0; int percent=-1; bool charging=false; };
    static Cache cache;
    if (ctx.IsClaimed(kZ87Vid, kZ87Receiver)) return;
    for (const HidColl* c : ctx.Find(kZ87Vid, kZ87Receiver, [](const HidColl& c) {
        return c.page()==0xFF1C && c.usage()==0x0092 && c.inLen()==64 && c.outLen()==64;
    })) {
        std::wstring name=HidProduct(*c);
        while (!name.empty() && name.back()==L' ') name.pop_back();
        if (_wcsicmp(name.c_str(),L"ATK Z87 Dongle")!=0) continue;
        ULONGLONG now=GetTickCount64();
        if (now>=cache.nextAsk) {
            cache.nextAsk=now+kReceiverIntervalMs;
            if (!ReadZ87Battery(*c,cache.sequence,cache.percent,cache.charging)) {
                cache.percent=-1; cache.charging=false;
            }
        }
        DeviceStatus s;
        s.key=L"atk_keyboard:1012"; s.name=L"ATK Z87";
        s.kind=DeviceKind::Keyboard; s.via=L"2.4 GHz receiver";
        s.graceSec=1800; s.percent=cache.percent; s.charging=cache.charging;
        s.asleep=s.percent<0;
        ctx.Add(s,kZ87Vid,kZ87Receiver);return;
    }
    cache={}; // Reconnects should not inherit an old receiver reading.
}

}  // namespace

void PollAtkKeyboard(PollContext& ctx) {
    PollZ87(ctx);
    static ULONGLONG lastAsk = 0;
    static int lastPercent = -1;
    static bool lastCharging = false;
    for (USHORT pid : { kCable, kReceiver }) {  // the cable wins when both are connected
        if (ctx.IsClaimed(kVid, pid)) continue;
        bool cable = pid == kCable;
        for (const HidColl* c : ctx.Find(kVid, pid, [](const HidColl& c) { return c.page() == 0xFF1C && c.usage() == 0x0092; })) {
            std::wstring product = HidProduct(*c);
            const Model* model = FindModel(product);
            if (!model) continue;
            DeviceStatus s;
            s.key = L"atk_keyboard:5088";  // one entry for the keyboard on either connection
            s.name = KeyboardName(model->product);
            s.kind = DeviceKind::Keyboard;
            s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
            s.graceSec = 1800;
            ULONGLONG now = GetTickCount64();
            bool ask = cable || !lastAsk || now - lastAsk >= kReceiverIntervalMs;
            int percent = -1;
            bool charging = false;
            if (ask) {
                lastAsk = now;
                if (ReadBattery(*c, model->vgn, percent, charging)) {
                    lastPercent = percent;
                    lastCharging = charging;
                }
            }
            if (ask && percent < 0) {
                s.asleep = true;  // no reply: asleep, off or busy; keep the last value
            } else {
                s.percent = ask ? percent : lastPercent;
                s.charging = ask ? charging : lastCharging;
                if (s.percent < 0) s.asleep = true;
            }
            ctx.Add(s, kVid, pid);
            if (cable) ctx.Claim(kVid, kReceiver);
            return;
        }
    }
}

}

void PollAtkKeyboard(PollContext& ctx) { wb_driver_atk_keyboard::PollAtkKeyboard(ctx); }
namespace wb_driver_atk_serial {
// ATK N9 Pro (VID 373B: dongle 11CB, wired 11CA) and first-generation N9 Ultra (dongle 1204,
// wired 1203) headsets. Unlike other headsets they are not queried over HID but over a USB
// virtual COM port (9600 8N1), with 64-byte frames:
//   request [len = 2][cmd][sub][0...]        reply [len][cmd][sub][data...]
//   cmd 00 sub 04 = status  -> data[1] == 1 when the headset is connected to the dongle
//   cmd 00 sub 03 = battery -> data[1] = percent (bits 0-6), charging (bit 7)
// Protocol from ATK's own configuration software (ATK HUB, "n9-pro" model; ATK HUB 3.2.31 maps
// the N9 Ultra ids to the same model and serial settings, see
// github.com/wushuai0802/myfinder-device-battery engine/upstream-protocols.md). The N9 Ultra
// reports its level in 10 % steps. The newer N9 V2 headsets use a different (HID) protocol.
// If the ATK desktop app is running it holds the port and the headset is skipped.

namespace {

struct Model { USHORT pid; const wchar_t* name; bool wired; };

constexpr Model kModels[] = {
    { 0x11CB, L"ATK N9 Pro", false },
    { 0x11CA, L"ATK N9 Pro", true },
    { 0x1204, L"ATK N9 Ultra", false },
    { 0x1203, L"ATK N9 Ultra", true },
};

struct Port { std::wstring name; const Model* model; };

std::vector<Port> FindPorts() {
    std::vector<Port> out;
    HDEVINFO di = SetupDiGetClassDevsW(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (di == INVALID_HANDLE_VALUE) return out;
    SP_DEVINFO_DATA dd{ sizeof(dd) };
    for (DWORD i = 0; SetupDiEnumDeviceInfo(di, i, &dd); i++) {
        wchar_t inst[512];
        if (!SetupDiGetDeviceInstanceIdW(di, &dd, inst, 512, nullptr)) continue;
        std::wstring id = inst;
        for (auto& ch : id) ch = towupper(ch);
        const Model* model = nullptr;
        for (const Model& m : kModels)
            if (id.find(L"VID_373B&PID_" + Hex4(m.pid)) != std::wstring::npos) model = &m;
        if (!model) continue;
        HKEY k = SetupDiOpenDevRegKey(di, &dd, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (k == INVALID_HANDLE_VALUE) continue;
        wchar_t name[32] = {};
        DWORD sz = sizeof(name) - sizeof(wchar_t), type = 0;
        if (RegQueryValueExW(k, L"PortName", nullptr, &type, (BYTE*)name, &sz) == ERROR_SUCCESS && type == REG_SZ)
            out.push_back({ name, model });
        RegCloseKey(k);
    }
    SetupDiDestroyDeviceInfoList(di);
    return out;
}

class Serial {
public:
    explicit Serial(const std::wstring& port) {
        h_ = CreateFileW((L"\\\\.\\" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h_ == INVALID_HANDLE_VALUE) return;
        DCB dcb{ sizeof(dcb) };
        GetCommState(h_, &dcb);
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary = TRUE;
        dcb.fOutxCtsFlow = dcb.fOutxDsrFlow = FALSE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        dcb.fOutX = dcb.fInX = FALSE;
        COMMTIMEOUTS to{};
        to.ReadIntervalTimeout = 50;
        to.ReadTotalTimeoutConstant = 300;
        to.WriteTotalTimeoutConstant = 300;
        if (!SetCommState(h_, &dcb) || !SetCommTimeouts(h_, &to)) Close();
        else PurgeComm(h_, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }
    ~Serial() { Close(); }
    bool ok() const { return h_ != INVALID_HANDLE_VALUE; }

    // Sends command (cmd, sub) and returns the data bytes of the matching 64-byte reply frame.
    bool Command(BYTE cmd, BYTE sub, std::vector<BYTE>& data) {
        BYTE frame[64] = { 2, cmd, sub };
        DWORD n = 0;
        if (!WriteFile(h_, frame, sizeof(frame), &n, nullptr) || n != sizeof(frame)) return false;
        std::vector<BYTE> buf;
        ULONGLONG end = GetTickCount64() + 800;
        while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
            BYTE chunk[128];
            if (!ReadFile(h_, chunk, sizeof(chunk), &n, nullptr)) return false;
            buf.insert(buf.end(), chunk, chunk + n);
            while (buf.size() >= 64) {  // the headset also pushes unsolicited frames; skip those
                if (buf[1] == cmd && buf[2] == sub) {
                    data.assign(buf.begin() + 3, buf.begin() + 64);
                    return true;
                }
                buf.erase(buf.begin(), buf.begin() + 64);
            }
        }
        return false;
    }

private:
    void Close() {
        if (h_ != INVALID_HANDLE_VALUE) CloseHandle(h_);
        h_ = INVALID_HANDLE_VALUE;
    }
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

}  // namespace

void PollAtkSerial(PollContext& ctx) {
    for (const Port& p : FindPorts()) {
        USHORT pid = p.model->pid;
        if (ctx.IsClaimed(0x373B, pid)) continue;
        if (ModConfig::IsIgnored(0x373B, pid)) continue;
        Serial port(p.name);
        if (!port.ok()) continue;  // in use (ATK app open) or not a working port
        std::vector<BYTE> data;
        if (!port.Command(0x00, 0x04, data)) continue;
        DeviceStatus s;
        s.key = L"atk-serial:" + Hex4(pid);
        s.name = p.model->name;
        s.kind = DeviceKind::Headset;
        s.via = p.model->wired ? L"USB cable" : L"2.4 GHz receiver";
        s.connected = data[1] == 1;
        if (s.connected && port.Command(0x00, 0x03, data)) {
            s.percent = Clamp100(data[1] & 0x7F);
            s.charging = (data[1] & 0x80) != 0;
        }
        ctx.Add(s, 0x373B, pid);
    }
}

}

void PollAtkSerial(PollContext& ctx) { wb_driver_atk_serial::PollAtkSerial(ctx); }

namespace wb_driver_audeze {
// Audeze Maxwell headsets (VID 3329) on the USB dongle or the USB-C cable.
// Ported from HaloBattery (github.com/HeyOkay/HaloBattery, providers/audeze.py, MIT; the dongle 4B19 and the
// cable 4B1A were tested there on hardware), whose packets come from HeadsetControl
// (lib/devices/audeze_maxwell.hpp).
//   Vendor collection FF13:0001. Request: 62-byte output report 06; answer: 62-byte input report 07,
//   fetched with GetInputReport. The answer is a rolling buffer of recent replies, so it is scanned for the
//   battery marker D6 0C 00 00 <percent>.
// Only the single battery query (attribute 0x0CD6) is sent, which HaloBattery measured to be enough on the
// dongle and the cable. HaloBattery falls back to HeadsetControl's 14-packet initialisation, which contains
// one packet it could not confirm to be a pure read, so that fallback is deliberately left out here.
// With no headset linked the dongle names itself "... Dongle" and keeps answering with a stale value, so it
// is not asked then. There is no charging flag: a Maxwell always charges on its USB-C cable, so the cable
// answering means charging.
namespace {

constexpr USHORT kVid = 0x3329;
struct Model { USHORT pid; bool cable; const wchar_t* name; };
const Model kModels[] = { { 0x4B1A, true, L"Audeze Maxwell" }, { 0x4B1E, true, L"Audeze Maxwell" },  // cables first: the direct connection wins
                          { 0x4B19, false, L"Audeze Maxwell" }, { 0x4B18, false, L"Audeze Maxwell" },
                          { 0x4B29, false, L"Audeze Maxwell 2" }, { 0x4B28, false, L"Audeze Maxwell 2" } };

int FindLevel(const std::vector<BYTE>& b) {
    for (size_t i = 0; i + 4 < b.size(); i++)
        if (b[i] == 0xD6 && b[i + 1] == 0x0C && b[i + 2] == 0x00 && b[i + 3] == 0x00 && b[i + 4] <= 100) return b[i + 4];
    return -1;
}

int ReadLevel(const HidColl& c) {
    HidDev d;
    if (!d.Open(c)) return -1;
    std::vector<BYTE> req(62, 0);
    BYTE q[] = { 0x06, 0x07, 0x80, 0x05, 0x5A, 0x03, 0x00, 0xD6, 0x0C };
    std::copy(std::begin(q), std::end(q), req.begin());
    int level = -1;
    for (int attempt = 0; attempt < 2; attempt++) {
        if (!d.Write(req)) return -1;
        for (int i = 0; i < 4; i++) {  // the answer can arrive a little later; the newest frame counts
            Sleep(60);
            std::vector<BYTE> r;
            if (!d.GetInputReport(0x07, r)) continue;
            int l = FindLevel(r);
            if (l >= 0) level = l;
            if (i >= 2 && level >= 0) return level;
        }
        if (level >= 0) return level;
    }
    return level;
}

}  // namespace

void PollAudeze(PollContext& ctx) {
    static std::map<USHORT, ULONGLONG> firstSeen;  // a headset reports 0% for a moment right after power-on
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        auto colls = ctx.Find(kVid, m.pid, [](const HidColl& c) { return c.page() == 0xFF13 && c.usage() == 0x0001 && c.inLen() > 0; });
        if (colls.empty()) {
            firstSeen.erase(m.pid);
            continue;
        }
        std::wstring product = HidProduct(*colls[0]);
        std::wstring lower = product;
        for (auto& ch : lower) ch = (wchar_t)towlower(ch);
        if (!m.cable && lower.size() >= 6 && lower.compare(lower.size() - 6, 6, L"dongle") == 0) {
            ctx.Claim(kVid, m.pid);  // no headset linked: nothing to read (and the stored value is stale)
            firstSeen.erase(m.pid);
            continue;
        }
        int level = ReadLevel(*colls[0]);
        ULONGLONG now = GetTickCount64();
        if (!firstSeen.count(m.pid)) firstSeen[m.pid] = now;
        DeviceStatus s;
        s.key = L"audeze:maxwell";  // dongle and cable are one headset
        s.name = m.name;
        s.kind = DeviceKind::Headset;
        s.via = m.cable ? L"USB cable" : L"2.4 GHz dongle";
        s.graceSec = 300;
        if (level < 0 || (level == 0 && now - firstSeen[m.pid] < 90000)) {
            s.asleep = true;  // no reading yet (or the power-on 0%): keep the last value
        } else {
            s.percent = level;
            s.charging = m.cable;
        }
        ctx.Add(s, kVid, m.pid);
        for (const Model& o : kModels) ctx.Claim(kVid, o.pid);  // one entry for the headset
        return;
    }
}

}

void PollAudeze(PollContext& ctx) { wb_driver_audeze::PollAudeze(ctx); }
namespace wb_driver_aula {
// AULA F75 / F87 Pro / F99, MCHOSE (K99 V2, G75 Pro, X75 ...) and Redragon Horus Pro (K618RGB-PRO)
// wireless keyboards on the "BY" 2.4 GHz receivers: 3554:FA09 (Compx, shared by these brands) and MCHOSE's
// own 41E4:2001.
// MCHOSE K87 / K99 receivers also come as 05AC:024F (Apple's vendor id), the id SONiX dongles use.
// M HUB lists 1452/591 among its devices; the MCHOSE ones are told apart by layout:
// the BY collection on interface 1 (FF02:0002, 20-byte input and output reports) next to an
// FF04:0002 feature collection, where the SONiX dongles only have FF60:0061 on interface 3.
// Only that exact layout is probed on this id (BatteryHub issue #1417, MCHOSE K87).
// The receiver exposes a 20-byte vendor collection (usage page FF02 or FF04, usage 02). Battery is a
// 20-byte output report 0x13 carrying command 0x4A and a trailing sum checksum. The reply is
// another 0x13 frame whose command byte, with its top bit masked, is 0x4A. Its payload starts at
// byte 5: the level (1..100), then the state byte (low nibble: full, high nibble: charging; AULA
// reports 0x01 on battery and 0x10 with the cable in).
// The receiver also pushes unrelated 0x13 frames (a 0x0A status among them), so the report id
// alone does not identify the reply. MCHOSE firmware leaves the reply's trailer at 0 instead of a
// checksum; such a reply is accepted only as a single-packet frame (byte 2 = 1 packet, byte 3 = index 0).
// A keyboard that is dozing or has just switched to 2.4 GHz leaves the first request unanswered
// (one F75 capture timed out at exactly 1 s), so the request is resent every ~1.3 s within one
// 4-second deadline, the documented exception for slow links. Byte 2 alternates between 00 (PyFlat,
// sanshoku captures on the F75) and 01 (the "packet count" the Womier L65 tool sends on the same
// receiver); the reply is identified by command and checksum either way.
// Receivers named "2.4G Wireless Receiver" / "MCHOSE 2.4G Wireless" are first asked for their
// six-byte model code (command 0x05, read-only), as MCHOSE's M HUB does, to tell an MCHOSE model
// apart; MCHOSE models then get M HUB's exact battery frame (13 4A 01 ...).
// AULA protocol from github.com/PyFlat/Device-Battery-Info (PR 6), github.com/deepan-alve/womier-l65-linux
// (windows/l65w.ps1) and github.com/ushineko/sanshoku issue 36.
// MCHOSE receiver ids, model codes and frames from the official M HUB web driver
// (www.mchose.com.cn, assets/main-DQZs6ZzX.js: HidIndexDeviceFilters, getDevName, keyboardPwds,
// getPassWord, getBattery, getBatteryParser; vendorId 1452 / productId 591 with usage page FF00
// or FF04, getDevName "K87" / "K99" on 591, wireless report id 19 = 0x13), as also documented in
// github.com/wushuai0802/myfinder-device-battery (engine/upstream-protocols.md, sections
// "MCHOSE BY keyboard receiver 3554:FA09" and "MCHOSE 41E4:2001 and other 41E4 identities").
//
// AULA HERO 68 Air (and possibly other AULA keyboards) on the "8K Wireless Receiver" (VID 372E,
// PID 106C) speaks a different, acknowledged protocol on usage page FF60 / usage 61 (interface 2):
//  * 20-byte reports: 09 66, three control bytes, 14 payload bytes, checksum.
//    Byte 3 (low 7 bits) is the frame index 1..5, byte 4 (low 7 bits) the frame type:
//    0E data, 00 ACK, 08 trailer from the host, 07 trailer from the keyboard. The top bits of
//    bytes 2, 3 and 4 form a 3-bit counter that advances with every frame a side sends.
//    Data frame checksum: byte 19 = sum of bytes 1..18.
//  * A message is always 4 data frames (56 bytes) plus a trailer. Host trailer: byte 12 =
//    0xFF - sum of the 56 message bytes, byte 13 = sum of bytes 1..12.
//  * Every data and trailer frame is acknowledged by the other side: bytes 2..4 echoed with the
//    type bits cleared (counter bit kept), byte 5 = sum of bytes 1..4. Unacknowledged frames are
//    resent by the keyboard every ~100 ms.
//  * Battery request: message 09 87 00 00 01 00. Reply: 87 00 00 01 00 02 <level> <flags>; the 02
//    is constant (it does not change with charging). The keyboard also pushes FE 05 <level> <flags>
//    on its own. <flags> is 0x10 while charging on the cable and 0x00 on battery.
// From a USB capture of AULA Hub (BatteryHub issue #201); read-only.
//
// AULA SC580 / SC580X mouse: USB cable 088D:062E, 2.4 GHz receiver 089D:062F. The vendor collection is
// usage page FF02 / usage 0000 with 33-byte reports (report id 0 + 32-byte payload):
//  * payload: [0] command, [1] status (0 in requests), [2] packets (01), [3] data length,
//    [4..30] data, [31] = sum of bytes 4..30.
//  * Command 0x30 (GetDeviceInfo, read-only) is what the vendor app polls for the battery. The reply
//    echoes the command; status 0 OK, 1 busy (retry after 300 ms), 2 mouse offline (asleep or out of
//    range of the receiver), 3 data error (retry). Reply data (from byte 4): "M137" identity,
//    firmware (2), sensor id, unknown, charge state (0 on battery, 1 charging, 2 full), level 0..100,
//    online flag.
//  * sc580ctl notes that the receiver can answer a write with the reply to the previous write, so a reply
//    to another command means "send again".
//  * Observed on 089D:062F: the receiver answers 0x30 within a few ms from its own copy of the mouse
//    state, and pushes D1 <online> <level> <charging> whenever the link goes down or comes back.
//  * Observed with the cable in: the cable interface answers charge state 1 with level 100 regardless
//    of the real charge, and the receiver reports the mouse offline (it switches to the wired link).
// Protocol from github.com/Crucialjun/omarchy-aula-SC580 (PROTOCOL.md, bin/sc580ctl; MIT), reverse
// engineered from the vendor's CBMouse.exe (class CDevDM, MS.ini Protocol=3) and verified on an SC580.
// Tested here on an SC580 over the cable and the receiver.

namespace {

struct Receiver {
    USHORT vid, pid;
};
const Receiver kReceivers[] = { { 0x3554, 0xFA09 }, { 0x3554, 0xFB40 }, { 0x41E4, 0x2001 }, { 0x05AC, 0x024F } };

// 05AC:024F is shared with SONiX dongles, so only the MCHOSE layout is accepted there.
bool IsAppleId(USHORT vid, USHORT pid) { return vid == 0x05AC && pid == 0x024F; }

bool IsByCollection(const HidColl& c) {
    if (!IsAppleId(c.vid, c.pid))
        return (c.page() == 0xFF02 || c.page() == 0xFF04) && c.usage() == 0x0002 && c.outLen() >= 20 && c.inLen() >= 20;
    return c.iface == 1 && c.page() == 0xFF02 && c.usage() == 0x0002 && c.inLen() == 20 && c.outLen() == 20;
}

// The MCHOSE 05AC:024F receiver also has the FF04:0002 feature collection on interface 1.
bool HasMchoseLayout(const PollContext& ctx, USHORT vid, USHORT pid) {
    return !ctx.Find(vid, pid, [](const HidColl& c) {
                   return c.iface == 1 && c.page() == 0xFF04 && c.usage() == 0x0002 && c.featLen() > 0 &&
                          c.inLen() == 0 && c.outLen() == 0;
               }).empty();
}

// M HUB's keyboardPwds table: the six-byte model code returned by command 0x05.
struct Model {
    BYTE code[6];
    const wchar_t* name;
};
const Model kModels[] = {
    { { 0x03, 0x00, 0x00, 0x00, 0x02, 0x1C }, L"MCHOSE G87" },
    { { 0x03, 0x00, 0x00, 0x00, 0x00, 0x77 }, L"MCHOSE K99" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0xDF }, L"MCHOSE K99 V2" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0x32 }, L"MCHOSE G98" },
    { { 0x03, 0x00, 0x00, 0x00, 0x00, 0xD6 }, L"MCHOSE X75" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x03 }, L"MCHOSE G98 Pro" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0xC6 }, L"MCHOSE X75 V2" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0x15 }, L"MCHOSE G75 Pro" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x18 }, L"MCHOSE G98 V2" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x2B }, L"MCHOSE UT98" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x01 }, L"MCHOSE Z75" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x09 }, L"MCHOSE Z75S" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0xC7 }, L"MCHOSE KX75" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0x74 }, L"MCHOSE GX87" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0xED }, L"MCHOSE GX87 V2" },
    { { 0x03, 0x00, 0x00, 0x00, 0x01, 0x35 }, L"MCHOSE K87" },
    { { 0x03, 0x00, 0x00, 0x00, 0x03, 0x35 }, L"MCHOSE G75 V2" },
    { { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x34 }, L"MCHOSE G87 V2" },
    { { 0x03, 0x00, 0x00, 0x00, 0x00, 0x6A }, L"Redragon Horus Pro" },
    { { 0x03, 0x00, 0x00, 0x00, 0x03, 0xDA }, L"Redragon Horus Pro V2" },
};

BYTE Sum(const std::vector<BYTE>& b, size_t n) {
    unsigned s = 0;
    for (size_t i = 0; i < n; i++) s += b[i];
    return (BYTE)(s & 0xFF);
}

bool SendRequest(HidDev& d, BYTE cmd, BYTE packets) {
    std::vector<BYTE> req(20, 0);
    req[0] = 0x13;
    req[1] = cmd;
    req[2] = packets;
    req[19] = Sum(req, 19);
    return d.Write(req);
}

// Sends 13 <cmd> ... and waits for the matching reply, resending every 1.3 s within `totalMs`.
bool Exchange(HidDev& d, BYTE cmd, std::vector<BYTE>& r, int totalMs) {
    const ULONGLONG start = GetTickCount64(), end = start + totalMs;  // single absolute deadline
    const int kResendMs = 1300;
    int sent = 0;
    d.Flush();
    for (;;) {
        LONGLONG now = (LONGLONG)GetTickCount64();
        if (now >= (LONGLONG)end) return false;
        if (now >= (LONGLONG)start + (LONGLONG)sent * kResendMs) {
            if (!SendRequest(d, cmd, (BYTE)(sent & 1))) return false;
            ++sent;
        }
        LONGLONG nextSend = (LONGLONG)start + (LONGLONG)sent * kResendMs;
        int wait = (int)(std::min<LONGLONG>(nextSend, (LONGLONG)end) - now);
        if (wait < 1) wait = 1;
        ULONGLONG before = GetTickCount64();
        int n = d.Read(r, wait);
        // A read that fails at once is an I/O error (receiver unplugged), not a timeout: stop.
        if (n < 0 && (int)(GetTickCount64() - before) + 20 < wait) return false;
        if (n < 20) continue;  // timeout (resend or give up above) or a short unrelated report
        if (r[0] != 0x13) continue;
        if ((r[1] & 0x7F) != cmd) continue;  // the receiver also pushes unrelated 0x13 frames
        if (std::all_of(r.begin() + 4, r.begin() + 19, [](BYTE b) { return b == 0; })) continue;  // echo of a request
        bool checksum = r[19] == Sum(r, 19);
        bool mchose = r[19] == 0 && r[2] == 1 && r[3] == 0;  // MCHOSE leaves the trailer at 0
        if (!checksum && !mchose) continue;
        return true;
    }
}

// M HUB reads the model code from bytes 5..10. One user capture (myfinder-device-battery issue 476)
// had it one byte earlier, so byte 4 is tried too. A code that matches neither leaves the model unknown.
const wchar_t* IdentifyModel(HidDev& d) {
    std::vector<BYTE> r;
    if (!Exchange(d, 0x05, r, 1300)) return nullptr;
    for (size_t off : { (size_t)5, (size_t)4 })
        for (const auto& m : kModels)
            if (std::memcmp(&r[off], m.code, 6) == 0) return m.name;
    return nullptr;
}

// --- HERO 68 Air / 8K Wireless Receiver (372E:106C) ---

constexpr BYTE kHeroId = 0x09, kHeroMagic = 0x66;
constexpr BYTE kHeroData = 0x0E, kHeroAck = 0x00, kHeroTrailerHost = 0x08, kHeroTrailerDev = 0x07;
constexpr int kHeroDeadlineMs = 1000;  // the whole exchange takes ~30 ms when the keyboard is awake
constexpr int kHeroResendMs = 150;     // resend a frame the keyboard has not acknowledged yet

BYTE SumRange(const std::vector<BYTE>& b, size_t from, size_t to) {
    unsigned s = 0;
    for (size_t i = from; i < to; i++) s += b[i];
    return (BYTE)(s & 0xFF);
}

// Bytes 0..4 of a frame; the 3-bit counter lives in the top bits of bytes 2, 3 and 4.
std::vector<BYTE> HeroHeader(int counter, BYTE index, BYTE type) {
    std::vector<BYTE> f(20, 0);
    f[0] = kHeroId;
    f[1] = kHeroMagic;
    f[2] = (BYTE)(0x05 | ((counter & 4) ? 0x80 : 0));
    f[3] = (BYTE)(index | ((counter & 2) ? 0x80 : 0));
    f[4] = (BYTE)(type | ((counter & 1) ? 0x80 : 0));
    return f;
}

std::vector<BYTE> HeroDataFrame(int counter, BYTE index, const std::vector<BYTE>& msg) {
    std::vector<BYTE> f = HeroHeader(counter, index, kHeroData);
    for (int i = 0; i < 14; i++) f[5 + i] = msg[(index - 1) * 14 + i];
    f[19] = SumRange(f, 1, 19);
    return f;
}

std::vector<BYTE> HeroTrailer(int counter, const std::vector<BYTE>& msg) {
    std::vector<BYTE> f = HeroHeader(counter, 5, kHeroTrailerHost);
    f[12] = (BYTE)(0xFF - SumRange(msg, 0, msg.size()));
    f[13] = SumRange(f, 1, 13);
    return f;
}

std::vector<BYTE> HeroAck(const std::vector<BYTE>& r) {
    std::vector<BYTE> f(20, 0);
    f[0] = kHeroId;
    f[1] = kHeroMagic;
    f[2] = r[2];
    f[3] = r[3];
    f[4] = (BYTE)(r[4] & 0x80);
    f[5] = SumRange(f, 1, 5);
    return f;
}

bool IsAckFor(const std::vector<BYTE>& r, const std::vector<BYTE>& f) {
    return (r[4] & 0x7F) == kHeroAck && r[2] == f[2] && r[3] == f[3] && (r[4] & 0x80) == (f[4] & 0x80) &&
            r[5] == SumRange(r, 1, 5);
}

class HeroLink {
public:
    explicit HeroLink(HidDev& d) : d_(d), end_(GetTickCount64() + kHeroDeadlineMs) {}

    // Sends the 4 data frames and the trailer, each one only after the previous was acknowledged.
    bool SendMessage(const std::vector<BYTE>& msg) {
        int counter = 1;  // AULA Hub starts a session at 1 as well
        for (BYTE i = 1; i <= 4; i++, counter = (counter + 1) & 7)
            if (!SendFrame(HeroDataFrame(counter, i, msg))) return false;
        return SendFrame(HeroTrailer(counter, msg));
    }

    // Reads (and acknowledges) the keyboard's frames until the reply has been received in full.
    bool WaitForBattery() {
        std::vector<BYTE> r;
        while (!(level_ >= 0 && replyDone_)) {
            if (!ReadFrame(r, Remaining())) break;
        }
        return level_ >= 0;
    }

    int level() const { return level_; }
    bool charging() const { return flags_ >= 0 && (flags_ & 0x10) != 0; }

private:
    int Remaining() const {
        LONGLONG left = (LONGLONG)end_ - (LONGLONG)GetTickCount64();
        return left > 0 ? (int)left : 0;
    }

    bool SendFrame(const std::vector<BYTE>& f) {
        std::vector<BYTE> r;
        while (Remaining() > 0) {
            if (!d_.Write(f)) return false;
            ULONGLONG resendAt = GetTickCount64() + kHeroResendMs;
            for (;;) {
                LONGLONG wait = std::min<LONGLONG>((LONGLONG)resendAt - (LONGLONG)GetTickCount64(), Remaining());
                if (wait <= 0) break;
                if (!ReadFrame(r, (int)wait)) {
                    if (ioError_) return false;
                    break;
                }
                if (IsAckFor(r, f)) return true;
            }
        }
        return false;
    }

    // Reads one HERO frame, acknowledges keyboard data/trailer frames and picks up the battery.
    // Returns false on timeout or I/O error.
    bool ReadFrame(std::vector<BYTE>& r, int waitMs) {
        for (;;) {
            if (waitMs <= 0) return false;
            ULONGLONG before = GetTickCount64();
            int n = d_.Read(r, waitMs);
            int took = (int)(GetTickCount64() - before);
            // A read that fails at once is an I/O error (receiver unplugged), not a timeout: stop.
            if (n < 0 && took + 20 < waitMs) {
                ioError_ = true;
                return false;
            }
            if (n < 0) return false;
            waitMs -= took;
            if (n < 20 || r[0] != kHeroId || r[1] != kHeroMagic) continue;  // unrelated report
            BYTE type = r[4] & 0x7F;
            if (type == kHeroData) {
                if (r[19] != SumRange(r, 1, 19)) continue;  // damaged: no ACK, the keyboard resends
                d_.Write(HeroAck(r));
                if ((r[3] & 0x7F) == 1) Inspect(r);
            } else if (type == kHeroTrailerDev) {
                d_.Write(HeroAck(r));
                if (inReply_) replyDone_ = true;
                inReply_ = false;
            }
            return true;
        }
    }

    // First data frame of a keyboard message: payload starts at byte 5.
    void Inspect(const std::vector<BYTE>& r) {
        inReply_ = false;
        if (r[5] == 0x87 && r[11] >= 1 && r[11] <= 100) {  // reply to our request
            level_ = r[11];
            flags_ = r[12];
            inReply_ = true;
        } else if (r[5] == 0xFE && r[6] == 0x05 && r[7] >= 1 && r[7] <= 100 && level_ < 0) {
            level_ = r[7];  // pushed by the keyboard on its own; keep waiting for the real reply
            flags_ = r[8];
        }
    }

    HidDev& d_;
    ULONGLONG end_;
    int level_ = -1, flags_ = -1;  // flags: the byte after the level, 0x10 while charging
    bool inReply_ = false, replyDone_ = false, ioError_ = false;
};

bool QueryHeroBattery(HidDev& d, int& level, bool& charging) {
    std::vector<BYTE> msg(56, 0);
    msg[0] = 0x09;
    msg[1] = 0x87;  // battery
    msg[4] = 0x01;
    d.Flush();
    HeroLink link(d);
    if (!link.SendMessage(msg)) return false;
    if (!link.WaitForBattery()) return false;
    level = link.level();
    charging = link.charging();
    return true;
}

// Other keyboards share these receiver ids: the Redragon K673 comes on 3554:FA09 as well, ignores
// these frames, and every request wakes it right after it has gone to sleep (BatteryHub issue #191). A
// receiver that has never answered since it was plugged in is therefore asked less and less often
// once it has stayed silent for a while (5, 10, 20, 40 minutes, then hourly). One answer brings
// back the normal per-poll query; unplugging the receiver starts over.
struct ReceiverState {
    bool answered = false;
    int misses = 0, backoffs = 0;
    ULONGLONG firstMiss = 0, nextTry = 0;
};
constexpr int kMissesBeforeBackoff = 3;
constexpr ULONGLONG kBackoffAfterMs = 5 * 60 * 1000;  // keep asking for 5 minutes: the keyboard may be switched on late
constexpr ULONGLONG kFirstBackoffMs = 5 * 60 * 1000;
constexpr ULONGLONG kMaxBackoffMs = 60 * 60 * 1000;

// --- SC580 / SC580X mouse (cable 088D:062E, receiver 089D:062F) ---

struct MouseId {
    USHORT vid, pid;
    bool cable;
};
const MouseId kSc580[] = { { 0x088D, 0x062E, true }, { 0x089D, 0x062F, false } };

constexpr BYTE kScGetInfo = 0x30;
constexpr BYTE kScOk = 0, kScBusy = 1, kScOffline = 2, kScDataError = 3;
constexpr int kScDeadlineMs = 1000;  // the cable answers within a few ms, the receiver within ~100 ms
constexpr int kScResendMs = 250;

enum class ScResult { Ok, Offline, Failed };

// Sends 30 00 01 00 ... and waits for an OK reply, resending on busy / stale replies within one deadline.
ScResult Sc580Info(HidDev& d, std::vector<BYTE>& data) {
    std::vector<BYTE> req(33, 0);  // report id 0 + 32-byte payload; the checksum over empty data is 0
    req[1] = kScGetInfo;
    req[3] = 0x01;
    const ULONGLONG end = GetTickCount64() + kScDeadlineMs;
    auto left = [&] { return (int)std::max<LONGLONG>((LONGLONG)end - (LONGLONG)GetTickCount64(), 0); };
    d.Flush();
    std::vector<BYTE> r;
    while (left() > 0) {
        if (!d.Write(req)) return ScResult::Failed;
        ULONGLONG resendAt = GetTickCount64() + kScResendMs;
        bool resend = false;
        while (!resend) {
            int wait = (int)std::min<LONGLONG>((LONGLONG)resendAt - (LONGLONG)GetTickCount64(), left());
            if (wait <= 0) break;
            ULONGLONG before = GetTickCount64();
            int n = d.Read(r, wait);
            // A read that fails at once is an I/O error (device unplugged), not a timeout: stop.
            if (n < 0 && (int)(GetTickCount64() - before) + 20 < wait) return ScResult::Failed;
            if (n < 32) continue;
            if (r[0] == 0xD1 || r[0] == 0xD2) continue;  // pushed power / DPI notifications
            if (r[0] != kScGetInfo) {                   // the receiver's reply to an earlier request
                resend = true;
                continue;
            }
            switch (r[1]) {
            case kScOk:
                if (r[31] != SumRange(r, 4, 31)) continue;
                data.assign(r.begin() + 4, r.begin() + 31);
                return ScResult::Ok;
            case kScOffline:
                return ScResult::Offline;
            case kScBusy:
                Sleep((DWORD)std::min(300, left()));
                resend = true;
                break;
            case kScDataError:
            default:
                resend = true;
                break;
            }
        }
    }
    return ScResult::Failed;
}

void PollSc580(PollContext& ctx) {
    const std::wstring key = L"aula:sc580";  // one entry when the mouse charges on the cable next to its receiver
    for (const auto& m : kSc580) {
        if (ctx.IsClaimed(m.vid, m.pid)) continue;
        bool listed = false;
        for (const auto& o : ctx.out) listed |= o.key == key;
        if (listed) {  // the cable answered first: leave the receiver alone
            ctx.Claim(m.vid, m.pid);
            continue;
        }
        for (const HidColl* c : ctx.Find(m.vid, m.pid, [](const HidColl& c) {
                 return c.page() == 0xFF02 && c.usage() == 0x0000 && c.outLen() >= 33 && c.inLen() >= 33;
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            std::vector<BYTE> data;
            ScResult res = Sc580Info(d, data);
            if (res == ScResult::Failed) continue;
            ctx.Claim(m.vid, m.pid);
            // The receiver answers status 2 (and pushes D1 00) as soon as the mouse dozes (seen 20-60 s after
            // it was last moved) or is switched off, and status 0 again right after it wakes. That is reported as an
            // asleep entry so the tray keeps the last level: leaving it out made the mouse disappear once
            // graceSec ran out, e.g. after the PC spent longer than that in Modern Standby.
            bool offline = res == ScResult::Offline || data[10] == 0;
            if (!offline && data[9] > 100) break;
            DeviceStatus s;
            s.key = key;
            s.name = L"AULA SC580";
            s.kind = DeviceKind::Mouse;
            s.via = m.cable ? L"USB cable" : L"2.4 GHz receiver";
            if (offline) {
                s.asleep = true;
            } else if (data[8] == 1) {
                // While charging the mouse reports a fixed 100 (seen going 89 -> 100 the moment the cable
                // went in, and back to 94 when it came out), so the level is unknown: the tray shows the
                // last real level with the charging mark. Charge state 2 (full) keeps its 100.
                s.charging = true;
            } else {
                s.percent = data[9];
            }
            s.graceSec = 1800;  // only used when the receiver stops answering altogether
            ctx.out.push_back(s);
            break;
        }
    }
}

}  // namespace

void PollAula(PollContext& ctx) {
    PollSc580(ctx);

    static ReceiverState states[std::size(kReceivers)];
    for (size_t i = 0; i < std::size(kReceivers); i++) {
        USHORT vid = kReceivers[i].vid, pid = kReceivers[i].pid;
        ReceiverState& st = states[i];
        if (ctx.Find(vid, pid, [](const HidColl&) { return true; }).empty()) st = ReceiverState();  // unplugged
        if (ctx.IsClaimed(vid, pid)) continue;
        if (!st.answered && st.nextTry && GetTickCount64() < st.nextTry) continue;
        bool tried = false, answered = false;
        if (IsAppleId(vid, pid) && !HasMchoseLayout(ctx, vid, pid)) continue;  // a SONiX dongle
        for (const HidColl* c : ctx.Find(vid, pid, IsByCollection)) {
            HidDev d;
            if (!d.Open(*c)) continue;
            // The generic receiver names carry a model code (M HUB asks no other 3554 / 41E4 device).
            std::wstring product = HidProduct(*c);
            bool generic = product.find(L"2.4G Wireless Receiver") != std::wstring::npos ||
                           product.find(L"MCHOSE 2.4G Wireless") != std::wstring::npos;
            // M HUB names a 05AC:024F keyboard from its product string ("K87", "K99"), so ask for the
            // model code there whatever the name is.
            const wchar_t* model = generic || IsAppleId(vid, pid) ? IdentifyModel(d) : nullptr;
            bool mchose = vid == 0x41E4 || IsAppleId(vid, pid) || model != nullptr;
            std::vector<BYTE> r;
            tried = true;
            // Byte 2 alternates 00 / 01 on resends; M HUB sends 01, the AULA references 00.
            if (!Exchange(d, 0x4A, r, 4000)) continue;
            int level = r[5];
            if (level < 1 || level > 100) continue;
            answered = true;
            DeviceStatus s;
            s.key = L"aula:" + Hex4(vid) + L":" + Hex4(pid);
            if (model)
                s.name = model;
            else if (IsAppleId(vid, pid) && !generic && !product.empty())
                s.name = product;  // a product string that names the keyboard, as M HUB's getDevName uses
            else
                s.name = mchose ? L"MCHOSE wireless keyboard" : L"AULA wireless keyboard";
            s.kind = DeviceKind::Keyboard;
            s.via = L"2.4 GHz receiver";
            s.percent = level;
            s.charging = (r[6] >> 4) == 1;  // high nibble: AULA's 0x10 cable flag, M HUB's chargeStatus
            // An idle keyboard sometimes misses the 4 s deadline (4 of 15 polls on an F75 that was awake);
            // keep the last value instead of dropping the keyboard until the next poll.
            s.graceSec = 1800;
            ctx.Add(s, vid, pid);
            break;
        }
        if (answered) {
            st.answered = true;
        } else if (tried && !st.answered) {
            ULONGLONG now = GetTickCount64();
            if (!st.misses++) st.firstMiss = now;
            if (st.misses >= kMissesBeforeBackoff && now - st.firstMiss >= kBackoffAfterMs) {
                int doublings = std::min(st.backoffs++, 4);
                st.nextTry = now + std::min(kFirstBackoffMs << doublings, kMaxBackoffMs);
            }
        }
    }

    for (auto [vid, pid] : ctx.Products(0x372E)) {
        if (pid != 0x106C) continue;  // HERO 68 Air "8K Wireless Receiver"; other 372E products are unknown
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF60 && c.usage() == 0x0061 && c.outLen() >= 20 && c.inLen() >= 20;
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            int level = -1;
            bool charging = false;
            if (!QueryHeroBattery(d, level, charging)) continue;
            DeviceStatus s;
            s.key = L"aula:" + Hex4(vid) + L":" + Hex4(pid);
            s.name = L"AULA wireless keyboard";
            s.kind = DeviceKind::Keyboard;
            s.via = L"2.4 GHz receiver";
            s.percent = level;
            s.charging = charging;
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollAula(PollContext& ctx) { wb_driver_aula::PollAula(ctx); }

namespace wb_driver_aula_mouse {
// AULA X9 Ultra, verified on receiver 320F:22A0, FF1C:0092, 64-byte reports.
// This receiver ID is shared with V9 mice, so do not infer the model from the ID.
// Protocol reference: https://github.com/HeyOkay/HaloBattery/issues/325 and
// https://github.com/ahmedkhursheed23/HaloBattery/blob/main/providers/lofree.py
// Independently verified: START 01, battery 1A, END 02, report ID 04.
// Battery reply byte 8 = percent, byte 9 = 0 battery / 1 charging / 2 full.
// The receiver caches the level. A pushed AA link-down frame marks it asleep;
// lack of a heartbeat is NOT proof of disconnection. Zero is an uninitialised cache.
// Only read-state transactions; no settings, pairing or firmware commands.

namespace AulaMouseProtocol {
inline std::vector<unsigned char> Request(unsigned char cmd) {
    std::vector<unsigned char> r(64, 0);
    r[0] = 4;
    r[3] = cmd;
    return r;
}
inline bool IsHeartbeat(const std::vector<unsigned char>& r) {
    return r.size() >= 9 && r[0] == 4 && r[1] == 0xAA && r[2] == 0 && r[3] == 0xAA &&
           (r[8] == 0 || r[8] == 0xFF);
}
inline bool IsReply(const std::vector<unsigned char>& r, unsigned char cmd) {
    return r.size() >= 8 && r[0] == 4 && r[1] == 0 && r[2] == 0 && r[3] == cmd &&
           r[4] == 0 && r[5] == 0 && r[6] == 0 && r[7] == 0;
}
inline bool ParseBattery(const std::vector<unsigned char>& r, int& percent, bool& charging) {
    if (r.size() < 10 || !IsReply(r, 0x1A) || r[8] == 0 || r[8] > 100 || r[9] > 2) return false;
    percent = r[8];
    charging = r[9] != 0;
    return true;
}
}  // namespace AulaMouseProtocol

namespace {
constexpr USHORT kVid = 0x320F, kPid = 0x22A0;

bool WaitReply(HidDev& d, BYTE cmd, ULONGLONG end, std::vector<BYTE>& reply, bool& linkDown) {
    while (GetTickCount64() < end) {
        const LONGLONG left = (LONGLONG)end - (LONGLONG)GetTickCount64();
        if (left <= 0) break;
        std::vector<BYTE> r;
        if (d.Read(r, left > 100 ? 100 : (int)left) < 0) continue;
        if (AulaMouseProtocol::IsHeartbeat(r)) {
            linkDown = r[8] == 0;
            continue;
        }
        if (AulaMouseProtocol::IsReply(r, cmd) && (cmd != 0x1A || r.size() >= 10)) {
            reply = std::move(r);
            return true;
        }
    }
    return false;
}

bool ReadBattery(const HidColl& c, int& percent, bool& charging, bool& linkDown) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    // Entire transaction, including cleanup, is bounded to one second.
    const ULONGLONG end = GetTickCount64() + 1000;
    std::vector<BYTE> reply;
    if (!d.Write(AulaMouseProtocol::Request(1), 150)) return false;
    bool started = WaitReply(d, 1, end - 550, reply, linkDown);
    bool valid = false;
    if (started && d.Write(AulaMouseProtocol::Request(0x1A), 150) &&
        WaitReply(d, 0x1A, end - 200, reply, linkDown)) {
        valid = AulaMouseProtocol::ParseBattery(reply, percent, charging);
    }
    // End even after a missing acknowledgement, to leave no transaction open.
    if (d.Write(AulaMouseProtocol::Request(2), 100))
        WaitReply(d, 2, end, reply, linkDown);
    return valid;
}
}  // namespace

void PollAulaMouse(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kPid)) return;
    for (const HidColl* c : ctx.Find(kVid, kPid, [](const HidColl& c) {
             return c.iface == 1 && c.page() == 0xFF1C && c.usage() == 0x0092 &&
                    c.inLen() == 64 && c.outLen() == 64;
         })) {
        if (HidProduct(*c) != L"AULA-8K Dongle") continue;
        DeviceStatus s;
        s.key = L"aula_mouse:320F:22A0";
        s.name = L"AULA wireless mouse (X9 / V9)";
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.graceSec = 1800;
        bool linkDown = false;
        int percent = -1;
        bool charging = false;
        if (ReadBattery(*c, percent, charging, linkDown) && !linkDown) {
            s.percent = percent;
            s.charging = charging;
        } else {
            s.asleep = true;
        }
        ctx.Add(s, kVid, kPid);
        return;
    }
}

}

void PollAulaMouse(PollContext& ctx) { wb_driver_aula_mouse::PollAulaMouse(ctx); }
namespace wb_driver_cherry {
// CHERRY MX 3.0S Wireless on its 2.4 GHz dongle (USB 046A:00EA, "MX 3.0S Dongle") or on the USB
// cable (046A:00E3, "MX 3.0S Wireless Keyboard"). Over Bluetooth Windows' own level is used instead.
//
// Both expose a 64-byte vendor channel on usage page FF1C, usage 0092 (a second FF1C:0092
// collection with 9-byte reports is not used). The framing and the battery command come from the
// community tool B1Chat/BatteryWatch (MIT), which verified them on the CHERRY PIXIU 75 receiver
// (046A:01E4); the CHERRY MX 3.0S Wireless uses the same usage page, usage and report size.
//
//   frame:  [0] report ID 4, [1..2] checksum (LE) = sum(bytes[3..end]) & 0xFFFF, [3] type, [4..] payload
//   query:  type 0x1A, payload 03 00 00 55 (read-only status request)
//   reply:  [0] 4, [3] 0x1A, [8] battery percent, [9] non-zero while USB powered
//
// Verified on a real MX 3.0S Wireless through the dongle. The cable (00E3) uses the same channel layout
// and is assumed to answer the same query. The matching and the reply checks below are deliberately
// strict, so a device that answers differently is skipped rather than misreported.

namespace {

constexpr USHORT kVid = 0x046A;  // CHERRY
constexpr USHORT kPage = 0xFF1C;
constexpr USHORT kUsage = 0x0092;
constexpr size_t kLen = 64;  // report length including the report ID
constexpr BYTE kReportId = 0x04;
constexpr BYTE kStatusType = 0x1A;

struct Model {
    USHORT pid;
    const wchar_t* via;
};
// Cable first: when it is plugged in the keyboard is powered and awake, and the dongle may still be
// attached too. Only one entry is ever reported for the keyboard.
constexpr Model kModels[] = { { 0x00E3, L"USB cable" }, { 0x00EA, L"2.4 GHz receiver" } };

// The one and only frame this driver ever sends.
std::vector<BYTE> BuildQuery() {
    std::vector<BYTE> f(kLen, 0);
    f[0] = kReportId;
    f[3] = kStatusType;
    f[4] = 0x03;
    f[7] = 0x55;
    unsigned sum = 0;
    for (size_t i = 3; i < f.size(); ++i) sum += f[i];
    f[1] = (BYTE)(sum & 0xFF);
    f[2] = (BYTE)((sum >> 8) & 0xFF);
    return f;
}

bool IsStatusReply(const std::vector<BYTE>& r) { return r.size() >= 10 && r[0] == kReportId && r[3] == kStatusType; }

}  // namespace

// Sends the status query on one collection. Returns false when nothing usable answered.
static bool Query(const HidColl& c, std::vector<BYTE>& reply) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    const std::vector<BYTE> query = BuildQuery();

    // The dongle only answers while the keyboard is awake: two tries, well under a second in all.
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (!d.Write(query, 300)) return false;
        const ULONGLONG end = GetTickCount64() + 400;
        for (;;) {
            const LONGLONG left = (LONGLONG)end - (LONGLONG)GetTickCount64();
            if (left <= 0) break;
            std::vector<BYTE> r;
            if (d.Read(r, (int)left) < 0) break;
            if (IsStatusReply(r)) {
                reply = std::move(r);
                return true;
            }
        }
    }
    return false;
}

static void PollCherryXtrfy(PollContext& ctx);  // Cherry XTRFY 1K Mouse Receiver, below

static void PollCherryKeyboard(PollContext& ctx) {
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, m.pid, [](const HidColl& c) {
                 return c.page() == kPage && c.usage() == kUsage && c.inLen() == kLen && c.outLen() == kLen;
             })) {
            std::vector<BYTE> reply;
            if (!Query(*c, reply)) continue;  // keyboard off or asleep: show nothing (last value is kept for a while)

            DeviceStatus s;
            s.key = L"cherry:mx30s";  // same entry whichever way the keyboard is connected
            s.name = L"CHERRY MX 3.0S Wireless";
            s.kind = DeviceKind::Keyboard;
            s.via = m.via;
            if (reply[8] <= 100) {
                s.percent = reply[8];
                s.charging = reply[9] != 0;  // USB powered, i.e. plugged in
            } else {
                s.asleep = true;  // answered without a usable level
            }
            s.graceSec = 1800;  // the dongle stops answering while the keyboard sleeps
            ctx.Add(s, kVid, m.pid);
            return;
        }
    }
}

void PollCherry(PollContext& ctx) {
    PollCherryKeyboard(ctx);
    PollCherryXtrfy(ctx);
}

// ---------------------------------------------------------------------------
// Cherry XTRFY 1K Mouse Receiver (046A:0330) and the mouse paired to it.
//
// The receiver exposes a 17-byte vendor channel on usage page FF02, usage 0002 (in 17 / out 17).
// It speaks the shared CompX framing also used by the Corsair Sabre V2 Pro (1B1C:2B2A) and the
// Areson receivers: 17-byte frames, report ID 08, checksum = 0x55 - sum of the first 16 bytes.
// Two read-only commands are sent, nothing else:
//   query:  [08][cmd][00 ...][checksum]
//     03 = link status -> [6] non-zero while the mouse is connected
//     04 = GetPower    -> [6] percent, [7] charging, [8..9] millivolts (big endian)
//   reply:  arrives on the same collection.
// Captured live from a Cherry XTRFY 1K Mouse Receiver with the mouse at 95% / 4084 mV:
//   08 04 00 00 00 02 5f 00 0f f4 00 00 00 00 00 00 e5
namespace {

constexpr USHORT kXtrfyPid = 0x0330;
constexpr size_t kXtrfyLen = 17;
constexpr BYTE kXtrfyReportId = 0x08;
constexpr BYTE kXtrfyLink = 0x03;
constexpr BYTE kXtrfyPower = 0x04;

std::vector<BYTE> XtrfyQuery(BYTE cmd) {
    std::vector<BYTE> f(kXtrfyLen, 0);
    f[0] = kXtrfyReportId;
    f[1] = cmd;
    unsigned sum = 0;
    for (size_t i = 0; i + 1 < f.size(); ++i) sum += f[i];
    f[kXtrfyLen - 1] = (BYTE)(0x55 - sum);
    return f;
}

bool XtrfyChecksumOk(const std::vector<BYTE>& r) {
    if (r.size() != kXtrfyLen) return false;
    unsigned sum = 0;
    for (size_t i = 0; i + 1 < r.size(); ++i) sum += r[i];
    return (BYTE)(0x55 - sum) == r[kXtrfyLen - 1];
}

// One command/reply exchange on the FF02:0002 collection. Fills `r` when a well-formed reply to
// exactly this command arrived.
bool XtrfyExchange(HidDev& d, BYTE cmd, std::vector<BYTE>& r) {
    d.Flush();
    if (!d.Write(XtrfyQuery(cmd), 300)) return false;
    const ULONGLONG end = GetTickCount64() + 500;
    for (;;) {
        const LONGLONG left = (LONGLONG)end - (LONGLONG)GetTickCount64();
        if (left <= 0) return false;
        std::vector<BYTE> frame;
        if (d.Read(frame, (int)left) < 0) return false;
        if (frame.size() == kXtrfyLen && frame[0] == kXtrfyReportId && frame[1] == cmd && XtrfyChecksumOk(frame)) {
            r = std::move(frame);
            return true;
        }
    }
}

}  // namespace

static void PollCherryXtrfy(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kXtrfyPid)) return;
    for (const HidColl* c : ctx.Find(kVid, kXtrfyPid, [](const HidColl& c) {
             return c.page() == 0xFF02 && c.usage() == 0x0002 && c.inLen() == kXtrfyLen && c.outLen() == kXtrfyLen;
         })) {
        HidDev d;
        if (!d.Open(*c)) return;
        DeviceStatus s;
        std::vector<BYTE> r;
        bool answered = false;
        // Check whether the mouse is switched on and linked before asking it for power.
        if (XtrfyExchange(d, kXtrfyLink, r)) {
            answered = true;
            s.connected = r[6] != 0;
        }
        if ((!answered || s.connected) && XtrfyExchange(d, kXtrfyPower, r)) {
            answered = true;
            s.connected = true;
            if (r[6] <= 100) s.percent = r[6];
            s.charging = r[7] != 0;
            s.millivolts = (r[8] << 8) | r[9];
        } else if (answered && s.connected) {
            s.asleep = true;  // linked, but no usable power reply
        }
        if (!answered) return;  // silent: mouse switched off or asleep; the last value is kept
        if (!s.connected) { ctx.Claim(kVid, kXtrfyPid); return; }
        s.key = L"cherry:xtrfy";
        s.name = L"Cherry XTRFY wireless mouse";
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.graceSec = 1800;  // the UI keeps the last reading, dimmed, while the mouse sleeps
        ctx.Add(s, kVid, kXtrfyPid);
        return;
    }
}

}

void PollCherry(PollContext& ctx) { wb_driver_cherry::PollCherry(ctx); }
namespace wb_driver_coolermaster {
// Cooler Master wireless mice (VID 2516): MM712 (cable 0169) and MM731 (cable 0167). Both pair with
// the same "Cooler Master Mice Dongle" (01B7). Vendor interface 3, usage page FF0A, no report id;
// 32-byte reports on the dongle, 64-byte reports over the cable.
//   Request: [46][81][00] -> status report [47][81][model] ... [10] percent, [11] charging (0 = no),
//            [12] link (0 cable, 1 dongle). The mouse also sends it on its own when the level changes.
//   [2] tells the models apart on the shared dongle: 50 = MM712; 70 seen in an MM731 status report.
//   A sleeping or switched-off mouse does not answer; the last value is kept (graceSec).
// MM712 verified on hardware (2516:01B7). MM731: same protocol family per MM731-Controller; its battery
// bytes and model byte are not verified.
// Protocol: github.com/syntactic-spice/coolermaster-usb-protocol (docs/PROTOCOL.md) and
// github.com/riVFerd/MM731-Controller.
namespace {

const USHORT kDongle = 0x01B7;
const USHORT kMM712Cable = 0x0169;
const USHORT kMM731Cable = 0x0167;

const wchar_t* ModelName(USHORT pid, BYTE model) {
    if (pid == kMM712Cable || (pid == kDongle && model == 0x50)) return L"Cooler Master MM712";
    if (pid == kMM731Cable || (pid == kDongle && model == 0x70)) return L"Cooler Master MM731";
    return L"Cooler Master wireless mouse";
}

// Sends the status request and waits for the 47 81 reply.
bool QueryStatus(HidDev& d, std::vector<BYTE>& r) {
    d.Flush();
    if (!d.Write({ 0x00, 0x46, 0x81, 0x00 })) return false;
    ULONGLONG end = GetTickCount64() + 500;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(r, left) < 13) return false;
        if (r[0] == 0x47 && r[1] == 0x81) return true;
    }
}

}  // namespace

void PollCoolerMaster(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x2516)) {  // sorted: the cable ids come before the dongle
        if (pid != kDongle && pid != kMM712Cable && pid != kMM731Cable) continue;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.iface == 3 && c.page() == 0xFF0A; })) {
            HidDev d;
            std::vector<BYTE> r;
            if (!d.Open(*c) || !QueryStatus(d, r)) continue;
            DeviceStatus s;
            s.key = L"coolermaster:" + Hex4(pid);
            s.name = ModelName(pid, r[2]);
            s.kind = DeviceKind::Mouse;
            s.via = r[12] == 0 ? L"USB cable" : L"2.4 GHz receiver";
            if (r[10] >= 1 && r[10] <= 100) s.percent = r[10];
            s.charging = r[11] != 0;
            s.graceSec = 1800;
            // With the cable and the dongle both plugged in the mouse shows up twice; keep one.
            bool dup = false;
            for (auto& o : ctx.out) {
                if (o.name != s.name) continue;
                dup = true;
                o.charging |= s.charging;
            }
            if (dup) ctx.Claim(vid, pid);
            else ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollCoolerMaster(PollContext& ctx) { wb_driver_coolermaster::PollCoolerMaster(ctx); }

namespace wb_driver_corsair {
// Corsair wireless headsets, mice and keyboards (VID 1B1C).
//  * Void / Void Pro / Void Elite / HS70 / HS80 family: output C9 64 -> [2] % (bit 7 = mic up), [4] status
//    (0 offline, 1-2 ok, 4-5 charging). From github.com/Sapd/HeadsetControl.
//  * "Bragi" devices (Virtuoso, Void V2, HS80 MAX, Slipstream mice/keyboards): wire bytes
//    02 <target> 02 <property> = GET; reply [1] source, [2] 02, [3] status (0 ok), [4..7] value LE.
//    Property 0F = battery (0..1000), 10 = charge state (1 charging, 3 full), 12 = product id (as in OpenRGB). Target 08 is the device
//    or receiver itself, 09+ are devices paired to a receiver. GET is read-only; we never switch the
//    device into software mode.
//  * Compx mice (e.g. Corsair Sabre V2 Pro Ultralight Wireless, 1B1C:2B2A):
//    17-byte report ID 08, command 03 = online status, command 04 = battery.
//    Byte 6 = percent, byte 7 = charging, bytes 8-9 = millivolts, byte 16 = checksum.
//    Uses HID collection FF02:0002. Protocol reference: HeyOkay/HaloBattery
//    protocol documentation; shared framing also used by the existing Kysona driver.

namespace {

const USHORT kVoidPids[] = { 0x1B1C, 0x1B27, 0x0A14, 0x0A16, 0x0A17, 0x0A1D, 0x0A1A, 0x1B2A, 0x1B23,
                             0x1B29, 0x0A55, 0x0A51, 0x0A52, 0x0A38, 0x0A4F, 0x0A2B, 0x0A75, 0x0A56 };
const USHORT kBragiPids[] = { 0x2A08, 0x2A02, 0x0A97, 0x0A64, 0x0A62, 0x0A3E, 0x0A3D, 0x2B00 };

// Corsair mice using the Compx battery protocol.
struct CorsairCompxModel { USHORT pid; const wchar_t* name; };
const CorsairCompxModel kCorsairCompxModels[] = {
    { 0x2B2A, L"Corsair Sabre V2 Pro Ultralight" },
    { 0x2B34, L"Corsair Sabre v2 PRO WIRELESS MG" },  // untested
};
const CorsairCompxModel* FindCorsairCompxModel(USHORT pid) {
    for (const auto& m : kCorsairCompxModels) if (m.pid == pid) return &m;
    return nullptr;
}

bool Contains(const USHORT* b, size_t n, USHORT v) { return std::find(b, b + n, v) != b + n; }
bool PollVoid(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    std::vector<BYTE> r;
    if (!d.Write({ 0xC9, 0x64 })) return false;
    ULONGLONG end = GetTickCount64() + 1000;
    while (true) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(r, left) < 5) return false;
        break;
    }
    BYTE st = r[4];
    s.key = L"corsair:" + Hex4(c.pid);
    s.name = HidProduct(c);
    if (s.name.empty()) s.name = L"Corsair wireless headset";
    s.kind = DeviceKind::Headset;
    s.via = L"2.4 GHz receiver";
    s.connected = st != 0;
    s.charging = st == 4 || st == 5;
    if (st == 1 || st == 2 || s.charging) s.percent = Clamp100(r[2] & 0x7F);
    return true;
}
class Bragi {
public:
    explicit Bragi(const HidColl& c) : numbered_(c.outLen() == 64) { ok_ = d_.Open(c); }
    bool ok() const { return ok_; }
    // Returns true and sets `value` on success; `status` holds the reply status (0 = ok, 5 = no such property).
    // Two framings exist. "Prefixed" (older Bragi): 02 <target> 02 <prop>, reply 02 <source> 02 <status> <value>.
    // "Plain" (newer Slipstream receivers such as 1B1C:2B00, e.g. Scimitar Elite SE Wireless):
    // 00 <target> 02 <prop> (report ID 0), reply <source> 02 <status> <value>. The first framing that answers
    // is remembered for this handle.
    bool Get(BYTE target, BYTE prop, unsigned& value, int timeoutMs = 300) {
        if (mode_ != Plain && GetFramed(false, target, prop, value, timeoutMs)) { mode_ = Prefixed; return true; }
        if (mode_ == Prefixed) return false;
        if (GetFramed(true, target, prop, value, timeoutMs)) { mode_ = Plain; return true; }
        return false;
    }
private:
    bool GetFramed(bool plain, BYTE target, BYTE prop, unsigned& value, int timeoutMs) {
        std::vector<BYTE> req;
        if (plain) req = { 0x00, target, 0x02, prop };
        else req = numbered_ ? std::vector<BYTE>{ 0x02, target, 0x02, prop } : std::vector<BYTE>{ 0x00, 0x02, target, 0x02, prop };
        d_.Flush();
        if (!d_.Write(req)) return false;
        BYTE source = target == 0x08 ? 0x00 : 0x01;
        size_t o = plain ? 0 : 1;  // offset of the source byte in the reply
        ULONGLONG end = GetTickCount64() + timeoutMs;
        std::vector<BYTE> r;
        for (;;) {
            int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
            if (left <= 0) return false;
            int n = d_.Read(r, left);
            if (n < 0) return false;
            if ((size_t)n < o + 7) continue;
            if (r[o + 1] != 0x02 || r[o] != source) continue;  // notification or another app's reply
            if (r[o + 2] != 0x00) return false;
            value = r[o + 3] | (r[o + 4] << 8) | (r[o + 5] << 16) | ((unsigned)r[o + 6] << 24);
            return true;
        }
    }
    enum Mode { Unknown, Prefixed, Plain };
    HidDev d_;
    bool numbered_;
    bool ok_ = false;
    Mode mode_ = Unknown;
};

// Devices seen behind a receiver, identified by the product ID they report (property 12).
struct PairedModel { USHORT pid; const wchar_t* name; DeviceKind kind; };
const PairedModel kPairedModels[] = {
    { 0x1B7E, L"Corsair Dark Core RGB Pro SE", DeviceKind::Mouse },  // tested on Slipstream receiver 1BDC
};
const PairedModel* FindPaired(unsigned pid) {
    for (const auto& m : kPairedModels) if (m.pid == pid) return &m;
    return nullptr;
}

// Corsair Compx battery protocol (17-byte reports).
BYTE CorsairCompxChecksum(const BYTE* b) {
    unsigned sum = 0;
    for (int i = 0; i < 16; i++) sum += b[i];
    return (BYTE)(0x55 - sum);
}
bool CorsairCompxQuery(HidDev& d, BYTE cmd, std::vector<BYTE>& r) {
    std::vector<BYTE> req(17, 0);
    req[0] = 0x08;
    req[1] = cmd;
    req[16] = CorsairCompxChecksum(req.data());
    d.Flush();
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + 500;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(r, left) < 17) return false;
        if (r[0] == 0x08 && r[1] == cmd && r[16] == CorsairCompxChecksum(r.data()))
            return true;
    }
}
bool PollCorsairCompx(PollContext& ctx, const HidColl& c, USHORT vid, USHORT pid, const CorsairCompxModel& model) {
    HidDev d;
    if (!d.Open(c)) return false;
    DeviceStatus s;
    std::vector<BYTE> r;
    bool answered = false;
    // Check whether the mouse is connected to the receiver.
    if (CorsairCompxQuery(d, 0x03, r)) { answered = true; s.connected = r[6] != 0; }
    // If command 03 is unsupported, try the battery query directly.
    if ((!answered || s.connected) && CorsairCompxQuery(d, 0x04, r)) {
        answered = true;
        s.connected = true;
        if (r[6] <= 100) s.percent = r[6];
        s.charging = r[7] != 0;
        s.millivolts = (r[8] << 8) | r[9];
    } else if (answered && s.connected) {
        // The receiver reports the mouse online, but the battery query failed.
        s.asleep = true;
    }
    if (!answered) return false;
    if (!s.connected) { ctx.Claim(vid, pid); return true; }
    s.key = L"corsair:" + Hex4(pid);
    s.name = model.name;
    s.kind = DeviceKind::Mouse;
    s.via = L"2.4 GHz receiver";
    s.graceSec = 1800;
    ctx.Add(s, vid, pid);
    return true;
}

}  // namespace
void PollCorsair(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x1B1C)) {
        if (Contains(kVoidPids, std::size(kVoidPids), pid)) {
            for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return (c.page() == 0xFFC5 || c.page() == 0xFF00) && c.outLen() > 0; })) {
                DeviceStatus s;
                if (PollVoid(*c, s)) { ctx.Add(s, vid, pid); break; }
            }
            continue;
        }
        // Corsair mice using the Compx battery protocol.
        if (const CorsairCompxModel* model = FindCorsairCompxModel(pid)) {
            for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                return c.page() == 0xFF02 && c.usage() == 0x0002 && c.outLen() >= 17 && c.inLen() >= 17;
            })) {
                if (PollCorsairCompx(ctx, *c, vid, pid, *model)) break;
            }
            continue;
        }
        bool knownBragi = Contains(kBragiPids, std::size(kBragiPids), pid);
        auto colls = ctx.Find(vid, pid, [knownBragi](const HidColl& c) {
            bool sizes = (c.outLen() == 64 || c.outLen() == 65) && (c.inLen() == 64 || c.inLen() == 65);
            return sizes && (c.page() == 0xFF42 || (knownBragi && c.page() >= 0xFF00) || (knownBragi && c.iface == 4));
        });
        for (const HidColl* c : colls) {
            Bragi b(*c);
            if (!b.ok()) continue;
            bool any = false;
            std::wstring product = HidProduct(*c);
            // Paired devices behind a receiver first, then the device itself (wired / Bluetooth).
            for (BYTE target : { 0x09, 0x0A, 0x0B, 0x08 }) {
                if (target == 0x08 && any) break;
                unsigned level = 0;
                bool ok = b.Get(target, 0x0F, level);
                for (int retry = 0; ok && (level == 0 || level > 1000) && retry < 2; retry++) ok = b.Get(target, 0x0F, level);
                if (!ok || level > 1000) continue;
                DeviceStatus s;
                s.key = L"corsair:" + Hex4(pid) + L":" + std::to_wstring(target);
                s.name = product.empty() ? L"Corsair wireless device" : product;
                s.kind = GuessKind(s.name);
                // A receiver names itself, not the device behind it, and has mouse collections of its own.
                // Property 12 is the paired device's product id (read-only); Corsair headsets use 0Axx.
                unsigned devPid = 0;
                if (target != 0x08 && b.Get(target, 0x12, devPid)) {
                    if (const PairedModel* m = FindPaired(devPid)) {
                        s.name = m->name;
                        s.kind = m->kind;
                    } else if ((devPid >> 8) == 0x0A) {
                        s.kind = DeviceKind::Headset;
                        if (product.find(L"Receiver") != std::wstring::npos) s.name = L"Corsair wireless headset";
                    }
                }
                // Corsair headsets and their receivers use product ids 0Axx (HS80 RGB receiver 0A6B names
                // itself "... Wireless Gaming Receiver", and its collections look like a keyboard).
                if (s.kind == DeviceKind::Other && (pid >> 8) == 0x0A) s.kind = DeviceKind::Headset;
                if (s.kind == DeviceKind::Other) s.kind = KindFromCollections(ctx.colls, vid, pid);
                s.via = target == 0x08 ? L"USB / Bluetooth" : L"Corsair receiver";
                s.percent = Clamp100((int)(level + 5) / 10);
                unsigned charge = 0;
                if (b.Get(target, 0x10, charge)) s.charging = charge == 1 || charge == 3;
                ctx.out.push_back(s);
                any = true;
            }
            if (any) { ctx.Claim(vid, pid); break; }
        }
    }
}

}

void PollCorsair(PollContext& ctx) { wb_driver_corsair::PollCorsair(ctx); }

namespace wb_driver_dareu {
// DAREU EM901X receiver (260D:1074) and USB cable (260D:1090), verified on hardware.
// Battery query from the official EM901X AIM-WL driver v1.0 (GetPower / SendCommand); protocol analysis:
// github.com/wushuai0802/myfinder-device-battery/blob/main/engine/upstream-protocols.md
// Unlike the Kysona transport, Feature 08 is sent on FF02:0002 and Input 09 arrives on
// the sibling FF01:0000 collection. Only command 04 is needed; no initialization/settings.
// Reply: [09][04][status=0]...[6=percent][7=charging]. No reply checksum rule is established.
// Sharkoon Skiller SGM50W (260D:1125, BatteryHub issue #1377): Sharkoon's firmware updater for it is a Compx tool
// (DeviceVidPid=260d1124, the ID it flashes over the cable) and the device exposes the same FF02:0002 feature-17 /
// FF01:0000 input-17 pair, so the same query is sent. Not tested on hardware.
//
// DAREU A84 keyboard (260D:0022, SONiX chip). From the official "DAREU A84 THREE MODE MECHANICAL
// KEYBOARD V1.6" driver (dareu.com/pages/keyboard-driver): its launcher (DeviceDriver.exe, config
// `vid="260D" pid="0022"`) opens the device's interface 0 (path containing "mi_00", the keyboard
// collection with the 65-byte feature report) and, on a timer, sends one command only:
//   SetFeature [00][04 F6 00 ...] (65 bytes), then GetFeature report 0, three reads discarded,
//   then up to three reads until byte 15 is non-zero; byte 5 is the level in percent.
// That is the launcher's only HID command, used just for its battery icon. No charging flag is read.

// DAREU Togran protocol on receiver 260D:1131 and A980 Pro cable 260D:1148 (verified on hardware), read from DAREU's own driver
// feature report 0A (65 bytes) on the consumer-control collection: [0A][slot<<4][len][class][cmd], reply echoes class/cmd
// slot 0 class 00/A0 lists paired mice, slot n class 07/80 gives [7]=status [8]=level [9]=max, status 1 = charging
static bool TogranQuery(HidDev& dev, BYTE slot, BYTE len, BYTE cls, BYTE cmd, std::vector<BYTE>& r) {
    std::vector<BYTE> req(65, 0);
    req[0] = 0x0A;
    req[1] = (BYTE)(slot << 4);
    req[2] = len;
    req[3] = cls;
    req[4] = cmd;
    if (!dev.SetFeature(req)) return false;
    for (int tries = 0; tries < 15; tries++) {
        Sleep(30);
        if (!dev.GetFeature(0x0A, r) || r.size() < 11) return false;
        if ((r[1] & 0x0F) == 2 && r[3] == cls && r[4] == cmd) return true;
    }
    return false;
}

static void AddTogranBattery(PollContext& ctx, HidDev& dev, BYTE slot, USHORT pid, USHORT mousePid) {
    std::vector<BYTE> b;
    if (!TogranQuery(dev, slot, 4, 0x07, 0x80, b)) return;
    BYTE maxLevel = b[9] ? b[9] : 100;
    if (b[8] > maxLevel) return;
    DeviceStatus s;
    s.key = L"dareu:260D:" + Hex4(pid) + (pid == mousePid ? L"" : L":" + Hex4(mousePid));
    s.name = mousePid == 0x1148 ? L"DAREU A980 Pro" : L"DAREU " + Hex4(mousePid);
    s.kind = DeviceKind::Mouse;
    s.via = slot ? L"2.4 GHz receiver" : L"USB cable";
    s.percent = (int)b[8] * 100 / maxLevel;
    s.charging = b[7] == 1;
    ctx.Add(s, 0x260D, pid);
}

// receiver 1131 lists mice by slot, the A980 Pro on its cable (1148) answers the battery query on slot 0.
// while wired the receiver keeps the mouse listed with status 0, so it isn't shown twice
static void PollDareuTogran(PollContext& ctx) {
    for (USHORT pid : { USHORT(0x1131), USHORT(0x1148) }) {
        if (ctx.IsClaimed(0x260D, pid)) continue;
        auto colls = ctx.Find(0x260D, pid, [](const HidColl& c) {
            return c.page() == 0x000C && c.usage() == 1 && c.featLen() == 65;
        });
        if (colls.size() != 1) continue;
        HidDev dev;
        if (!dev.OpenFeatureOnly(*colls[0])) continue;
        if (pid != 0x1131) {
            AddTogranBattery(ctx, dev, 0, pid, pid);
            continue;
        }
        std::vector<BYTE> r;
        if (!TogranQuery(dev, 0, 6, 0x00, 0xA0, r)) continue;
        int count = r[7];
        for (int slot = 1; slot <= count && slot <= 5; slot++) {
            const BYTE* e = &r[8 + (slot - 1) * 3];
            if (e[0]) AddTogranBattery(ctx, dev, (BYTE)slot, pid, (USHORT)((e[1] << 8) | e[2]));
        }
    }
}

namespace {

void PollDareuA84(PollContext& ctx) {
    const USHORT vid = 0x260D, pid = 0x0022;
    if (ctx.IsClaimed(vid, pid)) return;
    auto colls = ctx.Find(vid, pid, [](const HidColl& c) {
        return c.iface == 0 && c.page() == 0x01 && c.usage() == 0x06 && c.featLen() == 65;
    });
    if (colls.size() != 1) return;
    HidDev d;
    if (!d.OpenFeatureOnly(*colls[0])) return;
    std::vector<BYTE> req(65, 0), r;
    req[1] = 0x04;
    req[2] = 0xF6;
    if (!d.SetFeature(req)) return;
    Sleep(10);
    for (int i = 0; i < 3; i++) {  // the official driver discards these reads
        d.GetFeature(0, r);
        Sleep(10);
    }
    for (int i = 0; i < 3; i++) {
        if (i) Sleep(10);
        if (!d.GetFeature(0, r) || r.size() < 16 || r[15] == 0) continue;
        if (r[5] > 100) return;
        DeviceStatus s;
        s.key = L"dareu:" + Hex4(vid) + L":" + Hex4(pid);
        s.name = L"DAREU A84";
        s.kind = DeviceKind::Keyboard;
        s.via = L"HID";
        s.percent = r[5];
        ctx.Add(s, vid, pid);
        return;
    }
}

}  // namespace

void PollDareu(PollContext& ctx) {
    PollDareuTogran(ctx);
    PollDareuA84(ctx);
    const USHORT vid = 0x260D;
    // Prefer a valid cable reading over potentially cached receiver telemetry.
    bool em901xRead = false;
    for (USHORT pid : { USHORT(0x1090), USHORT(0x1074), USHORT(0x1125) }) {
        if (ctx.IsClaimed(vid, pid) || (em901xRead && pid == 0x1074)) continue;
        auto inputs = ctx.Find(vid, pid, [](const HidColl& c) {
            return c.iface == 1 && c.page() == 0xFF01 && c.usage() == 0 && c.inLen() == 17;
        });
        auto features = ctx.Find(vid, pid, [](const HidColl& c) {
            return c.iface == 1 && c.page() == 0xFF02 && c.usage() == 2 && c.featLen() == 17;
        });
        // Do not pair arbitrary collections when multiple devices share a product ID.
        if (inputs.size() != 1 || features.size() != 1) continue;

        HidDev input, feature;
        if (!input.Open(*inputs[0]) || !feature.OpenFeatureOnly(*features[0])) continue;
        std::vector<BYTE> req(17, 0), r;
        req[0] = 0x08;
        req[1] = 0x04;
        req[16] = 0x49;  // 0x55 minus the sum of bytes 0..15
        input.Flush();
        if (!feature.SetFeature(req)) continue;
        ULONGLONG end = GetTickCount64() + 1000;
        for (int reports = 0; reports < 32; reports++) {
            int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
            if (left <= 0 || input.Read(r, left) < 0) break;
            if (r.size() != 17 || r[0] != 0x09 || r[1] != 0x04 || r[2] != 0 || r[6] > 100) continue;
            DeviceStatus s;
            s.key = L"dareu:" + Hex4(vid) + L":" + Hex4(pid);
            s.name = pid == 0x1125 ? L"Sharkoon Skiller SGM50W" : L"DAREU EM901X";
            s.kind = DeviceKind::Mouse;
            s.via = pid == 0x1090 ? L"USB cable" : L"2.4 GHz receiver";
            s.percent = r[6];
            s.charging = r[7] != 0;
            ctx.Add(s, vid, pid);
            em901xRead = pid != 0x1125;
            break;
        }
    }
}

}

void PollDareu(PollContext& ctx) { wb_driver_dareu::PollDareu(ctx); }
namespace wb_driver_darmoshark {
// Darmoshark M3 and M3S over their Telink receivers. Protocol reference:
// https://github.com/Barba2k2/darmoshark-m3-configurator/blob/main/PROTOCOL.md
// Feature report 51: opcode 03 reads the receiver's bond (vid, pid, link state of the paired mouse),
// opcode 06 the protocol version, work mode and battery. An input report 54 E4 02 means no link.
namespace {
constexpr USHORT kVid = 0x248A;
struct Model {
    USHORT receiver;
    USHORT mouse;
    const wchar_t* name;
};

constexpr Model kModels[] = {
    {0xFF30, 0xFF12, L"Darmoshark M3"},
    {0xFF31, 0xFF18, L"Darmoshark M3S"},
};
constexpr BYTE kFeature = 0x51;
constexpr BYTE kBond = 0x03;
constexpr BYTE kIdentify = 0x06;
constexpr ULONGLONG kReceiverTimeoutMs = 1000;
constexpr ULONGLONG kFirstAttemptMs = 300;
constexpr ULONGLONG kQueryTimeoutMs = 500;

USHORT ReadId(const std::vector<BYTE>& reply, size_t offset) {
    return static_cast<USHORT>(reply[offset] | (reply[offset + 1] << 8));
}

bool Query(HidDev& device, BYTE opcode, std::vector<BYTE>& reply, ULONGLONG receiverDeadline) {
    const auto start = GetTickCount64();
    if (start >= receiverDeadline) return false;
    const auto deadline = (std::min)(receiverDeadline, start + kQueryTimeoutMs);

    device.Flush();
    if (GetTickCount64() >= deadline || !device.SetFeature({kFeature, opcode})) return false;

    std::vector<BYTE> input;
    for (auto now = GetTickCount64(); now < deadline; now = GetTickCount64()) {
        const auto waitMs = static_cast<int>((std::min)(20ULL, deadline - now));
        if (device.Read(input, waitMs) >= 3 &&
            input[0] == 0x54 && input[1] == 0xE4 && input[2] == 2) return false;
        if (GetTickCount64() >= deadline) return false;
        if (device.GetFeature(kFeature, reply) && reply.size() >= 12 &&
            reply[0] == kFeature && reply[1] == opcode) return GetTickCount64() < deadline;
    }
    return false;
}

int ReadBattery(HidDev& device, USHORT mouse, ULONGLONG deadline) {
    // The bond query replaces any previous battery reply in the receiver's
    // feature buffer before we request another one.
    std::vector<BYTE> reply;
    if (!Query(device, kBond, reply, deadline) ||
        ReadId(reply, 3) != kVid || ReadId(reply, 5) != mouse || !reply[7]) return -1;
    if (!Query(device, kIdentify, reply, deadline) || reply[2] != 1 ||
        ReadId(reply, 4) != kVid || ReadId(reply, 6) != mouse || reply[11] > 100) return -1;
    return reply[11];
}

void PollModel(PollContext& ctx, const Model& model) {
    if (ctx.IsClaimed(kVid, model.receiver)) return;
    for (const HidColl* coll : ctx.Find(kVid, model.receiver, [](const HidColl& c) {
             return c.iface == 1 && c.page() == 0x008C && c.usage() == 1 &&
                    c.inLen() == 33 && c.outLen() == 33 && c.featLen() == 1001;
         })) {
        const auto deadline = GetTickCount64() + kReceiverTimeoutMs;
        HidDev device;
        if (!device.Open(*coll)) continue;

        // Leave time to reread the bond and retry if the first battery request goes unanswered.
        const auto firstDeadline = (std::min)(deadline, GetTickCount64() + kFirstAttemptMs);
        int percent = ReadBattery(device, model.mouse, firstDeadline);
        if (percent < 0 && GetTickCount64() < deadline) percent = ReadBattery(device, model.mouse, deadline);
        if (percent < 0) continue;

        DeviceStatus status;
        status.key = ctx.StableKey(L"darmoshark:" + Hex4(model.receiver));
        status.name = model.name;
        status.kind = DeviceKind::Mouse;
        status.via = L"2.4 GHz receiver";
        status.percent = percent;
        ctx.Add(status, kVid, model.receiver);
        return;
    }
}
}  // namespace

void PollDarmoshark(PollContext& ctx) {
    for (const auto& model : kModels) {
        PollModel(ctx, model);
    }
}

// Darmoshark M3S Pro on its Nordic "1K NRF Receiver" (1915:0723), verified on hardware.
// Protocol read from a USB capture of Darmoshark's software. Vendor collection: interface 3,
// usage page FF0A, 64-byte reports without a report id. All 64 bytes of a frame sum to 0xA1 (mod 256).
//   Request  [41][00][81][01] 0 ... [csum]
//   Reply    [41][00][8C][01][..][..][percent] ...
namespace {
constexpr USHORT kNrfVid = 0x1915;
constexpr USHORT kNrfReceiver = 0x0723;
constexpr size_t kNrfFrame = 64;
constexpr BYTE kNrfSum = 0xA1;
constexpr ULONGLONG kNrfTimeoutMs = 600;

BYTE NrfChecksum(const std::vector<BYTE>& frame) {
    unsigned sum = 0;
    for (size_t i = 0; i + 1 < kNrfFrame; ++i) sum += frame[i];
    return static_cast<BYTE>((kNrfSum - sum) & 0xFF);
}

bool NrfReadBattery(HidDev& device, int& percent) {
    std::vector<BYTE> request(kNrfFrame + 1, 0);  // [0] = report id 0
    request[1] = 0x41;
    request[3] = 0x81;
    request[4] = 0x01;
    request[kNrfFrame] = NrfChecksum(std::vector<BYTE>(request.begin() + 1, request.end()));

    device.Flush();
    if (!device.Write(request)) return false;

    const auto deadline = GetTickCount64() + kNrfTimeoutMs;
    std::vector<BYTE> reply;
    for (auto now = GetTickCount64(); now < deadline; now = GetTickCount64()) {
        const auto waitMs = static_cast<int>((std::min)(100ULL, deadline - now));
        if (device.Read(reply, waitMs) < static_cast<int>(kNrfFrame)) continue;
        if (reply[0] != 0x41 || reply[2] != 0x8C || reply[3] != 0x01) continue;
        if (reply[kNrfFrame - 1] != NrfChecksum(reply) || reply[6] > 100) continue;
        percent = reply[6];
        return true;
    }
    return false;
}
}  // namespace

void PollDarmosharkNrf(PollContext& ctx) {
    if (ctx.IsClaimed(kNrfVid, kNrfReceiver)) return;
    for (const HidColl* coll : ctx.Find(kNrfVid, kNrfReceiver, [](const HidColl& c) {
             return c.iface == 3 && c.page() == 0xFF0A && c.usage() == 1 &&
                    c.inLen() == 65 && c.outLen() == 65;
         })) {
        // 1915 is Nordic's vendor id, which other brands' receivers use too.
        if (HidProduct(*coll) != L"1K NRF Receiver") continue;
        HidDev device;
        int percent = 0;
        if (!device.Open(*coll) || !NrfReadBattery(device, percent)) continue;

        DeviceStatus status;
        status.key = L"darmoshark:" + Hex4(kNrfVid) + L":" + Hex4(kNrfReceiver);
        status.name = L"Darmoshark M3S Pro";
        status.kind = DeviceKind::Mouse;
        status.via = L"2.4 GHz receiver";
        status.percent = percent;
        ctx.Add(status, kNrfVid, kNrfReceiver);
        return;
    }
}

}

void PollDarmoshark(PollContext& ctx) { wb_driver_darmoshark::PollDarmoshark(ctx); }
void PollDarmosharkNrf(PollContext& ctx) { wb_driver_darmoshark::PollDarmosharkNrf(ctx); }
namespace wb_driver_endgame {
// Endgame Gear 4K wireless mice (OP1w 4k v1/v2, XM2w 4k v1/v2) on the shared 4K dongle 3367:1970.
// Protocol from github.com/johanneszab/vole (re/PROTOCOL.md, verified on OP1w 4k v1 and v2 with
// dongle FW 1.01) and github.com/Janlhin/Endgame_Gear_Web_Configurator (src/session.ts, transport.ts):
//   Config channel = vendor collection FF01:0002, feature report A1 (64 bytes, no checksum):
//     [A1][cmd][target][payload len][00 ...], payload from byte 16.
//   SET_FEATURE the request, wait ~150 ms, GET_FEATURE report A1. Reply byte 1 is the status:
//     01 ok, 03 busy (read again), 08 / 11 mouse asleep or out of range.
//   B4 (battery):    reply[16] = percent. No charging flag is decoded for this family.
//   0E (mouse info): reply[16..17] = 67 33 (VID), reply[18..19] = mouse PID, little endian.
//   0F (probe):      after a failed command the dongle stays silent until a 0F gets through, so we
//                    send it only on failure (sending it every poll keeps the mouse from sleeping).
// Only these read commands are used; the write / reset / re-pair commands are never sent.
// The older XM2we / OP1we "WE" family uses a different 17-byte protocol and is not handled here.
namespace {

const USHORT kVid = 0x3367;
const USHORT kDongle4K = 0x1970;

const wchar_t* MouseName(USHORT pid) {
    switch (pid) {
        case 0x1972: return L"Endgame Gear OP1w 4k";
        case 0x1984: return L"Endgame Gear OP1w 4k v2";
        case 0x1968: return L"Endgame Gear XM2w 4k";
        case 0x1982: return L"Endgame Gear XM2w 4k v2";
        default: return nullptr;
    }
}

// Sends one command and returns the reply (out[0] = A1). Returns the status byte, or -1 on I/O error.
int Command(HidDev& d, BYTE cmd, BYTE target, std::vector<BYTE>& r) {
    std::vector<BYTE> req(64, 0);
    req[0] = 0xA1;
    req[1] = cmd;
    req[2] = target;
    if (!d.SetFeature(req)) return -1;
    for (int attempt = 0; attempt < 3; ++attempt) {
        Sleep(150 + attempt * 100);
        if (!d.GetFeature(0xA1, r) || r.size() < 20 || r[0] != 0xA1) return -1;
        if (r[1] != 0x03) return r[1];  // 03 = busy, read again
    }
    return 0x03;
}

}  // namespace

void PollEndgame(PollContext& ctx) {
    static std::map<std::wstring, const wchar_t*> lastName;  // by dongle path: the mouse only identifies itself while awake
    if (ctx.IsClaimed(kVid, kDongle4K)) return;
    for (const HidColl* c : ctx.Find(kVid, kDongle4K, [](const HidColl& c) { return c.page() == 0xFF01 && c.usage() == 0x0002 && c.featLen() >= 64; })) {
        HidDev d;
        if (!d.OpenFeatureOnly(*c)) continue;
        std::vector<BYTE> r;
        int st = Command(d, 0xB4, 0x00, r);
        if (st != 0x01 && st != 0x08 && st != 0x11) {
            // Unexpected status or no answer: probe the dongle and the mouse once, then retry.
            std::vector<BYTE> p;
            Command(d, 0x0F, 0x01, p);
            Command(d, 0x0F, 0x0F, p);
            st = Command(d, 0xB4, 0x00, r);
        }
        bool awake = st == 0x01 && r[16] <= 100;
        bool asleep = st == 0x08 || st == 0x11;
        if (!awake && !asleep) continue;

        DeviceStatus s;
        s.key = L"endgame:" + Hex4(kDongle4K);
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.asleep = asleep;  // 08 / 11: the mouse sleeps or is off; shown dimmed with the last known value
        if (awake) {
            s.percent = r[16];
            std::vector<BYTE> info;
            if (Command(d, 0x0E, 0x00, info) == 0x01 && info[16] == 0x67 && info[17] == 0x33) {
                const wchar_t* n = MouseName((USHORT)(info[18] | (info[19] << 8)));
                if (n) lastName[c->path] = n;
            }
        }
        auto it = lastName.find(c->path);
        s.name = it != lastName.end() ? it->second : L"Endgame Gear 4k wireless mouse";
        ctx.Add(s, kVid, kDongle4K);
        break;
    }
}

}

void PollEndgame(PollContext& ctx) { wb_driver_endgame::PollEndgame(ctx); }
namespace wb_driver_endgame_we {
// Endgame Gear XM2we on its original 2.4 GHz receiver (3367:1961), not the newer 4K dongle (Endgame driver).
// Ported from HaloBattery (github.com/HeyOkay/HaloBattery, providers/endgame_we.py, MIT; verified there on
// hardware), itself from github.com/okms/xm2we-linux (PROTOCOL.md: framing, battery).
//   Request: 17-byte feature report 08 on FF02:0002: [08][op][00 x14][checksum], sum of all 17 bytes = 0x55.
//   Reply:   17-byte input report 09 on FF01:0000 of the same receiver: [09][op][00 00 00][size][payload].
//   op 04 = battery: payload [percent][charging 0/1]; op 03 = radio link: payload [1 = mouse linked].
// Only these two status queries are sent; no configuration or EEPROM writes.
namespace {

constexpr USHORT kVid = 0x3367, kPid = 0x1961;
constexpr BYTE kBattery = 0x04, kLink = 0x03;

std::vector<BYTE> Frame(BYTE op) {
    std::vector<BYTE> d(17, 0);
    d[0] = 0x08;
    d[1] = op;
    unsigned sum = 0;
    for (int i = 0; i < 16; i++) sum += d[i];
    d[16] = (BYTE)(0x55 - sum);
    return d;
}

bool Payload(const std::vector<BYTE>& r, int n, BYTE op, std::vector<BYTE>& out) {
    if (n < 17 || r.size() < 17 || r[0] != 0x09 || r[1] != op || r[2] || r[3] || r[4]) return false;
    unsigned sum = 0;
    for (int i = 0; i < 17; i++) sum += r[i];
    if ((sum & 0xFF) != 0x55) return false;
    int size = r[5];
    if (size < 1 || size > 10) return false;
    out.assign(r.begin() + 6, r.begin() + 6 + size);
    return true;
}

bool Query(HidDev& feat, HidDev& in, BYTE op, std::vector<BYTE>& out) {
    in.Flush();
    if (!feat.SetFeature(Frame(op))) return false;
    ULONGLONG end = GetTickCount64() + 400;
    std::vector<BYTE> r;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) return false;
        int n = in.Read(r, left);
        if (n < 0) return false;
        if (Payload(r, n, op, out)) return true;
    }
}

// The receiver part of a collection path: the two collections of one receiver differ only in "&colNN"
// and the last "&NNNN" of the instance part.
std::wstring ReceiverKey(const std::wstring& path) {
    std::wstring p = path;
    for (auto& ch : p) ch = (wchar_t)towlower(ch);
    size_t a = p.find(L'#'), b = a == std::wstring::npos ? a : p.find(L'#', a + 1);
    size_t e = b == std::wstring::npos ? b : p.find(L'#', b + 1);
    if (e == std::wstring::npos) return p;
    std::wstring hw = p.substr(a + 1, b - a - 1), inst = p.substr(b + 1, e - b - 1);
    size_t col = hw.find(L"&col");
    if (col != std::wstring::npos) hw.erase(col, 6);
    size_t amp = inst.rfind(L'&');
    if (amp != std::wstring::npos) inst.erase(amp);
    return hw + L"#" + inst;
}

}  // namespace

void PollEndgameWe(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kPid)) return;
    auto ins = ctx.Find(kVid, kPid, [](const HidColl& c) { return c.page() == 0xFF01 && c.usage() == 0x0000 && c.inLen() >= 17; });
    for (const HidColl* f : ctx.Find(kVid, kPid, [](const HidColl& c) { return c.page() == 0xFF02 && c.usage() == 0x0002 && c.featLen() >= 17; })) {
        const HidColl* inColl = nullptr;
        for (const HidColl* i : ins)
            if (ReceiverKey(i->path) == ReceiverKey(f->path)) inColl = i;
        if (!inColl) continue;  // without the reply collection nothing is sent
        HidDev feat, in;
        if (!feat.OpenFeatureOnly(*f) || !in.Open(*inColl)) continue;
        std::vector<BYTE> battery, link;
        bool gotBattery = Query(feat, in, kBattery, battery);  // battery first: the link can drop as the charger goes in
        bool gotLink = Query(feat, in, kLink, link);
        if (!gotBattery || battery[0] > 100) continue;
        DeviceStatus s;
        s.key = L"endgame:1961";
        s.name = L"Endgame Gear XM2we";
        s.kind = DeviceKind::Mouse;
        s.via = L"2.4 GHz receiver";
        s.graceSec = 1800;
        bool charging = battery.size() > 1 && battery[1] == 1;
        if (charging || (gotLink && link[0] == 1)) {
            s.percent = battery[0];
            s.charging = charging;
        } else {
            s.asleep = true;  // the receiver answered, the mouse is not linked: keep the last value
        }
        ctx.Add(s, kVid, kPid);
        return;
    }
}

}

void PollEndgameWe(PollContext& ctx) { wb_driver_endgame_we::PollEndgameWe(ctx); }
namespace wb_driver_endorfy {
// ENDORFY Viro Wireless headset on its 2.4 GHz dongle (3738:0072), BatteryHub issue #1398.
// No ENDORFY source documents this dongle. Its HID layout is the stock Airoha USB dongle one: a single
// vendor collection FF13:0001 on interface 3 with 62-byte input and output reports (report 06 out,
// report 07 in), next to the telephony and consumer collections. That is the layout the Audeze Maxwell,
// Skullcandy PLYR, Alienware AW920H and HyperX Cloud III dongles show, so it is read with Airoha's
// generic RACE "get battery" query, the one other Airoha 2.4 GHz dongles answer:
//  * HaloBattery (github.com/HeyOkay/HaloBattery, MIT): providers/skullcandy.py, taken from the Headroom
//    project (patalbansishashank/Headroom, bin/headroom_race.py, recovered from the Airoha SDK sources
//    Skull-HQ ships, confirmed on 34F0:5310 and on a Skull-HQ capture of 34F0:3210), and
//    providers/audeze.py (from HeadsetControl, lib/devices/audeze_maxwell.hpp), which sends the same packet.
//  * Request, output report 06, 62 bytes: 06 07 80 05 5A 03 00 D6 0C 00
//      [06][frame length][recipient 0x80 = the headset behind the dongle]
//      [RACE frame 05 5A <u16le len = 3> <u16le opcode 0x0CD6> <role 00 = agent>]
//  * Reply, input report 07 read with GetInputReport: [07][length][recipient] then RACE frames
//    05 <type> <u16le len> <u16le opcode> <payload>. The battery is the 0x5D indication of opcode 0x0CD6
//    with payload [status 0][role][percent]; it can follow the 0x5B acknowledgement in the same report.
//    There is no charging flag.
// The query only reads. If the headset is off, or the dongle does not answer this query, nothing is
// shown and the dongle is claimed so no other driver writes to it.
namespace {

constexpr USHORT kVid = 0x3738, kViroDongle = 0x0072;
constexpr BYTE kOutId = 0x06, kInId = 0x07;
constexpr size_t kReportLen = 62;
constexpr BYTE kRaceHead = 0x05, kRaceIndication = 0x5D;
constexpr USHORT kOpBattery = 0x0CD6;
constexpr int kReads = 12, kReadDelayMs = 60;  // the answer arrives within ~40 ms on other Airoha dongles

bool IsVendorCollection(const HidColl& c) {
    return c.page() == 0xFF13 && c.usage() == 0x0001 && c.inLen() == kReportLen && c.outLen() == kReportLen;
}

// The percent carried by an input report 07, or -1 when it holds no battery indication.
int BatteryPercent(const std::vector<BYTE>& r) {
    if (r.size() < 9 || r[0] != kInId) return -1;
    size_t i = 3;
    while (i + 6 <= r.size()) {
        if (r[i] != kRaceHead) { i++; continue; }
        size_t len = r[i + 2] | (r[i + 3] << 8);
        if (len < 2 || i + 4 + len > r.size()) break;
        USHORT op = (USHORT)(r[i + 4] | (r[i + 5] << 8));
        if (r[i + 1] == kRaceIndication && op == kOpBattery && len >= 5 && r[i + 6] == 0 && r[i + 8] <= 100)
            return r[i + 8];
        i += 4 + len;
    }
    return -1;
}

int ReadLevel(const HidColl& c) {
    HidDev d;
    if (!d.Open(c)) return -1;
    // Report 07 still holds the previous answer, so a read identical to it is not taken as the new one
    // (except the last read, so a steady level is still shown).
    std::vector<BYTE> before;
    if (!d.GetInputReport(kInId, before)) before.clear();
    std::vector<BYTE> req(kReportLen, 0);
    const BYTE ask[] = { kOutId, 0x07, 0x80, kRaceHead, 0x5A, 0x03, 0x00, 0xD6, 0x0C, 0x00 };
    std::copy(std::begin(ask), std::end(ask), req.begin());
    if (!d.Write(req)) return -1;
    for (int i = 0; i < kReads; i++) {
        Sleep(kReadDelayMs);
        std::vector<BYTE> r;
        if (!d.GetInputReport(kInId, r)) continue;
        if (!before.empty() && r == before && i + 1 < kReads) continue;
        int level = BatteryPercent(r);
        if (level >= 0) return level;
    }
    return -1;
}

}  // namespace

void PollEndorfy(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kViroDongle)) return;
    for (const HidColl* c : ctx.Find(kVid, kViroDongle, IsVendorCollection)) {
        int level = ReadLevel(*c);
        ctx.Claim(kVid, kViroDongle);
        if (level < 0) return;  // headset off or no answer: leave it out, the tray keeps the last level for graceSec
        DeviceStatus s;
        s.key = L"endorfy:viro";
        s.name = L"ENDORFY Viro Wireless";
        s.kind = DeviceKind::Headset;
        s.via = L"2.4 GHz dongle";
        s.percent = level;
        s.graceSec = 180;
        ctx.Add(s, kVid, kViroDongle);
        return;
    }
}

}

void PollEndorfy(PollContext& ctx) { wb_driver_endorfy::PollEndorfy(ctx); }
namespace wb_driver_fractal {
// Fractal Design Scape headset (VID 36BC): 2.4 GHz dongle (PID 0001) or the headset itself on its
// USB-C cable (PID 2001). Both expose a 64-byte vendor collection on usage page FF00 / usage 01.
//
// Requests are output report 0x02: [0x02][device type][command]. The reply is an input report 0x02
// echoing the device type and command, followed by the payload.
//   GET_DEVICE_STATE = 0x21, sent to device type 0x11 (dongle) or 0xF1 (headset).
//   Dongle reply:  payload byte 3 = 1 when the headset is linked to it.
//   Headset reply: payload byte 14 = battery level in percent. Through the dongle the request is
//                  relayed to the headset over the air; on the cable it is answered directly.
// Byte offsets count from the device type (WebHID's DataView, which excludes the report id), so the
// hidapi-style buffer below is shifted by one.
//
// Source: Fractal's own WebHID configurator, Adjust Pro (adjust.fractal-design.com): classes
// BTDongle / Headset, getUpdatedDeviceState(), constants P911.REPORT_ID = 2,
// DEVICE_TYPE_FACTORY_DONGLE = 17, DEVICE_TYPE_FACTORY_HEADSET = 241, GET_DEVICE_STATE_CMD = 33.
// Adjust Pro does not read a charging flag. Payload byte 22 reads 1 with the headset on its USB cable
// and 0 over the air (checked on a Scape, firmware as shipped in 2026); it is used as the charging bit.
// The headset unlinks from the dongle while it sits on its charging stand, so it then shows as off.
namespace {

constexpr BYTE kReportId = 0x02;
constexpr BYTE kTypeDongle = 0x11;
constexpr BYTE kTypeHeadset = 0xF1;
constexpr BYTE kGetDeviceState = 0x21;

// Sends GET_DEVICE_STATE to `type` and waits for its reply (one absolute 1 s deadline).
bool GetState(HidDev& d, BYTE type, std::vector<BYTE>& r) {
    d.Flush();
    if (!d.Write({ kReportId, type, kGetDeviceState })) return false;
    ULONGLONG end = GetTickCount64() + 1000;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) return false;
        int n = d.Read(r, left);
        if (n < 0) return false;
        if (n >= 24 && r[0] == kReportId && r[1] == type && r[2] == kGetDeviceState) return true;
    }
}

}  // namespace

void PollFractal(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x36BC)) {
        if (pid != 0x0001 && pid != 0x2001) continue;
        bool cable = pid == 0x2001;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF00 && c.usage() == 0x0001 && c.outLen() >= 4 && c.inLen() >= 24;
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            DeviceStatus s;
            s.key = L"fractal:scape";  // same entry on the dongle and on the cable
            s.name = L"Fractal Scape";
            s.kind = DeviceKind::Headset;
            s.via = cable ? L"USB cable" : L"2.4 GHz dongle";
            std::vector<BYTE> r;
            if (!cable) {
                if (!GetState(d, kTypeDongle, r)) continue;
                if (r[4] != 1) {  // dongle answers, headset off, out of range or on its stand
                    s.connected = false;
                    ctx.Add(s, vid, pid);
                    break;
                }
            }
            if (!GetState(d, kTypeHeadset, r) || r[15] > 100) continue;
            s.percent = r[15];
            s.charging = r[23] == 1;
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollFractal(PollContext& ctx) { wb_driver_fractal::PollFractal(ctx); }
namespace wb_driver_gamesir {
// GameSir Nova Lite / Nova 2 / Nova 2 Lite on the 2.4 GHz receiver (3537:1098) or a USB cable.
// XInput reports these as a wired pad, so the battery is read from the vendor collection
// (usage page FF7A, 64-byte reports, no report ID) with the "device info" query GameSir's own
// client uses:
//   Request: 01 01
//   Reply:   [0]=01, [1..5] receiver address, [10..11] receiver firmware, [18] battery:
//            0..100 percent, FF = charging (no percentage while on the cable)
// Unsolicited "F4 00" state frames arrive on the same collection and are skipped.
// Protocol reference: github.com/WWWyyy452/GamesirBatteryTray and
// github.com/AJAY0406/Gamesir-Nova-2-Lite-Battery-Detection (both MIT).

namespace {

const USHORT kVid = 0x3537;

// 1098: receiver with a controller linked (it re-enumerates under another id when the pad is off).
// 100F: Nova 2 Lite on a cable.
bool IsNova(USHORT pid) { return pid == 0x1098 || pid == 0x100F || pid == 0x1040; }  // 1040: Nova Lite 2 receiver (same FF7A collection; untested)

}  // namespace

void PollGameSir(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(kVid)) {
        if (!IsNova(pid)) continue;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.page() == 0xFF7A && c.outLen() >= 3; })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            d.Flush();
            if (!d.Write({ 0x00, 0x01, 0x01 })) continue;
            std::vector<BYTE> r;
            bool found = false;
            ULONGLONG end = GetTickCount64() + 800;
            while (!found && (LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
                if (d.Read(r, 200) < 19) continue;
                if (r[0] != 0x01) continue;  // state frame (F4 00 ...) or anything else
                found = true;
            }
            if (!found || (r[18] > 100 && r[18] != 0xFF)) continue;
            DeviceStatus s;
            s.key = L"gamesir:" + Hex4(pid);
            s.name = L"GameSir Nova";
            s.kind = DeviceKind::Gamepad;
            s.via = pid == 0x100F ? L"USB cable" : L"2.4 GHz receiver";
            if (r[18] == 0xFF) s.charging = true;  // percent stays -1: the last known value is kept
            else s.percent = r[18];
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollGameSir(PollContext& ctx) { wb_driver_gamesir::PollGameSir(ctx); }

namespace wb_driver_generic {
// Fallbacks that work for any vendor:
//  * HID devices exposing a standard battery usage (Generic Device Controls "Battery Strength",
//    Battery System "Absolute State Of Charge") in a feature or input report.
//  * Bluetooth devices: Windows keeps the level it reads over HFP (classic) or the GATT Battery
//    Service (LE) in a device property, the same value Settings > Bluetooth & devices shows.
namespace {

bool IsBatteryUsage(USHORT page, USAGE usage) {
    return (page == 0x06 && usage == 0x20) || (page == 0x85 && usage == 0x65);
}

const DEVPROPKEY kBtBattery = { { 0x104EA319, 0x6EE2, 0x4701, { 0xBD, 0x47, 0x8D, 0xDB, 0xF4, 0x25, 0xBB, 0xE5 } }, 2 };
const DEVPROPKEY kIsConnected = { { 0x83DA6326, 0x97A6, 0x4088, { 0x94, 0x53, 0xA1, 0x92, 0x3F, 0x57, 0x3B, 0x29 } }, 15 };
const DEVPROPKEY kClassOfDevice = { { 0x2BD67D8B, 0x8BEB, 0x48D5, { 0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A } }, 10 };

DeviceKind KindFromClassOfDevice(UINT32 cod) {
    UINT32 major = (cod >> 8) & 0x1F, minor = (cod >> 2) & 0x3F;
    if (major == 0x02) return DeviceKind::Phone;
    if (major == 0x04) return DeviceKind::Headset;
    if (major == 0x05) {
        if ((minor & 0x0F) == 0x01 || (minor & 0x0F) == 0x02) return DeviceKind::Gamepad;
        if ((minor >> 4) == 1) return DeviceKind::Keyboard;
        if ((minor >> 4) == 2) return DeviceKind::Mouse;
    }
    return DeviceKind::Other;
}

std::wstring StrProp(HDEVINFO di, SP_DEVINFO_DATA* dd, const DEVPROPKEY& key) {
    DEVPROPTYPE t;
    wchar_t buf[256] = {};
    if (SetupDiGetDevicePropertyW(di, dd, &key, &t, (PBYTE)buf, sizeof(buf) - sizeof(wchar_t), nullptr, 0) && t == DEVPROP_TYPE_STRING)
        return buf;
    return L"";
}

// Container properties like the category are string lists; only the first entry matters here.
std::wstring FirstStringProp(HDEVINFO di, SP_DEVINFO_DATA* dd, const DEVPROPKEY& key) {
    DEVPROPTYPE t;
    wchar_t buf[256] = {};
    if (SetupDiGetDevicePropertyW(di, dd, &key, &t, (PBYTE)buf, sizeof(buf) - sizeof(wchar_t), nullptr, 0) &&
        (t == DEVPROP_TYPE_STRING_LIST || t == DEVPROP_TYPE_STRING))
        return buf;
    return L"";
}

// LE devices (BTHLE\DEV_...) usually have no Class of Device, but Windows still files them under a
// category for Settings > Bluetooth & devices.
DeviceKind KindFromCategory(const std::wstring& c) {
    if (c == L"Communication.Phone" || c.rfind(L"Communication.Phone.", 0) == 0) return DeviceKind::Phone;
    if (c == L"Input.Keyboard") return DeviceKind::Keyboard;
    if (c == L"Input.Mouse") return DeviceKind::Mouse;
    if (c == L"Input.Gaming") return DeviceKind::Gamepad;
    if (c.rfind(L"Communication.Headset", 0) == 0 || c == L"Audio.Headphones") return DeviceKind::Headset;
    return DeviceKind::Other;
}

// Bluetooth LE keyboards that also expose a mouse collection (for the knob or macros), so
// KindFromCollections would call them a mouse. Matched on the exact HID vid/pid seen on the LE link:
// 3151:4013 Attack Shark K86 (Rongyuan firmware), 19F5:324A NuPhy WH80.
DeviceKind KindFromBleIds(USHORT vid, USHORT pid) {
    if (vid == 0x3151 && pid == 0x4013) return DeviceKind::Keyboard;
    if (vid == 0x19F5 && pid == 0x324A) return DeviceKind::Keyboard;
    return DeviceKind::Other;
}

// Bluetooth entries are identified by their container id (two Xbox controllers can share a name);
// devices reported by other drivers, which can also say via "Bluetooth", are matched by name.
bool IsDuplicateBluetoothDevice(const DeviceStatus& candidate, const std::vector<DeviceStatus>& existing) {
    for (const auto& other : existing) {
        if (other.key == candidate.key) return true;
        if (other.key.rfind(L"bt:", 0) != 0 && other.name == candidate.name) return true;
    }
    return false;
}

std::wstring StripServiceSuffix(std::wstring n) {
    for (const wchar_t* suf : { L" Hands-Free AG", L" Hands-Free", L" Avrcp Transport", L" Stereo", L" Audio Gateway" }) {
        size_t l = wcslen(suf);
        if (n.size() > l && n.compare(n.size() - l, l, suf) == 0) return n.substr(0, n.size() - l);
    }
    return n;
}

// Container id ("{...}") of the physical device an HID interface belongs to, formatted like
// PollBluetooth's keys, or "" if unknown.
std::wstring ContainerOf(const std::wstring& path) {
    HDEVINFO di = SetupDiCreateDeviceInfoList(nullptr, nullptr);
    if (di == INVALID_HANDLE_VALUE) return L"";
    std::wstring result;
    SP_DEVICE_INTERFACE_DATA ifd{ sizeof(ifd) };
    SP_DEVINFO_DATA dd{ sizeof(dd) };
    GUID g{};
    DEVPROPTYPE t;
    if (SetupDiOpenDeviceInterfaceW(di, path.c_str(), 0, &ifd) &&
        SetupDiGetDeviceInterfaceDetailW(di, &ifd, nullptr, 0, nullptr, &dd) == FALSE && dd.DevInst &&
        SetupDiGetDevicePropertyW(di, &dd, &DEVPKEY_Device_ContainerId, &t, (PBYTE)&g, sizeof(g), nullptr, 0) &&
        t == DEVPROP_TYPE_GUID) {
        wchar_t buf[64];
        StringFromGUID2(g, buf, 64);
        result = buf;
    }
    SetupDiDestroyDeviceInfoList(di);
    return result;
}

}  // namespace

void PollGenericHid(PollContext& ctx) {
    // Drawing tablets and pen displays (any collection on the Digitizers page 0D) are never read:
    // polling one for a battery report flipped the Y axis of an XP-Pen Deco Fun S (BatteryHub issue #232).
    std::set<std::pair<USHORT, USHORT>> digitizers;
    for (const auto& c : ctx.colls)
        if (c.page() == 0x0D) digitizers.insert({ c.vid, c.pid });
    for (const auto& c : ctx.colls) {
        if (ctx.IsClaimed(c.vid, c.pid)) continue;
        if (c.page() == 0x84 || c.page() == 0x85) continue;  // UPS / power supplies
        if (digitizers.count({ c.vid, c.pid })) continue;
        if (!c.caps.NumberFeatureValueCaps && !c.caps.NumberInputValueCaps) continue;
        // Same physical device already shown with Windows' Bluetooth level (its HID field can read 0).
        if (!ctx.btContainers.empty() && ctx.btContainers.count(ContainerOf(c.path))) continue;
        HANDLE h = CreateFileW(c.path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE) continue;
        PHIDP_PREPARSED_DATA pp;
        if (!HidD_GetPreparsedData(h, &pp)) { CloseHandle(h); continue; }

        int pct = -1;
        for (HIDP_REPORT_TYPE type : { HidP_Feature, HidP_Input }) {
            USHORT n = type == HidP_Feature ? c.caps.NumberFeatureValueCaps : c.caps.NumberInputValueCaps;
            USHORT len = type == HidP_Feature ? c.featLen() : c.inLen();
            if (!n || !len) continue;
            std::vector<HIDP_VALUE_CAPS> vc(n);
            if (HidP_GetValueCaps(type, vc.data(), &n, pp) != HIDP_STATUS_SUCCESS) continue;
            for (USHORT i = 0; i < n && pct < 0; i++) {
                const auto& v = vc[i];
                USAGE u = v.IsRange ? v.Range.UsageMin : v.NotRange.Usage;
                if (!IsBatteryUsage(v.UsagePage, u)) continue;
                std::vector<BYTE> buf(len, 0);
                buf[0] = v.ReportID;
                BOOL got = type == HidP_Feature ? HidD_GetFeature(h, buf.data(), len) : HidD_GetInputReport(h, buf.data(), len);
                if (!got) continue;
                ULONG val = 0;
                if (HidP_GetUsageValue(type, v.UsagePage, v.LinkCollection, u, &val, pp, (PCHAR)buf.data(), len) != HIDP_STATUS_SUCCESS)
                    continue;
                // Apple devices (Magic Trackpad / Mouse / Keyboard) report a plain percentage while
                // declaring a 0-255 range; Linux handles them with HID_BATTERY_QUIRK_PERCENT.
                bool percentQuirk = c.vid == 0x05AC && val <= 100;
                if (percentQuirk) pct = (int)val;
                else if (v.LogicalMax > v.LogicalMin) pct = Clamp100((int)(((LONG)val - v.LogicalMin) * 100 / (v.LogicalMax - v.LogicalMin)));
                else if (val <= 100) pct = (int)val;
            }
            if (pct >= 0) break;
        }
        HidD_FreePreparsedData(pp);
        CloseHandle(h);
        if (pct < 0) continue;
        DeviceStatus s;
        s.name = HidProduct(c);
        if (s.name.empty()) s.name = L"Wireless device";
        s.key = L"hid:" + Hex4(c.vid) + L":" + Hex4(c.pid);
        s.via = L"HID";
        s.kind = KindFromCollections(ctx.colls, c.vid, c.pid);
        if (s.kind == DeviceKind::Other) s.kind = GuessKind(s.name);
        s.percent = pct;
        ctx.Add(s, c.vid, c.pid);
    }
}

void PollBluetooth(PollContext& ctx) {
    HDEVINFO di = SetupDiGetClassDevsW(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (di == INVALID_HANDLE_VALUE) return;

    struct Found { int pct; std::wstring name; bool connected; };
    std::map<std::wstring, std::wstring> containerName;  // container id -> name of the main BT device node
    std::map<std::wstring, UINT32> containerCod;         // container id -> class of device
    std::map<std::wstring, std::wstring> containerCat;   // container id -> device container category
    std::map<std::wstring, bool> containerConn;          // container id -> main BT device node connected
    std::map<std::wstring, Found> battery;
    SP_DEVINFO_DATA dd{ sizeof(dd) };
    for (DWORD i = 0; SetupDiEnumDeviceInfo(di, i, &dd); i++) {
        wchar_t inst[512];
        if (!SetupDiGetDeviceInstanceIdW(di, &dd, inst, 512, nullptr)) continue;
        if (wcsncmp(inst, L"BTHENUM\\", 8) && wcsncmp(inst, L"BTHLE\\", 6) && wcsncmp(inst, L"BTHLEDEVICE\\", 12)) continue;

        GUID container{};
        DEVPROPTYPE t;
        std::wstring cid = inst;
        if (SetupDiGetDevicePropertyW(di, &dd, &DEVPKEY_Device_ContainerId, &t, (PBYTE)&container, sizeof(container), nullptr, 0)) {
            wchar_t g[64];
            StringFromGUID2(container, g, 64);
            cid = g;
        }
        std::wstring name = StrProp(di, &dd, DEVPKEY_Device_FriendlyName);
        if (name.empty()) name = StrProp(di, &dd, DEVPKEY_Device_DeviceDesc);
        if (!wcsncmp(inst, L"BTHENUM\\DEV_", 12) || !wcsncmp(inst, L"BTHLE\\DEV_", 10)) {
            containerName[cid] = name;
            UINT32 cod = 0;
            if (SetupDiGetDevicePropertyW(di, &dd, &kClassOfDevice, &t, (PBYTE)&cod, sizeof(cod), nullptr, 0) && t == DEVPROP_TYPE_UINT32)
                containerCod[cid] = cod;
            std::wstring cat = FirstStringProp(di, &dd, DEVPKEY_DeviceContainer_Category);
            if (!cat.empty()) containerCat[cid] = cat;
            DEVPROP_BOOLEAN conn;
            // Classic and LE nodes can share one container: it is connected when either one is.
            if (SetupDiGetDevicePropertyW(di, &dd, &kIsConnected, &t, (PBYTE)&conn, sizeof(conn), nullptr, 0) &&
                t == DEVPROP_TYPE_BOOLEAN)
                containerConn[cid] = containerConn[cid] || conn != DEVPROP_FALSE;
        }

        BYTE pct = 0;
        if (!SetupDiGetDevicePropertyW(di, &dd, &kBtBattery, &t, &pct, sizeof(pct), nullptr, 0) || t != DEVPROP_TYPE_BYTE) continue;
        // The service node's own connected flag is only a fallback: headphones keep their level on the
        // Hands-Free node, which reads as disconnected while audio runs over A2DP. The main device node
        // decides below when there is one.
        DEVPROP_BOOLEAN conn = DEVPROP_TRUE;
        if (!SetupDiGetDevicePropertyW(di, &dd, &kIsConnected, &t, (PBYTE)&conn, sizeof(conn), nullptr, 0) ||
            t != DEVPROP_TYPE_BOOLEAN)
            conn = DEVPROP_TRUE;
        ULONG st = 0, prob = 0;
        if (CM_Get_DevNode_Status(&st, &prob, dd.DevInst, 0) == CR_SUCCESS && (st & DN_HAS_PROBLEM)) continue;
        if (pct > 100) continue;
        auto prev = battery.find(cid);
        if (prev != battery.end() && prev->second.connected && conn == DEVPROP_FALSE) continue;  // prefer a live node
        battery[cid] = { pct, StripServiceSuffix(name), conn != DEVPROP_FALSE };
    }
    SetupDiDestroyDeviceInfoList(di);

    // Physical devices a driver above already reported over a HID interface on the Bluetooth link
    // itself (e.g. Logitech HID++ names the LIFT "Logitech LIFT VERTICAL ERGONOMIC MOUSE" while
    // Bluetooth calls it "LIFT", so a name check misses it).
    std::set<std::wstring> reported;
    std::map<std::wstring, std::pair<USHORT, USHORT>> hidIds;  // container id -> HID vid/pid on that device
    for (const auto& c : ctx.colls) {
        std::wstring cont = ContainerOf(c.path);
        hidIds[cont] = { c.vid, c.pid };
        if (ctx.IsClaimed(c.vid, c.pid)) reported.insert(cont);
    }

    for (auto& [cid, f] : battery) {
        if (reported.count(cid)) continue;
        auto mc = containerConn.find(cid);
        if (mc != containerConn.end() ? !mc->second : !f.connected) continue;
        DeviceStatus s;
        s.key = L"bt:" + cid;
        auto it = containerName.find(cid);
        s.name = (it != containerName.end() && !it->second.empty()) ? it->second : f.name;
        if (s.name.empty()) s.name = L"Bluetooth device";
        if (IsDuplicateBluetoothDevice(s, ctx.out)) continue;
        s.via = L"Bluetooth";
        s.percent = f.pct;
        auto cod = containerCod.find(cid);
        s.kind = cod != containerCod.end() ? KindFromClassOfDevice(cod->second) : DeviceKind::Other;
        if (s.kind == DeviceKind::Other) {
            auto cat = containerCat.find(cid);
            if (cat != containerCat.end()) s.kind = KindFromCategory(cat->second);
        }
        // Bluetooth LE devices have no class of device; their HID collections tell a mouse from a keyboard.
        auto ids = hidIds.find(cid);
        if (s.kind == DeviceKind::Other && ids != hidIds.end()) s.kind = KindFromBleIds(ids->second.first, ids->second.second);
        if (s.kind == DeviceKind::Other && ids != hidIds.end()) s.kind = KindFromCollections(ctx.colls, ids->second.first, ids->second.second);
        if (s.kind == DeviceKind::Other) s.kind = GuessKind(s.name);
        ctx.out.push_back(s);
        ctx.btContainers.insert(cid);
    }
}

}

void PollBluetooth(PollContext& ctx) { wb_driver_generic::PollBluetooth(ctx); }

void PollGenericHid(PollContext& ctx) { wb_driver_generic::PollGenericHid(ctx); }

namespace wb_driver_glorious {
// Glorious (VID 258A), Hitscan (VID 3770), G-Wolves (VID 33E4) and WLMouse (VID 36A7) wireless mice using
// the same supplier protocol (G-Wolves calls it "XVI"; its other models use the Kysona protocol instead):
// Request: 64-byte feature report (id 0), bytes [3]=02 [4]=02 [6]=83.
// Reply:   [1]=status, [4]=02, [6]=83, [7]=charging (1), [8]=percent.
// Status A1 = awake, A4 = asleep, A0 = waking up, others = unreachable.
// Protocol: github.com/ikajdan/glorious-indicator, github.com/louis4craft/glorious-ctl,
// Hitscan's web configurator (control.hitscan.com) and G-Wolves' (mouse.xyz, Config/env-models.json).
// WLMouse's YING IDs: gm.wlmouse.gg/Config/env-models.json. The 1K dongle is shared with other
// models; only the YING 8K receiver and its battery collection are matched here.
// Model names, cable product ids and protocols from G-Wolves' own device list (mouse.xyz/Config/env-models.json).
// 33E4:3854 is a dongle shared by many models, so it has no specific name.
// Product ids from Hitscan's configurator: 3770:0300 receiver, 0136 / 0194 cables.
const wchar_t* GWolvesModelName(USHORT pid) {
    switch (pid) {
        case 0x3808: case 0x3817: return L"G-Wolves HTM Plus";
        case 0x6808: case 0x6817: return L"G-Wolves HSK Pro 2.0";
        case 0x5608: case 0x5617: case 0x5618: case 0x5619: return L"G-Wolves HTXU";
        case 0x3608: case 0x3617: case 0x3619: return L"G-Wolves Fenrir Pro";
        case 0x3908: case 0x3917: return L"G-Wolves VUK";
        case 0x7904: case 0x7913: return L"G-Wolves HT-S2";
        case 0x3708: case 0x3717: return L"G-Wolves Fenrir Max";
        case 0x5308: case 0x5317: return L"G-Wolves HTS Ultra";
        case 0x7704: case 0x7713: return L"G-Wolves HTR";
        case 0x3508: case 0x3517: return L"G-Wolves Fenrir";
        case 0x7908: case 0x7917: return L"G-Wolves HT-S2 Pro";
        case 0x5808: case 0x5817: return L"G-Wolves HSK Pro";
        case 0x2708: case 0x2717: case 0x2719: return L"G-Wolves HTX Mini";
        case 0x5408: case 0x5417: case 0x5418: case 0x5419: return L"G-Wolves HTS Plus";
        case 0x5708: case 0x5717: return L"G-Wolves HTX";
        case 0x5908: case 0x5917: return L"G-Wolves HSK Plus";
        case 0x7204: case 0x7203: return L"G-Wolves HSK Lite";
        case 0x7708: case 0x7717: return L"G-Wolves HTR Pro";
        case 0x5804: case 0x5803: return L"G-Wolves HSK Pro ACE";
        case 0x5404: case 0x5403: return L"G-Wolves HTS Plus ACE";
        case 0x5704: case 0x5703: return L"G-Wolves HTX ACE";
        case 0x5904: case 0x5903: return L"G-Wolves HSK Plus ACE";
        case 0x4718: case 0x4719: return L"G-Wolves Lycan";
        case 0x4219: return L"G-Wolves WARG";
        case 0x5219: return L"G-Wolves HTS Plus Pro";
        case 0x3519: return L"G-Wolves Fenrir Asym";
        case 0x2619: return L"G-Wolves HTX Mini Asym";
        case 0x1619: return L"G-Wolves Fenrir Max Asym";
        case 0x2119: return L"G-Wolves VUK Mini";
        default: return L"G-Wolves wireless mouse";
    }
}

bool GWolvesIsCable(USHORT pid) {
    static const USHORT wired[] = { 0x1619, 0x2119, 0x2619, 0x2708, 0x2719, 0x3508, 0x3519, 0x3608, 0x3619, 0x3708, 0x3808, 0x3908,
                                    0x4219, 0x4718, 0x4719, 0x5219, 0x5308, 0x5404, 0x5408, 0x5418, 0x5419, 0x5608, 0x5618, 0x5619,
                                    0x5704, 0x5708, 0x5804, 0x5808, 0x5904, 0x5908, 0x6808, 0x7204, 0x7704, 0x7708, 0x7904, 0x7908 };
    for (USHORT w : wired)
        if (w == pid) return true;
    return false;
}

namespace {

// G-Wolves models with "XVI" set in the device list; the others use the Kysona protocol.
bool GWolvesIsXvi(USHORT pid) {
    static const USHORT xvi[] = { 0x2708, 0x2717, 0x3508, 0x3517, 0x3608, 0x3617, 0x3708, 0x3717, 0x3808, 0x3817, 0x3908,
                                  0x3917, 0x5308, 0x5317, 0x5403, 0x5404, 0x5408, 0x5417, 0x5608, 0x5617, 0x5703, 0x5704,
                                  0x5708, 0x5717, 0x5803, 0x5804, 0x5808, 0x5817, 0x5903, 0x5904, 0x5908, 0x5917, 0x6808,
                                  0x6817, 0x7203, 0x7204, 0x7704, 0x7708, 0x7713, 0x7717, 0x7904, 0x7908, 0x7913, 0x7917 };
    return std::find(std::begin(xvi), std::end(xvi), pid) != std::end(xvi);
}

struct Vendor {
    USHORT vid;
    const wchar_t* keyPrefix;
    int replyDelayMs;
    bool (*known)(USHORT pid);  // products that use this protocol
    const wchar_t* (*name)(USHORT pid);
    bool (*isCable)(USHORT pid);
    bool (*matches)(const HidColl&) = nullptr;
};

const wchar_t* GloriousName(USHORT pid) {
    switch (pid) {
        case 0x2022: case 0x2011: return L"Glorious Model O Wireless";
        case 0x2023: case 0x2012: return L"Glorious Model D Wireless";
        case 0x2024: case 0x2013: return L"Glorious Model O- Wireless";
        case 0x2025: case 0x2014: return L"Glorious Model D- Wireless";
        case 0x2033: return L"Glorious Model O 2 Wireless";
        case 0x2034: case 0x2036: case 0x201A: return L"Glorious Model D 2 PRO";
        default: return nullptr;  // another brand on the same Sinowealth chip (e.g. Redragon keyboards)
    }
}
bool GloriousKnown(USHORT pid) { return GloriousName(pid) != nullptr; }
bool GloriousCable(USHORT pid) { return pid == 0x2011 || pid == 0x2012 || pid == 0x2013 || pid == 0x2014 || pid == 0x201A; }

const wchar_t* HitscanName(USHORT pid) {
    switch (pid) {
        case 0x0136: return L"Hitscan Hyperlight X";
        case 0x0194: return L"Hitscan Hyperlight N";
        default: return L"Hitscan Hyperlight";
    }
}
bool HitscanKnown(USHORT pid) { return pid == 0x0300 || pid == 0x0136 || pid == 0x0194; }
bool HitscanCable(USHORT pid) { return pid == 0x0136 || pid == 0x0194; }

const Vendor kVendors[] = {
    { 0x258A, L"glorious:", 50, GloriousKnown, GloriousName, GloriousCable },
    { 0x3770, L"hitscan:", 200, HitscanKnown, HitscanName, HitscanCable },
    { 0x33E4, L"gwolves:", 100, GWolvesIsXvi, GWolvesModelName, GWolvesIsCable },
    { 0x36A7, L"wlmouse:", 100, [](USHORT pid) { return pid == 0xA874; }, [](USHORT) { return L"WLMouse YING"; },
      [](USHORT) { return false; }, [](const HidColl& c) { return c.page() == 0xFFFF && c.usage() == 0; } },
};

}  // namespace

void PollGlorious(PollContext& ctx) {
    for (const Vendor& v : kVendors) {
        for (auto [vid, pid] : ctx.Products(v.vid)) {
            if (!v.known(pid)) continue;
            for (const HidColl* c : ctx.Find(vid, pid, [&v](const HidColl& c) {
                     return c.featLen() == 65 && (!v.matches || v.matches(c));
                 })) {
                HidDev d;
                if (!d.OpenFeatureOnly(*c)) continue;
                std::vector<BYTE> r;
                bool ok = false;
                for (int attempt = 0; attempt < 2; attempt++) {
                    std::vector<BYTE> w(65, 0);
                    w[3] = 0x02; w[4] = 0x02; w[6] = 0x83;
                    if (!d.SetFeature(w)) break;
                    Sleep(v.replyDelayMs);
                    if (!d.GetFeature(0, r) || r.size() < 9) break;
                    if (r[6] != 0x83 || (r[1] & 0xF0) != 0xA0) break;  // not this protocol
                    if (r[1] == 0xA0 && attempt == 0) { Sleep(400); continue; }  // waking up: ask again
                    ok = true;
                    break;
                }
                if (!ok) continue;
                DeviceStatus s;
                s.key = v.keyPrefix + Hex4(pid);
                s.name = v.name(pid);
                s.kind = DeviceKind::Mouse;
                s.via = v.isCable(pid) ? L"USB cable" : L"2.4 GHz receiver";
                s.charging = v.isCable(pid) || r[7] == 0x01;
                if (r[1] == 0xA1 && r[8] <= 100) s.percent = Clamp100(r[8] ? r[8] : 1);
                else s.asleep = true;
                ctx.Add(s, vid, pid);
                break;
            }
        }
    }
}

}

void PollGlorious(PollContext& ctx) { wb_driver_glorious::PollGlorious(ctx); }

const wchar_t* GWolvesModelName(USHORT p) { return wb_driver_glorious::GWolvesModelName(p); }
bool GWolvesIsCable(USHORT p) { return wb_driver_glorious::GWolvesIsCable(p); }

namespace wb_driver_hyperx {
// HyperX wireless headsets and Pulsefire Haste 2 mice (HP VID 03F0, older Kingston VID 0951).
// Protocols: Pulsefire Haste 2 Core Wireless (03F0:0AB5), Haste 2 Wireless (03F0:0F98) and Haste 2 Mini (03F0:0DA0)
// verified on hardware, see HasteWireless;
// Cloud III Wireless verified on hardware (03F0:0C9D) and github.com/Medprj/HyperXWM;
// Cloud III S from github.com/LennardKittner/HyperHeadset; Cloud Stinger Core from
// github.com/davidsantanaalves/HyperBatteryMonitor (also Cloud Core Wireless DTS 0951:173F, verified on hardware);
// the others follow github.com/Sapd/HeadsetControl.

namespace {

// Writes `req` and returns the first input report accepted by `match` within `timeoutMs`.
template <class Match>
bool Query(HidDev& d, const std::vector<BYTE>& req, std::vector<BYTE>& resp, int timeoutMs, Match match) {
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + timeoutMs;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(resp, left) < 0) return false;
        if (match(resp)) return true;
    }
}

DeviceStatus Base(USHORT pid, const wchar_t* name) {
    DeviceStatus s;
    s.key = L"hyperx:" + Hex4(pid);
    s.name = name;
    s.kind = DeviceKind::Headset;
    s.via = L"2.4 GHz receiver";
    return s;
}

// Cloud III Wireless: report 66 <cmd>; 82 -> [2] connected, 8A -> [2] charging, 89 -> [2..3] mV, [4] %.
bool Cloud3(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        return Query(d, { 0x66, cmd }, r, 800, [cmd](const std::vector<BYTE>& r) { return r.size() >= 5 && r[0] == 0x66 && r[1] == cmd; });
    };
    std::vector<BYTE> r;
    if (!q(0x82, r)) return false;
    s = Base(c.pid, (c.pid == 0x069F || c.pid == 0x0995) ? L"HyperX Cloud II Core Wireless" : L"HyperX Cloud III Wireless");
    s.connected = r[2] != 0;
    if (s.connected) {
        if (q(0x8A, r)) s.charging = r[2] == 1;
        if (q(0x89, r) && r[4] <= 100) { s.percent = r[4]; s.millivolts = (r[2] << 8) | r[3]; }
    }
    return true;
}

// Cloud II Wireless (HP): report 06 FF BB <cmd>; 02 -> [7] %, 03 -> [4] charging.
bool Cloud2(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        return Query(d, { 0x06, 0xFF, 0xBB, cmd }, r, 1000,
                     [cmd](const std::vector<BYTE>& r) { return r.size() >= 8 && r[0] == 0x06 && r[1] == 0xFF && r[2] == 0xBB && r[3] == cmd; });
    };
    std::vector<BYTE> r;
    if (!q(0x02, r) || r[7] > 100) return false;
    s = Base(c.pid, L"HyperX Cloud II Wireless");
    s.percent = r[7];
    s.millivolts = (r[5] << 8) | r[6];
    if (q(0x03, r)) s.charging = r[4] == 1;
    return true;
}

// Cloud II Wireless (Kingston 0951:1718) and Cloud Flight S (0951:16EA/16EB):
// fixed 62-byte frame, reply 0B 00 BB <cmd>.
bool Cloud2Kingston(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        std::vector<BYTE> req = { 0x06, 0x00, 0x02, 0x00, 0x9A, 0x00, 0x00, 0x68, 0x4A, 0x8E, 0x0A, 0x00, 0x00, 0x00, 0xBB, cmd, 0x00 };
        return Query(d, req, r, 1000, [cmd](const std::vector<BYTE>& r) { return r.size() >= 8 && r[0] == 0x0B && r[2] == 0xBB && r[3] == cmd; });
    };
    std::vector<BYTE> r;
    if (!q(0x02, r) || r[7] > 100) return false;
    s = Base(c.pid, (c.pid == 0x16EA || c.pid == 0x16EB) ? L"HyperX Cloud Flight S" : L"HyperX Cloud II Wireless");
    s.percent = r[7];
    if (q(0x03, r)) s.charging = r[4] == 1;
    return true;
}

// Cloud Alpha Wireless: 21 BB <cmd>; 03 -> [3]==1 offline, 0C -> [3]==1 charging, 0B -> [3] %.
bool CloudAlpha(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        return Query(d, { 0x21, 0xBB, cmd }, r, 1000, [cmd](const std::vector<BYTE>& r) { return r.size() >= 4 && r[0] == 0x21 && r[2] == cmd; });
    };
    std::vector<BYTE> r;
    if (!q(0x03, r)) return false;
    s = Base(c.pid, L"HyperX Cloud Alpha Wireless");
    s.connected = r[3] != 0x01;
    if (!s.connected) return true;
    if (q(0x0C, r)) s.charging = r[3] == 0x01;
    if (q(0x0B, r) && r[3] <= 100) s.percent = r[3];
    return true;
}

// Cloud Alpha 2 Wireless: [cmd][sub]; reply [cmd+1][sub]...; 52 01 -> [2] connected, 50 02 -> [2] %, [3] charging.
bool CloudAlpha2(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, BYTE sub, std::vector<BYTE>& r) {
        return Query(d, { cmd, sub }, r, 1000, [cmd, sub](const std::vector<BYTE>& r) { return r.size() >= 8 && r[0] == cmd + 1 && r[1] == sub; });
    };
    std::vector<BYTE> r;
    if (!q(0x52, 0x01, r)) return false;
    s = Base(c.pid, L"HyperX Cloud Alpha 2 Wireless");
    s.connected = r[2] != 0;
    if (s.connected && q(0x50, 0x02, r) && r[2] <= 100) {
        s.percent = r[2];
        s.charging = r[3] == 0x01;
        s.millivolts = r[6] | (r[7] << 8);
    }
    return true;
}

// Pulsefire Haste 2 (Core) Wireless: NGENUITY's read-only battery query, the same 52 01 / 50 02 pair as
// Cloud Alpha 2, on the dongle's vendor collection (interface 2). 64-byte reports.
// From github.com/PMarinov1994/ngenuity-linux-cli (Haste 2 Wireless). Verified on a Core Wireless (03F0:0AB5), on
// a Haste 2 Wireless (03F0:0F98, MI_02 FF00:0001, 64-byte reports; level matched NGENUITY, BatteryHub PR #312)
// and on a Haste 2 Mini (03F0:0DA0, level matches NGENUITY); the 0D97 dongle is untested.
bool HasteWireless(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, BYTE sub, std::vector<BYTE>& r) {
        return Query(d, { cmd, sub }, r, 1000, [cmd, sub](const std::vector<BYTE>& r) { return r.size() >= 8 && r[0] == cmd + 1 && r[1] == sub; });
    };
    std::vector<BYTE> r;
    // After a reconnect the dongle floods the channel with unrelated reports (44 xx, 45 xx, FB xx) and the
    // link reply can miss the time limit, so a missing link reply is not an error: the battery query decides.
    bool linkKnown = q(0x52, 0x01, r);
    bool linked = !linkKnown || r[2] != 0;
    bool gotBattery = linked && q(0x50, 0x02, r) && r[2] <= 100;
    if (!linkKnown && !gotBattery) return false;

    // For about 5 s after it powers on the mouse reports 40% instead of its real level (measured 2026-11-17 on
    // a Core Wireless: 40% for 2-5 s, then 70%). A reading inside that settle window is not trusted. The first
    // poll after the app starts assumes the mouse has been on for a while.
    static ULONGLONG linkUpAt = 0;
    static bool polled = false;
    const ULONGLONG kSettleMs = 6000;
    ULONGLONG now = GetTickCount64();
    if (!linked) linkUpAt = 0;
    else if (linkUpAt == 0) linkUpAt = polled ? now : now - kSettleMs;
    polled = true;
    bool settled = now - linkUpAt >= kSettleMs;

    s = Base(c.pid, c.pid == 0x0AB5 ? L"HyperX Pulsefire Haste 2 Core Wireless"
                 : c.pid == 0x0DA0 ? L"HyperX Pulsefire Haste 2 Mini"
                 : L"HyperX Pulsefire Haste 2 Wireless");
    s.kind = DeviceKind::Mouse;
    s.connected = linked;
    if (gotBattery && settled) {  // an unsettled reading stays unknown, so the tray keeps the last value dimmed
        s.percent = r[2];
        s.charging = r[3] == 0x01 || r[3] == 0x02;
        s.millivolts = r[6] | (r[7] << 8);
    }
    return true;
}

// Cloud Flight: 21 FF 05 -> [3..4] mV (above 0x100B means charging).
bool CloudFlight(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    std::vector<BYTE> r;
    if (!Query(d, { 0x21, 0xFF, 0x05 }, r, 1000, [](const std::vector<BYTE>& r) { return r.size() >= 5; })) return false;
    int mv = (r[3] << 8) | r[4];
    s = Base(c.pid, L"HyperX Cloud Flight");
    s.millivolts = mv;
    if (mv > 0x100B) {
        s.charging = true;
    } else if (mv <= 3648) {
        s.percent = Clamp100((int)(0.00125 * mv));
    } else if (mv > 3975) {
        s.percent = 100;
    } else {
        double v = mv;
        s.percent = Clamp100((int)std::lround(0.00000002547505 * std::pow(v, 4) - 0.0003900299 * std::pow(v, 3) +
                                              2.238321 * v * v - 5706.256 * v + 5452299.0));
    }
    return true;
}

// Cloud III S Wireless: report 0C [02 03 01 00 <cmd>]; reply [5] = cmd, [6] = value (FF = invalid).
//   02 -> [6] == 2 connected, 06 -> [6] %, 48 -> [6] 0 discharging / 1 charging / 2 full.
// From github.com/LennardKittner/HyperHeadset (cloud_iii_s_wireless.rs).
bool Cloud3S(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        return Query(d, { 0x0C, 0x02, 0x03, 0x01, 0x00, cmd }, r, 1000,
                     [cmd](const std::vector<BYTE>& r) { return r.size() >= 7 && r[0] == 0x0C && r[5] == cmd && r[6] != 0xFF; });
    };
    std::vector<BYTE> r;
    if (!q(0x02, r)) return false;
    s = Base(c.pid, L"HyperX Cloud III S Wireless");
    s.connected = r[6] == 2;
    if (!s.connected) return true;
    if (q(0x06, r) && r[6] <= 100) s.percent = r[6];
    if (q(0x48, r)) s.charging = r[6] == 1 || r[6] == 2;
    return true;
}

// Cloud Stinger Core Wireless (+ 7.1): feature report FF on the FF00:0001 collection (MI_03 Col03).
// From github.com/davidsantanaalves/HyperBatteryMonitor (Devices/CloudStingerCoreWirelessDevice.cs, MIT);
// the PlayStation receiver 03F0:0A93 was verified on hardware there (PR #6: 52 queries, codes 03/05/06).
// SetFeature(command), wait 20 ms, GetFeature(FF):
//   connection: reply [11] == 1 means the headset is on.
//   battery:    reply [9] = 03 discharging / 05 charging / 06 full, [11..12] = battery mV (little endian).
// The PlayStation receivers (03F0:0A93, 0951:1713) and DTS receiver (03F0:0A8E) can hand back a previous report;
// their battery reply is FF 12 05, so on them the reply is read again (up to 10 x 20 ms) until it looks right.
// 0951:1713 (Kingston PlayStation receiver, same MI_03 layout as 0A93) was verified on hardware.
// 0951:173F (Kingston receiver of the Cloud Core Wireless DTS, same MI_03 layout) was verified on hardware:
// connection reply [11] == 1 when the headset is on, 3 when it is off; battery reply FF 12 05 with [9] == 03
// while discharging and 05 while charging. The mV-to-percent curve is the Stinger Core one, not calibrated for
// this model.
// DTS 03F0:0A8E: battery/charging verified on Windows; same FF 12 05 reply (HyperBatteryMonitor PR #16).
bool StingerCore(const HidColl& c, DeviceStatus& s) {
    static const std::vector<BYTE> kConnection = { 0xFF, 0x07, 0x00, 0xFD, 0x04, 0x00, 0x00, 0x02, 0x82, 0x03,
                                                   0x05, 0x29, 0x0F, 0x00, 0x00, 0x03, 0x68, 0x10, 0x00, 0x00 };
    static const std::vector<BYTE> kBattery = { 0xFF, 0x07, 0x00, 0xFD, 0x04, 0x0C, 0xF1, 0x02, 0x01, 0x04, 0xF0, 0x0C };
    // mV -> % in 10% steps, from the same source (separate curves while charging and on battery).
    static const int kPct[] = { 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0 };
    static const int kChargingMv[] = { 4130, 4090, 4050, 4000, 3967, 3946, 3930, 3916, 3871, 3831, 3400 };
    static const int kDischargingMv[] = { 4121, 4042, 3954, 3905, 3838, 3785, 3763, 3738, 3700, 3651, 3517 };

    HidDev d;
    if (!d.Open(c)) return false;
    const bool ps = c.pid == 0x0A93 || c.pid == 0x1713 || c.pid == 0x173F || c.pid == 0x0A8E;
    auto exchange = [&](const std::vector<BYTE>& cmd, std::vector<BYTE>& r, bool (*good)(const std::vector<BYTE>&)) {
        if (!d.SetFeature(cmd)) return false;
        for (int i = 0; i < (ps ? 10 : 1); i++) {
            Sleep(20);
            if (!d.GetFeature(0xFF, r) || r.size() < 13) return false;
            if (!ps || good(r)) return true;
        }
        return false;
    };
    auto fresh = [](const std::vector<BYTE>& r) { return !(r[1] == 0x01 && r[2] == 0x00); };
    auto battery = [](const std::vector<BYTE>& r) { return r[0] == 0xFF && r[1] == 0x12 && r[2] == 0x05; };

    std::vector<BYTE> r;
    // The connection reply's header isn't documented, so after 10 stale-looking reads the last one is used.
    if (!exchange(kConnection, r, fresh) && !(ps && r.size() >= 13 && !fresh(r))) return false;
    s = Base(c.pid, c.pid == 0x173F ? L"HyperX Cloud Core Wireless"
                  : c.pid == 0x0A8E ? L"HyperX Cloud Stinger Core Wireless DTS"
                  : L"HyperX Cloud Stinger Core Wireless");
    s.connected = r[11] == 0x01;
    if (!s.connected || !exchange(kBattery, r, battery)) return true;
    BYTE code = r[9];
    if (code != 0x03 && code != 0x05 && code != 0x06) return true;
    s.millivolts = r[11] | (r[12] << 8);
    s.charging = code == 0x05;
    s.percent = code == 0x06 ? 100 : Interpolate(s.millivolts, code == 0x05 ? kChargingMv : kDischargingMv, kPct, 11);
    return true;
}

struct Model {
    USHORT vid;
    std::vector<USHORT> pids;
    bool (*accept)(const HidColl&);  // which collection carries the protocol
    bool (*poll)(const HidColl&, DeviceStatus&);
};

const Model kModels[] = {
    { 0x03F0, { 0x05B7, 0x0C9D }, [](const HidColl& c) { return c.outLen() == 62 && c.inLen() == 62; }, Cloud3 },
    // Cloud II Core Wireless: same 66 <cmd> protocol as Cloud III Wireless, 64-byte reports (HyperHeadset).
    { 0x03F0, { 0x069F, 0x0995 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 62 && c.inLen() >= 62; }, Cloud3 },
    // Cloud III S: report 0C lives on the 64/64-byte collection, which Windows lists under usage page 01C0
    // (not a vendor page). The FF13 (62-byte) and FFC0 (32-byte) collections don't carry it.
    { 0x03F0, { 0x06BE, 0x02CC }, [](const HidColl& c) { return c.outLen() == 64 && c.inLen() == 64; }, Cloud3S },
    { 0x03F0, { 0x0696, 0x018B, 0x0D93, 0x0C8C }, [](const HidColl& c) { return c.page() == 0xFF90 && c.usage() == 0x0303; }, Cloud2 },  // 0C8C: Cloud Flight for PS, untested
    { 0x0951, { 0x1718 }, [](const HidColl& c) { return c.outLen() == 62; }, Cloud2Kingston },
    // Cloud Flight S (0951:16EA / 16EB) shares the Cloud II Wireless read-only protocol.
    // 0951:16EA: FF13:0001 on interface 5, 62-byte input/output (BatteryHub issue #348).
    // Protocol: LennardKittner/HyperHeadset (MIT); hardware confirmation pending.
    { 0x0951, { 0x16EA, 0x16EB }, [](const HidColl& c) {
        return c.page() == 0xFF13 && c.usage() == 0x0001 && c.inLen() == 62 && c.outLen() == 62;
    }, Cloud2Kingston },
    { 0x03F0, { 0x098D }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudAlpha },
    { 0x03F0, { 0x08BE }, [](const HidColl& c) { return c.page() == 0xFF13 && c.usage() == 0xFF00; }, CloudAlpha2 },
    // Pulsefire Haste 2 Wireless (0F98 tested, 0D97 untested), Haste 2 Core Wireless (0AB5 tested) and Haste 2 Mini
    // (0DA0 tested) dongles.
    { 0x03F0, { 0x0AB5, 0x0D97, 0x0F98, 0x0DA0 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 2 && c.inLen() >= 8; }, HasteWireless },
    { 0x0951, { 0x16C4, 0x1723, 0x1749 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudFlight },
    { 0x03F0, { 0x0E90 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudFlight },  // Cloud Flight (HP)
    // Cloud Stinger Core Wireless: PlayStation receivers (HP 0A93, Kingston 1713), the HP DTS receiver (0A8E), the
    // original Kingston receiver, and the Cloud Core Wireless DTS receiver (Kingston 173F)
    { 0x03F0, { 0x0A93, 0x0A8E }, [](const HidColl& c) { return c.page() == 0xFF00 && c.usage() == 0x0001 && c.featLen() >= 32; }, StingerCore },
    { 0x0951, { 0x170B, 0x1713, 0x173F }, [](const HidColl& c) { return c.page() == 0xFF00 && c.usage() == 0x0001 && c.featLen() >= 32; }, StingerCore },
};

}  // namespace

void PollHyperX(PollContext& ctx) {
    for (const auto& m : kModels) {
        for (USHORT pid : m.pids) {
            if (ctx.IsClaimed(m.vid, pid)) continue;
            for (const HidColl* c : ctx.Find(m.vid, pid, m.accept)) {
                DeviceStatus s;
                if (m.poll(*c, s)) { ctx.Add(s, m.vid, pid); break; }
            }
        }
    }
}

}

void PollHyperX(PollContext& ctx) { wb_driver_hyperx::PollHyperX(ctx); }

namespace wb_driver_jbl {
// Quantum 350 (0ECB:206B): JBL QuantumENGINE 2.9.4 registers BatteryLevelProp (HIDV1 GET 49)
// and HIDV1WirelessConnectionStateProp (GET 68) for Q350TV1. Each returns one payload byte.
// The Windows collection is documented in BatteryHub issue #259.
//
// Quantum 610 Wireless (0ECB:205C, ID from JBL's QuantumUSBAudio.inf as listed in
// https://github.com/sidmax7/QuantumEngine quantum-linux/crates/jbl-quantum/src/models.rs): the same HIDV1
// GET 49 (battery, 0-100) and GET 68 (link, 01 = headset connected) on HID interface 5, captured on a real
// 610 dongle in https://github.com/Tymon3310/jbl-quantum-linux/blob/main/docs/QUANTUM610_PACKETS.md.
// The firmware answers an unimplemented report id with the previous implemented one, so the reply's
// first byte must match the id asked for (GetFeatureByte checks it). Only GETs are sent.
//
// Quantum 360 Wireless (0ECB:2115, same ID source; dongle listed with the HIDV1 dongle models in
// https://github.com/Tymon3310/jbl-quantum-linux/blob/main/docs/QUANTUM_LINEUP.md). Not HIDV3: its
// Windows dump in BatteryHub issue #183 has no FF12 collection, only the 350's layout of a Generic
// Desktop usage 0 collection (interface 5, 64-byte feature = the 63-byte HIDV1 reports) next to a
// telephony one. Same read-only GET 68 / GET 49.
//
// JBL Quantum 520 / 650 / 850 / 950 (and X variants) headsets, vendor 0ECB, "HIDV3" protocol on the
// vendor collection with usage page FF12 (64-byte reports: input 86, output 87, feature 88).
//   request (feature 88): [88][00 DD][01 00][01][00][02 00][feature lo][feature hi]
//            identifier DD00 = the device itself, command 0001 = get device info, 1 packet, index 0,
//            payload = the 2-byte feature id
//   reply   (input 86):   [86][00 DD][01 00][01][00][len lo][len hi][status lo][status hi]
//                         [feature lo][feature hi][size lo][size hi][value...]
//            status bit 15 set = error
// Battery = feature 000D, one byte: bit 7 charging, bits 0..6 percent.
// Read from JBL QuantumENGINE 2.9.4 (HIDV3Device / PowerFeature.Battery.HIDV3Left) and checked
// against a Quantum 650 Wireless on its 2.4 GHz dongle; the other models use the same code path there.
namespace {

constexpr USHORT kVid = 0x0ECB;
constexpr USHORT kBatteryFeature = 0x000D;

struct Model { USHORT pid; const wchar_t* name; bool dongle; };
constexpr Model kModels[] = {
    { 0x2125, L"JBL Quantum 650 Wireless", true },  { 0x2126, L"JBL Quantum 650 Wireless", false },
    { 0x213B, L"JBL Quantum 650X Wireless", true }, { 0x213D, L"JBL Quantum 650X Wireless", false },
    { 0x2127, L"JBL Quantum 950 Wireless", true },  { 0x2128, L"JBL Quantum 950 Wireless", false },
    { 0x213E, L"JBL Quantum 950X Wireless", true }, { 0x2140, L"JBL Quantum 950X Wireless", false },
    { 0x218D, L"JBL Quantum 520 Wireless", true },  { 0x218E, L"JBL Quantum 520 Wireless", false },
    { 0x21A7, L"JBL Quantum 520X Wireless", true }, { 0x21A9, L"JBL Quantum 520X Wireless", false },
    { 0x219A, L"JBL Quantum 850 Wireless", true },  { 0x219B, L"JBL Quantum 850 Wireless", false },
};

// With the headset switched off the dongle does not answer at all, so a timeout means "not there".
bool GetBattery(HidDev& d, BYTE& info) {
    d.Flush();
    if (!d.SetFeature({ 0x88, 0x00, 0xDD, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, (BYTE)kBatteryFeature,
                        (BYTE)(kBatteryFeature >> 8) }))
        return false;
    ULONGLONG end = GetTickCount64() + 800;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 16) return false;
        // Only the reply to "get device info" sent to the device itself.
        if (r[0] != 0x86 || r[1] != 0x00 || r[2] != 0xDD || r[3] != 0x01 || r[4] != 0x00 || r[5] != 0x01) continue;
        if (r[10] & 0x80) return false;  // error status (bit 15)
        if (r[11] != (BYTE)kBatteryFeature || r[12] != (BYTE)(kBatteryFeature >> 8)) continue;
        if (r[13] == 0 && r[14] == 0) continue;  // empty feature value
        info = r[15];
        return (info & 0x7F) <= 100;
    }
    return false;
}

// HIDV1 headsets: GET 68 (wireless link) and GET 49 (battery percent), one payload byte each.
struct Hidv1Model { USHORT pid; int iface; USHORT page; const wchar_t* name; };  // page 0 = any collection
constexpr Hidv1Model kHidv1Models[] = {
    { 0x206B, 3, 0x0001, L"JBL Quantum 350 Wireless" },
    { 0x205C, 5, 0, L"JBL Quantum 610 Wireless" },
    { 0x2115, 5, 0x0001, L"JBL Quantum 360 Wireless" },
};

void PollHidv1(PollContext& ctx) {
    for (const Hidv1Model& m : kHidv1Models) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        for (const auto* c : ctx.Find(kVid, m.pid, [&m](const HidColl& c) {
                 return c.iface == m.iface && c.featLen() >= 2 &&
                        (m.page == 0 || (c.page() == m.page && c.usage() == 0));
             })) {
            HidDev d;
            BYTE linked = 0, percent = 0;
            if (!d.OpenReadOnly(*c) || !d.GetFeatureByte(0x68, linked) || linked != 1 ||
                !d.GetFeatureByte(0x49, percent) || percent > 100) continue;
            DeviceStatus s;
            s.key = L"jbl:" + Hex4(m.pid);
            s.name = m.name;
            s.kind = DeviceKind::Headset;
            s.via = L"2.4 GHz receiver";
            s.percent = percent;
            ctx.Add(s, kVid, m.pid);
            break;
        }
    }
}

}  // namespace

void PollJbl(PollContext& ctx) {
    PollHidv1(ctx);
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, m.pid, [](const HidColl& c) {
                 return c.page() == 0xFF12 && c.featLen() >= 11 && c.inLen() >= 16;
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            BYTE info = 0;
            if (!GetBattery(d, info)) continue;
            DeviceStatus s;
            s.key = L"jbl:" + Hex4(m.pid);
            s.name = m.name;
            s.kind = DeviceKind::Headset;
            s.via = m.dongle ? L"2.4 GHz receiver" : L"USB cable";
            s.percent = info & 0x7F;
            s.charging = (info & 0x80) != 0;
            ctx.Add(s, kVid, m.pid);
            break;
        }
    }
}

}

void PollJbl(PollContext& ctx) { wb_driver_jbl::PollJbl(ctx); }
namespace wb_driver_kysona {
// Kysona, Pulsar and ATK / VXE wireless mice (VID 3554, ATK dongles 373B), G-Wolves models on this protocol
// (VID 33E4, e.g. Fenrir Asym) and the original Hitscan
// Hyperlight (VID 3770, verified on the 1K receiver 3770:0200), Pulsar's newer XS-1 generation
// (VID 3710, verified on the 8K Dongle 3710:5406) and Redragon's Compx-based mice (M916 Pro 1K,
// M913 tri-mode, G49: receiver 3554:F55D / F5D5, cable F55E; verified on an M916 Pro 1K through
// the F55D receiver). They share one protocol:
// 17-byte output/input report id 08: [08][cmd][0 x14][checksum], checksum = 0x55 - sum(bytes 0..15).
//   cmd 03 = online status -> reply [6] = 1 when the mouse is connected to the dongle
//   cmd 04 = battery       -> reply [6] = percent, [7] = charging, [8..9] = millivolts
// ATK VXE Dragonfly R1 family (receivers 3554:F58A / F58E / F505, cables F58C / F58F / F506) uses the same
// protocol on vendor collection FF02:0002; its receivers expose several other 17-byte vendor collections
// (FF03..FF06), so FF02:0002 is tried first. IDs from upstream (verified on F505).
// HECATE (Edifier) mice: dongle 35BB:D3FB, mouse over cable D3FA / D3FC / D3FD (HECATE Connect's
// Config.ini). HECATE headsets on 35BB speak something else and are never sent these frames.
// Reference: Linux drivers/hid/hid-kysona.c (Lode Willems) and the hid-pulsar patch discussion;
// Redragon ids from github.com/EliottV17/redragon-battery-omarchy and github.com/pingpingjinping/g49-web-control.
namespace {

// Products that use this protocol: the ids above, the devices of Linux drivers/hid/hid-pulsar.c and
// the G-Wolves models without "XVI" in mouse.xyz/Config/env-models.json.
bool IsKnown(USHORT vid, USHORT pid) {
    switch (vid) {
        case 0x3554:
            switch (pid) {
                case 0xF507: case 0xF508: case 0xF509:  // Pulsar
                case 0xF57C: case 0xF57D: case 0xF5D5:  // Kysona M600 / M600 V2, Redragon
                case 0xF55D: case 0xF55E:               // Redragon
                case 0xF58A: case 0xF58C: case 0xF58E: case 0xF58F: case 0xF505: case 0xF506:  // VXE R1
                    return true;
                default:
                    return false;
            }
        case 0x373B: return pid == 0x1085;  // ATK VXE R1 SE receiver
        case 0x3770: return pid == 0x0200;  // Hitscan Hyperlight
        case 0x3710: return pid == 0x5406;  // Pulsar XS-1 8K Dongle
        case 0x35BB: return pid == 0xD3FA || pid == 0xD3FB || pid == 0xD3FC || pid == 0xD3FD;  // HECATE mice
        case 0x33E4:
            switch (pid) {
                case 0x1619: case 0x2119: case 0x2619: case 0x2719: case 0x3519: case 0x3619: case 0x3854: case 0x4219:
                case 0x4718: case 0x4719: case 0x5219: case 0x5418: case 0x5419: case 0x5618: case 0x5619:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

// Redragon uses Compx's own ids, and its receiver only calls itself "2.4G Wireless Receiver".
bool IsRedragon(USHORT vid, USHORT pid) { return vid == 0x3554 && (pid == 0xF55D || pid == 0xF5D5 || pid == 0xF55E); }

bool IsVxeR1(USHORT vid, USHORT pid) {
    return vid == 0x3554 &&
           (pid == 0xF58A || pid == 0xF58C || pid == 0xF58E || pid == 0xF58F || pid == 0xF505 || pid == 0xF506);
}

// A VXE R1 cable reports under its receiver's id, so the mouse is one entry on either connection.
USHORT KeyPid(USHORT vid, USHORT pid) {
    if (vid == 0x3554 && pid == 0xF58C) return 0xF58A;
    if (vid == 0x3554 && pid == 0xF58F) return 0xF58E;
    if (vid == 0x3554 && pid == 0xF506) return 0xF505;
    return pid;
}

bool IsCable(USHORT vid, USHORT pid) {
    return (vid == 0x33E4 && GWolvesIsCable(pid)) || (vid == 0x35BB && (pid == 0xD3FA || pid == 0xD3FC || pid == 0xD3FD)) ||
           (vid == 0x3554 && (pid == 0xF55E || pid == 0xF507 || pid == 0xF57D || pid == 0xF58C || pid == 0xF58F || pid == 0xF506));
}

BYTE Checksum(const BYTE* b) {
    unsigned sum = 0;
    for (int i = 0; i < 16; i++) sum += b[i];
    return (BYTE)(0x55 - sum);
}

// Sends `cmd` and waits for its checksummed reply.
bool Query(HidDev& d, BYTE cmd, std::vector<BYTE>& r) {
    std::vector<BYTE> req(17, 0);
    req[0] = 0x08;
    req[1] = cmd;
    req[16] = Checksum(req.data());
    d.Flush();
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + 500;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(r, left) < 17) return false;
        if (r[0] == 0x08 && r[1] == cmd && r[16] == Checksum(r.data())) return true;
    }
}

std::wstring CleanName(std::wstring n, USHORT vid, USHORT pid) {
    if (vid == 0x33E4) return GWolvesModelName(pid);
    if (IsRedragon(vid, pid)) return L"Redragon wireless mouse";  // shared by several models
    if (IsVxeR1(vid, pid)) return L"VXE Dragonfly R1";  // the receivers call themselves "VXE NordicMouse 1K Dongle" etc.
    for (const wchar_t* suffix : { L" Dongle", L" Receiver", L" 2.4G", L" Wireless Receiver" }) {
        size_t l = wcslen(suffix);
        if (n.size() > l && n.compare(n.size() - l, l, suffix) == 0) n = n.substr(0, n.size() - l);
    }
    if (vid == 0x3770) return L"Hitscan Hyperlight";  // the receiver only calls itself "Hitscan USB Receiver"
    if (vid == 0x35BB) return L"HECATE mouse";
    if (n.empty()) n = vid == 0x373B ? L"ATK wireless mouse" : L"Wireless mouse";
    return n;
}

}  // namespace

void PollKysona(PollContext& ctx) {
    std::vector<std::wstring> seenNames;
    for (USHORT vendor : { (USHORT)0x3554, (USHORT)0x373B, (USHORT)0x3770, (USHORT)0x33E4, (USHORT)0x3710, (USHORT)0x35BB }) {
        for (auto [vid, pid] : ctx.Products(vendor)) {
            if (!IsKnown(vid, pid)) continue;
            // Kysona exposes a dedicated 17-byte collection; ATK / VXE receivers put report 08 in a
            // vendor collection that also carries larger reports. The checksummed reply filters out
            // anything that does not speak this protocol.
            auto colls = ctx.Find(vid, pid, [](const HidColl& c) {
                return (c.outLen() == 17 && c.inLen() == 17) || (c.page() >= 0xFF00 && c.outLen() >= 17 && c.inLen() >= 17);
            });
            std::stable_sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) {
                return (a->page() == 0xFF02 && a->usage() == 0x0002) > (b->page() == 0xFF02 && b->usage() == 0x0002);
            });
            for (const HidColl* c : colls) {
                HidDev d;
                if (!d.Open(*c)) continue;
                std::vector<BYTE> r;
                DeviceStatus s;
                bool answered = false;
                if (Query(d, 0x03, r)) {
                    answered = true;
                    s.connected = r[6] != 0;
                }
                if ((!answered || s.connected) && Query(d, 0x04, r)) {
                    answered = true;
                    s.connected = true;
                    if (r[6] <= 100) s.percent = r[6];
                    s.charging = r[7] != 0;
                    s.millivolts = (r[8] << 8) | r[9];
                } else if (answered && s.connected) {
                    s.asleep = true;  // the receiver says online but the mouse did not answer: keep the last value
                }
                if (!answered) continue;
                if (!s.connected) {
                    ctx.Claim(vid, pid);
                    break;
                }
                s.key = L"kysona:" + Hex4(vid) + L":" + Hex4(KeyPid(vid, pid));
                s.name = CleanName(HidProduct(*c), vid, pid);
                s.kind = KindFromCollections(ctx.colls, vid, pid);
                if (s.kind == DeviceKind::Other) s.kind = DeviceKind::Mouse;
                s.via = IsCable(vid, pid) ? L"USB cable" : L"2.4 GHz receiver";
                s.graceSec = 1800;
                // A mouse plugged in for charging can show up twice (cable + dongle); keep one.
                bool dup = false;
                for (auto& n : seenNames) dup |= n == s.name;
                if (dup) {
                    ctx.Claim(vid, pid);
                    for (auto& o : ctx.out)
                        if (o.name == s.name && s.charging) o.charging = true;
                } else {
                    seenNames.push_back(s.name);
                    ctx.Add(s, vid, pid);
                }
                break;
            }
        }
    }
}

}

void PollKysona(PollContext& ctx) { wb_driver_kysona::PollKysona(ctx); }

namespace wb_driver_lamzu {
// LAMZU wireless mice on their own vendor ids 373E and 37B0 (Maya X, Maya, Tachi, Inca, Paro, Thorn,
// Atlantis Mini/OG Champion, Orcus ...). The Atlantis on Compx 3554 uses the Kysona driver instead.
// Protocol from LAMZU's Aurora web driver (lamzu.net, "IsNewProtocol" models):
//   SET_FEATURE report 0, 64 bytes: [00][00][02][02][00][83][00 ...]
//   ~100 ms later GET_FEATURE report 0: [A1][..][..][02][..][83][charging][percent] after the report
//   id byte (the web driver accepts the reply with or without that byte, so we do too).
//   charging 1 = on the cable; percent 0 means the mouse is not linked to the dongle.
// Model ids from lamzu.net/Config/env-models.json. Several newer models share one dongle id, so those
// dongles get a generic name. Other brands use the same DriverCore vendor ids (Arbiter Studio on 373E),
// so only these product ids are queried.
namespace {

const wchar_t* ModelName(USHORT vid, USHORT pid) {
    struct M { USHORT vid; USHORT pids[3]; const wchar_t* name; };
    static const M kModels[] = {
        { 0x373E, { 0x001C, 0x001D, 0x001E }, L"LAMZU Maya X" },
        { 0x373E, { 0x0016, 0, 0 }, L"LAMZU Maya X V2" },
        { 0x37B0, { 0x001F, 0, 0 }, L"LAMZU Maya X V2" },
        { 0x37B0, { 0x0005, 0x000B, 0x000C }, L"LAMZU Tachi" },
        { 0x37B0, { 0x001C, 0x001E, 0 }, L"LAMZU Tachi Lite" },
        { 0x37B0, { 0x0009, 0x000F, 0x0010 }, L"LAMZU Inca" },
        { 0x37B0, { 0x0011, 0x0013, 0x0015 }, L"LAMZU Maya" },
        { 0x37B0, { 0x0007, 0x000D, 0x000E }, L"LAMZU Paro" },
        { 0x37B0, { 0x0017, 0x0019, 0x001B }, L"LAMZU Thorn" },
        { 0x37B0, { 0x0021, 0x0003, 0x0023 }, L"LAMZU Thorn V2" },
        { 0x37B0, { 0x0030, 0x0032, 0x0040 }, L"LAMZU Thorn V2" },
        { 0x37B0, { 0x0025, 0x0027, 0 }, L"LAMZU Atlantis OG Champion" },
        { 0x37B0, { 0x0028, 0x002B, 0x002A }, L"LAMZU Atlantis Mini" },
        { 0x37B0, { 0x003E, 0x003F, 0 }, L"LAMZU Atlantis Mini" },
        { 0x37B0, { 0x002C, 0x0052, 0 }, L"LAMZU Maya M" },
        { 0x37B0, { 0x003A, 0x003B, 0x0044 }, L"LAMZU Maya X" },
        { 0x37B0, { 0x003C, 0x0050, 0 }, L"LAMZU Maya" },
        { 0x37B0, { 0x0042, 0x0056, 0 }, L"LAMZU Orcus" },
        { 0x37B0, { 0x0046, 0x0054, 0 }, L"LAMZU Mini" },
    };
    for (const auto& m : kModels)
        if (m.vid == vid)
            for (USHORT p : m.pids)
                if (p && p == pid) return m.name;
    return nullptr;
}

// Wired product ids: the mouse itself on the USB cable.
bool IsCable(USHORT vid, USHORT pid) {
    if (vid == 0x373E) return pid == 0x001C;
    switch (pid) {
        case 0x0005: case 0x0007: case 0x0009: case 0x0011: case 0x0017: case 0x001C: case 0x001F: case 0x0021:
        case 0x0025: case 0x0028: case 0x002C: case 0x0030: case 0x0034: case 0x0038: case 0x003A: case 0x003C:
        case 0x003E: case 0x0040: case 0x0042: case 0x0044: case 0x0046: case 0x0050: case 0x0052: case 0x0056:
            return true;
        default:
            return false;
    }
}

}  // namespace

void PollLamzu(PollContext& ctx) {
    for (USHORT vendor : { (USHORT)0x373E, (USHORT)0x37B0 }) {
        for (auto [vid, pid] : ctx.Products(vendor)) {
            if (!ModelName(vid, pid) && !IsCable(vid, pid)) continue;
            for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.featLen() == 65; })) {
                HidDev d;
                if (!d.OpenFeatureOnly(*c)) continue;
                std::vector<BYTE> req(65, 0);  // [0] = report id 0
                req[3] = 0x02;
                req[4] = 0x02;
                req[6] = 0x83;
                if (!d.SetFeature(req)) continue;
                Sleep(100);
                std::vector<BYTE> r;
                if (!d.GetFeature(0x00, r) || r.size() < 10) continue;
                int chg, pct;
                if (r[1] == 0xA1 && r[4] == 0x02 && r[6] == 0x83) { chg = r[7]; pct = r[8]; }
                else if (r[0] == 0xA1 && r[3] == 0x02 && r[5] == 0x83) { chg = r[6]; pct = r[7]; }
                else continue;
                if (pct > 100) continue;
                DeviceStatus s;
                s.key = L"lamzu:" + Hex4(vid) + L":" + Hex4(pid);
                const wchar_t* name = ModelName(vid, pid);
                s.name = name ? name : L"LAMZU wireless mouse";
                s.kind = DeviceKind::Mouse;
                bool cable = IsCable(vid, pid);
                s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
                s.connected = pct > 0 || cable;  // 0 from a dongle = mouse off or out of range
                if (pct > 0) s.percent = pct;
                s.charging = chg == 1;
                ctx.Add(s, vid, pid);
                break;
            }
        }
    }
}

}

void PollLamzu(PollContext& ctx) { wb_driver_lamzu::PollLamzu(ctx); }

namespace wb_driver_logitech {
// Logitech HID++ (VID 046D): Unifying / Bolt / Lightspeed / Nano receivers, wired or Bluetooth
// devices, and G headsets. Features are discovered through the root feature, so devices are
// supported without a per-model table.
//
// Message: [report][device index][feature index][function << 4 | sw id][params...]
//   report 0x10 = short (7 bytes), 0x11 = long (20 bytes)
//   HID++ 2.0 error: [.., 0xFF, feature, fn|sw, code]   HID++ 1.0 error: [.., 0x8F, sub id, address, code]
// Battery features: 0x1004 unified battery, 0x1000 battery level, 0x1001 battery voltage,
// 0x1F20 ADC measurement (headsets). HID++ 1.0 devices: registers 0x0D and 0x07.
// References: Logitech HID++ 2.0 specification drafts, github.com/pwr-Solaar/Solaar,
// github.com/Sapd/HeadsetControl (headset voltage calibration); PRO X Wireless: G HUB's power model
// for 046D:0ABA (pro_headset/pro_x_wireless_headset_battery.xml), checked against G HUB (BatteryHub #3434).

namespace {

constexpr BYTE kSwId = 0x0D;  // our software id; replies to other apps (G HUB, Options+) carry theirs

// LIGHTSPEED dongles that speak only HID++ 2.0 and relay a single paired device: the dongle itself
// answers the ping on index 0xFF (it has no battery feature) and the mouse is on index 1.
// 046D:C54F = PRO X3 SUPERSTRIKE dongle (Solaar PR #3352, "PASSTHROUGH_DONGLE_IDS").
bool IsPassthroughDongle(USHORT pid) { return pid == 0xC54F; }

class Hidpp {
public:
    bool Open(const HidColl* shortC, const HidColl* longC) {
        if (shortC) hasShort_ = s_.Open(*shortC);
        if (longC) hasLong_ = l_.Open(*longC);
        return hasShort_ || hasLong_;
    }
    bool hasLong() const { return hasLong_; }

    // Sends a request and waits for its reply. Returns 1 = reply (params in `p`), 0 = error reply
    // (code in `err`), -1 = no answer.
    int Request(BYTE idx, BYTE b2, BYTE b3, std::initializer_list<BYTE> params, std::vector<BYTE>& p, BYTE& err,
                bool forceShort = false, int timeoutMs = 500) {
        bool useLong = hasLong_ && !forceShort;
        if (!useLong && !hasShort_) return -1;
        std::vector<BYTE> msg(useLong ? 20 : 7, 0);
        msg[0] = useLong ? 0x11 : 0x10;
        msg[1] = idx;
        msg[2] = b2;
        msg[3] = b3;
        size_t i = 4;
        for (BYTE v : params) if (i < msg.size()) msg[i++] = v;
        if (!(useLong ? l_ : s_).Write(msg)) return -1;

        ULONGLONG end = GetTickCount64() + timeoutMs;
        std::vector<BYTE> r;
        while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
            for (HidDev* d : { &l_, &s_ }) {
                if (!d->ok() || d->Read(r, 15) < 7) continue;
                if ((r[0] != 0x10 && r[0] != 0x11) || r[1] != idx) continue;
                if (r[2] == b2 && r[3] == b3) {
                    p.assign(r.begin() + 4, r.end());
                    return 1;
                }
                if ((r[2] == 0xFF || r[2] == 0x8F) && r[3] == b2 && r[4] == b3) {
                    err = r[5];
                    return 0;
                }
            }
        }
        return -1;
    }

    // HID++ 2.0 call: feature index + function number.
    int Call(BYTE idx, BYTE feature, BYTE fn, std::initializer_list<BYTE> params, std::vector<BYTE>& p) {
        BYTE err = 0;
        return Request(idx, feature, (BYTE)((fn << 4) | kSwId), params, p, err) == 1 ? 1 : 0;
    }

    BYTE FeatureIndex(BYTE idx, USHORT featureId) {
        std::vector<BYTE> p;
        if (Call(idx, 0x00, 0, { (BYTE)(featureId >> 8), (BYTE)featureId }, p) && !p.empty()) return p[0];
        return 0;
    }

private:
    HidDev s_, l_;
    bool hasShort_ = false, hasLong_ = false;
};

// Voltage -> % for a typical Li-ion cell (mice/keyboards).
int LiIonPercent(int mv) {
    static const int v[] = { 4186, 4067, 3989, 3922, 3859, 3811, 3778, 3751, 3717, 3671, 3646, 3579, 3500 };
    static const int p[] = { 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 5, 2, 0 };
    return Interpolate(mv, v, p, 13);
}

// Voltage -> % for G headsets (calibration from HeadsetControl).
int HeadsetPercent(USHORT pid, int mv) {
    static const int g533v[] = { 4200, 3850, 3790, 3750, 3680, 3330 }, g533p[] = { 100, 50, 30, 20, 5, 0 };
    static const int g535v[] = { 4175, 3817, 3766, 3730, 3664, 3310 }, g535p[] = { 100, 50, 30, 20, 5, 0 };
    static const int g633v[] = { 4100, 3950, 3850, 3750, 3650, 3500, 3300, 3150 }, g633p[] = { 100, 80, 60, 40, 20, 10, 5, 0 };
    static const int gprov[] = { 4150, 3830, 3780, 3740, 3670, 3320 }, gprop[] = { 100, 50, 30, 20, 5, 0 };
    static const int proxv[] = { 4102, 4046, 3993, 3950, 3915, 3877, 3838, 3809, 3789, 3772, 3758, 3747, 3736, 3722, 3707, 3687, 3661, 3644, 3548, 3426, 3293 }, proxp[] = { 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0 };
    static const int defv[] = { 4100, 3850, 3780, 3730, 3650, 3300 }, defp[] = { 100, 50, 30, 20, 5, 0 };
    switch (pid) {
        case 0x0A66: return Interpolate(mv, g533v, g533p, 6);
        case 0x0AC4: return Interpolate(mv, g535v, g535p, 6);
        case 0x0A5C: case 0x0A89: case 0x0A5B: case 0x0A87: case 0x0AB5: case 0x0AFE: return Interpolate(mv, g633v, g633p, 8);
        case 0x0AAA: case 0x0ABA: return Interpolate(mv, proxv, proxp, 21);
        case 0x0AA7: case 0x0AFB: return Interpolate(mv, gprov, gprop, 6);
        default: return Interpolate(mv, defv, defp, 6);
    }
}

DeviceKind KindFromType(BYTE t) {
    switch (t) {
        case 0x00: case 0x02: return DeviceKind::Keyboard;
        case 0x03: case 0x04: case 0x05: return DeviceKind::Mouse;
        case 0x08: return DeviceKind::Headset;
        case 0x0B: case 0x0C: return DeviceKind::Gamepad;
        default: return DeviceKind::Other;
    }
}

// Fills battery fields from whichever battery feature the device has. Returns false if none.
bool ReadBattery20(Hidpp& h, BYTE idx, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> p;
    if (BYTE f = h.FeatureIndex(idx, 0x1004)) {  // UNIFIED_BATTERY: get_status (fn 1)
        if (h.Call(idx, f, 1, {}, p) && p.size() >= 3) {
            int soc = p[0], level = p[1], st = p[2];
            if (soc > 0 && soc <= 100) {
                s.percent = soc;
            } else if (level) {
                s.percent = (level & 8) ? 100 : (level & 4) ? 60 : (level & 2) ? 20 : 5;
                s.levelText = (level & 8) ? L"Full" : (level & 4) ? L"Good" : (level & 2) ? L"Low" : L"Critical";
            }
            s.charging = st == 1 || st == 2 || st == 3;
            return true;
        }
    }
    if (BYTE f = h.FeatureIndex(idx, 0x1000)) {  // BATTERY_STATUS: GetBatteryLevelStatus (fn 0)
        if (h.Call(idx, f, 0, {}, p) && p.size() >= 3) {
            int st = p[2];
            s.charging = st >= 1 && st <= 4;
            s.percent = p[0] <= 100 ? p[0] : -1;
            if (s.percent == 0 && st == 3) s.percent = 100;
            return true;
        }
    }
    if (BYTE f = h.FeatureIndex(idx, 0x1001)) {  // BATTERY_VOLTAGE: GetBatteryInfo (fn 0)
        if (h.Call(idx, f, 0, {}, p) && p.size() >= 3) {
            s.millivolts = (p[0] << 8) | p[1];
            s.charging = (p[2] & 0x80) != 0;
            if (s.millivolts) s.percent = LiIonPercent(s.millivolts);
            return true;
        }
    }
    if (BYTE f = h.FeatureIndex(idx, 0x1F20)) {  // ADC_MEASUREMENT (headsets)
        if (h.Call(idx, f, 0, {}, p) && p.size() >= 3) {
            s.millivolts = (p[0] << 8) | p[1];
            s.charging = p[2] == 0x03;
            if (s.millivolts) s.percent = HeadsetPercent(pid, s.millivolts);
            return true;
        }
    }
    return false;
}

void ReadNameAndType20(Hidpp& h, BYTE idx, DeviceStatus& s) {
    BYTE f = h.FeatureIndex(idx, 0x0005);  // DEVICE_NAME
    if (!f) return;
    std::vector<BYTE> p;
    if (h.Call(idx, f, 0, {}, p) && !p.empty()) {
        int len = p[0];
        std::string name;
        while ((int)name.size() < len) {
            if (!h.Call(idx, f, 1, { (BYTE)name.size() }, p) || p.empty()) break;
            size_t before = name.size();
            for (BYTE ch : p) {
                if ((int)name.size() >= len || !ch) break;
                name += (char)ch;
            }
            if (name.size() == before) break;
        }
        if (!name.empty()) {
            std::wstring w(name.begin(), name.end());
            s.name = (w.rfind(L"Logitech", 0) == 0 || w.rfind(L"G ", 0) == 0 || w.rfind(L"MX", 0) == 0) ? w : L"Logitech " + w;
        }
    }
    if (h.Call(idx, f, 2, {}, p) && !p.empty()) s.kind = KindFromType(p[0]);
}

// DEVICE_INFORMATION (0x0003) getDeviceInfo: [entity count][unit id x4][transport x2][model id x6]...
// The unit id identifies the physical device, so a mouse keeps one entry whether it is reached
// through its receiver or directly over its charging cable (where it shows up under another
// product id and device index). Returns an empty string if the device does not report one.
std::wstring ReadUnitId20(Hidpp& h, BYTE idx) {
    BYTE f = h.FeatureIndex(idx, 0x0003);
    if (!f) return L"";
    std::vector<BYTE> p;
    if (!h.Call(idx, f, 0, {}, p) || p.size() < 5) return L"";
    if (!p[1] && !p[2] && !p[3] && !p[4]) return L"";
    return Hex4((USHORT)((p[1] << 8) | p[2])) + Hex4((USHORT)((p[3] << 8) | p[4]));
}

// HID++ 1.0 devices: register 0x0D (battery %) or 0x07 (battery level 1..7).
bool ReadBattery10(Hidpp& h, BYTE idx, DeviceStatus& s) {
    std::vector<BYTE> p;
    BYTE err;
    if (h.Request(idx, 0x81, 0x0D, {}, p, err, true) == 1 && p.size() >= 3 && p[0] && p[0] <= 100) {
        s.percent = p[0];
        s.charging = p[2] >= 0x21 && p[2] <= 0x26;
        return true;
    }
    if (h.Request(idx, 0x81, 0x07, {}, p, err, true) == 1 && p.size() >= 2 && p[0]) {
        static const int pct[] = { 0, 5, 10, 20, 35, 50, 75, 100 };
        s.percent = pct[p[0] > 7 ? 7 : p[0]];
        s.levelText = p[0] >= 7 ? L"Full" : p[0] >= 5 ? L"Good" : p[0] >= 3 ? L"Low" : L"Critical";
        s.charging = p[1] >= 0x21 && p[1] <= 0x26;
        return true;
    }
    return false;
}

// Identifies the physical device a HID collection belongs to, so two receivers with the same
// product id (e.g. two LIGHTSPEED C547 dongles) are polled separately. A collection path looks like
//   \\?\hid#vid_046d&pid_c547&mi_02&col01#8&2a2b3c4d&0&0000#{guid}
// and the collections of one interface differ only in "&colNN" and the last "&NNNN" of the
// instance part. Returns an empty string for a path of another shape (all such paths then share
// one group).
std::wstring PhysicalKey(const HidColl& c) {
    std::wstring p = c.path;
    for (auto& ch : p) ch = towlower(ch);
    size_t a = p.find(L'#');
    size_t b = a == std::wstring::npos ? a : p.find(L'#', a + 1);
    size_t e = b == std::wstring::npos ? b : p.find(L'#', b + 1);
    if (e == std::wstring::npos) return L"";
    std::wstring hw = p.substr(a + 1, b - a - 1), inst = p.substr(b + 1, e - b - 1);
    size_t col = hw.find(L"&col");
    if (col != std::wstring::npos) hw.erase(col, 6);
    size_t amp = inst.rfind(L'&');
    if (amp != std::wstring::npos) inst.erase(amp);
    return hw + L"#" + inst;
}

// Polls one physical device (receiver, cable or Bluetooth link) given its HID++ collections.
// `suffix` keeps the fallback keys of identical receivers apart. Returns true if it reported a device.
bool PollLogitechDevice(PollContext& ctx, USHORT pid, const std::vector<const HidColl*>& shorts,
                        const std::vector<const HidColl*>& longs, const std::wstring& suffix) {
    if (shorts.empty() && longs.empty()) return false;
    Hidpp h;
    if (!h.Open(shorts.empty() ? nullptr : shorts[0], longs.empty() ? nullptr : longs[0])) return false;

    std::wstring product = HidProduct(longs.empty() ? *shorts[0] : *longs[0]);
    bool isReceiver = product.find(L"Receiver") != std::wstring::npos || product.find(L"USB Receiver") != std::wstring::npos;

    // A device answering a HID++ 2.0 ping at index 0xFF is connected directly (USB cable,
    // Bluetooth, or a receiver-less headset); otherwise it is a receiver with paired devices.
    std::vector<BYTE> p;
    BYTE err = 0;
    std::vector<BYTE> indices;
    // A Bluetooth HID path (HOGP service 1812, classic HID 1124) is always a direct link. A sleepy
    // Bluetooth mouse can miss the 400 ms ping; probing it as a receiver then answers on every
    // index and lists the same mouse six times.
    std::wstring path = (longs.empty() ? shorts[0] : longs[0])->path;
    for (auto& ch : path) ch = towlower(ch);
    bool bluetooth = path.find(L"{00001812-") != std::wstring::npos || path.find(L"{00001124-") != std::wstring::npos;
    const bool passthrough = IsPassthroughDongle(pid);
    if (passthrough)
        indices = { 1 };
    else if (bluetooth ||
             (!isReceiver && h.Request(0xFF, 0x00, 0x10 | kSwId, { 0, 0, 0x5A }, p, err, false, 400) == 1 && p.size() >= 3 && p[0] >= 2))
        indices = { 0xFF };
    else
        indices = { 1, 2, 3, 4, 5, 6 };

    bool any = false;
    for (BYTE idx : indices) {
        DeviceStatus s;
        s.key = L"logi:" + Hex4(pid) + suffix + L":" + std::to_wstring(idx);
        s.name = (idx == 0xFF && !product.empty()) ? product : L"Logitech wireless device";
        s.via = idx == 0xFF ? L"USB / Bluetooth" : L"Logitech receiver";
        s.graceSec = idx == 0xFF ? 0 : 1800;  // receiver devices drop off the link while sleeping
        s.kind = passthrough ? DeviceKind::Mouse : GuessKind(s.name);

        int r = h.Request(idx, 0x00, 0x10 | kSwId, { 0, 0, 0x5A }, p, err, false, 400);
        bool found = false;
        if (r == 1 && p.size() >= 3 && p[0] >= 2) {
            ReadNameAndType20(h, idx, s);
            found = ReadBattery20(h, idx, pid, s);
            std::wstring unit = found ? ReadUnitId20(h, idx) : L"";
            if (!unit.empty()) {
                s.key = L"logi:unit:" + unit;
                // Unplugging the cable hands a receiver device back to its receiver; keep the
                // entry for a minute so it does not blink out while the link comes back.
                if (idx == 0xFF) s.graceSec = 60;
            }
            if (found && s.kind == DeviceKind::Other && longs.size() && longs[0]->page() == 0xFF43) s.kind = DeviceKind::Headset;
        } else if (r == 0 && err == 0x01) {  // "invalid sub id": a HID++ 1.0 device
            found = ReadBattery10(h, idx, s);
        }
        if (found) {
            if (s.kind == DeviceKind::Other) s.kind = GuessKind(s.name);
            ctx.out.push_back(s);
            any = true;
        }
    }
    return any;
}

}  // namespace

void PollLogitech(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x046D)) {
        const bool passthrough = IsPassthroughDongle(pid);
        // The SUPERSTRIKE receiver exposes HID++ on FF43:0301 (short) and FF43:0302 (long).
        auto allShorts = ctx.Find(vid, pid, [passthrough](const HidColl& c) {
            return c.outLen() == 7 &&
                   ((c.page() == 0xFF00 && c.usage() == 0x0001) ||
                    (passthrough && c.page() == 0xFF43 && c.usage() == 0x0301));
        });
        auto allLongs = ctx.Find(vid, pid, [passthrough](const HidColl& c) {
            return c.outLen() == 20 &&
                   ((c.page() == 0xFF00 && c.usage() == 0x0002) ||
                    (c.page() == 0xFF43 && (c.usage() == 0x0202 || (passthrough && c.usage() == 0x0302))));
        });

        // One group per physical device: its short and long HID++ collections.
        std::vector<std::wstring> groups;
        for (const auto* list : { &allShorts, &allLongs })
            for (const HidColl* c : *list) {
                std::wstring k = PhysicalKey(*c);
                if (std::find(groups.begin(), groups.end(), k) == groups.end()) groups.push_back(k);
            }

        bool any = false;
        for (size_t g = 0; g < groups.size(); ++g) {
            std::vector<const HidColl*> shorts, longs;
            for (const HidColl* c : allShorts) if (PhysicalKey(*c) == groups[g]) shorts.push_back(c);
            for (const HidColl* c : allLongs) if (PhysicalKey(*c) == groups[g]) longs.push_back(c);
            // The first device keeps the plain key; a second identical receiver gets "#2", ...
            std::wstring suffix = g ? L"#" + std::to_wstring(g + 1) : L"";
            if (PollLogitechDevice(ctx, pid, shorts, longs, suffix)) any = true;
        }
        if (any) ctx.Claim(vid, pid);
    }
}

}

void PollLogitech(PollContext& ctx) { wb_driver_logitech::PollLogitech(ctx); }


namespace wb_driver_machenike {
// Machenike L7 Arctic / L7 Mix. Read-state protocol from the official L7 Mix
// app 1.0.0.8 (DeviceDriver.exe), verified on an L7 Arctic over receiver and cable.
// Feature report 51, command 06, 21 bytes: [4..7] 8A 24 4E FF (L7 signature), [10] mode/link,
// [11] %, [12] charging. No setting, mode, macro or firmware commands are sent.
namespace {
constexpr USHORT kVendor = 0x248A, kReceiver = 0xFF4C, kCable = 0xFF4E;
constexpr USHORT kStatusLength = 21;

bool IsStatusCollection(const HidColl& c, bool cable) {
    return c.iface == (cable ? 2 : 1) && c.page() == 0x008C && c.usage() == 1 &&
           c.inLen() == 33 && c.outLen() == 33 && c.featLen() == 1001;
}

bool DecodeStatus(const std::vector<BYTE>& r, bool cable, DeviceStatus& s) {
    if (r.size() != kStatusLength || r[0] != 0x51 || r[1] != 0x06 ||
        r[4] != 0x8A || r[5] != 0x24 || r[6] != 0x4E || r[7] != 0xFF ||
        (r[10] & 7) != (cable ? 0 : 1)) return false;
    // The wireless link bit is clear on the cable too; the USB connection itself is authoritative there.
    bool connected = cable || (r[10] & 8) != 0;
    if (connected && (r[11] > 100 || r[12] > 1)) return false;
    s.connected = connected;
    s.percent = connected ? r[11] : -1;  // 0% is valid, not an offline sentinel
    s.charging = connected && r[12] == 1;
    return true;
}

bool QueryStatus(HidDev& d, bool cable, DeviceStatus& s) {
    std::vector<BYTE> request(kStatusLength, 0);
    request[0] = 0x51;
    request[1] = 0x06;
    ULONGLONG deadline = GetTickCount64() + 200;
    if (!d.SetFeature(request)) return false;
    // Allow the reply to replace the receiver's previous feature response. An open vendor app
    // can leave unrelated replies here, so accept only this command's validated status frame.
    do {
        Sleep(10);
        std::vector<BYTE> r;
        if (d.GetFeature(0x51, r) && DecodeStatus(r, cable, s)) return true;
    } while (GetTickCount64() < deadline);
    return false;
}
}  // namespace

void PollMachenike(PollContext& ctx) {
    bool haveCable = false;
    // A live cable reading takes precedence over the dongle's cached/disconnected mouse state.
    for (USHORT pid : { kCable, kReceiver }) {
        if (ctx.IsClaimed(kVendor, pid)) continue;
        bool cable = pid == kCable;
        for (const HidColl* c : ctx.Find(kVendor, pid, [cable](const HidColl& c) { return IsStatusCollection(c, cable); })) {
            if (HidProduct(*c) != L"L7 Mix") continue;
            // Reserve even on I/O failure, so that no later driver for the shared 248A vendor id
            // sends its different command to the L7's collections.
            ctx.Claim(kVendor, pid);
            if (haveCable) break;
            HidColl status = *c;
            // 1001 is the largest report on this collection (macro data). Report 51 is 21 bytes;
            // use its exact length in the common HID layer, as the official application does.
            status.caps.FeatureReportByteLength = kStatusLength;
            HidDev d;
            if (!d.OpenFeatureOnly(status)) break;
            DeviceStatus s;
            if (!QueryStatus(d, cable, s)) break;
            s.key = L"machenike:l7";  // keep the same tray entry when switching cable/receiver
            s.name = L"Machenike L7 Arctic / L7 Mix";
            s.kind = DeviceKind::Mouse;
            s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
            s.graceSec = cable ? 0 : 1800;
            haveCable = cable;
            ctx.Add(s, kVendor, pid);
            break;  // one exact status collection and one bounded query per product
        }
    }
}

}

void PollMachenike(PollContext& ctx) { wb_driver_machenike::PollMachenike(ctx); }
namespace wb_driver_mchose_a5 {
// MCHOSE first-generation A5 mice (VID 2023): A5 Pro and A5 Pro Max. These predate M HUB and use
// a different chip and protocol from MCHOSE's 5253 / 3837 families:
//   2023:F013  1K 2.4 GHz receiver (shared by A5 Pro and A5 Pro Max; product string "MCHOSE A5 2.4G")
//   2023:F015  4K 2.4 GHz receiver (shared as well)
//   2023:F017  A5 Pro on its USB cable
//   2023:F018  A5 Pro Max 1K receiver (Klegus/mchose-macos table)
//   2023:F019  A5 Pro Max on its USB cable
// Transport: 64-byte feature reports (65 with the report id) on the vendor collection, usage page
// FFFF (interface 2), report id 0. Only the battery read is sent:
//   request: [00 report id][00 00][route 02 = mouse][len 02][page 00][cmd 83][00 ...]
//   reply:   read back with a get-feature on report id 0:
//            [00 report id][A1 success][..][..][..][page 00][cmd 83][charging][level]
//            (0xA1 = OK, 0xA3 = unsupported; page and command are echoed)
// The mouse only answers while it is awake, so the reference writes once, waits 34 ms, then reads
// back up to eight times with one re-send in the middle; a silent receiver means the mouse is
// asleep or off.
// Sources:
//   - github.com/z750sasr/mchose-a5-pro-max-web-driver (MIT), docs/PROTOCOL.md: VID 2023, PIDs
//     F019 / F013 / F015, usage page FFFF, 64-byte feature reports, route / length / page / command
//     layout, A1 reply marker echoing page and command, page 00 carries the battery.
//   - github.com/HeyOkay/HaloBattery (MIT), providers/mchose.py: battery command 0x83 on page 0,
//     charging in reply byte 6 and level in byte 7 (after the A1 marker), the 34 ms / 8-read loop;
//     confirmed on a real A5 Pro Max on the F013 receiver (HaloBattery issue #149).
//   - github.com/Klegus/mchose-macos (MIT), README + mchose_config.py: the A5 Pro / A5 Pro Max id
//     table (F013, F015, F017, F018, F019, all VID 2023), interface 2 / usage page FFFF, 65-byte
//     feature reports with report id 0, 0xA1 = OK; reverse-engineered from MCHOSE's DriverCore.exe
//     and tested on an A5 Pro with the 4K receiver.
//   - github.com/OpenMouse-Project/mouse-protocol, docs/mchose-a5-gen1.md (documentation only):
//     the vendor's model table pairs F013 / F015 receivers with the F019 wired mouse.
// Not tested here on real hardware. F017 and F018 come from the Klegus table only.
// VID 2023 is not used by any other driver (Glorious' 0x2023 is a product id under VID 258A).

namespace {

const USHORT kVid = 0x2023;
const BYTE kReportId = 0x00;
const BYTE kRouteMouse = 0x02;
const BYTE kPage = 0x00;
const BYTE kCmdBattery = 0x83;
const BYTE kOk = 0xA1;

struct Model {
    USHORT pid;
    const wchar_t* name;
    bool cable;
};

const Model kModels[] = {
    { 0xF013, L"MCHOSE A5 Pro / Pro Max", false },
    { 0xF015, L"MCHOSE A5 Pro / Pro Max", false },
    { 0xF017, L"MCHOSE A5 Pro", true },
    { 0xF018, L"MCHOSE A5 Pro Max", false },
    { 0xF019, L"MCHOSE A5 Pro Max", true },
};

std::vector<BYTE> Request(size_t len) {
    std::vector<BYTE> req(len, 0);
    req[0] = kReportId;
    req[3] = kRouteMouse;
    req[4] = 2;  // payload length, as in the reference
    req[5] = kPage;
    req[6] = kCmdBattery;
    return req;
}

// r[0] is the report id on Windows; the A1 marker follows. Returns true for a battery reply.
bool Parse(const std::vector<BYTE>& r, int& level, bool& charging) {
    if (r.size() < 9) return false;
    if (r[0] != kOk && r[1] != kOk) return false;
    size_t at = r[1] == kOk ? 1 : 0;
    if (r[at + 4] != kPage || r[at + 5] != kCmdBattery) return false;
    BYTE lv = r[at + 7];
    if (lv == 0xFF) return false;
    level = lv > 100 ? 100 : lv;  // the reference clamps to 100
    charging = r[at + 6] != 0;
    return true;
}

// 1 = battery read, 0 = collection works but the mouse did not answer, -1 = could not talk to it.
int Query(const HidColl& c, int& level, bool& charging) {
    HidDev d;
    if (!d.OpenFeatureOnly(c)) return -1;
    std::vector<BYTE> req = Request(c.featLen());
    if (!d.SetFeature(req)) return -1;
    Sleep(34);
    for (int attempt = 0; attempt < 8; attempt++) {
        if (attempt == 3) {
            if (!d.SetFeature(req)) return -1;
            Sleep(34);
        }
        std::vector<BYTE> r;
        if (d.GetFeature(kReportId, r) && Parse(r, level, charging)) return 1;
        Sleep(30);
    }
    return 0;
}

}  // namespace

void PollMchoseA5(PollContext& ctx) {
    static std::set<USHORT> answered;  // a silent receiver is only shown as asleep once its mouse has answered
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        auto colls = ctx.Find(kVid, m.pid, [](const HidColl& c) { return c.page() == 0xFFFF && c.featLen() >= 65; });
        // interface 2 first: that is where the references talk to it
        std::stable_sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) { return (a->iface == 2) > (b->iface == 2); });
        int state = -1, level = 0;
        bool charging = false;
        for (const HidColl* c : colls) {
            int st = Query(*c, level, charging);
            if (st > state) state = st;
            if (st == 1) break;
        }
        if (state < 0) continue;
        DeviceStatus s;
        s.key = L"mchose-a5:" + Hex4(m.pid);
        s.name = m.name;
        s.kind = DeviceKind::Mouse;
        s.via = m.cable ? L"USB cable" : L"2.4 GHz receiver";
        if (state == 1) {
            answered.insert(m.pid);
            s.percent = level;
            s.charging = charging;
        } else {
            if (m.cable || !answered.count(m.pid)) continue;
            s.asleep = true;
        }
        ctx.Add(s, kVid, m.pid);
    }
}

}

void PollMchoseA5(PollContext& ctx) { wb_driver_mchose_a5::PollMchoseA5(ctx); }
namespace wb_driver_mchose_g3 {
// MCHOSE G3 V2 wireless mouse (VID 3837): 2.4 GHz receiver 3837:4245 and USB cable 3837:418C, on
// vendor collection FF01:0010 (65-byte input/output reports).
// Protocol from github.com/Jstdi001/MCHOSE-Monitor (MIT, docs/HID_PROTOCOL.md: battery query, one of
// the two read-only queries that tool sends), verified on MCHOSE G3 V2:
//   Output report (report ID 0, 64-byte payload zero-padded):
//     [00][55 30 A5 0B 2E 01 01 01 00 00 ...]
//   Input reply (after stripping leading 0 report ID):
//     [0] = AA, [1] = 30, [8] = percent (1..100), [9] = charging (1 = charging)

namespace {

constexpr USHORT kMchoseVid = 0x3837;
constexpr USHORT kG3V2WiredPid = 0x418C;
constexpr USHORT kG3V2WirelessPid = 0x4245;

bool QueryG3V2(const HidColl& c, int& percent, bool& charging) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    if (!d.Write({ 0x00, 0x55, 0x30, 0xA5, 0x0B, 0x2E, 0x01, 0x01, 0x01, 0x00, 0x00 })) return false;

    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 1000;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        int n = d.Read(r, left);
        if (n < 0) break;
        if (n >= 10 && r[0] == 0xAA && r[1] == 0x30) {
            if (r[8] < 1 || r[8] > 100) return false;
            percent = r[8];
            charging = r[9] == 1;
            return true;
        }
    }
    return false;
}

}  // namespace

void PollMchoseG3(PollContext& ctx) {
    // Prefer wired over 2.4 GHz when both are connected at the same time.
    bool found = false;
    for (USHORT pid : { kG3V2WiredPid, kG3V2WirelessPid }) {
        if (ctx.IsClaimed(kMchoseVid, pid)) continue;
        for (const HidColl* c : ctx.Find(kMchoseVid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF01 && c.usage() == 0x0010 && c.outLen() >= 65 && c.inLen() >= 65;
             })) {
            if (found) {
                ctx.Claim(kMchoseVid, pid);
                break;
            }
            int pct = -1;
            bool chg = false;
            if (!QueryG3V2(*c, pct, chg)) continue;

            DeviceStatus s;
            s.key = L"mchose:g3v2";
            s.name = HidProduct(*c);
            if (s.name.empty()) s.name = L"MCHOSE G3 V2";
            s.kind = DeviceKind::Mouse;
            s.via = pid == kG3V2WiredPid ? L"USB cable" : L"2.4 GHz receiver";
            s.percent = pct;
            s.charging = chg;
            s.graceSec = 1800;
            ctx.Add(s, kMchoseVid, pid);
            found = true;
            break;
        }
    }
}

}

void PollMchoseG3(PollContext& ctx) { wb_driver_mchose_g3::PollMchoseG3(ctx); }
namespace wb_driver_mchose_headset {
// MCHOSE wireless headsets on the "MC" protocol (VID 3837). Supports the MCHOSE V9 Pro 2
// (2.4 GHz dongle 3837:6011) on vendor collection FF07:0222 (report ID 0x55, 63-byte input/output reports).
// The MCHOSE R9 (3837:6013) uses the same protocol in M HUB but is not enabled here because it is untested.
//
// Protocol from MCHOSE's own M HUB driver (cdn.mchose.com.cn customPage bundle: buildMCFrame,
// createReadBatteryCommand, parseBatteryResponse; MC_HEADSET_REPORT_ID = 85), verified on a V9 Pro 2:
//   Frame (after the report ID): [0] AA start, [1] 01 version, [2] flags (1 = XOR checksum), [3] length
//   (sub-command + body), [4..5] message type (00 01 request/response, 00 04 notify), [6] sub-command,
//   [7..] body, then one checksum byte (0 when flags is 0).
//   Read battery: [55][AA 01 00 01 00 01 0B 00 ...] (read-only). The reply, and the headset's own
//   battery notification (sub-command 0x22), carry body[0] = 0 when the headset is off or asleep,
//   otherwise body[1] = charging (bit 7) | percent (bits 0..6).
namespace {

constexpr USHORT kMchoseVid = 0x3837;
constexpr USHORT kV9Pro2Pid = 0x6011;
constexpr BYTE kReportId = 0x55;
constexpr BYTE kStart = 0xAA;
constexpr BYTE kVersion = 0x01;
constexpr BYTE kReadBattery = 0x0B;
constexpr BYTE kNotifyBattery = 0x22;

enum class Reply { None, Off, Level };

// Checks one input report (r[0] = report ID) for an MC battery reply or battery notification.
Reply ParseBattery(const std::vector<BYTE>& r, int& percent, bool& charging) {
    if (r.size() < 9 || r[0] != kReportId) return Reply::None;
    const BYTE* f = r.data() + 1;
    size_t n = r.size() - 1;
    if (f[0] != kStart || f[1] != kVersion || f[4] != 0x00) return Reply::None;
    bool reply = f[5] == 0x01 && f[6] == kReadBattery;
    bool notify = f[5] == 0x04 && f[6] == kNotifyBattery;
    if (!reply && !notify) return Reply::None;

    size_t len = f[3];  // sub-command + body
    if (len < 3 || 6 + len >= n) return Reply::None;
    BYTE check = 0;
    if (f[2] == 1) {
        check = f[2] ^ f[3] ^ f[4] ^ f[5];
        for (size_t i = 0; i < len; i++) check ^= f[6 + i];
    }
    if (f[6 + len] != check) return Reply::None;

    const BYTE* body = f + 7;
    if (body[0] == 0) return Reply::Off;
    percent = body[1] & 0x7F;
    charging = (body[1] & 0x80) != 0;
    return percent <= 100 ? Reply::Level : Reply::None;
}

Reply QueryV9Pro2(const HidColl& c, int& percent, bool& charging) {
    HidDev d;
    if (!d.Open(c)) return Reply::None;
    d.Flush();
    // Request frame: start, version, flags 0, length 1, type 00 01, sub-command, checksum 0.
    if (!d.Write({ kReportId, kStart, kVersion, 0x00, 0x01, 0x00, 0x01, kReadBattery, 0x00 })) return Reply::None;

    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 1000;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 0) break;
        Reply got = ParseBattery(r, percent, charging);
        if (got != Reply::None) return got;
    }
    return Reply::None;
}

}  // namespace

void PollMchoseHeadset(PollContext& ctx) {
    if (ctx.IsClaimed(kMchoseVid, kV9Pro2Pid)) return;
    for (const HidColl* c : ctx.Find(kMchoseVid, kV9Pro2Pid, [](const HidColl& c) {
             return c.page() == 0xFF07 && c.usage() == 0x0222 && c.outLen() >= 64 && c.inLen() >= 64;
         })) {
        int pct = -1;
        bool chg = false;
        Reply got = QueryV9Pro2(*c, pct, chg);
        if (got == Reply::None) continue;

        DeviceStatus s;
        s.key = L"mchose:v9pro2";
        s.name = HidProduct(*c);
        if (s.name.empty()) s.name = L"MCHOSE V9 Pro 2";
        s.kind = DeviceKind::Headset;
        s.via = L"2.4 GHz receiver";
        if (got == Reply::Off) {
            s.asleep = true;  // the dongle answered, the headset is off or asleep: keep the last value
        } else {
            s.percent = pct;
            s.charging = chg;
        }
        s.graceSec = 1800;
        ctx.Add(s, kMchoseVid, kV9Pro2Pid);
        break;
    }
}

}

void PollMchoseHeadset(PollContext& ctx) { wb_driver_mchose_headset::PollMchoseHeadset(ctx); }
namespace wb_driver_mchose_keyboard {
// MCHOSE keyboards on the current common 2.4 GHz receiver, VID 3837 PID 3033 (product string
// "MCHOSE <model> 2.4G"; the G75 V2, G87 V2, K99 V3 and K87S all ship with it). M HUB pairs this
// receiver with two different protocols depending on the keyboard: the G75 V2 / G87 V2 / K99 V3
// speak "GLW" frames on the id-less 64-byte collection 0001:0000 (the FF31:0074 collection answers
// the same frames as a fallback), while the K87S speaks the "QHW" report-0x4D protocol behind
// different report descriptors. This driver implements GLW only: a QHW receiver never answers
// these frames, so it stays hidden instead of showing a phantom entry.
//   request: 64-byte output report without a report id:
//            [0]=0x55 flag [1]=command [2]=0x00 [3]=checksum [4]=size [5]=offset lo [6]=offset hi
//            [7]=0x00, zero padded; checksum = (size + offset + payload) & 0xFF, reads have no payload
//   reply:   input report with the same header, flag 0xAA, payload from byte 8 on
//   battery: "func config" read (command 0x05, offset 0, size 56): payload[32] bit 7 = charging,
//            bits 0..6 = battery percent (85 % while discharging on the test G75 V2)
// Protocol from MCHOSE's own M HUB web driver (cdn.mchose.com.cn customPage bundle, "Channel:Keyboard"
// GLW class: _simpleGetCommand / getFuncConfig / parseBaseFuncConfigStruct), confirmed on a real
// MCHOSE G75 V2 receiver (3837:3033): getInfo answers with a build-date string ("Jan 12 2026..."
// "...2.4G Dongle"), the func-config checksum matches byte for byte, and the battery byte follows
// the keyboard. The battery pushes (FA FB 07 <percent> <charge>) that M HUB also listens for were
// never observed on this receiver, so the value is polled instead.
// Like the ATK keyboard, the receiver is asked at most every 10 minutes rather than every poll:
// the keyboard decides on its own when to sleep, and the app should not keep it awake.

namespace {

constexpr USHORT kVid = 0x3837;
constexpr USHORT kPid = 0x3033;
constexpr BYTE kFlag = 0x55;          // request flag; replies use 0xAA
constexpr BYTE kCmdFuncConfig = 0x05;
constexpr size_t kPayload = 8;        // payload starts at byte 8 of the reply
constexpr size_t kBattery = 32;       // battery byte inside the func-config payload
constexpr ULONGLONG kAskIntervalMs = 10 * 60 * 1000;

std::wstring DisplayName(const HidColl& c) {
    std::wstring name = HidProduct(c);
    const wchar_t* suffix = L" 2.4G";  // "MCHOSE G75 V2 2.4G" -> "MCHOSE G75 V2"
    const size_t n = wcslen(suffix);
    if (name.size() > n && name.compare(name.size() - n, n, suffix) == 0) name.resize(name.size() - n);
    return name.empty() ? std::wstring(L"MCHOSE keyboard") : name;
}

bool ParseFuncConfig(const std::vector<BYTE>& r, int& percent, bool& charging) {
    if (r.size() < kPayload + kBattery + 1) return false;
    if (r[0] != 0xAA || r[1] != kCmdFuncConfig || r[5] || r[6]) return false;  // offset 0
    size_t size = r[4];
    if (size < kBattery + 1 || kPayload + size > r.size()) return false;
    int sum = r[4] + r[5] + r[6] + r[7];
    for (size_t i = 0; i < size; i++) sum += r[kPayload + i];
    if ((sum & 0xFF) != r[3]) return false;
    percent = r[kPayload + kBattery] & 0x7F;
    charging = (r[kPayload + kBattery] >> 7) != 0;
    return percent <= 100;
}

bool QueryBattery(const HidColl& c, int& percent, bool& charging) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    for (int attempt = 0; attempt < 2; attempt++) {
        if (attempt) Sleep(150);
        // Read command, 56 bytes at offset 0: [report id 0] flag, command, 0x00, checksum, size, offset.
        if (!d.Write({ 0x00, kFlag, kCmdFuncConfig, 0x00, 56, 56, 0x00, 0x00, 0x00 })) continue;
        ULONGLONG end = GetTickCount64() + 500;
        std::vector<BYTE> r;
        while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
            int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
            if (d.Read(r, left) < 0) break;
            if (ParseFuncConfig(r, percent, charging)) return true;
        }
    }
    return false;
}

}  // namespace

void PollMchoseKeyboard(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kPid)) return;
    static ULONGLONG lastAsk = 0;
    static int lastPercent = -1;
    static bool lastCharging = false;

    std::vector<const HidColl*> cs = ctx.Find(kVid, kPid, [](const HidColl& c) {
        return c.inLen() == 65 && c.outLen() == 65 &&
               ((c.page() == 0x0001 && c.usage() == 0x0000) ||  // M HUB's filter for this receiver
                (c.page() == 0xFF31 && c.usage() == 0x0074));   // answers the same frames
    });
    if (cs.empty()) return;  // receiver not present

    const ULONGLONG now = GetTickCount64();
    const bool ask = !lastAsk || now - lastAsk >= kAskIntervalMs;
    int percent = -1;
    bool charging = false;
    if (ask) {
        lastAsk = now;
        for (const HidColl* c : cs) {
            if (QueryBattery(*c, percent, charging)) break;
        }
    }
    if (percent < 0 && !ask) {  // between asks: keep showing the previous reading
        percent = lastPercent;
        charging = lastCharging;
    }
    if (percent < 0 && lastPercent < 0) return;  // never answered: QHW receiver (K87S) or dead link
    if (percent >= 0) {
        lastPercent = percent;
        lastCharging = charging;
    }

    DeviceStatus s;
    s.key = L"mchose:3837:3033";
    s.name = DisplayName(*cs.front());
    s.kind = DeviceKind::Keyboard;
    s.via = L"2.4 GHz receiver";
    s.graceSec = 1800;
    if (percent >= 0) {
        s.percent = Clamp100(percent);
        s.charging = charging;
    } else {
        s.asleep = true;  // asked but no reply: keyboard off, asleep or busy; keep the last value
    }
    ctx.Add(s, kVid, kPid);
}

}

void PollMchoseKeyboard(PollContext& ctx) { wb_driver_mchose_keyboard::PollMchoseKeyboard(ctx); }
namespace wb_driver_mchose_modern {
// MCHOSE "new" mice (VID 3837): the K7 V2 Ultra+ on its 2.4 GHz receiver (3837:1014) or on its USB
// cable (3837:4028), plus the other Nordic models M HUB groups with the receiver (ids 0x1018,
// 0x1016, 0x102C, 0x102D). Both the receiver and the cable product expose vendor collection
// FF01:0001 (64-byte input/output reports) and answer the same 64-byte "device info" frame:
//   request: output report id 0x4D:
//            [4D][01][flags 01 = read][len 00][cmd lo][cmd hi][biz][seq][xor of bytes 2..7]
//            command 0x0900 = read device info, which carries the battery
//   reply:   input report id 0x4D, same header, then len bytes of data and a checksum:
//            [4D][01][flags][len][cmd lo][cmd hi][biz][seq][data ...][xor of bytes 2..7+len]
//            data[0..1] = vendor id (3837), data[2..3] = paired model id, data[10] = link status
//            (0 = not linked, 1 = connected over the USB cable, 2 = linked to the receiver),
//            data[11] = charge status (1 = charging), data[12] = battery percent, data[13] = game mode
//            With the mouse switched off the receiver still answers, but with len = 1 and a single
//            0xFF in place of the model id (4D 01 01 01 00 09 00 00 FF 00 ...): no battery data, so
//            the device is reported as asleep rather than dropped. Verified on the K7 V2 Ultra+.
// Protocol from MCHOSE's own M HUB web driver (cdn.mchose.com.cn, assets/main-DQZs6ZzX.js:
// getCommandBuffer, mcReadCommand, readDeviceInfo with commandId 0x0900, deviceInfoToObj), and the
// same frame is documented in github.com/HifumiDaisuki01/MchoseBatteryTray (PROTOCOL.md,
// ModernRequest/ParseModern). Confirmed on a real K7 V2 Ultra+ receiver (3837:1014): 29% reported,
// matching M HUB, and on the same mouse plugged in by cable (3837:4028): 25% with charge status 1.
// The G3 V2 driver speaks a different protocol on FF01:0010; this one needs FF01:0001
// and report 0x4D, which the 0x1014 receiver does not expose through the feature-report path.
// K87 V3 keyboard receiver 3837:1015 uses the same command on FF80:0001, with a 20-byte payload
// and paired model 2023. Hardware reports and protocol discovery: HOSTErss, BatteryHub PR #2934.
// Keyboard queries are spaced three minutes apart to avoid repeatedly waking an idle keyboard.

namespace {

const USHORT kVid = 0x3837;
// Receiver ids from M HUB's newMouseWirelessPids; 0x4028 is the K7 V2 Ultra+ on its USB cable.
// Only 0x1014 (receiver) and 0x4028 (cable) were tested, the rest is what M HUB groups with them.
struct Target {
    USHORT pid;
    const wchar_t* via;
    bool keyboard = false;
};
const Target kTargets[] = {
    { 0x1014, L"2.4 GHz receiver" },
    { 0x1018, L"2.4 GHz receiver" },
    { 0x1016, L"2.4 GHz receiver" },
    { 0x102C, L"2.4 GHz receiver" },
    { 0x102D, L"2.4 GHz receiver" },
    { 0x4028, L"USB cable" },
    { 0x1015, L"2.4 GHz receiver", true },
};

const BYTE kReport = 0x4D;
const USHORT kCmdDeviceInfo = 0x0900;
const size_t kData = 8;  // first byte of the data block in both the request and the reply

BYTE Xor(const std::vector<BYTE>& b, size_t from, size_t to) {  // both ends inclusive
    BYTE x = 0;
    for (size_t i = from; i <= to && i < b.size(); i++) x ^= b[i];
    return x;
}

// Returns 1 for a status frame, 0 for a valid frame that says the mouse is not linked, -1 otherwise.
int ParseInfo(const std::vector<BYTE>& r, bool keyboard, int& percent, bool& charging) {
    if (r.size() < kData + 2 || r[0] != kReport) return -1;
    if ((USHORT)(r[4] | (r[5] << 8)) != kCmdDeviceInfo) return -1;
    const size_t len = r[3];
    if (kData + len >= r.size()) return -1;
    // While the mouse is off or asleep the receiver answers with the header only, a single 0xFF in
    // place of the model id and a zeroed data area (4D 01 01 01 00 09 00 00 FF 00 ...), so anything
    // shorter than the 14-byte device-info payload means "not linked" rather than "bad checksum".
    if (len < 14) return keyboard ? (len == 1 && r[kData] == 0xFF ? 0 : -1) : 0;
    if (r[kData + len] != Xor(r, 2, 7 + len)) return -1;
    if ((USHORT)(r[kData] | (r[kData + 1] << 8)) != kVid) return -1;
    if (keyboard && (len < 20 || r[kData + 2] != 0x23 || r[kData + 3] != 0x20)) return -1;
    if (r[kData + 10] == 0) return 0;  // not linked: the mouse is off or asleep
    if (r[kData + 12] > 100) return -1;
    percent = r[kData + 12];
    charging = r[kData + 11] == 1;
    return 1;
}

struct KeyboardPoll {
    ULONGLONG nextAsk = 0;
    DeviceStatus status;
};
std::map<std::wstring, KeyboardPoll> g_keyboards;  // removed when the receiver disappears

int QueryInfo(HidDev& d, const HidColl& c, bool keyboard, int& percent, bool& charging) {
    // One deadline includes writes, retry delays and unrelated input reports, rather than giving
    // each retry a fresh second. This is an active query, so a silent collection gets at most 1 s.
    const ULONGLONG deadline = GetTickCount64() + 1000;
    for (int attempt = 0; attempt < 3; attempt++) {
        int left = (int)((LONGLONG)deadline - (LONGLONG)GetTickCount64());
        if (attempt) {
            if (left <= 150) break;
            Sleep(150);
            left = (int)((LONGLONG)deadline - (LONGLONG)GetTickCount64());
        }
        if (left <= 0) break;
        std::vector<BYTE> req(c.outLen(), 0);
        req[0] = kReport;
        req[1] = 0x01;
        req[2] = 0x01;
        req[4] = (BYTE)(kCmdDeviceInfo & 0xFF);
        req[5] = (BYTE)(kCmdDeviceInfo >> 8);
        req[kData] = Xor(req, 2, 7);
        if (!d.Write(req, std::min(left, 250))) continue;
        const ULONGLONG end = std::min(deadline, GetTickCount64() + 250);
        std::vector<BYTE> r;
        while ((left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64())) > 0) {
            if (d.Read(r, left) < 0) break;
            int state = ParseInfo(r, keyboard, percent, charging);
            if (state >= 0) return state;
        }
    }
    return -1;
}

}  // namespace

void PollMchoseModern(PollContext& ctx) {
    std::set<std::wstring> present;
    for (const HidColl& c : ctx.colls)
        if (c.vid == kVid && c.pid == 0x1015) present.insert(c.path);
    for (auto it = g_keyboards.begin(); it != g_keyboards.end();)
        it = present.count(it->first) ? std::next(it) : g_keyboards.erase(it);
    for (const Target& t : kTargets) {
        if (ctx.IsClaimed(kVid, t.pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, t.pid, [&t](const HidColl& c) {
                 if (t.keyboard)
                     return c.page() == 0xFF80 && c.usage() == 1 && c.outLen() == 64 && c.inLen() == 64;
                 return (c.page() == 0xFF01 || c.page() == 0xFF0B) && c.outLen() >= 64 && c.inLen() >= 21;
             })) {
            if (t.keyboard) {
                auto it = g_keyboards.find(c->path);
                if (it != g_keyboards.end() && GetTickCount64() < it->second.nextAsk) {
                    ctx.Add(it->second.status, kVid, t.pid);
                    break;
                }
            }
            HidDev d;
            if (!d.Open(*c)) continue;
            d.Flush();
            int percent = -1;
            bool charging = false;
            int state = QueryInfo(d, *c, t.keyboard, percent, charging);
            if (state < 0 && !t.keyboard) continue;
            DeviceStatus s;
            s.key = L"mchose:" + Hex4(kVid) + L":" + Hex4(t.pid);
            s.name = HidProduct(*c);
            if (s.name.empty()) s.name = t.keyboard ? L"MCHOSE K87 V3" : L"MCHOSE mouse";
            if (t.keyboard && s.name.size() > 5 && s.name.compare(s.name.size() - 5, 5, L" 2.4G") == 0)
                s.name.resize(s.name.size() - 5);
            s.kind = t.keyboard ? DeviceKind::Keyboard : DeviceKind::Mouse;
            s.via = t.via;
            s.graceSec = 1800;
            if (state == 1) {
                s.percent = Clamp100(percent);
                s.charging = charging;
            } else {
                s.asleep = true;
            }
            if (t.keyboard) g_keyboards[c->path] = { GetTickCount64() + 3 * 60 * 1000, s };
            ctx.Add(s, kVid, t.pid);
            break;
        }
    }
}

}

void PollMchoseModern(PollContext& ctx) { wb_driver_mchose_modern::PollMchoseModern(ctx); }
namespace wb_driver_misc_mice {
// Smaller mouse brands, each with a community-documented battery protocol:
//  * Motospeed X6 (Realtek VID 0BDA, receivers FFE0 / FFF2, interface 4): output report B5 [06],
//    reply report B4, byte 20 = level (+128 while charging). The cable (FFF1) reports no battery.
//    From github.com/Jacobz888/LGSTrayBattery-MotospeedX6.
//  * Xenta receivers (Delux M900 Pro, Attack Shark X6 / R1 / X11 / X3 Max / V3 Pro / V6, Inphic; VID 1D57):
//    the receiver broadcasts input report 03 on usage page 000A every ~2 s: [03][model][40 or 41][state]
//    [percent], state 2 = full, 3 = charging (percent frozen). From github.com/AeonArcHQ/omarchy-delux-m900.
//    The collection is push-only (no output or feature report), so it is opened read-only: a read/write
//    open fails while a vendor app holds it without sharing write access.
//  * Ajazz AJ179 family (receiver 249A:5C2F, cables 248A:5C2E / 5D2E / 5E2E, interface 2): unnumbered
//    32-byte report, command 10 (device info, answered by the receiver): reply [10][00 = ok]...,
//    [4..7] model id, [12] charging, [13] percent, [14] mouse connected (receiver), [31] = sum([4..30]).
//    From github.com/Rockeyxx/aj179-linux (docs/PROTOCOL.md).
//  * AJAZZ AJ-series mice on the SONiX vendor id 3151 (AJ159 PRO receiver 402D, verified upstream;
//    AJ159 APEX 8K receiver 5007, verified by ajazz-control-center). The dongle exposes a 65-byte
//    feature collection on usage page FFFF / usage 02. A SET_FEATURE status poll (report 0x00, opcode
//    0xF7, zero payload) brings the 2.4 GHz telemetry link up; ~30 ms later GET_FEATURE report 0x05
//    carries the charge in byte 3 (0..100). A frame with a non-zero byte 1 or 2 is transient reconnect
//    garbage and is rejected. From github.com/Aiacos/ajazz-control-center (docs/protocols/mouse/aj_series.md).
namespace {

template <class Match>
bool ReadUntil(HidDev& d, std::vector<BYTE>& r, int timeoutMs, Match match) {
    ULONGLONG end = GetTickCount64() + timeoutMs;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 0) return false;
        if (match(r)) return true;
    }
    return false;
}

void PollMotospeed(PollContext& ctx) {
    for (USHORT pid : { (USHORT)0xFFE0, (USHORT)0xFFF2, (USHORT)0xFFF1 }) {
        if (ctx.IsClaimed(0x0BDA, pid)) continue;
        for (const HidColl* c : ctx.Find(0x0BDA, pid, [](const HidColl& c) { return c.iface == 4 && c.outLen() >= 21; })) {
            DeviceStatus s;
            s.key = L"motospeed:" + Hex4(pid);
            s.name = L"Motospeed X6";
            s.kind = DeviceKind::Mouse;
            if (pid == 0xFFF1) {  // plugged in: charging, no battery reading over the cable
                s.via = L"USB cable";
                s.charging = true;
                ctx.Add(s, 0x0BDA, pid);
                break;
            }
            HidDev d;
            if (!d.Open(*c)) continue;
            d.Flush();
            if (!d.Write({ 0xB5, 0x06 })) continue;
            std::vector<BYTE> r;
            if (!ReadUntil(d, r, 1500, [](const std::vector<BYTE>& r) { return r.size() > 20 && r[0] == 0xB4; })) continue;
            int raw = r[20];
            s.via = L"2.4 GHz receiver";
            s.charging = raw >= 128;
            s.percent = Clamp100(s.charging ? raw - 128 : raw);
            ctx.Add(s, 0x0BDA, pid);
            break;
        }
    }
}

// Xenta receiver (1D57:FA60, FA61, FA65; cable FA55 / 2155), used by Delux, Attack Shark and Inphic mice.
// The receiver broadcasts [03][model][event][state][level] every ~2 s; nothing is sent to it. The handle
// stays open between polls so these frames queue up. A reading counts once two frames agree, because
// every link-up starts with a false 100% (github.com/qu1ncky/BatteryTray, XentaReceiverProvider).
struct XentaModel { BYTE code; const wchar_t* name; bool tens; };
const XentaModel kXentaModels[] = {
    // Inphic HUB model table (hub.inphic.cn); 73 was read from an IN10 MAX on 1D57:FA65.
    { 0x73, L"Inphic IN10R MAX", false },
    { 0x74, L"Inphic IN10 MAX", false },
    { 0x3D, L"Delux M900 Pro", false },
    { 0x30, L"Delux M900 Pro", false },
    { 0x50, L"Delux M900 Pro", false },
    { 0x55, L"Attack Shark X11", false },
    { 0x4E, L"Attack Shark X3 Max", false },
    { 0xE5, L"Attack Shark V3 Pro", false },
    { 0x24, L"Attack Shark V6", false },
    { 0x10, L"Attack Shark X6 / R1", true },  // level reported in tens (0-10)
};

bool IsXentaFrame(const std::vector<BYTE>& r) {
    return r.size() >= 5 && r[0] == 0x03 && (r[2] == 0x40 || r[2] == 0x41);
}

struct XentaListener {
    std::unique_ptr<HidDev> dev;
    std::vector<BYTE> last, prev;  // the two most recent battery frames
    ULONGLONG lastFrameAt = 0;
};
std::map<std::wstring, XentaListener> g_xenta;  // by collection path

void PollXenta(PollContext& ctx) {
    const USHORT pids[] = { 0xFA60, 0xFA61, 0xFA65, 0xFA55, 0x2155 };
    std::set<std::wstring> present;
    for (USHORT pid : pids) {
        if (ctx.IsClaimed(0x1D57, pid)) continue;
        for (const HidColl* c : ctx.Find(0x1D57, pid, [](const HidColl& c) { return c.page() == 0x000A && c.inLen() >= 5; })) {
            present.insert(c->path);
            XentaListener& l = g_xenta[c->path];
            bool fresh = !l.dev;
            if (fresh) {
                l.dev = std::make_unique<HidDev>();
                if (!l.dev->OpenReadOnly(*c)) { g_xenta.erase(c->path); continue; }
            }
            // Take everything queued since the last poll; a new listener waits for two frames.
            ULONGLONG until = GetTickCount64() + (fresh ? 5000 : 0);
            std::vector<BYTE> r;
            int got = 0;
            while (true) {
                int wait = fresh && got < 2 ? (int)std::max<LONGLONG>(1, (LONGLONG)until - (LONGLONG)GetTickCount64()) : 30;
                if (fresh && got < 2 && (LONGLONG)until - (LONGLONG)GetTickCount64() <= 0) break;
                if (l.dev->Read(r, wait) < 5) break;
                if (!IsXentaFrame(r)) continue;
                l.prev = l.last;
                l.last.assign(r.begin(), r.begin() + 5);
                l.lastFrameAt = GetTickCount64();
                got++;
            }
            if (l.last.empty() || GetTickCount64() - l.lastFrameAt > 15000) continue;  // mouse off: no frames
            bool cable = pid == 0xFA55 || pid == 0x2155;
            BYTE code = l.last[1], st = l.last[3], level = l.last[4];
            const XentaModel* model = nullptr;
            for (const auto& m : kXentaModels)
                if (m.code == code) model = &m;
            DeviceStatus s;
            s.key = L"xenta:" + Hex4(pid);
            s.name = model ? model->name : L"Wireless mouse";
            s.kind = DeviceKind::Mouse;
            s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
            bool agreed = l.prev == l.last;
            if (pid == 0xFA65 && (code == 0x30 || code == 0x50)) {  // Delux reports its level while charging
                if (st == 0x02) s.percent = 100;
                else if (st == 0x03) {
                    s.charging = true;
                    if (level <= 100) s.percent = level;
                } else if (agreed && st == 0x01 && level >= 1 && level <= 100) {
                    s.percent = level;
                }
            } else if (cable || st == 0x02 || st == 0x03) {
                s.charging = true;  // the level is frozen on external power
                if (st == 0x02 && !cable) s.percent = 100;
            } else if (agreed) {
                if (model && model->tens) {
                    if (level >= 1 && level <= 10) s.percent = level * 10;
                } else if (st == 0x01 && level >= 1 && level <= 100) {
                    s.percent = level;
                }
            }
            if (s.percent < 0 && !s.charging) s.asleep = true;  // shown with the last known value
            ctx.Add(s, 0x1D57, pid);
            break;
        }
    }
    for (auto it = g_xenta.begin(); it != g_xenta.end();)  // forget receivers that were unplugged
        it = present.count(it->first) ? std::next(it) : g_xenta.erase(it);
}

void PollAjazz(PollContext& ctx) {
    const std::pair<USHORT, USHORT> products[] = { { 0x249A, 0x5C2F }, { 0x248A, 0x5C2E }, { 0x248A, 0x5D2E }, { 0x248A, 0x5E2E } };
    for (auto [vid, pid] : products) {
        if (ctx.IsClaimed(vid, pid)) continue;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.iface == 2 && c.outLen() == 33 && c.inLen() == 33; })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            d.Flush();
            if (!d.Write({ 0x00, 0x10 })) continue;  // report id 0, command 10, zero payload (checksum 0)
            std::vector<BYTE> r;
            bool ok = ReadUntil(d, r, 1500, [](const std::vector<BYTE>& r) {
                if (r.size() < 32 || r[0] != 0x10 || r[1] != 0x00) return false;
                unsigned sum = 0;
                for (int i = 4; i <= 30; i++) sum += r[i];
                return (BYTE)sum == r[31];
            });
            if (!ok || r[13] > 100) continue;
            DeviceStatus s;
            s.key = L"ajazz:" + Hex4(vid) + L":" + Hex4(pid);
            std::string model;
            for (int i = 4; i < 8 && r[i] >= 0x20 && r[i] < 0x7F; i++) model += (char)r[i];
            s.name = model.size() == 4 && model[0] == 'M' ? L"Ajazz AJ" + std::wstring(model.begin() + 1, model.end()) : L"Ajazz mouse";
            s.kind = DeviceKind::Mouse;
            bool receiver = vid == 0x249A;
            s.via = receiver ? L"2.4 GHz receiver" : L"USB cable";
            s.connected = !receiver || r[14] != 0;
            s.charging = r[12] != 0;
            if (s.connected) s.percent = r[13];
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

void PollAjazzAjSeries(PollContext& ctx) {
    for (USHORT pid : { (USHORT)0x402D, (USHORT)0x5007 }) {
        if (ctx.IsClaimed(0x3151, pid)) continue;
        for (const HidColl* c : ctx.Find(0x3151, pid, [](const HidColl& c) {
                 return c.page() == 0xFFFF && c.usage() == 0x02 && c.featLen() >= 65;
             })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            std::vector<BYTE> req(c->featLen(), 0);
            req[0] = 0x00;  // unnumbered report
            req[1] = 0xF7;  // status poll: brings the 2.4 GHz telemetry link up
            if (!d.SetFeature(req)) continue;
            Sleep(30);
            std::vector<BYTE> r;
            if (!d.GetFeature(0x05, r) || r.size() < 4) continue;
            if (r[0] != 0x05 || r[1] != 0 || r[2] != 0) continue;  // malformed / reconnect frame
            int pct = r[3];
            if (pct > 100) continue;
            DeviceStatus s;
            s.key = L"ajazzaj:" + Hex4(0x3151) + L":" + Hex4(pid);
            s.name = HidProduct(*c);
            if (s.name.empty()) s.name = L"AJAZZ mouse";
            s.kind = DeviceKind::Mouse;
            s.via = L"2.4 GHz receiver";
            s.connected = pct > 0;  // 0 means the link is up but the mouse is off
            if (s.connected) s.percent = pct;
            ctx.Add(s, 0x3151, pid);
            break;
        }
    }
}

}  // namespace

void PollMiscMice(PollContext& ctx) {
    PollMotospeed(ctx);
    PollXenta(ctx);
    PollAjazz(ctx);
    PollAjazzAjSeries(ctx);
}

}

void PollMiscMice(PollContext& ctx) { wb_driver_misc_mice::PollMiscMice(ctx); }

namespace wb_driver_msi {
// MSI wireless mice on their 2.4 GHz dongles.
//
// Protocol (taken from MSI Center's own Gaming Gear module, checked against real mice):
//   Output report (report ID 0, 64-byte payload zero-padded): [00][0F 20 00 ...]
//   Input reply (after stripping the leading 0 report ID):
//     [0] = 0F, [1] = 20, [14] = battery: bit 7 = charging, bits 0..6 = percent (0..100)
// A level of 0 while not charging is treated as "asleep" and the last known value is kept: MSI
// Center's log shows readings of 0 between normal ones, most likely while the mouse is off or asleep.
// MSI Center also sends 41 80 ("remote control") first. That switches the mouse into a software
// control mode, so it is deliberately not sent here: the 0F 20 query is enough.
// Unknown requests are answered with FF AA 00 00 plus an echo of the request, so a dongle that is
// never asked (see the passive GM41 below) stays silent.
//
// VERSA 300 ELITE WIRELESS (0DB0:9E22) and CLUTCH GM31 Lightweight Wireless (receiver 0DB0:0D3C,
// USB cable 0DB0:0D3B) both answer the query above, on collection FF0D:00B0 and FF00:0001.
// The GM31 was confirmed against MSI Center on a live mouse: the pushed frame
// 0F 20 00 00 23 05 7A 91 00 00 00 00 05 80 56 xx reads 0x56 = 86 %, the level MSI Center shows.
//
// CLUTCH GM41 Lightweight Wireless (receiver 0DB0:0D4C, USB cable 0DB0:0D4B) has no query: its
// collection (FF10:0006) only pushes A0 <raw> 07 <flags> 94 00 94 while the mouse moves, and <raw>
// falls as the battery drains. Sources: DARL0K/Wireless-Device-Battery-Monitor PR #1 and
// liamgibsonn/mouse-protocol PR #1 (USBPcap captures of MSI Center), so both GM41 ids and the
// cable ids here are untested.
namespace {

constexpr USHORT kMsiVid = 0x0DB0;

// Query: the dongle is asked for a status frame. Passive: the dongle pushes and cannot be asked.
enum class Mode { Query, Passive };

struct Model {
    USHORT pid;
    bool cable;
    Mode mode;
    USHORT page;
    USHORT usage;
    const wchar_t* name;
};

const Model kModels[] = {
    { 0x9E22, false, Mode::Query,   0xFF0D, 0x00B0, L"MSI VERSA 300 ELITE WIRELESS" },
    { 0x0D3C, false, Mode::Query,   0xFF00, 0x0001, L"MSI Clutch GM31 Lightweight Wireless" },
    { 0x0D3B, true,  Mode::Query,   0xFF00, 0x0001, L"MSI Clutch GM31 Lightweight Wireless" },
    { 0x0D4C, false, Mode::Passive, 0xFF10, 0x0006, L"MSI Clutch GM41 Lightweight Wireless" },
    { 0x0D4B, true,  Mode::Passive, 0xFF10, 0x0006, L"MSI Clutch GM41 Lightweight Wireless" },
};

bool Matches(const Model& m, const HidColl& c) {
    if (c.iface != 1 || c.page() != m.page || c.usage() != m.usage) return false;
    return m.mode == Mode::Query ? c.outLen() >= 65 && c.inLen() >= 65 : c.inLen() >= 7;
}

bool QueryStatus(const HidColl& c, int& percent, bool& charging) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    if (!d.Write({ 0x00, 0x0F, 0x20 })) return false;

    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 1000;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        int n = d.Read(r, left);
        if (n < 0) break;
        if (n >= 15 && r[0] == 0x0F && r[1] == 0x20) {
            int level = r[14] & 0x7F;
            if (level > 100) return false;
            percent = level;
            charging = (r[14] & 0x80) != 0;
            return true;
        }
    }
    return false;
}

// Linear fit through (0x4D, 57 %), (0x48, 54 %) and (0x45, 53 %): about 0.51 % per step.
int Gm41RawToPercent(int raw) { return Clamp100((raw * 51 + 1760 + 50) / 100); }

std::wstring Hex2(BYTE v) {
    const wchar_t* d = L"0123456789ABCDEF";
    return { d[v >> 4], d[v & 15] };
}

// The GM41 only reports while the mouse is moving, so this listens longer than a query waits;
// a sleeping mouse simply yields nothing and the app keeps its last value.
bool ListenGm41(const HidColl& c, int& percent, int& raw, BYTE& flags) {
    HidDev d;
    if (!d.OpenReadOnly(c)) return false;

    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 2000;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        int n = d.Read(r, left);
        if (n < 0) break;
        if (n < 7 || r[0] != 0xA0 || r[2] != 0x07) continue;
        raw = r[1];
        flags = r[3];
        percent = Gm41RawToPercent(raw);
        return true;
    }
    return false;
}

}  // namespace

void PollMsi(PollContext& ctx) {
    if (ctx.IsClaimed(kMsiVid, 0x9E22)) return;
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kMsiVid, m.pid)) continue;
        for (const HidColl* c : ctx.Find(kMsiVid, m.pid, [&m](const HidColl& c) { return Matches(m, c); })) {
            int pct = -1;
            int raw = 0;
            BYTE flags = 0;
            bool chg = false;
            bool ok = m.mode == Mode::Query ? QueryStatus(*c, pct, chg) : ListenGm41(*c, pct, raw, flags);
            if (!ok) break;

            DeviceStatus s;
            s.key = L"msi:" + Hex4(m.pid);
            s.name = m.name;  // the Clutch dongles report a serial number as their product string
            s.kind = DeviceKind::Mouse;
            s.via = m.cable ? L"USB cable" : L"2.4 GHz receiver";
            if (m.mode == Mode::Passive) s.detailText = L"approximate (raw 0x" + Hex2((BYTE)raw) + L", flags 0x" + Hex2(flags) + L")";
            if (pct == 0 && !chg) {
                s.asleep = true;
            } else {
                s.percent = pct;
                s.charging = chg;
            }
            s.graceSec = 1800;
            ctx.Add(s, kMsiVid, m.pid);
            break;
        }
    }
}

}

void PollMsi(PollContext& ctx) { wb_driver_msi::PollMsi(ctx); }
namespace wb_driver_ninjutso {
// Ninjutso mice:
//  * Sora V2, on its 1K receiver (1915:AE1C) or the USB cable (1915:AE11): NinjaForce bundled
//    out/renderer/assets/createSoraV2-DL6g5ddD.js, createSorav2.readState(). Feature report 05,
//    command 15: [9] battery (0 means 100%), [10] charging, [12] online. Indices include the report ID,
//    as in WebHID's returned DataView. On the cable the same query answers with [12] = 0 (tested on a
//    Sora V2), so the mouse counts as connected there, as with the Sprime PM1.
//    The "0 means 100%" rule is applied on the cable too; a full battery on the cable has not been captured yet.
//  * Ten / Ten Air (093A:EB01 receiver, 093A:E020 USB cable): NinjaForce (https://www.ninjaforce.co/)
//    and https://github.com/DrStopor/ninjutso-battery-tray. Feature report 06 (16 bytes) on
//    FF01:0001; commands 0x13 (online), 0x12 (battery %), 0x11 (charging), value at byte 8.
//    Verified on Ninjutso Ten Air (093A:EB01 and 093A:E020).
namespace {

bool QueryTen(HidDev& d, BYTE cmd, BYTE& value) {
    std::vector<BYTE> q(16, 0), r;
    q[0] = 0x06;
    q[1] = cmd;
    q[4] = 1;
    if (!d.SetFeature(q)) return false;
    for (int attempt = 0; attempt < 3; ++attempt) {
        Sleep(30);
        if (d.GetFeature(0x06, r) && r.size() == 16 && r[0] == 0x06 && r[1] == cmd) {
            value = r[8];
            return true;
        }
    }
    return false;
}

void PollTen(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x093A)) {
        if (pid != 0xEB01 && pid != 0xE020) continue;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF01 && c.usage() == 1 && c.featLen() == 16;
             })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            BYTE online = 0, percent = 0, charging = 0;
            if (!QueryTen(d, 0x13, online) || online != 1 || !QueryTen(d, 0x12, percent) || percent > 100 ||
                !QueryTen(d, 0x11, charging))
                continue;
            DeviceStatus s;
            s.key = L"ninjutso-ten:" + Hex4(pid);
            s.name = HidProduct(*c);
            if (s.name.empty()) s.name = L"Ninjutso Ten / Ten Air";
            s.kind = DeviceKind::Mouse;
            s.via = pid == 0xE020 ? L"USB cable" : L"2.4 GHz receiver";
            s.percent = percent;
            s.charging = charging != 0;
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}  // namespace

void PollNinjutso(PollContext& ctx) {
    constexpr USHORT vid = 0x1915, receiverPid = 0xAE1C, cablePid = 0xAE11;
    for (USHORT pid : { receiverPid, cablePid }) {
        if (ctx.IsClaimed(vid, pid)) continue;
        bool cable = pid == cablePid;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFFA0 && c.usage() == 1 && c.featLen() >= 32;
             })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            std::vector<BYTE> req(32, 0);
            req[0] = 0x05;
            req[1] = 0x15;
            req[4] = 1;
            DeviceStatus s;
            bool answered = false;
            for (int attempt = 0; attempt < 4; ++attempt) {
                if (attempt) Sleep(90);
                if (!d.SetFeature(req)) break;
                Sleep(15);
                std::vector<BYTE> r;
                if (!d.GetFeature(0x05, r) || r.size() < 13 || r[0] != 0x05 || r[1] != 0x15 || r[9] > 100) continue;
                answered = true;
                s.connected = cable || r[12] != 0;
                s.percent = s.connected ? (r[9] == 0 ? 100 : r[9]) : -1;
                s.charging = s.connected && r[10] != 0;
                if (s.connected) break;
            }
            if (!answered) continue;
            s.key = L"ninjutso:" + Hex4(vid) + L":" + Hex4(pid);
            s.name = L"Ninjutso Sora V2";
            s.kind = DeviceKind::Mouse;
            s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
            ctx.Add(s, vid, pid);
            break;
        }
    }
    PollTen(ctx);
}

}

void PollNinjutso(PollContext& ctx) { wb_driver_ninjutso::PollNinjutso(ctx); }
namespace wb_driver_nzxt {
// NZXT Lift Elite Wireless on its dongle (1E71:2101) or USB cable (1E71:2129), without NZXT CAM.
// Ported from HaloBattery (github.com/HeyOkay/HaloBattery, providers/nzxt.py, MIT; confirmed there on the
// reporter's mouse), which decoded it from USB captures of NZXT CAM 4.76.5.
// 64-byte reports on the vendor collection FFCA:0001:
//   request: 4E 02 81 00 B0 00 ...   (02 = the mouse, 81 = read a property, B0 = telemetry)
//   reply:   4E 02 97 ...; bytes 17-18 = percent (little endian), bytes 8-9 = cell mV (big endian),
//            byte 22 bit 0 = on the charging cable. The dongle first acks with 4E E5, which is skipped.
// Only this read is sent. While NZXT CAM is open the dongle stops answering; the last value is kept.
namespace {

constexpr USHORT kVid = 0x1E71, kReceiver = 0x2101, kCable = 0x2129;

bool ReadTelemetry(const HidColl& c, int& percent, int& mv, bool& charging) {
    HidDev d;
    if (!d.Open(c)) return false;
    d.Flush();
    std::vector<BYTE> req(65, 0);  // report id 0, then the 64-byte packet
    req[1] = 0x4E;
    req[2] = 0x02;
    req[3] = 0x81;
    req[5] = 0xB0;
    if (!d.Write(req)) return false;
    std::vector<BYTE> r;
    for (int i = 0; i < 10; i++) {
        int n = d.Read(r, 150);
        if (n < 0) return false;
        if (n < 27 || r[0] != 0x4E) continue;
        if (r[1] == 0xE5) continue;  // the dongle's acknowledgement
        if (r[1] != 0x02 || r[2] != 0x97) continue;
        int level = r[17] | (r[18] << 8);
        if (level > 100) return false;  // not a percentage: refuse rather than show it
        percent = level;
        mv = (r[8] << 8) | r[9];
        charging = (r[22] & 0x01) != 0;
        return true;
    }
    return false;
}

}  // namespace

void PollNzxt(PollContext& ctx) {
    // One entry for the mouse on both ids; the cable reading wins when both answer (it is the one charging).
    bool any = false, have = false;
    int percent = -1, mv = 0;
    bool charging = false;
    for (USHORT pid : { kCable, kReceiver }) {
        if (ctx.IsClaimed(kVid, pid)) continue;
        auto colls = ctx.Find(kVid, pid, [](const HidColl& c) { return c.page() == 0xFFCA && c.usage() == 0x0001; });
        if (colls.empty()) continue;
        any = true;
        ctx.Claim(kVid, pid);
        for (const HidColl* c : colls) {
            int p = -1, v = 0;
            bool ch = false;
            if (have || !ReadTelemetry(*c, p, v, ch)) continue;
            percent = p;
            mv = v;
            charging = ch;
            have = true;
        }
    }
    if (!any) return;
    DeviceStatus s;
    s.key = L"nzxt:2101";
    s.name = L"NZXT Lift Elite";
    s.kind = DeviceKind::Mouse;
    s.via = L"2.4 GHz receiver";
    s.graceSec = 1800;
    if (have) {
        s.percent = percent;
        s.charging = charging;
        if (mv >= 3000 && mv <= 4500) s.millivolts = mv;
    } else {
        s.asleep = true;  // mouse off, or NZXT CAM holds the dongle: keep the last value
    }
    ctx.out.push_back(s);
}

}

void PollNzxt(PollContext& ctx) { wb_driver_nzxt::PollNzxt(ctx); }
namespace wb_driver_phonelink {
// Microsoft Phone Link stores a localized Adaptive Card in its package folder.
// The card contains the phone connection status and battery information.
// Supports English and Japanese connection messages and battery tooltips.
// Reads localized fields from the JSON card layout; rejects malformed or ambiguous readings.
// The file is an undocumented Phone Link implementation detail.
// Read-only, no device or network access.

namespace PhoneLink {
struct Reading { std::wstring name; int percent=-1; bool charging=false; };
namespace Detail {
struct Value {
    wchar_t kind=0;
    std::wstring text;
    std::map<std::wstring, Value> object;
    std::vector<Value> array;
};
// The card is an undocumented format. Bound nesting and size, reject malformed JSON,
// and inspect fields on their owning object rather than searching notification text.
class Parser {
    const std::wstring& input;
    size_t at=0, nodes=0;
    void Space(){while(at<input.size() && (input[at]==L' '||input[at]==L'\r'||input[at]==L'\n'||input[at]==L'\t'))++at;}
    bool Take(wchar_t c){Space();if(at==input.size()||input[at]!=c)return false;++at;return true;}
    bool String(std::wstring& out){
        if(!Take(L'"'))return false;
        while(at<input.size()){
            wchar_t c=input[at++];if(c==L'"')return true;if(c<32)return false;
            if(c!=L'\\'){out+=c;continue;}
            if(at==input.size())return false;
            c=input[at++];
            if(c==L'u'){
                unsigned v=0;
                for(int i=0;i<4;++i){if(at==input.size())return false;wchar_t h=input[at++];unsigned n;
                    if(h>=L'0'&&h<=L'9')n=h-L'0';else if(h>=L'a'&&h<=L'f')n=h-L'a'+10;else if(h>=L'A'&&h<=L'F')n=h-L'A'+10;else return false;v=v*16+n;}
                out+=static_cast<wchar_t>(v);
            }else if(c==L'"'||c==L'\\'||c==L'/')out+=c;
            else if(c==L'n')out+=L'\n';else if(c==L't')out+=L'\t';else if(c==L'r')out+=L'\r';else if(c==L'b')out+=L'\b';else if(c==L'f')out+=L'\f';else return false;
        }return false;
    }
    bool Read(Value& v,int depth){
        Space();if(depth>64||++nodes>20000||at==input.size())return false;
        wchar_t c=input[at];v.kind=c;
        if(c==L'"')return String(v.text);
        if(c==L'{'){
            ++at;if(Take(L'}'))return true;
            do{std::wstring key;Value child;if(!String(key)||!Take(L':')||!Read(child,depth+1)||v.object.count(key))return false;v.object.emplace(std::move(key),std::move(child));if(Take(L'}'))return true;}while(Take(L','));return false;
        }
        if(c==L'['){++at;if(Take(L']'))return true;do{Value child;if(!Read(child,depth+1))return false;v.array.push_back(std::move(child));if(Take(L']'))return true;}while(Take(L','));return false;}
        for(const wchar_t* literal:{L"true",L"false",L"null"}){size_t n=wcslen(literal);if(input.compare(at,n,literal)==0){at+=n;return true;}}
        if(c==L'-')++at;
        if(at==input.size())return false;
        if(input[at]==L'0')++at;else {if(input[at]<L'1'||input[at]>L'9')return false;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;}
        if(at<input.size()&&input[at]==L'.'){++at;size_t start=at;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;if(at==start)return false;}
        if(at<input.size()&&(input[at]==L'e'||input[at]==L'E')){++at;if(at<input.size()&&(input[at]==L'+'||input[at]==L'-'))++at;size_t start=at;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;if(at==start)return false;}
        return true;
    }
public:
    explicit Parser(const std::wstring& s):input(s){}
    bool Parse(Value& v){if(input.size()>4*1024*1024||!Read(v,0))return false;Space();return at==input.size();}
};
inline const std::wstring* Field(const Value& v,const wchar_t* key){auto i=v.object.find(key);return i!=v.object.end()&&i->second.kind==L'"'?&i->second.text:nullptr;}
} // namespace Detail

inline bool ConnectedPhoneName(const std::wstring& speak,
                               std::wstring& name) {
    const wchar_t* suffixes[] = {
        L" - Connected to your PC",
        L" - PC \u306B\u63A5\u7D9A\u3057\u307E\u3057\u305F"
    };

    for (const wchar_t* suffix : suffixes) {
        const std::wstring ending(suffix);

        if (speak.size() > ending.size() &&
            speak.compare(speak.size() - ending.size(),
                          ending.size(), ending) == 0) {
            name = speak.substr(0, speak.size() - ending.size());
            return !name.empty();
        }
    }

    return false;
}

inline int BatteryFromTooltip(const std::wstring& tooltip,
                              bool& charging) {
    const std::wstring english = L"Battery ";
    const std::wstring japanese = L"\u30D0\u30C3\u30C6\u30EA\u30FC \u6B8B\u91CF ";

    bool en = tooltip.compare(0, english.size(), english) == 0;
    bool ja = tooltip.compare(0, japanese.size(), japanese) == 0;

    if (!en && !ja) return -1;

    size_t start = en ? english.size() : japanese.size();
    size_t end = start;

    while (end < tooltip.size() &&
           tooltip[end] >= L'0' && tooltip[end] <= L'9' &&
           end - start < 3) {
        ++end;
    }

    if (end == start || end >= tooltip.size() ||
        tooltip[end] != L'%') return -1;

    int level = std::stoi(tooltip.substr(start, end - start));
    if (level > 100) return -1;

    const std::wstring remainder = tooltip.substr(end + 1);

    if (en) {
        if (remainder != L" remaining" &&
            remainder != L" remaining, charging")
            return -1;

        charging = remainder == L" remaining, charging";
    } else {
        if (!remainder.empty()) return -1;
        charging = false;  // Charging state not established for Japanese cards.
    }

    return level;
}

namespace Detail {
inline void Battery(const Value& value,bool japanese,int& percent,bool& charging,bool& ambiguous){
    if(value.kind==L'['){for(const auto& child:value.array)Battery(child,japanese,percent,charging,ambiguous);return;}
    if(value.kind!=L'{')return;
    const auto* type=Field(value,L"type");
    if(type && *type!=L"AdaptiveCard" && *type!=L"Container" && *type!=L"ColumnSet" && *type!=L"Column" && *type!=L"TextBlock" && *type!=L"Image")return;
    if(const auto* tooltip=Field(value,L"tooltip")){
        bool ja=tooltip->rfind(L"\u30D0\u30C3\u30C6\u30EA\u30FC \u6B8B\u91CF ",0)==0, charge=false;
        int n=ja==japanese?BatteryFromTooltip(*tooltip,charge):-1;
        const auto* text=Field(value,L"text");
        if(n>=0 && (!text || *text==std::to_wstring(n)+L"%")){
            if(percent>=0 && (percent!=n||charging!=charge))ambiguous=true;
            percent=n;charging=charge;
        }
    }
    // Only traverse card layout fields, never actions, arbitrary metadata or notification payloads.
    for(const wchar_t* field:{L"body",L"items",L"columns"}){auto i=value.object.find(field);if(i!=value.object.end())Battery(i->second,japanese,percent,charging,ambiguous);}
}
} // namespace Detail
inline bool Parse(const std::wstring& json,Reading& out){
    Detail::Value root;
    if(!Detail::Parser(json).Parse(root)||root.kind!=L'{')return false;
    const auto* speak=Detail::Field(root,L"speak");Reading parsed;
    if(!speak||!ConnectedPhoneName(*speak,parsed.name))return false;
    const std::wstring suffix=L" - PC \u306B\u63A5\u7D9A\u3057\u307E\u3057\u305F";
    bool ja=speak->size()>=suffix.size() && speak->compare(speak->size()-suffix.size(),suffix.size(),suffix)==0;
    bool ambiguous=false;
    Detail::Battery(root,ja,parsed.percent,parsed.charging,ambiguous);
    if(ambiguous||parsed.percent<0)return false;
    out=std::move(parsed);return true;
}
} // namespace PhoneLink

namespace {

// A card that has not been rewritten for this long is shown dimmed, as a last known value.
constexpr ULONGLONG kStaleSec = 3 * 60 * 60;

bool ReadUtf8File(const std::wstring& path, std::wstring& out, FILETIME& written) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(h, &size) && size.QuadPart > 0 && size.QuadPart < 4 * 1024 * 1024 && GetFileTime(h, nullptr, nullptr, &written);
    std::string bytes;
    if (ok) {
        bytes.resize((size_t)size.QuadPart);
        DWORD got = 0;
        ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr) && got == bytes.size();
    }
    CloseHandle(h);
    if (!ok) return false;
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(), nullptr, 0);
    if (n <= 0) return false;
    out.resize(n);
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(), out.data(), n);
    return true;
}
}  // namespace

void PollPhoneLink(PollContext& ctx) {
    wchar_t base[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH);
    if (!n || n >= MAX_PATH) return;
    std::wstring path = std::wstring(base) + L"\\Packages\\Microsoft.YourPhone_8wekyb3d8bbwe\\LocalState\\StartMenu\\StartMenuCompanion.json";

    std::wstring json;
    FILETIME written{};

    if (!ReadUtf8File(path, json, written))
        return;

    PhoneLink::Reading reading;
    if (!PhoneLink::Parse(json, reading))
        return;

    FILETIME now{};
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a{}, b{};
    a.LowPart = now.dwLowDateTime, a.HighPart = now.dwHighDateTime;
    b.LowPart = written.dwLowDateTime, b.HighPart = written.dwHighDateTime;
    ULONGLONG ageSec = a.QuadPart > b.QuadPart ? (a.QuadPart - b.QuadPart) / 10000000ull : 0;

    DeviceStatus s;
    s.key = L"phonelink";
    s.name = reading.name;
    s.via = L"Phone Link";
    s.kind = DeviceKind::Phone;
    s.percent = reading.percent;
    s.charging = reading.charging;
    s.asleep = ageSec > kStaleSec;

    ctx.out.push_back(s);
}

}

void PollPhoneLink(PollContext& ctx) { wb_driver_phonelink::PollPhoneLink(ctx); }
namespace wb_driver_playstation {
// Sony DualShock 4 / DualSense / DualSense Edge (VID 054C), USB or Bluetooth.
// The battery is part of the regular input report:
//   DualShock 4: USB report 01 byte 30, Bluetooth report 11 byte 32: bits 0-3 level, bit 4 cable
//   DualSense:   USB report 01 byte 53, Bluetooth report 31 byte 54: bits 0-3 level (0-10), bits 4-7 state
// Over Bluetooth the battery is only in the full report, which another app (e.g. Steam) has to enable:
// enabling it here would break DirectInput for apps that don't use SDL until the controller is power
// cycled (SDL_HINT_JOYSTICK_ENHANCED_REPORTS).
// Layout reference: Linux drivers/hid/hid-playstation.c.
namespace {

bool IsDs4(USHORT pid) { return pid == 0x05C4 || pid == 0x09CC || pid == 0x0BA0; }
bool IsDualSense(USHORT pid) { return pid == 0x0CE6 || pid == 0x0DF2; }

}  // namespace

void PollPlayStation(PollContext& ctx) {
    for (const auto& c : ctx.colls) {
        if (c.vid != 0x054C || !(IsDs4(c.pid) || IsDualSense(c.pid))) continue;
        if (c.page() != 0x01 || c.usage() != 0x05) continue;  // the gamepad collection
        HidDev d;
        if (!d.Open(c)) continue;
        bool bt = c.inLen() > 64;
        std::vector<BYTE> r;
        int pct = -1;
        bool charging = false, found = false;
        ULONGLONG end = GetTickCount64() + 500;
        while (!found && (LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
            if (d.Read(r, 100) < 10) continue;
            if (IsDs4(c.pid)) {
                size_t at = r[0] == 0x01 ? 30 : r[0] == 0x11 ? 32 : 0;
                if (!at || r.size() <= at || (r[0] == 0x01 && bt)) continue;
                int level = r[at] & 0x0F;
                bool cable = (r[at] & 0x10) != 0;
                if (c.pid == 0x0BA0 && r[at] == 0) break;  // wireless adapter without a controller
                pct = cable ? (level >= 11 ? 100 : Clamp100(level * 10)) : Clamp100(level * 10 + 5);
                charging = cable && level < 11;
                found = true;
            } else {
                size_t at = r[0] == 0x01 ? 53 : r[0] == 0x31 ? 54 : 0;
                if (!at || r.size() <= at || (r[0] == 0x01 && bt)) continue;
                int level = r[at] & 0x0F, state = r[at] >> 4;
                pct = Clamp100(level * 10 + 5);
                charging = state == 0x1;
                if (state == 0x2) { pct = 100; charging = true; }
                found = true;
            }
        }
        if (!found) continue;
        DeviceStatus s;
        UINT h = 2166136261u;
        for (wchar_t ch : c.path) { h ^= ch; h *= 16777619u; }
        s.key = L"ps:" + Hex4(c.pid) + L":" + std::to_wstring(h);
        s.name = IsDs4(c.pid) ? L"DualShock 4" : c.pid == 0x0DF2 ? L"DualSense Edge" : L"DualSense";
        s.kind = DeviceKind::Gamepad;
        s.via = bt ? L"Bluetooth" : (c.pid == 0x0BA0 ? L"Wireless adapter" : L"USB cable");
        s.percent = pct;
        s.charging = charging;
        ctx.out.push_back(s);
        ctx.Claim(c.vid, c.pid);
    }
}

}

void PollPlayStation(PollContext& ctx) { wb_driver_playstation::PollPlayStation(ctx); }

namespace wb_driver_pulsar {
// Pulsar X2 V3 / X2 V3 eS and other Sonix-based Pulsar mice on vendor id 3710:
//   X2 V3:    USB cable 3402 ("X2 v3"), 1K dongle 5402 (confirmed by a user, BatteryHub issue #54)
//   X2 V3 eS: USB cable 3406, 8K dongle 5403
// Both dongles are shared with other Pulsar mice (Xlite V4, X3), so on a dongle the mouse gets a
// generic name. These do not speak the 17-byte report 08 protocol of the Kysona driver.
// Protocol from github.com/darthsoup/PulsarBattery (Sonix64Protocol.cs / Sonix64Backend.cs) and
// github.com/bluesceada/pulsar-mouse-linux (drivers/xlite_v4_wireless.py, verified on 5402 with an X3):
//   Feature report 0, 64-byte packet: [00][category][register][sub][00 ...][sum lo][sum hi],
//   sum = 16-bit sum of packet bytes 0..61 (little endian in bytes 62..63).
//   Battery read = 08 81 01. SET_FEATURE, then GET_FEATURE every ~50 ms until the reply echoes the
//   command: [status][08][81][01][..][..][percent] (packet byte 6, FF = no reading).
//   Status 01 = answered, 02 = still pending. 05 means "mouse not linked / asleep" in
//   pulsar-mouse-linux but "pending" in PulsarBattery, so we keep polling on 05 and only treat it as
//   "mouse off" if it is still the last answer when we give up.
//   The vendor collection (FFFF:0001) is on interfaces 2 and 3; interface 2 rejects SET_FEATURE, so
//   higher interfaces are tried first.
//   No reliable charging register: on the cable the mouse is charging; 0% from a dongle means the
//   mouse is off or out of range.
namespace {

const USHORT kVid = 0x3710;

struct Product { USHORT pid; bool cable; const wchar_t* name; };
const Product kProducts[] = {
    { 0x3402, true, L"Pulsar X2 V3" },
    { 0x3406, true, L"Pulsar X2 V3 eS" },
    { 0x5402, false, nullptr },  // 1K dongle
    { 0x5403, false, nullptr },  // 8K dongle
};

enum class Reply { Battery, Offline, None };

// Reads the battery percent into `pct`. Offline = the dongle answered that the mouse is not linked.
Reply ReadBattery(HidDev& d, int& pct) {
    std::vector<BYTE> req(65, 0);  // [0] = report id 0, then the 64-byte packet
    BYTE* p = req.data() + 1;
    p[1] = 0x08;
    p[2] = 0x81;
    p[3] = 0x01;
    unsigned sum = 0;
    for (int i = 0; i < 62; i++) sum += p[i];
    p[62] = (BYTE)(sum & 0xFF);
    p[63] = (BYTE)(sum >> 8);
    bool sawOffline = false;
    for (int attempt = 0; attempt < 3; ++attempt) {
        if (!d.SetFeature(req)) return Reply::None;
        for (int read = 0; read < 6; ++read) {
            Sleep(50);
            std::vector<BYTE> r;
            if (!d.GetFeature(0x00, r) || r.size() < 65) return Reply::None;
            const BYTE* w = r.data() + 1;
            if (w[1] != 0x08 || w[2] != 0x81 || w[3] != 0x01) continue;  // not our reply (yet)
            if (w[0] == 0x01) {
                if (w[6] > 100) return Reply::None;  // FF = no reading
                pct = w[6];
                return Reply::Battery;
            }
            sawOffline = w[0] == 0x05;  // 02 (or 05) = pending: keep polling
        }
    }
    return sawOffline ? Reply::Offline : Reply::None;
}

}  // namespace

void PollPulsar(PollContext& ctx) {
    for (const Product& prod : kProducts) {
        if (ctx.IsClaimed(kVid, prod.pid)) continue;
        auto colls = ctx.Find(kVid, prod.pid, [](const HidColl& c) { return c.featLen() >= 65; });
        std::sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) { return a->iface > b->iface; });
        for (const HidColl* c : colls) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            int pct = -1;
            Reply rep = ReadBattery(d, pct);
            if (rep == Reply::None) continue;
            DeviceStatus s;
            s.key = L"pulsar:" + Hex4(prod.pid);
            s.kind = DeviceKind::Mouse;
            if (prod.cable) {
                s.name = prod.name;
                s.via = L"USB cable";
                s.charging = true;
            } else {
                std::wstring n = HidProduct(*c);
                bool generic = n.empty() || n.find(L"Dongle") != std::wstring::npos ||
                               n.find(L"Receiver") != std::wstring::npos;
                s.name = generic ? L"Pulsar wireless mouse" : n;  // the dongles are shared by several models
                s.via = L"2.4 GHz receiver";
                s.connected = rep == Reply::Battery && pct > 0;
            }
            if (rep == Reply::Battery && (pct > 0 || prod.cable)) s.percent = pct;
            ctx.Add(s, kVid, prod.pid);
            break;
        }
    }
}

}

void PollPulsar(PollContext& ctx) { wb_driver_pulsar::PollPulsar(ctx); }
namespace wb_driver_rapoo {
// Rapoo VT gaming mice. Battery is pushed periodically; no commands or handshake are needed.
// Sources:
//  * Official Rapoo Hub (https://hub.rapoo.cn/), VT_nrf54L input-report decoder
//    and hardware manifests: report 07 on FF00:0002, state at byte 7, percent at byte 8,
//    optional mouse PID at 13..14. Connection is the low nibble of byte 1 (0 RF / 1 BT / 2 USB).
//  * https://github.com/PyFlat/Device-Battery-Info (RapooProtocol.cs): older 7-byte BB B0
//    reports on FF00:0002, state at byte 5, percent at byte 6; charging can report 7F (unknown).
//  * VT1 Pro Max (receiver 1233, cable 4433): USBPcap captures from the owner in BatteryHub issue #136
//    show the same passive 7-byte report on interface 1 / FF00:0002, sent without any request:
//    receiver `BB B0 71 84 03 01 64` every ~3.2 s (Rapoo app: 100%), cable while charging
//    `BB B0 71 84 03 02 5F` (state 2 = charging, 95%).
//  * VT3 Pro Max (receiver 24AE:1244, BatteryHub issue #2258): same passive seven-byte BB B0
//    status reports on FF00:0002. Source: https://github.com/zbndev/rapoo_battery
// Legacy BB semantics are restricted to known product IDs; other Telink firmware differs.
//  * VT3 Max Gen 2 receiver 1417 on another firmware revision (BatteryHub issue #1396) exposes a 13-byte
//    FF00:0002 collection. Rapoo Hub's VT_nrf54L decoder reads report 07 at any length and only
//    takes the optional fields when present, so state/percent at bytes 7/8 still fit; the mouse
//    PID at 13..14 does not, so identity falls back to the receiver PID (same rapoo:4617 key).

namespace {

// Selected names from Rapoo's manifests. Support is determined by the collection and valid
// status frame, so other models on the same protocol work without extending this table.
struct Model { USHORT cable, receiver; const wchar_t* name; };
const Model kModels[] = {
    { 0x4610, 0x1410, L"Rapoo VT0" },
    { 0x4620, 0x1420, L"Rapoo VT0 Max" },
    { 0x4615, 0x1415, L"Rapoo VT1" },
    { 0x4644, 0x1444, L"Rapoo VT1 Max" },
    { 0x4622, 0x1422, L"Rapoo VT2" },
    { 0x4623, 0x1423, L"Rapoo VT2 Max" },
    { 0x4612, 0x1412, L"Rapoo VT3" },
    { 0x4617, 0x1417, L"Rapoo VT3 Max (Gen 2)" },
    { 0x4B06, 0x1B06, L"Rapoo VT3 Max V2" },
    { 0x4611, 0x1411, L"Rapoo VT3S" },
    { 0x4621, 0x1421, L"Rapoo VT3S Max" },
    { 0x4613, 0x1413, L"Rapoo VT7" },
    { 0x4616, 0x1416, L"Rapoo VT7 Max" },
    { 0x4614, 0x1414, L"Rapoo VT9 (Gen 2)" },
    { 0x4618, 0x1418, L"Rapoo VT9 Max" },
    { 0x4415, 0x1215, L"Rapoo VT3 Pro" },
    { 0x4433, 0x1233, L"Rapoo VT1 Pro Max" },
};

const Model* FindModel(USHORT pid) {
    for (const auto& model : kModels)
        if (pid == model.cable || pid == model.receiver) return &model;
    return nullptr;
}

// Mice confirmed to send the 7-byte BB B0 status report.
bool IsLegacyVT3Pro(USHORT pid) {
    return pid == 0x1215 || pid == 0x4415 ||  // VT3 Pro
           pid == 0x1233 || pid == 0x4433 ||  // VT1 Pro Max (issue #136 captures)
           pid == 0x1244;                      // VT3 Pro Max receiver (issue #2258)
}

// Keep open handles between polls (like PollXenta) so Windows buffers passive status reports
// arriving during the 60 s refresh interval instead of missing mice whose push interval is >5 s.
std::map<std::wstring, std::unique_ptr<HidDev>> g_rapoo;  // by collection path

bool DrainQueued(HidDev& d, Rapoo::Battery& battery, Rapoo::Profile profile) {
    bool got = false;
    for (int i = 0; i < 64; i++) {
        std::vector<BYTE> r;
        if (d.Read(r, 30) < 0) break;
        if (Rapoo::DecodeBattery(r.data(), r.size(), battery, profile)) got = true;
    }
    return got;
}

bool WaitBattery(HidDev& d, Rapoo::Battery& battery, Rapoo::Profile profile, ULONGLONG end) {
    do {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        std::vector<BYTE> r;
        if (left <= 0 || d.Read(r, left) < 0) return false;
        if (Rapoo::DecodeBattery(r.data(), r.size(), battery, profile)) return true;
    } while (GetTickCount64() < end);
    return false;
}

bool ReadBattery(std::unique_ptr<HidDev>& slot, const HidColl& c, Rapoo::Battery& battery,
                 Rapoo::Profile profile, bool fresh) {
    if (!fresh && DrainQueued(*slot, battery, profile)) return true;
    // A passive report can take several seconds (VT1 Pro Max ~4 s; VT9 Gen 2 ~5-10 s on first open),
    // so wait up to 5 s when no queued packet was buffered yet. One deadline covers unrelated packets.
    ULONGLONG start = GetTickCount64();
    ULONGLONG end = start + 5000;
    if (WaitBattery(*slot, battery, profile, end)) return true;
    // If a persistent handle failed immediately (e.g. receiver reset between polls), reopen once.
    if (!fresh && GetTickCount64() - start < 50) {
        slot->Close();
        if (slot->Open(c)) return WaitBattery(*slot, battery, profile, end);
    }
    return false;
}

}  // namespace

void PollRapoo(PollContext& ctx) {
    std::set<std::wstring> present;
    for (auto [vid, pid] : ctx.Products(0x24AE)) {
        for (const HidColl* c : ctx.Find(vid, pid, [pid](const HidColl& c) {
                 // The other vendor collections are for configuration/firmware, not battery.
                 return c.page() == 0xFF00 && c.usage() == 0x0002 &&
                        (c.inLen() == 16 || c.inLen() == 19 || (c.inLen() == 13 && pid == 0x1417) ||
                         (c.inLen() == 7 && IsLegacyVT3Pro(pid)));
             })) {
            present.insert(c->path);
            auto& slot = g_rapoo[c->path];
            bool fresh = !slot;
            if (fresh) {
                slot = std::make_unique<HidDev>();
                if (!slot->Open(*c)) {
                    g_rapoo.erase(c->path);
                    continue;
                }
            }
            Rapoo::Battery battery;
            auto profile = c->inLen() == 7 ? Rapoo::Profile::LegacyVT3Pro : Rapoo::Profile::Nordic54L;
            if (!ReadBattery(slot, *c, battery, profile, fresh)) continue;

            const Model* model = FindModel(battery.mousePid ? battery.mousePid : pid);
            USHORT identity = model ? model->cable : (battery.mousePid ? battery.mousePid : pid);
            DeviceStatus s;
            s.key = L"rapoo:" + Hex4(identity);
            s.name = model ? model->name : HidProduct(*c);
            // No confirmed cable PID for the 1244 receiver; keep its own identity/key.
            if (pid == 0x1244) s.name = L"Rapoo VT3 Pro Max";
            if (s.name.empty() || s.name == L"Rapoo Gaming Device") s.name = L"Rapoo wireless mouse";
            s.kind = DeviceKind::Mouse;
            if (battery.connection == Rapoo::Connection::Cable ||
                (battery.connection == Rapoo::Connection::Unknown && model && pid == model->cable))
                s.via = L"USB cable";
            else if (battery.connection == Rapoo::Connection::Bluetooth)
                s.via = L"Bluetooth";
            else
                s.via = L"2.4 GHz receiver";
            s.percent = battery.percent;
            s.charging = battery.charging;
            s.graceSec = 1800;  // existing UI keeps the last reading, dimmed, while the mouse sleeps

            // A charging mouse can report through both cable and receiver. Keep a single entry,
            // preferring the cable's state but keeping any available percentage. Claim both
            // products before generic fallbacks.
            auto existing = std::find_if(ctx.out.begin(), ctx.out.end(), [&](const DeviceStatus& other) {
                return other.key == s.key;
            });
            if (existing == ctx.out.end()) ctx.Add(s, vid, pid);
            else {
                if (s.via == L"USB cable") {
                    int receiverPercent = existing->percent;
                    *existing = s;
                    if (existing->percent < 0) existing->percent = receiverPercent;
                } else if (existing->percent < 0 && s.percent >= 0) {
                    existing->percent = s.percent;
                }
                ctx.Claim(vid, pid);
            }
            break;
        }
    }
    for (auto it = g_rapoo.begin(); it != g_rapoo.end();)
        it = present.count(it->first) ? std::next(it) : g_rapoo.erase(it);
}

}

void PollRapoo(PollContext& ctx) { wb_driver_rapoo::PollRapoo(ctx); }

namespace wb_driver_razer {
// Razer wireless mice, keyboards and dongles (VID 1532).
// 90-byte feature report (report id 0):
//   [0] status  [1] transaction id  [2..3] remaining packets  [4] protocol type  [5] data size
//   [6] command class  [7] command id  [8..87] arguments  [88] crc (xor of 2..87)  [89] reserved
// Battery: class 07 id 80 -> args[1] = 0..255. Charging: class 07 id 84 -> args[1] = 1.
// Reply status: 01 busy, 02 ok, 03 failed, 04 timeout, 05 not supported.
// Use documented keyboard transaction ids; probe the known ids for other models.
// Reference: github.com/openrazer/openrazer (protocol documentation in its driver sources).
namespace {

BYTE KeyboardBatteryTid(USHORT pid) {
    // OpenRazer razerkbd_driver.c: razer_attr_read_charge_level / charge_status.
    switch (pid) {
        case 0x025C: case 0x0271: case 0x0290: case 0x0296:
        case 0x02BA: case 0x02C9: case 0x02D2: case 0x02D5: return 0x9F;  // wireless keyboards
        case 0x0258: case 0x0292: case 0x0298: case 0x02B9:
        case 0x02CD: case 0x02D4: case 0x02D7: return 0x1F;  // wired keyboards / Joro
        case 0x025A: return 0x3F;  // BlackWidow V3 Pro wired
        default: return 0;
    }
}

enum class Rz { Ok, Offline, Unsupported, NoReply };

Rz Command(HidDev& d, BYTE tid, BYTE cls, BYTE id, BYTE size, std::vector<BYTE>& args) {
    std::vector<BYTE> rep(91, 0);  // [0] = report id 0, then the 90-byte report
    BYTE* r = rep.data() + 1;
    r[1] = tid;
    r[5] = size;
    r[6] = cls;
    r[7] = id;
    BYTE crc = 0;
    for (int i = 2; i < 88; i++) crc ^= r[i];
    r[88] = crc;
    std::vector<BYTE> resp;
    bool send = true;
    Rz res = Rz::NoReply;
    for (int attempt = 0; attempt < 8; attempt++) {
        if (send && !d.SetFeature(rep)) return Rz::NoReply;
        // Wireless keyboards need about 5 ms (OpenRazer); a long wait lets Synapse replace the reply.
        Sleep(KeyboardBatteryTid(d.coll().pid) ? 5 : (attempt == 0 ? 15 : 40));
        send = true;
        res = Rz::NoReply;
        if (!d.GetFeature(0, resp) || resp.size() < 91) continue;
        const BYTE* a = resp.data() + 1;
        BYTE replyCrc = 0;
        for (int i = 2; i < 88; i++) replyCrc ^= a[i];
        // Synapse shares the feature-report mailbox. A different reply needs a fresh request.
        if (a[1] != tid || a[2] || a[3] || a[4] || a[5] != size || a[6] != cls || a[7] != id ||
            a[88] != replyCrc) continue;
        if (a[0] == 0x01) { send = false; continue; }  // busy: wait for this request to finish
        if (a[0] == 0x02) { args.assign(a + 8, a + 88); return Rz::Ok; }
        if (a[0] == 0x04) { res = Rz::Offline; continue; }  // retry a transient receiver timeout
        if (a[0] == 0x05 || a[0] == 0x03) return Rz::Unsupported;
    }
    return res;
}

std::wstring CleanName(std::wstring n) {
    for (const wchar_t* suffix : { L" Dongle", L" (Wired)", L" (Wireless)", L" Wireless Receiver", L" Receiver", L" HyperSpeed Dongle" }) {
        size_t l = wcslen(suffix);
        if (n.size() > l && n.compare(n.size() - l, l, suffix) == 0) n = n.substr(0, n.size() - l);
    }
    if (n.empty()) n = L"Razer device";
    return n;
}

// Exact keyboard/keypad/laptop and mouse/receiver IDs from OpenRazer (no PID-range assumptions).
// https://github.com/openrazer/openrazer/blob/cd5c4243bfafb08df7e2d2963367c3c150626eac/driver/razerkbd_driver.h
// https://github.com/openrazer/openrazer/blob/cd5c4243bfafb08df7e2d2963367c3c150626eac/driver/razermouse_driver.h
constexpr USHORT kKeyboardPids[] = {
    0x010D, 0x010E, 0x010F, 0x0111, 0x0113, 0x0118, 0x011A, 0x011B, 0x011C, 0x0201,
    0x0202, 0x0203, 0x0204, 0x0205, 0x0207, 0x0208, 0x0209, 0x020F, 0x0210, 0x0211,
    0x0214, 0x0216, 0x0217, 0x021A, 0x021E, 0x021F, 0x0220, 0x0221, 0x0224, 0x0225,
    0x0226, 0x0227, 0x0228, 0x022A, 0x022B, 0x022C, 0x022D, 0x022F, 0x0232, 0x0233,
    0x0234, 0x0235, 0x0237, 0x0239, 0x023A, 0x023B, 0x023F, 0x0240, 0x0241, 0x0243,
    0x0244, 0x0245, 0x0246, 0x024A, 0x024B, 0x024C, 0x024D, 0x024E, 0x0252, 0x0253,
    0x0255, 0x0256, 0x0257, 0x0258, 0x0259, 0x025A, 0x025C, 0x025D, 0x025E, 0x0266,
    0x0268, 0x0269, 0x026A, 0x026B, 0x026C, 0x026D, 0x026E, 0x026F, 0x0270, 0x0271,
    0x0276, 0x0279, 0x027A, 0x0282, 0x0287, 0x028A, 0x028B, 0x028C, 0x028D, 0x028F,
    0x0290, 0x0292, 0x0293, 0x0294, 0x0295, 0x0296, 0x0298, 0x029D, 0x029E, 0x029F,
    0x02A0, 0x02A1, 0x02A2, 0x02A3, 0x02A5, 0x02A6, 0x02A7, 0x02B0, 0x02B6, 0x02B8,
    0x02B9, 0x02BA, 0x02C5, 0x02C6, 0x02C7, 0x02C9 /* BlackWidow V4 Low-profile HyperSpeed */, 0x02CD,
    0x02CF, 0x02D2, 0x02D4, 0x02D5, 0x02D7, 0x02E0, 0x02E1, 0x02EA, 0x0A24,
};

constexpr USHORT kMousePids[] = {
    0x0013, 0x0015, 0x0016, 0x001F, 0x0020, 0x0024, 0x0025, 0x0029, 0x002E, 0x002F,
    0x0032, 0x0034, 0x0036, 0x0037, 0x0038, 0x0039, 0x003E, 0x003F, 0x0040, 0x0041,
    0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0048, 0x004C, 0x004F, 0x0050, 0x0053,
    0x0054, 0x0059, 0x005A, 0x005B, 0x005C, 0x005E, 0x005F, 0x0060, 0x0062, 0x0064,
    0x0065, 0x0067, 0x006A, 0x006B, 0x006C, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072,
    0x0073, 0x0077, 0x0078, 0x007A, 0x007B, 0x007C, 0x007D, 0x0080, 0x0083, 0x0084,
    0x0085, 0x0086, 0x0088, 0x008A, 0x008C, 0x008D, 0x008F, 0x0090, 0x0091, 0x0094,
    0x0095, 0x0096, 0x0098, 0x0099, 0x009A, 0x009C, 0x009E, 0x009F, 0x00A1, 0x00A3,
    0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00AA, 0x00AB, 0x00AF, 0x00B0, 0x00B2, 0x00B3,
    0x00B4, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2,
    0x00C3, 0x00C4, 0x00C5, 0x00C7, 0x00C8, 0x00CB, 0x00CC, 0x00CD, 0x00D0, 0x00D1,
    0x00D3, 0x00D4, 0x00D6, 0x00D7, 0x00DA, 0x00DB, 0x00DE, 0x00DF, 0x00E0, 0x00E5,
    0x00E6,
};

DeviceKind Kind(const std::vector<HidColl>& colls, USHORT vid, USHORT pid) {
    // Both keyboards and mice can expose keyboard AND mouse collections for macros/buttons.
    if (vid == 0x1532) {
        for (USHORT p : kKeyboardPids) if (p == pid) return DeviceKind::Keyboard;
        for (USHORT p : kMousePids) if (p == pid) return DeviceKind::Mouse;
    }
    return KindFromCollections(colls, vid, pid);
}

}  // namespace

void PollRazer(PollContext& ctx) {
    const auto firstRazer = ctx.out.size();
    for (auto [vid, pid] : ctx.Products(0x1532)) {
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.featLen() == 91; })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            Rz res = Rz::NoReply;
            BYTE tid = 0;
            std::vector<BYTE> args;
            bool sawOffline = false;
            BYTE keyboardTid = KeyboardBatteryTid(pid);
            std::vector<BYTE> tids = keyboardTid ? std::vector<BYTE>{ keyboardTid } :
                                                  std::vector<BYTE>{ 0x1F, 0x3F, 0xFF, 0x08, 0x9F };
            for (BYTE t : tids) {
                Rz r = Command(d, t, 0x07, 0x80, 0x02, args);
                if (r == Rz::Ok) { res = r; tid = t; break; }
                sawOffline |= r == Rz::Offline;  // may also mean "wrong transaction id": keep trying
            }
            if (res != Rz::Ok && sawOffline) res = Rz::Offline;
            if (res != Rz::Ok && res != Rz::Offline) continue;

            DeviceStatus s;
            s.key = L"razer:" + Hex4(pid);
            s.name = CleanName(HidProduct(*c));
            s.kind = Kind(ctx.colls, vid, pid);
            if (s.kind == DeviceKind::Other) s.kind = GuessKind(s.name);
            s.via = L"Razer receiver";
            s.graceSec = 120;  // retain the last reading briefly when Synapse prevents a valid reply
            if (res == Rz::Offline) {
                s.connected = false;
            } else {
                s.percent = Clamp100((args[1] * 100 + 127) / 255);
                std::vector<BYTE> ch;
                if (Command(d, tid, 0x07, 0x84, 0x02, ch) == Rz::Ok) s.charging = ch[1] == 1;
            }
            // Cable + receiver can report the same device; keep a live reading over an offline receiver.
            auto existing = std::find_if(ctx.out.begin() + firstRazer, ctx.out.end(),
                                         [&](const DeviceStatus& o) { return o.name == s.name; });
            if (existing == ctx.out.end()) {
                ctx.Add(s, vid, pid);
            } else {
                ctx.Claim(vid, pid);
                if (!existing->connected && s.connected) {
                    existing->connected = true;
                    existing->percent = s.percent;
                }
                existing->charging |= s.charging;
            }
            break;
        }
    }
}

}

void PollRazer(PollContext& ctx) { wb_driver_razer::PollRazer(ctx); }

namespace wb_driver_razer_headset {
// Razer BlackShark headsets with their own vendor protocol (not the 90-byte Razer feature report).
//
// BlackShark V3 / V3 Pro (1532:0576/0577 V3 Pro, 0579/057A V3, 0A4E/0A55 V3 Pro for Xbox), interface 5;
// V3 X further down.
// 64-byte output report 02: [02][00][60][00 00][00][size][00 00][dir][class][sub][argc][args]..
// [62] = XOR of bytes 0..61. GET battery = class 21 -> reply args [level %][charging]; class 2A ->
// args [cable].
// The headset drops its 2.4 GHz link if the battery is queried every few seconds, so it is asked at
// most once every 5 minutes and the last reading is reused in between.
// Protocol from OpenRazer PR #2794 (docs/v3_pro_protocol.md) and github.com/ReedME/omarchy-blackshark-v3.
namespace {

constexpr ULONGLONG kMinIntervalMs = 5 * 60 * 1000;

std::vector<BYTE> Packet(BYTE dir, BYTE cls) {
    std::vector<BYTE> b(64, 0);
    b[0] = 0x02;
    b[2] = 0x60;
    b[6] = 0x04;  // data size: dir, class, sub, argc
    b[9] = dir;
    b[10] = cls;
    BYTE crc = 0;
    for (int i = 0; i < 62; i++) crc ^= b[i];
    b[62] = crc;
    return b;
}

bool Get(HidDev& d, BYTE cls, std::vector<BYTE>& reply) {
    if (!d.Write(Packet(0x80, cls))) return false;
    ULONGLONG end = GetTickCount64() + 600;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(reply, left) < 15) return false;
        if (reply[0] == 0x02 && reply[2] == 0x60 && reply[10] == cls) return true;
    }
    return false;
}

struct Cached { ULONGLONG at = 0; DeviceStatus status; bool valid = false; };
std::map<USHORT, Cached> g_cache;

const wchar_t* Name(USHORT pid) {
    if (pid == 0x0A4E || pid == 0x0A55) return L"Razer BlackShark V3 Pro for Xbox";
    return (pid == 0x0576 || pid == 0x0577) ? L"Razer BlackShark V3 Pro" : L"Razer BlackShark V3";
}
// The lower id of each pair is the USB cable, the higher one the wireless receiver (0579 cable / 057A receiver).
bool Cable(USHORT pid) { return pid == 0x0576 || pid == 0x0579 || pid == 0x0A4E; }

// BlackShark V3 X (1532:057D HyperSpeed dongle, 057C cable): a different protocol on interface 0.
// 64-byte feature report 07: [07][status][txid][00 00 00][argc][class][cmd][args].. [62] = XOR of 3..60.
// Battery = class 07 cmd 80 -> args[1] is the level %; cmd 84 -> args[1] is charging.
// Status 02 = success, 01 = busy; anything else (e.g. headset off) is a failure.
// Protocol from OpenRazer PR #2899, verified upstream on a 057D dongle.
bool V3xGet(HidDev& d, BYTE cmd, BYTE& value) {
    static BYTE txid = 0x40;
    for (int attempt = 0; attempt < 4; attempt++) {
        std::vector<BYTE> req(64, 0);
        req[0] = 0x07;
        req[2] = ++txid;
        req[6] = 0x02;
        req[7] = 0x07;
        req[8] = cmd;
        BYTE crc = 0;
        for (int i = 3; i <= 60; i++) crc ^= req[i];
        req[62] = crc;
        if (!d.SetFeature(req)) return false;
        Sleep(60);
        std::vector<BYTE> r;
        if (!d.GetFeature(0x07, r) || r.size() < 63) continue;
        if (r[2] != txid || r[7] != 0x07 || r[8] != cmd || r[6] < 2) continue;
        if (r[1] != 0x02 && r[1] != 0x01) return false;
        value = r[10];
        return true;
    }
    return false;
}

void PollBlackSharkV3X(PollContext& ctx) {
    for (USHORT pid : { (USHORT)0x057D, (USHORT)0x057C }) {
        if (ctx.IsClaimed(0x1532, pid)) continue;
        auto colls = ctx.Find(0x1532, pid, [](const HidColl& c) { return c.iface == 0 && c.featLen() >= 64; });
        if (colls.empty()) continue;
        ctx.Claim(0x1532, pid);  // also when it doesn't answer, so that the generic Razer driver leaves it alone
        Cached& cache = g_cache[pid];
        if (cache.at && GetTickCount64() - cache.at < kMinIntervalMs) {
            if (cache.valid) ctx.Add(cache.status, 0x1532, pid);
            continue;
        }
        cache = { GetTickCount64() };  // a failed attempt counts too
        HidDev d;
        if (!d.OpenFeatureOnly(*colls[0])) continue;
        BYTE level = 0, charging = 0;
        if (!V3xGet(d, 0x80, level) || level > 100) continue;
        V3xGet(d, 0x84, charging);
        DeviceStatus s;
        s.key = L"blackshark:" + Hex4(pid);
        s.name = L"Razer BlackShark V3 X";
        s.kind = DeviceKind::Headset;
        s.via = pid == 0x057D ? L"2.4 GHz receiver" : L"USB cable";
        s.percent = level;
        s.charging = charging == 1;
        cache.status = s;
        cache.valid = true;
        ctx.Add(s, 0x1532, pid);
    }
}

}  // namespace

void PollRazerHeadsets(PollContext& ctx) {
    PollBlackSharkV3X(ctx);
    for (USHORT pid : { (USHORT)0x0576, (USHORT)0x0577, (USHORT)0x0579, (USHORT)0x057A, (USHORT)0x0A4E,
                        (USHORT)0x0A55 }) {
        if (ctx.IsClaimed(0x1532, pid)) continue;
        auto colls = ctx.Find(0x1532, pid, [](const HidColl& c) { return c.iface == 5 && c.outLen() >= 64 && c.inLen() >= 15; });
        if (colls.empty()) continue;
        ctx.Claim(0x1532, pid);  // also when it doesn't answer, so that the generic Razer driver leaves it alone
        Cached& cache = g_cache[pid];
        if (cache.at && GetTickCount64() - cache.at < kMinIntervalMs) {
            if (cache.valid) ctx.Add(cache.status, 0x1532, pid);
            continue;
        }
        cache = { GetTickCount64() };  // a failed attempt counts too
        HidDev d;
        if (!d.Open(*colls[0])) continue;
        // Handshake the headset expects before reads (init, charging state on both links).
        d.Write(Packet(0x00, 0x02));
        d.Write(Packet(0x00, 0x2A));
        d.Write(Packet(0x80, 0x2A));
        Sleep(250);
        d.Flush();
        std::vector<BYTE> r;
        if (!Get(d, 0x21, r) || r[13] > 100) continue;
        std::vector<BYTE> cableState;
        const bool cableCharging = Get(d, 0x2A, cableState) && cableState[13] == 1;
        DeviceStatus s;
        s.key = L"blackshark:" + Hex4(pid);
        s.name = Name(pid);
        s.kind = DeviceKind::Headset;
        s.via = Cable(pid) ? L"USB cable" : L"2.4 GHz receiver";
        s.percent = r[13];
        s.charging = r[14] != 0 || cableCharging;
        cache.status = s;
        cache.valid = true;
        ctx.Add(s, 0x1532, pid);
    }
}

}

void PollRazerHeadsets(PollContext& ctx) { wb_driver_razer_headset::PollRazerHeadsets(ctx); }

namespace wb_driver_rog {
// ASUS ROG wireless keyboards, mice, the ROG Pelta / Delta II headsets and Cetra Open earbuds (VID 0B05),
// including the ROG Omni Receiver (0B05:1ACE).
// Query: [report id][12][sub]   sub 01 = keyboard battery, sub 07 = mouse battery.
// Reply: [report id][12][sub]...  keyboard: [6] % (Falchion: [11]), [9] == 1 charging
//                                 mouse:    [5] level (x25 on older 0-4 gauges), [10] charging
// [1..2] == FF AA means the device behind the receiver did not answer (asleep / off).
// Offsets count the report id byte. Only query commands are sent.
// Protocol and model table from github.com/SteveWang92/peripheral-battery-tray (rog-battery,
// MIT, itself based on G-Helper) and github.com/1gcat/BatteryTool.
namespace {

enum class Kind { Keyboard, Mouse };

struct Model {
    USHORT pid;
    const wchar_t* name;
    Kind kind;
    BYTE reportId;       // 0 = unnumbered
    int iface;           // USB interface (-1 = any vendor-page collection)
    const wchar_t* col;  // required collection tag in the path ("col02"), or nullptr
    int offset;          // battery byte
    int scale;           // battery multiplier
};

constexpr Kind KB = Kind::Keyboard, MS = Kind::Mouse;

const Model kModels[] = {
    // Keyboards (Omni = col02 report 02, receiver/wired = interface 1 unnumbered)
    { 0x1A85, L"ROG Azoth", KB, 2, 2, L"col02", 6, 1 },
    { 0x1A85, L"ROG Azoth", KB, 0, 1, nullptr, 6, 1 },
    { 0x1A83, L"ROG Azoth", KB, 0, 1, nullptr, 6, 1 },
    { 0x1B42, L"ROG Azoth Extreme", KB, 2, 2, L"col02", 6, 1 },
    { 0x1B3F, L"ROG Azoth Extreme", KB, 0, 1, nullptr, 6, 1 },
    { 0x1CF1, L"ROG Azoth Extreme SE", KB, 2, 2, L"col02", 6, 1 },
    { 0x1CEF, L"ROG Azoth Extreme SE", KB, 0, 1, nullptr, 6, 1 },
    { 0x1C25, L"ROG Azoth X", KB, 0, 1, nullptr, 6, 1 },
    { 0x1C24, L"ROG Azoth X", KB, 0, 1, nullptr, 6, 1 },
    { 0x193E, L"ROG Falchion", KB, 0, 1, nullptr, 11, 1 },
    { 0x193C, L"ROG Falchion", KB, 0, 1, nullptr, 11, 1 },
    { 0x1B04, L"ROG Falchion RX", KB, 0, -1, nullptr, 11, 1 },
    { 0x1B06, L"ROG Falchion RX Low Profile", KB, 2, 2, L"col02", 11, 1 },
    { 0x1AB0, L"ROG Strix Scope II 96", KB, 2, 2, L"col02", 6, 1 },
    { 0x1AAE, L"ROG Strix Scope II 96", KB, 0, 1, nullptr, 6, 1 },
    { 0x1B7A, L"ROG Strix Scope II 96 RX", KB, 2, 2, L"col02", 6, 1 },
    { 0x1B78, L"ROG Strix Scope II 96 RX", KB, 0, 1, nullptr, 6, 1 },
    { 0x1A07, L"ROG Strix Scope RX TKL", KB, 0, -1, nullptr, 6, 1 },
    // Mice (Omni = col03 report 03, receiver/wired = interface 0 unnumbered)
    { 0x18E5, L"ROG Chakram", MS, 0, 0, nullptr, 5, 25 },
    { 0x18E3, L"ROG Chakram", MS, 0, 0, nullptr, 5, 25 },
    { 0x1A1A, L"ROG Chakram X", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A18, L"ROG Chakram X", MS, 0, 0, nullptr, 5, 1 },
    { 0x18A0, L"ROG Gladius II Wireless", MS, 0, 2, nullptr, 5, 25 },
    { 0x197F, L"ROG Gladius III Wireless", MS, 0, 0, nullptr, 5, 1 },
    { 0x197D, L"ROG Gladius III Wireless", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A72, L"ROG Gladius III Aimpoint", MS, 3, 2, L"col03", 5, 1 },
    { 0x1A72, L"ROG Gladius III Aimpoint", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A70, L"ROG Gladius III Aimpoint", MS, 0, 0, nullptr, 5, 1 },
    { 0x1B0C, L"ROG Gladius III Aimpoint EVA-02", MS, 0, 0, nullptr, 5, 1 },
    { 0x1B0A, L"ROG Gladius III Aimpoint EVA-02", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A94, L"ROG Harpe Ace Aim Lab Edition", MS, 3, 2, L"col03", 5, 1 },
    { 0x1A94, L"ROG Harpe Ace Aim Lab Edition", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A92, L"ROG Harpe Ace Aim Lab Edition", MS, 0, 0, nullptr, 5, 1 },
    { 0x1B68, L"ROG Harpe Ace Extreme", MS, 3, 2, L"col03", 5, 1 },
    { 0x1B69, L"ROG Harpe Ace Extreme", MS, 3, 2, L"col03", 5, 1 },
    { 0x1B67, L"ROG Harpe Ace Extreme", MS, 0, 0, nullptr, 5, 1 },
    { 0x1B65, L"ROG Harpe Ace Mini", MS, 3, 2, L"col03", 5, 1 },
    { 0x1B63, L"ROG Harpe Ace Mini", MS, 0, 0, nullptr, 5, 1 },
    { 0x1AD0, L"ROG Harpe II Ace", MS, 3, 2, L"col03", 5, 1 },
    { 0x1C69, L"ROG Harpe II Ace", MS, 0, 0, nullptr, 5, 1 },
    { 0x1960, L"ROG Keris Wireless", MS, 0, 0, nullptr, 5, 25 },
    { 0x195E, L"ROG Keris Wireless", MS, 0, 0, nullptr, 5, 25 },
    { 0x1A59, L"ROG Keris Wireless EVA Edition", MS, 0, 0, nullptr, 5, 25 },
    { 0x1A57, L"ROG Keris Wireless EVA Edition", MS, 0, 0, nullptr, 5, 25 },
    { 0x1A68, L"ROG Keris Wireless Aimpoint", MS, 3, 2, L"col03", 5, 1 },
    { 0x1A6A, L"ROG Keris Wireless Aimpoint", MS, 3, 2, L"col03", 5, 1 },
    { 0x1A68, L"ROG Keris Wireless Aimpoint", MS, 0, 0, nullptr, 5, 1 },
    { 0x1A66, L"ROG Keris Wireless Aimpoint", MS, 0, 0, nullptr, 5, 1 },
    { 0x1B1A, L"ROG Keris II Ace", MS, 3, 2, L"col03", 5, 1 },
    { 0x1B18, L"ROG Keris II Ace", MS, 3, 2, L"col03", 5, 1 },
    { 0x1B16, L"ROG Keris II Ace", MS, 0, 0, nullptr, 5, 1 },
    { 0x1C0E, L"ROG Keris II Origin", MS, 3, 2, L"col03", 5, 1 },
    { 0x1C0C, L"ROG Keris II Origin", MS, 0, 0, nullptr, 5, 1 },
    { 0x1D4E, L"ROG Keris II Origin KJP", MS, 3, 2, L"col03", 5, 1 },
    { 0x1D4C, L"ROG Keris II Origin KJP", MS, 0, 0, nullptr, 5, 1 },
    { 0x1908, L"ROG Pugio II", MS, 0, 0, nullptr, 5, 25 },
    { 0x1906, L"ROG Pugio II", MS, 0, 0, nullptr, 5, 25 },
    { 0x1979, L"ROG Spatha X", MS, 0, 0, nullptr, 5, 1 },
    { 0x1977, L"ROG Spatha X", MS, 0, 0, nullptr, 5, 1 },
    { 0x18B4, L"ROG Strix Carry", MS, 0, 1, nullptr, 7, 25 },
    { 0x1949, L"ROG Strix Impact II Wireless", MS, 0, 0, nullptr, 5, 25 },
    { 0x1947, L"ROG Strix Impact II Wireless", MS, 0, 0, nullptr, 5, 25 },
    { 0x1AD7, L"ROG Strix Impact III Wireless", MS, 3, 2, L"col03", 5, 1 },
};

bool PathHas(const std::wstring& path, const wchar_t* tag) {
    std::wstring p = path;
    for (auto& ch : p) ch = towlower(ch);
    return p.find(tag) != std::wstring::npos;
}

enum class Reply { Ok, Asleep, None };

Reply Query(HidDev& d, BYTE rid, BYTE cmd, BYTE sub, std::vector<BYTE>& reply) {
    d.Flush();
    if (!d.Write({ rid, cmd, sub })) return Reply::None;
    ULONGLONG end = GetTickCount64() + 500;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 11) return Reply::None;
        if (rid == 0) r.insert(r.begin(), 0x00);  // restore the report id slot so offsets match
        if (r.size() < 12) continue;
        if (r[1] == 0xFF && r[2] == 0xAA) return Reply::Asleep;
        if (r[1] == cmd && r[2] == sub) { reply = r; return Reply::Ok; }
    }
    return Reply::None;
}

// Reads one model's battery through an open collection. Returns false if it did not answer.
bool ReadModel(HidDev& d, const Model& m, DeviceStatus& s) {
    std::vector<BYTE> r;
    Reply res = Query(d, m.reportId, 0x12, m.kind == KB ? 0x01 : 0x07, r);
    if (res == Reply::None) return false;
    s.key = L"rog:" + Hex4(m.pid) + (m.kind == KB ? L":kb" : L":ms");
    s.name = m.name;
    s.kind = m.kind == KB ? DeviceKind::Keyboard : DeviceKind::Mouse;
    s.via = m.reportId ? L"ROG Omni Receiver" : L"2.4 GHz receiver / USB";
    if (res == Reply::Asleep) {
        s.asleep = true;
    } else if (m.kind == KB) {
        if (r[m.offset] <= 100) s.percent = r[m.offset];
        s.charging = r[9] == 1;
    } else {
        int v = r[m.offset] * m.scale;
        if (v > 0 && v <= 100) s.percent = v;
        else s.asleep = true;  // 0 means unavailable, not empty
        s.charging = r[10] != 0;
    }
    return true;
}

void AddOnce(PollContext& ctx, std::vector<std::wstring>& seen, const DeviceStatus& s) {
    for (auto& n : seen)
        if (n == s.name) return;
    seen.push_back(s.name);
    ctx.out.push_back(s);
}

// ROG Omni Receiver (0B05:1ACE): ask it which devices are paired ([01 A0 00] on col01 lists their
// product ids in 4-byte slots from byte 5), then read each one on its own channel of the receiver.
void PollOmni(PollContext& ctx, std::vector<std::wstring>& seen) {
    if (ctx.IsClaimed(0x0B05, 0x1ACE)) return;
    auto list = ctx.Find(0x0B05, 0x1ACE, [](const HidColl& c) { return c.iface == 2 && c.outLen() >= 12 && PathHas(c.path, L"col01"); });
    if (list.empty()) return;
    HidDev d;
    std::vector<BYTE> r;
    if (!d.Open(*list[0]) || Query(d, 0x01, 0xA0, 0x00, r) != Reply::Ok) return;
    ctx.Claim(0x0B05, 0x1ACE);
    for (size_t at = 5; at + 1 < r.size(); at += 4) {
        USHORT pid = (USHORT)(r[at] | (r[at + 1] << 8));
        if (!pid) break;
        for (const Model& m : kModels) {
            if (m.pid != pid || !m.reportId) continue;  // only the Omni-channel entry of that model
            for (const HidColl* c : ctx.Find(0x0B05, 0x1ACE, [&](const HidColl& c) { return c.iface == 2 && c.outLen() >= 12 && PathHas(c.path, m.col); })) {
                HidDev dev;
                DeviceStatus s;
                if (dev.Open(*c) && ReadModel(dev, m, s)) { AddOnce(ctx, seen, s); break; }
            }
            break;
        }
    }
}

// ROG Pelta (0B05:1B84) and ROG Delta II (0B05:1AFA) headsets. Vendor collection FF00:0001 on
// interface 3, report id CC, 64 bytes each way.
//   CC 12 07 -> CC 12 07 00 00 [5] sleep min [6] battery % (FE = unknown) [7] low-battery alert %
//   CC 12 08 -> CC 12 08 00 00 [5] 1 = charging
// A sleeping or switched-off headset answers with FF AA in [5..6] or [1..2].
struct HeadsetModel {
    USHORT pid;
    const wchar_t* name;
};

const HeadsetModel kHeadsets[] = {
    { 0x1B84, L"ROG Pelta" },
    { 0x1AFA, L"ROG Delta II" },
};

Reply PeltaQuery(HidDev& d, BYTE cmd, std::vector<BYTE>& reply) {
    d.Flush();
    // The first answer after the headset reconnects can take ~750 ms.
    ULONGLONG end = GetTickCount64() + 1200;
    std::vector<BYTE> r;
    int n = d.SetOutputAndRead({ 0xCC, 0x12, cmd }, r, 1200);
    if (n < 0 && d.Write({ 0xCC, 0x12, cmd })) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left > 0) n = d.Read(r, left);
    }
    while (n >= 8) {
        if (r[0] == 0xCC && r[1] == 0xFF && r[2] == 0xAA) return Reply::Asleep;
        if (r[0] == 0xCC && r[1] == 0x12) {
            if (r[5] == 0xFF && r[6] == 0xAA) return Reply::Asleep;
            if (r[2] == cmd) { reply = r; return Reply::Ok; }
        }
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) break;
        n = d.Read(r, left);
    }
    return Reply::None;
}

void PollPelta(PollContext& ctx) {
    for (const HeadsetModel& hs : kHeadsets) {
        if (ctx.IsClaimed(0x0B05, hs.pid)) continue;
        auto colls = ctx.Find(0x0B05, hs.pid, [](const HidColl& c) {
            return c.iface == 3 && c.page() == 0xFF00 && c.usage() == 0x0001 && c.inLen() >= 8 && c.outLen() >= 8;
        });
        for (const HidColl* c : colls) {
            HidDev d;
            std::vector<BYTE> r;
            if (!d.Open(*c)) continue;
            Reply res = PeltaQuery(d, 0x07, r);
            if (res == Reply::None) continue;
            DeviceStatus s;
            s.key = L"rog:" + Hex4(hs.pid) + L":hs";
            s.name = hs.name;
            s.kind = DeviceKind::Headset;
            s.via = L"2.4 GHz receiver";
            if (res == Reply::Asleep) {
                s.asleep = true;
            } else {
                if (r[6] <= 100) s.percent = r[6];
                else s.asleep = true;  // FE = level not known yet
                std::vector<BYTE> ch;
                if (PeltaQuery(d, 0x08, ch) == Reply::Ok) s.charging = ch[5] == 1;
            }
            ctx.out.push_back(s);
            ctx.Claim(0x0B05, hs.pid);
            break;
        }
    }
}

// ROG Cetra Open Wireless (0B05:1CAC) earbuds on their SpeedNova USB-C dongle. Same vendor
// collection and report as the Pelta (FF00:0001, interface 3, report id CC, 64 bytes), but the
// battery is behind sub 12 (sub 07 answers with unrelated data on this dongle):
//   CC 12 12 -> CC 12 12 00 00 [5] left % [6] right % [7] case %
// Verified upstream on hardware against Armoury Crate. The case level is the last one the case
// reported. A level of 0 is treated as not reported. With both earbuds in the closed case the
// dongle answers FF AA, shown as asleep.
void PollCetraOpen(PollContext& ctx) {
    constexpr USHORT pid = 0x1CAC;
    if (ctx.IsClaimed(0x0B05, pid)) return;
    auto colls = ctx.Find(0x0B05, pid, [](const HidColl& c) {
        return c.iface == 3 && c.page() == 0xFF00 && c.usage() == 0x0001 && c.inLen() >= 8 && c.outLen() >= 8;
    });
    for (const HidColl* c : colls) {
        HidDev d;
        std::vector<BYTE> r;
        if (!d.Open(*c)) continue;
        Reply res = PeltaQuery(d, 0x12, r);
        if (res == Reply::None) continue;
        DeviceStatus s;
        s.key = L"rog:" + Hex4(pid) + L":hs";
        s.name = L"ROG Cetra Open Wireless";
        s.kind = DeviceKind::Headset;
        s.via = L"2.4 GHz receiver";
        auto level = [&](int at) { return r.size() > (size_t)at && r[at] > 0 && r[at] <= 100 ? (int)r[at] : -1; };
        int left = res == Reply::Ok ? level(5) : -1;
        int right = res == Reply::Ok ? level(6) : -1;
        int box = res == Reply::Ok ? level(7) : -1;
        if (left < 0 && right < 0) {
            s.asleep = true;
        } else {
            s.percent = left < 0 ? right : right < 0 ? left : (left < right ? left : right);
            auto part = [&](const wchar_t* label, int v) {
                if (v < 0) return;
                if (!s.detailText.empty()) s.detailText += L", ";
                s.detailText += label + std::to_wstring(v) + L"%";
            };
            part(L"L ", left);
            part(L"R ", right);
            part(L"case ", box);
        }
        ctx.out.push_back(s);
        ctx.Claim(0x0B05, pid);
        break;
    }
}

}  // namespace

void PollRog(PollContext& ctx) {
    std::vector<std::wstring> seen;  // one entry per device name (wired + receiver both present while charging)
    PollPelta(ctx);
    PollCetraOpen(ctx);
    PollOmni(ctx, seen);
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(0x0B05, m.pid)) continue;
        auto colls = ctx.Find(0x0B05, m.pid, [&](const HidColl& c) {
            if (c.outLen() < 12 || c.page() < 0xFF00) return false;
            if (m.iface >= 0 && c.iface != m.iface) return false;
            return !m.col || PathHas(c.path, m.col);
        });
        for (const HidColl* c : colls) {
            HidDev d;
            DeviceStatus s;
            if (!d.Open(*c) || !ReadModel(d, m, s)) continue;
            // A receiver channel (e.g. the SpeedNova 8K receiver 1AD0) answers "asleep" when nothing is
            // paired to it too, so a model is only shown there once it has really answered.
            static std::set<USHORT> awake;
            if (m.reportId && s.asleep && !awake.count(m.pid)) continue;
            if (m.reportId && !s.asleep) awake.insert(m.pid);
            AddOnce(ctx, seen, s);
            ctx.Claim(0x0B05, m.pid);
            break;
        }
    }
}

}

void PollRog(PollContext& ctx) { wb_driver_rog::PollRog(ctx); }

namespace wb_driver_rongyuan {
// Rongyuan 2.4 GHz receivers, used by Attack Shark, Ajazz and many other mice and keyboards (and
// some MSI devices). One feature report returns both the keyboard and the mouse slot:
//   request [00 F7 00 ...] on interface 2, vendor collection FFFF:0002
//   reply   [00][ok][keyboard %][mouse %][keyboard offline][mouse offline]...
// Receiver list and protocol from github.com/qu1ncky/BatteryTray (RongyuanDongleProvider, MIT),
// itself taken from the official ATTACK SHARK web driver.
// The status report carries no model name, so names come from the receiver's product string with
// the link words stripped ("AJAZZ 2.4G 8K" -> "AJAZZ wireless mouse"). 3151:402D is shared by
// several mice (Attack Shark R3, AJAZZ AJ179 Pro, ...); it is only called "Attack Shark R3" when
// the product string names no other brand.

namespace {

const UINT kReceivers[] = {
    0x0DB0197E, 0x0DB08E9A, 0x145F0376, 0x145F038D, 0x14A5C045, 0x25AA2008, 0x25AA4008, 0x25AA4018, 0x31514011,
    0x31514014, 0x31514017, 0x31514021, 0x31514027, 0x3151402D, 0x31514038, 0x31515006, 0x31515007, 0x31515008,
    0x31515020, 0x31515026, 0x3151502B, 0x31515031, 0x31515037, 0x31515038, 0x3151503D, 0x3151503F, 0x31515040,
    0x31515042, 0x31515045, 0x3151504B, 0x3151504E, 0x31515050, 0x31515052, 0x31515055, 0x3151505A, 0x342DE3E0,
    0x342DE3E1, 0x342DE3E2, 0x342DE3E6, 0x342DE3E7, 0x342DE3E8, 0x342DE3F5, 0x342DE3F6, 0x342DE3F7, 0x342DE3FB,
    0x342DE3FC, 0x342DE3FD, 0x347AA302, 0x347AA305, 0x374AA205, 0x374AA217, 0x374AA220, 0x374AA223, 0x374AA226,
    0x374AA229, 0x374AA234, 0x374AA237, 0x374AA251, 0x374AA263, 0x374AA268, 0x374AA311, 0x374AA314, 0x374AA319,
    0x374AA322, 0x374AA329, 0x374AA355, 0x379A1804, 0x379A1807, 0x38A90002, 0x38EE0000, 0x38EE0002, 0x38EE0039,
    0x391D1A04, 0x391D1A08, 0x391D1A0E, 0x391D1A13, 0x391D1A25, 0x391D1A2B, 0x391D1A34, 0x391D2017, 0x391D2A05,
};

bool IsReceiver(USHORT vid, USHORT pid) {
    UINT key = ((UINT)vid << 16) | pid;
    return std::find(std::begin(kReceivers), std::end(kReceivers), key) != std::end(kReceivers);
}

// Product string without link words: "AJAZZ 2.4G 8K" -> "AJAZZ", "2.4G Wireless Receiver" -> "".
std::wstring Brand(const std::wstring& product) {
    std::wistringstream in(product);
    std::wstring word, out;
    while (in >> word) {
        std::wstring u = word;
        for (auto& ch : u) ch = (wchar_t)std::towupper(ch);
        if (u == L"2.4G" || u == L"2.4GHZ" || u == L"8K" || u == L"4K" || u == L"2K" || u == L"1K" || u == L"RECEIVER" ||
            u == L"WIRELESS" || u == L"DONGLE" || u == L"USB")
            continue;
        out += (out.empty() ? L"" : L" ") + word;
    }
    return out;
}

bool QueryStatus(HidDev& d, std::vector<BYTE>& reply) {
    for (int attempt = 0; attempt < 3; attempt++) {
        if (attempt) Sleep(100);
        std::vector<BYTE> cmd(d.coll().featLen(), 0);
        cmd[1] = 0xF7;
        if (!d.SetFeature(cmd)) return false;
        Sleep(20);
        if (d.GetFeature(0, reply) && reply.size() >= 7 && (reply[2] | reply[3] | reply[4] | reply[5] | reply[6]) != 0) return true;
    }
    return false;
}

}  // namespace

void PollRongyuan(PollContext& ctx) {
    std::vector<std::pair<USHORT, USHORT>> products;
    for (const auto& c : ctx.colls)
        if (IsReceiver(c.vid, c.pid) && !ctx.IsClaimed(c.vid, c.pid) &&
            std::find(products.begin(), products.end(), std::make_pair(c.vid, c.pid)) == products.end())
            products.push_back({ c.vid, c.pid });

    for (auto [vid, pid] : products) {
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.iface == 2 && c.page() == 0xFFFF && c.usage() == 0x0002 && c.featLen() >= 7; })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            std::vector<BYTE> r;
            if (!QueryStatus(d, r)) continue;
            std::wstring product = HidProduct(*c);
            bool generic = product.empty() || product.find(L"Receiver") != std::wstring::npos || product.find(L"2.4G") != std::wstring::npos;
            bool any = false;
            // Slot layout: [2] keyboard %, [3] mouse %, [4] keyboard offline, [5] mouse offline.
            for (int slot = 0; slot < 2; slot++) {
                bool keyboard = slot == 0;
                if (r[keyboard ? 4 : 5] != 0) continue;  // nothing connected in this slot
                BYTE level = r[keyboard ? 2 : 3];
                DeviceStatus s;
                s.key = L"rongyuan:" + Hex4(vid) + L":" + Hex4(pid) + (keyboard ? L":kb" : L":ms");
                std::wstring brand = Brand(product), upper = brand;
                for (auto& ch : upper) ch = (wchar_t)std::towupper(ch);
                bool otherBrand = !brand.empty() && upper.find(L"ATTACK SHARK") == std::wstring::npos;
                if (vid == 0x3151 && pid == 0x402D && !keyboard && !otherBrand) s.name = L"Attack Shark R3";
                else if (vid == 0x3151 && pid == 0x5038 && keyboard) s.name = L"MonsGeek keyboard";  // receiver says "MonsGeek 2.4G Wireless Keyboard"
                else if (generic && !brand.empty()) s.name = brand + (keyboard ? L" wireless keyboard" : L" wireless mouse");
                else if (vid == 0x3151 && pid == 0x5006 && keyboard && product.find(L"Akko") != std::wstring::npos)
                    s.name = L"Akko Air 01";
                else if (generic) s.name = keyboard ? L"Wireless keyboard" : L"Wireless mouse";
                else s.name = product;
                s.kind = keyboard ? DeviceKind::Keyboard : DeviceKind::Mouse;
                s.via = L"2.4 GHz receiver";

                // Retain the last battery reading while this keyboard is sleeping.
                if (vid == 0x3151 && pid == 0x5038 && keyboard) {
                    s.graceSec = 1800;
                }

                if (level >= 1 && level <= 100) s.percent = level;
                ctx.out.push_back(s);
                any = true;
            }
            ctx.Claim(vid, pid);
            (void)any;
            break;
        }
    }
}

}

void PollRongyuan(PollContext& ctx) { wb_driver_rongyuan::PollRongyuan(ctx); }

namespace wb_driver_sony_inzone {
// Sony INZONE H9 II (054C:0FA8) and H5 (054C:0EBF) headsets and the INZONE Buds dongle (054C:0EC2):
// a Sony command layer tunnelled through HID (report 02, 64 bytes) on the vendor collection with
// usage page FF04.
//   command: [02][len][01][00 FC][8+n][96 C3][addr][event][type][tid lo][tid hi][payload][sum]
//   event:   [02][len][04][FF][..][00][96 C3][addr][event][type][tid lo][tid hi][payload][sum]
// Battery = event 04, type 01 (GET) to the headset (addr 0x41). Both directions carry a checksum
// and a transaction id.
//   Headsets: reply payload [charging][percent], percent FF = headset off.
//   Buds:     reply payload [status][percent] for the left bud, right bud, then case; percent FF =
//             bud is in the case. With both buds in the case the dongle does not respond. No
//             charging flag.
// Protocol from github.com/Sapd/HeadsetControl (sony_inzone_protocol.hpp, sony_inzone_buds.hpp).
// Buds reply layout and left-first order from github.com/kinako19/inzone-buds-battery-tray and
// github.com/takamachi66/inzone-buds-battery-tray (docs/protocol*.md).

namespace {

struct Event { BYTE id, type; USHORT tid; std::vector<BYTE> payload; };

bool ParseEvent(const std::vector<BYTE>& b, Event& e) {
    if (b.size() < 15 || b[0] != 0x02) return false;
    int len = b[1];
    if (len < 12 || len > 62 || (int)b.size() < len + 2) return false;
    if (b[2] != 0x04 || b[3] != 0xFF || b[5] != 0x00 || b[6] != 0x96 || b[7] != 0xC3) return false;
    if ((b[8] >> 4) != 0x1) return false;  // must be addressed to the PC
    unsigned sum = 0;
    for (int i = 5; i <= len; i++) sum += b[i];
    if ((BYTE)sum != b[len + 1]) return false;
    e.id = b[9];
    e.type = b[10];
    e.tid = (USHORT)(b[11] | (b[12] << 8));
    e.payload.assign(b.begin() + 13, b.begin() + len + 1);
    return true;
}

enum class Reply { Ok, NoReply, Failed };

// NoReply/Failed are identical for headset
// For buds, NoReply can imply both buds are in the case
Reply GetBattery(HidDev& d, size_t minPayload, std::vector<BYTE>& payload) {
    static USHORT tid = 1;
    if (++tid <= 1) tid = 2;  // tid 1 is used by the dongle's own notifications
    std::vector<BYTE> req(64, 0);
    req[0] = 0x02;
    req[1] = 12;     // HCI byte count (no payload)
    req[2] = 0x01;   // HCI command
    req[3] = 0x00;
    req[4] = 0xFC;   // vendor opcode 0xFC00
    req[5] = 8;
    req[6] = 0x96;
    req[7] = 0xC3;
    req[8] = 0x41;   // PC -> headset
    req[9] = 0x04;   // battery info
    req[10] = 0x01;  // GET
    req[11] = (BYTE)tid;
    req[12] = (BYTE)(tid >> 8);
    unsigned sum = 0;
    for (int i = 6; i <= 12; i++) sum += req[i];
    req[13] = (BYTE)sum;
    d.Flush();
    if (!d.Write(req)) return Reply::Failed;
    ULONGLONG end = GetTickCount64() + 1500;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 15) return Reply::NoReply;
        Event e;
        if (!ParseEvent(r, e) || e.id != 0x04 || e.tid != tid || e.payload.size() < minPayload) continue;
        if (e.type != 0x10 && e.type != 0xA0) continue;  // RET or active notification
        payload = std::move(e.payload);
        return Reply::Ok;
    }
    return Reply::NoReply;
}

int Level(BYTE value) { return value <= 100 ? value : -1; }

std::wstring Pct(int value) { return value >= 0 ? std::to_wstring(value) + L"%" : L"-"; }

}  // namespace

void PollSonyInzone(PollContext& ctx) {
    // minPayload: headsets reply [charging][percent];
    // buds reply 3x [status][percent] for both buds + case.
    struct Model { USHORT pid; const wchar_t* name; bool buds; size_t minPayload; };
    for (const Model& m : { Model{ 0x0FA8, L"Sony INZONE H9 II", false, 2 },
                            Model{ 0x0EBF, L"Sony INZONE H5", false, 2 },
                            Model{ 0x0EC2, L"Sony INZONE Buds", true, 6 } }) {
        if (ctx.IsClaimed(0x054C, m.pid)) continue;
        auto status = [&] {
            DeviceStatus s;
            s.key = L"inzone:" + Hex4(m.pid);
            s.name = m.name;
            s.kind = DeviceKind::Headset;
            s.via = L"2.4 GHz receiver";
            return s;
        };
        bool added = false, noReply = false;
        auto filter = [](const HidColl& c) { return c.page() == 0xFF04 && c.outLen() >= 64; };
        for (const HidColl* c : ctx.Find(0x054C, m.pid, filter)) {
            HidDev d;
            if (!d.Open(*c)) continue;
            std::vector<BYTE> p;
            Reply reply = GetBattery(d, m.minPayload, p);
            if (reply == Reply::NoReply) noReply = true;
            if (reply != Reply::Ok) continue;
            DeviceStatus s = status();
            if (m.buds) {
                int left = Level(p[1]);
                int right = Level(p[3]);
                int caseLevel = Level(p[5]);
                s.percent = left < 0 || right < 0 ? std::max(left, right) : std::min(left, right);
                s.connected = s.percent >= 0;
                if (s.connected) {
                    s.detailText = L"L " + Pct(left) + L", R " + Pct(right);
                    if (caseLevel >= 0) s.detailText += L", case " + Pct(caseLevel);
                }
            } else {
                bool offline = p[1] == 0xFF;
                s.connected = !offline;
                s.percent = offline ? -1 : (p[1] <= 100 ? p[1] : -1);
                s.charging = p[0] != 0;
            }
            ctx.Add(s, 0x054C, m.pid);
            added = true;
            break;
        }
        // Buds: the dongle took the request but no collection answered, which is what it does with both
        // buds in the case. Add them as not connected (hidden) rather than treating the dongle as missing.
        if (m.buds && noReply && !added) {
            DeviceStatus s = status();
            s.connected = false;
            ctx.Add(s, 0x054C, m.pid);
        }
    }
}

}

void PollSonyInzone(PollContext& ctx) { wb_driver_sony_inzone::PollSonyInzone(ctx); }

namespace wb_driver_sprime {
// Sprime PM1 wireless mouse (Nordic vendor id 1915), on its 2.4 GHz receiver or the USB cable.
// Protocol from Sprime's web driver (sprime.pro): the same feature report 05, command 15 status
// query as the Ninjutso Sora V2.
//   SET_FEATURE report 5, 31 payload bytes: [05][15][00][00][01][00 ...]
//   GET_FEATURE report 5: [05][15] .. [9] percent, [10] charging, [11] fully charged, [12] online.
//   Indices include the report ID, as in WebHID's returned DataView.
// Product ids from the web driver's WebHID filters: mouse on the cable AC11 (black) / AC12 (white) /
// AC13 (red); receivers AC1C and AD8C. The bootloader (AC0B) is never touched.
namespace {

bool IsCable(USHORT pid) { return pid == 0xAC11 || pid == 0xAC12 || pid == 0xAC13; }
bool IsReceiver(USHORT pid) { return pid == 0xAC1C || pid == 0xAD8C; }

}  // namespace

void PollSprime(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x1915)) {
        if (!IsCable(pid) && !IsReceiver(pid)) continue;
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFFA0 && c.usage() == 1 && c.featLen() >= 32;
             })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            std::vector<BYTE> req(32, 0);
            req[0] = 0x05;  // report id
            req[1] = 0x15;  // status query
            req[4] = 0x01;
            bool cable = IsCable(pid);
            DeviceStatus s;
            bool answered = false;
            for (int attempt = 0; attempt < 4; ++attempt) {
                if (attempt) Sleep(90);
                if (!d.SetFeature(req)) break;
                Sleep(15);
                std::vector<BYTE> r;
                if (!d.GetFeature(0x05, r) || r.size() < 13 || r[0] != 0x05 || r[1] != 0x15 || r[9] > 100) continue;
                answered = true;
                bool full = r[11] != 0;
                s.connected = cable || r[12] != 0;  // receiver present, mouse off or out of range
                s.percent = !s.connected ? -1 : full ? 100 : r[9];
                s.charging = s.connected && r[10] != 0 && !full;
                if (s.connected) break;
            }
            if (!answered) continue;
            s.key = L"sprime:" + Hex4(vid) + L":" + Hex4(pid);
            s.name = L"Sprime PM1";
            s.kind = DeviceKind::Mouse;
            s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollSprime(PollContext& ctx) { wb_driver_sprime::PollSprime(ctx); }
namespace wb_driver_steelseries {
// SteelSeries wireless headsets and mice (VID 1038).
// Headset protocols follow github.com/Sapd/HeadsetControl, mouse protocols github.com/flozz/rivalcfg.
namespace {

bool In(USHORT pid, std::initializer_list<USHORT> list) { return std::find(list.begin(), list.end(), pid) != list.end(); }
bool In(USHORT pid, const std::vector<USHORT>& list) { return std::find(list.begin(), list.end(), pid) != list.end(); }

bool WriteRead(HidDev& d, std::vector<BYTE> req, std::vector<BYTE>& resp, int timeoutMs = 1000) {
    d.Flush();
    return d.Write(req) && d.Read(resp, timeoutMs) > 0;
}

DeviceStatus Headset(USHORT pid, const wchar_t* name) {
    DeviceStatus s;
    s.key = L"steelseries:" + Hex4(pid);
    s.name = name;
    s.kind = DeviceKind::Headset;
    s.via = L"2.4 GHz receiver";
    return s;
}

// ---- headsets ----------------------------------------------------------------------------------

// Arctis 1 / 1 Xbox / 7X: 06 12 -> [2]==01 offline, [3] %.
bool Arctis1(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x06, 0x12 }, r) || r.size() < 4) return false;
    s = Headset(pid, L"SteelSeries Arctis 1 Wireless");
    s.connected = r[2] != 0x01;
    if (s.connected) s.percent = Clamp100(r[3]);
    return true;
}

// Arctis 7 / 7 2019 / Pro 2019: 06 18 -> [2] %.
bool Arctis7(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x06, 0x18 }, r) || r.size() < 3) return false;
    s = Headset(pid, pid == 0x1252 ? L"SteelSeries Arctis Pro" : L"SteelSeries Arctis 7");
    s.percent = Clamp100(r[2]);
    s.connected = r[2] != 0;
    return true;
}

// Arctis 9: 00 20 -> [3] raw (0x64..0x9A), [4]==01 charging.
bool Arctis9(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x00, 0x20 }, r) || r.size() < 5) return false;
    s = Headset(pid, L"SteelSeries Arctis 9");
    s.percent = MapTo100(r[3], 0x64, 0x9A);
    s.charging = r[4] == 0x01;
    return true;
}

// Arctis Pro Wireless: 41 AA -> [0]==02 offline (04 online); 40 AA -> [0] level 0..4.
// Interface 0 does not number its reports (33-byte output report = ID 0 + 32 bytes), so the command
// goes behind a 0 report ID.
bool ArctisProCmd(HidDev& d, BYTE cmd, std::vector<BYTE>& r) {
    if (WriteRead(d, { 0x00, cmd, 0xAA }, r) && !r.empty()) return true;
    return WriteRead(d, { cmd, 0xAA }, r) && !r.empty();  // in case a firmware numbers its reports
}

bool ArctisProWireless(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!ArctisProCmd(d, 0x41, r)) return false;
    s = Headset(pid, L"SteelSeries Arctis Pro Wireless");
    s.connected = r[0] != 0x02;
    if (s.connected && ArctisProCmd(d, 0x40, r) && r[0] <= 4) s.percent = MapTo100(r[0], 0, 4);
    return true;
}

// Arctis Nova Pro Wireless: 06 B0 -> [15] status (01 offline, 02 charging), [6] level 0..8.
bool NovaProWireless(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x06, 0xB0 }, r) || r.size() < 16) return false;
    s = Headset(pid, L"SteelSeries Arctis Nova Pro Wireless");
    s.connected = r[15] != 0x01;
    s.charging = r[15] == 0x02;
    if (s.connected) s.percent = MapTo100(r[6], 0, 8);
    return true;
}

// Nova family: 00 B0 -> status block; layout depends on the model.
bool NovaFamily(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x00, 0xB0 }, r) || r.size() < 7) return false;
    if (In(pid, { 0x220E, 0x2212, 0x2216, 0x2236 })) {  // Arctis 7+
        s = Headset(pid, L"SteelSeries Arctis 7+");
        s.connected = r[1] != 0x01;
        s.percent = MapTo100(r[2], 0, 4);
        s.charging = r[3] == 0x01;
    } else if (In(pid, { 0x2269, 0x226D, 0x2232, 0x2253 })) {  // Nova 3P Wireless, Nova 5
        s = Headset(pid, In(pid, { 0x2232, 0x2253 }) ? L"SteelSeries Arctis Nova 5" : L"SteelSeries Arctis Nova 3P Wireless");
        s.connected = r[1] != 0x02;
        s.percent = Clamp100(r[3]);
        s.charging = r[4] == 0x01;
    } else if (pid == 0x230A) {  // Arctis GameBuds: lowest of the buds that are out of the case
        s = Headset(pid, L"SteelSeries Arctis GameBuds");
        int lvl = -1;
        if (r[3] == 0x03) lvl = r[5];
        if (r[4] == 0x03) lvl = lvl < 0 ? r[6] : std::min<int>(lvl, r[6]);
        s.connected = lvl >= 0;
        s.percent = lvl >= 0 ? Clamp100(lvl) : -1;
    } else {  // Arctis Nova 7 / 7X / 7P and variants
        bool discrete = In(pid, { 0x2202, 0x2206, 0x220A, 0x223A, 0x227A, 0x22A4 });
        s = Headset(pid, L"SteelSeries Arctis Nova 7");
        s.connected = r[3] != 0x00;
        s.charging = r[3] == 0x01 || r[3] == 0x02;
        s.percent = discrete ? MapTo100(r[2], 0, 4) : Clamp100(r[2]);
    }
    if (!s.connected) s.percent = -1;
    return true;
}

// ---- mice --------------------------------------------------------------------------------------

DeviceStatus Mouse(USHORT pid, const wchar_t* name, bool wired) {
    DeviceStatus s;
    s.key = L"steelseries:" + Hex4(pid);
    s.name = name;
    s.kind = DeviceKind::Mouse;
    s.via = wired ? L"USB cable" : L"2.4 GHz receiver";
    return s;
}

// Sends `cmd` and returns the first input report that echoes it. SteelSeries GG keeps its own
// commands going on the same interface and the receiver only answers the latest one, so a single
// query is often lost: it is resent every 100 ms until the echo arrives, within one 1 s deadline.
// Reports that do not echo `cmd` (GG's replies, acknowledgements) are skipped, except 40 FF: the
// receiver's answer while it cannot reach the mouse, which ends the wait at once.
bool QueryEcho(HidDev& d, BYTE cmd, std::vector<BYTE>& r, size_t minLen) {
    d.Flush();
    const ULONGLONG start = GetTickCount64(), end = start + 1000;
    ULONGLONG resend = start;
    for (;;) {
        ULONGLONG now = GetTickCount64();
        if (now >= end) return false;
        if (now >= resend) {
            if (!d.Write({ 0x00, cmd })) return false;
            resend = now + 100;
        }
        int wait = (int)(std::min(resend, end) - now);
        if (wait <= 0 || d.Read(r, wait) < 0) continue;
        if (r.size() >= minLen && r[0] == cmd) return true;
        if (r.size() >= 2 && r[0] == 0x40 && r[1] == 0xFF) return false;
    }
}

// Aerox / Prime Wireless family on interface 3: 92 on the cable, D2 through the 2.4 GHz receiver
// (commands to the receiver carry the 0x40 flag). The reply comes in one of two layouts:
//  * steps: [1] = (level/5 + 1) | charging<<7, and 00 means the mouse is off or asleep.
//  * percent (Aerox 3 Wireless Gen 2): [1] 00 on battery, 01 charging, 02 charged with the cable
//    still in; [2] = 0..100, or FF while the mouse has not measured yet. The receiver answering
//    00 00 means the mouse is off.
// A mouse that is off or asleep is left out of the results, so the last level is kept, dimmed, for
// graceSec.
struct AeroxModel {
    USHORT pid;
    USHORT receiverPid;  // stable identity on both the receiver and the cable
    const wchar_t* name;
    bool wired;
    bool percent;  // Gen 2 layout
};

const AeroxModel kAerox[] = {
    { 0x1838, 0x1838, L"SteelSeries Aerox 3 Wireless", false, false },
    { 0x183A, 0x1838, L"SteelSeries Aerox 3 Wireless", true, false },
    { 0x1878, 0x1878, L"SteelSeries Aerox 3 Wireless CS2 Dragon Lore Edition", false, false },
    { 0x187A, 0x1878, L"SteelSeries Aerox 3 Wireless CS2 Dragon Lore Edition", true, false },
    { 0x1890, 0x1890, L"SteelSeries Aerox 3 Wireless Gen 2", false, true },
    { 0x1892, 0x1890, L"SteelSeries Aerox 3 Wireless Gen 2", true, true },
    { 0x1852, 0x1852, L"SteelSeries Aerox 5 Wireless", false, false },
    { 0x1854, 0x1852, L"SteelSeries Aerox 5 Wireless", true, false },
    { 0x1858, 0x1858, L"SteelSeries Aerox 9 Wireless", false, false },
    { 0x185A, 0x1858, L"SteelSeries Aerox 9 Wireless", true, false },
    { 0x1840, 0x1840, L"SteelSeries Prime Wireless", false, false },
    { 0x1842, 0x1840, L"SteelSeries Prime Wireless", true, false },
    { 0x1860, 0x1860, L"SteelSeries Aerox 5 Wireless Diablo IV Edition", false, false },
};

std::vector<USHORT> AeroxPids() {
    std::vector<USHORT> pids;
    for (const auto& m : kAerox) pids.push_back(m.pid);
    return pids;
}

bool Aerox(HidDev& d, USHORT pid, DeviceStatus& s) {
    auto m = std::find_if(std::begin(kAerox), std::end(kAerox), [&](const AeroxModel& a) { return a.pid == pid; });
    if (m == std::end(kAerox)) return false;
    const BYTE cmd = m->wired ? 0x92 : 0xD2;
    std::vector<BYTE> r;
    if (!QueryEcho(d, cmd, r, m->percent ? 3 : 2)) return false;
    if (m->percent && r[1] > 0x02) return false;  // not a Gen 2 status byte
    bool off = m->percent ? (!m->wired && r[1] == 0x00 && r[2] == 0x00) : (r[1] & 0x7F) == 0;
    if (off) return false;
    s = Mouse(pid, m->name, m->wired);
    s.key = L"steelseries:" + Hex4(m->receiverPid);
    s.graceSec = 1800;
    if (m->percent) {
        s.charging = r[1] != 0x00;
        if (r[2] <= 100) s.percent = r[2];
    } else if ((r[1] & 0x7F) <= 21) {  // only levels 1..21 represent 0..100%
        s.percent = ((r[1] & 0x7F) - 1) * 5;
        s.charging = (r[1] & 0x80) != 0;
    }
    return true;
}

// Rival 3 Wireless (+ Gen 2), Rival 650: AA 01 -> [0] %, [2] charging.
bool Rival(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x00, 0xAA, 0x01 }, r, 300) || r.size() < 3) return false;
    bool rival650 = pid == 0x172B || pid == 0x1726;
    s = Mouse(pid, rival650 ? L"SteelSeries Rival 650 Wireless" : pid == 0x1872 ? L"SteelSeries Rival 3 Wireless Gen 2" : L"SteelSeries Rival 3 Wireless",
              pid == 0x172B);
    if (rival650) s.key = L"steelseries:172B";  // one entry whether it answers on the cable or the receiver
    s.percent = Clamp100(r[0]);
    s.charging = r[2] != 0;
    return true;
}

// Rival Pro (UWB, hot-swappable batteries): 92 -> 92 00 <mouse %> <flag> <spare battery %>.
// From the Rival Pro support pull request on rivalcfg (github.com/flozz/rivalcfg/pull/294).
bool RivalPro(HidDev& d, USHORT pid, DeviceStatus& s) {
    d.Flush();
    if (!d.Write({ 0x00, 0x92 })) return false;
    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 500;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0 || d.Read(r, left) < 3) return false;
        if (r[0] == 0x92 && r[1] == 0x00) break;
    }
    if (r[2] > 100) return false;
    s = Mouse(pid, L"SteelSeries Rival Pro", false);
    s.via = L"UWB base station";
    s.percent = r[2];
    return true;
}

struct Model {
    std::vector<USHORT> pids;
    bool (*accept)(const HidColl&);
    bool (*poll)(HidDev&, USHORT, DeviceStatus&);
};

bool PageFF43(const HidColl& c) { return c.page() == 0xFF43 && c.usage() == 0x0202; }
bool PageFFC0(const HidColl& c) { return c.page() == 0xFFC0 && c.usage() == 0x0001; }
bool Iface0(const HidColl& c) { return c.iface == 0 && c.outLen() > 0; }
bool Iface2(const HidColl& c) { return c.iface == 2 && c.outLen() > 0; }
bool Iface3(const HidColl& c) { return c.iface == 3 && c.outLen() > 0; }
bool Iface4(const HidColl& c) { return c.iface == 4 && c.outLen() > 0; }

const Model kModels[] = {
    { { 0x12B3, 0x12B6, 0x12D7 }, PageFF43, Arctis1 },
    { { 0x1260, 0x12AD, 0x1252 }, PageFF43, Arctis7 },
    { { 0x12C2 }, Iface0, Arctis9 },
    { { 0x1290 }, Iface0, ArctisProWireless },
    { { 0x12E0, 0x225D }, Iface4, NovaProWireless },
    { { 0x220E, 0x2212, 0x2216, 0x2236, 0x2269, 0x226D, 0x2232, 0x2253, 0x230A, 0x2202, 0x22A1, 0x227E, 0x2206, 0x2258, 0x229E,
        0x22AD, 0x223A, 0x22A9, 0x227A, 0x22A4, 0x22A5, 0x220A, 0x22A7 },
      PageFFC0, NovaFamily },
    { AeroxPids(), Iface3, Aerox },
    { { 0x1830, 0x1872 }, Iface3, Rival },
    { { 0x172B, 0x1726 }, Iface0, Rival },  // Rival 650: cable, 2.4 GHz receiver
    { { 0x1880 }, Iface2, RivalPro },
};

}  // namespace

void PollSteelSeries(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x1038)) {
        for (const auto& m : kModels) {
            if (!In(pid, m.pids)) continue;
            for (const HidColl* c : ctx.Find(vid, pid, m.accept)) {
                HidDev d;
                DeviceStatus s;
                if (!d.Open(*c) || !m.poll(d, pid, s)) continue;
                // Aerox/Prime and Rival 650 can answer on both products; keep one entry, preferring the cable.
                auto dup = std::find_if(ctx.out.begin(), ctx.out.end(), [&](const DeviceStatus& o) { return o.key == s.key; });
                if (dup == ctx.out.end()) ctx.Add(s, vid, pid);
                else {
                    if (s.via == L"USB cable") *dup = s;
                    ctx.Claim(vid, pid);
                }
                break;
            }
            break;
        }
    }
}

}

void PollSteelSeries(PollContext& ctx) { wb_driver_steelseries::PollSteelSeries(ctx); }

namespace wb_driver_steelseries_elite {
// SteelSeries Arctis Nova Elite (base station 1038:2244), without SteelSeries GG. A different exchange from
// the Nova 7 / Nova Pro family in wb_driver_steelseries.
// Ported from HaloBattery (github.com/HeyOkay/HaloBattery, providers/steelseries_elite.py, MIT; the level and
// charging were confirmed there against SteelSeries GG on a real station), from elegos/Linux-Arctis-Manager
// and loteran/Arctis-Sound-Manager (nova_elite.yaml, made from a USB capture of SteelSeries GG).
//   request: 64-byte output report 01 B0 00 ... to an interface-3 vendor collection (FFC0 first)
//   direct reply 01 B0: [6] headset level, [14] power (01 headset off, 02 on the cable), [15] charging (02)
//   or pushed frames 07 B7 <level> <spare> <charging 02> and 07 B5 <?> <bt> <power>, on either collection.
// Only the status request is sent.
namespace {

constexpr USHORT kVid = 0x1038, kPid = 0x2244;

}  // namespace

void PollSteelSeriesElite(PollContext& ctx) {
    if (ctx.IsClaimed(kVid, kPid)) return;
    auto colls = ctx.Find(kVid, kPid, [](const HidColl& c) { return c.iface == 3 && (c.inLen() > 0 || c.outLen() > 0); });
    if (colls.empty()) return;
    // FFC0 first, then other vendor pages (the station splits interface 3 into two collections).
    std::stable_sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) {
        auto rank = [](const HidColl* c) { return c->page() == 0xFFC0 ? 0 : c->page() >= 0xFF00 ? 1 : 2; };
        return rank(a) < rank(b);
    });
    std::vector<std::unique_ptr<HidDev>> handles;
    for (const HidColl* c : colls) {
        auto d = std::make_unique<HidDev>();
        if (d->Open(*c)) {
            d->Flush();
            handles.push_back(std::move(d));
        }
    }
    bool sent = false;
    for (auto& d : handles)
        if (d->coll().page() >= 0xFF00 && d->coll().outLen() > 0) {
            std::vector<BYTE> req(64, 0);
            req[0] = 0x01;
            req[1] = 0xB0;
            if (d->Write(req)) { sent = true; break; }  // a collection without report 01 refuses it
        }
    if (!sent) return;
    int level = -1, power = -1;
    bool charging = false;
    ULONGLONG end = GetTickCount64() + 1000;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0 && (level < 0 || power < 0)) {
        for (auto& d : handles) {
            int n = d->Read(r, 30);
            if (n < 5) continue;
            if (r[0] == 0x01 && r[1] == 0xB0 && n > 15) {  // the direct reply
                if (r[6] <= 100) level = r[6];
                charging = r[15] == 0x02;
                power = r[14];
            } else if (r[0] == 0x07 && r[1] == 0xB7 && r[2] <= 100) {
                level = r[2];
                charging = r[4] == 0x02;
            } else if (r[0] == 0x07 && r[1] == 0xB5) {
                power = r[4];
            }
        }
        if (power == 0x01) break;  // headset off
    }
    if (power == 0x01 || level < 0) return;
    DeviceStatus s;
    s.key = L"steelseries:2244";
    s.name = L"SteelSeries Arctis Nova Elite";
    s.kind = DeviceKind::Headset;
    s.via = L"Base station";
    s.percent = level;
    s.charging = charging || power == 0x02;
    ctx.Add(s, kVid, kPid);
}

}

void PollSteelSeriesElite(PollContext& ctx) { wb_driver_steelseries_elite::PollSteelSeriesElite(ctx); }
namespace wb_driver_switch_pro {
// Switch Pro-compatible controllers (057E:2009), including the GameSir Tarantula Pro
// over its 2.4 GHz receiver in Switch mode.
// SDL's SDL_hidapi_switch.c documents byte 2 of full input report 0x30:
// bits 7..5 are the battery level (0..4) and bit 4 indicates charging.
// Steam Input or another host must enable full reports. The driver only listens.

void PollSwitchPro(PollContext& ctx) {
    constexpr USHORT vid = 0x057E;
    constexpr USHORT pid = 0x2009;
    static constexpr const wchar_t* levels[] = {
        L"Empty", L"Critical", L"Low", L"Medium", L"Full",
    };

    for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
             return c.page() == 0x01 && (c.usage() == 0x04 || c.usage() == 0x05) && c.inLen() >= 13;
         })) {
        HidDev d;
        if (!d.OpenReadOnly(*c)) continue;

        const ULONGLONG deadline = GetTickCount64() + 350;
        std::vector<BYTE> report;
        for (ULONGLONG now = GetTickCount64(); now < deadline; now = GetTickCount64()) {
            const int size = d.Read(report, (int)(deadline - now));
            if (size < 0) break;
            if (size < 13 || report[0] != 0x30) continue;

            const int level = (report[2] & 0xE0) >> 5;
            if (level > 4) continue;

            // Use the HID path for identity, as in the PlayStation driver.
            UINT pathHash = 2166136261u;
            for (wchar_t ch : c->path) {
                pathHash ^= ch;
                pathHash *= 16777619u;
            }
            DeviceStatus s;
            s.key = L"switchpro:" + std::to_wstring(pathHash);
            s.name = HidProduct(*c);
            if (s.name.empty()) s.name = L"Nintendo Switch Pro Controller";
            s.kind = DeviceKind::Gamepad;
            s.via = L"Switch mode";
            s.percent = 25 * level;
            s.levelText = levels[level];
            s.charging = (report[2] & 0x10) != 0 && level < 4;
            s.graceSec = 300;
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollSwitchPro(PollContext& ctx) { wb_driver_switch_pro::PollSwitchPro(ctx); }
namespace wb_driver_vaxee {
// VAXEE 4K and 8K receivers and ZYGEN NP-01S V2 USB cable. Protocol reference:
// https://github.com/stuffz/mouse-battery-monitor/blob/main/docs/VAXEE.md
// Only battery (0B) and charging (10) reads are sent; never configuration writes.
namespace {
constexpr USHORT kVid = 0x3057;
constexpr USHORT kReceiver = 0x1002;
constexpr USHORT kReceiver8K = 0x2002;
constexpr USHORT kCable = 0x1011;

bool Supported(USHORT pid) { return pid == kReceiver || pid == kReceiver8K || pid == kCable; }

bool Query(HidDev& d, BYTE command, BYTE& value) {
    std::vector<BYTE> request(64, 0);
    request[0] = 0x0E;
    request[1] = 0xA5;
    request[2] = command;
    request[3] = 0x01;  // READ, not WRITE (02)
    request[4] = 0x01;
    if (!d.SetFeature(request)) return false;
    Sleep(100);
    std::vector<BYTE> reply;
    if (!d.GetFeature(0x0E, reply) || reply.size() < 6 ||
        reply[0] != 0x0E || reply[1] != 0xA5 ||
        reply[2] != command || reply[4] != 1) return false;
    value = reply[5];
    return true;
}
}  // namespace

void PollVaxee(PollContext& ctx) {
    // End the UI's sleeping-device grace period immediately when the physical
    // USB device disappears (including moving the cable from receiver to mouse).
    static std::map<std::wstring, DeviceStatus> lastGood;
    for (auto it = lastGood.begin(); it != lastGood.end();) {
        bool present = false;
        for (const auto& c : ctx.colls)
            if (c.path == it->first) { present = true; break; }
        if (present) { ++it; continue; }
        DeviceStatus gone = it->second;
        gone.connected = false;
        gone.percent = -1;
        gone.charging = false;
        ctx.out.push_back(gone);
        it = lastGood.erase(it);
    }
    for (const auto& product : ctx.Products(kVid)) {
        const USHORT pid = product.second;
        if (!Supported(pid)) continue;
        for (const HidColl* c : ctx.Find(kVid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF05 && c.usage() == 1 && c.featLen() == 64;
             })) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            BYTE battery = 0;
            bool valid = false;
            for (int attempt = 0; attempt < 2 && !valid; ++attempt)
                valid = Query(d, 0x0B, battery) && battery <= 20;
            if (!valid) continue;  // UI keeps the last reading dimmed during the grace period.
            DeviceStatus s;
            // A "gone" entry pushed above for the same mouse on its old port is replaced, not duplicated.
            s.key = ctx.StableKey(L"vaxee:" + Hex4(pid));
            ctx.out.erase(std::remove_if(ctx.out.begin(), ctx.out.end(), [&](const DeviceStatus& o) {
                              return o.key == s.key && !o.connected;
                          }), ctx.out.end());
            s.name = pid == kCable        ? L"ZYGEN NP-01S V2 Wireless"
                     : pid == kReceiver8K ? L"VAXEE 8K Wireless"
                                          : L"VAXEE wireless mouse";
            s.kind = DeviceKind::Mouse;
            s.via = pid == kCable ? L"USB cable" : L"2.4 GHz receiver";
            s.percent = battery * 5;
            s.graceSec = 1800;
            BYTE charging = 0;
            if (Query(d, 0x10, charging)) s.charging = charging != 0;
            lastGood[c->path] = s;
            ctx.Add(s, kVid, pid);
            break;
        }
    }
}

}

void PollVaxee(PollContext& ctx) { wb_driver_vaxee::PollVaxee(ctx); }
namespace wb_driver_wlmouse {
// WLMouse (VID 36A7): Beast X 8K, Beast X Mini / Mini Pro, Beast X Pro, Beast MAX, Sword X 8K, Beast G and
// their receivers. The Sword X 8K ids (A878 receiver, A879 cable) and its FFFF:0000 collection come from an
// owner (BatteryHub issue #75). The Beast X V2 8K receiver (A856) comes from an owner (BatteryHub issue #121):
// it exposes the same 65-byte FFFF:0000 collection on interface 2 as the other 8K receivers, so it gets the
// same query (HaloBattery sends it to every 36A7 product id). Its cable id is not known yet.
// Beast G (BatteryHub issue #237): A860 is its 8K receiver and A861 its cable, per the official WL MOUSE HUB
// device list as recorded by github.com/late-autumn-gif/mousedock (docs/device-db/mouse-db.json, same
// Feature 0 / 0x83 battery query), github.com/OpenMouse-Project/mouse-protocol (src/drivers/vendors.ts) and
// github.com/Zelozzzz/dorsal (src/dorsal/models.py).
// The YING 8K receiver (A874) is handled by the Glorious driver.
// Read-only status query on the vendor collection (usage page FFFF, 65-byte feature report 0):
//   SET_FEATURE [00][00][00][02][02][00][83][00 ...], then GET_FEATURE report 0 until
//   [00][status][00][02][02][00][83][charging][battery%]; status A1 / A2 = answered, A0 = mouse asleep.
//   No checksum. The receiver answers for the mouse; a silent receiver means the mouse is asleep or off.
// From github.com/djnawara/mouse-gauge (verified on a Beast X 8K through its A883 receiver) and
// github.com/Waturus/HaloBattery (providers/wlmouse.py; A880 receiver confirmed on hardware).
// Not handled: A887 / A888 (Beast X 4K / original Beast X). The sources disagree on their protocol
// (an output report 04 query, or a different CRC framing) and one reports they never answer this query.
namespace {

const USHORT kVid = 0x36A7;

// Cables come before receivers: a mouse on its cable with the receiver also plugged in shows up under both
// ids, so a cable that answers claims its receiver (`receiverPid`) and the mouse is listed once.
struct Model { USHORT pid; bool receiver; const wchar_t* name; USHORT receiverPid; };
const Model kModels[] = {
    { 0xA879, false, L"WLMouse Sword X 8K", 0xA878 },
    { 0xA884, false, L"WLMouse Beast X 8K", 0xA883 },
    { 0xA861, false, L"WLMouse Beast G", 0xA860 },
    { 0xA866, false, L"WLMouse mouse", 0 },
    { 0xA867, false, L"WLMouse mouse", 0 },
    { 0xA871, false, L"WLMouse Beast X Pro", 0xA870 },  // cable; tested on the charging Beast X Pro
    { 0xA856, true, L"WLMouse Beast X V2", 0 },  // 8K receiver; seen with a Beast X V2 Max
    { 0xA860, true, L"WLMouse Beast G", 0 },  // 8K receiver
    { 0xA868, true, L"WLMouse Beast X Mini Pro", 0 },
    { 0xA870, true, L"WLMouse Beast X Pro", 0 },
    { 0xA878, true, L"WLMouse Sword X 8K", 0 },
    { 0xA880, true, L"WLMouse Beast MAX", 0 },
    { 0xA883, true, L"WLMouse Beast X 8K", 0 },
    { 0xA885, true, L"WLMouse Beast X Mini", 0 },
};

enum class Reply { Battery, Asleep, None };

Reply Query(HidDev& d, int& pct, bool& charging) {
    std::vector<BYTE> req(65, 0);  // [0] = report id 0
    req[3] = 0x02;
    req[4] = 0x02;
    req[6] = 0x83;
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (!d.SetFeature(req)) return Reply::None;
        for (int read = 0; read < 6; ++read) {
            Sleep(60);
            std::vector<BYTE> r;
            if (!d.GetFeature(0x00, r) || r.size() < 9) return Reply::None;
            if (r[3] != 0x02 || r[4] != 0x02 || r[6] != 0x83) continue;  // not the reply (yet)
            if (r[1] == 0xA0) return Reply::Asleep;
            if (r[1] != 0xA1 && r[1] != 0xA2) continue;  // still our own request echoed back
            if (r[8] > 100) return Reply::None;
            pct = r[8];
            charging = r[7] == 0x01;
            return Reply::Battery;
        }
    }
    return Reply::None;
}

}  // namespace

void PollWlmouse(PollContext& ctx) {
    for (const Model& m : kModels) {
        if (ctx.IsClaimed(kVid, m.pid)) continue;
        auto colls = ctx.Find(kVid, m.pid, [](const HidColl& c) { return c.page() == 0xFFFF && c.featLen() >= 65; });
        // Usage 0 is the collection that answers in both sources; try it first.
        std::stable_sort(colls.begin(), colls.end(), [](const HidColl* a, const HidColl* b) {
            return (a->usage() == 0) > (b->usage() == 0);
        });
        for (const HidColl* c : colls) {
            HidDev d;
            if (!d.OpenFeatureOnly(*c)) continue;
            int pct = -1;
            bool charging = false;
            Reply rep = Query(d, pct, charging);
            if (rep == Reply::None) continue;
            DeviceStatus s;
            s.key = L"wlmouse:" + Hex4(m.pid);
            s.name = m.name;
            s.kind = DeviceKind::Mouse;
            s.via = m.receiver ? L"2.4 GHz receiver" : L"USB cable";
            if (m.receiver) s.graceSec = 1800;  // a sleeping mouse can leave the receiver silent
            if (rep == Reply::Asleep) {
                s.asleep = true;  // shown with the last known value
            } else {
                s.percent = pct;
                s.charging = charging;
            }
            ctx.Add(s, kVid, m.pid);
            if (m.receiverPid) ctx.Claim(kVid, m.receiverPid);  // same mouse, already listed via its cable
            break;
        }
    }
}

}

void PollWlmouse(PollContext& ctx) { wb_driver_wlmouse::PollWlmouse(ctx); }
namespace wb_driver_xinput {
namespace {

using namespace ABI::Windows::Gaming::Input;
using ABI::Windows::Devices::Power::IBatteryReport;

template <class T>
void Release(T*& p) {
    if (p) p->Release();
    p = nullptr;
}

IRawGameControllerStatics* Statics() {
    static IRawGameControllerStatics* statics = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        RoInitialize(RO_INIT_MULTITHREADED);  // our poll thread; harmless if already initialised
        const wchar_t* cls = L"Windows.Gaming.Input.RawGameController";
        HSTRING name = nullptr;
        if (SUCCEEDED(WindowsCreateString(cls, (UINT32)wcslen(cls), &name))) {
            RoGetActivationFactory(name, __uuidof(IRawGameControllerStatics), (void**)&statics);
            WindowsDeleteString(name);
        }
        if (statics) Sleep(300);  // the controller list fills in asynchronously after first use
    }
    return statics;
}

// Vendors whose controllers report a real battery through Windows.Gaming.Input:
// Microsoft, and GameSir in Xbox (GIP) mode (e.g. G7 Pro on its receiver, 3537:105E).
bool WgiVendor(UINT16 vid) { return vid == 0x045E || vid == 0x3537; }

// Adds Xbox controllers with a real percentage. Returns how many were added; `thirdParty` counts
// the non-Microsoft ones among them.
int PollWgi(PollContext& ctx, int& thirdParty) {
    thirdParty = 0;
    IRawGameControllerStatics* statics = Statics();
    if (!statics) return 0;
    __FIVectorView_1_Windows__CGaming__CInput__CRawGameController* list = nullptr;
    if (FAILED(statics->get_RawGameControllers(&list)) || !list) return 0;
    UINT32 count = 0;
    list->get_Size(&count);
    int added = 0;
    for (UINT32 i = 0; i < count; i++) {
        IRawGameController* raw = nullptr;
        if (FAILED(list->GetAt(i, &raw)) || !raw) continue;
        UINT16 vid = 0, pid = 0;
        raw->get_HardwareVendorId(&vid);
        raw->get_HardwareProductId(&pid);
        if (ctx.IsClaimed(vid, pid)) { Release(raw); continue; }  // a vendor driver already reads this pad
        IGameControllerBatteryInfo* info = nullptr;
        IBatteryReport* report = nullptr;
        if (WgiVendor(vid) && SUCCEEDED(raw->QueryInterface(__uuidof(IGameControllerBatteryInfo), (void**)&info)) && info &&
            SUCCEEDED(info->TryGetBatteryReport(&report)) && report) {
            __FIReference_1_INT32 *rem = nullptr, *full = nullptr;
            report->get_RemainingCapacityInMilliwattHours(&rem);
            report->get_FullChargeCapacityInMilliwattHours(&full);
            INT32 r = 0, f = 0;
            if (rem) rem->get_Value(&r);
            if (full) full->get_Value(&f);
            ABI::Windows::System::Power::BatteryStatus st = ABI::Windows::System::Power::BatteryStatus_NotPresent;
            report->get_Status(&st);
            if (f > 0 && st != ABI::Windows::System::Power::BatteryStatus_NotPresent) {
                DeviceStatus s;
                s.key = L"xbox:" + Hex4(pid) + L":" + std::to_wstring(i);
                s.name = L"Xbox controller";
                IRawGameController2* raw2 = nullptr;
                if (SUCCEEDED(raw->QueryInterface(__uuidof(IRawGameController2), (void**)&raw2)) && raw2) {
                    HSTRING dn = nullptr;
                    if (SUCCEEDED(raw2->get_DisplayName(&dn)) && dn) {
                        UINT32 len = 0;
                        const wchar_t* p = WindowsGetStringRawBuffer(dn, &len);
                        if (p && len) s.name.assign(p, len);
                        WindowsDeleteString(dn);
                    }
                    Release(raw2);
                }
                // GIP reports a generic "Xbox One Game Controller" for third-party pads.
                if (vid == 0x3537) s.name = L"GameSir controller";
                s.kind = DeviceKind::Gamepad;
                s.via = L"Xbox Wireless / Bluetooth";
                s.percent = Clamp100(r * 100 / f);
                s.charging = st == ABI::Windows::System::Power::BatteryStatus_Charging;
                ctx.Add(s, vid, pid);
                added++;
                if (vid != 0x045E) thirdParty++;
            }
            Release(rem);
            Release(full);
        }
        Release(report);
        Release(info);
        Release(raw);
    }
    Release(list);
    return added;
}

using GetBatteryFn = DWORD(WINAPI*)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);
using GetStateFn = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);

struct XInputApi { GetBatteryFn battery = nullptr; GetStateFn state = nullptr; };

const XInputApi& LoadXInput() {
    static XInputApi api = [] {
        XInputApi a;
        for (const wchar_t* dll : { L"xinput1_4.dll", L"xinput9_1_0.dll" }) {
            if (HMODULE m = LoadLibraryExW(dll, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
                a.battery = (GetBatteryFn)GetProcAddress(m, "XInputGetBatteryInformation");
                a.state = (GetStateFn)GetProcAddress(m, "XInputGetState");
                if (a.battery && a.state) break;
            }
        }
        return a;
    }();
    return api;
}

}  // namespace

void PollXInput(PollContext& ctx) {
    // An Xbox controller on Bluetooth may already be listed with a real percentage.
    bool btXbox = false;
    for (auto& s : ctx.out) btXbox |= s.via == L"Bluetooth" && s.name.find(L"Xbox") != std::wstring::npos;
    int thirdParty = 0;
    int precise = btXbox ? 0 : PollWgi(ctx, thirdParty);
    // GameSir pads answer XInput's headset query with their own battery ("Full") even with nothing
    // plugged in, so skip headsets when every precisely shown controller is third-party.
    bool skipHeadset = precise > 0 && thirdParty == precise;

    const XInputApi& api = LoadXInput();
    if (!api.battery || !api.state) return;
    auto query = [&](DWORD user, BYTE type, XINPUT_BATTERY_INFORMATION& bi) {
        bi = {};
        return api.battery(user, type, &bi) == ERROR_SUCCESS && bi.BatteryType != BATTERY_TYPE_DISCONNECTED &&
               bi.BatteryType != BATTERY_TYPE_WIRED;
    };
    for (DWORD user = 0; user < 4; user++) {
        // The Xbox Wireless adapter answers battery queries for empty slots too (one controller
        // showed up as four "Medium" headsets), so only look at slots with a controller connected.
        XINPUT_STATE state{};
        if (api.state(user, &state) != ERROR_SUCCESS) continue;
        XINPUT_BATTERY_INFORMATION pad{};
        const bool hasPad = query(user, BATTERY_DEVTYPE_GAMEPAD, pad);
        for (BYTE type : { (BYTE)BATTERY_DEVTYPE_GAMEPAD, (BYTE)BATTERY_DEVTYPE_HEADSET }) {
            if (type == BATTERY_DEVTYPE_GAMEPAD && (btXbox || precise > 0)) continue;  // already shown precisely
            if (type == BATTERY_DEVTYPE_HEADSET && skipHeadset) continue;
            XINPUT_BATTERY_INFORMATION bi{};
            if (type == BATTERY_DEVTYPE_GAMEPAD) {
                if (!hasPad) continue;
                bi = pad;
            } else {
                if (!query(user, type, bi)) continue;
                // Without a headset plugged in, the headset query can still answer with an unknown type.
                if (bi.BatteryType == BATTERY_TYPE_UNKNOWN) continue;
                // Xbox One / Series controllers echo their own battery back for the headset query.
                // Headsets on these controllers are powered by the controller, so a mirrored answer
                // is not a headset.
                if (hasPad && bi.BatteryType == pad.BatteryType && bi.BatteryLevel == pad.BatteryLevel) continue;
            }
            DeviceStatus s;
            s.key = L"xinput:" + std::to_wstring(user) + L":" + std::to_wstring(type);
            s.name = type == BATTERY_DEVTYPE_GAMEPAD ? L"Xbox controller " + std::to_wstring(user + 1) : L"Xbox headset";
            s.kind = type == BATTERY_DEVTYPE_GAMEPAD ? DeviceKind::Gamepad : DeviceKind::Headset;
            s.via = L"Xbox Wireless";
            switch (bi.BatteryLevel) {
                case BATTERY_LEVEL_EMPTY:  s.percent = 5;   s.levelText = L"Empty";  break;
                case BATTERY_LEVEL_LOW:    s.percent = 25;  s.levelText = L"Low";    break;
                case BATTERY_LEVEL_MEDIUM: s.percent = 60;  s.levelText = L"Medium"; break;
                default:                   s.percent = 100; s.levelText = L"Full";   break;
            }
            ctx.out.push_back(s);
        }
    }
}

}

void PollXInput(PollContext& ctx) { wb_driver_xinput::PollXInput(ctx); }

namespace wb_driver_zowie {
// BenQ ZOWIE wireless mice (VID 04A5). Zowie ships no software; these vendor commands were
// reverse-engineered by the community. Only the read-only battery commands below are ever sent
// (other EC-CW commands reboot the receiver and unpair the mouse).
//  * EC-CW series (EC1/EC2/EC3-CW): receiver 8006, cable 8005. Unnumbered 64-byte output report
//    [A5 03]; reply [mV lo][mV hi][percent]... mV = 0 when the mouse is not linked.
//    The Enhanced Receiver declares this collection on usage page 00FF instead of FFxx.
//    From github.com/yleshchuk/zowie-battery.
//  * DW series (U2-DW, S2-DW ...): receiver 800A, EC-DW cable 800D. Output report 08 [B4 15] on usage
//    page FF03, reply input report 09 on usage page FF04: mV = ([1] * 256 + [2]) * 10, [4] == C8.
//    From github.com/IgnacioBarb/zowie-battery-tray (MIT).
//  * U2: receiver 8008 (product string "BenQ ZOWIE Gaming Mouse"), same FF03 / FF04 layout and
//    commands as the DW series.
// On the charging dock the mouse charges over contacts, so nothing new shows up on USB; over radio
// only the voltage arrives. Charging is therefore inferred from how the voltage moves (same rules
// as zowie-battery-tray): a rise of 50 mV above the lowest reading means it went on the dock, a
// drop of 20 mV below the highest means it came off.
namespace {

// Voltage -> % anchored on the mouse's own LED thresholds (10 / 25 / 50 %).
int ZowiePercent(int mv) {
    static const int v[] = { 4200, 4000, 3890, 3760, 3680, 3500 };
    static const int p[] = { 100, 75, 50, 25, 10, 0 };
    return Interpolate(mv, v, p, 6);
}

// Charging guess from the voltage trend of one receiver. Only touched by the polling thread.
struct ChargeTrend {
    bool has = false, charging = false;
    int peak = 0, trough = 0;

    bool Update(int mv) {
        if (!has) {
            has = true;
            charging = mv >= 4250;  // a resting Li-ion cell never sits above ~4.2 V
            peak = trough = mv;
        } else if (charging) {
            peak = std::max(peak, mv);
            if (mv <= peak - 20) { charging = false; trough = mv; }
        } else {
            trough = std::min(trough, mv);
            if (mv >= trough + 50) { charging = true; peak = mv; }
        }
        return charging;
    }
};
std::map<USHORT, ChargeTrend> g_trend;

bool PollCw(PollContext& ctx, USHORT pid) {
    for (const HidColl* c : ctx.Find(0x04A5, pid, [](const HidColl& c) {
             return (c.page() >= 0xFF00 || c.page() == 0x00FF) && c.outLen() == 65;
         })) {
        HidDev d;
        if (!d.Open(*c)) continue;
        d.Flush();
        if (!d.Write({ 0x00, 0xA5, 0x03 })) continue;
        std::vector<BYTE> r;
        if (d.Read(r, 1000) < 3) continue;
        int mv = r[0] | (r[1] << 8);
        if (mv != 0 && (mv < 3000 || mv > 4500)) continue;  // not a battery reply
        DeviceStatus s;
        s.key = L"zowie:" + Hex4(pid);
        s.name = L"ZOWIE EC-CW";
        s.kind = DeviceKind::Mouse;
        s.via = pid == 0x8005 ? L"USB cable" : L"2.4 GHz receiver";
        s.charging = pid == 0x8005;
        if (mv == 0) {
            s.connected = false;
        } else {
            s.millivolts = mv;
            s.percent = (r[2] >= 1 && r[2] <= 100) ? r[2] : ZowiePercent(mv);
        }
        ctx.Add(s, 0x04A5, pid);
        return true;
    }
    return false;
}

bool PollDw(PollContext& ctx, USHORT pid) {
    auto cmd = ctx.Find(0x04A5, pid, [](const HidColl& c) { return c.page() == 0xFF03 && c.outLen() == 16; });
    auto rep = ctx.Find(0x04A5, pid, [](const HidColl& c) { return c.page() == 0xFF04 && c.inLen() == 16; });
    if (cmd.empty() || rep.empty()) return false;
    HidDev in, out;
    if (!in.Open(*rep[0]) || !out.Open(*cmd[0])) return false;  // open the reply channel first
    in.Flush();
    if (!out.Write({ 0x08, 0xB4, 0x15 })) return false;
    std::vector<BYTE> r;
    ULONGLONG end = GetTickCount64() + 1500;
    bool ok = false;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (in.Read(r, left) < 5) break;
        if (r[0] == 0x09) { ok = true; break; }
    }
    if (!ok || r[1] == 0xE6 || r[4] != 0xC8) return false;
    int mv = (r[1] * 256 + r[2]) * 10;
    bool cable = pid != 0x800A && pid != 0x8008;
    DeviceStatus s;
    s.key = L"zowie:" + Hex4(pid);
    s.name = HidProduct(*cmd[0]);
    if (pid == 0x8008) s.name = L"ZOWIE U2";
    else if (s.name.empty() || s.name.find(L"Receiver") != std::wstring::npos) s.name = L"ZOWIE DW mouse";
    s.kind = DeviceKind::Mouse;
    s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
    if (mv == 0) {
        s.connected = false;
    } else {
        s.millivolts = mv;
        s.percent = ZowiePercent(mv);
        s.charging = cable || g_trend[pid].Update(mv);
    }
    ctx.Add(s, 0x04A5, pid);
    return true;
}

}  // namespace

void PollZowie(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x04A5)) {
        if (pid == 0x8005 || pid == 0x8006) PollCw(ctx, pid);
        else if (pid == 0x800A || pid == 0x800D || pid == 0x8008) PollDw(ctx, pid);
    }
}

}

void PollZowie(PollContext& ctx) { wb_driver_zowie::PollZowie(ctx); }

namespace wb_poll {
// Runs every driver in order. Vendor drivers go first and claim the devices they handle; the
// generic fallbacks then only look at what is left.
std::vector<DeviceStatus> PollAllDevices(HANDLE stopEvent) {
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return {};
    StartAirPods();
    PollContext ctx;
    ctx.colls = EnumHid();
    ctx.colls.erase(std::remove_if(ctx.colls.begin(), ctx.colls.end(),
                                   [](const HidColl& c) { return ModConfig::IsIgnored(c.vid, c.pid); }),
                    ctx.colls.end());
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGlorious(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAula(ctx);  // before Kysona, which shares the 3554 vendor id and would probe it first
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAulaMouse(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollDareu(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollDarmoshark(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollDarmosharkNrf(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAtkSerial(ctx);  // before Kysona, which shares the ATK vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAtkKeyboard(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollPulsar(ctx);  // before Kysona, which also reads vendor 3710
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAtkBitmouse(ctx);  // before Kysona, which shares the ATK vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollKysona(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollCherry(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollArbiter(ctx);  // before LAMZU, which shares the 373E vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollLamzu(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollEndgame(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollEndgameWe(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollVaxee(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollWlmouse(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollNinjutso(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSprime(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAkko(ctx);  // before Rongyuan, same vendor id 38EE
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRongyuan(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSonyInzone(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollFractal(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollJbl(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRog(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollZowie(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchoseA5(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRapoo(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAreson(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchoseG3(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMsi(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMachenike(ctx);  // before the Ajazz readers in MiscMice, same 248A vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchoseModern(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchoseHeadset(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchoseKeyboard(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMiscMice(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRazerHeadsets(ctx);  // before the generic Razer driver, which shares the vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollHyperX(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAstro(ctx);  // before Logitech, which shares the 046D vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollLogitech(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRazer(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSteelSeriesElite(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSteelSeries(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAudeze(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollEndorfy(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAkg(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollNzxt(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollCorsair(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollCoolerMaster(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollPlayStation(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSwitchPro(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGameSir(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollPhoneLink(ctx);  // before the Bluetooth fallback, which can list the phone too
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAlienware(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollBluetooth(ctx);  // before the generic HID check: Windows' own level wins for the same device
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGenericHid(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAirPods(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollXInput(ctx);
    return ctx.out;
}

}

std::vector<DeviceStatus> PollAllDevices(HANDLE stopEvent) { return wb_poll::PollAllDevices(stopEvent); }

namespace wb_app {
// Tray app showing the battery level of wireless mice, headsets and Bluetooth devices (from BatteryHub's main.cpp).
namespace {

constexpr UINT WM_TRAY = WM_APP + 1;
constexpr UINT WM_RESULTS = WM_APP + 2;
constexpr UINT WM_WIDGET_CLICK = WidgetIpc::kClickMessage;
constexpr UINT_PTR TIMER_DEBOUNCE = 1;
constexpr UINT_PTR TIMER_WIDGET = 2;
constexpr ULONGLONG WIDGET_GRACE_MS = 10000;

constexpr UINT PLACEHOLDER_ID = 1;

enum MenuId : UINT { ID_REFRESH = 100, ID_DEVICE_FIRST = 1000 };

const wchar_t* kAppName = L"Taskbar Wireless Battery Widget";

HWND g_hwnd;
HANDLE g_refreshEvt, g_stopEvt, g_worker, g_airpodsEvt;
UINT g_taskbarCreatedMsg;
bool g_lightTaskbar;
bool g_wantWidget = true;      // user's choice: taskbar widget (default) or tray icons
bool g_useWidget = false;      // what is actually shown right now
HWND g_receiver = nullptr;     // Explorer's widget receiver while the taskbar widget is attached
ULONGLONG g_receiverSeen = 0;  // when the receiver last existed, or the start
bool g_hideLastKnown = false;  // hide (last known) / asleep devices from taskbar and tray

struct TrayEntry {
    UINT uid = 0;
    DeviceStatus st;
    int lastPct = -1;        // last known percentage, kept while the device sleeps
    ULONGLONG lastSeen = 0;
    bool added = false;
    bool lowWarned = false;
    std::wstring iconSig;    // what the current icon shows, to avoid needless redraws
    HICON icon = nullptr;
};
std::map<std::wstring, TrayEntry> g_entries;
bool g_placeholder = false;
HICON g_placeholderIcon = nullptr;

// ---- helpers ----------------------------------------------------------------------------------

UINT StableId(const std::wstring& key) {
    UINT h = 2166136261u;
    for (wchar_t c : key) { h ^= c; h *= 16777619u; }
    return (h & 0x7FFFFFFF) | 0x100;
}

bool ReadLightTaskbar() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
        return v != 0;
    return false;
}

// Device keys the user chose to hide, stored in Windhawk mod storage.
std::set<std::wstring> g_hidden;

void LoadHidden() {
    g_hidden.clear();
    std::vector<wchar_t> data(65536, 0);
    Wh_GetStringValue(L"HiddenDevices", data.data(), data.size());
    std::wstring text(data.data());
    for (size_t start = 0; start < text.size();) {
        // Accept the old newline format and the spaces written by portable 1.7.3.
        // Keys contain fixed prefixes, hex IDs, numbers and GUIDs, never these separators.
        size_t end = text.find_first_of(L"| \r\n", start);
        if (end != start) g_hidden.insert(text.substr(start, end == std::wstring::npos ? end : end-start));
        if (end == std::wstring::npos) break;
        start = end+1;
    }
}

void SaveHidden() {
    std::wstring text;
    // A single-line separator also works with Windhawk's portable INI storage.
    for (const auto& key : g_hidden) {
        if (!text.empty()) text += L'|';
        text += key;
    }
    Wh_SetStringValue(L"HiddenDevices", text.c_str());
}

bool IsHidden(const std::wstring& key) { return g_hidden.count(key) != 0; }

bool IsEntryHidden(const TrayEntry& e) {
    return IsHidden(e.st.key) || (g_hideLastKnown && (e.st.asleep || (e.st.percent < 0 && e.lastPct >= 0)));
}

bool LoadWidgetMode() { return ModConfig::taskbar.load(); }

bool LoadHideLastKnown() { return ModConfig::hideSleeping.load(); }

// ---- icon rendering ---------------------------------------------------------------------------
// Draws the percentage as large digits with a small level bar underneath. GDI text is rendered
// white-on-black into a scratch bitmap and its brightness is used as the alpha channel, which
// gives smooth edges on the transparent icon.

struct Rgb { BYTE r, g, b; };

HICON RenderIcon(const std::wstring& text, int barPct, Rgb textColor, Rgb barColor, bool dim) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 0;
    const int S = GetSystemMetricsForDpi(SM_CXSMICON, dpi ? dpi : GetDpiForSystem());
    const int barH = std::max(2, S / 8);
    const int textH = S - barH - 1;

    HDC dc = CreateCompatibleDC(nullptr);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    // scratch bitmap for the glyphs
    const int W = S * 3, H = S * 2;
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    void* sbits = nullptr;
    HBITMAP scratch = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &sbits, nullptr, 0);
    HGDIOBJ oldBmp = SelectObject(dc, scratch);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    auto* sp = static_cast<DWORD*>(sbits);

    int bx0 = 0, by0 = 0, bx1 = -1, by1 = -1;
    for (int fh = textH * 2; fh >= 6; fh--) {
        HFONT f = CreateFontW(-fh, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HGDIOBJ oldF = SelectObject(dc, f);
        SetTextCharacterExtra(dc, text.size() >= 3 ? -1 : 0);
        memset(sbits, 0, (size_t)W * H * 4);
        TextOutW(dc, 2, 0, text.c_str(), (int)text.size());
        GdiFlush();
        SelectObject(dc, oldF);
        DeleteObject(f);
        bx0 = W; by0 = H; bx1 = -1; by1 = -1;
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                if (sp[y * W + x] & 0xFF) { bx0 = std::min(bx0, x); bx1 = std::max(bx1, x); by0 = std::min(by0, y); by1 = std::max(by1, y); }
        if (bx1 < 0) break;
        if (bx1 - bx0 + 1 <= S && by1 - by0 + 1 <= textH) break;
    }

    // final icon bitmap
    bi.bmiHeader.biWidth = S;
    bi.bmiHeader.biHeight = -S;
    void* ibits = nullptr;
    HBITMAP color = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &ibits, nullptr, 0);
    auto* ip = static_cast<DWORD*>(ibits);
    memset(ip, 0, (size_t)S * S * 4);
    const int alphaScale = dim ? 140 : 255;
    auto put = [&](int x, int y, Rgb c, int a) {
        if (x < 0 || y < 0 || x >= S || y >= S || a <= 0) return;
        a = a * alphaScale / 255;
        ip[y * S + x] = ((DWORD)a << 24) | ((DWORD)c.r << 16) | ((DWORD)c.g << 8) | c.b;  // straight alpha
    };
    if (bx1 >= 0) {
        int gw = bx1 - bx0 + 1, gh = by1 - by0 + 1;
        int ox = (S - gw) / 2, oy = (textH - gh) / 2;
        for (int y = 0; y < gh; y++)
            for (int x = 0; x < gw; x++)
                put(ox + x, oy + y, textColor, sp[(by0 + y) * W + (bx0 + x)] & 0xFF);
    }
    // level bar: faint track + filled part
    int fill = barPct < 0 ? 0 : std::max(barPct > 0 ? 1 : 0, (barPct * S + 50) / 100);
    for (int y = S - barH; y < S; y++)
        for (int x = 0; x < S; x++) {
            if (x < fill) put(x, y, barColor, 255);
            else put(x, y, g_lightTaskbar ? Rgb{ 0, 0, 0 } : Rgb{ 255, 255, 255 }, 70);
        }

    SelectObject(dc, oldBmp);
    DeleteObject(scratch);
    DeleteDC(dc);

    HBITMAP mask = CreateBitmap(S, S, 1, 1, nullptr);
    {
        HDC mdc = CreateCompatibleDC(nullptr);
        HGDIOBJ o = SelectObject(mdc, mask);
        PatBlt(mdc, 0, 0, S, S, BLACKNESS);
        SelectObject(mdc, o);
        DeleteDC(mdc);
    }
    ICONINFO ii{ TRUE, 0, 0, mask, color };
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

Rgb Foreground() { return g_lightTaskbar ? Rgb{ 0, 0, 0 } : Rgb{ 255, 255, 255 }; }

HICON IconFor(int pct, bool charging, bool dim) {
    Rgb fg = Foreground();
    Rgb bar = charging        ? Rgb{ 0, 170, 255 }
            : pct <= ModConfig::warning.load() ? Rgb{ 232, 32, 48 }
            : pct <= 30       ? Rgb{ 255, 150, 0 }
                              : Rgb{ 30, 190, 90 };
    Rgb text = (!charging && pct >= 0 && pct <= ModConfig::warning.load()) ? Rgb{ 240, 50, 60 } : fg;
    std::wstring t = pct < 0 ? L"?" : std::to_wstring(pct);
    return RenderIcon(t, pct, text, bar, dim);
}

// ---- tray management --------------------------------------------------------------------------

void CopyTip(wchar_t* dst, size_t cap, const std::wstring& s) {
    wcsncpy(dst, s.c_str(), cap - 1);
    dst[cap - 1] = 0;
}

std::wstring StatusText(const TrayEntry& e, bool details = true) {
    const auto& s = e.st;
    if (!s.connected) return L"Off / disconnected";
    std::wstring t;
    if (!s.levelText.empty() && s.percent >= 0) t = s.levelText;
    else if (s.percent >= 0) t = std::to_wstring(s.percent) + L"%";
    else if (e.lastPct >= 0) t = std::to_wstring(e.lastPct) + L"% (last known)";
    else t = L"Unknown";
    if (s.charging) t += L", charging";
    if (s.asleep) t += L", asleep";
    if (details && !s.detailText.empty()) t += L"\n" + s.detailText;
    return t;
}

void TrayCall(DWORD msg, UINT uid, HICON icon, const std::wstring& tip, const wchar_t* infoTitle = nullptr,
              const std::wstring& info = L"") {
    NOTIFYICONDATAW nid{ sizeof(nid) };
    nid.hWnd = g_hwnd;
    nid.uID = uid;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = icon;
    CopyTip(nid.szTip, ARRAYSIZE(nid.szTip), tip);
    if (infoTitle) {
        nid.uFlags |= NIF_INFO;
        CopyTip(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), infoTitle);
        CopyTip(nid.szInfo, ARRAYSIZE(nid.szInfo), info);
        nid.dwInfoFlags = NIIF_WARNING;
    }
    Shell_NotifyIconW(msg, &nid);
    if (msg == NIM_ADD) {
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
    }
}

void TrayDelete(UINT uid) {
    NOTIFYICONDATAW nid{ sizeof(nid) };
    nid.hWnd = g_hwnd;
    nid.uID = uid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void UpdatePlaceholder() {
    if (g_useWidget) {
        if (g_placeholder) { TrayDelete(PLACEHOLDER_ID); g_placeholder = false; }
        return;
    }
    bool any = false;
    for (auto& [k, e] : g_entries) any |= e.added;
    if (!any && !g_placeholder) {
        if (!g_placeholderIcon) g_placeholderIcon = RenderIcon(L"-", -1, Foreground(), Foreground(), true);
        TrayCall(NIM_ADD, PLACEHOLDER_ID, g_placeholderIcon, L"Taskbar Wireless Battery Widget\nNo readable battery devices found");
        g_placeholder = true;
    } else if (any && g_placeholder) {
        TrayDelete(PLACEHOLDER_ID);
        g_placeholder = false;
    }
}

void ApplyEntry(TrayEntry& e) {
    const auto& s = e.st;
    bool visible = s.connected && !g_useWidget && !IsEntryHidden(e);
    if (!visible) {
        if (e.added) { TrayDelete(e.uid); e.added = false; }
        return;
    }
    int shown = s.percent >= 0 ? s.percent : e.lastPct;
    bool dim = s.percent < 0;
    std::wstring sig = std::to_wstring(shown) + (s.charging ? L"c" : L"") + (dim ? L"d" : L"") + (g_lightTaskbar ? L"l" : L"k");
    if (sig != e.iconSig || !e.icon) {
        if (e.icon) DestroyIcon(e.icon);
        e.icon = IconFor(shown, s.charging, dim);
        e.iconSig = sig;
    }
    std::wstring tip = s.name + L"\n" + StatusText(e);

    const wchar_t* infoTitle = nullptr;
    std::wstring info;
    if (s.charging || shown > ModConfig::warning.load() + 10) e.lowWarned = false;
    if (!s.charging && s.percent >= 0 && s.percent <= ModConfig::warning.load() && !e.lowWarned) {
        e.lowWarned = true;
        infoTitle = L"Low battery";
        info = s.name + L" battery remaining: " +
            (s.levelText.empty() ? std::to_wstring(s.percent) + L"%" : s.levelText);
    }
    TrayCall(e.added ? NIM_MODIFY : NIM_ADD, e.uid, e.icon, tip, infoTitle, info);
    e.added = true;
}

int KindOrder(DeviceKind k) {  // headset first, then mouse, ... (matches the left-to-right layout)
    switch (k) {
        case DeviceKind::Headset:  return 0;
        case DeviceKind::Mouse:    return 1;
        case DeviceKind::Keyboard: return 2;
        case DeviceKind::Gamepad:  return 3;
        case DeviceKind::Microphone: return 4;
        case DeviceKind::Phone:    return 5;
        default:                   return 6;
    }
}

void UpdateWidget() {
    if (!g_receiver) return;
    using namespace WidgetIpc;
    auto payload = std::make_unique<Payload>();
    payload->visible = g_useWidget;
    payload->fontSize = ModConfig::fontSize.load();
    payload->gap = ModConfig::gap.load();
    if (g_useWidget) {
        std::vector<const TrayEntry*> list;
        for (auto& [k, e] : g_entries)
            if (e.st.connected && !IsEntryHidden(e)) list.push_back(&e);
        std::sort(list.begin(), list.end(), [](const TrayEntry* a, const TrayEntry* b) {
            int ka = KindOrder(a->st.kind), kb = KindOrder(b->st.kind);
            return ka != kb ? ka < kb : a->st.name < b->st.name;
        });
        for (auto* e : list) {
            if (payload->count == ARRAYSIZE(payload->items)) break;
            Item& it = payload->items[payload->count++];
            it.kind = (INT32)e->st.kind;
            it.percent = e->st.percent >= 0 ? e->st.percent : e->lastPct;
            if (e->st.percent >= 0) CopyTip(it.text, ARRAYSIZE(it.text), e->st.levelText);
            bool low = !e->st.charging && it.percent >= 0 && it.percent <= ModConfig::warning.load();
            it.flags = (e->st.charging ? kCharging : 0) | (e->st.percent < 0 ? kDim : 0) | (low ? kLow : 0) |
                       (e->st.via == L"Bluetooth" ? kBluetooth : 0);
            CopyTip(it.tooltip, ARRAYSIZE(it.tooltip), e->st.name + L": " + StatusText(*e));
        }
    }
    COPYDATASTRUCT cds{ kCopyDataId, (DWORD)(offsetof(Payload, items) + payload->count * sizeof(Item)), payload.get() };
    // A receiver that is busy, e.g. while Explorer starts, gets the content again on the next Reconcile.
    if (!SendMessageTimeoutW(g_receiver, WM_COPYDATA, (WPARAM)g_hwnd, (LPARAM)&cds, SMTO_ABORTIFHUNG, 1000, nullptr))
        g_receiver = nullptr;
}

void OnResults(std::vector<DeviceStatus>& results) {
    std::map<std::wstring, bool> seen;
    for (auto& s : results) {
        auto& e = g_entries[s.key];
        if (!e.uid) e.uid = StableId(s.key);
        e.st = s;
        if (s.percent >= 0) e.lastPct = s.percent;
        e.lastSeen = GetTickCount64();
        seen[s.key] = true;
        ApplyEntry(e);
    }
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        auto& e = it->second;
        // Some receivers drop sleeping devices from the link; keep showing the last value for a while.
        bool inGrace = !seen.count(it->first) && e.st.graceSec > 0 && e.st.connected &&
                       GetTickCount64() - e.lastSeen < (ULONGLONG)e.st.graceSec * 1000;
        if (inGrace) {
            e.st.percent = -1;
            e.st.asleep = true;
            ApplyEntry(e);
        }
        if (!seen.count(it->first) && !inGrace) {
            if (it->second.added) TrayDelete(it->second.uid);
            if (it->second.icon) DestroyIcon(it->second.icon);
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }
    UpdatePlaceholder();
    UpdateWidget();
}

// Makes the actual display match the chosen mode; falls back to tray icons while Explorer has no
// taskbar widget attached (Windows 10, Explorer not ready yet). Explorer can take a while to attach
// it, also after it restarts, so the tray icons only appear after a grace period. Called once a second.
void Reconcile() {
    HWND receiver = FindWindowExW(HWND_MESSAGE, nullptr, WidgetIpc::kReceiverClass, nullptr);
    ULONGLONG now = GetTickCount64();
    if (receiver) g_receiverSeen = now;
    bool use = g_wantWidget && now - g_receiverSeen < WIDGET_GRACE_MS;
    bool changed = use != g_useWidget;
    g_useWidget = use;
    if (changed) {
        for (auto& [k, e] : g_entries) ApplyEntry(e);
        UpdatePlaceholder();
    }
    if (changed || receiver != g_receiver) {
        g_receiver = receiver;
        UpdateWidget();
    }
}

void ReAddAll() {
    g_placeholder = false;
    for (auto& [k, e] : g_entries) { e.added = false; ApplyEntry(e); }
    UpdatePlaceholder();
}

void RedrawAll() {
    if (g_placeholderIcon) {
        DestroyIcon(g_placeholderIcon);
        g_placeholderIcon = nullptr;
        if (g_placeholder) { TrayDelete(PLACEHOLDER_ID); g_placeholder = false; }
    }
    for (auto& [k, e] : g_entries) { e.iconSig.clear(); ApplyEntry(e); }
    UpdatePlaceholder();
}

void ShowMenu() {
    HMENU m = CreatePopupMenu();
    std::vector<std::wstring> keys;  // menu id ID_DEVICE_FIRST + i -> device key
    if (g_entries.empty()) AppendMenuW(m, MF_STRING | MF_GRAYED, 0, L"No readable battery devices found");
    else AppendMenuW(m, MF_STRING | MF_GRAYED, 0, L"Click a device to show or hide it");
    for (auto& [k, e] : g_entries) {
        std::wstring line = e.st.name + L":  " + StatusText(e, false) + L"   [" + e.st.via + L"]";
        AppendMenuW(m, MF_STRING | (IsHidden(k) ? 0 : MF_CHECKED), ID_DEVICE_FIRST + (UINT)keys.size(), line.c_str());
        keys.push_back(k);
    }
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, ID_REFRESH, L"Refresh now");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | MF_GRAYED, 0, L"Configure display and enable/disable in Windhawk");
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hwnd);
    UINT cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
    switch (cmd) {
        case ID_REFRESH: SetEvent(g_refreshEvt); break;
        default:
            if (cmd >= ID_DEVICE_FIRST && cmd < ID_DEVICE_FIRST + keys.size()) {
                const std::wstring& key = keys[cmd - ID_DEVICE_FIRST];
                if (!g_hidden.erase(key)) g_hidden.insert(key);
                SaveHidden();
                auto it = g_entries.find(key);
                if (it != g_entries.end()) ApplyEntry(it->second);
                UpdatePlaceholder();
                UpdateWidget();
            }
    }
}

// ---- worker thread ----------------------------------------------------------------------------

DWORD WINAPI Worker(LPVOID) {
    auto devices = PollAllDevices(g_stopEvt);
    ULONGLONG nextPoll = GetTickCount64() + (DWORD)ModConfig::pollMs.load();
    for (;;) {
        auto* v = new std::vector<DeviceStatus>(devices);
        if (!PostMessageW(g_hwnd, WM_RESULTS, 0, (LPARAM)v)) delete v;
        HANDLE hs[3] = { g_stopEvt, g_refreshEvt, g_airpodsEvt };
        const auto now = GetTickCount64();
        DWORD reason = WaitForMultipleObjects(3, hs, FALSE, now < nextPoll ? (DWORD)(nextPoll - now) : 0);
        if (reason == WAIT_OBJECT_0 || reason == WAIT_FAILED) { StopAirPods(); return 0; }
        if (reason == WAIT_OBJECT_0 + 2) {
            RefreshAirPods(devices); // BLE reception must not increase polling of other vendors
        } else {
            devices = PollAllDevices(g_stopEvt);
            nextPoll = GetTickCount64() + (DWORD)ModConfig::pollMs.load();
        }
    }
}

void RefreshSoon(UINT ms) { SetTimer(g_hwnd, TIMER_DEBOUNCE, ms, nullptr); }

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_taskbarCreatedMsg && msg != 0) {  // Explorer restarted
        ReAddAll();
        Reconcile();
        return 0;
    }
    switch (msg) {
        case WM_APP + 80:
            g_hideLastKnown = ModConfig::hideSleeping.load();
            g_wantWidget = ModConfig::taskbar.load();
            RedrawAll(); Reconcile(); UpdateWidget();
            SetEvent(g_refreshEvt);
            return 0;
        case WM_RESULTS: {
            auto* v = reinterpret_cast<std::vector<DeviceStatus>*>(lp);
            OnResults(*v);
            delete v;
            return 0;
        }
        case WM_TRAY:
            switch (LOWORD(lp)) {
                case WM_CONTEXTMENU:
                case NIN_SELECT:
                case NIN_KEYSELECT: ShowMenu(); break;
            }
            return 0;
        case WM_WIDGET_CLICK:
            ShowMenu();
            return 0;
        case WidgetIpc::kResendMessage:
            g_receiver = nullptr;
            Reconcile();
            return 0;
        case WM_ENDSESSION:  // sign-out / shutdown / installer restart manager
            if (wp) DestroyWindow(h);
            return 0;
        case WM_DEVICECHANGE:
            if (wp == DBT_DEVICEARRIVAL || wp == DBT_DEVICEREMOVECOMPLETE) RefreshSoon(2500);
            return TRUE;
        case WM_POWERBROADCAST:
            if (wp == PBT_APMRESUMEAUTOMATIC) RefreshSoon(5000);
            return TRUE;
        case WM_TIMER:
            if (wp == TIMER_DEBOUNCE) { KillTimer(h, TIMER_DEBOUNCE); SetEvent(g_refreshEvt); }
            if (wp == TIMER_WIDGET) Reconcile();
            return 0;
        case WM_SETTINGCHANGE:
            if (lp && !lstrcmpW((LPCWSTR)lp, L"ImmersiveColorSet")) {
                bool light = ReadLightTaskbar();
                if (light != g_lightTaskbar) { g_lightTaskbar = light; RedrawAll(); }
            }
            return 0;
        case WM_DESTROY:
            KillTimer(h, TIMER_WIDGET);
            g_useWidget = false;
            UpdateWidget();
            SetEvent(g_stopEvt);
            // A driver can be stuck in a synchronous HID call; the tool process exits right after uninit.
            if (g_worker) WaitForSingleObject(g_worker, 5000);
            for (auto& [k, e] : g_entries) {
                if (e.added) TrayDelete(e.uid);
                if (e.icon) DestroyIcon(e.icon);
            }
            if (g_placeholder) TrayDelete(PLACEHOLDER_ID);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

}  // namespace

int Run(HINSTANCE inst) {
    // Held so that the standalone apps and the mod never query the devices at the same time.
    std::vector<HANDLE> mutexes;
    auto closeMutexes = [&] {
        for (HANDLE m : mutexes) { ReleaseMutex(m); CloseHandle(m); }
    };
    for (const wchar_t* name : ModHost::kStandaloneMutexes) {
        HANDLE m = CreateMutexW(nullptr, TRUE, name);
        if (!m) { closeMutexes(); return 1; }
        mutexes.push_back(m);
        if (GetLastError() == ERROR_ALREADY_EXISTS) { closeMutexes(); return 0; }
    }

    g_lightTaskbar = ReadLightTaskbar();
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.lpszClassName = WidgetIpc::kToolWindowClass;
    RegisterClassExW(&wc);
    // A hidden top-level window (not message-only) so it receives the TaskbarCreated broadcast.
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, kAppName, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (!g_hwnd) { closeMutexes(); return 1; }
    // Explorer posts the widget clicks, also when Windhawk runs elevated.
    ChangeWindowMessageFilterEx(g_hwnd, WM_WIDGET_CLICK, MSGFLT_ALLOW, nullptr);
    ModHost::window = g_hwnd;
    if (ModHost::stopRequested) PostMessageW(g_hwnd, WM_CLOSE, 0, 0);

    DEV_BROADCAST_DEVICEINTERFACE_W filter{ sizeof(filter) };
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_DEVINTERFACE_HID;
    HDEVNOTIFY notify = RegisterDeviceNotificationW(g_hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    g_refreshEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_stopEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_airpodsEvt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    SetAirPodsRefreshEvent(g_airpodsEvt);
    g_wantWidget = LoadWidgetMode();
    g_hideLastKnown = LoadHideLastKnown();
    LoadHidden();
    g_receiverSeen = GetTickCount64();
    Reconcile();
    UpdatePlaceholder();  // something to click on until the first poll finishes
    SetTimer(g_hwnd, TIMER_WIDGET, 1000, nullptr);
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    // drain results posted after shutdown started
    while (PeekMessageW(&m, nullptr, WM_RESULTS, WM_RESULTS, PM_REMOVE)) delete reinterpret_cast<std::vector<DeviceStatus>*>(m.lParam);
    if (notify) UnregisterDeviceNotification(notify);
    closeMutexes();
    // A worker still stuck in a driver call keeps using the events until the process exits.
    if (!g_worker || WaitForSingleObject(g_worker, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_refreshEvt);
        CloseHandle(g_stopEvt);
        CloseHandle(g_airpodsEvt);
    }
    if (g_worker) CloseHandle(g_worker);
    return 0;
}

}

// The UI and device polling are kept on owned threads. Windhawk only controls lifetime/settings.
namespace ModHost {
HANDLE uiThread = nullptr;
DWORD uiThreadId = 0;
std::atomic<bool> stopRequested{false};
std::atomic<HWND> window{nullptr};
HMODULE module = nullptr;

DWORD WINAPI Run(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    int result = wb_app::Run(module);
    window = nullptr;
    UnregisterClassW(WidgetIpc::kToolWindowClass, module);
    Wh_Log(L"Battery UI thread exited, result=%d", result);
    return 0;
}
}

BOOL WhTool_ModInit() {
    for (PCWSTR name : ModHost::kStandaloneMutexes) {
        HANDLE existing = OpenMutexW(SYNCHRONIZE, FALSE, name);
        if (existing) {
            CloseHandle(existing);
            Wh_Log(L"Exit the standalone BatteryHub or Wireless Battery app before enabling this mod.");
            return FALSE;
        }
    }
    ModConfig::Load();
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModHost::Run), &ModHost::module)) return FALSE;
    ModHost::stopRequested = false;
    ModHost::uiThread = CreateThread(nullptr, 0, ModHost::Run, nullptr, 0, &ModHost::uiThreadId);
    if (!ModHost::uiThread) return FALSE;
    Wh_Log(L"Taskbar Wireless Battery Widget started, host PID=%lu", GetCurrentProcessId());
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    ModConfig::Load();
    if (HWND window = ModHost::window.load()) PostMessageW(window, WM_APP + 80, 0, 0);
}

void WhTool_ModUninit() {
    if (ModHost::uiThread) {
        ModHost::stopRequested = true;
        if (HWND window = ModHost::window.load()) PostMessageW(window, WM_CLOSE, 0, 0);
        // The UI thread waits up to 5 seconds for the worker; the process exits right after uninit.
        WaitForSingleObject(ModHost::uiThread, 10000);
        CloseHandle(ModHost::uiThread); ModHost::uiThread = nullptr;
    }
}

// ---- Explorer: the taskbar widget ------------------------------------------------------------------
// On the primary Windows 11 taskbar, the widget is a button in a column added at the left of the
// notification area (SystemTrayFrameGrid), so that the tray and the task buttons make room for it.
// The XAML is only accessed on the taskbar's thread: in messages to the subclassed taskbar window, and
// in the receiver window, which is created on that thread. The taskbar's XAML root is found as in
// "Multirow taskbar for Windows 11" by m417z.
namespace wb_taskbar {
namespace {

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using winrt::Windows::UI::Color;
using winrt::Windows::UI::Xaml::Media::SolidColorBrush;
using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;

void* CTaskBand_ITaskListWndSite_vftable;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
void* TaskbarHost_FrameHeight_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;
size_t g_taskbarElementOffset;  // of the taskbar element in TaskbarHost

HMODULE g_module, g_taskbarDll;
UINT g_taskbarMsg;  // to the subclassed taskbar window, wParam = TaskbarCommand
enum TaskbarCommand : WPARAM { kAttach, kDetach };
std::atomic<HWND> g_taskbar;
std::atomic<bool> g_unloading;
HANDLE g_stopEvent, g_lostEvent, g_watcher;

// Taskbar thread only.
struct ItemView {
    StackPanel panel{ nullptr };
    FontIcon icon{ nullptr };
    TextBlock text{ nullptr };
    FontIcon bolt{ nullptr };
    ToolTip tip{ nullptr };
};
Panel g_tray{ nullptr };  // SystemTrayFrameGrid
ColumnDefinition g_column{ nullptr };
Button g_button{ nullptr };
StackPanel g_itemsPanel{ nullptr };
std::vector<ItemView> g_views;
winrt::event_token g_clickToken, g_rightTappedToken, g_unloadedToken, g_themeToken;
HWND g_receiver;
HWND g_tool;  // the sender of the last payload
std::unique_ptr<WidgetIpc::Payload> g_payload;
UINT g_retryMs = 1000;
UINT_PTR RetryTimerId() { return (UINT_PTR)&g_retryMs; }  // distinct from Explorer's own timer ids
constexpr UINT_PTR kToolCheckTimer = 1;

const wchar_t* const kBolt = L"\uE945";            // LightningBolt
const wchar_t* const kBatteryUnknown = L"\uE996";  // shown when there is nothing to display

const wchar_t* Glyph(DeviceKind k, bool bluetooth) {
    switch (k) {
        case DeviceKind::Headset:  return L"\uE7F6";  // Headphone
        case DeviceKind::Mouse:    return L"\uE962";  // Mouse
        case DeviceKind::Keyboard: return L"\uE765";  // KeyboardClassic
        case DeviceKind::Gamepad:  return L"\uE7FC";  // Game
        case DeviceKind::Phone:    return L"\uE8EA";  // CellPhone
        case DeviceKind::Microphone: return L"\uE720";  // Microphone
        default:                   return bluetooth ? L"\uE702" : L"\uE772";  // Bluetooth / Devices
    }
}

bool FindTaskbarElementOffset(size_t* offset) {
#if defined(_M_X64)
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        *offset = b[7];
        return true;
    }
#elif defined(_M_ARM64)
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
        (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        *offset = (p[3] >> 12) & 0xFF;
        return true;
    }
#endif
    return false;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbar) {
    HWND taskSw = (HWND)GetPropW(taskbar, L"TaskbandHWND");
    void* taskBand = taskSw ? (void*)GetWindowLongPtrW(taskSw, 0) : nullptr;
    if (!taskBand) return nullptr;
    void* site = taskBand;
    for (int i = 0; *(void**)site != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        site = (void**)site + 1;
    }
    void* host[2]{};  // std::shared_ptr<TaskbarHost>
    CTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[1]) return nullptr;
    struct Release {
        void* control;
        ~Release() { std__Ref_count_base__Decref_Original(control); }
    } release{ host[1] };
    auto* unknown = host[0] ? *(::IUnknown**)((BYTE*)host[0] + g_taskbarElementOffset) : nullptr;
    FrameworkElement element{ nullptr };
    if (unknown) unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    return element ? element.XamlRoot() : nullptr;
}

FrameworkElement FindChildByName(DependencyObject parent, const wchar_t* name, int depth = 24) {
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i);
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name) return element;
        if (depth > 1)
            if (auto found = FindChildByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

SolidColorBrush MakeBrush(BYTE a, BYTE r, BYTE g, BYTE b) { return SolidColorBrush(Color{ a, r, g, b }); }

// The default button style, with the transparent background and the subtle hover of the tray buttons.
void ApplyButtonBrushes(bool light) {
    auto resources = g_button.Resources();
    auto put = [&](const wchar_t* key, BYTE alpha) {
        resources.Insert(winrt::box_value(winrt::hstring(key)),
                         light ? MakeBrush(alpha, 0, 0, 0) : MakeBrush(alpha, 255, 255, 255));
    };
    put(L"ButtonBackground", 0);
    put(L"ButtonBackgroundPointerOver", light ? 20 : 26);
    put(L"ButtonBackgroundPressed", light ? 12 : 18);
    put(L"ButtonBorderBrush", 0);
    put(L"ButtonBorderBrushPointerOver", 0);
    put(L"ButtonBorderBrushPressed", 0);
    g_button.Background(MakeBrush(0, 0, 0, 0));
}

ItemView CreateItemView() {
    Media::FontFamily icons(L"Segoe Fluent Icons, Segoe MDL2 Assets");
    ItemView v;
    v.panel = StackPanel();
    v.panel.Orientation(Orientation::Horizontal);
    v.panel.VerticalAlignment(VerticalAlignment::Center);
    v.icon = FontIcon();
    v.icon.FontFamily(icons);
    v.icon.FontSize(16);
    v.text = TextBlock();
    v.text.Margin({ 5, 0, 0, 0 });
    v.text.VerticalAlignment(VerticalAlignment::Center);
    v.bolt = FontIcon();
    v.bolt.FontFamily(icons);
    v.bolt.FontSize(11);
    v.bolt.Glyph(kBolt);
    v.bolt.Margin({ 2, 0, 0, 0 });
    v.panel.Children().Append(v.icon);
    v.panel.Children().Append(v.text);
    v.panel.Children().Append(v.bolt);
    v.tip = ToolTip();
    v.tip.Placement(Primitives::PlacementMode::Top);
    ToolTipService::SetToolTip(v.panel, v.tip);
    return v;
}

void Render() {
    if (!g_button) return;
    try {
        using namespace WidgetIpc;
        const Payload* p = g_payload.get();
        if (!p || !p->visible) {
            g_button.Visibility(Visibility::Collapsed);
            return;
        }
        bool light = g_button.ActualTheme() == ElementTheme::Light;
        ApplyButtonBrushes(light);
        auto red = light ? MakeBrush(255, 196, 30, 40) : MakeBrush(255, 255, 90, 90);
        auto blue = light ? MakeBrush(255, 0, 120, 212) : MakeBrush(255, 96, 205, 255);
        size_t count = std::max<UINT32>(p->count, 1);  // without devices, a placeholder to click on
        while (g_views.size() < count) {
            g_views.push_back(CreateItemView());
            g_itemsPanel.Children().Append(g_views.back().panel);
        }
        while (g_views.size() > count) {
            g_itemsPanel.Children().RemoveAtEnd();
            g_views.pop_back();
        }
        g_itemsPanel.Spacing(p->gap);
        std::wstring summary;
        if (!p->count) {
            ItemView& v = g_views[0];
            v.icon.Glyph(kBatteryUnknown);
            v.text.Visibility(Visibility::Collapsed);
            v.bolt.Visibility(Visibility::Collapsed);
            v.panel.Opacity(110.0 / 255);
            summary = L"Taskbar Wireless Battery Widget: no readable battery devices found";
            v.tip.Content(winrt::box_value(winrt::hstring(summary)));
        }
        for (UINT32 i = 0; i < p->count; i++) {
            ItemView& v = g_views[i];
            const Item& it = p->items[i];
            v.icon.Glyph(Glyph((DeviceKind)it.kind, it.flags & kBluetooth));
            v.text.Text(it.text[0] ? std::wstring(it.text) : it.percent >= 0 ? std::to_wstring(it.percent) + L"%" : L"?");
            v.text.FontSize(p->fontSize);
            v.text.Visibility(Visibility::Visible);
            if (it.flags & kLow) v.text.Foreground(red);
            else v.text.ClearValue(TextBlock::ForegroundProperty());
            v.bolt.Foreground(blue);
            v.bolt.Visibility(it.flags & kCharging ? Visibility::Visible : Visibility::Collapsed);
            v.panel.Opacity(it.flags & kDim ? 120.0 / 255 : 1.0);
            v.tip.Content(winrt::box_value(winrt::hstring(it.tooltip)));
            if (!summary.empty()) summary += L"\n";
            summary += it.tooltip;
        }
        Automation::AutomationProperties::SetName(g_button, summary);
        g_button.Visibility(Visibility::Visible);
    } catch (...) {
        Wh_Log(L"Updating the taskbar widget failed: %08X", (unsigned)winrt::to_hresult());
    }
}

bool StorePayload(const COPYDATASTRUCT* cds) {
    using WidgetIpc::Payload;
    constexpr size_t header = offsetof(Payload, items);
    if (cds->dwData != WidgetIpc::kCopyDataId || cds->cbData < header || cds->cbData > sizeof(Payload)) return false;
    auto p = std::make_unique<Payload>();
    memcpy(p.get(), cds->lpData, cds->cbData);
    if (p->count > ARRAYSIZE(p->items) || header + p->count * sizeof(WidgetIpc::Item) > cds->cbData) return false;
    for (UINT32 i = 0; i < p->count; i++) {
        p->items[i].text[ARRAYSIZE(p->items[i].text) - 1] = 0;
        p->items[i].tooltip[ARRAYSIZE(p->items[i].tooltip) - 1] = 0;
    }
    g_payload = std::move(p);
    return true;
}

LRESULT CALLBACK ReceiverProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COPYDATA:
            if (!StorePayload((const COPYDATASTRUCT*)lp)) return FALSE;
            g_tool = (HWND)wp;
            Render();
            return TRUE;
        case WM_TIMER:
            if (wp != kToolCheckTimer) return 0;
            // Without the polling process, e.g. after it crashed, its last readings would go stale.
            if (g_tool && !IsWindow(g_tool)) {
                g_tool = nullptr;
                g_payload.reset();
                Render();
            }
            if (!g_payload) {
                if (HWND tool = FindWindowW(WidgetIpc::kToolWindowClass, nullptr)) PostMessageW(tool, WidgetIpc::kResendMessage, 0, 0);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// The menu is shown by the polling process, which gets the right to take the foreground for it.
void NotifyTool() {
    HWND tool = g_tool ? g_tool : FindWindowW(WidgetIpc::kToolWindowClass, nullptr);
    DWORD pid = 0;
    if (!tool || !GetWindowThreadProcessId(tool, &pid)) return;
    AllowSetForegroundWindow(pid);
    PostMessageW(tool, WidgetIpc::kClickMessage, 0, 0);
}

Button CreateButton() {
    Button button;
    button.Name(L"TaskbarWirelessBatteryMonitor");
    button.MinWidth(0);
    button.MinHeight(0);
    button.Padding({ 8, 0, 8, 0 });
    button.Margin({ 0, 4, 0, 4 });
    button.BorderThickness({ 0, 0, 0, 0 });
    button.CornerRadius({ 4, 4, 4, 4 });
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    g_itemsPanel = StackPanel();
    g_itemsPanel.Orientation(Orientation::Horizontal);
    button.Content(g_itemsPanel);
    g_clickToken = button.Click([](auto&&, auto&&) { NotifyTool(); });
    g_rightTappedToken = button.RightTapped([](auto&&, auto&& args) {
        args.Handled(true);  // instead of the taskbar's context menu
        NotifyTool();
    });
    // Explorer can recreate the taskbar's XAML.
    g_unloadedToken = button.Unloaded([](auto&&, auto&&) {
        HWND taskbar = g_taskbar.load();
        if (taskbar && !g_unloading) PostMessageW(taskbar, g_taskbarMsg, kAttach, 0);
    });
    g_themeToken = button.ActualThemeChanged([](auto&&, auto&&) { Render(); });
    return button;
}

void Detach() {
    if (g_receiver) {
        DestroyWindow(g_receiver);
        g_receiver = nullptr;
    }
    if (g_button) {
        try {
            g_button.Click(g_clickToken);
            g_button.RightTapped(g_rightTappedToken);
            g_button.Unloaded(g_unloadedToken);
            g_button.ActualThemeChanged(g_themeToken);
            uint32_t index = 0;
            if (g_tray.Children().IndexOf(g_button, index)) g_tray.Children().RemoveAt(index);
            auto grid = g_tray.try_as<Grid>();
            if (grid && g_column && grid.ColumnDefinitions().IndexOf(g_column, index)) {
                grid.ColumnDefinitions().RemoveAt(index);
                for (auto child : grid.Children()) {
                    auto element = child.try_as<FrameworkElement>();
                    if (element && Grid::GetColumn(element) > (int)index) Grid::SetColumn(element, Grid::GetColumn(element) - 1);
                }
            }
        } catch (...) {
            Wh_Log(L"Removing the taskbar widget failed: %08X", (unsigned)winrt::to_hresult());
        }
    }
    g_views.clear();
    g_itemsPanel = nullptr;
    g_button = nullptr;
    g_column = nullptr;
    g_tray = nullptr;
}

bool Attach(HWND taskbar) {
    auto root = GetTaskbarXamlRoot(taskbar);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    auto tray = content ? FindChildByName(content, L"SystemTrayFrameGrid").try_as<Panel>() : nullptr;
    uint32_t index = 0;
    if (g_button && tray && tray == g_tray && tray.Children().IndexOf(g_button, index)) return true;
    Detach();
    if (!tray) return false;
    auto grid = tray.try_as<Grid>();
    if (grid) {
        if (!grid.ColumnDefinitions().Size()) return false;
        for (auto child : grid.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (element) Grid::SetColumn(element, Grid::GetColumn(element) + 1);
        }
        g_column = ColumnDefinition();
        g_column.Width({ 1, GridUnitType::Auto });
        grid.ColumnDefinitions().InsertAt(0, g_column);
    } else if (!tray.try_as<StackPanel>()) {
        return false;
    }
    g_tray = tray;
    g_button = CreateButton();
    if (grid) {
        Grid::SetColumn(g_button, 0);
        tray.Children().Append(g_button);
    } else {
        tray.Children().InsertAt(0, g_button);
    }
    g_receiver = CreateWindowExW(0, WidgetIpc::kReceiverClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, g_module, nullptr);
    if (g_receiver) SetTimer(g_receiver, kToolCheckTimer, 2000, nullptr);
    Render();
    Wh_Log(L"Taskbar widget attached");
    return true;
}

void TryAttach(HWND taskbar) {
    bool attached = false;
    try {
        attached = Attach(taskbar);
    } catch (...) {
        Wh_Log(L"Attaching the taskbar widget failed: %08X", (unsigned)winrt::to_hresult());
        Detach();
    }
    if (attached) {
        g_retryMs = 1000;
        return;
    }
    // The taskbar's XAML is not ready yet, or this is not the Windows 11 taskbar.
    SetTimer(taskbar, RetryTimerId(), g_retryMs, nullptr);
    g_retryMs = std::min(g_retryMs * 2, 30000u);
}

LRESULT CALLBACK TaskbarSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == g_taskbarMsg) {
        if (wp == kDetach) {
            KillTimer(hwnd, RetryTimerId());
            Detach();
        } else if (!g_unloading) {
            TryAttach(hwnd);
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == RetryTimerId()) {
        KillTimer(hwnd, RetryTimerId());
        if (!g_unloading) TryAttach(hwnd);
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        KillTimer(hwnd, RetryTimerId());
        Detach();
        g_taskbar = nullptr;
        SetEvent(g_lostEvent);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

// Windhawk can load the mod before Explorer creates the taskbar window, and Explorer can recreate it.
DWORD WINAPI WatchTaskbar(LPVOID) {
    for (;;) {
        if (!g_taskbar) {
            HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
            DWORD pid = 0;
            if (taskbar && GetWindowThreadProcessId(taskbar, &pid) && pid != GetCurrentProcessId()) return 0;
            if (taskbar && WindhawkUtils::SetWindowSubclassFromAnyThread(taskbar, TaskbarSubclassProc, 0)) {
                g_taskbar = taskbar;
                PostMessageW(taskbar, g_taskbarMsg, kAttach, 0);
            }
        }
        HANDLE events[] = { g_stopEvent, g_lostEvent };
        DWORD wait = WaitForMultipleObjects(2, events, FALSE, g_taskbar ? INFINITE : 1000);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) return 0;
    }
}

}  // namespace

bool IsExplorer() {
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!n || n == ARRAYSIZE(path)) return false;
    const wchar_t* name = wcsrchr(path, L'\\');
    return !_wcsicmp(name ? name + 1 : path, L"explorer.exe");
}

BOOL Init() {
    // Other Explorer processes, e.g. with folder windows in a separate process, have no taskbar.
    if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(taskbar, &pid);
        if (pid != GetCurrentProcessId()) return FALSE;
    }
    g_taskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_taskbarDll) return FALSE;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        { { LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})" }, &CTaskBand_ITaskListWndSite_vftable },
        { { LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )" },
          &CTaskBand_GetTaskbarHost_Original },
        { { LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )" }, &TaskbarHost_FrameHeight_Original },
        { { LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))" }, &std__Ref_count_base__Decref_Original },
    };
    if (!WindhawkUtils::HookSymbols(g_taskbarDll, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)) ||
        !FindTaskbarElementOffset(&g_taskbarElementOffset)) {
        Wh_Log(L"Unsupported taskbar, tray icons are shown instead of the taskbar widget");
        FreeLibrary(g_taskbarDll);
        g_taskbarDll = nullptr;
        return FALSE;
    }
    return TRUE;
}

void AfterInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ReceiverProc), &g_module);
    g_taskbarMsg = RegisterWindowMessageW(L"TaskbarWirelessBatteryXaml_" WH_MOD_ID);
    WNDCLASSW wc{};
    wc.lpfnWndProc = ReceiverProc;
    wc.hInstance = g_module;
    wc.lpszClassName = WidgetIpc::kReceiverClass;
    RegisterClassW(&wc);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_lostEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stopEvent && g_lostEvent) g_watcher = CreateThread(nullptr, 0, WatchTaskbar, nullptr, 0, nullptr);
}

void Uninit() {
    g_unloading = true;
    if (g_watcher) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_watcher, INFINITE);
        CloseHandle(g_watcher);
    }
    if (HWND taskbar = g_taskbar.exchange(nullptr)) {
        SendMessageW(taskbar, g_taskbarMsg, kDetach, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(taskbar, TaskbarSubclassProc);
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
    if (g_lostEvent) CloseHandle(g_lostEvent);
    UnregisterClassW(WidgetIpc::kReceiverClass, g_module);
    if (g_taskbarDll) FreeLibrary(g_taskbarDll);
}

}  // namespace wb_taskbar

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.
//
// The mod is also loaded into Explorer, where the callbacks only show the taskbar widget.

bool g_isExplorer;
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    if (wb_taskbar::IsExplorer()) {
        g_isExplorer = true;
        return wb_taskbar::Init();
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorer) {
        wb_taskbar::AfterInit();
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher || g_isExplorer) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        wb_taskbar::Uninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
