// ==WindhawkMod==
// @id              taskbar-wireless-battery-monitor
// @name            Taskbar Wireless Battery Monitor
// @name:zh-CN      任务栏无线设备电量监控
// @description     Show wireless peripheral battery levels on the taskbar or in the system tray.
// @description:zh-CN 显示多品牌无线外设电量，支持任务栏和托盘显示，以及可选的 AirPods 附近广播接收。
// @version         0.1.3
// @author          yaoiyiXu
// @github          https://github.com/yaoiyiXu
// @homepage        https://github.com/yaoiyiXu/taskbar-wireless-battery-monitor
// @include         windhawk.exe
// @license         GPL-3.0-or-later
// @compilerOptions -lhid -lsetupapi -lcfgmgr32 -lole32 -lcomctl32 -lgdi32 -lruntimeobject -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Wireless Battery Monitor

Display battery levels for supported wireless mice, keyboards, headsets and
controllers on the primary taskbar, or as one system tray icon per device.
Click the battery area to refresh or show/hide individual devices. Hover for details.
Configure the display mode, refresh interval, low-battery threshold, font size and
device spacing in Windhawk. Windhawk manages startup and enabling/disabling the mod.
The interface uses Chinese on Chinese Windows installations and English otherwise.

This tool runs in a dedicated Windhawk process, without injecting into Explorer.
Exit BOTH the original Wireless Battery and this project's Chinese standalone
WirelessBattery.exe before enabling it. Both standalone mutex names are checked. If the display does
not start, exit that app and disable/re-enable the mod.

## Device support and limitations

- Includes the native protocol drivers from [Wireless Battery](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor)
  and its Chinese adaptation. Brand support does not mean every model is supported.
  External HeadsetControl integration is not included.
- Vendor-specific battery queries send HID output/feature reports, or serial
  commands for supported ATK headsets, to peripherals and read their replies.
  These community-derived protocols have not been verified on every supported
  model. The formerly broad Glorious/Hitscan/G-Wolves and Kysona matches now use
  explicit VID:PID lists; ZOWIE DW queries are limited to 04A5:800A.
  AJ179/AJ-series and Keychron vendor queries are excluded from this mod until
  exact PID/protocol pairings are verified. Standard battery fallbacks remain available.
  Queries can conflict with vendor utilities that access a device exclusively.
- Periodic queries may wake peripherals or prevent their lights from sleeping.
  Use the **Devices to exclude from queries** setting (hex VID:PID pairs such as
  28BD:0947,046D:C52B) to stop HID/ATK battery queries for a product on the next
  poll. This differs from hiding a device in the menu, which only hides its display.
  Malformed exclusions pause HID/ATK queries until corrected. Digitizer products
  are excluded automatically. Identification metadata may still be enumerated.
- Bluetooth PlayStation controllers are only read passively. Full battery reports
  must already be enabled by another application; this mod never changes their mode.
- Standard Bluetooth battery readings depend on what Windows exposes.
- Only the primary, horizontal taskbar is supported. On Windows 11 a crowded or
  left-aligned taskbar can overlap the widget; Windows 10 taskbar resizing can
  flicker. Switch to tray mode if the taskbar layout is unsuitable.
- In taskbar mode low battery is indicated by red text; notification balloons
  are available in tray mode. Sleep/last-known readings are dimmed.
- Remaining drivers are skipped when stopping. If shutdown has not completed in
  five seconds (including a stuck synchronous HID call), the dedicated tool
  process exits; a live driver thread is never left executing in an unloaded DLL.

## Optional AirPods reception

AirPods BLE reception is **off by default**. Enable it in settings if needed.
It passively receives nearby Apple proximity broadcasts while enabled, without
changing pairing or audio settings. Battery levels are approximate, in steps
of about 10%; open the charging case near the computer to obtain case readings.

Broadcast ownership cannot be verified. Entries are labelled "nearby broadcast"
and may belong to someone else, especially when several pairs of the same model
are nearby. Readings older than 30 seconds are marked as historical at the next
refresh, and entries expire after about two minutes without a broadcast.
Raw Bluetooth addresses and encrypted payloads are not written to disk.

## Credits and license

