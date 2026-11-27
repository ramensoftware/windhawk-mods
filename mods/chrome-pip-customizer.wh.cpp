// ==WindhawkMod==
// @id              chrome-pip-customizer
// @name            Chrome PiP Customizer
// @description     Customize Chrome's video picture-in-picture window: remove the hover dimming, restyle and move the playback controls, pin the window, click the video to play/pause, double-click for full screen, and hide any button
// @version         1.0.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @donateUrl       https://buymeacoffee.com/widget/page/danrotaru
// @homepage        https://dan13.me/
// @include         chrome.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lcomctl32 -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome PiP Customizer

Customizes the video picture-in-picture (PiP) window of Google Chrome.

![Chrome PiP Customizer](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/chrome-pip-customizer/screenshots/main.webp)

- **Hover shading**: remove the dark overlay that dims the video while the
  pointer is over the window, and the dark gradients at the top and bottom.
- **Playback controls position**: keep the 10-second back, play/pause and
  10-second forward buttons in the center, move them to the bottom left,
  bottom center or bottom right, or put them left or right of the progress
  bar. Everything can also go on one row with the progress bar.
- **Play/pause button style**: transparent with a light circle on hover (like
  the close button), a subtle or dark circle, or Chrome's own style.
- **Full screen button**: left of the volume button, like double-clicking the
  video.
- **Pin button**: keeps the window above other windows, or lets them cover it.
  Every PiP window opens pinned.
- **Mouse**: click the video to play/pause (dragging still moves the window),
  double-click for full screen (a pinned window is unpinned until full screen
  ends), and close the window with a middle click or Ctrl+click.
- **When closing**: one behavior for every way of closing the window: pause
  the video, keep it playing, or go back to its tab.
- **Remove the size limits** of the window.
- **Full screen without border**: optionally remove the window's 1 px border
  and rounded corners while in full screen.
- **Crop button**: in full screen, enlarges the video to fill the screen,
  cutting off its edges, so there are no black bars.
- **Site name**: show or hide the site name and its icon in the top left
  corner, or show the tab title instead.
- **Hide controls**: hide the minimize, back to tab, close, volume, live
  caption, play/pause, seek, previous/next track buttons, the progress bar and
  the time.

## Notes

Changes apply the next time the pointer moves over an open PiP window. After
turning off the pin button or disabling the mod, reopen the PiP window to get
Chrome's minimize button and layout back.

Only the video PiP window is changed (the one opened by Chrome's own
picture-in-picture button). Document PiP windows that websites build
themselves are regular web pages and are not affected.

The mod hooks functions in `chrome.dll` by their debug symbols. The first
start on a new Chrome version downloads Chrome's symbol file, which can take a
while; later starts use Windhawk's cache.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- shading:
  - hideDimOverlay: true
    $name: Hide the dark overlay on hover
    $description: Chrome darkens the whole video while the pointer is over the window. Turn on to keep the video at full brightness.
  - hideTopShade: false
    $name: Hide the top shade
    $description: The dark gradient behind the site name and the window buttons.
  - hideBottomShade: false
    $name: Hide the bottom shade
    $description: The dark gradient behind the progress bar, time and volume button.
  $name: Hover shading
- title:
  - showFavicon: true
    $name: Show the site icon
  - showSiteName: true
    $name: Show the site name
  - showTabTitle: false
    $name: Show the tab title instead of the site name
  $name: Site name (top left)
- controlsPosition: progressLeft
  $name: Playback controls position
  $description: Where the 10-second back, play/pause and 10-second forward buttons are placed.
  $options:
  - center: Center (Chrome default)
  - bottomLeft: Bottom left, next to the time
  - bottomCenter: Bottom center
  - bottomRight: Bottom right, next to the volume button
  - progressLeft: Left of the progress bar
  - progressRight: Right of the progress bar
- singleRow: true
  $name: Put all controls on one row
  $description: Moves the time, volume and live caption buttons onto the progress bar's row, at the bottom of the window. The playback controls go on its left or right as set above.
- bottomPlayButtonSize: "28"
  $name: Play/pause button size
  $description: Used when the playback controls aren't in the center, where Chrome's size (48) is kept.
  $options:
  - "28": Small (28)
  - "32": Medium (32)
  - "36": Large (36)
- playButtonStyle: transparent
  $name: Play/pause button style
  $options:
  - transparent: Transparent
  - subtle: Subtle light circle
  - dark: Dark circle
  - chrome: Chrome default (blue when paused)
- showPinButton: true
  $name: Show the pin button
  $description: A button in the top right that keeps the window above other windows (solid icon) or lets other windows cover it. Every PiP window opens pinned. It takes the minimize button's place.
- unrestrictedSize: true
  $name: Remove the size limits
  $description: Chrome keeps the PiP window between 284x160 and 80% of the screen. Turn on to resize it smaller or larger, up to the whole screen.
