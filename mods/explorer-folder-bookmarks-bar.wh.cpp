// ==WindhawkMod==
// @id              explorer-folder-bookmarks-bar
// @name            Explorer Folder Bookmarks Bar
// @description     Adds a folder bookmarks bar under the address bar of Windows 11 File Explorer.
// @version         0.8.35
// @author          Maxim Fomin
// @github          https://github.com/MaxITService
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lshell32 -luuid -lruntimeobject -lwindowscodecs -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Folder Bookmarks Bar

[Project on GitHub](https://github.com/MaxITService/EXPLORER-bookmarks-bar-windhawk)

A row of folder bookmarks under the address bar of Windows 11 File Explorer.

![Explorer Folder Bookmarks Bar in File Explorer](https://raw.githubusercontent.com/MaxITService/EXPLORER-bookmarks-bar-windhawk/main/Promo/How-it-works.gif)

The bar appears in new File Explorer windows. Windows that were already open
get it too in most cases; if one doesn't, open a new window. Once a window has
the bar, it follows changes made in other windows and in settings.

## Bookmarks

- **+** bookmarks the current folder. With **Ctrl+B shortcut** on in
  settings, **Ctrl+B** does the same, or removes the bookmark if there is one.
- With folders selected in the file list, Ctrl+B bookmarks them all, or
  removes them if all are already bookmarked.
- Drag folders onto the bar to insert them where you drop them.
- Up to 32 bookmarks. One Ctrl+B or drop takes at most 20 folders. A short
  notice appears when the bar is full or a folder is already there.
- Click to open in the current tab, Ctrl+click for a new tab, right-click for
  a new window.
- Drag a bookmark to reorder. Middle-click removes it; the folder itself is
  not touched.
- With **Show parent of same-named folders** on in settings, buttons with
  the same name also show their parent folder: `projA\src`, `projB\src`.
- Hover shows the full path. A missing folder on a local disk gets a yellow
  warning icon. Custom folder icons (desktop.ini) are shown.
- Virtual locations such as Home can't be bookmarked.
- Each Windows account has its own bookmarks and recent folders. Settings
  apply to all accounts.

The bar grows from one to four rows as the window narrows, then scrolls
sideways.

## FX menu

Left-click **FX**, next to **+**, for your profile (**~**), Desktop, Documents,
Downloads and your own folders. Right-click it for all drives. Ctrl+click an
entry to open it in a new tab.

Add your folders in **Settings → FX custom folders**: an absolute path
(`%NAME%` variables work) and an optional label. Local, network and removable
folders all work. A folder is hidden while it's missing from a local disk or
its drive is unplugged. Network folders and folders on plugged-in removable
drives are always listed.

## Backup

Right-click **+** for **Save bookmarks…** and **Load bookmarks…** (JSON file).
Loading replaces the current list; a file that fails validation changes
nothing.

## Recent folders

Off by default. Turn on **Recent folders → Show recent folders** to show
buttons for folders you recently opened in File Explorer at the right end of
the bar, newest on the right: 3 by default, up to 25.

![Recent folders on the bookmarks bar](https://raw.githubusercontent.com/MaxITService/EXPLORER-bookmarks-bar-windhawk/main/Promo/How_recents_work.gif)

- Left-click **RC** for the rest of the list (10 in total by default, up
  to 25).
- Right-click **RC** to clear the list.
- Middle-click a recent folder to remove it; drag it onto the bookmarks to
  keep it.
- Bookmarked and missing folders are skipped. In a narrow window the oldest
  buttons move into RC first.

The mod remembers up to 64 folders opened in File Explorer, only in File
Explorer's memory: the list starts empty after File Explorer restarts or the
mod is updated. Turning the feature off erases it.

## If the bar doesn't appear

A Windows update can move the spot where the bar is inserted. Disable the mod
and check the Windhawk log.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fxCustomFolders:
    - - label: ''
        $name: Menu label
        $description: Optional. Default is the folder name.
      - path: ''
        $name: Folder path
        $description: For example C:\Projects, \\server\share or %USERPROFILE%\Pictures.
  $name: FX custom folders
  $description: Extra folders for the FX menu (the button after +), up to 24.
- bookmarkHotkey: false
  $name: Ctrl+B shortcut
  $description: Adds or removes bookmarks for the current or selected folders.
- parentForDuplicates: false
  $name: Show parent of same-named folders
  $description: Buttons with the same folder name also show their parent, for example projA\src and projB\src.
- recent:
  - enabled: false
    $name: Show recent folders
    $description: Turning it off erases the remembered folders. The settings below work only while it is on.
  - buttons: 3
    $name: Buttons
    $description: How many recent folders get a button, 1 to 25.
    #! $min: 1
    #! $max: 25
  - history: 10
    $name: Remembered folders
    $description: Total in the buttons and the RC menu, 1 to 25.
    #! $min: 1
    #! $max: 25
  - rcButton: menu
    $name: RC button
    $description: Menu with the other recent folders.
    $options:
    - menu: Shown; right-click opens Clear
    - clear: Shown; right-click clears at once
    - hidden: Hidden
  $name: Recent folders
  $description: Recent folders at the right end of the bar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <exdisp.h>
#include <oleauto.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <robuffer.h>
#include <sddl.h>
#include <shellapi.h>
#include <wincodec.h>
#include <windhawk_utils.h>

// winbase.h defines a legacy macro that collides with a WinRT method.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <cwctype>
#include <cwchar>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxp = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxmi = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace wjson = winrt::Windows::Data::Json;

constexpr wchar_t kBarName[] = L"WindhawkExplorerFolderBookmarksBar";
constexpr wchar_t kFrameProperty[] = L"WindhawkExplorerFolderBookmarksBarFrame";
constexpr size_t kMaxStorageChars = 30000;
constexpr size_t kMaxBookmarks = 32;
// A single Ctrl+B or drop handles at most this many folders.
constexpr size_t kMaxFoldersPerAction = 20;
// Ctrl+B with a huge selection must not stall the UI thread: only the first
// kMaxSelectionScanned selected items are examined.
constexpr DWORD kMaxSelectionScanned = 1000;
// How long the notice below the bar stays visible.
constexpr UINT kNoticeMs = 2500;
constexpr DWORD kMaxImportBytes = 262144;
constexpr wchar_t kExportFileName[] = L"explorer-folder-bookmarks.json";
constexpr size_t kMaxCustomFxFolders = 24;
constexpr size_t kMaxCachedIcons = 64;
constexpr ULONGLONG kFolderCheckIntervalMs = 10000;
constexpr ULONGLONG kIconCacheLifetimeMs = 300000;
constexpr ULONGLONG kFailedIconCacheLifetimeMs = 30000;
// The stock header at 96 DPI measured 38 + 48 + 48 units. Its centered
// navigation Grid is 54 units high, overhanging the 48-unit navigation row by
// 3 units on each side. An added 38-unit strip therefore needs 38 + 6 = 44
// extra units in the host when the command row stays at its natural 48.
constexpr double kRowHeight = 38.0;
constexpr double kButtonHeight = 32.0;
constexpr double kButtonVerticalInset = (kRowHeight - kButtonHeight) / 2.0;
constexpr double kInterRowGap = 2.0;
constexpr unsigned kMaxRows = 4;
// Lift the visible bar without changing its reserved layout height.
constexpr double kRowOpticalLift = 3.0;
constexpr int kNavigationOverhang = 6;
constexpr int kMeasuredHostHeightAt96Dpi = 136;
constexpr size_t kFixedButtons = 2;
constexpr float kDragThreshold = 6.0f;
// The bookmarks value name of versions before 0.8.32, shared by every
// account on the PC.
constexpr wchar_t kSharedBookmarksKey[] = L"folders";
// Versions before 0.8.34 stored recent folders here; they now stay in memory.
constexpr wchar_t kOldRecentsKey[] = L"recentFolders";
// More entries than can be listed are kept, so filtering out bookmarks and
// missing folders still leaves enough recent folders to show.
constexpr size_t kMaxStoredRecents = 64;
constexpr int kDefaultRecentButtons = 3;
constexpr int kMaxRecentButtons = 25;
constexpr int kDefaultRecentHistory = 10;
constexpr int kMaxRecentHistory = 25;

constexpr double RowAllocation(unsigned rows) {
    return kRowHeight * rows + kInterRowGap * (rows - 1);
}

constexpr int HostExtraAt96Dpi(unsigned rows) {
    return static_cast<int>(RowAllocation(rows)) + kNavigationOverhang;
}

// Public Shell COM identifiers, defined here to avoid SDK import ambiguity.
constexpr CLSID kShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr GUID kTopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf,
    {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};
constexpr IID kWebBrowserEvents2 = {
    0x34a715a0, 0x6587, 0x11d0,
    {0x92, 0x4a, 0x00, 0x20, 0xaf, 0xc7, 0xac, 0x4d}};
constexpr IID kShellWindowsEvents = {
    0xfe4106e0, 0x399a, 0x11d0,
    {0xa4, 0x8c, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr DISPID kDispidWindowRegistered = 200;
constexpr DISPID kDispidWindowRevoked = 201;
constexpr DISPID kDispidNavigateComplete2 = 252;
constexpr DISPID kDispidDocumentComplete = 259;

enum class FolderStatus { Available, Missing, Unknown };

struct RecentSettings {
    bool enabled = false;
    size_t buttons = kDefaultRecentButtons;
    bool menuButton = true;
    size_t history = kDefaultRecentHistory;
    bool rightClickClears = false;
};
struct FxFolder {
    std::wstring label;
    std::wstring path;
};

// Publish a complete settings snapshot; each UI thread uses its own copy.
std::mutex g_settingsMutex;
RecentSettings g_sharedRecentSettings;
std::vector<FxFolder> g_sharedFxFolders;
bool g_sharedBookmarkHotkey = false;
bool g_sharedParentForDuplicates = false;
std::atomic<ULONGLONG> g_settingsGeneration = 0;
thread_local RecentSettings g_recentSettings;
thread_local std::vector<FxFolder> g_fxFolders;
thread_local bool g_bookmarkHotkey = false;
thread_local bool g_parentForDuplicates = false;
thread_local ULONGLONG g_threadSettingsGeneration = 0;
thread_local bool g_settingsApplyPending = false;

bool UpdateThreadSettings() {
    if (g_threadSettingsGeneration == g_settingsGeneration.load()) {
        return false;
    }
    std::lock_guard lock(g_settingsMutex);
    g_recentSettings = g_sharedRecentSettings;
    g_fxFolders = g_sharedFxFolders;
    g_bookmarkHotkey = g_sharedBookmarkHotkey;
    g_parentForDuplicates = g_sharedParentForDuplicates;
    g_threadSettingsGeneration = g_settingsGeneration.load();
    g_settingsApplyPending = true;
    return true;
}

void NotifyExplorerBars(bool thisProcessOnly = false);
void ScheduleThreadWork();
bool TrackFrame(HWND window);

struct FolderButtonState {
    std::wstring path;
    winrt::weak_ref<muxc::Button> button;
    FolderStatus status;
    ULONGLONG iconLoadedAt = 0;
};

// Serialize read/modify/write across this account's Explorer processes as
// well as UI threads; other accounts keep their own values. The named
// mutex is opened in ModInit and closed after UI cleanup. The wait
// is bounded, so an Explorer process that stalls while holding the mutex
// cannot freeze this UI thread; the storage operation fails instead and its
// caller logs it.
struct StorageMutex {
    static constexpr DWORD kWaitMs = 3000;
    std::mutex local;
    HANDLE processMutex = nullptr;

    void lock() {
        local.lock();
        if (processMutex) {
            const DWORD wait = WaitForSingleObject(processMutex, kWaitMs);
            if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
                const DWORD error =
                    wait == WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
                if (wait == WAIT_TIMEOUT) {
                    Wh_Log(L"Storage is locked by another Explorer process; "
                           L"gave up after %u ms",
                           kWaitMs);
                }
                local.unlock();
                winrt::throw_hresult(HRESULT_FROM_WIN32(error));
            }
        }
    }

    void unlock() {
        if (processMutex) {
            ReleaseMutex(processMutex);
        }
        local.unlock();
    }
};
StorageMutex g_storageMutex;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_extensionHooked = false;
std::atomic<bool> g_frameHooked = false;
std::atomic<bool> g_extensionHookAttempted = false;
std::atomic<bool> g_frameHookAttempted = false;
std::mutex g_operationMutex;
std::condition_variable g_operationFinished;
unsigned g_activeOperations = 0;
thread_local IFileDialog* g_threadFileDialog = nullptr;

struct BarState {
    HWND frame = nullptr;
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token unloadedToken{};
    winrt::weak_ref<muxc::Grid> grid;
    winrt::weak_ref<muxc::Grid> hostGrid;
    winrt::weak_ref<mux::FrameworkElement> navControl;
    // The bar root holds the bookmark strip and, to its right, the recents.
    winrt::weak_ref<muxc::Grid> barRoot;
    winrt::weak_ref<muxc::ScrollViewer> strip;
    winrt::weak_ref<muxc::StackPanel> buttons;
    winrt::weak_ref<muxc::StackPanel> recents;
    winrt::event_token rootPointerToken{};
    winrt::event_token rootSizeToken{};
    winrt::event_token rootLoadedToken{};
    winrt::event_token rootDragOverToken{};
    winrt::event_token rootDragLeaveToken{};
    winrt::event_token rootDropToken{};
    std::vector<std::function<void()>> panelHandlers;
    std::vector<std::function<void()>> fxMenuHandlers;
    std::vector<std::function<void()>> driveHandlers;
    std::vector<std::function<void()>> recentHandlers;
    std::vector<std::function<void()>> recentMenuHandlers;
    std::vector<FolderButtonState> folderButtons;
    std::vector<FolderButtonState> recentFolderButtons;
    // Newest first. The first visibleRecents entries have buttons; the RC
    // menu lists the rest.
    std::vector<std::wstring> recentFolders;
    size_t recentButtonCount = 0;
    bool hasRecentMenuButton = false;
    size_t visibleRecents = 0;
    // Keep row definitions strong: they are not UIElements, and XAML can
    // discard an unreferenced wrapper so a weak reference no longer resolves.
    muxc::RowDefinition addedRow{nullptr};
    muxc::RowDefinition commandRow{nullptr};
    mux::GridLength oldCommandRowHeight{1.0, mux::GridUnitType::Star};
    bool createdFirstRow = false;
    double oldGridMinHeight = 0;
    double oldNavMinHeight = 0;
    double appliedGridMinHeight = 0;
    double appliedNavMinHeight = 0;
    double originalGridHeight = 0;
    double originalNavHeight = 0;
    unsigned rowCount = 1;
    double lastLayoutWidth = 0;
    bool reflowing = false;
    std::wstring renderedKey;
    std::wstring renderedRecentKey;
    // Parent parts of the bookmark labels; they can depend on the recents.
    std::wstring renderedLabelKey;
    ULONGLONG renderedSettingsGeneration = 0;
    bool renderComplete = false;
    bool refreshAfterDrag = false;
    ULONGLONG lastFolderCheckAt = 0;
    std::wstring dragPath;
    winrt::weak_ref<muxc::Button> dragButton;
    winrt::Windows::Foundation::Point dragStart{};
    uint32_t dragPointerId = 0;
    bool dragIsMouse = false;
    bool dragging = false;
    bool dragFromRecents = false;
    std::wstring suppressClick;
    double dragOldOpacity = 1.0;
    winrt::weak_ref<muxc::Button> dropTarget;
    mux::Thickness dropOldThickness{};
    bool dropAfter = false;
    // The short notice below the bar, created when first needed.
    muxc::Flyout notice{nullptr};
    muxc::TextBlock noticeText{nullptr};
};

// XAML objects must only be touched by their owning UI thread.
// Stable addresses matter while XAML layout callbacks run and new Explorer
// tabs can register another command bar on the same UI thread.
// The rows and other XAML references are released explicitly on their UI
// thread in RemoveBarVisuals and CleanupCurrentThread; do not release them
// from TLS destruction after Explorer's XAML teardown.
[[clang::no_destroy]] thread_local std::optional<std::list<BarState>> g_bars{
    std::in_place};
// Explorer's XAML window and its size hook run on the same UI thread. Use the
// largest active bar on that thread so another tab cannot be clipped.
thread_local unsigned g_frameRows = 0;
thread_local unsigned g_uiCallbackDepth = 0;
thread_local bool g_threadClosing = false;
thread_local bool g_drainingClosedFrames = false;
thread_local std::vector<HWND> g_closedFrames;
void DrainClosedFrames();

// COM and dialogs can pump a nested close message. Keep BarState/tracker alive
// until the outer callback stops using them, then release them on this UI thread.
struct UiCallbackScope {
    UiCallbackScope() { ++g_uiCallbackDepth; }
    ~UiCallbackScope() {
        if (--g_uiCallbackDepth == 0) {
            try {
                DrainClosedFrames();
            } catch (...) {
                Wh_Log(L"Deferred frame cleanup failed: %08X", winrt::to_hresult().value);
            }
        }
    }
};

// Event handlers, building a new window's bar, file dialogs and recent-folder
// tracking can run a nested message loop on an Explorer thread: COM calls to
// Explorer's shell, Shell icon lookups, dialogs.
// The cleanup that unloading sends to each thread can be dispatched inside
// such a loop, so unloading waits until no operation is on any stack, and no
// new one starts once it has begun.
struct OperationScope {
    UiCallbackScope uiScope;
    bool active = false;
    OperationScope() {
        std::lock_guard lock(g_operationMutex);
        if (!g_unloading) {
            ++g_activeOperations;
            active = true;
        }
    }
    ~OperationScope() {
        if (active) {
            std::lock_guard lock(g_operationMutex);
            --g_activeOperations;
            g_operationFinished.notify_all();
        }
    }
};

void RevokeHandlers(std::vector<std::function<void()>>& handlers) {
    auto pending = std::move(handlers);
    handlers.clear();
    for (auto it = pending.rbegin(); it != pending.rend(); ++it) {
        try {
            (*it)();
        } catch (...) {
            Wh_Log(L"Bookmark handler cleanup failed: %08X",
                   winrt::to_hresult().value);
        }
    }
}

// An exception must not leave a XAML delegate: XAML would see a failed
// HRESULT from an event handler. Log it and let the bar keep working. Each
// handler is an operation, so unloading waits for it, and a handler that
// fires after unloading began does nothing.
template <typename Handler>
auto GuardHandler(const wchar_t* name, Handler handler) {
    return [name, handler = std::move(handler)](auto&&... args) {
        OperationScope operation;
        if (!operation.active) {
            return;
        }
        try {
            handler(std::forward<decltype(args)>(args)...);
        } catch (...) {
            Wh_Log(L"%ls failed: %08X", name, winrt::to_hresult().value);
        }
    };
}

template <typename Element, typename Handler>
void TrackClick(std::vector<std::function<void()>>& handlers,
                const Element& element, Handler&& handler) {
    auto token = element.Click(
        GuardHandler(L"Click handler", std::forward<Handler>(handler)));
    auto weak = winrt::make_weak(element);
    handlers.emplace_back([weak, token] {
        if (auto current = weak.get()) {
            current.Click(token);
        }
    });
}

template <typename Element, typename Handler>
void TrackContextRequested(std::vector<std::function<void()>>& handlers,
                           const Element& element, Handler&& handler) {
    auto token = element.ContextRequested(GuardHandler(
        L"ContextRequested handler", std::forward<Handler>(handler)));
    auto weak = winrt::make_weak(element);
    handlers.emplace_back([weak, token] {
        if (auto current = weak.get()) {
            current.ContextRequested(token);
        }
    });
}

template <typename Handler>
void TrackOpening(std::vector<std::function<void()>>& handlers,
                  const muxc::MenuFlyout& menu, Handler&& handler) {
    auto token = menu.Opening(
        GuardHandler(L"Opening handler", std::forward<Handler>(handler)));
    auto weak = winrt::make_weak(menu);
    handlers.emplace_back([weak, token] {
        if (auto current = weak.get()) {
            current.Opening(token);
        }
    });
}

void TrackPointer(std::vector<std::function<void()>>& handlers,
                  const muxc::Button& button, const mux::RoutedEvent& event,
                  muxi::PointerEventHandler handler) {
    auto boxed = winrt::box_value(muxi::PointerEventHandler{
        GuardHandler(L"Pointer handler", std::move(handler))});
    button.AddHandler(event, boxed, true);
    auto weak = winrt::make_weak(button);
    handlers.emplace_back([weak, event, boxed] {
        if (auto current = weak.get()) {
            current.RemoveHandler(event, boxed);
        }
    });
}
struct IconPixels {
    int width = 0;
    int height = 0;
    std::vector<BYTE> bytes;
};

struct IconCacheEntry {
    std::wstring path;
    std::optional<IconPixels> pixels;
    winrt::weak_ref<muxmi::WriteableBitmap> bitmap;
    bool attempted = false;
    ULONGLONG loadedAt = 0;
    ULONGLONG usedAt = 0;
};

// WriteableBitmap is a XAML object, so never share these entries across UI
// threads. Eviction also bounds memory when bookmarks are repeatedly changed.
thread_local std::vector<IconCacheEntry> g_iconCache;

// Thread-local destructors don't run on Explorer threads that outlive the
// mod, so free this thread's icons and settings copy when its bars go away.
// A later window on the thread copies the settings again.
void ReleaseThreadCaches() {
    // Assigning {} would keep the capacity; swapping frees the buffers.
    std::vector<IconCacheEntry>().swap(g_iconCache);
    std::vector<FxFolder>().swap(g_fxFolders);
    g_threadSettingsGeneration = 0;
}

std::wstring NormalizePath(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

bool SamePath(const std::wstring& a, const std::wstring& b) {
    // Ordinal, case-insensitive for all of Unicode, independent of the locale.
    return CompareStringOrdinal(a.c_str(), static_cast<int>(a.size()),
                                b.c_str(), static_cast<int>(b.size()),
                                TRUE) == CSTR_EQUAL;
}

FolderStatus CheckFolderStatus(const std::wstring& path) {
    // Avoid probing network and removable storage on Explorer's UI thread.
    // An unknown result never produces a misleading missing-folder badge.
    if (path.size() < 3 || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/') ||
        !((path[0] >= L'A' && path[0] <= L'Z') ||
          (path[0] >= L'a' && path[0] <= L'z'))) {
        return FolderStatus::Unknown;
    }
    wchar_t root[] = {path[0], L':', L'\\', L'\0'};
    UINT driveType = GetDriveTypeW(root);
    if (driveType == DRIVE_NO_ROOT_DIR) {
        return FolderStatus::Missing;
    }
    if (driveType != DRIVE_FIXED && driveType != DRIVE_RAMDISK) {
        return FolderStatus::Unknown;
    }
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY)
                   ? FolderStatus::Available
                   : FolderStatus::Missing;
    }
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ||
        error == ERROR_INVALID_DRIVE) {
        return FolderStatus::Missing;
    }
    return FolderStatus::Unknown;
}

// Windhawk keeps a mod's storage for the whole PC, so each account's
// bookmarks use a value name that ends with its SID. Set once in Wh_ModInit;
// without a SID, the shared name of earlier versions stays in use.
std::wstring g_bookmarksKey = kSharedBookmarksKey;

std::wstring ReadValueLocked(const wchar_t* key) {
    std::vector<wchar_t> buffer(kMaxStorageChars + 1);
    size_t chars = Wh_GetStringValue(key, buffer.data(), buffer.size());
    if (chars == 0 || chars > kMaxStorageChars) {
        return {};
    }
    return std::wstring(buffer.data(), chars);
}

std::wstring ReadStorageLocked() {
    return ReadValueLocked(g_bookmarksKey.c_str());
}

std::vector<std::wstring> SplitLines(const std::wstring& storage,
                                     size_t limit) {
    std::vector<std::wstring> lines;
    size_t pos = 0;
    while (pos < storage.size() && lines.size() < limit) {
        size_t end = storage.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = storage.size();
        }
        if (end > pos) {
            lines.push_back(storage.substr(pos, end - pos));
        }
        pos = end + 1;
    }
    return lines;
}

std::vector<std::wstring> SplitPaths(const std::wstring& storage,
                                     size_t limit) {
    auto paths = SplitLines(storage, limit);
    for (auto& path : paths) {
        path = NormalizePath(std::move(path));
    }
    std::erase_if(paths, [](const auto& path) { return path.empty(); });
    return paths;
}

std::vector<std::wstring> SplitBookmarks(const std::wstring& storage) {
    return SplitPaths(storage, kMaxBookmarks);
}

std::wstring JoinLines(const std::vector<std::wstring>& lines) {
    std::wstring storage;
    for (const auto& line : lines) {
        if (!storage.empty()) {
            storage += L'\n';
        }
        storage += line;
    }
    return storage;
}

bool SaveBookmarksLocked(const std::vector<std::wstring>& folders) {
    std::wstring storage = JoinLines(folders);
    const bool saved = storage.size() <= kMaxStorageChars &&
                       Wh_SetStringValue(g_bookmarksKey.c_str(), storage.c_str());
    if (saved) {
        NotifyExplorerBars();
    } else {
        Wh_Log(L"Bookmarks not saved: %zu folder(s), %zu of %zu characters",
               folders.size(), storage.size(), kMaxStorageChars);
    }
    return saved;
}

// The SID of the account running this Explorer process, or empty.
std::wstring CurrentUserSid() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return {};
    }
    std::wstring result;
    alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
    DWORD size = 0;
    PWSTR sid = nullptr;
    if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
        ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer)->User.Sid,
                               &sid)) {
        result = sid;
        LocalFree(sid);
    }
    CloseHandle(token);
    return result;
}

