// ==WindhawkMod==
// @id              separate-system-tray-icons
// @name            Separate System Tray Icons
// @description     Replaces the grouped Windows 11 system tray button with separate sound, Bluetooth, network, Control Center, and battery buttons.
// @version         1.1.0
// @author          Asteski
// @license         GPL-3.0
// @github          https://github.com/Asteski
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lshell32 -lole32 -loleaut32 -lruntimeobject -luuid -liphlpapi -lwlanapi -lbthprops
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Separate System Tray Icons

Replaces the grouped Windows 11 system tray button with separate sound,
Bluetooth, network, Control Center, and battery buttons. The original grouped
button is hidden while the mod is active and restored when it is unloaded.

![Demonstration](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/e02d89d70dfb0ad3815b806e3eb2a49e56cb86b8/img/separate-system-tray-icons/separate-system-tray-icons.gif)

Buttons use XAML controls with font glyphs and vector artwork, keeping the
icons sharp at different display scales.

Buttons:

- Bluetooth -> Bluetooth flyout
- Network -> Network flyout
- Sound -> Sound output picker by default, or Control Center/volume mixer
- Control Center -> Control Center, Project, or a custom action
- Battery -> Power settings by default, or Control Center/custom action

Use **Toggle buttons visibility** to choose which buttons appear. Drag items in
**Button order** to arrange the buttons, including Battery. Removing an entry
hides that button; adding it back shows it when its visibility toggle is
enabled. Battery appears only on devices where Windows reports a battery.

Sound supports mouse-wheel volume adjustment (unmuting first) and middle-click
muting. **Volume scroll step** selects Windows' default step or 1, 5, or 10
percentage points per wheel notch. Middle-click can instead run a custom
action; the other buttons also have independent custom middle-click actions.
Leave an action empty to do nothing.

**Show master volume in Sound output**, enabled by default, adds a master
slider and mute button above the application mixer in Windows 11's Sound
output flyout. Moving the slider unmutes the output;
external volume changes and output-device switches are reflected while open.
Disable the standalone Sound Output Master Volume mod when using this
feature. Close and reopen the flyout after changing this option.

Sound can show currently playing media in its tooltip and use an output-device
glyph. Muted audio uses the standard mute glyph. Bluetooth can show connected
devices and their battery percentages (when reported by Windows) in its tooltip
and change appearance when switched off. Network can show Wi-Fi signal
strength and open a custom URL for **Perform speed test**.

Right-click a button for its context menu, including sound output selection,
Bluetooth pairing/file transfer, network actions, and battery preferences.
**Swap troubleshooting position** moves troubleshooting to the bottom of
the sound, Bluetooth, and network menus, moving the bottom actions to the top.

Under Battery, **Match other buttons' size without percentage** uses the
standard tray-button dimensions when percentage is hidden. It defaults to
off, retaining the wider horizontal battery button. Vertical icon-only sizing
and percentage-enabled layout are unchanged.

On first use on a device with a battery, the mod enables Windows' battery
percentage preference. Later changes in Settings or the battery menu are
preserved, including across Explorer restarts.

Battery percentage appears beside the icon on horizontal taskbars and below
it on vertical taskbars. **Use small battery icon** switches between 12- and
16-pixel glyphs without changing the percentage text size.

**Show leaf icon in Energy saver** is enabled by default. It overlays a green
vector leaf at the battery's bottom-left instead of replacing the battery.
The outline and center line follow the taskbar's light/dark theme. While the
leaf is shown, Energy saver does not turn the battery fill yellow; charging
and low-battery colors are retained.

Partial movements from smooth-scrolling mice accumulate into full wheel notches;
horizontal scrolling does not change volume. Reach the buttons through Windows'
taskbar keyboard navigation, then use Enter or Space to activate, or Shift+F10
or the Menu key to open a context menu. Buttons expose their current tooltip
information as accessible names and use system keyboard-focus visuals.

## Action formats

The custom-action prefix scheme and action-format documentation are adapted from
[Ultimate Custom Tray](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/ultimate-custom-tray.wh.cpp)
by [Salyts](https://github.com/Salyts).

Select **Custom action** for **Control Center action** or **Battery click
action**, then enter an action in that group's **Custom action** field. The
same formats work in **Custom middle-click action** fields.

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\app.exe"` | Opens a file or folder by absolute path. |
| `~` | `~Downloads` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `ps:` / `powershell:` | `ps:Start-Process notepad` | Runs through `powershell.exe`. |
| `key:` / `hotkey:` | `key:Ctrl+Shift+Esc` | Simulates a keyboard shortcut with virtual key presses. |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

For shortcut actions, use `Modifier+Modifier+Key`.
Supported modifiers: `Ctrl`, `Alt`, `Shift`, `Win`.
Examples: `key:Ctrl+Alt+D`, `hotkey:Win+R`, `key:0x7B` (F12 by VK code).

## Compatibility

Status changes use Windows notifications where available, with a 60-second
fallback refresh. The Wi-Fi connecting animation updates every 500 ms while
Windows reports an active connection attempt.
This version currently manages the primary taskbar; separate-monitor taskbars
are not supported by the current implementation.

The taskbar buttons run in Explorer. The optional master-volume addition
runs in ShellHost or ShellExperienceHost, depending on the Windows version.
Manual installation of the battery font is not required on supported builds.

## Taskbar Styler targets

The injected buttons normally use `SystemTray.OmniButton`. If a fallback is
needed, the mod logs the actual control class; the button names stay the same.

| Target | Description |
| --- | --- |
| `SystemTray.OmniButton#SeparateQuickSettingsXamlBluetooth` | Bluetooth button and its native template/hit target. |
| `SystemTray.OmniButton#SeparateQuickSettingsXamlNetwork` | Network button. |
| `SystemTray.OmniButton#SeparateQuickSettingsXamlSound` | Sound button, including wheel and middle-click handling. |
| `SystemTray.OmniButton#SeparateQuickSettingsXamlControlCenter` | Separate Control Center button; the original grouped button is always hidden. |
| `SystemTray.OmniButton#SeparateQuickSettingsXamlBattery` | Separate battery button, including its highlight and click handling. |
| `StackPanel#SeparateTrayBatteryContent` | Battery glyph and percentage layout; horizontal or vertical to match the taskbar. |
| `TextBlock#SeparateTrayBatteryPercentage` | Battery percentage text. |
| `Grid#SeparateTrayIconLayers` | Layered glyph host; battery has its own measured dimensions, other buttons use a 16-by-16 host. |
| `FontIcon#SeparateTrayIconPrimary` | Main status or output-device glyph. |
| `FontIcon#SeparateTrayIconUnderlay` | Theme-aware grey underlying glyph, where applicable. |
| `FontIcon#SeparateTrayIconOverlay` | Additional status layer, including Bluetooth's unavailable indicator and battery charge-level fill. |
| `Viewbox#SeparateTrayBatteryEnergySaverOverlay` | Scalable Energy saver leaf overlay. |
| `Path#SeparateTrayLeafBody` | Leaf shape and green fill. |
| `Path#SeparateTrayLeafStem` | Theme-aware center line of the leaf. |
| `MenuFlyoutPresenter#SeparateTrayContextMenuPresenter` | WinUI context menu presenter, named when the menu opens. |

The layer names are shared across buttons. An unqualified layer selector affects
all matching buttons. For example:

```yaml
controlStyles:
  - target: FontIcon#SeparateTrayIconPrimary
    styles:
      - FontSize=16
```

### Hover highlights and generated templates

Highlight geometry follows a live native notification icon's rendered insets
on all taskbar edges. The mod stretches its existing template background using
those insets instead of assigning a compact/default fixed highlight height.
Insets are refreshed on hover and when the tray is rebuilt. Battery also
refreshes its layout after percentage or taskbar-position changes. If no
usable native reference is available, the existing control template supplies
layout.

The native button template contains `Border#BackgroundBorder` on the supported
taskbar build. This is the existing hover-highlight element. Use the visual-tree
paths in the mod logs to target it beneath a particular button. Template
intermediate elements can vary between Windows builds: Taskbar Styler's `>`
requires a direct parent/child relationship, so do not assume the border or
glyph host is a direct child of the button.

### Styling limitations

The mod refreshes glyphs, foreground colors, visibility, overlay font sizes,
and button dimensions as state changes. Styles for those properties can be
overwritten by the next update. Stable names provide selectors, but external
stylers may not observe every dynamically created element. In particular, the
menu presenter receives its name after creation.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sound:
    - volumeWheelStep: "0"
      $name: Volume scroll step
      $description: "Volume change per mouse-wheel notch over Sound. System default uses Windows' normal volume steps."
      $options:
      - "0": System default
      - "1": 1%
      - "5": 5%
      - "10": 10%
    - soundClickAction: sound_output
      $name: Sound click action
      $description: "What happens when the separated sound icon is clicked. Open sound output picker simulates Ctrl+Win+V, which can also be received by the focused app."
      $options:
      - quick_settings: Open Control Center
      - sound_output: Open sound output picker
      - sndvol: Open volume mixer
    - showCurrentlyPlayingInSoundTooltip: true
      $name: Show currently playing in tooltip
    - soundIconFollowsOutputDevice: false
      $name: Sound glyph follows output device
      $description: "When enabled, the sound icon uses an output-device glyph for headphones, speakers, display audio, etc. Muted/unavailable audio still uses the normal mute glyph."
    - showMasterVolumeInOutputPicker: true
      $name: Show master volume in Sound output
      $description: "Add a master volume slider and mute button above the application sliders in the Windows 11 Sound output flyout. Close and reopen the flyout after first enabling the feature. Disable the standalone Sound Output Master Volume mod when using this option."
    - middleClickAction: toggle_mute
      $name: Middle-click action
      $options:
      - toggle_mute: Toggle mute
      - custom: Custom action
    - middleClickCustomAction: ""
      $name: Custom middle-click action
      $description: "Used when Middle-click action is Custom action. Uses the same action formats as Control Center custom actions."
  $name: Sound
  $description: Scroll to change volume and unmute. Configure click and middle-click actions below.
- bluetooth:
    - showConnectedDevicesInTooltip: true
      $name: Show connected devices in tooltip
      $description: Includes each device's battery percentage when Windows provides it.
    - changeGlyphWhenDisabled: true
      $name: Show unavailable Bluetooth indicator
      $description: Change the Bluetooth icon appearance while Bluetooth is off.
    - middleClickCustomAction: ""
      $name: Custom middle-click action
      $description: "Leave empty to do nothing. Supports the same action formats as Control Center custom actions."
  $name: Bluetooth
- network:
    - showSignalStrengthInTooltip: true
      $name: Show signal strength in tooltip
    - speedTestUrl: "https://www.speedtest.net/run"
      $name: Speed test custom URL
      $description: Website opened by Perform speed test. Enter a full http:// or https:// address.
    - middleClickCustomAction: ""
      $name: Custom middle-click action
      $description: "Leave empty to do nothing. Supports the same action formats as Control Center custom actions."
  $name: Network
- controlCenter:
    - controlCenterGlyph: F4C3
      $name: Control Center glyph
    - actionMode: default
      $name: Control Center action
      $options:
      - default: Open Control Center
      - project: Open Project
      - custom: Custom action
    - controlCenterAction: "ms-controlcenter:"
      $name: Custom action
      $description: "Used when Custom action is selected. Supports file paths, ~ folders, cmd:, ps:, powershell:, key:, hotkey:, web:, and ms-settings: actions."
    - middleClickCustomAction: ""
      $name: Custom middle-click action
      $description: "Leave empty to do nothing. Supports the same action formats as Control Center custom actions."
  $name: Control Center
- battery:
    - smallGlyph: false
      $name: Use small battery icon
      $description: Use a smaller battery icon. Turn off for the default size. Percentage text keeps its normal size.
    - energySaverLeafGlyph: true
      $name: Show leaf icon in Energy saver
      $description: Overlay a green leaf at the bottom-left of the battery icon while Energy saver is active. Battery and percentage remain visible.
    - matchOtherButtonSize: false
      $name: Match other buttons' size without percentage
      $description: "Use the same button and highlight dimensions as the other tray icons when battery percentage is hidden. When disabled, keep the wider horizontal battery button. Percentage-enabled layout is unchanged."
    - actionMode: custom
      $name: Battery click action
      $options:
      - default: Open Control Center
      - custom: Custom action
    - customAction: "ms-settings:powersleep"
      $name: Custom action
      $description: "Used when Custom action is selected. Supports file paths, ~ folders, cmd:, ps:, powershell:, key:, hotkey:, web:, and ms-settings: actions."
    - middleClickCustomAction: ""
      $name: Custom middle-click action
      $description: "Leave empty to do nothing. Supports the same action formats as Control Center custom actions."
  $name: Battery
- visibility:
    - showSoundButton: true
      $name: Sound
    - showBluetoothButton: true
      $name: Bluetooth
    - showNetworkButton: true
      $name: Network
    - showControlCenterButton: true
      $name: Control Center
    - showBatteryButton: true
      $name: Battery
  $name: Toggle buttons visibility
- buttonOrder: [sound, bluetooth, network, controlcenter, battery]
  $name: Button order
  $description: "Drag to reorder buttons. Remove an entry to hide its button; add it back to show it. The visibility toggles above must also be enabled."
  $options:
  - sound: Sound
  - bluetooth: Bluetooth
  - network: Network
  - controlcenter: Control Center
  - battery: Battery
- swapTroubleshootingWithBottom: false
  $name: Swap troubleshooting position
  $description: "Move troubleshooting to the bottom of Bluetooth, network, and sound menus, swapping it with the bottom action(s)."
*/
// ==/WindhawkModSettings==

// WIN32_LEAN_AND_MEAN must be a compiler option: Windhawk preincludes windows.h.
#include <winsock2.h>
#include <ws2tcpip.h>
#include <objbase.h>
#include <mmsystem.h>
#include <windhawk_utils.h>
#include <memory>
#include <functional>
#include <winrt/Windows.System.h>

#include <windows.h>
#include <inspectable.h>
#include <shellapi.h>
#include <shlobj.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <netlistmgr.h>
#include <propsys.h>
#include <iphlpapi.h>
#include <iprtrmib.h>
#include <wlanapi.h>
#include <bluetoothapis.h>
#include <dbt.h>
#include <netioapi.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <cstring>
#include <new>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <deque>
#include <mutex>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wmc = winrt::Windows::Media::Control;
namespace wdr = winrt::Windows::Devices::Radios;
namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxa = winrt::Windows::UI::Xaml::Automation;
namespace wuc = winrt::Windows::UI::Xaml::Controls;
namespace wucp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxmk = winrt::Windows::UI::Xaml::Markup;
namespace wuxm = winrt::Windows::UI::Xaml::Media;


struct Settings {
    bool soundMiddleClickToggleMute = true;
    std::wstring soundMiddleClickAction;
    std::wstring bluetoothMiddleClickAction;
    std::wstring networkMiddleClickAction;
    std::wstring controlCenterMiddleClickAction;
    std::wstring batteryMiddleClickAction;
    std::wstring soundClickAction = L"sound_output";
    int volumeWheelStep = 0;
    std::wstring controlCenterGlyph = L"F4C3";
    std::wstring controlCenterAction = L"ms-controlcenter:";
    std::wstring batteryAction = L"ms-settings:powersleep";
    bool batteryCustomAction = true;
    bool showConnectedDevicesInTooltip = true;
    bool changeBluetoothGlyphWhenDisabled = true;
    bool showSignalStrengthInTooltip = true;
    std::wstring speedTestUrl = L"https://www.speedtest.net/run";
    bool showBluetoothButton = true;
    bool showNetworkButton = true;
    bool showSoundButton = true;
    bool showControlCenterButton = true;
    bool showBatteryButton = true;
    bool smallBatteryGlyph = false;
    bool batteryMatchOtherButtonSize = false;
    bool energySaverLeafGlyph = true;
    bool showCurrentlyPlayingInSoundTooltip = true;
    bool soundIconFollowsOutputDevice = false;
    std::wstring buttonOrder = L"sound,bluetooth,network,controlcenter,battery";
    bool swapTroubleshootingWithBottom = false;
};

enum class NetworkKind {
    Disconnected,
    Ethernet,
    Wifi,
    WifiConnecting,
    WifiDisconnected,
    WifiDisabled,
};

struct NetworkState {
    NetworkKind kind = NetworkKind::Disconnected;
    ULONG signal = 0;
    std::wstring name;
    bool internetAccess = false;
};

struct SoundState {
    bool available = false;
    bool muted = false;
    float volume = 0.0f;
    std::wstring outputName;
    EndpointFormFactor outputFormFactor = UnknownFormFactor;
};

struct AudioOutputEndpoint {
    std::wstring id;
    std::wstring name;
    bool isDefault = false;
};

static void UpdateDynamicXamlIcons();
static void PostTrayRefresh(bool radiosChanged);
static void RefreshBatteryMenuItems();
static void QueueEnergySaverWork(int operation);
static bool TryApplyNativeNetworkGlyphs();
static bool TryApplyNativeSoundGlyphs();
static bool TryApplyNativeBatteryGlyphs(wuxm::Brush const& fill);
static void ApplyNativeBatteryFont();
static void RequestBluetoothTooltipRefresh();
enum TrayRefreshReason : unsigned {
    RefreshAudio = 1u << 0,
    RefreshAudioDevices = 1u << 1,
    RefreshNetwork = 1u << 2,
    RefreshRadios = 1u << 3,
    RefreshPower = 1u << 4,
    RefreshMedia = 1u << 5,
    RefreshAudioWork = 1u << 6,
    RefreshAll = (1u << 6) - 1,
};
static void RequestTrayRefresh(unsigned reasons);
static void EnsureTrayRefreshWindow();
static void DestroyTrayRefreshWindow();
static void RemoveXamlButtons();
static void StartStatusEvents(HWND hwnd);
static void StopStatusEvents();
static void StopStatusEventsAsync();
static void RestoreGridTrayMutation();
static void InvalidateEnergySaverRead();
static void RefreshMediaTooltipInfoFromStatusWorker();
static winrt::hstring GetNetworkGlyph(NetworkState const& state);
static NetworkState g_displayNetworkState;

// Status collection can involve RPC to the audio, WLAN and network-list
// services. Keep that work on the status worker; the taskbar UI thread only
// consumes this snapshot while it updates XAML.
struct StatusSnapshot {
    bool bluetoothAvailable = false;
    NetworkState network;
    SoundState sound;
    std::vector<AudioOutputEndpoint> audioOutputs;
    bool ready = false;
};
static SRWLOCK g_statusSnapshotLock = SRWLOCK_INIT;
static StatusSnapshot g_statusSnapshot;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);

static Settings g_settings;
static HWND g_taskbarWnd = nullptr;
[[clang::no_destroy]] static wux::FrameworkElement g_bluetoothButton{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_networkButton{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_soundButton{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_batteryButton{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_compactGroupedButton{nullptr};
struct IconLayers {
    wuc::Grid host{nullptr};
    wuc::FontIcon underlay{nullptr};
    wuc::FontIcon primary{nullptr};
    wuc::FontIcon overlay{nullptr};
};

[[clang::no_destroy]] static IconLayers g_bluetoothIcon;
[[clang::no_destroy]] static IconLayers g_networkIcon;
[[clang::no_destroy]] static wf::IInspectable g_nativeNetworkViewModel{nullptr};
[[clang::no_destroy]] static wf::IInspectable g_nativeSoundViewModel{nullptr};
[[clang::no_destroy]] static wf::IInspectable g_nativeBatteryViewModel{nullptr};
[[clang::no_destroy]] static wuxm::FontFamily g_nativeBatteryFont{nullptr};
static ULONGLONG g_nextNativeBatteryFontSearch = 0;
static double g_nativeBatteryGlyphWidth = NAN;
static double g_nativeBatteryGlyphHeight = NAN;
static double g_nativeBatteryGlyphFontSize = 16;
static bool g_usingNativeBatteryGlyphs = false;
static ULONGLONG g_nextNativeBatterySearch = 0;
[[clang::no_destroy]] static wuxmk::IXamlMetadataProvider g_trayMetadataProvider{nullptr};
static bool g_usingNativeNetworkGlyphs = false;
static ULONGLONG g_nextNativeNetworkSearch = 0;
static bool g_usingNativeSoundGlyphs = false;
static ULONGLONG g_nextNativeSoundSearch = 0;
[[clang::no_destroy]] static IconLayers g_soundIcon;
[[clang::no_destroy]] static IconLayers g_compactGroupedIcon;
[[clang::no_destroy]] static IconLayers g_batteryIcon;
[[clang::no_destroy]] static wux::FrameworkElement g_batteryLeafIcon{nullptr};
[[clang::no_destroy]] static wuc::TextBlock g_batteryPercentageText{nullptr};
[[clang::no_destroy]] static wuc::StackPanel g_batteryContent{nullptr};
static std::atomic<int> g_batteryPercentageEnabled{-1};
static std::wstring g_batteryTooltipCache;


struct MediaTooltipInfo {
    bool hasMedia = false;
    bool paused = false;
    std::wstring appName;
    std::wstring title;
    std::wstring artist;
};

static std::wstring g_bluetoothTooltipCache;
static std::wstring g_networkTooltipCache;
static std::wstring g_soundTooltipCache;
[[clang::no_destroy]] static wuc::MenuFlyout g_activeTrayContextFlyout{nullptr};
[[clang::no_destroy]] static wuc::Panel g_trayPanel{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_trayControlCenterButton{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_originalGroupedButton{nullptr};
[[clang::no_destroy]] static wux::Style g_nativeGroupedButtonStyle{nullptr};
[[clang::no_destroy]] static wux::Style g_nativeNotifyIconStyle{nullptr};
static wux::Visibility g_originalGroupedVisibility = wux::Visibility::Visible;
static double g_originalGroupedWidth = NAN;
static double g_originalGroupedMinWidth = 0;
static double g_originalGroupedMaxWidth = INFINITY;
// Match the compact tray's individual icon width before any ordinary
// NotifyIconView is available. A visible native icon overrides this fallback.
static double g_trayButtonWidth = 28;
static double g_trayButtonHeight = 32;
static int g_notifyMetricDiagnosticCount = 0;
[[clang::no_destroy]] static wux::DispatcherTimer g_updateTimer{nullptr};
static bool g_updateTimerFast = false;
[[clang::no_destroy]] static wux::DispatcherTimer g_retryTimer{nullptr};
[[clang::no_destroy]] static wux::DispatcherTimer g_metricRefreshTimer{nullptr};
static int g_retryCount = 0;
static std::atomic<bool> g_unloading{false};
static HWINEVENTHOOK g_foregroundEventHook = nullptr;
static void CALLBACK TrayFlyoutForegroundChanged(HWINEVENTHOOK, DWORD, HWND,
                                                 LONG, LONG, DWORD, DWORD);
static SRWLOCK g_workerThreadsLock = SRWLOCK_INIT;
static std::vector<HANDLE> g_workerThreads;

static HANDLE StartOwnedWorker(std::function<void()> task) {
    struct Work { std::function<void()> task; };
    AcquireSRWLockExclusive(&g_workerThreadsLock);
    if (g_unloading) { ReleaseSRWLockExclusive(&g_workerThreadsLock); return nullptr; }
    auto work = new (std::nothrow) Work{std::move(task)};
    if (!work) { ReleaseSRWLockExclusive(&g_workerThreadsLock); return nullptr; }
    HANDLE thread = CreateThread(nullptr, 0, +[](void* parameter) WINAPI -> DWORD {
        auto work = static_cast<Work*>(parameter);
        try { work->task(); } catch (...) {}
        delete work;
        return 0;
    }, work, CREATE_SUSPENDED, nullptr);
    if (thread) {
        // Retain only workers that are still executing. The tracked duplicate
        // is needed for unload, but completed workers must not accumulate
        // kernel handles for the whole Explorer session.
        std::erase_if(g_workerThreads, [](HANDLE handle) {
            if (WaitForSingleObject(handle, 0) != WAIT_OBJECT_0) return false;
            CloseHandle(handle);
            return true;
        });
        HANDLE tracked = nullptr;
        if (!DuplicateHandle(GetCurrentProcess(), thread, GetCurrentProcess(),
            &tracked, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            // The worker has not started yet, so it can be discarded without
            // leaving code that unload cannot join.
            TerminateThread(thread, ERROR_NOT_ENOUGH_MEMORY);
            CloseHandle(thread);
            delete work;
            thread = nullptr;
        } else {
            g_workerThreads.push_back(tracked);
            if (ResumeThread(thread) == static_cast<DWORD>(-1)) {
                g_workerThreads.pop_back();
                CloseHandle(tracked);
                TerminateThread(thread, GetLastError());
                CloseHandle(thread);
                delete work;
                thread = nullptr;
            }
        }
    } else delete work;
    ReleaseSRWLockExclusive(&g_workerThreadsLock);
    return thread;
}

static void WaitForOwnedWorkers() {
    std::vector<HANDLE> workers;
    AcquireSRWLockExclusive(&g_workerThreadsLock);
    workers = std::move(g_workerThreads);
    g_workerThreads.clear();
    ReleaseSRWLockExclusive(&g_workerThreadsLock);
    for (auto thread : workers) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

static std::vector<std::function<void()>> g_uiEventRevokers;
static std::vector<std::function<void()>> g_contextMenuEventRevokers;
static std::vector<std::function<void()>> g_timerEventRevokers;
static void RevokeEvents(std::vector<std::function<void()>>& revokers) {
    auto saved = std::move(revokers);
    revokers.clear();
    for (auto& revoke : saved) { try { revoke(); } catch (...) {} }
}
static bool g_dumpedTree = false;
static int g_wifiConnectingFrame = 0;
static SRWLOCK g_mediaTooltipLock = SRWLOCK_INIT;
static MediaTooltipInfo g_mediaTooltipInfo;
static std::atomic<ULONGLONG> g_lastMediaTooltipQueryTick{0};
static std::atomic<bool> g_mediaTooltipQueryInProgress{false};
static bool g_metricRefreshPending = false;
static ULONGLONG g_lastMetricRebuildTick = 0;
[[clang::no_destroy]] static wux::FrameworkElement g_sizeRefreshTrayElement{nullptr};
[[clang::no_destroy]] static wux::FrameworkElement g_sizeRefreshControlCenterButton{nullptr};
static winrt::event_token g_sizeRefreshTrayToken{};
static winrt::event_token g_sizeRefreshControlCenterToken{};

static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
static Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
static HMODULE g_batterySettingsModule = nullptr;
static HMODULE g_powerProfileModule = nullptr;

static std::wstring GetStringSettingWithDefault(PCWSTR name,
                                                PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = *value ? value : fallback;
    Wh_FreeStringSetting(value);
    return result;
}

static std::wstring LoadButtonOrderSetting() {
    std::wstring order;
    for (int index = 0; index < 5; ++index) {
        PCWSTR value = Wh_GetStringSetting(L"buttonOrder[%d]", index);
        if (!*value) {
            Wh_FreeStringSetting(value);
            break;
        }
        if (!order.empty()) order += L",";
        order += value;
        Wh_FreeStringSetting(value);
    }
    return order;
}

static void LoadSettings() {
    g_settings.soundMiddleClickToggleMute =
        GetStringSettingWithDefault(L"sound.middleClickAction", L"toggle_mute") != L"custom";
    g_settings.soundMiddleClickAction = GetStringSettingWithDefault(L"sound.middleClickCustomAction", L"");
    g_settings.bluetoothMiddleClickAction = GetStringSettingWithDefault(L"bluetooth.middleClickCustomAction", L"");
    g_settings.networkMiddleClickAction = GetStringSettingWithDefault(L"network.middleClickCustomAction", L"");
    g_settings.controlCenterMiddleClickAction = GetStringSettingWithDefault(L"controlCenter.middleClickCustomAction", L"");
    g_settings.batteryMiddleClickAction = GetStringSettingWithDefault(L"battery.middleClickCustomAction", L"");
    g_settings.batteryCustomAction = GetStringSettingWithDefault(L"battery.actionMode", L"custom") == L"custom";
    g_settings.batteryAction = g_settings.batteryCustomAction
        ? GetStringSettingWithDefault(L"battery.customAction", L"ms-settings:powersleep")
        : L"ms-controlcenter:";
    g_settings.soundClickAction =
        GetStringSettingWithDefault(L"sound.soundClickAction", L"sound_output");
    const auto volumeStep = GetStringSettingWithDefault(L"sound.volumeWheelStep", L"0");
    g_settings.volumeWheelStep = volumeStep == L"1" ? 1 :
        volumeStep == L"5" ? 5 : volumeStep == L"10" ? 10 : 0;
    g_settings.controlCenterGlyph =
        GetStringSettingWithDefault(L"controlCenter.controlCenterGlyph", L"F4C3");
    g_settings.controlCenterAction =
        GetStringSettingWithDefault(L"controlCenter.controlCenterAction",
                                    L"ms-controlcenter:");
    const auto controlCenterActionMode =
        GetStringSettingWithDefault(L"controlCenter.actionMode", L"default");
    if (controlCenterActionMode == L"project")
        g_settings.controlCenterAction = L"key:Win+P";
    else if (controlCenterActionMode != L"custom")
        g_settings.controlCenterAction = L"ms-controlcenter:";
    g_settings.showConnectedDevicesInTooltip = Wh_GetIntSetting(L"bluetooth.showConnectedDevicesInTooltip") != 0;
    g_settings.changeBluetoothGlyphWhenDisabled = Wh_GetIntSetting(L"bluetooth.changeGlyphWhenDisabled") != 0;
    g_settings.showSignalStrengthInTooltip = Wh_GetIntSetting(L"network.showSignalStrengthInTooltip") != 0;
    g_settings.speedTestUrl = GetStringSettingWithDefault(L"network.speedTestUrl", L"https://www.speedtest.net/run");
    g_settings.showBluetoothButton =
        Wh_GetIntSetting(L"visibility.showBluetoothButton") != 0;
    g_settings.showNetworkButton =
        Wh_GetIntSetting(L"visibility.showNetworkButton") != 0;
    g_settings.showSoundButton =
        Wh_GetIntSetting(L"visibility.showSoundButton") != 0;
    g_settings.showControlCenterButton =
        Wh_GetIntSetting(L"visibility.showControlCenterButton") != 0;
    g_settings.showBatteryButton =
        Wh_GetIntSetting(L"visibility.showBatteryButton") != 0;
    // Preserve the existing stored toggle when upgrading from battery-only sizing.
    g_settings.smallBatteryGlyph = Wh_GetIntSetting(L"battery.smallGlyph") != 0;
    g_settings.batteryMatchOtherButtonSize = Wh_GetIntSetting(L"battery.matchOtherButtonSize") != 0;
    g_settings.energySaverLeafGlyph = Wh_GetIntSetting(L"battery.energySaverLeafGlyph") != 0;
    g_settings.showCurrentlyPlayingInSoundTooltip =
        Wh_GetIntSetting(L"sound.showCurrentlyPlayingInSoundTooltip") != 0;
    g_settings.soundIconFollowsOutputDevice =
        Wh_GetIntSetting(L"sound.soundIconFollowsOutputDevice") != 0;
    g_settings.buttonOrder = LoadButtonOrderSetting();
    g_settings.swapTroubleshootingWithBottom =
        Wh_GetIntSetting(L"swapTroubleshootingWithBottom") != 0;
    Wh_Log(L"Tray button settings: bluetooth=%d network=%d sound=%d order=[%s].",
           g_settings.showBluetoothButton, g_settings.showNetworkButton,
           g_settings.showSoundButton, g_settings.buttonOrder.c_str());
}

static bool IsSystemLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LONG ret = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return ret == ERROR_SUCCESS && value != 0;
}

static wuxm::Brush MakeIconBrush() {
    wu::Color color{};
    color.A = 255;
    if (IsSystemLightTheme()) {
        color.R = 0;
        color.G = 0;
        color.B = 0;
    } else {
        color.R = 255;
        color.G = 255;
        color.B = 255;
    }

    return wuxm::SolidColorBrush(color);
}

[[clang::no_destroy]] static wuxm::Brush g_underlayFallback{nullptr};

static wuxm::Brush MakeUnderlayBrush() {
    // Share the native foreground without changing its opacity: the glyph
    // applies the native 20% underlay opacity independently.
    if (auto native = g_originalGroupedButton.try_as<wuc::Control>()) {
        if (auto brush = native.Foreground()) return brush;
    }
    static bool cachedThemeIsLight = false;
    const bool light = IsSystemLightTheme();
    if (g_underlayFallback && cachedThemeIsLight == light) return g_underlayFallback;
    try {
        auto control = wuxmk::XamlReader::Load(
            LR"(<ContentControl xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" Foreground="{ThemeResource TextFillColorPrimaryBrush}"/>)")
            .as<wuc::ContentControl>();
        if (auto brush = control.Foreground()) {
            cachedThemeIsLight = light;
            g_underlayFallback = brush;
            return g_underlayFallback;
        }
    } catch (...) {}
    wu::Color color{};
    color.A = light ? 0xE4 : 0xFF;
    const BYTE channel = light ? 0x00 : 0xFF;
    color.R = channel;
    color.G = channel;
    color.B = channel;
    cachedThemeIsLight = light;
    g_underlayFallback = wuxm::SolidColorBrush(color);
    return g_underlayFallback;
}

static winrt::hstring GlyphFromHexSetting(std::wstring const& setting,
                                          wchar_t fallbackGlyph) {
    wchar_t const* text = setting.c_str();
    while (*text == L' ' || *text == L'\t' || *text == L'#') {
        ++text;
    }
    if ((text[0] == L'0') && (text[1] == L'x' || text[1] == L'X')) {
        text += 2;
    }

    wchar_t* end = nullptr;
    unsigned long value = wcstoul(text, &end, 16);
    if (end == text || value == 0 || value > 0xFFFF) {
        value = static_cast<unsigned long>(fallbackGlyph);
    }

    wchar_t glyph[2] = {static_cast<wchar_t>(value), 0};
    return winrt::hstring(glyph);
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = static_cast<wchar_t>(towlower(c));
    }
    return s;
}

static std::wstring Trim(std::wstring value) {
    size_t start = 0;
    while (start < value.size() && iswspace(value[start])) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && iswspace(value[end - 1])) {
        --end;
    }

    return value.substr(start, end - start);
}

static bool ContainsCI(std::wstring const& value, PCWSTR needle) {
    return ToLower(value).find(ToLower(needle)) != std::wstring::npos;
}

static bool StartsWithCI(std::wstring const& value, PCWSTR prefix) {
    const size_t length = wcslen(prefix);
    return value.size() >= length &&
           _wcsnicmp(value.c_str(), prefix, length) == 0;
}

static bool EndsWithCI(std::wstring const& value, PCWSTR suffix) {
    const size_t length = wcslen(suffix);
    return value.size() >= length &&
           _wcsicmp(value.c_str() + value.size() - length, suffix) == 0;
}

static std::wstring StripQuotes(std::wstring const& value) {
    if (value.size() >= 2 && value.front() == L'"' &&
        value.back() == L'"') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

static bool GetKnownFolderPath(PCWSTR name, std::wstring& out) {
    // Used by custom actions such as ~Downloads. Resolve through Windows so
    // redirected folders and profiles on other drives remain supported.
    KNOWNFOLDERID folderId{};
    std::wstring lower = ToLower(name);
    if (lower == L"downloads") {
        folderId = FOLDERID_Downloads;
    } else if (lower == L"documents" || lower == L"personal") {
        folderId = FOLDERID_Documents;
    } else if (lower == L"music") {
        folderId = FOLDERID_Music;
    } else if (lower == L"pictures") {
        folderId = FOLDERID_Pictures;
    } else if (lower == L"videos") {
        folderId = FOLDERID_Videos;
    } else if (lower == L"desktop") {
        folderId = FOLDERID_Desktop;
    } else if (lower == L"profile" || lower == L"home") {
        folderId = FOLDERID_Profile;
    } else {
        return false;
    }

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(folderId, 0, nullptr, &raw)) &&
              raw;
    if (ok) {
        out = raw;
    }
    if (raw) {
        CoTaskMemFree(raw);
    }
    return ok;
}

static bool TryParseShortcut(std::wstring_view shortcut,
                             UINT* modifiersOut,
                             UINT* vkOut) {
    auto trim = [](std::wstring_view s) {
        size_t start = 0;
        while (start < s.size() && iswspace(s[start])) {
            ++start;
        }

        size_t end = s.size();
        while (end > start && iswspace(s[end - 1])) {
            --end;
        }

        return s.substr(start, end - start);
    };

    std::unordered_map<std::wstring, UINT> modifiersMap = {
        {L"ALT", MOD_ALT},         {L"CTRL", MOD_CONTROL},
        {L"CONTROL", MOD_CONTROL}, {L"SHIFT", MOD_SHIFT},
        {L"WIN", MOD_WIN},
    };
    std::unordered_map<std::wstring, UINT> vkMap = {
        {L"TAB", VK_TAB},             {L"ENTER", VK_RETURN},
        {L"RETURN", VK_RETURN},       {L"SPACE", VK_SPACE},
        {L"ESC", VK_ESCAPE},          {L"ESCAPE", VK_ESCAPE},
        {L"BACKSPACE", VK_BACK},      {L"HOME", VK_HOME},
        {L"END", VK_END},             {L"PAGEUP", VK_PRIOR},
        {L"PAGEDOWN", VK_NEXT},       {L"INSERT", VK_INSERT},
        {L"DELETE", VK_DELETE},       {L"LEFT", VK_LEFT},
        {L"RIGHT", VK_RIGHT},         {L"UP", VK_UP},
        {L"DOWN", VK_DOWN},           {L"VOLUMEMUTE", VK_VOLUME_MUTE},
        {L"VOLUMEUP", VK_VOLUME_UP},  {L"VOLUMEDOWN", VK_VOLUME_DOWN},
        {L"MEDIAPLAYPAUSE", VK_MEDIA_PLAY_PAUSE},
        {L"MEDIANEXT", VK_MEDIA_NEXT_TRACK},
        {L"MEDIAPREV", VK_MEDIA_PREV_TRACK},
        {L"MEDIASTOP", VK_MEDIA_STOP},
    };

    UINT modifiers = 0;
    UINT vk = 0;
    size_t start = 0;
    while (start <= shortcut.size()) {
        size_t plus = shortcut.find(L'+', start);
        std::wstring_view part =
            plus == std::wstring_view::npos
                ? shortcut.substr(start)
                : shortcut.substr(start, plus - start);
        part = trim(part);
        if (part.empty()) {
            return false;
        }

        std::wstring token(part);
        for (auto& ch : token) {
            ch = static_cast<wchar_t>(towupper(ch));
        }

        auto modIt = modifiersMap.find(token);
        if (modIt != modifiersMap.end()) {
            modifiers |= modIt->second;
        } else {
            if (vk != 0) {
                return false;
            }

            if (token.size() == 1 &&
                ((token[0] >= L'A' && token[0] <= L'Z') ||
                 (token[0] >= L'0' && token[0] <= L'9'))) {
                vk = static_cast<UINT>(token[0]);
            } else if (token.size() >= 2 && token[0] == L'F') {
                int fn = _wtoi(token.c_str() + 1);
                if (fn < 1 || fn > 24) {
                    return false;
                }
                vk = VK_F1 + static_cast<UINT>(fn - 1);
            } else {
                auto vkIt = vkMap.find(token);
                if (vkIt != vkMap.end()) {
                    vk = vkIt->second;
                } else {
                    size_t pos = 0;
                    try {
                        unsigned long parsed = std::stoul(token, &pos, 0);
                        if (pos != token.size() || parsed == 0 ||
                            parsed > 0xFF) {
                            return false;
                        }
                        vk = static_cast<UINT>(parsed);
                    } catch (...) {
                        return false;
                    }
                }
            }
        }

        if (plus == std::wstring_view::npos) {
            break;
        }
        start = plus + 1;
    }

    if (vk == 0) {
        return false;
    }

    *modifiersOut = modifiers;
    *vkOut = vk;
    return true;
}

static bool SendShortcut(UINT modifiers, UINT vk) {
    std::vector<INPUT> inputs;
    inputs.reserve(18);

    auto addKey = [&inputs](WORD key, DWORD flags) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = flags;
        inputs.push_back(input);
    };

    struct Modifier { UINT flag; WORD key; };
    constexpr Modifier allModifiers[] = {
        {MOD_CONTROL, VK_CONTROL}, {MOD_SHIFT, VK_SHIFT},
        {MOD_ALT, VK_MENU}, {MOD_WIN, VK_LWIN},
    };
    std::vector<WORD> released;
    // Prevent an unrelated held modifier (for example Shift) from changing a
    // built-in shortcut into a different command. Restore it afterwards when
    // the physical key is still down.
    for (auto const& modifier : allModifiers) {
        if (!(modifiers & modifier.flag) &&
            (GetAsyncKeyState(modifier.key) & 0x8000)) {
            addKey(modifier.key, KEYEVENTF_KEYUP);
            released.push_back(modifier.key);
        }
    }

    if (modifiers & MOD_CONTROL) addKey(VK_CONTROL, 0);
    if (modifiers & MOD_SHIFT) addKey(VK_SHIFT, 0);
    if (modifiers & MOD_ALT) addKey(VK_MENU, 0);
    if (modifiers & MOD_WIN) addKey(VK_LWIN, 0);

    addKey(static_cast<WORD>(vk), 0);
    addKey(static_cast<WORD>(vk), KEYEVENTF_KEYUP);

    if (modifiers & MOD_WIN) addKey(VK_LWIN, KEYEVENTF_KEYUP);
    if (modifiers & MOD_ALT) addKey(VK_MENU, KEYEVENTF_KEYUP);
    if (modifiers & MOD_SHIFT) addKey(VK_SHIFT, KEYEVENTF_KEYUP);
    if (modifiers & MOD_CONTROL) addKey(VK_CONTROL, KEYEVENTF_KEYUP);

    for (auto it = released.rbegin(); it != released.rend(); ++it) {
        if (GetAsyncKeyState(*it) & 0x8000) addKey(*it, 0);
    }

    UINT sent = SendInput(static_cast<UINT>(inputs.size()), inputs.data(),
                          sizeof(INPUT));
    return sent == inputs.size();
}

// Shell activation and audio-service RPC must never block Explorer's UI.
static void QueueShellWork(std::function<void()> work) {
    HANDLE thread = StartOwnedWorker([work = std::move(work)] {
        const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
            Wh_Log(L"Shell worker COM initialization failed: 0x%08X", hr);
            return;
        }
        struct ApartmentCleanup { ~ApartmentCleanup() { CoUninitialize(); } } cleanup;
        if (!g_unloading) work();
    });
    if (thread) CloseHandle(thread);
}

static void ExecuteActionWorker(std::wstring const& action) {
    if (action.empty()) {
        return;
    }

    if (StartsWithCI(action, L"web:")) {
        ShellExecuteW(nullptr, L"open", action.substr(4).c_str(), nullptr,
                      nullptr, SW_SHOWNORMAL);
        return;
    }
    if (StartsWithCI(action, L"ms-settings:") ||
        StartsWithCI(action, L"ms-controlcenter:") ||
        StartsWithCI(action, L"ms-availablenetworks:") ||
        StartsWithCI(action, L"ms-actioncenter:")) {
        ShellExecuteW(nullptr, L"open", action.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
        return;
    }
    if (StartsWithCI(action, L"cmd:")) {
        std::wstring args = L"/C " + action.substr(4);
        ShellExecuteW(nullptr, L"open", L"cmd.exe", args.c_str(), nullptr,
                      SW_HIDE);
        return;
    }
    if (StartsWithCI(action, L"ps:") || StartsWithCI(action, L"powershell:")) {
        const size_t prefixLength = StartsWithCI(action, L"ps:") ? 3 : 11;
        std::wstring args =
            L"-NoProfile -ExecutionPolicy Bypass -Command " +
            action.substr(prefixLength);
        ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(),
                      nullptr, SW_HIDE);
        return;
    }
    if (StartsWithCI(action, L"key:") || StartsWithCI(action, L"hotkey:")) {
        std::wstring shortcut =
            StartsWithCI(action, L"key:") ? action.substr(4)
                                           : action.substr(7);
        UINT modifiers = 0;
        UINT vk = 0;
        if (!TryParseShortcut(shortcut, &modifiers, &vk)) {
            Wh_Log(L"Invalid replacement button shortcut action: %s",
                   shortcut.c_str());
            return;
        }
        if (!SendShortcut(modifiers, vk)) {
            Wh_Log(L"Failed to send replacement button shortcut: %s",
                   shortcut.c_str());
        }
        return;
    }
    if (!action.empty() && action.front() == L'~') {
        std::wstring target = action.substr(1);
        std::wstring resolved;
        if (GetKnownFolderPath(target.c_str(), resolved)) {
            ShellExecuteW(nullptr, L"open", resolved.c_str(), nullptr, nullptr,
                          SW_SHOWNORMAL);
            return;
        }
        wchar_t buffer[MAX_PATH * 4]{};
        if (SearchPathW(nullptr, target.c_str(), nullptr, ARRAYSIZE(buffer),
                        buffer, nullptr)) {
            ShellExecuteW(nullptr, L"open", buffer, nullptr, nullptr,
                          SW_SHOWNORMAL);
            return;
        }
        Wh_Log(L"Replacement button ~search target not found: %s",
               target.c_str());
        return;
    }

    std::wstring path = StripQuotes(action);
    ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr,
                  SW_SHOWNORMAL);
    return;
}


static void ExecuteAction(std::wstring const& action,
                          std::function<void()> launched = {}) {
    if (action.empty()) return;
    QueueShellWork([action, launched = std::move(launched)] {
        ExecuteActionWorker(action);
        if (launched && !g_unloading) launched();
    });
}

static void LaunchUri(PCWSTR uri, std::function<void()> launched = {}) {
    ExecuteAction(uri, std::move(launched));
}

static void OpenBluetooth(std::function<void()> launched = {}) {
    LaunchUri(L"ms-controlcenter:bluetooth", std::move(launched));
}

static void OpenAddBluetoothDevice() {
    QueueShellWork([] {
        HINSTANCE result = ShellExecuteW(nullptr, L"open", L"DevicePairingWizard.exe",
                                         L"/bluetooth", nullptr, SW_SHOWNORMAL);
        Wh_Log(L"ShellExecuteW(DevicePairingWizard.exe /bluetooth) returned %p", result);
    });
}

static void OpenNetwork(std::function<void()> launched = {}) {
    LaunchUri(L"ms-availablenetworks:", std::move(launched));
}

static void OpenSoundOutput() {
    // Ctrl+Win+V is Windows' native sound-output picker command.
    if (!SendShortcut(MOD_CONTROL | MOD_WIN, 'V'))
        Wh_Log(L"Failed to invoke the native sound output picker shortcut.");
}

static std::atomic<int> g_airplaneModeState{-1};
static std::atomic<bool> g_airplaneModeQueryBusy{false};

static bool QueryAirplaneModeLikelyEnabled() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool coInitialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) hr = S_OK;
    if (FAILED(hr)) return false;
    try {
        auto access = wdr::Radio::RequestAccessAsync().get();
        if (access != wdr::RadioAccessStatus::Allowed) {
            if (coInitialized) CoUninitialize();
            return false;
        }

        auto radios = wdr::Radio::GetRadiosAsync().get();
        bool sawWirelessRadio = false;
        bool sawOnWirelessRadio = false;
        for (auto const& radio : radios) {
            auto kind = radio.Kind();
            if (kind != wdr::RadioKind::WiFi &&
                kind != wdr::RadioKind::Bluetooth &&
                kind != wdr::RadioKind::MobileBroadband) {
                continue;
            }

            sawWirelessRadio = true;
            if (radio.State() == wdr::RadioState::On) {
                sawOnWirelessRadio = true;
            }
        }

        const bool enabled = sawWirelessRadio && !sawOnWirelessRadio;
        if (coInitialized) CoUninitialize();
        return enabled;
    } catch (...) {
        if (coInitialized) CoUninitialize();
        Wh_Log(L"Airplane mode state query failed: 0x%08X", winrt::to_hresult());
        return false;
    }
}

