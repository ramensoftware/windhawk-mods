// ==WindhawkMod==
// @id              desktop-shortcut-groups
// @name            Desktop Shortcut Groups
// @description     Group desktop shortcuts into phone-style folders: drag one shortcut onto another with Ctrl+Alt, then open the group in a panel
// @version         1.0.0
// @author          skylord
// @github          https://github.com/skylords-forge
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lgdi32 -ldwmapi -ladvapi32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Shortcut Groups

Group your desktop shortcuts the way phones group apps. Drag one shortcut onto
another while holding **Ctrl+Alt**, and both move into a new group. The group's
icon shows a preview of the shortcuts inside it, and clicking it opens them in
a panel.

![Groups on the desktop](https://raw.githubusercontent.com/skylords-forge/desktop-shortcut-groups/main/screenshots/main.gif)

## Recommended desktop setting

Turn off **Auto arrange icons** (right-click the desktop → **View**). With
auto arrange on, Windows rearranges the icons itself, so a new group isn't
placed where the target shortcut was and shortcuts moved out of a group may
not land where you expect.

## Usage

### Create a group

Start dragging a shortcut, hold **Ctrl+Alt**, and drop it on another
shortcut. The key is only needed to create a group: from then on, plain drag
and drop is enough.

### Add shortcuts to a group

Drag a shortcut onto the group, no key needed. Anything else dropped on a
group the usual way (for example a file copied with Ctrl) is picked up as
well, and the group's icon is updated.

### Open a group

Click the group to show its shortcuts in a panel, and click a shortcut to
launch it. Click outside the panel or press **Esc** to close it. The arrow
keys and **Enter** work too.

By default, the panel opens in the center of the screen and dims the
background. It can also open right next to the group (see the settings).

### Reorder a group

Drag the shortcuts around in the panel. The group's icon is updated when the
order of the shortcuts it shows changes. Dropping outside the panel or
pressing **Esc** cancels.

### Remove a shortcut from a group

Right-click it in the panel and choose **Remove from group**. It moves back to
the first free spot on the desktop. The rest of the menu is the usual Windows
context menu (Open file location, Properties, Delete, and so on).

When only one shortcut is left in a group, it moves back to where the group
was and the group is deleted. This can be turned off in the settings.

### Other actions

- **Open a group as a regular folder:** Shift+double-click it, use the
  **Open folder** button in the panel, or choose Open from its context menu.
- **Rename, delete or copy a group:** since a click opens the panel,
  right-click the group and use **Rename**, **Delete** or **Copy** from its
  menu, like for any other folder. You can also select it by right-clicking
  it or by dragging a selection rectangle, then press F2, Delete or Ctrl+C.
  Deleting a group sends its shortcuts to the Recycle Bin with it.

## The group icon

The icon shows up to 4 shortcuts in a 2×2 grid. Groups with more than 4
shortcuts switch to a 3×3 grid of up to 9 (can be turned off). Square app
icons get rounded corners so that they fit the rounded background, which
follows the Windows light/dark theme by default.

## How it works

A group is a regular folder on your desktop that holds your shortcuts. To
make it a group, the mod adds a few things to the folder:

- A hidden `desktop.ini` file. It marks the folder as a group, stores the
  order of the shortcuts, and points to the group icon.
- A hidden `groupicon-*.ico` file: the generated preview icon. It's replaced
  by a new file whenever the icon changes (a new name makes Windows show the
  new icon right away), and the previous file is deleted on the next change.
- The folder's read-only (or system) attribute. Windows only uses the custom
  icon from `desktop.ini` for folders that have it; it doesn't prevent
  changing the folder's content.

These are standard Windows folder customizations (the same mechanism as
Properties → Customize → Change Icon). The mod updates them when a group
changes, and when the mod loads, the theme changes or the settings change,
only for groups whose icon is out of date.

**After the mod is disabled or uninstalled**, these files stay in each group
folder on purpose: your groups remain ordinary folders that keep their
preview icon and the order of their shortcuts, and open in File Explorer like
any other folder. Enabling the mod again turns them back into groups. To
turn a group into a completely plain folder, delete its `desktop.ini` and
`groupicon-*.ico` files (shown with "Show hidden files" in File Explorer) and
clear its read-only attribute, or move its shortcuts out and delete it.

## Settings

- **Grouping key:** Ctrl+Alt (default) or Shift+Alt. The keys must match
  exactly: with Ctrl+Alt, Ctrl+Shift+Alt doesn't group. Plain Ctrl, Shift and
  Ctrl+Shift are left alone because Windows already uses them (copy, move,
  create shortcut).
- **Open with:** single click (default), double-click, or the File Explorer
  setting. Applies to groups on the desktop and shortcuts in the panel.
- **Panel position:** center of the screen (default) or near the group.
- **Background dimming:** 0–100%, default 50%. Only for the centered panel.
- **Open groups in a panel:** turn off to open groups as regular folders.
- **Dissolve groups with one shortcut**, **3×3 grid**, **group icon
  background**, **default group name**.

## Limitations

- Only `.lnk` and `.url` shortcuts can be grouped with the grouping key.
- Shortcuts on the shared (Public) desktop need administrator rights to be
  moved, so they can't be grouped.
- Changes made inside a group folder by other means (e.g. in a File Explorer
  window) are picked up when the group's panel is opened or the mod loads.
- If File Explorer itself is set to open items with a single click, groups
  open with a single click even when **Open with** is set to double-click.
- Ctrl+Z doesn't undo grouping.
- With "Auto arrange icons" enabled, a new group isn't placed where the
  target shortcut was.
- The panel's blurred background requires Windows 11 22H2 or newer; older
  versions get a plain background.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- groupName: Group
  $name: Default group name
  $description: Base name for new group folders. A number is appended when the name is already taken.
- modifierKey: ctrlAlt
  $name: Grouping key
  $description: Key combination to hold while dropping a shortcut onto another shortcut or group.
  $options:
  - ctrlAlt: Ctrl+Alt
  - shiftAlt: Shift+Alt
- openInPanel: true
  $name: Open groups in a panel
  $description: Opening a group shows its shortcuts in a panel instead of opening the folder. Shift+double-click always opens the folder.
- openWith: single
  $name: Open with
  $description: How groups on the desktop and shortcuts in the panel are opened. Dragging and right-clicking work the same either way.
  $options:
  - double: Double-click
  - single: Single click
  - system: Follow Windows setting
- panelPosition: center
  $name: Panel position
  $description: Where a group's panel opens. Near the group, it opens below the group, or above or beside it when there's no room.
  $options:
  - center: Center of the screen
  - near: Near the group
- dimming: 50
  $name: Background dimming (%)
  $description: How much the screen behind the panel is darkened when it opens in the center of the screen, from 0 (not at all) to 100 (black). Not applied when the panel opens near the group.
- dissolveSingleItem: true
  $name: Dissolve groups with one shortcut
  $description: When only one shortcut is left in a group, move it back to where the group was and delete the group.
- preview3x3: true
  $name: Switch to a 3×3 grid when a group has more than 4 shortcuts
  $description: When off, group icons always show a 2×2 grid of up to 4 shortcuts.
- previewTheme: system
  $name: Group icon background
  $options:
  - system: Windows theme
  - light: Light
  - dark: Dark
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

// The mod's own module, used to register the panel window classes.
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Constants

constexpr wchar_t kIniSection[] = L"WindhawkDesktopGroups";
constexpr wchar_t kIconPrefix[] = L"groupicon-";
constexpr wchar_t kDropTargetProp[] = L"OleDropTargetInterface";

constexpr UINT_PTR kPositionTimerId = 0x57484447;
constexpr UINT kPositionTimerIntervalMs = 150;
constexpr int kPositionMaxAttempts = 20;
constexpr int kInstallMaxAttempts = 30;
constexpr DWORD kWatchIntervalMs = 1000;
constexpr DWORD kWatchInstalledIntervalMs = 5000;
constexpr DWORD kWatchOtherProcessIntervalMs = 30000;

// Icon sizes written to each group icon file.
constexpr int kIconSizes[] = {256, 96, 64, 48, 32, 16};

// {4C96BE40-915C-11CF-99D3-00AA004AE837}
constexpr GUID kSidSTopLevelBrowser = {
    0x4C96BE40,
    0x915C,
    0x11CF,
    {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class ClickMode { Double, Single, System };
enum class PreviewTheme { System, Light, Dark };
enum class PanelPosition { Center, Near };

struct Settings {
    std::wstring groupName = L"Group";
    PreviewTheme previewTheme = PreviewTheme::System;
    // Whether groups with more than 4 shortcuts get a 3x3 preview grid.
    bool preview3x3 = true;
    // Exact modifier state (MK_* flags) that triggers grouping.
    DWORD modifierMask = MK_CONTROL | MK_ALT;
    bool openInPanel = true;
    ClickMode clickMode = ClickMode::Single;
    bool dissolveSingleItem = true;
    PanelPosition panelPosition = PanelPosition::Center;
    // Background dimming behind a centered panel, in percent.
    int dimming = 50;
};

// Modifier flags considered when matching the grouping key.
constexpr DWORD kModifierFlags = MK_CONTROL | MK_SHIFT | MK_ALT;

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring SanitizeFileName(const std::wstring& name) {
    std::wstring result;
    for (wchar_t c : name) {
        if (c < 32 || wcschr(L"\\/:*?\"<>|", c)) {
            continue;
        }
        result += c;
    }
    while (!result.empty() &&
           (result.back() == L' ' || result.back() == L'.')) {
        result.pop_back();
    }
    while (!result.empty() && result.front() == L' ') {
        result.erase(result.begin());
    }
    return result.empty() ? L"Group" : result;
}

// Whether a string setting has the given value.
bool SettingIs(PCWSTR name, PCWSTR value) {
    return wcscmp(WindhawkUtils::StringSetting::make(name).get(), value) == 0;
}

void LoadSettings() {
    Settings settings;

    settings.groupName = SanitizeFileName(
        WindhawkUtils::StringSetting::make(L"groupName").get());

    if (SettingIs(L"previewTheme", L"light")) {
        settings.previewTheme = PreviewTheme::Light;
    } else if (SettingIs(L"previewTheme", L"dark")) {
        settings.previewTheme = PreviewTheme::Dark;
    } else {
        settings.previewTheme = PreviewTheme::System;
    }

    settings.preview3x3 = Wh_GetIntSetting(L"preview3x3") != 0;

    settings.modifierMask = SettingIs(L"modifierKey", L"shiftAlt")
                                ? MK_SHIFT | MK_ALT
                                : MK_CONTROL | MK_ALT;

    settings.openInPanel = Wh_GetIntSetting(L"openInPanel") != 0;

    if (SettingIs(L"openWith", L"double")) {
        settings.clickMode = ClickMode::Double;
    } else if (SettingIs(L"openWith", L"system")) {
        settings.clickMode = ClickMode::System;
    } else {
        settings.clickMode = ClickMode::Single;
    }

    settings.dissolveSingleItem =
        Wh_GetIntSetting(L"dissolveSingleItem") != 0;

    settings.panelPosition = SettingIs(L"panelPosition", L"near")
                                 ? PanelPosition::Near
                                 : PanelPosition::Center;

    settings.dimming = std::clamp(Wh_GetIntSetting(L"dimming"), 0, 100);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = settings;
}

bool IsAppsDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
            &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

// Whether group icons get the dark background.
bool IsPreviewDark(const Settings& settings) {
    switch (settings.previewTheme) {
        case PreviewTheme::Light:
            return false;
        case PreviewTheme::Dark:
            return true;
        case PreviewTheme::System:
            break;
    }
    return IsAppsDarkMode();
}

////////////////////////////////////////////////////////////////////////////////
// Global state

class GroupingDropTarget;

struct PendingPosition {
    std::wstring path;
    POINT point;
    int attempts;
};

UINT g_msgInstall;
UINT g_msgUninstall;
UINT g_msgRefreshIcons;
HANDLE g_stopEvent;
HANDLE g_watchThread;

// Shared between the watch thread and the desktop thread.
std::atomic<HWND> g_defView{nullptr};
std::atomic<bool> g_installed{false};

// Accessed only on the desktop thread.
HWND g_listView;
HWND g_dropHwnd;
GroupingDropTarget* g_wrapper;
IFolderView* g_folderView;
std::wstring g_userDesktop;
std::wstring g_publicDesktop;
std::vector<PendingPosition> g_pendingPositions;
int g_installAttempts;

// Calls in progress on the desktop thread that can run a modal loop (a
// dialog, a menu, drag and drop). The mod must not be unloaded while one is
// running: returning into unloaded code would crash Explorer.
std::atomic<int> g_busyCalls{0};
// Set while unloading, so that no new such call starts.
std::atomic<bool> g_unloading{false};

class BusyScope {
   public:
    BusyScope() { g_busyCalls++; }
    ~BusyScope() { g_busyCalls--; }
    BusyScope(const BusyScope&) = delete;
    BusyScope& operator=(const BusyScope&) = delete;
};

// Defined further below.
void ScheduleGroupSync(const std::wstring& folder);
void NotifyMoved(const std::wstring& source, const std::wstring& destination);
std::wstring GetViewItemPath(int index);

////////////////////////////////////////////////////////////////////////////////
// Path helpers

bool EqualsNoCase(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) ==
           CSTR_EQUAL;
}

bool LessNoCase(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) ==
           CSTR_LESS_THAN;
}

std::wstring ParentDir(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? std::wstring() : path.substr(0, pos);
}

std::wstring FileNameOf(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

bool PathExists(const std::wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool HasShortcutExtension(const std::wstring& path) {
    PCWSTR ext = PathFindExtensionW(path.c_str());
    return _wcsicmp(ext, L".lnk") == 0 || _wcsicmp(ext, L".url") == 0;
}

bool IsInDesktopDir(const std::wstring& path) {
    std::wstring parent = ParentDir(path);
    return (!g_userDesktop.empty() && EqualsNoCase(parent, g_userDesktop)) ||
           (!g_publicDesktop.empty() && EqualsNoCase(parent, g_publicDesktop));
}

std::wstring GetKnownFolder(REFKNOWNFOLDERID id) {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path)) && path) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

// Returns "dir\stem.ext", or "dir\stem (N).ext" if that already exists.
std::wstring UniqueFilePath(const std::wstring& dir,
                            const std::wstring& fileName) {
    PCWSTR extPtr = PathFindExtensionW(fileName.c_str());
    std::wstring ext = extPtr;
    std::wstring stem = fileName.substr(0, fileName.size() - ext.size());

    std::wstring candidate = dir + L"\\" + fileName;
    for (int i = 2; PathExists(candidate); i++) {
        candidate = dir + L"\\" + stem + L" (" + std::to_wstring(i) + L")" +
                    ext;
    }
    return candidate;
}

// Returns "dir\base", or "dir\base N" if that already exists.
std::wstring UniqueFolderPath(const std::wstring& dir,
                              const std::wstring& base) {
    std::wstring candidate = dir + L"\\" + base;
    for (int i = 2; PathExists(candidate); i++) {
        candidate = dir + L"\\" + base + L" " + std::to_wstring(i);
    }
    return candidate;
}

////////////////////////////////////////////////////////////////////////////////
// desktop.ini helpers

std::wstring IniPathOf(const std::wstring& folder) {
    return folder + L"\\desktop.ini";
}

// Creates an empty UTF-16 desktop.ini so that the profile APIs write Unicode.
void EnsureUnicodeIni(const std::wstring& ini) {
    if (PathExists(ini)) {
        return;
    }

    HANDLE file = CreateFileW(ini.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    const WORD bom = 0xFEFF;
    DWORD written;
    WriteFile(file, &bom, sizeof(bom), &written, nullptr);
    CloseHandle(file);
}

bool WriteIniValue(const std::wstring& ini,
                   PCWSTR section,
                   PCWSTR key,
                   const std::wstring& value) {
    // Hidden/system files can fail to be rewritten, so clear the attributes
    // temporarily.
    SetFileAttributesW(ini.c_str(), FILE_ATTRIBUTE_NORMAL);
    BOOL ok =
        WritePrivateProfileStringW(section, key, value.c_str(), ini.c_str());
    WritePrivateProfileStringW(nullptr, nullptr, nullptr, ini.c_str());
    SetFileAttributesW(ini.c_str(),
                       FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
    return ok;
}

void DeleteIniValue(const std::wstring& ini, PCWSTR section, PCWSTR key) {
    SetFileAttributesW(ini.c_str(), FILE_ATTRIBUTE_NORMAL);
    WritePrivateProfileStringW(section, key, nullptr, ini.c_str());
    WritePrivateProfileStringW(nullptr, nullptr, nullptr, ini.c_str());
    SetFileAttributesW(ini.c_str(),
                       FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

std::wstring ReadIniValue(const std::wstring& ini, PCWSTR section, PCWSTR key) {
    wchar_t buffer[MAX_PATH * 2] = L"";
    GetPrivateProfileStringW(section, key, L"", buffer, ARRAYSIZE(buffer),
                             ini.c_str());
    return buffer;
}

bool IsGroupFolder(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES ||
        !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    return GetPrivateProfileIntW(kIniSection, L"IsGroup", 0,
                                 IniPathOf(path).c_str()) == 1;
}

std::vector<std::wstring> ReadOrder(const std::wstring& ini) {
    std::vector<wchar_t> buffer(32768);
    GetPrivateProfileStringW(kIniSection, L"Order", L"", buffer.data(),
                             static_cast<DWORD>(buffer.size()), ini.c_str());

    std::vector<std::wstring> result;
    std::wstring current;
    for (const wchar_t* p = buffer.data(); *p; p++) {
        if (*p == L'|') {
            if (!current.empty()) {
                result.push_back(current);
            }
            current.clear();
        } else {
            current += *p;
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

std::wstring JoinOrder(const std::vector<std::wstring>& names) {
    std::wstring result;
    for (const auto& name : names) {
        if (!result.empty()) {
            result += L'|';
        }
        result += name;
    }
    return result;
}

// Lists the visible entries of a group folder (skips desktop.ini, icons, and
// other hidden/system files).
std::vector<std::wstring> ListGroupEntries(const std::wstring& folder) {
    std::vector<std::wstring> result;

    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((folder + L"\\*").c_str(), FindExInfoBasic,
                                   &data, FindExSearchNameMatch, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) {
        return result;
    }

    do {
        if (wcscmp(data.cFileName, L".") == 0 ||
            wcscmp(data.cFileName, L"..") == 0) {
            continue;
        }
        if (data.dwFileAttributes &
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }
        result.push_back(data.cFileName);
    } while (FindNextFileW(find, &data));

    FindClose(find);

    std::sort(result.begin(), result.end(), LessNoCase);
    return result;
}

// Keeps the stored order for entries that still exist and appends new
// entries alphabetically.
std::vector<std::wstring> BuildOrder(const std::wstring& folder) {
    std::vector<std::wstring> entries = ListGroupEntries(folder);
    std::vector<std::wstring> result;

    for (const auto& name : ReadOrder(IniPathOf(folder))) {
        auto it = std::find_if(
            entries.begin(), entries.end(),
            [&](const std::wstring& entry) { return EqualsNoCase(entry, name); });
        if (it != entries.end()) {
            result.push_back(*it);
            entries.erase(it);
        }
    }

    result.insert(result.end(), entries.begin(), entries.end());
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Image helpers. Pixels are premultiplied BGRA, top-down.

struct Image {
    int width = 0;
    int height = 0;
    std::vector<uint32_t> pixels;
};

inline uint32_t PackPixel(uint32_t b, uint32_t g, uint32_t r, uint32_t a) {
    return (b & 0xFF) | ((g & 0xFF) << 8) | ((r & 0xFF) << 16) |
           ((a & 0xFF) << 24);
}

bool LoadItemImage(const std::wstring& path, int size, Image& image) {
    IShellItemImageFactory* factory = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(&factory)))) {
        return false;
    }

    HBITMAP bitmap = nullptr;
    SIZE requested = {size, size};
    HRESULT hr = factory->GetImage(requested, SIIGBF_ICONONLY, &bitmap);
    factory->Release();
    if (FAILED(hr) || !bitmap) {
        return false;
    }

    BITMAP bm = {};
    GetObjectW(bitmap, sizeof(bm), &bm);
    if (bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        DeleteObject(bitmap);
        return false;
    }

    image.width = bm.bmWidth;
    image.height = bm.bmHeight;
    image.pixels.assign(static_cast<size_t>(image.width) * image.height, 0);

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = image.width;
    info.bmiHeader.biHeight = -image.height;  // Top-down.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, bitmap, 0, image.height, image.pixels.data(),
                          &info, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(bitmap);

    if (lines != image.height) {
        return false;
    }

    // Normalize to premultiplied alpha.
    bool anyAlpha = false;
    bool straightAlpha = false;
    for (uint32_t p : image.pixels) {
        uint32_t a = p >> 24;
        if (a) {
            anyAlpha = true;
        }
        if ((p & 0xFF) > a || ((p >> 8) & 0xFF) > a || ((p >> 16) & 0xFF) > a) {
            straightAlpha = true;
        }
    }

    for (uint32_t& p : image.pixels) {
        if (!anyAlpha) {
            p |= 0xFF000000;
        } else if (straightAlpha) {
            uint32_t a = p >> 24;
            p = PackPixel(((p & 0xFF) * a + 127) / 255,
                          (((p >> 8) & 0xFF) * a + 127) / 255,
                          (((p >> 16) & 0xFF) * a + 127) / 255, a);
        }
    }

    return true;
}

void BlendPixel(uint32_t& dst, uint32_t src) {
    uint32_t sa = src >> 24;
    if (!sa) {
        return;
    }
    uint32_t inv = 255 - sa;
    auto channel = [&](int shift) {
        uint32_t s = (src >> shift) & 0xFF;
        uint32_t d = (dst >> shift) & 0xFF;
        return std::min<uint32_t>(255, s + (d * inv + 127) / 255);
    };
    dst = PackPixel(channel(0), channel(8), channel(16), channel(24));
}

void BlendImage(Image& dst, const Image& src, int offsetX, int offsetY) {
    for (int y = 0; y < src.height; y++) {
        int dy = offsetY + y;
        if (dy < 0 || dy >= dst.height) {
            continue;
        }
        for (int x = 0; x < src.width; x++) {
            int dx = offsetX + x;
            if (dx < 0 || dx >= dst.width) {
                continue;
            }
            BlendPixel(dst.pixels[static_cast<size_t>(dy) * dst.width + dx],
                       src.pixels[static_cast<size_t>(y) * src.width + x]);
        }
    }
}

// Signed distance from a point to a rounded rectangle (negative inside).
float RoundedRectDistance(float px,
                          float py,
                          float left,
                          float top,
                          float right,
                          float bottom,
                          float radius);

// Rounds the corners of icons that fill a square up to their corners (e.g. a
// square app logo), so that they don't poke into the rounded group
// background. Icons with transparent corners (round ones) are left as is.
void RoundSquareIcon(Image& image) {
    if (image.width < 8 || image.height < 8) {
        return;
    }

    auto alphaAt = [&](int x, int y) {
        return image.pixels[static_cast<size_t>(y) * image.width + x] >> 24;
    };

    // Bounds of the visible pixels: icons often have a transparent margin.
    int left = image.width, top = image.height, right = -1, bottom = -1;
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            if (alphaAt(x, y) > 16) {
                left = std::min(left, x);
                top = std::min(top, y);
                right = std::max(right, x);
                bottom = std::max(bottom, y);
            }
        }
    }
    int width = right - left + 1;
    int height = bottom - top + 1;
    if (right < 0 || width < 8 || height < 8) {
        return;
    }

    // Square-cornered: all four corners of the visible bounds are opaque.
    constexpr uint32_t kOpaque = 200;
    int inset = std::max(1, std::min(width, height) / 32);
    if (alphaAt(left + inset, top + inset) < kOpaque ||
        alphaAt(right - inset, top + inset) < kOpaque ||
        alphaAt(left + inset, bottom - inset) < kOpaque ||
        alphaAt(right - inset, bottom - inset) < kOpaque) {
        return;
    }

    float radius = std::min(width, height) * 0.2f;
    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            float distance = RoundedRectDistance(
                x + 0.5f, y + 0.5f, static_cast<float>(left),
                static_cast<float>(top), static_cast<float>(right + 1),
                static_cast<float>(bottom + 1), radius);
            float coverage = std::clamp(0.5f - distance, 0.0f, 1.0f);
            if (coverage >= 1) {
                continue;
            }
            // Premultiplied: scaling all channels scales the alpha.
            uint32_t& p =
                image.pixels[static_cast<size_t>(y) * image.width + x];
            auto scaleChannel = [&](int shift) {
                return static_cast<uint32_t>(((p >> shift) & 0xFF) * coverage +
                                             0.5f);
            };
            p = PackPixel(scaleChannel(0), scaleChannel(8), scaleChannel(16),
                          scaleChannel(24));
        }
    }
}

// Signed distance from a point to a rounded rectangle (negative inside).
float RoundedRectDistance(float px,
                          float py,
                          float left,
                          float top,
                          float right,
                          float bottom,
                          float radius) {
    float cx = (left + right) / 2;
    float cy = (top + bottom) / 2;
    float hx = (right - left) / 2 - radius;
    float hy = (bottom - top) / 2 - radius;
    float qx = std::fabs(px - cx) - hx;
    float qy = std::fabs(py - cy) - hy;
    float ox = std::max(qx, 0.0f);
    float oy = std::max(qy, 0.0f);
    return std::sqrt(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0f) -
           radius;
}

void FillRoundedRect(Image& image,
                     float left,
                     float top,
                     float right,
                     float bottom,
                     float radius,
                     uint32_t r,
                     uint32_t g,
                     uint32_t b,
                     uint32_t a) {
    int minX = std::max(0, static_cast<int>(std::floor(left)));
    int minY = std::max(0, static_cast<int>(std::floor(top)));
    int maxX = std::min(image.width, static_cast<int>(std::ceil(right)));
    int maxY = std::min(image.height, static_cast<int>(std::ceil(bottom)));
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            float distance = RoundedRectDistance(x + 0.5f, y + 0.5f, left, top,
                                                 right, bottom, radius);
            float coverage = std::clamp(0.5f - distance, 0.0f, 1.0f);
            if (coverage <= 0) {
                continue;
            }
            uint32_t pa = static_cast<uint32_t>(a * coverage + 0.5f);
            uint32_t pixel = PackPixel((b * pa + 127) / 255, (g * pa + 127) / 255,
                                       (r * pa + 127) / 255, pa);
            BlendPixel(image.pixels[static_cast<size_t>(y) * image.width + x],
                       pixel);
        }
    }
}

