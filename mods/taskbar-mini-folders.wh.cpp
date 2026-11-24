// ==WindhawkMod==
// @id              taskbar-mini-folders
// @name            Taskbar Mini Folders
// @description     Adds mini folders with a 2x2 icon preview to the Windows 11 taskbar. Click one to open a panel with all its apps, files and shortcuts.
// @version         1.1.0
// @author          Sanekx Arcs
// @github          https://github.com/SanekxArcs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lgdi32 -lmsimg32 -ldwmapi -lshcore -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Mini Folders

Adds mini folders to the Windows 11 taskbar, like the app folders on a phone.
Each mini folder is a small tile that shows the first four icons inside it.
Click the tile and a panel opens above the taskbar (or beside it, if the
taskbar is on the side of the screen) with everything in the folder. Click an
icon to launch it.

Use it to group apps (Work, Games, Tools) and keep the taskbar short.

![An open mini folder with its apps](https://i.imgur.com/h4lhXaY.png)

![Mini folders in the taskbar and an empty mini folder](https://i.imgur.com/8uHZ4Qf.png)

## How it works

Every mini folder is a real folder on disk. By default it is
`%USERPROFILE%\TaskbarFolders\<folder name>`. The mod creates the folder the
first time you use **+ Add...** or **Open folder** in it, not before.
Whatever is in that folder shows up in the mini folder: shortcuts, programs,
files and other folders. Hidden and system files are skipped. Items are sorted
by name.

## Using it

| Action | Result |
|---|---|
| Click a mini folder | Opens its panel. Click it again, press Esc or click elsewhere to close the panel |
| Click an icon in the panel | Opens the app, file or folder |
| Right-click an icon | Open, Open file location, Remove from this folder (Delete for real files and folders) |
| **+ Add...** | Pick apps or files. A shortcut to each one is created in the folder |
| **Open folder** | Opens the folder in Explorer, so you can drag items in, rename or delete them |

"Remove from this folder" sends a shortcut to the Recycle Bin without asking.
For a real file or folder (not a shortcut) the menu item is called "Delete",
because it removes the real item: it goes to the Recycle Bin, and Windows may
ask you to confirm first. On a drive without a Recycle Bin (for example a
network drive) the item is deleted permanently, and Windows asks first.

The file picker of **+ Add...** starts in the Start menu programs folder, where
most installed apps have a shortcut.

## Settings

| Setting | Default | What it does |
|---|---|---|
| Mini folders | Apps, Work, Games | The list of mini folders. Each has a name and an optional path |
| Hide this mini folder | off | Hides one mini folder from the taskbar without deleting it. Its name, path and files stay, so you can show it again later |
| Folder path | empty | Leave empty to use `%USERPROFILE%\TaskbarFolders\<name>`. Any existing folder works, and environment variables are expanded |
| Position in the taskbar | Next to the Start button | Can also be after the last taskbar app, after a chosen number of taskbar icons, or at the far left edge of the taskbar |
| Number of taskbar icons before the mini folders | 3 | Only for "After a chosen number of taskbar icons". Start, Search and Task View count too. 0 = before the first icon |
| Mini folder button size | 40 | Size of the tile, from 28 to 56 |
| Icons per row in the opened folder | 4 | Width of the panel, from 1 to 10 icons |

Renaming a mini folder that uses the default path points it to a new, empty
folder. The old folder and its contents stay on disk.

The mini folders also work when the taskbar is on the left or right side of
the screen. Then the tiles form a column, "left" means "top", and the panel
opens beside the taskbar.

## Files and folders the mod creates

* The folder of a mini folder (by default inside `%USERPROFILE%\TaskbarFolders`),
  created when you first use **+ Add...** or **Open folder** in it.
* The shortcuts that **+ Add...** puts into that folder.

These are your files. The mod never deletes them, so they stay on disk after
the mod is disabled or removed. Delete the `TaskbarFolders` folder yourself if
you no longer need it.

## Performance

* Folder contents and icons are read on a background thread, never on the
  taskbar thread, so a slow disk or a network folder can't freeze the taskbar.
* Nothing runs in the background: no timers and no folder watching. The tile
  preview is refreshed when you open the mini folder or change it from the
  panel.
* Opening the same mini folder again reuses the icons that are already loaded.

## Limitations

* Windows 11 with the new taskbar only. It doesn't work with the old taskbar
  restored by ExplorerPatcher or similar tools.
* If you change a folder in Explorer, the tile preview updates the next time
  you open that mini folder.
* Dragging items onto a mini folder is not supported. Use **+ Add...** or
  **Open folder**.
* The mini folders stay together as one group, and they can't be dragged
  between the taskbar icons. Use the position settings to choose where the
  group goes.
* The panel does not scroll. A folder with many items gets more icons per row
  so that the panel fits the screen. Items that still don't fit are not shown.

Tip: unpin the normal taskbar icons and keep everything in mini folders.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folders:
  - - name: Apps
      $name: Folder name
    - path: ""
      $name: Folder path (empty = %USERPROFILE%\TaskbarFolders\<name>)
    - hidden: false
      $name: Hide this mini folder
      $description: Hides it from the taskbar but keeps its name, path and files, so you can show it again later
  - - name: Work
      $name: Folder name
    - path: ""
      $name: Folder path (empty = %USERPROFILE%\TaskbarFolders\<name>)
    - hidden: false
      $name: Hide this mini folder
      $description: Hides it from the taskbar but keeps its name, path and files, so you can show it again later
  - - name: Games
      $name: Folder name
    - path: ""
      $name: Folder path (empty = %USERPROFILE%\TaskbarFolders\<name>)
    - hidden: false
      $name: Hide this mini folder
      $description: Hides it from the taskbar but keeps its name, path and files, so you can show it again later
  $name: Mini folders
- position: left
  $name: Position in the taskbar
  $options:
  - left: Next to the Start button (with the apps)
  - right: After the last taskbar app
  - after: After a chosen number of taskbar icons
  - edge: Far left edge of the taskbar (top edge if the taskbar is vertical)
- afterIcons: 3
  $name: Number of taskbar icons before the mini folders
  $description: >-
    Only for the position "After a chosen number of taskbar icons". Counted
    from the left (from the top on a vertical taskbar). Start, Search and Task
    View count as icons too. 0 puts the mini folders before the first icon.
- buttonSize: 40
  $name: Mini folder button size (28 to 56)
- columns: 4
  $name: Icons per row in the opened folder (1 to 10)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>  // ButtonBase::Click
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// ============================================================================
//  Settings
// ============================================================================

struct FolderDef {
    std::wstring name;
    std::wstring dir;  // expanded, absolute
};

enum PositionMode { kPosStart = 0, kPosEnd = 1, kPosEdge = 2, kPosAfter = 3 };

struct Settings {
    std::vector<FolderDef> folders;
    int mode = 0;  // PositionMode
    int afterIcons = 3;  // for kPosAfter
    int buttonSize = 40;
    int columns = 4;
};

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring ExpandEnv(const std::wstring& s) {
    WCHAR buf[2048];
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), buf, ARRAYSIZE(buf));
    if (n == 0 || n > ARRAYSIZE(buf)) {
        return s;
    }
    return buf;
}

std::wstring SanitizeFileName(std::wstring s) {
    for (auto& c : s) {
        if (wcschr(L"\\/:*?\"<>|", c) || c < 32) {
            c = L'_';
        }
    }
    while (!s.empty() && (s.back() == L' ' || s.back() == L'.')) {
        s.pop_back();
    }
    return s.empty() ? L"Folder" : s;
}

