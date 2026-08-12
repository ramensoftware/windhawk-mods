// ==WindhawkMod==
// @id              taskbar-icon-separators
// @name            Taskbar Icon Separators
// @description     Create tracked icon separators with configurable padding on the taskbar.
// @version         0.5.22
// @author          meteoni
// @github          https://github.com/Meteony
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lpropsys -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Icon Separators

Create tracked icon separators with configurable padding on the taskbar.

![Screenshot](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/taskbar-icon-separators/SEP.png)
_Example for creating separators on the taskbar_

Another mod offers similar functionality, but this implementation takes a different approach.

This mod uses private COM APIs to insert a genuine taskbar button, styles its width and centering, and disables all interaction events. Because each separator occupies a real pinned slot, it stays anchored between the same pinned items as running apps change.

Note: The mod creates pinned shortcuts targeting the Windows `systray.exe` stub and reorders the persisted taskbar pin list. Cleanup is best-effort; in the worst failure case, separators may require manual unpinning.

Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- identifierPrefix: WindhawkSeparator
  $name: Separator Identifier Prefix
  $description: >-
    Customizable part of separator identities. A fixed unique suffix is always appended.
- maxWidthCompatibilityMode: false
  $name: Taskbar Icon Size Compatibility Mode
  $description: >-
    Only normal icon sizes take effect with this option enabled. 

    Compatibility mode for Taskbar height and icon size. 
    Sets MaxWidth property while keeping native taskbar icon sizes untouched to avoid collision. 
    
    Enable this if you observe crashes when used with that mod. 
- separators:
    - - index: 1
        $name: Final Position
        $description: 1 = first pinned taskbar position. Right after Search / Taskview. 
      - width: 20
        $name: Normal Width
        $description: Width for normal taskbar icon mode. 
      - widthSmall: 10
        $name: Small width
        $description: Width for small taskbar icon mode. 
  $name: Separators
  $description: Each new entry correspond to a new separator being created.
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
#include <cwctype>
#include <iterator>
#include <mutex>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
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
    std::wstring identity;
};

struct Settings {
    std::wstring identifierPrefix;
    bool maxWidthCompatibilityMode;
    std::vector<SeparatorSetting> separators;
};

static Settings g_settings;
static SeparatorSetting g_refreshPulseSetting;
static std::wstring g_storagePath;
static std::wstring g_iconPath;

static std::atomic<bool> g_taskbarViewDllHooked = false;
static std::atomic<bool> g_taskbarInteractionHooksInstalled = false;
static std::atomic<bool> g_loadLibraryWatcherInstalled = false;
static std::atomic<bool> g_unloading = false;
static std::atomic<bool> g_afterInit = false;
static std::mutex g_lifecycleMutex;
static bool g_taskbarViewHookInstallationAttempted = false;
static bool g_taskbarInteractionHookInstallationAttempted = false;
static bool g_backendStopped = false;
static HANDLE g_backendThread = nullptr;
static HANDLE g_backendStopEvent = nullptr;
static thread_local HANDLE g_currentBackendStopEvent = nullptr;

static constexpr wchar_t kSeparatorIdentitySuffix[] = L"8F31A7D2";

// Reserved outside the supported separator range. The helper is briefly
// pinned and unpinned after positioning to make Explorer process a concrete
// pin-list mutation and reconcile stale taskbar visuals.
static constexpr int kRefreshPulseOrdinal = 100001;

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

static std::wstring SanitizeIdentifierPrefix(std::wstring value) {
    for (wchar_t& ch : value) {
        const bool allowed =
            (ch >= L'a' && ch <= L'z') ||
            (ch >= L'A' && ch <= L'Z') ||
            (ch >= L'0' && ch <= L'9') ||
            ch == L'-' || ch == L'_';
        if (!allowed) {
            ch = L'_';
        }
    }

    if (value.empty()) {
        value = L"WindhawkSeparator";
    }

    constexpr size_t kMaxPrefixLength = 48;
    if (value.size() > kMaxPrefixLength) {
        value.resize(kMaxPrefixLength);
    }

    return value;
}

static std::wstring BuildSeparatorIdentity(
    std::wstring_view prefix,
    int ordinal) {
    return std::wstring(prefix) +
           L"." +
           kSeparatorIdentitySuffix +
           L"." +
           std::to_wstring(ordinal);
}

static std::wstring GetSeparatorShortcutPath(
    const SeparatorSetting& separator) {
    return JoinPath(
        g_storagePath,
        separator.identity + L".lnk");
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

    g_settings.identifierPrefix = SanitizeIdentifierPrefix(
        WindhawkUtils::StringSetting::make(
            L"identifierPrefix").get());

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

        g_settings.separators.push_back({
            .ordinal = i + 1,
            .targetIndex = position - 1,
            .width = static_cast<double>(width),
            .widthSmall = static_cast<double>(widthSmall),
            .identity = BuildSeparatorIdentity(
                g_settings.identifierPrefix,
                i + 1),
        });
    }

    g_refreshPulseSetting = {
        .ordinal = kRefreshPulseOrdinal,
        .targetIndex = 0,
        .width = g_settings.separators.empty()
                     ? 20.0
                     : g_settings.separators.front().width,
        .widthSmall = g_settings.separators.empty()
                          ? 10.0
                          : g_settings.separators.front().widthSmall,
        .identity = BuildSeparatorIdentity(
            g_settings.identifierPrefix,
            kRefreshPulseOrdinal),
    };

    Wh_Log(
        L"[SETTINGS] prefix='%s' separators=%zu widthMode=%s",
        g_settings.identifierPrefix.c_str(),
        g_settings.separators.size(),
        g_settings.maxWidthCompatibilityMode
            ? L"MaxWidthCompatibility"
            : L"NativeExtent");

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
            L"separator.ico");

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

static bool ContainsIdentityToken(
    std::wstring_view text,
    std::wstring_view identity) {
    size_t position = text.find(identity);
    while (position != std::wstring_view::npos) {
        const size_t end = position + identity.size();
        const auto isIdentityCharacter = [](wchar_t ch) {
            return std::iswalnum(ch) ||
                   ch == L'_' || ch == L'-' || ch == L'.';
        };

        const bool validStart =
            position == 0 ||
            !isIdentityCharacter(text[position - 1]);
        const bool validEnd =
            end == text.size() ||
            !isIdentityCharacter(text[end]);

        if (validStart && validEnd) {
            return true;
        }

        position = text.find(identity, position + 1);
    }

    return false;
}