- closeAction: backToTab
  $name: When closing
  $description: For every way of closing the window, the close button, Alt+F4, the taskbar, a middle click or Ctrl+click.
  $options:
  - backToTab: Go back to the tab
  - pause: Pause the video (Chrome's close button)
  - keepPlaying: Keep the video playing in its tab
- clickToPlayPause: false
  $name: Click the video to play/pause
  $description: A click that doesn't move the window plays or pauses the video. Dragging still moves the window.
- doubleClickFullscreen: true
  $name: Double-click for full screen
  $description: Double-click the video to make the PiP window fill its screen, showing only the video. Double-click again or press Esc to restore it. A pinned window is unpinned while in full screen.
- fullscreenNoBorder: false
  $name: Hide the border and rounded corners in full screen
  $description: Windows draws a 1 px border and rounds the corners of the PiP window. Turn on to remove both while the window is in full screen, so the video reaches the screen's edges and corners.
- middleClickClose: true
  $name: Middle click or Ctrl+click closes the window
- hide:
  - showFullscreenButton: true
    $name: Show the full screen button
    $description: A button left of the volume button that makes the window fill its screen, like double-clicking the video. Click it again to restore the window.
  - showCropButton: true
    $name: Show the crop button
    $description: In full screen, a button in the top right, left of the pin button, that enlarges the video to fill the whole screen, cutting off its edges, so there are no black bars. Click it again to show the whole video.
  - minimize: true
    $name: Hide the minimize button
    $description: Used when the pin button is off. Minimize closes the PiP window and keeps the video playing in its tab.
  - backToTab: false
    $name: Hide the back to tab button
  - close: false
    $name: Hide the close button
  - mute: false
    $name: Hide the volume (mute) button
  - liveCaption: true
    $name: Hide the live caption button
  - playPause: false
    $name: Hide the play/pause button
  - seekBackward: true
    $name: Hide the 10 seconds back button
  - seekForward: true
    $name: Hide the 10 seconds forward button
  - previousNextTrack: false
    $name: Hide the previous/next track buttons
    $description: Shown by sites with playlists. The progress bar then uses the full width.
  - progressBar: false
    $name: Hide the progress bar
  - time: false
    $name: Hide the time
    $description: The current time and duration, or the LIVE badge.
  $name: Hide controls
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <mutex>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class ControlsPosition {
  Center,
  BottomLeft,
  BottomCenter,
  BottomRight,
  ProgressLeft,
  ProgressRight,
};

enum class PlayButtonStyle { Transparent, Subtle, Dark, Chrome };

enum class CloseAction { BackToTab, Pause, KeepPlaying };

struct Settings {
  bool hideDimOverlay = true;
  bool hideTopShade = false;
  bool hideBottomShade = false;
  ControlsPosition controlsPosition = ControlsPosition::ProgressLeft;
  bool singleRow = true;
  int bottomPlayButtonSize = 28;
  PlayButtonStyle playButtonStyle = PlayButtonStyle::Transparent;
  bool showPinButton = true;
  bool unrestrictedSize = true;
  CloseAction closeAction = CloseAction::BackToTab;
  bool clickToPlayPause = false;
  bool doubleClickFullscreen = true;
  bool fullscreenNoBorder = false;
  bool middleClickClose = true;
  bool showFavicon = true;
  bool showSiteName = true;
  bool showTabTitle = false;
  bool showFullscreenButton = true;
  bool showCropButton = true;
  bool hideMinimize = true;
  bool hideBackToTab = false;
  bool hideClose = false;
  bool hideMute = false;
  bool hideLiveCaption = true;
  bool hidePlayPause = false;
  bool hideSeekBackward = true;
  bool hideSeekForward = true;
  bool hidePreviousNextTrack = false;
  bool hideProgressBar = false;
  bool hideTime = false;
};

static Settings g_settings;
static std::mutex g_settingsMutex;
// Bumped on every settings change; each PiP window re-applies the layout when
// its applied generation is older.
static std::atomic<int> g_settingsGeneration{1};

static bool StringSettingIs(PCWSTR value, PCWSTR expected) {
  return wcscmp(value, expected) == 0;
}

static void LoadSettings() {
  Settings s;
  s.hideDimOverlay = Wh_GetIntSetting(L"shading.hideDimOverlay");
  s.hideTopShade = Wh_GetIntSetting(L"shading.hideTopShade");
  s.hideBottomShade = Wh_GetIntSetting(L"shading.hideBottomShade");

  PCWSTR position = Wh_GetStringSetting(L"controlsPosition");
  if (StringSettingIs(position, L"center")) {
    s.controlsPosition = ControlsPosition::Center;
  } else if (StringSettingIs(position, L"bottomLeft")) {
    s.controlsPosition = ControlsPosition::BottomLeft;
  } else if (StringSettingIs(position, L"bottomCenter")) {
    s.controlsPosition = ControlsPosition::BottomCenter;
  } else if (StringSettingIs(position, L"bottomRight")) {
    s.controlsPosition = ControlsPosition::BottomRight;
  } else if (StringSettingIs(position, L"progressRight")) {
    s.controlsPosition = ControlsPosition::ProgressRight;
  }
  Wh_FreeStringSetting(position);

  s.singleRow = Wh_GetIntSetting(L"singleRow");

  PCWSTR playSize = Wh_GetStringSetting(L"bottomPlayButtonSize");
  int size = _wtoi(playSize);
  Wh_FreeStringSetting(playSize);
  s.bottomPlayButtonSize = (size >= 24 && size <= 40) ? size : 28;

  PCWSTR style = Wh_GetStringSetting(L"playButtonStyle");
  if (StringSettingIs(style, L"subtle")) {
    s.playButtonStyle = PlayButtonStyle::Subtle;
  } else if (StringSettingIs(style, L"dark")) {
    s.playButtonStyle = PlayButtonStyle::Dark;
  } else if (StringSettingIs(style, L"chrome")) {
    s.playButtonStyle = PlayButtonStyle::Chrome;
  }
  Wh_FreeStringSetting(style);

  s.showPinButton = Wh_GetIntSetting(L"showPinButton");
  s.unrestrictedSize = Wh_GetIntSetting(L"unrestrictedSize");

  PCWSTR closeAction = Wh_GetStringSetting(L"closeAction");
  if (StringSettingIs(closeAction, L"pause")) {
    s.closeAction = CloseAction::Pause;
  } else if (StringSettingIs(closeAction, L"keepPlaying")) {
    s.closeAction = CloseAction::KeepPlaying;
  }
  Wh_FreeStringSetting(closeAction);

  s.clickToPlayPause = Wh_GetIntSetting(L"clickToPlayPause");
  s.doubleClickFullscreen = Wh_GetIntSetting(L"doubleClickFullscreen");
  s.fullscreenNoBorder = Wh_GetIntSetting(L"fullscreenNoBorder");
  s.middleClickClose = Wh_GetIntSetting(L"middleClickClose");

  s.showFavicon = Wh_GetIntSetting(L"title.showFavicon");
  s.showSiteName = Wh_GetIntSetting(L"title.showSiteName");
  s.showTabTitle = Wh_GetIntSetting(L"title.showTabTitle");

  s.showFullscreenButton = Wh_GetIntSetting(L"hide.showFullscreenButton");
  s.showCropButton = Wh_GetIntSetting(L"hide.showCropButton");
  s.hideMinimize = Wh_GetIntSetting(L"hide.minimize");
  s.hideBackToTab = Wh_GetIntSetting(L"hide.backToTab");
  s.hideClose = Wh_GetIntSetting(L"hide.close");
  s.hideMute = Wh_GetIntSetting(L"hide.mute");
  s.hideLiveCaption = Wh_GetIntSetting(L"hide.liveCaption");
  s.hidePlayPause = Wh_GetIntSetting(L"hide.playPause");
  s.hideSeekBackward = Wh_GetIntSetting(L"hide.seekBackward");
  s.hideSeekForward = Wh_GetIntSetting(L"hide.seekForward");
  s.hidePreviousNextTrack = Wh_GetIntSetting(L"hide.previousNextTrack");
  s.hideProgressBar = Wh_GetIntSetting(L"hide.progressBar");
  s.hideTime = Wh_GetIntSetting(L"hide.time");

  {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = s;
  }
  g_settingsGeneration.fetch_add(1);
}

static Settings GetSettings() {
  std::lock_guard<std::mutex> lock(g_settingsMutex);
  return g_settings;
}

// -----------------------------------------------------------------------------
// Chrome types and functions
// -----------------------------------------------------------------------------

// gfx::Rect, gfx::Point and gfx::Size have these exact layouts.
struct GfxRect {
  int x, y, width, height;
};
struct GfxPoint {
  int x, y;
};
struct GfxSize {
  int width, height;
};

// std::u16string_view, and views::SizeBounds (two std::optional<int>; all
// zero means unbounded).
struct U16StringView {
  const wchar_t* data;
  size_t size;
};
struct SizeBounds {
  int values[4];
};

// chrome.dll uses the MSVC C++ ABI, checked by disassembling each function
// this mod calls:
// - A function returning a class takes a hidden result pointer, after `this`
//   for member functions. The functions are declared that way so the GNU ABI
//   of this mod's compiler doesn't apply.
// - A std::unique_ptr or std::u16string_view argument is passed as a pointer
//   to the caller's object. View::SetBackground moves from the unique_ptr.
// - ui::ColorVariant (a std::variant<ColorId, SkColor>) is 8 bytes and passed
//   by value in a register.
using ViewSetVisibleFn = void (*)(void* view, bool visible);
using ViewGetVisibleFn = bool (*)(const void* view);
using ViewSetBoundsRectFn = void (*)(void* view, const GfxRect* bounds);
using ViewSetPositionFn = void (*)(void* view, const GfxPoint* position);
using ViewSetSizeFn = void (*)(void* view, const GfxSize* size);
using ViewGetRectFn = GfxRect* (*)(const void* view, GfxRect* result);
using ViewGetPreferredSizeFn = GfxSize* (*)(const void* view, GfxSize* result,
                                            const SizeBounds* bounds);
using ViewGetTooltipTextFn = void* (*)(const void* view);
using ViewSetBackgroundFn = void (*)(void* view, void** background);
using CreatePillBackgroundFn = void** (*)(void** result,
                                          unsigned long long colorVariant,
                                          int borderThickness);
using ImageModelFromVectorIconFn = void* (*)(void* result,
                                             const void* icon,
                                             unsigned long long colorVariant,
                                             int iconSize,
                                             const void* badgeIcon);
using ImageModelDtorFn = void (*)(void* imageModel);
using ImageButtonSetImageModelFn = void (*)(void* button,
                                            int state,
                                            const void* imageModel);
using LabelSetElideBehaviorFn = void (*)(void* label, int behavior);
using LabelSetTextFn = void (*)(void* label, const U16StringView* text);
using LabelGetTextFn = U16StringView* (*)(const void* label,
                                          U16StringView* result);
using HwndForWidgetFn = HWND (*)(const void* widget);
// gfx::Size is passed by value as one 8-byte register: width, then height.
using LayerAsSurfaceFn = void* (*)(void* layer);
// std::vector<ui::Layer*>, returned through a hidden result pointer; its
// buffer is freed with Chrome's operator delete. Chrome's libc++ (ABI v2,
// _LIBCPP_ABI_VECTOR_LAYOUT_SIZE_BASED) stores a vector as data, size and
// capacity rather than three pointers.
struct LayerVector {
  void** data;
  size_t size;
  size_t capacity;
};
using ViewGetLayersInOrderFn = LayerVector* (*)(void* view, LayerVector* result,
                                                int viewLayer);
using OperatorDeleteFn = void (*)(void* memory);
using ViewAddChildViewAtImplFn = void (*)(void* parent, void* child,
                                          size_t index);
using LayerSurfaceSetSurfaceSizeFn = void (*)(void* surface,
                                              unsigned long long size);
using InkDropGetFn = void* (*)(const void* view);
using InkDropHostSetModeFn = void (*)(void* inkDropHost, int mode);

using ControllerTogglePlayPauseFn = bool (*)(void* controller);
using ControllerGetWebContentsFn = void* (*)(void* controller);
using WebContentsGetTitleFn = const void* (*)(void* webContents);
using PlaybackButtonSetPlaybackStateFn = void (*)(void* button, int state);
using PipManagerGetInstanceFn = void* (*)();
using PipManagerExitViaWindowUiFn = bool (*)(void* manager, int behavior);

static ViewSetVisibleFn g_ViewSetVisible;
static ViewGetVisibleFn g_ViewGetVisible;
static ViewSetBoundsRectFn g_ViewSetBoundsRect;
static ViewSetPositionFn g_ViewSetPosition;
static ViewSetSizeFn g_ViewSetSize;
static ViewGetRectFn g_ViewGetMirroredBounds;
static ViewGetRectFn g_ViewGetLocalBounds;
static ViewGetPreferredSizeFn g_ViewGetPreferredSize;
static ViewGetTooltipTextFn g_ViewGetTooltipText;
static ViewSetBackgroundFn g_ViewSetBackground;
static CreatePillBackgroundFn g_CreatePillBackground;
static ImageModelFromVectorIconFn g_ImageModelFromVectorIcon;
static ImageModelDtorFn g_ImageModelDtor;
static ImageButtonSetImageModelFn g_ImageButtonSetImageModel;
static LabelSetElideBehaviorFn g_LabelSetElideBehavior;
static LabelSetTextFn g_LabelSetText;
static LabelGetTextFn g_LabelGetText;
static HwndForWidgetFn g_HwndForWidget;
static LayerAsSurfaceFn g_LayerAsSurface;
static ViewGetLayersInOrderFn g_ViewGetLayersInOrder;
static OperatorDeleteFn g_ChromeOperatorDelete;
static ViewAddChildViewAtImplFn g_ViewAddChildViewAtImpl;
static LayerSurfaceSetSurfaceSizeFn g_LayerSurfaceSetSurfaceSize;
static InkDropGetFn g_InkDropGet;
static InkDropHostSetModeFn g_InkDropHostSetMode;
static ControllerTogglePlayPauseFn g_ControllerTogglePlayPause;
static ControllerGetWebContentsFn g_ControllerGetWebContents;
static WebContentsGetTitleFn g_WebContentsGetTitle;
static PlaybackButtonSetPlaybackStateFn g_PlaybackButtonSetPlaybackState;
static PipManagerGetInstanceFn g_PipManagerGetInstance;
static PipManagerExitViaWindowUiFn g_PipManagerExitViaWindowUi;

// Primary vtables (the ui::LayerDelegate one at offset 0 of every views::View)
// used to recognize the PiP window's child views.
static void* g_vtView;
static void* g_vtImageView;
static void* g_vtLabel;
static void* g_vtMediaProgressView;
static void* g_vtCloseButton;
static void* g_vtMinimizeButton;
static void* g_vtBackToTabButton;
static void* g_vtPlaybackButton;
static void* g_vtSimpleButton;
static void* g_vtToggleMuteButton;
static void* g_vtLiveCaptionButton;
// The views::Widget part of VideoOverlayWindowViews, and its controller.
static void* g_vtWindowWidgetPart;
static void* g_vtController;

// Pin and full screen icons (gfx::VectorIcon).
static void* g_keepIcon;
static void* g_keepFilledIcon;
static void* g_vtToggleCameraButton;
static void* g_vtToggleMicrophoneButton;
static void* g_cropIcon;
// views::ViewLayer::kInclude: the view's own layer is in the list.
static constexpr int kViewLayerInclude = 0;

using OnUpdateControlsBoundsFn = void (*)(void* window);
static OnUpdateControlsBoundsFn g_OnUpdateControlsBoundsOriginal;

using UpdateControlsVisibilityFn = void (*)(void* window,
                                            bool isVisible,
                                            bool shouldAnimate);
static UpdateControlsVisibilityFn g_UpdateControlsVisibilityOriginal;

// base::TimeDelta is an int64 passed by value.
using UpdateTimestampLabelFn = void (*)(void* window,
                                        long long currentTime,
                                        long long duration);
static UpdateTimestampLabelFn g_UpdateTimestampLabelOriginal;

using UpdateImageAndTextFn = void (*)(void* button);
static UpdateImageAndTextFn g_UpdateImageAndTextOriginal;

using SetSourceTitleFn = void (*)(void* window, const void* title);
static SetSourceTitleFn g_SetSourceTitleOriginal;

// ToggleCameraButton / ToggleMicrophoneButton::UpdateImageAndTooltipText.
using UpdateCallButtonImageFn = void (*)(void* button);
static UpdateCallButtonImageFn g_UpdateCameraButtonImageOriginal;
static UpdateCallButtonImageFn g_UpdateMicrophoneButtonImageOriginal;

using UpdateNaturalSizeFn = void (*)(void* window, const GfxSize* naturalSize);
static UpdateNaturalSizeFn g_UpdateNaturalSizeOriginal;

using SetSurfaceIdFn = void (*)(void* window, const void* surfaceId);
static SetSurfaceIdFn g_SetSurfaceIdOriginal;

using UpdateLayerBoundsWithLetterboxingFn =
    void (*)(void* window, unsigned long long windowSize);
static UpdateLayerBoundsWithLetterboxingFn
    g_UpdateLayerBoundsWithLetterboxingOriginal;

using CloseAndPauseIfAvailableFn = void (*)(void* window);
static CloseAndPauseIfAvailableFn g_CloseAndPauseIfAvailableOriginal;

// views::Widget virtuals: `this` is the Widget part of the window.
using GetSizeConstraintFn = GfxSize* (*)(const void* widgetPart, GfxSize* result);
static GetSizeConstraintFn g_GetMinimumSizeOriginal;
static GetSizeConstraintFn g_GetMaximumSizeOriginal;

// content::VideoOverlayWindow::PlaybackState
static constexpr int kPlaybackPlaying = 0;
static constexpr int kPlaybackPaused = 1;

// PictureInPictureWindowManager::UiBehavior
static constexpr int kUiCloseWindowOnly = 0;
static constexpr int kUiCloseWindowAndPauseVideo = 1;
static constexpr int kUiCloseWindowAndFocusOpener = 2;

// gfx::ElideBehavior
static constexpr int kElideHead = 2;
static constexpr int kElideTail = 4;

// views::Button::ButtonState
static constexpr int kButtonStateNormal = 0;

// views::InkDropHost::InkDropMode
static constexpr int kInkDropOff = 0;
static constexpr int kInkDropOn = 1;

// ui::ColorVariant holding an SkColor: the color, then variant index 1.
static unsigned long long SkColorVariant(unsigned argb) {
  return argb | (1ULL << 32);
}

// -----------------------------------------------------------------------------
// Locating the members of VideoOverlayWindowViews
// -----------------------------------------------------------------------------
//
// VideoOverlayWindowViews keeps its child views in consecutive raw_ptr members
// (chrome/browser/ui/views/overlay/video_overlay_window_views.h):
//
//   video_view_, controls_scrim_view_, controls_top_scrim_view_, controls_bottom_scrim_view_,
//   controls_container_view_, playback_controls_container_view_,
//   vc_controls_container_view_, favicon_view_, origin_, close_controls_view_,
//   minimize_button_, back_to_tab_button_, back_to_tab_label_button_,
//   previous_track_controls_view_, play_pause_controls_view_,
//   replay_10_seconds_button_, forward_10_seconds_button_,
//   next_track_controls_view_, skip_ad_controls_view_, resize_handle_view_,
//   toggle_microphone_button_, toggle_camera_button_, hang_up_button_,
//   toggle_mute_button_, progress_view_, timestamp_, live_status_,
//   live_caption_button_, live_caption_dialog_, overlay_view_, title_view_
//
// The offset of that block is found once by scanning the window object for a
// pointer to a CloseImageButton, then checking the classes of its neighbours.
// Slots are numbered relative to close_controls_view_. The controller_ member
// and the views::Widget base are found the same way by their vtables.

enum Slot : int {
  kSlotVideo = -9,
  kSlotScrim = -8,
  kSlotTopScrim = -7,
  kSlotBottomScrim = -6,
  kSlotControlsContainer = -5,
  kSlotPlaybackContainer = -4,
  kSlotVcContainer = -3,
  kSlotFavicon = -2,
  kSlotOrigin = -1,
  kSlotClose = 0,
  kSlotMinimize = 1,
  kSlotBackToTab = 2,
  kSlotPreviousTrack = 4,
  kSlotPlayPause = 5,
  kSlotReplay = 6,
  kSlotForward = 7,
  kSlotNextTrack = 8,
  kSlotToggleMicrophone = 11,
  kSlotToggleCamera = 12,
  kSlotToggleMute = 14,
  kSlotProgress = 15,
  kSlotTimestamp = 16,
  kSlotLiveStatus = 17,
  kSlotLiveCaption = 18,
  kSlotTitleView = 21,
};

static constexpr int kScanBytes = 0x2000;

// Byte offsets in VideoOverlayWindowViews, or -1 until found.
static std::atomic<int> g_closeSlotOffset{-1};
static std::atomic<int> g_controllerOffset{-1};
static std::atomic<int> g_widgetOffset{-1};
static std::atomic<bool> g_locateFailedLogged{false};

static bool SafeReadPtr(const void* address, void** value) {
  SIZE_T read = 0;
  return ReadProcessMemory(GetCurrentProcess(), address, value, sizeof(*value),
                           &read) &&
         read == sizeof(*value);
}

static bool IsPlausiblePointer(void* p) {
  auto v = reinterpret_cast<ULONG_PTR>(p);
  return v >= 0x10000 && v < 0x7FFFFFFF0000ULL && (v & 7) == 0;
}

static void* SafeVtableOf(void* object) {
  void* vtable = nullptr;
  if (!IsPlausiblePointer(object) || !SafeReadPtr(object, &vtable)) {
    return nullptr;
  }
  return vtable;
}

static bool ScanSlotHasClass(BYTE* window, int closeOffset, int slot,
                             void* vtable) {
  if (!vtable) {
    return true;  // Symbol unavailable; nothing to check against.
  }
  int offset = closeOffset + slot * 8;
  if (offset < 0 || offset >= kScanBytes) {
    return false;
  }
  void* view = nullptr;
  return SafeReadPtr(window + offset, &view) && view &&
         SafeVtableOf(view) == vtable;
}

static int ScanForCloseSlot(void* window) {
  auto* base = static_cast<BYTE*>(window);
  for (int offset = 0; offset < kScanBytes; offset += 8) {
    void* candidate = nullptr;
    if (!SafeReadPtr(base + offset, &candidate) ||
        SafeVtableOf(candidate) != g_vtCloseButton) {
      continue;
    }

    bool matches =
        ScanSlotHasClass(base, offset, kSlotMinimize, g_vtMinimizeButton) &&
        ScanSlotHasClass(base, offset, kSlotBackToTab, g_vtBackToTabButton) &&
        ScanSlotHasClass(base, offset, kSlotPlayPause, g_vtPlaybackButton) &&
        ScanSlotHasClass(base, offset, kSlotReplay, g_vtSimpleButton) &&
        ScanSlotHasClass(base, offset, kSlotForward, g_vtSimpleButton) &&
        ScanSlotHasClass(base, offset, kSlotScrim, g_vtView) &&
        ScanSlotHasClass(base, offset, kSlotControlsContainer, g_vtView) &&
        ScanSlotHasClass(base, offset, kSlotFavicon, g_vtImageView) &&
        ScanSlotHasClass(base, offset, kSlotOrigin, g_vtLabel);
    if (matches) {
      return offset;
    }
  }
  return -1;
}

static bool LocateViews(void* window) {
  int offset = g_closeSlotOffset.load();
  if (offset >= 0) {
    void* close = *reinterpret_cast<void**>(static_cast<BYTE*>(window) + offset);
    if (close && *reinterpret_cast<void**>(close) == g_vtCloseButton) {
      return true;
    }
  }

  offset = ScanForCloseSlot(window);
  if (offset < 0) {
    if (!g_locateFailedLogged.exchange(true)) {
      Wh_Log(L"ERROR: PiP window layout not recognized; this Chrome version "
             L"is not supported");
    }
    return false;
  }

  Wh_Log(L"PiP close button member found at offset 0x%X", offset);
  g_closeSlotOffset = offset;
  return true;
}

// Returns the view in `slot`, or nullptr if it's missing or isn't of the
// expected class.
static void* ViewAt(void* window, int slot, void* expectedVtable) {
  int offset = g_closeSlotOffset.load() + slot * 8;
  void* view = *reinterpret_cast<void**>(static_cast<BYTE*>(window) + offset);
  if (!view) {
    return nullptr;
  }
  if (expectedVtable && *reinterpret_cast<void**>(view) != expectedVtable) {
    return nullptr;
  }
  return view;
}

// The controller_ member: a pointer to a VideoPictureInPictureWindowControllerImpl.
static void* ControllerOf(void* window) {
  if (!g_vtController) {
    return nullptr;
  }
  auto* base = static_cast<BYTE*>(window);
  int offset = g_controllerOffset.load();
  if (offset >= 0) {
    void* controller = *reinterpret_cast<void**>(base + offset);
    if (SafeVtableOf(controller) == g_vtController) {
      return controller;
    }
  }
  for (offset = 0; offset < kScanBytes; offset += 8) {
    void* candidate = nullptr;
    if (SafeReadPtr(base + offset, &candidate) &&
        SafeVtableOf(candidate) == g_vtController) {
      Wh_Log(L"PiP controller member found at offset 0x%X", offset);
      g_controllerOffset = offset;
      return candidate;
    }
  }
  return nullptr;
}

// The window's views::Widget base, recognized by its vtable pointer.
static HWND HwndOf(void* window) {
  if (!g_vtWindowWidgetPart || !g_HwndForWidget) {
    return nullptr;
  }
  auto* base = static_cast<BYTE*>(window);
  int offset = g_widgetOffset.load();
  if (offset < 0) {
    for (int o = 0; o < 0x400; o += 8) {
      void* value = nullptr;
      if (SafeReadPtr(base + o, &value) && value == g_vtWindowWidgetPart) {
        offset = o;
        Wh_Log(L"PiP widget base found at offset 0x%X", offset);
        g_widgetOffset = offset;
        break;
      }
    }
    if (offset < 0) {
      return nullptr;
    }
  }
  if (*reinterpret_cast<void**>(base + offset) != g_vtWindowWidgetPart) {
    return nullptr;
  }
  return g_HwndForWidget(base + offset);
}

static const void* TabTitleOf(void* window) {
  if (!g_ControllerGetWebContents || !g_WebContentsGetTitle) {
    return nullptr;
  }
  void* controller = ControllerOf(window);
  if (!controller) {
    return nullptr;
  }
  void* webContents = g_ControllerGetWebContents(controller);
  return webContents ? g_WebContentsGetTitle(webContents) : nullptr;
}

// -----------------------------------------------------------------------------
// Per-window state
// -----------------------------------------------------------------------------

// Views whose visibility Chrome sets only once. When this mod hides one, it
// must show it again itself once the setting is turned off. The other views get
// their visibility from Chrome on every layout.
enum PersistentHide : unsigned {
  kHideFavicon = 1 << 0,
  kHideOrigin = 1 << 1,
  kHideClose = 1 << 2,
  kHideMinimize = 1 << 3,
  kHideBackToTab = 1 << 4,
  kHideMute = 1 << 5,
  kHideLiveCaption = 1 << 6,
  kHideProgress = 1 << 7,
};

struct WindowState {
  void* window = nullptr;
  void* closeButton = nullptr;  // Detects a new window at a reused address.
  HWND hwnd = nullptr;
  int appliedGeneration = 0;
  unsigned hiddenMask = 0;
  bool playButtonResized = false;
  bool titleElideChanged = false;
  int appliedPlayButtonStyle = -1;
  bool fullscreen = false;
  RECT restoreRect{};
  bool pinned = true;  // Chrome's PiP window starts always on top.
  bool pinnedBeforeFullscreen = true;
  int appliedPinIcon = -1;
  bool callButtonsMoved = false;
  bool cropped = false;  // The crop button is on.
  bool borderRemoved = false;
  int lastTimeWidth = -1;
};

// Only touched on Chrome's UI thread.
static std::vector<WindowState> g_windows;

static WindowState* FindWindowState(void* window) {
  for (auto& state : g_windows) {
    if (state.window == window) {
      return &state;
    }
  }
  return nullptr;
}

static WindowState& GetWindowState(void* window, void* closeButton) {
  WindowState* state = FindWindowState(window);
  if (state && state->closeButton != closeButton) {
    *state = WindowState{};
    state->window = window;
  }
  if (!state) {
    if (g_windows.size() >= 32) {
      g_windows.erase(g_windows.begin());
    }
    g_windows.push_back(WindowState{});
    state = &g_windows.back();
    state->window = window;
  }
  state->closeButton = closeButton;
  return *state;
}

// The site name Chrome set for each window, to restore it after the tab title
// was shown. Kept apart from WindowState because Chrome sets it before the
// first layout. Only touched on Chrome's UI thread.
struct TitleState {
  void* window = nullptr;
  std::wstring siteName;
  bool tabTitleShown = false;
};
static std::vector<TitleState> g_titles;

static TitleState& GetTitleState(void* window) {
  for (auto& title : g_titles) {
    if (title.window == window) {
      return title;
    }
  }
  if (g_titles.size() >= 32) {
    g_titles.erase(g_titles.begin());
  }
  g_titles.push_back(TitleState{window});
  return g_titles.back();
}

// Subclassed PiP windows, to unsubclass them when the mod unloads.
static std::mutex g_subclassedMutex;
static std::vector<HWND> g_subclassedWindows;

// -----------------------------------------------------------------------------
// Play/pause button style
// -----------------------------------------------------------------------------

// ARGB. 0 means no background.
static constexpr unsigned kTransparentHoverColor = 0x26FFFFFF;
static constexpr unsigned kSubtleCircleColor = 0x33FFFFFF;
static constexpr unsigned kSubtleHoverColor = 0x4DFFFFFF;
static constexpr unsigned kDarkCircleColor = 0x99000000;
static constexpr unsigned kDarkHoverColor = 0x99303030;

static void SetPillBackground(void* button, unsigned color) {
  if (!g_CreatePillBackground) {
    return;
  }
  void* background = nullptr;
  g_CreatePillBackground(&background, SkColorVariant(color), 0);
  if (background) {
    g_ViewSetBackground(button, &background);  // Takes ownership.
  }
}

// The play/pause button under the pointer, tracked by the PiP window
// procedure. Only touched on Chrome's UI thread.
static void* g_hoveredPlayButton;

// Runs after Chrome set the button's icon and background for the playback
// state, and when the pointer enters or leaves the button.
//
// The mod's styles show hover with the background instead of Chrome's ink
// drop. An ink drop moves the button onto its own layer while shown, which
// snaps it to pixels differently: the button jumped by a pixel and blurred.
static void ApplyPlayButtonStyle(void* button, PlayButtonStyle style) {
  if (!g_ViewSetBackground || style == PlayButtonStyle::Chrome) {
    return;
  }
  const bool hovered = button == g_hoveredPlayButton;
  unsigned color = 0;
  switch (style) {
    case PlayButtonStyle::Transparent:
      color = hovered ? kTransparentHoverColor : 0;
      break;
    case PlayButtonStyle::Subtle:
      color = hovered ? kSubtleHoverColor : kSubtleCircleColor;
      break;
    case PlayButtonStyle::Dark:
      color = hovered ? kDarkHoverColor : kDarkCircleColor;
      break;
    case PlayButtonStyle::Chrome:
      break;
  }
  if (color) {
    SetPillBackground(button, color);
  } else {
    void* noBackground = nullptr;
    g_ViewSetBackground(button, &noBackground);
  }
}

// Chrome's ink drop for Chrome's style, the mod's hover background otherwise.
static void SetPlayButtonInkDrop(void* button, PlayButtonStyle style) {
  if (!g_InkDropGet || !g_InkDropHostSetMode) {
    return;
  }
  if (void* host = g_InkDropGet(button)) {
    g_InkDropHostSetMode(
        host, style == PlayButtonStyle::Chrome ? kInkDropOn : kInkDropOff);
  }
}


static void UpdateImageAndTextHook(void* button) {
  g_UpdateImageAndTextOriginal(button);
  // The linker can merge identical functions, so make sure this is the
  // play/pause button.
  if (*static_cast<void**>(button) == g_vtPlaybackButton) {
    ApplyPlayButtonStyle(button, GetSettings().playButtonStyle);
  }
}

// -----------------------------------------------------------------------------
// Pin button
// -----------------------------------------------------------------------------
//
// The pin button is Chrome's minimize button with a pin icon. Its clicks are
// taken in the window procedure before Chrome sees them, so it never
// minimizes.

// Color of the other PiP button icons (kColorPipWindowForeground, dark mode).
static constexpr unsigned kPipIconColor = 0xFFE3E3E3;
static constexpr int kPipIconSize = 16;

// Replaces the button's tooltip text in place, when it fits in the string's
// buffer: Chrome exports no setter. libc++'s std::u16string here is 24 bytes;
// the last byte's top bit marks the long form ({data, size, capacity}),
// otherwise the characters are inline and the last byte is the size.
static void SetButtonTooltip(void* button, const wchar_t* text) {
  if (!g_ViewGetTooltipText) {
    return;
  }
  auto* string = static_cast<BYTE*>(g_ViewGetTooltipText(button));
  if (!string) {
    return;
  }
  const size_t length = wcslen(text);
  constexpr size_t kShortCapacity = 10;
  if (!(string[23] & 0x80)) {
    if (length > kShortCapacity) {
      return;
    }
    memcpy(string, text, (length + 1) * sizeof(wchar_t));
    string[23] = static_cast<BYTE>(length);
    return;
  }
  // The long form's last word holds the allocated length (characters,
  // including the terminator) below the top bit.
  wchar_t* data = nullptr;
  size_t allocated = 0;
  memcpy(&data, string, sizeof(data));
  memcpy(&allocated, string + 16, sizeof(allocated));
  allocated &= ~(size_t{1} << 63);
  if (!data || allocated > 4096 || length + 1 > allocated) {
    return;
  }
  memcpy(data, text, (length + 1) * sizeof(wchar_t));
  memcpy(string + 8, &length, sizeof(length));
}

static bool CanSetButtonIcons() {
  return g_ImageModelFromVectorIcon && g_ImageModelDtor &&
         g_ImageButtonSetImageModel;
}

static void SetButtonIcon(void* button, const void* icon, int iconSize) {
  // ui::ImageModel is under 64 bytes; leave plenty of room.
  alignas(16) BYTE imageModel[256] = {};
  g_ImageModelFromVectorIcon(imageModel, icon, SkColorVariant(kPipIconColor),
                             iconSize, nullptr);
  g_ImageButtonSetImageModel(button, kButtonStateNormal, imageModel);
  g_ImageModelDtor(imageModel);
}

static void ApplyPinIcon(void* button, WindowState& state) {
  const int pinned = state.pinned ? 1 : 0;
  if (state.appliedPinIcon == pinned || !CanSetButtonIcons() || !g_keepIcon ||
      !g_keepFilledIcon) {
    return;
  }
  SetButtonIcon(button, pinned ? g_keepFilledIcon : g_keepIcon, kPipIconSize);
  SetButtonTooltip(button, pinned ? L"Unpin" : L"Pin");
  state.appliedPinIcon = pinned;
}

// -----------------------------------------------------------------------------
// Full screen and crop buttons
// -----------------------------------------------------------------------------
//
// These are Chrome's camera and microphone buttons, which are only used in
// video calls, with other icons. The mod moves them out of the video call
// controls' container into the controls container, which covers the window,
// so Chrome's hit test finds them where they are. Their clicks are taken in
// the window procedure, so the camera and microphone are never toggled. Chrome
// redraws their icons when they are resized; the mod sets its icon again
// right after.

// Icon sizes: the top buttons have 16 px icons in 24 px buttons. The full
// screen button is as big as the other bottom buttons (28 px) with a 16 px
// icon, smaller than theirs.
static constexpr int kFullscreenIconSize = 16;
static constexpr int kTopIconSize = 16;
static constexpr int kTopButtonExtent = 24;

// Index in the controls container's children: after the playback and video
// call containers, which cover the whole window, so the buttons are above
// them and get the mouse, like Chrome's top buttons.
static constexpr size_t kCallButtonChildIndex = 2;

static bool CanShowFullscreenButton() {
  return CanSetButtonIcons() && g_vtToggleCameraButton &&
         g_ViewAddChildViewAtImpl;
}

static bool CanShowCropButton() {
  return CanSetButtonIcons() && g_cropIcon &&
         g_vtToggleMicrophoneButton && g_ViewAddChildViewAtImpl;
}

// The buttons this mod gave its own icons, with their windows. Only touched on
// Chrome's UI thread.
struct RepurposedButton {
  void* button;
  void* window;
};
static std::vector<RepurposedButton> g_fullscreenButtons;
static std::vector<RepurposedButton> g_cropButtons;

static void Remember(std::vector<RepurposedButton>& buttons, void* button,
                     void* window) {
  for (auto& entry : buttons) {
    if (entry.button == button) {
      entry.window = window;
      return;
    }
  }
  if (buttons.size() >= 32) {
    buttons.erase(buttons.begin());
  }
  buttons.push_back({button, window});
}

static void* WindowOfButton(const std::vector<RepurposedButton>& buttons,
                            void* button) {
  for (auto& entry : buttons) {
    if (entry.button == button) {
      return entry.window;
    }
  }
  return nullptr;
}

static const char* CopyToHeap(const char* text) {
  const size_t size = strlen(text) + 1;
  auto* copy = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, size));
  if (copy) {
    memcpy(copy, text, size);
  }
  return copy;
}