static void RefreshAirplaneModeAsync() {
    if (g_airplaneModeState.load() >= 0 || g_airplaneModeQueryBusy.exchange(true)) return;
    HANDLE thread = StartOwnedWorker([] {
        const bool enabled = QueryAirplaneModeLikelyEnabled();
        g_airplaneModeState.store(enabled ? 1 : 0);
        g_airplaneModeQueryBusy.store(false);
        RequestTrayRefresh(RefreshNetwork);
    });
    if (thread) CloseHandle(thread);
    else g_airplaneModeQueryBusy.store(false);
}

struct RadioChangeWork {
    bool enabled;
};

static DWORD WINAPI SetAirplaneModeLikelyEnabledThreadProc(void* param) {
    const auto work = *static_cast<RadioChangeWork*>(param);
    delete static_cast<RadioChangeWork*>(param);
    const bool enabled = work.enabled;
    bool coInitialized = false;

    try {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        coInitialized = SUCCEEDED(hr);
        if (hr == RPC_E_CHANGED_MODE) hr = S_OK;
        if (FAILED(hr)) winrt::throw_hresult(hr);

        auto access = wdr::Radio::RequestAccessAsync().get();
        if (access == wdr::RadioAccessStatus::Allowed) {
            auto radios = wdr::Radio::GetRadiosAsync().get();
            const auto targetState =
                enabled ? wdr::RadioState::Off : wdr::RadioState::On;
            for (auto const& radio : radios) {
                auto kind = radio.Kind();
                if (kind == wdr::RadioKind::WiFi ||
                    kind == wdr::RadioKind::Bluetooth ||
                    kind == wdr::RadioKind::MobileBroadband) {
                    radio.SetStateAsync(targetState).get();
                }
            }
        }

    } catch (...) {
        // A worker may finish after Windhawk has begun unloading the mod.
    }
    if (coInitialized) CoUninitialize();
    g_airplaneModeState.store(enabled ? 0 : 1);
    RequestTrayRefresh(RefreshRadios | RefreshNetwork | RefreshAudio | RefreshAudioDevices);
    return 0;
}

static void SetAirplaneModeLikelyEnabled(bool enabled) {
    if (g_unloading) return;
    auto* value = new (std::nothrow) RadioChangeWork{enabled};
    HANDLE thread = value ? StartOwnedWorker([value] {
        SetAirplaneModeLikelyEnabledThreadProc(value);
    }) : nullptr;
    if (thread) {
        CloseHandle(thread);
    } else {
        delete value;
    }
}

struct WindowSearchByPid {
    DWORD pid = 0;
    HWND hwnd = nullptr;
};

static BOOL CALLBACK FindVisibleWindowByPidProc(HWND hwnd, LPARAM lParam) {
    auto* search = reinterpret_cast<WindowSearchByPid*>(lParam);
    if (!search || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != search->pid) {
        return TRUE;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect) ||
        rect.right - rect.left < 100 || rect.bottom - rect.top < 100) {
        return TRUE;
    }

    search->hwnd = hwnd;
    return FALSE;
}

static HWND FindVisibleWindowByPid(DWORD pid) {
    WindowSearchByPid search{};
    search.pid = pid;
    EnumWindows(FindVisibleWindowByPidProc, reinterpret_cast<LPARAM>(&search));
    return search.hwnd;
}

static int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void PositionWindowNearTaskbar(HWND hwnd, PCWSTR label) {
    if (!hwnd) {
        return;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        Wh_Log(L"%s placement: Shell_TrayWnd not found.", label);
        return;
    }

    RECT taskbarRect{};
    RECT mixerRect{};
    if (!GetWindowRect(taskbar, &taskbarRect) ||
        !GetWindowRect(hwnd, &mixerRect)) {
        Wh_Log(L"%s placement: failed to read window rectangles.", label);
        return;
    }

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        Wh_Log(L"sndvol placement: GetMonitorInfoW failed.");
        return;
    }

    const int gap = 10;
    const int mixerWidth = mixerRect.right - mixerRect.left;
    const int mixerHeight = mixerRect.bottom - mixerRect.top;
    const int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;

    enum class TaskbarEdge {
        Top,
        Bottom,
        Left,
        Right,
    };

    TaskbarEdge edge = TaskbarEdge::Bottom;
    if (taskbarWidth >= taskbarHeight) {
        const int distanceToTop =
            abs(taskbarRect.top - monitorInfo.rcMonitor.top);
        const int distanceToBottom =
            abs(monitorInfo.rcMonitor.bottom - taskbarRect.bottom);
        edge = distanceToTop <= distanceToBottom ? TaskbarEdge::Top
                                                 : TaskbarEdge::Bottom;
    } else {
        const int distanceToLeft =
            abs(taskbarRect.left - monitorInfo.rcMonitor.left);
        const int distanceToRight =
            abs(monitorInfo.rcMonitor.right - taskbarRect.right);
        edge = distanceToLeft <= distanceToRight ? TaskbarEdge::Left
                                                 : TaskbarEdge::Right;
    }

    int x = taskbarRect.right - mixerWidth - gap;
    int y = taskbarRect.bottom + gap;

    switch (edge) {
        case TaskbarEdge::Top:
            x = taskbarRect.right - mixerWidth - gap;
            y = taskbarRect.bottom + gap;
            break;
        case TaskbarEdge::Bottom:
            x = taskbarRect.right - mixerWidth - gap;
            y = taskbarRect.top - mixerHeight - gap;
            break;
        case TaskbarEdge::Left:
            x = taskbarRect.right + gap;
            y = taskbarRect.bottom - mixerHeight - gap;
            break;
        case TaskbarEdge::Right:
            x = taskbarRect.left - mixerWidth - gap;
            y = taskbarRect.bottom - mixerHeight - gap;
            break;
    }

    x = ClampInt(x, monitorInfo.rcWork.left + gap,
                 monitorInfo.rcWork.right - mixerWidth - gap);
    y = ClampInt(y, monitorInfo.rcWork.top + gap,
                 monitorInfo.rcWork.bottom - mixerHeight - gap);

    SetWindowPos(hwnd, HWND_TOP, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    Wh_Log(L"%s placement: moved hwnd=%p to %d,%d near taskbar rect=%ld,%ld,%ld,%ld monitor=%ldx%ld.",
           label, hwnd, x, y, taskbarRect.left, taskbarRect.top,
           taskbarRect.right, taskbarRect.bottom, monitorWidth, monitorHeight);
}

struct PositionNearTaskbarRequest {
    HANDLE process = nullptr;
    std::wstring label = L"window";
};

static DWORD WINAPI PositionWindowNearTaskbarThreadProc(void* param) {
    auto* request = static_cast<PositionNearTaskbarRequest*>(param);
    HANDLE process = request ? request->process : nullptr;
    std::wstring label = request ? std::move(request->label) : L"window";
    delete request;
    if (!process) {
        return 0;
    }

    DWORD pid = GetProcessId(process);
    // Keep teardown responsive: placement is cosmetic and must yield quickly
    // once the mod is unloading.
    for (int i = 0; i < 20 && !g_unloading; ++i) {
        const DWORD idle = WaitForInputIdle(process, 100);
        if (idle == 0 || idle == WAIT_FAILED) break;
    }

    HWND hwnd = nullptr;
    for (int i = 0; i < 40 && !hwnd && !g_unloading; ++i) {
        hwnd = FindVisibleWindowByPid(pid);
        if (!hwnd) {
            Sleep(50);
        }
    }

    if (hwnd) {
        PositionWindowNearTaskbar(hwnd, label.c_str());
    } else {
        Wh_Log(L"%s placement: no visible top-level window found for pid=%lu.",
               label.c_str(), pid);
    }

    CloseHandle(process);
    return 0;
}

static void OpenVolumeMixer() {
    QueueShellWork([] {
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.lpVerb = L"open";
        sei.lpFile = L"sndvol.exe";
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteExW(&sei)) {
            Wh_Log(L"ShellExecuteExW(sndvol.exe) failed: %lu", GetLastError());
            return;
        }

        Wh_Log(L"ShellExecuteExW(sndvol.exe) process=%p", sei.hProcess);
        if (sei.hProcess) {
            auto* request = new (std::nothrow) PositionNearTaskbarRequest{
                sei.hProcess, L"sndvol"};
            HANDLE thread = request ? StartOwnedWorker([request] { PositionWindowNearTaskbarThreadProc(request); })
                                    : nullptr;
            if (thread) {
                CloseHandle(thread);
            } else {
                Wh_Log(L"sndvol placement: CreateThread failed: %lu",
                       GetLastError());
                delete request;
                CloseHandle(sei.hProcess);
            }
        }
    });
}


static void OpenControlPanelWindow(PCWSTR parameters, PCWSTR label) {
    QueueShellWork([parameters = std::wstring(parameters), label = std::wstring(label)] {
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.lpVerb = L"open";
        sei.lpFile = L"control.exe";
        sei.lpParameters = parameters.c_str();
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteExW(&sei)) {
            Wh_Log(L"ShellExecuteExW(control.exe %s) failed: %lu", parameters.c_str(),
                   GetLastError());
            return;
        }

        if (sei.hProcess) {
            auto* request = new (std::nothrow) PositionNearTaskbarRequest{
                sei.hProcess, label};
            HANDLE thread = request ? StartOwnedWorker([request] { PositionWindowNearTaskbarThreadProc(request); })
                                    : nullptr;
            if (thread) {
                CloseHandle(thread);
            } else {
                Wh_Log(L"%s placement: CreateThread failed: %lu", label.c_str(),
                       GetLastError());
                delete request;
                CloseHandle(sei.hProcess);
            }
        }
    });
}


static void OpenSoundsControlPanel() {
    OpenControlPanelWindow(L"mmsys.cpl,,2", L"sounds control panel");
}

static void OpenBluetoothFileTransfer(bool send) {
    QueueShellWork([send] {
        ShellExecuteW(nullptr, L"open", L"fsquirt.exe",
                      send ? L"-send" : L"-receive", nullptr, SW_SHOWNORMAL);
    });
}

static void OpenSound(std::function<void()> launched = {}) {
    if (_wcsicmp(g_settings.soundClickAction.c_str(), L"sound_output") == 0) {
        OpenSoundOutput();
        if (launched) launched();
        return;
    }

    if (_wcsicmp(g_settings.soundClickAction.c_str(), L"sndvol") == 0) {
        OpenVolumeMixer();
        return;
    }

    LaunchUri(L"ms-controlcenter:", std::move(launched));
}

static std::wstring GetAudioEndpointFriendlyName(IMMDevice* device) {
    if (!device) {
        return {};
    }

    IPropertyStore* properties = nullptr;
    HRESULT hr = device->OpenPropertyStore(STGM_READ, &properties);
    if (FAILED(hr) || !properties) {
        return {};
    }

    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring name;
    if (SUCCEEDED(properties->GetValue(PKEY_Device_FriendlyName, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        name = value.pwszVal;
    }

    PropVariantClear(&value);
    properties->Release();
    return name;
}

static constexpr PROPERTYKEY kAudioEndpointFormFactorKey = {
    {0x1da5d803,
     0xd492,
     0x4edd,
     {0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e}},
    0};

static EndpointFormFactor GetAudioEndpointFormFactor(IMMDevice* device) {
    if (!device) {
        return UnknownFormFactor;
    }

    IPropertyStore* properties = nullptr;
    HRESULT hr = device->OpenPropertyStore(STGM_READ, &properties);
    if (FAILED(hr) || !properties) {
        return UnknownFormFactor;
    }

    PROPVARIANT value;
    PropVariantInit(&value);
    EndpointFormFactor formFactor = UnknownFormFactor;
    if (SUCCEEDED(properties->GetValue(kAudioEndpointFormFactorKey,
                                       &value))) {
        if (value.vt == VT_UI4 || value.vt == VT_UINT) {
            formFactor = static_cast<EndpointFormFactor>(value.ulVal);
        } else if (value.vt == VT_I4 || value.vt == VT_INT) {
            formFactor = static_cast<EndpointFormFactor>(value.lVal);
        }
    }

    PropVariantClear(&value);
    properties->Release();
    return formFactor;
}

static SoundState GetSoundState() {
    SoundState state{};

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInitialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) {
        hr = S_OK;
    }
    if (FAILED(hr)) {
        return state;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator),
                          reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        if (coInitialized) {
            CoUninitialize();
        }
        return state;
    }

    IMMDevice* device = nullptr;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    enumerator->Release();
    if (FAILED(hr) || !device) {
        if (coInitialized) {
            CoUninitialize();
        }
        return state;
    }

    state.outputName = GetAudioEndpointFriendlyName(device);
    state.outputFormFactor = GetAudioEndpointFormFactor(device);

    IAudioEndpointVolume* volume = nullptr;
    hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                          reinterpret_cast<void**>(&volume));
    device->Release();
    if (FAILED(hr) || !volume) {
        if (coInitialized) {
            CoUninitialize();
        }
        return state;
    }

    BOOL muted = FALSE;
    float level = 0.0f;
    if (SUCCEEDED(volume->GetMute(&muted)) &&
        SUCCEEDED(volume->GetMasterVolumeLevelScalar(&level))) {
        state.available = true;
        state.muted = muted != FALSE;
        if (level < 0.0f) {
            level = 0.0f;
        } else if (level > 1.0f) {
            level = 1.0f;
        }
        state.volume = level;
    }

    volume->Release();
    if (coInitialized) {
        CoUninitialize();
    }
    return state;
}

static void StepDefaultEndpointVolumeWorker(IAudioEndpointVolume* volume, int steps, int configuredStep) {
    if (!steps) return;
    const bool up = steps > 0;
    if (!volume) return;

    BOOL muted = FALSE;
    HRESULT hr = volume->GetMute(&muted);
    if (SUCCEEDED(hr) && muted) {
        hr = volume->SetMute(FALSE, nullptr);
    }

    if (SUCCEEDED(hr)) {
        float current = 0;
        if (configuredStep > 0 &&
            SUCCEEDED(volume->GetMasterVolumeLevelScalar(&current))) {
            const float step = static_cast<float>(configuredStep) * steps / 100.0f;
            const float target = (std::clamp)(current + step, 0.0f, 1.0f);
            hr = volume->SetMasterVolumeLevelScalar(target, nullptr);
        } else {
            for (int remaining = std::abs(steps); remaining && SUCCEEDED(hr); --remaining)
                hr = up ? volume->VolumeStepUp(nullptr)
                        : volume->VolumeStepDown(nullptr);
        }
    }
    Wh_Log(L"Sound wheel: VolumeStep%s returned 0x%08X", up ? L"Up" : L"Down",
           hr);

}

static void ToggleDefaultEndpointMuteWorker(IAudioEndpointVolume* volume) {
    if (!volume) return;

    BOOL muted = FALSE;
    HRESULT hr = volume->GetMute(&muted);
    if (SUCCEEDED(hr)) {
        hr = volume->SetMute(!muted, nullptr);
    }
    Wh_Log(L"Sound middle-click mute toggle returned 0x%08X", hr);

}

static std::vector<AudioOutputEndpoint> GetActiveAudioOutputEndpoints() {
    std::vector<AudioOutputEndpoint> result;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool coInitialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) {
        hr = S_OK;
    }
    if (FAILED(hr)) {
        return result;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator),
                          reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        if (coInitialized) CoUninitialize();
        return result;
    }

    std::wstring defaultId;
    IMMDevice* defaultDevice = nullptr;
    if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                      &defaultDevice)) &&
        defaultDevice) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(defaultDevice->GetId(&id)) && id) {
            defaultId = id;
            CoTaskMemFree(id);
        }
        defaultDevice->Release();
    }

    IMMDeviceCollection* devices = nullptr;
    if (SUCCEEDED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                                  &devices)) &&
        devices) {
        UINT count = 0;
        devices->GetCount(&count);
        for (UINT index = 0; index < count; ++index) {
            IMMDevice* device = nullptr;
            if (FAILED(devices->Item(index, &device)) || !device) {
                continue;
            }

            LPWSTR id = nullptr;
            if (SUCCEEDED(device->GetId(&id)) && id) {
                AudioOutputEndpoint endpoint;
                endpoint.id = id;
                endpoint.name = GetAudioEndpointFriendlyName(device);
                endpoint.isDefault = endpoint.id == defaultId;
                if (endpoint.name.empty()) {
                    endpoint.name = L"Audio output";
                }
                result.push_back(std::move(endpoint));
                CoTaskMemFree(id);
            }
            device->Release();
        }
        devices->Release();
    }

    enumerator->Release();
    if (coInitialized) CoUninitialize();
    return result;
}

// The endpoint-selection API is undocumented but stable across modern
// Windows releases. It is what many volume-control utilities use to change
// the default playback device without opening Settings.
struct IPolicyConfigVista : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT,
                                                       WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*,
                                                       WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT,
                                                           INT64*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                        PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                        const PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR, ERole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

struct IPolicyConfig : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT,
                                                       WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*,
                                                       WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT,
                                                           INT64*, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, INT64*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                        PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&,
                                                        const PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR, ERole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

template <typename TPolicy>
static HRESULT SetDefaultAudioOutputWithPolicy(TPolicy* policy,
                                               std::wstring const& id) {
    if (!policy) {
        return E_POINTER;
    }

    HRESULT hr = policy->SetDefaultEndpoint(id.c_str(), eConsole);
    HRESULT hrMultimedia = policy->SetDefaultEndpoint(id.c_str(), eMultimedia);
    HRESULT hrCommunications =
        policy->SetDefaultEndpoint(id.c_str(), eCommunications);
    if (FAILED(hr)) {
        return hr;
    }
    if (FAILED(hrMultimedia)) {
        return hrMultimedia;
    }
    return hrCommunications;
}

static void SetDefaultAudioOutputWorker(std::wstring const& id) {
    if (id.empty()) {
        return;
    }

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool coInitialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) hr = S_OK;
    if (FAILED(hr)) return;

    HRESULT finalHr = E_NOINTERFACE;

    const CLSID clsidPolicyConfig =
        {0x870AF99C, 0x171D, 0x4F9E,
         {0xAF, 0x0D, 0xE6, 0x3D, 0xF4, 0x0C, 0x2B, 0xC9}};
    const IID iidPolicyConfig =
        {0xF8679F50, 0x850A, 0x41CF,
         {0x9C, 0x72, 0x43, 0x0F, 0x29, 0x02, 0x90, 0xC8}};
    IPolicyConfig* policy = nullptr;
    hr = CoCreateInstance(clsidPolicyConfig, nullptr, CLSCTX_ALL,
                          iidPolicyConfig,
                          reinterpret_cast<void**>(&policy));
    if (SUCCEEDED(hr) && policy) {
        finalHr = SetDefaultAudioOutputWithPolicy(policy, id);
        Wh_Log(L"Set default audio output with IPolicyConfig [%s]: 0x%08X",
               id.c_str(), finalHr);
        policy->Release();
    } else {
        Wh_Log(L"CoCreateInstance(IPolicyConfig) failed: 0x%08X", hr);
    }

    if (FAILED(finalHr)) {
        const CLSID clsidPolicyConfigVista =
        {0x294935CE, 0xF637, 0x4E7C,
         {0xA4, 0x1B, 0xAB, 0x25, 0x5B, 0xE8, 0x4F, 0xC6}};
        const IID iidPolicyConfigVista =
        {0x568B9108, 0x44BF, 0x40B4,
         {0x90, 0x06, 0x86, 0xAF, 0xE5, 0xB5, 0xA6, 0x20}};
        IPolicyConfigVista* vistaPolicy = nullptr;
        hr = CoCreateInstance(clsidPolicyConfigVista, nullptr, CLSCTX_ALL,
                              iidPolicyConfigVista,
                              reinterpret_cast<void**>(&vistaPolicy));
        if (SUCCEEDED(hr) && vistaPolicy) {
            finalHr = SetDefaultAudioOutputWithPolicy(vistaPolicy, id);
            Wh_Log(L"Set default audio output with IPolicyConfigVista [%s]: 0x%08X",
                   id.c_str(), finalHr);
            vistaPolicy->Release();
        } else {
            Wh_Log(L"CoCreateInstance(IPolicyConfigVista) failed: 0x%08X", hr);
        }
    }

    Wh_Log(L"Set default audio output final [%s]: 0x%08X", id.c_str(),
           finalHr);
    if (coInitialized) CoUninitialize();
}

enum class AudioWorkKind { Volume, Mute, Output };
struct AudioWork {
    AudioWorkKind kind;
    int steps = 0;
    int configuredStep = 0;
    std::wstring outputId;
};
static std::mutex g_audioWorkLock;
static std::deque<AudioWork> g_audioWork;

static void AppendAudioWork(std::deque<AudioWork>& queue, AudioWork work) {
    if (work.kind == AudioWorkKind::Volume && !queue.empty()) {
        auto& last = queue.back();
        // Preserve direction changes at 0/100%, mute/output boundaries, and
        // the step setting captured at the time of the input.
        if (last.kind == AudioWorkKind::Volume &&
            last.configuredStep == work.configuredStep &&
            (last.steps > 0) == (work.steps > 0) &&
            std::abs(static_cast<long long>(last.steps) + work.steps) <= INT_MAX) {
            last.steps += work.steps;
            return;
        }
    }
    queue.push_back(std::move(work));
}

static void QueueAudioWork(AudioWork work) {
    {
        std::lock_guard lock(g_audioWorkLock);
        if (g_unloading) return;
        AppendAudioWork(g_audioWork, std::move(work));
    }
    RequestTrayRefresh(RefreshAudioWork);
}

static std::deque<AudioWork> TakeAudioWork() {
    std::lock_guard lock(g_audioWorkLock);
    std::deque<AudioWork> work;
    work.swap(g_audioWork);
    return work;
}

static void StepDefaultEndpointVolume(int steps) {
    if (!steps) return;
    QueueAudioWork({AudioWorkKind::Volume, steps, g_settings.volumeWheelStep, {}});
}

static void ToggleDefaultEndpointMute() {
    QueueAudioWork({AudioWorkKind::Mute});
}

static void SetDefaultAudioOutput(std::wstring id) {
    if (id.empty()) return;
    QueueAudioWork({AudioWorkKind::Output, 0, 0, std::move(id)});
}

static bool IsPhysicalEthernet(MIB_IF_ROW2 const& row) {
    return row.Type == IF_TYPE_ETHERNET_CSMACD &&
           row.InterfaceAndOperStatusFlags.HardwareInterface &&
           !row.InterfaceAndOperStatusFlags.FilterInterface &&
           row.MediaConnectState == MediaConnectStateConnected;
}

static bool IsBluetoothAvailable() {
    // Radio enumeration reports the user-visible Bluetooth toggle state. The
    // legacy Bluetooth APIs below only answer whether a hardware radio exists.
    try {
        auto radios = wdr::Radio::GetRadiosAsync().get();
        for (auto const& radio : radios) {
            if (radio.Kind() == wdr::RadioKind::Bluetooth) {
                return radio.State() == wdr::RadioState::On;
            }
        }
        return false;
    } catch (...) {
        // Keep the legacy probe as a compatibility fallback for builds where
        // Windows does not expose the Radio API to Explorer.
    }

    BLUETOOTH_FIND_RADIO_PARAMS params{};
    params.dwSize = sizeof(params);

    HANDLE radio = nullptr;
    HBLUETOOTH_RADIO_FIND find = BluetoothFindFirstRadio(&params, &radio);
    if (!find) {
        return false;
    }

    if (radio) {
        CloseHandle(radio);
    }
    BluetoothFindRadioClose(find);
    return true;
}

static bool HasInternetAccess() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInitialized = SUCCEEDED(hr);
    if (hr == RPC_E_CHANGED_MODE) {
        hr = S_OK;
    }
    if (FAILED(hr)) {
        return false;
    }

    INetworkListManager* networkListManager = nullptr;
    hr = CoCreateInstance(CLSID_NetworkListManager, nullptr, CLSCTX_ALL,
                          IID_PPV_ARGS(&networkListManager));
    if (FAILED(hr) || !networkListManager) {
        if (coInitialized) {
            CoUninitialize();
        }
        return false;
    }

    NLM_CONNECTIVITY connectivity = NLM_CONNECTIVITY_DISCONNECTED;
    bool hasInternet =
        SUCCEEDED(networkListManager->GetConnectivity(&connectivity)) &&
        ((connectivity & NLM_CONNECTIVITY_IPV4_INTERNET) ||
         (connectivity & NLM_CONNECTIVITY_IPV6_INTERNET));

    networkListManager->Release();
    if (coInitialized) {
        CoUninitialize();
    }
    return hasInternet;
}

static std::wstring Dot11SsidToString(DOT11_SSID const& ssid) {
    if (ssid.uSSIDLength == 0) {
        return {};
    }

    char const* bytes = reinterpret_cast<char const*>(ssid.ucSSID);
    int byteCount = static_cast<int>(ssid.uSSIDLength);
    int wideCount =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes, byteCount,
                            nullptr, 0);
    UINT codePage = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    if (wideCount <= 0) {
        codePage = CP_ACP;
        flags = 0;
        wideCount =
            MultiByteToWideChar(codePage, flags, bytes, byteCount, nullptr, 0);
    }

    if (wideCount <= 0) {
        return {};
    }

    std::wstring result(static_cast<size_t>(wideCount), L'\0');
    MultiByteToWideChar(codePage, flags, bytes, byteCount, result.data(),
                        wideCount);
    return result;
}

static NetworkState GetNetworkState() {
    NetworkState state{};
    state.internetAccess = HasInternetAccess();
    bool sawWifi = false;
    bool sawEnabledWifi = false;

    HANDLE wlan = nullptr;
    DWORD negotiatedVersion = 0;
    if (WlanOpenHandle(2, nullptr, &negotiatedVersion, &wlan) == ERROR_SUCCESS) {
        PWLAN_INTERFACE_INFO_LIST interfaces = nullptr;
        if (WlanEnumInterfaces(wlan, nullptr, &interfaces) == ERROR_SUCCESS &&
            interfaces) {
            for (DWORD i = 0; i < interfaces->dwNumberOfItems; ++i) {
                auto const& iface = interfaces->InterfaceInfo[i];
                sawWifi = true;

                if (iface.isState != wlan_interface_state_not_ready) {
                    sawEnabledWifi = true;
                }

                if (iface.isState == wlan_interface_state_associating ||
                    iface.isState == wlan_interface_state_discovering ||
                    iface.isState == wlan_interface_state_authenticating) {
                    state.kind = NetworkKind::WifiConnecting;
                }

                if (iface.isState != wlan_interface_state_connected) {
                    continue;
                }

                DWORD dataSize = 0;
                WLAN_OPCODE_VALUE_TYPE opcode{};
                PWLAN_CONNECTION_ATTRIBUTES attrs = nullptr;
                if (WlanQueryInterface(
                        wlan, &iface.InterfaceGuid,
                        wlan_intf_opcode_current_connection, nullptr,
                        &dataSize, reinterpret_cast<PVOID*>(&attrs),
                        &opcode) == ERROR_SUCCESS &&
                    attrs) {
                    state.kind = NetworkKind::Wifi;
                    state.signal =
                        attrs->wlanAssociationAttributes.wlanSignalQuality;
                    state.name = Dot11SsidToString(
                        attrs->wlanAssociationAttributes.dot11Ssid);
                    WlanFreeMemory(attrs);
                    break;
                }
            }
            WlanFreeMemory(interfaces);
        }
        WlanCloseHandle(wlan, nullptr);
    }

    if (state.kind == NetworkKind::Wifi) {
        return state;
    }

    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) == NO_ERROR && table) {
        for (ULONG i = 0; i < table->NumEntries; ++i) {
            if (IsPhysicalEthernet(table->Table[i])) {
                state.kind = NetworkKind::Ethernet;
                break;
            }
        }
        FreeMibTable(table);
    }
    if (state.kind == NetworkKind::Ethernet ||
        state.kind == NetworkKind::WifiConnecting) {
        return state;
    }

    if (sawWifi) {
        state.kind =
            sawEnabledWifi ? NetworkKind::WifiDisconnected
                           : NetworkKind::WifiDisabled;
    }

    return state;
}

static StatusSnapshot GetStatusSnapshot() {
    AcquireSRWLockShared(&g_statusSnapshotLock);
    StatusSnapshot snapshot = g_statusSnapshot;
    ReleaseSRWLockShared(&g_statusSnapshotLock);
    return snapshot;
}