GNU GPL v3.0 or later. Based on [Wireless Battery](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor)
and the [Chinese / G87 adaptation](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor/pull/43).
AirPods protocol reference: [SpriteOvO/AirPodsDesktop](https://github.com/SpriteOvO/AirPodsDesktop)
(GPL-3.0-or-later). The dedicated host follows the official Windhawk "Mods as tools" example.
[Source and full notices](https://github.com/yaoiyiXu/taskbar-wireless-battery-monitor).

## 中文说明

在主显示器任务栏或系统托盘显示受支持无线外设的电量。点击电量区域可刷新、
显示或隐藏设备，悬停可查看详情；显示模式、刷新周期等在 Windhawk 设置页调整。
中文 Windows 显示中文界面，其他语言系统默认显示英文。启用前请退出独立版
WirelessBattery.exe；启动和启停由 Windhawk 管理。

AirPods 广播接收默认关闭，可在设置中手动开启。电量约 10% 一档，充电盒读数
可能需要在电脑旁开盖。广播归属无法验证，可能显示附近其他人的耳机。
超过 30 秒未更新的数据在下一次刷新时标成历史，约两分钟无广播后移除。

厂商协议会向设备发送 HID 查询报文，部分 ATK 耳机使用串口命令。协议来自社区，
并未逐型号验证。原先宽泛的匹配已收紧；AJ179/AJ 系列和 Keychron 的厂商查询暂不包含，
等待验证精确型号 ID 与协议。查询可能唤醒设备或阻止灯光休眠，也可能与独占设备的
厂商软件冲突。支持某品牌不代表支持其全部型号，不包含外部 HeadsetControl 扩展。
可在“排除查询的设备”中填写十六进制 VID:PID，逗号分隔，下次轮询生效。
菜单隐藏仅影响显示。数位板自动排除；蓝牙 PlayStation 手柄不会被本模组切换模式。

仅支持主显示器的横向任务栏；Windows 11 拥挤或左对齐时可能重叠，Windows 10
调整任务按钮区域时可能闪烁，可切换托盘模式。任务栏模式通过红字提示低电量，
托盘模式另有气泡通知。关闭超过五秒时会退出模组的独立宿主进程，避免无限等待。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayMode: taskbar
  $name: Display mode
  $name:zh-CN: 显示模式
  $options:
  - taskbar: Taskbar icons and battery levels
  - tray: One tray icon per device
  $options:zh-CN:
  - taskbar: 任务栏图标与电量
  - tray: 每个设备一个托盘图标
- pollSeconds: 60
  $name: Device refresh interval (seconds, 10–600)
  $name:zh-CN: 常规设备刷新周期（秒，10～600）
- warningPercent: 15
  $name: Low-battery threshold (0–50)
  $name:zh-CN: 低电量阈值（0～50）
- fontSize: 13
  $name: Battery font size (10–24)
  $name:zh-CN: 电量字号（10～24）
- deviceGap: 14
  $name: Device spacing (4–40)
  $name:zh-CN: 设备间距（4～40）
- hideSleeping: false
  $name: Hide sleeping devices and last-known readings
  $name:zh-CN: 隐藏休眠或仅有历史电量的设备
- enableAirPods: false
  $name: Receive nearby AirPods battery broadcasts
  $name:zh-CN: 接收附近 AirPods 电量广播
  $description: Approximate readings; ownership is unverified and nearby people's AirPods may appear.
  $description:zh-CN: 电量为近似值，广播归属未验证，可能显示附近其他人的耳机。
- ignoreDevices: ""
  $name: Devices to exclude from queries (VID:PID)
  $name:zh-CN: 排除查询的设备（VID:PID）
  $description: "Comma-separated hex IDs, e.g. 28BD:0947,046D:C52B. Stops HID/ATK battery queries on the next poll. Hiding in the menu only hides the display."
  $description:zh-CN: "十六进制 ID，逗号分隔，例如 28BD:0947,046D:C52B。下次轮询停止 HID/ATK 电量查询；菜单隐藏仅影响显示。"
*/
// ==/WindhawkModSettings==

// Source modules and reproducible bundler: https://github.com/yaoiyiXu/taskbar-wireless-battery-monitor

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

// English is the fallback. Device protocol strings and keys are never translated.
namespace ui {
inline bool IsChinese() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE;
}

inline const wchar_t* Text(const wchar_t* english, const wchar_t* chinese) {
    return IsChinese() ? chinese : english;
}
}  // namespace ui

enum class DeviceKind { Other, Mouse, Headset, Keyboard, Gamepad };

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
    bool OpenFeatureOnly(const HidColl& c);  // zero access: enough for feature reports, works on mice/keyboards
    void Close();
    bool ok() const { return h_ != INVALID_HANDLE_VALUE; }
    const HidColl& coll() const { return c_; }

    bool Write(const std::vector<BYTE>& data, int timeoutMs = 500);
    bool Write(std::initializer_list<BYTE> data, int timeoutMs = 500) { return Write(std::vector<BYTE>(data), timeoutMs); }
    // Reads one input report into `buf` (resized to fit). Returns the byte count, or -1 on timeout/error.
    int Read(std::vector<BYTE>& buf, int timeoutMs);
    void Flush();  // discard queued input reports

    bool SetFeature(const std::vector<BYTE>& data);
    bool GetFeature(BYTE reportId, std::vector<BYTE>& out);  // out[0] = report ID

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

    bool IsClaimed(USHORT vid, USHORT pid) const { return claimed.count({ vid, pid }) != 0; }
    void Claim(USHORT vid, USHORT pid) { claimed.insert({ vid, pid }); }
    void Add(const DeviceStatus& s, USHORT vid, USHORT pid) {
        out.push_back(s);
        Claim(vid, pid);
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
void PollGlorious(PollContext& ctx);  // also Hitscan
void PollAula(PollContext& ctx);      // AULA F75 / F87 Pro / F99 wireless keyboards (before Kysona, same vendor id)
void PollMchose(PollContext& ctx);    // MCHOSE G87 41E4:2001, 2.4 GHz only
void PollAtkSerial(PollContext& ctx); // ATK N9 Pro headset (virtual COM port)
void PollLamzu(PollContext& ctx);     // LAMZU mice on vendor ids 373E / 37B0
void PollRongyuan(PollContext& ctx);  // Rongyuan receivers (Attack Shark, Ajazz keyboards, ...)
void PollSonyInzone(PollContext& ctx);  // Sony INZONE H9 II / H5
void PollRog(PollContext& ctx);       // ASUS ROG keyboards and mice
void PollZowie(PollContext& ctx);     // BenQ ZOWIE EC-CW / DW mice
void PollRapoo(PollContext& ctx);     // Rapoo VT gaming mice (passive battery reports)
void PollMiscMice(PollContext& ctx);  // Motospeed X6, Delux M900 Pro, Ajazz AJ179 and AJ159/AJ-series
void PollRazerHeadsets(PollContext& ctx);  // Razer BlackShark V3 / V3 Pro
void PollKysona(PollContext& ctx);    // also Pulsar, ATK / VXE, Hitscan Hyperlight
void PollHyperX(PollContext& ctx);
void PollLogitech(PollContext& ctx);
void PollRazer(PollContext& ctx);
void PollSteelSeries(PollContext& ctx);
void PollCorsair(PollContext& ctx);
void PollCoolerMaster(PollContext& ctx);
void PollPlayStation(PollContext& ctx);
void PollGameSir(PollContext& ctx);    // Nova Lite / Nova 2 / Nova 2 Lite
// Fallbacks.
void PollGenericHid(PollContext& ctx);
void PollBluetooth(PollContext& ctx);
void PollAirPods(PollContext& ctx);
void PollXInput(PollContext& ctx);

// A small transparent strip on the taskbar, just left of the notification area, showing
// "[icon] 30%  [icon] 93%". Implemented as a layered child window of the taskbar (Shell_TrayWnd).
struct WidgetItem {
    DeviceKind kind = DeviceKind::Other;
    int  percent  = -1;     // -1 = unknown
    std::wstring text;      // shown instead of the percentage when set (coarse levels like "Low")
    bool charging = false;
    bool dim      = false;  // asleep / stale value
    bool low      = false;
    bool bluetooth = false;  // picks the Bluetooth icon when the device type is unknown
};

namespace widget {
// Creates the widget on the primary taskbar. Clicks are reported by posting `clickMsg` to `notifyWnd`.
bool Create(HINSTANCE inst, HWND notifyWnd, UINT clickMsg, bool lightTheme);
void Destroy();
bool Alive();
void SetItems(const std::vector<WidgetItem>& items, const std::wstring& tooltip);
void SetLightTheme(bool light);
// Re-checks the taskbar layout and moves the widget if needed; call about once a second.
// Returns false when the taskbar it was attached to is gone.
bool Reposition();
}

// Translate at presentation boundaries; preserve protocol values and device keys.
namespace zh {
inline std::wstring Text(const std::wstring& value) {
    if (!ui::IsChinese()) return value;
    struct Entry { const wchar_t* source; const wchar_t* translated; };
    static const Entry entries[] = {
        { L"Empty", L"耗尽" },
        { L"Low", L"偏低" },
        { L"Medium", L"中等" },
        { L"Full", L"充足" },
        { L"Good", L"良好" },
        { L"Critical", L"极低" },
        { L"Bluetooth", L"蓝牙" },
        { L"Apple BLE", L"苹果蓝牙广播" },
        { L"USB cable", L"USB 有线" },
        { L"2.4 GHz receiver", L"2.4 GHz 接收器" },
        { L"USB / Bluetooth", L"USB / 蓝牙" },
        { L"Logitech receiver", L"Logitech 接收器" },
        { L"Razer receiver", L"Razer 接收器" },
        { L"Corsair receiver", L"Corsair 接收器" },
        { L"Wireless adapter", L"无线适配器" },
        { L"Xbox Wireless", L"Xbox 无线" },
        { L"Xbox Wireless / Bluetooth", L"Xbox 无线 / 蓝牙" },
        { L"ROG Omni Receiver", L"ROG Omni 接收器" },
        { L"2.4 GHz receiver / USB", L"2.4 GHz 接收器 / USB" },
        { L"UWB base station", L"UWB 基站" },
        { L"Wireless device", L"无线设备" },
        { L"Bluetooth device", L"蓝牙设备" },
        { L"Wireless mouse", L"无线鼠标" },
        { L"Wireless keyboard", L"无线键盘" },
        { L"Headset", L"耳机" },
        { L"Xbox controller", L"Xbox 手柄" },
        { L"Xbox headset", L"Xbox 耳机" },
        { L"AULA wireless keyboard", L"AULA 无线键盘" },
        { L"Corsair wireless headset", L"Corsair 无线耳机" },
        { L"Corsair wireless device", L"Corsair 无线设备" },
        { L"LAMZU wireless mouse", L"LAMZU 无线鼠标" },
        { L"Logitech wireless device", L"Logitech 无线设备" },
        { L"Rapoo wireless mouse", L"Rapoo 无线鼠标" },
        { L"Redragon wireless mouse", L"Redragon 无线鼠标" },
        { L"Keychron mouse", L"Keychron 鼠标" },
        { L"Ajazz mouse", L"Ajazz 鼠标" },
        { L"AJAZZ mouse", L"AJAZZ 鼠标" },
        { L"ZOWIE DW mouse", L"ZOWIE DW 鼠标" },
    };
    for (const auto& entry : entries)
        if (value == entry.source) return entry.translated;
    const std::wstring prefix = L"Xbox controller ";
    if (value.rfind(prefix, 0) == 0) return L"Xbox 手柄 " + value.substr(prefix.size());
    return value;
}
} // namespace zh

// Live, read-only BLE reception; no pairing, settings or audio changes.
void StartAirPods();
void StopAirPods();
void SetAirPodsRefreshEvent(void* event);
void RefreshAirPods(std::vector<DeviceStatus>& devices);

// Independent decoder of Apple proximity advertisements. Layout reference:
// SpriteOvO/AirPodsDesktop, Source/Core/AppleCP.h and AppleCP.cpp (GPL-3.0-or-later).
namespace airpods {
struct Battery {
    int left = -1, right = -1, box = -1;
    bool leftCharging = false, rightCharging = false, boxCharging = false;
    uint16_t model = 0;
};

inline const wchar_t* ModelName(uint16_t model) {
    switch (model) {
        case 0x0220: return L"AirPods 1";
        case 0x0F20: return L"AirPods 2";
        case 0x1320: return L"AirPods 3";
        case 0x1920: return L"AirPods 4";
        case 0x1B20: return L"AirPods 4 ANC";
        case 0x0E20: return L"AirPods Pro";
        case 0x1420: return L"AirPods Pro 2";
        case 0x2420: return L"AirPods Pro 2 USB-C";
        case 0x2720: return L"AirPods Pro 3";
        case 0x0A20: return L"AirPods Max";
        case 0x1F20: return L"AirPods Max USB-C";
        default: return nullptr;
    }
}

inline int Percent(unsigned nibble) { return nibble <= 10 ? int(nibble * 10) : -1; }

inline bool Decode(const uint8_t* data, size_t size, Battery& result) {
    if (!data || size != 27 || data[0] != 0x07 || data[1] != 0x19 || data[2] != 0x01) return false;
    Battery next;
    next.model = (uint16_t(data[3]) << 8) | data[4];
    if (!ModelName(next.model)) return false;
    // Low nibble and bit 4 describe the broadcasting bud; status bit 5 identifies its side.
    const bool fromLeft = (data[5] & 0x20) != 0;
    const int current = Percent(data[6] & 0x0F), other = Percent(data[6] >> 4);
    next.left = fromLeft ? current : other;
    next.right = fromLeft ? other : current;
    next.box = Percent(data[7] & 0x0F);
    next.leftCharging = (data[7] & (fromLeft ? 0x10 : 0x20)) != 0;
    next.rightCharging = (data[7] & (fromLeft ? 0x20 : 0x10)) != 0;
    next.boxCharging = (data[7] & 0x40) != 0;
    if (next.left < 0 && next.right < 0 && next.box < 0) return false;
    result = next;
    return true;
}

inline int EarPercent(const Battery& b) {
    if (b.left < 0) return b.right;
    if (b.right < 0) return b.left;
    return std::min(b.left, b.right);
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

// Upstream driver baseline: bbb5c75f4aaca0adc85fd56fcf377b78ea680f4f, plus PR #43 (MCHOSE).
// Safety backports: 99c55b211bc9ab1d40e9c863c74f59480e0039b6 (digitizers/ignore),
// 1e07289997d91cbb67d00a879e39f2a26130de49 (Razer/Synapse).
// This is a selective backport, NOT a full update to upstream main.
namespace ModSafety {
std::mutex ignoredMutex;
std::set<std::pair<USHORT, USHORT>> ignored;

void ConfigureIgnored(const std::wstring& text) {
    std::set<std::pair<USHORT, USHORT>> parsed;
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find_first_of(L",; \t\r\n", start);
        std::wstring token = text.substr(start, end == std::wstring::npos ? end : end - start);
        if (!token.empty()) {
            if (token.size() != 9 || token[4] != L':' ||
                token.find_first_not_of(L"0123456789abcdefABCDEF", 0) != 4 ||
                token.substr(5).find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos) {
                Wh_Log(L"Invalid ignored-device entry; polling paused until the setting is corrected.");
                // Fail closed rather than accidentally querying a mistyped exclusion.
                parsed.insert({0, 0});
            } else {
                parsed.insert({(USHORT)wcstoul(token.substr(0, 4).c_str(), nullptr, 16),
                               (USHORT)wcstoul(token.substr(5).c_str(), nullptr, 16)});
            }
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    std::lock_guard<std::mutex> lock(ignoredMutex);
    ignored = std::move(parsed);
}

bool IsIgnored(USHORT vid, USHORT pid) {
    std::lock_guard<std::mutex> lock(ignoredMutex);
    return ignored.count({0, 0}) || ignored.count({vid, pid});
}

void Filter(std::vector<HidColl>& colls) {
    std::set<std::pair<USHORT, USHORT>> digitizers;
    for (const auto& c : colls)
        if (c.page() == 0x0D) digitizers.insert({c.vid, c.pid});
    colls.erase(std::remove_if(colls.begin(), colls.end(), [&](const HidColl& c) {
        return IsIgnored(c.vid, c.pid) || digitizers.count({c.vid, c.pid});
    }), colls.end());
}

bool KysonaKnown(USHORT vid, USHORT pid) {
    // IDs explicitly documented in upstream kysona.cpp at 666b7def119066d098c105805d68f9314af2fc64.
    return (vid == 0x3554 && (pid == 0xF55D || pid == 0xF5D5 || pid == 0xF55E ||
            pid == 0xF58A || pid == 0xF58C || pid == 0xF58E || pid == 0xF58F || pid == 0xF505 || pid == 0xF506)) ||
           (vid == 0x3770 && pid == 0x0200) || (vid == 0x3710 && pid == 0x5406);
}
} // namespace ModSafety

namespace ModConfig {
inline std::atomic<int> pollMs{60000}, warning{15}, fontSize{13}, gap{14};
inline std::atomic<bool> taskbar{true}, hideSleeping{false}, airpodsEnabled{false};
inline void* bleEvent = nullptr;
inline void Load() {
    pollMs = std::clamp(Wh_GetIntSetting(L"pollSeconds"), 10, 600) * 1000;
    warning = std::clamp(Wh_GetIntSetting(L"warningPercent"), 0, 50);
    fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 10, 24);
    gap = std::clamp(Wh_GetIntSetting(L"deviceGap"), 4, 40);
    hideSleeping = !!Wh_GetIntSetting(L"hideSleeping");
    airpodsEnabled = !!Wh_GetIntSetting(L"enableAirPods");
    auto mode = WindhawkUtils::StringSetting::make(L"displayMode");
    taskbar = wcscmp(mode, L"tray") != 0;
    auto ignored = WindhawkUtils::StringSetting::make(L"ignoreDevices");
    ModSafety::ConfigureIgnored(ignored.get());
}
}
namespace ModHost { extern std::atomic<bool> stopRequested; extern std::atomic<HWND> window; }

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
    ModSafety::Filter(out);
    return out;
}

// ---- HidDev -----------------------------------------------------------------------------------

bool HidDev::Open(const HidColl& c) {
    Close();
    if (ModSafety::IsIgnored(c.vid, c.pid)) return false;
    c_ = c;
    h_ = CreateFileW(c.path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                     FILE_FLAG_OVERLAPPED, nullptr);
    overlapped_ = true;
    return ok();
}

bool HidDev::OpenFeatureOnly(const HidColl& c) {
    Close();
    if (ModSafety::IsIgnored(c.vid, c.pid)) return false;
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
            if (FAILED(buffer->get_Length(&length)) || length != 27 ||
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
    if (pct < 0) return ui::Text(L"Unknown", L"未知");
    return ui::Text(L"~", L"约") + std::to_wstring(pct) + L"%" + (charging ? ui::Text(L" (charging)", L"（充电中）") : L"");
}
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

void PollAirPods(PollContext& ctx) {
    StartAirPods();
    auto s = GetReceiver().state;
    std::lock_guard<std::mutex> lock(s->mutex);
    const auto now = GetTickCount64();
    // Rotating BLE addresses cannot prove ownership. Report one nearby source per model,
    // explicitly labelled as unverified, rather than attributing it to a paired device.
    std::map<uint16_t, Seen> strongest;
    for (const auto& [address, seen] : s->seen) {
        if (now - seen.when > 120000) continue;
        auto it = strongest.find(seen.battery.model);
        if (it == strongest.end() || seen.rssi > it->second.rssi || now - it->second.when > 15000)
            strongest[seen.battery.model] = seen;
    }
    for (const auto& [model, seen] : strongest) {
        const auto& b = seen.battery;
        DeviceStatus status;
        status.key = L"airpods-nearby:" + Hex4(model);
        status.name = std::wstring(airpods::ModelName(model)) + ui::Text(L" (nearby broadcast)", L"（附近广播）");
        status.kind = DeviceKind::Headset;
        status.via = L"Apple BLE";
        status.percent = airpods::EarPercent(b);
        const bool onlyBox = status.percent < 0;
        if (onlyBox) status.percent = b.box;
        status.levelText = (onlyBox ? ui::Text(L"Case ~", L"盒约") : ui::Text(L"~", L"约")) + std::to_wstring(status.percent) + L"%";
        status.charging = onlyBox ? b.boxCharging :
            (((b.left >= 0 && b.leftCharging) && (b.right < 0 || b.rightCharging)) ||
             (b.left < 0 && b.right >= 0 && b.rightCharging));
        status.detailText = ui::Text(L"Left: ", L"左耳") + Level(b.left, b.leftCharging) + ui::Text(L"; right: ", L"；右耳") + Level(b.right, b.rightCharging) +
            ui::Text(L"; case: ", L"；充电盒") + Level(b.box, b.boxCharging) + ui::Text(L"; nearby broadcast, ownership unverified", L"；附近广播，归属未验证");
        if (now - seen.when > 30000) {
            status.asleep = true; status.percent = -1;
            status.levelText.clear(); status.detailText += ui::Text(L"; last known", L"；历史读数");
        }
        ctx.out.push_back(status);
    }
}

void RefreshAirPods(std::vector<DeviceStatus>& devices) {
    devices.erase(std::remove_if(devices.begin(), devices.end(), [](const DeviceStatus& s) {
        return s.key.compare(0, 15, L"airpods-nearby:") == 0;
    }), devices.end());
    PollContext ctx;
    wb_driver_airpods::PollAirPods(ctx);
    devices.insert(devices.end(), ctx.out.begin(), ctx.out.end());
}

}

void PollAirPods(PollContext& ctx) { if (ModConfig::airpodsEnabled) wb_driver_airpods::PollAirPods(ctx); }

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

namespace wb_driver_atk_serial {
// ATK N9 Pro headset (VID 373B: dongle 11CB, wired 11CA). Unlike other headsets it is not
// queried over HID but over a USB virtual COM port (9600 8N1), with 64-byte frames:
//   request [len = 2][cmd][sub][0...]        reply [len][cmd][sub][data...]
//   cmd 00 sub 04 = status  -> data[1] == 1 when the headset is connected to the dongle
//   cmd 00 sub 03 = battery -> data[1] = percent (bits 0-6), charging (bit 7)
// Protocol from ATK's own configuration software (ATK HUB, "n9-pro" model).
// If the ATK desktop app is running it holds the port and the headset is skipped.
namespace {

struct Port { std::wstring name; USHORT pid; };

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
        USHORT pid = id.find(L"VID_373B&PID_11CB") != std::wstring::npos ? 0x11CB
                   : id.find(L"VID_373B&PID_11CA") != std::wstring::npos ? 0x11CA
                                                                         : 0;
        if (!pid) continue;
        HKEY k = SetupDiOpenDevRegKey(di, &dd, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (k == INVALID_HANDLE_VALUE) continue;
        wchar_t name[32] = {};
        DWORD sz = sizeof(name) - sizeof(wchar_t), type = 0;
        if (RegQueryValueExW(k, L"PortName", nullptr, &type, (BYTE*)name, &sz) == ERROR_SUCCESS && type == REG_SZ)
            out.push_back({ name, pid });
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
        if (ModSafety::IsIgnored(0x373B, p.pid)) continue;
        if (ctx.IsClaimed(0x373B, p.pid)) continue;
        Serial port(p.name);
        if (!port.ok()) continue;  // in use (ATK app open) or not a working port
        std::vector<BYTE> data;
        if (!port.Command(0x00, 0x04, data)) continue;
        DeviceStatus s;
        s.key = L"atk-serial:" + Hex4(p.pid);
        s.name = L"ATK N9 Pro";
        s.kind = DeviceKind::Headset;
        s.via = p.pid == 0x11CA ? L"USB cable" : L"2.4 GHz receiver";
        s.connected = data[1] == 1;
        if (s.connected && port.Command(0x00, 0x03, data)) {
            s.percent = Clamp100(data[1] & 0x7F);
            s.charging = (data[1] & 0x80) != 0;
        }
        ctx.Add(s, 0x373B, p.pid);
    }
}

}

void PollAtkSerial(PollContext& ctx) { wb_driver_atk_serial::PollAtkSerial(ctx); }

namespace wb_driver_aula {
// AULA F75 / F87 Pro / F99 wireless keyboards on the Compx 2.4 GHz receiver (VID 3554, PID FA09).
// The receiver exposes a 20-byte vendor collection on usage page FF02 / usage 02. Battery is a
// 20-byte output report 0x13 carrying command 0x4A and a trailing sum checksum. The reply is
// another 0x13 frame whose command byte, with its top bit masked, is 0x4A. Its payload starts at
// byte 5: the level (1..100), then the state byte (0x01 on battery, 0x10 with the cable in).
// The receiver also pushes unrelated 0x13 frames (a 0x0A status among them), so the report id
// alone does not identify the reply.
// From github.com/PyFlat/Device-Battery-Info (PR 6) and github.com/deepan-alve/womier-l65-linux.
namespace {

BYTE Sum(const std::vector<BYTE>& b, size_t n) {
    unsigned s = 0;
    for (size_t i = 0; i < n; i++) s += b[i];
    return (BYTE)(s & 0xFF);
}

bool QueryBattery(HidDev& d, std::vector<BYTE>& r) {
    std::vector<BYTE> req(20, 0);
    req[0] = 0x13;
    req[1] = 0x4A;
    req[19] = Sum(req, 19);
    d.Flush();
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + 1000;
    for (;;) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (left <= 0) return false;
        int n = d.Read(r, left);
        if (n < 0) return false;
        if (n < 20) continue;
        if (r[0] != 0x13) continue;
        if ((r[1] & 0x7F) != 0x4A) continue;
        if (r[19] != Sum(r, 19)) continue;
        return true;
    }
}

}  // namespace

void PollAula(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x3554)) {
        if (pid != 0xFA09) continue;  // other 3554 products are mice on a different protocol
        for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                 return c.page() == 0xFF02 && c.usage() == 0x0002 && c.outLen() >= 20 && c.inLen() >= 20;
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            std::vector<BYTE> r;
            // The first request can go unanswered while the 2.4 GHz link wakes up; retry once.
            if (!QueryBattery(d, r) && !QueryBattery(d, r)) continue;
            int level = r[5];
            if (level < 1 || level > 100) continue;
            DeviceStatus s;
            s.key = L"aula:" + Hex4(vid) + L":" + Hex4(pid);
            s.name = L"AULA wireless keyboard";
            s.kind = DeviceKind::Keyboard;
            s.via = L"2.4 GHz receiver";
            s.percent = level;
            s.charging = r[6] == 0x10;
            ctx.Add(s, vid, pid);
            break;
        }
    }
}

}

