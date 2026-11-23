// ==WindhawkMod==
// @id              quick-settings-remake
// @name            Quick Settings Remake
// @description     Per-app volume mixer, monitor brightness (DDC/CI), tile editing and a Sound page in Windows 11 Quick Settings
// @version         1.4.0
// @author          znbaboy
// @github          https://github.com/znbaboy
// @license         MIT
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lshlwapi -lshell32 -lgdi32 -lwinmm -ladvapi32 -ldwmapi -ldxva2
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Settings Remake

A full volume mixer, monitor brightness, a Sound page and tile editing, right
inside Windows 11 Quick Settings.

![Main page](https://raw.githubusercontent.com/znbaboy/windhawk-mods/quick-settings-remake-assets/main.png)

## Features
- **Volume mixer**: master volume plus a slider per app, with icons and live
  levels. Click an icon to mute. Apps fold behind **Show N apps**.
- **Monitor brightness**: a DDC/CI slider per external monitor (like Twinkle
  Tray). With HDR on, it sets SDR content brightness.
- **Brightness page**: combine monitors into one slider, choose which show,
  reorder them.
- **Sound page**: output and input devices, input volume and **Listen to this
  device**.
- **Edit tiles**: pin, unpin and re-pin tiles from the **Pinned** header or
  right-click.

| Brightness | Sound | Edit tiles |
|---|---|---|
| ![Brightness](https://raw.githubusercontent.com/znbaboy/windhawk-mods/quick-settings-remake-assets/brightness.png) | ![Sound](https://raw.githubusercontent.com/znbaboy/windhawk-mods/quick-settings-remake-assets/sound.png) | ![Edit tiles](https://raw.githubusercontent.com/znbaboy/windhawk-mods/quick-settings-remake-assets/edit.png) |

## Requirements
- Windows 11 24H2 or later.
- DDC/CI turned on in the monitor's menu for brightness. Laptop screens keep
  the Windows slider.

Everything is restored when the mod is disabled.

100% vibe coded.

## Troubleshooting
Turn on mod logs in Windhawk. A working setup logs `Mixer injected`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- anchorElement: SlidersGroup
  $name: Anchor element
  $description: x:Name of the Quick Settings element the mixer is placed under (default is the brightness/volume slider group)
- hideInactive: false
  $name: Hide apps that aren't playing audio
- hiddenApps: TextInputHost, ShellExperienceHost, ShellHost
  $name: Hidden apps
  $description: Comma-separated app names to leave out of the mixer
- compactTiles: true
  $name: Collapse tiles to one row when they fit
- hideFooter: true
  $name: Hide the bottom settings row
- monitorBrightness: true
  $name: Monitor brightness (DDC/CI)
  $description: Brightness sliders for external monitors, like Twinkle Tray
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <audioclient.h>
#include <audiopolicy.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <physicalmonitorenumerationapi.h>
#include <shellapi.h>
#include <shlwapi.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

// ============================================================================
// Overview
//
//  Discovery   ControlCenter.dll symbol hooks (Connect, OnGotFocus) -> XAML
//              root -> Grid#L1Grid > ContentControl#SlidersGroup (the anchor).
//  MixerPanel  Our UI, inserted as a new row in L1Grid below SlidersGroup.
//              The native volume row is measured once (for alignment) and
//              then hidden; our master row replaces it.
//  Audio       WASAPI: IAudioSessionManager2 for apps, IAudioEndpointVolume
//              for master/mic, peak meters for the level lines. AudioEvents
//              turns WASAPI notifications into UI-thread refreshes.
//  Brightness  DDC/CI (or SDR white level with HDR on) on one worker thread.
//  Tiles       The tile GridView shows a filtered copy of Windows' list
//              (unpinned names persist in mod storage).
//  Threading   All XAML and audio-UI work runs on the Quick Settings UI
//              thread; cross-thread entry points go through g_uiQueue.
//  Restore     Every native change is recorded and undone in Destroy().
// ============================================================================

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = wux::Controls;
namespace wuxm = wux::Media;
using wux::Thickness;
using wux::Visibility;
using wux::Automation::AutomationProperties;
using wuxm::VisualTreeHelper;
using TileList = wf::Collections::IObservableVector<wf::IInspectable>;


std::atomic<bool> g_unloading{false};
winrt::Windows::System::DispatcherQueue g_uiQueue{nullptr};  // Quick Settings UI thread

// ============================================================================
// Settings and storage
// ============================================================================

std::vector<std::wstring> Split(std::wstring_view s, wchar_t sep, bool trim = false) {
    std::vector<std::wstring> out;
    for (size_t start = 0; start <= s.size();) {
        size_t end = std::min(s.find(sep, start), s.size());
        auto item = s.substr(start, end - start);
        if (trim) {
            size_t a = item.find_first_not_of(L" \t");
            item = a == item.npos ? std::wstring_view{} : item.substr(a, item.find_last_not_of(L" \t") - a + 1);
        }
        if (!item.empty()) out.emplace_back(item);
        start = end + 1;
    }
    return out;
}

struct {
    std::wstring anchorName;
    bool hideInactive = false;
    std::vector<std::wstring> hiddenApps;
    bool compactTiles = true;
    bool hideFooter = true;
    bool brightness = true;
} g_settings;

std::wstring StringSetting(PCWSTR key) {
    PCWSTR s = Wh_GetStringSetting(key);
    std::wstring value = s ? s : L"";
    Wh_FreeStringSetting(s);
    return value;
}

void LoadSettings() {
    g_settings.anchorName = StringSetting(L"anchorElement");
    g_settings.hideInactive = Wh_GetIntSetting(L"hideInactive") != 0;
    g_settings.hiddenApps = Split(StringSetting(L"hiddenApps"), L',', true);
    g_settings.compactTiles = Wh_GetIntSetting(L"compactTiles") != 0;
    g_settings.hideFooter = Wh_GetIntSetting(L"hideFooter") != 0;
    g_settings.brightness = Wh_GetIntSetting(L"monitorBrightness") != 0;
}

// '|'-separated lists in the mod's storage (unpinned tiles, tile and monitor order...).
std::vector<std::wstring> LoadList(PCWSTR key) {
    WCHAR buf[4096] = {};
    Wh_GetStringValue(key, buf, ARRAYSIZE(buf));
    return Split(buf, L'|');
}

void SaveList(PCWSTR key, const std::vector<std::wstring>& items) {
    std::wstring s;
    for (auto& i : items) s += (s.empty() ? L"" : L"|") + i;
    Wh_SetStringValue(key, s.c_str());
}

// ============================================================================
// Monitor brightness over DDC/CI (like Twinkle Tray)
//
// DDC/CI calls take ~40-100 ms each, so they all run on one worker thread that
// owns the physical monitor handles. Slider moves are coalesced: only the
// latest value per monitor is sent once the previous write has finished.
// Built-in laptop panels are skipped (Windows' own brightness slider covers them).
// ============================================================================

struct MonitorState {
    std::wstring key;   // stable id: monitor device path
    std::wstring name;  // EDID friendly name, e.g. "MPG321URX"
    int value = -1;     // brightness 0-100, -1 = unknown
    bool ddc = false;   // brightness readable over DDC/CI
    bool hdr = false;   // HDR on: the slider sets SDR content brightness instead
    bool Controllable() const { return ddc || hdr; }
};

struct DisplayTarget {
    std::wstring gdi, name, path;
    bool internal = false;
    LUID adapter{};
    UINT32 id = 0;
};

// HDR and SDR content brightness ("SDR white level", Settings > Display > HDR).
// Own struct copies: older SDK/mingw headers lack some of these.
struct AdvColorInfo2 {  // DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO_2 (Win11 24H2+)
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    UINT32 flags;
    UINT32 colorEncoding;
    UINT32 bitsPerColorChannel;
    UINT32 activeColorMode;  // 0 SDR, 1 WCG, 2 HDR
};
struct AdvColorInfo {  // DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    UINT32 flags;  // bit 1 enabled, bit 2 wide color enforced (ACM, not HDR)
    UINT32 colorEncoding;
    UINT32 bitsPerColorChannel;
};
struct SdrWhiteLevel {  // DISPLAYCONFIG_SDR_WHITE_LEVEL: 1000 = 80 nits
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    ULONG level;
};
struct SetSdrWhiteLevel {  // undocumented setter used by Settings (type 0xFFFFFFEE)
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    ULONG level;
    BYTE finalValue;
};

template <class T>
T DisplayInfo(UINT32 type, LUID adapter, UINT32 id) {
    T info{};
    info.header = {(DISPLAYCONFIG_DEVICE_INFO_TYPE)type, sizeof(T), adapter, id};
    return info;
}

bool IsHdrOn(LUID adapter, UINT32 id) {
    auto i2 = DisplayInfo<AdvColorInfo2>(15, adapter, id);
    if (DisplayConfigGetDeviceInfo(&i2.header) == ERROR_SUCCESS) return i2.activeColorMode == 2;
    auto i1 = DisplayInfo<AdvColorInfo>(9, adapter, id);
    if (DisplayConfigGetDeviceInfo(&i1.header) != ERROR_SUCCESS) return false;
    return (i1.flags & 2) && !(i1.flags & 4);
}

// Slider 0-100 = 80-480 nits, the same range as the Settings slider.
int GetSdrBrightness(LUID adapter, UINT32 id) {
    auto w = DisplayInfo<SdrWhiteLevel>(11, adapter, id);
    if (DisplayConfigGetDeviceInfo(&w.header) != ERROR_SUCCESS) return -1;
    return std::clamp((int)std::lround(((double)w.level - 1000.0) / 50.0), 0, 100);
}

void SetSdrBrightness(LUID adapter, UINT32 id, int value) {
    auto w = DisplayInfo<SetSdrWhiteLevel>(0xFFFFFFEE, adapter, id);
    w.level = 1000 + std::clamp(value, 0, 100) * 50;
    w.finalValue = 1;
    DisplayConfigSetDeviceInfo(&w.header);
}

// Active display paths: GDI name (\\.\DISPLAY1) -> monitor name and device path.
std::vector<DisplayTarget> QueryDisplayTargets() {
    std::vector<DisplayTarget> out;
    UINT32 np = 0, nm = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &np, &nm) != ERROR_SUCCESS) return out;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(np);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(nm);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &np, paths.data(), &nm, modes.data(), nullptr) != ERROR_SUCCESS)
        return out;
    paths.resize(np);
    for (auto& p : paths) {
        auto src = DisplayInfo<DISPLAYCONFIG_SOURCE_DEVICE_NAME>(DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
                                                                 p.sourceInfo.adapterId, p.sourceInfo.id);
        auto tgt = DisplayInfo<DISPLAYCONFIG_TARGET_DEVICE_NAME>(DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME,
                                                                 p.targetInfo.adapterId, p.targetInfo.id);
        if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS ||
            DisplayConfigGetDeviceInfo(&tgt.header) != ERROR_SUCCESS)
            continue;
        auto tech = tgt.outputTechnology;
        out.push_back({src.viewGdiDeviceName, tgt.monitorFriendlyDeviceName, tgt.monitorDevicePath,
                       tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                           tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                           tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED,
                       p.targetInfo.adapterId, p.targetInfo.id});
    }
    return out;
}

// Cheap check for monitors being plugged, unplugged or rearranged.
std::wstring MonitorSignature() {
    std::wstring sig;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR h, HDC, LPRECT r, LPARAM lp) -> BOOL {
            MONITORINFOEXW mi{};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(h, &mi))
                *(std::wstring*)lp += mi.szDevice + (L':' + std::to_wstring(r->left)) + L',' +
                                      std::to_wstring(r->top) + L';';
            return TRUE;
        },
        (LPARAM)&sig);
    return sig;
}

void NotifyMonitorsChanged();  // worker thread -> UI thread

class DdcWorker {
   public:
    void Start() {
        std::lock_guard lock(m);
        if (thread.joinable()) return;
        stop = false;
        thread = std::thread([this] { Run(); });
    }
    void Stop() {
        {
            std::lock_guard lock(m);
            stop = true;
        }
        cv.notify_all();
        if (thread.joinable()) thread.join();
    }
    // Re-enumerate monitors and read their brightness.
    void Refresh() {
        {
            std::lock_guard lock(m);
            refresh = true;
        }
        cv.notify_all();
    }
    void Set(const std::wstring& key, int value) {
        {
            std::lock_guard lock(m);
            pending[key] = value;
            for (auto& s : state)
                if (s.key == key) s.value = value;
        }
        cv.notify_all();
    }
    std::vector<MonitorState> Snapshot() {
        std::lock_guard lock(m);
        return state;
    }

   private:
    struct Handle {
        std::wstring key;
        HANDLE h;
        DWORD max;
        bool hdr;
        LUID adapter;
        UINT32 id;
    };

    void Run() {
        std::unique_lock lock(m);
        while (true) {
            cv.wait(lock, [this] { return stop || refresh || !pending.empty(); });
            if (stop) break;
            bool enumerate = std::exchange(refresh, false);
            auto work = enumerate ? decltype(pending){} : std::exchange(pending, {});
            lock.unlock();
            if (enumerate) {
                Enumerate();
                NotifyMonitorsChanged();
            }
            for (auto& [key, v] : work)
                for (auto& h : handles) {
                    if (h.key != key) continue;
                    if (h.hdr) SetSdrBrightness(h.adapter, h.id, v);
                    else SetVCPFeature(h.h, 0x10, (DWORD)std::lround(v * h.max / 100.0));
                }
            lock.lock();
        }
        lock.unlock();
        Release();
    }

    void Release() {
        for (auto& h : handles) DestroyPhysicalMonitor(h.h);
        handles.clear();
    }

    void Enumerate() {
        Release();
        auto targets = QueryDisplayTargets();
        std::vector<HMONITOR> monitors;
        EnumDisplayMonitors(
            nullptr, nullptr,
            [](HMONITOR h, HDC, LPRECT, LPARAM lp) -> BOOL {
                ((std::vector<HMONITOR>*)lp)->push_back(h);
                return TRUE;
            },
            (LPARAM)&monitors);

        std::vector<MonitorState> found;
        for (HMONITOR hm : monitors) {
            MONITORINFOEXW mi{};
            mi.cbSize = sizeof(mi);
            if (!GetMonitorInfoW(hm, &mi)) continue;
            std::vector<const DisplayTarget*> mine;  // >1 when duplicated (clone mode)
            for (auto& t : targets)
                if (_wcsicmp(t.gdi.c_str(), mi.szDevice) == 0) mine.push_back(&t);
            DWORD n = 0;
            if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hm, &n) || !n) continue;
            std::vector<PHYSICAL_MONITOR> phys(n);
            if (!GetPhysicalMonitorsFromHMONITOR(hm, n, phys.data())) continue;
            for (DWORD i = 0; i < n; i++) {
                const DisplayTarget* t = i < mine.size() ? mine[i] : nullptr;
                HANDLE h = phys[i].hPhysicalMonitor;
                if (t && t->internal) {
                    DestroyPhysicalMonitor(h);
                    continue;
                }
                MonitorState s;
                s.key = t && !t->path.empty() ? t->path : std::wstring(mi.szDevice) + L"#" + std::to_wstring(i);
                s.name = t && !t->name.empty() ? t->name
                         : phys[i].szPhysicalMonitorDescription[0] ? phys[i].szPhysicalMonitorDescription
                                                                    : L"Display";
                s.hdr = t && IsHdrOn(t->adapter, t->id);
                DWORD cur = 0, max = 100;
                if (s.hdr) {
                    s.value = GetSdrBrightness(t->adapter, t->id);
                } else {
                    for (int attempt = 0; attempt < 2 && !s.ddc; attempt++) {  // DDC reads fail now and then
                        s.ddc = GetVCPFeatureAndVCPFeatureReply(h, 0x10, nullptr, &cur, &max) && max > 0;
                        if (!s.ddc) Sleep(40);
                    }
                    if (s.ddc) s.value = (int)std::lround(std::min(cur, max) * 100.0 / max);
                    else max = 100;
                }
                handles.push_back({s.key, h, max, s.hdr, t ? t->adapter : LUID{}, t ? t->id : 0});
                found.push_back(s);
            }
        }
        // Same model twice: "Name", "Name (2)".
        for (size_t i = 0; i < found.size(); i++) {
            int same = 1;
            for (size_t j = 0; j < i; j++)
                if (found[j].name == found[i].name || found[j].name.rfind(found[i].name + L" (", 0) == 0) same++;
            if (same > 1) found[i].name += L" (" + std::to_wstring(same) + L")";
        }
        std::lock_guard lock(m);
        // Keep values the user set while we were reading.
        for (auto& f : found)
            if (auto it = pending.find(f.key); it != pending.end()) f.value = it->second;
        state = std::move(found);
    }

    std::thread thread;
    std::mutex m;
    std::condition_variable cv;
    bool stop = false, refresh = false;
    std::map<std::wstring, int> pending;  // latest value per monitor, not yet sent
    std::vector<MonitorState> state;      // guarded by m
    std::vector<Handle> handles;          // worker thread only
};

DdcWorker g_ddc;

// ============================================================================
// Audio helpers (WASAPI) — UI thread only
// ============================================================================

static const PROPERTYKEY kPKEY_Device_FriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};

// Undocumented IPolicyConfig (Win7+), used to set the default endpoint.
static const CLSID kCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    // bFxStore: FALSE = the endpoint's own property store, TRUE = its effects (FX) store.
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, BOOL bFxStore, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, BOOL bFxStore, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR wszDeviceId, ERole eRole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// IAudioMeterInformation, declared here because the toolchain's headers only
// forward-declare it. Always queried with the explicit IID.
static const IID kIID_AudioMeter = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};
struct AudioMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};

winrt::com_ptr<IMMDeviceEnumerator> g_enumerator;  // one for the mod's lifetime

IMMDeviceEnumerator* Enumerator() {
    if (!g_enumerator)
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                         g_enumerator.put_void());
    return g_enumerator.get();
}

winrt::com_ptr<IMMDevice> DefaultDevice(EDataFlow flow) {
    winrt::com_ptr<IMMDevice> dev;
    if (auto en = Enumerator()) en->GetDefaultAudioEndpoint(flow, eMultimedia, dev.put());
    return dev;
}

winrt::com_ptr<IMMDevice> DeviceById(const std::wstring& id) {
    winrt::com_ptr<IMMDevice> dev;
    if (auto en = Enumerator(); en && !id.empty()) en->GetDevice(id.c_str(), dev.put());
    return dev;
}

std::wstring DeviceId(winrt::com_ptr<IMMDevice> const& dev) {
    std::wstring id;
    LPWSTR s = nullptr;
    if (dev && SUCCEEDED(dev->GetId(&s)) && s) {
        id = s;
        CoTaskMemFree(s);
    }
    return id;
}

template <class T>
winrt::com_ptr<T> Activate(winrt::com_ptr<IMMDevice> const& dev, REFIID iid = __uuidof(T)) {
    winrt::com_ptr<T> out;
    if (dev) dev->Activate(iid, CLSCTX_ALL, nullptr, out.put_void());
    return out;
}