static void RefreshStatusSnapshot(unsigned reasons) {
    // Preserve domains that didn't change, including their last good value if
    // an individual service query fails. Only the status thread publishes.
    StatusSnapshot snapshot = GetStatusSnapshot();
    if (!snapshot.ready) reasons |= RefreshAll;
    auto refresh = [&](unsigned domain, auto&& collect) {
        if (!(reasons & domain)) return;
        try { collect(); }
        catch (...) {
            Wh_Log(L"Status domain %u refresh failed: 0x%08X", domain, winrt::to_hresult());
        }
    };
    refresh(RefreshRadios, [&] { snapshot.bluetoothAvailable = IsBluetoothAvailable(); });
    refresh(RefreshNetwork, [&] { snapshot.network = GetNetworkState(); });
    refresh(RefreshAudio, [&] { snapshot.sound = GetSoundState(); });
    refresh(RefreshAudioDevices, [&] { snapshot.audioOutputs = GetActiveAudioOutputEndpoints(); });
    refresh(RefreshMedia, [&] { RefreshMediaTooltipInfoFromStatusWorker(); });
    snapshot.ready = true;
    AcquireSRWLockExclusive(&g_statusSnapshotLock);
    g_statusSnapshot = std::move(snapshot);
    ReleaseSRWLockExclusive(&g_statusSnapshotLock);
}

static BOOL CALLBACK FindTaskbarWndProc(HWND hwnd, LPARAM lp) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }

    WCHAR className[64]{};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lp) = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hwnd = nullptr;
    EnumWindows(FindTaskbarWndProc, reinterpret_cast<LPARAM>(&hwnd));
    return hwnd;
}

static wux::XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!taskbarWnd || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original) {
        return nullptr;
    }

    HWND taskSwWnd = reinterpret_cast<HWND>(
        GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        Wh_Log(L"GetTaskbarXamlRoot: TaskbandHWND not found.");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    if (!taskBand) {
        Wh_Log(L"GetTaskbarXamlRoot: taskBand pointer not found.");
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
                    CTaskBand_ITaskListWndSite_vftable;
         ++i) {
        if (i == 20) {
            Wh_Log(L"GetTaskbarXamlRoot: ITaskListWndSite vftable not found.");
            return nullptr;
        }
        taskBandForTaskListWndSite =
            reinterpret_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        Wh_Log(L"GetTaskbarXamlRoot: TaskbarHost shared_ptr is empty.");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool offsetResolved = false;
#if defined(_M_X64) || defined(__x86_64__)
    const BYTE* b = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
        offsetResolved = true;
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    const DWORD* p = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        offsetResolved = true;
    }
#endif
    if (!offsetResolved) {
        Wh_Log(L"GetTaskbarXamlRoot: unsupported FrameHeight pattern.");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *reinterpret_cast<IUnknown**>(
            reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) +
            taskbarElementIUnknownOffset);

    wux::FrameworkElement taskbarElement{nullptr};
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(),
            winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

static bool ClassNameMatches(wf::IInspectable const& object, PCWSTR expected) {
    if (!object || !expected) {
        return false;
    }

    std::wstring className = winrt::get_class_name(object).c_str();
    return _wcsicmp(className.c_str(), expected) == 0;
}

static wux::FrameworkElement FindChildByName(wux::DependencyObject const& root,
                                             PCWSTR name) {
    if (!root || !name) {
        return nullptr;
    }

    std::vector<wux::DependencyObject> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        if (auto element = current.try_as<wux::FrameworkElement>()) {
            if (_wcsicmp(element.Name().c_str(), name) == 0) {
                return element;
            }
        }

        int childCount = wuxm::VisualTreeHelper::GetChildrenCount(current);
        for (int i = childCount - 1; i >= 0; --i) {
            auto child = wuxm::VisualTreeHelper::GetChild(current, i);
            if (child) {
                stack.push_back(child);
            }
        }
    }

    return nullptr;
}

static void DumpXamlTree(wux::DependencyObject const& root,
                         int depth = 0,
                         int maxDepth = 6) {
    if (!root || depth > maxDepth) {
        return;
    }

    std::wstring indent(depth * 2, L' ');
    auto element = root.try_as<wux::FrameworkElement>();
    Wh_Log(L"%s%s#%s", indent.c_str(),
           winrt::get_class_name(root).c_str(),
           element ? element.Name().c_str() : L"");

    int childCount = 0;
    try {
        childCount = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        Wh_Log(L"%s  <children unavailable: 0x%08X>", indent.c_str(),
               winrt::to_hresult());
        return;
    }

    for (int i = 0; i < childCount; ++i) {
        DumpXamlTree(wuxm::VisualTreeHelper::GetChild(root, i), depth + 1,
                     maxDepth);
    }
}

// The normal tray owns its layout-mode padding, template and styles. Observe
// the result instead of imposing horizontal-taskbar highlight heights.
static winrt::weak_ref<wux::FrameworkElement> g_nativeTrayMetricReference;

static void ApplyHoverBackgroundMetrics(wux::FrameworkElement const& button) {
    if (!button || g_unloading) return;
    try {
        auto reference = g_nativeTrayMetricReference.get();
        if (!reference || !reference.XamlRoot() ||
            reference.XamlRoot() != button.XamlRoot()) return;
        auto nativeBackground = FindChildByName(reference, L"BackgroundBorder")
                                    .try_as<wux::FrameworkElement>();
        auto background = FindChildByName(button, L"BackgroundBorder")
                              .try_as<wux::FrameworkElement>();
        if (!nativeBackground || !background) return;
        const double referenceWidth = reference.ActualWidth();
        const double referenceHeight = reference.ActualHeight();
        const double nativeWidth = nativeBackground.ActualWidth();
        const double nativeHeight = nativeBackground.ActualHeight();
        if (referenceWidth <= 0 || referenceHeight <= 0 ||
            nativeWidth <= 0 || nativeHeight <= 0 ||
            button.ActualWidth() <= 0 || button.ActualHeight() <= 0) return;
        auto nativeOrigin = nativeBackground.TransformToVisual(reference)
                                .TransformPoint({0, 0});
        double left = nativeOrigin.X;
        double top = nativeOrigin.Y;
        double right = referenceWidth - left - nativeWidth;
        double bottom = referenceHeight - top - nativeHeight;
        // A hidden/unarranged or animated reference is not a reliable metric.
        if (left < -0.5 || top < -0.5 || right < -0.5 || bottom < -0.5) return;

        if (button == g_batteryButton && g_settings.batteryMatchOtherButtonSize && g_batteryContent &&
            g_batteryContent.Orientation() == wuc::Orientation::Horizontal &&
            g_batteryPercentageText &&
            g_batteryPercentageText.Visibility() != wux::Visibility::Visible) {
            const double scale = button.XamlRoot().RasterizationScale();
            if (scale > 0) right -= 1.0 / scale;
        }

        if (button == g_batteryButton && g_batteryPercentageText &&
            g_batteryPercentageText.Visibility() == wux::Visibility::Visible) {
            const bool vertical = g_batteryContent &&
                g_batteryContent.Orientation() == wuc::Orientation::Vertical;
            // Battery is wider/taller than a normal tray item. Preserve the
            // native highlight's exact cross-axis size, rather than letting
            // a difference in outer battery size stretch or shrink it.
            if (vertical) {
                left += (button.ActualWidth() - referenceWidth) / 2;
                right = button.ActualWidth() - left - nativeWidth;
            } else {
                top += (button.ActualHeight() - referenceHeight) / 2;
                bottom = button.ActualHeight() - top - nativeHeight;
            }
            // Match physical pixel edges at fractional monitor scaling. Snap
            // the border rectangle, not four independent margin lengths.
            auto root = button.XamlRoot().Content();
            const auto origin = button.TransformToVisual(root).TransformPoint({0, 0});
            const double scale = button.XamlRoot().RasterizationScale();
            if (scale > 0) {
                auto snap = [scale](double value) { return std::round(value * scale) / scale; };
                right = button.ActualWidth() - (snap(origin.X + button.ActualWidth() - right) - origin.X);
                bottom = button.ActualHeight() - (snap(origin.Y + button.ActualHeight() - bottom) - origin.Y);
                left = snap(origin.X + left) - origin.X;
                top = snap(origin.Y + top) - origin.Y;
            }
        }

        auto parent = wuxm::VisualTreeHelper::GetParent(background)
                          .try_as<wux::FrameworkElement>();
        if (!parent || parent.ActualWidth() <= 0 || parent.ActualHeight() <= 0) return;
        const auto parentOrigin = parent.TransformToVisual(button).TransformPoint({0, 0});
        const wux::Thickness inset{
            left - parentOrigin.X,
            top - parentOrigin.Y,
            right - (button.ActualWidth() - parentOrigin.X - parent.ActualWidth()),
            bottom - (button.ActualHeight() - parentOrigin.Y - parent.ActualHeight())};

        // Stretch inside the existing template. Its border, visual states and
        // styling remain in charge of painting. Width can grow for battery %.
        auto clearIfLocal = [&](wux::DependencyProperty const& property) {
            if (background.ReadLocalValue(property) != wux::DependencyProperty::UnsetValue())
                background.ClearValue(property);
        };
        clearIfLocal(wux::FrameworkElement::WidthProperty());
        clearIfLocal(wux::FrameworkElement::MinWidthProperty());
        clearIfLocal(wux::FrameworkElement::MaxWidthProperty());
        clearIfLocal(wux::FrameworkElement::HeightProperty());
        clearIfLocal(wux::FrameworkElement::MinHeightProperty());
        clearIfLocal(wux::FrameworkElement::MaxHeightProperty());
        if (background.HorizontalAlignment() != wux::HorizontalAlignment::Stretch)
            background.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        if (background.VerticalAlignment() != wux::VerticalAlignment::Stretch)
            background.VerticalAlignment(wux::VerticalAlignment::Stretch);
        const auto previous = background.Margin();
        if (previous.Left != inset.Left || previous.Top != inset.Top ||
            previous.Right != inset.Right || previous.Bottom != inset.Bottom) {
            background.Margin(inset);
            Wh_Log(L"Native tray highlight insets %.1f,%.1f,%.1f,%.1f -> %s.",
                   left, top, right, bottom, button.Name().c_str());
        }
    } catch (...) {
        Wh_Log(L"Native tray highlight layout unavailable: 0x%08X", winrt::to_hresult());
    }
}

static wux::FrameworkElement FindAncestorFrameworkElement(
    wux::DependencyObject const& start) {
    auto current = start;
    while (current) {
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (auto element = current.try_as<wux::FrameworkElement>()) {
            return element;
        }
    }

    return nullptr;
}

static wux::FrameworkElement FindControlCenterButton(
    wux::DependencyObject const& root) {
    if (!root) {
        return nullptr;
    }

    std::vector<wux::DependencyObject> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        if (auto element = current.try_as<wux::FrameworkElement>()) {
            if (_wcsicmp(element.Name().c_str(), L"ControlCenterButton") == 0 &&
                ClassNameMatches(element, L"SystemTray.OmniButton")) {
                return element;
            }
        }

        int childCount = wuxm::VisualTreeHelper::GetChildrenCount(current);
        for (int i = childCount - 1; i >= 0; --i) {
            auto child = wuxm::VisualTreeHelper::GetChild(current, i);
            if (child) {
                stack.push_back(child);
            }
        }
    }

    return nullptr;
}


static bool IsInjectedElement(wux::FrameworkElement const& element) {
    if (!element) {
        return false;
    }

    const auto name = element.Name();
    return std::wstring_view(name).starts_with(L"SeparateQuickSettingsXaml");
}

static bool GetTaskbarGeometry(RECT* taskbarRect, RECT* hostRect,
                               bool* horizontal, bool* nearFirstEdge) {
    HWND taskbar = g_taskbarWnd ? g_taskbarWnd
                                : FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar || !GetWindowRect(taskbar, taskbarRect) ||
        !GetWindowRect(taskbar, hostRect)) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    const int width = taskbarRect->right - taskbarRect->left;
    const int height = taskbarRect->bottom - taskbarRect->top;
    *horizontal = width >= height;
    if (*horizontal) {
        *nearFirstEdge = abs(taskbarRect->top - monitorInfo.rcMonitor.top) <=
                         abs(monitorInfo.rcMonitor.bottom - taskbarRect->bottom);
    } else {
        *nearFirstEdge = abs(taskbarRect->left - monitorInfo.rcMonitor.left) <=
                         abs(monitorInfo.rcMonitor.right - taskbarRect->right);
    }
    return true;
}

static bool IsBatteryLayoutHorizontal() {
    RECT bar{}, host{};
    bool horizontal = true, first = true;
    GetTaskbarGeometry(&bar, &host, &horizontal, &first);
    // A vertical-taskbar mod may transform the XAML tray without immediately
    // changing the hosting HWND's rectangle. Use the arranged tray on screen.
    if (g_trayPanel && g_trayPanel.ActualWidth() > 0 && g_trayPanel.ActualHeight() > 0) {
        auto bounds = g_trayPanel.TransformToVisual(nullptr).TransformBounds(
            {0, 0, static_cast<float>(g_trayPanel.ActualWidth()),
             static_cast<float>(g_trayPanel.ActualHeight())});
        if (bounds.Height > bounds.Width * 1.5f) return false;
        if (bounds.Width > bounds.Height * 1.5f) return true;
    }
    return horizontal;
}

static double GetBatteryLeafOffsetX() {
    const double base = g_settings.smallBatteryGlyph ? -4 : -5;
    const bool shiftRight = !g_settings.smallBatteryGlyph &&
        g_settings.energySaverLeafGlyph && g_settings.batteryMatchOtherButtonSize &&
        IsBatteryLayoutHorizontal();
    return base + (shiftRight ? 2 : 0);
}

// Resolve Windows' own placement helper without detouring it. The optional
// fallback uses the same local exclusion rectangle and themed spacing.
using NativeTooltipPlacement_t = void(__cdecl*)(
    wuc::ToolTip const&, int, wf::Size, wf::Size, bool, bool);
static NativeTooltipPlacement_t g_nativeTooltipPlacement = nullptr;
static bool g_nativeTooltipResolutionAttempted = false;

static void ResolveNativeTooltipPlacement() {
    if (g_nativeTooltipResolutionAttempted) return;
    auto module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) return;
    g_nativeTooltipResolutionAttempted = true;
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {{
            LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))"
        }, &g_nativeTooltipPlacement, nullptr, true},
    };
    WindhawkUtils::HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"Native taskbar tooltip placement helper: %p.", g_nativeTooltipPlacement);
}

static void ApplyNativeTrayToolTipPlacement(
    wuc::ToolTip const& tip, wux::FrameworkElement const& targetButton) {
    if (!tip || !targetButton) return;
    RECT taskbarRect{}, hostRect{};
    bool horizontal = true, nearFirstEdge = true;
    if (!GetTaskbarGeometry(&taskbarRect, &hostRect, &horizontal, &nearFirstEdge)) return;

    // Native TaskbarLocation order: Left, Top, Right, Bottom.
    const int location = horizontal ? (nearFirstEdge ? 1 : 3)
                                    : (nearFirstEdge ? 0 : 2);
    const auto placement = horizontal
        ? (nearFirstEdge ? wucp::PlacementMode::Bottom : wucp::PlacementMode::Top)
        : (nearFirstEdge ? wucp::PlacementMode::Right : wucp::PlacementMode::Left);
    const wf::Size actual{
        static_cast<float>(targetButton.ActualWidth()),
        static_cast<float>(targetButton.ActualHeight())};
    if (actual.Width <= 0 || actual.Height <= 0) return;
    wf::Size desired{
        static_cast<float>(targetButton.Width()),
        static_cast<float>(targetButton.Height())};

    wuc::ToolTipService::SetPlacementTarget(targetButton, targetButton);
    wuc::ToolTipService::SetPlacement(targetButton, placement);
    tip.PlacementTarget(targetButton);
    tip.Placement(placement);
    tip.HorizontalOffset(0);
    tip.VerticalOffset(0);

    // Match the native helper's exclusion rectangle. Horizontal taskbars use
    // a square based on the owning row's height; vertical taskbars use the
    // desired width (or actual width when auto-sized) and actual height.
    tip.PlacementRect(winrt::box_value(wf::Rect{
        0, 0,
        horizontal ? actual.Height
                   : (std::isfinite(desired.Width) ? desired.Width : actual.Width),
        actual.Height}).as<wf::IReference<wf::Rect>>());

    auto excludeVerticalTray = [&] {
        if (horizontal || !g_trayPanel ||
            g_trayPanel.XamlRoot() != targetButton.XamlRoot()) return;
        const double width = g_trayPanel.ActualWidth();
        if (!std::isfinite(width) || width <= 0) return;
        // A wide vertical tray can contain several icon columns. Its entire
        // width, rather than just this icon's column, must remain unobscured.
        // Express the tray bounds in the tooltip owner's local coordinates;
        // keep the owner's Y so the tooltip still tracks the hovered row.
        const auto origin = g_trayPanel.TransformToVisual(targetButton)
                                .TransformPoint({0, 0});
        tip.PlacementRect(winrt::box_value(wf::Rect{
            origin.X, 0, static_cast<float>(width), actual.Height})
                .as<wf::IReference<wf::Rect>>());
    };

    ResolveNativeTooltipPlacement();
    if (g_nativeTooltipPlacement) {
        try {
            // The optional flags select special button layouts. These tray
            // buttons use the ordinary, non-expanded layout.
            g_nativeTooltipPlacement(tip, location, desired, actual, false, false);
            excludeVerticalTray();
            return;
        } catch (...) {
            Wh_Log(L"Native tooltip placement failed: 0x%08X; using themed fallback.",
                   winrt::to_hresult());
        }
    }
    try {
        auto resource = wux::Application::Current().Resources()
            .TryLookup(winrt::box_value(L"ToolTipMargin"));
        if (resource) {
            const double margin = winrt::unbox_value<double>(resource);
            if (std::isfinite(margin)) {
                // Windows' orientation helper supplies a positive resource
                // offset in the active axis for both opposing edges.
                if (horizontal) tip.VerticalOffset(margin);
                else tip.HorizontalOffset(margin);
            }
        }
    } catch (...) {
        // Keep the button-local placement and XAML's own spacing when this
        // Windows build has no compatible taskbar margin resource.
    }
    excludeVerticalTray();
}

static void SetTrayToolTip(wux::FrameworkElement const& targetButton,
                           std::wstring const& tooltip) {
    if (!targetButton || tooltip.empty()) {
        return;
    }

    try {
        // Keep the stock tooltip. Shell rejects a standalone XAML Popup in
        // explorer.exe (0x8000FFFF), so this is the supported host surface.
        wuc::ToolTip tip;
        tip.Content(winrt::box_value(tooltip));
        ApplyNativeTrayToolTipPlacement(tip, targetButton);
        {
        auto eventSource = tip;
        auto eventToken = eventSource.Opened([weakTarget = winrt::make_weak(targetButton)](wf::IInspectable const& sender,
                                 wux::RoutedEventArgs const&) {
        if (g_unloading) return;
            if (auto openedTip = sender.try_as<wuc::ToolTip>()) {
                if (auto target = weakTarget.get()) {
                    ApplyNativeTrayToolTipPlacement(openedTip, target);
                    if (target == g_bluetoothButton) RequestBluetoothTooltipRefresh();
                }
            }
        });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Opened(eventToken);
        });
    }
        wuc::ToolTipService::SetToolTip(targetButton, tip);
        wuxa::AutomationProperties::SetName(targetButton, tooltip);
    } catch (...) {
        Wh_Log(L"SetTrayToolTip failed for %s#%s: 0x%08X",
               winrt::get_class_name(targetButton).c_str(),
               targetButton.Name().c_str(), winrt::to_hresult());
    }
}

static void SetCachedTrayToolTip(wux::FrameworkElement const& targetButton,
                                 std::wstring& cache,
                                 std::wstring const& tooltip) {
    if (!targetButton || tooltip.empty()) {
        return;
    }

    try {
        if (cache == tooltip) {
            return;
        }
        cache = tooltip;
        auto existing = wuc::ToolTipService::GetToolTip(targetButton)
                            .try_as<wuc::ToolTip>();
        if (existing) {
            // Replacing the ToolTip object closes it. Updating its live
            // content preserves the open tooltip while the sound wheel moves
            // the volume, exactly like the native tray sound control.
            existing.Content(winrt::box_value(tooltip));
        } else {
            SetTrayToolTip(targetButton, tooltip);
        }
        wuxa::AutomationProperties::SetName(targetButton, tooltip);
        Wh_Log(L"Updated tray tooltip for %s#%s: [%s]",
               winrt::get_class_name(targetButton).c_str(),
               targetButton.Name().c_str(), tooltip.c_str());
    } catch (...) {
        Wh_Log(L"Updating fixed tooltip failed for %s#%s: 0x%08X",
               winrt::get_class_name(targetButton).c_str(),
               targetButton.Name().c_str(), winrt::to_hresult());
    }
}

static void RemoveInjectedControls(wuc::Panel const& parent) {
    if (!parent) {
        return;
    }

    RevokeEvents(g_uiEventRevokers);
    g_nativeNetworkViewModel = nullptr;
    g_nativeSoundViewModel = nullptr;
    g_nativeBatteryViewModel = nullptr;
    // FontFamily is an immutable resource reference, not a visual. Retain it
    // across tray rebuilds: the hidden native template may not be realized
    // again soon enough to rediscover its packaged font before first paint.
    g_nextNativeBatteryFontSearch = 0;
    g_usingNativeBatteryGlyphs = false;
    g_nextNativeBatterySearch = 0;
    g_trayMetadataProvider = nullptr;
    g_usingNativeNetworkGlyphs = false;
    g_nextNativeNetworkSearch = 0;
    g_usingNativeSoundGlyphs = false;
    g_nextNativeSoundSearch = 0;
    try { if (g_activeTrayContextFlyout) g_activeTrayContextFlyout.Hide(); } catch (...) {}
    g_activeTrayContextFlyout = nullptr;
    auto children = parent.Children();
    for (uint32_t i = children.Size(); i > 0; --i) {
        uint32_t index = i - 1;
        auto element = children.GetAt(index).try_as<wux::FrameworkElement>();
        if (IsInjectedElement(element)) {
            children.RemoveAt(index);
        }
    }
    RestoreGridTrayMutation();

    g_bluetoothButton = nullptr;
    g_networkButton = nullptr;
    g_soundButton = nullptr;
    g_batteryButton = nullptr;
    g_batteryIcon = {};
    g_batteryLeafIcon = nullptr;
    g_batteryPercentageText = nullptr;
    g_batteryContent = nullptr;
    g_batteryTooltipCache.clear();
    g_compactGroupedButton = nullptr;
    g_bluetoothIcon = {};
    g_networkIcon = {};
    g_soundIcon = {};
    g_compactGroupedIcon = {};
    g_bluetoothTooltipCache.clear();
    g_networkTooltipCache.clear();
    g_soundTooltipCache.clear();
}

struct GridTrayMutation {
    winrt::weak_ref<wuc::Grid> grid;
    std::vector<wuc::ColumnDefinition> columns;
    std::vector<std::pair<winrt::weak_ref<wux::FrameworkElement>, int>> shiftedChildren;
};
[[clang::no_destroy]] static std::optional<GridTrayMutation> g_gridTrayMutation;

static void RestoreGridTrayMutation() {
    if (!g_gridTrayMutation) return;
    try {
        if (auto grid = g_gridTrayMutation->grid.get()) {
            for (auto const& [childRef, column] : g_gridTrayMutation->shiftedChildren) {
                if (auto child = childRef.get()) wuc::Grid::SetColumn(child, column);
            }
            auto columns = grid.ColumnDefinitions();
            for (auto const& inserted : g_gridTrayMutation->columns) {
                for (uint32_t i = 0; i < columns.Size(); ++i) {
                    if (columns.GetAt(i) == inserted) { columns.RemoveAt(i); break; }
                }
            }
        }
    } catch (...) {}
    // Release strong references even if the tree is gone or restoration fails.
    g_gridTrayMutation.reset();
}

static void RestoreOriginalGroupedButton() {
    if (g_originalGroupedButton) {
        try { g_originalGroupedButton.Visibility(g_originalGroupedVisibility); } catch (...) {}
        try { g_originalGroupedButton.Width(g_originalGroupedWidth); } catch (...) {}
        try { g_originalGroupedButton.MinWidth(g_originalGroupedMinWidth); } catch (...) {}
        try { g_originalGroupedButton.MaxWidth(g_originalGroupedMaxWidth); } catch (...) {}
    }
}

static void CaptureOriginalGroupedButton(wux::FrameworkElement const& button) {
    if (!button) {
        return;
    }

    if (!g_originalGroupedButton || g_originalGroupedButton != button) {
        g_originalGroupedButton = button;
        g_originalGroupedVisibility = button.Visibility();
        g_originalGroupedWidth = button.Width();
        g_originalGroupedMinWidth = button.MinWidth();
        g_originalGroupedMaxWidth = button.MaxWidth();

        if (auto control = button.try_as<wuc::Control>()) {
            g_nativeGroupedButtonStyle = control.Style();
            Wh_Log(L"Captured grouped button class=%s style=%p.",
                   winrt::get_class_name(button).c_str(),
                   winrt::get_abi(g_nativeGroupedButtonStyle));
        } else {
            Wh_Log(L"Grouped button is not projected as Windows.UI.Xaml.Controls.Control: %s.",
                   winrt::get_class_name(button).c_str());
        }
    }

}

static void HideOriginalGroupedButton(wux::FrameworkElement const& button) {
    if (!button) return;
    CaptureOriginalGroupedButton(button);
    if (button.Visibility() != wux::Visibility::Collapsed) button.Visibility(wux::Visibility::Collapsed);
    if (button.Width() != 0) button.Width(0);
    if (button.MinWidth() != 0) button.MinWidth(0);
    if (button.MaxWidth() != 0) button.MaxWidth(0);
}

static bool IsUsableTrayButtonDimension(double value) {
    // The normal 48-pixel taskbar reports 32x48 XAML units on this build;
    // the compact taskbar reports smaller values.  Both are valid tray
    // hit-target sizes, so do not reject the default height as a fallback.
    return !std::isnan(value) && value >= 18 && value <= 64;
}

static bool TryCaptureTrayButtonMetricsFromElement(
    wux::FrameworkElement const& reference) {
    if (!reference || IsInjectedElement(reference)) {
        return false;
    }

    double width = reference.ActualWidth();
    double height = reference.ActualHeight();
    bool usedDeclaredSize = false;
    if (!IsUsableTrayButtonDimension(width) ||
        !IsUsableTrayButtonDimension(height)) {
        // During taskbar/XAML rebuilds the icon can exist in the visual tree
        // before its first measure pass.  Its declared Width/Height is still
        // the correct live taskbar metric and becomes available earlier.
        width = reference.Width();
        height = reference.Height();
        usedDeclaredSize = true;
    }
    if (!IsUsableTrayButtonDimension(width) ||
        !IsUsableTrayButtonDimension(height)) {
        return false;
    }

    g_trayButtonWidth = width;
    g_trayButtonHeight = height;
    g_nativeTrayMetricReference = winrt::make_weak(reference);
    Wh_Log(L"Captured single tray button metrics from %s#%s: %.1fx%.1f%s.",
           winrt::get_class_name(reference).c_str(), reference.Name().c_str(),
           g_trayButtonWidth, g_trayButtonHeight,
           usedDeclaredSize ? L" (declared)" : L"");
    return true;
}

// The direct children of SystemTrayFrameGrid are layout stacks.  Their
// dimensions describe the stack, not the individual tray hit target.  Walk
// the visual subtree and prefer the real NotifyIconView used by Windows for
// ordinary tray icons.  This keeps our OmniButtons on the same live size when
// the taskbar switches between its normal and small metrics.
static bool TryCaptureNativeNotifyIconMetrics(
    wux::DependencyObject const& root) {
    if (!root) {
        return false;
    }

    std::vector<wux::DependencyObject> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        auto element = current.try_as<wux::FrameworkElement>();
        if (element && !IsInjectedElement(element)) {
            const auto className = winrt::get_class_name(current);
            const auto name = element.Name();
            const bool isNotifyIcon =
                _wcsicmp(className.c_str(), L"SystemTray.NotifyIconView") == 0 ||
                _wcsicmp(name.c_str(), L"NotifyItemIcon") == 0;
            if (isNotifyIcon && g_notifyMetricDiagnosticCount < 8) {
                Wh_Log(L"NotifyIcon candidate %s#%s actual=%.1fx%.1f declared=%.1fx%.1f.",
                       className.c_str(), name.c_str(), element.ActualWidth(),
                       element.ActualHeight(), element.Width(), element.Height());
                ++g_notifyMetricDiagnosticCount;
            }
            if (isNotifyIcon && TryCaptureTrayButtonMetricsFromElement(element)) {
                if (auto control = element.try_as<wuc::Control>()) {
                    try {
                        auto style = control.Style();
                        if (style) {
                            g_nativeNotifyIconStyle = style;
                            Wh_Log(L"Captured live NotifyIconView style from %s#%s.",
                                   className.c_str(), name.c_str());
                        }
                    } catch (...) {
                        Wh_Log(L"Reading NotifyIconView style failed: 0x%08X",
                               winrt::to_hresult());
                    }
                }
                Wh_Log(L"Using individual native tray icon metrics from %s#%s.",
                       className.c_str(), name.c_str());
                return true;
            }
        }

        const int count = wuxm::VisualTreeHelper::GetChildrenCount(current);
        for (int i = count - 1; i >= 0; --i) {
            auto child = wuxm::VisualTreeHelper::GetChild(current, i);
            if (child) {
                stack.push_back(child);
            }
        }
    }

    return false;
}

static void ApplyTrayButtonMetrics(wux::FrameworkElement const& element);

static void CaptureTrayButtonMetricsFromPanel(
    wuc::Panel const& parentPanel,
    wux::FrameworkElement const& controlCenterButton) {
    if (!parentPanel) {
        return;
    }

    auto children = parentPanel.Children();
    uint32_t controlCenterIndex = children.Size();
    if (controlCenterButton) {
        children.IndexOf(controlCenterButton.as<wux::UIElement>(),
                         controlCenterIndex);
    }

    // Prefer a real individual tray icon anywhere below the frame.  The
    // ordinary notification-area stacks are present in both taskbar sizes,
    // and their NotifyIconView dimensions are the most faithful hit-target
    // reference available to us.
    if (TryCaptureNativeNotifyIconMetrics(parentPanel)) {
        return;
    }

    // Adjacent containers (clock, chevron, empty app-icon stacks) are not
    // individual tray buttons. Retain the last valid metrics if none exist.

    Wh_Log(L"No nearby single tray button metrics found; using fallback %.1fx%.1f.",
           g_trayButtonWidth, g_trayButtonHeight);
}



static void ApplyTrayButtonMetrics(wux::FrameworkElement const& element) {
    if (!element) {
        return;
    }

    try {
        element.ClearValue(wux::FrameworkElement::WidthProperty());
        element.ClearValue(wux::FrameworkElement::HeightProperty());
        element.ClearValue(wux::FrameworkElement::MaxWidthProperty());
        element.ClearValue(wux::FrameworkElement::MinWidthProperty());
        element.ClearValue(wux::FrameworkElement::MaxHeightProperty());
        element.ClearValue(wux::FrameworkElement::MinHeightProperty());
    } catch (...) {
        Wh_Log(L"ApplyTrayButtonMetrics: ClearValue failed for %s#%s: 0x%08X",
               winrt::get_class_name(element).c_str(), element.Name().c_str(),
               winrt::to_hresult());
    }

    element.Width(g_trayButtonWidth);
    element.Height(g_trayButtonHeight);
    element.MinWidth(0);
    element.MaxWidth(g_trayButtonWidth);
    if (element.Name() == L"SeparateQuickSettingsXamlBattery") {
        const bool horizontal = IsBatteryLayoutHorizontal();
        const bool percentage = g_batteryPercentageEnabled.load() == 1;
        // Icon-only layout must not depend on repeated glyph measurements.
        const double iconWidth = horizontal && !g_settings.batteryMatchOtherButtonSize
            ? (g_settings.smallBatteryGlyph ? 31.0 : 36.0) : g_trayButtonWidth;
        element.Width(horizontal && percentage ? NAN : iconWidth);
        element.MinWidth(0);
        element.MaxWidth(percentage ? INFINITY : iconWidth);
        if (!horizontal && percentage) {
            element.Height(NAN);
            element.MinHeight(g_trayButtonHeight);
            element.MaxHeight(INFINITY);
            element.HorizontalAlignment(wux::HorizontalAlignment::Center);
            element.VerticalAlignment(wux::VerticalAlignment::Center);
            return;
        }
    }
    element.MinHeight(0);
    element.MaxHeight(g_trayButtonHeight);
    element.HorizontalAlignment(wux::HorizontalAlignment::Center);
    element.VerticalAlignment(wux::VerticalAlignment::Center);

    if (auto uiElement = element.try_as<wux::UIElement>()) {
        uiElement.InvalidateMeasure();
        uiElement.InvalidateArrange();
    }
}

static bool ApplyXamlButtons();
static void RefreshInjectedButtonMetrics();

static void ScheduleMetricRefresh() {
    const ULONGLONG now = GetTickCount64();
    if (g_unloading || g_metricRefreshPending ||
        (g_lastMetricRebuildTick && now - g_lastMetricRebuildTick < 1000)) {
        return;
    }

    g_metricRefreshPending = true;
    if (!g_metricRefreshTimer) {
        g_metricRefreshTimer = wux::DispatcherTimer();
        g_metricRefreshTimer.Interval(std::chrono::milliseconds(250));
        {
        auto eventSource = g_metricRefreshTimer;
        auto eventToken = eventSource.Tick([](wf::IInspectable const&,
                                     wf::IInspectable const&) {
        if (g_unloading) return;
            if (g_metricRefreshTimer) {
                g_metricRefreshTimer.Stop();
            }
            g_metricRefreshPending = false;
            g_lastMetricRebuildTick = GetTickCount64();
            // Explorer commits the compact template asynchronously. Rebuild
            // once after it settles, then ignore the layout events produced by
            // that rebuild for a short cooldown.
            Wh_Log(L"Rebuilding the tray once after a size transition.");
            ApplyXamlButtons();
        });
        g_timerEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Tick(eventToken);
        });
    }
    }
    g_metricRefreshTimer.Start();
}

static void RefreshInjectedButtonMetrics() {
    if (!g_trayPanel || !g_trayControlCenterButton) {
        return;
    }

    const double oldWidth = g_trayButtonWidth;
    const double oldHeight = g_trayButtonHeight;
    CaptureTrayButtonMetricsFromPanel(g_trayPanel,
                                      g_trayControlCenterButton);
    if (oldWidth == g_trayButtonWidth && oldHeight == g_trayButtonHeight) {
        return;
    }

    Wh_Log(L"Tray metrics changed %.1fx%.1f -> %.1fx%.1f; updating injected "
           L"buttons.", oldWidth, oldHeight, g_trayButtonWidth,
           g_trayButtonHeight);
    ApplyTrayButtonMetrics(g_bluetoothButton);
    ApplyTrayButtonMetrics(g_networkButton);
    ApplyTrayButtonMetrics(g_soundButton);
    ApplyTrayButtonMetrics(g_compactGroupedButton);
    ApplyTrayButtonMetrics(g_batteryButton);
    ApplyHoverBackgroundMetrics(g_bluetoothButton);
    ApplyHoverBackgroundMetrics(g_networkButton);
    ApplyHoverBackgroundMetrics(g_soundButton);
    ApplyHoverBackgroundMetrics(g_compactGroupedButton);
    ApplyHoverBackgroundMetrics(g_batteryButton);
    if (g_trayPanel) {
        g_trayPanel.InvalidateMeasure();
        g_trayPanel.InvalidateArrange();
        g_trayPanel.UpdateLayout();
    }
}

static void AttachTaskbarSizeRefreshHandlers(
    wux::FrameworkElement const& trayElement,
    wux::FrameworkElement const& controlCenterButton) {
    auto lastHorizontal = std::make_shared<bool>(IsBatteryLayoutHorizontal());
    auto handler = [lastHorizontal](wf::IInspectable const& sender,
                      wux::SizeChangedEventArgs const& args) {
        const auto oldSize = args.PreviousSize();
        const auto newSize = args.NewSize();
        if (oldSize.Width == newSize.Width && oldSize.Height == newSize.Height) {
            return;
        }

        // Queue cached-state rendering after this arrange pass, including
        // when hit-target metrics are unchanged. Relocation can otherwise
        // leave the battery horizontal until the next power-status update.
        PostTrayRefresh(false);

        const bool horizontal = IsBatteryLayoutHorizontal();
        const bool sameOrientation = horizontal == *lastHorizontal;
        *lastHorizontal = horizontal;
        if (sameOrientation) {
            if (auto reference = g_nativeTrayMetricReference.get(); reference &&
                reference.ActualWidth() == g_trayButtonWidth &&
                reference.ActualHeight() == g_trayButtonHeight) {
                // Battery percentage changes the tray's extent, not the
                // native hit-target metrics. Rebuilding every button here
                // discards their templates and interrupts hover states.
                return;
            }
        }

        auto element = sender.try_as<wux::FrameworkElement>();
        Wh_Log(L"Taskbar tray size changed on %s#%s %.1fx%.1f -> %.1fx%.1f; scheduling metric refresh.",
               element ? winrt::get_class_name(element).c_str() : L"",
               element ? element.Name().c_str() : L"", oldSize.Width,
               oldSize.Height, newSize.Width, newSize.Height);
        ScheduleMetricRefresh();
    };

    try {
        if (g_sizeRefreshTrayElement) {
            g_sizeRefreshTrayElement.SizeChanged(g_sizeRefreshTrayToken);
            g_sizeRefreshTrayElement = nullptr;
            g_sizeRefreshTrayToken = {};
        }
        if (g_sizeRefreshControlCenterButton) {
            g_sizeRefreshControlCenterButton.SizeChanged(
                g_sizeRefreshControlCenterToken);
            g_sizeRefreshControlCenterButton = nullptr;
            g_sizeRefreshControlCenterToken = {};
        }
        if (trayElement) {
            g_sizeRefreshTrayToken = trayElement.SizeChanged(handler);
            g_sizeRefreshTrayElement = trayElement;
        }
        if (controlCenterButton) {
            g_sizeRefreshControlCenterToken =
                controlCenterButton.SizeChanged(handler);
            g_sizeRefreshControlCenterButton = controlCenterButton;
        }
    } catch (...) {
        Wh_Log(L"AttachTaskbarSizeRefreshHandlers failed: 0x%08X",
               winrt::to_hresult());
    }
}