// Shared bookmarks can't be attributed to one account, so the first account
// to start Explorer with this version takes them and the shared value is
// removed. Shared bookmarks found later were written by an older version
// after that, so they are newer and replace this account's list.
void ClaimSharedValueLocked(const wchar_t* shared, const std::wstring& own) {
    const auto value = ReadValueLocked(shared);
    if (value.empty()) {
        return;
    }
    if (!Wh_SetStringValue(own.c_str(), value.c_str())) {
        Wh_Log(L"Could not move the shared %ls value to this account", shared);
        return;
    }
    Wh_DeleteValue(shared);
    Wh_Log(L"Moved the shared %ls value to this account", shared);
}

void InitUserStorage() try {
    // Erase the history that earlier versions kept for every account.
    Wh_DeleteValue(kOldRecentsKey);
    const auto sid = CurrentUserSid();
    if (sid.empty()) {
        Wh_Log(L"Could not read the account SID; bookmarks are shared by all "
               L"accounts");
        return;
    }
    g_bookmarksKey = std::wstring(kSharedBookmarksKey) + L"_" + sid;
    std::lock_guard lock(g_storageMutex);
    ClaimSharedValueLocked(kSharedBookmarksKey, g_bookmarksKey);
} catch (...) {
    Wh_Log(L"Could not move the shared bookmarks to this account: %08X",
           winrt::to_hresult().value);
}

// Suggest the profile location for an export without fixing the user's choice.
// The bar computes this on every rebuild, so it skips the existence check, as
// KnownFolderPath does.
std::wstring SuggestedBackupPath() {
    PWSTR profile = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_DONT_VERIFY,
                                    nullptr, &profile)) || !profile) {
        return {};
    }
    std::wstring path(profile);
    CoTaskMemFree(profile);
    if (path.empty()) {
        return {};
    }
    if (path.back() != L'\\') {
        path += L'\\';
    }
    return path + kExportFileName;
}

// Without KF_FLAG_DONT_VERIFY, Windows checks that the folder exists, which
// can stall on a folder redirected to an unreachable share. Callers apply
// CheckFolderStatus instead, which never probes network storage.
std::wstring KnownFolderPath(REFKNOWNFOLDERID folderId) {
    PWSTR value = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_DONT_VERIFY, nullptr,
                                    &value)) || !value) {
        return {};
    }
    std::wstring path(value);
    CoTaskMemFree(value);
    return NormalizePath(std::move(path));
}

std::wstring DriveMenuLabel(const std::wstring& root) {
    if (root.size() < 2 || root[1] != L':') {
        return root;
    }
    std::wstring letter = root.substr(0, 2);
    UINT type = GetDriveTypeW(root.c_str());
    if (type == DRIVE_FIXED || type == DRIVE_RAMDISK) {
        wchar_t volumeName[MAX_PATH + 1]{};
        if (GetVolumeInformationW(root.c_str(), volumeName, MAX_PATH + 1,
                                  nullptr, nullptr, nullptr, nullptr, 0) &&
            volumeName[0]) {
            return letter + L" — " + volumeName;
        }
    }
    const wchar_t* fallback = L"Unavailable";
    switch (type) {
        case DRIVE_FIXED: fallback = L"Local disk"; break;
        case DRIVE_REMOVABLE: fallback = L"Removable drive"; break;
        case DRIVE_REMOTE: fallback = L"Network drive"; break;
        case DRIVE_CDROM: fallback = L"Optical drive"; break;
        case DRIVE_RAMDISK: fallback = L"RAM disk"; break;
    }
    return letter + L" — " + fallback;
}

struct ScopedFile {
    HANDLE value = INVALID_HANDLE_VALUE;
    explicit ScopedFile(HANDLE handle = INVALID_HANDLE_VALUE) : value(handle) {}
    ~ScopedFile() {
        if (value != INVALID_HANDLE_VALUE) {
            CloseHandle(value);
        }
    }
    ScopedFile(const ScopedFile&) = delete;
    ScopedFile& operator=(const ScopedFile&) = delete;
};

bool IsAbsoluteFolderPath(const std::wstring& path) {
    bool drive = path.size() >= 3 &&
                 ((path[0] >= L'A' && path[0] <= L'Z') ||
                  (path[0] >= L'a' && path[0] <= L'z')) &&
                 path[1] == L':' && path[2] == L'\\';
    bool unc = path.size() >= 5 && path[0] == L'\\' &&
               path[1] == L'\\' && path[2] != L'\\' &&
               path.find(L'\\', 2) != std::wstring::npos;
    return (drive || unc) &&
           std::none_of(path.begin(), path.end(), [](wchar_t ch) {
               return ch < 32;
           });
}

bool ValidateImportedFolders(const std::vector<std::wstring>& folders) {
    if (folders.size() > kMaxBookmarks) {
        return false;
    }
    size_t storageChars = 0;
    for (size_t i = 0; i < folders.size(); ++i) {
        const auto& path = folders[i];
        if (!IsAbsoluteFolderPath(path) || path != NormalizePath(path) ||
            path.find(L'\n') != std::wstring::npos ||
            path.find(L'\r') != std::wstring::npos) {
            return false;
        }
        storageChars += path.size() + (i != 0);
        if (storageChars > kMaxStorageChars) {
            return false;
        }
        for (size_t j = 0; j < i; ++j) {
            if (SamePath(path, folders[j])) {
                return false;
            }
        }
    }
    return true;
}

bool WideToUtf8(const std::wstring& wide, std::string& utf8) {
    if (wide.size() > INT_MAX) {
        return false;
    }
    int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                                    wide.data(), static_cast<int>(wide.size()),
                                    nullptr, 0, nullptr, nullptr);
    if (!count) {
        return false;
    }
    utf8.resize(count);
    return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                               wide.data(), static_cast<int>(wide.size()),
                               utf8.data(), count, nullptr, nullptr) == count;
}

bool Utf8ToWide(const std::string& utf8, std::wstring& wide) {
    if (utf8.empty() || utf8.size() > INT_MAX) {
        return false;
    }
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                    utf8.data(), static_cast<int>(utf8.size()),
                                    nullptr, 0);
    if (!count) {
        return false;
    }
    wide.resize(count);
    return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                               utf8.data(), static_cast<int>(utf8.size()),
                               wide.data(), count) == count;
}

bool SaveBackup(const std::wstring& path) try {
    if (path.empty()) {
        return false;
    }
    std::vector<std::wstring> folders;
    {
        std::lock_guard lock(g_storageMutex);
        folders = SplitBookmarks(ReadStorageLocked());
    }
    wjson::JsonObject root;
    root.SetNamedValue(L"format", wjson::JsonValue::CreateStringValue(
                                       L"explorer-folder-bookmarks-bar"));
    root.SetNamedValue(L"version", wjson::JsonValue::CreateNumberValue(1));
    wjson::JsonArray list;
    for (const auto& folder : folders) {
        list.Append(wjson::JsonValue::CreateStringValue(folder));
    }
    root.SetNamedValue(L"folders", list);
    std::string bytes;
    auto jsonText = root.Stringify();
    if (!WideToUtf8(std::wstring(jsonText.c_str()), bytes) ||
        bytes.size() > kMaxImportBytes) {
        return false;
    }
    std::wstring directory = path.substr(0, path.find_last_of(L'\\'));
    wchar_t temp[MAX_PATH]{};
    if (!GetTempFileNameW(directory.c_str(), L"efb", 0, temp)) {
        Wh_Log(L"Bookmark backup temp file failed: %lu", GetLastError());
        return false;
    }
    bool saved = false;
    {
        ScopedFile file(CreateFileW(temp, GENERIC_WRITE, 0, nullptr,
                                    TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                    nullptr));
        if (file.value != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            saved = WriteFile(file.value, bytes.data(),
                              static_cast<DWORD>(bytes.size()), &written,
                              nullptr) && written == bytes.size() &&
                    FlushFileBuffers(file.value);
        }
    }
    if (saved) {
        saved = MoveFileExW(temp, path.c_str(),
                            MOVEFILE_REPLACE_EXISTING |
                                MOVEFILE_WRITE_THROUGH) != 0;
    }
    if (!saved) {
        Wh_Log(L"Bookmark backup save failed: %lu", GetLastError());
        DeleteFileW(temp);
    }
    return saved;
} catch (...) {
    Wh_Log(L"Bookmark backup serialization failed: %08X",
           winrt::to_hresult().value);
    return false;
}

bool LoadBackup(const std::wstring& path) try {
    if (path.empty()) {
        return false;
    }
    ScopedFile file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                nullptr));
    if (file.value == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Bookmark backup open failed: %lu", GetLastError());
        return false;
    }
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.value, &size) || size.QuadPart <= 0 ||
        size.QuadPart > kMaxImportBytes) {
        return false;
    }
    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD read = 0;
    if (!ReadFile(file.value, bytes.data(), static_cast<DWORD>(bytes.size()),
                  &read, nullptr) || read != bytes.size()) {
        return false;
    }
    // A backup saved with a UTF-8 byte order mark is still a valid backup.
    if (bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB &&
        static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
        Wh_Log(L"Bookmark backup: skipped a UTF-8 byte order mark");
    }
    std::wstring wide;
    if (!Utf8ToWide(bytes, wide)) {
        return false;
    }
    wjson::JsonObject root{nullptr};
    if (!wjson::JsonObject::TryParse(wide, root) ||
        root.GetNamedString(L"format") != L"explorer-folder-bookmarks-bar" ||
        root.GetNamedNumber(L"version") != 1) {
        return false;
    }
    auto list = root.GetNamedArray(L"folders");
    if (list.Size() > kMaxBookmarks) {
        return false;
    }
    std::vector<std::wstring> folders;
    folders.reserve(list.Size());
    for (unsigned i = 0; i < list.Size(); ++i) {
        if (list.GetAt(i).ValueType() != wjson::JsonValueType::String) {
            return false;
        }
        auto value = list.GetStringAt(i);
        // Keep the full length: a JSON \u0000 escape yields an embedded NUL,
        // which validation must see and reject instead of a shortened path.
        folders.emplace_back(value.data(), value.size());
    }
    if (!ValidateImportedFolders(folders)) {
        Wh_Log(L"Bookmark backup rejected: a folder path is invalid, "
               L"duplicated or too long");
        return false;
    }
    std::lock_guard lock(g_storageMutex);
    return SaveBookmarksLocked(folders);
} catch (...) {
    Wh_Log(L"Bookmark backup validation failed: %08X",
           winrt::to_hresult().value);
    return false;
}

// What the user is told after adding bookmarks that could not all be added.
enum class BookmarkNotice : unsigned {
    None,
    Full,
    Partial,
    AlreadyBookmarked,
};

// The outcome of adding folders to the bookmark list.
struct BookmarkChange {
    bool changed = false;
    size_t added = 0;
    // Folders left out because the list holds kMaxBookmarks already.
    size_t noRoom = 0;
    size_t existing = 0;
    size_t invalid = 0;

    BookmarkNotice Notice() const {
        if (noRoom != 0) {
            return added != 0 ? BookmarkNotice::Partial : BookmarkNotice::Full;
        }
        if (added == 0 && existing != 0 && invalid == 0) {
            return BookmarkNotice::AlreadyBookmarked;
        }
        return BookmarkNotice::None;
    }
};

BookmarkChange AddBookmark(std::wstring path) {
    BookmarkChange change;
    path = NormalizePath(std::move(path));
    if (path.empty() || path.find_first_of(L"\r\n") != std::wstring::npos) {
        return change;
    }
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    if (std::any_of(folders.begin(), folders.end(),
                    [&](const auto& old) { return SamePath(old, path); })) {
        change.existing = 1;
        return change;
    }
    if (folders.size() >= kMaxBookmarks) {
        change.noRoom = 1;
        return change;
    }
    folders.push_back(std::move(path));
    change.changed = SaveBookmarksLocked(folders);
    change.added = change.changed ? 1 : 0;
    return change;
}

bool RemoveBookmark(const std::wstring& path) {
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    size_t oldSize = folders.size();
    std::erase_if(folders, [&](const auto& old) { return SamePath(old, path); });
    return oldSize != folders.size() && SaveBookmarksLocked(folders);
}

bool MoveBookmarkToIndex(const std::wstring& source, size_t insertionIndex) {
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    if (insertionIndex > folders.size()) {
        return false;
    }
    auto sourceIt = std::find_if(folders.begin(), folders.end(),
                                 [&](const auto& path) {
                                     return SamePath(path, source);
                                 });
    if (sourceIt == folders.end()) {
        return false;
    }
    size_t sourceIndex = std::distance(folders.begin(), sourceIt);
    if (sourceIndex < insertionIndex) {
        --insertionIndex;
    }
    if (sourceIndex == insertionIndex) {
        return false;
    }
    std::wstring moved = std::move(*sourceIt);
    folders.erase(sourceIt);
    folders.insert(folders.begin() + insertionIndex, std::move(moved));
    return SaveBookmarksLocked(folders);
}

// Inserts the paths that are not bookmarked yet, in their given order, while
// room remains, and counts what was inserted and what was left out. The
// caller saves when anything was added.
BookmarkChange InsertMissingLocked(std::vector<std::wstring>& folders,
                                   const std::vector<std::wstring>& paths,
                                   size_t insertionIndex) {
    insertionIndex = std::min(insertionIndex, folders.size());
    BookmarkChange change;
    for (auto path : paths) {
        path = NormalizePath(std::move(path));
        if (!IsAbsoluteFolderPath(path)) {
            ++change.invalid;
        } else if (std::any_of(folders.begin(), folders.end(),
                               [&](const auto& old) {
                                   return SamePath(old, path);
                               })) {
            ++change.existing;
        } else if (folders.size() >= kMaxBookmarks) {
            ++change.noRoom;
        } else {
            folders.insert(folders.begin() + insertionIndex + change.added,
                           std::move(path));
            ++change.added;
        }
    }
    return change;
}

// Saves what InsertMissingLocked inserted. A failed save leaves the stored
// list unchanged, so nothing counts as added.
BookmarkChange SaveInsertedLocked(const std::vector<std::wstring>& folders,
                                  BookmarkChange change) {
    change.changed = change.added != 0 && SaveBookmarksLocked(folders);
    if (!change.changed) {
        change.added = 0;
    }
    return change;
}

// insertionIndex past the end appends.
BookmarkChange InsertBookmarks(const std::vector<std::wstring>& paths,
                               size_t insertionIndex) {
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    auto change = InsertMissingLocked(folders, paths, insertionIndex);
    return SaveInsertedLocked(folders, change);
}

BookmarkChange InsertBookmarkAt(const std::wstring& path,
                                size_t insertionIndex) {
    return InsertBookmarks({path}, insertionIndex);
}

// Removes the paths when every one of them is bookmarked; otherwise appends
// the missing ones.
BookmarkChange ToggleBookmarks(const std::vector<std::wstring>& paths) {
    std::vector<std::wstring> normalized;
    for (const auto& path : paths) {
        if (auto value = NormalizePath(path); !value.empty()) {
            normalized.push_back(std::move(value));
        }
    }
    if (normalized.empty()) {
        return {};
    }
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    auto bookmarked = [&](const std::wstring& path) {
        return std::any_of(folders.begin(), folders.end(),
                           [&](const auto& old) { return SamePath(old, path); });
    };
    if (std::all_of(normalized.begin(), normalized.end(), bookmarked)) {
        std::erase_if(folders, [&](const auto& old) {
            return std::any_of(normalized.begin(), normalized.end(),
                               [&](const auto& path) {
                                   return SamePath(old, path);
                               });
        });
        BookmarkChange removal;
        removal.changed = SaveBookmarksLocked(folders);
        return removal;
    }
    auto change = InsertMissingLocked(folders, normalized, folders.size());
    return SaveInsertedLocked(folders, change);
}

std::wstring ButtonLabel(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos || pos + 1 == path.size()) {
        return path;
    }
    return path.substr(pos + 1);
}

// A button label: the folder name, plus enough parent folders to tell it
// apart from other buttons with the same name (empty when not needed).
struct FolderLabel {
    std::wstring parent;
    std::wstring name;
};

std::wstring LabelText(const FolderLabel& label) {
    return label.parent.empty() ? label.name : label.parent + L"\\" + label.name;
}

