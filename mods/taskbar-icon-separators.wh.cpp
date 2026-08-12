// ==WindhawkMod==
// @id              taskbar-separators-prototype
// @name            Taskbar Separators - Prototype
// @description     Creates genuine taskbar separators with selectable native-extent and MaxWidth compatibility width modes.
// @version         0.5.2
// @author          meteoni
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lpropsys -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Prototype for genuine Windows 11 taskbar separators.

Each configured separator is represented by its own real pinned Shell shortcut.
The mod:
1. Writes a compact embedded multi-size separator icon to its Windhawk storage folder.
2. Creates one uniquely identified .lnk per configured separator.
3. Pins each shortcut with the private PinManager COM interface.
4. Moves each pin to its configured position.
5. Gives generated separator TaskListButtons configurable normal/small widths.
6. Suppresses activation clicks, tooltips, and legacy/modern context menus.
7. Marks separator TaskListButtons non-draggable through their native IsDraggable property.
8. Neutralizes pointer-over visuals and keeps narrow separator glyphs centered.
9. Unpins the separators and deletes the generated files when the mod unloads.

The position setting is 1-based for the user: 1 means the first pinned position.

By default, separator width uses the original per-instance taskbar extent
override. It writes only the matched separator's MediumTaskbarButtonExtent and,
on DynamicIconScaling builds, SmallTaskbarButtonExtent after the normal
TaskListButton::UpdateVisualStates hook chain returns. This preserves separate
'width' and 'widthSmall' values and still allows Taskbar height and icon size to
participate in the normal hook chain.

If Taskbar height and icon size compatibility causes Explorer/taskbar crashes,
enable MaxWidth compatibility mode. That mode leaves native/TIS button extents
untouched and constrains only the separator FrameworkElement::MaxWidth. It is
intentionally simpler, but it does NOT distinguish normal and small icon modes:
'width' is used in both modes and 'widthSmall' is ignored.

This is still a prototype. Separator interaction suppression is scoped to
the generated taskbar groups/buttons only; ordinary taskbar items are untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- identifierPrefix: WindhawkSeparator-8F31A7D2
  $name: Separator identifier
  $description: >-
    File/display-name prefix for generated separator shortcuts. Unsupported
    filename/regex characters are replaced with underscores.
- maxWidthCompatibilityMode: false
  $name: MaxWidth compatibility mode
  $description: >-
    Compatibility mode for Taskbar height and icon size / TaskbarIconSize.
    Leave disabled normally. Enable this if the normal separator width mode
    causes Explorer or taskbar crashes when used with that mod. MaxWidth mode
    leaves native taskbar button extents untouched, but it cannot distinguish
    normal and small icon modes: Width is used for both and Small width is
    ignored. Changing this setting reloads the mod and recreates the separator
    buttons cleanly.
- separators:
    - - index: 5
        $name: Position
        $description: 1 = first pinned taskbar position.
      - width: 14
        $name: Width
        $description: Width for the normal taskbar icon mode. In MaxWidth compatibility mode, this width is used for both modes.
      - widthSmall: 10
        $name: Small width
        $description: Width for the small taskbar icon mode. Ignored in MaxWidth compatibility mode.
    - - index: 10
        $name: Position
        $description: 1 = first pinned taskbar position.
      - width: 14
        $name: Width
        $description: Width for the normal taskbar icon mode. In MaxWidth compatibility mode, this width is used for both modes.
      - widthSmall: 10
        $name: Small width
        $description: Width for the small taskbar icon mode. Ignored in MaxWidth compatibility mode.
  $name: Separators
  $description: Add or remove entries to control how many separators are created.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Private taskbar PinManager COM ABI, verified on the current Windows build.
// -----------------------------------------------------------------------------

static const CLSID CLSID_PinManager = {
    0xA5C8D635, 0xB4ED, 0x452B,
    {0x81, 0x09, 0x95, 0x01, 0x78, 0x10, 0x96, 0xD1}
};

static const IID IID_IPinManagerInterop2 = {
    0x87D9E034, 0x56D0, 0x4F8C,
    {0xBE, 0x59, 0x99, 0x7B, 0x01, 0x75, 0x47, 0x10}
};

static const IID IID_IPinManagerInterop3 = {
    0x27216D91, 0x78B8, 0x406D,
    {0x85, 0xC4, 0x5F, 0xDB, 0xF5, 0x1C, 0xD1, 0x6B}
};

enum PINNEDLISTMODIFYCALLER : int {
    // Observed from a native modern taskbar unpin on this build.
    PMC_JUMPVIEWBROKER = 11,

    PMC_TASKBANDINSERT = 23,
    PMC_TASKBANDMODIFY = 24,
    PMC_TASKBANDPIN = 25,
    PMC_TASKBANDPINGROUP = 26,
    PMC_TASKBANDREORDER = 27,
};