// Downscales an image by area averaging (premultiplied pixels average
// correctly).
Image ScaleImage(const Image& src, int width, int height) {
    Image dst;
    dst.width = width;
    dst.height = height;
    dst.pixels.assign(static_cast<size_t>(width) * height, 0);
    if (src.width <= 0 || src.height <= 0 || width <= 0 || height <= 0) {
        return dst;
    }
    if (src.width == width && src.height == height) {
        dst.pixels = src.pixels;
        return dst;
    }

    float scaleX = static_cast<float>(src.width) / width;
    float scaleY = static_cast<float>(src.height) / height;
    for (int y = 0; y < height; y++) {
        float y0 = y * scaleY;
        float y1 = std::min<float>((y + 1) * scaleY, src.height);
        for (int x = 0; x < width; x++) {
            float x0 = x * scaleX;
            float x1 = std::min<float>((x + 1) * scaleX, src.width);
            float sum[4] = {0, 0, 0, 0};
            float total = 0;
            for (int sy = static_cast<int>(y0); sy < y1; sy++) {
                float wy = std::min<float>(sy + 1, y1) - std::max<float>(sy, y0);
                for (int sx = static_cast<int>(x0); sx < x1; sx++) {
                    float wx =
                        std::min<float>(sx + 1, x1) - std::max<float>(sx, x0);
                    float w = wx * wy;
                    uint32_t p =
                        src.pixels[static_cast<size_t>(sy) * src.width + sx];
                    for (int c = 0; c < 4; c++) {
                        sum[c] += ((p >> (c * 8)) & 0xFF) * w;
                    }
                    total += w;
                }
            }
            if (total <= 0) {
                continue;
            }
            auto channel = [&](int c) {
                return static_cast<uint32_t>(
                    std::min(255.0f, sum[c] / total + 0.5f));
            };
            dst.pixels[static_cast<size_t>(y) * width + x] =
                PackPixel(channel(0), channel(1), channel(2), channel(3));
        }
    }
    return dst;
}

// Scales an image to fit a square box, keeping its aspect ratio.
Image ScaleToFit(const Image& src, int box) {
    if (src.width <= box && src.height <= box) {
        return src;
    }
    float scale =
        static_cast<float>(box) / std::max(src.width, src.height);
    int width = std::max(1, static_cast<int>(std::lround(src.width * scale)));
    int height =
        std::max(1, static_cast<int>(std::lround(src.height * scale)));
    return ScaleImage(src, width, height);
}

// Spacing of the group icon grid, relative to the icon size: tight, so that
// the shortcut icons get as much room as possible.
constexpr float kPreviewPadding = 0.065f;

float PreviewGap(int size, int columns) {
    return size * (columns >= 3 ? 0.038f : 0.044f);
}

// Size of a grid cell of a group icon of the given size.
float PreviewCellSize(int size, int columns) {
    return (size - 2 * size * kPreviewPadding -
            (columns - 1) * PreviewGap(size, columns)) /
           columns;
}

// Below this cell size, the shell's own small icons (designed for small
// sizes) look better than scaled-down large ones.
constexpr int kMinScaledCell = 24;

// Renders the group icon: a rounded background with a grid of up to
// columns x columns shortcut icons. largeIcons holds the shown shortcuts'
// icons at the largest cell size, scaled down for larger cells; smaller cells
// load the shell's icon at their own size.
Image RenderGroupPreview(int size,
                         const std::wstring& folder,
                         const std::vector<std::wstring>& names,
                         bool dark,
                         int columns,
                         const std::vector<Image>& largeIcons) {
    Image image;
    image.width = size;
    image.height = size;
    image.pixels.assign(static_cast<size_t>(size) * size, 0);

    // The background fills the whole icon, like full-bleed app icons; its
    // antialiased edge stays within the image.
    float margin = 0;
    float radius = size * 0.23f;
    if (dark) {
        FillRoundedRect(image, margin, margin, size - margin, size - margin,
                        radius, 48, 48, 52, 215);
    } else {
        FillRoundedRect(image, margin, margin, size - margin, size - margin,
                        radius, 242, 242, 247, 215);
    }

    float padding = size * kPreviewPadding;
    float gap = PreviewGap(size, columns);
    float cell = PreviewCellSize(size, columns);
    int cellPixels = std::max(1, static_cast<int>(cell));

    size_t count =
        std::min<size_t>(static_cast<size_t>(columns) * columns, names.size());
    for (size_t i = 0; i < count; i++) {
        Image icon;
        if (cellPixels >= kMinScaledCell && i < largeIcons.size() &&
            largeIcons[i].width > 0) {
            icon = ScaleToFit(largeIcons[i], cellPixels);
        } else {
            if (!LoadItemImage(folder + L"\\" + names[i], cellPixels, icon)) {
                continue;
            }
            RoundSquareIcon(icon);
        }

        float cellX = padding + (i % columns) * (cell + gap);
        float cellY = padding + (i / columns) * (cell + gap);
        int x = static_cast<int>(std::lround(cellX + (cell - icon.width) / 2));
        int y = static_cast<int>(std::lround(cellY + (cell - icon.height) / 2));
        BlendImage(image, icon, x, y);
    }

    return image;
}

#pragma pack(push, 2)
struct IconDir {
    WORD reserved;
    WORD type;
    WORD count;
};

struct IconDirEntry {
    BYTE width;
    BYTE height;
    BYTE colorCount;
    BYTE reserved;
    WORD planes;
    WORD bitCount;
    DWORD bytesInRes;
    DWORD imageOffset;
};
#pragma pack(pop)