void PollAula(PollContext& ctx) { wb_driver_aula::PollAula(ctx); }

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
//    Property 0F = battery (0..1000), 10 = charge state (1 charging, 3 full). Target 08 is the device
//    or receiver itself, 09+ are devices paired to a receiver. GET is read-only; we never switch the
//    device into software mode.
namespace {

const USHORT kVoidPids[] = { 0x1B1C, 0x1B27, 0x0A14, 0x0A16, 0x0A17, 0x0A1D, 0x0A1A, 0x1B2A, 0x1B23,
                             0x1B29, 0x0A55, 0x0A51, 0x0A52, 0x0A38, 0x0A4F, 0x0A2B, 0x0A75, 0x0A56 };
const USHORT kBragiPids[] = { 0x2A08, 0x2A02, 0x0A97, 0x0A64, 0x0A62, 0x0A3E, 0x0A3D };

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
    bool Get(BYTE target, BYTE prop, unsigned& value, int timeoutMs = 300) {
        std::vector<BYTE> req = numbered_ ? std::vector<BYTE>{ 0x02, target, 0x02, prop } : std::vector<BYTE>{ 0x00, 0x02, target, 0x02, prop };
        d_.Flush();
        if (!d_.Write(req)) return false;
        BYTE source = target == 0x08 ? 0x00 : 0x01;
        ULONGLONG end = GetTickCount64() + timeoutMs;
        std::vector<BYTE> r;
        for (;;) {
            int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
            if (left <= 0 || d_.Read(r, left) < 8) return false;
            if (r[2] != 0x02 || r[1] != source) continue;  // notification or another app's reply
            if (r[3] != 0x00) return false;
            value = r[4] | (r[5] << 8) | (r[6] << 16) | ((unsigned)r[7] << 24);
            return true;
        }
    }

private:
    HidDev d_;
    bool numbered_;
    bool ok_ = false;
};

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
bool IsNova(USHORT pid) { return pid == 0x1098 || pid == 0x100F; }

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
            s.via = pid == 0x1098 ? L"2.4 GHz receiver" : L"USB cable";
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

std::wstring StripServiceSuffix(std::wstring n) {
    for (const wchar_t* suf : { L" Hands-Free AG", L" Hands-Free", L" Avrcp Transport", L" Stereo", L" Audio Gateway" }) {
        size_t l = wcslen(suf);
        if (n.size() > l && n.compare(n.size() - l, l, suf) == 0) return n.substr(0, n.size() - l);
    }
    return n;
}

}  // namespace

void PollGenericHid(PollContext& ctx) {
    // Backport upstream 99c55b2 / issue #232: never battery-poll drawing tablets.
    std::set<std::pair<USHORT, USHORT>> digitizers;
    for (const auto& c : ctx.colls)
        if (c.page() == 0x0D) digitizers.insert({c.vid, c.pid});
    for (const auto& c : ctx.colls) {
        if (digitizers.count({c.vid, c.pid})) continue;
        if (ctx.IsClaimed(c.vid, c.pid)) continue;
        if (c.page() == 0x84 || c.page() == 0x85) continue;  // UPS / power supplies
        if (!c.caps.NumberFeatureValueCaps && !c.caps.NumberInputValueCaps) continue;
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
            DEVPROP_BOOLEAN conn;
            if (SetupDiGetDevicePropertyW(di, &dd, &kIsConnected, &t, (PBYTE)&conn, sizeof(conn), nullptr, 0) &&
                t == DEVPROP_TYPE_BOOLEAN)
                containerConn[cid] = conn != DEVPROP_FALSE;
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

    for (auto& [cid, f] : battery) {
        auto mc = containerConn.find(cid);
        if (mc != containerConn.end() ? !mc->second : !f.connected) continue;
        DeviceStatus s;
        s.key = L"bt:" + cid;
        auto it = containerName.find(cid);
        s.name = (it != containerName.end() && !it->second.empty()) ? it->second : f.name;
        if (s.name.empty()) s.name = L"Bluetooth device";
        // Skip devices a vendor driver already reported over HID++ / Bragi on the same link.
        bool dup = false;
        for (auto& o : ctx.out) dup |= o.name == s.name;
        if (dup) continue;
        s.via = L"Bluetooth";
        s.percent = f.pct;
        auto cod = containerCod.find(cid);
        s.kind = cod != containerCod.end() ? KindFromClassOfDevice(cod->second) : DeviceKind::Other;
        if (s.kind == DeviceKind::Other) s.kind = GuessKind(s.name);
        ctx.out.push_back(s);
    }
}

}

void PollBluetooth(PollContext& ctx) { wb_driver_generic::PollBluetooth(ctx); }

void PollGenericHid(PollContext& ctx) { wb_driver_generic::PollGenericHid(ctx); }