// The full screen icons: two diagonal arrows pointing out to the top left
// and bottom right corners, and pointing in while in full screen. Chrome has
// no such pair, so they are gfx::VectorIcon objects built here in Chrome's
// vector icon format:
//
//   struct PathElement { union { CommandType command; float arg; }; };
//   struct VectorIconRep { base::raw_span<const PathElement> path; };
//   struct VectorIcon {
//     base::raw_span<const VectorIconRep> reps;
//     const char* name;
//   };
//
// A raw_span is a pointer and a size. Chrome caches painted icons by the
// VectorIcon's address and keeps it in the buttons' images, so the objects
// are allocated once and never freed, also after this mod unloads.

// gfx::CommandType values (ui/gfx/vector_icon_types.h).
enum VectorCommand : int {
  kVectorStroke = 5,
  kVectorMoveTo = 7,
  kVectorLineTo = 11,
  kVectorCanvasDimensions = 28,
};

struct VectorIconRepData {
  const void* path;
  size_t pathSize;
};
struct VectorIconData {
  const VectorIconRepData* reps;
  size_t repsSize;
  const char* name;
};

// Path elements are 4 bytes each: a command, or a float argument.
class VectorPath {
 public:
  VectorPath& Command(VectorCommand command) {
    uint32_t value = static_cast<uint32_t>(command);
    words_.push_back(value);
    return *this;
  }
  VectorPath& Arg(float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    words_.push_back(bits);
    return *this;
  }
  VectorPath& Line(float x1, float y1, float x2, float y2, float x3, float y3) {
    // An arrowhead: a corner from (x1, y1) through (x2, y2) to (x3, y3).
    return Command(kVectorMoveTo).Arg(x1).Arg(y1)
        .Command(kVectorLineTo).Arg(x2).Arg(y2)
        .Command(kVectorLineTo).Arg(x3).Arg(y3);
  }
  VectorPath& Segment(float x1, float y1, float x2, float y2) {
    return Command(kVectorMoveTo).Arg(x1).Arg(y1)
        .Command(kVectorLineTo).Arg(x2).Arg(y2);
  }