// ---------------------------------------------------------------------------
// Audio change notifications
//
// One object receives every WASAPI callback for the watched output device and
// only posts "what changed" to the UI thread (coalesced), which then refreshes
// the same way the timer used to. Callbacks arrive on audio threads.
//   IMMNotificationClient        default device / device list
//   IAudioEndpointVolumeCallback master volume and mute
//   IAudioSessionNotification    new app session
//   IAudioSessionEvents          app volume, mute, state, session end
// Session-created notifications are only delivered when the session manager
// is set up on an MTA thread, so everything is registered on a short-lived
// worker thread (the UI thread is an STA).
// ---------------------------------------------------------------------------

enum AudioChange { kDeviceChanged = 1, kMasterChanged = 2, kSessionsChanged = 4, kAppVolumeChanged = 8, kAudioAll = 15 };
void NotifyAudioChanged(int what);  // any thread -> UI thread

class AudioEvents final : public IMMNotificationClient,
                          public IAudioEndpointVolumeCallback,
                          public IAudioSessionNotification,
                          public IAudioSessionEvents {
   public:
    // UI thread. Watches this output device (and the device list); no-op when already watching it.
    void Watch(const std::wstring& id) {
        if (watching && id == deviceId) return;
        Stop();
        watching = true;
        deviceId = id;
        if (!mtaCookie) CoIncrementMTAUsage(&mtaCookie);  // keeps the MTA (and our registrations) alive
        arming = true;
        thread = std::thread([this, id] { Arm(id); });
    }
    // UI thread. Joins the arming worker once it has finished.
    void Reap() {
        if (thread.joinable() && !arming) thread.join();
    }
    // UI thread. Unregistering waits for callbacks that are still running.
    void Stop() {
        if (thread.joinable()) thread.join();
        watching = false;
        Registrations old;
        {
            std::lock_guard lock(m);
            armed = false;
            old = std::move(reg);
            reg = {};
        }
        if (old.manager) old.manager->UnregisterSessionNotification(this);
        for (auto& s : old.sessions) s->UnregisterAudioSessionNotification(this);
        if (old.volume) old.volume->UnregisterControlChangeNotify(this);
        if (old.enumerator) old.enumerator->UnregisterEndpointNotificationCallback(this);
    }
    void ReleaseMta() {
        if (mtaCookie) CoDecrementMTAUsage(mtaCookie);
        mtaCookie = nullptr;
    }

   private:
    struct Registrations {
        winrt::com_ptr<IMMDeviceEnumerator> enumerator;
        winrt::com_ptr<IAudioEndpointVolume> volume;
        winrt::com_ptr<IAudioSessionManager2> manager;
        std::vector<winrt::com_ptr<IAudioSessionControl>> sessions;
    };

    void Arm(const std::wstring& id) {  // worker thread (MTA)
        bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
        {
            Registrations r;
            winrt::com_ptr<IMMDevice> dev;
            CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                             r.enumerator.put_void());
            if (r.enumerator && !id.empty()) r.enumerator->GetDevice(id.c_str(), dev.put());
            r.volume = Activate<IAudioEndpointVolume>(dev);
            r.manager = Activate<IAudioSessionManager2>(dev);

            std::lock_guard lock(m);
            armed = true;
            reg = std::move(r);
            if (reg.enumerator) reg.enumerator->RegisterEndpointNotificationCallback(this);
            if (reg.volume) reg.volume->RegisterControlChangeNotify(this);
            winrt::com_ptr<IAudioSessionEnumerator> list;
            int count = 0;
            // Enumerating once is what starts the session-created notifications.
            if (reg.manager && SUCCEEDED(reg.manager->RegisterSessionNotification(this)) &&
                SUCCEEDED(reg.manager->GetSessionEnumerator(list.put())) && SUCCEEDED(list->GetCount(&count)))
                for (int i = 0; i < count; i++) {
                    winrt::com_ptr<IAudioSessionControl> s;
                    if (SUCCEEDED(list->GetSession(i, s.put()))) AddSession(s);
                }
        }
        if (com) CoUninitialize();
        arming = false;
        NotifyAudioChanged(kAudioAll);  // catch up on anything that changed while arming
    }

    // Caller holds m. Also drops sessions that have ended.
    void AddSession(winrt::com_ptr<IAudioSessionControl> const& session) {
        std::erase_if(reg.sessions, [this](auto& s) {
            AudioSessionState state;
            bool ended = FAILED(s->GetState(&state)) || state == AudioSessionStateExpired;
            if (ended) s->UnregisterAudioSessionNotification(this);
            return ended;
        });
        if (SUCCEEDED(session->RegisterAudioSessionNotification(this))) reg.sessions.push_back(session);
    }

    // IUnknown: a global object, never destroyed by reference count.
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IMMNotificationClient)) *out = static_cast<IMMNotificationClient*>(this);
        else if (iid == __uuidof(IAudioEndpointVolumeCallback)) *out = static_cast<IAudioEndpointVolumeCallback*>(this);
        else if (iid == __uuidof(IAudioSessionNotification)) *out = static_cast<IAudioSessionNotification*>(this);
        else if (iid == __uuidof(IAudioSessionEvents)) *out = static_cast<IAudioSessionEvents*>(this);
        else return *out = nullptr, E_NOINTERFACE;
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 2; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }

    // IMMNotificationClient
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) NotifyAudioChanged(kDeviceChanged);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return NotifyAudioChanged(kDeviceChanged), S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }

    // IAudioEndpointVolumeCallback
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override { return NotifyAudioChanged(kMasterChanged), S_OK; }

    // IAudioSessionNotification
    HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* session) override {
        {
            winrt::com_ptr<IAudioSessionControl> s;
            s.copy_from(session);
            std::lock_guard lock(m);
            if (armed && s) AddSession(s);
        }
        return NotifyAudioChanged(kSessionsChanged), S_OK;
    }

    // IAudioSessionEvents
    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float, BOOL, LPCGUID) override { return NotifyAudioChanged(kAppVolumeChanged), S_OK; }
    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState) override { return NotifyAudioChanged(kSessionsChanged), S_OK; }
    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason) override { return NotifyAudioChanged(kSessionsChanged), S_OK; }
    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID, LPCGUID) override { return S_OK; }

    std::thread thread;                // arming worker
    std::atomic<bool> arming{false};
    std::wstring deviceId;             // UI thread only
    bool watching = false;             // UI thread only
    CO_MTA_USAGE_COOKIE mtaCookie = nullptr;
    std::mutex m;
    bool armed = false;                // guarded by m
    Registrations reg;                 // guarded by m
};

AudioEvents g_audioEvents;

struct AudioDevice {
    std::wstring id, name;
};

// Active devices of one direction.
std::vector<AudioDevice> ListDevices(EDataFlow flow) {
    std::vector<AudioDevice> out;
    auto en = Enumerator();
    winrt::com_ptr<IMMDeviceCollection> coll;
    if (!en || FAILED(en->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, coll.put()))) return out;
    UINT count = 0;
    coll->GetCount(&count);
    for (UINT i = 0; i < count; i++) {
        winrt::com_ptr<IMMDevice> dev;
        if (FAILED(coll->Item(i, dev.put()))) continue;
        AudioDevice d{DeviceId(dev), flow == eRender ? L"Speakers" : L"Microphone"};
        winrt::com_ptr<IPropertyStore> props;
        if (SUCCEEDED(dev->OpenPropertyStore(STGM_READ, props.put()))) {
            PROPVARIANT pv;
            PropVariantInit(&pv);
            if (SUCCEEDED(props->GetValue(kPKEY_Device_FriendlyName, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal &&
                *pv.pwszVal)
                d.name = pv.pwszVal;
            PropVariantClear(&pv);
        }
        out.push_back(std::move(d));
    }
    return out;
}

void SetDefaultDevice(const std::wstring& id) {
    winrt::com_ptr<IPolicyConfig> pc;
    if (FAILED(CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL, kIID_IPolicyConfig, pc.put_void())) ||
        !pc)
        return;
    for (ERole r : {eConsole, eMultimedia, eCommunications}) pc->SetDefaultEndpoint(id.c_str(), r);
}

// "Listen to this device" (classic Sound panel > Recording > Properties > Listen):
// two properties of the capture endpoint, written through IPolicyConfig like
// the Sound panel does (no admin needed).
static const PROPERTYKEY kPKEY_ListenTarget = {  // VT_LPWSTR playback device id, empty = default device
    {0x24dbb0fc, 0x9311, 0x4b3d, {0x9c, 0xf0, 0x18, 0xff, 0x15, 0x56, 0x39, 0xd4}}, 0};
static const PROPERTYKEY kPKEY_ListenEnabled = {  // VT_BOOL
    {0x24dbb0fc, 0x9311, 0x4b3d, {0x9c, 0xf0, 0x18, 0xff, 0x15, 0x56, 0x39, 0xd4}}, 1};

struct ListenState {
    bool on = false;
    std::wstring target;  // empty = default playback device
};

ListenState GetListen(winrt::com_ptr<IMMDevice> const& mic) {
    ListenState st;
    winrt::com_ptr<IPropertyStore> props;
    if (!mic || FAILED(mic->OpenPropertyStore(STGM_READ, props.put()))) return st;
    PROPVARIANT pv;
    PropVariantInit(&pv);
    if (SUCCEEDED(props->GetValue(kPKEY_ListenEnabled, &pv)) && pv.vt == VT_BOOL) st.on = pv.boolVal != VARIANT_FALSE;
    PropVariantClear(&pv);
    if (SUCCEEDED(props->GetValue(kPKEY_ListenTarget, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) st.target = pv.pwszVal;
    PropVariantClear(&pv);
    return st;
}

bool SetListen(const std::wstring& micId, ListenState const& st) {
    winrt::com_ptr<IPolicyConfig> pc;
    if (micId.empty() ||
        FAILED(CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL, kIID_IPolicyConfig, pc.put_void())) ||
        !pc)
        return false;
    PROPVARIANT target;
    PropVariantInit(&target);
    target.vt = VT_LPWSTR;
    target.pwszVal = const_cast<LPWSTR>(st.target.c_str());  // not owned: no PropVariantClear
    PROPVARIANT enabled;
    PropVariantInit(&enabled);
    enabled.vt = VT_BOOL;
    enabled.boolVal = st.on ? VARIANT_TRUE : VARIANT_FALSE;
    HRESULT h1 = pc->SetPropertyValue(micId.c_str(), FALSE, kPKEY_ListenTarget, &target);  // target first
    HRESULT h2 = pc->SetPropertyValue(micId.c_str(), FALSE, kPKEY_ListenEnabled, &enabled);
    if (FAILED(h1) || FAILED(h2)) Wh_Log(L"Listen to this device: %08X %08X", h1, h2);
    return SUCCEEDED(h2);
}

// ---------------------------------------------------------------------------
// App sessions
// ---------------------------------------------------------------------------

bool IsProcessAlive(DWORD pid) {
    HANDLE h = pid ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid) : nullptr;
    DWORD code = 0;
    bool alive = h && GetExitCodeProcess(h, &code) && code == STILL_ACTIVE;
    if (h) CloseHandle(h);
    return alive;
}

// Full exe path, or empty when the process can't be opened.
std::wstring ExePath(DWORD pid) {
    WCHAR path[MAX_PATH * 2] = {};
    if (HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        DWORD size = ARRAYSIZE(path);
        if (!QueryFullProcessImageNameW(h, 0, path, &size)) path[0] = 0;
        CloseHandle(h);
    }
    return path;
}

// "C:\...\chrome.exe" -> "chrome"
std::wstring ExeName(const std::wstring& path) {
    PCWSTR file = PathFindFileNameW(path.c_str());
    return {file, (PCWSTR)PathFindExtensionW(file)};
}

// The exe's FileDescription (e.g. "Google Chrome", "Spotify"), or empty.
std::wstring FileDescription(const std::wstring& path) {
    DWORD dummy = 0, len = GetFileVersionInfoSizeW(path.c_str(), &dummy);
    std::vector<BYTE> buf(len);
    struct LangCp {
        WORD lang, cp;
    }* tr = nullptr;
    UINT n = 0;
    if (!len || !GetFileVersionInfoW(path.c_str(), 0, len, buf.data()) ||
        !VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (void**)&tr, &n) || n < sizeof(LangCp))
        return L"";
    WCHAR sub[64];
    swprintf_s(sub, L"\\StringFileInfo\\%04x%04x\\FileDescription", tr->lang, tr->cp);
    LPWSTR desc = nullptr;
    return VerQueryValueW(buf.data(), sub, (void**)&desc, &n) && n > 1 && desc ? desc : L"";
}

// Session display name (may be an "@dll,-id" resource reference), or empty.
std::wstring SessionDisplayName(IAudioSessionControl* ctl) {
    std::wstring label;
    LPWSTR dn = nullptr;
    if (SUCCEEDED(ctl->GetDisplayName(&dn)) && dn && *dn) {
        WCHAR out[256];
        if (dn[0] != L'@') label = dn;
        else if (SUCCEEDED(SHLoadIndirectString(dn, out, ARRAYSIZE(out), nullptr))) label = out;
    }
    if (dn) CoTaskMemFree(dn);
    return label;
}

struct AppGroup {
    DWORD key = 0;
    bool muted = false;
    std::wstring exePath, label;
    float volume = 1.0f;
    std::vector<winrt::com_ptr<ISimpleAudioVolume>> vols;
    std::vector<winrt::com_ptr<AudioMeter>> meters;
};

// Per-process info is expensive (process handle, exe version resource), so it
// is resolved once per pid and reused on every refresh.
struct AppInfo {
    std::wstring label, exePath;
    bool hidden = false;
};
std::map<DWORD, AppInfo> g_appInfo;  // cleared when settings change

AppInfo ResolveApp(IAudioSessionControl* ctl, DWORD pid) {
    AppInfo info;
    info.exePath = ExePath(pid);
    std::wstring exe = ExeName(info.exePath);
    info.label = SessionDisplayName(ctl);
    if (info.label.empty() && !info.exePath.empty()) info.label = FileDescription(info.exePath);
    if (info.label.empty()) info.label = exe.empty() ? L"App " + std::to_wstring(pid) : exe;
    for (auto& h : g_settings.hiddenApps)
        if (_wcsicmp(h.c_str(), info.label.c_str()) == 0 || _wcsicmp(h.c_str(), exe.c_str()) == 0) info.hidden = true;
    return info;
}

// One group per program, in Windows' own session order (same as the native mixer).
std::vector<AppGroup> GetAppSessions(winrt::com_ptr<IMMDevice> const& dev) {
    std::vector<AppGroup> groups;
    std::map<DWORD, AppInfo> seen;
    auto mgr = Activate<IAudioSessionManager2>(dev);
    winrt::com_ptr<IAudioSessionEnumerator> sessions;
    if (!mgr || FAILED(mgr->GetSessionEnumerator(sessions.put()))) return groups;

    int count = 0;
    sessions->GetCount(&count);
    for (int i = 0; i < count; i++) {
        winrt::com_ptr<IAudioSessionControl> ctl;
        if (FAILED(sessions->GetSession(i, ctl.put()))) continue;
        auto ctl2 = ctl.try_as<IAudioSessionControl2>();
        auto vol = ctl.try_as<ISimpleAudioVolume>();
        if (!ctl2 || !vol) continue;

        AudioSessionState state;
        if (FAILED(ctl->GetState(&state)) || state == AudioSessionStateExpired) continue;
        if (g_settings.hideInactive && state != AudioSessionStateActive) continue;
        if (ctl2->IsSystemSoundsSession() == S_OK) continue;  // covered by the master slider

        DWORD pid = 0;
        ctl2->GetProcessId(&pid);
        if (!IsProcessAlive(pid)) continue;

        auto cached = g_appInfo.find(pid);
        auto& info = seen[pid] = cached != g_appInfo.end() ? cached->second : ResolveApp(ctl.get(), pid);
        if (info.hidden) continue;

        // One row per program: apps like Discord run several audio processes
        // (voice, embedded video...). Group by exe path; pid only if unknown.
        DWORD key = pid;
        if (!info.exePath.empty()) {
            std::wstring lower = info.exePath;
            CharLowerBuffW(lower.data(), (DWORD)lower.size());
            key = (DWORD)std::hash<std::wstring>{}(lower);
        }
        auto it = std::find_if(groups.begin(), groups.end(), [key](const AppGroup& g) { return g.key == key; });
        if (it == groups.end()) {
            AppGroup g;
            g.key = key;
            g.label = info.label;
            g.exePath = info.exePath;
            vol->GetMasterVolume(&g.volume);
            BOOL m = FALSE;
            if (SUCCEEDED(vol->GetMute(&m))) g.muted = m != FALSE;
            groups.push_back(std::move(g));
            it = groups.end() - 1;
        }
        it->vols.push_back(vol);
        winrt::com_ptr<AudioMeter> meter;
        if (SUCCEEDED(ctl->QueryInterface(kIID_AudioMeter, meter.put_void())) && meter) it->meters.push_back(meter);
    }
    g_appInfo = std::move(seen);  // drop processes that went away
    return groups;
}

// ============================================================================
// XAML helpers — UI thread only
// ============================================================================

using wux::HorizontalAlignment;
using wux::VerticalAlignment;

const wux::GridLength kStar{1, wux::GridUnitType::Star}, kAuto{1, wux::GridUnitType::Auto};
wux::GridLength Px(double v) { return {v, wux::GridUnitType::Pixel}; }

template <class T>
T Resource(PCWSTR key) {
    try {
        auto k = winrt::box_value(key);
        if (auto app = wux::Application::Current())
            if (auto res = app.Resources(); res && res.HasKey(k)) return res.Lookup(k).try_as<T>();
    } catch (...) {
    }
    return nullptr;
}

wuxm::Brush TransparentBrush() { return wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()); }

// Quick Settings follows the system (taskbar) theme.
bool SystemUsesDarkTheme() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

// A theme resource brush, or a fixed ARGB fallback when the resource is missing.
wuxm::Brush ThemeBrush(PCWSTR key, uint32_t dark, uint32_t light) {
    if (auto b = Resource<wuxm::Brush>(key)) return b;
    uint32_t c = SystemUsesDarkTheme() ? dark : light;
    return wuxm::SolidColorBrush(winrt::Windows::UI::ColorHelper::FromArgb(c >> 24, c >> 16, c >> 8, c));
}
wuxm::Brush AccentBrush() { return ThemeBrush(L"SystemControlHighlightAccentBrush", 0xFF4CC2FF, 0xFF4CC2FF); }

// Font of the tile captions ("Bluetooth", "Night light"), copied at runtime so
// the mixer labels match them exactly.
struct {
    bool valid = false;
    wuxm::FontFamily family{nullptr};
    double size = 14;
    winrt::Windows::UI::Text::FontWeight weight{400};
    wuxm::Brush foreground{nullptr};
} g_tileFont;

void ApplyTileFont(wuxc::TextBlock const& tb, bool secondary) {
    if (g_tileFont.valid) {
        if (g_tileFont.family) tb.FontFamily(g_tileFont.family);
        tb.FontSize(g_tileFont.size);
        tb.FontWeight(g_tileFont.weight);
        if (!secondary && g_tileFont.foreground) tb.Foreground(g_tileFont.foreground);
    } else {
        tb.FontSize(12);
    }
    if (secondary || !g_tileFont.valid) {
        if (auto br = Resource<wuxm::Brush>(L"TextFillColorSecondaryBrush")) tb.Foreground(br);
        else tb.Opacity(0.78);
    }
}