struct IPinManagerInterop2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PinItemToTaskbarShim(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE PinItemFromTrustedCaller(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyPrependDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyAppendDefaultTaskbarLayout() = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyInPlaceTaskbarLayout(
        int layoutType) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyReorderTaskbarLayout(
        int layoutType,
        int value) = 0;

    virtual HRESULT STDMETHODCALLTYPE UnpinTaskbarItem(
        PCUIDLIST_ABSOLUTE pidl,
        PINNEDLISTMODIFYCALLER caller) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdatePinnedTaskbarItem(
        PCUIDLIST_ABSOLUTE oldPidl,
        PCUIDLIST_ABSOLUTE newPidl,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

struct IPinManagerInterop3 : IPinManagerInterop2 {
    virtual HRESULT STDMETHODCALLTYPE MoveTaskbarPin(
        PCUIDLIST_ABSOLUTE pidl,
        int index,
        PINNEDLISTMODIFYCALLER caller) = 0;
};

// -----------------------------------------------------------------------------
// Settings/state.
// -----------------------------------------------------------------------------

struct SeparatorSetting {
    int ordinal;       // Stable within this loaded settings snapshot: 1, 2, ...
    int targetIndex;   // Zero-based value passed to MoveTaskbarPin.
    double width;      // Normal width; also used as MaxWidth in compatibility mode.
    double widthSmall; // Small icon mode width in native-extent mode.
};

struct Settings {
    std::wstring identifierPrefix;
    bool maxWidthCompatibilityMode;
    std::vector<SeparatorSetting> separators;
};

static Settings g_settings;
static std::wstring g_storagePath;
static std::wstring g_iconPath;

static std::atomic<bool> g_taskbarViewDllHooked;
static std::atomic<bool> g_unloading;

// Internal AppUserModelID namespace. Kept independent from the user-visible
// filename prefix so user changes don't accidentally create invalid AppIDs.
static constexpr wchar_t kInternalAppIdPrefix[] =
    L"Windhawk.TaskbarSeparator.8F31A7D2";

// -----------------------------------------------------------------------------
// Embedded icon.
//
// Compact five-frame 32-bit ICO generated from the separator artwork.
// Keeps native 16/24/32/48 px taskbar frames plus a crisp 256 px fallback.
// Main stroke max alpha is ~68% to retain the original separator's subtlety.
// -----------------------------------------------------------------------------

static constexpr unsigned char kSeparatorIcon[] = {
    0x00, 0x00, 0x01, 0x00, 0x05, 0x00, 0x10, 0x10, 0x00, 0x00, 0x01, 0x00,
    0x20, 0x00, 0x96, 0x00, 0x00, 0x00, 0x56, 0x00, 0x00, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0xA3, 0x00, 0x00, 0x00, 0xEC, 0x00,
    0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0xD7, 0x00,
    0x00, 0x00, 0x8F, 0x01, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x01, 0x00,
    0x20, 0x00, 0xD8, 0x00, 0x00, 0x00, 0x66, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0xFB, 0x02, 0x00, 0x00, 0x3E, 0x03,
    0x00, 0x00, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00,
    0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
    0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3, 0xFF, 0x61, 0x00,
    0x00, 0x00, 0x5D, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xC5, 0x8F, 0xB1,
    0x0D, 0x80, 0x40, 0x0C, 0x03, 0x1D, 0x44, 0xC1, 0x38, 0x4C, 0xC5, 0x6A,
    0xC9, 0x18, 0x4C, 0x85, 0x28, 0x90, 0x4C, 0xF3, 0x75, 0x1C, 0x7D, 0x84,
    0x70, 0x7B, 0xD2, 0xE5, 0x02, 0x88, 0x45, 0x84, 0x65, 0x7C, 0xC9, 0xA0,
    0xBB, 0x1B, 0xC9, 0x63, 0x5A, 0x30, 0xB6, 0x77, 0x05, 0x5B, 0x06, 0xD7,
    0x0C, 0x9A, 0x99, 0x14, 0x54, 0x0A, 0x30, 0x5D, 0x40, 0x52, 0x1E, 0x51,
    0x05, 0xA6, 0x8E, 0x54, 0x5E, 0x60, 0x47, 0x40, 0x00, 0xCF, 0xAF, 0x05,
    0x72, 0x15, 0xC1, 0xD5, 0x15, 0xDC, 0x5D, 0xC1, 0x89, 0x2F, 0xF7, 0x02,
    0x23, 0xAF, 0x11, 0x61, 0xB9, 0x24, 0x17, 0xE9, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 0x89, 0x50, 0x4E, 0x47,
    0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x08, 0x06, 0x00, 0x00,
    0x00, 0xE0, 0x77, 0x3D, 0xF8, 0x00, 0x00, 0x00, 0x6A, 0x49, 0x44, 0x41,
    0x54, 0x78, 0xDA, 0xE5, 0x93, 0xB1, 0x0D, 0x80, 0x30, 0x0C, 0x04, 0xCF,
    0x09, 0x05, 0x03, 0xB0, 0x30, 0xD3, 0x24, 0x53, 0x31, 0x07, 0x08, 0xA4,
    0xA7, 0x62, 0x00, 0x17, 0x8F, 0x22, 0xC5, 0xF5, 0x4B, 0xA7, 0xB7, 0xCF,
    0x30, 0xDA, 0xF4, 0xDE, 0x6B, 0x26, 0x5F, 0x32, 0xE1, 0xD6, 0x5A, 0x95,
    0xB4, 0xDB, 0x00, 0x40, 0x00, 0x9B, 0x13, 0x00, 0xB0, 0xD8, 0xC2, 0x11,
    0x01, 0xB0, 0x0E, 0xD5, 0xA0, 0xB8, 0xAD, 0xB3, 0x03, 0x52, 0x75, 0x25,
    0x7D, 0x26, 0x59, 0x35, 0xB5, 0x5B, 0x64, 0x5F, 0x91, 0xF5, 0xC8, 0xFA,
    0xC3, 0xA2, 0x7B, 0xAE, 0x3F, 0x18, 0x12, 0x70, 0xBA, 0x01, 0x8F, 0x5B,
    0xD3, 0xCB, 0x0D, 0x38, 0x98, 0x6A, 0x5E, 0xAA, 0x6E, 0x15, 0x90, 0x48,
    0x17, 0x18, 0x68, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
    0x42, 0x60, 0x82, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00,
    0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x20, 0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7A, 0x7A, 0xF4,
    0x00, 0x00, 0x00, 0x9E, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xED, 0x95,
    0xC1, 0x0D, 0x02, 0x31, 0x0C, 0x04, 0xC7, 0xE1, 0xC4, 0x87, 0x2F, 0x7D,
    0x50, 0x0A, 0x0F, 0x8A, 0x8C, 0x8B, 0xA0, 0x16, 0x2A, 0xB8, 0x02, 0x90,
    0x38, 0x58, 0x3E, 0xA1, 0x81, 0x04, 0x69, 0x85, 0xC8, 0x4A, 0x79, 0xC6,
    0x9E, 0xD8, 0x6B, 0x07, 0x7E, 0x59, 0x99, 0x19, 0x99, 0x59, 0x46, 0x62,
    0x74, 0x5F, 0xAE, 0xB5, 0x86, 0xA4, 0x45, 0xD2, 0xC5, 0x02, 0x00, 0x04,
    0x70, 0x06, 0x4E, 0x2E, 0x00, 0x80, 0x03, 0x70, 0x74, 0x02, 0x14, 0x60,
    0xE7, 0x04, 0x88, 0x51, 0x80, 0xA5, 0x3B, 0x73, 0x04, 0x2D, 0xF9, 0xDE,
    0x69, 0xC2, 0x32, 0xF2, 0x88, 0x6F, 0xB4, 0x60, 0x58, 0x13, 0xA0, 0xBB,
    0x7F, 0x92, 0x3E, 0x3E, 0x88, 0xBF, 0xDF, 0x03, 0xE1, 0xDC, 0x03, 0x76,
    0x0F, 0xCC, 0x31, 0x7C, 0xB5, 0x63, 0x05, 0x78, 0xCE, 0x4D, 0x38, 0x01,
    0x5C, 0x00, 0x02, 0x36, 0xE0, 0xEE, 0xAC, 0x80, 0xDC, 0x53, 0x60, 0x1F,
    0xC3, 0x0D, 0x78, 0x38, 0x3D, 0xB0, 0x02, 0x37, 0xCB, 0x67, 0xD4, 0x74,
    0x6D, 0x55, 0x98, 0xEA, 0xD6, 0x1B, 0x1B, 0x0D, 0x24, 0x96, 0xB9, 0xB5,
    0x4C, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42,
    0x60, 0x82, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00,
    0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
    0x00, 0x30, 0x08, 0x06, 0x00, 0x00, 0x00, 0x57, 0x02, 0xF9, 0x87, 0x00,
    0x00, 0x00, 0x9F, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xED, 0x96, 0x31,
    0x0E, 0x02, 0x31, 0x0C, 0x04, 0x37, 0x84, 0x82, 0x07, 0xF0, 0xDC, 0x6B,
    0x78, 0x4D, 0xFC, 0x2A, 0xDE, 0x01, 0xE2, 0x24, 0xD3, 0xE4, 0x9A, 0x13,
    0xA2, 0x0B, 0x1B, 0x60, 0xB6, 0x89, 0x94, 0xC2, 0xD1, 0x66, 0x64, 0x7B,
    0x25, 0xF4, 0xC3, 0x8A, 0x88, 0x1A, 0x11, 0x75, 0xE4, 0x1B, 0x87, 0x6F,
    0xFF, 0xA4, 0xE3, 0x88, 0xA2, 0xAD, 0xB5, 0x2A, 0x49, 0x99, 0x79, 0xE9,
    0x57, 0x0B, 0x04, 0x3E, 0x49, 0x40, 0x52, 0xE9, 0xE7, 0x79, 0xB4, 0x01,
    0x08, 0x98, 0xEB, 0x43, 0xE0, 0x75, 0x03, 0x94, 0xAD, 0x05, 0x74, 0x82,
    0x00, 0x3D, 0x30, 0xB9, 0x30, 0x80, 0x01, 0xA6, 0xD0, 0x00, 0x65, 0xE6,
    0x3E, 0x13, 0x41, 0xC0, 0x95, 0x46, 0xD9, 0x03, 0xEE, 0x2C, 0xC4, 0x14,
    0x72, 0x4F, 0x21, 0x08, 0xB8, 0xA6, 0x50, 0x42, 0x60, 0x92, 0xBC, 0xFE,
    0x80, 0x00, 0x06, 0x30, 0x80, 0x01, 0x0C, 0xFC, 0xF5, 0x1E, 0xB8, 0x41,
    0xC0, 0x4C, 0x60, 0x85, 0x80, 0x39, 0x8D, 0xDE, 0x21, 0x60, 0x26, 0x70,
    0x85, 0x00, 0x42, 0xE8, 0xAD, 0x9E, 0x87, 0xF7, 0x18, 0x47, 0xDD, 0x53,
    0x1E, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42,
    0x60, 0x82, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00,
    0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x5C, 0x72, 0xA8, 0x66, 0x00,
    0x00, 0x02, 0xC2, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xED, 0xDD, 0xC1,
    0x6D, 0xC2, 0x50, 0x10, 0x45, 0x51, 0x7F, 0x60, 0x41, 0x01, 0x14, 0xE7,
    0x62, 0xD8, 0xA4, 0x1A, 0xBB, 0x2A, 0xEA, 0x48, 0x44, 0xA2, 0xA1, 0x04,
    0x7E, 0xB2, 0x8A, 0xDF, 0x9C, 0xB3, 0xB6, 0x58, 0x8C, 0xA5, 0xAB, 0x59,
    0x8C, 0xCC, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x34, 0x8C, 0xA0, 0x8F, 0x7D, 0xDF, 0xCF, 0xB3, 0xCF,
    0xAE, 0xEB, 0xFA, 0x63, 0x62, 0xF9, 0x4E, 0x46, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00,
    0x80, 0x00, 0x00, 0xC7, 0x77, 0x31, 0x82, 0xE3, 0xDB, 0xB6, 0x6D, 0xEA,
    0xC2, 0xAF, 0xAA, 0x3E, 0x7E, 0xF1, 0xB3, 0x77, 0x93, 0xB5, 0x01, 0x00,
    0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0xFC, 0x67, 0x4E, 0x81,
    0x33, 0xCC, 0x7E, 0xDC, 0xF5, 0x66, 0x54, 0xD8, 0x00, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04,
    0x00, 0x10, 0x00, 0x40, 0x00, 0x80, 0x14, 0x4E, 0x81, 0xBD, 0x6F, 0x6C,
    0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00,
    0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0xC0, 0x91, 0xB9, 0x0C, 0x0B,
    0x30, 0xC6, 0xEC, 0x37, 0x41, 0x97, 0xAB, 0x69, 0x61, 0x03, 0x00, 0x04,
    0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00,
    0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x52, 0x38, 0x05, 0xF6, 0xBE,
    0xB1, 0x01, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00,
    0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00,
    0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00,
    0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0xF0, 0x9E,
    0xFF, 0x8B, 0x0F, 0x50, 0x55, 0xB3, 0x8F, 0x0E, 0xD3, 0xC2, 0x06, 0x00,
    0x08, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0xA4, 0x70, 0x0A, 0x9C,
    0x61, 0x78, 0xDF, 0xD8, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00,
    0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x10, 0x00, 0xA0,
    0x29, 0x97, 0x61, 0x01, 0xC6, 0xF0, 0xAD, 0x4F, 0x6C, 0x00, 0x80, 0x00,
    0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00,
    0x08, 0x00, 0x08, 0x00, 0x20, 0x00, 0x40, 0x3F, 0x4E, 0x81, 0x03, 0x54,
    0x95, 0x21, 0x60, 0x03, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00,
    0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x01,
    0x00, 0xDA, 0x71, 0x0A, 0x9C, 0xC1, 0x2D, 0x30, 0x36, 0x00, 0x40, 0x00,
    0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00,
    0x04, 0x00, 0x04, 0x00, 0xE8, 0xCA, 0x25, 0x60, 0x2F, 0x4F, 0x23, 0xC0,
    0x06, 0x00, 0x08, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80,
    0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80,
    0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x02,
    0x00, 0x08, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x7D, 0x5C,
    0x8C, 0xA0, 0x95, 0x4F, 0x23, 0xC0, 0x06, 0x00, 0x08, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80,
    0x00, 0x00, 0x02, 0x00, 0xA4, 0x70, 0x0A, 0xDC, 0xCB, 0xB7, 0x11, 0x60,
    0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x80, 0x30, 0x2E, 0x01, 0x33,
    0xD4, 0xE4, 0x73, 0x5F, 0x46, 0x85, 0x0D, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x40, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00,
    0x01, 0x00, 0x04, 0x00, 0x48, 0xE1, 0x14, 0x38, 0xC3, 0xEC, 0x29, 0xF0,
    0xC3, 0xA8, 0xB0, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20,
    0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xF2,
    0x02, 0x89, 0xB8, 0x19, 0xBE, 0xE1, 0x19, 0xC6, 0xA9, 0x00, 0x00, 0x00,
    0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};

static_assert(sizeof(kSeparatorIcon) == 1593);

// -----------------------------------------------------------------------------
// Small helpers.
// -----------------------------------------------------------------------------

static std::wstring JoinPath(
    const std::wstring& directory,
    const std::wstring& fileName) {
    if (directory.empty()) {
        return fileName;
    }

    if (directory.back() == L'\\' || directory.back() == L'/') {
        return directory + fileName;
    }

    return directory + L"\\" + fileName;
}

static std::wstring SanitizePrefix(std::wstring value) {
    // Keep this intentionally restrictive:
    // - safe as a Windows filename
    // - easy to use later in a XAML regex
    // - no escaping surprises
    for (wchar_t& ch : value) {
        const bool allowed =
            (ch >= L'a' && ch <= L'z') ||
            (ch >= L'A' && ch <= L'Z') ||
            (ch >= L'0' && ch <= L'9') ||
            ch == L'-' ||
            ch == L'_';

        if (!allowed) {
            ch = L'_';
        }
    }

    while (!value.empty() &&
           (value.back() == L' ' || value.back() == L'.')) {
        value.pop_back();
    }

    if (value.empty()) {
        value = L"WindhawkSeparator-8F31A7D2";
    }

    // Keep generated paths and future XAML selectors reasonably short.
    constexpr size_t kMaxPrefixLength = 64;
    if (value.size() > kMaxPrefixLength) {
        value.resize(kMaxPrefixLength);
    }

    return value;
}

static std::wstring GetSeparatorBaseName(int ordinal) {
    return g_settings.identifierPrefix + L"-" + std::to_wstring(ordinal);
}

static std::wstring GetSeparatorShortcutPath(int ordinal) {
    return JoinPath(
        g_storagePath,
        GetSeparatorBaseName(ordinal) + L".lnk");
}

static std::wstring GetSeparatorAppId(int ordinal) {
    return std::wstring(kInternalAppIdPrefix) +
           L"." +
           std::to_wstring(ordinal);
}

static bool FileExists(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES &&
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static bool DeleteFileIfPresent(const std::wstring& path) {
    if (!FileExists(path)) {
        return true;
    }

    if (DeleteFileW(path.c_str())) {
        return true;
    }

    Wh_Log(
        L"[FILES] DeleteFileW('%s') failed error=%u",
        path.c_str(),
        GetLastError());

    return false;
}

static bool WriteBinaryFile(
    const std::wstring& path,
    const void* data,
    DWORD size) {
    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(
            L"[FILES] CreateFileW('%s') failed error=%u",
            path.c_str(),
            GetLastError());
        return false;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(
        file,
        data,
        size,
        &written,
        nullptr);

    DWORD error = ok ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);

    if (!ok || written != size) {
        Wh_Log(
            L"[FILES] WriteFile('%s') failed/incomplete "
            L"error=%u written=%u expected=%u",
            path.c_str(),
            error,
            written,
            size);
        return false;
    }

    return true;
}

static bool LoadSettings() {
    g_settings = {};

    PCWSTR prefixSetting = Wh_GetStringSetting(L"identifierPrefix");
    if (prefixSetting) {
        g_settings.identifierPrefix = SanitizePrefix(prefixSetting);
        Wh_FreeStringSetting(prefixSetting);
    } else {
        g_settings.identifierPrefix =
            L"WindhawkSeparator-8F31A7D2";
    }

    g_settings.maxWidthCompatibilityMode =
        Wh_GetIntSetting(L"maxWidthCompatibilityMode") != 0;

    // Wh_GetIntSetting returns 0 for a missing setting.
    // Positions are therefore deliberately user-facing 1-based values.
    constexpr int kMaxSeparators = 64;

    for (int i = 0; i < kMaxSeparators; i++) {
        int position =
            Wh_GetIntSetting(L"separators[%d].index", i);

        if (position == 0) {
            break;
        }

        if (position < 1) {
            Wh_Log(
                L"[SETTINGS] Ignoring separator %d: invalid position=%d",
                i + 1,
                position);
            continue;
        }

        int width =
            Wh_GetIntSetting(L"separators[%d].width", i);
        int widthSmall =
            Wh_GetIntSetting(L"separators[%d].widthSmall", i);

        // Keep old settings snapshots usable if these fields didn't exist yet.
        if (width <= 0) {
            width = 14;
        }
        if (widthSmall <= 0) {
            widthSmall = 10;
        }

        g_settings.separators.push_back({
            .ordinal = i + 1,
            .targetIndex = position - 1,
            .width = static_cast<double>(width),
            .widthSmall = static_cast<double>(widthSmall),
        });
    }

    Wh_Log(
        L"[SETTINGS] prefix='%s' separators=%zu widthMode=%s",
        g_settings.identifierPrefix.c_str(),
        g_settings.separators.size(),
        g_settings.maxWidthCompatibilityMode
            ? L"MaxWidthCompatibility"
            : L"NativeExtent");

    for (const auto& separator : g_settings.separators) {
        Wh_Log(
            L"[SETTINGS] separator=%d targetIndex=%d (user position=%d) "
            L"width=%g widthSmall=%g",
            separator.ordinal,
            separator.targetIndex,
            separator.targetIndex + 1,
            separator.width,
            separator.widthSmall);
    }

    return true;
}

static bool InitializeStoragePath() {
    wchar_t buffer[32768] = {};

    size_t chars =
        Wh_GetModStoragePath(buffer, ARRAYSIZE(buffer));

    if (!chars || !buffer[0]) {
        Wh_Log(L"[FILES] Wh_GetModStoragePath failed");
        return false;
    }

    g_storagePath = buffer;

    // The API provides a usable mod storage directory, but this is harmless
    // if the directory already exists.
    if (!CreateDirectoryW(g_storagePath.c_str(), nullptr)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            Wh_Log(
                L"[FILES] CreateDirectoryW('%s') failed error=%u",
                g_storagePath.c_str(),
                error);
            return false;
        }
    }

    g_iconPath =
        JoinPath(
            g_storagePath,
            g_settings.identifierPrefix + L".ico");

    Wh_Log(L"[FILES] storage='%s'", g_storagePath.c_str());
    Wh_Log(L"[FILES] icon='%s'", g_iconPath.c_str());

    return true;
}


// -----------------------------------------------------------------------------
// TaskListButton separator width styling.
//
// Native-extent mode (default):
// - restore the original separator width mechanism;
// - let Windows and all downstream UpdateVisualStates hooks finish first;
// - then write only this separator instance's medium/small taskbar extents;
// - re-run the real padding/column entry points so other installed hooks can
//   still participate.
//
// MaxWidth compatibility mode:
// - never writes the private medium/small taskbar extent fields;
// - after UpdateVisualStates finishes, constrains only FrameworkElement::MaxWidth;
// - intended as a fallback if Taskbar height and icon size / TaskbarIconSize
//   compatibility causes Explorer/taskbar crashes;
// - deliberately uses 'width' in both icon modes, so widthSmall is unavailable.
//
// Settings changes always request a full mod reload, so changing width mode
// destroys/recreates the separator buttons instead of hot-swapping ownership on
// live TaskListButtons.
// -----------------------------------------------------------------------------

static void* g_taskListButtonUpdateButtonPaddingAddress;
static void* g_taskListButtonUpdateIconColumnDefinitionAddress;
static void* g_taskbarConfigurationGetIconHeightInViewPixelsAddress;
static bool g_hasDynamicIconScaling;

// Native TaskListButton IsDraggable XAML binding helpers, resolved from
// Taskbar.View.dll. The isolated runtime probe verified that setting this
// property to false prevents drag initiation without touching pointer input,
// drag gesture handlers, or the taskbar model's reorder path.
using TaskListButton_GetIsDraggable_t =
    winrt::Windows::Foundation::IInspectable(*)(
        winrt::Windows::Foundation::IInspectable const& target);

static TaskListButton_GetIsDraggable_t
    g_taskListButtonGetIsDraggable;

using TaskListButton_SetIsDraggable_t =
    void(*)(
        winrt::Windows::Foundation::IInspectable const& target,
        winrt::Windows::Foundation::IInspectable const& value);

static TaskListButton_SetIsDraggable_t
    g_taskListButtonSetIsDraggable;

static thread_local bool g_settingSeparatorIsDraggable;

static std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);

    static RtlQueryFeatureConfiguration_t queryFeatureConfiguration = [] {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        return ntdll
                   ? reinterpret_cast<RtlQueryFeatureConfiguration_t>(
                         GetProcAddress(ntdll, "RtlQueryFeatureConfiguration"))
                   : nullptr;
    }();

    if (!queryFeatureConfiguration) {
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {};
    INT64 changeStamp = 0;

    HRESULT hr = queryFeatureConfiguration(
        featureId,
        1,
        &changeStamp,
        &feature);

    if (FAILED(hr)) {
        return std::nullopt;
    }

    switch (feature.enabledState) {
        case FEATURE_ENABLED_STATE_DISABLED:
            return false;
        case FEATURE_ENABLED_STATE_ENABLED:
            return true;
        default:
            return std::nullopt;
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
static TaskListButton_UpdateVisualStates_t
    g_taskListButtonUpdateVisualStatesOriginal;

static FrameworkElement GetTaskListButtonElement(void* pThis) {
    FrameworkElement element = nullptr;

    // Same C++/WinRT subobject adjustment used by the existing taskbar mods.
    IUnknown* xamlUnknown =
        reinterpret_cast<IUnknown*>(pThis) + 3;

    HRESULT hr = xamlUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(element));

    if (FAILED(hr)) {
        return nullptr;
    }

    return element;
}

static FrameworkElement FindChildByName(
    const FrameworkElement& root,
    std::wstring_view name) {
    if (!root) {
        return nullptr;
    }

    try {
        int childCount =
            winrt::Windows::UI::Xaml::Media::
                VisualTreeHelper::GetChildrenCount(root);

        for (int i = 0; i < childCount; i++) {
            auto childObject =
                winrt::Windows::UI::Xaml::Media::
                    VisualTreeHelper::GetChild(root, i);

            auto child =
                childObject.try_as<FrameworkElement>();

            if (!child) {
                continue;
            }

            auto childName = child.Name();
            std::wstring_view childNameView{
                childName.c_str(),
                childName.size()
            };

            if (childNameView == name) {
                return child;
            }

            if (auto nested =
                    FindChildByName(child, name)) {
                return nested;
            }
        }
    } catch (...) {
        // The visual tree can change while the taskbar is reconciling items.
        // Treat that as a transient miss; UpdateVisualStates will run again.
    }

    return nullptr;
}

static const SeparatorSetting* FindSeparatorByAutomationName(
    std::wstring_view name) {
    if (g_settings.identifierPrefix.empty() || name.empty()) {
        return nullptr;
    }

    const std::wstring needle =
        g_settings.identifierPrefix + L"-";

    size_t pos = name.find(needle);

    while (pos != std::wstring_view::npos) {
        size_t digitPos = pos + needle.size();

        if (digitPos < name.size() &&
            name[digitPos] >= L'0' &&
            name[digitPos] <= L'9') {
            unsigned int ordinal = 0;
            size_t end = digitPos;

            while (end < name.size() &&
                   name[end] >= L'0' &&
                   name[end] <= L'9') {
                ordinal =
                    ordinal * 10 +
                    static_cast<unsigned int>(name[end] - L'0');

                if (ordinal > 100000) {
                    break;
                }

                end++;
            }

            for (const auto& separator : g_settings.separators) {
                if (separator.ordinal ==
                    static_cast<int>(ordinal)) {
                    return &separator;
                }
            }
        }

        pos = name.find(needle, pos + 1);
    }

    return nullptr;
}

static const SeparatorSetting* GetSeparatorForElement(
    const FrameworkElement& element) {
    if (!element) {
        return nullptr;
    }

    winrt::hstring automationName;

    try {
        automationName =
            winrt::Windows::UI::Xaml::Automation::
                AutomationProperties::GetName(element);
    } catch (...) {
        return nullptr;
    }

    std::wstring_view name{
        automationName.c_str(),
        automationName.size()
    };

    return FindSeparatorByAutomationName(name);
}

static const SeparatorSetting* GetSeparatorForTaskListButton(
    void* pThis,
    FrameworkElement* elementOut = nullptr) {
    FrameworkElement element =
        GetTaskListButtonElement(pThis);

    const SeparatorSetting* separator =
        GetSeparatorForElement(element);

    if (separator && elementOut) {
        *elementOut = element;
    }

    return separator;
}

static LONG GetMediumTaskbarButtonExtentOffset() {
    static LONG offset = []() -> LONG {
        if (!g_taskListButtonUpdateIconColumnDefinitionAddress) {
            Wh_Log(
                L"[STYLE] UpdateIconColumnDefinition address unavailable");
            return 0;
        }

        // x64 logic from the upstream Taskbar height and icon size mod.
        // Search for a movsd that loads the per-instance medium extent,
        // followed by a subsd using the loaded value.
        const BYTE* start =
            reinterpret_cast<const BYTE*>(
                g_taskListButtonUpdateIconColumnDefinitionAddress);
        const BYTE* end = start + 0x200;

        LONG offsetCandidate = 0;
        LONG foundOffset = 0;

        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0xC0) == 0x80) {
                offsetCandidate =
                    *reinterpret_cast<const LONG*>(p + 4);
            }

            if (p[0] == 0xF2 && p[1] == 0x44 &&
                p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0xC0) == 0x80) {
                offsetCandidate =
                    *reinterpret_cast<const LONG*>(p + 5);
            }

            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x5C &&
                (p[3] & 0xC0) == 0x80) {
                foundOffset = offsetCandidate;
                break;
            }

            if (p[0] == 0xF2 && p[1] == 0x44 &&
                p[2] == 0x0F && p[3] == 0x5C &&
                (p[4] & 0xC0) == 0x80) {
                foundOffset = offsetCandidate;
                break;
            }
        }

        Wh_Log(
            L"[STYLE] mediumTaskbarButtonExtentOffset=0x%X",
            foundOffset);

        return (foundOffset < 0 || foundOffset > 0xFFFF)
                   ? 0
                   : foundOffset;
    }();

    return offset;
}