  // Copies the path into a never-freed VectorIcon.
  const void* Build(const char* name) const {
    HANDLE heap = GetProcessHeap();
    auto* path = static_cast<uint32_t*>(
        HeapAlloc(heap, 0, words_.size() * sizeof(uint32_t)));
    auto* rep = static_cast<VectorIconRepData*>(
        HeapAlloc(heap, 0, sizeof(VectorIconRepData)));
    auto* icon =
        static_cast<VectorIconData*>(HeapAlloc(heap, 0, sizeof(VectorIconData)));
    if (!path || !rep || !icon) {
      return nullptr;
    }
    memcpy(path, words_.data(), words_.size() * sizeof(uint32_t));
    *rep = {path, words_.size()};
    *icon = {rep, 1, name};
    return icon;
  }

 private:
  std::vector<uint32_t> words_;
};

// On a 24 x 24 canvas, stroked 2 units wide, like Chrome's own icons.
static VectorPath FullscreenIconPath(bool collapse) {
  VectorPath path;
  path.Command(kVectorCanvasDimensions).Arg(24)
      .Command(kVectorStroke).Arg(2);
  if (!collapse) {
    // Out: arrowheads at the corners.
    path.Line(4, 10, 4, 4, 10, 4).Segment(4, 4, 10, 10)
        .Line(14, 20, 20, 20, 20, 14).Segment(20, 20, 14, 14);
  } else {
    // In: arrowheads near the center.
    path.Line(10, 4, 10, 10, 4, 10).Segment(10, 10, 4, 4)
        .Line(14, 20, 14, 14, 20, 14).Segment(14, 14, 20, 20);
  }
  return path;
}

static const void* FullscreenIcon(bool collapse) {
  // String literals live in the mod's image, which unloads; the names are
  // copied next to the icons.
  static const void* expand = nullptr;
  static const void* collapseIcon = nullptr;
  const void*& icon = collapse ? collapseIcon : expand;
  if (!icon) {
    icon = FullscreenIconPath(collapse).Build(
        collapse ? CopyToHeap("PipCustomizerExitFullscreenIcon")
                 : CopyToHeap("PipCustomizerFullscreenIcon"));
  }
  return icon;
}

static void ApplyFullscreenIcon(void* button, bool fullscreen) {
  if (const void* icon = FullscreenIcon(fullscreen)) {
    SetButtonIcon(button, icon, kFullscreenIconSize);
  }
  SetButtonTooltip(button, fullscreen ? L"Restore" : L"Full screen");
}

static void ApplyCropIcon(void* button, bool cropped) {
  // Chrome's four rounded corner brackets, the same whether or not the video
  // is cropped.
  if (const void* icon = g_cropIcon) {
    SetButtonIcon(button, icon, kTopIconSize);
  }
  SetButtonTooltip(button, cropped ? L"Fit video" : L"Fill window");
}

// Moves the camera and microphone buttons into the controls container, once
// per window.
static void MoveCallButtons(void* window, WindowState& state) {
  if (state.callButtonsMoved || !g_ViewAddChildViewAtImpl) {
    return;
  }
  void* controls = ViewAt(window, kSlotControlsContainer, g_vtView);
  void* camera = ViewAt(window, kSlotToggleCamera, g_vtToggleCameraButton);
  void* microphone =
      ViewAt(window, kSlotToggleMicrophone, g_vtToggleMicrophoneButton);
  if (!controls || !camera || !microphone) {
    return;
  }
  g_ViewAddChildViewAtImpl(controls, camera, kCallButtonChildIndex);
  g_ViewAddChildViewAtImpl(controls, microphone, kCallButtonChildIndex);
  state.callButtonsMoved = true;
}

// A shown full screen or crop button, or nullptr. The controls container is
// hidden while the controls are faded out.
static void* ShownCallButton(void* window, int slot, void* vtable) {
  if (!LocateViews(window)) {
    return nullptr;
  }
  WindowState* state = FindWindowState(window);
  void* controls = ViewAt(window, kSlotControlsContainer, g_vtView);
  void* button = ViewAt(window, slot, vtable);
  if (!state || !state->callButtonsMoved || !controls || !button ||
      !g_ViewGetVisible(controls) || !g_ViewGetVisible(button)) {
    return nullptr;
  }
  return button;
}

static void* FullscreenButtonOf(void* window) {
  if (!GetSettings().showFullscreenButton || !CanShowFullscreenButton()) {
    return nullptr;
  }
  return ShownCallButton(window, kSlotToggleCamera, g_vtToggleCameraButton);
}

static void* CropButtonOf(void* window) {
  WindowState* state = FindWindowState(window);
  if (!GetSettings().showCropButton || !CanShowCropButton() || !state ||
      !state->fullscreen) {
    return nullptr;
  }
  return ShownCallButton(window, kSlotToggleMicrophone,
                         g_vtToggleMicrophoneButton);
}

// Chrome redraws the camera and microphone icons when the buttons are
// resized.
static void UpdateCameraButtonImageHook(void* button) {
  g_UpdateCameraButtonImageOriginal(button);
  if (void* window = WindowOfButton(g_fullscreenButtons, button)) {
    WindowState* state = FindWindowState(window);
    ApplyFullscreenIcon(button, state && state->fullscreen);
  }
}

static void UpdateMicrophoneButtonImageHook(void* button) {
  g_UpdateMicrophoneButtonImageOriginal(button);
  if (void* window = WindowOfButton(g_cropButtons, button)) {
    WindowState* state = FindWindowState(window);
    ApplyCropIcon(button, state && state->cropped);
  }
}