static void UpdateDynamicXamlIcons();

static bool RefreshTaskbarLayoutIfRebuilt() {
    HWND taskbarWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        return false;
    }

    auto xamlRoot = GetTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        return false;
    }

    auto root = xamlRoot.Content().try_as<wux::FrameworkElement>();
    if (!root) {
        return false;
    }

    auto currentButton = FindControlCenterButton(root);
    auto currentParent = currentButton
                             ? FindAncestorFrameworkElement(currentButton)
                             : nullptr;
    auto currentPanel = currentParent.try_as<wuc::Panel>();
    if (!currentPanel || !currentButton) {
        return false;
    }

    if (currentPanel == g_trayPanel &&
        currentButton == g_trayControlCenterButton) {
        return false;
    }

    Wh_Log(L"Detected taskbar tray XAML rebuild; reinjecting separated buttons.");
    g_taskbarWnd = taskbarWnd;
    return ApplyXamlButtons();
}

static void EnsureUpdateTimer() {
    EnsureTrayRefreshWindow();
    if (g_updateTimer) {
        return;
    }

    g_updateTimer = wux::DispatcherTimer();
    g_updateTimer.Interval(std::chrono::seconds(60));
    {
        auto eventSource = g_updateTimer;
        auto eventToken = eventSource.Tick([lastFallback = ULONGLONG{0}](wf::IInspectable const&,
                          wf::IInspectable const&) mutable {
        if (g_unloading) return;
        const auto now = GetTickCount64();
        if (!lastFallback || now - lastFallback >= 600000) {
            lastFallback = now;
            UpdateDynamicXamlIcons();
            // SizeChanged and StartTaskbar handle normal rebuilds. Keep this
            // expensive tree walk only as a ten-minute recovery fallback.
            if (!RefreshTaskbarLayoutIfRebuilt()) {
                RefreshInjectedButtonMetrics();
            }
            if (g_originalGroupedButton)
                HideOriginalGroupedButton(g_originalGroupedButton);
        } else if (g_networkIcon.primary &&
                   g_displayNetworkState.kind == NetworkKind::WifiConnecting) {
            // Advancing belongs solely to this 500 ms timer. Rendering a
            // status snapshot must not consume an animation frame.
            if (!TryApplyNativeNetworkGlyphs()) {
                g_usingNativeNetworkGlyphs = false;
                ++g_wifiConnectingFrame;
                g_networkIcon.primary.Glyph(GetNetworkGlyph(g_displayNetworkState));
            }
        }
    });
        g_timerEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Tick(eventToken);
        });
    }
    g_updateTimer.Start();
}

static void UpdateTimerCadence() {
    if (!g_updateTimer) return;
    const bool needFastTimer =
        g_displayNetworkState.kind == NetworkKind::WifiConnecting;
    if (needFastTimer == g_updateTimerFast) return;
    g_updateTimerFast = needFastTimer;
    g_updateTimer.Stop();
    g_updateTimer.Interval(needFastTimer ? std::chrono::milliseconds(500)
                                         : std::chrono::seconds(60));
    g_updateTimer.Start();
}

static winrt::hstring GetNetworkGlyph(NetworkState const& state) {
    switch (state.kind) {
        case NetworkKind::Ethernet:
            return L"\xE839";
        case NetworkKind::Wifi:
            if (state.signal <= 25) {
                return L"\xE872";
            }
            if (state.signal <= 50) {
                return L"\xE873";
            }
            if (state.signal <= 75) {
                return L"\xE874";
            }
            return L"\xE701";
        case NetworkKind::WifiConnecting: {
            static PCWSTR frames[] = {L"\xE873", L"\xEAA5", L"\xEAA8"};
            return frames[g_wifiConnectingFrame % ARRAYSIZE(frames)];
        }
        case NetworkKind::WifiDisconnected:
            return L"\xF384";
        case NetworkKind::WifiDisabled:
            return L"\xF384";
        case NetworkKind::Disconnected:
        default:
            return L"\xF384";
    }
}

static winrt::hstring GetSoundOutputDeviceGlyph(SoundState const& state) {
    if (ContainsCI(state.outputName, L"buds") ||
        ContainsCI(state.outputName, L"earbuds")) {
        return L"\xF4C0";
    }

    switch (state.outputFormFactor) {
        case Headphones:
        case Headset:
        case Handset:
            return L"\xE7F6";
        case DigitalAudioDisplayDevice:
            return L"\xE7F3";
        case Speakers:
        case LineLevel:
        case UnknownDigitalPassthrough:
        case SPDIF:
            return L"\xE7F5";
        default:
            return L"\xE767";
    }
}

static winrt::hstring GetSoundVolumeGlyph(SoundState const& state) {
    if (state.volume <= 0.001f) {
        return L"\xE992";
    }
    if (state.volume < 0.34f) {
        return L"\xE993";
    }
    if (state.volume < 0.67f) {
        return L"\xE994";
    }
    return L"\xE767";
}

static winrt::hstring GetSoundGlyph(SoundState const& state) {
    if (!state.available || state.muted) {
        return L"\xE74F";
    }

    if (g_settings.soundIconFollowsOutputDevice) {
        return GetSoundOutputDeviceGlyph(state);
    }

    return GetSoundVolumeGlyph(state);
}

static std::wstring GetNetworkTooltip(NetworkState const& state) {
    auto accessLine = [&state]() {
        return state.internetAccess ? L"Internet access" : L"No internet access";
    };

    switch (state.kind) {
        case NetworkKind::Ethernet:
            return std::wstring(L"Ethernet\n") + accessLine();
        case NetworkKind::Wifi: {
            std::wstring name = state.name.empty() ? L"Wi-Fi" : state.name;
            if (!g_settings.showSignalStrengthInTooltip) return name + L"\n" + accessLine();
            return name + L"\n" + accessLine() + L"\n\nSignal strength: " +
                   std::to_wstring(state.signal) + L"%";
        }
        case NetworkKind::WifiConnecting: {
            if (!state.name.empty()) {
                return std::wstring(L"Connecting to ") + state.name + L"\n" +
                       accessLine();
            }
            return std::wstring(L"Connecting to Wi-Fi\n") + accessLine();
        }
        case NetworkKind::WifiDisconnected:
            return L"No internet access\nNo connections available";
        case NetworkKind::WifiDisabled:
            return L"No internet access\nNo connections available";
        case NetworkKind::Disconnected:
        default:
            return L"No internet access";
    }
}

namespace bt = winrt::Windows::Devices::Bluetooth;
namespace de = winrt::Windows::Devices::Enumeration;
[[clang::no_destroy]] static wf::IAsyncOperation<de::DeviceInformationCollection> g_btQueries[3]{nullptr, nullptr, nullptr};
static std::wstring g_btConnectedNames;
static size_t g_btConnectedCount = 0;
static ULONGLONG g_btQueryTick = 0;
static bool g_btQueryInvalidated = true;
static bool g_btQueryRequested = false;

// Windows' peripheral battery property, also exposed on headset audio devnodes.
static constexpr auto kBluetoothBatteryProperty = L"{104EA319-6EE2-4701-BD47-8DDBF425BBE5} 2";

static int GetBluetoothBatteryLevel(wf::IInspectable const& value) {
    auto property = value.try_as<wf::IPropertyValue>();
    if (!property) return -1;
    uint32_t level;
    switch (property.Type()) {
        case wf::PropertyType::UInt8: level = property.GetUInt8(); break;
        case wf::PropertyType::UInt16: level = property.GetUInt16(); break;
        case wf::PropertyType::UInt32: level = property.GetUInt32(); break;
        default: return -1;
    }
    return level <= 100 ? static_cast<int>(level) : -1;
}

static std::wstring GetBluetoothContainerKey(wf::IInspectable const& value) {
    auto id = winrt::unbox_value_or<winrt::guid>(value, winrt::guid{});
    return id == winrt::guid{} ? L"" : std::wstring(winrt::to_hstring(id));
}

static std::wstring GetBluetoothTooltip(bool available) {
    if (!available || !g_settings.showConnectedDevicesInTooltip) {
        for (auto& query : g_btQueries) {
            if (query) query.Cancel();
            query = nullptr;
        }
        g_btConnectedNames.clear();
        g_btConnectedCount = 0;
        g_btQueryTick = 0;
        g_btQueryRequested = false;
        return available ? L"Bluetooth" : L"Bluetooth is off";
    }
    try {
        if (g_btQueries[0] && g_btQueries[1] && g_btQueries[2] &&
            g_btQueries[0].Status() != wf::AsyncStatus::Started &&
            g_btQueries[1].Status() != wf::AsyncStatus::Started &&
            g_btQueries[2].Status() != wf::AsyncStatus::Started) {
            // A failed enumeration is not evidence that devices disconnected.
            for (size_t index = 0; index < 2; ++index) {
                if (g_btQueries[index].Status() != wf::AsyncStatus::Completed) {
                    winrt::throw_hresult(g_btQueries[index].ErrorCode());
                }
            }
            std::unordered_map<std::wstring, int> batteries;
            if (g_btQueries[2].Status() == wf::AsyncStatus::Completed) {
                for (auto const& device : g_btQueries[2].GetResults()) {
                    auto props = device.Properties();
                    auto key = GetBluetoothContainerKey(props.TryLookup(L"System.Devices.ContainerId"));
                    int level = GetBluetoothBatteryLevel(props.TryLookup(kBluetoothBatteryProperty));
                    if (!key.empty() && level >= 0) {
                        auto [entry, inserted] = batteries.emplace(key, level);
                        // Multiple reporting components: show the lowest reported charge.
                        if (!inserted) entry->second = (std::min)(entry->second, level);
                    }
                }
            }
            g_btQueries[2] = nullptr;
            struct ConnectedDevice { std::wstring name; int battery = -1; };
            std::unordered_map<std::wstring, ConnectedDevice> devices;
            for (size_t index = 0; index < 2; ++index) {
                auto& query = g_btQueries[index];
                if (query.Status() == wf::AsyncStatus::Completed) {
                    for (auto const& device : query.GetResults()) {
                        auto props = device.Properties();
                        auto connected = props.TryLookup(L"System.Devices.Aep.IsConnected");
                        if (!connected || !winrt::unbox_value_or<bool>(connected, false)) continue;
                        auto address = props.TryLookup(L"System.Devices.Aep.DeviceAddress");
                        std::wstring key = address ? winrt::unbox_value_or<winrt::hstring>(address, L"").c_str() : L"";
                        if (key.empty()) key = device.Id().c_str();
                        auto& entry = devices[ToLower(key)];
                        entry.name = device.Name().c_str();
                        int level = GetBluetoothBatteryLevel(props.TryLookup(kBluetoothBatteryProperty));
                        auto container = GetBluetoothContainerKey(props.TryLookup(L"System.Devices.Aep.ContainerId"));
                        auto battery = batteries.find(container);
                        if (level < 0 && battery != batteries.end()) level = battery->second;
                        if (level >= 0) entry.battery = level;
                    }
                } else {
                    Wh_Log(L"Bluetooth connected-endpoint query failed: 0x%08X", query.ErrorCode().value);
                }
                query = nullptr;
            }
            std::wstring connectedNames;
            for (auto const& entry : devices) {
                std::wstring name = entry.second.name;
                for (auto& character : name) {
                    if (character == L'\r' || character == L'\n' ||
                        character == L'\u2028' || character == L'\u2029') {
                        character = L' ';
                    }
                }
                name = Trim(name);
                connectedNames += L"\n- " +
                    (name.empty() ? std::wstring(L"Bluetooth device") : name);
                if (entry.second.battery >= 0) {
                    connectedNames += L" (" + std::to_wstring(entry.second.battery) + L"%)";
                }
            }
            // Publish only a complete snapshot, including a genuinely empty one.
            g_btConnectedNames = std::move(connectedNames);
            g_btConnectedCount = devices.size();
        }
        // Enumeration is requested only when the user opens the tooltip.
        if (!g_btQueries[0] && g_btQueryRequested) {
            g_btQueryRequested = false;
            g_btQueryInvalidated = false;
            auto properties = winrt::single_threaded_vector<winrt::hstring>({
                L"System.Devices.Aep.IsConnected", L"System.Devices.Aep.DeviceAddress",
                L"System.Devices.Aep.ContainerId", kBluetoothBatteryProperty});
            g_btQueries[0] = de::DeviceInformation::FindAllAsync(
                bt::BluetoothDevice::GetDeviceSelectorFromConnectionStatus(bt::BluetoothConnectionStatus::Connected),
                properties, de::DeviceInformationKind::AssociationEndpoint);
            g_btQueries[1] = de::DeviceInformation::FindAllAsync(
                bt::BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(bt::BluetoothConnectionStatus::Connected),
                properties, de::DeviceInformationKind::AssociationEndpoint);
            g_btQueries[2] = de::DeviceInformation::FindAllAsync(
                L"System.Devices.Present:=System.StructuredQueryType.Boolean#True",
                winrt::single_threaded_vector<winrt::hstring>({
                    L"System.Devices.ContainerId", kBluetoothBatteryProperty}),
                de::DeviceInformationKind::Device);
            g_btQueryTick = GetTickCount64();
        }
    } catch (...) {
        for (auto& query : g_btQueries) query = nullptr;
        g_btQueryTick = GetTickCount64();
        Wh_Log(L"Bluetooth tooltip query failed: 0x%08X", winrt::to_hresult());
    }
    if (g_btConnectedNames.empty()) {
        return L"Bluetooth\n\nNo devices connected";
    }
    return g_btConnectedCount == 1
               ? L"Bluetooth\n\nConnected device:" + g_btConnectedNames
               : L"Bluetooth\n\nConnected devices (" +
                     std::to_wstring(g_btConnectedCount) + L"):" +
                     g_btConnectedNames;
}

static std::wstring GetFriendlyMediaAppName(std::wstring appId) {
    appId = Trim(appId);
    if (appId.empty()) {
        return L"Media app";
    }

    std::wstring lower = ToLower(appId);
    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Google Chrome";
    if (lower.find(L"msedge") != std::wstring::npos ||
        lower.find(L"microsoftedge") != std::wstring::npos) {
        return L"Microsoft Edge";
    }
    if (lower.find(L"firefox") != std::wstring::npos) return L"Firefox";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (lower.find(L"zunemusic") != std::wstring::npos ||
        lower.find(L"mediaplayer") != std::wstring::npos) {
        return L"Media Player";
    }

    size_t bang = appId.find(L'!');
    if (bang != std::wstring::npos) {
        appId.resize(bang);
    }

    size_t slash = appId.find_last_of(L"\\/");
    if (slash != std::wstring::npos && slash + 1 < appId.size()) {
        appId = appId.substr(slash + 1);
    }

    if (EndsWithCI(appId, L".exe")) {
        appId.resize(appId.size() - 4);
    }

    size_t underscore = appId.find(L'_');
    if (underscore != std::wstring::npos) {
        appId.resize(underscore);
    }

    size_t dot = appId.find_last_of(L'.');
    if (dot != std::wstring::npos && dot + 1 < appId.size()) {
        appId = appId.substr(dot + 1);
    }

    appId = Trim(appId);
    if (!appId.empty()) {
        appId[0] = static_cast<wchar_t>(towupper(appId[0]));
    }
    return appId.empty() ? L"Media app" : appId;
}

static void StoreMediaTooltipInfo(MediaTooltipInfo const& info) {
    AcquireSRWLockExclusive(&g_mediaTooltipLock);
    g_mediaTooltipInfo = info;
    ReleaseSRWLockExclusive(&g_mediaTooltipLock);
}

static MediaTooltipInfo LoadMediaTooltipInfo() {
    AcquireSRWLockShared(&g_mediaTooltipLock);
    MediaTooltipInfo info = g_mediaTooltipInfo;
    ReleaseSRWLockShared(&g_mediaTooltipLock);
    return info;
}