// Section heading: tile font, two points larger, semibold, primary colour.
void StyleHeading(wuxc::TextBlock const& tb) {
    ApplyTileFont(tb, false);
    tb.FontSize((g_tileFont.valid ? g_tileFont.size : 12) + 2);
    tb.FontWeight({600});
    if (auto br = Resource<wuxm::Brush>(L"TextFillColorPrimaryBrush")) tb.Foreground(br);
}

wuxc::TextBlock MakeCaption(const std::wstring& text) {
    wuxc::TextBlock label;
    label.Text(text);
    ApplyTileFont(label, false);
    label.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    label.VerticalAlignment(VerticalAlignment::Center);
    return label;
}

wuxc::TextBlock MakeSubtitle(const std::wstring& text) {
    wuxc::TextBlock t;
    t.Text(text);
    ApplyTileFont(t, true);
    t.FontSize(std::max(11.0, (g_tileFont.valid ? g_tileFont.size : 14) - 2));
    t.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    t.VerticalAlignment(VerticalAlignment::Center);
    return t;
}

wuxc::TextBlock MakeSectionHeader(const std::wstring& text) {
    auto t = MakeCaption(text);
    t.FontWeight({600});
    t.Margin({2, 12, 0, 6});
    return t;
}

wuxc::FontIcon MakeGlyph(PCWSTR glyph, double size = 16) {
    wuxc::FontIcon g;
    g.Glyph(glyph);
    g.FontSize(size);
    return g;
}

// Borderless square button, the look of the native Quick Settings header buttons.
wuxc::Button FlatButton(double size, wux::UIElement const& content, PCWSTR tip = nullptr) {
    wuxc::Button b;
    b.Width(size);
    b.Height(size);
    b.Padding({});
    b.Background(TransparentBrush());
    b.BorderThickness({});
    b.CornerRadius({4, 4, 4, 4});
    b.VerticalAlignment(VerticalAlignment::Center);
    b.Content(content);
    if (tip) {
        wuxc::ToolTipService::SetToolTip(b, winrt::box_value(tip));
        AutomationProperties::SetName(b, tip);
    }
    return b;
}

wuxc::ToggleSwitch MakeToggle(bool on, const std::wstring& name) {
    wuxc::ToggleSwitch ts;
    ts.OnContent(winrt::box_value(L""));
    ts.OffContent(winrt::box_value(L""));
    ts.MinWidth(0);
    ts.HorizontalAlignment(HorizontalAlignment::Right);
    ts.IsOn(on);
    AutomationProperties::SetName(ts, name);
    return ts;
}

// Settings-style card (CardBackgroundFillColorDefault / CardStroke).
wuxc::Border MakeCard(wux::UIElement const& child, Thickness padding, Thickness margin) {
    wuxc::Border card;
    card.Background(ThemeBrush(L"CardBackgroundFillColorDefaultBrush", 0x0DFFFFFF, 0xB3FFFFFF));
    card.BorderBrush(ThemeBrush(L"CardStrokeColorDefaultBrush", 0x19000000, 0x0F000000));
    card.BorderThickness({1, 1, 1, 1});
    card.CornerRadius({6, 6, 6, 6});
    card.Padding(padding);
    card.Margin(margin);
    card.Child(child);
    return card;
}
wuxc::Border MakePageCard(wux::UIElement const& child) { return MakeCard(child, {16, 12, 16, 12}, {0, 0, 0, 4}); }

wuxc::Grid MakeGrid(std::initializer_list<wux::GridLength> cols, std::initializer_list<wux::GridLength> rows = {}) {
    wuxc::Grid g;
    for (auto w : cols) {
        wuxc::ColumnDefinition c;
        c.Width(w);
        g.ColumnDefinitions().Append(c);
    }
    for (auto h : rows) {
        wuxc::RowDefinition r;
        r.Height(h);
        g.RowDefinitions().Append(r);
    }
    return g;
}

// Adds `el` to the grid cell and returns it.
template <class T>
T Put(wuxc::Grid const& g, T const& el, int col, int row = 0) {
    wuxc::Grid::SetColumn(el, col);
    wuxc::Grid::SetRow(el, row);
    g.Children().Append(el);
    return el;
}

template <class C, class T>
void RemoveFrom(C const& collection, T const& item) {
    uint32_t idx;
    if (item && collection.IndexOf(item, idx)) collection.RemoveAt(idx);
}

// Level line: thin accent bar under a slider track, scaled by the live peak.
struct Meter {
    wuxc::Grid host{nullptr};
    wuxm::ScaleTransform scale{nullptr};
    double level = 0;  // smoothed

    static Meter Make() {
        Meter m;
        m.host = wuxc::Grid();
        m.host.Height(2);
        m.host.Margin({0, -7, 0, 0});
        m.host.CornerRadius({1, 1, 1, 1});
        m.host.IsHitTestVisible(false);
        wux::Shapes::Rectangle bar;
        bar.RadiusX(1);
        bar.RadiusY(1);
        bar.Fill(AccentBrush());
        bar.Opacity(0.75);
        m.scale = wuxm::ScaleTransform();
        m.scale.ScaleX(0);
        bar.RenderTransform(m.scale);
        m.host.Children().Append(bar);
        return m;
    }
    // Perceptual-ish curve; instant attack, time-based release (full bar drops in ~0.4 s).
    void Update(float peak, double dt) {
        if (!scale) return;
        double target = std::sqrt(std::clamp((double)peak, 0.0, 1.0));
        level = target >= level ? target : std::max(target, level - dt * 2.5);
        if (level < 0.005) level = 0;
        scale.ScaleX(level);
    }
};

// Page transitions like the native Quick Settings sub-pages: slide + fade on
// the element's composition visual (Translation/Opacity), Fluent decelerate
// curve. XAML layout and native RenderTransforms are never touched.
constexpr float kSlide = 48.0f;
constexpr int kOutMs = 120, kInMs = 280;

void SlideFade(wux::UIElement const& el, float fromX, float toX, float fromO, float toO, int ms) {
    try {
        wux::Hosting::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
        auto visual = wux::Hosting::ElementCompositionPreview::GetElementVisual(el);
        auto comp = visual.Compositor();
        auto ease = comp.CreateCubicBezierEasingFunction({0.1f, 0.9f}, {0.2f, 1.0f});
        auto t = comp.CreateVector3KeyFrameAnimation();
        t.InsertKeyFrame(0.0f, {fromX, 0.0f, 0.0f});
        t.InsertKeyFrame(1.0f, {toX, 0.0f, 0.0f}, ease);
        t.Duration(std::chrono::milliseconds(ms));
        auto o = comp.CreateScalarKeyFrameAnimation();
        o.InsertKeyFrame(0.0f, fromO);
        o.InsertKeyFrame(1.0f, toO, ease);
        o.Duration(std::chrono::milliseconds(ms));
        visual.StartAnimation(L"Translation", t);
        visual.StartAnimation(L"Opacity", o);
    } catch (...) {
    }
}
void ResetSlideFade(wux::UIElement const& el) { SlideFade(el, 0, 0, 1, 1, 1); }

// Vertical variant for list items (expand/collapse), with an optional delay
// for a staggered entrance like the native lists.
void SlideFadeY(wux::UIElement const& el, float fromY, float toY, float fromO, float toO, int ms, int delayMs = 0) {
    try {
        wux::Hosting::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
        auto visual = wux::Hosting::ElementCompositionPreview::GetElementVisual(el);
        auto comp = visual.Compositor();
        auto ease = comp.CreateCubicBezierEasingFunction({0.1f, 0.9f}, {0.2f, 1.0f});
        auto t = comp.CreateVector3KeyFrameAnimation();
        t.InsertKeyFrame(0.0f, {0.0f, fromY, 0.0f});
        t.InsertKeyFrame(1.0f, {0.0f, toY, 0.0f}, ease);
        t.Duration(std::chrono::milliseconds(ms));
        auto o = comp.CreateScalarKeyFrameAnimation();
        o.InsertKeyFrame(0.0f, fromO);
        o.InsertKeyFrame(1.0f, toO, ease);
        o.Duration(std::chrono::milliseconds(ms));
        if (delayMs > 0) {
            t.DelayTime(std::chrono::milliseconds(delayMs));
            o.DelayTime(std::chrono::milliseconds(delayMs));
            t.DelayBehavior(winrt::Windows::UI::Composition::AnimationDelayBehavior::SetInitialValueBeforeDelay);
            o.DelayBehavior(winrt::Windows::UI::Composition::AnimationDelayBehavior::SetInitialValueBeforeDelay);
        }
        visual.StartAnimation(L"Translation", t);
        visual.StartAnimation(L"Opacity", o);
    } catch (...) {
    }
}

// Runs `then` once after `ms` on the UI thread (used to sequence out -> in).
void AfterDelay(int ms, std::function<void()> then) {
    wux::DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(ms));
    auto token = std::make_shared<winrt::event_token>();
    *token = timer.Tick([timer, token, then](auto&&, auto&&) {
        timer.Stop();
        timer.Tick(*token);
        then();
    });
    timer.Start();
}

// ---------------------------------------------------------------------------
// Visual tree
// ---------------------------------------------------------------------------

wux::FrameworkElement FindChildByName(wux::DependencyObject const& parent, std::wstring_view name) {
    if (!parent) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && fe.Name() == name) return fe;
        if (auto found = FindChildByName(child, name)) return found;
    }
    return nullptr;
}

// Breadth-first visit of up to `limit` elements, starting with `root` itself.
// `visit(node, inButton)`: inButton is true inside (or on) a ButtonBase.
template <class F>
void Walk(wux::DependencyObject const& root, size_t limit, F visit) {
    if (!root) return;
    std::vector<std::pair<wux::DependencyObject, bool>> q{{root, false}};
    for (size_t k = 0; k < q.size() && k < limit; k++) {
        auto [node, inButton] = q[k];
        inButton = inButton || node.try_as<wuxc::Primitives::ButtonBase>();
        visit(node, inButton);
        int n = VisualTreeHelper::GetChildrenCount(node);
        for (int c = 0; c < n; c++) q.push_back({VisualTreeHelper::GetChild(node, c), inButton});
    }
}

// True when the text is only icon-font glyphs (Private Use Area) or blank.
bool IsGlyphText(std::wstring_view t) {
    return std::all_of(t.begin(), t.end(), [](wchar_t ch) { return (ch >= 0xE000 && ch <= 0xF8FF) || iswspace(ch); });
}

// Tile name: the caption under the tile (a TextBlock outside the tile's
// button), else the button's accessible name, else text inside the button.
// Icon glyphs are ignored.
std::wstring TileName(wux::DependencyObject const& container) {
    std::wstring caption, buttonName, anyText;
    Walk(container, 200, [&](wux::DependencyObject const& node, bool inButton) {
        if (auto bb = node.try_as<wuxc::Primitives::ButtonBase>(); bb && buttonName.empty()) {
            std::wstring n(AutomationProperties::GetName(bb));
            if (!IsGlyphText(n)) buttonName = n;
        }
        if (auto tb = node.try_as<wuxc::TextBlock>()) {
            std::wstring t(tb.Text());
            if (IsGlyphText(t)) return;
            if (!inButton) caption = t;
            else if (anyText.empty()) anyText = t;
        }
    });
    return !caption.empty() ? caption : !buttonName.empty() ? buttonName : anyText;
}

// COM identity of a tile item. IInspectable pointers for the same object can
// differ between collections; the IUnknown pointer is the stable identity.
void* TileKey(wf::IInspectable const& item) {
    if (!item) return nullptr;
    winrt::com_ptr<::IUnknown> unk;
    winrt::get_unknown(item)->QueryInterface(__uuidof(::IUnknown), unk.put_void());
    return unk.get();  // object stays alive via the collection / tileNames
}

// Native volume row inside SlidersGroup:
//   [mute button] [ControlCenter.AsyncSlider] [Button#VolumeL2Button]
struct NativeVolumeRow {
    wux::FrameworkElement slider{nullptr}, icon{nullptr}, l2{nullptr};
};

NativeVolumeRow FindNativeVolumeRow(wux::FrameworkElement const& anchor) {
    NativeVolumeRow r;
    r.l2 = FindChildByName(anchor, L"VolumeL2Button");
    auto parent = r.l2 ? VisualTreeHelper::GetParent(r.l2) : nullptr;
    if (!parent) return r;
    int n = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < n; i++) {
        auto c = VisualTreeHelper::GetChild(parent, i).try_as<wux::FrameworkElement>();
        if (!c || c == r.l2) continue;
        if (!r.slider && c.try_as<wuxc::Slider>()) r.slider = c;
        else if (!r.icon && c.try_as<wuxc::Primitives::ButtonBase>()) r.icon = c;
    }
    Wh_Log(L"Native volume row: slider=%d icon=%d l2=%d", !!r.slider, !!r.icon, !!r.l2);
    return r;
}

// Finds the background drawn behind the slider band: the first full-width,
// non-transparent background in SlidersGroup's own subtree (breadth-first).
wuxm::Brush FindBandBrush(wux::FrameworkElement const& anchor) {
    double fullWidth = anchor.ActualWidth() * 0.9;
    std::vector<wux::DependencyObject> level{anchor};
    for (int depth = 0; depth < 8 && !level.empty(); depth++) {
        std::vector<wux::DependencyObject> next;
        for (auto& el : level) {
            auto fe = el.try_as<wux::FrameworkElement>();
            if (!fe || fe.ActualWidth() < fullWidth) continue;  // only full-width layers

            wuxm::Brush b{nullptr};
            if (auto c = el.try_as<wuxc::Control>()) b = c.Background();
            else if (auto p = el.try_as<wuxc::Panel>()) b = p.Background();
            else if (auto bd = el.try_as<wuxc::Border>()) b = bd.Background();

            auto scb = b.try_as<wuxm::SolidColorBrush>();
            if (b && !(scb && scb.Color().A == 0) && b.Opacity() != 0) {
                Wh_Log(L"Band background from %s #%s", winrt::get_class_name(el).c_str(), fe.Name().c_str());
                return b;
            }
            int n = VisualTreeHelper::GetChildrenCount(el);
            for (int i = 0; i < n; i++) next.push_back(VisualTreeHelper::GetChild(el, i));
        }
        level = std::move(next);
    }
    Wh_Log(L"No band background in SlidersGroup (shares the L1Grid background)");
    return nullptr;
}

// A row we added to a native Grid, plus the children moved to make room.
struct AddedRow {
    winrt::weak_ref<wuxc::Grid> grid;
    wuxc::RowDefinition def{nullptr};
    struct Slot {
        winrt::weak_ref<wux::FrameworkElement> el;
        int row, span;
    };
    std::vector<Slot> moved;

    // Inserts an Auto row at `at`. Children for which `shifts(el, row)` is true
    // move down one row; children spanning the new row, or picked by
    // `grows(el, endRow)`, get one more row of span.
    template <class S, class G>
    void Insert(wuxc::Grid const& g, int at, S shifts, G grows) {
        grid = g;
        for (auto c : g.Children()) {
            auto fe = c.try_as<wux::FrameworkElement>();
            if (!fe) continue;
            int r = wuxc::Grid::GetRow(fe), rs = wuxc::Grid::GetRowSpan(fe);
            if (shifts(fe, r)) wuxc::Grid::SetRow(fe, r + 1);
            else if (r + rs > at || grows(fe, r + rs)) wuxc::Grid::SetRowSpan(fe, rs + 1);
            else continue;
            moved.push_back({winrt::make_weak(fe), r, rs});
        }
        def = wuxc::RowDefinition();
        def.Height(kAuto);
        g.RowDefinitions().InsertAt(at, def);
    }
    void Remove() {
        if (auto g = grid.get()) RemoveFrom(g.RowDefinitions(), def);
        for (auto& s : moved)
            if (auto el = s.el.get()) {
                wuxc::Grid::SetRow(el, s.row);
                wuxc::Grid::SetRowSpan(el, s.span);
            }
        moved.clear();
        def = nullptr;
    }
};

// 32px app icon from the exe, cached per path.
wuxm::ImageSource IconForExe(const std::wstring& path) {
    static std::map<std::wstring, wuxm::ImageSource> cache;
    if (path.empty()) return nullptr;
    if (auto it = cache.find(path); it != cache.end()) return it->second;
    auto& result = cache.emplace(path, nullptr).first->second;

    HICON hIcon = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hIcon, nullptr, 1) == 0 || !hIcon) {
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) hIcon = sfi.hIcon;
    }
    if (!hIcon) return nullptr;

    constexpr int sz = 32;
    BITMAPINFO bi{};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), sz, -sz /* top-down */, 1, 32, BI_RGB};
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dc && dib && bits) {
        HGDIOBJ old = SelectObject(dc, dib);
        memset(bits, 0, sz * sz * 4);
        DrawIconEx(dc, 0, 0, hIcon, sz, sz, 0, nullptr, DI_NORMAL);
        GdiFlush();

        auto px = (uint8_t*)bits;
        bool anyAlpha = false;
        for (int i = 0; i < sz * sz && !anyAlpha; i++) anyAlpha = px[i * 4 + 3] != 0;
        if (!anyAlpha)  // legacy icon without alpha channel
            for (int i = 0; i < sz * sz; i++)
                if (px[i * 4] | px[i * 4 + 1] | px[i * 4 + 2]) px[i * 4 + 3] = 255;

        wuxm::Imaging::WriteableBitmap wb(sz, sz);
        memcpy(wb.PixelBuffer().data(), bits, sz * sz * 4);
        wb.Invalidate();
        result = wb;
        SelectObject(dc, old);
    }
    if (dib) DeleteObject(dib);
    if (dc) DeleteDC(dc);
    DestroyIcon(hIcon);
    return result;
}

// ============================================================================
// Mixer row
//
//          App name
//   [icon] ==========o-----------------     72
//          ▔▔▔▔▔ level
// The icon is centered on the whole row and toggles mute. Header and slider
// share a column, so they always start at the same x.
// ============================================================================

struct MixerRow {
    wuxc::Border card{nullptr};
    wuxc::Slider slider{nullptr};
    wuxc::Button iconButton{nullptr};
    wuxc::TextBlock percent{nullptr};
    wuxc::ColumnDefinition colIcon{nullptr}, colNumber{nullptr};
    Meter meter;
    std::function<void(bool)> setMuted;  // updates the icon look
    wuxc::Slider::ValueChanged_revoker percentRevoker, valueRevoker;
    wuxc::Button::Click_revoker clickRevoker;
    std::vector<winrt::com_ptr<ISimpleAudioVolume>> vols;  // app rows: for value sync
    std::vector<winrt::com_ptr<AudioMeter>> meters;
    std::wstring exePath;  // app rows: for the stacked icons on "Show N apps"
};

wuxc::Slider MakeSlider(wux::Style const& style, double value, const std::wstring& name) {
    wuxc::Slider slider;
    if (style) {
        try {
            slider.Style(style);
        } catch (...) {
        }
    }
    slider.Minimum(0);
    slider.Maximum(100);
    slider.StepFrequency(1);
    slider.Value(value);
    slider.VerticalAlignment(VerticalAlignment::Center);
    slider.Margin({0, -6, 0, -6});
    AutomationProperties::SetName(slider, name);
    return slider;
}