namespace wb_driver_glorious {
// Glorious (VID 258A), Hitscan (VID 3770) and G-Wolves (VID 33E4) wireless mice using the same
// supplier protocol (G-Wolves calls it "XVI"; its other models use the Kysona protocol instead):
// Request: 64-byte feature report (id 0), bytes [3]=02 [4]=02 [6]=83.
// Reply:   [1]=status, [4]=02, [6]=83, [7]=charging (1), [8]=percent.
// Status A1 = awake, A4 = asleep, A0 = waking up, others = unreachable.
// Protocol: github.com/ikajdan/glorious-indicator, github.com/louis4craft/glorious-ctl,
// Hitscan's web configurator (control.hitscan.com) and G-Wolves' (mouse.xyz, Config/env-models.json).
// Model names and cable product ids from G-Wolves' own device list (mouse.xyz/Config/env-models.json).
// 33E4:3854 is a dongle shared by many models, so it has no specific name.
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

struct Vendor {
    USHORT vid;
    const wchar_t* keyPrefix;
    int replyDelayMs;
    const wchar_t* (*name)(USHORT pid);
    bool (*isCable)(USHORT pid);
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

// 258A is Sinowealth's vendor id, shared by many brands. For a product we do not know, use the
// device's own name and work out whether it is a keyboard or a mouse instead of assuming a mouse.
DeviceKind SinowealthKind(const std::vector<HidColl>& all, USHORT vid, USHORT pid, const std::wstring& name) {
    DeviceKind k = GuessKind(name);
    if (k != DeviceKind::Other) return k;
    int keyboards = 0;
    for (const auto& c : all)
        if (c.vid == vid && c.pid == pid && c.page() == 0x01 && c.usage() == 0x06) keyboards++;
    if (keyboards >= 2) return DeviceKind::Keyboard;  // keyboards expose boot + NKRO keyboard collections
    return KindFromCollections(all, vid, pid);
}
bool GloriousCable(USHORT pid) { return pid == 0x2011 || pid == 0x2012 || pid == 0x2013 || pid == 0x2014 || pid == 0x201A; }

const wchar_t* HitscanName(USHORT pid) {
    switch (pid) {
        case 0x0136: return L"Hitscan Hyperlight X";
        case 0x0194: return L"Hitscan Hyperlight N";
        default: return L"Hitscan Hyperlight";
    }
}
bool HitscanCable(USHORT pid) { return pid == 0x0136 || pid == 0x0194; }

const Vendor kVendors[] = {
    { 0x258A, L"glorious:", 50, GloriousName, GloriousCable },
    { 0x3770, L"hitscan:", 200, HitscanName, HitscanCable },
    { 0x33E4, L"gwolves:", 100, GWolvesModelName, GWolvesIsCable },
};

}  // namespace

void PollGlorious(PollContext& ctx) {
    for (const Vendor& v : kVendors) {
        for (auto [vid, pid] : ctx.Products(v.vid)) {
            // Exact PIDs from the model table and protocol references above.
            bool known = vid == 0x258A ? GloriousName(pid) != nullptr :
                         vid == 0x3770 ? (pid == 0x0136 || pid == 0x0194) :
                         vid == 0x33E4 && pid == 0x3854;
            if (!known) continue;
            for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) { return c.featLen() == 65; })) {
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
                    ok = true;
                    if (r[1] == 0xA0 && attempt == 0) { Sleep(400); continue; }  // waking up: ask again
                    break;
                }
                if (!ok) continue;
                DeviceStatus s;
                s.key = v.keyPrefix + Hex4(pid);
                const wchar_t* known = v.name(pid);
                if (known) {
                    s.name = known;
                    s.kind = DeviceKind::Mouse;
                } else {
                    s.name = HidProduct(*c);
                    if (s.name.empty()) s.name = L"Wireless device";
                    s.kind = SinowealthKind(ctx.colls, vid, pid, s.name);
                }
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
// HyperX wireless headsets (HP VID 03F0, older Kingston VID 0951).
// Protocols: Cloud III Wireless verified on hardware (03F0:0C9D) and github.com/Medprj/HyperXWM;
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

// Cloud II Wireless (Kingston 0951:1718): fixed 62-byte frame, reply 0B 00 BB <cmd>.
bool Cloud2Kingston(const HidColl& c, DeviceStatus& s) {
    HidDev d;
    if (!d.Open(c)) return false;
    auto q = [&](BYTE cmd, std::vector<BYTE>& r) {
        std::vector<BYTE> req = { 0x06, 0x00, 0x02, 0x00, 0x9A, 0x00, 0x00, 0x68, 0x4A, 0x8E, 0x0A, 0x00, 0x00, 0x00, 0xBB, cmd, 0x00 };
        return Query(d, req, r, 1000, [cmd](const std::vector<BYTE>& r) { return r.size() >= 8 && r[0] == 0x0B && r[2] == 0xBB && r[3] == cmd; });
    };
    std::vector<BYTE> r;
    if (!q(0x02, r) || r[7] > 100) return false;
    s = Base(c.pid, L"HyperX Cloud II Wireless");
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
    { 0x03F0, { 0x06BE, 0x02CC }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 7; }, Cloud3S },
    { 0x03F0, { 0x0696, 0x018B, 0x0D93 }, [](const HidColl& c) { return c.page() == 0xFF90 && c.usage() == 0x0303; }, Cloud2 },
    { 0x0951, { 0x1718 }, [](const HidColl& c) { return c.outLen() == 62; }, Cloud2Kingston },
    { 0x03F0, { 0x098D }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudAlpha },
    { 0x03F0, { 0x08BE }, [](const HidColl& c) { return c.page() == 0xFF13 && c.usage() == 0xFF00; }, CloudAlpha2 },
    { 0x0951, { 0x16C4, 0x1723, 0x1749 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudFlight },
    { 0x03F0, { 0x0E90 }, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() >= 3; }, CloudFlight },  // Cloud Flight (HP)
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
// Reference: Linux drivers/hid/hid-kysona.c (Lode Willems) and the hid-pulsar patch discussion;
// Redragon ids from github.com/EliottV17/redragon-battery-omarchy and github.com/pingpingjinping/g49-web-control.
namespace {

// Redragon uses Compx's own ids, and its receiver only calls itself "2.4G Wireless Receiver".
bool IsRedragon(USHORT vid, USHORT pid) { return vid == 0x3554 && (pid == 0xF55D || pid == 0xF5D5 || pid == 0xF55E); }

bool IsCable(USHORT vid, USHORT pid) {
    return (vid == 0x33E4 && GWolvesIsCable(pid)) || (vid == 0x3554 && pid == 0xF55E);
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
    for (const wchar_t* suffix : { L" Dongle", L" Receiver", L" 2.4G", L" Wireless Receiver" }) {
        size_t l = wcslen(suffix);
        if (n.size() > l && n.compare(n.size() - l, l, suffix) == 0) n = n.substr(0, n.size() - l);
    }
    if (vid == 0x3770) return L"Hitscan Hyperlight";  // the receiver only calls itself "Hitscan USB Receiver"
    if (n.empty()) n = vid == 0x373B ? L"ATK wireless mouse" : L"Wireless mouse";
    return n;
}

}  // namespace

void PollKysona(PollContext& ctx) {
    std::vector<std::wstring> seenNames;
    for (USHORT vendor : { (USHORT)0x3554, (USHORT)0x373B, (USHORT)0x3770, (USHORT)0x33E4, (USHORT)0x3710 }) {
        for (auto [vid, pid] : ctx.Products(vendor)) {
            if (!ModSafety::KysonaKnown(vid, pid)) continue;
            // Kysona exposes a dedicated 17-byte collection; ATK / VXE receivers put report 08 in a
            // vendor collection that also carries larger reports. The checksummed reply filters out
            // anything that does not speak this protocol.
            for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
                     return (c.outLen() == 17 && c.inLen() == 17) || (c.page() >= 0xFF00 && c.outLen() >= 17 && c.inLen() >= 17);
                 })) {
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
                }
                if (!answered) continue;
                s.key = L"kysona:" + Hex4(vid) + L":" + Hex4(pid);
                s.name = CleanName(HidProduct(*c), vid, pid);
                s.kind = KindFromCollections(ctx.colls, vid, pid);
                if (s.kind == DeviceKind::Other) s.kind = DeviceKind::Mouse;
                s.via = IsCable(vid, pid) ? L"USB cable" : L"2.4 GHz receiver";
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
// dongles get a generic name.
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
// github.com/Sapd/HeadsetControl (headset voltage calibration).
namespace {

constexpr BYTE kSwId = 0x0D;  // our software id; replies to other apps (G HUB, Options+) carry theirs

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
    static const int defv[] = { 4100, 3850, 3780, 3730, 3650, 3300 }, defp[] = { 100, 50, 30, 20, 5, 0 };
    switch (pid) {
        case 0x0A66: return Interpolate(mv, g533v, g533p, 6);
        case 0x0AC4: return Interpolate(mv, g535v, g535p, 6);
        case 0x0A5C: case 0x0A89: case 0x0A5B: case 0x0A87: case 0x0AB5: case 0x0AFE: return Interpolate(mv, g633v, g633p, 8);
        case 0x0AA7: case 0x0AAA: case 0x0ABA: case 0x0AFB: return Interpolate(mv, gprov, gprop, 6);
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

}  // namespace

void PollLogitech(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x046D)) {
        auto shorts = ctx.Find(vid, pid, [](const HidColl& c) { return c.page() == 0xFF00 && c.usage() == 0x0001 && c.outLen() == 7; });
        auto longs = ctx.Find(vid, pid, [](const HidColl& c) {
            return ((c.page() == 0xFF00 && c.usage() == 0x0002) || (c.page() == 0xFF43 && c.usage() == 0x0202)) && c.outLen() == 20;
        });
        if (shorts.empty() && longs.empty()) continue;
        Hidpp h;
        if (!h.Open(shorts.empty() ? nullptr : shorts[0], longs.empty() ? nullptr : longs[0])) continue;

        std::wstring product = HidProduct(longs.empty() ? *shorts[0] : *longs[0]);
        bool isReceiver = product.find(L"Receiver") != std::wstring::npos || product.find(L"USB Receiver") != std::wstring::npos;

        // A device answering a HID++ 2.0 ping at index 0xFF is connected directly (USB cable,
        // Bluetooth, or a receiver-less headset); otherwise it is a receiver with paired devices.
        std::vector<BYTE> p;
        BYTE err = 0;
        std::vector<BYTE> indices;
        if (!isReceiver && h.Request(0xFF, 0x00, 0x10 | kSwId, { 0, 0, 0x5A }, p, err, false, 400) == 1 && p.size() >= 3 && p[0] >= 2)
            indices = { 0xFF };
        else
            indices = { 1, 2, 3, 4, 5, 6 };

        bool any = false;
        for (BYTE idx : indices) {
            DeviceStatus s;
            s.key = L"logi:" + Hex4(pid) + L":" + std::to_wstring(idx);
            s.name = (idx == 0xFF && !product.empty()) ? product : L"Logitech wireless device";
            s.via = idx == 0xFF ? L"USB / Bluetooth" : L"Logitech receiver";
            s.graceSec = idx == 0xFF ? 0 : 1800;  // receiver devices drop off the link while sleeping
            s.kind = GuessKind(s.name);

            int r = h.Request(idx, 0x00, 0x10 | kSwId, { 0, 0, 0x5A }, p, err, false, 400);
            bool found = false;
            if (r == 1 && p.size() >= 3 && p[0] >= 2) {
                ReadNameAndType20(h, idx, s);
                found = ReadBattery20(h, idx, pid, s);
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
        if (any) ctx.Claim(vid, pid);
    }
}

}

void PollLogitech(PollContext& ctx) { wb_driver_logitech::PollLogitech(ctx); }

namespace wb_driver_mchose {
// MCHOSE G87 2.4 GHz battery query. Protocol documentation:
// https://github.com/marsuyone/mchose-battery-tray (7206b460), README G87 section.
// Independently implemented using the documented command; no settings writes.
// Observed reply: 13 4A 01 00 02 38 01 00 ... 99 (56%).
void PollMchose(PollContext& ctx) {
    constexpr USHORT vid = 0x41E4, pid = 0x2001;
    if (ctx.IsClaimed(vid, pid)) return;
    for (const HidColl* c : ctx.Find(vid, pid, [](const HidColl& c) {
        return c.page() == 0xFF02 && c.usage() == 2 && c.inLen() == 20 && c.outLen() == 20;
    })) {
        HidDev d;
        if (!d.Open(*c)) continue;
        std::vector<BYTE> request(20, 0), reply;
        request[0] = 0x13;
        request[1] = 0x4A;
        request[2] = 1;
        d.Flush();
        if (!d.Write(request)) continue;
        const ULONGLONG deadline = GetTickCount64() + 1000;
        for (;;) {
            const auto now = GetTickCount64();
            if (now >= deadline) break;
            const int n = d.Read(reply, (int)(deadline - now));
            if (n < 0) break;
            if (n != 20 || reply[0] != 0x13 || reply[1] != 0x4A ||
                reply[2] != 1 || reply[3] != 0 || reply[4] != 2 || reply[5] > 100) continue;
            unsigned sum = 0;
            for (size_t i = 0; i < 19; ++i) sum += reply[i];
            if ((sum & 0xFF) != reply[19]) continue;
            DeviceStatus status;
            status.key = L"mchose:41E4:2001";
            status.name = L"MCHOSE G87";
            status.kind = DeviceKind::Keyboard;
            status.via = L"2.4 GHz receiver";
            status.percent = reply[5];
            // Charging flags are not confirmed for this model; do not guess.
            status.graceSec = 1800;
            ctx.Add(status, vid, pid);
            return;
        }
    }
}

}

void PollMchose(PollContext& ctx) { wb_driver_mchose::PollMchose(ctx); }

namespace wb_driver_misc_mice {
// Smaller mouse brands, each with a community-documented battery protocol:
//  * Motospeed X6 (Realtek VID 0BDA, receivers FFE0 / FFF2, interface 4): output report B5 [06],
//    reply report B4, byte 20 = level (+128 while charging). The cable (FFF1) reports no battery.
//    From github.com/Jacobz888/LGSTrayBattery-MotospeedX6.
//  * Xenta receivers (Delux M900 Pro, Attack Shark X6 / R1 / X11 / X3 Max / V3 Pro; VID 1D57): the receiver
//    broadcasts input report 03 on usage page 000A every ~2 s: [03][model][40][state][percent],
//    state 2 = full, 3 = charging (percent frozen). From github.com/AeonArcHQ/omarchy-delux-m900.
//  * Ajazz AJ179 family (VID 249A receiver, 248A cable, interface 2): unnumbered 32-byte report,
//    command 10 (device info, answered by the receiver): reply [10][00 = ok]..., [4..7] model id,
//    [12] charging, [13] percent, [14] mouse connected (receiver), [31] = sum([4..30]).
//    From github.com/Rockeyxx/aj179-linux (docs/PROTOCOL.md).
//  * AJAZZ AJ-series mice on the SONiX vendor id 3151 (AJ159 PRO / APEX and siblings). The dongle
//    exposes a 65-byte feature collection on usage page FFFF / usage 02. A SET_FEATURE status poll
//    (report 0x00, opcode 0xF7, zero payload) brings the 2.4 GHz telemetry link up; ~30 ms later
//    GET_FEATURE report 0x05 carries the charge in byte 3 (0..100). A frame with a non-zero byte 1
//    or 2 is transient reconnect garbage and is rejected.
//    From github.com/Aiacos/ajazz-control-center (docs/protocols/mouse/aj_series.md).
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

// Xenta receiver (1D57:FA60, also FA65; cable FA55 / 2155), used by Delux and Attack Shark mice.
// The receiver broadcasts [03][model][40][state][level] every ~2 s; nothing is sent to it. The handle
// stays open between polls so these frames queue up. A reading counts once two frames agree, because
// every link-up starts with a false 100% (github.com/qu1ncky/BatteryTray, XentaReceiverProvider).
struct XentaModel { BYTE code; const wchar_t* name; bool tens; };
const XentaModel kXentaModels[] = {
    { 0x3D, L"Delux M900 Pro", false },
    { 0x55, L"Attack Shark X11", false },
    { 0x4E, L"Attack Shark X3 Max", false },
    { 0xE5, L"Attack Shark V3 Pro", false },
    { 0x10, L"Attack Shark X6 / R1", true },  // level reported in tens (0-10)
};

struct XentaListener {
    std::unique_ptr<HidDev> dev;
    std::vector<BYTE> last, prev;  // the two most recent battery frames
    ULONGLONG lastFrameAt = 0;
};
std::map<std::wstring, XentaListener> g_xenta;  // by collection path

void PollXenta(PollContext& ctx) {
    const USHORT pids[] = { 0xFA60, 0xFA65, 0xFA55, 0x2155 };
    std::set<std::wstring> present;
    for (USHORT pid : pids) {
        if (ctx.IsClaimed(0x1D57, pid)) continue;
        for (const HidColl* c : ctx.Find(0x1D57, pid, [](const HidColl& c) { return c.page() == 0x000A && c.inLen() >= 5; })) {
            present.insert(c->path);
            XentaListener& l = g_xenta[c->path];
            bool fresh = !l.dev;
            if (fresh) {
                l.dev = std::make_unique<HidDev>();
                if (!l.dev->Open(*c)) { g_xenta.erase(c->path); continue; }
            }
            // Take everything queued since the last poll; a new listener waits for two frames.
            ULONGLONG until = GetTickCount64() + (fresh ? 5000 : 0);
            std::vector<BYTE> r;
            int got = 0;
            while (true) {
                int wait = fresh && got < 2 ? (int)std::max<LONGLONG>(1, (LONGLONG)until - (LONGLONG)GetTickCount64()) : 30;
                if (fresh && got < 2 && (LONGLONG)until - (LONGLONG)GetTickCount64() <= 0) break;
                if (l.dev->Read(r, wait) < 5) break;
                if (r[0] != 0x03 || r[2] != 0x40) continue;
                l.prev = l.last;
                l.last.assign(r.begin(), r.begin() + 5);
                l.lastFrameAt = GetTickCount64();
                got++;
            }
            if (l.last.empty() || GetTickCount64() - l.lastFrameAt > 15000) break;  // mouse off: no frames
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
            if (cable || st == 0x02 || st == 0x03) {
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

}  // namespace

void PollMiscMice(PollContext& ctx) {
    PollMotospeed(ctx);
    PollXenta(ctx);

}

}

void PollMiscMice(PollContext& ctx) { wb_driver_misc_mice::PollMiscMice(ctx); }

namespace wb_driver_playstation {
// Sony DualShock 4 / DualSense / DualSense Edge (VID 054C), USB or Bluetooth.
// The battery is part of the regular input report:
//   DualShock 4: USB report 01 byte 30, Bluetooth report 11 byte 32: bits 0-3 level, bit 4 cable
//   DualSense:   USB report 01 byte 53, Bluetooth report 31 byte 54: bits 0-3 level (0-10), bits 4-7 state
// Never enable enhanced Bluetooth reports: doing so breaks legacy DirectInput until power cycling.
// Bluetooth battery readings require another app to have already enabled full reports.
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

namespace wb_driver_rapoo {
// Rapoo VT gaming mice. Battery is pushed periodically; no commands or handshake are needed.
// Sources:
//  * Official Rapoo Hub (https://hub.rapoo.cn/), VT_nrf54L input-report decoder
//    and hardware manifests: report 07 on FF00:0002, state at byte 7, percent at byte 8,
//    optional mouse PID at 13..14. Connection is the low nibble of byte 1 (0 RF / 1 BT / 2 USB).
//  * https://github.com/PyFlat/Device-Battery-Info (RapooProtocol.cs): older 7-byte BB B0
//    reports on FF00:0002, state at byte 5, percent at byte 6; charging can report 7F (unknown).
// Legacy BB semantics are restricted to VT3 Pro 1215/4415; other Telink firmware differs.
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
    { 0x4614, 0x1414, L"Rapoo VT9" },
    { 0x4618, 0x1418, L"Rapoo VT9 Max" },
    { 0x4415, 0x1215, L"Rapoo VT3 Pro" },
};

const Model* FindModel(USHORT pid) {
    for (const auto& model : kModels)
        if (pid == model.cable || pid == model.receiver) return &model;
    return nullptr;
}

bool IsLegacyVT3Pro(USHORT pid) { return pid == 0x1215 || pid == 0x4415; }

bool ReadBattery(HidDev& d, Rapoo::Battery& battery, Rapoo::Profile profile) {
    // A passive legacy report can take ~3.2 seconds. One deadline includes unrelated reports;
    // do not flush, since a queued status report is already useful.
    ULONGLONG end = GetTickCount64() + 4000;
    do {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        std::vector<BYTE> r;
        if (left <= 0 || d.Read(r, left) < 0) return false;
        if (Rapoo::DecodeBattery(r.data(), r.size(), battery, profile)) return true;
    } while (GetTickCount64() < end);
    return false;
}

}  // namespace

void PollRapoo(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x24AE)) {
        for (const HidColl* c : ctx.Find(vid, pid, [pid](const HidColl& c) {
                 // The other vendor collections are for configuration/firmware, not battery.
                 return c.page() == 0xFF00 && c.usage() == 0x0002 &&
                        (c.inLen() == 19 || (c.inLen() == 7 && IsLegacyVT3Pro(pid)));
             })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            Rapoo::Battery battery;
            auto profile = c->inLen() == 7 ? Rapoo::Profile::LegacyVT3Pro : Rapoo::Profile::Nordic54L;
            if (!ReadBattery(d, battery, profile)) continue;

            const Model* model = FindModel(battery.mousePid ? battery.mousePid : pid);
            USHORT identity = model ? model->cable : (battery.mousePid ? battery.mousePid : pid);
            DeviceStatus s;
            s.key = L"rapoo:" + Hex4(identity);
            s.name = model ? model->name : HidProduct(*c);
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
        case 0x02BA: case 0x02D2: case 0x02D5: return 0x9F;  // wireless keyboards
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
    0x02B9, 0x02BA, 0x02C5, 0x02C6, 0x02C7, 0x02CD, 0x02CF, 0x02D2, 0x02D4, 0x02D5,
    0x02D7, 0x02E0, 0x02E1, 0x02EA, 0x0A24,
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
    std::vector<std::wstring> seenNames;
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
            // A wireless mouse plugged in for charging shows up twice (cable + dongle); keep one.
            bool dup = false;
            for (auto& n : seenNames) dup |= n == s.name;
            if (!dup) {
                seenNames.push_back(s.name);
                ctx.Add(s, vid, pid);
            } else {
                ctx.Claim(vid, pid);
                for (auto& o : ctx.out)
                    if (o.name == s.name && s.charging) o.charging = true;
            }
            break;
        }
    }
}

}

void PollRazer(PollContext& ctx) { wb_driver_razer::PollRazer(ctx); }

namespace wb_driver_razer_headset {
// Razer BlackShark V3 / V3 Pro headsets (1532:0576/0577 V3 Pro, 0579/057A V3), interface 5.
// 64-byte output report 02: [02][00][60][00 00][00][size][00 00][dir][class][sub][argc][args]..
// [62] = XOR of bytes 0..61. GET battery = class 21 -> reply args [level %][charging].
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

struct Cached { ULONGLONG at = 0; DeviceStatus status; bool valid = false; bool attempted = false; };
std::map<USHORT, Cached> g_cache;

const wchar_t* Name(USHORT pid) { return (pid == 0x0576 || pid == 0x0577) ? L"Razer BlackShark V3 Pro" : L"Razer BlackShark V3"; }

}  // namespace

void PollRazerHeadsets(PollContext& ctx) {
    for (USHORT pid : { (USHORT)0x0576, (USHORT)0x0577, (USHORT)0x0579, (USHORT)0x057A }) {
        if (ctx.IsClaimed(0x1532, pid)) continue;
        auto colls = ctx.Find(0x1532, pid, [](const HidColl& c) { return c.iface == 5 && c.outLen() >= 64 && c.inLen() >= 15; });
        if (colls.empty()) continue;
        Cached& cache = g_cache[pid];
        if (cache.attempted && GetTickCount64() - cache.at < kMinIntervalMs) {
            if (cache.valid) ctx.Add(cache.status, 0x1532, pid);
            else ctx.Claim(0x1532, pid); // prevent the generic Razer driver from retrying
            continue;
        }
        cache.at = GetTickCount64();
        cache.attempted = true; // rate-limit failed opens, handshakes and timeouts too
        ctx.Claim(0x1532, pid);
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
        DeviceStatus s;
        s.key = L"blackshark:" + Hex4(pid);
        s.name = Name(pid);
        s.kind = DeviceKind::Headset;
        s.via = L"2.4 GHz receiver";
        s.percent = r[13];
        s.charging = r[14] != 0;
        cache.status = s;
        cache.valid = true;
        ctx.Add(s, 0x1532, pid);
    }
}

}