std::vector<FolderLabel> FolderLabels(const std::vector<std::wstring>& paths,
                                      bool showParents) {
    std::vector<FolderLabel> labels;
    std::vector<std::vector<std::wstring>> parents(paths.size());
    for (size_t i = 0; i < paths.size(); ++i) {
        labels.push_back({{}, ButtonLabel(paths[i])});
        // Drive roots and other paths without a folder name keep their label.
        if (!showParents || labels[i].name == paths[i]) {
            continue;
        }
        size_t start = 0;
        while (start < paths[i].size()) {
            size_t end = paths[i].find_first_of(L"\\/", start);
            if (end == std::wstring::npos) {
                end = paths[i].size();
            }
            if (end > start) {
                parents[i].push_back(paths[i].substr(start, end - start));
            }
            start = end + 1;
        }
        if (!parents[i].empty()) {
            parents[i].pop_back();
        }
    }
    auto suffix = [&](size_t index, size_t levels) {
        const auto& parts = parents[index];
        std::wstring text;
        for (size_t k = parts.size() - std::min(levels, parts.size());
             k < parts.size(); ++k) {
            if (!text.empty()) {
                text += L'\\';
            }
            text += parts[k];
        }
        return text;
    };
    for (size_t i = 0; i < paths.size(); ++i) {
        if (parents[i].empty()) {
            continue;
        }
        std::vector<size_t> others;
        for (size_t j = 0; j < paths.size(); ++j) {
            if (j != i && !parents[j].empty() &&
                SamePath(labels[j].name, labels[i].name)) {
                others.push_back(j);
            }
        }
        if (others.empty()) {
            continue;
        }
        // Add parent levels until no other button shows the same text.
        size_t levels = 1;
        while (levels < parents[i].size() &&
               std::any_of(others.begin(), others.end(), [&](size_t j) {
                   return SamePath(suffix(i, levels), suffix(j, levels));
               })) {
            ++levels;
        }
        labels[i].parent = suffix(i, levels);
    }
    return labels;
}

std::wstring TrimSetting(std::wstring value) {
    auto nonSpace = [](wchar_t ch) { return !iswspace(ch); };
    auto first = std::find_if(value.begin(), value.end(), nonSpace);
    if (first == value.end()) {
        return {};
    }
    auto last = std::find_if(value.rbegin(), value.rend(), nonSpace).base();
    return std::wstring(first, last);
}

std::wstring ReadFxSetting(int index, const wchar_t* field) {
    std::wstring name = L"fxCustomFolders[" + std::to_wstring(index) +
                        L"]." + field;
    auto value = WindhawkUtils::StringSetting::make(name.c_str());
    return TrimSetting(value.get());
}

std::wstring ExpandFxPath(const std::wstring& raw) {
    if (raw.empty() || raw.size() > 4096) {
        return {};
    }
    DWORD required = ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0);
    if (required == 0 || required > 32768) {
        return {};
    }
    std::wstring expanded(required, L'\0');
    DWORD written = ExpandEnvironmentStringsW(raw.c_str(), expanded.data(),
                                               required);
    if (written == 0 || written > required) {
        return {};
    }
    expanded.resize(written - 1);
    auto path = NormalizePath(TrimSetting(std::move(expanded)));
    return IsAbsoluteFolderPath(path) ? path : std::wstring{};
}

std::vector<FxFolder> LoadFxCustomFolders() {
    std::vector<FxFolder> folders;
    for (int index = 0; index < static_cast<int>(kMaxCustomFxFolders);
         ++index) {
        auto rawPath = ReadFxSetting(index, L"path");
        if (rawPath.empty()) {
            continue;
        }
        auto path = ExpandFxPath(rawPath);
        if (path.empty()) {
            Wh_Log(L"Skipping invalid FX folder setting at index %d", index);
            continue;
        }
        if (std::any_of(folders.begin(), folders.end(),
                        [&](const FxFolder& entry) {
                            return SamePath(entry.path, path);
                        })) {
            continue;
        }
        auto label = ReadFxSetting(index, L"label");
        if (label.empty()) {
            label = ButtonLabel(path);
        }
        if (label.size() > 80 ||
            std::any_of(label.begin(), label.end(), [](wchar_t ch) {
                return ch < 32;
            })) {
            Wh_Log(L"Skipping invalid FX folder label at index %d", index);
            continue;
        }
        folders.push_back({std::move(label), std::move(path)});
    }
    return folders;
}

struct ComScope {
    HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ~ComScope() {
        if (SUCCEEDED(result)) {
            CoUninitialize();
        }
    }
};

void LoadSettings() {
    RecentSettings recent;
    recent.enabled = Wh_GetIntSetting(L"recent.enabled") != 0;
    recent.buttons = static_cast<size_t>(
        std::clamp(Wh_GetIntSetting(L"recent.buttons"), 1, kMaxRecentButtons));
    recent.history = static_cast<size_t>(std::clamp(
        Wh_GetIntSetting(L"recent.history"),
        static_cast<int>(recent.buttons), kMaxRecentHistory));
    // An unknown value keeps the default: RC shown, right-click offers Clear.
    auto rcButton = WindhawkUtils::StringSetting::make(L"recent.rcButton");
    const std::wstring_view rcMode = rcButton.get();
    recent.menuButton = rcMode != L"hidden";
    recent.rightClickClears = rcMode == L"clear";
    auto folders = LoadFxCustomFolders();
    const bool hotkey = Wh_GetIntSetting(L"bookmarkHotkey") != 0;
    const bool parents = Wh_GetIntSetting(L"parentForDuplicates") != 0;
    {
        std::lock_guard lock(g_settingsMutex);
        g_sharedRecentSettings = recent;
        g_sharedFxFolders = std::move(folders);
        g_sharedBookmarkHotkey = hotkey;
        g_sharedParentForDuplicates = parents;
        ++g_settingsGeneration;
    }
    Wh_Log(L"Recent folders: %ls, %zu button(s), %zu remembered, RC %ls; "
           L"parents for duplicates %ls",
           recent.enabled ? L"on" : L"off", recent.buttons,
           recent.history, rcMode.data(), parents ? L"on" : L"off");
}

// Recent folders stay in this Explorer process's memory, newest first. Each
// account runs its own Explorer processes, so no other account sees them,
// and nothing is written to the storage that all accounts share.
std::mutex g_recentsMutex;
std::vector<std::wstring> g_recents;

// Returns true when the order changed.
bool RecordExplorerRecent(std::wstring path) {
    path = NormalizePath(std::move(path));
    if (!IsAbsoluteFolderPath(path)) {
        return false;
    }
    {
        std::lock_guard lock(g_recentsMutex);
        if (!g_recents.empty() && SamePath(g_recents.front(), path)) {
            return false;
        }
        std::erase_if(g_recents,
                      [&](const auto& old) { return SamePath(old, path); });
        g_recents.insert(g_recents.begin(), std::move(path));
        if (g_recents.size() > kMaxStoredRecents) {
            g_recents.resize(kMaxStoredRecents);
        }
    }
    NotifyExplorerBars(true);
    return true;
}

void RemoveRecent(const std::wstring& path) {
    {
        std::lock_guard lock(g_recentsMutex);
        if (!std::erase_if(g_recents, [&](const auto& old) {
                return SamePath(old, path);
            })) {
            return;
        }
    }
    NotifyExplorerBars(true);
}

void ClearRecents() {
    {
        std::lock_guard lock(g_recentsMutex);
        g_recents.clear();
    }
    NotifyExplorerBars(true);
}

// Nothing is recorded while recent folders are off, and the remembered list
// is erased, so turning the feature off also clears its history.
void EraseRecentsWhileOff() {
    {
        std::lock_guard lock(g_settingsMutex);
        if (g_sharedRecentSettings.enabled) {
            return;
        }
    }
    std::lock_guard lock(g_recentsMutex);
    if (!g_recents.empty()) {
        g_recents.clear();
        Wh_Log(L"Recent folders are off: remembered folders erased");
    }
}

struct RecentView {
    // Newest first, at most the configured history length.
    std::vector<std::wstring> folders;
    // Labels of the folders that get a button.
    std::vector<FolderLabel> labels;
};

RecentView LoadRecentView(const std::vector<std::wstring>& bookmarks) {
    RecentView view;
    std::vector<std::wstring> candidates;
    {
        std::lock_guard lock(g_recentsMutex);
        candidates = g_recents;
    }
    for (auto& path : candidates) {
        if (view.folders.size() >= g_recentSettings.history) {
            break;
        }
        auto samePath = [&](const auto& other) { return SamePath(other, path); };
        if (std::any_of(bookmarks.begin(), bookmarks.end(), samePath) ||
            std::any_of(view.folders.begin(), view.folders.end(), samePath) ||
            CheckFolderStatus(path) == FolderStatus::Missing) {
            continue;
        }
        view.folders.push_back(std::move(path));
    }
    return view;
}

std::wstring ChooseBackupPath(HWND owner, bool save,
                              const std::wstring& suggestedPath) {
    ComScope com;
    if (FAILED(com.result)) {
        return {};
    }
    winrt::com_ptr<IFileDialog> dialog;
    const CLSID& dialogClass = save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog;
    if (FAILED(CoCreateInstance(dialogClass, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dialog.put())))) {
        return {};
    }
    FILEOPENDIALOGOPTIONS options = 0;
    if (FAILED(dialog->GetOptions(&options)) ||
        FAILED(dialog->SetOptions(
            options | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST |
            (save ? FOS_OVERWRITEPROMPT : FOS_FILEMUSTEXIST)))) {
        return {};
    }
    const COMDLG_FILTERSPEC filters[] = {{L"JSON files", L"*.json"},
                                         {L"All files", L"*.*"}};
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);
    dialog->SetDefaultExtension(L"json");
    auto separator = suggestedPath.find_last_of(L"\\/");
    if (separator != std::wstring::npos) {
        dialog->SetFileName(suggestedPath.c_str() + separator + 1);
        winrt::com_ptr<IShellItem> folder;
        if (SUCCEEDED(SHCreateItemFromParsingName(
                suggestedPath.substr(0, separator).c_str(), nullptr,
                IID_PPV_ARGS(folder.put())))) {
            dialog->SetDefaultFolder(folder.get());
        }
    }
    if (g_unloading) {
        return {};
    }
    g_threadFileDialog = dialog.get();
    HRESULT showResult = dialog->Show(owner);
    g_threadFileDialog = nullptr;
    if (FAILED(showResult)) {
        return {};
    }
    winrt::com_ptr<IShellItem> selected;
    if (FAILED(dialog->GetResult(selected.put()))) {
        return {};
    }
    PWSTR rawPath = nullptr;
    if (FAILED(selected->GetDisplayName(SIGDN_FILESYSPATH, &rawPath)) ||
        !rawPath) {
        CoTaskMemFree(rawPath);
        return {};
    }
    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

winrt::com_ptr<IShellBrowser> ActiveShellBrowser(HWND explorerWindow) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put())))) {
        return nullptr;
    }

    HWND activeTab = FindWindowExW(explorerWindow, nullptr,
                                   L"ShellTabWindowClass", nullptr);
    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) {
        return nullptr;
    }

    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch) {
            continue;
        }
        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        SHANDLE_PTR windowValue = 0;
        if (!webBrowser || FAILED(webBrowser->get_HWND(&windowValue)) ||
            reinterpret_cast<HWND>(windowValue) != explorerWindow) {
            continue;
        }
        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider) {
            continue;
        }
        winrt::com_ptr<IShellBrowser> browser;
        if (FAILED(serviceProvider->QueryService(
                kTopLevelBrowser, IID_PPV_ARGS(browser.put())))) {
            continue;
        }
        HWND tabWindow = nullptr;
        if (FAILED(browser->GetWindow(&tabWindow)) || !tabWindow) {
            continue;
        }
        if ((activeTab && tabWindow != activeTab) ||
            (!activeTab && !IsWindowVisible(tabWindow))) {
            continue;
        }
        return browser;
    }
    return nullptr;
}

bool IsExplorerFrame(HWND window) {
    wchar_t className[64]{};
    return window &&
           GetClassNameW(window, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CabinetWClass") == 0;
}

// The Explorer frame that hosts this element's XAML island, or null.
HWND IslandWindow(const mux::UIElement& element) {
    try {
        if (auto root = element.XamlRoot()) {
            if (auto environment = root.ContentIslandEnvironment()) {
                HWND window = GetAncestor(
                    reinterpret_cast<HWND>(static_cast<uintptr_t>(
                        environment.AppWindowId().Value)),
                    GA_ROOT);
                return IsExplorerFrame(window) ? window : nullptr;
            }
        }
    } catch (...) {
        Wh_Log(L"Could not locate Explorer window: %08X",
               winrt::to_hresult().value);
    }
    return nullptr;
}

HWND ExplorerWindowForElement(const mux::FrameworkElement& element) {
    if (HWND window = IslandWindow(element)) {
        return window;
    }
    HWND window = GetActiveWindow();
    window = window ? GetAncestor(window, GA_ROOT) : nullptr;
    return IsExplorerFrame(window) ? window : nullptr;
}

// A short notice under the bar when folders could not all be bookmarked.
// The flyout is transient, so it should not take focus from the file list; the
// thread timer hides it. Drops finish off the UI thread and reach it through
// the refresh message, which carries the notice in its WPARAM.
struct PendingNotice {
    HWND frame = nullptr;
    BookmarkNotice kind = BookmarkNotice::None;
    unsigned added = 0;
    unsigned total = 0;
};
thread_local PendingNotice g_pendingNotice;
thread_local UINT_PTR g_noticeTimer = 0;

PendingNotice MakeNotice(HWND frame, const BookmarkChange& change) {
    return {frame, change.Notice(), static_cast<unsigned>(change.added),
            static_cast<unsigned>(change.added + change.noRoom)};
}

WPARAM EncodeNotice(const BookmarkChange& change) {
    auto byte = [](size_t value) {
        return static_cast<WPARAM>(std::min<size_t>(value, 255));
    };
    return static_cast<WPARAM>(change.Notice()) | (byte(change.added) << 8) |
           (byte(change.added + change.noRoom) << 16);
}

PendingNotice DecodeNotice(HWND frame, WPARAM value) {
    auto kind = static_cast<BookmarkNotice>(value & 0xFF);
    if (kind > BookmarkNotice::AlreadyBookmarked) {
        kind = BookmarkNotice::None;
    }
    return {frame, kind, static_cast<unsigned>((value >> 8) & 0xFF),
            static_cast<unsigned>((value >> 16) & 0xFF)};
}

std::wstring NoticeText(const PendingNotice& notice) {
    const std::wstring limit = std::to_wstring(kMaxBookmarks);
    switch (notice.kind) {
        case BookmarkNotice::Full:
            return L"Bookmarks full (" + limit + L" folders)";
        case BookmarkNotice::Partial:
            return L"Added " + std::to_wstring(notice.added) + L" of " +
                   std::to_wstring(notice.total) + L": bookmarks full (" +
                   limit + L")";
        case BookmarkNotice::AlreadyBookmarked:
            return L"Already bookmarked";
        default:
            return {};
    }
}

void HideNotices() {
    if (g_noticeTimer) {
        KillTimer(nullptr, g_noticeTimer);
        g_noticeTimer = 0;
    }
    for (auto& bar : *g_bars) {
        if (!bar.notice) {
            continue;
        }
        try {
            bar.notice.Hide();
        } catch (...) {
            Wh_Log(L"Could not hide the notice: %08X",
                   winrt::to_hresult().value);
        }
    }
}

void CALLBACK NoticeTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    if (id != g_noticeTimer) {
        KillTimer(nullptr, id);
        return;
    }
    HideNotices();
}

void ShowBookmarkNotice(const PendingNotice& notice) try {
    if (notice.kind == BookmarkNotice::None || g_unloading) {
        return;
    }
    BarState* target = nullptr;
    for (auto& bar : *g_bars) {
        auto root = bar.barRoot.get();
        if (root && root.IsLoaded() &&
            (!notice.frame || IslandWindow(root) == notice.frame)) {
            target = &bar;
            break;
        }
    }
    if (!target) {
        Wh_Log(L"Notice %d dropped: no loaded bar for the window",
               static_cast<int>(notice.kind));
        return;
    }
    if (!target->notice) {
        muxc::TextBlock text;
        muxc::Flyout flyout;
        flyout.Content(text);
        flyout.Placement(muxp::FlyoutPlacementMode::Bottom);
        target->noticeText = text;
        target->notice = flyout;
    }
    target->noticeText.Text(winrt::hstring(NoticeText(notice)));
    const bool wasOpen = target->notice.IsOpen();
    if (!wasOpen) {
        muxp::FlyoutShowOptions options;
        options.Placement(muxp::FlyoutPlacementMode::Bottom);
        options.ShowMode(muxp::FlyoutShowMode::Transient);
        target->notice.ShowAt(target->barRoot.get(), options);
    }
    if (g_noticeTimer) {
        KillTimer(nullptr, g_noticeTimer);
    }
    g_noticeTimer = SetTimer(nullptr, 0, kNoticeMs, NoticeTimerProc);
    Wh_Log(L"Notice shown: kind=%d, added %u of %u, already open=%d, "
           L"timer=%d",
           static_cast<int>(notice.kind), notice.added, notice.total,
           wasOpen ? 1 : 0, g_noticeTimer ? 1 : 0);
    if (!g_noticeTimer) {
        // Without a timer nothing would close it.
        target->notice.Hide();
    }
} catch (...) {
    Wh_Log(L"Could not show the notice: %08X", winrt::to_hresult().value);
}

std::wstring ShellBrowserFolder(IShellBrowser* browser) {
    winrt::com_ptr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put())) || !view) {
        return {};
    }
    auto folderView = view.try_as<IFolderView>();
    winrt::com_ptr<IPersistFolder2> persistFolder;
    if (!folderView ||
        FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put())))) {
        return {};
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(persistFolder->GetCurFolder(&pidl)) || !pidl) {
        return {};
    }
    std::vector<wchar_t> path(32768);
    bool ok = SHGetPathFromIDListEx(pidl, path.data(), path.size(),
                                    GPFIDL_DEFAULT);
    CoTaskMemFree(pidl);
    return ok ? NormalizePath(path.data()) : std::wstring{};
}

std::wstring CurrentFolder(HWND explorerWindow) {
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    return browser ? ShellBrowserFolder(browser.get()) : std::wstring{};
}

// Filesystem folders selected in the active tab. ZIP archives and other items
// that are both a folder and a file are left out.
std::vector<std::wstring> SelectedFolders(HWND explorerWindow) {
    std::vector<std::wstring> folders;
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    winrt::com_ptr<IShellView> view;
    if (!browser || FAILED(browser->QueryActiveShellView(view.put())) ||
        !view) {
        return folders;
    }
    auto folderView = view.try_as<IFolderView>();
    winrt::com_ptr<IShellItemArray> items;
    DWORD count = 0;
    if (!folderView ||
        FAILED(folderView->Items(SVGIO_SELECTION,
                                 IID_PPV_ARGS(items.put()))) ||
        !items || FAILED(items->GetCount(&count))) {
        return folders;
    }
    constexpr SFGAOF kFilesystemFolder = SFGAO_FOLDER | SFGAO_FILESYSTEM;
    const DWORD scanLimit = std::min<DWORD>(count, kMaxSelectionScanned);
    for (DWORD i = 0; i < scanLimit && folders.size() < kMaxFoldersPerAction;
         ++i) {
        winrt::com_ptr<IShellItem> item;
        SFGAOF attributes = 0;
        if (FAILED(items->GetItemAt(i, item.put())) ||
            FAILED(item->GetAttributes(kFilesystemFolder | SFGAO_STREAM,
                                       &attributes)) ||
            (attributes & kFilesystemFolder) != kFilesystemFolder ||
            (attributes & SFGAO_STREAM)) {
            continue;
        }
        PWSTR path = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
            folders.push_back(NormalizePath(path));
        }
        CoTaskMemFree(path);
    }
    return folders;
}

void NavigateToFolder(HWND explorerWindow, const std::wstring& path) {
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    if (!browser) {
        return;
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr) && pidl) {
        hr = browser->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
        CoTaskMemFree(pidl);
    }
    if (FAILED(hr)) {
        Wh_Log(L"Bookmark navigation failed: %08X", hr);
    }
}

// Without SEE_MASK_FLAG_NO_UI, Windows shows its own error box for a missing
// or unreachable folder. That box runs a modal loop inside the click handler,
// and unloading would wait until the user closes it. A failure is logged,
// as for a plain click.
bool LaunchFolderVerb(HWND owner, const wchar_t* verb, const std::wstring& path) {
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    info.hwnd = owner;
    info.lpVerb = verb;
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&info)) {
        return true;
    }
    Wh_Log(L"Bookmark %ls failed: %u", verb, GetLastError());
    return false;
}

void OpenFolderInNewTab(HWND explorerWindow, const std::wstring& path) {
    // Windows 11 registers this Folder shell verb for Explorer tabs. Explorer
    // chooses which window receives the tab; HWND is the owner for the request.
    if (LaunchFolderVerb(explorerWindow, L"opennewtab", path)) {
        Wh_Log(L"Bookmark opened in a new tab: %ls", path.c_str());
    }
}

void OpenFolderInNewWindow(HWND explorerWindow, const std::wstring& path) {
    // The Folder verb behind Explorer's "Open in new window" command.
    if (LaunchFolderVerb(explorerWindow, L"opennewwindow", path)) {
        Wh_Log(L"Bookmark opened in a new window: %ls", path.c_str());
    }
}

// Opens the folder in the Explorer window that hosts anchor: Ctrl+click
// opens a new tab, an ordinary click navigates the active tab.
void OpenFolderFrom(const mux::FrameworkElement& anchor,
                    const std::wstring& path) {
    HWND window = anchor ? ExplorerWindowForElement(anchor) : nullptr;
    if (!window) {
        return;
    }
    if (GetKeyState(VK_CONTROL) & 0x8000) {
        OpenFolderInNewTab(window, path);
    } else {
        NavigateToFolder(window, path);
    }
}