// Writes a multi-size .ico file with 32-bit BMP entries.
bool WriteIconFile(const std::wstring& path, const std::vector<Image>& images) {
    std::vector<BYTE> out;
    auto append = [&](const void* data, size_t size) {
        const BYTE* bytes = static_cast<const BYTE*>(data);
        out.insert(out.end(), bytes, bytes + size);
    };

    IconDir dir = {0, 1, static_cast<WORD>(images.size())};
    append(&dir, sizeof(dir));

    size_t entriesOffset = out.size();
    out.resize(out.size() + sizeof(IconDirEntry) * images.size());

    for (size_t i = 0; i < images.size(); i++) {
        const Image& image = images[i];
        int size = image.width;
        DWORD maskStride = ((size + 31) / 32) * 4;
        DWORD xorSize = static_cast<DWORD>(size) * size * 4;
        DWORD maskSize = maskStride * size;

        IconDirEntry entry = {};
        entry.width = size >= 256 ? 0 : static_cast<BYTE>(size);
        entry.height = entry.width;
        entry.planes = 1;
        entry.bitCount = 32;
        entry.bytesInRes = sizeof(BITMAPINFOHEADER) + xorSize + maskSize;
        entry.imageOffset = static_cast<DWORD>(out.size());
        memcpy(out.data() + entriesOffset + i * sizeof(entry), &entry,
               sizeof(entry));

        BITMAPINFOHEADER header = {};
        header.biSize = sizeof(header);
        header.biWidth = size;
        header.biHeight = size * 2;  // XOR + AND masks.
        header.biPlanes = 1;
        header.biBitCount = 32;
        header.biCompression = BI_RGB;
        header.biSizeImage = xorSize + maskSize;
        append(&header, sizeof(header));

        // Bottom-up rows with straight (non-premultiplied) alpha.
        for (int y = size - 1; y >= 0; y--) {
            for (int x = 0; x < size; x++) {
                uint32_t p = image.pixels[static_cast<size_t>(y) * size + x];
                uint32_t a = p >> 24;
                BYTE bgra[4] = {0, 0, 0, 0};
                if (a) {
                    for (int c = 0; c < 3; c++) {
                        uint32_t v = (p >> (c * 8)) & 0xFF;
                        bgra[c] = static_cast<BYTE>(
                            std::min<uint32_t>(255, (v * 255 + a / 2) / a));
                    }
                    bgra[3] = static_cast<BYTE>(a);
                }
                append(bgra, sizeof(bgra));
            }
        }

        out.insert(out.end(), maskSize, 0);
    }

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Failed to create icon file %s: %u", path.c_str(),
               GetLastError());
        return false;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(file, out.data(), static_cast<DWORD>(out.size()),
                        &written, nullptr);
    CloseHandle(file);
    return ok && written == out.size();
}

// Deletes group icon files other than the given ones.
void DeleteOldIcons(const std::wstring& folder,
                    const std::wstring& keep,
                    const std::wstring& keepPrevious) {
    WIN32_FIND_DATAW data;
    std::wstring pattern = folder + L"\\" + kIconPrefix + L"*.ico";
    HANDLE find = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &data,
                                   FindExSearchNameMatch, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (!EqualsNoCase(data.cFileName, keep) &&
            !EqualsNoCase(data.cFileName, keepPrevious)) {
            DeleteFileW((folder + L"\\" + data.cFileName).c_str());
        }
    } while (FindNextFileW(find, &data));

    FindClose(find);
}

////////////////////////////////////////////////////////////////////////////////
// Group operations

// Points the folder's icon at the given file (relative to the folder).
void SetFolderIcon(const std::wstring& folder, const std::wstring& iconName) {
    std::wstring ini = IniPathOf(folder);

    // The official API also invalidates the shell's cached folder
    // customization, which a plain desktop.ini write doesn't.
    std::wstring iconFile = iconName;
    SHFOLDERCUSTOMSETTINGS settings = {};
    settings.dwSize = sizeof(settings);
    settings.dwMask = FCSM_ICONFILE;
    settings.pszIconFile = iconFile.data();
    settings.iIconIndex = 0;
    HRESULT hr =
        SHGetSetFolderCustomSettings(&settings, folder.c_str(), FCS_FORCEWRITE);
    if (FAILED(hr)) {
        Wh_Log(L"SHGetSetFolderCustomSettings failed for %s: 0x%08X",
               folder.c_str(), hr);
    }

    // Keep a relative IconResource so that the icon survives renaming or
    // moving the group, and drop the legacy keys the API may have written.
    DeleteIniValue(ini, L".ShellClassInfo", L"IconFile");
    DeleteIniValue(ini, L".ShellClassInfo", L"IconIndex");
    WriteIniValue(ini, L".ShellClassInfo", L"IconResource", iconName + L",0");
}

void NotifyFolderChanged(const std::wstring& folder) {
    // FLUSHNOWAIT: this may run on the desktop thread, which also processes
    // the notifications, so waiting for delivery could deadlock.
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                   folder.c_str(), nullptr);
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                   folder.c_str(), nullptr);
}

// Identifies what a group icon shows; the icon is re-rendered only when this
// changes. Bump the version when the rendering itself changes.
constexpr int kIconRenderVersion = 2;

// 64-bit FNV-1a hash of a string, in hex.
std::wstring HashToHex(const std::wstring& text) {
    uint64_t hash = 14695981039346656037ull;
    for (wchar_t c : text) {
        for (int shift = 0; shift < 16; shift += 8) {
            hash ^= (static_cast<uint32_t>(c) >> shift) & 0xFF;
            hash *= 1099511628211ull;
        }
    }
    wchar_t buffer[17];
    swprintf_s(buffer, ARRAYSIZE(buffer), L"%016llX",
               static_cast<unsigned long long>(hash));
    return buffer;
}

// A fixed-length key of what a group icon shows: the render version, theme
// and grid, and the shown shortcuts with their last-write times (so that a
// shortcut whose own icon was changed is picked up too). Stored as a hash,
// since the full text can exceed what the profile APIs read back.
std::wstring IconKeyOf(const std::wstring& folder,
                       const std::vector<std::wstring>& order,
                       bool dark,
                       int columns) {
    size_t shown =
        std::min<size_t>(static_cast<size_t>(columns) * columns, order.size());
    std::wstring key = std::to_wstring(kIconRenderVersion) + L";" +
                       (dark ? L"dark" : L"light") + L";" +
                       std::to_wstring(columns) + L";";
    for (size_t i = 0; i < shown; i++) {
        ULONGLONG lastWrite = 0;
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (GetFileAttributesExW((folder + L"\\" + order[i]).c_str(),
                                 GetFileExInfoStandard, &data)) {
            lastWrite =
                (static_cast<ULONGLONG>(data.ftLastWriteTime.dwHighDateTime)
                 << 32) |
                data.ftLastWriteTime.dwLowDateTime;
        }
        key += order[i] + L"@" + std::to_wstring(lastWrite) + L"|";
    }
    return HashToHex(key);
}

// The icon file named by a group's desktop.ini, without its index.
std::wstring IconFileOf(const std::wstring& ini) {
    std::wstring icon = ReadIniValue(ini, L".ShellClassInfo", L"IconResource");
    size_t comma = icon.find_last_of(L',');
    if (comma != std::wstring::npos) {
        icon.resize(comma);
    }
    return FileNameOf(icon);
}

// Brings a group's desktop.ini and icon up to date. Files are only written
// when something changed: the order is rewritten if it differs, and the icon
// is re-rendered only if what it shows (the first shortcuts, the theme, the
// grid) changed, its file is missing, or force is set. This keeps group
// folders from being rewritten needlessly, e.g. on every sign-in.
bool UpdateGroupIcon(const std::wstring& folder, bool force = false) {
    std::wstring ini = IniPathOf(folder);
    EnsureUnicodeIni(ini);

    std::vector<std::wstring> order = BuildOrder(folder);
    if (GetPrivateProfileIntW(kIniSection, L"IsGroup", 0, ini.c_str()) != 1) {
        WriteIniValue(ini, kIniSection, L"IsGroup", L"1");
    }
    if (ReadOrder(ini) != order) {
        WriteIniValue(ini, kIniSection, L"Order", JoinOrder(order));
    }

    Settings settings = GetSettings();
    bool dark = IsPreviewDark(settings);
    int columns = settings.preview3x3 && order.size() > 4 ? 3 : 2;
    std::wstring key = IconKeyOf(folder, order, dark, columns);

    // The icon currently in use. It's kept until the next update: the shell
    // may still be reading it, and a missing file makes it fall back to the
    // default folder icon.
    std::wstring previousIcon = IconFileOf(ini);

    if (!force && !previousIcon.empty() &&
        PathExists(folder + L"\\" + previousIcon) &&
        ReadIniValue(ini, kIniSection, L"IconKey") == key) {
        // Up to date. desktop.ini is only honored for read-only or system
        // folders, so restore the attribute if it was removed.
        DWORD attrs = GetFileAttributesW(folder.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES &&
            !(attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM))) {
            PathMakeSystemFolderW(folder.c_str());
            NotifyFolderChanged(folder);
        }
        return true;
    }

    // Each shown shortcut's icon is extracted once, at the largest cell
    // size, and scaled down for the other sizes.
    size_t shown =
        std::min<size_t>(static_cast<size_t>(columns) * columns, order.size());
    int largestCell =
        std::max(1, static_cast<int>(PreviewCellSize(kIconSizes[0], columns)));
    std::vector<Image> largeIcons(shown);
    for (size_t i = 0; i < shown; i++) {
        if (LoadItemImage(folder + L"\\" + order[i], largestCell,
                          largeIcons[i])) {
            RoundSquareIcon(largeIcons[i]);
        }
    }

    std::vector<Image> images;
    for (int size : kIconSizes) {
        images.push_back(
            RenderGroupPreview(size, folder, order, dark, columns, largeIcons));
    }

    // A new file name for every update bypasses the shell icon cache.
    static unsigned int counter = 0;
    std::wstring iconName = std::wstring(kIconPrefix) +
                            std::to_wstring(GetTickCount64()) + L"-" +
                            std::to_wstring(++counter) + L".ico";
    if (!WriteIconFile(folder + L"\\" + iconName, images)) {
        return false;
    }

    // desktop.ini is only honored for read-only or system folders.
    if (!PathMakeSystemFolderW(folder.c_str())) {
        Wh_Log(L"PathMakeSystemFolderW failed for %s: %u", folder.c_str(),
               GetLastError());
    }

    SetFolderIcon(folder, iconName);
    WriteIniValue(ini, kIniSection, L"IconKey", key);
    DeleteOldIcons(folder, iconName, previousIcon);
    NotifyFolderChanged(folder);

    Wh_Log(L"Updated icon of %s: %s (%d items)", folder.c_str(),
           iconName.c_str(), static_cast<int>(order.size()));
    return true;
}

// Brings all groups on the user's desktop up to date. Only groups whose icon
// is out of date (e.g. after a theme or settings change) are re-rendered.
void RefreshAllGroupIcons() {
    if (g_userDesktop.empty()) {
        return;
    }

    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((g_userDesktop + L"\\*").c_str(),
                                   FindExInfoBasic, &data,
                                   FindExSearchLimitToDirectories, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    std::vector<std::wstring> groups;
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            wcscmp(data.cFileName, L".") == 0 ||
            wcscmp(data.cFileName, L"..") == 0) {
            continue;
        }
        std::wstring path = g_userDesktop + L"\\" + data.cFileName;
        if (IsGroupFolder(path)) {
            groups.push_back(path);
        }
    } while (FindNextFileW(find, &data));

    FindClose(find);

    for (const auto& group : groups) {
        UpdateGroupIcon(group);
    }
}

// Moves a file into a folder and returns its new file name, or an empty
// string on failure.
std::wstring MoveIntoFolder(const std::wstring& source,
                            const std::wstring& folder) {
    std::wstring destination = UniqueFilePath(folder, FileNameOf(source));
    if (!MoveFileExW(source.c_str(), destination.c_str(),
                     MOVEFILE_COPY_ALLOWED)) {
        Wh_Log(L"Failed to move %s to %s: %u", source.c_str(),
               destination.c_str(), GetLastError());
        return std::wstring();
    }

    NotifyMoved(source, destination);
    return FileNameOf(destination);
}

struct TargetInfo {
    bool valid = false;
    bool isGroup = false;
    int index = -1;
    std::wstring path;
    bool hasPosition = false;
    POINT position = {};
};

void SchedulePosition(const std::wstring& path, POINT point) {
    HWND defView = g_defView.load();
    if (!defView) {
        return;
    }
    g_pendingPositions.push_back({path, point, 0});
    SetTimer(defView, kPositionTimerId, kPositionTimerIntervalMs, nullptr);
}

bool CreateGroup(const TargetInfo& target,
                 const std::vector<std::wstring>& dragged) {
    if (g_userDesktop.empty()) {
        return false;
    }

    std::wstring folder =
        UniqueFolderPath(g_userDesktop, GetSettings().groupName);
    if (!CreateDirectoryW(folder.c_str(), nullptr)) {
        Wh_Log(L"Failed to create %s: %u", folder.c_str(), GetLastError());
        return false;
    }

    // Keep the folder hidden while it's being built so that the desktop
    // doesn't pick it up (and cache it) as a plain folder.
    SetFileAttributesW(folder.c_str(),
                       FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);

    std::wstring ini = IniPathOf(folder);
    EnsureUnicodeIni(ini);
    WriteIniValue(ini, kIniSection, L"IsGroup", L"1");

    std::vector<std::wstring> order;
    std::wstring targetName = MoveIntoFolder(target.path, folder);
    if (targetName.empty()) {
        // Roll back: the target couldn't be moved (e.g. Public desktop).
        DeleteFileW(ini.c_str());
        SetFileAttributesW(folder.c_str(), FILE_ATTRIBUTE_NORMAL);
        RemoveDirectoryW(folder.c_str());
        return false;
    }
    order.push_back(targetName);

    for (const auto& path : dragged) {
        std::wstring name = MoveIntoFolder(path, folder);
        if (!name.empty()) {
            order.push_back(name);
        }
    }

    WriteIniValue(ini, kIniSection, L"Order", JoinOrder(order));
    UpdateGroupIcon(folder);

    // Reveal the finished group, keeping the read-only/system attribute that
    // makes the shell honor desktop.ini.
    DWORD attrs = GetFileAttributesW(folder.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesW(folder.c_str(), attrs & ~FILE_ATTRIBUTE_HIDDEN);
    }
    SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folder.c_str(),
                   nullptr);

    if (target.hasPosition) {
        SchedulePosition(folder, target.position);
    }

    Wh_Log(L"Created group %s with %d items", folder.c_str(),
           static_cast<int>(order.size()));
    return true;
}

bool AddToGroup(const TargetInfo& target,
                const std::vector<std::wstring>& dragged) {
    std::wstring ini = IniPathOf(target.path);
    std::vector<std::wstring> order = ReadOrder(ini);

    bool movedAny = false;
    for (const auto& path : dragged) {
        std::wstring name = MoveIntoFolder(path, target.path);
        if (!name.empty()) {
            order.push_back(name);
            movedAny = true;
        }
    }

    if (!movedAny) {
        return false;
    }

    WriteIniValue(ini, kIniSection, L"Order", JoinOrder(order));
    UpdateGroupIcon(target.path);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Desktop folder view access (desktop thread only)

// Returns the desktop's active shell view; the caller releases the result.
IShellView* GetDesktopShellView() {
    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows)))) {
        return nullptr;
    }

    VARIANT location;
    VariantInit(&location);
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;

    VARIANT empty;
    VariantInit(&empty);

    long hwnd = 0;
    IDispatch* dispatch = nullptr;
    HRESULT hr = shellWindows->FindWindowSW(&location, &empty, SWC_DESKTOP,
                                            &hwnd, SWFO_NEEDDISPATCH,
                                            &dispatch);
    shellWindows->Release();
    if (hr != S_OK || !dispatch) {
        if (dispatch) {
            dispatch->Release();
        }
        return nullptr;
    }

    IServiceProvider* serviceProvider = nullptr;
    hr = dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider));
    dispatch->Release();
    if (FAILED(hr)) {
        return nullptr;
    }

    IShellBrowser* browser = nullptr;
    hr = serviceProvider->QueryService(kSidSTopLevelBrowser,
                                       IID_PPV_ARGS(&browser));
    serviceProvider->Release();
    if (FAILED(hr)) {
        return nullptr;
    }

    IShellView* view = nullptr;
    hr = browser->QueryActiveShellView(&view);
    browser->Release();
    if (FAILED(hr) || !view) {
        return nullptr;
    }

    return view;
}