void LoadSettings() {
    Settings s;
    for (int i = 0; i < 64; i++) {
        std::wstring n = WindhawkUtils::StringSetting::make(L"folders[%d].name", i).get();
        std::wstring p = WindhawkUtils::StringSetting::make(L"folders[%d].path", i).get();
        if (n.empty() && p.empty()) {
            break;
        }
        // "Hide this mini folder": skipped, but its settings and files stay.
        // A missing value (settings saved by version 1.0.0) reads as 0, shown.
        if (Wh_GetIntSetting(L"folders[%d].hidden", i)) {
            continue;
        }

        FolderDef f;
        if (p.empty()) {
            p = L"%USERPROFILE%\\TaskbarFolders\\" + SanitizeFileName(n);
        }
        f.dir = ExpandEnv(p);
        while (f.dir.size() > 3 && (f.dir.back() == L'\\' || f.dir.back() == L'/')) {
            f.dir.pop_back();
        }
        f.name = n.empty() ? std::wstring(PathFindFileNameW(f.dir.c_str())) : n;

        // The folder is not created here: see EnsureFolderExists.
        s.folders.push_back(std::move(f));
    }

    WindhawkUtils::StringSetting position = WindhawkUtils::StringSetting::make(L"position");
    s.mode = wcscmp(position.get(), L"right") == 0   ? kPosEnd
             : wcscmp(position.get(), L"edge") == 0  ? kPosEdge
             : wcscmp(position.get(), L"after") == 0 ? kPosAfter
                                                     : kPosStart;
    s.afterIcons = std::clamp(Wh_GetIntSetting(L"afterIcons"), 0, 99);

    s.buttonSize = std::clamp(Wh_GetIntSetting(L"buttonSize"), 28, 56);
    s.columns = std::clamp(Wh_GetIntSetting(L"columns"), 1, 10);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

// ============================================================================
//  Folder contents + icons
// ============================================================================

// A mini folder's directory is created only when the user acts on it
// ("+ Add...", "Open folder"), so the mod leaves nothing on disk for folders
// that are never used.
void EnsureFolderExists(const std::wstring& dir) {
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
}

bool IsShortcutExt(PCWSTR path) {
    PCWSTR ext = PathFindExtensionW(path);
    return _wcsicmp(ext, L".lnk") == 0 || _wcsicmp(ext, L".url") == 0;
}

std::wstring DisplayNameOf(const std::wstring& path) {
    std::wstring name = PathFindFileNameW(path.c_str());
    PCWSTR ext = PathFindExtensionW(name.c_str());
    if (_wcsicmp(ext, L".lnk") == 0 || _wcsicmp(ext, L".url") == 0 ||
        _wcsicmp(ext, L".exe") == 0) {
        name.resize(ext - name.c_str());
    }
    return name;
}

// Lists the visible items of a folder, sorted by display name. `stamp`
// receives a value that changes when an item is added, removed, renamed or
// modified.
std::vector<std::wstring> ListFolder(const std::wstring& dir, ULONGLONG* stamp = nullptr) {
    std::vector<std::wstring> items;
    if (stamp) {
        *stamp = 0;
    }
    ULONGLONG sum = 0;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return items;
    }
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }
        ULONGLONG written = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                            fd.ftLastWriteTime.dwLowDateTime;
        sum += (written ^ std::hash<std::wstring_view>{}(fd.cFileName)) * 0x9E3779B97F4A7C15ULL;
        items.push_back(dir + L"\\" + fd.cFileName);
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    if (stamp) {
        *stamp = sum;
    }

    std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
        return StrCmpLogicalW(DisplayNameOf(a).c_str(), DisplayNameOf(b).c_str()) < 0;
    });
    return items;
}

// Premultiplied BGRA pixels, top-down.
struct IconPixels {
    int w = 0;
    int h = 0;
    std::vector<BYTE> bgra;

    bool operator==(const IconPixels&) const = default;
};

bool GetIconPixels(const std::wstring& path, int size, IconPixels& out) {
    winrt::com_ptr<IShellItemImageFactory> factory;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(factory.put())))) {
        return false;
    }

    HBITMAP hbm = nullptr;
    if (FAILED(factory->GetImage({size, size}, SIIGBF_ICONONLY, &hbm)) || !hbm) {
        return false;
    }

    BITMAP bm{};
    GetObject(hbm, sizeof(bm), &bm);
    if (bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        DeleteObject(hbm);
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bm.bmWidth;
    bi.bmiHeader.biHeight = -bm.bmHeight;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    out.w = bm.bmWidth;
    out.h = bm.bmHeight;
    out.bgra.assign((size_t)out.w * out.h * 4, 0);

    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, hbm, 0, out.h, out.bgra.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(hbm);
    if (lines == 0) {
        return false;
    }

    bool anyAlpha = false;
    bool premultiplied = true;
    for (size_t i = 0; i < out.bgra.size(); i += 4) {
        BYTE a = out.bgra[i + 3];
        if (a) {
            anyAlpha = true;
        }
        if (out.bgra[i] > a || out.bgra[i + 1] > a || out.bgra[i + 2] > a) {
            premultiplied = false;
        }
    }
    for (size_t i = 0; i < out.bgra.size(); i += 4) {
        if (!anyAlpha) {
            out.bgra[i + 3] = 255;
        } else if (!premultiplied) {
            BYTE a = out.bgra[i + 3];
            out.bgra[i] = (BYTE)(out.bgra[i] * a / 255);
            out.bgra[i + 1] = (BYTE)(out.bgra[i + 1] * a / 255);
            out.bgra[i + 2] = (BYTE)(out.bgra[i + 2] * a / 255);
        }
    }
    return true;
}

// ============================================================================
//  Taskbar helpers
// ============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid;
            WCHAR cls[32];
            if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
                GetClassName(hWnd, cls, ARRAYSIZE(cls)) &&
                _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

HWND GetTaskbarUiWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }
    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

FrameworkElement EnumChildElements(FrameworkElement element,
                                   std::function<bool(FrameworkElement)> callback) {
    int count = wuxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child && callback(child)) {
            return child;
        }
    }
    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

// ============================================================================
//  XAML injection (runs on the taskbar UI thread)
// ============================================================================

constexpr WCHAR kPanelName[] = L"WhMiniFoldersPanel";
constexpr double kSpacing = 4;
constexpr double kPanelSideMargin = 4;

const UINT WM_APP_SHOW_FOLDER = WM_APP + 0x51;

std::atomic<bool> g_unloading;
std::atomic<HWND> g_popupWnd;

// Counts the queued injection callbacks that still exist, run or not, so
// Wh_ModUninit can wait until none of them is left in the dispatcher.
std::atomic<int> g_pendingDispatch;

struct PendingDispatchGuard {
    PendingDispatchGuard() { g_pendingDispatch++; }
    ~PendingDispatchGuard() { g_pendingDispatch--; }
    PendingDispatchGuard(const PendingDispatchGuard&) = delete;
    PendingDispatchGuard& operator=(const PendingDispatchGuard&) = delete;
};

struct FrameState {
    winrt::weak_ref<FrameworkElement> frame;
    bool queued = false;
    bool injected = false;
    int attempts = 0;
};

// The taskbar icon that got extra margin to make a gap for the mini folders
// (position "after a chosen number of taskbar icons").
struct GapState {
    winrt::weak_ref<FrameworkElement> item;
    Thickness originalMargin{};
    bool leading = false;
};

void RestoreGap(GapState& gap) {
    if (auto item = gap.item.get()) {
        item.Margin(gap.originalMargin);
    }
    gap.item = nullptr;
}

struct Injected {
    winrt::weak_ref<FrameworkElement> frame;
    winrt::weak_ref<wuxc::Panel> rootGrid;
    winrt::weak_ref<FrameworkElement> repeater;
    winrt::weak_ref<wuxc::StackPanel> panel;
    Thickness originalMargin{};
    bool changedMargin = false;
    bool setTransform = false;
    winrt::event_token layoutToken{};
    std::shared_ptr<GapState> gap;
};

