// ==WindhawkMod==
// @id              taskbar-icon-separators
// @name            Taskbar Icon Separators
// @description     Create tracked icon separators with configurable padding on the taskbar.
// @version         1.0.13
// @author          meteoni
// @github          https://github.com/Meteony
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lpropsys -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Icon Separators

Create tracked icon separators with configurable padding on the taskbar.

![Screenshot](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/taskbar-icon-separators/SEP2.png)
_Example for creating separators on the taskbar_

Another mod offers similar functionality; this implementation takes on a more native-first spin.

This mod uses private COM APIs to insert a genuine taskbar button, styles its width and centering, and keeps activation/tooltip behavior inert. 
Separators remain genuine, can be manually reordered, and can also be created from the taskbar context menu.


Note: The mod creates pinned shortcuts targeting Windows `systray.exe` (with fallback) and reorders the persisted taskbar pin list. 
Cleanup is best-effort; in the worst failure case, manual taskbar unpinning and reordering may be required.

Add separators from the taskbar context menu and drag them into place; positions
are saved automatically. A newly added, unanchored separator keeps a small
"Drag to anchor this separator" guide attached above it until it is moved once; the guide is attached directly to the live TaskListButton with Popup.PlacementTarget, so Windows keeps it aligned through dragging and taskbar relayouts.
Right-click a separator to remove it. Windhawk settings expose only the global separator width.
Win+1 through Win+0 taskbar shortcuts automatically skip anchored separator slots
when selecting their numbered app.

Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 20
  $name: Separator Width
  $description: Width of every separator slot.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <cwchar>
#include <climits>
#include <iterator>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <new>
#include <optional>
#include <cstdlib>
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
    int ordinal;       // Current mod-state row, 1-based (logging only).
    int sourceIndex;   // Current mod-state vector index, 0-based.
    int targetIndex;   // Zero-based value passed to MoveTaskbarPin.
    double width;      // FrameworkElement::MaxWidth for the separator slot.
    std::wstring stableId; // Persistent internal identity, independent of array order.
    std::wstring identity; // AppUserModelID / shortcut stem derived from stableId.
};

struct Settings {
    std::wstring identifierPrefix;
    double width = 20.0;
    std::vector<SeparatorSetting> separators;
};

static Settings g_settings;
static Settings g_appliedSettings;
static SeparatorSetting g_refreshPulseSetting;
static std::vector<SeparatorSetting> g_storedSeparatorSettings;
static std::shared_mutex g_settingsMutex;
// Serializes native Add/drag/Unpin mutations with state-file writes and the
// global Windhawk width-setting callback.
static std::mutex g_settingsMutationMutex;
static std::wstring g_storagePath;
static std::wstring g_iconPath;
static std::wstring g_separatorStatePath;

// True after native state changes in memory and until the backend worker has
// durably written the matching separator-state snapshot.
static bool g_separatorStateDirty = false;  // guarded by g_settingsMutationMutex

static std::atomic<bool> g_taskbarViewDllHooked = false;
static std::atomic<bool> g_taskbarDllHooked = false;
static std::atomic<bool> g_taskbarViewHookInstallationClaimed = false;
static std::atomic<bool> g_taskbarDllHookInstallationClaimed = false;
static std::atomic<unsigned int> g_hookInstallersInFlight = 0;
static std::atomic<bool> g_loadLibraryWatcherInstalled = false;
static std::atomic<bool> g_unloading = false;
// Closed at the start of Wh_ModBeforeUninit, before the broader unloading flag
// is set. This prevents late module-load callbacks from claiming new hook work.
static std::atomic<bool> g_hookInstallationClosed = false;
// True only while this mod intentionally tears down its own pins. User-facing
// Add/Unpin callbacks must not mutate desired state during that window.
static std::atomic<bool> g_internalCleanupInProgress = false;
// This mutex protects only backend worker handles/lifetime. Hook installation
// deliberately never takes it, especially from LoadLibraryExW_Hook.
static std::mutex g_lifecycleMutex;
static bool g_backendStopped = false;
static std::atomic<bool> g_backendHasConverged = false;
static HANDLE g_backendThread = nullptr;
static HANDLE g_backendStopEvent = nullptr;
static HANDLE g_backendWakeEvent = nullptr;
static thread_local HANDLE g_currentBackendStopEvent = nullptr;
static thread_local HANDLE g_currentBackendWakeEvent = nullptr;

static constexpr wchar_t kSeparatorIdentitySuffix[] = L"8F31A7D2";
static constexpr wchar_t kSeparatorIdentifierPrefix[] = L"WindhawkSeparator";
static constexpr wchar_t kSeparatorStateFileName[] = L"separator-state-v1.txt";

// Read-only compatibility key used only for the one-time migration from the
// pre-p42 builds. New builds never write separator configuration to Windhawk
// values/settings; the authoritative separator list lives in mod storage.
static constexpr wchar_t kLegacyIdentityManifestValueName[] =
    L"separatorIdentityManifestV1";

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

static std::wstring BuildSeparatorIdentity(
    std::wstring_view prefix,
    std::wstring_view stableId) {
    return std::wstring(prefix) +
           L"." +
           kSeparatorIdentitySuffix +
           L"." +
           std::wstring(stableId);
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

// The private mod storage directory is the durable ownership record. Every
// shortcut in it was created by this mod, including shortcuts from an older
// settings snapshot or an interrupted refresh pulse.
static std::vector<std::wstring> EnumerateSeparatorShortcuts() {
    std::vector<std::wstring> paths;
    WIN32_FIND_DATAW findData = {};
    HANDLE find = FindFirstFileW(
        JoinPath(g_storagePath, L"*.lnk").c_str(),
        &findData);

    if (find == INVALID_HANDLE_VALUE) {
        return paths;
    }

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            paths.push_back(
                JoinPath(g_storagePath, findData.cFileName));
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);
    std::sort(paths.begin(), paths.end());
    return paths;
}

static std::wstring GetShortcutIdentity(const std::wstring& path) {
    size_t fileNameStart = path.find_last_of(L"\\/");
    fileNameStart = fileNameStart == std::wstring::npos
                        ? 0
                        : fileNameStart + 1;

    size_t extension = path.find_last_of(L'.');
    if (extension == std::wstring::npos || extension < fileNameStart) {
        extension = path.size();
    }

    return path.substr(fileNameStart, extension - fileNameStart);
}

static bool IsIdentityInSettings(
    const Settings& settings,
    std::wstring_view identity) {
    return std::any_of(
        settings.separators.begin(),
        settings.separators.end(),
        [identity](const SeparatorSetting& separator) {
            return identity == std::wstring_view(separator.identity);
        });
}


struct LegacySeparatorIdentitySeed {
    std::wstring stableId;
    int sourceIndex;
    int targetIndex;
};

static bool IsValidStableId(std::wstring_view value) {
    if (value.empty() || value.size() > 64) {
        return false;
    }

    return std::all_of(
        value.begin(),
        value.end(),
        [](wchar_t ch) {
            return std::iswalnum(ch) ||
                   ch == L'-' || ch == L'_';
        });
}

static std::optional<std::wstring> ExtractStableIdFromSeparatorIdentity(
    std::wstring_view identity) {
    static const std::wstring marker =
        std::wstring(L".") +
        kSeparatorIdentitySuffix +
        L".";

    size_t markerPosition =
        identity.rfind(marker);
    if (markerPosition == std::wstring_view::npos) {
        return std::nullopt;
    }

    std::wstring_view stableId =
        identity.substr(
            markerPosition + marker.size());

    if (!IsValidStableId(stableId)) {
        return std::nullopt;
    }

    return std::wstring(stableId);
}


static bool ParseWideInt(
    std::wstring_view text,
    int* valueOut) {
    if (!valueOut || text.empty()) {
        return false;
    }

    std::wstring copy(text);
    wchar_t* end = nullptr;
    long value = std::wcstol(
        copy.c_str(),
        &end,
        10);

    if (!end || *end != L'\0' ||
        value < INT_MIN || value > INT_MAX) {
        return false;
    }

    *valueOut = static_cast<int>(value);
    return true;
}

static std::vector<LegacySeparatorIdentitySeed>
LoadLegacyIdentityManifest() {
    std::vector<wchar_t> buffer(65536);
    size_t length = Wh_GetStringValue(
        kLegacyIdentityManifestValueName,
        buffer.data(),
        buffer.size());

    if (!length) {
        return {};
    }

    std::vector<LegacySeparatorIdentitySeed> result;
    std::wstring_view data(buffer.data(), length);
    size_t lineStart = 0;

    while (lineStart < data.size()) {
        size_t lineEnd = data.find(L'\n', lineStart);
        if (lineEnd == std::wstring_view::npos) {
            lineEnd = data.size();
        }

        std::wstring_view line =
            data.substr(lineStart, lineEnd - lineStart);
        if (!line.empty() && line.back() == L'\r') {
            line.remove_suffix(1);
        }

        // Legacy format:
        // stableId<TAB>sourceIndex<TAB>targetIndex<TAB>width
        std::wstring_view fields[4];
        size_t fieldStart = 0;
        bool valid = true;

        for (int field = 0; field < 4; field++) {
            size_t fieldEnd =
                field == 3
                    ? line.size()
                    : line.find(L'\t', fieldStart);

            if (fieldEnd == std::wstring_view::npos ||
                fieldStart > line.size()) {
                valid = false;
                break;
            }

            fields[field] =
                line.substr(fieldStart, fieldEnd - fieldStart);
            fieldStart = fieldEnd + 1;
        }

        if (valid && IsValidStableId(fields[0])) {
            int sourceIndex = 0;
            int targetIndex = 0;
            int ignoredWidth = 0;

            if (ParseWideInt(fields[1], &sourceIndex) &&
                ParseWideInt(fields[2], &targetIndex) &&
                ParseWideInt(fields[3], &ignoredWidth) &&
                sourceIndex >= -1 &&
                targetIndex >= -1) {
                result.push_back({
                    .stableId = std::wstring(fields[0]),
                    .sourceIndex = sourceIndex,
                    .targetIndex = targetIndex,
                });
            }
        }

        lineStart = lineEnd + 1;
    }

    return result;
}

static void AppendHex(
    std::wstring& output,
    unsigned long long value,
    int digits) {
    static constexpr wchar_t kHexDigits[] =
        L"0123456789abcdef";

    for (int digit = digits - 1; digit >= 0; digit--) {
        output.push_back(
            kHexDigits[
                (value >> (digit * 4)) & 0xF]);
    }
}

static bool GenerateRandomBytes(
    BYTE* buffer,
    ULONG size) {
    using BCryptGenRandom_t =
        LONG(WINAPI*)(
            void* algorithm,
            BYTE* buffer,
            ULONG size,
            ULONG flags);

    static BCryptGenRandom_t bcryptGenRandom = [] {
        HMODULE bcrypt =
            GetModuleHandleW(L"bcrypt.dll");
        if (!bcrypt) {
            bcrypt = LoadLibraryW(L"bcrypt.dll");
        }

        return bcrypt
                   ? reinterpret_cast<BCryptGenRandom_t>(
                         GetProcAddress(
                             bcrypt,
                             "BCryptGenRandom"))
                   : nullptr;
    }();

    // BCRYPT_USE_SYSTEM_PREFERRED_RNG.
    constexpr ULONG kUseSystemPreferredRng = 0x00000002;

    return bcryptGenRandom &&
        bcryptGenRandom(
            nullptr,
            buffer,
            size,
            kUseSystemPreferredRng) >= 0;
}

static std::wstring GenerateStableId() {
    BYTE randomBytes[16] = {};

    if (GenerateRandomBytes(
            randomBytes,
            ARRAYSIZE(randomBytes))) {
        std::wstring result = L"g";
        result.reserve(33);

        for (BYTE byte : randomBytes) {
            AppendHex(result, byte, 2);
        }

        return result;
    }

    // BCryptGenRandom should always be available on supported Windows
    // versions. Retain a process-local fallback so a native Add action doesn't
    // fail merely because random generation unexpectedly did.
    static std::atomic<unsigned long long> counter{0};
    LARGE_INTEGER performanceCounter = {};
    QueryPerformanceCounter(&performanceCounter);

    unsigned long long seed =
        static_cast<unsigned long long>(
            performanceCounter.QuadPart) ^
        (GetTickCount64() << 17) ^
        (static_cast<unsigned long long>(
             GetCurrentProcessId()) << 32) ^
        GetCurrentThreadId();

    std::wstring result = L"g";
    AppendHex(result, seed, 16);
    AppendHex(
        result,
        counter.fetch_add(
            1,
            std::memory_order_relaxed),
        16);
    return result;
}


static bool ParseStateInt(
    std::string_view text,
    int* valueOut) {
    if (!valueOut || text.empty()) {
        return false;
    }

    std::string copy(text);
    char* end = nullptr;
    long value = std::strtol(
        copy.c_str(),
        &end,
        10);

    if (!end || *end != '\0' ||
        value < INT_MIN || value > INT_MAX) {
        return false;
    }

    *valueOut = static_cast<int>(value);
    return true;
}

static bool ReadSeparatorStateFile(
    double width,
    Settings* settingsOut,
    bool* existsOut,
    bool* contentInvalidOut) {
    if (!settingsOut || !existsOut || !contentInvalidOut) {
        return false;
    }

    *existsOut = false;
    *contentInvalidOut = false;
    settingsOut->identifierPrefix = kSeparatorIdentifierPrefix;
    settingsOut->width = width;
    settingsOut->separators.clear();

    DWORD attrs =
        GetFileAttributesW(g_separatorStatePath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND ||
            GetLastError() == ERROR_PATH_NOT_FOUND) {
            return true;
        }

        Wh_Log(
            L"[STATE] GetFileAttributesW('%s') failed error=%u",
            g_separatorStatePath.c_str(),
            GetLastError());
        return false;
    }

    if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
        Wh_Log(
            L"[STATE] State path is unexpectedly a directory: '%s'",
            g_separatorStatePath.c_str());
        return false;
    }

    *existsOut = true;

    HANDLE file =
        CreateFileW(
            g_separatorStatePath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(
            L"[STATE] CreateFileW('%s') for read failed error=%u",
            g_separatorStatePath.c_str(),
            GetLastError());
        return false;
    }

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file, &size)) {
        DWORD error = GetLastError();
        CloseHandle(file);
        Wh_Log(
            L"[STATE] GetFileSizeEx failed error=%u",
            error);
        return false;
    }

    if (size.QuadPart < 0 ||
        size.QuadPart > 1024 * 1024) {
        CloseHandle(file);
        Wh_Log(
            L"[STATE] Invalid state-file size=%lld",
            static_cast<long long>(size.QuadPart));
        *contentInvalidOut = true;
        return false;
    }

    std::string data(
        static_cast<size_t>(size.QuadPart),
        '\0');

    DWORD bytesRead = 0;
    BOOL readOk =
        data.empty() ||
        ReadFile(
            file,
            data.data(),
            static_cast<DWORD>(data.size()),
            &bytesRead,
            nullptr);
    DWORD readError = readOk ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);

    if (!readOk || bytesRead != data.size()) {
        Wh_Log(
            L"[STATE] ReadFile failed/incomplete error=%u read=%u expected=%zu",
            readError,
            bytesRead,
            data.size());
        return false;
    }

    size_t lineStart = 0;
    bool headerSeen = false;
    constexpr size_t kMaxSeparators = 256;

    while (lineStart <= data.size()) {
        size_t lineEnd = data.find('\n', lineStart);
        if (lineEnd == std::string::npos) {
            lineEnd = data.size();
        }

        std::string_view line(
            data.data() + lineStart,
            lineEnd - lineStart);
        if (!line.empty() && line.back() == '\r') {
            line.remove_suffix(1);
        }

        if (!headerSeen) {
            if (line != "v1") {
                Wh_Log(L"[STATE] Unsupported/corrupt separator state header");
                *contentInvalidOut = true;
                return false;
            }
            headerSeen = true;
        } else if (!line.empty()) {
            size_t tab = line.find('\t');
            if (tab == std::string_view::npos ||
                tab == 0 ||
                tab + 1 >= line.size()) {
                Wh_Log(L"[STATE] Malformed separator state row");
                *contentInvalidOut = true;
                return false;
            }

            std::string_view stableIdAscii =
                line.substr(0, tab);
            std::wstring stableId;
            stableId.reserve(stableIdAscii.size());

            bool asciiOnly = true;
            for (unsigned char ch : stableIdAscii) {
                if (ch >= 0x80) {
                    asciiOnly = false;
                    break;
                }
                stableId.push_back(
                    static_cast<wchar_t>(ch));
            }

            int targetIndex = 0;
            if (!asciiOnly ||
                !IsValidStableId(stableId) ||
                !ParseStateInt(
                    line.substr(tab + 1),
                    &targetIndex) ||
                targetIndex < -1) {
                Wh_Log(L"[STATE] Invalid separator state row");
                *contentInvalidOut = true;
                return false;
            }

            if (settingsOut->separators.size() >=
                kMaxSeparators) {
                Wh_Log(
                    L"[STATE] Too many separators in state file");
                *contentInvalidOut = true;
                return false;
            }

            if (std::any_of(
                    settingsOut->separators.begin(),
                    settingsOut->separators.end(),
                    [&stableId](const SeparatorSetting& existing) {
                        return existing.stableId == stableId;
                    })) {
                Wh_Log(
                    L"[STATE] Duplicate stable ID in state file: '%s'",
                    stableId.c_str());
                *contentInvalidOut = true;
                return false;
            }

            int sourceIndex =
                static_cast<int>(
                    settingsOut->separators.size());

            settingsOut->separators.push_back({
                .ordinal = sourceIndex + 1,
                .sourceIndex = sourceIndex,
                .targetIndex = targetIndex,
                .width = width,
                .stableId = stableId,
                .identity = BuildSeparatorIdentity(
                    settingsOut->identifierPrefix,
                    stableId),
            });
        }

        if (lineEnd == data.size()) {
            break;
        }
        lineStart = lineEnd + 1;
    }

    if (!headerSeen) {
        Wh_Log(L"[STATE] Empty/corrupt separator state file");
        *contentInvalidOut = true;
        return false;
    }

    return true;
}