static void RefreshTaskListButtonLayout(void* pThis) {
    // Call real function entry points, not our own trampolines.
    //
    // UpdateButtonPadding is intentionally invoked through its target address:
    // if Taskbar height and icon size hooks it, that hook participates.
    if (g_taskListButtonUpdateButtonPaddingAddress) {
        auto updateButtonPadding =
            reinterpret_cast<void(WINAPI*)(void*)>(
                g_taskListButtonUpdateButtonPaddingAddress);

        updateButtonPadding(pThis);
    }

    // Our width write occurs after UpdateVisualStates has already returned.
    // Re-run the column update explicitly so XAML consumes the new extent.
    if (g_taskListButtonUpdateIconColumnDefinitionAddress) {
        auto updateIconColumnDefinition =
            reinterpret_cast<void(WINAPI*)(void*)>(
                g_taskListButtonUpdateIconColumnDefinitionAddress);

        updateIconColumnDefinition(pThis);
    }
}

static void ApplySeparatorWidthOverride(void* pThis) {
    if (g_unloading ||
        !g_taskListButtonUpdateIconColumnDefinitionAddress) {
        return;
    }

    const SeparatorSetting* separator =
        GetSeparatorForTaskListButton(pThis);

    if (!separator) {
        return;
    }

    LONG mediumOffset =
        GetMediumTaskbarButtonExtentOffset();

    if (!mediumOffset) {
        return;
    }

    auto* mediumExtent =
        reinterpret_cast<double*>(
            reinterpret_cast<BYTE*>(pThis) + mediumOffset);

    bool changed = false;

    if (*mediumExtent >= 1 &&
        *mediumExtent < 10000 &&
        *mediumExtent != separator->width) {
        *mediumExtent = separator->width;
        changed = true;
    }

    // On DynamicIconScaling builds the small extent immediately precedes
    // the medium extent. Don't assume that layout on older taskbars.
    if (g_hasDynamicIconScaling) {
        double* smallExtent = mediumExtent - 1;

        if (*smallExtent >= 1 &&
            *smallExtent < 10000 &&
            *smallExtent != separator->widthSmall) {
            *smallExtent = separator->widthSmall;
            changed = true;
        }
    }

    if (changed) {
        RefreshTaskListButtonLayout(pThis);
    }
}