struct ClickReg {
    winrt::weak_ref<wuxc::Button> button;
    winrt::event_token token;
};

// UI thread only.
std::vector<FrameState> g_frames;
std::vector<Injected> g_injected;
std::vector<ClickReg> g_clickRegs;

// What a mini folder button shows: the first items of the folder and their
// icons. Filled on the popup thread (RefreshPreviews), so the taskbar thread
// never touches the disk.
struct FolderPreview {
    std::wstring dir;
    bool loaded = false;
    std::vector<std::wstring> paths;
    std::vector<IconPixels> icons;  // same size as paths; w == 0 if no icon

    bool operator==(const FolderPreview&) const = default;
};

constexpr size_t kPreviewCount = 4;
constexpr int kPreviewIconPx = 32;

std::mutex g_previewMutex;
std::vector<FolderPreview> g_previews;  // by folder index

// UI thread only. The TaskListButton hook is only needed until a taskbar
// frame is found.
bool g_backupTriggerNeeded = true;

FrameState* FindFrameState(FrameworkElement const& frame) {
    for (auto& s : g_frames) {
        if (auto f = s.frame.get(); f && f == frame) {
            return &s;
        }
    }
    return nullptr;
}

wuxm::ImageSource MakeImageSource(const IconPixels& px) {
    wuxm::Imaging::WriteableBitmap wb(px.w, px.h);
    auto buffer = wb.PixelBuffer();

    // Windows::Storage::Streams::IBufferByteAccess
    static constexpr GUID IID_IBufferByteAccess = {
        0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};
    struct IBufferByteAccessLocal : ::IUnknown {
        virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
    };

    winrt::com_ptr<IBufferByteAccessLocal> access;
    winrt::check_hresult(static_cast<::IUnknown*>(winrt::get_abi(buffer))
                             ->QueryInterface(IID_IBufferByteAccess, access.put_void()));
    BYTE* data = nullptr;
    winrt::check_hresult(access->Buffer(&data));
    memcpy(data, px.bgra.data(), std::min<size_t>(px.bgra.size(), buffer.Capacity()));
    wb.Invalidate();
    return wb;
}

void OnFolderButtonClick(int folderIndex) {
    POINT pt;
    GetCursorPos(&pt);
    if (g_popupWnd) {
        PostMessage(g_popupWnd, WM_APP_SHOW_FOLDER, folderIndex, MAKELPARAM(pt.x, pt.y));
    }
}

wuxc::Button MakeFolderButton(int index, const FolderDef& folder, int size,
                              const FolderPreview* preview) {
    wuxc::Button button;
    button.Width(size);
    button.Height(size);
    button.Padding(Thickness{0, 0, 0, 0});
    button.BorderThickness(Thickness{0, 0, 0, 0});
    button.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
    button.CornerRadius(CornerRadius{4, 4, 4, 4});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    wuxc::ToolTipService::SetToolTip(button, winrt::box_value(folder.name));
    Automation::AutomationProperties::SetName(button, folder.name);

    double tile = std::round(size * 0.78);
    double gap = 2;
    double pad = 3;
    double mini = std::floor((tile - pad * 2 - gap) / 2);

    wuxc::Border border;
    border.Width(tile);
    border.Height(tile);
    border.CornerRadius(CornerRadius{6, 6, 6, 6});
    border.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0x40, 0x80, 0x80, 0x80}));
    border.Padding(Thickness{pad, pad, pad, pad});

    if (!preview || !preview->loaded || preview->dir != folder.dir) {
        // Not read yet: an empty tile. The panel is rebuilt once it's ready.
    } else if (preview->paths.empty()) {
        wuxc::FontIcon icon;
        icon.Glyph(L"\uE8B7");  // folder
        icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        icon.FontSize(std::round(tile * 0.5));
        border.Child(icon);
    } else {
        wuxc::Grid grid;
        grid.RowSpacing(gap);
        grid.ColumnSpacing(gap);
        for (int i = 0; i < 2; i++) {
            wuxc::RowDefinition row;
            row.Height(GridLengthHelper::FromPixels(mini));
            grid.RowDefinitions().Append(row);
            wuxc::ColumnDefinition col;
            col.Width(GridLengthHelper::FromPixels(mini));
            grid.ColumnDefinitions().Append(col);
        }
        for (size_t i = 0; i < preview->icons.size(); i++) {
            const IconPixels& px = preview->icons[i];
            if (px.w <= 0 || px.h <= 0) {
                continue;
            }
            wuxc::Image image;
            image.Width(mini);
            image.Height(mini);
            image.Stretch(wuxm::Stretch::Uniform);
            image.Source(MakeImageSource(px));
            wuxc::Grid::SetRow(image, (int)(i / 2));
            wuxc::Grid::SetColumn(image, (int)(i % 2));
            grid.Children().Append(image);
        }
        border.Child(grid);
    }

    button.Content(border);

    auto token = button.Click([index](auto const&, auto const&) {
        OnFolderButtonClick(index);
    });
    g_clickRegs.push_back({winrt::make_weak(button), token});

    return button;
}

// Are the taskbar icons aligned to the left (Settings > Personalization >
// Taskbar > Taskbar behaviors)? Asked on every layout pass, so the registry is
// read at most twice a second.
bool IsTaskbarLeftAligned() {
    static ULONGLONG lastRead;
    static bool leftAligned;
    ULONGLONG now = GetTickCount64();
    if (!lastRead || now - lastRead >= 500) {
        DWORD value = 1, size = sizeof(value);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &value, &size);
        leftAligned = value == 0;
        lastRead = now;
    }
    return leftAligned;
}

// How far the app buttons are moved to make room for the mini folders.
// Centered icons: half the panel width, so folders + apps stay centered.
// Left-aligned icons: the whole width when the folders come first, nothing
// when they come last.
double RepeaterShift(int mode, double width, bool leftAligned) {
    if (mode == kPosStart) {
        return leftAligned ? width : width / 2;
    }
    return leftAligned ? 0 : -width / 2;
}

void RebuildAllPanels();

// The taskbar is vertical when it's placed on the left or right side of the
// screen. Unknown (not laid out yet) counts as horizontal; the LayoutUpdated
// handler rebuilds the panel once the real size is known.
bool IsVerticalLayout(FrameworkElement const& rootGrid) {
    double w = rootGrid.ActualWidth(), h = rootGrid.ActualHeight();
    return w > 0 && h > 0 && h > w;
}

// UI thread. Rebuilds the panels from the dispatcher, not from inside a
// layout event. Several requests in a row become one rebuild.
bool g_rebuildQueued = false;

void QueueRebuildOnUiThread() {
    if (g_rebuildQueued || g_unloading) {
        return;
    }
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) {
        return;
    }
    auto pending = std::make_shared<PendingDispatchGuard>();
    g_rebuildQueued = true;
    bool queued = queue.TryEnqueue([pending]() {
        g_rebuildQueued = false;
        if (g_unloading) {
            return;
        }
        try {
            RebuildAllPanels();
        } catch (...) {
            Wh_Log(L"Rebuild error %08X", winrt::to_hresult());
        }
    });
    if (!queued) {
        g_rebuildQueued = false;
    }
}