MixerRow BuildRow(const std::wstring& name, wux::UIElement const& iconVisual, int initial, wux::Style const& sliderStyle) {
    MixerRow mr;
    // Columns: [icon area] [header + slider + level] [number area]; every header row is the same height.
    auto row = MakeGrid({Px(56), kStar, Px(60)}, {Px(22), kAuto, kAuto});
    mr.colIcon = row.ColumnDefinitions().GetAt(0);
    mr.colNumber = row.ColumnDefinitions().GetAt(2);

    auto header = Put(row, MakeCaption(name), 1);
    header.HorizontalAlignment(HorizontalAlignment::Left);
    wuxc::Grid::SetColumnSpan(header, 2);

    wuxc::Grid iconHost;
    iconHost.Children().Append(iconVisual);
    auto muteBadge = MakeGlyph(L"\xE74F", 9);
    muteBadge.HorizontalAlignment(HorizontalAlignment::Right);
    muteBadge.VerticalAlignment(VerticalAlignment::Bottom);
    muteBadge.Margin({0, 0, -5, -5});
    muteBadge.Visibility(Visibility::Collapsed);
    iconHost.Children().Append(muteBadge);
    mr.iconButton = Put(row, FlatButton(32, iconHost, L"Mute"), 0);
    mr.iconButton.HorizontalAlignment(HorizontalAlignment::Left);
    AutomationProperties::SetName(mr.iconButton, L"Mute " + name);
    wuxc::Grid::SetRowSpan(mr.iconButton, 3);

    mr.slider = Put(row, MakeSlider(sliderStyle, initial, name), 1, 1);
    mr.slider.Name(L"WhMixerSlider");

    auto percent = Put(row, wuxc::TextBlock(), 2, 1);  // volume number on the right
    ApplyTileFont(percent, true);
    percent.Text(winrt::to_hstring(initial));
    percent.Width(32);
    percent.TextAlignment(wux::TextAlignment::Right);
    percent.HorizontalAlignment(HorizontalAlignment::Right);
    percent.VerticalAlignment(VerticalAlignment::Center);
    mr.percent = percent;
    mr.percentRevoker = mr.slider.ValueChanged(
        winrt::auto_revoke, [percent](auto&&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& e) {
            percent.Text(winrt::to_hstring((int)std::lround(e.NewValue())));
        });

    mr.meter = Meter::Make();
    Put(row, mr.meter.host, 1, 2);

    mr.card = MakeCard(row, {0, 6, 0, 8}, {12, 0, 12, 6});
    // No "Edit quick settings" menu inside slider boxes.
    mr.card.ContextRequested([](auto&&, wux::Input::ContextRequestedEventArgs const& e) { e.Handled(true); });
    mr.setMuted = [iconVisual, muteBadge](bool muted) {
        iconVisual.Opacity(muted ? 0.4 : 1.0);
        muteBadge.Visibility(muted ? Visibility::Visible : Visibility::Collapsed);
    };
    return mr;
}

// ============================================================================
// Mixer panel (injected XAML) — touched on the UI thread only
// ============================================================================

enum class PageKind { Sound, Brightness };

struct MixerPanel : std::enable_shared_from_this<MixerPanel> {
    using HiddenList = std::vector<std::pair<winrt::weak_ref<wux::UIElement>, Visibility>>;

    // --- Own UI
    wuxc::StackPanel root{nullptr}, rowsHost{nullptr};
    wuxc::Grid titleRow{nullptr};
    std::vector<wuxc::TextBlock> headings;  // section titles, restyled with the tile font
    std::vector<MixerRow> rows;             // rows[0] = master
    wuxc::Button showAllButton{nullptr};
    bool showAll = false;
    int showAllGen = 0;  // a newer expand/collapse cancels a pending collapse
    wuxm::Animation::Storyboard rowsHeightAnim{nullptr};  // rowsHost height while expanding/collapsing
    wux::FrameworkElement::SizeChanged_revoker rowsClipRevoker;     // clip rowsHost to its animated height
    wux::DispatcherTimer timer{nullptr}, visTimer{nullptr};  // 700 ms sync, 100 ms open/close check
    winrt::event_token tickToken{}, visTickToken{};
    wuxm::CompositionTarget::Rendering_revoker renderingRevoker;  // per-frame meters, only while visible
    bool lastVisible = false, metersActive = false;
    ULONGLONG lastMeterTick = 0;

    // --- Native elements: where we live, what we hid, what we measured
    AddedRow mixerRow, pinnedRow;
    wuxc::RowDefinition addedFirstRow{nullptr};  // only when the host grid had no rows
    HiddenList hidden;                           // volume row, L2 button, footer, pager...
    winrt::weak_ref<wux::FrameworkElement> anchorRef, nativeSlider, nativeIcon, togglesRef;
    winrt::weak_ref<wuxc::Grid> l1Ref;
    bool metricsCaptured = false;
    double sliderLeft = 56, sliderRightInset = 20, iconLeft = 20, iconW = 32, iconH = 32;
    double pageInsetL = 12, pageInsetR = 12;
    wux::Style sliderStyle{nullptr};

    // --- Audio
    std::wstring currentDeviceId;
    std::vector<DWORD> shownKeys;
    winrt::com_ptr<IAudioEndpointVolume> endpoint;
    bool suppressMaster = false;
    ULONGLONG lastMasterUserChange = 0;

    // --- Tiles. Windows' own collection (nativeTiles) is never modified: the
    // GridView shows a filtered copy (tileSource) that follows it.
    winrt::weak_ref<wuxc::GridView> tileGrid;
    wf::IInspectable nativeTilesSource{nullptr};  // restored on unload
    TileList nativeTiles{nullptr}, tileSource{nullptr};
    winrt::event_token nativeTilesToken{};
    wux::FrameworkElement::SizeChanged_revoker gridSizeRevoker;
    std::vector<std::wstring> unpinned;  // persisted names
    std::map<void*, std::pair<wf::IInspectable, std::wstring>> tileNames;
    // Tile names by position in Windows' list, saved across ShellHost restarts.
    // A fresh ShellHost adds tiles before their names can be read; until then
    // each tile is assumed to be the one saved at its position, so unpinned
    // tiles never show for a moment.
    std::vector<std::wstring> savedTileOrder;
    int nameRetries = 15;  // ticks spent re-probing tiles still known only by guess
    // Off-layout probe that realizes new tiles invisibly to read their names,
    // so unpinned tiles are filtered before they are ever shown.
    wuxc::Canvas probeHost{nullptr};
    wuxc::ItemsControl probe{nullptr};
    // Scrolling/page sensors are disabled while every tile fits on one page.
    bool scrollLocked = false;
    wuxc::ScrollMode origScrollMode = wuxc::ScrollMode::Enabled;
    wuxc::ScrollBarVisibility origScrollBar = wuxc::ScrollBarVisibility::Auto;
    wux::UIElement::PointerWheelChanged_revoker wheelRevoker, panelWheelRevoker;
    // One-row mode: heights changed on the grid and its fixed-height ancestors.
    struct CompactEntry {
        winrt::weak_ref<wux::FrameworkElement> el;
        double orig, target;
    };
    std::vector<CompactEntry> compactHeights;
    double tileItemH = 0;  // last known tile height (0 right after our list is swapped in)

    // --- "Edit quick settings": tile menus, panel right-click, Pinned header, edit mode
    std::set<void*> scannedContainers;  // tile containers already checked for a menu
    std::vector<std::pair<winrt::weak_ref<wuxc::MenuFlyout>, std::vector<winrt::weak_ref<wuxc::MenuFlyoutItemBase>>>>
        menuAdds;
    winrt::weak_ref<wuxc::MenuFlyout> ownTogglesFlyout;
    wuxc::MenuFlyout panelFlyout{nullptr};
    wux::UIElement::ContextRequested_revoker l1ContextRevoker;
    wuxc::Grid pinnedHeader{nullptr};  // Pinned ......................... [+] [✎]
    wuxc::Button pinnedEditButton{nullptr}, pinnedAddButton{nullptr};
    bool editMode = false;
    wuxc::Canvas overlay{nullptr};  // over the tiles while editing: blocks clicks, hosts unpin badges
    std::wstring overlaySig;

    // --- Sound / Brightness page
    bool pageOpen = false, pageAnimating = false;
    PageKind pageKind = PageKind::Sound;
    wuxc::Grid page{nullptr};
    HiddenList pageHidden;
    winrt::com_ptr<IAudioEndpointVolume> micVolume;
    winrt::com_ptr<AudioMeter> micMeter;
    winrt::com_ptr<IAudioClient> micClient;  // capture stream so the meter moves
    winrt::com_ptr<IAudioCaptureClient> micCapture;
    Meter micLevel;

    // --- Monitor brightness: section above the mixer + Brightness page
    wuxc::StackPanel brightHost{nullptr}, brightRowsHost{nullptr};  // collapsed without controllable monitors
    wuxc::Grid brightTitle{nullptr};
    std::vector<MixerRow> brightRows;
    std::vector<std::wstring> brightKeys;                   // identity of the current rows
    std::vector<std::wstring> monitorOrder, monitorHidden;  // persisted
    std::map<std::wstring, ULONGLONG> brightUserChange;     // key -> last drag time
    std::wstring monitorSig;
    bool suppressBright = false;
    bool linkBrightness = false;  // persisted: one slider for all monitors

    // Runs f(panel) on the UI queue, if the panel is still alive by then.
    template <class F>
    void Later(F f, winrt::Windows::System::DispatcherQueuePriority priority = {}) {
        if (g_uiQueue)
            g_uiQueue.TryEnqueue(priority, [weak = weak_from_this(), f] {
                if (auto self = weak.lock()) f(*self);
            });
    }
    // Event handler that runs f(panel) after the event returns (the handler's
    // own element may be destroyed by f).
    template <class F>
    auto Deferred(F f) {
        return [weak = weak_from_this(), f](auto&&...) {
            if (auto self = weak.lock()) self->Later(f);
        };
    }

    void Create();
    void Destroy();
    void BindNative(wux::FrameworkElement const& anchor, NativeVolumeRow const& n);
    void Hide(wux::UIElement const& el);
    void Unhide(wux::UIElement const& el);
    wuxc::Grid MakeTitle(PCWSTR text, std::initializer_list<wuxc::Button> buttons, double spacing = 2);
    void CaptureMetrics();
    void ApplyMetrics();
    void OnTick();
    void OnAudioChanged(int what);
    void UpdateVisibility();
    void SetMetersActive(bool active);
    void OnMeterTick();

    void RefreshSessions(bool force);
    void RefreshMaster(bool force);
    void SyncAppValues();
    void UpdateShowAll(bool animate = false);

    void OpenPage(PageKind kind);
    void ClosePage(bool animate = false);
    void BuildPage();
    void BuildSoundPage(wuxc::StackPanel const& content);
    void BuildBrightnessPage(wuxc::StackPanel const& content);
    void StopMic();

    bool IsMonitorHidden(const std::wstring& key) const;
    std::vector<MonitorState> OrderedMonitors() const;
    bool BrightDragging(const std::wstring* key = nullptr) const;
    void RefreshBrightness(bool force);
    void OnMonitorsChanged();
    void MoveMonitor(const std::wstring& key, int delta);

    void ApplyTiles(wuxc::Grid const& l1);
    bool IsUnpinned(const std::wstring& name) const;
    bool LearnTileNames();
    void ProbeTileNames(std::vector<wf::IInspectable> const& items);
    void RebuildTileView();
    void SetPinned(const std::wstring& name, bool pinned);
    void UpdateScrollLock(uint32_t count, int perRow);
    void UpdateCompact();
    void InsertPinnedHeader(wuxc::Grid const& l1, wux::FrameworkElement const& toggles);
    wuxc::MenuFlyoutItem MakeEditMenuItem(PCWSTR text = L"Edit quick settings", PCWSTR glyph = L"\xE70F");
    void AttachTileMenus();
    void EnterEdit();
    void ExitEdit();
    void UpdateOverlay();
};

std::shared_ptr<MixerPanel> g_panel;

// Audio callbacks -> UI thread, coalesced: one queued refresh carries every change since the last one.
std::atomic<int> g_audioPending{0};
void NotifyAudioChanged(int what) {
    if (g_unloading || !g_uiQueue || g_audioPending.fetch_or(what) != 0) return;
    bool queued = g_uiQueue.TryEnqueue([] {
        int changes = g_audioPending.exchange(0);
        if (g_panel) g_panel->OnAudioChanged(changes);
    });
    if (!queued) g_audioPending = 0;
}

void NotifyMonitorsChanged() {
    if (g_unloading || !g_uiQueue) return;
    g_uiQueue.TryEnqueue([] {
        if (g_panel) g_panel->OnMonitorsChanged();
    });
}

void MixerPanel::Hide(wux::UIElement const& el) {
    if (!el) return;
    for (auto& h : hidden)
        if (h.first.get() == el) return;
    hidden.push_back({winrt::make_weak(el), el.Visibility()});
    el.Visibility(Visibility::Collapsed);
}

void MixerPanel::Unhide(wux::UIElement const& el) {
    auto it = std::find_if(hidden.begin(), hidden.end(), [&](auto& h) { return h.first.get() == el; });
    if (!el || it == hidden.end()) return;
    el.Visibility(it->second);
    hidden.erase(it);
}

// Section title row:  Title ............................ [buttons]
// The last button's glyph ends where the volume numbers end (28px button, 14px glyph).
wuxc::Grid MixerPanel::MakeTitle(PCWSTR text, std::initializer_list<wuxc::Button> buttons, double spacing) {
    auto row = MakeGrid({kStar, kAuto});
    auto title = Put(row, wuxc::TextBlock(), 0);
    title.Text(text);
    title.VerticalAlignment(VerticalAlignment::Center);
    StyleHeading(title);
    headings.push_back(title);
    auto side = Put(row, wuxc::StackPanel(), 1);
    side.Orientation(wuxc::Orientation::Horizontal);
    side.Spacing(spacing);
    side.HorizontalAlignment(HorizontalAlignment::Right);
    side.VerticalAlignment(VerticalAlignment::Center);
    side.Margin({0, 0, -7, 0});
    for (auto& b : buttons) side.Children().Append(b);
    return row;
}

void MixerPanel::Create() {
    std::weak_ptr<MixerPanel> weak = weak_from_this();
    auto pageArrow = [&](PCWSTR tip, PageKind kind) {  // chevron, like the native sub-page buttons
        auto b = FlatButton(28, MakeGlyph(L"\xE76C", 12), tip);
        b.Click(Deferred([kind](MixerPanel& p) { p.OpenPage(kind); }));
        return b;
    };
    auto link = [](PCWSTR glyph, PCWSTR tip, PCWSTR file, PCWSTR args) {
        auto b = FlatButton(28, MakeGlyph(glyph, 14), tip);
        b.Click([file, args](auto&&...) { ShellExecuteW(nullptr, L"open", file, args, nullptr, SW_SHOWNORMAL); });
        return b;
    };

    root = wuxc::StackPanel();
    root.Background(TransparentBrush());  // gaps between cards take right-clicks
    root.Name(L"WhMixerPanel");
    root.Padding({0, 8, 0, 8});

    // Brightness section, above the mixer like the native brightness slider.
    brightHost = wuxc::StackPanel();
    brightHost.Visibility(Visibility::Collapsed);
    brightHost.Margin({0, 0, 0, 8});
    brightTitle = MakeTitle(L"Brightness", {pageArrow(L"Monitor settings", PageKind::Brightness)});
    brightRowsHost = wuxc::StackPanel();
    brightHost.Children().Append(brightTitle);
    brightHost.Children().Append(brightRowsHost);
    root.Children().Append(brightHost);

    // Volume mixer ............ [sound settings] [legacy sound] [>]
    auto soundArrow = pageArrow(L"More sound settings", PageKind::Sound);
    soundArrow.Margin({2, 0, 0, 0});
    titleRow = MakeTitle(L"Volume mixer", {link(L"\xE713", L"Sound settings", L"ms-settings:sound", nullptr),
                                           link(L"\xE7F5", L"Legacy sound settings", L"control.exe", L"mmsys.cpl"),
                                           soundArrow});
    root.Children().Append(titleRow);
    rowsHost = wuxc::StackPanel();
    root.Children().Append(rowsHost);

    // "Show N apps" / "Show less" under the cards.
    showAllButton = wuxc::Button();
    showAllButton.Background(TransparentBrush());
    showAllButton.BorderThickness({});
    showAllButton.HorizontalAlignment(HorizontalAlignment::Stretch);
    showAllButton.HorizontalContentAlignment(HorizontalAlignment::Center);
    showAllButton.Padding({8, 4, 8, 4});
    showAllButton.CornerRadius({4, 4, 4, 4});
    showAllButton.Visibility(Visibility::Collapsed);
    showAllButton.Click([weak](auto&&...) {
        if (auto self = weak.lock()) {
            self->showAll = !self->showAll;
            self->UpdateShowAll(true);
        }
    });
    root.Children().Append(showAllButton);

    root.SizeChanged([weak](auto&&...) {
        if (auto self = weak.lock()) {
            self->CaptureMetrics();
            self->ApplyMetrics();
        }
    });

    // The 700 ms timer keeps running while Quick Settings is closed, so the
    // panel is current the moment it opens. The 100 ms timer is a cheap
    // open/close check: closing must close the sub-page and edit mode right
    // away, even on a quick reopen.
    timer = wux::DispatcherTimer();
    timer.Interval(std::chrono::milliseconds(700));
    tickToken = timer.Tick([weak](auto&&...) {
        if (auto self = weak.lock()) self->OnTick();
    });
    visTimer = wux::DispatcherTimer();
    visTimer.Interval(std::chrono::milliseconds(100));
    visTickToken = visTimer.Tick([weak](auto&&...) {
        if (auto self = weak.lock()) self->UpdateVisibility();
    });
    root.Loaded([weak](auto&&...) {
        if (auto self = weak.lock(); self && self->timer) {
            self->timer.Start();
            self->visTimer.Start();
        }
    });

    unpinned = LoadList(L"unpinnedTiles");
    std::erase_if(unpinned, [](auto& n) { return IsGlyphText(n); });
    savedTileOrder = LoadList(L"tileOrder");
    monitorOrder = LoadList(L"monitorOrder");
    monitorHidden = LoadList(L"monitorHidden");
    linkBrightness = LoadList(L"linkBrightness") == std::vector<std::wstring>{L"1"};
    monitorSig = MonitorSignature();
    if (g_settings.brightness) {
        g_ddc.Start();
        g_ddc.Refresh();  // rows appear when the first read comes back
    }
    RefreshBrightness(true);
    currentDeviceId = DeviceId(DefaultDevice(eRender));
    RefreshSessions(true);
    timer.Start();
}

void MixerPanel::BindNative(wux::FrameworkElement const& anchor, NativeVolumeRow const& n) {
    anchorRef = anchor;
    if (n.slider) nativeSlider = n.slider;
    if (n.icon) nativeIcon = n.icon;
    Hide(n.l2);  // remove the "Volume mixer" page button
    try {
        if (auto st = n.slider ? n.slider.Style() : nullptr) {
            std::wstring target(st.TargetType().Name);
            Wh_Log(L"Native slider style target: %s", target.c_str());
            if (target == L"Windows.UI.Xaml.Controls.Slider") sliderStyle = st;
        }
    } catch (...) {
    }
}