struct SeparatorMaxWidthState {
    winrt::weak_ref<FrameworkElement> element;
    void* identity;
    double originalMaxWidth;
};

static std::vector<SeparatorMaxWidthState> g_separatorMaxWidthStates;

static void PruneExpiredSeparatorMaxWidthStates() {
    g_separatorMaxWidthStates.erase(
        std::remove_if(
            g_separatorMaxWidthStates.begin(),
            g_separatorMaxWidthStates.end(),
            [](const SeparatorMaxWidthState& state) {
                return !state.element.get();
            }),
        g_separatorMaxWidthStates.end());
}

static auto FindSeparatorMaxWidthState(void* identity) {
    return std::find_if(
        g_separatorMaxWidthStates.begin(),
        g_separatorMaxWidthStates.end(),
        [identity](const SeparatorMaxWidthState& state) {
            return state.identity == identity;
        });
}

static void ApplySeparatorMaxWidthOverride(
    const FrameworkElement& element,
    const SeparatorSetting* separator) {
    if (!element) {
        return;
    }

    PruneExpiredSeparatorMaxWidthStates();

    void* identity = winrt::get_abi(element);
    auto stateIt = FindSeparatorMaxWidthState(identity);

    // If a TaskListButton is recycled/rebound and no longer represents one of
    // our separators, undo only the MaxWidth value that SEP itself installed.
    if (g_unloading || !separator) {
        if (stateIt != g_separatorMaxWidthStates.end()) {
            try {
                if (element.MaxWidth() != stateIt->originalMaxWidth) {
                    Wh_Log(
                        L"[STYLE] Restoring recycled TaskListButton MaxWidth: "
                        L"%g -> %g",
                        element.MaxWidth(),
                        stateIt->originalMaxWidth);
                    element.MaxWidth(stateIt->originalMaxWidth);
                }
            } catch (...) {
                // The element may be disappearing during taskbar reconstruction.
            }

            g_separatorMaxWidthStates.erase(stateIt);
        }

        return;
    }

    // Compatibility mode intentionally has one width only.
    const double desiredMaxWidth = separator->width;
    if (!(desiredMaxWidth > 0)) {
        return;
    }

    if (stateIt == g_separatorMaxWidthStates.end()) {
        double originalMaxWidth;

        try {
            originalMaxWidth = element.MaxWidth();
        } catch (...) {
            return;
        }

        g_separatorMaxWidthStates.push_back({
            .element = winrt::make_weak(element),
            .identity = identity,
            .originalMaxWidth = originalMaxWidth,
        });

        stateIt = std::prev(g_separatorMaxWidthStates.end());
    }

    try {
        if (element.MaxWidth() != desiredMaxWidth) {
            Wh_Log(
                L"[STYLE] Separator MaxWidth: %g -> %g "
                L"(native/TIS extents untouched)",
                element.MaxWidth(),
                desiredMaxWidth);
            element.MaxWidth(desiredMaxWidth);
        }
    } catch (...) {
        // Treat taskbar reconstruction as a transient miss.
    }
}