bool InjectIntoFrame(FrameworkElement frame) {
    auto rootGridElement = FindChildByName(frame, L"RootGrid");
    if (!rootGridElement) {
        Wh_Log(L"RootGrid not found");
        return false;
    }
    auto rootGrid = rootGridElement.try_as<wuxc::Panel>();
    auto repeater = FindChildByName(rootGridElement, L"TaskbarFrameRepeater");
    if (!rootGrid || !repeater) {
        Wh_Log(L"TaskbarFrameRepeater not found");
        return false;
    }
    if (FindChildByName(rootGridElement, kPanelName)) {
        return true;  // already there
    }

    Settings s = GetSettings();
    if (s.folders.empty()) {
        return true;
    }

    // On a vertical taskbar everything below works along the Y axis instead
    // of the X axis: the tiles form a column, and "left" means "top".
    bool vertical = IsVerticalLayout(rootGridElement);

    wuxc::StackPanel panel;
    panel.Name(kPanelName);
    panel.Spacing(kSpacing);
    if (vertical) {
        panel.Orientation(wuxc::Orientation::Vertical);
        panel.HorizontalAlignment(HorizontalAlignment::Center);
        panel.VerticalAlignment(VerticalAlignment::Top);
        panel.Margin(Thickness{0, kPanelSideMargin, 0, kPanelSideMargin});
    } else {
        panel.Orientation(wuxc::Orientation::Horizontal);
        panel.HorizontalAlignment(HorizontalAlignment::Left);
        panel.VerticalAlignment(VerticalAlignment::Center);
        panel.Margin(Thickness{kPanelSideMargin, 0, kPanelSideMargin, 0});
    }
    if (s.mode != kPosEdge) {
        panel.Opacity(0);  // shown once it's positioned
    }

    std::vector<FolderPreview> previews;
    {
        std::lock_guard<std::mutex> lock(g_previewMutex);
        previews = g_previews;
    }
    for (size_t i = 0; i < s.folders.size(); i++) {
        panel.Children().Append(MakeFolderButton((int)i, s.folders[i], s.buttonSize,
                                                 i < previews.size() ? &previews[i] : nullptr));
    }

    if (auto grid = rootGrid.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRow(panel, wuxc::Grid::GetRow(repeater));
        wuxc::Grid::SetColumn(panel, wuxc::Grid::GetColumn(repeater));
        wuxc::Grid::SetRowSpan(panel, wuxc::Grid::GetRowSpan(repeater));
        wuxc::Grid::SetColumnSpan(panel, wuxc::Grid::GetColumnSpan(repeater));
    }

    rootGrid.Children().Append(panel);

    // Size of the tiles along the taskbar (width, or height if vertical).
    double contentLength = s.folders.size() * s.buttonSize + (s.folders.size() - 1) * kSpacing;
    double length = kPanelSideMargin * 2 + contentLength;

    Injected inj;
    inj.frame = winrt::make_weak(frame);
    inj.rootGrid = winrt::make_weak(rootGrid);
    inj.repeater = winrt::make_weak(repeater);
    inj.panel = winrt::make_weak(panel);
    inj.originalMargin = repeater.Margin();
    inj.gap = std::make_shared<GapState>();

    wuxm::TranslateTransform shift = nullptr;
    if (s.mode == kPosEdge) {
        // Far left (top) edge: push the taskbar buttons aside.
        Thickness margin = inj.originalMargin;
        (vertical ? margin.Top : margin.Left) += length;
        repeater.Margin(margin);
        inj.changedMargin = true;
    } else if (s.mode != kPosAfter && !repeater.RenderTransform()) {
        // Next to the Start button / after the last app: shift the app
        // buttons to make room (see RepeaterShift), then keep the panel
        // glued to the first/last button.
        shift = wuxm::TranslateTransform();
        double shiftBy = RepeaterShift(s.mode, length, vertical || IsTaskbarLeftAligned());
        vertical ? shift.Y(shiftBy) : shift.X(shiftBy);
        repeater.RenderTransform(shift);
        inj.setTransform = true;
    }

    int mode = s.mode;
    int afterIcons = s.afterIcons;
    auto gap = inj.gap;
    auto weakRoot = winrt::make_weak(rootGridElement);
    auto weakRepeater = winrt::make_weak(repeater);
    auto weakPanel = winrt::make_weak(panel);
    inj.layoutToken = repeater.LayoutUpdated(
        [weakRoot, weakRepeater, weakPanel, shift, gap, mode, afterIcons, vertical, length,
         contentLength](auto const&, auto const&) {
            if (g_unloading) {
                return;
            }
            try {
                auto root = weakRoot.get();
                auto rep = weakRepeater.get();
                auto pnl = weakPanel.get();
                if (!root || !rep || !pnl) {
                    return;
                }

                // The taskbar was moved between a side and the top/bottom
                // of the screen (or wasn't laid out yet): build it again.
                double rootW = root.ActualWidth(), rootH = root.ActualHeight();
                if (rootW > 0 && rootH > 0 && (rootH > rootW) != vertical) {
                    QueueRebuildOnUiThread();
                    return;
                }
                if (mode == kPosEdge) {
                    return;
                }

                // The taskbar alignment can be changed at any time.
                if (shift) {
                    double shiftBy = RepeaterShift(mode, length, vertical || IsTaskbarLeftAligned());
                    double current = vertical ? shift.Y() : shift.X();
                    if (current != shiftBy) {
                        vertical ? shift.Y(shiftBy) : shift.X(shiftBy);
                    }
                }

                // Visible buttons (Start + apps): where each one starts
                // along the taskbar, and its size along the taskbar.
                struct Item {
                    double pos;
                    double size;
                    FrameworkElement element;
                };
                std::vector<Item> items;
                double rootLength = vertical ? rootH : rootW;
                int count = wuxm::VisualTreeHelper::GetChildrenCount(rep);
                for (int i = 0; i < count; i++) {
                    auto item = wuxm::VisualTreeHelper::GetChild(rep, i)
                                    .try_as<FrameworkElement>();
                    if (!item || item.Visibility() != Visibility::Visible) {
                        continue;
                    }
                    double size = vertical ? item.ActualHeight() : item.ActualWidth();
                    if (size <= 0) {
                        continue;
                    }
                    auto pt = item.TransformToVisual(root).TransformPoint(
                        winrt::Windows::Foundation::Point{0, 0});
                    double pos = vertical ? pt.Y : pt.X;
                    if (pos < -1 || pos > rootLength) {
                        continue;  // recycled, off-screen
                    }
                    items.push_back({pos, size, item});
                }
                if (items.empty()) {
                    return;
                }

                double target;
                if (mode == kPosAfter) {
                    // Make a gap after icon number `afterIcons` by giving
                    // that icon extra margin (or the first icon, for 0). The
                    // taskbar then lays out and centers the icons itself.
                    std::sort(items.begin(), items.end(),
                              [](const Item& a, const Item& b) { return a.pos < b.pos; });
                    int n = std::min(afterIcons, (int)items.size());
                    const Item& anchor = items[n == 0 ? 0 : n - 1];
                    bool leading = n == 0;
                    if (gap->item.get() != anchor.element || gap->leading != leading) {
                        RestoreGap(*gap);
                        Thickness m = anchor.element.Margin();
                        gap->originalMargin = m;
                        gap->leading = leading;
                        gap->item = winrt::make_weak(anchor.element);
                        if (vertical) {
                            (leading ? m.Top : m.Bottom) += length;
                        } else {
                            (leading ? m.Left : m.Right) += length;
                        }
                        anchor.element.Margin(m);
                        return;  // the taskbar lays out again
                    }
                    target = leading ? anchor.pos - length + kPanelSideMargin
                                     : anchor.pos + anchor.size + kPanelSideMargin;
                } else {
                    double minPos = 1e9, maxPos = -1e9;
                    for (auto& it : items) {
                        minPos = std::min(minPos, it.pos);
                        maxPos = std::max(maxPos, it.pos + it.size);
                    }
                    target = mode == kPosStart ? minPos - kPanelSideMargin - contentLength
                                               : maxPos + kPanelSideMargin;
                }
                target = std::max(0.0, target);

                // Where the panel's layout slot starts (without its margin).
                Thickness m = pnl.Margin();
                auto cur = pnl.TransformToVisual(root).TransformPoint(
                    winrt::Windows::Foundation::Point{0, 0});
                double slot = vertical ? cur.Y - m.Top : cur.X - m.Left;
                double newStart = std::round(target - slot);
                double& start = vertical ? m.Top : m.Left;
                if (std::abs(newStart - start) > 0.5) {
                    start = newStart;
                    (vertical ? m.Bottom : m.Right) = 0;
                    pnl.Margin(m);
                } else if (pnl.Opacity() < 1) {
                    pnl.Opacity(1);
                }
            } catch (...) {
                Wh_Log(L"Position error %08X", winrt::to_hresult());
            }
        });

    g_injected.push_back(inj);
    Wh_Log(L"Injected %zu mini folders, mode %d, vertical %d", s.folders.size(), s.mode,
           vertical);
    return true;
}