static const SeparatorSetting* FindSeparatorByAutomationName(
    std::wstring_view name) {
    if (name.empty()) {
        return nullptr;
    }

    for (const auto& separator : g_settings.separators) {
        if (ContainsIdentityToken(name, separator.identity)) {
            return &separator;
        }
    }

    return ContainsIdentityToken(
               name,
               g_refreshPulseSetting.identity)
               ? &g_refreshPulseSetting
               : nullptr;
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

#if defined(_M_X64)
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

        return (foundOffset < 0 || foundOffset > 0xFFFF)
                   ? 0
                   : foundOffset;
#elif defined(_M_ARM64)
        // ARM64 logic from the upstream Taskbar height and icon size mod.
        // Find two double loads from the same object which feed an fsub; the
        // first load is the per-instance medium extent.
        const DWORD* start =
            reinterpret_cast<const DWORD*>(
                g_taskListButtonUpdateIconColumnDefinitionAddress);
        const DWORD* end = start + 0x80;

        std::regex loadWithOffset(
            R"(ldr\s+(d\d+), \[(x\d+), #0x([0-9a-f]+)\])");
        std::regex otherLoad(R"(ldr\s+(d\d+),.*)");
        std::regex subtract(
            R"(fsub\s+d\d+, (d\d+), (d\d+))");

        struct LoadInstruction {
            std::string destinationRegister;
            std::string sourceRegister;
            LONG offset;
        } loads[32];
        size_t loadCount = 0;

        for (const DWORD* instruction = start;
             instruction != end;
             instruction++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm(
                    const_cast<DWORD*>(instruction),
                    &result)) {
                break;
            }

            std::string_view text = result.text;
            if (text == "ret") {
                break;
            }

            if (loadCount == ARRAYSIZE(loads)) {
                break;
            }

            std::match_results<std::string_view::const_iterator> match;
            if (std::regex_match(
                    text.begin(),
                    text.end(),
                    match,
                    loadWithOffset)) {
                loads[loadCount++] = {
                    .destinationRegister = match[1].str(),
                    .sourceRegister = match[2].str(),
                    .offset = static_cast<LONG>(
                        std::stoul(match[3].str(), nullptr, 16)),
                };
                continue;
            }

            if (std::regex_match(
                    text.begin(),
                    text.end(),
                    match,
                    otherLoad)) {
                loads[loadCount++] = {
                    .destinationRegister = match[1].str(),
                    .sourceRegister = {},
                    .offset = 0,
                };
                continue;
            }

            if (!std::regex_match(
                    text.begin(),
                    text.end(),
                    match,
                    subtract)) {
                continue;
            }

            const std::string firstInput = match[1].str();
            const std::string secondInput = match[2].str();
            const LoadInstruction* firstLoad = nullptr;
            const LoadInstruction* secondLoad = nullptr;

            for (size_t i = loadCount; i > 0; i--) {
                const LoadInstruction& load = loads[i - 1];
                if (!firstLoad &&
                    load.destinationRegister == firstInput) {
                    firstLoad = &load;
                }
                if (!secondLoad &&
                    load.destinationRegister == secondInput) {
                    secondLoad = &load;
                }
            }

            if (firstLoad && secondLoad &&
                !firstLoad->sourceRegister.empty() &&
                firstLoad->sourceRegister == secondLoad->sourceRegister) {
                LONG foundOffset = firstLoad->offset;
                return (foundOffset < 0 || foundOffset > 0xFFFF)
                           ? 0
                           : foundOffset;
            }
        }

        return 0;
#else
#error "Unsupported architecture"
#endif
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

struct SeparatorVisualState {
    winrt::weak_ref<FrameworkElement> element;
    void* taskListButton;

    bool mediumExtentCaptured = false;
    double originalMediumExtent = 0;
    bool smallExtentCaptured = false;
    double originalSmallExtent = 0;

    bool maxWidthCaptured = false;
    winrt::Windows::Foundation::IInspectable originalMaxWidthLocalValue{
        nullptr};

    winrt::weak_ref<FrameworkElement> iconPanel;
    bool iconPanelAlignmentCaptured = false;
    winrt::Windows::Foundation::IInspectable
        originalIconPanelAlignmentLocalValue{nullptr};
    bool iconPanelToolTipCaptured = false;
    winrt::Windows::Foundation::IInspectable
        originalIconPanelToolTipLocalValue{nullptr};

    winrt::weak_ref<FrameworkElement> icon;
    bool iconAlignmentCaptured = false;
    winrt::Windows::Foundation::IInspectable
        originalIconAlignmentLocalValue{nullptr};

    winrt::weak_ref<FrameworkElement> backgroundElement;
    bool backgroundOpacityCaptured = false;
    winrt::Windows::Foundation::IInspectable
        originalBackgroundOpacityLocalValue{nullptr};

    bool taskListButtonToolTipCaptured = false;
    winrt::Windows::Foundation::IInspectable
        originalTaskListButtonToolTipLocalValue{nullptr};

    bool isDraggableCaptured = false;
    bool originalIsDraggable = false;
};

static thread_local std::vector<SeparatorVisualState>
    g_separatorVisualStates;

static void PruneExpiredSeparatorVisualStates() {
    g_separatorVisualStates.erase(
        std::remove_if(
            g_separatorVisualStates.begin(),
            g_separatorVisualStates.end(),
            [](const SeparatorVisualState& state) {
                return !state.element.get();
            }),
        g_separatorVisualStates.end());
}

static auto FindSeparatorVisualState(void* taskListButton) {
    return std::find_if(
        g_separatorVisualStates.begin(),
        g_separatorVisualStates.end(),
        [taskListButton](const SeparatorVisualState& state) {
            return state.taskListButton == taskListButton;
        });
}