// Reads the native row's geometry once (relative to our panel), then hides the
// native volume row: our main row replaces it.
void MixerPanel::CaptureMetrics() {
    if (metricsCaptured || !root || root.ActualWidth() <= 0) return;
    auto ns = nativeSlider.get();
    if (ns && ns.ActualWidth() <= 0) return;  // not laid out yet
    metricsCaptured = true;
    if (!ns) return;
    try {
        auto pt = ns.TransformToVisual(root).TransformPoint({0, 0});
        sliderLeft = pt.X;
        sliderRightInset = std::max(0.0, root.ActualWidth() - (pt.X + ns.ActualWidth()));
        if (auto ni = nativeIcon.get(); ni && ni.ActualWidth() > 0) {
            iconW = std::min(32.0, ni.ActualWidth());
            iconH = std::min(32.0, ni.ActualHeight());
            iconLeft = ni.TransformToVisual(root).TransformPoint({0, 0}).X + (ni.ActualWidth() - iconW) / 2;
        }
        Wh_Log(L"Metrics: slider %.1f..-%.1f icon %.1f (%.0fx%.0f)", sliderLeft, sliderRightInset, iconLeft, iconW,
               iconH);
    } catch (...) {
    }

    // Hide the native volume row (whole SlidersGroup on desktops; just the
    // volume item when there are more sliders, e.g. brightness).
    auto anchor = anchorRef.get();
    if (!anchor) return;
    wuxc::GridViewItem volumeItem{nullptr};
    wux::DependencyObject it = ns;
    for (int i = 0; i < 16 && it && !volumeItem; i++) {
        it = VisualTreeHelper::GetParent(it);
        volumeItem = it.try_as<wuxc::GridViewItem>();
    }
    auto gv = FindChildByName(anchor, L"RootGridView").try_as<wuxc::GridView>();
    if (!volumeItem || (gv && gv.Items().Size() <= 1)) Hide(anchor);
    else Hide(volumeItem);
}

void MixerPanel::ApplyMetrics() {
    if (!root) return;
    constexpr double kNumberCol = 36;  // "100"
    constexpr double kInnerPad = 16;   // same gap on both sides inside the card
    constexpr double kIconGlyph = 16;  // drawn icon size inside the icon button
    constexpr double bt = 1;           // card border

    // Cards are centered in the panel: the left edge sits kInnerPad before the
    // visible icon, the right edge mirrors it.
    double card = std::max(2.0, iconLeft + (iconW - kIconGlyph) / 2 - kInnerPad);
    double inL = card + bt;

    for (auto* list : {&brightRows, &rows})
        for (auto& r : *list) {
            r.card.Margin({card, 0, card, 6});
            r.colIcon.Width(Px(std::max(0.0, sliderLeft - inL)));
            r.colNumber.Width(Px(kNumberCol + kInnerPad + 8));  // text right edge kInnerPad from the card edge
            r.iconButton.Width(iconW);
            r.iconButton.Height(iconH);
            r.iconButton.Margin({std::max(0.0, iconLeft - inL), 0, 0, 0});
            r.percent.Width(kNumberCol);
            r.percent.Margin({0, 0, kInnerPad - bt, 0});
        }
    // Title rows use the cards' inner padding: titles start at the icons, the
    // last button's glyph ends where the volume numbers end.
    Thickness titleMargin{card + kInnerPad, 2, card + kInnerPad, 6};
    titleRow.Margin(titleMargin);
    brightTitle.Margin(titleMargin);
    if (pinnedHeader) {
        auto rm = root.Margin();  // the header shares the mixer's column; add its margin
        pinnedHeader.Margin({rm.Left + card + kInnerPad, 12, rm.Right + card + kInnerPad, 0});
    }
    for (auto& h : headings) StyleHeading(h);
    showAllButton.Margin({card, 0, card, 0});
    pageInsetL = pageInsetR = card;
}

// ---------------------------------------------------------------------------
// Timers: background sync, open/close, level meters
// ---------------------------------------------------------------------------

// The Quick Settings window is hidden/cloaked rather than destroyed when it
// closes, so check the window itself too.
bool IsControlCenterWindowShown() {
    static HWND cached = nullptr;
    if (!cached || !IsWindow(cached)) {
        cached = nullptr;
        for (HWND h = nullptr; !cached && (h = FindWindowExW(nullptr, h, L"ControlCenterWindow", nullptr));) {
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (pid == GetCurrentProcessId()) cached = h;
        }
    }
    if (!cached) return true;  // unknown: don't block
    DWORD cloaked = 0;
    return IsWindowVisible(cached) &&
           !(SUCCEEDED(DwmGetWindowAttribute(cached, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked);
}

void MixerPanel::UpdateVisibility() {
    auto xr = root.XamlRoot();
    bool visible = xr && xr.IsHostVisible() && IsControlCenterWindowShown();
    SetMetersActive(visible);
    if (!visible) {  // both are no-ops when already closed
        ExitEdit();
        ClosePage();
    }
    if (visible == lastVisible) return;
    lastVisible = visible;
    // Brightness may have been changed on the monitor itself: re-read on open.
    if (visible && g_settings.brightness) g_ddc.Refresh();
    if (visible) OnAudioChanged(kAudioAll);  // in case an audio event was missed while closed
}

void MixerPanel::OnTick() {
    UpdateVisibility();
    // Brightness: re-read when monitors are plugged or rearranged.
    if (g_settings.brightness)
        if (auto sig = MonitorSignature(); sig != monitorSig) {
            monitorSig = sig;
            g_ddc.Refresh();
        }

    // Safety net: parts not realized when we injected get bound now.
    if (auto l1 = l1Ref.get(); l1 && !tileGrid.get()) ApplyTiles(l1);
    if (auto anchor = anchorRef.get(); anchor && !nativeSlider.get()) {
        if (auto n = FindNativeVolumeRow(anchor); n.slider) {
            BindNative(anchor, n);
            metricsCaptured = false;
            CaptureMetrics();
        }
    }

    // Tiles still shown/hidden by guess: try to read their names again.
    if (nameRetries > 0 && nativeTiles && tileSource) {
        bool unnamed = false;
        for (uint32_t i = 0; i < nativeTiles.Size() && !unnamed; i++)
            unnamed = !tileNames.count(TileKey(nativeTiles.GetAt(i)));
        if (unnamed) {
            nameRetries--;
            RebuildTileView();
        }
    }
    if (LearnTileNames()) RebuildTileView();  // late-realized or re-added tiles
    UpdateCompact();
    UpdateOverlay();
    AttachTileMenus();  // tiles realize lazily and get recycled

    // Audio follows WASAPI events (OnAudioChanged); polling is only a safety
    // net while Quick Settings is on screen.
    if (lastVisible) OnAudioChanged(kAudioAll);
}

// Something changed on the audio side (AudioEvents), or a periodic re-check.
void MixerPanel::OnAudioChanged(int what) {
    g_audioEvents.Reap();
    if (!root) return;
    if (what & kDeviceChanged)
        if (auto def = DeviceId(DefaultDevice(eRender)); def != currentDeviceId) {
            currentDeviceId = def;
            RefreshSessions(true);
            return;
        }
    if (what & kMasterChanged) RefreshMaster(false);
    if (what & kSessionsChanged) RefreshSessions(false);  // rebuilds only when apps come or go
    if (what & (kSessionsChanged | kAppVolumeChanged)) SyncAppValues();
}

// CompositionTarget::Rendering keeps XAML rendering every frame, so it is only
// subscribed while Quick Settings is on screen.
void MixerPanel::SetMetersActive(bool active) {
    if (active == metersActive) return;
    metersActive = active;
    lastMeterTick = 0;
    renderingRevoker.revoke();
    if (active)
        renderingRevoker = wuxm::CompositionTarget::Rendering(winrt::auto_revoke, [weak = weak_from_this()](auto&&...) {
            if (auto self = weak.lock()) self->OnMeterTick();
        });
}

void MixerPanel::OnMeterTick() {
    if (!root || !lastVisible) return;
    ULONGLONG now = GetTickCount64();
    double dt = lastMeterTick ? std::min(0.1, (now - lastMeterTick) / 1000.0) : 0.016;
    lastMeterTick = now;
    if (micCapture) {  // drain the capture buffer
        UINT32 packet = 0, frames = 0;
        BYTE* data = nullptr;
        DWORD flags = 0;
        for (int guard = 0; guard < 32 && SUCCEEDED(micCapture->GetNextPacketSize(&packet)) && packet; guard++) {
            if (FAILED(micCapture->GetBuffer(&data, &frames, &flags, nullptr, nullptr))) break;
            micCapture->ReleaseBuffer(frames);
        }
    }
    float p = 0;
    if (pageOpen && micMeter && SUCCEEDED(micMeter->GetPeakValue(&p))) micLevel.Update(p, dt);
    for (auto& r : rows) {
        float peak = 0;
        for (auto& m : r.meters)
            if (m && SUCCEEDED(m->GetPeakValue(&p))) peak = std::max(peak, p);
        r.meter.Update(peak, dt);
    }
}

// ---------------------------------------------------------------------------
// Audio rows
// ---------------------------------------------------------------------------

void MixerPanel::RefreshSessions(bool force) {
    // Selected device, or the default output when the id is empty or stale.
    auto dev = DeviceById(currentDeviceId);
    if (!dev) dev = DefaultDevice(eRender);
    g_audioEvents.Watch(DeviceId(dev));
    auto groups = GetAppSessions(dev);

    std::vector<DWORD> keys;
    for (auto& g : groups) keys.push_back(g.key);
    if (!force && keys == shownKeys) return;
    shownKeys = keys;
    rows.clear();
    rowsHost.Children().Clear();
    auto weak = weak_from_this();

    // --- Master row (replaces the native volume row)
    {
        endpoint = Activate<IAudioEndpointVolume>(dev);
        float level = 0;
        if (endpoint) endpoint->GetMasterVolumeLevelScalar(&level);
        auto mr = BuildRow(L"Master volume", MakeGlyph(L"\xE767"), (int)std::lround(level * 100.0), sliderStyle);
        mr.valueRevoker = mr.slider.ValueChanged(
            winrt::auto_revoke,
            [weak, ep = endpoint](auto&&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& e) {
                auto self = weak.lock();
                if (!self || self->suppressMaster || !ep) return;
                self->lastMasterUserChange = GetTickCount64();
                ep->SetMasterVolumeLevelScalar((float)(e.NewValue() / 100.0), nullptr);
            });
        // Native-style "ding" when the slider is released (drag end or click).
        // handledEventsToo: the Slider marks pointer events as handled.
        mr.slider.AddHandler(wux::UIElement::PointerReleasedEvent(),
                             winrt::box_value(wux::Input::PointerEventHandler([](auto&&...) {
                                 PlaySoundW(L"SystemDefault", nullptr, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
                             })),
                             true);
        mr.clickRevoker = mr.iconButton.Click(winrt::auto_revoke, [ep = endpoint, setMuted = mr.setMuted](auto&&...) {
            BOOL m = FALSE;
            if (!ep) return;
            ep->GetMute(&m);
            ep->SetMute(!m, nullptr);
            setMuted(!m);
        });
        if (auto meter = Activate<AudioMeter>(dev, kIID_AudioMeter)) mr.meters.push_back(meter);
        rowsHost.Children().Append(mr.card);
        rows.push_back(std::move(mr));
        RefreshMaster(true);
    }

    // --- App rows
    for (auto& g : groups) {
        wux::UIElement icon = MakeGlyph(L"\xE767");
        if (auto img = IconForExe(g.exePath)) {
            wuxc::Image image;
            image.Source(img);
            image.Width(16);
            image.Height(16);
            icon = image;
        }
        auto mr = BuildRow(g.label, icon, (int)std::lround(g.volume * 100.0), sliderStyle);
        mr.setMuted(g.muted);
        mr.valueRevoker = mr.slider.ValueChanged(
            winrt::auto_revoke, [vols = g.vols](auto&&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& e) {
                for (auto& v : vols) v->SetMasterVolume((float)(e.NewValue() / 100.0), nullptr);
            });
        mr.clickRevoker = mr.iconButton.Click(winrt::auto_revoke, [vols = g.vols, setMuted = mr.setMuted](auto&&...) {
            BOOL muted = FALSE;
            if (!vols.empty()) vols.front()->GetMute(&muted);
            for (auto& v : vols) v->SetMute(!muted, nullptr);
            setMuted(!muted);
        });
        mr.meters = g.meters;
        mr.vols = g.vols;
        mr.exePath = g.exePath;
        rowsHost.Children().Append(mr.card);
        rows.push_back(std::move(mr));
    }
    UpdateShowAll();
    ApplyMetrics();
}

void MixerPanel::RefreshMaster(bool force) {
    if (rows.empty() || !endpoint) return;
    if (!force && GetTickCount64() - lastMasterUserChange < 800) return;  // user dragging
    float level = 0;
    BOOL muted = FALSE;
    if (FAILED(endpoint->GetMasterVolumeLevelScalar(&level))) return;
    endpoint->GetMute(&muted);
    double v = std::round(level * 100.0);
    if (force || std::abs(rows[0].slider.Value() - v) >= 1) {
        suppressMaster = true;
        rows[0].slider.Value(v);
        suppressMaster = false;
    }
    rows[0].setMuted(muted != FALSE);
}

// Updates app sliders/mute in place (volume changed elsewhere), no rebuild.
void MixerPanel::SyncAppValues() {
    for (auto& r : rows) {
        float v = 0;
        BOOL muted = FALSE;
        if (r.vols.empty() || FAILED(r.vols.front()->GetMasterVolume(&v))) continue;
        r.vols.front()->GetMute(&muted);
        double target = std::round(v * 100.0);
        if (std::abs(r.slider.Value() - target) >= 1) r.slider.Value(target);  // handler re-applies the same value
        r.setMuted(muted != FALSE);
    }
}

// App rows are collapsed behind "Show N apps" until expanded; master always shows.
void MixerPanel::UpdateShowAll(bool animate) {
    size_t apps = rows.empty() ? 0 : rows.size() - 1;
    bool collapsible = apps > 0;
    bool open = !collapsible || showAll;
    int gen = ++showAllGen;

    // The panel grows from the bottom, so a sudden size change makes the whole
    // panel jump. Instead the rows' container height is animated (the panel
    // grows/shrinks smoothly, its content clipped), with the rows fading and
    // sliding up a little, on the sub-pages' decelerate curve.
    double from = rowsHost.ActualHeight();
    if (rowsHeightAnim) {  // interrupted: continue from where it is
        rowsHeightAnim.Stop();
        rowsHeightAnim = nullptr;
    }
    constexpr int kExpandMs = kInMs, kCollapseMs = 200;
    constexpr float kRise = 16;
    for (size_t i = 1; i < rows.size(); i++) {
        auto& card = rows[i].card;
        if (!animate) {
            card.Visibility(open ? Visibility::Visible : Visibility::Collapsed);
            ResetSlideFade(card);
        } else if (open) {
            card.Visibility(Visibility::Visible);
            SlideFadeY(card, kRise, 0, 0, 1, kExpandMs);
        } else {
            SlideFadeY(card, 0, kRise / 2, 1, 0, kCollapseMs / 2);
        }
    }
    if (!animate || from <= 0) {
        rowsHost.ClearValue(wux::FrameworkElement::HeightProperty());
        rowsClipRevoker.revoke();
        rowsHost.Clip(nullptr);
    } else {
        // Target height: the visible rows' natural heights (margins included).
        double width = rowsHost.ActualWidth(), to = 0;
        for (size_t i = 0; i < rows.size(); i++) {
            if (i > 0 && !open) break;
            rows[i].card.Measure({(float)width, std::numeric_limits<float>::infinity()});
            to += rows[i].card.DesiredSize().Height;
        }
        rowsHost.Height(from);
        // Rows outside the animated height are clipped, not drawn over the button below.
        auto clipTo = [host = rowsHost] {
            wuxm::RectangleGeometry clip;
            clip.Rect({0, 0, (float)host.ActualWidth(), (float)host.ActualHeight()});
            host.Clip(clip);
        };
        clipTo();
        rowsClipRevoker = rowsHost.SizeChanged(winrt::auto_revoke, [clipTo](auto&&...) { clipTo(); });
        wuxm::Animation::DoubleAnimation a;
        a.From(from);
        a.To(to);
        a.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(open ? kExpandMs : kCollapseMs)));
        a.EnableDependentAnimation(true);  // layout property: runs on the UI thread
        wuxm::Animation::ExponentialEase ease;
        ease.Exponent(6);
        ease.EasingMode(wuxm::Animation::EasingMode::EaseOut);
        a.EasingFunction(ease);
        wuxm::Animation::Storyboard sb;
        wuxm::Animation::Storyboard::SetTarget(a, rowsHost);
        wuxm::Animation::Storyboard::SetTargetProperty(a, L"Height");
        sb.Children().Append(a);
        sb.Completed([weak = weak_from_this(), gen](auto&&...) {
            auto self = weak.lock();
            if (!self || gen != self->showAllGen) return;  // a newer toggle took over
            if (self->rowsHeightAnim) self->rowsHeightAnim.Stop();
            self->rowsHeightAnim = nullptr;
            self->rowsHost.ClearValue(wux::FrameworkElement::HeightProperty());  // back to natural size
            self->rowsClipRevoker.revoke();
            self->rowsHost.Clip(nullptr);
            if (!self->showAll)
                for (size_t i = 1; i < self->rows.size(); i++) {
                    self->rows[i].card.Visibility(Visibility::Collapsed);
                    ResetSlideFade(self->rows[i].card);
                }
        });
        rowsHeightAnim = sb;
        sb.Begin();
    }
    showAllButton.Visibility(collapsible ? Visibility::Visible : Visibility::Collapsed);
    if (!collapsible) return;

    auto text = MakeCaption(showAll ? L"Show less" : L"Show " + std::to_wstring(apps) + (apps == 1 ? L" app" : L" apps"));
    ApplyTileFont(text, true);
    wuxc::StackPanel sp;
    sp.Orientation(wuxc::Orientation::Horizontal);
    sp.Spacing(6);

    // Collapsed: the apps' icons, overlapping like avatars, before the text.
    //   (o(o(o +2  Show 5 apps  v
    if (!showAll) {
        constexpr size_t kMaxIcons = 4;
        constexpr double kChip = 22, kOverlap = 7;
        wuxc::StackPanel stack;
        stack.Orientation(wuxc::Orientation::Horizontal);
        stack.VerticalAlignment(VerticalAlignment::Center);
        stack.Margin({0, 0, 2, 0});
        auto chip = [&](wux::UIElement const& content, bool first) {
            wuxc::Border b;
            b.Width(kChip);
            b.Height(kChip);
            b.CornerRadius({kChip / 2, kChip / 2, kChip / 2, kChip / 2});
            // Solid fill + panel-coloured ring keep overlapping icons apart.
            b.Background(ThemeBrush(L"SolidBackgroundFillColorTertiaryBrush", 0xFF2C2C2C, 0xFFF9F9F9));
            b.BorderBrush(ThemeBrush(L"CardStrokeColorDefaultBrush", 0x19000000, 0x0F000000));
            b.BorderThickness({1, 1, 1, 1});
            b.Margin({first ? 0.0 : -kOverlap, 0, 0, 0});
            b.Child(content);
            stack.Children().Append(b);
        };
        size_t shown = std::min(apps, kMaxIcons);
        for (size_t i = 1; i <= shown; i++) {
            wux::FrameworkElement icon = MakeGlyph(L"\xE767", 11);
            if (auto img = IconForExe(rows[i].exePath)) {
                wuxc::Image image;
                image.Source(img);
                image.Width(14);
                image.Height(14);
                icon = image;
            }
            icon.HorizontalAlignment(HorizontalAlignment::Center);
            icon.VerticalAlignment(VerticalAlignment::Center);
            chip(icon, i == 1);
        }
        if (apps > shown) {  // "+N" for the rest
            auto more = MakeSubtitle(L"+" + std::to_wstring(apps - shown));
            more.FontSize(10);
            more.HorizontalAlignment(HorizontalAlignment::Center);
            chip(more, false);
        }
        sp.Children().Append(stack);
    }
    auto chevron = MakeGlyph(showAll ? L"\xE70E" : L"\xE70D", 10);  // up / down
    chevron.VerticalAlignment(VerticalAlignment::Center);
    sp.Children().Append(text);
    sp.Children().Append(chevron);
    showAllButton.Content(sp);
}

// ---------------------------------------------------------------------------
// Sub-pages (opened by the arrows in the title rows, like the native ones)
//
//   [<]  Sound                          [<]  Brightness
//   Output   [device dropdown]          Main page   Combine monitors   [on]
//   Input    [device dropdown]          Monitors    one card per monitor:
//            + input volume, level        [▭] MPG321URX          [^] [v]
//                                             Show on main page    [on]
// ---------------------------------------------------------------------------

void MixerPanel::OpenPage(PageKind kind) {
    auto l1 = l1Ref.get();
    if (pageOpen || pageAnimating || !l1) return;
    ExitEdit();
    pageOpen = pageAnimating = true;
    pageKind = kind;
    // The page always takes exactly the main page's height (scrolls if longer).
    double mainHeight = l1.ActualHeight();

    // Main page elements (the unnamed background Borders stay) slide out to the left.
    std::vector<wux::FrameworkElement> mainEls;
    for (auto c : l1.Children())
        if (auto fe = c.try_as<wux::FrameworkElement>(); fe && !(fe.try_as<wuxc::Border>() && fe.Name().empty()))
            mainEls.push_back(fe);
    for (auto& fe : mainEls) SlideFade(fe, 0, -kSlide, 1, 0, kOutMs);

    AfterDelay(kOutMs, [weak = weak_from_this(), mainEls, mainHeight] {
        auto self = weak.lock();
        auto l1 = self ? self->l1Ref.get() : nullptr;
        if (self) self->pageAnimating = false;
        if (!l1 || !self->pageOpen) {  // closed mid-animation: undo the fade
            for (auto& fe : mainEls) ResetSlideFade(fe);
            return;
        }
        for (auto& fe : mainEls) {
            self->pageHidden.push_back({winrt::make_weak<wux::UIElement>(fe), fe.Visibility()});
            fe.Visibility(Visibility::Collapsed);
            ResetSlideFade(fe);  // so they come back clean
        }
        self->page = wuxc::Grid();
        if (mainHeight > 0) self->page.Height(mainHeight);
        wuxc::Grid::SetRowSpan(self->page, std::max(1, (int)l1.RowDefinitions().Size()));
        wuxc::Grid::SetColumnSpan(self->page, std::max(1, (int)l1.ColumnDefinitions().Size()));
        l1.Children().Append(self->page);
        self->BuildPage();
        SlideFade(self->page, kSlide, 0, 0, 1, kInMs);  // in from the right
    });
}

void MixerPanel::StopMic() {
    if (micClient) micClient->Stop();
    micCapture = nullptr;
    micClient = nullptr;
    micMeter = nullptr;
    micVolume = nullptr;
    micLevel = {};
}

void MixerPanel::ClosePage(bool animate) {
    if (!pageOpen) return;
    pageOpen = false;
    StopMic();

    auto finish = [this](bool slideIn) {
        if (auto l1 = l1Ref.get()) RemoveFrom(l1.Children(), page);
        page = nullptr;
        for (auto& [weakEl, visibility] : pageHidden)
            if (auto el = weakEl.get()) {
                el.Visibility(visibility);
                if (slideIn) SlideFade(el, -kSlide, 0, 0, 1, kInMs);  // back in from the left
            }
        pageHidden.clear();
        pageAnimating = false;
        UpdateCompact();  // tiles may have changed while the page was open
        if (pageKind == PageKind::Brightness) RefreshBrightness(true);  // order/visibility may have changed
    };
    if (!animate || !page) return finish(false);
    pageAnimating = true;
    SlideFade(page, 0, kSlide, 1, 0, kOutMs);  // out to the right
    AfterDelay(kOutMs, [weak = weak_from_this(), finish] {
        if (auto self = weak.lock()) finish(true);
    });
}

void MixerPanel::BuildPage() {
    if (!page) return;
    page.Children().Clear();
    StopMic();

    wuxc::StackPanel content;
    content.Padding({pageInsetL, 12, pageInsetR, 8});

    // [<] Title
    auto header = MakeGrid({kAuto, kStar});
    auto back = Put(header, FlatButton(32, MakeGlyph(L"\xE72B", 14), L"Back"), 0);
    back.Click(Deferred([](MixerPanel& p) { p.ClosePage(true); }));
    auto title = Put(header, MakeCaption(pageKind == PageKind::Brightness ? L"Brightness" : L"Sound"), 1);
    title.FontWeight({600});
    title.FontSize((g_tileFont.valid ? g_tileFont.size : 14) + 2);
    title.Margin({8, 0, 0, 0});
    content.Children().Append(header);

    if (pageKind == PageKind::Brightness) BuildBrightnessPage(content);
    else BuildSoundPage(content);

    wuxc::ScrollViewer sv;
    sv.VerticalAlignment(VerticalAlignment::Stretch);
    // Hidden (not Disabled) keeps the content measured at its natural height,
    // so the overflow check below sees the real size.
    sv.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Hidden);
    sv.VerticalScrollMode(wuxc::ScrollMode::Disabled);
    sv.Content(content);
    page.Children().Append(sv);

    // Scroll only when content is really cut off. A few pixels of overflow
    // (bottom padding) just get clipped instead of showing a scroll bar.
    auto updateScroll = [weakSv = winrt::make_weak(sv), weakContent = winrt::make_weak(content)](auto&&...) {
        auto v = weakSv.get();
        auto c = weakContent.get();
        if (!v || !c || v.ActualHeight() <= 0) return;
        bool needed = c.ActualHeight() > v.ActualHeight() + 16;
        v.VerticalScrollBarVisibility(needed ? wuxc::ScrollBarVisibility::Auto : wuxc::ScrollBarVisibility::Hidden);
        v.VerticalScrollMode(needed ? wuxc::ScrollMode::Enabled : wuxc::ScrollMode::Disabled);
        if (!needed) v.ChangeView(nullptr, 0.0, nullptr, true);
    };
    sv.SizeChanged(updateScroll);
    content.SizeChanged(updateScroll);
}