static void CenterSeparatorIcon(
    const FrameworkElement& taskListButton) {
    if (!taskListButton || g_unloading) {
        return;
    }

    try {
        // Once a TaskListButton gets narrower than the normal icon slot, the
        // stock template can effectively left-anchor the icon content. Center
        // both the slot and the actual Icon child instead of using a magic
        // pixel translation, so the correction adapts to icon-size mods/DPI.
        auto iconPanel =
            FindChildByName(
                taskListButton,
                L"IconPanel");

        if (!iconPanel) {
            return;
        }

        iconPanel.HorizontalAlignment(
            HorizontalAlignment::Center);

        if (auto icon =
                FindChildByName(
                    iconPanel,
                    L"Icon")) {
            icon.HorizontalAlignment(
                HorizontalAlignment::Center);
        }
    } catch (...) {
        // Cosmetic only. Leave the stock layout intact on a future template.
    }
}

static void SuppressSeparatorHoverChrome(
    const FrameworkElement& taskListButton) {
    if (!taskListButton || g_unloading) {
        return;
    }

    try {
        // Taskbar.TaskListLabeledButtonPanel owns the task-button CommonStates.
        // Its Border#BackgroundElement is the rounded hover/press surface.
        // Keep it in the tree (so layout is unaffected) and make only that
        // surface invisible for separator buttons.
        auto iconPanel =
            FindChildByName(
                taskListButton,
                L"IconPanel");

        if (iconPanel) {
            if (auto backgroundElement =
                    FindChildByName(
                        iconPanel,
                        L"BackgroundElement")) {
                backgroundElement.Opacity(0.0);
            }
        }

        // Clear XAML-attached tooltip content as a presentation-layer fallback.
        winrt::Windows::Foundation::IInspectable noToolTip{nullptr};

        winrt::Windows::UI::Xaml::Controls::ToolTipService::
            SetToolTip(
                taskListButton,
                noToolTip);

        if (iconPanel) {
            winrt::Windows::UI::Xaml::Controls::ToolTipService::
                SetToolTip(
                    iconPanel,
                    noToolTip);
        }
    } catch (...) {
        // Cosmetic only. The taskbar model hooks below still suppress click and
        // tooltip text even if a future template changes the XAML subtree.
    }
}

static void DisableSeparatorDragging(
    const FrameworkElement& taskListButton) {
    if (!taskListButton ||
        g_unloading ||
        g_settingSeparatorIsDraggable ||
        !g_taskListButtonGetIsDraggable ||
        !g_taskListButtonSetIsDraggable) {
        return;
    }

    g_settingSeparatorIsDraggable = true;

    try {
        auto target =
            taskListButton.as<
                winrt::Windows::Foundation::IInspectable>();

        auto beforeBox =
            g_taskListButtonGetIsDraggable(target);

        bool before =
            winrt::unbox_value<bool>(beforeBox);

        if (before) {
            auto falseBox =
                winrt::box_value(false);

            g_taskListButtonSetIsDraggable(
                target,
                falseBox);
        }
    } catch (...) {
        // Input hardening only. If a future Taskbar.View build changes the
        // binding helper ABI, leave stock drag behavior rather than affecting
        // unrelated taskbar buttons.
    }

    g_settingSeparatorIsDraggable = false;
}