static bool SaveSeparatorStateFile(
    const Settings& settings) {
    std::string serialized = "v1\n";

    for (const auto& separator : settings.separators) {
        if (!IsValidStableId(separator.stableId) ||
            separator.targetIndex < -1) {
            Wh_Log(
                L"[STATE] Refusing to persist invalid separator state");
            return false;
        }

        for (wchar_t ch : separator.stableId) {
            if (ch > 0x7F) {
                Wh_Log(
                    L"[STATE] Refusing to persist non-ASCII stable ID");
                return false;
            }
            serialized.push_back(
                static_cast<char>(ch));
        }

        serialized.push_back('\t');
        serialized +=
            std::to_string(separator.targetIndex);
        serialized.push_back('\n');
    }

    std::wstring temporaryPath =
        g_separatorStatePath + L".tmp";

    HANDLE file =
        CreateFileW(
            temporaryPath.c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(
            L"[STATE] CreateFileW('%s') for write failed error=%u",
            temporaryPath.c_str(),
            GetLastError());
        return false;
    }

    DWORD written = 0;
    BOOL writeOk =
        serialized.empty() ||
        WriteFile(
            file,
            serialized.data(),
            static_cast<DWORD>(serialized.size()),
            &written,
            nullptr);
    DWORD writeError =
        writeOk ? ERROR_SUCCESS : GetLastError();

    BOOL flushOk =
        writeOk && FlushFileBuffers(file);
    DWORD flushError =
        flushOk ? ERROR_SUCCESS : GetLastError();

    CloseHandle(file);

    if (!writeOk ||
        written != serialized.size() ||
        !flushOk) {
        Wh_Log(
            L"[STATE] State write failed error=%u flushError=%u written=%u expected=%zu",
            writeError,
            flushError,
            written,
            serialized.size());
        DeleteFileW(temporaryPath.c_str());
        return false;
    }

    if (!MoveFileExW(
            temporaryPath.c_str(),
            g_separatorStatePath.c_str(),
            MOVEFILE_REPLACE_EXISTING |
                MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(
            L"[STATE] Atomic state replace failed error=%u",
            GetLastError());
        DeleteFileW(temporaryPath.c_str());
        return false;
    }

    return true;
}

static double LoadSeparatorWidthSetting() {
    constexpr int kDefaultWidth = 20;
    constexpr int kMinWidth = 1;
    constexpr int kMaxWidth = 1000;

    int configuredWidth =
        Wh_GetIntSetting(L"width");

    if (configuredWidth == 0) {
        configuredWidth = kDefaultWidth;
    }

    int width =
        std::clamp(
            configuredWidth,
            kMinWidth,
            kMaxWidth);

    if (width != configuredWidth) {
        Wh_Log(
            L"[SETTINGS] Clamped separator width from %d to %d",
            configuredWidth,
            width);
    }

    return static_cast<double>(width);
}

static bool AdoptLegacySeparatorShortcuts(
    Settings* settings) {
    if (!settings) {
        return false;
    }

    const auto shortcutPaths =
        EnumerateSeparatorShortcuts();

    if (shortcutPaths.empty()) {
        return true;
    }

    const std::wstring refreshPulseStableId =
        std::to_wstring(kRefreshPulseOrdinal);

    bool sawUnrecognizedShortcut = false;
    size_t adoptedCount = 0;

    for (const auto& path : shortcutPaths) {
        std::wstring identity =
            GetShortcutIdentity(path);

        auto stableId =
            ExtractStableIdFromSeparatorIdentity(
                identity);
        if (!stableId) {
            sawUnrecognizedShortcut = true;
            Wh_Log(
                L"[STATE] Legacy shortcut identity couldn't be parsed: '%s'",
                identity.c_str());
            continue;
        }

        // A refresh helper can survive an interrupted old backend pass. It is
        // not a user separator and may be cleaned as stale after migration.
        if (*stableId == refreshPulseStableId) {
            continue;
        }

        if (std::any_of(
                settings->separators.begin(),
                settings->separators.end(),
                [&stableId](const SeparatorSetting& separator) {
                    return separator.stableId == *stableId;
                })) {
            continue;
        }

        const int sourceIndex =
            static_cast<int>(
                settings->separators.size());

        settings->separators.push_back({
            .ordinal = sourceIndex + 1,
            .sourceIndex = sourceIndex,
            // The old shortcut proves ownership but not its pin-list index.
            // Keep it unanchored at Windows' native end until the user drags it;
            // the ordinary TryMoveGroup path then records a durable position.
            .targetIndex = -1,
            .width = settings->width,
            .stableId = *stableId,
            // Preserve the exact old shortcut identity for this migration
            // session. The new state format stores only stable IDs/positions.
            .identity = std::move(identity),
        });

        adoptedCount++;
    }

    if (sawUnrecognizedShortcut) {
        // No authoritative state file exists yet. Even a partially successful
        // adoption must not authorize stale cleanup to delete an owned artifact
        // that this migration couldn't identify.
        Wh_Log(
            L"[STATE] Refusing migration while unrecognized legacy "
            L"shortcut artifacts exist");
        return false;
    }

    if (adoptedCount > 0) {
        Wh_Log(
            L"[STATE] Adopted %zu legacy separator shortcut(s) from mod storage",
            adoptedCount);
    }

    return true;
}


static bool MigrateLegacySeparatorState(
    double width,
    Settings* settingsOut) {
    if (!settingsOut) {
        return false;
    }

    Settings migrated;
    migrated.identifierPrefix =
        kSeparatorIdentifierPrefix;
    migrated.width = width;

    auto legacySeeds =
        LoadLegacyIdentityManifest();

    if (!legacySeeds.empty()) {
        // The old manifest was already ordered in the mod's logical separator
        // order. Keep every stable ID and target position, but ignore its old
        // per-row width: p42 has one global width setting.
        for (const auto& seed : legacySeeds) {
            if (!IsValidStableId(seed.stableId) ||
                seed.targetIndex < -1) {
                continue;
            }

            if (std::any_of(
                    migrated.separators.begin(),
                    migrated.separators.end(),
                    [&seed](const SeparatorSetting& existing) {
                        return existing.stableId == seed.stableId;
                    })) {
                continue;
            }

            int sourceIndex =
                static_cast<int>(
                    migrated.separators.size());

            migrated.separators.push_back({
                .ordinal = sourceIndex + 1,
                .sourceIndex = sourceIndex,
                .targetIndex = seed.targetIndex,
                .width = width,
                .stableId = seed.stableId,
                .identity = BuildSeparatorIdentity(
                    migrated.identifierPrefix,
                    seed.stableId),
            });
        }
    } else {
        // Older builds without an identity manifest: import the old visible
        // separator rows once. Numeric IDs preserve the pre-stable-ID shortcut
        // names for the ordinary migration case.
        constexpr int kMaxSeparators = 256;

        for (int i = 0; i < kMaxSeparators; i++) {
            int position =
                Wh_GetIntSetting(
                    L"separators[%d].index",
                    i);

            if (position == 0) {
                break;
            }

            int targetIndex = -1;
            if (position == -1) {
                targetIndex = -1;
            } else if (position >= 1) {
                targetIndex = position - 1;
            } else {
                continue;
            }

            std::wstring stableId =
                std::to_wstring(i + 1);

            migrated.separators.push_back({
                .ordinal = i + 1,
                .sourceIndex = i,
                .targetIndex = targetIndex,
                .width = width,
                .stableId = stableId,
                .identity = BuildSeparatorIdentity(
                    migrated.identifierPrefix,
                    stableId),
            });
        }
    }

    // Published v0.5.38 could have live separator shortcuts even when its
    // default settings row was never explicitly persisted. If the legacy
    // settings sources found nothing, recover those owned shortcuts before
    // creating the new authoritative state file.
    if (migrated.separators.empty() &&
        !AdoptLegacySeparatorShortcuts(
            &migrated)) {
        return false;
    }

    if (!SaveSeparatorStateFile(migrated)) {
        Wh_Log(
            L"[STATE] Failed to create p42 separator state during migration");
        return false;
    }

    Wh_Log(
        L"[STATE] Migrated %zu separator(s) into '%s'",
        migrated.separators.size(),
        g_separatorStatePath.c_str());

    *settingsOut = std::move(migrated);
    return true;
}

static bool LoadSettingsFromStorage(
    Settings* settingsOut,
    SeparatorSetting* refreshPulseOut,
    bool* stateNeedsSaveOut) {
    if (!settingsOut || !refreshPulseOut || !stateNeedsSaveOut) {
        return false;
    }

    *stateNeedsSaveOut = false;

    double width =
        LoadSeparatorWidthSetting();

    bool stateExists = false;
    bool contentInvalid = false;
    Settings settings;

    if (!ReadSeparatorStateFile(
            width,
            &settings,
            &stateExists,
            &contentInvalid)) {
        if (!contentInvalid) {
            return false;
        }

        Settings recovered;
        recovered.identifierPrefix =
            kSeparatorIdentifierPrefix;
        recovered.width = width;

        if (!AdoptLegacySeparatorShortcuts(&recovered)) {
            Wh_Log(
                L"[STATE] State recovery couldn't adopt every owned shortcut");
            return false;
        }

        DWORD attributes =
            GetFileAttributesW(g_separatorStatePath.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring backupPath =
                g_separatorStatePath +
                L".invalid-" +
                std::to_wstring(GetTickCount64());

            if (MoveFileExW(
                    g_separatorStatePath.c_str(),
                    backupPath.c_str(),
                    MOVEFILE_WRITE_THROUGH)) {
                Wh_Log(
                    L"[STATE] Moved invalid state file aside as '%s'",
                    backupPath.c_str());
            } else {
                Wh_Log(
                    L"[STATE] Couldn't move invalid state file aside "
                    L"error=%u; recovery will retry persistence in place",
                    GetLastError());
            }
        }

        settings = std::move(recovered);
        stateExists = true;
        *stateNeedsSaveOut = true;

        Wh_Log(
            L"[STATE] Recovered %zu separator(s) from owned shortcuts",
            settings.separators.size());
    }

    if (!stateExists &&
        !MigrateLegacySeparatorState(
            width,
            &settings)) {
        return false;
    }

    *refreshPulseOut = {
        .ordinal = kRefreshPulseOrdinal,
        .sourceIndex = -1,
        .targetIndex = 0,
        .width = width,
        .stableId = std::to_wstring(
            kRefreshPulseOrdinal),
        .identity = BuildSeparatorIdentity(
            settings.identifierPrefix,
            std::to_wstring(
                kRefreshPulseOrdinal)),
    };

    Wh_Log(
        L"[STATE] Loaded %zu separator(s) from mod storage; width=%d",
        settings.separators.size(),
        static_cast<int>(width));

    *settingsOut = std::move(settings);
    return true;
}

static const SeparatorSetting* FindSeparatorByIdentity(
    const Settings& settings,
    std::wstring_view identity) {
    auto it = std::find_if(
        settings.separators.begin(),
        settings.separators.end(),
        [identity](const SeparatorSetting& separator) {
            return separator.identity ==
                std::wstring(identity);
        });

    return it == settings.separators.end()
               ? nullptr
               : &*it;
}

static void AppendStoredSeparatorCandidate(
    std::vector<SeparatorSetting>& stored,
    const SeparatorSetting& candidate,
    const Settings& desiredSettings,
    const SeparatorSetting& refreshPulse) {
    if (candidate.identity.empty() ||
        IsIdentityInSettings(
            desiredSettings,
            candidate.identity) ||
        candidate.identity == refreshPulse.identity) {
        return;
    }

    if (std::any_of(
            stored.begin(),
            stored.end(),
            [&candidate](const SeparatorSetting& existing) {
                return existing.identity == candidate.identity;
            })) {
        return;
    }

    SeparatorSetting copy = candidate;
    copy.ordinal = 0;
    stored.push_back(std::move(copy));
}

static std::vector<SeparatorSetting>
BuildStoredSeparatorSettings(
    const Settings& desiredSettings,
    const SeparatorSetting& refreshPulse,
    const Settings& appliedSettings,
    const Settings* previousDesiredSettings = nullptr,
    const SeparatorSetting* previousRefreshPulse = nullptr,
    const std::vector<SeparatorSetting>* retainedStoredSettings = nullptr) {
    std::vector<SeparatorSetting> stored;

    // Keep identities from earlier transitions until a successful backend
    // pass proves that their backing shortcuts are gone. This also covers a
    // previous backend snapshot that was superseded before it created/pinned
    // all of its files: if one such item appears late, it is still styled and
    // inert rather than briefly becoming an ordinary taskbar button.
    if (retainedStoredSettings) {
        for (const auto& separator : *retainedStoredSettings) {
            AppendStoredSeparatorCandidate(
                stored,
                separator,
                desiredSettings,
                refreshPulse);
        }
    }

    if (previousDesiredSettings) {
        for (const auto& separator :
             previousDesiredSettings->separators) {
            AppendStoredSeparatorCandidate(
                stored,
                separator,
                desiredSettings,
                refreshPulse);
        }
    }

    if (previousRefreshPulse) {
        AppendStoredSeparatorCandidate(
            stored,
            *previousRefreshPulse,
            desiredSettings,
            refreshPulse);
    }

    const double defaultWidth =
        !appliedSettings.separators.empty()
            ? appliedSettings.separators.front().width
            : (!desiredSettings.separators.empty()
                   ? desiredSettings.separators.front().width
                   : 20.0);

    for (const auto& path : EnumerateSeparatorShortcuts()) {
        std::wstring identity =
            GetShortcutIdentity(path);

        if (identity.empty() ||
            IsIdentityInSettings(
                desiredSettings,
                identity) ||
            identity == refreshPulse.identity) {
            continue;
        }

        if (std::any_of(
                stored.begin(),
                stored.end(),
                [&identity](const SeparatorSetting& existing) {
                    return existing.identity == identity;
                })) {
            continue;
        }

        const SeparatorSetting* known =
            FindSeparatorByIdentity(
                appliedSettings,
                identity);

        if (!known && previousDesiredSettings) {
            known = FindSeparatorByIdentity(
                *previousDesiredSettings,
                identity);
        }

        if (known) {
            AppendStoredSeparatorCandidate(
                stored,
                *known,
                desiredSettings,
                refreshPulse);
        } else {
            stored.push_back({
                .ordinal = 0,
                .sourceIndex = -1,
                .targetIndex = 0,
                .width = defaultWidth,
                .stableId = {},
                .identity = std::move(identity),
            });
        }
    }

    return stored;
}

static bool SettingsRequireBackendReconcile(
    const Settings& oldSettings,
    const Settings& newSettings);

static bool SettingsSnapshotsEqual(
    const Settings& a,
    const Settings& b) {
    if (a.identifierPrefix != b.identifierPrefix ||
        a.separators.size() != b.separators.size()) {
        return false;
    }

    for (size_t i = 0; i < a.separators.size(); i++) {
        const auto& left = a.separators[i];
        const auto& right = b.separators[i];

        if (left.ordinal != right.ordinal ||
            left.sourceIndex != right.sourceIndex ||
            left.targetIndex != right.targetIndex ||
            left.width != right.width ||
            left.stableId != right.stableId ||
            left.identity != right.identity) {
            return false;
        }
    }

    return true;
}

static void RefreshStoredSeparatorSettings() {
    Settings desired;
    Settings applied;
    SeparatorSetting refreshPulse;

    {
        std::shared_lock lock(g_settingsMutex);
        desired = g_settings;
        applied = g_appliedSettings;
        refreshPulse = g_refreshPulseSetting;
    }

    auto stored =
        BuildStoredSeparatorSettings(
            desired,
            refreshPulse,
            applied);

    {
        std::unique_lock lock(g_settingsMutex);

        // The desired settings might have changed while the filesystem was
        // enumerated. In that case the settings-change path will rebuild this
        // snapshot again.
        if (SettingsSnapshotsEqual(
                desired,
                g_settings)) {
            g_storedSeparatorSettings =
                std::move(stored);
        }
    }
}

static bool WriteBinaryFile(
    const std::wstring& path,
    const void* data,
    DWORD size) {
    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
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
    Settings settings;
    SeparatorSetting refreshPulse;
    bool stateNeedsSave = false;

    if (!LoadSettingsFromStorage(
            &settings,
            &refreshPulse,
            &stateNeedsSave)) {
        Wh_Log(L"[STATE] Failed to load separator state");
        return false;
    }

    std::unique_lock lock(g_settingsMutex);
    g_settings = std::move(settings);
    g_appliedSettings = {};
    g_refreshPulseSetting =
        std::move(refreshPulse);
    g_storedSeparatorSettings.clear();
    g_separatorStateDirty = stateNeedsSave;
    return true;
}

static const SeparatorSetting* FindSeparatorByStableId(
    const Settings& settings,
    std::wstring_view stableId) {
    auto it = std::find_if(
        settings.separators.begin(),
        settings.separators.end(),
        [stableId](const SeparatorSetting& separator) {
            return separator.stableId ==
                std::wstring(stableId);
        });

    return it == settings.separators.end()
               ? nullptr
               : &*it;
}

static bool SettingsRequireBackendReconcile(
    const Settings& oldSettings,
    const Settings& newSettings) {
    if (oldSettings.identifierPrefix !=
        newSettings.identifierPrefix) {
        return true;
    }

    if (oldSettings.separators.size() !=
        newSettings.separators.size()) {
        return true;
    }

    for (const auto& oldSeparator :
         oldSettings.separators) {
        const SeparatorSetting* newSeparator =
            FindSeparatorByStableId(
                newSettings,
                oldSeparator.stableId);

        if (!newSeparator ||
            oldSeparator.identity != newSeparator->identity ||
            oldSeparator.targetIndex != newSeparator->targetIndex) {
            return true;
        }
    }

    return false;
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
    g_separatorStatePath =
        JoinPath(
            g_storagePath,
            kSeparatorStateFileName);

    return true;
}


// -----------------------------------------------------------------------------
// TaskListButton separator width styling.
//
// Separator width is owned through the XAML FrameworkElement::MaxWidth
// property only. No private TaskListButton extent fields or private layout
// entry points are modified.
// -----------------------------------------------------------------------------

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
static TaskListButton_UpdateVisualStates_t
    g_taskListButtonUpdateVisualStatesOriginal;

static void QueueBackendWork();
static void FlushPendingSeparatorStateSynchronously();
static void RefreshTrackedSeparatorVisualStates();
using TaskListButton_OnDragCompletedGesture_t =
    void(WINAPI*)(void* pThis);
static TaskListButton_OnDragCompletedGesture_t
    g_taskListButtonOnDragCompletedGestureOriginal;

static void WINAPI TaskListButton_OnDragCompletedGesture_Hook(
    void* pThis) {
    g_taskListButtonOnDragCompletedGestureOriginal(
        pThis);

    if (!g_unloading) {
        bool stateDirty = false;
        {
            std::lock_guard<std::mutex> mutationLock(
                g_settingsMutationMutex);
            stateDirty = g_separatorStateDirty;
        }

        if (stateDirty) {
            // Persistence was already queued by TryMoveGroup. This optional
            // completion callback only refreshes presentation after the native
            // drag has completely unwound, closing an unanchored separator's
            // guide once it has its first concrete index.
            RefreshTrackedSeparatorVisualStates();
        }

    }
}

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

static bool FindSeparatorByAutomationName(
    std::wstring_view name,
    SeparatorSetting* separatorOut = nullptr) {
    if (name.empty()) {
        return false;
    }

    // Almost every TaskListButton is unrelated to this mod. Reject it with a
    // single search before comparing complete identities from the settings or
    // durable storage snapshots.
    static const std::wstring identityMarker =
        std::wstring(L".") + kSeparatorIdentitySuffix + L".";
    if (name.find(identityMarker) == std::wstring_view::npos) {
        return false;
    }

    std::shared_lock lock(g_settingsMutex);

    const auto copyMatch =
        [separatorOut](const SeparatorSetting& separator) {
            if (separatorOut) {
                *separatorOut = separator;
            }
            return true;
        };

    for (const auto& separator : g_settings.separators) {
        if (ContainsIdentityToken(name, separator.identity)) {
            return copyMatch(separator);
        }
    }

    for (const auto& separator : g_storedSeparatorSettings) {
        if (ContainsIdentityToken(name, separator.identity)) {
            return copyMatch(separator);
        }
    }

    if (ContainsIdentityToken(
            name,
            g_refreshPulseSetting.identity)) {
        return copyMatch(g_refreshPulseSetting);
    }

    return false;
}

static bool GetSeparatorForElement(
    const FrameworkElement& element,
    SeparatorSetting* separatorOut = nullptr) {
    if (!element) {
        return false;
    }

    winrt::hstring automationName;

    try {
        automationName =
            winrt::Windows::UI::Xaml::Automation::
                AutomationProperties::GetName(element);
    } catch (...) {
        return false;
    }

    std::wstring_view name{
        automationName.c_str(),
        automationName.size()
    };

    return FindSeparatorByAutomationName(
        name,
        separatorOut);
}

static bool GetSeparatorForTaskListButton(
    void* pThis,
    FrameworkElement* elementOut = nullptr,
    SeparatorSetting* separatorOut = nullptr) {
    FrameworkElement element =
        GetTaskListButtonElement(pThis);

    if (!GetSeparatorForElement(
            element,
            separatorOut)) {
        return false;
    }

    if (elementOut) {
        *elementOut = element;
    }

    return true;
}

struct SeparatorVisualState {
    winrt::weak_ref<FrameworkElement> element;
    void* taskListButton;

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

    // targetIndex == -1 presentation. The Popup is created asynchronously after
    // TaskListButton::UpdateVisualStates has unwound, and is kept open until the
    // first successful native drag assigns a concrete targetIndex.
    winrt::Windows::UI::Xaml::Controls::Primitives::Popup
        anchorHintPopup{nullptr};
    winrt::Windows::Foundation::IAsyncAction
        anchorHintAction{nullptr};
    std::wstring anchorHintIdentity;
    bool anchorHintQueued = false;
};

static thread_local std::list<SeparatorVisualState>
    g_separatorVisualStates;
static thread_local bool g_updatingSeparatorVisualStates;

struct SeparatorVisualStateUpdateGuard {
    ~SeparatorVisualStateUpdateGuard() {
        g_updatingSeparatorVisualStates = false;
    }
};

static void CloseSeparatorAnchorHint(
    SeparatorVisualState& state) {
    state.anchorHintQueued = false;
    state.anchorHintIdentity.clear();

    // RunAsync work is cancellable. This matters on mod unload: no queued
    // dispatcher delegate may retain a callback into code after the mod is gone.
    if (state.anchorHintAction) {
        try {
            state.anchorHintAction.Cancel();
        } catch (...) {
        }
        state.anchorHintAction = nullptr;
    }

    if (!state.anchorHintPopup) {
        return;
    }

    try {
        state.anchorHintPopup.IsOpen(false);
    } catch (...) {
    }

    state.anchorHintPopup = nullptr;
}

static void PruneExpiredSeparatorVisualStates() {
    for (auto it = g_separatorVisualStates.begin();
         it != g_separatorVisualStates.end();) {
        if (!it->element.get()) {
            CloseSeparatorAnchorHint(*it);
            it = g_separatorVisualStates.erase(it);
        } else {
            ++it;
        }
    }
}

static auto FindSeparatorVisualState(void* taskListButton) {
    return std::find_if(
        g_separatorVisualStates.begin(),
        g_separatorVisualStates.end(),
        [taskListButton](const SeparatorVisualState& state) {
            return state.taskListButton == taskListButton;
        });
}

static void ShowQueuedSeparatorAnchorHint(
    void* taskListButton,
    winrt::weak_ref<FrameworkElement> weakElement,
    std::wstring identity) {
    auto stateIt =
        FindSeparatorVisualState(taskListButton);
    if (stateIt == g_separatorVisualStates.end()) {
        return;
    }

    SeparatorVisualState& state = *stateIt;

    // A recycled TaskListButton can queue a second request before an older one
    // runs. Only the request for the currently tracked identity owns this state.
    if (state.anchorHintIdentity != identity) {
        return;
    }

    state.anchorHintQueued = false;
    state.anchorHintAction = nullptr;

    if (g_unloading) {
        CloseSeparatorAnchorHint(state);
        return;
    }

    FrameworkElement element = weakElement.get();
    FrameworkElement trackedElement = state.element.get();
    if (!element ||
        !trackedElement ||
        winrt::get_abi(element) != winrt::get_abi(trackedElement)) {
        CloseSeparatorAnchorHint(state);
        return;
    }

    SeparatorSetting separator;
    if (!GetSeparatorForElement(
            element,
            &separator) ||
        separator.identity != identity ||
        separator.targetIndex >= 0) {
        CloseSeparatorAnchorHint(state);
        return;
    }

    try {
        using namespace winrt::Windows::UI;
        using namespace winrt::Windows::UI::Xaml::Controls;
        using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
        using namespace winrt::Windows::UI::Xaml::Media;

        XamlRoot xamlRoot = element.XamlRoot();
        if (!xamlRoot) {
            return;
        }

        Popup popup;
        popup.XamlRoot(xamlRoot);
        popup.IsLightDismissEnabled(false);
        popup.ShouldConstrainToRootBounds(false);
        popup.IsHitTestVisible(false);

        // A tiny self-contained callout rather than another taskbar tooltip.
        // It deliberately owns no input and remains visible until we close it.
        StackPanel callout;
        callout.HorizontalAlignment(HorizontalAlignment::Center);
        callout.IsHitTestVisible(false);

        bool lightTheme =
            element.ActualTheme() == ElementTheme::Light;

        Color backgroundColor =
            lightTheme
                ? Color{0xF7, 0xF9, 0xF9, 0xF9}
                : Color{0xF7, 0x2B, 0x2B, 0x2B};
        Color foregroundColor =
            lightTheme
                ? Color{0xFF, 0x1B, 0x1B, 0x1B}
                : Color{0xFF, 0xFF, 0xFF, 0xFF};
        Color borderColor =
            lightTheme
                ? Color{0x24, 0x00, 0x00, 0x00}
                : Color{0x28, 0xFF, 0xFF, 0xFF};

        SolidColorBrush backgroundBrush{backgroundColor};
        SolidColorBrush foregroundBrush{foregroundColor};
        SolidColorBrush borderBrush{borderColor};

        Border card;
        card.Background(backgroundBrush);
        card.BorderBrush(borderBrush);
        card.BorderThickness(Thickness{1, 1, 1, 1});
        card.CornerRadius(CornerRadius{8, 8, 8, 8});
        card.Padding(Thickness{12, 7, 12, 7});
        card.IsHitTestVisible(false);

        TextBlock label;
        label.Text(L"Drag to anchor this separator");
        label.FontSize(12);
        label.Foreground(foregroundBrush);
        label.TextWrapping(TextWrapping::NoWrap);
        label.IsHitTestVisible(false);
        card.Child(label);

        // A short stem visually ties the callout to the narrow separator slot.
        Border stem;
        stem.Width(2);
        stem.Height(6);
        stem.Background(backgroundBrush);
        stem.HorizontalAlignment(HorizontalAlignment::Center);
        stem.IsHitTestVisible(false);

        callout.Children().Append(card);
        callout.Children().Append(stem);
        popup.Child(callout);

        // Windows 11 Popup has a real PlacementTarget. Let XAML own the
        // relationship instead of copying target coordinates into detached
        // HorizontalOffset/VerticalOffset values. This makes the guide follow
        // native drag transforms, virtual-desktop relayouts, animations and
        // container movement automatically.
        popup.PlacementTarget(element);
        popup.DesiredPlacement(PopupPlacementMode::Top);

        state.anchorHintPopup = popup;
        state.anchorHintIdentity = identity;

        popup.IsOpen(true);

        Wh_Log(
            L"[NATIVE-ADD] Showing persistent anchor guide identity='%s'",
            identity.c_str());
    } catch (...) {
        state.anchorHintPopup = nullptr;
        Wh_Log(
            L"[NATIVE-ADD] Failed to show persistent anchor guide identity='%s'",
            identity.c_str());
    }
}

static void UpdateSeparatorAnchorHint(
    SeparatorVisualState& state,
    const FrameworkElement& element,
    const SeparatorSetting& separator) {
    if (!element ||
        g_unloading ||
        separator.targetIndex >= 0) {
        CloseSeparatorAnchorHint(state);
        return;
    }

    if ((state.anchorHintPopup || state.anchorHintQueued) &&
        state.anchorHintIdentity != separator.identity) {
        CloseSeparatorAnchorHint(state);
    }

    if (state.anchorHintPopup &&
        state.anchorHintIdentity == separator.identity) {
        try {
            if (state.anchorHintPopup.IsOpen()) {
                // PlacementTarget keeps the Popup attached to the live
                // TaskListButton; no manual coordinate refresh is needed.
                return;
            }
        } catch (...) {
        }

        CloseSeparatorAnchorHint(state);
    }

    if (state.anchorHintQueued &&
        state.anchorHintIdentity == separator.identity) {
        return;
    }

    // Never create/open the Popup from inside UpdateVisualStates itself.
    // Constructing popup presentation while TaskListButton is still
    // materializing can re-enter XAML/layout at a fragile point. Queue one
    // low-priority dispatcher callback and revalidate identity/state there.
    try {
        auto dispatcher = element.Dispatcher();
        if (!dispatcher) {
            return;
        }

        state.anchorHintIdentity = separator.identity;
        state.anchorHintQueued = true;

        auto weakElement =
            winrt::make_weak(element);
        void* taskListButton =
            state.taskListButton;
        std::wstring identity =
            separator.identity;

        state.anchorHintAction =
            dispatcher.RunAsync(
                winrt::Windows::UI::Core::
                    CoreDispatcherPriority::Low,
                winrt::Windows::UI::Core::DispatchedHandler{
                    [taskListButton,
                     weakElement,
                     identity = std::move(identity)]() mutable {
                        ShowQueuedSeparatorAnchorHint(
                            taskListButton,
                            weakElement,
                            std::move(identity));
                    }});
    } catch (...) {
        state.anchorHintQueued = false;
        state.anchorHintIdentity.clear();
    }
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
    const FrameworkElement& iconPanel) {
    if (!iconPanel) {
        return;
    }

    try {
        // Once a TaskListButton gets narrower than the normal icon slot, the
        // stock template can effectively left-anchor the icon content. Center
        // both the slot and the actual Icon child instead of using a magic
        // pixel translation, so the correction adapts to icon-size mods/DPI.
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
    const FrameworkElement& taskListButton,
    const FrameworkElement& iconPanel) {
    if (!taskListButton) {
        return;
    }

    try {
        // Taskbar.TaskListLabeledButtonPanel owns the task-button CommonStates.
        // Its Border#BackgroundElement is the rounded hover/press surface.
        // Keep it in the tree (so layout is unaffected) and make only that
        // surface invisible for separator buttons.
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

        // The persistent anchor guide is a separate Popup. Keep every ordinary
        // taskbar tooltip silent so it never competes with that presentation.
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
    std::list<SeparatorVisualState>::iterator stateIt) {
    SeparatorVisualState& state = *stateIt;

    CloseSeparatorAnchorHint(state);

    auto element = state.element.get();

    // The FrameworkElement weak reference is the lifetime authority for the
    // raw TaskListButton implementation pointer stored alongside it.
    if (!element) {
        g_separatorVisualStates.erase(stateIt);
        return;
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
    // Keep the stack-backed parameter alive until the target thread has
    // finished processing the hook callback. UnhookWindowsHookEx doesn't wait
    // for a callback that was already entered.
    SendMessageW(
        window,
        message,
        0,
        reinterpret_cast<LPARAM>(&parameters));
    UnhookWindowsHookEx(hook);
    return parameters.executed;
}

// XAML owns menu items and their delegates beyond the hook invocation that
// creates them. Retain the exact objects/tokens on their taskbar UI thread so
// teardown can unregister every mod-owned delegate before the DLL is unmapped.
static thread_local winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem
    g_injectedAddSeparatorMenuItem{nullptr};
static thread_local winrt::event_token
    g_injectedAddSeparatorClickToken{};
static thread_local winrt::Windows::UI::Xaml::Controls::MenuFlyout
    g_openSeparatorContextMenu{nullptr};
static thread_local winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem
    g_openSeparatorContextMenuItem{nullptr};
static thread_local winrt::event_token
    g_openSeparatorContextMenuClickToken{};
static thread_local winrt::event_token
    g_openSeparatorContextMenuClosedToken{};
static std::atomic<bool> g_separatorContextMenuOpen = false;

static void DetachInjectedAddSeparatorMenuHandler() {
    try {
        if (g_injectedAddSeparatorMenuItem) {
            g_injectedAddSeparatorMenuItem.Click(
                g_injectedAddSeparatorClickToken);
        }
    } catch (...) {
        Wh_Log(L"[CTX] Failed to detach Add separator menu handler");
    }
    g_injectedAddSeparatorMenuItem = nullptr;
    g_injectedAddSeparatorClickToken = {};
}

static void CloseSeparatorContextMenuOnCurrentThread(bool hide) {
    try {
        if (g_openSeparatorContextMenuItem) {
            g_openSeparatorContextMenuItem.Click(
                g_openSeparatorContextMenuClickToken);
        }
    } catch (...) {
        Wh_Log(L"[CTX] Failed to detach separator menu command");
    }

    try {
        if (g_openSeparatorContextMenu) {
            g_openSeparatorContextMenu.Closed(
                g_openSeparatorContextMenuClosedToken);
        }
    } catch (...) {
        Wh_Log(L"[CTX] Failed to detach separator menu close handler");
    }

    if (hide) {
        try {
            if (g_openSeparatorContextMenu) {
                g_openSeparatorContextMenu.Hide();
            }
        } catch (...) {
            Wh_Log(L"[CTX] Failed to hide separator context menu");
        }
    }
    g_openSeparatorContextMenuItem = nullptr;
    g_openSeparatorContextMenu = nullptr;
    g_openSeparatorContextMenuClickToken = {};
    g_openSeparatorContextMenuClosedToken = {};
}

static void WINAPI RestoreSeparatorVisualStatesOnCurrentThread(void*) {
    DetachInjectedAddSeparatorMenuHandler();
    CloseSeparatorContextMenuOnCurrentThread(true);
    g_separatorContextMenuOpen = false;

    g_updatingSeparatorVisualStates = true;
    SeparatorVisualStateUpdateGuard updateGuard;
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

static void WINAPI RefreshSeparatorVisualStatesOnCurrentThread(void*) {
    g_updatingSeparatorVisualStates = true;
    SeparatorVisualStateUpdateGuard updateGuard;
    PruneExpiredSeparatorVisualStates();

    for (auto it = g_separatorVisualStates.begin();
         it != g_separatorVisualStates.end();) {
        auto current = it++;

        SeparatorVisualState& state = *current;
        auto element = state.element.get();

        SeparatorSetting separator;
        if (!element ||
            g_unloading ||
            !GetSeparatorForElement(
                element,
                &separator)) {
            RestoreSeparatorVisualState(current);
            continue;
        }

        ApplySeparatorMaxWidthOverride(
            state,
            element,
            separator);

        FrameworkElement iconPanel =
            FindChildByName(element, L"IconPanel");
        CenterSeparatorIcon(state, iconPanel);
        SuppressSeparatorHoverChrome(
            state,
            element,
            iconPanel);
        UpdateSeparatorAnchorHint(
            state,
            element,
            separator);
        }
}

static void RefreshTrackedSeparatorVisualStates() {
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

            auto* context =
                reinterpret_cast<EnumContext*>(lParam);
            if (std::find(
                    context->processedThreadIds.begin(),
                    context->processedThreadIds.end(),
                    threadId) !=
                context->processedThreadIds.end()) {
                return TRUE;
            }

            context->processedThreadIds.push_back(threadId);

            if (!RunFromWindowThread(
                    window,
                    RefreshSeparatorVisualStatesOnCurrentThread,
                    nullptr)) {
                Wh_Log(
                    L"[SETTINGS] Failed to refresh taskbar visuals on thread %u",
                    threadId);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
}

// Native empty-taskbar context menu frontend.
//
// This follows the same Taskbar.View menu construction path used by
// Taskbar Restart Explorer: mark
// the ShowTaskbarSettingsContextMenu build scope, intercept the first
// IVector<MenuFlyoutItemBase>::Append, and add one native MenuFlyoutItem.
static void RequestBackendReconcile();
static void AddSeparatorFromTaskbarMenu();

static constexpr wchar_t kAddSeparatorMenuText[] = L"Add separator";
static constexpr wchar_t kAddSeparatorMenuName[] = L"WindhawkAddSeparatorItem";
static constexpr wchar_t kAddSeparatorMenuDividerName[] =
    L"WindhawkAddSeparatorDivider";

static thread_local int g_taskbarSettingsMenuDepth = 0;
static thread_local bool g_taskbarSettingsMenuInjected = false;

static bool IsNamedMenuItem(
    winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const& item,
    const wchar_t* name) {
    try {
        if (auto element = item.try_as<FrameworkElement>()) {
            return std::wstring_view(element.Name().c_str()) == name;
        }
    } catch (...) {
    }
    return false;
}

static bool IsMenuSeparator(
    winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const& item) {
    try {
        return !!item.try_as<
            winrt::Windows::UI::Xaml::Controls::MenuFlyoutSeparator>();
    } catch (...) {
        return false;
    }
}

static void OnAddSeparatorMenuClick(
    winrt::Windows::Foundation::IInspectable const&,
    RoutedEventArgs const&) {
    AddSeparatorFromTaskbarMenu();
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(
        void* pThis,
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const& item);
static MenuFlyoutItemBaseVector_Append_t
    g_menuFlyoutItemBaseVectorAppendOriginal;

static void AppendAddSeparatorMenuItems(void* vectorThis) {
    using namespace winrt::Windows::UI::Xaml::Controls;

    // A newer taskbar-menu build supersedes the previous injected item on this
    // UI thread. Detach its callback before replacing our retained reference.
    DetachInjectedAddSeparatorMenuHandler();

    MenuFlyoutItem item;
    item.Name(kAddSeparatorMenuName);
    item.Tag(winrt::box_value(winrt::hstring{kAddSeparatorMenuName}));
    item.Text(kAddSeparatorMenuText);

    FontIcon icon;
    icon.FontFamily(
        winrt::Windows::UI::Xaml::Media::FontFamily(
            L"Segoe Fluent Icons"));
    icon.Glyph(L"\xE710");
    icon.FontSize(16);
    item.Icon(icon);
    winrt::event_token clickToken =
        item.Click(
            RoutedEventHandler{&OnAddSeparatorMenuClick});

    MenuFlyoutSeparator divider;
    divider.Name(kAddSeparatorMenuDividerName);
    divider.Tag(
        winrt::box_value(
            winrt::hstring{kAddSeparatorMenuDividerName}));

    g_menuFlyoutItemBaseVectorAppendOriginal(vectorThis, item);
    g_injectedAddSeparatorMenuItem = item;
    g_injectedAddSeparatorClickToken = clickToken;
    g_menuFlyoutItemBaseVectorAppendOriginal(vectorThis, divider);

    g_taskbarSettingsMenuInjected = true;
    Wh_Log(L"[NATIVE-ADD] Injected Add separator into taskbar context menu");
}

static void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const& item) {
    if (!g_unloading &&
        g_taskbarSettingsMenuDepth > 0 &&
        !g_taskbarSettingsMenuInjected) {
        try {
            if (!IsMenuSeparator(item) &&
                !IsNamedMenuItem(item, kAddSeparatorMenuName) &&
                !IsNamedMenuItem(item, kAddSeparatorMenuDividerName)) {
                AppendAddSeparatorMenuItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"[NATIVE-ADD] Menu append inspection failed; skipping injection");
        }
    }

    g_menuFlyoutItemBaseVectorAppendOriginal(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_taskbarSettingsMenuInjected = false;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        if (outer && !g_taskbarSettingsMenuInjected && !g_unloading) {
            Wh_Log(L"[NATIVE-ADD] No taskbar menu append observed; injection skipped");
        }
        g_taskbarSettingsMenuDepth--;
    }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(
        FrameworkElement const& target,
        void* taskbarSettings,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args,
        unsigned long long options);
static ContextMenus_ShowTaskbarSettingsContextMenu_t
    g_contextMenusShowTaskbarSettingsContextMenuOriginal;

static void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    FrameworkElement const& target,
    void* taskbarSettings,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    ScopedTaskbarSettingsMenuBuild scopedBuild;
    g_contextMenusShowTaskbarSettingsContextMenuOriginal(
        target,
        taskbarSettings,
        args,
        options);
}

// Modern per-item taskbar context-menu paths.
//
// Different Windows 11 builds route the request through slightly different
// Taskbar.View entry points, so retain all three proven hooks. Every route feeds
// the same tiny handler: identify our separator, suppress the stock JumpView,
// and show a one-item explorer-hosted XAML MenuFlyout. The first route that
// reaches the handler opens the flyout; the remaining callbacks for the same
// request only suppress their native path.
using TaskListButton_OnContextRequested_t =
    void(WINAPI*)(
        void* pThis,
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskListButton_OnContextRequested_t
    g_taskListButtonOnContextRequestedOriginal;

using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskListButtonHandlers_HandleContextRequested_t
    g_taskListButtonHandlersHandleContextRequestedOriginal;

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(
        void* pThis,
        winrt::Windows::UI::Xaml::UIElement const& sender,
        winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args);

static TaskbarResources_OnTaskListButtonContextRequested_t
    g_taskbarResourcesOnTaskListButtonContextRequestedOriginal;

static void RemoveSeparatorFromContextMenu(
    std::wstring identity);

static void OnSeparatorContextMenuClosed(
    winrt::Windows::Foundation::IInspectable const&,
    winrt::Windows::Foundation::IInspectable const&) {
    CloseSeparatorContextMenuOnCurrentThread(false);
    g_separatorContextMenuOpen = false;
}

static void OnSeparatorUnpinMenuClick(
    winrt::Windows::Foundation::IInspectable const& sender,
    RoutedEventArgs const&) {
    // The target TaskListButton may disappear before FlyoutBase::Closed is
    // delivered. Release the de-duplication gate at command dispatch too.
    g_separatorContextMenuOpen = false;

    if (g_unloading ||
        g_internalCleanupInProgress.load()) {
        return;
    }

    try {
        auto item =
            sender.try_as<
                winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem>();
        if (!item) {
            return;
        }

        auto tag = item.Tag();
        if (!tag) {
            return;
        }

        winrt::hstring identity =
            winrt::unbox_value<winrt::hstring>(tag);
        if (identity.empty()) {
            return;
        }

        RemoveSeparatorFromContextMenu(
            std::wstring(identity.c_str(), identity.size()));
    } catch (...) {
        Wh_Log(L"[CTX] Separator Unpin callback failed");
    }
}

static void ShowSeparatorContextMenu(
    const FrameworkElement& target,
    const SeparatorSetting& separator) {
    if (!target ||
        separator.identity.empty() ||
        g_unloading) {
        return;
    }

    bool expected = false;
    if (!g_separatorContextMenuOpen.compare_exchange_strong(
            expected,
            true)) {
        return;
    }

    // The click callback releases the process-wide gate before XAML always
    // delivers Closed. If this UI thread still owns that older menu, retire it
    // before publishing a replacement.
    CloseSeparatorContextMenuOnCurrentThread(true);

    try {
        using namespace winrt::Windows::UI::Xaml::Controls;
        using namespace winrt::Windows::UI::Xaml::Controls::Primitives;

        MenuFlyout menu;
        menu.Placement(FlyoutPlacementMode::Top);

        MenuFlyoutItem unpin;
        unpin.Text(L"Remove separator");
        unpin.Tag(
            winrt::box_value(
                winrt::hstring{separator.identity}));

        FontIcon icon;
        icon.FontFamily(
            winrt::Windows::UI::Xaml::Media::FontFamily(
                L"Segoe Fluent Icons"));
        // Segoe Fluent Icons: Remove (minus sign).
        icon.Glyph(L"\xE738");
        icon.FontSize(16);
        unpin.Icon(icon);
        winrt::event_token clickToken =
            unpin.Click(
                RoutedEventHandler{
                    &OnSeparatorUnpinMenuClick});

        menu.Items().Append(unpin);
        winrt::event_token closedToken =
            menu.Closed(
                winrt::Windows::Foundation::
                    EventHandler<winrt::Windows::Foundation::IInspectable>{
                        &OnSeparatorContextMenuClosed});

        // Publish ownership before ShowAt: if XAML partially opens the flyout
        // and then throws, the catch path can still detach both delegates.
        g_openSeparatorContextMenu = menu;
        g_openSeparatorContextMenuItem = unpin;
        g_openSeparatorContextMenuClickToken = clickToken;
        g_openSeparatorContextMenuClosedToken = closedToken;

        menu.ShowAt(target);
        Wh_Log(
            L"[CTX] Opened separator context menu identity='%s'",
            separator.identity.c_str());
    } catch (...) {
        CloseSeparatorContextMenuOnCurrentThread(true);
        g_separatorContextMenuOpen = false;
        Wh_Log(L"[CTX] Failed to show separator context menu");
    }
}

static bool TryHandleSeparatorContextRequest(
    const FrameworkElement& element,
    const SeparatorSetting* knownSeparator = nullptr) {
    if (g_unloading || !element) {
        return false;
    }

    SeparatorSetting separator;
    if (knownSeparator) {
        separator = *knownSeparator;
    } else if (!GetSeparatorForElement(
                   element,
                   &separator)) {
        return false;
    }

    ShowSeparatorContextMenu(
        element,
        separator);
    return true;
}

static void WINAPI TaskListButton_OnContextRequested_Hook(
    void* pThis,
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    FrameworkElement element = nullptr;
    SeparatorSetting separator;

    if (!g_unloading &&
        GetSeparatorForTaskListButton(
            pThis,
            &element,
            &separator)) {
        TryHandleSeparatorContextRequest(
            element,
            &separator);
        return;
    }

    g_taskListButtonOnContextRequestedOriginal(
        pThis,
        sender,
        args);
}

static void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    auto element = sender.try_as<FrameworkElement>();
    if (TryHandleSeparatorContextRequest(element)) {
        return;
    }

    g_taskListButtonHandlersHandleContextRequestedOriginal(
        sender,
        args);
}

static void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis,
    winrt::Windows::UI::Xaml::UIElement const& sender,
    winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const& args) {
    auto element = sender.try_as<FrameworkElement>();
    if (TryHandleSeparatorContextRequest(element)) {
        return;
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

    // Layout/property updates below can synchronously re-enter this hook. The
    // nested frame has already run the original function; only suppress our
    // own registry mutations until the outer frame completes.
    if (g_updatingSeparatorVisualStates) {
        return;
    }
    g_updatingSeparatorVisualStates = true;
    SeparatorVisualStateUpdateGuard updateGuard;

    FrameworkElement element =
        GetTaskListButtonElement(pThis);
    SeparatorSetting separator;
    bool isSeparator =
        GetSeparatorForElement(
            element,
            &separator);

    PruneExpiredSeparatorVisualStates();
    auto stateIt = FindSeparatorVisualState(pThis);

    // ItemsRepeater recycles TaskListButton containers. Undo every property
    // owned by this mod as soon as a container stops representing a separator.
    if (!isSeparator || !element || g_unloading) {
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

    ApplySeparatorMaxWidthOverride(
        state,
        element,
        separator);

    // If unload began while width was being applied, undo this button instead
    // of applying the remaining separator-only properties.
    if (g_unloading) {
        RestoreSeparatorVisualState(stateIt);
        return;
    }

    FrameworkElement iconPanel =
        FindChildByName(element, L"IconPanel");
    CenterSeparatorIcon(state, iconPanel);
    SuppressSeparatorHoverChrome(state, element, iconPanel);
    UpdateSeparatorAnchorHint(state, element, separator);
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
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))"
            },
            &g_contextMenusShowTaskbarSettingsContextMenuOriginal,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )"
            },
            &g_menuFlyoutItemBaseVectorAppendOriginal,
            MenuFlyoutItemBaseVector_Append_Hook,
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
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"
            },
            &g_taskListButtonOnDragCompletedGestureOriginal,
            TaskListButton_OnDragCompletedGesture_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskListButtonOnContextRequestedOriginal,
            TaskListButton_OnContextRequested_Hook,
            true,
        },
        {
            {
                LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskListButtonHandlersHandleContextRequestedOriginal,
            TaskListButtonHandlers_HandleContextRequested_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"
            },
            &g_taskbarResourcesOnTaskListButtonContextRequestedOriginal,
            TaskbarResources_OnTaskListButtonContextRequested_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            taskbarViewHooks,
            ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"[STYLE] HookSymbols(Taskbar view) failed");
        return false;
    }

    if (!g_contextMenusShowTaskbarSettingsContextMenuOriginal ||
        !g_menuFlyoutItemBaseVectorAppendOriginal) {
        Wh_Log(
            L"[STYLE] Add-separator taskbar menu route unavailable");
        return false;
    }

    const bool hasContextRequestRoute =
        g_taskListButtonOnContextRequestedOriginal ||
        g_taskListButtonHandlersHandleContextRequestedOriginal ||
        g_taskbarResourcesOnTaskListButtonContextRequestedOriginal;

    Wh_Log(
        L"[STYLE] Context routes: button=%d handlers=%d resources=%d",
        g_taskListButtonOnContextRequestedOriginal ? 1 : 0,
        g_taskListButtonHandlersHandleContextRequestedOriginal ? 1 : 0,
        g_taskbarResourcesOnTaskListButtonContextRequestedOriginal ? 1 : 0);

    if (!hasContextRequestRoute) {
        Wh_Log(L"[STYLE] No supported Taskbar.View context-request route resolved");
        return false;
    }

    Wh_Log(L"[STYLE] Taskbar view hooks installed; widthMode=MaxWidth");

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_loadLibraryExWOriginal;