void MixerPanel::BuildSoundPage(wuxc::StackPanel const& content) {
    // Device dropdown card:  [icon] [ dropdown ............ v ]
    // Returns the card's body, so more rows (input volume) can be nested into it.
    auto addDevicePicker = [&](EDataFlow flow, PCWSTR heading, PCWSTR glyph, std::wstring& defaultId) {
        content.Children().Append(MakeSectionHeader(heading));
        auto devices = ListDevices(flow);
        defaultId = devices.empty() ? L"" : DeviceId(DefaultDevice(flow));

        auto g = MakeGrid({Px(32), kStar});
        auto icon = Put(g, MakeGlyph(glyph), 0);
        icon.HorizontalAlignment(HorizontalAlignment::Left);
        icon.VerticalAlignment(VerticalAlignment::Center);
        wuxc::StackPanel body;
        body.Children().Append(g);
        content.Children().Append(MakePageCard(body));
        if (devices.empty()) {
            Put(g, MakeSubtitle(L"No devices"), 1);
            return body;
        }

        auto cb = Put(g, wuxc::ComboBox(), 1);
        cb.HorizontalAlignment(HorizontalAlignment::Stretch);
        AutomationProperties::SetName(cb, flow == eRender ? L"Output device" : L"Input device");
        // "Speakers (Realtek Audio)" -> "Speakers", unless two devices share the short name.
        std::vector<std::wstring> ids, shorts;
        for (auto& d : devices) shorts.push_back(d.name.substr(0, d.name.find(L" (")));
        for (size_t i = 0; i < devices.size(); i++) {
            bool clash = std::count(shorts.begin(), shorts.end(), shorts[i]) > 1;
            cb.Items().Append(winrt::box_value(winrt::hstring(clash ? devices[i].name : shorts[i])));
            ids.push_back(devices[i].id);
            if (devices[i].id == defaultId) cb.SelectedIndex((int)i);
        }
        cb.SelectionChanged([ids, weak = weak_from_this()](auto&& sender, auto&&) {
            int i = sender.template as<wuxc::ComboBox>().SelectedIndex();
            auto self = weak.lock();
            if (!self || i < 0 || i >= (int)ids.size()) return;
            SetDefaultDevice(ids[i]);
            self->Later([](MixerPanel& p) {
                p.BuildPage();
                p.RefreshSessions(true);  // main page follows device changes
            });
        });
        return body;
    };

    std::wstring defOut, defMic;
    addDevicePicker(eRender, L"Output", L"\xE7F5", defOut);
    auto micCard = addDevicePicker(eCapture, L"Input", L"\xE720", defMic);

    // Input volume for the default input device, nested under its dropdown.
    auto dev = DeviceById(defMic);
    micVolume = Activate<IAudioEndpointVolume>(dev);
    if (!micVolume) return;
    micMeter = Activate<AudioMeter>(dev, kIID_AudioMeter);
    // A mic meter only moves while something records: open a light
    // shared-mode capture stream (Settings does the same).
    micClient = Activate<IAudioClient>(dev);
    if (micClient) {
        WAVEFORMATEX* fmt = nullptr;
        bool ok = SUCCEEDED(micClient->GetMixFormat(&fmt)) && fmt &&
                  SUCCEEDED(micClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, fmt, nullptr)) &&
                  SUCCEEDED(micClient->GetService(__uuidof(IAudioCaptureClient), micCapture.put_void())) &&
                  SUCCEEDED(micClient->Start());
        if (fmt) CoTaskMemFree(fmt);
        if (!ok) {
            micCapture = nullptr;
            micClient = nullptr;
        }
    }

    float level = 0;
    micVolume->GetMasterVolumeLevelScalar(&level);
    auto g = MakeGrid({Px(32), kStar, Px(36)}, {kAuto, kAuto, kAuto});
    g.Margin({0, 10, 0, 0});
    Put(g, MakeCaption(L"Input volume"), 1);
    auto sl = Put(g, MakeSlider(sliderStyle, std::round(level * 100), L"Input volume"), 1, 1);
    auto num = Put(g, MakeSubtitle(std::to_wstring((int)std::lround(level * 100))), 2, 1);
    num.HorizontalAlignment(HorizontalAlignment::Right);
    sl.ValueChanged([vol = micVolume, num](auto&&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& e) {
        vol->SetMasterVolumeLevelScalar((float)(e.NewValue() / 100.0), nullptr);
        num.Text(winrt::to_hstring((int)std::lround(e.NewValue())));
    });
    micLevel = Meter::Make();
    Put(g, micLevel.host, 1, 2);
    micCard.Children().Append(g);

    // Listen to this device ............................ [on]
    //     [ Default playback device ................ v ]   (while on)
    auto listen = GetListen(dev);
    auto lg = MakeGrid({Px(32), kStar, kAuto});
    lg.Margin({0, 12, 0, 0});
    Put(lg, MakeCaption(L"Listen to this device"), 1);
    auto ts = Put(lg, MakeToggle(listen.on, L"Listen to this device"), 2);
    ts.Toggled([weak = weak_from_this(), micId = defMic, target = listen.target](auto&& sender, auto&&) {
        SetListen(micId, {sender.template as<wuxc::ToggleSwitch>().IsOn(), target});
        if (auto self = weak.lock()) self->Later([](MixerPanel& p) { p.BuildPage(); });  // show/hide the picker
    });
    micCard.Children().Append(lg);
    if (!listen.on) return;

    auto pg = MakeGrid({Px(32), kStar});
    pg.Margin({0, 8, 0, 0});
    auto cb = Put(pg, wuxc::ComboBox(), 1);
    cb.HorizontalAlignment(HorizontalAlignment::Stretch);
    AutomationProperties::SetName(cb, L"Play through this device");
    std::vector<std::wstring> ids{L""};
    cb.Items().Append(winrt::box_value(L"Default playback device"));
    cb.SelectedIndex(0);
    for (auto& d : ListDevices(eRender)) {
        cb.Items().Append(winrt::box_value(winrt::hstring(d.name)));
        ids.push_back(d.id);
        if (_wcsicmp(d.id.c_str(), listen.target.c_str()) == 0) cb.SelectedIndex((int)ids.size() - 1);
    }
    cb.SelectionChanged([ids, micId = defMic](auto&& sender, auto&&) {
        int i = sender.template as<wuxc::ComboBox>().SelectedIndex();
        if (i >= 0 && i < (int)ids.size()) SetListen(micId, {true, ids[i]});
    });
    micCard.Children().Append(pg);
}

// ---------------------------------------------------------------------------
// Monitor brightness: main-page rows + Brightness page
// ---------------------------------------------------------------------------

bool MixerPanel::IsMonitorHidden(const std::wstring& key) const {
    return std::find(monitorHidden.begin(), monitorHidden.end(), key) != monitorHidden.end();
}

// Monitors in the user's order; new ones go to the end.
std::vector<MonitorState> MixerPanel::OrderedMonitors() const {
    auto out = g_ddc.Snapshot();
    auto rank = [&](const MonitorState& m) {
        return std::find(monitorOrder.begin(), monitorOrder.end(), m.key) - monitorOrder.begin();
    };
    std::stable_sort(out.begin(), out.end(), [&](auto& a, auto& b) { return rank(a) < rank(b); });
    return out;
}

// A slider (this one, or any) moved in the last moment: don't overwrite it with a stale read.
bool MixerPanel::BrightDragging(const std::wstring* key) const {
    ULONGLONG now = GetTickCount64();
    for (auto& [k, t] : brightUserChange)
        if ((!key || *key == k) && now - t < 1500) return true;
    return false;
}

// force: rebuild rows. Otherwise rows are rebuilt only when the set of shown
// monitors changed, and values are updated in place.
void MixerPanel::RefreshBrightness(bool force) {
    if (!root) return;
    std::vector<MonitorState> shown;
    bool any = false;
    if (g_settings.brightness)
        for (auto& m : OrderedMonitors()) {
            if (!m.Controllable()) continue;
            any = true;
            if (!IsMonitorHidden(m.key)) shown.push_back(m);
        }
    // Keep the section (and its arrow) while controllable monitors exist, even
    // if all are hidden from the main page, so they can be shown again.
    brightHost.Visibility(any ? Visibility::Visible : Visibility::Collapsed);

    // One slider per monitor, or one for all of them when combined.
    struct Slot {
        std::wstring name;
        std::vector<std::wstring> keys;
        int value;
    };
    std::vector<Slot> slots;
    for (auto& m : shown) slots.push_back({m.name, {m.key}, m.value});
    if (linkBrightness && shown.size() > 1) {
        slots = {{L"All monitors", {}, shown.front().value}};
        for (auto& m : shown) slots[0].keys.push_back(m.key);
    }

    std::vector<std::wstring> ids;  // identity of the current rows
    for (auto& s : slots) {
        ids.emplace_back();
        for (auto& k : s.keys) ids.back() += k + L'|';
    }
    if (!force && ids == brightKeys) {
        for (size_t i = 0; i < slots.size() && i < brightRows.size(); i++) {
            auto& sl = brightRows[i].slider;
            if (slots[i].value < 0 || BrightDragging(&slots[i].keys.front())) continue;
            if (std::abs(sl.Value() - slots[i].value) >= 1) {
                suppressBright = true;
                sl.Value(slots[i].value);
                suppressBright = false;
            }
        }
        return;
    }
    brightKeys = ids;
    brightRows.clear();
    brightRowsHost.Children().Clear();

    for (auto& s : slots) {
        auto mr = BuildRow(s.name, MakeGlyph(L"\xE706"), std::max(0, s.value), sliderStyle);
        // Brightness has no mute and no level line.
        mr.iconButton.IsHitTestVisible(false);
        mr.iconButton.IsTabStop(false);
        wuxc::ToolTipService::SetToolTip(mr.iconButton, nullptr);
        AutomationProperties::SetName(mr.iconButton, L"");
        mr.meter.host.Visibility(Visibility::Collapsed);
        mr.valueRevoker = mr.slider.ValueChanged(
            winrt::auto_revoke, [weak = weak_from_this(), targets = s.keys](
                                    auto&&, wuxc::Primitives::RangeBaseValueChangedEventArgs const& e) {
                auto self = weak.lock();
                if (!self || self->suppressBright) return;
                for (auto& key : targets) {
                    self->brightUserChange[key] = GetTickCount64();
                    g_ddc.Set(key, (int)std::lround(e.NewValue()));
                }
            });
        brightRowsHost.Children().Append(mr.card);
        brightRows.push_back(std::move(mr));
    }
    ApplyMetrics();
}

// A refresh finished on the DDC worker.
void MixerPanel::OnMonitorsChanged() {
    RefreshBrightness(false);
    if (pageOpen && pageKind == PageKind::Brightness && !pageAnimating && !BrightDragging()) BuildPage();
}

void MixerPanel::MoveMonitor(const std::wstring& key, int delta) {
    std::vector<std::wstring> order;
    for (auto& m : OrderedMonitors()) order.push_back(m.key);
    int i = (int)(std::find(order.begin(), order.end(), key) - order.begin()), j = i + delta;
    if (i >= (int)order.size() || j < 0 || j >= (int)order.size()) return;
    std::swap(order[i], order[j]);
    SaveList(L"monitorOrder", monitorOrder = order);
    BuildPage();
}