// Modern WinUI taskbar context-menu paths.
//
// Both hooks are retained because the runtime probe showed that a separator
// right-click reaches both handlers on the current build. They are filtered
// independently and neither touches generic pointer input.
using TaskListButton_OnContextRequested_t =
    void(WINAPI*)(
        void* pThis,
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskListButton_OnContextRequested_t
    g_taskListButtonOnContextRequestedOriginal;

static void WINAPI TaskListButton_OnContextRequested_Hook(
    void* pThis,
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    if (!g_unloading &&
        GetSeparatorForTaskListButton(pThis)) {
        return;
    }

    g_taskListButtonOnContextRequestedOriginal(
        pThis,
        sender,
        args);
}

using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskListButtonHandlers_HandleContextRequested_t
    g_taskListButtonHandlersHandleContextRequestedOriginal;

static void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    if (!g_unloading) {
        auto element =
            sender.try_as<FrameworkElement>();

        if (element &&
            GetSeparatorForElement(element)) {
            return;
        }
    }

    g_taskListButtonHandlersHandleContextRequestedOriginal(
        sender,
        args);
}

static void WINAPI TaskListButton_UpdateVisualStates_Hook(
    void* pThis) {
    // Preserve the original precedence: Windows and every downstream hook,
    // including Taskbar height and icon size, finish first.
    g_taskListButtonUpdateVisualStatesOriginal(pThis);

    FrameworkElement element =
        GetTaskListButtonElement(pThis);
    const SeparatorSetting* separator =
        GetSeparatorForElement(element);

    if (g_settings.maxWidthCompatibilityMode) {
        // Compatibility path: native/TIS extents remain untouched.
        ApplySeparatorMaxWidthOverride(
            element,
            separator);
    } else {
        // Default path: original per-instance medium/small extent override.
        ApplySeparatorWidthOverride(pThis);
    }

    if (!separator || !element || g_unloading) {
        return;
    }

    CenterSeparatorIcon(element);
    SuppressSeparatorHoverChrome(element);
    DisableSeparatorDragging(element);
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

static bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"
            },
            &g_taskListButtonUpdateButtonPaddingAddress,
            nullptr,
            true, // Used only by native-extent mode.
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"
            },
            &g_taskListButtonUpdateIconColumnDefinitionAddress,
            nullptr,
            true, // Used only by native-extent mode.
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"
            },
            &g_taskListButtonUpdateVisualStatesOriginal,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {
                LR"(struct winrt::Windows::Foundation::IInspectable __cdecl winrt::Taskbar::implementation::GetValueTypeMember_IsDraggable<struct winrt::Taskbar::TaskListButton,bool>(struct winrt::Windows::Foundation::IInspectable const &))"
            },
            &g_taskListButtonGetIsDraggable,
            nullptr,
        },
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::SetValueTypeMember_IsDraggable<struct winrt::Taskbar::TaskListButton,bool>(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::Foundation::IInspectable const &))"
            },
            &g_taskListButtonSetIsDraggable,
            nullptr,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskListButtonOnContextRequestedOriginal,
            TaskListButton_OnContextRequested_Hook,
        },
        {
            {
                LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskListButtonHandlersHandleContextRequestedOriginal,
            TaskListButtonHandlers_HandleContextRequested_Hook,
        },
        {
            {
                LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"
            },
            &g_taskbarConfigurationGetIconHeightInViewPixelsAddress,
            nullptr,
            true, // DynamicIconScaling-era builds; native-extent mode only.
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            hooks,
            ARRAYSIZE(hooks))) {
        Wh_Log(L"[STYLE] HookSymbols(Taskbar view) failed");
        return false;
    }

    constexpr UINT kDynamicIconScaling = 29785184;

    g_hasDynamicIconScaling =
        g_taskbarConfigurationGetIconHeightInViewPixelsAddress &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true);

    if (!g_settings.maxWidthCompatibilityMode) {
        // Preserve the requirements of the original native-extent path.
        if (!g_taskListButtonUpdateButtonPaddingAddress ||
            !g_taskListButtonUpdateIconColumnDefinitionAddress ||
            !GetMediumTaskbarButtonExtentOffset()) {
            Wh_Log(
                L"[STYLE] Native-extent width mode is unavailable on this "
                L"Taskbar.View build. Enable 'MaxWidth compatibility mode' "
                L"as a fallback.");
            return false;
        }
    }

    Wh_Log(
        L"[STYLE] Taskbar view hooks ready; widthMode=%s "
        L"DynamicIconScaling=%d IsDraggable=1 modernContext=1",
        g_settings.maxWidthCompatibilityMode
            ? L"MaxWidthCompatibility"
            : L"NativeExtent",
        g_hasDynamicIconScaling);

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_loadLibraryExWOriginal;

static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags) {
    HMODULE module =
        g_loadLibraryExWOriginal(
            lpLibFileName,
            hFile,
            dwFlags);

    if (!module) {
        return module;
    }

    if (!g_taskbarViewDllHooked &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllHooked.exchange(true)) {
        Wh_Log(
            L"[STYLE] Taskbar view module loaded: %s",
            lpLibFileName ? lpLibFileName : L"<unknown>");

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            g_taskbarViewDllHooked = false;
        }
    }

    return module;
}

static bool InitializeTaskbarStylingHooks() {
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllHooked = true;

        if (!HookTaskbarViewDllSymbols(module)) {
            g_taskbarViewDllHooked = false;
            return false;
        }

        return true;
    }

    Wh_Log(
        L"[STYLE] Taskbar view module not loaded yet; "
        L"installing LoadLibraryExW watcher");

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }

    auto loadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));

    if (!loadLibraryExW) {
        return false;
    }

    WindhawkUtils::SetFunctionHook(
        loadLibraryExW,
        LoadLibraryExW_Hook,
        &g_loadLibraryExWOriginal);

    return true;
}



// -----------------------------------------------------------------------------
// Separator interaction suppression.
//
// Taskbar.View owns modern context requests and the native IsDraggable property;
// those are handled above. taskbar.dll is used only for model-level semantic
// actions that already expose the exact ITaskGroup:
//
// - CTaskListWnd::HandleClick:
//     swallow activation for our separator groups.
// - CTaskListWnd::OnContextMenu:
//     swallow the legacy context-menu route.
// - CTaskBtnGroup::ShouldShowToolTip:
//     legacy tooltip gate fallback.
// - CTaskGroup::GetToolTipText:
//     directly report no tooltip text for separator groups.
//
// No pointer press/move hooks, drag gesture hooks, TryMoveGroup hooks, or
// jumplist-hover hooks are needed.
// -----------------------------------------------------------------------------

static void* g_taskGroupGetAppIdAddress;
static void* g_taskBtnGroupGetGroupAddress;

using TaskListWnd_HandleClick_t =
    HRESULT(WINAPI*)(
        void* pThis,
        void* taskGroup,
        void* taskItem,
        const void* launcherOptions);

static TaskListWnd_HandleClick_t
    g_taskListWndHandleClickOriginal;

using TaskListWnd_OnContextMenu_t =
    void(WINAPI*)(
        void* pThis,
        POINT point,
        HWND hwnd,
        bool keyboardInvoked,
        void* taskGroup,
        void* taskItem);

static TaskListWnd_OnContextMenu_t
    g_taskListWndOnContextMenuOriginal;

using TaskBtnGroup_ShouldShowToolTip_t =
    int(WINAPI*)(
        void* pThis,
        void* taskItem);

static TaskBtnGroup_ShouldShowToolTip_t
    g_taskBtnGroupShouldShowToolTipOriginal;


using TaskGroup_GetToolTipText_t =
    HRESULT(WINAPI*)(
        void* pThis,
        void* taskItem,
        wchar_t* text,
        int cchText);

static TaskGroup_GetToolTipText_t
    g_taskGroupGetToolTipTextOriginal;