BarState* FindState(const muxc::StackPanel& panel) {
    for (auto& state : *g_bars) {
        if (state.buttons.get() == panel) {
            return &state;
        }
    }
    return nullptr;
}

std::vector<muxc::Button> BarButtons(const muxc::StackPanel& panel) {
    std::vector<muxc::Button> buttons;
    for (const auto& rowElement : panel.Children()) {
        if (auto row = rowElement.try_as<muxc::StackPanel>()) {
            for (const auto& child : row.Children()) {
                if (auto button = child.try_as<muxc::Button>()) {
                    buttons.push_back(button);
                }
            }
        }
    }
    return buttons;
}

// Ask Explorer to recalculate its stock header after the mod is unloaded.
void RelayoutThreadFrames() {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM) -> BOOL {
            if (IsExplorerFrame(window) && !IsIconic(window)) {
                SetWindowPos(window, nullptr, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
                RECT client{};
                if (GetClientRect(window, &client)) {
                    SendMessageW(window, WM_SIZE,
                                 IsZoomed(window) ? SIZE_MAXIMIZED
                                                  : SIZE_RESTORED,
                                 MAKELPARAM(client.right, client.bottom));
                }
            }
            return TRUE;
        },
        0);
}

// A native WM_SIZE has no callback into the mod and runs after the current
// XAML layout pass, so row changes do not reenter Explorer's layout code.
void PostFrameRelayout() {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM) -> BOOL {
            if (IsExplorerFrame(window) && !IsIconic(window)) {
                RECT client{};
                if (GetClientRect(window, &client)) {
                    PostMessageW(window, WM_SIZE,
                                 IsZoomed(window) ? SIZE_MAXIMIZED
                                                  : SIZE_RESTORED,
                                 MAKELPARAM(client.right, client.bottom));
                }
            }
            return TRUE;
        },
        0);
}

void ClearDrag(BarState& state);

// A mouse press can end without PointerReleased when Alt+Tab or a system
// dialog takes the mouse. Once the button is physically up the drag is over,
// so refreshes never leave a drag operation active.
bool DragInProgress(BarState& state) {
    if (state.dragPath.empty()) {
        return false;
    }
    if (state.dragIsMouse) {
        int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
        if (!(GetAsyncKeyState(button) & 0x8000)) {
            ClearDrag(state);
            return false;
        }
    }
    return true;
}

void ClearDropTarget(BarState& state) {
    if (auto button = state.dropTarget.get()) {
        try {
            // These bookmark buttons are ours; Tag holds the original brush
            // on the button itself instead of in thread-local state.
            auto oldBrush = button.Tag().try_as<muxm::Brush>();
            button.BorderBrush(oldBrush);
            button.Tag(nullptr);
            button.BorderThickness(state.dropOldThickness);
        } catch (...) {
            Wh_Log(L"Could not restore bookmark drag border: %08X",
                   winrt::to_hresult().value);
        }
    }
    state.dropTarget = nullptr;
    state.dropAfter = false;
}

void ClearDrag(BarState& state) {
    ClearDropTarget(state);
    if (auto button = state.dragButton.get()) {
        try {
            button.Opacity(state.dragOldOpacity);
        } catch (...) {
            Wh_Log(L"Could not restore bookmark drag opacity: %08X",
                   winrt::to_hresult().value);
        }
    }
    state.dragPath.clear();
    state.dragButton = nullptr;
    state.dragPointerId = 0;
    state.dragIsMouse = false;
    state.dragging = false;
    state.dragFromRecents = false;
    if (std::exchange(state.refreshAfterDrag, false)) {
        NotifyExplorerBars();
    }
}

void UpdateFrameRowCount() {
    unsigned rows = 0;
    for (const auto& bar : *g_bars) {
        if (bar.strip.get()) {
            rows = std::max(rows, bar.rowCount);
        }
    }
    rows = std::min(rows, kMaxRows);
    if (rows != g_frameRows) {
        g_frameRows = rows;
        if (!g_unloading) {
            PostFrameRelayout();
        }
    }
}

void SetBarRowCount(BarState& state, unsigned count) {
    count = std::clamp(count, 1u, kMaxRows);
    if (count == state.rowCount) {
        return;
    }
    const double height = RowAllocation(count);
    if (state.addedRow) {
        state.addedRow.Height(
            mux::GridLength{height, mux::GridUnitType::Pixel});
    }
    auto strip = state.strip.get();
    if (strip) {
        strip.Height(height);
    }
    if (auto root = state.barRoot.get()) {
        root.Height(height);
    }
    if (auto grid = state.grid.get()) {
        if (grid.MinHeight() == state.appliedGridMinHeight) {
            state.appliedGridMinHeight =
                std::max(state.oldGridMinHeight,
                         state.originalGridHeight + height);
            grid.MinHeight(state.appliedGridMinHeight);
        }
        grid.InvalidateMeasure();
    }
    if (auto nav = state.navControl.get()) {
        if (nav.MinHeight() == state.appliedNavMinHeight) {
            state.appliedNavMinHeight =
                std::max(state.oldNavMinHeight,
                         state.originalNavHeight + height);
            nav.MinHeight(state.appliedNavMinHeight);
        }
        nav.InvalidateMeasure();
    }
    if (auto host = state.hostGrid.get()) {
        host.InvalidateMeasure();
    }
    state.rowCount = count;
    if (strip) {
        UpdateFrameRowCount();
    }
}

struct ReflowGuard {
    bool& active;
    ~ReflowGuard() { active = false; }
};

double MeasuredWidth(const mux::FrameworkElement& element) {
    element.Measure(winrt::Windows::Foundation::Size{10000, kRowHeight});
    double desired = element.DesiredSize().Width;
    if (!std::isfinite(desired) || desired <= 0) {
        auto margin = element.Margin();
        desired = element.ActualWidth() + margin.Left + margin.Right;
    }
    return std::max(1.0, desired);
}

// Rows needed by the same greedy wrapping that ReflowPanel applies. Rows past
// kMaxRows are counted too, so overflow compares as worse than a full row four.
unsigned WrappedRows(const std::vector<double>& widths, double width) {
    unsigned rows = 1;
    double used = 0;
    for (double desired : widths) {
        if (used > 0 && used + desired > width + 0.5) {
            ++rows;
            used = 0;
        }
        used += desired;
    }
    return rows;
}

// Shows as many recent buttons as fit without adding bookmark rows and
// returns the width the recents column needs. The oldest (leftmost) buttons
// are hidden first. With RC, the baseline row count includes the separator and
// RC; without it, the baseline uses the full width and the separator goes when
// no button is shown.
double FitRecents(BarState& state, double width,
                  const std::vector<double>& bookmarkWidths) {
    state.visibleRecents = 0;
    auto recents = state.recents.get();
    if (!recents || recents.Children().Size() == 0) {
        return 0;
    }
    // Children: separator, recent buttons from oldest to newest, then RC.
    std::vector<mux::FrameworkElement> items;
    for (const auto& child : recents.Children()) {
        if (auto element = child.try_as<mux::FrameworkElement>()) {
            element.Visibility(mux::Visibility::Visible);
            items.push_back(element);
        }
    }
    const size_t count = state.recentButtonCount;
    const bool hasMenu = state.hasRecentMenuButton;
    if (items.size() != 1 + count + (hasMenu ? 1 : 0)) {
        return 0;
    }
    std::vector<double> widths;
    for (const auto& item : items) {
        widths.push_back(MeasuredWidth(item));
    }
    const double fixed = widths[0] + (hasMenu ? widths.back() : 0);
    auto newestWidth = [&](size_t shown) {
        double total = 0;
        for (size_t i = count + 1 - shown; i <= count; ++i) {
            total += widths[i];
        }
        return total;
    };
    size_t shown = count;
    const unsigned baseline =
        WrappedRows(bookmarkWidths, hasMenu ? width - fixed : width);
    while (shown > 0 &&
           WrappedRows(bookmarkWidths,
                       width - fixed - newestWidth(shown)) > baseline) {
        --shown;
    }
    for (size_t i = 1; i <= count; ++i) {
        items[i].Visibility(i > count - shown ? mux::Visibility::Visible
                                              : mux::Visibility::Collapsed);
    }
    const bool anyShown = hasMenu || shown > 0;
    items[0].Visibility(anyShown ? mux::Visibility::Visible
                                 : mux::Visibility::Collapsed);
    state.visibleRecents = shown;
    return anyShown ? fixed + newestWidth(shown) : 0;
}

void ReflowPanel(const muxc::StackPanel& panel, double width) try {
    auto state = FindState(panel);
    if (!state || state->reflowing || DragInProgress(*state) ||
        !std::isfinite(width) || width < 1 || width > 100000 ||
        (std::fabs(state->lastLayoutWidth - width) < 0.5 &&
         panel.Children().Size() != 0)) {
        return;
    }
    state->reflowing = true;
    ReflowGuard guard{state->reflowing};
    auto buttons = BarButtons(panel);
    if (buttons.empty()) {
        return;
    }
    std::vector<double> widths;
    widths.reserve(buttons.size());
    for (const auto& button : buttons) {
        widths.push_back(MeasuredWidth(button));
    }
    const double available =
        std::max(1.0, width - FitRecents(*state, width, widths));
    // Keep strong button refs before detaching the old rows. Reparent only
    // after all measurements succeed, so a measurement failure leaves the
    // current visual tree intact.
    for (const auto& rowElement : panel.Children()) {
        if (auto row = rowElement.try_as<muxc::StackPanel>()) {
            row.Children().Clear();
        }
    }
    panel.Children().Clear();
    muxc::StackPanel row;
    row.Orientation(muxc::Orientation::Horizontal);
    panel.Children().Append(row);
    unsigned rowCount = 1;
    double used = 0;
    for (size_t i = 0; i < buttons.size(); ++i) {
        const double desired = widths[i];
        if (used > 0 && used + desired > available + 0.5 &&
            rowCount < kMaxRows) {
            muxc::StackPanel nextRow;
            nextRow.Orientation(muxc::Orientation::Horizontal);
            nextRow.Margin(mux::Thickness{0, kInterRowGap, 0, 0});
            panel.Children().Append(nextRow);
            row = nextRow;
            ++rowCount;
            used = 0;
        }
        row.Children().Append(buttons[i]);
        used += desired;
    }
    state->lastLayoutWidth = width;
    SetBarRowCount(*state, rowCount);
} catch (...) {
    Wh_Log(L"Bookmark auto-row layout failed: %08X",
           winrt::to_hresult().value);
}

size_t BookmarkDropIndex(const muxc::StackPanel& panel,
                         winrt::Windows::Foundation::Point position) {
    auto rows = panel.Children();
    if (rows.Size() == 0) {
        return 0;
    }
    const double pitch = kRowHeight + kInterRowGap;
    unsigned targetRow = static_cast<unsigned>(std::clamp(
        static_cast<int>(position.Y / pitch), 0,
        static_cast<int>(rows.Size()) - 1));
    size_t buttonIndex = 0;
    size_t bookmarkIndex = 0;
    for (unsigned rowIndex = 0; rowIndex < rows.Size(); ++rowIndex) {
        auto row = rows.GetAt(rowIndex).try_as<muxc::StackPanel>();
        if (!row) {
            continue;
        }
        double offset = 0;
        for (const auto& child : row.Children()) {
            auto button = child.try_as<muxc::Button>();
            if (!button) {
                continue;
            }
            auto margin = button.Margin();
            double midpoint = offset + margin.Left +
                              button.ActualWidth() / 2;
            if (buttonIndex >= kFixedButtons) {
                if (rowIndex == targetRow && position.X < midpoint) {
                    return bookmarkIndex;
                }
                ++bookmarkIndex;
            }
            offset += margin.Left + button.ActualWidth() + margin.Right;
            ++buttonIndex;
        }
        if (rowIndex == targetRow) {
            return bookmarkIndex;
        }
    }
    return bookmarkIndex;
}

void ShowInsertionMark(BarState& state, const muxc::StackPanel& panel,
                       size_t insertionIndex) {
    // + and FX are fixed controls, so only bookmarks receive a drop marker.
    auto children = BarButtons(panel);
    if (children.size() <= kFixedButtons) {
        ClearDropTarget(state);
        return;
    }
    size_t bookmarkCount = children.size() - kFixedButtons;
    size_t index = std::min(insertionIndex, bookmarkCount);
    bool after = index == bookmarkCount;
    auto button = children[kFixedButtons + (after ? index - 1 : index)];
    if (!button) {
        ClearDropTarget(state);
        return;
    }
    if (state.dropTarget.get() == button && state.dropAfter == after) {
        return;
    }
    ClearDropTarget(state);
    try {
        state.dropTarget = winrt::make_weak(button);
        button.Tag(button.BorderBrush());
        state.dropOldThickness = button.BorderThickness();
        state.dropAfter = after;
        button.BorderBrush(muxm::SolidColorBrush(
            winrt::Windows::UI::Color{255, 255, 210, 60}));
        button.BorderThickness(after ? mux::Thickness{0, 0, 3, 0}
                                     : mux::Thickness{3, 0, 0, 0});
    } catch (...) {
        Wh_Log(L"Could not show bookmark drop position: %08X",
               winrt::to_hresult().value);
        ClearDropTarget(state);
    }
}

void RefreshPanel(const muxc::StackPanel& panel);

muxc::FontIcon MakeFluentIcon(const wchar_t* glyph) {
    muxc::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
    icon.FontSize(18);
    icon.VerticalAlignment(mux::VerticalAlignment::Center);
    return icon;
}

struct ScopedIcon {
    HICON value = nullptr;
    ~ScopedIcon() {
        if (value) {
            DestroyIcon(value);
        }
    }
};

// Ask Shell for this particular folder's icon. It applies desktop.ini icon
// customizations and the user's icon cache; USEFILEATTRIBUTES would skip them.
std::optional<IconPixels> FolderIconPixels(const std::wstring& path) {
    SHFILEINFOW info{};
    if (!SHGetFileInfoW(path.c_str(), 0, &info, sizeof(info),
                        SHGFI_ICON | SHGFI_LARGEICON) || !info.hIcon) {
        return std::nullopt;
    }
    ScopedIcon shellIcon{info.hIcon};
    try {
        ComScope com;
        winrt::com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(factory.put())))) {
            return std::nullopt;
        }
        winrt::com_ptr<IWICBitmap> source;
        if (FAILED(factory->CreateBitmapFromHICON(shellIcon.value,
                                                  source.put()))) {
            return std::nullopt;
        }
        UINT width = 0;
        UINT height = 0;
        if (FAILED(source->GetSize(&width, &height)) || width == 0 ||
            height == 0 || width > 256 || height > 256) {
            return std::nullopt;
        }
        winrt::com_ptr<IWICFormatConverter> converter;
        if (FAILED(factory->CreateFormatConverter(converter.put())) ||
            FAILED(converter->Initialize(source.get(),
                                         GUID_WICPixelFormat32bppBGRA,
                                         WICBitmapDitherTypeNone, nullptr, 0,
                                         WICBitmapPaletteTypeCustom))) {
            return std::nullopt;
        }
        UINT byteCount = width * height * 4;
        IconPixels result{static_cast<int>(width), static_cast<int>(height),
                          std::vector<BYTE>(byteCount)};
        if (FAILED(converter->CopyPixels(nullptr, width * 4, byteCount,
                                         result.bytes.data()))) {
            return std::nullopt;
        }
        return result;
    } catch (...) {
        Wh_Log(L"Failed to load bookmark folder icon: %08X",
               winrt::to_hresult().value);
        return std::nullopt;
    }
}

muxmi::WriteableBitmap BitmapFromPixels(const IconPixels& pixels) try {
    muxmi::WriteableBitmap bitmap(pixels.width, pixels.height);
    auto buffer = bitmap.PixelBuffer();
    if (buffer.Length() < pixels.bytes.size()) {
        return nullptr;
    }
    auto access = buffer.as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* destination = nullptr;
    if (FAILED(access->Buffer(&destination)) || !destination) {
        return nullptr;
    }
    std::memcpy(destination, pixels.bytes.data(), pixels.bytes.size());
    bitmap.Invalidate();
    return bitmap;
} catch (...) {
    Wh_Log(L"Failed to create bookmark icon bitmap: %08X",
           winrt::to_hresult().value);
    return nullptr;
}

void ForgetCachedIcon(const std::wstring& path) {
    std::erase_if(g_iconCache, [&](const auto& entry) {
        return SamePath(entry.path, path);
    });
}

muxmi::WriteableBitmap CachedFolderBitmap(const std::wstring& path) {
    ULONGLONG now = GetTickCount64();
    auto it = std::find_if(g_iconCache.begin(), g_iconCache.end(),
                           [&](const auto& entry) {
                               return SamePath(entry.path, path);
                           });
    if (it == g_iconCache.end()) {
        if (g_iconCache.size() >= kMaxCachedIcons) {
            auto oldest = std::min_element(
                g_iconCache.begin(), g_iconCache.end(),
                [](const auto& a, const auto& b) {
                    return a.usedAt < b.usedAt;
                });
            g_iconCache.erase(oldest);
        }
        g_iconCache.emplace_back();
        it = std::prev(g_iconCache.end());
        it->path = path;
    }
    it->usedAt = now;
    ULONGLONG lifetime = it->pixels ? kIconCacheLifetimeMs
                                   : kFailedIconCacheLifetimeMs;
    if (!it->attempted || now - it->loadedAt >= lifetime) {
        auto pixels = FolderIconPixels(path);
        const bool samePixels = (!pixels && !it->pixels) ||
            (pixels && it->pixels && pixels->width == it->pixels->width &&
             pixels->height == it->pixels->height && pixels->bytes == it->pixels->bytes);
        if (!samePixels) {
            it->pixels = std::move(pixels);
            it->bitmap = {};
        }
        it->attempted = true;
        it->loadedAt = now;
    }
    if (!it->pixels) {
        return nullptr;
    }
    if (auto bitmap = it->bitmap.get()) {
        return bitmap;
    }
    auto bitmap = BitmapFromPixels(*it->pixels);
    if (bitmap) {
        it->bitmap = winrt::make_weak(bitmap);
    }
    return bitmap;
}

mux::UIElement FolderIcon(const std::wstring& path, FolderStatus status) {
    if (status == FolderStatus::Missing) {
        ForgetCachedIcon(path);
        auto warning = MakeFluentIcon(L"\uE7BA");
        warning.Foreground(muxm::SolidColorBrush(
            winrt::Windows::UI::Color{255, 255, 210, 60}));
        return warning;
    }
    if (status == FolderStatus::Unknown) {
        // Avoid a potentially blocking Shell icon handler for remote or
        // inaccessible paths; keep the bookmark usable with a generic icon.
        return MakeFluentIcon(L"\uE8B7");
    }
    if (auto bitmap = CachedFolderBitmap(path)) {
        muxc::Image image;
        image.Source(bitmap);
        image.Width(18);
        image.Height(18);
        image.VerticalAlignment(mux::VerticalAlignment::Center);
        return image;
    }
    return MakeFluentIcon(L"\uE8B7");
}

void RefreshThreadBars() {
    std::vector<winrt::weak_ref<muxc::StackPanel>> panels;
    for (const auto& bar : *g_bars) {
        panels.push_back(bar.buttons);
    }
    for (const auto& weakPanel : panels) {
        if (auto panel = weakPanel.get()) {
            RefreshPanel(panel);
        }
    }
}

muxc::Button MakeLabelButton(const wchar_t* text) {
    muxc::Button button;
    muxc::TextBlock label;
    label.Text(text);
    label.FontSize(13);
    button.Content(label);
    button.Width(kButtonHeight);
    button.Height(kButtonHeight);
    button.MinWidth(0);
    button.MinHeight(0);
    button.Padding(mux::Thickness{0, 0, 0, 0});
    button.HorizontalContentAlignment(mux::HorizontalAlignment::Center);
    button.VerticalContentAlignment(mux::VerticalAlignment::Center);
    return button;
}

muxc::TextBlock MakeFolderText(const std::wstring& text) {
    muxc::TextBlock block;
    block.Text(winrt::hstring(text));
    block.VerticalAlignment(mux::VerticalAlignment::Center);
    block.FontSize(14);
    block.TextTrimming(mux::TextTrimming::CharacterEllipsis);
    return block;
}