void PollRazerHeadsets(PollContext& ctx) { wb_driver_razer_headset::PollRazerHeadsets(ctx); }

namespace wb_driver_rog {
// ASUS ROG wireless keyboards and mice (VID 0B05), including the ROG Omni Receiver (0B05:1ACE).
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

}  // namespace

void PollRog(PollContext& ctx) {
    std::vector<std::wstring> seen;  // one entry per device name (wired + receiver both present while charging)
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
                if (vid == 0x3151 && pid == 0x402D && !keyboard) s.name = L"Attack Shark R3";
                else if (generic) s.name = keyboard ? L"Wireless keyboard" : L"Wireless mouse";
                else s.name = product;
                s.kind = keyboard ? DeviceKind::Keyboard : DeviceKind::Mouse;
                s.via = L"2.4 GHz receiver";
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
// Sony INZONE H9 II (054C:0FA8) and H5 (054C:0EBF) headsets: a Sony command layer tunnelled through
// HID (report 02, 64 bytes) on the vendor collection with usage page FF04.
//   command: [02][len][01][00 FC][8+n][96 C3][addr][event][type][tid lo][tid hi][payload][sum]
//   event:   [02][len][04][FF][..][00][96 C3][addr][event][type][tid lo][tid hi][payload][sum]
// Battery = event 04, type 01 (GET) to the headset (addr 0x41); reply payload [charging][percent],
// percent FF = headset off. Both directions carry a checksum and a transaction id.
// Protocol from github.com/Sapd/HeadsetControl (sony_inzone_protocol.hpp).
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

bool GetBattery(HidDev& d, int& percent, bool& charging, bool& offline) {
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
    if (!d.Write(req)) return false;
    ULONGLONG end = GetTickCount64() + 1500;
    std::vector<BYTE> r;
    while ((LONGLONG)end - (LONGLONG)GetTickCount64() > 0) {
        int left = (int)((LONGLONG)end - (LONGLONG)GetTickCount64());
        if (d.Read(r, left) < 15) return false;
        Event e;
        if (!ParseEvent(r, e) || e.id != 0x04 || e.tid != tid || e.payload.size() < 2) continue;
        if (e.type != 0x10 && e.type != 0xA0) continue;  // RET or active notification
        offline = e.payload[1] == 0xFF;
        percent = offline ? -1 : (e.payload[1] <= 100 ? e.payload[1] : -1);
        charging = e.payload[0] != 0;
        return true;
    }
    return false;
}

}  // namespace