IFolderView* GetDesktopFolderView() {
    if (g_folderView) {
        return g_folderView;
    }

    IShellView* view = GetDesktopShellView();
    if (!view) {
        return nullptr;
    }

    IFolderView* folderView = nullptr;
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&folderView));
    view->Release();
    if (FAILED(hr)) {
        return nullptr;
    }

    g_folderView = folderView;
    return g_folderView;
}

std::wstring GetItemPath(IShellFolder* folder, PCUITEMID_CHILD pidl) {
    STRRET str;
    if (FAILED(folder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &str))) {
        return std::wstring();
    }

    wchar_t buffer[MAX_PATH * 4];
    if (FAILED(StrRetToBufW(&str, pidl, buffer, ARRAYSIZE(buffer)))) {
        return std::wstring();
    }
    return buffer;
}

// Returns the view item with the given path; the caller frees the result.
PITEMID_CHILD FindViewItemByPath(IFolderView* folderView,
                                 const std::wstring& path) {
    int count = 0;
    if (FAILED(folderView->ItemCount(SVGIO_ALLVIEW, &count))) {
        return nullptr;
    }

    IShellFolder* folder = nullptr;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
        return nullptr;
    }

    PITEMID_CHILD result = nullptr;
    for (int i = 0; i < count && !result; i++) {
        PITEMID_CHILD pidl = nullptr;
        if (FAILED(folderView->Item(i, &pidl)) || !pidl) {
            continue;
        }
        if (EqualsNoCase(GetItemPath(folder, pidl), path)) {
            result = pidl;
        } else {
            CoTaskMemFree(pidl);
        }
    }

    folder->Release();
    return result;
}

void ProcessPendingPositions(HWND defView) {
    IFolderView* folderView = GetDesktopFolderView();

    for (auto it = g_pendingPositions.begin();
         it != g_pendingPositions.end();) {
        PITEMID_CHILD pidl =
            folderView ? FindViewItemByPath(folderView, it->path) : nullptr;
        if (pidl) {
            PCUITEMID_CHILD items[] = {pidl};
            POINT point = it->point;
            folderView->SelectAndPositionItems(1, items, &point,
                                               SVSI_POSITIONITEM);
            CoTaskMemFree(pidl);
            it = g_pendingPositions.erase(it);
        } else if (++it->attempts >= kPositionMaxAttempts) {
            it = g_pendingPositions.erase(it);
        } else {
            ++it;
        }
    }

    if (g_pendingPositions.empty()) {
        KillTimer(defView, kPositionTimerId);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Drop target wrapper

class GroupingDropTarget final : public IDropTarget {
   public:
    explicit GroupingDropTarget(IDropTarget* inner) : m_inner(inner) {
        m_inner->AddRef();
    }

    GroupingDropTarget(const GroupingDropTarget&) = delete;
    GroupingDropTarget& operator=(const GroupingDropTarget&) = delete;

    IDropTarget* Inner() const { return m_inner; }

    // IUnknown

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) Release() override {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // IDropTarget

    STDMETHODIMP DragEnter(IDataObject* dataObject,
                           DWORD keyState,
                           POINTL point,
                           DWORD* effect) override {
        ResetDragState();
        if (dataObject) {
            m_dataObject = dataObject;
            m_dataObject->AddRef();
        }
        // Right-button drags end with Explorer's Copy/Move/Create shortcuts
        // menu, which is kept.
        m_rightDrag = (keyState & MK_RBUTTON) != 0;
        BeginBusy();

        DWORD allowed = effect ? *effect : DROPEFFECT_NONE;
        HRESULT hr = m_inner->DragEnter(dataObject, keyState, point, effect);
        if (FAILED(hr)) {
            // OLE won't call DragLeave or Drop after a failed DragEnter.
            ResetDragState();
            EndBusy();
            return hr;
        }
        UpdateFeedback(keyState, point, allowed, effect);
        return hr;
    }

    STDMETHODIMP DragOver(DWORD keyState,
                          POINTL point,
                          DWORD* effect) override {
        DWORD allowed = effect ? *effect : DROPEFFECT_NONE;
        HRESULT hr = m_inner->DragOver(keyState, point, effect);
        UpdateFeedback(keyState, point, allowed, effect);
        return hr;
    }

    STDMETHODIMP DragLeave() override {
        SetHighlight(-1);
        ResetDragState();
        HRESULT hr = m_inner->DragLeave();
        EndBusy();
        return hr;
    }

    STDMETHODIMP Drop(IDataObject* dataObject,
                      DWORD keyState,
                      POINTL point,
                      DWORD* effect) override {
        // The drag over the desktop ends here, however this returns.
        struct EndBusyOnExit {
            GroupingDropTarget* target;
            ~EndBusyOnExit() { target->EndBusy(); }
        } endBusy{this};

        DWORD allowed = effect ? *effect : DROPEFFECT_NONE;
        TargetInfo target = GetGroupingTarget(keyState, point, allowed);
        SetHighlight(-1);

        if (!target.valid) {
            ResetDragState();
            // Explorer handles the drop; a group it went into gets its
            // icon refreshed once the files have arrived.
            std::wstring group = GroupPathAt(point);
            HRESULT hr = m_inner->Drop(dataObject, keyState, point, effect);
            if (!group.empty()) {
                ScheduleGroupSync(group);
            }
            return hr;
        }

        std::vector<std::wstring> dragged = m_draggedPaths;
        ResetDragState();

        // Cancel the original drop and handle it ourselves. Reporting no
        // effect keeps the drag source from touching the moved files.
        m_inner->DragLeave();
        if (effect) {
            *effect = DROPEFFECT_NONE;
        }

        bool ok = target.isGroup ? AddToGroup(target, dragged)
                                 : CreateGroup(target, dragged);
        if (!ok) {
            MessageBeep(MB_ICONWARNING);
        }
        return S_OK;
    }

    void SetHighlight(int index) {
        if (index == m_highlightIndex) {
            return;
        }
        if (g_listView && IsWindow(g_listView)) {
            if (m_highlightIndex >= 0) {
                ListView_SetItemState(g_listView, m_highlightIndex, 0,
                                      LVIS_DROPHILITED);
            }
            if (index >= 0) {
                ListView_SetItemState(g_listView, index, LVIS_DROPHILITED,
                                      LVIS_DROPHILITED);
            }
        }
        m_highlightIndex = index;
    }

   private:
    enum class DraggedState { Unknown, Valid, Invalid };

    ~GroupingDropTarget() {
        ResetDragState();
        EndBusy();
        m_inner->Release();
    }

    // A drag over the desktop runs in a modal loop on the desktop thread, so
    // it counts as a busy call while it's over the desktop.
    void BeginBusy() {
        if (!m_busy) {
            m_busy = true;
            g_busyCalls++;
        }
    }

    void EndBusy() {
        if (m_busy) {
            m_busy = false;
            g_busyCalls--;
        }
    }

    void ResetDragState() {
        if (m_dataObject) {
            m_dataObject->Release();
            m_dataObject = nullptr;
        }
        m_draggedPaths.clear();
        m_draggedState = DraggedState::Unknown;
        m_cachedIndex = -2;
        m_cachedTarget = TargetInfo();
    }

    // Lazily extracts the dragged paths; only shortcuts lying directly on the
    // desktop qualify.
    bool EnsureDraggedPaths() {
        if (m_draggedState != DraggedState::Unknown) {
            return m_draggedState == DraggedState::Valid;
        }

        m_draggedState = DraggedState::Invalid;
        if (!m_dataObject) {
            return false;
        }

        FORMATETC format = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                            TYMED_HGLOBAL};
        STGMEDIUM medium = {};
        if (FAILED(m_dataObject->GetData(&format, &medium))) {
            return false;
        }

        HDROP drop = static_cast<HDROP>(medium.hGlobal);
        UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
        std::vector<std::wstring> paths;
        bool allValid = count > 0;

        for (UINT i = 0; i < count && allValid; i++) {
            UINT length = DragQueryFileW(drop, i, nullptr, 0);
            std::wstring path(length, L'\0');
            DragQueryFileW(drop, i, path.data(), length + 1);

            if (!HasShortcutExtension(path) || !IsInDesktopDir(path)) {
                allValid = false;
            } else {
                paths.push_back(path);
            }
        }

        ReleaseStgMedium(&medium);

        if (!allValid) {
            return false;
        }

        m_draggedPaths = std::move(paths);
        m_draggedState = DraggedState::Valid;
        return true;
    }

    TargetInfo ResolveIndex(int index) {
        TargetInfo info;

        IFolderView* folderView = GetDesktopFolderView();
        if (!folderView) {
            return info;
        }

        PITEMID_CHILD pidl = nullptr;
        if (FAILED(folderView->Item(index, &pidl)) || !pidl) {
            return info;
        }

        IShellFolder* folder = nullptr;
        std::wstring path;
        if (SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
            path = GetItemPath(folder, pidl);
            folder->Release();
        }

        POINT position = {};
        bool hasPosition =
            SUCCEEDED(folderView->GetItemPosition(pidl, &position));
        CoTaskMemFree(pidl);

        if (path.empty()) {
            return info;
        }

        for (const auto& dragged : m_draggedPaths) {
            if (EqualsNoCase(dragged, path)) {
                return info;
            }
        }

        if (HasShortcutExtension(path) && IsInDesktopDir(path)) {
            info.isGroup = false;
        } else if (IsGroupFolder(path)) {
            info.isGroup = true;
        } else {
            return info;
        }

        info.valid = true;
        info.index = index;
        info.path = path;
        info.hasPosition = hasPosition;
        info.position = position;
        return info;
    }

    TargetInfo ResolveAt(POINTL screenPoint) {
        if (!g_listView) {
            return TargetInfo();
        }

        LVHITTESTINFO hitTest = {};
        hitTest.pt = {screenPoint.x, screenPoint.y};
        ScreenToClient(g_listView, &hitTest.pt);

        int index = ListView_HitTest(g_listView, &hitTest);
        if (index < 0 || !(hitTest.flags & LVHT_ONITEM)) {
            index = -1;
        }

        if (index != m_cachedIndex) {
            m_cachedIndex = index;
            m_cachedTarget = index >= 0 ? ResolveIndex(index) : TargetInfo();
        }
        return m_cachedTarget;
    }

    // With the grouping key, desktop shortcuts dropped on a shortcut form a
    // group and dropped on a group join it. Without any key, dropping on a
    // group adds to it as well. Other keys (Ctrl to copy, etc.) are left to
    // Explorer.
    TargetInfo GetGroupingTarget(DWORD keyState, POINTL point, DWORD allowed) {
        if (m_rightDrag) {
            return TargetInfo();
        }
        DWORD modifiers = keyState & kModifierFlags;
        bool groupingKey = modifiers == GetSettings().modifierMask;
        if ((!groupingKey && modifiers != 0) ||
            !(allowed & DROPEFFECT_MOVE)) {
            return TargetInfo();
        }
        if (!EnsureDraggedPaths()) {
            return TargetInfo();
        }
        TargetInfo target = ResolveAt(point);
        if (!groupingKey && !target.isGroup) {
            return TargetInfo();
        }
        return target;
    }

    // Returns the group under a screen point regardless of what's dragged.
    std::wstring GroupPathAt(POINTL screenPoint) {
        if (!g_listView) {
            return std::wstring();
        }
        LVHITTESTINFO hitTest = {};
        hitTest.pt = {screenPoint.x, screenPoint.y};
        ScreenToClient(g_listView, &hitTest.pt);
        int index = ListView_HitTest(g_listView, &hitTest);
        if (index < 0 || !(hitTest.flags & LVHT_ONITEM)) {
            return std::wstring();
        }
        std::wstring path = GetViewItemPath(index);
        return IsGroupFolder(path) ? path : std::wstring();
    }

    void UpdateFeedback(DWORD keyState,
                        POINTL point,
                        DWORD allowed,
                        DWORD* effect) {
        TargetInfo target = GetGroupingTarget(keyState, point, allowed);
        SetHighlight(target.valid ? target.index : -1);
        if (target.valid && effect) {
            *effect = DROPEFFECT_MOVE;
        }
    }

    LONG m_refCount = 1;
    IDropTarget* m_inner;
    IDataObject* m_dataObject = nullptr;
    DraggedState m_draggedState = DraggedState::Unknown;
    std::vector<std::wstring> m_draggedPaths;
    int m_cachedIndex = -2;
    TargetInfo m_cachedTarget;
    int m_highlightIndex = -1;
    bool m_rightDrag = false;
    bool m_busy = false;
};

////////////////////////////////////////////////////////////////////////////////
// Group maintenance (desktop thread only)

constexpr UINT_PTR kSyncTimerId = 0x57484448;
constexpr UINT kSyncTimerIntervalMs = 1500;
constexpr int kSyncMaxTicks = 4;

// Groups to re-check shortly after a shell command (e.g. Delete from the
// panel's context menu), which may complete asynchronously.
struct PendingSync {
    std::wstring folder;
    int ticks;
};

std::vector<PendingSync> g_pendingSyncs;

// Moves the group's only remaining shortcut back to the desktop, at the
// group's position, and deletes the group folder.
bool DissolveGroup(const std::wstring& folder) {
    std::vector<std::wstring> entries = ListGroupEntries(folder);
    if (entries.size() > 1 || g_userDesktop.empty()) {
        return false;
    }

    POINT position = {};
    bool hasPosition = false;
    if (IFolderView* folderView = GetDesktopFolderView()) {
        if (PITEMID_CHILD pidl = FindViewItemByPath(folderView, folder)) {
            hasPosition =
                SUCCEEDED(folderView->GetItemPosition(pidl, &position));
            CoTaskMemFree(pidl);
        }
    }

    std::wstring moved;
    if (entries.size() == 1) {
        std::wstring source = folder + L"\\" + entries[0];
        moved = UniqueFilePath(g_userDesktop, entries[0]);
        if (!MoveFileExW(source.c_str(), moved.c_str(),
                         MOVEFILE_COPY_ALLOWED)) {
            Wh_Log(L"Failed to move %s out of its group: %u", source.c_str(),
                   GetLastError());
            return false;
        }
        NotifyMoved(source, moved);
    }

    // Remove the mod's own files; anything else left in the folder (e.g.
    // hidden files of the user) keeps it from being deleted.
    DeleteOldIcons(folder, std::wstring(), std::wstring());
    std::wstring ini = IniPathOf(folder);
    SetFileAttributesW(ini.c_str(), FILE_ATTRIBUTE_NORMAL);
    DeleteFileW(ini.c_str());
    SetFileAttributesW(folder.c_str(), FILE_ATTRIBUTE_NORMAL);
    if (RemoveDirectoryW(folder.c_str())) {
        SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                       folder.c_str(), nullptr);
    } else {
        Wh_Log(L"Failed to remove group folder %s: %u", folder.c_str(),
               GetLastError());
    }

    if (!moved.empty() && hasPosition) {
        SchedulePosition(moved, position);
    }

    Wh_Log(L"Dissolved group %s", folder.c_str());
    return true;
}

// Brings a group up to date after its content changed: dissolves it if
// allowed and only one shortcut is left, otherwise refreshes its icon if the
// content differs from the stored order.
void SyncGroup(const std::wstring& folder, bool allowDissolve) {
    if (!IsGroupFolder(folder)) {
        return;
    }

    std::vector<std::wstring> order = BuildOrder(folder);
    if (allowDissolve && GetSettings().dissolveSingleItem && order.size() <= 1) {
        DissolveGroup(folder);
        return;
    }

    // Writes nothing if the group is up to date.
    UpdateGroupIcon(folder);
}

void ScheduleGroupSync(const std::wstring& folder) {
    HWND defView = g_defView.load();
    if (!defView) {
        return;
    }
    g_pendingSyncs.push_back({folder, 0});
    SetTimer(defView, kSyncTimerId, kSyncTimerIntervalMs, nullptr);
}