static MediaTooltipInfo GetCurrentlyPlayingMediaInfo() {
    if (!g_settings.showCurrentlyPlayingInSoundTooltip) {
        return {};
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = g_lastMediaTooltipQueryTick.load();
    if ((!previous || now - previous >= 1000) &&
        !g_mediaTooltipQueryInProgress.exchange(true)) {
        g_lastMediaTooltipQueryTick.store(now);
        HANDLE thread = StartOwnedWorker([]() {
            MediaTooltipInfo info{};
            bool initialized = false;
            try {
                winrt::init_apartment(winrt::apartment_type::multi_threaded);
                initialized = true;
                auto manager =
                    wmc::GlobalSystemMediaTransportControlsSessionManager::
                        RequestAsync()
                            .get();
                auto session = manager ? manager.GetCurrentSession() : nullptr;
                if (session) {
                    auto playbackInfo = session.GetPlaybackInfo();
                    const auto status =
                        playbackInfo
                            ? playbackInfo.PlaybackStatus()
                            : wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                  Closed;
                    if (status ==
                            wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                Playing ||
                        status ==
                            wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                Paused) {
                        auto properties =
                            session.TryGetMediaPropertiesAsync().get();
                        info.title = Trim(properties.Title().c_str());
                        info.artist = Trim(properties.Artist().c_str());
                        if (info.artist.empty()) {
                            info.artist = Trim(properties.AlbumArtist().c_str());
                        }
                        if (!info.title.empty() || !info.artist.empty()) {
                            info.hasMedia = true;
                            info.paused =
                                status ==
                                wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                                    Paused;
                            info.appName = GetFriendlyMediaAppName(
                                session.SourceAppUserModelId().c_str());
                        }
                    }
                }
            } catch (...) {
                Wh_Log(L"Currently playing tooltip query failed: 0x%08X",
                       winrt::to_hresult());
            }
            if (initialized) winrt::uninit_apartment();
            StoreMediaTooltipInfo(info);
            g_mediaTooltipQueryInProgress.store(false);
        });
        if (thread) CloseHandle(thread);
        else g_mediaTooltipQueryInProgress.store(false);
    }

    return LoadMediaTooltipInfo();
}

static void RefreshMediaTooltipInfoFromStatusWorker() {
    if (!g_settings.showCurrentlyPlayingInSoundTooltip) {
        StoreMediaTooltipInfo({});
        return;
    }
    // The status worker already owns the COM/RPC refresh path. Trigger the
    // existing cached query here so XAML tooltip rendering never creates work.
    GetCurrentlyPlayingMediaInfo();
}

static std::wstring GetMediaTooltipTrackText(MediaTooltipInfo const& media) {
    if (!media.artist.empty() && !media.title.empty()) {
        return media.artist + L" - " + media.title;
    }

    if (!media.title.empty()) {
        return media.title;
    }

    if (!media.artist.empty()) {
        return media.artist;
    }

    return L"Unknown";
}

static std::wstring GetSoundTooltip(SoundState const& state) {
    if (!state.available) {
        return L"No audio output device";
    }

    int percent = static_cast<int>(state.volume * 100.0f + 0.5f);
    if (percent < 0) {
        percent = 0;
    } else if (percent > 100) {
        percent = 100;
    }

    std::wstring name =
        state.outputName.empty() ? L"Volume" : state.outputName;
    std::wstring tooltip = state.muted ? name + L": Muted"
                                       : name + L": " + std::to_wstring(percent) + L"%";

    MediaTooltipInfo media = LoadMediaTooltipInfo();
    if (media.hasMedia) {
        tooltip += L"\n\nCurrently playing:\n";
        tooltip += GetMediaTooltipTrackText(media);
        if (!media.appName.empty()) {
            tooltip += L"\n\nSource: ";
            tooltip += media.appName;
        }
    }

    return tooltip;
}

static void RefreshEnergySaverStateAsync();
template<typename T, typename G> static void SetTrayGlyph(T const& icon, G const& value) {
    const winrt::hstring glyph{value};
    if (icon && icon.Glyph() != glyph) icon.Glyph(glyph);
}
template<typename T> static void SetTrayVisibility(T const& element, wux::Visibility value) {
    if (element && element.Visibility() != value) element.Visibility(value);
}
template<typename T> static void SetTrayOpacity(T const& element, double value) {
    if (element && element.Opacity() != value) element.Opacity(value);
}
template<typename T> static void SetTrayForeground(T const& element, wuxm::Brush const& brush) {
    if (!element) return;
    auto current = element.Foreground();
    if (current == brush) return;
    auto a = current.template try_as<wuxm::SolidColorBrush>();
    auto b = brush.try_as<wuxm::SolidColorBrush>();
    if (a && b && a.Color() == b.Color() && a.Opacity() == b.Opacity()) return;
    element.Foreground(brush);
}
static std::atomic<int> g_energySaverState{-1};
static std::atomic<int> g_energySaverActive{-1};
static std::atomic<bool> g_modernEnergySaverStatusSeen{false};
static constexpr GUID kPowerSavingStatus{
    0xe00958c0, 0xc213, 0x4ace, {0xac, 0x77, 0xfe, 0xcc, 0xed, 0x2e, 0xee, 0xa5}};
static constexpr GUID kEnergySaverStatus{
    0x550e8400, 0xe29b, 0x41d4, {0xa7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00}};

static int GetEnergySaverDisplayState() {
    const int active = g_energySaverActive.load();
    return active >= 0 ? active : g_energySaverState.load();
}

static wuxm::Brush BatteryStatusBrush(PCWSTR resource, wu::Color fallback) {
    try {
        auto value = wux::Application::Current().Resources().TryLookup(winrt::box_value(resource));
        if (auto brush = value.try_as<wuxm::Brush>()) return brush;
    } catch (...) {}
    return wuxm::SolidColorBrush(fallback);
}

static void UpdateSeparateBatteryButton() {
    if (!g_batteryButton || !g_batteryIcon.primary) return;
    // Verified against the Windows Settings handler on this build. Read the
    // user's preference directly so external Settings changes aren't gated by
    // an unrelated Energy saver COM operation.
    DWORD preference = 0, preferenceSize = sizeof(preference);
    const auto preferenceError = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"IsBatteryPercentageEnabled", RRF_RT_REG_DWORD, nullptr,
        &preference, &preferenceSize);
    if (preferenceError == ERROR_SUCCESS || preferenceError == ERROR_FILE_NOT_FOUND)
        g_batteryPercentageEnabled.store(preference != 0 ? 1 : 0);
    SYSTEM_POWER_STATUS status{};
    const bool known = GetSystemPowerStatus(&status) != FALSE;
    const bool present = known && status.BatteryFlag != 255 && !(status.BatteryFlag & 128);
    SetTrayVisibility(g_batteryButton, present && g_settings.showBatteryButton
        ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (!present) return;
    static bool percentageDefaultChecked = false;
    if (!percentageDefaultChecked) {
        percentageDefaultChecked = true;
        // This is a Windows preference. Initialize it once per installation,
        // then honor subsequent changes from Settings or the battery menu.
        if (!Wh_GetIntValue(L"batteryPercentageDefaultApplied", 0))
            QueueEnergySaverWork(3);
    }
    const bool percentKnown = status.BatteryLifePercent <= 100;
    const int percent = percentKnown ? status.BatteryLifePercent : 0;
    const int level = (std::clamp)((percent + 9) / 10, 1, 10);
    const bool charging = (status.BatteryFlag & 8) != 0;
    // Active charging stops at full charge or a firmware charge limit.
    // Keep the connected-power artwork visible while AC is online, but
    // retain the actual charging flag for the tooltip's precise wording.
    const bool powerConnected = status.ACLineStatus == 1 || charging;
    // Also honor the modern Always use energy saver setting, including AC.
    // It is refreshed off-thread; never invoke Settings COM from rendering.
    const int activeSaver = g_energySaverActive.load();
    const bool saver = activeSaver >= 0 ? activeSaver == 1 :
        status.SystemStatusFlag == 1 || g_energySaverState.load() == 1;
    const bool showLeaf = saver && g_settings.energySaverLeafGlyph;
    auto foreground = MakeIconBrush();
    auto fill = foreground;
    wchar_t base = 0xF8D0;
    wchar_t overlay = static_cast<wchar_t>(0xF8D0 + level);
    // SysBatt Fluent Icons layer mapping from the trashpanda fork.
    if (saver && !showLeaf && !powerConnected) {
        base = 0xF8D0;
        overlay = static_cast<wchar_t>(0xF8D0 + level);
        fill = wuxm::SolidColorBrush(wu::Color{255, 234, 163, 0});
    } else if (powerConnected) {
        base = 0xF8DB;
        overlay = level <= 2 ? static_cast<wchar_t>(0xF8D0 + level)
                             : static_cast<wchar_t>(0xF8DC + level - 3);
        fill = BatteryStatusBrush(L"SystemFillColorSuccessBrush", {255, 15, 123, 15});
    } else if (percent <= 10) {
        fill = BatteryStatusBrush(L"SystemFillColorCriticalBrush", {255, 196, 43, 28});
    } else if (percent <= 20) {
        fill = BatteryStatusBrush(L"SystemFillColorCautionBrush", {255, 157, 93, 0});
    }
    ApplyNativeBatteryFont();
    if (!TryApplyNativeBatteryGlyphs(fill)) {
        g_usingNativeBatteryGlyphs = false;
        SetTrayGlyph(g_batteryIcon.primary, std::wstring(1, base));
        SetTrayForeground(g_batteryIcon.primary, foreground);
        SetTrayGlyph(g_batteryIcon.overlay, std::wstring(1, overlay));
        SetTrayForeground(g_batteryIcon.overlay, fill);
        SetTrayVisibility(g_batteryIcon.overlay, percentKnown && percent > 0
            ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        SetTrayVisibility(g_batteryIcon.underlay, wux::Visibility::Collapsed);
    }
    if (g_batteryLeafIcon) {
        // The leaf carries the Energy saver indication. Preserve normal
        // battery fill (including charging/low-battery colors) instead of
        // taking the native Energy saver yellow brush override.
        if (showLeaf) SetTrayForeground(g_batteryIcon.overlay, fill);
        const double size = g_batteryIcon.primary.FontSize() * 0.75;
        if (g_batteryLeafIcon.Width() != size) g_batteryLeafIcon.Width(size);
        if (g_batteryLeafIcon.Height() != size) g_batteryLeafIcon.Height(size);
        if (auto offset = g_batteryLeafIcon.RenderTransform().try_as<wuxm::TranslateTransform>()) {
            const double x = GetBatteryLeafOffsetX();
            if (offset.X() != x) offset.X(x);
        }
        // Reuse the tray foreground for both strokes: black in light mode,
        // light in dark mode. The green interior stays independently filled.
        for (auto name : {L"SeparateTrayLeafBody", L"SeparateTrayLeafStem"}) {
            if (auto shape = FindChildByName(g_batteryLeafIcon, name)
                    .try_as<wux::Shapes::Shape>())
                shape.Stroke(foreground);
        }
        SetTrayVisibility(g_batteryLeafIcon, showLeaf ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }
    const bool showPercent = percentKnown && g_batteryPercentageEnabled.load() == 1;
    // Visibility and text must participate in this measure pass, rather than
    // changing them after the button has already chosen its size.
    const std::wstring label = percentKnown ? std::to_wstring(percent) + L"%" : L"";
    if (g_batteryPercentageText.Text() != label) g_batteryPercentageText.Text(label);
    SetTrayVisibility(g_batteryPercentageText, showPercent ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    const bool horizontal = IsBatteryLayoutHorizontal();
    if (auto content = g_batteryContent) {
        auto orientation = horizontal ? wuc::Orientation::Horizontal : wuc::Orientation::Vertical;
        double spacing = horizontal ? 4 : 0;
        // Glyph-layer panels are not the glyph/percentage layout panel.
        // Do not copy their zero inter-layer spacing into the visible row.
        if (content.Orientation() != orientation) content.Orientation(orientation);
        if (content.Spacing() != spacing) content.Spacing(spacing);
        // The fixed icon-only button already gets its inset from the native
        // OmniButton template. Extra content margins consume the glyph's space
        // and clip it inside that fixed width.
        const double inset = showPercent && horizontal ? 8 : 0;
        const auto margin = content.Margin();
        if (margin.Left != inset || margin.Right != inset)
            content.Margin({inset, 0, inset, 0});
    }
    const double iconWidth = horizontal && !g_settings.batteryMatchOtherButtonSize
        ? (g_settings.smallBatteryGlyph ? 31.0 : 36.0) : g_trayButtonWidth;
    const double batteryWidth = showPercent && horizontal ? NAN : iconWidth;
    const double batteryMaxWidth = showPercent ? INFINITY : iconWidth;
    if (g_batteryButton.MaxWidth() != batteryMaxWidth)
        g_batteryButton.MaxWidth(batteryMaxWidth);
    const double currentWidth = g_batteryButton.Width();
    if (std::isnan(batteryWidth) ? !std::isnan(currentWidth) : currentWidth != batteryWidth)
        g_batteryButton.Width(batteryWidth);
    // Two vertical rows must measure naturally instead of being clipped to
    // the height captured from a single-glyph notification button.
    const bool stackedPercentage = showPercent && !horizontal;
    // Measure only our own stack. Do not change native template ancestors:
    // those participate in the tray's shared layout and visual states.
    double batteryHeight = g_trayButtonHeight;
    if (stackedPercentage && g_batteryContent) {
        g_batteryContent.Measure({static_cast<float>(batteryWidth), INFINITY});
        double padding = 0;
        if (auto control = g_batteryButton.try_as<wuc::Control>()) {
            const auto p = control.Padding();
            padding = p.Top + p.Bottom;
        }
        // Keep the normal row's surrounding space, then add the percentage
        // row. Merely max(normal row, glyph + text) often keeps the old height
        // unchanged, squeezing both rows into one notification hit target.
        const double extraRow = std::max(0.0,
            static_cast<double>(g_batteryContent.DesiredSize().Height) -
            static_cast<double>(g_batteryIcon.host.DesiredSize().Height));
        batteryHeight = std::max(g_trayButtonHeight,
            std::max(g_trayButtonHeight + extraRow,
                static_cast<double>(g_batteryContent.DesiredSize().Height) + padding));
    }
    if (g_batteryButton.MinHeight() != 0) g_batteryButton.MinHeight(0);
    if (g_batteryButton.MaxHeight() != INFINITY) g_batteryButton.MaxHeight(INFINITY);
    if (g_batteryButton.Height() != batteryHeight) {
        g_batteryButton.Height(batteryHeight);
        g_batteryButton.InvalidateMeasure();
        if (g_trayPanel) g_trayPanel.InvalidateMeasure();
        Wh_Log(L"Battery measured layout: horizontal=%d percentage=%d height=%.1f.",
               horizontal, showPercent, batteryHeight);
    }
    if (auto offset = g_batteryPercentageText.RenderTransform().try_as<wuxm::TranslateTransform>()) {
        const double y = horizontal ? -1 : 0;
        if (offset.Y() != y) offset.Y(y);
    }
    // Center the visible SysBatt outline, whose ink is offset in its font box.
    if (auto offset = g_batteryIcon.host.RenderTransform().try_as<wuxm::TranslateTransform>()) {
        // SysBatt's advance box is asymmetric even when copied from the
        // native text element. Keep its optical correction for icon-only
        // buttons; percentage rows retain their existing baseline alignment.
        // Keep horizontal optical centering unchanged. Vertical battery
        // glyphs use no horizontal translation, with or without percentage.
        const double x = horizontal ? (showPercent ? 0 : 2) : 0;
        if (offset.X() != x) offset.X(x);
    }
    SetTrayForeground(g_batteryPercentageText, foreground);
    std::wstring tooltip = percentKnown ? L"Battery: " + label : L"Battery";
    tooltip += charging ? L" (Charging)" :
        status.ACLineStatus == 1 ? (percentKnown && percent == 100
            ? L" (Fully charged, plugged in)" : L" (Plugged in, not charging)")
        : L" (On battery)";
    if (status.ACLineStatus == 0 && status.BatteryLifeTime != DWORD(-1)) {
        const auto minutes = status.BatteryLifeTime / 60;
        tooltip += L"\n" + std::to_wstring(minutes / 60) + L" h " +
                   std::to_wstring(minutes % 60) + L" m remaining";
    }
    // Match the menu's AlwaysOn toggle. The legacy power-status flag can remain
    // on (for example, automatic low-battery saving) after that toggle is off.
    if (saver && !charging)
        tooltip += L"\n\nEnergy saver is enabled";
    SetCachedTrayToolTip(g_batteryButton, g_batteryTooltipCache, tooltip);
    ApplyHoverBackgroundMetrics(g_batteryButton);
}

// Read the shell's own XAML metadata instead of calling private C++ methods
// or depending on build-specific object offsets. All reads run on the tray UI
// thread; the original controls are observed without moving their content.
static wf::IInspectable ReadTrayProperty(wf::IInspectable const& object, PCWSTR name) {
    if (!object) return nullptr;
    try {
        if (auto provider = object.try_as<wux::Data::ICustomPropertyProvider>()) {
            auto property = provider.GetCustomProperty(name);
            if (property && property.CanRead()) return property.GetValue(object);
        }
        if (!g_trayMetadataProvider) {
            auto module = GetModuleHandleW(L"SystemTray.dll");
            auto factoryFn = module ? reinterpret_cast<HRESULT(WINAPI*)(void*, void**)>(
                GetProcAddress(module, "DllGetActivationFactory")) : nullptr;
            if (!factoryFn) return nullptr;
            winrt::hstring typeName = L"SystemTray.XamlMetaDataProvider";
            wf::IActivationFactory factory{nullptr};
            winrt::check_hresult(factoryFn(winrt::get_abi(typeName), winrt::put_abi(factory)));
            g_trayMetadataProvider = factory.ActivateInstance<wf::IInspectable>()
                .as<wuxmk::IXamlMetadataProvider>();
        }
        auto type = g_trayMetadataProvider.GetXamlType(winrt::get_class_name(object));
        if (!type) return nullptr;
        auto member = type.GetMember(name);
        if (member) return member.GetValue(object);
    } catch (...) {}
    return nullptr;
}

static bool IsNativeNetworkModel(wf::IInspectable const& object, int depth = 0) {
    if (!object || depth > 3) return false;
    const auto name = winrt::get_class_name(object);
    if (name == L"SystemTray.NetworkSystemTrayIconDataModel" ||
        name == L"SystemTray.NetworkButton") return true;
    if (!std::wstring_view(name).starts_with(L"SystemTray.")) return false;
    for (auto property : {L"DataModel", L"ContentVM", L"ViewModel"}) {
        auto child = ReadTrayProperty(object, property);
        if (child && child != object && IsNativeNetworkModel(child, depth + 1)) return true;
    }
    return false;
}

static bool ResolveNativeNetworkGlyphs() {
    if (g_nativeNetworkViewModel) return true;
    const auto now = GetTickCount64();
    if (!g_trayControlCenterButton || now < g_nextNativeNetworkSearch) return false;
    g_nextNativeNetworkSearch = now + 5000;
    struct Candidate { wux::DependencyObject element; bool network; };
    std::vector<Candidate> stack{{g_trayControlCenterButton, false}};
    static int diagnosticCount = 0;
    while (!stack.empty()) {
        auto candidate = stack.back();
        stack.pop_back();
        if (auto element = candidate.element.try_as<wux::FrameworkElement>()) {
            if (IsInjectedElement(element)) continue;
            const auto klass = winrt::get_class_name(element);
            candidate.network = candidate.network || klass == L"SystemTray.NetworkButton";
            auto viewModel = ReadTrayProperty(element, L"ViewModel");
            auto context = element.DataContext();
            if (!viewModel) viewModel = context;
            if (viewModel && winrt::get_class_name(viewModel) != L"SystemTray.TextIconContentViewModel") {
                auto content = ReadTrayProperty(viewModel, L"ContentVM");
                if (content) viewModel = content;
            }
            if (viewModel && winrt::get_class_name(viewModel) == L"SystemTray.TextIconContentViewModel") {
                const bool network = candidate.network || IsNativeNetworkModel(viewModel) ||
                    IsNativeNetworkModel(context);
                if (diagnosticCount < 16) {
                    ++diagnosticCount;
                    Wh_Log(L"Native network candidate: %s#%s model=%s network=%d.",
                           klass.c_str(), element.Name().c_str(),
                           winrt::get_class_name(viewModel).c_str(), network);
                }
                if (network) {
                    auto base = ReadTrayProperty(viewModel, L"BaseText");
                    if (!base || winrt::unbox_value_or<winrt::hstring>(base, L"").empty()) continue;
                    if (auto changes = viewModel.try_as<wux::Data::INotifyPropertyChanged>()) {
                        auto token = changes.PropertyChanged([](auto const&, auto const&) {
                            // Post only a UI refresh. Do not enumerate adapters
                            // or touch XAML from the model's notification thread.
                            if (!g_unloading) PostTrayRefresh(false);
                        });
                        g_uiEventRevokers.push_back([weak = winrt::make_weak(changes), token] {
                            if (auto source = weak.get()) source.PropertyChanged(token);
                        });
                    }
                    g_nativeNetworkViewModel = viewModel;
                    Wh_Log(L"Native network glyph source selected: %s#%s.",
                           klass.c_str(), element.Name().c_str());
                    return true;
                }
            }
        }
        for (int i = wuxm::VisualTreeHelper::GetChildrenCount(candidate.element) - 1; i >= 0; --i)
            stack.push_back({wuxm::VisualTreeHelper::GetChild(candidate.element, i), candidate.network});
    }
    if (diagnosticCount < 16) {
        ++diagnosticCount;
        Wh_Log(L"Native network model not available; using manual network glyphs.");
    }
    return false;
}

static bool TryApplyNativeNetworkGlyphs() {
    try {
        if (!g_networkIcon.primary || !ResolveNativeNetworkGlyphs()) return false;
        auto baseValue = ReadTrayProperty(g_nativeNetworkViewModel, L"BaseText");
        if (!baseValue) return false;
        const auto base = winrt::unbox_value_or<winrt::hstring>(baseValue, L"");
        if (base.empty()) return false;
        // Read all layers before updating so a failed read keeps the manual
        // fallback coherent. Empty underlays/overlays are valid native states.
        auto underlayValue = ReadTrayProperty(g_nativeNetworkViewModel, L"UnderlayText");
        auto overlayValue = ReadTrayProperty(g_nativeNetworkViewModel, L"OverlayText");
        if (!underlayValue || !overlayValue) return false;
        const auto underlay = winrt::unbox_value_or<winrt::hstring>(underlayValue, L"");
        const auto overlay = winrt::unbox_value_or<winrt::hstring>(overlayValue, L"");
        SetTrayGlyph(g_networkIcon.primary, base);
        SetTrayForeground(g_networkIcon.primary, MakeIconBrush());
        SetTrayGlyph(g_networkIcon.underlay, underlay);
        SetTrayForeground(g_networkIcon.underlay, MakeUnderlayBrush());
        SetTrayVisibility(g_networkIcon.underlay, underlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        SetTrayGlyph(g_networkIcon.overlay, overlay);
        SetTrayForeground(g_networkIcon.overlay, MakeIconBrush());
        SetTrayVisibility(g_networkIcon.overlay, overlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        if (!g_usingNativeNetworkGlyphs) Wh_Log(L"Network glyphs now follow native base/underlay/overlay text.");
        g_usingNativeNetworkGlyphs = true;
        return true;
    } catch (...) {
        Wh_Log(L"Native network glyph read failed: 0x%08X; using manual fallback.", winrt::to_hresult());
    }
    return false;
}

static bool IsNativeSoundModel(wf::IInspectable const& object, int depth = 0) {
    if (!object || depth > 3) return false;
    const auto name = winrt::get_class_name(object);
    if (name == L"SystemTray.VolumeSystemTrayIconDataModel" ||
        name == L"SystemTray.VolumeButton") return true;
    if (!std::wstring_view(name).starts_with(L"SystemTray.")) return false;
    for (auto property : {L"DataModel", L"ContentVM", L"ViewModel"}) {
        auto child = ReadTrayProperty(object, property);
        if (child && child != object && IsNativeSoundModel(child, depth + 1)) return true;
    }
    return false;
}

static bool ResolveNativeSoundGlyphs() {
    if (g_nativeSoundViewModel) return true;
    const auto now = GetTickCount64();
    if (!g_trayControlCenterButton || now < g_nextNativeSoundSearch) return false;
    g_nextNativeSoundSearch = now + 5000;
    struct Candidate { wux::DependencyObject element; bool sound; };
    std::vector<Candidate> stack{{g_trayControlCenterButton, false}};
    static int diagnosticCount = 0;
    while (!stack.empty()) {
        auto candidate = stack.back();
        stack.pop_back();
        if (auto element = candidate.element.try_as<wux::FrameworkElement>()) {
            if (IsInjectedElement(element)) continue;
            const auto klass = winrt::get_class_name(element);
            candidate.sound = candidate.sound || klass == L"SystemTray.VolumeButton";
            auto viewModel = ReadTrayProperty(element, L"ViewModel");
            auto context = element.DataContext();
            if (!viewModel) viewModel = context;
            if (viewModel && winrt::get_class_name(viewModel) != L"SystemTray.TextIconContentViewModel") {
                auto content = ReadTrayProperty(viewModel, L"ContentVM");
                if (content) viewModel = content;
            }
            if (viewModel && winrt::get_class_name(viewModel) == L"SystemTray.TextIconContentViewModel") {
                const bool sound = candidate.sound || IsNativeSoundModel(viewModel) ||
                    IsNativeSoundModel(context);
                if (diagnosticCount < 16) {
                    ++diagnosticCount;
                    Wh_Log(L"Native sound candidate: %s#%s model=%s sound=%d.",
                           klass.c_str(), element.Name().c_str(),
                           winrt::get_class_name(viewModel).c_str(), sound);
                }
                if (sound) {
                    auto base = ReadTrayProperty(viewModel, L"BaseText");
                    if (!base || winrt::unbox_value_or<winrt::hstring>(base, L"").empty()) continue;
                    if (auto changes = viewModel.try_as<wux::Data::INotifyPropertyChanged>()) {
                        auto token = changes.PropertyChanged([](auto const&, auto const&) {
                            // Post only a UI refresh. Do not enumerate adapters
                            // or touch XAML from the model's notification thread.
                            if (!g_unloading) PostTrayRefresh(false);
                        });
                        g_uiEventRevokers.push_back([weak = winrt::make_weak(changes), token] {
                            if (auto source = weak.get()) source.PropertyChanged(token);
                        });
                    }
                    g_nativeSoundViewModel = viewModel;
                    Wh_Log(L"Native sound glyph source selected: %s#%s.",
                           klass.c_str(), element.Name().c_str());
                    return true;
                }
            }
        }
        for (int i = wuxm::VisualTreeHelper::GetChildrenCount(candidate.element) - 1; i >= 0; --i)
            stack.push_back({wuxm::VisualTreeHelper::GetChild(candidate.element, i), candidate.sound});
    }
    if (diagnosticCount < 16) {
        ++diagnosticCount;
        Wh_Log(L"Native sound model not available; using manual sound glyphs.");
    }
    return false;
}

static bool TryApplyNativeSoundGlyphs() {
    try {
        if (!g_soundIcon.primary || !ResolveNativeSoundGlyphs()) return false;
        auto baseValue = ReadTrayProperty(g_nativeSoundViewModel, L"BaseText");
        if (!baseValue) return false;
        const auto base = winrt::unbox_value_or<winrt::hstring>(baseValue, L"");
        if (base.empty()) return false;
        // Read all layers before updating so a failed read keeps the manual
        // fallback coherent. Empty underlays/overlays are valid native states.
        auto underlayValue = ReadTrayProperty(g_nativeSoundViewModel, L"UnderlayText");
        auto overlayValue = ReadTrayProperty(g_nativeSoundViewModel, L"OverlayText");
        if (!underlayValue || !overlayValue) return false;
        const auto underlay = winrt::unbox_value_or<winrt::hstring>(underlayValue, L"");
        const auto overlay = winrt::unbox_value_or<winrt::hstring>(overlayValue, L"");
        SetTrayGlyph(g_soundIcon.primary, base);
        SetTrayForeground(g_soundIcon.primary, MakeIconBrush());
        SetTrayGlyph(g_soundIcon.underlay, underlay);
        SetTrayForeground(g_soundIcon.underlay, MakeUnderlayBrush());
        SetTrayVisibility(g_soundIcon.underlay, underlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        SetTrayGlyph(g_soundIcon.overlay, overlay);
        SetTrayForeground(g_soundIcon.overlay, MakeIconBrush());
        SetTrayVisibility(g_soundIcon.overlay, overlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        if (!g_usingNativeSoundGlyphs) Wh_Log(L"Sound glyphs now follow native base/underlay/overlay text.");
        g_usingNativeSoundGlyphs = true;
        return true;
    } catch (...) {
        Wh_Log(L"Native sound glyph read failed: 0x%08X; using manual fallback.", winrt::to_hresult());
    }
    return false;
}

static bool IsNativeBatteryModel(wf::IInspectable const& object, int depth = 0) {
    if (!object || depth > 3) return false;
    const auto name = winrt::get_class_name(object);
    if (name == L"SystemTray.PowerSystemTrayIconDataModel" ||
        name == L"SystemTray.BatteryIconContent") return true;
    if (!std::wstring_view(name).starts_with(L"SystemTray.")) return false;
    for (auto property : {L"DataModel", L"ContentVM", L"ViewModel"}) {
        auto child = ReadTrayProperty(object, property);
        if (child && child != object && IsNativeBatteryModel(child, depth + 1)) return true;
    }
    return false;
}

static void ApplyNativeBatteryFont() {
    // Preserve the native packaged font reference instead of looking up only
    // the globally installed family name. Never move the native visual.
    if (g_trayControlCenterButton &&
        GetTickCount64() >= g_nextNativeBatteryFontSearch) {
        g_nextNativeBatteryFontSearch = GetTickCount64() + 5000;
        struct Candidate { wux::DependencyObject element; bool battery; };
        std::vector<Candidate> stack{{g_trayControlCenterButton, false}};
        while (!stack.empty()) {
            auto candidate = stack.back();
            stack.pop_back();
            auto element = candidate.element.try_as<wux::FrameworkElement>();
            if (element && IsInjectedElement(element)) continue;
            if (element && winrt::get_class_name(element) == L"SystemTray.BatteryIconContent")
                candidate.battery = true;
            if (candidate.battery) {
                wuxm::FontFamily font{nullptr};
                if (auto text = candidate.element.try_as<wuc::TextBlock>()) font = text.FontFamily();
                else if (auto icon = candidate.element.try_as<wuc::FontIcon>()) font = icon.FontFamily();
                if (font) {
                    std::wstring source(font.Source());
                    std::transform(source.begin(), source.end(), source.begin(),
                        [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
                    if (source.find(L"sysbatt") != std::wstring::npos) {
                        const bool changed = !g_nativeBatteryFont ||
                            g_nativeBatteryFont.Source() != font.Source();
                        g_nativeBatteryFont = font;
                        if (element && element.ActualWidth() > 0 && element.ActualHeight() > 0) {
                            g_nativeBatteryGlyphWidth = element.ActualWidth();
                            g_nativeBatteryGlyphHeight = element.ActualHeight();
                            if (auto text = element.try_as<wuc::TextBlock>())
                                g_nativeBatteryGlyphFontSize = text.FontSize();
                            else if (auto icon = element.try_as<wuc::FontIcon>())
                                g_nativeBatteryGlyphFontSize = icon.FontSize();
                        }
                        if (changed) Wh_Log(L"Native battery font resource selected: %s", font.Source().c_str());
                    }
                }
            }
            for (int i = wuxm::VisualTreeHelper::GetChildrenCount(candidate.element) - 1; i >= 0; --i)
                stack.push_back({wuxm::VisualTreeHelper::GetChild(candidate.element, i), candidate.battery});
        }
    }
    if (g_nativeBatteryFont) {
        for (auto const& glyph : {g_batteryIcon.primary, g_batteryIcon.underlay, g_batteryIcon.overlay})
            if (glyph) {
                glyph.FontFamily(g_nativeBatteryFont);
                const double size = g_settings.smallBatteryGlyph ? 12 : 16;
                if (glyph.FontSize() != size) glyph.FontSize(size);
            }
        // FontIcon's natural advance can differ from the native TextBlock's
        // layout box. Reserve that native box so glyph ink cannot overlap %.
        if (g_batteryIcon.host && g_nativeBatteryGlyphFontSize > 0 &&
            std::isfinite(g_nativeBatteryGlyphWidth) && std::isfinite(g_nativeBatteryGlyphHeight)) {
            const double ratio = (g_settings.smallBatteryGlyph ? 12.0 : 16.0) /
                g_nativeBatteryGlyphFontSize;
            const double width = g_nativeBatteryGlyphWidth * ratio;
            const double height = g_nativeBatteryGlyphHeight * ratio;
            if (g_batteryIcon.host.Width() != width) g_batteryIcon.host.Width(width);
            if (g_batteryIcon.host.Height() != height) g_batteryIcon.host.Height(height);
        }
    }
}

static bool ResolveNativeBatteryGlyphs() {
    if (g_nativeBatteryViewModel) return true;
    const auto now = GetTickCount64();
    if (!g_trayControlCenterButton || now < g_nextNativeBatterySearch) return false;
    g_nextNativeBatterySearch = now + 5000;
    struct Candidate { wux::DependencyObject element; bool battery; };
    std::vector<Candidate> stack{{g_trayControlCenterButton, false}};
    static int diagnosticCount = 0;
    while (!stack.empty()) {
        auto candidate = stack.back();
        stack.pop_back();
        if (auto element = candidate.element.try_as<wux::FrameworkElement>()) {
            if (IsInjectedElement(element)) continue;
            const auto klass = winrt::get_class_name(element);
            candidate.battery = candidate.battery || klass == L"SystemTray.BatteryIconContent";
            auto viewModel = ReadTrayProperty(element, L"ViewModel");
            auto context = element.DataContext();
            if (!viewModel) viewModel = context;
            if (viewModel && winrt::get_class_name(viewModel) != L"SystemTray.TextIconContentViewModel") {
                auto content = ReadTrayProperty(viewModel, L"ContentVM");
                if (content) viewModel = content;
            }
            if (viewModel && winrt::get_class_name(viewModel) == L"SystemTray.TextIconContentViewModel") {
                const bool battery = candidate.battery || IsNativeBatteryModel(viewModel) ||
                    IsNativeBatteryModel(context);
                if (diagnosticCount < 16) {
                    ++diagnosticCount;
                    Wh_Log(L"Native battery candidate: %s#%s model=%s battery=%d.",
                           klass.c_str(), element.Name().c_str(),
                           winrt::get_class_name(viewModel).c_str(), battery);
                }
                if (battery) {
                    auto base = ReadTrayProperty(viewModel, L"BaseText");
                    if (!base || winrt::unbox_value_or<winrt::hstring>(base, L"").empty()) continue;
                    if (auto changes = viewModel.try_as<wux::Data::INotifyPropertyChanged>()) {
                        auto token = changes.PropertyChanged([](auto const&, auto const&) {
                            // Post only a UI refresh. Do not query power status
                            // or touch XAML from the model's notification thread.
                            if (!g_unloading) PostTrayRefresh(false);
                        });
                        g_uiEventRevokers.push_back([weak = winrt::make_weak(changes), token] {
                            if (auto source = weak.get()) source.PropertyChanged(token);
                        });
                    }
                    g_nativeBatteryViewModel = viewModel;
                    Wh_Log(L"Native battery glyph source selected: %s#%s.",
                           klass.c_str(), element.Name().c_str());
                    return true;
                }
            }
        }
        for (int i = wuxm::VisualTreeHelper::GetChildrenCount(candidate.element) - 1; i >= 0; --i)
            stack.push_back({wuxm::VisualTreeHelper::GetChild(candidate.element, i), candidate.battery});
    }
    if (diagnosticCount < 16) {
        ++diagnosticCount;
        Wh_Log(L"Native battery model not available; using manual battery glyphs.");
    }
    return false;
}

static bool TryApplyNativeBatteryGlyphs(wuxm::Brush const& fill) {
    try {
        if (!g_batteryIcon.primary || !ResolveNativeBatteryGlyphs()) return false;
        auto baseValue = ReadTrayProperty(g_nativeBatteryViewModel, L"BaseText");
        if (!baseValue) return false;
        const auto base = winrt::unbox_value_or<winrt::hstring>(baseValue, L"");
        if (base.empty()) return false;
        // Read all layers before updating so a failed read keeps the manual
        // fallback coherent. Empty underlays/overlays are valid native states.
        auto underlayValue = ReadTrayProperty(g_nativeBatteryViewModel, L"UnderlayText");
        auto overlayValue = ReadTrayProperty(g_nativeBatteryViewModel, L"OverlayText");
        if (!underlayValue || !overlayValue) return false;
        const auto underlay = winrt::unbox_value_or<winrt::hstring>(underlayValue, L"");
        const auto overlay = winrt::unbox_value_or<winrt::hstring>(overlayValue, L"");
        SetTrayGlyph(g_batteryIcon.primary, base);
        SetTrayForeground(g_batteryIcon.primary, MakeIconBrush());
        SetTrayGlyph(g_batteryIcon.underlay, underlay);
        SetTrayForeground(g_batteryIcon.underlay, MakeUnderlayBrush());
        SetTrayVisibility(g_batteryIcon.underlay, underlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        SetTrayGlyph(g_batteryIcon.overlay, overlay);
        // Keep charging/low-battery colors when Windows does not expose an override.
        auto overlayBrush = ReadTrayProperty(g_nativeBatteryViewModel,
            L"IconOverlayForegroundOverride").try_as<wuxm::Brush>();
        SetTrayForeground(g_batteryIcon.overlay, overlayBrush ? overlayBrush : fill);
        SetTrayVisibility(g_batteryIcon.overlay, overlay.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        if (!g_usingNativeBatteryGlyphs) Wh_Log(L"Battery glyphs now follow native base/underlay/overlay text.");
        g_usingNativeBatteryGlyphs = true;
        return true;
    } catch (...) {
        Wh_Log(L"Native battery glyph read failed: 0x%08X; using manual fallback.", winrt::to_hresult());
    }
    return false;
}

static void UpdateDynamicXamlIcons() {
    static bool updating = false;
    // COM calls can pump a queued refresh while the taskbar ASTA is busy.
    // Never re-enter XAML mutation partway through an earlier update.
    if (updating) return;
    struct ResetUpdating { bool& value; ~ResetUpdating() { value = false; } } reset{updating};
    updating = true;
    try {
        RefreshEnergySaverStateAsync();
        UpdateSeparateBatteryButton();
        RefreshBatteryMenuItems();
        auto primaryBrush = MakeIconBrush();
        auto underlayBrush = MakeUnderlayBrush();
        if (g_compactGroupedIcon.primary)
            SetTrayForeground(g_compactGroupedIcon.primary, primaryBrush);
        if (g_compactGroupedIcon.underlay)
            SetTrayForeground(g_compactGroupedIcon.underlay, underlayBrush);
        if (g_compactGroupedIcon.overlay)
            SetTrayForeground(g_compactGroupedIcon.overlay, primaryBrush);
        const StatusSnapshot snapshot = GetStatusSnapshot();

        if (g_bluetoothIcon.primary) {
            const bool bluetoothAvailable = snapshot.bluetoothAvailable;
            if (g_bluetoothButton) {
                SetTrayVisibility(g_bluetoothButton, wux::Visibility::Visible);
                SetTrayOpacity(g_bluetoothButton, 1.0);
            }
            SetTrayVisibility(g_bluetoothIcon.primary, wux::Visibility::Visible);
            SetTrayOpacity(g_bluetoothIcon.primary,
                (bluetoothAvailable || !g_settings.changeBluetoothGlyphWhenDisabled) ? 1.0 : 0.2);
            // Keep the Bluetooth glyph visible when disabled; the overlay is
            // responsible for marking the unavailable state.
            SetTrayGlyph(g_bluetoothIcon.primary, L"\xE702");
            SetTrayForeground(g_bluetoothIcon.primary, (bluetoothAvailable || !g_settings.changeBluetoothGlyphWhenDisabled)
                                                   ? primaryBrush
                                                   : underlayBrush);
            SetCachedTrayToolTip(
                g_bluetoothButton, g_bluetoothTooltipCache,
                GetBluetoothTooltip(bluetoothAvailable));
            if (g_bluetoothIcon.underlay) {
                SetTrayVisibility(g_bluetoothIcon.underlay, wux::Visibility::Collapsed);
            }
            if (g_bluetoothIcon.overlay) {
                SetTrayGlyph(g_bluetoothIcon.overlay, L"\xE871");
                g_bluetoothIcon.overlay.FontFamily(
                    wuxm::FontFamily(L"Segoe Fluent Icons"));
                g_bluetoothIcon.overlay.FontSize(16);
                g_bluetoothIcon.overlay.FontWeight({400});
                SetTrayForeground(g_bluetoothIcon.overlay, primaryBrush);
                SetTrayVisibility(g_bluetoothIcon.overlay,
                    (bluetoothAvailable || !g_settings.changeBluetoothGlyphWhenDisabled) ? wux::Visibility::Collapsed
                                       : wux::Visibility::Visible);
            }
        }

        if (g_networkIcon.primary) {
            NetworkState const& state = snapshot.network;
            if (state.kind != NetworkKind::WifiConnecting ||
                g_displayNetworkState.kind != NetworkKind::WifiConnecting) {
                g_wifiConnectingFrame = 0;
            }
            g_displayNetworkState = state;
            SetCachedTrayToolTip(g_networkButton, g_networkTooltipCache,
                                 GetNetworkTooltip(state));
            if (!TryApplyNativeNetworkGlyphs()) {
                g_usingNativeNetworkGlyphs = false;
                SetTrayGlyph(g_networkIcon.primary, GetNetworkGlyph(state));
                SetTrayForeground(g_networkIcon.primary, primaryBrush);
                if (g_networkIcon.underlay) {
                    SetTrayGlyph(g_networkIcon.underlay, L"\xE701");
                    SetTrayForeground(g_networkIcon.underlay, underlayBrush);
                    SetTrayVisibility(g_networkIcon.underlay,
                        state.kind == NetworkKind::Wifi ||
                                state.kind == NetworkKind::WifiConnecting
                            ? wux::Visibility::Visible
                            : wux::Visibility::Collapsed);
                }
                if (g_networkIcon.overlay) {
                    SetTrayVisibility(g_networkIcon.overlay, wux::Visibility::Collapsed);
                }
            }
        }

        if (g_soundIcon.primary) {
            SoundState const& state = snapshot.sound;
            const bool useOutputDeviceGlyph =
                g_settings.soundIconFollowsOutputDevice && state.available && !state.muted;
            SetCachedTrayToolTip(g_soundButton, g_soundTooltipCache,
                                 GetSoundTooltip(state));
            if (useOutputDeviceGlyph || !TryApplyNativeSoundGlyphs()) {
                g_usingNativeSoundGlyphs = false;
                SetTrayGlyph(g_soundIcon.primary, useOutputDeviceGlyph
                                          ? GetSoundOutputDeviceGlyph(state)
                                          : GetSoundGlyph(state));
                SetTrayForeground(g_soundIcon.primary, primaryBrush);
                if (g_soundIcon.underlay) {
                    SetTrayGlyph(g_soundIcon.underlay, L"\xEBC5");
                    SetTrayForeground(g_soundIcon.underlay, underlayBrush);
                    SetTrayVisibility(g_soundIcon.underlay,
                        state.available && !state.muted && !useOutputDeviceGlyph
                            ? wux::Visibility::Visible
                            : wux::Visibility::Collapsed);
                }
                if (g_soundIcon.overlay) {
                    SetTrayGlyph(g_soundIcon.overlay, L"\xE74F");
                    SetTrayForeground(g_soundIcon.overlay, primaryBrush);
                    SetTrayVisibility(g_soundIcon.overlay, wux::Visibility::Collapsed);
                }
            }
        }
        UpdateTimerCadence();
    } catch (...) {
        Wh_Log(L"UpdateDynamicXamlIcons error: 0x%08X", winrt::to_hresult());
    }
}

// Workers post native messages only; all XAML work stays on the owning thread.
static SRWLOCK g_refreshLock = SRWLOCK_INIT;
static HWND g_refreshWindow = nullptr;
static bool g_refreshClassRegistered = false;
static unsigned g_refreshPending = 0;
// Protect status-thread publication and refresh-event lifetime. Never join a
// thread while holding this lock: its notification callbacks can acquire it.
static SRWLOCK g_statusEventsLock = SRWLOCK_INIT;
static std::atomic<HANDLE> g_statusRefreshEvent{nullptr};
static std::atomic<unsigned> g_statusRefreshReasons{0};
static constexpr UINT kRefreshMessage = WM_APP + 164;
static constexpr UINT kDestroyRefreshWindowMessage = kRefreshMessage + 2;
static constexpr PCWSTR kRefreshWindowClass = L"SeparateSystemTrayIcons.Refresh";

static void PostTrayRefresh(bool radiosChanged) {
    AcquireSRWLockExclusive(&g_refreshLock);
    if (!g_unloading && g_refreshWindow) {
        const bool alreadyQueued = g_refreshPending != 0;
        g_refreshPending |= radiosChanged ? 3u : 1u;
        if (!alreadyQueued && !PostMessageW(g_refreshWindow, kRefreshMessage, 0, 0))
            g_refreshPending = 0;
    }
    ReleaseSRWLockExclusive(&g_refreshLock);
}

static void RequestTrayRefresh(unsigned reasons) {
    // Status callbacks can run on arbitrary threads. Signal the collector and
    // keep the current snapshot painted until the replacement is ready.
    AcquireSRWLockShared(&g_statusEventsLock);
    if (!g_unloading) {
        g_statusRefreshReasons.fetch_or(reasons);
        if (HANDLE refreshEvent = g_statusRefreshEvent.load())
            SetEvent(refreshEvent);
    }
    ReleaseSRWLockShared(&g_statusEventsLock);
    PostTrayRefresh((reasons & RefreshRadios) != 0);
}

static void RefreshBluetoothTooltipOnly() {
    if (!g_bluetoothButton) return;
    SetCachedTrayToolTip(g_bluetoothButton, g_bluetoothTooltipCache,
                        GetBluetoothTooltip(GetStatusSnapshot().bluetoothAvailable));
}

static void RequestBluetoothTooltipRefresh() {
    if (g_unloading || !g_refreshWindow || !g_settings.showConnectedDevicesInTooltip) return;
    if (!g_btQueries[0] && (g_btQueryInvalidated || !g_btQueryTick ||
                          GetTickCount64() - g_btQueryTick >= 30000)) {
        g_btQueryRequested = true;
        RefreshBluetoothTooltipOnly();
    }
    if (g_btQueries[0]) SetTimer(g_refreshWindow, 2, 250, nullptr);
}

static LRESULT CALLBACK TrayRefreshWindowProc(HWND hwnd, UINT message,
                                             WPARAM wp, LPARAM lp) {
    if (message == WM_NCDESTROY) {
        // Runs on the window/hook owner thread, including thread shutdown.
        AcquireSRWLockExclusive(&g_refreshLock);
        const bool current = g_refreshWindow == hwnd;
        if (current) {
            g_refreshWindow = nullptr;
            g_refreshPending = 0;
        }
        ReleaseSRWLockExclusive(&g_refreshLock);
        if (current && g_foregroundEventHook) {
            UnhookWinEvent(g_foregroundEventHook);
            g_foregroundEventHook = nullptr;
        }
    }
    if (message == kRefreshMessage + 1) {
        Wh_Log(L"Status event subscriptions ready: 0x%X", static_cast<unsigned>(wp));
        return 0;
    }
    if (message == kDestroyRefreshWindowMessage) {
        // This window belongs to the XAML owner thread, even when the taskbar
        // HWND has disappeared. Complete teardown before acknowledging unload.
        RemoveXamlButtons();
        return 1;
    }
    if (message == WM_POWERBROADCAST) {
        if (wp == PBT_POWERSETTINGCHANGE) {
            auto* setting = reinterpret_cast<POWERBROADCAST_SETTING*>(lp);
            if (setting) {
                const bool modern = IsEqualGUID(setting->PowerSetting, kEnergySaverStatus);
                const bool legacy = IsEqualGUID(setting->PowerSetting, kPowerSavingStatus);
                if ((modern || legacy) && setting->DataLength == sizeof(DWORD)) {
                    DWORD value = 0;
                    memcpy(&value, setting->Data, sizeof(value));
                    if (modern) g_modernEnergySaverStatusSeen.store(true);
                    // Modern Energy saver also operates on AC. Do not let
                    // the legacy battery-only status overwrite its report.
                    if (modern || !g_modernEnergySaverStatusSeen.load()) {
                        g_energySaverActive.store(value != 0 ? 1 : 0);
                        Wh_Log(L"Energy saver active state: %u (%s notification).",
                               value, modern ? L"modern" : L"legacy");
                        PostTrayRefresh(false);
                    }
                }
                if (modern || legacy || IsEqualGUID(setting->PowerSetting, GUID_ACDC_POWER_SOURCE))
                    InvalidateEnergySaverRead();
            }
        }
        RequestTrayRefresh(RefreshPower);
        return TRUE;
    }
    if (message == WM_DEVICECHANGE) {
        RequestTrayRefresh(RefreshRadios | RefreshNetwork | RefreshAudio | RefreshAudioDevices);
        return TRUE;
    }
    if (message == WM_TIMER && wp == 2) {
        if (g_unloading || GetTickCount64() - g_btQueryTick >= 10000) {
            for (auto& query : g_btQueries) {
                if (query) { try { query.Cancel(); } catch (...) {} }
                query = nullptr;
            }
            g_btQueryRequested = false;
            g_btQueryInvalidated = true;
        } else {
            // Poll only this requested tooltip, not every tray glyph and API.
            RefreshBluetoothTooltipOnly();
        }
        if (!g_btQueries[0]) KillTimer(hwnd, 2);
        return 0;
    }
    if (message == kRefreshMessage || (message == WM_TIMER && wp == 1)) {
        KillTimer(hwnd, 1);
        AcquireSRWLockExclusive(&g_refreshLock);
        const unsigned pending = g_refreshPending;
        g_refreshPending = 0;
        ReleaseSRWLockExclusive(&g_refreshLock);
        if (g_unloading) return 0;
        if (pending & 2) {
            g_airplaneModeState.store(-1);
            // Let an in-flight query finish, then collect the invalidated state.
            // Repeated notifications must not continually cancel its results.
            g_btQueryInvalidated = true;
        }
        RefreshAirplaneModeAsync();
        UpdateDynamicXamlIcons();
        // One follow-up allows Windows' asynchronous state propagation to settle.
        if (message == kRefreshMessage) SetTimer(hwnd, 1, 150, nullptr);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

static HMODULE GetModModule() {
    HMODULE owner = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<PCWSTR>(&TrayRefreshWindowProc), &owner)) {
        Wh_Log(L"Cannot resolve refresh window module: %lu", GetLastError());
    }
    return owner;
}

static bool IsTrayRefreshWindow(HWND hwnd, HMODULE owner) {
    return owner && hwnd && IsWindow(hwnd) &&
        reinterpret_cast<HMODULE>(GetClassLongPtrW(hwnd, GCLP_HMODULE)) == owner &&
        reinterpret_cast<WNDPROC>(GetClassLongPtrW(hwnd, GCLP_WNDPROC)) == TrayRefreshWindowProc;
}

static void EnsureTrayRefreshWindow() {
    if (g_unloading) return;
    const HMODULE owner = GetModModule();
    if (!owner || IsTrayRefreshWindow(g_refreshWindow, owner)) return;
    // The collector's power/device subscriptions target its original HWND.
    // Retire it off the UI thread; startup waits for its cleanup to finish.
    StopStatusEventsAsync();
    AcquireSRWLockExclusive(&g_refreshLock);
    g_refreshWindow = nullptr;
    g_refreshPending = 0;
    ReleaseSRWLockExclusive(&g_refreshLock);
    if (!g_refreshClassRegistered) {
        WNDCLASSW cls{};
        cls.lpfnWndProc = TrayRefreshWindowProc;
        cls.hInstance = owner;
        cls.lpszClassName = kRefreshWindowClass;
        if (!RegisterClassW(&cls)) {
            Wh_Log(L"Refresh window class registration failed: %lu", GetLastError());
            return;
        }
        g_refreshClassRegistered = true;
    }
    HWND hwnd = CreateWindowExW(0, kRefreshWindowClass, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, owner, nullptr);
    if (!hwnd) {
        Wh_Log(L"Refresh window creation failed: %lu", GetLastError());
        if (UnregisterClassW(kRefreshWindowClass, owner))
            g_refreshClassRegistered = false;
        return;
    }
    AcquireSRWLockExclusive(&g_refreshLock);
    g_refreshWindow = hwnd;
    ReleaseSRWLockExclusive(&g_refreshLock);
    g_foregroundEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        TrayFlyoutForegroundChanged, 0, 0,
        WINEVENT_OUTOFCONTEXT);
    if (hwnd) StartStatusEvents(hwnd);
}

static void DestroyTrayRefreshWindow() {
    if (g_foregroundEventHook) {
        UnhookWinEvent(g_foregroundEventHook);
        g_foregroundEventHook = nullptr;
    }
    AcquireSRWLockExclusive(&g_refreshLock);
    HWND hwnd = g_refreshWindow;
    g_refreshWindow = nullptr;
    g_refreshPending = 0;
    ReleaseSRWLockExclusive(&g_refreshLock);
    const HMODULE owner = GetModModule();
    if (IsTrayRefreshWindow(hwnd, owner)) {
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        if (!DestroyWindow(hwnd))
            Wh_Log(L"Refresh window destruction failed: %lu", GetLastError());
    }
    // The HWND can already be gone; the class still belongs to this module.
    if (owner && g_refreshClassRegistered) {
        if (UnregisterClassW(kRefreshWindowClass, owner)) {
            g_refreshClassRegistered = false;
            return;
        }
        const DWORD error = GetLastError();
        if (error != ERROR_CLASS_DOES_NOT_EXIST)
            Wh_Log(L"Refresh window class cleanup failed: %lu", error);
    }
}

struct AudioStatusObserver : winrt::implements<AudioStatusObserver,
        IMMNotificationClient, IAudioEndpointVolumeCallback> {
    HANDLE changed = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    ~AudioStatusObserver() { if (changed) CloseHandle(changed); }
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        RequestTrayRefresh(RefreshAudio); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) override {
        if (flow == eRender) SetEvent(changed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { SetEvent(changed); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { SetEvent(changed); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { SetEvent(changed); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        RequestTrayRefresh(RefreshAudio | RefreshAudioDevices); return S_OK;
    }
};

static void WINAPI NetworkInterfaceChanged(void*, PMIB_IPINTERFACE_ROW, MIB_NOTIFICATION_TYPE) {
    RequestTrayRefresh(RefreshNetwork);
}
static void WINAPI WirelessStatusChanged(PWLAN_NOTIFICATION_DATA, void*) {
    RequestTrayRefresh(RefreshNetwork);
}

struct StatusEventWork {
    HWND window;
    HANDLE stop;
    HANDLE refresh;
};
static HANDLE g_statusEventStop = nullptr;
static HANDLE g_statusEventThread = nullptr;
// Protected by g_statusEventsLock. Only the latest replacement is started,
// after the retiring collector has stopped touching shared status caches.
static bool g_statusEventsStopping = false;
static HWND g_statusRestartWindow = nullptr;
static HANDLE g_statusCleanupThread = nullptr;

static DWORD WINAPI StatusEventThread(void* parameter) {
    const auto work = *static_cast<StatusEventWork*>(parameter);
    delete static_cast<StatusEventWork*>(parameter);
    {
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    HANDLE ipNotification = nullptr, wlan = nullptr;
    HKEY keys[3]{};
    HANDLE keyEvents[3]{CreateEventW(nullptr, FALSE, FALSE, nullptr),
                        CreateEventW(nullptr, FALSE, FALSE, nullptr),
                        CreateEventW(nullptr, FALSE, FALSE, nullptr)};
    HPOWERNOTIFY power[4]{};
    std::vector<std::pair<HANDLE, HDEVNOTIFY>> radios;
    struct RadioStateSubscription {
        wdr::Radio radio;
        wdr::Radio::StateChanged_revoker changed;
    };
    // Keep event sources alive and revoke before releasing them/COM on exit.
    std::vector<RadioStateSubscription> radioStates;
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    winrt::com_ptr<IAudioEndpointVolume> volume;
    auto observer = winrt::make_self<AudioStatusObserver>();
    bool audioRegistered = false;
    unsigned sources = 0;
    auto bindVolume = [&] {
        if (volume) {
            volume->UnregisterControlChangeNotify(observer.get());
            volume = nullptr;
        }
        if (enumerator) {
            winrt::com_ptr<IMMDevice> endpoint;
            if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, endpoint.put())) &&
                SUCCEEDED(endpoint->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                    nullptr, volume.put_void()))) {
                if (FAILED(volume->RegisterControlChangeNotify(observer.get()))) volume = nullptr;
            }
        }
    };
    auto bindRadios = [&] {
        try {
            std::vector<RadioStateSubscription> next;
            for (auto const& radio : wdr::Radio::GetRadiosAsync().get()) {
                if (radio.Kind() != wdr::RadioKind::Bluetooth) continue;
                auto changed = radio.StateChanged(winrt::auto_revoke,
                    [](wdr::Radio const&, wf::IInspectable const&) {
                        // Device arrival can precede the transition to On.
                        // Collect again when the actual radio state changes.
                        RequestTrayRefresh(RefreshRadios);
                    });
                next.push_back({radio, std::move(changed)});
            }
            radioStates.swap(next);
        } catch (...) {
            // Keep existing subscriptions if an adapter enumeration fails.
            Wh_Log(L"Bluetooth state subscription failed: 0x%08X", winrt::to_hresult());
        }
        for (auto [handle, notification] : radios) {
            if (notification) UnregisterDeviceNotification(notification);
            CloseHandle(handle);
        }
        radios.clear();
        BLUETOOTH_FIND_RADIO_PARAMS params{sizeof(params)};
        HANDLE radio = nullptr;
        if (auto find = BluetoothFindFirstRadio(&params, &radio)) {
            do {
                DEV_BROADCAST_HANDLE filter{};
                filter.dbch_size = sizeof(filter);
                filter.dbch_devicetype = DBT_DEVTYP_HANDLE;
                filter.dbch_handle = radio;
                radios.emplace_back(radio, RegisterDeviceNotificationW(work.window,
                    &filter, DEVICE_NOTIFY_WINDOW_HANDLE));
            } while (BluetoothFindNextRadio(find, &radio));
            BluetoothFindRadioClose(find);
        }
    };
    if (SUCCEEDED(initialized)) {
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator), enumerator.put_void()))) {
            audioRegistered = SUCCEEDED(enumerator->RegisterEndpointNotificationCallback(observer.get()));
            bindVolume();
        }
    }
    if (audioRegistered) sources |= 1;
    if (volume) sources |= 2;
    if (NotifyIpInterfaceChange(AF_UNSPEC, NetworkInterfaceChanged, nullptr, FALSE, &ipNotification) == NO_ERROR)
        sources |= 4;
    DWORD version = 0;
    if (WlanOpenHandle(2, nullptr, &version, &wlan) == ERROR_SUCCESS &&
        WlanRegisterNotification(wlan, WLAN_NOTIFICATION_SOURCE_ACM | WLAN_NOTIFICATION_SOURCE_MSM,
            TRUE, WirelessStatusChanged, nullptr, nullptr, nullptr) == ERROR_SUCCESS) sources |= 8;
    power[0] = RegisterPowerSettingNotification(work.window, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);
    power[1] = RegisterPowerSettingNotification(work.window, &GUID_BATTERY_PERCENTAGE_REMAINING, DEVICE_NOTIFY_WINDOW_HANDLE);
    power[2] = RegisterPowerSettingNotification(work.window, &kPowerSavingStatus, DEVICE_NOTIFY_WINDOW_HANDLE);
    power[3] = RegisterPowerSettingNotification(work.window, &kEnergySaverStatus, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (power[0] && power[1]) sources |= 16;
    bindRadios();
    for (auto [handle, notification] : radios) if (notification) sources |= 32;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0, KEY_NOTIFY, &keys[0]);
    RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power",
        0, KEY_NOTIFY, &keys[1]);
    RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_NOTIFY, &keys[2]);
    auto armKey = [&](int i) {
        if (keys[i] && keyEvents[i])
            RegNotifyChangeKeyValue(keys[i], TRUE, REG_NOTIFY_CHANGE_LAST_SET, keyEvents[i], TRUE);
    };
    armKey(0); armKey(1); armKey(2);
    if (keys[0] && keys[1]) sources |= 64;
    PostMessageW(work.window, kRefreshMessage + 1, sources, 0);
    RefreshStatusSnapshot(RefreshAll);
    PostTrayRefresh(false);
    ULONGLONG lastRecovery = GetTickCount64();
    if (observer->changed && keyEvents[0] && keyEvents[1] && keyEvents[2]) {
        HANDLE waits[]{work.stop, work.refresh, observer->changed,
                       keyEvents[0], keyEvents[1], keyEvents[2]};
        while (true) {
            const ULONGLONG elapsed = GetTickCount64() - lastRecovery;
            const DWORD timeout = elapsed >= 30000 ? 0 : static_cast<DWORD>(30000 - elapsed);
            const DWORD result = WaitForMultipleObjects(6, waits, FALSE, timeout);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
            unsigned reasons = g_statusRefreshReasons.exchange(0);
            if (result == WAIT_OBJECT_0 + 2 ||
                WaitForSingleObject(observer->changed, 0) == WAIT_OBJECT_0) {
                bindVolume();
                reasons |= RefreshAudio | RefreshAudioDevices;
            }
            if (result == WAIT_OBJECT_0 + 3 || result == WAIT_OBJECT_0 + 4) {
                armKey(static_cast<int>(result - WAIT_OBJECT_0 - 3));
                InvalidateEnergySaverRead();
                reasons |= RefreshPower;
            }
            if (result == WAIT_OBJECT_0 + 5) {
                // Re-arm before publishing: the UI refresh recolors every glyph
                // and percentage text without re-enumerating status domains.
                armKey(2);
            }
            // Recover radio handles after adapters are unplugged/reconnected.
            if (GetTickCount64() - lastRecovery >= 30000) {
                reasons |= RefreshAll;
                lastRecovery = GetTickCount64();
            }
            if (reasons & RefreshAudioWork) {
                auto pending = TakeAudioWork();
                for (auto const& command : pending) {
                    if (g_unloading) break;
                    try {
                        if (command.kind == AudioWorkKind::Output) {
                            SetDefaultAudioOutputWorker(command.outputId);
                            bindVolume();
                            reasons |= RefreshAudioDevices;
                        } else {
                            if (!volume) bindVolume();
                            if (command.kind == AudioWorkKind::Volume)
                                StepDefaultEndpointVolumeWorker(volume.get(), command.steps,
                                                                command.configuredStep);
                            else ToggleDefaultEndpointMuteWorker(volume.get());
                        }
                    } catch (...) {
                        Wh_Log(L"Queued audio command failed: 0x%08X", winrt::to_hresult());
                    }
                }
                reasons = (reasons & ~RefreshAudioWork) | RefreshAudio;
            }
            if (reasons & RefreshRadios) bindRadios();
            if (reasons) RefreshStatusSnapshot(reasons);
            PostTrayRefresh(false);
        }
    }
    // Never hold g_refreshLock while unregistering: APIs may wait for callbacks.
    radioStates.clear();
    if (ipNotification) CancelMibChangeNotify2(ipNotification);
    if (wlan) {
        WlanRegisterNotification(wlan, WLAN_NOTIFICATION_SOURCE_NONE, TRUE,
            nullptr, nullptr, nullptr, nullptr);
        WlanCloseHandle(wlan, nullptr);
    }
    if (volume) volume->UnregisterControlChangeNotify(observer.get());
    if (audioRegistered) enumerator->UnregisterEndpointNotificationCallback(observer.get());
    volume = nullptr; enumerator = nullptr; observer = nullptr;
    for (auto [handle, notification] : radios) {
        if (notification) UnregisterDeviceNotification(notification);
        CloseHandle(handle);
    }
    for (auto notification : power) if (notification) UnregisterPowerSettingNotification(notification);
    for (auto key : keys) if (key) RegCloseKey(key);
    for (auto event : keyEvents) if (event) CloseHandle(event);
    if (SUCCEEDED(initialized)) CoUninitialize();
    }
    return 0;
}

static void StartStatusEvents(HWND hwnd) {
    AcquireSRWLockExclusive(&g_statusEventsLock);
    if (hwnd) g_statusRestartWindow = hwnd;
    hwnd = g_statusRestartWindow;
    if (g_statusEventsStopping || g_statusEventStop || g_unloading ||
        !hwnd || !IsWindow(hwnd)) {
        ReleaseSRWLockExclusive(&g_statusEventsLock);
        return;
    }
    HANDLE stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE refresh = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!stop || !refresh) {
        if (stop) CloseHandle(stop);
        if (refresh) CloseHandle(refresh);
        ReleaseSRWLockExclusive(&g_statusEventsLock);
        return;
    }
    auto work = new (std::nothrow) StatusEventWork{hwnd, stop, refresh};
    HANDLE thread = work ? CreateThread(nullptr, 0, StatusEventThread, work, 0, nullptr) : nullptr;
    if (thread) {
        g_statusEventStop = stop;
        g_statusRefreshEvent.store(refresh);
        g_statusEventThread = thread;
    }
    else {
        delete work; CloseHandle(stop); CloseHandle(refresh);
    }
    ReleaseSRWLockExclusive(&g_statusEventsLock);
}

static void StopStatusEventsAsync() {
    AcquireSRWLockExclusive(&g_statusEventsLock);
    g_statusRestartWindow = nullptr;
    if (g_statusEventsStopping || !g_statusEventThread || g_unloading) {
        ReleaseSRWLockExclusive(&g_statusEventsLock);
        return;
    }
    const HANDLE thread = g_statusEventThread;
    const HANDLE stop = g_statusEventStop;
    const HANDLE refresh = g_statusRefreshEvent.load();
    if (stop) SetEvent(stop);
    if (refresh) SetEvent(refresh);
    g_statusEventsStopping = true;
    HANDLE cleanup = StartOwnedWorker([thread, stop, refresh] {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        if (stop) CloseHandle(stop);
        if (refresh) CloseHandle(refresh);
        AcquireSRWLockExclusive(&g_statusEventsLock);
        g_statusEventsStopping = false;
        ReleaseSRWLockExclusive(&g_statusEventsLock);
        StartStatusEvents(nullptr);
    });
    if (cleanup) {
        // Transfer ownership only after the cleanup worker is tracked for unload.
        g_statusEventThread = nullptr;
        g_statusEventStop = nullptr;
        g_statusRefreshEvent.store(nullptr);
        if (g_statusCleanupThread) CloseHandle(g_statusCleanupThread);
        g_statusCleanupThread = cleanup;
    } else {
        // Retain all handles for synchronous unload; never wait on the UI thread.
        g_statusEventsStopping = false;
        Wh_Log(L"Could not start status cleanup worker; recovery deferred until reload.");
    }
    ReleaseSRWLockExclusive(&g_statusEventsLock);
}

static void StopStatusEvents() {
    // Unload sets g_unloading before taking this lock. A startup already in
    // progress must publish its handles first; any later startup is rejected.
    AcquireSRWLockExclusive(&g_statusEventsLock);
    HANDLE stop = std::exchange(g_statusEventStop, nullptr);
    HANDLE thread = std::exchange(g_statusEventThread, nullptr);
    HANDLE cleanup = std::exchange(g_statusCleanupThread, nullptr);
    HANDLE refreshEvent = g_statusRefreshEvent.exchange(nullptr);
    if (stop) SetEvent(stop);
    if (refreshEvent) SetEvent(refreshEvent);
    ReleaseSRWLockExclusive(&g_statusEventsLock);
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    if (stop) CloseHandle(stop);
    if (refreshEvent) CloseHandle(refreshEvent);
    if (cleanup) {
        WaitForSingleObject(cleanup, INFINITE);
        CloseHandle(cleanup);
    }
}

enum class ButtonKind {
    Bluetooth,
    Network,
    Sound,
    ControlCenter,
    Battery,
    QuickSettings = ControlCenter, // source compatibility for existing paths
};

static PCWSTR ButtonKindName(ButtonKind kind) {
    switch (kind) {
        case ButtonKind::Bluetooth:
            return L"bluetooth";
        case ButtonKind::Network:
            return L"network";
        case ButtonKind::Sound:
            return L"sound";
        case ButtonKind::ControlCenter:
            return L"controlcenter";
        case ButtonKind::Battery:
            return L"battery";
        default:
            return L"unknown";
    }
}

enum class TrayContextCommand : UINT {
    BluetoothAddDevice = 1,
    BluetoothSendFile,
    BluetoothReceiveFile,
    BluetoothSettings,
    BluetoothTroubleshoot,
    NetworkSpeedTest,
    NetworkSettings,
    NetworkToggleAirplaneMode,
    NetworkFirewallProtection,
    NetworkTroubleshoot,
    SoundSettings,
    SoundMixer,
    SoundSounds,
    SoundTroubleshoot,
    QuickSettingsTaskManager,
    QuickSettingsTaskbarSettings,
    QuickSettingsSystemSettings,
    BatteryPowerSettings,
};

static void ExecuteTrayContextCommand(TrayContextCommand command) {
    switch (command) {
        case TrayContextCommand::BluetoothAddDevice:
            OpenAddBluetoothDevice();
            break;
        case TrayContextCommand::BluetoothSendFile:
            OpenBluetoothFileTransfer(true);
            break;
        case TrayContextCommand::BluetoothReceiveFile:
            OpenBluetoothFileTransfer(false);
            break;
        case TrayContextCommand::BluetoothSettings:
            ExecuteAction(L"ms-settings:bluetooth");
            break;
        case TrayContextCommand::BluetoothTroubleshoot:
            ExecuteAction(L"ms-settings:troubleshoot");
            break;
        case TrayContextCommand::NetworkSpeedTest:
            if (g_settings.speedTestUrl.starts_with(L"https://") ||
                g_settings.speedTestUrl.starts_with(L"http://")) {
                ExecuteAction(L"web:" + g_settings.speedTestUrl);
            } else {
                Wh_Log(L"Speed test custom URL is invalid; expected http:// or https://.");
            }
            break;
        case TrayContextCommand::NetworkSettings:
            ExecuteAction(L"ms-settings:network");
            break;
        case TrayContextCommand::NetworkToggleAirplaneMode:
            SetAirplaneModeLikelyEnabled(g_airplaneModeState.load() != 1);
            break;
        case TrayContextCommand::NetworkFirewallProtection:
            ExecuteAction(L"windowsdefender://Network/");
            break;
        case TrayContextCommand::NetworkTroubleshoot:
            ExecuteAction(L"ms-settings:troubleshoot");
            break;
        case TrayContextCommand::SoundSettings:
            ExecuteAction(L"ms-settings:sound");
            break;
        case TrayContextCommand::SoundMixer:
            OpenVolumeMixer();
            break;
        case TrayContextCommand::SoundSounds:
            OpenSoundsControlPanel();
            break;
        case TrayContextCommand::SoundTroubleshoot:
            ExecuteAction(L"ms-settings:troubleshoot");
            break;
        case TrayContextCommand::QuickSettingsTaskManager:
            ExecuteAction(L"taskmgr.exe");
            break;
        case TrayContextCommand::QuickSettingsTaskbarSettings:
            ExecuteAction(L"ms-settings:taskbar");
            break;
        case TrayContextCommand::QuickSettingsSystemSettings:
            ExecuteAction(L"ms-settings:");
            break;
        case TrayContextCommand::BatteryPowerSettings:
            ExecuteAction(L"ms-settings:powersleep");
            break;
        default:
            break;
    }
}

static void AppendWinUiContextItem(
    wfc::IVector<wuc::MenuFlyoutItemBase> const& items, PCWSTR text,
    TrayContextCommand command, PCWSTR glyph = nullptr) {
    wuc::MenuFlyoutItem item;
    item.Text(text);
    if (glyph && *glyph) {
        wuc::FontIcon icon;
        icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
        icon.Glyph(glyph);
        icon.Width(16);
        icon.Height(16);
        icon.FontSize(16);
        item.Icon(icon);
    }
    {
        auto eventSource = item;
        auto eventToken = eventSource.Click([command](wf::IInspectable const&,
                         wux::RoutedEventArgs const&) {
        if (g_unloading) return;
        ExecuteTrayContextCommand(command);
    });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
    }
    items.Append(item);
}

static void AppendWinUiSeparator(
    wfc::IVector<wuc::MenuFlyoutItemBase> const& items) {
    items.Append(wuc::MenuFlyoutSeparator());
}

static void MoveWinUiItemToIndex(
    wfc::IVector<wuc::MenuFlyoutItemBase> const& items,
    PCWSTR text, uint32_t targetIndex) {
    for (uint32_t i = 0; i < items.Size(); ++i) {
        auto item = items.GetAt(i).try_as<wuc::MenuFlyoutItem>();
        auto subItem = items.GetAt(i).try_as<wuc::MenuFlyoutSubItem>();
        if ((!item || item.Text() != text) &&
            (!subItem || subItem.Text() != text)) continue;
        auto value = items.GetAt(i);
        items.RemoveAt(i);
        if (targetIndex > items.Size()) targetIndex = items.Size();
        items.InsertAt(targetIndex, value);
        return;
    }
}

static void MoveWinUiTroubleshootToBottom(
    wfc::IVector<wuc::MenuFlyoutItemBase> const& items,
    PCWSTR text) {
    for (uint32_t i = 0; i < items.Size(); ++i) {
        auto item = items.GetAt(i).try_as<wuc::MenuFlyoutItem>();
        if (!item || item.Text() != text) continue;
        auto value = items.GetAt(i);
        items.RemoveAt(i);
        items.Append(value);
        return;
    }
}

static void AppendWinUiBluetoothContextMenu(wuc::MenuFlyout const& flyout) {
    auto items = flyout.Items();
    if (g_settings.swapTroubleshootingWithBottom) {
        AppendWinUiContextItem(items, L"Send a File",
                               TrayContextCommand::BluetoothSendFile, L"\xE724");
        AppendWinUiContextItem(items, L"Receive a File",
                               TrayContextCommand::BluetoothReceiveFile, L"\xE896");
        AppendWinUiSeparator(items);
    } else {
        AppendWinUiContextItem(items, L"Troubleshoot bluetooth problems",
                               TrayContextCommand::BluetoothTroubleshoot,
                               L"\xE90F");
        AppendWinUiSeparator(items);
    }
    AppendWinUiContextItem(items, L"Bluetooth settings",
                           TrayContextCommand::BluetoothSettings, L"\xE713");
    AppendWinUiContextItem(items, L"Connect a new device",
                           TrayContextCommand::BluetoothAddDevice, L"\xE710");
    AppendWinUiSeparator(items);
    if (g_settings.swapTroubleshootingWithBottom) {
        AppendWinUiContextItem(items, L"Troubleshoot bluetooth problems",
                               TrayContextCommand::BluetoothTroubleshoot,
                               L"\xE90F");
    } else {
        AppendWinUiContextItem(items, L"Send a File",
                               TrayContextCommand::BluetoothSendFile, L"\xE724");
        AppendWinUiContextItem(items, L"Receive a File",
                               TrayContextCommand::BluetoothReceiveFile, L"\xE896");
    }
    if (g_settings.swapTroubleshootingWithBottom) {
        MoveWinUiTroubleshootToBottom(items, L"Troubleshoot bluetooth problems");
    }
}

static void AppendWinUiSoundContextMenu(wuc::MenuFlyout const& flyout) {
    auto items = flyout.Items();
    if (g_settings.swapTroubleshootingWithBottom) {
        AppendWinUiContextItem(items, L"Sounds", TrayContextCommand::SoundSounds,
                               L"\xE8D6");
        AppendWinUiSeparator(items);
    } else {
        AppendWinUiContextItem(items, L"Troubleshoot sound problems",
                               TrayContextCommand::SoundTroubleshoot, L"\xE90F");
        AppendWinUiSeparator(items);
    }
    AppendWinUiContextItem(items, L"Sound settings",
                           TrayContextCommand::SoundSettings, L"\xE713");
    AppendWinUiContextItem(items, L"Open volume mixer",
                           TrayContextCommand::SoundMixer, L"\xE9E9");
    AppendWinUiSeparator(items);

    wuc::MenuFlyoutSubItem outputSubmenu;
    outputSubmenu.Text(L"Output device");
    {
        wuc::FontIcon icon;
        icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
        icon.Glyph(L"\xE995");
        icon.Width(16);
        icon.Height(16);
        icon.FontSize(16);
        outputSubmenu.Icon(icon);
    }
    // Endpoint enumeration is collected by the status worker. Building a
    // context menu must not synchronously RPC into the audio service.
    auto outputs = GetStatusSnapshot().audioOutputs;
    if (outputs.empty()) {
        wuc::MenuFlyoutItem noOutput;
        noOutput.Text(L"No output devices");
        noOutput.IsEnabled(false);
        outputSubmenu.Items().Append(noOutput);
    } else {
        for (auto const& output : outputs) {
            wuc::ToggleMenuFlyoutItem outputItem;
            outputItem.Text(output.name);
            outputItem.IsChecked(output.isDefault);
            {
        auto eventSource = outputItem;
        auto eventToken = eventSource.Click([id = output.id](wf::IInspectable const&,
                                              wux::RoutedEventArgs const&) {
        if (g_unloading) return;
                SetDefaultAudioOutput(id);
            });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
    }
            outputSubmenu.Items().Append(outputItem);
        }
    }
    if (!g_settings.swapTroubleshootingWithBottom) {
        AppendWinUiContextItem(items, L"Sounds", TrayContextCommand::SoundSounds,
                               L"\xE8D6");
    }
    items.Append(outputSubmenu);

    if (g_settings.swapTroubleshootingWithBottom) {
        MoveWinUiItemToIndex(items, L"Output device", 0);
        MoveWinUiItemToIndex(items, L"Sounds", 1);
        AppendWinUiContextItem(items, L"Troubleshoot sound problems",
                               TrayContextCommand::SoundTroubleshoot, L"\xE90F");
    }
}

static void AppendWinUiNetworkContextMenu(wuc::MenuFlyout const& flyout) {
    auto items = flyout.Items();
    RefreshAirplaneModeAsync();
    const bool airplaneEnabled = g_airplaneModeState.load() == 1;

    auto appendAirplane = [&]() {
        wuc::MenuFlyoutItem airplaneItem;
        airplaneItem.Text(airplaneEnabled ? L"Disable Airplane mode"
                                          : L"Enable Airplane mode");
        {
            wuc::FontIcon icon;
            icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
            icon.Glyph(L"\xE709");
            icon.Width(16);
            icon.Height(16);
            icon.FontSize(16);
            airplaneItem.Icon(icon);
        }
        auto eventSource = airplaneItem;
        auto eventToken = eventSource.Click([](wf::IInspectable const&,
                                               wux::RoutedEventArgs const&) {
            if (g_unloading) return;
            ExecuteTrayContextCommand(TrayContextCommand::NetworkToggleAirplaneMode);
        });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
        items.Append(airplaneItem);
    };

    if (g_settings.swapTroubleshootingWithBottom) {
        appendAirplane();
        AppendWinUiSeparator(items);
    } else {
        AppendWinUiContextItem(items, L"Troubleshoot network problems",
                               TrayContextCommand::NetworkTroubleshoot, L"\xE90F");
        AppendWinUiSeparator(items);
    }
    AppendWinUiContextItem(items, L"Network and Internet settings",
                           TrayContextCommand::NetworkSettings, L"\xE713");
    AppendWinUiContextItem(items, L"Firewall protection",
                           TrayContextCommand::NetworkFirewallProtection,
                           L"\xEA18");
    AppendWinUiSeparator(items);
    AppendWinUiContextItem(items, L"Perform speed test",
                           TrayContextCommand::NetworkSpeedTest, L"\xF42F");

    if (g_settings.swapTroubleshootingWithBottom) {
        AppendWinUiContextItem(items, L"Troubleshoot network problems",
                               TrayContextCommand::NetworkTroubleshoot, L"\xE90F");
        MoveWinUiItemToIndex(items, L"Perform speed test", 0);
        MoveWinUiItemToIndex(items, airplaneEnabled ? L"Disable Airplane mode"
                                                    : L"Enable Airplane mode", 1);
        MoveWinUiTroubleshootToBottom(items, L"Troubleshoot network problems");
    } else {
        appendAirplane();
    }
}

static void AppendWinUiQuickSettingsContextMenu(wuc::MenuFlyout const& flyout) {
    auto items = flyout.Items();
    AppendWinUiContextItem(items, L"Task Manager",
                           TrayContextCommand::QuickSettingsTaskManager,
                           L"\xE9D9");
    AppendWinUiSeparator(items);
    AppendWinUiContextItem(items, L"Taskbar settings",
                           TrayContextCommand::QuickSettingsTaskbarSettings,
                           L"\xE713");
    AppendWinUiContextItem(items, L"System settings",
                           TrayContextCommand::QuickSettingsSystemSettings,
                           L"\xE713");
}

// Private Settings ABI: resolve dynamically and require the exact interface.
// This changes the AlwaysOn preference, not the transient energy-saver status.
struct BatterySettingItem : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Reserved6() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved7() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved8() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved9() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved10() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved11() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved12() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(HSTRING, ::IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetValue(HSTRING, ::IInspectable*) = 0;
};
__CRT_UUID_DECL(BatterySettingItem, 0x40c037cc, 0xd8bf, 0x489e,
                0x86, 0x97, 0xd6, 0x6b, 0xaa, 0x32, 0x21, 0xbf)

static winrt::com_ptr<BatterySettingItem> GetBatteryBooleanSetting(PCWSTR settingName = L"SystemSettings_PowerAndBattery_EnergySaverAlwaysOn") {
    if (!g_batterySettingsModule) {
        g_batterySettingsModule = LoadLibraryExW(
            L"SettingsHandlers_OneCore_BatterySaver.dll", nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    using GetSetting = HRESULT(WINAPI*)(HSTRING, ::IInspectable**);
    const auto getSetting = g_batterySettingsModule ? reinterpret_cast<GetSetting>(
        GetProcAddress(g_batterySettingsModule, "GetSetting")) : nullptr;
    if (!getSetting) winrt::throw_hresult(E_NOTIMPL);
    winrt::hstring name{settingName};
    winrt::com_ptr<::IInspectable> object;
    winrt::check_hresult(getSetting(
        reinterpret_cast<HSTRING>(winrt::get_abi(name)), object.put()));
    return object.as<BatterySettingItem>();
}

static bool ReadBatteryBooleanSetting(
    winrt::com_ptr<BatterySettingItem> const& setting) {
    winrt::hstring key{L"Value"};
    wf::IInspectable value{nullptr};
    winrt::check_hresult(setting->GetValue(
        reinterpret_cast<HSTRING>(winrt::get_abi(key)),
        reinterpret_cast<::IInspectable**>(winrt::put_abi(value))));
    return winrt::unbox_value<bool>(value);
}

static std::atomic<bool> g_energySaverBusy{false};
static std::atomic<bool> g_energySaverReadInvalidated{true};
static winrt::weak_ref<wuc::MenuFlyoutItem> g_energySaverMenuItem;
static winrt::weak_ref<wuc::MenuFlyoutItem> g_batteryPercentageMenuItem;
static void InvalidateEnergySaverRead() {
    // Keep a usable cached value while a fresh Settings read is in flight.
    g_energySaverReadInvalidated.store(true);
}
static SRWLOCK g_energySaverQueueLock = SRWLOCK_INIT;
static unsigned g_batteryOperationsQueued = 0;

struct EnergySaverWork {
    int operation;  // 0: refresh, 1: saver toggle, 2: percentage toggle, 3: initial percentage enable.
};

static DWORD WINAPI EnergySaverWorker(void* parameter) {
    auto work = *static_cast<EnergySaverWork*>(parameter);
    delete static_cast<EnergySaverWork*>(parameter);
nextOperation:
    bool stateChanged = false;
    HRESULT error = S_OK;
    bool initialized = false;
    try {
        // All Settings objects are created, used and released in this apartment.
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        initialized = true;
        {
            const bool percentageOperation = work.operation == 2 || work.operation == 3;
            auto setting = GetBatteryBooleanSetting(percentageOperation
                ? L"SystemSettings_PowerAndBattery_BatteryPercentageToggle"
                : L"SystemSettings_PowerAndBattery_EnergySaverAlwaysOn");
            bool enabled = ReadBatteryBooleanSetting(setting);
            if (work.operation && (work.operation != 3 || !enabled)) {
                const bool requested = work.operation == 3 ? true : !enabled;
                auto value = winrt::box_value(requested);
                winrt::hstring key{L"Value"};
                winrt::check_hresult(setting->SetValue(
                    reinterpret_cast<HSTRING>(winrt::get_abi(key)),
                    reinterpret_cast<::IInspectable*>(winrt::get_abi(value))));
                enabled = ReadBatteryBooleanSetting(setting);
                // SetValue can return before the Settings handler publishes
                // its new value. Reopen the object to avoid its stale cache.
                // This bounded verification runs only on the owned worker.
                for (int attempt = 0; enabled != requested && attempt < 15 && !g_unloading; ++attempt) {
                    Sleep(100);
                    setting = GetBatteryBooleanSetting(percentageOperation
                        ? L"SystemSettings_PowerAndBattery_BatteryPercentageToggle"
                        : L"SystemSettings_PowerAndBattery_EnergySaverAlwaysOn");
                    enabled = ReadBatteryBooleanSetting(setting);
                }
                if (enabled != requested)
                    Wh_Log(L"Battery setting change still pending: operation=%d requested=%d observed=%d.",
                           work.operation, requested, enabled);
            }
            if (percentageOperation)
                stateChanged = g_batteryPercentageEnabled.exchange(enabled ? 1 : 0) != (enabled ? 1 : 0);
            else stateChanged = g_energySaverState.exchange(enabled ? 1 : 0) != (enabled ? 1 : 0);
            if (work.operation == 3 && enabled)
                Wh_SetIntValue(L"batteryPercentageDefaultApplied", 1);
        }
    } catch (...) {
        error = winrt::to_hresult();
    }
    if (initialized) winrt::uninit_apartment();
    if (FAILED(error) && work.operation) {
        Wh_Log(L"Windows could not change the battery setting: 0x%08X", error);
    }
    AcquireSRWLockExclusive(&g_energySaverQueueLock);
    if (g_batteryOperationsQueued) {
        work.operation = (g_batteryOperationsQueued & 1) ? 1 :
            (g_batteryOperationsQueued & 2) ? 2 : 3;
        g_batteryOperationsQueued &= ~(1u << (work.operation - 1));
        ReleaseSRWLockExclusive(&g_energySaverQueueLock);
        goto nextOperation;
    }
    g_energySaverBusy.store(false);
    ReleaseSRWLockExclusive(&g_energySaverQueueLock);
    // Refresh open menu items even when the value did not change: the first
    // read may have completed after the menu was built in an unknown state.
    PostTrayRefresh(false);
    if (work.operation || stateChanged) RequestTrayRefresh(RefreshPower);
    return 0;
}

static void QueueEnergySaverWork(int operation) {
    if (g_unloading) return;
    AcquireSRWLockExclusive(&g_energySaverQueueLock);
    if (g_energySaverBusy.exchange(true)) {
        // Preserve a click that arrives while a background state read is running.
        if (operation) g_batteryOperationsQueued |= 1u << (operation - 1);
        ReleaseSRWLockExclusive(&g_energySaverQueueLock);
        return;
    }
    ReleaseSRWLockExclusive(&g_energySaverQueueLock);
    auto work = new (std::nothrow) EnergySaverWork{operation};
    HANDLE thread = work ? StartOwnedWorker([work] { EnergySaverWorker(work); }) : nullptr;
    if (thread) {
        CloseHandle(thread);
    } else {
        delete work;
        g_energySaverBusy.store(false);
    }
}

static void RefreshEnergySaverStateAsync() {
    // Registry/power notifications invalidate the cache. Do not continuously
    // create a Settings apartment merely to poll an unchanged preference.
    if (g_batteryButton && !g_energySaverBusy.load() &&
        g_energySaverReadInvalidated.exchange(false))
        QueueEnergySaverWork(false);
}

static void AppendWinUiEnergySaverItem(wuc::MenuFlyout const& flyout) {
    wuc::MenuFlyoutItem item;
    item.Text(L"Enable Energy saver");
    wuc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\xE8BE");
    icon.Width(16);
    icon.Height(16);
    icon.FontSize(16);
    item.Icon(icon);
    const int state = GetEnergySaverDisplayState();
    item.Text(state == 1 ? L"Disable Energy saver" : L"Enable Energy saver");
    item.IsEnabled(state >= 0);
    g_energySaverMenuItem = winrt::make_weak(item);
    {
        auto eventSource = item;
        auto eventToken = eventSource.Click([](auto const&, auto const&) {
        if (g_unloading) return;
        QueueEnergySaverWork(true);
    });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
    }
    flyout.Items().Append(item);
}

static void SetBatteryPercentageCheck(wuc::MenuFlyoutItem const& item, bool enabled) {
    // Use the shared icon column; ToggleMenuFlyoutItem reserves a separate one.
    wuc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(enabled ? L"\xE73E" : L"");
    icon.Width(16);
    icon.Height(16);
    icon.FontSize(12);
    item.Icon(icon);
    winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetItemStatus(item, enabled ? L"Checked" : L"Unchecked");
}

static void RefreshBatteryMenuItems() {
    if (auto item = g_energySaverMenuItem.get()) {
        const int state = GetEnergySaverDisplayState();
        const winrt::hstring text = state == 1 ? L"Disable Energy saver" : L"Enable Energy saver";
        if (item.Text() != text) item.Text(text);
        if (item.IsEnabled() != (state >= 0)) item.IsEnabled(state >= 0);
    }
    if (auto item = g_batteryPercentageMenuItem.get()) {
        const int state = g_batteryPercentageEnabled.load();
        if (item.IsEnabled() != (state >= 0)) item.IsEnabled(state >= 0);
        const winrt::hstring status = state == 1 ? L"Checked" : L"Unchecked";
        if (wuxa::AutomationProperties::GetItemStatus(item) != status)
            SetBatteryPercentageCheck(item, state == 1);
    }
}

static void AppendWinUiBatteryPercentageItem(wuc::MenuFlyout const& flyout) {
    auto toggleAfterClose = std::make_shared<bool>(false);
    auto closedToken = flyout.Closed([toggleAfterClose](auto const&, auto const&) {
        // Resizing an open menu's anchor during its closing animation makes
        // the popup briefly move/clip. Commit the setting after it has closed.
        if (std::exchange(*toggleAfterClose, false) && !g_unloading)
            QueueEnergySaverWork(2);
    });
    g_contextMenuEventRevokers.push_back([weak = winrt::make_weak(flyout), closedToken] {
        if (auto source = weak.get()) source.Closed(closedToken);
    });
    wuc::MenuFlyoutItem item;
    item.Text(L"Battery percentage");
    const int state = g_batteryPercentageEnabled.load();
    SetBatteryPercentageCheck(item, state == 1);
    item.IsEnabled(state >= 0);
    g_batteryPercentageMenuItem = winrt::make_weak(item);
    {
        auto eventSource = item;
        auto eventToken = eventSource.Click([toggleAfterClose, weak = winrt::make_weak(flyout)](auto const&, auto const&) {
        if (g_unloading) return;
        if (auto menu = weak.get()) {
            *toggleAfterClose = true;
            menu.Hide();
        }
    });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
    }
    flyout.Items().Append(item);
}
static void AppendWinUiBatteryContextMenu(wuc::MenuFlyout const& flyout) {
    // Separate user preferences for AC and DC, not legacy power-plan GUIDs.
    using GetMode = DWORD(WINAPI*)(GUID*);
    using SetMode = DWORD(WINAPI*)(const GUID*);
    if (!g_powerProfileModule) {
        g_powerProfileModule = LoadLibraryExW(L"powrprof.dll", nullptr,
                                              LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    const GUID modes[] = {
        {0x961cc777, 0x2547, 0x4f9d, {0x81,0x74,0x7d,0x86,0x18,0x1b,0x8a,0x7a}},
        {},
        {0xded574b5, 0x45a0, 0x4f42, {0x87,0x37,0x46,0x34,0x5c,0x09,0xc2,0x38}},
    };
    const PCWSTR labels[] = {L"Best power efficiency", L"Balanced", L"Best performance"};
    wuc::MenuFlyoutSubItem powerMode;
    powerMode.Text(L"Power mode");
    wuc::FontIcon powerIcon;
    powerIcon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    powerIcon.Glyph(L"\xEC48");
    powerIcon.Width(16);
    powerIcon.Height(16);
    powerIcon.FontSize(16);
    powerMode.Icon(powerIcon);
    SYSTEM_POWER_STATUS supplyStatus{};
    const bool supplyKnown = GetSystemPowerStatus(&supplyStatus) && supplyStatus.ACLineStatus != 255;
    const bool ac = supplyKnown && supplyStatus.ACLineStatus == 1;
    {
        auto getMode = g_powerProfileModule ? reinterpret_cast<GetMode>(GetProcAddress(g_powerProfileModule,
            ac ? "PowerGetUserConfiguredACPowerMode" : "PowerGetUserConfiguredDCPowerMode")) : nullptr;
        auto setMode = g_powerProfileModule ? reinterpret_cast<SetMode>(GetProcAddress(g_powerProfileModule,
            ac ? "PowerSetUserConfiguredACPowerMode" : "PowerSetUserConfiguredDCPowerMode")) : nullptr;


        GUID current{};
        const bool readable = getMode && getMode(&current) == ERROR_SUCCESS;
        powerMode.IsEnabled(supplyKnown && readable && setMode);
        for (int i = 0; i < 3; ++i) {
            wuc::ToggleMenuFlyoutItem item;
            item.Text(labels[i]);
            item.IsChecked(readable && IsEqualGUID(current, modes[i]));
            const GUID mode = modes[i];
            {
        auto eventSource = item;
        auto eventToken = eventSource.Click([setMode, mode](auto const&, auto const&) {
        if (g_unloading) return;
                const DWORD error = setMode ? setMode(&mode) : ERROR_NOT_SUPPORTED;
                if (error != ERROR_SUCCESS) {
                    Wh_Log(L"Changing power mode failed: %lu", error);
                }
            });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Click(eventToken);
        });
    }
            powerMode.Items().Append(item);
        }

    }
    flyout.Items().Append(powerMode);
    AppendWinUiEnergySaverItem(flyout);
    AppendWinUiBatteryPercentageItem(flyout);
    AppendWinUiSeparator(flyout.Items());
    AppendWinUiContextItem(flyout.Items(), L"Power and sleep settings",
                           TrayContextCommand::BatteryPowerSettings, L"\xE713");
}

static void PositionTrayMenuPopup(wux::XamlRoot const& root,
                                 wuc::MenuFlyout const& flyout,
                                 wux::FrameworkElement const& target) {
    RECT bar{}, host{};
    bool horizontal{}, first{};
    if (!root || !GetTaskbarGeometry(&bar, &host, &horizontal, &first)) return;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi)) return;
    const double scale = root.RasterizationScale();
    const double gap = 12.0 * scale;
    for (auto const& popup : wuxm::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
        auto presenter = popup.Child().try_as<wuc::MenuFlyoutPresenter>();
        if (!presenter || presenter.Items().Size() == 0 || flyout.Items().Size() == 0 ||
            presenter.Items().GetAt(0) != flyout.Items().GetAt(0)) continue;
        presenter.Name(L"SeparateTrayContextMenuPresenter");
        presenter.UpdateLayout();
        auto origin = presenter.TransformToVisual(root.Content()).TransformPoint({0, 0});
        const double width = presenter.ActualWidth() * scale;
        const double height = presenter.ActualHeight() * scale;
        if (width <= 0 || height <= 0) continue;
        double x = host.left + origin.X * scale;
        double y = host.top + origin.Y * scale;
        const double oldX = x, oldY = y;
        if (horizontal) {
            // Align with the button; the monitor clamp below keeps menus
            // inside the existing edge inset when there is insufficient room.
            auto targetOrigin = target.TransformToVisual(root.Content())
                                    .TransformPoint({0, 0});
            x = host.left + targetOrigin.X * scale;
            y = first ? bar.bottom + gap : bar.top - gap - height;
        } else {
            x = first ? bar.right + gap : bar.left - gap - width;
            y = mi.rcMonitor.bottom - gap - height;
        }
        x = (std::max)(mi.rcMonitor.left + gap,
                       (std::min)(x, mi.rcMonitor.right - gap - width));
        y = (std::max)(mi.rcMonitor.top + gap,
                       (std::min)(y, mi.rcMonitor.bottom - gap - height));
        popup.HorizontalOffset(popup.HorizontalOffset() + (x - oldX) / scale);
        popup.VerticalOffset(popup.VerticalOffset() + (y - oldY) / scale);
    }
}

static void ShowWinUiFlyoutNearTaskbar(wuc::MenuFlyout const& flyout,
                                       wux::FrameworkElement const& target) {
    RECT taskbarRect{};
    RECT hostRect{};
    bool horizontal = true;
    bool nearFirstEdge = true;
    if (!GetTaskbarGeometry(&taskbarRect, &hostRect, &horizontal,
                            &nearFirstEdge)) {
        flyout.ShowAt(target);
        return;
    }

    constexpr float kGap = 12.0f;
    wf::Point point{
        static_cast<float>(target.ActualWidth() / 2.0),
        static_cast<float>(target.ActualHeight() / 2.0)};
    wucp::FlyoutPlacementMode placement = wucp::FlyoutPlacementMode::Auto;

    if (horizontal) {
        if (nearFirstEdge) {
            point.Y = static_cast<float>(target.ActualHeight()) + kGap;
            placement = wucp::FlyoutPlacementMode::Bottom;
        } else {
            point.Y = -kGap;
            placement = wucp::FlyoutPlacementMode::Top;
        }
    } else {
        if (nearFirstEdge) {
            point.X = static_cast<float>(target.ActualWidth()) + kGap;
            placement = wucp::FlyoutPlacementMode::Right;
        } else {
            point.X = -kGap;
            placement = wucp::FlyoutPlacementMode::Left;
        }
    }

    wucp::FlyoutShowOptions options;
    options.Position(winrt::box_value(point).as<wf::IReference<wf::Point>>());
    options.Placement(placement);
    options.ShowMode(wucp::FlyoutShowMode::Standard);
    // Position the measured native presenter, rather than relying on ShowAt's
    // anchor, whose final placement includes framework offsets and clamping.
    auto weakRoot = winrt::make_weak(target.XamlRoot());
    auto weakTarget = winrt::make_weak(target);
    {
        auto eventSource = flyout;
        auto eventToken = eventSource.Opened([weakRoot, weakTarget](auto const& sender, auto const&) {
        if (g_unloading) return;
        try {
            auto root = weakRoot.get();
            auto target = weakTarget.get();
            if (root && target)
                PositionTrayMenuPopup(root, sender.template as<wuc::MenuFlyout>(), target);
        } catch (...) {
            Wh_Log(L"Menu placement failed: 0x%08X", winrt::to_hresult());
        }
    });
        g_contextMenuEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Opened(eventToken);
        });
    }
    flyout.ShowAt(target, options);
}

static bool ShowWinUiTrayContextMenu(wux::FrameworkElement const& target,
                                     ButtonKind kind) {
    if (!target) {
        return false;
    }

    try {
        // Menu event handlers belong to the current flyout. Revoke the old
        // set before replacing it so repeated right-clicks do not retain
        // delegates for every menu previously opened this session.
        if (g_activeTrayContextFlyout) g_activeTrayContextFlyout.Hide();
        g_activeTrayContextFlyout = nullptr;
        RevokeEvents(g_contextMenuEventRevokers);
        auto flyout = wuc::MenuFlyout();
        if (kind == ButtonKind::Bluetooth) {
            AppendWinUiBluetoothContextMenu(flyout);
        } else if (kind == ButtonKind::Sound) {
            AppendWinUiSoundContextMenu(flyout);
        } else if (kind == ButtonKind::Network) {
            AppendWinUiNetworkContextMenu(flyout);
        } else if (kind == ButtonKind::QuickSettings) {
            AppendWinUiQuickSettingsContextMenu(flyout);
        } else if (kind == ButtonKind::Battery) {
            AppendWinUiBatteryContextMenu(flyout);
        }

        g_activeTrayContextFlyout = flyout;
        ShowWinUiFlyoutNearTaskbar(flyout, target);
        return true;
    } catch (...) {
        Wh_Log(L"WinUI context menu failed for %s: 0x%08X",
               ButtonKindName(kind), winrt::to_hresult());
        return false;
    }
}

static void ShowTrayContextMenu(wux::FrameworkElement const& target,
                                ButtonKind kind) {
    ShowWinUiTrayContextMenu(target, kind);
}

static bool TryParseButtonKind(std::wstring const& rawToken,
                               ButtonKind* kind) {
    std::wstring token = ToLower(Trim(rawToken));
    if (token == L"battery") {
        *kind = ButtonKind::Battery;
        return true;
    }
    if (token == L"bluetooth" || token == L"bt") {
        *kind = ButtonKind::Bluetooth;
        return true;
    }
    if (token == L"network" || token == L"wifi" || token == L"wi-fi") {
        *kind = ButtonKind::Network;
        return true;
    }
    if (token == L"sound" || token == L"audio" || token == L"volume") {
        *kind = ButtonKind::Sound;
        return true;
    }
    if (token == L"controlcenter" || token == L"quick_settings" || token == L"quicksettings" ||
        token == L"control_center" ||
        token == L"grouped") {
        *kind = ButtonKind::QuickSettings;
        return true;
    }
    return false;
}

static bool IsButtonKindVisible(ButtonKind kind) {
    switch (kind) {
        case ButtonKind::Bluetooth:
            return g_settings.showBluetoothButton;
        case ButtonKind::Network:
            return g_settings.showNetworkButton;
        case ButtonKind::Sound:
            return g_settings.showSoundButton;
        case ButtonKind::QuickSettings:
            return g_settings.showControlCenterButton;
        case ButtonKind::Battery:
            return g_settings.showBatteryButton;
        default:
            return false;
    }
}

static void AppendOrderedButtonKind(std::vector<ButtonKind>& order,
                                    bool used[],
                                    ButtonKind kind) {
    const size_t index = static_cast<size_t>(kind);
    if (index >= 5 || used[index] || !IsButtonKindVisible(kind)) {
        return;
    }

    used[index] = true;
    order.push_back(kind);
}

static std::vector<ButtonKind> GetVisibleButtonOrder() {
    std::vector<ButtonKind> order;
    bool used[5]{};

    size_t start = 0;
    while (start <= g_settings.buttonOrder.size()) {
        size_t comma = g_settings.buttonOrder.find(L',', start);
        std::wstring token = g_settings.buttonOrder.substr(
            start, comma == std::wstring::npos ? std::wstring::npos
                                               : comma - start);

        ButtonKind kind{};
        if (TryParseButtonKind(token, &kind)) {
            AppendOrderedButtonKind(order, used, kind);
        } else if (!Trim(token).empty()) {
            Wh_Log(L"Ignoring unknown buttonOrder entry: [%s].",
                   Trim(token).c_str());
        }

        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }

    std::wstring orderLog;
    for (auto kind : order) {
        if (!orderLog.empty()) {
            orderLog += L",";
        }
        orderLog += ButtonKindName(kind);
    }
    Wh_Log(L"Resolved visible tray button order: %s", orderLog.c_str());

    return order;
}

static bool SoundUsesQuickSettings() {
    // The native output picker needs the same tracked close path as Quick
    // Settings. Ctrl+Win+V itself only opens the picker on recent Windows
    // builds; it doesn't reliably toggle it closed on a second invocation.
    return _wcsicmp(g_settings.soundClickAction.c_str(), L"quick_settings") == 0 ||
           _wcsicmp(g_settings.soundClickAction.c_str(), L"sound_output") == 0;
}

static std::atomic<HWND> g_openedTrayFlyout[5]{};
static std::atomic<ULONGLONG> g_openedTrayFlyoutTick[5]{};
static std::atomic<ULONGLONG> g_trayFlyoutFocusHandoffUntil[5]{};

static bool IsTaskbarForeground(HWND hwnd) {
    wchar_t className[64]{};
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId() &&
           GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
           (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

static void CALLBACK TrayFlyoutForegroundChanged(HWINEVENTHOOK, DWORD event,
                                                 HWND hwnd, LONG, LONG,
                                                 DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || g_unloading) return;
    // Out-of-context events can be delivered after a newer focus transition.
    if (hwnd != GetForegroundWindow()) return;
    for (size_t i = 0; i < ARRAYSIZE(g_openedTrayFlyout); ++i) {
        const HWND recorded = g_openedTrayFlyout[i].load();
        if (recorded && recorded != hwnd) {
            if (IsTaskbarForeground(hwnd)) {
                // Pointer activation precedes Tapped. Preserve this one click
                // even if Shell auto-dismisses its flyout while focusing tray.
                g_trayFlyoutFocusHandoffUntil[i] = GetTickCount64() + 500;
                continue;
            }
            g_openedTrayFlyout[i] = nullptr;
            g_openedTrayFlyoutTick[i] = 0;
            g_trayFlyoutFocusHandoffUntil[i] = 0;
        }
    }
}

static bool IsShellHostedWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    DWORD processId{};
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE process = processId ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId) : nullptr;
    if (!process) return false;
    wchar_t path[MAX_PATH]{};
    DWORD length = ARRAYSIZE(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    PCWSTR name = ok ? wcsrchr(path, L'\\') : nullptr;
    name = name ? name + 1 : path;
    if (!ok) return false;
    if (_wcsicmp(name, L"explorer.exe") == 0) {
        // Explorer also owns Run, properties sheets and other shell dialogs.
        // Only its dedicated Control Center window is a toggle target.
        wchar_t className[128]{};
        return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
               _wcsicmp(className, L"ControlCenterWindow") == 0;
    }
    return _wcsicmp(name, L"ShellHost.exe") == 0 ||
           _wcsicmp(name, L"ShellExperienceHost.exe") == 0;
}

static bool IsRecordedTrayFlyout(HWND hwnd) {
    wchar_t className[128]{};
    if (!hwnd || !IsShellHostedWindow(hwnd) ||
        !GetClassNameW(hwnd, className, ARRAYSIZE(className))) return false;
    // A flyout can have different shell window classes between builds (some
    // are Explorer-hosted), but it is never a taskbar, desktop or folder.
    return _wcsicmp(className, L"Shell_TrayWnd") != 0 &&
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0 &&
           _wcsicmp(className, L"CabinetWClass") != 0 &&
           _wcsicmp(className, L"Progman") != 0 &&
           _wcsicmp(className, L"WorkerW") != 0;
}

static void CaptureOpenedTrayFlyoutAsync(ButtonKind kind) {
    const size_t index = static_cast<size_t>(kind);
    g_openedTrayFlyout[index] = nullptr;
    g_trayFlyoutFocusHandoffUntil[index] = 0;
    HANDLE thread = StartOwnedWorker([kind, index] {
        // The URI action creates the flyout asynchronously. Record only the
        // foreground shell window created by this click; never scan arbitrary
        // windows later when toggling it closed. Some builds create the sound
        // output picker noticeably later than Control Center.
        for (int i = 0; i < 40 && !g_unloading; ++i) {
            Sleep(25);
            HWND hwnd = GetForegroundWindow();
            if (IsRecordedTrayFlyout(hwnd)) {
                g_openedTrayFlyout[index] = hwnd;
                g_openedTrayFlyoutTick[index] = GetTickCount64();
                return;
            }
        }
    });
    if (thread) CloseHandle(thread);
}

static bool HandleTrayButtonClick(ButtonKind kind) {
    if (kind == ButtonKind::Battery && g_settings.batteryCustomAction) {
        ExecuteAction(g_settings.batteryAction);
        return true;
    }

    const size_t index = static_cast<size_t>(kind);
    const bool canToggle = kind != ButtonKind::Sound || SoundUsesQuickSettings();
    const HWND opened = g_openedTrayFlyout[index].load();
    if (canToggle && g_openedTrayFlyoutTick[index].load()) {
        const HWND foreground = GetForegroundWindow();
        const ULONGLONG handoffUntil = g_trayFlyoutFocusHandoffUntil[index].load();
        // Tapped can also precede delivery of the foreground notification.
        const bool trayHandoff = IsTaskbarForeground(foreground) &&
            (!handoffUntil || GetTickCount64() <= handoffUntil);
        if ((foreground == opened || trayHandoff) && IsRecordedTrayFlyout(opened)) {
            PostMessageW(opened, WM_CLOSE, 0, 0);
            g_openedTrayFlyout[index] = nullptr;
            g_openedTrayFlyoutTick[index] = 0;
            g_trayFlyoutFocusHandoffUntil[index] = 0;
            return true;
        }
        // A hidden/destroyed recorded flyout means it was dismissed outside
        // this button. Open a fresh one; never send Escape to another window.
        g_openedTrayFlyout[index] = nullptr;
        g_openedTrayFlyoutTick[index] = 0;
    }

    // Start capture after shell activation returns, so a slow cold launch
    // doesn't consume the flyout detection window before anything appears.
    std::function<void()> launched;
    if (canToggle) launched = [kind] { CaptureOpenedTrayFlyoutAsync(kind); };
    if (kind == ButtonKind::Bluetooth) {
        OpenBluetooth(std::move(launched));
    } else if (kind == ButtonKind::Network) {
        OpenNetwork(std::move(launched));
    } else if (kind == ButtonKind::QuickSettings) {
        ExecuteAction(g_settings.controlCenterAction, std::move(launched));
    } else if (kind == ButtonKind::Battery) {
        ExecuteAction(g_settings.batteryAction, std::move(launched));
    } else {
        OpenSound(std::move(launched));
    }

    return true;
}

// PointerPressed is handled for a middle click, but the private tray control
// can still raise Tapped afterwards. Keep a short per-button suppression
// window so middle-click never falls through to the normal click action.
static ULONGLONG g_suppressTapUntil[5]{};

static size_t ButtonKindIndex(ButtonKind kind) {
    return static_cast<size_t>(kind);
}

static void SuppressMiddleClickTap(ButtonKind kind) {
    g_suppressTapUntil[ButtonKindIndex(kind)] = GetTickCount64() + 750;
}

static bool ConsumeSuppressedTap(ButtonKind kind) {
    auto& until = g_suppressTapUntil[ButtonKindIndex(kind)];
    if (until && GetTickCount64() <= until) {
        until = 0;
        return true;
    }
    return false;
}

static wuc::FontIcon CreateTrayFontIcon(PCWSTR glyph, wuxm::Brush const& brush) {
    wuc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(16);
    icon.Glyph(glyph);
    icon.Foreground(brush);
    icon.HorizontalAlignment(wux::HorizontalAlignment::Center);
    icon.VerticalAlignment(wux::VerticalAlignment::Center);
    return icon;
}

static IconLayers CreateTrayIconLayers(PCWSTR primaryGlyph) {
    IconLayers layers;

    wuc::Grid host;
    host.Name(L"SeparateTrayIconLayers");
    host.HorizontalAlignment(wux::HorizontalAlignment::Center);
    host.VerticalAlignment(wux::VerticalAlignment::Center);
    host.Width(16);
    host.Height(16);

    layers.host = host;
    layers.underlay = CreateTrayFontIcon(L"", MakeUnderlayBrush());
    layers.underlay.Opacity(0.2);
    layers.primary = CreateTrayFontIcon(primaryGlyph, MakeIconBrush());
    layers.overlay = CreateTrayFontIcon(L"", MakeIconBrush());
    layers.underlay.Name(L"SeparateTrayIconUnderlay");
    layers.primary.Name(L"SeparateTrayIconPrimary");
    layers.overlay.Name(L"SeparateTrayIconOverlay");

    layers.underlay.Visibility(wux::Visibility::Collapsed);
    layers.overlay.Visibility(wux::Visibility::Collapsed);

    host.Children().Append(layers.underlay);
    host.Children().Append(layers.primary);
    host.Children().Append(layers.overlay);

    return layers;
}

static wux::FrameworkElement TryCreateNativeOmniButton(PCWSTR name) {
    try {
        auto loaded = wuxmk::XamlReader::Load(
            LR"(<SystemTray:OmniButton xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" xmlns:SystemTray="using:SystemTray" MinWidth="0" HorizontalAlignment="Center" VerticalAlignment="Center" HorizontalContentAlignment="Center" VerticalContentAlignment="Center"/>)");
        auto element = loaded.try_as<wux::FrameworkElement>();
        if (!element) {
            Wh_Log(L"XamlReader loaded object, but it is not FrameworkElement: %s.",
                   winrt::get_class_name(loaded).c_str());
            return nullptr;
        }

        element.Name(name);

        // XamlReader can construct the private SystemTray.OmniButton type, but
        // does not automatically give it the taskbar instance's style. Use the
        // live ControlCenterButton style first: it is the matching control
        // type and owns the BackgroundBorder/hover template that Taskbar
        // Styler can target.
        if (auto control = element.try_as<wuc::Control>()) {
            bool styleApplied = false;
            if (g_nativeGroupedButtonStyle) {
                try {
                    control.Style(g_nativeGroupedButtonStyle);
                    styleApplied = true;
                    Wh_Log(L"Applied the live ControlCenterButton style to OmniButton#%s.",
                           name);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"ControlCenterButton style is incompatible with OmniButton#%s: 0x%08X %s.",
                           name, e.code(), e.message().c_str());
                } catch (...) {
                    Wh_Log(L"ControlCenterButton style is incompatible with OmniButton#%s: 0x%08X.",
                           name, winrt::to_hresult());
                }
            }
            if (!styleApplied && g_nativeNotifyIconStyle) {
                try {
                    control.Style(g_nativeNotifyIconStyle);
                    styleApplied = true;
                    Wh_Log(L"Applied the live NotifyIconView style to OmniButton#%s.",
                           name);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"NotifyIconView style is incompatible with OmniButton#%s: 0x%08X %s.",
                           name, e.code(), e.message().c_str());
                } catch (...) {
                    Wh_Log(L"NotifyIconView style is incompatible with OmniButton#%s: 0x%08X.",
                           name, winrt::to_hresult());
                }
            }
            if (!styleApplied) {
                Wh_Log(L"Native OmniButton#%s was created before a compatible tray style was captured.",
                       name);
            }
        } else {
            Wh_Log(L"Native tray element %s#%s is not projected as Control; cannot apply its live style.",
                   winrt::get_class_name(element).c_str(), name);
        }

        ApplyTrayButtonMetrics(element);
        Wh_Log(L"Created native tray element through XamlReader: %s#%s",
               winrt::get_class_name(element).c_str(), element.Name().c_str());
        return element;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"XamlReader SystemTray.OmniButton creation failed: 0x%08X %s",
               e.code(), e.message().c_str());
    } catch (...) {
        Wh_Log(L"XamlReader SystemTray.OmniButton creation failed: 0x%08X",
               winrt::to_hresult());
    }

    return nullptr;
}