void RemoveAllPanels() {
    for (auto& reg : g_clickRegs) {
        if (auto b = reg.button.get()) {
            b.Click(reg.token);
        }
    }
    g_clickRegs.clear();

    for (auto& inj : g_injected) {
        try {
            auto rootGrid = inj.rootGrid.get();
            auto panel = inj.panel.get();
            if (rootGrid && panel) {
                uint32_t index;
                if (rootGrid.Children().IndexOf(panel, index)) {
                    rootGrid.Children().RemoveAt(index);
                }
            }
            if (auto repeater = inj.repeater.get()) {
                if (inj.layoutToken) {
                    repeater.LayoutUpdated(inj.layoutToken);
                }
                if (inj.changedMargin) {
                    repeater.Margin(inj.originalMargin);
                }
                if (inj.setTransform) {
                    repeater.RenderTransform(nullptr);
                }
            }
            if (inj.gap) {
                RestoreGap(*inj.gap);
            }
        } catch (...) {
            Wh_Log(L"Remove error %08X", winrt::to_hresult());
        }
    }
    g_injected.clear();
}

void RebuildAllPanels() {
    std::vector<FrameworkElement> frames;
    for (auto& inj : g_injected) {
        if (auto f = inj.frame.get()) {
            frames.push_back(f);
        }
    }
    RemoveAllPanels();
    for (auto& f : frames) {
        try {
            InjectIntoFrame(f);
        } catch (...) {
            Wh_Log(L"Inject error %08X", winrt::to_hresult());
        }
    }
}

void QueueInject(FrameworkElement const& frame) {
    // Called on every taskbar layout pass: keep the common path short.
    FrameState* state = FindFrameState(frame);
    if (!state) {
        std::erase_if(g_frames, [](FrameState& s) { return !s.frame.get(); });
        g_frames.push_back({winrt::make_weak(frame)});
        state = &g_frames.back();
    }
    if (state->queued || state->injected || state->attempts >= 5) {
        return;
    }
    state->queued = true;
    state->attempts++;

    // Each copy of the callback holds `pending`, so the counter drops only
    // when the last copy is gone: after it ran, or if it was dropped unrun.
    auto pending = std::make_shared<PendingDispatchGuard>();
    auto weak = winrt::make_weak(frame);
    auto run = [weak, pending]() {
        if (!g_unloading) {
            try {
                if (auto f = weak.get()) {
                    bool ok = InjectIntoFrame(f);
                    if (FrameState* s = FindFrameState(f)) {
                        s->queued = false;
                        s->injected = ok;
                    }
                    if (ok) {
                        g_backupTriggerNeeded = false;
                    }
                }
            } catch (...) {
                Wh_Log(L"Inject error %08X", winrt::to_hresult());
                if (auto f = weak.get()) {
                    if (FrameState* s = FindFrameState(f)) {
                        s->queued = false;
                    }
                }
            }
        }
    };

    bool queued = false;
    try {
        // XAML islands (the Windows 11 taskbar) use DispatcherQueue.
        auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (queue) {
            queued = queue.TryEnqueue(run);
        }
        if (!queued) {
            auto dispatcher = frame.Dispatcher();
            if (dispatcher) {
                dispatcher.TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                                       run);
                queued = true;
            }
        }
    } catch (...) {
        Wh_Log(L"Queue error %08X", winrt::to_hresult());
    }
    if (!queued) {
        state->queued = false;
    }
}

using TaskbarFrame_MeasureOverride_t = int(WINAPI*)(void* pThis,
                                                    winrt::Windows::Foundation::Size size,
                                                    winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(void* pThis,
                                             winrt::Windows::Foundation::Size size,
                                             winrt::Windows::Foundation::Size* resultSize) {
    if (!g_unloading) {
        try {
            FrameworkElement frame = nullptr;
            ((::IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                                 winrt::put_abi(frame));
            if (frame) {
                QueueInject(frame);
            }
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }
    return TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
}

// Backup trigger: taskbar app buttons update their visual state often
// (hover, window opened/closed). Walk up to the TaskbarFrame from there.
// Does nothing once a frame has the mini folders.
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading || !g_backupTriggerNeeded) {
        return;
    }

    try {
        void* buttonIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown buttonIUnknown;
        winrt::copy_from_abi(buttonIUnknown, buttonIUnknownPtr);
        auto element = buttonIUnknown.try_as<FrameworkElement>();

        // TaskListButton -> TaskbarFrameRepeater -> RootGrid -> TaskbarFrame
        for (int i = 0; element && i < 6; i++) {
            auto parent = wuxm::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
            if (!parent) {
                return;
            }
            if (parent.Name() == L"TaskbarFrame") {
                if (i == 2) {
                    QueueInject(parent);
                }
                return;
            }
            element = parent;
        }
    } catch (...) {
        Wh_Log(L"UpdateVisualStates error %08X", winrt::to_hresult());
    }
}

void RequestRebuild() {
    HWND ui = GetTaskbarUiWnd();
    if (ui) {
        RunFromWindowThread(ui, [](PVOID) { RebuildAllPanels(); }, nullptr);
    }
}

// ============================================================================
//  Button previews (read on the popup thread)
// ============================================================================

const UINT WM_APP_REFRESH_PREVIEWS = WM_APP + 0x52;
const UINT WM_APP_HIDE = WM_APP + 0x53;

std::atomic<bool> g_refreshQueued;

// Any thread. Several requests in a row become one refresh.
void QueuePreviewRefresh() {
    if (g_popupWnd && !g_refreshQueued.exchange(true) &&
        !PostMessage(g_popupWnd, WM_APP_REFRESH_PREVIEWS, 0, 0)) {
        g_refreshQueued = false;
    }
}

// Popup thread. Reads the first items of every folder and their icons, and
// rebuilds the taskbar buttons only if something changed.
void RefreshPreviews() {
    g_refreshQueued = false;

    Settings s = GetSettings();
    std::vector<FolderPreview> fresh(s.folders.size());
    for (size_t i = 0; i < s.folders.size(); i++) {
        if (g_unloading) {
            return;
        }
        FolderPreview& p = fresh[i];
        p.dir = s.folders[i].dir;
        p.loaded = true;
        p.paths = ListFolder(p.dir);
        if (p.paths.size() > kPreviewCount) {
            p.paths.resize(kPreviewCount);
        }
        for (auto& path : p.paths) {
            if (g_unloading) {
                return;
            }
            IconPixels px;
            if (!GetIconPixels(path, kPreviewIconPx, px)) {
                px = {};
            }
            p.icons.push_back(std::move(px));
        }
    }

    bool changed;
    {
        std::lock_guard<std::mutex> lock(g_previewMutex);
        changed = g_previews != fresh;
        if (changed) {
            g_previews = std::move(fresh);
        }
    }
    if (changed && !g_unloading) {
        RequestRebuild();
    }
}

// ============================================================================
//  Popup panel (own thread, plain Win32)
// ============================================================================

struct PopupItem {
    std::wstring path;
    std::wstring label;
    HBITMAP bmp = nullptr;
    int bw = 0;
    int bh = 0;
    RECT rc{};
};

enum HitId { kHitNone = -1, kHitAdd = 100000, kHitOpen = 100001 };

struct PopupState {
    int folder = -1;
    std::wstring title;
    std::wstring dir;
    POINT anchor{};
    UINT dpi = 96;
    std::vector<PopupItem> items;
    RECT rcAdd{};
    RECT rcOpen{};
    RECT rcEmpty{};
    int hover = kHitNone;
    bool trackingLeave = false;
    bool light = false;
    HFONT font = nullptr;
    HFONT fontTitle = nullptr;
    int iconPx = 40;
    DWORD hiddenTick = 0;
    int hiddenFolder = -1;
    // What `items` was loaded from, to reuse it when nothing changed.
    std::wstring itemsDir;
    ULONGLONG itemsStamp = 0;
} g_popup;

HANDLE g_popupThread;

int Px(int dip) {
    return MulDiv(dip, g_popup.dpi, 96);
}

bool SystemUsesLightTheme() {
    DWORD value = 0, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

void PopupFreeItems() {
    for (auto& it : g_popup.items) {
        if (it.bmp) {
            DeleteObject(it.bmp);
        }
    }
    g_popup.items.clear();
    g_popup.itemsDir.clear();
}

HBITMAP MakeDib(const IconPixels& px) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = px.w;
    bi.bmiHeader.biHeight = -px.h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bmp && bits) {
        memcpy(bits, px.bgra.data(), px.bgra.size());
    }
    return bmp;
}