void MixerPanel::BuildBrightnessPage(wuxc::StackPanel const& content) {
    auto weak = weak_from_this();
    // [icon] text ................................ [right]   (the card's rows share these columns)
    auto line = [](PCWSTR glyph, wuxc::TextBlock const& text) {
        auto g = MakeGrid({Px(32), kStar, kAuto, kAuto});
        auto icon = Put(g, MakeGlyph(glyph ? glyph : L""), 0);
        icon.HorizontalAlignment(HorizontalAlignment::Left);
        icon.VerticalAlignment(VerticalAlignment::Center);
        Put(g, text, 1);
        return g;
    };

    // Combine all monitors into one slider on the main page.
    auto combine = line(L"\xE71B", MakeCaption(L"Combine monitors into one slider"));
    Put(combine, MakeToggle(linkBrightness, L"Combine monitors into one slider"), 2)
        .Toggled([weak](auto&& sender, auto&&) {
            if (auto self = weak.lock())
                SaveList(L"linkBrightness",
                         {(self->linkBrightness = sender.template as<wuxc::ToggleSwitch>().IsOn()) ? L"1" : L"0"});
        });
    content.Children().Append(MakeSectionHeader(L"Main page"));
    content.Children().Append(MakePageCard(combine));

    content.Children().Append(MakeSectionHeader(L"Monitors"));
    auto monitors = OrderedMonitors();
    if (monitors.empty()) content.Children().Append(MakePageCard(MakeSubtitle(L"No external monitors found")));

    for (size_t i = 0; i < monitors.size(); i++) {
        auto& m = monitors[i];
        wuxc::StackPanel body;
        content.Children().Append(MakePageCard(body));

        // [▭] Name ........................ [^] [v]
        auto top = line(L"\xE7F4", MakeCaption(m.name));
        auto up = Put(top, FlatButton(28, MakeGlyph(L"\xE70E", 12), L"Move up"), 2);
        auto down = Put(top, FlatButton(28, MakeGlyph(L"\xE70D", 12), L"Move down"), 3);
        up.IsEnabled(i > 0);
        down.IsEnabled(i + 1 < monitors.size());
        up.Click(Deferred([key = m.key](MixerPanel& p) { p.MoveMonitor(key, -1); }));
        down.Click(Deferred([key = m.key](MixerPanel& p) { p.MoveMonitor(key, 1); }));
        body.Children().Append(top);

        if (m.hdr) body.Children().Append(line(nullptr, MakeSubtitle(L"HDR is on: sets SDR content brightness")));
        if (!m.Controllable()) {
            auto note = MakeSubtitle(L"DDC/CI isn't available. Turn it on in the monitor's menu.");
            note.TextWrapping(wux::TextWrapping::Wrap);
            body.Children().Append(line(nullptr, note));
            continue;
        }

        //     Show on main page                 [on]
        auto show = line(nullptr, MakeSubtitle(L"Show on main page"));
        Put(show, MakeToggle(!IsMonitorHidden(m.key), L"Show " + m.name + L" on main page"), 2)
            .Toggled([weak, key = m.key](auto&& sender, auto&&) {
                auto self = weak.lock();
                if (!self) return;
                std::erase(self->monitorHidden, key);
                if (!sender.template as<wuxc::ToggleSwitch>().IsOn()) self->monitorHidden.push_back(key);
                SaveList(L"monitorHidden", self->monitorHidden);
            });
        body.Children().Append(show);
    }
}

// ---------------------------------------------------------------------------
// Tiles: filtered view, one-row collapse, scroll lock
// ---------------------------------------------------------------------------

void MixerPanel::ApplyTiles(wuxc::Grid const& l1) {
    auto weak = weak_from_this();
    l1Ref = l1;
    auto toggles = FindChildByName(l1, L"TogglesGroup");
    if (!toggles) return;
    togglesRef = toggles;
    auto gv = FindChildByName(toggles, L"RootGridView").try_as<wuxc::GridView>();
    if (!gv) return;
    tileGrid = gv;
    if (!pinnedHeader) InsertPinnedHeader(l1, toggles);
    gridSizeRevoker = gv.SizeChanged(winrt::auto_revoke, [weak](auto&&...) {
        if (auto self = weak.lock()) self->UpdateCompact();  // no late snap to one row
    });

    // Copy the tile caption font (the last real-text TextBlock of a tile), then rebuild rows with it.
    for (uint32_t i = 0; i < gv.Items().Size() && !g_tileFont.valid; i++) {
        wuxc::TextBlock caption{nullptr};
        Walk(gv.ContainerFromIndex(i), 160, [&](wux::DependencyObject const& node, bool) {
            if (auto tb = node.try_as<wuxc::TextBlock>(); tb && !IsGlyphText(tb.Text())) caption = tb;
        });
        if (!caption) continue;
        g_tileFont = {true, caption.FontFamily(), caption.FontSize(), caption.FontWeight(), caption.Foreground()};
        Wh_Log(L"Tile font: %s %.1f", g_tileFont.family ? g_tileFont.family.Source().c_str() : L"?", g_tileFont.size);
        RefreshSessions(true);
    }

    nativeTilesSource = gv.ItemsSource();
    nativeTiles = nativeTilesSource.try_as<TileList>();
    if (!nativeTiles) {
        Wh_Log(L"Tile source isn't an observable vector; pin/unpin disabled");
    } else {
        LearnTileNames();  // every tile is realized right now
        tileSource = winrt::single_threaded_observable_vector<wf::IInspectable>();
        nativeTilesToken = nativeTiles.VectorChanged([weak](auto&&...) {
            if (auto self = weak.lock()) self->RebuildTileView();  // follow Windows' changes
        });
        RebuildTileView();
        gv.ItemsSource(tileSource);
    }

    // "Edit quick settings" lives in the tiles' right-click menu, and on the
    // whole panel too, so tiles can be re-pinned even when all are unpinned.
    AttachTileMenus();
    if (tileSource && !panelFlyout) {
        panelFlyout = wuxc::MenuFlyout();
        panelFlyout.Items().Append(MakeEditMenuItem());
        l1ContextRevoker = l1.ContextRequested(
            winrt::auto_revoke, [weak](wux::UIElement const& sender, wux::Input::ContextRequestedEventArgs const& e) {
                auto self = weak.lock();
                if (!self || !self->panelFlyout || self->pageOpen || self->editMode) return;
                e.Handled(true);
                wuxc::Primitives::FlyoutShowOptions opts;
                wf::Point pt;
                if (e.TryGetPosition(sender, pt)) opts.Position(pt);
                self->panelFlyout.ShowAt(sender, opts);
            });
    }

    if (LearnTileNames()) RebuildTileView();
    UpdateCompact();
}

bool MixerPanel::IsUnpinned(const std::wstring& name) const {
    return std::any_of(unpinned.begin(), unpinned.end(),
                       [&](auto& n) { return _wcsicmp(n.c_str(), name.c_str()) == 0; });
}

// Records names of realized tiles (names need a realized container). Returns
// true when a tile that should be hidden was found visible.
bool MixerPanel::LearnTileNames() {
    auto gv = tileGrid.get();
    bool hiddenVisible = false;
    for (uint32_t i = 0; gv && i < gv.Items().Size(); i++) {
        auto item = gv.Items().GetAt(i);
        auto it = tileNames.find(TileKey(item));
        if (it == tileNames.end()) {
            std::wstring name = TileName(gv.ContainerFromIndex(i));
            if (name.empty()) continue;
            it = tileNames.insert({TileKey(item), {item, name}}).first;
        }
        if (IsUnpinned(it->second.second)) hiddenVisible = true;
    }
    return hiddenVisible;
}

// Names tiles that were never realized: an invisible ItemsControl with the
// tile grid's item template builds them off-layout.
void MixerPanel::ProbeTileNames(std::vector<wf::IInspectable> const& items) {
    auto gv = tileGrid.get();
    auto l1 = l1Ref.get();
    if (!gv || !l1 || items.empty()) return;
    try {
        if (!probe) {
            // A zero-size Canvas: its children are laid out but never affect the page layout.
            probeHost = wuxc::Canvas();
            probeHost.Width(0);
            probeHost.Height(0);
            probeHost.Opacity(0);
            probeHost.IsHitTestVisible(false);
            AutomationProperties::SetAccessibilityView(probeHost, wux::Automation::Peers::AccessibilityView::Raw);
            probe = wuxc::ItemsControl();
            probe.ItemTemplate(gv.ItemTemplate());
            probe.ItemTemplateSelector(gv.ItemTemplateSelector());
            probe.Width(std::max(100.0, gv.ActualWidth()));
            probeHost.Children().Append(probe);
            l1.Children().Append(probeHost);
        }
        probe.ItemsSource(winrt::single_threaded_vector(std::vector(items)));
        probe.UpdateLayout();  // realize the templates synchronously
        for (uint32_t i = 0; i < items.size(); i++) {
            std::wstring name = TileName(probe.ContainerFromIndex(i));
            if (!name.empty()) tileNames[TileKey(items[i])] = {items[i], name};
        }
        probe.ItemsSource(nullptr);
    } catch (...) {
        Wh_Log(L"Tile probe failed: %08X", winrt::to_hresult());
    }
}

// Filtered copy of Windows' tile list: everything except unpinned tiles.
// Tiles without a known name use the saved position guess, else stay visible.
void MixerPanel::RebuildTileView() {
    if (!nativeTiles || !tileSource) return;
    // Name any tile we haven't seen yet before deciding what to show.
    // Never probe a tile that is on screen: a second copy of its template can
    // take over its animated icon (Night light turned white on its "on" tile).
    // On-screen tiles are named from their real containers instead.
    std::set<void*> shown;
    for (auto item : tileSource) shown.insert(TileKey(item));
    std::vector<wf::IInspectable> unknown;
    for (auto item : nativeTiles)
        if (auto key = TileKey(item); !tileNames.count(key) && !shown.count(key)) unknown.push_back(item);
    if (!unknown.empty() && !unpinned.empty()) ProbeTileNames(unknown);

    uint32_t n = nativeTiles.Size();
    std::vector<wf::IInspectable> want;
    std::vector<std::wstring> order;
    bool allNamed = n > 0;
    for (uint32_t i = 0; i < n; i++) {
        auto item = nativeTiles.GetAt(i);
        std::wstring name;
        if (auto it = tileNames.find(TileKey(item)); it != tileNames.end()) {
            name = it->second.second;
        } else {
            allNamed = false;
            if (i < savedTileOrder.size()) name = savedTileOrder[i];  // best guess until named
        }
        order.push_back(name);
        if (name.empty() || !IsUnpinned(name)) want.push_back(item);
    }
    if (allNamed && order != savedTileOrder) SaveList(L"tileOrder", savedTileOrder = order);

    // Skip when unchanged (avoids re-realizing every tile).
    bool same = want.size() == tileSource.Size();
    for (uint32_t i = 0; same && i < want.size(); i++) same = TileKey(tileSource.GetAt(i)) == TileKey(want[i]);
    if (!same) {
        tileSource.ReplaceAll(want);
        Wh_Log(L"Tile view: %u of %u tiles shown", (uint32_t)want.size(), n);
    }
    UpdateCompact();
}

void MixerPanel::SetPinned(const std::wstring& name, bool pinned) {
    if (name.empty() || !tileSource) return;
    if (pinned) std::erase_if(unpinned, [&](auto& n) { return _wcsicmp(n.c_str(), name.c_str()) == 0; });
    else if (!IsUnpinned(name)) unpinned.push_back(name);
    SaveList(L"unpinnedTiles", unpinned);
    Wh_Log(L"%s tile '%s'", pinned ? L"Pinned" : L"Unpinned", name.c_str());
    RebuildTileView();
}

// With two rows or fewer there is only one page: no scrolling, no page flips.
// The paged grid scrolls itself from code on mouse wheel, so ScrollMode alone
// isn't enough. With the vertical scroll bar *Disabled* the ScrollViewer
// constrains its content to the viewport: there is nothing left to scroll,
// even from code. Wheel input is also swallowed so the pager never reacts.
void MixerPanel::UpdateScrollLock(uint32_t count, int perRow) {
    auto gv = tileGrid.get();
    auto toggles = togglesRef.get();
    bool lock = count > 0 && (int)count <= perRow * 2;
    if (!gv || !toggles || lock == scrollLocked) return;
    scrollLocked = lock;

    if (lock) {
        origScrollMode = wuxc::ScrollViewer::GetVerticalScrollMode(gv);
        origScrollBar = wuxc::ScrollViewer::GetVerticalScrollBarVisibility(gv);
        if (auto sv = FindChildByName(gv, L"ScrollViewer").try_as<wuxc::ScrollViewer>())
            sv.ChangeView(0.0, 0.0, nullptr, true);  // first page, before locking
        // Handled on the items panel (first in the route, over the tiles) and on
        // the grid itself (gaps between tiles), before the paging logic on ancestors.
        auto swallow = [](auto&&, wux::Input::PointerRoutedEventArgs const& e) { e.Handled(true); };
        if (auto panel = gv.ItemsPanelRoot()) panelWheelRevoker = panel.PointerWheelChanged(winrt::auto_revoke, swallow);
        wheelRevoker = gv.PointerWheelChanged(winrt::auto_revoke, swallow);
    } else {
        wheelRevoker.revoke();
        panelWheelRevoker.revoke();
    }
    wuxc::ScrollViewer::SetVerticalScrollMode(gv, lock ? wuxc::ScrollMode::Disabled : origScrollMode);
    wuxc::ScrollViewer::SetVerticalScrollBarVisibility(gv, lock ? wuxc::ScrollBarVisibility::Disabled : origScrollBar);
    // PaginatedGridView also flips pages from these edge sensors.
    for (auto name : {L"NextPageSensor", L"PreviousPageSensor", L"QuickActionsPager"})
        if (auto el = FindChildByName(toggles, name)) lock ? Hide(el) : Unhide(el);
}

void MixerPanel::UpdateCompact() {
    auto gv = tileGrid.get();
    auto toggles = togglesRef.get();
    if (!gv || !toggles) return;

    // No pinned tiles: drop the tile area and its "Pinned" title (Edit stays on
    // the panel's right-click); the title comes back while editing for the + button.
    // A sub-page owns the main page's visibility while it is open or sliding:
    // showing these here would put them on top of it. ClosePage re-syncs.
    uint32_t count = gv.Items().Size();
    bool empty = count == 0 && tileSource;
    if (!pageOpen && !pageAnimating) {
        empty ? Hide(toggles) : Unhide(toggles);
        if (pinnedHeader) pinnedHeader.Visibility(empty && !editMode ? Visibility::Collapsed : Visibility::Visible);
    }

    auto wrap = gv.ItemsPanelRoot().try_as<wuxc::ItemsWrapGrid>();
    int perRow = wrap && wrap.MaximumRowsOrColumns() > 0 ? wrap.MaximumRowsOrColumns() : 3;
    double itemH = wrap ? wrap.ItemHeight() : 0;
    if (!(itemH > 0))
        if (auto first = gv.ContainerFromIndex(0).try_as<wux::FrameworkElement>())
            itemH = first.ActualHeight() + first.Margin().Top + first.Margin().Bottom;
    // Right after injection the new list has no containers yet: reuse the
    // height measured a moment earlier, so one-row applies before the first
    // frame instead of on the next tick (visible jump after ShellHost restarts).
    if (itemH > 0) tileItemH = itemH;
    else itemH = tileItemH;
    bool oneRow = g_settings.compactTiles && count > 0 && (int)count <= perRow && itemH > 0;
    UpdateScrollLock(count, perRow);

    // Page dots only when the tiles need more than one page (2 rows). Windows
    // sizes the pager for its own full list, so enforce it on every check.
    if (auto pager = FindChildByName(toggles, L"QuickActionsPager")) {
        if (count > (uint32_t)perRow * 2) {
            Unhide(pager);
        } else {
            Hide(pager);
            if (pager.Visibility() != Visibility::Collapsed) pager.Visibility(Visibility::Collapsed);
        }
    }
    static int lastLogged = -1;
    if (int state = (int)count * 10 + oneRow; state != lastLogged) {
        lastLogged = state;
        Wh_Log(L"Compact check: %u tiles, %d per row, item %.0f, grid %.0f -> %s", count, perRow, itemH,
               gv.ActualHeight(), oneRow ? L"one row" : L"keep");
    }

    if (!oneRow) {
        for (auto& c : compactHeights)
            if (auto el = c.el.get()) el.Height(c.orig);
        compactHeights.clear();
    } else if (!compactHeights.empty()) {
        // Windows can reset the grid size when Quick Settings rebuilds: put our
        // one-row heights straight back (called from SizeChanged, before render).
        if (gv.ActualHeight() > itemH + 1)
            for (auto& c : compactHeights)
                if (auto el = c.el.get()) el.Height(c.target);
    } else {
        double delta = gv.ActualHeight() - itemH;
        if (gv.ActualHeight() <= 0 || delta <= 1) return;  // not laid out yet / already one row
        // The two-row height is fixed somewhere between the grid and
        // TogglesGroup: shrink every explicitly sized element on that path.
        wux::DependencyObject it = gv;
        for (int depth = 0; depth < 12 && it; depth++, it = VisualTreeHelper::GetParent(it)) {
            auto fe = it.try_as<wux::FrameworkElement>();
            if (!fe) continue;
            double h = fe.Height();
            if (!std::isnan(h) || fe == gv) {
                double target = std::isnan(h) ? itemH : std::max(itemH, h - delta);
                compactHeights.push_back({winrt::make_weak(fe), h, target});
                fe.Height(target);
                Wh_Log(L"Compact: %s %.0f -> %.0f", winrt::get_class_name(fe).c_str(),
                       std::isnan(h) ? fe.ActualHeight() : h, target);
            }
            if (fe == toggles) break;
        }
    }
}

// ---------------------------------------------------------------------------
// "Edit quick settings": Pinned header, menus, edit mode
// ---------------------------------------------------------------------------

// Pinned ........................ [+] [✎]   in its own L1Grid row above the tiles.
void MixerPanel::InsertPinnedHeader(wuxc::Grid const& l1, wux::FrameworkElement const& toggles) {
    pinnedAddButton = FlatButton(28, MakeGlyph(L"\xE710", 12), L"Add a tile");  // shown while editing
    pinnedAddButton.Visibility(Visibility::Collapsed);
    pinnedEditButton = FlatButton(28, MakeGlyph(L"\xE70F", 12), L"Edit pinned");
    pinnedEditButton.Click(Deferred([](MixerPanel& p) { p.editMode ? p.ExitEdit() : p.EnterEdit(); }));
    pinnedHeader = MakeTitle(L"Pinned", {pinnedAddButton, pinnedEditButton}, 4);

    // New row at the tiles' row: the tiles move down one row; anything else
    // starting there (the panel background) or spanning it grows to cover it.
    int at = wuxc::Grid::GetRow(toggles);
    pinnedRow.Insert(
        l1, at, [&](auto& fe, int row) { return row > at || (row == at && fe == toggles); },
        [](auto&, int) { return false; });
    // Same column as the mixer, so the title lines up with the other titles.
    auto& ref = root ? static_cast<wux::FrameworkElement const&>(root) : toggles;
    wuxc::Grid::SetRow(pinnedHeader, at);
    wuxc::Grid::SetColumn(pinnedHeader, wuxc::Grid::GetColumn(ref));
    wuxc::Grid::SetColumnSpan(pinnedHeader, wuxc::Grid::GetColumnSpan(ref));
    l1.Children().Append(pinnedHeader);
    ApplyMetrics();
}

wuxc::MenuFlyoutItem MixerPanel::MakeEditMenuItem(PCWSTR text, PCWSTR glyph) {
    wuxc::MenuFlyoutItem mi;
    mi.Text(text);
    wuxc::FontIcon icon;
    icon.Glyph(glyph);
    mi.Icon(icon);
    mi.Click(Deferred([](MixerPanel& p) { p.editMode ? p.ExitEdit() : p.EnterEdit(); }));  // after the menu closes
    return mi;
}

// Adds "Edit quick settings" to every tile's native context menu (the one
// with "Go to Settings"), and gives the empty tile area a menu with it too.
void MixerPanel::AttachTileMenus() {
    auto gv = tileGrid.get();
    auto toggles = togglesRef.get();
    if (!gv || !toggles || !tileSource) return;

    for (uint32_t i = 0; i < gv.Items().Size(); i++) {
        auto container = gv.ContainerFromIndex(i);
        if (!container || !scannedContainers.insert(winrt::get_abi(container)).second) continue;
        Walk(container, 48, [&](wux::DependencyObject const& node, bool) {
            auto ue = node.try_as<wux::UIElement>();
            auto mf = ue ? ue.ContextFlyout().try_as<wuxc::MenuFlyout>() : nullptr;
            if (!mf || std::any_of(menuAdds.begin(), menuAdds.end(), [&](auto& m) { return m.first.get() == mf; }))
                return;
            wuxc::MenuFlyoutSeparator sep;
            auto item = MakeEditMenuItem();
            mf.Items().Append(sep);
            mf.Items().Append(item);
            menuAdds.push_back({mf, {sep, item}});
        });
    }

    // Empty space around the tiles (and tiles without a native menu).
    if (!toggles.ContextFlyout() && !ownTogglesFlyout.get()) {
        wuxc::MenuFlyout mf;
        mf.Items().Append(MakeEditMenuItem());
        toggles.ContextFlyout(mf);
        ownTogglesFlyout = mf;
    }
}