// An empty label name means the plain folder name.
muxc::Button MakeFolderButton(const std::wstring& path, FolderStatus status,
                              const std::wstring& tooltip,
                              const FolderLabel& folderLabel = {}) {
    muxc::Button button;
    muxc::StackPanel content;
    content.Orientation(muxc::Orientation::Horizontal);
    content.VerticalAlignment(mux::VerticalAlignment::Center);
    content.Children().Append(FolderIcon(path, status));
    const std::wstring name =
        folderLabel.name.empty() ? ButtonLabel(path) : folderLabel.name;
    mux::FrameworkElement label{nullptr};
    if (folderLabel.parent.empty()) {
        label = MakeFolderText(name);
    } else {
        // The parent takes the width the name leaves and is trimmed first,
        // so the folder name stays visible: "projA\src", "pro…\src".
        muxc::Grid parts;
        muxc::ColumnDefinition parentColumn;
        parentColumn.Width({1, mux::GridUnitType::Star});
        muxc::ColumnDefinition nameColumn;
        nameColumn.Width({0, mux::GridUnitType::Auto});
        parts.ColumnDefinitions().Append(parentColumn);
        parts.ColumnDefinitions().Append(nameColumn);
        parts.HorizontalAlignment(mux::HorizontalAlignment::Left);
        auto parentText = MakeFolderText(folderLabel.parent);
        auto nameText = MakeFolderText(L"\\" + name);
        nameText.MaxWidth(110);
        muxc::Grid::SetColumn(nameText, 1);
        parts.Children().Append(parentText);
        parts.Children().Append(nameText);
        label = parts;
    }
    label.VerticalAlignment(mux::VerticalAlignment::Center);
    label.Margin(mux::Thickness{8, 0, 0, 0});
    label.MaxWidth(155);
    if (status == FolderStatus::Missing) {
        label.Opacity(0.7);
    }
    content.Children().Append(label);
    button.Content(content);
    button.Height(kButtonHeight);
    button.MaxWidth(208);
    button.Padding(mux::Thickness{12, 0, 12, 0});
    button.VerticalContentAlignment(mux::VerticalAlignment::Center);
    button.Margin(mux::Thickness{0, kButtonVerticalInset, 8,
                                 kButtonVerticalInset});
    muxc::ToolTipService::SetToolTip(
        button, winrt::box_value(winrt::hstring(tooltip)));
    return button;
}

std::wstring FolderTooltip(const std::wstring& path, FolderStatus status,
                           bool recent) {
    std::wstring tooltip = path;
    if (status == FolderStatus::Missing) {
        tooltip += L"\nFolder not found";
    }
    tooltip += L"\nCtrl+click: new tab; right-click: new window";
    tooltip += recent ? L"\nDrag onto bookmarks to keep; middle-click: remove"
                      : L"\nDrag: reorder; middle-click: remove";
    return tooltip;
}

ULONGLONG FolderIconLoadedAt(const std::wstring& path) {
    auto it = std::find_if(g_iconCache.begin(), g_iconCache.end(),
                          [&](const auto& icon) { return SamePath(icon.path, path); });
    return it == g_iconCache.end() ? 0 : it->loadedAt;
}

void RememberFolderButton(std::vector<FolderButtonState>& buttons,
                          const std::wstring& path, const muxc::Button& button,
                          FolderStatus status) {
    buttons.push_back({path, winrt::make_weak(button), status,
                       FolderIconLoadedAt(path)});
}

// Keep Button, label, event handlers and focus; replace only changed visuals.
bool RefreshFolderButtons(std::vector<FolderButtonState>& buttons, bool recent,
                          ULONGLONG now) {
    bool layoutChanged = false;
    for (auto& entry : buttons) {
        auto button = entry.button.get();
        if (!button) {
            continue;
        }
        const FolderStatus status = CheckFolderStatus(entry.path);
        bool refreshIcon = status != entry.status;
        if (status == FolderStatus::Available) {
            auto icon = std::find_if(g_iconCache.begin(), g_iconCache.end(),
                [&](const auto& item) { return SamePath(item.path, entry.path); });
            refreshIcon |= icon == g_iconCache.end() || !icon->attempted ||
                icon->loadedAt != entry.iconLoadedAt ||
                now - icon->loadedAt >= (icon->pixels ? kIconCacheLifetimeMs
                                                    : kFailedIconCacheLifetimeMs);
        }
        if (!refreshIcon) {
            continue;
        }
        auto content = button.Content().as<muxc::StackPanel>();
        auto newIcon = FolderIcon(entry.path, status);
        auto oldImage = content.Children().GetAt(0).try_as<muxc::Image>();
        auto newImage = newIcon.try_as<muxc::Image>();
        auto oldGlyph = content.Children().GetAt(0).try_as<muxc::FontIcon>();
        auto newGlyph = newIcon.try_as<muxc::FontIcon>();
        const bool sameVisual = (oldImage && newImage && oldImage.Source() == newImage.Source()) ||
                                (oldGlyph && newGlyph && oldGlyph.Glyph() == newGlyph.Glyph());
        if (!sameVisual) {
            content.Children().SetAt(0, newIcon);
            layoutChanged |= static_cast<bool>(oldImage) != static_cast<bool>(newImage);
        }
        entry.iconLoadedAt = FolderIconLoadedAt(entry.path);
        if (status != entry.status) {
            // The label text stays; it may be a TextBlock or a parent/name Grid.
            auto label = content.Children().GetAt(1).as<mux::UIElement>();
            label.Opacity(status == FolderStatus::Missing ? 0.7 : 1.0);
            muxc::ToolTipService::SetToolTip(
                button, winrt::box_value(winrt::hstring(
                            FolderTooltip(entry.path, status, recent))));
            layoutChanged = true;
        }
        entry.status = status;
    }
    return layoutChanged;
}

// Drops land only on the bookmark strip, not on the recents beside it.
bool PointerInsideStrip(const BarState& state,
                        const muxi::PointerRoutedEventArgs& args) {
    auto strip = state.strip.get();
    if (!strip) {
        return false;
    }
    auto position = args.GetCurrentPoint(strip).Position();
    return position.X >= 0 && position.Y >= 0 &&
           position.X <= strip.ActualWidth() &&
           position.Y <= strip.ActualHeight();
}

// Click opens the folder, right-click opens it in a new window, middle-click
// removes the bookmark or recent entry, and a left drag reorders a bookmark
// or turns a recent folder into one.
void AttachFolderButtonHandlers(BarState& state, const muxc::Button& button,
                                const std::wstring& path,
                                const winrt::weak_ref<muxc::StackPanel>& weakPanel,
                                bool recent) {
    auto& handlers = recent ? state.recentHandlers : state.panelHandlers;
    // Buttons consume left presses. AddHandler receives them after the
    // class handler and keeps ordinary Click behavior for a short press.
    TrackPointer(handlers, button,
        mux::UIElement::PointerPressedEvent(),
        muxi::PointerEventHandler{
            [path, weakPanel, recent](
                const winrt::Windows::Foundation::IInspectable& sender,
                const muxi::PointerRoutedEventArgs& args) {
                if (g_unloading) {
                    return;
                }
                auto panel = weakPanel.get();
                auto state = panel ? FindState(panel) : nullptr;
                auto element = sender.try_as<muxc::Button>();
                if (!state || !element) {
                    return;
                }
                auto point = args.GetCurrentPoint(panel);
                if (point.Properties().IsMiddleButtonPressed()) {
                    args.Handled(true);
                    ClearDrag(*state);
                    if (recent) {
                        RemoveRecent(path);
                        RefreshPanel(panel);
                    } else if (RemoveBookmark(path)) {
                        RefreshPanel(panel);
                    }
                } else if (point.Properties().IsLeftButtonPressed()) {
                    ClearDrag(*state);
                    state->suppressClick.clear();
                    state->dragPath = path;
                    state->dragFromRecents = recent;
                    state->dragButton = winrt::make_weak(element);
                    state->dragOldOpacity = element.Opacity();
                    state->dragPointerId = point.PointerId();
                    state->dragIsMouse =
                        args.Pointer().PointerDeviceType() ==
                        winrt::Microsoft::UI::Input::PointerDeviceType::Mouse;
                    state->dragStart = point.Position();
                    state->dragging = false;
                    element.CapturePointer(args.Pointer());
                }
            }});
    TrackPointer(handlers, button,
        mux::UIElement::PointerMovedEvent(),
        muxi::PointerEventHandler{
            [weakPanel](const winrt::Windows::Foundation::IInspectable& sender,
                        const muxi::PointerRoutedEventArgs& args) {
                auto panel = weakPanel.get();
                auto state = panel ? FindState(panel) : nullptr;
                auto element = sender.try_as<muxc::Button>();
                if (!state || !element || state->dragButton.get() != element) {
                    return;
                }
                auto point = args.GetCurrentPoint(panel);
                if (point.PointerId() != state->dragPointerId ||
                    !point.Properties().IsLeftButtonPressed()) {
                    return;
                }
                auto position = point.Position();
                float dx = position.X - state->dragStart.X;
                float dy = position.Y - state->dragStart.Y;
                if (dx * dx + dy * dy >= kDragThreshold * kDragThreshold) {
                    if (!state->dragging) {
                        element.Opacity(0.55);
                    }
                    state->dragging = true;
                    state->suppressClick = state->dragPath;
                    if (PointerInsideStrip(*state, args)) {
                        ShowInsertionMark(*state, panel,
                                          BookmarkDropIndex(panel, position));
                    } else {
                        ClearDropTarget(*state);
                    }
                    args.Handled(true);
                }
            }});
    TrackPointer(handlers, button,
        mux::UIElement::PointerReleasedEvent(),
        muxi::PointerEventHandler{
            [weakPanel](const winrt::Windows::Foundation::IInspectable& sender,
                        const muxi::PointerRoutedEventArgs& args) {
                auto panel = weakPanel.get();
                auto state = panel ? FindState(panel) : nullptr;
                auto element = sender.try_as<muxc::Button>();
                if (!state || !element || state->dragButton.get() != element) {
                    return;
                }
                auto point = args.GetCurrentPoint(panel);
                if (point.PointerId() != state->dragPointerId) {
                    return;
                }
                auto position = point.Position();
                bool dragging = state->dragging;
                bool fromRecents = state->dragFromRecents;
                auto source = state->dragPath;
                bool inside = PointerInsideStrip(*state, args);
                ClearDrag(*state);
                if (!dragging) {
                    return;
                }
                args.Handled(true);
                if (!inside) {
                    return;
                }
                size_t index = BookmarkDropIndex(panel, position);
                BookmarkChange change;
                bool changed;
                if (fromRecents) {
                    change = InsertBookmarkAt(source, index);
                    changed = change.changed;
                } else {
                    changed = MoveBookmarkToIndex(source, index);
                }
                if (changed) {
                    RefreshPanel(panel);
                }
                ShowBookmarkNotice(MakeNotice(IslandWindow(panel), change));
            }});
    // Touch and pen contact can be canceled without a release; mouse
    // presses are covered by DragInProgress.
    TrackPointer(handlers, button,
        mux::UIElement::PointerCanceledEvent(),
        muxi::PointerEventHandler{
            [weakPanel](const winrt::Windows::Foundation::IInspectable& sender,
                        const muxi::PointerRoutedEventArgs&) {
                auto panel = weakPanel.get();
                auto state = panel ? FindState(panel) : nullptr;
                auto element = sender.try_as<muxc::Button>();
                if (!state || !element || state->dragButton.get() != element) {
                    return;
                }
                ClearDrag(*state);
            }});
    TrackClick(handlers, button, [path, weakPanel](
                     const winrt::Windows::Foundation::IInspectable& sender,
                     const mux::RoutedEventArgs&) {
        if (g_unloading) {
            return;
        }
        if (auto panel = weakPanel.get()) {
            if (auto state = FindState(panel);
                state && SamePath(state->suppressClick, path)) {
                state->suppressClick.clear();
                return;
            }
        }
        OpenFolderFrom(sender.try_as<mux::FrameworkElement>(), path);
    });
    // Also raised by the context menu key and by press-and-hold on touch.
    TrackContextRequested(handlers, button, [path](
                              const mux::UIElement& sender,
                              const muxi::ContextRequestedEventArgs& args) {
        args.Handled(true);
        if (g_unloading) {
            return;
        }
        auto element = sender.try_as<mux::FrameworkElement>();
        if (HWND window = element ? ExplorerWindowForElement(element) : nullptr) {
            OpenFolderInNewWindow(window, path);
        }
    });
}

void ClearRecentsFromBar(const winrt::weak_ref<muxc::StackPanel>& weakPanel) {
    if (g_unloading) {
        return;
    }
    auto panel = weakPanel.get();
    if (!panel || !FindState(panel)) {
        return;
    }
    ClearRecents();
    RefreshThreadBars();
}

void AppendDisabledMenuItem(const muxc::MenuFlyout& menu, const wchar_t* text,
                            const wchar_t* tooltip = nullptr) {
    muxc::MenuFlyoutItem item;
    item.Text(text);
    item.IsEnabled(false);
    if (tooltip) {
        muxc::ToolTipService::SetToolTip(item, winrt::box_value(tooltip));
    }
    menu.Items().Append(item);
}

// Rebuilt on each opening, so it matches the buttons the last reflow showed.
void FillRecentMenu(BarState& state, const muxc::MenuFlyout& menu,
                    const winrt::weak_ref<muxc::Button>& weakAnchor) {
    RevokeHandlers(state.recentMenuHandlers);
    menu.Items().Clear();
    if (state.visibleRecents >= state.recentFolders.size()) {
        AppendDisabledMenuItem(menu, state.recentFolders.empty()
                                         ? L"No recent folders"
                                         : L"No other recent folders");
    } else {
        // Same rule as the buttons, over the bookmarks and the whole list.
        std::vector<std::wstring> paths;
        for (const auto& bookmark : state.folderButtons) {
            paths.push_back(bookmark.path);
        }
        const size_t firstRecent = paths.size();
        paths.insert(paths.end(), state.recentFolders.begin(),
                     state.recentFolders.end());
        const auto labels = FolderLabels(paths, g_parentForDuplicates);
        for (size_t i = state.visibleRecents; i < state.recentFolders.size();
             ++i) {
            const auto& path = state.recentFolders[i];
            muxc::MenuFlyoutItem item;
            item.Text(winrt::hstring(LabelText(labels[firstRecent + i])));
            muxc::ToolTipService::SetToolTip(
                item, winrt::box_value(winrt::hstring(path)));
            TrackClick(state.recentMenuHandlers, item, [path, weakAnchor](
                           const winrt::Windows::Foundation::IInspectable&,
                           const mux::RoutedEventArgs&) {
                if (!g_unloading) {
                    OpenFolderFrom(weakAnchor.get(), path);
                }
            });
            menu.Items().Append(item);
        }
    }
}

void AppendRecents(BarState& state, const muxc::StackPanel& recents,
                   const winrt::weak_ref<muxc::StackPanel>& weakPanel,
                   const std::vector<FolderLabel>& labels = {}) {
    muxc::Border separator;
    separator.Width(1);
    separator.Height(20);
    separator.Margin(mux::Thickness{4, 0, 12, 0});
    separator.VerticalAlignment(mux::VerticalAlignment::Center);
    separator.Background(muxm::SolidColorBrush(
        winrt::Windows::UI::Color{160, 128, 128, 128}));
    recents.Children().Append(separator);

    state.recentButtonCount =
        std::min(state.recentFolders.size(), g_recentSettings.buttons);
    // The newest folder sits next to the right edge.
    for (size_t i = state.recentButtonCount; i-- > 0;) {
        const auto& path = state.recentFolders[i];
        const FolderStatus status = CheckFolderStatus(path);
        auto button = MakeFolderButton(path, status, FolderTooltip(path, status, true),
                                       i < labels.size() ? labels[i] : FolderLabel{});
        RememberFolderButton(state.recentFolderButtons, path, button, status);
        AttachFolderButtonHandlers(state, button, path, weakPanel, true);
        recents.Children().Append(button);
    }

    // RC stays available while no recent folder has a button, so the feature
    // remains visible and its list can still be cleared.
    state.hasRecentMenuButton =
        g_recentSettings.menuButton || state.recentButtonCount == 0;
    if (!state.hasRecentMenuButton) {
        return;
    }
    auto rcButton = MakeLabelButton(L"RC");
    rcButton.Margin(mux::Thickness{0, kButtonVerticalInset, 8,
                                   kButtonVerticalInset});
    muxc::ToolTipService::SetToolTip(
        rcButton,
        winrt::box_value(g_recentSettings.rightClickClears
                             ? L"More recent folders; right-click clears "
                               L"immediately"
                             : L"More recent folders; right-click to clear"));
    auto weakRc = winrt::make_weak(rcButton);
    muxc::MenuFlyout recentMenu;
    TrackOpening(state.recentHandlers, recentMenu, [weakPanel, weakRc](
                     const winrt::Windows::Foundation::IInspectable& sender,
                     const winrt::Windows::Foundation::IInspectable&) {
        auto panel = weakPanel.get();
        auto state = panel ? FindState(panel) : nullptr;
        auto menu = sender.try_as<muxc::MenuFlyout>();
        if (!g_unloading && state && menu) {
            FillRecentMenu(*state, menu, weakRc);
        }
    });
    // A flyout without items does not open, so Opening would never run.
    AppendDisabledMenuItem(recentMenu, L"No recent folders");
    rcButton.Flyout(recentMenu);
    if (g_recentSettings.rightClickClears) {
        TrackContextRequested(
            state.recentHandlers, rcButton,
            [weakPanel](const mux::UIElement&,
                        const muxi::ContextRequestedEventArgs& args) {
                args.Handled(true);
                ClearRecentsFromBar(weakPanel);
            });
    } else {
        muxc::MenuFlyout clearMenu;
        muxc::MenuFlyoutItem clearItem;
        clearItem.Text(L"Clear recent folders");
        TrackClick(state.recentHandlers, clearItem, [weakPanel](
                       const winrt::Windows::Foundation::IInspectable&,
                       const mux::RoutedEventArgs&) {
            ClearRecentsFromBar(weakPanel);
        });
        clearMenu.Items().Append(clearItem);
        rcButton.ContextFlyout(clearMenu);
    }
    recents.Children().Append(rcButton);
}

void RefreshRecentPanel(BarState& state, RecentView view,
                        const winrt::weak_ref<muxc::StackPanel>& weakPanel) {
    RevokeHandlers(state.recentMenuHandlers);
    RevokeHandlers(state.recentHandlers);
    state.recentFolderButtons.clear();
    state.recentFolders = std::move(view.folders);
    state.recentButtonCount = 0;
    state.hasRecentMenuButton = false;
    state.visibleRecents = 0;
    if (auto recents = state.recents.get()) {
        recents.Children().Clear();
        if (g_recentSettings.enabled) {
            AppendRecents(state, recents, weakPanel, view.labels);
        }
    }
}