void PopupHide() {
    if (IsWindowVisible(g_popupWnd)) {
        ShowWindow(g_popupWnd, SW_HIDE);
        g_popup.hiddenTick = GetTickCount();
        g_popup.hiddenFolder = g_popup.folder;
    }
    g_popup.hover = kHitNone;
}

void PopupShow(int folder, POINT anchor, bool force) {
    // The window may already be gone when a file picker, a menu or a
    // confirmation returns while the mod is being unloaded.
    if (g_unloading || !IsWindow(g_popupWnd)) {
        return;
    }

    if (!force) {
        if (IsWindowVisible(g_popupWnd) && g_popup.folder == folder) {
            PopupHide();
            return;
        }
        // The click that closed the popup (by deactivating it) also arrives
        // here: treat it as a toggle.
        if (!IsWindowVisible(g_popupWnd) && g_popup.hiddenFolder == folder &&
            GetTickCount() - g_popup.hiddenTick < 400) {
            return;
        }
    }

    Settings s = GetSettings();
    if (folder < 0 || folder >= (int)s.folders.size()) {
        return;
    }

    g_popup.folder = folder;
    g_popup.anchor = anchor;
    g_popup.title = s.folders[folder].name;
    g_popup.dir = s.folders[folder].dir;
    g_popup.light = SystemUsesLightTheme();
    g_popup.hover = kHitNone;

    HMONITOR mon = MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    g_popup.dpi = dpiX;

    if (g_popup.font) DeleteObject(g_popup.font);
    if (g_popup.fontTitle) DeleteObject(g_popup.fontTitle);
    g_popup.font = CreateFontW(-Px(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH, L"Segoe UI");
    g_popup.fontTitle = CreateFontW(-Px(14), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    // The panel doesn't scroll: it must fit the work area of the monitor.
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(mon, &mi);
    RECT work = mi.rcWork;
    int gapPx = Px(10);
    int pad = Px(12), headerH = Px(44), cellW = Px(84), cellH = Px(82);
    int maxCols = std::max(1, (int)(work.right - work.left - gapPx * 2 - pad * 2) / cellW);
    int maxRows = std::max(1, (int)(work.bottom - work.top - gapPx * 2 - headerH - pad) / cellH);

    // Items + icons. Opening the same, unchanged folder again reuses them.
    int iconPx = Px(40);
    ULONGLONG stamp = 0;
    auto paths = ListFolder(g_popup.dir, &stamp);
    if (paths.size() > (size_t)maxCols * maxRows) {
        paths.resize((size_t)maxCols * maxRows);  // the rest can't be shown
    }
    bool reuse = g_popup.itemsDir == g_popup.dir && g_popup.itemsStamp == stamp &&
                 g_popup.iconPx == iconPx && g_popup.items.size() == paths.size();
    for (size_t i = 0; reuse && i < paths.size(); i++) {
        reuse = g_popup.items[i].bmp && g_popup.items[i].path == paths[i];
    }
    if (!reuse) {
        PopupFreeItems();
        g_popup.iconPx = iconPx;
        for (auto& p : paths) {
            if (g_unloading) {
                return;
            }
            PopupItem it;
            it.path = p;
            it.label = DisplayNameOf(p);
            IconPixels px;
            if (GetIconPixels(p, iconPx, px)) {
                it.bmp = MakeDib(px);
                it.bw = px.w;
                it.bh = px.h;
            }
            g_popup.items.push_back(std::move(it));
        }
        g_popup.itemsDir = g_popup.dir;
        g_popup.itemsStamp = stamp;
    }

    // If the taskbar preview is out of date (files changed in Explorer),
    // refresh it.
    {
        std::vector<std::wstring> first(paths.begin(),
                                        paths.begin() + std::min(kPreviewCount, paths.size()));
        bool differs;
        {
            std::lock_guard<std::mutex> lock(g_previewMutex);
            differs = folder >= (int)g_previews.size() || g_previews[folder].paths != first;
        }
        if (differs) {
            QueuePreviewRefresh();
        }
    }

    // Layout. A folder with too many rows for the screen gets more columns.
    int n = (int)g_popup.items.size();
    int cols = std::max(1, std::min({s.columns, maxCols, std::max(n, 1)}));
    int rows = n ? (n + cols - 1) / cols : 1;
    if (rows > maxRows) {
        cols = std::min(maxCols, (n + maxRows - 1) / maxRows);
        rows = (n + cols - 1) / cols;
    }
    int w = std::max(pad * 2 + cols * cellW, Px(300));
    int h = headerH + rows * cellH + pad;

    HDC dc = GetDC(g_popupWnd);
    HFONT old = (HFONT)SelectObject(dc, g_popup.font);
    SIZE szOpen{}, szAdd{};
    GetTextExtentPoint32W(dc, L"Open folder", 11, &szOpen);
    GetTextExtentPoint32W(dc, L"+ Add...", 8, &szAdd);
    SelectObject(dc, old);
    ReleaseDC(g_popupWnd, dc);

    int btnPadX = Px(10);
    int btnTop = Px(10), btnBottom = headerH - Px(6);
    g_popup.rcOpen = {w - pad - szOpen.cx - btnPadX * 2, btnTop, w - pad, btnBottom};
    g_popup.rcAdd = {g_popup.rcOpen.left - Px(6) - szAdd.cx - btnPadX * 2, btnTop,
                     g_popup.rcOpen.left - Px(6), btnBottom};

    int startX = (w - cols * cellW) / 2;
    for (int i = 0; i < n; i++) {
        int c = i % cols, r = i / cols;
        g_popup.items[i].rc = {startX + c * cellW, headerH + r * cellH,
                               startX + (c + 1) * cellW, headerH + (r + 1) * cellH};
    }
    g_popup.rcEmpty = {pad, headerH, w - pad, headerH + cellH};

    // Position above (or below) the taskbar, centered on the click. Beside
    // it if the taskbar is on the left or right side of the screen.
    int x = anchor.x - w / 2;
    x = std::max((int)work.left + gapPx, std::min(x, (int)work.right - w - gapPx));
    int yCentered = anchor.y - h / 2;
    yCentered = std::max((int)work.top + gapPx, std::min(yCentered, (int)work.bottom - h - gapPx));
    int y;
    if (anchor.y >= work.bottom) {
        y = work.bottom - h - gapPx;  // taskbar at the bottom
    } else if (anchor.y < work.top) {
        y = work.top + gapPx;  // taskbar at the top
    } else if (anchor.x < work.left) {
        x = work.left + gapPx;  // taskbar on the left
        y = yCentered;
    } else if (anchor.x >= work.right) {
        x = work.right - w - gapPx;  // taskbar on the right
        y = yCentered;
    } else {
        y = anchor.y - h - Px(30);  // auto-hide taskbar
        y = std::max((int)work.top + gapPx, y);
    }

    BOOL dark = !g_popup.light;
    DwmSetWindowAttribute(g_popupWnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
    int corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(g_popupWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));

    SetWindowPos(g_popupWnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    InvalidateRect(g_popupWnd, nullptr, TRUE);
    SetForegroundWindow(g_popupWnd);
}

int PopupHitTest(POINT pt) {
    if (PtInRect(&g_popup.rcAdd, pt)) return kHitAdd;
    if (PtInRect(&g_popup.rcOpen, pt)) return kHitOpen;
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        if (PtInRect(&g_popup.items[i].rc, pt)) return i;
    }
    return kHitNone;
}

void FillRound(HDC dc, RECT rc, COLORREF color, int radius) {
    HBRUSH br = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ ob = SelectObject(dc, br), op = SelectObject(dc, pen);
    RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(br);
    DeleteObject(pen);
}

void PopupPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);

    HDC dc = CreateCompatibleDC(hdc);
    HBITMAP buf = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    HGDIOBJ oldBuf = SelectObject(dc, buf);

    bool light = g_popup.light;
    COLORREF bg = light ? RGB(249, 249, 249) : RGB(32, 32, 32);
    COLORREF hoverBg = light ? RGB(232, 232, 232) : RGB(55, 55, 55);
    COLORREF btnBg = light ? RGB(238, 238, 238) : RGB(45, 45, 45);
    COLORREF text = light ? RGB(25, 25, 25) : RGB(240, 240, 240);
    COLORREF dim = light ? RGB(95, 95, 95) : RGB(165, 165, 165);

    HBRUSH bgBrush = CreateSolidBrush(bg);
    FillRect(dc, &rc, bgBrush);
    DeleteObject(bgBrush);
    SetBkMode(dc, TRANSPARENT);

    // Title.
    SelectObject(dc, g_popup.fontTitle);
    SetTextColor(dc, text);
    RECT rt = {Px(16), 0, g_popup.rcAdd.left - Px(8), Px(44)};
    DrawTextW(dc, g_popup.title.c_str(), -1, &rt,
              DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);

    // Header buttons.
    SelectObject(dc, g_popup.font);
    struct {
        RECT rc;
        int id;
        PCWSTR label;
    } buttons[] = {{g_popup.rcAdd, kHitAdd, L"+ Add..."}, {g_popup.rcOpen, kHitOpen, L"Open folder"}};
    for (auto& b : buttons) {
        FillRound(dc, b.rc, g_popup.hover == b.id ? hoverBg : btnBg, Px(8));
        SetTextColor(dc, text);
        RECT r = b.rc;
        DrawTextW(dc, b.label, -1, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
    }

    // Items.
    HDC iconDc = CreateCompatibleDC(dc);
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        auto& it = g_popup.items[i];
        if (g_popup.hover == i) {
            RECT hr = it.rc;
            InflateRect(&hr, -Px(3), -Px(3));
            FillRound(dc, hr, hoverBg, Px(10));
        }
        int iconTop = it.rc.top + Px(10);
        if (it.bmp) {
            HGDIOBJ ob = SelectObject(iconDc, it.bmp);
            int ix = (it.rc.left + it.rc.right - g_popup.iconPx) / 2;
            BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            AlphaBlend(dc, ix, iconTop, g_popup.iconPx, g_popup.iconPx, iconDc, 0, 0, it.bw, it.bh, bf);
            SelectObject(iconDc, ob);
        }
        SetTextColor(dc, text);
        RECT lr = {it.rc.left + Px(4), iconTop + g_popup.iconPx + Px(6), it.rc.right - Px(4),
                   it.rc.bottom - Px(4)};
        DrawTextW(dc, it.label.c_str(), -1, &lr,
                  DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
    DeleteDC(iconDc);

    if (g_popup.items.empty()) {
        SetTextColor(dc, dim);
        RECT er = g_popup.rcEmpty;
        DrawTextW(dc, L"Empty. Click \"+ Add...\" or \"Open folder\" and drop shortcuts there.", -1,
                  &er, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBuf);
    DeleteObject(buf);
    DeleteDC(dc);
    EndPaint(hWnd, &ps);
}

std::wstring UniquePath(const std::wstring& dir, const std::wstring& base, const std::wstring& ext) {
    std::wstring path = dir + L"\\" + base + ext;
    for (int i = 2; GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES && i < 1000; i++) {
        path = dir + L"\\" + base + L" (" + std::to_wstring(i) + L")" + ext;
    }
    return path;
}

void AddToFolder(const std::wstring& dir, const std::wstring& target) {
    std::wstring fileName = PathFindFileNameW(target.c_str());
    PCWSTR ext = PathFindExtensionW(fileName.c_str());
    std::wstring base(fileName.c_str(), ext);

    if (IsShortcutExt(target.c_str())) {
        CopyFileW(target.c_str(), UniquePath(dir, base, ext).c_str(), TRUE);
        return;
    }

    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put())))) {
        return;
    }
    link->SetPath(target.c_str());
    std::wstring workDir = target;
    PathRemoveFileSpecW(workDir.data());
    workDir.resize(wcslen(workDir.c_str()));
    link->SetWorkingDirectory(workDir.c_str());
    if (auto file = link.try_as<IPersistFile>()) {
        file->Save(UniquePath(dir, base.empty() ? fileName : base, L".lnk").c_str(), TRUE);
    }
}