static wux::FrameworkElement CreateFallbackButton(PCWSTR name) {
    wuc::Button button;
    button.Name(name);
    button.HorizontalAlignment(wux::HorizontalAlignment::Center);
    button.VerticalAlignment(wux::VerticalAlignment::Center);
    button.HorizontalContentAlignment(wux::HorizontalAlignment::Center);
    button.VerticalContentAlignment(wux::VerticalAlignment::Center);

    if (g_nativeGroupedButtonStyle) {
        try {
            button.Style(g_nativeGroupedButtonStyle);
            Wh_Log(L"Applied captured grouped-button style to fallback Button#%s.",
                   name);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Applying captured grouped-button style to Button#%s failed: 0x%08X %s",
                   name, e.code(), e.message().c_str());
            button.Padding({0, 0, 0, 0});
            button.BorderThickness({0, 0, 0, 0});
            button.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        } catch (...) {
            Wh_Log(L"Applying captured grouped-button style to Button#%s failed: 0x%08X",
                   name, winrt::to_hresult());
            button.Padding({0, 0, 0, 0});
            button.BorderThickness({0, 0, 0, 0});
            button.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        }
    } else {
        button.Padding({0, 0, 0, 0});
        button.BorderThickness({0, 0, 0, 0});
        button.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        Wh_Log(L"No captured grouped-button style; using plain fallback Button#%s.",
               name);
    }

    ApplyTrayButtonMetrics(button);
    return button;
}