void RefreshPanel(const muxc::StackPanel& panel) try {
    UiCallbackScope uiScope;
    if (g_unloading || g_threadClosing) {
        return;
    }
    auto state = FindState(panel);
    if (!state) {
        return;
    }
    UpdateThreadSettings();
    // Do not read storage or change controls while a bookmark is being dragged.
    if (DragInProgress(*state) && panel.Children().Size() != 0) {
        state->refreshAfterDrag = true;
        return;
    }
    std::wstring storage;
    {
        std::lock_guard lock(g_storageMutex);
        storage = ReadStorageLocked();
    }
    const auto bookmarks = SplitBookmarks(storage);
    RecentView recentView;
    std::wstring recentKey;
    if (g_recentSettings.enabled) {
        recentView = LoadRecentView(bookmarks);
        for (const auto& path : recentView.folders) {
            recentKey += L'\x1f';
            recentKey += path;
        }
    }
    // Labels compare the bookmarks with the recent buttons, so a recent-only
    // change can relabel a bookmark; that needs a full rebuild.
    std::vector<std::wstring> barPaths = bookmarks;
    barPaths.insert(barPaths.end(), recentView.folders.begin(),
                    recentView.folders.begin() +
                        std::min(recentView.folders.size(), g_recentSettings.buttons));
    auto labels = FolderLabels(barPaths, g_parentForDuplicates);
    std::wstring labelKey;
    for (size_t i = 0; i < bookmarks.size(); ++i) {
        labelKey += L'\x1f';
        labelKey += labels[i].parent;
    }
    recentView.labels.assign(labels.begin() + bookmarks.size(), labels.end());
    ULONGLONG now = GetTickCount64();
    const bool fullRebuild = !state->renderComplete ||
        panel.Children().Size() == 0 || storage != state->renderedKey ||
        labelKey != state->renderedLabelKey ||
        state->renderedSettingsGeneration != g_threadSettingsGeneration;
    const bool recentChanged = recentKey != state->renderedRecentKey;
    const bool checkFolders = state->lastFolderCheckAt == 0 ||
        now - state->lastFolderCheckAt >= kFolderCheckIntervalMs;
    if (!fullRebuild) {
        bool layoutChanged = false;
        if (checkFolders) {
            layoutChanged = RefreshFolderButtons(state->folderButtons, false, now);
            if (!recentChanged) {
                layoutChanged |= RefreshFolderButtons(state->recentFolderButtons, true, now);
            }
            state->lastFolderCheckAt = now;
        }
        if (recentChanged) {
            RefreshRecentPanel(*state, std::move(recentView), winrt::make_weak(panel));
            state->renderedRecentKey = std::move(recentKey);
        }
        if (layoutChanged || recentChanged) {
            state->lastLayoutWidth = 0;
            if (auto root = state->barRoot.get()) {
                ReflowPanel(panel, root.ActualWidth());
            }
        }
        if (checkFolders || recentChanged) {
            Wh_Log(L"Panel refreshed in place: folder check=%d, recents changed=%d",
                   checkFolders ? 1 : 0, recentChanged ? 1 : 0);
        }
        return;
    }
    state->renderComplete = false;
    ClearDrag(*state);
    RevokeHandlers(state->recentMenuHandlers);
    RevokeHandlers(state->recentHandlers);
    RevokeHandlers(state->fxMenuHandlers);
    RevokeHandlers(state->driveHandlers);
    RevokeHandlers(state->panelHandlers);
    state->folderButtons.clear();
    state->recentFolderButtons.clear();
    panel.Children().Clear();
    muxc::StackPanel firstRow;
    firstRow.Orientation(muxc::Orientation::Horizontal);
    panel.Children().Append(firstRow);
    auto weakPanel = winrt::make_weak(panel);

    muxc::Button addButton;
    auto addIcon = MakeFluentIcon(L"\uE710");
    addIcon.FontSize(16);
    addButton.Content(addIcon);
    addButton.Width(kButtonHeight);
    addButton.Height(kButtonHeight);
    addButton.MinWidth(0);
    addButton.MinHeight(0);
    addButton.Padding(mux::Thickness{0, 0, 0, 0});
    addButton.HorizontalContentAlignment(mux::HorizontalAlignment::Center);
    addButton.VerticalContentAlignment(mux::VerticalAlignment::Center);
    addButton.Margin(mux::Thickness{8, kButtonVerticalInset, 8,
                                    kButtonVerticalInset});
    muxc::ToolTipService::SetToolTip(
        addButton, winrt::box_value(g_bookmarkHotkey
                                        ? L"Bookmark this folder (Ctrl+B)"
                                        : L"Bookmark this folder"));
    // ContextFlyout opens on right-click; ordinary left-click still adds the
    // current folder.
    std::wstring backupPath = SuggestedBackupPath();
    muxc::MenuFlyout backupMenu;
    muxc::MenuFlyoutItem saveItem;
    saveItem.Text(L"Save bookmarks\u2026");
    TrackClick(state->panelHandlers, saveItem, [backupPath, weakPanel](
                       const winrt::Windows::Foundation::IInspectable& sender,
                       const mux::RoutedEventArgs&) {
        OperationScope operation;
        if (!operation.active) {
            return;
        }
        auto panel = weakPanel.get();
        HWND window = panel ? ExplorerWindowForElement(panel) : nullptr;
        auto selectedPath = ChooseBackupPath(window, true, backupPath);
        if (selectedPath.empty() || g_unloading) {
            return;
        }
        bool ok = SaveBackup(selectedPath);
        if (ok) {
            Wh_Log(L"Bookmarks saved to %ls", selectedPath.c_str());
        } else {
            Wh_Log(L"Could not save bookmarks to %ls", selectedPath.c_str());
        }
        if (auto item = sender.try_as<muxc::MenuFlyoutItem>()) {
            muxc::ToolTipService::SetToolTip(
                item, winrt::box_value(winrt::hstring(
                          std::wstring(ok ? L"Saved to " : L"Save failed: ") +
                          selectedPath)));
        }
    });
    backupMenu.Items().Append(saveItem);
    muxc::MenuFlyoutItem loadItem;
    loadItem.Text(L"Load bookmarks\u2026");
    TrackClick(state->panelHandlers, loadItem, [backupPath, weakPanel](
                       const winrt::Windows::Foundation::IInspectable& sender,
                       const mux::RoutedEventArgs&) {
        OperationScope operation;
        if (!operation.active) {
            return;
        }
        auto panel = weakPanel.get();
        HWND window = panel ? ExplorerWindowForElement(panel) : nullptr;
        auto selectedPath = ChooseBackupPath(window, false, backupPath);
        if (selectedPath.empty() || g_unloading) {
            return;
        }
        bool ok = LoadBackup(selectedPath);
        if (ok) {
            Wh_Log(L"Bookmarks loaded from %ls", selectedPath.c_str());
        } else {
            Wh_Log(L"Could not load bookmarks from %ls",
                   selectedPath.c_str());
        }
        if (auto item = sender.try_as<muxc::MenuFlyoutItem>()) {
            muxc::ToolTipService::SetToolTip(
                item, winrt::box_value(winrt::hstring(
                          std::wstring(ok ? L"Loaded from " : L"Load failed: ") +
                          selectedPath)));
        }
        if (ok) {
            if (auto livePanel = weakPanel.get()) {
                RefreshPanel(livePanel);
            }
        }
    });
    backupMenu.Items().Append(loadItem);
    addButton.ContextFlyout(backupMenu);
    TrackClick(state->panelHandlers, addButton, [weakPanel](
                        const winrt::Windows::Foundation::IInspectable& sender,
                        const mux::RoutedEventArgs&) {
        if (g_unloading) {
            return;
        }
        auto button = sender.try_as<mux::FrameworkElement>();
        HWND window = button ? ExplorerWindowForElement(button) : nullptr;
        if (!window) {
            return;
        }
        const auto change = AddBookmark(CurrentFolder(window));
        if (change.changed) {
            if (auto panel = weakPanel.get()) {
                RefreshPanel(panel);
            }
        }
        ShowBookmarkNotice(MakeNotice(window, change));
    });
    firstRow.Children().Append(addButton);

    auto fxButton = MakeLabelButton(L"FX");
    fxButton.Margin(mux::Thickness{0, kButtonVerticalInset, 16,
                                   kButtonVerticalInset});
    muxc::ToolTipService::SetToolTip(
        fxButton, winrt::box_value(
            L"Folders; right-click for drives"));
    auto weakFxButton = winrt::make_weak(fxButton);
    muxc::MenuFlyout foldersMenu;
    muxc::MenuFlyout drivesMenu;
    auto appendLocation = [weakFxButton, state](muxc::MenuFlyout& menu,
                              const std::wstring& label,
                              const std::wstring& path,
                              bool driveItem = false) {
        if (path.empty()) {
            return;
        }
        muxc::MenuFlyoutItem item;
        item.Text(label);
        muxc::ToolTipService::SetToolTip(
            item, winrt::box_value(winrt::hstring(path)));
        TrackClick(driveItem ? state->driveHandlers : state->fxMenuHandlers,
                   item, [path, weakFxButton](
                       const winrt::Windows::Foundation::IInspectable&,
                       const mux::RoutedEventArgs&) {
            if (!g_unloading) {
                OpenFolderFrom(weakFxButton.get(), path);
            }
        });
        menu.Items().Append(item);
    };
    // Build FX on open, so restored local folders reappear without rebuilding
    // bookmark buttons on hover. Settings are the UI thread's current snapshot.
    TrackOpening(state->panelHandlers, foldersMenu, [appendLocation, state](
        const winrt::Windows::Foundation::IInspectable& sender,
        const winrt::Windows::Foundation::IInspectable&) {
        if (g_unloading) {
            return;
        }
        UpdateThreadSettings();
        auto menu = sender.as<muxc::MenuFlyout>();
        RevokeHandlers(state->fxMenuHandlers);
        menu.Items().Clear();
        auto appendStandard = [&](const wchar_t* label, REFKNOWNFOLDERID id) {
            auto path = KnownFolderPath(id);
            if (!path.empty() && CheckFolderStatus(path) == FolderStatus::Missing) {
                Wh_Log(L"FX: %ls hidden, missing from a local disk: %ls", label,
                       path.c_str());
                return;
            }
            appendLocation(menu, label, path);
        };
        appendStandard(L"~", FOLDERID_Profile);
        appendStandard(L"Desktop", FOLDERID_Desktop);
        appendStandard(L"Documents", FOLDERID_Documents);
        appendStandard(L"Downloads", FOLDERID_Downloads);
        bool separatorAdded = false;
        for (const auto& folder : g_fxFolders) {
            if (CheckFolderStatus(folder.path) == FolderStatus::Missing) {
                continue;
            }
            if (!separatorAdded) {
                menu.Items().Append(muxc::MenuFlyoutSeparator{});
                separatorAdded = true;
            }
            appendLocation(menu, folder.label, folder.path);
        }
        if (menu.Items().Size() == 0) {
            AppendDisabledMenuItem(menu, L"No folders available");
        }
    });
    AppendDisabledMenuItem(foldersMenu, L"No folders available");
    // Build the list when FX opens so newly attached drives appear immediately.
    // Only local fixed drives are probed; other drives are listed unchecked.
    TrackOpening(state->panelHandlers, drivesMenu, [appendLocation, state](
                           const winrt::Windows::Foundation::IInspectable& sender,
                           const winrt::Windows::Foundation::IInspectable&) {
        if (g_unloading) {
            return;
        }
        auto menu = sender.try_as<muxc::MenuFlyout>();
        if (!menu) {
            return;
        }
        RevokeHandlers(state->driveHandlers);
        menu.Items().Clear();
        DWORD driveMask = GetLogicalDrives();
        for (unsigned index = 0; index < 26; ++index) {
            if (!(driveMask & (DWORD{1} << index))) {
                continue;
            }
            wchar_t drive[] = {static_cast<wchar_t>(L'A' + index), L':', L'\\', L'\0'};
            if (CheckFolderStatus(drive) == FolderStatus::Missing) {
                continue;
            }
            appendLocation(menu, DriveMenuLabel(drive), drive, true);
        }
        if (menu.Items().Size() == 0) {
            AppendDisabledMenuItem(menu, L"No drives available");
        }
    });
    AppendDisabledMenuItem(drivesMenu, L"No drives available");
    fxButton.Flyout(foldersMenu);
    fxButton.ContextFlyout(drivesMenu);
    firstRow.Children().Append(fxButton);

    size_t withParent = 0;
    for (size_t i = 0; i < bookmarks.size(); ++i) {
        const auto& path = bookmarks[i];
        FolderStatus status = CheckFolderStatus(path);
        withParent += labels[i].parent.empty() ? 0 : 1;
        auto button = MakeFolderButton(path, status, FolderTooltip(path, status, false),
                                       labels[i]);
        RememberFolderButton(state->folderButtons, path, button, status);
        AttachFolderButtonHandlers(*state, button, path, weakPanel, false);
        firstRow.Children().Append(button);
    }

    RefreshRecentPanel(*state, std::move(recentView), weakPanel);

    state->renderedKey = std::move(storage);
    state->renderedRecentKey = std::move(recentKey);
    state->renderedLabelKey = std::move(labelKey);
    state->renderedSettingsGeneration = g_threadSettingsGeneration;
    state->renderComplete = true;
    Wh_Log(L"Panel rebuilt: %zu bookmark(s), %zu with parent, settings generation %llu",
           bookmarks.size(), withParent, state->renderedSettingsGeneration);
    state->lastFolderCheckAt = now;
    state->lastLayoutWidth = 0;
    if (auto root = state->barRoot.get()) {
        ReflowPanel(panel, root.ActualWidth());
    }
} catch (...) {
    Wh_Log(L"Bookmarks bar refresh failed: %08X", winrt::to_hresult().value);
    // A half-built panel is rebuilt on the next hover instead of being kept
    // until the folder check interval passes. The recent buttons of the
    // previous render may have lost their handlers, so remove them.
    if (auto state = FindState(panel)) {
        state->renderComplete = false;
        state->lastFolderCheckAt = 0;
        state->recentFolders.clear();
        state->recentButtonCount = 0;
        state->hasRecentMenuButton = false;
        state->visibleRecents = 0;
        try {
            if (auto recents = state->recents.get()) {
                recents.Children().Clear();
            }
        } catch (...) {
            Wh_Log(L"Could not clear stale recent folders: %08X",
                   winrt::to_hresult().value);
        }
    }
}

// File Explorer history: each Explorer UI thread listens to the navigation
// events of its own tabs. ShellWindows events report opened and closed tabs.
void OnShellEvent(DISPID id, DISPPARAMS* params);

class ShellEventSink final : public IDispatch {
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                             void** object) override {
        if (!object) {
            return E_POINTER;
        }
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IDispatch) ||
            IsEqualIID(iid, kWebBrowserEvents2) ||
            IsEqualIID(iid, kShellWindowsEvents)) {
            *object = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refs; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refs = --m_refs;
        if (refs == 0) {
            delete this;
        }
        return refs;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (!count) {
            return E_POINTER;
        }
        *count = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID,
                                            DISPID*) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID id, REFIID, LCID, WORD,
                                     DISPPARAMS* params, VARIANT*,
                                     EXCEPINFO*, UINT*) override {
        if (!g_unloading) {
            OnShellEvent(id, params);
        }
        return S_OK;
    }

private:
    std::atomic<ULONG> m_refs{1};
};

struct BrowserConnection {
    winrt::com_ptr<IUnknown> identity;
    winrt::com_ptr<IConnectionPoint> point;
    DWORD cookie = 0;
};

struct RecentTracker {
    ShellEventSink* sink = nullptr;
    winrt::com_ptr<IShellWindows> shellWindows;
    winrt::com_ptr<IConnectionPoint> windowsPoint;
    DWORD windowsCookie = 0;
    std::vector<BrowserConnection> browsers;
};

// Deleted only by StopRecentTracking. A thread that ends without cleanup
// leaks its tracker instead of releasing COM objects after COM shut down.
thread_local RecentTracker* g_recentTracker = nullptr;

// Work deferred to a thread timer on this Explorer UI thread.
thread_local UINT_PTR g_workTimer = 0;
thread_local bool g_recentSyncPending = false;
thread_local bool g_refreshPending = false;
thread_local HWND g_bookmarkTogglePending = nullptr;
thread_local bool g_bookmarkToggleFromFileList = false;
thread_local bool g_threadWorking = false;

bool ConnectEvents(IUnknown* source, REFIID events, IDispatch* sink,
                   winrt::com_ptr<IConnectionPoint>& point, DWORD& cookie) {
    winrt::com_ptr<IConnectionPointContainer> container;
    if (FAILED(source->QueryInterface(IID_PPV_ARGS(container.put()))) ||
        FAILED(container->FindConnectionPoint(events, point.put())) ||
        FAILED(point->Advise(sink, &cookie))) {
        point = nullptr;
        return false;
    }
    return true;
}

bool EnsureRecentTracker() {
    if (g_recentTracker) {
        return true;
    }
    auto tracker = std::make_unique<RecentTracker>();
    if (FAILED(CoCreateInstance(kShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(tracker->shellWindows.put())))) {
        return false;
    }
    tracker->sink = new ShellEventSink();
    if (!ConnectEvents(tracker->shellWindows.get(), kShellWindowsEvents,
                       tracker->sink, tracker->windowsPoint,
                       tracker->windowsCookie)) {
        Wh_Log(L"Could not listen for File Explorer windows");
        tracker->sink->Release();
        return false;
    }
    g_recentTracker = tracker.release();
    return true;
}

void StopRecentTracking() {
    RecentTracker* tracker = std::exchange(g_recentTracker, nullptr);
    if (!tracker) {
        return;
    }
    for (auto& browser : tracker->browsers) {
        browser.point->Unadvise(browser.cookie);
    }
    if (tracker->windowsPoint) {
        tracker->windowsPoint->Unadvise(tracker->windowsCookie);
    }
    // Release references that COM stubs hold for other apartments, so no
    // event can reach the sink after the mod is unloaded.
    CoDisconnectObject(tracker->sink, 0);
    tracker->sink->Release();
    delete tracker;
}

// Connects to the tabs whose frame belongs to this thread and forgets the
// tabs that ShellWindows no longer lists.
void SyncBrowserConnections(RecentTracker& tracker) {
    long count = 0;
    if (FAILED(tracker.shellWindows->get_Count(&count))) {
        return;
    }
    const DWORD thread = GetCurrentThreadId();
    std::vector<winrt::com_ptr<IUnknown>> live;
    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(tracker.shellWindows->Item(index, dispatch.put())) ||
            !dispatch) {
            continue;
        }
        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        SHANDLE_PTR windowValue = 0;
        if (!webBrowser || FAILED(webBrowser->get_HWND(&windowValue)) ||
            GetWindowThreadProcessId(reinterpret_cast<HWND>(windowValue),
                                     nullptr) != thread) {
            continue;
        }
        auto identity = dispatch.try_as<IUnknown>();
        if (!identity) {
            continue;
        }
        live.push_back(identity);
        if (std::any_of(tracker.browsers.begin(), tracker.browsers.end(),
                        [&](const auto& browser) {
                            return browser.identity == identity;
                        })) {
            continue;
        }
        BrowserConnection connection;
        connection.identity = identity;
        if (ConnectEvents(dispatch.get(), kWebBrowserEvents2, tracker.sink,
                          connection.point, connection.cookie)) {
            tracker.browsers.push_back(std::move(connection));
        }
    }
    std::erase_if(tracker.browsers, [&](auto& browser) {
        if (std::any_of(live.begin(), live.end(), [&](const auto& item) {
                return item == browser.identity;
            })) {
            return false;
        }
        browser.point->Unadvise(browser.cookie);
        return true;
    });
}

void SyncRecentTracking() {
    if (!g_recentSettings.enabled) {
        StopRecentTracking();
        return;
    }
    ComScope com;
    if (!EnsureRecentTracker()) {
        return;
    }
    SyncBrowserConnections(*g_recentTracker);
    Wh_Log(L"Recent tracking: %d tab(s) connected on this thread",
           static_cast<int>(g_recentTracker->browsers.size()));
    const bool hasPanel = std::any_of(g_bars->begin(), g_bars->end(),
                                    [](const auto& bar) { return !!bar.strip.get(); });
    if (g_recentTracker->browsers.empty() && !hasPanel) {
        StopRecentTracking();
    }
}

// With folders selected in the file list, Ctrl+B toggles those; otherwise it
// toggles the active tab's folder. Virtual locations have no path.
BookmarkChange ToggleBookmarksForHotkey(HWND window, bool fileListFocused) {
    std::vector<std::wstring> paths;
    if (fileListFocused) {
        paths = SelectedFolders(window);
    }
    if (paths.empty()) {
        if (auto path = CurrentFolder(window); !path.empty()) {
            paths.push_back(std::move(path));
        }
    }
    const auto change = ToggleBookmarks(paths);
    Wh_Log(L"Ctrl+B: %d folder(s), file list focused=%d, bookmarks changed=%d, "
           L"added=%d, no room=%d, already bookmarked=%d",
           static_cast<int>(paths.size()), fileListFocused ? 1 : 0,
           change.changed ? 1 : 0, static_cast<int>(change.added),
           static_cast<int>(change.noRoom), static_cast<int>(change.existing));
    return change;
}

void RunThreadWork() {
    if (g_threadWorking) {
        return;
    }
    OperationScope operation;
    if (!operation.active) {
        return;
    }
    g_threadWorking = true;
    try {
        UpdateThreadSettings();
        if (std::exchange(g_settingsApplyPending, false)) {
            g_recentSyncPending = true;
            g_refreshPending = true;
            Wh_Log(L"UI thread applying settings generation %llu",
                   g_threadSettingsGeneration);
        }
        while (!g_unloading && !g_threadClosing && (g_recentSyncPending || g_refreshPending ||
                                g_bookmarkTogglePending)) {
            if (HWND window = std::exchange(g_bookmarkTogglePending, nullptr)) {
                const auto change = ToggleBookmarksForHotkey(
                    window, g_bookmarkToggleFromFileList);
                if (change.changed) {
                    g_refreshPending = true;
                }
                if (change.Notice() != BookmarkNotice::None) {
                    g_pendingNotice = MakeNotice(window, change);
                }
            }
            if (std::exchange(g_recentSyncPending, false)) {
                SyncRecentTracking();
            }
            if (std::exchange(g_refreshPending, false)) {
                RefreshThreadBars();
            }
            if (g_pendingNotice.kind != BookmarkNotice::None) {
                ShowBookmarkNotice(std::exchange(g_pendingNotice, {}));
            }
        }
    } catch (...) {
        Wh_Log(L"Bookmarks bar update failed: %08X",
               winrt::to_hresult().value);
    }
    g_threadWorking = false;
}

void CALLBACK ThreadWorkTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    if (id == g_workTimer) {
        g_workTimer = 0;
    }
    RunThreadWork();
}

// Shell events, hotkeys and drops can arrive inside Explorer's own code or
// during a cross-apartment call, so the work runs later from a thread timer.
void ScheduleThreadWork() {
    if (!g_workTimer && !g_unloading && !g_threadClosing) {
        g_workTimer = SetTimer(nullptr, 0, 0, ThreadWorkTimerProc);
    }
}

void StopThreadWork() {
    if (g_workTimer) {
        KillTimer(nullptr, g_workTimer);
        g_workTimer = 0;
    }
    g_recentSyncPending = false;
    g_refreshPending = false;
    g_bookmarkTogglePending = nullptr;
    g_pendingNotice = {};
    if (g_noticeTimer) {
        KillTimer(nullptr, g_noticeTimer);
        g_noticeTimer = 0;
    }
}

void ScheduleRecentWork(bool sync, bool refresh) {
    if (!g_recentSettings.enabled || g_unloading) {
        return;
    }
    g_recentSyncPending |= sync;
    g_refreshPending |= refresh;
    ScheduleThreadWork();
}

// A thread message hook sees Ctrl+B before Explorer dispatches it, in the
// file list, the navigation pane and the bar; Explorer has no shortcut on it.
// It also receives refresh requests that other threads post to the frame.
thread_local HHOOK g_messageHook = nullptr;

UINT RefreshBarsMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_ExplorerFolderBookmarksBar_Refresh");
    return message;
}