void PopupAddFiles() {
    int folder = g_popup.folder;
    POINT anchor = g_popup.anchor;
    std::wstring dir = g_popup.dir;
    std::wstring title = g_popup.title;
    PopupHide();

    winrt::com_ptr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return;
    }
    DWORD opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_ALLOWMULTISELECT | FOS_NODEREFERENCELINKS | FOS_FILEMUSTEXIST |
                    FOS_FORCEFILESYSTEM);
    dlg->SetTitle((L"Add to \"" + title + L"\"").c_str());

    winrt::com_ptr<IShellItem> startFolder;
    std::wstring programs = ExpandEnv(L"%ProgramData%\\Microsoft\\Windows\\Start Menu\\Programs");
    if (SUCCEEDED(SHCreateItemFromParsingName(programs.c_str(), nullptr,
                                              IID_PPV_ARGS(startFolder.put())))) {
        dlg->SetFolder(startFolder.get());
    }

    if (SUCCEEDED(dlg->Show(nullptr))) {
        winrt::com_ptr<IShellItemArray> results;
        if (SUCCEEDED(dlg->GetResults(results.put()))) {
            DWORD count = 0;
            results->GetCount(&count);
            if (count) {
                EnsureFolderExists(dir);
            }
            for (DWORD i = 0; i < count; i++) {
                winrt::com_ptr<IShellItem> item;
                PWSTR path = nullptr;
                if (SUCCEEDED(results->GetItemAt(i, item.put())) &&
                    SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                    AddToFolder(dir, path);
                    CoTaskMemFree(path);
                }
            }
        }
    }

    // PopupShow also refreshes the taskbar preview if the first items changed.
    PopupShow(folder, anchor, true);
}