static void SetWindowPinned(HWND hwnd, WindowState& state, bool pinned) {
  state.pinned = pinned;
  SetWindowPos(hwnd, pinned ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// Whether a client-area mouse message is over a visible view. The controls'
// parents cover the whole window, so their bounds are window coordinates.
static bool IsOverView(HWND hwnd, void* view, LPARAM lParam) {
  if (!view || !g_ViewGetVisible(view)) {
    return false;
  }
  // Mouse messages are in pixels, views in DIPs.
  const double scale = GetDpiForWindow(hwnd) / 96.0;
  const double x = GET_X_LPARAM(lParam) / scale;
  const double y = GET_Y_LPARAM(lParam) / scale;
  GfxRect r{};
  g_ViewGetMirroredBounds(view, &r);
  return x >= r.x && x < r.x + r.width && y >= r.y && y < r.y + r.height;
}

// Whether a client-area mouse message is over the pin button.
static bool IsOverPinButton(HWND hwnd, void* window, LPARAM lParam) {
  if (!GetSettings().showPinButton || !LocateViews(window)) {
    return false;
  }
  return IsOverView(hwnd, ViewAt(window, kSlotMinimize, g_vtMinimizeButton),
                    lParam);
}

// Updates the play/pause button's hover background when the pointer moves
// onto or off it. `lParam` is null when the pointer left the client area.
static void UpdatePlayButtonHover(HWND hwnd, void* window, const LPARAM* lParam) {
  const PlayButtonStyle style = GetSettings().playButtonStyle;
  if (style == PlayButtonStyle::Chrome || !LocateViews(window)) {
    return;
  }
  void* button = ViewAt(window, kSlotPlayPause, g_vtPlaybackButton);
  if (!button) {
    return;
  }
  const bool hovered = lParam && IsOverView(hwnd, button, *lParam);
  const bool wasHovered = g_hoveredPlayButton == button;
  if (hovered == wasHovered) {
    return;
  }
  g_hoveredPlayButton = hovered ? button : nullptr;
  ApplyPlayButtonStyle(button, style);
}

// -----------------------------------------------------------------------------
// Mouse actions
// -----------------------------------------------------------------------------

static void TogglePlayPause(void* window) {
  if (!g_ControllerTogglePlayPause) {
    return;
  }
  void* controller = ControllerOf(window);
  if (!controller) {
    return;
  }
  // Same as Chrome's play/pause button.
  bool active = g_ControllerTogglePlayPause(controller);
  if (g_PlaybackButtonSetPlaybackState && LocateViews(window)) {
    if (void* button = ViewAt(window, kSlotPlayPause, g_vtPlaybackButton)) {
      g_PlaybackButtonSetPlaybackState(
          button, active ? kPlaybackPlaying : kPlaybackPaused);
    }
  }
}

// The window being closed by CloseWindow, so that a WM_CLOSE it causes goes
// to Chrome instead of starting another close.
static void* g_closingWindow;

static void CloseWindowWith(void* window, CloseAction action);

// Closes the window the way the "When closing" setting says.
static void CloseWindow(void* window) {
  void* previous = g_closingWindow;
  g_closingWindow = window;
  CloseWindowWith(window, GetSettings().closeAction);
  g_closingWindow = previous;
}

static void CloseWindowWith(void* window, CloseAction action) {
  if (action == CloseAction::Pause && g_CloseAndPauseIfAvailableOriginal) {
    // Chrome's close button.
    g_CloseAndPauseIfAvailableOriginal(window);
    return;
  }
  if (!g_PipManagerGetInstance || !g_PipManagerExitViaWindowUi) {
    if (g_CloseAndPauseIfAvailableOriginal) {
      g_CloseAndPauseIfAvailableOriginal(window);
    }
    return;
  }
  void* manager = g_PipManagerGetInstance();
  if (!manager) {
    return;
  }
  int behavior = kUiCloseWindowAndPauseVideo;
  if (action == CloseAction::KeepPlaying) {
    behavior = kUiCloseWindowOnly;
  } else if (action == CloseAction::BackToTab) {
    behavior = kUiCloseWindowAndFocusOpener;
  }
  g_PipManagerExitViaWindowUi(manager, behavior);
}

// A left press on the video. Chrome's hit test reports the video as the
// caption, so Windows runs its move loop on the press; the press counts as a
// click if the window didn't move by the time the button is released.
struct PendingClick {
  HWND hwnd = nullptr;
  RECT windowRect{};
  POINT cursor{};
  DWORD time = 0;
};
static PendingClick g_pendingClick;

// With double-click for full screen, a click plays or pauses a moment after
// the press, so a quick double-click doesn't briefly pause the video. This
// timer, set on the PiP window, waits. Most double-clicks press again well
// within this time; waiting the whole double-click time felt sluggish.
static constexpr UINT_PTR kClickToggleTimerId = 0x50495043;  // 'PIPC'
static constexpr DWORD kClickToggleDelayMs = 200;

// The last click that played or paused, so a slower double-click can undo it.
static HWND g_lastToggleHwnd;
static DWORD g_lastToggleTime;

static void ClickTogglePlayPause(HWND hwnd, void* window) {
  TogglePlayPause(window);
  g_lastToggleHwnd = hwnd;
  g_lastToggleTime = GetTickCount();
}

// The window whose pin, full screen or crop button got the last left press.
static HWND g_pinPressHwnd;
static HWND g_fullscreenPressHwnd;
static HWND g_cropPressHwnd;

static void ToggleCrop(void* window);

static void FinishPendingClick(HWND hwnd, void* window) {
  PendingClick click = g_pendingClick;
  g_pendingClick = {};
  if (click.hwnd != hwnd) {
    return;
  }

  RECT rect{};
  POINT cursor{};
  GetWindowRect(hwnd, &rect);
  GetCursorPos(&cursor);
  const bool windowMoved = !EqualRect(&rect, &click.windowRect);
  const bool cursorMoved =
      abs(cursor.x - click.cursor.x) > GetSystemMetrics(SM_CXDRAG) ||
      abs(cursor.y - click.cursor.y) > GetSystemMetrics(SM_CYDRAG);
  const bool tooLong = GetTickCount() - click.time > 1000;
  if (windowMoved || cursorMoved || tooLong) {
    return;
  }
  if (!GetSettings().doubleClickFullscreen) {
    ClickTogglePlayPause(hwnd, window);
    return;
  }
  // Counted from the press, as the double-click time is.
  const DWORD delay = std::min<DWORD>(kClickToggleDelayMs, GetDoubleClickTime());
  const DWORD elapsed = GetTickCount() - click.time;
  if (elapsed >= delay) {
    ClickTogglePlayPause(hwnd, window);
    return;
  }
  SetTimer(hwnd, kClickToggleTimerId, delay - elapsed, nullptr);
}

// Sets whether the window stays on top, and updates the pin button.
static void SetPinnedAndUpdateButton(HWND hwnd, void* window, WindowState& state,
                                     bool pinned) {
  SetWindowPinned(hwnd, state, pinned);
  if (GetSettings().showPinButton && LocateViews(window)) {
    if (void* pin = ViewAt(window, kSlotMinimize, g_vtMinimizeButton)) {
      ApplyPinIcon(pin, state);
    }
  }
}

static void SetCropped(void* window, WindowState& state, bool cropped);

static void UpdateFullscreenButtonIcon(void* window, WindowState& state) {
  if (!CanShowFullscreenButton() || !LocateViews(window)) {
    return;
  }
  void* button = ViewAt(window, kSlotToggleCamera, g_vtToggleCameraButton);
  if (button && WindowOfButton(g_fullscreenButtons, button)) {
    ApplyFullscreenIcon(button, state.fullscreen);
  }
}

// Windows 11 draws a 1 px border around the window and rounds its corners.
// These attributes turn both off for this window, and back to the system's
// default. Older Windows versions ignore them.
static void SetWindowBorderRemoved(HWND hwnd, WindowState& state, bool removed) {
  if (state.borderRemoved == removed) {
    return;
  }
  state.borderRemoved = removed;
  // DWMWA_WINDOW_CORNER_PREFERENCE (33) and DWMWA_BORDER_COLOR (34), named
  // here in case the SDK headers predate them.
  const DWORD corners = removed ? 1 /* DWMWCP_DONOTROUND */
                                : 0 /* DWMWCP_DEFAULT */;
  const COLORREF border = removed ? 0xFFFFFFFE /* DWMWA_COLOR_NONE */
                                  : 0xFFFFFFFF /* DWMWA_COLOR_DEFAULT */;
  DwmSetWindowAttribute(hwnd, 33, &corners, sizeof(corners));
  DwmSetWindowAttribute(hwnd, 34, &border, sizeof(border));
}

// Makes the window cover its monitor, or restores it. The video is
// letterboxed by Chrome to the new size. A pinned window is unpinned while it
// covers the monitor, and pinned again afterwards.
static void ToggleFullscreen(HWND hwnd, void* window) {
  WindowState* state = FindWindowState(window);
  if (!state) {
    return;
  }

  if (state->fullscreen) {
    // The crop button is only in full screen.
    SetCropped(window, *state, false);
    state->fullscreen = false;
    SetWindowBorderRemoved(hwnd, *state, false);
    const RECT& r = state->restoreRect;
    SetWindowPos(hwnd, nullptr, r.left, r.top, r.right - r.left,
                 r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
    if (state->pinnedBeforeFullscreen != state->pinned) {
      SetPinnedAndUpdateButton(hwnd, window, *state,
                               state->pinnedBeforeFullscreen);
    }
    UpdateFullscreenButtonIcon(window, *state);
    return;
  }

  MONITORINFO monitor{sizeof(monitor)};
  if (!GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                      &monitor)) {
    return;
  }
  GetWindowRect(hwnd, &state->restoreRect);
  state->pinnedBeforeFullscreen = state->pinned;
  if (state->pinned) {
    SetPinnedAndUpdateButton(hwnd, window, *state, false);
  }
  // Set first, so the maximum size hook allows the whole monitor.
  state->fullscreen = true;
  if (GetSettings().fullscreenNoBorder) {
    SetWindowBorderRemoved(hwnd, *state, true);
  }
  const RECT& r = monitor.rcMonitor;
  SetWindowPos(hwnd, nullptr, r.left, r.top, r.right - r.left,
               r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
  UpdateFullscreenButtonIcon(window, *state);
}

static bool IsFullscreen(void* window) {
  WindowState* state = FindWindowState(window);
  return state && state->fullscreen;
}

static LRESULT CALLBACK PipWindowSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                              LPARAM lParam, DWORD_PTR refData) {
  void* window = reinterpret_cast<void*>(refData);

  switch (msg) {
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
      const Settings s = GetSettings();
      if (s.middleClickClose && GetKeyState(VK_CONTROL) < 0) {
        CloseWindow(window);
        return 0;
      }
      if (msg != WM_NCLBUTTONDOWN) {
        if (IsOverPinButton(hwnd, window, lParam)) {
          g_pinPressHwnd = hwnd;
          return 0;
        }
        if (IsOverView(hwnd, FullscreenButtonOf(window), lParam)) {
          g_fullscreenPressHwnd = hwnd;
          return 0;
        }
        if (IsOverView(hwnd, CropButtonOf(window), lParam)) {
          g_cropPressHwnd = hwnd;
          return 0;
        }
        break;
      }
      if (wParam != HTCAPTION || !s.clickToPlayPause) {
        break;
      }

      g_pendingClick.hwnd = hwnd;
      GetWindowRect(hwnd, &g_pendingClick.windowRect);
      g_pendingClick.cursor = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      g_pendingClick.time = GetTickCount();

      // Returns after the move loop, when the button is released.
      LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
      if (g_pendingClick.hwnd == hwnd && GetKeyState(VK_LBUTTON) >= 0) {
        FinishPendingClick(hwnd, window);
      }
      return result;
    }

    case WM_LBUTTONUP:
      if (g_fullscreenPressHwnd == hwnd) {
        g_fullscreenPressHwnd = nullptr;
        if (IsOverView(hwnd, FullscreenButtonOf(window), lParam)) {
          ToggleFullscreen(hwnd, window);
        }
        return 0;
      }
      if (g_cropPressHwnd == hwnd) {
        g_cropPressHwnd = nullptr;
        if (IsOverView(hwnd, CropButtonOf(window), lParam)) {
          ToggleCrop(window);
        }
        return 0;
      }
      if (g_pinPressHwnd == hwnd) {
        g_pinPressHwnd = nullptr;
        WindowState* state = FindWindowState(window);
        if (state && IsOverPinButton(hwnd, window, lParam)) {
          SetPinnedAndUpdateButton(hwnd, window, *state, !state->pinned);
        }
        return 0;
      }
      if (g_pendingClick.hwnd == hwnd) {
        FinishPendingClick(hwnd, window);
      }
      break;

    case WM_NCLBUTTONUP:
      if (g_pendingClick.hwnd == hwnd) {
        FinishPendingClick(hwnd, window);
      }
      break;

    case WM_NCLBUTTONDBLCLK: {
      g_pendingClick = {};
      if (wParam != HTCAPTION || !GetSettings().doubleClickFullscreen) {
        break;
      }
      // A quick double-click: the first click hasn't played or paused yet.
      // A slower one: it has; undo it.
      if (!KillTimer(hwnd, kClickToggleTimerId) && g_lastToggleHwnd == hwnd &&
          GetTickCount() - g_lastToggleTime <= GetDoubleClickTime()) {
        TogglePlayPause(window);
      }
      g_lastToggleHwnd = nullptr;
      ToggleFullscreen(hwnd, window);
      return 0;
    }

    case WM_TIMER:
      if (wParam == kClickToggleTimerId) {
        KillTimer(hwnd, kClickToggleTimerId);
        ClickTogglePlayPause(hwnd, window);
        return 0;
      }
      break;

    case WM_MOUSEMOVE:
      UpdatePlayButtonHover(hwnd, window, &lParam);
      break;

    case WM_NCMOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
      UpdatePlayButtonHover(hwnd, window, nullptr);
      break;

    case WM_KEYDOWN:
      if (wParam == VK_ESCAPE && IsFullscreen(window)) {
        ToggleFullscreen(hwnd, window);
        return 0;
      }
      break;

    case WM_MBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
      if (GetSettings().middleClickClose) {
        return 0;
      }
      break;

    case WM_MBUTTONUP:
    case WM_NCMBUTTONUP:
      if (GetSettings().middleClickClose) {
        CloseWindow(window);
        return 0;
      }
      break;

    // The taskbar's and the system menu's Close. Chrome closes the window
    // itself without WM_CLOSE.
    case WM_CLOSE:
      if (g_closingWindow == window) {
        break;
      }
      CloseWindow(window);
      return 0;

    // Keeps the window unpinned when Chrome puts it back on top.
    case WM_WINDOWPOSCHANGING: {
      auto* pos = reinterpret_cast<WINDOWPOS*>(lParam);
      WindowState* state = FindWindowState(window);
      if (state && !state->pinned && !(pos->flags & SWP_NOZORDER) &&
          pos->hwndInsertAfter == HWND_TOPMOST) {
        pos->hwndInsertAfter = HWND_NOTOPMOST;
      }
      break;
    }

    case WM_NCDESTROY: {
      KillTimer(hwnd, kClickToggleTimerId);
      if (g_pendingClick.hwnd == hwnd) {
        g_pendingClick = {};
      }
      if (g_pinPressHwnd == hwnd) {
        g_pinPressHwnd = nullptr;
      }
      if (g_fullscreenPressHwnd == hwnd) {
        g_fullscreenPressHwnd = nullptr;
      }
      if (g_cropPressHwnd == hwnd) {
        g_cropPressHwnd = nullptr;
      }
      g_hoveredPlayButton = nullptr;
      std::lock_guard<std::mutex> lock(g_subclassedMutex);
      std::erase(g_subclassedWindows, hwnd);
      break;
    }
  }

  return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void SubclassPipWindow(void* window, WindowState& state) {
  if (state.hwnd) {
    return;
  }
  HWND hwnd = HwndOf(window);
  if (!hwnd) {
    return;
  }
  state.hwnd = hwnd;
  // Every PiP window opens on top, whatever an earlier window was set to.
  SetWindowPinned(hwnd, state, true);
  state.appliedPinIcon = -1;
  if (WindhawkUtils::SetWindowSubclassFromAnyThread(
          hwnd, PipWindowSubclassProc, reinterpret_cast<DWORD_PTR>(window))) {
    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    g_subclassedWindows.push_back(hwnd);
  }
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

// Chrome's layout constants (video_overlay_window_views.cc and the button
// sources).
static constexpr int kCenterButtonSize = 48;
static constexpr int kActionButtonSize = 28;
static constexpr int kTopButtonSize = 24;
static constexpr int kTopButtonTop = 5;
static constexpr int kTopButtonRightMargin = 4;
static constexpr int kFaviconLeftMargin = 8;
static constexpr int kFaviconSize = 24;
static constexpr int kFaviconRightMargin = 4;
static constexpr int kOriginTop = 5;
static constexpr int kOriginHeight = 24;
static constexpr int kProgressBarHeight = 26;
static constexpr int kTimestampHeight = 16;

// Spacing used by this mod's layouts.
static constexpr int kRowGap = 8;
// The progress bar leaves 8 px empty inside each end, so nothing more is
// added between it and its neighbours.
static constexpr int kProgressBarGap = 0;
// The bottom controls sit higher than Chrome's, with less space at the
// sides.
static constexpr int kBottomMargin = 10;
static constexpr int kSideMargin = 6;
// Distance between the centers of the two rows (Chrome's is 29).
static constexpr int kTwoRowSpacing = 30;
// The empty space inside each end of the progress bar.
static constexpr int kProgressBarInset = 8;
static constexpr int kButtonGap = 4;

static constexpr GfxPoint kOffscreen = {-10000, -10000};

static bool IsVisible(void* view) {
  return view && g_ViewGetVisible(view);
}

static GfxRect BoundsOf(void* view) {
  GfxRect r{};
  g_ViewGetMirroredBounds(view, &r);
  return r;
}

static void SetBounds(void* view, int x, int y, int width, int height) {
  GfxRect r{x, y, width < 0 ? 0 : width, height < 0 ? 0 : height};
  g_ViewSetBoundsRect(view, &r);
}

static void SetPosition(void* view, int x, int y) {
  GfxPoint p{x, y};
  g_ViewSetPosition(view, &p);
}

// The width a label needs for its text.
static int PreferredWidth(void* view) {
  if (!g_ViewGetPreferredSize) {
    return BoundsOf(view).width;
  }
  SizeBounds unbounded{};
  GfxSize size{};
  g_ViewGetPreferredSize(view, &size, &unbounded);
  return size.width;
}

// Hides a view and moves it out of the window, so that Chrome's hit test,
// which uses the bounds of every control, no longer treats its area as a
// button and the window can be dragged there.
static void HideView(void* view) {
  g_ViewSetVisible(view, false);
  g_ViewSetPosition(view, &kOffscreen);
}

// For views in PersistentHide: hides the view, or shows it again if this mod
// hid it before.
static void ApplyPersistentHide(WindowState& state, void* view, unsigned bit,
                                bool hide) {
  if (!view) {
    return;
  }
  if (hide) {
    HideView(view);
    state.hiddenMask |= bit;
  } else if (state.hiddenMask & bit) {
    g_ViewSetVisible(view, true);
    state.hiddenMask &= ~bit;
  }
}

static void SetLabelText(void* label, const std::wstring& text) {
  if (g_LabelSetText) {
    U16StringView view{text.c_str(), text.size()};
    g_LabelSetText(label, &view);
  }
}

static void ApplyTitle(void* window, void* origin, const Settings& s,
                       WindowState& state) {
  if (!origin || !s.showSiteName) {
    return;
  }
  TitleState& title = GetTitleState(window);
  if (s.showTabTitle && g_SetSourceTitleOriginal) {
    // The tab title can change while the window is open.
    if (const void* tabTitle = TabTitleOf(window)) {
      g_SetSourceTitleOriginal(window, tabTitle);
      title.tabTitleShown = true;
    }
  } else if (title.tabTitleShown && !title.siteName.empty()) {
    SetLabelText(origin, title.siteName);
    title.tabTitleShown = false;
  }

  // Chrome elides the site name at the start, like a URL; a title reads
  // better elided at the end.
  if (g_LabelSetElideBehavior && s.showTabTitle != state.titleElideChanged) {
    g_LabelSetElideBehavior(origin, s.showTabTitle ? kElideTail : kElideHead);
    state.titleElideChanged = s.showTabTitle;
  }
}

// The views of the bottom area.
struct BottomViews {
  void* playbackContainer;
  void* playPause;
  void* replay;
  void* forward;
  void* previousTrack;
  void* nextTrack;
  void* progress;
  void* timestamp;
  void* liveStatus;
  void* mute;
  void* liveCaption;
  // The full screen button, when shown.
  void* fullscreenButton;
};

// Shows the full screen button at a position in the window. Chrome hides it
// on every layout.
static void PlaceFullscreenButton(const BottomViews& v, int x, int y) {
  SetBounds(v.fullscreenButton, x, y, kActionButtonSize, kActionButtonSize);
  g_ViewSetVisible(v.fullscreenButton, true);
}

// The playback controls laid out in a row: 10 seconds back, play/pause,
// 10 seconds forward, whichever are shown.
struct ControlsRow {
  struct Item {
    void* view;
    int size;
  } items[3];
  int count = 0;
  int width = 0;

  void Add(void* view, bool hidden, int size) {
    if (!view || hidden || !IsVisible(view)) {
      return;
    }
    if (count) {
      width += kRowGap;
    }
    items[count++] = {view, size};
    width += size;
  }

  void Place(int x, int centerY) const {
    for (int i = 0; i < count; i++) {
      SetPosition(items[i].view, x, centerY - items[i].size / 2);
      x += items[i].size + kRowGap;
    }
  }
};

static bool ControlsOnRight(ControlsPosition position) {
  return position == ControlsPosition::BottomRight ||
         position == ControlsPosition::ProgressRight;
}

// Everything on one line at the bottom:
// [prev] [controls] [time] [progress bar........] [controls] [mute] [CC] [next]
static void LayOutSingleRow(const BottomViews& v, const Settings& s,
                            const ControlsRow& row, WindowState& state, int w,
                            int h) {
  const int centerY = h - kBottomMargin - kActionButtonSize / 2;
  const int buttonY = centerY - kActionButtonSize / 2;
  const bool controlsMoved = s.controlsPosition != ControlsPosition::Center;
  const bool controlsRight = ControlsOnRight(s.controlsPosition);

  int left = kSideMargin;
  bool buttonsOnLeft = false;
  if (IsVisible(v.previousTrack)) {
    SetPosition(v.previousTrack, left, buttonY);
    left += kActionButtonSize + kButtonGap;
    buttonsOnLeft = true;
  }
  if (controlsMoved && !controlsRight && row.count) {
    row.Place(left, centerY);
    left += row.width + kButtonGap;
    buttonsOnLeft = true;
  }

  // The time comes before the progress bar. Text needs more room from the
  // window edge than a button, whose icon is inset.
  if (!s.hideTime) {
    void* time = IsVisible(v.liveStatus) ? v.liveStatus : v.timestamp;
    if (IsVisible(time)) {
      if (!buttonsOnLeft) {
        left = kSideMargin + kProgressBarInset;
      }
      const int width = PreferredWidth(time);
      SetBounds(time, left, centerY - kTimestampHeight / 2, width,
                kTimestampHeight);
      left += width;
      state.lastTimeWidth = width;
    }
  }
  left += kProgressBarGap;

  int right = w - kSideMargin;
  auto placeButton = [&](void* view) {
    right -= kActionButtonSize;
    SetPosition(view, right, buttonY);
    right -= kButtonGap;
  };
  if (IsVisible(v.nextTrack)) {
    placeButton(v.nextTrack);
  }
  if (v.liveCaption && !s.hideLiveCaption) {
    placeButton(v.liveCaption);
  }
  if (v.mute && !s.hideMute) {
    placeButton(v.mute);
  }
  if (v.fullscreenButton) {
    right -= kActionButtonSize;
    PlaceFullscreenButton(v, right, buttonY);
    right -= kButtonGap;
  }
  right -= kRowGap - kButtonGap;
  if (controlsMoved && controlsRight && row.count) {
    right -= row.width;
    row.Place(right, centerY);
    right -= kProgressBarGap;
  }

  if (v.progress && !s.hideProgressBar) {
    SetBounds(v.progress, left, centerY - kProgressBarHeight / 2, right - left,
              kProgressBarHeight);
  }
}

// The progress bar's row above a bottom line with the time and the volume and
// live caption buttons:
// [prev] [controls] [progress bar....................] [controls] [next]
// [controls] [time] ............ [controls] [controls] [mute] [CC]
// Uses the same margins as the one-row layout. The bar's row moves down onto
// the bottom line when nothing else is there.
static void LayOutTwoRows(const BottomViews& v, const Settings& s,
                          const ControlsRow& row, WindowState& state, int w,
                          int h) {
  const bool bottomRowControls =
      s.controlsPosition == ControlsPosition::BottomLeft ||
      s.controlsPosition == ControlsPosition::BottomCenter ||
      s.controlsPosition == ControlsPosition::BottomRight;

  const int bottomCenterY = h - kBottomMargin - kActionButtonSize / 2;
  const int bottomButtonY = bottomCenterY - kActionButtonSize / 2;

  // Bottom right: volume, then live caption at the edge.
  int rightEdge = w - kSideMargin;
  bool rightButtons = false;
  auto placeRightButton = [&](void* view) {
    rightEdge -= kActionButtonSize;
    SetPosition(view, rightEdge, bottomButtonY);
    rightEdge -= kButtonGap;
    rightButtons = true;
  };
  if (v.liveCaption && !s.hideLiveCaption) {
    placeRightButton(v.liveCaption);
  }
  if (v.mute && !s.hideMute) {
    placeRightButton(v.mute);
  }
  if (v.fullscreenButton) {
    rightEdge -= kActionButtonSize;
    PlaceFullscreenButton(v, rightEdge, bottomButtonY);
    rightEdge -= kButtonGap;
    rightButtons = true;
  }
  if (!rightButtons) {
    rightEdge = w - kSideMargin;
  }

  const bool timeShown =
      !s.hideTime && (IsVisible(v.timestamp) || IsVisible(v.liveStatus));
  const bool bottomRowEmpty =
      !timeShown && !rightButtons && !(bottomRowControls && row.count);

  // --- Progress bar row ------------------------------------------------------

  const int progressCenterY =
      bottomRowEmpty ? bottomCenterY : bottomCenterY - kTwoRowSpacing;
  const int progressY = progressCenterY - kProgressBarHeight / 2;
  const int rowButtonY = progressCenterY - kActionButtonSize / 2;

  int barLeft = kSideMargin;
  int barRight = w - kSideMargin;
  if (IsVisible(v.previousTrack)) {
    SetPosition(v.previousTrack, barLeft, rowButtonY);
    barLeft += kActionButtonSize + kButtonGap;
  }
  if (IsVisible(v.nextTrack)) {
    barRight -= kActionButtonSize;
    SetPosition(v.nextTrack, barRight, rowButtonY);
    barRight -= kButtonGap;
  }

  if (s.controlsPosition == ControlsPosition::ProgressLeft && row.count) {
    row.Place(barLeft, progressCenterY);
    barLeft += row.width + kProgressBarGap;
  } else if (s.controlsPosition == ControlsPosition::ProgressRight &&
             row.count) {
    barRight -= row.width;
    row.Place(barRight, progressCenterY);
    barRight -= kProgressBarGap;
  }

  if (v.progress && !s.hideProgressBar) {
    SetBounds(v.progress, barLeft, progressY, barRight - barLeft,
              kProgressBarHeight);
  }

  // --- Bottom line -----------------------------------------------------------

  // The time starts where the progress bar's track does, inside its padding.
  int timeX = kSideMargin + kProgressBarInset;
  int timeRight = rightEdge;

  if (bottomRowControls && row.count) {
    int rowX = kSideMargin;
    if (s.controlsPosition == ControlsPosition::BottomCenter) {
      rowX = (w - row.width) / 2;
    } else if (s.controlsPosition == ControlsPosition::BottomRight) {
      rowX = rightEdge - row.width;
    }
    row.Place(rowX, bottomCenterY);

    // The time goes after the controls when they are on the left, otherwise
    // it is shortened so it doesn't run under them.
    if (s.controlsPosition == ControlsPosition::BottomLeft) {
      timeX = rowX + row.width + kButtonGap;
    } else {
      timeRight = rowX - kButtonGap;
    }
  }

  if (!s.hideTime) {
    const int timeY = bottomCenterY - kTimestampHeight / 2;
    if (v.timestamp) {
      SetBounds(v.timestamp, timeX, timeY, timeRight - timeX, kTimestampHeight);
    }
    if (v.liveStatus) {
      SetPosition(v.liveStatus, timeX, timeY);
    }
  }
  state.lastTimeWidth = -1;
}

static void LayOutBottom(const BottomViews& v, const Settings& s,
                         WindowState& state, int w, int h) {
  const bool movedControls = s.controlsPosition != ControlsPosition::Center;

  ControlsRow row;
  if (movedControls) {
    row.Add(v.replay, s.hideSeekBackward, kActionButtonSize);
    row.Add(v.playPause, s.hidePlayPause, s.bottomPlayButtonSize);
    row.Add(v.forward, s.hideSeekForward, kActionButtonSize);
  }

  if (s.singleRow) {
    LayOutSingleRow(v, s, row, state, w, h);
    return;
  }

  LayOutTwoRows(v, s, row, state, w, h);
}

static void SetCropped(void* window, WindowState& state, bool cropped);

static void ApplyLayout(void* window) {
  if (!LocateViews(window)) {
    return;
  }

  void* close = ViewAt(window, kSlotClose, g_vtCloseButton);
  void* controlsContainer = ViewAt(window, kSlotControlsContainer, g_vtView);
  if (!close || !controlsContainer) {
    return;
  }

  WindowState& state = GetWindowState(window, close);
  state.appliedGeneration = g_settingsGeneration.load();
  const Settings s = GetSettings();

  SubclassPipWindow(window, state);

  // Chrome sizes the controls container to the window first.
  GfxRect local{};
  g_ViewGetLocalBounds(controlsContainer, &local);
  const int w = local.width;
  const int h = local.height;
  if (w <= 0 || h <= 0) {
    return;
  }

  // --- Hover shading ---------------------------------------------------------

  if (s.hideDimOverlay) {
    if (void* v = ViewAt(window, kSlotScrim, g_vtView)) {
      SetBounds(v, 0, 0, 0, 0);
    }
  }
  if (s.hideTopShade) {
    if (void* v = ViewAt(window, kSlotTopScrim, g_vtView)) {
      SetBounds(v, 0, 0, 0, 0);
    }
  }
  if (s.hideBottomShade) {
    if (void* v = ViewAt(window, kSlotBottomScrim, g_vtView)) {
      SetBounds(v, 0, 0, 0, 0);
    }
  }

  // The border setting changed while in full screen.
  if (state.hwnd) {
    SetWindowBorderRemoved(state.hwnd, state,
                           state.fullscreen && s.fullscreenNoBorder);
  }

  // The crop button was turned off while the video was cropped.
  if (state.cropped && (!s.showCropButton || !state.fullscreen)) {
    SetCropped(window, state, false);
  }

  // --- Top right buttons -----------------------------------------------------

  void* minimize = ViewAt(window, kSlotMinimize, g_vtMinimizeButton);
  void* backToTab = ViewAt(window, kSlotBackToTab, g_vtBackToTabButton);

  // The minimize button becomes the pin button.
  const bool hideMinimize = !s.showPinButton && s.hideMinimize;
  if (minimize && s.showPinButton) {
    ApplyPinIcon(minimize, state);
  }
  if (!s.showPinButton && !state.pinned && !state.fullscreen && state.hwnd) {
    SetWindowPinned(state.hwnd, state, true);
  }

  ApplyPersistentHide(state, close, kHideClose, s.hideClose);
  ApplyPersistentHide(state, backToTab, kHideBackToTab, s.hideBackToTab);
  ApplyPersistentHide(state, minimize, kHideMinimize, hideMinimize);

  // The crop button, left of the pin button, only in full screen. Chrome
  // hides it on every layout.
  void* crop = nullptr;
  if (s.showCropButton && state.fullscreen && CanShowCropButton()) {
    MoveCallButtons(window, state);
    void* microphone =
        ViewAt(window, kSlotToggleMicrophone, g_vtToggleMicrophoneButton);
    if (microphone && state.callButtonsMoved) {
      crop = microphone;
      Remember(g_cropButtons, crop, window);
      GfxSize size{kTopButtonExtent, kTopButtonExtent};
      g_ViewSetSize(crop, &size);
      ApplyCropIcon(crop, state.cropped);
      g_ViewSetVisible(crop, true);
    }
  }

  // Right to left, as Chrome places them. Close the gaps left by hidden ones.
  struct TopButton {
    void* view;
    bool hidden;
  } topButtons[] = {
      {close, s.hideClose},
      {backToTab, s.hideBackToTab},
      {minimize, hideMinimize},
      {crop, false},
  };
  const bool anyTopHidden =
      s.hideClose || s.hideBackToTab || hideMinimize || crop;
  int visibleTopButtons = 0;
  for (auto& button : topButtons) {
    if (!button.view || button.hidden) {
      continue;
    }
    if (anyTopHidden) {
      SetPosition(button.view,
                  w - kTopButtonRightMargin - kTopButtonSize * (visibleTopButtons + 1),
                  kTopButtonTop);
    }
    visibleTopButtons++;
  }

  // --- Site name -------------------------------------------------------------

  void* favicon = ViewAt(window, kSlotFavicon, g_vtImageView);
  void* origin = ViewAt(window, kSlotOrigin, g_vtLabel);

  ApplyPersistentHide(state, favicon, kHideFavicon, !s.showFavicon);
  ApplyPersistentHide(state, origin, kHideOrigin, !s.showSiteName);
  ApplyTitle(window, origin, s, state);

  if (origin && s.showSiteName && (!s.showFavicon || anyTopHidden)) {
    const int originX =
        s.showFavicon ? kFaviconLeftMargin + kFaviconSize + kFaviconRightMargin
                      : kFaviconLeftMargin;
    const int originRight =
        visibleTopButtons
            ? w - kTopButtonRightMargin - kTopButtonSize * visibleTopButtons -
                  kFaviconRightMargin
            : w - kFaviconLeftMargin;
    SetBounds(origin, originX, kOriginTop, originRight - originX, kOriginHeight);

    // The title view may be narrower than the window, which would clip the
    // wider site name.
    if (void* title = ViewAt(window, kSlotTitleView, g_vtView)) {
      GfxRect titleBounds = BoundsOf(title);
      if (titleBounds.width < w) {
        SetBounds(title, 0, titleBounds.y, w, titleBounds.height);
      }
    }
  }

  // --- Bottom right buttons --------------------------------------------------

  BottomViews v{};
  v.mute = ViewAt(window, kSlotToggleMute, g_vtToggleMuteButton);
  v.liveCaption = ViewAt(window, kSlotLiveCaption, g_vtLiveCaptionButton);

  ApplyPersistentHide(state, v.mute, kHideMute, s.hideMute);
  ApplyPersistentHide(state, v.liveCaption, kHideLiveCaption, s.hideLiveCaption);

  // --- Playback controls -----------------------------------------------------

  v.playbackContainer = ViewAt(window, kSlotPlaybackContainer, g_vtView);
  v.playPause = ViewAt(window, kSlotPlayPause, g_vtPlaybackButton);
  v.replay = ViewAt(window, kSlotReplay, g_vtSimpleButton);
  v.forward = ViewAt(window, kSlotForward, g_vtSimpleButton);
  v.previousTrack = ViewAt(window, kSlotPreviousTrack, g_vtSimpleButton);
  v.nextTrack = ViewAt(window, kSlotNextTrack, g_vtSimpleButton);
  v.progress = ViewAt(window, kSlotProgress, g_vtMediaProgressView);
  v.timestamp = ViewAt(window, kSlotTimestamp, g_vtLabel);
  v.liveStatus = ViewAt(window, kSlotLiveStatus, g_vtLabel);

  // Re-apply the style after a settings change.
  if (v.playPause && state.appliedPlayButtonStyle != (int)s.playButtonStyle) {
    if (g_UpdateImageAndTextOriginal) {
      g_UpdateImageAndTextOriginal(v.playPause);
    }
    SetPlayButtonInkDrop(v.playPause, s.playButtonStyle);
    ApplyPlayButtonStyle(v.playPause, s.playButtonStyle);
    state.appliedPlayButtonStyle = (int)s.playButtonStyle;
  }

  // Chrome sets the visibility of these on every layout.
  if (s.hidePlayPause && v.playPause) HideView(v.playPause);
  if (s.hideSeekBackward && v.replay) HideView(v.replay);
  if (s.hideSeekForward && v.forward) HideView(v.forward);
  if (s.hidePreviousNextTrack) {
    if (v.previousTrack) HideView(v.previousTrack);
    if (v.nextTrack) HideView(v.nextTrack);
  }
  if (s.hideTime) {
    if (v.timestamp) HideView(v.timestamp);
    if (v.liveStatus) HideView(v.liveStatus);
  }
  ApplyPersistentHide(state, v.progress, kHideProgress, s.hideProgressBar);

  // Size of the play/pause button. Chrome sets it only once.
  const bool movedControls = s.controlsPosition != ControlsPosition::Center;
  if (v.playPause) {
    if (movedControls) {
      GfxSize size{s.bottomPlayButtonSize, s.bottomPlayButtonSize};
      g_ViewSetSize(v.playPause, &size);
      state.playButtonResized = true;
    } else if (state.playButtonResized) {
      GfxSize size{kCenterButtonSize, kCenterButtonSize};
      g_ViewSetSize(v.playPause, &size);
      state.playButtonResized = false;
    }
  }

  // Video conferencing controls replace the playback controls.
  if (!IsVisible(v.playbackContainer)) {
    return;
  }

  if (s.showFullscreenButton && CanShowFullscreenButton()) {
    MoveCallButtons(window, state);
    void* camera = ViewAt(window, kSlotToggleCamera, g_vtToggleCameraButton);
    if (camera && state.callButtonsMoved) {
      v.fullscreenButton = camera;
      Remember(g_fullscreenButtons, camera, window);
      ApplyFullscreenIcon(camera, state.fullscreen);
    }
  }

  LayOutBottom(v, s, state, w, h);
}

// -----------------------------------------------------------------------------
// Video crop
// -----------------------------------------------------------------------------
//
// Chrome letterboxes the video: the video view gets the largest bounds with
// the video's aspect ratio that fit the window, and the video surface is
// stretched to those bounds. To crop, the view gets the smallest such bounds
// that cover the window instead; the window clips the overflow.
//
// Chrome letterboxes the video again when the window is resized, when the
// video's size changes and when it gets a new surface, so the crop is applied
// again after each.

// The view's surface layer (ui::LayerSurface), or nullptr.
static void* SurfaceOf(void* view) {
  LayerVector layers{};
  g_ViewGetLayersInOrder(view, &layers, kViewLayerInclude);
  void* surface = nullptr;
  // A view has its own layer and a few layers above or below it.
  if (layers.data && layers.size <= 16) {
    for (size_t i = 0; i < layers.size; i++) {
      if (layers.data[i] && (surface = g_LayerAsSurface(layers.data[i]))) {
        break;
      }
    }
  }
  if (layers.data) {
    g_ChromeOperatorDelete(layers.data);
  }
  return surface;
}

static unsigned long long PackSize(int width, int height) {
  return static_cast<unsigned>(width) |
         (static_cast<unsigned long long>(static_cast<unsigned>(height)) << 32);
}

static bool CanCropVideo() {
  return g_UpdateLayerBoundsWithLetterboxingOriginal && g_ViewGetLayersInOrder &&
         g_ChromeOperatorDelete && g_LayerAsSurface &&
         g_LayerSurfaceSetSurfaceSize;
}

// The window's size in DIPs, from the controls container that covers it.
static bool WindowSizeOf(void* window, int* width, int* height) {
  void* controls = ViewAt(window, kSlotControlsContainer, g_vtView);
  if (!controls) {
    return false;
  }
  GfxRect local{};
  g_ViewGetLocalBounds(controls, &local);
  *width = local.width;
  *height = local.height;
  return local.width > 0 && local.height > 0;
}

// Scales the video view up to cover the window. Repeating it changes
// nothing, so it can run after anything that may have letterboxed the video.
static void CropVideo(void* window) {
  int windowWidth = 0;
  int windowHeight = 0;
  if (!CanCropVideo() || !LocateViews(window) ||
      !WindowSizeOf(window, &windowWidth, &windowHeight)) {
    return;
  }
  void* video = ViewAt(window, kSlotVideo, g_vtView);
  void* surface = video ? SurfaceOf(video) : nullptr;
  if (!surface) {
    Wh_Log(L"Crop: the video's surface layer wasn't found");
    return;
  }
  const GfxRect current = BoundsOf(video);
  if (current.width <= 0 || current.height <= 0) {
    return;
  }
  const double scale =
      std::max(static_cast<double>(windowWidth) / current.width,
               static_cast<double>(windowHeight) / current.height);
  const int width = static_cast<int>(std::ceil(current.width * scale - 0.001));
  const int height = static_cast<int>(std::ceil(current.height * scale - 0.001));
  SetBounds(video, (windowWidth - width) / 2, (windowHeight - height) / 2,
            width, height);
  // The surface is drawn no larger than this size.
  g_LayerSurfaceSetSurfaceSize(surface, PackSize(width, height));
}

// Crops the video again if the window's crop button is on.
static void ReapplyCrop(void* window) {
  WindowState* state = FindWindowState(window);
  if (state && state->cropped) {
    CropVideo(window);
  }
}

static void SetCropped(void* window, WindowState& state, bool cropped) {
  if (state.cropped == cropped || !CanCropVideo()) {
    return;
  }
  state.cropped = cropped;
  if (cropped) {
    CropVideo(window);
  } else {
    // Chrome letterboxes the video again.
    int width = 0;
    int height = 0;
    if (WindowSizeOf(window, &width, &height)) {
      g_UpdateLayerBoundsWithLetterboxingOriginal(window,
                                                  PackSize(width, height));
    }
  }
  if (void* crop =
          ViewAt(window, kSlotToggleMicrophone, g_vtToggleMicrophoneButton)) {
    if (WindowOfButton(g_cropButtons, crop)) {
      ApplyCropIcon(crop, cropped);
    }
  }
}

static void ToggleCrop(void* window) {
  if (WindowState* state = FindWindowState(window)) {
    SetCropped(window, *state, !state->cropped);
  }
}

// Chrome lays out the video when the window is resized, including when it
// enters or leaves full screen.
static void UpdateLayerBoundsWithLetterboxingHook(void* window,
                                                  unsigned long long windowSize) {
  g_UpdateLayerBoundsWithLetterboxingOriginal(window, windowSize);
  ReapplyCrop(window);
}

// When the video's size changes, Chrome letterboxes it with its own copy of
// the code above.
static void UpdateNaturalSizeHook(void* window, const GfxSize* naturalSize) {
  g_UpdateNaturalSizeOriginal(window, naturalSize);
  ReapplyCrop(window);
}

// A new surface is shown at the window's size, which limits the drawn video
// to it.
static void SetSurfaceIdHook(void* window, const void* surfaceId) {
  g_SetSurfaceIdOriginal(window, surfaceId);
  ReapplyCrop(window);
}

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------

static void OnUpdateControlsBoundsHook(void* window) {
  g_OnUpdateControlsBoundsOriginal(window);
  ApplyLayout(window);
}

// Called when the pointer enters or leaves the window. Re-applies the layout
// after a settings change, as settings change on another thread.
static void UpdateControlsVisibilityHook(void* window,
                                         bool isVisible,
                                         bool shouldAnimate) {
  g_UpdateControlsVisibilityOriginal(window, isVisible, shouldAnimate);

  if (!isVisible) {
    return;
  }
  WindowState* state = FindWindowState(window);
  if (state && state->appliedGeneration != g_settingsGeneration.load()) {
    OnUpdateControlsBoundsHook(window);
  }
}

// With all controls on one row the time is sized to its text, so the row is
// laid out again when the text gets wider or narrower.
static void UpdateTimestampLabelHook(void* window,
                                     long long currentTime,
                                     long long duration) {
  g_UpdateTimestampLabelOriginal(window, currentTime, duration);

  WindowState* state = FindWindowState(window);
  if (!state || !GetSettings().singleRow || !LocateViews(window)) {
    return;
  }
  void* timestamp = ViewAt(window, kSlotTimestamp, g_vtLabel);
  if (IsVisible(timestamp) && PreferredWidth(timestamp) != state->lastTimeWidth) {
    ApplyLayout(window);
  }
}

static void SetSourceTitleHook(void* window, const void* title) {
  g_SetSourceTitleOriginal(window, title);

  // Keep the site name to restore it after showing the tab title.
  if (!g_LabelGetText || !LocateViews(window)) {
    return;
  }
  void* origin = ViewAt(window, kSlotOrigin, g_vtLabel);
  if (!origin) {
    return;
  }
  U16StringView text{};
  g_LabelGetText(origin, &text);
  TitleState& titleState = GetTitleState(window);
  titleState.siteName.assign(text.data, text.size);
  titleState.tabTitleShown = false;

  if (GetSettings().showTabTitle) {
    if (const void* tabTitle = TabTitleOf(window)) {
      g_SetSourceTitleOriginal(window, tabTitle);
      titleState.tabTitleShown = true;
    }
  }
}

// Chrome's close button and Alt+F4.
static void CloseAndPauseIfAvailableHook(void* window) {
  CloseWindow(window);
}

// Chrome keeps the window between a minimum size and 80% of the work area.
// Windows asks for these limits (WM_GETMINMAXINFO) on every resize.
static constexpr GfxSize kUnrestrictedMinimumSize = {80, 45};
static constexpr GfxSize kUnrestrictedMaximumSize = {16384, 16384};

static void* WindowFromWidgetPart(const void* widgetPart) {
  int offset = g_widgetOffset.load();
  if (offset < 0) {
    return nullptr;
  }
  return const_cast<BYTE*>(static_cast<const BYTE*>(widgetPart)) - offset;
}

// The linker can merge identical functions, so a hook may also run for other
// classes; only the PiP window's Widget part has this vtable.
static bool IsPipWidgetPart(const void* widgetPart) {
  return g_vtWindowWidgetPart &&
         *static_cast<void* const*>(widgetPart) == g_vtWindowWidgetPart;
}

static GfxSize* GetMinimumSizeHook(const void* widgetPart, GfxSize* result) {
  g_GetMinimumSizeOriginal(widgetPart, result);
  if (IsPipWidgetPart(widgetPart) && GetSettings().unrestrictedSize) {
    *result = kUnrestrictedMinimumSize;
  }
  return result;
}

static GfxSize* GetMaximumSizeHook(const void* widgetPart, GfxSize* result) {
  g_GetMaximumSizeOriginal(widgetPart, result);
  if (!IsPipWidgetPart(widgetPart)) {
    return result;
  }
  void* window = WindowFromWidgetPart(widgetPart);
  if (GetSettings().unrestrictedSize || (window && IsFullscreen(window))) {
    *result = kUnrestrictedMaximumSize;
  }
  return result;
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

static constexpr PCWSTR kChromeSymbolServer =
    L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";

static std::atomic<bool> g_hooksInstalled{false};

static bool HookChrome(HMODULE chromeDll) {
  if (g_hooksInstalled.exchange(true)) {
    return true;
  }

  // Functions with parameters are matched by their decorated names. Methods
  // without a public symbol have only a private function record named without
  // a signature, and are matched by that name. Only the layout is required;
  // every other feature is disabled if its functions are missing.
  WindhawkUtils::SYMBOL_HOOK chromeDllHooks[] = {
      {{LR"(VideoOverlayWindowViews::OnUpdateControlsBounds)"},
       &g_OnUpdateControlsBoundsOriginal,
       OnUpdateControlsBoundsHook},
      {{LR"(VideoOverlayWindowViews::UpdateControlsVisibility)"},
       &g_UpdateControlsVisibilityOriginal,
       UpdateControlsVisibilityHook,
       true},
      {{LR"(VideoOverlayWindowViews::UpdateTimestampLabel)"},
       &g_UpdateTimestampLabelOriginal,
       UpdateTimestampLabelHook,
       true},
      {{LR"(ToggleCameraButton::UpdateImageAndTooltipText)"},
       &g_UpdateCameraButtonImageOriginal,
       UpdateCameraButtonImageHook,
       true},
      {{LR"(ToggleMicrophoneButton::UpdateImageAndTooltipText)"},
       &g_UpdateMicrophoneButtonImageOriginal,
       UpdateMicrophoneButtonImageHook,
       true},
      {{LR"(?UpdateNaturalSize@VideoOverlayWindowViews@@UEAAXAEBVSize@gfx@@@Z)"},
       &g_UpdateNaturalSizeOriginal,
       UpdateNaturalSizeHook,
       true},
      {{LR"(?SetSurfaceId@VideoOverlayWindowViews@@UEAAXAEBVSurfaceId@viz@@@Z)"},
       &g_SetSurfaceIdOriginal,
       SetSurfaceIdHook,
       true},
      {{LR"(VideoOverlayWindowViews::UpdateLayerBoundsWithLetterboxing)"},
       &g_UpdateLayerBoundsWithLetterboxingOriginal,
       UpdateLayerBoundsWithLetterboxingHook,
       true},
      {{LR"(VideoOverlayWindowViews::CloseAndPauseIfAvailable)"},
       &g_CloseAndPauseIfAvailableOriginal,
       CloseAndPauseIfAvailableHook,
       true},
      {{LR"(PlaybackImageButton::UpdateImageAndText)"},
       &g_UpdateImageAndTextOriginal,
       UpdateImageAndTextHook,
       true},
      {{LR"(?SetSourceTitle@VideoOverlayWindowViews@@UEAAXAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@@Z)"},
       &g_SetSourceTitleOriginal,
       SetSourceTitleHook,
       true},
      {{LR"(?GetMinimumSize@VideoOverlayWindowViews@@UEBA?AVSize@gfx@@XZ)"},
       &g_GetMinimumSizeOriginal,
       GetMinimumSizeHook,
       true},
      {{LR"(?GetMaximumSize@VideoOverlayWindowViews@@UEBA?AVSize@gfx@@XZ)"},
       &g_GetMaximumSizeOriginal,
       GetMaximumSizeHook,
       true},

      {{LR"(?SetVisible@View@views@@UEAAX_N@Z)"}, &g_ViewSetVisible},
      {{LR"(?GetVisible@View@views@@QEBA_NXZ)"}, &g_ViewGetVisible},
      {{LR"(?SetBoundsRect@View@views@@QEAAXAEBVRect@gfx@@@Z)"},
       &g_ViewSetBoundsRect},
      {{LR"(?SetPosition@View@views@@QEAAXAEBVPoint@gfx@@@Z)"},
       &g_ViewSetPosition},
      {{LR"(?SetSize@View@views@@QEAAXAEBVSize@gfx@@@Z)"}, &g_ViewSetSize},
      {{LR"(?GetMirroredBounds@View@views@@QEBA?AVRect@gfx@@XZ)"},
       &g_ViewGetMirroredBounds},
      {{LR"(?GetLocalBounds@View@views@@QEBA?AVRect@gfx@@XZ)"},
       &g_ViewGetLocalBounds},
      {{LR"(?GetPreferredSize@View@views@@QEBA?AVSize@gfx@@AEBVSizeBounds@2@@Z)"},
       &g_ViewGetPreferredSize, nullptr, true},
      {{LR"(?GetTooltipText@View@views@@QEBAAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@XZ)"},
       &g_ViewGetTooltipText, nullptr, true},
      {{LR"(?SetBackground@View@views@@QEAAXV?$unique_ptr@VBackground@views@@U?$default_delete@VBackground@views@@@__Cr@std@@@__Cr@std@@@Z)"},
       &g_ViewSetBackground, nullptr, true},
      {{LR"(?CreatePillBackground@views@@YA?AV?$unique_ptr@VBackground@views@@U?$default_delete@VBackground@views@@@__Cr@std@@@__Cr@std@@VColorVariant@ui@@H@Z)"},
       &g_CreatePillBackground, nullptr, true},
      {{LR"(?FromVectorIcon@ImageModel@ui@@SA?AV12@AEBUVectorIcon@gfx@@VColorVariant@2@HPEBU34@@Z)"},
       &g_ImageModelFromVectorIcon, nullptr, true},
      {{LR"(??1ImageModel@ui@@QEAA@XZ)"}, &g_ImageModelDtor, nullptr, true},
      {{LR"(?SetImageModel@ImageButton@views@@UEAAXW4ButtonState@Button@2@AEBVImageModel@ui@@@Z)"},
       &g_ImageButtonSetImageModel, nullptr, true},
      {{LR"(?SetElideBehavior@Label@views@@QEAAXW4ElideBehavior@gfx@@@Z)"},
       &g_LabelSetElideBehavior, nullptr, true},
      {{LR"(?SetText@Label@views@@UEAAXV?$basic_string_view@_SU?$char_traits@_S@__Cr@std@@@__Cr@std@@@Z)"},
       &g_LabelSetText, nullptr, true},
      {{LR"(?GetText@Label@views@@QEBA?AV?$basic_string_view@_SU?$char_traits@_S@__Cr@std@@@__Cr@std@@XZ)"},
       &g_LabelGetText, nullptr, true},
      {{LR"(?Get@InkDrop@views@@SAPEBVInkDropHost@2@PEBVView@2@@Z)"},
       &g_InkDropGet, nullptr, true},
      {{LR"(?SetMode@InkDropHost@views@@QEAAXW4InkDropMode@12@@Z)"},
       &g_InkDropHostSetMode, nullptr, true},
      {{LR"(?AsSurface@Layer@ui@@QEAAPEAVLayerSurface@2@XZ)"},
       &g_LayerAsSurface, nullptr, true},
      {{LR"(?SetSurfaceSize@LayerSurface@ui@@QEAAXVSize@gfx@@@Z)"},
       &g_LayerSurfaceSetSurfaceSize, nullptr, true},
      {{LR"(?GetLayersInOrder@View@views@@QEAA?AV?$vector@PEAVLayer@ui@@V?$allocator@PEAVLayer@ui@@@__Cr@std@@@__Cr@std@@W4ViewLayer@2@@Z)"},
       &g_ViewGetLayersInOrder, nullptr, true},
      {{LR"(??3@YAXPEAX@Z)"}, &g_ChromeOperatorDelete, nullptr, true},
      {{LR"(?HWNDForWidget@views@@YAPEAUHWND__@@PEBVWidget@1@@Z)"},
       &g_HwndForWidget, nullptr, true},
      {{LR"(?TogglePlayPause@VideoPictureInPictureWindowControllerImpl@content@@UEAA_NXZ)"},
       &g_ControllerTogglePlayPause, nullptr, true},
      {{LR"(?GetWebContents@VideoPictureInPictureWindowControllerImpl@content@@UEAAPEAVWebContents@2@XZ)"},
       &g_ControllerGetWebContents, nullptr, true},
      {{LR"(?GetTitle@WebContentsImpl@content@@UEAAAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@XZ)"},
       &g_WebContentsGetTitle, nullptr, true},
      {{LR"(?SetPlaybackState@PlaybackImageButton@@QEAAXW4PlaybackState@VideoOverlayWindow@content@@@Z)"},
       &g_PlaybackButtonSetPlaybackState, nullptr, true},
      {{LR"(?GetInstance@PictureInPictureWindowManager@@SAPEAV1@XZ)"},
       &g_PipManagerGetInstance, nullptr, true},
      {{LR"(?ExitPictureInPictureViaWindowUi@PictureInPictureWindowManager@@QEAA_NW4UiBehavior@1@@Z)"},
       &g_PipManagerExitViaWindowUi, nullptr, true},

      {{LR"(??_7CloseImageButton@@6BLayerDelegate@ui@@@)"}, &g_vtCloseButton},
      {{LR"(??_7PlaybackImageButton@@6BLayerDelegate@ui@@@)"},
       &g_vtPlaybackButton},
      {{LR"(??_7View@views@@6BLayerDelegate@ui@@@)"}, &g_vtView},
      {{LR"(??_7ImageView@views@@6BLayerDelegate@ui@@@)"}, &g_vtImageView, nullptr,
       true},
      {{LR"(??_7Label@views@@6BLayerDelegate@ui@@@)"}, &g_vtLabel, nullptr, true},
      {{LR"(??_7MediaProgressView@global_media_controls@@6BLayerDelegate@ui@@@)"},
       &g_vtMediaProgressView, nullptr, true},
      {{LR"(??_7OverlayWindowMinimizeButton@@6BLayerDelegate@ui@@@)"},
       &g_vtMinimizeButton, nullptr, true},
      {{LR"(??_7OverlayWindowBackToTabButton@@6BLayerDelegate@ui@@@)"},
       &g_vtBackToTabButton, nullptr, true},
      {{LR"(??_7SimpleOverlayWindowImageButton@@6BLayerDelegate@ui@@@)"},
       &g_vtSimpleButton, nullptr, true},
      {{LR"(??_7ToggleMuteButton@@6BLayerDelegate@ui@@@)"},
       &g_vtToggleMuteButton, nullptr, true},
      {{LR"(??_7OverlayWindowLiveCaptionButton@@6BLayerDelegate@ui@@@)"},
       &g_vtLiveCaptionButton, nullptr, true},
      {{LR"(??_7VideoOverlayWindowViews@@6BNativeWidgetDelegate@internal@views@@@)"},
       &g_vtWindowWidgetPart, nullptr, true},
      {{LR"(??_7VideoPictureInPictureWindowControllerImpl@content@@6BVideoPictureInPictureWindowController@1@@)"},
       &g_vtController, nullptr, true},
      {{LR"(?kKeepIcon@views@@3UVectorIcon@gfx@@B)"}, &g_keepIcon, nullptr, true},
      {{LR"(?kKeepFilledIcon@views@@3UVectorIcon@gfx@@B)"}, &g_keepFilledIcon,
       nullptr, true},
      {{LR"(??_7ToggleCameraButton@@6BLayerDelegate@ui@@@)"},
       &g_vtToggleCameraButton, nullptr, true},
      {{LR"(??_7ToggleMicrophoneButton@@6BLayerDelegate@ui@@@)"},
       &g_vtToggleMicrophoneButton, nullptr, true},
      {{LR"(?kFullscreenIcon@@3UVectorIcon@gfx@@B)"}, &g_cropIcon, nullptr,
       true},
      {{LR"(?AddChildViewAtImpl@View@views@@AEAAXPEAV12@_K@Z)"},
       &g_ViewAddChildViewAtImpl, nullptr, true},
  };

  WH_HOOK_SYMBOLS_OPTIONS options = {};
  options.optionsSize = sizeof(options);
  options.symbolServer = kChromeSymbolServer;
  // chrome.dll.pdb is several GB. Skipping undecoration keeps the one-time
  // pass per Chrome build fast; Windhawk caches the result.
  options.noUndecoratedSymbols = TRUE;

  if (!WindhawkUtils::HookSymbols(chromeDll, chromeDllHooks,
                                  ARRAYSIZE(chromeDllHooks), &options)) {
    Wh_Log(L"ERROR: Failed to resolve Chrome functions");
    return false;
  }

  Wh_Log(L"Chrome functions resolved. Button style: %d, mouse: %d, pin: %d, "
         L"tab title: %d, closing: %d, one row: %d",
         g_UpdateImageAndTextOriginal && g_ViewSetBackground &&
             g_CreatePillBackground,
         g_HwndForWidget && g_vtWindowWidgetPart && g_vtController &&
             g_ControllerTogglePlayPause,
         g_ImageModelFromVectorIcon && g_ImageModelDtor &&
             g_ImageButtonSetImageModel && g_keepIcon && g_keepFilledIcon,
         g_SetSourceTitleOriginal && g_ControllerGetWebContents &&
             g_WebContentsGetTitle && g_LabelGetText && g_LabelSetText,
         g_CloseAndPauseIfAvailableOriginal && g_PipManagerExitViaWindowUi,
         g_ViewGetPreferredSize && g_UpdateTimestampLabelOriginal);
  return true;
}

using LoadLibraryExWFn = decltype(&LoadLibraryExW);
static LoadLibraryExWFn g_LoadLibraryExWOriginal;

static std::atomic<bool> g_setupThreadStarted{false};
static HANDLE g_setupThread;

// Resolving symbols can take long the first time a Chrome version runs, so
// at Chrome startup it happens off the loader thread.
static DWORD WINAPI SetupThreadProc(LPVOID param) {
  if (HookChrome(static_cast<HMODULE>(param))) {
    Wh_ApplyHookOperations();
  }
  return 0;
}

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR fileName, HANDLE file,
                                         DWORD flags) {
  HMODULE module = g_LoadLibraryExWOriginal(fileName, file, flags);
  if (!module || g_setupThreadStarted.load()) {
    return module;
  }

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");
  if (chromeDll && chromeDll == module &&
      !g_setupThreadStarted.exchange(true)) {
    Wh_Log(L"chrome.dll loaded");
    g_setupThread =
        CreateThread(nullptr, 0, SetupThreadProc, chromeDll, 0, nullptr);
  }
  return module;
}

static bool IsBrowserProcess() {
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argv) {
    return false;
  }

  bool browser = true;
  for (int i = 1; i < argc; i++) {
    if (wcsncmp(argv[i], L"--type=", 7) == 0) {
      browser = false;
      break;
    }
  }
  LocalFree(argv);
  return browser;
}

BOOL Wh_ModInit() {
  // The PiP window lives in the browser process.
  if (!IsBrowserProcess()) {
    return FALSE;
  }

  Wh_Log(L"Browser process, PID=%lu", GetCurrentProcessId());
  LoadSettings();

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");
  if (chromeDll) {
    return HookChrome(chromeDll);
  }

  HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
  auto loadLibraryExW =
      kernelBase ? reinterpret_cast<LoadLibraryExWFn>(
                       GetProcAddress(kernelBase, "LoadLibraryExW"))
                 : nullptr;
  if (!loadLibraryExW ||
      !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExWHook,
                                      &g_LoadLibraryExWOriginal)) {
    Wh_Log(L"ERROR: Failed to hook LoadLibraryExW");
    return FALSE;
  }
  return TRUE;
}

void Wh_ModSettingsChanged() {
  LoadSettings();
}

void Wh_ModUninit() {
  if (g_setupThread) {
    WaitForSingleObject(g_setupThread, INFINITE);
    CloseHandle(g_setupThread);
    g_setupThread = nullptr;
  }

  std::vector<HWND> windows;
  {
    std::lock_guard<std::mutex> lock(g_subclassedMutex);
    windows.swap(g_subclassedWindows);
  }
  for (HWND hwnd : windows) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PipWindowSubclassProc);
  }
}