void NotifyExplorerBars(bool thisProcessOnly) {
    if (g_unloading) {
        return;
    }
    // Marked windows can belong to another Explorer process using this mod.
    // Messages contain no pointers; each window reads shared storage itself.
    // Recent folders are in this process's memory, so only its windows need
    // to redraw them.
    EnumWindows([](HWND window, LPARAM thisProcess) -> BOOL {
        DWORD process = 0;
        if (GetPropW(window, kFrameProperty) &&
            (!thisProcess || (GetWindowThreadProcessId(window, &process) &&
                              process == GetCurrentProcessId()))) {
            PostMessageW(window, RefreshBarsMessage(), 0, 0);
        }
        return TRUE;
    }, thisProcessOnly);
}

bool FileListFocused() {
    HWND focus = GetFocus();
    wchar_t className[64]{};
    return focus && GetClassNameW(focus, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"DirectUIHWND") == 0;
}

bool IsTextInputElement(const winrt::Windows::Foundation::IInspectable& element) {
    return element && (element.try_as<muxc::TextBox>() ||
                       element.try_as<muxc::RichEditBox>() ||
                       element.try_as<muxc::PasswordBox>() ||
                       element.try_as<muxc::AutoSuggestBox>());
}

// Typing keeps Ctrl+B: rename fields are Win32 edit controls, and the address
// and search boxes are XAML text boxes in the frame's island.
bool TextInputFocused(HWND frame) {
    HWND focus = GetFocus();
    wchar_t className[64]{};
    if (focus && GetClassNameW(focus, className, ARRAYSIZE(className))) {
        if (wcscmp(className, L"DirectUIHWND") == 0 ||
            wcscmp(className, L"SysTreeView32") == 0) {
            return false;
        }
        if (_wcsicmp(className, L"Edit") == 0 ||
            _wcsnicmp(className, L"RichEdit", 8) == 0) {
            return true;
        }
    }
    try {
        for (const auto& bar : *g_bars) {
            auto root = bar.barRoot.get();
            if (!root || IslandWindow(root) != frame) {
                continue;
            }
            if (auto xamlRoot = root.XamlRoot();
                xamlRoot &&
                IsTextInputElement(muxi::FocusManager::GetFocusedElement(xamlRoot))) {
                return true;
            }
        }
    } catch (...) {
        return true;
    }
    return false;
}

// True when this thread installed the bookmarks bar in the frame.
// RemoveBarVisuals clears barRoot, so a non-empty barRoot means the bar is in
// place; on an unknown Windows build TryInstallBar leaves the header alone and
// this stays false. When the window of an installed bar cannot be determined
// from its XAML island, Ctrl+B is handled rather than ignored.
bool FrameHasInstalledBar(HWND frame) {
    bool unresolved = false;
    try {
        for (const auto& bar : *g_bars) {
            auto root = bar.barRoot.get();
            if (!root) {
                continue;
            }
            HWND window = IslandWindow(root);
            if (window == frame) {
                return true;
            }
            unresolved |= !window;
        }
    } catch (...) {
    }
    if (unresolved) {
        Wh_Log(L"Ctrl+B: the bookmarks bar window is unknown; handling the "
               L"key anyway");
    }
    return unresolved;
}

LRESULT CALLBACK ThreadMessageHook(int code, WPARAM wParam, LPARAM lParam) {
    auto message = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading &&
        message->message == RefreshBarsMessage()) {
        if (auto notice = DecodeNotice(message->hwnd, message->wParam);
            notice.kind != BookmarkNotice::None) {
            g_pendingNotice = notice;
        }
        message->message = WM_NULL;
        g_refreshPending = true;
        ScheduleThreadWork();
    }
    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading &&
        message->message == WM_KEYDOWN && message->wParam == 'B') {
        if (UpdateThreadSettings()) {
            g_refreshPending = true;
            ScheduleThreadWork();
        }
    }
    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading &&
        g_bookmarkHotkey &&
        message->message == WM_KEYDOWN && message->wParam == 'B' &&
        !(message->lParam & (1 << 30)) &&
        (GetKeyState(VK_CONTROL) & 0x8000) &&
        !(GetKeyState(VK_SHIFT) & 0x8000) &&
        !(GetKeyState(VK_MENU) & 0x8000) &&
        !(GetKeyState(VK_LWIN) & 0x8000) &&
        !(GetKeyState(VK_RWIN) & 0x8000)) {
        HWND frame = message->hwnd ? GetAncestor(message->hwnd, GA_ROOT)
                                   : nullptr;
        if (IsExplorerFrame(frame)) {
            if (!FrameHasInstalledBar(frame)) {
                Wh_Log(L"Ctrl+B ignored: no bookmarks bar in this window");
            } else if (!TextInputFocused(frame)) {
                message->message = WM_NULL;
                g_bookmarkTogglePending = frame;
                g_bookmarkToggleFromFileList = FileListFocused();
                ScheduleThreadWork();
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void InstallMessageHook() {
    if (g_messageHook || g_unloading) {
        return;
    }
    g_messageHook = SetWindowsHookExW(WH_GETMESSAGE, ThreadMessageHook,
                                      nullptr, GetCurrentThreadId());
    if (!g_messageHook) {
        Wh_Log(L"Could not install the message hook: %lu", GetLastError());
    }
}

void RemoveMessageHook() {
    if (g_messageHook) {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
}

bool RecordNavigation(IDispatch* dispatch, DISPID id) {
    if (!dispatch) {
        return false;
    }
    winrt::com_ptr<IServiceProvider> provider;
    winrt::com_ptr<IShellBrowser> browser;
    if (FAILED(dispatch->QueryInterface(IID_PPV_ARGS(provider.put()))) ||
        FAILED(provider->QueryService(kTopLevelBrowser,
                                      IID_PPV_ARGS(browser.put())))) {
        return false;
    }
    const auto folder = ShellBrowserFolder(browser.get());
    const bool changed = RecordExplorerRecent(folder);
    Wh_Log(L"Navigation event %d: %ls (%ls)", static_cast<int>(id),
           folder.c_str(), changed ? L"recorded" : L"not recorded");
    return changed;
}

void OnShellEvent(DISPID id, DISPPARAMS* params) try {
    if (g_threadClosing) {
        return;
    }
    if (UpdateThreadSettings()) {
        ScheduleThreadWork();
    }
    if (!g_recentSettings.enabled) {
        return;
    }
    switch (id) {
        case kDispidNavigateComplete2:
        case kDispidDocumentComplete:
            // The first argument is the tab that navigated. NavigateComplete2
            // can still report the previous folder, which is already the
            // newest entry; DocumentComplete then records the new one.
            if (params && params->cArgs >= 2 &&
                params->rgvarg[params->cArgs - 1].vt == VT_DISPATCH &&
                RecordNavigation(params->rgvarg[params->cArgs - 1].pdispVal, id)) {
                ScheduleRecentWork(false, true);
            }
            break;
        case kDispidWindowRegistered:
        case kDispidWindowRevoked:
            ScheduleRecentWork(true, false);
            break;
    }
} catch (...) {
    Wh_Log(L"Recent folder event failed: %08X", winrt::to_hresult().value);
}

namespace wdt = winrt::Windows::ApplicationModel::DataTransfer;

// Drops whose items are still being read. Unloading cancels these reads and
// waits until every completion has run, so none can call into the unloaded
// DLL; a read that ignores cancellation delays unloading until it finishes.
std::atomic<unsigned> g_pendingDrops = 0;
std::mutex g_dropReadsMutex;
std::vector<winrt::Windows::Foundation::IAsyncInfo> g_dropReads;

void ForgetDropRead(const winrt::Windows::Foundation::IUnknown& read) {
    std::lock_guard lock(g_dropReadsMutex);
    std::erase_if(g_dropReads,
                  [&](const auto& pending) { return pending == read; });
}

bool HasStorageItems(const mux::DragEventArgs& args) {
    return args.DataView().Contains(wdt::StandardDataFormats::StorageItems());
}

// Over the bookmark strip a drop is inserted at the marked position; over the
// recents it appends.
size_t ExternalDropIndex(const BarState& state, const muxc::StackPanel& panel,
                         const mux::DragEventArgs& args) {
    auto strip = state.strip.get();
    if (!strip) {
        return SIZE_MAX;
    }
    auto position = args.GetPosition(strip);
    if (position.X < 0 || position.Y < 0 || position.X > strip.ActualWidth() ||
        position.Y > strip.ActualHeight()) {
        return SIZE_MAX;
    }
    return BookmarkDropIndex(panel, args.GetPosition(panel));
}

void OnBarDragOver(const muxc::StackPanel& panel,
                   const mux::DragEventArgs& args) {
    auto state = FindState(panel);
    if (!state || g_unloading || !HasStorageItems(args)) {
        args.AcceptedOperation(wdt::DataPackageOperation::None);
        return;
    }
    // Never accept Move: the source would delete folders it believes were
    // moved. Link or Copy leaves them untouched.
    auto allowed = args.AllowedOperations();
    if ((allowed & wdt::DataPackageOperation::Link) ==
        wdt::DataPackageOperation::Link) {
        args.AcceptedOperation(wdt::DataPackageOperation::Link);
    } else if ((allowed & wdt::DataPackageOperation::Copy) ==
               wdt::DataPackageOperation::Copy) {
        args.AcceptedOperation(wdt::DataPackageOperation::Copy);
    } else {
        args.AcceptedOperation(wdt::DataPackageOperation::None);
        return;
    }
    args.DragUIOverride().Caption(L"Add to bookmarks");
    args.DragUIOverride().IsCaptionVisible(true);
    ShowInsertionMark(*state, panel, ExternalDropIndex(*state, panel, args));
}

void OnBarDrop(const muxc::StackPanel& panel, const mux::DragEventArgs& args) {
    auto state = FindState(panel);
    if (!state) {
        return;
    }
    ClearDropTarget(*state);
    if (g_unloading || !HasStorageItems(args)) {
        return;
    }
    if (args.AcceptedOperation() == wdt::DataPackageOperation::None) {
        Wh_Log(L"Drop ignored: no drop operation was accepted");
        return;
    }
    // No drop deferral: File Explorer's file list starts the drag on this
    // thread, and the item read needs this thread while a deferral would
    // keep it waiting inside the drop, freezing the window. The read finishes
    // after the drop returns; the bars then refresh on their own thread
    // through the message hook.
    size_t index = ExternalDropIndex(*state, panel, args);
    HWND frame = IslandWindow(panel);
    Wh_Log(L"Drop accepted: reading items, insertion index %d",
           index == SIZE_MAX ? -1 : static_cast<int>(index));
    auto request = args.DataView().GetStorageItemsAsync();
    auto onRead = [index, frame](
                      const auto& completed,
                      winrt::Windows::Foundation::AsyncStatus status) {
        try {
            if (status != winrt::Windows::Foundation::AsyncStatus::Completed) {
                Wh_Log(L"Dropped items could not be read: status %d",
                       static_cast<int>(status));
            }
            std::vector<std::wstring> paths;
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                for (const auto& item : completed.GetResults()) {
                    if (paths.size() >= kMaxFoldersPerAction) {
                        break;
                    }
                    if (item.IsOfType(
                            winrt::Windows::Storage::StorageItemTypes::Folder) &&
                        !item.Path().empty()) {
                        paths.emplace_back(item.Path().c_str());
                    }
                }
            }
            const auto change =
                g_unloading ? BookmarkChange{} : InsertBookmarks(paths, index);
            Wh_Log(L"Drop read finished: %d folder(s) found, inserted=%d, "
                   L"added=%d, no room=%d, already bookmarked=%d",
                   static_cast<int>(paths.size()), change.changed ? 1 : 0,
                   static_cast<int>(change.added),
                   static_cast<int>(change.noRoom),
                   static_cast<int>(change.existing));
            if (frame && (change.changed ||
                          change.Notice() != BookmarkNotice::None)) {
                PostMessageW(frame, RefreshBarsMessage(),
                             EncodeNotice(change), 0);
            }
        } catch (...) {
            Wh_Log(L"Could not bookmark dropped folders: %08X",
                   winrt::to_hresult().value);
        }
        ForgetDropRead(completed);
        --g_pendingDrops;
    };
    // Listed before registering: a read that has already finished completes
    // inside Completed() and removes itself again.
    {
        std::lock_guard lock(g_dropReadsMutex);
        g_dropReads.push_back(request);
    }
    ++g_pendingDrops;
    try {
        request.Completed(onRead);
    } catch (...) {
        // Nothing will complete this read, so it must not delay unloading.
        ForgetDropRead(request);
        --g_pendingDrops;
        Wh_Log(L"Could not wait for the dropped items: %08X",
               winrt::to_hresult().value);
    }
}

muxc::Grid FindNavigationGrid(const mux::DependencyObject& root, int depth) {
    if (!root || depth > 64) {
        return nullptr;
    }
    if (auto grid = root.try_as<muxc::Grid>();
        grid && grid.Name() == L"NavigationBarControlGrid") {
        return grid;
    }
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto grid = FindNavigationGrid(
                muxm::VisualTreeHelper::GetChild(root, i), depth + 1)) {
            return grid;
        }
    }
    return nullptr;
}

void RemoveBarVisuals(BarState& state) {
    RevokeHandlers(state.recentMenuHandlers);
    RevokeHandlers(state.recentHandlers);
    RevokeHandlers(state.fxMenuHandlers);
    RevokeHandlers(state.driveHandlers);
    RevokeHandlers(state.panelHandlers);
    auto root = state.barRoot.get();
    if (root) {
        root.PointerEntered(state.rootPointerToken);
        root.SizeChanged(state.rootSizeToken);
        root.Loaded(state.rootLoadedToken);
        root.DragOver(state.rootDragOverToken);
        root.DragLeave(state.rootDragLeaveToken);
        root.Drop(state.rootDropToken);
    }
    if (auto grid = state.grid.get()) {
        if (root) {
            auto children = grid.Children();
            for (unsigned i = 0; i < children.Size(); ++i) {
                if (children.GetAt(i) == root) {
                    children.RemoveAt(i);
                    break;
                }
            }
        }
        auto rows = grid.RowDefinitions();
        if (state.addedRow && rows.Size() &&
            rows.GetAt(rows.Size() - 1) == state.addedRow) {
            rows.RemoveAt(rows.Size() - 1);
        }
        if (state.createdFirstRow && rows.Size() == 1) {
            rows.RemoveAt(0);
        }
        if (grid.MinHeight() == state.appliedGridMinHeight) {
            grid.MinHeight(state.oldGridMinHeight);
        }
        grid.InvalidateMeasure();
    }
    if (auto nav = state.navControl.get()) {
        if (nav.MinHeight() == state.appliedNavMinHeight) {
            nav.MinHeight(state.oldNavMinHeight);
        }
        nav.InvalidateMeasure();
    }
    if (auto host = state.hostGrid.get()) {
        auto rows = host.RowDefinitions();
        if (state.commandRow && rows.Size() == 3 &&
            rows.GetAt(2) == state.commandRow &&
            state.commandRow.Height().GridUnitType ==
                mux::GridUnitType::Auto) {
            state.commandRow.Height(state.oldCommandRowHeight);
        }
        host.InvalidateMeasure();
    }
    state.grid = nullptr;
    state.hostGrid = nullptr;
    state.navControl = nullptr;
    state.barRoot = nullptr;
    state.strip = nullptr;
    state.buttons = nullptr;
    state.recents = nullptr;
    state.rootPointerToken = {};
    state.rootSizeToken = {};
    state.rootLoadedToken = {};
    state.rootDragOverToken = {};
    state.rootDragLeaveToken = {};
    state.rootDropToken = {};
    state.recentFolders.clear();
    state.folderButtons.clear();
    state.recentFolderButtons.clear();
    state.recentButtonCount = 0;
    state.hasRecentMenuButton = false;
    state.visibleRecents = 0;
    if (state.notice) {
        try {
            state.notice.Hide();
        } catch (...) {
            Wh_Log(L"Notice cleanup failed: %08X", winrt::to_hresult().value);
        }
        state.notice = nullptr;
        state.noticeText = nullptr;
    }
    state.addedRow = nullptr;
    state.commandRow = nullptr;
    state.createdFirstRow = false;
    state.originalGridHeight = 0;
    state.originalNavHeight = 0;
    state.rowCount = 1;
    state.lastLayoutWidth = 0;
    state.reflowing = false;
    state.renderedKey.clear();
    state.renderedRecentKey.clear();
    state.renderedLabelKey.clear();
    state.renderComplete = false;
    state.lastFolderCheckAt = 0;
    ClearDrag(state);
    state.suppressClick.clear();
    if (!g_unloading) {
        UpdateFrameRowCount();
    }
}

thread_local std::vector<HWND> g_frames;

void CleanupClosedFrameState(HWND window) {
    if (!g_bars) {
        return;
    }
    const bool lastFrame = g_frames.empty();
    for (auto it = g_bars->begin(); it != g_bars->end();) {
        if (!lastFrame && it->frame != window) {
            ++it;
            continue;
        }
        try {
            if (auto commandBar = it->commandBar.get()) {
                commandBar.Loaded(it->loadedToken);
                commandBar.Unloaded(it->unloadedToken);
            }
            RemoveBarVisuals(*it);
        } catch (...) {
            Wh_Log(L"Closed frame cleanup failed: %08X", winrt::to_hresult().value);
        }
        it = g_bars->erase(it);
    }
    if (lastFrame) {
        StopThreadWork();
        StopRecentTracking();
        RemoveMessageHook();
        ReleaseThreadCaches();
        g_frameRows = 0;
    } else {
        ScheduleRecentWork(true, false);
    }
    Wh_Log(L"Explorer frame closed: %zu frame(s), %zu bar(s) left on thread",
           g_frames.size(), g_bars->size());
}

void DrainClosedFrames() {
    if (g_uiCallbackDepth || g_drainingClosedFrames) {
        return;
    }
    g_drainingClosedFrames = true;
    ReflowGuard guard{g_drainingClosedFrames};
    while (!g_closedFrames.empty()) {
        auto closed = std::exchange(g_closedFrames, {});
        for (HWND window : closed) {
            CleanupClosedFrameState(window);
        }
    }
}

void CleanupClosedFrame(HWND window) {
    RemovePropW(window, kFrameProperty);
    std::erase(g_frames, window);
    g_threadClosing = g_frames.empty();
    g_closedFrames.push_back(window);
    DrainClosedFrames();
}

LRESULT CALLBACK FrameSubclassProc(HWND window, UINT message, WPARAM wParam,
                                   LPARAM lParam, DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        try {
            CleanupClosedFrame(window);
        } catch (...) {
            Wh_Log(L"Frame destruction handler failed: %08X", winrt::to_hresult().value);
        }
    } else if (message == RefreshBarsMessage() && !g_unloading) {
        try {
            if (auto notice = DecodeNotice(window, wParam);
                notice.kind != BookmarkNotice::None) {
                g_pendingNotice = notice;
            }
            g_refreshPending = true;
            ScheduleThreadWork();
        } catch (...) {
            Wh_Log(L"Frame refresh request failed: %08X", winrt::to_hresult().value);
        }
        return 0;
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

bool TrackFrame(HWND window) {
    if (!window || GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) {
        return false;
    }
    if (std::find(g_frames.begin(), g_frames.end(), window) != g_frames.end()) {
        return true;
    }
    g_threadClosing = false;
    g_frames.push_back(window);
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(window, FrameSubclassProc, 0)) {
        const DWORD error = GetLastError();
        g_frames.pop_back();
        Wh_Log(L"Could not track Explorer frame lifetime: %u", error);
        return false;
    }
    if (!SetPropW(window, kFrameProperty, reinterpret_cast<HANDLE>(1))) {
        const DWORD error = GetLastError();
        Wh_Log(L"Could not mark Explorer frame for refresh notifications: %u", error);
    }
    return true;
}

void TryInstallBar(const muxc::CommandBar& commandBar);

void TryInstallBar(const muxc::CommandBar& commandBar) {
    BarState* state = nullptr;
    try {
    if (g_unloading || !g_frameHooked) {
        return;
    }
    auto root = commandBar.XamlRoot();
    if (!root || !root.Content()) {
        return;
    }
    auto grid = FindNavigationGrid(root.Content(), 0);
    if (!grid) {
        return;
    }

    for (auto& item : *g_bars) {
        if (item.commandBar.get() == commandBar) {
            state = &item;
            break;
        }
    }
    if (!state || (state->strip.get() && state->grid.get() == grid)) {
        return;
    }
    if (HWND frame = IslandWindow(commandBar); TrackFrame(frame)) {
        state->frame = frame;
    }
    // Another tab's command bar may already own the row in this grid.
    for (const auto& child : grid.Children()) {
        auto element = child.try_as<mux::FrameworkElement>();
        if (element && element.Name() == kBarName) {
            return;
        }
    }
    if (state->addedRow || state->strip.get()) {
        // Explorer rebuilt its navigation bar; undo the stale installation.
        RemoveBarVisuals(*state);
    }

    auto nav = muxm::VisualTreeHelper::GetParent(grid)
                   .try_as<mux::FrameworkElement>();
    auto host = nav ? muxm::VisualTreeHelper::GetParent(nav)
                          .try_as<muxc::Grid>()
                    : nullptr;
    if (!host) {
        return;
    }
    auto hostRows = host.RowDefinitions();
    // The measured Explorer header has [Auto, *, *] with navigation in row 1.
    // Leave unknown Windows builds untouched instead of altering another Grid.
    if (hostRows.Size() != 3 || muxc::Grid::GetRow(nav) != 1 ||
        hostRows.GetAt(0).Height().GridUnitType != mux::GridUnitType::Auto ||
        hostRows.GetAt(1).Height().GridUnitType != mux::GridUnitType::Star ||
        hostRows.GetAt(2).Height().GridUnitType != mux::GridUnitType::Star) {
        return;
    }
    const double originalGridHeight =
        std::max<double>(grid.ActualHeight(), grid.DesiredSize().Height);
    const double originalNavHeight =
        std::max<double>(nav.ActualHeight(), nav.DesiredSize().Height);
    muxc::RowDefinition row;
    row.Height(mux::GridLength{kRowHeight, mux::GridUnitType::Pixel});
    auto rows = grid.RowDefinitions();
    state->grid = winrt::make_weak(grid);
    if (rows.Size() == 0) {
        muxc::RowDefinition originalRow;
        rows.Append(originalRow);
        state->createdFirstRow = true;
    }
    unsigned rowIndex = rows.Size();
    state->hostGrid = winrt::make_weak(host);
    state->addedRow = row;
    rows.Append(row);

    // Otherwise the two equal star rows each consume half of the extra host
    // height. Auto keeps Explorer's command row at its natural 48 units.
    state->commandRow = hostRows.GetAt(2);
    state->oldCommandRowHeight = state->commandRow.Height();
    state->commandRow.Height(
        mux::GridLength{1.0, mux::GridUnitType::Auto});

    muxc::StackPanel buttons;
    buttons.Orientation(muxc::Orientation::Vertical);
    muxc::ScrollViewer strip;
    strip.Height(kRowHeight);
    // A visible scrollbar would consume the fixed row height and clip the
    // buttons. Horizontal panning remains available when row four overflows.
    strip.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Hidden);
    strip.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Disabled);
    strip.HorizontalScrollMode(muxc::ScrollMode::Enabled);
    strip.VerticalScrollMode(muxc::ScrollMode::Disabled);
    strip.Content(buttons);
    // Recents stay on the first row, outside the scrolling bookmark strip.
    muxc::StackPanel recents;
    recents.Orientation(muxc::Orientation::Horizontal);
    recents.Height(kRowHeight);
    recents.VerticalAlignment(mux::VerticalAlignment::Top);

    muxc::Grid barRoot;
    barRoot.Name(kBarName);
    barRoot.Height(kRowHeight);
    // A transparent background makes empty parts of the bar a drop target.
    barRoot.Background(muxm::SolidColorBrush(
        winrt::Windows::UI::Color{0, 0, 0, 0}));
    barRoot.AllowDrop(true);
    muxc::ColumnDefinition stripColumn;
    muxc::ColumnDefinition recentsColumn;
    recentsColumn.Width(mux::GridLength{1.0, mux::GridUnitType::Auto});
    barRoot.ColumnDefinitions().Append(stripColumn);
    barRoot.ColumnDefinitions().Append(recentsColumn);
    muxc::Grid::SetColumn(recents, 1);
    barRoot.Children().Append(strip);
    barRoot.Children().Append(recents);
    // Move the scroll and hit-test surface without changing row allocation.
    muxm::TranslateTransform barOffset;
    barOffset.Y(-kRowOpticalLift);
    barRoot.RenderTransform(barOffset);
    muxc::Grid::SetRow(barRoot, static_cast<int>(rowIndex));
    muxc::Grid::SetColumnSpan(
        barRoot,
        static_cast<int>(std::max(1u, grid.ColumnDefinitions().Size())));

    state->navControl = winrt::make_weak(nav);
    state->barRoot = winrt::make_weak(barRoot);
    state->strip = winrt::make_weak(strip);
    state->buttons = winrt::make_weak(buttons);
    state->recents = winrt::make_weak(recents);
    state->oldGridMinHeight = grid.MinHeight();
    state->originalGridHeight = originalGridHeight;
    state->originalNavHeight = originalNavHeight;
    state->appliedGridMinHeight =
        std::max(state->oldGridMinHeight, originalGridHeight + kRowHeight);
    grid.MinHeight(state->appliedGridMinHeight);
    if (nav) {
        state->oldNavMinHeight = nav.MinHeight();
        state->appliedNavMinHeight =
            std::max(state->oldNavMinHeight, originalNavHeight + kRowHeight);
        nav.MinHeight(state->appliedNavMinHeight);
    }

    grid.Children().Append(barRoot);
    grid.InvalidateMeasure();
    host.InvalidateMeasure();
    auto weakButtons = winrt::make_weak(buttons);
    state->rootPointerToken = barRoot.PointerEntered(GuardHandler(
        L"Bar PointerEntered handler", [weakButtons](auto const&, auto const&) {
            auto panel = weakButtons.get();
            if (!panel) {
                return;
            }
            RefreshPanel(panel);
        }));
    // The full bar width decides both the recents and the bookmark wrapping.
    state->rootSizeToken = barRoot.SizeChanged(GuardHandler(
        L"Bar SizeChanged handler",
        [weakButtons](auto const&, const mux::SizeChangedEventArgs& args) {
            if (auto panel = weakButtons.get()) {
                ReflowPanel(panel, args.NewSize().Width);
            }
        }));
    state->rootLoadedToken = barRoot.Loaded(GuardHandler(
        L"Bar Loaded handler", [weakButtons](auto const&, auto const&) {
            if (auto panel = weakButtons.get()) {
                if (auto bar = FindState(panel)) {
                    if (auto root = bar->barRoot.get()) {
                        ReflowPanel(panel, root.ActualWidth());
                    }
                }
            }
        }));
    state->rootDragOverToken = barRoot.DragOver(GuardHandler(
        L"Bar DragOver handler",
        [weakButtons](auto const&, const mux::DragEventArgs& args) {
            if (auto panel = weakButtons.get()) {
                OnBarDragOver(panel, args);
            }
        }));
    state->rootDragLeaveToken = barRoot.DragLeave(GuardHandler(
        L"Bar DragLeave handler", [weakButtons](auto const&, auto const&) {
            if (auto panel = weakButtons.get()) {
                if (auto bar = FindState(panel)) {
                    ClearDropTarget(*bar);
                }
            }
        }));
    state->rootDropToken = barRoot.Drop(GuardHandler(
        L"Bar Drop handler",
        [weakButtons](auto const&, const mux::DragEventArgs& args) {
            if (auto panel = weakButtons.get()) {
                OnBarDrop(panel, args);
            }
        }));
    RefreshPanel(buttons);
    UpdateFrameRowCount();
    } catch (...) {
        Wh_Log(L"Bookmarks bar insertion failed: %08X",
               winrt::to_hresult().value);
        if (state) {
            try {
                RemoveBarVisuals(*state);
            } catch (...) {
                Wh_Log(L"Bookmarks bar rollback failed: %08X",
                       winrt::to_hresult().value);
            }
        }
    }
}