void PopupItemMenu(int index, POINT screenPt) {
    if (index < 0 || index >= (int)g_popup.items.size()) {
        return;
    }
    std::wstring path = g_popup.items[index].path;
    // A shortcut is only removed from the mini folder. Anything else is the
    // real file or folder, so the menu says what happens to it.
    bool shortcut = IsShortcutExt(path.c_str());

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, L"Open");
    AppendMenuW(menu, MF_STRING, 2, L"Open file location");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 3, shortcut ? L"Remove from this folder" : L"Delete");
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screenPt.x, screenPt.y, 0,
                             g_popupWnd, nullptr);
    DestroyMenu(menu);

    if (cmd == 1) {
        PopupHide();
        ShellExecuteW(nullptr, nullptr, path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == 2) {
        PopupHide();
        // For shortcuts, show the target; otherwise the item itself.
        std::wstring target = path;
        if (_wcsicmp(PathFindExtensionW(path.c_str()), L".lnk") == 0) {
            winrt::com_ptr<IShellLinkW> link;
            if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                           IID_PPV_ARGS(link.put())))) {
                if (auto file = link.try_as<IPersistFile>();
                    file && SUCCEEDED(file->Load(path.c_str(), STGM_READ))) {
                    WCHAR buf[MAX_PATH] = L"";
                    if (SUCCEEDED(link->GetPath(buf, MAX_PATH, nullptr, 0)) && *buf) {
                        target = buf;
                    }
                }
            }
        }
        if (PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(target.c_str())) {
            SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
            ILFree(pidl);
        }
    } else if (cmd == 3) {
        int folder = g_popup.folder;
        POINT anchor = g_popup.anchor;
        std::wstring from = path;
        from.push_back(L'\0');  // double null terminated
        SHFILEOPSTRUCTW op{};
        op.hwnd = g_popupWnd;
        op.wFunc = FO_DELETE;
        op.pFrom = from.c_str();
        // Recycle Bin; ask first unless it's just a shortcut. Always ask when
        // the item can't be recycled and would be deleted permanently.
        op.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_WANTNUKEWARNING |
                    (shortcut ? FOF_NOCONFIRMATION : 0);
        SHFileOperationW(&op);
        PopupShow(folder, anchor, true);
    }
}

LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_SHOW_FOLDER:
            PopupShow((int)wParam, {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, false);
            return 0;

        case WM_APP_REFRESH_PREVIEWS:
            RefreshPreviews();
            return 0;

        case WM_APP_HIDE:
            PopupHide();
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                PopupHide();
            }
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PopupHide();
                return 0;
            }
            break;

        case WM_MOUSEMOVE: {
            if (!g_popup.trackingLeave) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
                TrackMouseEvent(&tme);
                g_popup.trackingLeave = true;
            }
            int hit = PopupHitTest({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (hit != g_popup.hover) {
                g_popup.hover = hit;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_popup.trackingLeave = false;
            if (g_popup.hover != kHitNone) {
                g_popup.hover = kHitNone;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP: {
            int hit = PopupHitTest({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (hit == kHitAdd) {
                PopupAddFiles();
            } else if (hit == kHitOpen) {
                std::wstring dir = g_popup.dir;
                PopupHide();
                EnsureFolderExists(dir);
                ShellExecuteW(nullptr, L"open", dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            } else if (hit >= 0 && hit < (int)g_popup.items.size()) {
                std::wstring path = g_popup.items[hit].path;
                PopupHide();
                SHELLEXECUTEINFOW sei{sizeof(sei)};
                sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
                sei.lpFile = path.c_str();
                sei.nShow = SW_SHOWNORMAL;
                // No second try when the user cancelled (e.g. a UAC prompt).
                if (!ShellExecuteExW(&sei) && GetLastError() != ERROR_CANCELLED) {
                    ShellExecuteW(nullptr, nullptr, path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                }
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hit = PopupHitTest(pt);
            ClientToScreen(hWnd, &pt);
            PopupItemMenu(hit, pt);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PopupPaint(hWnd);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PopupFreeItems();
            if (g_popup.font) DeleteObject(g_popup.font);
            if (g_popup.fontTitle) DeleteObject(g_popup.fontTitle);
            g_popup.font = g_popup.fontTitle = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI PopupThreadProc(LPVOID readyEvent) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // The class belongs to the mod's own module, not to explorer.exe, so it
    // can never be mixed up with a class left by another instance.
    HMODULE modModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)PopupThreadProc, &modModule);

    WNDCLASSEXW wc{sizeof(wc)};
    wc.style = CS_DROPSHADOW;
    wc.lpfnWndProc = PopupWndProc;
    wc.hInstance = modModule;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"WhTaskbarMiniFoldersPopup";
    bool registered = modModule && RegisterClassExW(&wc);
    if (registered) {
        g_popupWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wc.lpszClassName,
                                     L"Mini folder", WS_POPUP, 0, 0, 100, 100, nullptr, nullptr,
                                     wc.hInstance, nullptr);
    } else {
        Wh_Log(L"Popup window class not registered: %u", GetLastError());
    }
    SetEvent((HANDLE)readyEvent);

    if (g_popupWnd) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_popupWnd = nullptr;
    if (registered) {
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }
    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    return 0;
}

void StartPopupThread() {
    HANDLE ready = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_popupThread = CreateThread(nullptr, 0, PopupThreadProc, ready, 0, nullptr);
    if (g_popupThread) {
        WaitForSingleObject(ready, 5000);
    }
    CloseHandle(ready);
    QueuePreviewRefresh();
}

void StopPopupThread() {
    if (g_popupWnd) {
        PostMessage(g_popupWnd, WM_CLOSE, 0, 0);
    }
    if (g_popupThread) {
        // No time limit: the mod must not be unloaded while this thread still
        // runs its code. The thread never waits for the caller, and its slow
        // loops stop early once g_unloading is set.
        WaitForSingleObject(g_popupThread, INFINITE);
        CloseHandle(g_popupThread);
        g_popupThread = nullptr;
    }
}

// ============================================================================
//  Hooking Taskbar.View.dll
// ============================================================================

std::atomic<bool> g_taskbarViewHooked;

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
            true,  // optional
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,  // optional
        },
    };
    return WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                      ARRAYSIZE(taskbarViewHooks));
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewHooked && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}

// Makes the taskbar lay itself out again, which calls MeasureOverride.
void TriggerTaskbarLayout() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    }
}

// ============================================================================
//  Windhawk entry points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewHooked = true;
        if (!HookTaskbarViewSymbols(module)) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(kernelBase, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    StartPopupThread();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewHooked) {
        if (HMODULE module = GetTaskbarViewModuleHandle();
            module && !g_taskbarViewHooked.exchange(true)) {
            if (HookTaskbarViewSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    TriggerTaskbarLayout();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (HWND ui = GetTaskbarUiWnd()) {
        RunFromWindowThread(ui, [](PVOID) { RemoveAllPanels(); }, nullptr);
    }

    // No time limit: a callback that is still queued on the taskbar thread
    // points into this mod. The taskbar thread never waits for this thread.
    while (g_pendingDispatch > 0) {
        Sleep(20);
    }

    StopPopupThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_popupWnd) {
        PostMessage(g_popupWnd, WM_APP_HIDE, 0, 0);
    }

    RequestRebuild();

    // A frame that had no folders before (nothing injected) needs a new pass.
    if (HWND ui = GetTaskbarUiWnd()) {
        RunFromWindowThread(ui, [](PVOID) {
            for (auto& s : g_frames) {
                s.injected = false;
                s.attempts = 0;
            }
            g_backupTriggerNeeded = true;
        }, nullptr);
    }
    TriggerTaskbarLayout();

    // The folder list may have changed: read the previews again.
    QueuePreviewRefresh();
}
