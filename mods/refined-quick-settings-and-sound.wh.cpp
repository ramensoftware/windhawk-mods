// ==WindhawkMod==
// @id refined-quick-settings-and-sound
// @name Refined Quick Settings and Sound
// @description A "vanilla+" rework of the Windows 11 Quick Settings and Volume mixer panels for daily use
// @version 0.1.154
// @author HeyOkay
// @github https://github.com/HeyOkay
// @include explorer.exe
// @include ShellHost.exe
// @architecture x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lole32 -loleaut32 -lruntimeobject -liphlpapi -lshlwapi -lshell32 -lpowrprof -luuid
// @license GPL-3.0-only
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Refined Quick Settings and Sound

![Quick Settings and Sound preview](https://raw.githubusercontent.com/HeyOkay/Refined-Quick-Settings-and-Sound/main/docs/assets/preview-lossless.webp)

This "vanilla+" Windhawk mod reworks the Windows 11 "Quick Settings" and
"Volume mixer" panels and makes them fit for daily use. It follows the
Windows 11 design language, refining existing features and adding new ones:
thin sliders, even row spacing at every display scale and a smooth frame
transition to the Sound page.

Everything runs inside the shell's own panels (ShellHost): no extra windows,
the system Acrylic, fonts, themes, lists, bindings and animations are kept, and
customizations are restored when the mod is unloaded.

## Features

**Quick Settings**

- Brightness control for DDC/CI-compatible displays, with animated glyphs.
- Press the brightness glyph to toggle night light on and off.
- Open display settings directly from the glyph on the right side of the slider.

**Sound**

- The "Sound output" page is now simply "Sound".
- Change the output and input devices and the spatial sound provider from
  dropdown lists.
- Below them, a reworked Volume mixer for the apps. It is collapsible and
  remembers its last state.
- Mouse wheel on every slider; press an app's icon to mute and restore it, or
  its volume number to set it to 100.
- Hide apps from the mixer by name (setting below).

**Taskbar**

- Click the volume icon to open Sound directly.
- The system's animated volume glyph and an Ethernet glyph for wired networks.

## Compatibility

- Windows 11 24H2 or later (Quick Settings hosted by ShellHost), x86-64
  (ARM64 is untested).
  Development testing: build 26300, primarily at 125% scale.
- Other Windows builds and display scales need validation. The mod depends on
  internal shell XAML elements and symbols which can change with Windows updates.
- DDC/CI brightness requires a compatible monitor with DDC/CI enabled.
- Added labels such as "Sound" and "Input device" are in English.
- The first Sound opening in each ShellHost process starts with the mixer
  collapsed; subsequent openings remember its state.

Control Center discovery uses COM-interface symbol hooks in ControlCenter.dll and the public XAML visual tree; it does not open a XAML Diagnostics connection. Compatibility with other mods that edit the same controls still requires live testing. If the panel was already open when enabling the mod, close and reopen it; restart ShellHost if it is not picked up.

## Notes

Disable other mods that change the Quick Settings, the Sound page or the
taskbar volume icon before enabling this one. Errors are written to the
Windhawk log.

## Credits and license

GPL-3.0-only. The UI-thread dispatch technique follows
[m417z's Notification Center Styler](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-notification-center-styler.wh.cpp)
(GPL-3.0). ControlCenter.dll discovery uses the COM produce-interface pattern
documented by [Per-monitor brightness](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/per-monitor-brightness.wh.cpp).
The hidden-applications parser is adapted from the local
Sound Output Direct and Taller mod (GPL-3.0-only).
The sliders-horizontal and ethernet-port icons are from
[Lucide](https://lucide.dev/) (ISC); the license notice is included below
in this source file.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- hiddenApps: ""
  $name: Hidden applications
  $description: Parts of application names, separated by semicolons, to hide from the volume mixer. Hiding does not mute them.
*/
// ==/WindhawkModSettings==




#ifndef WH_WINRT_WINUI2
#define WH_WINRT_WINUI2
#endif
#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <unknwn.h>
#include <mutex>
#include <set>
#include <map>
#include <memory>
#include <functional>
#include <atomic>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>

#include <mmdeviceapi.h>
#include <mmreg.h> // WAVEFORMATEX for the endpoint policy interface
#include <powrprof.h>
#include <powersetting.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <propsys.h>
#include <shlwapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <condition_variable>
#include <deque>
#include <thread>

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <string>
#include <vector>
namespace sound {
// Adapted from the hidden-applications parser of the Sound Output Direct and
// Taller Windhawk mod; the only code taken from it.
// Matching happens before a row is created, so hidden apps occupy no space.
inline std::wstring Lower(std::wstring value) {
  if (!value.empty())
    CharLowerBuffW(value.data(), static_cast<DWORD>(value.size()));
  return value;
}
inline std::vector<std::wstring> ParseHidden(std::wstring const &text) {
  std::vector<std::wstring> result;
  size_t start = 0;
  while (start < text.size()) {
    auto end = text.find_first_of(L";,\r\n", start);
    if (end == text.npos)
      end = text.size();
    auto part = text.substr(start, end - start);
    auto first = part.find_first_not_of(L" \t");
    if (first != part.npos)
      result.push_back(Lower(part.substr(first, part.find_last_not_of(L" \t") - first + 1)));
    start = end + 1;
  }
  return result;
}
inline bool Hidden(std::vector<std::wstring> const &patterns, std::wstring const &name,
                   std::wstring const &path) {
  auto text = Lower(name + L" " + path);
  return std::any_of(patterns.begin(), patterns.end(),
                     [&](auto const &p) { return text.find(p) != text.npos; });
}
inline float Clamp(float value) { return std::clamp(value, 0.f, 1.f); }
inline int VolumeIcon(float value, bool mute) {
  return mute || value <= 0 ? 0 : value <= .33f ? 1 : value <= .66f ? 2 : 3;
}
struct SavedVolume {
  float last = .5f;
  void Observe(float value, bool) {
    if (value > 0)
      last = Clamp(value);
  }
  float Toggle(float current, bool muted) {
    Observe(current, muted);
    return !muted && current > 0 ? 0 : last;
  }
};
struct Device {
  std::wstring id, name;
  bool current = false;
};
struct Snapshot {
  uint64_t masterNoticeAt = 0, masterRevision = 0;
  std::vector<Device> output, input;
  std::wstring outputId, inputId, error;
  float master = 0;
  bool muted = false, masterAvailable = false;
  unsigned long long revision = 0;
  unsigned long long applied = 0;
};
enum class Action { MasterVolume, MasterMute, Input, Refresh };
struct Command {
  Action action = Action::Refresh;
  std::wstring id;
  float value = 0;
  unsigned long long serial = 0;
};
} // namespace sound

namespace sound {
// Display-on and resume-from-sleep notifications (power callbacks on a system
// thread). DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS and DEVICE_NOTIFY_CALLBACK (2) are
// declared here: the mod's MinGW headers lack them.
class PowerWake {
  struct Subscribe {
    ULONG(CALLBACK *Callback)(PVOID, ULONG, PVOID);
    PVOID Context;
  };
  static constexpr DWORD notifyCallback = 2;
  static constexpr GUID displayState = {
      0x6fe69556, 0x704a, 0x47a0, {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};
  std::function<void()> on_;
  Subscribe params_{};
  HPOWERNOTIFY display_ = nullptr, resume_ = nullptr;
  static ULONG CALLBACK Changed(PVOID context, ULONG type, PVOID setting) {
    bool wake = type == PBT_APMRESUMEAUTOMATIC;
    if (type == PBT_POWERSETTINGCHANGE && setting) {
      auto s = static_cast<POWERBROADCAST_SETTING *>(setting);
      wake = IsEqualGUID(s->PowerSetting, displayState) && s->DataLength >= sizeof(DWORD) &&
             *reinterpret_cast<DWORD const *>(s->Data) == 1;
    }
    if (wake && context)
      try {
        static_cast<PowerWake *>(context)->on_();
      } catch (...) {
      }
    return 0;
  }

public:
  explicit PowerWake(std::function<void()> on) : on_(std::move(on)) {
    params_.Callback = Changed;
    params_.Context = this;
    if (PowerSettingRegisterNotification(&displayState, notifyCallback, &params_, &display_) !=
        ERROR_SUCCESS)
      display_ = nullptr;
    if (PowerRegisterSuspendResumeNotification(notifyCallback, &params_, &resume_) !=
        ERROR_SUCCESS)
      resume_ = nullptr;
  }
  PowerWake(PowerWake const &) = delete;
  PowerWake &operator=(PowerWake const &) = delete;
  ~PowerWake() {
    if (display_)
      PowerSettingUnregisterNotification(display_);
    if (resume_)
      PowerUnregisterSuspendResumeNotification(resume_);
  }
};
// All COM audio objects belong to one MTA thread. The UI exchanges plain data.
class Audio final {
  struct VolumeFeed {
    std::mutex mutex;
    std::wstring id;
    float value = 0;
    bool muted = false, available = false, active = true;
    uint64_t revision = 0, noticeAt = 0;
    std::function<void()> publish;
  };
  struct VolumeNotify : winrt::implements<VolumeNotify, IAudioEndpointVolumeCallback> {
    std::shared_ptr<VolumeFeed> feed;
    std::wstring id;
    VolumeNotify(std::shared_ptr<VolumeFeed> value, std::wstring endpoint)
        : feed(std::move(value)), id(std::move(endpoint)) {}
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA data) noexcept override {
      if (!data)
        return E_POINTER;
      try {
        std::function<void()> publish;
        {
          std::lock_guard lock(feed->mutex);
          if (!feed->active || feed->id != id)
            return S_OK;
          feed->value = data->fMasterVolume;
          feed->muted = data->bMuted;
          feed->available = true;
          feed->revision++;
          feed->noticeAt = GetTickCount64();
          publish = feed->publish;
        }
        if (publish)
          publish();
        return S_OK;
      } catch (...) {
        return winrt::to_hresult();
      }
    }
  };
  struct Wake {
    std::mutex mutex;
    std::condition_variable cv;
    bool changed = true;
  };
  struct Notify : winrt::implements<Notify, IMMNotificationClient> {
    std::shared_ptr<Wake> wake;
    explicit Notify(std::shared_ptr<Wake> value) : wake(std::move(value)) {}
    void Signal() {
      std::lock_guard lock(wake->mutex);
      wake->changed = true;
      wake->cv.notify_one();
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override {
      Signal();
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
      Signal();
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
      Signal();
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
      Signal();
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
      Signal();
      return S_OK;
    }
  };
  // Undocumented endpoint policy ABI, limited to the known interface and checked HRESULTs.
  struct Policy : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(LPCWSTR, WAVEFORMATEX **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(LPCWSTR, BOOL, WAVEFORMATEX **) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(LPCWSTR, WAVEFORMATEX *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(LPCWSTR, BOOL, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(LPCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(LPCWSTR, void *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(LPCWSTR, void *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(LPCWSTR, const PROPERTYKEY *,
                                                       PROPVARIANT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(LPCWSTR, const PROPERTYKEY *,
                                                       PROPVARIANT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(LPCWSTR, ERole) = 0;
  };
  std::shared_ptr<Wake> wake_ = std::make_shared<Wake>();
  std::thread worker_;
  std::atomic<bool> stop_{false};
  std::mutex mutex_;
  std::deque<Command> commands_;
  Snapshot snapshot_;
  std::function<void()> publish_;
  winrt::com_ptr<IMMDeviceEnumerator> enumerator_;
  winrt::com_ptr<IAudioEndpointVolume> master_;
  std::shared_ptr<VolumeFeed> volumeFeed_ = std::make_shared<VolumeFeed>();
  winrt::com_ptr<VolumeNotify> volumeObserver_;
  std::map<std::wstring, SavedVolume> masters_;
  std::wstring masterId_, error_;
  unsigned long long revision_ = 0, applied_ = 0;
  std::atomic<unsigned long long> next_{0};
  static std::wstring Id(IMMDevice *device) {
    LPWSTR raw = nullptr;
    if (!device || FAILED(device->GetId(&raw)))
      return {};
    std::wstring result(raw);
    CoTaskMemFree(raw);
    return result;
  }
  static std::wstring Friendly(IMMDevice *device) {
    winrt::com_ptr<IPropertyStore> props;
    PROPVARIANT value{};
    std::wstring name = L"Audio device";
    constexpr PROPERTYKEY friendlyKey = {
        {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};
    if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, props.put())) &&
        SUCCEEDED(props->GetValue(friendlyKey, &value))) {
      if (value.vt == VT_LPWSTR)
        name = value.pwszVal;
      PropVariantClear(&value);
    }
    return name;
  }
  std::vector<Device> Devices(EDataFlow flow, std::wstring &defaultId) {
    winrt::com_ptr<IMMDevice> current;
    enumerator_->GetDefaultAudioEndpoint(flow, eMultimedia, current.put());
    defaultId = Id(current.get());
    std::vector<Device> result;
    winrt::com_ptr<IMMDeviceCollection> devices;
    winrt::check_hresult(enumerator_->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, devices.put()));
    UINT count = 0;
    winrt::check_hresult(devices->GetCount(&count));
    for (UINT i = 0; i < count; i++) {
      winrt::com_ptr<IMMDevice> device;
      if (FAILED(devices->Item(i, device.put())))
        continue;
      auto id = Id(device.get());
      result.push_back({id, Friendly(device.get()), id == defaultId});
    }
    return result;
  }
  void BindMaster(std::wstring const &id) {
    if (id == masterId_ && master_)
      return;
    if (master_ && volumeObserver_)
      master_->UnregisterControlChangeNotify(volumeObserver_.get());
    volumeObserver_ = nullptr;
    master_ = nullptr;
    masterId_ = id;
    {
      std::lock_guard lock(volumeFeed_->mutex);
      volumeFeed_->id = id;
      volumeFeed_->available = false;
      volumeFeed_->revision++;
    }
    if (id.empty())
      return;
    winrt::com_ptr<IMMDevice> device;
    if (SUCCEEDED(enumerator_->GetDevice(id.c_str(), device.put())))
      device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                       master_.put_void());
    if (master_) {
      volumeObserver_ = winrt::make_self<VolumeNotify>(volumeFeed_, id);
      if (FAILED(master_->RegisterControlChangeNotify(volumeObserver_.get())))
        volumeObserver_ = nullptr;
      uint64_t revision;
      {
        std::lock_guard lock(volumeFeed_->mutex);
        revision = volumeFeed_->revision;
      }
      float value = 0;
      BOOL muted = FALSE;
      if (SUCCEEDED(master_->GetMasterVolumeLevelScalar(&value)) &&
          SUCCEEDED(master_->GetMute(&muted))) {
        std::lock_guard lock(volumeFeed_->mutex);
        if (volumeFeed_->revision == revision) {
          volumeFeed_->value = value;
          volumeFeed_->muted = muted;
          volumeFeed_->available = bool(volumeObserver_);
        }
      }
    }
  }
  void SetDefault(std::wstring const &id) {
    const CLSID clsid = {
        0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
    const IID iid = {0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
    winrt::com_ptr<Policy> policy;
    winrt::check_hresult(
        CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, iid, policy.put_void()));
    winrt::com_ptr<IMMDevice> selected;
    winrt::check_hresult(enumerator_->GetDevice(id.c_str(), selected.put()));
    EDataFlow flow;
    winrt::check_hresult(selected.as<IMMEndpoint>()->GetDataFlow(&flow));
    std::vector<std::pair<ERole, std::wstring>> before;
    for (auto role : {eConsole, eMultimedia, eCommunications}) {
      winrt::com_ptr<IMMDevice> device;
      enumerator_->GetDefaultAudioEndpoint(flow, role, device.put());
      before.emplace_back(role, Id(device.get()));
    }
    size_t completed = 0;
    for (auto const &[role, previous] : before) {
      auto result = policy->SetDefaultEndpoint(id.c_str(), role);
      if (FAILED(result)) {
        for (size_t i = 0; i < completed; i++)
          if (!before[i].second.empty())
            policy->SetDefaultEndpoint(before[i].second.c_str(), before[i].first);
        winrt::check_hresult(result);
      }
      ++completed;
    }
  }
  void Execute(Command const &cmd) {
    error_.clear();
    auto value = Clamp(cmd.value);
    if (cmd.action == Action::MasterVolume || cmd.action == Action::MasterMute) {
      if (!master_)
        throw winrt::hresult_error(E_NOTFOUND, L"Output device unavailable");
      float before = 0;
      BOOL muted = FALSE;
      winrt::check_hresult(master_->GetMasterVolumeLevelScalar(&before));
      winrt::check_hresult(master_->GetMute(&muted));
      auto &saved = masters_[masterId_];
      saved.Observe(before, muted);
      if (cmd.action == Action::MasterMute)
        value = saved.Toggle(before, muted);
      winrt::check_hresult(master_->SetMasterVolumeLevelScalar(value, nullptr));
      winrt::check_hresult(master_->SetMute(value <= 0, nullptr));
    } else if (cmd.action == Action::Input) {
      winrt::com_ptr<IMMDevice> target;
      winrt::check_hresult(enumerator_->GetDevice(cmd.id.c_str(), target.put()));
      DWORD status = 0;
      winrt::check_hresult(target->GetState(&status));
      if (!(status & DEVICE_STATE_ACTIVE))
        throw winrt::hresult_error(E_NOTFOUND, L"Device disconnected");
      auto endpoint = target.as<IMMEndpoint>();
      EDataFlow flow;
      winrt::check_hresult(endpoint->GetDataFlow(&flow));
      if (flow != eCapture)
        throw winrt::hresult_error(E_INVALIDARG);
      SetDefault(cmd.id);
    }
  }
  void Read() {
    Snapshot state;
    state.revision = ++revision_;
    state.applied = applied_;
    state.error = error_;
    state.output = Devices(eRender, state.outputId);
    state.input = Devices(eCapture, state.inputId);
    BindMaster(state.outputId);
    if (master_) {
      BOOL mute = FALSE;
      state.masterAvailable = SUCCEEDED(master_->GetMasterVolumeLevelScalar(&state.master)) &&
                              SUCCEEDED(master_->GetMute(&mute));
      state.muted = mute;
      masters_[masterId_].Observe(state.master, state.muted);
      // An invalidated endpoint keeps failing: it is opened again on the next
      // read (the next notification).
      if (!state.masterAvailable) {
        if (volumeObserver_)
          master_->UnregisterControlChangeNotify(volumeObserver_.get());
        volumeObserver_ = nullptr;
        master_ = nullptr;
      }
    }
    // The shell's own lists serve output devices, application sessions and
    // spatial sound; this worker serves the added input selector, the master
    // volume and the tray.
    {
      std::lock_guard lock(mutex_);
      snapshot_ = std::move(state);
    }
    if (publish_)
      publish_();
  }
  void Run() noexcept {
    bool apartment = false;
    try {
      winrt::init_apartment(winrt::apartment_type::multi_threaded);
      apartment = true;
      winrt::check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                            CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
                                            enumerator_.put_void()));
      auto observer = winrt::make_self<Notify>(wake_);
      winrt::check_hresult(enumerator_->RegisterEndpointNotificationCallback(observer.get()));
      while (!stop_) {
        std::deque<Command> pending;
        {
          std::lock_guard lock(mutex_);
          pending.swap(commands_);
        }
        for (auto const &cmd : pending) {
          try {
            Execute(cmd);
          } catch (winrt::hresult_error const &ex) {
            WCHAR code[32];
            swprintf_s(code, L" (0x%08X)", unsigned(ex.code().value));
            error_ = std::wstring(ex.message()) + code;
          }
          applied_ = cmd.serial;
        }
        try {
          Read();
        } catch (winrt::hresult_error const &ex) {
          {
            std::lock_guard lock(mutex_);
            snapshot_.error =
                std::wstring(L"Cannot refresh audio devices: ") + ex.message().c_str();
            snapshot_.applied = applied_;
          }
          if (publish_)
            publish_();
        }
        std::unique_lock lock(wake_->mutex);
        // No polling: endpoint, default-device and master volume changes arrive
        // as notifications (Notify, VolumeNotify), commands through Send.
        wake_->cv.wait(lock, [&] { return stop_ || wake_->changed; });
        wake_->changed = false;
      }
      enumerator_->UnregisterEndpointNotificationCallback(observer.get());
      observer = nullptr;
    } catch (winrt::hresult_error const &ex) {
      {
        std::lock_guard lock(mutex_);
        snapshot_.error = std::wstring(L"Core Audio: ") + ex.message().c_str();
      }
      if (publish_)
        publish_();
    } catch (...) {
      {
        std::lock_guard lock(mutex_);
        snapshot_.error = L"Core Audio worker stopped unexpectedly";
      }
      if (publish_)
        publish_();
    }
    if (master_ && volumeObserver_)
      master_->UnregisterControlChangeNotify(volumeObserver_.get());
    volumeObserver_ = nullptr;
    master_ = nullptr;
    enumerator_ = nullptr;
    if (apartment)
      winrt::uninit_apartment();
  }

public:
  Audio() = default;
  Audio(Audio const &) = delete;
  Audio &operator=(Audio const &) = delete;
  ~Audio() { Stop(); }
  void Start(std::function<void()> publish) {
    publish_ = std::move(publish);
    {
      std::lock_guard lock(volumeFeed_->mutex);
      volumeFeed_->publish = publish_;
      volumeFeed_->active = true;
    }
    stop_ = false;
    worker_ = std::thread([this] { Run(); });
  }
  void Stop() {
    {
      std::lock_guard lock(volumeFeed_->mutex);
      volumeFeed_->active = false;
      volumeFeed_->publish = {};
    }
    {
      // Under the wake mutex, so the worker cannot miss it between its check
      // and its wait.
      std::lock_guard lock(wake_->mutex);
      stop_ = true;
    }
    wake_->cv.notify_one();
    if (worker_.joinable())
      worker_.join();
    publish_ = {};
  }
  unsigned long long Send(Command command) {
    command.serial = ++next_;
    auto serial = command.serial;
    {
      std::lock_guard lock(mutex_);
      commands_.push_back(std::move(command));
    }
    {
      std::lock_guard lock(wake_->mutex);
      wake_->changed = true;
    }
    wake_->cv.notify_one();
    return serial;
  }
  Snapshot Get() {
    Snapshot state;
    {
      std::lock_guard lock(mutex_);
      state = snapshot_;
    }
    {
      std::lock_guard lock(volumeFeed_->mutex);
      if (volumeFeed_->available && volumeFeed_->id == state.outputId) {
        state.master = volumeFeed_->value;
        state.muted = volumeFeed_->muted;
        state.masterAvailable = true;
        state.masterRevision = volumeFeed_->revision;
        state.masterNoticeAt = volumeFeed_->noticeAt;
      }
    }
    return state;
  }
};
} // namespace sound


// Lucide ISC assets: the mixer settings glyph and ethernet-port for the
// taskbar network icon; all other glyphs are system Segoe Fluent Icons via
// SymbolGlyph().
namespace sound {
inline PCWSTR IconMarkup(std::wstring_view name) {
  if (name == L"sliders-horizontal")
    return LR"icon(<Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M10 5H3"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M12 19H3"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M14 3v4"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M16 17v4"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M21 12h-9"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M21 19h-5"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M21 5h-7"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M8 10v4"/><Path Stroke="{ThemeResource SystemControlForegroundBaseHighBrush}" StrokeThickness="2" StrokeStartLineCap="Round" StrokeEndLineCap="Round" StrokeLineJoin="Round" Data="M8 12H3"/>)icon";
  return L"";
}
// Lucide ethernet-port (ISC), 24x24 grid, for the taskbar network icon.
// Polylines generated from the Lucide path; the "cut" variants leave a gap in
// the outline where the status badge sits (XAML has no masks), the badges are
// filled with EvenOdd holes. Stroke 1.5 on the 24 grid = 1 DIP at 16 DIP.
inline constexpr PCWSTR ethernetPort =
    L"M10,8 L10,9 M14,8 L14,9 M18,8 L18,9 M6,8 L6,9 M19,17 L18.7,17.02 18.31,17.12 17.95,17.29 "
    L"17.71,17.47 17.5,17.68 17.32,17.92 16.75,18.98 16.52,19.3 16.31,19.52 16.06,19.69 "
    L"15.89,19.79 15.61,19.9 15.32,19.97 15.02,20 8.92,20 8.53,19.94 8.24,19.85 7.97,19.72 "
    L"7.65,19.48 7.51,19.34 7.33,19.1 6.76,18.04 6.59,17.79 6.32,17.5 6,17.27 5.63,17.1 5.24,17.01 "
    L"5.04,17 3.94,17 3.65,16.97 3.45,16.92 3.17,16.82 2.82,16.62 2.53,16.35 2.4,16.2 2.24,15.95 "
    L"2.12,15.67 2.06,15.48 2,15.08 2,5.88 2.04,5.59 2.13,5.3 2.21,5.11 2.36,4.86 2.55,4.62 "
    L"2.69,4.49 2.85,4.36 3.11,4.21 3.39,4.1 3.58,4.04 3.98,4 20.08,4 20.38,4.04 20.67,4.11 "
    L"20.94,4.24 21.19,4.4 21.49,4.67 21.72,4.99 21.82,5.17 21.92,5.45 21.99,5.84 22,14.94 "
    L"21.97,15.34 21.9,15.63 21.83,15.82 21.63,16.16 21.36,16.46 21.13,16.65 20.96,16.75 "
    L"20.59,16.91 20.2,16.99 19,17 Z";
inline constexpr PCWSTR ethernetPortCutX =
    L"M10,8 L10,9 M14,8 L14,9 M18,8 L18,9 M6,8 L6,9 M11.32,20 L8.92,20 8.53,19.94 8.15,19.81 "
    L"7.97,19.72 7.73,19.54 7.45,19.26 7.28,19.01 6.65,17.87 6.4,17.57 6.08,17.32 5.73,17.14 "
    L"5.34,17.03 5.04,17 3.84,16.99 3.55,16.95 3.36,16.89 2.99,16.73 2.74,16.56 2.46,16.28 "
    L"2.24,15.95 2.12,15.67 2.06,15.48 2,15.08 2,5.88 2.04,5.59 2.09,5.39 2.25,5.03 2.42,4.78 "
    L"2.55,4.62 2.85,4.36 3.11,4.21 3.39,4.1 3.68,4.03 3.98,4 19.98,4 20.38,4.04 20.76,4.15 "
    L"20.94,4.24 21.19,4.4 21.35,4.52 21.55,4.74 21.72,4.99 21.82,5.17 21.92,5.45 21.98,5.74 "
    L"22,6.04 22,12.14";
inline constexpr PCWSTR ethernetPortCutWarn =
    L"M10,8 L10,9 M14,8 L14,9 M18,8 L18,9 M6,8 L6,9 M11.82,20 L8.92,20 8.53,19.94 8.24,19.85 "
    L"7.97,19.72 7.81,19.6 7.58,19.41 7.45,19.26 7.28,19.01 6.65,17.87 6.4,17.57 6.08,17.32 "
    L"5.73,17.14 5.34,17.03 5.04,17 3.84,16.99 3.55,16.95 3.36,16.89 2.99,16.73 2.74,16.56 "
    L"2.46,16.28 2.24,15.95 2.12,15.67 2.06,15.48 2,15.08 2,5.88 2.04,5.59 2.09,5.39 2.16,5.2 "
    L"2.3,4.94 2.42,4.78 2.62,4.55 2.85,4.36 3.02,4.26 3.2,4.17 3.48,4.07 3.78,4.01 4.08,4 19.98,4 "
    L"20.28,4.02 20.48,4.06 20.76,4.15 20.94,4.24 21.19,4.4 21.35,4.52 21.55,4.74 21.67,4.9 "
    L"21.82,5.17 21.89,5.35 21.97,5.64 21.99,5.84 22,14.34";
inline constexpr PCWSTR ethernetBadgeX =
    L"F0 M13.5,18.5 A5,5 0 1 1 23.5,18.5 A5,5 0 1 1 13.5,18.5 Z M19.42,16.45 L20.55,17.58 "
    L"L19.63,18.5 L20.55,19.42 L19.42,20.55 L18.5,19.63 L17.58,20.55 L16.45,19.42 L17.37,18.5 "
    L"L16.45,17.58 L17.58,16.45 L18.5,17.37 Z";
inline constexpr PCWSTR ethernetBadgeWarn =
    L"F0 M18.5,12.7 L24.1,22.8 L12.9,22.8 Z M17.8,16.1 L19.2,16.1 L19.05,19.6 L17.95,19.6 Z "
    L"M17.65,21.25 A0.85,0.85 0 1 1 19.35,21.25 A0.85,0.85 0 1 1 17.65,21.25 Z";
} // namespace sound

namespace native {
namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace M = X::Media;
namespace D = X::Data;
using winrt::Windows::Foundation::IInspectable;
inline std::atomic<bool> stopping = false;
inline HMODULE module = nullptr;
inline std::vector<std::wstring> hidden;
inline UINT volumePulseMessage = RegisterWindowMessageW(L"Windhawk.NativeSound.VolumePulse.1");
// Used only while unloading. Wait for the asynchronous operation itself,
// not an event set inside its delegate: the DLL must outlive the delegate's return.
inline bool DispatchCleanup(winrt::Windows::UI::Core::CoreDispatcher const &dispatcher,
                            std::function<void()> action) {
  if (!dispatcher)
    return false;
  try {
    if (dispatcher.HasThreadAccess()) {
      action();
      return true;
    }
    auto cleaned = std::make_shared<std::atomic<bool>>(false);
    auto operation = dispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
        [action = std::move(action), cleaned] {
          try {
            action();
            *cleaned = true;
          } catch (...) {
            Wh_Log(L"UI shutdown cleanup: %08X", winrt::to_hresult());
          }
        });
    while (operation.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
      Sleep(1);
    return operation.Status() == winrt::Windows::Foundation::AsyncStatus::Completed &&
           cleaned->load();
  } catch (...) {
    Wh_Log(L"UI shutdown dispatcher: %08X", winrt::to_hresult());
    return false;
  }
}
// Post only an integer message, never a callback or object pointer. Removing
// the subclass on unload makes any remaining queued notifications harmless.
struct VolumePulse {
  HWND window = nullptr;
  std::atomic<bool> active{true}, queued{false};
  explicit VolumePulse(HWND value) : window(value) {}
  void Post() {
    if (active && !queued.exchange(true))
      if (!window || !PostMessageW(window, volumePulseMessage, 0, 0))
        queued = false;
  }
};
inline X::FrameworkElement Icon(PCWSTR name, double size = 17, double stroke = 1.6) {
  std::wstring geometry = sound::IconMarkup(name);
  for (size_t at = 0; (at = geometry.find(L"StrokeThickness=\"2\"", at)) != geometry.npos;) {
    auto value = L"StrokeThickness=\"" + std::to_wstring(stroke) + L"\"";
    geometry.replace(at, 19, value);
    at += value.size();
  }
  auto icon =
      X::Markup::XamlReader::Load(
          L"<Viewbox xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='" +
          std::to_wstring(size) + L"' Height='" + std::to_wstring(size) +
          L"' HorizontalAlignment='Center' VerticalAlignment='Center' "
          L"UseLayoutRounding='True'><Canvas Width='24' Height='24'>" +
          geometry + L"</Canvas></Viewbox>")
          .as<X::FrameworkElement>();
  return icon;
}
template <class F> X::FrameworkElement Find(X::DependencyObject const &root, F const &match) {
  if (!root)
    return nullptr;
  if (auto e = root.try_as<X::FrameworkElement>(); e && match(e))
    return e;
  for (int i = 0; i < M::VisualTreeHelper::GetChildrenCount(root); i++)
    if (auto e = Find(M::VisualTreeHelper::GetChild(root, i), match))
      return e;
  return nullptr;
}
inline X::FrameworkElement Named(X::DependencyObject const &root, PCWSTR name) {
  return Find(root, [&](auto const &e) { return e.Name() == name; });
}
inline X::FrameworkElement Parent(X::DependencyObject const &e) {
  return M::VisualTreeHelper::GetParent(e).try_as<X::FrameworkElement>();
}
inline std::wstring Class(IInspectable const &e) {
  return e ? std::wstring(winrt::get_class_name(e)) : L"";
}
inline IInspectable VolumeModel(C::Slider const &slider) {
  if (auto expression = slider.GetBindingExpression(P::RangeBase::ValueProperty())) {
    if (auto data = expression.DataItem())
      return data;
    if (auto source = expression.ParentBinding().Source())
      return source;
  }
  return slider.DataContext();
}
inline IInspectable Resource(X::FrameworkElement root, PCWSTR name) {
  auto key = winrt::box_value(name);
  for (auto e = root; e; e = Parent(e))
    if (e.Resources().HasKey(key))
      return e.Resources().Lookup(key);
  auto app = X::Application::Current();
  if (app && app.Resources().HasKey(key))
    return app.Resources().Lookup(key);
  return nullptr;
}
// Use the shell's symbol font, with the same volume glyphs it uses in the tray.
// No SVG substitute or suppression of the native taskbar glyph is needed.
inline C::FontIcon SymbolGlyph(X::FrameworkElement const &owner, wchar_t code, double size = 16) {
  C::FontIcon icon;
  if (auto font = Resource(owner, L"SymbolThemeFontFamily").try_as<M::FontFamily>())
    icon.FontFamily(font);
  icon.Glyph(winrt::hstring(std::wstring(1, code)));
  icon.FontSize(size);
  icon.Width(size);
  icon.Height(size);
  icon.HorizontalAlignment(X::HorizontalAlignment::Center);
  icon.VerticalAlignment(X::VerticalAlignment::Center);
  icon.IsHitTestVisible(false);
  return icon;
}
inline C::FontIcon VolumeGlyph(X::FrameworkElement const &owner, int level, double size = 16) {
  return SymbolGlyph(owner,
                     level == 0   ? 0xe74f
                     : level == 1 ? 0xe993
                     : level == 2 ? 0xe994
                                  : 0xe995,
                     size);
}
// Built in code: XamlReader::Load ran several times per Sound opening (pickers,
// input rows, master row), and parsed XAML stays registered with XAML Diagnostics.
inline C::Grid Grid(PCWSTR columns) {
  C::Grid grid;
  std::wstring s(columns);
  for (size_t a = 0; a < s.size();) {
    auto b = s.find(',', a);
    if (b == s.npos)
      b = s.size();
    auto part = s.substr(a, b - a);
    C::ColumnDefinition column;
    if (part == L"*")
      column.Width(X::GridLengthHelper::FromValueAndType(1, X::GridUnitType::Star));
    else if (part == L"Auto")
      column.Width(X::GridLengthHelper::Auto());
    else
      column.Width(X::GridLengthHelper::FromValueAndType(std::stod(part), X::GridUnitType::Pixel));
    grid.ColumnDefinitions().Append(column);
    a = b + 1;
  }
  return grid;
}
inline C::Button Button(X::FrameworkElement const &owner, PCWSTR name, PCWSTR glyph = nullptr) {
  C::Button b;
  b.Width(32);
  b.Height(32);
  b.MinWidth(0);
  b.MinHeight(0);
  b.Padding({0, 0, 0, 0});
  if (auto style = Resource(owner, L"IconButtonStyle").try_as<X::Style>())
    b.Style(style);
  X::Automation::AutomationProperties::SetName(b, name);
  C::ToolTipService::SetToolTip(b, winrt::box_value(name));
  if (glyph)
    b.Content(Icon(glyph));
  return b;
}
// Block divider colour: the Quick Settings tiles Border, whose bottom edge
// divides the tiles from the All settings block. Set by QuickPage, read by the
// Sound page; a binding follows theme changes.
inline winrt::weak_ref<C::Border> quickDivider;
inline C::Border QuickDivider() {
  auto b = quickDivider.get();
  if (!b)
    return nullptr;
  auto t = b.BorderThickness();
  return t.Bottom > 0 || t.Top > 0 ? b : nullptr;
}
inline D::Binding DividerBinding(C::Border const &source) {
  D::Binding binding;
  binding.Source(source);
  binding.Path(X::PropertyPath(L"BorderBrush"));
  binding.Mode(D::BindingMode::OneWay);
  return binding;
}
// Dark theme: every divider and slider rail looks like the line above All
// settings. An opaque grey would look wrong over a tinted wallpaper.
// Shown and not cloaked by DWM (the shell hides its panel windows by cloaking).
inline bool WindowShown(HWND window) {
  if (!window || !IsWindowVisible(window))
    return false;
  using Dwm = HRESULT(WINAPI *)(HWND, DWORD, void *, DWORD);
  static auto get = [] {
    auto dll = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    return dll ? reinterpret_cast<Dwm>(GetProcAddress(dll, "DwmGetWindowAttribute")) : nullptr;
  }();
  DWORD cloaked = 0;
  return !(get && SUCCEEDED(get(window, 14 /*DWMWA_CLOAKED*/, &cloaked, sizeof(cloaked))) &&
           cloaked);
}
inline bool DarkTheme(X::FrameworkElement const &e) {
  return e && e.ActualTheme() == X::ElementTheme::Dark;
}
// That line is the system DividerStrokeColorDefault (#15FFFFFF, white at 8.3%)
// over the tiles layer (#09FFFFFF).
inline M::Brush DarkDivider() {
  return M::SolidColorBrush(winrt::Windows::UI::Color{0x15, 0xFF, 0xFF, 0xFF});
}
// Lines drawn directly on the base acrylic (Quick Settings dark block, Sound
// footer: SV divider and rails) have no layer under them, so they use the
// composition of both: 1-(1-0.035)(1-0.082) = 0.114 (#1DFFFFFF).
inline M::Brush DarkBaseDivider() {
  return M::SolidColorBrush(winrt::Windows::UI::Color{0x1D, 0xFF, 0xFF, 0xFF});
}
// Light theme: the Quick Settings Border's stroke, or the system divider
// stroke before that Border is known.
inline void PaintDivider(X::Shapes::Shape const &shape, X::FrameworkElement const &owner,
                         bool onBase = true) {
  if (DarkTheme(owner)) {
    shape.Fill(onBase ? DarkBaseDivider() : DarkDivider());
    return;
  }
  if (auto source = QuickDivider()) {
    shape.SetBinding(X::Shapes::Shape::FillProperty(), DividerBinding(source));
    return;
  }
  if (auto brush = Resource(owner, L"DividerStrokeColorDefaultBrush").try_as<M::Brush>())
    shape.Fill(brush);
}
inline void PaintDivider(C::Border const &border, X::FrameworkElement const &owner) {
  if (DarkTheme(owner)) {
    border.BorderBrush(DarkDivider());
    return;
  }
  if (auto source = QuickDivider()) {
    border.SetBinding(C::Border::BorderBrushProperty(), DividerBinding(source));
    return;
  }
  if (auto brush = Resource(owner, L"DividerStrokeColorDefaultBrush").try_as<M::Brush>())
    border.BorderBrush(brush);
}
inline void PaintDivider(C::StackPanel const &panel, X::FrameworkElement const &owner) {
  if (DarkTheme(owner)) {
    panel.BorderBrush(DarkDivider());
    return;
  }
  if (auto brush = Resource(owner, L"DividerStrokeColorDefaultBrush").try_as<M::Brush>())
    panel.BorderBrush(brush);
}
// The 38 DIP row step in whole pixels for the current scale (100%: 38,
// 125%: 48, 150%: 57). Fixed DIP values tuned for one scale land on different
// pixel steps at another.
inline double StepPx(double scale) { return std::ceil(38 * scale - 1e-6); }
// Restores the binding, not just the value it happened to produce.
struct Journal {
  std::vector<std::function<void()>> undo;
  std::set<std::pair<void *, void *>> saved;
  void Save(X::DependencyObject const &object, X::DependencyProperty const &property) {
    auto key = std::make_pair(winrt::get_abi(object), winrt::get_abi(property));
    if (!saved.insert(key).second)
      return;
    auto value = object.ReadLocalValue(property);
    D::BindingBase binding{nullptr};
    if (auto e = object.try_as<X::FrameworkElement>())
      if (auto expression = e.GetBindingExpression(property))
        binding = expression.ParentBinding();
    undo.push_back([object, property, value, binding] {
      if (binding)
        D::BindingOperations::SetBinding(object, property, binding);
      else if (value == X::DependencyProperty::UnsetValue())
        object.ClearValue(property);
      else
        object.SetValue(property, value);
    });
  }
  void Restore() {
    for (auto it = undo.rbegin(); it != undo.rend(); ++it)
      try {
        (*it)();
      } catch (...) {
        Wh_Log(L"Native restoration: 0x%08X", winrt::to_hresult());
      }
    undo.clear();
    saved.clear();
  }
  ~Journal() { Restore(); }
};
// Members are looked up once per (type, name): the shell's metadata provider was
// asked for the type and member on every read, hundreds of times per opening.
[[clang::no_destroy]] inline std::map<std::wstring, X::Markup::IXamlMember> modelMembers;
inline X::Markup::IXamlMember ModelMember(IInspectable const &model, PCWSTR name) {
  try {
    if (!model)
      return nullptr;
    auto key = Class(model) + L"|" + name;
    if (auto it = modelMembers.find(key); it != modelMembers.end())
      return it->second;
    X::Markup::IXamlMember member{nullptr};
    if (auto provider = X::Application::Current().try_as<X::Markup::IXamlMetadataProvider>())
      if (auto type = provider.GetXamlType(winrt::hstring(Class(model))))
        member = type.GetMember(name);
    modelMembers[key] = member;
    return member;
  } catch (...) {
  }
  return nullptr;
}
inline IInspectable ModelValue(IInspectable const &model, PCWSTR name) {
  if (!model)
    return nullptr;
  try {
    if (auto p = model.try_as<D::ICustomPropertyProvider>())
      if (auto v = p.GetCustomProperty(name); v && v.CanRead())
        return v.GetValue(model);
    if (auto member = ModelMember(model, name))
      return member.GetValue(model);
  } catch (...) {
  }
  return nullptr;
}
inline IInspectable NumberLike(IInspectable const &current, double number) {
  auto value = current.try_as<winrt::Windows::Foundation::IPropertyValue>();
  if (!value)
    return nullptr;
  using T = winrt::Windows::Foundation::PropertyType;
  switch (value.Type()) {
  case T::Int32:
    return winrt::box_value(int32_t(std::round(number)));
  case T::UInt32:
    return winrt::box_value(uint32_t(std::round(number)));
  case T::Single:
    return winrt::box_value(float(number));
  case T::Double:
    return winrt::box_value(number);
  default:
    return nullptr;
  }
}
inline bool SetNumber(IInspectable const &model, PCWSTR name, double number) {
  auto next = NumberLike(ModelValue(model, name), number);
  if (!next)
    return false;
  if (auto provider = model.try_as<D::ICustomPropertyProvider>())
    if (auto member = provider.GetCustomProperty(name); member && member.CanWrite()) {
      member.SetValue(model, next);
      return true;
    }
  if (auto member = ModelMember(model, name); member && !member.IsReadOnly()) {
    member.SetValue(model, next);
    return true;
  }
  return false;
}
// Let the shell's binding resolver access its model, including private XAML
// property adapters. The displayed Slider and its Binding are never assigned.
struct NumberRelay : winrt::implements<NumberRelay, D::IValueConverter> {
  double wanted;
  bool read = false, wrote = false;
  IInspectable original{nullptr};
  D::IValueConverter converter{nullptr};
  IInspectable parameter{nullptr};
  winrt::hstring language;
  NumberRelay(double value, D::Binding const &binding)
      : wanted(value), converter(binding.Converter()), parameter(binding.ConverterParameter()),
        language(binding.ConverterLanguage()) {}
  IInspectable Convert(IInspectable const &value, X::Interop::TypeName const &,
                       IInspectable const &, winrt::hstring const &) {
    original = value;
    read = true;
    return winrt::box_value(wanted);
  }
  IInspectable ConvertBack(IInspectable const &, X::Interop::TypeName const &type,
                           IInspectable const &, winrt::hstring const &lang) {
    auto value = converter ? converter.ConvertBack(winrt::box_value(wanted), type, parameter,
                                                   language.empty() ? lang : language)
                           : NumberLike(original, wanted);
    wrote = bool(value);
    return value;
  }
};
inline bool SendBoundNumber(C::Slider const &target, IInspectable const &model, double number) {
  auto original = target.GetBindingExpression(P::RangeBase::ValueProperty());
  if (!original || !model)
    return false;
  auto parent = original.ParentBinding();
  if (parent.Mode() != D::BindingMode::TwoWay)
    return false;
  C::Slider proxy;
  proxy.Minimum(target.Minimum());
  proxy.Maximum(target.Maximum());
  auto relay = winrt::make_self<NumberRelay>(number, parent);
  D::Binding binding;
  binding.Source(model);
  binding.Path(parent.Path());
  binding.Mode(D::BindingMode::TwoWay);
  binding.UpdateSourceTrigger(D::UpdateSourceTrigger::Explicit);
  binding.Converter(*relay);
  proxy.SetBinding(P::RangeBase::ValueProperty(), binding);
  try {
    if (relay->read && (relay->converter || NumberLike(relay->original, number)))
      if (auto expression = proxy.GetBindingExpression(P::RangeBase::ValueProperty()))
        expression.UpdateSource();
  } catch (...) {
    proxy.ClearValue(P::RangeBase::ValueProperty());
    throw;
  }
  proxy.ClearValue(P::RangeBase::ValueProperty());
  return relay->wrote && target.GetBindingExpression(P::RangeBase::ValueProperty()) == original;
}
inline std::wstring String(IInspectable const &value) {
  if (!value)
    return {};
  if (auto p = value.try_as<winrt::Windows::Foundation::IPropertyValue>();
      p && p.Type() == winrt::Windows::Foundation::PropertyType::String)
    return std::wstring(p.GetString());
  return {};
}
} // namespace native



#include <winrt/Microsoft.UI.Xaml.Controls.h>
namespace native {
namespace U = winrt::Microsoft::UI::Xaml::Controls;
// Exact system Lottie asset: no SVG replacement or authored volume animation.
inline winrt::Windows::Foundation::IActivationFactory NativeFactory(PCWSTR type, HMODULE module) {
  using Get = HRESULT(WINAPI *)(void *, void **);
  auto get = reinterpret_cast<Get>(GetProcAddress(module, "DllGetActivationFactory"));
  if (!get)
    throw winrt::hresult_error(E_NOINTERFACE);
  winrt::Windows::Foundation::IActivationFactory factory{nullptr};
  winrt::hstring name(type);
  winrt::check_hresult(get(winrt::get_abi(name), winrt::put_abi(factory)));
  return factory;
}
inline U::IAnimatedVisualSource2 NativeSource(PCWSTR type) {
  auto module = GetModuleHandleW(L"ControlCenter.dll");
  // One process-lifetime system resource load, reused on later mod reloads.
  // Composition can retain the native visual after its XAML owner is removed.
  if (!module)
    module = LoadLibraryExW(L"ControlCenter.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!module)
    winrt::throw_last_error();
  return NativeFactory(type, module)
      .ActivateInstance<IInspectable>()
      .as<U::IAnimatedVisualSource2>();
}
inline U::IAnimatedVisualSource2 NativeVolumeSource() {
  return NativeSource(L"ControlCenter.QA_Volume");
}
inline winrt::Windows::Foundation::IActivationFactory NativeAnimatedFactory() {
  try {
    return winrt::get_activation_factory<U::AnimatedIcon>();
  } catch (...) {
    auto module = GetModuleHandleW(L"Microsoft.UI.Xaml.dll");
    if (!module)
      throw;
    return NativeFactory(L"Microsoft.UI.Xaml.Controls.AnimatedIcon", module);
  }
}
inline PCWSTR VolumeState(float value, bool muted) {
  return muted           ? L"Mute"
         : value <= 0    ? L"Volume_00"
         : value <= .33f ? L"Volume_01"
         : value <= .66f ? L"Volume_33"
                         : L"Volume_66";
}
// AnimatedIcon queues a state change that arrives during a transition and
// plays it after the current one, so quick presses of a volume button played
// the earlier animations in turn. A change during a transition starts fresh: a
// new AnimatedIcon (same system QA_Volume asset) is put into the host showing
// the previous target state, and on load it gets the new state, so only the
// latest transition plays; the old icon is removed then.
class AnimatedVolume {
  U::AnimatedIcon icon_{nullptr};
  C::Grid host_{nullptr};
  U::IAnimatedIconStatics statics_{nullptr};
  winrt::Windows::Foundation::IActivationFactory factory_{nullptr};
  std::wstring state_;
  int64_t stateToken_ = 0;
  bool reapplying_ = false;
  ULONGLONG changedAt_ = 0;
  winrt::event_token loadedToken_{}, lostToken_{};
  U::AnimatedIcon loading_{nullptr};
  static constexpr ULONGLONG transitionMs = 700; // QA_Volume transitions are shorter
  U::AnimatedIcon Make() {
    auto source = NativeVolumeSource();
    IInspectable inner{nullptr};
    auto icon = factory_.as<U::IAnimatedIconFactory>().CreateInstance(nullptr, inner);
    // 16 DIP, the size of the shell's own icons.
    icon.Source(source);
    icon.Width(16);
    icon.Height(16);
    icon.MirroredWhenRightToLeft(false);
    icon.HorizontalAlignment(X::HorizontalAlignment::Center);
    icon.VerticalAlignment(X::VerticalAlignment::Center);
    icon.IsHitTestVisible(false);
    if (!source.Markers().HasKey(L"MuteToVolume_01_End"))
      throw winrt::hresult_error(E_NOTIMPL, L"Unknown native volume markers");
    return icon;
  }
  // Hovering the SV button still switched the glyph to Mute with the Grid
  // isolation: whoever writes AnimatedIcon.State on the icon (QA_Volume has
  // no PointerOver/Normal states), the volume state set by Update() wins.
  void Watch() {
    if (icon_)
      stateToken_ = icon_.RegisterPropertyChangedCallback(
          statics_.StateProperty(),
          [this](X::DependencyObject const &, X::DependencyProperty const &) { Reapply(); });
  }
  void Unwatch() {
    if (icon_ && stateToken_)
      try {
        icon_.UnregisterPropertyChangedCallback(statics_.StateProperty(), stateToken_);
      } catch (...) {
      }
    stateToken_ = 0;
  }
  void SetOn(U::AnimatedIcon const &icon, std::wstring const &state) {
    reapplying_ = true;
    try {
      statics_.SetState(icon, state);
    } catch (...) {
      reapplying_ = false;
      throw;
    }
    reapplying_ = false;
  }
  void Fresh(std::wstring const &from) {
    auto next = Make();
    SetOn(next, from);
    if (loading_) {
      try {
        loading_.Loaded(loadedToken_);
      } catch (...) {
      }
    }
    Unwatch();
    auto old = icon_;
    icon_ = next;
    Watch();
    // The previous icon stays until the new one is loaded, so no frame is empty.
    auto weakOld = winrt::make_weak(old);
    auto weakHost = winrt::make_weak(host_);
    loading_ = next;
    loadedToken_ = next.Loaded([this, weakOld, weakHost](IInspectable const &sender, auto &&) {
      try {
        auto icon = sender.as<U::AnimatedIcon>();
        icon.Loaded(loadedToken_);
        if (loading_ == icon)
          loading_ = nullptr;
        if (icon == icon_ && !state_.empty())
          SetOn(icon, state_);
        if (auto h = weakHost.get())
          for (int32_t k = int32_t(h.Children().Size()) - 1; k >= 0; k--) {
            auto child = h.Children().GetAt(uint32_t(k)).try_as<U::AnimatedIcon>();
            if (!child || child != icon_)
              h.Children().RemoveAt(uint32_t(k));
          }
      } catch (...) {
      }
    });
    host_.Children().Append(next);
  }

public:
  AnimatedVolume() {
    factory_ = NativeAnimatedFactory();
    statics_ = factory_.as<U::IAnimatedIconStatics>();
    icon_ = Make();
    Watch();
    // AnimatedIcon also follows AnimatedIcon.State set on its parent: a button
    // template sets "PointerOver"/"Pressed" there. QA_Volume has no such states,
    // so hovering the SV button left the glyph in its Mute frame. A plain Grid as
    // the direct parent keeps the state set only by Update().
    host_ = C::Grid();
    host_.IsHitTestVisible(false);
    host_.HorizontalAlignment(X::HorizontalAlignment::Center);
    host_.VerticalAlignment(X::VerticalAlignment::Center);
    host_.Children().Append(icon_);
    // After the graphics device was lost (display off and on, sleep) the
    // Lottie picture comes back on its first frame (the Mute pose) while
    // State keeps its value, so the icon is rebuilt at its current state.
    try {
      lostToken_ = M::CompositionTarget::SurfaceContentsLost([this](auto &&, auto &&) { Redraw(); });
    } catch (...) {
    }
  }
  AnimatedVolume(AnimatedVolume const &) = delete;
  AnimatedVolume &operator=(AnimatedVolume const &) = delete;
  ~AnimatedVolume() {
    if (lostToken_)
      try {
        M::CompositionTarget::SurfaceContentsLost(lostToken_);
      } catch (...) {
      }
    Unwatch();
    if (loading_)
      try {
        loading_.Loaded(loadedToken_);
      } catch (...) {
      }
  }
  void Reapply() {
    if (reapplying_ || state_.empty() || !icon_)
      return;
    try {
      auto now = std::wstring(statics_.GetState(icon_));
      if (now == state_)
        return;
      SetOn(icon_, state_);
    } catch (...) {
    }
  }
  // A fresh icon showing the current state, without a transition.
  void Redraw() {
    if (state_.empty() || !host_ || !host_.IsLoaded())
      return;
    try {
      Fresh(state_);
    } catch (...) {
      Wh_Log(L"VolumeIconRedrawError %08X", winrt::to_hresult());
    }
  }
  // Always the isolating Grid (the icon inside it is replaced).
  X::FrameworkElement Isolated() { return host_; }
  void Update(float value, bool muted, bool available = true) {
    std::wstring state = available ? VolumeState(value, muted) : L"Disabled";
    if (state_ == state)
      return;
    auto previous = state_;
    state_ = state;
    auto now = GetTickCount64();
    bool playing = !previous.empty() && now - changedAt_ < transitionMs;
    changedAt_ = now;
    if (playing && host_ && host_.IsLoaded()) {
      try {
        Fresh(previous);
        return;
      } catch (...) {
        Wh_Log(L"VolumeIconFreshError %08X", winrt::to_hresult());
      }
    }
    if (loading_)
      return; // the loading icon takes state_ when it is loaded
    SetOn(icon_, state);
  }
};
} // namespace native



namespace native {
// Thin slider look shared by the Sound page and Quick Settings:
// 1 DIP rail, 4 DIP value bar with rounded ends, thumb invisible until hover,
// drag or keyboard focus. The rail and the bar run from the thumb centre at 0
// to the thumb centre at 100, so the bar always ends under the (hidden) thumb
// and 100 fills the whole line; the caller widens the Slider by half a thumb
// on each side through the margin. Native template parts, states, Fill brushes
// and bindings are kept; everything is journaled.
struct SliderLook {
  Journal &journal;
  std::vector<std::function<void()>> &revoke;
  std::set<void *> &seen;
  bool onBase =
      true; // rail on the base acrylic (QS dark block, SV), not on the Sound content layer
  void Set(X::DependencyObject const &object, X::DependencyProperty const &property,
           IInspectable const &value) {
    journal.Save(object, property);
    object.SetValue(property, value);
  }
  template <class R> void Keep(R &&r) {
    auto revoker = std::make_shared<std::decay_t<R>>(std::forward<R>(r));
    revoke.push_back([revoker] { revoker->revoke(); });
  }
  // 32 DIP control with the rail centred: the native template is 44 DIP tall
  // with a 3 DIP top inset that otherwise offsets the rail from the icon.
  void Geometry(C::Slider const &slider) {
    Set(slider, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
    Set(slider, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
    Set(slider, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{}));
    Set(slider, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    slider.ApplyTemplate();
    if (auto track = Named(slider, L"HorizontalTemplate").try_as<C::Grid>()) {
      Set(track, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
      Set(track, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
      Set(track, C::Grid::PaddingProperty(), winrt::box_value(X::Thickness{}));
      if (track.RowDefinitions().Size() == 3)
        for (uint32_t i : {0u, 2u})
          Set(track.RowDefinitions().GetAt(i), C::RowDefinition::HeightProperty(),
              winrt::box_value(X::GridLengthHelper::FromValueAndType(1, X::GridUnitType::Star)));
    }
  }
  // The rail in the colour of the block dividers. The native rail stays
  // (transparent): its PointerOver/Pressed/Disabled states animate its Fill,
  // which would override a local Fill. A 1 DIP rectangle in the same template
  // cell takes its place, below the value bar.
  void Rail(X::Shapes::Rectangle const &rail, double half) {
    auto grid = Parent(rail).try_as<C::Panel>();
    if (!grid)
      return;
    X::Shapes::Rectangle mine{nullptr};
    for (auto const &child : grid.Children())
      if (auto r = child.try_as<X::Shapes::Rectangle>(); r && r.Name() == L"NativeSoundRail") {
        mine = r;
        break;
      }
    if (!mine) {
      mine = X::Shapes::Rectangle();
      mine.Name(L"NativeSoundRail");
      mine.IsHitTestVisible(false);
      mine.Height(1);
      mine.VerticalAlignment(X::VerticalAlignment::Center);
      mine.RadiusX(.5);
      mine.RadiusY(.5);
      C::Grid::SetRow(mine, C::Grid::GetRow(rail));
      C::Grid::SetRowSpan(mine, C::Grid::GetRowSpan(rail));
      C::Grid::SetColumn(mine, C::Grid::GetColumn(rail));
      C::Grid::SetColumnSpan(mine, C::Grid::GetColumnSpan(rail));
      uint32_t index = 0;
      if (grid.Children().IndexOf(rail, index))
        grid.Children().InsertAt(index + 1, mine);
      else
        grid.Children().Append(mine);
      auto weakGrid = winrt::make_weak(grid);
      journal.undo.push_back([weakGrid, mine] {
        if (auto g = weakGrid.get()) {
          uint32_t i = 0;
          if (g.Children().IndexOf(mine, i))
            g.Children().RemoveAt(i);
        }
      });
      Set(rail, X::UIElement::OpacityProperty(), winrt::box_value(0.));
      auto weakRail = winrt::make_weak(rail);
      Keep(mine.ActualThemeChanged(
          winrt::auto_revoke,
          [weakRail, onBase = onBase](X::FrameworkElement const &sender, auto &&) {
            if (auto r = weakRail.get())
              if (auto shape = sender.try_as<X::Shapes::Shape>())
                PaintDivider(shape, r, onBase);
          }));
    }
    mine.Margin({half, 0, half, 0});
    PaintDivider(mine, rail, onBase);
  }
  // margin(half) gives the Slider margin for half the thumb width.
  bool Thin(C::Slider const &slider, std::function<X::Thickness(double)> const &margin) {
    auto rail = Named(slider, L"HorizontalTrackRect").try_as<X::Shapes::Rectangle>();
    auto bar = Named(slider, L"HorizontalDecreaseRect").try_as<X::Shapes::Rectangle>();
    auto thumb = Named(slider, L"HorizontalThumb").try_as<P::Thumb>();
    if (!rail || !bar || !thumb)
      return false;
    double half =
        std::max(0., (std::isfinite(thumb.Width()) && thumb.Width() > 0 ? thumb.Width()
                                                                        : thumb.ActualWidth()) /
                         2);
    Set(rail, X::FrameworkElement::HeightProperty(), winrt::box_value(1.));
    Set(rail, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    Set(rail, X::FrameworkElement::MarginProperty(),
        winrt::box_value(X::Thickness{half, 0, half, 0}));
    Set(rail, X::Shapes::Rectangle::RadiusXProperty(), winrt::box_value(.5));
    Set(rail, X::Shapes::Rectangle::RadiusYProperty(), winrt::box_value(.5));
    Set(bar, X::FrameworkElement::HeightProperty(), winrt::box_value(4.));
    Set(bar, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    Set(bar, X::FrameworkElement::MarginProperty(),
        winrt::box_value(X::Thickness{half, 0, -half, 0}));
    Set(bar, X::Shapes::Rectangle::RadiusXProperty(), winrt::box_value(2.));
    Set(bar, X::Shapes::Rectangle::RadiusYProperty(), winrt::box_value(2.));
    Rail(rail, half);
    Set(slider, X::FrameworkElement::MarginProperty(), winrt::box_value(margin(half)));
    if (!seen.insert(winrt::get_abi(slider)).second)
      return true;
    Set(thumb, X::UIElement::OpacityProperty(), winrt::box_value(0.));
    auto previous = thumb.OpacityTransition();
    X::ScalarTransition fade;
    fade.Duration(std::chrono::milliseconds(83));
    thumb.OpacityTransition(fade);
    journal.undo.push_back([thumb, previous] { thumb.OpacityTransition(previous); });
    struct Visible {
      bool over = false, drag = false, focus = false;
    };
    auto state = std::make_shared<Visible>();
    auto weakThumb = winrt::make_weak(thumb);
    auto apply = [state, weakThumb] {
      if (auto t = weakThumb.get())
        t.Opacity(state->over || state->drag || state->focus ? 1. : 0.);
    };
    auto entered = winrt::box_value(X::Input::PointerEventHandler([state, apply](auto &&, auto &&) {
      state->over = true;
      apply();
    }));
    auto exited = winrt::box_value(X::Input::PointerEventHandler([state, apply](auto &&, auto &&) {
      state->over = false;
      apply();
    }));
    slider.AddHandler(X::UIElement::PointerEnteredEvent(), entered, true);
    slider.AddHandler(X::UIElement::PointerExitedEvent(), exited, true);
    revoke.push_back([slider, entered, exited] {
      slider.RemoveHandler(X::UIElement::PointerEnteredEvent(), entered);
      slider.RemoveHandler(X::UIElement::PointerExitedEvent(), exited);
    });
    Keep(thumb.DragStarted(winrt::auto_revoke, [state, apply](auto &&, auto &&) {
      state->drag = true;
      apply();
    }));
    Keep(thumb.DragCompleted(winrt::auto_revoke, [state, apply](auto &&, auto &&) {
      state->drag = false;
      apply();
    }));
    // Pressing the track moves the value without a thumb drag: show it while pressed.
    auto pressed = winrt::box_value(X::Input::PointerEventHandler([state, apply](auto &&, auto &&) {
      state->drag = true;
      apply();
    }));
    auto released =
        winrt::box_value(X::Input::PointerEventHandler([state, apply](auto &&, auto &&) {
          state->drag = false;
          apply();
        }));
    slider.AddHandler(X::UIElement::PointerPressedEvent(), pressed, true);
    for (auto ev : {X::UIElement::PointerReleasedEvent(), X::UIElement::PointerCaptureLostEvent(),
                    X::UIElement::PointerCanceledEvent()})
      slider.AddHandler(ev, released, true);
    revoke.push_back([slider, pressed, released] {
      slider.RemoveHandler(X::UIElement::PointerPressedEvent(), pressed);
      for (auto ev : {X::UIElement::PointerReleasedEvent(), X::UIElement::PointerCaptureLostEvent(),
                      X::UIElement::PointerCanceledEvent()})
        slider.RemoveHandler(ev, released);
    });
    Keep(slider.GotFocus(winrt::auto_revoke,
                         [state, apply, weak = winrt::make_weak(slider)](auto &&, auto &&) {
                           if (auto s = weak.get())
                             state->focus = s.FocusState() == X::FocusState::Keyboard;
                           apply();
                         }));
    Keep(slider.LostFocus(winrt::auto_revoke, [state, apply](auto &&, auto &&) {
      state->focus = false;
      apply();
    }));
    return true;
  }
};
} // namespace native



#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.System.h>
namespace native {
// Hiding the page being left (Quick Settings on the way to Sound, Sound on the
// way back): opacity 1 -> 0 in 200 ms, decelerating; the shell's entrance of
// the new page stays as it is.
// The shell shows the leaving page as its own copy (IDuplicateVisual), which
// the mod cannot animate or move. So the button's command is wrapped: on
// Execute the page is rendered once (RenderTargetBitmap), the picture is laid
// over the panel, the page is made transparent (so the shell's copy shows
// nothing), the original command runs, and the picture fades in composition.
// The picture is bottom-aligned in RootGrid: the panel grows and shrinks from
// its bottom edge, so it stays where it was on screen while the panel's height
// changes. It hangs from a zero-height Canvas, so it never takes part in
// layout, and is clipped to RootGrid.
// The shell takes its copy from what XAML last rendered, so the page is made
// transparent in XAML and the command runs only after a frame with that state
// (and the picture) has been rendered. For Sound the copied element is L2Frame,
// the parent of the page; for Quick Settings it is L1Grid itself and the
// shell's fade starts the copy at full opacity, so for a panel its children
// are made transparent instead (Quick Settings restores them when shown).
inline void HideNow(X::UIElement const &e) {
  if (!e)
    return;
  if (auto panel = e.try_as<C::Panel>()) {
    for (auto const &child : panel.Children())
      child.Opacity(0);
    return;
  }
  e.Opacity(0);
}
inline void AfterRender(std::function<void()> next) {
  struct State {
    winrt::event_token token{};
    int frames = 0;
    bool done = false;
  };
  auto state = std::make_shared<State>();
  auto finish = [state, next] {
    if (state->done)
      return;
    state->done = true;
    try {
      M::CompositionTarget::Rendering(state->token);
    } catch (...) {
    }
    next();
  };
  // Run `next` after the Rendering callback, not inside it: navigating from
  // inside Rendering renders the native Sound page once before the mod has
  // restructured it.
  auto post = [finish] {
    try {
      if (auto q = winrt::Windows::System::DispatcherQueue::GetForCurrentThread())
        if (q.TryEnqueue([finish] { finish(); }))
          return;
    } catch (...) {
    }
    finish();
  };
  try {
    state->token = M::CompositionTarget::Rendering([state, post](auto &&, auto &&) {
      if (++state->frames == 2)
        post();
    });
  } catch (...) {
    finish();
  }
}
// Fades an element's visual to 0 (composition, independent of the UI thread);
// ShowAgain gives it its opacity back.
inline void FadeOutNow(X::UIElement const &e, int ms) {
  try {
    auto v = X::Hosting::ElementCompositionPreview::GetElementVisual(e);
    auto c = v.Compositor();
    auto a = c.CreateScalarKeyFrameAnimation();
    using winrt::Windows::Foundation::Numerics::float2;
    a.InsertKeyFrame(0, 1);
    a.InsertKeyFrame(1, 0, c.CreateCubicBezierEasingFunction(float2{0, 0}, float2{0, 1}));
    a.Duration(std::chrono::milliseconds(ms));
    v.StartAnimation(L"Opacity", a);
  } catch (...) {
  }
}
inline void AfterDelay(int ms, std::function<void()> next) {
  try {
    auto q = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!q) {
      next();
      return;
    }
    auto timer = q.CreateTimer();
    timer.Interval(std::chrono::milliseconds(ms));
    timer.IsRepeating(false);
    auto keep = std::make_shared<winrt::Windows::System::DispatcherQueueTimer>(timer);
    timer.Tick([keep, next](auto &&, auto &&) {
      try {
        keep->Stop();
      } catch (...) {
      }
      next();
    });
    timer.Start();
  } catch (...) {
    next();
  }
}
inline void ShowAgain(X::UIElement const &e) {
  if (!e)
    return;
  try {
    auto v = X::Hosting::ElementCompositionPreview::GetElementVisual(e);
    v.StopAnimation(L"Opacity");
    v.Opacity(1);
  } catch (...) {
  }
}
struct DelayedCommand : winrt::implements<DelayedCommand, X::Input::ICommand> {
  X::Input::ICommand original;
  std::function<void(IInspectable const &)> run;
  DelayedCommand(X::Input::ICommand o, std::function<void(IInspectable const &)> r)
      : original(std::move(o)), run(std::move(r)) {}
  bool CanExecute(IInspectable const &p) { return original.CanExecute(p); }
  void Execute(IInspectable const &p) { run(p); }
  winrt::event_token
  CanExecuteChanged(winrt::Windows::Foundation::EventHandler<IInspectable> const &h) {
    return original.CanExecuteChanged(h);
  }
  void CanExecuteChanged(winrt::event_token const &t) noexcept {
    try {
      original.CanExecuteChanged(t);
    } catch (...) {
    }
  }
};
constexpr int exitDuration = 200;
inline winrt::Windows::UI::Composition::CompositionEasingFunction
ExitEasing(winrt::Windows::UI::Composition::Compositor const &c) {
  using winrt::Windows::Foundation::Numerics::float2;
  return c.CreateCubicBezierEasingFunction(float2{0, 0}, float2{0, 1});
}
// snapshot: element rendered and laid over (L1Grid, L2Frame); hide: element
// made transparent before the shell takes its copy (L1Grid, the Sound page).
inline void ExitWithSnapshot(X::FrameworkElement const &snapshot, X::FrameworkElement const &hide,
                             std::function<void()> proceed) {
  auto host = Parent(snapshot).try_as<C::Grid>();
  if (!host || snapshot.ActualHeight() < 1 || snapshot.ActualWidth() < 1) {
    proceed();
    return;
  }
  M::Imaging::RenderTargetBitmap natural;
  auto weakSnapshot = winrt::make_weak(snapshot), weakHide = winrt::make_weak(hide);
  auto weakHost = winrt::make_weak(host);
  // Lays the picture over the panel; pw x ph is the natural render size in pixels.
  std::function<void(M::Imaging::RenderTargetBitmap, int, int,
                     winrt::Windows::Foundation::AsyncStatus)>
      show = [=](M::Imaging::RenderTargetBitmap bitmap, int pw, int ph,
                 winrt::Windows::Foundation::AsyncStatus status) {
        auto s = weakSnapshot.get();
        auto h = weakHost.get();
        if (status != winrt::Windows::Foundation::AsyncStatus::Completed || !s || !h) {
          proceed();
          return;
        }
        C::Grid box{nullptr};
        try {
          auto at = s.TransformToVisual(h).TransformPoint({0, 0});
          double w = s.ActualWidth(), ht = s.ActualHeight();
          double bottom = h.ActualHeight() - (at.Y + ht);
          box = C::Grid();
          box.IsHitTestVisible(false);
          C::Canvas::SetZIndex(box, 1000);
          C::Grid::SetRowSpan(box, std::max(1u, h.RowDefinitions().Size()));
          C::Grid::SetColumnSpan(box, std::max(1u, h.ColumnDefinitions().Size()));
          // The bitmap can cover more than the element's layout box (items with
          // negative margins): natural size, top-left at the element's origin.
          auto root = s.XamlRoot();
          double scale = root ? root.RasterizationScale() : 1.;
          double iw = pw > 0 ? pw / scale : w, ih = ph > 0 ? ph / scale : ht;
          C::Image image;
          image.Source(bitmap);
          image.Stretch(M::Stretch::Fill);
          image.Width(iw);
          image.Height(ih);
          C::Canvas anchor;
          anchor.Height(0);
          anchor.VerticalAlignment(X::VerticalAlignment::Bottom);
          anchor.Margin({0, 0, 0, bottom});
          C::Canvas::SetLeft(image, at.X);
          C::Canvas::SetTop(image, -ht);
          anchor.Children().Append(image);
          box.Children().Append(anchor);
          // Clipped at the host's edges (the panel shrinking under Sound's picture).
          {
            M::RectangleGeometry geometry;
            geometry.Rect({0, 0, float(h.ActualWidth()), float(h.ActualHeight())});
            box.Clip(geometry);
            auto weakClip = winrt::make_weak(geometry);
            box.SizeChanged([weakClip](auto &&, X::SizeChangedEventArgs const &args) {
              if (auto g = weakClip.get())
                g.Rect({0, 0, args.NewSize().Width, args.NewSize().Height});
            });
          }
          h.Children().Append(box);
          if (auto e = weakHide.get())
            HideNow(e);
          AfterRender(proceed);
          namespace Comp = winrt::Windows::UI::Composition;
          auto v = X::Hosting::ElementCompositionPreview::GetElementVisual(image);
          auto c = v.Compositor();
          auto ease = ExitEasing(c);
          auto duration = std::chrono::milliseconds(exitDuration);
          auto batch = c.CreateScopedBatch(Comp::CompositionBatchTypes::Animation);
          auto fade = c.CreateScalarKeyFrameAnimation();
          fade.InsertKeyFrame(0, 1);
          fade.InsertKeyFrame(1, 0, ease);
          fade.Duration(duration);
          v.StartAnimation(L"Opacity", fade);
          batch.End();
          auto weakBox = winrt::make_weak(box);
          batch.Completed([weakHost, weakBox](auto &&, auto &&) {
            if (auto h2 = weakHost.get())
              if (auto b = weakBox.get()) {
                uint32_t i = 0;
                if (h2.Children().IndexOf(b, i))
                  h2.Children().RemoveAt(i);
              }
          });
        } catch (...) {
          Wh_Log(L"ExitSnapshotError %08X", winrt::to_hresult());
          if (box)
            try {
              uint32_t i = 0;
              if (h.Children().IndexOf(box, i))
                h.Children().RemoveAt(i);
            } catch (...) {
            }
          if (auto e = weakHide.get())
            HideNow(e);
          proceed();
        }
      };
  // Animated (Lottie) glyphs come out pixelated at the natural size while text
  // is sharp: a second render at twice the size, shown at the natural size.
  // Falls back to the natural render.
  try {
    natural.RenderAsync(snapshot).Completed(
        [=](auto const &, winrt::Windows::Foundation::AsyncStatus status) {
          int pw = natural.PixelWidth(), ph = natural.PixelHeight();
          auto s = weakSnapshot.get();
          if (status != winrt::Windows::Foundation::AsyncStatus::Completed || !s || pw <= 0 ||
              ph <= 0) {
            show(natural, pw, ph, status);
            return;
          }
          try {
            M::Imaging::RenderTargetBitmap twice;
            twice.RenderAsync(s, pw * 2, ph * 2)
                .Completed([=](auto const &, winrt::Windows::Foundation::AsyncStatus second) {
                  if (second == winrt::Windows::Foundation::AsyncStatus::Completed &&
                      twice.PixelWidth() > pw)
                    show(twice, pw, ph, second);
                  else
                    show(natural, pw, ph, status);
                });
          } catch (...) {
            show(natural, pw, ph, status);
          }
        });
  } catch (...) {
    Wh_Log(L"ExitSnapshotError %08X", winrt::to_hresult());
    proceed();
  }
}
// The wrapper's Execute; with animations off the command runs at once.
inline void Leave(X::FrameworkElement const &snapshot, X::FrameworkElement const &hide,
                  X::Input::ICommand const &original, IInspectable const &parameter) {
  auto proceed = [original, parameter] {
    try {
      original.Execute(parameter);
    } catch (...) {
    }
  };
  if (winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) {
    ExitWithSnapshot(snapshot, hide, proceed);
    return;
  }
  proceed();
}
} // namespace native

#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
namespace native {
struct Menu {
  C::Button button{nullptr};
  C::TextBlock label{nullptr};
  C::FontIcon endpointIcon{nullptr};
  C::Flyout flyout{nullptr};
  C::ListView list{nullptr};
  X::FrameworkElement arrow{nullptr};
  X::Media::Animation::Storyboard arrowStory{nullptr};
  bool open = false;
};
struct AppRow {
  winrt::weak_ref<C::Grid> root;
  winrt::weak_ref<C::Slider> slider;
  winrt::weak_ref<P::ToggleButton> toggle;
  C::Border badge{nullptr};
  C::TextBlock level{nullptr};
  C::Button levelButton{nullptr}, levelMute{nullptr};
  std::shared_ptr<AnimatedVolume> levelIcon;
  double last = 50, before = 0, expected = NAN;
  ULONGLONG verifyAt = 0;
  IInspectable model{nullptr};
  bool beforeValid = false;
  ULONGLONG unnamedSince = 0;
  std::wstring name;
  IInspectable nameModel{nullptr};
  double wheelTarget = NAN;
  ULONGLONG wheelAt = 0;
};
// Last complete Core Audio snapshot from any Sound page. A new page's worker
// has not read anything during the first frames (blank Input, master at 0).
inline sound::Snapshot lastAudio;
inline bool soundOpenedOnce = false;
class SoundPage {
  X::FrameworkElement page_{nullptr}, outputAnchor_{nullptr}, frame_{nullptr};
  Journal journal_;
  // L2Frame outlives the page (it hosts every Sound page): its changes are kept
  // apart so an abandoned page still gives the frame its native height back.
  Journal frameJournal_;
  HWND host_ = nullptr;
  std::shared_ptr<double> ownedHeight_ = std::make_shared<double>(NAN);
  double wantedHeight_ = 0;
  std::vector<std::shared_ptr<Menu>> menus_;
  std::vector<std::shared_ptr<AppRow>> rows_;
  std::vector<std::function<void()>> revoke_;
  C::ListView mixerList_{nullptr};
  C::Button mixerButton_{nullptr};
  X::FrameworkElement arrow_{nullptr};
  C::StackPanel inputBlock_{nullptr};
  std::shared_ptr<Menu> input_;
  C::Slider master_{nullptr};
  C::Button masterIcon_{nullptr};
  C::Grid footer_{nullptr};
  C::Border footerBorder_{nullptr};
  X::Style pickerBase_{nullptr};
  std::shared_ptr<AnimatedVolume> masterAnimation_;
  std::function<void()> volumeChanged_;
  bool preparing_ = false, prepared_ = false, rowsDirty_ = true;
  std::vector<std::pair<winrt::weak_ref<C::TextBlock>, M::TranslateTransform>> textBaselines_;
  sound::Audio audio_;
  bool sync_ = false, expanded_ = true, restoring_ = false;
  int glyph_ = -1;
  std::wstring inputSignature_;
  sound::Snapshot latest_;
  unsigned long long pending_ = 0;
  std::set<void *> appIds_;
  std::wstring pendingInputId_;
  unsigned long long pendingInput_ = 0;
  X::DispatcherTimer gate_{nullptr};
  winrt::event_token gateTick_{};
  ULONGLONG gateStarted_ = 0;
  int gateStable_ = 0; // Snapshot of the VM rows, shared with RenderAsync's completion handler.
  struct MixerSnapshot {
    std::mutex lock;
    M::Imaging::RenderTargetBitmap bitmap{nullptr};
    std::vector<void *> items;
    double height = 0;
    ULONGLONG at = 0;
    bool capturing = false;
  };
  std::shared_ptr<MixerSnapshot> snapshot_ = std::make_shared<MixerSnapshot>();
  M::ImageBrush snapshotBrush_{nullptr};
  X::UIElement itemsHost_{nullptr};
  IInspectable savedBackground_{nullptr};
  bool crossfade_ = false, holdAnimation_ = false;
  double stableListHeight_ = -1;
  bool hideUnnamed_ = false, holding_ = false, pendingRows_ = false;
  ULONGLONG holdStarted_ = 0;
  int holdStable_ = 0;
  std::wstring holdSignature_;
  std::vector<void *> rowItems_;
  X::DispatcherTimer holdTimer_{nullptr};
  winrt::event_token holdTick_{};
  X::DispatcherTimer rowsTimer_{nullptr};
  winrt::event_token rowsTick_{};
  ULONGLONG layoutWorkTick_ = 0;
  int layoutWorkCount_ = 0;
  std::wstring rowSignature_;
  bool fading_ = false;
  std::wstring gateLabels_;
  bool gating_ = false;
  double heldHeight_ = 0;
  int savedExpanded_ = -1, savedCollapsed_ = -1;
  Journal gateJournal_;
  X::DispatcherTimer animation_{nullptr};
  winrt::event_token animationTick_{};
  double start_ = 0;
  double from_ = 0, to_ = 0, arrowStart_ = 0;
  X::Media::Animation::Storyboard gearStory_{nullptr};
  C::TextBlock pageTitle_{nullptr};
  // VM rows on opening: the new page first shows the previous session list, then
  // the shell replaces it; the rows are hidden until the list settles, then fade.
  bool openHold_ = false, openRebuilt_ = false;
  // Page title ("Sound"): its font family for the app volume numbers.
  // The native QA_All_Settings animation in FooterIconButtonStyle, as the Quick
  // Settings gear: it turns while the button is held.
  void SettingsGear(C::Button const &button) {
    try {
      if (auto style = Resource(page_, L"FooterIconButtonStyle").try_as<X::Style>()) {
        button.Style(style);
        button.Width(36);
        button.Height(36);
        button.Margin({-2, -2, -2, -2});
      }
      auto factory = NativeAnimatedFactory();
      IInspectable inner{nullptr};
      auto icon = factory.as<U::IAnimatedIconFactory>().CreateInstance(nullptr, inner);
      auto statics = factory.as<U::IAnimatedIconStatics>();
      icon.Source(NativeSource(L"ControlCenter.QA_All_Settings"));
      icon.Width(16);
      icon.Height(16);
      icon.IsHitTestVisible(false);
      button.Content(icon);
      // The footer style writes AnimatedIcon.State on the button; the icon follows it.
      auto token = button.RegisterPropertyChangedCallback(
          statics.StateProperty(),
          [statics, icon](X::DependencyObject const &sender, X::DependencyProperty const &) {
            try {
              statics.SetState(icon, statics.GetState(sender));
            } catch (...) {
            }
          });
      revoke_.push_back([button, statics, token] {
        button.UnregisterPropertyChangedCallback(statics.StateProperty(), token);
      });
    } catch (...) {
      Wh_Log(L"SoundGearIconError %08X", winrt::to_hresult());
      button.Content(SymbolGlyph(page_, 0xe713, 14));
    }
  }
  template <class R> void Keep(R &&r) {
    auto revoker = std::make_shared<std::decay_t<R>>(std::forward<R>(r));
    revoke_.push_back([revoker] { revoker->revoke(); });
  }
  static void Rotate(X::FrameworkElement const &glyph, X::Media::Animation::Storyboard &previous,
                     double target, int milliseconds) {
    namespace A = X::Media::Animation;
    auto rotation = glyph.RenderTransform().try_as<M::RotateTransform>();
    if (!rotation) {
      rotation = M::RotateTransform();
      glyph.RenderTransformOrigin({.5f, .5f});
      glyph.RenderTransform(rotation);
    }
    auto from = rotation.Angle();
    if (previous)
      previous.Stop();
    rotation.Angle(from);
    if (!winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) {
      rotation.Angle(target);
      return;
    }
    A::DoubleAnimationUsingKeyFrames turn;
    A::SplineDoubleKeyFrame key;
    key.Value(target);
    key.KeyTime(A::KeyTimeHelper::FromTimeSpan(std::chrono::milliseconds(milliseconds)));
    A::KeySpline spline;
    spline.ControlPoint1({.1f, .9f});
    spline.ControlPoint2({.2f, 1.f});
    key.KeySpline(spline);
    turn.KeyFrames().Append(key);
    A::Storyboard::SetTarget(turn, glyph);
    A::Storyboard::SetTargetProperty(turn, L"(UIElement.RenderTransform).(RotateTransform.Angle)");
    previous = A::Storyboard();
    previous.Children().Append(turn);
    previous.Begin();
  }
  static double PanelEase(double progress) {
    double low = 0, high = 1, t = progress;
    for (int i = 0; i < 20; i++) {
      t = (low + high) / 2;
      double x = 3 * (1 - t) * (1 - t) * t * .1 + 3 * (1 - t) * t * t * .2 + t * t * t;
      if (x < progress)
        low = t;
      else
        high = t;
    }
    return 3 * (1 - t) * (1 - t) * t * .9 + 3 * (1 - t) * t * t + t * t * t;
  }
  void Set(X::DependencyObject const &object, X::DependencyProperty const &property,
           IInspectable const &value) {
    journal_.Save(object, property);
    object.SetValue(property, value);
  }
  void TextLine(C::TextBlock const &text, double height) {
    Set(text, C::TextBlock::LineHeightProperty(), winrt::box_value(height));
    Set(text, C::TextBlock::LineStackingStrategyProperty(),
        winrt::box_value(X::LineStackingStrategy::BlockLineHeight));
    Set(text, C::TextBlock::TextLineBoundsProperty(), winrt::box_value(X::TextLineBounds::Full));
    Set(text, C::TextBlock::PaddingProperty(), winrt::box_value(X::Thickness{}));
    for (auto const &entry : textBaselines_)
      if (entry.first.get() == text)
        return;
    M::TransformGroup transforms;
    if (auto previous = text.RenderTransform())
      transforms.Children().Append(previous);
    M::TranslateTransform optical;
    transforms.Children().Append(optical);
    Set(text, X::UIElement::RenderTransformProperty(), transforms);
    textBaselines_.push_back({winrt::make_weak(text), optical});
  }
  void AlignTextBaselines() {
    auto root = page_.XamlRoot();
    double scale = root ? root.RasterizationScale() : 1.;
    // The canonical browser's zero-height inline probe measures a 15.2 DIP
    // baseline in its 14px body text at 125%. XAML BlockLineHeight scales its
    // baseline instead. Correct that measured optical difference on whole
    // physical pixels, keeping the line box and all hit targets unchanged.
    for (auto const &entry : textBaselines_)
      if (auto text = entry.first.get(); text && text.IsLoaded() && text.ActualHeight() > 0) {
        double target = 15.2 * text.FontSize() / 14.;
        double delta = std::round((target - text.BaselineOffset()) * scale) / scale;
        if (std::abs(entry.second.Y() - delta) > .001)
          entry.second.Y(delta);
      }
  }
  double HeadingBottom() const {
    auto root = page_.XamlRoot();
    double scale = root ? root.RasterizationScale() : 1.;
    return 9 - (std::abs(scale - 1.25) < .001 ? 1 / scale : 0.);
  }
  void NormalizeGroup(X::FrameworkElement start) {
    for (auto p = Parent(start); p && p != page_; p = Parent(p))
      if (auto grid = p.try_as<C::GridView>()) {
        Set(grid, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{}));
        break;
      }
  }
  void SliderGeometry(C::Slider const &slider) {
    SliderLook{journal_, revoke_, thinSliders_}.Geometry(slider);
    ThinSlider(slider);
  }
  // Glyphs centred on the row/button height. At 125% (40 px rows/buttons, 20 px
  // glyph) the native insets put dropdown rows 1 px high and closed OD/ID/SS
  // buttons 1 px low. Margins, not transforms, keep the glyphs on the pixel grid.
  static constexpr double listGlyphDrop = .8, buttonGlyphRaise = .8;
  std::set<void *> thinSliders_;
  // Thin look: SliderLook. VM rows keep the right edge on the badge column.
  void ThinSlider(C::Slider const &slider) {
    SliderLook{journal_, revoke_, thinSliders_, slider == master_}.Thin(
        slider, [this, slider](double half) {
          return slider == master_ ? X::Thickness{12 - half, 0, 12 - half, 0}
                                   : X::Thickness{12 - half, 0, -half, 0};
        });
  }
  void AlignRows() {
    auto root = page_.XamlRoot();
    double scale = root ? root.RasterizationScale() : 1.;
    unsigned visible = 0;
    for (uint32_t i = 0; i < mixerList_.Items().Size(); i++)
      if (auto container = mixerList_.ContainerFromIndex(i).try_as<C::ListViewItem>();
          container && container.Visibility() != X::Visibility::Collapsed) {
        // Quantize cumulative positions, not each 38 DIP row independently
        // (at 125% one step is 47.5 px: 47/48).
        double before = std::floor(visible * 38 * scale + 1e-6),
               after = std::floor((visible + 1) * 38 * scale + 1e-6);
        double height = (after - before) / scale;
        visible++;
        Set(container, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
        Set(container, X::FrameworkElement::HeightProperty(), winrt::box_value(height));
        Find(container, [&](auto const &e) {
          if (auto slider = e.template try_as<C::Slider>();
              slider && slider.Name() == L"AppVolumeLevel")
            if (auto row = Parent(slider).template try_as<C::Grid>())
              Set(row, X::FrameworkElement::HeightProperty(), winrt::box_value(height));
          return false;
        });
      }
  }
  void CompactRealized(std::shared_ptr<Menu> const &menu) {
    auto list = menu->list;
    if (!list)
      return;
    for (uint32_t i = 0; i < list.Items().Size(); i++) {
      auto item = list.ContainerFromIndex(i).try_as<C::ListViewItem>();
      // Explicit input containers can be prepared before Flyout.Opened and the
      // first visible frame; native data-backed menus still use realized rows.
      if (!item && menu == input_)
        item = list.Items().GetAt(i).try_as<C::ListViewItem>();
      if (!item)
        continue;
      Set(item, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
      Set(item, X::FrameworkElement::MinHeightProperty(), winrt::box_value(32.));
      Set(item, X::FrameworkElement::MarginProperty(),
          winrt::box_value(X::Thickness{0, 0, 0, i + 1 < list.Items().Size() ? 4. : 0.}));
      // A 1 px border is snapped to one physical pixel at this XamlRoot scale.
      // Include that transparent border in the content inset, not in the native
      // presenter's background inset: 5 DIP above, 6 below.
      auto root = list.XamlRoot();
      if (!root)
        root = menu->button.XamlRoot();
      double border = 1. / (root ? root.RasterizationScale() : 1.);
      // 7 instead of 11 with the button 4 DIP narrower on each side (20 DIP from
      // the panel edges, like the SV mute/settings plates): glyph axes kept.
      X::Thickness inset{7 + border, 5 + border, 7 + border, 6 + border};
      Set(item, C::Control::PaddingProperty(), winrt::box_value(inset));
      Set(item, C::Control::HorizontalContentAlignmentProperty(),
          winrt::box_value(X::HorizontalAlignment::Stretch));
      Set(item, C::Control::VerticalContentAlignmentProperty(),
          winrt::box_value(X::VerticalAlignment::Stretch));
      item.ApplyTemplate();
      Find(item, [&](auto const &e) {
        if (auto presenter = e.template try_as<P::ListViewItemPresenter>()) {
          // Height on ListViewItem does not override a minimum on its presenter.
          // Normalize the actual template root as well, retaining its native states.
          Set(presenter, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
          Set(presenter, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
          Set(presenter, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
          Set(presenter, C::ContentPresenter::PaddingProperty(), winrt::box_value(X::Thickness{}));
          Set(presenter, C::ContentPresenter::BorderThicknessProperty(),
              winrt::box_value(X::Thickness{}));
          Set(presenter, C::ContentPresenter::HorizontalContentAlignmentProperty(),
              winrt::box_value(X::HorizontalAlignment::Stretch));
          Set(presenter, C::ContentPresenter::VerticalContentAlignmentProperty(),
              winrt::box_value(X::VerticalAlignment::Stretch));
          Set(presenter, P::ListViewItemPresenter::ContentMarginProperty(),
              winrt::box_value(inset));
          Set(presenter, P::ListViewItemPresenter::PointerOverBackgroundMarginProperty(),
              winrt::box_value(X::Thickness{}));
          Set(presenter, P::ListViewItemPresenter::SelectionIndicatorModeProperty(),
              winrt::box_value(P::ListViewItemPresenterSelectionIndicatorMode::Overlay));
          Set(presenter, P::ListViewItemPresenter::SelectionIndicatorVisualEnabledProperty(),
              winrt::box_value(true));
          // This shell template has separate Border margins, which survive the
          // presenter's PointerOverBackgroundMargin. Normalize those actual nodes.
          for (int childIndex = 0; childIndex < M::VisualTreeHelper::GetChildrenCount(presenter);
               childIndex++)
            if (auto border = M::VisualTreeHelper::GetChild(presenter, childIndex)
                                  .template try_as<C::Border>())
              Set(border, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
        }
        if (auto text = e.template try_as<C::TextBlock>()) {
          if (text.Name() == L"TitleText") {
            // Constrain the native DataTemplate root, rather than translating its
            // descendants. Keep all children, including the broadcast-audio command.
            if (auto content = Parent(text).template try_as<C::Grid>()) {
              Set(content, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
              Set(content, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
              Set(content, C::Grid::PaddingProperty(), winrt::box_value(X::Thickness{}));
              Set(content, X::FrameworkElement::VerticalAlignmentProperty(),
                  winrt::box_value(X::VerticalAlignment::Stretch));
            }
            TextLine(text, 19.);
            Set(text, C::TextBlock::TextTrimmingProperty(),
                winrt::box_value(X::TextTrimming::CharacterEllipsis));
            Set(text, C::TextBlock::TextWrappingProperty(),
                winrt::box_value(X::TextWrapping::NoWrap));
            Set(text, C::ToolTipService::ToolTipProperty(), nullptr);
            Set(text, X::FrameworkElement::MarginProperty(),
                winrt::box_value(X::Thickness{menu == input_ ? 8. : 0., 0, 0, 0}));
            Set(text, X::FrameworkElement::VerticalAlignmentProperty(),
                winrt::box_value(X::VerticalAlignment::Center));
          } else if (!wcscmp(X::Automation::AutomationProperties::GetName(menu->button).c_str(),
                             L"Output device") &&
                     text.Text().size() == 1 && Parent(text).template try_as<C::Grid>() &&
                     Parent(text).template as<C::Grid>().ColumnDefinitions().Size() == 3) {
            Set(text, X::FrameworkElement::WidthProperty(), winrt::box_value(16.));
            Set(text, X::FrameworkElement::HeightProperty(), winrt::box_value(16.));
            Set(text, X::FrameworkElement::MarginProperty(),
                winrt::box_value(X::Thickness{0, 0, 8, 0}));
            Set(text, X::FrameworkElement::VerticalAlignmentProperty(),
                winrt::box_value(X::VerticalAlignment::Center));
            // Keep the native endpoint font glyph and its original binding.
            // No Viewbox scaling or guessed device-type replacement.
            auto parent = Parent(text).template as<C::Grid>();
            Set(parent.ColumnDefinitions().GetAt(0), C::ColumnDefinition::WidthProperty(),
                winrt::box_value(X::GridLengthHelper::FromPixels(24)));
            if (!Named(parent, L"NativeSoundEndpointFontIcon")) {
              auto glyph = SymbolGlyph(page_, text.Text()[0]);
              glyph.Name(L"NativeSoundEndpointFontIcon");
              glyph.FontFamily(text.FontFamily());
              glyph.Margin({0, listGlyphDrop, 8, -listGlyphDrop});
              D::Binding binding;
              binding.Source(text);
              binding.Path(X::PropertyPath(L"Text"));
              glyph.SetBinding(C::FontIcon::GlyphProperty(), binding);
              Add(parent, glyph);
              Set(text, X::UIElement::OpacityProperty(), winrt::box_value(0.));
            }
            if (item.IsSelected() && menu->endpointIcon)
              menu->endpointIcon.Glyph(text.Text());
          }
        }
        return false;
      });
    }
  }
  void PreparePicker(std::shared_ptr<Menu> const &menu) noexcept {
    try {
      CompactRealized(menu);
    } catch (...) {
      auto error = winrt::to_hresult();
      Wh_Log(L"Picker preparation: %08X", error);
    }
  }
  IInspectable RowModel(C::Slider const &slider) {
    for (auto e = Parent(slider); e && e != mixerList_; e = Parent(e))
      if (auto container = e.try_as<C::ListViewItem>()) {
        if (auto item = mixerList_.ItemFromContainer(container);
            item && item != X::DependencyProperty::UnsetValue())
          return item;
        break;
      }
    return VolumeModel(slider);
  }
  IInspectable ReadModel(IInspectable const &object, PCWSTR name) {
    return ModelValue(object, name);
  }
  bool WriteVolume(C::Slider const &slider, IInspectable const &object, double number) {
    try {
      if (SetNumber(object, L"Volume", number))
        return true;
    } catch (...) {
      Wh_Log(L"Model setter: %08X", winrt::to_hresult());
    }
    try {
      if (SendBoundNumber(slider, object, number))
        return true;
    } catch (...) {
      Wh_Log(L"Binding relay: %08X", winrt::to_hresult());
    }
    return false;
  }
  void ResizeNative() {
    if (!frame_ || !footerBorder_)
      return;
    auto viewport = Named(page_, L"ListContent").try_as<C::ScrollViewer>();
    if (!viewport)
      return;
    auto body = viewport.Content().try_as<X::FrameworkElement>();
    auto header = Named(page_, L"PageHeader");
    if (!body || !header)
      return;
    auto width = float(std::max(1., page_.ActualWidth()));
    body.Measure({width, INFINITY});
    footerBorder_.Measure({width, INFINITY});
    auto xamlRoot = page_.XamlRoot();
    double scale = xamlRoot ? xamlRoot.RasterizationScale() : 1.;
    MONITORINFO monitor{sizeof(monitor)};
    double cap = GetMonitorInfoW(MonitorFromWindow(host_, MONITOR_DEFAULTTONEAREST), &monitor)
                     ? (monitor.rcWork.bottom - monitor.rcWork.top) / scale - 24
                     : (xamlRoot ? xamlRoot.Size().Height - 24 : page_.ActualHeight());
    double content =
        header.ActualHeight() + body.DesiredSize().Height + footerBorder_.DesiredSize().Height;
    wantedHeight_ = std::min(std::floor(cap * scale) / scale, std::ceil(content * scale) / scale);
    auto target = TargetHeight();
    if (target > 0 &&
        (!std::isfinite(frame_.Height()) || std::abs(frame_.Height() - target) > .25)) {
      *ownedHeight_ = target;
      frame_.Height(target);
    }
  }
  // No custom value tooltip: the native Slider thumb tooltip is the only one
  // (IsThumbToolTipEnabled), as in the stock shell.
  void Add(C::Panel const &parent, X::UIElement const &child, uint32_t index = UINT_MAX) {
    parent.Children().InsertAt(std::min(index, parent.Children().Size()), child);
    journal_.undo.push_back([parent, child] {
      uint32_t i;
      if (parent.Children().IndexOf(child, i))
        parent.Children().RemoveAt(i);
    });
  }
  void Summary(std::shared_ptr<Menu> const &m) {
    auto selected = m->list.SelectedItem();
    std::wstring text;
    if (selected) {
      text = String(ModelValue(selected, L"EndpointFriendlyName"));
      if (text.empty())
        text = String(selected);
      if (text.empty())
        if (auto container = m->list.ContainerFromItem(selected).try_as<X::FrameworkElement>())
          if (auto title = Named(container, L"TitleText").try_as<C::TextBlock>())
            text = title.Text();
    }
    if (text.empty() && X::Automation::AutomationProperties::GetName(m->button) == L"Output device")
      for (auto const &device : latest_.output)
        if (device.current) {
          text = device.name;
          break;
        }
    if (m->endpointIcon &&
        X::Automation::AutomationProperties::GetName(m->button) == L"Output device") {
      auto code = String(ModelValue(selected, L"EndpointIcon"));
      if (code.size() == 1)
        m->endpointIcon.Glyph(code);
      else if (auto item = m->list.ContainerFromItem(selected).try_as<X::DependencyObject>())
        Find(item, [&](auto const &e) {
          if (auto glyph = e.template try_as<C::TextBlock>();
              glyph && glyph.Text().size() == 1 && C::Grid::GetColumn(glyph) == 0 &&
              Parent(glyph).template try_as<C::Grid>() &&
              Parent(glyph).template as<C::Grid>().ColumnDefinitions().Size() == 3) {
            m->endpointIcon.Glyph(glyph.Text());
            return true;
          }
          return false;
        });
    } // The native list can briefly lose its selection while the shell refreshes
    // endpoints; keep the last shown name instead of flashing a placeholder.
    if (text.empty())
      text = std::wstring(m->label.Text());
    m->label.Text(text.empty() ? L"—" : text);
    m->button.IsEnabled(m->list.IsEnabled() && m->list.Items().Size() > 0);
  }
  std::shared_ptr<Menu> Picker(X::FrameworkElement const &owner, PCWSTR name) {
    auto m = std::make_shared<Menu>();
    m->button = C::Button();
    m->button.Height(32);
    m->button.MinHeight(32);
    m->button.MinWidth(0);
    m->button.Padding({7, 0, 7, 0});
    m->button.Margin({20, 0, 20, 18});
    m->button.HorizontalAlignment(X::HorizontalAlignment::Stretch);
    m->button.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
    X::Automation::AutomationProperties::SetName(m->button, name);
    m->button.VerticalContentAlignment(X::VerticalAlignment::Center);
    auto g = Grid(L"17,*,12");
    if (!wcscmp(name, L"Spatial sound")) {
      auto phones = SymbolGlyph(owner, 0xe7f6, 16);
      phones.Margin({0, -buttonGlyphRaise, 0, buttonGlyphRaise});
      g.Children().Append(phones);
    } else {
      m->endpointIcon = SymbolGlyph(owner, !wcscmp(name, L"Input device") ? 0xe720 : 0xe7f5);
      m->endpointIcon.Margin({0, -buttonGlyphRaise, 0, buttonGlyphRaise});
      g.Children().Append(m->endpointIcon);
    }
    m->label = C::TextBlock();
    TextLine(m->label, 19.6);
    m->label.TextTrimming(X::TextTrimming::CharacterEllipsis);
    m->label.TextWrapping(X::TextWrapping::NoWrap);
    m->label.VerticalAlignment(X::VerticalAlignment::Center);
    m->label.Margin({8, 0, 8, 0});
    C::Grid::SetColumn(m->label, 1);
    g.Children().Append(m->label);
    m->arrow = SymbolGlyph(owner, 0xe70d, 12);
    C::Grid::SetColumn(m->arrow, 2);
    g.Children().Append(m->arrow);
    m->button.Content(g);
    m->flyout = C::Flyout();
    m->flyout.Placement(P::FlyoutPlacementMode::Bottom);
    m->button.Flyout(m->flyout);
    auto presenterStyle = X::Style(winrt::xaml_typename<C::FlyoutPresenter>());
    presenterStyle.Setters().Append(
        X::Setter(C::Control::PaddingProperty(), winrt::box_value(X::Thickness{4, 4, 4, 4})));
    presenterStyle.Setters().Append(
        X::Setter(X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.)));
    presenterStyle.Setters().Append(X::Setter(C::Control::CornerRadiusProperty(),
                                              winrt::box_value(X::CornerRadius{8, 8, 8, 8})));
    m->flyout.FlyoutPresenterStyle(presenterStyle);
    Keep(m->flyout.Opening(winrt::auto_revoke, [this, m](auto &&, auto &&) {
      m->open = true;
      if (m != input_)
        CaptureMixer();
      Rotate(m->arrow, m->arrowStory, 180, 220);
      for (auto const &other : menus_)
        if (other != m)
          other->flyout.Hide();
      if (m->list) {
        auto root = m->button.XamlRoot();
        double scale = root ? root.RasterizationScale() : 1.;
        m->list.Width(std::max(32., m->button.ActualWidth() - 8 - 2 / scale));
        m->list.MaxHeight(300);
        if (m == input_)
          PreparePicker(m);
      }
    }));
    Keep(m->flyout.Opened(winrt::auto_revoke, [this, m](auto &&, auto &&) { PreparePicker(m); }));
    Keep(m->flyout.Closed(winrt::auto_revoke, [m](auto &&, auto &&) {
      m->open = false;
      Rotate(m->arrow, m->arrowStory, 0, 220);
    }));
    menus_.push_back(m);
    return m;
  }
  void Compact(C::ListView const &list) {
    journal_.Save(list, C::ItemsControl::ItemContainerStyleProperty());
    auto style = X::Style(winrt::xaml_typename<C::ListViewItem>());
    auto nativeStyle = Resource(page_, L"DefaultListViewItemStyle").try_as<X::Style>();
    style.BasedOn(pickerBase_   ? pickerBase_
                  : nativeStyle ? nativeStyle
                                : list.ItemContainerStyle());
    style.Setters().Append(
        X::Setter(X::FrameworkElement::MinHeightProperty(), winrt::box_value(32.)));
    style.Setters().Append(X::Setter(X::FrameworkElement::HeightProperty(), winrt::box_value(32.)));
    style.Setters().Append(
        X::Setter(C::Control::PaddingProperty(), winrt::box_value(X::Thickness{7, 0, 7, 0})));
    style.Setters().Append(X::Setter(C::Control::HorizontalContentAlignmentProperty(),
                                     winrt::box_value(X::HorizontalAlignment::Stretch)));
    style.Setters().Append(X::Setter(C::Control::VerticalContentAlignmentProperty(),
                                     winrt::box_value(X::VerticalAlignment::Center)));
    list.ItemContainerStyle(style);
    Set(list, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{}));
    Set(list, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
    Set(list, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
    Set(list, C::ScrollViewer::HorizontalScrollModeProperty(),
        winrt::box_value(C::ScrollMode::Disabled));
    Set(list, C::ScrollViewer::HorizontalScrollBarVisibilityProperty(),
        winrt::box_value(C::ScrollBarVisibility::Disabled));
    Set(list, C::ScrollViewer::VerticalScrollModeProperty(),
        winrt::box_value(C::ScrollMode::Enabled));
    Set(list, C::ScrollViewer::VerticalScrollBarVisibilityProperty(),
        winrt::box_value(C::ScrollBarVisibility::Auto));
  }
  void ConvertList(C::ListView const &list, PCWSTR title) {
    auto parent = Parent(list).as<C::Panel>();
    uint32_t index = 0;
    if (!parent.Children().IndexOf(list, index))
      throw winrt::hresult_error(E_UNEXPECTED);
    auto m = Picker(parent, title);
    m->list = list;
    journal_.Save(list, X::FrameworkElement::DataContextProperty());
    list.DataContext(list.DataContext());
    journal_.Save(list, X::FrameworkElement::WidthProperty());
    journal_.Save(list, X::FrameworkElement::MaxHeightProperty());
    Compact(list);
    parent.Children().RemoveAt(index);
    m->flyout.Content(list);
    journal_.undo.push_back([parent, list, index, m] {
      m->flyout.Hide();
      m->flyout.Content(nullptr);
      parent.Children().InsertAt(std::min(index, parent.Children().Size()), list);
    });
    Add(parent, m->button, index);
    Keep(list.SelectionChanged(winrt::auto_revoke, [this, m](auto &&, auto &&) {
      Summary(m);
      if (m->open)
        m->flyout.Hide();
    }));
    Summary(m);
  }
  void Input(C::Panel const &parent) {
    inputBlock_ = C::StackPanel();
    inputBlock_.BorderThickness({0, 1, 0, 0});
    PaintDivider(inputBlock_, parent);
    C::TextBlock heading;
    heading.Text(L"Input device");
    heading.FontWeight({600});
    TextLine(heading, 19.6);
    heading.Margin({20, 10, 20, HeadingBottom()});
    inputBlock_.Children().Append(heading);
    input_ = Picker(parent, L"Input device");
    input_->list = C::ListView();
    input_->list.SelectionMode(C::ListViewSelectionMode::Single);
    input_->list.IsItemClickEnabled(true);
    input_->list.HorizontalAlignment(X::HorizontalAlignment::Stretch);
    C::ScrollViewer::SetHorizontalScrollBarVisibility(input_->list,
                                                      C::ScrollBarVisibility::Disabled);
    Compact(input_->list);
    input_->flyout.Content(input_->list);
    inputBlock_.Children().Append(input_->button);
    Add(parent, inputBlock_);
    // Keep the native flyout animation; newly created input rows must not run
    // independent entrance/reposition transitions inside that moving surface.
    input_->list.ItemContainerTransitions(X::Media::Animation::TransitionCollection());
    Keep(input_->list.ItemClick(
        winrt::auto_revoke,
        [this](auto &&, C::ItemClickEventArgs const &args) { ChooseInput(args); }));
  }
  void ChooseInput(C::ItemClickEventArgs const &args) noexcept {
    try {
      auto clicked = args.ClickedItem();
      std::wstring id;
      // With explicit ListViewItems, ClickedItem can be their content Grid.
      // Resolve only the clicked object/container, never the previous selection.
      if (auto element = clicked.try_as<X::FrameworkElement>()) {
        id = String(element.Tag());
        if (id.empty())
          if (auto container = input_->list.ContainerFromItem(clicked).try_as<C::ListViewItem>())
            id = String(container.Tag());
        for (auto node = element; id.empty() && node && node != input_->list; node = Parent(node))
          if (auto item = node.try_as<C::ListViewItem>())
            id = String(item.Tag());
      }
      if (id.empty())
        throw winrt::hresult_error(E_INVALIDARG, L"Clicked microphone has no endpoint ID");
      pendingInputId_ = id;
      pendingInput_ = audio_.Send({sound::Action::Input, id});
      input_->flyout.Hide();
    } catch (...) {
      auto error = winrt::to_hresult();
      Wh_Log(L"Input selection: %08X", error);
    }
  }
  void SyncInput(sound::Snapshot const &state) {
    // Selection changes do not change the collection. Replacing explicit XAML
    // containers during ItemClick/closing can invalidate the routed input path.
    std::wstring signature;
    for (auto const &device : state.input)
      signature += std::to_wstring(device.id.size()) + L":" + device.id +
                   std::to_wstring(device.name.size()) + L":" + device.name;
    if (signature != inputSignature_ && !input_->open) {
      input_->list.Items().Clear();
      for (auto const &device : state.input) {
        C::ListViewItem item;
        item.Style(input_->list.ItemContainerStyle());
        item.Tag(winrt::box_value(device.id));
        item.ContentTransitions(X::Media::Animation::TransitionCollection());
        item.Transitions(X::Media::Animation::TransitionCollection());
        auto grid = Grid(L"16,*");
        grid.Tag(winrt::box_value(device.id));
        {
          auto mic = SymbolGlyph(page_, 0xe720);
          mic.Margin({0, listGlyphDrop, 0, -listGlyphDrop});
          grid.Children().Append(mic);
        }
        C::TextBlock label;
        label.Name(L"TitleText");
        label.Text(device.name);
        label.TextTrimming(X::TextTrimming::CharacterEllipsis);
        label.TextWrapping(X::TextWrapping::NoWrap);
        label.VerticalAlignment(X::VerticalAlignment::Center);
        label.FontSize(14);
        TextLine(label, 19);
        label.Margin({8, 0, 0, 0});
        C::Grid::SetColumn(label, 1);
        grid.Children().Append(label);
        item.Content(grid);
        input_->list.Items().Append(item);
      }
      CompactRealized(input_);
      inputSignature_ = signature;
    }
    C::ListViewItem selected{nullptr};
    for (auto const &device : state.input)
      if (device.current) {
        input_->label.Text(device.name);
        for (auto const &entry : input_->list.Items())
          if (auto item = entry.try_as<C::ListViewItem>();
              item && String(item.Tag()) == device.id) {
            selected = item;
            break;
          }
        break;
      }
    if (selected && input_->list.SelectedItem() != selected)
      input_->list.SelectedItem(selected);
    input_->button.IsEnabled(!state.input.empty());
    if (pendingInput_ && state.applied >= pendingInput_) {
      pendingInput_ = 0;
    }
  }
  void Mixer() {
    auto title = Named(page_, L"MixerGroupTitle").as<C::TextBlock>();
    auto header = Parent(title).as<C::Panel>();
    uint32_t index;
    if (!header.Children().IndexOf(title, index))
      throw winrt::hresult_error(E_UNEXPECTED);
    if (auto block = Parent(header).try_as<C::StackPanel>())
      Set(block, C::StackPanel::PaddingProperty(), winrt::box_value(X::Thickness{0, 0, 0, 11}));
    // IconButtonStyle supplies a symbol font. Freeze the native text typography
    // while the heading is still in its original parent, before reparenting it.
    auto titleFont = title.FontFamily();
    auto titleSize = title.FontSize();
    Set(title, C::TextBlock::FontFamilyProperty(), titleFont);
    Set(title, C::TextBlock::FontSizeProperty(), winrt::box_value(titleSize));
    TextLine(title, 19.6);
    mixerButton_ = C::Button();
    mixerButton_.MinWidth(0);
    mixerButton_.MinHeight(32);
    mixerButton_.Height(32);
    mixerButton_.Padding({8, 0, 8, 0});
    mixerButton_.Margin({12, 11, 52, 11});
    mixerButton_.HorizontalAlignment(X::HorizontalAlignment::Left);
    mixerButton_.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
    if (auto style = Resource(page_, L"IconButtonStyle").try_as<X::Style>())
      mixerButton_.Style(style);
    mixerButton_.FontFamily(titleFont);
    mixerButton_.FontSize(titleSize);
    mixerButton_.Width(NAN);
    mixerButton_.Height(32);
    mixerButton_.MinHeight(32);
    mixerButton_.MinWidth(0);
    mixerButton_.Padding({8, 0, 8, 0});
    mixerButton_.Margin({12, 11, 52, 0});
    mixerButton_.VerticalContentAlignment(X::VerticalAlignment::Center);
    mixerButton_.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
    journal_.Save(title, X::FrameworkElement::MarginProperty());
    title.Margin({0, 0, 8, 0}); // the chevron 8 DIP after the title; the plate hugs both
    auto g = Grid(L"Auto,12");
    header.Children().RemoveAt(index);
    g.Children().Append(title);
    arrow_ = SymbolGlyph(page_, 0xe70d, 12);
    C::Grid::SetColumn(arrow_, 1);
    g.Children().Append(arrow_);
    mixerButton_.Content(g);
    journal_.undo.push_back([header, title, index, g] {
      uint32_t i;
      if (g.Children().IndexOf(title, i))
        g.Children().RemoveAt(i);
      header.Children().InsertAt(std::min(index, header.Children().Size()), title);
    });
    Add(header, mixerButton_, index);
    if (auto settings = Find(header, [](auto const &e) {
                          auto button = e.template try_as<C::Button>();
                          return button &&
                                 String(button.CommandParameter()) == L"ms-settings:apps-volume";
                        }).try_as<C::Button>()) {
      journal_.Save(settings, X::FrameworkElement::StyleProperty());
      if (auto iconStyle = Resource(page_, L"IconButtonStyle").try_as<X::Style>())
        settings.Style(iconStyle);
      for (auto const &property :
           {X::FrameworkElement::WidthProperty(), X::FrameworkElement::HeightProperty(),
            X::FrameworkElement::MinWidthProperty(), X::FrameworkElement::MinHeightProperty(),
            X::FrameworkElement::MarginProperty(), C::Control::PaddingProperty(),
            C::ContentControl::ContentProperty()})
        journal_.Save(settings, property);
      settings.Width(32);
      settings.Height(32);
      settings.MinWidth(0);
      settings.MinHeight(0);
      settings.Padding({0, 0, 0, 0});
      settings.Margin({0, 11, 20, 0});
      settings.Content(Icon(L"sliders-horizontal"));
    }
    journal_.Save(mixerList_, X::UIElement::VisibilityProperty());
    journal_.Save(mixerList_, X::FrameworkElement::HeightProperty());
    journal_.Save(mixerList_, X::UIElement::OpacityProperty());
    // The VM's 220ms expand/collapse animation owns the whole list. Individual
    // entrance/reposition transitions would continue after the preflight pass.
    Set(mixerList_, C::ItemsControl::ItemContainerTransitionsProperty(),
        X::Media::Animation::TransitionCollection());
    // First Sound page in this shell process opens with VM collapsed: the shell
    // loads mixer rows last and asynchronously. The saved state is not changed
    // and applies from the next opening; a click saves the new state as usual.
    expanded_ = soundOpenedOnce && Wh_GetIntValue(L"nativeMixerExpanded", 1) != 0;
    soundOpenedOnce = true;
    ApplyExpanded(false);
    Keep(mixerButton_.Click(winrt::auto_revoke, [this](auto &&, auto &&) {
      expanded_ = !expanded_;
      Wh_SetIntValue(L"nativeMixerExpanded", expanded_);
      ApplyExpanded(true);
    }));
    journal_.Save(mixerList_, C::ItemsControl::ItemContainerStyleProperty());
    auto itemStyle = X::Style(winrt::xaml_typename<C::ListViewItem>());
    itemStyle.BasedOn(mixerList_.ItemContainerStyle());
    itemStyle.Setters().Append(
        X::Setter(C::Control::PaddingProperty(), winrt::box_value(X::Thickness{})));
    itemStyle.Setters().Append(
        X::Setter(X::FrameworkElement::MinHeightProperty(), winrt::box_value(38.)));
    itemStyle.Setters().Append(
        X::Setter(X::FrameworkElement::HeightProperty(), winrt::box_value(38.)));
    itemStyle.Setters().Append(X::Setter(C::Control::HorizontalContentAlignmentProperty(),
                                         winrt::box_value(X::HorizontalAlignment::Stretch)));
    mixerList_.ItemContainerStyle(itemStyle);
    Set(mixerList_, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{}));
    Set(mixerList_, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
  }
  void ApplyExpanded(bool animate) {
    if (holding_) {
      holding_ = false;
      if (holdTimer_)
        holdTimer_.Stop();
    }
    holdAnimation_ = false;
    ClearSnapshot();
    if (animation_)
      animation_.Stop();
    auto rotation = arrow_.RenderTransform().try_as<M::RotateTransform>();
    if (!rotation) {
      rotation = M::RotateTransform();
      arrow_.RenderTransformOrigin({.5f, .5f});
      arrow_.RenderTransform(rotation);
    }
    arrowStart_ = rotation.Angle();
    X::Automation::AutomationProperties::SetHelpText(mixerButton_,
                                                     expanded_ ? L"Expanded" : L"Collapsed");
    if (!animate || !winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) {
      hideUnnamed_ = false;
      rotation.Angle(expanded_ ? 180 : 0);
      mixerList_.Height(NAN);
      mixerList_.Opacity(1);
      mixerList_.Visibility(expanded_ ? X::Visibility::Visible : X::Visibility::Collapsed);
      return;
    }
    hideUnnamed_ = expanded_;
    from_ = mixerList_.Visibility() == X::Visibility::Visible ? mixerList_.ActualHeight() : 0;
    mixerList_.Visibility(X::Visibility::Visible);
    mixerList_.Height(NAN);
    mixerList_.Measure({float(std::max(1., mixerList_.ActualWidth())), INFINITY});
    // Rows realized by this measure (first expansion after a collapsed opening)
    // get hidden apps and 38 DIP heights before the target height is taken.
    if (expanded_)
      try {
        PrepareMixerRows();
        AlignTextBaselines();
        mixerList_.Measure({float(std::max(1., mixerList_.ActualWidth())), INFINITY});
      } catch (...) {
        Wh_Log(L"ExpandPrepareError %08X", winrt::to_hresult());
      }
    to_ = expanded_ ? mixerList_.DesiredSize().Height : 0;
    mixerList_.Height(from_);
    start_ = NowMs();
    animFirst_ = true;
    EnsureAnimation();
    animation_.Start();
    StartAnimationLoop();
  }
  static double NowMs() {
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return double(c.QuadPart) * 1000. / double(f.QuadPart);
  }
  std::function<void()> animationFrame_;
  bool animFirst_ = false, renderingOn_ = false;
  double frameStart_ = NAN;
  winrt::event_token renderingToken_{};
  void StartAnimationLoop() {
    if (renderingOn_)
      return;
    renderingOn_ = true;
    renderingToken_ = M::CompositionTarget::Rendering([this](auto &&, auto &&) {
      try {
        if (!animation_ || !animation_.IsEnabled()) {
          StopAnimationLoop();
          return;
        }
        if (animationFrame_)
          animationFrame_();
      } catch (...) {
        Wh_Log(L"AnimationFrameError %08X", winrt::to_hresult());
      }
    });
  }
  void StopAnimationLoop() {
    if (!renderingOn_)
      return;
    renderingOn_ = false;
    try {
      M::CompositionTarget::Rendering(renderingToken_);
    } catch (...) {
    }
  }
  void EnsureAnimation() {
    if (animation_)
      return;
    // The timer only marks the animation as running; frames come from
    // CompositionTarget.Rendering (one per rendered frame instead of a 16 ms
    // DispatcherTimer beating against vsync), time from QPC (GetTickCount64
    // moves in ~16 ms steps). The panel height and the list's natural height are
    // measured every frame: the first natural height is measured before the rows
    // are re-styled, so a height derived once overshoots.
    animation_ = X::DispatcherTimer();
    animation_.Interval(std::chrono::hours(1));
    animationTick_ = animation_.Tick([](auto &&, auto &&) {});
    animationFrame_ = [this] {
      auto t = std::min(1., (NowMs() - start_) / 220.);
      auto eased = PanelEase(t);
      if (animFirst_) {
        animFirst_ = false;
        frameStart_ = frame_ && std::isfinite(frame_.Height()) ? frame_.Height() : wantedHeight_;
      }
      // The shell may still add/reorder sessions while the list expands:
      // prepare new rows and re-measure the natural target on every tick.
      // A rebuild seen by the timer before LayoutUpdated must still be held.
      if (holdAnimation_ && expanded_ && t < 1)
        try {
          if (IsRebuild(rowItems_, RowItems())) {
            StartHold();
            return;
          }
        } catch (...) {
        }
      if (expanded_ && t < 1)
        try {
          if (rowsDirty_ || RowSignature() != rowSignature_) {
            PrepareMixerRows();
            AlignTextBaselines();
          }
          mixerList_.Height(NAN);
          mixerList_.Measure({float(std::max(1., mixerList_.ActualWidth())), INFINITY});
          to_ = mixerList_.DesiredSize().Height;
        } catch (...) {
        }
      mixerList_.Height(from_ + (to_ - from_) * eased);
      arrow_.RenderTransform().as<M::RotateTransform>().Angle(
          arrowStart_ + ((expanded_ ? 180. : 0.) - arrowStart_) * eased);
      if (crossfade_) {
        auto f = std::min(1., t * 220 / 160);
        mixerList_.Opacity(1);
        if (itemsHost_)
          itemsHost_.Opacity(f);
        if (snapshotBrush_)
          snapshotBrush_.Opacity(1 - f);
      } else
        mixerList_.Opacity(expanded_ ? std::min(1., t * 220 / 160)
                                     : std::max(0., 1 - t * 220 / 160));
      if (t >= 1) {
        animation_.Stop();
        hideUnnamed_ = false;
        holdAnimation_ = false;
        ClearSnapshot();
        ScheduleRowsRecheck();
        mixerList_.Height(NAN);
        mixerList_.Opacity(1);
        mixerList_.Visibility(expanded_ ? X::Visibility::Visible : X::Visibility::Collapsed);
      }
      ResizeNative();
    };
  }
  void PrepareMixerRows() {
    SliderGeometry(master_);
    auto style = Resource(page_, L"HeaderIconSliderStyle").try_as<X::Style>();
    std::vector<C::Slider> found;
    Find(mixerList_, [&](auto const &e) {
      if (auto s = e.template try_as<C::Slider>(); s && s.Name() == L"AppVolumeLevel")
        found.push_back(s);
      return false;
    });
    for (auto const &s : found)
      App(s, style);
    bool pending = false;
    for (auto const &row : rows_)
      if (auto root = row->root.get()) {
        auto slider = row->slider.get();
        if (slider && RowModel(slider) != row->model) {
          row->model = RowModel(slider);
          row->last = slider.Value() > 0 ? slider.Value() : 50;
          row->beforeValid = false;
          row->expected = NAN;
          row->unnamedSince = 0;
        }
        // AppName once per row model (it was read on every pass, dozens of times per
        // opening). Re-read only while the binding has not resolved it yet.
        std::wstring name;
        if (row->nameModel == row->model && !row->name.empty())
          name = row->name;
        else {
          name = String(ReadModel(row->model, L"AppName"));
          row->name = name;
          row->nameModel = row->model;
        }
        if (name.empty())
          name = root.Name();
        // A new row whose binding has not resolved yet cannot be matched against
        // hiddenApps; keep it transparent (max 500 ms) instead of showing it.
        bool unnamed = name.empty();
        if (unnamed) {
          auto now = GetTickCount64();
          if (!row->unnamedSince)
            row->unnamedSince = now;
          if (now - row->unnamedSince < 500)
            pending = true;
          else
            unnamed = false;
        } else
          row->unnamedSince = 0;
        X::FrameworkElement container = root;
        for (auto p = Parent(root); p && p != mixerList_; p = Parent(p))
          if (p.try_as<C::ListViewItem>()) {
            container = p;
            break;
          }
        if (auto item = container.try_as<C::ListViewItem>()) {
          Set(item, X::UIElement::TransitionsProperty(),
              X::Media::Animation::TransitionCollection());
          Set(item, C::ContentControl::ContentTransitionsProperty(),
              X::Media::Animation::TransitionCollection());
        }
        journal_.Save(container, X::UIElement::VisibilityProperty());
        container.Visibility(sound::Hidden(hidden, name, L"") ? X::Visibility::Collapsed
                                                              : X::Visibility::Visible);
        journal_.Save(container, X::UIElement::OpacityProperty());
        container.Opacity(unnamed && hideUnnamed_ ? 0 : 1);
        if (slider && std::isfinite(row->expected) && GetTickCount64() >= row->verifyAt) {
          row->expected = NAN;
        }
      }
    AlignRows();
    rowsDirty_ = false;
    rowSignature_ = RowSignature();
    rowItems_ = RowItems();
    pendingRows_ = pending;
    // Unresolved rows are re-checked by a timer on the next frame. Keeping
    // rowsDirty_ set made every LayoutUpdated re-layout: XAML layout cycle,
    // ShellHost 0xc000027b in Windows.UI.Xaml.dll when Spatial sound changed.
    if (pending)
      ScheduleRowsRecheck();
  }
  double PanelCap() const {
    auto root = page_.XamlRoot();
    double scale = root ? root.RasterizationScale() : 1.;
    MONITORINFO monitor{sizeof(monitor)};
    double cap = GetMonitorInfoW(MonitorFromWindow(host_, MONITOR_DEFAULTTONEAREST), &monitor)
                     ? (monitor.rcWork.bottom - monitor.rcWork.top) / scale - 24
                     : (root ? root.Size().Height - 24 : page_.ActualHeight());
    return std::floor(cap * scale) / scale;
  }
  void SetPanelHeight(double value) {
    if (frame_ && value > 0) {
      *ownedHeight_ = value;
      frame_.Height(value);
    }
  }
  void Master(C::ItemsControl const &oldFooter, X::Style const &sliderStyle) {
    // Keep the actual Footer and its native group/border visible. The shell uses
    // that slot when measuring the advanced page; a sibling is not equivalent.
    footerBorder_ = Find(oldFooter, [](auto const &e) {
                      auto border = e.template try_as<C::Border>();
                      return border && border.BorderThickness().Top > 0;
                    }).as<C::Border>();
    // The SV/VM divider in the colour of the Quick Settings block divider.
    {
      journal_.Save(footerBorder_, C::Border::BorderBrushProperty());
      journal_.Save(footerBorder_, C::Border::BorderThicknessProperty());
      // 1 DIP like the other dividers (one pixel at 125%).
      footerBorder_.BorderThickness({0, 1, 0, 0});
      FooterLine();
      Keep(footerBorder_.ActualThemeChanged(winrt::auto_revoke, [this](auto &&, auto &&) {
        try {
          if (footerBorder_)
            FooterLine();
          if (inputBlock_)
            PaintDivider(inputBlock_, page_);
        } catch (...) {
        }
      }));
    }
    auto original = footerBorder_.Child();
    footer_ = Grid(L"32,*,32");
    footer_.Name(L"NativeSoundMasterRow");
    FooterMargins();
    masterIcon_ = Button(page_, L"Mute system volume");
    masterIcon_.Content(VolumeGlyph(page_, 3));
    try {
      masterAnimation_ = std::make_shared<AnimatedVolume>();
      masterIcon_.Content(masterAnimation_->Isolated());
    } catch (...) {
      Wh_Log(L"MasterAnimationError %08X", winrt::to_hresult());
    }
    footer_.Children().Append(masterIcon_);
    master_ = C::Slider();
    master_.Style(sliderStyle);
    master_.MinWidth(0);
    master_.MinHeight(0);
    master_.Height(32);
    master_.Margin({12, 0, 12, 0});
    master_.Maximum(100);
    master_.StepFrequency(1);
    master_.IsThumbToolTipEnabled(true);
    C::Grid::SetColumn(master_, 1);
    footer_.Children().Append(master_);
    X::Automation::AutomationProperties::SetName(master_, L"System volume");
    auto settings = Button(page_, L"More volume settings");
    SettingsGear(settings);
    C::Grid::SetColumn(settings, 2);
    footer_.Children().Append(settings);
    auto nativeBorder = footerBorder_;
    nativeBorder.Child(footer_);
    journal_.undo.push_back([nativeBorder, original] { nativeBorder.Child(original); });
    Keep(settings.Click(winrt::auto_revoke, [](auto &&, auto &&) {
      ShellExecuteW(nullptr, L"open", L"ms-settings:sound", nullptr, nullptr, SW_SHOWNORMAL);
    }));
    Keep(master_.ValueChanged(winrt::auto_revoke, [this](auto &&, auto const &args) {
      if (!sync_) {
        userChangeAt_ = GetTickCount64();
        auto value = float(args.NewValue() / 100);
        pending_ = audio_.Send({sound::Action::MasterVolume, L"", value});
        MasterGlyph(value, value <= 0);
      }
    }));
    Keep(master_.PointerWheelChanged(
        winrt::auto_revoke, [this](auto &&, X::Input::PointerRoutedEventArgs const &args) {
          try {
            if (args.Handled() || !master_.IsEnabled())
              return;
            auto point = args.GetCurrentPoint(master_);
            if (point.Properties().IsHorizontalMouseWheel())
              return;
            double delta = point.Properties().MouseWheelDelta() / 120.;
            if (!delta)
              return;
            master_.Value(std::clamp(master_.Value() + delta * master_.SmallChange(),
                                     master_.Minimum(), master_.Maximum()));
            args.Handled(true);
          } catch (...) {
            Wh_Log(L"MasterWheelError %08X", winrt::to_hresult());
          }
        }));
    Keep(masterIcon_.Click(winrt::auto_revoke, [this](auto &&, auto &&) {
      pending_ = audio_.Send({sound::Action::MasterMute});
    }));
  }

public:
  explicit SoundPage(X::FrameworkElement const &page, HWND host,
                     std::function<void()> volumeChanged)
      : page_(page), host_(host),
        volumeChanged_(std::move(volumeChanged)) {}
  void MasterGlyph(float value, bool muted) {
    if (masterAnimation_)
      try {
        masterAnimation_->Update(value, muted);
        return;
      } catch (...) {
        Wh_Log(L"MasterAnimationError %08X", winrt::to_hresult());
        masterAnimation_.reset();
        glyph_ = -1;
      }
    int glyph = sound::VolumeIcon(value, muted);
    if (glyph != glyph_) {
      glyph_ = glyph;
      masterIcon_.Content(VolumeGlyph(page_, glyph));
    }
  }
  void RefreshMasterNow() {
    if (master_ && page_.IsLoaded())
      ApplyMaster(State());
  }
  sound::Snapshot State() {
    auto s = audio_.Get();
    if (s.revision) {
      lastAudio = s;
      return s;
    }
    if (lastAudio.revision) {
      auto c = lastAudio;
      c.applied = s.applied;
      c.error = s.error;
      c.masterNoticeAt = 0;
      return c;
    }
    return s;
  }
  void ApplyMaster(sound::Snapshot const &state) {
    if (!master_ || state.applied < pending_)
      return;
    // While the user moves SV, the echo of an earlier value from Core Audio is
    // not written back into the slider (it pulled the thumb back between drag
    // steps).
    bool dragging = GetTickCount64() - userChangeAt_ < 400;
    sync_ = true;
    master_.IsEnabled(state.masterAvailable);
    masterIcon_.IsEnabled(state.masterAvailable);
    if (!dragging)
      master_.Value(state.muted ? 0 : std::round(state.master * 100));
    sync_ = false;
    if (!dragging)
      MasterGlyph(state.master, state.muted);
  }
  X::FrameworkElement Page() const { return page_; }
  bool Alive() const {
    return page_ && page_.IsLoaded() && outputAnchor_ && outputAnchor_.IsLoaded() &&
           Named(page_, L"OutputGroupTitle") == outputAnchor_;
  }
  bool Apply() {
    auto outputTitle = Named(page_, L"OutputGroupTitle");
    auto output = Named(page_, L"ListWithOutputGroupTitle").try_as<C::ListView>();
    auto spatialTitle = Named(page_, L"SpatialGroupTitle");
    auto mixer = Named(page_, L"MixerGroupTitle");
    auto oldFooter = Named(page_, L"Footer").try_as<C::ItemsControl>();
    auto style = Resource(page_, L"HeaderIconSliderStyle").try_as<X::Style>();
    if (!outputTitle || !output || !spatialTitle || !mixer || !oldFooter || !style)
      return false;
    auto spatial = Find(Parent(spatialTitle), [](auto const &e) {
                     return bool(e.template try_as<C::ListView>());
                   }).try_as<C::ListView>();
    auto mixerRoot = Parent(Parent(mixer));
    mixerList_ = Find(mixerRoot, [](auto const &e) {
                   return bool(e.template try_as<C::ListView>());
                 }).try_as<C::ListView>();
    if (!spatial || !mixerList_ || !Parent(output).try_as<C::StackPanel>() ||
        !Find(oldFooter, [](auto const &e) {
          auto border = e.template try_as<C::Border>();
          return border && border.BorderThickness().Top > 0;
        }))
      return false;
    outputAnchor_ = outputTitle;
    if (auto original = output.ItemContainerStyle())
      pickerBase_ = original.BasedOn();
    for (auto heading : {outputTitle, spatialTitle, mixer})
      NormalizeGroup(heading);
    for (auto name : {L"PageTitleText", L"PageTitle"})
      if (!pageTitle_)
        pageTitle_ = Named(page_, name).try_as<C::TextBlock>();
    for (auto heading : {outputTitle, spatialTitle}) {
      journal_.Save(heading, X::FrameworkElement::MarginProperty());
      heading.Margin({20, 10, 20, HeadingBottom()});
      TextLine(heading.as<C::TextBlock>(), 19.6);
    }
    ConvertList(output, L"Output device");
    Input(Parent(outputTitle).as<C::Panel>());
    ConvertList(spatial, L"Spatial sound");
    Mixer();
    Master(oldFooter, style);
    if (auto back = Named(page_, L"BackButton").try_as<C::Button>()) {
      Set(back, X::FrameworkElement::WidthProperty(), winrt::box_value(32.));
      Set(back, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
      Set(back, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
      Set(back, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
      Set(back, X::FrameworkElement::MarginProperty(),
          winrt::box_value(X::Thickness{16, 0, 12, 0}));
      // Hiding the page (ExitWithSnapshot): its command is wrapped; weak references only,
      // an abandoned page keeps the wrapper.
      if (auto original = back.Command()) {
        auto weakPage = winrt::make_weak(page_);
        Set(back, P::ButtonBase::CommandProperty(),
            winrt::make<DelayedCommand>(original, [weakPage, original](IInspectable const &p) {
              auto page = weakPage.get();
              X::FrameworkElement frame{nullptr};
              if (page)
                for (auto e = Parent(page); e; e = Parent(e))
                  if (e.Name() == L"L2Frame") {
                    frame = e;
                    break;
                  }
              if (!page || !frame) {
                original.Execute(p);
                return;
              }
              Leave(frame, page, original, p);
            }));
      } else {
        // BackButton has no Command (FullScreenPage.xaml: the shell handles Click
        // in code), and its handler runs before any added one, so the page would be
        // gone before a snapshot. A clone with the same style and glyph takes the
        // clicks; the original stays in place (transparent, no hit testing, not a
        // tab stop) and is clicked through UI Automation once the snapshot is laid
        // over the panel.
        if (auto parent = Parent(back).try_as<C::Panel>()) {
          C::Button clone;
          clone.Style(back.Style());
          clone.Width(back.Width());
          clone.Height(back.Height());
          clone.MinWidth(0);
          clone.MinHeight(0);
          clone.Margin(back.Margin());
          clone.Padding(back.Padding());
          clone.HorizontalAlignment(back.HorizontalAlignment());
          clone.VerticalAlignment(back.VerticalAlignment());
          C::Grid::SetColumn(clone, C::Grid::GetColumn(back));
          C::Grid::SetRow(clone, C::Grid::GetRow(back));
          if (auto icon = back.Content().try_as<C::FontIcon>()) {
            C::FontIcon copy;
            copy.Glyph(icon.Glyph());
            copy.FontSize(icon.FontSize());
            copy.FontFamily(icon.FontFamily());
            copy.MirroredWhenRightToLeft(icon.MirroredWhenRightToLeft());
            clone.Content(copy);
          }
          auto name = X::Automation::AutomationProperties::GetName(back);
          X::Automation::AutomationProperties::SetName(clone, name);
          if (auto tip = C::ToolTipService::GetToolTip(back))
            C::ToolTipService::SetToolTip(clone, tip);
          Set(back, X::UIElement::OpacityProperty(), winrt::box_value(0.));
          Set(back, X::UIElement::IsHitTestVisibleProperty(), winrt::box_value(false));
          Set(back, C::Control::IsTabStopProperty(), winrt::box_value(false));
          uint32_t index = 0;
          parent.Children().IndexOf(back, index);
          Add(parent, clone, index + 1);
          auto weakBack = winrt::make_weak(back);
          auto weakPage = winrt::make_weak(page_);
          Keep(clone.Click(winrt::auto_revoke, [weakBack, weakPage](auto &&, auto &&) {
            auto press = [weakBack] {
              try {
                if (auto b = weakBack.get())
                  if (auto peer = X::Automation::Peers::FrameworkElementAutomationPeer::
                          CreatePeerForElement(b))
                    if (auto invoke =
                            peer.GetPattern(X::Automation::Peers::PatternInterface::Invoke)
                                .try_as<X::Automation::Provider::IInvokeProvider>())
                      invoke.Invoke();
              } catch (...) {
                Wh_Log(L"BackInvokeError %08X", winrt::to_hresult());
              }
            };
            // L2Frame is looked up at click time: Apply finds frame_ only after this
            // handler is created.
            auto page = weakPage.get();
            X::FrameworkElement frame{nullptr};
            if (page)
              for (auto e = Parent(page); e; e = Parent(e))
                if (e.Name() == L"L2Frame") {
                  frame = e;
                  break;
                }
            if (page && frame &&
                winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) {
              ExitWithSnapshot(frame, page, press);
              return;
            }
            press();
          }));
        }
      }
    }
    for (auto name : {L"PageTitleText", L"PageTitle"})
      if (auto t = Named(page_, name).try_as<C::TextBlock>()) {
        journal_.Save(t, C::TextBlock::TextProperty());
        t.Text(L"Sound");
        TextLine(t, 19.6);
      }
    journal_.Save(page_, X::FrameworkElement::HeightProperty());
    page_.Height(NAN);
    for (auto e = Parent(page_); e; e = Parent(e))
      if (e.Name() == L"L2Frame") {
        frame_ = e;
        break;
      }
    if (frame_) {
      frameJournal_.Save(frame_, X::FrameworkElement::MinHeightProperty());
      frame_.MinHeight(0);
      auto property = X::FrameworkElement::HeightProperty();
      auto original = frame_.ReadLocalValue(property);
      D::BindingBase binding{nullptr};
      if (auto expression = frame_.GetBindingExpression(property))
        binding = expression.ParentBinding();
      auto frame = frame_;
      auto owned = ownedHeight_;
      frameJournal_.undo.push_back([frame, owned, original, property, binding] {
        if (std::isfinite(*owned) && std::abs(frame.Height() - *owned) < .25) {
          if (binding)
            D::BindingOperations::SetBinding(frame, property, binding);
          else if (original == X::DependencyProperty::UnsetValue())
            frame.ClearValue(property);
          else
            frame.SetValue(property, original);
        }
      });
    }
    audio_.Start(volumeChanged_);
    return true;
  }
  void PrepareInitialLayout() {
    if (preparing_ || !frame_)
      return;
    preparing_ = true;
    try {
      // The VM list keeps its saved Visibility during preparation: collapsing and
      // re-expanding it here forces extra layout passes and shows native rows with
      // zero volume/mute and no app icons. The other VM state is measured by
      // ApplyExpanded only when the user toggles it. No forced
      // page_.UpdateLayout() here: it costs hundreds of milliseconds. Rows,
      // baselines and Frame height are applied in the first LayoutUpdated, after
      // the shell's own layout pass and before that frame is rendered.
      ApplyExpanded(false);
      auto state = State();
      latest_ = state;
      SyncInput(state);
      ApplyMaster(state);
      for (auto const &menu : menus_)
        if (menu != input_)
          Summary(menu);
      rowsDirty_ = true;
      prepared_ = true;
      preparing_ = false;
      if (!DataReady(false))
        StartGate();
      RememberHeight();
      StartOpenHold();
      FooterMargins();
    } catch (...) {
      preparing_ = false;
      throw;
    }
  }
  // First opening in a shell session: the native view models load asynchronously
  // ("No output devices found", empty Spatial/VM, then the panel grows). Keep the
  // page content transparent until the data and VM rows are present, max 300 ms.
  std::shared_ptr<Menu> MenuNamed(PCWSTR name) {
    for (auto const &m : menus_)
      if (X::Automation::AutomationProperties::GetName(m->button) == name)
        return m;
    return nullptr;
  }
  bool DataReady(bool rows) {
    auto out = MenuNamed(L"Output device"), spatial = MenuNamed(L"Spatial sound");
    if (!out || !out->list.Items().Size() || !out->list.SelectedItem())
      return false;
    // The shell first selects a provisional endpoint (seen: Steam Streaming
    // Microphone with Spatial "Off"); wait until it is Core Audio's default.
    auto state = State();
    std::wstring current;
    for (auto const &d : state.output)
      if (d.current) {
        current = d.name;
        break;
      }
    if (current.empty())
      return false;
    if (auto name = String(ModelValue(out->list.SelectedItem(), L"EndpointFriendlyName"));
        !name.empty() && name != current)
      return false;
    if (spatial && !spatial->list.Items().Size())
      return false;
    if (!expanded_)
      return true;
    auto n = mixerList_.Items().Size();
    if (!n)
      return false;
    return !rows || (!rowsDirty_ && mixerList_.ContainerFromIndex(n - 1));
  }
  // While the content is hidden the empty surface keeps one height: the last
  // shown height for this VM state (persisted), instead of following the data.
  PCWSTR HeightKey() const {
    return expanded_ ? L"nativeLastHeightExpanded" : L"nativeLastHeightCollapsed";
  }
  double TargetHeight() const { return gating_ && heldHeight_ > 0 ? heldHeight_ : wantedHeight_; }
  void RememberHeight() {
    if (gating_ || wantedHeight_ <= 0 || (animation_ && animation_.IsEnabled()))
      return;
    int value = int(std::lround(wantedHeight_ * 100));
    int &saved = expanded_ ? savedExpanded_ : savedCollapsed_;
    if (saved != value) {
      saved = value;
      Wh_SetIntValue(HeightKey(), value);
    }
  }
  void StartGate() {
    heldHeight_ = Wh_GetIntValue(HeightKey(), 0) / 100.;
    if (heldHeight_ > 0)
      heldHeight_ = std::min(heldHeight_, PanelCap());
    gating_ = heldHeight_ > 0;
    if (gating_)
      SetPanelHeight(heldHeight_);
    gateJournal_.Save(page_, X::UIElement::OpacityProperty());
    page_.Opacity(0);
    gateStarted_ = GetTickCount64();
    gateStable_ = 0;
    gateLabels_.clear();
    if (!gate_) {
      gate_ = X::DispatcherTimer();
      gate_.Interval(std::chrono::milliseconds(16));
      gateTick_ = gate_.Tick([this](auto &&, auto &&) { GateTick(); });
    }
    gate_.Start();
  }
  void GateTick() {
    if (fading_) {
      auto f = std::min(1., (GetTickCount64() - gateStarted_) / 150.);
      try {
        page_.Opacity(PanelEase(f));
      } catch (...) {
        f = 1;
      }
      if (f >= 1) {
        fading_ = false;
        gate_.Stop();
        gateJournal_.Restore();
      }
      return;
    }
    try {
      if (rowsDirty_) {
        PrepareMixerRows();
        AlignTextBaselines();
        ResizeNative();
      }
      for (auto const &menu : menus_)
        if (menu != input_)
          Summary(menu);
      std::wstring labels;
      for (auto const &menu : menus_)
        labels += std::wstring(menu->label.Text()) + L"|";
      bool ready = DataReady(true);
      gateStable_ = ready && labels == gateLabels_ ? gateStable_ + 1 : ready ? 1 : 0;
      gateLabels_ = labels;
      // Three ticks (~48 ms) with the same labels: provisional selections and new
      // VM row bindings settle before the content is shown.
      if (gateStable_ >= 3 || GetTickCount64() - gateStarted_ >= 400)
        EndGate(ready);
    } catch (...) {
      Wh_Log(L"OpenGateError %08X", winrt::to_hresult());
      EndGate(false);
    }
  }
  void EndGate(bool ready) {
    if (gate_)
      gate_.Stop();
    gating_ = false;
    try {
      for (auto const &menu : menus_)
        if (menu != input_)
          Summary(menu);
      ResizeNative();
      RememberHeight();
    } catch (...) {
    }
    if (winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled() && gate_) {
      // 150 ms fade of the page content with the VM easing; the gate timer drives
      // it, the journal restores the native Opacity when it ends.
      fading_ = true;
      gateStarted_ = GetTickCount64();
      gate_.Start();
    } else
      gateJournal_.Restore();
  }
  // Containers and items of the VM list, in order. The shell keeps adding and
  // reordering sessions while the list first expands; the visual tree callback
  // for new rows arrives frames later, so compare on every layout instead.
  std::wstring RowSignature() {
    std::wstring s;
    auto items = mixerList_.Items();
    auto n = items.Size();
    for (uint32_t i = 0; i < n; i++) {
      s += std::to_wstring(
               reinterpret_cast<uintptr_t>(winrt::get_abi(mixerList_.ContainerFromIndex(i)))) +
           L":" + std::to_wstring(reinterpret_cast<uintptr_t>(winrt::get_abi(items.GetAt(i)))) +
           L";";
    }
    return s;
  }
  std::vector<void *> RowItems() {
    std::vector<void *> v;
    auto items = mixerList_.Items();
    for (uint32_t i = 0; i < items.Size(); i++)
      v.push_back(winrt::get_abi(items.GetAt(i)));
    return v;
  }
  // Spatial sound / output changes make the shell replace the whole session
  // collection in steps (seen: 2 unbound rows at 0 with mute, then 5, then
  // values). Hold the VM height, keep the list transparent until the new rows
  // are named and stable (max 500 ms), then fade/resize with the VM animation.
  // A single app starting or closing keeps most items and is not a rebuild.
  static bool IsRebuild(std::vector<void *> const &before, std::vector<void *> const &now) {
    if (before.empty())
      return false;
    size_t kept = 0;
    for (auto p : before)
      if (std::find(now.begin(), now.end(), p) != now.end())
        kept++;
    return kept * 2 < before.size();
  }
  void StartHold() {
    // A second rebuild can follow within ~100 ms (FxSound restores its output
    // after a selection): stop the crossfade and show the rows as before again.
    if (animation_ && animation_.IsEnabled() && holdAnimation_) {
      animation_.Stop();
      holdAnimation_ = false;
    }
    holding_ = true;
    holdStarted_ = GetTickCount64();
    holdStable_ = 0;
    holdSignature_.clear();
    // LayoutUpdated already reports the first intermediate arrangement; hold the
    // height of the last stable one (seen: the panel grew for the empty phase).
    auto held = std::max(0., stableListHeight_ > 0 ? stableListHeight_ : mixerList_.ActualHeight());
    mixerList_.Height(held);
    // Show the rows as they were (snapshot from the OD/SS flyout opening or the
    // maintenance pass) instead of an empty area; crossfade to the new rows.
    if (crossfade_ && snapshotBrush_ && itemsHost_) {
      itemsHost_.Opacity(0);
      snapshotBrush_.Opacity(1);
      mixerList_.Opacity(1);
    } else {
      M::Imaging::RenderTargetBitmap bitmap{nullptr};
      {
        std::lock_guard guard(snapshot_->lock);
        if (snapshot_->bitmap &&
            (snapshot_->items == rowItems_ || GetTickCount64() - snapshot_->at < 3000) &&
            std::abs(snapshot_->height - held) < 1)
          bitmap = snapshot_->bitmap;
      }
      if (!(bitmap && ShowSnapshot(bitmap)))
        mixerList_.Opacity(0);
    }
    if (!holdTimer_) {
      holdTimer_ = X::DispatcherTimer();
      holdTimer_.Interval(std::chrono::milliseconds(16));
      holdTick_ = holdTimer_.Tick([this](auto &&, auto &&) { HoldTick(); });
    }
    holdTimer_.Start();
  }
  void CaptureMixer() {
    if (!expanded_ || holding_ || !mixerList_ ||
        mixerList_.Visibility() != X::Visibility::Visible ||
        (animation_ && animation_.IsEnabled()) || mixerList_.ActualHeight() < 1)
      return;
    auto shared = snapshot_;
    {
      std::lock_guard guard(shared->lock);
      if (shared->capturing)
        return;
      shared->capturing = true;
    }
    try {
      M::Imaging::RenderTargetBitmap bitmap;
      auto items = rowItems_;
      auto height = mixerList_.ActualHeight();
      bitmap.RenderAsync(mixerList_)
          .Completed([shared, bitmap, items,
                      height](auto const &, winrt::Windows::Foundation::AsyncStatus status) {
            std::lock_guard guard(shared->lock);
            shared->capturing = false;
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
              shared->bitmap = bitmap;
              shared->items = items;
              shared->height = height;
              shared->at = GetTickCount64();
            }
          });
    } catch (...) {
      std::lock_guard guard(shared->lock);
      shared->capturing = false;
      Wh_Log(L"SnapshotError %08X", winrt::to_hresult());
    }
  }
  bool ShowSnapshot(M::Imaging::RenderTargetBitmap const &bitmap) {
    try {
      itemsHost_ = Find(mixerList_,
                        [](auto const &e) { return bool(e.template try_as<C::ItemsPresenter>()); });
      if (!itemsHost_)
        return false;
      savedBackground_ = mixerList_.ReadLocalValue(C::Control::BackgroundProperty());
      snapshotBrush_ = M::ImageBrush();
      snapshotBrush_.ImageSource(bitmap);
      snapshotBrush_.Stretch(M::Stretch::Fill);
      mixerList_.Background(snapshotBrush_);
      itemsHost_.Opacity(0);
      mixerList_.Opacity(1);
      crossfade_ = true;
      return true;
    } catch (...) {
      Wh_Log(L"SnapshotError %08X", winrt::to_hresult());
      ClearSnapshot();
      return false;
    }
  }
  void ClearSnapshot() {
    if (!crossfade_ && !snapshotBrush_)
      return;
    crossfade_ = false;
    try {
      if (itemsHost_)
        itemsHost_.ClearValue(X::UIElement::OpacityProperty());
    } catch (...) {
    }
    try {
      if (!savedBackground_ || savedBackground_ == X::DependencyProperty::UnsetValue())
        mixerList_.ClearValue(C::Control::BackgroundProperty());
      else
        mixerList_.SetValue(C::Control::BackgroundProperty(), savedBackground_);
    } catch (...) {
    }
    snapshotBrush_ = nullptr;
    itemsHost_ = nullptr;
    savedBackground_ = nullptr;
  }
  void HoldTick() {
    try {
      if (rowsDirty_ || RowSignature() != rowSignature_) {
        PrepareMixerRows();
        AlignTextBaselines();
      }
      auto n = mixerList_.Items().Size();
      bool ready = n && !pendingRows_ && mixerList_.ContainerFromIndex(n - 1);
      auto sig = RowSignature();
      holdStable_ = ready && sig == holdSignature_ ? holdStable_ + 1 : ready ? 1 : 0;
      holdSignature_ = sig;
      // The shell adds sessions with pauses of more than 48 ms: ~100 ms of quiet.
      auto waited = GetTickCount64() - holdStarted_;
      // Opening without a refill: the previous list was current; 200 ms of quiet.
      bool settled =
          openHold_ ? (openRebuilt_ ? holdStable_ >= 6 : holdStable_ >= 12) : holdStable_ >= 6;
      if (settled || waited >= 600)
        EndHold();
    } catch (...) {
      Wh_Log(L"RebuildHoldError %08X", winrt::to_hresult());
      EndHold();
    }
  }
  void EndHold() {
    if (!holding_)
      return;
    holding_ = false;
    if (holdTimer_)
      holdTimer_.Stop();
    if (openHold_) {
      openHold_ = false;
      openRebuilt_ = false;
    }
    if (!expanded_ || !winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) {
      ClearSnapshot();
      mixerList_.Height(NAN);
      mixerList_.Opacity(1);
      ResizeNative();
      return;
    }
    if (auto rotation = arrow_.RenderTransform().try_as<M::RotateTransform>())
      arrowStart_ = rotation.Angle();
    from_ = mixerList_.ActualHeight();
    mixerList_.Height(NAN);
    mixerList_.Measure({float(std::max(1., mixerList_.ActualWidth())), INFINITY});
    to_ = mixerList_.DesiredSize().Height;
    mixerList_.Height(from_);
    start_ = NowMs();
    animFirst_ = true;
    EnsureAnimation();
    holdAnimation_ = true;
    animation_.Start();
    StartAnimationLoop();
  }
  void ScheduleRowsRecheck() {
    if (!rowsTimer_) {
      rowsTimer_ = X::DispatcherTimer();
      rowsTimer_.Interval(std::chrono::milliseconds(16));
      rowsTick_ = rowsTimer_.Tick([this](auto &&, auto &&) {
        rowsTimer_.Stop();
        try {
          if (page_) {
            PrepareMixerRows();
            AlignTextBaselines();
            if (!animation_ || !animation_.IsEnabled())
              ResizeNative();
          }
        } catch (...) {
          Wh_Log(L"RowsRecheckError %08X", winrt::to_hresult());
        }
      });
    }
    if (!rowsTimer_.IsEnabled())
      rowsTimer_.Start();
  }
  void MarkRowsDirty() { rowsDirty_ = true; }
  // Divider -> SV rail and SV rail -> panel edge: one StepPx each. The divider
  // is a 1 DIP border (one pixel), the SV control 32 DIP (centre at 16).
  void FooterMargins() {
    if (!footer_)
      return;
    auto root = page_.XamlRoot();
    double s = root ? root.RasterizationScale() : 1.;
    double step = StepPx(s), half = std::round(16 * s), border = std::max(1., std::round(s));
    footer_.Margin({20, (step - border - half) / s, 20, (step - half) / s});
  }
  // The SV divider lies on the footer's base acrylic, the line above All
  // settings on the tiles' layer (#09FFFFFF, = the Sound content's
  // LayerOnAcrylicFillColorDefault overlay). White over white composes:
  // 1-(1-0.035)(1-0.082) = 0.114 -> #1DFFFFFF gives the same pixel as layer +
  // line on any wallpaper.
  void FooterLine() {
    if (DarkTheme(page_))
      footerBorder_.BorderBrush(DarkBaseDivider());
    else
      PaintDivider(footerBorder_, page_);
  }
  void StartOpenHold() {
    if (!expanded_ || holding_ || !mixerList_ ||
        mixerList_.Visibility() != X::Visibility::Visible ||
        !winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled())
      return;
    openHold_ = true;
    openRebuilt_ = false;
    holding_ = true;
    holdStarted_ = GetTickCount64();
    holdStable_ = 0;
    holdSignature_.clear();
    mixerList_.Opacity(0);
    if (!holdTimer_) {
      holdTimer_ = X::DispatcherTimer();
      holdTimer_.Interval(std::chrono::milliseconds(16));
      holdTick_ = holdTimer_.Tick([this](auto &&, auto &&) { HoldTick(); });
    }
    holdTimer_.Start();
  }
  ULONGLONG userChangeAt_ = 0;
  void BeforeRender() {
    if (!prepared_ || preparing_)
      return;
    // Every slider step re-lays out its template and raises this pass; the VM
    // row checks (signatures over all containers) are skipped while the SV
    // slider is being moved.
    if (GetTickCount64() - userChangeAt_ < 150 && !rowsDirty_ &&
        !(animation_ && animation_.IsEnabled()))
      return;
    // Safety valve: if our layout work repeats within one timer quantum, the
    // layout manager is iterating; stop contributing instead of risking a cycle.
    {
      auto now = GetTickCount64();
      if (now != layoutWorkTick_) {
        layoutWorkTick_ = now;
        layoutWorkCount_ = 0;
      }
      if (++layoutWorkCount_ > 8) {
        return;
      }
    }
    if ((!holding_ || openHold_) && expanded_ &&
        mixerList_.Visibility() == X::Visibility::Visible &&
        (!(animation_ && animation_.IsEnabled()) || holdAnimation_)) {
      auto items = RowItems();
      bool animating = animation_ && animation_.IsEnabled();
      if (items == rowItems_ && !rowsDirty_) {
        if (!animating && !openRebuilt_)
          stableListHeight_ = mixerList_.ActualHeight();
      } else if (IsRebuild(rowItems_, items)) {
        // Opening: the rows are hidden already; keep the list height of the
        // previous rows while the shell refills it, then wait for it to settle.
        if (openHold_) {
          if (!openRebuilt_) {
            openRebuilt_ = true;
            holdStable_ = 0;
            if (stableListHeight_ > 0)
              mixerList_.Height(stableListHeight_);
          }
        } else
          StartHold();
      }
    }
    if (!rowsDirty_ && mixerList_.Visibility() == X::Visibility::Visible &&
        RowSignature() != rowSignature_)
      rowsDirty_ = true;
    if (rowsDirty_ && expanded_ && animation_ && animation_.IsEnabled()) {
      // Rows changed during the expand animation: prepare them and retarget it.
      PrepareMixerRows();
      AlignTextBaselines();
      auto h = mixerList_.Height();
      mixerList_.Height(NAN);
      mixerList_.Measure({float(std::max(1., mixerList_.ActualWidth())), INFINITY});
      to_ = mixerList_.DesiredSize().Height;
      mixerList_.Height(h);
    }
    if (rowsDirty_) {
      PrepareMixerRows();
      AlignTextBaselines();
      ResizeNative();
    }
    // A shell write of the stock L2 height must not survive until the 200ms
    // maintenance pass. Correct it in layout, while this sound page owns it.
    if (auto target = TargetHeight();
        frame_ && target > 0 &&
        (!std::isfinite(frame_.Height()) || std::abs(frame_.Height() - target) > .25)) {
      SetPanelHeight(target);
    }
  }
  // Level column: the number, or the mute glyph at 0.
  static void ShowLevel(AppRow &row, double value) {
    long n = std::lround(value);
    if (row.level)
      row.level.Text(std::to_wstring(n));
    if (row.levelButton) {
      row.levelButton.Visibility(n <= 0 ? X::Visibility::Collapsed : X::Visibility::Visible);
      // At 100 the button is disabled rather than taken out of hit testing: a
      // hit-test change under the pointer leaves the button's pointer-over state
      // set, and its hover plate came back on the next visual state update.
      if (row.levelButton.IsEnabled() != (n < 100))
        row.levelButton.IsEnabled(n < 100);
    }
    if (row.levelMute)
      row.levelMute.Visibility(n <= 0 ? X::Visibility::Visible : X::Visibility::Collapsed);
  }
  void App(C::Slider const &slider, X::Style const &style) {
    if (appIds_.contains(winrt::get_abi(slider)))
      return;
    auto root = Parent(slider).try_as<C::Grid>();
    if (!root)
      return;
    auto toggle = Find(root, [](auto const &e) {
                    return bool(e.template try_as<P::ToggleButton>());
                  }).try_as<P::ToggleButton>();
    if (!toggle)
      return;
    appIds_.insert(winrt::get_abi(slider));
    auto row = std::make_shared<AppRow>();
    row->root = winrt::make_weak(root);
    row->slider = winrt::make_weak(slider);
    row->model = RowModel(slider);
    if (slider.Value() > 0)
      row->last = slider.Value();
    rows_.push_back(row);
    journal_.Save(slider, X::FrameworkElement::StyleProperty());
    slider.Style(style);
    journal_.Save(slider, X::FrameworkElement::MinWidthProperty());
    slider.MinWidth(0);
    journal_.Save(slider, X::FrameworkElement::MarginProperty());
    slider.Margin({12, 0, 0, 0});
    journal_.Save(root, X::FrameworkElement::MarginProperty());
    root.Margin({20, 0, 64, 0});
    Set(root, X::FrameworkElement::HeightProperty(), winrt::box_value(38.));
    SliderGeometry(slider);
    for (auto prop : {X::FrameworkElement::WidthProperty(), X::FrameworkElement::HeightProperty()})
      journal_.Save(toggle, prop);
    toggle.Width(32);
    toggle.Height(32);
    row->toggle = winrt::make_weak(toggle);
    Set(toggle, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
    Set(toggle, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
    Set(toggle, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    Set(toggle, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
    Set(toggle, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{}));
    Set(toggle, C::Control::HorizontalContentAlignmentProperty(),
        winrt::box_value(X::HorizontalAlignment::Center));
    Set(toggle, C::Control::VerticalContentAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    // The shell badge is anchored to the bottom of the entire 48 DIP row.
    // A button-relative badge instead: right -5, bottom 2 of a 32 DIP icon.
    std::vector<X::FrameworkElement> nativeBadges;
    for (auto const &child : root.Children())
      if (auto e = child.try_as<X::FrameworkElement>();
          e && e != toggle && e != slider && C::Grid::GetColumn(e) == 0)
        nativeBadges.push_back(e);
    for (auto const &badge : nativeBadges)
      Set(badge, X::UIElement::VisibilityProperty(), winrt::box_value(X::Visibility::Collapsed));
    row->badge = C::Border();
    row->badge.Width(13);
    row->badge.Height(13);
    row->badge.CornerRadius({4, 4, 4, 4});
    row->badge.HorizontalAlignment(X::HorizontalAlignment::Right);
    row->badge.VerticalAlignment(X::VerticalAlignment::Center);
    row->badge.Margin({0, 15, -5, 0});
    row->badge.IsHitTestVisible(false);
    row->badge.Child(VolumeGlyph(page_, 0, 11));
    if (auto brush = Resource(page_, L"SolidBackgroundFillColorBaseBrush").try_as<M::Brush>())
      row->badge.Background(brush);
    row->badge.Visibility(slider.Value() <= 0 ? X::Visibility::Visible : X::Visibility::Collapsed);
    C::Canvas::SetZIndex(row->badge, 1);
    // The mute indicator replaces the number in the level column at 0. The
    // app's volume is shown as a number in the column under the SV settings
    // button: 32 DIP wide, 12 DIP after the slider, i.e. inside the row's 64 DIP
    // right margin (negative margin; a Grid does not clip).
    // The number sits in a 32 DIP button with the same hover plate as the mute
    // button: pressing it (1-99) sets the app's volume to 100, written to the
    // session's model like the mouse wheel. At 100 it takes no input; at 0 the
    // mute button takes its place.
    row->levelButton = Button(page_, L"Set volume to 100");
    row->levelButton.HorizontalAlignment(X::HorizontalAlignment::Right);
    row->levelButton.VerticalAlignment(X::VerticalAlignment::Center);
    row->levelButton.Margin({0, 0, -44, 0});
    row->level = C::TextBlock();
    row->level.TextAlignment(X::TextAlignment::Center);
    row->level.HorizontalAlignment(X::HorizontalAlignment::Center);
    row->level.VerticalAlignment(X::VerticalAlignment::Center);
    // 12 DIP (caption size): "100" stays within a glyph's width inside the 32 DIP
    // plate, like the icons beside it. The button style carries a symbol font, so
    // the text font is set explicitly.
    if (pageTitle_)
      row->level.FontFamily(pageTitle_.FontFamily());
    else if (auto font = Resource(page_, L"ContentControlThemeFontFamily").try_as<M::FontFamily>())
      row->level.FontFamily(font);
    row->level.FontSize(12);
    row->level.Text(std::to_wstring(std::lround(slider.Value())));
    row->levelButton.Content(row->level);
    // The text keeps the page title's colour (bound, so it follows the theme)
    // also while the button is disabled at 100.
    if (pageTitle_) {
      D::Binding binding;
      binding.Source(pageTitle_);
      binding.Path(X::PropertyPath(L"Foreground"));
      row->level.SetBinding(C::TextBlock::ForegroundProperty(), binding);
    }
    C::Grid::SetColumnSpan(row->levelButton, std::max(1u, root.ColumnDefinitions().Size()));
    C::Grid::SetRowSpan(row->levelButton, std::max(1u, root.RowDefinitions().Size()));
    Add(root, row->levelButton);
    Keep(row->levelButton.Click(winrt::auto_revoke, [this, row](auto &&, auto &&) {
      try {
        auto s = row->slider.get();
        if (!s)
          return;
        auto value = std::lround(s.Value());
        if (value < 1 || value > 99)
          return;
        if (RowModel(s) != row->model)
          row->model = RowModel(s);
        row->wheelTarget = NAN;
        if (!WriteVolume(s, row->model, 100))
          s.Value(100);
      } catch (...) {
        Wh_Log(L"Mixer level button: %08X", winrt::to_hresult());
      }
    }));
    // At 0 a 32 DIP button with the SV's own glyph (QA_Volume, Mute state, same
    // size) replaces the number; pressing it unmutes like the app icon does: the
    // native toggle is toggled through UI Automation, so its command and the
    // mod's restore run exactly as for a click on the icon.
    row->levelMute = Button(page_, L"Unmute");
    row->levelMute.HorizontalAlignment(X::HorizontalAlignment::Right);
    row->levelMute.VerticalAlignment(X::VerticalAlignment::Center);
    row->levelMute.Margin({0, 0, -44, 0});
    try {
      row->levelIcon = std::make_shared<AnimatedVolume>();
      row->levelIcon->Update(0, true);
      row->levelMute.Content(row->levelIcon->Isolated());
    } catch (...) {
      row->levelIcon.reset();
      row->levelMute.Content(VolumeGlyph(page_, 0, 16));
    }
    {
      auto weakToggle = winrt::make_weak(toggle);
      Keep(row->levelMute.Click(winrt::auto_revoke, [weakToggle](auto &&, auto &&) {
        try {
          if (auto t = weakToggle.get())
            if (auto peer =
                    X::Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(t))
              if (auto provider = peer.GetPattern(X::Automation::Peers::PatternInterface::Toggle)
                                      .try_as<X::Automation::Provider::IToggleProvider>())
                provider.Toggle();
        } catch (...) {
          Wh_Log(L"MixerUnmuteError %08X", winrt::to_hresult());
        }
      }));
    }
    C::Grid::SetColumnSpan(row->levelMute, std::max(1u, root.ColumnDefinitions().Size()));
    C::Grid::SetRowSpan(row->levelMute, std::max(1u, root.RowDefinitions().Size()));
    Add(root, row->levelMute);
    ShowLevel(*row, slider.Value());
    // Application images and the native fallback FontIcon remain shell-owned.
    // ButtonBase can mark routed input handled before instance subscribers run.
    // Capture the old value even for handled input, before the native Click.
    auto capture = [this, row] {
      if (auto s = row->slider.get()) {
        if (RowModel(s) != row->model) {
          row->model = RowModel(s);
          row->last = s.Value() > 0 ? s.Value() : 50;
        }
        row->before = s.Value();
        row->beforeValid = true;
      }
    };
    auto pressed =
        winrt::box_value(X::Input::PointerEventHandler([capture](auto &&, auto &&) { capture(); }));
    toggle.AddHandler(X::UIElement::PointerPressedEvent(), pressed, true);
    revoke_.push_back(
        [toggle, pressed] { toggle.RemoveHandler(X::UIElement::PointerPressedEvent(), pressed); });
    auto key = winrt::box_value(
        X::Input::KeyEventHandler([capture](auto &&, X::Input::KeyRoutedEventArgs const &args) {
          using K = winrt::Windows::System::VirtualKey;
          if (args.Key() == K::Space || args.Key() == K::Enter)
            capture();
        }));
    toggle.AddHandler(X::UIElement::PreviewKeyDownEvent(), key, true);
    revoke_.push_back(
        [toggle, key] { toggle.RemoveHandler(X::UIElement::PreviewKeyDownEvent(), key); });
    Keep(toggle.Click(winrt::auto_revoke, [this, row](auto &&, auto &&) {
      try {
        if (auto slider = row->slider.get()) {
          if (RowModel(slider) != row->model) {
            row->model = RowModel(slider);
            row->beforeValid = false;
            row->last = slider.Value() > 0 ? slider.Value() : 50;
          }
          auto before = row->beforeValid ? row->before : slider.Value();
          row->beforeValid = false;
          if (before > 0)
            row->last = before;
          auto next = before > 0 ? 0 : row->last;
          bool sent = WriteVolume(slider, row->model, next);
          row->expected = sent ? next : NAN;
          row->verifyAt = GetTickCount64() + 600;
        }
      } catch (...) {
        Wh_Log(L"Native mute model: %08X", winrt::to_hresult());
      }
    }));
    // Mouse wheel as on SV. Written to the session's model like mute/restore;
    // quick notches add up on the last requested value instead of a slider that
    // has not caught up yet.
    Keep(slider.PointerWheelChanged(
        winrt::auto_revoke, [this, row](auto &&, X::Input::PointerRoutedEventArgs const &args) {
          try {
            auto s = row->slider.get();
            if (!s || args.Handled() || !s.IsEnabled())
              return;
            auto point = args.GetCurrentPoint(s);
            if (point.Properties().IsHorizontalMouseWheel())
              return;
            double delta = point.Properties().MouseWheelDelta() / 120.;
            if (!delta)
              return;
            args.Handled(true);
            if (RowModel(s) != row->model) {
              row->model = RowModel(s);
              row->wheelTarget = NAN;
            }
            auto now = GetTickCount64();
            double base = now - row->wheelAt < 400 && std::isfinite(row->wheelTarget)
                              ? row->wheelTarget
                              : s.Value();
            double step = s.SmallChange() > 0 ? s.SmallChange() : 1;
            double next = std::clamp(std::round(base + delta * step), s.Minimum(), s.Maximum());
            if (next == base)
              return;
            row->wheelTarget = next;
            row->wheelAt = now;
            if (!WriteVolume(s, row->model, next))
              s.Value(next);
          } catch (...) {
            Wh_Log(L"MixerWheelError %08X", winrt::to_hresult());
          }
        }));
    Keep(slider.ValueChanged(winrt::auto_revoke, [row](auto &&, auto const &args) {
      if (args.NewValue() > 0)
        row->last = args.NewValue();
      row->badge.Visibility(args.NewValue() <= 0 ? X::Visibility::Visible
                                                 : X::Visibility::Collapsed);
      ShowLevel(*row, args.NewValue());
    }));
  }
  void Refresh() {
    latest_ = State();
    for (auto const &menu : menus_)
      if (menu != input_)
        Summary(menu);
    auto const &state = latest_;
    ApplyMaster(state);
    SyncInput(state);
    for (auto const &menu : menus_)
      if (menu->open)
        CompactRealized(menu);
    PrepareMixerRows();
    AlignTextBaselines();
    AlignRows();
    ResizeNative();
    RememberHeight();
    {
      bool stale;
      {
        std::lock_guard guard(snapshot_->lock);
        stale = !snapshot_->bitmap || snapshot_->items != rowItems_ ||
                GetTickCount64() - snapshot_->at > 2000;
      }
      if (stale)
        CaptureMixer();
    }
  }
  void Restore() {
    if (restoring_)
      return;
    restoring_ = true;
    if (gate_) {
      gate_.Stop();
      gate_.Tick(gateTick_);
      gate_ = nullptr;
    }
    if (rowsTimer_) {
      rowsTimer_.Stop();
      rowsTimer_.Tick(rowsTick_);
      rowsTimer_ = nullptr;
    }
    if (holdTimer_) {
      holdTimer_.Stop();
      holdTimer_.Tick(holdTick_);
      holdTimer_ = nullptr;
    }
    holding_ = false;
    openHold_ = false;
    ClearSnapshot();
    fading_ = false;
    gateJournal_.Restore();
    audio_.Stop();
    StopAnimationLoop();
    if (animation_) {
      animation_.Stop();
      animation_.Tick(animationTick_);
      animation_ = nullptr;
    }
    if (gearStory_)
      gearStory_.Stop();
    for (auto const &menu : menus_) {
      menu->flyout.Hide();
      if (menu->arrowStory)
        menu->arrowStory.Stop();
    }
    for (auto &r : revoke_)
      try {
        r();
      } catch (...) {
      }
    revoke_.clear();
    journal_.Restore();
    frameJournal_.Restore();
    rows_.clear();
    menus_.clear();
    page_ = nullptr;
  }
  // A page Windows has replaced: stop timers, audio and handlers but leave its
  // controls as they are. Restoring it moves the old Output ListView out of its
  // flyout back into the dead page; its selection is then re-evaluated and the
  // shell can switch the default output, and sound stops.
  void Abandon() {
    if (restoring_)
      return;
    restoring_ = true;
    if (gate_) {
      gate_.Stop();
      gate_.Tick(gateTick_);
      gate_ = nullptr;
    }
    if (rowsTimer_) {
      rowsTimer_.Stop();
      rowsTimer_.Tick(rowsTick_);
      rowsTimer_ = nullptr;
    }
    if (holdTimer_) {
      holdTimer_.Stop();
      holdTimer_.Tick(holdTick_);
      holdTimer_ = nullptr;
    }
    holding_ = false;
    openHold_ = false;
    ClearSnapshot();
    fading_ = false;
    gateJournal_.undo.clear();
    gateJournal_.saved.clear();
    audio_.Stop();
    StopAnimationLoop();
    if (animation_) {
      animation_.Stop();
      animation_.Tick(animationTick_);
      animation_ = nullptr;
    }
    if (gearStory_)
      gearStory_.Stop();
    for (auto const &menu : menus_) {
      menu->flyout.Hide();
      if (menu->arrowStory)
        menu->arrowStory.Stop();
    }
    for (auto &r : revoke_)
      try {
        r();
      } catch (...) {
      }
    revoke_.clear();
    journal_.undo.clear();
    journal_.saved.clear();
    frameJournal_.Restore();
    rows_.clear();
    menus_.clear();
    page_ = nullptr;
  }
  ~SoundPage() { Restore(); }
};
} // namespace native









namespace native {
// System volume of the Quick Settings row, with the logic of SV on the Sound
// page: the slider is the mod's own and writes go straight to the default
// render endpoint (the role the Sound page uses), with the same mute rule
// (sound::SavedVolume: mute = level 0, unmute = last level). The shell's own
// slider updates the level through its pipeline and lags behind the pointer.
// Changes made elsewhere arrive through the endpoint's volume callback (the
// values come with it, no re-read); a default-device change re-opens the
// endpoint. Callbacks run on Core Audio threads and only queue work on the UI
// thread; Close() stops them.
class QuickVolume {
  struct State {
    std::mutex m;
    bool alive = true;
    winrt::Windows::System::DispatcherQueue queue{nullptr};
    std::function<void(float, bool)> changed;
    std::function<void()> reopen;
  };
  static void Post(std::shared_ptr<State> const &state, std::function<void(State &)> work) {
    winrt::Windows::System::DispatcherQueue queue{nullptr};
    {
      std::lock_guard lock(state->m);
      if (!state->alive)
        return;
      queue = state->queue;
    }
    if (!queue)
      return;
    auto weak = std::weak_ptr<State>(state);
    queue.TryEnqueue([weak, work = std::move(work)] {
      auto s = weak.lock();
      if (!s)
        return;
      {
        std::lock_guard lock(s->m);
        if (!s->alive)
          return;
      }
      try {
        work(*s);
      } catch (...) {
      }
    });
  }
  struct VolumeNotify : winrt::implements<VolumeNotify, IAudioEndpointVolumeCallback> {
    std::shared_ptr<State> state;
    explicit VolumeNotify(std::shared_ptr<State> s) : state(std::move(s)) {}
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA data) noexcept override {
      if (!data)
        return E_POINTER;
      float value = data->fMasterVolume;
      bool muted = data->bMuted;
      try {
        Post(state, [value, muted](State &s) {
          if (s.changed)
            s.changed(value, muted);
        });
      } catch (...) {
      }
      return S_OK;
    }
  };
  struct DeviceNotify : winrt::implements<DeviceNotify, IMMNotificationClient> {
    std::shared_ptr<State> state;
    explicit DeviceNotify(std::shared_ptr<State> s) : state(std::move(s)) {}
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role,
                                                     LPCWSTR) noexcept override {
      if (flow == eRender && role == eMultimedia)
        try {
          Post(state, [](State &s) {
            if (s.reopen)
              s.reopen();
          });
        } catch (...) {
        }
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) noexcept override {
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) noexcept override {
      return S_OK;
    }
  };
  std::shared_ptr<State> state_ = std::make_shared<State>();
  winrt::com_ptr<IMMDeviceEnumerator> enumerator_;
  winrt::com_ptr<IAudioEndpointVolume> volume_;
  winrt::com_ptr<VolumeNotify> volumeNotify_;
  winrt::com_ptr<DeviceNotify> deviceNotify_;
  bool deviceRegistered_ = false;
  std::map<std::wstring, sound::SavedVolume> saved_;
  std::wstring id_;
  int mutedSet_ = -1;
  void CloseEndpoint() {
    if (volume_ && volumeNotify_)
      volume_->UnregisterControlChangeNotify(volumeNotify_.get());
    volume_ = nullptr;
    id_.clear();
    mutedSet_ = -1;
  }
  void OpenEndpoint() {
    CloseEndpoint();
    if (!enumerator_)
      return;
    winrt::com_ptr<IMMDevice> device;
    if (FAILED(enumerator_->GetDefaultAudioEndpoint(eRender, eMultimedia, device.put())))
      return;
    LPWSTR id = nullptr;
    if (SUCCEEDED(device->GetId(&id)) && id) {
      id_ = id;
      CoTaskMemFree(id);
    }
    if (FAILED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                                volume_.put_void()))) {
      volume_ = nullptr;
      return;
    }
    if (volumeNotify_)
      volume_->RegisterControlChangeNotify(volumeNotify_.get());
  }

public:
  QuickVolume() = default;
  QuickVolume(QuickVolume const &) = delete;
  QuickVolume &operator=(QuickVolume const &) = delete;
  // On the UI thread. `changed(value,muted)` runs there after any change of the
  // endpoint's level or mute; `reopened` after the default device changed.
  bool Open(std::function<void(float, bool)> changed, std::function<void()> reopened) {
    state_->queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    state_->changed = [this, changed = std::move(changed)](float value, bool muted) {
      mutedSet_ = muted ? 1 : 0;
      saved_[id_].Observe(value, muted);
      if (changed)
        changed(value, muted);
    };
    state_->reopen = [this, reopened = std::move(reopened)] {
      OpenEndpoint();
      if (reopened)
        reopened();
    };
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                __uuidof(IMMDeviceEnumerator), enumerator_.put_void())))
      return false;
    volumeNotify_ = winrt::make_self<VolumeNotify>(state_);
    deviceNotify_ = winrt::make_self<DeviceNotify>(state_);
    deviceRegistered_ =
        SUCCEEDED(enumerator_->RegisterEndpointNotificationCallback(deviceNotify_.get()));
    OpenEndpoint();
    return bool(volume_);
  }
  void Close() {
    {
      std::lock_guard lock(state_->m);
      state_->alive = false;
      state_->changed = {};
      state_->reopen = {};
    }
    try {
      CloseEndpoint();
    } catch (...) {
    }
    try {
      if (enumerator_ && deviceNotify_ && deviceRegistered_)
        enumerator_->UnregisterEndpointNotificationCallback(deviceNotify_.get());
    } catch (...) {
    }
    deviceRegistered_ = false;
    enumerator_ = nullptr;
  }
  ~QuickVolume() { Close(); }
  bool Read(float &value, bool &muted) {
    if (!volume_)
      return false;
    BOOL m = FALSE;
    if (FAILED(volume_->GetMasterVolumeLevelScalar(&value)) || FAILED(volume_->GetMute(&m)))
      return false;
    muted = m;
    mutedSet_ = m ? 1 : 0;
    saved_[id_].Observe(value, muted);
    return true;
  }
  // As SV (sound::Audio::Execute): level from the slider, muted exactly at 0. The
  // mute is written only when it changes, so a drag costs one call per step.
  void SetValue(float value) {
    if (!volume_)
      return;
    value = sound::Clamp(value);
    volume_->SetMasterVolumeLevelScalar(value, nullptr);
    int muted = value <= 0 ? 1 : 0;
    if (muted != mutedSet_) {
      volume_->SetMute(muted, nullptr);
      mutedSet_ = muted;
    }
    saved_[id_].Observe(value, muted);
  }
  void ToggleMute() {
    if (!volume_)
      return;
    float before = 0;
    BOOL muted = FALSE;
    if (FAILED(volume_->GetMasterVolumeLevelScalar(&before)) || FAILED(volume_->GetMute(&muted)))
      return;
    auto &saved = saved_[id_];
    saved.Observe(before, muted);
    auto value = saved.Toggle(before, muted);
    volume_->SetMasterVolumeLevelScalar(value, nullptr);
    volume_->SetMute(value <= 0, nullptr);
    mutedSet_ = value <= 0 ? 1 : 0;
  }
};
} // namespace native


// Monitor brightness over DDC/CI (dxva2, loaded at run time) for displays
// without a native brightness slider. All monitor I/O runs on one worker
// thread: a DDC/CI request takes tens of milliseconds and must not block the
// shell's UI thread. Writes are coalesced to the latest value and spaced at
// least 50 ms apart, which monitors tolerate.
namespace native {
struct PhysicalMonitor {
  HANDLE handle;
  WCHAR description[128];
};
class Ddc {
  using CountFn = BOOL(WINAPI *)(HMONITOR, DWORD *);
  using GetFn = BOOL(WINAPI *)(HMONITOR, DWORD, PhysicalMonitor *);
  using DestroyFn = BOOL(WINAPI *)(DWORD, PhysicalMonitor *);
  using ReadFn = BOOL(WINAPI *)(HANDLE, DWORD *, DWORD *, DWORD *);
  using WriteFn = BOOL(WINAPI *)(HANDLE, DWORD);
  struct Api {
    CountFn count = nullptr;
    GetFn get = nullptr;
    DestroyFn destroy = nullptr;
    ReadFn read = nullptr;
    WriteFn write = nullptr;
    bool ok = false;
  };
  static Api const &Functions() {
    static Api api = [] {
      Api a;
      auto dll = LoadLibraryExW(L"dxva2.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
      if (!dll)
        return a;
      a.count =
          reinterpret_cast<CountFn>(GetProcAddress(dll, "GetNumberOfPhysicalMonitorsFromHMONITOR"));
      a.get = reinterpret_cast<GetFn>(GetProcAddress(dll, "GetPhysicalMonitorsFromHMONITOR"));
      a.destroy = reinterpret_cast<DestroyFn>(GetProcAddress(dll, "DestroyPhysicalMonitors"));
      a.read = reinterpret_cast<ReadFn>(GetProcAddress(dll, "GetMonitorBrightness"));
      a.write = reinterpret_cast<WriteFn>(GetProcAddress(dll, "SetMonitorBrightness"));
      a.ok = a.count && a.get && a.destroy && a.read && a.write;
      return a;
    }();
    return api;
  }
  struct Range {
    DWORD low = 0, high = 100;
    bool known = false;
  };
  std::mutex lock_;
  std::condition_variable wake_;
  std::thread worker_;
  bool stop_ = false;
  HMONITOR readMonitor_ = nullptr, writeMonitor_ = nullptr;
  int want_ = -1;
  std::map<HMONITOR, Range> ranges_;
  // Results for the UI thread.
  HMONITOR resultMonitor_ = nullptr;
  bool resultSupported_ = false;
  int resultValue_ = -1;
  unsigned revision_ = 0;
  int lastError_ = 0;
  template <class F> static bool Each(HMONITOR monitor, F &&f) {
    auto &api = Functions();
    if (!api.ok || !monitor)
      return false;
    DWORD n = 0;
    if (!api.count(monitor, &n) || !n)
      return false;
    std::vector<PhysicalMonitor> list(n);
    if (!api.get(monitor, n, list.data()))
      return false;
    bool any = false;
    for (auto &m : list)
      any |= f(m.handle);
    api.destroy(n, list.data());
    return any;
  }
  void Run() {
    for (;;) {
      HMONITOR read = nullptr, write = nullptr;
      int value = -1;
      {
        std::unique_lock guard(lock_);
        wake_.wait(guard, [&] { return stop_ || readMonitor_ || want_ >= 0; });
        if (stop_)
          return;
        read = readMonitor_;
        readMonitor_ = nullptr;
        write = writeMonitor_;
        value = want_;
        want_ = -1;
      }
      if (write && value >= 0) {
        Range range;
        {
          std::lock_guard guard(lock_);
          range = ranges_[write];
        }
        if (!range.known)
          Each(write, [&](HANDLE h) {
            DWORD lo = 0, cur = 0, hi = 0;
            if (!Functions().read(h, &lo, &cur, &hi) || hi <= lo)
              return false;
            range = {lo, hi, true};
            return true;
          });
        bool ok =
            range.known && Each(write, [&](HANDLE h) {
              return bool(Functions().write(
                  h, range.low + DWORD(std::lround(value * (range.high - range.low) / 100.))));
            });
        {
          std::lock_guard guard(lock_);
          if (range.known)
            ranges_[write] = range;
          if (!ok)
            lastError_ = int(GetLastError());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      if (read) {
        Range range;
        int current = -1;
        bool ok = Each(read, [&](HANDLE h) {
          DWORD lo = 0, cur = 0, hi = 0;
          if (!Functions().read(h, &lo, &cur, &hi) || hi <= lo)
            return false;
          if (current < 0) {
            range = {lo, hi, true};
            current = int(std::lround((cur - lo) * 100. / (hi - lo)));
          }
          return true;
        });
        std::lock_guard guard(lock_);
        if (ok)
          ranges_[read] = range;
        else
          lastError_ = int(GetLastError());
        resultMonitor_ = read;
        resultSupported_ = ok;
        resultValue_ = current;
        revision_++;
      }
    }
  }

public:
  Ddc() {
    worker_ = std::thread([this] { Run(); });
  }
  Ddc(Ddc const &) = delete;
  Ddc &operator=(Ddc const &) = delete;
  ~Ddc() {
    {
      std::lock_guard guard(lock_);
      stop_ = true;
    }
    wake_.notify_all();
    if (worker_.joinable())
      worker_.join();
  }
  void Read(HMONITOR monitor) {
    {
      std::lock_guard guard(lock_);
      readMonitor_ = monitor;
    }
    wake_.notify_all();
  }
  void Write(HMONITOR monitor, int value) {
    {
      std::lock_guard guard(lock_);
      writeMonitor_ = monitor;
      want_ = std::clamp(value, 0, 100);
    }
    wake_.notify_all();
  }
  struct Result {
    HMONITOR monitor;
    bool supported;
    int value;
    unsigned revision;
    int error;
  };
  Result Latest() {
    std::lock_guard guard(lock_);
    return {resultMonitor_, resultSupported_, resultValue_, revision_, lastError_};
  }
};
} // namespace native

#include <winrt/Windows.UI.Text.h>
namespace native {
// Quick Settings main page (ControlCenter.ControlCenterView). L1Grid holds a
// light Border spanning TogglesGroup + SlidersGroup, then FooterGrid
// (LeftFooter: battery on laptops; RightFooter: the native All settings
// button). Changes:
// - the light Border covers the tiles only; sliders sit in the dark block;
// - All settings: title on the left (plate from 12, text from 20 DIP, like the
//   Volume mixer heading), gear in the right column;
// - dark block rows one 38 DIP step apart (StepPx): All settings,
//   [brightness], volume; 32 DIP buttons, 20 DIP side insets, thin sliders;
// - the native battery footer moves into All settings; the old footer is hidden;
// - the volume L2 button shows the mixer glyph without the chevron and still
//   runs the native LaunchAdvancedPageCommand;
// - the pager keeps its native place, without arrows, with plain dots.
// Native tiles, paging, commands, bindings and values are untouched; every
// change is journaled and restored on unload.
class QuickPage {
  X::FrameworkElement view_{nullptr};
  Journal journal_;
  std::vector<std::function<void()>> revoke_;
  std::set<void *> thinSliders_;
  std::set<std::pair<void *, int>> done_;
  ULONGLONG lastEnsure_ = 0;
  bool dirty_ = true, restoring_ = false;
  int pagerTheme_ = -1;
  X::Style normalPip_{nullptr}, selectedPip_{nullptr};
  C::Grid settingsRow_{nullptr};
  C::Button gear_{nullptr}, title_{nullptr};
  U::AnimatedIcon gearIcon_{nullptr};
  winrt::weak_ref<C::Button> nativeSettings_;
  // Brightness row: night light status/toggle, DDC/CI slider, display settings.
  HWND host_ = nullptr;
  std::unique_ptr<Ddc> ddc_;
  C::StackPanel header_{nullptr};
  C::Grid brightRow_{nullptr};
  C::Button night_{nullptr}, display_{nullptr};
  C::Slider bright_{nullptr};
  U::AnimatedIcon nightIcon_{nullptr};
  U::IAnimatedIconStatics statics_{nullptr};
  bool reapplying_ = false;
  std::vector<std::shared_ptr<AnimatedVolume>> volumeIcons_;
  std::wstring nightState_, nightTip_;
  int nightRegistry_ = -1;
  U::AnimatedVisualPlayer brightPlayer_{nullptr};
  U::IAnimatedVisualSource2 brightSource_{nullptr};
  X::FrameworkElement brightVisual_{nullptr};
  double brightFrom_ = 0, brightTo_ = 1;
  HMONITOR monitor_ = nullptr;
  unsigned seenRevision_ = 0;
  ULONGLONG lastRead_ = 0, lastUser_ = 0;
  bool sync_ = false, visible_ = false;
  X::DispatcherTimer brightIdle_{nullptr};
  winrt::event_token brightIdleTick_{};
  static inline std::map<HMONITOR, int> brightCache; // -1: no DDC/CI
  template <class R> void Keep(R &&r) {
    auto revoker = std::make_shared<std::decay_t<R>>(std::forward<R>(r));
    revoke_.push_back([revoker] { revoker->revoke(); });
  }
  void Set(X::DependencyObject const &object, X::DependencyProperty const &property,
           IInspectable const &value) {
    journal_.Save(object, property);
    object.SetValue(property, value);
  }
  bool First(IInspectable const &object, int tag) {
    return done_.insert({winrt::get_abi(object), tag}).second;
  }
  SliderLook Look() { return {journal_, revoke_, thinSliders_}; }
  // The volume row's own slider and mute button (QuickVolume).
  std::unique_ptr<QuickVolume> quickVolume_;
  C::Slider quickSlider_{nullptr};
  C::Button quickMute_{nullptr};
  std::shared_ptr<AnimatedVolume> quickIcon_;
  bool quickSync_ = false, quickAvailable_ = false;
  ULONGLONG quickUserAt_ = 0;
  // As ApplyMaster on the Sound page: while the slider is being moved (400 ms
  // after the last own step) the echo of our own writes does not move it back.
  void ApplyVolume(float value, bool muted, bool available) {
    if (!quickSlider_)
      return;
    quickAvailable_ = available;
    bool dragging = GetTickCount64() - quickUserAt_ < 400;
    if (available && !dragging) {
      quickSync_ = true;
      try {
        quickSlider_.Value(muted ? 0 : std::round(value * 100));
      } catch (...) {
      }
      quickSync_ = false;
    }
    if (quickSlider_.IsEnabled() != available)
      quickSlider_.IsEnabled(available);
    if (quickMute_ && quickMute_.IsEnabled() != available)
      quickMute_.IsEnabled(available);
    if (quickIcon_) {
      if (dragging) {
        auto v = float(quickSlider_.Value() / 100);
        quickIcon_->Update(v, v <= 0, available);
      } else
        quickIcon_->Update(value, muted, available);
    }
  }
  void RefreshVolume() {
    if (!quickSlider_)
      return;
    float value = 0;
    bool muted = false;
    bool ok = quickVolume_ && quickVolume_->Read(value, muted);
    ApplyVolume(value, muted, ok);
  }
  // The media widget above Quick Settings: the shell hides it a few frames
  // after the Sound command runs, and the panel window's visible area follows
  // the content, so anything above the new top edge is cut. The widget fades in
  // 90 ms while RootContent (media + ControlCenterRegion) keeps its height,
  // then the height is released. A longer fade would need the command to wait.
  static void HoldMedia(X::FrameworkElement const &media, int ms) {
    auto content = Parent(media);
    if (!content || content.ActualHeight() < 1)
      return;
    auto local = content.ReadLocalValue(X::FrameworkElement::MinHeightProperty());
    content.MinHeight(content.ActualHeight());
    auto weakContent = winrt::make_weak(content);
    AfterDelay(ms, [weakContent, local] {
      if (auto c = weakContent.get())
        try {
          if (local == X::DependencyProperty::UnsetValue())
            c.ClearValue(X::FrameworkElement::MinHeightProperty());
          else
            c.SetValue(X::FrameworkElement::MinHeightProperty(), local);
        } catch (...) {
        }
    });
  }
  static X::FrameworkElement FindMedia(X::FrameworkElement const &from) {
    if (!from)
      return nullptr;
    X::FrameworkElement top = from;
    for (auto e = Parent(from); e; e = Parent(e))
      top = e;
    return Named(top, L"MediaTransportControls");
  }
  // Brightness -> volume rail and volume rail -> panel edge are one StepPx each
  // at every scale. The native slider items have their own rounding, so the
  // steps are measured after layout and the brightness row height and the
  // grid's bottom margin corrected, at most 4 times per scale.
  C::GridView grid_{nullptr};
  C::Slider volume_{nullptr};
  double rowHeight_ = 39.2, bottomMargin_ = 11.4, alignedScale_ = 0;
  int alignPasses_ = 0;
  void AlignSteps() {
    // Measured only while Quick Settings itself is shown and the view is exactly
    // its height (on the Sound page the view is as tall as Sound). Corrections
    // are limited to +-6 DIP. Cheap exit first: this runs on every layout pass.
    if (alignPasses_ >= 4 && view_ && view_.XamlRoot() &&
        view_.XamlRoot().RasterizationScale() == alignedScale_)
      return;
    if (!grid_ || !volume_ || !view_ || !volume_.IsLoaded() || volume_.ActualHeight() < 1 ||
        !PanelVisible())
      return;
    auto l1 = Named(view_, L"L1Grid");
    if (!l1 || l1.Visibility() != X::Visibility::Visible ||
        std::abs(l1.ActualHeight() - view_.ActualHeight()) > 1)
      return;
    if (auto frame = Named(view_, L"L2Frame");
        frame && frame.Visibility() == X::Visibility::Visible)
      return;
    auto root = view_.XamlRoot();
    if (!root)
      return;
    double s = root.RasterizationScale();
    if (s != alignedScale_) {
      alignedScale_ = s;
      alignPasses_ = 0;
    }
    if (alignPasses_ >= 4)
      return;
    // The pixel row of the drawn 1 DIP rail (SliderLook), not the slider's
    // centre: at 125% the centre falls on half pixels.
    auto centre = [&](X::FrameworkElement const &e) {
      X::FrameworkElement rail = Named(e, L"NativeSoundRail");
      X::FrameworkElement target = rail ? rail : e;
      double y = target.TransformToVisual(view_).TransformPoint({0, 0}).Y;
      if (!rail)
        y += e.ActualHeight() / 2;
      return std::floor(y * s + .5);
    };
    double step = StepPx(s), edge = std::round(view_.ActualHeight() * s), vol = centre(volume_);
    bool changed = false;
    if (std::abs(edge - vol - step) >= 1) {
      bottomMargin_ = std::clamp(bottomMargin_ - (edge - vol - step) / s, 11.4 - 6, 11.4 + 6);
      auto m = grid_.Margin();
      m.Bottom = bottomMargin_;
      grid_.Margin(m);
      changed = true;
    }
    if (bright_ && brightRow_ && brightRow_.Visibility() == X::Visibility::Visible &&
        bright_.ActualHeight() > 0) {
      double b = centre(bright_);
      if (std::abs(vol - b - step) >= 1) {
        rowHeight_ = std::clamp(rowHeight_ + (step - (vol - b)) / s, 39.2 - 6, 39.2 + 6);
        brightRow_.Height(rowHeight_);
        changed = true;
      }
    }
    alignPasses_ = changed ? alignPasses_ + 1 : 4;
  }
  std::wstring SettingsName() {
    if (auto b = nativeSettings_.get()) {
      auto n = std::wstring(X::Automation::AutomationProperties::GetName(b));
      if (!n.empty())
        return n;
    }
    return L"All settings";
  }
  void OpenSettings() {
    try {
      if (auto b = nativeSettings_.get())
        if (auto command = b.Command()) {
          auto parameter = b.CommandParameter();
          if (!parameter)
            parameter = b.DataContext();
          if (command.CanExecute(parameter)) {
            command.Execute(parameter);
            return;
          }
        }
    } catch (...) {
      Wh_Log(L"QuickSettingsCommandError %08X", winrt::to_hresult());
    }
    ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
  }
  void SmallButton(C::Control const &button) {
    Set(button, X::FrameworkElement::WidthProperty(), winrt::box_value(32.));
    Set(button, X::FrameworkElement::HeightProperty(), winrt::box_value(32.));
    Set(button, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
    Set(button, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
    Set(button, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{4, 0, 4, 0}));
    Set(button, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
  }
  // Gear (native QA_All_Settings animation, FooterIconButtonStyle states) and an
  // "All settings" title styled like the Volume mixer title; both run the native
  // footer button's command, which opens Settings and closes the panel.
  C::Grid SettingsRow() {
    // 38 DIP steps: 5.6 + 38.4 lands on whole pixels at 125% (7 + 48 px).
    auto row = Grid(L"40,*,40");
    row.Height(38.4);
    row.Margin({0, 5.6, 0, 0});
    auto name = SettingsName();
    gear_ = C::Button();
    if (auto style = Resource(view_, L"FooterIconButtonStyle").try_as<X::Style>())
      gear_.Style(style);
    gear_.Width(36);
    gear_.Height(36);
    gear_.MinWidth(0);
    gear_.MinHeight(0);
    gear_.Padding({0, 0, 0, 0});
    gear_.HorizontalAlignment(X::HorizontalAlignment::Center);
    gear_.VerticalAlignment(X::VerticalAlignment::Center);
    X::Automation::AutomationProperties::SetName(gear_, name);
    C::ToolTipService::SetToolTip(gear_, winrt::box_value(name));
    try {
      auto factory = NativeAnimatedFactory();
      IInspectable inner{nullptr};
      gearIcon_ = factory.as<U::IAnimatedIconFactory>().CreateInstance(nullptr, inner);
      auto statics = factory.as<U::IAnimatedIconStatics>();
      gearIcon_.Source(NativeSource(L"ControlCenter.QA_All_Settings"));
      gearIcon_.Width(16);
      gearIcon_.Height(16);
      gearIcon_.IsHitTestVisible(false);
      gear_.Content(gearIcon_);
      // The native footer icon binds its State to the button's AnimatedIcon.State.
      auto icon = gearIcon_;
      auto gear = gear_;
      auto token = gear_.RegisterPropertyChangedCallback(
          statics.StateProperty(),
          [statics, icon](X::DependencyObject const &sender, X::DependencyProperty const &) {
            try {
              statics.SetState(icon, statics.GetState(sender));
            } catch (...) {
            }
          });
      revoke_.push_back([gear, statics, token] {
        gear.UnregisterPropertyChangedCallback(statics.StateProperty(), token);
      });
    } catch (...) {
      Wh_Log(L"QuickGearIconError %08X", winrt::to_hresult());
      gear_.Content(SymbolGlyph(view_, 0xe713, 16));
    }
    // The hover frame hugs the text.
    title_ = C::Button();
    if (auto style = Resource(view_, L"IconButtonStyle").try_as<X::Style>())
      title_.Style(style);
    title_.Width(NAN);
    title_.Height(32);
    title_.MinWidth(0);
    title_.MinHeight(32);
    title_.Padding({8, 0, 8, 0});
    title_.Margin({-4, 0, 8, 0});
    title_.HorizontalAlignment(X::HorizontalAlignment::Left);
    title_.HorizontalContentAlignment(X::HorizontalAlignment::Left);
    title_.VerticalContentAlignment(X::VerticalAlignment::Center);
    // IconButtonStyle supplies a symbol font: take the text font from a native tile label.
    C::TextBlock text;
    text.Text(name);
    text.FontSize(14);
    text.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    text.TextTrimming(X::TextTrimming::CharacterEllipsis);
    if (auto label = Named(view_, L"TitleText").try_as<C::TextBlock>())
      text.FontFamily(label.FontFamily());
    title_.Content(text);
    X::Automation::AutomationProperties::SetName(title_, name);
    // Title on the left like the section headings, the gear in the right column
    // (with the display and sound settings buttons).
    C::Grid::SetColumn(title_, 0);
    C::Grid::SetColumnSpan(title_, 2);
    row.Children().Append(title_);
    C::Grid::SetColumn(gear_, 2);
    row.Children().Append(gear_);
    Keep(gear_.Click(winrt::auto_revoke, [this](auto &&, auto &&) { OpenSettings(); }));
    Keep(title_.Click(winrt::auto_revoke, [this](auto &&, auto &&) { OpenSettings(); }));
    settingsRow_ = row;
    return row;
  }
  U::AnimatedIcon NativeIcon(PCWSTR source) {
    auto factory = NativeAnimatedFactory();
    IInspectable inner{nullptr};
    auto icon = factory.as<U::IAnimatedIconFactory>().CreateInstance(nullptr, inner);
    if (!statics_)
      statics_ = factory.as<U::IAnimatedIconStatics>();
    icon.Source(NativeSource(source));
    icon.Width(16);
    icon.Height(16);
    icon.MirroredWhenRightToLeft(false);
    icon.IsHitTestVisible(false);
    icon.HorizontalAlignment(X::HorizontalAlignment::Center);
    icon.VerticalAlignment(X::VerticalAlignment::Center);
    return icon;
  }
  // Same 32 DIP hover plate as the mute and Sound buttons.
  C::Button IconButton(std::wstring const &name) {
    C::Button b;
    if (auto style = Resource(view_, L"IconButtonStyle").try_as<X::Style>())
      b.Style(style);
    b.Width(32);
    b.Height(32);
    b.MinWidth(0);
    b.MinHeight(0);
    b.Padding({0, 0, 0, 0});
    b.HorizontalAlignment(X::HorizontalAlignment::Center);
    b.VerticalAlignment(X::VerticalAlignment::Center);
    X::Automation::AutomationProperties::SetName(b, name);
    C::ToolTipService::SetToolTip(b, winrt::box_value(name));
    return b;
  }
  // Brightness row: night light (status and toggle), brightness over DDC/CI
  // where Windows has no brightness slider, display settings.
  C::Grid BrightnessRow() {
    // Header rows on whole pixels at 125%: rows of fractional pixel height push
    // every row below off the grid.
    auto row = Grid(L"40,*,40");
    row.Height(39.2);
    row.Visibility(X::Visibility::Collapsed); // corrected by AlignSteps
    night_ = IconButton(L"Night light");
    // A plain Grid parent: the icons take only the states set here (see AnimatedVolume).
    C::Grid icons;
    icons.IsHitTestVisible(false);
    icons.HorizontalAlignment(X::HorizontalAlignment::Center);
    icons.VerticalAlignment(X::VerticalAlignment::Center);
    // QA_Nightlight draws its picture smaller in the 16 DIP box than
    // QA_Brightness (at 125%: 18 px vs 20 px wide); 17.6 DIP (22 px box) matches
    // the brightness picture that replaces it while the slider moves.
    try {
      nightIcon_ = NativeIcon(L"ControlCenter.QA_Nightlight");
      nightIcon_.Width(17.6);
      nightIcon_.Height(17.6);
      icons.Children().Append(nightIcon_);
    } catch (...) {
      Wh_Log(L"QuickBrightnessIconError %08X", winrt::to_hresult());
      icons.Children().Append(SymbolGlyph(view_, 0xe706, 16));
    }
    try {
      BrightnessIcon();
      if (brightVisual_) {
        brightVisual_.Visibility(X::Visibility::Collapsed);
        icons.Children().Append(brightVisual_);
      }
    } catch (...) {
      Wh_Log(L"QuickBrightnessPlayerError %08X", winrt::to_hresult());
    }
    night_.Content(icons);
    row.Children().Append(night_);
    // Status only: QA_Nightlight's PointerOver/Pressed frames show the other
    // status, so the icon stays on NormalOn/NormalOff; the button's own hover
    // plate shows the pointer. The button template still writes
    // PointerOver/Normal into the icon through its parents (as with QA_Volume,
    // see AnimatedVolume::Reapply), so a foreign state is replaced at once.
    if (nightIcon_ && statics_) {
      auto icon = nightIcon_;
      auto statics = statics_;
      auto token = nightIcon_.RegisterPropertyChangedCallback(
          statics.StateProperty(),
          [this](X::DependencyObject const &, X::DependencyProperty const &) {
            if (reapplying_ || nightState_.empty())
              return;
            try {
              auto now = std::wstring(statics_.GetState(nightIcon_));
              if (now == nightState_)
                return;
              reapplying_ = true;
              statics_.SetState(nightIcon_, nightState_);
              reapplying_ = false;
            } catch (...) {
              reapplying_ = false;
            }
          });
      revoke_.push_back([icon, statics, token] {
        icon.UnregisterPropertyChangedCallback(statics.StateProperty(), token);
      });
      // Every opening reloads the icon and its player can start on the default
      // frame while State keeps its value (no change, no callback): jump to the
      // current status ("" first, so no Off->On transition plays).
      Keep(nightIcon_.Loaded(winrt::auto_revoke, [this](auto &&, auto &&) {
        if (nightState_.empty() || !statics_ || !nightIcon_)
          return;
        reapplying_ = true;
        try {
          statics_.SetState(nightIcon_, L"");
          statics_.SetState(nightIcon_, nightState_);
        } catch (...) {
        }
        reapplying_ = false;
      }));
    }
    bright_ = C::Slider();
    if (auto style = Resource(view_, L"HeaderIconSliderStyle").try_as<X::Style>())
      bright_.Style(style);
    bright_.MinWidth(0);
    bright_.Minimum(0);
    bright_.Maximum(100);
    bright_.StepFrequency(1);
    bright_.SmallChange(2);
    bright_.LargeChange(10);
    bright_.IsThumbToolTipEnabled(true);
    X::Automation::AutomationProperties::SetName(bright_, L"Brightness");
    C::Grid::SetColumn(bright_, 1);
    row.Children().Append(bright_);
    display_ = IconButton(L"Display settings");
    display_.Content(SymbolGlyph(view_, 0xe7f4, 13.6));
    C::Grid::SetColumn(display_, 2);
    row.Children().Append(display_);
    Keep(night_.Click(winrt::auto_revoke, [this](auto &&, auto &&) { ToggleNight(); }));
    // 13.6 DIP (17 px at 125%), the size of the mixer and sound settings glyphs
    // in the same column.
    Keep(display_.Click(winrt::auto_revoke, [](auto &&, auto &&) {
      ShellExecuteW(nullptr, L"open", L"ms-settings:display", nullptr, nullptr, SW_SHOWNORMAL);
    }));
    Keep(bright_.ValueChanged(winrt::auto_revoke, [this](auto &&, auto const &args) {
      if (sync_ || !ddc_)
        return;
      int value = int(std::lround(args.NewValue()));
      lastUser_ = GetTickCount64();
      brightCache[monitor_] = value;
      ddc_->Write(monitor_, value);
      ShowBrightness(value);
    }));
    Keep(bright_.PointerWheelChanged(
        winrt::auto_revoke, [this](auto &&, X::Input::PointerRoutedEventArgs const &args) {
          try {
            if (args.Handled())
              return;
            auto point = args.GetCurrentPoint(bright_);
            if (point.Properties().IsHorizontalMouseWheel())
              return;
            double delta = point.Properties().MouseWheelDelta() / 120.;
            if (!delta)
              return;
            bright_.Value(std::clamp(bright_.Value() + delta * bright_.SmallChange(),
                                     bright_.Minimum(), bright_.Maximum()));
            args.Handled(true);
          } catch (...) {
          }
        }));
    brightIdle_ = X::DispatcherTimer();
    brightIdle_.Interval(std::chrono::milliseconds(800));
    brightIdleTick_ = brightIdle_.Tick([this](auto &&, auto &&) {
      brightIdle_.Stop();
      if (brightVisual_ && nightIcon_) {
        brightVisual_.Visibility(X::Visibility::Collapsed);
        nightIcon_.Visibility(X::Visibility::Visible);
      }
    });
    // The status is read before the first state is set; with the -1 default the
    // icon would start as NormalOff and animate to On a moment later.
    nightRegistry_ = RegistryNight();
    brightRow_ = row;
    NightState();
    return row;
  }
  // QA_Brightness has two markers, Brightness_at_0 and Brightness_at_100: the
  // icon is drawn for a level between them (the native slider's player). An
  // AnimatedVisualPlayer shows it at the slider's level; AnimatedIcon could only
  // snap to the two ends.
  void BrightnessIcon() {
    auto source = NativeSource(L"ControlCenter.QA_Brightness");
    brightSource_ = source;
    if (auto markers = source.Markers()) {
      if (markers.HasKey(L"Brightness_at_0"))
        brightFrom_ = markers.Lookup(L"Brightness_at_0");
      if (markers.HasKey(L"Brightness_at_100"))
        brightTo_ = markers.Lookup(L"Brightness_at_100");
    }
    winrt::Windows::Foundation::IActivationFactory factory{nullptr};
    try {
      factory = winrt::get_activation_factory<U::AnimatedVisualPlayer>();
    } catch (...) {
      auto mux = GetModuleHandleW(L"Microsoft.UI.Xaml.dll");
      if (!mux)
        throw;
      factory = NativeFactory(L"Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", mux);
    }
    // Composable class: IActivationFactory::ActivateInstance returns E_NOTIMPL.
    IInspectable inner{nullptr};
    brightPlayer_ = factory.as<U::IAnimatedVisualPlayerFactory>().CreateInstance(nullptr, inner);
    brightPlayer_.AutoPlay(false);
    brightPlayer_.Source(source.as<U::IAnimatedVisualSource>());
    brightPlayer_.Width(16);
    brightPlayer_.Height(16);
    brightPlayer_.IsHitTestVisible(false);
    brightPlayer_.HorizontalAlignment(X::HorizontalAlignment::Center);
    brightPlayer_.VerticalAlignment(X::VerticalAlignment::Center);
    brightVisual_ = brightPlayer_;
  }
  // While brightness changes the button shows the native brightness icon at the
  // new level, then the night light status 800 ms after the last change.
  void ShowBrightness(int value) {
    if (!brightVisual_ || !nightIcon_)
      return;
    try {
      if (brightSource_) {
        bool light = view_.ActualTheme() == X::ElementTheme::Light;
        brightSource_.SetColorProperty(L"Foreground",
                                       light ? winrt::Windows::UI::Color{0xE4, 0, 0, 0}
                                             : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF});
      }
      if (brightPlayer_)
        brightPlayer_.SetProgress(brightFrom_ +
                                  (brightTo_ - brightFrom_) * std::clamp(value, 0, 100) / 100.);
    } catch (...) {
    }
    nightIcon_.Visibility(X::Visibility::Collapsed);
    brightVisual_.Visibility(X::Visibility::Visible);
    brightIdle_.Stop();
    brightIdle_.Start();
  }
  // Night light status, read (never written) from the shell's CloudStore state
  // (byte 18: 0x15 on, 0x13 off).
  static int RegistryNight() {
    constexpr auto key =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Store\\DefaultAccount\\Current"
        L"\\default$windows.data.bluelightreduction.bluelightreductionstate\\windows.data."
        L"bluelightreduction.bluelightreductionstate";
    BYTE data[256];
    DWORD size = sizeof(data);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"Data", RRF_RT_REG_BINARY, nullptr, data, &size) !=
            ERROR_SUCCESS ||
        size < 19)
      return -1;
    return data[18] == 0x15 ? 1 : data[18] == 0x13 ? 0 : -1;
  }
  bool NightOn() { return nightRegistry_ == 1; }
  void NightState() {
    if (!night_)
      return;
    bool on = NightOn();
    auto state = std::wstring(on ? L"NormalOn" : L"NormalOff");
    if (nightIcon_ && statics_ && state != nightState_) {
      nightState_ = state;
      reapplying_ = true;
      try {
        statics_.SetState(nightIcon_, state);
      } catch (...) {
      }
      reapplying_ = false;
    }
    std::wstring name = std::wstring(L"Night light") + (on ? L": on" : L": off");
    if (name != nightTip_) {
      nightTip_ = name;
      C::ToolTipService::SetToolTip(night_, winrt::box_value(name));
      X::Automation::AutomationProperties::SetName(night_, name);
    }
  }
  void PollNight() {
    int night = RegistryNight();
    if (night != nightRegistry_) {
      nightRegistry_ = night;
      NightState();
    }
  }
  static void *Identity(IInspectable const &o) {
    return o ? winrt::get_abi(o.as<winrt::Windows::Foundation::IUnknown>()) : nullptr;
  }
  // The native tile when it is realized (page 2 tiles are released when page 1
  // shows, and containers are recycled, so it is looked up on every click).
  X::FrameworkElement NightTileNow() {
    void *mine = nightIcon_ ? Identity(nightIcon_) : nullptr;
    X::FrameworkElement found{nullptr};
    std::function<void(X::DependencyObject const &)> walk = [&](X::DependencyObject const &o) {
      if (found)
        return;
      if (auto a = o.try_as<U::AnimatedIcon>())
        if (Identity(a) != mine && Class(a.Source()) == L"ControlCenter.QA_Nightlight") {
          found = a.as<X::FrameworkElement>();
          return;
        }
      for (int i = 0; i < M::VisualTreeHelper::GetChildrenCount(o); i++)
        walk(M::VisualTreeHelper::GetChild(o, i));
    };
    walk(view_);
    if (!found)
      return nullptr;
    for (auto p = Parent(found); p && p != view_; p = Parent(p))
      if (Class(p).find(L"ToggleButton") != std::wstring::npos)
        return p;
    return nullptr;
  }
  // Night light toggle: the native tile through UI Automation when it is
  // realized, otherwise the Night light page of Settings.
  void ToggleNight() {
    try {
      if (auto tile = NightTileNow())
        if (auto peer =
                X::Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(tile)) {
          if (auto toggle = peer.GetPattern(X::Automation::Peers::PatternInterface::Toggle)
                                .try_as<X::Automation::Provider::IToggleProvider>()) {
            toggle.Toggle();
            nightRegistry_ = nightRegistry_ == 1 ? 0 : 1;
            NightState();
            return;
          }
          if (auto invoke = peer.GetPattern(X::Automation::Peers::PatternInterface::Invoke)
                                .try_as<X::Automation::Provider::IInvokeProvider>()) {
            invoke.Invoke();
            nightRegistry_ = nightRegistry_ == 1 ? 0 : 1;
            NightState();
            return;
          }
        }
    } catch (...) {
      Wh_Log(L"QuickNightToggleError %08X", winrt::to_hresult());
    }
    ShellExecuteW(nullptr, L"open", L"ms-settings:nightlight", nullptr, nullptr, SW_SHOWNORMAL);
  }
  bool NativeBrightness() {
    auto group = Named(view_, L"SlidersGroup");
    return group && Named(group, L"BrightnessPlayer");
  }
  bool PanelVisible() { return WindowShown(host_); }
  void ApplyBright(int value) {
    if (!brightRow_)
      return;
    bool show = value >= 0 && !NativeBrightness();
    auto want = show ? X::Visibility::Visible : X::Visibility::Collapsed;
    if (brightRow_.Visibility() != want)
      brightRow_.Visibility(want);
    if (value >= 0 && std::lround(bright_.Value()) != value) {
      sync_ = true;
      bright_.Value(value);
      sync_ = false;
    }
  }
  // Panel monitor: cached value at once, a DDC/CI read on opening and every 3 s
  // while open (not while the user is changing it).
  void Brightness(bool opening) {
    if (!brightRow_ || !ddc_)
      return;
    auto now = GetTickCount64();
    auto monitor = MonitorFromWindow(host_, MONITOR_DEFAULTTONEAREST);
    if (monitor != monitor_) {
      monitor_ = monitor;
      lastRead_ = 0;
      auto cached = brightCache.find(monitor);
      ApplyBright(cached != brightCache.end() ? cached->second : -1);
    }
    if (!NativeBrightness() && (opening || now - lastRead_ > 3000) && now - lastUser_ > 1500 &&
        (opening || visible_)) {
      lastRead_ = now;
      ddc_->Read(monitor_);
    }
    auto r = ddc_->Latest();
    if (r.revision != seenRevision_) {
      seenRevision_ = r.revision;
      if (r.monitor == monitor_) {
        int value = r.supported ? r.value : -1;
        brightCache[monitor_] = value;
        if (now - lastUser_ > 1500 || !r.supported)
          ApplyBright(value);
      }
    }
  }

  // Move the shell's actual footer ItemsControl, not a copy of its battery icon.
  // Keep its item templates, commands, bindings and future collection changes.
  bool MoveBatteryFooter(X::FrameworkElement const &left, X::FrameworkElement const &footer) {
    if (!left || !settingsRow_) {
      if (settingsRow_ && First(view_, 72))
        Wh_Log(L"Quick battery: LeftFooter was not found");
      return false;
    }
    // A template root can be owned by a ContentPresenter rather than directly
    // by a Panel. Move its enclosing slot; never remove generated ItemsPanel
    // children, whose collection is controlled by the ItemsControl.
    for (auto node = left; node; node = Parent(node))
      if (node == settingsRow_)
        return true;
    auto moving = left;
    C::Panel parent{nullptr};
    uint32_t index = 0;
    for (auto node = left; node && node != footer; node = Parent(node)) {
      auto candidate = Parent(node).try_as<C::Panel>();
      if (candidate && !candidate.IsItemsHost() && candidate.Children().IndexOf(node, index)) {
        moving = node;
        parent = candidate;
        break;
      }
    }
    if (!parent) {
      if (First(left, 73))
        Wh_Log(L"Quick battery: no detachable slot, left=%s parent=%s footer=%s",
               Class(left).c_str(), Class(Parent(left)).c_str(), Class(footer).c_str());
      return false;
    }
    auto element = moving;
    // Preserve the former inheritance source, including subsequent DataContext
    // replacements. Never overwrite an explicit native DataContext/binding.
    if (element.ReadLocalValue(X::FrameworkElement::DataContextProperty()) ==
        X::DependencyProperty::UnsetValue()) {
      journal_.Save(element, X::FrameworkElement::DataContextProperty());
      D::Binding binding;
      binding.Source(parent);
      binding.Path(X::PropertyPath(L"DataContext"));
      binding.Mode(D::BindingMode::OneWay);
      element.SetBinding(X::FrameworkElement::DataContextProperty(), binding);
    }
    Set(element, C::Grid::ColumnProperty(), winrt::box_value(1));
    Set(element, C::Grid::RowProperty(), winrt::box_value(0));
    Set(element, C::Grid::ColumnSpanProperty(), winrt::box_value(1));
    Set(element, C::Grid::RowSpanProperty(), winrt::box_value(1));
    Set(element, X::FrameworkElement::WidthProperty(), winrt::box_value(static_cast<double>(NAN)));
    Set(element, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
    Set(element, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
    Set(element, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{0, 0, 6, 0}));
    Set(element, X::FrameworkElement::HorizontalAlignmentProperty(),
        winrt::box_value(X::HorizontalAlignment::Right));
    Set(element, X::FrameworkElement::VerticalAlignmentProperty(),
        winrt::box_value(X::VerticalAlignment::Center));
    auto oldParent = winrt::make_weak(parent);
    auto newParent = winrt::make_weak(settingsRow_);
    // Registered before the mutation, so a failed Append is reversible as well.
    journal_.undo.push_back([oldParent, newParent, element, index] {
      if (auto dest = newParent.get()) {
        uint32_t current = 0;
        if (dest.Children().IndexOf(element, current))
          dest.Children().RemoveAt(current);
      }
      if (auto source = oldParent.get(); source && !Parent(element)) {
        uint32_t current = 0;
        if (!source.Children().IndexOf(element, current))
          source.Children().InsertAt(std::min(index, source.Children().Size()), element);
      }
    });
    parent.Children().RemoveAt(index);
    settingsRow_.Children().Append(element);
    Wh_Log(L"Quick battery: moved %s from %s into All settings",
           Class(element).c_str(), Class(parent).c_str());
    return true;
  }
  void CompactBattery(X::FrameworkElement const &left) {
    // Match the gear: 36-DIP button, with a 2-DIP template inset on every
    // side, giving both native hover plates the same 32-DIP height.
    Find(left, [&](auto const &element) {
      if (auto button = element.template try_as<C::Button>()) {
        if (First(button, 70)) {
          Set(button, X::FrameworkElement::HeightProperty(), winrt::box_value(36.));
          Set(button, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
          Set(button, X::FrameworkElement::MinWidthProperty(), winrt::box_value(0.));
          Set(button, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{}));
          Set(button, C::Control::PaddingProperty(), winrt::box_value(X::Thickness{8, 0, 8, 0}));
          Set(button, C::Control::HorizontalContentAlignmentProperty(),
              winrt::box_value(X::HorizontalAlignment::Center));
          Set(button, C::Control::VerticalContentAlignmentProperty(),
              winrt::box_value(X::VerticalAlignment::Center));
          Set(button, X::FrameworkElement::VerticalAlignmentProperty(),
              winrt::box_value(X::VerticalAlignment::Center));
        }
        // Remove the asymmetric 4,3,2,3 footer ItemsControl margin and any
        // enclosing slot insets. The plate, not the percentage, ends at the
        // slider rail: 6 DIP outside the button + 2 DIP inside its template.
        for (auto slot = Parent(button); slot && slot != settingsRow_; slot = Parent(slot))
          if (First(slot, 75)) {
            bool outer = Parent(slot) == settingsRow_;
            Set(slot, X::FrameworkElement::MarginProperty(),
                winrt::box_value(X::Thickness{0, 0, outer ? 6. : 0., 0}));
            Set(slot, X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.));
            Set(slot, X::FrameworkElement::VerticalAlignmentProperty(),
                winrt::box_value(X::VerticalAlignment::Center));
          }
        if (auto presenter = Named(button, L"ContentPresenter"); presenter && First(presenter, 71))
          Set(presenter, X::FrameworkElement::MarginProperty(), winrt::box_value(X::Thickness{2, 2, 2, 2}));
        // Keep the native icon/font/bindings, but centre both icon branches and
        // the percentage on the gear axis (the new percentage has a -1 top inset).
        if (auto content = button.Content().template try_as<X::FrameworkElement>()) {
          if (First(content, 76))
            Set(content, X::FrameworkElement::VerticalAlignmentProperty(),
                winrt::box_value(X::VerticalAlignment::Center));
          Find(content, [&](auto const &part) {
            if (part.template try_as<C::TextBlock>() && First(part, 77)) {
              auto margin = part.Margin();
              margin.Top = margin.Bottom = 0;
              Set(part, X::FrameworkElement::MarginProperty(), winrt::box_value(margin));
              Set(part, X::FrameworkElement::VerticalAlignmentProperty(),
                  winrt::box_value(X::VerticalAlignment::Center));
            }
            return false;
          });
        }
      }
      return false; // Visit all native buttons without stopping the traversal.
    });
    // Reserve the battery width and its 6-DIP outer inset; gear stays in column 2.
    if (title_ && settingsRow_.ActualWidth() > 0) {
      double available = std::max(0., settingsRow_.ActualWidth() - 40. - left.ActualWidth() - 6. - 12.);
      if (title_.MaxWidth() != available)
        title_.MaxWidth(available);
    }
  }
  void Footer() {
    auto footer = Named(view_, L"FooterGrid");
    if (!footer) {
      if (First(view_, 74)) Wh_Log(L"Quick battery: FooterGrid was not found");
      return;
    }
    if (!nativeSettings_.get())
      if (auto icon = Find(footer, [](auto const &e) {
            auto a = e.template try_as<U::AnimatedIcon>();
            return a && Class(a.Source()) == L"ControlCenter.QA_All_Settings";
          }))
        for (auto p = Parent(icon); p && p != footer; p = Parent(p))
          if (auto button = p.try_as<C::Button>()) {
            nativeSettings_ = winrt::make_weak(button);
            break;
          }
    // Search the view: after migration LeftFooter lives in our header.
    auto left = Named(view_, L"LeftFooter");
    bool moved = MoveBatteryFooter(left, footer);
    if (moved)
      CompactBattery(left);
    bool used = left && ((left.try_as<C::ItemsControl>() &&
                         left.as<C::ItemsControl>().Items().Size() > 0) || Find(left, [&](auto const &e) {
                  return e != left && e.template try_as<C::ItemsControl>() &&
                         e.Visibility() == X::Visibility::Visible;
                }));
    // Preserve the native fallback if an unfamiliar parent prevents migration,
    // or if no working replacement for the native Settings button was found.
    auto want = (!moved && used) || !nativeSettings_.get()
                    ? X::Visibility::Visible : X::Visibility::Collapsed;
    if (footer.Visibility() != want)
      Set(footer, X::UIElement::VisibilityProperty(), winrt::box_value(want));
  }
  // The light Border spans the tiles and the sliders rows: end it above the sliders.
  void DarkBlock(C::Grid const &l1) {
    auto sliders = Named(l1, L"SlidersGroup");
    if (!sliders)
      return;
    int row = C::Grid::GetRow(sliders);
    for (auto const &child : l1.Children())
      if (auto border = child.try_as<C::Border>(); border && border.Name().empty()) {
        int top = C::Grid::GetRow(border), span = C::Grid::GetRowSpan(border);
        if (top < row && top + span > row)
          Set(border, C::Grid::RowSpanProperty(), winrt::box_value(row - top));
        // Its edge divides the tiles from the dark block: the divider colour for
        // the slider rails and the Sound page's SV/VM divider.
        if (top < row) {
          quickDivider = winrt::make_weak(border);
        }
      }
  }
  void Pager() {
    auto pager = Named(view_, L"QuickActionsPager").try_as<U::PipsPager>();
    if (!pager)
      return;
    if (First(pager, 1)) {
      normalPip_ = pager.NormalPipStyle();
      selectedPip_ = pager.SelectedPipStyle();
      Set(pager, U::PipsPager::PreviousButtonVisibilityProperty(),
          winrt::box_value(U::PipsPagerButtonVisibility::Collapsed));
      Set(pager, U::PipsPager::NextButtonVisibilityProperty(),
          winrt::box_value(U::PipsPagerButtonVisibility::Collapsed));
    }
    double pipScale = pager.XamlRoot() ? pager.XamlRoot().RasterizationScale() : 1.;
    int theme = (pager.ActualTheme() == X::ElementTheme::Light ? 1 : 0) |
                (int(std::lround(pipScale * 100)) << 1);
    if (theme == pagerTheme_)
      return;
    pagerTheme_ = theme;
    theme &= 1;
    // Dots in the text colour, inactive at 35%, 10 DIP apart. Position stays
    // native: centred between the tiles and the panel edge.
    winrt::Windows::UI::Color color = theme ? winrt::Windows::UI::Color{0xE4, 0, 0, 0}
                                            : winrt::Windows::UI::Color{0xFF, 0xF6, 0xF6, 0xF6};
    auto pip = [&](X::Style const &base, double opacity, double size) {
      X::Style style(winrt::xaml_typename<C::Button>());
      if (base)
        style.BasedOn(base);
      M::SolidColorBrush brush(color);
      brush.Opacity(opacity);
      style.Setters().Append(X::Setter(C::Control::ForegroundProperty(), brush));
      style.Setters().Append(
          X::Setter(X::FrameworkElement::HeightProperty(), winrt::box_value(10.)));
      style.Setters().Append(
          X::Setter(X::FrameworkElement::MinHeightProperty(), winrt::box_value(0.)));
      style.Setters().Append(X::Setter(C::Control::FontSizeProperty(), winrt::box_value(size)));
      return style;
    };
    // Both dots on one vertical axis: their pixel sizes must have the same
    // parity (at 100%, 4 and 5 px cannot be centred together; 4 and 6 can).
    double normalPx = std::max(2., std::round(4 * pipScale)), selectedPx = std::round(5 * pipScale);
    if (int(selectedPx - normalPx) % 2)
      selectedPx += 1;
    Set(pager, U::PipsPager::NormalPipStyleProperty(), pip(normalPip_, .35, normalPx / pipScale));
    Set(pager, U::PipsPager::SelectedPipStyleProperty(),
        pip(selectedPip_, 1., selectedPx / pipScale));
  }
  // Mute button of the volume row: a button of the mod's own in the same Grid
  // cell, with the system QA_Volume asset driven by Core Audio and the slider.
  // The mod's icon cannot live inside the native ToggleButton: the shell owns
  // that button and puts its own icon and Mute state back.
  // The volume row: the mod's own slider (QuickVolume) and mute button. The
  // native slider stays in the layout, transparent, without hit testing, tab
  // stop or UI Automation presence, so the shell's row keeps its size; the
  // native ToggleButton leaves the row and is put back on unload.
  void VolumeIcon(C::Button const &l2) {
    auto row = Parent(l2).try_as<C::Panel>();
    if (!row)
      return;
    P::ToggleButton mute{nullptr};
    C::Slider slider{nullptr};
    for (auto const &child : row.Children()) {
      if (auto t = child.try_as<P::ToggleButton>())
        mute = t;
      if (auto v = child.try_as<C::Slider>(); v && v != quickSlider_)
        slider = v;
    }
    if (!mute || !slider || !First(mute, 5))
      return;
    std::shared_ptr<AnimatedVolume> icon;
    try {
      icon = std::make_shared<AnimatedVolume>();
    } catch (...) {
      Wh_Log(L"QuickVolumeIconError %08X", winrt::to_hresult());
      return;
    }
    volumeIcons_.push_back(icon);
    quickIcon_ = icon;
    auto hide = [this](X::UIElement const &e) {
      Set(e, X::UIElement::OpacityProperty(), winrt::box_value(0.));
      Set(e, X::UIElement::IsHitTestVisibleProperty(), winrt::box_value(false));
      Set(e, C::Control::IsTabStopProperty(), winrt::box_value(false));
      Set(e, X::Automation::AutomationProperties::AccessibilityViewProperty(),
          winrt::box_value(X::Automation::Peers::AccessibilityView::Raw));
    };
    auto insertAfter = [this, row](X::UIElement const &native, X::UIElement const &own) {
      uint32_t index = 0;
      row.Children().IndexOf(native, index);
      row.Children().InsertAt(index + 1, own);
      auto weakRow = winrt::make_weak(row);
      journal_.undo.push_back([weakRow, own] {
        if (auto r = weakRow.get()) {
          uint32_t i = 0;
          if (r.Children().IndexOf(own, i))
            r.Children().RemoveAt(i);
        }
      });
    };
    // Mute button, built as SV's on the Sound page (Button(), IconButtonStyle,
    // 32 DIP, the isolated system QA_Volume icon), in the toggle's Grid cell.
    auto own = Button(view_, L"Mute system volume");
    own.Margin(mute.Margin());
    own.HorizontalAlignment(mute.HorizontalAlignment());
    own.VerticalAlignment(X::VerticalAlignment::Center);
    C::Grid::SetColumn(own, C::Grid::GetColumn(mute));
    C::Grid::SetRow(own, C::Grid::GetRow(mute));
    own.Content(icon->Isolated());
    {
      uint32_t index = 0;
      if (row.Children().IndexOf(mute, index)) {
        row.Children().RemoveAt(index);
        row.Children().InsertAt(index, own);
      } else
        row.Children().Append(own);
      auto weakRow = winrt::make_weak(row);
      journal_.undo.push_back([weakRow, own, mute, index] {
        if (auto r = weakRow.get()) {
          uint32_t i = 0;
          if (r.Children().IndexOf(own, i))
            r.Children().RemoveAt(i);
          if (!r.Children().IndexOf(mute, i))
            r.Children().InsertAt(std::min(index, r.Children().Size()), mute);
        }
      });
    }
    quickMute_ = own;
    // Slider: as master_ on the Sound page; geometry and thin look follow in Sliders().
    C::Slider mine;
    if (auto style = Resource(view_, L"HeaderIconSliderStyle").try_as<X::Style>())
      mine.Style(style);
    mine.Name(L"NativeQuickVolume");
    mine.MinWidth(0);
    mine.MinHeight(0);
    mine.Height(32);
    mine.Minimum(0);
    mine.Maximum(100);
    mine.StepFrequency(1);
    mine.SmallChange(slider.SmallChange() > 0 ? slider.SmallChange() : 2);
    mine.LargeChange(10);
    mine.IsThumbToolTipEnabled(true);
    mine.Margin(slider.Margin());
    mine.HorizontalAlignment(X::HorizontalAlignment::Stretch);
    mine.VerticalAlignment(X::VerticalAlignment::Center);
    C::Grid::SetColumn(mine, C::Grid::GetColumn(slider));
    C::Grid::SetRow(mine, C::Grid::GetRow(slider));
    C::Grid::SetColumnSpan(mine, C::Grid::GetColumnSpan(slider));
    {
      auto n = std::wstring(X::Automation::AutomationProperties::GetName(slider));
      X::Automation::AutomationProperties::SetName(mine, n.empty() ? L"Volume" : n.c_str());
    }
    mine.Value(slider.Value());
    hide(slider);
    insertAfter(slider, mine);
    quickSlider_ = mine;
    volume_ = mine;
    // Core Audio.
    quickVolume_ = std::make_unique<QuickVolume>();
    try {
      quickVolume_->Open([this](float value, bool muted) { ApplyVolume(value, muted, true); },
                         [this] { RefreshVolume(); });
    } catch (...) {
    }
    RefreshVolume();
    Keep(mine.ValueChanged(winrt::auto_revoke,
                           [this](auto &&, P::RangeBaseValueChangedEventArgs const &args) {
                             if (quickSync_ || !quickVolume_)
                               return;
                             quickUserAt_ = GetTickCount64();
                             auto value = float(args.NewValue() / 100);
                             try {
                               quickVolume_->SetValue(value);
                             } catch (...) {
                               Wh_Log(L"QuickVolumeSetError %08X", winrt::to_hresult());
                             }
                             if (quickIcon_)
                               quickIcon_->Update(value, value <= 0, quickAvailable_);
                           }));
    Keep(mine.PointerWheelChanged(
        winrt::auto_revoke, [this](auto &&, X::Input::PointerRoutedEventArgs const &args) {
          try {
            auto s = quickSlider_;
            if (!s || args.Handled() || !s.IsEnabled())
              return;
            auto point = args.GetCurrentPoint(s);
            if (point.Properties().IsHorizontalMouseWheel())
              return;
            double delta = point.Properties().MouseWheelDelta() / 120.;
            if (!delta)
              return;
            s.Value(std::clamp(s.Value() + delta * s.SmallChange(), s.Minimum(), s.Maximum()));
            args.Handled(true);
          } catch (...) {
          }
        }));
    Keep(own.Click(winrt::auto_revoke, [this](auto &&, auto &&) {
      try {
        if (quickVolume_)
          quickVolume_->ToggleMute();
      } catch (...) {
        Wh_Log(L"QuickMuteToggleError %08X", winrt::to_hresult());
      }
      quickUserAt_ = 0;
      RefreshVolume();
    }));
  }
  void Sliders(C::Grid const &l1) {
    auto group = Named(l1, L"SlidersGroup");
    if (!group)
      return;
    auto grid = Named(group, L"RootGridView").try_as<C::GridView>();
    if (!grid)
      return;
    if (First(grid, 2)) {
      // Top 2.4: the All settings plate 11.2 DIP below the divider, as the
      // Volume mixer heading on Sound. The bottom margin and the brightness row
      // height are corrected by AlignSteps for whole-pixel steps.
      Set(grid, X::FrameworkElement::MarginProperty(),
          winrt::box_value(X::Thickness{12, 2.4, 16, 11.4}));
      grid_ = grid;
      X::Style item(winrt::xaml_typename<C::GridViewItem>());
      if (auto base = grid.ItemContainerStyle())
        item.BasedOn(base);
      // Native slider items are ~46 DIP: -4/-4 gives the 38 DIP step from a header
      // row centre and between native items; the grid's bottom margin takes the 7
      // DIP back, so the distance from the panel edge stays.
      item.Setters().Append(X::Setter(X::FrameworkElement::MarginProperty(),
                                      winrt::box_value(X::Thickness{4, -4, 0, -4})));
      Set(grid, C::ItemsControl::ItemContainerStyleProperty(), item);
      Set(grid, C::ListViewBase::HeaderTransitionsProperty(), M::Animation::TransitionCollection());
      // The grid starts at 12 DIP so the All settings plate (from 12) is not
      // clipped by it; header and native items move back by 4.
      header_ = C::StackPanel();
      header_.Margin({4, 0, 0, 0});
      header_.Children().Append(SettingsRow());
      header_.Children().Append(BrightnessRow());
      Set(grid, C::ListViewBase::HeaderProperty(), header_);
    }
    // The header presenter left-aligns its content by default.
    if (header_ && header_.IsLoaded())
      for (auto p = Parent(header_); p && p != grid; p = Parent(p))
        if (auto presenter = p.try_as<C::ContentControl>()) {
          if (First(presenter, 3))
            Set(presenter, C::Control::HorizontalContentAlignmentProperty(),
                winrt::box_value(X::HorizontalAlignment::Stretch));
          break;
        }
    auto l2 = Named(grid, L"VolumeL2Button").try_as<C::Button>();
    if (!l2)
      return;
    if (First(l2, 4)) {
      SmallButton(l2);
      Set(l2, C::ContentControl::ContentProperty(), Icon(L"sliders-horizontal"));
      Set(l2, C::ToolTipService::ToolTipProperty(), winrt::box_value(L"Sound settings"));
      Set(l2, X::Automation::AutomationProperties::NameProperty(),
          winrt::box_value(L"Sound settings"));
      if (auto row = Parent(l2).try_as<C::Panel>())
        for (auto const &child : row.Children())
          if (auto toggle = child.try_as<P::ToggleButton>())
            SmallButton(toggle);
      // Hiding Quick Settings on the way to Sound (ExitWithSnapshot).
      if (auto original = l2.Command()) {
        auto weakView = winrt::make_weak(view_);
        Set(l2, P::ButtonBase::CommandProperty(),
            winrt::make<DelayedCommand>(original, [weakView, original](IInspectable const &p) {
              auto view = weakView.get();
              X::FrameworkElement l1{nullptr};
              if (view)
                l1 = Named(view, L"L1Grid");
              if (!l1) {
                original.Execute(p);
                return;
              }
              auto media = FindMedia(view);
              // The media widget above fades in parallel; see HoldMedia.
              bool fading = media && media.Visibility() == X::Visibility::Visible &&
                            media.ActualHeight() > 1 && media.Opacity() > 0 &&
                            winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled();
              if (fading) {
                FadeOutNow(media, 90);
                HoldMedia(media, 90);
              }
              revealAt = GetTickCount64();
              Leave(l1, l1, original, p);
            }));
      }
    }
    VolumeIcon(l2);
    // Track starts 12 DIP after the 32 DIP button, as on Sound: 40 DIP column + 8.
    if (auto row = Parent(l2).try_as<C::Panel>())
      for (auto const &child : row.Children())
        if (auto slider = child.try_as<C::Slider>();
            slider && !thinSliders_.count(winrt::get_abi(slider))) {
          auto look = Look();
          look.Geometry(slider);
          look.Thin(slider, [](double half) { return X::Thickness{8 - half, 0, 8 - half, 0}; });
        }
    if (quickSlider_)
      volume_ = quickSlider_;
    else if (auto row = Parent(l2).try_as<C::Panel>())
      for (auto const &child : row.Children())
        if (auto slider = child.try_as<C::Slider>())
          volume_ = slider;
  }
  // The frame grows smoothly on the way to Sound without changing the shell's
  // timings, heights or animations: when ControlCenterRegion gets Sound's
  // height, its visual is clipped to the old height at the bottom and the
  // rounded clip opens upwards in 220 ms (composition, the VM curve). Only after
  // the Sound button was pressed (1.5 s), so VM expansion is unaffected. The
  // shrink back cannot be done this way: a clip cannot show more than the layout.
  static inline ULONGLONG revealAt = 0;
  winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry revealGeometry_{nullptr};
  winrt::Windows::UI::Composition::CompositionRectangleGeometry edgeGeometry_{nullptr};
  winrt::weak_ref<X::FrameworkElement> edgeOwner_, revealRegion_;
  void FrameReveal() {
    X::FrameworkElement region{nullptr};
    for (auto e = Parent(view_); e; e = Parent(e))
      if (e.Name() == L"ControlCenterRegion") {
        region = e;
        break;
      }
    if (!region || !First(region, 11))
      return;
    auto weak = winrt::make_weak(region);
    auto alive = std::make_shared<bool>(true);
    Keep(region.SizeChanged(winrt::auto_revoke, [this, weak,
                                                 alive](auto &&, X::SizeChangedEventArgs const &a) {
      try {
        auto r = weak.get();
        if (!r)
          return;
        double h0 = a.PreviousSize().Height, h1 = a.NewSize().Height;
        float w = a.NewSize().Width;
        namespace Comp = winrt::Windows::UI::Composition;
        using winrt::Windows::Foundation::Numerics::float2;
        auto v = X::Hosting::ElementCompositionPreview::GetElementVisual(r);
        auto c = v.Compositor();
        // The panel's ThemeShadow follows the layout bounds, not the clip. It stays,
        // and the parent of its owner gets a plain clip whose top edge follows the
        // frame's top edge (same curve), wide and deep enough for the shadow on the
        // sides and below, so only the shadow above the growing edge is cut.
        auto shadowParent = [&]() -> X::FrameworkElement {
          for (X::FrameworkElement e = r; e; e = Parent(e))
            if (e.Shadow())
              return Parent(e);
          return Parent(r);
        };
        auto regionTop = [&](X::FrameworkElement const &p) -> float {
          try {
            return float(r.TransformToVisual(p).TransformPoint({0, 0}).Y);
          } catch (...) {
            return 0.f;
          }
        };
        if (revealGeometry_) {
          // With VM collapsed the height changes twice (the shell's L2 minimum
          // first, then the mod's smaller height): any size change keeps the visible
          // bottom and retargets the reveal.
          revealGeometry_.StopAnimation(L"Offset");
          revealGeometry_.StopAnimation(L"Size");
          auto o = revealGeometry_.Offset();
          auto z = revealGeometry_.Size();
          float top = std::max(0.f, o.y + float(h1 - h0));
          revealGeometry_.Offset({0, top});
          revealGeometry_.Size({w, std::max(0.f, std::min(z.y, float(h1) - top))});
          if (edgeGeometry_)
            if (auto p = edgeOwner_.get()) {
              edgeGeometry_.StopAnimation(L"Offset");
              edgeGeometry_.Offset({-64.f, regionTop(p) + top});
            }
        } else {
          if (!revealAt || GetTickCount64() - revealAt > 1500 || h0 <= 0 || h1 - h0 < 1 ||
              !winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled())
            return;
          revealAt = 0;
          revealGeometry_ = c.CreateRoundedRectangleGeometry();
          float radius = 8;
          if (auto g = r.try_as<C::Grid>())
            if (g.CornerRadius().TopLeft > 0)
              radius = float(g.CornerRadius().TopLeft);
          revealGeometry_.CornerRadius({radius, radius});
          revealGeometry_.Offset({0, float(h1 - h0)});
          revealGeometry_.Size({w, float(h0)});
          v.Clip(c.CreateGeometricClip(revealGeometry_));
          if (auto p = shadowParent()) {
            auto pv = X::Hosting::ElementCompositionPreview::GetElementVisual(p);
            edgeGeometry_ = c.CreateRectangleGeometry();
            edgeGeometry_.Offset({-64.f, regionTop(p) + float(h1 - h0)});
            edgeGeometry_.Size({float(p.ActualWidth()) + 128.f, float(p.ActualHeight()) + 4000.f});
            pv.Clip(c.CreateGeometricClip(edgeGeometry_));
            edgeOwner_ = winrt::make_weak(p);
          }
        }
        auto ease = c.CreateCubicBezierEasingFunction(float2{.1f, .9f}, float2{.2f, 1.f});
        auto duration = std::chrono::milliseconds(220);
        auto batch = c.CreateScopedBatch(Comp::CompositionBatchTypes::Animation);
        auto offset = c.CreateVector2KeyFrameAnimation();
        offset.InsertKeyFrame(1, float2{0, 0}, ease);
        offset.Duration(duration);
        auto size = c.CreateVector2KeyFrameAnimation();
        size.InsertKeyFrame(1, float2{w, float(h1)}, ease);
        size.Duration(duration);
        revealGeometry_.StartAnimation(L"Offset", offset);
        revealGeometry_.StartAnimation(L"Size", size);
        if (edgeGeometry_)
          if (auto p = edgeOwner_.get()) {
            auto edge = c.CreateVector2KeyFrameAnimation();
            edge.InsertKeyFrame(1, float2{-64.f, regionTop(p)}, ease);
            edge.Duration(duration);
            edgeGeometry_.StartAnimation(L"Offset", edge);
          }
        batch.End();
        auto geometry = revealGeometry_;
        batch.Completed([this, geometry, alive](auto &&, auto &&) {
          if (!*alive || revealGeometry_ != geometry)
            return;
          EndReveal();
        });
      } catch (...) {
        Wh_Log(L"FrameRevealError %08X", winrt::to_hresult());
      }
    }));
    revealRegion_ = weak;
    revoke_.push_back([this, alive] {
      *alive = false;
      EndReveal();
    });
  }
  void EndReveal() {
    revealGeometry_ = nullptr;
    edgeGeometry_ = nullptr;
    if (auto r = revealRegion_.get())
      try {
        X::Hosting::ElementCompositionPreview::GetElementVisual(r).Clip(nullptr);
      } catch (...) {
      }
    if (auto p = edgeOwner_.get())
      try {
        X::Hosting::ElementCompositionPreview::GetElementVisual(p).Clip(nullptr);
      } catch (...) {
      }
    edgeOwner_ = nullptr;
  }
  void Ensure() {
    if (restoring_ || !view_)
      return;
    dirty_ = false;
    lastEnsure_ = GetTickCount64();
    try {
      auto l1 = Named(view_, L"L1Grid").try_as<C::Grid>();
      if (!l1)
        return;
      RefreshVolume();
      // Back from Sound: L1Grid visible again; the transparency set for the
      // hiding picture (ExitWithSnapshot) and the mute glyph state are refreshed.
      if (First(l1, 9)) {
        auto weak = winrt::make_weak(l1);
        auto token = l1.RegisterPropertyChangedCallback(
            X::UIElement::VisibilityProperty(), [this, weak](auto &&, auto &&) {
              try {
                auto g = weak.get();
                if (!g || g.Visibility() != X::Visibility::Visible)
                  return;
                RefreshVolume();
                if (g.Opacity() == 0)
                  g.Opacity(1);
                if (auto media = FindMedia(view_)) {
                  if (media.Opacity() == 0)
                    media.Opacity(1);
                  ShowAgain(media);
                }
                for (auto const &child : g.Children())
                  if (child.Opacity() == 0)
                    child.Opacity(1);
                ShowAgain(g);
              } catch (...) {
              }
            });
        revoke_.push_back([weak, token] {
          if (auto g = weak.get())
            try {
              g.UnregisterPropertyChangedCallback(X::UIElement::VisibilityProperty(), token);
            } catch (...) {
            }
        });
      }
      DarkBlock(l1);
      Pager();
      Sliders(l1);
      Footer();
      FrameReveal();
      if (bright_ && bright_.IsLoaded() && !thinSliders_.count(winrt::get_abi(bright_))) {
        auto look = Look();
        look.Geometry(bright_);
        look.Thin(bright_, [](double half) { return X::Thickness{8 - half, 0, 8 - half, 0}; });
      }
    } catch (...) {
      Wh_Log(L"Quick Settings layout: %08X", winrt::to_hresult());
    }
  }

public:
  QuickPage(X::FrameworkElement const &view, HWND host) : view_(view), host_(host) {
    try {
      ddc_ = std::make_unique<Ddc>();
      monitor_ = MonitorFromWindow(host_, MONITOR_DEFAULTTOPRIMARY);
      ddc_->Read(monitor_);
    } catch (...) {
      Wh_Log(L"QuickDdcError %08X", winrt::to_hresult());
    }
    // Before the first frame of every opening: new templates appear while the
    // panel is laid out (Track marks the page dirty); LayoutUpdated precedes render.
    // Not applied here: the constructor runs inside the tree-change callback.
    // Opening: re-read the monitor brightness once the panel becomes visible.
    // New templates are not announced without tree reports: Ensure also runs
    // every 250 ms of layout activity (opening, paging, page changes).
    Keep(view_.LayoutUpdated(winrt::auto_revoke, [this](auto &&, auto &&) {
      if (dirty_ || GetTickCount64() - lastEnsure_ > 250)
        Ensure();
      try {
        AlignSteps();
      } catch (...) {
      }
      bool shown = PanelVisible();
      if (shown && !visible_) {
        visible_ = true;
        try {
          PollNight();
          Brightness(true);
        } catch (...) {
        }
      } else
        visible_ = shown;
    }));
  }
  X::FrameworkElement View() const { return view_; }
  void Dirty() { dirty_ = true; }
  void Refresh() {
    visible_ = PanelVisible();
    if (dirty_ || visible_)
      Ensure();
    try {
      PollNight();
      Brightness(false);
    } catch (...) {
    }
  }
  void Restore() {
    if (restoring_)
      return;
    restoring_ = true;
    if (quickVolume_) {
      quickVolume_->Close();
      quickVolume_.reset();
    }
    quickSlider_ = nullptr;
    quickMute_ = nullptr;
    quickIcon_ = nullptr;
    if (brightIdle_) {
      brightIdle_.Stop();
      brightIdle_.Tick(brightIdleTick_);
      brightIdle_ = nullptr;
    }
    ddc_.reset();
    for (auto &r : revoke_)
      try {
        r();
      } catch (...) {
      }
    revoke_.clear();
    journal_.Restore();
    view_ = nullptr;
  }
  ~QuickPage() { Restore(); }
};
} // namespace native

// Public visual-tree discovery from ControlCenter.dll COM-interface hooks.
// No XAML Diagnostics connection, TAP, or diagnostic instance handles.
namespace native {
inline HWND ThreadWindow(DWORD id) {
  std::vector<HWND> found;
  EnumThreadWindows(
      id,
      [](HWND w, LPARAM p) -> BOOL {
        WCHAR name[96]{};
        GetClassNameW(w, name, 96);
        if (!_wcsicmp(name, L"ControlCenterWindow") ||
            !_wcsicmp(name, L"Windows.UI.Core.CoreWindow"))
          reinterpret_cast<std::vector<HWND> *>(p)->push_back(w);
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&found));
  for (auto handle : found) {
    WCHAR name[96]{};
    GetClassNameW(handle, name, ARRAYSIZE(name));
    if (!_wcsicmp(name, L"ControlCenterWindow"))
      return handle;
  }
  return found.empty() ? nullptr : found.front();
}
inline UINT dispatchMessage = RegisterWindowMessageW(L"Windhawk.SoundNative.Dispatch.2");
struct Job {
  std::function<void()> action;
  bool succeeded = false;
};
inline std::mutex jobsMutex;
inline std::map<UINT_PTR, Job *> jobs;
inline std::atomic<UINT_PTR> nextJob = 0;
inline LRESULT CALLBACK DispatchHook(int code, WPARAM w, LPARAM l) {
  if (code >= 0) {
    auto msg = reinterpret_cast<CWPSTRUCT *>(l);
    if (msg->message == dispatchMessage) {
      Job *job = nullptr;
      {
        std::lock_guard lock(jobsMutex);
        auto i = jobs.find(msg->wParam);
        if (i != jobs.end())
          job = i->second;
      }
      if (job)
        try {
          job->action();
          job->succeeded = true;
        } catch (...) {
          Wh_Log(L"UI dispatch: %08X", winrt::to_hresult());
        }
    }
  }
  return CallNextHookEx(nullptr, code, w, l);
}
inline bool Dispatch(HWND window, std::function<void()> action) {
  auto thread = GetWindowThreadProcessId(window, nullptr);
  if (!thread)
    return false;
  if (thread == GetCurrentThreadId()) {
    try {
      action();
      return true;
    } catch (...) {
      Wh_Log(L"UI dispatch: %08X", winrt::to_hresult());
      return false;
    }
  }
  auto hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, module, thread);
  if (!hook)
    return false;
  Job job{std::move(action)};
  auto cookie = ++nextJob;
  {
    std::lock_guard lock(jobsMutex);
    jobs[cookie] = &job;
  }
  SendMessageW(window, dispatchMessage, cookie, 0);
  {
    std::lock_guard lock(jobsMutex);
    jobs.erase(cookie);
  }
  UnhookWindowsHookEx(hook);
  return job.succeeded;
}

struct Runtime;
inline void CALLBACK PanelShown(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
inline std::mutex runtimesMutex;
inline std::map<DWORD, Runtime *> runtimes;
struct Runtime {
  HWND window = nullptr, pulseWindow = nullptr;
  winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
  HWINEVENTHOOK showHook = nullptr, uncloakHook = nullptr;
  X::DispatcherTimer timer{nullptr}, discoveryTimer{nullptr};
  winrt::event_token tick{}, discoveryTick{};
  std::vector<winrt::weak_ref<X::FrameworkElement>> discoveryCandidates;
  unsigned discoveryAttempts = 0;
  bool busy = false, discovering = false;
  winrt::weak_ref<X::FrameworkElement> l2frame;
  int64_t frameToken = 0;
  std::vector<std::unique_ptr<SoundPage>> pages;
  std::vector<std::unique_ptr<QuickPage>> quickPages;
  struct PageWatch {
    winrt::weak_ref<X::FrameworkElement> page;
    winrt::event_token loaded{}, layout{};
  };
  std::vector<PageWatch> pageWatches;
  struct ViewWatch {
    winrt::weak_ref<X::FrameworkElement> view;
    winrt::event_token loaded{}, layout{};
  };
  std::vector<ViewWatch> viewWatches;
  std::shared_ptr<VolumePulse> volumePulse;
  bool volumeDuringRefresh = false;
  bool Hidden() const { return !WindowShown(window); }
  void Wake() {
    if (timer && !stopping) {
      timer.Interval(std::chrono::milliseconds(200));
      timer.Start();
    }
  }
  static LRESULT CALLBACK VolumeProc(HWND handle, UINT message, WPARAM w, LPARAM l,
                                     DWORD_PTR reference) {
    auto self = reinterpret_cast<Runtime *>(reference);
    if (message == volumePulseMessage) {
      if (self->volumePulse)
        self->volumePulse->queued = false;
      if (!stopping)
        try {
          if (self->busy)
            self->volumeDuringRefresh = true;
          else
            for (auto const &page : self->pages)
              page->RefreshMasterNow();
        } catch (...) {
          Wh_Log(L"MasterPulseError %08X", winrt::to_hresult());
        }
      return 0;
    }
    if (message == WM_NCDESTROY) {
      if (self->pulseWindow == handle) {
        if (self->volumePulse)
          self->volumePulse->active = false;
        self->pulseWindow = nullptr;
      }
      if (self->window == handle)
        self->window = nullptr;
      WindhawkUtils::RemoveWindowSubclassFromAnyThread(handle, VolumeProc);
    }
    return DefSubclassProc(handle, message, w, l);
  }
  void EnsureVolumePulse() {
    HWND next = window && IsWindow(window) ? window : ThreadWindow(GetCurrentThreadId());
    if (!next || GetWindowThreadProcessId(next, nullptr) != GetCurrentThreadId())
      return;
    window = next;
    if (pulseWindow == next && IsWindow(pulseWindow))
      return;
    if (volumePulse)
      volumePulse->active = false;
    if (pulseWindow && IsWindow(pulseWindow))
      WindhawkUtils::RemoveWindowSubclassFromAnyThread(pulseWindow, VolumeProc);
    pulseWindow = nullptr;
    auto fresh = std::make_shared<VolumePulse>(next);
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(next, VolumeProc,
                                                       reinterpret_cast<DWORD_PTR>(this))) {
      fresh->active = false;
      Wh_Log(L"Could not subclass the Control Center window");
      return;
    }
    pulseWindow = next;
    volumePulse = std::move(fresh);
  }
  explicit Runtime(X::FrameworkElement const &anchor)
      : window(ThreadWindow(GetCurrentThreadId())), dispatcher(anchor.Dispatcher()) {
    timer = X::DispatcherTimer();
    timer.Interval(std::chrono::milliseconds(200));
    tick = timer.Tick([this](auto &&, auto &&) { Refresh(); });
    discoveryTimer = X::DispatcherTimer();
    discoveryTimer.Interval(std::chrono::milliseconds(1));
    discoveryTick = discoveryTimer.Tick([this](auto &&, auto &&) { DiscoverPending(); });
    DWORD process = GetCurrentProcessId(), thread = GetCurrentThreadId();
    showHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, PanelShown, process,
                               thread, WINEVENT_OUTOFCONTEXT);
    uncloakHook = SetWinEventHook(0x8018 /*EVENT_OBJECT_UNCLOAKED*/, 0x8018, nullptr, PanelShown,
                                  process, thread, WINEVENT_OUTOFCONTEXT);
    std::lock_guard lock(runtimesMutex);
    runtimes[thread] = this;
  }
  void OnFrameContent(X::DependencyObject const &sender) {
    auto frame = sender.try_as<C::ContentControl>();
    if (!frame)
      return;
    auto page = frame.Content().try_as<X::FrameworkElement>();
    if (!page || Class(page).find(L"ControlCenter.FullScreenPage") == std::wstring::npos)
      return;
    WatchPage(page);
    Wake();
    if (page.IsLoaded())
      BeforeDisplay(page, true);
  }
  void WatchFrame(X::FrameworkElement const &frame) {
    if (auto current = l2frame.get(); current && current == frame)
      return;
    UnwatchFrame();
    if (!frame.try_as<C::ContentControl>())
      return;
    l2frame = winrt::make_weak(frame);
    frameToken = frame.RegisterPropertyChangedCallback(
        C::ContentControl::ContentProperty(),
        [this](X::DependencyObject const &sender, X::DependencyProperty const &) {
          try {
            OnFrameContent(sender);
          } catch (...) {
          }
        });
    OnFrameContent(frame);
  }
  void UnwatchFrame() {
    if (auto frame = l2frame.get(); frame && frameToken)
      try {
        frame.UnregisterPropertyChangedCallback(C::ContentControl::ContentProperty(), frameToken);
      } catch (...) {
      }
    l2frame = nullptr;
    frameToken = 0;
  }
  void PreparePage(X::FrameworkElement const &page, bool opening) {
    if (!page.IsLoaded() || Retired(page))
      return;
    for (auto const &item : pages)
      if (item->Page() == page) {
        if (opening)
          item->PrepareInitialLayout();
        else
          item->BeforeRender();
        return;
      }
    if (!Named(page, L"OutputGroupTitle"))
      return;
    auto pulse = volumePulse;
    auto patch = std::make_unique<SoundPage>(page, window, [pulse] {
      if (pulse)
        pulse->Post();
    });
    if (patch->Apply()) {
      patch->PrepareInitialLayout();
      pages.push_back(std::move(patch));
      Wh_Log(L"Sound page attached through ControlCenter symbols");
    }
  }
  void BeforeDisplay(X::FrameworkElement const &page, bool opening) noexcept {
    if (stopping || busy)
      return;
    busy = true;
    try {
      EnsureVolumePulse();
      RetirePending();
      PreparePage(page, opening);
    } catch (...) {
      Wh_Log(L"Initial native layout: %08X", winrt::to_hresult());
    }
    busy = false;
    if (volumeDuringRefresh) {
      volumeDuringRefresh = false;
      if (volumePulse)
        volumePulse->Post();
    }
  }
  // Windows builds a new Sound FullScreenPage for every opening and the old one
  // stays loaded. If every old page kept its patch, timers and layout handlers,
  // each opening would get slower. A newly created page retires all others:
  // patch restored, handlers removed, never patched again.
  std::vector<winrt::weak_ref<X::FrameworkElement>> retired;
  bool Retired(X::FrameworkElement const &page) {
    for (auto it = retired.begin(); it != retired.end();) {
      auto p = it->get();
      if (!p) {
        it = retired.erase(it);
        continue;
      }
      if (p == page)
        return true;
      ++it;
    }
    return false;
  }
  void RetireOthers(X::FrameworkElement const &page) {
    for (auto it = pageWatches.begin(); it != pageWatches.end();) {
      auto existing = it->page.get();
      if (existing && existing == page) {
        ++it;
        continue;
      }
      if (existing) {
        existing.Loaded(it->loaded);
        existing.LayoutUpdated(it->layout);
        retired.push_back(it->page);
      }
      it = pageWatches.erase(it);
    }
    for (auto it = pages.begin(); it != pages.end();) {
      if ((*it)->Page() == page) {
        ++it;
        continue;
      }
      if (auto old = (*it)->Page())
        retired.push_back(winrt::make_weak(old));
      (*it)->Abandon();
      it = pages.erase(it);
    }
  }
  winrt::weak_ref<X::FrameworkElement> newestPage;
  // Same for Quick Settings, in case the shell also rebuilds ControlCenterView.
  winrt::weak_ref<X::FrameworkElement> newestQuick;
  void RetireQuick() {
    auto view = newestQuick.get();
    if (!view)
      return;
    newestQuick = nullptr;
    for (auto it = quickPages.begin(); it != quickPages.end();) {
      if ((*it)->View() == view) {
        ++it;
        continue;
      }
      (*it)->Restore();
      it = quickPages.erase(it);
    }
  }
  void RetirePending() {
    if (auto page = newestPage.get()) {
      newestPage = nullptr;
      RetireOthers(page);
    }
  }
  void WatchPage(X::FrameworkElement const &page) {
    if (Retired(page))
      return;
    for (auto it = pageWatches.begin(); it != pageWatches.end();)
      if (auto existing = it->page.get()) {
        if (existing == page)
          return;
        ++it;
      } else
        it = pageWatches.erase(it);
    // Retirement is deferred until BeforeDisplay or Refresh. BeforeDisplay
    // (Loaded/LayoutUpdated of the new page) and Refresh retire the others.
    newestPage = winrt::make_weak(page);
    auto weak = winrt::make_weak(page);
    PageWatch watch;
    watch.page = weak;
    watch.loaded = page.Loaded([this, weak](auto &&, auto &&) {
      if (auto page = weak.get())
        BeforeDisplay(page, true);
    });
    watch.layout = page.LayoutUpdated([this, weak](auto &&, auto &&) {
      if (auto page = weak.get())
        BeforeDisplay(page, false);
    });
    pageWatches.push_back(std::move(watch));
  }

  void WatchView(X::FrameworkElement const &view) {
    for (auto it = viewWatches.begin(); it != viewWatches.end();) {
      if (auto existing = it->view.get()) {
        if (existing == view)
          return;
        ++it;
      } else
        it = viewWatches.erase(it);
    }
    auto weak = winrt::make_weak(view);
    auto retry = [this, weak](auto &&, auto &&) {
      if (stopping || discovering || l2frame.get())
        return;
      if (auto view = weak.get())
        Discover(view);
    };
    ViewWatch watch{weak};
    watch.loaded = view.Loaded(retry);
    watch.layout = view.LayoutUpdated(retry);
    viewWatches.push_back(std::move(watch));
  }
  void Track(X::FrameworkElement const &element) {
    auto type = Class(element);
    if (type == L"ControlCenter.ControlCenterView") {
      WatchView(element);
      bool known = false;
      for (auto const &q : quickPages)
        if (q->View() == element) {
          q->Dirty();
          known = true;
          break;
        }
      if (!known) {
        quickPages.push_back(std::make_unique<QuickPage>(element, window));
        newestQuick = winrt::make_weak(element);
        Wh_Log(L"Control Center view discovered through symbols");
      }
    }
    if (element.Name() == L"L2Frame")
      WatchFrame(element);
    if (type.find(L"ControlCenter.FullScreenPage") != std::wstring::npos) {
      WatchPage(element);
      if (element.IsLoaded())
        BeforeDisplay(element, false);
    }
  }
  bool Discover(X::FrameworkElement const &element) {
    if (!element || stopping || discovering)
      return false;
    discovering = true;
    bool found = false;
    try {
      auto root = element;
      if (auto xaml = element.XamlRoot()) {
        if (auto content = xaml.Content().try_as<X::FrameworkElement>())
          root = content;
      } else {
        for (int depth = 0; depth < 64; ++depth) {
          auto parent = Parent(root);
          if (!parent)
            break;
          root = parent;
        }
      }
      std::vector<X::FrameworkElement> targets;
      Find(root, [&](auto const &item) {
        auto type = Class(item);
        if (type == L"ControlCenter.ControlCenterView" || item.Name() == L"L2Frame" ||
            type.find(L"ControlCenter.FullScreenPage") != std::wstring::npos)
          targets.push_back(item);
        return false;
      });
      // Tree traversal finishes before patching anything in that tree.
      EnsureVolumePulse();
      for (auto const &target : targets) {
        Track(target);
        found = true;
      }
      Wake();
      Refresh();
    } catch (...) {
      Wh_Log(L"Control Center discovery: %08X", winrt::to_hresult());
    }
    discovering = false;
    return found;
  }
  void QueueDiscovery(X::FrameworkElement const &child) {
    if (stopping)
      return;
    // Connect runs inside InitializeComponent: keep only a weak reference to
    // its completed child, and never query or retain the half-constructed view.
    if (discoveryCandidates.size() < 64)
      discoveryCandidates.push_back(winrt::make_weak(child));
    if (!discoveryAttempts) {
      discoveryAttempts = 1;
      discoveryTimer.Interval(std::chrono::milliseconds(1));
      discoveryTimer.Start();
    }
  }
  void DiscoverPending() {
    discoveryTimer.Stop();
    if (stopping) {
      discoveryCandidates.clear();
      discoveryAttempts = 0;
      return;
    }
    auto candidates = std::move(discoveryCandidates);
    discoveryCandidates.clear();
    bool found = false;
    for (auto const &weak : candidates)
      if (auto child = weak.get())
        if (Discover(child)) {
          found = true;
          break;
        }
    if (found || discoveryAttempts++ >= 8) {
      discoveryCandidates.clear();
      discoveryAttempts = 0;
    } else {
      for (auto &weak : candidates)
        if (discoveryCandidates.size() < 64)
          discoveryCandidates.push_back(std::move(weak));
      discoveryTimer.Interval(std::chrono::milliseconds(16));
      discoveryTimer.Start();
    }
  }
  void Refresh() {
    if (stopping || busy)
      return;
    busy = true;
    try {
      EnsureVolumePulse();
      RetirePending();
      RetireQuick();
      for (auto it = pages.begin(); it != pages.end();)
        if (!(*it)->Alive()) {
          (*it)->Abandon();
          it = pages.erase(it);
        } else {
          (*it)->Refresh();
          ++it;
        }
      for (auto const &q : quickPages)
        q->Refresh();
    } catch (...) {
      Wh_Log(L"Native layout: %08X", winrt::to_hresult());
    }
    if (timer) {
      if (Hidden() && showHook && uncloakHook)
        timer.Stop();
      else {
        auto interval = std::chrono::milliseconds(Hidden() ? 2000 : 200);
        if (timer.Interval() != winrt::Windows::Foundation::TimeSpan(interval))
          timer.Interval(interval);
      }
    }
    busy = false;
    if (volumeDuringRefresh) {
      volumeDuringRefresh = false;
      if (volumePulse)
        volumePulse->Post();
    }
  }
  void Stop() {
    if (timer) {
      timer.Stop();
      timer.Tick(tick);
    }
    if (discoveryTimer) {
      discoveryTimer.Stop();
      discoveryTimer.Tick(discoveryTick);
    }
    discoveryCandidates.clear();
    for (auto hook : {showHook, uncloakHook})
      if (hook)
        UnhookWinEvent(hook);
    showHook = uncloakHook = nullptr;
    if (volumePulse)
      volumePulse->active = false;
    if (pulseWindow && IsWindow(pulseWindow))
      WindhawkUtils::RemoveWindowSubclassFromAnyThread(pulseWindow, VolumeProc);
    pulseWindow = nullptr;
    UnwatchFrame();
    for (auto const &watch : viewWatches)
      if (auto view = watch.view.get()) {
        view.Loaded(watch.loaded);
        view.LayoutUpdated(watch.layout);
      }
    viewWatches.clear();
    for (auto const &watch : pageWatches)
      if (auto page = watch.page.get()) {
        page.Loaded(watch.loaded);
        page.LayoutUpdated(watch.layout);
      }
    pageWatches.clear();
    for (auto &q : quickPages)
      q->Restore();
    quickPages.clear();
    for (auto &page : pages)
      page->Restore();
    pages.clear();
    modelMembers.clear();
    timer = nullptr;
    discoveryTimer = nullptr;
    std::lock_guard lock(runtimesMutex);
    if (auto it = runtimes.find(GetCurrentThreadId()); it != runtimes.end() && it->second == this)
      runtimes.erase(it);
  }
};
// UI-thread state is explicitly stopped on unload; no XAML destruction during
// process/thread termination after the other threads have already disappeared.
[[clang::no_destroy]] inline thread_local std::unique_ptr<Runtime> runtime;
inline void CALLBACK PanelShown(HWINEVENTHOOK, DWORD, HWND handle, LONG object, LONG, DWORD,
                                DWORD) {
  if (stopping || object != OBJID_WINDOW || !runtime || !handle)
    return;
  WCHAR name[96]{};
  GetClassNameW(handle, name, ARRAYSIZE(name));
  if (_wcsicmp(name, L"ControlCenterWindow"))
    return;
  // This is only the visibility handle. EnsureVolumePulse owns pulseWindow.
  runtime->window = handle;
  runtime->Wake();
}
inline void StopBridge() {
  std::vector<std::pair<DWORD, Runtime *>> copy;
  {
    std::lock_guard lock(runtimesMutex);
    for (auto pair : runtimes)
      copy.push_back(pair);
  }
  auto stop = [] {
    if (runtime) {
      runtime->Stop();
      runtime.reset();
    }
  };
  for (auto [thread, state] : copy) {
    auto threadHandle = OpenThread(SYNCHRONIZE, FALSE, thread);
    bool exited = threadHandle && WaitForSingleObject(threadHandle, 0) == WAIT_OBJECT_0;
    if (threadHandle)
      CloseHandle(threadHandle);
    if (exited) {
      // Its UI callbacks cannot run; do not release XAML references off-thread.
      Wh_Log(L"Control Center UI thread %u has exited; retaining its XAML state", thread);
      continue;
    }
    HWND handle = state->pulseWindow && IsWindow(state->pulseWindow) ? state->pulseWindow
                                                                     : ThreadWindow(thread);
    if (handle && GetWindowThreadProcessId(handle, nullptr) == thread && Dispatch(handle, stop))
      continue;
    // A dispatcher shutting down can complete without invoking the delegate.
    // Keep unreachable thread-affine state, but never pin this mod's DLL.
    auto dispatcher = state->dispatcher;
    if (!DispatchCleanup(dispatcher, stop))
      Wh_Log(L"Control Center UI thread %u is shutting down; retaining its XAML state", thread);
  }
}
} // namespace native

namespace native {
// Discovery uses only COM produce<> entry points, never the implementation's
// partially constructed `this`. Pattern validated in ControlCenter.pdb.
using Focus = int(WINAPI *)(void *, void *);
using Connect = int(WINAPI *)(void *, int, void *);
inline Focus originalViewFocus = nullptr, originalPageFocus = nullptr;
inline Connect originalViewConnect = nullptr, originalPageConnect = nullptr;
inline std::atomic<int> discoveryState{0}; // 0 waiting, 1 installing, 2 ready, 3 failed
inline std::mutex discoveryMutex;
inline std::mutex discoveryInstallMutex;
inline thread_local bool insideDiscovery = false;
inline void Discovered(void *object, bool deferred) noexcept {
  if (!object || stopping || insideDiscovery)
    return;
  insideDiscovery = true;
  try {
    X::FrameworkElement element{nullptr};
    if (SUCCEEDED(static_cast<::IUnknown *>(object)->QueryInterface(
            winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(element))) &&
        element) {
      {
        // Unload crosses this lock before taking the runtime snapshot. It is
        // never held while the engine waits for work on the UI thread.
        std::lock_guard lock(discoveryMutex);
        if (!stopping && !runtime)
          runtime = std::make_unique<Runtime>(element);
      }
      if (!stopping && runtime) {
        if (deferred)
          runtime->QueueDiscovery(element);
        else
          runtime->Discover(element);
      }
    }
  } catch (...) {
    Wh_Log(L"ControlCenter symbol callback: %08X", winrt::to_hresult());
  }
  insideDiscovery = false;
}
inline int WINAPI ViewFocus(void *self, void *args) {
  auto result = originalViewFocus(self, args);
  Discovered(self, false);
  return result;
}
inline int WINAPI PageFocus(void *self, void *args) {
  auto result = originalPageFocus(self, args);
  Discovered(self, false);
  return result;
}
inline int WINAPI ViewConnect(void *self, int id, void *target) {
  auto result = originalViewConnect(self, id, target);
  Discovered(target, true);
  return result;
}
inline int WINAPI PageConnect(void *self, int id, void *target) {
  auto result = originalPageConnect(self, id, target);
  Discovered(target, true);
  return result;
}
inline bool HookControlCenter(HMODULE controlCenter, bool applyNow) {
  std::lock_guard installLock(discoveryInstallMutex);
  if (stopping)
    return false;
  int expected = 0;
  if (!discoveryState.compare_exchange_strong(expected, 1))
    return expected != 3;
  WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
      {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
       &originalViewFocus,
       ViewFocus},
      {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
       &originalViewConnect,
       ViewConnect,
       true},
      {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
       &originalPageFocus,
       PageFocus,
       true},
      {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
       &originalPageConnect,
       PageConnect,
       true},
  };
  bool ok = WindhawkUtils::HookSymbols(controlCenter, controlCenterDllHooks,
                                       ARRAYSIZE(controlCenterDllHooks));
  if (ok && applyNow)
    ok = Wh_ApplyHookOperations();
  discoveryState = ok ? 2 : 3;
  Wh_Log(L"ControlCenter.dll discovery hooks: %s", ok ? L"ready" : L"failed");
  return ok;
}
using HostLoad = decltype(&LoadLibraryExW);
inline HostLoad originalHostLoad = nullptr;
inline HMODULE WINAPI HostLoaded(LPCWSTR path, HANDLE file, DWORD flags) {
  auto result = originalHostLoad(path, file, flags);
  if (result && !stopping && discoveryState == 0 &&
      !(flags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                 LOAD_LIBRARY_AS_IMAGE_RESOURCE)) &&
      result == GetModuleHandleW(L"ControlCenter.dll"))
    HookControlCenter(result, true);
  return result;
}
inline bool InitializeHost() {
  auto kernel = GetModuleHandleW(L"kernelbase.dll");
  auto load =
      kernel ? reinterpret_cast<HostLoad>(GetProcAddress(kernel, "LoadLibraryExW")) : nullptr;
  if (!load || !WindhawkUtils::SetFunctionHook(load, HostLoaded, &originalHostLoad))
    return false;
  if (auto controlCenter = GetModuleHandleW(L"ControlCenter.dll"))
    return HookControlCenter(controlCenter, false);
  Wh_Log(L"Waiting for ControlCenter.dll to be loaded by the shell");
  return true;
}
inline void AfterHostInit() {
  // Close the race where the DLL loaded before the load hook was armed.
  if (!stopping)
    if (auto controlCenter = GetModuleHandleW(L"ControlCenter.dll"))
      HookControlCenter(controlCenter, true);
}
inline void StopHost() {
  {
    std::lock_guard installLock(discoveryInstallMutex);
    // Finish any symbol installation before the module can be unloaded.
  }
  {
    std::lock_guard lock(discoveryMutex);
    // stopping is already set by Wh_ModUninit; wait only for runtime creation.
  }
  StopBridge();
}
} // namespace native



#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <optional>
namespace native::tray {
inline std::atomic<bool> stopping{false}, moduleHooked{false};

// Globals that own threads, COM objects or XAML references are never destroyed
// by the C++ runtime: Shutdown releases them explicitly, and destructors run at
// process exit after other modules are gone.
[[clang::no_destroy]] inline std::optional<sound::Audio> audio; // emplaced in Initialize, reset in Shutdown
// Display on / resume: the volume icons are rebuilt (their Lottie picture can
// come back on its Mute frame), on each taskbar's own thread via the pulse.
[[clang::no_destroy]] inline std::optional<sound::PowerWake> powerWake;
inline std::atomic<unsigned> redrawGeneration{0}; // each record redraws once per wake
inline HINSTANCE moduleInstance = nullptr;
struct Job {
  std::function<void()> call;
};
inline std::mutex jobsMutex, recordsMutex;
inline std::map<UINT_PTR, std::shared_ptr<Job>> jobs;
inline std::atomic<UINT_PTR> nextJob{0};
inline UINT dispatchMessage = RegisterWindowMessageW(L"Windhawk.SoundPanel.ThreadDispatch.1");
inline LRESULT CALLBACK DispatchHook(int code, WPARAM w, LPARAM l) {
  if (code >= 0) {
    auto message = reinterpret_cast<CWPSTRUCT *>(l);
    if (message->message == dispatchMessage) {
      std::shared_ptr<Job> job;
      {
        std::lock_guard lock(jobsMutex);
        auto it = jobs.find(message->wParam);
        if (it != jobs.end())
          job = it->second;
      }
      if (job)
        try {
          job->call();
        } catch (winrt::hresult_error const &ex) {
          Wh_Log(L"Tray UI error 0x%08X", ex.code().value);
        }
    }
  }
  return CallNextHookEx(nullptr, code, w, l);
}
inline void Dispatch(HWND target, std::function<void()> action) {
  DWORD thread = GetWindowThreadProcessId(target, nullptr);
  if (!thread)
    return;
  if (thread == GetCurrentThreadId()) {
    action();
    return;
  }
  auto hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, moduleInstance, thread);
  if (!hook) {
    Wh_Log(L"Tray dispatch hook failed: %u", GetLastError());
    return;
  }
  auto job = std::make_shared<Job>();
  job->call = std::move(action);
  auto cookie = ++nextJob;
  {
    std::lock_guard lock(jobsMutex);
    jobs.emplace(cookie, job);
  }
  // Synchronous dispatch is deliberate: no DLL callback may outlive unload.
  SendMessageW(target, dispatchMessage, cookie, 0);
  {
    std::lock_guard lock(jobsMutex);
    jobs.erase(cookie);
  }
  UnhookWindowsHookEx(hook);
}
inline std::vector<HWND> Taskbars() {
  std::vector<HWND> result;
  EnumWindows(
      [](HWND window, LPARAM data) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        if (pid != GetCurrentProcessId())
          return TRUE;
        WCHAR name[64]{};
        GetClassNameW(window, name, 64);
        if (!_wcsicmp(name, L"Shell_TrayWnd") || !_wcsicmp(name, L"Shell_SecondaryTrayWnd"))
          reinterpret_cast<std::vector<HWND> *>(data)->push_back(window);
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&result));
  return result;
}
inline void *siteVtable = nullptr;
using GetHost = void *(WINAPI *)(void *, void **);
inline GetHost getHost = nullptr;
using Decref = void(WINAPI *)(void *);
inline Decref decref = nullptr;
inline void *frameHeight = nullptr;
inline bool Readable(void const *pointer, size_t bytes) {
  MEMORY_BASIC_INFORMATION info{};
  if (!pointer || !VirtualQuery(pointer, &info, sizeof(info)) || info.State != MEM_COMMIT ||
      info.Protect & (PAGE_GUARD | PAGE_NOACCESS))
    return false;
  auto address = reinterpret_cast<uintptr_t>(pointer),
       end = reinterpret_cast<uintptr_t>(info.BaseAddress) + info.RegionSize;
  return bytes <= end - address;
}
inline X::XamlRoot Root(HWND taskbar) {
  if (!getHost || !siteVtable || !decref || !frameHeight)
    return nullptr;
  auto bandWindow = reinterpret_cast<HWND>(GetPropW(taskbar, L"TaskbandHWND"));
  if (!bandWindow)
    return nullptr;
  auto band = reinterpret_cast<void **>(GetWindowLongPtrW(bandWindow, 0));
  void **site = nullptr;
  for (int i = 0; i < 20; i++) {
    if (!Readable(band + i, sizeof(void *)))
      break;
    if (band[i] == siteVtable) {
      site = band + i;
      break;
    }
  }
  if (!site)
    return nullptr;
  // Offset of the taskbar element's IUnknown inside TaskbarHost, read from
  // the start of TaskbarHost::FrameHeight (same patterns as taskbar-multirow).
  size_t offset = 0;
#if defined(_M_X64)
  // sub rsp, imm8 / add rcx, offset
  auto code = static_cast<BYTE const *>(frameHeight);
  if (Readable(code, 8) && code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xec &&
      code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xc1 && code[7] <= 0x7f)
    offset = code[7];
#elif defined(_M_ARM64)
  // pacibsp / stp fp, lr, [sp, #-0x10]! / mov fp, sp / ldr x8, [x0, #offset]!
  auto code = static_cast<DWORD const *>(frameHeight);
  if (Readable(code, 4 * sizeof(DWORD)) && code[0] == 0xD503237F &&
      (code[1] & 0xFFC07FFF) == 0xA9807BFD && code[2] == 0x910003FD &&
      (code[3] & 0xFFF00FE0) == 0xF8400C00)
    offset = (code[3] >> 12) & 0xFF;
#endif
  if (!offset) {
    Wh_Log(L"Unsupported TaskbarHost layout; existing icons are styled when recreated");
    return nullptr;
  }
  void *shared[2]{};
  getHost(site, shared);
  X::XamlRoot root{nullptr};
  if (shared[0]) {
    auto address = reinterpret_cast<IUnknown **>(static_cast<BYTE *>(shared[0]) + offset);
    if (Readable(address, sizeof(void *)) && *address)
      try {
        X::FrameworkElement value{nullptr};
        (*address)->QueryInterface(winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(value));
        if (value)
          root = value.XamlRoot();
      } catch (...) {
      }
  }
  if (shared[1])
    decref(shared[1]);
  return root;
}
struct Record {
  DWORD thread = 0;
  HWND taskbar = nullptr;
  winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
  winrt::weak_ref<X::FrameworkElement> icon, native;
  winrt::weak_ref<C::Panel> parent;
  X::FrameworkElement visual{nullptr};
  X::DispatcherTimer timer{nullptr};
  winrt::event_token loaded{}, tick{};
  Journal glyphJournal;
  std::shared_ptr<AnimatedVolume> animated;
  unsigned redrawn = 0;
  std::vector<std::function<void()>> glyphRevoke;
  int displayed = -1, hiddenCount = -1;
  unsigned attempts = 0;
  bool attached = false;
  int kind = 0, netState = 0;
  bool networkSeen = false; // kind: 1 volume, 2 wired network
};
[[clang::no_destroy]] inline std::vector<std::shared_ptr<Record>> records;
inline bool VolumeLeaf(X::DependencyObject const &node) {
  winrt::hstring value;
  if (auto text = node.try_as<C::TextBlock>())
    value = text.Text();
  else if (auto icon = node.try_as<C::FontIcon>())
    value = icon.Glyph();
  if (value.size() != 1)
    return false;
  switch (value[0]) {
  case 0xe74f:
  case 0xe992:
  case 0xe993:
  case 0xe994:
  case 0xe995:
  case 0xea85:
  case 0xebc5:
    return true;
  }
  return false;
}
// Wired network icon of the same ControlCenterButton group. Windows draws it
// with Segoe Fluent Icons E839 (connected) and F8CC (the same glyph with a
// cut-out corner for a badge); for no internet or no connection it shows the
// globe F384, for any network type. The mod shows Lucide ethernet-port with
// matching badges: the globe becomes ethernet-port with the cross badge only
// while a wired hardware adapter has its cable connected, otherwise it stays
// native. Wi-Fi and every other glyph stay native.
inline wchar_t LeafCode(X::DependencyObject const &node) {
  winrt::hstring value;
  if (auto text = node.try_as<C::TextBlock>())
    value = text.Text();
  else if (auto icon = node.try_as<C::FontIcon>())
    value = icon.Glyph();
  return value.size() == 1 ? value[0] : 0;
}
inline int NetworkLeaf(X::DependencyObject const &node) {
  switch (LeafCode(node)) {
  case 0xe839:
  case 0xf8cc:
    return 1;
  case 0xeb55:
    return 2;
  case 0xeb56:
    return 3;
  case 0xf384:
    return 4;
  }
  return 0;
}
// A wired hardware adapter with its medium connected (not Hyper-V/WSL/VPN virtual ones).
// Cached between network changes: NotifyIpInterfaceChange clears the cache,
// so the interface table is read only after a change, not on every tick.
inline std::atomic<int> wiredCache{-1};
inline HANDLE ipNotification = nullptr;
inline void CALLBACK IpInterfaceChanged(PVOID, PMIB_IPINTERFACE_ROW, MIB_NOTIFICATION_TYPE) {
  wiredCache = -1;
}
inline bool ReadWiredConnected() {
  PMIB_IF_TABLE2 table = nullptr;
  if (GetIfTable2(&table) != NO_ERROR || !table)
    return false;
  bool found = false;
  for (ULONG i = 0; i < table->NumEntries && !found; i++) {
    auto const &row = table->Table[i];
    found = row.Type == IF_TYPE_ETHERNET_CSMACD &&
            row.InterfaceAndOperStatusFlags.HardwareInterface &&
            !row.InterfaceAndOperStatusFlags.FilterInterface &&
            row.MediaConnectState == MediaConnectStateConnected;
  }
  FreeMibTable(table);
  return found;
}
inline bool WiredConnected() {
  int cached = wiredCache;
  if (cached >= 0 && ipNotification)
    return cached == 1;
  bool value = ReadWiredConnected();
  wiredCache = value ? 1 : 0;
  return value;
}
inline M::Brush LeafBrush(X::FrameworkElement const &leaf) {
  if (auto text = leaf.try_as<C::TextBlock>())
    return text.Foreground();
  if (auto icon = leaf.try_as<C::IconElement>())
    return icon.Foreground();
  return nullptr;
}
inline double LeafSize(X::FrameworkElement const &leaf) {
  double size = 0;
  if (auto text = leaf.try_as<C::TextBlock>())
    size = text.FontSize();
  else if (auto icon = leaf.try_as<C::FontIcon>())
    size = icon.FontSize();
  return size > 0 && size < 64 ? size : 16;
}
inline X::FrameworkElement NetworkVisual(int state, double size) {
  PCWSTR outline = state == 2   ? sound::ethernetPortCutX
                   : state == 3 ? sound::ethernetPortCutWarn
                                : sound::ethernetPort;
  PCWSTR badge = state == 2   ? sound::ethernetBadgeX
                 : state == 3 ? sound::ethernetBadgeWarn
                              : nullptr;
  std::wstring markup =
      L"<Viewbox xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='" +
      std::to_wstring(size) + L"' Height='" + std::to_wstring(size) +
      L"' HorizontalAlignment='Center' VerticalAlignment='Center' IsHitTestVisible='False'><Canvas "
      L"Width='24' Height='24'><Path StrokeThickness='1.5' StrokeStartLineCap='Round' "
      L"StrokeEndLineCap='Round' StrokeLineJoin='Round' Data='" +
      std::wstring(outline) + L"'/>";
  if (badge)
    markup += L"<Path Data='" + std::wstring(badge) + L"'/>";
  markup += L"</Canvas></Viewbox>";
  return X::Markup::XamlReader::Load(markup).as<X::FrameworkElement>();
}
// The glyph's own brush (theme, high contrast), copied on every pass.
inline void PaintNetwork(X::FrameworkElement const &visual, M::Brush const &brush) {
  auto box = visual.try_as<C::Viewbox>();
  if (!box || !brush)
    return;
  auto canvas = box.Child().try_as<C::Canvas>();
  if (!canvas)
    return;
  auto children = canvas.Children();
  if (children.Size() > 0)
    if (auto path = children.GetAt(0).try_as<X::Shapes::Path>(); path && path.Stroke() != brush)
      path.Stroke(brush);
  if (children.Size() > 1)
    if (auto path = children.GetAt(1).try_as<X::Shapes::Path>(); path && path.Fill() != brush)
      path.Fill(brush);
}
template <class F>
inline X::FrameworkElement Find(X::DependencyObject const &node, F const &predicate) {
  if (auto element = node.try_as<X::FrameworkElement>(); element && predicate(element))
    return element;
  for (int i = 0; i < M::VisualTreeHelper::GetChildrenCount(node); i++)
    if (auto match = Find(M::VisualTreeHelper::GetChild(node, i), predicate))
      return match;
  return nullptr;
}
inline bool InControlCenter(X::FrameworkElement element) {
  while (element) {
    if (element.Name() == L"ControlCenterButton")
      return true;
    element = M::VisualTreeHelper::GetParent(element).try_as<X::FrameworkElement>();
  }
  return false;
}
inline void CopySlot(X::FrameworkElement const &to, X::FrameworkElement const &from) {
  to.HorizontalAlignment(from.HorizontalAlignment());
  to.VerticalAlignment(from.VerticalAlignment());
  to.Margin(from.Margin());
  C::Grid::SetColumn(to, C::Grid::GetColumn(from));
  C::Grid::SetRow(to, C::Grid::GetRow(from));
  C::Grid::SetColumnSpan(to, C::Grid::GetColumnSpan(from));
  C::Grid::SetRowSpan(to, C::Grid::GetRowSpan(from));
  C::Canvas::SetZIndex(to, C::Canvas::GetZIndex(from));
  to.IsHitTestVisible(false);
}
inline void RestoreGlyphs(std::shared_ptr<Record> const &record) {
  for (auto &revoke : record->glyphRevoke)
    try {
      revoke();
    } catch (...) {
      Wh_Log(L"Glyph callback removal: %08X", winrt::to_hresult());
    }
  record->glyphRevoke.clear();
  record->glyphJournal.Restore();
}
inline void RemoveVisual(std::shared_ptr<Record> const &record) {
  if (auto parent = record->parent.get(); parent && record->visual) {
    uint32_t index;
    if (parent.Children().IndexOf(record->visual, index))
      parent.Children().RemoveAt(index);
  }
  record->visual = nullptr;
}
// Opacity only, journaled, kept at 0 against template/transition writes (as volume).
inline void HideLeaves(std::shared_ptr<Record> const &record,
                       std::vector<X::FrameworkElement> const &leaves) {
  for (auto const &node : leaves) {
    auto property = X::UIElement::OpacityProperty();
    auto key = std::make_pair(winrt::get_abi(node), winrt::get_abi(property));
    bool fresh = !record->glyphJournal.saved.contains(key);
    record->glyphJournal.Save(node, property);
    if (node.Opacity() != 0)
      node.Opacity(0);
    if (fresh)
      try {
        auto guard = std::make_shared<bool>(false);
        auto weak = winrt::make_weak(node);
        auto token =
            node.RegisterPropertyChangedCallback(property, [weak, guard](auto &&, auto &&) {
              if (stopping || *guard)
                return;
              if (auto glyph = weak.get(); glyph && glyph.Opacity() != 0) {
                *guard = true;
                try {
                  glyph.Opacity(0);
                } catch (...) {
                }
                *guard = false;
              }
            });
        record->glyphRevoke.push_back(
            [node, property, token] { node.UnregisterPropertyChangedCallback(property, token); });
      } catch (...) {
        Wh_Log(L"Network glyph opacity watch unavailable; timer retained: %08X",
               winrt::to_hresult());
      }
  }
}
inline void RefreshNetwork(std::shared_ptr<Record> const &record, X::FrameworkElement const &icon) {
  auto parent = record->parent.get();
  if (!parent)
    return;
  std::vector<X::FrameworkElement> leaves;
  int state = 0;
  Find(icon, [&](auto const &node) {
    if (int s = NetworkLeaf(node)) {
      leaves.push_back(node);
      if (!state)
        state = s;
    }
    return false;
  });
  bool globeWithoutCable = state == 4 && !WiredConnected();
  if (!state || globeWithoutCable) {
    // The icon shows something else now (Wi-Fi, globe without cable): native
    // again; the next passes look for a supported glyph from scratch.
    RemoveVisual(record);
    RestoreGlyphs(record);
    record->attached = false;
    record->kind = 0;
    record->netState = 0;
    record->native = nullptr;
    record->parent = nullptr;
    record->attempts = 0;
    return;
  }
  auto brush = LeafBrush(leaves.front());
  if (state != record->netState || !record->visual) {
    // The new picture is attached before the old one goes and before any
    // native glyph is hidden: a failure leaves the native icon visible.
    auto visual = NetworkVisual(state == 4 ? 2 : state, LeafSize(leaves.front()));
    visual.Name(L"NativeSoundTrayNetwork");
    if (auto grid = parent.try_as<C::Grid>()) {
      C::Grid::SetColumnSpan(visual, std::max(1u, grid.ColumnDefinitions().Size()));
      C::Grid::SetRowSpan(visual, std::max(1u, grid.RowDefinitions().Size()));
    }
    PaintNetwork(visual, brush);
    parent.Children().Append(visual);
    RemoveVisual(record);
    record->visual = visual;
    record->netState = state;
  } else
    PaintNetwork(record->visual, brush);
  HideLeaves(record, leaves);
}
inline void Refresh(std::shared_ptr<Record> const &record) {
  if (stopping)
    return;
  auto icon = record->icon.get();
  if (!icon) {
    if (record->timer)
      record->timer.Stop();
    return;
  }
  if (record->attached) {
    auto native = record->native.get();
    if (!native || !native.IsLoaded()) {
      if (auto parent = record->parent.get(); parent && record->visual) {
        uint32_t index;
        if (parent.Children().IndexOf(record->visual, index))
          parent.Children().RemoveAt(index);
      }
      RestoreGlyphs(record);
      record->visual = nullptr;
      record->animated.reset();
      record->native = nullptr;
      record->parent = nullptr;
      record->attached = false;
      record->displayed = -1;
      record->kind = 0;
      record->netState = 0;
    }
  }
  if (!record->attached) {
    if (!InControlCenter(icon)) {
      if (record->timer)
        record->timer.Stop();
      return;
    }
    // Use the outermost Grid within this IconView. The replacement must
    // remain outside every native glyph/template branch that we suppress.
    int kind = 1;
    auto leaf = Find(icon, [](auto const &node) { return VolumeLeaf(node); });
    if (!leaf) {
      int wired = -1;
      leaf = Find(icon, [&wired](auto const &node) {
        int s = NetworkLeaf(node);
        if (s == 4) {
          if (wired < 0)
            wired = WiredConnected();
          return wired == 1;
        }
        return s != 0;
      });
      kind = 2;
    }
    X::FrameworkElement glyph{nullptr};
    C::Grid parent{nullptr};
    for (auto slot = leaf; slot && slot != icon;
         slot = M::VisualTreeHelper::GetParent(slot).try_as<X::FrameworkElement>()) {
      auto owner = M::VisualTreeHelper::GetParent(slot);
      if (auto grid = owner.try_as<C::Grid>()) {
        glyph = slot;
        parent = grid;
      }
    }
    if (!glyph) {
      // Icons without a supported glyph keep a slow poll (2 s): Windows
      // switches the same icon between Wi-Fi, the globe and Ethernet.
      ++record->attempts;
      if (record->attempts >= 8 && record->timer &&
          record->timer.Interval() != winrt::Windows::Foundation::TimeSpan(std::chrono::seconds(2)))
        record->timer.Interval(std::chrono::seconds(2));
      return;
    }
    record->native = winrt::make_weak(glyph);
    record->parent = winrt::make_weak(parent.as<C::Panel>());
    record->attached = true;
    record->kind = kind;
    if (kind == 2)
      record->networkSeen = true;
    // Attached: volume changes arrive as pulses (VolumeChanged) and the glyph
    // callbacks keep the native leaves hidden, so the timer is only a slow
    // safety net; the network glyph follows the shell's own glyph changes,
    // which have no event here.
    if (record->timer)
      record->timer.Interval(kind == 2 ? std::chrono::seconds(1) : std::chrono::seconds(2));
  }
  if (record->kind == 2) {
    RefreshNetwork(record, icon);
    return;
  }
  if (!audio)
    return;
  auto state = audio->Get();
  auto native = record->native.get();
  auto parent = record->parent.get();
  if (!native || !parent)
    return;
  if (!state.masterAvailable) {
    if (record->visual) {
      uint32_t index;
      if (parent.Children().IndexOf(record->visual, index))
        parent.Children().RemoveAt(index);
    }
    RestoreGlyphs(record);
    record->visual = nullptr;
    record->animated.reset();
    return;
  }
  if (!record->animated) {
    // Construct and attach the real system AnimatedIcon before suppressing
    // any native leaf. Failure leaves the original taskbar icon visible.
    auto animated = std::make_shared<AnimatedVolume>();
    auto replacement = animated->Isolated();
    replacement.Name(L"NativeSoundTrayGlyph");
    if (auto grid = parent.try_as<C::Grid>()) {
      C::Grid::SetColumnSpan(replacement, std::max(1u, grid.ColumnDefinitions().Size()));
      C::Grid::SetRowSpan(replacement, std::max(1u, grid.RowDefinitions().Size()));
    }
    parent.Children().Append(replacement);
    record->visual = replacement;
    record->animated = std::move(animated);
  }
  record->animated->Update(state.master, state.muted);
  // Never hide the old image before its replacement is attached. No clipping
  // or layout changes: only glyph-leaf opacity, including transition copies.
  try {
    int hidden = 0;
    std::vector<X::FrameworkElement> leaves;
    Find(icon, [&](auto const &node) {
      if (VolumeLeaf(node))
        leaves.push_back(node);
      return false;
    });
    for (auto const &node : leaves) {
      auto property = X::UIElement::OpacityProperty();
      auto key = std::make_pair(winrt::get_abi(node), winrt::get_abi(property));
      bool fresh = !record->glyphJournal.saved.contains(key);
      record->glyphJournal.Save(node, property);
      node.Opacity(0);
      hidden++;
      if (fresh)
        try {
          auto guard = std::make_shared<bool>(false);
          auto weak = winrt::make_weak(node);
          auto token =
              node.RegisterPropertyChangedCallback(property, [weak, guard](auto &&, auto &&) {
                if (stopping || *guard)
                  return;
                if (auto glyph = weak.get(); glyph && glyph.Opacity() != 0) {
                  *guard = true;
                  try {
                    glyph.Opacity(0);
                  } catch (...) {
                  }
                  *guard = false;
                }
              });
          record->glyphRevoke.push_back(
              [node, property, token] { node.UnregisterPropertyChangedCallback(property, token); });
        } catch (...) {
          Wh_Log(L"Native glyph opacity watch unavailable; timer retained: %08X",
                 winrt::to_hresult());
        }
    }
    if (hidden != record->hiddenCount) {
      record->hiddenCount = hidden;
    }
  } catch (...) {
    Wh_Log(L"TrayLastError %08X", winrt::to_hresult());
    RestoreGlyphs(record);
    if (record->visual) {
      uint32_t index;
      if (parent.Children().IndexOf(record->visual, index))
        parent.Children().RemoveAt(index);
      record->visual = nullptr;
    }
    record->displayed = -1;
    throw;
  }
}
inline std::mutex pulsesMutex;
inline std::map<HWND, std::shared_ptr<VolumePulse>> pulses;
inline LRESULT CALLBACK VolumeProc(HWND window, UINT message, WPARAM w, LPARAM l, DWORD_PTR) {
  if (message == volumePulseMessage) {
    {
      std::lock_guard lock(pulsesMutex);
      if (auto it = pulses.find(window); it != pulses.end())
        it->second->queued = false;
    }
    if (!stopping) {
      std::vector<std::shared_ptr<Record>> local;
      {
        std::lock_guard lock(recordsMutex);
        for (auto const &record : records)
          if (record->taskbar == window && record->attached)
            local.push_back(record);
      }
      unsigned generation = redrawGeneration;
      for (auto const &record : local)
        try {
          if (record->redrawn != generation) {
            record->redrawn = generation;
            if (record->animated)
              record->animated->Redraw();
          }
          Refresh(record);
        } catch (...) {
          Wh_Log(L"TrayAnimationError %08X", winrt::to_hresult());
        }
    }
    return 0;
  }
  if (message == WM_NCDESTROY) {
    {
      std::lock_guard lock(pulsesMutex);
      if (auto it = pulses.find(window); it != pulses.end()) {
        it->second->active = false;
        pulses.erase(it);
      }
    }
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, VolumeProc);
  }
  return DefSubclassProc(window, message, w, l);
}
inline void EnsurePulse(HWND window) {
  {
    std::lock_guard lock(pulsesMutex);
    if (pulses.contains(window))
      return;
  }
  if (WindhawkUtils::SetWindowSubclassFromAnyThread(window, VolumeProc, 0)) {
    std::lock_guard lock(pulsesMutex);
    pulses.emplace(window, std::make_shared<VolumePulse>(window));
  }
}
inline void VolumeChanged() {
  std::vector<std::shared_ptr<VolumePulse>> copy;
  {
    std::lock_guard lock(pulsesMutex);
    for (auto const &[window, pulse] : pulses)
      copy.push_back(pulse);
  }
  for (auto const &pulse : copy)
    pulse->Post();
}
inline void Observe(X::FrameworkElement const &element, HWND taskbar) {
  if (!element || stopping)
    return;
  EnsurePulse(taskbar);
  {
    std::lock_guard lock(recordsMutex);
    for (auto const &r : records)
      if (auto current = r->icon.get(); current && current.as<IUnknown>() == element.as<IUnknown>())
        return;
  }
  auto record = std::make_shared<Record>();
  record->thread = GetCurrentThreadId();
  record->taskbar = taskbar;
  record->dispatcher = element.Dispatcher();
  record->icon = winrt::make_weak(element);
  std::weak_ptr<Record> weak = record;
  record->loaded = element.Loaded([weak](auto &&, auto &&) {
    if (auto r = weak.lock())
      try {
        r->attempts = 0;
        Refresh(r);
        if (r->attached || InControlCenter(r->icon.get()))
          r->timer.Start();
      } catch (...) {
        Wh_Log(L"Volume icon attach failed");
      }
  });
  record->timer = X::DispatcherTimer();
  record->timer.Interval(std::chrono::milliseconds(300));
  record->tick = record->timer.Tick([weak](auto &&, auto &&) {
    if (auto r = weak.lock())
      try {
        Refresh(r);
      } catch (...) {
        Wh_Log(L"TrayAnimationError %08X", winrt::to_hresult());
        r->timer.Stop();
        Wh_Log(L"Volume icon refresh failed");
      }
  });
  {
    std::lock_guard lock(recordsMutex);
    records.push_back(record);
  }
  Refresh(record);
  if (record->attached || InControlCenter(element))
    record->timer.Start();
}
inline void Walk(X::DependencyObject const &node, HWND taskbar) {
  if (auto element = node.try_as<X::FrameworkElement>())
    if (VolumeLeaf(element) || NetworkLeaf(element)) {
      auto icon = element;
      for (auto ancestor = element; ancestor;
           ancestor = M::VisualTreeHelper::GetParent(ancestor).try_as<X::FrameworkElement>()) {
        if (std::wstring_view(winrt::get_class_name(ancestor)).find(L"SystemTray.IconView") !=
            std::wstring_view::npos) {
          icon = ancestor;
          break;
        }
      }
      if (InControlCenter(icon))
        Observe(icon, taskbar);
      return;
    }
  for (int i = 0; i < M::VisualTreeHelper::GetChildrenCount(node); i++)
    Walk(M::VisualTreeHelper::GetChild(node, i), taskbar);
}
using Construct = void *(WINAPI *)(void *);
inline Construct originalConstructor = nullptr;
inline void *WINAPI Constructed(void *self) {
  auto result = originalConstructor(self);
  if (stopping)
    return result;
  // C++/WinRT implementation's projected interface occupies the second slot.
  try {
    auto address = reinterpret_cast<IUnknown **>(self) + 1;
    if (Readable(address, sizeof(void *)) && *address) {
      X::FrameworkElement element{nullptr};
      (*address)->QueryInterface(winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(element));
      if (element) {
        HWND taskbar = nullptr;
        for (auto window : Taskbars())
          if (GetWindowThreadProcessId(window, nullptr) == GetCurrentThreadId()) {
            taskbar = window;
            break;
          }
        if (taskbar)
          Observe(element, taskbar);
      }
    }
  } catch (...) {
    Wh_Log(L"New tray icon observation failed");
  }
  return result;
}
inline void OpenNativeSound() {
  INPUT input[6]{};
  UINT count = 0;
  bool winHeld = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0,
       ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
  auto key = [&](WORD code, bool up) {
    input[count].type = INPUT_KEYBOARD;
    input[count].ki.wVk = code;
    input[count++].ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
  };
  if (!winHeld)
    key(VK_LWIN, false);
  if (!ctrlHeld)
    key(VK_CONTROL, false);
  key('V', false);
  key('V', true);
  if (!ctrlHeld)
    key(VK_CONTROL, true);
  if (!winHeld)
    key(VK_LWIN, true);
  auto sent = SendInput(count, input, sizeof(INPUT));
  Wh_Log(L"Native Sound shortcut: %u/%u events", sent, count);
}
// The native handler takes TappedRoutedEventArgs by value. In the MSVC ABI the
// caller passes the address of a temporary and the callee destroys it; the
// mod's compiler would treat a by-value parameter differently, so the hook takes
// that address and the native handler (or this hook, when it handles the tap)
// releases the object.
using NativeTapped = void(WINAPI *)(void *, IInspectable const &, void *);
inline NativeTapped originalTapped = nullptr;
inline void WINAPI Tapped(void *self, IInspectable const &sender, void *argsSlot) {
  auto &args = *static_cast<X::Input::TappedRoutedEventArgs *>(argsSlot);
  if (!stopping)
    try {
      // Intercept the shell's own handler, before it activates Quick Settings.
      // A later UIElement.Tapped subscription cannot cancel an action already
      // performed by this handler.
      auto address = reinterpret_cast<IUnknown **>(self) + 1;
      if (Readable(address, sizeof(void *)) && *address) {
        X::FrameworkElement icon{nullptr};
        (*address)->QueryInterface(winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(icon));
        if (icon && InControlCenter(icon) &&
            Find(icon, [](auto const &child) { return VolumeLeaf(child); })) {
          args.Handled(true);
          Wh_Log(L"Sound panel: native volume click intercepted");
          OpenNativeSound();
          try {
            for (auto taskbar : Taskbars())
              if (GetWindowThreadProcessId(taskbar, nullptr) == GetCurrentThreadId()) {
                Observe(icon, taskbar);
                break;
              }
          } catch (...) {
            Wh_Log(L"Lazy volume icon styling failed; click was still handled");
          }
          args = nullptr; // the callee's share of the by-value argument
          return;
        }
      }
    } catch (winrt::hresult_error const &error) {
      Wh_Log(L"Native volume handler: 0x%08X", error.code().value);
    }
  originalTapped(self, sender, argsSlot);
}
inline bool HookTray(HMODULE module) {
  // Undecorated names verified against the same RVAs in both cached SystemTray PDBs.
  // SystemTray.dll
  WindhawkUtils::SYMBOL_HOOK hooks[] = {
      {{L"public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void)"},
       &originalConstructor,
       Constructed},
      {{L"private: void __cdecl winrt::SystemTray::implementation::IconView::OnTappedHandler("
        L"struct winrt::Windows::Foundation::IInspectable const &,"
        L"struct winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs)"},
       &originalTapped,
       Tapped}};
  auto result = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
  return result;
}
using Load = decltype(&LoadLibraryExW);
inline Load originalLoad = nullptr;
inline HMODULE WINAPI Loaded(LPCWSTR path, HANDLE file, DWORD flags) {
  auto module = originalLoad(path, file, flags);
  if (module && !stopping && module == GetModuleHandleW(L"SystemTray.dll") &&
      !moduleHooked.exchange(true))
    if (HookTray(module))
      Wh_ApplyHookOperations();
  return module;
}
inline bool Initialize(HINSTANCE instance) {
  moduleInstance = instance;
  stopping = false;
  audio.emplace();
  powerWake.emplace([] {
    ++redrawGeneration;
    VolumeChanged();
  });
  wiredCache = -1;
  if (NotifyIpInterfaceChange(AF_UNSPEC, IpInterfaceChanged, nullptr, FALSE, &ipNotification) !=
      NO_ERROR)
    ipNotification = nullptr;
  auto taskbar = GetModuleHandleW(L"Taskbar.dll");
  if (!taskbar)
    taskbar = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!taskbar) {
    Wh_Log(L"Taskbar.dll unavailable: %u", GetLastError());
    return false;
  }
  // Taskbar.dll
  WindhawkUtils::SYMBOL_HOOK roots[] = {
      {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &siteVtable},
      {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
       &getHost},
      {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &frameHeight},
      {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &decref}};
  if (!WindhawkUtils::HookSymbols(taskbar, roots, ARRAYSIZE(roots)))
    Wh_Log(L"Taskbar root symbols unavailable; constructor/tap discovery retained");
  if (auto module = GetModuleHandleW(L"SystemTray.dll")) {
    moduleHooked = true;
    auto result = HookTray(module);
    if (result)
      audio->Start(VolumeChanged);
    return result;
  }
  auto load =
      reinterpret_cast<Load>(GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW"));
  auto result = WindhawkUtils::SetFunctionHook(load, Loaded, &originalLoad);
  if (result)
    audio->Start(VolumeChanged);
  return result;
}
inline void AttachExisting() {
  for (auto taskbar : Taskbars())
    Dispatch(taskbar, [taskbar] {
      EnsurePulse(taskbar);
      auto root = Root(taskbar);
      if (root)
        Walk(root.Content(), taskbar);
    });
  size_t attached = 0;
  {
    std::lock_guard lock(recordsMutex);
    for (auto const &r : records)
      if (r->attached)
        attached++;
  }
  Wh_Log(L"Sound panel: attached %zu volume icons", attached);
}
// All XAML cleanup and the final Record destruction stay on the owner thread.
// Leave a record in the no_destroy vector if cleanup fails partway through.
inline void CleanupThreadRecords() {
  std::vector<std::shared_ptr<Record>> local;
  {
    std::lock_guard lock(recordsMutex);
    for (auto const &record : records)
      if (record->thread == GetCurrentThreadId())
        local.push_back(record);
  }
  for (auto const &record : local)
    try {
      if (record->timer) {
        record->timer.Stop();
        record->timer.Tick(record->tick);
        record->timer = nullptr;
      }
      if (auto icon = record->icon.get(); icon && record->loaded.value) {
        icon.Loaded(record->loaded);
        record->loaded = {};
      }
      RestoreGlyphs(record);
      RemoveVisual(record);
      record->animated.reset();
      std::lock_guard lock(recordsMutex);
      auto it = std::find(records.begin(), records.end(), record);
      if (it != records.end())
        records.erase(it);
    } catch (...) {
      Wh_Log(L"Tray record cleanup on UI thread %u: %08X",
             GetCurrentThreadId(), winrt::to_hresult());
    }
}
inline void Shutdown() {
  stopping = true;
  powerWake.reset();
  if (ipNotification) {
    CancelMibChangeNotify2(ipNotification); // waits for a running callback
    ipNotification = nullptr;
  }
  std::vector<HWND> windows = Taskbars();
  auto rememberWindow = [&](HWND window) {
    if (window && std::find(windows.begin(), windows.end(), window) == windows.end())
      windows.push_back(window);
  };
  {
    std::lock_guard lock(pulsesMutex);
    for (auto const &[window, pulse] : pulses) {
      pulse->active = false;
      rememberWindow(window);
    }
  }
  if (audio)
    audio->Stop();
  {
    std::lock_guard lock(recordsMutex);
    for (auto const &record : records)
      rememberWindow(record->taskbar);
  }
  for (auto window : windows) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
      // The subclass has its own synchronous removal message, independent
      // of whether our window-dispatch hook can be installed.
      WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, VolumeProc);
      Dispatch(window, CleanupThreadRecords);
    }
  }
  std::map<DWORD, winrt::Windows::UI::Core::CoreDispatcher> remaining;
  {
    std::lock_guard lock(recordsMutex);
    for (auto const &record : records)
      remaining.try_emplace(record->thread, record->dispatcher);
  }
  for (auto const &[thread, dispatcher] : remaining) {
    auto threadHandle = OpenThread(SYNCHRONIZE, FALSE, thread);
    bool exited = threadHandle && WaitForSingleObject(threadHandle, 0) == WAIT_OBJECT_0;
    if (threadHandle)
      CloseHandle(threadHandle);
    if (exited) {
      Wh_Log(L"Tray UI thread %u has exited; retaining its XAML records", thread);
      continue;
    }
    if (DispatchCleanup(dispatcher, CleanupThreadRecords))
      Wh_Log(L"Tray shutdown: dispatcher cleanup ran on UI thread %u", thread);
    else
      Wh_Log(L"Tray UI thread %u is shutting down; retaining its XAML records", thread);
  }
  {
    std::lock_guard lock(pulsesMutex);
    pulses.clear();
  }
  {
    std::lock_guard lock(recordsMutex);
    if (records.empty())
      records.shrink_to_fit();
    else
      Wh_Log(L"Retaining %zu tray records on unreachable UI threads", records.size());
  }
  audio.reset();
}
} // namespace native::tray

namespace native {
inline bool explorer = false;
inline void Settings() {
  auto setting = WindhawkUtils::StringSetting::make(L"hiddenApps");
  hidden = sound::ParseHidden(setting.get());
}
} // namespace native
BOOL Wh_ModInit() {
  using namespace native;
  GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                         GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                     reinterpret_cast<LPCWSTR>(&Wh_ModInit), &module);
  WCHAR path[32768]{};
  GetModuleFileNameW(nullptr, path, 32768);
  explorer = !_wcsicmp(PathFindFileNameW(path), L"explorer.exe");
  stopping = false;
  Settings();
  if (explorer) {
    if (!tray::Initialize(module)) {
      tray::Shutdown();
      return FALSE;
    }
    return TRUE;
  }
  return InitializeHost();
}
void Wh_ModAfterInit() {
  if (native::explorer) {
    native::tray::AttachExisting();
  } else
    native::AfterHostInit();
}
void Wh_ModUninit() {
  native::stopping = true;
  if (native::explorer) {
    native::tray::Shutdown();
  } else
    native::StopHost();
}
BOOL Wh_ModSettingsChanged(BOOL *reload) {
  *reload = TRUE;
  return TRUE;
}

/* Lucide license:
ISC License

Copyright (c) 2026 Lucide Icons and Contributors

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

---

The following Lucide icons are derived from the Feather project:

airplay, alert-circle, alert-octagon, alert-triangle, aperture, arrow-down-circle, arrow-down-left, arrow-down-right, arrow-down, arrow-left-circle, arrow-left, arrow-right-circle, arrow-right, arrow-up-circle, arrow-up-left, arrow-up-right, arrow-up, at-sign, calendar, cast, check, chevron-down, chevron-left, chevron-right, chevron-up, chevrons-down, chevrons-left, chevrons-right, chevrons-up, circle, clipboard, clock, code, columns, command, compass, corner-down-left, corner-down-right, corner-left-down, corner-left-up, corner-right-down, corner-right-up, corner-up-left, corner-up-right, crosshair, database, divide-circle, divide-square, dollar-sign, download, external-link, feather, frown, hash, headphones, help-circle, info, italic, key, layout, life-buoy, link-2, link, loader, lock, log-in, log-out, maximize, meh, minimize, minimize-2, minus-circle, minus-square, minus, monitor, moon, more-horizontal, more-vertical, move, music, navigation-2, navigation, octagon, pause-circle, percent, plus-circle, plus-square, plus, power, radio, rss, search, server, share, shopping-bag, sidebar, smartphone, smile, square, table-2, tablet, target, terminal, trash-2, trash, triangle, tv, type, upload, x-circle, x-octagon, x-square, x, zoom-in, zoom-out

The MIT License (MIT) (for the icons listed above)

Copyright (c) 2013-present Cole Bemis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