void PollSonyInzone(PollContext& ctx) {
    struct Model { USHORT pid; USHORT usage; const wchar_t* name; };
    for (const Model& m : { Model{ 0x0FA8, 0x0001, L"Sony INZONE H9 II" }, Model{ 0x0EBF, 0x0002, L"Sony INZONE H5" } }) {
        if (ctx.IsClaimed(0x054C, m.pid)) continue;
        for (const HidColl* c : ctx.Find(0x054C, m.pid, [&](const HidColl& c) { return c.page() == 0xFF04 && c.outLen() >= 64; })) {
            HidDev d;
            if (!d.Open(*c)) continue;
            int pct = -1;
            bool charging = false, offline = false;
            if (!GetBattery(d, pct, charging, offline)) continue;
            DeviceStatus s;
            s.key = L"inzone:" + Hex4(m.pid);
            s.name = m.name;
            s.kind = DeviceKind::Headset;
            s.via = L"2.4 GHz receiver";
            s.connected = !offline;
            s.percent = pct;
            s.charging = charging;
            ctx.Add(s, 0x054C, m.pid);
            break;
        }
    }
}

}

void PollSonyInzone(PollContext& ctx) { wb_driver_sony_inzone::PollSonyInzone(ctx); }

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

// Arctis Pro Wireless: 41 AA -> [0]==02 offline; 40 AA -> [0] level 0..4.
bool ArctisProWireless(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x41, 0xAA }, r) || r.empty()) return false;
    s = Headset(pid, L"SteelSeries Arctis Pro Wireless");
    s.connected = r[0] != 0x02;
    if (s.connected && WriteRead(d, { 0x40, 0xAA }, r) && !r.empty()) s.percent = MapTo100(r[0], 0, 4);
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

// Aerox 3/5/9 Wireless, Prime Wireless: 92 (wired) / D2 (wireless) -> [1] = (level/5 + 1) | charging<<7.
bool Aerox(HidDev& d, USHORT pid, DeviceStatus& s) {
    bool wired = In(pid, { 0x183A, 0x1854, 0x185A, 0x1842 });
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x00, (BYTE)(wired ? 0x92 : 0xD2) }, r, 300) || r.size() < 2) return false;
    const wchar_t* name = In(pid, { 0x1838, 0x183A }) ? L"SteelSeries Aerox 3 Wireless"
                        : In(pid, { 0x1852, 0x1854 }) ? L"SteelSeries Aerox 5 Wireless"
                        : In(pid, { 0x1858, 0x185A }) ? L"SteelSeries Aerox 9 Wireless"
                                                      : L"SteelSeries Prime Wireless";
    s = Mouse(pid, name, wired);
    int raw = r[1] & 0x7F;
    s.charging = (r[1] & 0x80) != 0;
    if (raw == 0) s.connected = false;  // mouse off / out of range
    else s.percent = Clamp100((raw - 1) * 5);
    return true;
}

// Rival 3 Wireless (+ Gen 2), Rival 650: AA 01 -> [0] %, [2] charging.
bool Rival(HidDev& d, USHORT pid, DeviceStatus& s) {
    std::vector<BYTE> r;
    if (!WriteRead(d, { 0x00, 0xAA, 0x01 }, r, 300) || r.size() < 3) return false;
    s = Mouse(pid, pid == 0x172B ? L"SteelSeries Rival 650 Wireless" : pid == 0x1872 ? L"SteelSeries Rival 3 Wireless Gen 2" : L"SteelSeries Rival 3 Wireless",
              pid == 0x172B);
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
    { { 0x12E0 }, Iface4, NovaProWireless },
    { { 0x220E, 0x2212, 0x2216, 0x2236, 0x2269, 0x226D, 0x2232, 0x2253, 0x230A, 0x2202, 0x22A1, 0x227E, 0x2206, 0x2258, 0x229E,
        0x22AD, 0x223A, 0x22A9, 0x227A, 0x22A4, 0x22A5, 0x220A, 0x22A7 },
      PageFFC0, NovaFamily },
    { { 0x1838, 0x183A, 0x1852, 0x1854, 0x1858, 0x185A, 0x1840, 0x1842 }, Iface3, Aerox },
    { { 0x1830, 0x1872 }, Iface3, Rival },
    { { 0x172B }, Iface0, Rival },
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
                if (d.Open(*c) && m.poll(d, pid, s)) { ctx.Add(s, vid, pid); break; }
            }
            break;
        }
    }
}

}

void PollSteelSeries(PollContext& ctx) { wb_driver_steelseries::PollSteelSeries(ctx); }

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