static bool IsSeparatorTaskGroup(void* taskGroup) {
    if (!taskGroup || !g_taskGroupGetAppIdAddress) {
        return false;
    }

    using CTaskGroup_GetAppID_t =
        const wchar_t*(WINAPI*)(void* pThis);

    auto getAppId =
        reinterpret_cast<CTaskGroup_GetAppID_t>(
            g_taskGroupGetAppIdAddress);

    const wchar_t* appId = getAppId(taskGroup);
    if (!appId || !*appId) {
        return false;
    }

    std::wstring_view appIdView(appId);
    std::wstring_view prefix(kInternalAppIdPrefix);

    if (appIdView.size() <= prefix.size() + 1 ||
        appIdView.substr(0, prefix.size()) != prefix ||
        appIdView[prefix.size()] != L'.') {
        return false;
    }

    std::wstring_view suffix =
        appIdView.substr(prefix.size() + 1);

    if (suffix.empty()) {
        return false;
    }

    unsigned int ordinal = 0;

    for (wchar_t ch : suffix) {
        if (ch < L'0' || ch > L'9') {
            return false;
        }

        ordinal =
            ordinal * 10 +
            static_cast<unsigned int>(ch - L'0');

        if (ordinal > 100000) {
            return false;
        }
    }

    for (const auto& separator : g_settings.separators) {
        if (separator.ordinal ==
            static_cast<int>(ordinal)) {
            return true;
        }
    }

    return false;
}

static HRESULT WINAPI TaskListWnd_HandleClick_Hook(
    void* pThis,
    void* taskGroup,
    void* taskItem,
    const void* launcherOptions) {
    if (!g_unloading &&
        IsSeparatorTaskGroup(taskGroup)) {
        Wh_Log(
            L"[INPUT] Suppressed separator click group=%p item=%p",
            taskGroup,
            taskItem);

        return S_OK;
    }

    return g_taskListWndHandleClickOriginal(
        pThis,
        taskGroup,
        taskItem,
        launcherOptions);
}

static void WINAPI TaskListWnd_OnContextMenu_Hook(
    void* pThis,
    POINT point,
    HWND hwnd,
    bool keyboardInvoked,
    void* taskGroup,
    void* taskItem) {
    if (!g_unloading &&
        IsSeparatorTaskGroup(taskGroup)) {
        return;
    }

    g_taskListWndOnContextMenuOriginal(
        pThis,
        point,
        hwnd,
        keyboardInvoked,
        taskGroup,
        taskItem);
}

static int WINAPI TaskBtnGroup_ShouldShowToolTip_Hook(
    void* pThis,
    void* taskItem) {
    if (!g_unloading &&
        pThis &&
        g_taskBtnGroupGetGroupAddress) {
        using CTaskBtnGroup_GetGroup_t =
            void*(WINAPI*)(void* pThis);

        auto getGroup =
            reinterpret_cast<CTaskBtnGroup_GetGroup_t>(
                g_taskBtnGroupGetGroupAddress);

        void* taskGroup = getGroup(pThis);

        if (IsSeparatorTaskGroup(taskGroup)) {
            return FALSE;
        }
    }

    return g_taskBtnGroupShouldShowToolTipOriginal(
        pThis,
        taskItem);
}


static HRESULT WINAPI TaskGroup_GetToolTipText_Hook(
    void* pThis,
    void* taskItem,
    wchar_t* text,
    int cchText) {
    if (!g_unloading &&
        IsSeparatorTaskGroup(pThis)) {
        if (text && cchText > 0) {
            text[0] = L'\0';
        }

        // Explicitly report "no tooltip text".
        return S_FALSE;
    }

    return g_taskGroupGetToolTipTextOriginal(
        pThis,
        taskItem,
        text,
        cchText);
}

static bool InitializeTaskbarInteractionHooks() {
    HMODULE taskbarDll =
        GetModuleHandleW(L"taskbar.dll");

    if (!taskbarDll) {
        Wh_Log(
            L"[INPUT] taskbar.dll isn't loaded; "
            L"interaction suppression unavailable");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"
            },
            &g_taskGroupGetAppIdAddress,
            nullptr,
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskGroup::GetToolTipText(struct ITaskItem *,unsigned short *,int))"
            },
            &g_taskGroupGetToolTipTextOriginal,
            TaskGroup_GetToolTipText_Hook,
        },
        {
            {
                LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"
            },
            &g_taskBtnGroupGetGroupAddress,
            nullptr,
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"
            },
            &g_taskListWndHandleClickOriginal,
            TaskListWnd_HandleClick_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"
            },
            &g_taskListWndOnContextMenuOriginal,
            TaskListWnd_OnContextMenu_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl CTaskBtnGroup::ShouldShowToolTip(struct ITaskItem *))"
            },
            &g_taskBtnGroupShouldShowToolTipOriginal,
            TaskBtnGroup_ShouldShowToolTip_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            taskbarDll,
            hooks,
            ARRAYSIZE(hooks))) {
        Wh_Log(
            L"[INPUT] HookSymbols(taskbar.dll) failed");
        return false;
    }

    Wh_Log(
        L"[INPUT] Taskbar interaction hooks ready; "
        L"click=1 legacyContext=1 tooltip=1");

    return true;
}


// -----------------------------------------------------------------------------
// Shortcut creation.
// -----------------------------------------------------------------------------

static HRESULT SetShortcutAppId(
    IShellLinkW* shellLink,
    const std::wstring& appId) {
    IPropertyStore* propertyStore = nullptr;

    HRESULT hr = shellLink->QueryInterface(
        IID_PPV_ARGS(&propertyStore));

    if (FAILED(hr)) {
        return hr;
    }

    PROPVARIANT value;
    PropVariantInit(&value);

    hr = InitPropVariantFromString(
        appId.c_str(),
        &value);

    if (SUCCEEDED(hr)) {
        hr = propertyStore->SetValue(
            PKEY_AppUserModel_ID,
            value);
    }

    if (SUCCEEDED(hr)) {
        hr = propertyStore->Commit();
    }

    PropVariantClear(&value);
    propertyStore->Release();

    return hr;
}

static HRESULT CreateSeparatorShortcut(
    int ordinal,
    const std::wstring& shortcutPath) {
    IShellLinkW* shellLink = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_ShellLink,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&shellLink));

    if (FAILED(hr)) {
        return hr;
    }

    wchar_t systemDirectory[MAX_PATH] = {};

    UINT systemDirectoryLength =
        GetSystemDirectoryW(
            systemDirectory,
            ARRAYSIZE(systemDirectory));

    if (!systemDirectoryLength ||
        systemDirectoryLength >= ARRAYSIZE(systemDirectory)) {
        shellLink->Release();
        return HRESULT_FROM_WIN32(GetLastError());
    }

    std::wstring target =
        JoinPath(systemDirectory, L"cmd.exe");

    // If the raw test button is accidentally clicked before XAML styling is
    // added, launch a hidden cmd.exe that exits immediately.
    hr = shellLink->SetPath(target.c_str());

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetArguments(L"/d /c exit");
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetShowCmd(SW_HIDE);
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetIconLocation(
            g_iconPath.c_str(),
            0);
    }

    std::wstring description =
        GetSeparatorBaseName(ordinal);

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetDescription(
            description.c_str());
    }

    // Give every separator a distinct AppUserModelID so the Shell considers
    // them distinct taskbar identities even though they share cmd.exe as the
    // harmless launch target.
    if (SUCCEEDED(hr)) {
        hr = SetShortcutAppId(
            shellLink,
            GetSeparatorAppId(ordinal));
    }

    if (SUCCEEDED(hr)) {
        IPersistFile* persistFile = nullptr;

        hr = shellLink->QueryInterface(
            IID_PPV_ARGS(&persistFile));

        if (SUCCEEDED(hr)) {
            hr = persistFile->Save(
                shortcutPath.c_str(),
                TRUE);

            persistFile->Release();
        }
    }

    shellLink->Release();

    Wh_Log(
        L"[FILES] Create shortcut #%d '%s' hr=0x%08X",
        ordinal,
        shortcutPath.c_str(),
        static_cast<unsigned int>(hr));

    return hr;
}

// -----------------------------------------------------------------------------
// PIDL + PinManager helpers.
// -----------------------------------------------------------------------------

static HRESULT GetPidlForPath(
    const std::wstring& path,
    PIDLIST_ABSOLUTE* pidl) {
    *pidl = nullptr;

    SFGAOF attrs = 0;

    HRESULT hr = SHParseDisplayName(
        path.c_str(),
        nullptr,
        pidl,
        0,
        &attrs);

    Wh_Log(
        L"[PIDL] '%s' -> hr=0x%08X pidl=%p",
        path.c_str(),
        static_cast<unsigned int>(hr),
        *pidl);

    return hr;
}