static bool HookTaskbarDllSymbols(HMODULE taskbarDll);
static HWND FindCurrentProcessTaskbarWnd();
static void StartBackendWorker();

class HookInstallerScope {
public:
    HookInstallerScope() {
        if (g_hookInstallationClosed.load(std::memory_order_acquire)) {
            return;
        }

        g_hookInstallersInFlight.fetch_add(
            1,
            std::memory_order_acq_rel);

        // Close the race where unload begins between the first closed check and
        // publishing this installer as in-flight.
        if (g_hookInstallationClosed.load(std::memory_order_acquire)) {
            g_hookInstallersInFlight.fetch_sub(
                1,
                std::memory_order_acq_rel);
            return;
        }

        active_ = true;
    }

    ~HookInstallerScope() {
        if (active_) {
            g_hookInstallersInFlight.fetch_sub(
                1,
                std::memory_order_acq_rel);
        }
    }

    explicit operator bool() const {
        return active_;
    }

private:
    bool active_ = false;
};

static void DrainHookInstallers() {
    constexpr ULONGLONG kDrainTimeoutMs = 2000;
    constexpr DWORD kDrainSleepMs = 5;

    const ULONGLONG started = GetTickCount64();
    while (g_hookInstallersInFlight.load(
               std::memory_order_acquire) != 0 &&
           GetTickCount64() - started < kDrainTimeoutMs) {
        Sleep(kDrainSleepMs);
    }

    const unsigned int remaining =
        g_hookInstallersInFlight.load(std::memory_order_acquire);
    if (remaining != 0) {
        Wh_Log(
            L"[HOOK] Timed out draining %u in-flight hook installer(s); proceeding with unload",
            remaining);
    }
}