static bool ApplySeparatorWidthOverride(
    SeparatorVisualState& state,
    const SeparatorSetting& separator) {
    if (!g_taskListButtonUpdateIconColumnDefinitionAddress) {
        return false;
    }

    LONG mediumOffset = GetMediumTaskbarButtonExtentOffset();
    if (!mediumOffset) {
        return false;
    }

    auto* mediumExtent =
        reinterpret_cast<double*>(
            reinterpret_cast<BYTE*>(state.taskListButton) + mediumOffset);
    bool changed = false;

    if (*mediumExtent < 1 || *mediumExtent >= 10000) {
        return false;
    }

    if (!state.mediumExtentCaptured) {
        state.originalMediumExtent = *mediumExtent;
        state.mediumExtentCaptured = true;
    }

    if (*mediumExtent != separator.width) {
        *mediumExtent = separator.width;
        changed = true;
    }

    // On DynamicIconScaling builds the small extent immediately precedes
    // the medium extent. Don't assume that layout on older taskbars.
    if (g_hasDynamicIconScaling) {
        double* smallExtent = mediumExtent - 1;

        if (*smallExtent >= 1 && *smallExtent < 10000) {
            if (!state.smallExtentCaptured) {
                state.originalSmallExtent = *smallExtent;
                state.smallExtentCaptured = true;
            }

            if (*smallExtent != separator.widthSmall) {
                *smallExtent = separator.widthSmall;
                changed = true;
            }
        }
    }

    if (changed) {
        RefreshTaskListButtonLayout(state.taskListButton);
    }

    return true;
}

static void ApplySeparatorMaxWidthOverride(
    SeparatorVisualState& state,
    const FrameworkElement& element,
    const SeparatorSetting& separator) {
    if (!state.maxWidthCaptured) {
        try {
            state.originalMaxWidthLocalValue =
                element.ReadLocalValue(
                    FrameworkElement::MaxWidthProperty());
            state.maxWidthCaptured = true;
        } catch (...) {
            return;
        }
    }

    try {
        if (element.MaxWidth() != separator.width) {
            element.MaxWidth(separator.width);
        }
    } catch (...) {
        // Treat taskbar reconstruction as a transient miss.
    }
}

static void CenterSeparatorIcon(
    SeparatorVisualState& state,
    const FrameworkElement& taskListButton) {
    if (!taskListButton) {
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

        auto trackedIconPanel = state.iconPanel.get();
        if (!trackedIconPanel ||
            winrt::get_abi(trackedIconPanel) != winrt::get_abi(iconPanel)) {
            state.iconPanel = winrt::make_weak(iconPanel);
            state.originalIconPanelAlignmentLocalValue =
                iconPanel.ReadLocalValue(
                    FrameworkElement::HorizontalAlignmentProperty());
            state.iconPanelAlignmentCaptured = true;
            state.originalIconPanelToolTipLocalValue =
                iconPanel.ReadLocalValue(
                    winrt::Windows::UI::Xaml::Controls::
                        ToolTipService::ToolTipProperty());
            state.iconPanelToolTipCaptured = true;
        }

        iconPanel.HorizontalAlignment(
            HorizontalAlignment::Center);

        if (auto icon =
                FindChildByName(
                    iconPanel,
                    L"Icon")) {
            auto trackedIcon = state.icon.get();
            if (!trackedIcon ||
                winrt::get_abi(trackedIcon) != winrt::get_abi(icon)) {
                state.icon = winrt::make_weak(icon);
                state.originalIconAlignmentLocalValue =
                    icon.ReadLocalValue(
                        FrameworkElement::HorizontalAlignmentProperty());
                state.iconAlignmentCaptured = true;
            }

            icon.HorizontalAlignment(
                HorizontalAlignment::Center);
        }
    } catch (...) {
        // Cosmetic only. Leave the stock layout intact on a future template.
    }
}