void ProcessPendingSyncs(HWND defView) {
    for (auto it = g_pendingSyncs.begin(); it != g_pendingSyncs.end();) {
        // Checked on every tick: the command may finish late, and syncing an
        // up-to-date group does nothing.
        SyncGroup(it->folder, true);
        if (++it->ticks >= kSyncMaxTicks || !PathExists(it->folder)) {
            it = g_pendingSyncs.erase(it);
        } else {
            ++it;
        }
    }

    if (g_pendingSyncs.empty()) {
        KillTimer(defView, kSyncTimerId);
    }
}

// Desktop view checks after moving files in or out of the desktop.
//
// A move done during a drag (the desktop being both the drag source and the
// drop target) can have its change notification missed by the desktop view,
// leaving a "ghost" icon of a file that's gone, or a missing icon for a file
// that arrived. Each moved desktop file is checked shortly afterwards; the
// notification is repeated if needed, and the view is refreshed as a last
// resort.

constexpr UINT_PTR kViewCheckTimerId = 0x57484449;
constexpr UINT kViewCheckIntervalMs = 250;
// Notifications are repeated for this many ticks before refreshing.
constexpr int kViewCheckNotifyTicks = 2;

struct ViewCheck {
    std::wstring path;
    int ticks;
};

std::vector<ViewCheck> g_viewChecks;

void ScheduleViewCheck(const std::wstring& path) {
    HWND defView = g_defView.load();
    if (!defView || !IsInDesktopDir(path)) {
        return;
    }
    g_viewChecks.push_back({path, 0});
    SetTimer(defView, kViewCheckTimerId, kViewCheckIntervalMs, nullptr);
}

// Notifies the shell of a file moved between folders, as a delete and a
// create (clearer than a rename across folders), and schedules checks of the
// desktop view.
void NotifyMoved(const std::wstring& source, const std::wstring& destination) {
    SHChangeNotify(SHCNE_DELETE, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                   source.c_str(), nullptr);
    SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                   destination.c_str(), nullptr);
    ScheduleViewCheck(source);
    ScheduleViewCheck(destination);
}

void ProcessViewChecks(HWND defView) {
    IFolderView* folderView = GetDesktopFolderView();
    bool refresh = false;

    for (auto it = g_viewChecks.begin(); it != g_viewChecks.end();) {
        bool exists = PathExists(it->path);
        bool shown = false;
        if (folderView) {
            if (PITEMID_CHILD pidl = FindViewItemByPath(folderView, it->path)) {
                shown = true;
                CoTaskMemFree(pidl);
            }
        }

        if (shown == exists) {
            it = g_viewChecks.erase(it);
            continue;
        }

        if (++it->ticks <= kViewCheckNotifyTicks) {
            SHChangeNotify(exists ? SHCNE_CREATE : SHCNE_DELETE,
                           SHCNF_PATHW | SHCNF_FLUSHNOWAIT, it->path.c_str(),
                           nullptr);
            ++it;
        } else {
            Wh_Log(L"Desktop view out of date for %s (%s), refreshing",
                   it->path.c_str(), exists ? L"missing" : L"ghost");
            refresh = true;
            it = g_viewChecks.erase(it);
        }
    }

    if (refresh) {
        // Same as Refresh in the desktop's context menu; icon positions are
        // kept.
        if (IShellView* view = GetDesktopShellView()) {
            view->Refresh();
            view->Release();
        }
        g_viewChecks.clear();
    }

    if (g_viewChecks.empty()) {
        KillTimer(defView, kViewCheckTimerId);
    }
}

// Moves a shortcut out of its group onto the desktop, where the shell places
// it in the first free spot. Returns true on success.
bool MoveOutOfGroup(const std::wstring& path) {
    if (g_userDesktop.empty()) {
        return false;
    }

    std::wstring destination = UniqueFilePath(g_userDesktop, FileNameOf(path));
    if (!MoveFileExW(path.c_str(), destination.c_str(),
                     MOVEFILE_COPY_ALLOWED)) {
        Wh_Log(L"Failed to move %s to the desktop: %u", path.c_str(),
               GetLastError());
        return false;
    }

    NotifyMoved(path, destination);
    return true;
}

// Whether desktop groups and panel shortcuts open with a single click.
bool UseSingleClick() {
    switch (GetSettings().clickMode) {
        case ClickMode::Single:
            return true;
        case ClickMode::Double:
            return false;
        case ClickMode::System:
            break;
    }

    SHELLFLAGSTATE state = {};
    SHGetSettings(&state, SSF_DOUBLECLICKINWEBVIEW);
    return !state.fDoubleClickInWebView;
}

////////////////////////////////////////////////////////////////////////////////
// Group panel (desktop thread only)
//
// Double-clicking a group shows its shortcuts in a panel centered on the
// group's monitor, above a dimmed overlay. The panel uses the Windows 11
// acrylic backdrop when available; its content is rendered into a
// premultiplied BGRA buffer and copied to the window as is, so that the
// backdrop shows through the transparent parts.

constexpr wchar_t kPanelClass[] = L"WindhawkDesktopGroupPanel";
constexpr wchar_t kOverlayClass[] = L"WindhawkDesktopGroupOverlay";
constexpr UINT_PTR kPanelAnimTimerId = 1;
constexpr DWORD kPanelAnimMs = 180;
constexpr UINT kPanelAnimIntervalMs = 10;
// Gap between the group and a panel opened near it.
constexpr int kNearGap = 8;

// Reordering by dragging.
constexpr UINT_PTR kReorderTimerId = 2;
constexpr UINT kReorderIntervalMs = 15;
constexpr float kReorderEasing = 0.35f;
constexpr ULONGLONG kAutoScrollIntervalMs = 450;
constexpr int kPanelMinColumns = 3;
constexpr int kPanelMaxColumns = 4;
constexpr int kPanelMaxVisibleRows = 4;

// Hit-test results other than item indices.
constexpr int kHitNone = -1;
constexpr int kHitOpenFolder = -2;

// Context menu command IDs: ours, then the shell's range.
constexpr UINT kCmdRemoveFromGroup = 1;
constexpr UINT kCmdShellFirst = 0x100;
constexpr UINT kCmdShellLast = 0x7FFF;

// DWM attributes that older SDK headers may not define.
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmCornerRound = 2;
constexpr DWORD kDwmSystemBackdropType = 38;
constexpr DWORD kDwmBackdropTransientWindow = 3;

struct ItemPosition {
    float x;
    float y;
};

struct PanelItem {
    std::wstring path;
    std::wstring name;
    Image icon;
};

struct PanelLayout {
    int padding = 0;
    int titleHeight = 0;
    int buttonWidth = 0;
    int buttonHeight = 0;
    int cellWidth = 0;
    int cellHeight = 0;
    int iconSize = 0;
    int iconTop = 0;
    int labelGap = 0;
    int highlightRadius = 0;
    int columns = 1;
    int totalRows = 1;
    int visibleRows = 1;
    int contentTop = 0;
    int width = 0;
    int height = 0;
};

struct PanelState {
    HWND overlay = nullptr;
    HWND panel = nullptr;
    std::wstring folder;
    std::wstring title;
    std::vector<PanelItem> items;
    PanelLayout layout;
    float scale = 1.0f;
    bool dark = false;
    bool backdrop = false;
    HFONT titleFont = nullptr;
    HFONT labelFont = nullptr;
    int hover = kHitNone;
    int pressed = kHitNone;
    int focus = kHitNone;
    int scrollRow = 0;
    bool trackingMouse = false;
    bool closing = false;
    Image frame;
    RECT startRect = {};
    RECT finalRect = {};
    ULONGLONG animStart = 0;
    bool animating = false;
    // Final opacity of the dimming overlay.
    BYTE overlayAlpha = 0;

    // Reordering: armed on a button press on an item, dragging once the
    // pointer moved far enough.
    bool dragArmed = false;
    bool dragging = false;
    POINT dragStart = {};
    POINT dragPoint = {};
    POINT dragGrab = {};
    int dragIndex = -1;
    int dropSlot = -1;
    ULONGLONG lastAutoScroll = 0;
    // Animated cell positions (top-left) while items move; empty otherwise.
    std::vector<ItemPosition> itemPositions;
};

PanelState g_panel;
bool g_panelClassesRegistered;

// The shell context menu being tracked, for owner-drawn submenus.
IContextMenu2* g_menu2;
IContextMenu3* g_menu3;

// A panel request is queued from the list view notification and handled
// after it returns, so that the list view doesn't take the focus back.
UINT g_msgShowPanel;
bool g_panelPending;
std::wstring g_pendingPanelFolder;
RECT g_pendingPanelIconRect;
RECT g_pendingPanelItemRect;

HINSTANCE ModuleInstance() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

int Scaled(int value) {
    return static_cast<int>(std::lround(value * g_panel.scale));
}

HFONT CreateUiFont(int pixelHeight, int weight) {
    LOGFONTW font = {};
    font.lfHeight = -pixelHeight;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    // Grayscale antialiasing: the coverage becomes the alpha channel.
    font.lfQuality = ANTIALIASED_QUALITY;
    wcscpy_s(font.lfFaceName, L"Segoe UI Variable Text");
    HFONT result = CreateFontIndirectW(&font);
    if (!result) {
        wcscpy_s(font.lfFaceName, L"Segoe UI");
        result = CreateFontIndirectW(&font);
    }
    return result;
}

// Draws text into a premultiplied image: GDI renders white text on black,
// and the rendered coverage is used as the alpha of the requested color.
void DrawTextToImage(Image& dst,
                     const std::wstring& text,
                     const RECT& rect,
                     HFONT font,
                     uint32_t r,
                     uint32_t g,
                     uint32_t b,
                     UINT format) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0 || text.empty() || !font) {
        return;
    }

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // Top-down.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    HDC dc = CreateCompatibleDC(nullptr);
    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(dc);
        return;
    }

    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    HGDIOBJ oldFont = SelectObject(dc, font);
    memset(bits, 0, static_cast<size_t>(width) * height * 4);
    SetTextColor(dc, RGB(255, 255, 255));
    SetBkMode(dc, TRANSPARENT);

    RECT local = {0, 0, width, height};
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &local,
              format | DT_NOPREFIX);
    GdiFlush();

    const uint32_t* pixels = static_cast<const uint32_t*>(bits);
    for (int y = 0; y < height; y++) {
        int dy = rect.top + y;
        if (dy < 0 || dy >= dst.height) {
            continue;
        }
        for (int x = 0; x < width; x++) {
            int dx = rect.left + x;
            if (dx < 0 || dx >= dst.width) {
                continue;
            }
            uint32_t p = pixels[static_cast<size_t>(y) * width + x];
            uint32_t coverage = std::max(
                {p & 0xFF, (p >> 8) & 0xFF, (p >> 16) & 0xFF});
            if (!coverage) {
                continue;
            }
            BlendPixel(dst.pixels[static_cast<size_t>(dy) * dst.width + dx],
                       PackPixel((b * coverage + 127) / 255,
                                 (g * coverage + 127) / 255,
                                 (r * coverage + 127) / 255, coverage));
        }
    }

    SelectObject(dc, oldFont);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

std::wstring DisplayNameOf(const std::wstring& path) {
    std::wstring name = FileNameOf(path);
    if (HasShortcutExtension(name)) {
        name.resize(name.size() - wcslen(PathFindExtensionW(name.c_str())));
    }
    return name;
}

void ComputePanelLayout() {
    PanelLayout& l = g_panel.layout;
    int count = static_cast<int>(g_panel.items.size());

    l.padding = Scaled(20);
    l.titleHeight = Scaled(44);
    l.buttonWidth = Scaled(100);
    l.buttonHeight = Scaled(28);
    l.cellWidth = Scaled(96);
    l.cellHeight = Scaled(108);
    l.iconSize = Scaled(48);
    l.iconTop = Scaled(10);
    l.labelGap = Scaled(6);
    l.highlightRadius = Scaled(8);

    l.columns = std::clamp(count, kPanelMinColumns, kPanelMaxColumns);
    l.totalRows = std::max(1, (count + l.columns - 1) / l.columns);
    l.visibleRows = std::min(l.totalRows, kPanelMaxVisibleRows);
    l.contentTop = l.padding + l.titleHeight;
    l.width = 2 * l.padding + l.columns * l.cellWidth;
    l.height = l.contentTop + l.visibleRows * l.cellHeight + l.padding;
}

int MaxScrollRow() {
    return std::max(0, g_panel.layout.totalRows - g_panel.layout.visibleRows);
}

// Returns the item's cell in panel coordinates, or an empty rect if the item
// is scrolled out of view.
RECT ItemRect(int index) {
    const PanelLayout& l = g_panel.layout;
    int row = index / l.columns - g_panel.scrollRow;
    int column = index % l.columns;
    if (row < 0 || row >= l.visibleRows) {
        return RECT{};
    }
    int left = l.padding + column * l.cellWidth;
    int top = l.contentTop + row * l.cellHeight;
    return RECT{left, top, left + l.cellWidth, top + l.cellHeight};
}

// The item's cell as currently drawn: animated while (and just after)
// reordering, static otherwise.
RECT DisplayedItemRect(int index) {
    if (g_panel.itemPositions.size() != g_panel.items.size()) {
        return ItemRect(index);
    }
    const PanelLayout& l = g_panel.layout;
    int left = static_cast<int>(std::lround(g_panel.itemPositions[index].x));
    int top = static_cast<int>(std::lround(g_panel.itemPositions[index].y));
    return RECT{left, top, left + l.cellWidth, top + l.cellHeight};
}

RECT OpenButtonRect() {
    const PanelLayout& l = g_panel.layout;
    int right = l.width - l.padding;
    int top = l.padding + (l.titleHeight - l.buttonHeight) / 2 - Scaled(4);
    return RECT{right - l.buttonWidth, top, right, top + l.buttonHeight};
}

int PanelHitTest(POINT point) {
    RECT button = OpenButtonRect();
    if (PtInRect(&button, point)) {
        return kHitOpenFolder;
    }
    for (int i = 0; i < static_cast<int>(g_panel.items.size()); i++) {
        RECT cell = ItemRect(i);
        if (!IsRectEmpty(&cell) && PtInRect(&cell, point)) {
            return i;
        }
    }
    return kHitNone;
}