void TrackCommandBar(const muxc::CommandBar& commandBar) try {
    // Building the bar asks Shell for folder icons, so it is an operation
    // unloading waits for, like an event handler.
    OperationScope operation;
    if (!operation.active || !commandBar ||
        commandBar.Name() != L"FileExplorerCommandBar") {
        return;
    }
    UpdateThreadSettings();
    for (const auto& state : *g_bars) {
        if (state.commandBar.get() == commandBar) {
            TryInstallBar(commandBar);
            return;
        }
    }
    // Tabs come and go; forget bars whose command bar and strip are gone.
    g_bars->remove_if([](BarState& state) {
        if (state.commandBar.get() || state.strip.get()) {
            return false;
        }
        RevokeHandlers(state.recentMenuHandlers);
        RevokeHandlers(state.recentHandlers);
        RevokeHandlers(state.fxMenuHandlers);
        RevokeHandlers(state.driveHandlers);
        RevokeHandlers(state.panelHandlers);
        RemoveBarVisuals(state);
        return true;
    });
    UpdateFrameRowCount();
    g_bars->emplace_back();
    BarState& state = g_bars->back();
    if (HWND frame = IslandWindow(commandBar); TrackFrame(frame)) {
        state.frame = frame;
    }
    state.commandBar = winrt::make_weak(commandBar);
    auto weakBar = state.commandBar;
    state.loadedToken = commandBar.Loaded(GuardHandler(
        L"Command bar Loaded handler", [weakBar](auto const&, auto const&) {
            if (auto bar = weakBar.get()) {
                TryInstallBar(bar);
            }
        }));
    state.unloadedToken = commandBar.Unloaded(GuardHandler(
        L"Command bar Unloaded handler",
        [weakBar](auto const&, auto const&) {
            auto bar = weakBar.get();
            // WinUI can raise Unloaded after a quick re-add has already raised
            // Loaded; keep the bar while its command bar is in the tree.
            if (!bar || bar.IsLoaded()) {
                return;
            }
            for (auto& state : *g_bars) {
                if (state.commandBar.get() == bar) {
                    // Another tab's command bar can share a header that stays
                    // on screen; keep the bar there instead of removing it.
                    if (auto strip = state.strip.get();
                        strip && strip.IsLoaded()) {
                        break;
                    }
                    try {
                        RemoveBarVisuals(state);
                    } catch (...) {
                        Wh_Log(L"Bookmarks bar unload cleanup failed: %08X",
                               winrt::to_hresult().value);
                    }
                    break;
                }
            }
            if (std::none_of(g_bars->begin(), g_bars->end(),
                             [](const BarState& state) {
                                 return !!state.strip.get();
                             })) {
                g_iconCache.clear();
            }
        }));
    TryInstallBar(commandBar);
    InstallMessageHook();
    // A new command bar usually means a new tab to follow for recents.
    ScheduleRecentWork(true, false);
} catch (...) {
    Wh_Log(L"Bookmarks bar tracking failed: %08X",
           winrt::to_hresult().value);
}

void CollectCommandBars(const mux::DependencyObject& element, int depth,
                        std::vector<muxc::CommandBar>& bars) {
    if (!element || depth > 64) {
        return;
    }
    if (auto bar = element.try_as<muxc::CommandBar>();
        bar && bar.Name() == L"FileExplorerCommandBar") {
        bars.push_back(bar);
        return;
    }
    int count = muxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        CollectCommandBars(muxm::VisualTreeHelper::GetChild(element, i),
                           depth + 1, bars);
    }
}

void ScanXamlRootForCommandBars(const mux::UIElement& element) {
    if (!element) {
        return;
    }
    auto root = element.XamlRoot();
    if (!root || !root.Content()) {
        return;
    }
    std::vector<muxc::CommandBar> bars;
    CollectCommandBars(root.Content(), 0, bars);
    for (const auto& bar : bars) {
        TrackCommandBar(bar);
    }
}

void ScanCurrentThreadForCommandBars() try {
    if (g_unloading || !g_frameHooked) {
        return;
    }
    std::vector<winrt::weak_ref<muxc::CommandBar>> knownBars;
    for (const auto& state : *g_bars) {
        knownBars.push_back(state.commandBar);
    }
    for (const auto& weakBar : knownBars) {
        if (auto bar = weakBar.get()) {
            ScanXamlRootForCommandBars(bar);
        }
    }
    auto focused = muxi::FocusManager::GetFocusedElement();
    if (auto element = focused ? focused.try_as<mux::UIElement>() : nullptr) {
        ScanXamlRootForCommandBars(element);
    }
} catch (...) {
    Wh_Log(L"Bookmarks bar scan failed: %08X",
           winrt::to_hresult().value);
}

// CommandBarManager receives an actual typed WinUI CommandBar parameter. Its
// implementation object is intentionally never cast to a XAML object.
using CommandBarSetter = void(WINAPI*)(void*, void*);
CommandBarSetter g_commandBarSetterOriginal = nullptr;
void WINAPI CommandBarSetterHook(void* self, void* value) {
    g_commandBarSetterOriginal(self, value);
    if (!g_unloading && value) {
        TrackCommandBar(*reinterpret_cast<muxc::CommandBar*>(value));
    }
}

// The stock header rows are 38/48/48 at 96 DPI. The navigation
// Grid overhangs its row by 3 units at both ends. For N wrapped rows, reserve
// 38*N + 2*(N-1) + 6 = 40*N + 4 units. The original getter returns a fresh
// physical-pixel size on each call; the ratio scales with DPI.
using DesiredSizeGetter = HRESULT(WINAPI*)(void*, SIZE*);
DesiredSizeGetter g_desiredSizeOriginal = nullptr;
HRESULT WINAPI DesiredSizeHook(void* self, SIZE* size) {
    HRESULT result = g_desiredSizeOriginal(self, size);
    if (FAILED(result) || !size || g_unloading || g_frameRows == 0) {
        return result;
    }
    const LONG originalHeight = size->cy;
    const bool eligible = originalHeight >= 80 && originalHeight <= 600;
    const unsigned frameRows = std::min(g_frameRows, kMaxRows);
    const LONG extra = eligible
                           ? MulDiv(originalHeight, HostExtraAt96Dpi(frameRows),
                                    kMeasuredHostHeightAt96Dpi)
                           : 0;
    size->cy += extra;
    return result;
}

bool HookExplorerFrame(bool apply) {
    if (g_frameHooked) {
        return true;
    }
    HMODULE module = GetModuleHandleW(L"Windows.UI.FileExplorer.dll");
    if (!module) {
        return true;
    }
    // Bound symbol resolution to one attempt for this Explorer process;
    // repeated failures would invalidate Windhawk's symbol cache.
    bool expected = false;
    if (!g_frameHookAttempted.compare_exchange_strong(expected, true)) {
        return g_frameHooked;
    }
    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
        &g_desiredSizeOriginal, DesiredSizeHook}};
    if (!WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook)) ||
        !g_desiredSizeOriginal) {
        Wh_Log(L"File Explorer frame size symbol unavailable");
        return false;
    }
    g_frameHooked = true;
    if (apply) {
        Wh_ApplyHookOperations();
    }
    return true;
}

bool HookExplorerExtension(bool apply) {
    if (g_extensionHooked) {
        return true;
    }
    HMODULE module = GetModuleHandleW(L"FileExplorerExtensions.dll");
    if (!module) {
        return true;
    }
    // Bound symbol resolution to one attempt for this Explorer process.
    bool expected = false;
    if (!g_extensionHookAttempted.compare_exchange_strong(expected, true)) {
        return g_extensionHooked;
    }
    // FileExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))"},
        &g_commandBarSetterOriginal, CommandBarSetterHook}};
    if (!WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook)) ||
        !g_commandBarSetterOriginal) {
        Wh_Log(L"File Explorer command bar symbol unavailable");
        return false;
    }
    g_extensionHooked = true;
    if (apply) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_loadLibraryOriginal = nullptr;
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR file, HANDLE handle, DWORD flags) {
    HMODULE module = g_loadLibraryOriginal(file, handle, flags);
    if (module && file && !g_unloading &&
        (!g_extensionHookAttempted || !g_frameHookAttempted)) {
        const wchar_t* base = file;
        for (const wchar_t* p = file; *p; ++p) {
            if (*p == L'\\' || *p == L'/') {
                base = p + 1;
            }
        }
        if (_wcsicmp(base, L"Windows.UI.FileExplorer.dll") == 0) {
            HookExplorerFrame(true);
        } else if (_wcsicmp(base, L"FileExplorerExtensions.dll") == 0) {
            HookExplorerExtension(true);
        }
    }
    return module;
}

void ForExplorerWindows(void (*callback)());

void CleanupCurrentThread() {
    auto frames = std::exchange(g_frames, {});
    for (HWND frame : frames) {
        RemovePropW(frame, kFrameProperty);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(frame, FrameSubclassProc);
    }
    RemoveMessageHook();
    StopThreadWork();
    try {
        StopRecentTracking();
    } catch (...) {
        Wh_Log(L"Recent folder tracking cleanup failed: %08X",
               winrt::to_hresult().value);
    }
    if (!g_bars) {
        // Another window on this thread already ran this cleanup.
        return;
    }
    bool hadBars = !g_bars->empty();
    for (auto& state : *g_bars) {
        try {
            if (auto commandBar = state.commandBar.get()) {
                commandBar.Loaded(state.loadedToken);
                commandBar.Unloaded(state.unloadedToken);
            }
            RemoveBarVisuals(state);
        } catch (...) {
            Wh_Log(L"Bookmarks bar cleanup failed: %08X",
                   winrt::to_hresult().value);
        }
    }
    // The hooks, timers, listeners and handlers are gone and g_unloading is
    // set, so nothing on this thread reads g_bars after this.
    g_bars.reset();
    ReleaseThreadCaches();
    if (hadBars) {
        // The size hook is inactive now, so Explorer returns to its stock
        // header height instead of leaving an empty band until a resize.
        RelayoutThreadFrames();
    }
}

struct ThreadCall {
    void (*callback)();
};

UINT ThreadCallMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_ExplorerFolderBookmarksBar_ThreadCall");
    return message;
}

LRESULT CALLBACK ThreadCallHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == ThreadCallMessage()) {
            auto call = reinterpret_cast<ThreadCall*>(cwp->lParam);
            call->callback();
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void RunOnWindowThread(HWND window, void (*callback)()) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return;
    }
    if (threadId == GetCurrentThreadId()) {
        callback();
        return;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, ThreadCallHook, nullptr,
                                   threadId);
    if (!hook) {
        return;
    }
    ThreadCall call{callback};
    SendMessageW(window, ThreadCallMessage(), 0,
                 reinterpret_cast<LPARAM>(&call));
    UnhookWindowsHookEx(hook);
}

void ForExplorerWindows(void (*callback)()) {
    EnumWindows(
        [](HWND window, LPARAM param) -> BOOL {
            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);
            if (processId != GetCurrentProcessId()) {
                return TRUE;
            }
            wchar_t className[64]{};
            if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"CabinetWClass") == 0) {
                RunOnWindowThread(
                    window, reinterpret_cast<void (*)()>(param));
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(callback));
}

void BeginUnloading() {
    std::lock_guard lock(g_operationMutex);
    g_unloading = true;
}

void CloseActiveDialogCurrentThread() {
    if (g_threadFileDialog) {
        g_threadFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_storageMutex.processMutex = CreateMutexW(
        nullptr, FALSE, L"Local\\Windhawk_ExplorerFolderBookmarksBar_Storage");
    if (!g_storageMutex.processMutex) {
        const DWORD error = GetLastError();
        Wh_Log(L"Could not synchronize bookmark storage: %u", error);
        return FALSE;
    }
    InitUserStorage();
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
                              ? reinterpret_cast<LoadLibraryExW_t>(
                                    GetProcAddress(kernelBase, "LoadLibraryExW"))
                              : nullptr;
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExWHook,
                                       &g_loadLibraryOriginal)) {
        Wh_Log(L"Could not hook kernelbase LoadLibraryExW");
        CloseHandle(g_storageMutex.processMutex);
        g_storageMutex.processMutex = nullptr;
        return FALSE;
    }
    // Ensure the frame symbol can be hooked before Explorer creates a window.
    if (!GetModuleHandleW(L"Windows.UI.FileExplorer.dll")) {
        LoadLibraryExW(L"Windows.UI.FileExplorer.dll", nullptr,
                       LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    HookExplorerFrame(false);
    HookExplorerExtension(false);
    return TRUE;
}

void Wh_ModAfterInit() {
    HookExplorerFrame(true);
    HookExplorerExtension(true);
    if (g_frameHooked && g_extensionHooked) {
        ForExplorerWindows(ScanCurrentThreadForCommandBars);
    }
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        EraseRecentsWhileOff();
        NotifyExplorerBars();
        Wh_Log(L"Settings published without reloading the mod: generation %llu",
               g_settingsGeneration.load());
    } catch (...) {
        Wh_Log(L"Could not apply changed settings: %08X", winrt::to_hresult().value);
    }
}

void Wh_ModBeforeUninit() {
    BeginUnloading();
}

void Wh_ModUninit() {
    BeginUnloading();
    for (;;) {
        ForExplorerWindows(CloseActiveDialogCurrentThread);
        std::unique_lock lock(g_operationMutex);
        if (g_activeOperations == 0) {
            break;
        }
        g_operationFinished.wait_for(lock, std::chrono::milliseconds(100));
    }
    // Cancel pending drop reads and wait until each completion has run; a
    // completion after unloading would call into the unloaded DLL. Cancel on
    // every pass, since a read may not take the first cancellation.
    for (unsigned pass = 0; g_pendingDrops != 0; ++pass) {
        std::vector<winrt::Windows::Foundation::IAsyncInfo> reads;
        {
            std::lock_guard lock(g_dropReadsMutex);
            reads = g_dropReads;
        }
        for (const auto& read : reads) {
            try {
                read.Cancel();
            } catch (...) {
                if (pass == 0) {
                    Wh_Log(L"Wh_ModUninit: could not cancel a dropped-item "
                           L"read: %08X",
                           winrt::to_hresult().value);
                }
            }
        }
        if (pass == 50) {
            Wh_Log(L"Wh_ModUninit: still waiting for %u dropped-item read(s)",
                   g_pendingDrops.load());
        }
        Sleep(100);
    }
    {
        std::lock_guard lock(g_dropReadsMutex);
        g_dropReads.clear();
    }
    ForExplorerWindows(CleanupCurrentThread);
    if (g_storageMutex.processMutex) {
        CloseHandle(g_storageMutex.processMutex);
        g_storageMutex.processMutex = nullptr;
    }
}
