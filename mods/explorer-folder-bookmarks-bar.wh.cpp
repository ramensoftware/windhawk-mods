// ==WindhawkMod==
// @id              explorer-folder-bookmarks-bar
// @name            Explorer Folder Bookmarks Bar
// @description     Adds an adaptive folder bookmarks bar to newly opened Windows 11 File Explorer windows.
// @version         0.7.8
// @author          Maxim Fomin
// @github          https://github.com/MaxITService
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -lruntimeobject -lwindowscodecs
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Folder Bookmarks Bar

For the Windows 11 WinUI File Explorer. A scrollable bookmark bar appears below
the address bar. Click **+** to bookmark the current filesystem folder. Click a
bookmark to navigate the active tab to it. Ctrl+click asks Explorer to open it
in a new tab. Drag a bookmark onto another to reorder the row. Middle-click a
bookmark to remove it from the bar. Removing a bookmark never deletes its
target folder.

![Explorer Folder Bookmarks Bar in File Explorer](https://raw.githubusercontent.com/MaxITService/EXPLORER-bookmarks-bar-windhawk/main/Promo/How-it-works.gif)

After enabling or updating the mod, open a new File Explorer window to use the
bar. Windows that were already open may remain unchanged. Opening a new window
is the supported way to activate the bar without relying on live window updates.

The bar expands from one to four rows as the window narrows. If bookmarks
still exceed the fourth row, the bar can pan sideways.
Left-click **FX**, next to **+**, for the profile (**~**), Desktop, Documents,
Downloads, and the custom folders listed in this mod's Windhawk settings.
Custom shortcuts are empty by default. Add a folder path and optional label in
**Settings → FX custom folders**; blank entries are ignored. Paths must be
absolute, and `%NAME%` environment variables are expanded. Local, network
(UNC or mapped drive) and removable-drive folders all work. A folder missing
from a local disk is hidden from FX until it exists again. Like bookmarks,
network and removable entries are shown without being checked; if one is
unavailable, Explorer reports it when you click it. New settings take effect in
newly opened Explorer windows. Right-click **FX** for all drives with their
labels. The list updates each time the menu opens. Ctrl+click a menu entry to
open it in a new tab.

Right-click **+** for **Save bookmarks** and **Load bookmarks**. The commands
open a file dialog so you can choose the JSON backup. The profile folder is
suggested initially. Loading replaces the current list only after the entire
UTF-8 JSON file passes validation.

The bookmark list lives in this mod's Windhawk local storage. This version
supports up to 32 folders and uses the folder name as the button label. Icons
come from Windows Shell, including desktop.ini custom folder icons. Hovering a
bookmark shows its full path. Virtual locations such as Home are ignored by
**+**. Bookmark removal never touches the target folder or its contents.
Missing folders on local fixed drives show a yellow warning icon. Icons are
cached for up to five minutes and refreshed when the bar redraws.

The XAML insertion point can change in a Windows update. If the bar is not
visible, disable the mod and check the Windhawk log before trying it again.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fxCustomFolders:
    - - label: ''
        $name: Menu label
        $description: Name shown for this folder in the FX menu. If blank, the folder's own name is used.
      - path: ''
        $name: Folder path
        $description: Full path of the folder, for example C:\Projects, \\server\share\Docs or %USERPROFILE%\Pictures. Leave blank to skip this entry. A folder missing from a local disk is hidden until it exists again.
  $name: FX custom folders
  $description: Adds your own folders to the menu of the FX button (the second button on the bookmarks bar, right after +). Left-click FX to open the menu. It always lists your profile (~), Desktop, Documents and Downloads, followed by the folders from this list in the same order (up to 24). Click an entry to open it in the current tab; Ctrl+click opens it in a new tab. Right-click FX for a list of all drives. Changes apply to Explorer windows opened after you save. Local, network and removable-drive folders all work. A folder missing from a local disk is hidden until it exists again; network and removable entries are always shown, and Explorer reports it if one is unavailable when you click it.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <exdisp.h>
#include <oleauto.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <robuffer.h>
#include <shellapi.h>
#include <wincodec.h>
#include <windhawk_utils.h>

// winbase.h defines a legacy macro that collides with a WinRT method.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
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
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxmi = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace wjson = winrt::Windows::Data::Json;

constexpr wchar_t kBarName[] = L"WindhawkExplorerFolderBookmarksBar";
constexpr size_t kMaxStorageChars = 30000;
constexpr size_t kMaxBookmarks = 32;
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
// Lift the visible strip without changing its reserved layout height.
constexpr double kRowOpticalLift = 3.0;
constexpr int kNavigationOverhang = 6;
constexpr int kMeasuredHostHeightAt96Dpi = 136;
constexpr size_t kFixedButtons = 2;
constexpr float kDragThreshold = 6.0f;

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

std::mutex g_storageMutex;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_extensionHooked = false;
std::atomic<bool> g_frameHooked = false;
std::atomic<bool> g_extensionHookAttempted = false;
std::atomic<bool> g_frameHookAttempted = false;
std::mutex g_dialogMutex;
std::condition_variable g_dialogFinished;
unsigned g_activeDialogOperations = 0;
thread_local IFileDialog* g_threadFileDialog = nullptr;

struct BarState {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token unloadedToken{};
    winrt::weak_ref<muxc::Grid> grid;
    winrt::weak_ref<muxc::Grid> hostGrid;
    winrt::weak_ref<mux::FrameworkElement> navControl;
    winrt::weak_ref<muxc::ScrollViewer> strip;
    winrt::weak_ref<muxc::StackPanel> buttons;
    winrt::event_token stripPointerToken{};
    winrt::event_token stripSizeToken{};
    winrt::event_token stripLoadedToken{};
    std::vector<std::function<void()>> panelHandlers;
    std::vector<std::function<void()>> driveHandlers;
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
    std::wstring renderedStorage;
    ULONGLONG lastFolderCheckAt = 0;
    std::wstring dragPath;
    winrt::weak_ref<muxc::Button> dragButton;
    winrt::Windows::Foundation::Point dragStart{};
    uint32_t dragPointerId = 0;
    bool dragIsMouse = false;
    bool dragging = false;
    std::wstring suppressClick;
    double dragOldOpacity = 1.0;
    winrt::weak_ref<muxc::Button> dropTarget;
    mux::Thickness dropOldThickness{};
    bool dropAfter = false;
};

// XAML objects must only be touched by their owning UI thread.
// Stable addresses matter while XAML layout callbacks run and new Explorer
// tabs can register another command bar on the same UI thread.
thread_local std::list<BarState> g_bars;
// Explorer's XAML window and its size hook run on the same UI thread. Use the
// largest active bar on that thread so another tab cannot be clipped.
thread_local unsigned g_frameRows = 0;

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

template <typename Element, typename Handler>
void TrackClick(std::vector<std::function<void()>>& handlers,
                const Element& element, Handler&& handler) {
    auto token = element.Click(std::forward<Handler>(handler));
    auto weak = winrt::make_weak(element);
    handlers.emplace_back([weak, token] {
        if (auto current = weak.get()) {
            current.Click(token);
        }
    });
}

template <typename Handler>
void TrackOpening(std::vector<std::function<void()>>& handlers,
                  const muxc::MenuFlyout& menu, Handler&& handler) {
    auto token = menu.Opening(std::forward<Handler>(handler));
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
    auto boxed = winrt::box_value(std::move(handler));
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

std::wstring NormalizePath(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

bool SamePath(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

enum class FolderStatus { Available, Missing, Unknown };

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

std::wstring ReadStorageLocked() {
    std::vector<wchar_t> buffer(kMaxStorageChars + 1);
    size_t chars = Wh_GetStringValue(L"folders", buffer.data(), buffer.size());
    if (chars == 0 || chars > kMaxStorageChars) {
        return {};
    }
    return std::wstring(buffer.data(), chars);
}

std::vector<std::wstring> SplitBookmarks(const std::wstring& storage) {
    std::vector<std::wstring> folders;
    size_t pos = 0;
    while (pos < storage.size() && folders.size() < kMaxBookmarks) {
        size_t end = storage.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = storage.size();
        }
        std::wstring path = NormalizePath(storage.substr(pos, end - pos));
        if (!path.empty()) {
            folders.push_back(std::move(path));
        }
        pos = end + 1;
    }
    return folders;
}

bool SaveBookmarksLocked(const std::vector<std::wstring>& folders) {
    std::wstring storage;
    for (const auto& folder : folders) {
        if (!storage.empty()) {
            storage += L'\n';
        }
        storage += folder;
    }
    return storage.size() <= kMaxStorageChars &&
           Wh_SetStringValue(L"folders", storage.c_str());
}

// Suggest the profile location for an export without fixing the user's choice.
std::wstring SuggestedBackupPath() {
    PWSTR profile = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_DEFAULT,
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

std::wstring KnownFolderPath(REFKNOWNFOLDERID folderId) {
    PWSTR value = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_DEFAULT, nullptr,
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
        folders.emplace_back(value.c_str());
    }
    if (!ValidateImportedFolders(folders)) {
        return false;
    }
    std::lock_guard lock(g_storageMutex);
    return SaveBookmarksLocked(folders);
} catch (...) {
    Wh_Log(L"Bookmark backup validation failed: %08X",
           winrt::to_hresult().value);
    return false;
}

bool AddBookmark(std::wstring path) {
    path = NormalizePath(std::move(path));
    if (path.empty() || path.find_first_of(L"\r\n") != std::wstring::npos) {
        return false;
    }
    std::lock_guard lock(g_storageMutex);
    auto folders = SplitBookmarks(ReadStorageLocked());
    if (folders.size() >= kMaxBookmarks ||
        std::any_of(folders.begin(), folders.end(),
                    [&](const auto& old) { return SamePath(old, path); })) {
        return false;
    }
    folders.push_back(std::move(path));
    return SaveBookmarksLocked(folders);
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

std::wstring ButtonLabel(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos || pos + 1 == path.size()) {
        return path;
    }
    return path.substr(pos + 1);
}

struct FxFolder {
    std::wstring label;
    std::wstring path;
};

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
    return TrimSetting(value.get() ? value.get() : L"");
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
        // Like bookmarks, only a folder known to be missing on a local fixed
        // drive is hidden; network and removable paths are never probed.
        if (CheckFolderStatus(path) == FolderStatus::Missing) {
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

struct DialogOperationScope {
    bool active = false;
    DialogOperationScope() {
        std::lock_guard lock(g_dialogMutex);
        if (!g_unloading) {
            ++g_activeDialogOperations;
            active = true;
        }
    }
    ~DialogOperationScope() {
        if (active) {
            std::lock_guard lock(g_dialogMutex);
            --g_activeDialogOperations;
            g_dialogFinished.notify_all();
        }
    }
};

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

std::wstring CurrentFolder(HWND explorerWindow) {
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    if (!browser) {
        return {};
    }
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

void OpenFolderInNewTab(HWND explorerWindow, const std::wstring& path) {
    // Windows 11 registers this Folder shell verb for Explorer tabs. Explorer
    // chooses which window receives the tab; HWND is the owner for the request.
    HINSTANCE result = ShellExecuteW(explorerWindow, L"opennewtab",
                                     path.c_str(), nullptr, nullptr,
                                     SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Wh_Log(L"Bookmark new-tab navigation failed: %d",
               static_cast<int>(reinterpret_cast<INT_PTR>(result)));
    }
}

BarState* FindState(const muxc::StackPanel& panel) {
    for (auto& state : g_bars) {
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
}

void UpdateFrameRowCount() {
    unsigned rows = 0;
    for (const auto& bar : g_bars) {
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
        button.Measure(winrt::Windows::Foundation::Size{10000, kRowHeight});
        double desired = button.DesiredSize().Width;
        if (!std::isfinite(desired) || desired <= 0) {
            auto margin = button.Margin();
            desired = button.ActualWidth() + margin.Left + margin.Right;
        }
        widths.push_back(std::max(1.0, desired));
    }
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
        if (used > 0 && used + desired > width + 0.5 &&
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
        it->pixels = FolderIconPixels(path);
        it->bitmap = {};
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

void RefreshPanel(const muxc::StackPanel& panel) {
    if (g_unloading) {
        return;
    }
    auto state = FindState(panel);
    if (!state) {
        return;
    }
    std::wstring storage;
    {
        std::lock_guard lock(g_storageMutex);
        storage = ReadStorageLocked();
    }
    ULONGLONG now = GetTickCount64();
    if (DragInProgress(*state) && panel.Children().Size() != 0) {
        return;
    }
    if (storage == state->renderedStorage && panel.Children().Size() != 0 &&
        state->lastFolderCheckAt != 0 &&
        now - state->lastFolderCheckAt < kFolderCheckIntervalMs) {
        return;
    }
    ClearDrag(*state);
    RevokeHandlers(state->driveHandlers);
    RevokeHandlers(state->panelHandlers);
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
        addButton, winrt::box_value(L"Bookmark this folder"));
    // ContextFlyout opens on right-click; ordinary left-click still adds the
    // current folder.
    std::wstring backupPath = SuggestedBackupPath();
    muxc::MenuFlyout backupMenu;
    muxc::MenuFlyoutItem saveItem;
    saveItem.Text(L"Save bookmarks");
    muxc::ToolTipService::SetToolTip(
        saveItem, winrt::box_value(L"Choose a JSON backup file"));
    TrackClick(state->panelHandlers, saveItem, [backupPath, weakPanel](
                       const winrt::Windows::Foundation::IInspectable& sender,
                       const mux::RoutedEventArgs&) {
        DialogOperationScope operation;
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
    loadItem.Text(L"Load bookmarks");
    muxc::ToolTipService::SetToolTip(
        loadItem, winrt::box_value(L"Choose a JSON backup to load"));
    TrackClick(state->panelHandlers, loadItem, [backupPath, weakPanel](
                       const winrt::Windows::Foundation::IInspectable& sender,
                       const mux::RoutedEventArgs&) {
        DialogOperationScope operation;
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
            if (auto panel = weakPanel.get()) {
                RefreshPanel(panel);
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
        if (window && AddBookmark(CurrentFolder(window))) {
            if (auto panel = weakPanel.get()) {
                RefreshPanel(panel);
            }
        }
    });
    firstRow.Children().Append(addButton);

    muxc::Button fxButton;
    muxc::TextBlock fxLabel;
    fxLabel.Text(L"FX");
    fxLabel.FontSize(13);
    fxButton.Content(fxLabel);
    fxButton.Width(kButtonHeight);
    fxButton.Height(kButtonHeight);
    fxButton.MinWidth(0);
    fxButton.MinHeight(0);
    fxButton.Padding(mux::Thickness{0, 0, 0, 0});
    fxButton.HorizontalContentAlignment(mux::HorizontalAlignment::Center);
    fxButton.VerticalContentAlignment(mux::VerticalAlignment::Center);
    fxButton.Margin(mux::Thickness{0, kButtonVerticalInset, 16,
                                   kButtonVerticalInset});
    muxc::ToolTipService::SetToolTip(
        fxButton, winrt::box_value(
            L"FX: left-click for folders; right-click for drives"));
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
        TrackClick(driveItem ? state->driveHandlers : state->panelHandlers,
                   item, [path, weakFxButton](
                       const winrt::Windows::Foundation::IInspectable&,
                       const mux::RoutedEventArgs&) {
            if (g_unloading) {
                return;
            }
            auto button = weakFxButton.get();
            HWND window = button ? ExplorerWindowForElement(button) : nullptr;
            if (!window) {
                return;
            }
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                OpenFolderInNewTab(window, path);
            } else {
                NavigateToFolder(window, path);
            }
        });
        menu.Items().Append(item);
    };
    appendLocation(foldersMenu, L"~", KnownFolderPath(FOLDERID_Profile));
    appendLocation(foldersMenu, L"Desktop", KnownFolderPath(FOLDERID_Desktop));
    appendLocation(foldersMenu, L"Documents", KnownFolderPath(FOLDERID_Documents));
    appendLocation(foldersMenu, L"Downloads", KnownFolderPath(FOLDERID_Downloads));
    const auto customFolders = LoadFxCustomFolders();
    if (!customFolders.empty()) {
        foldersMenu.Items().Append(muxc::MenuFlyoutSeparator{});
        for (const auto& folder : customFolders) {
            appendLocation(foldersMenu, folder.label, folder.path);
        }
    }
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
            muxc::MenuFlyoutItem unavailable;
            unavailable.Text(L"No available drives");
            unavailable.IsEnabled(false);
            menu.Items().Append(unavailable);
        }
    });
    muxc::MenuFlyoutItem waitingForDrives;
    waitingForDrives.Text(L"No available drives");
    waitingForDrives.IsEnabled(false);
    drivesMenu.Items().Append(waitingForDrives);
    fxButton.Flyout(foldersMenu);
    fxButton.ContextFlyout(drivesMenu);
    firstRow.Children().Append(fxButton);

    for (const auto& path : SplitBookmarks(storage)) {
        FolderStatus status = CheckFolderStatus(path);
        muxc::Button button;
        muxc::StackPanel content;
        content.Orientation(muxc::Orientation::Horizontal);
        content.VerticalAlignment(mux::VerticalAlignment::Center);
        content.Children().Append(FolderIcon(path, status));
        muxc::TextBlock label;
        label.Text(winrt::hstring(ButtonLabel(path)));
        label.VerticalAlignment(mux::VerticalAlignment::Center);
        label.Margin(mux::Thickness{8, 0, 0, 0});
        label.MaxWidth(155);
        label.FontSize(14);
        label.TextTrimming(mux::TextTrimming::CharacterEllipsis);
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
        std::wstring tooltip = path;
        if (status == FolderStatus::Missing) {
            tooltip += L"\nFolder not found";
        }
        tooltip += L"\nDrag to reorder; Ctrl+click for new tab"
                   L"\nMiddle-click to remove bookmark";
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring(tooltip)));
        // Buttons consume left presses. AddHandler receives them after the
        // class handler and keeps ordinary Click behavior for a short press.
        TrackPointer(state->panelHandlers, button,
            mux::UIElement::PointerPressedEvent(),
            muxi::PointerEventHandler{
                [path, weakPanel](const winrt::Windows::Foundation::IInspectable& sender,
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
                        if (RemoveBookmark(path)) {
                            RefreshPanel(panel);
                        }
                    } else if (point.Properties().IsLeftButtonPressed()) {
                        ClearDrag(*state);
                        state->suppressClick.clear();
                        state->dragPath = path;
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
        TrackPointer(state->panelHandlers, button,
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
                    if (dx * dx + dy * dy >=
                        kDragThreshold * kDragThreshold) {
                        if (!state->dragging) {
                            element.Opacity(0.55);
                        }
                        state->dragging = true;
                        state->suppressClick = state->dragPath;
                        if (position.Y >= 0 &&
                            position.Y <= panel.ActualHeight()) {
                            ShowInsertionMark(
                                *state, panel,
                                BookmarkDropIndex(panel, position));
                        } else {
                            ClearDropTarget(*state);
                        }
                        args.Handled(true);
                    }
                }});
        TrackPointer(state->panelHandlers, button,
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
                    auto source = state->dragPath;
                    ClearDrag(*state);
                    if (dragging) {
                        args.Handled(true);
                        if (position.Y >= 0 && position.Y <= panel.ActualHeight() &&
                            MoveBookmarkToIndex(
                                source,
                                BookmarkDropIndex(panel, position))) {
                            RefreshPanel(panel);
                        }
                    }
                }});
        // Touch and pen contact can be canceled without a release; mouse
        // presses are covered by DragInProgress.
        TrackPointer(state->panelHandlers, button,
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
        TrackClick(state->panelHandlers, button, [path, weakPanel](
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
            auto element = sender.try_as<mux::FrameworkElement>();
            HWND window = element ? ExplorerWindowForElement(element) : nullptr;
            if (window) {
                if (GetKeyState(VK_CONTROL) & 0x8000) {
                    OpenFolderInNewTab(window, path);
                } else {
                    NavigateToFolder(window, path);
                }
            }
        });
        firstRow.Children().Append(button);
    }
    state->renderedStorage = std::move(storage);
    state->lastFolderCheckAt = now;
    state->lastLayoutWidth = 0;
    if (auto strip = state->strip.get()) {
        ReflowPanel(panel, strip.ActualWidth());
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
    RevokeHandlers(state.driveHandlers);
    RevokeHandlers(state.panelHandlers);
    auto strip = state.strip.get();
    if (strip) {
        strip.PointerEntered(state.stripPointerToken);
        strip.SizeChanged(state.stripSizeToken);
        strip.Loaded(state.stripLoadedToken);
    }
    if (auto grid = state.grid.get()) {
        if (strip) {
            auto children = grid.Children();
            for (unsigned i = 0; i < children.Size(); ++i) {
                if (children.GetAt(i) == strip) {
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
    state.strip = nullptr;
    state.buttons = nullptr;
    state.stripPointerToken = {};
    state.stripSizeToken = {};
    state.stripLoadedToken = {};
    state.addedRow = nullptr;
    state.commandRow = nullptr;
    state.createdFirstRow = false;
    state.originalGridHeight = 0;
    state.originalNavHeight = 0;
    state.rowCount = 1;
    state.lastLayoutWidth = 0;
    state.reflowing = false;
    state.renderedStorage.clear();
    state.lastFolderCheckAt = 0;
    ClearDrag(state);
    state.suppressClick.clear();
    if (!g_unloading) {
        UpdateFrameRowCount();
    }
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

    for (auto& item : g_bars) {
        if (item.commandBar.get() == commandBar) {
            state = &item;
            break;
        }
    }
    if (!state || (state->strip.get() && state->grid.get() == grid)) {
        return;
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
    strip.Name(kBarName);
    strip.Height(kRowHeight);
    // Move the scroll and hit-test surface without changing row allocation.
    muxm::TranslateTransform stripOffset;
    stripOffset.Y(-kRowOpticalLift);
    strip.RenderTransform(stripOffset);
    // A visible scrollbar would consume the fixed row height and clip the
    // buttons. Horizontal panning remains available when row four overflows.
    strip.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Hidden);
    strip.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Disabled);
    strip.HorizontalScrollMode(muxc::ScrollMode::Enabled);
    strip.VerticalScrollMode(muxc::ScrollMode::Disabled);
    strip.Content(buttons);
    muxc::Grid::SetRow(strip, static_cast<int>(rowIndex));
    muxc::Grid::SetColumnSpan(
        strip, static_cast<int>(std::max(1u, grid.ColumnDefinitions().Size())));

    state->navControl = winrt::make_weak(nav);
    state->strip = winrt::make_weak(strip);
    state->buttons = winrt::make_weak(buttons);
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

    grid.Children().Append(strip);
    grid.InvalidateMeasure();
    host.InvalidateMeasure();
    auto weakButtons = winrt::make_weak(buttons);
    state->stripPointerToken =
        strip.PointerEntered([weakButtons](auto const&, auto const&) {
            auto panel = weakButtons.get();
            if (!panel) {
                return;
            }
            RefreshPanel(panel);
        });
    state->stripSizeToken = strip.SizeChanged(
        [weakButtons](auto const&, const mux::SizeChangedEventArgs& args) {
            if (auto panel = weakButtons.get()) {
                ReflowPanel(panel, args.NewSize().Width);
            }
        });
    state->stripLoadedToken = strip.Loaded(
        [weakButtons](auto const&, auto const&) {
            if (auto panel = weakButtons.get()) {
                if (auto bar = FindState(panel)) {
                    if (auto strip = bar->strip.get()) {
                        ReflowPanel(panel, strip.ActualWidth());
                    }
                }
            }
        });
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
    if (g_unloading || !commandBar ||
        commandBar.Name() != L"FileExplorerCommandBar") {
        return;
    }
    for (const auto& state : g_bars) {
        if (state.commandBar.get() == commandBar) {
            TryInstallBar(commandBar);
            return;
        }
    }
    // Tabs come and go; forget bars whose command bar and strip are gone.
    g_bars.remove_if([](BarState& state) {
        if (state.commandBar.get() || state.strip.get()) {
            return false;
        }
        RevokeHandlers(state.driveHandlers);
        RevokeHandlers(state.panelHandlers);
        return true;
    });
    UpdateFrameRowCount();
    g_bars.emplace_back();
    BarState& state = g_bars.back();
    state.commandBar = winrt::make_weak(commandBar);
    auto weakBar = state.commandBar;
    state.loadedToken = commandBar.Loaded([weakBar](auto const&, auto const&) {
        if (auto bar = weakBar.get()) {
            TryInstallBar(bar);
        }
    });
    state.unloadedToken = commandBar.Unloaded(
        [weakBar](auto const&, auto const&) {
            auto bar = weakBar.get();
            // WinUI can raise Unloaded after a quick re-add has already raised
            // Loaded; keep the bar while its command bar is in the tree.
            if (!bar || bar.IsLoaded()) {
                return;
            }
            for (auto& state : g_bars) {
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
            if (std::none_of(g_bars.begin(), g_bars.end(),
                             [](const BarState& state) {
                                 return !!state.strip.get();
                             })) {
                g_iconCache.clear();
            }
        });
    TryInstallBar(commandBar);
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
    for (const auto& state : g_bars) {
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
        {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
         LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const & __ptr64) __ptr64)"},
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
    bool hadBars = !g_bars.empty();
    for (auto& state : g_bars) {
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
    g_bars.clear();
    g_iconCache.clear();
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
    std::lock_guard lock(g_dialogMutex);
    g_unloading = true;
}

void CloseActiveDialogCurrentThread() {
    if (g_threadFileDialog) {
        g_threadFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    }
}

BOOL Wh_ModInit() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
                              ? reinterpret_cast<LoadLibraryExW_t>(
                                    GetProcAddress(kernelBase, "LoadLibraryExW"))
                              : nullptr;
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExWHook,
                                       &g_loadLibraryOriginal)) {
        Wh_Log(L"Could not hook kernelbase LoadLibraryExW");
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

void Wh_ModBeforeUninit() {
    BeginUnloading();
}

void Wh_ModUninit() {
    BeginUnloading();
    for (;;) {
        ForExplorerWindows(CloseActiveDialogCurrentThread);
        std::unique_lock lock(g_dialogMutex);
        if (g_activeDialogOperations == 0) {
            break;
        }
        g_dialogFinished.wait_for(lock, std::chrono::milliseconds(100));
    }
    ForExplorerWindows(CleanupCurrentThread);
}