void RenderPanel() {
    const PanelLayout& l = g_panel.layout;
    Image& frame = g_panel.frame;
    frame.width = l.width;
    frame.height = l.height;
    frame.pixels.assign(static_cast<size_t>(l.width) * l.height, 0);

    bool dark = g_panel.dark;
    uint32_t text = dark ? 255 : 20;
    uint32_t highlight = dark ? 255 : 0;

    // Tint over the backdrop, or an almost opaque background without one.
    uint32_t tintAlpha = g_panel.backdrop ? 90 : 235;
    uint32_t tint = dark ? 32 : 243;
    uint32_t tintPixel =
        PackPixel((tint * tintAlpha + 127) / 255, (tint * tintAlpha + 127) / 255,
                  (tint * tintAlpha + 127) / 255, tintAlpha);
    std::fill(frame.pixels.begin(), frame.pixels.end(), tintPixel);

    // Title.
    RECT button = OpenButtonRect();
    RECT titleRect = {l.padding, l.padding, button.left - Scaled(12),
                      l.padding + l.titleHeight - Scaled(8)};
    DrawTextToImage(frame, g_panel.title, titleRect, g_panel.titleFont, text,
                    text, text,
                    DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // "Open folder" button.
    bool buttonHot = g_panel.hover == kHitOpenFolder;
    FillRoundedRect(frame, static_cast<float>(button.left),
                    static_cast<float>(button.top),
                    static_cast<float>(button.right),
                    static_cast<float>(button.bottom),
                    static_cast<float>(l.buttonHeight) / 2, highlight,
                    highlight, highlight, buttonHot ? 46 : 24);
    DrawTextToImage(frame, L"Open folder", button, g_panel.labelFont, text,
                    text, text, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Items.
    auto drawItem = [&](int i, const RECT& cell, uint32_t backgroundAlpha) {
        if (backgroundAlpha) {
            float inset = static_cast<float>(Scaled(4));
            FillRoundedRect(frame, cell.left + inset, cell.top + inset,
                            cell.right - inset, cell.bottom - inset,
                            static_cast<float>(l.highlightRadius), highlight,
                            highlight, highlight, backgroundAlpha);
        }

        const Image& icon = g_panel.items[i].icon;
        int iconLeft = cell.left + (l.cellWidth - l.iconSize) / 2;
        int iconTop = cell.top + l.iconTop;
        if (icon.width > 0) {
            BlendImage(frame, icon, iconLeft + (l.iconSize - icon.width) / 2,
                       iconTop + (l.iconSize - icon.height) / 2);
        }

        RECT label = {cell.left + Scaled(6),
                      iconTop + l.iconSize + l.labelGap,
                      cell.right - Scaled(6), cell.bottom - Scaled(6)};
        DrawTextToImage(frame, g_panel.items[i].name, label,
                        g_panel.labelFont, text, text, text,
                        DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL |
                            DT_END_ELLIPSIS);
    };

    bool animated = !g_panel.itemPositions.empty();
    int contentBottom = l.contentTop + l.visibleRows * l.cellHeight;
    for (int i = 0; i < static_cast<int>(g_panel.items.size()); i++) {
        if (g_panel.dragging && i == g_panel.dragIndex) {
            continue;  // Drawn last, on top.
        }

        RECT cell = animated ? DisplayedItemRect(i) : ItemRect(i);
        if (IsRectEmpty(&cell)) {
            continue;
        }
        if (animated) {
            // Moving items are shown while their center is in view.
            int centerY = (cell.top + cell.bottom) / 2;
            if (centerY < l.contentTop || centerY > contentBottom) {
                continue;
            }
        }

        uint32_t backgroundAlpha = 0;
        if (!g_panel.dragging) {
            bool hot = g_panel.hover == i;
            bool focused = g_panel.focus == i;
            if (hot || focused) {
                backgroundAlpha =
                    g_panel.pressed == i ? 52 : (hot ? 34 : 24);
            }
        }
        drawItem(i, cell, backgroundAlpha);
    }

    // The dragged item follows the pointer, "lifted" with a background.
    if (g_panel.dragging && g_panel.dragIndex >= 0) {
        int left = g_panel.dragPoint.x - g_panel.dragGrab.x;
        int top = g_panel.dragPoint.y - g_panel.dragGrab.y;
        RECT cell = {left, top, left + l.cellWidth, top + l.cellHeight};
        drawItem(g_panel.dragIndex, cell, 60);
    }

    // Scroll indicator.
    if (MaxScrollRow() > 0) {
        float trackTop = static_cast<float>(l.contentTop);
        float trackHeight = static_cast<float>(l.visibleRows * l.cellHeight);
        float thumbHeight = trackHeight * l.visibleRows / l.totalRows;
        float thumbTop = trackTop + (trackHeight - thumbHeight) *
                                        g_panel.scrollRow / MaxScrollRow();
        float right = static_cast<float>(l.width - Scaled(6));
        FillRoundedRect(frame, right - Scaled(4), thumbTop, right,
                        thumbTop + thumbHeight, static_cast<float>(Scaled(2)),
                        highlight, highlight, highlight, 90);
    }
}

void RedrawPanel() {
    if (!g_panel.panel) {
        return;
    }
    RenderPanel();
    InvalidateRect(g_panel.panel, nullptr, FALSE);
}

void PaintPanel(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);

    const Image& frame = g_panel.frame;
    if (frame.width > 0 && frame.height > 0) {
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = frame.width;
        info.bmiHeader.biHeight = -frame.height;  // Top-down.
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        RECT client;
        GetClientRect(hwnd, &client);
        if (client.right == frame.width && client.bottom == frame.height) {
            SetDIBitsToDevice(dc, 0, 0, frame.width, frame.height, 0, 0, 0,
                              frame.height, frame.pixels.data(), &info,
                              DIB_RGB_COLORS);
        } else {
            // During the open animation the window is smaller than the frame.
            SetStretchBltMode(dc, COLORONCOLOR);
            StretchDIBits(dc, 0, 0, client.right, client.bottom, 0, 0,
                          frame.width, frame.height, frame.pixels.data(),
                          &info, DIB_RGB_COLORS, SRCCOPY);
        }
    }

    EndPaint(hwnd, &ps);
}

void ClosePanel() {
    if (g_panel.closing || (!g_panel.overlay && !g_panel.panel)) {
        return;
    }
    g_panel.closing = true;

    // Destroying the owner (the overlay) destroys the owned panel as well.
    DestroyWindow(g_panel.overlay ? g_panel.overlay : g_panel.panel);

    if (g_panel.titleFont) {
        DeleteObject(g_panel.titleFont);
    }
    if (g_panel.labelFont) {
        DeleteObject(g_panel.labelFont);
    }
    g_panel = PanelState();
}

void LoadPanelItems(const std::vector<std::wstring>& order) {
    g_panel.items.clear();
    int iconSize = Scaled(48);
    for (const auto& name : order) {
        PanelItem item;
        item.path = g_panel.folder + L"\\" + name;
        item.name = DisplayNameOf(name);
        LoadItemImage(item.path, iconSize, item.icon);
        RoundSquareIcon(item.icon);
        g_panel.items.push_back(std::move(item));
    }
}

// Reloads the panel after the group's content changed, keeping the panel
// centered where it is.
void ReloadPanelItems() {
    if (!g_panel.panel) {
        return;
    }

    std::vector<std::wstring> order = BuildOrder(g_panel.folder);
    if (order.empty()) {
        ClosePanel();
        return;
    }

    LoadPanelItems(order);
    ComputePanelLayout();

    int count = static_cast<int>(g_panel.items.size());
    g_panel.scrollRow = std::min(g_panel.scrollRow, MaxScrollRow());
    if (g_panel.focus >= count) {
        g_panel.focus = count - 1;
    }
    g_panel.hover = kHitNone;
    g_panel.pressed = kHitNone;
    g_panel.dragArmed = false;
    g_panel.dragging = false;
    g_panel.dragIndex = -1;
    g_panel.dropSlot = -1;
    g_panel.itemPositions.clear();
    KillTimer(g_panel.panel, kReorderTimerId);

    RECT current;
    GetWindowRect(g_panel.panel, &current);
    int centerX = (current.left + current.right) / 2;
    int centerY = (current.top + current.bottom) / 2;
    int width = g_panel.layout.width;
    int height = g_panel.layout.height;
    int left = centerX - width / 2;
    int top = centerY - height / 2;

    // Keep the shrunk panel within the work area (relevant near the group).
    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    if (GetMonitorInfoW(MonitorFromRect(&current, MONITOR_DEFAULTTONEAREST),
                        &monitorInfo)) {
        const RECT& work = monitorInfo.rcWork;
        left = std::max<LONG>(work.left, std::min<LONG>(left, work.right - width));
        top = std::max<LONG>(work.top, std::min<LONG>(top, work.bottom - height));
    }
    g_panel.finalRect = {left, top, left + width, top + height};

    RenderPanel();
    SetWindowPos(g_panel.panel, nullptr, left, top, width, height,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(g_panel.panel, nullptr, FALSE);
}

void RemovePanelItem(int index) {
    if (index < 0 || index >= static_cast<int>(g_panel.items.size())) {
        return;
    }

    std::wstring folder = g_panel.folder;
    if (!MoveOutOfGroup(g_panel.items[index].path)) {
        MessageBeep(MB_ICONWARNING);
        return;
    }

    if (GetSettings().dissolveSingleItem && BuildOrder(folder).size() <= 1) {
        ClosePanel();
        DissolveGroup(folder);
        return;
    }

    UpdateGroupIcon(folder);
    ReloadPanelItems();
}

void ShowItemContextMenu(int index, POINT screenPoint) {
    if (index < 0 || index >= static_cast<int>(g_panel.items.size())) {
        return;
    }
    // The menu and the chosen command run modal loops. Counted before checking
    // for unloading, so that Wh_ModUninit can't miss the call.
    BusyScope busy;
    if (g_unloading) {
        return;
    }

    std::wstring path = g_panel.items[index].path;
    std::wstring folder = g_panel.folder;

    IContextMenu* contextMenu = nullptr;
    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                              IID_PPV_ARGS(&item)))) {
        item->BindToHandler(nullptr, BHID_SFUIObject,
                            IID_PPV_ARGS(&contextMenu));
        item->Release();
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        if (contextMenu) {
            contextMenu->Release();
        }
        return;
    }

    AppendMenuW(menu, MF_STRING, kCmdRemoveFromGroup, L"Remove from group");
    if (contextMenu) {
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        contextMenu->QueryContextMenu(menu, GetMenuItemCount(menu),
                                      kCmdShellFirst, kCmdShellLast,
                                      CMF_NORMAL);
        contextMenu->QueryInterface(IID_PPV_ARGS(&g_menu3));
        if (!g_menu3) {
            contextMenu->QueryInterface(IID_PPV_ARGS(&g_menu2));
        }
    }

    UINT command = TrackPopupMenuEx(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screenPoint.x, screenPoint.y,
        g_panel.panel, nullptr);

    if (g_menu3) {
        g_menu3->Release();
        g_menu3 = nullptr;
    }
    if (g_menu2) {
        g_menu2->Release();
        g_menu2 = nullptr;
    }
    DestroyMenu(menu);

    if (command == kCmdRemoveFromGroup) {
        RemovePanelItem(index);
    } else if (command >= kCmdShellFirst && contextMenu) {
        // The command may show its own windows (e.g. Properties), so the
        // panel gets out of the way first.
        ClosePanel();

        CMINVOKECOMMANDINFOEX invoke = {};
        invoke.cbSize = sizeof(invoke);
        // ASYNCOK: the shell may run the command (and its dialogs) on its own
        // thread.
        invoke.fMask =
            CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE | CMIC_MASK_ASYNCOK;
        invoke.hwnd = g_listView;
        invoke.lpVerb = MAKEINTRESOURCEA(command - kCmdShellFirst);
        invoke.lpVerbW = MAKEINTRESOURCEW(command - kCmdShellFirst);
        invoke.nShow = SW_SHOWNORMAL;
        invoke.ptInvoke = screenPoint;
        HRESULT hr = contextMenu->InvokeCommand(
            reinterpret_cast<CMINVOKECOMMANDINFO*>(&invoke));
        if (FAILED(hr)) {
            Wh_Log(L"Context menu command %u failed: 0x%08X",
                   command - kCmdShellFirst, hr);
        }

        // E.g. Delete removes the shortcut from the group.
        ScheduleGroupSync(folder);
    }

    if (contextMenu) {
        contextMenu->Release();
    }
}

// Opens a file or folder. ASYNCOK lets the shell run it (and any dialog, e.g.
// for a shortcut whose target is gone) on its own thread; the call is still
// tracked in case it blocks.
void ShellExecuteAsync(PCWSTR verb, const std::wstring& path) {
    // Counted before checking for unloading, so that Wh_ModUninit can't miss
    // the call.
    BusyScope busy;
    if (g_unloading) {
        return;
    }

    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_ASYNCOK;
    info.lpVerb = verb;
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"Failed to open %s: %u", path.c_str(), GetLastError());
    }
}

void LaunchPanelItem(int index) {
    if (index < 0 || index >= static_cast<int>(g_panel.items.size())) {
        return;
    }
    std::wstring path = g_panel.items[index].path;
    ClosePanel();
    ShellExecuteAsync(nullptr, path);
}

void OpenPanelFolder() {
    std::wstring folder = g_panel.folder;
    ClosePanel();
    ShellExecuteAsync(L"open", folder);
}

void EnsureFocusVisible() {
    if (g_panel.focus < 0) {
        return;
    }
    int row = g_panel.focus / g_panel.layout.columns;
    if (row < g_panel.scrollRow) {
        g_panel.scrollRow = row;
    } else if (row >= g_panel.scrollRow + g_panel.layout.visibleRows) {
        g_panel.scrollRow = row - g_panel.layout.visibleRows + 1;
    }
}

void MoveFocus(int delta) {
    int count = static_cast<int>(g_panel.items.size());
    if (!count) {
        return;
    }
    if (g_panel.focus < 0) {
        g_panel.focus = 0;
    } else {
        g_panel.focus = std::clamp(g_panel.focus + delta, 0, count - 1);
    }
    EnsureFocusVisible();
    RedrawPanel();
}

void StepPanelAnimation() {
    ULONGLONG elapsed = GetTickCount64() - g_panel.animStart;
    float t = std::min(1.0f, static_cast<float>(elapsed) / kPanelAnimMs);
    float eased = 1 - (1 - t) * (1 - t) * (1 - t);

    auto lerp = [&](LONG from, LONG to) {
        return static_cast<int>(std::lround(from + (to - from) * eased));
    };
    const RECT& a = g_panel.startRect;
    const RECT& b = g_panel.finalRect;
    int left = lerp(a.left, b.left);
    int top = lerp(a.top, b.top);
    int right = lerp(a.right, b.right);
    int bottom = lerp(a.bottom, b.bottom);

    SetWindowPos(g_panel.panel, nullptr, left, top, right - left, bottom - top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_panel.overlay) {
        SetLayeredWindowAttributes(
            g_panel.overlay, 0,
            static_cast<BYTE>(std::lround(g_panel.overlayAlpha * eased)),
            LWA_ALPHA);
    }
    InvalidateRect(g_panel.panel, nullptr, FALSE);

    if (t >= 1) {
        KillTimer(g_panel.panel, kPanelAnimTimerId);
        g_panel.animating = false;
    }
}