static bool InstallTaskbarViewHooks(
    HMODULE module,
    bool applyOperations) {
    HookInstallerScope installer;
    if (!installer || g_unloading) {
        return false;
    }

    if (g_taskbarViewDllHooked.load(std::memory_order_acquire)) {
        return true;
    }

    bool expected = false;
    if (!g_taskbarViewHookInstallationClaimed.compare_exchange_strong(
            expected,
            true,
            std::memory_order_acq_rel)) {
        return g_taskbarViewDllHooked.load(std::memory_order_acquire);
    }

    // No mod mutex is held while entering the Windhawk symbol/hook engine.
    if (!HookTaskbarViewDllSymbols(module)) {
        return false;
    }

    // If unload started while symbol resolution was in progress, don't enter
    // another engine operation from this late installer.
    if (g_hookInstallationClosed.load(std::memory_order_acquire) ||
        g_unloading) {
        return false;
    }

    if (applyOperations && !Wh_ApplyHookOperations()) {
        Wh_Log(L"[STYLE] Wh_ApplyHookOperations failed");
        return false;
    }

    g_taskbarViewDllHooked.store(true, std::memory_order_release);
    return true;
}

static bool InstallTaskbarDllHooks(
    HMODULE module,
    bool applyOperations) {
    HookInstallerScope installer;
    if (!installer || g_unloading) {
        return false;
    }

    if (g_taskbarDllHooked.load(std::memory_order_acquire)) {
        return true;
    }

    bool expected = false;
    if (!g_taskbarDllHookInstallationClaimed.compare_exchange_strong(
            expected,
            true,
            std::memory_order_acq_rel)) {
        return g_taskbarDllHooked.load(std::memory_order_acquire);
    }

    // Resolve every taskbar.dll symbol in one HookSymbols call. Besides being
    // faster, this avoids invalidating WindhawkUtils' per-module symbol cache.
    if (!HookTaskbarDllSymbols(module)) {
        return false;
    }

    if (g_hookInstallationClosed.load(std::memory_order_acquire) ||
        g_unloading) {
        return false;
    }

    if (applyOperations && !Wh_ApplyHookOperations()) {
        Wh_Log(L"[TASKBAR] Wh_ApplyHookOperations failed");
        return false;
    }

    g_taskbarDllHooked.store(true, std::memory_order_release);
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

    if (!module ||
        g_hookInstallationClosed.load(std::memory_order_acquire) ||
        g_unloading) {
        return module;
    }

    if (!g_taskbarViewDllHooked &&
        GetTaskbarViewModuleHandle() == module) {
        Wh_Log(
            L"[STYLE] Taskbar view module loaded: %s",
            lpLibFileName ? lpLibFileName : L"<unknown>");

        InstallTaskbarViewHooks(module, true);
    }

    if (!g_taskbarDllHooked.load(std::memory_order_acquire) &&
        GetModuleHandleW(L"taskbar.dll") == module) {
        Wh_Log(
            L"[TASKBAR] taskbar.dll loaded; installing taskbar hooks");
        InstallTaskbarDllHooks(module, true);

        // Do not scan windows or create the backend worker from inside a
        // LoadLibraryExW hook. TrayUI::StartTaskbar and Wh_ModAfterInit own
        // backend readiness/startup outside the loader-lock-sensitive path.
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
// Taskbar.View owns modern context requests and the drag-completion boundary;
// those are handled above. taskbar.dll is used only for model-level semantic
// actions that already expose the exact ITaskGroup:
//
// - CTaskListWnd::HandleClick:
//     swallow activation for our separator groups.
// - CTaskListWnd::OnContextMenu:
//     suppress the legacy separator context-menu route.
// - CTaskBtnGroup::ShouldShowToolTip:
//     legacy tooltip gate fallback.
// - CTaskGroup::GetToolTipText:
//     directly report no tooltip text for separator groups.
//
// CTaskListWnd::TryMoveGroup is observed (never blocked) so successful native
// pinned-item drags can be mirrored into the mod-owned state file. Separator
// Unpin is a direct command from our Explorer-hosted MenuFlyout to desired state.
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
    std::shared_lock lock(g_settingsMutex);

    for (const auto& separator : g_settings.separators) {
        if (appIdView ==
            std::wstring_view(separator.identity)) {
            return true;
        }
    }

    for (const auto& separator : g_storedSeparatorSettings) {
        if (appIdView ==
            std::wstring_view(separator.identity)) {
            return true;
        }
    }

    return appIdView ==
        std::wstring_view(
            g_refreshPulseSetting.identity);
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
        Wh_Log(L"[CTX] Suppressed legacy separator context menu");
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


using TaskListWnd_IsPinned_t =
    bool(WINAPI*)(void* pThis, void* taskGroup);
static TaskListWnd_IsPinned_t
    g_taskListWndIsPinned;

using TaskListWnd_GetRelativeTaskOrder_t =
    int(WINAPI*)(void* pThis, void* taskGroup);
static TaskListWnd_GetRelativeTaskOrder_t
    g_taskListWndGetRelativeTaskOrder;
static TaskListWnd_GetRelativeTaskOrder_t
    g_taskListWndMultiGetRelativeTaskOrder;

using TaskListWnd_TryMoveGroup_t =
    bool(WINAPI*)(void* pThis, void* taskGroup, unsigned int index);
static TaskListWnd_TryMoveGroup_t
    g_taskListWndTryMoveGroupOriginal;

using TaskListWnd_HandleWinNumHotKey_t =
    HRESULT(WINAPI*)(void* pThis, short index, unsigned short flags);
static TaskListWnd_HandleWinNumHotKey_t
    g_taskListWndHandleWinNumHotKeyOriginal;

using TaskListWndMulti_IsPinned_t =
    bool(WINAPI*)(void* pThis, void* taskGroup);
static TaskListWndMulti_IsPinned_t
    g_taskListWndMultiIsPinned;

using TaskListWndMulti_TryMoveGroup_t =
    bool(WINAPI*)(void* pThis, void* taskGroup, unsigned int index);
static TaskListWndMulti_TryMoveGroup_t
    g_taskListWndMultiTryMoveGroupOriginal;

static thread_local int g_tryMoveGroupHookDepth = 0;

static std::wstring CopyTaskGroupAppId(void* taskGroup) {
    if (!taskGroup || !g_taskGroupGetAppIdAddress) {
        return {};
    }

    using CTaskGroup_GetAppID_t =
        const wchar_t*(WINAPI*)(void* pThis);
    auto getAppId =
        reinterpret_cast<CTaskGroup_GetAppID_t>(
            g_taskGroupGetAppIdAddress);

    const wchar_t* appId = getAppId(taskGroup);
    return appId ? std::wstring(appId) : std::wstring();
}

static void ApplyPinnedMoveToSnapshot(
    Settings& settings,
    std::wstring_view movedAppId,
    int oldIndex,
    int newIndex) {
    if (oldIndex == newIndex) {
        return;
    }

    SeparatorSetting* movedSeparator = nullptr;
    for (auto& separator : settings.separators) {
        if (separator.identity == movedAppId) {
            movedSeparator = &separator;
            break;
        }
    }

    for (auto& separator : settings.separators) {
        if (&separator == movedSeparator || separator.targetIndex < 0) {
            continue;
        }

        if (oldIndex < newIndex) {
            if (separator.targetIndex > oldIndex &&
                separator.targetIndex <= newIndex) {
                separator.targetIndex--;
            }
        } else {
            if (separator.targetIndex >= newIndex &&
                separator.targetIndex < oldIndex) {
                separator.targetIndex++;
            }
        }
    }

    if (movedSeparator) {
        movedSeparator->targetIndex = newIndex;
    }
}


// -----------------------------------------------------------------------------
// Win+number compensation.
//
// CTaskListWnd::HandleWinNumHotKey uses a zero-based taskbar-group index. The
// stock implementation counts our genuine pinned separator groups, which makes
// every app to the right of a separator shift by one Win+number slot. Translate
// the requested logical app index to the corresponding physical taskbar-group
// index by skipping every resolved separator position.
//
// targetIndex == -1 is intentionally ignored. It means the separator is
// unanchored: PinManager leaves it at the native right edge until the user drags
// it once. Since no existing taskbar app lies to the right of a freshly appended
// unanchored slot, it must not shift any existing Win+number target. The first
// native separator drag records a concrete targetIndex, after which normal
// compensation applies automatically.
// -----------------------------------------------------------------------------

static short CompensateWinNumHotKeyIndex(short logicalIndex) {
    // Win+1..Win+0 reaches this layer as zero-based 0..9. Leave any unexpected
    // internal/special value alone rather than changing undocumented behavior.
    if (logicalIndex < 0 || logicalIndex > 9 ||
        g_unloading ||
        g_internalCleanupInProgress.load(std::memory_order_acquire)) {
        return logicalIndex;
    }

    // While the backend is creating/removing/recovering pins, desired state can
    // briefly differ from the visible taskbar. Stock behavior is safer for that
    // very short transition. Native user drags don't clear this latch: their
    // new positions are mirrored synchronously by TryMoveGroup below, so Win+N
    // compensation follows a completed drag immediately.
    if (!g_backendHasConverged.load(std::memory_order_acquire)) {
        return logicalIndex;
    }

    std::vector<int> separatorPositions;
    {
        std::shared_lock lock(g_settingsMutex);
        separatorPositions.reserve(g_settings.separators.size());

        for (const auto& separator : g_settings.separators) {
            if (separator.targetIndex >= 0) {
                separatorPositions.push_back(separator.targetIndex);
            }
        }
    }

    if (separatorPositions.empty()) {
        return logicalIndex;
    }

    std::sort(
        separatorPositions.begin(),
        separatorPositions.end());

    int physicalIndex = logicalIndex;
    for (int separatorIndex : separatorPositions) {
        if (separatorIndex <= physicalIndex) {
            ++physicalIndex;
        } else {
            // Positions are sorted, so no later separator can affect this
            // target unless an earlier one has already shifted it into range.
            break;
        }
    }

    if (physicalIndex > SHRT_MAX) {
        return logicalIndex;
    }

    return static_cast<short>(physicalIndex);
}

static HRESULT WINAPI TaskListWnd_HandleWinNumHotKey_Hook(
    void* pThis,
    short index,
    unsigned short flags) {
    short compensatedIndex =
        CompensateWinNumHotKeyIndex(index);

    if (compensatedIndex != index) {
        Wh_Log(
            L"[WINNUM] logical=%d physical=%d flags=0x%04X",
            static_cast<int>(index),
            static_cast<int>(compensatedIndex),
            static_cast<unsigned int>(flags));
    }

    return g_taskListWndHandleWinNumHotKeyOriginal(
        pThis,
        compensatedIndex,
        flags);
}


// -----------------------------------------------------------------------------
// Native taskbar mutation persistence.
//
// Windhawk no longer owns separator position/list configuration. TryMoveGroup
// mirrors native reorder operations into the in-memory separator state and
// immediately queues the backend persistence pass. The optional Taskbar.View
// drag-completion hook only refreshes presentation. Add and Remove have their
// own discrete commit points and queue the same worker; live taskbar UI
// callbacks never perform synchronous disk I/O.
// -----------------------------------------------------------------------------

static void FlushPendingSeparatorStateSynchronously() {
    Settings snapshot;

    {
        std::lock_guard<std::mutex> mutationLock(
            g_settingsMutationMutex);

        if (!g_separatorStateDirty) {
            return;
        }

        {
            std::shared_lock lock(g_settingsMutex);
            snapshot = g_settings;
        }

        // Teardown-only fallback. Live taskbar paths never call this function.
        if (SaveSeparatorStateFile(snapshot)) {
            g_separatorStateDirty = false;
            Wh_Log(L"[STATE] Persisted pending separator state during teardown");
        } else {
            Wh_Log(L"[STATE] Failed to persist pending separator state during teardown");
        }
    }
}

static void RecordNativePinnedMove(
    std::wstring_view movedAppId,
    int oldIndex,
    int newIndex) {
    if (oldIndex < 0 ||
        newIndex < 0 ||
        oldIndex == newIndex) {
        return;
    }

    bool touchesSeparator = false;

    {
        std::lock_guard<std::mutex> mutationLock(
            g_settingsMutationMutex);
        std::unique_lock settingsLock(g_settingsMutex);

        Settings before = g_settings;

        ApplyPinnedMoveToSnapshot(
            g_settings,
            movedAppId,
            oldIndex,
            newIndex);

        touchesSeparator =
            !SettingsSnapshotsEqual(
                before,
                g_settings);

        if (!touchesSeparator) {
            return;
        }

        // Windows already performed this move. Mirror it into the applied and
        // stale-artifact snapshots immediately so the backend never "corrects"
        // the user's native taskbar order while the drag is still in progress.
        ApplyPinnedMoveToSnapshot(
            g_appliedSettings,
            movedAppId,
            oldIndex,
            newIndex);

        for (auto& stored :
             g_storedSeparatorSettings) {
            if (stored.identity == movedAppId) {
                stored.targetIndex = newIndex;
                continue;
            }

            if (stored.targetIndex < 0) {
                continue;
            }

            if (oldIndex < newIndex) {
                if (stored.targetIndex > oldIndex &&
                    stored.targetIndex <= newIndex) {
                    stored.targetIndex--;
                }
            } else if (
                stored.targetIndex >= newIndex &&
                stored.targetIndex < oldIndex) {
                stored.targetIndex++;
            }
        }

        g_separatorStateDirty = true;
    }

    Wh_Log(
        L"[NATIVE-MOVE] staged native pinned move appId='%s' %d -> %d",
        std::wstring(movedAppId).c_str(),
        oldIndex,
        newIndex);

    // Persist through the long-lived worker even if the optional
    // OnDragCompletedGesture presentation callback isn't available.
    QueueBackendWork();
}

static bool RunTryMoveGroupObserver(
    TaskListWnd_TryMoveGroup_t original,
    TaskListWnd_IsPinned_t isPinnedFunction,
    TaskListWnd_GetRelativeTaskOrder_t getRelativeTaskOrder,
    const wchar_t* route,
    void* pThis,
    void* taskGroup,
    unsigned int index) {
    const bool outer =
        g_tryMoveGroupHookDepth++ == 0;

    int oldIndex = -1;
    int newIndex = -1;
    bool pinned = false;
    std::wstring appId;

    if (!g_unloading &&
        outer &&
        taskGroup &&
        getRelativeTaskOrder &&
        isPinnedFunction) {
        oldIndex =
            getRelativeTaskOrder(
                pThis,
                taskGroup);
        pinned =
            isPinnedFunction(
                pThis,
                taskGroup);
        appId =
            CopyTaskGroupAppId(taskGroup);
    }

    bool result =
        original(
            pThis,
            taskGroup,
            index);

    if (!g_unloading &&
        outer &&
        taskGroup &&
        getRelativeTaskOrder) {
        newIndex =
            getRelativeTaskOrder(
                pThis,
                taskGroup);
    }

    --g_tryMoveGroupHookDepth;

    if (!g_unloading &&
        outer &&
        result &&
        pinned &&
        !appId.empty() &&
        oldIndex >= 0 &&
        newIndex >= 0 &&
        oldIndex != newIndex) {
        Wh_Log(
            L"[NATIVE-MOVE] %s appId='%s' relative=%d -> %d argTarget=%u",
            route,
            appId.c_str(),
            oldIndex,
            newIndex,
            index);

        RecordNativePinnedMove(
            appId,
            oldIndex,
            newIndex);
    }

    return result;
}

static bool WINAPI TaskListWnd_TryMoveGroup_Hook(
    void* pThis,
    void* taskGroup,
    unsigned int index) {
    return RunTryMoveGroupObserver(
        g_taskListWndTryMoveGroupOriginal,
        g_taskListWndIsPinned,
        g_taskListWndGetRelativeTaskOrder,
        L"CTaskListWnd::TryMoveGroup",
        pThis,
        taskGroup,
        index);
}

static bool WINAPI TaskListWndMulti_TryMoveGroup_Hook(
    void* pThis,
    void* taskGroup,
    unsigned int index) {
    return RunTryMoveGroupObserver(
        reinterpret_cast<TaskListWnd_TryMoveGroup_t>(
            g_taskListWndMultiTryMoveGroupOriginal),
        reinterpret_cast<TaskListWnd_IsPinned_t>(
            g_taskListWndMultiIsPinned),
        g_taskListWndMultiGetRelativeTaskOrder,
        L"CTaskListWndMulti::TryMoveGroup",
        pThis,
        taskGroup,
        index);
}

static void ShiftSeparatorTargetsAfterRemoval(
    Settings& settings,
    std::wstring_view removedIdentity,
    int removedIndex) {
    settings.separators.erase(
        std::remove_if(
            settings.separators.begin(),
            settings.separators.end(),
            [removedIdentity](const SeparatorSetting& separator) {
                return separator.identity ==
                    removedIdentity;
            }),
        settings.separators.end());

    if (removedIndex >= 0) {
        for (auto& separator :
             settings.separators) {
            if (separator.targetIndex >
                removedIndex) {
                separator.targetIndex--;
            }
        }
    }

    for (size_t i = 0;
         i < settings.separators.size();
         i++) {
        settings.separators[i].sourceIndex =
            static_cast<int>(i);
        settings.separators[i].ordinal =
            static_cast<int>(i) + 1;
    }
}


static void RemoveSeparatorFromContextMenu(
    std::wstring identity) {
    if (identity.empty() ||
        g_unloading ||
        g_internalCleanupInProgress.load()) {
        return;
    }

    Settings updated;
    int removedIndex = -1;

    {
        std::lock_guard<std::mutex> mutationLock(
            g_settingsMutationMutex);

        {
            std::shared_lock lock(g_settingsMutex);
            updated = g_settings;

            const SeparatorSetting* existing =
                FindSeparatorByIdentity(
                    g_settings,
                    identity);
            if (!existing) {
                return;
            }

            removedIndex = existing->targetIndex;
        }

        ShiftSeparatorTargetsAfterRemoval(
            updated,
            identity,
            removedIndex);

        {
            std::unique_lock lock(g_settingsMutex);
            g_settings = updated;
            g_backendHasConverged = false;
        }

        // The backend worker persists this snapshot before it is allowed to
        // unpin anything, preserving persist-before-mutate without blocking
        // the taskbar UI thread on FlushFileBuffers/MoveFileEx.
        g_separatorStateDirty = true;
    }

    Wh_Log(
        L"[NATIVE-UNPIN] Removed separator from desired state identity='%s' index=%d",
        identity.c_str(),
        removedIndex);

    // Keep the old applied/stored snapshots until the backend has actually
    // unpinned the shortcut. That keeps the still-visible button recognized,
    // styled, and inert during the short convergence window.
    QueueBackendWork();
}

using TrayUI_StartTaskbar_t =
    void(WINAPI*)(void* pThis);

static TrayUI_StartTaskbar_t
    g_trayUIStartTaskbarOriginal;

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    g_trayUIStartTaskbarOriginal(pThis);

    if (g_unloading) {
        return;
    }

    Wh_Log(L"[LIFECYCLE] TrayUI::StartTaskbar completed");
    StartBackendWorker();
}

static bool HookTaskbarDllSymbols(HMODULE taskbarDll) {
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"
            },
            &g_trayUIStartTaskbarOriginal,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {
                LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"
            },
            &g_taskGroupGetAppIdAddress,
            nullptr,
        },
        {
            {
                LR"(public: virtual bool __cdecl CTaskListWnd::IsPinned(struct ITaskGroup *))"
            },
            &g_taskListWndIsPinned,
            nullptr,
        },
        {
            {
                LR"(public: virtual int __cdecl CTaskListWnd::GetRelativeTaskOrder(struct ITaskGroup *))"
            },
            &g_taskListWndGetRelativeTaskOrder,
            nullptr,
        },
        {
            {
                LR"(public: virtual long __cdecl CTaskListWnd::HandleWinNumHotKey(short,unsigned short))"
            },
            &g_taskListWndHandleWinNumHotKeyOriginal,
            TaskListWnd_HandleWinNumHotKey_Hook,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl CTaskListWnd::TryMoveGroup(struct ITaskGroup *,unsigned int))"
            },
            &g_taskListWndTryMoveGroupOriginal,
            TaskListWnd_TryMoveGroup_Hook,
        },
        {
            {
                LR"(public: virtual bool __cdecl CTaskListWndMulti::IsPinned(struct ITaskGroup *))"
            },
            &g_taskListWndMultiIsPinned,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl CTaskListWndMulti::GetRelativeTaskOrder(struct ITaskGroup *))"
            },
            &g_taskListWndMultiGetRelativeTaskOrder,
            nullptr,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl CTaskListWndMulti::TryMoveGroup(struct ITaskGroup *,unsigned int))"
            },
            &g_taskListWndMultiTryMoveGroupOriginal,
            TaskListWndMulti_TryMoveGroup_Hook,
            true,
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
            true,
        },
        {
            {
                LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"
            },
            &g_taskListWndOnContextMenuOriginal,
            TaskListWnd_OnContextMenu_Hook,
            true,
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
        Wh_Log(L"[TASKBAR] HookSymbols(taskbar.dll) failed");
        return false;
    }

    Wh_Log(L"[TASKBAR] taskbar.dll hooks installed in one symbol pass");
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
    bool useCmdFallback = !FileExists(target);

    if (useCmdFallback) {
        target = JoinPath(systemDirectory, L"cmd.exe");
        if (!FileExists(target)) {
            shellLink->Release();
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
    }

    // systray.exe is an inert Windows stub, so a stranded shortcut doesn't
    // launch a console or perform an action if it is clicked. It isn't a
    // documented Windows 11 contract, so retain a hidden, immediately exiting
    // command processor as a compatibility fallback.
    hr = shellLink->SetPath(target.c_str());

    if (SUCCEEDED(hr) && useCmdFallback) {
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

static bool IsBackendStopRequested();
static bool IsBackendPassSuperseded();

static bool UnpinAndDeleteShortcut(
    IPinManagerInterop3* pinManager,
    const std::wstring& shortcutPath,
    const wchar_t* logCategory,
    bool recoverMissingPin = true) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = GetPidlForPath(shortcutPath, &pidl);

    if (FAILED(hr) || !pidl) {
        Wh_Log(
            L"[%s] Can't resolve owned shortcut '%s'; keeping it",
            logCategory,
            shortcutPath.c_str());
        return false;
    }

    HRESULT unpinHr =
        pinManager->UnpinTaskbarItem(
            pidl,
            PMC_JUMPVIEWBROKER);

    // A shortcut can exist even if Explorer died before the matching pin.
    // Establish a known pinned state, then remove it, so such backing files
    // don't become permanent false failures in future cleanup passes.
    HRESULT pinHr = S_OK;
    if (FAILED(unpinHr) && recoverMissingPin) {
        pinHr =
            pinManager->PinItemFromTrustedCaller(
                pidl,
                PMC_TASKBANDPIN);
        if (SUCCEEDED(pinHr)) {
            unpinHr =
                pinManager->UnpinTaskbarItem(
                    pidl,
                    PMC_JUMPVIEWBROKER);
        }
    }

    CoTaskMemFree(pidl);

    if (FAILED(unpinHr)) {
        if (recoverMissingPin) {
            Wh_Log(
                L"[%s] Failed to remove owned shortcut '%s' "
                L"pinHr=0x%08X unpinHr=0x%08X",
                logCategory,
                shortcutPath.c_str(),
                static_cast<unsigned int>(pinHr),
                static_cast<unsigned int>(unpinHr));
        } else {
            Wh_Log(
                L"[%s] Failed to unpin owned shortcut '%s' "
                L"hr=0x%08X; recovery disabled after hook removal",
                logCategory,
                shortcutPath.c_str(),
                static_cast<unsigned int>(unpinHr));
        }
        return false;
    }

    return DeleteFileIfPresent(shortcutPath);
}

static bool CleanupStaleSeparatorShortcuts(
    IPinManagerInterop3* pinManager,
    const Settings& settings) {
    std::vector<std::wstring> stalePaths;

    for (const auto& path : EnumerateSeparatorShortcuts()) {
        if (!IsIdentityInSettings(
                settings,
                GetShortcutIdentity(path))) {
            stalePaths.push_back(path);
        }
    }

    if (stalePaths.empty()) {
        return true;
    }

    Wh_Log(
        L"[SYNC] Removing %zu shortcut(s) not present in desired settings",
        stalePaths.size());

    bool success = true;
    for (auto it = stalePaths.rbegin();
         it != stalePaths.rend();
         ++it) {
        if (IsBackendPassSuperseded()) {
            return false;
        }

        if (!UnpinAndDeleteShortcut(
                pinManager,
                *it,
                L"SYNC")) {
            success = false;
        }
    }

    return success;
}

// -----------------------------------------------------------------------------
// Creation / positioning.
// -----------------------------------------------------------------------------

static bool PrepareSeparatorFiles(
    const Settings& settings,
    const Settings& appliedSettings,
    const SeparatorSetting& refreshPulse,
    std::vector<std::wstring>* identitiesToPin) {
    if (IsBackendPassSuperseded()) {
        return false;
    }

    identitiesToPin->clear();

    if (settings.separators.empty()) {
        Wh_Log(L"[PIN] No separators configured");
        return true;
    }

    // The icon file is shared by every separator shortcut. Rewriting it during
    // a live settings change needlessly invalidates Explorer's icon-cache
    // assumptions, so leave an existing file untouched.
    if (!FileExists(g_iconPath) &&
        !WriteBinaryFile(
            g_iconPath,
            kSeparatorIcon,
            static_cast<DWORD>(sizeof(kSeparatorIcon)))) {
        return false;
    }

    // Stable separator shortcuts are immutable while their identity survives.
    // Overwriting a pinned .lnk in place can make Explorer keep the taskbar
    // button while losing its cached glyph. Create only genuinely new/missing
    // backing files; survivors keep the exact file Windows already knows.
    for (const auto& separator : settings.separators) {
        if (IsBackendPassSuperseded()) {
            return false;
        }

        std::wstring shortcutPath =
            GetSeparatorShortcutPath(separator);
        bool shortcutCreated = false;

        if (!FileExists(shortcutPath)) {
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

            shortcutCreated = true;
        }

        // Pin only identities that aren't part of the last converged snapshot,
        // plus a survivor whose backing file had to be recovered. This keeps
        // ordinary add/remove/reorder operations from re-asserting every
        // already-correct pinned item.
        const SeparatorSetting* appliedSeparator =
            FindSeparatorByStableId(
                appliedSettings,
                separator.stableId);

        if (shortcutCreated ||
            !appliedSeparator ||
            appliedSeparator->identity != separator.identity) {
            identitiesToPin->push_back(
                separator.identity);
        }
    }

    if (IsBackendPassSuperseded()) {
        return false;
    }

    // The refresh helper is intentionally transient, so recreating it is safe.
    std::wstring refreshShortcutPath =
        GetSeparatorShortcutPath(refreshPulse);
    HRESULT refreshHr =
        CreateSeparatorShortcut(
            refreshPulse,
            refreshShortcutPath);

    if (FAILED(refreshHr)) {
        Wh_Log(
            L"[REFRESH] Failed to create pin-pulse shortcut hr=0x%08X",
            static_cast<unsigned int>(refreshHr));
        return false;
    }

    return true;
}

static bool InitializeTaskbarDllHooks() {
    if (HMODULE taskbarDll = GetModuleHandleW(L"taskbar.dll")) {
        return InstallTaskbarDllHooks(taskbarDll, false);
    }

    Wh_Log(
        L"[TASKBAR] taskbar.dll isn't loaded yet; "
        L"installing LoadLibraryExW watcher");
    return InstallLoadLibraryWatcher();
}

static bool IsBackendStopRequested() {
    return g_currentBackendStopEvent &&
        WaitForSingleObject(
            g_currentBackendStopEvent,
            0) == WAIT_OBJECT_0;
}

static bool IsBackendReconcileRequested() {
    return g_currentBackendWakeEvent &&
        WaitForSingleObject(
            g_currentBackendWakeEvent,
            0) == WAIT_OBJECT_0;
}

static bool IsBackendPassSuperseded() {
    return IsBackendStopRequested() ||
        IsBackendReconcileRequested();
}

enum class BackendWaitResult {
    Timeout,
    Stop,
    Reconcile,
    Failed,
};

static BackendWaitResult WaitForBackendStopOrReconcile(
    DWORD timeout) {
    HANDLE events[] = {
        g_currentBackendStopEvent,
        g_currentBackendWakeEvent,
    };

    if (!events[0] || !events[1]) {
        return BackendWaitResult::Failed;
    }

    DWORD result = WaitForMultipleObjects(
        ARRAYSIZE(events),
        events,
        FALSE,
        timeout);

    switch (result) {
        case WAIT_TIMEOUT:
            return BackendWaitResult::Timeout;
        case WAIT_OBJECT_0:
            return BackendWaitResult::Stop;
        case WAIT_OBJECT_0 + 1:
            return BackendWaitResult::Reconcile;
        default:
            return BackendWaitResult::Failed;
    }
}

static bool PinSeparators(
    IPinManagerInterop3* pinManager,
    const Settings& settings,
    const std::vector<std::wstring>& identitiesToPin) {
    bool success = true;

    // Only new/recovered identities need a pin mutation. Existing stable pins
    // are left untouched and are handled solely by the positioning pass.
    for (const auto& separator : settings.separators) {
        if (std::find(
                identitiesToPin.begin(),
                identitiesToPin.end(),
                separator.identity) == identitiesToPin.end()) {
            continue;
        }

        if (IsBackendPassSuperseded()) {
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

static bool PositionSeparators(
    IPinManagerInterop3* pinManager,
    const Settings& settings) {
    bool success = true;

    // Moving in ascending destination order makes multiple requested positions
    // deterministic as items are pulled forward from their initial appended
    // locations.
    std::vector<SeparatorSetting> moveOrder =
        settings.separators;

    std::stable_sort(
        moveOrder.begin(),
        moveOrder.end(),
        [](const SeparatorSetting& a, const SeparatorSetting& b) {
            return a.targetIndex < b.targetIndex;
        });

    for (const auto& separator : moveOrder) {
        if (IsBackendPassSuperseded()) {
            return false;
        }

        // targetIndex=-1 is an intentional unanchored state. Leave the
        // separator where Windows appended it until the user drags it once.
        if (separator.targetIndex < 0) {
            Wh_Log(
                L"[MOVE] Separator #%d left at native appended position",
                separator.ordinal);
            continue;
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

static bool PulseTaskbarPinList(
    IPinManagerInterop3* pinManager,
    const SeparatorSetting& refreshPulse) {
    if (IsBackendPassSuperseded()) {
        return false;
    }

    std::wstring shortcutPath =
        GetSeparatorShortcutPath(refreshPulse);

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

static bool ConvergeSeparatorPins(
    const Settings& settings,
    const Settings& appliedSettings,
    const SeparatorSetting& refreshPulse) {
    if (settings.separators.empty() &&
        EnumerateSeparatorShortcuts().empty()) {
        DeleteFileIfPresent(g_iconPath);
        return true;
    }

    constexpr int kConvergeAttempts = 5;
    constexpr DWORD kInitialRetryDelay = 250;
    constexpr DWORD kRetryBackoff = 2;
    DWORD retryDelay = kInitialRetryDelay;
    bool staleCleanupComplete = false;
    bool filesPrepared = false;
    std::vector<std::wstring> identitiesToPin;

    for (int attempt = 1;
         attempt <= kConvergeAttempts;
         ++attempt) {
        if (IsBackendStopRequested()) {
            Wh_Log(L"[SYNC] Separator reconciliation cancelled");
            return false;
        }

        if (IsBackendReconcileRequested()) {
            Wh_Log(L"[SYNC] Reconciliation superseded by newer settings");
            return false;
        }

        Wh_Log(
            L"[SYNC] Separator reconciliation attempt %d/%d",
            attempt,
            kConvergeAttempts);

        IPinManagerInterop3* pinManager = nullptr;
        HRESULT hr = CreatePinManager(&pinManager);

        bool staleShortcutsRemoved = staleCleanupComplete;
        bool filesReady = filesPrepared;
        bool pinsReady = false;
        bool positioned = false;
        bool refreshed = false;

        if (SUCCEEDED(hr) && pinManager) {
            // Storage is the durable list of shortcut artifacts this mod owns.
            // Remove only identities not present in the desired snapshot;
            // unchanged separators are never unpinned during live refresh.
            if (!staleCleanupComplete) {
                staleCleanupComplete =
                    CleanupStaleSeparatorShortcuts(
                        pinManager,
                        settings);
                staleShortcutsRemoved = staleCleanupComplete;
            }

            if (staleShortcutsRemoved && !filesPrepared) {
                filesPrepared =
                    PrepareSeparatorFiles(
                        settings,
                        appliedSettings,
                        refreshPulse,
                        &identitiesToPin);
                filesReady = filesPrepared;
            }

            if (staleShortcutsRemoved && filesReady &&
                settings.separators.empty()) {
                DeleteFileIfPresent(g_iconPath);
                pinsReady = true;
                positioned = true;
                refreshed = true;
            } else if (staleShortcutsRemoved && filesReady) {
                pinsReady =
                    PinSeparators(
                        pinManager,
                        settings,
                        identitiesToPin);

                if (pinsReady) {
                    positioned =
                        PositionSeparators(
                            pinManager,
                            settings);

                    if (positioned) {
                        refreshed =
                            PulseTaskbarPinList(
                                pinManager,
                                refreshPulse);
                    }
                } else {
                    Wh_Log(
                        L"[SYNC] Pin pass incomplete; skipping move pass");
                }
            }

            pinManager->Release();
        }

        if (staleShortcutsRemoved && filesReady &&
            pinsReady && positioned && refreshed) {
            Wh_Log(
                L"[SYNC] Separator state converged on attempt %d",
                attempt);
            return true;
        }

        if (IsBackendStopRequested()) {
            Wh_Log(L"[SYNC] Separator reconciliation cancelled");
            return false;
        }

        if (attempt < kConvergeAttempts) {
            Wh_Log(
                L"[SYNC] Reconciliation attempt failed; "
                L"retrying in %u ms",
                retryDelay);

            BackendWaitResult waitResult =
                WaitForBackendStopOrReconcile(retryDelay);

            if (waitResult == BackendWaitResult::Stop) {
                Wh_Log(L"[SYNC] Separator reconciliation cancelled");
                return false;
            }

            if (waitResult == BackendWaitResult::Reconcile) {
                Wh_Log(L"[SYNC] Reconciliation superseded by newer settings");
                return false;
            }

            if (waitResult == BackendWaitResult::Failed) {
                Wh_Log(
                    L"[SYNC] Reconciliation retry wait failed error=%u",
                    GetLastError());
                return false;
            }

            retryDelay *= kRetryBackoff;
        }
    }

    Wh_Log(
        L"[SYNC] Separator state did not converge after %d attempts",
        kConvergeAttempts);
    return false;
}

// -----------------------------------------------------------------------------
// Destruction / cleanup.
// -----------------------------------------------------------------------------

static bool SeparatorShortcutArtifactsExist() {
    return !EnumerateSeparatorShortcuts().empty();
}

static bool UnpinAndDeleteSeparators(bool recoverMissingPins) {
    std::vector<std::wstring> shortcutPaths =
        EnumerateSeparatorShortcuts();

    if (shortcutPaths.empty()) {
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

    // Storage is authoritative: remove current separators, stale settings
    // identities, and a refresh helper left by an interrupted pulse alike.
    for (auto it = shortcutPaths.rbegin();
         it != shortcutPaths.rend();
         ++it) {
        if (!UnpinAndDeleteShortcut(
                pinManager,
                *it,
                L"CLEANUP",
                recoverMissingPins)) {
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

struct BackendThreadParameters {
    HANDLE stopEvent;
    HANDLE wakeEvent;
};

static DWORD WINAPI BackendThreadProc(void* parameter) {
    auto* parameters =
        static_cast<BackendThreadParameters*>(parameter);
    HANDLE stopEvent = parameters->stopEvent;
    HANDLE wakeEvent = parameters->wakeEvent;
    delete parameters;

    g_currentBackendStopEvent = stopEvent;
    g_currentBackendWakeEvent = wakeEvent;

    Wh_Log(L"[INIT] Separator backend worker starting");

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    if (FAILED(hrInit)) {
        Wh_Log(
            L"[INIT] Backend worker CoInitializeEx failed hr=0x%08X",
            static_cast<unsigned int>(hrInit));
        g_currentBackendStopEvent = nullptr;
        g_currentBackendWakeEvent = nullptr;
        return 0;
    }

    // A failed state write is retried with bounded exponential backoff. After
    // the final retry, stay asleep until a fresh explicit wake (Add/Remove/
    // drag/settings/lifecycle) rather than hammering the storage path forever.
    static constexpr DWORD kStateWriteRetryDelaysMs[] = {
        250,
        500,
        1000,
        2000,
        4000,
    };
    size_t stateWriteFailureCount = 0;

    for (;;) {
        // A request that arrived before this iteration is represented by the
        // snapshot we are about to take. A newer request arriving afterwards
        // re-signals the manual-reset event and supersedes this pass.
        ResetEvent(wakeEvent);

        Settings settings;
        Settings appliedSettings;
        SeparatorSetting refreshPulse;
        bool stateDirty = false;

        {
            // Keep the established mutation->settings lock order, but only for
            // the cheap in-memory snapshot. Disk I/O happens after both locks
            // have been released.
            std::lock_guard<std::mutex> mutationLock(
                g_settingsMutationMutex);
            std::shared_lock lock(g_settingsMutex);
            settings = g_settings;
            appliedSettings = g_appliedSettings;
            refreshPulse = g_refreshPulseSetting;
            stateDirty = g_separatorStateDirty;
        }

        bool stateReady = true;
        if (stateDirty) {
            stateReady = SaveSeparatorStateFile(settings);

            if (stateReady) {
                stateWriteFailureCount = 0;

                bool snapshotStillCurrent = false;
                {
                    std::lock_guard<std::mutex> mutationLock(
                        g_settingsMutationMutex);
                    std::shared_lock lock(g_settingsMutex);
                    snapshotStillCurrent =
                        SettingsSnapshotsEqual(
                            settings,
                            g_settings);
                    if (snapshotStillCurrent) {
                        g_separatorStateDirty = false;
                    }
                }

                if (!snapshotStillCurrent) {
                    Wh_Log(
                        L"[STATE] Persisted snapshot was superseded; "
                        L"writing the newer state next");
                    continue;
                }

                Wh_Log(L"[STATE] Persisted separator state on backend worker");
            } else if (!IsBackendStopRequested()) {
                stateWriteFailureCount++;
                Wh_Log(
                    L"[STATE] Failed to persist separator state (failure %zu); "
                    L"PinManager convergence deferred",
                    stateWriteFailureCount);
            }
        } else {
            stateWriteFailureCount = 0;
        }

        if (IsBackendStopRequested()) {
            break;
        }

        bool needsReconcile =
            !g_backendHasConverged.load(std::memory_order_acquire) ||
            SettingsRequireBackendReconcile(
                appliedSettings,
                settings);

        bool success = stateReady;
        if (stateReady && needsReconcile) {
            success =
                ConvergeSeparatorPins(
                    settings,
                    appliedSettings,
                    refreshPulse);

            if (success) {
                {
                    std::unique_lock lock(g_settingsMutex);

                    // Record what the pin backend actually converged to. If
                    // only presentation/logging metadata changed while it was
                    // working, adopt the newer snapshot too because no further
                    // PinManager work is required.
                    g_appliedSettings = settings;

                    if (!SettingsRequireBackendReconcile(
                            settings,
                            g_settings)) {
                        g_appliedSettings = g_settings;
                        g_backendHasConverged.store(
                            true,
                            std::memory_order_release);
                    } else {
                        g_backendHasConverged.store(
                            false,
                            std::memory_order_release);
                    }
                }

                // Directory enumeration belongs to the worker as well. No
                // taskbar UI callback scans mod storage anymore.
                RefreshStoredSeparatorSettings();
            }
        }

        if (!success && !IsBackendStopRequested()) {
            g_backendHasConverged.store(
                false,
                std::memory_order_release);

            if (!IsBackendReconcileRequested()) {
                if (stateReady) {
                    Wh_Log(
                        L"[SYNC] One or more separator operations failed; "
                        L"waiting for another settings/retry request");
                } else if (
                    stateWriteFailureCount <=
                    ARRAYSIZE(kStateWriteRetryDelaysMs)) {
                    Wh_Log(
                        L"[STATE] Retrying state persistence in %u ms",
                        kStateWriteRetryDelaysMs[
                            stateWriteFailureCount - 1]);
                } else {
                    Wh_Log(
                        L"[STATE] State persistence retry limit reached; "
                        L"waiting for the next explicit wake");
                }
            }
        }

        if (IsBackendStopRequested()) {
            break;
        }

        HANDLE events[] = {
            stopEvent,
            wakeEvent,
        };

        DWORD timeout = INFINITE;
        if (!stateReady &&
            stateWriteFailureCount > 0 &&
            stateWriteFailureCount <=
                ARRAYSIZE(kStateWriteRetryDelaysMs)) {
            timeout =
                kStateWriteRetryDelaysMs[
                    stateWriteFailureCount - 1];
        }

        DWORD waitResult =
            WaitForMultipleObjects(
                ARRAYSIZE(events),
                events,
                FALSE,
                timeout);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_TIMEOUT && !stateReady) {
            continue;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            // A real user/settings/lifecycle request gets a fresh retry budget.
            stateWriteFailureCount = 0;
            continue;
        }

        Wh_Log(
            L"[SYNC] Backend wait failed result=0x%08X error=%u",
            waitResult,
            GetLastError());
        break;
    }

    CoUninitialize();
    g_currentBackendStopEvent = nullptr;
    g_currentBackendWakeEvent = nullptr;

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

static bool CleanupSeparatorArtifacts(bool recoverMissingPins) {
    // Shared mod storage may be visible to several explorer.exe instances.
    // Only the process that owns Shell_TrayWnd may mutate the taskbar pin list.
    if (!FindCurrentProcessTaskbarWnd() ||
        !SeparatorShortcutArtifactsExist()) {
        return true;
    }

    HRESULT hrInit =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    const bool shouldUninitialize = SUCCEEDED(hrInit);

    if (hrInit != RPC_E_CHANGED_MODE && FAILED(hrInit)) {
        Wh_Log(
            L"[CLEANUP] CoInitializeEx failed hr=0x%08X; "
            L"can't safely remove separators",
            static_cast<unsigned int>(hrInit));
        return false;
    }

    bool success =
        UnpinAndDeleteSeparators(recoverMissingPins);

    if (shouldUninitialize) {
        CoUninitialize();
    }

    return success;
}

static bool StartBackendWorkerLocked() {
    if (g_backendStopped) {
        return true;
    }

    if (g_backendThread) {
        DWORD threadState =
            WaitForSingleObject(
                g_backendThread,
                0);

        if (threadState == WAIT_TIMEOUT) {
            return true;
        }

        if (threadState == WAIT_FAILED) {
            Wh_Log(
                L"[INIT] Backend worker handle wait failed error=%u; "
                L"recreating worker",
                GetLastError());
        }

        // The long-lived worker should normally only exit during shutdown.
        // If initialization/waiting failed, allow a later readiness/settings
        // event to create a fresh worker.
        CloseHandle(g_backendThread);
        g_backendThread = nullptr;

        if (g_backendWakeEvent) {
            CloseHandle(g_backendWakeEvent);
            g_backendWakeEvent = nullptr;
        }

        if (g_backendStopEvent) {
            CloseHandle(g_backendStopEvent);
            g_backendStopEvent = nullptr;
        }
    }

    g_backendStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_backendStopEvent) {
        Wh_Log(
            L"[INIT] CreateEvent for backend stop failed error=%u",
            GetLastError());
        return false;
    }

    g_backendWakeEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_backendWakeEvent) {
        Wh_Log(
            L"[INIT] CreateEvent for backend wake failed error=%u",
            GetLastError());
        CloseHandle(g_backendStopEvent);
        g_backendStopEvent = nullptr;
        return false;
    }

    auto* parameters =
        new (std::nothrow) BackendThreadParameters{
            .stopEvent = g_backendStopEvent,
            .wakeEvent = g_backendWakeEvent,
        };

    if (!parameters) {
        Wh_Log(L"[INIT] Failed to allocate backend worker parameters");
        CloseHandle(g_backendWakeEvent);
        CloseHandle(g_backendStopEvent);
        g_backendWakeEvent = nullptr;
        g_backendStopEvent = nullptr;
        return false;
    }

    g_backendThread =
        CreateThread(
            nullptr,
            0,
            BackendThreadProc,
            parameters,
            0,
            nullptr);

    if (!g_backendThread) {
        Wh_Log(
            L"[INIT] CreateThread for backend worker failed error=%u",
            GetLastError());
        delete parameters;
        CloseHandle(g_backendWakeEvent);
        CloseHandle(g_backendStopEvent);
        g_backendWakeEvent = nullptr;
        g_backendStopEvent = nullptr;
        return false;
    }

    return true;
}

static void StartBackendWorker() {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (g_unloading || g_backendStopped) {
        return;
    }

    bool workerWasRunning =
        g_backendThread &&
        WaitForSingleObject(
            g_backendThread,
            0) == WAIT_TIMEOUT;

    if (!StartBackendWorkerLocked()) {
        Wh_Log(L"[INIT] Failed to start separator backend worker");
        return;
    }

    // TrayUI::StartTaskbar can run again if Explorer reconstructs the taskbar
    // in-process. A persistent worker already exists in that case, so wake it
    // to re-assert the pin order and refresh pulse for the new presentation.
    if (workerWasRunning && g_backendWakeEvent) {
        SetEvent(g_backendWakeEvent);
    }
}

static void RequestBackendReconcile() {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (g_unloading || g_backendStopped) {
        return;
    }

    // Preserve the Stage A readiness policy: if the taskbar hasn't been
    // created yet, TrayUI::StartTaskbar will start the worker later and it
    // will consume the newest desired settings snapshot.
    if (!g_backendThread &&
        !FindCurrentProcessTaskbarWnd()) {
        return;
    }

    // Always go through StartBackendWorkerLocked. Besides creating a missing
    // worker, it notices a worker that exited unexpectedly and recreates it.
    if (!StartBackendWorkerLocked()) {
        Wh_Log(
            L"[SYNC] Failed to start separator backend worker");
        return;
    }

    if (g_backendWakeEvent) {
        SetEvent(g_backendWakeEvent);
    }
}


static bool SignalBackendWorker() {
    std::lock_guard<std::mutex> lock(g_lifecycleMutex);

    if (g_unloading || g_backendStopped ||
        !g_backendThread || !g_backendWakeEvent) {
        return false;
    }

    if (WaitForSingleObject(
            g_backendThread,
            0) != WAIT_TIMEOUT) {
        return false;
    }

    return SetEvent(g_backendWakeEvent) != FALSE;
}

static void QueueBackendWork() {
    // Native taskbar callbacks normally arrive after TrayUI::StartTaskbar, so
    // the hot path is a single SetEvent. Fall back to the existing worker
    // readiness logic only if the worker is unexpectedly absent.
    if (!SignalBackendWorker()) {
        RequestBackendReconcile();
    }
}

static void AddSeparatorFromTaskbarMenu() {
    if (g_unloading ||
        g_internalCleanupInProgress.load()) {
        return;
    }

    std::lock_guard<std::mutex> mutationLock(
        g_settingsMutationMutex);

    Settings updated;
    {
        std::shared_lock lock(g_settingsMutex);
        updated = g_settings;
    }

    std::wstring stableId;
    const auto alreadyUsed =
        [&updated](std::wstring_view candidate) {
            return std::any_of(
                updated.separators.begin(),
                updated.separators.end(),
                [candidate](const SeparatorSetting& separator) {
                    return separator.stableId ==
                        candidate;
                });
        };

    do {
        stableId = GenerateStableId();
    } while (alreadyUsed(stableId));

    int sourceIndex =
        static_cast<int>(
            updated.separators.size());

    updated.separators.push_back({
        .ordinal = sourceIndex + 1,
        .sourceIndex = sourceIndex,
        // Explicitly unanchored. Windows appends it at the native end; the
        // first user drag records the concrete position through TryMoveGroup.
        .targetIndex = -1,
        .width = updated.width,
        .stableId = stableId,
        .identity = BuildSeparatorIdentity(
            updated.identifierPrefix,
            stableId),
    });

    {
        std::unique_lock lock(g_settingsMutex);
        g_settings = updated;
        g_backendHasConverged = false;
    }

    // Publish the desired state immediately, but let the backend worker make
    // it durable before PinManager is allowed to create the new pin.
    g_separatorStateDirty = true;

    Wh_Log(
        L"[NATIVE-ADD] Added separator identity='%s' at native end",
        updated.separators.back().identity.c_str());

    QueueBackendWork();
}

static void StopBackendWorker() {
    HANDLE thread = nullptr;
    HANDLE stopEvent = nullptr;
    HANDLE wakeEvent = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_lifecycleMutex);

        // Permanent latch: no loader-hook callback or settings callback can
        // create/wake a worker after shutdown begins.
        g_backendStopped = true;
        g_backendHasConverged = false;

        stopEvent =
            std::exchange(
                g_backendStopEvent,
                nullptr);
        wakeEvent =
            std::exchange(
                g_backendWakeEvent,
                nullptr);
        thread =
            std::exchange(
                g_backendThread,
                nullptr);

        if (stopEvent) {
            SetEvent(stopEvent);
        }
        if (wakeEvent) {
            SetEvent(wakeEvent);
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

    if (wakeEvent) {
        CloseHandle(wakeEvent);
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
        if (shellProcessId == GetCurrentProcessId()) {
            return true;
        }
    }

    // During an Explorer restart, GetShellWindow can be null or can still
    // refer to the previous, dying shell. Exclude known folder-process command
    // lines while allowing the replacement shell to wait for its own taskbar.
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

BOOL ExplorerModInit() {
    if (!IsMainExplorerProcess()) {
        return FALSE;
    }

    Wh_Log(L"[INIT] Taskbar Icon Separators loading");

    if (!InitializeStoragePath()) {
        return FALSE;
    }

    if (!LoadSettings()) {
        return FALSE;
    }

    if (!InitializeTaskbarStylingHooks()) {
        Wh_Log(
            L"[INIT] TaskListButton styling hooks unavailable");
    }

    if (!InitializeTaskbarDllHooks()) {
        Wh_Log(
            L"[INIT] taskbar.dll hooks unavailable");
    }

    // Hook operations installed during Wh_ModInit are applied automatically
    // before Wh_ModAfterInit. Missing taskbar hooks remain independent from
    // the pin-list backend and can still be installed on a late module load.
    return TRUE;
}

void ExplorerModAfterInit() {
    // Close the small race where the module wasn't present in Wh_ModInit but
    // appeared before/around Wh_ModAfterInit.
    if (!g_taskbarViewDllHooked) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            InstallTaskbarViewHooks(module, true);
        }
    }

    if (!g_taskbarDllHooked.load(std::memory_order_acquire)) {
        if (HMODULE module = GetModuleHandleW(L"taskbar.dll")) {
            InstallTaskbarDllHooks(module, true);
        }
    }

    // If the mod is enabled after Explorer already created the taskbar,
    // TrayUI::StartTaskbar has already returned and won't fire for us.
    if (FindCurrentProcessTaskbarWnd()) {
        Wh_Log(L"[LIFECYCLE] Existing taskbar window found; starting backend");
        StartBackendWorker();
    } else {
        Wh_Log(L"[LIFECYCLE] Waiting for TrayUI::StartTaskbar");
    }
}

void ExplorerModBeforeUninit() {
    // Hook APIs are legal only until this callback returns. Close every late
    // installation path first, then give already-entered installers a short,
    // bounded chance to leave the Windhawk hook engine. Never block unload
    // indefinitely waiting on engine work from another thread.
    g_hookInstallationClosed.store(true, std::memory_order_release);
    DrainHookInstallers();

    // Join the worker first so there can be no concurrent state-file writer.
    // Then perform one teardown-only synchronous durability fallback for a
    // missed drag-completion signal or an in-flight native mutation.
    StopBackendWorker();
    FlushPendingSeparatorStateSynchronously();

    // Remove pins while the styling and input hooks can still keep a transient
    // recovery pin styled and inert.
    g_internalCleanupInProgress = true;
    CleanupSeparatorArtifacts(true);

    // Stop applying separator state before releasing the cleanup guard.
    g_unloading = true;
    g_internalCleanupInProgress = false;

    // Restore every tracked container on its owning taskbar UI thread before
    // Windhawk removes the hooks.
    RestoreTrackedSeparatorVisualStates();
}

void ExplorerModUninit() {
    Wh_Log(L"[UNINIT] Taskbar Icon Separators unloading");

    // Defensive in case an older Windhawk build skips Wh_ModBeforeUninit.
    g_unloading = true;
    StopBackendWorker();
    FlushPendingSeparatorStateSynchronously();

    // Defensive fallback for Windhawk builds that skip Wh_ModBeforeUninit.
    // Hooks may already be gone here, so never recover an absent pin by
    // temporarily pinning it again. Cleanup is also restricted to the
    // explorer.exe instance that actually owns the taskbar.
    CleanupSeparatorArtifacts(false);

    Wh_Log(L"[UNINIT] Taskbar Icon Separators unloaded");
}

BOOL ExplorerModSettingsChanged(BOOL* bReload) {
    if (bReload) {
        *bReload = FALSE;
    }

    double newWidth =
        LoadSeparatorWidthSetting();

    {
        std::lock_guard<std::mutex> mutationLock(
            g_settingsMutationMutex);
        std::unique_lock lock(g_settingsMutex);

        g_settings.width = newWidth;
        g_appliedSettings.width = newWidth;

        for (auto& separator :
             g_settings.separators) {
            separator.width = newWidth;
        }
        for (auto& separator :
             g_appliedSettings.separators) {
            separator.width = newWidth;
        }
        for (auto& separator :
             g_storedSeparatorSettings) {
            separator.width = newWidth;
        }

        g_refreshPulseSetting.width =
            newWidth;
    }

    Wh_Log(
        L"[SETTINGS] Applied global separator width=%d",
        static_cast<int>(newWidth));

    // Width is presentation-only. Native separator positions and identities
    // live in the mod-storage state file and never require PinManager work.
    RefreshTrackedSeparatorVisualStates();

    return TRUE;
}


// -----------------------------------------------------------------------------
// Windhawk entry points.
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    return ExplorerModInit();
}

void Wh_ModAfterInit() {
    ExplorerModAfterInit();
}

void Wh_ModBeforeUninit() {
    ExplorerModBeforeUninit();
}

void Wh_ModUninit() {
    ExplorerModUninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    return ExplorerModSettingsChanged(bReload);
}