static bool SetElementIcon(wux::FrameworkElement const& element,
                           wux::UIElement const& icon) {
    if (auto contentControl = element.try_as<wuc::ContentControl>()) {
        contentControl.Content(icon);
        Wh_Log(L"Set icon through ContentControl on %s#%s.",
               winrt::get_class_name(element).c_str(), element.Name().c_str());
        return true;
    }

    if (auto itemsControl = element.try_as<wuc::ItemsControl>()) {
        itemsControl.Items().Append(icon);
        Wh_Log(L"Set icon through ItemsControl.Items on %s#%s.",
               winrt::get_class_name(element).c_str(), element.Name().c_str());
        return true;
    }

    Wh_Log(L"Cannot set icon content on %s#%s: not ContentControl.",
           winrt::get_class_name(element).c_str(), element.Name().c_str());
    return false;
}

// OmniButton's stock template is designed around the grouped Quick Settings
// content: an ItemsPresenter containing a horizontal stack of several entries.
// A button made with XamlReader gets the same template, but its generated item
// host keeps the grouped layout's left bias. Centre the *template's item host*,
// leaving the OmniButton, its BackgroundBorder, and all of its native visual
// states untouched.
static void CenterNativeOmniButtonItemHost(wux::FrameworkElement const& button) {
    try {
        std::vector<wux::DependencyObject> stack;
        stack.push_back(button);
        int centeredHosts = 0;

        while (!stack.empty()) {
            auto current = stack.back();
            stack.pop_back();

            auto element = current.try_as<wux::FrameworkElement>();
            if (element && current != button) {
                const auto className = winrt::get_class_name(current);
                if (_wcsicmp(className.c_str(),
                             L"Windows.UI.Xaml.Controls.ItemsPresenter") == 0 ||
                    _wcsicmp(className.c_str(),
                             L"Windows.UI.Xaml.Controls.StackPanel") == 0) {
                    element.HorizontalAlignment(wux::HorizontalAlignment::Center);
                    element.VerticalAlignment(wux::VerticalAlignment::Center);
                    ++centeredHosts;
                }

                if (auto content = current.try_as<wuc::ContentControl>()) {
                    content.HorizontalContentAlignment(wux::HorizontalAlignment::Center);
                    content.VerticalContentAlignment(wux::VerticalAlignment::Center);
                }
            }

            const int count = wuxm::VisualTreeHelper::GetChildrenCount(current);
            for (int i = 0; i < count; ++i) {
                auto child = wuxm::VisualTreeHelper::GetChild(current, i);
                if (child) {
                    stack.push_back(child);
                }
            }
        }

        Wh_Log(L"Centered %d native OmniButton item-host element(s) for %s#%s.",
               centeredHosts, winrt::get_class_name(button).c_str(),
               button.Name().c_str());
    } catch (...) {
        Wh_Log(L"CenterNativeOmniButtonItemHost failed for %s#%s: 0x%08X",
               winrt::get_class_name(button).c_str(), button.Name().c_str(),
               winrt::to_hresult());
    }
}

struct TrayButtonInputState {
    int wheelRemainder = 0;
    ULONGLONG lastWheelTick = 0;
    bool enterDown = false;
    bool spaceDown = false;

    int ConsumeWheelDelta(int delta, ULONGLONG now) {
        if (now - lastWheelTick > 1500) wheelRemainder = 0;
        lastWheelTick = now;
        const long long total = static_cast<long long>(wheelRemainder) + delta;
        wheelRemainder = static_cast<int>(total % WHEEL_DELTA);
        return static_cast<int>(total / WHEEL_DELTA);
    }
};

static void AttachTrayButtonHandlers(wux::FrameworkElement const& element,
                                     ButtonKind kind) {
    if (kind == ButtonKind::Battery) {
        auto token = element.SizeChanged([](auto const&, auto const&) {
            // Reconcile the border against the final arranged size, rather
            // than leaving margins calculated from the previous orientation.
            if (!g_unloading) PostTrayRefresh(false);
        });
        g_uiEventRevokers.push_back([weak = winrt::make_weak(element), token] {
            if (auto source = weak.get()) source.SizeChanged(token);
        });
    }
    auto uiElement = element.as<wux::UIElement>();
    {
        auto token = uiElement.PointerEntered([](wf::IInspectable const& sender,
                                                 wuxi::PointerRoutedEventArgs const&) {
            if (!g_unloading) ApplyHoverBackgroundMetrics(sender.as<wux::FrameworkElement>());
        });
        g_uiEventRevokers.push_back([weak = winrt::make_weak(uiElement), token] {
            if (auto source = weak.get()) source.PointerEntered(token);
        });
    }
    auto input = std::make_shared<TrayButtonInputState>();
    if (auto control = element.try_as<wuc::Control>()) {
        control.IsTabStop(true);
        control.UseSystemFocusVisuals(true);
    }
    wuxa::AutomationProperties::SetHelpText(element,
        L"Press Enter or Space to activate. Press Shift+F10 or the Menu key for options.");

    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.KeyDown([kind, input](wf::IInspectable const& sender,
                                    wuxi::KeyRoutedEventArgs const& args) {
        if (g_unloading) return;
        using Key = winrt::Windows::System::VirtualKey;
        const auto key = args.Key();
        if (key == Key::Enter) {
            args.Handled(true);
            if (!input->enterDown) {
                input->enterDown = true;
                HandleTrayButtonClick(kind);
            }
        } else if (key == Key::Space) {
            input->spaceDown = true;
            args.Handled(true);
        } else if (key == Key::Application ||
                   (key == Key::F10 && (GetKeyState(VK_SHIFT) & 0x8000))) {
            args.Handled(true);
            ShowTrayContextMenu(sender.as<wux::FrameworkElement>(), kind);
        }
    });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.KeyDown(eventToken);
        });
    }
    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.KeyUp([kind, input](wf::IInspectable const&,
                                  wuxi::KeyRoutedEventArgs const& args) {
        if (g_unloading) return;
        using Key = winrt::Windows::System::VirtualKey;
        if (args.Key() == Key::Enter) {
            input->enterDown = false;
            args.Handled(true);
        } else if (args.Key() == Key::Space) {
            const bool activate = input->spaceDown;
            input->spaceDown = false;
            args.Handled(true);
            if (activate) HandleTrayButtonClick(kind);
        }
    });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.KeyUp(eventToken);
        });
    }
    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.LostFocus([input](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        if (g_unloading) return;
        input->enterDown = input->spaceDown = false;
    });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.LostFocus(eventToken);
        });
    }

    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.RightTapped(
        [kind](wf::IInspectable const& sender,
               wuxi::RightTappedRoutedEventArgs const& args) {
        if (g_unloading) return;
            auto element = sender.try_as<wux::FrameworkElement>();
            if (element) {
                ShowTrayContextMenu(element, kind);
            }
            args.Handled(true);
        });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.RightTapped(eventToken);
        });
    }

    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.Tapped([kind](wf::IInspectable const&,
                            wuxi::TappedRoutedEventArgs const& args) {
        if (g_unloading) return;
        if (ConsumeSuppressedTap(kind)) {
            args.Handled(true);
            return;
        }

        HandleTrayButtonClick(kind);
        args.Handled(true);
    });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Tapped(eventToken);
        });
    }

    {
        auto eventSource = uiElement;
        auto eventToken = eventSource.PointerPressed(
        [kind](wf::IInspectable const& sender,
               wuxi::PointerRoutedEventArgs const& args) {
        if (g_unloading) return;
            auto element = sender.try_as<wux::UIElement>();
            auto point = args.GetCurrentPoint(element);
            if (!point.Properties().IsMiddleButtonPressed()) {
                return;
            }

            SuppressMiddleClickTap(kind);
            if (kind == ButtonKind::Sound && g_settings.soundMiddleClickToggleMute) {
                ToggleDefaultEndpointMute();
            } else {
                std::wstring action;
                switch (kind) {
                    case ButtonKind::Sound: action = g_settings.soundMiddleClickAction; break;
                    case ButtonKind::Bluetooth: action = g_settings.bluetoothMiddleClickAction; break;
                    case ButtonKind::Network: action = g_settings.networkMiddleClickAction; break;
                    case ButtonKind::ControlCenter: action = g_settings.controlCenterMiddleClickAction; break;
                    case ButtonKind::Battery: action = g_settings.batteryMiddleClickAction; break;
                }
                if (!action.empty()) ExecuteAction(action);
            }
            args.Handled(true);
        });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.PointerPressed(eventToken);
        });
    }

    if (kind == ButtonKind::Sound) {
        {
        auto eventSource = uiElement;
        auto eventToken = eventSource.PointerWheelChanged(
            [input](wf::IInspectable const& sender,
               wuxi::PointerRoutedEventArgs const& args) {
        if (g_unloading) return;
                auto element = sender.try_as<wux::UIElement>();
                auto point = args.GetCurrentPoint(element);
                if (point.Properties().IsHorizontalMouseWheel()) return;
                int delta = point.Properties().MouseWheelDelta();
                if (delta != 0) {
                    const int steps = input->ConsumeWheelDelta(delta, GetTickCount64());
                    if (steps) {
                        StepDefaultEndpointVolume(steps);
                    }
                    args.Handled(true);
                }
            });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.PointerWheelChanged(eventToken);
        });
    }
    }
}

static wux::FrameworkElement CreateTrayButton(ButtonKind kind,
                                              PCWSTR glyph,
                                              PCWSTR name,
                                              PCWSTR tooltip) {
    // OmniButton is the private tray control that accepts injected XAML
    // content on this build while keeping the native pointer-over chrome.
    auto element = TryCreateNativeOmniButton(name);
    bool nativeElement = static_cast<bool>(element);
    if (!element) {
        element = CreateFallbackButton(name);
    }

    SetTrayToolTip(element, tooltip);

    IconLayers icon = CreateTrayIconLayers(glyph);
    bool iconSet = SetElementIcon(element, icon.host);
    if (!iconSet && nativeElement) {
        if (!iconSet) {
            Wh_Log(L"Native tray element#%s has no public content/items "
                   L"surface; falling back to styled public Button.", name);
            element = CreateFallbackButton(name);
            icon = CreateTrayIconLayers(glyph);
            iconSet = SetElementIcon(element, icon.host);
        }
        SetTrayToolTip(element, tooltip);
    }

    if (kind == ButtonKind::Bluetooth) {
        g_bluetoothIcon = icon;
    } else if (kind == ButtonKind::Network) {
        g_networkIcon = icon;
    } else if (kind == ButtonKind::Sound) {
        g_soundIcon = icon;
    } else if (kind == ButtonKind::QuickSettings) {
        g_compactGroupedIcon = icon;
    } else if (kind == ButtonKind::Battery) {
        g_batteryIcon = icon;
    }

    AttachTrayButtonHandlers(element, kind);

    if (nativeElement &&
        _wcsicmp(winrt::get_class_name(element).c_str(),
                 L"SystemTray.OmniButton") == 0) {
        // The private control's visual tree exists only once it is in the live
        // taskbar. Do this on Loaded rather than offsetting the FontIcon itself.
        {
        auto eventSource = element;
        auto eventToken = eventSource.Loaded([](wf::IInspectable const& sender,
                          wux::RoutedEventArgs const&) {
        if (g_unloading) return;
            if (auto button = sender.try_as<wux::FrameworkElement>()) {
                ApplyTrayButtonMetrics(button);
                CenterNativeOmniButtonItemHost(button);
                ApplyHoverBackgroundMetrics(button);
            }
        });
        g_uiEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Loaded(eventToken);
        });
    }
    }

    return element;
}

struct OrderedTrayButtons {
    std::vector<wux::FrameworkElement> all;
};

static std::vector<ButtonKind> GetVisibleButtonOrder();
static wux::FrameworkElement ButtonElementForKind(ButtonKind kind);


static wux::FrameworkElement ButtonElementForKind(ButtonKind kind) {
    switch (kind) {
        case ButtonKind::Bluetooth:
            return g_bluetoothButton;
        case ButtonKind::Network:
            return g_networkButton;
        case ButtonKind::Sound:
            return g_soundButton;
        case ButtonKind::QuickSettings:
            return g_compactGroupedButton;
        case ButtonKind::Battery:
            return g_batteryButton;
        default:
            return nullptr;
    }
}

static OrderedTrayButtons CreateTrayButtons() {
    OrderedTrayButtons buttons;
    if (g_settings.showBatteryButton) {
        g_batteryButton = CreateTrayButton(ButtonKind::Battery, L"\xF8D0",
            L"SeparateQuickSettingsXamlBattery", L"Battery");
        auto host = g_batteryIcon.host;
        // SysBatt includes side bearings outside the visible battery outline.
        // Let the glyph measure naturally; the outer icon-only button supplies
        // the stable width without clipping its font layout box.
        host.Width(NAN);
        host.Height(NAN);
        host.RenderTransform(wuxm::TranslateTransform());
        for (auto const& glyph : {g_batteryIcon.primary, g_batteryIcon.overlay, g_batteryIcon.underlay}) {
            glyph.FontFamily(g_nativeBatteryFont ? g_nativeBatteryFont :
                wuxm::FontFamily(L"SysBatt Fluent Icons"));
            glyph.FontSize(g_settings.smallBatteryGlyph ? 12 : 16);
            glyph.Width(NAN);
            glyph.Height(NAN);
        }
        // Replace the original icon content before reparenting its grid.
        // A separate layer leaves the native outline/fill/charging layers
        // intact and stays inside the battery's existing measurement box.
        // Filled vector artwork: FontIcon foreground cannot fill the hollow
        // interior of an outline glyph. Viewbox scales all paths together.
        g_batteryLeafIcon = wuxmk::XamlReader::Load(LR"(
            <Viewbox xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                     Stretch="Uniform">
                <Canvas Width="16" Height="16">
                    <Path Name="SeparateTrayLeafBody"
                          Data="M8,1 C6,3 2,6 2,9 C2,12 4.8,14 8,14 C11.2,14 14,12 14,9 C14,6 10,3 8,1 Z"
                          Fill="#8EC22E" Stroke="Black" StrokeThickness="1"
                          StrokeLineJoin="Round"/>
                    <Path Name="SeparateTrayLeafStem" Data="M8,7 L8,15"
                          Stroke="Black" StrokeThickness="1"
                          StrokeStartLineCap="Round" StrokeEndLineCap="Round"/>
                </Canvas>
            </Viewbox>)").as<wux::FrameworkElement>();
        g_batteryLeafIcon.Name(L"SeparateTrayBatteryEnergySaverOverlay");
        g_batteryLeafIcon.Width(g_settings.smallBatteryGlyph ? 9 : 12);
        g_batteryLeafIcon.Height(g_settings.smallBatteryGlyph ? 9 : 12);
        wuxm::TranslateTransform leafOffset;
        leafOffset.X(GetBatteryLeafOffsetX());
        g_batteryLeafIcon.RenderTransform(leafOffset);
        g_batteryLeafIcon.HorizontalAlignment(wux::HorizontalAlignment::Left);
        g_batteryLeafIcon.VerticalAlignment(wux::VerticalAlignment::Bottom);
        g_batteryLeafIcon.IsHitTestVisible(false);
        g_batteryLeafIcon.Visibility(wux::Visibility::Collapsed);
        host.Children().Append(g_batteryLeafIcon);
        if (auto items = g_batteryButton.try_as<wuc::ItemsControl>()) items.Items().Clear();
        else if (auto control = g_batteryButton.try_as<wuc::ContentControl>()) control.Content(nullptr);
        wuc::StackPanel content;
        g_batteryContent = content;
        content.Name(L"SeparateTrayBatteryContent");
        content.Orientation(wuc::Orientation::Horizontal);
        content.VerticalAlignment(wux::VerticalAlignment::Center);
        content.Spacing(4);
        // Native battery content has breathing room inside its hover surface.
        content.Margin({8, 0, 8, 0});
        content.Children().Append(host);
        g_batteryPercentageText = wuc::TextBlock();
        g_batteryPercentageText.Name(L"SeparateTrayBatteryPercentage");
        // Percentage is caption text, not a 16-DIP icon glyph. Let the tray's
        // caption style supply typography; retain a 12-DIP fallback if absent.
        g_batteryPercentageText.FontSize(12);
        try {
            auto style = wux::Application::Current().Resources().TryLookup(
                winrt::box_value(L"CaptionTextBlockStyle")).try_as<wux::Style>();
            if (style) {
                g_batteryPercentageText.Style(style);
                g_batteryPercentageText.ClearValue(wuc::TextBlock::FontSizeProperty());
            }
        } catch (...) {
            // The taskbar can be created before application resources exist.
        }
        g_batteryPercentageText.VerticalAlignment(wux::VerticalAlignment::Center);
        g_batteryPercentageText.HorizontalAlignment(wux::HorizontalAlignment::Center);
        wuxm::TranslateTransform percentageOffset;
        // Keep the full inter-item gap; only a small baseline correction is
        // needed. These are XAML DIPs, scaled by the monitor's DPI.
        percentageOffset.Y(-1);
        g_batteryPercentageText.RenderTransform(percentageOffset);
        g_batteryPercentageText.Visibility(wux::Visibility::Collapsed);
        content.Children().Append(g_batteryPercentageText);
        SetElementIcon(g_batteryButton, content);
        ApplyTrayButtonMetrics(g_batteryButton);
        UpdateSeparateBatteryButton();
    }

    if (g_settings.showBluetoothButton) {
        g_bluetoothButton =
            CreateTrayButton(ButtonKind::Bluetooth, L"\xE702",
                             L"SeparateQuickSettingsXamlBluetooth",
                             L"Bluetooth");
        Wh_Log(L"Bluetooth tray button created: element=%p icon=%p.",
               winrt::get_abi(g_bluetoothButton),
               winrt::get_abi(g_bluetoothIcon.primary));
    } else {
        g_bluetoothButton = nullptr;
    }

    if (g_settings.showNetworkButton) {
        g_networkButton =
            CreateTrayButton(ButtonKind::Network, L"\xE701",
                             L"SeparateQuickSettingsXamlNetwork",
                             L"Network");
    } else {
        g_networkButton = nullptr;
    }

    if (g_settings.showSoundButton) {
        g_soundButton = CreateTrayButton(ButtonKind::Sound, L"\xE767",
                                         L"SeparateQuickSettingsXamlSound",
                                         L"Sound");
    } else {
        g_soundButton = nullptr;
    }

    if (g_settings.showControlCenterButton) {
        auto compactGlyph =
            GlyphFromHexSetting(g_settings.controlCenterGlyph, L'\xF4C3');
        g_compactGroupedButton =
            CreateTrayButton(ButtonKind::QuickSettings, compactGlyph.c_str(),
                             L"SeparateQuickSettingsXamlControlCenter",
                             L"Control Center");
    } else {
        g_compactGroupedButton = nullptr;
    }

    for (auto kind : GetVisibleButtonOrder()) {
        auto element = ButtonElementForKind(kind);
        if (element) buttons.all.push_back(element);
    }

    return buttons;
}