static void SuppressSeparatorHoverChrome(
    SeparatorVisualState& state,
    const FrameworkElement& taskListButton) {
    if (!taskListButton) {
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
                auto trackedBackground = state.backgroundElement.get();
                if (!trackedBackground ||
                    winrt::get_abi(trackedBackground) !=
                        winrt::get_abi(backgroundElement)) {
                    state.backgroundElement =
                        winrt::make_weak(backgroundElement);
                    state.originalBackgroundOpacityLocalValue =
                        backgroundElement.ReadLocalValue(
                            UIElement::OpacityProperty());
                    state.backgroundOpacityCaptured = true;
                }

                backgroundElement.Opacity(0.0);
            }
        }

        // Clear XAML-attached tooltip content as a presentation-layer fallback.
        if (!state.taskListButtonToolTipCaptured) {
            state.originalTaskListButtonToolTipLocalValue =
                taskListButton.ReadLocalValue(
                    winrt::Windows::UI::Xaml::Controls::
                        ToolTipService::ToolTipProperty());
            state.taskListButtonToolTipCaptured = true;
        }

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
    SeparatorVisualState& state,
    const FrameworkElement& taskListButton) {
    if (!taskListButton ||
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

        bool isDraggable =
            winrt::unbox_value<bool>(beforeBox);

        if (!state.isDraggableCaptured) {
            state.originalIsDraggable = isDraggable;
            state.isDraggableCaptured = true;
        }

        if (isDraggable) {
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

static void RestoreDependencyPropertyLocalValue(
    const DependencyObject& object,
    const DependencyProperty& property,
    const winrt::Windows::Foundation::IInspectable& originalLocalValue) {
    if (originalLocalValue == DependencyProperty::UnsetValue()) {
        object.ClearValue(property);
    } else {
        object.SetValue(property, originalLocalValue);
    }
}

static void RestoreSeparatorVisualState(
    std::vector<SeparatorVisualState>::iterator stateIt) {
    SeparatorVisualState& state = *stateIt;
    auto element = state.element.get();

    // The FrameworkElement weak reference is the lifetime authority for the
    // raw TaskListButton implementation pointer stored alongside it.
    if (!element) {
        g_separatorVisualStates.erase(stateIt);
        return;
    }

    bool extentChanged = false;

    LONG mediumOffset =
        (state.taskListButton &&
         (state.mediumExtentCaptured || state.smallExtentCaptured))
            ? GetMediumTaskbarButtonExtentOffset()
            : 0;
    if (mediumOffset) {
        auto* mediumExtent =
            reinterpret_cast<double*>(
                reinterpret_cast<BYTE*>(state.taskListButton) + mediumOffset);

        if (state.mediumExtentCaptured &&
            *mediumExtent != state.originalMediumExtent) {
            *mediumExtent = state.originalMediumExtent;
            extentChanged = true;
        }

        if (state.smallExtentCaptured) {
            double* smallExtent = mediumExtent - 1;
            if (*smallExtent != state.originalSmallExtent) {
                *smallExtent = state.originalSmallExtent;
                extentChanged = true;
            }
        }
    }

    try {
        if (state.maxWidthCaptured) {
            RestoreDependencyPropertyLocalValue(
                element,
                FrameworkElement::MaxWidthProperty(),
                state.originalMaxWidthLocalValue);
        }
    } catch (...) {
    }

    try {
        if (state.taskListButtonToolTipCaptured) {
            RestoreDependencyPropertyLocalValue(
                element,
                winrt::Windows::UI::Xaml::Controls::
                    ToolTipService::ToolTipProperty(),
                state.originalTaskListButtonToolTipLocalValue);
        }
    } catch (...) {
    }

    try {
        if (state.isDraggableCaptured &&
            g_taskListButtonSetIsDraggable &&
            !g_settingSeparatorIsDraggable) {
            g_settingSeparatorIsDraggable = true;
            auto target = element.as<
                winrt::Windows::Foundation::IInspectable>();
            g_taskListButtonSetIsDraggable(
                target,
                winrt::box_value(state.originalIsDraggable));
            g_settingSeparatorIsDraggable = false;
        }
    } catch (...) {
        g_settingSeparatorIsDraggable = false;
    }

    if (auto iconPanel = state.iconPanel.get()) {
        try {
            if (state.iconPanelAlignmentCaptured) {
                RestoreDependencyPropertyLocalValue(
                    iconPanel,
                    FrameworkElement::HorizontalAlignmentProperty(),
                    state.originalIconPanelAlignmentLocalValue);
            }
            if (state.iconPanelToolTipCaptured) {
                RestoreDependencyPropertyLocalValue(
                    iconPanel,
                    winrt::Windows::UI::Xaml::Controls::
                        ToolTipService::ToolTipProperty(),
                    state.originalIconPanelToolTipLocalValue);
            }
        } catch (...) {
        }
    }

    if (auto icon = state.icon.get()) {
        try {
            if (state.iconAlignmentCaptured) {
                RestoreDependencyPropertyLocalValue(
                    icon,
                    FrameworkElement::HorizontalAlignmentProperty(),
                    state.originalIconAlignmentLocalValue);
            }
        } catch (...) {
        }
    }

    if (auto backgroundElement = state.backgroundElement.get()) {
        try {
            if (state.backgroundOpacityCaptured) {
                RestoreDependencyPropertyLocalValue(
                    backgroundElement,
                    UIElement::OpacityProperty(),
                    state.originalBackgroundOpacityLocalValue);
            }
        } catch (...) {
        }
    }

    if (extentChanged) {
        RefreshTaskListButtonLayout(state.taskListButton);
    }

    g_separatorVisualStates.erase(stateIt);
}

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

struct RunFromWindowThreadParameters {
    RunFromWindowThreadProc procedure;
    void* parameter;
    bool executed;
};

static UINT GetRunFromWindowThreadMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

static LRESULT CALLBACK RunFromWindowThreadHook(
    int code,
    WPARAM wParam,
    LPARAM lParam) {
    if (code == HC_ACTION) {
        const auto* windowMessage =
            reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (windowMessage->message == GetRunFromWindowThreadMessage() &&
            windowMessage->lParam) {
            auto* parameters =
                reinterpret_cast<RunFromWindowThreadParameters*>(
                    windowMessage->lParam);
            if (!parameters->executed) {
                parameters->executed = true;
                parameters->procedure(parameters->parameter);
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static bool RunFromWindowThread(
    HWND window,
    RunFromWindowThreadProc procedure,
    void* parameter) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    UINT message = GetRunFromWindowThreadMessage();
    if (!threadId || !message) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        procedure(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        RunFromWindowThreadHook,
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParameters parameters = {
        procedure,
        parameter,
        false,
    };
    DWORD_PTR result = 0;
    SendMessageTimeoutW(
        window,
        message,
        0,
        reinterpret_cast<LPARAM>(&parameters),
        SMTO_ABORTIFHUNG,
        2000,
        &result);
    UnhookWindowsHookEx(hook);
    return parameters.executed;
}

static void WINAPI RestoreSeparatorVisualStatesOnCurrentThread(void*) {
    PruneExpiredSeparatorVisualStates();

    while (!g_separatorVisualStates.empty()) {
        RestoreSeparatorVisualState(g_separatorVisualStates.begin());
    }
}

static void RestoreTrackedSeparatorVisualStates() {
    struct EnumContext {
        std::vector<DWORD> processedThreadIds;
    } context;

    EnumWindows(
        [](HWND window, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            DWORD threadId =
                GetWindowThreadProcessId(window, &processId);
            if (!threadId || processId != GetCurrentProcessId()) {
                return TRUE;
            }

            wchar_t className[64] = {};
            if (!GetClassNameW(
                    window,
                    className,
                    ARRAYSIZE(className)) ||
                (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
                 _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0)) {
                return TRUE;
            }

            auto* context = reinterpret_cast<EnumContext*>(lParam);
            if (std::find(
                    context->processedThreadIds.begin(),
                    context->processedThreadIds.end(),
                    threadId) != context->processedThreadIds.end()) {
                return TRUE;
            }

            context->processedThreadIds.push_back(threadId);
            if (!RunFromWindowThread(
                    window,
                    RestoreSeparatorVisualStatesOnCurrentThread,
                    nullptr)) {
                Wh_Log(
                    L"[UNINIT] Failed to restore taskbar visuals on thread %u",
                    threadId);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
}

// Modern WinUI taskbar context-menu paths.
//
// These hooks cover the context-request routes seen across Taskbar.View builds.
// They are filtered independently and don't touch generic pointer input.
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

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(
        void* pThis,
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskbarResources_OnTaskListButtonContextRequested_t
    g_taskbarResourcesOnTaskListButtonContextRequestedOriginal;

static void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis,
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    if (!g_unloading) {
        auto element =
            sender.try_as<FrameworkElement>();

        if (element &&
            GetSeparatorForElement(element)) {
            // Don't set args.Handled here. That path is crash-prone on some
            // Taskbar.View builds; skipping the handler is sufficient.
            return;
        }
    }

    g_taskbarResourcesOnTaskListButtonContextRequestedOriginal(
        pThis,
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

    PruneExpiredSeparatorVisualStates();
    auto stateIt = FindSeparatorVisualState(pThis);

    // ItemsRepeater recycles TaskListButton containers. Undo every property
    // owned by this mod as soon as a container stops representing a separator.
    if (!separator || !element || g_unloading) {
        if (stateIt != g_separatorVisualStates.end()) {
            RestoreSeparatorVisualState(stateIt);
        }
        return;
    }

    if (stateIt == g_separatorVisualStates.end()) {
        g_separatorVisualStates.push_back({
            .element = winrt::make_weak(element),
            .taskListButton = pThis,
        });
        stateIt = std::prev(g_separatorVisualStates.end());
    }

    SeparatorVisualState& state = *stateIt;

    if (g_settings.maxWidthCompatibilityMode) {
        // Compatibility path: native/TIS extents remain untouched.
        ApplySeparatorMaxWidthOverride(
            state,
            element,
            *separator);
    } else {
        // Default path: original per-instance medium/small extent override.
        if (!ApplySeparatorWidthOverride(
                state,
                *separator)) {
            // A future build can retain the symbol while changing the object
            // layout. Keep the separator usable with the public XAML fallback.
            ApplySeparatorMaxWidthOverride(
                state,
                element,
                *separator);
        }
    }

    CenterSeparatorIcon(state, element);
    SuppressSeparatorHoverChrome(state, element);
    DisableSeparatorDragging(state, element);
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

static bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
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
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskbarResourcesOnTaskListButtonContextRequestedOriginal,
            TaskbarResources_OnTaskListButtonContextRequested_Hook,
            true,
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
            taskbarViewHooks,
            ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"[STYLE] HookSymbols(Taskbar view) failed");
        return false;
    }

    constexpr UINT kDynamicIconScaling = 29785184;

    g_hasDynamicIconScaling =
        g_taskbarConfigurationGetIconHeightInViewPixelsAddress &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true);

    if (!g_settings.maxWidthCompatibilityMode) {
        // Native object layout is private and can change independently of the
        // symbols. Fall back to MaxWidth instead of aborting or pinning an
        // unstyled full-width button.
        if (!g_taskListButtonUpdateButtonPaddingAddress ||
            !g_taskListButtonUpdateIconColumnDefinitionAddress ||
            !GetMediumTaskbarButtonExtentOffset()) {
            Wh_Log(
                L"[STYLE] Native-extent resolution unavailable; "
                L"using MaxWidth fallback");
            g_settings.maxWidthCompatibilityMode = true;
        }
    }

    Wh_Log(
        L"[STYLE] Taskbar view hooks installed; widthMode=%s",
        g_settings.maxWidthCompatibilityMode
            ? L"MaxWidthCompatibility"
            : L"NativeExtent");

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_loadLibraryExWOriginal;

static bool HookTaskbarInteractionSymbols(HMODULE taskbarDll);
static void TryStartBackendWorker();

static bool InstallTaskbarViewHooks(
    HMODULE module,
    bool applyOperations) {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (g_unloading) {
        return false;
    }

    if (g_taskbarViewDllHooked) {
        return true;
    }

    if (g_taskbarViewHookInstallationAttempted) {
        return false;
    }

    g_taskbarViewHookInstallationAttempted = true;
    if (!HookTaskbarViewDllSymbols(module)) {
        return false;
    }

    if (applyOperations && !Wh_ApplyHookOperations()) {
        Wh_Log(L"[STYLE] Wh_ApplyHookOperations failed");
        return false;
    }

    g_taskbarViewDllHooked = true;
    return true;
}

static bool InstallTaskbarInteractionHooks(
    HMODULE module,
    bool applyOperations) {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (g_unloading) {
        return false;
    }

    if (g_taskbarInteractionHooksInstalled) {
        return true;
    }

    if (g_taskbarInteractionHookInstallationAttempted) {
        return false;
    }

    g_taskbarInteractionHookInstallationAttempted = true;
    if (!HookTaskbarInteractionSymbols(module)) {
        return false;
    }

    if (applyOperations && !Wh_ApplyHookOperations()) {
        Wh_Log(L"[INPUT] Wh_ApplyHookOperations failed");
        return false;
    }

    g_taskbarInteractionHooksInstalled = true;
    return true;
}

static HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags) {
    HMODULE module =
        g_loadLibraryExWOriginal(
            lpLibFileName,
            hFile,
            dwFlags);

    if (!module || g_unloading) {
        return module;
    }

    if (!g_taskbarViewDllHooked &&
        GetTaskbarViewModuleHandle() == module) {
        Wh_Log(
            L"[STYLE] Taskbar view module loaded: %s",
            lpLibFileName ? lpLibFileName : L"<unknown>");

        if (InstallTaskbarViewHooks(module, true)) {
            TryStartBackendWorker();
        }
    }

    if (!g_taskbarInteractionHooksInstalled &&
        GetModuleHandleW(L"taskbar.dll") == module) {
        Wh_Log(L"[INPUT] taskbar.dll loaded; installing interaction hooks");

        if (InstallTaskbarInteractionHooks(module, true)) {
            TryStartBackendWorker();
        }
    }

    return module;
}

static bool InstallLoadLibraryWatcher() {
    if (g_loadLibraryWatcherInstalled.exchange(true)) {
        return true;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        g_loadLibraryWatcherInstalled = false;
        return false;
    }

    auto loadLibraryExW =
        reinterpret_cast<decltype(&LoadLibraryExW)>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));

    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExW_Hook,
            &g_loadLibraryExWOriginal)) {
        g_loadLibraryWatcherInstalled = false;
        return false;
    }

    return true;
}

static bool InitializeTaskbarStylingHooks() {
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        return InstallTaskbarViewHooks(module, false);
    }

    Wh_Log(
        L"[STYLE] Taskbar view module not loaded yet; "
        L"installing LoadLibraryExW watcher");

    return InstallLoadLibraryWatcher();
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
    for (const auto& separator : g_settings.separators) {
        if (appIdView == std::wstring_view(separator.identity)) {
            return true;
        }
    }

    return appIdView ==
        std::wstring_view(g_refreshPulseSetting.identity);
}

static HRESULT WINAPI TaskListWnd_HandleClick_Hook(
    void* pThis,
    void* taskGroup,
    void* taskItem,
    const void* launcherOptions) {
    if (!g_unloading &&
        IsSeparatorTaskGroup(taskGroup)) {
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

static bool HookTaskbarInteractionSymbols(HMODULE taskbarDll) {
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
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
            true,
        },
        {
            {
                LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"
            },
            &g_taskBtnGroupGetGroupAddress,
            nullptr,
            true,
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
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            taskbarDll,
            taskbarDllHooks,
            ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(
            L"[INPUT] HookSymbols(taskbar.dll) failed");
        return false;
    }

    Wh_Log(L"[INPUT] Taskbar interaction hooks installed");

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
    const SeparatorSetting& separator,
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
        JoinPath(systemDirectory, L"systray.exe");

    // systray.exe is an inert Windows stub, so a stranded shortcut doesn't
    // launch a console or perform an action if it is clicked.
    hr = shellLink->SetPath(target.c_str());

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetShowCmd(SW_HIDE);
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetIconLocation(
            g_iconPath.c_str(),
            0);
    }

    if (SUCCEEDED(hr)) {
        hr = shellLink->SetDescription(
            separator.identity.c_str());
    }

    // Give every separator a distinct AppUserModelID so the Shell considers
    // them distinct taskbar identities even though they share the same inert
    // launch target.
    if (SUCCEEDED(hr)) {
        hr = SetShortcutAppId(
            shellLink,
            separator.identity);
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

    if (FAILED(hr)) {
        Wh_Log(
            L"[PIN] QueryInterface(IPinManagerInterop3) "
            L"failed hr=0x%08X",
            static_cast<unsigned int>(hr));
    }

    return hr;
}

// -----------------------------------------------------------------------------
// Creation / positioning.
// -----------------------------------------------------------------------------

static bool PrepareSeparatorFiles() {
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
            GetSeparatorShortcutPath(separator);

        HRESULT hr =
            CreateSeparatorShortcut(
                separator,
                shortcutPath);

        if (FAILED(hr)) {
            Wh_Log(
                L"[PIN] Failed to create separator #%d hr=0x%08X",
                separator.ordinal,
                static_cast<unsigned int>(hr));
            return false;
        }
    }

    std::wstring refreshShortcutPath =
        GetSeparatorShortcutPath(g_refreshPulseSetting);
    HRESULT refreshHr =
        CreateSeparatorShortcut(
            g_refreshPulseSetting,
            refreshShortcutPath);

    if (FAILED(refreshHr)) {
        Wh_Log(
            L"[REFRESH] Failed to create pin-pulse shortcut hr=0x%08X",
            static_cast<unsigned int>(refreshHr));
        return false;
    }

    return true;
}

static bool InitializeTaskbarInteractionHooks() {
    if (HMODULE taskbarDll = GetModuleHandleW(L"taskbar.dll")) {
        return InstallTaskbarInteractionHooks(taskbarDll, false);
    }

    Wh_Log(
        L"[INPUT] taskbar.dll isn't loaded yet; "
        L"installing LoadLibraryExW watcher");
    return InstallLoadLibraryWatcher();
}

static bool IsBackendStopRequested() {
    return g_currentBackendStopEvent &&
        WaitForSingleObject(
            g_currentBackendStopEvent,
            0) == WAIT_OBJECT_0;
}

static bool WaitForBackendStop(DWORD timeout) {
    return g_currentBackendStopEvent &&
        WaitForSingleObject(
            g_currentBackendStopEvent,
            timeout) == WAIT_OBJECT_0;
}

static bool PinSeparators(IPinManagerInterop3* pinManager) {
    bool success = true;

    // Complete the entire pin phase before positioning anything. Retrying this
    // operation is safe because each shortcut has a stable path and AppUserModelID.
    for (const auto& separator : g_settings.separators) {
        if (IsBackendStopRequested()) {
            return false;
        }

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator);

        PIDLIST_ABSOLUTE pidl = nullptr;

        HRESULT hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            Wh_Log(
                L"[PIN] Can't resolve separator #%d hr=0x%08X",
                separator.ordinal,
                static_cast<unsigned int>(hr));
            success = false;
            continue;
        }

        HRESULT pinHr =
            pinManager->PinItemFromTrustedCaller(
                pidl,
                PMC_TASKBANDPIN);

        if (FAILED(pinHr)) {
            Wh_Log(
                L"[PIN] Failed to pin separator #%d hr=0x%08X",
                separator.ordinal,
                static_cast<unsigned int>(pinHr));
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    return success;
}

static bool PositionSeparators(IPinManagerInterop3* pinManager) {
    bool success = true;

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
        if (IsBackendStopRequested()) {
            return false;
        }

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator);

        PIDLIST_ABSOLUTE pidl = nullptr;

        HRESULT hr = GetPidlForPath(
            shortcutPath,
            &pidl);

        if (FAILED(hr) || !pidl) {
            Wh_Log(
                L"[MOVE] Can't resolve separator #%d hr=0x%08X",
                separator.ordinal,
                static_cast<unsigned int>(hr));
            success = false;
            continue;
        }

        HRESULT moveHr =
            pinManager->MoveTaskbarPin(
                pidl,
                separator.targetIndex,
                PMC_TASKBANDREORDER);

        if (FAILED(moveHr)) {
            Wh_Log(
                L"[MOVE] Failed to move separator #%d to index=%d "
                L"hr=0x%08X",
                separator.ordinal,
                separator.targetIndex,
                static_cast<unsigned int>(moveHr));
            success = false;
        }

        CoTaskMemFree(pidl);
    }

    return success;
}

static bool PulseTaskbarPinList(IPinManagerInterop3* pinManager) {
    if (IsBackendStopRequested()) {
        return false;
    }

    std::wstring shortcutPath =
        GetSeparatorShortcutPath(g_refreshPulseSetting);

    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = GetPidlForPath(shortcutPath, &pidl);

    if (FAILED(hr) || !pidl) {
        Wh_Log(
            L"[REFRESH] Can't resolve pin-pulse shortcut hr=0x%08X",
            static_cast<unsigned int>(hr));
        return false;
    }

    HRESULT pinHr =
        pinManager->PinItemFromTrustedCaller(
            pidl,
            PMC_TASKBANDPIN);

    if (FAILED(pinHr)) {
        Wh_Log(
            L"[REFRESH] Failed to pin transient refresh item hr=0x%08X",
            static_cast<unsigned int>(pinHr));
        CoTaskMemFree(pidl);
        return false;
    }

    // Once the helper is pinned, always attempt the matching unpin even if an
    // unload was requested in between. Wh_ModBeforeUninit joins this worker
    // before general cleanup, so completing the pair can't race shutdown.
    HRESULT unpinHr =
        pinManager->UnpinTaskbarItem(
            pidl,
            PMC_JUMPVIEWBROKER);

    CoTaskMemFree(pidl);

    if (SUCCEEDED(unpinHr)) {
        return DeleteFileIfPresent(shortcutPath);
    }

    Wh_Log(
        L"[REFRESH] Failed to unpin transient refresh item hr=0x%08X",
        static_cast<unsigned int>(unpinHr));

    // Keep the shortcut when unpinning fails so a later retry or unload can
    // resolve the same PIDL and remove the helper safely.
    return false;
}

static bool CreateAndPinSeparators() {
    if (!PrepareSeparatorFiles()) {
        return false;
    }

    if (g_settings.separators.empty()) {
        return true;
    }

    constexpr int kStartupAttempts = 5;
    constexpr DWORD kInitialStartupRetryDelay = 250;
    constexpr DWORD kStartupRetryBackoff = 2;
    DWORD retryDelay = kInitialStartupRetryDelay;

    for (int attempt = 1;
         attempt <= kStartupAttempts;
         ++attempt) {
        if (IsBackendStopRequested()) {
            Wh_Log(L"[INIT] Separator startup cancelled");
            return false;
        }

        Wh_Log(
            L"[INIT] Separator startup attempt %d/%d",
            attempt,
            kStartupAttempts);

        IPinManagerInterop3* pinManager = nullptr;
        HRESULT hr = CreatePinManager(&pinManager);

        bool pinsReady = false;
        bool positioned = false;
        bool refreshed = false;

        if (SUCCEEDED(hr) && pinManager) {
            pinsReady = PinSeparators(pinManager);

            if (pinsReady) {
                positioned = PositionSeparators(pinManager);

                if (positioned) {
                    refreshed = PulseTaskbarPinList(pinManager);
                }
            } else {
                Wh_Log(
                    L"[INIT] Pin pass incomplete; skipping move pass");
            }

            pinManager->Release();
        }

        if (pinsReady && positioned && refreshed) {
            Wh_Log(
                L"[INIT] Separator startup converged on attempt %d",
                attempt);
            return true;
        }

        if (IsBackendStopRequested()) {
            Wh_Log(L"[INIT] Separator startup cancelled");
            return false;
        }

        if (attempt < kStartupAttempts) {
            Wh_Log(
                L"[INIT] Separator startup attempt failed; "
                L"retrying in %u ms",
                retryDelay);

            if (WaitForBackendStop(retryDelay)) {
                Wh_Log(L"[INIT] Separator startup cancelled");
                return false;
            }

            retryDelay *= kStartupRetryBackoff;
        }
    }

    Wh_Log(
        L"[INIT] Separator startup did not converge after %d attempts",
        kStartupAttempts);
    return false;
}

// -----------------------------------------------------------------------------
// Destruction / cleanup.
// -----------------------------------------------------------------------------

static bool SeparatorShortcutArtifactsExist() {
    if (FileExists(
            GetSeparatorShortcutPath(g_refreshPulseSetting))) {
        return true;
    }

    return std::any_of(
        g_settings.separators.begin(),
        g_settings.separators.end(),
        [](const SeparatorSetting& separator) {
            return FileExists(
                GetSeparatorShortcutPath(separator));
        });
}

static bool UnpinAndDeleteSeparators() {
    std::wstring refreshShortcutPath =
        GetSeparatorShortcutPath(g_refreshPulseSetting);

    if (g_settings.separators.empty() &&
        !FileExists(refreshShortcutPath)) {
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

    // Recover a transient refresh helper left pinned by a failed pulse, mod
    // reload, or Explorer termination between its pin and unpin operations.
    if (FileExists(refreshShortcutPath)) {
        PIDLIST_ABSOLUTE refreshPidl = nullptr;
        hr = GetPidlForPath(refreshShortcutPath, &refreshPidl);

        if (SUCCEEDED(hr) && refreshPidl) {
            // File existence alone doesn't prove whether an earlier pulse got
            // as far as pinning. First ensure the stable helper identity is
            // pinned, then unpin it from a known state.
            HRESULT pinHr =
                pinManager->PinItemFromTrustedCaller(
                    refreshPidl,
                    PMC_TASKBANDPIN);

            HRESULT unpinHr = E_FAIL;
            if (SUCCEEDED(pinHr)) {
                unpinHr =
                    pinManager->UnpinTaskbarItem(
                        refreshPidl,
                        PMC_JUMPVIEWBROKER);
            }

            CoTaskMemFree(refreshPidl);

            if (SUCCEEDED(pinHr) && SUCCEEDED(unpinHr)) {
                if (!DeleteFileIfPresent(refreshShortcutPath)) {
                    allUnpinned = false;
                }
            } else {
                Wh_Log(
                    L"[CLEANUP] Failed to remove transient refresh item "
                    L"pinHr=0x%08X unpinHr=0x%08X",
                    static_cast<unsigned int>(pinHr),
                    static_cast<unsigned int>(unpinHr));
                allUnpinned = false;
            }
        } else {
            Wh_Log(
                L"[CLEANUP] Can't resolve transient refresh item; "
                L"keeping its shortcut");
            allUnpinned = false;
        }
    }

    // Reverse order isn't required because unpinning is PIDL-based, but it
    // minimizes visual/index churn while several separators disappear.
    for (auto it = g_settings.separators.rbegin();
         it != g_settings.separators.rend();
         ++it) {
        const auto& separator = *it;

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator);

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

        // 11 is the exact caller value observed during the native modern
        // Notepad unpin trace on this build, and was already tested with
        // UnpinTaskbarItem successfully.
        HRESULT unpinHr =
            pinManager->UnpinTaskbarItem(
                pidl,
                PMC_JUMPVIEWBROKER);

        CoTaskMemFree(pidl);

        if (SUCCEEDED(unpinHr)) {
            DeleteFileIfPresent(shortcutPath);
        } else {
            Wh_Log(
                L"[CLEANUP] Failed to unpin separator #%d hr=0x%08X",
                separator.ordinal,
                static_cast<unsigned int>(unpinHr));
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

static HWND FindCurrentProcessTaskbarWnd();

static DWORD WINAPI BackendThreadProc(void* parameter) {
    g_currentBackendStopEvent =
        static_cast<HANDLE>(parameter);

    Wh_Log(L"[INIT] Separator backend worker starting");

    if (!FindCurrentProcessTaskbarWnd()) {
        Wh_Log(L"[INIT] Waiting for the taskbar window");

        while (!FindCurrentProcessTaskbarWnd()) {
            if (WaitForBackendStop(250)) {
                Wh_Log(L"[INIT] Separator startup cancelled");
                return 0;
            }
        }
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    if (FAILED(hrInit)) {
        Wh_Log(
            L"[INIT] Backend worker CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(hrInit));
        return 0;
    }

    bool success = CreateAndPinSeparators();

    CoUninitialize();

    if (!success && !IsBackendStopRequested()) {
        Wh_Log(
            L"[INIT] One or more separator operations failed; "
            L"mod remains loaded so unload can still clean up");
    }

    Wh_Log(L"[INIT] Separator backend worker finished");
    return 0;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(
        [](HWND window, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            wchar_t className[32] = {};
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(
                    window,
                    className,
                    ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = window;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

static bool StartBackendWorkerLocked() {
    if (g_backendStopped || g_backendThread) {
        return true;
    }

    g_backendStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_backendStopEvent) {
        Wh_Log(
            L"[INIT] CreateEvent for backend worker failed error=%u",
            GetLastError());
        return false;
    }

    g_backendThread =
        CreateThread(
            nullptr,
            0,
            BackendThreadProc,
            g_backendStopEvent,
            0,
            nullptr);

    if (!g_backendThread) {
        Wh_Log(
            L"[INIT] CreateThread for backend worker failed error=%u",
            GetLastError());
        CloseHandle(g_backendStopEvent);
        g_backendStopEvent = nullptr;
        return false;
    }

    return true;
}

static void TryStartBackendWorker() {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (!g_afterInit || g_unloading || g_backendStopped) {
        return;
    }

    if (!StartBackendWorkerLocked()) {
        Wh_Log(L"[INIT] Failed to start separator backend worker");
    }
}

static void StopBackendWorker() {
    HANDLE thread = nullptr;
    HANDLE stopEvent = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_lifecycleMutex);

        // Permanent latch: no loader-hook callback can create a worker after
        // shutdown begins.
        g_backendStopped = true;
        stopEvent = std::exchange(g_backendStopEvent, nullptr);
        thread = std::exchange(g_backendThread, nullptr);

        if (stopEvent) {
            SetEvent(stopEvent);
        }
    }

    // Don't hold g_lifecycleMutex while joining the worker.
    if (thread) {
        if (WaitForSingleObject(thread, 0) == WAIT_TIMEOUT) {
            Wh_Log(L"[UNINIT] Waiting for separator backend worker");
        }

        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    if (stopEvent) {
        CloseHandle(stopEvent);
    }
}

static bool IsMainExplorerProcess() {
    HWND shellWindow = GetShellWindow();
    if (shellWindow) {
        DWORD shellProcessId = 0;
        GetWindowThreadProcessId(
            shellWindow,
            &shellProcessId);
        return shellProcessId == GetCurrentProcessId();
    }

    // During early shell startup GetShellWindow can still be null. Exclude
    // known folder-process command lines while allowing the future shell host
    // to install its late-load watcher.
    std::wstring commandLine = GetCommandLineW();
    std::transform(
        commandLine.begin(),
        commandLine.end(),
        commandLine.begin(),
        [](wchar_t ch) { return std::towlower(ch); });

    return commandLine.find(L" /factory") == std::wstring::npos &&
           commandLine.find(L" /separate") == std::wstring::npos &&
           commandLine.find(L" -embedding") == std::wstring::npos;
}

BOOL Wh_ModInit() {
    if (!IsMainExplorerProcess()) {
        return FALSE;
    }

    // A completed taskbar with neither Windows 11 taskbar module is an
    // unsupported shell (not an early Windows 11 startup). Don't retain a
    // loader hook there for the process lifetime.
    if (FindCurrentProcessTaskbarWnd() &&
        !GetTaskbarViewModuleHandle() &&
        !GetModuleHandleW(L"taskbar.dll")) {
        return FALSE;
    }

    Wh_Log(L"[INIT] Taskbar Icon Separators loading");

    LoadSettings();

    if (!InitializeStoragePath()) {
        return FALSE;
    }

    if (!InitializeTaskbarStylingHooks()) {
        Wh_Log(
            L"[INIT] TaskListButton styling hooks unavailable");
    }

    if (!InitializeTaskbarInteractionHooks()) {
        Wh_Log(
            L"[INIT] Taskbar interaction hooks unavailable");
    }

    // Hook operations installed during Wh_ModInit are applied automatically
    // before Wh_ModAfterInit. Missing taskbar hooks remain independent from
    // the pin-list backend and can still be installed on a late module load.
    return TRUE;
}

void Wh_ModAfterInit() {
    g_afterInit = true;

    // Close the small race where the module wasn't present in Wh_ModInit but
    // appeared before/around Wh_ModAfterInit.
    if (!g_taskbarViewDllHooked) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            InstallTaskbarViewHooks(module, true);
        }
    }

    // Do the equivalent late-attach check for taskbar.dll. It might have
    // loaded between Wh_ModInit and hook activation, before the watcher could
    // observe its LoadLibraryExW call.
    if (!g_taskbarInteractionHooksInstalled) {
        if (HMODULE module = GetModuleHandleW(L"taskbar.dll")) {
            InstallTaskbarInteractionHooks(module, true);
        }
    }

    TryStartBackendWorker();
}

void Wh_ModBeforeUninit() {
    // Stop applying separator state, then restore every tracked container on
    // its owning taskbar UI thread before Windhawk removes the hooks.
    g_unloading = true;
    StopBackendWorker();
    RestoreTrackedSeparatorVisualStates();
}

void Wh_ModUninit() {
    Wh_Log(L"[UNINIT] Taskbar Icon Separators unloading");

    // Defensive in case an older Windhawk build skips Wh_ModBeforeUninit.
    g_unloading = true;
    StopBackendWorker();

    // Cleanup is based on durable artifacts, not worker or hook readiness.
    // This also removes leftovers from an earlier Explorer session when the
    // current taskbar implementation couldn't be hooked.
    if (!SeparatorShortcutArtifactsExist()) {
        Wh_Log(L"[UNINIT] Taskbar Icon Separators unloaded");
        return;
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize =
        SUCCEEDED(hrInit);

    if (hrInit != RPC_E_CHANGED_MODE && FAILED(hrInit)) {
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

    Wh_Log(L"[UNINIT] Taskbar Icon Separators unloaded");
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