void UpdateHover(POINT point) {
    int hit = PanelHitTest(point);
    if (hit != g_panel.hover) {
        g_panel.hover = hit;
        RedrawPanel();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Reordering shortcuts in the panel by dragging them

// Returns the item index the pointer is over during a drag, as a position in
// the group (including rows scrolled out of view), clamped to the items.
int ReorderSlotAt(POINT point) {
    const PanelLayout& l = g_panel.layout;
    int count = static_cast<int>(g_panel.items.size());
    int x = static_cast<int>(point.x) - l.padding;
    int y = static_cast<int>(point.y) - l.contentTop;
    int column = std::clamp(x / std::max(1, l.cellWidth), 0, l.columns - 1);
    int row = std::clamp(y / std::max(1, l.cellHeight), 0,
                         l.visibleRows - 1) +
              g_panel.scrollRow;
    return std::clamp(row * l.columns + column, 0, count - 1);
}

// Where an item belongs while dragging: the other items close the dragged
// item's gap and open one at the drop slot.
RECT ReorderTargetRect(int index) {
    int display = index;
    if (index != g_panel.dragIndex) {
        int withoutDragged = index > g_panel.dragIndex ? index - 1 : index;
        display = withoutDragged >= g_panel.dropSlot ? withoutDragged + 1
                                                     : withoutDragged;
    }

    const PanelLayout& l = g_panel.layout;
    int row = display / l.columns - g_panel.scrollRow;
    int column = display % l.columns;
    int left = l.padding + column * l.cellWidth;
    int top = l.contentTop + row * l.cellHeight;
    return RECT{left, top, left + l.cellWidth, top + l.cellHeight};
}

void StartReorderTimer() {
    SetTimer(g_panel.panel, kReorderTimerId, kReorderIntervalMs, nullptr);
}

void BeginReorder(int index, POINT point) {
    g_panel.dragging = true;
    g_panel.dragIndex = index;
    g_panel.dropSlot = index;
    g_panel.dragPoint = point;
    g_panel.hover = kHitNone;
    g_panel.pressed = kHitNone;

    RECT cell = ItemRect(index);
    g_panel.dragGrab = {g_panel.dragStart.x - cell.left,
                        g_panel.dragStart.y - cell.top};

    // Animated positions start from the current layout.
    g_panel.itemPositions.clear();
    for (int i = 0; i < static_cast<int>(g_panel.items.size()); i++) {
        const PanelLayout& l = g_panel.layout;
        int row = i / l.columns - g_panel.scrollRow;
        int column = i % l.columns;
        g_panel.itemPositions.push_back(
            {static_cast<float>(l.padding + column * l.cellWidth),
             static_cast<float>(l.contentTop + row * l.cellHeight)});
    }

    StartReorderTimer();
    RedrawPanel();
}

void UpdateReorder(POINT point) {
    g_panel.dragPoint = point;
    g_panel.dropSlot = ReorderSlotAt(point);
    RedrawPanel();
}

// Ends a drag; the new order is saved if commit is true.
void EndReorder(bool commit) {
    if (!g_panel.dragging) {
        return;
    }
    // Cleared first: releasing the capture below re-enters through
    // WM_CAPTURECHANGED.
    g_panel.dragging = false;
    g_panel.dragArmed = false;
    if (GetCapture() == g_panel.panel) {
        ReleaseCapture();
    }

    int from = g_panel.dragIndex;
    int to = g_panel.dropSlot;
    g_panel.dragIndex = -1;

    if (commit && from >= 0 && to >= 0 && from != to) {
        Settings settings = GetSettings();
        int count = static_cast<int>(g_panel.items.size());
        int previewCount = settings.preview3x3 && count > 4 ? 9 : 4;

        std::vector<std::wstring> before;
        for (const auto& item : g_panel.items) {
            before.push_back(FileNameOf(item.path));
        }

        // Keep the animated positions in step with the items.
        PanelItem moved = std::move(g_panel.items[from]);
        g_panel.items.erase(g_panel.items.begin() + from);
        g_panel.items.insert(g_panel.items.begin() + to, std::move(moved));
        if (g_panel.itemPositions.size() == g_panel.items.size()) {
            ItemPosition position = g_panel.itemPositions[from];
            g_panel.itemPositions.erase(g_panel.itemPositions.begin() + from);
            g_panel.itemPositions.insert(g_panel.itemPositions.begin() + to,
                                         position);
        }

        std::vector<std::wstring> after;
        for (const auto& item : g_panel.items) {
            after.push_back(FileNameOf(item.path));
        }
        WriteIniValue(IniPathOf(g_panel.folder), kIniSection, L"Order",
                      JoinOrder(after));

        // The group icon only shows the first few shortcuts.
        size_t shown = std::min<size_t>(previewCount, after.size());
        if (!std::equal(before.begin(), before.begin() + shown, after.begin())) {
            UpdateGroupIcon(g_panel.folder);
        }

        g_panel.focus = to;
        Wh_Log(L"Reordered %s: %d -> %d", g_panel.folder.c_str(), from, to);
    }

    g_panel.dropSlot = -1;
    // Items glide to their final cells.
    StartReorderTimer();
    RedrawPanel();
}

// Moves the animated positions towards their targets and scrolls the panel
// when dragging near its top or bottom edge.
void StepReorderAnimation() {
    if (g_panel.dragging) {
        const PanelLayout& l = g_panel.layout;
        int contentBottom = l.contentTop + l.visibleRows * l.cellHeight;
        int edge = l.cellHeight / 3;
        ULONGLONG now = GetTickCount64();
        if (now - g_panel.lastAutoScroll >= kAutoScrollIntervalMs) {
            int direction = 0;
            if (g_panel.dragPoint.y < l.contentTop + edge) {
                direction = -1;
            } else if (g_panel.dragPoint.y > contentBottom - edge) {
                direction = 1;
            }
            int row = std::clamp(g_panel.scrollRow + direction, 0,
                                 MaxScrollRow());
            if (row != g_panel.scrollRow) {
                g_panel.scrollRow = row;
                g_panel.lastAutoScroll = now;
                g_panel.dropSlot = ReorderSlotAt(g_panel.dragPoint);
            }
        }
    }

    bool settled = true;
    for (int i = 0; i < static_cast<int>(g_panel.itemPositions.size()); i++) {
        RECT target = g_panel.dragging ? ReorderTargetRect(i) : ItemRect(i);
        if (!g_panel.dragging && IsRectEmpty(&target)) {
            // Scrolled out of view: compute where it would be.
            const PanelLayout& l = g_panel.layout;
            int row = i / l.columns - g_panel.scrollRow;
            target.left = l.padding + (i % l.columns) * l.cellWidth;
            target.top = l.contentTop + row * l.cellHeight;
        }
        ItemPosition& position = g_panel.itemPositions[i];
        float dx = target.left - position.x;
        float dy = target.top - position.y;
        if (std::fabs(dx) < 0.5f && std::fabs(dy) < 0.5f) {
            position.x = static_cast<float>(target.left);
            position.y = static_cast<float>(target.top);
            continue;
        }
        position.x += dx * kReorderEasing;
        position.y += dy * kReorderEasing;
        settled = false;
    }

    if (!g_panel.dragging && settled) {
        KillTimer(g_panel.panel, kReorderTimerId);
        g_panel.itemPositions.clear();
    }
    RedrawPanel();
}

LRESULT CALLBACK PanelWndProc(HWND hwnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (msg) {
        case WM_PAINT:
            PaintPanel(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_TIMER:
            if (wParam == kPanelAnimTimerId) {
                StepPanelAnimation();
                return 0;
            }
            if (wParam == kReorderTimerId) {
                StepReorderAnimation();
                return 0;
            }
            break;

        case WM_MOUSEMOVE: {
            if (g_panel.animating) {
                return 0;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (g_panel.dragging) {
                UpdateReorder(point);
                return 0;
            }
            if (g_panel.dragArmed && (wParam & MK_LBUTTON) &&
                g_panel.pressed >= 0 &&
                (abs(point.x - g_panel.dragStart.x) >
                     GetSystemMetrics(SM_CXDRAG) ||
                 abs(point.y - g_panel.dragStart.y) >
                     GetSystemMetrics(SM_CYDRAG))) {
                BeginReorder(g_panel.pressed, point);
                UpdateReorder(point);
                return 0;
            }
            if (!g_panel.trackingMouse) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hwnd, 0};
                g_panel.trackingMouse = TrackMouseEvent(&track);
            }
            UpdateHover({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            return 0;
        }

        case WM_MOUSELEAVE:
            g_panel.trackingMouse = false;
            if (g_panel.dragArmed || g_panel.dragging) {
                return 0;  // The mouse is captured.
            }
            g_panel.hover = kHitNone;
            g_panel.pressed = kHitNone;
            RedrawPanel();
            return 0;

        case WM_LBUTTONDOWN:
            if (!g_panel.animating && !g_panel.dragging) {
                POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                g_panel.pressed = PanelHitTest(point);
                if (g_panel.pressed >= 0) {
                    // A press on an item may become a drag.
                    g_panel.dragArmed = true;
                    g_panel.dragStart = point;
                    SetCapture(hwnd);
                }
                RedrawPanel();
            }
            return 0;

        case WM_CAPTURECHANGED:
            if (g_panel.dragging) {
                EndReorder(false);
            }
            g_panel.dragArmed = false;
            return 0;

        case WM_LBUTTONUP: {
            POINT upPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (g_panel.dragging) {
                // Dropping outside the panel cancels.
                RECT client;
                GetClientRect(hwnd, &client);
                EndReorder(PtInRect(&client, upPoint));
                return 0;
            }
            if (g_panel.dragArmed) {
                g_panel.dragArmed = false;
                if (GetCapture() == hwnd) {
                    ReleaseCapture();
                }
            }
            int hit = PanelHitTest(upPoint);
            int pressed = g_panel.pressed;
            g_panel.pressed = kHitNone;
            if (hit != pressed || hit == kHitNone) {
                RedrawPanel();
                return 0;
            }
            if (hit == kHitOpenFolder) {
                OpenPanelFolder();
            } else if (UseSingleClick()) {
                LaunchPanelItem(hit);
            } else {
                // Double-click mode: a single click selects.
                g_panel.focus = hit;
                RedrawPanel();
            }
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            if (g_panel.animating || UseSingleClick()) {
                return 0;
            }
            int hit = PanelHitTest({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (hit >= 0) {
                LaunchPanelItem(hit);
            } else if (hit == kHitOpenFolder) {
                OpenPanelFolder();
            }
            return 0;
        }

        case WM_CONTEXTMENU: {
            if (g_panel.animating || g_panel.dragging) {
                return 0;
            }
            int index;
            POINT point;
            if (lParam == -1) {
                // Keyboard: Shift+F10 or the menu key.
                index = g_panel.focus;
                if (index < 0) {
                    return 0;
                }
                EnsureFocusVisible();
                RedrawPanel();
                RECT cell = ItemRect(index);
                point = {(cell.left + cell.right) / 2,
                         (cell.top + cell.bottom) / 2};
                ClientToScreen(hwnd, &point);
            } else {
                point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                POINT client = point;
                ScreenToClient(hwnd, &client);
                index = PanelHitTest(client);
            }
            if (index >= 0) {
                g_panel.focus = index;
                RedrawPanel();
                ShowItemContextMenu(index, point);
            }
            return 0;
        }

        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
        case WM_MENUCHAR: {
            // Owner-drawn shell submenus (Open with, Send to).
            LRESULT result = 0;
            if (g_menu3 &&
                SUCCEEDED(g_menu3->HandleMenuMsg2(msg, wParam, lParam,
                                                  &result))) {
                return result;
            }
            if (g_menu2 &&
                SUCCEEDED(g_menu2->HandleMenuMsg(msg, wParam, lParam))) {
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            int row = std::clamp(g_panel.scrollRow + (delta > 0 ? -1 : 1), 0,
                                 MaxScrollRow());
            if (row != g_panel.scrollRow) {
                g_panel.scrollRow = row;
                POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &point);
                if (g_panel.dragging) {
                    g_panel.dropSlot = ReorderSlotAt(point);
                } else {
                    g_panel.hover = PanelHitTest(point);
                }
                RedrawPanel();
            }
            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    if (g_panel.dragging) {
                        EndReorder(false);
                    } else {
                        ClosePanel();
                    }
                    return 0;
                case VK_LEFT:
                    MoveFocus(-1);
                    return 0;
                case VK_RIGHT:
                    MoveFocus(1);
                    return 0;
                case VK_UP:
                    MoveFocus(-g_panel.layout.columns);
                    return 0;
                case VK_DOWN:
                    MoveFocus(g_panel.layout.columns);
                    return 0;
                case VK_RETURN:
                case VK_SPACE:
                    LaunchPanelItem(g_panel.focus);
                    return 0;
            }
            break;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && !g_panel.closing) {
                // Closing directly would destroy the window while it's being
                // deactivated.
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
            break;

        case WM_CLOSE:
            ClosePanel();
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            ClosePanel();
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterPanelClasses() {
    if (g_panelClassesRegistered) {
        return true;
    }

    WNDCLASSEXW panelClass = {sizeof(panelClass)};
    panelClass.style = CS_DBLCLKS;
    panelClass.lpfnWndProc = PanelWndProc;
    panelClass.hInstance = ModuleInstance();
    panelClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    panelClass.lpszClassName = kPanelClass;

    WNDCLASSEXW overlayClass = {sizeof(overlayClass)};
    overlayClass.lpfnWndProc = OverlayWndProc;
    overlayClass.hInstance = ModuleInstance();
    overlayClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    overlayClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    overlayClass.lpszClassName = kOverlayClass;

    if (!RegisterClassExW(&panelClass)) {
        Wh_Log(L"Failed to register the panel class: %u", GetLastError());
        return false;
    }
    if (!RegisterClassExW(&overlayClass)) {
        Wh_Log(L"Failed to register the overlay class: %u", GetLastError());
        UnregisterClassW(kPanelClass, ModuleInstance());
        return false;
    }

    g_panelClassesRegistered = true;
    return true;
}

void UnregisterPanelClasses() {
    if (!g_panelClassesRegistered) {
        return;
    }
    UnregisterClassW(kPanelClass, ModuleInstance());
    UnregisterClassW(kOverlayClass, ModuleInstance());
    g_panelClassesRegistered = false;
}

void ApplyPanelWindowStyle(HWND hwnd) {
    BOOL dark = g_panel.dark;
    DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));

    DWORD corner = kDwmCornerRound;
    DwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));

    // The whole client area is "glass": pixel alpha decides what's opaque.
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    DWORD backdrop = kDwmBackdropTransientWindow;
    g_panel.backdrop = SUCCEEDED(DwmSetWindowAttribute(
        hwnd, kDwmSystemBackdropType, &backdrop, sizeof(backdrop)));
}

// Places a panel of the given size near a group: below it, or above it, or
// beside it, whichever fits first, always within the work area.
RECT PanelRectNearGroup(const RECT& item, int width, int height,
                        const RECT& work) {
    int gap = Scaled(kNearGap);
    auto clampX = [&](int x) {
        return std::max<LONG>(work.left, std::min<LONG>(x, work.right - width));
    };
    auto clampY = [&](int y) {
        return std::max<LONG>(work.top, std::min<LONG>(y, work.bottom - height));
    };
    int centerX = (item.left + item.right) / 2;
    int centerY = (item.top + item.bottom) / 2;

    int left;
    int top;
    if (item.bottom + gap + height <= work.bottom) {
        left = clampX(centerX - width / 2);
        top = item.bottom + gap;
    } else if (item.top - gap - height >= work.top) {
        left = clampX(centerX - width / 2);
        top = item.top - gap - height;
    } else if (item.right + gap + width <= work.right) {
        left = item.right + gap;
        top = clampY(centerY - height / 2);
    } else if (item.left - gap - width >= work.left) {
        left = item.left - gap - width;
        top = clampY(centerY - height / 2);
    } else {
        left = clampX(work.left + (work.right - work.left - width) / 2);
        top = clampY(work.top + (work.bottom - work.top - height) / 2);
    }
    return RECT{left, top, left + width, top + height};
}

// Creates the dimming overlay behind a centered panel, or none for 0%.
HWND CreatePanelOverlay(int dimming, const RECT& screen) {
    if (dimming <= 0) {
        return nullptr;
    }

    HWND overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kOverlayClass, L"", WS_POPUP, screen.left, screen.top,
        screen.right - screen.left, screen.bottom - screen.top, nullptr,
        nullptr, ModuleInstance(), nullptr);
    if (!overlay) {
        Wh_Log(L"Failed to create the overlay: %u", GetLastError());
        return nullptr;
    }

    // Faded in by the open animation.
    g_panel.overlayAlpha =
        static_cast<BYTE>(std::lround(std::min(dimming, 100) * 255 / 100.0));
    SetLayeredWindowAttributes(overlay, 0, 0, LWA_ALPHA);
    return overlay;
}

bool ShowGroupPanel(const std::wstring& folder,
                    RECT iconRect,
                    RECT itemRect) {
    ClosePanel();
    if (!RegisterPanelClasses()) {
        return false;
    }

    // Pick up changes made by other means, e.g. in File Explorer.
    SyncGroup(folder, false);

    std::vector<std::wstring> order = BuildOrder(folder);
    if (order.empty()) {
        return false;
    }

    // The panel opens on the group's monitor, which may have its own scale
    // factor (the desktop list view spans all monitors with a single DPI).
    HMONITOR monitor = MonitorFromRect(&iconRect, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) ||
        !dpiX) {
        dpiX = 96;
    }
    g_panel.scale = dpiX / 96.0f;
    g_panel.dark = IsAppsDarkMode();
    g_panel.folder = folder;
    g_panel.title = FileNameOf(folder);
    g_panel.titleFont = CreateUiFont(Scaled(18), FW_SEMIBOLD);
    g_panel.labelFont = CreateUiFont(Scaled(12), FW_NORMAL);

    LoadPanelItems(order);
    ComputePanelLayout();

    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    GetMonitorInfoW(monitor, &monitorInfo);
    const RECT& work = monitorInfo.rcWork;
    const RECT& screen = monitorInfo.rcMonitor;

    Settings settings = GetSettings();
    bool nearGroup = settings.panelPosition == PanelPosition::Near &&
                !IsRectEmpty(&itemRect);

    int width = g_panel.layout.width;
    int height = g_panel.layout.height;
    if (nearGroup) {
        g_panel.finalRect = PanelRectNearGroup(itemRect, width, height, work);
    } else {
        int left = work.left + (work.right - work.left - width) / 2;
        int top = work.top + (work.bottom - work.top - height) / 2;
        g_panel.finalRect = {left, top, left + width, top + height};
    }
    g_panel.startRect = iconRect;
    if (IsRectEmpty(&g_panel.startRect)) {
        g_panel.startRect = g_panel.finalRect;
    }

    // Near the group the panel behaves like a menu: no background effect.
    if (!nearGroup) {
        g_panel.overlay = CreatePanelOverlay(settings.dimming, screen);
    }

    const RECT& start = g_panel.startRect;
    g_panel.panel = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kPanelClass, g_panel.title.c_str(),
        WS_POPUP, start.left, start.top, start.right - start.left,
        start.bottom - start.top, g_panel.overlay, nullptr, ModuleInstance(),
        nullptr);
    if (!g_panel.panel) {
        Wh_Log(L"Failed to create the panel: %u", GetLastError());
        ClosePanel();
        return false;
    }

    ApplyPanelWindowStyle(g_panel.panel);
    RenderPanel();

    if (g_panel.overlay) {
        ShowWindow(g_panel.overlay, SW_SHOWNOACTIVATE);
    }
    ShowWindow(g_panel.panel, SW_SHOW);
    SetForegroundWindow(g_panel.panel);
    SetFocus(g_panel.panel);

    g_panel.animStart = GetTickCount64();
    g_panel.animating = true;
    SetTimer(g_panel.panel, kPanelAnimTimerId, kPanelAnimIntervalMs, nullptr);
    StepPanelAnimation();

    Wh_Log(L"Opened panel for %s (%d items, backdrop %d)", folder.c_str(),
           static_cast<int>(g_panel.items.size()), g_panel.backdrop);
    return true;
}

// Returns the file system path of a desktop list view item.
std::wstring GetViewItemPath(int index) {
    IFolderView* folderView = GetDesktopFolderView();
    if (!folderView || index < 0) {
        return std::wstring();
    }

    PITEMID_CHILD pidl = nullptr;
    if (FAILED(folderView->Item(index, &pidl)) || !pidl) {
        return std::wstring();
    }

    std::wstring path;
    IShellFolder* folder = nullptr;
    if (SUCCEEDED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
        path = GetItemPath(folder, pidl);
        folder->Release();
    }
    CoTaskMemFree(pidl);
    return path;
}

// Handles a double-click or activation of a desktop item. Returns true if the
// item is a group and the panel takes over; the notification must then not
// reach the default handler, which would open the folder.
// For a click (as opposed to an activation, which can come from the keyboard)
// the index must be the clicked item: it's -1 for a click on empty space, and
// falling back to the focused item would open a selected group.
bool HandleItemActivation(HWND defView,
                          int index,
                          UINT keyFlags,
                          bool isClick) {
    if (!GetSettings().openInPanel || !g_listView) {
        return false;
    }

    // Shift (or any modifier) opens the group as a regular folder.
    if (keyFlags & (LVKF_SHIFT | LVKF_CONTROL | LVKF_ALT)) {
        return false;
    }

    if (index < 0 && !isClick) {
        index = ListView_GetNextItem(g_listView, -1, LVNI_FOCUSED);
    }
    if (index < 0 || ListView_GetSelectedCount(g_listView) > 1) {
        return false;
    }

    std::wstring path = GetViewItemPath(index);
    if (path.empty() || !IsGroupFolder(path)) {
        return false;
    }

    // A double-click sends both NM_DBLCLK and LVN_ITEMACTIVATE; the panel is
    // shown once.
    if (g_panelPending || g_panel.panel) {
        return true;
    }

    RECT iconRect = {};
    if (ListView_GetItemRect(g_listView, index, &iconRect, LVIR_ICON)) {
        MapWindowPoints(g_listView, nullptr,
                        reinterpret_cast<POINT*>(&iconRect), 2);
    }
    // Icon and label, so that a panel near the group doesn't cover them.
    RECT itemRect = {};
    if (ListView_GetItemRect(g_listView, index, &itemRect, LVIR_BOUNDS)) {
        MapWindowPoints(g_listView, nullptr,
                        reinterpret_cast<POINT*>(&itemRect), 2);
    }

    g_panelPending = true;
    g_pendingPanelFolder = path;
    g_pendingPanelIconRect = iconRect;
    g_pendingPanelItemRect = itemRect;
    PostMessageW(defView, g_msgShowPanel, 0, 0);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Install / uninstall (desktop thread)

// Returns the list view and its ancestors up to (not including) the desktop
// window, nearest first.
std::vector<HWND> GetDropCandidates(HWND listView) {
    std::vector<HWND> result;
    HWND desktop = GetDesktopWindow();
    for (HWND hwnd = listView; hwnd && hwnd != desktop;
         hwnd = GetAncestor(hwnd, GA_PARENT)) {
        result.push_back(hwnd);
    }
    return result;
}

void InstallOnDesktopThread(HWND defView) {
    if (g_wrapper) {
        g_installed = true;
        return;
    }

    g_installAttempts++;

    HWND listView = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (!listView) {
        return;
    }

    // OLE resolves a drop by walking up from the window under the cursor to
    // the first registered ancestor, so the desktop's registration may live
    // on the list view, the DefView window, or one of its parents (Progman /
    // WorkerW, owned by the desktop browser).
    std::vector<HWND> candidates = GetDropCandidates(listView);

    // Prefer the registered pointer exposed as a window property.
    HWND propHwnd = nullptr;
    IDropTarget* original = nullptr;
    for (HWND hwnd : candidates) {
        auto* prop = static_cast<IDropTarget*>(GetPropW(hwnd, kDropTargetProp));
        if (g_installAttempts == 1) {
            wchar_t className[64] = L"";
            GetClassNameW(hwnd, className, ARRAYSIZE(className));
            Wh_Log(L"Candidate %p (%s): drop target property %p", hwnd,
                   className, prop);
        }
        if (prop && !original) {
            prop->AddRef();
            original = prop;
            propHwnd = hwnd;
        }
    }

    // Fall back to the shell view's own drop target.

    if (!original) {
        IShellView* view = GetDesktopShellView();
        if (!view) {
            Wh_Log(L"Desktop shell view not found");
            return;
        }

        HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&original));
        view->Release();
        if (FAILED(hr) || !original) {
            Wh_Log(L"Desktop shell view has no IDropTarget: 0x%08X", hr);
            return;
        }
    }

    // The wrapper holds its own reference, keeping the original alive across
    // the revoke below.
    GroupingDropTarget* wrapper = new GroupingDropTarget(original);
    original->Release();

    // Revoke the registration from whichever window holds it. A window
    // without a registration returns DRAGDROP_E_NOTREGISTERED and is left
    // untouched.
    HWND dropHwnd = nullptr;
    HRESULT hr = DRAGDROP_E_NOTREGISTERED;
    if (propHwnd) {
        hr = RevokeDragDrop(propHwnd);
        if (SUCCEEDED(hr)) {
            dropHwnd = propHwnd;
        }
    } else {
        for (HWND hwnd : candidates) {
            hr = RevokeDragDrop(hwnd);
            if (SUCCEEDED(hr)) {
                dropHwnd = hwnd;
                break;
            }
            if (hr != DRAGDROP_E_NOTREGISTERED) {
                break;
            }
        }
    }

    if (!dropHwnd) {
        Wh_Log(L"RevokeDragDrop failed: 0x%08X", hr);
        wrapper->Release();
        return;
    }

    hr = RegisterDragDrop(dropHwnd, wrapper);
    if (FAILED(hr)) {
        Wh_Log(L"RegisterDragDrop failed: 0x%08X", hr);
        RegisterDragDrop(dropHwnd, wrapper->Inner());
        wrapper->Release();
        return;
    }

    g_wrapper = wrapper;
    g_dropHwnd = dropHwnd;
    g_listView = listView;
    g_userDesktop = GetKnownFolder(FOLDERID_Desktop);
    g_publicDesktop = GetKnownFolder(FOLDERID_PublicDesktop);
    GetDesktopFolderView();
    g_installed = true;

    wchar_t className[64] = L"";
    GetClassNameW(dropHwnd, className, ARRAYSIZE(className));
    Wh_Log(L"Installed on window %p (%s) via %s", dropHwnd, className,
           propHwnd ? L"window property" : L"shell view");

    RefreshAllGroupIcons();
}

