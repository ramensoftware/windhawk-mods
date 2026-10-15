// ==WindhawkMod==
// @id              dynamic-island-pro-for-windows
// @name            Dynamic Island Pro
// @description     Persistent Windows Dynamic Island with customizable dashboards, media controls, clipboard history, File Tray, and activity notifications.
// @version         2.0.1
// @author          Akash (Modified from Himanshu's Dynamic Island for Windows)
// @github          https://github.com/devcode90
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshcore -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lruntimeobject -lwindowscodecs -lavrt -lsetupapi -lwinhttp -lpdh -lwinmm -ldxva2
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Island Pro

Dynamic Island Pro is a Windows desktop customization mod based on Dynamic Island for Windows. It features persistent dashboard selection, a live clipboard dashboard, separate Audio and Video playback views, looping section navigation, customizable themes, and configurable dashboard ordering.

## 1. Dashboard Navigation and Appearance

- Remember the last selected dashboard section when the island collapses or the application restarts.
- Support looping navigation between enabled dashboard sections.
- Allow users to configure the order of dashboard sections.
- Provide multiple themes and maintain a consistent visual style across all dashboards.
- Include a live analog clock accent on the Time dashboard, with support for reduced-motion preferences.
- Browse previous and future months, pick a month or year, and select a date in the Calendar dashboard.

## 2. Audio and Video Playback

- Provide separate Audio and Video dashboards using Windows System Media Transport Controls (SMTC).
- Display available media information, playback status, and supported controls.
- Seek by clicking or dragging the timeline when the active media session supports position changes.
- Support video-player detection using heuristic matching.
- Allow users to add application-name or AppUserModelId fragments to the Video app identifiers setting when their player is not detected automatically.
- Display video thumbnails supplied by the media session when available.
- Send playback commands supported by the active media session.

### Audio Volume Control

- Implement a hover-expanding volume control in the Audio playback dashboard.
- Support mouse-wheel volume adjustment.
- Provide a continuously draggable volume slider for precise adjustment.
- Keep the control responsive and smoothly animated.

### Video Playback Limitations

- SMTC provides media-session information and supported playback commands, but does not expose a generic live video frame or universal Picture-in-Picture surface.
- The Video dashboard can display a supplied thumbnail, but cannot embed arbitrary live video using SMTC alone.

## 3. Clipboard Dashboard

- Maintain a configurable history of copied text and images, with a default limit of 100 entries. History is stored locally so it survives restarts.
- Pin entries to keep them in history and move them to the front; unpin an entry before clearing or removing it.
- Display clipboard entries in a responsive, paginated card grid.
- Adapt the grid layout to the available dashboard space.
- Capture clipboard updates from the time the mod starts.
- Keep the clipboard history on disk across restarts until the user clears entries; copied text and image previews remain local to the device.
- Do not import older entries from Windows Clipboard History.
- Keep history in the user's local Windhawk data directory; it is not uploaded or imported from Windows Clipboard History.
- Keep clipboard history available in the dedicated dashboard independently of temporary clipboard notifications.

## 4. File Tray

- Capture file paths copied through the Windows clipboard.
- Store file references only. Copy and Cut place the path on the clipboard with the appropriate preferred drop effect; the destination application decides whether to copy or move the original file when pasted.
- Keep File Tray references on disk across restarts until the user clears them.
- Provide Copy and Cut commands that place the selected file path on the clipboard for pasting into a destination.
- Keep File Tray behavior separate from clipboard content history.

## 5. Download and File-Copy Notifications

- Generate file-copy notifications based on file-copy operations detected through clipboard file copies.
- Monitor the Windows Downloads folder for download-related activity.
- Display notifications for supported activities when the necessary information is available.
- Do not claim to detect arbitrary file transfers initiated by Windows Shell in other folders, because those operations are outside the current monitoring scope.

## 6. Technical and Functional Requirements

- Preserve existing dashboard behavior and user settings when extending the mod.
- Keep dashboard navigation, clipboard history, media controls, File Tray, and notifications compatible with one another.
- Handle unavailable media metadata, unsupported playback commands, and undetected applications gracefully.
- Maintain responsive layouts and efficient rendering.
- Clearly distinguish implemented functionality from features requiring additional APIs or separate implementation.

## Desired Result

A customizable Windows Dynamic Island featuring persistent dashboard navigation, separate Audio and Video views, interactive volume controls, a live analog clock accent, persistent local clipboard history, File Tray file references, and notifications for supported file-copy and download activity, while respecting the technical limitations of Windows media-session APIs and the existing monitoring mechanisms.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
    - Position: top-center
      $name: Position
      $options:
        - top-center: Top center
        - top-left: Top left
        - top-right: Top right
        - bottom-center: Bottom center
        - bottom-left: Bottom left
        - bottom-right: Bottom right
    - TargetMonitor: primary
      $name: Monitor
      $options:
        - primary: Primary monitor
        - follow: Monitor under the pointer
    - SizeScale: "1.0"
      $name: Size scale
    - IslandWidth: 150
      $name: Collapsed island width
    - IslandHeight: 44
      $name: Collapsed island height
    - ExpandedWidth: 380
      $name: Expanded island width
    - ExpandedHeight: 184
      $name: Expanded island height
    - CornerRadius: 44
      $name: Island corner radius
      $description: Rounded-edge radius for the expanded island. The collapsed island keeps its shape-specific corners.
    - OffsetX: 0
      $name: Horizontal offset
    - OffsetY: 0
      $name: Vertical offset
    - SeparateExpandedOffsetY: false
      $name: Separate expanded offset
    - OffsetYExpanded: 0
      $name: Expanded vertical offset
    - AutoDpiScale: true
      $name: Automatic DPI scaling
    - ShapeStyle: pill
      $name: Island shape
      $options:
        - pill: Classic pill
        - notch: macOS notch
        - w11: Windows 11 style
    - BorderMergedMode: false
      $name: Merge island with screen border
- Animations:
    - RespectReducedMotion: true
      $name: Respect Windows reduced-motion preference
    - TargetFPS: auto
      $name: Target frame rate
      $options:
        - auto: Automatic
        - "60": 60 FPS
        - "120": 120 FPS
        - "144": 144 FPS
        - "240": 240 FPS
        - "360": 360 FPS
        - "480": 480 FPS
        - "500": 500 FPS
    - AnimationStyle: smooth
      $name: Animation style
      $options:
        - smooth: Smooth
        - default: Default
        - bouncy: Bouncy
        - snappy: Snappy
    - AnimationSpeed: normal
      $name: Animation speed
      $options:
        - very-slow: Very slow
        - slow: Slow
        - normal: Normal
        - fast: Fast
        - very-fast: Very fast
        - ultra-fast: Ultra fast
- Modules:
    - Media: true
      $name: Media playback
    - MusicVisualizer: true
      $name: Music visualizer while expanded
      $description: Click the visualizer to switch between the mirrored bars and minimal waveform.
    - VisualizerStrength: 100
      $name: Music visualizer strength
      $description: Overall visualizer strength in percent.
    - MediaAutoExpandHotkey: true
      $name: Ctrl + Alt + S toggles new-track auto-expand
    - MediaAutoExpand: false
      $name: Expand automatically for new tracks
    - MediaExpandBlocklist: ""
      $name: Apps excluded from media auto-expand
    - VideoAppIds: "vlc,netflix,prime video,primevideo,disney+,hulu,microsoft.zunevideo,movies & tv,video"
      $name: Video app identifiers
      $description: Comma-separated app names or AppUserModelId fragments treated as video players.
    - MiniVideoPlayer: true
      $name: Video playback dashboard
      $description: Show a dedicated video session dashboard using Windows media-session metadata and any thumbnail the player provides. Windows does not expose live video frames through this API.
    - Clipboard: true
      $name: Clipboard notifications
    - ClipboardSection: true
      $name: Clipboard dashboard section
    - ClipboardHistoryLimit: 100
      $name: Clipboard history items
      $description: Maximum number of copied text and image items to keep in memory.
    - StatusCountdownProgress: false
      $name: Show countdown progress on status cards
    - Battery: true
      $name: Battery status
    - Progress: true
      $name: Download and file progress
    - Volume: true
      $name: Volume activity
    - FileCopyNotifications: true
      $name: Copied file notifications
    - BrowserDownloadNotifications: true
      $name: Downloads folder notifications
    - WindowsNotifications: true
      $name: Windows notification previews
    - NotificationDuration: 4
      $name: Notification duration in seconds
      $description: Duration for temporary notification cards (1 to 30 seconds).
    - CapsLock: true
      $name: Caps Lock and Num Lock indicators
    - BluetoothIndicator: true
      $name: Bluetooth device events
    - BluetoothShowBattery: true
      $name: Bluetooth device battery
    - DoNotDisturbIndicator: true
      $name: Do Not Disturb status
    - NotificationRespectDnD: true
      $name: Respect Do Not Disturb for notifications
    - HardwareMonitorModule: true
      $name: Hardware monitor dashboard
    - CalendarModule: true
      $name: Calendar dashboard
    - FileTrayModule: true
      $name: File tray dashboard
    - FileTrayMaxItems: 12
      $name: Maximum file tray items
    - SectionOrder: "time,clipboard,calendar,hardware,filetray,quickaccess,video,audio"
      $name: Dashboard section order
      $description: 'Reorder sections by editing this comma-separated list: time, clipboard, calendar, hardware, filetray, quickaccess, video, audio.'
    - QuickAccess: true
      $name: Quick Access dashboard
    - QuickAccessLinks: "YouTube|https://www.youtube.com/,ChatGPT|https://chatgpt.com/,Amazon Music|https://music.amazon.in/,YouTube Music|https://music.youtube.com/,Spotify|https://open.spotify.com/"
      $name: Quick Access links
      $description: Up to 12 comma-separated Label|URL entries. Links open in the default browser.
    - QuickAccessBrowser: default
      $name: Browser for Quick Access links
      $options:
        - default: Default browser
        - chrome: Google Chrome
        - brave: Brave
    - ShowSeconds: false
      $name: Show seconds
    - ClockFormat: system
      $name: Clock format
      $options:
        - system: Follow Windows
        - 12h: 12-hour
        - 24h: 24-hour
    - DateFormat: ""
      $name: Custom date format
    - DateFirst: false
      $name: Show date before time
    - Language: auto
      $name: Language
      $options:
        - auto: Follow Windows
        - en: English
        - fr: French
        - es: Spanish
        - de: German
        - pt: Portuguese
        - it: Italian
        - ru: Russian
        - tr: Turkish
        - hi: Hindi
        - zh: Simplified Chinese
        - ja: Japanese
        - ko: Korean
- Behavior:
    - RememberLastSection: true
      $name: Remember last section
      $description: Restore the most recently selected dashboard section after restarting the mod.
    - AutoHideIdleSeconds: 0
      $name: Hide after idle seconds
    - AutoHideFullscreen: true
      $name: Hide in fullscreen apps
    - UnhideOnHover: true
      $name: Reveal on hover
    - AlwaysOnTop: true
      $name: Always on top
    - ExpandOnHover: true
      $name: Expand on hover
- Shortcuts:
    - HideShowHotkeyEnabled: true
      $name: Enable auto-hide toggle shortcut
    - HideShowKey: D
      $name: Auto-hide toggle key (Ctrl+Alt+key)
- Indicators:
    - PrivacyDots: true
      $name: Microphone and camera indicators
    - PrivacyDotsMic: true
      $name: Microphone indicator
    - PrivacyDotsCam: true
      $name: Camera indicator
    - PrivacyDotsMicHex: "#FF9500"
      $name: Microphone indicator color
    - PrivacyDotsCamHex: "#22C55E"
      $name: Camera indicator color
- Themes:
    - ThemePreset: obsidian
      $name: Theme
      $options:
        - obsidian: Obsidian
        - graphite: Graphite
        - slate: Slate
        - nord: Nord
        - evergreen: Evergreen
        - espresso: Espresso
        - plum: Plum
        - porcelain: Porcelain Light
        - liquid-glass: Liquid Glass
        - custom: Custom colors
    - AccentColorMode: auto
      $name: Accent color
      $options:
        - auto: Album art when available
        - system: Windows accent
        - custom: Custom accent
    - CustomAccentHex: "#4CC9F0"
      $name: Custom accent color
    - PillBgColor: "#08080A"
      $name: Custom background color
    - TextPrimaryColor: "#FFFFFF"
      $name: Primary text color
    - TextSecondaryColor: "#9B9BA5"
      $name: Secondary text color
    - ContourBorderHex: "#1E1E22"
      $name: Border color
    - ContourBorderMode: default
      $name: Border style
      $options:
        - default: Theme border
        - auto: Accent border
        - borderless: No border
    - FontFamily: ""
      $name: Font family
    - CalendarAccent: red
      $name: Calendar accent
      $options:
        - red: Red
        - system: Windows accent
    - BackdropMaterial: none
      $name: Backdrop material
      $options:
        - none: None
        - blur: Blur
        - acrylic: Acrylic
    - BackdropTint: 55
      $name: Backdrop tint
    - BackdropFillOpacity: 45
      $name: Surface opacity
    - MaterialDepth: true
      $name: Depth shading
    - DropShadow: true
      $name: Drop shadow
    - AccentBloom: 100
      $name: Accent bloom
    - TextScale: 100
      $name: Text scale
    - TintIntensity: 72
      $name: Tint intensity
    - PillOpacity: 96
      $name: Island opacity
    - ClockAccentGlow: true
      $name: Clock accent glow
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <appmodel.h>
#include <unknwn.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <setupapi.h>
#include <devpropdef.h>
#include <dbt.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shcore.h>
#include <windowsx.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <objbase.h>
#include <wrl/client.h>
#include <uiautomation.h>
#include <winhttp.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <mutex>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <set>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#if __has_include(<winrt/Windows.UI.Notifications.Management.h>) && \
    __has_include(<winrt/Windows.UI.Notifications.h>)
#define DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER 1
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.Notifications.Management.h>
#else
#define DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER 0
#endif

#if __has_include(<winrt/Windows.Devices.Enumeration.h>) && \
    __has_include(<winrt/Windows.Devices.Bluetooth.h>) && \
    __has_include(<winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>)
#define DYNAMIC_ISLAND_HAS_BLUETOOTH_WATCHER 1
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#else
#define DYNAMIC_ISLAND_HAS_BLUETOOTH_WATCHER 0
#endif

using Microsoft::WRL::ComPtr;
using namespace std::chrono_literals;

namespace {

constexpr wchar_t kWindowClass[] = L"Windhawk.DynamicIslandPro";
constexpr UINT WM_APP_LAYOUT_CHANGED = WM_APP + 0x442;
constexpr UINT WM_APP_NEW_EVENT = WM_APP + 0x443;
constexpr UINT WM_APP_MOUSE_WAKE = WM_APP + 0x446;
// RegisterHotKey cannot associate a hot key with a window created by another
// thread, and the same applies to tearing one down. LoadSettings runs on
// Windhawk's settings thread while the overlay window belongs to the render
// thread, so hotkey and backdrop changes are posted across and applied there.
constexpr UINT WM_APP_APPLY_HOTKEY = WM_APP + 0x447;
constexpr UINT WM_APP_APPLY_BACKDROP = WM_APP + 0x448;
constexpr int ID_HIDE_SHOW_HOTKEY = 1;
constexpr int ID_MEDIA_AUTOEXPAND_HOTKEY = 2;
constexpr float kRenderPadX = 28.0f;
constexpr float kRenderPadY = 22.0f;
constexpr UINT kClipboardImageThumbMaxDim = 160;  // longer-side cap for the clipboard image thumbnail

// Layout for the expanded media dashboard. DrawMedia paints every element at an
// offset from the content rect it is handed, and OverlayWndProc hit-tests in
// that same content space (see MediaContentFromClient), so both sides read the
// constants below and cannot drift apart.
namespace MediaLayout {
    constexpr float kExpandedWidth = 380.0f;
    constexpr float kExpandedHeight = 184.0f;

    constexpr float kScrubberY = 114.0f;
    constexpr float kScrubMargin = 24.0f;
    constexpr float kScrubBarLeftInset = 48.0f;
    constexpr float kScrubBarRightInset = 48.0f;

    constexpr float kScrubHitHalfHeight = 14.0f;
    constexpr float kScrubHitPadX = 6.0f;

    // Distance from the content rect's left/right edge to each end of the
    // scrubber bar. Used by the content-space hit test so the bar's clickable
    // span follows the actual pill width instead of assuming 380px.
    constexpr float kScrubInsetLeft = kScrubMargin + kScrubBarLeftInset;
    constexpr float kScrubInsetRight = kScrubMargin + kScrubBarRightInset;

    // Transport controls exactly as DrawMedia paints them: y is measured down
    // from the content rect's top edge, x as an offset from the content rect's
    // horizontal center. The hit test derives its boxes from these same values.
    constexpr float kControlsY = 148.0f;
    constexpr float kControlSpacing = 64.0f;
    constexpr float kNavButtonRadius = 16.0f;
    constexpr float kPlayButtonRadius = 22.0f;
    // Extra slack around each button so the round targets are comfortable to
    // hit. Kept small enough that prev/next never overlap play/pause:
    // prev spans -86..-42, play spans -28..+28.
    constexpr float kControlHitPad = 6.0f;

    // Expanded-layout album art, as drawn by DrawMedia.
    constexpr float kArtInsetX = 24.0f;
    constexpr float kArtInsetY = 20.0f;
    constexpr float kArtSize = 64.0f;
    constexpr float kVisualizerLeft = 112.0f;
    constexpr float kVisualizerRightCollapsedInset = 58.0f;
    constexpr float kVisualizerVolumeRevealWidth = 126.0f;
    constexpr float kVisualizerTop = 50.0f;
    constexpr float kVisualizerBottom = 81.0f;
    constexpr float kVolumeCenterY = 63.0f;

    // The expanded dashboard is faded in by DrawMedia via
    // expandedAlpha = clamp((contentHeight - 60) / 60), so anything at or below
    // this height is still the collapsed pill and must not be hit-tested.
    constexpr float kExpandedMinHeight = 60.0f;
}

namespace CalendarLayout {
    constexpr float kGridLeft = 134.0f;
    constexpr float kGridRightInset = 22.0f;
    constexpr float kHeaderTop = 13.0f;
    constexpr float kHeaderBottom = 38.0f;
    constexpr float kGridTop = 42.0f;
    constexpr float kWeekdayHeight = 18.0f;
    constexpr float kDateBottomInset = 14.0f;
    constexpr int kPickerColumns = 4;
    constexpr int kPickerRows = 3;
}

// Layout for the File Tray card (#33). Shared by DrawFileTrayDashboard and the
// row hit test in OverlayWndProc for the same reason MediaLayout is shared.
namespace FileTrayLayout {
    constexpr float kPadX = 56.0f;
    constexpr float kListTop = 14.0f;
    constexpr float kListBottomInset = 14.0f;
    constexpr float kRowHeight = 30.0f;
    constexpr float kRowGap = 6.0f;

    constexpr int VisibleRowCapacity(float contentHeight) {
        const float span = contentHeight - kListBottomInset - kListTop + kRowGap;
        const int capacity = static_cast<int>(span / (kRowHeight + kRowGap));
        return capacity < 1 ? 1 : capacity;
    }
}

// Layout for the collapsed idle strip (the clock, and optionally a weather
// reading beside it).
//
// Fixes windhawk-mods#5086. Note the tracker: plain "#33"-style references in
// this file are issues on devcode90/Dynamic-Island-for-Windows, whereas
// "windhawk-mods#NNNN" is the ramensoftware/windhawk-mods tracker that the
// published mod is submitted through. Both use bare #N in their own context, so
// the upstream ones are always qualified here.
//
// This width used to be a bare constant in ActivityForKind -- 96px, or 170px
// with weather -- which was wrong in both directions. Measured at the idle
// format's own face and size, "9:41" is 22.7px of ink in a 96px pill, so 24%
// occupancy and the rest dead air. The weather variant then split the pill
// 50/50 at its centre no matter how wide the two strings actually were.
//
// The fixed width also truncated long clocks, though only at larger type: the
// text box was a flat 84px while the idle font is 13.0f * textScale, so
// "10:41:32 PM" fits at textScale 1.0 (67.7px) but is clipped at 1.4 (94.8px)
// and 1.6 (108.4px).
//
// The strip is now measured and sized to its real content. As with
// GameOverlayLayout, the sizer (the render loop) and the painter
// (DrawIdleDashboard) both read these constants so they cannot drift apart.
namespace IdleStripLayout {
    constexpr float kPadX = 14.0f;         // inner padding at each end
    constexpr float kSlotGap = 9.0f;       // clock <-> divider <-> weather
    constexpr float kDividerWidth = 1.0f;
    constexpr float kDividerInsetY = 9.0f;

    // Room reserved on the right for the mic/camera dot. DrawPrivacyDots anchors
    // it at rect.right - 20 with a 4px radius, so 18px clears it without letting
    // the text slide under it. Previously this was stolen from the text box while
    // the pill stayed 96px, so the clock just re-centred into a narrower slot.
    constexpr float kPrivacyReserve = 18.0f;

    constexpr float kHeight = 36.0f;

    // The stadium cap is kHeight/2 at each end, so anything below ~2x the height
    // stops reading as a pill. The ceiling keeps a long localized string from
    // turning the island into a bar.
    constexpr float kMinWidth = 72.0f;
    constexpr float kMaxWidth = 280.0f;

    // Measured widths are rounded up to this so sub-pixel text metrics can't
    // resize the layered window every frame.
    constexpr float kWidthQuantum = 2.0f;
}

// Layout for the game overlay strip. The size the island animates to is decided
// in the render loop, while the contents are painted by DrawGameOverlay. Those
// two kept their own copies of the card width, padding and height, so widening a
// card in one place left the other sizing the island for the old value -- the
// strip would reserve room for four cards and then drop the last one on the
// "ran out of room" check. Both read this now.
//
// Deliberately free of g_settings so it can sit up here with the other layout
// namespaces; callers pass what they know.
namespace GameOverlayLayout {
    struct Metrics {
        float padX;
        float padY;
        float cardW;
        float gap;
        float fpsW;
        float fpsGap;
        float radius;
        float height;
    };

    // Cards are wider than the original 52/62 so a label and a three-digit value
    // sit in separate bands without touching, and the corner radius matches the
    // 9-10px used by every other card instead of the old 16, which on a 44px-tall
    // card was very nearly a stadium.
    constexpr Metrics For(bool compact) {
        return compact ? Metrics{10.0f,  9.0f, 62.0f, 6.0f, 74.0f, 7.0f,  9.0f, 56.0f}
                       : Metrics{12.0f, 11.0f, 74.0f, 7.0f, 88.0f, 8.0f, 10.0f, 68.0f};
    }

    constexpr float kMinWidth = 140.0f;

    constexpr float Width(bool compact, bool showFps, int metricCount) {
        const Metrics m = For(compact);
        float w = m.padX * 2.0f;
        if (showFps) {
            w += m.fpsW + m.fpsGap;
        }
        if (metricCount > 0) {
            w += static_cast<float>(metricCount) * m.cardW +
                 static_cast<float>(metricCount - 1) * m.gap;
        }
        return w < kMinWidth ? kMinWidth : w;
    }
}

// ── Media dashboard hit-test geometry ────────────────────────────────────────
// DrawMedia publishes the exact content-space rect it painted into, together
// with the scale that maps content px to client px. OverlayWndProc converts
// mouse positions through this instead of assuming the pill is centered in the
// client area, which it is not when the notch / border-merged offset, the hover
// scale, or a split (two-pill) layout is in play. Keeping one published source
// of truth is what stops the drawn buttons and their hit boxes from drifting
// apart -- the drift that made prev/next clicks fall through to "open the app".
// The fields are guarded by a seqlock rather than read independently: the render
// thread rewrites them every frame, and during the expand animation the content
// height sweeps ~36 -> 184px, so a reader that caught half of one frame and half
// of the next could misplace a hit box for a click. g_mediaHitSeq is odd while a
// write is in progress; readers retry until they see the same even value twice.
// The payload stays in relaxed atomics so there is no formal data race; the
// seqlock counter is what provides consistency *across* the fields.
std::atomic<unsigned> g_mediaHitSeq{0};
std::atomic<float> g_mediaHitLeft{0.0f};
std::atomic<float> g_mediaHitTop{0.0f};
std::atomic<float> g_mediaHitRight{0.0f};
std::atomic<float> g_mediaHitBottom{0.0f};
std::atomic<float> g_mediaHitScale{1.0f};
std::atomic<unsigned long long> g_mediaHitStamp{0};

struct MediaContentPoint {
    bool valid = false;
    float x = 0.0f;        // relative to the content rect's left edge
    float y = 0.0f;        // relative to the content rect's top edge
    float width = 0.0f;    // content rect width
    float height = 0.0f;   // content rect height
};

// The conversion and hit-test helpers live further down, just after Clamp().

enum class IslandKind {
    Idle,
    Media,
    Progress,
    Clipboard,
    Notification,
    Volume,
    BatteryLow,
    CapsLock,
    Device,
    Bluetooth,
    Timer,
    DoNotDisturb,
    Split,
};

enum class BluetoothDeviceCategory {
    Headphones,
    Speaker,
    Mouse,
    Keyboard,
    Phone,
    Generic,
};

enum class Position {
    TopCenter,
    TopLeft,
    TopRight,
    BottomCenter,
    BottomLeft,
    BottomRight,
};

// Anything anchored to the bottom edge shares the same vertical placement and
// cannot use the top-edge macOS notch shape.
constexpr bool IsBottomPosition(Position position) {
    return position == Position::BottomCenter ||
           position == Position::BottomLeft ||
           position == Position::BottomRight;
}

enum class AccentMode {
    Auto,
    System,
    Custom,
};

enum class AnimationStyle {
    Smooth,
    Default,
    Bouncy,
    Snappy,
};

enum class CalendarAccentMode {
    Red,
    System,
};

// Curated palettes. These replace the original four (OLED Black, Fluent,
// Midnight Blue, Deep Purple), which were built for a material that painted a
// glass highlight along every edge. With the edge lighting gone, a palette has
// to carry the whole look on flat fills, so each of these is tuned for
// separation by value alone -- and one is light, which the old set had none of.
//
// Order is load-bearing: it is persisted as an integer index, so append new
// entries at the end rather than inserting. Custom must stay last.
enum class ThemePreset {
    Obsidian,
    Graphite,
    Slate,
    Nord,
    Evergreen,
    Espresso,
    Plum,
    Porcelain,
    LiquidGlass,
    Custom,
};

// One table drives the settings dropdown, the right-click menu labels and the
// resolved colors. It used to be three separate lists, which is how the menu
// ended up carrying special cases for palette indexes that no longer matched.
//
// Secondary text is held at roughly 4.5:1 against its own background in every
// row, because no edge highlight is left to help muted labels separate from the
// surface behind them.
struct ThemePalette {
    const wchar_t* id;      // value stored in Themes.ThemePreset
    const wchar_t* label;   // right-click menu label
    const wchar_t* bg;
    const wchar_t* fg;
    const wchar_t* sec;
    const wchar_t* border;
};

static constexpr ThemePalette kThemePalettes[] = {
    {L"obsidian",  L"Obsidian (Default)", L"#08080A", L"#FFFFFF", L"#9B9BA5", L"#1E1E22"},
    {L"graphite",  L"Graphite",           L"#1C1C1E", L"#FFFFFF", L"#A8A8AE", L"#323236"},
    {L"slate",     L"Slate",              L"#111721", L"#E9EEF6", L"#92A2B8", L"#232C3A"},
    {L"nord",      L"Nord",               L"#2E3440", L"#ECEFF4", L"#A7B0C0", L"#3B4252"},
    {L"evergreen", L"Evergreen",          L"#0C1512", L"#E4F1EA", L"#8CAE9D", L"#1A2A23"},
    {L"espresso",  L"Espresso",           L"#1A1512", L"#F6EEE7", L"#B7A395", L"#2E2420"},
    {L"plum",      L"Plum",               L"#16111C", L"#F1EAF6", L"#AC9BB9", L"#281F33"},
    {L"porcelain", L"Porcelain (Light)",  L"#F5F6F8", L"#14161A", L"#5B6270", L"#D8DBE1"},
    {L"liquid-glass", L"Liquid Glass",    L"#1B2633", L"#F4FAFF", L"#B6C9D9", L"#C9E7FA"},
};

// Custom sits one past the last real palette. Derived, so adding a palette
// cannot leave a stale literal behind.
static constexpr int kCustomThemeIndex = static_cast<int>(ARRAYSIZE(kThemePalettes));

// Persisted under a new key. Retiring the original palettes renumbered these
// integers, and reusing "ColorTheme" would have silently reinterpreted a saved
// 4 (previously Custom) as the palette that now sits at index 4.
static constexpr const wchar_t* kThemeValueName = L"ColorThemeV2";
static constexpr const wchar_t* kLegacyThemeValueName = L"ColorTheme";

// Base menu command id for the theme submenu; entries occupy
// [kThemeMenuIdBase, kThemeMenuIdBase + kCustomThemeIndex].
static constexpr UINT kThemeMenuIdBase = 20;

// Maps a preset id to its palette index, accepting the retired ids too: a
// user's stored setting keeps its old string after the option disappears from
// the dropdown, so each legacy id is pointed at the new palette closest to it.
inline int ThemeIndexFromId(std::wstring_view id) {
    auto iequals = [](std::wstring_view a, std::wstring_view b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (towlower(a[i]) != towlower(b[i])) return false;
        }
        return true;
    };

    for (int i = 0; i < kCustomThemeIndex; ++i) {
        if (iequals(id, kThemePalettes[i].id)) return i;
    }
    if (iequals(id, L"custom")) return kCustomThemeIndex;

    if (iequals(id, L"oled-black")) return 0;     // -> Obsidian
    if (iequals(id, L"fluent") ||
        iequals(id, L"mica") ||
        iequals(id, L"dark-gray")) return 1;      // -> Graphite
    if (iequals(id, L"midnight-blue")) return 2;  // -> Slate
    if (iequals(id, L"deep-purple")) return 6;    // -> Plum
    return -1;
}

// Real Windows blur / acrylic painted behind the island (#59).
enum class BackdropMaterial {
    None,
    Blur,
    Acrylic,
};

enum class ContourBorderMode {
    Default,
    Auto,
    Borderless,
};

struct Settings {
    Position position = Position::TopCenter;
    int targetMonitor = 0;
    int offsetX = 0;
    int offsetY = 0;
    bool separateExpandedOffsetY = false;  // #83
    int offsetYExpanded = 0;               // #83
    float sizeScale = 1.0f;
    float islandWidth = 150.0f;
    float islandHeight = 44.0f;
    float expandedWidth = 380.0f;
    float expandedHeight = 184.0f;
    float cornerRadius = 44.0f;
    std::wstring fontFamily;
    AccentMode accentMode = AccentMode::Auto;
    D2D1_COLOR_F customAccent = D2D1::ColorF(0x4cc9f0);
    int targetFps = 0; // 0 = Auto
    AnimationStyle animationStyle = AnimationStyle::Default;
    float animationSpeed = 1.0f;
    bool respectReducedMotion = true;
    bool reduceMotion = false;
    bool media = true;
    bool miniVideoPlayer = true;
    bool musicVisualizer = true;
    int visualizerStrength = 100;
    bool mediaAutoExpand = false;
    bool mediaAutoExpandHotkey = true;
    bool windowsNotifications = true;
    int notificationDurationSeconds = 4;
    bool quickAccess = true;
    std::vector<std::pair<std::wstring, std::wstring>> quickAccessLinks;
    std::wstring quickAccessBrowser = L"default";
    std::vector<std::wstring> videoAppIds;
    bool clipboard = true;
    bool clipboardSection = true;
    int clipboardHistoryLimit = 100;
    bool rememberLastSection = true;
    bool statusCountdownProgress = false;
    bool battery = true;
    bool progress = true;
    bool volume = true;
    bool fileCopyNotifications = true;
    bool browserDownloadNotifications = true;
    CalendarAccentMode calendarAccent = CalendarAccentMode::Red;
    bool privacyDots = true;
    bool privacyDotsMic = true;
    bool privacyDotsCam = true;
    bool privacyDotsPulse = true;
    // Modules.CapsLock -- fixes windhawk-mods#4352, which asked for a way to turn
    // the Caps Lock / Num Lock indicator off.
    //
    // Enforced in three places, because any one of them alone leaks:
    //   ChooseActivities        - the pill is never selected for display
    //   WM_APP_CAPSLOCK handler - no state is recorded, no nudge is triggered
    //   CapsLockHookWanted      - WH_KEYBOARD_LL is not installed at all
    // The activity gate is the one that actually hides the pill; the others stop
    // the work leading up to it.
    bool capsLock = true;
    bool timerEnabled = true;
    bool hideShowHotkeyEnabled = true;
    UINT hideShowModifiers = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    UINT hideShowVk = 'D';
    bool bluetoothIndicator = true;
    bool bluetoothShowBattery = true;
    D2D1_COLOR_F privacyDotsMicHex = D2D1::ColorF(1.0f, 0.584f, 0.0f, 1.0f); // #FF9500
    D2D1_COLOR_F privacyDotsCamHex = D2D1::ColorF(0.133f, 0.776f, 0.239f, 1.0f); // #10B981
    float tintOpacity = 0.72f;
    float pillOpacity = 0.96f;
    bool gameOverlay = false;
    bool showMetricText = true;
    bool weather = true;
    std::wstring weatherCity;
    bool weatherFahrenheit = false;
    int autoHideIdleSeconds = 0;
    bool autoHideFullscreen = true;
    bool borderMergedMode = false;
    bool unhideOnHover = true;
    bool alwaysOnTop = true;
    bool expandOnHover = true;
    bool autoDpiScale = true;
    bool w11Style = false;
    bool notchStyle = false;
    // Color customization
    ThemePreset themePreset = ThemePreset::Obsidian;
    D2D1_COLOR_F pillBgColor = D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f); // #08080A
    D2D1_COLOR_F textPrimaryColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f); // #FFFFFF
    D2D1_COLOR_F textSecondaryColor = D2D1::ColorF(0.608f, 0.608f, 0.647f, 1.0f); // #9B9BA5
    ContourBorderMode contourBorderMode = ContourBorderMode::Default;
    bool contourBorderEnabled = true;
    D2D1_COLOR_F contourBorderColor = D2D1::ColorF(0.200f, 0.200f, 0.220f, 1.0f); // #333338
    bool clockAccentGlow = true;
    bool privacyDotsEnabled = true;
    bool privacyDotPulsing = true;
    D2D1_COLOR_F micDotColor = D2D1::ColorF(1.0f, 0.584f, 0.0f, 1.0f); // #FF9500
    D2D1_COLOR_F camDotColor = D2D1::ColorF(0.204f, 0.780f, 0.349f, 1.0f); // #34C759
    bool hardwareMonitorModule = true;
    bool calendarModule = true;
    std::vector<int> dashboardOrder{0, 2, 3, 4, 5, 7, 1, 6};
    bool doNotDisturbIndicator = true;
    bool notificationRespectDnD = true;

    // ── Premium material / redesign ──────────────────────────────────────────
    bool materialDepth = true;      // downward depth shading + accent bloom
    bool dropShadow = true;         // soft shadow under the island
    float accentBloom = 1.0f;       // 0..2 multiplier on the accent wash
    float textScale = 1.0f;         // independent typography scale (#41)
    BackdropMaterial backdropMaterial = BackdropMaterial::None;  // #59
    float backdropTint = 0.55f;     // acrylic tint strength, 0..1
    float backdropFillAlpha = 0.45f;  // how opaque the pill's own fill stays

    // ── Clock / date presentation (#61) ──────────────────────────────────────
    bool showSeconds = false;
    bool use24HourClock = false;    // false = follow system locale
    bool clockFollowSystem = true;
    std::wstring dateFormat;        // empty = locale default
    bool dateFirst = false;         // show date before time in the idle strip

    // ── Localization (#35) ───────────────────────────────────────────────────
    std::wstring language = L"auto";

    // ── File tray (#33) ──────────────────────────────────────────────────────
    bool fileTrayModule = false;
    int fileTrayMaxItems = 10;

    // ── Media auto-expand exclusions (#62) ───────────────────────────────────
    std::vector<std::wstring> mediaExpandBlocklist;

    // ── Game overlay options (#25) ───────────────────────────────────────────
    bool gameOverlayShowFps = true;
    bool gameOverlayShowCpu = true;
    bool gameOverlayShowGpu = true;
    bool gameOverlayShowRam = true;
    bool gameOverlayShowDisk = true;
    bool gameOverlayCompact = false;
};

struct BitmapPixels {
    std::vector<uint8_t> bgra;
    UINT width = 0;
    UINT height = 0;
    uint64_t generation = 0;
    D2D1_COLOR_F sampledAccent = D2D1::ColorF(0x4cc9f0);
};

struct MediaSnapshot {
    bool available = false;
    bool playing = false;
    bool isVideo = false;
    std::wstring title;
    std::wstring artist;
    std::wstring albumTitle;
    std::wstring sourceAppUserModelId;
    std::wstring sourceName;
    std::wstring sourceBadge;
    BitmapPixels art;
    BitmapPixels sourceIcon;
    uint64_t artGeneration = 0;
    uint64_t sourceIconGeneration = 0;
    double artChangedAt = 0.0;
    double titleChangedAt = 0.0;
    int64_t positionTicks = 0;
    int64_t startTicks = 0;
    int64_t endTicks = 0;
    int64_t minSeekTicks = 0;
    int64_t maxSeekTicks = 0;
    int64_t lastUpdatedTicks = 0;
};

struct ClipboardSnapshot {
    bool active = false;
    bool image = false;
    std::wstring text;
    std::wstring dashboardText;
    std::wstring appName;
    BitmapPixels appIcon;
    BitmapPixels imagePreview;  // decoded thumbnail when the clipboard holds an image
    double expiresAt = 0.0;
};

struct ClipboardHistoryItem {
    uint64_t id = 0;
    bool pinned = false;
    bool image = false;
    std::wstring text;
    std::wstring appName;
    BitmapPixels imagePreview;
    double copiedAt = 0.0;
};

struct BatterySnapshot {
    bool active = false;
    bool low = false;
    bool charging = false;
    int percent = 100;
    DWORD secondsRemaining = BATTERY_LIFE_UNKNOWN;
    double expiresAt = 0.0;
};

struct ProgressSnapshot {
    bool active = false;
    int percent = 0;
};

struct NotificationSnapshot {
    bool active = false;
    std::wstring app;
    std::wstring title;
    std::wstring body;
    BitmapPixels icon;
    double expiresAt = 0.0;
};

struct VolumeSnapshot {
    bool active = false;
    int percent = 0;
    bool muted = false;
    std::wstring deviceName;
    double expiresAt = 0.0;
};

struct CapsLockSnapshot {
    bool active = false;
    bool capsOn = false;
    bool numOn = false;
    bool isNumEvent = false;
    double expiresAt = 0.0;
};

struct TimerSnapshot {
    bool active = false;          // a session exists (running or paused)
    bool running = false;         // currently counting down
    bool isBreak = false;         // work vs break session
    int totalSeconds = 0;
    double endsAt = 0.0;          // NowSeconds() at completion, while running
    double remainingAtPause = 0.0;
    bool justFinished = false;
    double finishedExpiresAt = 0.0;
};

enum class DeviceEventType {
    Connected,
    Disconnected,
};

struct DeviceSnapshot {
    bool active = false;
    DeviceEventType eventType = DeviceEventType::Connected;
    std::wstring deviceName;  // e.g. "USB Drive" or "Bluetooth Device"
    bool isBluetoothLike = false;
    double expiresAt = 0.0;
};

struct BluetoothDeviceSnapshot {
    bool active = false;
    bool connected = true;   // true = just connected, false = just disconnected
    std::wstring deviceName;
    int batteryPercent = -1; // -1 = unknown/unavailable
    BluetoothDeviceCategory category = BluetoothDeviceCategory::Generic;
    double expiresAt = 0.0;
};

struct SystemSnapshot {
    int volumePercent = 0;
    bool volumeMuted = false;
    int cpuPercent = 0;
    int memoryPercent = 0;
    float memoryUsedGB = 0.0f;
    float memoryTotalGB = 0.0f;
    int diskFreePercent = 0;
    uint64_t hostPrivateBytes = 0;
    int renderFps = 0;
    int gpuPercent = -1;
    float netUpMbps = 0.0f;
    float netDownMbps = 0.0f;
    bool charging = false;
    bool micActive = false;      // orange dot: microphone in use
    bool cameraActive = false;   // green dot: camera in use
    std::wstring foregroundTitle;
    std::wstring micApp;
    std::wstring cameraApp;
};

struct Activity {
    IslandKind kind = IslandKind::Idle;
    float width = 120.0f;
    float height = 36.0f;
};

struct WeatherSnapshot {
    bool hasData = false;
    float temperature = 0.0f;
    int weatherCode = 0;
    std::wstring city;
    std::wstring weatherDesc;
    std::wstring windSpeed;
    std::wstring windDir;
    std::wstring humidity;
    std::wstring feelsLike;
    double lastUpdated = 0.0;
};

struct DoNotDisturbSnapshot {
    bool active = false;
    bool enabled = false;
    double expiresAt = 0.0;
};

// One file parked on the island's shelf (#33). Kept deliberately small: the
// tray holds references, never copies of the files themselves.
struct FileTrayItem {
    std::wstring path;
    std::wstring name;
    uint64_t sizeBytes = 0;
    bool isDirectory = false;
    BitmapPixels icon;
};

struct SharedState {
    MediaSnapshot media;
    ClipboardSnapshot clipboard;
    // Immutable snapshots keep per-frame SharedState copies cheap while the
    // clipboard history itself can contain many text strings and thumbnails.
    std::shared_ptr<const std::vector<ClipboardHistoryItem>> clipboardHistory;
    NotificationSnapshot notification;
    VolumeSnapshot volume;
    CapsLockSnapshot capsLock;
    TimerSnapshot timer;
    DeviceSnapshot device;
    BluetoothDeviceSnapshot bluetoothDevice;
    DoNotDisturbSnapshot doNotDisturb;
    BatterySnapshot battery;
    ProgressSnapshot progress;
    SystemSnapshot system;
    WeatherSnapshot weather;
    std::array<float, 48> waveform{};
    size_t waveformWrite = 0;
    bool muted = false;
    std::vector<FileTrayItem> fileTrayItems;
};

struct SpringValue {
    float value = 0.0f;
    float velocity = 0.0f;
    float target = 0.0f;

    void Reset(float v) {
        value = target = v;
        velocity = 0.0f;
    }

    void Step(float totalDt, float stiffness, float damping) {
        const float kFixedDt = 0.0005f;
        while (totalDt > 0.0f) {
            float dt = std::min(totalDt, kFixedDt);
            const float displacement = value - target;
            const float acceleration = -stiffness * displacement - damping * velocity;
            velocity += acceleration * dt;
            value += velocity * dt;
            totalDt -= dt;
        }

        if (std::fabs(value - target) < 0.01f && std::fabs(velocity) < 0.01f) {
            value = target;
            velocity = 0.0f;
        }
    }
};

Settings g_settings;

// Guards every access to g_settings. LoadSettings() replaces the whole struct
// from Windhawk's settings thread *and* from the tray context menu (which runs
// on the render thread), while the render, weather and media threads read it
// concurrently. Settings holds std::wstring / std::vector members, so an
// unsynchronised assignment can free a buffer a reader is still walking -- not
// a stale-value glitch but a genuine use-after-free.
//
// This is a strict LEAF lock: never acquire another mutex while holding it.
// That keeps it deadlock-free even where a caller already owns g_stateMutex
// (see WeatherThreadProc), because no path can ever take the two in the
// opposite order.
std::mutex g_settingsMutex;

// Returns a private copy of the current settings. Callers then read from the
// copy for the rest of the frame, which also means a settings change landing
// mid-frame can't tear a single render across two configurations.
Settings GetSettingsCopy() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::mutex g_stateMutex;
SharedState g_state;
std::atomic<uint64_t> g_artGenerationCounter = 0;
std::atomic<uint64_t> g_clipboardHistoryId = 0;

HWND g_hwnd = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsChangedEvent = nullptr;
HANDLE g_renderThread = nullptr;
HANDLE g_mediaThread = nullptr;
HANDLE g_audioThread = nullptr;
HANDLE g_notificationThread = nullptr;
HANDLE g_bluetoothThread = nullptr;
HANDLE g_downloadThread = nullptr;
std::atomic<bool> g_running = false;
std::atomic<int> g_idleTab = 0;
std::atomic<int> g_clipboardHistoryPage = 0;
std::atomic<int> g_fileTrayScrollOffset = 0;
std::atomic<int> g_calendarViewYear = 0;
std::atomic<int> g_calendarViewMonth = 0;
std::atomic<int> g_calendarSelectedDay = 0;
std::atomic<int> g_calendarPickerMode = 0;  // 0 = days, 1 = month chooser, 2 = year chooser
std::atomic<int> g_calendarYearRangeStart = 0;
std::atomic<bool> g_layoutDirty = true;
std::atomic<bool> g_clickExpanded = false;
std::atomic<int> g_pressedMediaButton = -1;
std::atomic<int> g_hoveredMediaButton = -1;
std::atomic<bool> g_hoveredMediaVolume = false;
std::atomic<bool> g_mediaVolumeDragging = false;
std::atomic<double> g_mediaVolumeAdjustUntil = 0.0;
std::atomic<uint64_t> g_seekRequestGeneration = 0;
std::atomic<int> g_visualizerStyle = 0;
std::atomic<int> g_visualizerStrength{100};
std::atomic<bool> g_mediaAutoExpandEnabled = false;
std::atomic<bool> g_mediaAutoExpandHotkeyEnabled = true;
std::atomic<int> g_notificationDurationSeconds{4};
std::atomic<int> g_hoveredClipboardAction = 0;
std::atomic<int> g_hoveredFileTrayRow = -1;  // row index under the cursor on the File Tray card
// UI-thread-owned drag state for dragging a history card onto its trash button.
int g_contentDragKind = 0; // 1 = clipboard history, 2 = File Tray
uint64_t g_contentDragClipboardId = 0;
std::wstring g_contentDragFilePath;
POINT g_contentDragStart = {};
POINT g_contentDragPoint = {};
bool g_contentDragActive = false;
std::mutex g_mediaSourceMutex;
std::vector<std::wstring> g_mediaSourceIds;
std::wstring g_preferredMediaSourceId;
std::atomic<bool> g_scrubbing = false;           // true while press-dragging the media timeline scrubber
std::atomic<float> g_scrubDragFraction = 0.0f;   // live 0..1 drag position while g_scrubbing is true
std::atomic<double> g_lastLiveSeekTime = 0.0;    // throttle gate for live seeks while dragging
std::atomic<bool> g_audioCaptureNeeded = false;  // gates the WASAPI loopback thread
std::atomic<bool> g_manuallyHidden = false;      // user-toggled hide, via menu or hotkey
UINT g_registeredHotkeyModifiers = 0;            // modifiers currently registered with the OS
UINT g_registeredHotkeyVk = 0;                   // vk currently registered with the OS
bool g_hotkeyRegistered = false;
bool g_mediaExpandHotkeyRegistered = false;

// --- Zero-CPU parking for idle/fullscreen auto-hide (mirrors g_manuallyHidden) ---
std::atomic<bool> g_autoHiddenParked = false;          // true while OS-hidden due to idle/fullscreen auto-hide
std::atomic<bool> g_autoHideEnabled = true;            // runtime toggle, controlled by the shortcut
std::atomic<bool> g_fullscreenOverrideVisible = false; // user forced the island visible via hotkey while fullscreen
std::atomic<bool> g_isFullscreen = false;              // cached fullscreen state, shared with the hotkey handler
std::atomic<double> g_hotkeyUnhideUntil = 0.0;         // grace deadline to keep island visible after hotkey / settings unhide
std::atomic<bool> g_hoverWakeSession = false;
std::atomic<bool> g_hoverWakeInitialHover = false;
std::atomic<double> g_hoverRevealUntil = 0.0;
FILETIME g_prevIdleTime = {};
FILETIME g_prevKernelTime = {};
FILETIME g_prevUserTime = {};
UINT g_shellHookMessage = 0;
UINT g_taskbarCreatedMessage = 0;
bool g_volumeInitialized = false;
std::atomic<double> g_lastNudgeTime = 0.0;

std::mutex g_bluetoothBatteryCacheMutex;
std::unordered_map<std::wstring, int> g_bluetoothBatteryCache;  // Bluetooth device Id -> last known battery percent (-1 = never learned)
std::atomic<uint64_t> g_bluetoothConnectGeneration = 0;
std::atomic<bool> g_isDnDActive = false;
void* g_wnfDndSubscription = nullptr;

constexpr GUID kSubTypeIeeeFloat = {
    0x00000003,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
};



double NowSeconds() {
    using clock = std::chrono::steady_clock;
    static const auto start = clock::now();
    return std::chrono::duration<double>(clock::now() - start).count();
}

float Clamp(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

int ClampInt(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

// ── Dashboard tab loop ───────────────────────────────────────────────────────
// Time is always its own primary section. A playback page is added only while
// an open SMTC session exists, so paused playback remains available without
// exposing an idle playback section after the owning app releases its session.
// Stable IDs keep saved selection, rendering and input in sync as tabs change.
int ActivePlaybackSectionId(const Settings& settings, const SharedState& state) {
    if (!settings.media || !state.media.available) return -1;
    return state.media.isVideo && settings.miniVideoPlayer ? 1 : 6;
}

int CurrentPlaybackSectionId(const Settings& settings) {
    std::lock_guard lock(g_stateMutex);
    return ActivePlaybackSectionId(settings, g_state);
}

std::vector<int> DashboardSectionIds(const Settings& settings, const SharedState& state) {
    std::vector<int> sections{0};  // Time
    if (settings.clipboardSection) sections.push_back(2);
    if (settings.calendarModule) sections.push_back(3);
    if (settings.hardwareMonitorModule) sections.push_back(4);
    if (settings.fileTrayModule) sections.push_back(5);
    if (settings.quickAccess) sections.push_back(8);
    const int playbackId = ActivePlaybackSectionId(settings, state);
    if (playbackId >= 0) sections.push_back(playbackId);
    std::stable_sort(sections.begin(), sections.end(), [&](int a, int b) {
        const auto rank = [&](int id) {
            const auto it = std::find(settings.dashboardOrder.begin(),
                                      settings.dashboardOrder.end(), id);
            return it == settings.dashboardOrder.end()
                       ? settings.dashboardOrder.size()
                       : static_cast<size_t>(it - settings.dashboardOrder.begin());
        };
        return rank(a) < rank(b);
    });
    return sections;
}

int ActiveTabCount(const Settings& settings) {
    std::lock_guard lock(g_stateMutex);
    return static_cast<int>(DashboardSectionIds(settings, g_state).size());
}

int DashboardIndexForSectionId(const Settings& settings, int sectionId);

int NormalizedTabIndex(const Settings& settings) {
    const int total = std::max(1, ActiveTabCount(settings));
    const int raw = g_idleTab.load(std::memory_order_relaxed);
    return ((raw % total) + total) % total;
}

// The expensive GPU / network counters are only sampled while the hardware card
// is actually on screen. Resolve the section by stable ID so custom section order
// and a temporarily available playback section cannot invalidate the index.
int HardwareMonitorTabIndex(const Settings& settings) {
    return settings.hardwareMonitorModule
               ? DashboardIndexForSectionId(settings, 4)
               : -1;
}

int FileTrayTabIndex(const Settings& settings) {
    return settings.fileTrayModule ? DashboardIndexForSectionId(settings, 5) : -1;
}

int DashboardSectionIdAtIndex(const Settings& settings, int index) {
    std::lock_guard lock(g_stateMutex);
    const std::vector<int> sections = DashboardSectionIds(settings, g_state);
    const int count = std::max(1, static_cast<int>(sections.size()));
    const int normalized = ((index % count) + count) % count;
    return sections[normalized];
}

int DashboardIndexForSectionId(const Settings& settings, int sectionId) {
    std::lock_guard lock(g_stateMutex);
    const std::vector<int> sections = DashboardSectionIds(settings, g_state);
    auto it = std::find(sections.begin(), sections.end(), sectionId);
    return it == sections.end() ? 0 : static_cast<int>(it - sections.begin());
}

bool IsMediaPlaybackTab(const Settings& settings, int tabIndex) {
    const int playbackId = CurrentPlaybackSectionId(settings);
    return playbackId >= 0 && tabIndex == DashboardIndexForSectionId(settings, playbackId);
}

void SetDashboardTabIndex(const Settings& settings, int index) {
    const int count = std::max(1, ActiveTabCount(settings));
    const int wrapped = ((index % count) + count) % count;
    g_idleTab.store(wrapped, std::memory_order_relaxed);
    g_hoveredMediaVolume.store(false, std::memory_order_relaxed);
    if (settings.rememberLastSection) {
        Wh_SetIntValue(L"LastDashboardSection", DashboardSectionIdAtIndex(settings, wrapped));
    }
    g_layoutDirty = true;
}

int ClipboardGridColumns(size_t itemCount) {
    if (itemCount <= 1) return 1;
    if (itemCount <= 4) return 2;
    return 3;
}

int ClipboardGridRows(size_t itemCount) {
    if (itemCount <= 2) return 1;
    return 2;
}

int ClipboardHistoryPageSize(size_t itemCount) {
    return ClipboardGridColumns(itemCount) * ClipboardGridRows(itemCount);
}

int ClipboardHistoryPageCount(size_t itemCount) {
    if (itemCount == 0) return 1;
    const size_t pageSize = static_cast<size_t>(ClipboardHistoryPageSize(itemCount));
    return static_cast<int>((itemCount + pageSize - 1) / pageSize);
}

// ── Localization (#35) ───────────────────────────────────────────────────────
// Every user-visible string in the island goes through Loc(). The English text
// doubles as the lookup key, so an untranslated string degrades to readable
// English rather than to a raw identifier, and adding a language means adding
// one column -- no key bookkeeping.
enum class UiLanguage {
    English,
    French,
    Spanish,
    German,
    Portuguese,
    Italian,
    Russian,
    Turkish,
    Hindi,
    ChineseSimplified,
    Japanese,
    Korean,
    Count,
};

std::atomic<int> g_uiLanguage{static_cast<int>(UiLanguage::English)};

UiLanguage LanguageFromTag(std::wstring_view tag) {
    struct Entry { const wchar_t* tag; UiLanguage lang; };
    static constexpr Entry kTags[] = {
        {L"en", UiLanguage::English},   {L"fr", UiLanguage::French},
        {L"es", UiLanguage::Spanish},   {L"de", UiLanguage::German},
        {L"pt", UiLanguage::Portuguese},{L"it", UiLanguage::Italian},
        {L"ru", UiLanguage::Russian},   {L"tr", UiLanguage::Turkish},
        {L"hi", UiLanguage::Hindi},     {L"zh", UiLanguage::ChineseSimplified},
        {L"ja", UiLanguage::Japanese},  {L"ko", UiLanguage::Korean},
    };
    if (tag.size() < 2) {
        return UiLanguage::English;
    }
    for (const Entry& e : kTags) {
        // Prefix match so "pt-BR" / "zh-Hans-CN" resolve to their base language.
        if (_wcsnicmp(tag.data(), e.tag, 2) == 0) {
            return e.lang;
        }
    }
    return UiLanguage::English;
}

// Resolves the "auto" language setting from the user's Windows UI language.
UiLanguage DetectSystemLanguage() {
    wchar_t name[LOCALE_NAME_MAX_LENGTH] = {};
    if (GetUserDefaultLocaleName(name, ARRAYSIZE(name)) > 0) {
        return LanguageFromTag(name);
    }
    return UiLanguage::English;
}

// ── Clock / date presentation (#61) ──────────────────────────────────────────

// Formats the time honouring the 12h/24h override and the seconds toggle.
// Windows' own locale formatting is used unless the user forced a mode, so the
// default keeps regional conventions (separators, AM/PM placement) intact.
std::wstring FormatIslandTime(const SYSTEMTIME& local, bool followSystem, bool use24Hour,
                              bool showSeconds) {
    wchar_t buffer[64] = {};

    if (followSystem) {
        const DWORD flags = showSeconds ? 0 : TIME_NOSECONDS;
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, flags, &local, nullptr, buffer,
                            ARRAYSIZE(buffer)) > 0) {
            return buffer;
        }
        return L"--:--";
    }

    // Explicit override: build the pattern rather than fighting locale flags.
    const wchar_t* pattern = use24Hour ? (showSeconds ? L"HH:mm:ss" : L"HH:mm")
                                       : (showSeconds ? L"h:mm:ss tt" : L"h:mm tt");
    if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, pattern, buffer,
                        ARRAYSIZE(buffer)) > 0) {
        return buffer;
    }
    return L"--:--";
}

// Formats the date. A custom pattern is passed straight to Windows, which
// already supports yyyy / MM / dd / MMM / MMMM / ddd / dddd and prints anything
// else literally -- so CJK patterns such as yyyy年MM月dd日 work as typed.
std::wstring FormatIslandDate(const SYSTEMTIME& local, const std::wstring& customFormat,
                              const wchar_t* fallbackPattern) {
    wchar_t buffer[128] = {};

    if (!customFormat.empty()) {
        if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, customFormat.c_str(), buffer,
                            ARRAYSIZE(buffer), nullptr) > 0) {
            return buffer;
        }
        // An invalid pattern falls through to the default rather than showing
        // nothing at all.
    }

    if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, fallbackPattern, buffer,
                        ARRAYSIZE(buffer), nullptr) > 0) {
        return buffer;
    }
    return std::wstring();
}

const wchar_t* Loc(const wchar_t* english) {
    struct Row {
        const wchar_t* key;
        // Indexed by UiLanguage; nullptr falls back to the English key.
        const wchar_t* text[static_cast<size_t>(UiLanguage::Count)];
    };

    // Order: en, fr, es, de, pt, it, ru, tr, hi, zh, ja, ko
    static const Row kRows[] = {
        {L"Media", {nullptr, L"Média", L"Multimedia", L"Medien", L"Mídia", L"Media", L"Медиа", L"Medya", L"मीडिया", L"媒体", L"メディア", L"미디어"}},
        {L"Calendar", {nullptr, L"Calendrier", L"Calendario", L"Kalender", L"Calendário", L"Calendario", L"Календарь", L"Takvim", L"कैलेंडर", L"日历", L"カレンダー", L"캘린더"}},
        {L"Weather", {nullptr, L"Météo", L"Tiempo", L"Wetter", L"Tempo", L"Meteo", L"Погода", L"Hava", L"मौसम", L"天气", L"天気", L"날씨"}},
        {L"Hardware Monitor", {nullptr, L"Moniteur matériel", L"Monitor de hardware", L"Hardware-Monitor", L"Monitor de hardware", L"Monitor hardware", L"Монитор системы", L"Donanım İzleme", L"हार्डवेयर मॉनिटर", L"硬件监视器", L"ハードウェア モニター", L"하드웨어 모니터"}},
        {L"File Tray", {nullptr, L"Bac à fichiers", L"Bandeja de archivos", L"Dateiablage", L"Bandeja de arquivos", L"Vassoio file", L"Файлы", L"Dosya Tepsisi", L"फ़ाइल ट्रे", L"文件托盘", L"ファイル トレイ", L"파일 트레이"}},
        {L"Bluetooth", {nullptr, L"Bluetooth", L"Bluetooth", L"Bluetooth", L"Bluetooth", L"Bluetooth", L"Bluetooth", L"Bluetooth", L"ब्लूटूथ", L"蓝牙", L"Bluetooth", L"블루투스"}},
        {L"Copied", {nullptr, L"Copié", L"Copiado", L"Kopiert", L"Copiado", L"Copiato", L"Скопировано", L"Kopyalandı", L"कॉपी किया गया", L"已复制", L"コピーしました", L"복사됨"}},
        {L"Volume", {nullptr, L"Volume", L"Volumen", L"Lautstärke", L"Volume", L"Volume", L"Громкость", L"Ses", L"वॉल्यूम", L"音量", L"音量", L"볼륨"}},
        {L"Muted", {nullptr, L"Muet", L"Silenciado", L"Stumm", L"Sem som", L"Muto", L"Без звука", L"Sessiz", L"म्यूट", L"已静音", L"ミュート", L"음소거"}},
        {L"Low Battery", {nullptr, L"Batterie faible", L"Batería baja", L"Akku schwach", L"Bateria fraca", L"Batteria scarica", L"Батарея разряжена", L"Pil Az", L"बैटरी कम", L"电量低", L"バッテリー残量低下", L"배터리 부족"}},
        {L"Charging", {nullptr, L"En charge", L"Cargando", L"Wird geladen", L"Carregando", L"In carica", L"Зарядка", L"Şarj oluyor", L"चार्ज हो रहा है", L"正在充电", L"充電中", L"충전 중"}},
        {L"Connected", {nullptr, L"Connecté", L"Conectado", L"Verbunden", L"Conectado", L"Connesso", L"Подключено", L"Bağlandı", L"कनेक्ट किया गया", L"已连接", L"接続済み", L"연결됨"}},
        {L"Disconnected", {nullptr, L"Déconnecté", L"Desconectado", L"Getrennt", L"Desconectado", L"Disconnesso", L"Отключено", L"Bağlantı kesildi", L"डिस्कनेक्ट किया गया", L"已断开", L"切断されました", L"연결 끊김"}},
        {L"Caps Lock", {nullptr, L"Verr. Maj", L"Bloq Mayús", L"Feststelltaste", L"Caps Lock", L"Blocco maiuscole", L"Caps Lock", L"Caps Lock", L"कैप्स लॉक", L"大写锁定", L"Caps Lock", L"Caps Lock"}},
        {L"Num Lock", {nullptr, L"Verr. Num", L"Bloq Num", L"Num-Taste", L"Num Lock", L"Blocco num", L"Num Lock", L"Num Lock", L"नम लॉक", L"数字锁定", L"Num Lock", L"Num Lock"}},
        {L"On", {nullptr, L"Activé", L"Activado", L"Ein", L"Ligado", L"Attivo", L"Вкл", L"Açık", L"चालू", L"开", L"オン", L"켜짐"}},
        {L"Off", {nullptr, L"Désactivé", L"Desactivado", L"Aus", L"Desligado", L"Disattivo", L"Выкл", L"Kapalı", L"बंद", L"关", L"オフ", L"꺼짐"}},
        {L"Do Not Disturb", {nullptr, L"Ne pas déranger", L"No molestar", L"Nicht stören", L"Não perturbe", L"Non disturbare", L"Не беспокоить", L"Rahatsız Etme", L"परेशान न करें", L"专注助手", L"応答不可", L"방해 금지"}},
        {L"Focus", {nullptr, L"Concentration", L"Concentración", L"Fokus", L"Foco", L"Concentrazione", L"Фокус", L"Odak", L"फ़ोकस", L"专注", L"集中", L"집중"}},
        {L"Break", {nullptr, L"Pause", L"Descanso", L"Pause", L"Pausa", L"Pausa", L"Перерыв", L"Mola", L"विराम", L"休息", L"休憩", L"휴식"}},
        {L"Unknown", {nullptr, L"Inconnu", L"Desconocido", L"Unbekannt", L"Desconhecido", L"Sconosciuto", L"Неизвестно", L"Bilinmiyor", L"अज्ञात", L"未知", L"不明", L"알 수 없음"}},
        {L"Loading...", {nullptr, L"Chargement...", L"Cargando...", L"Wird geladen...", L"Carregando...", L"Caricamento...", L"Загрузка...", L"Yükleniyor...", L"लोड हो रहा है...", L"加载中...", L"読み込み中...", L"불러오는 중..."}},
        {L"Locating...", {nullptr, L"Localisation...", L"Ubicando...", L"Standort...", L"Localizando...", L"Localizzazione...", L"Определение...", L"Konum...", L"स्थान...", L"定位中...", L"位置情報...", L"위치 확인 중..."}},
        {L"Wind", {nullptr, L"Vent", L"Viento", L"Wind", L"Vento", L"Vento", L"Ветер", L"Rüzgar", L"हवा", L"风速", L"風", L"바람"}},
        {L"Feels Like", {nullptr, L"Ressenti", L"Sensación", L"Gefühlt", L"Sensação", L"Percepita", L"Ощущается", L"Hissedilen", L"महसूस", L"体感", L"体感", L"체감"}},
        {L"Humidity", {nullptr, L"Humidité", L"Humedad", L"Luftfeuchte", L"Umidade", L"Umidità", L"Влажность", L"Nem", L"नमी", L"湿度", L"湿度", L"습도"}},
        {L"Drag files here", {nullptr, L"Déposez des fichiers ici", L"Arrastra archivos aquí", L"Dateien hierher ziehen", L"Arraste arquivos aqui", L"Trascina i file qui", L"Перетащите файлы сюда", L"Dosyaları buraya sürükleyin", L"फ़ाइलें यहाँ खींचें", L"将文件拖到此处", L"ここにファイルをドラッグ", L"여기에 파일을 끌어다 놓으세요"}},
        {L"No devices", {nullptr, L"Aucun appareil", L"Sin dispositivos", L"Keine Geräte", L"Nenhum dispositivo", L"Nessun dispositivo", L"Нет устройств", L"Cihaz yok", L"कोई डिवाइस नहीं", L"无设备", L"デバイスなし", L"장치 없음"}},
        {L"item", {nullptr, L"élément", L"elemento", L"Element", L"item", L"elemento", L"элемент", L"öğe", L"आइटम", L"项", L"項目", L"항목"}},
        {L"items", {nullptr, L"éléments", L"elementos", L"Elemente", L"itens", L"elementi", L"элементов", L"öğe", L"आइटम", L"项", L"項目", L"항목"}},
    };

    const int langIndex = g_uiLanguage.load(std::memory_order_relaxed);
    if (langIndex <= static_cast<int>(UiLanguage::English) ||
        langIndex >= static_cast<int>(UiLanguage::Count)) {
        return english;
    }

    for (const Row& row : kRows) {
        if (wcscmp(row.key, english) == 0) {
            const wchar_t* translated = row.text[static_cast<size_t>(langIndex)];
            return translated ? translated : english;
        }
    }
    return english;
}

// ── Media dashboard hit-testing (see the notes next to MediaLayout) ──────────

// Converts a client-area mouse position into the media dashboard's content
// space. Returns valid == false when DrawMedia has not painted recently, so a
// stale frame can never produce a phantom hit.
MediaContentPoint MediaContentFromClient(int clientX, int clientY) {
    MediaContentPoint out;

    // Seqlock read: retry until a write is not in progress and the counter has
    // not moved, so all six fields come from the same frame.
    float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f, scale = 1.0f;
    unsigned long long stamp = 0;
    for (int attempt = 0; attempt < 8; ++attempt) {
        const unsigned before = g_mediaHitSeq.load(std::memory_order_relaxed);
        if (before & 1u) {
            continue;  // write in progress
        }
        std::atomic_thread_fence(std::memory_order_acquire);

        left = g_mediaHitLeft.load(std::memory_order_relaxed);
        top = g_mediaHitTop.load(std::memory_order_relaxed);
        right = g_mediaHitRight.load(std::memory_order_relaxed);
        bottom = g_mediaHitBottom.load(std::memory_order_relaxed);
        scale = g_mediaHitScale.load(std::memory_order_relaxed);
        stamp = g_mediaHitStamp.load(std::memory_order_relaxed);

        std::atomic_thread_fence(std::memory_order_acquire);
        if (g_mediaHitSeq.load(std::memory_order_relaxed) == before) {
            break;
        }
        stamp = 0;  // torn; force another attempt (or bail out below)
    }

    if (stamp == 0 || GetTickCount64() - stamp > 500) {
        return out;
    }

    if (scale < 0.05f) {
        scale = 1.0f;
    }

    const float width = right - left;
    const float height = bottom - top;
    if (width <= 1.0f || height <= 1.0f) {
        return out;
    }

    // DrawPill scales content about the pill centre, so undo that about the
    // same point to get back into content space.
    const float centerX = (left + right) * 0.5f;
    const float centerY = (top + bottom) * 0.5f;
    const float contentX = (static_cast<float>(clientX) - centerX) / scale + centerX;
    const float contentY = (static_cast<float>(clientY) - centerY) / scale + centerY;

    out.valid = true;
    out.x = contentX - left;
    out.y = contentY - top;
    out.width = width;
    out.height = height;
    return out;
}

// Returns 0 = previous, 1 = play/pause, 2 = next, or -1 for no button.
int MediaTransportHitTest(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) {
        return -1;
    }

    const float centerX = pt.width * 0.5f;
    const float dy = pt.y - MediaLayout::kControlsY;

    struct Target {
        float offsetX;
        float radius;
        int command;
    };
    const Target targets[] = {
        {-MediaLayout::kControlSpacing, MediaLayout::kNavButtonRadius, 0},
        {0.0f, MediaLayout::kPlayButtonRadius, 1},
        {MediaLayout::kControlSpacing, MediaLayout::kNavButtonRadius, 2},
    };

    for (const Target& t : targets) {
        const float reach = t.radius + MediaLayout::kControlHitPad;
        if (std::fabs(dy) <= reach && std::fabs(pt.x - (centerX + t.offsetX)) <= reach) {
            return t.command;
        }
    }
    return -1;
}

// Fraction along the scrubber bar (0..1) for a content-space point, or -1 when
// the point is not on the bar.
float MediaScrubFractionFromContent(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) {
        return -1.0f;
    }

    const float barLeft = MediaLayout::kScrubInsetLeft;
    const float barRight = pt.width - MediaLayout::kScrubInsetRight;
    if (barRight - barLeft <= 1.0f) {
        return -1.0f;
    }

    if (std::fabs(pt.y - MediaLayout::kScrubberY) > MediaLayout::kScrubHitHalfHeight ||
        pt.x < barLeft - MediaLayout::kScrubHitPadX ||
        pt.x > barRight + MediaLayout::kScrubHitPadX) {
        return -1.0f;
    }

    return Clamp((Clamp(pt.x, barLeft, barRight) - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
}

// Clamped fraction for an ongoing drag, ignoring the on-bar test so the scrub
// keeps tracking once the press has been captured.
float MediaScrubFractionUnbounded(const MediaContentPoint& pt) {
    if (!pt.valid) {
        return -1.0f;
    }
    const float barLeft = MediaLayout::kScrubInsetLeft;
    const float barRight = pt.width - MediaLayout::kScrubInsetRight;
    if (barRight - barLeft <= 1.0f) {
        return -1.0f;
    }
    return Clamp((Clamp(pt.x, barLeft, barRight) - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
}

namespace ClipboardLayout {
    constexpr float kGridLeft = 58.0f;
    constexpr float kGridRightInset = 26.0f;
    constexpr float kGridTop = 12.0f;
    constexpr float kGridBottomInset = 12.0f;
    constexpr float kGutterLeft = 8.0f;
    constexpr float kGutterRight = 48.0f;
    constexpr float kTrashSize = 34.0f;
}

int CalendarHeaderActionAtContentPoint(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight ||
        pt.y < CalendarLayout::kHeaderTop || pt.y > CalendarLayout::kHeaderBottom) {
        return 0;
    }
    const float left = CalendarLayout::kGridLeft;
    const float right = pt.width - CalendarLayout::kGridRightInset;
    if (pt.x >= left && pt.x < left + 24.0f) return 1;        // previous
    if (pt.x >= left + 25.0f && pt.x < left + 132.0f) return 2; // choose month
    if (pt.x >= left + 132.0f && pt.x < right - 25.0f) return 3; // choose year
    if (pt.x >= right - 24.0f && pt.x <= right) return 4;    // next
    return 0;
}

int CalendarPickerCellAtContentPoint(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return -1;
    const float left = CalendarLayout::kGridLeft;
    const float right = pt.width - CalendarLayout::kGridRightInset;
    const float top = CalendarLayout::kGridTop + 8.0f;
    const float bottom = pt.height - CalendarLayout::kDateBottomInset;
    constexpr float gap = 4.0f;
    const float cellW = (right - left - gap * (CalendarLayout::kPickerColumns - 1)) /
                        CalendarLayout::kPickerColumns;
    const float cellH = (bottom - top - gap * (CalendarLayout::kPickerRows - 1)) /
                        CalendarLayout::kPickerRows;
    if (pt.x < left || pt.x >= right || pt.y < top || pt.y >= bottom ||
        cellW <= 0.0f || cellH <= 0.0f) return -1;
    const int column = std::min(CalendarLayout::kPickerColumns - 1,
        static_cast<int>((pt.x - left) / (cellW + gap)));
    const int row = std::min(CalendarLayout::kPickerRows - 1,
        static_cast<int>((pt.y - top) / (cellH + gap)));
    const float localX = pt.x - (left + column * (cellW + gap));
    const float localY = pt.y - (top + row * (cellH + gap));
    if (localX > cellW || localY > cellH) return -1;
    return row * CalendarLayout::kPickerColumns + column;
}

int CalendarDayAtContentPoint(const MediaContentPoint& pt, int year, int month) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return 0;
    const float gridLeft = CalendarLayout::kGridLeft;
    const float colW = (pt.width - CalendarLayout::kGridRightInset - gridLeft) / 7.0f;
    const float gridTop = CalendarLayout::kGridTop;
    const float headerH = CalendarLayout::kWeekdayHeight;
    const float datesTop = gridTop + headerH + 7.0f;
    const float gridBottom = pt.height - CalendarLayout::kDateBottomInset;
    int calcMonth = month;
    int calcYear = year;
    if (calcMonth < 3) { calcMonth += 12; --calcYear; }
    const int k = calcYear % 100;
    const int j = calcYear / 100;
    const int weekday = (1 + 13 * (calcMonth + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    const int startDay = (weekday + 6) % 7;
    const int days = month == 2
        ? (((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28)
        : ((month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31);
    const int rowCount = (startDay + days + 6) / 7;
    const float rowH = (gridBottom - datesTop) / static_cast<float>(rowCount > 0 ? rowCount : 1);
    if (pt.x < gridLeft || pt.x >= gridLeft + 7.0f * colW ||
        pt.y < datesTop || pt.y >= gridBottom || colW <= 0.0f || rowH <= 0.0f) return 0;
    const int column = std::min(6, static_cast<int>((pt.x - gridLeft) / colW));
    const int row = std::min(rowCount - 1, static_cast<int>((pt.y - datesTop) / rowH));
    const int day = row * 7 + column - startDay + 1;
    return day >= 1 && day <= days ? day : 0;
}

int CalendarDaysInMonthForNavigation(int year, int month) {
    if (month == 2) {
        return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28;
    }
    return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

void InitializeCalendarSelection(const SYSTEMTIME& today) {
    if (g_calendarViewYear.load(std::memory_order_relaxed) != 0) return;
    g_calendarViewYear.store(today.wYear, std::memory_order_relaxed);
    g_calendarViewMonth.store(today.wMonth, std::memory_order_relaxed);
    g_calendarSelectedDay.store(today.wDay, std::memory_order_relaxed);
}

void ShiftCalendarViewByMonths(int delta) {
    SYSTEMTIME today = {};
    GetLocalTime(&today);
    InitializeCalendarSelection(today);
    int year = g_calendarViewYear.load(std::memory_order_relaxed);
    int month = g_calendarViewMonth.load(std::memory_order_relaxed);
    int day = g_calendarSelectedDay.load(std::memory_order_relaxed);
    month += delta < 0 ? -1 : 1;
    if (month < 1) { month = 12; year = std::max(1601, year - 1); }
    if (month > 12) { month = 1; year = std::min(9999, year + 1); }
    day = std::min(day, CalendarDaysInMonthForNavigation(year, month));
    g_calendarViewYear.store(year, std::memory_order_relaxed);
    g_calendarViewMonth.store(month, std::memory_order_relaxed);
    g_calendarSelectedDay.store(day, std::memory_order_relaxed);
    g_layoutDirty = true;
}

bool HandleCalendarDashboardClick(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return false;
    SYSTEMTIME today = {};
    GetLocalTime(&today);
    InitializeCalendarSelection(today);

    const int mode = g_calendarPickerMode.load(std::memory_order_relaxed);
    const int action = CalendarHeaderActionAtContentPoint(pt);
    int year = g_calendarViewYear.load(std::memory_order_relaxed);
    int month = g_calendarViewMonth.load(std::memory_order_relaxed);
    int selectedDay = g_calendarSelectedDay.load(std::memory_order_relaxed);
    if (action == 2) {
        g_calendarPickerMode.store(mode == 1 ? 0 : 1, std::memory_order_relaxed);
        g_layoutDirty = true;
        return true;
    }
    if (action == 3) {
        int firstYear = g_calendarYearRangeStart.load(std::memory_order_relaxed);
        if (firstYear == 0) {
            firstYear = ClampInt(year - 5, 1601, 9988);
            g_calendarYearRangeStart.store(firstYear, std::memory_order_relaxed);
        }
        g_calendarPickerMode.store(mode == 2 ? 0 : 2, std::memory_order_relaxed);
        g_layoutDirty = true;
        return true;
    }
    if (action == 1 || action == 4) {
        const int delta = action == 1 ? -1 : 1;
        if (mode == 2) {
            const int firstYear = g_calendarYearRangeStart.load(std::memory_order_relaxed);
            g_calendarYearRangeStart.store(ClampInt(firstYear + delta * 12, 1601, 9988),
                                           std::memory_order_relaxed);
        } else if (mode == 1) {
            g_calendarViewYear.store(ClampInt(year + delta, 1601, 9999),
                                     std::memory_order_relaxed);
        } else {
            int totalMonth = year * 12 + month - 1 + delta;
            totalMonth = ClampInt(totalMonth, 1601 * 12, 9999 * 12 + 11);
            year = totalMonth / 12;
            month = totalMonth % 12 + 1;
            selectedDay = std::min(selectedDay, CalendarDaysInMonthForNavigation(year, month));
            g_calendarViewYear.store(year, std::memory_order_relaxed);
            g_calendarViewMonth.store(month, std::memory_order_relaxed);
            g_calendarSelectedDay.store(selectedDay, std::memory_order_relaxed);
        }
        g_layoutDirty = true;
        return true;
    }

    if (mode != 0) {
        const int cell = CalendarPickerCellAtContentPoint(pt);
        if (cell < 0) return false;
        if (mode == 1) {
            month = cell + 1;
            selectedDay = std::min(selectedDay, CalendarDaysInMonthForNavigation(year, month));
            g_calendarViewMonth.store(month, std::memory_order_relaxed);
            g_calendarSelectedDay.store(selectedDay, std::memory_order_relaxed);
        } else {
            year = g_calendarYearRangeStart.load(std::memory_order_relaxed) + cell;
            year = ClampInt(year, 1601, 9999);
            selectedDay = std::min(selectedDay, CalendarDaysInMonthForNavigation(year, month));
            g_calendarViewYear.store(year, std::memory_order_relaxed);
            g_calendarSelectedDay.store(selectedDay, std::memory_order_relaxed);
        }
        g_calendarPickerMode.store(0, std::memory_order_relaxed);
        g_layoutDirty = true;
        return true;
    }

    const int day = CalendarDayAtContentPoint(pt, year, month);
    if (day > 0) {
        g_calendarSelectedDay.store(day, std::memory_order_relaxed);
        g_layoutDirty = true;
        return true;
    }
    return false;
}

// Volume stays as a small speaker glyph until hovered, then reveals a precise
// slider. The interaction box expands with the visual control so the pointer can
// move onto the slider without causing it to collapse.
bool MediaVolumeHoverHitTest(const MediaContentPoint& pt, bool expanded) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return false;
    const float left = expanded ? pt.width - 208.0f : pt.width - 54.0f;
    return pt.x >= left && pt.x <= pt.width - 16.0f &&
           pt.y >= MediaLayout::kVolumeCenterY - 25.0f &&
           pt.y <= MediaLayout::kVolumeCenterY + 23.0f;
}

float MediaVolumeSliderFraction(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight ||
        !g_hoveredMediaVolume.load(std::memory_order_relaxed)) return -1.0f;
    const float left = pt.width - 176.0f;
    const float right = pt.width - 54.0f;
    if (pt.x < left - 8.0f || pt.x > right + 8.0f ||
        pt.y < MediaLayout::kVolumeCenterY - 13.0f ||
        pt.y > MediaLayout::kVolumeCenterY + 18.0f) {
        return -1.0f;
    }
    return Clamp((Clamp(pt.x, left, right) - left) / (right - left), 0.0f, 1.0f);
}

bool MediaVolumeIconHitTest(const MediaContentPoint& pt) {
    return pt.valid && pt.height > MediaLayout::kExpandedMinHeight &&
           pt.x >= pt.width - 48.0f && pt.x <= pt.width - 14.0f &&
           pt.y >= MediaLayout::kVolumeCenterY - 18.0f &&
           pt.y <= MediaLayout::kVolumeCenterY + 18.0f;
}

int QuickAccessTileAtContentPoint(const MediaContentPoint& pt, size_t count) {
    if (!pt.valid || count == 0 || pt.height <= MediaLayout::kExpandedMinHeight) return -1;
    constexpr float pad = 16.0f;
    constexpr float gap = 8.0f;
    const float left = pad, right = pt.width - pad;
    const float top = 16.0f, bottom = pt.height - 16.0f;
    const float cellW = (right - left - 3.0f * gap) / 4.0f;
    const float cellH = (bottom - top - 2.0f * gap) / 3.0f;
    if (pt.x < left || pt.x >= right || pt.y < top || pt.y >= bottom) return -1;
    const int col = ClampInt(static_cast<int>((pt.x - left) / (cellW + gap)), 0, 3);
    const int row = ClampInt(static_cast<int>((pt.y - top) / (cellH + gap)), 0, 2);
    const float x = left + col * (cellW + gap);
    const float y = top + row * (cellH + gap);
    if (pt.x > x + cellW || pt.y > y + cellH) return -1;
    const int index = row * 4 + col;
    return index < static_cast<int>(count) ? index : -1;
}

void OpenQuickAccessLink(const Settings& settings, size_t index) {
    if (index >= settings.quickAccessLinks.size()) return;
    const std::wstring& url = settings.quickAccessLinks[index].second;
    const wchar_t* browser = settings.quickAccessBrowser == L"chrome" ? L"chrome.exe" :
                             settings.quickAccessBrowser == L"brave" ? L"brave.exe" : nullptr;
    if (browser) {
        const HINSTANCE result = ShellExecuteW(nullptr, L"open", browser, url.c_str(),
                                                nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(result) > 32) return;
    }
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// Index of the File Tray row under a content-space point, or -1. Rows are drawn
// newest-first, so index 0 is the most recently dropped file.
int FileTrayRowAtContentPoint(const MediaContentPoint& pt, int itemCount) {
    if (!pt.valid || itemCount <= 0 || pt.height <= MediaLayout::kExpandedMinHeight) {
        return -1;
    }
    if (pt.x < FileTrayLayout::kPadX || pt.x > pt.width - FileTrayLayout::kPadX) {
        return -1;
    }

    const float listBottom = pt.height - FileTrayLayout::kListBottomInset;
    if (pt.y < FileTrayLayout::kListTop || pt.y > listBottom) {
        return -1;
    }

    const float stride = FileTrayLayout::kRowHeight + FileTrayLayout::kRowGap;
    const int index = static_cast<int>((pt.y - FileTrayLayout::kListTop) / stride);
    // Reject the gap between rows so hovering dead space highlights nothing.
    const float rowTop = FileTrayLayout::kListTop + index * stride;
    if (pt.y > rowTop + FileTrayLayout::kRowHeight) {
        return -1;
    }

    const int capacity = FileTrayLayout::VisibleRowCapacity(pt.height);
    if (index < 0 || index >= capacity || index >= itemCount) {
        return -1;
    }
    return index;
}

bool FileTrayClearButtonAtContentPoint(const MediaContentPoint& pt) {
    if (!pt.valid) return false;
    const float centerY = pt.height * 0.5f;
    return pt.x >= 8.0f && pt.x <= 44.0f &&
           pt.y >= centerY - 19.0f && pt.y <= centerY + 19.0f;
}

bool FileTrayMoreButtonAtContentPoint(const MediaContentPoint& pt, int row) {
    return pt.valid && row >= 0 && pt.x >= pt.width - FileTrayLayout::kPadX - 42.0f &&
           pt.x <= pt.width - FileTrayLayout::kPadX - 2.0f;
}

int ClipboardHistoryItemAtContentPoint(const MediaContentPoint& pt, size_t itemCount, int page) {
    if (!pt.valid || itemCount == 0 || pt.height <= MediaLayout::kExpandedMinHeight) return -1;
    const int columns = ClipboardGridColumns(itemCount);
    const int rows = ClipboardGridRows(itemCount);
    const int pageSize = columns * rows;
    const int pageCount = ClipboardHistoryPageCount(itemCount);
    page = ClampInt(page, 0, pageCount - 1);
    const float gridLeft = ClipboardLayout::kGridLeft;
    const float gridRight = pt.width - ClipboardLayout::kGridRightInset;
    const float gridTop = ClipboardLayout::kGridTop;
    const float gridBottom = pt.height - ClipboardLayout::kGridBottomInset;
    const float gap = itemCount > 4 ? 8.0f : 10.0f;
    const float cellW = (gridRight - gridLeft - gap * (columns - 1)) / columns;
    const float cellH = (gridBottom - gridTop - gap * (rows - 1)) / rows;
    if (pt.x < gridLeft || pt.x >= gridRight || pt.y < gridTop || pt.y >= gridBottom ||
        cellW <= 0.0f || cellH <= 0.0f) return -1;
    const size_t pageStart = static_cast<size_t>(page * pageSize);
    const int visible = static_cast<int>(std::min(static_cast<size_t>(pageSize), itemCount - pageStart));
    for (int slot = 0; slot < visible; ++slot) {
        const int row = slot / columns;
        const int firstInRow = row * columns;
        const int rowCount = std::min(columns, visible - firstInRow);
        const float rowOffset = (columns - rowCount) * (cellW + gap) * 0.5f;
        const float left = gridLeft + rowOffset + (slot % columns) * (cellW + gap);
        const float top = gridTop + row * (cellH + gap);
        if (pt.x >= left && pt.x <= left + cellW && pt.y >= top && pt.y <= top + cellH) {
            return static_cast<int>(pageStart + slot);
        }
    }
    return -1;
}

bool ClipboardClearButtonAtContentPoint(const MediaContentPoint& pt) {
    if (!pt.valid) return false;
    const float centerY = pt.height * 0.5f;
    return pt.x >= ClipboardLayout::kGutterLeft &&
           pt.x <= ClipboardLayout::kGutterRight &&
           pt.y >= centerY - 19.0f && pt.y <= centerY + 19.0f;
}

bool MediaVisualizerHitTest(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return false;
    const float rightInset = MediaLayout::kVisualizerRightCollapsedInset +
        (g_hoveredMediaVolume.load(std::memory_order_relaxed)
             ? MediaLayout::kVisualizerVolumeRevealWidth : 0.0f);
    return pt.x >= MediaLayout::kVisualizerLeft && pt.x <= pt.width - rightInset &&
           pt.y >= MediaLayout::kVisualizerTop - 4.0f &&
           pt.y <= MediaLayout::kVisualizerBottom + 4.0f;
}

// True when the point is over the expanded layout's album art.
bool MediaArtHitTest(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) {
        return false;
    }
    return pt.x >= MediaLayout::kArtInsetX &&
           pt.x <= MediaLayout::kArtInsetX + MediaLayout::kArtSize &&
           pt.y >= MediaLayout::kArtInsetY &&
           pt.y <= MediaLayout::kArtInsetY + MediaLayout::kArtSize;
}

bool EqualsNoCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }

    return true;
}

bool IsClipboardUrl(std::wstring_view value) {
    while (!value.empty() && iswspace(value.front())) value.remove_prefix(1);
    return (value.size() >= 7 && EqualsNoCase(value.substr(0, 7), L"http://")) ||
           (value.size() >= 8 && EqualsNoCase(value.substr(0, 8), L"https://")) ||
           (value.size() >= 6 && EqualsNoCase(value.substr(0, 6), L"ftp://")) ||
           (value.size() >= 4 && EqualsNoCase(value.substr(0, 4), L"www."));
}

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring GetStringSettingWithFallback(PCWSTR primary, PCWSTR fallback, PCWSTR fallback2 = nullptr) {
    std::wstring result = GetStringSettingCopy(primary);
    if (!result.empty()) {
        return result;
    }
    result = GetStringSettingCopy(fallback);
    if (!result.empty()) {
        return result;
    }
    if (fallback2) {
        return GetStringSettingCopy(fallback2);
    }
    return L"";
}



D2D1_COLOR_F ColorFromHex(std::wstring text, D2D1_COLOR_F fallback) {
    // Trim surrounding whitespace: a stray space used to make the whole value
    // fail to parse and silently fall back to the default color.
    const size_t firstChar = text.find_first_not_of(L" \t\r\n");
    if (firstChar == std::wstring::npos) {
        return fallback;
    }
    text = text.substr(firstChar, text.find_last_not_of(L" \t\r\n") - firstChar + 1);

    if (!text.empty() && text[0] == L'#') {
        text.erase(text.begin());
    }

    // Accepted forms: RGB, RGBA, RRGGBB, RRGGBBAA. The alpha-bearing forms are
    // how a translucent island background is specified independently of the
    // global pill transparency slider.
    const size_t digits = text.size();
    if (digits != 3 && digits != 4 && digits != 6 && digits != 8) {
        return fallback;
    }
    if (text.find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos) {
        return fallback;
    }

    auto nibble = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return c - L'A' + 10;
    };

    int r = 0, g = 0, b = 0, a = 255;
    if (digits == 3 || digits == 4) {
        // Shorthand, each digit doubled so 'f' means 0xff.
        r = nibble(text[0]) * 17;
        g = nibble(text[1]) * 17;
        b = nibble(text[2]) * 17;
        if (digits == 4) {
            a = nibble(text[3]) * 17;
        }
    } else {
        r = nibble(text[0]) * 16 + nibble(text[1]);
        g = nibble(text[2]) * 16 + nibble(text[3]);
        b = nibble(text[4]) * 16 + nibble(text[5]);
        if (digits == 8) {
            a = nibble(text[6]) * 16 + nibble(text[7]);
        }
    }

    return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

static float HueToRgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

D2D1_COLOR_F HslToRgb(float h, float s, float l, float a = 1.0f) {
    h = std::fmod(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
    s = Clamp(s, 0.0f, 1.0f);
    l = Clamp(l, 0.0f, 1.0f);

    if (s <= 1e-5f) {
        return D2D1::ColorF(l, l, l, a);
    }

    const float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    const float p = 2.0f * l - q;
    const float hNorm = h / 360.0f;

    const float r = Clamp(HueToRgb(p, q, hNorm + 1.0f / 3.0f), 0.0f, 1.0f);
    const float g = Clamp(HueToRgb(p, q, hNorm), 0.0f, 1.0f);
    const float b = Clamp(HueToRgb(p, q, hNorm - 1.0f / 3.0f), 0.0f, 1.0f);

    return D2D1::ColorF(r, g, b, a);
}

void RgbToHsl(float r, float g, float b, float& h, float& s, float& l) {
    r = Clamp(r, 0.0f, 1.0f);
    g = Clamp(g, 0.0f, 1.0f);
    b = Clamp(b, 0.0f, 1.0f);

    const float maxVal = std::max({r, g, b});
    const float minVal = std::min({r, g, b});
    const float delta = maxVal - minVal;

    l = (maxVal + minVal) * 0.5f;

    if (delta <= 1e-5f) {
        h = 0.0f;
        s = 0.0f;
        return;
    }

    s = (l > 0.5f) ? (delta / (2.0f - maxVal - minVal)) : (delta / (maxVal + minVal));

    if (maxVal == r) {
        h = ((g - b) / delta) + (g < b ? 6.0f : 0.0f);
    } else if (maxVal == g) {
        h = ((b - r) / delta) + 2.0f;
    } else {
        h = ((r - g) / delta) + 4.0f;
    }
    h *= 60.0f;
    if (h < 0.0f) h += 360.0f;
    if (h >= 360.0f) h -= 360.0f;
}

double RelativeLuminance(D2D1_COLOR_F c) {
    auto toLinear = [](float channel) -> double {
        const double v = Clamp(channel, 0.0f, 1.0f);
        return (v <= 0.04045) ? (v / 12.92) : std::pow((v + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * toLinear(c.r) + 0.7152 * toLinear(c.g) + 0.0722 * toLinear(c.b);
}

// Nudges an accent until it clears 3:1 against the surface it will sit on.
//
// This used to only ever *lighten*, which silently assumed a dark island. On a
// light background lightening reduces contrast, so the loop would run all the
// way to l = 0.95 and hand back a near-white accent that vanished into the
// surface. The step direction is now chosen from the background's luminance,
// which is what lets a light theme (Porcelain) keep a readable accent.
D2D1_COLOR_F EnsureContrastAgainstBackground(D2D1_COLOR_F candidate, D2D1_COLOR_F bgColor) {
    float h = 0.0f, s = 0.0f, l = 0.0f;
    RgbToHsl(candidate.r, candidate.g, candidate.b, h, s, l);

    if (s > 0.01f) {
        s = std::max(s, 0.35f);
    }

    const double bgLum = RelativeLuminance(bgColor);
    const bool onDark = bgLum < 0.45;

    // Clamp toward the half that can actually move away from the background.
    l = onDark ? std::min(l, 0.85f) : std::max(l, 0.15f);

    const float step = onDark ? 0.02f : -0.02f;
    const float limit = onDark ? 0.95f : 0.06f;

    candidate = HslToRgb(h, s, l, candidate.a);
    auto contrastOf = [&](const D2D1_COLOR_F& c) {
        const double lum = RelativeLuminance(c);
        return (std::max(bgLum, lum) + 0.05) / (std::min(bgLum, lum) + 0.05);
    };

    double contrast = contrastOf(candidate);
    // Bounded by `limit` in both directions, so this terminates either way.
    while (contrast < 3.0 && (onDark ? (l < limit) : (l > limit))) {
        l += step;
        candidate = HslToRgb(h, s, l, candidate.a);
        contrast = contrastOf(candidate);
    }

    return candidate;
}

// Returns true if the currently active foreground window is in full screen mode
bool IsForegroundFullscreen(HWND targetHwnd) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == targetHwnd || fg == GetDesktopWindow() || fg == GetShellWindow()) {
        return false;
    }

    if (!IsWindowVisible(fg)) return false;

    wchar_t className[256] = {};
    GetClassNameW(fg, className, 256);
    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0) {
        return false;
    }

    RECT clientRect = {};
    if (!GetClientRect(fg, &clientRect)) return false;
    POINT pt = {0, 0};
    ClientToScreen(fg, &pt);
    clientRect.left += pt.x;
    clientRect.right += pt.x;
    clientRect.top += pt.y;
    clientRect.bottom += pt.y;

    HMONITOR hMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (GetMonitorInfoW(hMon, &mi)) {
        if (clientRect.left <= mi.rcMonitor.left &&
            clientRect.top <= mi.rcMonitor.top &&
            clientRect.right >= mi.rcMonitor.right &&
            clientRect.bottom >= mi.rcMonitor.bottom) {
            return true;
        }
    }

    return false;
}

// Returns the DPI scale factor for the primary monitor (1.0 = 96 DPI = 100%)
float GetPrimaryMonitorDpiScale() {
    POINT pt = {0, 0};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
        GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor"));
    if (pGetDpiForMonitor) {
        pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY);
    }
    return static_cast<float>(dpiX) / 96.0f;
}

int GetMonitorRefreshRate(HWND hwnd) {
    HMONITOR monitor = nullptr;
    if (hwnd) {
        monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    } else {
        POINT pt = {0, 0};
        monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(monitor, &mi)) {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
            if (dm.dmDisplayFrequency > 1) {
                return static_cast<int>(dm.dmDisplayFrequency);
            }
        }
    }
    HDC hdc = GetDC(nullptr);
    int rate = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(nullptr, hdc);
    return (rate > 1) ? rate : 60;
}

D2D1_COLOR_F GetSystemAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    using DwmGetColorizationColor_t = HRESULT(WINAPI*)(DWORD*, BOOL*);
    auto proc = reinterpret_cast<DwmGetColorizationColor_t>(
        GetProcAddress(GetModuleHandleW(L"dwmapi.dll"), "DwmGetColorizationColor"));

    if (proc && SUCCEEDED(proc(&color, &opaque))) {
        return D2D1::ColorF(
            ((color >> 16) & 0xff) / 255.0f,
            ((color >> 8) & 0xff) / 255.0f,
            (color & 0xff) / 255.0f,
            1.0f);
    }

    return D2D1::ColorF(0x4cc9f0);
}

void ApplyHideShowHotkey();                 // forward declaration; defined after LoadSettings()
void ApplyBackdropMaterial(HWND);           // forward declaration; defined after LoadSettings()
void ApplyBackdropRegion(HWND, int, int);   // forward declaration; defined below PositionOverlayWindow
void NotifyKeyboardThreadSettingChanged();  // forward declaration; defined with the keyboard hook


void LoadSettings() {
    Settings next;

    wchar_t rememberedMediaSource[512] = {};
    if (Wh_GetStringValue(L"LastMediaSourceId", rememberedMediaSource,
                          ARRAYSIZE(rememberedMediaSource)) > 0) {
        std::lock_guard lock(g_mediaSourceMutex);
        g_preferredMediaSourceId = rememberedMediaSource;
    }

    const std::wstring position = GetStringSettingCopy(L"Appearance.Position");
    if (EqualsNoCase(position, L"top-left")) {
        next.position = Position::TopLeft;
    } else if (EqualsNoCase(position, L"top-right")) {
        next.position = Position::TopRight;
    } else if (EqualsNoCase(position, L"bottom-center")) {
        next.position = Position::BottomCenter;
    } else if (EqualsNoCase(position, L"bottom-left")) {
        next.position = Position::BottomLeft;
    } else if (EqualsNoCase(position, L"bottom-right")) {
        next.position = Position::BottomRight;
    }

    const std::wstring scale = GetStringSettingCopy(L"Appearance.SizeScale");
    if (!scale.empty()) {
        wchar_t* end;
        float parsedScale = wcstof(scale.c_str(), &end);
        if (end != scale.c_str() && parsedScale > 0.1f && parsedScale < 10.0f) {
            next.sizeScale = parsedScale;
        }
    }

    next.islandWidth = static_cast<float>(ClampInt(Wh_GetIntSetting(L"Appearance.IslandWidth"), 80, 420));
    next.islandHeight = static_cast<float>(ClampInt(Wh_GetIntSetting(L"Appearance.IslandHeight"), 32, 100));
    next.expandedWidth = static_cast<float>(ClampInt(Wh_GetIntSetting(L"Appearance.ExpandedWidth"), 380, 900));
    next.expandedHeight = static_cast<float>(ClampInt(Wh_GetIntSetting(L"Appearance.ExpandedHeight"), 184, 600));
    next.cornerRadius = static_cast<float>(ClampInt(Wh_GetIntSetting(L"Appearance.CornerRadius"), 4, 100));

    // Auto DPI scaling: multiply sizeScale by monitor DPI factor.
    // On a 4K 200% display this doubles the island to the right physical size.
    if (Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0) {
        next.sizeScale *= GetPrimaryMonitorDpiScale();
    }

    std::wstring fontSetting = GetStringSettingWithFallback(L"Themes.FontFamily", L"Appearance.FontFamily");
    size_t firstChar = fontSetting.find_first_not_of(L" \t\r\n\"'");
    if (firstChar != std::wstring::npos) {
        size_t lastChar = fontSetting.find_last_not_of(L" \t\r\n\"'");
        next.fontFamily = fontSetting.substr(firstChar, lastChar - firstChar + 1);
    } else {
        next.fontFamily.clear();
    }

    const std::wstring accentMode = GetStringSettingCopy(L"Themes.AccentColorMode");
    if (EqualsNoCase(accentMode, L"system")) {
        next.accentMode = AccentMode::System;
    } else if (EqualsNoCase(accentMode, L"custom")) {
        next.accentMode = AccentMode::Custom;
    }

    next.customAccent = ColorFromHex(GetStringSettingCopy(L"Themes.CustomAccentHex"), next.customAccent);

    const std::wstring calAccent = GetStringSettingWithFallback(L"Themes.CalendarAccent", L"CalendarWeather.CalendarAccent", L"Modules.CalendarAccent");
    if (EqualsNoCase(calAccent, L"system")) {
        next.calendarAccent = CalendarAccentMode::System;
    } else {
        next.calendarAccent = CalendarAccentMode::Red;
    }

    const std::wstring fpsStr = GetStringSettingWithFallback(L"Animations.TargetFPS", L"Appearance.TargetFPS");
    if (EqualsNoCase(fpsStr, L"auto") || fpsStr.empty()) {
        next.targetFps = 0;
    } else {
        next.targetFps = _wtoi(fpsStr.c_str());
        if (next.targetFps < 0) next.targetFps = 0;
    }

    const std::wstring styleStr = GetStringSettingWithFallback(L"Animations.AnimationStyle", L"Appearance.AnimationStyle");
    if (EqualsNoCase(styleStr, L"smooth")) {
        next.animationStyle = AnimationStyle::Smooth;
    } else if (EqualsNoCase(styleStr, L"bouncy")) {
        next.animationStyle = AnimationStyle::Bouncy;
    } else if (EqualsNoCase(styleStr, L"snappy")) {
        next.animationStyle = AnimationStyle::Snappy;
    } else {
        next.animationStyle = AnimationStyle::Default;
    }

    const std::wstring speed = GetStringSettingWithFallback(L"Animations.AnimationSpeed", L"Appearance.AnimationSpeed", L"Behavior.AnimationSpeed");
    if (EqualsNoCase(speed, L"very-slow")) {
        next.animationSpeed = 0.5f;
    } else if (EqualsNoCase(speed, L"slow")) {
        next.animationSpeed = 0.75f;
    } else if (EqualsNoCase(speed, L"fast")) {
        next.animationSpeed = 1.35f;
    } else if (EqualsNoCase(speed, L"very-fast")) {
        next.animationSpeed = 1.65f;
    } else if (EqualsNoCase(speed, L"ultra-fast")) {
        next.animationSpeed = 2.0f;
    } else {
        next.animationSpeed = 1.0f;
    }

    next.respectReducedMotion = Wh_GetIntSetting(L"Animations.RespectReducedMotion") != 0;
    if (next.respectReducedMotion) {
        BOOL clientAnimations = TRUE;
        if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &clientAnimations, 0)) {
            next.reduceMotion = clientAnimations == FALSE;
        }
    }

    next.media = Wh_GetIntSetting(L"Modules.Media") != 0;
    next.miniVideoPlayer = Wh_GetIntSetting(L"Modules.MiniVideoPlayer") != 0;
    next.musicVisualizer = Wh_GetIntSetting(L"Modules.MusicVisualizer") != 0;
    next.visualizerStrength = ClampInt(Wh_GetIntSetting(L"Modules.VisualizerStrength"), 10, 200);
    next.mediaAutoExpand = Wh_GetIntSetting(L"Modules.MediaAutoExpand") != 0;
    next.mediaAutoExpandHotkey = Wh_GetIntSetting(L"Modules.MediaAutoExpandHotkey") != 0;
    next.windowsNotifications = Wh_GetIntSetting(L"Modules.WindowsNotifications") != 0;
    next.notificationDurationSeconds = ClampInt(
        Wh_GetIntSetting(L"Modules.NotificationDuration"), 1, 30);
    next.quickAccess = Wh_GetIntSetting(L"Modules.QuickAccess") != 0;
    next.quickAccessBrowser = GetStringSettingCopy(L"Modules.QuickAccessBrowser");
    if (next.quickAccessBrowser != L"chrome" && next.quickAccessBrowser != L"brave") {
        next.quickAccessBrowser = L"default";
    }
    next.quickAccessLinks.clear();
    {
        const std::wstring raw = GetStringSettingCopy(L"Modules.QuickAccessLinks");
        size_t start = 0;
        while (start < raw.size() && next.quickAccessLinks.size() < 12) {
            const size_t comma = raw.find(L',', start);
            const size_t end = comma == std::wstring::npos ? raw.size() : comma;
            const std::wstring token = raw.substr(start, end - start);
            const size_t split = token.find(L'|');
            if (split != std::wstring::npos && split > 0 && split + 1 < token.size()) {
                std::wstring label = token.substr(0, split);
                std::wstring url = token.substr(split + 1);
                if (url.find(L"://") != std::wstring::npos) {
                    next.quickAccessLinks.emplace_back(std::move(label), std::move(url));
                }
            }
            if (comma == std::wstring::npos) break;
            start = comma + 1;
        }
    }
    next.videoAppIds.clear();
    {
        const std::wstring raw = GetStringSettingCopy(L"Modules.VideoAppIds");
        size_t start = 0;
        while (start <= raw.size()) {
            const size_t comma = raw.find(L',', start);
            const size_t end = comma == std::wstring::npos ? raw.size() : comma;
            std::wstring token = raw.substr(start, end - start);
            const size_t first = token.find_first_not_of(L" \t\r\n");
            if (first != std::wstring::npos) {
                const size_t last = token.find_last_not_of(L" \t\r\n");
                token = token.substr(first, last - first + 1);
                std::transform(token.begin(), token.end(), token.begin(),
                               [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
                next.videoAppIds.push_back(std::move(token));
            }
            if (comma == std::wstring::npos) break;
            start = comma + 1;
        }
    }
    next.volume = Wh_GetIntSetting(L"Modules.Volume") != 0;
    next.calendarModule = Wh_GetIntSetting(L"Modules.CalendarModule") != 0;
    next.dashboardOrder.clear();
    {
        const std::wstring raw = GetStringSettingCopy(L"Modules.SectionOrder");
        size_t start = 0;
        while (start <= raw.size()) {
            const size_t comma = raw.find(L',', start);
            const size_t end = comma == std::wstring::npos ? raw.size() : comma;
            std::wstring name = raw.substr(start, end - start);
            const size_t first = name.find_first_not_of(L" \t\r\n");
            if (first != std::wstring::npos) {
                const size_t last = name.find_last_not_of(L" \t\r\n");
                name = name.substr(first, last - first + 1);
                int id = -1;
                if (EqualsNoCase(name, L"time")) id = 0;
                else if (EqualsNoCase(name, L"video")) id = 1;
                else if (EqualsNoCase(name, L"clipboard")) id = 2;
                else if (EqualsNoCase(name, L"calendar")) id = 3;
                else if (EqualsNoCase(name, L"hardware")) id = 4;
                else if (EqualsNoCase(name, L"filetray")) id = 5;
                else if (EqualsNoCase(name, L"audio")) id = 6;
                else if (EqualsNoCase(name, L"quickaccess")) id = 8;
                if (id >= 0 && std::find(next.dashboardOrder.begin(),
                                         next.dashboardOrder.end(), id) == next.dashboardOrder.end()) {
                    next.dashboardOrder.push_back(id);
                }
            }
            if (comma == std::wstring::npos) break;
            start = comma + 1;
        }
        // Append omitted IDs in a stable default order. This keeps a shortened
        // custom string from making an enabled dashboard unreachable.
        constexpr int fallbackOrder[] = {0, 2, 3, 4, 5, 8, 1, 6};
        for (int id : fallbackOrder) {
            if (std::find(next.dashboardOrder.begin(), next.dashboardOrder.end(), id) ==
                next.dashboardOrder.end()) {
                next.dashboardOrder.push_back(id);
            }
        }
    }
    next.fileCopyNotifications = Wh_GetIntSetting(L"Modules.FileCopyNotifications") != 0;
    next.browserDownloadNotifications = Wh_GetIntSetting(L"Modules.BrowserDownloadNotifications") != 0;
    if (!next.volume) {
        std::lock_guard lock(g_stateMutex);
        g_state.volume.active = false;
    }
    next.clipboard = Wh_GetIntSetting(L"Modules.Clipboard") != 0;
    next.clipboardSection = Wh_GetIntSetting(L"Modules.ClipboardSection") != 0;
    next.clipboardHistoryLimit = ClampInt(Wh_GetIntSetting(L"Modules.ClipboardHistoryLimit"), 1, 1000);
    next.statusCountdownProgress = Wh_GetIntSetting(L"Modules.StatusCountdownProgress") != 0;

    next.battery = Wh_GetIntSetting(L"Modules.Battery") != 0;
    next.progress = Wh_GetIntSetting(L"Modules.Progress") != 0;
    next.capsLock = Wh_GetIntSetting(L"Modules.CapsLock") != 0;
    next.timerEnabled = false;  // Focus Timer is not part of Dynamic Island Pro.

    next.hideShowHotkeyEnabled = Wh_GetIntSetting(L"Shortcuts.HideShowHotkeyEnabled") != 0;

    next.hideShowModifiers = MOD_CONTROL | MOD_ALT;
    next.hideShowModifiers |= MOD_NOREPEAT;

    // Single A-Z/0-9 key only; anything else (blank, multi-char, symbol) falls back to 'D'.
    const std::wstring hotkeyKeyStr = GetStringSettingCopy(L"Shortcuts.HideShowKey");
    next.hideShowVk = 'D';
    if (hotkeyKeyStr.size() == 1) {
        const wchar_t ch = towupper(hotkeyKeyStr[0]);
        if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
            next.hideShowVk = static_cast<UINT>(ch);
        }
    }

    next.bluetoothIndicator = Wh_GetIntSetting(L"Modules.BluetoothIndicator") != 0;
    next.bluetoothShowBattery = Wh_GetIntSetting(L"Modules.BluetoothShowBattery") != 0;
    next.doNotDisturbIndicator = Wh_GetIntSetting(L"Modules.DoNotDisturbIndicator") != 0;
    next.notificationRespectDnD = Wh_GetIntSetting(L"Modules.NotificationRespectDnD") != 0;

    // ── Premium material / redesign ──────────────────────────────────────────
    {
        const std::wstring backdrop = GetStringSettingCopy(L"Themes.BackdropMaterial");
        if (EqualsNoCase(backdrop, L"acrylic")) {
            next.backdropMaterial = BackdropMaterial::Acrylic;
        } else if (EqualsNoCase(backdrop, L"blur")) {
            next.backdropMaterial = BackdropMaterial::Blur;
        } else {
            next.backdropMaterial = BackdropMaterial::None;
        }
    }
    next.backdropTint = Clamp(Wh_GetIntSetting(L"Themes.BackdropTint") / 100.0f, 0.0f, 1.0f);
    next.backdropFillAlpha = Clamp(Wh_GetIntSetting(L"Themes.BackdropFillOpacity") / 100.0f, 0.0f, 1.0f);

    next.materialDepth = Wh_GetIntSetting(L"Themes.MaterialDepth") != 0;
    next.dropShadow = Wh_GetIntSetting(L"Themes.DropShadow") != 0;
    next.accentBloom = Clamp(Wh_GetIntSetting(L"Themes.AccentBloom") / 100.0f, 0.0f, 2.0f);
    next.textScale = Clamp(Wh_GetIntSetting(L"Themes.TextScale") / 100.0f, 0.7f, 1.6f);

    // ── Clock / date presentation (#61) ──────────────────────────────────────
    next.showSeconds = Wh_GetIntSetting(L"Modules.ShowSeconds") != 0;
    const std::wstring clockMode = GetStringSettingCopy(L"Modules.ClockFormat");
    next.clockFollowSystem = clockMode.empty() || EqualsNoCase(clockMode, L"system");
    next.use24HourClock = EqualsNoCase(clockMode, L"24h");
    next.dateFormat = GetStringSettingCopy(L"Modules.DateFormat");
    next.dateFirst = Wh_GetIntSetting(L"Modules.DateFirst") != 0;

    // ── Localization (#35) ───────────────────────────────────────────────────
    {
        std::wstring langTag = GetStringSettingCopy(L"Modules.Language");
        if (langTag.empty()) {
            langTag = L"auto";
        }
        next.language = langTag;
        const UiLanguage resolved = EqualsNoCase(langTag, L"auto") ? DetectSystemLanguage()
                                                                  : LanguageFromTag(langTag);
        g_uiLanguage.store(static_cast<int>(resolved), std::memory_order_relaxed);
    }

    // ── File tray (#33) ──────────────────────────────────────────────────────
    next.fileTrayModule = Wh_GetIntSetting(L"Modules.FileTrayModule") != 0;
    next.fileTrayMaxItems = ClampInt(Wh_GetIntSetting(L"Modules.FileTrayMaxItems"), 1, 25);

    // ── Media auto-expand exclusions (#62) ───────────────────────────────────
    next.mediaExpandBlocklist.clear();
    {
        const std::wstring raw = GetStringSettingCopy(L"Modules.MediaExpandBlocklist");
        size_t start = 0;
        while (start <= raw.size()) {
            const size_t comma = raw.find(L',', start);
            const size_t end = (comma == std::wstring::npos) ? raw.size() : comma;
            std::wstring token = raw.substr(start, end - start);
            const size_t a = token.find_first_not_of(L" \t\r\n");
            if (a != std::wstring::npos) {
                const size_t b = token.find_last_not_of(L" \t\r\n");
                std::wstring entry = token.substr(a, b - a + 1);
                // Stored lowercase so matching against media source/title is
                // case-insensitive without re-lowering on every frame.
                std::transform(entry.begin(), entry.end(), entry.begin(),
                               [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
                next.mediaExpandBlocklist.push_back(std::move(entry));
            }
            if (comma == std::wstring::npos) {
                break;
            }
            start = comma + 1;
        }
    }

    // Game Overlay and Weather are intentionally not exposed in this variant.
    next.gameOverlay = false;
    next.weather = false;
    next.tintOpacity = Clamp(Wh_GetIntSetting(L"Themes.TintIntensity") / 100.0f, 0.0f, 1.0f);
    const int settingOpacity = Wh_GetIntSetting(L"Themes.PillOpacity");
    const int localOpacity = Wh_GetIntValue(L"PillOpacityOverride", -1);
    next.pillOpacity = Clamp((localOpacity >= 0 ? localOpacity : settingOpacity) / 100.0f,
                             0.35f, 1.0f);
    next.showMetricText = false;
    next.weatherCity.clear();
    next.weatherFahrenheit = false;
    const std::wstring hideSec = GetStringSettingWithFallback(L"Behavior.AutoHideIdleSeconds", L"Appearance.AutoHideIdleSeconds");
    next.autoHideIdleSeconds = hideSec.empty() ? 0 : _wtoi(hideSec.c_str());
    next.unhideOnHover = Wh_GetIntSetting(L"Behavior.UnhideOnHover") != 0;
    next.alwaysOnTop = Wh_GetIntSetting(L"Behavior.AlwaysOnTop") != 0;
    const int localExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1);
    next.expandOnHover = localExpandOnHover >= 0 ? (localExpandOnHover != 0) : (Wh_GetIntSetting(L"Behavior.ExpandOnHover") != 0);
    next.autoDpiScale = Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0;
    next.offsetX = Wh_GetIntSetting(L"Appearance.OffsetX");
    next.offsetY = Wh_GetIntSetting(L"Appearance.OffsetY");
    next.separateExpandedOffsetY = Wh_GetIntSetting(L"Appearance.SeparateExpandedOffsetY") != 0;
    next.offsetYExpanded = Wh_GetIntSetting(L"Appearance.OffsetYExpanded");

    std::wstring mon = GetStringSettingCopy(L"Appearance.TargetMonitor");
    if (mon == L"primary") next.targetMonitor = 0;
    else if (mon == L"follow") next.targetMonitor = -1;
    else next.targetMonitor = _wtoi(mon.c_str());

    std::wstring shapeStr = GetStringSettingCopy(L"Appearance.ShapeStyle");
    static std::wstring s_lastConfiguredShape = L"";
    if (!shapeStr.empty() && shapeStr != s_lastConfiguredShape) {
        s_lastConfiguredShape = shapeStr;
        Wh_SetIntValue(L"W11StyleOverride", -1);
        Wh_SetIntValue(L"NotchStyleOverride", -1);
    }
    bool baseW11 = EqualsNoCase(shapeStr, L"w11");
    bool baseNotch = EqualsNoCase(shapeStr, L"notch");

    const int localW11Style = Wh_GetIntValue(L"W11StyleOverride", -1);
    next.w11Style = localW11Style >= 0 ? (localW11Style != 0) : baseW11;

    const int localNotchStyle = Wh_GetIntValue(L"NotchStyleOverride", -1);
    next.notchStyle = localNotchStyle >= 0 ? (localNotchStyle != 0) : baseNotch;

    // A bottom-anchored island cannot be a top-edge macOS notch.
    if (IsBottomPosition(next.position)) {
        next.notchStyle = false;
    }

    // Color settings — check local theme override first, then settings YAML.
    // Palettes live in kThemePalettes at file scope so the right-click menu
    // resolves the same colors and labels this does.
    //
    // A custom hex field counts as an intentional override once it differs from
    // the value shipped as its default. Presets keep working untouched for
    // anyone who never edits these fields, but an edited hex now takes effect
    // immediately instead of being silently discarded unless the Theme preset
    // also happened to be switched to Custom. Clearing the field back to its
    // default hands control back to the preset.
    //
    // `defaultHexes` deliberately lists the *historical* defaults as well as the
    // current one. Retiring the old palettes moved these defaults, and Windhawk
    // keeps whatever value is already stored in a user's config -- so someone who
    // never touched the hex fields would still be carrying "#0D0D0F" from the old
    // OLED Black default. Matching only the current default would read that as a
    // deliberate override and pin every new palette back to the old background.
    auto resolveColor = [](const wchar_t* settingKey,
                           std::initializer_list<const wchar_t*> defaultHexes,
                           D2D1_COLOR_F presetColor) -> D2D1_COLOR_F {
        const std::wstring value = GetStringSettingCopy(settingKey);
        if (value.empty()) {
            return presetColor;
        }

        // Compare the parsed colors rather than the strings, so equivalent
        // spellings of the default ("0D0D0F" without the '#', "#0D0D0FFF",
        // different case) are all still recognised as "untouched" and leave the
        // preset in charge.
        const D2D1_COLOR_F sentinel = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
        const D2D1_COLOR_F parsed = ColorFromHex(value, sentinel);
        if (parsed.r == sentinel.r && parsed.g == sentinel.g &&
            parsed.b == sentinel.b && parsed.a == sentinel.a) {
            // Unparseable, so treat it as not set.
            return presetColor;
        }

        for (const wchar_t* defaultHex : defaultHexes) {
            const D2D1_COLOR_F defaults = ColorFromHex(defaultHex, sentinel);
            if (parsed.r == defaults.r && parsed.g == defaults.g &&
                parsed.b == defaults.b && parsed.a == defaults.a) {
                return presetColor;
            }
        }
        return parsed;
    };

    auto applyPreset = [&](Settings& target, int idx) {
        const ThemePalette& p = kThemePalettes[idx];
        target.pillBgColor = resolveColor(
            L"Themes.PillBgColor", {kThemePalettes[0].bg, L"#0D0D0F"},
            ColorFromHex(p.bg, D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f)));
        target.textPrimaryColor = resolveColor(
            L"Themes.TextPrimaryColor", {L"#FFFFFF", L"#F7F7F7"},
            ColorFromHex(p.fg, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));
        target.textSecondaryColor = resolveColor(
            L"Themes.TextSecondaryColor", {kThemePalettes[0].sec, L"#B0B0B8", L"#888888"},
            ColorFromHex(p.sec, D2D1::ColorF(0.608f, 0.608f, 0.647f, 1.0f)));
        target.contourBorderColor = resolveColor(
            L"Themes.ContourBorderHex", {kThemePalettes[0].border, L"#333338"},
            ColorFromHex(p.border, D2D1::ColorF(0.118f, 0.118f, 0.133f, 1.0f)));
    };

    std::wstring themePresetStr = GetStringSettingCopy(L"Themes.ThemePreset");
    // The active palette is persisted as an integer so the right-click menu can
    // change it without rewriting the settings file. Migrate the old key across
    // once, since the integers were renumbered when the palettes changed.
    if (Wh_GetIntValue(kThemeValueName, -1) < 0) {
        const int legacy = Wh_GetIntValue(kLegacyThemeValueName, -1);
        if (legacy >= 0) {
            int migrated;
            switch (legacy) {
                case 0:  migrated = 0; break;                  // OLED Black    -> Obsidian
                case 1:  migrated = 1; break;                  // Fluent        -> Graphite
                case 2:  migrated = 2; break;                  // Midnight Blue -> Slate
                case 3:  migrated = 6; break;                  // Deep Purple   -> Plum
                case 4:  migrated = 1; break;                  // Fluent Design -> Graphite
                default: migrated = kCustomThemeIndex; break;   // out of range meant Custom
            }
            Wh_SetIntValue(kThemeValueName, migrated);
        }
    }

    static std::wstring s_lastConfiguredPreset = L"";
    if (!themePresetStr.empty() && themePresetStr != s_lastConfiguredPreset) {
        s_lastConfiguredPreset = themePresetStr;
        const int fromSettings = ThemeIndexFromId(themePresetStr);
        if (fromSettings >= 0) {
            Wh_SetIntValue(kThemeValueName, fromSettings);
        }
    }

    const int theme = Wh_GetIntValue(kThemeValueName, -1);
    if (theme >= 0 && theme < kCustomThemeIndex) {
        next.themePreset = static_cast<ThemePreset>(theme);
        applyPreset(next, theme);
    } else if (theme >= kCustomThemeIndex || EqualsNoCase(themePresetStr, L"custom")) {
        // Explicit Custom: the hex fields are authoritative, defaults included.
        next.themePreset = ThemePreset::Custom;
        next.pillBgColor = ColorFromHex(GetStringSettingCopy(L"Themes.PillBgColor"),
                                        D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f));
        next.textPrimaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextPrimaryColor"),
                                             D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
        next.textSecondaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextSecondaryColor"),
                                               D2D1::ColorF(0.608f, 0.608f, 0.647f, 1.0f));
        next.contourBorderColor = ColorFromHex(GetStringSettingCopy(L"Themes.ContourBorderHex"),
                                               D2D1::ColorF(0.118f, 0.118f, 0.133f, 1.0f));
    } else {
        const int fromSettings = ThemeIndexFromId(themePresetStr);
        const int presetIdx = (fromSettings >= 0 && fromSettings < kCustomThemeIndex) ? fromSettings : 0;
        next.themePreset = static_cast<ThemePreset>(presetIdx);
        applyPreset(next, presetIdx);
    }

    if (next.themePreset == ThemePreset::LiquidGlass) {
        // Keep the glass treatment inside the rounded D2D surface. DWM acrylic
        // applies to the overlay window's full expanded bounds, leaving a large
        // rectangular blur behind the smaller collapsed island.
        next.backdropMaterial = BackdropMaterial::None;
        next.backdropTint = 0.0f;
        next.backdropFillAlpha = 0.0f;
        next.materialDepth = true;
        next.dropShadow = true;
        next.accentBloom = std::max(next.accentBloom, 1.15f);
        if (localOpacity < 0 && settingOpacity == 96) {
            next.pillOpacity = 0.70f;
        }
    }

    // Graphite inherits the old Fluent theme's auto-translucency: it is the
    // neutral Windows 11 grey, and it reads best when the desktop shows through
    // a little. The rest of the palettes are opaque unless the user says so.
    if (next.themePreset == ThemePreset::Graphite && localOpacity < 0) {
        next.pillOpacity = 0.88f;
    }

    next.borderMergedMode = Wh_GetIntSetting(L"Appearance.BorderMergedMode") != 0;
    next.autoHideFullscreen = Wh_GetIntSetting(L"Behavior.AutoHideFullscreen") != 0;
    g_autoHideEnabled.store(Wh_GetIntValue(L"AutoHideEnabled", 1) != 0,
                            std::memory_order_relaxed);
    next.hardwareMonitorModule = Wh_GetIntSetting(L"Modules.HardwareMonitorModule") != 0;
    const std::wstring borderModeStr = GetStringSettingCopy(L"Themes.ContourBorderMode");
    if (EqualsNoCase(borderModeStr, L"borderless")) {
        next.contourBorderMode = ContourBorderMode::Borderless;
        next.contourBorderEnabled = false;
    } else if (EqualsNoCase(borderModeStr, L"auto")) {
        next.contourBorderMode = ContourBorderMode::Auto;
        next.contourBorderEnabled = true;
    } else if (EqualsNoCase(borderModeStr, L"default")) {
        next.contourBorderMode = ContourBorderMode::Default;
        next.contourBorderEnabled = true;
    } else {
        next.contourBorderEnabled = true;  // no such setting; the mode dropdown drives this
        next.contourBorderMode = next.contourBorderEnabled ? ContourBorderMode::Default : ContourBorderMode::Borderless;
    }
    next.clockAccentGlow = Wh_GetIntSetting(L"Themes.ClockAccentGlow") != 0;
    bool settingsChangedWhileHidden = (g_autoHiddenParked.load() || g_manuallyHidden.load());
    bool unhideRequested = !g_autoHideEnabled.load(std::memory_order_relaxed) ||
                           (next.autoHideIdleSeconds == 0) ||
                           (next.unhideOnHover && !g_settings.unhideOnHover);
    if (unhideRequested || (settingsChangedWhileHidden && next.autoHideIdleSeconds == 0)) {
        g_autoHiddenParked = false;
        g_manuallyHidden = false;
        Wh_SetIntValue(L"ManuallyHidden", 0);
        g_hotkeyUnhideUntil.store(NowSeconds() + (next.autoHideIdleSeconds > 0 ? next.autoHideIdleSeconds : 6.0));
        if (g_hwnd) {
            ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
            PostMessageW(g_hwnd, WM_APP_NEW_EVENT, 0, 0);
        }
    } else if (settingsChangedWhileHidden && next.unhideOnHover) {
        g_autoHiddenParked = false;
        g_manuallyHidden = false;
        Wh_SetIntValue(L"ManuallyHidden", 0);
        g_hotkeyUnhideUntil.store(NowSeconds() + (next.autoHideIdleSeconds > 0 ? next.autoHideIdleSeconds : 6.0));
        if (g_hwnd) {
            ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
            PostMessageW(g_hwnd, WM_APP_NEW_EVENT, 0, 0);
        }
    }

    // Privacy Indicators: unified reading with complete fallback to legacy keys
    next.privacyDots = Wh_GetIntSetting(L"Indicators.PrivacyDots") != 0;
    next.privacyDotsMic = Wh_GetIntSetting(L"Indicators.PrivacyDotsMic") != 0;
    next.privacyDotsCam = Wh_GetIntSetting(L"Indicators.PrivacyDotsCam") != 0;
    next.privacyDotsPulse = true;  // no such setting; pulsing is always on
    next.rememberLastSection = Wh_GetIntSetting(L"Behavior.RememberLastSection") != 0;

    std::wstring micHexStr = GetStringSettingWithFallback(L"Indicators.PrivacyDotsMicHex", L"Indicators.MicDotHex", L"Modules.PrivacyDotsMicHex");
    next.privacyDotsMicHex = ColorFromHex(micHexStr, D2D1::ColorF(1.0f, 0.584f, 0.0f, 1.0f));

    std::wstring camHexStr = GetStringSettingWithFallback(L"Indicators.PrivacyDotsCamHex", L"Indicators.CamDotHex", L"Modules.PrivacyDotsCamHex");
    next.privacyDotsCamHex = ColorFromHex(camHexStr, D2D1::ColorF(0.204f, 0.780f, 0.349f, 1.0f));

    // Compatibility aliases: always keep in sync
    next.privacyDotsEnabled = next.privacyDots;
    next.privacyDotPulsing = next.privacyDotsPulse;
    next.micDotColor = next.privacyDotsMicHex;
    next.camDotColor = next.privacyDotsCamHex;

    Wh_SetIntValue(L"PinnedExpanded", 0);

    const int lastSectionId = next.rememberLastSection
                                  ? Wh_GetIntValue(L"LastDashboardSection", 0)
                                  : 0;
    g_idleTab.store(DashboardIndexForSectionId(next, lastSectionId),
                    std::memory_order_relaxed);

    // Compare-then-publish under one lock so the change flags describe exactly
    // the transition we are about to commit. Two LoadSettings() calls can run
    // concurrently (settings thread vs. tray menu on the render thread); without
    // this, both could read the same "old" values and each decide a hotkey
    // re-registration was needed, or neither would.
    bool cityChanged = false;
    bool hotkeySettingChanged = false;
    bool backdropChanged = false;
    bool capsLockChanged = false;
    {
        std::lock_guard lock(g_settingsMutex);
        cityChanged = next.weatherCity != g_settings.weatherCity;
        capsLockChanged = next.capsLock != g_settings.capsLock;
        hotkeySettingChanged =
            next.hideShowHotkeyEnabled != g_settings.hideShowHotkeyEnabled ||
            next.hideShowModifiers != g_settings.hideShowModifiers ||
            next.hideShowVk != g_settings.hideShowVk ||
            next.mediaAutoExpandHotkey != g_settings.mediaAutoExpandHotkey;
        backdropChanged =
            next.backdropMaterial != g_settings.backdropMaterial ||
            std::fabs(next.backdropTint - g_settings.backdropTint) > 0.001f;
        g_settings = std::move(next);
    }
    g_mediaAutoExpandHotkeyEnabled.store(g_settings.mediaAutoExpandHotkey,
                                         std::memory_order_relaxed);
    const int runtimeAutoExpand = Wh_GetIntValue(L"MediaAutoExpandRuntime", -1);
    g_mediaAutoExpandEnabled.store(runtimeAutoExpand >= 0
        ? runtimeAutoExpand != 0 : g_settings.mediaAutoExpand, std::memory_order_relaxed);
    g_notificationDurationSeconds.store(g_settings.notificationDurationSeconds,
                                         std::memory_order_relaxed);
    g_visualizerStrength.store(g_settings.visualizerStrength, std::memory_order_relaxed);
    {
        const int historyLimit = GetSettingsCopy().clipboardHistoryLimit;
        std::lock_guard lock(g_stateMutex);
        if (g_state.clipboardHistory &&
            g_state.clipboardHistory->size() > static_cast<size_t>(historyLimit)) {
            std::vector<ClipboardHistoryItem> items = *g_state.clipboardHistory;
            while (items.size() > static_cast<size_t>(historyLimit)) {
                size_t eraseIndex = items.size();
                for (size_t i = items.size(); i > 0; --i) {
                    if (!items[i - 1].pinned) {
                        eraseIndex = i - 1;
                        break;
                    }
                }
                if (eraseIndex == items.size()) break;
                items.erase(items.begin() + eraseIndex);
            }
            g_state.clipboardHistory =
                std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
        }
    }
    g_layoutDirty = true;
    if (cityChanged && g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
    // Installs or removes WH_KEYBOARD_LL to match. The keyboard thread would pick
    // this up on its next backstop tick anyway; this just makes it immediate.
    if (capsLockChanged) {
        NotifyKeyboardThreadSettingChanged();
    }
    // g_hwnd only exists once RenderThreadProc has created the overlay window;
    // the very first LoadSettings() call (at mod init, before StartThreads())
    // runs with g_hwnd still null, so ApplyHideShowHotkey() no-ops there and
    // RenderThreadProc does the initial registration itself right after
    // CreateWindowExW. Every later call (e.g. from WhTool_ModSettingsChanged)
    // re-registers live so hotkey edits apply without a mod restart.
    // Posted, not called: UnregisterHotKey / RegisterHotKey fail with
    // ERROR_WINDOW_OF_OTHER_THREAD from this thread, which left the old
    // combination registered, the new one never registered, and the bookkeeping
    // flag claiming otherwise -- so switching the hotkey off never released it.
    if (hotkeySettingChanged) {
        if (g_hwnd) {
            PostMessageW(g_hwnd, WM_APP_APPLY_HOTKEY, 0, 0);
        }
    }
    // Same story for the backdrop: no-ops before the window exists, and applies
    // live afterwards so switching blur/acrylic needs no mod restart.
    if (backdropChanged && g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_APPLY_BACKDROP, 0, 0);
    }
}

void EnableBlurBehind(HWND hwnd) {
    DWM_BLURBEHIND blur = {};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = FALSE;
    DwmEnableBlurBehindWindow(hwnd, &blur);
}

// ── Backdrop material (#59) ──────────────────────────────────────────────────
// Real Windows blur/acrylic behind the island.
//
// This uses SetWindowCompositionAttribute, which is the same undocumented entry
// point Windows' own shell surfaces use for acrylic. It is resolved dynamically
// so the mod still loads cleanly on builds where it is absent, and every failure
// path simply leaves the island fully opaque rather than breaking rendering.
//
// It composes with UpdateLayeredWindow: DWM blurs whatever is behind the window,
// and the per-pixel alpha we paint decides how much of that blur shows through.
// That is why enabling a backdrop also caps the pill's own fill alpha further
// down -- an opaque fill would hide the very effect being switched on.
namespace Backdrop {

enum CompositionAttribute : int {
    WCA_ACCENT_POLICY = 19,
};

enum AccentState : int {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
};

struct AccentPolicy {
    AccentState state;
    DWORD flags;
    DWORD gradientColor;  // ABGR
    DWORD animationId;
};

struct CompositionAttributeData {
    CompositionAttribute attribute;
    void* data;
    SIZE_T dataSize;
};

using SetWindowCompositionAttributeFn = BOOL(WINAPI*)(HWND, CompositionAttributeData*);

SetWindowCompositionAttributeFn Resolve() {
    static SetWindowCompositionAttributeFn cached = [] {
        // user32 is already loaded in-process; GetModuleHandle avoids taking a
        // reference we would then have to release.
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32) {
            return static_cast<SetWindowCompositionAttributeFn>(nullptr);
        }
        return reinterpret_cast<SetWindowCompositionAttributeFn>(
            GetProcAddress(user32, "SetWindowCompositionAttribute"));
    }();
    return cached;
}

}  // namespace Backdrop

void ApplyBackdropMaterial(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    const auto setAttribute = Backdrop::Resolve();
    if (!setAttribute) {
        return;  // Unsupported build; island stays opaque.
    }

    Backdrop::AccentPolicy policy = {};
    switch (g_settings.backdropMaterial) {
        case BackdropMaterial::Blur:
            policy.state = Backdrop::ACCENT_ENABLE_BLURBEHIND;
            break;
        case BackdropMaterial::Acrylic:
            policy.state = Backdrop::ACCENT_ENABLE_ACRYLICBLURBEHIND;
            break;
        case BackdropMaterial::None:
        default:
            policy.state = Backdrop::ACCENT_DISABLED;
            break;
    }

    // Acrylic needs a tint supplied in the policy itself; the shell mixes this
    // over the blurred backdrop. Stored ABGR, and the alpha here is the tint
    // strength rather than the window's opacity.
    if (policy.state == Backdrop::ACCENT_ENABLE_ACRYLICBLURBEHIND) {
        const D2D1_COLOR_F tint = g_settings.pillBgColor;
        const auto channel = [](float v) {
            return static_cast<DWORD>(Clamp(v, 0.0f, 1.0f) * 255.0f);
        };
        const DWORD tintAlpha = channel(g_settings.backdropTint);
        policy.gradientColor = (tintAlpha << 24) | (channel(tint.b) << 16) |
                               (channel(tint.g) << 8) | channel(tint.r);
    }

    Backdrop::CompositionAttributeData data = {};
    data.attribute = Backdrop::WCA_ACCENT_POLICY;
    data.data = &policy;
    data.dataSize = sizeof(policy);
    setAttribute(hwnd, &data);
}

// Always unregisters before registering so a settings change never leaks the
// previous hotkey. Safe to call before the overlay window exists (no-ops).
void ApplyHideShowHotkey() {
    if (!g_hwnd) {
        return;
    }

    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_hwnd, ID_HIDE_SHOW_HOTKEY);
        g_hotkeyRegistered = false;
    }
    if (g_mediaExpandHotkeyRegistered) {
        UnregisterHotKey(g_hwnd, ID_MEDIA_AUTOEXPAND_HOTKEY);
        g_mediaExpandHotkeyRegistered = false;
    }

    if (g_settings.hideShowHotkeyEnabled) {
        if (RegisterHotKey(g_hwnd, ID_HIDE_SHOW_HOTKEY, g_settings.hideShowModifiers,
                           g_settings.hideShowVk)) {
            g_registeredHotkeyModifiers = g_settings.hideShowModifiers;
            g_registeredHotkeyVk = g_settings.hideShowVk;
            g_hotkeyRegistered = true;
        } else {
            Wh_Log(L"Failed to register hide/show hotkey (error %lu).", GetLastError());
        }
    }
    if (g_settings.mediaAutoExpandHotkey) {
        if (RegisterHotKey(g_hwnd, ID_MEDIA_AUTOEXPAND_HOTKEY,
                           MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'S')) {
            g_mediaExpandHotkeyRegistered = true;
        } else {
            Wh_Log(L"Failed to register Ctrl+Alt+S auto-expand hotkey (error %lu).", GetLastError());
        }
    }
}



struct MonitorEnumData {
    std::vector<HMONITOR> monitors;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) {
    auto* data = reinterpret_cast<MonitorEnumData*>(dwData);
    data->monitors.push_back(hMonitor);
    return TRUE;
}

RECT GetAnchorWorkRect() {
    HMONITOR selectedMonitor = nullptr;

    if (g_settings.targetMonitor == -1) {
        POINT pt = {0, 0};
        GetCursorPos(&pt);
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    } else if (g_settings.targetMonitor > 0) {
        MonitorEnumData data;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&data));

        int index = g_settings.targetMonitor - 1;
        if (index >= 0 && index < static_cast<int>(data.monitors.size())) {
            selectedMonitor = data.monitors[index];
        }
    }

    if (!selectedMonitor) {
        POINT pt = {0, 0};
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(selectedMonitor, &mi);
    return mi.rcWork;
}

// Vertical offset for the island's current size (#83).
//
// One Offset Y forced a compromise: a value that tucks the idle pill up near the
// screen edge leaves the expanded dashboard awkward to reach, and vice versa.
// With the separate offset enabled, Offset Y becomes the collapsed value and
// OffsetYExpanded the expanded one, and the island *eases* between them in step
// with the expansion itself rather than snapping at some threshold.
//
// windowHeight is the layered window's height, which carries kRenderPadY on both
// edges, so the pill's own height is recovered before measuring how expanded it
// is.
int ResolveOffsetY(float windowHeight) {
    if (!g_settings.separateExpandedOffsetY) {
        return g_settings.offsetY;
    }

    const float scale = std::max(0.05f, g_settings.sizeScale);
    const float pillHeight = std::max(0.0f, windowHeight - kRenderPadY * 2.0f) / scale;

    // Collapsed alert pills are ~44px tall in content space; the expanded
    // dashboard is MediaLayout::kExpandedHeight.
    constexpr float kCollapsedHeight = 44.0f;
    const float span = MediaLayout::kExpandedHeight - kCollapsedHeight;
    const float t = (span <= 1.0f) ? 0.0f
                                   : Clamp((pillHeight - kCollapsedHeight) / span, 0.0f, 1.0f);

    const float collapsed = static_cast<float>(g_settings.offsetY);
    const float expanded = static_cast<float>(g_settings.offsetYExpanded);
    return static_cast<int>(std::lround(collapsed + (expanded - collapsed) * t));
}

void PositionOverlayWindow(HWND hwnd, int width, int height) {
    RECT work = GetAnchorWorkRect();
    const bool flushTop = g_settings.notchStyle || g_settings.borderMergedMode;
    int x = work.left + (work.right - work.left - width) / 2;
    int y = flushTop ? work.top : (work.top + 8);

    switch (g_settings.position) {
        case Position::TopLeft:
            x = work.left + 16;
            y = flushTop ? work.top : (work.top + 8);
            break;
        case Position::TopRight:
            x = work.right - width - 16;
            y = flushTop ? work.top : (work.top + 8);
            break;
        case Position::BottomCenter:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.bottom - height - 40;
            break;
        case Position::BottomLeft:
            x = work.left + 16;
            y = work.bottom - height - 40;
            break;
        case Position::BottomRight:
            x = work.right - width - 16;
            y = work.bottom - height - 40;
            break;
        case Position::TopCenter:
        default:
            break;
    }

    HWND zOrder = g_settings.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;

    // Manage owner window to firmly anchor to desktop when alwaysOnTop is false
    if (g_settings.alwaysOnTop) {
        SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, 0);
    } else {
        HWND hProgman = FindWindowW(L"Progman", nullptr);
        if (hProgman) {
            SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hProgman));
        }
    }

    x += g_settings.offsetX;
    y += ResolveOffsetY(static_cast<float>(height));
    SetWindowPos(hwnd, zOrder, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

    ApplyBackdropRegion(hwnd, width, height);
}

// A composition backdrop (#59) is blurred across the window's whole rectangle
// and is *not* masked by the per-pixel alpha we paint. Left alone that would
// show a blurred rectangle filling the transparent render padding around the
// island. Constraining the window to a rounded region confines the blur to the
// island's own silhouette.
//
// The region is cleared again whenever no backdrop is active, because it would
// otherwise clip the soft drop shadow, which is drawn out in that same padding.
void ApplyBackdropRegion(HWND hwnd, int windowWidth, int windowHeight) {
    if (!hwnd) {
        return;
    }

    if (g_settings.backdropMaterial == BackdropMaterial::None) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }

    const int pillWidth = windowWidth - static_cast<int>(std::round(kRenderPadX * 2.0f));
    const int pillHeight = windowHeight - static_cast<int>(std::round(kRenderPadY * 2.0f));
    if (pillWidth <= 1 || pillHeight <= 1) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }

    const int left = static_cast<int>(std::round(kRenderPadX));
    const int top = (g_settings.notchStyle || g_settings.borderMergedMode)
                        ? 0
                        : static_cast<int>(std::round(kRenderPadY));

    // The pill bounces a few pixels vertically via the nudge spring, which this
    // function does not see, so the region is padded to avoid clipping content
    // mid-animation.
    constexpr int kNudgeSlack = 10;
    const int regionTop = std::max(0, top - kNudgeSlack);
    const int regionBottom = std::min(windowHeight, top + pillHeight + kNudgeSlack);

    int radius;
    const bool expandedRegion = pillHeight > g_settings.islandHeight * g_settings.sizeScale * 1.2f;
    if (g_settings.notchStyle) {
        radius = static_cast<int>(std::round(16.0f * g_settings.sizeScale));
    } else if (g_settings.w11Style) {
        radius = static_cast<int>(std::round(expandedRegion
            ? std::min(pillHeight * 0.22f, g_settings.cornerRadius * g_settings.sizeScale)
            : 10.0f * g_settings.sizeScale));
    } else {
        radius = static_cast<int>(std::round(expandedRegion
            ? std::min(pillHeight * 0.5f, g_settings.cornerRadius * g_settings.sizeScale)
            : pillHeight * 0.5f));
    }
    radius = std::max(0, radius);

    // CreateRoundRectRgn takes the full ellipse size, not the corner radius.
    HRGN region = CreateRoundRectRgn(left, regionTop, left + pillWidth + 1, regionBottom + 1,
                                     radius * 2, radius * 2);
    if (region) {
        // Ownership transfers to the system on success.
        if (SetWindowRgn(hwnd, region, TRUE) == 0) {
            DeleteObject(region);
        }
    }
}

// Windows' own shell surfaces are all WS_EX_TOPMOST, and they are *supposed* to
// sit above the island. Raising ourselves over the Start menu, the notification
// centre, the volume OSD, Task View, a popup menu or a tooltip would be a bug,
// not a fix, so these classes are never treated as something to reclaim from.
bool IsShellOwnedWindow(HWND hwnd) {
    wchar_t className[128] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) <= 0) {
        // Unknown, so treat it as shell-owned and leave the z-order alone.
        return true;
    }

    static const wchar_t* const kShellClasses[] = {
        L"Windows.UI.Core.CoreWindow",              // Start, Action Center, Search
        L"Xaml_WindowedPopupClass",                 // WinUI flyouts and popups
        L"Shell_TrayWnd",                           // taskbar
        L"Shell_SecondaryTrayWnd",                  // taskbar on other monitors
        L"TopLevelWindowForOverflowXamlIsland",     // taskbar overflow
        L"MultitaskingViewFrame",                   // Task View
        L"ForegroundStaging",                       // Task View staging
        L"#32768",                                  // popup menus
        L"tooltips_class32",                        // tooltips
        L"NarratorHelperWindow",
    };

    for (const wchar_t* candidate : kShellClasses) {
        if (_wcsicmp(className, candidate) == 0) {
            return true;
        }
    }
    return false;
}

// Re-asserts the island's place in the topmost band when another always-on-top
// window has been raised over it (PowerToys' bar, taskbar mods, other
// overlays). PositionOverlayWindow only runs on a resize or an explicit layout
// change, so without this the island stayed buried until the next one.
//
// Deliberately conservative: shell surfaces are ignored, a slight clip does not
// count, and we only act when we are still in the topmost band ourselves. That
// keeps this from turning into a z-order tug-of-war or from stealing focus
// surfaces away from Windows.
void EnsureTopmost(HWND hwnd) {
    if (!g_settings.alwaysOnTop || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return;
    }

    // If we are not topmost at all, PositionOverlayWindow owns fixing that;
    // walking the whole z-order from a demoted position would be expensive.
    if ((GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) == 0) {
        return;
    }

    RECT islandRect = {};
    if (!GetWindowRect(hwnd, &islandRect)) {
        return;
    }

    const long islandArea = std::max(1L, (islandRect.right - islandRect.left) *
                                             (islandRect.bottom - islandRect.top));
    // Require a real overlap, not a one-pixel graze.
    const long minOverlapArea = std::max(256L, islandArea / 8);

    bool covered = false;
    for (HWND above = GetWindow(hwnd, GW_HWNDPREV); above != nullptr;
         above = GetWindow(above, GW_HWNDPREV)) {
        if (above == hwnd) {
            continue;
        }
        // Everything above a topmost window is itself topmost, so reaching a
        // non-topmost window means we are already at the top of that band.
        if ((GetWindowLongW(above, GWL_EXSTYLE) & WS_EX_TOPMOST) == 0) {
            break;
        }
        if (!IsWindowVisible(above) || IsIconic(above) || IsShellOwnedWindow(above)) {
            continue;
        }

        RECT otherRect = {};
        RECT intersection = {};
        if (GetWindowRect(above, &otherRect) &&
            IntersectRect(&intersection, &islandRect, &otherRect)) {
            const long overlap = (intersection.right - intersection.left) *
                                 (intersection.bottom - intersection.top);
            if (overlap >= minOverlapArea) {
                covered = true;
                break;
            }
        }
    }

    if (covered) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

RECT GetIslandDockRect() {
    RECT work = GetAnchorWorkRect();
    const float scale = g_settings.sizeScale;
    const int dockWidth = static_cast<int>(std::max(280.0f, 340.0f * scale));
    const int dockHeight = static_cast<int>(std::max(48.0f, 56.0f * scale));
    const bool flushTop = g_settings.notchStyle || g_settings.borderMergedMode;

    int x = work.left + (work.right - work.left - dockWidth) / 2;
    int y = flushTop ? work.top : (work.top + 8);

    switch (g_settings.position) {
        case Position::TopLeft:
            x = work.left + 16;
            y = flushTop ? work.top : (work.top + 8);
            break;
        case Position::TopRight:
            x = work.right - dockWidth - 16;
            y = flushTop ? work.top : (work.top + 8);
            break;
        case Position::BottomCenter:
            x = work.left + (work.right - work.left - dockWidth) / 2;
            y = work.bottom - dockHeight - 40;
            break;
        case Position::BottomLeft:
            x = work.left + 16;
            y = work.bottom - dockHeight - 40;
            break;
        case Position::BottomRight:
            x = work.right - dockWidth - 16;
            y = work.bottom - dockHeight - 40;
            break;
        case Position::TopCenter:
        default:
            break;
    }

    x += g_settings.offsetX;
    // The dock rect is the hover target for a hidden island, which is always in
    // its collapsed state, so it follows the collapsed offset.
    y += g_settings.offsetY;

    RECT r;
    r.left = x;
    r.right = x + dockWidth;
    if (IsBottomPosition(g_settings.position)) {
        r.top = y;
        r.bottom = std::max(static_cast<int>(work.bottom), y + dockHeight + 20);
    } else {
        r.top = std::min(y, static_cast<int>(work.top));
        r.bottom = y + dockHeight;
    }
    return r;
}

bool DecodeImageBytesToPixels(const std::vector<uint8_t>& bytes, BitmapPixels* outPixels) {
    if (!outPixels || bytes.empty()) {
        return false;
    }

    ComPtr<IWICImagingFactory> factory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        return false;
    }

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) {
        return false;
    }

    void* locked = GlobalLock(mem);
    memcpy(locked, bytes.data(), bytes.size());
    GlobalUnlock(mem);

    ComPtr<IStream> stream;
    hr = CreateStreamOnHGlobal(mem, TRUE, &stream);
    if (FAILED(hr)) {
        GlobalFree(mem);
        return false;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad,
                                          &decoder);
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<IWICFormatConverter> converter;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr)) {
        return false;
    }

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
                               WICBitmapDitherTypeNone, nullptr, 0.0,
                               WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) {
        return false;
    }

    UINT width = 0;
    UINT height = 0;
    converter->GetSize(&width, &height);
    if (!width || !height || width > 2048 || height > 2048) {
        return false;
    }

    BitmapPixels pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.bgra.resize(static_cast<size_t>(width) * height * 4);

    hr = converter->CopyPixels(nullptr, width * 4,
                               static_cast<UINT>(pixels.bgra.size()),
                               pixels.bgra.data());
    if (FAILED(hr)) {
        return false;
    }

    struct Bucket {
        uint32_t count = 0;
        uint32_t r = 0;
        uint32_t g = 0;
        uint32_t b = 0;
        uint32_t satSum = 0;  // accumulated saturation for vibrancy-weighted winner
    };

    std::array<Bucket, 16 * 16 * 16> buckets{};
    for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
        const uint8_t alpha = pixels.bgra[i + 3];
        const uint8_t blue  = pixels.bgra[i + 0];
        const uint8_t green = pixels.bgra[i + 1];
        const uint8_t red   = pixels.bgra[i + 2];
        if (alpha < 32) {
            continue;
        }

        const int maxc = std::max({red, green, blue});
        const int minc = std::min({red, green, blue});
        const int luminance = (54 * red + 183 * green + 19 * blue) / 256;
        const int saturation = maxc - minc;
        // Reject near-black, near-white, and near-gray pixels.
        // Raise luminance floor slightly (36 instead of 28) so very dark-but-colorful
        // pixels don't swamp vibrancy scoring on dark-mood album art.
        if (luminance < 36 || luminance > 220 || saturation < 28) {
            continue;
        }

        const size_t bucketIndex = ((red >> 4) << 8) | ((green >> 4) << 4) | (blue >> 4);
        Bucket& bucket = buckets[bucketIndex];
        const uint32_t weight = 1 + static_cast<uint32_t>(saturation / 32);  // stronger vibrancy weight
        bucket.count  += weight;
        bucket.r      += red   * weight;
        bucket.g      += green * weight;
        bucket.b      += blue  * weight;
        bucket.satSum += static_cast<uint32_t>(saturation) * weight;
    }

    // --- Vibrancy-weighted winner selection ---
    // Raw frequency alone lets large muted regions (e.g. a beige background)
    // beat a smaller vivid color that's visually "the" accent.
    // Strategy: collect the top 3 by count, then pick whichever has the
    // highest average saturation — cheap, no extra image pass required.
    struct Candidate { const Bucket* b = nullptr; size_t idx = 0; };
    Candidate top[3];
    for (size_t i = 0; i < buckets.size(); ++i) {
        const Bucket& bk = buckets[i];
        if (bk.count == 0) continue;
        for (int s = 0; s < 3; ++s) {
            if (!top[s].b || bk.count > top[s].b->count) {
                for (int t = 2; t > s; --t) top[t] = top[t - 1];
                top[s] = {&bk, i};
                break;
            }
        }
    }

    // Among those top-3, pick the one with the highest avg saturation.
    const Bucket* best = nullptr;
    size_t bestIdx = 0;
    float bestVibrancy = -1.0f;
    for (int s = 0; s < 3; ++s) {
        if (!top[s].b) break;
        const float vibrancy = static_cast<float>(top[s].b->satSum) /
                               static_cast<float>(top[s].b->count);
        if (vibrancy > bestVibrancy) {
            bestVibrancy = vibrancy;
            best = top[s].b;
            bestIdx = top[s].idx;
        }
    }

    if (best && best->count > 0) {
        // --- Neighbor-bucket merging ---
        // A color straddling a bucket boundary splits its votes across up to 8
        // adjacent cells. Pool the winning bucket with all 26 face/edge/corner
        // neighbors before averaging so the final RGB is stable and representative.
        uint64_t poolCount = 0;
        double poolR = 0, poolG = 0, poolB = 0;

        const int bi = static_cast<int>((bestIdx >> 8) & 0xF);  // red index
        const int gi = static_cast<int>((bestIdx >> 4) & 0xF);  // green index
        const int bli = static_cast<int>( bestIdx       & 0xF); // blue index

        for (int dr = -1; dr <= 1; ++dr) {
            for (int dg = -1; dg <= 1; ++dg) {
                for (int db = -1; db <= 1; ++db) {
                    const int ni = bi + dr, nj = gi + dg, nk = bli + db;
                    if (ni < 0 || ni > 15 || nj < 0 || nj > 15 || nk < 0 || nk > 15) continue;
                    const size_t nIdx = (static_cast<size_t>(ni) << 8) |
                                        (static_cast<size_t>(nj) << 4) |
                                         static_cast<size_t>(nk);
                    const Bucket& nb = buckets[nIdx];
                    if (nb.count == 0) continue;
                    poolCount += nb.count;
                    poolR += nb.r;
                    poolG += nb.g;
                    poolB += nb.b;
                }
            }
        }

        if (poolCount > 0) {
            const float invC = 1.0f / static_cast<float>(poolCount);
            const float rawR = static_cast<float>(poolR * invC) / 255.0f;
            const float rawG = static_cast<float>(poolG * invC) / 255.0f;
            const float rawB = static_cast<float>(poolB * invC) / 255.0f;

            D2D1_COLOR_F bgColor = g_settings.pillBgColor;
            if (bgColor.a <= 0.0f) {
                bgColor = D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f); // Obsidian default #08080A
            }

            pixels.sampledAccent = EnsureContrastAgainstBackground(
                D2D1::ColorF(rawR, rawG, rawB, 1.0f),
                bgColor);
        }
        // If poolCount is somehow 0 (all neighbors empty and center also cleared),
        // the existing sampledAccent default (#4cc9f0) is kept unchanged — explicit
        // fallback for fully-grayscale/near-black-and-white album art.
    }
    // else: no qualifying colorful pixel found (B&W / fully-filtered image)
    // — leave sampledAccent at the BitmapPixels default (#4cc9f0).

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);
    return true;
}

bool IconToPixels(HICON icon, UINT size, BitmapPixels* outPixels) {
    if (!icon || !outPixels || !size) {
        return false;
    }

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) {
        return false;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(size);
    bi.bmiHeader.biHeight = -static_cast<LONG>(size);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return false;
    }

    // Fill with transparent black (alpha=0) so icon edges don't bleed dark fringe.
    HGDIOBJ old = SelectObject(dc, bitmap);
    RECT fill = {0, 0, static_cast<LONG>(size), static_cast<LONG>(size)};
    // Use NULL_BRUSH (transparent) then manually zero-fill the BGRA buffer after copy.
    FillRect(dc, &fill, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
    // Zero out the bits so background is fully transparent before drawing icon.
    ZeroMemory(bits, static_cast<size_t>(size) * size * 4);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);

    BitmapPixels pixels;
    pixels.width = size;
    pixels.height = size;
    pixels.bgra.resize(static_cast<size_t>(size) * size * 4);
    memcpy(pixels.bgra.data(), bits, pixels.bgra.size());

    // Older icons can have no alpha in the color bitmap. Treat black pixels as
    // transparent only when the icon did not write any alpha at all.
    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.bgra.size(); i += 4) {
        if (pixels.bgra[i] != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        // No alpha channel: treat near-black as transparent, rest as opaque.
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
            const bool black = pixels.bgra[i] < 4 && pixels.bgra[i + 1] < 4 && pixels.bgra[i + 2] < 4;
            pixels.bgra[i + 3] = black ? 0 : 255;
        }
    } else {
        // Convert to premultiplied alpha so D2D renders edges cleanly without dark fringing.
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
            const uint8_t a = pixels.bgra[i + 3];
            if (a < 255 && a > 0) {
                pixels.bgra[i + 0] = static_cast<uint8_t>(pixels.bgra[i + 0] * a / 255);
                pixels.bgra[i + 1] = static_cast<uint8_t>(pixels.bgra[i + 1] * a / 255);
                pixels.bgra[i + 2] = static_cast<uint8_t>(pixels.bgra[i + 2] * a / 255);
            }
        }
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);

    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return true;
}

bool ProcessImageNameForPid(DWORD pid, std::wstring* imageName) {
    if (!pid || !imageName) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) {
        *imageName = path;
    }
    return ok;
}

HICON CopyWindowIcon(HWND hwnd, WPARAM iconType) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 80, &result);
    return result ? CopyIcon(reinterpret_cast<HICON>(result)) : nullptr;
}

HICON getProcessIcon(DWORD pid) {
    std::wstring path;
    if (ProcessImageNameForPid(pid, &path) && !path.empty()) {
        HICON hIcon = nullptr;
        UINT iconId = 0;
        // Try to fetch a high-res 64x64 icon first to avoid pixelated icons
        using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
        static auto pPrivateExtractIconsW = reinterpret_cast<PrivateExtractIconsW_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));
        if (pPrivateExtractIconsW && pPrivateExtractIconsW(path.c_str(), 0, 64, 64, &hIcon, &iconId, 1, 0) == 1 && hIcon) {
            return hIcon;
        }

        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_LARGEICON)) {
            return sfi.hIcon;
        }

        HICON large = nullptr;
        HICON small = nullptr;
        if (ExtractIconExW(path.c_str(), 0, &large, &small, 1) > 0) {
            if (small) {
                DestroyIcon(small);
            }
            if (large) {
                return large;
            }
        }
    }

    return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
}

HICON getWindowIcon(HWND hwnd) {
    if (!hwnd) {
        return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
    }

    if (HICON icon = CopyWindowIcon(hwnd, ICON_BIG)) {
        return icon;
    }
    if (HICON icon = CopyWindowIcon(hwnd, ICON_SMALL)) {
        return icon;
    }

    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON))) {
        return CopyIcon(icon);
    }
    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM))) {
        return CopyIcon(icon);
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return getProcessIcon(pid);
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring BaseNameFromPath(std::wstring path) {
    const size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        path.erase(0, slash + 1);
    }
    return path;
}

std::wstring StripExtension(std::wstring value) {
    const size_t dot = value.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        value.resize(dot);
    }
    return value;
}

bool ProcessImageNameForWindow(HWND hwnd, std::wstring* imageName) {
    if (!imageName) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    return ProcessImageNameForPid(pid, imageName);
}

std::wstring FriendlyMediaSourceName(std::wstring_view source) {
    const std::wstring lower = ToLowerCopy(std::wstring(source));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YouTube";
    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Chrome";
    if (lower.find(L"msedge") != std::wstring::npos || lower.find(L"edge") != std::wstring::npos) return L"Edge";
    if (lower.find(L"firefox") != std::wstring::npos) return L"Firefox";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (lower.find(L"wmplayer") != std::wstring::npos) return L"Windows Media";
    if (lower.find(L"zune") != std::wstring::npos || lower.find(L"media") != std::wstring::npos) return L"Media Player";

    std::wstring text(source);
    const size_t bang = text.find(L'!');
    if (bang != std::wstring::npos && bang + 1 < text.size()) {
        text.erase(0, bang + 1);
    }
    const size_t dot = text.find(L'.');
    if (dot != std::wstring::npos) {
        text.resize(dot);
    }
    return text.empty() ? L"Media" : text;
}

std::wstring MediaSourceBadge(std::wstring_view sourceName) {
    const std::wstring lower = ToLowerCopy(std::wstring(sourceName));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YT";
    if (lower.find(L"spotify") != std::wstring::npos) return L"SP";
    if (lower.find(L"chrome") != std::wstring::npos) return L"CH";
    if (lower.find(L"edge") != std::wstring::npos) return L"ED";
    if (lower.find(L"firefox") != std::wstring::npos) return L"FF";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (sourceName.empty()) return L"\u25b6";
    std::wstring badge;
    badge.push_back(static_cast<wchar_t>(towupper(sourceName[0])));
    return badge;
}

bool WindowLooksLikeMediaSource(HWND hwnd, const std::wstring& sourceLower) {
    if (!IsWindowVisible(hwnd) || hwnd == g_hwnd) {
        return false;
    }

    std::wstring image;
    if (!ProcessImageNameForWindow(hwnd, &image)) {
        return false;
    }

    const std::wstring base = ToLowerCopy(BaseNameFromPath(image));
    if (base.empty()) {
        return false;
    }

    return sourceLower.find(base) != std::wstring::npos ||
           (base.find(L"chrome") != std::wstring::npos && sourceLower.find(L"chrome") != std::wstring::npos) ||
           (base.find(L"msedge") != std::wstring::npos && sourceLower.find(L"edge") != std::wstring::npos) ||
           (base.find(L"vlc") != std::wstring::npos && sourceLower.find(L"vlc") != std::wstring::npos);
}

// #62: short-form video feeds change "track" every few seconds, which turned
// auto-expand into a constant popup. A blocklist entry is matched loosely
// against the friendly source name, the raw AUMID and the title, so a single
// entry like "tiktok" catches both the desktop app and a browser tab. The
// collapsed pill still updates -- only the expansion is suppressed.
//
// Reads the blocklist under g_settingsMutex rather than taking a Settings
// reference: the caller is the render loop, and walking a std::vector<std::wstring>
// that LoadSettings() may be reassigning is a use-after-free, not just a stale
// read. The haystack is built outside the lock because that part allocates.
bool MediaExpandBlocked(const MediaSnapshot& media) {
    {
        // Fast path for the overwhelmingly common empty-blocklist case: one
        // uncontended lock, no allocation, no string work.
        std::lock_guard lock(g_settingsMutex);
        if (g_settings.mediaExpandBlocklist.empty()) {
            return false;
        }
    }

    const std::wstring haystack = ToLowerCopy(
        media.sourceName + L"\n" + media.sourceAppUserModelId + L"\n" + media.title);

    std::lock_guard lock(g_settingsMutex);
    for (const std::wstring& needle : g_settings.mediaExpandBlocklist) {
        if (!needle.empty() && haystack.find(needle) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

bool IsBrowserMediaSource(std::wstring_view source) {
    const std::wstring lower = ToLowerCopy(std::wstring(source));
    return lower.find(L"chrome") != std::wstring::npos ||
           lower.find(L"edge") != std::wstring::npos ||
           lower.find(L"msedge") != std::wstring::npos ||
           lower.find(L"firefox") != std::wstring::npos ||
           lower.find(L"youtube") != std::wstring::npos;
}

std::wstring SiteBadgeFromTitle(std::wstring_view title) {
    const std::wstring lower = ToLowerCopy(std::wstring(title));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YT";
    if (lower.find(L"netflix") != std::wstring::npos) return L"NF";
    if (lower.find(L"prime video") != std::wstring::npos || lower.find(L"amazon") != std::wstring::npos) return L"PV";
    if (lower.find(L"disney") != std::wstring::npos) return L"D+";
    if (lower.find(L"hotstar") != std::wstring::npos) return L"HS";
    if (lower.find(L"spotify") != std::wstring::npos) return L"SP";
    if (lower.find(L"soundcloud") != std::wstring::npos) return L"SC";
    return L"WEB";
}

struct IconCacheEntry {
    DWORD pid = 0;
    std::wstring exePath;
    BitmapPixels pixels;
    uint64_t lastUsed = 0;
};

std::mutex g_iconCacheMutex;
std::unordered_map<DWORD, IconCacheEntry> g_iconCacheByPid;
uint64_t g_iconCacheClock = 0;

BitmapPixels GetCachedProcessIconPixels(DWORD pid, UINT size) {
    std::wstring exePath;
    ProcessImageNameForPid(pid, &exePath);

    {
        std::lock_guard lock(g_iconCacheMutex);
        auto it = g_iconCacheByPid.find(pid);
        if (it != g_iconCacheByPid.end() && it->second.exePath == exePath &&
            it->second.pixels.width == size) {
            it->second.lastUsed = ++g_iconCacheClock;
            return it->second.pixels;
        }
    }

    BitmapPixels pixels;
    HICON icon = getProcessIcon(pid);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }

    if (!pixels.bgra.empty()) {
        std::lock_guard lock(g_iconCacheMutex);
        g_iconCacheByPid[pid] = IconCacheEntry{pid, exePath, pixels, ++g_iconCacheClock};
        if (g_iconCacheByPid.size() > 32) {
            auto oldest = g_iconCacheByPid.begin();
            for (auto it = g_iconCacheByPid.begin(); it != g_iconCacheByPid.end(); ++it) {
                if (it->second.lastUsed < oldest->second.lastUsed) {
                    oldest = it;
                }
            }
            g_iconCacheByPid.erase(oldest);
        }
    }

    return pixels;
}

BitmapPixels GetWindowIconPixels(HWND hwnd, UINT size) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    BitmapPixels pixels = GetCachedProcessIconPixels(pid, size);
    if (!pixels.bgra.empty()) {
        return pixels;
    }

    HICON icon = getWindowIcon(hwnd);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }
    return pixels;
}

bool IsIgnorableForegroundWindow(HWND hwnd, const std::wstring& title) {
    wchar_t className[128] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    const std::wstring cls = ToLowerCopy(className);
    const std::wstring lowerTitle = ToLowerCopy(title);

    std::wstring image;
    ProcessImageNameForWindow(hwnd, &image);
    const std::wstring base = ToLowerCopy(BaseNameFromPath(image));

    return title.empty() ||
           hwnd == g_hwnd ||
           cls == L"shell_traywnd" ||
           cls == L"workerw" ||
           cls == L"progman" ||
           lowerTitle.find(L"windhawk") != std::wstring::npos ||
           base == L"explorer.exe" ||
           base == L"windhawk.exe";
}

BOOL CALLBACK FindMediaSourceWindowProc(HWND hwnd, LPARAM lParam) {
    auto* data = reinterpret_cast<std::pair<const std::wstring*, HWND*>*>(lParam);
    if (WindowLooksLikeMediaSource(hwnd, *data->first)) {
        *data->second = hwnd;
        return FALSE;
    }
    return TRUE;
}

std::wstring FindBrowserMediaSiteBadge(const std::wstring& sourceAppUserModelId) {
    const std::wstring sourceLower = ToLowerCopy(sourceAppUserModelId);
    HWND found = nullptr;
    std::pair<const std::wstring*, HWND*> data{&sourceLower, &found};
    EnumWindows(FindMediaSourceWindowProc, reinterpret_cast<LPARAM>(&data));
    if (found) {
        wchar_t title[192] = {};
        GetWindowTextW(found, title, ARRAYSIZE(title));
        return SiteBadgeFromTitle(title);
    }
    return L"WEB";
}

BitmapPixels FindMediaSourceIcon(const std::wstring& sourceAppUserModelId) {
    BitmapPixels pixels;
    if (IsBrowserMediaSource(sourceAppUserModelId)) {
        // Continue searching for the browser window anyway to retrieve
        // the browser's app icon or the PWA's app icon.
    }

    const std::wstring sourceLower = ToLowerCopy(sourceAppUserModelId);
    HWND found = nullptr;
    std::pair<const std::wstring*, HWND*> data{&sourceLower, &found};
    EnumWindows(FindMediaSourceWindowProc, reinterpret_cast<LPARAM>(&data));
    if (found) {
        pixels = GetWindowIconPixels(found, 32);
    }
    return pixels;
}

struct FindAppIconData {
    const std::wstring* targetName;
    HWND bestHwnd = nullptr;
    std::unordered_map<DWORD, std::wstring> pidToProcessName;
};

BOOL CALLBACK FindAppIconWindowProc(HWND hwnd, LPARAM lParam) {
    auto* d = reinterpret_cast<FindAppIconData*>(lParam);
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    if (hwnd == g_hwnd) {
        return TRUE;
    }

    // 1. Fast path: Check window title first (lightweight check without opening process handles)
    wchar_t title[128] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    std::wstring titleLower = ToLowerCopy(title);
    if (!titleLower.empty() && titleLower.find(*d->targetName) != std::wstring::npos) {
        d->bestHwnd = hwnd;
        return FALSE; // Found via title, stop enumeration
    }

    // 2. Slow path fallback: Check process executable name
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return TRUE;
    }

    std::wstring baseNoExe;
    auto it = d->pidToProcessName.find(pid);
    if (it != d->pidToProcessName.end()) {
        baseNoExe = it->second;
    } else {
        std::wstring image;
        if (ProcessImageNameForPid(pid, &image)) {
            std::wstring base = ToLowerCopy(BaseNameFromPath(image));
            baseNoExe = base;
            if (baseNoExe.size() > 4 && baseNoExe.substr(baseNoExe.size() - 4) == L".exe") {
                baseNoExe = baseNoExe.substr(0, baseNoExe.size() - 4);
            }
            d->pidToProcessName[pid] = baseNoExe;
        } else {
            d->pidToProcessName[pid] = L"";
        }
    }

    if (!baseNoExe.empty()) {
        if (baseNoExe.find(*d->targetName) != std::wstring::npos ||
            d->targetName->find(baseNoExe) != std::wstring::npos) {
            d->bestHwnd = hwnd;
            return FALSE; // Found via process name, stop enumeration
        }
    }

    return TRUE;
}

BitmapPixels FindAppIconByName(const std::wstring& appName, UINT size) {
    BitmapPixels pixels;
    if (appName.empty()) {
        return pixels;
    }

    const std::wstring appNameLower = ToLowerCopy(appName);
    FindAppIconData data;
    data.targetName = &appNameLower;

    EnumWindows(FindAppIconWindowProc, reinterpret_cast<LPARAM>(&data));

    if (data.bestHwnd) {
        pixels = GetWindowIconPixels(data.bestHwnd, size);
    }
    return pixels;
}

std::vector<uint8_t> ReadWinRtStreamBytes(
    const winrt::Windows::Storage::Streams::IRandomAccessStreamReference& reference) {
    std::vector<uint8_t> bytes;
    if (!reference) {
        return bytes;
    }

    auto stream = reference.OpenReadAsync().get();
    if (!stream) {
        return bytes;
    }

    const uint64_t size64 = stream.Size();
    if (size64 == 0 || size64 > 8 * 1024 * 1024) {
        return bytes;
    }

    const uint32_t size = static_cast<uint32_t>(size64);
    winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
    reader.LoadAsync(size).get();
    bytes.resize(size);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes.data(), bytes.data() + bytes.size()));
    return bytes;
}

void TriggerNudge() {
    // If we're parked (auto-hidden), any real event must wake us regardless
    // of the normal throttle, or it'd never be seen until the next hook/
    // hotkey/fullscreen-recheck wake.
    const bool wasParked = g_autoHiddenParked.exchange(false, std::memory_order_relaxed);
    const double now = NowSeconds();
    const double previous = g_lastNudgeTime.load();
    if (!wasParked && now - previous < 0.45) {
        return;
    }
    g_lastNudgeTime = now;
    HWND hwnd = g_hwnd;
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_NEW_EVENT, 0, 0);
    }
}

// Chooses which SMTC session the island should follow.
//
// GetCurrentSession() returns whatever Windows last treated as the foreground
// media app, which is often a stale or paused session belonging to a different
// player. Following it blindly meant a player that was genuinely playing could
// be ignored entirely -- the reason VLC frequently produced no reaction at all
// while Spotify worked. Prefer the current session when it is actually playing,
// otherwise the first session that is, and only then fall back to the current
// one so paused/idle state still shows.
bool IsVideoMediaSession(
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession const& session,
    const Settings& settings) {
    if (!session) return false;

    std::wstring identity;
    try {
        identity = session.SourceAppUserModelId().c_str();
        auto properties = session.TryGetMediaPropertiesAsync().get();
        if (properties) {
            identity += L" ";
            identity += properties.Title().c_str();
            identity += L" ";
            identity += properties.AlbumTitle().c_str();
        }
    } catch (...) {
    }
    std::transform(identity.begin(), identity.end(), identity.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });

    static constexpr const wchar_t* kVideoHints[] = {
        L"vlc", L"netflix", L"prime video", L"primevideo", L"disney+",
        L"hulu", L"zunevideo", L"movies & tv", L"youtube", L"video",
    };
    for (const wchar_t* hint : kVideoHints) {
        if (identity.find(hint) != std::wstring::npos) return true;
    }
    for (const std::wstring& hint : settings.videoAppIds) {
        if (!hint.empty() && identity.find(hint) != std::wstring::npos) return true;
    }
    return false;
}

winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession SelectActiveMediaSession(
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager const& manager,
    int requestedMediaKind, const Settings& settings) {
    using PlaybackStatus =
        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    auto sessionIsPlaying = [](auto const& candidate) -> bool {
        if (!candidate) {
            return false;
        }
        try {
            return candidate.GetPlaybackInfo().PlaybackStatus() == PlaybackStatus::Playing;
        } catch (...) {
            return false;
        }
    };

    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession current = nullptr;
    try {
        current = manager.GetCurrentSession();
    } catch (...) {
    }

    std::vector<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession> sessions;
    try {
        for (auto const& candidate : manager.GetSessions()) {
            sessions.push_back(candidate);
        }
    } catch (...) {
    }

    auto matchesRequestedKind = [&](auto const& candidate) {
        if (requestedMediaKind < 0) return true;
        const bool isVideo = IsVideoMediaSession(candidate, settings);
        return requestedMediaKind == 1 ? isVideo : !isVideo;
    };

    std::vector<std::wstring> sourceIds;
    for (auto const& candidate : sessions) {
        if (!candidate || !matchesRequestedKind(candidate)) continue;
        try {
            std::wstring id = candidate.SourceAppUserModelId().c_str();
            if (!id.empty() && std::find(sourceIds.begin(), sourceIds.end(), id) == sourceIds.end()) {
                sourceIds.push_back(std::move(id));
            }
        } catch (...) {}
    }
    std::wstring preferredId;
    {
        std::lock_guard lock(g_mediaSourceMutex);
        g_mediaSourceIds = sourceIds;
        preferredId = g_preferredMediaSourceId;
    }
    if (!preferredId.empty()) {
        for (auto const& candidate : sessions) {
            if (!candidate || !matchesRequestedKind(candidate)) continue;
            try {
                if (candidate.SourceAppUserModelId().c_str() == preferredId) return candidate;
            } catch (...) {}
        }
    }

    // Keep Windows' current session preferred when it is playing, then prefer
    // another playing session of the selected type.
    if (sessionIsPlaying(current) && matchesRequestedKind(current)) return current;
    for (auto const& candidate : sessions) {
        if (sessionIsPlaying(candidate) && matchesRequestedKind(candidate)) return candidate;
    }

    if (requestedMediaKind >= 0) {
        // Let a paused session remain visible on its own page. The Video page
        // stays filtered; the primary page falls back below if no audio exists.
        if (current && matchesRequestedKind(current)) return current;
        for (auto const& candidate : sessions) {
            if (candidate && matchesRequestedKind(candidate)) return candidate;
        }
        if (requestedMediaKind == 0) {
            // Keep the primary media pill responsive when only a video session
            // exists. Its actual type is still labeled in the dashboard, and
            // the dedicated Video page remains available for filtered control.
            if (sessionIsPlaying(current)) return current;
            for (auto const& candidate : sessions) {
                if (sessionIsPlaying(candidate)) return candidate;
            }
            if (current) return current;
            if (!sessions.empty()) return sessions.front();
        }
        return nullptr;
    }

    if (current) return current;
    if (sessions.empty()) return nullptr;
    return sessions.front();
}

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
    using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    Manager manager{nullptr};
    bool loggedUnavailable = false;

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        const Settings settings = GetSettingsCopy();
        const int selectedSectionBefore = DashboardSectionIdAtIndex(
            settings, NormalizedTabIndex(settings));
        MediaSnapshot next;

        try {
            if (!manager) {
                manager = Manager::RequestAsync().get();
            }

            if (manager) {
                int requestedMediaKind = -1;
                if (settings.media) {
                    const int sectionId = DashboardSectionIdAtIndex(
                        settings, NormalizedTabIndex(settings));
                    if (sectionId == 6) requestedMediaKind = 0;  // Audio page
                    else if (sectionId == 1) requestedMediaKind = 1;  // Video page
                }
                auto session = SelectActiveMediaSession(manager, requestedMediaKind, settings);
                if (session) {
                    auto properties = session.TryGetMediaPropertiesAsync().get();
                    auto playback = session.GetPlaybackInfo();
                    auto timeline = session.GetTimelineProperties();

                    next.available = true;
                    next.playing = playback.PlaybackStatus() == PlaybackStatus::Playing;
                    next.title = properties.Title().c_str();
                    next.artist = properties.Artist().c_str();
                    next.albumTitle = properties.AlbumTitle().c_str();
                    next.sourceAppUserModelId = session.SourceAppUserModelId().c_str();

                    if (timeline) {
                        int64_t np = timeline.Position().count();
                        int64_t ns = timeline.StartTime().count();
                        int64_t ne = timeline.EndTime().count();
                        int64_t nmin = timeline.MinSeekTime().count();
                        int64_t nmax = timeline.MaxSeekTime().count();
                        bool npP = (playback.PlaybackStatus() == PlaybackStatus::Playing);

                        std::lock_guard lock(g_stateMutex);
                        const bool sameSession = g_state.media.sourceAppUserModelId ==
                                                 next.sourceAppUserModelId;
                        // Some SMTC providers briefly report an empty timeline
                        // while transitioning between Playing and Paused. Keep
                        // the last known duration/range for that same session.
                        if (sameSession && ne <= ns && g_state.media.endTicks > g_state.media.startTicks) {
                            ns = g_state.media.startTicks;
                            ne = g_state.media.endTicks;
                        }
                        if (sameSession && nmax <= nmin &&
                            g_state.media.maxSeekTicks > g_state.media.minSeekTicks) {
                            nmin = g_state.media.minSeekTicks;
                            nmax = g_state.media.maxSeekTicks;
                        }
                        if (np != g_state.media.positionTicks ||
                            ns != g_state.media.startTicks ||
                            ne != g_state.media.endTicks ||
                            nmin != g_state.media.minSeekTicks ||
                            nmax != g_state.media.maxSeekTicks ||
                            npP != g_state.media.playing) {
                            next.positionTicks = np;
                            next.startTicks = ns;
                            next.endTicks = ne;
                            next.minSeekTicks = nmin;
                            next.maxSeekTicks = nmax;
                            next.lastUpdatedTicks = GetTickCount64();
                        } else {
                            next.positionTicks = g_state.media.positionTicks;
                            next.startTicks = g_state.media.startTicks;
                            next.endTicks = g_state.media.endTicks;
                            next.minSeekTicks = g_state.media.minSeekTicks;
                            next.maxSeekTicks = g_state.media.maxSeekTicks;
                            next.lastUpdatedTicks = g_state.media.lastUpdatedTicks;
                        }
                    } else {
                        std::lock_guard lock(g_stateMutex);
                        if (g_state.media.sourceAppUserModelId == next.sourceAppUserModelId) {
                            next.positionTicks = g_state.media.positionTicks;
                            next.startTicks = g_state.media.startTicks;
                            next.endTicks = g_state.media.endTicks;
                            next.minSeekTicks = g_state.media.minSeekTicks;
                            next.maxSeekTicks = g_state.media.maxSeekTicks;
                            next.lastUpdatedTicks = g_state.media.lastUpdatedTicks;
                        }
                    }

                    next.sourceName = FriendlyMediaSourceName(next.sourceAppUserModelId);
                    next.isVideo = IsVideoMediaSession(session, settings);

                    // Fallback for VLC, which often exposes a session with no
                    // Title metadata at all.
                    //
                    // The marker must still be at the END of the caption, just
                    // without insisting on the exact " - " separator that skins
                    // and fullscreen do not reliably produce. Matching the
                    // marker anywhere would happily pick up a browser tab named
                    // "How to use VLC media player - YouTube". The owning
                    // process is checked too, so only a real VLC window counts.
                    if (next.title.empty() && next.sourceName == L"VLC") {
                        const std::wstring marker = L"VLC media player";
                        HWND hwnd = nullptr;
                        while ((hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) != nullptr) {
                            if (!IsWindowVisible(hwnd)) {
                                continue;
                            }

                            wchar_t windowTitle[512];
                            if (GetWindowTextW(hwnd, windowTitle, ARRAYSIZE(windowTitle)) <= 0) {
                                continue;
                            }

                            std::wstring caption(windowTitle);
                            while (!caption.empty() && caption.back() == L' ') {
                                caption.pop_back();
                            }
                            if (caption.size() <= marker.size() ||
                                caption.compare(caption.size() - marker.size(),
                                                marker.size(), marker) != 0) {
                                continue;
                            }

                            std::wstring image;
                            if (!ProcessImageNameForWindow(hwnd, &image) ||
                                ToLowerCopy(BaseNameFromPath(image)).find(L"vlc") == std::wstring::npos) {
                                continue;
                            }

                            // Everything before the app name is the media title,
                            // minus any trailing " - " separator.
                            std::wstring candidate = caption.substr(0, caption.size() - marker.size());
                            while (!candidate.empty() &&
                                   (candidate.back() == L' ' || candidate.back() == L'-')) {
                                candidate.pop_back();
                            }
                            if (!candidate.empty()) {
                                next.title = candidate;
                                break;
                            }
                        }
                    }

                    std::wstring prevSourceAppUserModelId;
                    bool hasPrevIcon = false;
                    BitmapPixels prevIcon;
                    uint64_t prevIconGeneration = 0;
                    std::wstring prevBadge;

                    std::wstring prevTitle;
                    std::wstring prevArtist;
                    BitmapPixels prevArt;
                    uint64_t prevArtGeneration = 0;
                    double prevArtChangedAt = 0.0;
                    double prevTitleChangedAt = 0.0;

                    bool prevPlaying = false;
                    {
                        std::lock_guard lock(g_stateMutex);
                        prevSourceAppUserModelId = g_state.media.sourceAppUserModelId;
                        hasPrevIcon = !g_state.media.sourceIcon.bgra.empty();
                        prevIcon = g_state.media.sourceIcon;
                        prevIconGeneration = g_state.media.sourceIconGeneration;
                        prevBadge = g_state.media.sourceBadge;

                        prevTitle = g_state.media.title;
                        prevArtist = g_state.media.artist;
                        prevArt = g_state.media.art;
                        prevArtGeneration = g_state.media.artGeneration;
                        prevArtChangedAt = g_state.media.artChangedAt;
                        prevTitleChangedAt = g_state.media.titleChangedAt;
                        prevPlaying = g_state.media.playing;
                    }

                    if (next.sourceAppUserModelId == prevSourceAppUserModelId) {
                        next.sourceBadge = prevBadge;
                        next.sourceIcon = prevIcon;
                        next.sourceIconGeneration = prevIconGeneration;

                        if (!hasPrevIcon) {
                            next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                            next.sourceIconGeneration = next.sourceIcon.generation;
                        }
                    } else {
                        next.sourceBadge = MediaSourceBadge(next.sourceName);
                        if (IsBrowserMediaSource(next.sourceAppUserModelId)) {
                            next.sourceBadge = FindBrowserMediaSiteBadge(next.sourceAppUserModelId);
                        }
                        next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                        next.sourceIconGeneration = next.sourceIcon.generation;
                    }

                    // Track when title/artist last changed. Browsers often update
                    // the text metadata a beat before they swap the artwork bytes,
                    // so we keep retrying the art fetch for a short "settle window"
                    // after any track change instead of locking onto the first
                    // (possibly stale/empty) result forever.
                    const bool metadataChanged = (next.title != prevTitle || next.artist != prevArtist || (!prevPlaying && next.playing));
                    next.titleChangedAt = (metadataChanged && !next.title.empty() && next.playing) ? NowSeconds() : prevTitleChangedAt;
                    constexpr double kArtSettleSeconds = 3.0;
                    const bool artSettled = !metadataChanged &&
                        (NowSeconds() - prevTitleChangedAt) > kArtSettleSeconds;

                    if (artSettled && !prevArt.bgra.empty()) {
                        next.art = prevArt;
                        next.artGeneration = prevArtGeneration;
                        next.artChangedAt = prevArtChangedAt;
                    } else if (auto thumbnail = properties.Thumbnail()) {
                        std::vector<uint8_t> bytes = ReadWinRtStreamBytes(thumbnail);
                        if (!bytes.empty()) {
                            BitmapPixels decoded;
                            if (DecodeImageBytesToPixels(bytes, &decoded)) {
                                next.art = std::move(decoded);
                                next.artGeneration = next.art.generation;
                                next.artChangedAt = NowSeconds();
                            }
                        }
                    }

                    // If this particular poll's fetch came back empty (thumbnail
                    // not ready yet), don't flash a blank cover for a track that
                    // hasn't actually changed — keep the old art until a fresh
                    // fetch succeeds. But never do this across an actual track
                    // change, or we're back to showing the wrong song's art.
                    if (next.art.bgra.empty() && !prevArt.bgra.empty() && !metadataChanged) {
                        next.art = prevArt;
                        next.artGeneration = prevArtGeneration;
                        next.artChangedAt = prevArtChangedAt;
                    }
                }
            }
        } catch (...) {
            if (!loggedUnavailable) {
                Wh_Log(L"WinRT media session unavailable; media module will fall back to idle.");
                loggedUnavailable = true;
            }
        }

        bool trackJustChanged = false;
        bool mediaStateChanged = false;
        bool mediaSessionOpened = false;
        bool mediaSessionClosed = false;
        bool mediaSessionChanged = false;
        bool playbackJustStarted = false;
        int activePlaybackId = 6;
        {
            std::lock_guard lock(g_stateMutex);
            mediaSessionOpened = !g_state.media.available && next.available;
            mediaSessionClosed = g_state.media.available && !next.available;
            mediaSessionChanged = g_state.media.available && next.available &&
                (g_state.media.isVideo != next.isVideo ||
                 g_state.media.sourceAppUserModelId != next.sourceAppUserModelId);
            playbackJustStarted = !g_state.media.playing && next.playing;
            mediaStateChanged = g_state.media.available != next.available ||
                                g_state.media.playing != next.playing ||
                                g_state.media.isVideo != next.isVideo ||
                                g_state.media.title != next.title ||
                                g_state.media.sourceAppUserModelId != next.sourceAppUserModelId;
            const bool isDifferentTrack = (!next.title.empty() && next.playing) &&
                (next.title != g_state.media.title || next.artist != g_state.media.artist || (!g_state.media.playing && next.playing));

            if (isDifferentTrack) {
                next.titleChangedAt = NowSeconds();
                trackJustChanged = true;
            }

            if (!g_state.media.art.bgra.empty() &&
                next.title == g_state.media.title && next.artist == g_state.media.artist &&
                next.positionTicks >= g_state.media.positionTicks) {
                next.art = g_state.media.art;
                next.artGeneration = g_state.media.artGeneration;
                next.artChangedAt = g_state.media.artChangedAt;
            }
            if (next.sourceIcon.bgra.empty() &&
                next.sourceAppUserModelId == g_state.media.sourceAppUserModelId) {
                next.sourceIcon = g_state.media.sourceIcon;
                next.sourceIconGeneration = g_state.media.sourceIconGeneration;
            }

            g_state.media = std::move(next);
            activePlaybackId = ActivePlaybackSectionId(settings, g_state);
        }

        if (mediaSessionOpened || mediaSessionChanged || playbackJustStarted) {
            SetDashboardTabIndex(settings, DashboardIndexForSectionId(settings, activePlaybackId));
        } else if (mediaSessionClosed &&
                   (selectedSectionBefore == 1 || selectedSectionBefore == 6)) {
            // When the owning app closes, leave the vanished playback page and
            // return to the primary Time dashboard.
            SetDashboardTabIndex(settings, DashboardIndexForSectionId(settings, 0));
        }

        if (trackJustChanged || mediaStateChanged) {
            g_layoutDirty = true;
            if (trackJustChanged && g_mediaAutoExpandEnabled.load(std::memory_order_relaxed)) {
                TriggerNudge();
            }
        }

        WaitForSingleObject(g_stopEvent, 250);
    }

    winrt::uninit_apartment();
    return 0;
}

typedef LONG NTSTATUS;
typedef NTSTATUS (NTAPI *PWNF_USER_CALLBACK)(
    ULONG64 StateName,
    ULONG ChangeStamp,
    void* TypeId,
    void* CallbackContext,
    const void* Buffer,
    ULONG BufferSize
);

typedef NTSTATUS(NTAPI* PFN_RtlSubscribeWnfStateChangeNotification)(
    void** Subscription,
    ULONG64 StateName,
    ULONG ChangeStamp,
    PWNF_USER_CALLBACK Callback,
    void* CallbackContext,
    const void* TypeId,
    ULONG SerializationGroup,
    ULONG Unknown
);

typedef NTSTATUS(NTAPI* PFN_RtlUnsubscribeWnfStateChangeNotification)(
    void* Subscription
);

typedef NTSTATUS(NTAPI* PFN_NtQueryWnfStateData)(
    const ULONG64* StateName,
    const void* TypeId,
    const void* ExplicitScope,
    ULONG* ChangeStamp,
    void* Buffer,
    ULONG* BufferSize
);

constexpr ULONG64 kWnfQuietHoursActiveProfileChanged = 0xD83063EA3BF1C75ULL;

NTSTATUS NTAPI WnfDndCallback(
    ULONG64 stateName,
    ULONG changeStamp,
    void* typeId,
    void* callbackContext,
    const void* buffer,
    ULONG bufferSize
) {
    if (stateName != kWnfQuietHoursActiveProfileChanged) return 0;
    int val = 0;
    if (buffer && bufferSize >= sizeof(int)) {
        val = *reinterpret_cast<const int*>(buffer);
    }
    const bool active = (val != 0);
    static std::atomic<bool> s_firstWnf = true;
    if (s_firstWnf.exchange(false)) {
        g_isDnDActive.store(active);
        return 0;
    }

    const bool prev = g_isDnDActive.exchange(active);
    if (prev != active && g_settings.doNotDisturbIndicator) {
        {
            std::lock_guard lock(g_stateMutex);
            g_state.doNotDisturb.active = true;
            g_state.doNotDisturb.enabled = active;
            g_state.doNotDisturb.expiresAt = NowSeconds() + 3.0;
        }
        TriggerNudge();
    }
    return 0;
}

void SubscribeDndNotification() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return;

    auto pfnQuery = reinterpret_cast<PFN_NtQueryWnfStateData>(
        GetProcAddress(hNtdll, "NtQueryWnfStateData"));
    if (pfnQuery) {
        ULONG stamp = 0;
        ULONG size = sizeof(int);
        int val = 0;
        ULONG64 stateName = kWnfQuietHoursActiveProfileChanged;
        if (pfnQuery(&stateName, nullptr, nullptr, &stamp, &val, &size) == 0 && size >= sizeof(int)) {
            g_isDnDActive.store(val != 0);
        }
    }

    auto pfnSubscribe = reinterpret_cast<PFN_RtlSubscribeWnfStateChangeNotification>(
        GetProcAddress(hNtdll, "RtlSubscribeWnfStateChangeNotification"));
    if (!pfnSubscribe) return;

    pfnSubscribe(&g_wnfDndSubscription, kWnfQuietHoursActiveProfileChanged, 0,
                 WnfDndCallback, nullptr, nullptr, 0, 0);
}

void UnsubscribeDndNotification() {
    if (!g_wnfDndSubscription) return;
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto pfnUnsubscribe = reinterpret_cast<PFN_RtlUnsubscribeWnfStateChangeNotification>(
            GetProcAddress(hNtdll, "RtlUnsubscribeWnfStateChangeNotification"));
        if (pfnUnsubscribe) {
            pfnUnsubscribe(g_wnfDndSubscription);
        }
    }
    g_wnfDndSubscription = nullptr;
}

#if DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER
DWORD WINAPI NotificationThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using winrt::Windows::UI::Notifications::KnownNotificationBindings;
    using winrt::Windows::UI::Notifications::NotificationKinds;
    using winrt::Windows::UI::Notifications::Management::UserNotificationListener;
    using winrt::Windows::UI::Notifications::Management::UserNotificationListenerAccessStatus;

    std::set<uint32_t> seenIds;
    bool firstPoll = true;
    bool accessLogged = false;

    // The Windows Notification Service (WNS) and UWP subsystem take time to initialize on boot.
    // If this runs too early, instantiating UserNotificationListener::Current()
    // can permanently bind to an uninitialized COM proxy, permanently breaking notifications for the process.
    // To prevent this, we enforce a strict 30-second delay from process creation before touching the API.
    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER ct;
        ct.LowPart = creationTime.dwLowDateTime;
        ct.HighPart = creationTime.dwHighDateTime;

        FILETIME systemTime;
        GetSystemTimeAsFileTime(&systemTime);
        ULARGE_INTEGER st;
        st.LowPart = systemTime.dwLowDateTime;
        st.HighPart = systemTime.dwHighDateTime;

        uint64_t msSinceProcessStart = (st.QuadPart - ct.QuadPart) / 10000;
        if (msSinceProcessStart < 30000) {
            DWORD waitTime = 30000 - (DWORD)msSinceProcessStart;
            Wh_Log(L"Process started recently. Delaying UserNotificationListener init by %d ms to let UWP subsystem load...", waitTime);
            WaitForSingleObject(g_stopEvent, waitTime);
        }
    }

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        try {
            auto listener = UserNotificationListener::Current();
            auto access = listener.RequestAccessAsync().get();
            if (access != UserNotificationListenerAccessStatus::Allowed) {
                if (!accessLogged) {
                    if (access == UserNotificationListenerAccessStatus::Denied) {
                        // Actionable: this is a Windows privacy switch, not a
                        // mod setting, and it is the usual reason the module
                        // stays silent when everything else is configured.
                        Wh_Log(L"Notification listener permission DENIED by Windows. "
                               L"Enable Settings > Privacy & security > Notifications > "
                               L"\"Let apps access your notifications\", then restart the mod. "
                               L"Retrying meanwhile...");
                    } else {
                        Wh_Log(L"Notification listener permission not granted or UWP subsystem not ready on boot (status: %d); retrying connection loop...", (int)access);
                    }
                    accessLogged = true;
                }
                WaitForSingleObject(g_stopEvent, 3000);
                continue;
            }

            if (accessLogged) {
                Wh_Log(L"WinRT UserNotificationListener successfully connected.");
                accessLogged = false;
            }

            while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
                try {
                    auto notifications = listener.GetNotificationsAsync(NotificationKinds::Toast).get();
                    std::set<uint32_t> currentIds;

                    for (uint32_t i = 0; i < notifications.Size(); ++i) {
                        currentIds.insert(notifications.GetAt(i).Id());
                    }

                    if (firstPoll) {
                        seenIds = std::move(currentIds);
                        firstPoll = false;
                        WaitForSingleObject(g_stopEvent, 1000);
                        continue;
                    }

                    for (uint32_t i = 0; i < notifications.Size(); ++i) {
                        try {
                            auto userNotification = notifications.GetAt(i);
                            const uint32_t id = userNotification.Id();

                            if (seenIds.count(id)) {
                                continue;
                            }

                            // Immediately mark as seen so we don't process it again
                            seenIds.insert(id);

                            if (!g_settings.windowsNotifications ||
                                (g_settings.notificationRespectDnD && g_isDnDActive.load())) {
                                continue;
                            }

                            NotificationSnapshot snapshot;
                            snapshot.active = true;
                            snapshot.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
                            auto appInfo = userNotification.AppInfo();
                            auto displayInfo = appInfo.DisplayInfo();
                            snapshot.app = displayInfo.DisplayName().c_str();
                            try {
                                auto logo = displayInfo.GetLogo({32.0f, 32.0f});
                                std::vector<uint8_t> logoBytes = ReadWinRtStreamBytes(logo);
                                if (!logoBytes.empty()) {
                                    DecodeImageBytesToPixels(logoBytes, &snapshot.icon);
                                }
                            } catch (...) {
                            }

                            if (snapshot.icon.bgra.empty() && !snapshot.app.empty()) {
                                snapshot.icon = FindAppIconByName(snapshot.app, 64);
                            }

                            auto notification = userNotification.Notification();
                            auto binding = notification.Visual().GetBinding(KnownNotificationBindings::ToastGeneric());
                            if (binding) {
                                auto textElements = binding.GetTextElements();
                                if (textElements.Size() > 0) {
                                    snapshot.title = textElements.GetAt(0).Text().c_str();
                                }
                                if (textElements.Size() > 1) {
                                    snapshot.body = textElements.GetAt(1).Text().c_str();
                                }
                            }

                            if (snapshot.title.empty()) {
                                snapshot.title = snapshot.app.empty() ? L"New notification" : snapshot.app;
                            }
                            if (!snapshot.body.empty()) {
                                snapshot.title += L" - " + snapshot.body;
                            }
                            if (snapshot.title.size() > 120) {
                                snapshot.title.resize(120);
                                snapshot.title += L"...";
                            }

                            {
                                std::lock_guard lock(g_stateMutex);
                                g_state.notification = std::move(snapshot);
                            }
                            TriggerNudge();
                        } catch (const winrt::hresult_error& nex) {
                            if (nex.to_abi() == 0x80004001 || nex.to_abi() == 0x80040154) { // E_NOTIMPL or REGDB_E_CLASSNOTREG
                                // UWP subsystem not ready, skip without spamming logs
                            } else {
                                Wh_Log(L"Failed to parse a notification (0x%08X); skipping.", nex.to_abi());
                            }
                        } catch (...) {
                            Wh_Log(L"Failed to parse a notification; skipping.");
                        }
                    }

                    seenIds = std::move(currentIds);
                } catch (const winrt::hresult_error& ex) {
                    const HRESULT hr = ex.to_abi();
                    if (hr == 0x80004001 || hr == 0x80040154) { // E_NOTIMPL or REGDB_E_CLASSNOTREG
                        if (!accessLogged) {
                            Wh_Log(L"Notification listener UWP subsystem not fully ready (0x%08X). Retrying in background...", hr);
                            accessLogged = true;
                        }
                    } else {
                        Wh_Log(L"NotificationThreadProc inner loop WinRT error: %s (0x%08X); reconnecting...", ex.message().c_str(), hr);
                    }
                    WaitForSingleObject(g_stopEvent, 3000);
                    break;
                } catch (...) {
                    Wh_Log(L"NotificationThreadProc inner loop unknown exception; reconnecting...");
                    WaitForSingleObject(g_stopEvent, 3000);
                    break;
                }

                WaitForSingleObject(g_stopEvent, 1000);
            }
        } catch (const winrt::hresult_error& ex) {
            if (!accessLogged) {
                Wh_Log(L"NotificationThreadProc connection error: %s (0x%08X). Retrying in 3s...", ex.message().c_str(), ex.to_abi());
                accessLogged = true;
            }
            WaitForSingleObject(g_stopEvent, 3000);
        } catch (...) {
            if (!accessLogged) {
                Wh_Log(L"NotificationThreadProc unknown connection exception. Retrying in 3s...");
                accessLogged = true;
            }
            WaitForSingleObject(g_stopEvent, 3000);
        }
    }

    winrt::uninit_apartment();
    return 0;
}
#endif

#if DYNAMIC_ISLAND_HAS_BLUETOOTH_WATCHER

BluetoothDeviceCategory ClassifyBluetoothDevice(
    const winrt::Windows::Devices::Enumeration::DeviceInformation& info) {
    // 1) Name-based heuristic first — cheap, synchronous, and correct for
    //    the overwhelming majority of consumer devices, which advertise a
    //    descriptive friendly name.
    std::wstring name = ToLowerCopy(std::wstring(info.Name().c_str()));

    if (name.find(L"headphone") != std::wstring::npos ||
        name.find(L"headset") != std::wstring::npos ||
        name.find(L"earbud") != std::wstring::npos ||
        name.find(L"buds") != std::wstring::npos ||
        name.find(L"airpods") != std::wstring::npos) {
        return BluetoothDeviceCategory::Headphones;
    }
    if (name.find(L"speaker") != std::wstring::npos ||
        name.find(L"soundbar") != std::wstring::npos ||
        name.find(L"boombox") != std::wstring::npos) {
        return BluetoothDeviceCategory::Speaker;
    }
    if (name.find(L"mouse") != std::wstring::npos) {
        return BluetoothDeviceCategory::Mouse;
    }
    if (name.find(L"keyboard") != std::wstring::npos) {
        return BluetoothDeviceCategory::Keyboard;
    }
    if (name.find(L"iphone") != std::wstring::npos ||
        name.find(L"phone") != std::wstring::npos ||
        name.find(L"galaxy") != std::wstring::npos ||
        name.find(L"pixel") != std::wstring::npos) {
        return BluetoothDeviceCategory::Phone;
    }

    // 2) Fall back to the classic Bluetooth Class-of-Device major-class bits.
    try {
        using winrt::Windows::Devices::Bluetooth::BluetoothDevice;
        using winrt::Windows::Devices::Bluetooth::BluetoothMajorClass;
        auto device = BluetoothDevice::FromIdAsync(info.Id()).get();
        if (device) {
            switch (device.ClassOfDevice().MajorClass()) {
                case BluetoothMajorClass::Phone:
                    return BluetoothDeviceCategory::Phone;
                case BluetoothMajorClass::AudioVideo:
                    return BluetoothDeviceCategory::Headphones;
                case BluetoothMajorClass::Peripheral:
                    return BluetoothDeviceCategory::Mouse;
                default:
                    break;
            }
        }
    } catch (...) {
        // Not every device id resolves to a classic BluetoothDevice — BLE-only
        // peripherals in particular will throw here. Fall through.
    }

    return BluetoothDeviceCategory::Generic;
}

// Reads the standard GATT Battery Service (0x180F) / Battery Level
// characteristic (0x2A19). Only works for devices that expose battery this
// way — mostly BLE and BLE-dual-mode devices. Classic-only devices will
// fail the GetGattServicesForUuidAsync call and we just report -1 (unknown).
int TryReadBluetoothBatteryPercentBLE(winrt::hstring const& deviceId) {
    using namespace winrt::Windows::Devices::Bluetooth;
    using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

    try {
        auto bleDevice = BluetoothLEDevice::FromIdAsync(deviceId).get();
        if (!bleDevice) {
            return -1;
        }

        auto servicesResult = bleDevice.GetGattServicesForUuidAsync(
            GattServiceUuids::Battery(), BluetoothCacheMode::Uncached).get();
        if (servicesResult.Status() != GattCommunicationStatus::Success ||
            servicesResult.Services().Size() == 0) {
            return -1;
        }

        auto service = servicesResult.Services().GetAt(0);
        auto charsResult = service.GetCharacteristicsForUuidAsync(
            GattCharacteristicUuids::BatteryLevel(), BluetoothCacheMode::Uncached).get();
        if (charsResult.Status() != GattCommunicationStatus::Success ||
            charsResult.Characteristics().Size() == 0) {
            return -1;
        }

        auto characteristic = charsResult.Characteristics().GetAt(0);
        auto readResult = characteristic.ReadValueAsync(BluetoothCacheMode::Uncached).get();
        if (readResult.Status() != GattCommunicationStatus::Success) {
            return -1;
        }

        auto buffer = readResult.Value();
        if (buffer.Length() < 1) {
            return -1;
        }

        auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
        uint8_t raw = reader.ReadByte();
        return ClampInt(static_cast<int>(raw), 0, 100);
    } catch (...) {
        return -1;
    }
}

// Windows' own Settings > Bluetooth & devices page shows a battery percentage
// for most classic (non-BLE) headphones/earbuds/speakers using an
// undocumented per-devnode property exposed by the Microsoft Bluetooth
// classic driver stack — not GATT. This is the same property those battery
// tray-icon utilities read. Query it via SetupAPI on the Bluetooth-class
// devnode whose instance ID embeds the device's Bluetooth address.
static const GUID kGuidDevClassBluetooth = {
    0xe0cbf06c, 0xcd8b, 0x4647, {0xbb, 0x8a, 0x26, 0x3b, 0x43, 0xf0, 0xf9, 0x74}};

static const DEVPROPKEY PKEY_Bluetooth_Battery = {
    {0x104ea319, 0x6ee2, 0x4701, {0xbd, 0x47, 0x8d, 0xdb, 0xf4, 0x25, 0xbb, 0xe5}}, 2};

int TryReadClassicBluetoothBatteryPercent(uint64_t address) {
    if (!address) {
        Wh_Log(L"BT battery: no address to search for.");
        return -1;
    }

    wchar_t addrHex[16] = {};
    swprintf_s(addrHex, L"%012llX", static_cast<unsigned long long>(address));
    const std::wstring addrLower = ToLowerCopy(addrHex);
    Wh_Log(L"BT battery: searching devnodes for address %s", addrLower.c_str());

    // Enumerate everything the Bluetooth bus driver (BTHENUM) exposes,
    // regardless of which device setup class it landed in. This is broader
    // than filtering by GUID_DEVCLASS_BLUETOOTH and matches what battery
    // tray utilities do.
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsExW(
        nullptr, L"BTHENUM", nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT,
        nullptr, nullptr, nullptr);

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        Wh_Log(L"BT battery: SetupDiGetClassDevsExW(BTHENUM) failed (0x%lx), falling back to class GUID.", GetLastError());
        deviceInfoSet = SetupDiGetClassDevsW(&kGuidDevClassBluetooth, nullptr, nullptr, DIGCF_PRESENT);
        if (deviceInfoSet == INVALID_HANDLE_VALUE) {
            Wh_Log(L"BT battery: fallback enumeration also failed.");
            return -1;
        }
    }

    int result = -1;
    int matchedCount = 0;
    SP_DEVINFO_DATA devInfoData = {};
    devInfoData.cbSize = sizeof(devInfoData);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &devInfoData); ++i) {
        wchar_t instanceId[512] = {};
        if (!SetupDiGetDeviceInstanceIdW(deviceInfoSet, &devInfoData, instanceId,
                                         ARRAYSIZE(instanceId), nullptr)) {
            continue;
        }

        if (ToLowerCopy(instanceId).find(addrLower) == std::wstring::npos) {
            continue;
        }

        ++matchedCount;
        Wh_Log(L"BT battery: matched devnode %s", instanceId);

        DEVPROPTYPE propType = 0;
        BYTE battery = 0;
        DWORD required = 0;
        if (SetupDiGetDevicePropertyW(deviceInfoSet, &devInfoData, &PKEY_Bluetooth_Battery,
                                      &propType, &battery, sizeof(battery), &required, 0)) {
            Wh_Log(L"BT battery: property present on this devnode, type=%lu value=%u", propType, battery);
            if (propType == DEVPROP_TYPE_BYTE && battery != 0xFF) {
                result = ClampInt(static_cast<int>(battery), 0, 100);
                break;
            }
        } else {
            Wh_Log(L"BT battery: PKEY_Bluetooth_Battery not set on this devnode yet (error 0x%lx).", GetLastError());
        }
    }

    if (matchedCount == 0) {
        Wh_Log(L"BT battery: no devnode instance ID contained address %s.", addrLower.c_str());
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return result;
}

int TryReadBluetoothBatteryPercent(winrt::hstring const& deviceId) {
    int result = -1;

    // Try the classic per-devnode battery property first — this is what
    // Settings > Bluetooth & devices reads, and it's what covers most
    // headsets/earbuds/speakers that pair over classic Bluetooth (BR/EDR)
    // rather than BLE.
    try {
        using winrt::Windows::Devices::Bluetooth::BluetoothDevice;
        auto classicDevice = BluetoothDevice::FromIdAsync(deviceId).get();
        if (classicDevice) {
            result = TryReadClassicBluetoothBatteryPercent(classicDevice.BluetoothAddress());
        }
    } catch (...) {
        // Not resolvable as a classic BluetoothDevice (BLE-only peripheral) — fall through.
    }

    if (result < 0) {
        // Fall back to BLE GATT Battery Service for BLE / dual-mode devices.
        result = TryReadBluetoothBatteryPercentBLE(deviceId);
    }

    if (result >= 0) {
        // Remember this reading so a later disconnect (when the device can
        // no longer be queried) can still show the last known level.
        std::lock_guard lock(g_bluetoothBatteryCacheMutex);
        g_bluetoothBatteryCache[std::wstring(deviceId.c_str())] = result;
    }

    return result;
}

// Returns the last battery percent we successfully read for this device
// while it was connected, or -1 if we never learned one.
int GetLastKnownBluetoothBatteryPercent(const std::wstring& deviceId) {
    std::lock_guard lock(g_bluetoothBatteryCacheMutex);
    auto it = g_bluetoothBatteryCache.find(deviceId);
    return it != g_bluetoothBatteryCache.end() ? it->second : -1;
}

// Small per-device cache so a Removed event (which only carries an Id, not
// a full DeviceInformation) can still show a name/icon on disconnect.
struct BluetoothTrackedDevice {
    std::wstring name;
    BluetoothDeviceCategory category;
};

void HandleBluetoothConnected(
    const winrt::Windows::Devices::Enumeration::DeviceInformation& info,
    std::unordered_map<std::wstring, BluetoothTrackedDevice>& cache,
    std::mutex& cacheMutex) {
    std::wstring name = info.Name().c_str();
    if (name.empty()) {
        return;
    }

    BluetoothDeviceCategory category = ClassifyBluetoothDevice(info);
    {
        std::lock_guard lock(cacheMutex);
        cache[std::wstring(info.Id().c_str())] = BluetoothTrackedDevice{name, category};
    }

    if (!g_settings.bluetoothIndicator) {
        return;
    }

    Wh_Log(L"Bluetooth: connected - %s", name.c_str());

    const uint64_t myGeneration = ++g_bluetoothConnectGeneration;
    const std::wstring deviceId = info.Id().c_str();

    BluetoothDeviceSnapshot snapshot;
    snapshot.active = true;
    snapshot.connected = true;
    snapshot.deviceName = name;
    snapshot.category = category;
    snapshot.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
    snapshot.batteryPercent = g_settings.bluetoothShowBattery
        ? TryReadBluetoothBatteryPercent(info.Id())
        : -1;

    {
        std::lock_guard lock(g_stateMutex);
        g_state.bluetoothDevice = std::move(snapshot);
    }
    TriggerNudge();

    // Windows often hasn't populated the battery property at the exact
    // instant the connection event fires — it needs a moment to actually
    // query the device. Keep retrying in the background for a while; if a
    // value shows up and this connection is still the one being displayed,
    // patch it into the live state and re-render.
    if (g_settings.bluetoothShowBattery && snapshot.batteryPercent < 0) {
        std::thread([deviceId, myGeneration]() {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            for (int attempt = 0; attempt < 6; ++attempt) {
                Sleep(1500);
                if (g_bluetoothConnectGeneration.load() != myGeneration) {
                    break;  // a newer connect/disconnect event superseded this one
                }

                int battery = TryReadBluetoothBatteryPercent(winrt::hstring(deviceId));
                if (battery >= 0) {
                    std::lock_guard lock(g_stateMutex);
                    if (g_bluetoothConnectGeneration.load() == myGeneration &&
                        g_state.bluetoothDevice.connected) {
                        g_state.bluetoothDevice.batteryPercent = battery;
                        Wh_Log(L"Bluetooth: battery arrived late (%d%%) on retry %d.", battery, attempt + 1);
                    }
                    TriggerNudge();
                    break;
                }
            }
            winrt::uninit_apartment();
        }).detach();
    }
}

void HandleBluetoothDisconnected(
    winrt::hstring const& id,
    std::unordered_map<std::wstring, BluetoothTrackedDevice>& cache,
    std::mutex& cacheMutex) {
    ++g_bluetoothConnectGeneration;  // cancel any pending battery retry for the old connection

    if (!g_settings.bluetoothIndicator) {
        return;
    }

    std::wstring name;
    BluetoothDeviceCategory category = BluetoothDeviceCategory::Generic;
    {
        std::lock_guard lock(cacheMutex);
        auto it = cache.find(std::wstring(id.c_str()));
        if (it != cache.end()) {
            name = it->second.name;
            category = it->second.category;
        }
    }
    if (name.empty()) {
        name = L"Bluetooth Device";
    }

    Wh_Log(L"Bluetooth: disconnected - %s", name.c_str());

    BluetoothDeviceSnapshot snapshot;
    snapshot.active = true;
    snapshot.connected = false;
    snapshot.deviceName = name;
    snapshot.category = category;
    snapshot.batteryPercent = GetLastKnownBluetoothBatteryPercent(std::wstring(id.c_str()));
    snapshot.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);

    {
        std::lock_guard lock(g_stateMutex);
        g_state.bluetoothDevice = std::move(snapshot);
    }
    TriggerNudge();
}

DWORD WINAPI BluetoothThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using winrt::Windows::Devices::Enumeration::DeviceInformation;
    using winrt::Windows::Devices::Enumeration::DeviceInformationUpdate;
    using winrt::Windows::Devices::Enumeration::DeviceWatcher;
    using winrt::Windows::Devices::Bluetooth::BluetoothDevice;
    using winrt::Windows::Devices::Bluetooth::BluetoothLEDevice;
    using winrt::Windows::Devices::Bluetooth::BluetoothConnectionStatus;

    std::unordered_map<std::wstring, BluetoothTrackedDevice> deviceCache;
    std::mutex cacheMutex;
    std::vector<DeviceWatcher> watchers;

    // Watching the "Connected" selector directly means a device APPEARING
    // in the watcher (Added) is a connect, and DISAPPEARING (Removed) is a
    // disconnect — no property polling or IsConnected lookups required.
    auto startWatcher = [&](winrt::hstring const& selector, const wchar_t* label) {
        try {
            DeviceWatcher watcher = DeviceInformation::CreateWatcher(selector);
            auto enumDone = std::make_shared<std::atomic<bool>>(false);

            watcher.EnumerationCompleted(
                [enumDone](DeviceWatcher const&, winrt::Windows::Foundation::IInspectable const&) {
                    *enumDone = true;
                });

            watcher.Added([&deviceCache, &cacheMutex, enumDone](
                              DeviceWatcher const&, DeviceInformation const& info) {
                // Devices reported before EnumerationCompleted are the
                // watcher's initial snapshot (already connected when the mod
                // started) — cache them silently so a later disconnect still
                // resolves a name, but don't pop a card for a connection the
                // user didn't just cause.
                if (!*enumDone) {
                    std::wstring name = info.Name().c_str();
                    if (!name.empty()) {
                        std::lock_guard lock(cacheMutex);
                        deviceCache[std::wstring(info.Id().c_str())] =
                            BluetoothTrackedDevice{name, ClassifyBluetoothDevice(info)};
                    }
                    return;
                }
                HandleBluetoothConnected(info, deviceCache, cacheMutex);
            });

            watcher.Removed([&deviceCache, &cacheMutex](
                                DeviceWatcher const&, DeviceInformationUpdate const& update) {
                HandleBluetoothDisconnected(update.Id(), deviceCache, cacheMutex);
            });

            watcher.Start();
            watchers.push_back(watcher);
            Wh_Log(L"Bluetooth: %s watcher started.", label);
        } catch (...) {
            Wh_Log(L"Bluetooth: failed to start %s watcher.", label);
        }
    };

    startWatcher(
        BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected),
        L"classic");
    startWatcher(
        BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected),
        L"BLE");

    // DeviceWatcher does its work via WinRT callbacks on background threads;
    // this thread just needs to stay alive to keep the watchers rooted
    // until shutdown is signaled.
    while (WaitForSingleObject(g_stopEvent, 1000) == WAIT_TIMEOUT) {
    }

    for (auto& watcher : watchers) {
        try {
            watcher.Stop();
        } catch (...) {
        }
    }

    winrt::uninit_apartment();
    return 0;
}

#else  // !DYNAMIC_ISLAND_HAS_BLUETOOTH_WATCHER

DWORD WINAPI BluetoothThreadProc(void*) {
    // SDK used to build this mod doesn't expose the WinRT Bluetooth headers;
    // the indicator silently stays inactive instead of failing the mod.
    return 0;
}

#endif  // DYNAMIC_ISLAND_HAS_BLUETOOTH_WATCHER

float SampleAudioAmplitude(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format || !format->nChannels) {
        return 0.0f;
    }

    double sum = 0.0;
    size_t samples = static_cast<size_t>(frames) * format->nChannels;

    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX) &&
         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat,
                     kSubTypeIeeeFloat))) {
        auto* f = reinterpret_cast<float*>(data);
        for (size_t i = 0; i < samples; ++i) {
            sum += f[i] * f[i];
        }
    } else if (format->wBitsPerSample == 16) {
        auto* s = reinterpret_cast<int16_t*>(data);
        for (size_t i = 0; i < samples; ++i) {
            const double v = s[i] / 32768.0;
            sum += v * v;
        }
    }

    const double rms = samples ? std::sqrt(sum / samples) : 0.0;
    return Clamp(static_cast<float>(rms * 4.0), 0.0f, 1.0f);
}

void PushWaveformSample(float amplitude) {
    std::lock_guard lock(g_stateMutex);

    float lastVal = 0.0f;
    if (g_state.waveformWrite > 0) {
        lastVal = g_state.waveform[(g_state.waveformWrite - 1) % g_state.waveform.size()];
    }

    // Apply an attack/release envelope (Exponential Moving Average)
    // Quick snappy attack (0.7) for beats, buttery smooth release (0.85) for decay.
    float smoothed;
    if (amplitude > lastVal) {
        smoothed = lastVal * 0.3f + amplitude * 0.7f;
    } else {
        smoothed = lastVal * 0.85f + amplitude * 0.15f;
    }

    g_state.waveform[g_state.waveformWrite % g_state.waveform.size()] = smoothed;
    ++g_state.waveformWrite;
}

void PushAudioChunks(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format || !format->nChannels) {
        PushWaveformSample(0.0f);
        return;
    }

    constexpr UINT32 chunkFrames = 64;
    const UINT32 channels = format->nChannels;
    const bool isFloat =
        format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX) &&
         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat,
                     kSubTypeIeeeFloat));
    const bool is16 = format->wBitsPerSample == 16;

    if (!isFloat && !is16) {
        PushWaveformSample(SampleAudioAmplitude(data, frames, format));
        return;
    }

    for (UINT32 frame = 0; frame < frames; frame += chunkFrames) {
        const UINT32 chunk = std::min(chunkFrames, frames - frame);
        double sum = 0.0;
        const size_t samples = static_cast<size_t>(chunk) * channels;
        const size_t start = static_cast<size_t>(frame) * channels;

        if (isFloat) {
            auto* f = reinterpret_cast<float*>(data);
            for (size_t i = 0; i < samples; ++i) {
                const double v = f[start + i];
                sum += v * v;
            }
        } else {
            auto* s = reinterpret_cast<int16_t*>(data);
            for (size_t i = 0; i < samples; ++i) {
                const double v = s[start + i] / 32768.0;
                sum += v * v;
            }
        }

        PushWaveformSample(Clamp(static_cast<float>(std::sqrt(sum / samples) * 4.0), 0.0f, 1.0f));
    }
}

// --- Weather Fetching Helpers ---
std::string HttpGet(const wchar_t* host, const wchar_t* path, bool https = true) {
    std::string response;
    HINTERNET hSession = WinHttpOpen(L"DynamicIsland/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        Wh_Log(L"Weather HttpGet: WinHttpOpen failed with error %lu", GetLastError());
        return response;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, host, https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
    if (hConnect) {
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, https ? WINHTTP_FLAG_SECURE : 0);
        if (hRequest) {
            if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, nullptr)) {
                DWORD size = 0;
                DWORD downloaded = 0;
                do {
                    if (WinHttpQueryDataAvailable(hRequest, &size) && size > 0) {
                        std::vector<char> buffer(size + 1);
                        if (WinHttpReadData(hRequest, buffer.data(), size, &downloaded)) {
                            buffer[downloaded] = '\0';
                            response.append(buffer.data());
                        } else {
                            Wh_Log(L"Weather HttpGet: WinHttpReadData failed with error %lu", GetLastError());
                        }
                    }
                } while (size > 0);
            } else {
                Wh_Log(L"Weather HttpGet: WinHttpSendRequest/ReceiveResponse failed with error %lu", GetLastError());
            }
            WinHttpCloseHandle(hRequest);
        } else {
            Wh_Log(L"Weather HttpGet: WinHttpOpenRequest failed with error %lu", GetLastError());
        }
        WinHttpCloseHandle(hConnect);
    } else {
        Wh_Log(L"Weather HttpGet: WinHttpConnect failed with error %lu", GetLastError());
    }
    WinHttpCloseHandle(hSession);
    return response;
}

// Percent-encodes a city name for use as a wttr.in path segment. Only spaces
// used to be escaped, so a city containing a comma, an accent or any other
// non-ASCII character produced a malformed request; wttr.in then answered with
// an error page that the parser happily turned into "0 degrees".
std::wstring PercentEncodeUtf8(const std::wstring& text) {
    // Some users worked around the old space-only escaping by typing the escape
    // themselves ("New%20York"). Re-encoding that would send "New%2520York", so
    // an already-encoded value is passed through untouched. Only the exact
    // "%XX" shape counts, so a literal percent sign in a name still encodes.
    auto looksPreEncoded = [](const std::wstring& value) {
        const size_t pos = value.find(L'%');
        if (pos == std::wstring::npos || pos + 2 >= value.size()) {
            return false;
        }
        return iswxdigit(value[pos + 1]) && iswxdigit(value[pos + 2]);
    };
    if (looksPreEncoded(text)) {
        return text;
    }

    const int bytes = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (bytes <= 1) {
        return std::wstring();
    }

    std::string utf8(static_cast<size_t>(bytes - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, utf8.data(), bytes, nullptr, nullptr);

    static constexpr wchar_t kHexDigits[] = L"0123456789ABCDEF";
    std::wstring out;
    out.reserve(utf8.size() * 3);
    for (const unsigned char c : utf8) {
        const bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                                (c >= '0' && c <= '9') ||
                                c == '-' || c == '_' || c == '.' || c == '~';
        if (unreserved) {
            out.push_back(static_cast<wchar_t>(c));
        } else {
            out.push_back(L'%');
            out.push_back(kHexDigits[(c >> 4) & 0x0f]);
            out.push_back(kHexDigits[c & 0x0f]);
        }
    }
    return out;
}

DWORD WINAPI WeatherThreadProc(void*) {
    // Initial delay to avoid slowing down startup
    WaitForSingleObject(g_stopEvent, 3000);

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        std::wstring cityOverride;
        bool isFahrenheit = false;
        bool weatherEnabled = true;
        {
            // g_settingsMutex, not g_stateMutex: all three values below live in
            // g_settings, so the old g_stateMutex here guarded nothing and left
            // the weatherCity string copy racing against LoadSettings().
            std::lock_guard lock(g_settingsMutex);
            weatherEnabled = g_settings.weather;
            cityOverride = g_settings.weatherCity;
            isFahrenheit = g_settings.weatherFahrenheit;
        }

        if (!weatherEnabled) {
            WaitForSingleObject(g_stopEvent, 2000);
            continue;
        }

        std::wstring url = L"/?format=j1";
        if (!cityOverride.empty()) {
            const std::wstring encodedCity = PercentEncodeUtf8(cityOverride);
            if (!encodedCity.empty()) {
                url = L"/" + encodedCity + L"?format=j1";
            }
        }

        Wh_Log(L"Weather: Requesting weather from wttr.in/host: wttr.in, path: %s", url.c_str());
        std::string wRes = HttpGet(L"wttr.in", url.c_str(), true);
        if (wRes.empty()) {
            Wh_Log(L"Weather: HTTPS request failed, retrying over plain HTTP...");
            wRes = HttpGet(L"wttr.in", url.c_str(), false);
        }

        bool weatherCommitted = false;

        if (!wRes.empty()) {
            Wh_Log(L"Weather: Received response from wttr.in (size: %zu bytes)", wRes.size());
            float temp = 0.0f;
            int code = 0;
            std::wstring desc = L"";
            std::wstring windSpeed = L"";
            std::wstring windDir = L"";
            std::wstring humidity = L"";
            std::wstring feelsLike = L"";
            std::wstring cityLabel = L"Local Weather";

            const char* areaStr = strstr(wRes.c_str(), "\"areaName\":");
            if (areaStr) {
                const char* valStr = strstr(areaStr, "\"value\":");
                if (valStr) {
                    valStr += 8;
                    while (*valStr == ' ' || *valStr == '\"') valStr++;
                    const char* end = strchr(valStr, '\"');
                    if (end) {
                        std::string cityA(valStr, end - valStr);
                        int wchars_num = MultiByteToWideChar(CP_UTF8, 0, cityA.c_str(), -1, NULL, 0);
                        if (wchars_num > 0) {
                            std::vector<wchar_t> wstr(wchars_num);
                            MultiByteToWideChar(CP_UTF8, 0, cityA.c_str(), -1, &wstr[0], wchars_num);
                            cityLabel = wstr.data();
                        }
                    }
                }
            }

            const char* currentStr = strstr(wRes.c_str(), "\"current_condition\":");
            if (currentStr) {
                auto ParseStringField = [&](const char* key, std::wstring& out) {
                    const char* kStr = strstr(currentStr, key);
                    if (kStr) {
                        kStr += strlen(key);
                        while (*kStr == ' ' || *kStr == '\"' || *kStr == ':') kStr++;
                        const char* end = strchr(kStr, '\"');
                        if (end) {
                            std::string valA(kStr, end - kStr);
                            int wchars_num = MultiByteToWideChar(CP_UTF8, 0, valA.c_str(), -1, NULL, 0);
                            if (wchars_num > 0) {
                                std::vector<wchar_t> wstr(wchars_num);
                                MultiByteToWideChar(CP_UTF8, 0, valA.c_str(), -1, &wstr[0], wchars_num);
                                out = wstr.data();
                            }
                        }
                    }
                };

                const char* tempStr = strstr(currentStr, isFahrenheit ? "\"temp_F\":" : "\"temp_C\":");
                if (tempStr) {
                    tempStr += 9;
                    while (*tempStr == ' ' || *tempStr == '\"') tempStr++;
                    sscanf(tempStr, "%f", &temp);
                }
                const char* codeStr = strstr(currentStr, "\"weatherCode\":");
                if (codeStr) {
                    codeStr += 14;
                    while (*codeStr == ' ' || *codeStr == '\"') codeStr++;
                    sscanf(codeStr, "%d", &code);
                }

                const char* descStr = strstr(currentStr, "\"weatherDesc\":");
                if (descStr) {
                    const char* valStr = strstr(descStr, "\"value\":");
                    if (valStr) {
                        valStr += 8;
                        while (*valStr == ' ' || *valStr == '\"') valStr++;
                        const char* end = strchr(valStr, '\"');
                        if (end) {
                            std::string valA(valStr, end - valStr);
                            int wchars_num = MultiByteToWideChar(CP_UTF8, 0, valA.c_str(), -1, NULL, 0);
                            if (wchars_num > 0) {
                                std::vector<wchar_t> wstr(wchars_num);
                                MultiByteToWideChar(CP_UTF8, 0, valA.c_str(), -1, &wstr[0], wchars_num);
                                desc = wstr.data();
                                while(!desc.empty() && desc.back() == L' ') desc.pop_back();
                            }
                        }
                    }
                }

                ParseStringField(isFahrenheit ? "\"windspeedMiles\"" : "\"windspeedKmph\"", windSpeed);
                ParseStringField("\"winddir16Point\"", windDir);
                ParseStringField("\"humidity\"", humidity);
                ParseStringField(isFahrenheit ? "\"FeelsLikeF\"" : "\"FeelsLikeC\"", feelsLike);

                // Treat the response as usable only if a real reading came out
                // of it. wttr.in answers rate limiting and unknown locations
                // with a page that contains no current_condition (or an empty
                // one); committing that regardless is what made the island
                // display a confident, wrong "0 degrees".
                // 0 is not a valid WWO weather code, so it doubles as a
                // "nothing was parsed" sentinel here.
                weatherCommitted = (code != 0) || !desc.empty();

                std::wstring finalCity = cityOverride.empty() ? cityLabel : cityOverride;
                if (weatherCommitted) {
                    Wh_Log(L"Weather parsed success: city=%s, temp=%.1f, feelsLike=%s, humidity=%s%%, desc=%s",
                           finalCity.c_str(), temp, feelsLike.c_str(), humidity.c_str(), desc.c_str());
                } else {
                    Wh_Log(L"Weather: response had no usable reading, keeping previous data.");
                }
            } else {
                Wh_Log(L"Weather: Failed to find \"current_condition\" in response.");
            }

            if (weatherCommitted) {
                std::lock_guard lock(g_stateMutex);
                g_state.weather.hasData = true;
                g_state.weather.temperature = temp;
                g_state.weather.weatherCode = code;
                if (!cityOverride.empty()) g_state.weather.city = cityOverride;
                else g_state.weather.city = cityLabel;
                g_state.weather.weatherDesc = desc;
                g_state.weather.windSpeed = windSpeed;
                g_state.weather.windDir = windDir;
                g_state.weather.humidity = humidity;
                g_state.weather.feelsLike = feelsLike;
                g_state.weather.lastUpdated = NowSeconds();
            }
        } else {
            Wh_Log(L"Weather: HttpGet returned empty response.");
        }

        // Retry soon after a failure instead of leaving the dashboard empty for
        // a full refresh interval, but back off on repeated failures. wttr.in is
        // a free community service and rate limiting is one of the failures being
        // recovered from here, so a flat fast retry would sustain the very
        // condition it is reacting to.
        static const DWORD kBackoffMs[] = {60 * 1000, 2 * 60 * 1000, 5 * 60 * 1000, 15 * 60 * 1000};
        static size_t backoffIndex = 0;

        DWORD waitMs;
        if (weatherCommitted) {
            backoffIndex = 0;
            waitMs = 15 * 60 * 1000;
        } else {
            waitMs = kBackoffMs[backoffIndex];
            if (backoffIndex + 1 < ARRAYSIZE(kBackoffMs)) {
                ++backoffIndex;
            }
        }

        HANDLE events[] = {g_stopEvent, g_settingsChangedEvent};
        DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, waitMs);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_OBJECT_0 + 1) {
            // The city changed, so retry immediately at full speed.
            backoffIndex = 0;
        }
    }
    return 0;
}

DWORD WINAPI AudioThreadProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        // Don't touch the audio engine at all unless a playing waveform is
        // actually going to be shown right now. Cheap poll, no capture open.
        if (!g_audioCaptureNeeded.load(std::memory_order_relaxed)) {
            WaitForSingleObject(g_stopEvent, 250);
            continue;
        }

        ComPtr<IMMDeviceEnumerator> enumerator;
        ComPtr<IMMDevice> device;
        ComPtr<IAudioClient> client;
        ComPtr<IAudioCaptureClient> capture;
        WAVEFORMATEX* mixFormat = nullptr;

        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                      CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (SUCCEEDED(hr)) {
            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        }
        if (SUCCEEDED(hr)) {
            hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                  reinterpret_cast<void**>(client.GetAddressOf()));
        }
        if (SUCCEEDED(hr)) {
            hr = client->GetMixFormat(&mixFormat);
        }
        if (SUCCEEDED(hr)) {
            REFERENCE_TIME bufferDuration = 10000000 / 5;
            hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                    AUDCLNT_STREAMFLAGS_LOOPBACK,
                                    bufferDuration, 0, mixFormat, nullptr);
        }
        if (SUCCEEDED(hr)) {
            hr = client->GetService(IID_PPV_ARGS(&capture));
        }
        if (SUCCEEDED(hr)) {
            hr = client->Start();
        }

        if (FAILED(hr)) {
            if (mixFormat) {
                CoTaskMemFree(mixFormat);
            }
            WaitForSingleObject(g_stopEvent, 2500);
            continue;
        }

        // Watchdog state — reset every time a capture session (re)starts so we
        // don't flag a stall before the first packet has even arrived.
        double lastPacketTime = NowSeconds();
        double nextWatchdogCheck = lastPacketTime + 1.0;

        while (WaitForSingleObject(g_stopEvent, 16) == WAIT_TIMEOUT) {
            if (!g_audioCaptureNeeded.load(std::memory_order_relaxed)) {
                break;
            }
            UINT32 packetFrames = 0;
            if (FAILED(capture->GetNextPacketSize(&packetFrames))) {
                break;
            }

            float amplitude = 0.0f;
            int packets = 0;
            while (packetFrames > 0) {
                BYTE* data = nullptr;
                UINT32 frames = 0;
                DWORD flags = 0;
                if (FAILED(capture->GetBuffer(&data, &frames, &flags, nullptr, nullptr))) {
                    break;
                }

                lastPacketTime = NowSeconds();

                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                    amplitude = std::max(amplitude, SampleAudioAmplitude(data, frames, mixFormat));
                    PushAudioChunks(data, frames, mixFormat);
                } else {
                    PushWaveformSample(0.0f);
                }
                capture->ReleaseBuffer(frames);
                ++packets;

                if (FAILED(capture->GetNextPacketSize(&packetFrames))) {
                    packetFrames = 0;
                }
            }

            if (packets > 0) {
                if (amplitude <= 0.001f) {
                    PushWaveformSample(0.0f);
                }
            } else {
                PushWaveformSample(0.0f);
            }

            // --- Stall watchdog -------------------------------------------
            // Checked at most once a second (cheap: one time comparison plus
            // a mutex-protected bool read, same cost as work already done
            // elsewhere in this loop). Only forces a reconnect if SMTC says
            // media is playing but we've had zero packets for 5+ seconds —
            // that combination almost never happens unless the loopback
            // engine has stalled, so it won't fire during normal silence.
            const double now = NowSeconds();
            if (now >= nextWatchdogCheck) {
                nextWatchdogCheck = now + 1.0;
                bool mediaPlayingNow = false;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaPlayingNow = g_state.media.playing;
                }
                if (mediaPlayingNow && (now - lastPacketTime) > 5.0) {
                    Wh_Log(L"Audio loopback capture appears stalled while media is playing; reconnecting...");
                    break;
                }
            }
        }

        client->Stop();
        if (mixFormat) {
            CoTaskMemFree(mixFormat);
        }
    }

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }

    return 0;
}
void UpdateBatterySnapshot() {
    SYSTEM_POWER_STATUS status = {};
    if (!GetSystemPowerStatus(&status)) {
        return;
    }

    bool newCharging = (status.ACLineStatus == 1);
    int newPercent = status.BatteryLifePercent == 255 ? 100 : status.BatteryLifePercent;

    bool triggerAlert = false;

    {
        std::lock_guard lock(g_stateMutex);
        static bool s_batteryInit = false;
        if (!s_batteryInit) {
            g_state.battery.charging = newCharging;
            g_state.battery.percent = newPercent;
            s_batteryInit = true;
        }

        if (g_state.battery.charging != newCharging) {
            triggerAlert = true;
        }

        if (!newCharging && newPercent < g_state.battery.percent && (newPercent == 20 || newPercent == 10)) {
            triggerAlert = true;
        }

        g_state.battery.charging = newCharging;
        g_state.battery.percent = newPercent;
        g_state.battery.secondsRemaining = status.BatteryLifeTime;
        g_state.battery.low = (!newCharging && newPercent <= 20);

        if (triggerAlert) {
            g_state.battery.active = true;
            g_state.battery.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
        }
    }

    if (triggerAlert) {
        TriggerNudge();
    }
}

ULONGLONG FileTimeToUInt64(FILETIME ft) {
    ULARGE_INTEGER value = {};
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;
    return value.QuadPart;
}

static PDH_HQUERY g_gpuQuery = NULL;
static PDH_HCOUNTER g_gpuCounter = NULL;

static void InitGpuQuery() {
    if (g_gpuQuery == NULL) {
        if (PdhOpenQueryW(NULL, 0, &g_gpuQuery) == ERROR_SUCCESS) {
            PdhAddEnglishCounterW(g_gpuQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_gpuCounter);
            PdhCollectQueryData(g_gpuQuery);
        }
    }
}

static int GetGpuUsage() {
    InitGpuQuery();
    if (!g_gpuQuery || !g_gpuCounter) return 0;

    PdhCollectQueryData(g_gpuQuery);

    DWORD bufferSize = 0;
    DWORD itemCount = 0;
    PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, NULL);

    if (bufferSize > 0) {
        std::vector<BYTE> buffer(bufferSize);
        PDH_FMT_COUNTERVALUE_ITEM_W* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());

        if (PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items) == ERROR_SUCCESS) {
            double total = 0;
            for (DWORD i = 0; i < itemCount; i++) {
                if (items[i].szName && wcsstr(items[i].szName, L"engtype_3D")) {
                    total += items[i].FmtValue.doubleValue;
                }
            }
            return ClampInt(static_cast<int>(total), 0, 100);
        }
    }
    return 0;
}

static PDH_HQUERY g_netQuery = NULL;
static PDH_HCOUNTER g_netUpCounter = NULL;
static PDH_HCOUNTER g_netDownCounter = NULL;

static void InitNetQuery() {
    if (g_netQuery == NULL) {
        if (PdhOpenQueryW(NULL, 0, &g_netQuery) == ERROR_SUCCESS) {
            PdhAddEnglishCounterW(g_netQuery, L"\\Network Interface(*)\\Bytes Sent/sec", 0, &g_netUpCounter);
            PdhAddEnglishCounterW(g_netQuery, L"\\Network Interface(*)\\Bytes Received/sec", 0, &g_netDownCounter);
            PdhCollectQueryData(g_netQuery);
        }
    }
}

static void GetNetworkUsage(float& outUpMbps, float& outDownMbps) {
    outUpMbps = 0.0f;
    outDownMbps = 0.0f;
    InitNetQuery();
    if (!g_netQuery || !g_netUpCounter || !g_netDownCounter) return;

    PdhCollectQueryData(g_netQuery);

    auto getSum = [](PDH_HCOUNTER counter) -> double {
        DWORD bufferSize = 0;
        DWORD itemCount = 0;
        PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, NULL);
        if (bufferSize > 0) {
            std::vector<BYTE> buffer(bufferSize);
            PDH_FMT_COUNTERVALUE_ITEM_W* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
            if (PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items) == ERROR_SUCCESS) {
                double total = 0;
                for (DWORD i = 0; i < itemCount; i++) {
                    if (items[i].szName) {
                        if (wcsstr(items[i].szName, L"Loopback") == nullptr) {
                            total += items[i].FmtValue.doubleValue;
                        }
                    }
                }
                return total;
            }
        }
        return 0.0;
    };

    // Bytes to Mbps
    outUpMbps = static_cast<float>(getSum(g_netUpCounter) * 8.0 / 1000000.0);
    outDownMbps = static_cast<float>(getSum(g_netDownCounter) * 8.0 / 1000000.0);
}

void UpdateSystemSnapshot(bool includeGpuStats, bool includeNetStats) {
    SystemSnapshot next;
    {
        std::lock_guard lock(g_stateMutex);
        next = g_state.system;
        next.charging = g_state.system.charging;
    }

    // GPU/network sampling is comparatively expensive (the GPU counter in
    // particular enumerates every GPU engine instance across every process
    // using the GPU), so only sample them when something on screen is
    // actually displaying them. Otherwise just carry the last sampled
    // value forward (already done via `next = g_state.system` above).
    if (includeGpuStats) {
        next.gpuPercent = GetGpuUsage();
        PROCESS_MEMORY_COUNTERS_EX memory = {};
        memory.cb = sizeof(memory);
        if (K32GetProcessMemoryInfo(GetCurrentProcess(),
                                    reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory),
                                    sizeof(memory))) {
            next.hostPrivateBytes = static_cast<uint64_t>(memory.PrivateUsage);
        }
    }
    if (includeNetStats) {
        GetNetworkUsage(next.netUpMbps, next.netDownMbps);
    }

    MEMORYSTATUSEX memory = {};
    memory.dwLength = sizeof(memory);
    if (GlobalMemoryStatusEx(&memory)) {
        next.memoryPercent = static_cast<int>(memory.dwMemoryLoad);
        next.memoryTotalGB = static_cast<float>(memory.ullTotalPhys) / (1024.0f * 1024.0f * 1024.0f);
        next.memoryUsedGB = next.memoryTotalGB - static_cast<float>(memory.ullAvailPhys) / (1024.0f * 1024.0f * 1024.0f);
    }

    ULARGE_INTEGER freeBytesAvailable = {};
    ULARGE_INTEGER totalBytes = {};
    ULARGE_INTEGER totalFreeBytes = {};
    if (GetDiskFreeSpaceExW(L"C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes) &&
        totalBytes.QuadPart > 0) {
        next.diskFreePercent = ClampInt(
            static_cast<int>(totalFreeBytes.QuadPart * 100 / totalBytes.QuadPart), 0, 100);
    }

    HWND foreground = GetForegroundWindow();
    if (foreground && foreground != g_hwnd) {
        wchar_t title[96] = {};
        GetWindowTextW(foreground, title, ARRAYSIZE(title));
        if (!IsIgnorableForegroundWindow(foreground, title)) {
            next.foregroundTitle = title;
            if (next.foregroundTitle.size() > 42) {
                next.foregroundTitle.resize(42);
                next.foregroundTitle += L"...";
            }

        } else {
            next.foregroundTitle.clear();
        }
    }

    FILETIME idle = {};
    FILETIME kernel = {};
    FILETIME user = {};
    if (GetSystemTimes(&idle, &kernel, &user)) {
        const ULONGLONG idleNow = FileTimeToUInt64(idle);
        const ULONGLONG kernelNow = FileTimeToUInt64(kernel);
        const ULONGLONG userNow = FileTimeToUInt64(user);
        const ULONGLONG idlePrev = FileTimeToUInt64(g_prevIdleTime);
        const ULONGLONG kernelPrev = FileTimeToUInt64(g_prevKernelTime);
        const ULONGLONG userPrev = FileTimeToUInt64(g_prevUserTime);

        const ULONGLONG total = (kernelNow - kernelPrev) + (userNow - userPrev);
        const ULONGLONG idleDelta = idleNow - idlePrev;
        if (total > 0 && kernelPrev != 0) {
            next.cpuPercent = ClampInt(static_cast<int>((total - idleDelta) * 100 / total), 0, 100);
        }

        g_prevIdleTime = idle;
        g_prevKernelTime = kernel;
        g_prevUserTime = user;
    }

    static ComPtr<IAudioEndpointVolume> s_volume;
    if (!s_volume) {
        ComPtr<IMMDeviceEnumerator> enumerator;
        ComPtr<IMMDevice> device;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (SUCCEEDED(hr)) {
            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        }
        if (SUCCEEDED(hr)) {
            hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(s_volume.GetAddressOf()));
        }
    }

    bool haveVolumeSnapshot = false;
    if (s_volume) {
        float level = 0.0f;
        BOOL muted = FALSE;
        if (SUCCEEDED(s_volume->GetMasterVolumeLevelScalar(&level)) && SUCCEEDED(s_volume->GetMute(&muted))) {
            next.volumePercent = ClampInt(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
            next.volumeMuted = muted != FALSE;
            haveVolumeSnapshot = true;
        } else {
            s_volume.Reset(); // Retry next time
        }
    }

    std::lock_guard lock(g_stateMutex);
    // Keep the last successful reading if the default endpoint is temporarily
    // unavailable; a failed poll must not paint a false 0% / muted state.
    if (!haveVolumeSnapshot) {
        next.volumePercent = g_state.system.volumePercent;
        next.volumeMuted = g_state.system.volumeMuted;
    }
    const bool volumeChanged =
        g_volumeInitialized &&
        (std::abs(next.volumePercent - g_state.system.volumePercent) >= 2 ||
         next.volumeMuted != g_state.system.volumeMuted);
    g_state.system = next;
    g_state.muted = next.volumeMuted;
    if (volumeChanged && g_settings.volume &&
        NowSeconds() >= g_mediaVolumeAdjustUntil.load(std::memory_order_relaxed)) {
        g_state.volume.active = true;
        g_state.volume.percent = next.volumePercent;
        g_state.volume.muted = next.volumeMuted;
        g_state.volume.deviceName = L"System audio";
        g_state.volume.expiresAt = NowSeconds() + 1.8;
        TriggerNudge();
    }
    g_volumeInitialized = true;
}

// ---- Privacy indicator helpers ----
std::wstring CleanActiveAppName(const std::wstring& raw) {
    std::wstring name;
    size_t hashPos = raw.rfind(L'#');
    if (hashPos != std::wstring::npos) {
        name = raw.substr(hashPos + 1);
        if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0) {
            name.resize(name.size() - 4);
        }
    } else {
        size_t underPos = raw.find(L'_');
        name = (underPos != std::wstring::npos) ? raw.substr(0, underPos) : raw;
        if (name.rfind(L"Microsoft.", 0) == 0) name = name.substr(10);
        if (name.rfind(L"Windows.", 0) == 0) name = name.substr(8);
        else if (name.rfind(L"Windows", 0) == 0 && name.size() > 7) name = name.substr(7);
    }
    std::wstring lower = ToLowerCopy(name);
    if (lower == L"msedge") return L"Microsoft Edge";
    if (lower == L"chrome") return L"Google Chrome";
    if (lower == L"brave") return L"Brave";
    if (lower == L"firefox") return L"Firefox";
    if (lower == L"discord") return L"Discord";
    if (lower == L"zoom") return L"Zoom";
    if (lower == L"obs64" || lower == L"obs32" || lower == L"obs") return L"OBS Studio";
    if (lower == L"teams") return L"Microsoft Teams";
    if (lower == L"skype") return L"Skype";
    if (lower == L"audacity") return L"Audacity";
    if (lower == L"fl64" || lower == L"fl") return L"FL Studio";
    if (lower == L"ciscocollabhost") return L"Webex";
    return name;
}

// Reads a REG_QWORD, returning false when the value is absent or is not a
// well-formed 64-bit quantity. The previous code read straight into a uint64_t
// without checking the type or size, so a shorter value could leave the high
// bytes untouched and fake a zero timestamp.
bool ReadRegQword(HKEY key, const wchar_t* name, uint64_t* out) {
    DWORD type = 0;
    uint64_t value = 0;
    DWORD dataSize = sizeof(value);
    if (RegQueryValueExW(key, name, nullptr, &type,
                         reinterpret_cast<LPBYTE>(&value), &dataSize) != ERROR_SUCCESS) {
        return false;
    }
    if (type != REG_QWORD || dataSize != sizeof(value)) {
        return false;
    }
    *out = value;
    return true;
}

// A ConsentStore entry counts as in-use only when Windows has recorded a start
// and has not yet recorded a stop. Testing LastUsedTimeStop == 0 on its own
// also matched entries that had never been used at all (both timestamps zero),
// which is what left the microphone dot lit permanently for some users even
// though Windows' own privacy indicator showed nothing in use.
bool ConsentStoreEntryInUse(HKEY key) {
    uint64_t stopTime = 0;
    if (!ReadRegQword(key, L"LastUsedTimeStop", &stopTime) || stopTime != 0) {
        return false;
    }

    uint64_t startTime = 0;
    if (ReadRegQword(key, L"LastUsedTimeStart", &startTime)) {
        return startTime != 0;
    }

    // Stop says "not stopped" but there is no readable start timestamp. This is
    // the shape of a never-used entry, so it is reported as not in use, but log
    // it once: if some Windows build stores these differently it would otherwise
    // be an indicator that silently stops working with nothing to explain it.
    static std::atomic<bool> loggedMissingStart{false};
    if (!loggedMissingStart.exchange(true)) {
        Wh_Log(L"Privacy indicator: a ConsentStore entry has LastUsedTimeStop=0 but no "
               L"readable REG_QWORD LastUsedTimeStart; treating it as not in use.");
    }
    return false;
}

bool IsDeviceActiveViaRegistry(const wchar_t* capability, std::wstring* outAppName = nullptr) {
    bool isActive = false;
    std::wstring basePath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\";
    basePath += capability;

    auto CheckSubkeys = [&](HKEY hKeyParent) -> bool {
        DWORD index = 0;
        wchar_t subKeyName[256];
        DWORD nameLen = ARRAYSIZE(subKeyName);
        while (RegEnumKeyExW(hKeyParent, index, subKeyName, &nameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
            HKEY hSub;
            if (RegOpenKeyExW(hKeyParent, subKeyName, 0, KEY_READ, &hSub) == ERROR_SUCCESS) {
                if (_wcsicmp(subKeyName, L"NonPackaged") == 0) {
                    DWORD npIndex = 0;
                    wchar_t npSubKeyName[256];
                    DWORD npNameLen = ARRAYSIZE(npSubKeyName);
                    while (RegEnumKeyExW(hSub, npIndex, npSubKeyName, &npNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                        HKEY hNpSub;
                        if (RegOpenKeyExW(hSub, npSubKeyName, 0, KEY_READ, &hNpSub) == ERROR_SUCCESS) {
                            if (ConsentStoreEntryInUse(hNpSub)) {
                                if (outAppName && outAppName->empty()) {
                                    *outAppName = CleanActiveAppName(npSubKeyName);
                                }
                                RegCloseKey(hNpSub);
                                RegCloseKey(hSub);
                                return true;
                            }
                            RegCloseKey(hNpSub);
                        }
                        npIndex++;
                        npNameLen = ARRAYSIZE(npSubKeyName);
                    }
                } else {
                    if (ConsentStoreEntryInUse(hSub)) {
                        if (outAppName && outAppName->empty()) {
                            *outAppName = CleanActiveAppName(subKeyName);
                        }
                        RegCloseKey(hSub);
                        return true;
                    }
                }
                RegCloseKey(hSub);
            }
            index++;
            nameLen = ARRAYSIZE(subKeyName);
        }
        return false;
    };

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, basePath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        isActive = CheckSubkeys(hKey);
        RegCloseKey(hKey);
    }

    if (!isActive) {
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, basePath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            isActive = CheckSubkeys(hKey);
            RegCloseKey(hKey);
        }
    }

    return isActive;
}

bool IsMicrophoneActive(std::wstring* outAppName = nullptr) {
    return IsDeviceActiveViaRegistry(L"microphone", outAppName);
}

bool IsCameraActive(std::wstring* outAppName = nullptr) {
    return IsDeviceActiveViaRegistry(L"webcam", outAppName);
}

void UpdateProgressSnapshot() {
    const int progress = Wh_GetIntValue(L"ProgressPercent", -1);
    std::lock_guard lock(g_stateMutex);
    g_state.progress.active = progress >= 0 && progress <= 100;
    g_state.progress.percent = ClampInt(progress, 0, 100);
}

void UpdatePrivacyIndicators() {
    std::wstring micApp;
    std::wstring camApp;
    const bool mic = (g_settings.privacyDots && g_settings.privacyDotsMic) ? IsMicrophoneActive(&micApp) : false;
    const bool cam = (g_settings.privacyDots && g_settings.privacyDotsCam) ? IsCameraActive(&camApp) : false;
    std::lock_guard lock(g_stateMutex);
    g_state.system.micActive = mic;
    g_state.system.cameraActive = cam;
    g_state.system.micApp = micApp;
    g_state.system.cameraApp = camApp;
}

std::wstring ReadClipboardText(HWND hwnd) {
    std::wstring text;
    if (!OpenClipboard(hwnd)) {
        return text;
    }

    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (data) {
        auto* locked = static_cast<const wchar_t*>(GlobalLock(data));
        if (locked) {
            text = locked;
            GlobalUnlock(data);
        }
    }

    CloseClipboard();
    return text;
}

// Decodes whatever bitmap is currently on the clipboard into a small BGRA
// thumbnail, aspect-preserving and capped at maxDim on the longer side.
// Requesting CF_BITMAP works regardless of which bitmap format the source
// app actually placed on the clipboard (CF_DIB/CF_DIBV5) — Windows
// synthesizes CF_BITMAP from those automatically.
bool ReadClipboardImagePixels(HWND hwnd, BitmapPixels* outPixels, UINT maxDim) {
    if (!outPixels || !OpenClipboard(hwnd)) {
        return false;
    }

    HBITMAP sourceBitmap = static_cast<HBITMAP>(GetClipboardData(CF_BITMAP));
    if (!sourceBitmap) {
        CloseClipboard();
        return false;
    }

    BITMAP bm = {};
    if (!GetObject(sourceBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        CloseClipboard();
        return false;
    }

    UINT srcW = static_cast<UINT>(bm.bmWidth);
    UINT srcH = static_cast<UINT>(bm.bmHeight);
    UINT dstW = srcW;
    UINT dstH = srcH;
    if (srcW > maxDim || srcH > maxDim) {
        const float scale = static_cast<float>(maxDim) / static_cast<float>(std::max(srcW, srcH));
        dstW = std::max<UINT>(1, static_cast<UINT>(srcW * scale));
        dstH = std::max<UINT>(1, static_cast<UINT>(srcH * scale));
    }

    HDC screen = GetDC(nullptr);
    HDC srcDc = CreateCompatibleDC(screen);
    HDC dstDc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!srcDc || !dstDc) {
        if (srcDc) DeleteDC(srcDc);
        if (dstDc) DeleteDC(dstDc);
        CloseClipboard();
        return false;
    }

    // Selecting the clipboard's own HBITMAP into a scratch DC to read from it
    // is the MSDN-documented way to do this; we must not DeleteObject it.
    HGDIOBJ oldSrc = SelectObject(srcDc, sourceBitmap);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(dstW);
    bi.bmiHeader.biHeight = -static_cast<LONG>(dstH);  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dstDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib) {
        SelectObject(srcDc, oldSrc);
        DeleteDC(srcDc);
        DeleteDC(dstDc);
        CloseClipboard();
        return false;
    }

    HGDIOBJ oldDst = SelectObject(dstDc, dib);
    SetStretchBltMode(dstDc, HALFTONE);
    SetBrushOrgEx(dstDc, 0, 0, nullptr);
    StretchBlt(dstDc, 0, 0, static_cast<int>(dstW), static_cast<int>(dstH),
               srcDc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

    BitmapPixels pixels;
    pixels.width = dstW;
    pixels.height = dstH;
    pixels.bgra.resize(static_cast<size_t>(dstW) * dstH * 4);
    memcpy(pixels.bgra.data(), bits, pixels.bgra.size());

    // GDI blits never populate an alpha channel; without this the D2D bitmap
    // (which expects premultiplied alpha) would render fully invisible.
    for (size_t i = 3; i < pixels.bgra.size(); i += 4) {
        pixels.bgra[i] = 255;
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);

    SelectObject(dstDc, oldDst);
    DeleteObject(dib);
    SelectObject(srcDc, oldSrc);
    DeleteDC(srcDc);
    DeleteDC(dstDc);
    CloseClipboard();
    return true;
}

bool IsLikelyToastWindow(HWND hwnd, const wchar_t* className, const wchar_t* title) {
    if (hwnd == g_hwnd || !hwnd) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER)) {
        return false;
    }

    const std::wstring cls = ToLowerCopy(className ? className : L"");
    const std::wstring text = ToLowerCopy(title ? title : L"");

    // Classic Windows 10 toasts have clear class names
    if (cls.find(L"notification") != std::wstring::npos ||
        cls.find(L"toast") != std::wstring::npos ||
        cls.find(L"windows.ui.notifications") != std::wstring::npos) {
        return true;
    }

    // Windows 11 toasts use generic XAML or CoreWindow classes, usually hosted by
    // explorer.exe, sihost.exe, or ShellExperienceHost.exe.
    // Importantly, their title is often empty at the exact moment of creation!
    if (cls.find(L"xaml_windowedpopupclass") != std::wstring::npos ||
        cls.find(L"windows.ui.core.corewindow") != std::wstring::npos) {

        std::wstring image;
        if (ProcessImageNameForWindow(hwnd, &image)) {
            const std::wstring base = ToLowerCopy(BaseNameFromPath(image));
            if (base == L"explorer.exe" || base == L"sihost.exe" || base == L"shellexperiencehost.exe") {
                // Ensure it's not the start menu, search, or action center main panel
                if (text != L"start" && text != L"action center" && text != L"search" && text != L"task view") {
                    return true;
                }
            }
        }
    }

    return false;
}

void CaptureShellNotification(HWND hwnd) {
    if (!GetSettingsCopy().windowsNotifications) return;
    // Grace period: ignore notifications that fire in the first 3 seconds after
    // the mod starts. sihost.exe gets injected while system windows are still
    // settling, which causes false positives (e.g. Snipping Tool windows).
    if (NowSeconds() < 3.0) {
        return;
    }

    wchar_t className[128] = {};
    wchar_t title[192] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    if (!IsLikelyToastWindow(hwnd, className, title)) {
        return;
    }

    NotificationSnapshot notification;
    notification.active = true;
    notification.app = L"Notification";
    notification.title = title;
    notification.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
    // Fetch a 64px icon to ensure crisp rendering inside the pill
    notification.icon = GetWindowIconPixels(hwnd, 64);

    if (notification.title.size() > 96) {
        notification.body = notification.title.substr(64);
        notification.title.resize(64);
        notification.title += L"...";
    }

    {
        std::lock_guard lock(g_stateMutex);
        g_state.notification = std::move(notification);
    }
    TriggerNudge();

    // Spawn a background thread to extract the full rich text body of the toast using UI Automation.
    // Modern Windows Toasts often only provide the App Name via GetWindowTextW, leaving the body hidden in the XAML tree.
    std::thread([hwnd]() {
        Sleep(400); // Give the heavy UWP XAML tree enough time to fully construct the text nodes
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (SUCCEEDED(hr)) {
            IUIAutomation* uia = nullptr;
            hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&uia);
            if (SUCCEEDED(hr) && uia) {
                IUIAutomation2* uia2 = nullptr;
                if (SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2), (void**)&uia2)) && uia2) {
                    uia2->put_TransactionTimeout(500);
                    uia2->put_ConnectionTimeout(500);
                    uia2->Release();
                }
                IUIAutomationElement* windowEl = nullptr;
                if (SUCCEEDED(uia->ElementFromHandle(hwnd, &windowEl)) && windowEl) {
                    IUIAutomationCondition* cond = nullptr;
                    uia->CreateTrueCondition(&cond);
                    IUIAutomationElementArray* elements = nullptr;
                    if (SUCCEEDED(windowEl->FindAll(TreeScope_Descendants, cond, &elements)) && elements) {
                        int count = 0;
                        elements->get_Length(&count);
                        std::wstring appName;
                        std::wstring fullText;
                        for (int i = 0; i < count; ++i) {
                            IUIAutomationElement* el = nullptr;
                            if (SUCCEEDED(elements->GetElement(i, &el)) && el) {
                                BSTR name = nullptr;
                                el->get_CurrentName(&name);
                                if (name && wcslen(name) > 0) {
                                    std::wstring chunk = name;
                                    // Skip generic screen-reader labels often found in toasts
                                    if (chunk != L"Notification" && chunk != L"New notification") {
                                        if (appName.empty()) {
                                            appName = chunk;
                                        } else {
                                            if (!fullText.empty()) fullText += L"  -  ";
                                            fullText += chunk;
                                        }
                                    }
                                }
                                if (name) SysFreeString(name);
                                el->Release();
                            }
                        }
                        elements->Release();

                        if (fullText.empty() && !appName.empty()) {
                            fullText = appName;
                            appName = L"Notification";
                        }

                        if (!fullText.empty()) {
                            std::lock_guard lock(g_stateMutex);
                            if (g_state.notification.active) {
                                if (!appName.empty()) {
                                    g_state.notification.app = appName;
                                    if (appName != L"Notification") {
                                        BitmapPixels resolvedIcon = FindAppIconByName(appName, 64);
                                        if (!resolvedIcon.bgra.empty()) {
                                            g_state.notification.icon = std::move(resolvedIcon);
                                        }
                                    }
                                }
                                g_state.notification.title = fullText;
                            }
                        }
                    }
                    if (cond) cond->Release();
                    windowEl->Release();
                }
                uia->Release();
            }
            CoUninitialize();
        }
    }).detach();
}

// Persistent histories live in the user's local Windhawk data directory. The
// clipboard history is deliberately stored only because the user enabled this
// behavior in the mod request; no image or text is copied into the source tree.
std::wstring DynamicIslandDataDirectory() {
    wchar_t localAppData[MAX_PATH] = {};
    const DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData,
                                                  ARRAYSIZE(localAppData));
    if (length == 0 || length >= ARRAYSIZE(localAppData)) return {};
    std::wstring directory(localAppData);
    directory += L"\\Windhawk\\DynamicIslandPro";
    const std::wstring parent = directory.substr(0, directory.find_last_of(L'\\'));
    CreateDirectoryW(parent.c_str(), nullptr);
    CreateDirectoryW(directory.c_str(), nullptr);
    return directory;
}

bool WritePersistenceBytes(HANDLE file, const void* data, DWORD bytes) {
    DWORD written = 0;
    return bytes == 0 || (WriteFile(file, data, bytes, &written, nullptr) && written == bytes);
}

bool ReadPersistenceBytes(HANDLE file, void* data, DWORD bytes) {
    DWORD received = 0;
    return bytes == 0 || (ReadFile(file, data, bytes, &received, nullptr) && received == bytes);
}

template <typename T>
bool WritePersistenceValue(HANDLE file, const T& value) {
    return WritePersistenceBytes(file, &value, static_cast<DWORD>(sizeof(T)));
}

template <typename T>
bool ReadPersistenceValue(HANDLE file, T* value) {
    return ReadPersistenceBytes(file, value, static_cast<DWORD>(sizeof(T)));
}

bool WritePersistenceString(HANDLE file, const std::wstring& value) {
    if (value.size() > 32768) return false;
    const uint32_t chars = static_cast<uint32_t>(value.size());
    return WritePersistenceValue(file, chars) &&
           WritePersistenceBytes(file, value.data(), chars * sizeof(wchar_t));
}

bool ReadPersistenceString(HANDLE file, std::wstring* value, uint32_t maxChars = 32768) {
    uint32_t chars = 0;
    if (!ReadPersistenceValue(file, &chars) || chars > maxChars) return false;
    value->resize(chars);
    return ReadPersistenceBytes(file, value->data(), chars * sizeof(wchar_t));
}

HANDLE CreatePersistenceTempFile(const wchar_t* filename, std::wstring* finalPath,
                                 std::wstring* tempPath) {
    const std::wstring directory = DynamicIslandDataDirectory();
    if (directory.empty()) return INVALID_HANDLE_VALUE;
    *finalPath = directory + L"\\" + filename;
    *tempPath = *finalPath + L".tmp";
    return CreateFileW(tempPath->c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
}

bool FinishPersistenceFile(HANDLE file, const std::wstring& finalPath,
                           const std::wstring& tempPath, bool success) {
    if (file != INVALID_HANDLE_VALUE) {
        if (success) success = FlushFileBuffers(file) != FALSE;
        CloseHandle(file);
    }
    if (!success) {
        DeleteFileW(tempPath.c_str());
        return false;
    }
    if (!MoveFileExW(tempPath.c_str(), finalPath.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tempPath.c_str());
        return false;
    }
    return true;
}

struct HistoryFileHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t count;
};

void SaveClipboardHistoryToDisk() {
    static std::mutex persistenceMutex;
    std::lock_guard persistenceLock(persistenceMutex);
    std::shared_ptr<const std::vector<ClipboardHistoryItem>> history;
    {
        std::lock_guard lock(g_stateMutex);
        history = g_state.clipboardHistory;
    }
    const size_t count = history ? std::min<size_t>(history->size(), 1000) : 0;
    std::wstring finalPath, tempPath;
    HANDLE file = CreatePersistenceTempFile(L"clipboard-history.bin", &finalPath, &tempPath);
    if (file == INVALID_HANDLE_VALUE) return;
    const HistoryFileHeader header{0x32435044u, 1u, static_cast<uint32_t>(count)};
    bool ok = WritePersistenceValue(file, header);
    for (size_t i = 0; ok && i < count; ++i) {
        const ClipboardHistoryItem& item = (*history)[i];
        const uint8_t pinned = item.pinned ? 1 : 0;
        const uint8_t image = item.image ? 1 : 0;
        const uint32_t width = item.imagePreview.width;
        const uint32_t height = item.imagePreview.height;
        const uint64_t pixelBytes = item.imagePreview.bgra.size();
        ok = WritePersistenceValue(file, item.id) && WritePersistenceValue(file, pinned) &&
             WritePersistenceValue(file, image) && WritePersistenceValue(file, item.copiedAt) &&
             WritePersistenceString(file, item.text) && WritePersistenceString(file, item.appName) &&
             WritePersistenceValue(file, width) && WritePersistenceValue(file, height) &&
             WritePersistenceValue(file, pixelBytes) && pixelBytes <= 512ull * 1024ull &&
             WritePersistenceBytes(file, item.imagePreview.bgra.data(), static_cast<DWORD>(pixelBytes));
    }
    FinishPersistenceFile(file, finalPath, tempPath, ok);
}

void LoadClipboardHistoryFromDisk() {
    const std::wstring directory = DynamicIslandDataDirectory();
    if (directory.empty()) return;
    const std::wstring path = directory + L"\\clipboard-history.bin";
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    HistoryFileHeader header{};
    bool ok = ReadPersistenceValue(file, &header) && header.magic == 0x32435044u &&
              header.version == 1u && header.count <= 1000;
    std::vector<ClipboardHistoryItem> items;
    if (ok) items.reserve(header.count);
    uint64_t highestId = 0;
    for (uint32_t i = 0; ok && i < header.count; ++i) {
        ClipboardHistoryItem item;
        uint8_t pinned = 0, image = 0;
        uint32_t width = 0, height = 0;
        uint64_t pixelBytes = 0;
        ok = ReadPersistenceValue(file, &item.id) && ReadPersistenceValue(file, &pinned) &&
             ReadPersistenceValue(file, &image) && ReadPersistenceValue(file, &item.copiedAt) &&
             ReadPersistenceString(file, &item.text, 65536) &&
             ReadPersistenceString(file, &item.appName, 4096) &&
             ReadPersistenceValue(file, &width) && ReadPersistenceValue(file, &height) &&
             ReadPersistenceValue(file, &pixelBytes);
        const uint64_t expected = static_cast<uint64_t>(width) * height * 4ull;
        if (!ok || pixelBytes > 512ull * 1024ull || pixelBytes != expected ||
            width > kClipboardImageThumbMaxDim || height > kClipboardImageThumbMaxDim) {
            ok = false;
            break;
        }
        item.pinned = pinned != 0;
        item.image = image != 0;
        if (pixelBytes > 0) {
            item.imagePreview.width = width;
            item.imagePreview.height = height;
            item.imagePreview.bgra.resize(static_cast<size_t>(pixelBytes));
            ok = ReadPersistenceBytes(file, item.imagePreview.bgra.data(),
                                      static_cast<DWORD>(pixelBytes));
            item.imagePreview.generation = item.id;
        }
        highestId = std::max(highestId, item.id);
        items.push_back(std::move(item));
    }
    CloseHandle(file);
    if (!ok) return;
    const size_t limit = static_cast<size_t>(GetSettingsCopy().clipboardHistoryLimit);
    if (items.size() > limit) items.resize(limit);
    {
        std::lock_guard lock(g_stateMutex);
        g_state.clipboardHistory =
            std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
        const uint64_t currentId = g_clipboardHistoryId.load(std::memory_order_relaxed);
        if (highestId > currentId) {
            g_clipboardHistoryId.store(highestId, std::memory_order_relaxed);
        }
    }
    g_layoutDirty = true;
}

void SaveFileTrayToDisk() {
    static std::mutex persistenceMutex;
    std::lock_guard persistenceLock(persistenceMutex);
    std::vector<FileTrayItem> items;
    {
        std::lock_guard lock(g_stateMutex);
        items = g_state.fileTrayItems;
    }
    const size_t count = std::min<size_t>(items.size(), 25);
    std::wstring finalPath, tempPath;
    HANDLE file = CreatePersistenceTempFile(L"file-tray.bin", &finalPath, &tempPath);
    if (file == INVALID_HANDLE_VALUE) return;
    const HistoryFileHeader header{0x32545044u, 1u, static_cast<uint32_t>(count)};
    bool ok = WritePersistenceValue(file, header);
    for (size_t i = 0; ok && i < count; ++i) {
        const uint8_t directory = items[i].isDirectory ? 1 : 0;
        ok = WritePersistenceString(file, items[i].path) &&
             WritePersistenceString(file, items[i].name) &&
             WritePersistenceValue(file, items[i].sizeBytes) &&
             WritePersistenceValue(file, directory);
    }
    FinishPersistenceFile(file, finalPath, tempPath, ok);
}

void LoadFileTrayFromDisk() {
    const std::wstring directory = DynamicIslandDataDirectory();
    if (directory.empty()) return;
    const std::wstring path = directory + L"\\file-tray.bin";
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    HistoryFileHeader header{};
    bool ok = ReadPersistenceValue(file, &header) && header.magic == 0x32545044u &&
              header.version == 1u && header.count <= 25;
    std::vector<FileTrayItem> items;
    if (ok) items.reserve(header.count);
    for (uint32_t i = 0; ok && i < header.count; ++i) {
        FileTrayItem item;
        uint8_t directoryFlag = 0;
        ok = ReadPersistenceString(file, &item.path, 32768) &&
             ReadPersistenceString(file, &item.name, 32768) &&
             ReadPersistenceValue(file, &item.sizeBytes) &&
             ReadPersistenceValue(file, &directoryFlag);
        if (!ok) break;
        item.isDirectory = directoryFlag != 0;
        WIN32_FILE_ATTRIBUTE_DATA attr{};
        if (GetFileAttributesExW(item.path.c_str(), GetFileExInfoStandard, &attr)) {
            item.isDirectory = (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            item.sizeBytes = (static_cast<uint64_t>(attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
        }
        SHFILEINFOW info{};
        if (SHGetFileInfoW(item.path.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_SMALLICON) && info.hIcon) {
            IconToPixels(info.hIcon, 32, &item.icon);
            DestroyIcon(info.hIcon);
        }
        items.push_back(std::move(item));
    }
    CloseHandle(file);
    if (!ok) return;
    const size_t limit = static_cast<size_t>(GetSettingsCopy().fileTrayMaxItems);
    if (items.size() > limit) items.erase(items.begin(), items.end() - limit);
    {
        std::lock_guard lock(g_stateMutex);
        g_state.fileTrayItems = std::move(items);
    }
    g_layoutDirty = true;
}

void CaptureClipboard(HWND hwnd, bool announce = true) {
    const Settings settings = GetSettingsCopy();
    ClipboardSnapshot clip;
    clip.expiresAt = NowSeconds() + 2.5;
    ClipboardHistoryItem historyItem;
    bool hasHistoryItem = false;
    HWND owner = GetClipboardOwner();
    if (!owner) {
        owner = GetForegroundWindow();
    }
    wchar_t ownerTitle[80] = {};
    if (owner) {
        GetWindowTextW(owner, ownerTitle, ARRAYSIZE(ownerTitle));
    }
    if (owner && !IsIgnorableForegroundWindow(owner, ownerTitle)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        // Fetch at 64px for crisp rendering — 18px/32px is often too small for icon APIs and returns empty.
        clip.appIcon = GetWindowIconPixels(owner, 64);

        clip.appName = ownerTitle;
        if (clip.appName.empty()) {
            std::wstring path;
            if (ProcessImageNameForPid(pid, &path)) {
                clip.appName = StripExtension(BaseNameFromPath(path));
            }
        }
        if (clip.appName.size() > 24) {
            clip.appName.resize(24);
            clip.appName += L"...";
        }
    }

    std::wstring text = ReadClipboardText(hwnd);
    if (!text.empty()) {
        constexpr size_t maxDashboardChars = 32768;
        if (text.size() > maxDashboardChars) {
            text.resize(maxDashboardChars);
            text += L"\n\n[Clipboard preview truncated]";
        }
        clip.dashboardText = text;
        constexpr size_t maxChars = 96;
        std::wstring toastText = text;
        std::replace(toastText.begin(), toastText.end(), L'\r', L' ');
        std::replace(toastText.begin(), toastText.end(), L'\n', L' ');
        if (toastText.size() > maxChars) {
            toastText.resize(maxChars);
            toastText += L"...";
        }
        clip.text = std::move(toastText);
        clip.image = false;
        clip.active = announce && settings.clipboard;
        historyItem.text = clip.dashboardText;
        historyItem.appName = clip.appName;
        historyItem.copiedAt = NowSeconds();
        hasHistoryItem = true;
    } else {
        BitmapPixels imagePixels;
        if (ReadClipboardImagePixels(hwnd, &imagePixels, kClipboardImageThumbMaxDim)) {
            clip.text = L"Image copied";
            clip.dashboardText = L"Image copied";
            clip.image = true;
            clip.active = announce && settings.clipboard;
            clip.imagePreview = std::move(imagePixels);
            historyItem.image = true;
            historyItem.text = L"Image copied";
            historyItem.appName = clip.appName;
            historyItem.imagePreview = clip.imagePreview;
            historyItem.copiedAt = NowSeconds();
            hasHistoryItem = true;
        }
    }

    if (settings.clipboard || settings.clipboardSection) {
        {
            std::lock_guard lock(g_stateMutex);
            g_state.clipboard = std::move(clip);
            if (hasHistoryItem) {
                historyItem.id = ++g_clipboardHistoryId;
                std::vector<ClipboardHistoryItem> items;
                if (g_state.clipboardHistory) {
                    items = *g_state.clipboardHistory;
                }
                items.insert(items.begin(), std::move(historyItem));
                std::stable_partition(items.begin(), items.end(),
                    [](const ClipboardHistoryItem& item) { return item.pinned; });
                while (items.size() > static_cast<size_t>(settings.clipboardHistoryLimit)) {
                    size_t eraseIndex = items.size();
                    for (size_t i = items.size(); i > 0; --i) {
                        if (!items[i - 1].pinned) {
                            eraseIndex = i - 1;
                            break;
                        }
                    }
                    if (eraseIndex == items.size()) break;
                    items.erase(items.begin() + eraseIndex);
                }
                g_state.clipboardHistory =
                    std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
            }
        }
        if (hasHistoryItem) SaveClipboardHistoryToDisk();
    }
}

void CaptureClipboardFilesToTray(HWND hwnd) {
    const Settings settings = GetSettingsCopy();
    if ((!settings.fileTrayModule && !settings.fileCopyNotifications) || !OpenClipboard(hwnd)) return;

    std::vector<std::wstring> paths;
    HDROP drop = static_cast<HDROP>(GetClipboardData(CF_HDROP));
    if (drop) {
        const UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
        paths.reserve(count);
        for (UINT i = 0; i < count; ++i) {
            const UINT length = DragQueryFileW(drop, i, nullptr, 0);
            if (length == 0) continue;
            std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
            if (DragQueryFileW(drop, i, buffer.data(), static_cast<UINT>(buffer.size())) > 0) {
                paths.emplace_back(buffer.data());
            }
        }
    }
    CloseClipboard();
    if (paths.empty()) return;

    std::vector<FileTrayItem> added;
    if (settings.fileTrayModule) {
        added.reserve(paths.size());
        for (const std::wstring& path : paths) {
            FileTrayItem item;
            item.path = path;
            item.name = BaseNameFromPath(path);
            if (item.name.empty()) item.name = path;
            WIN32_FILE_ATTRIBUTE_DATA attr = {};
            if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attr)) {
                item.isDirectory = (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                item.sizeBytes = (static_cast<uint64_t>(attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
            }
            SHFILEINFOW info = {};
            if (SHGetFileInfoW(path.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_SMALLICON) && info.hIcon) {
                IconToPixels(info.hIcon, 32, &item.icon);
                DestroyIcon(info.hIcon);
            }
            added.push_back(std::move(item));
        }
    }

    {
        std::lock_guard lock(g_stateMutex);
        if (settings.fileTrayModule) {
            for (auto& item : added) {
                auto existing = std::find_if(g_state.fileTrayItems.begin(), g_state.fileTrayItems.end(),
                                             [&](const FileTrayItem& other) {
                                                 return _wcsicmp(other.path.c_str(), item.path.c_str()) == 0;
                                             });
                if (existing != g_state.fileTrayItems.end()) g_state.fileTrayItems.erase(existing);
                g_state.fileTrayItems.push_back(std::move(item));
            }
            const size_t cap = static_cast<size_t>(std::max(1, settings.fileTrayMaxItems));
            while (g_state.fileTrayItems.size() > cap) g_state.fileTrayItems.erase(g_state.fileTrayItems.begin());
            g_fileTrayScrollOffset.store(0, std::memory_order_relaxed);
        }
        if (settings.fileCopyNotifications) {
            NotificationSnapshot notification;
            notification.active = true;
            notification.app = L"File Tray";
            notification.title = paths.size() == 1 ? L"File copied" : std::to_wstring(paths.size()) + L" files copied";
            notification.body = paths.size() == 1 ? BaseNameFromPath(paths.front()) : L"Added to File Tray";
            notification.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
            g_state.notification = std::move(notification);
        }
    }
    if (settings.fileTrayModule && !added.empty()) SaveFileTrayToDisk();
    g_layoutDirty = true;
}

void SetClickThrough(HWND hwnd, bool clickThrough) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    const bool has = (exStyle & WS_EX_TRANSPARENT) != 0;
    if (clickThrough == has) {
        return;
    }

    if (clickThrough) {
        exStyle |= WS_EX_TRANSPARENT;
    } else {
        exStyle &= ~WS_EX_TRANSPARENT;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void OpenRelevantApp();


void ToggleEndpointMute();
void SeekMediaToTicks(int64_t targetTicks);

void HandleStatusClickAtPoint(HWND hwnd, LPARAM lParam) {
    return;
}

void StartFocusTimer(int minutes, bool isBreak) {
    {
        std::lock_guard lock(g_stateMutex);
        g_state.timer.active = true;
        g_state.timer.running = true;
        g_state.timer.isBreak = isBreak;
        g_state.timer.totalSeconds = minutes * 60;
        g_state.timer.endsAt = NowSeconds() + minutes * 60;
        g_state.timer.remainingAtPause = 0.0;
        g_state.timer.justFinished = false;
    }
    TriggerNudge();
}

void ToggleTimerPause() {
    {
        std::lock_guard lock(g_stateMutex);
        if (!g_state.timer.active) {
            return;
        }
        const double now = NowSeconds();
        if (g_state.timer.running) {
            g_state.timer.remainingAtPause = std::max(0.0, g_state.timer.endsAt - now);
            g_state.timer.running = false;
        } else {
            g_state.timer.endsAt = now + g_state.timer.remainingAtPause;
            g_state.timer.running = true;
        }
    }
    TriggerNudge();
}

void StopFocusTimer() {
    {
        std::lock_guard lock(g_stateMutex);
        g_state.timer.active = false;
        g_state.timer.running = false;
        g_state.timer.justFinished = false;
    }
    TriggerNudge();
}

void ToggleEndpointMute() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDevice> device;
    ComPtr<IAudioEndpointVolume> volume;

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (SUCCEEDED(hr)) {
        hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    }
    if (SUCCEEDED(hr)) {
        hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                              reinterpret_cast<void**>(volume.GetAddressOf()));
    }
    if (SUCCEEDED(hr)) {
        BOOL muted = FALSE;
        volume->GetMute(&muted);
        volume->SetMute(!muted, nullptr);
        float scalar = 0.0f;
        volume->GetMasterVolumeLevelScalar(&scalar);
        std::lock_guard lock(g_stateMutex);
        g_state.muted = !muted;
        g_state.system.volumeMuted = !muted;
        g_state.system.volumePercent = ClampInt(static_cast<int>(scalar * 100.0f + 0.5f), 0, 100);
        g_state.volume.active = false;
        g_layoutDirty = true;
    }
}

void AdjustEndpointVolume(int wheelDelta) {
    g_mediaVolumeAdjustUntil.store(NowSeconds() + 1.25, std::memory_order_relaxed);
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDevice> device;
    ComPtr<IAudioEndpointVolume> volume;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (SUCCEEDED(hr)) hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (SUCCEEDED(hr)) {
        hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                              reinterpret_cast<void**>(volume.GetAddressOf()));
    }
    if (FAILED(hr) || !volume) return;

    float current = 0.0f;
    if (FAILED(volume->GetMasterVolumeLevelScalar(&current))) return;
    const int currentPercent = ClampInt(static_cast<int>(current * 100.0f + 0.5f), 0, 100);
    const int direction = wheelDelta > 0 ? 1 : (wheelDelta < 0 ? -1 : 0);
    const int nextPercent = ClampInt(currentPercent + direction * 2, 0, 100);
    const float next = nextPercent / 100.0f;
    if (SUCCEEDED(volume->SetMasterVolumeLevelScalar(next, nullptr))) {
        BOOL muted = FALSE;
        volume->GetMute(&muted);
        std::lock_guard lock(g_stateMutex);
        g_state.system.volumePercent = ClampInt(static_cast<int>(next * 100.0f + 0.5f), 0, 100);
        g_state.system.volumeMuted = muted != FALSE;
        g_state.muted = g_state.system.volumeMuted;
        g_state.volume.active = false;
    }
    g_layoutDirty = true;
}

void SetEndpointVolumeScalar(float scalar) {
    g_mediaVolumeAdjustUntil.store(NowSeconds() + 1.25, std::memory_order_relaxed);
    // Reuse the endpoint during pointer drags so high-rate slider events do not
    // repeatedly activate COM devices on the overlay's input thread.
    static ComPtr<IAudioEndpointVolume> volume;
    if (!volume) {
        ComPtr<IMMDeviceEnumerator> enumerator;
        ComPtr<IMMDevice> device;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                      CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (SUCCEEDED(hr)) hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        if (SUCCEEDED(hr)) {
            hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                                  reinterpret_cast<void**>(volume.GetAddressOf()));
        }
        if (FAILED(hr) || !volume) return;
    }

    scalar = Clamp(scalar, 0.0f, 1.0f);
    if (FAILED(volume->SetMasterVolumeLevelScalar(scalar, nullptr))) {
        volume.Reset();
        return;
    }
    BOOL muted = FALSE;
    volume->GetMute(&muted);
    std::lock_guard lock(g_stateMutex);
    g_state.system.volumePercent = ClampInt(static_cast<int>(scalar * 100.0f + 0.5f), 0, 100);
    g_state.system.volumeMuted = muted != FALSE;
    g_state.muted = g_state.system.volumeMuted;
    g_state.volume.active = false;
    g_layoutDirty = true;
}

// Shared by the scrubber's live drag updates and its on-release commit.
void SeekMediaToTicks(int64_t targetTicks) {
    const uint64_t generation = g_seekRequestGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
    std::thread([targetTicks, generation]() {
        static std::mutex seekMutex;
        std::lock_guard seekLock(seekMutex);
        if (generation != g_seekRequestGeneration.load(std::memory_order_relaxed)) return;
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
            auto manager = Manager::RequestAsync().get();
            if (manager) {
                auto session = SelectActiveMediaSession(manager, -1, GetSettingsCopy());

                if (session) {
                    const auto timeline = session.GetTimelineProperties();
                    int64_t minTicks = timeline.MinSeekTime().count();
                    int64_t maxTicks = timeline.MaxSeekTime().count();
                    if (maxTicks <= minTicks) {
                        minTicks = timeline.StartTime().count();
                        maxTicks = timeline.EndTime().count();
                    }
                    int64_t requestedTicks = targetTicks;
                    if (maxTicks > minTicks) {
                        requestedTicks = std::clamp(requestedTicks, minTicks, maxTicks);
                    }
                    const bool applied = session.TryChangePlaybackPositionAsync(requestedTicks).get();
                    if (!applied) {
                        Wh_Log(L"The active media session rejected a seek request.");
                    }
                }
            }
        } catch (...) {}
    }).detach();
}

// Resolves the SMTC session matching the island's current media source, falling
// back to whatever Windows reports as the current session. Must be called on a
// thread with an initialised apartment.
winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession ResolveMediaSession() {
    using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
    auto manager = Manager::RequestAsync().get();
    if (!manager) {
        return nullptr;
    }

    return SelectActiveMediaSession(manager, -1, GetSettingsCopy());
}

// Sends a transport command: 0 = previous, 1 = play/pause, 2 = next.
//
// SMTC is the only mechanism used when a session exists. The global media keys
// are deliberately NOT used as a fallback for a session that merely *reports*
// failure: the shell routes those keys to whatever it considers the current
// session, which is not necessarily the session the island is showing (see
// SelectActiveMediaSession), and a player that acts on the request while still
// answering false would then be driven twice -- skipping two tracks, or toggling
// play/pause straight back. The keys are only synthesized when SMTC could not
// act at all, where there is nothing to double-drive and no better option.
void SendMediaTransportCommand(int cmd) {
    std::thread([cmd]() {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);

        bool sessionAttempted = false;
        bool reportedSuccess = false;
        try {
            auto session = ResolveMediaSession();
            if (session) {
                sessionAttempted = true;
                if (cmd == 0) {
                    reportedSuccess = session.TrySkipPreviousAsync().get();
                } else if (cmd == 1) {
                    reportedSuccess = session.TryTogglePlayPauseAsync().get();
                } else if (cmd == 2) {
                    reportedSuccess = session.TrySkipNextAsync().get();
                }
            }
        } catch (...) {
            sessionAttempted = false;
        }

        if (sessionAttempted) {
            if (!reportedSuccess) {
                // Not escalated to a media key on purpose (see above). Logged so
                // a player that genuinely refuses transport control is
                // diagnosable from the mod log.
                Wh_Log(L"Media transport command %d was refused by the session.", cmd);
            }
            return;
        }

        BYTE vk = 0;
        if (cmd == 0) {
            vk = VK_MEDIA_PREV_TRACK;
        } else if (cmd == 1) {
            vk = VK_MEDIA_PLAY_PAUSE;
        } else if (cmd == 2) {
            vk = VK_MEDIA_NEXT_TRACK;
        }
        if (vk != 0) {
            keybd_event(vk, 0, KEYEVENTF_EXTENDEDKEY, 0);
            keybd_event(vk, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
    }).detach();
}

struct WindowSearch {
    std::wstring targetTitle;
    std::wstring targetApp;
    HWND foundHwnd = nullptr;
    HWND fallbackHwnd = nullptr;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    auto* search = reinterpret_cast<WindowSearch*>(lParam);

    wchar_t title[512];
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0) {
        std::wstring wTitle(title);
        // Case-insensitive check if window title contains currently playing media title
        auto it = std::search(
            wTitle.begin(), wTitle.end(),
            search->targetTitle.begin(), search->targetTitle.end(),
            [](wchar_t ch1, wchar_t ch2) { return towlower(ch1) == towlower(ch2); }
        );

        if (it != wTitle.end()) {
            search->foundHwnd = hwnd;
            return FALSE; // found exact title, stop enumerating
        }
    }

    // Fallback: check if the window belongs to the target app (by process executable name)
    if (!search->fallbackHwnd && !search->targetApp.empty()) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != 0) {
            // Packaged apps often expose an AUMID in SMTC instead of an exe
            // path. Resolve the window's package AUMID as a second match path.
            HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (process) {
                UINT32 length = 0;
                const LONG sizeResult = GetApplicationUserModelId(process, &length, nullptr);
                if (sizeResult == ERROR_INSUFFICIENT_BUFFER && length > 1) {
                    std::vector<wchar_t> appId(length, L'\0');
                    if (GetApplicationUserModelId(process, &length, appId.data()) == ERROR_SUCCESS) {
                        const std::wstring targetLower = ToLowerCopy(search->targetApp);
                        const std::wstring appIdLower = ToLowerCopy(std::wstring(appId.data()));
                        if (appIdLower == targetLower ||
                            appIdLower.find(targetLower) != std::wstring::npos ||
                            targetLower.find(appIdLower) != std::wstring::npos) {
                            search->fallbackHwnd = hwnd;
                            CloseHandle(process);
                            return TRUE;
                        }
                    }
                }
                CloseHandle(process);
            }
            std::wstring exePath;
            if (ProcessImageNameForPid(pid, &exePath)) {
                std::wstring targetLower = ToLowerCopy(search->targetApp);
                std::wstring exeLower = ToLowerCopy(exePath);

                // Remove quotes from target AppUserModelId if any
                if (targetLower.size() >= 2 && targetLower.front() == L'"' && targetLower.back() == L'"') {
                    targetLower = targetLower.substr(1, targetLower.size() - 2);
                }

                if (exeLower == targetLower) {
                    search->fallbackHwnd = hwnd;
                } else {
                    std::wstring exeName = exeLower;
                    size_t slashPos = exeName.find_last_of(L"\\/");
                    if (slashPos != std::wstring::npos) {
                        exeName = exeName.substr(slashPos + 1);
                    }
                    if (exeName == targetLower || exeName == targetLower + L".exe") {
                        search->fallbackHwnd = hwnd;
                    }
                }
            }
        }
    }

    return TRUE;
}

void OpenRelevantApp() {
    std::wstring title;
    std::wstring app;
    {
        std::lock_guard lock(g_stateMutex);
        title = g_state.media.title;
        app = g_state.media.sourceAppUserModelId;
    }

    // Try to find and focus window containing track title (ideal for browser playing YouTube/Spotify)
    if (!title.empty() || !app.empty()) {
        WindowSearch search;
        search.targetTitle = title;
        search.targetApp = app;
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&search));

        HWND hwndToFocus = search.foundHwnd ? search.foundHwnd : search.fallbackHwnd;

        if (hwndToFocus) {
            if (IsIconic(hwndToFocus)) {
                ShowWindow(hwndToFocus, SW_RESTORE);
            }
            SetForegroundWindow(hwndToFocus);
            return;
        }
    }

    // Fallback: Launch or focus via AppUserModelId or Path
    if (!app.empty()) {
        std::wstring executePath = app;

        // Remove surrounding quotes if any
        if (executePath.size() >= 2 && executePath.front() == L'"' && executePath.back() == L'"') {
            executePath = executePath.substr(1, executePath.size() - 2);
        }

        bool isFilePath = (executePath.find(L":\\") != std::wstring::npos ||
                           (executePath.size() >= 4 && executePath.substr(executePath.size() - 4) == L".exe"));

        if (isFilePath) {
            if (GetFileAttributesW(executePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                // Path doesn't exist. Try 64-bit Program Files if it was in x86
                size_t x86Pos = executePath.find(L" (x86)");
                if (x86Pos != std::wstring::npos) {
                    std::wstring altPath = executePath;
                    altPath.erase(x86Pos, 6);
                    if (GetFileAttributesW(altPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                        executePath = altPath;
                    }
                }

                // If it STILL doesn't exist after trying alternatives, just gracefully abort!
                // Trying to guess 'brave.exe' triggers broken Windows Registry App Paths.
                if (GetFileAttributesW(executePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                    return;
                }
            }

            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpFile = executePath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        } else {
            // It's a UWP/Desktop AppUserModelId, launch via AppsFolder
            std::wstring shellPath = L"shell:AppsFolder\\" + executePath;
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpFile = shellPath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        return;
    }

    ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
}

void DismissTransientState() {
    std::lock_guard lock(g_stateMutex);
    g_state.clipboard.active = false;
    g_state.notification.active = false;
    g_state.volume.active = false;
    g_state.progress.active = false;
    g_state.capsLock.active = false;
    g_state.device.active = false;
    g_state.bluetoothDevice.active = false;
    g_state.battery.active = false;
    Wh_SetIntValue(L"ProgressPercent", -1);
}

void ShowContextMenu(HWND hwnd, POINT screenPoint) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 40, g_manuallyHidden.load() ? L"Show Island" : L"Hide Island");
    AppendMenuW(menu, MF_STRING, 1, L"Dismiss");

    if (g_settings.fileTrayModule) {
        size_t trayCount = 0;
        {
            std::lock_guard lock(g_stateMutex);
            trayCount = g_state.fileTrayItems.size();
        }
        std::wstring label = std::wstring(Loc(L"File Tray")) + L": ";
        if (trayCount == 0) {
            label += L"—";
            AppendMenuW(menu, MF_STRING | MF_GRAYED | MF_DISABLED, 41, label.c_str());
        } else {
            label += L"Clear " + std::to_wstring(trayCount);
            AppendMenuW(menu, MF_STRING, 41, label.c_str());
        }
    }
    AppendMenuW(menu, MF_STRING, 2, L"Pin expanded");
    std::wstring shapeStr = GetStringSettingCopy(L"Appearance.ShapeStyle");
    const int activeW11 = Wh_GetIntValue(L"W11StyleOverride", -1) >= 0
                          ? Wh_GetIntValue(L"W11StyleOverride", 0)
                          : EqualsNoCase(shapeStr, L"w11");
    AppendMenuW(menu, MF_STRING, 10, activeW11 ? L"Use iPhone Pill Style" : L"Use Windows 11 Flyout Style");
    const int activeNotch = Wh_GetIntValue(L"NotchStyleOverride", -1) >= 0
                          ? Wh_GetIntValue(L"NotchStyleOverride", 0)
                          : EqualsNoCase(shapeStr, L"notch");
    if (IsBottomPosition(g_settings.position)) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED | MF_DISABLED, 12, L"macOS Notch (Unavailable at Bottom)");
    } else {
        AppendMenuW(menu, MF_STRING, 12, activeNotch ? L"Disable macOS Notch Style" : L"Use macOS Notch Style");
    }
    const int activeExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0
                          ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0)
                          : (Wh_GetIntSetting(L"Behavior.ExpandOnHover") != 0);
    AppendMenuW(menu, MF_STRING, 11, activeExpandOnHover ? L"Expand on Click" : L"Expand on Hover");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 4, L"Transparency 100%");
    AppendMenuW(menu, MF_STRING, 5, L"Transparency 85%");
    AppendMenuW(menu, MF_STRING, 6, L"Transparency 70%");
    AppendMenuW(menu, MF_STRING, 7, L"Transparency 55%");
    AppendMenuW(menu, MF_STRING, 8, L"Reset transparency");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    // Color theme presets. Built from kThemePalettes and nested in a submenu:
    // nine palettes listed flat would dominate the root menu.
    const int activeTheme = Wh_GetIntValue(kThemeValueName, static_cast<int>(g_settings.themePreset));
    HMENU themeMenu = CreatePopupMenu();
    for (int i = 0; i < kCustomThemeIndex; ++i) {
        AppendMenuW(themeMenu, MF_STRING | (activeTheme == i ? MF_CHECKED : 0),
                    kThemeMenuIdBase + static_cast<UINT>(i), kThemePalettes[i].label);
    }
    AppendMenuW(themeMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(themeMenu, MF_STRING | (activeTheme >= kCustomThemeIndex ? MF_CHECKED : 0),
                kThemeMenuIdBase + static_cast<UINT>(kCustomThemeIndex), L"Custom Colors");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(themeMenu), L"Theme");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 9, L"Open Windhawk settings");

    SetForegroundWindow(hwnd);
    const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                   screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    // Theme ids are a contiguous range rather than individual cases, so adding a
    // palette to kThemePalettes needs no change here.
    if (cmd >= kThemeMenuIdBase &&
        cmd <= kThemeMenuIdBase + static_cast<UINT>(kCustomThemeIndex)) {
        Wh_SetIntValue(kThemeValueName, static_cast<int>(cmd - kThemeMenuIdBase));
        LoadSettings();
        return;
    }

    switch (cmd) {
        case 1:
            DismissTransientState();
            g_clickExpanded = false;
            g_layoutDirty = true;
            TriggerNudge();
            break;
        case 2:
            Wh_SetIntValue(L"PinnedExpanded", Wh_GetIntValue(L"PinnedExpanded", 0) ? 0 : 1);
            break;
        case 4:
            Wh_SetIntValue(L"PillOpacityOverride", 100);
            LoadSettings();
            break;
        case 5:
            Wh_SetIntValue(L"PillOpacityOverride", 85);
            LoadSettings();
            break;
        case 6:
            Wh_SetIntValue(L"PillOpacityOverride", 70);
            LoadSettings();
            break;
        case 7:
            Wh_SetIntValue(L"PillOpacityOverride", 55);
            LoadSettings();
            break;
        case 8:
            Wh_SetIntValue(L"PillOpacityOverride", -1);
            LoadSettings();
            break;
        case 9: {
            // Launch the Windhawk UI, resolved relative to our own module rather
            // than by launching our own executable.
            //
            // Running the current process directly only worked because under
            // Windhawk 1.x that *is* windhawk.exe. Windhawk 2.0 runs tool mods in
            // a separate windhawk-mod.exe, so re-launching the current process
            // would have spawned another mod host instead of opening settings.
            // Taking the directory and appending windhawk.exe is correct on both,
            // since the two executables live side by side.
            wchar_t windhawkPath[MAX_PATH] = {};
            const DWORD pathLen =
                GetModuleFileNameW(nullptr, windhawkPath, ARRAYSIZE(windhawkPath));
            if (pathLen == 0 || pathLen >= ARRAYSIZE(windhawkPath)) {
                Wh_Log(L"Could not resolve the Windhawk directory (GetModuleFileNameW: %lu).",
                       pathLen);
                break;
            }

            wchar_t* lastSlash = wcsrchr(windhawkPath, L'\\');
            if (!lastSlash) {
                Wh_Log(L"Unexpected module path with no directory separator.");
                break;
            }
            *(lastSlash + 1) = L'\0';  // keep the trailing slash

            if (wcscat_s(windhawkPath, ARRAYSIZE(windhawkPath), L"windhawk.exe") != 0) {
                Wh_Log(L"Windhawk directory path is too long to append the executable name.");
                break;
            }

            HINSTANCE result = ShellExecuteW(nullptr, L"open",
                                             windhawkPath,
                                             nullptr,
                                             nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(result) <= 32) {
                Wh_Log(L"Failed to open Windhawk settings (%s).", windhawkPath);
            }
            break;
        }
        case 10: {
            std::wstring shapeStr = GetStringSettingCopy(L"Appearance.ShapeStyle");
            const int activeW11Val = Wh_GetIntValue(L"W11StyleOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"W11StyleOverride", 0)
                                  : EqualsNoCase(shapeStr, L"w11");
            Wh_SetIntValue(L"W11StyleOverride", activeW11Val ? 0 : 1);
            if (!activeW11Val) Wh_SetIntValue(L"NotchStyleOverride", 0);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 12: {
            if (IsBottomPosition(g_settings.position)) {
                break;
            }
            std::wstring shapeStr = GetStringSettingCopy(L"Appearance.ShapeStyle");
            const int activeNotchVal = Wh_GetIntValue(L"NotchStyleOverride", -1) >= 0
                                    ? Wh_GetIntValue(L"NotchStyleOverride", 0)
                                    : EqualsNoCase(shapeStr, L"notch");
            Wh_SetIntValue(L"NotchStyleOverride", activeNotchVal ? 0 : 1);
            if (!activeNotchVal) Wh_SetIntValue(L"W11StyleOverride", 0);
            LoadSettings();
            g_layoutDirty = true;
            TriggerNudge();
            break;
        }
        case 11: {
            const int activeExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0)
                                  : (Wh_GetIntSetting(L"Behavior.ExpandOnHover") != 0);
            Wh_SetIntValue(L"ExpandOnHoverOverride", activeExpandOnHover ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 40: {
            // Only reachable while visible (a hidden window can't be
            // right-clicked), so in practice this toggles hidden -> the
            // hotkey is the only way back. That's intentional; see readme.
            const bool nowHidden = !g_manuallyHidden.load();
            g_manuallyHidden = nowHidden;
            Wh_SetIntValue(L"ManuallyHidden", nowHidden ? 1 : 0);
            if (nowHidden) {
                g_hotkeyUnhideUntil.store(0.0);
            }
            g_layoutDirty = true;
            break;
        }
        case 41: {
            // Clear saved references without touching the original files.
            {
                std::lock_guard lock(g_stateMutex);
                g_state.fileTrayItems.clear();
            }
            SaveFileTrayToDisk();
            g_hoveredFileTrayRow = -1;
            g_layoutDirty = true;
            break;
        }
    }
}

void ShowClipboardHistoryItemMenu(HWND hwnd, POINT screenPoint, uint64_t itemId) {
    bool pinned = false;
    bool found = false;
    {
        std::lock_guard lock(g_stateMutex);
        if (g_state.clipboardHistory) {
            for (const ClipboardHistoryItem& item : *g_state.clipboardHistory) {
                if (item.id == itemId) {
                    pinned = item.pinned;
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) return;

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 80, pinned ? L"Unpin item" : L"Pin item");
    AppendMenuW(menu, MF_STRING | (pinned ? MF_GRAYED : MF_ENABLED), 81, L"Remove item");
    SetForegroundWindow(hwnd);
    const UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                        screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
    if (command != 80 && command != 81) return;

    {
        std::lock_guard lock(g_stateMutex);
        if (!g_state.clipboardHistory) return;
        std::vector<ClipboardHistoryItem> items = *g_state.clipboardHistory;
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const ClipboardHistoryItem& item) { return item.id == itemId; });
        if (it == items.end()) return;
        if (command == 80) {
            it->pinned = !it->pinned;
            std::stable_partition(items.begin(), items.end(),
                [](const ClipboardHistoryItem& item) { return item.pinned; });
        } else if (!it->pinned) {
            items.erase(it);
        } else {
            return;
        }
        g_state.clipboardHistory =
            std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
    }
    if (command == 80) g_clipboardHistoryPage.store(0, std::memory_order_relaxed);
    SaveClipboardHistoryToDisk();
    g_layoutDirty = true;
}

void ClearUnpinnedClipboardHistory() {
    {
        std::lock_guard lock(g_stateMutex);
        if (!g_state.clipboardHistory) return;
        std::vector<ClipboardHistoryItem> items;
        for (const ClipboardHistoryItem& item : *g_state.clipboardHistory) {
            if (item.pinned) items.push_back(item);
        }
        g_state.clipboardHistory =
            std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
        g_clipboardHistoryPage.store(0, std::memory_order_relaxed);
    }
    SaveClipboardHistoryToDisk();
    g_layoutDirty = true;
}

void ToggleClipboardItemPin(uint64_t itemId) {
    {
        std::lock_guard lock(g_stateMutex);
        if (!g_state.clipboardHistory) return;
        std::vector<ClipboardHistoryItem> items = *g_state.clipboardHistory;
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const ClipboardHistoryItem& item) { return item.id == itemId; });
        if (it == items.end()) return;
        it->pinned = !it->pinned;
        std::stable_partition(items.begin(), items.end(),
            [](const ClipboardHistoryItem& item) { return item.pinned; });
        g_state.clipboardHistory =
            std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
        g_clipboardHistoryPage.store(0, std::memory_order_relaxed);
    }
    SaveClipboardHistoryToDisk();
    g_layoutDirty = true;
}

void RemoveUnpinnedClipboardItem(uint64_t itemId) {
    {
        std::lock_guard lock(g_stateMutex);
        if (!g_state.clipboardHistory) return;
        std::vector<ClipboardHistoryItem> items = *g_state.clipboardHistory;
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const ClipboardHistoryItem& item) { return item.id == itemId; });
        if (it == items.end() || it->pinned) return;
        items.erase(it);
        g_state.clipboardHistory =
            std::make_shared<const std::vector<ClipboardHistoryItem>>(std::move(items));
    }
    SaveClipboardHistoryToDisk();
    g_layoutDirty = true;
}

bool CopyClipboardHistoryItem(HWND hwnd, uint64_t itemId) {
    ClipboardHistoryItem item;
    bool found = false;
    {
        std::lock_guard lock(g_stateMutex);
        if (g_state.clipboardHistory) {
            auto it = std::find_if(g_state.clipboardHistory->begin(), g_state.clipboardHistory->end(),
                                   [itemId](const ClipboardHistoryItem& value) { return value.id == itemId; });
            if (it != g_state.clipboardHistory->end()) {
                item = *it;
                found = true;
            }
        }
    }
    auto openClipboard = [&]() {
        for (int attempt = 0; attempt < 8; ++attempt) {
            if (OpenClipboard(hwnd)) return true;
            Sleep(8);
        }
        return false;
    };
    if (!found || !openClipboard()) return false;

    bool copied = false;
    if (!item.image) {
        const SIZE_T bytes = (item.text.size() + 1) * sizeof(wchar_t);
        HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (data) {
            void* target = GlobalLock(data);
            if (target) {
                memcpy(target, item.text.c_str(), bytes);
                GlobalUnlock(data);
                if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, data)) {
                    copied = true;
                    data = nullptr;
                }
            }
            if (data) GlobalFree(data);
        }
    } else if (item.imagePreview.width && item.imagePreview.height &&
               item.imagePreview.bgra.size() >= static_cast<size_t>(item.imagePreview.width) *
                                                  item.imagePreview.height * 4) {
        BITMAPV5HEADER header{};
        header.bV5Size = sizeof(header);
        header.bV5Width = static_cast<LONG>(item.imagePreview.width);
        header.bV5Height = -static_cast<LONG>(item.imagePreview.height);
        header.bV5Planes = 1;
        header.bV5BitCount = 32;
        header.bV5Compression = BI_BITFIELDS;
        header.bV5RedMask = 0x00FF0000;
        header.bV5GreenMask = 0x0000FF00;
        header.bV5BlueMask = 0x000000FF;
        header.bV5AlphaMask = 0xFF000000;
        header.bV5CSType = LCS_sRGB;
        const SIZE_T pixelBytes = item.imagePreview.width * item.imagePreview.height * 4ull;
        HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, sizeof(header) + pixelBytes);
        if (data) {
            BYTE* target = static_cast<BYTE*>(GlobalLock(data));
            if (target) {
                memcpy(target, &header, sizeof(header));
                memcpy(target + sizeof(header), item.imagePreview.bgra.data(), pixelBytes);
                GlobalUnlock(data);
                if (EmptyClipboard() && SetClipboardData(CF_DIBV5, data)) {
                    copied = true;
                    data = nullptr;
                }
            }
            if (data) GlobalFree(data);
        }
    }
    CloseClipboard();
    return copied;
}

bool SetFilePathClipboard(HWND hwnd, const std::wstring& path, bool cut = false) {
    if (path.empty()) return false;
    const size_t pathBytes = (path.size() + 2) * sizeof(wchar_t);
    const SIZE_T bytes = sizeof(DROPFILES) + pathBytes;
    HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
    if (!data) return false;
    auto* drop = static_cast<DROPFILES*>(GlobalLock(data));
    if (!drop) {
        GlobalFree(data);
        return false;
    }
    drop->pFiles = sizeof(DROPFILES);
    drop->fWide = TRUE;
    memcpy(reinterpret_cast<BYTE*>(drop) + sizeof(DROPFILES), path.c_str(), path.size() * sizeof(wchar_t));
    GlobalUnlock(data);

    bool clipboardOpened = false;
    for (int attempt = 0; attempt < 8 && !clipboardOpened; ++attempt) {
        clipboardOpened = OpenClipboard(hwnd) != FALSE;
        if (!clipboardOpened) Sleep(8);
    }
    if (!clipboardOpened) {
        GlobalFree(data);
        return false;
    }
    bool success = false;
    if (EmptyClipboard() && SetClipboardData(CF_HDROP, data)) {
        success = true;  // ownership transferred to the clipboard
        data = nullptr;
        // Explorer and many file managers honor this standard clipboard hint
        // to paste the referenced file as a move (cut) or copy operation.
        const UINT effectFormat = RegisterClipboardFormatW(L"Preferred DropEffect");
        HGLOBAL effectData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DWORD));
        if (effectFormat && effectData) {
            DWORD* effect = static_cast<DWORD*>(GlobalLock(effectData));
            if (effect) {
                *effect = cut ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
                GlobalUnlock(effectData);
                if (!SetClipboardData(effectFormat, effectData)) GlobalFree(effectData);
            } else {
                GlobalFree(effectData);
            }
        } else if (effectData) {
            GlobalFree(effectData);
        }
    }
    CloseClipboard();
    if (data) GlobalFree(data);
    return success;
}

void ShowFileTrayItemMenu(HWND hwnd, POINT screenPoint, const std::wstring& path) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, 80, L"Open");
    AppendMenuW(menu, MF_STRING, 82, L"Copy");
    AppendMenuW(menu, MF_STRING, 84, L"Cut");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 83, L"Remove from File Tray");
    SetForegroundWindow(hwnd);
    // The menu is opened by a left click on the row's three-dot button. Do not
    // require right-button selection here or the visible actions appear inert.
    const UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTBUTTON,
                                        screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    if (command == 80) {
        SHELLEXECUTEINFOW execute = {sizeof(execute)};
        execute.fMask = SEE_MASK_FLAG_NO_UI;
        execute.lpFile = path.c_str();
        execute.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&execute);
    } else if (command == 82) {
        SetFilePathClipboard(hwnd, path);
    } else if (command == 84) {
        SetFilePathClipboard(hwnd, path, true);
    } else if (command == 83) {
        {
            std::lock_guard lock(g_stateMutex);
            auto& items = g_state.fileTrayItems;
            items.erase(std::remove_if(items.begin(), items.end(),
                                       [&path](const FileTrayItem& item) {
                                           return _wcsicmp(item.path.c_str(), path.c_str()) == 0;
                                       }), items.end());
        }
        SaveFileTrayToDisk();
        g_hoveredFileTrayRow = -1;
        g_fileTrayScrollOffset = 0;
        g_layoutDirty = true;
    }
}

// ── Weather iconography ──────────────────────────────────────────────────────
// The dashboard used colour emoji (☀️ ⛅ 🌡️) rendered with ENABLE_COLOR_FONT,
// which clashed badly with the island's monochrome, accent-tinted UI and varied
// in size and baseline between glyphs. These are drawn as vectors instead: they
// inherit the accent colour, scale cleanly, sit on a predictable baseline, and
// can never fall back to a missing-glyph box the way a font-dependent icon can.
enum class WeatherVisual {
    Clear,
    PartlyCloudy,
    Cloudy,
    Fog,
    Storm,
    Rain,
    Snow,
    Unknown,
};

// Groupings mirror GetWeatherIconAndText so both stay keyed off the same WWO
// weather codes.
WeatherVisual WeatherVisualFromCode(int code) {
    switch (code) {
        case 113:
            return WeatherVisual::Clear;
        case 116:
            return WeatherVisual::PartlyCloudy;
        case 119: case 122:
            return WeatherVisual::Cloudy;
        case 143: case 248: case 260:
            return WeatherVisual::Fog;
        case 200: case 386: case 389: case 392: case 395:
            return WeatherVisual::Storm;
        case 176: case 263: case 266: case 281: case 284: case 293: case 296:
        case 299: case 302: case 305: case 308: case 311: case 314: case 353:
        case 356: case 359:
            return WeatherVisual::Rain;
        case 179: case 182: case 185: case 227: case 230: case 317: case 320:
        case 323: case 326: case 329: case 332: case 335: case 338: case 350:
        case 362: case 365: case 368: case 371:
            return WeatherVisual::Snow;
        default:
            return WeatherVisual::Unknown;
    }
}

struct MarqueeLayoutCache {
    std::wstring text;
    IDWriteTextFormat* format = nullptr;
    float wrapWidth = 0.0f;
    ComPtr<IDWriteTextLayout> layout;
    DWRITE_TEXT_METRICS metrics{};
};

// Measured geometry of the collapsed idle strip, produced by
// Renderer::MeasureIdleStrip. The render loop uses totalWidth to size the
// island; DrawIdleDashboard uses the per-slot widths to place the clock, divider
// and weather reading inside it.
struct IdleStripMetrics {
    float clockWidth = 0.0f;
    float weatherWidth = 0.0f;
    bool hasWeather = false;
    bool hasPrivacy = false;
    float totalWidth = IdleStripLayout::kMinWidth;
};

// Substitutes '0' for every decimal digit before measuring.
//
// Proportional faces give '1' a visibly narrower advance than '0', so measuring
// the live clock would change the pill's width as the time changed -- with
// seconds enabled that is a layered-window resize and reposition every second,
// and a visible twitch. Normalising to the widest digit makes the width stable
// for a given digit count, and since '0' is never narrower than the digit it
// replaces, the real string is guaranteed to fit the box we reserve.
std::wstring WidestDigitForm(std::wstring text) {
    for (wchar_t& c : text) {
        if (c >= L'0' && c <= L'9') {
            c = L'0';
        }
    }
    return text;
}

class Renderer {
   public:
    bool MediaVolumeTransitioning(bool controlVisible) const {
        if (!controlVisible) return false;
        const float target = g_hoveredMediaVolume.load(std::memory_order_relaxed) ? 1.0f : 0.0f;
        return std::fabs(mediaVolumeReveal_ - target) > 0.01f;
    }

    bool Initialize(HWND hwnd) {
        hwnd_ = hwnd;

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                       __uuidof(ID2D1Factory),
                                       reinterpret_cast<void**>(d2dFactory_.GetAddressOf()));
        if (FAILED(hr)) {
            return false;
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                 reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf()));
        if (FAILED(hr)) {
            return false;
        }

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f,
            D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

        hr = d2dFactory_->CreateDCRenderTarget(&props, &target_);
        if (FAILED(hr)) {
            return false;
        }

        const Settings initialSettings = GetSettingsCopy();
        EnsureTextFormats(initialSettings.sizeScale, initialSettings.fontFamily,
                          initialSettings.textScale);

        return CreateBackingBitmap(520, 140);
    }

    bool Render(const SharedState& state, const Settings& settings, const Activity& primary,
                const std::optional<Activity>& secondary, float width, float height,
                float nudge, bool hover, bool pinned, double now) {
        EnsureTextFormats(settings.sizeScale, settings.fontFamily, settings.textScale);
        const int pixelWidth = std::max(1, static_cast<int>(std::ceil(width + kRenderPadX * 2.0f)));
        const int pixelHeight = std::max(1, static_cast<int>(std::ceil(height + kRenderPadY * 2.0f)));

        if (pixelWidth != bitmapWidth_ || pixelHeight != bitmapHeight_) {
            if (!CreateBackingBitmap(pixelWidth, pixelHeight)) {
                return false;
            }
            PositionOverlayWindow(hwnd_, pixelWidth, pixelHeight);
        } else if (g_layoutDirty.exchange(false)) {
            PositionOverlayWindow(hwnd_, pixelWidth, pixelHeight);
        }

        RECT rc = {0, 0, bitmapWidth_, bitmapHeight_};
        HRESULT hr = target_->BindDC(memDc_, &rc);
        if (FAILED(hr)) {
            return false;
        }

        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));

        EnsureBrushes(settings, state, now);
        settingsOpacity_ = settings.pillOpacity;

        const float hoverScale = (settings.expandOnHover && (hover || pinned)) ? 1.025f : 1.0f;
        const float scale = hoverScale;

        const float top = (settings.notchStyle || settings.borderMergedMode) ? std::max(0.0f, nudge) : (kRenderPadY + nudge);
        const float left = kRenderPadX;

        if (width >= 2.0f && height >= 2.0f) {
            if (secondary) {
                const float gap = 12.0f * settings.sizeScale;
                const float maxH = std::max(primary.height, secondary->height);
                const float pTop = top + (maxH - primary.height) * 0.5f;
                const float sTop = top + (maxH - secondary->height) * 0.5f;

                DrawPill(state, settings, primary,
                         D2D1::RectF(left, pTop, left + primary.width, pTop + primary.height),
                         scale, now);
                DrawPill(state, settings, *secondary,
                         D2D1::RectF(left + primary.width + gap, sTop,
                                      left + primary.width + gap + secondary->width,
                                      sTop + secondary->height),
                         scale, now);
            } else {
                DrawPill(state, settings, primary,
                         D2D1::RectF(left, top, left + width, top + height), scale, now);
            }
        }

        hr = target_->EndDraw();
        if (FAILED(hr)) {
            return false;
        }

        POINT src = {0, 0};
        SIZE size = {bitmapWidth_, bitmapHeight_};
        POINT dst = {};
        RECT winRect = {};
        GetWindowRect(hwnd_, &winRect);
        dst.x = winRect.left;
        dst.y = winRect.top;

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = static_cast<BYTE>(Clamp(settings.pillOpacity, 0.35f, 1.0f) * 255.0f);
        blend.AlphaFormat = AC_SRC_ALPHA;

        return UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend,
                                   ULW_ALPHA) != FALSE;
    }

    void Shutdown() {
        marqueeTitleCache_.layout.Reset();
        marqueeArtistCache_.layout.Reset();
        marqueeAlbumCache_.layout.Reset();
        marqueeClipboardCache_.layout.Reset();
        marqueeNotificationCache_.layout.Reset();
        scratchColorBrush_.Reset();

        artBitmap_.Reset();
        notificationIconBitmap_.Reset();
        mediaSourceIconBitmap_.Reset();
        clipboardIconBitmap_.Reset();
        clipboardImageBitmap_.Reset();
        for (auto& bitmap : clipboardHistoryBitmaps_) bitmap.Reset();
        clipboardHistoryBitmapGenerations_.fill(0);
        accentBrush_.Reset();
        redBrush_.Reset();
        textBrush_.Reset();
        mutedBrush_.Reset();
        tintBrush_.Reset();
        shadowBrush_.Reset();
        micDotBrush_.Reset();
        micGlowBrush_.Reset();
        camDotBrush_.Reset();
        camGlowBrush_.Reset();
        weatherDescFormat_.Reset();
        micGlowBrush_.Reset();
        camDotBrush_.Reset();
        camGlowBrush_.Reset();
        target_.Reset();
        textFormat_.Reset();
        smallTextFormat_.Reset();
        boldTextFormat_.Reset();
        hugeTextFormat_.Reset();
        clockFormat_.Reset();
        iconFormat_.Reset();
        mediaPlayIconFormat_.Reset();
        mediaNavIconFormat_.Reset();
        idleTextFormat_.Reset();
        calDayLargeFormat_.Reset();
        calGridFormat_.Reset();
        dwriteFactory_.Reset();
        d2dFactory_.Reset();

        if (oldBitmap_) {
            SelectObject(memDc_, oldBitmap_);
            oldBitmap_ = nullptr;
        }
        if (dib_) {
            DeleteObject(dib_);
            dib_ = nullptr;
        }
        if (memDc_) {
            DeleteDC(memDc_);
            memDc_ = nullptr;
        }
    }

    // Measures the collapsed idle strip so the render loop can size the island to
    // the text it is actually about to paint, instead of the old fixed 96/170px.
    // DrawIdleDashboard calls this too and lays the slots out from the same
    // numbers, so the pill can never be sized for a clock width the painter is
    // not using.
    IdleStripMetrics MeasureIdleStrip(const SharedState& state, const Settings& settings,
                                      double now) {
        // Idempotent and cheap when nothing changed, but necessary here: textScale
        // drives the idle font size, so measuring before the formats are rebuilt
        // would size the pill for the previous Text size setting.
        EnsureTextFormats(settings.sizeScale, settings.fontFamily, settings.textScale);

        IdleStripMetrics metrics;
        IDWriteTextFormat* fmt = idleTextFormat_ ? idleTextFormat_.Get() : smallTextFormat_.Get();

        SYSTEMTIME local = {};
        GetLocalTime(&local);
        const std::wstring clock = FormatIslandTime(local, settings.clockFollowSystem,
                                                    settings.use24HourClock, settings.showSeconds);
        metrics.clockWidth =
            MeasureTextWidthCached(WidestDigitForm(clock), fmt, idleClockWidthCache_);

        metrics.hasWeather = settings.weather;
        if (metrics.hasWeather) {
            // Mirrors the label DrawIdleDashboard builds, including the no-data
            // placeholder, so the reserved slot matches what gets drawn.
            const bool hasData = state.weather.hasData && (now - state.weather.lastUpdated < 3600.0);
            wchar_t label[32] = {};
            if (hasData) {
                std::wstring icon = L"\U0001F321\uFE0F";
                std::wstring desc = state.weather.weatherDesc;
                GetWeatherIconAndText(state.weather.weatherCode, icon, desc);
                swprintf_s(label, L"%s %.0f\x00B0", icon.c_str(), state.weather.temperature);
            } else {
                wcscpy_s(label, ARRAYSIZE(label), L"\U0001F321\uFE0F --\x00B0");
            }
            metrics.weatherWidth =
                MeasureTextWidthCached(WidestDigitForm(label), fmt, idleWeatherWidthCache_);
        }

        metrics.hasPrivacy =
            (state.system.micActive && settings.privacyDots && settings.privacyDotsMic) ||
            (state.system.cameraActive && settings.privacyDots && settings.privacyDotsCam);

        float total = IdleStripLayout::kPadX * 2.0f + metrics.clockWidth;
        if (metrics.hasWeather) {
            total += IdleStripLayout::kSlotGap * 2.0f + IdleStripLayout::kDividerWidth +
                     metrics.weatherWidth;
        }
        if (metrics.hasPrivacy) {
            total += IdleStripLayout::kPrivacyReserve;
        }

        total = std::ceil(total / IdleStripLayout::kWidthQuantum) * IdleStripLayout::kWidthQuantum;
        metrics.totalWidth = Clamp(total, IdleStripLayout::kMinWidth, IdleStripLayout::kMaxWidth);
        return metrics;
    }

   private:
    // Keyed on the string alone. EnsureTextFormats invalidates both caches
    // whenever it rebuilds the formats, so a cached width can never outlive the
    // font size it was measured at -- comparing the IDWriteTextFormat pointer
    // would not be enough, since a rebuilt format can land on the freed address.
    struct TextWidthCache {
        std::wstring key;
        float width = 0.0f;
        bool valid = false;
    };
    TextWidthCache idleClockWidthCache_;
    TextWidthCache idleWeatherWidthCache_;

    float MeasureTextWidthCached(const std::wstring& text, IDWriteTextFormat* fmt,
                                 TextWidthCache& cache) {
        if (cache.valid && cache.key == text) {
            return cache.width;
        }

        float width = 0.0f;
        if (fmt && dwriteFactory_ && !text.empty()) {
            ComPtr<IDWriteTextLayout> layout;
            // Effectively unbounded wrap width: the idle formats are NO_WRAP, and
            // we want the natural advance, not a wrapped block.
            if (SUCCEEDED(dwriteFactory_->CreateTextLayout(
                    text.c_str(), static_cast<UINT32>(text.size()), fmt,
                    4096.0f, IdleStripLayout::kHeight, &layout)) &&
                layout) {
                DWRITE_TEXT_METRICS tm = {};
                if (SUCCEEDED(layout->GetMetrics(&tm))) {
                    width = std::max(tm.width, tm.widthIncludingTrailingWhitespace);
                }
            }
        }

        cache.key = text;
        cache.width = width;
        cache.valid = true;
        return width;
    }

    bool CreateBackingBitmap(int width, int height) {
        if (oldBitmap_) {
            SelectObject(memDc_, oldBitmap_);
            oldBitmap_ = nullptr;
        }
        if (dib_) {
            DeleteObject(dib_);
            dib_ = nullptr;
        }
        if (!memDc_) {
            HDC screen = GetDC(nullptr);
            memDc_ = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            if (!memDc_) {
                return false;
            }
        }

        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        dib_ = CreateDIBSection(memDc_, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!dib_) {
            return false;
        }

        oldBitmap_ = static_cast<HBITMAP>(SelectObject(memDc_, dib_));
        bitmapWidth_ = width;
        bitmapHeight_ = height;
        return true;
    }

    float lastFontScale_ = 0.0f;
    float lastTextScale_ = 0.0f;
    std::wstring lastFontFamily_;
    // TextScale magnifies the complete dashboard layout, not only its glyphs.
    // The renderer enlarges section geometry and applies the same transform,
    // so fixed-height labels and controls remain proportionate and unclipped.
    void EnsureTextFormats(float scale, const std::wstring& fontFamily, float textScale) {
        if (std::abs(scale - lastFontScale_) < 0.001f && fontFamily == lastFontFamily_ &&
            std::abs(textScale - lastTextScale_) < 0.001f) {
            return;
        }
        lastTextScale_ = textScale;
        const float ts = 1.0f;

        // Every format below is about to be recreated at a new size, so any
        // width measured against the old ones is stale.
        idleClockWidthCache_.valid = false;
        idleWeatherWidthCache_.valid = false;

        textFormat_ = nullptr;
        smallTextFormat_ = nullptr;
        clockFormat_ = nullptr;
        boldTextFormat_ = nullptr;
        hugeTextFormat_ = nullptr;
        iconFormat_ = nullptr;
        mediaPlayIconFormat_ = nullptr;
        mediaNavIconFormat_ = nullptr;
        idleTextFormat_ = nullptr;
        calDayLargeFormat_ = nullptr;
        calGridFormat_ = nullptr;
        timeDashboardFormat_ = nullptr;
        dateDashboardFormat_ = nullptr;

        const wchar_t* defaultDisplay = L"Segoe UI Variable Display";
        const wchar_t* defaultSmall = L"Segoe UI Variable Small";
        const wchar_t* mainFamily = fontFamily.empty() ? defaultDisplay : fontFamily.c_str();
        const wchar_t* smallFamily = fontFamily.empty() ? defaultSmall : fontFamily.c_str();

        auto createFormat = [&](const wchar_t* family, const wchar_t* fallback,
                                DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style,
                                DWRITE_FONT_STRETCH stretch, float size,
                                ComPtr<IDWriteTextFormat>& out) {
            HRESULT hr = dwriteFactory_->CreateTextFormat(
                family, nullptr, weight, style, stretch, size, L"", &out);
            if (FAILED(hr) || !out) {
                if (fallback && wcscmp(family, fallback) != 0) {
                    hr = dwriteFactory_->CreateTextFormat(
                        fallback, nullptr, weight, style, stretch, size, L"", &out);
                }
                if (FAILED(hr) || !out) {
                    dwriteFactory_->CreateTextFormat(
                        L"Segoe UI", nullptr, weight, style, stretch, size, L"", &out);
                }
            }
        };

        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 13.5f * ts, textFormat_);
        createFormat(smallFamily, defaultSmall,
                     DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 11.0f * ts, smallTextFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 18.0f * ts, clockFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 12.0f * ts, boldTextFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 42.0f * ts, hugeTextFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 13.0f * ts, idleTextFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 63.0f * ts, calDayLargeFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 14.4f * ts, calGridFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 56.0f * ts, timeDashboardFormat_);
        createFormat(mainFamily, defaultDisplay,
                     DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                     DWRITE_FONT_STRETCH_NORMAL, 16.0f * ts, dateDashboardFormat_);

        usingFluentIcons_ = false;
        ComPtr<IDWriteFontCollection> sysFonts;
        if (dwriteFactory_ && SUCCEEDED(dwriteFactory_->GetSystemFontCollection(&sysFonts, FALSE)) && sysFonts) {
            UINT32 fontIdx = 0;
            BOOL fontFound = FALSE;
            if (SUCCEEDED(sysFonts->FindFamilyName(L"Segoe Fluent Icons", &fontIdx, &fontFound)) && fontFound) {
                usingFluentIcons_ = true;
            }
        }

        const wchar_t* iconFontFamily = usingFluentIcons_ ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";

        HRESULT hrIcon = dwriteFactory_->CreateTextFormat(
            iconFontFamily, nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &iconFormat_);
        if (FAILED(hrIcon) || !iconFormat_) {
            dwriteFactory_->CreateTextFormat(
                L"Segoe MDL2 Assets", nullptr,
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &iconFormat_);
        }

        HRESULT hrPlay = dwriteFactory_->CreateTextFormat(
            iconFontFamily, nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"", &mediaPlayIconFormat_);
        if (FAILED(hrPlay) || !mediaPlayIconFormat_) {
            dwriteFactory_->CreateTextFormat(
                L"Segoe MDL2 Assets", nullptr,
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"", &mediaPlayIconFormat_);
        }

        HRESULT hrNav = dwriteFactory_->CreateTextFormat(
            iconFontFamily, nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &mediaNavIconFormat_);
        if (FAILED(hrNav) || !mediaNavIconFormat_) {
            dwriteFactory_->CreateTextFormat(
                L"Segoe MDL2 Assets", nullptr,
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &mediaNavIconFormat_);
        }

        if (textFormat_) {
            textFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (smallTextFormat_) {
            smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (boldTextFormat_) {
            boldTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            boldTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            // Match every other centered format: without this the text hugs the
            // top of its layout rect, which left the weather dashboard's city /
            // icon / temperature visually "stuck" near the top of the island.
            boldTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (hugeTextFormat_) {
            hugeTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            hugeTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            hugeTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (clockFormat_) {
            clockFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            clockFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            clockFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (idleTextFormat_) {
            idleTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            idleTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            idleTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (calDayLargeFormat_) {
            calDayLargeFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            calDayLargeFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            calDayLargeFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (calGridFormat_) {
            calGridFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            calGridFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            calGridFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (timeDashboardFormat_) {
            timeDashboardFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            timeDashboardFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            timeDashboardFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (dateDashboardFormat_) {
            dateDashboardFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            dateDashboardFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            dateDashboardFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (iconFormat_) {
            iconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (mediaPlayIconFormat_) {
            mediaPlayIconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            mediaPlayIconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            mediaPlayIconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (mediaNavIconFormat_) {
            mediaNavIconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            mediaNavIconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            mediaNavIconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        if (fontFamily != lastFontFamily_) {
            marqueeTitleCache_.layout.Reset();
            marqueeArtistCache_.layout.Reset();
            marqueeAlbumCache_.layout.Reset();
            marqueeClipboardCache_.layout.Reset();
            marqueeNotificationCache_.layout.Reset();
            weatherDescFormat_.Reset();
            weatherDescFormatSize_ = -1.0f;
        }

        lastFontScale_ = scale;
        lastFontFamily_ = fontFamily;
    }

    void EnsureBrushes(const Settings& settings, const SharedState& state, double now) {
        D2D1_COLOR_F targetAccent = settings.customAccent;
        if (settings.accentMode == AccentMode::System) {
            targetAccent = GetSystemAccentColor();
        } else if (settings.accentMode == AccentMode::Auto && !state.media.art.bgra.empty()) {
            // Already contrast-corrected when the art was decoded.
            targetAccent = state.media.art.sampledAccent;
        } else {
            // A custom or system accent was never checked against the island's
            // own background. That did not matter while every theme was dark,
            // but it does now that one is light: the default cyan sits at about
            // 1.6:1 on Porcelain. Corrected before the smoothing lerp so the
            // value is stable rather than re-derived every frame.
            targetAccent = EnsureContrastAgainstBackground(targetAccent, settings.pillBgColor);
        }

        // Smooth accent transition: exponential lerp toward target, ~300ms half-life.
        // On the very first frame (lastAccentTime_ < 0) snap immediately so there's
        // no fade-in from the default color on startup.
        if (lastAccentTime_ < 0.0) {
            currentAccent_ = targetAccent;
            lastAccentTime_ = now;
        } else {
            const double dt = std::max(0.0, std::min(now - lastAccentTime_, 0.1));  // cap at 100ms
            lastAccentTime_ = now;
            // k = 1 - exp(-dt / tau), tau ≈ 0.18s → reaches 95% in ~350ms
            const float k = 1.0f - static_cast<float>(std::exp(-dt / 0.18));
            currentAccent_.r += (targetAccent.r - currentAccent_.r) * k;
            currentAccent_.g += (targetAccent.g - currentAccent_.g) * k;
            currentAccent_.b += (targetAccent.b - currentAccent_.b) * k;
            currentAccent_.a = 1.0f;
        }

        const D2D1_COLOR_F accent = currentAccent_;

        if (!accentBrush_) target_->CreateSolidColorBrush(accent, &accentBrush_);
        else accentBrush_->SetColor(accent);

        if (!redBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.27f, 0.27f, 1.0f), &redBrush_);

        // Use user-configured text colors.
        D2D1_COLOR_F primary = settings.textPrimaryColor;
        primary.a = 0.98f;
        if (!textBrush_) target_->CreateSolidColorBrush(primary, &textBrush_);
        else textBrush_->SetColor(primary);

        D2D1_COLOR_F secondary = settings.textSecondaryColor;
        secondary.a = 0.90f;
        if (!mutedBrush_) target_->CreateSolidColorBrush(secondary, &mutedBrush_);
        else mutedBrush_->SetColor(secondary);

        // Keep the authored alpha. An 8-digit #RRGGBBAA background is how a
        // translucent island is requested independently of the global pill
        // transparency slider; DrawPillSurface combines the two.
        pillBgColor_ = settings.pillBgColor;

        D2D1_COLOR_F tintColor = D2D1::ColorF(0.010f, 0.010f, 0.012f, settings.tintOpacity);
        if (!tintBrush_) target_->CreateSolidColorBrush(tintColor, &tintBrush_);
        else tintBrush_->SetColor(tintColor);

        if (!shadowBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.70f), &shadowBrush_);

        BuildMaterialTokens(settings);
    }

    // ── Premium material system ─────────────────────────────────────────────
    // Every surface in the island is built from one small token set so the whole
    // UI shares a single visual language. Tokens are recomputed once per frame
    // from the resolved theme, and adapt to background luminance so a light
    // custom background gets dark separators instead of washed-out white ones.
    struct MaterialTokens {
        D2D1_COLOR_F base{};          // the pill's own fill
        D2D1_COLOR_F raised{};        // inner cards, chips, wells
        D2D1_COLOR_F raisedStrong{};  // pressed / active chips
        D2D1_COLOR_F hairline{};      // 1px separators between content
        D2D1_COLOR_F stroke{};        // outer contour
        D2D1_COLOR_F shadow{};        // drop shadow tint
        D2D1_COLOR_F textPrimary{};
        D2D1_COLOR_F textSecondary{};
        D2D1_COLOR_F textTertiary{};
        D2D1_COLOR_F accent{};
        D2D1_COLOR_F accentSoft{};
        bool onDark = true;
        float opacity = 1.0f;
    };

    static D2D1_COLOR_F MixColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t) {
        return D2D1::ColorF(a.r + (b.r - a.r) * t,
                            a.g + (b.g - a.g) * t,
                            a.b + (b.b - a.b) * t,
                            a.a + (b.a - a.a) * t);
    }

    static D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float alpha) {
        c.a = Clamp(alpha, 0.0f, 1.0f);
        return c;
    }

    void BuildMaterialTokens(const Settings& settings) {
        MaterialTokens t;
        t.opacity = settingsOpacity_;
        t.base = pillBgColor_;

        // Light surfaces need dark scrims/edges, dark surfaces need light ones.
        const bool onDark = RelativeLuminance(t.base) < 0.45;
        t.onDark = onDark;

        const D2D1_COLOR_F white = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
        const D2D1_COLOR_F black = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
        const D2D1_COLOR_F lift = onDark ? white : black;

        // Cards carry a slightly stronger fill than before. They used to be
        // outlined with a hairline ring, and losing that ring is what the fill
        // now has to compensate for so a card still reads as a distinct surface.
        t.raised = WithAlpha(lift, onDark ? 0.085f : 0.062f);
        t.raisedStrong = WithAlpha(lift, onDark ? 0.150f : 0.105f);
        t.hairline = WithAlpha(lift, onDark ? 0.100f : 0.085f);
        t.shadow = WithAlpha(black, 0.55f);

        t.stroke = settings.contourBorderColor;
        t.accent = currentAccent_;
        t.accentSoft = WithAlpha(currentAccent_, 0.18f);

        t.textPrimary = WithAlpha(settings.textPrimaryColor, 0.98f);
        t.textSecondary = WithAlpha(settings.textSecondaryColor, 0.88f);
        // Derived rather than configured, so a third tier always reads as
        // quieter than "secondary" whatever the user picked.
        t.textTertiary = WithAlpha(MixColor(settings.textSecondaryColor, t.base, 0.35f), 0.72f);

        material_ = t;
    }

    void DrawPill(const SharedState& state, const Settings& settings, const Activity& activity,
                  D2D1_RECT_F rect, float scale, double now) {
        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float w = (rect.right - rect.left) * scale;
        const float h = (rect.bottom - rect.top) * scale;
        rect = D2D1::RectF(cx - w * 0.5f, cy - h * 0.5f, cx + w * 0.5f, cy + h * 0.5f);

        const float visualScale = settings.sizeScale * settings.textScale;
        const float height = rect.bottom - rect.top;
        const bool expandedGeometry = height > settings.islandHeight * settings.sizeScale * 1.2f;
        const float radius = settings.notchStyle
            ? std::min(height * 0.5f, 16.0f * settings.sizeScale)
            : settings.w11Style
                ? (expandedGeometry
                    ? std::min(height * 0.22f, settings.cornerRadius * visualScale)
                    : std::min(height * 0.22f, 10.0f * settings.sizeScale))
                : expandedGeometry
                    ? std::min(settings.cornerRadius * visualScale, height * 0.5f)
                    : height * 0.5f;
        DrawSoftShadow(rect, radius);

        DrawPillSurface(rect, radius, activity.kind, settings);
        if (activity.kind == IslandKind::Progress) {
            DrawProgressRing(rect, state.progress.percent);
        }

        if (activity.kind == IslandKind::BatteryLow) {
            const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 2.0 * 3.14159265 * 2.1));
            redBrush_->SetOpacity(0.45f + 0.45f * pulse);
            DrawIslandShape(rect, radius, settings.w11Style, settings.notchStyle, redBrush_.Get(), 2.0f);
            redBrush_->SetOpacity(1.0f);
        }

        // No inner highlight ring here. A second bright hairline just inside the
        // contour read as a light rim tracing the whole island, which is exactly
        // the edge glow this design drops. The contour stroke alone defines the
        // silhouette now.

        D2D1_MATRIX_3X2_F oldTransform;
        target_->GetTransform(&oldTransform);
        D2D1_POINT_2F pillCenter = D2D1::Point2F((rect.left + rect.right) * 0.5f, (rect.top + rect.bottom) * 0.5f);
        target_->SetTransform(D2D1::Matrix3x2F::Scale(visualScale, visualScale, pillCenter) * oldTransform);

        float invScale = 1.0f / visualScale;
        float unW = (rect.right - rect.left) * invScale;
        float unH = (rect.bottom - rect.top) * invScale;
        D2D1_RECT_F unscaledRect = D2D1::RectF(pillCenter.x - unW * 0.5f, pillCenter.y - unH * 0.5f, pillCenter.x + unW * 0.5f, pillCenter.y + unH * 0.5f);

        switch (activity.kind) {
            case IslandKind::Media:
                DrawMedia(state, unscaledRect, settings, now);
                break;
            case IslandKind::Clipboard:
                DrawClipboard(state, unscaledRect);
                break;
            case IslandKind::Notification:
                DrawNotification(state, unscaledRect);
                break;
            case IslandKind::Volume:
                DrawVolume(state, unscaledRect);
                break;
            case IslandKind::CapsLock:
                DrawCapsLock(state, unscaledRect);
                break;
            case IslandKind::Device:
                DrawDevice(state, unscaledRect);
                break;
            case IslandKind::Bluetooth:
                DrawBluetoothDevice(state, unscaledRect);
                break;
            case IslandKind::Timer:
                DrawTimer(state, unscaledRect);
                break;
            case IslandKind::BatteryLow:
                DrawBattery(state, unscaledRect);
                break;
            case IslandKind::Progress:
                DrawProgress(state, unscaledRect);
                break;
            case IslandKind::DoNotDisturb:
                DrawDoNotDisturb(state, unscaledRect);
                break;
            case IslandKind::Idle:
            default:
                DrawIdleDashboard(state, unscaledRect, settings, now);
                break;
        }

        // ── Apple-style privacy indicator dots ───────────────────────────────
        // Green dot = camera in use, Orange dot = mic in use.
        // Drawn in top-right corner of pill, outside content area.
        DrawPrivacyDots(state, settings, unscaledRect, now);

        target_->SetTransform(oldTransform);
    }

    // A real soft shadow, approximated by stacking concentric rounded shapes
    // with a quadratic alpha falloff. There is no GPU device here (this renders
    // to a DC-bound target for UpdateLayeredWindow), so a Gaussian blur effect
    // is not available -- but the render padding around the pill leaves room to
    // fake it convincingly and cheaply.
    void DrawSoftShadow(D2D1_RECT_F rect, float radius) {
        // A backdrop material clips the window to the island's silhouette, so
        // anything drawn out in the padding would be cut off anyway.
        if (!g_settings.dropShadow || g_settings.backdropMaterial != BackdropMaterial::None) {
            return;
        }

        const float spread = Clamp(14.0f * g_settings.sizeScale, 6.0f, kRenderPadY - 4.0f);
        const float yOffset = spread * 0.35f;
        constexpr int kSteps = 7;

        ComPtr<ID2D1SolidColorBrush> brush;
        if (FAILED(target_->CreateSolidColorBrush(material_.shadow, &brush)) || !brush) {
            return;
        }

        for (int i = kSteps; i >= 1; --i) {
            const float t = static_cast<float>(i) / static_cast<float>(kSteps);
            const float grow = spread * t;
            // Quadratic falloff keeps the core dense and the outer edge feathered.
            const float alpha = material_.shadow.a * (1.0f - t) * (1.0f - t) * 0.55f * settingsOpacity_;
            if (alpha <= 0.002f) {
                continue;
            }
            brush->SetOpacity(alpha);

            const D2D1_RECT_F shadowRect = D2D1::RectF(
                rect.left - grow, rect.top - grow * 0.55f + yOffset,
                rect.right + grow, rect.bottom + grow + yOffset);
            FillIslandShape(shadowRect, radius + grow, g_settings.w11Style,
                            g_settings.notchStyle, brush.Get());
        }
        brush->SetOpacity(1.0f);
    }

    // Vertical depth shading. This used to open with a white sheen across the
    // top, which on a dark island read as a lit strip along the upper edge --
    // the same edge-glow look the rest of this design removes. What is left is
    // a single downward shade that grounds the surface without lighting any
    // edge: neutral at the top, gradually deeper toward the bottom.
    void FillSurfaceDepth(D2D1_RECT_F rect, float radius, bool strong) {
        const float h = rect.bottom - rect.top;
        if (h <= 2.0f) {
            return;
        }

        // Light backgrounds need less of it: the same alpha over near-white
        // turns into a visible grey wash rather than subtle depth.
        const float botA = (strong ? 0.085f : 0.060f) *
                           (material_.onDark ? 1.0f : 0.55f) * settingsOpacity_;

        D2D1_GRADIENT_STOP stops[3] = {};
        stops[0].position = 0.0f;
        stops[0].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
        stops[1].position = 0.45f;
        stops[1].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, botA * 0.25f);
        stops[2].position = 1.0f;
        stops[2].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, botA);

        ComPtr<ID2D1GradientStopCollection> collection;
        if (FAILED(target_->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2,
                                                         D2D1_EXTEND_MODE_CLAMP, &collection)) ||
            !collection) {
            return;
        }

        ComPtr<ID2D1LinearGradientBrush> brush;
        if (FAILED(target_->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(rect.left, rect.top),
                    D2D1::Point2F(rect.left, rect.bottom)),
                collection.Get(), &brush)) ||
            !brush) {
            return;
        }
        FillIslandShape(rect, radius, g_settings.w11Style, g_settings.notchStyle, brush.Get());
    }

    // A wide, very soft accent wash bled in from the top of the surface. Driven
    // by the album-art accent, this is what makes the media surface feel alive
    // without tinting the whole pill.
    void FillAccentBloom(D2D1_RECT_F rect, float radius, float strength) {
        if (strength <= 0.01f) {
            return;
        }

        const float w = rect.right - rect.left;
        const float h = rect.bottom - rect.top;
        if (w <= 2.0f || h <= 2.0f) {
            return;
        }

        D2D1_GRADIENT_STOP stops[2] = {};
        stops[0].position = 0.0f;
        stops[0].color = WithAlpha(material_.accent, strength * settingsOpacity_);
        stops[1].position = 1.0f;
        stops[1].color = WithAlpha(material_.accent, 0.0f);

        ComPtr<ID2D1GradientStopCollection> collection;
        if (FAILED(target_->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2,
                                                         D2D1_EXTEND_MODE_CLAMP, &collection)) ||
            !collection) {
            return;
        }

        const D2D1_POINT_2F center = D2D1::Point2F((rect.left + rect.right) * 0.5f, rect.top);
        ComPtr<ID2D1RadialGradientBrush> brush;
        if (FAILED(target_->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(center, D2D1::Point2F(0, 0), w * 0.75f, h * 1.15f),
                collection.Get(), &brush)) ||
            !brush) {
            return;
        }
        FillIslandShape(rect, radius, g_settings.w11Style, g_settings.notchStyle, brush.Get());
    }

    // Rounded inner card used by every dashboard, so panels across the media,
    // calendar, weather, hardware and file-tray surfaces match exactly.
    //
    // Fill only, no outline. Each card used to also get a hairline ring, and six
    // of those side by side in the hardware grid turned into a mesh of bright
    // edges competing with the content. The fill alpha in BuildMaterialTokens
    // was raised to carry the separation on its own.
    void DrawCard(D2D1_RECT_F rect, float radius, bool active = false) {
        ComPtr<ID2D1SolidColorBrush> fill;
        if (SUCCEEDED(target_->CreateSolidColorBrush(
                active ? material_.raisedStrong : material_.raised, &fill)) && fill) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), fill.Get());
        }
    }

    // Accent-filled progress track shared by the scrubber, volume, battery and
    // timer, so "progress" looks the same everywhere.
    void DrawAccentTrack(D2D1_RECT_F track, float progress, float radius) {
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        if (SUCCEEDED(target_->CreateSolidColorBrush(material_.raisedStrong, &trackBrush)) && trackBrush) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(track, radius, radius), trackBrush.Get());
        }

        const float span = (track.right - track.left) * Clamp(progress, 0.0f, 1.0f);
        if (span <= 0.5f) {
            return;
        }

        const D2D1_RECT_F fillRect = D2D1::RectF(track.left, track.top, track.left + span, track.bottom);

        D2D1_GRADIENT_STOP stops[2] = {};
        stops[0].position = 0.0f;
        stops[0].color = WithAlpha(MixColor(material_.accent, D2D1::ColorF(1, 1, 1, 1), 0.30f), 0.95f);
        stops[1].position = 1.0f;
        stops[1].color = WithAlpha(material_.accent, 1.0f);

        ComPtr<ID2D1GradientStopCollection> collection;
        ComPtr<ID2D1LinearGradientBrush> grad;
        if (SUCCEEDED(target_->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2,
                                                            D2D1_EXTEND_MODE_CLAMP, &collection)) &&
            collection &&
            SUCCEEDED(target_->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(fillRect.left, fillRect.top),
                                                    D2D1::Point2F(fillRect.right, fillRect.top)),
                collection.Get(), &grad)) &&
            grad) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, radius, radius), grad.Get());
        }
    }

    void DrawPrivacyDots(const SharedState& state, const Settings& settings, D2D1_RECT_F rect, double now) {
        UNREFERENCED_PARAMETER(now);
        const float height = rect.bottom - rect.top;
        if (height > 55.0f) return;

        const bool mic = state.system.micActive && settings.privacyDots && settings.privacyDotsMic;
        const bool cam = state.system.cameraActive && settings.privacyDots && settings.privacyDotsCam;
        if (!mic && !cam) return;

        const float dotR   = 4.0f;
        const float margin = 16.0f;
        const float dotY   = rect.top + (rect.bottom - rect.top) * 0.5f;

        const float x = rect.right - margin - dotR;

        if (cam) {
            D2D1_COLOR_F camColor = settings.privacyDotsCamHex;
            camColor.a = settingsOpacity_;
            if (!camDotBrush_) target_->CreateSolidColorBrush(camColor, &camDotBrush_);
            else camDotBrush_->SetColor(camColor);

            if (camDotBrush_) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY), dotR, dotR), camDotBrush_.Get());
            }
        } else if (mic) {
            D2D1_COLOR_F micColor = settings.privacyDotsMicHex;
            micColor.a = settingsOpacity_;
            if (!micDotBrush_) target_->CreateSolidColorBrush(micColor, &micDotBrush_);
            else micDotBrush_->SetColor(micColor);

            if (micDotBrush_) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY), dotR, dotR), micDotBrush_.Get());
            }
        }
    }

    ComPtr<ID2D1PathGeometry> CreateNotchGeometry(D2D1_RECT_F rect, float radius) {
        ComPtr<ID2D1PathGeometry> geom;
        if (FAILED(d2dFactory_->CreatePathGeometry(&geom))) return nullptr;

        ComPtr<ID2D1GeometrySink> sink;
        if (FAILED(geom->Open(&sink))) return nullptr;

        float r = std::min({radius, (rect.right - rect.left) * 0.5f, (rect.bottom - rect.top) * 0.5f});
        if (r < 0.0f) r = 0.0f;

        sink->BeginFigure(D2D1::Point2F(rect.left, rect.top), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.right, rect.top));
        sink->AddLine(D2D1::Point2F(rect.right, rect.bottom - r));
        if (r > 0.0f) {
            sink->AddArc(D2D1::ArcSegment(
                D2D1::Point2F(rect.right - r, rect.bottom),
                D2D1::SizeF(r, r), 0.0f,
                D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            sink->AddLine(D2D1::Point2F(rect.left + r, rect.bottom));
            sink->AddArc(D2D1::ArcSegment(
                D2D1::Point2F(rect.left, rect.bottom - r),
                D2D1::SizeF(r, r), 0.0f,
                D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        } else {
            sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        return geom;
    }

    ComPtr<ID2D1Geometry> CreateIslandMaskGeometry(D2D1_RECT_F rect, float radius, bool notchStyle) {
        if (notchStyle) {
            ComPtr<ID2D1PathGeometry> geom = CreateNotchGeometry(rect, radius);
            if (geom) {
                ComPtr<ID2D1Geometry> baseGeom;
                geom.As(&baseGeom);
                return baseGeom;
            }
        }
        ComPtr<ID2D1RoundedRectangleGeometry> rr;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(rect, radius, radius), &rr);
        ComPtr<ID2D1Geometry> baseGeom;
        if (rr) rr.As(&baseGeom);
        return baseGeom;
    }

    void FillIslandShape(D2D1_RECT_F rect, float radius, bool w11Style, bool notchStyle, ID2D1Brush* brush) {
        if (!brush) return;
        if (notchStyle) {
            auto geom = CreateNotchGeometry(rect, radius);
            if (geom) {
                target_->FillGeometry(geom.Get(), brush);
                return;
            }
        }
        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), brush);
    }

    void DrawIslandShape(D2D1_RECT_F rect, float radius, bool w11Style, bool notchStyle, ID2D1Brush* brush, float strokeWidth) {
        if (!brush) return;
        if (notchStyle) {
            auto geom = CreateNotchGeometry(rect, radius);
            if (geom) {
                target_->DrawGeometry(geom.Get(), brush, strokeWidth);
                return;
            }
        }
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), brush, strokeWidth);
    }

    // The island's material, composed bottom-up:
    //   tint scrim -> base fill -> depth shading -> accent bloom -> contour
    // Each layer is individually subtle; together they give the pill depth
    // instead of the flat single-fill look it had before. Deliberately absent:
    // any bright rim, hairline or sheen tracing the island's edge.
    void DrawPillSurface(D2D1_RECT_F rect, float radius, IslandKind kind, const Settings& settings) {
        // The dark tint scrim exists to deepen an opaque background. With a real
        // backdrop enabled it would just mud up the blur, so it is skipped.
        if (tintBrush_ && settings.backdropMaterial == BackdropMaterial::None) {
            FillIslandShape(rect, radius, settings.w11Style, settings.notchStyle, tintBrush_.Get());
        }

        // User-defined pill background color. The authored alpha is combined
        // with the global pill transparency, so a plain 6-digit hex behaves
        // exactly as before (alpha 1.0) while #RRGGBBAA stays translucent.
        ComPtr<ID2D1SolidColorBrush> blackBrush;
        D2D1_COLOR_F bg = pillBgColor_;
        bg.a = Clamp(bg.a * settingsOpacity_, 0.0f, 1.0f);
        if (settings.backdropMaterial != BackdropMaterial::None) {
            // DWM is blurring what is behind the window; an opaque fill would
            // hide it entirely, so cap the fill and let the backdrop through.
            bg.a = std::min(bg.a, settings.backdropFillAlpha);
        }
        target_->CreateSolidColorBrush(bg, &blackBrush);
        if (blackBrush) {
            FillIslandShape(rect, radius, settings.w11Style, settings.notchStyle, blackBrush.Get());
        }

        if (settings.materialDepth) {
            FillSurfaceDepth(rect, radius, kind == IslandKind::Media || kind == IslandKind::Idle);

            // Media leans on the album-art accent; other surfaces get a whisper
            // of it so the whole UI still feels connected to what is playing.
            const float bloom = (kind == IslandKind::Media) ? 0.115f
                                : (kind == IslandKind::Idle) ? 0.055f
                                                             : 0.075f;
            FillAccentBloom(rect, radius, bloom * settings.accentBloom);
        }

        if (settings.contourBorderMode != ContourBorderMode::Borderless && settings.contourBorderEnabled) {
            D2D1_COLOR_F borderColor = settings.contourBorderColor;
            float strokeWidth = settings.themePreset == ThemePreset::LiquidGlass ? 1.3f
                              : settings.w11Style ? 1.0f : 0.8f;

            if (settings.contourBorderMode == ContourBorderMode::Auto) {
                if (currentAccent_.a > 0.0f) {
                    borderColor = currentAccent_;
                    borderColor.a = std::min(1.0f, (kind == IslandKind::Idle ? 0.35f : 0.60f) * settingsOpacity_);
                    strokeWidth = 1.0f;
                } else {
                    borderColor.a = std::min(1.0f, borderColor.a * settingsOpacity_);
                }
            } else {
                borderColor.a = std::min(1.0f, borderColor.a * settingsOpacity_);
            }

            ComPtr<ID2D1SolidColorBrush> border;
            target_->CreateSolidColorBrush(borderColor, &border);
            if (border) {
                D2D1_RECT_F borderRect = D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f,
                                                     rect.right - 0.5f, rect.bottom - 0.5f);
                DrawIslandShape(borderRect, radius, settings.w11Style, settings.notchStyle, border.Get(), strokeWidth);
            }
        }

        if (settings.themePreset == ThemePreset::LiquidGlass) {
            // A restrained upper-surface reflection gives the acrylic fill a
            // glass cue without tracing the whole island with a bright rim.
            const float sheenBottom = rect.top + (rect.bottom - rect.top) * 0.46f;
            const D2D1_RECT_F sheenClip = D2D1::RectF(rect.left, rect.top,
                                                      rect.right, sheenBottom);
            ComPtr<ID2D1SolidColorBrush> sheen;
            if (SUCCEEDED(target_->CreateSolidColorBrush(
                    D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.055f), &sheen)) && sheen) {
                target_->PushAxisAlignedClip(sheenClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                FillIslandShape(rect, radius, settings.w11Style, settings.notchStyle,
                                sheen.Get());
                target_->PopAxisAlignedClip();
            }
        }
    }

    void DrawAccentGlow(D2D1_RECT_F rect, const Activity& activity, double now) {
        float opacity = activity.kind == IslandKind::Media ? 0.23f : 0.12f;
        if (activity.kind == IslandKind::BatteryLow) {
            redBrush_->SetOpacity(0.18f);
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.right - 38, rect.top + 20), 56, 36),
                                 redBrush_.Get());
            redBrush_->SetOpacity(1.0f);
            return;
        }

        opacity += 0.05f * (0.5f + 0.5f * std::sin(static_cast<float>(now * 1.7)));
        accentBrush_->SetOpacity(opacity);
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.left + 48, rect.top + 10), 70, 42),
                             accentBrush_.Get());
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.right - 58, rect.bottom - 8), 76, 42),
                             accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
    }

    static void GetWeatherIconAndText(int code, std::wstring& icon, std::wstring& text) {
        switch (code) {
            case 113: icon = L"☀️"; break;
            case 116: icon = L"⛅"; break;
            case 119: case 122: icon = L"☁️"; break;
            case 143: case 248: case 260: icon = L"🌫️"; break;
            case 200: case 386: case 389: case 392: case 395: icon = L"⛈️"; break;
            case 176: case 263: case 266: case 281: case 284: case 293: case 296: case 299: case 302: case 305: case 308: case 311: case 314: case 353: case 356: case 359: icon = L"🌧️"; break;
            case 179: case 182: case 185: case 227: case 230: case 317: case 320: case 323: case 326: case 329: case 332: case 335: case 338: case 350: case 362: case 365: case 368: case 371: icon = L"❄️"; break;
            default: icon = L"🌡️"; break;
        }
    }

    static int GetDaysInMonth(int year, int month) {
        if (month == 2) {
            bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            return leap ? 29 : 28;
        }
        if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
        return 31;
    }

    static int GetDayOfWeek(int year, int month, int day) {
        if (month < 3) { month += 12; year -= 1; }
        int k = year % 100;
        int j = year / 100;
        int h = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
        return (h + 6) % 7;
    }



    void DrawCalendarDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, SYSTEMTIME& local) {
        (void)state;
        (void)now;

        const SYSTEMTIME today = local;
        InitializeCalendarSelection(today);
        const int viewYear = ClampInt(g_calendarViewYear.load(std::memory_order_relaxed), 1601, 9999);
        const int viewMonth = ClampInt(g_calendarViewMonth.load(std::memory_order_relaxed), 1, 12);
        const int selectedDay = ClampInt(g_calendarSelectedDay.load(std::memory_order_relaxed),
                                         1, GetDaysInMonth(viewYear, viewMonth));
        local.wYear = static_cast<WORD>(viewYear);
        local.wMonth = static_cast<WORD>(viewMonth);
        local.wDay = static_cast<WORD>(selectedDay);
        local.wDayOfWeek = static_cast<WORD>(GetDayOfWeek(viewYear, viewMonth, selectedDay));

        // ── Hero column ──────────────────────────────────────────────────────
        // Today's date as an editorial block: month over a large day figure over
        // the weekday. Uses the shared DrawCard so it matches every other panel
        // instead of being a one-off translucent slab.
        const D2D1_RECT_F hero = D2D1::RectF(rect.left + 22.0f * scale, rect.top + 16.0f * scale,
                                             rect.left + 115.0f * scale, rect.bottom - 20.0f * scale);
        DrawCard(hero, 12.0f * scale);

        // The accent is the album-art / system accent like everywhere else. This
        // used to fall back to a hardcoded red (#D94A38) whenever the mode wasn't
        // "System", which was the one colour in the whole island that answered to
        // nothing -- it clashed with the accent on every other surface.
        D2D1_COLOR_F accentColor = (settings.calendarAccent == CalendarAccentMode::System)
            ? GetSystemAccentColor()
            : material_.accent;
        accentColor.a = 0.95f * settingsOpacity_;

        ComPtr<ID2D1SolidColorBrush> accent;
        target_->CreateSolidColorBrush(accentColor, &accent);

        if (calendarCachedDate_.wYear != local.wYear || calendarCachedDate_.wMonth != local.wMonth ||
            calendarCachedDate_.wDay != local.wDay) {
            wchar_t monthNameBuf[32] = {};
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"MMMM", monthNameBuf, ARRAYSIZE(monthNameBuf), nullptr);
            // No longer uppercased. towupper is per-character and locale-blind:
            // it turns Turkish "i" into "I" rather than "İ", and does nothing at
            // all for CJK month names, so the effect was inconsistent by locale.
            calendarCachedMonthName_ = monthNameBuf;

            wchar_t weekdayNameBuf[32] = {};
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"dddd", weekdayNameBuf, ARRAYSIZE(weekdayNameBuf), nullptr);
            calendarCachedWeekdayName_ = weekdayNameBuf;

            calendarCachedDate_ = local;
        }

        // Month and year stay on separate lines. Putting them on one ("September
        // 2026") needs about 95px at 12px bold and the hero column only offers
        // 85, so any long month name clipped -- and month names are exactly the
        // strings that get long once localized.
        if (boldTextFormat_) {
            target_->DrawTextW(calendarCachedMonthName_.c_str(),
                               static_cast<UINT32>(calendarCachedMonthName_.size()),
                               boldTextFormat_.Get(),
                               D2D1::RectF(hero.left + 3.0f * scale, hero.top + 6.0f * scale,
                                           hero.right - 3.0f * scale, hero.top + 22.0f * scale),
                               accent.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        wchar_t yearStr[16] = {};
        swprintf_s(yearStr, L"%d", local.wYear);
        mutedBrush_->SetOpacity(0.62f);
        if (smallTextFormat_) {
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(yearStr, static_cast<UINT32>(wcslen(yearStr)), smallTextFormat_.Get(),
                               D2D1::RectF(hero.left, hero.top + 21.0f * scale,
                                           hero.right, hero.top + 36.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        wchar_t dayStr[16] = {};
        swprintf_s(dayStr, L"%d", local.wDay);
        textBrush_->SetOpacity(0.97f);
        target_->DrawTextW(dayStr, static_cast<UINT32>(wcslen(dayStr)),
                           calDayLargeFormat_ ? calDayLargeFormat_.Get() : hugeTextFormat_.Get(),
                           D2D1::RectF(hero.left, hero.top + 34.0f * scale,
                                       hero.right, hero.bottom - 26.0f * scale),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        mutedBrush_->SetOpacity(0.70f);
        if (boldTextFormat_) {
            target_->DrawTextW(calendarCachedWeekdayName_.c_str(),
                               static_cast<UINT32>(calendarCachedWeekdayName_.size()),
                               boldTextFormat_.Get(),
                               D2D1::RectF(hero.left + 4.0f * scale, hero.bottom - 24.0f * scale,
                                           hero.right - 4.0f * scale, hero.bottom - 6.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        // ── Month grid ───────────────────────────────────────────────────────
        const float gridStart = rect.left + CalendarLayout::kGridLeft * scale;
        const float gridRight = rect.right - CalendarLayout::kGridRightInset * scale;
        const float headerTop = rect.top + CalendarLayout::kHeaderTop * scale;
        const float headerBottom = rect.top + CalendarLayout::kHeaderBottom * scale;
        const float gridTop = rect.top + CalendarLayout::kGridTop * scale;
        const float colW = (gridRight - gridStart) / 7.0f;
        const float headerH = CalendarLayout::kWeekdayHeight * scale;
        const wchar_t* days[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};

        const int pickerMode = g_calendarPickerMode.load(std::memory_order_relaxed);
        const float navButtonWidth = 24.0f * scale;
        const D2D1_RECT_F prevButton = D2D1::RectF(
            gridStart, headerTop, gridStart + navButtonWidth, headerBottom);
        const D2D1_RECT_F nextButton = D2D1::RectF(
            gridRight - navButtonWidth, headerTop, gridRight, headerBottom);
        ComPtr<ID2D1SolidColorBrush> navSurface;
        target_->CreateSolidColorBrush(WithAlpha(material_.raisedStrong, 0.98f * settingsOpacity_), &navSurface);
        ComPtr<ID2D1SolidColorBrush> headerInk;
        target_->CreateSolidColorBrush(EnsureContrastAgainstBackground(
            material_.textPrimary, material_.raisedStrong), &headerInk);
        if (navSurface) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(prevButton, 7.0f * scale, 7.0f * scale), navSurface.Get());
            target_->FillRoundedRectangle(D2D1::RoundedRect(nextButton, 7.0f * scale, 7.0f * scale), navSurface.Get());
            target_->DrawRoundedRectangle(D2D1::RoundedRect(prevButton, 7.0f * scale, 7.0f * scale), accentBrush_.Get(), 0.9f * scale);
            target_->DrawRoundedRectangle(D2D1::RoundedRect(nextButton, 7.0f * scale, 7.0f * scale), accentBrush_.Get(), 0.9f * scale);
        }
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(L"\uE76B", 1, iconFormat_.Get(), prevButton, headerInk.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            target_->DrawTextW(L"\uE76C", 1, iconFormat_.Get(), nextButton, headerInk.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        const float monthTextLeft = prevButton.right + 3.0f * scale;
        const float monthTextRight = gridStart + 132.0f * scale;
        const float yearTextLeft = monthTextRight + 1.0f * scale;
        const float yearTextRight = nextButton.left - 3.0f * scale;
        const int yearRangeStart = g_calendarYearRangeStart.load(std::memory_order_relaxed);
        std::wstring yearHeaderLabel = yearStr;
        if (pickerMode == 2 && yearRangeStart > 0) {
            yearHeaderLabel = std::to_wstring(yearRangeStart) + L"–" +
                              std::to_wstring(yearRangeStart + 11);
        }
        const D2D1_RECT_F monthHeaderButton = D2D1::RectF(
            monthTextLeft, headerTop, monthTextRight, headerBottom);
        const D2D1_RECT_F yearHeaderButton = D2D1::RectF(
            yearTextLeft, headerTop, yearTextRight, headerBottom);
        if (navSurface) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(monthHeaderButton, 7.0f * scale, 7.0f * scale), navSurface.Get());
            target_->FillRoundedRectangle(D2D1::RoundedRect(yearHeaderButton, 7.0f * scale, 7.0f * scale), navSurface.Get());
            target_->DrawRoundedRectangle(D2D1::RoundedRect(monthHeaderButton, 7.0f * scale, 7.0f * scale), accentBrush_.Get(), 0.9f * scale);
            target_->DrawRoundedRectangle(D2D1::RoundedRect(yearHeaderButton, 7.0f * scale, 7.0f * scale), accentBrush_.Get(), 0.9f * scale);
        }
        smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        target_->DrawTextW(calendarCachedMonthName_.c_str(),
                           static_cast<UINT32>(calendarCachedMonthName_.size()), textFormat_.Get(),
                           monthHeaderButton,
                           pickerMode == 1 ? accent.Get() : headerInk.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        target_->DrawTextW(yearHeaderLabel.c_str(), static_cast<UINT32>(yearHeaderLabel.size()), textFormat_.Get(),
                           yearHeaderButton,
                           pickerMode == 2 ? accent.Get() : headerInk.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        if (pickerMode != 0) {
            constexpr float pickerGap = 4.0f;
            const float pickerTop = gridTop + 8.0f * scale;
            const float pickerBottom = rect.bottom - CalendarLayout::kDateBottomInset * scale;
            const float cellW = (gridRight - gridStart - pickerGap * (CalendarLayout::kPickerColumns - 1) * scale) /
                                CalendarLayout::kPickerColumns;
            const float cellH = (pickerBottom - pickerTop - pickerGap * (CalendarLayout::kPickerRows - 1) * scale) /
                                CalendarLayout::kPickerRows;
            for (int i = 0; i < CalendarLayout::kPickerColumns * CalendarLayout::kPickerRows; ++i) {
                const int column = i % CalendarLayout::kPickerColumns;
                const int row = i / CalendarLayout::kPickerColumns;
                const float x = gridStart + column * (cellW + pickerGap * scale);
                const float y = pickerTop + row * (cellH + pickerGap * scale);
                const D2D1_RECT_F cell = D2D1::RectF(x, y, x + cellW, y + cellH);
                const bool selected = pickerMode == 1 ? (i + 1 == viewMonth)
                                                       : (yearRangeStart + i == viewYear);
                if (selected && accent) {
                    target_->FillRoundedRectangle(D2D1::RoundedRect(cell, 7.0f * scale, 7.0f * scale), accent.Get());
                } else if (navSurface) {
                    target_->FillRoundedRectangle(D2D1::RoundedRect(cell, 7.0f * scale, 7.0f * scale), navSurface.Get());
                }

                std::wstring label;
                if (pickerMode == 1) {
                    SYSTEMTIME monthDate = local;
                    monthDate.wMonth = static_cast<WORD>(i + 1);
                    monthDate.wDay = 1;
                    wchar_t monthBuf[32] = {};
                    if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &monthDate, L"MMM",
                                        monthBuf, ARRAYSIZE(monthBuf), nullptr) > 0) {
                        label = monthBuf;
                    } else {
                        label = std::to_wstring(i + 1);
                    }
                } else {
                    label = std::to_wstring(yearRangeStart + i);
                }
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()), smallTextFormat_.Get(),
                                   cell, selected ? textBrush_.Get() : mutedBrush_.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            return;
        }

        const int startDayIdx = GetDayOfWeek(local.wYear, local.wMonth, 1);
        const int monthDays = GetDaysInMonth(local.wYear, local.wMonth);
        const int rowCount = (startDayIdx + monthDays + 6) / 7;  // 5 or 6

        // Row height is derived from the space actually available rather than
        // fixed at 26px. At 26 a six-row month ran to y=197 inside a 184px
        // island, so the last row was silently clipped by the island mask --
        // visible every month that starts late in the week.
        const float datesTop = gridTop + headerH + 7.0f * scale;
        const float gridBottom = rect.bottom - CalendarLayout::kDateBottomInset * scale;
        const float rowH = (gridBottom - datesTop) / static_cast<float>(rowCount > 0 ? rowCount : 1);

        IDWriteTextFormat* gridFmt = calGridFormat_ ? calGridFormat_.Get() : boldTextFormat_.Get();

        // Weekday initials are uniformly quiet. They used to paint S and S in the
        // accent, which put three competing accent marks in the grid (both
        // weekend headers plus today) and made the headers look selected.
        mutedBrush_->SetOpacity(0.55f);
        for (int i = 0; i < 7; ++i) {
            const D2D1_RECT_F cell = D2D1::RectF(gridStart + i * colW, gridTop,
                                                 gridStart + (i + 1) * colW, gridTop + headerH);
            target_->DrawTextW(days[i], 1, gridFmt, cell, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        // Hairline under the weekday headers.
        const float hrY = gridTop + headerH + 3.0f * scale;
        ComPtr<ID2D1SolidColorBrush> hrBrush;
        if (SUCCEEDED(target_->CreateSolidColorBrush(
                WithAlpha(material_.hairline, material_.hairline.a * settingsOpacity_), &hrBrush)) && hrBrush) {
            target_->DrawLine(D2D1::Point2F(gridStart + 2.0f * scale, hrY),
                              D2D1::Point2F(gridStart + 7.0f * colW - 2.0f * scale, hrY),
                              hrBrush.Get(), 1.0f * scale);
        }

        // Today's marker is sized from the cell it has to live in, so it stays a
        // circle around the figure instead of a fixed 12px disc that swallowed
        // the digits once the rows got shorter.
        const float markerR = std::min(colW, rowH) * 0.5f - 1.5f * scale;

        int row = 0;
        int col = startDayIdx;
        for (int d = 1; d <= monthDays; ++d) {
            const D2D1_RECT_F cell = D2D1::RectF(gridStart + col * colW, datesTop + row * rowH,
                                                 gridStart + (col + 1) * colW, datesTop + (row + 1) * rowH);
            const std::wstring dayText = std::to_wstring(d);

            const bool selectedDate = d == local.wDay;
            const bool isToday = today.wYear == local.wYear &&
                                 today.wMonth == local.wMonth && today.wDay == d;
            if (selectedDate) {
                target_->FillEllipse(
                    D2D1::Ellipse(D2D1::Point2F(cell.left + colW * 0.5f, cell.top + rowH * 0.5f),
                                  markerR, markerR),
                    accent.Get());
                // Today's figure is drawn against the accent fill, so it needs the
                // primary text colour at full strength rather than the 0.85 the
                // other days use.
                textBrush_->SetOpacity(1.0f);
                target_->DrawTextW(dayText.c_str(), static_cast<UINT32>(dayText.size()), gridFmt,
                                   cell, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            } else if (isToday) {
                target_->DrawEllipse(
                    D2D1::Ellipse(D2D1::Point2F(cell.left + colW * 0.5f, cell.top + rowH * 0.5f),
                                  markerR, markerR),
                    accent.Get(), 1.2f * scale);
                target_->DrawTextW(dayText.c_str(), static_cast<UINT32>(dayText.size()), gridFmt,
                                   cell, accent.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            } else if (col == 0 || col == 6) {
                // Weekends recede instead of taking the accent, leaving today as
                // the only accented thing in the grid.
                mutedBrush_->SetOpacity(0.62f);
                target_->DrawTextW(dayText.c_str(), static_cast<UINT32>(dayText.size()), gridFmt,
                                   cell, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            } else {
                textBrush_->SetOpacity(0.88f);
                target_->DrawTextW(dayText.c_str(), static_cast<UINT32>(dayText.size()), gridFmt,
                                   cell, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }

            if (++col > 6) { col = 0; ++row; }
        }

        textBrush_->SetOpacity(0.96f);
        mutedBrush_->SetOpacity(0.75f);
    }

    // Fixes windhawk-mods#4352: wind direction was printed as compass
    // initialisms (WSW, NE), which is meteorologist shorthand rather than
    // something glanceable. These are flow arrows, not bearing arrows -- a wind
    // *from* the north-east is drawn as an arrow pointing south-west, matching
    // the convention weather apps use.
    std::wstring WindDirToArrow(const std::wstring& dir) {
        if (dir == L"N") return L"\x2193";
        if (dir == L"NNE" || dir == L"NE" || dir == L"ENE") return L"\x2199";
        if (dir == L"E") return L"\x2190";
        if (dir == L"ESE" || dir == L"SE" || dir == L"SSE") return L"\x2196";
        if (dir == L"S") return L"\x2191";
        if (dir == L"SSW" || dir == L"SW" || dir == L"WSW") return L"\x2197";
        if (dir == L"W") return L"\x2192";
        if (dir == L"WNW" || dir == L"NW" || dir == L"NNW") return L"\x2198";
        return dir;
    }

    // ── Vector weather icons ────────────────────────────────────────────────
    // All sized relative to `s` (the icon's box width) and centred on `c`, so a
    // single call site controls scale. Built from ellipses and rounded rects
    // rather than path geometry wherever possible, to keep per-frame allocation
    // down.

    void DrawCloudShape(D2D1_POINT_2F c, float s, ID2D1Brush* brush) {
        const D2D1_RECT_F base = D2D1::RectF(c.x - 0.44f * s, c.y + 0.02f * s,
                                             c.x + 0.44f * s, c.y + 0.21f * s);
        target_->FillRoundedRectangle(D2D1::RoundedRect(base, 0.10f * s, 0.10f * s), brush);
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x - 0.24f * s, c.y + 0.02f * s), 0.18f * s, 0.18f * s), brush);
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x - 0.01f * s, c.y - 0.10f * s), 0.24f * s, 0.24f * s), brush);
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x + 0.25f * s, c.y + 0.03f * s), 0.17f * s, 0.17f * s), brush);
    }

    void DrawSunShape(D2D1_POINT_2F c, float s, ID2D1Brush* brush, float coreRadius = 0.26f) {
        target_->FillEllipse(D2D1::Ellipse(c, coreRadius * s, coreRadius * s), brush);

        const float inner = (coreRadius + 0.09f) * s;
        const float outer = (coreRadius + 0.21f) * s;
        for (int i = 0; i < 8; ++i) {
            const float a = static_cast<float>(i) * 3.14159265f / 4.0f;
            const float ca = std::cos(a);
            const float sa = std::sin(a);
            target_->DrawLine(D2D1::Point2F(c.x + ca * inner, c.y + sa * inner),
                              D2D1::Point2F(c.x + ca * outer, c.y + sa * outer),
                              brush, std::max(1.2f, 0.055f * s));
        }
    }

    void DrawLightningShape(D2D1_POINT_2F c, float s, ID2D1Brush* brush) {
        ComPtr<ID2D1PathGeometry> bolt;
        if (FAILED(d2dFactory_->CreatePathGeometry(&bolt)) || !bolt) {
            return;
        }
        ComPtr<ID2D1GeometrySink> sink;
        if (FAILED(bolt->Open(&sink)) || !sink) {
            return;
        }
        sink->BeginFigure(D2D1::Point2F(c.x + 0.07f * s, c.y + 0.16f * s), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(c.x - 0.11f * s, c.y + 0.44f * s));
        sink->AddLine(D2D1::Point2F(c.x + 0.00f * s, c.y + 0.44f * s));
        sink->AddLine(D2D1::Point2F(c.x - 0.06f * s, c.y + 0.66f * s));
        sink->AddLine(D2D1::Point2F(c.x + 0.15f * s, c.y + 0.36f * s));
        sink->AddLine(D2D1::Point2F(c.x + 0.03f * s, c.y + 0.36f * s));
        sink->AddLine(D2D1::Point2F(c.x + 0.12f * s, c.y + 0.16f * s));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        target_->FillGeometry(bolt.Get(), brush);
    }

    void DrawWeatherIcon(D2D1_POINT_2F center, float size, WeatherVisual visual,
                         ID2D1Brush* strong, ID2D1Brush* soft) {
        const float s = size;
        const float stroke = std::max(1.3f, 0.06f * s);

        switch (visual) {
            case WeatherVisual::Clear:
                DrawSunShape(center, s, strong, 0.28f);
                break;

            case WeatherVisual::PartlyCloudy: {
                // Sun peeking out behind the cloud's upper-left.
                DrawSunShape(D2D1::Point2F(center.x - 0.20f * s, center.y - 0.20f * s), s * 0.62f, soft, 0.30f);
                DrawCloudShape(D2D1::Point2F(center.x + 0.05f * s, center.y + 0.06f * s), s * 0.92f, strong);
                break;
            }

            case WeatherVisual::Cloudy:
                DrawCloudShape(D2D1::Point2F(center.x, center.y - 0.06f * s), s, soft);
                DrawCloudShape(D2D1::Point2F(center.x + 0.04f * s, center.y + 0.06f * s), s * 0.86f, strong);
                break;

            case WeatherVisual::Fog: {
                DrawCloudShape(D2D1::Point2F(center.x, center.y - 0.16f * s), s * 0.92f, strong);
                for (int i = 0; i < 3; ++i) {
                    const float y = center.y + (0.24f + 0.15f * static_cast<float>(i)) * s;
                    const float half = (0.34f - 0.05f * static_cast<float>(i)) * s;
                    target_->DrawLine(D2D1::Point2F(center.x - half, y),
                                      D2D1::Point2F(center.x + half, y), soft, stroke);
                }
                break;
            }

            case WeatherVisual::Storm:
                DrawCloudShape(D2D1::Point2F(center.x, center.y - 0.20f * s), s * 0.92f, soft);
                DrawLightningShape(center, s, strong);
                break;

            case WeatherVisual::Rain: {
                DrawCloudShape(D2D1::Point2F(center.x, center.y - 0.18f * s), s * 0.92f, strong);
                for (int i = 0; i < 3; ++i) {
                    const float x = center.x + (-0.22f + 0.22f * static_cast<float>(i)) * s;
                    const float y = center.y + 0.24f * s;
                    target_->DrawLine(D2D1::Point2F(x + 0.05f * s, y),
                                      D2D1::Point2F(x - 0.03f * s, y + 0.26f * s), soft, stroke);
                }
                break;
            }

            case WeatherVisual::Snow: {
                DrawCloudShape(D2D1::Point2F(center.x, center.y - 0.18f * s), s * 0.92f, strong);
                for (int i = 0; i < 3; ++i) {
                    const D2D1_POINT_2F f = D2D1::Point2F(
                        center.x + (-0.22f + 0.22f * static_cast<float>(i)) * s,
                        center.y + (0.34f + (i == 1 ? 0.06f : 0.0f)) * s);
                    const float r = 0.075f * s;
                    for (int k = 0; k < 3; ++k) {
                        const float a = static_cast<float>(k) * 3.14159265f / 3.0f;
                        target_->DrawLine(D2D1::Point2F(f.x - std::cos(a) * r, f.y - std::sin(a) * r),
                                          D2D1::Point2F(f.x + std::cos(a) * r, f.y + std::sin(a) * r),
                                          soft, std::max(1.0f, 0.035f * s));
                    }
                }
                break;
            }

            case WeatherVisual::Unknown:
            default: {
                // Thermometer: stem, bulb, and a couple of gradation ticks.
                const float stemW = 0.11f * s;
                const D2D1_RECT_F stem = D2D1::RectF(center.x - stemW, center.y - 0.40f * s,
                                                     center.x + stemW, center.y + 0.18f * s);
                target_->FillRoundedRectangle(D2D1::RoundedRect(stem, stemW, stemW), soft);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y + 0.26f * s), 0.19f * s, 0.19f * s), strong);
                const D2D1_RECT_F mercury = D2D1::RectF(center.x - stemW * 0.55f, center.y - 0.12f * s,
                                                        center.x + stemW * 0.55f, center.y + 0.20f * s);
                target_->FillRoundedRectangle(D2D1::RoundedRect(mercury, stemW * 0.55f, stemW * 0.55f), strong);
                break;
            }
        }
    }

    // Small vector glyphs shared by the weather, hardware and game-overlay cards.
    //   0 wind   1 thermometer   2 droplet
    //   3 cpu    4 memory        5 gpu       6 upload   7 download   8 disk
    //   9 gauge (frame rate)
    //
    // The game overlay used to carry its own icon set (DrawGameIcon) drawn at
    // different stroke weights and proportions, so the same CPU appeared as two
    // different symbols depending on which surface you were looking at. Every
    // surface now draws from this one family.
    void DrawMetricGlyph(D2D1_POINT_2F c, float s, int kind, ID2D1Brush* brush) {
        const float stroke = std::max(1.1f, 0.11f * s);

        if (kind == 9) {  // gauge: dial arc, ticks and a needle
            const float r = 0.40f * s;
            target_->DrawEllipse(D2D1::Ellipse(c, r, r), brush, stroke);
            for (int i = 0; i < 5; ++i) {
                const float a = -3.14159265f * 0.8f + static_cast<float>(i) * 3.14159265f * 0.4f;
                const float ca = std::cos(a);
                const float sa = std::sin(a);
                target_->DrawLine(D2D1::Point2F(c.x + ca * r, c.y + sa * r),
                                  D2D1::Point2F(c.x + ca * (r - 0.10f * s), c.y + sa * (r - 0.10f * s)),
                                  brush, stroke * 0.7f);
            }
            target_->FillEllipse(D2D1::Ellipse(c, 0.075f * s, 0.075f * s), brush);
            const float na = -3.14159265f * 0.25f;
            target_->DrawLine(c, D2D1::Point2F(c.x + std::cos(na) * r * 0.82f,
                                               c.y + std::sin(na) * r * 0.82f),
                              brush, stroke);
            return;
        }

        // Arrow used by the upload/download glyphs; `dir` is -1 up, +1 down.
        auto drawArrow = [&](float dir) {
            target_->DrawLine(D2D1::Point2F(c.x, c.y - 0.38f * s * dir),
                              D2D1::Point2F(c.x, c.y + 0.34f * s * dir), brush, stroke);
            target_->DrawLine(D2D1::Point2F(c.x - 0.24f * s, c.y + 0.10f * s * dir),
                              D2D1::Point2F(c.x, c.y + 0.36f * s * dir), brush, stroke);
            target_->DrawLine(D2D1::Point2F(c.x + 0.24f * s, c.y + 0.10f * s * dir),
                              D2D1::Point2F(c.x, c.y + 0.36f * s * dir), brush, stroke);
        };

        switch (kind) {
            case 3: {  // cpu: chip body with pins on all four sides
                const D2D1_RECT_F body = D2D1::RectF(c.x - 0.28f * s, c.y - 0.28f * s,
                                                     c.x + 0.28f * s, c.y + 0.28f * s);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(body, 0.07f * s, 0.07f * s), brush, stroke);
                const D2D1_RECT_F core = D2D1::RectF(c.x - 0.11f * s, c.y - 0.11f * s,
                                                     c.x + 0.11f * s, c.y + 0.11f * s);
                target_->FillRoundedRectangle(D2D1::RoundedRect(core, 0.03f * s, 0.03f * s), brush);
                for (int i = -1; i <= 1; ++i) {
                    const float o = static_cast<float>(i) * 0.15f * s;
                    target_->DrawLine(D2D1::Point2F(c.x + o, c.y - 0.28f * s), D2D1::Point2F(c.x + o, c.y - 0.42f * s), brush, stroke * 0.8f);
                    target_->DrawLine(D2D1::Point2F(c.x + o, c.y + 0.28f * s), D2D1::Point2F(c.x + o, c.y + 0.42f * s), brush, stroke * 0.8f);
                    target_->DrawLine(D2D1::Point2F(c.x - 0.28f * s, c.y + o), D2D1::Point2F(c.x - 0.42f * s, c.y + o), brush, stroke * 0.8f);
                    target_->DrawLine(D2D1::Point2F(c.x + 0.28f * s, c.y + o), D2D1::Point2F(c.x + 0.42f * s, c.y + o), brush, stroke * 0.8f);
                }
                break;
            }
            case 4: {  // memory: module with contact notches along the bottom
                const D2D1_RECT_F body = D2D1::RectF(c.x - 0.42f * s, c.y - 0.26f * s,
                                                     c.x + 0.42f * s, c.y + 0.22f * s);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(body, 0.06f * s, 0.06f * s), brush, stroke);
                for (int i = -2; i <= 2; ++i) {
                    const float x = c.x + static_cast<float>(i) * 0.16f * s;
                    target_->DrawLine(D2D1::Point2F(x, c.y - 0.10f * s), D2D1::Point2F(x, c.y + 0.06f * s), brush, stroke * 0.85f);
                }
                target_->DrawLine(D2D1::Point2F(c.x - 0.20f * s, c.y + 0.34f * s),
                                  D2D1::Point2F(c.x + 0.20f * s, c.y + 0.34f * s), brush, stroke);
                break;
            }
            case 5: {  // gpu: board with a fan
                const D2D1_RECT_F body = D2D1::RectF(c.x - 0.44f * s, c.y - 0.24f * s,
                                                     c.x + 0.44f * s, c.y + 0.26f * s);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(body, 0.06f * s, 0.06f * s), brush, stroke);
                target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(c.x - 0.14f * s, c.y + 0.01f * s), 0.15f * s, 0.15f * s), brush, stroke * 0.9f);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x - 0.14f * s, c.y + 0.01f * s), 0.045f * s, 0.045f * s), brush);
                target_->DrawLine(D2D1::Point2F(c.x + 0.16f * s, c.y - 0.10f * s), D2D1::Point2F(c.x + 0.32f * s, c.y - 0.10f * s), brush, stroke * 0.8f);
                target_->DrawLine(D2D1::Point2F(c.x + 0.16f * s, c.y + 0.04f * s), D2D1::Point2F(c.x + 0.32f * s, c.y + 0.04f * s), brush, stroke * 0.8f);
                break;
            }
            case 6:  // upload
                drawArrow(-1.0f);
                break;
            case 7:  // download
                drawArrow(1.0f);
                break;
            case 8: {  // disk: stacked platters
                for (int i = 0; i < 3; ++i) {
                    const float y = c.y - 0.22f * s + static_cast<float>(i) * 0.22f * s;
                    target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(c.x, y), 0.36f * s, 0.12f * s), brush, stroke * 0.9f);
                }
                break;
            }
            default:
                break;
        }
        if (kind >= 3) {
            return;
        }

        switch (kind) {
            case 0: {  // wind: three streaming lines
                const float lens[3] = {0.46f, 0.30f, 0.38f};
                for (int i = 0; i < 3; ++i) {
                    const float y = c.y + (-0.22f + 0.22f * static_cast<float>(i)) * s;
                    target_->DrawLine(D2D1::Point2F(c.x - 0.44f * s, y),
                                      D2D1::Point2F(c.x - 0.44f * s + lens[i] * s * 1.9f, y),
                                      brush, stroke);
                }
                break;
            }
            case 1: {  // thermometer
                const float w = 0.13f * s;
                const D2D1_RECT_F stem = D2D1::RectF(c.x - w, c.y - 0.42f * s, c.x + w, c.y + 0.12f * s);
                target_->FillRoundedRectangle(D2D1::RoundedRect(stem, w, w), brush);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x, c.y + 0.24f * s), 0.22f * s, 0.22f * s), brush);
                break;
            }
            case 2:
            default: {  // droplet: circle body with a tapered tip
                ComPtr<ID2D1PathGeometry> drop;
                if (SUCCEEDED(d2dFactory_->CreatePathGeometry(&drop)) && drop) {
                    ComPtr<ID2D1GeometrySink> sink;
                    if (SUCCEEDED(drop->Open(&sink)) && sink) {
                        sink->BeginFigure(D2D1::Point2F(c.x, c.y - 0.44f * s), D2D1_FIGURE_BEGIN_FILLED);
                        sink->AddBezier(D2D1::BezierSegment(
                            D2D1::Point2F(c.x + 0.34f * s, c.y - 0.02f * s),
                            D2D1::Point2F(c.x + 0.30f * s, c.y + 0.36f * s),
                            D2D1::Point2F(c.x, c.y + 0.38f * s)));
                        sink->AddBezier(D2D1::BezierSegment(
                            D2D1::Point2F(c.x - 0.30f * s, c.y + 0.36f * s),
                            D2D1::Point2F(c.x - 0.34f * s, c.y - 0.02f * s),
                            D2D1::Point2F(c.x, c.y - 0.44f * s)));
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        target_->FillGeometry(drop.Get(), brush);
                    }
                }
                break;
            }
        }
    }

    void DrawWeatherDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, bool hasWeather, const std::wstring& wIcon, const std::wstring& wText) {
        wchar_t wTemp[32] = {};
        if (hasWeather) swprintf_s(wTemp, L"%.0f\x00B0", state.weather.temperature);
        else wcscpy_s(wTemp, L"--\x00B0");

        std::wstring city = hasWeather ? state.weather.city : std::wstring(Loc(L"Locating..."));
        std::wstring desc = wText;

        // ── Layout ───────────────────────────────────────────────────────────
        // Left is an editorial hero block (place / reading / condition), left
        // aligned so the city, temperature and description share one optical
        // margin. Right is a stack of metric cards using the same card primitive
        // as every other dashboard. The old version centred each element in its
        // own box, which is what left the icon and the temperature floating apart
        // with a dead gap between them.
        const float heroLeft = rect.left + 26.0f * scale;
        const float heroRight = rect.left + 188.0f * scale;
        const float dividerX = rect.left + 200.0f * scale;
        const float metricLeft = rect.left + 214.0f * scale;
        const float metricRight = rect.right - 22.0f * scale;

        // Accent-tinted icon brushes: `strong` carries the shape, `soft` the
        // secondary detail (rain, rays, fog bands).
        ComPtr<ID2D1SolidColorBrush> iconStrong;
        ComPtr<ID2D1SolidColorBrush> iconSoft;
        target_->CreateSolidColorBrush(WithAlpha(material_.accent, 0.95f * settingsOpacity_), &iconStrong);
        target_->CreateSolidColorBrush(WithAlpha(material_.accent, 0.42f * settingsOpacity_), &iconSoft);

        // Place label.
        textBrush_->SetOpacity(0.55f);
        if (smallTextFormat_) {
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            target_->DrawTextW(city.c_str(), static_cast<UINT32>(city.length()), smallTextFormat_.Get(),
                               D2D1::RectF(heroLeft, rect.top + 26.0f * scale, heroRight, rect.top + 44.0f * scale),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        // Icon and temperature share one band and sit adjacent, so they read as
        // a single unit instead of two centred items.
        const float readingCenterY = rect.top + 82.0f * scale;
        if (hasWeather && iconStrong && iconSoft) {
            DrawWeatherIcon(D2D1::Point2F(heroLeft + 22.0f * scale, readingCenterY), 44.0f * scale,
                            WeatherVisualFromCode(state.weather.weatherCode),
                            iconStrong.Get(), iconSoft.Get());
        } else if (iconStrong && iconSoft) {
            DrawWeatherIcon(D2D1::Point2F(heroLeft + 22.0f * scale, readingCenterY), 44.0f * scale,
                            WeatherVisual::Unknown, iconStrong.Get(), iconSoft.Get());
        }

        textBrush_->SetOpacity(0.98f);
        if (hugeTextFormat_) {
            hugeTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            target_->DrawTextW(wTemp, static_cast<UINT32>(wcslen(wTemp)), hugeTextFormat_.Get(),
                               D2D1::RectF(heroLeft + 52.0f * scale, readingCenterY - 32.0f * scale,
                                           heroRight, readingCenterY + 32.0f * scale),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            hugeTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }

        const float descTop = rect.top + 120.0f * scale;
        const float descBottom = rect.top + 162.0f * scale;

        // Description
        const size_t descLength = desc.length();
        float descFontSize = 13.5f;
        if (descLength > 58) descFontSize = 9.8f;
        else if (descLength > 44) descFontSize = 10.5f;
        else if (descLength > 32) descFontSize = 11.5f;
        else if (descLength > 22) descFontSize = 12.5f;
        descFontSize *= scale;

        if (std::fabs(weatherDescFormatSize_ - descFontSize) > 0.01f || !weatherDescFormat_) {
            weatherDescFormat_.Reset();
            if (dwriteFactory_) {
                bool created = false;
                if (!settings.fontFamily.empty()) {
                    HRESULT hr = dwriteFactory_->CreateTextFormat(
                        settings.fontFamily.c_str(), nullptr,
                        DWRITE_FONT_WEIGHT_SEMI_BOLD,
                        DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL,
                        descFontSize, L"", &weatherDescFormat_);
                    if (SUCCEEDED(hr)) created = true;
                }
                if (!created) {
                    HRESULT hr = dwriteFactory_->CreateTextFormat(
                        L"Segoe UI Variable Display", nullptr,
                        DWRITE_FONT_WEIGHT_SEMI_BOLD,
                        DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL,
                        descFontSize, L"", &weatherDescFormat_);
                    if (FAILED(hr)) {
                        dwriteFactory_->CreateTextFormat(
                            L"Segoe UI", nullptr,
                            DWRITE_FONT_WEIGHT_SEMI_BOLD,
                            DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL,
                            descFontSize, L"", &weatherDescFormat_);
                    }
                }
            }
            if (weatherDescFormat_) {
                weatherDescFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
                // Left aligned to share the hero column's margin with the city
                // and temperature.
                weatherDescFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                weatherDescFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
            weatherDescFormatSize_ = descFontSize;
        }

        textBrush_->SetOpacity(0.82f);
        target_->DrawTextW(desc.c_str(), static_cast<UINT32>(desc.length()),
                           weatherDescFormat_ ? weatherDescFormat_.Get() : textFormat_.Get(),
                           D2D1::RectF(heroLeft, descTop, heroRight, descBottom),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        textBrush_->SetOpacity(0.96f);

        // Hairline divider, inset from both ends so it reads as a separator
        // rather than a full-height rule.
        ComPtr<ID2D1SolidColorBrush> divider;
        target_->CreateSolidColorBrush(WithAlpha(material_.hairline, material_.hairline.a * settingsOpacity_), &divider);
        target_->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(dividerX, rect.top + 34.0f * scale,
                                          dividerX + 1.0f * scale, rect.bottom - 34.0f * scale),
                              0.5f * scale, 0.5f * scale), divider.Get());

        // ── Metric cards ─────────────────────────────────────────────────────
        // Each card is icon + label on one line with the value beneath, which
        // keeps long localized labels from colliding with the value the way a
        // single "Label: value" line did.
        struct Metric {
            int glyph;
            const wchar_t* label;
            std::wstring value;
        };

        const std::wstring windUnit = settings.weatherFahrenheit ? L" mph" : L" km/h";
        const Metric metrics[3] = {
            {0, Loc(L"Wind"),
             hasWeather ? state.weather.windSpeed + windUnit + L" " + WindDirToArrow(state.weather.windDir)
                        : std::wstring(L"--")},
            {1, Loc(L"Feels Like"),
             hasWeather ? state.weather.feelsLike + L"\x00B0" : std::wstring(L"--")},
            {2, Loc(L"Humidity"),
             hasWeather ? state.weather.humidity + L"%" : std::wstring(L"--")},
        };

        const float cardH = 40.0f * scale;
        const float cardGap = 7.0f * scale;
        const float stackH = cardH * 3.0f + cardGap * 2.0f;
        float cardTop = (rect.top + rect.bottom) * 0.5f - stackH * 0.5f;

        for (const Metric& m : metrics) {
            const D2D1_RECT_F card = D2D1::RectF(metricLeft, cardTop, metricRight, cardTop + cardH);
            DrawCard(card, 10.0f * scale);

            if (iconSoft) {
                DrawMetricGlyph(D2D1::Point2F(card.left + 17.0f * scale, card.top + 14.0f * scale),
                                14.0f * scale, m.glyph, iconSoft.Get());
            }

            const float textLeft = card.left + 30.0f * scale;
            if (smallTextFormat_) {
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                mutedBrush_->SetOpacity(0.62f);
                target_->DrawTextW(m.label, static_cast<UINT32>(wcslen(m.label)), smallTextFormat_.Get(),
                                   D2D1::RectF(textLeft, card.top + 4.0f * scale,
                                               card.right - 8.0f * scale, card.top + 20.0f * scale),
                                   mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            if (textFormat_) {
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                textBrush_->SetOpacity(0.95f);
                target_->DrawTextW(m.value.c_str(), static_cast<UINT32>(m.value.size()), textFormat_.Get(),
                                   D2D1::RectF(textLeft, card.top + 19.0f * scale,
                                               card.right - 8.0f * scale, card.bottom - 3.0f * scale),
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            cardTop += cardH + cardGap;
        }

        textBrush_->SetOpacity(0.96f);
        mutedBrush_->SetOpacity(0.75f);
    }

    // File Tray (#33): a shelf for files dragged onto the island. Rows are built
    // from the shared DrawCard primitive so the shelf matches every other
    // dashboard, with the real Explorer icon for each file.
    void DrawFileTrayDashboard(const SharedState& state, D2D1_RECT_F rect,
                               const Settings& settings, float scale) {
        const float padX = FileTrayLayout::kPadX * scale;

        const D2D1_COLOR_F fileCardSurface = material_.onDark
            ? D2D1::ColorF(0.10f, 0.12f, 0.15f, 0.98f * settingsOpacity_)
            : D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.98f * settingsOpacity_);
        const float clearCenterX = rect.left + 26.0f * scale;
        const float clearCenterY = (rect.top + rect.bottom) * 0.5f;
        const D2D1_RECT_F clearRect = D2D1::RectF(clearCenterX - 17.0f * scale,
                                                   clearCenterY - 17.0f * scale,
                                                   clearCenterX + 17.0f * scale,
                                                   clearCenterY + 17.0f * scale);
        ComPtr<ID2D1SolidColorBrush> clearFill;
        target_->CreateSolidColorBrush(fileCardSurface, &clearFill);
        if (clearFill) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(clearRect, 10.0f * scale,
                                                             10.0f * scale), clearFill.Get());
            target_->DrawRoundedRectangle(D2D1::RoundedRect(clearRect, 10.0f * scale,
                                                             10.0f * scale), accentBrush_.Get(), 0.8f);
        }
        if (iconFormat_) {
            ComPtr<ID2D1SolidColorBrush> clearIcon;
            target_->CreateSolidColorBrush(
                EnsureContrastAgainstBackground(material_.textPrimary, fileCardSurface), &clearIcon);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(L"\uE74D", 1, iconFormat_.Get(), clearRect,
                               clearIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        const float listTop = rect.top + FileTrayLayout::kListTop * scale;
        const float listBottom = rect.bottom - FileTrayLayout::kListBottomInset * scale;

        if (state.fileTrayItems.empty()) {
            // Keep the empty state borderless and centered in the file area.
            const D2D1_RECT_F dropRect = D2D1::RectF(rect.left + padX, listTop,
                                                      rect.right - padX, listBottom);
            if (textFormat_) {
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                const wchar_t* hint = Loc(L"Drag files here");
                mutedBrush_->SetOpacity(0.80f);
                target_->DrawTextW(hint, static_cast<UINT32>(wcslen(hint)), textFormat_.Get(),
                                   dropRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }
            mutedBrush_->SetOpacity(0.75f);
            return;
        }

        const float rowH = FileTrayLayout::kRowHeight * scale;
        const float rowGap = FileTrayLayout::kRowGap * scale;
        const int capacity = FileTrayLayout::VisibleRowCapacity((rect.bottom - rect.top) / scale);
        const int scrollMax = std::max(0, static_cast<int>(state.fileTrayItems.size()) - capacity);
        const int scrollOffset = ClampInt(g_fileTrayScrollOffset.load(std::memory_order_relaxed),
                                          0, scrollMax);
        g_fileTrayScrollOffset.store(scrollOffset, std::memory_order_relaxed);

        // Newest first: the file just dropped is the one the user wants.
        int drawn = 0;
        for (auto it = state.fileTrayItems.rbegin() + scrollOffset;
             it != state.fileTrayItems.rend() && drawn < capacity; ++it, ++drawn) {
            const FileTrayItem& item = *it;
            const float rowTop = listTop + drawn * (rowH + rowGap);
            const D2D1_RECT_F row = D2D1::RectF(rect.left + padX, rowTop, rect.right - padX, rowTop + rowH);

            const bool hovered = (g_hoveredFileTrayRow.load(std::memory_order_relaxed) == drawn);
            const D2D1_COLOR_F rowSurface = hovered
                ? MixColor(fileCardSurface, material_.accent, 0.16f)
                : fileCardSurface;
            ComPtr<ID2D1SolidColorBrush> rowFill;
            if (SUCCEEDED(target_->CreateSolidColorBrush(rowSurface, &rowFill)) && rowFill) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(row, 9.0f * scale, 9.0f * scale),
                                              rowFill.Get());
            }
            ComPtr<ID2D1SolidColorBrush> fileNameBrush;
            ComPtr<ID2D1SolidColorBrush> fileMetaBrush;
            target_->CreateSolidColorBrush(
                EnsureContrastAgainstBackground(material_.textPrimary, rowSurface), &fileNameBrush);
            target_->CreateSolidColorBrush(
                EnsureContrastAgainstBackground(material_.textSecondary, rowSurface), &fileMetaBrush);

            // Real shell icon when we managed to extract one, otherwise a glyph.
            const D2D1_RECT_F iconRect = D2D1::RectF(row.left + 8.0f * scale, row.top + 6.0f * scale,
                                                      row.left + 26.0f * scale, row.bottom - 6.0f * scale);
            if (!item.icon.bgra.empty()) {
                DrawBitmapPixels(item.icon, iconRect, fileTrayIconBitmap_, fileTrayIconGeneration_, 0.98f);
            } else if (iconFormat_) {
                const wchar_t* glyph = item.isDirectory ? L"\uE8B7" : L"\uE7C3";
                accentBrush_->SetOpacity(0.85f);
                target_->DrawTextW(glyph, 1, iconFormat_.Get(), iconRect, accentBrush_.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_NONE);
                accentBrush_->SetOpacity(1.0f);
            }

            wchar_t sizeBuf[40] = {};
            if (item.isDirectory) {
                wcscpy_s(sizeBuf, L"—");
            } else if (item.sizeBytes >= 1073741824ull) {
                swprintf_s(sizeBuf, L"%.1f GB", static_cast<double>(item.sizeBytes) / 1073741824.0);
            } else if (item.sizeBytes >= 1048576ull) {
                swprintf_s(sizeBuf, L"%.1f MB", static_cast<double>(item.sizeBytes) / 1048576.0);
            } else if (item.sizeBytes >= 1024ull) {
                swprintf_s(sizeBuf, L"%.0f KB", static_cast<double>(item.sizeBytes) / 1024.0);
            } else {
                swprintf_s(sizeBuf, L"%llu B", static_cast<unsigned long long>(item.sizeBytes));
            }

            const float sizeW = 66.0f * scale;
            const D2D1_RECT_F nameRect = D2D1::RectF(row.left + 32.0f * scale, row.top,
                                                      row.right - sizeW - 36.0f * scale, row.bottom);
            if (smallTextFormat_) {
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(item.name.c_str(), static_cast<UINT32>(item.name.size()),
                                   smallTextFormat_.Get(), nameRect, fileNameBrush.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_CLIP);

                const D2D1_RECT_F sizeRect = D2D1::RectF(row.right - sizeW - 32.0f * scale, row.top,
                                                          row.right - 34.0f * scale, row.bottom);
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                target_->DrawTextW(sizeBuf, static_cast<UINT32>(wcslen(sizeBuf)),
                                   smallTextFormat_.Get(), sizeRect, fileMetaBrush.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_NONE);
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
            const float dotsX = row.right - 20.0f * scale;
            const float dotsY = (row.top + row.bottom) * 0.5f;
            const D2D1_RECT_F dotsButton = D2D1::RectF(row.right - 38.0f * scale,
                                                        dotsY - 13.0f * scale,
                                                        row.right - 4.0f * scale,
                                                        dotsY + 13.0f * scale);
            const D2D1_COLOR_F dotsSurface = hovered
                ? MixColor(rowSurface, material_.accent, 0.30f)
                : (material_.onDark ? D2D1::ColorF(0.20f, 0.23f, 0.27f, 0.98f * settingsOpacity_)
                                    : D2D1::ColorF(0.84f, 0.87f, 0.91f, 0.98f * settingsOpacity_));
            ComPtr<ID2D1SolidColorBrush> dotsButtonBrush;
            target_->CreateSolidColorBrush(dotsSurface, &dotsButtonBrush);
            if (dotsButtonBrush) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(dotsButton, 8.0f * scale,
                                                                  8.0f * scale),
                                              dotsButtonBrush.Get());
            }
            ComPtr<ID2D1SolidColorBrush> dotsBrush;
            target_->CreateSolidColorBrush(
                EnsureContrastAgainstBackground(material_.textPrimary, dotsSurface), &dotsBrush);
            if (dotsBrush) {
                for (int dot = -1; dot <= 1; ++dot) {
                    const float radius = hovered ? 1.8f * scale : 1.4f * scale;
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotsX + dot * 5.0f * scale,
                                                                     dotsY), radius, radius),
                                         dotsBrush.Get());
                }
            }
        }

        // "+N more" when the shelf holds more than fits.
        const size_t fileTrayTotal = state.fileTrayItems.size();
        if (static_cast<int>(fileTrayTotal) > capacity && smallTextFormat_) {
            wchar_t moreBuf[32] = {};
            swprintf_s(moreBuf, L"+%d", static_cast<int>(fileTrayTotal) - capacity);
            const D2D1_RECT_F moreRect = D2D1::RectF(rect.left + padX, listBottom - 12.0f * scale,
                                                      rect.right - padX, listBottom);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            mutedBrush_->SetOpacity(0.65f);
            target_->DrawTextW(moreBuf, static_cast<UINT32>(wcslen(moreBuf)), smallTextFormat_.Get(),
                               moreRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        }

        textBrush_->SetOpacity(0.96f);
        mutedBrush_->SetOpacity(0.75f);
    }

    // Load colour is *semantic*, not decorative: the accent while a component is
    // comfortable, amber under pressure, red when saturated. The previous design
    // assigned a fixed rainbow colour per metric, which carried no information
    // (CPU and NET UP were both green, GPU and NET DOWN both cyan) and fought the
    // album-art accent.
    D2D1_COLOR_F LoadStateColor(float fraction) const {
        if (fraction >= 0.90f) {
            return D2D1::ColorF(1.0f, 0.35f, 0.32f, 1.0f);
        }
        if (fraction >= 0.75f) {
            return D2D1::ColorF(1.0f, 0.69f, 0.13f, 1.0f);
        }
        return material_.accent;
    }

    void DrawHardwareMonitorDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, float scale) {
        (void)settings;

        const float padX = 24.0f * scale;

        // Header, left aligned to match the File Tray and weather dashboards.
        textBrush_->SetOpacity(0.96f);
        const wchar_t* hwTitle = Loc(L"Hardware Monitor");
        if (boldTextFormat_) {
            boldTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            target_->DrawTextW(hwTitle, static_cast<UINT32>(wcslen(hwTitle)), boldTextFormat_.Get(),
                               D2D1::RectF(rect.left + padX, rect.top + 16.0f * scale,
                                           rect.right - padX, rect.top + 32.0f * scale),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            boldTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }

        wchar_t cpuBuf[32], ramBuf[40], gpuBuf[32], upBuf[32], downBuf[32], hostRamBuf[40];
        swprintf_s(cpuBuf, L"%d%%", state.system.cpuPercent);
        swprintf_s(ramBuf, L"%.1f / %.1f GB", state.system.memoryUsedGB, state.system.memoryTotalGB);
        if (state.system.gpuPercent >= 0) {
            swprintf_s(gpuBuf, L"%d%%", state.system.gpuPercent);
        } else {
            wcscpy_s(gpuBuf, L"--");
        }
        swprintf_s(upBuf, L"%.1f Mbps", state.system.netUpMbps);
        swprintf_s(downBuf, L"%.1f Mbps", state.system.netDownMbps);
        if (state.system.hostPrivateBytes > 0) {
            swprintf_s(hostRamBuf, L"%.0f MB",
                       static_cast<double>(state.system.hostPrivateBytes) / (1024.0 * 1024.0));
        } else {
            wcscpy_s(hostRamBuf, L"--");
        }

        const float ramFraction = (state.system.memoryTotalGB > 0.01f)
            ? Clamp(state.system.memoryUsedGB / state.system.memoryTotalGB, 0.0f, 1.0f)
            : -1.0f;

        struct HwMetric {
            int glyph;
            const wchar_t* label;
            const wchar_t* value;
            float fraction;  // negative means "no meaningful 0-100 scale"
        };

        // Network throughput has no natural ceiling, so those two cards get no
        // load bar rather than a bar against an invented maximum.
        const HwMetric metrics[6] = {
            {3, L"CPU",      cpuBuf,  Clamp(state.system.cpuPercent / 100.0f, 0.0f, 1.0f)},
            {6, L"NET UP",   upBuf,   -1.0f},
            {4, L"RAM",      ramBuf,  ramFraction},
            {7, L"NET DOWN", downBuf, -1.0f},
            {5, L"GPU",      gpuBuf,  state.system.gpuPercent >= 0
                                          ? Clamp(state.system.gpuPercent / 100.0f, 0.0f, 1.0f)
                                          : -1.0f},
            {3, L"WINDHAWK RAM", hostRamBuf, -1.0f},
        };

        // 2 x 3 grid of cards. Cards give the grid its structure, so the old
        // full-height centre divider is gone.
        const float colGap = 9.0f * scale;
        const float colW = (rect.right - rect.left - padX * 2.0f - colGap) * 0.5f;
        const float rowH = 38.0f * scale;
        const float rowGap = 6.0f * scale;
        const float gridTop = rect.top + 38.0f * scale;

        for (int i = 0; i < 6; ++i) {
            const HwMetric& m = metrics[i];
            const int col = i % 2;
            const int row = i / 2;

            const float cardLeft = rect.left + padX + static_cast<float>(col) * (colW + colGap);
            const float cardTop = gridTop + static_cast<float>(row) * (rowH + rowGap);
            const D2D1_RECT_F card = D2D1::RectF(cardLeft, cardTop, cardLeft + colW, cardTop + rowH);

            DrawCard(card, 9.0f * scale);

            const bool hasBar = m.fraction >= 0.0f;
            const D2D1_COLOR_F tint = hasBar ? LoadStateColor(m.fraction) : material_.accent;

            ComPtr<ID2D1SolidColorBrush> glyphBrush;
            target_->CreateSolidColorBrush(WithAlpha(tint, 0.85f * settingsOpacity_), &glyphBrush);
            if (glyphBrush) {
                DrawMetricGlyph(D2D1::Point2F(card.left + 16.0f * scale, card.top + 15.0f * scale),
                                15.0f * scale, m.glyph, glyphBrush.Get());
            }

            const float textLeft = card.left + 30.0f * scale;
            const float textRight = card.right - 9.0f * scale;

            // Label sits above the value rather than sharing a line with it.
            // A single "Label: value" line collides once localized -- German
            // "Luftfeuchte" or Russian "Влажность" leave no room for the number.
            // The three bands (label / value / bar) are kept adjacent but
            // non-overlapping so nothing clips into its neighbour.
            if (smallTextFormat_) {
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                mutedBrush_->SetOpacity(0.60f);
                target_->DrawTextW(m.label, static_cast<UINT32>(wcslen(m.label)), smallTextFormat_.Get(),
                                   D2D1::RectF(textLeft, card.top + 2.0f * scale,
                                               textRight, card.top + 15.0f * scale),
                                   mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            if (textFormat_) {
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                textBrush_->SetOpacity(0.95f);
                // Without a bar the value takes the band the bar would have
                // used, so the two card shapes stay optically balanced.
                const float valueBottom = hasBar ? card.top + 30.0f * scale : card.bottom - 4.0f * scale;
                target_->DrawTextW(m.value, static_cast<UINT32>(wcslen(m.value)), textFormat_.Get(),
                                   D2D1::RectF(textLeft, card.top + 15.0f * scale, textRight, valueBottom),
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            if (hasBar) {
                const D2D1_RECT_F track = D2D1::RectF(textLeft, card.bottom - 6.0f * scale,
                                                      textRight, card.bottom - 3.5f * scale);
                ComPtr<ID2D1SolidColorBrush> trackBrush;
                if (SUCCEEDED(target_->CreateSolidColorBrush(material_.raisedStrong, &trackBrush)) && trackBrush) {
                    target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1.25f * scale, 1.25f * scale), trackBrush.Get());
                }

                const float span = (track.right - track.left) * m.fraction;
                if (span > 0.5f) {
                    ComPtr<ID2D1SolidColorBrush> fillBrush;
                    if (SUCCEEDED(target_->CreateSolidColorBrush(WithAlpha(tint, 0.95f), &fillBrush)) && fillBrush) {
                        target_->FillRoundedRectangle(
                            D2D1::RoundedRect(D2D1::RectF(track.left, track.top, track.left + span, track.bottom),
                                              1.25f * scale, 1.25f * scale),
                            fillBrush.Get());
                    }
                }
            }
        }

        textBrush_->SetOpacity(0.96f);
        mutedBrush_->SetOpacity(0.75f);
    }

    void DrawTimeDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, SYSTEMTIME& local) {
        (void)now;

        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;

        if (settings.clockAccentGlow) {
            D2D1_COLOR_F accentColor = accentBrush_ ? accentBrush_->GetColor() : D2D1::ColorF(0x4cc9f0);
            const bool glowColorChanged = !timeGlowColorValid_ ||
                std::fabs(accentColor.r - timeGlowColor_.r) > 0.025f ||
                std::fabs(accentColor.g - timeGlowColor_.g) > 0.025f ||
                std::fabs(accentColor.b - timeGlowColor_.b) > 0.025f;
            if (!timeGlowBrush_ || glowColorChanged) {
                D2D1_GRADIENT_STOP stops[2] = {
                    {0.0f, D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.24f)},
                    {1.0f, D2D1::ColorF(accentColor.r, accentColor.g, accentColor.b, 0.0f)},
                };
                ComPtr<ID2D1GradientStopCollection> glowStops;
                if (SUCCEEDED(target_->CreateGradientStopCollection(stops, 2, &glowStops)) && glowStops) {
                    target_->CreateRadialGradientBrush(
                        D2D1::RadialGradientBrushProperties(D2D1::Point2F(cx, cy - 8.0f * scale),
                            D2D1::Point2F(0, 0), 150.0f * scale, 70.0f * scale),
                        glowStops.Get(), &timeGlowBrush_);
                    timeGlowColor_ = accentColor;
                    timeGlowColorValid_ = timeGlowBrush_.Get() != nullptr;
                }
            }
            if (timeGlowBrush_) {
                const D2D1_POINT_2F glowCenter = D2D1::Point2F(cx, cy - 8.0f * scale);
                timeGlowBrush_->SetCenter(glowCenter);
                timeGlowBrush_->SetRadiusX(150.0f * scale);
                timeGlowBrush_->SetRadiusY(70.0f * scale);
                target_->FillEllipse(D2D1::Ellipse(glowCenter, 150.0f * scale, 70.0f * scale),
                                     timeGlowBrush_.Get());
            }
        }

        // A restrained live clock face gives the Time dashboard a little motion
        // without animating the glass surface or rebuilding a costly effect.
        // The renderer schedules frames only while this dashboard is visible.
        if (!settings.reduceMotion) {
            const D2D1_POINT_2F face = D2D1::Point2F(rect.right - 38.0f * scale,
                                                       rect.top + 37.0f * scale);
            const float radius = 13.0f * scale;
            mutedBrush_->SetOpacity(0.52f);
            target_->DrawEllipse(D2D1::Ellipse(face, radius, radius), mutedBrush_.Get(), 1.0f * scale);
            for (int tick = 0; tick < 12; ++tick) {
                const float angle = static_cast<float>(tick) * 6.28318530718f / 12.0f;
                const float inner = radius * 0.76f;
                const float outer = radius * 0.91f;
                target_->DrawLine(
                    D2D1::Point2F(face.x + std::cos(angle) * inner, face.y + std::sin(angle) * inner),
                    D2D1::Point2F(face.x + std::cos(angle) * outer, face.y + std::sin(angle) * outer),
                    mutedBrush_.Get(), 1.0f * scale);
            }
            const float seconds = static_cast<float>(local.wSecond) + local.wMilliseconds / 1000.0f;
            const float handAngle = seconds * 6.28318530718f / 60.0f - 1.57079632679f;
            accentBrush_->SetOpacity(0.96f);
            target_->DrawLine(face,
                D2D1::Point2F(face.x + std::cos(handAngle) * radius * 0.70f,
                              face.y + std::sin(handAngle) * radius * 0.70f),
                accentBrush_.Get(), 1.6f * scale);
            target_->FillEllipse(D2D1::Ellipse(face, 1.7f * scale, 1.7f * scale), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
            mutedBrush_->SetOpacity(0.75f);
        }

        // Time and date as a single typographic block. `dateFirst` promotes the
        // date to the headline for people who care about it more than the clock
        // (#61); either way the pair stays optically centred as a unit.
        const std::wstring timeText = FormatIslandTime(local, settings.clockFollowSystem,
                                                       settings.use24HourClock, settings.showSeconds);
        const std::wstring dateText = FormatIslandDate(local, settings.dateFormat, L"dddd, MMMM d");

        IDWriteTextFormat* bigFmt = timeDashboardFormat_ ? timeDashboardFormat_.Get() : hugeTextFormat_.Get();
        IDWriteTextFormat* smallFmt = dateDashboardFormat_ ? dateDashboardFormat_.Get() : boldTextFormat_.Get();

        const std::wstring& headline = settings.dateFirst ? dateText : timeText;
        const std::wstring& subline = settings.dateFirst ? timeText : dateText;
        IDWriteTextFormat* headlineFmt = settings.dateFirst ? smallFmt : bigFmt;
        IDWriteTextFormat* sublineFmt = settings.dateFirst ? bigFmt : smallFmt;

        if (settings.dateFirst) {
            // Date on top reads as a label, so give the big clock the lower slot.
            const D2D1_RECT_F headRect = D2D1::RectF(rect.left, cy - 46.0f * scale, rect.right, cy - 22.0f * scale);
            textBrush_->SetOpacity(0.92f);
            target_->DrawTextW(headline.c_str(), static_cast<UINT32>(headline.size()), headlineFmt,
                               headRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

            const D2D1_RECT_F subRect = D2D1::RectF(rect.left, cy - 20.0f * scale, rect.right, cy + 44.0f * scale);
            textBrush_->SetOpacity(0.98f);
            target_->DrawTextW(subline.c_str(), static_cast<UINT32>(subline.size()), sublineFmt,
                               subRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        } else {
            const D2D1_RECT_F headRect = D2D1::RectF(rect.left, cy - 44.0f * scale, rect.right, cy + 20.0f * scale);
            textBrush_->SetOpacity(0.98f);
            target_->DrawTextW(headline.c_str(), static_cast<UINT32>(headline.size()), headlineFmt,
                               headRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

            const D2D1_RECT_F subRect = D2D1::RectF(rect.left, cy + 22.0f * scale, rect.right, cy + 46.0f * scale);
            mutedBrush_->SetOpacity(0.85f);
            target_->DrawTextW(subline.c_str(), static_cast<UINT32>(subline.size()), sublineFmt,
                               subRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        textBrush_->SetOpacity(0.98f);

        const bool micActive = state.system.micActive && settings.privacyDots && settings.privacyDotsMic;
        const bool camActive = state.system.cameraActive && settings.privacyDots && settings.privacyDotsCam;

        if (camActive || micActive) {
            std::wstring label;
            D2D1_COLOR_F dotColor;

            if (camActive && micActive) {
                dotColor = settings.privacyDotsCamHex;
                if (!state.system.cameraApp.empty() && !state.system.micApp.empty() && state.system.cameraApp == state.system.micApp) {
                    label = state.system.cameraApp + L" is using camera & microphone";
                } else if (!state.system.cameraApp.empty() && !state.system.micApp.empty()) {
                    label = state.system.cameraApp + L" & " + state.system.micApp + L" using camera & mic";
                } else if (!state.system.cameraApp.empty()) {
                    label = state.system.cameraApp + L" is using camera & microphone";
                } else if (!state.system.micApp.empty()) {
                    label = state.system.micApp + L" is using camera & microphone";
                } else {
                    label = L"Camera & microphone in use";
                }
            } else if (camActive) {
                dotColor = settings.privacyDotsCamHex;
                if (!state.system.cameraApp.empty()) {
                    label = state.system.cameraApp + L" is using your camera";
                } else {
                    label = L"Camera in use";
                }
            } else {
                dotColor = settings.privacyDotsMicHex;
                if (!state.system.micApp.empty()) {
                    label = state.system.micApp + L" is using your microphone";
                } else {
                    label = L"Microphone in use";
                }
            }

            IDWriteTextFormat* fmt = smallTextFormat_ ? smallTextFormat_.Get() : textFormat_.Get();
            if (fmt && dwriteFactory_) {
                ComPtr<IDWriteTextLayout> textLayout;
                HRESULT hr = dwriteFactory_->CreateTextLayout(
                    label.c_str(), static_cast<UINT32>(label.size()),
                    fmt, 500.0f, 30.0f, &textLayout);

                if (SUCCEEDED(hr) && textLayout) {
                    DWRITE_TEXT_METRICS tm = {};
                    textLayout->GetMetrics(&tm);

                    const float pillH = 22.0f * scale;
                    const float pillW = tm.width + 28.0f * scale;
                    const float pillX = cx - pillW * 0.5f;
                    const float pillY = rect.bottom - 18.0f * scale - pillH;

                    ComPtr<ID2D1SolidColorBrush> badgeBg;
                    target_->CreateSolidColorBrush(WithAlpha(material_.raised, material_.raised.a * settingsOpacity_), &badgeBg);
                    if (badgeBg) {
                        target_->FillRoundedRectangle(
                            D2D1::RoundedRect(D2D1::RectF(pillX, pillY, pillX + pillW, pillY + pillH), pillH * 0.5f, pillH * 0.5f),
                            badgeBg.Get());
                    }

                    ComPtr<ID2D1SolidColorBrush> dotBrush;
                    dotColor.a = settingsOpacity_;
                    target_->CreateSolidColorBrush(dotColor, &dotBrush);
                    if (dotBrush) {
                        const float badgeDotR = 3.5f * scale;
                        target_->FillEllipse(
                            D2D1::Ellipse(D2D1::Point2F(pillX + 10.0f * scale, pillY + pillH * 0.5f), badgeDotR, badgeDotR),
                            dotBrush.Get());
                    }

                    mutedBrush_->SetOpacity(0.90f);
                    target_->DrawTextLayout(
                        D2D1::Point2F(pillX + 18.0f * scale, pillY + (pillH - tm.height) * 0.5f),
                        textLayout.Get(), mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
                    mutedBrush_->SetOpacity(0.75f);
                }
            }
        }

        textBrush_->SetOpacity(0.96f);
        mutedBrush_->SetOpacity(0.75f);
    }

    void DrawIdleDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings,
                           double now) {
        if (!clockFormat_) return;

        // Clip to the island's real silhouette (pill / notch / w11 rounded
        // rect), not its bounding box — a plain rect clip leaves the corners
        // outside the rounded shape unclipped, which is what was showing up
        // as a faint square "border" around the round island while collapsing.
        // Publish the same content-space geometry the media surface does, so the
        // File Tray's row hit test works while the island is idle too.
        PublishContentGeometry(rect);

        const float dashHeight = rect.bottom - rect.top;
        const float dashRadius = ContentIslandRadius(dashHeight);
        ComPtr<ID2D1Geometry> dashMask = CreateIslandMaskGeometry(rect, dashRadius, g_settings.notchStyle);
        ComPtr<ID2D1Layer> dashLayer;
        target_->CreateLayer(&dashLayer);
        const bool haveDashMask = dashMask && dashLayer;
        if (haveDashMask) {
            target_->PushLayer(D2D1::LayerParameters(rect, dashMask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), dashLayer.Get());
        } else {
            target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        }

        SYSTEMTIME local = {};
        GetLocalTime(&local);
        const std::wstring collapsedTime = FormatIslandTime(local, settings.clockFollowSystem,
                                                            settings.use24HourClock,
                                                            settings.showSeconds);
        const wchar_t* timeBuf = collapsedTime.c_str();

        const float scale = 1.0f;
        const float width = rect.right - rect.left;

        bool hasWeather = state.weather.hasData && (now - state.weather.lastUpdated < 3600.0);
        std::wstring wIcon = L"🌡️";
        std::wstring wText = Loc(L"Loading...");
        if (hasWeather) {
            wText = state.weather.weatherDesc;
            GetWeatherIconAndText(state.weather.weatherCode, wIcon, wText);
        }

        // Cross-fade the collapsed status-bar view and the expanded tab view
        // across a small width band around the switchover point instead of
        // a hard cut. Previously this was a plain if/else on width, so the
        // clock's glow (and everything else in the expanded view) just got
        // clipped smaller as the pill shrank and then vanished outright the
        // instant width crossed the threshold — a pop, not a fade. Blending
        // both views by opacity (with an eased curve) makes it a dissolve.
        constexpr float kExpandThreshold = 220.0f;
        constexpr float kCrossfadeRange = 46.0f;
        auto SmoothFade = [](float t) {
            t = Clamp(t, 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        };
        const float collapsedAlpha = SmoothFade((kExpandThreshold - width) / kCrossfadeRange);
        const float expandedAlpha = SmoothFade((width - (kExpandThreshold - kCrossfadeRange)) / kCrossfadeRange);

        ComPtr<ID2D1Layer> dashFadeLayer;
        target_->CreateLayer(&dashFadeLayer);

        if (collapsedAlpha > 0.01f) {
            if (dashFadeLayer) {
                target_->PushLayer(D2D1::LayerParameters(rect, nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                                          D2D1::IdentityMatrix(), collapsedAlpha, nullptr,
                                                          D2D1_LAYER_OPTIONS_NONE),
                                   dashFadeLayer.Get());
            }

            // Collapsed Mode (Apple Dynamic Island status bar).
            //
            // Slots are placed from the same measurements that sized the pill, so
            // the clock gets exactly the room it needs and the divider sits between
            // the two strings instead of at an arbitrary geometric centre. The old
            // version split the pill 50/50 at centerX and padded both ends by 6px,
            // which is what produced the dead air on short strings
            // (windhawk-mods#5086).
            const IdleStripMetrics idleMetrics = MeasureIdleStrip(state, settings, now);
            IDWriteTextFormat* idleFmt =
                idleTextFormat_ ? idleTextFormat_.Get() : smallTextFormat_.Get();

            // The dot is anchored to the right edge by DrawPrivacyDots, so reserve
            // its lane rather than shrinking the text box out from under the clock.
            const float privacyReserve =
                idleMetrics.hasPrivacy ? IdleStripLayout::kPrivacyReserve * scale : 0.0f;
            const float innerLeft = rect.left + IdleStripLayout::kPadX * scale;
            const float innerRight =
                rect.right - IdleStripLayout::kPadX * scale - privacyReserve;

            float blockWidth = idleMetrics.clockWidth;
            if (idleMetrics.hasWeather) {
                blockWidth += (IdleStripLayout::kSlotGap * 2.0f +
                               IdleStripLayout::kDividerWidth) * scale +
                              idleMetrics.weatherWidth;
            }

            // Centre the block in whatever room the pill currently has. Mid-spring
            // the rect is wider or narrower than the natural width, and clamping
            // the slack at zero stops the slots from inverting when it is narrower.
            const float slack = std::max(0.0f, (innerRight - innerLeft) - blockWidth);
            float slotX = innerLeft + slack * 0.5f;

            // Each slot is exactly its measured width, and the measurement used the
            // widest-digit form, so the live string is centred inside a box it is
            // guaranteed to fit rather than drifting as the digits change.
            textBrush_->SetOpacity(0.96f);
            const D2D1_RECT_F timeRect =
                D2D1::RectF(slotX, rect.top, slotX + idleMetrics.clockWidth, rect.bottom);
            target_->DrawTextW(timeBuf, static_cast<UINT32>(wcslen(timeBuf)), idleFmt,
                               timeRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            slotX += idleMetrics.clockWidth;

            if (idleMetrics.hasWeather) {
                slotX += IdleStripLayout::kSlotGap * scale;

                ComPtr<ID2D1SolidColorBrush> divider;
                target_->CreateSolidColorBrush(
                    WithAlpha(material_.hairline, material_.hairline.a * settingsOpacity_),
                    &divider);
                if (divider) {
                    const float divW = IdleStripLayout::kDividerWidth * scale;
                    const float divTop = rect.top + IdleStripLayout::kDividerInsetY * scale;
                    const float divBottom = rect.bottom - IdleStripLayout::kDividerInsetY * scale;
                    target_->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF(slotX, divTop, slotX + divW, divBottom),
                                          divW * 0.5f, divW * 0.5f), divider.Get());
                }
                slotX += (IdleStripLayout::kDividerWidth + IdleStripLayout::kSlotGap) * scale;

                wchar_t weatherLabel[32] = {};
                if (hasWeather) swprintf_s(weatherLabel, L"%s %.0f\x00B0", wIcon.c_str(), state.weather.temperature);
                else wcscpy_s(weatherLabel, ARRAYSIZE(weatherLabel), L"\U0001F321\uFE0F --\x00B0");

                const D2D1_RECT_F wRect =
                    D2D1::RectF(slotX, rect.top, slotX + idleMetrics.weatherWidth, rect.bottom);
                target_->DrawTextW(weatherLabel, static_cast<UINT32>(wcslen(weatherLabel)), idleFmt,
                                   wRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
            textBrush_->SetOpacity(1.0f);

            if (dashFadeLayer) {
                target_->PopLayer();
            }
        }

        if (expandedAlpha > 0.01f) {
            if (dashFadeLayer) {
                target_->PushLayer(D2D1::LayerParameters(rect, nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                                          D2D1::IdentityMatrix(), expandedAlpha, nullptr,
                                                          D2D1_LAYER_OPTIONS_NONE),
                                   dashFadeLayer.Get());
            }

            const std::vector<int> activeTabs = DashboardSectionIds(settings, state);

            int maxTabs = static_cast<int>(activeTabs.size());
            int tabIdx = g_idleTab.load(std::memory_order_relaxed);
            tabIdx = ((tabIdx % maxTabs) + maxTabs) % maxTabs;
            int activeTabId = activeTabs[tabIdx];

            if (activeTabId == 0) DrawTimeDashboard(state, rect, settings, now, scale, local);
            else if (activeTabId == 1 || activeTabId == 6) {
                // Playback tabs are also reachable while the island's primary
                // activity is Time. DrawIdleDashboard owns that path, so it
                // must render the same media content instead of leaving only
                // the page indicator visible.
                const D2D1_RECT_F artRect = D2D1::RectF(
                    rect.left + MediaLayout::kArtInsetX, rect.top + MediaLayout::kArtInsetY,
                    rect.left + MediaLayout::kArtInsetX + MediaLayout::kArtSize,
                    rect.top + MediaLayout::kArtInsetY + MediaLayout::kArtSize);
                DrawAlbumArt(state.media, artRect, now, 14.0f, true);

                const float volumeReveal = AdvanceMediaVolumeReveal(now);
                const float waveLeft = rect.left + MediaLayout::kVisualizerLeft;
                const float waveRight = rect.right -
                    (MediaLayout::kVisualizerRightCollapsedInset +
                     MediaLayout::kVisualizerVolumeRevealWidth * volumeReveal);
                const D2D1_RECT_F waveRect = D2D1::RectF(
                    waveLeft, rect.top + MediaLayout::kVisualizerTop,
                    waveRight, rect.top + MediaLayout::kVisualizerBottom);
                const std::wstring title = state.media.title.empty()
                    ? std::wstring(Loc(L"Unknown")) : state.media.title;
                const float titleRight = rect.right - 18.0f;
                DrawMarqueeText(title,
                    D2D1::RectF(waveLeft, rect.top + 12.0f, titleRight, rect.top + 34.0f),
                    textFormat_.Get(), textBrush_.Get(), now, 42.0f, marqueeTitleCache_, true);
                std::wstring subtitle = state.media.artist;
                if (!state.media.albumTitle.empty()) {
                    if (!subtitle.empty()) subtitle += L"  —  ";
                    subtitle += state.media.albumTitle;
                }
                if (!subtitle.empty()) {
                    DrawMarqueeText(subtitle,
                        D2D1::RectF(waveLeft, rect.top + 34.0f, titleRight, rect.top + 49.0f),
                        smallTextFormat_.Get(), mutedBrush_.Get(), now, 30.0f, marqueeArtistCache_, true);
                }
                if (settings.musicVisualizer) {
                    DrawWaveform(state, waveRect, true);
                } else {
                    ComPtr<ID2D1SolidColorBrush> idleWaveBrush;
                    target_->CreateSolidColorBrush(WithAlpha(material_.textSecondary, 0.68f),
                                                    &idleWaveBrush);
                    const float gap = waveRect.right - waveRect.left > 60.0f ? 3.0f : 2.0f;
                    const int count = std::max(1, static_cast<int>(
                        (waveRect.right - waveRect.left + gap) / (2.5f + gap)));
                    const float barWidth = (waveRect.right - waveRect.left - gap * (count - 1)) / count;
                    for (int bar = 0; bar < count && idleWaveBrush; ++bar) {
                        const float x = waveRect.left + bar * (barWidth + gap);
                        target_->FillRoundedRectangle(D2D1::RoundedRect(
                            D2D1::RectF(x, waveRect.top + 7.0f, x + barWidth, waveRect.bottom - 7.0f),
                            1.2f, 1.2f), idleWaveBrush.Get());
                    }
                }

                const int64_t mediaStartTicks = state.media.startTicks;
                const int64_t mediaDurationTicks = std::max<int64_t>(
                    0, state.media.endTicks - mediaStartTicks);
                const double duration = mediaDurationTicks / 10000000.0;
                double position = state.media.positionTicks / 10000000.0;
                if (state.media.playing && state.media.lastUpdatedTicks > 0) {
                    position += (GetTickCount64() - state.media.lastUpdatedTicks) / 1000.0;
                }
                position = std::max(0.0, std::min(
                    position - mediaStartTicks / 10000000.0, duration));
                const float progress = duration > 0.0
                    ? Clamp(static_cast<float>(position / duration), 0.0f, 1.0f) : 0.0f;
                if (duration > 0.0 && smallTextFormat_) {
                    wchar_t elapsedBuf[24] = {}, remainingBuf[24] = {};
                    const int elapsedSeconds = static_cast<int>(position);
                    const int remainingSeconds = std::max(0, static_cast<int>(duration - position));
                    swprintf_s(elapsedBuf, L"%d:%02d", elapsedSeconds / 60, elapsedSeconds % 60);
                    swprintf_s(remainingBuf, L"-%d:%02d", remainingSeconds / 60, remainingSeconds % 60);
                    const float trackLeft = rect.left + 24.0f;
                    const float trackRight = rect.right - 24.0f;
                    const float trackY = rect.top + MediaLayout::kScrubberY;
                    const D2D1_RECT_F elapsedRect = D2D1::RectF(trackLeft, trackY - 9.0f,
                                                                 trackLeft + 38.0f, trackY + 9.0f);
                    const D2D1_RECT_F remainingRect = D2D1::RectF(trackRight - 38.0f, trackY - 9.0f,
                                                                   trackRight, trackY + 9.0f);
                    smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    mutedBrush_->SetOpacity(0.78f);
                    target_->DrawTextW(elapsedBuf, static_cast<UINT32>(wcslen(elapsedBuf)),
                                       smallTextFormat_.Get(), elapsedRect, mutedBrush_.Get(),
                                       D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    target_->DrawTextW(remainingBuf, static_cast<UINT32>(wcslen(remainingBuf)),
                                       smallTextFormat_.Get(), remainingRect, mutedBrush_.Get(),
                                       D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                    mutedBrush_->SetOpacity(0.75f);
                }
                DrawAccentTrack(D2D1::RectF(rect.left + MediaLayout::kScrubInsetLeft,
                                             rect.top + MediaLayout::kScrubberY - 2.5f,
                                             rect.right - MediaLayout::kScrubInsetRight,
                                             rect.top + MediaLayout::kScrubberY + 2.5f),
                                progress, 2.5f);
                const float centerX = (rect.left + rect.right) * 0.5f;
                const float controlsY = rect.top + MediaLayout::kControlsY;
                DrawMediaControls(state.media.playing,
                    D2D1::Point2F(centerX - MediaLayout::kControlSpacing, controlsY),
                    D2D1::Point2F(centerX, controlsY),
                    D2D1::Point2F(centerX + MediaLayout::kControlSpacing, controlsY), now);
                DrawMediaVolumeControl(state, rect, volumeReveal);
            }
            else if (activeTabId == 2) DrawClipboardDashboard(state, rect);
            else if (activeTabId == 3) DrawCalendarDashboard(state, rect, settings, now, scale, local);
            else if (activeTabId == 4) DrawHardwareMonitorDashboard(state, rect, settings, scale);
            else if (activeTabId == 5) DrawFileTrayDashboard(state, rect, settings, scale);
            else if (activeTabId == 8) DrawQuickAccessDashboard(settings, rect);

            DrawContentDragPreview();
            // Pagination dots (Vertical on the right edge)
            if (maxTabs > 1) {
                const float dotX = rect.right - 10.0f * scale;
                const float dotY = (rect.top + rect.bottom) * 0.5f;
                const float spacing = 8.0f * scale;
                const float r = 2.5f * scale;

                ComPtr<ID2D1SolidColorBrush> activeDot, inactiveDot;
                target_->CreateSolidColorBrush(WithAlpha(material_.textPrimary, 0.90f * settingsOpacity_), &activeDot);
                target_->CreateSolidColorBrush(WithAlpha(material_.textPrimary, 0.22f * settingsOpacity_), &inactiveDot);

                float startY = dotY - (spacing * (maxTabs - 1)) * 0.5f;
                for (int i = 0; i < maxTabs; ++i) {
                    target_->FillEllipse(
                        D2D1::Ellipse(D2D1::Point2F(dotX, startY + spacing * i), r, r),
                        (i == tabIdx) ? activeDot.Get() : inactiveDot.Get()
                    );
                }
            }

            if (dashFadeLayer) {
                target_->PopLayer();
            }
        }

        if (haveDashMask) target_->PopLayer(); else target_->PopAxisAlignedClip();
    }

    void DrawGameOverlay(const SharedState& state, D2D1_RECT_F rect, float unused_scale) {
        (void)unused_scale;
        const float scale = 1.0f;
        const bool compact = g_settings.gameOverlayCompact;
        const GameOverlayLayout::Metrics m = GameOverlayLayout::For(compact);

        const float cardTop = rect.top + m.padY;
        const float cardBottom = rect.bottom - m.padY;
        float cursorX = rect.left + m.padX;

        // ── FPS, given hero treatment ────────────────────────────────────────
        // Frame rate is the number a player actually watches, so it gets a wider
        // card and a larger figure than the percentages beside it. It has no
        // 0-100 ceiling, so it gets no load bar and keeps the plain accent.
        if (g_settings.gameOverlayShowFps) {
            const D2D1_RECT_F fpsCard = D2D1::RectF(cursorX, cardTop, cursorX + m.fpsW, cardBottom);
            DrawCard(fpsCard, m.radius);

            ComPtr<ID2D1SolidColorBrush> fpsIcon;
            if (SUCCEEDED(target_->CreateSolidColorBrush(
                    WithAlpha(material_.accent, 0.90f * settingsOpacity_), &fpsIcon)) && fpsIcon) {
                DrawMetricGlyph(D2D1::Point2F(fpsCard.left + 15.0f * scale, fpsCard.top + 15.0f * scale),
                                15.0f * scale, 9, fpsIcon.Get());
            }

            const float textLeft = fpsCard.left + 28.0f * scale;
            const float textRight = fpsCard.right - 9.0f * scale;

            if (g_settings.showMetricText && smallTextFormat_) {
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                mutedBrush_->SetOpacity(0.58f);
                target_->DrawTextW(L"FPS", 3, smallTextFormat_.Get(),
                                   D2D1::RectF(textLeft, fpsCard.top + 2.0f * scale,
                                               textRight, fpsCard.top + 16.0f * scale),
                                   mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            wchar_t fpsValue[16] = {};
            swprintf_s(fpsValue, L"%d", state.system.renderFps);
            IDWriteTextFormat* fpsFmt = clockFormat_ ? clockFormat_.Get() : textFormat_.Get();
            if (fpsFmt) {
                // clockFormat_ is centre-aligned by default; the strip reads as a
                // left-aligned column, so override for this draw and restore.
                fpsFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                textBrush_->SetOpacity(0.97f);
                // Shares the label's left margin so the glyph sits in its own
                // column and the text column has one straight edge, rather than
                // the value hanging out to the left of the label above it.
                const float top = g_settings.showMetricText ? fpsCard.top + 16.0f * scale : cardTop;
                target_->DrawTextW(fpsValue, static_cast<UINT32>(wcslen(fpsValue)), fpsFmt,
                                   D2D1::RectF(textLeft, top,
                                               textRight, cardBottom - 3.0f * scale),
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                fpsFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            }

            cursorX = fpsCard.right + m.fpsGap;
        }

        // ── Load cards ───────────────────────────────────────────────────────
        // Laid out from whichever metrics are enabled (#25), so turning some off
        // closes the gap instead of leaving a hole. Icons come from the shared
        // DrawMetricGlyph family, so CPU here is the same symbol as CPU on the
        // hardware dashboard.
        struct GameCard {
            const wchar_t* label;
            int percent;
            int glyph;
            bool enabled;
        };
        const GameCard cards[] = {
            {L"CPU", state.system.cpuPercent,              3, g_settings.gameOverlayShowCpu},
            {L"RAM", state.system.memoryPercent,           4, g_settings.gameOverlayShowRam},
            {L"GPU", state.system.gpuPercent,              5, g_settings.gameOverlayShowGpu},
            {L"DISK", 100 - state.system.diskFreePercent,  8, g_settings.gameOverlayShowDisk},
        };

        for (const GameCard& card : cards) {
            if (!card.enabled) {
                continue;
            }
            if (cursorX + m.cardW > rect.right - m.padX + 0.5f) {
                break;  // ran out of room
            }
            DrawGameMetricCard(D2D1::RectF(cursorX, cardTop, cursorX + m.cardW, cardBottom),
                               card.label, card.percent, card.glyph, m.radius);
            cursorX += m.cardW + m.gap;
        }

        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawGameMetricCard(D2D1_RECT_F rect, const wchar_t* label, int percent, int glyph, float radius) {
        const float scale = 1.0f;

        // Same semantic load colour the hardware dashboard uses: accent while a
        // component is comfortable, amber under pressure, red when saturated.
        // This replaces a per-metric rainbow (cyan CPU, magenta RAM, green GPU,
        // orange disk) that encoded nothing and fought the album-art accent.
        const bool known = percent >= 0;
        const float pct = known ? Clamp(percent / 100.0f, 0.0f, 1.0f) : 0.0f;
        const D2D1_COLOR_F tint = known ? LoadStateColor(pct) : material_.accent;

        // Fill only. This card used to carry a hairline border *and* a
        // metric-coloured ring on top of it -- two bright outlines per card,
        // four cards across, which is the edge lighting this design drops.
        DrawCard(rect, radius);

        ComPtr<ID2D1SolidColorBrush> glyphBrush;
        if (SUCCEEDED(target_->CreateSolidColorBrush(
                WithAlpha(tint, 0.85f * settingsOpacity_), &glyphBrush)) && glyphBrush) {
            DrawMetricGlyph(D2D1::Point2F(rect.left + 15.0f * scale, rect.top + 15.0f * scale),
                            14.0f * scale, glyph, glyphBrush.Get());
        }

        const float textLeft = rect.left + 28.0f * scale;
        const float textRight = rect.right - 8.0f * scale;

        // Label above value above bar, matching the hardware dashboard exactly.
        if (g_settings.showMetricText && smallTextFormat_) {
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            mutedBrush_->SetOpacity(0.58f);
            target_->DrawTextW(label, static_cast<UINT32>(wcslen(label)), smallTextFormat_.Get(),
                               D2D1::RectF(textLeft, rect.top + 2.0f * scale,
                                           textRight, rect.top + 16.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        wchar_t value[16] = {};
        if (known) {
            swprintf_s(value, L"%d%%", percent);
        } else {
            wcscpy_s(value, ARRAYSIZE(value), L"--");
        }

        if (textFormat_) {
            textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            textBrush_->SetOpacity(0.95f);
            // Aligned with the label above it, so the glyph owns the left column
            // and label/value share one straight edge.
            const float valueTop = g_settings.showMetricText ? rect.top + 17.0f * scale
                                                             : rect.top + 4.0f * scale;
            target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                               D2D1::RectF(textLeft, valueTop,
                                           textRight, rect.bottom - 8.0f * scale),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        // The bar spans the card rather than just the text column: at this size it
        // reads as the card's own meter, and a 38px stub under the value did not.
        const D2D1_RECT_F track = D2D1::RectF(rect.left + 13.0f * scale, rect.bottom - 7.0f * scale,
                                              rect.right - 13.0f * scale, rect.bottom - 4.5f * scale);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        if (SUCCEEDED(target_->CreateSolidColorBrush(material_.raisedStrong, &trackBrush)) && trackBrush) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1.25f * scale, 1.25f * scale), trackBrush.Get());
        }
        const float span = (track.right - track.left) * pct;
        if (span > 0.5f) {
            ComPtr<ID2D1SolidColorBrush> fillBrush;
            if (SUCCEEDED(target_->CreateSolidColorBrush(WithAlpha(tint, 0.95f), &fillBrush)) && fillBrush) {
                target_->FillRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(track.left, track.top, track.left + span, track.bottom),
                                      1.25f * scale, 1.25f * scale),
                    fillBrush.Get());
            }
        }

        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    // Publishes the content-space rect currently being painted so
    // OverlayWndProc hit-tests against the real geometry rather than assuming
    // the pill is centered in the client area. Seqlock write: bump to odd,
    // store, bump to even, so a reader can never mix two frames.
    void PublishContentGeometry(D2D1_RECT_F rect) {
        const unsigned seq = g_mediaHitSeq.load(std::memory_order_relaxed);
        g_mediaHitSeq.store(seq + 1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);

        g_mediaHitLeft.store(rect.left, std::memory_order_relaxed);
        g_mediaHitTop.store(rect.top, std::memory_order_relaxed);
        g_mediaHitRight.store(rect.right, std::memory_order_relaxed);
        g_mediaHitBottom.store(rect.bottom, std::memory_order_relaxed);
        g_mediaHitScale.store(g_settings.sizeScale * g_settings.textScale,
                              std::memory_order_relaxed);
        g_mediaHitStamp.store(GetTickCount64(), std::memory_order_relaxed);

        std::atomic_thread_fence(std::memory_order_release);
        g_mediaHitSeq.store(seq + 2, std::memory_order_relaxed);
    }

    // Corner radius for the island mask in *content* space -- the coordinate
    // system DrawMedia and DrawIdleDashboard are handed, which DrawPill scales
    // by sizeScale afterwards. Because that scale is applied later, these radii
    // must not be pre-multiplied by sizeScale. They previously were, so the
    // stadium corner arc grew with the size scale until it cut across the album
    // art's top-left corner (visible from roughly 2x upwards).
    float ContentIslandRadius(float contentHeight) const {
        const float requestedRadius = g_settings.w11Style
            ? std::min(contentHeight * 0.22f, g_settings.cornerRadius)
            : g_settings.cornerRadius;
        return std::min(contentHeight * 0.5f, requestedRadius);
    }

    float AdvanceMediaVolumeReveal(double now) {
        const bool expanded = g_hoveredMediaVolume.load(std::memory_order_relaxed);
        const float targetReveal = expanded ? 1.0f : 0.0f;
        const double elapsed = lastMediaVolumeDrawTime_ < 0.0
            ? 0.0 : Clamp(static_cast<float>(now - lastMediaVolumeDrawTime_), 0.0f, 0.05f);
        lastMediaVolumeDrawTime_ = now;
        const float easing = 1.0f - std::exp(-static_cast<float>(elapsed) / 0.095f);
        mediaVolumeReveal_ += (targetReveal - mediaVolumeReveal_) * easing;
        if (std::fabs(targetReveal - mediaVolumeReveal_) < 0.005f) mediaVolumeReveal_ = targetReveal;
        return Clamp(mediaVolumeReveal_, 0.0f, 1.0f);
    }

    void DrawMediaVolumeControl(const SharedState& state, D2D1_RECT_F rect, float reveal) {
        const float right = rect.right - 16.0f;
        const float centerY = rect.top + MediaLayout::kVolumeCenterY;
        const bool muted = state.muted || state.system.volumeMuted || state.system.volumePercent <= 0;
        const wchar_t* glyph = muted ? L"\uE74F" : L"\uE767";
        // Keep the icon anchored at the right edge while the slider reveals to
        // its left. Moving the glyph with the animated shell made it appear to
        // jump during wheel input and while the reveal spring was settling.
        const D2D1_RECT_F iconRect = D2D1::RectF(right - 30.0f, centerY - 11.0f,
                                                  right - 8.0f, centerY + 11.0f);
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            ComPtr<ID2D1SolidColorBrush> iconBrush;
            const D2D1_COLOR_F iconColor = muted
                ? MixColor(material_.textPrimary, currentAccent_, 0.82f)
                : material_.textPrimary;
            target_->CreateSolidColorBrush(iconColor, &iconBrush);
            target_->DrawTextW(glyph, 1, iconFormat_.Get(), iconRect,
                               iconBrush ? iconBrush.Get() : textBrush_.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        if (reveal > 0.09f) {
            const float left = right - 160.0f * reveal;
            const float trackRight = right - 36.0f;
            const float y = centerY + 1.0f;
            const float fraction = Clamp(state.system.volumePercent / 100.0f, 0.0f, 1.0f);
            const D2D1_RECT_F track = D2D1::RectF(left, y - 2.5f, trackRight, y + 2.5f);
            ComPtr<ID2D1SolidColorBrush> trackBrush, fillBrush, thumb;
            target_->CreateSolidColorBrush(material_.raisedStrong, &trackBrush);
            const D2D1_COLOR_F levelAccent = MixColor(
                D2D1::ColorF(currentAccent_.r * 0.32f, currentAccent_.g * 0.32f,
                             currentAccent_.b * 0.32f, 1.0f), currentAccent_, fraction);
            target_->CreateSolidColorBrush(WithAlpha(levelAccent, settingsOpacity_), &fillBrush);
            if (trackBrush) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2.5f, 2.5f), trackBrush.Get());
            }
            const float fillRight = left + (trackRight - left) * fraction;
            if (fillBrush && fillRight > left + 0.5f) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(
                    D2D1::RectF(left, y - 2.5f, fillRight, y + 2.5f), 2.5f, 2.5f), fillBrush.Get());
            }
            target_->CreateSolidColorBrush(material_.textPrimary, &thumb);
            if (thumb) {
                const float thumbX = left + (trackRight - left) * fraction;
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, y), 5.0f, 5.0f), thumb.Get());
            }
        }
    }

    void DrawContentDragPreview() {
        if (!g_contentDragActive || !target_ || !textFormat_) return;
        const float x = static_cast<float>(g_contentDragPoint.x) + 12.0f;
        const float y = static_cast<float>(g_contentDragPoint.y) + 12.0f;
        const float pulse = 1.0f + 0.025f * std::sin(
            static_cast<float>(GetTickCount64() % 520) * 6.2831853f / 520.0f);
        const D2D1_RECT_F preview = D2D1::RectF(x, y, x + 154.0f * pulse, y + 42.0f * pulse);
        ComPtr<ID2D1SolidColorBrush> surface, edge;
        target_->CreateSolidColorBrush(WithAlpha(material_.base, 0.99f), &surface);
        target_->CreateSolidColorBrush(currentAccent_, &edge);
        if (surface) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(preview, 12.0f, 12.0f), surface.Get());
            if (edge) target_->DrawRoundedRectangle(D2D1::RoundedRect(preview, 12.0f, 12.0f),
                                                     edge.Get(), 2.0f);
            const std::wstring label = g_contentDragKind == 1
                ? std::wstring(L"Clipboard item")
                : std::wstring(L"Holding: ") + BaseNameFromPath(g_contentDragFilePath);
            textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()), textFormat_.Get(),
                               preview, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
    }

    // Takes settings by reference from Render()'s private copy rather than
    // reaching for g_settings. The dashboards it delegates to read std::wstring
    // members (DrawCalendarDashboard -> settings.dateFormat), which would
    // otherwise race with LoadSettings() replacing the struct mid-frame.
    void DrawMedia(const SharedState& state, D2D1_RECT_F rect, const Settings& settings,
                   double now) {
        const float height = rect.bottom - rect.top;

        PublishContentGeometry(rect);

        const float radius = ContentIslandRadius(height);
        ComPtr<ID2D1Geometry> mask = CreateIslandMaskGeometry(rect, radius, settings.notchStyle);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(&layer);

        float expandedAlpha = std::clamp((height - MediaLayout::kExpandedMinHeight) / 60.0f, 0.0f, 1.0f);
        float collapsedAlpha = std::clamp((80.0f - height) / 30.0f, 0.0f, 1.0f);

        // Expanded UI
        if (expandedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), expandedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());

            const std::vector<int> activeTabs = DashboardSectionIds(settings, state);

            // Same settings object as the list above, so the index can't be
            // normalised against a tab set that no longer matches.
            int maxTabs = static_cast<int>(activeTabs.size());
            int tabIdx = g_idleTab.load(std::memory_order_relaxed);
            tabIdx = ((tabIdx % maxTabs) + maxTabs) % maxTabs;
            int activeTabId = activeTabs[tabIdx];

            if (activeTabId == 0) {
                SYSTEMTIME local = {};
                GetLocalTime(&local);
                DrawTimeDashboard(state, rect, settings, now, 1.0f, local);
            } else if (activeTabId == 1 || activeTabId == 6) {
                // Expanded Apple DI media: large square art on left, text center.
                const float artSize = MediaLayout::kArtSize;
                D2D1_RECT_F artRect = D2D1::RectF(rect.left + MediaLayout::kArtInsetX,
                                                  rect.top + MediaLayout::kArtInsetY,
                                                  rect.left + MediaLayout::kArtInsetX + artSize,
                                                  rect.top + MediaLayout::kArtInsetY + artSize);
                DrawAlbumArt(state.media, artRect, now, 16.0f, true);

                const float volumeReveal = AdvanceMediaVolumeReveal(now);
                const float waveLeft = rect.left + MediaLayout::kVisualizerLeft;
                const float waveRight = rect.right -
                    (MediaLayout::kVisualizerRightCollapsedInset +
                     MediaLayout::kVisualizerVolumeRevealWidth * volumeReveal);
                D2D1_RECT_F waveRect = D2D1::RectF(waveLeft,
                                                   rect.top + MediaLayout::kVisualizerTop,
                                                   waveRight,
                                                   rect.top + MediaLayout::kVisualizerBottom);
                const float textLeft = rect.left + MediaLayout::kVisualizerLeft;
                const float textRight = rect.right - 18.0f;

                // Title — bold, prominent.
                D2D1_RECT_F titleRect = D2D1::RectF(textLeft, rect.top + 12.0f, textRight, rect.top + 34.0f);
                DrawMarqueeText(state.media.title.empty() ? std::wstring(Loc(L"Unknown")) : state.media.title,
                                titleRect, textFormat_.Get(), textBrush_.Get(), now, 42.0f, marqueeTitleCache_, true);

                // Artist — muted below title.
                D2D1_RECT_F artistRect = D2D1::RectF(textLeft, rect.top + 34.0f, textRight, rect.top + 49.0f);
                mutedBrush_->SetOpacity(0.80f);
                std::wstring subtitle = state.media.artist;
                if (!state.media.albumTitle.empty()) {
                    if (!subtitle.empty()) subtitle += L"  —  ";
                    subtitle += state.media.albumTitle;
                }
                DrawMarqueeText(subtitle,
                                artistRect, smallTextFormat_.Get(), mutedBrush_.Get(), now, 30.0f, marqueeArtistCache_, true);
                mutedBrush_->SetOpacity(0.75f);

                if (settings.musicVisualizer) {
                    DrawWaveform(state, waveRect, true);
                } else {
                    const float gap = 2.5f;
                    const float availableW = waveRect.right - waveRect.left;
                    const int count = std::max(7, static_cast<int>(
                        (availableW + gap) / (2.0f + gap)));
                    const float barWidth = (availableW - gap * (count - 1)) / count;
                    const float centerY = (waveRect.top + waveRect.bottom) * 0.5f;
                    mutedBrush_->SetOpacity(0.5f);
                    for (int i = 0; i < count; ++i) {
                        const float dotX = waveRect.left + i * (barWidth + gap) + barWidth * 0.5f;
                        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, centerY), 1.2f, 1.2f), mutedBrush_.Get());
                    }
                }

                // Timeline (Scrubber)
                const float scrubberY = rect.top + MediaLayout::kScrubberY;
                const int64_t mediaStartTicks = state.media.startTicks;
                const int64_t mediaDurationTicks = std::max<int64_t>(
                    0, state.media.endTicks - mediaStartTicks);
                double currentPosition = (state.media.positionTicks - mediaStartTicks) / 10000000.0;
                double duration = mediaDurationTicks / 10000000.0;
                if (state.media.playing && state.media.lastUpdatedTicks > 0) {
                    currentPosition += (GetTickCount64() - state.media.lastUpdatedTicks) / 1000.0;
                }
                currentPosition = std::max(0.0, std::min(currentPosition, duration));

                const bool isDraggingThisBar = g_scrubbing.load(std::memory_order_relaxed);
                float progress;
                if (isDraggingThisBar) {
                    progress = Clamp(g_scrubDragFraction.load(std::memory_order_relaxed), 0.0f, 1.0f);
                    currentPosition = duration * progress;
                } else {
                    progress = duration > 0.0 ? static_cast<float>(currentPosition / duration) : 0.0f;
                }

                auto FormatTime = [](double seconds) -> std::wstring {
                    if (seconds <= 0.0 || _isnan(seconds)) return L"0:00";
                    int m = static_cast<int>(seconds) / 60;
                    int s = static_cast<int>(seconds) % 60;
                    wchar_t buf[16];
                    swprintf_s(buf, L"%d:%02d", m, s);
                    return buf;
                };

                std::wstring elapsedStr = FormatTime(currentPosition);
                std::wstring remainStr = L"-" + FormatTime(duration - currentPosition);

                const float scrubLeft = rect.left + MediaLayout::kScrubMargin;
                const float scrubRight = rect.right - MediaLayout::kScrubMargin;

                const float barLeft = scrubLeft + MediaLayout::kScrubBarLeftInset;
                const float barRight = scrubRight - MediaLayout::kScrubBarRightInset;
                const float timeGap = 8.0f;

                D2D1_RECT_F elRect = D2D1::RectF(scrubLeft, scrubberY - 10.0f, barLeft - timeGap, scrubberY + 10.0f);
                D2D1_RECT_F remRect = D2D1::RectF(barRight + timeGap, scrubberY - 10.0f, scrubRight, scrubberY + 10.0f);

                mutedBrush_->SetOpacity(0.8f);
                if (smallTextFormat_) {
                    smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                    target_->DrawTextW(elapsedStr.c_str(), static_cast<UINT32>(elapsedStr.size()), smallTextFormat_.Get(), elRect, mutedBrush_.Get());

                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    target_->DrawTextW(remainStr.c_str(), static_cast<UINT32>(remainStr.size()), smallTextFormat_.Get(), remRect, mutedBrush_.Get());

                    smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                    smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                }

                // The scrubber uses the shared accent track, so it matches the
                // volume, battery and timer bars exactly. The bar thickens while
                // dragging, which is the standard cue that it is grabbable.
                const float barHalf = isDraggingThisBar ? 3.5f : 2.5f;
                DrawAccentTrack(D2D1::RectF(barLeft, scrubberY - barHalf, barRight, scrubberY + barHalf),
                                progress, barHalf);

                const D2D1_COLOR_F scrubColor =
                    (currentAccent_.a > 0.0f) ? currentAccent_ : D2D1::ColorF(0x4cc9f0);
                const float scrubW = (barRight - barLeft) * progress;
                const float thumbX = barLeft + scrubW;
                const float thumbR = isDraggingThisBar ? 6.5f : 4.5f;

                // Halo first so the thumb sits on top of it.
                if (isDraggingThisBar) {
                    ComPtr<ID2D1SolidColorBrush> thumbHalo;
                    if (SUCCEEDED(target_->CreateSolidColorBrush(WithAlpha(scrubColor, 0.22f), &thumbHalo)) &&
                        thumbHalo) {
                        target_->FillEllipse(
                            D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR * 2.2f, thumbR * 2.2f),
                            thumbHalo.Get());
                    }
                }

                // A white thumb with an accent ring reads more precisely against
                // album art than a solid accent dot.
                ComPtr<ID2D1SolidColorBrush> thumbFill;
                if (SUCCEEDED(target_->CreateSolidColorBrush(
                        D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.98f), &thumbFill)) && thumbFill) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR, thumbR),
                                         thumbFill.Get());
                }
                ComPtr<ID2D1SolidColorBrush> thumbRing;
                if (SUCCEEDED(target_->CreateSolidColorBrush(WithAlpha(scrubColor, 0.85f), &thumbRing)) &&
                    thumbRing) {
                    target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR, thumbR),
                                         thumbRing.Get(), 1.6f);
                }

                // Controls. Positions come from MediaLayout so the hit test in
                // OverlayWndProc stays in lockstep with what is drawn here.
                const float cy = rect.top + MediaLayout::kControlsY;
                const float cx = (rect.left + rect.right) * 0.5f;
            DrawMediaControls(state.media.playing,
                                  D2D1::Point2F(cx - MediaLayout::kControlSpacing, cy),
                                  D2D1::Point2F(cx, cy),
                                  D2D1::Point2F(cx + MediaLayout::kControlSpacing, cy),
                                  now);
                DrawMediaVolumeControl(state, rect, volumeReveal);
            } else if (activeTabId == 2) {
                DrawClipboardDashboard(state, rect);
            } else if (activeTabId == 3) {
                SYSTEMTIME local = {}; GetLocalTime(&local);
                DrawCalendarDashboard(state, rect, settings, now, 1.0f, local);
            } else if (activeTabId == 4) {
                DrawHardwareMonitorDashboard(state, rect, settings, 1.0f);
            } else if (activeTabId == 5) {
                DrawFileTrayDashboard(state, rect, settings, 1.0f);
            } else if (activeTabId == 8) {
                DrawQuickAccessDashboard(settings, rect);
            }

            DrawContentDragPreview();
            // Pagination dots (Vertical on the right edge)
            if (maxTabs > 1) {
                const float scale = 1.0f;
                const float dotX = rect.right - 10.0f * scale;
                const float dotY = (rect.top + rect.bottom) * 0.5f;
                const float spacing = 8.0f * scale;
                const float r = 2.5f * scale;

                ComPtr<ID2D1SolidColorBrush> activeDot, inactiveDot;
                target_->CreateSolidColorBrush(WithAlpha(material_.textPrimary, 0.90f * settingsOpacity_), &activeDot);
                target_->CreateSolidColorBrush(WithAlpha(material_.textPrimary, 0.22f * settingsOpacity_), &inactiveDot);

                float startY = dotY - (spacing * (maxTabs - 1)) * 0.5f;
                for (int i = 0; i < maxTabs; ++i) {
                    target_->FillEllipse(
                        D2D1::Ellipse(D2D1::Point2F(dotX, startY + spacing * i), r, r),
                        (i == tabIdx) ? activeDot.Get() : inactiveDot.Get()
                    );
                }
            }

            target_->PopLayer();
        }

        // Collapsed UI
        if (collapsedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), collapsedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());

            const float cy = (rect.top + rect.bottom) * 0.5f;
            const float artPadding = 6.0f;
            const float artSize = height - artPadding * 2.0f;

            D2D1_RECT_F artRect = D2D1::RectF(rect.left + artPadding, cy - artSize * 0.5f,
                                              rect.left + artPadding + artSize, cy + artSize * 0.5f);
            DrawAlbumArt(state.media, artRect, now, artSize * 0.5f, false);

            float shiftX = 0.0f;
            if (state.system.micActive || state.system.cameraActive) {
                shiftX = 22.0f;
            }

            D2D1_RECT_F waveRect = D2D1::RectF(rect.right - 42.0f - shiftX, cy - 10.0f,
                                               rect.right - 14.0f - shiftX, cy + 10.0f);
            if (state.media.playing) {
                DrawWaveform(state, waveRect);
            } else {
                const float gap = 2.5f;
                const float availableW = waveRect.right - waveRect.left;
                const int count = std::max(1, static_cast<int>((availableW + gap) / (2.0f + gap)));
                const float barWidth = (availableW - gap * (count - 1)) / count;
                mutedBrush_->SetOpacity(0.5f);
                for (int i = 0; i < count; ++i) {
                    const float dotX = waveRect.left + i * (barWidth + gap) + barWidth * 0.5f;
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, cy), 1.2f, 1.2f), mutedBrush_.Get());
                }
            }

            target_->PopLayer();
        }
    }

    void UpdateMediaButtonAnimations(double now) {
        float dt = 0.016f;
        if (lastMediaBtnTime_ > 0.0) {
            dt = static_cast<float>(std::max(0.001, std::min(now - lastMediaBtnTime_, 0.05)));
        }
        lastMediaBtnTime_ = now;

        const int pressedCmd = g_pressedMediaButton.load();
        bool isStillAnimating = false;

        for (int i = 0; i < 3; ++i) {
            const float target = (pressedCmd == i) ? 1.0f : 0.0f;
            const float tau = (target > mediaBtnPress_[i]) ? 0.025f : 0.075f;
            const float k = 1.0f - std::exp(-dt / tau);
            mediaBtnPress_[i] += (target - mediaBtnPress_[i]) * k;

            if (std::abs(mediaBtnPress_[i] - target) < 0.002f) {
                mediaBtnPress_[i] = target;
            } else {
                isStillAnimating = true;
            }
        }

        if (isStillAnimating) {
            g_layoutDirty = true;
        }
    }

    void DrawMediaControls(bool playing, D2D1_POINT_2F prev, D2D1_POINT_2F play, D2D1_POINT_2F next, double now) {
        UpdateMediaButtonAnimations(now);
        DrawMediaButton(prev, MediaLayout::kNavButtonRadius, 0, false);
        DrawMediaButton(play, MediaLayout::kPlayButtonRadius, playing ? 1 : 2, true);
        DrawMediaButton(next, MediaLayout::kNavButtonRadius, 3, false);
    }

    void DrawMediaButton(D2D1_POINT_2F center, float radius, int kind, bool primary) {
        int buttonCmd = (kind == 0) ? 0 : ((kind == 1 || kind == 2) ? 1 : 2);
        bool isHovered = (g_hoveredMediaButton.load() == buttonCmd);
        const float press = (buttonCmd >= 0 && buttonCmd < 3) ? mediaBtnPress_[buttonCmd] : 0.0f;

        const float r = radius * (1.0f - 0.13f * press);

        const D2D1_COLOR_F restingBg = D2D1::ColorF(
            1.0f, 1.0f, 1.0f,
            primary ? (isHovered ? 0.16f : 0.080f) : (isHovered ? 0.09f : 0.040f)
        );
        const D2D1_COLOR_F pressedBg = D2D1::ColorF(
            currentAccent_.r, currentAccent_.g, currentAccent_.b,
            primary ? 0.28f : 0.18f
        );

        const D2D1_COLOR_F currentBg = D2D1::ColorF(
            restingBg.r + (pressedBg.r - restingBg.r) * press,
            restingBg.g + (pressedBg.g - restingBg.g) * press,
            restingBg.b + (pressedBg.b - restingBg.b) * press,
            restingBg.a + (pressedBg.a - restingBg.a) * press
        );

        ComPtr<ID2D1SolidColorBrush> bg;
        target_->CreateSolidColorBrush(currentBg, &bg);
        target_->FillEllipse(D2D1::Ellipse(center, r, r), bg.Get());

        const float baseOpacity = primary ? (isHovered ? 1.0f : 0.88f) : (isHovered ? 0.92f : 0.62f);
        const float iconOpacity = Clamp(baseOpacity + (1.0f - baseOpacity) * press, 0.0f, 1.0f);
        accentBrush_->SetOpacity(iconOpacity);

        const wchar_t* glyph = nullptr;
        IDWriteTextFormat* fmt = nullptr;
        if (usingFluentIcons_) {
            if (kind == 0) {
                glyph = L"\uF8AC";
                fmt = mediaNavIconFormat_.Get();
            } else if (kind == 1) {
                glyph = L"\uF8AE";
                fmt = mediaPlayIconFormat_.Get();
            } else if (kind == 2) {
                glyph = L"\uF5B0";
                fmt = mediaPlayIconFormat_.Get();
            } else if (kind == 3) {
                glyph = L"\uF8AD";
                fmt = mediaNavIconFormat_.Get();
            }
        } else {
            if (kind == 0) {
                glyph = L"\uE100";
                fmt = mediaNavIconFormat_.Get();
            } else if (kind == 1) {
                glyph = L"\uE103";
                fmt = mediaPlayIconFormat_.Get();
            } else if (kind == 2) {
                glyph = L"\uE102";
                fmt = mediaPlayIconFormat_.Get();
            } else if (kind == 3) {
                glyph = L"\uE101";
                fmt = mediaNavIconFormat_.Get();
            }
        }

        if (glyph && fmt) {
            const float offsetX = (kind == 2) ? 1.0f : 0.0f;
            D2D1_RECT_F glyphRect = D2D1::RectF(
                center.x - radius + offsetX,
                center.y - radius,
                center.x + radius + offsetX,
                center.y + radius
            );

            if (press > 0.001f) {
                D2D1_MATRIX_3X2_F oldTransform;
                target_->GetTransform(&oldTransform);
                const float scaleFactor = 1.0f - 0.13f * press;
                target_->SetTransform(D2D1::Matrix3x2F::Scale(scaleFactor, scaleFactor, center) * oldTransform);
                target_->DrawTextW(glyph, 1, fmt, glyphRect, accentBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
                target_->SetTransform(oldTransform);
            } else {
                target_->DrawTextW(glyph, 1, fmt, glyphRect, accentBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
        }

        accentBrush_->SetOpacity(1.0f);
    }

    void DrawAlbumArt(const MediaSnapshot& media, D2D1_RECT_F rect, double now, float radius = 9.0f, bool drawBadge = true) {
        UNREFERENCED_PARAMETER(now);
        UNREFERENCED_PARAMETER(drawBadge);
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        HRESULT hrMask = d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        HRESULT hrLayer = target_->CreateLayer(nullptr, &layer);
        const bool roundedClip = SUCCEEDED(hrMask) && SUCCEEDED(hrLayer) && mask && layer;
        if (roundedClip) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get()), layer.Get());
        } else {
            target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        }

        if (!media.art.bgra.empty()) {
            if (artGeneration_ != media.art.generation || !artBitmap_) {
                D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                target_->CreateBitmap(D2D1::SizeU(media.art.width, media.art.height),
                                      media.art.bgra.data(), media.art.width * 4,
                                      &props, &artBitmap_);
                artGeneration_ = media.art.generation;
            }

            D2D1_RECT_F dst = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
            target_->DrawBitmap(artBitmap_.Get(), dst, 1.0f,
                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            accentBrush_->SetOpacity(0.24f);
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
            // SMTC sometimes provides no media thumbnail. Do not substitute
            // the source application's logo: use a neutral music/video glyph.
            const wchar_t* glyph = media.isVideo ? L"\u25B6" : L"\u266B";
            if (iconFormat_) {
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(glyph, 1, iconFormat_.Get(), rect,
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
        }

        if (roundedClip) {
            target_->PopLayer();
        } else {
            target_->PopAxisAlignedClip();
        }
    }

    void DrawBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F rect,
                          ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                          float opacity = 1.0f) {
        if (pixels.bgra.empty()) {
            return;
        }

        if (cachedGeneration != pixels.generation || !cache) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            target_->CreateBitmap(D2D1::SizeU(pixels.width, pixels.height),
                                  pixels.bgra.data(), pixels.width * 4,
                                  &props, &cache);
            cachedGeneration = pixels.generation;
        }

        if (cache) {
            target_->DrawBitmap(cache.Get(), rect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    // Draws a bitmap filling a rounded-rect badge with a small inset for polish.
    void DrawRoundedBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F badge,
                                 float cornerRadius,
                                 ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                 float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;

        // 2px inset so the icon has clean edges inside the badge.
        const float pad = 2.0f;
        D2D1_RECT_F iconRect = D2D1::RectF(badge.left + pad, badge.top + pad,
                                           badge.right - pad, badge.bottom - pad);
        const float innerR = std::max(0.0f, cornerRadius - pad);

        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(iconRect, innerR, innerR), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (mask && layer) {
            target_->PushLayer(
                D2D1::LayerParameters(iconRect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                layer.Get());
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
        }
    }

    // Like DrawRoundedBitmapPixels, but center-crops (cover-fit) instead of
    // stretching, so a non-square clipboard image isn't squashed into the
    // square badge.
    void DrawCoverFitBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F badge,
                                  float cornerRadius,
                                  ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                  float opacity = 1.0f) {
        if (pixels.bgra.empty() || !pixels.width || !pixels.height) return;

        const float pad = 2.0f;
        D2D1_RECT_F iconRect = D2D1::RectF(badge.left + pad, badge.top + pad,
                                           badge.right - pad, badge.bottom - pad);
        const float innerR = std::max(0.0f, cornerRadius - pad);
        const float boxW = iconRect.right - iconRect.left;
        const float boxH = iconRect.bottom - iconRect.top;
        if (boxW <= 0.0f || boxH <= 0.0f) return;

        const float srcAspect = static_cast<float>(pixels.width) / static_cast<float>(pixels.height);
        const float boxAspect = boxW / boxH;
        float drawW = boxW;
        float drawH = boxH;
        if (srcAspect > boxAspect) {
            drawH = boxH;
            drawW = boxH * srcAspect;
        } else {
            drawW = boxW;
            drawH = boxW / srcAspect;
        }
        const float cx = (iconRect.left + iconRect.right) * 0.5f;
        const float cy = (iconRect.top + iconRect.bottom) * 0.5f;
        D2D1_RECT_F drawRect = D2D1::RectF(cx - drawW * 0.5f, cy - drawH * 0.5f,
                                           cx + drawW * 0.5f, cy + drawH * 0.5f);

        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(iconRect, innerR, innerR), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (mask && layer) {
            target_->PushLayer(
                D2D1::LayerParameters(iconRect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                layer.Get());
            DrawBitmapPixels(pixels, drawRect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, drawRect, cache, cachedGeneration, opacity);
        }
    }

    void DrawCircularBitmapPixels(const BitmapPixels& pixels, D2D1_POINT_2F center, float radius,
                                  ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                  float opacity = 1.0f) {
        if (pixels.bgra.empty()) {
            return;
        }

        D2D1_RECT_F rect = D2D1::RectF(center.x - radius, center.y - radius,
                                      center.x + radius, center.y + radius);
        ComPtr<ID2D1EllipseGeometry> ellipse;
        d2dFactory_->CreateEllipseGeometry(D2D1::Ellipse(center, radius, radius), &ellipse);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (ellipse && layer) {
            target_->PushLayer(D2D1::LayerParameters(
                                  rect, ellipse.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                              layer.Get());
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
        }

        ComPtr<ID2D1SolidColorBrush> border;
        target_->CreateSolidColorBrush(material_.hairline, &border);
        if (border) {
            target_->DrawEllipse(D2D1::Ellipse(center, radius, radius), border.Get(), 1.0f);
        }
    }

    void DrawMarqueeText(const std::wstring& text, D2D1_RECT_F rect, IDWriteTextFormat* format,
                         ID2D1Brush* brush, double now, float speed, MarqueeLayoutCache& cache,
                         bool centerWhenFits = false) {
        if (!format || !brush || text.empty()) {
            return;
        }

        const float wrapHeight = rect.bottom - rect.top;
        // Only rebuild the layout when text/format/height actually changed.
        // Scroll offset is applied via the translated draw origin below, so
        // it never invalidates the cache — this is what lets the marquee
        // scroll every frame without calling CreateTextLayout every frame.
        if (cache.text != text || cache.format != format ||
            std::fabs(cache.wrapWidth - wrapHeight) > 0.01f || !cache.layout) {
            cache.layout.Reset();
            dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()),
                                             format, 2000.0f, wrapHeight, &cache.layout);
            cache.text = text;
            cache.format = format;
            cache.wrapWidth = wrapHeight;
            cache.metrics = {};
            if (cache.layout) {
                cache.layout->GetMetrics(&cache.metrics);
            }
        }

        if (!cache.layout) {
            return;
        }

        const float available = rect.right - rect.left;

        D2D1_RECT_F clipRect = rect;
        clipRect.top -= 10.0f;
        clipRect.bottom += 10.0f;
        target_->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        if (cache.metrics.widthIncludingTrailingWhitespace <= available) {
            const float x = centerWhenFits
                ? rect.left + (available - cache.metrics.widthIncludingTrailingWhitespace) * 0.5f
                : rect.left;
            target_->DrawTextLayout(D2D1::Point2F(x, rect.top), cache.layout.Get(), brush,
                                    D2D1_DRAW_TEXT_OPTIONS_NONE);
        } else {
            const float cycle = cache.metrics.widthIncludingTrailingWhitespace + 38.0f;
            const float offset = std::fmod(static_cast<float>(now) * speed, cycle);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset, rect.top), cache.layout.Get(),
                                    brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset + cycle, rect.top),
                                    cache.layout.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        target_->PopAxisAlignedClip();
    }

    void DrawWaveform(const SharedState& state, D2D1_RECT_F rect, bool animate = true) {
        const float width = rect.right - rect.left;
        const float height = rect.bottom - rect.top;
        const bool mirroredBars = (g_visualizerStyle.load(std::memory_order_relaxed) & 1) == 0;
        const size_t count = static_cast<size_t>(ClampInt(static_cast<int>(width / 4.5f), 8, 32));
        const float centerY = (rect.top + rect.bottom) * 0.5f;
        const float strength = Clamp(g_visualizerStrength.load(std::memory_order_relaxed) / 100.0f,
                                      0.1f, 2.0f);
        float hue = 0.0f, saturation = 0.0f, lightness = 0.0f;
        RgbToHsl(currentAccent_.r, currentAccent_.g, currentAccent_.b,
                 hue, saturation, lightness);
        auto colorAt = [&](size_t i) {
            const float shift = count > 1
                ? -28.0f + 56.0f * static_cast<float>(i) / static_cast<float>(count - 1) : 0.0f;
            return HslToRgb(hue + shift, std::max(0.35f, saturation),
                            Clamp(lightness + 0.08f, 0.25f, 0.78f));
        };
        auto amplitudeAt = [&](size_t i) {
            const size_t offset = (count - i) * 4;
            const size_t source = (state.waveformWrite + state.waveform.size() - offset) %
                                  state.waveform.size();
            const float sample = animate ? Clamp(state.waveform[source], 0.0f, 1.0f)
                : 0.20f + 0.20f * (0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.73f));
            return Clamp(sample * strength, 0.0f, 1.0f);
        };

        if (mirroredBars) {
            const float gap = 2.5f;
            const float barWidth = (width - gap * (count - 1)) / count;
            const float maxHeight = height * 0.43f;
            for (size_t i = 0; i < count; ++i) {
                const float amp = amplitudeAt(i);
                const float h = std::max(2.0f, amp * maxHeight);
                const float x = rect.left + i * (barWidth + gap);
                D2D1_RECT_F bar = D2D1::RectF(x, centerY - h, x + barWidth, centerY + h);
                accentBrush_->SetColor(colorAt(i));
                accentBrush_->SetOpacity(0.45f + 0.5f * amp);
                target_->FillRoundedRectangle(D2D1::RoundedRect(
                    bar, std::min(barWidth * 0.5f, 2.0f), std::min(barWidth * 0.5f, 2.0f)),
                    accentBrush_.Get());
            }
        } else {
            // Minimal waveform: retain the existing final style, with amplitude
            // now controlled by the user setting and the current system volume.
            const float stepX = width / static_cast<float>(count - 1);
            for (size_t i = 1; i < count; ++i) {
                const float a0 = amplitudeAt(i - 1), a1 = amplitudeAt(i);
                const float y0 = centerY - (a0 - 0.5f) * height * 0.46f;
                const float y1 = centerY - (a1 - 0.5f) * height * 0.46f;
                accentBrush_->SetColor(colorAt(i));
                accentBrush_->SetOpacity(0.82f);
                target_->DrawLine(D2D1::Point2F(rect.left + (i - 1) * stepX, y0),
                                  D2D1::Point2F(rect.left + i * stepX, y1),
                                  accentBrush_.Get(), 1.6f);
            }
        }
        accentBrush_->SetOpacity(1.0f);
        accentBrush_->SetColor(currentAccent_);
    }

    void DrawWaveformLegacy(const SharedState& state, D2D1_RECT_F rect, bool animate = true) {
        const float availableW = rect.right - rect.left;
        const int style = ((g_visualizerStyle.load(std::memory_order_relaxed) % 5) + 5) % 5;
        const size_t count = static_cast<size_t>(ClampInt(
            static_cast<int>(availableW / (style >= 3 ? 4.0f : 4.5f)), 8, 32));
        const float gap = style >= 3 ? 0.0f : 2.5f;
        const float barWidth = (availableW - gap * (count - 1)) / count;
        const float centerY = (rect.top + rect.bottom) * 0.5f;
        const float maxH = (rect.bottom - rect.top) * 0.43f;
        const float volumeGain = 0.12f + 0.88f *
            Clamp(state.system.volumePercent / 100.0f, 0.0f, 1.0f);

        // Colors are hue-shifted from the active theme/art accent, so all five
        // visualizers remain coordinated with the selected theme.
        float hue = 0.0f, saturation = 0.0f, lightness = 0.0f;
        RgbToHsl(currentAccent_.r, currentAccent_.g, currentAccent_.b,
                 hue, saturation, lightness);
        auto colorAt = [&](size_t i) {
            const float shift = count > 1
                ? -28.0f + 56.0f * static_cast<float>(i) / static_cast<float>(count - 1) : 0.0f;
            return HslToRgb(hue + shift, std::max(0.35f, saturation),
                            Clamp(lightness + 0.08f, 0.25f, 0.78f));
        };
        auto amplitudeAt = [&](size_t i) {
            const size_t source = (state.waveformWrite + state.waveform.size() -
                                   ((count - i) * 4 % state.waveform.size())) % state.waveform.size();
            const float sample = animate
                ? Clamp(state.waveform[source], 0.03f, 1.0f)
                : 0.20f + 0.20f * (0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.73f));
            if (style == 1) {
                // SMTC has no FFT feed; use logarithmically spaced recent
                // amplitude samples as a lightweight spectrum-style proxy.
                const float freqBias = 1.0f - 0.40f * static_cast<float>(i) /
                                                static_cast<float>(std::max<size_t>(1, count - 1));
                return Clamp(sample * freqBias * 1.24f * volumeGain, 0.04f, 1.0f);
            }
            if (style == 2) {
                const float lowFrequencyLift = 1.0f - 0.30f * static_cast<float>(i) /
                                                        static_cast<float>(std::max<size_t>(1, count - 1));
                return Clamp((sample * 0.80f + 0.12f) * lowFrequencyLift * 0.62f * volumeGain,
                             0.03f, 1.0f);
            }
            const float styleGain = style == 0 ? 0.74f : 1.0f;
            return Clamp(sample * styleGain * volumeGain, 0.0f, 1.0f);
        };

        if (style == 3) {
            // Fluid silhouette: a compact, filled pressure shape with a smooth
            // upper contour and a quieter mirrored lower contour.
            ComPtr<ID2D1PathGeometry> silhouette;
            ComPtr<ID2D1GeometrySink> sink;
            if (SUCCEEDED(d2dFactory_->CreatePathGeometry(&silhouette)) && silhouette &&
                SUCCEEDED(silhouette->Open(&sink)) && sink) {
                const float baseline = centerY;
                const float halfHeight = (rect.bottom - rect.top) * 0.40f;
                sink->BeginFigure(D2D1::Point2F(rect.left, baseline), D2D1_FIGURE_BEGIN_FILLED);
                for (size_t i = 0; i < count; ++i) {
                    const float x = rect.left + availableW * static_cast<float>(i) /
                                    static_cast<float>(count - 1);
                    const float amp = amplitudeAt(i);
                    const float y = baseline - (0.10f + amp * 0.90f) * halfHeight;
                    if (i == 0) {
                        sink->AddLine(D2D1::Point2F(x, y));
                    } else {
                        const float px = rect.left + availableW * static_cast<float>(i - 1) /
                                         static_cast<float>(count - 1);
                        const float prevAmp = amplitudeAt(i - 1);
                        const float py = baseline - (0.10f + prevAmp * 0.90f) * halfHeight;
                        const float dx = x - px;
                        sink->AddBezier(D2D1::BezierSegment(
                            D2D1::Point2F(px + dx / 3.0f, py),
                            D2D1::Point2F(px + dx * 2.0f / 3.0f, y),
                            D2D1::Point2F(x, y)));
                    }
                }
                for (size_t i = count; i > 0; --i) {
                    const size_t index = i - 1;
                    const float x = rect.left + availableW * static_cast<float>(index) /
                                    static_cast<float>(count - 1);
                    const float amp = amplitudeAt(index);
                    const float y = baseline + (0.04f + amp * 0.35f) * halfHeight;
                    if (i == count) {
                        sink->AddLine(D2D1::Point2F(x, y));
                    } else {
                        const size_t nextIndex = index + 1;
                        const float px = rect.left + availableW * static_cast<float>(nextIndex) /
                                         static_cast<float>(count - 1);
                        const float prevAmp = amplitudeAt(nextIndex);
                        const float py = baseline + (0.04f + prevAmp * 0.35f) * halfHeight;
                        const float dx = px - x;
                        sink->AddBezier(D2D1::BezierSegment(
                            D2D1::Point2F(px - dx / 3.0f, py),
                            D2D1::Point2F(px - dx * 2.0f / 3.0f, y),
                            D2D1::Point2F(x, y)));
                    }
                }
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                accentBrush_->SetColor(currentAccent_);
                accentBrush_->SetOpacity(0.86f);
                target_->FillGeometry(silhouette.Get(), accentBrush_.Get());
                accentBrush_->SetOpacity(1.0f);
            }
            accentBrush_->SetColor(currentAccent_);
            return;
        }

        for (size_t i = 0; i < count; ++i) {
            const float amplitude = amplitudeAt(i);
            const float x = rect.left + static_cast<float>(i) * (barWidth + gap);
            const float h = std::max(style >= 3 ? 1.0f : 2.0f,
                                     amplitude * maxH * (style == 1 ? 1.22f : 1.0f));
            accentBrush_->SetColor(colorAt(i));
            accentBrush_->SetOpacity(style == 4 ? 0.62f : 0.96f);

            if (style <= 2) {
                const float top = style == 0 ? centerY - h :
                                  (style == 2 ? centerY - h * 0.5f : rect.bottom - h);
                const float bottom = style == 0 ? centerY + h :
                                     (style == 2 ? centerY + h * 0.5f : rect.bottom);
                const D2D1_RECT_F bar = D2D1::RectF(x, top, x + barWidth, bottom);
                target_->FillRoundedRectangle(D2D1::RoundedRect(
                    bar, std::min(2.0f, barWidth * 0.5f), std::min(2.0f, barWidth * 0.5f)),
                    accentBrush_.Get());
            } else {
                float lineAmp = amplitude;
                if (style == 3 && i > 0 && i + 1 < count) {
                    lineAmp = (amplitudeAt(i - 1) + amplitude * 2.0f + amplitudeAt(i + 1)) * 0.25f;
                }
                const float y = centerY - (lineAmp - 0.5f) *
                    (style == 3 ? rect.bottom - rect.top : (rect.bottom - rect.top) * 0.46f);
                if (i > 0) {
                    const float prevAmp = style == 3 && i > 1
                        ? (amplitudeAt(i - 2) + amplitudeAt(i - 1) * 2.0f + amplitude) * 0.25f
                        : amplitudeAt(i - 1);
                    const float prevY = centerY - (prevAmp - 0.5f) *
                        (style == 3 ? rect.bottom - rect.top : (rect.bottom - rect.top) * 0.46f);
                    target_->DrawLine(D2D1::Point2F(rect.left + (i - 1) * (barWidth + gap), prevY),
                                      D2D1::Point2F(x, y), accentBrush_.Get(), style == 3 ? 2.2f : 1.25f);
                }
                if (style == 3) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), 1.35f, 1.35f),
                                         accentBrush_.Get());
                }
            }
        }
        accentBrush_->SetOpacity(1.0f);
        accentBrush_->SetColor(currentAccent_);
    }

    void DrawCountdownProgress(float left, float right, float bottom, float progress) {
        if (!g_settings.statusCountdownProgress) return;
        const float h = 2.5f;
        D2D1_RECT_F track = D2D1::RectF(left, bottom - h, right, bottom);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1.25f, 1.25f), trackBrush.Get());
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top,
                                       track.left + (track.right - track.left) * Clamp(progress, 0.0f, 1.0f),
                                       track.bottom);
        accentBrush_->SetOpacity(0.70f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 1.25f, 1.25f), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawClipboard(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 40.0f || rect.right - rect.left < 100.0f) return;
        const double now = NowSeconds();
        const float ttl = 2.5f;
        const float remaining = Clamp(static_cast<float>(state.clipboard.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14.0f, cy - badgeSz * 0.5f,
                                        rect.left + 14.0f + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        if (state.clipboard.image && !state.clipboard.imagePreview.bgra.empty()) {
            DrawCoverFitBitmapPixels(state.clipboard.imagePreview,
                                     badge, br,
                                     clipboardImageBitmap_,
                                     clipboardImageGeneration_, 1.0f);
        } else if (!state.clipboard.appIcon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.clipboard.appIcon,
                                    badge, br,
                                    clipboardIconBitmap_,
                                    clipboardIconGeneration_, 0.96f);
        } else {
            const wchar_t* glyph = state.clipboard.image
                ? (usingFluentIcons_ ? L"\uE91B" : L"\uE114")
                : (usingFluentIcons_ ? L"\uF0E3" : L"\uE8C8");
            textBrush_->SetOpacity(0.95f);

            if (iconFormat_) {
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(glyph,
                                   static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            textBrush_->SetOpacity(0.90f);
        }

        const float tx = badge.right + 14.0f;
        const bool showBar = g_settings.statusCountdownProgress;
        D2D1_RECT_F titleRect = showBar
            ? D2D1::RectF(tx, cy - 18.0f, rect.right - 14.0f, cy - 2.0f)
            : D2D1::RectF(tx, cy - 16.0f, rect.right - 14.0f, cy - 1.0f);
        mutedBrush_->SetOpacity(0.48f);
        const std::wstring clipTitle =
            state.clipboard.appName.empty()
                ? (state.clipboard.image ? std::wstring(L"Image copied") : std::wstring(L"Clipboard"))
                : state.clipboard.appName + L"  \u00b7  Clipboard";
        target_->DrawTextW(clipTitle.c_str(), static_cast<UINT32>(clipTitle.size()),
                           smallTextFormat_.Get(), titleRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        D2D1_RECT_F textRect = showBar
            ? D2D1::RectF(tx, cy - 2.0f, rect.right - 14.0f, cy + 15.0f)
            : D2D1::RectF(tx, cy - 1.0f, rect.right - 14.0f, cy + 16.0f);
        DrawMarqueeText(state.clipboard.text.empty() ? std::wstring(Loc(L"Copied")) : state.clipboard.text,
                        textRect, textFormat_.Get(), textBrush_.Get(), now, 34.0f, marqueeClipboardCache_);

        DrawCountdownProgress(tx, rect.right - 14.0f, rect.bottom - 6.0f, progress);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawQuickAccessDashboard(const Settings& settings, D2D1_RECT_F rect) {
        constexpr float pad = 16.0f;
        constexpr float gap = 8.0f;
        const float left = rect.left + pad, right = rect.right - pad;
        const float top = rect.top + 16.0f, bottom = rect.bottom - 16.0f;
        const float cellW = (right - left - 3.0f * gap) / 4.0f;
        const float cellH = (bottom - top - 2.0f * gap) / 3.0f;
        const D2D1_COLOR_F accents[] = {
            D2D1::ColorF(0.95f, 0.20f, 0.18f), D2D1::ColorF(0.28f, 0.55f, 0.96f),
            D2D1::ColorF(0.12f, 0.63f, 0.38f), D2D1::ColorF(0.90f, 0.18f, 0.20f),
            D2D1::ColorF(0.10f, 0.52f, 0.91f), D2D1::ColorF(0.55f, 0.40f, 0.90f)
        };
        for (size_t i = 0; i < settings.quickAccessLinks.size() && i < 12; ++i) {
            const int row = static_cast<int>(i / 4), col = static_cast<int>(i % 4);
            const float x = left + col * (cellW + gap), y = top + row * (cellH + gap);
            const D2D1_RECT_F tile = D2D1::RectF(x, y, x + cellW, y + cellH);
            ComPtr<ID2D1SolidColorBrush> tileBrush, iconBrush;
            target_->CreateSolidColorBrush(WithAlpha(material_.raisedStrong, 0.92f), &tileBrush);
            target_->CreateSolidColorBrush(accents[i % ARRAYSIZE(accents)], &iconBrush);
            if (tileBrush) target_->FillRoundedRectangle(D2D1::RoundedRect(tile, 12.0f, 12.0f), tileBrush.Get());
            const float iconSize = std::min(34.0f, cellH * 0.43f);
            const D2D1_RECT_F icon = D2D1::RectF(
                (tile.left + tile.right - iconSize) * 0.5f, tile.top + 6.0f,
                (tile.left + tile.right + iconSize) * 0.5f, tile.top + 6.0f + iconSize);
            if (iconBrush) target_->FillRoundedRectangle(D2D1::RoundedRect(icon, 9.0f, 9.0f), iconBrush.Get());
            std::wstring initial = settings.quickAccessLinks[i].first.substr(0, 1);
            if (textFormat_) {
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(initial.c_str(), static_cast<UINT32>(initial.size()),
                    smallTextFormat_.Get(), icon, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                const D2D1_RECT_F label = D2D1::RectF(tile.left + 3.0f, icon.bottom + 3.0f,
                                                       tile.right - 3.0f, tile.bottom - 3.0f);
                target_->DrawTextW(settings.quickAccessLinks[i].first.c_str(),
                    static_cast<UINT32>(settings.quickAccessLinks[i].first.size()),
                    smallTextFormat_.Get(), label, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
        }
    }

    void DrawClipboardDashboard(const SharedState& state, D2D1_RECT_F rect) {
        const auto history = state.clipboardHistory;
        const size_t itemCount = history ? history->size() : 0;
        const int columns = ClipboardGridColumns(itemCount);
        const int rows = ClipboardGridRows(itemCount);
        const int pageSize = columns * rows;
        const int pageCount = ClipboardHistoryPageCount(itemCount);
        const int page = ClampInt(g_clipboardHistoryPage.load(std::memory_order_relaxed),
                                  0, pageCount - 1);
        g_clipboardHistoryPage.store(page, std::memory_order_relaxed);
        const D2D1_COLOR_F cardSurface = material_.onDark
            ? D2D1::ColorF(0.10f, 0.12f, 0.15f, 0.98f * settingsOpacity_)
            : D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.98f * settingsOpacity_);

        const float gutterCenterX = (ClipboardLayout::kGutterLeft + ClipboardLayout::kGutterRight) * 0.5f;
        const float centerY = (rect.top + rect.bottom) * 0.5f;
        const D2D1_RECT_F clearButton = D2D1::RectF(
            rect.left + gutterCenterX - ClipboardLayout::kTrashSize * 0.5f,
            centerY - ClipboardLayout::kTrashSize * 0.5f,
            rect.left + gutterCenterX + ClipboardLayout::kTrashSize * 0.5f,
            centerY + ClipboardLayout::kTrashSize * 0.5f);
        const D2D1_COLOR_F clearSurface = material_.onDark
            ? D2D1::ColorF(0.20f, 0.23f, 0.27f, 0.98f * settingsOpacity_)
            : D2D1::ColorF(0.84f, 0.87f, 0.91f, 0.98f * settingsOpacity_);
        ComPtr<ID2D1SolidColorBrush> clearFill;
        target_->CreateSolidColorBrush(clearSurface, &clearFill);
        if (clearFill) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(clearButton, 10.0f, 10.0f), clearFill.Get());
            target_->DrawRoundedRectangle(D2D1::RoundedRect(clearButton, 10.0f, 10.0f),
                                          accentBrush_.Get(), 0.8f);
        }
        if (iconFormat_) {
            ComPtr<ID2D1SolidColorBrush> clearIcon;
            target_->CreateSolidColorBrush(
                EnsureContrastAgainstBackground(material_.textPrimary, clearSurface), &clearIcon);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(L"\uE74D", 1, iconFormat_.Get(), clearButton,
                               clearIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        const std::wstring countLabel = std::to_wstring(itemCount);
        const D2D1_RECT_F countRect = D2D1::RectF(
            rect.left + ClipboardLayout::kGutterLeft, clearButton.bottom + 5.0f,
            rect.left + ClipboardLayout::kGutterRight, clearButton.bottom + 21.0f);
        smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        mutedBrush_->SetOpacity(0.78f);
        target_->DrawTextW(countLabel.c_str(), static_cast<UINT32>(countLabel.size()),
                           smallTextFormat_.Get(), countRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);
        smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        mutedBrush_->SetOpacity(0.75f);

        const D2D1_RECT_F grid = D2D1::RectF(
            rect.left + ClipboardLayout::kGridLeft, rect.top + ClipboardLayout::kGridTop,
            rect.right - ClipboardLayout::kGridRightInset,
            rect.bottom - ClipboardLayout::kGridBottomInset);
        if (pageCount > 1 && iconFormat_) {
            const D2D1_RECT_F up = D2D1::RectF(rect.left + 17.0f, rect.top + 9.0f,
                                                rect.left + 39.0f, rect.top + 25.0f);
            const D2D1_RECT_F down = D2D1::RectF(rect.left + 17.0f, centerY + 60.0f,
                                                  rect.left + 39.0f, centerY + 78.0f);
            ComPtr<ID2D1SolidColorBrush> scrollBrush;
            target_->CreateSolidColorBrush(WithAlpha(material_.textPrimary, 0.82f * settingsOpacity_), &scrollBrush);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(L"\uE70E", 1, iconFormat_.Get(), up, scrollBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            target_->DrawTextW(L"\uE70D", 1, iconFormat_.Get(), down, scrollBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        if (itemCount == 0) {
            ComPtr<ID2D1SolidColorBrush> emptyBrush;
            target_->CreateSolidColorBrush(cardSurface, &emptyBrush);
            const D2D1_RECT_F emptyCard = D2D1::RectF(grid.left, grid.top, grid.right, grid.bottom);
            target_->FillRoundedRectangle(D2D1::RoundedRect(emptyCard, 16.0f, 16.0f), emptyBrush.Get());
            const wchar_t* hint = L"Copy text or an image to start your clipboard history";
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            mutedBrush_->SetOpacity(0.82f);
            target_->DrawTextW(hint, static_cast<UINT32>(wcslen(hint)), smallTextFormat_.Get(),
                               emptyCard, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            mutedBrush_->SetOpacity(0.75f);
            return;
        }

        ComPtr<ID2D1SolidColorBrush> cardBrush;
        ComPtr<ID2D1SolidColorBrush> cardBorderBrush;
        ComPtr<ID2D1SolidColorBrush> selectedBorderBrush;
        target_->CreateSolidColorBrush(cardSurface, &cardBrush);
        target_->CreateSolidColorBrush(WithAlpha(material_.stroke, 0.45f * settingsOpacity_),
                                       &cardBorderBrush);
        target_->CreateSolidColorBrush(WithAlpha(material_.accent, 0.72f * settingsOpacity_),
                                       &selectedBorderBrush);

        const float gap = itemCount > 4 ? 8.0f : 10.0f;
        const float cellW = (grid.right - grid.left - gap * (columns - 1)) / columns;
        const float cellH = (grid.bottom - grid.top - gap * (rows - 1)) / rows;
        const size_t start = static_cast<size_t>(page * pageSize);
        const size_t end = std::min(itemCount, start + static_cast<size_t>(pageSize));
        const int visible = static_cast<int>(end - start);
        int slot = 0;
        for (size_t i = start; i < end; ++i, ++slot) {
            const int row = slot / columns;
            const int rowCount = std::min(columns, visible - row * columns);
            const float rowOffset = (columns - rowCount) * (cellW + gap) * 0.5f;
            const float x = grid.left + rowOffset + (slot % columns) * (cellW + gap);
            const float y = grid.top + row * (cellH + gap);
            const D2D1_RECT_F card = D2D1::RectF(x, y, x + cellW, y + cellH);
            const float radius = itemCount <= 2 ? 15.0f : 12.0f;
            target_->FillRoundedRectangle(D2D1::RoundedRect(card, radius, radius), cardBrush.Get());
            target_->DrawRoundedRectangle(D2D1::RoundedRect(card, radius, radius),
                                          i == 0 ? selectedBorderBrush.Get() : cardBorderBrush.Get(),
                                          i == 0 ? 1.15f : 0.8f);

            const ClipboardHistoryItem& item = (*history)[i];
            const bool urlItem = !item.image && IsClipboardUrl(item.text);
            D2D1_COLOR_F semanticColor = item.image
                ? D2D1::ColorF(0.22f, 0.86f, 0.43f, 1.0f)
                : urlItem ? D2D1::ColorF(0.18f, 0.53f, 0.98f, 1.0f)
                          : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
            if (!item.image && !urlItem) {
                const D2D1_COLOR_F darkPalette[] = {
                    D2D1::ColorF(0.98f, 0.80f, 0.42f, 1.0f),
                    D2D1::ColorF(0.55f, 0.91f, 0.66f, 1.0f),
                    D2D1::ColorF(1.00f, 0.66f, 0.56f, 1.0f),
                    D2D1::ColorF(0.88f, 0.75f, 0.56f, 1.0f)};
                const D2D1_COLOR_F lightPalette[] = {
                    D2D1::ColorF(0.44f, 0.29f, 0.05f, 1.0f),
                    D2D1::ColorF(0.10f, 0.43f, 0.20f, 1.0f),
                    D2D1::ColorF(0.62f, 0.20f, 0.12f, 1.0f),
                    D2D1::ColorF(0.43f, 0.27f, 0.10f, 1.0f)};
                semanticColor = material_.onDark ? darkPalette[item.id % 4] : lightPalette[item.id % 4];
            }
            semanticColor = EnsureContrastAgainstBackground(semanticColor, cardSurface);
            ComPtr<ID2D1SolidColorBrush> semanticBrush;
            target_->CreateSolidColorBrush(semanticColor, &semanticBrush);
            const float inset = itemCount > 4 ? 8.0f : 11.0f;
            float contentLeft = card.left + inset;
            const float contentRight = card.right - inset - 8.0f;
            D2D1_RECT_F contentRect = D2D1::RectF(contentLeft, card.top + 7.0f,
                                                   contentRight, card.bottom - 7.0f);
            if (item.image && !item.imagePreview.bgra.empty()) {
                DrawCoverFitBitmapPixels(item.imagePreview, contentRect, 9.0f,
                                         clipboardHistoryBitmaps_[slot],
                                         clipboardHistoryBitmapGenerations_[slot], 1.0f);
            } else {
                std::wstring preview = item.text;
                std::replace(preview.begin(), preview.end(), L'\r', L' ');
                std::replace(preview.begin(), preview.end(), L'\n', L' ');
                constexpr size_t maxPreviewChars = 220;
                if (preview.size() > maxPreviewChars) {
                    preview.resize(maxPreviewChars);
                    preview += L"…";
                }
                smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
                smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                target_->DrawTextW(preview.c_str(), static_cast<UINT32>(preview.size()),
                                   smallTextFormat_.Get(), contentRect, semanticBrush.Get(),
                                   D2D1_DRAW_TEXT_OPTIONS_CLIP);
                smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            }
            D2D1_COLOR_F outline = item.pinned ? material_.accent
                : urlItem ? D2D1::ColorF(0.18f, 0.53f, 0.98f, 0.9f)
                : item.image ? D2D1::ColorF(0.22f, 0.86f, 0.43f, 0.95f)
                             : (i == 0 ? material_.accent : WithAlpha(semanticColor, 0.5f));
            ComPtr<ID2D1SolidColorBrush> semanticBorder;
            target_->CreateSolidColorBrush(outline, &semanticBorder);
            if (semanticBorder) {
                target_->DrawRoundedRectangle(D2D1::RoundedRect(card, radius, radius),
                                              semanticBorder.Get(), item.pinned ? 2.2f :
                                                  (item.image || urlItem ? 1.6f : 0.9f));
            }
        }

        // Paging is controlled by Shift+wheel and compact gutter arrows above
        // the trash button; no page number or footer steals card area.
    }

    void DrawVideoUnavailableDashboard(D2D1_RECT_F rect) {
        const float pad = 26.0f;
        const D2D1_RECT_F title = D2D1::RectF(rect.left + pad, rect.top + 30.0f,
                                              rect.right - pad, rect.top + 56.0f);
        target_->DrawTextW(L"Video Playback", 14, textFormat_.Get(), title,
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        const wchar_t* message = L"No matching video player is active.";
        const D2D1_RECT_F body = D2D1::RectF(rect.left + pad, rect.top + 66.0f,
                                             rect.right - pad, rect.bottom - 20.0f);
        mutedBrush_->SetOpacity(0.70f);
        target_->DrawTextW(message, static_cast<UINT32>(wcslen(message)),
                           smallTextFormat_.Get(), body, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);
        mutedBrush_->SetOpacity(0.75f);
    }

    void DrawNotification(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 48.0f || rect.right - rect.left < 120.0f) return;
        const double now = NowSeconds();
        const float ttl = static_cast<float>(g_notificationDurationSeconds.load(std::memory_order_relaxed));
        const float remaining = Clamp(static_cast<float>(state.notification.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        const float cy = (rect.top + rect.bottom) * 0.5f;
        // Apple DI: app icon is a large iOS-style rounded square.
        const float iconSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - iconSz * 0.5f,
                                        rect.left + 14 + iconSz, cy + iconSz * 0.5f);
        const float br = iconSz * 0.35f;  // Softer iOS superellipse-like squircle.

        // Icon background plate.
        ComPtr<ID2D1SolidColorBrush> plateBrush;
        target_->CreateSolidColorBrush(material_.raisedStrong, &plateBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), plateBrush.Get());

        if (!state.notification.icon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.notification.icon, badge, br,
                                    notificationIconBitmap_, notificationIconGeneration_, 1.0f);

            // Draw a red dot (badge) at the top-right of the app icon
            ComPtr<ID2D1SolidColorBrush> badgeColor;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &badgeColor);

            const float dotR = iconSz * 0.13f;
            const float dotX = badge.right - dotR * 0.5f;
            const float dotY = badge.top + dotR * 0.5f;

            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeColor.Get());

            ComPtr<ID2D1SolidColorBrush> badgeBorder;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &badgeBorder);
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeBorder.Get(), 0.9f);
        } else {
            const wchar_t* glyph = usingFluentIcons_ ? L"\uEA8F" : L"\uE7E7";
            textBrush_->SetOpacity(0.95f);
            if (iconFormat_) {
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(glyph,
                                   static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
            textBrush_->SetOpacity(0.90f);
        }

        const float tx = badge.right + 14.0f;
        const bool showBar = g_settings.statusCountdownProgress;
        D2D1_RECT_F appRect = showBar
            ? D2D1::RectF(tx, cy - 18.0f, rect.right - 14.0f, cy - 2.0f)
            : D2D1::RectF(tx, cy - 16.0f, rect.right - 14.0f, cy - 1.0f);
        mutedBrush_->SetOpacity(0.75f);
        target_->DrawTextW(state.notification.app.c_str(),
                           static_cast<UINT32>(state.notification.app.size()),
                           smallTextFormat_.Get(), appRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        D2D1_RECT_F titleRect = showBar
            ? D2D1::RectF(tx, cy - 2.0f, rect.right - 14.0f, cy + 15.0f)
            : D2D1::RectF(tx, cy - 1.0f, rect.right - 14.0f, cy + 16.0f);
        textBrush_->SetOpacity(0.95f);
        DrawMarqueeText(state.notification.title.empty() ? L"Notification" : state.notification.title,
                        titleRect, textFormat_.Get(), textBrush_.Get(), now, 28.0f, marqueeNotificationCache_);
        textBrush_->SetOpacity(0.90f);

        DrawCountdownProgress(tx, rect.right - 14.0f, rect.bottom - 6.0f, progress);
        mutedBrush_->SetOpacity(0.50f);
    }

    void DrawVolume(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const bool muted = state.volume.muted || state.volume.percent == 0;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f; // Softer squircle corners

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const wchar_t* glyph = muted ? L"\uE74F" : (usingFluentIcons_ ? L"\uE767" : L"\uE993");
        textBrush_->SetOpacity(0.95f);

        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);

            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 13.0f, rect.right - 58, cy + 3.0f);
        mutedBrush_->SetOpacity(0.50f);
        const std::wstring deviceLabel =
            state.volume.deviceName.empty() ? std::wstring(Loc(L"Volume")) : state.volume.deviceName;
        target_->DrawTextW(deviceLabel.c_str(), static_cast<UINT32>(deviceLabel.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        wchar_t value[32] = {};
        if (muted) {
            wcscpy_s(value, ARRAYSIZE(value), Loc(L"Muted"));
        } else {
            swprintf_s(value, L"%d%%", state.volume.percent);
        }
        D2D1_RECT_F valueRect = D2D1::RectF(rect.right - 58, cy - 13.0f, rect.right - 14, cy + 3.0f);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), smallTextFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);

        D2D1_RECT_F track = D2D1::RectF(tx, cy + 7.0f, rect.right - 14, cy + 12.0f);
        // Shared accent track, so the volume bar matches the media scrubber.
        const float pct = Clamp(state.volume.percent / 100.0f, 0.0f, 1.0f);
        if (muted) {
            // Muted still shows the level, just drained of colour.
            ComPtr<ID2D1SolidColorBrush> trackBrush;
            if (SUCCEEDED(target_->CreateSolidColorBrush(material_.raisedStrong, &trackBrush)) && trackBrush) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2.5f, 2.5f), trackBrush.Get());
            }
            D2D1_RECT_F fill = D2D1::RectF(track.left, track.top,
                                           track.left + (track.right - track.left) * pct,
                                           track.bottom);
            ComPtr<ID2D1SolidColorBrush> dim;
            if (SUCCEEDED(target_->CreateSolidColorBrush(
                    WithAlpha(material_.textSecondary, 0.35f), &dim)) && dim) {
                target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 2.5f, 2.5f), dim.Get());
            }
        } else {
            DrawAccentTrack(track, pct, 2.5f);
        }
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawTimer(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const double now = NowSeconds();
        double remaining = state.timer.justFinished
            ? 0.0
            : (state.timer.running ? std::max(0.0, state.timer.endsAt - now)
                                    : state.timer.remainingAtPause);
        const int totalSec = state.timer.totalSeconds > 0 ? state.timer.totalSeconds : 1;
        const float progress = state.timer.justFinished
            ? 1.0f
            : Clamp(1.0f - static_cast<float>(remaining / totalSec), 0.0f, 1.0f);

        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const float ringR = badgeSz * 0.30f;
        D2D1_POINT_2F ringCenter = D2D1::Point2F((badge.left + badge.right) * 0.5f,
                                                  (badge.top + badge.bottom) * 0.5f);
        ComPtr<ID2D1SolidColorBrush> ringTrack;
        target_->CreateSolidColorBrush(material_.raisedStrong, &ringTrack);
        target_->DrawEllipse(D2D1::Ellipse(ringCenter, ringR, ringR), ringTrack.Get(), 2.0f);

        ComPtr<ID2D1PathGeometry> geometry;
        d2dFactory_->CreatePathGeometry(&geometry);
        ComPtr<ID2D1GeometrySink> sink;
        geometry->Open(&sink);
        const float start = -3.14159265f * 0.5f;
        const float sweep = 2.0f * 3.14159265f * progress;
        const int segments = std::max(2, static_cast<int>(40 * progress));
        auto pointAt = [&](float a) {
            return D2D1::Point2F(ringCenter.x + std::cos(a) * ringR,
                                  ringCenter.y + std::sin(a) * ringR);
        };
        sink->BeginFigure(pointAt(start), D2D1_FIGURE_BEGIN_HOLLOW);
        for (int i = 1; i <= segments; ++i) {
            sink->AddLine(pointAt(start + sweep * i / segments));
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        ComPtr<ID2D1SolidColorBrush> ringFg;
        D2D1_COLOR_F fgColor = state.timer.isBreak
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)
            : D2D1::ColorF(1.0f, 0.58f, 0.0f, 1.0f);
        if (state.timer.justFinished) {
            fgColor = D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f);
        }
        target_->CreateSolidColorBrush(fgColor, &ringFg);
        target_->DrawGeometry(geometry.Get(), ringFg.Get(), 2.4f);

        if (state.timer.active && !state.timer.running) {
            ComPtr<ID2D1SolidColorBrush> pauseBrush;
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.9f), &pauseBrush);
            const float h = ringR * 0.7f;
            target_->FillRectangle(
                D2D1::RectF(ringCenter.x - 2.6f, ringCenter.y - h * 0.5f,
                            ringCenter.x - 0.8f, ringCenter.y + h * 0.5f), pauseBrush.Get());
            target_->FillRectangle(
                D2D1::RectF(ringCenter.x + 0.8f, ringCenter.y - h * 0.5f,
                            ringCenter.x + 2.6f, ringCenter.y + h * 0.5f), pauseBrush.Get());
        }

        const float tx = badge.right + 14;
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = state.timer.justFinished
            ? (state.timer.isBreak ? L"Break Complete" : L"Focus Complete")
            : (state.timer.isBreak ? L"Break Timer" : L"Focus Timer");
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 16.0f, rect.right - 14.0f, cy - 1.0f);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        wchar_t value[32] = {};
        int rem = static_cast<int>(std::ceil(remaining));
        swprintf_s(value, L"%d:%02d", rem / 60, rem % 60);
        D2D1_RECT_F valueRect = D2D1::RectF(tx, cy - 1.0f, rect.right - 14.0f, cy + 16.0f);
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawCapsLock(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 110.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const wchar_t* glyph = nullptr;
        std::wstring label;
        bool isOn = false;

        if (state.capsLock.isNumEvent) {
            glyph = L"1";
            label = Loc(L"Num Lock");
            isOn = state.capsLock.numOn;
        } else {
            glyph = L"A";
            label = Loc(L"Caps Lock");
            isOn = state.capsLock.capsOn;
        }

        // Draw central bold keycap glyph, vertically and horizontally centered
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), clockFormat_.Get(), badge,
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);

        // Draw physical glowing status LED inside the keycap at top-right with padding
        ComPtr<ID2D1SolidColorBrush> ledBrush;
        D2D1_COLOR_F ledColor = isOn
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)   // Green glowing LED for ON
            : D2D1::ColorF(1.0f,  1.0f,  1.0f,  0.22f);  // Dim white for OFF
        target_->CreateSolidColorBrush(ledColor, &ledBrush);

        const float ledR = 2.2f;
        D2D1_POINT_2F ledCenter = D2D1::Point2F(badge.right - 5.5f, badge.top + 5.5f);
        target_->FillEllipse(D2D1::Ellipse(ledCenter, ledR, ledR), ledBrush.Get());

        // Draw label text ("Caps Lock" / "Num Lock") - increased font size and vertically centered
        const float tx = badge.right + 14.0f;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 9.0f, rect.right - 46.0f, cy + 11.0f);
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           textFormat_.Get(), labelRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        // Draw status string (ON/OFF) - increased font size and vertically centered
        std::wstring status = isOn ? Loc(L"On") : Loc(L"Off");
        D2D1_RECT_F statusRect = D2D1::RectF(rect.right - 44.0f, cy - 9.0f, rect.right - 14.0f, cy + 11.0f);
        if (isOn) {
            textBrush_->SetOpacity(0.95f);
            target_->DrawTextW(status.c_str(), static_cast<UINT32>(status.size()), textFormat_.Get(),
                               statusRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        } else {
            mutedBrush_->SetOpacity(0.75f);
            target_->DrawTextW(status.c_str(), static_cast<UINT32>(status.size()), textFormat_.Get(),
                               statusRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }

    void DrawDevice(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 100.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const bool connected = (state.device.eventType == DeviceEventType::Connected);

        // Badge circle with colored dot
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const wchar_t* glyph = usingFluentIcons_ ? L"\uECF0" : L"\uE88E";
        textBrush_->SetOpacity(0.95f);
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        textBrush_->SetOpacity(0.90f);

        ComPtr<ID2D1SolidColorBrush> dotBrush;
        D2D1_COLOR_F dotColor = connected
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)
            : D2D1::ColorF(1.0f,  0.27f, 0.22f, 1.0f);
        target_->CreateSolidColorBrush(dotColor, &dotBrush);

        D2D1_POINT_2F dotCenter = D2D1::Point2F(badge.right - 4.5f, badge.bottom - 4.5f);
        target_->FillEllipse(D2D1::Ellipse(dotCenter, 4.5f, 4.5f), dotBrush.Get());

        const float tx = badge.right + 14.0f;
        const bool showBar = g_settings.statusCountdownProgress;
        const double now = NowSeconds();
        const float ttl = 3.0f;
        const float remaining = Clamp(static_cast<float>(state.device.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = connected ? L"Device Connected" : L"Device Removed";
        D2D1_RECT_F labelRect = showBar
            ? D2D1::RectF(tx, cy - 18.0f, rect.right - 14.0f, cy - 2.0f)
            : D2D1::RectF(tx, cy - 16.0f, rect.right - 14.0f, cy - 1.0f);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        textBrush_->SetOpacity(0.95f);
        const std::wstring& name = state.device.deviceName.empty()
            ? (state.device.isBluetoothLike ? std::wstring(L"Bluetooth") : std::wstring(L"USB Device"))
            : state.device.deviceName;
        D2D1_RECT_F nameRect = showBar
            ? D2D1::RectF(tx, cy - 2.0f, rect.right - 14.0f, cy + 15.0f)
            : D2D1::RectF(tx, cy - 1.0f, rect.right - 14.0f, cy + 16.0f);
        target_->DrawTextW(name.c_str(), static_cast<UINT32>(name.size()),
                           textFormat_.Get(), nameRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);

        DrawCountdownProgress(tx, rect.right - 14.0f, rect.bottom - 6.0f, progress);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawDoNotDisturb(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 110.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14.0f, cy - badgeSz * 0.5f,
                                        rect.left + 14.0f + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const bool isOn = state.doNotDisturb.enabled;
        const wchar_t* glyph = isOn ? L"\uE7ED" : L"\uEA8F";
        textBrush_->SetOpacity(0.95f);
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        ComPtr<ID2D1SolidColorBrush> ledBrush;
        D2D1_COLOR_F ledColor = isOn
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)
            : D2D1::ColorF(1.0f,  1.0f,  1.0f,  0.22f);
        target_->CreateSolidColorBrush(ledColor, &ledBrush);

        const float ledR = 2.2f;
        D2D1_POINT_2F ledCenter = D2D1::Point2F(badge.right - 5.5f, badge.top + 5.5f);
        target_->FillEllipse(D2D1::Ellipse(ledCenter, ledR, ledR), ledBrush.Get());

        const float tx = badge.right + 14.0f;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 9.0f, rect.right - 46.0f, cy + 11.0f);
        textBrush_->SetOpacity(0.95f);
        std::wstring label = Loc(L"Do Not Disturb");
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           textFormat_.Get(), labelRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        std::wstring status = isOn ? Loc(L"On") : Loc(L"Off");
        D2D1_RECT_F statusRect = D2D1::RectF(rect.right - 44.0f, cy - 9.0f, rect.right - 14.0f, cy + 11.0f);
        if (isOn) {
            textBrush_->SetOpacity(0.95f);
            target_->DrawTextW(status.c_str(), static_cast<UINT32>(status.size()), textFormat_.Get(),
                               statusRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        } else {
            mutedBrush_->SetOpacity(0.75f);
            target_->DrawTextW(status.c_str(), static_cast<UINT32>(status.size()), textFormat_.Get(),
                               statusRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        const double now = NowSeconds();
        const float ttl = 3.0f;
        const float remaining = Clamp(static_cast<float>(state.doNotDisturb.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;
        DrawCountdownProgress(tx, rect.right - 14.0f, rect.bottom - 4.0f, progress);
        mutedBrush_->SetOpacity(0.58f);
        textBrush_->SetOpacity(0.90f);
    }

    void DrawBluetoothCategoryIcon(D2D1_RECT_F badge, BluetoothDeviceCategory category) {
        const wchar_t* glyph = L"\uE702";
        switch (category) {
            case BluetoothDeviceCategory::Headphones:
                glyph = L"\uE7F6";
                break;
            case BluetoothDeviceCategory::Speaker:
                glyph = L"\uE7F5";
                break;
            case BluetoothDeviceCategory::Mouse:
                glyph = L"\uE962";
                break;
            case BluetoothDeviceCategory::Keyboard:
                glyph = L"\uE92E";
                break;
            case BluetoothDeviceCategory::Phone:
                glyph = L"\uE8EA";
                break;
            case BluetoothDeviceCategory::Generic:
            default:
                glyph = L"\uE702";
                break;
        }

        textBrush_->SetOpacity(0.95f);
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }
        textBrush_->SetOpacity(0.90f);
    }

    void DrawBluetoothDevice(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const double now = NowSeconds();
        const float ttl = static_cast<float>(g_notificationDurationSeconds.load(std::memory_order_relaxed));
        const float remaining = Clamp(static_cast<float>(state.bluetoothDevice.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        const bool connected = state.bluetoothDevice.connected;
        const int battery = state.bluetoothDevice.batteryPercent;
        const bool hasBattery = g_settings.bluetoothShowBattery && battery >= 0;

        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        DrawBluetoothCategoryIcon(badge, state.bluetoothDevice.category);

        ComPtr<ID2D1SolidColorBrush> dotBrush;
        D2D1_COLOR_F dotColor = connected
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)
            : D2D1::ColorF(1.0f, 0.27f, 0.22f, 1.0f);
        target_->CreateSolidColorBrush(dotColor, &dotBrush);
        D2D1_POINT_2F dotCenter = D2D1::Point2F(badge.right - 4.5f, badge.bottom - 4.5f);
        target_->FillEllipse(D2D1::Ellipse(dotCenter, 4.5f, 4.5f), dotBrush.Get());

        const float tx = badge.right + 14.0f;
        const bool showBar = g_settings.statusCountdownProgress;
        const float rightEdge = hasBattery ? rect.right - 62.0f : rect.right - 14.0f;

        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = std::wstring(Loc(L"Bluetooth")) + L" " + (connected ? Loc(L"Connected") : Loc(L"Disconnected"));
        D2D1_RECT_F labelRect = showBar
            ? D2D1::RectF(tx, cy - 18.0f, rightEdge, cy - 2.0f)
            : D2D1::RectF(tx, cy - 16.0f, rightEdge, cy - 1.0f);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        textBrush_->SetOpacity(0.95f);
        const std::wstring& name = state.bluetoothDevice.deviceName.empty()
            ? std::wstring(L"Bluetooth Device")
            : state.bluetoothDevice.deviceName;
        D2D1_RECT_F nameRect = showBar
            ? D2D1::RectF(tx, cy - 2.0f, rightEdge, cy + 15.0f)
            : D2D1::RectF(tx, cy - 1.0f, rightEdge, cy + 16.0f);
        target_->DrawTextW(name.c_str(), static_cast<UINT32>(name.size()),
                           textFormat_.Get(), nameRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        if (hasBattery) {
            const float colCenter = rect.right - 31.0f;
            const float bw = 16.0f;
            const float bh = 8.5f;
            const float nubW = 1.6f;
            const float totalW = bw + nubW;
            const float batLeft = colCenter - totalW * 0.5f;
            const float batY = showBar ? cy - 10.0f : cy - 9.0f;

            D2D1_RECT_F batRect = D2D1::RectF(batLeft, batY - bh * 0.5f,
                                              batLeft + bw, batY + bh * 0.5f);
            ComPtr<ID2D1SolidColorBrush> batBorder;
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.75f), &batBorder);
            target_->DrawRoundedRectangle(D2D1::RoundedRect(batRect, 1.5f, 1.5f), batBorder.Get(), 1.2f);
            D2D1_RECT_F nub = D2D1::RectF(batRect.right, batY - 2.0f, batRect.right + nubW, batY + 2.0f);
            target_->FillRectangle(nub, batBorder.Get());

            const float pct = Clamp(battery / 100.0f, 0.0f, 1.0f);
            D2D1_RECT_F fill = D2D1::RectF(batRect.left + 1.5f, batRect.top + 1.5f,
                                           batRect.left + 1.5f + (bw - 3.0f) * pct, batRect.bottom - 1.5f);
            ComPtr<ID2D1SolidColorBrush> fillBrush;
            D2D1_COLOR_F fillColor = battery <= 20 ? D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f)
                                                    : D2D1::ColorF(1, 1, 1, 0.92f);
            target_->CreateSolidColorBrush(fillColor, &fillBrush);
            target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 0.8f, 0.8f), fillBrush.Get());

            wchar_t pctBuf[16] = {};
            swprintf_s(pctBuf, L"%d%%", battery);
            D2D1_RECT_F pctRect = showBar
                ? D2D1::RectF(colCenter - 25.0f, cy - 2.0f, colCenter + 25.0f, cy + 14.0f)
                : D2D1::RectF(colCenter - 25.0f, cy - 1.0f, colCenter + 25.0f, cy + 15.0f);
            textBrush_->SetOpacity(0.85f);
            if (smallTextFormat_) {
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                target_->DrawTextW(pctBuf, static_cast<UINT32>(wcslen(pctBuf)), smallTextFormat_.Get(),
                                   pctRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
                smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }
        }

        DrawCountdownProgress(tx, rect.right - 14.0f, rect.bottom - 6.0f, progress);
        mutedBrush_->SetOpacity(0.58f);
        textBrush_->SetOpacity(0.90f);
    }

    void DrawBattery(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(material_.raisedStrong, &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        // Draw battery vector icon
        const float bx = badge.left + badgeSz * 0.25f;
        const float by = cy - badgeSz * 0.22f;
        const float bw = badgeSz * 0.45f;
        const float bh = badgeSz * 0.44f;
        D2D1_RECT_F batRect = D2D1::RectF(bx, by, bx + bw, by + bh);

        ComPtr<ID2D1SolidColorBrush> batBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.85f), &batBorder);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(batRect, 2, 2), batBorder.Get(), 1.5f);

        // Battery Terminal (Nub)
        D2D1_RECT_F nubRect = D2D1::RectF(batRect.right, cy - 3, batRect.right + 2.5f, cy + 3);
        target_->FillRectangle(nubRect, batBorder.Get());

        // Battery Fill
        const float pct = Clamp(state.battery.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fillRect = D2D1::RectF(batRect.left + 2, batRect.top + 2,
                                           batRect.left + 2 + (bw - 4) * pct, batRect.bottom - 2);

        ComPtr<ID2D1SolidColorBrush> batFill;
        if (state.battery.low) {
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &batFill); // Red
        } else if (state.battery.charging) {
            target_->CreateSolidColorBrush(D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f), &batFill); // Green
        } else {
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.95f), &batFill); // White
        }
        target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, 1, 1), batFill.Get());

        // Text Labels
        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 16.0f, rect.right - 14.0f, cy - 1.0f);
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = state.battery.charging ? L"Power Connected" : L"Battery Alert";
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        wchar_t value[128] = {};
        if (state.battery.secondsRemaining != BATTERY_LIFE_UNKNOWN && !state.battery.charging) {
            const DWORD minutes = state.battery.secondsRemaining / 60;
            swprintf_s(value, ARRAYSIZE(value), L"%d%% \u2022 %luh %02lum left",
                       state.battery.percent, minutes / 60, minutes % 60);
        } else {
            swprintf_s(value, ARRAYSIZE(value), L"%d%%", state.battery.percent);
        }

        D2D1_RECT_F valueRect = D2D1::RectF(tx, cy - 1.0f, rect.right - 14.0f, cy + 16.0f);
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);
    }

    void DrawProgress(const SharedState& state, D2D1_RECT_F rect) {
        wchar_t buffer[64] = {};
        swprintf_s(buffer, L"Progress %d%%", state.progress.percent);
        IDWriteTextFormat* fmt = idleTextFormat_ ? idleTextFormat_.Get() : textFormat_.Get();
        target_->DrawTextW(buffer, static_cast<UINT32>(wcslen(buffer)), fmt,
                           rect,
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

    void DrawProgressRing(D2D1_RECT_F rect, int percent) {
        ComPtr<ID2D1PathGeometry> geometry;
        d2dFactory_->CreatePathGeometry(&geometry);
        ComPtr<ID2D1GeometrySink> sink;
        geometry->Open(&sink);

        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float rx = (rect.right - rect.left) * 0.5f + 6.0f;
        const float ry = (rect.bottom - rect.top) * 0.5f + 6.0f;
        const float start = -3.14159265f * 0.5f;
        const float sweep = 2.0f * 3.14159265f * Clamp(percent / 100.0f, 0.0f, 1.0f);
        const int segments = std::max(2, static_cast<int>(48 * percent / 100.0f));

        auto pointAt = [&](float a) {
            return D2D1::Point2F(cx + std::cos(a) * rx, cy + std::sin(a) * ry);
        };

        sink->BeginFigure(pointAt(start), D2D1_FIGURE_BEGIN_HOLLOW);
        for (int i = 1; i <= segments; ++i) {
            const float a = start + sweep * i / segments;
            sink->AddLine(pointAt(a));
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        accentBrush_->SetOpacity(0.92f);
        target_->DrawGeometry(geometry.Get(), accentBrush_.Get(), 3.0f);
        accentBrush_->SetOpacity(1.0f);
    }

    HWND hwnd_ = nullptr;
    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HBITMAP oldBitmap_ = nullptr;
    int bitmapWidth_ = 0;
    int bitmapHeight_ = 0;

    ComPtr<ID2D1Factory> d2dFactory_;
    ComPtr<ID2D1DCRenderTarget> target_;
    ComPtr<IDWriteFactory> dwriteFactory_;
    ComPtr<IDWriteTextFormat> textFormat_;
    ComPtr<IDWriteTextFormat> smallTextFormat_;
    ComPtr<IDWriteTextFormat> boldTextFormat_;
    ComPtr<IDWriteTextFormat> hugeTextFormat_;
    ComPtr<IDWriteTextFormat> clockFormat_;
    ComPtr<IDWriteTextFormat> iconFormat_;
    ComPtr<IDWriteTextFormat> mediaPlayIconFormat_;
    ComPtr<IDWriteTextFormat> mediaNavIconFormat_;
    bool usingFluentIcons_ = true;
    ComPtr<IDWriteTextFormat> idleTextFormat_;
    ComPtr<IDWriteTextFormat> calDayLargeFormat_;
    ComPtr<IDWriteTextFormat> calGridFormat_;
    ComPtr<IDWriteTextFormat> timeDashboardFormat_;
    ComPtr<IDWriteTextFormat> dateDashboardFormat_;
    ComPtr<ID2D1SolidColorBrush> accentBrush_;
    ComPtr<ID2D1SolidColorBrush> redBrush_;
    ComPtr<ID2D1SolidColorBrush> textBrush_;
    ComPtr<ID2D1SolidColorBrush> mutedBrush_;
    ComPtr<ID2D1SolidColorBrush> tintBrush_;
    ComPtr<ID2D1SolidColorBrush> shadowBrush_;
    ComPtr<ID2D1SolidColorBrush> micDotBrush_;
    ComPtr<ID2D1SolidColorBrush> micGlowBrush_;
    ComPtr<ID2D1SolidColorBrush> camDotBrush_;
    ComPtr<ID2D1SolidColorBrush> camGlowBrush_;
    ComPtr<ID2D1SolidColorBrush> scratchColorBrush_;
    ComPtr<ID2D1RadialGradientBrush> timeGlowBrush_;
    D2D1_COLOR_F timeGlowColor_ = D2D1::ColorF(0x4cc9f0);
    bool timeGlowColorValid_ = false;
    MarqueeLayoutCache marqueeTitleCache_;
    MarqueeLayoutCache marqueeArtistCache_;
    MarqueeLayoutCache marqueeAlbumCache_;
    MarqueeLayoutCache marqueeClipboardCache_;
    MarqueeLayoutCache marqueeNotificationCache_;
    ComPtr<ID2D1Bitmap> artBitmap_;
    ComPtr<ID2D1Bitmap> notificationIconBitmap_;
    ComPtr<ID2D1Bitmap> mediaSourceIconBitmap_;
    ComPtr<ID2D1Bitmap> clipboardIconBitmap_;
    ComPtr<ID2D1Bitmap> clipboardImageBitmap_;
    std::array<ComPtr<ID2D1Bitmap>, 6> clipboardHistoryBitmaps_{};
    std::array<uint64_t, 6> clipboardHistoryBitmapGenerations_{};
    uint64_t artGeneration_ = 0;
    SYSTEMTIME calendarCachedDate_{};
    std::wstring calendarCachedMonthName_;
    std::wstring calendarCachedWeekdayName_;
    ComPtr<IDWriteTextFormat> weatherDescFormat_;
    float weatherDescFormatSize_ = -1.0f;
    uint64_t notificationIconGeneration_ = 0;
    uint64_t mediaSourceIconGeneration_ = 0;
    uint64_t clipboardIconGeneration_ = 0;
    uint64_t clipboardImageGeneration_ = 0;
    ComPtr<ID2D1Bitmap> fileTrayIconBitmap_;
    uint64_t fileTrayIconGeneration_ = 0;
    float settingsOpacity_ = 0.96f;
    D2D1_COLOR_F pillBgColor_ = D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f);
    // Design tokens for the current frame, rebuilt by EnsureBrushes.
    MaterialTokens material_{};
    // Accent color lerp: smoothly transition between successive sampled accents
    // so track changes don't produce a jarring instant color pop.
    D2D1_COLOR_F currentAccent_ = D2D1::ColorF(0x4cc9f0);
    double       lastAccentTime_ = -1.0;  // -1 = not yet set (will snap on first frame)
    float        mediaBtnPress_[3] = {0.0f, 0.0f, 0.0f};
    double       lastMediaBtnTime_ = -1.0;
    float        mediaVolumeReveal_ = 0.0f;
    double       lastMediaVolumeDrawTime_ = -1.0;
};

Activity ActivityForKind(IslandKind kind, const Settings& settings, const SharedState& state) {
    Activity activity;
    activity.kind = kind;

    switch (kind) {
        case IslandKind::Media:
            activity.width = settings.islandWidth;
            activity.height = settings.islandHeight;
            break;
        case IslandKind::Progress:
            activity.width = 230.0f;
            activity.height = 48.0f;
            break;
        case IslandKind::Clipboard:
            activity.width = 340.0f;
            activity.height = 56.0f;
            break;
        case IslandKind::Notification:
            activity.width = 360.0f;
            activity.height = 58.0f;
            break;
        case IslandKind::Volume:
            activity.width = 300.0f;
            activity.height = 54.0f;
            break;
        case IslandKind::BatteryLow:
            activity.width = 290.0f;
            activity.height = 52.0f;
            break;
        case IslandKind::CapsLock:
            activity.width = 180.0f;
            activity.height = 42.0f;
            break;
        case IslandKind::Device:
            activity.width = 240.0f;
            activity.height = 50.0f;
            break;
        case IslandKind::Bluetooth:
            activity.width = 280.0f;
            activity.height = 54.0f;
            break;
        case IslandKind::Timer:
            activity.width = 260.0f;
            activity.height = 54.0f;
            break;
        case IslandKind::DoNotDisturb:
            activity.width = 220.0f;
            activity.height = 42.0f;
            break;
        case IslandKind::Idle:
        default:
            if (g_autoHideEnabled.load(std::memory_order_relaxed) &&
                settings.autoHideIdleSeconds == -1 && !state.system.micActive && !state.system.cameraActive) {
                activity.width = 0.0f;
                activity.height = 0.0f;
            } else {
                // Seed only. The real collapsed width is measured from the clock
                // and weather strings by Renderer::MeasureIdleStrip and applied in
                // the render loop -- this function has no DWrite access. A fixed
                // width here was the whole bug behind windhawk-mods#5086: dead
                // air around a short "9:41", and clipping on "10:41:32 PM" once
                // Text size reached 140.
                activity.width = settings.weather ? 170.0f : 96.0f;
                activity.height = IdleStripLayout::kHeight;
            }
            break;
    }

    if (kind == IslandKind::Idle) {
        activity.height = settings.islandHeight;
    }
    activity.width *= settings.sizeScale;
    activity.height *= settings.sizeScale;
    return activity;
}

std::vector<IslandKind> ChooseActivities(const SharedState& state, const Settings& settings, double now) {
    std::vector<IslandKind> activities;

    if (state.clipboard.active && now < state.clipboard.expiresAt) {
        activities.push_back(IslandKind::Clipboard);
    }
    // settings.capsLock is checked here, like every other module above and below.
    // Leaving it out was what made the Caps Lock toggle not actually work: the
    // handler gate stopped the nudge but the pill was still selected and drawn.
    if (settings.capsLock && state.capsLock.active && now < state.capsLock.expiresAt) {
        activities.push_back(IslandKind::CapsLock);
    }
    if (state.device.active && now < state.device.expiresAt) {
        activities.push_back(IslandKind::Device);
    }
    if (settings.bluetoothIndicator && state.bluetoothDevice.active &&
        now < state.bluetoothDevice.expiresAt) {
        activities.push_back(IslandKind::Bluetooth);
    }
    if (settings.doNotDisturbIndicator && state.doNotDisturb.active &&
        now < state.doNotDisturb.expiresAt) {
        activities.push_back(IslandKind::DoNotDisturb);
    }
    if (settings.volume && state.volume.active && now < state.volume.expiresAt) {
        activities.push_back(IslandKind::Volume);
    }
    if (state.notification.active && now < state.notification.expiresAt) {
        activities.push_back(IslandKind::Notification);
    }
    if (settings.battery && state.battery.active && now < state.battery.expiresAt) {
        activities.push_back(IslandKind::BatteryLow);
    }
    if (settings.progress && state.progress.active) {
        activities.push_back(IslandKind::Progress);
    }
    if (settings.timerEnabled &&
        ((state.timer.justFinished && now < state.timer.finishedExpiresAt) ||
         state.timer.active)) {
        activities.push_back(IslandKind::Timer);
    }
    // Keep an established media session visible while paused. The session is
    // marked unavailable when the source closes; playback state only controls
    // animated artwork, not the dashboard's structure.
    if (settings.media && state.media.available) {
        activities.push_back(IslandKind::Media);
    }

    if (activities.empty()) {
        activities.push_back(IslandKind::Idle);
    }

    return activities;
}

constexpr UINT WM_APP_CAPSLOCK = WM_APP + 0x444;
HHOOK g_keyboardHook = nullptr;
HANDLE g_keyboardThread = nullptr;
DWORD g_keyboardThreadId = 0;

// Deliberately does nothing but forward the event.
//
// This used to write g_state.capsLock here, under g_stateMutex, before posting.
// Two problems with that. It recorded the pill even when the Caps Lock module was
// switched off, because the setting is only checked once the message reaches the
// window thread -- so the pill still appeared for its full 2.5s. And a
// WH_KEYBOARD_LL callback runs inline on the input path, blocking every keystroke
// system-wide until it returns, so taking a lock that the render, weather or
// media threads also hold risked stalling typing on the whole desktop.
//
// The WM_APP_CAPSLOCK handler writes exactly the same fields on the window
// thread, after checking the setting. Nothing is lost by only posting.
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            if (kbd->vkCode == VK_CAPITAL || kbd->vkCode == VK_NUMLOCK) {
                const bool capsOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
                const bool numOn = (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
                if (HWND hwnd = g_hwnd) {
                    const LPARAM state = (capsOn ? 1 : 0) | (numOn ? 2 : 0);
                    PostMessageW(hwnd, WM_APP_CAPSLOCK, kbd->vkCode, state);
                }
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// A system-wide WH_KEYBOARD_LL puts this process on the path of every keystroke
// on the desktop, so it is only installed while it has something to report. The
// hook exists solely to notice Caps Lock and Num Lock for the indicator pill;
// with that module off it was still running for nothing.
static bool CapsLockHookWanted() {
    return g_settings.capsLock;
}

// Wakes the keyboard thread so it re-evaluates whether the hook is needed.
void NotifyKeyboardThreadSettingChanged() {
    if (g_keyboardThreadId != 0) {
        PostThreadMessageW(g_keyboardThreadId, WM_NULL, 0, 0);
    }
}

DWORD WINAPI KeyboardThreadProc(void*) {
    // Bounded by the stop event rather than an open-ended Sleep loop, so an early
    // unload cannot leave this spinning while waiting for a window that is never
    // going to appear.
    while (!g_hwnd && WaitForSingleObject(g_stopEvent, 10) == WAIT_TIMEOUT) {
    }

    bool quit = false;
    while (!quit && WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        const bool wanted = CapsLockHookWanted();
        if (wanted && !g_keyboardHook) {
            g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
        } else if (!wanted && g_keyboardHook) {
            UnhookWindowsHookEx(g_keyboardHook);
            g_keyboardHook = nullptr;
        }

        // A low-level hook is delivered through the installing thread's message
        // queue, so this has to keep pumping while the hook is up.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) {
            break;
        }

        // Blocks until the stop event fires, a message arrives, or the timeout.
        // LoadSettings posts a WM_NULL when the module is toggled so the hook is
        // reconciled at once; the timeout is only a backstop. A timed wait, not a
        // spin, so an idle keyboard thread costs nothing.
        MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, 1000, QS_ALLINPUT);
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

// --- Low-level mouse hook: wakes the parked render thread when the cursor
// approaches where the (currently OS-hidden) island sits, so hover-to-unhide
// keeps working without polling GetCursorPos every frame.
HHOOK g_mouseHook = nullptr;
HANDLE g_mouseThread = nullptr;
DWORD g_mouseThreadId = 0;

// Wakes the mouse thread so it re-evaluates whether the wake hook is needed.
// Posted whenever the island parks or unparks, so the hook is installed and
// removed in step with that rather than on the thread's backstop timeout.
inline void NotifyMouseThreadParkedChanged() {
    if (g_mouseThreadId != 0) {
        PostThreadMessageW(g_mouseThreadId, WM_NULL, 0, 0);
    }
}
std::atomic<int64_t> g_lastMouseWakeCheckMs = 0;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // WM_MOUSEWHEEL normally follows the focused window. The island is a
    // no-activate overlay, so forward the wheel while a hover-wake reveal is
    // active and the pointer is over the island without stealing keyboard focus.
    if (nCode == HC_ACTION &&
        (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) &&
        g_hoverWakeSession.load(std::memory_order_relaxed) &&
        !g_autoHiddenParked.load(std::memory_order_relaxed) &&
        !g_manuallyHidden.load(std::memory_order_relaxed)) {
        const auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        const RECT dockRect = GetIslandDockRect();
        const HWND hwnd = g_hwnd;
        RECT islandRect{};
        if (hwnd && info) {
            const bool overExpandedIsland = GetWindowRect(hwnd, &islandRect) &&
                                             PtInRect(&islandRect, info->pt);
            if (overExpandedIsland || PtInRect(&dockRect, info->pt)) {
                const WPARAM wheel = MAKEWPARAM(0, HIWORD(info->mouseData));
                const LPARAM point = MAKELPARAM(info->pt.x, info->pt.y);
                PostMessageW(hwnd, static_cast<UINT>(wParam), wheel, point);
                return 1;
            }
        }
    }

    if (nCode == HC_ACTION && wParam == WM_MOUSEMOVE &&
        g_settings.unhideOnHover &&
        (g_autoHiddenParked.load(std::memory_order_relaxed) ||
         g_manuallyHidden.load(std::memory_order_relaxed))) {
        const int64_t nowMs = static_cast<int64_t>(GetTickCount64());
        const int64_t last = g_lastMouseWakeCheckMs.load(std::memory_order_relaxed);
        if (nowMs - last >= 60) {  // ~16Hz check rate for responsive unhide
            g_lastMouseWakeCheckMs.store(nowMs, std::memory_order_relaxed);
            auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            HWND hwnd = g_hwnd;
            RECT dockRect = GetIslandDockRect();
            if (hwnd && PtInRect(&dockRect, info->pt)) {
                g_autoHiddenParked = false;
                g_manuallyHidden = false;
                Wh_SetIntValue(L"ManuallyHidden", 0);
                g_hoverWakeSession = true;
                g_hoverWakeInitialHover = true;
                g_hoverRevealUntil = 0.0;
                PostMessageW(hwnd, WM_APP_MOUSE_WAKE, 0, 0);
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// A system-wide WH_MOUSE_LL routes every mouse event on the desktop through this
// thread, so it is only installed while it can actually do something: the hook's
// whole job is to notice a hover over a *parked* island.
//
// Gating on the settings instead was not enough. UnhideOnHover and
// AutoHideFullscreen both default to true, so every user on defaults still got
// the hook from startup -- the exact case the gate was added to avoid. Tying it
// to the parked state means it exists only while the island is actually hidden.
static bool MouseWakeHookWanted() {
    return g_settings.unhideOnHover &&
           (g_autoHiddenParked.load(std::memory_order_relaxed) ||
            g_manuallyHidden.load(std::memory_order_relaxed) ||
            g_hoverWakeSession.load(std::memory_order_relaxed));
}

DWORD WINAPI MouseThreadProc(void*) {
    while (!g_hwnd && WaitForSingleObject(g_stopEvent, 10) == WAIT_TIMEOUT) {
    }

    bool quit = false;
    while (!quit && WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        const bool wanted = MouseWakeHookWanted();
        if (wanted && !g_mouseHook) {
            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
        } else if (!wanted && g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
        }

        // A low-level hook is delivered through the installing thread's message
        // queue, so this has to keep pumping while the hook is up.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) {
            break;
        }

        // Blocks until the stop event fires, a message arrives, or the timeout.
        // The render thread posts a WM_NULL when it parks or unparks, so the hook
        // is reconciled immediately rather than up to a tick later; the timeout
        // is only a backstop in case a transition is ever missed. This is a timed
        // wait, not a spin, so an idle mouse thread costs nothing.
        MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, 1000, QS_ALLINPUT);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    return 0;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static POINT s_touchStart = {0, 0};
    static ULONGLONG s_touchStartTime = 0;
    switch (msg) {
        case WM_CREATE:
            AddClipboardFormatListener(hwnd);
            LoadClipboardHistoryFromDisk();
            LoadFileTrayFromDisk();
            CaptureClipboardFilesToTray(hwnd);
            if (g_shellHookMessage == 0) g_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");
            if (g_taskbarCreatedMessage == 0) g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
            RegisterShellHookWindow(hwnd);
            // File Tray (#33). Accepting drops costs nothing when the module is
            // off; WM_DROPFILES just ignores them in that case.
            DragAcceptFiles(hwnd, TRUE);
            return 0;

        case WM_DROPFILES: {
            HDROP drop = reinterpret_cast<HDROP>(wParam);
            if (!g_settings.fileTrayModule) {
                DragFinish(drop);
                return 0;
            }

            const UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
            std::vector<FileTrayItem> added;
            added.reserve(count);

            for (UINT i = 0; i < count; ++i) {
                wchar_t path[MAX_PATH] = {};
                if (DragQueryFileW(drop, i, path, ARRAYSIZE(path)) == 0) {
                    continue;
                }

                FileTrayItem item;
                item.path = path;

                // BaseNameFromPath instead of PathFindFileNameW so we do not
                // pull in shlwapi just for this.
                item.name = BaseNameFromPath(path);
                if (item.name.empty()) {
                    item.name = path;
                }

                WIN32_FILE_ATTRIBUTE_DATA attr = {};
                if (GetFileAttributesExW(path, GetFileExInfoStandard, &attr)) {
                    item.isDirectory = (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                    item.sizeBytes = (static_cast<uint64_t>(attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
                }

                // Grab the real Explorer icon so the shelf looks native.
                SHFILEINFOW info = {};
                if (SHGetFileInfoW(path, 0, &info, sizeof(info), SHGFI_ICON | SHGFI_SMALLICON) && info.hIcon) {
                    IconToPixels(info.hIcon, 32, &item.icon);
                    DestroyIcon(info.hIcon);
                }

                added.push_back(std::move(item));
            }
            DragFinish(drop);

            if (!added.empty()) {
                {
                    std::lock_guard lock(g_stateMutex);
                    for (auto& item : added) {
                        // Re-dropping a file promotes the existing entry instead of
                        // creating a duplicate.
                        auto existing = std::find_if(g_state.fileTrayItems.begin(), g_state.fileTrayItems.end(),
                                                     [&](const FileTrayItem& other) {
                                                         return _wcsicmp(other.path.c_str(), item.path.c_str()) == 0;
                                                     });
                        if (existing != g_state.fileTrayItems.end()) {
                            g_state.fileTrayItems.erase(existing);
                        }
                        g_state.fileTrayItems.push_back(std::move(item));
                    }

                    const size_t cap = static_cast<size_t>(std::max(1, g_settings.fileTrayMaxItems));
                    while (g_state.fileTrayItems.size() > cap) {
                        g_state.fileTrayItems.erase(g_state.fileTrayItems.begin());
                    }
                }
                SaveFileTrayToDisk();
            }

            // Jump by stable section ID so the drop cannot land on an adjacent
            // dashboard when optional sections change the numeric tab order.
            g_fileTrayScrollOffset.store(0, std::memory_order_relaxed);
            const int trayTab = DashboardIndexForSectionId(g_settings, 5);
            if (g_settings.fileTrayModule) {
                SetDashboardTabIndex(g_settings, trayTab);
            }
            g_layoutDirty = true;
            return 0;
        }

        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
            DeregisterShellHookWindow(hwnd);
            return 0;

        // Both of these touch the window from the thread that owns it, the only
        // thread allowed to bind a hot key to it or reshape it.
        case WM_APP_APPLY_HOTKEY:
            ApplyHideShowHotkey();
            return 0;

        case WM_APP_APPLY_BACKDROP: {
            ApplyBackdropMaterial(hwnd);
            RECT rc{};
            if (GetWindowRect(hwnd, &rc)) {
                ApplyBackdropRegion(hwnd, rc.right - rc.left, rc.bottom - rc.top);
            }
            return 0;
        }

        case WM_APP_MOUSE_WAKE:
            // No-op payload — its only job is to wake MsgWaitForMultipleObjects
            // and get drained by the PeekMessage pump.
            return 0;

        case WM_APP_CAPSLOCK: {
            if (!g_settings.capsLock) return 0;
            bool isNum = (wParam == VK_NUMLOCK);
            bool capsOn = (lParam & 1) != 0;
            bool numOn = (lParam & 2) != 0;
            {
                std::lock_guard lock(g_stateMutex);
                g_state.capsLock.active = true;
                g_state.capsLock.capsOn = capsOn;
                g_state.capsLock.numOn = numOn;
                g_state.capsLock.isNumEvent = isNum;
                g_state.capsLock.expiresAt = NowSeconds() + 2.5;
            }
            TriggerNudge();
            return 0;
        }

        case WM_DEVICECHANGE: {
            // DBT_DEVICEARRIVAL = 0x8000, DBT_DEVICEREMOVECOMPLETE = 0x8004
            if (wParam == 0x8000 || wParam == 0x8004) {
                bool arrived = (wParam == 0x8000);
                std::wstring devName;
                bool isBt = false;

                if (lParam) {
                    auto* hdr = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);
                    if (hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                        devName = L"USB Drive";
                    } else if (hdr->dbch_devicetype == DBT_DEVTYP_PORT) {
                        devName = L"COM Device";
                    } else {
                        // Generic/Bluetooth OEM
                        isBt = true;
                        devName = L"Bluetooth Device";
                    }
                }

                {
                    std::lock_guard lock(g_stateMutex);
                    g_state.device.active = true;
                    g_state.device.eventType = arrived ? DeviceEventType::Connected
                                                       : DeviceEventType::Disconnected;
                    g_state.device.deviceName = devName;
                    g_state.device.isBluetoothLike = isBt;
                    g_state.device.expiresAt = NowSeconds() + 3.0;
                }
                TriggerNudge();
            }
            return 0;
        }

        case WM_POWERBROADCAST: {
            if (wParam == PBT_APMRESUMESUSPEND || wParam == PBT_APMRESUMEAUTOMATIC || wParam == PBT_APMRESUMECRITICAL) {
                if (g_settingsChangedEvent) {
                    SetEvent(g_settingsChangedEvent);
                }
                {
                    std::lock_guard lock(g_stateMutex);
                    g_state.weather.lastUpdated = 0.0;
                }
                TriggerNudge();
            }
            return TRUE;
        }

        case WM_CLIPBOARDUPDATE:
            if (g_settings.clipboard || g_settings.clipboardSection) {
                CaptureClipboard(hwnd);
                g_layoutDirty = true;
            }
            if (g_settings.fileTrayModule || g_settings.fileCopyNotifications) CaptureClipboardFilesToTray(hwnd);
            return 0;



        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);

            RECT clientRect = {};
            GetClientRect(hwnd, &clientRect);

            const int minX = static_cast<int>(kRenderPadX);
            const int maxX = clientRect.right - static_cast<int>(kRenderPadX);
            const int minY = (g_settings.notchStyle || g_settings.borderMergedMode) ? 0 : static_cast<int>(kRenderPadY);
            const int maxY = clientRect.bottom - static_cast<int>(kRenderPadY);

            if (maxX > minX && maxY > minY) {
                if (pt.x < minX || pt.x > maxX || pt.y < minY || pt.y > maxY) {
                    return HTTRANSPARENT;
                }
            }
            return HTCLIENT;
        }

        case WM_APP_LAYOUT_CHANGED:
            g_layoutDirty = true;
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                if (g_scrubbing.load()) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }

                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                bool mediaActive = false;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaActive = g_settings.media && g_state.media.available;
                }

                const int currentTab = NormalizedTabIndex(g_settings);
                if (mediaActive && IsMediaPlaybackTab(g_settings, currentTab)) {
                    const MediaContentPoint cp = MediaContentFromClient(pt.x, pt.y);
                    const bool hoverClickable = MediaArtHitTest(cp) ||
                                               MediaVisualizerHitTest(cp) ||
                                               MediaTransportHitTest(cp) != -1 ||
                                               MediaVolumeIconHitTest(cp) ||
                                               MediaScrubFractionFromContent(cp) >= 0.0f ||
                                               MediaVolumeHoverHitTest(cp, g_hoveredMediaVolume.load()) ||
                                               MediaVolumeSliderFraction(cp) >= 0.0f;
                    if (hoverClickable) {
                        SetCursor(LoadCursorW(nullptr, IDC_HAND));
                        return TRUE;
                    }
                }

                if (g_settings.calendarModule &&
                    currentTab == DashboardIndexForSectionId(g_settings, 3)) {
                    const MediaContentPoint cp = MediaContentFromClient(pt.x, pt.y);
                    SYSTEMTIME calendarToday = {};
                    GetLocalTime(&calendarToday);
                    InitializeCalendarSelection(calendarToday);
                    const int pickerMode = g_calendarPickerMode.load(std::memory_order_relaxed);
                    if (CalendarHeaderActionAtContentPoint(cp) != 0 ||
                        (pickerMode != 0 ? CalendarPickerCellAtContentPoint(cp) >= 0
                                         : CalendarDayAtContentPoint(cp,
                                             g_calendarViewYear.load(std::memory_order_relaxed),
                                             g_calendarViewMonth.load(std::memory_order_relaxed)) > 0)) {
                        SetCursor(LoadCursorW(nullptr, IDC_HAND));
                        return TRUE;
                    }
                }

                if (g_settings.clipboardSection &&
                    currentTab == DashboardIndexForSectionId(g_settings, 2)) {
                    const MediaContentPoint cp = MediaContentFromClient(pt.x, pt.y);
                    std::shared_ptr<const std::vector<ClipboardHistoryItem>> history;
                    {
                        std::lock_guard lock(g_stateMutex);
                        history = g_state.clipboardHistory;
                    }
                    const size_t count = history ? history->size() : 0;
                    const int index = ClipboardHistoryItemAtContentPoint(
                        cp, count, g_clipboardHistoryPage.load(std::memory_order_relaxed));
                    if (index >= 0) {
                        SetCursor(LoadCursorW(nullptr, IDC_HAND));
                        return TRUE;
                    }
                }

                // A File Tray row under the cursor is clickable (opens the file).
                if (g_settings.fileTrayModule && currentTab == FileTrayTabIndex(g_settings) &&
                    g_hoveredFileTrayRow.load(std::memory_order_relaxed) >= 0) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
                if (g_settings.quickAccess &&
                    currentTab == DashboardIndexForSectionId(g_settings, 8) &&
                    QuickAccessTileAtContentPoint(MediaContentFromClient(pt.x, pt.y),
                        g_settings.quickAccessLinks.size()) >= 0) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);

                s_touchStart.x = xPos;
                s_touchStart.y = yPos;
                s_touchStartTime = GetTickCount64();

                bool mediaActive = false;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaActive = g_settings.media && g_state.media.available;
                }

                const int currentTab = NormalizedTabIndex(g_settings);
                if (mediaActive && IsMediaPlaybackTab(g_settings, currentTab)) {
                    const MediaContentPoint cp = MediaContentFromClient(xPos, yPos);

                    if (MediaVolumeIconHitTest(cp)) {
                        ToggleEndpointMute();
                        return 0;
                    }

                    const float volumeFraction = MediaVolumeSliderFraction(cp);
                    if (volumeFraction >= 0.0f) {
                        g_mediaVolumeDragging = true;
                        g_hoveredMediaVolume = true;
                        SetEndpointVolumeScalar(volumeFraction);
                        SetCapture(hwnd);
                        return 0;
                    }

                    const int cmd = MediaTransportHitTest(cp);
                    if (cmd != -1) {
                        g_pressedMediaButton = cmd;
                        SetCapture(hwnd);
                        g_layoutDirty = true;
                        return 0;
                    }

                    const float fraction = MediaScrubFractionFromContent(cp);
                    if (fraction >= 0.0f) {
                        g_scrubDragFraction.store(fraction, std::memory_order_relaxed);
                        g_scrubbing = true;
                        g_lastLiveSeekTime = 0.0;
                        SetCapture(hwnd);
                        g_layoutDirty = true;
                        return 0;
                    }
                }

                const MediaContentPoint dragPoint = MediaContentFromClient(xPos, yPos);
                if (dragPoint.valid && g_settings.clipboardSection &&
                    currentTab == DashboardIndexForSectionId(g_settings, 2) &&
                    !ClipboardClearButtonAtContentPoint(dragPoint)) {
                    std::shared_ptr<const std::vector<ClipboardHistoryItem>> history;
                    {
                        std::lock_guard lock(g_stateMutex);
                        history = g_state.clipboardHistory;
                    }
                    const int item = ClipboardHistoryItemAtContentPoint(
                        dragPoint, history ? history->size() : 0,
                        g_clipboardHistoryPage.load(std::memory_order_relaxed));
                    if (item >= 0 && history) {
                        g_contentDragKind = 1;
                        g_contentDragClipboardId = (*history)[static_cast<size_t>(item)].id;
                        g_contentDragFilePath.clear();
                    }
                } else if (dragPoint.valid && g_settings.fileTrayModule &&
                           currentTab == FileTrayTabIndex(g_settings) &&
                           !FileTrayClearButtonAtContentPoint(dragPoint)) {
                    std::lock_guard lock(g_stateMutex);
                    const int row = FileTrayRowAtContentPoint(
                        dragPoint, static_cast<int>(g_state.fileTrayItems.size()));
                    if (row >= 0) {
                        const size_t index = g_state.fileTrayItems.size() - 1 -
                            static_cast<size_t>(g_fileTrayScrollOffset.load(std::memory_order_relaxed) + row);
                        g_contentDragKind = 2;
                        g_contentDragFilePath = g_state.fileTrayItems[index].path;
                        g_contentDragClipboardId = 0;
                    }
                }
                if (g_contentDragKind != 0) {
                    g_contentDragStart = POINT{xPos, yPos};
                    g_contentDragPoint = POINT{static_cast<LONG>(dragPoint.x),
                                               static_cast<LONG>(dragPoint.y)};
                    g_contentDragActive = false;
                    SetCapture(hwnd);
                    return 0;
                }
            }
            break;

        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);

            const int xPos = GET_X_LPARAM(lParam);
            const int yPos = GET_Y_LPARAM(lParam);

            if (g_contentDragKind != 0 && GetCapture() == hwnd) {
                const MediaContentPoint dragPoint = MediaContentFromClient(xPos, yPos);
                if (dragPoint.valid) {
                    g_contentDragPoint = POINT{static_cast<LONG>(dragPoint.x),
                                               static_cast<LONG>(dragPoint.y)};
                }
                if (!g_contentDragActive &&
                    (std::abs(xPos - g_contentDragStart.x) >= 8 ||
                     std::abs(yPos - g_contentDragStart.y) >= 8)) {
                    g_contentDragActive = true;
                }
                if (g_contentDragActive) {
                    g_layoutDirty = true;
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return 0;
                }
            }

            if (g_mediaVolumeDragging.load(std::memory_order_relaxed)) {
                const MediaContentPoint cp = MediaContentFromClient(xPos, yPos);
                const float fraction = MediaVolumeSliderFraction(cp);
                if (fraction >= 0.0f) {
                    SetEndpointVolumeScalar(fraction);
                }
                return 0;
            }

            if (g_scrubbing.load()) {
                const MediaContentPoint cp = MediaContentFromClient(xPos, yPos);
                const float fraction = MediaScrubFractionUnbounded(cp);
                if (fraction < 0.0f) {
                    return 0;
                }
                g_scrubDragFraction.store(fraction, std::memory_order_relaxed);
                g_layoutDirty = true;

                const double now = NowSeconds();
                if (now - g_lastLiveSeekTime.load(std::memory_order_relaxed) >= 0.15) {
                    g_lastLiveSeekTime.store(now, std::memory_order_relaxed);
                    int64_t startTicks = 0;
                    int64_t endTicks = 0;
                    int64_t minSeekTicks = 0;
                    int64_t maxSeekTicks = 0;
                    {
                        std::lock_guard lock(g_stateMutex);
                        startTicks = g_state.media.startTicks;
                        endTicks = g_state.media.endTicks;
                        minSeekTicks = g_state.media.minSeekTicks;
                        maxSeekTicks = g_state.media.maxSeekTicks;
                    }
                    if (endTicks > startTicks) {
                        const int64_t seekStart = maxSeekTicks > minSeekTicks ? minSeekTicks : startTicks;
                        const int64_t seekEnd = maxSeekTicks > minSeekTicks ? maxSeekTicks : endTicks;
                        const int64_t targetTicks = seekStart + static_cast<int64_t>(
                            fraction * (seekEnd - seekStart));
                        SeekMediaToTicks(targetTicks);
                    }
                }
                return 0;
            }

            int hovered = -1;
            bool mediaActive = false;
            {
                std::lock_guard lock(g_stateMutex);
                mediaActive = g_settings.media && g_state.media.available;
            }
            const int currentTab = NormalizedTabIndex(g_settings);
            bool volumeHovered = false;

            if (mediaActive && IsMediaPlaybackTab(g_settings, currentTab)) {
                const MediaContentPoint cp = MediaContentFromClient(xPos, yPos);
                hovered = MediaTransportHitTest(cp);
                volumeHovered = MediaVolumeHoverHitTest(cp, g_hoveredMediaVolume.load());
                if (MediaVolumeSliderFraction(cp) >= 0.0f) volumeHovered = true;
            }

            if (g_hoveredMediaVolume.exchange(volumeHovered) != volumeHovered) {
                g_layoutDirty = true;
            }

            if (g_hoveredMediaButton.exchange(hovered) != hovered) {
                g_layoutDirty = true;
            }

            // File Tray row highlight.
            int hoveredRow = -1;
            if (g_settings.fileTrayModule && currentTab == FileTrayTabIndex(g_settings)) {
                size_t trayCount = 0;
                {
                    std::lock_guard lock(g_stateMutex);
                    trayCount = g_state.fileTrayItems.size();
                }
                hoveredRow = FileTrayRowAtContentPoint(MediaContentFromClient(xPos, yPos),
                                                       static_cast<int>(trayCount));
            }
            if (g_hoveredFileTrayRow.exchange(hoveredRow) != hoveredRow) {
                g_layoutDirty = true;
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoveredMediaButton.exchange(-1) != -1) {
                g_layoutDirty = true;
            }
            if (g_hoveredMediaVolume.exchange(false)) {
                g_layoutDirty = true;
            }
            return 0;

        case WM_CAPTURECHANGED:
            if (reinterpret_cast<HWND>(lParam) != hwnd) {
                g_contentDragKind = 0;
                g_contentDragClipboardId = 0;
                g_contentDragFilePath.clear();
                g_contentDragActive = false;
                if (g_mediaVolumeDragging.exchange(false)) {
                    g_layoutDirty = true;
                }
                if (g_scrubbing.exchange(false)) {
                    g_layoutDirty = true;
                }
                if (g_pressedMediaButton.exchange(-1) != -1) {
                    g_layoutDirty = true;
                }
                if (g_hoveredMediaButton.exchange(-1) != -1) {
                    g_layoutDirty = true;
                }
            }
            return 0;

        case WM_LBUTTONUP:
            {
                if (g_contentDragKind != 0) {
                    const int dragKind = g_contentDragKind;
                    const uint64_t clipboardId = g_contentDragClipboardId;
                    const std::wstring filePath = g_contentDragFilePath;
                    const bool wasDragging = g_contentDragActive;
                    const MediaContentPoint dropPoint = MediaContentFromClient(
                        GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    g_contentDragKind = 0;
                    g_contentDragClipboardId = 0;
                    g_contentDragFilePath.clear();
                    g_contentDragActive = false;
                    ReleaseCapture();
                    if (wasDragging) {
                        if (dragKind == 1 && ClipboardClearButtonAtContentPoint(dropPoint)) {
                            RemoveUnpinnedClipboardItem(clipboardId);
                        } else if (dragKind == 2 && FileTrayClearButtonAtContentPoint(dropPoint)) {
                            {
                                std::lock_guard lock(g_stateMutex);
                                auto& items = g_state.fileTrayItems;
                                items.erase(std::remove_if(items.begin(), items.end(),
                                    [&filePath](const FileTrayItem& item) {
                                        return _wcsicmp(item.path.c_str(), filePath.c_str()) == 0;
                                    }), items.end());
                            }
                            SaveFileTrayToDisk();
                            g_hoveredFileTrayRow = -1;
                            g_layoutDirty = true;
                        }
                        g_layoutDirty = true;
                        return 0;
                    }
                }
                if (g_mediaVolumeDragging.exchange(false, std::memory_order_relaxed)) {
                    const MediaContentPoint cp = MediaContentFromClient(
                        GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    const float fraction = MediaVolumeSliderFraction(cp);
                    if (fraction >= 0.0f) {
                        SetEndpointVolumeScalar(fraction);
                    }
                    ReleaseCapture();
                    g_layoutDirty = true;
                    return 0;
                }
                if (g_scrubbing.load()) {
                    g_scrubbing = false;
                    ReleaseCapture();

                    const float finalFraction = Clamp(g_scrubDragFraction.load(std::memory_order_relaxed), 0.0f, 1.0f);
                    int64_t startTicks = 0;
                    int64_t endTicks = 0;
                    int64_t minSeekTicks = 0;
                    int64_t maxSeekTicks = 0;
                    {
                        std::lock_guard lock(g_stateMutex);
                        startTicks = g_state.media.startTicks;
                        endTicks = g_state.media.endTicks;
                        minSeekTicks = g_state.media.minSeekTicks;
                        maxSeekTicks = g_state.media.maxSeekTicks;
                    }
                    if (endTicks > startTicks) {
                        const int64_t seekStart = maxSeekTicks > minSeekTicks ? minSeekTicks : startTicks;
                        const int64_t seekEnd = maxSeekTicks > minSeekTicks ? maxSeekTicks : endTicks;
                        const int64_t targetTicks = seekStart + static_cast<int64_t>(
                            finalFraction * (seekEnd - seekStart));
                        SeekMediaToTicks(targetTicks);
                        std::lock_guard lock(g_stateMutex);
                        g_state.media.positionTicks = targetTicks;
                        g_state.media.lastUpdatedTicks = GetTickCount64();
                    }
                    g_layoutDirty = true;
                    return 0;
                }

                if (g_pressedMediaButton.load() != -1) {
                    g_pressedMediaButton = -1;
                    ReleaseCapture();
                    g_layoutDirty = true;
                }

                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);

                ULONGLONG now = GetTickCount64();
                if (s_touchStartTime > 0 && (now - s_touchStartTime) < 500) {
                    int dx = xPos - s_touchStart.x;
                    if (abs(dx) > 40) { // Horizontal swipe threshold
                        bool switchedMediaSource = false;
                        if (IsMediaPlaybackTab(g_settings, NormalizedTabIndex(g_settings))) {
                            std::vector<std::wstring> sources;
                            std::wstring activeSource;
                            {
                                std::lock_guard lock(g_mediaSourceMutex);
                                sources = g_mediaSourceIds;
                            }
                            {
                                std::lock_guard lock(g_stateMutex);
                                activeSource = g_state.media.sourceAppUserModelId;
                            }
                            if (sources.size() > 1) {
                                auto it = std::find(sources.begin(), sources.end(), activeSource);
                                int index = it == sources.end() ? 0 : static_cast<int>(it - sources.begin());
                                index = (index + (dx > 0 ? -1 : 1) +
                                         static_cast<int>(sources.size())) %
                                        static_cast<int>(sources.size());
                                {
                                    std::lock_guard lock(g_mediaSourceMutex);
                                    g_preferredMediaSourceId = sources[static_cast<size_t>(index)];
                                }
                                Wh_SetStringValue(L"LastMediaSourceId",
                                                  sources[static_cast<size_t>(index)].c_str());
                                TriggerNudge();
                                g_layoutDirty = true;
                                switchedMediaSource = true;
                            }
                        }
                        const int maxTabs = ActiveTabCount(g_settings);
                        if (!switchedMediaSource && maxTabs > 1) {
                            if (dx > 0) { // Swipe right -> previous tab
                                SetDashboardTabIndex(g_settings, NormalizedTabIndex(g_settings) - 1);
                            } else { // Swipe left -> next tab
                                SetDashboardTabIndex(g_settings, NormalizedTabIndex(g_settings) + 1);
                            }
                        }
                        s_touchStartTime = 0;
                        return 0; // Consume swipe gesture
                    }
                }
                s_touchStartTime = 0;

                bool mediaActive = false;
                std::vector<IslandKind> kinds;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaActive = g_settings.media && g_state.media.available;
                    kinds = ChooseActivities(g_state, g_settings, NowSeconds());
                }
                bool expanded = Wh_GetIntValue(L"PinnedExpanded", 0) != 0 || g_clickExpanded.load();
                if (!g_settings.expandOnHover && !expanded) {
                    g_clickExpanded = true;
                    g_layoutDirty = true;
                    return 0; // consumed click to expand
                }

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                const float height = static_cast<float>(clientRect.bottom - clientRect.top);

                const int currentTab = NormalizedTabIndex(g_settings);

                const MediaContentPoint cp = MediaContentFromClient(xPos, yPos);

                if (mediaActive && IsMediaPlaybackTab(g_settings, currentTab) &&
                    MediaVisualizerHitTest(cp)) {
                    g_visualizerStyle.store(
                        (g_visualizerStyle.load(std::memory_order_relaxed) + 1) % 2,
                        std::memory_order_relaxed);
                    g_layoutDirty = true;
                    return 0;
                }

                if (g_settings.quickAccess &&
                    currentTab == DashboardIndexForSectionId(g_settings, 8)) {
                    const int link = QuickAccessTileAtContentPoint(cp,
                        g_settings.quickAccessLinks.size());
                    if (link >= 0) {
                        OpenQuickAccessLink(g_settings, static_cast<size_t>(link));
                        return 0;
                    }
                }

                if (g_settings.calendarModule &&
                    currentTab == DashboardIndexForSectionId(g_settings, 3) &&
                    HandleCalendarDashboardClick(cp)) {
                    return 0;
                }

                if (g_settings.clipboardSection &&
                    currentTab == DashboardIndexForSectionId(g_settings, 2) && cp.valid) {
                    if (ClipboardClearButtonAtContentPoint(cp)) {
                        ClearUnpinnedClipboardHistory();
                        return 0;
                    }
                    std::shared_ptr<const std::vector<ClipboardHistoryItem>> history;
                    {
                        std::lock_guard lock(g_stateMutex);
                        history = g_state.clipboardHistory;
                    }
                    const size_t count = history ? history->size() : 0;
                    const int page = g_clipboardHistoryPage.load(std::memory_order_relaxed);
                    const int itemIndex = ClipboardHistoryItemAtContentPoint(cp, count, page);
                    if (itemIndex >= 0) {
                        const ClipboardHistoryItem& item = (*history)[static_cast<size_t>(itemIndex)];
                        CopyClipboardHistoryItem(hwnd, item.id);
                        return 0;
                    }
                }

                // The clipboard dashboard's paging controls use the same
                // content-space transform as its drawing code, including DPI
                // scale and hover expansion.
                if (g_settings.clipboardSection &&
                    currentTab == DashboardIndexForSectionId(g_settings, 2) && cp.valid) {
                    size_t historyCount = 0;
                    {
                        std::lock_guard lock(g_stateMutex);
                        if (g_state.clipboardHistory) historyCount = g_state.clipboardHistory->size();
                    }
                    const int pageCount = ClipboardHistoryPageCount(historyCount);
                    const bool previousPageClick = cp.x >= 14.0f && cp.x <= 42.0f &&
                                                   cp.y >= 7.0f && cp.y <= 26.0f;
                    const bool nextPageClick = cp.x >= 14.0f && cp.x <= 42.0f &&
                                               cp.y >= cp.height * 0.5f + 60.0f &&
                                               cp.y <= cp.height * 0.5f + 78.0f;
                    if (pageCount > 1 && (previousPageClick || nextPageClick)) {
                        int page = g_clipboardHistoryPage.load(std::memory_order_relaxed);
                        if (previousPageClick) {
                            page = (page + pageCount - 1) % pageCount;
                        } else if (nextPageClick) {
                            page = (page + 1) % pageCount;
                        } else {
                            page = -1;
                        }
                        if (page >= 0) {
                            g_clipboardHistoryPage.store(page, std::memory_order_relaxed);
                            g_layoutDirty = true;
                            return 0;
                        }
                    }
                }

                // File Tray: clicking a row opens that file with its default app.
                if (g_settings.fileTrayModule && currentTab == FileTrayTabIndex(g_settings)) {
                    if (FileTrayClearButtonAtContentPoint(cp)) {
                        {
                            std::lock_guard lock(g_stateMutex);
                            g_state.fileTrayItems.clear();
                        }
                        SaveFileTrayToDisk();
                        g_hoveredFileTrayRow = -1;
                        g_fileTrayScrollOffset = 0;
                        g_layoutDirty = true;
                        return 0;
                    }
                    std::wstring toOpen;
                    bool showItemMenu = false;
                    {
                        std::lock_guard lock(g_stateMutex);
                        const int row = FileTrayRowAtContentPoint(
                            cp, static_cast<int>(g_state.fileTrayItems.size()));
                        if (row >= 0) {
                            // Rows render newest-first, so map back from the end.
                            const size_t index = g_state.fileTrayItems.size() - 1 -
                                static_cast<size_t>(g_fileTrayScrollOffset.load(std::memory_order_relaxed) + row);
                            toOpen = g_state.fileTrayItems[index].path;
                            showItemMenu = FileTrayMoreButtonAtContentPoint(cp, row);
                        }
                    }
                    if (!toOpen.empty()) {
                        if (showItemMenu) {
                            POINT screenPoint = {xPos, yPos};
                            ClientToScreen(hwnd, &screenPoint);
                            ShowFileTrayItemMenu(hwnd, screenPoint, toOpen);
                            return 0;
                        }
                        SHELLEXECUTEINFOW sei = {sizeof(sei)};
                        sei.fMask = SEE_MASK_FLAG_NO_UI;
                        sei.lpFile = toOpen.c_str();
                        sei.nShow = SW_SHOWNORMAL;
                        ShellExecuteExW(&sei);
                        return 0;
                    }
                    // A click on the empty shelf should not fall through to
                    // "focus the media app".
                    return 0;
                }

                if (mediaActive && IsMediaPlaybackTab(g_settings, currentTab)) {
                    const int cmd = MediaTransportHitTest(cp);
                    if (cmd != -1) {
                        SendMediaTransportCommand(cmd);
                        return 0;
                    }

                    // A release over the scrubber that never went through the
                    // drag path (press started elsewhere) is swallowed rather
                    // than falling through to "open the app". The actual seek
                    // is handled by the g_scrubbing branch above.
                    if (MediaScrubFractionFromContent(cp) >= 0.0f) {
                        return 0;
                    }
                }

                if (height > 50.0f) {
                    if (mediaActive) {
                        if (IsMediaPlaybackTab(g_settings, currentTab)) {
                            OpenRelevantApp();
                        }
                    } else if (kinds.empty() || kinds[0] != IslandKind::Idle) {
                        HandleStatusClickAtPoint(hwnd, lParam);
                    }
                } else {
                    if (mediaActive) {
                        OpenRelevantApp();
                    } else {
                        HandleStatusClickAtPoint(hwnd, lParam);
                    }
                }
            }
            return 0;

        case WM_MBUTTONUP:
            ToggleEndpointMute();
            return 0;

        case WM_LBUTTONDBLCLK:
            Wh_SetIntValue(L"PinnedExpanded", Wh_GetIntValue(L"PinnedExpanded", 0) ? 0 : 1);
            return 0;

        case WM_MOUSEWHEEL: {
            static ULONGLONG lastScrollTime = 0;
            ULONGLONG now = GetTickCount64();
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            const int currentTab = NormalizedTabIndex(g_settings);
            if (delta != 0 && IsMediaPlaybackTab(g_settings, currentTab)) {
                POINT cursor = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &cursor);
                const MediaContentPoint cp = MediaContentFromClient(cursor.x, cursor.y);
                if (MediaVolumeHoverHitTest(cp, g_hoveredMediaVolume.load()) ||
                    MediaVolumeSliderFraction(cp) >= 0.0f) {
                    AdjustEndpointVolume(delta);
                    return 0;
                }
            }

            if (now - lastScrollTime < 100) return 0;
            lastScrollTime = now;

            if (delta != 0 && g_settings.calendarModule &&
                currentTab == DashboardIndexForSectionId(g_settings, 3) &&
                (GetKeyState(VK_SHIFT) & 0x8000) != 0) {
                ShiftCalendarViewByMonths(delta > 0 ? -1 : 1);
                return 0;
            }

            if (g_settings.fileTrayModule && delta != 0 &&
                currentTab == FileTrayTabIndex(g_settings) &&
                (GetKeyState(VK_SHIFT) & 0x8000) != 0) {
                size_t itemCount = 0;
                {
                    std::lock_guard lock(g_stateMutex);
                    itemCount = g_state.fileTrayItems.size();
                }
                const int capacity = FileTrayLayout::VisibleRowCapacity(MediaLayout::kExpandedHeight);
                const int maxOffset = std::max(0, static_cast<int>(itemCount) - capacity);
                const int direction = delta > 0 ? -1 : 1;
                const int offset = ClampInt(g_fileTrayScrollOffset.load(std::memory_order_relaxed) + direction,
                                            0, maxOffset);
                g_fileTrayScrollOffset.store(offset, std::memory_order_relaxed);
                g_layoutDirty = true;
                return 0;
            }

            const int maxTabs = ActiveTabCount(g_settings);
            if (maxTabs > 1 && delta != 0) {
                if (g_settings.clipboardSection &&
                    currentTab == DashboardIndexForSectionId(g_settings, 2) &&
                    (GetKeyState(VK_SHIFT) & 0x8000) != 0) {
                    size_t historyCount = 0;
                    {
                        std::lock_guard lock(g_stateMutex);
                        if (g_state.clipboardHistory) historyCount = g_state.clipboardHistory->size();
                    }
                    const int pageCount = ClipboardHistoryPageCount(historyCount);
                    if (pageCount > 1) {
                        const int direction = delta > 0 ? -1 : 1;
                        const int page = g_clipboardHistoryPage.load(std::memory_order_relaxed);
                        g_clipboardHistoryPage.store((page + direction + pageCount) % pageCount,
                                                     std::memory_order_relaxed);
                        g_layoutDirty = true;
                        return 0;
                    }
                }
                SetDashboardTabIndex(g_settings,
                                     NormalizedTabIndex(g_settings) + (delta > 0 ? -1 : 1));
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const POINT clientPoint = pt;
            const int currentTab = NormalizedTabIndex(g_settings);
            const MediaContentPoint cp = MediaContentFromClient(clientPoint.x, clientPoint.y);
            if (g_settings.clipboardSection &&
                currentTab == DashboardIndexForSectionId(g_settings, 2) && cp.valid) {
                std::shared_ptr<const std::vector<ClipboardHistoryItem>> history;
                {
                    std::lock_guard lock(g_stateMutex);
                    history = g_state.clipboardHistory;
                }
                const size_t count = history ? history->size() : 0;
                const int index = ClipboardHistoryItemAtContentPoint(
                    cp, count, g_clipboardHistoryPage.load(std::memory_order_relaxed));
                if (index >= 0) {
                    const uint64_t itemId = (*history)[static_cast<size_t>(index)].id;
                    ClientToScreen(hwnd, &pt);
                    ShowClipboardHistoryItemMenu(hwnd, pt, itemId);
                    return 0;
                }
            }
            if (g_settings.fileTrayModule &&
                currentTab == FileTrayTabIndex(g_settings) && cp.valid) {
                std::wstring path;
                {
                    std::lock_guard lock(g_stateMutex);
                    const int row = FileTrayRowAtContentPoint(
                        cp, static_cast<int>(g_state.fileTrayItems.size()));
                    if (row >= 0) {
                        const size_t index = g_state.fileTrayItems.size() - 1 -
                            static_cast<size_t>(g_fileTrayScrollOffset.load(std::memory_order_relaxed) + row);
                        path = g_state.fileTrayItems[index].path;
                    }
                }
                if (!path.empty()) {
                    ClientToScreen(hwnd, &pt);
                    ShowFileTrayItemMenu(hwnd, pt, path);
                    return 0;
                }
            }
            ClientToScreen(hwnd, &pt);
            ShowContextMenu(hwnd, pt);
            return 0;
        }

        case WM_HOTKEY: {
            if (wParam == ID_HIDE_SHOW_HOTKEY) {
                const bool enabled = !g_autoHideEnabled.load(std::memory_order_relaxed);
                g_autoHideEnabled.store(enabled, std::memory_order_relaxed);
                Wh_SetIntValue(L"AutoHideEnabled", enabled ? 1 : 0);
                g_autoHiddenParked.store(false, std::memory_order_relaxed);
                g_fullscreenOverrideVisible.store(false, std::memory_order_relaxed);
                if (!enabled) {
                    g_hotkeyUnhideUntil.store(NowSeconds() + 6.0, std::memory_order_relaxed);
                    if (!g_manuallyHidden.load(std::memory_order_relaxed)) {
                        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                        PostMessageW(hwnd, WM_APP_NEW_EVENT, 0, 0);
                    }
                }
                NotifyMouseThreadParkedChanged();
                g_layoutDirty = true;
            } else if (wParam == ID_MEDIA_AUTOEXPAND_HOTKEY &&
                       g_mediaAutoExpandHotkeyEnabled.load(std::memory_order_relaxed)) {
                const bool enabled = !g_mediaAutoExpandEnabled.load(std::memory_order_relaxed);
                g_mediaAutoExpandEnabled.store(enabled, std::memory_order_relaxed);
                Wh_SetIntValue(L"MediaAutoExpandRuntime", enabled ? 1 : 0);
                g_layoutDirty = true;
            }
            return 0;
        }
    }

    if (msg == g_shellHookMessage && g_shellHookMessage != 0) {
        if (wParam == HSHELL_WINDOWCREATED) {
            CaptureShellNotification(reinterpret_cast<HWND>(lParam));
        }
        return 0;
    }

    if (msg == g_taskbarCreatedMessage && g_taskbarCreatedMessage != 0) {
        Wh_Log(L"TaskbarCreated received; re-registering shell hook window.");
        DeregisterShellHookWindow(hwnd);
        RegisterShellHookWindow(hwnd);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI RenderThreadProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kWindowClass, L"Dynamic Island Pro", WS_POPUP, 0, 0, 520, 140,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!hwnd) {
        Wh_Log(L"Failed to create Dynamic Island overlay window.");
        if (SUCCEEDED(hrCo)) {
            CoUninitialize();
        }
        return 0;
    }

    g_hwnd = hwnd;
    if (g_shellHookMessage == 0) g_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");
    if (g_taskbarCreatedMessage == 0) g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    using ChangeWindowMessageFilterEx_t = BOOL(WINAPI*)(HWND, UINT, DWORD, PVOID);
    static auto pChangeWindowMessageFilterEx = reinterpret_cast<ChangeWindowMessageFilterEx_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "ChangeWindowMessageFilterEx"));
    if (pChangeWindowMessageFilterEx) {
        if (g_shellHookMessage) pChangeWindowMessageFilterEx(hwnd, g_shellHookMessage, 1 /*MSGFLT_ALLOW*/, nullptr);
        if (g_taskbarCreatedMessage) pChangeWindowMessageFilterEx(hwnd, g_taskbarCreatedMessage, 1 /*MSGFLT_ALLOW*/, nullptr);
        pChangeWindowMessageFilterEx(hwnd, WM_COPYDATA, 1 /*MSGFLT_ALLOW*/, nullptr);
        pChangeWindowMessageFilterEx(hwnd, 0x0049 /*WM_COPYGLOBALDATA*/, 1 /*MSGFLT_ALLOW*/, nullptr);
    } else {
        using ChangeWindowMessageFilter_t = BOOL(WINAPI*)(UINT, DWORD);
        static auto pChangeWindowMessageFilter = reinterpret_cast<ChangeWindowMessageFilter_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "ChangeWindowMessageFilter"));
        if (pChangeWindowMessageFilter) {
            if (g_shellHookMessage) pChangeWindowMessageFilter(g_shellHookMessage, 1 /*MSGFLT_ADD*/);
            if (g_taskbarCreatedMessage) pChangeWindowMessageFilter(g_taskbarCreatedMessage, 1 /*MSGFLT_ADD*/);
            pChangeWindowMessageFilter(WM_COPYDATA, 1 /*MSGFLT_ADD*/);
            pChangeWindowMessageFilter(0x0049 /*WM_COPYGLOBALDATA*/, 1 /*MSGFLT_ADD*/);
        }
    }
    EnableBlurBehind(hwnd);
    ApplyBackdropMaterial(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    ApplyHideShowHotkey();

    if (g_settings.autoHideIdleSeconds == 0) {
        g_manuallyHidden = false;
        Wh_SetIntValue(L"ManuallyHidden", 0);
    } else {
        g_manuallyHidden = Wh_GetIntValue(L"ManuallyHidden", 0) != 0;
    }
    if (g_manuallyHidden.load()) {
        ShowWindow(hwnd, SW_HIDE);
    }

    Renderer renderer;
    if (!renderer.Initialize(hwnd)) {
        DestroyWindow(hwnd);
        g_hwnd = nullptr;
        if (SUCCEEDED(hrCo)) {
            CoUninitialize();
        }
        return 0;
    }

    using TimeBeginPeriod_t = MMRESULT(WINAPI*)(UINT);
    using TimeEndPeriod_t = MMRESULT(WINAPI*)(UINT);
    static auto pTimeBeginPeriod = reinterpret_cast<TimeBeginPeriod_t>(
        GetProcAddress(LoadLibraryW(L"winmm.dll"), "timeBeginPeriod"));
    static auto pTimeEndPeriod = reinterpret_cast<TimeEndPeriod_t>(
        GetProcAddress(GetModuleHandleW(L"winmm.dll"), "timeEndPeriod"));

    // A 1ms timer resolution costs power, so it is requested only while the island
    // is actually animating. It used to be requested once here and released at
    // shutdown, so a parked or idle island held it for the mod's whole lifetime.
    //
    // Since Windows 10 2004 this affects only the calling process's timers rather
    // than the system clock globally, but the power cost is the reason to scope it
    // either way.
    //
    // The flag is only set when timeBeginPeriod actually succeeded, so the
    // begin/end pairs stay balanced -- these calls are reference counted per
    // process, and an unmatched timeEndPeriod would decrement someone else's
    // request.
    bool highResTimer = false;
    bool animationThreadPowerActive = false;

    // How long the resolution is held after animation stops. Long enough to ride
    // out the gaps between 60Hz content frames and brief pauses between spring
    // animations without flapping, short enough that a settled island gives it back
    // promptly. A plain local rather than a static, so an unload/reload cycle cannot
    // carry a stale timestamp across.
    constexpr double kHighResTimerHoldSec = 0.5;
    double lastAnimatingAt = -1.0;

    auto setHighResTimer = [&](bool want) {
        if (want == highResTimer) {
            return;
        }
        if (want) {
            if (pTimeBeginPeriod && pTimeBeginPeriod(1) == TIMERR_NOERROR) {
                highResTimer = true;
            }
        } else {
            if (pTimeEndPeriod) {
                pTimeEndPeriod(1);
            }
            highResTimer = false;
        }
    };

    // Opt this one thread out of EcoQoS only while an animation needs precise
    // pacing. On battery, Windows can otherwise delay a low-priority injected
    // renderer enough to make short spring transitions visibly uneven. It is
    // opted back into efficiency immediately after animation settles.
    auto setAnimationThreadPowerMode = [&](bool active) {
        if (active == animationThreadPowerActive) return;
        THREAD_POWER_THROTTLING_STATE state = {};
        state.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
        state.StateMask = active ? 0 : THREAD_POWER_THROTTLING_EXECUTION_SPEED;
        SetThreadInformation(GetCurrentThread(), ThreadPowerThrottling,
                             &state, sizeof(state));
        animationThreadPowerActive = active;
    };

    SpringValue widthSpring;
    SpringValue heightSpring;
    SpringValue nudgeSpring;
    widthSpring.Reset((g_autoHideEnabled.load(std::memory_order_relaxed) &&
                       g_settings.autoHideIdleSeconds == -1 ? 0.0f : 120.0f) * g_settings.sizeScale);
    heightSpring.Reset((g_autoHideEnabled.load(std::memory_order_relaxed) &&
                        g_settings.autoHideIdleSeconds == -1 ? 0.0f : 36.0f) * g_settings.sizeScale);
    nudgeSpring.Reset(0.0f);

    IslandKind previousPrimary = IslandKind::Idle;
    auto previousFrame = std::chrono::steady_clock::now();
    auto nextFrameTarget = previousFrame;
    double nextBatteryPoll = 0.0;
    double nextProgressPoll = 0.0;
    double nextSystemPoll = 0.0;
    double nextPrivacyPoll = 0.0;
    bool wasManuallyHidden = false;
    bool wasAutoHiddenParked = false;
    bool parkedForFullscreen = false;

    static double lastInteractionTime = NowSeconds();
    static bool pointerWasOverIsland = false;
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MSG message = {};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_APP_NEW_EVENT) {
                nudgeSpring.value = -6.0f;
                nudgeSpring.velocity = 0.0f;
                nudgeSpring.target = 0.0f;
                lastInteractionTime = NowSeconds();
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        bool justUnhidden = false;

        if (g_manuallyHidden.load()) {
            // Manual hide always takes precedence over — and invalidates —
            // any auto-park bookkeeping, since the window's shown/hidden
            // state is now fully owned by the manual toggle. Without this,
            // toggling manual-hide off after having been auto-parked would
            // leave g_autoHiddenParked stale-true and the window stuck
            // hidden.
            g_autoHiddenParked = false;
            wasAutoHiddenParked = false;

            if (!wasManuallyHidden) {
                // Just hid: drop to zero-CPU parking immediately, no
                // lingering render/animation work this frame.
                ShowWindow(hwnd, SW_HIDE);
                g_audioCaptureNeeded.store(false, std::memory_order_relaxed);
                NotifyMouseThreadParkedChanged();
                wasManuallyHidden = true;
            }
            previousFrame = std::chrono::steady_clock::now();
            // Fully parked: no polling, no timer wakeups — only the stop
            // event or a posted/queued message (hotkey, settings change,
            // clipboard update, etc.) wakes this thread while hidden. Nothing
            // is being paced, so give the system timer resolution back.
            setHighResTimer(false);
            setAnimationThreadPowerMode(false);
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
            nextFrameTarget = std::chrono::steady_clock::now();
            continue;
        }

        // Sits ahead of the parked branch below on purpose: that branch can block
        // for up to 1.5s, and the mouse thread needs to hear about a transition
        // before that, not after.
        {
            static bool prevParkedForHook = false;
            const bool parkedNow = g_autoHiddenParked.load(std::memory_order_relaxed);
            if (parkedNow != prevParkedForHook) {
                prevParkedForHook = parkedNow;
                NotifyMouseThreadParkedChanged();
            }
        }

        if (g_autoHiddenParked.load()) {
            if (!g_autoHideEnabled.load(std::memory_order_relaxed) ||
                (g_settings.autoHideIdleSeconds == 0 && !parkedForFullscreen)) {
                g_autoHiddenParked = false;
            } else {
                previousFrame = std::chrono::steady_clock::now();
                // Auto-parked, so nothing is animating; same reasoning as above.
                setHighResTimer(false);
                setAnimationThreadPowerMode(false);
                if (parkedForFullscreen) {
                    MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, 1500, QS_ALLINPUT);
                    const bool stillFullscreen =
                        g_autoHideEnabled.load(std::memory_order_relaxed) &&
                        g_settings.autoHideFullscreen && IsForegroundFullscreen(hwnd);
                    g_isFullscreen.store(stillFullscreen, std::memory_order_relaxed);
                    if (!stillFullscreen) {
                        g_fullscreenOverrideVisible = false;
                        g_autoHiddenParked = false;
                    } else if (g_fullscreenOverrideVisible.load()) {
                        g_autoHiddenParked = false;
                    }
                } else {
                    MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
                }
                nextFrameTarget = std::chrono::steady_clock::now();
                continue;
            }
        }

        if (wasManuallyHidden) {
            // Just un-hid. Show now; springs get snapped straight to their
            // freshly-computed targets further down this same iteration so
            // there's no stale pop-in animation from wherever they were
            // left off before hiding.
            wasManuallyHidden = false;
            NotifyMouseThreadParkedChanged();
            justUnhidden = true;
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            lastInteractionTime = NowSeconds();
            g_layoutDirty = true;
            previousFrame = std::chrono::steady_clock::now();
            nextFrameTarget = previousFrame;
        }

        if (wasAutoHiddenParked && !g_autoHiddenParked.load()) {
            // Something woke us (mouse near the island, a transient alert,
            // the hotkey, fullscreen ending, etc.) — reveal and let this
            // frame's normal logic decide whether to actually stay visible
            // or immediately re-collapse and re-park.
            wasAutoHiddenParked = false;
            justUnhidden = true;
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            lastInteractionTime = NowSeconds();
            g_layoutDirty = true;
            previousFrame = std::chrono::steady_clock::now();
            nextFrameTarget = previousFrame;
        }

        const double now = NowSeconds();
        if (now >= nextBatteryPoll) {
            UpdateBatterySnapshot();
            nextBatteryPoll = now + 15.0;
        }
        if (now >= nextProgressPoll) {
            UpdateProgressSnapshot();
            nextProgressPoll = now + 0.25;
        }
        if (now >= nextPrivacyPoll) {
            UpdatePrivacyIndicators();
            nextPrivacyPoll = now + 0.5;
        }

        bool timerJustCompleted = false;
        SharedState snapshot;
        {
            std::lock_guard lock(g_stateMutex);
            snapshot = g_state;
            if (g_state.timer.active && g_state.timer.running && now >= g_state.timer.endsAt) {
                g_state.timer.active = false;
                g_state.timer.running = false;
                g_state.timer.justFinished = true;
                g_state.timer.finishedExpiresAt = now + 6.0;
                timerJustCompleted = true;
            }
            if (g_state.timer.justFinished && now >= g_state.timer.finishedExpiresAt) {
                g_state.timer.justFinished = false;
            }
            snapshot.timer = g_state.timer;
            if (g_state.clipboard.active && now >= g_state.clipboard.expiresAt) {
                g_state.clipboard.active = false;
                snapshot.clipboard.active = false;
            }
            if (g_state.notification.active && now >= g_state.notification.expiresAt) {
                g_state.notification.active = false;
                snapshot.notification.active = false;
            }
            if (g_state.volume.active && now >= g_state.volume.expiresAt) {
                g_state.volume.active = false;
                snapshot.volume.active = false;
            }
            if (g_state.capsLock.active && now >= g_state.capsLock.expiresAt) {
                g_state.capsLock.active = false;
                snapshot.capsLock.active = false;
            }
            if (g_state.battery.active && now >= g_state.battery.expiresAt) {
                g_state.battery.active = false;
                snapshot.battery.active = false;
            }
            if (g_state.device.active && now >= g_state.device.expiresAt) {
                g_state.device.active = false;
                snapshot.device.active = false;
            }
        }
            if (timerJustCompleted) {
                TriggerNudge();
        }

        const std::vector<IslandKind> kinds = ChooseActivities(snapshot, g_settings, now);
        Activity primary = ActivityForKind(kinds[0], g_settings, snapshot);
        std::optional<Activity> secondary;
        if (kinds.size() >= 2) {
            secondary = ActivityForKind(kinds[1], g_settings, snapshot);
        }

        // TextScale is a dashboard scale: grow panel geometry and controls
        // along with the text so fixed-height cards do not become crowded.
        const float dashboardScale = Clamp(g_settings.textScale, 0.7f, 1.6f);
        primary.width *= dashboardScale;
        primary.height *= dashboardScale;
        if (secondary) {
            secondary->width *= dashboardScale;
            secondary->height *= dashboardScale;
        }

        const bool pinned = Wh_GetIntValue(L"PinnedExpanded", 0) != 0;

        if (primary.kind != previousPrimary) {
            if (primary.kind != IslandKind::Idle) {
                nudgeSpring.value = -6.0f;
                nudgeSpring.velocity = 0.0f;
                nudgeSpring.target = 0.0f;
            }
            lastInteractionTime = now;
        }
        previousPrimary = primary.kind;

        RECT windowRect = {};
        GetWindowRect(hwnd, &windowRect);
        POINT cursor = {};
        GetCursorPos(&cursor);

        bool hover = false;
        if (widthSpring.value > 1.0f && heightSpring.value > 1.0f) {
            const float topPad = (g_settings.notchStyle || g_settings.borderMergedMode) ? 0.0f : kRenderPadY;
            RECT pillRect = {
                windowRect.left + static_cast<int>(std::round(kRenderPadX)),
                windowRect.top + static_cast<int>(std::round(topPad + nudgeSpring.value)),
                windowRect.left + static_cast<int>(std::round(kRenderPadX + widthSpring.value)),
                windowRect.top + static_cast<int>(std::round(topPad + nudgeSpring.value + heightSpring.value))
            };
            hover = PtInRect(&pillRect, cursor) != FALSE;
        } else if (g_settings.unhideOnHover) {
            RECT dockRect = GetIslandDockRect();
            hover = PtInRect(&dockRect, cursor) != FALSE;
        }

        if (g_hoverWakeSession.load(std::memory_order_relaxed)) {
            if (hover) {
                pointerWasOverIsland = true;
                g_hoverRevealUntil.store(0.0, std::memory_order_relaxed);
            } else if (pointerWasOverIsland) {
                pointerWasOverIsland = false;
                // Keep the wake reveal pill-sized only while the pointer is still
                // at the island's wake position. Once the user leaves, normal
                // hover expansion should work again during the reveal grace period.
                g_hoverWakeInitialHover.store(false, std::memory_order_relaxed);
                g_hoverRevealUntil.store(now + 3.0, std::memory_order_relaxed);
            }
        } else {
            pointerWasOverIsland = false;
        }

        bool needsRender = false;

        if (!hover && g_clickExpanded.load()) {
            g_clickExpanded = false;
            needsRender = true;
        }
        if (!hover && g_hoveredMediaButton.load() != -1) {
            g_hoveredMediaButton = -1;
            needsRender = true;
        }
        // Fixes windhawk-mods#4738: active playback used to count as a continuous
        // event, so the island stayed visible for as long as anything was playing
        // and kept resetting the auto-hide timer. Only the 5s window after a
        // *title* change is transient now, so the pill behaves like the clipboard
        // and battery alerts -- it surfaces, then hides again while playback
        // continues in the background.
        const bool recentTrackChange = g_mediaAutoExpandEnabled.load(std::memory_order_relaxed) &&
                                       !MediaExpandBlocked(snapshot.media) &&
                                       primary.kind == IslandKind::Media &&
                                       snapshot.media.playing &&
                                       !snapshot.media.title.empty() &&
                                       (now - snapshot.media.titleChangedAt < 5.0);

        bool isTransientAlert = (primary.kind == IslandKind::Clipboard ||
                                 primary.kind == IslandKind::Notification ||
                                 primary.kind == IslandKind::Volume ||
                                 primary.kind == IslandKind::BatteryLow ||
                                 primary.kind == IslandKind::CapsLock ||
                                 primary.kind == IslandKind::Device ||
                                 primary.kind == IslandKind::Bluetooth ||
                                 primary.kind == IslandKind::DoNotDisturb ||
                                 recentTrackChange);

        const double hoverRevealDeadline = g_hoverRevealUntil.load(std::memory_order_relaxed);
        const bool hoverRevealExpired = g_hoverWakeSession.load(std::memory_order_relaxed) &&
                                        !hover && hoverRevealDeadline > 0.0 &&
                                        now >= hoverRevealDeadline;
        const bool unhideGraceActive = (now < g_hotkeyUnhideUntil.load()) ||
            (g_hoverWakeSession.load(std::memory_order_relaxed) &&
             (hover || (hoverRevealDeadline > 0.0 && now < hoverRevealDeadline)));
        if (hoverRevealExpired) {
            g_hoverWakeSession.store(false, std::memory_order_relaxed);
            g_hoverWakeInitialHover.store(false, std::memory_order_relaxed);
            NotifyMouseThreadParkedChanged();
        }
        const bool hoverUnhides = g_settings.unhideOnHover && hover;

        bool currentlyHidden = false;
        if (g_autoHideEnabled.load(std::memory_order_relaxed) && !unhideGraceActive) {
            if (g_settings.autoHideIdleSeconds == -1 && !isTransientAlert && !pinned) {
                currentlyHidden = true;
            } else if (g_settings.autoHideIdleSeconds > 0) {
                currentlyHidden = (now - lastInteractionTime > g_settings.autoHideIdleSeconds);
            }
        }
        const bool initialWakeHover = g_hoverWakeInitialHover.load(std::memory_order_relaxed);
        if (initialWakeHover && snapshot.media.available) {
            const int playbackTab = DashboardIndexForSectionId(
                g_settings, CurrentPlaybackSectionId(g_settings));
            if (NormalizedTabIndex(g_settings) != playbackTab) {
                SetDashboardTabIndex(g_settings, playbackTab);
            }
        }
        const bool hoverWakeCompact = initialWakeHover && !snapshot.media.available &&
            !g_clickExpanded.load(std::memory_order_relaxed);
        bool isHoverExpanded = !hoverWakeCompact &&
            (g_settings.expandOnHover ? hover : (hover && g_clickExpanded.load()));
        if (currentlyHidden && !g_settings.unhideOnHover) {
            isHoverExpanded = false;
        } else if (isHoverExpanded || hoverUnhides || pinned || isTransientAlert || unhideGraceActive) {
            lastInteractionTime = now;
        }

        bool isHidden = false;
        if (g_autoHideEnabled.load(std::memory_order_relaxed) && !unhideGraceActive) {
            if (g_settings.autoHideIdleSeconds == -1 && !isTransientAlert && !isHoverExpanded && !hoverUnhides && !pinned) {
                isHidden = true;
            } else if (g_settings.autoHideIdleSeconds > 0) {
                if (hoverUnhides) {
                    isHidden = false;
                } else {
                    isHidden = (now - lastInteractionTime > g_settings.autoHideIdleSeconds);
                }
            }
        }
        if (hoverRevealExpired) {
            isHidden = true;
        }

        static bool isFullscreen = false;
        static double lastFullscreenCheck = -1.0;  // -1.0 guarantees the very first iteration checks
        if (now - lastFullscreenCheck > 0.5) {
            const bool newFullscreen = g_autoHideEnabled.load(std::memory_order_relaxed) &&
                                       g_settings.autoHideFullscreen && IsForegroundFullscreen(hwnd);
            if (isFullscreen && !newFullscreen) {
                // Fullscreen ended — re-arm so the next fullscreen session
                // hides again even if the hotkey was used to reveal the
                // island this time.
                g_fullscreenOverrideVisible = false;
            }
            isFullscreen = newFullscreen;
            g_isFullscreen.store(isFullscreen, std::memory_order_relaxed);
            lastFullscreenCheck = now;
        }

        // Reclaim the top of the z-order if another always-on-top window has
        // been raised over the island. Cheap and rate-limited, and skipped
        // while the island is hidden or suppressed so it cannot un-hide itself.
        static double lastTopmostCheck = 0.0;
        if (!isFullscreen && !g_manuallyHidden.load() && !g_autoHiddenParked.load() &&
            now - lastTopmostCheck > 1.0) {
            EnsureTopmost(hwnd);
            lastTopmostCheck = now;
        }

        const bool micIndicatorActive = snapshot.system.micActive && g_settings.privacyDots && g_settings.privacyDotsMic;
        const bool camIndicatorActive = snapshot.system.cameraActive && g_settings.privacyDots && g_settings.privacyDotsCam;
        const bool privacyActive = micIndicatorActive || camIndicatorActive;

        // The hardware metrics are sampled only while the Hardware Monitor
        // dashboard is expanded and selected.
        //
        // Hoisted out of the poll block below because two decisions need it: whether
        // to sample the expensive GPU and network counters at all, and whether a
        // change in those numbers is worth repainting for.
        const int metricTabIdx = NormalizedTabIndex(g_settings);
        const bool onHardwareMonitorTab = (metricTabIdx == HardwareMonitorTabIndex(g_settings));
        const bool hwMonitorVisible = (primary.kind == IslandKind::Idle || primary.kind == IslandKind::Media) &&
            !isFullscreen && (pinned || isHoverExpanded) && onHardwareMonitorTab;
        const bool systemMetricsVisible = hwMonitorVisible;

        if (now >= nextSystemPoll) {
            const bool needGpuStats = hwMonitorVisible;
            const bool needNetStats = hwMonitorVisible;  // net is only ever drawn in the HW dashboard

            UpdateSystemSnapshot(needGpuStats, needNetStats);
            nextSystemPoll = now + 0.25;
        }

        if (primary.kind == IslandKind::Idle) {
            if (!isFullscreen && !hoverWakeCompact && (pinned || isHoverExpanded)) {
                primary.width = g_settings.expandedWidth * g_settings.sizeScale * dashboardScale;
                primary.height = g_settings.expandedHeight * g_settings.sizeScale * dashboardScale;
            } else if (primary.width > 0.0f) {
                // Collapsed: size the strip to the text it will actually render
                // (windhawk-mods#5086). ActivityForKind's 96/170px was wrong both
                // ways -- dead air around a short "9:41", and clipping on
                // "10:41:32 PM" at larger Text size.
                // The > 0 guard preserves ActivityForKind's fully-hidden case.
                primary.width = std::max(
                    renderer.MeasureIdleStrip(snapshot, g_settings, now).totalWidth,
                    g_settings.islandWidth) * g_settings.sizeScale * dashboardScale;
            }
        }
        if (primary.kind == IslandKind::Media) {
            if (!isFullscreen && !hoverWakeCompact &&
                (isHoverExpanded || pinned || recentTrackChange)) {
                primary.width = g_settings.expandedWidth * g_settings.sizeScale * dashboardScale;
                primary.height = g_settings.expandedHeight * g_settings.sizeScale * dashboardScale;
            }
        }

        const bool fullscreenSuppressed =
            isFullscreen && !g_fullscreenOverrideVisible.load(std::memory_order_relaxed);
        if ((isHidden || fullscreenSuppressed) && !privacyActive && !pinned && !isHoverExpanded &&
            (!isTransientAlert || hoverRevealExpired)) {
            const bool showClosingCircle = hoverRevealExpired && now < hoverRevealDeadline + 0.28;
            primary.width = showClosingCircle ? 36.0f * g_settings.sizeScale * dashboardScale : 0.0f;
            primary.height = showClosingCircle ? 36.0f * g_settings.sizeScale * dashboardScale : 0.0f;
            secondary.reset();
        }

        const bool mediaWaveformVisible =
            g_settings.media && snapshot.media.playing &&
            ((primary.kind == IslandKind::Media && primary.width > 1.0f && primary.height > 1.0f) ||
             (primary.kind == IslandKind::Idle && (pinned || isHoverExpanded) &&
              IsMediaPlaybackTab(g_settings, metricTabIdx)) ||
             (secondary && secondary->kind == IslandKind::Media && secondary->width > 1.0f && secondary->height > 1.0f));
        g_audioCaptureNeeded.store(mediaWaveformVisible, std::memory_order_relaxed);

        float targetWidth = primary.width;
        float targetHeight = primary.height;
        if (secondary) {
            targetWidth = primary.width + secondary->width + 12.0f * g_settings.sizeScale;
            targetHeight = std::max(primary.height, secondary->height);
        }

        widthSpring.target = targetWidth;
        heightSpring.target = targetHeight;

        if (justUnhidden || (unhideGraceActive && widthSpring.value < 0.5f && targetWidth > 1.0f)) {
            // Start from the small parked circle and let the existing springs
            // grow into the collapsed pill. Snapping directly to targetWidth
            // made hover wake look like an expanded-card pop-in.
            const float wakeSize = 36.0f * g_settings.sizeScale;
            widthSpring.Reset(wakeSize);
            heightSpring.Reset(wakeSize);
            nudgeSpring.Reset(0.0f);
        }

        const auto currentFrame = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentFrame - previousFrame).count();
        previousFrame = currentFrame;
        dt = Clamp(dt, 0.001f, 0.050f);

        float styleStiffnessMult = 1.0f;
        float styleDampingMult = 1.0f;
        if (g_settings.animationStyle == AnimationStyle::Smooth) {
            styleStiffnessMult = 1.0f;
            styleDampingMult = 1.35f; // Critically damped, no bounciness
        } else if (g_settings.animationStyle == AnimationStyle::Bouncy) {
            styleStiffnessMult = 1.1f;
            styleDampingMult = 0.70f; // Underdamped, lively elasticity
        } else if (g_settings.animationStyle == AnimationStyle::Snappy) {
            styleStiffnessMult = 1.5f;
            styleDampingMult = 1.25f; // High stiffness and quick settle
        }

        const float speed = g_settings.animationSpeed;
        float widthStiffness = 280.0f * styleStiffnessMult;
        float widthDamping = 24.0f * styleDampingMult;
        if (targetWidth > widthSpring.value) {
            widthStiffness = 380.0f * styleStiffnessMult;
            widthDamping = 26.0f * styleDampingMult;
        } else if (targetWidth < widthSpring.value) {
            widthStiffness = 200.0f * styleStiffnessMult;
            widthDamping = 28.0f * styleDampingMult;
        }

        float heightStiffness = 280.0f * styleStiffnessMult;
        float heightDamping = 24.0f * styleDampingMult;
        if (targetHeight > heightSpring.value) {
            heightStiffness = 380.0f * styleStiffnessMult;
            heightDamping = 26.0f * styleDampingMult;
        } else if (targetHeight < heightSpring.value) {
            heightStiffness = 200.0f * styleStiffnessMult;
            heightDamping = 28.0f * styleDampingMult;
        }

        if (g_settings.reduceMotion) {
            widthSpring.Reset(targetWidth);
            heightSpring.Reset(targetHeight);
            nudgeSpring.Reset(nudgeSpring.target);
        } else {
            widthSpring.Step(dt * speed, widthStiffness, widthDamping);
        }
        if (widthSpring.value < 0.0f) {
            widthSpring.value = 0.0f;
            widthSpring.velocity = 0.0f;
        }

        if (!g_settings.reduceMotion) {
            heightSpring.Step(dt * speed, heightStiffness, heightDamping);
            if (heightSpring.value < 0.0f) {
                heightSpring.value = 0.0f;
                heightSpring.velocity = 0.0f;
            }
            nudgeSpring.Step(dt * speed, 280.0f * styleStiffnessMult, 24.0f * styleDampingMult);
        }

        {
            std::lock_guard lock(g_stateMutex);
            g_state.system.renderFps = ClampInt(static_cast<int>(1.0f / std::max(dt, 0.001f) + 0.5f), 0, 1000);
        }

        const bool draggingOrHover = hover || ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && PtInRect(&windowRect, cursor));

        // Ctrl+hover see-through: holding Ctrl while hovering makes the island
        // transparent and click-through in ANY state, so clicks pass through
        // to windows underneath. Releasing Ctrl or moving away restores it.
        const bool ctrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool ctrlHoverCT = hover && ctrlHeld;

        SetClickThrough(hwnd, (primary.kind == IslandKind::Idle && !draggingOrHover && !pinned) || ctrlHoverCT);

        // Check if animating structurally
        if (std::abs(widthSpring.velocity) > 0.01f || std::abs(widthSpring.target - widthSpring.value) > 0.01f ||
            std::abs(heightSpring.velocity) > 0.01f || std::abs(heightSpring.target - heightSpring.value) > 0.01f ||
            std::abs(nudgeSpring.velocity) > 0.01f || std::abs(nudgeSpring.target - nudgeSpring.value) > 0.01f) {
            needsRender = true;
        }

        // Active Monitor Tracking (Follow Mouse)
        if (g_settings.targetMonitor == -1) {
            static HMONITOR s_lastMonitor = nullptr;
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR currentMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            if (currentMonitor != s_lastMonitor) {
                s_lastMonitor = currentMonitor;
                g_layoutDirty = true;
            }
        }

        // Check if layout was explicitly invalidated
        if (g_layoutDirty.load()) {
            needsRender = true;
        }

        // Hover or pinned state changes visual elements slightly
        static bool prevHover = false;
        static bool prevPinned = false;
        if (hover != prevHover || pinned != prevPinned) {
            needsRender = true;
            prevHover = hover;
            prevPinned = pinned;
        }

        // Activities that animate continuously: the waveform bars, the marquee
        // scroll and the battery pulse. These still only want ~60Hz -- they look no
        // different above it, and they should not piggyback on whatever high Target
        // FPS the user picked for structural resize animation.
        //
        // The rate limit lives in the pacer at the bottom of the loop, not here.
        // This used to gate needsRender behind a 1/60s (16.667ms) timer and then
        // fall through to the flat 16ms idle wait, which cannot satisfy it: 16ms is
        // shorter than the gate, so the next pass failed the check and waited a
        // second time. The result was a paint roughly every 32ms -- about 31fps
        // instead of 60, which is what made playing media look choppy.
        // Media only counts as continuous while something on it is actually moving.
        //
        // ChooseActivities selects the Media pill for any *available* SMTC session,
        // playing or not, so keying off the kind alone meant a paused Spotify or a
        // browser tab with a paused video -- which can sit there for hours, since
        // browsers keep the session alive -- held the island at a 60fps render loop
        // and 1ms timer resolution indefinitely. Nothing on it moves in that state:
        // the collapsed pill draws the album art plus a row of flat bars, taking the
        // !playing branch that skips DrawWaveform entirely.
        //
        // Paused media now falls back to the ordinary change detection above (title
        // and art generation, springs, hover), so the timer is released 0.5s after
        // the last paint.
        //
        // The marquees are expanded-only, hence the hover/pinned terms. No need to
        // include recentTrackChange: it already requires snapshot.media.playing, so
        // it cannot be true while paused.
        // Checks the secondary pill too. The island can show two pills side by side,
        // so a playing media pill sitting next to a running timer or a progress ring
        // is the secondary one -- and looking only at primary left its waveform
        // frozen.
        const bool mediaShown = primary.kind == IslandKind::Media ||
                                (secondary && secondary->kind == IslandKind::Media);
        const bool mediaAnimating = mediaShown &&
            (snapshot.media.playing ||
             (g_settings.musicVisualizer && (isHoverExpanded || pinned)));
        const bool mediaVolumeControlVisible =
            (primary.kind == IslandKind::Idle || primary.kind == IslandKind::Media) &&
            (isHoverExpanded || pinned) && IsMediaPlaybackTab(g_settings, metricTabIdx);
        const bool timeDashboardAnimating =
            (primary.kind == IslandKind::Idle || primary.kind == IslandKind::Media) &&
            (isHoverExpanded || pinned) && !g_settings.reduceMotion &&
            DashboardSectionIdAtIndex(g_settings, metricTabIdx) == 0;

        // Transient alert pills. All expire within a few seconds, so treating them as
        // continuous cannot run away.
        //
        // Device, Bluetooth and Do Not Disturb are here only when the countdown bar
        // is switched on, since that bar is the one thing on them that moves.
        const auto isTransientPill = [](IslandKind kind) {
            return kind == IslandKind::BatteryLow || kind == IslandKind::Clipboard ||
                   kind == IslandKind::Notification;
        };
        const auto hasCountdownBar = [](IslandKind kind) {
            return kind == IslandKind::Device || kind == IslandKind::Bluetooth ||
                   kind == IslandKind::DoNotDisturb;
        };
        const bool countdownBarsOn = g_settings.statusCountdownProgress;

        const bool continuousAnimation =
            mediaAnimating || timeDashboardAnimating || g_contentDragActive ||
            renderer.MediaVolumeTransitioning(mediaVolumeControlVisible) ||
            isTransientPill(primary.kind) ||
            (secondary && isTransientPill(secondary->kind)) ||
            (countdownBarsOn && (hasCountdownBar(primary.kind) ||
                                 (secondary && hasCountdownBar(secondary->kind))));
        if (continuousAnimation) {
            needsRender = true;
        }

        // Privacy dots
        if (snapshot.system.micActive || snapshot.system.cameraActive) {
            needsRender = true;
        }

        // Idle dashboard clock changes once a minute
        static SYSTEMTIME prevTime = {};
        if (primary.kind == IslandKind::Idle && !isHidden) {
            SYSTEMTIME local = {};
            GetLocalTime(&local);
            if (local.wMinute != prevTime.wMinute) {
                needsRender = true;
                prevTime = local;
            }
        }

        // Text that ticks once a second: the focus timer countdown and, when Show
        // seconds is on, the clock.
        //
        // Both used to ride on a side effect. The system poll refreshed CPU load
        // every second and the change detection compared it unconditionally, so the
        // whole island repainted about once a second whether anything visible had
        // changed or not. Gating those metric comparisons on visibility removed that,
        // which left DrawTimer recomputing its m:ss from NowSeconds() on a surface
        // nothing marked dirty -- a 25 minute session sat at 25:00 until some
        // unrelated event forced a paint -- and left the seconds clock updating once
        // a minute despite its own setting promising every second.
        //
        // Keyed to the displayed value rather than to elapsed time, so each visible
        // change paints exactly once. Deliberately not folded into
        // continuousAnimation: these need one frame per second, not the 1ms timer
        // resolution that continuous animation asks for.
        int shownSecond = -1;
        const bool timerShown = primary.kind == IslandKind::Timer ||
                               (secondary && secondary->kind == IslandKind::Timer);
        if (timerShown && snapshot.timer.running) {
            shownSecond = static_cast<int>(std::ceil(snapshot.timer.endsAt - now));
        } else if (primary.kind == IslandKind::Idle && g_settings.showSeconds && !isHidden) {
            SYSTEMTIME st = {};
            GetLocalTime(&st);
            shownSecond = st.wSecond;
        }
        static int s_prevShownSecond = -1;
        if (shownSecond != s_prevShownSecond) {
            s_prevShownSecond = shownSecond;
            needsRender = true;
        }

        // Compare data snapshot to detect changes
        static uint64_t prevArtGen = 0;
        static uint64_t prevSrcIconGen = 0;
        static uint64_t prevNotifIconGen = 0;
        static uint64_t prevClipIconGen = 0;
        static int prevCpu = -1;
        static int prevRam = -1;
        static int prevDisk = -1;
        static int prevVol = -1;
        static bool prevMuted = false;
        static int prevBat = -1;
        static bool prevCharging = false;
        static int prevProg = -1;
        static std::wstring prevMediaTitle;
        static bool prevPlaying = false;

        // CPU / RAM / disk are compared only while a surface that draws them is on
        // screen. UpdateSystemSnapshot refreshes them every second and CPU load
        // essentially always differs between samples, so comparing them
        // unconditionally repainted the whole island once a second for numbers that
        // appear nowhere on the collapsed pill.
        const bool systemMetricsChanged =
            systemMetricsVisible && (snapshot.system.cpuPercent != prevCpu ||
                                     snapshot.system.memoryPercent != prevRam ||
                                     snapshot.system.diskFreePercent != prevDisk);

        if (snapshot.media.artGeneration != prevArtGen ||
            snapshot.media.sourceIconGeneration != prevSrcIconGen ||
            snapshot.media.title != prevMediaTitle ||
            // Play/pause has to be in here now that the metric tick no longer
            // repaints every second as a side effect. Pausing while collapsed swaps
            // the live waveform for the static bars, and without this the frozen
            // last playing frame stayed up until some unrelated repaint came along.
            snapshot.media.playing != prevPlaying ||
            snapshot.notification.icon.generation != prevNotifIconGen ||
            snapshot.clipboard.appIcon.generation != prevClipIconGen ||
            systemMetricsChanged ||
            snapshot.system.volumePercent != prevVol ||
            snapshot.system.volumeMuted != prevMuted ||
            snapshot.battery.percent != prevBat ||
            snapshot.battery.charging != prevCharging ||
            snapshot.progress.percent != prevProg) {
            needsRender = true;
            prevArtGen = snapshot.media.artGeneration;
            prevSrcIconGen = snapshot.media.sourceIconGeneration;
            prevMediaTitle = snapshot.media.title;
            prevPlaying = snapshot.media.playing;
            prevNotifIconGen = snapshot.notification.icon.generation;
            prevClipIconGen = snapshot.clipboard.appIcon.generation;
            prevCpu = snapshot.system.cpuPercent;
            prevRam = snapshot.system.memoryPercent;
            prevDisk = snapshot.system.diskFreePercent;
            prevVol = snapshot.system.volumePercent;
            prevMuted = snapshot.system.volumeMuted;
            prevBat = snapshot.battery.percent;
            prevCharging = snapshot.battery.charging;
            prevProg = snapshot.progress.percent;
        }

        // Track whether Ctrl+hover click-through state changed so we re-render
        static bool prevCtrlHoverCT = false;
        if (ctrlHoverCT != prevCtrlHoverCT) {
            needsRender = true;
            prevCtrlHoverCT = ctrlHoverCT;
        }

        if (needsRender) {
            // When Ctrl+hover click-through is active, reduce pill opacity so the
            // island becomes visually see-through to match the pass-through behavior.
            Settings renderSettings = GetSettingsCopy();
            if (ctrlHoverCT) {
                renderSettings.pillOpacity = Clamp(renderSettings.pillOpacity * 0.35f, 0.15f, 0.45f);
            } else if (renderSettings.themePreset == ThemePreset::Graphite && Wh_GetIntValue(L"PillOpacityOverride", -1) < 0) {
                const bool isExpanded = isHoverExpanded || pinned || isTransientAlert || (widthSpring.value > 260.0f);
                renderSettings.pillOpacity = isExpanded ? 0.98f : 0.88f;
            }
            renderer.Render(snapshot, renderSettings, primary, secondary,
                            widthSpring.value, heightSpring.value, nudgeSpring.value,
                            hover, pinned, now);
        }

        // --- Zero-CPU parking for auto-hidden states (idle timeout / fullscreen) ---
        // Only parks once the collapse animation has actually settled at 0,
        // so the shrink still animates before we cut over to OS-hidden.
        const bool wantsAutoHiddenPark =
            !g_manuallyHidden.load() && !pinned && !isHoverExpanded && !isTransientAlert &&
            !privacyActive && (isHidden || fullscreenSuppressed) &&
            (!isTransientAlert || hoverRevealExpired) &&
            widthSpring.value < 0.5f && heightSpring.value < 0.5f &&
            std::fabs(widthSpring.velocity) < 0.5f && std::fabs(heightSpring.velocity) < 0.5f;

        if (wantsAutoHiddenPark) {
            parkedForFullscreen = fullscreenSuppressed;
            wasAutoHiddenParked = true;
            g_autoHiddenParked = true;
            g_hoverWakeSession = false;
            g_hoverWakeInitialHover = false;
            g_hoverRevealUntil = 0.0;
            ShowWindow(hwnd, SW_HIDE);
            g_audioCaptureNeeded.store(false, std::memory_order_relaxed);
            setHighResTimer(false);
            setAnimationThreadPowerMode(false);
            continue;
        }

        int targetFps = g_settings.targetFps;
        if (targetFps <= 0) {
            targetFps = GetMonitorRefreshRate(hwnd);
        }
        targetFps = ClampInt(targetFps, 30, 1000);
        double targetFrameMs = 1000.0 / static_cast<double>(targetFps);

        // Structural animation -- the springs resizing or nudging the island -- is
        // what benefits from a high Target FPS. Continuous content does not, so once
        // the springs have settled the interval is relaxed to 60Hz and the precise
        // pacer below hits it accurately.
        //
        // std::max, not a plain assignment: a user who deliberately set Target FPS
        // to 40 should keep 40 rather than being pushed up to 60.
        const bool springsAnimating =
            std::fabs(widthSpring.value - widthSpring.target) > 0.5f ||
            std::fabs(heightSpring.value - heightSpring.target) > 0.5f ||
            std::fabs(widthSpring.velocity) > 0.5f ||
            std::fabs(heightSpring.velocity) > 0.5f ||
            std::fabs(nudgeSpring.value) > 0.5f ||
            std::fabs(nudgeSpring.velocity) > 0.5f;

        if (continuousAnimation && !springsAnimating) {
            constexpr double kContinuousFrameMs = 1000.0 / 60.0;
            targetFrameMs = std::max(targetFrameMs, kContinuousFrameMs);
        }

        // Keyed to actual animation, not to repainting.
        //
        // A single repaint does not need 1ms pacing -- it needs one frame, which the
        // pacer delivers fine at the default resolution. Only things that draw a
        // *sequence* of frames care: continuous content and spring motion.
        //
        // Keying it to needsRender was wrong twice over. Per frame it thrashed,
        // because needsRender alternates when 60Hz content runs under a higher
        // Target FPS. And with the hold added, any one-off repaint still took the
        // resolution for the full hold -- including the metric tick, which fired
        // every second, so a plain idle island requested and released it once a
        // second and held it roughly half the time with nothing moving at all.
        //
        // continuousAnimation and springsAnimating are exactly the two cases that
        // want precise frame spacing, so they drive it directly.
        if (continuousAnimation || springsAnimating) {
            lastAnimatingAt = now;
        }
        setHighResTimer(lastAnimatingAt >= 0.0 && now - lastAnimatingAt < kHighResTimerHoldSec);
        setAnimationThreadPowerMode(continuousAnimation || springsAnimating);

        if (!needsRender) {
            // Nothing changed on screen, so poll at ~60Hz rather than the target
            // frame rate. Accuracy does not matter here -- this is a poll interval,
            // not frame pacing -- so it runs at whatever resolution is in effect.
            WaitForSingleObject(g_stopEvent, 16);
            nextFrameTarget = std::chrono::steady_clock::now();
        } else {
            // When animating, achieve ultra-smooth target refresh rate (e.g. 144Hz, 240Hz, 360Hz+).
            nextFrameTarget += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double, std::milli>(targetFrameMs));

            auto nowTime = std::chrono::steady_clock::now();
            if (nowTime < nextFrameTarget) {
                double remainingMs = std::chrono::duration<double, std::milli>(nextFrameTarget - nowTime).count();
                if (remainingMs >= 1.5) {
                    // Sleep for the bulk of the remaining time using OS event wait (zero CPU usage)
                    WaitForSingleObject(g_stopEvent, static_cast<DWORD>(remainingMs - 0.5));
                }
                // Yield for the final fraction of a millisecond to ensure jitter-free presentation on 360Hz displays without CPU waste
                while (std::chrono::steady_clock::now() < nextFrameTarget &&
                       WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
                    std::this_thread::yield();
                }
            } else {
                // If we fell behind, reset target to avoid speed-up catch-up loop
                nextFrameTarget = nowTime;
            }
        }
    }

    // Balances whichever state the loop exited in; a no-op if already released.
    setHighResTimer(false);
    setAnimationThreadPowerMode(false);

    renderer.Shutdown();
    DestroyWindow(hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(kWindowClass, wc.hInstance);

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }

    return 0;
}



DWORD WINAPI DownloadMonitorThreadProc(void*) {
    PWSTR downloadsRaw = nullptr;
    // Keep the known-folder ID local: some Windhawk clang/lld configurations
    // do not link the external FOLDERID_Downloads symbol from uuid.lib.
    static constexpr GUID kDownloadsFolderId =
        {0x374DE290, 0x123F, 0x4565, {0x91, 0x64, 0x39, 0xC4, 0x92, 0x5E, 0x46, 0x7B}};
    if (FAILED(SHGetKnownFolderPath(kDownloadsFolderId, KF_FLAG_DEFAULT, nullptr, &downloadsRaw)) ||
        !downloadsRaw) {
        return 0;
    }
    const std::wstring downloads(downloadsRaw);
    CoTaskMemFree(downloadsRaw);

    HANDLE change = FindFirstChangeNotificationW(
        downloads.c_str(), FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE);
    if (change == INVALID_HANDLE_VALUE) return 0;

    // Seed the set so the first notification does not report files that were
    // already in Downloads before the mod started.
    std::unordered_set<std::wstring> knownFiles;
    auto scanNewFile = [&]() -> std::wstring {
        std::wstring newest;
        WIN32_FIND_DATAW data = {};
        HANDLE find = FindFirstFileW((downloads + L"\\*").c_str(), &data);
        if (find == INVALID_HANDLE_VALUE) return newest;
        do {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            const std::wstring name = data.cFileName;
            if (name == L"." || name == L"..") continue;
            const std::wstring lower = [&]() {
                std::wstring value = name;
                std::transform(value.begin(), value.end(), value.begin(),
                               [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
                return value;
            }();
            auto hasSuffix = [&](const wchar_t* suffix) {
                const size_t suffixLength = wcslen(suffix);
                return lower.size() >= suffixLength &&
                       lower.compare(lower.size() - suffixLength, suffixLength, suffix) == 0;
            };
            if (hasSuffix(L".crdownload") || hasSuffix(L".part") ||
                hasSuffix(L".tmp") || hasSuffix(L".download")) continue;
            if (knownFiles.insert(name).second) newest = name;
        } while (FindNextFileW(find, &data));
        FindClose(find);
        return newest;
    };
    scanNewFile();

    HANDLE waits[] = {g_stopEvent, change};
    while (WaitForMultipleObjects(2, waits, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
        const Settings settings = GetSettingsCopy();
        const std::wstring newFile = scanNewFile();
        if (settings.browserDownloadNotifications && !newFile.empty()) {
            NotificationSnapshot notification;
            notification.active = true;
            notification.app = L"Downloads";
            notification.title = L"Download detected";
            notification.body = newFile;
            notification.expiresAt = NowSeconds() + g_notificationDurationSeconds.load(std::memory_order_relaxed);
            {
                std::lock_guard lock(g_stateMutex);
                g_state.notification = std::move(notification);
            }
            TriggerNudge();
        }
        if (!FindNextChangeNotification(change)) break;
    }
    FindCloseChangeNotification(change);
    return 0;
}

bool StartThreads() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_settingsChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_settingsChangedEvent) {
        return false;
    }

    g_running = true;
    g_renderThread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0, nullptr);
    if (!g_renderThread) {
        return false;
    }

    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0, nullptr);
    g_keyboardThread = CreateThread(nullptr, 0, KeyboardThreadProc, nullptr, 0, &g_keyboardThreadId);
    g_mouseThread = CreateThread(nullptr, 0, MouseThreadProc, nullptr, 0, &g_mouseThreadId);
#if DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER
    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
#endif
    g_bluetoothThread = CreateThread(nullptr, 0, BluetoothThreadProc, nullptr, 0, nullptr);
    g_downloadThread = CreateThread(nullptr, 0, DownloadMonitorThreadProc, nullptr, 0, nullptr);
    SubscribeDndNotification();

    return true;
}

void StopThreads() {
    UnsubscribeDndNotification();
    if (g_keyboardThreadId != 0) {
        PostThreadMessageW(g_keyboardThreadId, WM_QUIT, 0, 0);
    }
    if (g_mouseThreadId != 0) {
        PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
    }
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    HANDLE handles[] = {g_renderThread, g_mediaThread, g_audioThread, g_notificationThread, g_keyboardThread, g_mouseThread, g_bluetoothThread, g_downloadThread};
    for (HANDLE handle : handles) {
        if (handle) {
            WaitForSingleObject(handle, 3000);
            CloseHandle(handle);
        }
    }

    g_renderThread = nullptr;
    g_mediaThread = nullptr;
    g_audioThread = nullptr;
    g_notificationThread = nullptr;
    g_keyboardThread = nullptr;
    g_keyboardThreadId = 0;
    g_mouseThread = nullptr;
    g_mouseThreadId = 0;
    g_bluetoothThread = nullptr;
    g_downloadThread = nullptr;

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_settingsChangedEvent) {
        CloseHandle(g_settingsChangedEvent);
        g_settingsChangedEvent = nullptr;
    }

    g_running = false;
}



}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    if (!StartThreads()) {
        StopThreads();
        return FALSE;
    }

    g_layoutDirty = true;
    Wh_Log(L"Dynamic Island Pro initialized.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    }
    StopThreads();
    Wh_Log(L"Dynamic Island Pro unloaded.");
}

//////////////////////////////////////////////////////////////////////////////////
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