// Adds Xbox controllers with a real percentage. Returns how many were added.
int PollWgi(PollContext& ctx) {
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
        IGameControllerBatteryInfo* info = nullptr;
        IBatteryReport* report = nullptr;
        if (vid == 0x045E && SUCCEEDED(raw->QueryInterface(__uuidof(IGameControllerBatteryInfo), (void**)&info)) && info &&
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
                s.kind = DeviceKind::Gamepad;
                s.via = L"Xbox Wireless / Bluetooth";
                s.percent = Clamp100(r * 100 / f);
                s.charging = st == ABI::Windows::System::Power::BatteryStatus_Charging;
                ctx.Add(s, vid, pid);
                added++;
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
    int precise = btXbox ? 0 : PollWgi(ctx);

    const XInputApi& api = LoadXInput();
    if (!api.battery || !api.state) return;
    for (DWORD user = 0; user < 4; user++) {
        // The Xbox Wireless adapter answers battery queries for empty slots too (one controller
        // showed up as four "Medium" headsets), so only look at slots with a controller connected.
        XINPUT_STATE state{};
        if (api.state(user, &state) != ERROR_SUCCESS) continue;
        for (BYTE type : { (BYTE)BATTERY_DEVTYPE_GAMEPAD, (BYTE)BATTERY_DEVTYPE_HEADSET }) {
            if (type == BATTERY_DEVTYPE_GAMEPAD && (btXbox || precise > 0)) continue;  // already shown precisely
            XINPUT_BATTERY_INFORMATION bi{};
            if (api.battery(user, type, &bi) != ERROR_SUCCESS) continue;
            if (bi.BatteryType == BATTERY_TYPE_DISCONNECTED || bi.BatteryType == BATTERY_TYPE_WIRED) continue;
            // Without a headset plugged in, the headset query can still answer with an unknown type.
            if (type == BATTERY_DEVTYPE_HEADSET && bi.BatteryType == BATTERY_TYPE_UNKNOWN) continue;
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
//    From github.com/yleshchuk/zowie-battery.
//  * DW series (U2-DW, S2-DW ...): receiver 800A. Output report 08 [B4 15] on usage page FF03,
//    reply input report 09 on usage page FF04: mV = ([1] * 256 + [2]) * 10, [4] == C8.
//    From github.com/IgnacioBarb/zowie-battery-tray (MIT).
namespace {

// Voltage -> % anchored on the mouse's own LED thresholds (10 / 25 / 50 %).
int ZowiePercent(int mv) {
    static const int v[] = { 4200, 4000, 3890, 3760, 3680, 3500 };
    static const int p[] = { 100, 75, 50, 25, 10, 0 };
    return Interpolate(mv, v, p, 6);
}

bool PollCw(PollContext& ctx, USHORT pid) {
    for (const HidColl* c : ctx.Find(0x04A5, pid, [](const HidColl& c) { return c.page() >= 0xFF00 && c.outLen() == 65; })) {
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
    bool cable = pid != 0x800A;
    DeviceStatus s;
    s.key = L"zowie:" + Hex4(pid);
    s.name = HidProduct(*cmd[0]);
    if (s.name.empty() || s.name.find(L"Receiver") != std::wstring::npos) s.name = L"ZOWIE DW mouse";
    s.kind = DeviceKind::Mouse;
    s.via = cable ? L"USB cable" : L"2.4 GHz receiver";
    if (mv == 0) {
        s.connected = false;
    } else {
        s.millivolts = mv;
        s.percent = ZowiePercent(mv);
        s.charging = cable || mv >= 4250;
    }
    ctx.Add(s, 0x04A5, pid);
    return true;
}

}  // namespace

void PollZowie(PollContext& ctx) {
    for (auto [vid, pid] : ctx.Products(0x04A5)) {
        if (pid == 0x8005 || pid == 0x8006) PollCw(ctx, pid);
        else if (pid == 0x800A) PollDw(ctx, pid);  // only acts on devices with the DW command/reply collections
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
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGlorious(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAula(ctx);       // before Kysona, which shares the 3554 vendor id and would probe it first
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMchose(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAtkSerial(ctx);  // before Kysona, which shares the ATK vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollKysona(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollLamzu(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRongyuan(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSonyInzone(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRog(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollZowie(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRapoo(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollMiscMice(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRazerHeadsets(ctx);  // before the generic Razer driver, which shares the vendor id
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollHyperX(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollLogitech(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollRazer(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollSteelSeries(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollCorsair(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollCoolerMaster(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollPlayStation(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGameSir(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollGenericHid(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollBluetooth(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollAirPods(ctx);
    if (stopEvent && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) return ctx.out;
    PollXInput(ctx);
    return ctx.out;
}

}

std::vector<DeviceStatus> PollAllDevices(HANDLE stopEvent) { return wb_poll::PollAllDevices(stopEvent); }

namespace wb_render {
namespace widget {
bool Create(HINSTANCE, HWND, UINT, bool);
void Destroy();
bool Alive();
void SetItems(const std::vector<WidgetItem>&, const std::wstring&);
void SetLightTheme(bool);
bool Reposition();
}
namespace {

HWND g_wnd, g_taskbar, g_tip, g_notify, g_rebar;
UINT g_clickMsg;
bool g_light, g_hover;
std::vector<WidgetItem> g_items;
std::wstring g_tipText;
RECT g_lastTray{}, g_lastBar{};
int g_lastW = -1, g_lastDpi = 0;
SIZE g_size{};

const wchar_t* kClass = L"TaskbarWirelessBatteryWidget";

int Dpi() {
    UINT d = g_taskbar ? GetDpiForWindow(g_taskbar) : 0;
    return d ? (int)d : 96;
}
int Px(int v) { return MulDiv(v, Dpi(), 96); }

wchar_t Glyph(DeviceKind k, bool bluetooth) {
    switch (k) {
        case DeviceKind::Headset:  return 0xE7F6;  // Headphone
        case DeviceKind::Mouse:    return 0xE962;  // Mouse
        case DeviceKind::Keyboard: return 0xE765;  // KeyboardClassic
        case DeviceKind::Gamepad:  return 0xE7FC;  // Game
        default:                   return bluetooth ? 0xE702 : 0xE772;  // Bluetooth / Devices
    }
}
constexpr wchar_t kBolt = 0xE945;            // LightningBolt
constexpr wchar_t kBatteryUnknown = 0xE996;  // shown when there is nothing to display

// Creates a font, falling back when the preferred face is not installed (GDI would silently substitute).
HFONT MakeFont(int px, int weight, const wchar_t* face, const wchar_t* fallback) {
    for (const wchar_t* f : { face, fallback }) {
        HFONT h = CreateFontW(-px, 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, DEFAULT_PITCH, f);
        HDC dc = CreateCompatibleDC(nullptr);
        HGDIOBJ o = SelectObject(dc, h);
        wchar_t got[LF_FACESIZE] = {};
        GetTextFaceW(dc, LF_FACESIZE, got);
        SelectObject(dc, o);
        DeleteDC(dc);
        if (!_wcsicmp(got, f) || f == fallback) return h;
        DeleteObject(h);
    }
    return nullptr;
}

// Grayscale coverage mask of a string, cropped to its ink bounds.
struct Mask {
    int w = 0, h = 0;
    std::vector<BYTE> a;
};

Mask RenderMask(const std::wstring& s, HFONT font) {
    Mask m;
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ of = SelectObject(dc, font);
    SIZE sz;
    GetTextExtentPoint32W(dc, s.c_str(), (int)s.size(), &sz);
    int W = sz.cx + 8, H = sz.cy + 8;
    BITMAPINFO bi{};
    bi.bmiHeader = { sizeof(BITMAPINFOHEADER), W, -H, 1, 32, BI_RGB };
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ ob = SelectObject(dc, bmp);
    memset(bits, 0, (size_t)W * H * 4);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    TextOutW(dc, 4, 4, s.c_str(), (int)s.size());
    GdiFlush();
    auto* p = static_cast<DWORD*>(bits);
    int x0 = W, y0 = H, x1 = -1, y1 = -1;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            if (p[y * W + x] & 0xFF) { x0 = std::min(x0, x); x1 = std::max(x1, x); y0 = std::min(y0, y); y1 = std::max(y1, y); }
    if (x1 >= 0) {
        m.w = x1 - x0 + 1;
        m.h = y1 - y0 + 1;
        m.a.resize((size_t)m.w * m.h);
        for (int y = 0; y < m.h; y++)
            for (int x = 0; x < m.w; x++) m.a[y * m.w + x] = (BYTE)(p[(y0 + y) * W + x0 + x] & 0xFF);
    }
    SelectObject(dc, ob);
    SelectObject(dc, of);
    DeleteObject(bmp);
    DeleteDC(dc);
    return m;
}

struct Rgb { int r, g, b; };

struct Canvas {
    int w, h;
    std::vector<DWORD> px;  // premultiplied BGRA
    Canvas(int w_, int h_) : w(w_), h(h_), px((size_t)w_ * h_, 0) {}
    void Blend(int x, int y, Rgb c, int a) {  // source-over, a = 0..255
        if (x < 0 || y < 0 || x >= w || y >= h || a <= 0) return;
        DWORD d = px[y * w + x];
        int inv = 255 - a;
        int da = d >> 24, dr = (d >> 16) & 255, dg = (d >> 8) & 255, db = d & 255;
        int oa = a + da * inv / 255;
        int orr = c.r * a / 255 + dr * inv / 255;
        int og = c.g * a / 255 + dg * inv / 255;
        int ob = c.b * a / 255 + db * inv / 255;
        px[y * w + x] = ((DWORD)oa << 24) | ((DWORD)orr << 16) | ((DWORD)og << 8) | (DWORD)ob;
    }
    void Draw(const Mask& m, int x, int y, Rgb c, int alpha) {
        for (int j = 0; j < m.h; j++)
            for (int i = 0; i < m.w; i++) Blend(x + i, y + j, c, m.a[j * m.w + i] * alpha / 255);
    }
};

struct Piece { Mask m; Rgb color; int alpha; int gapAfter; };

std::vector<Piece> Layout(int& width) {
    Rgb fg = g_light ? Rgb{ 0, 0, 0 } : Rgb{ 255, 255, 255 };
    Rgb red = g_light ? Rgb{ 196, 30, 40 } : Rgb{ 255, 90, 90 };
    Rgb charge = g_light ? Rgb{ 0, 120, 212 } : Rgb{ 96, 205, 255 };
    HFONT icons = MakeFont(Px(16), FW_NORMAL, L"Segoe Fluent Icons", L"Segoe MDL2 Assets");
    HFONT bolt = MakeFont(Px(11), FW_NORMAL, L"Segoe Fluent Icons", L"Segoe MDL2 Assets");
    HFONT text = MakeFont(Px(ModConfig::fontSize.load()), FW_NORMAL, L"Segoe UI Variable Text", L"Segoe UI");

    std::vector<Piece> out;
    if (g_items.empty()) out.push_back({ RenderMask(std::wstring(1, kBatteryUnknown), icons), fg, 110, 0 });
    for (size_t i = 0; i < g_items.size(); i++) {
        const auto& it = g_items[i];
        int a = it.dim ? 120 : 255;
        Rgb tc = it.low ? red : fg;
        out.push_back({ RenderMask(std::wstring(1, Glyph(it.kind, it.bluetooth)), icons), fg, a, Px(5) });
        std::wstring t = !it.text.empty() ? it.text : it.percent >= 0 ? std::to_wstring(it.percent) + L"%" : L"?";
        out.push_back({ RenderMask(t, text), tc, a, it.charging ? Px(2) : 0 });
        if (it.charging) out.push_back({ RenderMask(std::wstring(1, kBolt), bolt), charge, a, 0 });
        out.back().gapAfter = i + 1 < g_items.size() ? Px(ModConfig::gap.load()) : 0;
    }
    DeleteObject(icons);
    DeleteObject(bolt);
    DeleteObject(text);

    width = Px(8) * 2;
    for (auto& p : out) width += p.m.w + p.gapAfter;
    return out;
}

void Paint() {
    if (!g_wnd) return;
    RECT bar;
    GetClientRect(g_taskbar, &bar);
    int H = bar.bottom - bar.top;
    int W = 0;
    auto pieces = Layout(W);
    if (W <= 0 || H <= 0) return;
    Canvas cv(W, H);

    // Nearly transparent background so the whole strip receives mouse input,
    // plus a rounded hover highlight like the system tray icons have.
    for (auto& p : cv.px) p = 0x01000000;
    if (g_hover) {
        int inset = Px(4), r = Px(4);
        Rgb hc = g_light ? Rgb{ 0, 0, 0 } : Rgb{ 255, 255, 255 };
        int ha = g_light ? 20 : 26;
        for (int y = inset; y < H - inset; y++)
            for (int x = 0; x < W; x++) {
                int dx = x < r ? r - x : (x >= W - r ? x - (W - r - 1) : 0);
                int dy = y < inset + r ? inset + r - y : (y >= H - inset - r ? y - (H - inset - r - 1) : 0);
                if (dx * dx + dy * dy <= r * r) cv.Blend(x, y, hc, ha);
            }
    }

    int x = Px(8);
    for (auto& p : pieces) {
        cv.Draw(p.m, x, (H - p.m.h) / 2, p.color, p.alpha);
        x += p.m.w + p.gapAfter;
    }

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader = { sizeof(BITMAPINFOHEADER), W, -H, 1, 32, BI_RGB };
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    memcpy(bits, cv.px.data(), (size_t)W * H * 4);
    HGDIOBJ ob = SelectObject(mem, bmp);
    g_size = { W, H };
    SIZE sz{ W, H };
    POINT src{ 0, 0 };
    BLENDFUNCTION bf{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_wnd, screen, nullptr, &sz, mem, &src, 0, &bf, ULW_ALPHA);
    SelectObject(mem, ob);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    g_lastW = -1;  // force a re-position with the new size
}

LRESULT CALLBACK WidgetProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEMOVE:
            if (!g_hover) {
                g_hover = true;
                TRACKMOUSEEVENT t{ sizeof(t), TME_LEAVE, h, 0 };
                TrackMouseEvent(&t);
                Paint();
                widget::Reposition();
            }
            return 0;
        case WM_MOUSELEAVE:
            g_hover = false;
            Paint();
            widget::Reposition();
            return 0;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            PostMessageW(g_notify, g_clickMsg, 0, 0);
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

}  // namespace

namespace widget {

bool Create(HINSTANCE inst, HWND notifyWnd, UINT clickMsg, bool lightTheme) {
    Destroy();
    g_taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!g_taskbar || !FindWindowExW(g_taskbar, nullptr, L"TrayNotifyWnd", nullptr)) { g_taskbar = nullptr; return false; }
    RECT tb;
    GetWindowRect(g_taskbar, &tb);
    if (tb.bottom - tb.top > tb.right - tb.left) { g_taskbar = nullptr; return false; }  // vertical taskbar: use tray icons
    // Windows 11's taskbar is a XAML island; on Windows 10 the task buttons live in ReBarWindow32
    // and have to be narrowed so they never slide under the widget.
    g_rebar = FindWindowExW(g_taskbar, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr)
                  ? nullptr
                  : FindWindowExW(g_taskbar, nullptr, L"ReBarWindow32", nullptr);
    g_notify = notifyWnd;
    g_clickMsg = clickMsg;
    g_light = lightTheme;

    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc{ sizeof(wc) };
        wc.lpfnWndProc = WidgetProc;
        wc.hInstance = inst;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kClass;
        registered = RegisterClassExW(&wc) != 0;
    }
    g_wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE, kClass, L"", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                            g_taskbar, nullptr, inst, nullptr);
    if (!g_wnd) { g_taskbar = nullptr; return false; }

    INITCOMMONCONTROLSEX icc{ sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);
    g_tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr, WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, 0, 0, 0, 0,
                            notifyWnd, nullptr, inst, nullptr);
    if (g_tip) {
        TTTOOLINFOW ti{ sizeof(ti) };
        ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        ti.hwnd = notifyWnd;
        ti.uId = (UINT_PTR)g_wnd;
        ti.lpszText = const_cast<wchar_t*>(ui::Text(L"Taskbar Wireless Battery Monitor", L"任务栏无线设备电量监控"));
        SendMessageW(g_tip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        SendMessageW(g_tip, TTM_SETMAXTIPWIDTH, 0, 600);
    }
    g_lastDpi = Dpi();
    Paint();
    g_lastW = -1;
    Reposition();
    ShowWindow(g_wnd, SW_SHOWNOACTIVATE);
    return true;
}

void Destroy() {
    if (g_rebar && IsWindow(g_rebar) && g_taskbar && IsWindow(g_taskbar)) {
        // Let Explorer lay the task buttons out again over the full width.
        RECT r;
        GetClientRect(g_taskbar, &r);
        PostMessageW(g_taskbar, WM_SIZE, SIZE_RESTORED, MAKELPARAM(r.right, r.bottom));
    }
    g_rebar = nullptr;
    if (g_tip) { DestroyWindow(g_tip); g_tip = nullptr; }
    if (g_wnd) { DestroyWindow(g_wnd); g_wnd = nullptr; }
    g_taskbar = nullptr;
    g_hover = false;
}

bool Alive() { return g_wnd && IsWindow(g_wnd) && g_taskbar && IsWindow(g_taskbar); }

void SetItems(const std::vector<WidgetItem>& items, const std::wstring& tooltip) {
    g_items = items;
    if (tooltip != g_tipText && g_tip) {
        g_tipText = tooltip;
        TTTOOLINFOW ti{ sizeof(ti) };
        ti.hwnd = g_notify;
        ti.uId = (UINT_PTR)g_wnd;
        ti.lpszText = const_cast<wchar_t*>(g_tipText.c_str());
        SendMessageW(g_tip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    }
    Paint();
    Reposition();
}

void SetLightTheme(bool light) {
    g_light = light;
    Paint();
}

bool Reposition() {
    if (!Alive()) return false;
    if (Dpi() != g_lastDpi) { g_lastDpi = Dpi(); Paint(); }
    HWND tray = FindWindowExW(g_taskbar, nullptr, L"TrayNotifyWnd", nullptr);
    if (!tray) return true;
    RECT tr, bar;
    GetWindowRect(tray, &tr);
    GetWindowRect(g_taskbar, &bar);
    POINT p{ tr.left, bar.top };
    ScreenToClient(g_taskbar, &p);
    int left = p.x - g_size.cx - Px(2);

    // Windows 10: Explorer re-expands the task buttons on its own layout passes; narrow them again.
    if (g_rebar && IsWindow(g_rebar)) {
        RECT rr;
        GetWindowRect(g_rebar, &rr);
        POINT tl{ rr.left, rr.top };
        ScreenToClient(g_taskbar, &tl);
        if (tl.x + (rr.right - rr.left) > left && left > tl.x)
            MoveWindow(g_rebar, tl.x, tl.y, left - tl.x, rr.bottom - rr.top, TRUE);
    }

    // Keep us above the taskbar's own content (our sibling windows).
    bool covered = GetWindow(g_wnd, GW_HWNDPREV) != nullptr;
    if (!covered && EqualRect(&tr, &g_lastTray) && EqualRect(&bar, &g_lastBar) && g_lastW == g_size.cx) return true;
    g_lastTray = tr;
    g_lastBar = bar;
    g_lastW = g_size.cx;
    SetWindowPos(g_wnd, HWND_TOP, left, 0, g_size.cx, g_size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    return true;
}

}  // namespace widget

}

namespace widget {
bool Create(HINSTANCE i, HWND w, UINT m, bool l) { return wb_render::widget::Create(i,w,m,l); }
void Destroy() { wb_render::widget::Destroy(); }
bool Alive() { return wb_render::widget::Alive(); }
void SetItems(const std::vector<WidgetItem>& i, const std::wstring& s) { wb_render::widget::SetItems(i,s); }
void SetLightTheme(bool b) { wb_render::widget::SetLightTheme(b); }
bool Reposition() { return wb_render::widget::Reposition(); }
}

namespace wb_app {
// Wireless Battery - tray app showing the battery level of wireless mice, headsets and Bluetooth devices.
namespace {

constexpr UINT WM_TRAY = WM_APP + 1;
constexpr UINT WM_RESULTS = WM_APP + 2;
constexpr UINT WM_WIDGET_CLICK = WM_APP + 3;
constexpr UINT_PTR TIMER_DEBOUNCE = 1;
constexpr UINT_PTR TIMER_WIDGET = 2;

constexpr UINT PLACEHOLDER_ID = 1;

enum MenuId : UINT { ID_REFRESH = 100, ID_DEVICE_FIRST = 1000 };

const wchar_t* kAppName = ui::Text(L"Taskbar Wireless Battery Monitor", L"任务栏无线设备电量监控");

HINSTANCE g_inst;
HWND g_hwnd;
HANDLE g_refreshEvt, g_stopEvt, g_worker, g_airpodsEvt;
UINT g_taskbarCreatedMsg;
bool g_lightTaskbar;
bool g_wantWidget = true;      // user's choice: taskbar widget (default) or tray icons
bool g_useWidget = false;      // what is actually shown right now
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
    if (!s.connected) return ui::Text(L"Off / disconnected", L"已关闭 / 未连接");
    std::wstring t;
    if (!s.levelText.empty() && s.percent >= 0) t = zh::Text(s.levelText);
    else if (s.percent >= 0) t = std::to_wstring(s.percent) + L"%";
    else if (e.lastPct >= 0) t = std::to_wstring(e.lastPct) + ui::Text(L"% (last known)", L"%（上次记录）");
    else t = ui::Text(L"Unknown", L"未知");
    if (s.charging) t += ui::Text(L", charging", L"，充电中");
    if (s.asleep) t += ui::Text(L", asleep", L"，休眠中");
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
        TrayCall(NIM_ADD, PLACEHOLDER_ID, g_placeholderIcon, ui::Text(L"Taskbar Wireless Battery Monitor\nNo readable battery devices found", L"任务栏无线设备电量监控\n未找到可读取电量的设备"));
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
    std::wstring tip = zh::Text(s.name) + L"\n" + StatusText(e);

    const wchar_t* infoTitle = nullptr;
    std::wstring info;
    if (s.charging || shown > ModConfig::warning.load() + 10) e.lowWarned = false;
    if (!s.charging && s.percent >= 0 && s.percent <= ModConfig::warning.load() && !e.lowWarned) {
        e.lowWarned = true;
        infoTitle = ui::Text(L"Low battery", L"电量不足");
        info = zh::Text(s.name) + ui::Text(L" battery remaining: ", L" 的剩余电量为 ") +
            (s.levelText.empty() ? std::to_wstring(s.percent) + L"%" : zh::Text(s.levelText));
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
        default:                   return 4;
    }
}

void UpdateWidget() {
    if (!g_useWidget) return;
    std::vector<const TrayEntry*> list;
    for (auto& [k, e] : g_entries)
        if (e.st.connected && !IsEntryHidden(e)) list.push_back(&e);
    std::sort(list.begin(), list.end(), [](const TrayEntry* a, const TrayEntry* b) {
        int ka = KindOrder(a->st.kind), kb = KindOrder(b->st.kind);
        return ka != kb ? ka < kb : a->st.name < b->st.name;
    });
    std::vector<WidgetItem> items;
    std::wstring tip;
    for (auto* e : list) {
        WidgetItem it;
        it.kind = e->st.kind;
        it.percent = e->st.percent >= 0 ? e->st.percent : e->lastPct;
        if (e->st.percent >= 0) it.text = zh::Text(e->st.levelText);
        it.charging = e->st.charging;
        it.dim = e->st.percent < 0;
        it.low = !it.charging && it.percent >= 0 && it.percent <= ModConfig::warning.load();
        it.bluetooth = e->st.via == L"Bluetooth";
        items.push_back(it);
        if (!tip.empty()) tip += L"\n";
        tip += zh::Text(e->st.name) + L": " + StatusText(*e);
    }
    if (tip.empty()) tip = ui::Text(L"Taskbar Wireless Battery Monitor: no readable battery devices found", L"任务栏无线设备电量监控：未找到可读取电量的设备");
    widget::SetItems(items, tip);
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

// Makes the actual display match the chosen mode; falls back to tray icons if the taskbar
// widget cannot be created (e.g. Explorer not ready yet). Called once a second.
void Reconcile() {
    bool alive = g_wantWidget && widget::Alive();
    if (g_wantWidget && !alive) alive = widget::Create(g_inst, g_hwnd, WM_WIDGET_CLICK, g_lightTaskbar);
    if (!g_wantWidget) widget::Destroy();
    bool changed = alive != g_useWidget;
    g_useWidget = alive;
    if (changed) {
        for (auto& [k, e] : g_entries) ApplyEntry(e);
        UpdatePlaceholder();
        UpdateWidget();
    } else if (g_useWidget) {
        widget::Reposition();
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
    if (g_entries.empty()) AppendMenuW(m, MF_STRING | MF_GRAYED, 0, ui::Text(L"No readable battery devices found", L"未找到可读取电量的设备"));
    else AppendMenuW(m, MF_STRING | MF_GRAYED, 0, ui::Text(L"Click a device to show or hide it", L"点击设备可显示或隐藏"));
    for (auto& [k, e] : g_entries) {
        std::wstring line = zh::Text(e.st.name) + L":  " + StatusText(e, false) + L"   [" + zh::Text(e.st.via) + L"]";
        AppendMenuW(m, MF_STRING | (IsHidden(k) ? 0 : MF_CHECKED), ID_DEVICE_FIRST + (UINT)keys.size(), line.c_str());
        keys.push_back(k);
    }
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, ID_REFRESH, ui::Text(L"Refresh now", L"立即刷新"));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | MF_GRAYED, 0, ui::Text(L"Configure display and enable/disable in Windhawk", L"显示设置和启停请在 Windhawk 中调整"));
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
        widget::Destroy();
        g_useWidget = false;
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
                if (light != g_lightTaskbar) { g_lightTaskbar = light; RedrawAll(); widget::SetLightTheme(light); }
            }
            return 0;
        case WM_DESTROY:
            KillTimer(h, TIMER_WIDGET);
            widget::Destroy();
            SetEvent(g_stopEvt);
            if (g_worker && WaitForSingleObject(g_worker, 5000) != WAIT_OBJECT_0) {
                // Dedicated tool host: exit instead of unloading code still running in a driver.
                ExitProcess(0);
            }
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
    g_inst = inst;

    // Shared with WirelessBattery.exe so the standalone app and mod cannot overlap.
    HANDLE mutex = CreateMutexW(nullptr, TRUE, L"Local\\TaskbarWirelessBatteryTray");
    if (!mutex) return 1;
    if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(mutex); return 0; }

    g_lightTaskbar = ReadLightTaskbar();
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.lpszClassName = L"TaskbarWirelessBatteryWnd";
    RegisterClassExW(&wc);
    // A hidden top-level window (not message-only) so it receives the TaskbarCreated broadcast.
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, kAppName, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (!g_hwnd) { ReleaseMutex(mutex); CloseHandle(mutex); return 1; }
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
    if (g_worker) CloseHandle(g_worker);
    ReleaseMutex(mutex);
    CloseHandle(mutex);
    CloseHandle(g_refreshEvt);
    CloseHandle(g_stopEvt);
    CloseHandle(g_airpodsEvt);
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
    UnregisterClassW(L"TaskbarWirelessBatteryWidget", module);
    UnregisterClassW(L"TaskbarWirelessBatteryWnd", module);
    Wh_Log(L"Battery UI thread exited, result=%d", result);
    return 0;
}
}

BOOL WhTool_ModInit() {
    for (PCWSTR name : {L"Local\\TaskbarWirelessBatteryTray", L"Local\\WirelessBatteryTray"}) {
        HANDLE existing = OpenMutexW(SYNCHRONIZE, FALSE, name);
        if (existing) {
            CloseHandle(existing);
            Wh_Log(L"Exit both the original and Chinese standalone WirelessBattery.exe before enabling this mod.");
            return FALSE;
        }
    }
    ModConfig::Load();
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModHost::Run), &ModHost::module)) return FALSE;
    ModHost::stopRequested = false;
    ModHost::uiThread = CreateThread(nullptr, 0, ModHost::Run, nullptr, 0, &ModHost::uiThreadId);
    if (!ModHost::uiThread) return FALSE;
    Wh_Log(L"Taskbar Wireless Battery Monitor started, host PID=%lu", GetCurrentProcessId());
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
        if (WaitForSingleObject(ModHost::uiThread, 5000) != WAIT_OBJECT_0) {
            // Only the dedicated -tool-mod process reaches this callback.
            // Never return to DLL unload with a live UI/driver thread.
            ExitProcess(0);
        }
        CloseHandle(ModHost::uiThread); ModHost::uiThread = nullptr;
    }
}

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
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