void ReleaseDesktopState(HWND defView, bool restoreOriginal) {
    ClosePanel();
    g_panelPending = false;

    KillTimer(defView, kPositionTimerId);
    g_pendingPositions.clear();
    KillTimer(defView, kSyncTimerId);
    g_pendingSyncs.clear();
    KillTimer(defView, kViewCheckTimerId);
    g_viewChecks.clear();

    if (g_wrapper) {
        g_wrapper->SetHighlight(-1);

        if (g_dropHwnd && IsWindow(g_dropHwnd)) {
            IDropTarget* original = g_wrapper->Inner();
            original->AddRef();
            RevokeDragDrop(g_dropHwnd);
            if (restoreOriginal) {
                RegisterDragDrop(g_dropHwnd, original);
            }
            original->Release();
        }

        g_wrapper->Release();
        g_wrapper = nullptr;
    }

    if (g_folderView) {
        g_folderView->Release();
        g_folderView = nullptr;
    }

    g_listView = nullptr;
    g_dropHwnd = nullptr;
    g_installed = false;
}

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    if (uMsg == g_msgInstall && g_msgInstall) {
        InstallOnDesktopThread(hWnd);
        return 0;
    }

    if (uMsg == g_msgUninstall && g_msgUninstall) {
        ReleaseDesktopState(hWnd, true);
        return 0;
    }

    if (uMsg == g_msgRefreshIcons && g_msgRefreshIcons) {
        if (g_wrapper) {
            RefreshAllGroupIcons();
        }
        return 0;
    }

    if (uMsg == g_msgShowPanel && g_msgShowPanel) {
        if (g_panelPending) {
            g_panelPending = false;
            ShowGroupPanel(g_pendingPanelFolder, g_pendingPanelIconRect,
                           g_pendingPanelItemRect);
        }
        return 0;
    }

    switch (uMsg) {
        case WM_NOTIFY: {
            auto* header = reinterpret_cast<NMHDR*>(lParam);
            if (!g_wrapper || !header || header->hwndFrom != g_listView) {
                break;
            }
            auto* activate = reinterpret_cast<NMITEMACTIVATE*>(lParam);
            if (header->code == NM_DBLCLK ||
                header->code == LVN_ITEMACTIVATE) {
                if (HandleItemActivation(hWnd, activate->iItem,
                                         activate->uKeyFlags, false)) {
                    return 0;
                }
            } else if (header->code == NM_CLICK && UseSingleClick()) {
                // Only a click on the item itself (icon or label) opens it.
                LVHITTESTINFO hitTest = {};
                hitTest.pt = activate->ptAction;
                int hit = ListView_HitTest(g_listView, &hitTest);
                if (hit >= 0 && hit == activate->iItem &&
                    (hitTest.flags & LVHT_ONITEM)) {
                    // Selection still happens as usual, so the click isn't
                    // swallowed.
                    HandleItemActivation(hWnd, hit, activate->uKeyFlags,
                                         true);
                }
            }
            break;
        }

        case WM_TIMER:
            if (wParam == kPositionTimerId) {
                ProcessPendingPositions(hWnd);
                return 0;
            }
            if (wParam == kViewCheckTimerId) {
                ProcessViewChecks(hWnd);
                return 0;
            }
            if (wParam == kSyncTimerId) {
                ProcessPendingSyncs(hWnd);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            ReleaseDesktopState(hWnd, false);
            g_defView = nullptr;
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Watch thread: finds the desktop window and (re)installs when needed.

bool IsOwnProcessWindow(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId == GetCurrentProcessId();
}

HWND FindDesktopDefView() {
    HWND defView = nullptr;

    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    if (!defView) {
        HWND worker = nullptr;
        while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr))) {
            defView =
                FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
            if (defView) {
                break;
            }
        }
    }

    return defView && IsOwnProcessWindow(defView) ? defView : nullptr;
}

// Whether another process owns the desktop, i.e. this is a separate File
// Explorer process where the desktop never appears.
bool DesktopInOtherProcess() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    return progman && !IsOwnProcessWindow(progman);
}

DWORD WINAPI WatchThreadProc(LPVOID) {
    int attempts = 0;
    bool appsDark = IsAppsDarkMode();

    // Signaled by the registry when the Windows theme may have changed.
    HKEY personalizeKey = nullptr;
    HANDLE themeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    auto watchTheme = [&] {
        return personalizeKey &&
               RegNotifyChangeKeyValue(personalizeKey, FALSE,
                                       REG_NOTIFY_CHANGE_LAST_SET, themeEvent,
                                       TRUE) == ERROR_SUCCESS;
    };
    bool themeWatched =
        themeEvent &&
        RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_NOTIFY | KEY_QUERY_VALUE, &personalizeKey) == ERROR_SUCCESS &&
        watchTheme();

    while (true) {
        HWND defView = g_defView.load();
        if (defView && !IsWindow(defView)) {
            g_defView = nullptr;
            defView = nullptr;
        }

        if (!defView) {
            HWND found = FindDesktopDefView();
            if (found && WindhawkUtils::SetWindowSubclassFromAnyThread(
                             found, DefViewSubclassProc, 0)) {
                g_defView = found;
                defView = found;
                attempts = 0;
            }
        }

        // Posted (not sent) so that the install code may make COM calls.
        if (defView && !g_installed && attempts < kInstallMaxAttempts) {
            attempts++;
            PostMessageW(defView, g_msgInstall, 0, 0);
        }

        // Look for the desktop every second until it's installed; then only
        // check now and then that it still exists. Separate File Explorer
        // processes never get the desktop, so they check rarely.
        DWORD timeout = kWatchIntervalMs;
        if (g_installed) {
            timeout = kWatchInstalledIntervalMs;
        } else if (!defView && DesktopInOtherProcess()) {
            timeout = kWatchOtherProcessIntervalMs;
        }

        HANDLE handles[] = {g_stopEvent, themeEvent};
        DWORD result = WaitForMultipleObjects(themeWatched ? 2 : 1, handles,
                                              FALSE, timeout);
        if (result == WAIT_OBJECT_0) {
            break;
        }

        if (result == WAIT_OBJECT_0 + 1) {
            themeWatched = watchTheme();
            // Group icons following the Windows theme are re-rendered when
            // the theme changes.
            bool nowDark = IsAppsDarkMode();
            if (nowDark != appsDark) {
                appsDark = nowDark;
                HWND target = g_defView.load();
                if (target && g_installed &&
                    GetSettings().previewTheme == PreviewTheme::System) {
                    PostMessageW(target, g_msgRefreshIcons, 0, 0);
                }
            }
        }
    }

    if (personalizeKey) {
        RegCloseKey(personalizeKey);
    }
    if (themeEvent) {
        CloseHandle(themeEvent);
    }
    return 0;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Windhawk entry points

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_msgInstall =
        RegisterWindowMessageW(L"Windhawk_DesktopShortcutGroups_Install");
    g_msgUninstall =
        RegisterWindowMessageW(L"Windhawk_DesktopShortcutGroups_Uninstall");
    g_msgShowPanel =
        RegisterWindowMessageW(L"Windhawk_DesktopShortcutGroups_ShowPanel");
    g_msgRefreshIcons =
        RegisterWindowMessageW(L"Windhawk_DesktopShortcutGroups_RefreshIcons");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_watchThread = CreateThread(nullptr, 0, WatchThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_watchThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_watchThread) {
        WaitForSingleObject(g_watchThread, INFINITE);
        CloseHandle(g_watchThread);
        g_watchThread = nullptr;
    }

    // Calls that run a modal loop on the desktop thread (e.g. a dialog opened
    // from the panel) must return before the mod is unloaded.
    if (g_busyCalls > 0) {
        Wh_Log(L"Waiting for %d call(s) in progress", g_busyCalls.load());
        while (g_busyCalls > 0) {
            Sleep(100);
        }
    }

    HWND defView = g_defView.exchange(nullptr);
    if (defView && IsWindow(defView)) {
        SendMessageW(defView, g_msgUninstall, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(defView,
                                                         DefViewSubclassProc);
    }

    // The panel windows were destroyed by the uninstall message above.
    UnregisterPanelClasses();

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    Settings previous = GetSettings();
    LoadSettings();
    Settings current = GetSettings();

    // Group icons are rendered on the desktop thread.
    if (previous.previewTheme != current.previewTheme ||
        previous.preview3x3 != current.preview3x3) {
        HWND defView = g_defView.load();
        if (defView) {
            PostMessageW(defView, g_msgRefreshIcons, 0, 0);
        }
    }
}