void MixerPanel::EnterEdit() {
    auto l1 = l1Ref.get();
    auto toggles = togglesRef.get();
    if (editMode || pageOpen || pageAnimating || !l1 || !toggles || !tileSource) return;
    editMode = true;
    overlaySig.clear();
    pinnedHeader.Visibility(Visibility::Visible);
    pinnedEditButton.Content(MakeGlyph(L"\xE73E", 12));  // check = done

    // Overlay over the tiles: blocks normal clicks and hosts the unpin badges.
    overlay = wuxc::Canvas();
    overlay.Background(TransparentBrush());
    wuxc::Grid::SetRow(overlay, wuxc::Grid::GetRow(toggles));
    wuxc::Grid::SetRowSpan(overlay, wuxc::Grid::GetRowSpan(toggles));
    wuxc::Grid::SetColumn(overlay, wuxc::Grid::GetColumn(toggles));
    wuxc::Grid::SetColumnSpan(overlay, wuxc::Grid::GetColumnSpan(toggles));
    wuxc::Canvas::SetZIndex(overlay, 100);
    wuxc::MenuFlyout done;
    done.Items().Append(MakeEditMenuItem(L"Done editing", L"\xE73E"));
    overlay.ContextFlyout(done);
    l1.Children().Append(overlay);

    // [+] next to the checkmark: flyout with the unpinned tiles.
    wuxc::MenuFlyout add;
    for (auto& name : unpinned) {
        wuxc::MenuFlyoutItem mi;
        mi.Text(name);
        mi.Click(Deferred([name](MixerPanel& p) {
            p.SetPinned(name, true);
            p.ExitEdit();
            p.EnterEdit();  // rebuild the Add list and badges
        }));
        add.Items().Append(mi);
    }
    pinnedAddButton.Flyout(add);
    pinnedAddButton.IsEnabled(!unpinned.empty());
    pinnedAddButton.Visibility(Visibility::Visible);

    // Badges need laid-out tiles.
    Later([](MixerPanel& p) { p.UpdateOverlay(); }, winrt::Windows::System::DispatcherQueuePriority::Low);
}

// Unpin badges at the top-right corner of every tile on the current page.
void MixerPanel::UpdateOverlay() {
    auto gv = tileGrid.get();
    if (!editMode || !overlay || !gv || !tileSource) return;

    struct Badge {
        std::wstring name;
        double x, y;
    };
    std::vector<Badge> badges;
    std::wstring sig;
    double gvW = gv.ActualWidth(), gvH = gv.ActualHeight();
    for (auto item : tileSource) {
        auto container = gv.ContainerFromItem(item).try_as<wux::FrameworkElement>();
        if (!container || container.ActualWidth() <= 0) continue;
        auto inGv = container.TransformToVisual(gv).TransformPoint({0, 0});
        if (inGv.X < -1 || inGv.Y < -1 || inGv.X + 1 > gvW || inGv.Y + 1 > gvH) continue;  // other page
        auto pt = container.TransformToVisual(overlay).TransformPoint({0, 0});
        std::wstring name = TileName(container);
        if (name.empty()) continue;
        badges.push_back({name, pt.X + container.ActualWidth() - 16, pt.Y - 6});
        sig += name + L"@" + std::to_wstring((int)pt.X) + L"," + std::to_wstring((int)pt.Y) + L";";
    }
    if (sig == overlaySig) return;  // nothing moved (e.g. same page)
    overlaySig = sig;
    overlay.Children().Clear();

    for (auto& b : badges) {
        wuxc::Button badge;
        badge.Width(22);
        badge.Height(22);
        badge.Padding({});
        badge.CornerRadius({11, 11, 11, 11});
        if (auto br = Resource<wuxm::Brush>(L"SolidBackgroundFillColorTertiaryBrush")) badge.Background(br);
        badge.Content(MakeGlyph(L"\xE77A", 11));  // unpin
        wuxc::ToolTipService::SetToolTip(badge, winrt::box_value(winrt::hstring(L"Unpin " + b.name)));
        badge.Click(Deferred([name = b.name](MixerPanel& p) {
            p.SetPinned(name, false);
            p.ExitEdit();
            p.EnterEdit();
        }));
        wuxc::Canvas::SetLeft(badge, b.x);
        wuxc::Canvas::SetTop(badge, b.y);
        overlay.Children().Append(badge);
    }
}

void MixerPanel::ExitEdit() {
    if (!editMode) return;
    editMode = false;
    pinnedEditButton.Content(MakeGlyph(L"\xE70F", 12));
    pinnedAddButton.Visibility(Visibility::Collapsed);
    pinnedAddButton.Flyout(nullptr);
    if (auto l1 = l1Ref.get()) RemoveFrom(l1.Children(), overlay);
    overlay = nullptr;
    UpdateCompact();  // re-hide the tile area if nothing ended up pinned
}

// ---------------------------------------------------------------------------
// Restore: undo every native change, in reverse order
// ---------------------------------------------------------------------------

void MixerPanel::Destroy() {
    try {
        ExitEdit();
        ClosePage();
        if (rowsHeightAnim) rowsHeightAnim.Stop();  // its Completed handler is mod code
        rowsHeightAnim = nullptr;
        rowsClipRevoker.revoke();
        if (timer) {
            timer.Stop();
            timer.Tick(tickToken);
            visTimer.Stop();
            visTimer.Tick(visTickToken);
        }
        SetMetersActive(false);
        g_audioEvents.Stop();

        // Menus
        auto toggles = togglesRef.get();
        if (auto own = ownTogglesFlyout.get(); own && toggles && toggles.ContextFlyout() == own)
            toggles.ContextFlyout(nullptr);
        for (auto& [menu, items] : menuAdds)
            if (auto mf = menu.get())
                for (auto& item : items) RemoveFrom(mf.Items(), item.get());
        menuAdds.clear();
        l1ContextRevoker.revoke();
        panelFlyout = nullptr;

        // Tiles: hand Windows its own, untouched list back.
        auto gv = tileGrid.get();
        auto l1 = l1Ref.get();
        if (nativeTiles) nativeTiles.VectorChanged(nativeTilesToken);
        gridSizeRevoker.revoke();
        if (gv && nativeTilesSource) gv.ItemsSource(nativeTilesSource);
        nativeTiles = nullptr;
        nativeTilesSource = nullptr;
        tileSource = nullptr;
        tileNames.clear();
        UpdateScrollLock(0, 1);  // unlock
        if (l1) RemoveFrom(l1.Children(), probeHost);
        probe = nullptr;
        probeHost = nullptr;
        for (auto& c : compactHeights)
            if (auto el = c.el.get()) el.Height(c.orig);
        compactHeights.clear();
        for (auto& [weakEl, visibility] : hidden)
            if (auto el = weakEl.get()) el.Visibility(visibility);
        hidden.clear();

        // Pinned header, then the mixer itself.
        if (l1) RemoveFrom(l1.Children(), pinnedHeader);
        pinnedRow.Remove();
        rows.clear();
        brightRows.clear();
        if (root)
            if (auto parent = VisualTreeHelper::GetParent(root).try_as<wuxc::Panel>())
                RemoveFrom(parent.Children(), root);
        auto hostGrid = mixerRow.grid.get();
        mixerRow.Remove();
        if (hostGrid) RemoveFrom(hostGrid.RowDefinitions(), addedFirstRow);
    } catch (...) {
    }
    timer = visTimer = nullptr;
    endpoint = nullptr;
    pinnedHeader = nullptr;
    root = nullptr;
}

// ============================================================================
// Injection
// Main page layout (24H2/25H2, per the Notification Center Styler themes):
//   ... > Grid#L1Grid > ContentControl#SlidersGroup (brightness + volume)
// The mixer is added to L1Grid as a new Auto row right below SlidersGroup.
// ============================================================================

bool IsPanelAttached() { return g_panel && g_panel->root && VisualTreeHelper::GetParent(g_panel->root); }

void InjectBelow(wux::FrameworkElement const& anchor) {
    auto parent = VisualTreeHelper::GetParent(anchor);
    auto grid = parent.try_as<wuxc::Grid>();
    auto stack = parent.try_as<wuxc::StackPanel>();
    uint32_t index = 0;
    if (!grid && !(stack && stack.Orientation() == wuxc::Orientation::Vertical && stack.Children().IndexOf(anchor, index))) {
        Wh_Log(L"Unsupported anchor parent, not injecting");
        return;
    }

    auto panel = std::make_shared<MixerPanel>();
    panel->BindNative(anchor, FindNativeVolumeRow(anchor));
    panel->Create();
    if (auto b = FindBandBrush(anchor)) panel->root.Background(b);
    panel->root.Margin(anchor.Margin());
    g_panel = panel;

    if (!grid) {  // vertical StackPanel: insert after the anchor
        stack.Children().InsertAt(index + 1, panel->root);
        Wh_Log(L"Mixer injected (StackPanel)");
        return;
    }

    // Grid: insert an Auto row below the anchor, shifting the rows below it.
    // The background Border ending at the sliders stretches over the mixer too.
    auto rows = grid.RowDefinitions();
    if (rows.Size() == 0) {  // single implicit row: make it explicit first
        panel->addedFirstRow = wuxc::RowDefinition();
        panel->addedFirstRow.Height(kAuto);
        rows.Append(panel->addedFirstRow);
    }
    int at = std::min<int>(wuxc::Grid::GetRow(anchor) + wuxc::Grid::GetRowSpan(anchor), rows.Size());
    panel->mixerRow.Insert(
        grid, at, [&](auto&, int row) { return row >= at; },
        [&](auto& fe, int endRow) { return endRow == at && fe.template try_as<wuxc::Border>() && fe.Name().empty(); });
    wuxc::Grid::SetRow(panel->root, at);
    wuxc::Grid::SetColumn(panel->root, wuxc::Grid::GetColumn(anchor));
    wuxc::Grid::SetColumnSpan(panel->root, wuxc::Grid::GetColumnSpan(anchor));
    grid.Children().Append(panel->root);

    if (g_settings.hideFooter) panel->Hide(FindChildByName(grid, L"FooterGrid"));
    try {
        panel->ApplyTiles(grid);
    } catch (...) {
        Wh_Log(L"ApplyTiles failed: %08X", winrt::to_hresult());
    }
    Wh_Log(L"Mixer injected (Grid row %d)", at);
}

void AttachToTree(wux::FrameworkElement const& root) {
    if (!root || g_unloading || IsPanelAttached()) return;
    auto anchor = FindChildByName(root, g_settings.anchorName);
    if (!anchor) {
        Wh_Log(L"Anchor '%s' not found yet", g_settings.anchorName.c_str());
        return;
    }
    if (g_panel) {  // old panel detached (tree rebuilt): clean it up first
        g_panel->Destroy();
        g_panel = nullptr;
    }
    try {
        InjectBelow(anchor);
    } catch (...) {
        Wh_Log(L"InjectBelow failed: %08X", winrt::to_hresult());
    }
}

// ============================================================================
// ControlCenter.dll discovery
// Same technique as the "Quick Settings Media Progress Bar" mod: hook
// ControlCenterView methods, then walk the public XAML tree.
// ============================================================================

wux::FrameworkElement ElementFromAbi(void* object) {
    wf::IUnknown unk{nullptr};
    winrt::copy_from_abi(unk, object);
    return unk.try_as<wux::FrameworkElement>();
}

// Top of the XAML tree the element lives in.
wux::FrameworkElement TreeRoot(wux::FrameworkElement root) {
    if (auto xr = root.XamlRoot()) {
        auto content = xr.Content().try_as<wux::FrameworkElement>();
        return content ? content : root;
    }
    for (int depth = 0; depth < 64; ++depth) {
        auto parent = VisualTreeHelper::GetParent(root).try_as<wux::FrameworkElement>();
        if (!parent) break;
        root = parent;
    }
    return root;
}

void OnControlCenterDiscovered(wux::FrameworkElement const& element) noexcept {
    if (!element || g_unloading) return;
    try {
        if (!g_uiQueue) g_uiQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        auto root = TreeRoot(element);
        AttachToTree(root);
        // Sliders may be realized a moment later.
        if (g_uiQueue)
            g_uiQueue.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, [weak = winrt::make_weak(root)] {
                if (auto r = weak.get()) AttachToTree(r);
            });
    } catch (...) {
        Wh_Log(L"OnControlCenterDiscovered failed: %08X", winrt::to_hresult());
    }
}

// Fallback discovery: OnGotFocus of the view and of the full-screen page.
using Focus_t = int(WINAPI*)(void*, void*);
Focus_t g_origViewFocus = nullptr, g_origPageFocus = nullptr;

template <Focus_t& original>
int WINAPI Focus_Hook(void* self, void* args) {
    int res = original ? original(self, args) : 0;
    OnControlCenterDiscovered(ElementFromAbi(self));
    return res;
}

// Early discovery: IComponentConnector::Connect runs while the view is being
// built (before it is shown). After Loaded, every layout pass (LayoutUpdated
// runs before the frame is drawn) checks whether the tile grid and the native
// volume row are realized, and injects as soon as they are, so the native
// layout never flashes. This matters after Windows restarts ShellHost (idle):
// the view is then built on open.
using Connect_t = int(WINAPI*)(void*, int, void*);
Connect_t g_origViewConnect = nullptr;
void* g_lastConnectView = nullptr;  // identity only, never dereferenced
winrt::weak_ref<wux::FrameworkElement> g_pendingView;
winrt::event_token g_pendingLoadedToken{}, g_pendingLayoutToken{};

void RevokePendingView() {
    if (auto v = g_pendingView.get()) {
        v.Loaded(g_pendingLoadedToken);
        v.LayoutUpdated(g_pendingLayoutToken);
    }
    g_pendingView = nullptr;
}

void InjectWhenReady(wux::FrameworkElement const& view) {
    bool needed = !g_unloading && !IsPanelAttached();
    if (needed) {
        auto root = TreeRoot(view);
        auto anchor = FindChildByName(root, g_settings.anchorName);
        if (!anchor || !FindChildByName(anchor, L"VolumeL2Button") || !FindChildByName(root, L"RootGridView")) return;
    }
    RevokePendingView();
    if (needed) OnControlCenterDiscovered(view);
}

int WINAPI ViewConnect_Hook(void* self, int id, void* target) {
    int res = g_origViewConnect ? g_origViewConnect(self, id, target) : 0;
    if (g_unloading || !self) return res;
    try {
        auto view = ElementFromAbi(self);
        if (!view || winrt::get_abi(view) == g_lastConnectView) return res;
        g_lastConnectView = winrt::get_abi(view);
        if (!g_uiQueue) g_uiQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        RevokePendingView();
        g_pendingView = view;
        g_pendingLoadedToken = view.Loaded([](wf::IInspectable const& sender, auto&&) {
            auto fe = sender.try_as<wux::FrameworkElement>();
            if (!fe) return;
            fe.Loaded(std::exchange(g_pendingLoadedToken, {}));
            g_pendingLayoutToken = fe.LayoutUpdated([](auto&&...) {
                if (auto v = g_pendingView.get()) InjectWhenReady(v);
            });
            InjectWhenReady(fe);
        });
    } catch (...) {
        Wh_Log(L"Connect hook failed: %08X", winrt::to_hresult());
    }
    return res;
}

std::atomic<int> g_discoveryState{0};  // 0 waiting, 1 installing, 2 ready, 3 failed
std::mutex g_discoveryMutex;

bool HookControlCenter(HMODULE controlCenter, bool applyNow) {
    int expected = 0;
    if (!g_discoveryState.compare_exchange_strong(expected, 1)) return expected != 3;
    std::lock_guard lock(g_discoveryMutex);
    if (g_unloading) return false;

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &g_origViewFocus,
         Focus_Hook<g_origViewFocus>},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &g_origPageFocus,
         Focus_Hook<g_origPageFocus>,
         true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
         &g_origViewConnect,
         ViewConnect_Hook,
         true},
    };
    bool ok = WindhawkUtils::HookSymbols(controlCenter, hooks, ARRAYSIZE(hooks));
    if (ok && applyNow) ok = Wh_ApplyHookOperations();
    g_discoveryState = ok ? (applyNow ? 2 : 1) : 3;
    Wh_Log(L"ControlCenter.dll hooks: %s", ok ? L"ready" : L"failed");
    return ok;
}

// ControlCenter.dll not loaded yet at init: hook it as soon as it loads.
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && name && !g_unloading && g_discoveryState == 0 &&
        !(flags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE)) &&
        _wcsicmp(PathFindFileNameW(name), L"ControlCenter.dll") == 0) {
        Wh_Log(L"ControlCenter.dll loaded");
        HookControlCenter(module, true);
    }
    return module;
}


// ============================================================================
// Lifecycle
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    if (HMODULE cc = GetModuleHandleW(L"ControlCenter.dll")) {
        HookControlCenter(cc, false);
    } else {
        Wh_Log(L"Waiting for ControlCenter.dll...");
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        void* loadLibrary = kernelBase ? (void*)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
        if (loadLibrary) Wh_SetFunctionHook(loadLibrary, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_unloading) return;
    if (g_discoveryState == 1) {
        bool ok = Wh_ApplyHookOperations();
        g_discoveryState = ok ? 2 : 3;
        Wh_Log(L"ControlCenter.dll hooks applied: %s", ok ? L"ready" : L"failed");
    } else if (g_discoveryState == 0) {
        if (HMODULE cc = GetModuleHandleW(L"ControlCenter.dll")) HookControlCenter(cc, true);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (!g_uiQueue) return;
    g_uiQueue.TryEnqueue([] {
        g_appInfo.clear();  // "Hidden apps" may have changed
        if (!g_panel) return;
        g_panel->RefreshSessions(true);
        if (g_settings.brightness) {
            g_ddc.Start();
            g_ddc.Refresh();
        }
        g_panel->RefreshBrightness(true);
    });
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");
    g_unloading = true;
    { std::lock_guard lock(g_discoveryMutex); }  // let a running hook installation finish
    g_ddc.Stop();  // before the UI teardown: its last callbacks run first

    // Runs on the UI thread and waits. No timeout: returning early would unmap
    // code the lambda still runs.
    auto runOnUi = [](void (*work)()) {
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        bool queued = g_uiQueue.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, [done, work] {
            work();
            if (done) SetEvent(done);
        });
        if (queued && done) WaitForSingleObject(done, INFINITE);
        if (done) CloseHandle(done);
    };
    if (g_uiQueue) {
        runOnUi([] {
            try {
                RevokePendingView();
                if (g_panel) g_panel->Destroy();
            } catch (...) {
            }
            g_panel = nullptr;
            g_audioEvents.Stop();
            g_appInfo.clear();
            g_enumerator = nullptr;
        });
        runOnUi([] {});  // drain: refreshes queued by audio callbacks before they were unregistered
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_ddc.Stop();
    g_audioEvents.ReleaseMta();
}