static bool TryInjectBesideControlCenterButton(wux::FrameworkElement const& root) {
    auto controlCenterButton = FindControlCenterButton(root);
    if (!controlCenterButton) {
        Wh_Log(L"TryInjectBesideControlCenterButton: ControlCenterButton not "
               L"found from root.");
        return false;
    }

    Wh_Log(L"TryInjectBesideControlCenterButton: ControlCenterButton found.");

    auto parentElement = FindAncestorFrameworkElement(controlCenterButton);
    if (!parentElement) {
        Wh_Log(L"TryInjectBesideControlCenterButton: parent not found.");
        return false;
    }

    if (auto parentPanel = parentElement.try_as<wuc::Panel>()) {
        RemoveInjectedControls(parentPanel);
        g_trayPanel = parentPanel;
        g_trayControlCenterButton = controlCenterButton;
        CaptureTrayButtonMetricsFromPanel(parentPanel, controlCenterButton);
        AttachTaskbarSizeRefreshHandlers(parentElement, controlCenterButton);
        ResolveNativeNetworkGlyphs();
        ResolveNativeSoundGlyphs();
        ApplyNativeBatteryFont();
        ResolveNativeBatteryGlyphs();
        HideOriginalGroupedButton(controlCenterButton);

        auto children = parentPanel.Children();
        uint32_t insertIndex = children.Size();
        uint32_t controlCenterIndex = 0;
        if (children.IndexOf(controlCenterButton.as<wux::UIElement>(),
                             controlCenterIndex)) {
            insertIndex = controlCenterIndex;
        }

        auto buttons = CreateTrayButtons();
        for (uint32_t i = 0; i < buttons.all.size(); ++i) {
            children.InsertAt(insertIndex + i,
                              buttons.all[i].as<wux::UIElement>());
        }

        UpdateDynamicXamlIcons();
        EnsureUpdateTimer();
        Wh_Log(L"TryInjectBesideControlCenterButton: injected into parent "
               L"panel %s#%s at index=%u.",
               winrt::get_class_name(parentElement).c_str(),
               parentElement.Name().c_str(), insertIndex);
        return true;
    }

    Wh_Log(L"TryInjectBesideControlCenterButton: parent is not a Panel: %s#%s",
           winrt::get_class_name(parentElement).c_str(),
           parentElement.Name().c_str());
    return false;
}

static void InsertGridTrayButtons(wuc::Grid const& trayGrid,
                                  std::vector<wux::FrameworkElement> const& buttons,
                                  int insertCol) {
    const int buttonCount = static_cast<int>(buttons.size());
    if (!trayGrid || buttonCount <= 0) {
        return;
    }

    RestoreGridTrayMutation();
    g_gridTrayMutation.emplace();
    g_gridTrayMutation->grid = winrt::make_weak(trayGrid);
    for (int i = 0; i < buttonCount; ++i) {
        wuc::ColumnDefinition column;
        column.Width({1.0, wux::GridUnitType::Auto});
        if (insertCol + i >=
            static_cast<int>(trayGrid.ColumnDefinitions().Size())) {
            trayGrid.ColumnDefinitions().Append(column);
        } else {
            trayGrid.ColumnDefinitions().InsertAt(insertCol + i, column);
        }
        g_gridTrayMutation->columns.push_back(column);
    }

    for (uint32_t i = 0; i < trayGrid.Children().Size(); ++i) {
        auto child =
            trayGrid.Children().GetAt(i).try_as<wux::FrameworkElement>();
        if (!child || IsInjectedElement(child)) {
            continue;
        }

        int childCol = wuc::Grid::GetColumn(child);
        if (childCol >= insertCol) {
            g_gridTrayMutation->shiftedChildren.emplace_back(
                winrt::make_weak(child), childCol);
            wuc::Grid::SetColumn(child, childCol + buttonCount);
        }
    }

    for (uint32_t i = 0; i < buttons.size(); ++i) {
        wuc::Grid::SetColumn(buttons[i], insertCol + static_cast<int>(i));
        trayGrid.Children().Append(buttons[i]);
    }

    Wh_Log(L"Inserted %d separated tray button(s) at SystemTrayFrameGrid column %d.",
           buttonCount, insertCol);
}

static bool ApplyXamlButtons() {
    if (g_unloading) {
        return false;
    }

    HWND taskbarWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        Wh_Log(L"ApplyXamlButtons: taskbar window not found.");
        return false;
    }
    g_taskbarWnd = taskbarWnd;
    Wh_Log(L"ApplyXamlButtons: taskbar window=%p", taskbarWnd);

    auto xamlRoot = GetTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"ApplyXamlButtons: XamlRoot not found.");
        return false;
    }
    Wh_Log(L"ApplyXamlButtons: XamlRoot found.");

    auto root = xamlRoot.Content().try_as<wux::FrameworkElement>();
    if (!root) {
        Wh_Log(L"ApplyXamlButtons: root FrameworkElement not found.");
        return false;
    }
    Wh_Log(L"ApplyXamlButtons: root=%s#%s",
           winrt::get_class_name(root).c_str(), root.Name().c_str());

    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    auto trayGrid = trayFrame.try_as<wuc::Grid>();
    if (!trayGrid) {
        Wh_Log(L"ApplyXamlButtons: SystemTrayFrameGrid not found.");
        if (!g_dumpedTree) {
            g_dumpedTree = true;
            Wh_Log(L"ApplyXamlButtons: dumping XAML tree because "
                   L"SystemTrayFrameGrid was not found.");
            DumpXamlTree(root, 0, 7);
        }
        if (TryInjectBesideControlCenterButton(root)) {
            return true;
        }
        return false;
    }
    Wh_Log(L"ApplyXamlButtons: SystemTrayFrameGrid found.");

    RemoveInjectedControls(trayGrid);

    auto controlCenterButton = FindControlCenterButton(trayGrid);
    if (controlCenterButton) {
        Wh_Log(L"ApplyXamlButtons: ControlCenterButton found, column=%d.",
               wuc::Grid::GetColumn(controlCenterButton));
    } else {
        Wh_Log(L"ApplyXamlButtons: ControlCenterButton not found; injecting at "
               L"end of SystemTrayFrameGrid.");
    }

    CaptureTrayButtonMetricsFromPanel(trayGrid, controlCenterButton);
    g_trayPanel = trayGrid;
    g_trayControlCenterButton = controlCenterButton;
    AttachTaskbarSizeRefreshHandlers(trayGrid, controlCenterButton);
    ResolveNativeNetworkGlyphs();
    ResolveNativeSoundGlyphs();
    ApplyNativeBatteryFont();
    ResolveNativeBatteryGlyphs();
    HideOriginalGroupedButton(controlCenterButton);

    int insertCol = static_cast<int>(trayGrid.ColumnDefinitions().Size());
    if (controlCenterButton) {
        insertCol = wuc::Grid::GetColumn(controlCenterButton);
        if (insertCol < 0) {
            insertCol = static_cast<int>(trayGrid.ColumnDefinitions().Size());
        }
    }

    auto buttons = CreateTrayButtons();
    InsertGridTrayButtons(trayGrid, buttons.all, insertCol);

    EnsureUpdateTimer();

    UpdateDynamicXamlIcons();
    Wh_Log(L"ApplyXamlButtons: injected native XAML tray buttons.");
    return true;
}

static void ApplyXamlButtonsWithRetry() {
    if (g_unloading) {
        return;
    }

    if (ApplyXamlButtons()) {
        g_retryCount = 0;
        if (g_retryTimer) {
            g_retryTimer.Stop();
            g_retryTimer = nullptr;
        }
        return;
    }

    if (++g_retryCount > 50) {
        Wh_Log(L"ApplyXamlButtonsWithRetry: giving up after %d attempts.",
               g_retryCount);
        if (g_retryTimer) {
            g_retryTimer.Stop();
            g_retryTimer = nullptr;
        }
        return;
    }

    if (!g_retryTimer) {
        g_retryTimer = wux::DispatcherTimer();
        g_retryTimer.Interval(std::chrono::milliseconds(100));
        {
        auto eventSource = g_retryTimer;
        auto eventToken = eventSource.Tick([](wf::IInspectable const&,
                             wf::IInspectable const&) {
        if (g_unloading) return;
            ApplyXamlButtonsWithRetry();
        });
        g_timerEventRevokers.push_back([weakSource = winrt::make_weak(eventSource), eventToken] {
            if (auto source = weakSource.get()) source.Tick(eventToken);
        });
    }
        g_retryTimer.Start();
        Wh_Log(L"ApplyXamlButtonsWithRetry: retry timer started.");
    }
}

static void RemoveXamlButtons() {
    // Settings reapplication rebuilds XAML only. Its status subscriptions are
    // independent of the settings and must stay alive rather than making the
    // taskbar thread wait for an in-flight RPC snapshot.
    if (g_unloading) {
        try { DestroyTrayRefreshWindow(); }
        catch (...) { Wh_Log(L"DestroyTrayRefreshWindow error: 0x%08X", winrt::to_hresult()); }
    }
    try {
        RevokeEvents(g_timerEventRevokers);
        RevokeEvents(g_contextMenuEventRevokers);
        RevokeEvents(g_uiEventRevokers);
    }
    catch (...) { Wh_Log(L"Event revocation error: 0x%08X", winrt::to_hresult()); }
    try { if (g_activeTrayContextFlyout) g_activeTrayContextFlyout.Hide(); } catch (...) {}
    g_activeTrayContextFlyout = nullptr;
    for (auto& query : g_btQueries) {
        if (query) {
            try { query.Cancel(); } catch (...) {}
            query = nullptr;
        }
    }
    g_btConnectedNames.clear();
    g_btConnectedCount = 0;
    g_btQueryTick = 0;
    g_btQueryRequested = false;
    g_btQueryInvalidated = true;
    try {
        if (g_updateTimer) {
            g_updateTimer.Stop();
            g_updateTimer = nullptr;
        }
        g_updateTimerFast = false;
        if (g_retryTimer) {
            g_retryTimer.Stop();
            g_retryTimer = nullptr;
        }
        if (g_metricRefreshTimer) {
            g_metricRefreshTimer.Stop();
            g_metricRefreshTimer = nullptr;
        }
        g_retryCount = 0;
        g_metricRefreshPending = false;
        if (g_sizeRefreshTrayElement) {
            g_sizeRefreshTrayElement.SizeChanged(g_sizeRefreshTrayToken);
            g_sizeRefreshTrayElement = nullptr;
            g_sizeRefreshTrayToken = {};
        }
        if (g_sizeRefreshControlCenterButton) {
            g_sizeRefreshControlCenterButton.SizeChanged(
                g_sizeRefreshControlCenterToken);
            g_sizeRefreshControlCenterButton = nullptr;
            g_sizeRefreshControlCenterToken = {};
        }

        HWND taskbarWnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
        auto xamlRoot = taskbarWnd ? GetTaskbarXamlRoot(taskbarWnd) : nullptr;
        auto root = xamlRoot ? xamlRoot.Content().try_as<wux::FrameworkElement>()
                             : nullptr;
        auto trayFrame = root ? FindChildByName(root, L"SystemTrayFrameGrid")
                              : nullptr;
        if (auto trayGrid = trayFrame.try_as<wuc::Grid>()) {
            RemoveInjectedControls(trayGrid);
        } else if (root) {
            auto controlCenterButton = FindControlCenterButton(root);
            auto parentElement = controlCenterButton
                                     ? FindAncestorFrameworkElement(
                                           controlCenterButton)
                                     : nullptr;
            if (auto parentPanel = parentElement.try_as<wuc::Panel>()) {
                RemoveInjectedControls(parentPanel);
            }
        }

        RestoreOriginalGroupedButton();
        g_bluetoothButton = nullptr;
        g_networkButton = nullptr;
        g_soundButton = nullptr;
        g_bluetoothIcon = {};
        g_networkIcon = {};
        g_soundIcon = {};
        g_originalGroupedButton = nullptr;
    } catch (...) {
        Wh_Log(L"RemoveXamlButtons error: 0x%08X", winrt::to_hresult());
    }
    // Release cached XAML references on this UI thread, not at DLL destruction.
    RestoreGridTrayMutation();
    g_underlayFallback = nullptr;
    g_bluetoothButton = g_networkButton = g_soundButton = g_batteryButton = nullptr;
    g_compactGroupedButton = g_trayControlCenterButton = g_originalGroupedButton = nullptr;
    g_bluetoothIcon = {}; g_networkIcon = {}; g_soundIcon = {};
    g_batteryIcon = {}; g_compactGroupedIcon = {};
    g_batteryLeafIcon = nullptr;
    g_batteryPercentageText = nullptr;
    g_batteryContent = nullptr;
    g_trayPanel = nullptr;
    g_nativeGroupedButtonStyle = g_nativeNotifyIconStyle = nullptr;
    g_sizeRefreshTrayElement = g_sizeRefreshControlCenterButton = nullptr;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID);
static UINT g_runFromWindowThreadRegisteredMsg = 0;

static void WINAPI ApplyXamlButtonsProc(PVOID) {
    try { ApplyXamlButtonsWithRetry(); }
    catch (...) { Wh_Log(L"ApplyXamlButtonsProc error: 0x%08X", winrt::to_hresult()); }
}

static void WINAPI RemoveXamlButtonsProc(PVOID) {
    try { RemoveXamlButtons(); }
    catch (...) { Wh_Log(L"RemoveXamlButtonsProc error: 0x%08X", winrt::to_hresult()); }
}

static void WINAPI ReloadSettingsAndReapplyProc(PVOID) {
    try { LoadSettings(); RemoveXamlButtons(); ApplyXamlButtonsWithRetry(); }
    catch (...) { Wh_Log(L"ReloadSettingsAndReapplyProc error: 0x%08X", winrt::to_hresult()); }
}

static LRESULT CALLBACK RunFromWindowThreadHookProc(int code,
                                                    WPARAM wParam,
                                                    LPARAM lParam) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == g_runFromWindowThreadRegisteredMsg) {
            struct Param {
                RunFromWindowThreadProc_t proc;
                PVOID procParam;
            };

            auto* param = reinterpret_cast<Param*>(cwp->lParam);
            param->proc(param->procParam);
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static bool RunFromWindowThread(HWND hwnd,
                                RunFromWindowThreadProc_t proc,
                                PVOID procParam) {
    if (!hwnd) {
        return false;
    }

    if (!g_runFromWindowThreadRegisteredMsg) {
        g_runFromWindowThreadRegisteredMsg =
            RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_"
                                   WH_MOD_ID);
    }

    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
                                   RunFromWindowThreadHookProc, nullptr,
                                   threadId);
    if (!hook) {
        Wh_Log(L"RunFromWindowThread: SetWindowsHookEx failed: %lu",
               GetLastError());
        return false;
    }

    struct Param {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    } param{proc, procParam};

    SendMessageW(hwnd, g_runFromWindowThreadRegisteredMsg, 0,
                 reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

static void ApplyFromTaskbarThread() {
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!g_taskbarWnd) {
        Wh_Log(L"ApplyFromTaskbarThread: Shell_TrayWnd not found.");
        return;
    }

    RunFromWindowThread(g_taskbarWnd, ApplyXamlButtonsProc, nullptr);
}

static void WINAPI TrayUI_StartTaskbar_Hook(void* self) {
    TrayUI_StartTaskbar_Original(self);
    ApplyFromTaskbarThread();
}

static bool HookTaskbarDllSymbols() {
    HMODULE taskbarModule =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarModule) {
        Wh_Log(L"HookTaskbarDllSymbols: taskbar.dll not loaded.");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original,
         TrayUI_StartTaskbar_Hook},
    };

    if (!WindhawkUtils::HookSymbols(taskbarModule, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookTaskbarDllSymbols: failed to resolve taskbar symbols.");
        return false;
    }

    Wh_Log(L"HookTaskbarDllSymbols: resolved taskbar XAML symbols.");
    return true;
}

// Sound Output Master Volume, by Asteski, imported from version 0.2.6.
// Distributed under GPL-3.0. The glyph size is fixed at 16 pixels.
namespace SoundOutputMasterVolume {
namespace xaml = winrt::Windows::UI::Xaml;
namespace controls = xaml::Controls;
namespace media = xaml::Media;
namespace automation = xaml::Automation;
namespace foundation = winrt::Windows::Foundation;
using winrt::Windows::UI::Core::CoreDispatcher;
using winrt::Windows::UI::Core::CoreDispatcherPriority;

static std::atomic<bool> g_stopping = false;
static constexpr int kGlyphSize = 16;
static std::atomic<bool> g_enabled = false;
static void LoadSettings() {
    g_enabled = Wh_GetIntSetting(L"sound.showMasterVolumeInOutputPicker") != 0;
}
static HANDLE g_stopEvent;
static HANDLE g_attachThread;
static wchar_t g_statusKey[MAX_PATH];
static void Status(PCWSTR message) {
    Wh_SetStringValue(g_statusKey, message);
    Wh_Log(L"%s", message);
}

// All XAML work, including Core Audio access, stays on its owning UI thread.
// Poll only while the flyout row is loaded. Resolving the default endpoint on
// every operation also prevents a device switch from leaving stale bindings.
static winrt::com_ptr<IAudioEndpointVolume> DefaultVolume() {
    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    winrt::check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), enumerator.put_void()));
    winrt::com_ptr<IMMDevice> device;
    winrt::check_hresult(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                            device.put()));
    winrt::com_ptr<IAudioEndpointVolume> volume;
    winrt::check_hresult(device->Activate(__uuidof(IAudioEndpointVolume),
                                         CLSCTX_INPROC_SERVER, nullptr,
                                         volume.put_void()));
    return volume;
}

struct MasterRow : std::enable_shared_from_this<MasterRow> {
    controls::Grid row{nullptr}, wrapper{nullptr};
    controls::Panel parent{nullptr};
    xaml::FrameworkElement list{nullptr};
    controls::Button mute{nullptr};
    controls::FontIcon glyph{nullptr};
    controls::Slider slider{nullptr};
    xaml::DispatcherTimer timer{nullptr};
    winrt::event_token clicked{}, changed{}, loaded{}, unloaded{}, tick{};
    winrt::event_token entered{}, exited{}, themeChanged{};
    int oldRow{}, oldColumn{}, oldRowSpan{}, oldColumnSpan{};
    bool updating = false;
    bool hasMuteState = false, lastMuted = false;
    bool hovering = false;

    void UpdateHighlight() noexcept {
        try {
            auto color = winrt::Windows::UI::Colors::Transparent();
            if (hovering) {
                color = mute.ActualTheme() == xaml::ElementTheme::Light
                    ? winrt::Windows::UI::Color{255, 233, 233, 233}
                    : winrt::Windows::UI::Color{255, 49, 49, 49};
            }
            mute.Background(media::SolidColorBrush(color));
        } catch (...) { Wh_Log(L"Hover update failed: %08X", winrt::to_hresult()); }
    }

    void Refresh() noexcept {
        try {
            const int size = kGlyphSize;
            if (glyph.FontSize() != size) glyph.FontSize(size);
            auto volume = DefaultVolume();
            float level;
            BOOL muted;
            winrt::check_hresult(volume->GetMasterVolumeLevelScalar(&level));
            winrt::check_hresult(volume->GetMute(&muted));
            updating = true;
            slider.Value(std::round(level * 100));
            updating = false;
            slider.IsEnabled(true);
            mute.IsEnabled(true);
            glyph.Glyph(muted ? L"\xE74F" : level == 0 ? L"\xE992" :
                        level <= 0.33f ? L"\xE993" :
                        level <= 0.66f ? L"\xE994" : L"\xE995");
            if (!hasMuteState || lastMuted != (muted != FALSE)) {
                automation::AutomationProperties::SetName(mute,
                                                         muted ? L"Unmute" : L"Mute");
                controls::ToolTipService::SetToolTip(mute,
                    winrt::box_value(muted ? L"Unmute" : L"Mute"));
                lastMuted = muted != FALSE;
                hasMuteState = true;
            }
        } catch (...) {
            updating = false;
            slider.IsEnabled(false);
            mute.IsEnabled(false);
            glyph.Glyph(L"\xE74F");
        }
    }

    void Create(xaml::FrameworkElement const& appList, controls::Panel const& host) {
        list = appList;
        parent = host;
        row = controls::Grid();
        row.Name(L"SoundOutputMasterVolume");
        row.Height(44);
        row.Margin({0, 0, 0, 0});
        auto iconColumn = controls::ColumnDefinition();
        iconColumn.Width({40, xaml::GridUnitType::Pixel});
        row.ColumnDefinitions().Append(iconColumn);
        auto sliderColumn = controls::ColumnDefinition();
        sliderColumn.Width({1, xaml::GridUnitType::Star});
        row.ColumnDefinitions().Append(sliderColumn);
        mute = controls::Button();
        mute.Name(L"SoundOutputMasterMute");
        // Only explicit pointer handling controls this background. There are
        // no native state animations or competing theme-resource setters.
        mute.Template(xaml::Markup::XamlReader::Load(LR"(
            <ControlTemplate TargetType="Button"
                xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation">
                <Border Background="{TemplateBinding Background}" CornerRadius="4">
                    <ContentPresenter Content="{TemplateBinding Content}"
                        HorizontalAlignment="Center" VerticalAlignment="Center" />
                </Border>
            </ControlTemplate>
        )").as<controls::ControlTemplate>());
        mute.Width(40);
        mute.Height(40);
        mute.Padding({0, 0, 0, 0});
        mute.VerticalAlignment(xaml::VerticalAlignment::Center);
        // Translate the whole button, including
        // its hit target, without shifting the adjacent slider's left edge.
        mute.Background(media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
        mute.BorderBrush(media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
        mute.BorderThickness({0, 0, 0, 0});
        auto buttonOffset = media::TranslateTransform();
        buttonOffset.X(2);
        buttonOffset.Y(1);
        mute.RenderTransform(buttonOffset);
        glyph = controls::FontIcon();
        glyph.Name(L"SoundOutputMasterGlyph");
        glyph.FontFamily(media::FontFamily(L"Segoe Fluent Icons"));
        glyph.FontSize(kGlyphSize);
        mute.Content(glyph);
        slider = controls::Slider();
        slider.Name(L"SoundOutputMasterSlider");
        slider.Minimum(0);
        slider.Maximum(100);
        slider.StepFrequency(1);
        slider.SmallChange(1);
        slider.LargeChange(10);
        slider.Margin({6, 0, 16, 0});
        slider.VerticalAlignment(xaml::VerticalAlignment::Center);
        automation::AutomationProperties::SetName(slider, L"Master volume");
        controls::Grid::SetColumn(slider, 1);
        row.Children().Append(mute);
        row.Children().Append(slider);

        auto weak = weak_from_this();
        entered = mute.PointerEntered([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping) {
                self->hovering = true;
                self->UpdateHighlight();
            }
        });
        exited = mute.PointerExited([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping) {
                self->hovering = false;
                self->UpdateHighlight();
            }
        });
        themeChanged = mute.ActualThemeChanged([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping)
                self->UpdateHighlight();
        });
        clicked = mute.Click([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping) {
                try {
                    auto volume = DefaultVolume();
                    BOOL muted;
                    winrt::check_hresult(volume->GetMute(&muted));
                    winrt::check_hresult(volume->SetMute(!muted, nullptr));
                } catch (...) { Wh_Log(L"Mute failed: %08X", winrt::to_hresult()); }
                self->Refresh();
            }
        });
        changed = slider.ValueChanged([weak](auto const&, auto const& args) {
            if (auto self = weak.lock(); self && !self->updating && !g_stopping) {
                try {
                    auto volume = DefaultVolume();
                    winrt::check_hresult(volume->SetMasterVolumeLevelScalar(
                        static_cast<float>(args.NewValue() / 100), nullptr));
                    winrt::check_hresult(volume->SetMute(FALSE, nullptr));
                } catch (...) { Wh_Log(L"Volume failed: %08X", winrt::to_hresult()); }
                self->Refresh();
            }
        });
        timer = xaml::DispatcherTimer();
        timer.Interval(std::chrono::milliseconds(150));
        tick = timer.Tick([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping) self->Refresh();
        });
        loaded = row.Loaded([weak](auto const&, auto const&) {
            if (auto self = weak.lock(); self && !g_stopping) {
                self->Refresh();
                self->timer.Start();
            }
        });
        unloaded = row.Unloaded([weak](auto const&, auto const&) {
            if (auto self = weak.lock()) self->timer.Stop();
        });

        wrapper = controls::Grid();
        wrapper.Name(L"SoundOutputMasterVolumeHost");
        auto top = controls::RowDefinition();
        top.Height({1, xaml::GridUnitType::Auto});
        wrapper.RowDefinitions().Append(top);
        wrapper.RowDefinitions().Append(controls::RowDefinition());
        oldRow = controls::Grid::GetRow(list);
        oldColumn = controls::Grid::GetColumn(list);
        oldRowSpan = controls::Grid::GetRowSpan(list);
        oldColumnSpan = controls::Grid::GetColumnSpan(list);
        controls::Grid::SetRow(wrapper, oldRow);
        controls::Grid::SetColumn(wrapper, oldColumn);
        controls::Grid::SetRowSpan(wrapper, oldRowSpan);
        controls::Grid::SetColumnSpan(wrapper, oldColumnSpan);
        uint32_t index;
        if (!parent.Children().IndexOf(list, index))
            throw winrt::hresult_error(E_UNEXPECTED);
        parent.Children().RemoveAt(index);
        try {
            controls::Grid::SetRow(list, 1);
            controls::Grid::SetColumn(list, 0);
            controls::Grid::SetRowSpan(list, 1);
            controls::Grid::SetColumnSpan(list, 1);
            wrapper.Children().Append(row);
            wrapper.Children().Append(list);
            parent.Children().InsertAt(index, wrapper);
        } catch (...) {
            wrapper.Children().Clear();
            RestorePlacement();
            parent.Children().InsertAt(index, list);
            throw;
        }
        Refresh();
        Wh_Log(L"Inserted master volume above %s#%s",
               winrt::get_class_name(list).c_str(), list.Name().c_str());
        Status(L"Master volume row inserted");
    }

    void RestorePlacement() {
        controls::Grid::SetRow(list, oldRow);
        controls::Grid::SetColumn(list, oldColumn);
        controls::Grid::SetRowSpan(list, oldRowSpan);
        controls::Grid::SetColumnSpan(list, oldColumnSpan);
    }

    void Remove() noexcept {
        try {
            if (timer) { timer.Stop(); timer.Tick(tick); }
            if (mute) {
                mute.Click(clicked);
                mute.PointerEntered(entered);
                mute.PointerExited(exited);
                mute.ActualThemeChanged(themeChanged);
            }
            if (slider) slider.ValueChanged(changed);
            if (row) { row.Loaded(loaded); row.Unloaded(unloaded); }
            if (wrapper && parent && list) {
                uint32_t index;
                if (parent.Children().IndexOf(wrapper, index)) {
                    parent.Children().RemoveAt(index);
                    wrapper.Children().Clear();
                    RestorePlacement();
                    parent.Children().InsertAt(index, list);
                }
            }
        } catch (...) { Wh_Log(L"Row cleanup failed: %08X", winrt::to_hresult()); }
    }
};

struct UiState {
    DWORD threadId{};
    CoreDispatcher dispatcher{nullptr};
    std::vector<std::shared_ptr<MasterRow>> rows;
    std::vector<winrt::weak_ref<xaml::FrameworkElement>> pending;
    struct Observer {
        winrt::weak_ref<xaml::FrameworkElement> element;
        winrt::event_token loaded;
    };
    std::vector<Observer> observers;
};
static std::mutex g_uiMutex;
static std::vector<std::shared_ptr<UiState>> g_uiStates;
static std::shared_ptr<UiState> CurrentUi(xaml::FrameworkElement const& element) {
    std::lock_guard lock(g_uiMutex);
    for (auto const& state : g_uiStates)
        if (state->threadId == GetCurrentThreadId()) return state;
    auto state = std::make_shared<UiState>();
    state->threadId = GetCurrentThreadId();
    state->dispatcher = element.Dispatcher();
    g_uiStates.push_back(state);
    return state;
}

static xaml::FrameworkElement FindAppList(xaml::DependencyObject const& root) {
    // Select the items control within VolumeMixerList, never the endpoint list
    // or an individual app's visual template.
    for (int i = 0, count = media::VisualTreeHelper::GetChildrenCount(root);
         i < count; ++i) {
        auto child = media::VisualTreeHelper::GetChild(root, i);
        if (child.try_as<controls::ItemsControl>())
            return child.as<xaml::FrameworkElement>();
        if (auto found = FindAppList(child)) return found;
    }
    return nullptr;
}

static void ConsiderElement(xaml::FrameworkElement const& element) {
    if (!g_enabled || g_stopping) return;
    auto mixer = element;
    while (mixer && winrt::get_class_name(mixer) != L"ControlCenter.VolumeMixerList") {
        mixer = media::VisualTreeHelper::GetParent(mixer)
                    .try_as<xaml::FrameworkElement>();
    }
    if (!mixer) return;
    auto state = CurrentUi(mixer);
    for (auto it = state->rows.begin(); it != state->rows.end();) {
        if (!(*it)->wrapper.XamlRoot()) {
            (*it)->Remove();
            it = state->rows.erase(it);
        } else {
            ++it;
        }
    }
    for (auto const& row : state->rows) {
        if (row->wrapper.XamlRoot() == mixer.XamlRoot() &&
            row->wrapper.Parent()) {
            // One mixer per flyout XamlRoot. Detached old trees must not stop
            // a newly built flyout from getting its own row.
            auto ancestor = row->wrapper.as<xaml::DependencyObject>();
            while (ancestor) {
                if (ancestor == mixer) return;
                ancestor = media::VisualTreeHelper::GetParent(ancestor);
            }
        }
    }
    for (auto const& weak : state->pending) if (weak.get() == mixer) return;
    auto weak = winrt::make_weak(mixer);
    state->pending.push_back(weak);
    state->dispatcher.RunAsync(CoreDispatcherPriority::Low, [weak, state] {
        if (g_stopping || !g_enabled) { state->pending.clear(); return; }
        try {
            auto mixer = weak.get();
            if (mixer) {
                auto list = FindAppList(mixer);
                if (list) {
                    auto parent = media::VisualTreeHelper::GetParent(list)
                                      .try_as<controls::Panel>();
                    if (parent) {
                        auto row = std::make_shared<MasterRow>();
                        row->Create(list, parent);
                        state->rows.push_back(std::move(row));
                    } else {
                        Wh_Log(L"Mixer list parent isn't a Panel; no row inserted");
                        Status(L"Mixer list parent isn't a Panel");
                    }
                } else {
                    Status(L"Mixer loaded but app list not found");
                }
            }
        } catch (...) { Wh_Log(L"Injection failed: %08X", winrt::to_hresult()); }
        state->pending.clear();
    });
}

// IXamlType::ActivateInstance has a public COM ABI. The shell's metadata
// provider supplies the actual function used by the compiled XAML loader,
// including when WinRT activation factories aren't used for custom controls.
using ActivateType_t = HRESULT(WINAPI*)(void*, void**);
static ActivateType_t ActivateType_Original;
static HRESULT WINAPI ActivateType_Hook(void* type, void** result) {
    HRESULT hr = ActivateType_Original(type, result);
    if (FAILED(hr) || !result || !*result || g_stopping) return hr;
    try {
        foundation::IInspectable object{nullptr};
        winrt::copy_from_abi(object, *result);
        auto element = object.try_as<xaml::FrameworkElement>();
        if (element && winrt::get_class_name(element) == L"ControlCenter.VolumeMixerList") {
            auto state = CurrentUi(element);
            auto weak = winrt::make_weak(element);
            auto token = element.Loaded([weak](auto const&, auto const&) {
                if (g_stopping) return;
                try {
                    if (auto mixer = weak.get()) ConsiderElement(mixer);
                } catch (...) { Wh_Log(L"Mixer Loaded failed: %08X", winrt::to_hresult()); }
            });
            state->observers.push_back({weak, token});
            Wh_Log(L"Observed native VolumeMixerList activation");
            Status(L"Observed native VolumeMixerList activation");
        }
    } catch (...) { Wh_Log(L"Mixer activation observation failed: %08X", winrt::to_hresult()); }
    return hr;
}

static DWORD WINAPI AttachThread(void*) {
    HRESULT apartmentHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(apartmentHr)) return 0;
    while (!g_stopping) {
        auto module = GetModuleHandleW(L"ControlCenter.dll");
        if (!module) module = GetModuleHandleW(L"Windows.UI.ControlCenter.dll");
        if (module) {
            try {
                auto factoryFn = reinterpret_cast<HRESULT(WINAPI*)(void*,void**)>(
                    GetProcAddress(module, "DllGetActivationFactory"));
                if (!factoryFn) throw winrt::hresult_error(E_NOINTERFACE);
                winrt::hstring name = L"ControlCenter.XamlMetaDataProvider";
                foundation::IActivationFactory factory{nullptr};
                winrt::check_hresult(factoryFn(winrt::get_abi(name), winrt::put_abi(factory)));
                auto provider = factory.ActivateInstance<foundation::IInspectable>()
                    .as<xaml::Markup::IXamlMetadataProvider>();
                auto type = provider.GetXamlType(L"ControlCenter.VolumeMixerList");
                if (!type || !type.IsConstructible()) throw winrt::hresult_error(E_NOINTERFACE);
                // Six IInspectable methods precede twelve IXamlType getters.
                auto target = (*reinterpret_cast<void***>(winrt::get_abi(type)))[18];
                if (!Wh_SetFunctionHook(target, reinterpret_cast<void*>(ActivateType_Hook),
                                        reinterpret_cast<void**>(&ActivateType_Original)) ||
                    !Wh_ApplyHookOperations()) throw winrt::hresult_error(E_FAIL);
                Wh_Log(L"Hooked native mixer XAML activation at %p; no diagnostics connection", target);
                Status(L"Native mixer activation hooked; no diagnostics connection");
                break;
            } catch (...) {
                Wh_Log(L"Mixer activation hook unavailable: %08X", winrt::to_hresult());
                // A loaded but incompatible provider won't improve by polling.
                break;
            }
        }
        if (WaitForSingleObject(g_stopEvent, 250) != WAIT_TIMEOUT) break;
    }
    CoUninitialize();
    return 0;
}

BOOL Init() {
    LoadSettings();
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    auto name = wcsrchr(path, L'\\');
    swprintf_s(g_statusKey, L"%s.status", name ? name + 1 : path);
    Status(L"Waiting for ControlCenter module");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    return g_stopEvent != nullptr;
}

void AfterInit() {
    g_attachThread = CreateThread(nullptr, 0, AttachThread, nullptr, 0, nullptr);
    if (!g_attachThread) Wh_Log(L"Couldn't start XAML attachment thread");
}

void SettingsChanged() {
    LoadSettings();
    std::vector<std::shared_ptr<UiState>> states;
    { std::lock_guard lock(g_uiMutex); states = g_uiStates; }
    for (auto const& state : states) {
        try {
            state->dispatcher.RunAsync(CoreDispatcherPriority::Low, [state] {
                if (g_stopping) return;
                if (!g_enabled) {
                    for (auto const& row : state->rows) row->Remove();
                    state->rows.clear();
                } else {
                    for (auto const& observer : state->observers)
                        if (auto element = observer.element.get()) ConsiderElement(element);
                }
            }).get();
        } catch (...) { Wh_Log(L"Master volume settings update failed: %08X", winrt::to_hresult()); }
    }
}

void Uninit() {
    g_stopping = true;
    SetEvent(g_stopEvent);
    if (g_attachThread) {
        WaitForSingleObject(g_attachThread, INFINITE);
        CloseHandle(g_attachThread);
    }
    std::vector<std::shared_ptr<UiState>> states;
    { std::lock_guard lock(g_uiMutex); states.swap(g_uiStates); }
    for (auto const& state : states) {
        try {
            // Queue behind the observation callbacks at the same priority so
            // no queued lambda remains when Windhawk unloads the module.
            state->dispatcher.RunAsync(CoreDispatcherPriority::Low, [state] {
                for (auto const& observer : state->observers)
                    if (auto element = observer.element.get())
                        element.Loaded(observer.loaded);
                state->observers.clear();
                for (auto const& row : state->rows) row->Remove();
                state->rows.clear();
                state->pending.clear();
            }).get();
        } catch (...) { Wh_Log(L"UI thread has shut down: %08X", winrt::to_hresult()); }
    }
    CloseHandle(g_stopEvent);
}


} // namespace SoundOutputMasterVolume

static bool g_isSoundOutputHost = false;

BOOL Wh_ModInit() {
    wchar_t processPath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    const wchar_t* processName = wcsrchr(processPath, L'\\');
    processName = processName ? processName + 1 : processPath;
    g_isSoundOutputHost = _wcsicmp(processName, L"explorer.exe") != 0;
    if (g_isSoundOutputHost) return SoundOutputMasterVolume::Init();
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_Log(L"Separate Quick Settings Tray Icons XAML loaded.");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isSoundOutputHost) { SoundOutputMasterVolume::AfterInit(); return; }
    ApplyFromTaskbarThread();
}

void Wh_ModSettingsChanged() {
    if (g_isSoundOutputHost) { SoundOutputMasterVolume::SettingsChanged(); return; }
    if (HWND currentTaskbar = FindCurrentProcessTaskbarWnd())
        g_taskbarWnd = currentTaskbar;
    if (g_taskbarWnd &&
        RunFromWindowThread(g_taskbarWnd, ReloadSettingsAndReapplyProc, nullptr))
        return;

    // Explorer can briefly have no taskbar during a rebuild. Preserve the new
    // values so the next TrayUI::StartTaskbar injection uses them.
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_isSoundOutputHost) { SoundOutputMasterVolume::Uninit(); return; }
    g_unloading = true;
    // This can wait for network/audio RPC, so do it on Windhawk's unload
    // thread before any XAML cleanup is marshalled to Explorer's UI thread.
    StopStatusEvents();
    TakeAudioWork();
    if (HWND currentTaskbar = FindCurrentProcessTaskbarWnd())
        g_taskbarWnd = currentTaskbar;
    bool removed = false;
    if (g_taskbarWnd) {
        removed = RunFromWindowThread(g_taskbarWnd, RemoveXamlButtonsProc, nullptr);
    }
    if (!removed) {
        // XAML must remain on its owner thread, but native resources cannot
        // survive merely because Shell_TrayWnd is temporarily unavailable.
        HWND refresh = nullptr;
        AcquireSRWLockShared(&g_refreshLock);
        refresh = g_refreshWindow;
        ReleaseSRWLockShared(&g_refreshLock);
        if (refresh && IsWindow(refresh)) {
            removed = SendMessageW(refresh, kDestroyRefreshWindowMessage,
                                   0, 0) == 1;
        }
        if (!removed) {
            // No owner window remains to marshal through. This is best effort
            // only; thread-affine objects cannot be cleaned up here reliably.
            Wh_Log(L"Owner-thread teardown unavailable; attempting final cleanup.");
            RemoveXamlButtons();
        }
    }
    WaitForOwnedWorkers();
    if (g_batterySettingsModule) {
        FreeLibrary(g_batterySettingsModule);
        g_batterySettingsModule = nullptr;
    }
    if (g_powerProfileModule) {
        FreeLibrary(g_powerProfileModule);
        g_powerProfileModule = nullptr;
    }
}