static HRESULT CreatePinManager(
    IPinManagerInterop3** pinManager) {
    *pinManager = nullptr;

    IPinManagerInterop2* interop2 = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_PinManager,
        nullptr,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_IPinManagerInterop2,
        reinterpret_cast<void**>(&interop2));

    if (FAILED(hr)) {
        Wh_Log(
            L"[PIN] CoCreateInstance(IPinManagerInterop2) "
            L"failed hr=0x%08X",
            static_cast<unsigned int>(hr));
        return hr;
    }

    hr = interop2->QueryInterface(
        IID_IPinManagerInterop3,
        reinterpret_cast<void**>(pinManager));

    interop2->Release();

    Wh_Log(
        L"[PIN] QueryInterface(IPinManagerInterop3) "
        L"hr=0x%08X ptr=%p",
        static_cast<unsigned int>(hr),
        *pinManager);

    return hr;
}

// -----------------------------------------------------------------------------
// Creation / positioning.
// -----------------------------------------------------------------------------

static bool CreateAndPinSeparators() {
    if (g_settings.separators.empty()) {
        Wh_Log(L"[PIN] No separators configured");
        return true;
    }

    if (!WriteBinaryFile(
            g_iconPath,
            kSeparatorIcon,
            static_cast<DWORD>(sizeof(kSeparatorIcon)))) {
        return false;
    }

    // First create every backing shortcut, before mutating the taskbar.
    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        HRESULT hr =
            CreateSeparatorShortcut(
                separator.ordinal,
                shortcutPath);

        if (FAILED(hr)) {
            Wh_Log(
                L"[PIN] Failed to create separator #%d",
                separator.ordinal);
            return false;
        }
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);
    if (FAILED(hr) || !pinManager) {
        return false;
    }

    // Pin all shortcuts first. PinItemFromTrustedCaller is the modern working
    // path on this build; PinItemToTaskbarShim was observed to be a no-op.
    bool success = true;

    for (const auto& separator : g_settings.separators) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[PIN] Pin separator #%d",
            separator.ordinal);

        HRESULT pinHr =
            pinManager->PinItemFromTrustedCaller(
                pidl,
                PMC_TASKBANDPIN);

        Wh_Log(
            L"[PIN] PinItemFromTrustedCaller #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(pinHr));

        if (FAILED(pinHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    // Moving in ascending destination order makes multiple requested positions
    // deterministic as items are pulled forward from their initial appended
    // locations.
    std::vector<SeparatorSetting> moveOrder =
        g_settings.separators;

    std::stable_sort(
        moveOrder.begin(),
        moveOrder.end(),
        [](const SeparatorSetting& a, const SeparatorSetting& b) {
            return a.targetIndex < b.targetIndex;
        });

    for (const auto& separator : moveOrder) {
        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            success = false;
            continue;
        }

        Wh_Log(
            L"[MOVE] separator #%d -> index=%d",
            separator.ordinal,
            separator.targetIndex);

        HRESULT moveHr =
            pinManager->MoveTaskbarPin(
                pidl,
                separator.targetIndex,
                PMC_TASKBANDREORDER);

        Wh_Log(
            L"[MOVE] MoveTaskbarPin #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(moveHr));

        if (FAILED(moveHr)) {
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    pinManager->Release();

    return success;
}

// -----------------------------------------------------------------------------
// Destruction / cleanup.
// -----------------------------------------------------------------------------

static bool UnpinAndDeleteSeparators() {
    if (g_settings.separators.empty()) {
        DeleteFileIfPresent(g_iconPath);
        return true;
    }

    IPinManagerInterop3* pinManager = nullptr;

    HRESULT hr = CreatePinManager(&pinManager);

    if (FAILED(hr) || !pinManager) {
        Wh_Log(
            L"[CLEANUP] Can't acquire PinManager; "
            L"leaving backing files in place to avoid broken pins");
        return false;
    }

    bool allUnpinned = true;

    // Reverse order isn't required because unpinning is PIDL-based, but it
    // minimizes visual/index churn while several separators disappear.
    for (auto it = g_settings.separators.rbegin();
         it != g_settings.separators.rend();
         ++it) {
        const auto& separator = *it;

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator.ordinal);

        PIDLIST_ABSOLUTE pidl = nullptr;

        hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            Wh_Log(
                L"[CLEANUP] Can't resolve separator #%d; "
                L"keeping its shortcut",
                separator.ordinal);
            allUnpinned = false;
            continue;
        }

        Wh_Log(
            L"[CLEANUP] Unpin separator #%d",
            separator.ordinal);

        // 11 is the exact caller value observed during the native modern
        // Notepad unpin trace on this build, and was already tested with
        // UnpinTaskbarItem successfully.
        HRESULT unpinHr =
            pinManager->UnpinTaskbarItem(
                pidl,
                PMC_JUMPVIEWBROKER);

        Wh_Log(
            L"[CLEANUP] UnpinTaskbarItem #%d -> hr=0x%08X",
            separator.ordinal,
            static_cast<unsigned int>(unpinHr));

        CoTaskMemFree(pidl);

        if (SUCCEEDED(unpinHr)) {
            DeleteFileIfPresent(shortcutPath);
        } else {
            allUnpinned = false;
        }
    }

    pinManager->Release();

    if (allUnpinned) {
        DeleteFileIfPresent(g_iconPath);
    } else {
        Wh_Log(
            L"[CLEANUP] At least one unpin failed; "
            L"keeping shared icon '%s'",
            g_iconPath.c_str());
    }

    return allUnpinned;
}

// -----------------------------------------------------------------------------
// Windhawk lifetime.
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[INIT] Taskbar separator prototype MaxWidth test loading");

    LoadSettings();

    if (!InitializeStoragePath()) {
        return FALSE;
    }

    if (!InitializeTaskbarStylingHooks()) {
        Wh_Log(
            L"[INIT] Failed to initialize TaskListButton styling hooks");
        return FALSE;
    }

    if (!InitializeTaskbarInteractionHooks()) {
        Wh_Log(
            L"[INIT] Failed to initialize taskbar interaction hooks");
        return FALSE;
    }

    // Hook operations installed during Wh_ModInit are applied automatically
    // before Wh_ModAfterInit. Create the actual pins there so their first
    // UpdateVisualStates pass can already be intercepted.
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[INIT] Taskbar separator backend starting");

    // Close the small race where the module wasn't present in Wh_ModInit but
    // appeared before/around Wh_ModAfterInit.
    if (!g_taskbarViewDllHooked) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllHooked.exchange(true)) {
                if (HookTaskbarViewDllSymbols(module)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_taskbarViewDllHooked = false;
                }
            }
        }
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[INIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[INIT] CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(hrInit));
        return;
    }

    bool success =
        CreateAndPinSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    if (!success) {
        Wh_Log(
            L"[INIT] One or more prototype operations failed; "
            L"mod remains loaded so unload can still clean up");
    }

    Wh_Log(L"[INIT] Taskbar separator prototype loaded");
}

void Wh_ModBeforeUninit() {
    // Stop re-applying widths/interaction state while Windhawk is unwinding
    // hooks and while the backing taskbar items are about to be removed.
    //
    // IsDraggable is mutable state on the live XAML TaskListButton, so a tiny
    // standalone probe can leave it false after that probe is disabled. The
    // production mod owns these separator pins and unpins them in Wh_ModUninit,
    // which destroys the corresponding TaskListButtons and their property
    // state. Don't try to restore the XAML property here: this callback isn't
    // guaranteed to execute on the taskbar UI thread.
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L"[UNINIT] Taskbar separator prototype unloading");

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit == RPC_E_CHANGED_MODE) {
        Wh_Log(
            L"[UNINIT] COM already initialized with another "
            L"apartment model; continuing");
    } else if (FAILED(hrInit)) {
        Wh_Log(
            L"[UNINIT] CoInitializeEx failed hr=0x%08X; "
            L"can't safely remove separators",
            static_cast<unsigned int>(hrInit));
        return;
    }

    UnpinAndDeleteSeparators();

    if (shouldUninitialize) {
        CoUninitialize();
    }

    Wh_Log(L"[UNINIT] Taskbar separator prototype unloaded");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Keep the old loaded settings intact. The old instance must unpin/delete
    // exactly the separator identities it created; the reloaded instance then
    // reads the new settings and recreates fresh buttons. This is also how the
    // native-extent <-> MaxWidth compatibility mode switch stays clean: width
    // ownership is never hot-swapped on an existing TaskListButton.
    *bReload = TRUE;
    return TRUE;
}
