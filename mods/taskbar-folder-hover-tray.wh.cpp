// ==WindhawkMod==
// @id              taskbar-folder-hover-tray
// @name            Taskbar Folder Hover Tray
// @description     Adds folder shortcut buttons flush inside the Windows 11 taskbar app icons. Hovering one instantly opens a grid of the folder's contents that you can move into and click.
// @version         1.16
// @author          Kiploom
// @github          https://github.com/Kiploom
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -lgdiplus -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folder Hover Tray

Adds one or more **folder shortcut buttons** directly into the Windows 11 taskbar,
sitting flush with your pinned and running app icons. **Hover** a button and a grid
of that folder's contents appears instantly above the taskbar - move your mouse
straight up and click an item. No clicking the taskbar first.

Windows 11 only.

![Hover tray with cascading subfolders](https://raw.githubusercontent.com/Kiploom/images/main/taskbar-folder-hover-tray-cascade.png)

![Folder grid above the taskbar](https://raw.githubusercontent.com/Kiploom/images/main/taskbar-folder-hover-tray-grid.png)

## Features

- Buttons are sized from a live taskbar button, so they match your app icons
  exactly at any taskbar size and DPI.
- The hover grid opens with **zero delay** - folder contents and icons are scanned
  and cached on a background thread.
- A "corridor" between the button and the grid keeps the grid open while you move
  your mouse up to it, so there is no dead zone.
- Subfolders cascade: rest on a subfolder and its own grid opens beside the first
  one, as deep as you like. **Maximum folder depth** caps the nesting - `-1` is
  unlimited, `0` stops subfolders from opening on hover, `1` allows one level.
- Subfolders are listed first in every grid and carry a folder badge in the
  accent colour on the corner of their icon, so what cascades is obvious at a
  glance. Folder shortcuts (`.lnk` pointing at a folder) cascade the same way.
- Set each button's icon to an **emoji**, an **`.ico` / `.png` file**, or an
  **`app.exe,0`** icon resource. Leave it empty to use the folder's own icon.
- Left click an item to open it, right click for the full Windows shell context menu.
- Folder buttons are injected on the primary taskbar and every secondary-monitor taskbar.

## Setting up a folder

Each record in the **Folder shortcuts** setting has three fields:

| Field | Example | Notes |
|-------|---------|-------|
| Name  | `Apps` | Shown as the title at the top of the hover grid |
| Folder path | `%USERPROFILE%\Desktop` | Environment variables are expanded. `shell:` targets such as `shell:Desktop` also work. |
| Icon | an emoji, `C:\icons\apps.ico`, or `C:\Windows\explorer.exe,0` | Leave empty to use the folder's own icon |

A good setup is to make a folder somewhere, fill it with shortcuts to the apps
you want grouped, and point a button at it.

## How it is positioned

The Windows 11 taskbar app strip is a WinUI `ItemsRepeater` that refuses to lay out
any child it did not create itself, so this mod cannot literally become a taskbar
item. Instead it adds itself as an overlay to the taskbar's root grid and widens the
margin of the neighbouring app icon to carve out a real gap in the strip, then keeps
itself seated in that gap on every layout pass. The taskbar re-centers around the
gap, so the result reads as flush.

Two consequences worth knowing:

- The buttons cannot be drag-reordered like real taskbar items.
- They do not collapse into the overflow button when the taskbar gets full.

## Related mod: Taskbar Folder Menus

[Taskbar Folder Menus](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-folder-menus.wh.cpp)
(`taskbar-folder-menus` by sb4ssman) is another Windows 11 explorer.exe mod for
browsing folders from the taskbar. Both support configurable folder/shortcut
buttons, `shell:` targets and environment-variable expansion, subfolders that
expand on hover (cascade), shell context menus on items, and browsing folder
contents without minimizing windows.

They differ in placement and UI:

| | Taskbar Folder Hover Tray (this) | Taskbar Folder Menus |
|---|---|---|
| Placement | Carved gap inside the **app icon strip** (flush with pinned/running apps) | Next to the **system tray** / notification area |
| Open | **Hover** opens immediately | **Click** opens |
| UI | Custom-drawn **icon grid** popup | **Native Shell** cascading menus |
| Buttons | Sized from live taskbar app buttons; emoji/custom icons optional | Compact tray buttons with emoji labels and extensive tray grid layout options |

Prefer **Taskbar Folder Menus** if you want tray-side buttons and native Shell
menus. Prefer **this mod** if you want a hover-opened icon grid seated in the
app icon strip.

## Known Conflicts

**Taskbar Fluent Media Player** uses the same gap-carving technique when its
position is one of `taskbar_left_start`, `taskbar_right_start`,
`taskbar_after_search_*`, `taskbar_after_taskview_*` or `taskbar_after_widgets_*`.
If its anchor happens to match the one picked here, the two mods will fight over
the same margin - use the **Anchor** setting to move to a different neighbour. Its
`taskbar_*_edge` positions are plain overlays that never touch a margin, so those
do not conflict.

**Windows 11 Taskbar Styler** can restyle these buttons if one of its rules matches
`Button`. If a folder button suddenly looks wrong, check for a broad rule there.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folders:
  - - name: Apps
      $name: Name
      $description: Shown as the title at the top of the main hover grid.
    - path: '%USERPROFILE%\Desktop'
      $name: Folder path
      $description: >-
        A folder path. Environment variables such as %USERPROFILE% are expanded.
        Shell targets such as shell:Desktop also work.
    - icon: ''
      $name: Icon
      $description: >-
        An emoji, a path to an .ico or .png file, or an icon resource such as
        C:\Windows\explorer.exe,0. Leave empty to use the folder's own icon.
  $name: Folder shortcuts
  $description: One record per taskbar button. Add or remove records to add or remove buttons.

- position: beforeApps
  $name: Position
  $description: Which side of the app icons the buttons sit on.
  $options:
  - beforeApps: Before the app icons
  - afterApps: After the app icons

- anchor: firstApp
  $name: Anchor
  $description: >-
    Which taskbar element the gap is carved next to. Change this if another mod is
    fighting for the same anchor. Auto picks the outermost app icon.
  $options:
  - firstApp: Auto (outermost app icon)
  - widgets: Widgets button
  - taskView: Task View button
  - start: Start button

- hoverDelayMs: 0
  $name: Hover delay (ms)
  $description: Delay before the grid appears. 0 is instant.

- closeDelayMs: 250
  $name: Close delay (ms)
  $description: >-
    How long the grid stays open after the mouse leaves the button and the grid.
    Moving onto another taskbar icon counts as leaving.

- columns: 0
  $name: Grid columns
  $description: 0 chooses a square-ish grid automatically.

- maxItems: 60
  $name: Maximum items
  $description: >-
    Limit how many entries the grid shows. 0 means unlimited, but that is only
    sensible for small folders — the grid is also clipped to what fits on the
    monitor, so extras are not shown.

- includeSubfolders: true
  $name: Show subfolders
  $description: Include subfolders as entries in the grid.

- maxFolderDepth: -1
  $name: Maximum folder depth
  $description: >-
    How many levels of subfolder menus can cascade when you hover a subfolder.
    -1 is unlimited, 0 means subfolders never open on hover, 1 allows one level
    of subfolders, and so on. Turn off "Show subfolders" to hide them entirely.

- submenuDelayMs: 150
  $name: Subfolder open delay (ms)
  $description: >-
    How long to rest on a subfolder before its menu opens. A small delay stops
    menus from firing off while you sweep across the grid. 0 is instant.

- submenuCloseDelayMs: 300
  $name: Subfolder close delay (ms)
  $description: >-
    How long a cascaded subfolder menu stays open after the mouse leaves the
    cell that opened it. Gives you time to move diagonally into the submenu.

- showHidden: false
  $name: Show hidden items
  $description: Include hidden and system files.

- showExtensions: false
  $name: Show file extensions
  $description: Shortcut (.lnk) extensions are always hidden.

- sortBy: name
  $name: Sort by
  $description: >-
    Folders always come first so the entries that cascade stay together at the
    top. This orders the items within each group.
  $options:
  - name: Name
  - modified: Date modified (newest first)

- cellWidth: 92
  $name: Cell width (px)

- cellHeight: 88
  $name: Cell height (px)

- iconSize: 32
  $name: Item icon size (px)

- fontSize: 12
  $name: Item label size (px)

- titleFontSize: 13
  $name: Folder title size (px)
  $description: Size of the name shown at the top of the main hover grid.

- titleAlign: center
  $name: Folder title position
  $description: >-
    Horizontal position of the folder name on the main hover grid. Subfolder
    menus never show a title.
  $options:
  - left: Left
  - center: Center
  - right: Right

- cornerRadius: 8
  $name: Grid corner radius (px)

- panelOpacity: 85
  $name: Grid opacity (%)

- acrylic: true
  $name: Acrylic blur
  $description: Blur the desktop behind the grid. Turn off if it looks wrong.

- gapAbove: 8
  $name: Gap above taskbar (px)
  $description: Distance between the taskbar and the grid.

- gapBefore: 0
  $name: Padding before buttons (px)

- gapAfter: 0
  $name: Padding after buttons (px)

- buttonIconSize: 0
  $name: Button icon size (px)
  $description: 0 matches the size Windows uses for its own taskbar icons.

- openFolderOnClick: true
  $name: Click opens the folder
  $description: Left clicking a taskbar button opens the folder in File Explorer.
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <windhawk_utils.h>

#include <commoncontrols.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>

#include <gdiplus.h>
#include <objidl.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#ifndef SEE_MASK_ASYNCOK
#define SEE_MASK_ASYNCOK 0x00100000
#endif

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

////////////////////////////////////////////////////////////////////////////////
// Settings

struct FolderEntry {
    std::wstring name;
    std::wstring path;
    std::wstring icon;
};

enum class SortMode { Name, Modified };

struct Settings {
    std::vector<FolderEntry> folders;
    std::wstring position = L"beforeApps";
    std::wstring anchor = L"firstApp";
    int hoverDelayMs = 0;
    int closeDelayMs = 250;
    int columns = 0;
    int maxItems = 60;
    bool includeSubfolders = true;
    int maxFolderDepth = -1;
    int submenuDelayMs = 150;
    int submenuCloseDelayMs = 300;
    bool showHidden = false;
    bool showExtensions = false;
    SortMode sortBy = SortMode::Name;
    int cellWidth = 92;
    int cellHeight = 88;
    int iconSize = 32;
    int fontSize = 12;
    int titleFontSize = 13;
    std::wstring titleAlign = L"center";
    int cornerRadius = 8;
    int panelOpacity = 85;
    bool acrylic = true;
    int gapAbove = 8;
    int gapBefore = 0;
    int gapAfter = 0;
    int buttonIconSize = 0;
    bool openFolderOnClick = true;
};

Settings g_settings;

std::atomic<bool> g_unloading{false};

////////////////////////////////////////////////////////////////////////////////
// Small helpers

std::wstring Trim(std::wstring s) {
    size_t b = s.find_first_not_of(L" \t\r\n");
    if (b == std::wstring::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t\r\n");
    return s.substr(b, e - b + 1);
}

std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    DWORD needed = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!needed) {
        return s;
    }
    std::wstring out(needed, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), out.data(), needed);
    if (!written) {
        return s;
    }
    out.resize(written - 1);
    return out;
}

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

// An icon setting is a file reference if it looks like a path, otherwise it is
// literal text to render (an emoji).
bool IconSettingIsFile(const std::wstring& icon) {
    if (icon.size() < 3) {
        return false;
    }
    if (icon.find(L'\\') != std::wstring::npos ||
        icon.find(L'/') != std::wstring::npos) {
        return true;
    }
    return icon[1] == L':';
}

void LoadFolders(std::vector<FolderEntry>* out) {
    out->clear();
    for (int i = 0; i < 64; i++) {
        auto rawPath =
            WindhawkUtils::StringSetting::make(L"folders[%d].path", i);
        std::wstring path = Trim(std::wstring(rawPath.get()));
        // Skip blank slots so a hole in the middle does not drop later folders.
        if (path.empty()) {
            continue;
        }

        auto rawName =
            WindhawkUtils::StringSetting::make(L"folders[%d].name", i);
        auto rawIcon =
            WindhawkUtils::StringSetting::make(L"folders[%d].icon", i);

        FolderEntry entry;
        entry.name = Trim(std::wstring(rawName.get()));
        entry.path = ExpandEnv(path);
        entry.icon = Trim(std::wstring(rawIcon.get()));
        if (entry.name.empty()) {
            entry.name = entry.path;
        }
        out->push_back(std::move(entry));
    }
}

void LoadSettings() {
    LoadFolders(&g_settings.folders);

    g_settings.position = GetStringSetting(L"position");
    if (g_settings.position.empty()) {
        g_settings.position = L"beforeApps";
    }
    g_settings.anchor = GetStringSetting(L"anchor");
    if (g_settings.anchor.empty()) {
        g_settings.anchor = L"firstApp";
    }

    // "type" was the old folders-first option, which is now unconditional.
    std::wstring sortBy = GetStringSetting(L"sortBy");
    g_settings.sortBy =
        sortBy == L"modified" ? SortMode::Modified : SortMode::Name;

    g_settings.hoverDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"hoverDelayMs"), 0, 5000);
    g_settings.closeDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"closeDelayMs"), 0, 5000);
    g_settings.columns = std::clamp<int>(Wh_GetIntSetting(L"columns"), 0, 24);
    g_settings.maxItems =
        std::clamp<int>(Wh_GetIntSetting(L"maxItems"), 0, 400);
    g_settings.includeSubfolders = Wh_GetIntSetting(L"includeSubfolders");
    g_settings.maxFolderDepth =
        std::clamp<int>(Wh_GetIntSetting(L"maxFolderDepth"), -1, 32);
    g_settings.submenuDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"submenuDelayMs"), 0, 5000);
    g_settings.submenuCloseDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"submenuCloseDelayMs"), 0, 5000);
    g_settings.showHidden = Wh_GetIntSetting(L"showHidden");
    g_settings.showExtensions = Wh_GetIntSetting(L"showExtensions");
    g_settings.cellWidth =
        std::clamp<int>(Wh_GetIntSetting(L"cellWidth"), 32, 400);
    g_settings.cellHeight =
        std::clamp<int>(Wh_GetIntSetting(L"cellHeight"), 32, 400);
    g_settings.iconSize =
        std::clamp<int>(Wh_GetIntSetting(L"iconSize"), 8, 256);
    g_settings.fontSize = std::clamp<int>(Wh_GetIntSetting(L"fontSize"), 6, 48);
    g_settings.titleFontSize =
        std::clamp<int>(Wh_GetIntSetting(L"titleFontSize"), 6, 48);
    g_settings.titleAlign = GetStringSetting(L"titleAlign");
    if (g_settings.titleAlign != L"left" && g_settings.titleAlign != L"right") {
        g_settings.titleAlign = L"center";
    }
    g_settings.cornerRadius =
        std::clamp<int>(Wh_GetIntSetting(L"cornerRadius"), 0, 32);
    g_settings.panelOpacity =
        std::clamp<int>(Wh_GetIntSetting(L"panelOpacity"), 5, 100);
    g_settings.acrylic = Wh_GetIntSetting(L"acrylic");
    g_settings.gapAbove =
        std::clamp<int>(Wh_GetIntSetting(L"gapAbove"), 0, 200);
    g_settings.gapBefore =
        std::clamp<int>(Wh_GetIntSetting(L"gapBefore"), 0, 200);
    g_settings.gapAfter =
        std::clamp<int>(Wh_GetIntSetting(L"gapAfter"), 0, 200);
    g_settings.buttonIconSize =
        std::clamp<int>(Wh_GetIntSetting(L"buttonIconSize"), 0, 128);
    g_settings.openFolderOnClick = Wh_GetIntSetting(L"openFolderOnClick");
}

////////////////////////////////////////////////////////////////////////////////
// taskbar.dll plumbing
//
// Shared boilerplate from the Windhawk taskbar mods: reach the taskbar's
// XamlRoot through CTaskBand -> TaskbarHost, and marshal work onto the taskbar
// UI thread.

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void* result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }
    if (!TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetPrimaryTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (!CSecondaryTaskBand_GetTaskbarHost_Original ||
        !CSecondaryTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    WCHAR className[32];
    if (!GetClassName(hTaskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return GetPrimaryTaskbarXamlRoot(hTaskbarWnd);
    }
    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(hTaskbarWnd);
    }
    return nullptr;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Visual tree helpers

FrameworkElement FindChildByName(FrameworkElement const& element,
                                 std::wstring_view name,
                                 int maxDepth = 24) {
    if (!element || maxDepth <= 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    if (!root) {
        return nullptr;
    }
    FrameworkElement taskbarFrame = nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == L"Taskbar.TaskbarFrame") {
            taskbarFrame = child;
            break;
        }
    }
    if (!taskbarFrame) {
        return nullptr;
    }
    auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid", 4);
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

// The repeater's realized children are not in layout order, so anything that
// depends on "leftmost" or "rightmost" is resolved geometrically instead.
void EnumRepeaterChildren(FrameworkElement const& repeater,
                          std::vector<FrameworkElement>* out) {
    out->clear();
    if (!repeater) {
        return;
    }
    int count = VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (child) {
            out->push_back(child);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// GDI+ and icon helpers

ULONG_PTR g_gdiplusToken = 0;

struct ThemeCache {
    bool dark = true;
    BYTE accentR = 0;
    BYTE accentG = 120;
    BYTE accentB = 215;
    bool variableFontAvailable = false;
    bool resolved = false;
};

ThemeCache g_themeCache;

void RefreshThemeCache() {
    try {
        winrt::Windows::UI::ViewManagement::UISettings settings;
        auto bg = settings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Background);
        g_themeCache.dark = ((int)bg.R + bg.G + bg.B) < 384;

        auto accent = settings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        g_themeCache.accentR = accent.R;
        g_themeCache.accentG = accent.G;
        g_themeCache.accentB = accent.B;
    } catch (...) {
        g_themeCache.dark = true;
        g_themeCache.accentR = 0;
        g_themeCache.accentG = 120;
        g_themeCache.accentB = 215;
    }

    Gdiplus::FontFamily probe(L"Segoe UI Variable Text");
    g_themeCache.variableFontAvailable = probe.IsAvailable() != FALSE;
    g_themeCache.resolved = true;
}

bool IsDarkTheme() {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return g_themeCache.dark;
}

Gdiplus::Color GetAccentGdipColor(BYTE alpha) {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return Gdiplus::Color(alpha, g_themeCache.accentR, g_themeCache.accentG,
                          g_themeCache.accentB);
}

PCWSTR PopupFontName() {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return g_themeCache.variableFontAvailable ? L"Segoe UI Variable Text"
                                              : L"Segoe UI";
}

// True when the alpha channel carries real transparency (partial alpha, or a
// mix of fully transparent and opaque). All-0 or all-255 means the AND mask
// must supply transparency instead — common for classic XOR+mask icons.
bool IconPixelsHaveUsefulAlpha(const BYTE* bgra,
                               int width,
                               int height,
                               int stride) {
    if (!bgra || width <= 0 || height <= 0) {
        return false;
    }
    bool sawTransparent = false;
    bool sawOpaque = false;
    bool sawPartial = false;
    for (int y = 0; y < height; y++) {
        const BYTE* row = bgra + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            BYTE a = row[(size_t)x * 4 + 3];
            if (a == 0) {
                sawTransparent = true;
            } else if (a == 255) {
                sawOpaque = true;
            } else {
                sawPartial = true;
            }
            if (sawPartial || (sawTransparent && sawOpaque)) {
                return true;
            }
        }
    }
    return false;
}

// AND mask bit: 1/white = transparent, 0/black = opaque. Bits are MSB-first
// within each byte; rows are DWORD-aligned.
bool IconMaskBitTransparent(const BYTE* maskBits,
                            int maskWidthBytes,
                            int x,
                            int y) {
    const BYTE* row = maskBits + (size_t)y * maskWidthBytes;
    BYTE b = row[x >> 3];
    return (b & (0x80 >> (x & 7))) != 0;
}

// Pull a top-down 32bpp BGRA copy of an HBITMAP via GetDIBits (preserves the
// alpha byte for existing 32bpp sources on modern Windows).
bool CopyHbitmapToBgra32(HBITMAP hbm,
                         std::vector<BYTE>* out,
                         int* outW,
                         int* outH) {
    if (!hbm || !out || !outW || !outH) {
        return false;
    }
    BITMAP bm{};
    if (!GetObject(hbm, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return false;
    }
    const int w = bm.bmWidth;
    const int h = bm.bmHeight;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    out->assign((size_t)w * h * 4, 0);
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return false;
    }
    int got = GetDIBits(hdc, hbm, 0, h, out->data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);
    if (got != h) {
        return false;
    }
    *outW = w;
    *outH = h;
    return true;
}

// Clear junk RGB under transparent pixels and premultiply for PARGB drawing.
// Classic XOR icons often leave white/gray in alpha==0 cells; SourceOver onto a
// dark PARGB panel then reads as a faint lighter veil. Near-zero alpha with
// near-white/near-black RGB (mask/DrawIconEx noise) is treated as fully clear
// so soft edges of modern icons are preserved.
void SanitizeAndPremultiplyBgra(BYTE* bgra,
                                int width,
                                int height,
                                int stride) {
    if (!bgra || width <= 0 || height <= 0) {
        return;
    }
    for (int y = 0; y < height; y++) {
        BYTE* row = bgra + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            BYTE* p = row + (size_t)x * 4;
            BYTE b = p[0], g = p[1], r = p[2], a = p[3];
            if (a == 0) {
                p[0] = p[1] = p[2] = 0;
                continue;
            }
            if (a < 8) {
                const int mx = (std::max)((int)r, (std::max)((int)g, (int)b));
                const int mn = (std::min)((int)r, (std::min)((int)g, (int)b));
                const bool nearWhite = mn >= 220;
                const bool nearBlack = mx <= 35;
                if (nearWhite || nearBlack) {
                    p[0] = p[1] = p[2] = p[3] = 0;
                    continue;
                }
            }
            p[0] = (BYTE)((int)b * a / 255);
            p[1] = (BYTE)((int)g * a / 255);
            p[2] = (BYTE)((int)r * a / 255);
        }
    }
}

// After scaling/filtering a PARGB bitmap, re-clear fully transparent pixels and
// clamp channels so rgb never exceeds alpha (invalid premul from bicubic).
void SanitizeParGbBitmap(Gdiplus::Bitmap* bmp) {
    if (!bmp) {
        return;
    }
    const int w = (int)bmp->GetWidth();
    const int h = (int)bmp->GetHeight();
    if (w <= 0 || h <= 0) {
        return;
    }
    Gdiplus::BitmapData bd{};
    Gdiplus::Rect lockRect(0, 0, w, h);
    if (bmp->LockBits(&lockRect,
                      (Gdiplus::ImageLockMode)(Gdiplus::ImageLockModeRead |
                                               Gdiplus::ImageLockModeWrite),
                      PixelFormat32bppPARGB, &bd) != Gdiplus::Ok) {
        return;
    }
    for (int y = 0; y < h; y++) {
        BYTE* row = (BYTE*)bd.Scan0 + (size_t)y * bd.Stride;
        for (int x = 0; x < w; x++) {
            BYTE* p = row + (size_t)x * 4;
            BYTE a = p[3];
            if (a == 0) {
                p[0] = p[1] = p[2] = 0;
                continue;
            }
            if (a < 8) {
                const int mx =
                    (std::max)((int)p[0], (std::max)((int)p[1], (int)p[2]));
                // Premultiplied near-white junk is still bright relative to alpha.
                const bool brightJunk = mx >= (int)a * 200 / 255 && mx >= 6;
                const bool nearBlack = mx <= 2;
                if (brightJunk || nearBlack) {
                    p[0] = p[1] = p[2] = p[3] = 0;
                    continue;
                }
            }
            if (p[0] > a) {
                p[0] = a;
            }
            if (p[1] > a) {
                p[1] = a;
            }
            if (p[2] > a) {
                p[2] = a;
            }
        }
    }
    bmp->UnlockBits(&bd);
}

// Apply the icon's 1-bit AND mask as alpha. For color icons the mask matches
// the color size (or a double-height legacy layout); for monochrome icons the
// AND plane is the top half. Destination may differ in size from the mask —
// coordinates are mapped so scaled DrawIconEx buffers still work.
void ApplyIconAndMaskToBgra(BYTE* bgra,
                            int width,
                            int height,
                            HBITMAP hbmMask,
                            bool monoIcon) {
    if (!bgra || !hbmMask || width <= 0 || height <= 0) {
        return;
    }

    BITMAP bmMask{};
    if (!GetObject(hbmMask, sizeof(bmMask), &bmMask) || bmMask.bmWidth <= 0 ||
        bmMask.bmHeight <= 0) {
        return;
    }

    const int maskW = bmMask.bmWidth;
    const int maskH = bmMask.bmHeight;
    int andRows;
    if (monoIcon) {
        andRows = maskH / 2;
    } else if (maskH >= height * 2 && height > 0) {
        andRows = height;
    } else {
        andRows = maskH;
    }
    if (andRows <= 0) {
        return;
    }

    struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[2];
    } bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = maskW;
    bi.bmiHeader.biHeight = -maskH;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 1;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiColors[0] = {0, 0, 0, 0};
    bi.bmiColors[1] = {255, 255, 255, 0};

    const int maskStride = ((maskW + 31) / 32) * 4;
    std::vector<BYTE> maskBits((size_t)maskStride * maskH, 0);

    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return;
    }
    int got = GetDIBits(hdc, hbmMask, 0, maskH, maskBits.data(),
                        reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);
    if (got <= 0) {
        return;
    }

    for (int y = 0; y < height; y++) {
        const int my = (y * andRows) / height;
        BYTE* row = bgra + (size_t)y * width * 4;
        for (int x = 0; x < width; x++) {
            const int mx = (x * maskW) / width;
            if (IconMaskBitTransparent(maskBits.data(), maskStride, mx, my)) {
                row[(size_t)x * 4 + 0] = 0;
                row[(size_t)x * 4 + 1] = 0;
                row[(size_t)x * 4 + 2] = 0;
                row[(size_t)x * 4 + 3] = 0;
            } else if (row[(size_t)x * 4 + 3] == 0) {
                // Opaque under the mask but alpha was never filled — make solid.
                row[(size_t)x * 4 + 3] = 255;
            }
        }
    }
}

// `bgra` must already be premultiplied (SanitizeAndPremultiplyBgra). Output is
// PixelFormat32bppPARGB to match RebuildLevelBase / UpdateLayeredWindow.
std::shared_ptr<Gdiplus::Bitmap> BitmapFromBgraScaled(const BYTE* bgra,
                                                      int srcW,
                                                      int srcH,
                                                      int size) {
    if (!bgra || srcW <= 0 || srcH <= 0 || size <= 0) {
        return nullptr;
    }

    std::shared_ptr<Gdiplus::Bitmap> result;
    try {
        Gdiplus::Bitmap source(srcW, srcH, srcW * 4, PixelFormat32bppPARGB,
                               const_cast<BYTE*>(bgra));
        if (source.GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        result =
            std::make_shared<Gdiplus::Bitmap>(size, size, PixelFormat32bppPARGB);
        if (!result || result->GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        Gdiplus::Graphics g(result.get());
        if (g.GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        if (g.DrawImage(&source, 0, 0, size, size) != Gdiplus::Ok) {
            return nullptr;
        }
        SanitizeParGbBitmap(result.get());
    } catch (...) {
        return nullptr;
    }
    return result;
}

// Convert HICON to a sized 32bpp PARGB bitmap for every icon type:
// 1) Prefer color-plane pixels that already carry useful alpha (modern ARGB).
// 2) Otherwise apply the 1-bit AND mask (black=opaque, white=transparent) so
//    classic XOR+mask icons never keep white/black bogus backgrounds.
// 3) Sanitize transparent RGB + premultiply, then scale as PARGB.
// 4) Last resort: DrawIconEx onto a zero-cleared 32bpp DIB, then mask if needed.
std::shared_ptr<Gdiplus::Bitmap> HIconToBitmap(HICON hIcon, int size) {
    if (!hIcon || size <= 0) {
        return nullptr;
    }

    ICONINFO ii{};
    if (GetIconInfo(hIcon, &ii)) {
        const bool monoIcon = (ii.hbmColor == nullptr);
        std::vector<BYTE> bgra;
        int srcW = 0;
        int srcH = 0;
        bool havePixels = false;

        if (ii.hbmColor) {
            havePixels = CopyHbitmapToBgra32(ii.hbmColor, &bgra, &srcW, &srcH);
        } else if (ii.hbmMask) {
            // Monochrome: reconstruct color from the XOR (bottom) half via
            // DrawIconEx onto a cleared DIB at the mask's logical size.
            BITMAP bmMask{};
            if (GetObject(ii.hbmMask, sizeof(bmMask), &bmMask) &&
                bmMask.bmWidth > 0 && bmMask.bmHeight >= 2) {
                srcW = bmMask.bmWidth;
                srcH = bmMask.bmHeight / 2;
                BITMAPINFO bi{};
                bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bi.bmiHeader.biWidth = srcW;
                bi.bmiHeader.biHeight = -srcH;
                bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;

                HDC hScreen = GetDC(nullptr);
                if (hScreen) {
                    HDC hMem = CreateCompatibleDC(hScreen);
                    ReleaseDC(nullptr, hScreen);
                    if (hMem) {
                        void* bits = nullptr;
                        HBITMAP hDib = CreateDIBSection(hMem, &bi, DIB_RGB_COLORS,
                                                        &bits, nullptr, 0);
                        if (hDib && bits) {
                            HGDIOBJ old = SelectObject(hMem, hDib);
                            memset(bits, 0, (size_t)srcW * srcH * 4);
                            DrawIconEx(hMem, 0, 0, hIcon, srcW, srcH, 0, nullptr,
                                       DI_NORMAL);
                            SelectObject(hMem, old);
                            GdiFlush();
                            bgra.assign((BYTE*)bits,
                                        (BYTE*)bits + (size_t)srcW * srcH * 4);
                            havePixels = true;
                            DeleteObject(hDib);
                        }
                        DeleteDC(hMem);
                    }
                }
            }
        }

        if (havePixels && srcW > 0 && srcH > 0) {
            if (!IconPixelsHaveUsefulAlpha(bgra.data(), srcW, srcH, srcW * 4) &&
                ii.hbmMask) {
                ApplyIconAndMaskToBgra(bgra.data(), srcW, srcH, ii.hbmMask,
                                       monoIcon);
            }
            SanitizeAndPremultiplyBgra(bgra.data(), srcW, srcH, srcW * 4);
            auto scaled =
                BitmapFromBgraScaled(bgra.data(), srcW, srcH, size);
            if (ii.hbmColor) {
                DeleteObject(ii.hbmColor);
            }
            if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
            if (scaled) {
                return scaled;
            }
        } else {
            if (ii.hbmColor) {
                DeleteObject(ii.hbmColor);
            }
            if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
        }
    }

    // Last resort: DrawIconEx into a zero-cleared 32bpp DIB, then re-apply the
    // AND mask when the draw left a useless alpha channel (XOR garbage RGB
    // such as opaque white in transparent cells).
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        return nullptr;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    ReleaseDC(nullptr, hScreenDC);
    if (!hMemDC) {
        return nullptr;
    }

    void* bits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        DeleteDC(hMemDC);
        return nullptr;
    }

    HGDIOBJ hOld = SelectObject(hMemDC, hBmp);
    memset(bits, 0, (size_t)size * size * 4);
    DrawIconEx(hMemDC, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
    SelectObject(hMemDC, hOld);
    GdiFlush();

    BYTE* px = (BYTE*)bits;
    if (!IconPixelsHaveUsefulAlpha(px, size, size, size * 4)) {
        ICONINFO maskInfo{};
        if (GetIconInfo(hIcon, &maskInfo)) {
            ApplyIconAndMaskToBgra(px, size, size, maskInfo.hbmMask,
                                   maskInfo.hbmColor == nullptr);
            if (maskInfo.hbmColor) {
                DeleteObject(maskInfo.hbmColor);
            }
            if (maskInfo.hbmMask) {
                DeleteObject(maskInfo.hbmMask);
            }
        } else {
            // No mask available: opaque where any color was drawn.
            for (size_t i = 0; i < (size_t)size * size * 4; i += 4) {
                if (px[i] || px[i + 1] || px[i + 2]) {
                    px[i + 3] = 255;
                }
            }
        }
    }
    SanitizeAndPremultiplyBgra(px, size, size, size * 4);

    std::shared_ptr<Gdiplus::Bitmap> result;
    try {
        result =
            std::make_shared<Gdiplus::Bitmap>(size, size, PixelFormat32bppPARGB);
        if (result->GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::BitmapData bd{};
            Gdiplus::Rect lockRect(0, 0, size, size);
            if (result->LockBits(&lockRect, Gdiplus::ImageLockModeWrite,
                                 PixelFormat32bppPARGB, &bd) == Gdiplus::Ok) {
                for (int y = 0; y < size; y++) {
                    memcpy((BYTE*)bd.Scan0 + (size_t)y * bd.Stride,
                           px + (size_t)y * size * 4, (size_t)size * 4);
                }
                result->UnlockBits(&bd);
                SanitizeParGbBitmap(result.get());
            } else {
                result.reset();
            }
        } else {
            result.reset();
        }
    } catch (...) {
        result.reset();
    }

    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// Not exported by the MinGW uuid library, so it is spelled out here.
constexpr GUID kIImageListGuid = {
    0x46eb5926,
    0x582e,
    0x4017,
    {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

// Pick the system image list whose native size best covers the requested size.
int ShilForSize(int pixelSize) {
    if (pixelSize <= 16) {
        return SHIL_SMALL;
    }
    if (pixelSize <= 32) {
        return SHIL_LARGE;
    }
    if (pixelSize <= 48) {
        return SHIL_EXTRALARGE;
    }
    return SHIL_JUMBO;
}

bool PathIsLnkFile(const std::wstring& path) {
    return path.size() >= 4 &&
           _wcsicmp(path.c_str() + (path.size() - 4), L".lnk") == 0;
}

// SHDefExtractIconW does not composite the shortcut overlay arrow.
HICON ExtractIconDef(const std::wstring& path, int index, int pixelSize) {
    HICON hLarge = nullptr;
    HICON hSmall = nullptr;
    if (SUCCEEDED(SHDefExtractIconW(path.c_str(), index, 0, &hLarge, &hSmall,
                                    (UINT)pixelSize)) &&
        hLarge) {
        if (hSmall) {
            DestroyIcon(hSmall);
        }
        return hLarge;
    }
    if (hSmall) {
        return hSmall;
    }
    return nullptr;
}

// System image list index without drawing overlays (no SHGFI_LINKOVERLAY /
// SHGFI_ICON, which bake the shortcut arrow into .lnk icons).
HICON GetShellIconFromImageList(const std::wstring& path, int pixelSize) {
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                        SHGFI_SYSICONINDEX)) {
        return nullptr;
    }

    HICON hIcon = nullptr;
    IImageList* imageList = nullptr;
    if (SUCCEEDED(SHGetImageList(ShilForSize(pixelSize), kIImageListGuid,
                                 (void**)&imageList)) &&
        imageList) {
        imageList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
        imageList->Release();
    }
    return hIcon;
}

// Resolve a .lnk's custom icon location or target path and extract without the
// shell link overlay.
HICON GetIconFromShortcutFile(const std::wstring& lnkPath, int pixelSize) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return nullptr;
    }

    IPersistFile* persist = nullptr;
    HRESULT hr = link->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (FAILED(hr) || !persist) {
        link->Release();
        return nullptr;
    }

    hr = persist->Load(lnkPath.c_str(), STGM_READ);
    persist->Release();
    if (FAILED(hr)) {
        link->Release();
        return nullptr;
    }

    WCHAR iconLoc[MAX_PATH]{};
    int iconIndex = 0;
    hr = link->GetIconLocation(iconLoc, ARRAYSIZE(iconLoc), &iconIndex);
    if (SUCCEEDED(hr) && iconLoc[0]) {
        WCHAR expanded[MAX_PATH]{};
        PCWSTR file = iconLoc;
        DWORD expandedLen =
            ExpandEnvironmentStringsW(iconLoc, expanded, ARRAYSIZE(expanded));
        if (expandedLen > 0 && expandedLen <= ARRAYSIZE(expanded)) {
            file = expanded;
        }
        HICON hIcon = ExtractIconDef(file, iconIndex, pixelSize);
        if (hIcon) {
            link->Release();
            return hIcon;
        }
    }

    WCHAR target[MAX_PATH]{};
    hr = link->GetPath(target, ARRAYSIZE(target), nullptr, SLGP_RAWPATH);
    if (FAILED(hr) || !target[0]) {
        hr = link->GetPath(target, ARRAYSIZE(target), nullptr, 0);
    }
    link->Release();
    if (FAILED(hr) || !target[0]) {
        return nullptr;
    }

    WCHAR expandedTarget[MAX_PATH]{};
    PCWSTR useTarget = target;
    DWORD targetLen = ExpandEnvironmentStringsW(target, expandedTarget,
                                                ARRAYSIZE(expandedTarget));
    if (targetLen > 0 && targetLen <= ARRAYSIZE(expandedTarget)) {
        useTarget = expandedTarget;
    }

    if (HICON hIcon = ExtractIconDef(useTarget, 0, pixelSize)) {
        return hIcon;
    }
    return GetShellIconFromImageList(useTarget, pixelSize);
}

HICON GetShellIconForPath(const std::wstring& path, int pixelSize) {
    // Prefer paths that never composite SHGFI_LINKOVERLAY / shortcut arrows.
    if (PathIsLnkFile(path)) {
        if (HICON hIcon = GetIconFromShortcutFile(path, pixelSize)) {
            return hIcon;
        }
        // Extracting from the .lnk itself still avoids the overlay arrow.
        if (HICON hIcon = ExtractIconDef(path, 0, pixelSize)) {
            return hIcon;
        }
    } else if (HICON hIcon = ExtractIconDef(path, 0, pixelSize)) {
        return hIcon;
    }

    if (HICON hIcon = GetShellIconFromImageList(path, pixelSize)) {
        return hIcon;
    }

    // Last resort — SHGFI_ICON may include the shortcut overlay for .lnk files.
    SHFILEINFOW sfi{};
    UINT flags =
        SHGFI_ICON | (pixelSize <= 16 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), flags)) {
        return sfi.hIcon;
    }
    return nullptr;
}

// Accepts "C:\path\app.exe,3" as well as a plain image or icon file path.
HICON ExtractIconFromResourceSpec(const std::wstring& spec, int pixelSize) {
    std::wstring file = spec;
    int index = 0;

    size_t comma = spec.find_last_of(L',');
    if (comma != std::wstring::npos && comma + 1 < spec.size()) {
        PCWSTR numStart = spec.c_str() + comma + 1;
        WCHAR* end = nullptr;
        long parsed = wcstol(numStart, &end, 10);
        if (end && *end == L'\0') {
            file = spec.substr(0, comma);
            index = (int)parsed;
        }
    }

    if (HICON hIcon = ExtractIconDef(file, index, pixelSize)) {
        return hIcon;
    }
    return GetShellIconFromImageList(file, pixelSize);
}

////////////////////////////////////////////////////////////////////////////////
// Folder scanning and caching

struct GridItem {
    std::wstring displayName;
    std::wstring fullPath;
    // When a .lnk points at a folder we cascade against the target path, but
    // keep the shortcut itself here so its custom icon still shows.
    std::wstring iconPath;
    bool isFolder = false;
    ULONGLONG modified = 0;
    std::shared_ptr<Gdiplus::Bitmap> icon;
};

struct FolderData {
    std::vector<GridItem> items;
    bool ready = false;
    ULONGLONG scannedAtTick = 0;
    int scannedIconSize = 0;
};

struct ScanRequest {
    std::wstring path;
    int iconPixelSize = 32;
};

// Keyed by resolved folder path so subfolders opened on hover share the same
// cache as the folders configured on the taskbar.
std::mutex g_cacheMutex;
[[clang::no_destroy]] std::optional<
    std::unordered_map<std::wstring, std::shared_ptr<FolderData>>>
    g_folderCache{std::in_place};

std::unordered_map<std::wstring, std::shared_ptr<FolderData>>& FolderCache() {
    if (!g_folderCache) {
        g_folderCache.emplace();
    }
    return *g_folderCache;
}

std::mutex g_scanMutex;
std::vector<ScanRequest> g_scanQueue;
// Atomic: ScanFolderInto reads this outside the mutex during icon extraction.
std::atomic<bool> g_scanThreadStop{false};
// Manual-reset stop + auto-reset work. Idle wait is MsgWaitForMultipleObjects
// with QS_ALLINPUT (no 50 ms poll). Created lazily with the scan thread.
HANDLE g_scanStopEvent = nullptr;
HANDLE g_scanWorkEvent = nullptr;
// optional + no_destroy: a bare std::thread aborts Explorer on process exit if
// it is still joinable when globals are destroyed.
[[clang::no_destroy]] std::optional<std::thread> g_scanThread;

// Shell icon handlers on an STA thread may create windows and post messages.
// Pump while idle/between extractions so a wait without a message loop cannot
// hang forever; stop still wakes via g_scanStopEvent + g_scanThreadStop.
void PumpScanThreadMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            // Do not tear down the scan thread on an unexpected WM_QUIT.
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

std::wstring CacheKey(const std::wstring& path) {
    std::wstring key = path;
    CharLowerBuffW(key.data(), (DWORD)key.size());
    return key;
}

// A `shell:` target is resolved to a filesystem path so the plain directory
// walk below can handle every configured folder the same way.
std::wstring ResolveFolderPath(const std::wstring& raw) {
    if (raw.size() > 6 && _wcsnicmp(raw.c_str(), L"shell:", 6) == 0) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(
                SHParseDisplayName(raw.c_str(), nullptr, &pidl, 0, nullptr)) &&
            pidl) {
            WCHAR buf[MAX_PATH]{};
            bool ok = SHGetPathFromIDListW(pidl, buf);
            CoTaskMemFree(pidl);
            if (ok && buf[0]) {
                return buf;
            }
        }
        return L"";
    }

    std::wstring path = raw;
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

std::wstring MakeDisplayName(const std::wstring& fileName, bool isFolder) {
    if (isFolder) {
        return fileName;
    }
    size_t dot = fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        return fileName;
    }
    std::wstring ext = fileName.substr(dot);
    // Windows always hides shortcut extensions, regardless of the user's
    // setting.
    if (_wcsicmp(ext.c_str(), L".lnk") == 0 ||
        _wcsicmp(ext.c_str(), L".url") == 0) {
        return fileName.substr(0, dot);
    }
    if (!g_settings.showExtensions) {
        return fileName.substr(0, dot);
    }
    return fileName;
}

// Folder shortcuts (.lnk whose target is a directory) are files to
// FindFirstFile, but Explorer treats them as folders. Resolve the target so
// they can cascade.
std::wstring ResolveFolderShortcutTarget(const std::wstring& path) {
    size_t len = path.size();
    if (len < 5 || _wcsicmp(path.c_str() + (len - 4), L".lnk") != 0) {
        return {};
    }

    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return {};
    }

    IPersistFile* persist = nullptr;
    HRESULT hr = link->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (FAILED(hr) || !persist) {
        link->Release();
        return {};
    }

    hr = persist->Load(path.c_str(), STGM_READ);
    persist->Release();
    if (FAILED(hr)) {
        link->Release();
        return {};
    }

    // Skip Resolve(): it can hang on unreachable network targets and may show
    // UI.
    WCHAR target[MAX_PATH]{};
    hr = link->GetPath(target, ARRAYSIZE(target), nullptr, SLGP_RAWPATH);
    if (FAILED(hr) || !target[0]) {
        hr = link->GetPath(target, ARRAYSIZE(target), nullptr, 0);
    }
    link->Release();

    if (FAILED(hr) || !target[0]) {
        return {};
    }

    DWORD attrs = GetFileAttributesW(target);
    if (attrs == INVALID_FILE_ATTRIBUTES ||
        (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        return {};
    }

    std::wstring resolved = target;
    while (resolved.size() > 3 &&
           (resolved.back() == L'\\' || resolved.back() == L'/')) {
        resolved.pop_back();
    }
    return resolved;
}

void ScanFolderInto(const std::wstring& path,
                    int iconPixelSize,
                    FolderData* out) {
    out->items.clear();
    out->scannedIconSize = iconPixelSize;

    if (path.empty()) {
        return;
    }

    std::wstring pattern = path + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (g_unloading || g_scanThreadStop) {
            break;
        }
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        bool isRealDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        std::wstring itemPath = path + L"\\" + fd.cFileName;
        std::wstring iconPath;

        bool isFolder = isRealDir;
        if (!isFolder) {
            std::wstring target = ResolveFolderShortcutTarget(itemPath);
            if (!target.empty()) {
                isFolder = true;
                iconPath = itemPath;
                itemPath = std::move(target);
            }
        }

        if (isFolder && !g_settings.includeSubfolders) {
            continue;
        }
        if (!g_settings.showHidden &&
            (fd.dwFileAttributes &
             (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            continue;
        }
        if (_wcsicmp(fd.cFileName, L"desktop.ini") == 0) {
            continue;
        }

        GridItem item;
        item.isFolder = isFolder;
        item.fullPath = std::move(itemPath);
        item.iconPath = std::move(iconPath);
        // Use the on-disk kind for the label so .lnk extensions stay stripped.
        item.displayName = MakeDisplayName(fd.cFileName, isRealDir);
        item.modified = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                        fd.ftLastWriteTime.dwLowDateTime;
        out->items.push_back(std::move(item));
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    // Folders always lead, so the things that cascade sit together at the top
    // of the grid. The sort setting then orders within each group.
    std::sort(out->items.begin(), out->items.end(),
              [](const GridItem& a, const GridItem& b) {
                  if (a.isFolder != b.isFolder) {
                      return a.isFolder;
                  }
                  if (g_settings.sortBy == SortMode::Modified &&
                      a.modified != b.modified) {
                      return a.modified > b.modified;
                  }
                  return StrCmpLogicalW(a.displayName.c_str(),
                                        b.displayName.c_str()) < 0;
              });

    if (g_settings.maxItems > 0 &&
        (int)out->items.size() > g_settings.maxItems) {
        out->items.resize(g_settings.maxItems);
    }

    for (auto& item : out->items) {
        if (g_unloading || g_scanThreadStop) {
            break;
        }
        const std::wstring& iconSource =
            item.iconPath.empty() ? item.fullPath : item.iconPath;
        HICON hIcon = GetShellIconForPath(iconSource, iconPixelSize);
        if (hIcon) {
            item.icon = HIconToBitmap(hIcon, iconPixelSize);
            DestroyIcon(hIcon);
        }
        // Keep the STA responsive for in-process shell icon handlers.
        PumpScanThreadMessages();
    }
}

void StartScanThread();

void ScanThreadMain() {
    // Shell icon extractors (SHGetFileInfo / IExtractIcon) expect STA. MTA
    // causes many extractions to fail with the generic document icon.
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Capture event handles once; StopScanThread closes them only after join.
    HANDLE stopEvent = g_scanStopEvent;
    HANDLE workEvent = g_scanWorkEvent;

    for (;;) {
        ScanRequest request;
        {
            std::unique_lock<std::mutex> lock(g_scanMutex);
            for (;;) {
                if (g_scanThreadStop.load(std::memory_order_relaxed)) {
                    winrt::uninit_apartment();
                    return;
                }
                if (!g_scanQueue.empty()) {
                    break;
                }

                lock.unlock();
                // Event-driven idle: sleep until stop, work, or an STA message.
                // No timed poll — keeps Explorer out of a permanent 20 Hz wake.
                HANDLE waits[] = {stopEvent, workEvent};
                DWORD w = MsgWaitForMultipleObjects(2, waits, FALSE, INFINITE,
                                                    QS_ALLINPUT);
                if (w == WAIT_OBJECT_0) {
                    winrt::uninit_apartment();
                    return;
                }
                if (w == WAIT_OBJECT_0 + 2) {
                    PumpScanThreadMessages();
                } else if (w == WAIT_FAILED) {
                    Wh_Log(L"Scan thread MsgWaitForMultipleObjects failed (%u)",
                           GetLastError());
                    Sleep(10);
                }
                // WAIT_OBJECT_0+1 (work) or after pump: re-check the queue.
                lock.lock();
            }

            request = g_scanQueue.front();
            g_scanQueue.erase(g_scanQueue.begin());
        }

        if (request.iconPixelSize <= 0) {
            request.iconPixelSize = 32;
        }

        auto fresh = std::make_shared<FolderData>();
        ScanFolderInto(request.path, request.iconPixelSize, fresh.get());
        fresh->ready = true;
        fresh->scannedAtTick = GetTickCount64();

        if (g_unloading ||
            g_scanThreadStop.load(std::memory_order_relaxed)) {
            break;
        }

        {
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            FolderCache()[CacheKey(request.path)] = fresh;
        }

        Wh_Log(L"Scanned %s: %d items", request.path.c_str(),
               (int)fresh->items.size());
    }

    winrt::uninit_apartment();
}

void RequestScan(const std::wstring& path, int iconPixelSize) {
    if (path.empty() || g_unloading) {
        return;
    }

    // Lazy start (and restart after StopScanThread). Safe from the UI thread:
    // StartScanThread only creates the worker; Stop during uninit sets
    // g_unloading / g_scanThreadStop first so a raced enqueue is dropped.
    StartScanThread();

    std::lock_guard<std::mutex> lock(g_scanMutex);
    if (g_unloading || g_scanThreadStop.load(std::memory_order_relaxed)) {
        return;
    }
    for (const auto& queued : g_scanQueue) {
        if (queued.iconPixelSize == iconPixelSize &&
            _wcsicmp(queued.path.c_str(), path.c_str()) == 0) {
            return;
        }
    }
    g_scanQueue.push_back({path, iconPixelSize});
    if (g_scanWorkEvent) {
        SetEvent(g_scanWorkEvent);
    }
}

std::shared_ptr<FolderData> GetFolderData(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    auto found = FolderCache().find(CacheKey(path));
    return found == FolderCache().end() ? nullptr : found->second;
}

// Returns the cached contents, queueing a scan when the entry is missing,
// stale, or was rendered at a different icon size. Never blocks.
std::shared_ptr<FolderData> GetFolderDataAndRefresh(const std::wstring& path,
                                                    int iconPixelSize) {
    auto data = GetFolderData(path);
    if (!data || !data->ready || data->scannedIconSize != iconPixelSize ||
        GetTickCount64() - data->scannedAtTick > 5000) {
        RequestScan(path, iconPixelSize);
    }
    return data;
}

void StartScanThread() {
    std::lock_guard<std::mutex> lock(g_scanMutex);
    if (g_unloading) {
        return;
    }
    // Already running (or StopScanThread is joining). Never clear stop while
    // a join is in progress — RequestScan drops work via g_scanThreadStop.
    if (g_scanThread) {
        return;
    }

    if (!g_scanStopEvent) {
        g_scanStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_scanStopEvent) {
            Wh_Log(L"CreateEventW for scan stop failed (%u)", GetLastError());
            return;
        }
    } else {
        ResetEvent(g_scanStopEvent);
    }
    if (!g_scanWorkEvent) {
        g_scanWorkEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_scanWorkEvent) {
            Wh_Log(L"CreateEventW for scan work failed (%u)", GetLastError());
            CloseHandle(g_scanStopEvent);
            g_scanStopEvent = nullptr;
            return;
        }
    }

    g_scanThreadStop.store(false, std::memory_order_relaxed);
    g_scanThread.emplace(ScanThreadMain);
}

void StopScanThread() {
    {
        std::lock_guard<std::mutex> lock(g_scanMutex);
        g_scanThreadStop.store(true, std::memory_order_relaxed);
        g_scanQueue.clear();
        if (g_scanStopEvent) {
            SetEvent(g_scanStopEvent);
        }
    }
    if (g_scanThread && g_scanThread->joinable()) {
        g_scanThread->join();
    }
    g_scanThread.reset();

    if (g_scanStopEvent) {
        CloseHandle(g_scanStopEvent);
        g_scanStopEvent = nullptr;
    }
    if (g_scanWorkEvent) {
        CloseHandle(g_scanWorkEvent);
        g_scanWorkEvent = nullptr;
    }
}

void ResetFolderData() {
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    FolderCache().clear();
}

////////////////////////////////////////////////////////////////////////////////
// The hover grid window
//
// A XAML Popup would be clipped to the taskbar's XAML island, which is only as
// tall as the taskbar, so the grid is a real top-level layered window instead.
// Per-pixel alpha gives antialiased rounded corners and makes hit testing
// outside the panel fall through automatically.

constexpr PCWSTR kPopupClassName = L"WH_TaskbarFolderHoverTray_Grid";
constexpr PCWSTR kMenuOwnerClassName = L"WH_TaskbarFolderHoverTray_MenuOwner";
constexpr UINT_PTR kTickTimerId = 1;
constexpr UINT_PTR kOpenTimerId = 2;
constexpr UINT kTickTimerMs = 50;
// Hard cap on cascade depth so a symlink loop cannot exhaust window handles.
constexpr int kMaxLevels = 16;

// One grid in the cascade. Level 0 hangs off a taskbar button; deeper levels
// hang off the cell in their parent that opened them.
struct PopupLevel {
    HWND hwnd = nullptr;
    int depth = 0;
    std::wstring path;
    // Only filled for the root (taskbar) grid; subfolder menus leave this
    // empty.
    std::wstring title;
    std::vector<GridItem> items;
    std::vector<RECT> cellRects;
    RECT rect{};
    RECT anchorRect{};
    int spawnerCell = -1;
    int hoverCell = -1;
    int pressedCell = -1;
    bool loading = false;
    // Static content (panel, icons, labels) without hover/press chrome. Hover
    // changes blit this and overlay the highlight instead of repainting all cells.
    std::unique_ptr<Gdiplus::Bitmap> cachedBase;
    int cachedBaseW = 0;
    int cachedBaseH = 0;
    bool baseDirty = true;
};

HWND g_menuOwnerWnd = nullptr;
HWND g_taskbarWnd = nullptr;

// Windows are created lazily, one per depth, and reused across cascades.
std::vector<HWND> g_levelWindows;
[[clang::no_destroy]] std::optional<std::vector<std::unique_ptr<PopupLevel>>>
    g_levels{std::in_place};

std::vector<std::unique_ptr<PopupLevel>>& Levels() {
    if (!g_levels) {
        g_levels.emplace();
    }
    return *g_levels;
}

RECT g_rootAnchorRect{};
int g_popupDpi = 96;
bool g_menuActive = false;
ULONGLONG g_outsideSinceTick = 0;

std::wstring g_pendingRootPath;
std::wstring g_pendingRootTitle;
RECT g_pendingRootRect{};

int g_pendingSubDepth = -1;
int g_pendingSubCell = -1;
ULONGLONG g_pendingSubSinceTick = 0;
ULONGLONG g_closeDeeperSinceTick = 0;

IContextMenu2* g_activeContextMenu2 = nullptr;
IContextMenu3* g_activeContextMenu3 = nullptr;

// Both take their arguments by value: reopening a level destroys the PopupLevel
// the caller may have read them from.
void OpenRootLevel(std::wstring path, RECT anchorRect, std::wstring title);
void OpenSubLevel(int parentDepth, int cell);
void CloseLevelsFrom(int depth);
void CloseChain();
void StartRetryThread();
bool EnsurePopupClasses();
HWND EnsureMenuOwnerWindow();

int ScaleForPopup(int value) {
    return MulDiv(value, g_popupDpi, 96);
}

struct AccentPolicy {
    int accentState;
    int accentFlags;
    unsigned int gradientColor;
    int animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    void* data;
    size_t dataSize;
};

void ApplyBackdrop(HWND hWnd) {
    int corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                          sizeof(corner));

    if (!g_settings.acrylic) {
        return;
    }

    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);
    static auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWindowCompositionAttribute) {
        return;
    }

    // ACCENT_ENABLE_ACRYLICBLURBEHIND with a nearly transparent tint; the
    // visible panel colour is painted by us so the blur only adds depth.
    AccentPolicy policy{};
    policy.accentState = 4;
    policy.accentFlags = 0;
    policy.gradientColor = IsDarkTheme() ? 0x20000000 : 0x20FFFFFF;
    policy.animationId = 0;

    WindowCompositionAttributeData data{};
    data.attribute = 19;  // WCA_ACCENT_POLICY
    data.data = &policy;
    data.dataSize = sizeof(policy);
    setWindowCompositionAttribute(hWnd, &data);
}

void AddRoundedRect(Gdiplus::GraphicsPath* path,
                    const Gdiplus::Rect& rect,
                    int radius) {
    if (radius <= 0) {
        path->AddRectangle(rect);
        return;
    }
    int d = radius * 2;
    d = std::min<int>(d, std::min<int>(rect.Width, rect.Height));
    path->AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path->AddArc(rect.GetRight() - d, rect.Y, d, d, 270.0f, 90.0f);
    path->AddArc(rect.GetRight() - d, rect.GetBottom() - d, d, d, 0.0f, 90.0f);
    path->AddArc(rect.X, rect.GetBottom() - d, d, d, 90.0f, 90.0f);
    path->CloseFigure();
}

// A minimal folder silhouette: a body whose top edge steps up on the left to
// form the tab. Kept as straight edges so it stays crisp at badge sizes.
void AddFolderGlyphPath(Gdiplus::GraphicsPath* path,
                        const Gdiplus::RectF& box) {
    Gdiplus::REAL step = box.Height * 0.24f;
    Gdiplus::PointF pts[6] = {
        {box.X, box.Y},
        {box.X + box.Width * 0.42f, box.Y},
        {box.X + box.Width * 0.54f, box.Y + step},
        {box.X + box.Width, box.Y + step},
        {box.X + box.Width, box.Y + box.Height},
        {box.X, box.Y + box.Height},
    };
    path->AddPolygon(pts, 6);
}

// Extra top band reserved for the root folder title. Subfolder menus skip this.
int RootTitleBandHeight(const PopupLevel* level) {
    if (!level || level->depth != 0 || level->title.empty()) {
        return 0;
    }
    return ScaleForPopup(8) + ScaleForPopup(g_settings.titleFontSize) +
           ScaleForPopup(10);
}

// Fills level->cellRects and returns the required window size in physical
// pixels. When maxHeight > 0, auto columns are derived from how many rows fit
// in that height so large folders go wide instead of tall-and-narrow. When
// maxWidth > 0, columns are capped so the grid stays on the monitor; items
// beyond cols*rows are trimmed from level->items.
SIZE ComputeLevelLayout(PopupLevel* level,
                        int maxHeight = 0,
                        int maxWidth = 0) {
    int itemCount = (int)level->items.size();
    int padding = ScaleForPopup(8);
    int cellW = ScaleForPopup(g_settings.cellWidth);
    int cellH = ScaleForPopup(g_settings.cellHeight);
    int titleBand = RootTitleBandHeight(level);

    int count = std::max<int>(itemCount, 1);
    int cols;
    if (g_settings.columns > 0) {
        cols = std::min<int>(g_settings.columns, count);
    } else if (maxHeight > 0 && cellH > 0) {
        int usable = std::max(0, maxHeight - padding * 2 - titleBand);
        int maxRows = std::max<int>(1, usable / cellH);
        cols = (count + maxRows - 1) / maxRows;
        cols = std::clamp<int>(cols, 1, count);
        // Prefer a squarer grid when it still fits the available height.
        int squareCols = (int)std::ceil(std::sqrt((double)count));
        squareCols = std::clamp<int>(squareCols, cols, count);
        int squareRows = (count + squareCols - 1) / squareCols;
        if (squareRows <= maxRows) {
            cols = squareCols;
        }
    } else {
        cols = (int)std::ceil(std::sqrt((double)count));
        cols = std::clamp<int>(cols, 1, 6);
        cols = std::min<int>(cols, count);
    }

    if (maxWidth > 0 && cellW > 0) {
        int maxCols = std::max(1, (maxWidth - padding * 2) / cellW);
        cols = std::min(cols, maxCols);
    }

    int rows = (count + cols - 1) / cols;

    if (maxHeight > 0 && cellH > 0) {
        int usable = maxHeight - padding * 2 - titleBand;
        int maxRows = std::max<int>(1, usable / cellH);
        while (rows > maxRows && cols < count) {
            cols++;
            rows = (count + cols - 1) / cols;
        }
        // Height growth can widen past the monitor — re-apply the width cap.
        if (maxWidth > 0 && cellW > 0) {
            int maxCols = std::max(1, (maxWidth - padding * 2) / cellW);
            if (cols > maxCols) {
                cols = maxCols;
                rows = (count + cols - 1) / cols;
            }
        }
        if (rows > maxRows) {
            rows = maxRows;
        }
    }

    // Drop items that would fall outside the visible cols*rows grid.
    int capacity = cols * std::max(rows, 1);
    if (itemCount > capacity) {
        level->items.resize(capacity);
        itemCount = capacity;
        count = std::max<int>(itemCount, 1);
        rows = itemCount > 0 ? (itemCount + cols - 1) / cols : 1;
    }

    level->cellRects.clear();
    level->cellRects.reserve(itemCount);
    for (int i = 0; i < itemCount; i++) {
        int r = i / cols;
        int c = i % cols;
        RECT cell;
        cell.left = padding + c * cellW;
        cell.top = padding + titleBand + r * cellH;
        cell.right = cell.left + cellW;
        cell.bottom = cell.top + cellH;
        level->cellRects.push_back(cell);
    }

    SIZE size;
    size.cx = padding * 2 + cols * cellW;
    size.cy = padding * 2 + titleBand + rows * cellH;
    if (itemCount == 0) {
        // Leave room for the "Loading" / "Empty folder" message.
        size.cx = padding * 2 + cellW * 2;
        size.cy = std::max<int>(size.cy, padding * 2 + titleBand + cellH);
    }
    if (maxHeight > 0) {
        size.cy = std::min<int>(size.cy, maxHeight);
    }
    if (maxWidth > 0) {
        size.cx = std::min<int>(size.cx, maxWidth);
    }
    return size;
}

bool CanExpand(int depth) {
    if (g_settings.maxFolderDepth < 0) {
        return depth + 1 < kMaxLevels;
    }
    return depth < g_settings.maxFolderDepth && depth + 1 < kMaxLevels;
}

void InvalidateLevelBase(PopupLevel* level) {
    if (!level) {
        return;
    }
    level->baseDirty = true;
    level->cachedBase.reset();
    level->cachedBaseW = 0;
    level->cachedBaseH = 0;
}

// Icon, folder badge, and label for one grid cell. Shared by the static base
// paint and the hover/press cell overlay so the two stay visually identical.
void DrawCell(Gdiplus::Graphics& g,
              PopupLevel* level,
              int index,
              const Gdiplus::Color& textColor,
              const Gdiplus::Color& badgeFillColor,
              const Gdiplus::Color& badgeMarkColor,
              const Gdiplus::Color& badgeRingColor,
              Gdiplus::Font& font,
              Gdiplus::StringFormat& format) {
    if (!level || index < 0 || index >= (int)level->cellRects.size() ||
        index >= (int)level->items.size()) {
        return;
    }

    const RECT& cell = level->cellRects[index];
    int cellW = cell.right - cell.left;
    int cellH = cell.bottom - cell.top;
    const GridItem& item = level->items[index];

    int iconSize = ScaleForPopup(g_settings.iconSize);
    int iconTop = ScaleForPopup(10);
    int labelGap = ScaleForPopup(6);
    bool expandable = CanExpand(level->depth);

    Gdiplus::Rect iconRect(cell.left + (cellW - iconSize) / 2,
                           cell.top + iconTop, iconSize, iconSize);
    if (item.icon) {
        g.DrawImage(item.icon.get(), iconRect);
    }

    if (item.isFolder && expandable) {
        int badge = std::max<int>(14, (iconSize * 54 + 50) / 100);
        int badgeLeft = iconRect.GetRight() - badge + badge / 4;
        int badgeTop = iconRect.GetBottom() - badge + badge / 4;
        Gdiplus::Rect badgeRect(badgeLeft, badgeTop, badge, badge);

        Gdiplus::GraphicsPath badgePath;
        AddRoundedRect(&badgePath, badgeRect, badge / 2);
        Gdiplus::SolidBrush badgeBrush(badgeFillColor);
        g.FillPath(&badgeBrush, &badgePath);
        Gdiplus::Pen badgeRing(badgeRingColor, 1.0f);
        g.DrawPath(&badgeRing, &badgePath);

        Gdiplus::REAL glyphW = badge * 0.58f;
        Gdiplus::REAL glyphH = glyphW * 0.80f;
        Gdiplus::RectF glyphBox(
            badgeLeft + (badge - glyphW) / 2.0f,
            badgeTop + (badge - glyphH) / 2.0f + badge * 0.02f, glyphW, glyphH);
        Gdiplus::GraphicsPath glyphPath;
        AddFolderGlyphPath(&glyphPath, glyphBox);
        Gdiplus::SolidBrush markBrush(badgeMarkColor);
        g.FillPath(&markBrush, &glyphPath);
    }

    Gdiplus::SolidBrush textBrush(textColor);
    Gdiplus::RectF labelRect(
        (Gdiplus::REAL)(cell.left + ScaleForPopup(4)),
        (Gdiplus::REAL)(cell.top + iconTop + iconSize + labelGap),
        (Gdiplus::REAL)(cellW - ScaleForPopup(8)),
        (Gdiplus::REAL)(cellH - iconTop - iconSize - labelGap -
                        ScaleForPopup(4)));
    if (labelRect.Height > 0) {
        g.DrawString(item.displayName.c_str(), -1, &font, labelRect, &format,
                     &textBrush);
    }
}

// Paints the static panel (no hover/press chrome) into level->cachedBase.
bool RebuildLevelBase(PopupLevel* level, int width, int height) {
    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height,
                                                    PixelFormat32bppPARGB);
    if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    Gdiplus::Graphics g(bitmap.get());
    g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    bool dark = IsDarkTheme();
    BYTE panelAlpha =
        (BYTE)std::clamp<int>(g_settings.panelOpacity * 255 / 100, 10, 255);
    Gdiplus::Color panelColor = dark
                                    ? Gdiplus::Color(panelAlpha, 43, 43, 43)
                                    : Gdiplus::Color(panelAlpha, 249, 249, 249);
    Gdiplus::Color borderColor =
        dark ? Gdiplus::Color(40, 255, 255, 255) : Gdiplus::Color(28, 0, 0, 0);
    Gdiplus::Color textColor = dark ? Gdiplus::Color(255, 255, 255, 255)
                                    : Gdiplus::Color(255, 26, 26, 26);
    Gdiplus::Color badgeFillColor = GetAccentGdipColor(255);
    bool lightAccent = ((int)badgeFillColor.GetR() + badgeFillColor.GetG() +
                        badgeFillColor.GetB()) > 500;
    Gdiplus::Color badgeMarkColor = lightAccent
                                        ? Gdiplus::Color(255, 20, 20, 20)
                                        : Gdiplus::Color(255, 255, 255, 255);
    Gdiplus::Color badgeRingColor =
        dark ? Gdiplus::Color(160, 0, 0, 0) : Gdiplus::Color(70, 255, 255, 255);

    int radius = ScaleForPopup(g_settings.cornerRadius);
    Gdiplus::Rect panelRect(0, 0, width - 1, height - 1);
    Gdiplus::GraphicsPath panelPath;
    AddRoundedRect(&panelPath, panelRect, radius);

    Gdiplus::SolidBrush panelBrush(panelColor);
    g.FillPath(&panelBrush, &panelPath);
    Gdiplus::Pen borderPen(borderColor, 1.0f);
    g.DrawPath(&borderPen, &panelPath);

    PCWSTR fontName = PopupFontName();
    Gdiplus::FontFamily family(fontName);
    Gdiplus::Font font(&family, (Gdiplus::REAL)ScaleForPopup(g_settings.fontSize),
                       Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(textColor);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentNear);
    format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    format.SetFormatFlags(Gdiplus::StringFormatFlagsLineLimit);

    int titleBand = RootTitleBandHeight(level);
    if (titleBand > 0) {
        Gdiplus::Font titleFont(
            &family, (Gdiplus::REAL)ScaleForPopup(g_settings.titleFontSize),
            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat titleFormat;
        if (g_settings.titleAlign == L"left") {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentNear);
        } else if (g_settings.titleAlign == L"right") {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentFar);
        } else {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
        }
        titleFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        titleFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        titleFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        int pad = ScaleForPopup(12);
        Gdiplus::RectF titleRect(
            (Gdiplus::REAL)pad, (Gdiplus::REAL)ScaleForPopup(4),
            (Gdiplus::REAL)(width - pad * 2),
            (Gdiplus::REAL)(titleBand - ScaleForPopup(4)));
        g.DrawString(level->title.c_str(), -1, &titleFont, titleRect,
                     &titleFormat, &textBrush);
    }

    if (level->items.empty()) {
        Gdiplus::StringFormat centered;
        centered.SetAlignment(Gdiplus::StringAlignmentCenter);
        centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        PCWSTR message = level->loading ? L"Loading..." : L"Empty folder";
        Gdiplus::RectF area(0.0f, (Gdiplus::REAL)titleBand, (Gdiplus::REAL)width,
                            (Gdiplus::REAL)(height - titleBand));
        g.DrawString(message, -1, &font, area, &centered, &textBrush);
    }

    for (size_t i = 0; i < level->items.size() && i < level->cellRects.size();
         i++) {
        DrawCell(g, level, (int)i, textColor, badgeFillColor, badgeMarkColor,
                 badgeRingColor, font, format);
    }

    level->cachedBase = std::move(bitmap);
    level->cachedBaseW = width;
    level->cachedBaseH = height;
    level->baseDirty = false;
    return true;
}

void PaintLevel(PopupLevel* level) {
    if (!level || !level->hwnd) {
        return;
    }

    int width = level->rect.right - level->rect.left;
    int height = level->rect.bottom - level->rect.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    if (level->baseDirty || !level->cachedBase || level->cachedBaseW != width ||
        level->cachedBaseH != height) {
        if (!RebuildLevelBase(level, width, height)) {
            return;
        }
    }

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        return;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (!hMemDC) {
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        if (hBmp) {
            DeleteObject(hBmp);
        }
        DeleteDC(hMemDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }
    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);

    {
        Gdiplus::Bitmap surface(width, height, width * 4, PixelFormat32bppPARGB,
                                (BYTE*)bits);
        Gdiplus::Graphics g(&surface);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.DrawImage(level->cachedBase.get(), 0, 0, width, height);

        g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

        bool dark = IsDarkTheme();
        Gdiplus::Color hoverColor =
            dark ? Gdiplus::Color(28, 255, 255, 255) : Gdiplus::Color(18, 0, 0, 0);
        Gdiplus::Color pressColor =
            dark ? Gdiplus::Color(46, 255, 255, 255) : Gdiplus::Color(32, 0, 0, 0);
        Gdiplus::Color textColor = dark ? Gdiplus::Color(255, 255, 255, 255)
                                        : Gdiplus::Color(255, 26, 26, 26);
        Gdiplus::Color badgeFillColor = GetAccentGdipColor(255);
        bool lightAccent = ((int)badgeFillColor.GetR() + badgeFillColor.GetG() +
                            badgeFillColor.GetB()) > 500;
        Gdiplus::Color badgeMarkColor = lightAccent
                                            ? Gdiplus::Color(255, 20, 20, 20)
                                            : Gdiplus::Color(255, 255, 255, 255);
        Gdiplus::Color badgeRingColor = dark ? Gdiplus::Color(160, 0, 0, 0)
                                             : Gdiplus::Color(70, 255, 255, 255);

        PCWSTR fontName = PopupFontName();
        Gdiplus::FontFamily family(fontName);
        Gdiplus::Font font(&family,
                           (Gdiplus::REAL)ScaleForPopup(g_settings.fontSize),
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentNear);
        format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        format.SetFormatFlags(Gdiplus::StringFormatFlagsLineLimit);

        // Highlight under icon/label: clear only inside the rounded path, then
        // fill highlight and DrawCell. A full-cell SourceCopy FillRectangle
        // (added with cachedBase in v1.5) left a sharp translucent fringe in the
        // ScaleForPopup(2) inset margin that did not match the base panel alpha.
        auto repaintCell = [&](int cellIndex, bool pressed) {
            if (cellIndex < 0 || cellIndex >= (int)level->cellRects.size() ||
                cellIndex >= (int)level->items.size()) {
                return;
            }
            const RECT& cell = level->cellRects[cellIndex];
            int cellW = cell.right - cell.left;
            int cellH = cell.bottom - cell.top;

            Gdiplus::Rect highlight(
                cell.left + ScaleForPopup(2), cell.top + ScaleForPopup(2),
                cellW - ScaleForPopup(4), cellH - ScaleForPopup(4));
            Gdiplus::GraphicsPath highlightPath;
            AddRoundedRect(&highlightPath, highlight, ScaleForPopup(6));

            bool darkPanel = IsDarkTheme();
            BYTE panelAlpha = (BYTE)std::clamp<int>(
                g_settings.panelOpacity * 255 / 100, 10, 255);
            Gdiplus::Color panelColor =
                darkPanel ? Gdiplus::Color(panelAlpha, 43, 43, 43)
                          : Gdiplus::Color(panelAlpha, 249, 249, 249);
            Gdiplus::SolidBrush eraseBrush(panelColor);
            g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
            g.FillPath(&eraseBrush, &highlightPath);
            g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

            Gdiplus::SolidBrush highlightBrush(pressed ? pressColor
                                                       : hoverColor);
            g.FillPath(&highlightBrush, &highlightPath);

            DrawCell(g, level, cellIndex, textColor, badgeFillColor,
                     badgeMarkColor, badgeRingColor, font, format);
        };

        if (level->hoverCell >= 0 && level->hoverCell != level->pressedCell) {
            repaintCell(level->hoverCell, false);
        }
        if (level->pressedCell >= 0) {
            repaintCell(level->pressedCell, true);
        }
    }

    POINT ptSrc{0, 0};
    POINT ptDst{level->rect.left, level->rect.top};
    SIZE size{width, height};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(level->hwnd, hScreenDC, &ptDst, &size, hMemDC, &ptSrc, 0,
                        &blend, ULW_ALPHA);

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);
}

PopupLevel* LevelFromHwnd(HWND hWnd) {
    for (auto& level : Levels()) {
        if (level->hwnd == hWnd) {
            return level.get();
        }
    }
    return nullptr;
}

int CellFromClientPoint(PopupLevel* level, POINT pt) {
    for (size_t i = 0; i < level->cellRects.size() && i < level->items.size();
         i++) {
        if (PtInRect(&level->cellRects[i], pt)) {
            return (int)i;
        }
    }
    return -1;
}

// Hides every level at or below `depth`, deepest first.
void CloseLevelsFrom(int depth) {
    if (depth < 0) {
        depth = 0;
    }
    while ((int)Levels().size() > depth) {
        auto& level = Levels().back();
        if (level->hwnd) {
            ShowWindow(level->hwnd, SW_HIDE);
        }
        Levels().pop_back();
    }
    if (g_pendingSubDepth >= depth - 1) {
        g_pendingSubDepth = -1;
        g_pendingSubCell = -1;
    }
    g_closeDeeperSinceTick = 0;
}

void CloseChain() {
    if (!Levels().empty() && Levels()[0]->hwnd) {
        KillTimer(Levels()[0]->hwnd, kTickTimerId);
    }
    CloseLevelsFrom(0);
    g_outsideSinceTick = 0;
}

RECT BridgingRect(const RECT& a, const RECT& b) {
    RECT bridge;
    bridge.left = std::min<LONG>(a.left, b.left);
    bridge.right = std::max<LONG>(a.right, b.right);
    bridge.top = std::min<LONG>(a.top, b.top);
    bridge.bottom = std::max<LONG>(a.bottom, b.bottom);
    return bridge;
}

// The dead-zone-free path from the taskbar button up into the root grid. Only
// the gap BETWEEN them counts - the taskbar strip itself is excluded, so moving
// to another app on the taskbar closes the menu instead of keeping it stuck
// open.
bool CursorInRootCorridor(POINT pt) {
    if (Levels().empty()) {
        return false;
    }

    const RECT& anchor = g_rootAnchorRect;
    const RECT& popup = Levels()[0]->rect;

    RECT gap{};
    gap.left = std::min<LONG>(anchor.left, popup.left);
    gap.right = std::max<LONG>(anchor.right, popup.right);

    if (popup.bottom <= anchor.top) {
        // Normal bottom taskbar: popup sits above the button.
        gap.top = popup.bottom;
        gap.bottom = anchor.top;
    } else if (popup.top >= anchor.bottom) {
        // Top taskbar: popup sits below the button.
        gap.top = anchor.bottom;
        gap.bottom = popup.top;
    } else {
        return false;
    }

    // PtInRect excludes the bottom edge; inflate a zero-height seam so a
    // gapAbove of 0 still lets the cursor cross into the grid.
    if (gap.bottom <= gap.top) {
        gap.bottom = gap.top + 1;
    }

    return PtInRect(&gap, pt);
}

// The cascade stays open while the cursor is over the taskbar button, over any
// open grid, in the gap from the button up into the root grid, or in the band
// bridging two consecutive grids. The taskbar strip outside the button is NOT
// part of the live path.
bool CursorIsInLivePath() {
    POINT pt;
    if (!GetCursorPos(&pt) || Levels().empty()) {
        return false;
    }

    if (PtInRect(&g_rootAnchorRect, pt)) {
        return true;
    }

    for (const auto& level : Levels()) {
        if (PtInRect(&level->rect, pt)) {
            return true;
        }
    }

    if (CursorInRootCorridor(pt)) {
        return true;
    }

    for (size_t i = 1; i < Levels().size(); i++) {
        RECT bridge = BridgingRect(Levels()[i - 1]->rect, Levels()[i]->rect);
        if (PtInRect(&bridge, pt)) {
            return true;
        }
    }

    return false;
}

// The deepest open grid containing the cursor, or -1 when it is elsewhere.
int LevelIndexUnderCursor() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return -1;
    }
    for (int i = (int)Levels().size() - 1; i >= 0; i--) {
        if (PtInRect(&Levels()[i]->rect, pt)) {
            return i;
        }
    }
    return -1;
}

void ShowItemContextMenu(std::wstring path, POINT screenPt) {
    if (!g_menuOwnerWnd ||
        GetWindowThreadProcessId(g_menuOwnerWnd, nullptr) !=
            GetCurrentThreadId()) {
        return;
    }
    if (path.empty()) {
        return;
    }

    // Stay raised for the whole modal body, including InvokeCommand which also
    // pumps messages - otherwise OnTick can tear down levels under the menu.
    g_menuActive = true;

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl) {
        g_menuActive = false;
        return;
    }

    IShellFolder* parentFolder = nullptr;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(pidl, IID_IShellFolder, (void**)&parentFolder,
                              &child)) ||
        !parentFolder) {
        CoTaskMemFree(pidl);
        g_menuActive = false;
        return;
    }

    IContextMenu* contextMenu = nullptr;
    if (SUCCEEDED(parentFolder->GetUIObjectOf(g_menuOwnerWnd, 1, &child,
                                              IID_IContextMenu, nullptr,
                                              (void**)&contextMenu)) &&
        contextMenu) {
        HMENU menu = CreatePopupMenu();
        if (menu) {
            if (SUCCEEDED(contextMenu->QueryContextMenu(
                    menu, 0, 1, 0x7FFF, CMF_NORMAL | CMF_EXPLORE))) {
                g_activeContextMenu3 = nullptr;
                g_activeContextMenu2 = nullptr;
                if (FAILED(contextMenu->QueryInterface(
                        IID_IContextMenu3, (void**)&g_activeContextMenu3))) {
                    contextMenu->QueryInterface(IID_IContextMenu2,
                                                (void**)&g_activeContextMenu2);
                }

                // A never-shown 0x0 owner cannot become foreground; park a 1x1
                // window at the click so TrackPopupMenuEx dismisses correctly.
                SetWindowPos(g_menuOwnerWnd, HWND_TOPMOST, screenPt.x, screenPt.y,
                             1, 1, SWP_SHOWWINDOW);
                SetForegroundWindow(g_menuOwnerWnd);
                UINT cmd = TrackPopupMenuEx(menu,
                                            TPM_RETURNCMD | TPM_RIGHTBUTTON |
                                                TPM_LEFTALIGN | TPM_BOTTOMALIGN,
                                            screenPt.x, screenPt.y,
                                            g_menuOwnerWnd, nullptr);
                PostMessageW(g_menuOwnerWnd, WM_NULL, 0, 0);

                if (g_activeContextMenu3) {
                    g_activeContextMenu3->Release();
                    g_activeContextMenu3 = nullptr;
                }
                if (g_activeContextMenu2) {
                    g_activeContextMenu2->Release();
                    g_activeContextMenu2 = nullptr;
                }

                if (cmd) {
                    // Dismiss the hover chain before the shell verb runs so
                    // Explorer windows are not covered by stale grids.
                    CloseChain();
                    CMINVOKECOMMANDINFOEX info{};
                    info.cbSize = sizeof(info);
                    info.fMask = CMIC_MASK_UNICODE;
                    info.hwnd = g_menuOwnerWnd;
                    info.lpVerb = (LPCSTR)(INT_PTR)(cmd - 1);
                    info.lpVerbW = (LPCWSTR)(INT_PTR)(cmd - 1);
                    info.nShow = SW_SHOWNORMAL;
                    contextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
                }

                ShowWindow(g_menuOwnerWnd, SW_HIDE);
            }
            DestroyMenu(menu);
        }
        contextMenu->Release();
    }

    parentFolder->Release();
    CoTaskMemFree(pidl);
    g_menuActive = false;
}

// Launch on the UI thread via SEE_MASK_ASYNCOK so the shell does the work on
// its own thread - no detached worker that can outlive the mod DLL.
void LaunchPath(const std::wstring& path) {
    if (path.empty()) {
        return;
    }

    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask =
        SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI | SEE_MASK_ASYNCOK;
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"ShellExecuteEx failed for %s (error %u)", path.c_str(),
               GetLastError());
    }
}

LRESULT CALLBACK MenuOwnerWndProc(HWND hWnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    // New monitors create Shell_SecondaryTrayWnd without StartTaskbar; kick a
    // retry so secondary taskbars still get folder buttons.
    if (uMsg == WM_DISPLAYCHANGE && !g_unloading) {
        StartRetryThread();
    }

    if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM ||
        uMsg == WM_MEASUREITEM || uMsg == WM_MENUCHAR) {
        if (g_activeContextMenu3) {
            LRESULT result = 0;
            if (SUCCEEDED(g_activeContextMenu3->HandleMenuMsg2(
                    uMsg, wParam, lParam, &result))) {
                return result;
            }
        }
        if (g_activeContextMenu2 &&
            SUCCEEDED(
                g_activeContextMenu2->HandleMenuMsg(uMsg, wParam, lParam))) {
            return uMsg == WM_INITMENUPOPUP ? 0 : TRUE;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Re-copies any level still waiting on its first scan. Levels resize as their
// contents arrive, so this reopens rather than just repainting.
void RefreshLoadingLevels() {
    for (size_t i = 0; i < Levels().size(); i++) {
        auto* level = Levels()[i].get();
        if (!level->loading) {
            continue;
        }
        auto data = GetFolderData(level->path);
        if (!data || !data->ready) {
            continue;
        }
        // Copied out first: reopening destroys the level these came from.
        std::wstring path = level->path;
        std::wstring title = level->title;
        RECT anchorRect = level->anchorRect;
        int spawnerCell = level->spawnerCell;
        if (i == 0) {
            OpenRootLevel(std::move(path), anchorRect, std::move(title));
        } else {
            OpenSubLevel((int)i - 1, spawnerCell);
        }
        return;
    }
}

// Queues scans for the subfolders of a level so cascading into one is instant.
// Capped so a folder full of folders does not flood the scan thread.
void PrefetchSubfolders(PopupLevel* level) {
    if (!CanExpand(level->depth)) {
        return;
    }
    constexpr int kMaxPrefetch = 12;
    int iconPixelSize = ScaleForPopup(g_settings.iconSize);
    int queued = 0;
    for (const auto& item : level->items) {
        if (queued >= kMaxPrefetch) {
            break;
        }
        if (item.isFolder && !GetFolderData(item.fullPath)) {
            RequestScan(item.fullPath, iconPixelSize);
            queued++;
        }
    }
}

// Drives the cascade: opens a pending submenu once its dwell time elapses,
// retires levels the cursor has moved away from, and dismisses the chain when
// the cursor leaves the live path. Runs on level 0's window only.
void OnTick() {
    if (g_menuActive) {
        g_outsideSinceTick = 0;
        g_closeDeeperSinceTick = 0;
        return;
    }

    RefreshLoadingLevels();

    ULONGLONG now = GetTickCount64();

    if (!CursorIsInLivePath()) {
        if (g_outsideSinceTick == 0) {
            g_outsideSinceTick = now;
        } else if (now - g_outsideSinceTick >=
                   (ULONGLONG)g_settings.closeDelayMs) {
            CloseChain();
        }
        return;
    }
    g_outsideSinceTick = 0;

    int cursorLevel = LevelIndexUnderCursor();

    // Retire anything deeper than the grid the cursor is in, unless the cursor
    // is still resting on the cell that opened the next one down.
    if (cursorLevel >= 0 && (int)Levels().size() > cursorLevel + 1) {
        auto* level = Levels()[cursorLevel].get();
        bool onSpawner =
            level->hoverCell >= 0 &&
            level->hoverCell == Levels()[cursorLevel + 1]->spawnerCell;
        if (onSpawner) {
            g_closeDeeperSinceTick = 0;
        } else if (g_closeDeeperSinceTick == 0) {
            g_closeDeeperSinceTick = now;
        } else if (now - g_closeDeeperSinceTick >=
                   (ULONGLONG)g_settings.submenuCloseDelayMs) {
            CloseLevelsFrom(cursorLevel + 1);
        }
    } else {
        g_closeDeeperSinceTick = 0;
    }

    if (g_pendingSubDepth >= 0 &&
        now - g_pendingSubSinceTick >= (ULONGLONG)g_settings.submenuDelayMs) {
        int depth = g_pendingSubDepth;
        int cell = g_pendingSubCell;
        g_pendingSubDepth = -1;
        g_pendingSubCell = -1;
        OpenSubLevel(depth, cell);
    }
}

LRESULT CALLBACK PopupWndProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) {
    // The open-delay timer fires before any level exists, so it is handled
    // before the level lookup.
    if (uMsg == WM_TIMER && wParam == kOpenTimerId) {
        KillTimer(hWnd, kOpenTimerId);
        std::wstring path = std::move(g_pendingRootPath);
        std::wstring title = std::move(g_pendingRootTitle);
        g_pendingRootPath.clear();
        g_pendingRootTitle.clear();
        POINT cursor{};
        // The cursor may have moved on during the delay.
        if (!path.empty() && GetCursorPos(&cursor) &&
            PtInRect(&g_pendingRootRect, cursor)) {
            OpenRootLevel(std::move(path), g_pendingRootRect, std::move(title));
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == kTickTimerId) {
        OnTick();
        return 0;
    }

    // Theme/display messages do not need a live level; handle them even when
    // the HWND is a reused shell with no PopupLevel attached yet. Top-level
    // popups receive WM_SETTINGCHANGE broadcasts even while hidden, so only
    // dismiss an open chain — otherwise every SPI broadcast would CloseChain
    // and cancel pending opens. Monitor reinject is owned by MenuOwnerWndProc.
    if (uMsg == WM_DISPLAYCHANGE || uMsg == WM_SETTINGCHANGE ||
        uMsg == WM_THEMECHANGED) {
        RefreshThemeCache();
        if (!Levels().empty()) {
            CloseChain();
        }
        return 0;
    }

    PopupLevel* level = LevelFromHwnd(hWnd);
    if (!level) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_MOUSEMOVE: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            if (cell != level->hoverCell) {
                level->hoverCell = cell;
                PaintLevel(level);

                bool expandable =
                    cell >= 0 && cell < (int)level->items.size() &&
                    level->items[cell].isFolder && CanExpand(level->depth);
                int alreadyOpen = (int)Levels().size() > level->depth + 1
                                      ? Levels()[level->depth + 1]->spawnerCell
                                      : -1;
                if (expandable && cell != alreadyOpen) {
                    g_pendingSubDepth = level->depth;
                    g_pendingSubCell = cell;
                    g_pendingSubSinceTick = GetTickCount64();
                } else {
                    g_pendingSubDepth = -1;
                    g_pendingSubCell = -1;
                }
            }
            g_outsideSinceTick = 0;
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            level->pressedCell = CellFromClientPoint(level, pt);
            PaintLevel(level);
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            int pressed = level->pressedCell;
            level->pressedCell = -1;
            if (cell >= 0 && cell == pressed &&
                cell < (int)level->items.size()) {
                std::wstring path = level->items[cell].fullPath;
                CloseChain();
                LaunchPath(path);
            } else {
                PaintLevel(level);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            if (cell >= 0 && cell < (int)level->items.size()) {
                POINT screenPt = pt;
                ClientToScreen(hWnd, &screenPt);
                ShowItemContextMenu(std::wstring(level->items[cell].fullPath),
                                    screenPt);
            }
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, kTickTimerId);
            KillTimer(hWnd, kOpenTimerId);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsurePopupClasses() {
    enum class ClassState { Untried, Ok, Failed };
    static ClassState classState = ClassState::Untried;
    if (classState != ClassState::Untried) {
        return classState == ClassState::Ok;
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW popupClass{};
    popupClass.cbSize = sizeof(popupClass);
    popupClass.lpfnWndProc = PopupWndProc;
    popupClass.hInstance = instance;
    popupClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    popupClass.lpszClassName = kPopupClassName;
    ATOM popupAtom = RegisterClassExW(&popupClass);

    WNDCLASSEXW ownerClass{};
    ownerClass.cbSize = sizeof(ownerClass);
    ownerClass.lpfnWndProc = MenuOwnerWndProc;
    ownerClass.hInstance = instance;
    ownerClass.lpszClassName = kMenuOwnerClassName;
    ATOM ownerAtom = RegisterClassExW(&ownerClass);

    if (!popupAtom || !ownerAtom) {
        Wh_Log(L"RegisterClassExW failed (error %u); hover grids disabled",
               GetLastError());
        if (popupAtom) {
            UnregisterClassW(kPopupClassName, instance);
        }
        if (ownerAtom) {
            UnregisterClassW(kMenuOwnerClassName, instance);
        }
        classState = ClassState::Failed;
        return false;
    }

    classState = ClassState::Ok;
    return true;
}

HWND EnsureMenuOwnerWindow() {
    if (g_menuOwnerWnd) {
        // DestroyWindow/CreateWindow must run on the owning thread; a wrong-
        // thread caller cannot replace the window safely.
        if (GetWindowThreadProcessId(g_menuOwnerWnd, nullptr) !=
            GetCurrentThreadId()) {
            Wh_Log(L"Menu owner window belongs to a different thread; refusing "
                   L"to use or recreate it from this thread");
            return nullptr;
        }
        return g_menuOwnerWnd;
    }
    if (!EnsurePopupClasses()) {
        return nullptr;
    }

    // Top-level so it receives WM_DISPLAYCHANGE when a monitor is plugged in.
    // Kept hidden until a shell context menu needs an activatable owner.
    // Must be created on the taskbar UI thread (via InjectHostGrids /
    // EnsureLevelWindow).
    g_menuOwnerWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW, kMenuOwnerClassName, L"", WS_POPUP, 0, 0, 0, 0,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_menuOwnerWnd) {
        Wh_Log(L"Failed to create menu owner window (error %u)", GetLastError());
    }
    return g_menuOwnerWnd;
}

// Returns the reusable window for the given cascade depth, creating it on first
// use. Depth 0's window also owns the tick timer.
HWND EnsureLevelWindow(int depth) {
    if (depth < 0 || depth >= kMaxLevels) {
        return nullptr;
    }
    if (depth < (int)g_levelWindows.size() && g_levelWindows[depth]) {
        return g_levelWindows[depth];
    }

    if (!EnsurePopupClasses()) {
        return nullptr;
    }
    // Best-effort: owner is only required for shell context menus. A failure
    // must not block creating the hover grid window itself.
    EnsureMenuOwnerWindow();

    HWND hWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kPopupClassName, L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (!hWnd) {
        Wh_Log(L"Failed to create the hover grid window (error %u)",
               GetLastError());
        return nullptr;
    }

    ApplyBackdrop(hWnd);

    if ((int)g_levelWindows.size() <= depth) {
        g_levelWindows.resize(depth + 1, nullptr);
    }
    g_levelWindows[depth] = hWnd;
    return hWnd;
}

void UpdatePopupDpi() {
    // Sub-levels intentionally keep the root's DPI so a cascade that spills
    // onto another monitor does not suddenly change cell sizes mid-gesture.
    HWND reference = g_taskbarWnd;
    if (!reference && !g_levelWindows.empty()) {
        reference = g_levelWindows[0];
    }
    if (reference) {
        int dpi = (int)GetDpiForWindow(reference);
        g_popupDpi = dpi > 0 ? dpi : 96;
    }
}

// Seats a level at its depth and retires anything deeper. The window at this
// depth stays mapped, so reopening the same depth resizes in place instead of
// blinking.
PopupLevel* InstallLevel(std::unique_ptr<PopupLevel> level) {
    int depth = level->depth;
    CloseLevelsFrom(depth + 1);
    if ((int)Levels().size() <= depth) {
        Levels().resize(depth);
        Levels().push_back(std::move(level));
    } else {
        Levels()[depth] = std::move(level);
    }
    return Levels()[depth].get();
}

// Places and shows a level's window, then paints it.
void PresentLevel(PopupLevel* level, const SIZE& size) {
    level->rect.right = level->rect.left + size.cx;
    level->rect.bottom = level->rect.top + size.cy;
    InvalidateLevelBase(level);

    SetWindowPos(level->hwnd, HWND_TOPMOST, level->rect.left, level->rect.top,
                 size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    PaintLevel(level);
}

void OpenRootLevel(std::wstring path, RECT anchorRect, std::wstring title) {
    if (g_unloading || path.empty()) {
        return;
    }

    HWND hWnd = EnsureLevelWindow(0);
    if (!hWnd) {
        return;
    }

    UpdatePopupDpi();

    auto level = std::make_unique<PopupLevel>();
    level->hwnd = hWnd;
    level->depth = 0;
    level->path = path;
    level->title = std::move(title);
    level->anchorRect = anchorRect;

    auto data =
        GetFolderDataAndRefresh(path, ScaleForPopup(g_settings.iconSize));
    level->loading = true;
    if (data && data->ready) {
        level->items = data->items;
        level->loading = false;
    }

    g_rootAnchorRect = anchorRect;
    g_outsideSinceTick = 0;

    HMONITOR monitor = MonitorFromRect(&anchorRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    const RECT& screen = monitorInfo.rcMonitor;

    int gap = ScaleForPopup(g_settings.gapAbove);
    int availableAbove = anchorRect.top - gap - screen.top;
    int availableBelow = screen.bottom - anchorRect.bottom - gap;
    int maxHeight = std::max(availableAbove, availableBelow);
    if (maxHeight < ScaleForPopup(96)) {
        maxHeight = std::max(ScaleForPopup(96),
                             (int)(screen.bottom - screen.top) - ScaleForPopup(16));
    }
    int maxWidth =
        std::max(0, (int)(screen.right - screen.left) - ScaleForPopup(8));

    SIZE size = ComputeLevelLayout(level.get(), maxHeight, maxWidth);

    int centerX = (anchorRect.left + anchorRect.right) / 2;
    int left = centerX - size.cx / 2;

    // Prefer above the taskbar; never flip below a bottom taskbar where the
    // grid would land past the monitor edge.
    if (availableAbove >= size.cy) {
        level->rect.top = anchorRect.top - gap - size.cy;
    } else if (availableBelow >= size.cy) {
        level->rect.top = anchorRect.bottom + gap;
    } else if (availableAbove >= availableBelow) {
        level->rect.top = screen.top;
    } else {
        level->rect.top = std::max<int>(screen.top, screen.bottom - size.cy);
    }
    level->rect.top = std::clamp<int>(
        level->rect.top, screen.top,
        std::max<int>(screen.top, screen.bottom - size.cy));
    int loLeft = screen.left + ScaleForPopup(4);
    int hiLeft = screen.right - size.cx - ScaleForPopup(4);
    level->rect.left =
        hiLeft <= loLeft ? screen.left : std::clamp(left, loLeft, hiLeft);

    auto* installed = InstallLevel(std::move(level));
    PresentLevel(installed, size);
    PrefetchSubfolders(installed);

    SetTimer(hWnd, kTickTimerId, kTickTimerMs, nullptr);
}

// Opens the grid for the subfolder in `cell` of the level at `parentDepth`,
// cascading to the side of the parent like a normal submenu.
void OpenSubLevel(int parentDepth, int cell) {
    if (g_unloading || parentDepth < 0 || parentDepth >= (int)Levels().size()) {
        return;
    }

    auto* parent = Levels()[parentDepth].get();
    if (cell < 0 || cell >= (int)parent->items.size() ||
        !parent->items[cell].isFolder || !CanExpand(parentDepth)) {
        return;
    }

    int depth = parentDepth + 1;
    HWND hWnd = EnsureLevelWindow(depth);
    if (!hWnd) {
        return;
    }

    auto level = std::make_unique<PopupLevel>();
    level->hwnd = hWnd;
    level->depth = depth;
    level->path = parent->items[cell].fullPath;
    level->spawnerCell = cell;

    RECT cellScreenRect = parent->cellRects[cell];
    OffsetRect(&cellScreenRect, parent->rect.left, parent->rect.top);
    level->anchorRect = cellScreenRect;

    auto data = GetFolderDataAndRefresh(level->path,
                                        ScaleForPopup(g_settings.iconSize));
    level->loading = true;
    if (data && data->ready) {
        level->items = data->items;
        level->loading = false;
    }

    HMONITOR monitor = MonitorFromRect(&parent->rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    const RECT& screen = monitorInfo.rcMonitor;
    const RECT& screenWork = monitorInfo.rcWork;

    // Cap height to the work area (excludes the taskbar) and width to the
    // parent monitor so cascade grids stay on-screen.
    int maxHeight = std::max(
        0, (int)(screenWork.bottom - screenWork.top) - ScaleForPopup(16));
    int maxWidth =
        std::max(0, (int)(screen.right - screen.left) - ScaleForPopup(8));
    SIZE size = ComputeLevelLayout(level.get(), maxHeight, maxWidth);

    int seam = ScaleForPopup(2);
    int left = parent->rect.right + seam;
    if (left + size.cx > screen.right) {
        int flipped = parent->rect.left - seam - size.cx;
        left = flipped >= screen.left
                   ? flipped
                   : std::max<int>(screen.left, screen.right - size.cx);
    }

    // Top-aligned with the cell that opened it, kept within the work area.
    int top = cellScreenRect.top - ScaleForPopup(8);
    top = std::clamp<int>(
        top, screenWork.top,
        std::max<int>(screenWork.top, screenWork.bottom - size.cy));

    int loLeft = screen.left;
    int hiLeft = screen.right - size.cx;
    level->rect.left =
        hiLeft <= loLeft ? screen.left : std::clamp(left, loLeft, hiLeft);
    level->rect.top = top;

    auto* installed = InstallLevel(std::move(level));
    PresentLevel(installed, size);
    PrefetchSubfolders(installed);
}

////////////////////////////////////////////////////////////////////////////////
// The taskbar buttons

constexpr PCWSTR kHostGridName = L"FolderHoverTrayHost";
constexpr double kFallbackButtonSize = 44.0;

struct ButtonState {
    Button button{nullptr};
    winrt::event_token clickToken{};
    winrt::event_token enterToken{};
    winrt::event_token exitToken{};
    int folderIndex = -1;
};

// One injected host per taskbar window (primary + each secondary monitor).
struct TaskbarHost {
    HWND hwnd = nullptr;
    Grid hostGrid{nullptr};
    Grid trackedRootGrid{nullptr};
    FrameworkElement anchor{nullptr};
    FrameworkElement cachedRepeater{nullptr};
    std::vector<ButtonState> buttonStates;
    Thickness anchorOriginalMargin{};
    bool hasAnchorOriginalMargin = false;
    winrt::event_token layoutUpdatedToken{};
    double buttonWidth = kFallbackButtonSize;
    double buttonHeight = kFallbackButtonSize;
    ULONGLONG lastAnchorResolveTick = 0;
    ULONGLONG lastButtonSizeTick = 0;
};

[[clang::no_destroy]] std::optional<std::vector<std::unique_ptr<TaskbarHost>>>
    g_taskbarHosts{std::in_place};

std::vector<std::unique_ptr<TaskbarHost>>& TaskbarHosts() {
    if (!g_taskbarHosts) {
        g_taskbarHosts.emplace();
    }
    return *g_taskbarHosts;
}

std::atomic<bool> g_injectionLive{false};

// Lifetime retry worker: stays alive until StopRetryThread (settings/uninit).
// StartRetryThread ensures the worker exists and SetEvent(kick) — never joins
// from a window proc. A HANDLE has no destructor that aborts at process exit.
// StopRetryThread joins with MsgWaitForMultipleObjects so settings/uninit can
// wait without deadlocking on inbound SendMessage.
HANDLE g_retryThread = nullptr;
HANDLE g_retryStopEvent = nullptr;   // manual-reset
HANDLE g_retryKickEvent = nullptr;   // auto-reset
SRWLOCK g_retryLock = SRWLOCK_INIT;

// WriteableBitmap's backing store is reachable only through this interop
// interface, which the C++/WinRT projection does not declare for us.
struct IBufferByteAccess : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};

constexpr GUID kIBufferByteAccessGuid = {
    0x905a0fef,
    0xbc53,
    0x11df,
    {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};

ImageSource HIconToImageSource(HICON hIcon, int size) {
    try {
        auto bitmap = HIconToBitmap(hIcon, size);
        if (!bitmap) {
            return nullptr;
        }

        Gdiplus::BitmapData bd{};
        Gdiplus::Rect lockRect(0, 0, size, size);
        // HIconToBitmap stores PARGB; WriteableBitmap also wants premultiplied
        // BGRA.
        if (bitmap->LockBits(&lockRect, Gdiplus::ImageLockModeRead,
                             PixelFormat32bppPARGB, &bd) != Gdiplus::Ok) {
            return nullptr;
        }

        try {
            // Activation can throw; keep it inside the LockBits guard.
            winrt::Windows::UI::Xaml::Media::Imaging::WriteableBitmap writeable(
                size, size);
            auto buffer = writeable.PixelBuffer();
            IBufferByteAccess* byteAccess = nullptr;
            auto* bufferUnknown = (IUnknown*)winrt::get_abi(buffer);
            if (bufferUnknown &&
                SUCCEEDED(bufferUnknown->QueryInterface(kIBufferByteAccessGuid,
                                                        (void**)&byteAccess)) &&
                byteAccess) {
                BYTE* dest = nullptr;
                if (SUCCEEDED(byteAccess->Buffer(&dest)) && dest) {
                    for (int y = 0; y < size; y++) {
                        memcpy(dest + (size_t)y * size * 4,
                               (const BYTE*)bd.Scan0 + (size_t)y * bd.Stride,
                               (size_t)size * 4);
                    }
                }
                byteAccess->Release();
            }
            bitmap->UnlockBits(&bd);
            writeable.Invalidate();
            return writeable;
        } catch (...) {
            bitmap->UnlockBits(&bd);
            return nullptr;
        }
    } catch (...) {
        return nullptr;
    }
}

void SetButtonBrush(Button const& button, PCWSTR key, Brush const& brush) {
    button.Resources().Insert(winrt::box_value(winrt::hstring(key)), brush);
}

Brush MakeBrush(BYTE a, BYTE r, BYTE g, BYTE b) {
    return SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(a, r, g, b));
}

void ApplyNativeButtonStyle(Button const& button) {
    bool dark = IsDarkTheme();

    SetButtonBrush(button, L"ButtonBackground", MakeBrush(0, 0, 0, 0));
    SetButtonBrush(
        button, L"ButtonBackgroundPointerOver",
        dark ? MakeBrush(28, 255, 255, 255) : MakeBrush(18, 0, 0, 0));
    SetButtonBrush(
        button, L"ButtonBackgroundPressed",
        dark ? MakeBrush(46, 255, 255, 255) : MakeBrush(32, 0, 0, 0));
    SetButtonBrush(button, L"ButtonBackgroundDisabled", MakeBrush(0, 0, 0, 0));

    for (PCWSTR key : {L"ButtonBorderBrush", L"ButtonBorderBrushPointerOver",
                       L"ButtonBorderBrushPressed"}) {
        SetButtonBrush(button, key, MakeBrush(0, 0, 0, 0));
    }

    auto foreground =
        dark ? MakeBrush(255, 255, 255, 255) : MakeBrush(255, 26, 26, 26);
    for (PCWSTR key : {L"ButtonForeground", L"ButtonForegroundPointerOver",
                       L"ButtonForegroundPressed"}) {
        SetButtonBrush(button, key, foreground);
    }
    button.Foreground(foreground);

    button.BorderThickness({0, 0, 0, 0});
    button.Padding({0, 0, 0, 0});
    button.MinWidth(0);
    button.MinHeight(0);
    button.CornerRadius({6, 6, 6, 6});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    button.UseSystemFocusVisuals(false);
}

UIElement MakeButtonContent(const FolderEntry& entry, double buttonSize) {
    int iconExtent = g_settings.buttonIconSize > 0
                         ? g_settings.buttonIconSize
                         : (int)std::lround(buttonSize * 0.545);
    iconExtent = std::clamp<int>(iconExtent, 8, 128);

    // Emoji or any other short literal text.
    if (!entry.icon.empty() && !IconSettingIsFile(entry.icon)) {
        TextBlock text;
        text.Text(entry.icon);
        text.FontFamily(FontFamily(L"Segoe UI Emoji"));
        text.FontSize(iconExtent * 0.92);
        text.HorizontalAlignment(HorizontalAlignment::Center);
        text.VerticalAlignment(VerticalAlignment::Center);
        text.TextAlignment(TextAlignment::Center);
        text.IsTextSelectionEnabled(false);
        return text;
    }

    Image image;
    image.Width(iconExtent);
    image.Height(iconExtent);
    image.Stretch(Stretch::Uniform);
    image.HorizontalAlignment(HorizontalAlignment::Center);
    image.VerticalAlignment(VerticalAlignment::Center);

    bool loaded = false;

    if (!entry.icon.empty()) {
        std::wstring icon = entry.icon;
        bool isImageFile = false;
        for (PCWSTR ext : {L".ico", L".png", L".jpg", L".jpeg", L".bmp",
                           L".gif", L".tif", L".tiff"}) {
            size_t len = wcslen(ext);
            if (icon.size() > len &&
                _wcsicmp(icon.c_str() + icon.size() - len, ext) == 0) {
                isImageFile = true;
                break;
            }
        }

        if (isImageFile) {
            try {
                WCHAR url[2048];
                DWORD urlLen = ARRAYSIZE(url);
                if (SUCCEEDED(UrlCreateFromPathW(icon.c_str(), url, &urlLen,
                                                 0))) {
                    winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage
                        bitmap;
                    bitmap.DecodePixelWidth(iconExtent * 2);
                    bitmap.UriSource(
                        winrt::Windows::Foundation::Uri(winrt::hstring(url)));
                    image.Source(bitmap);
                    loaded = true;
                }
            } catch (...) {
                loaded = false;
            }
        }

        if (!loaded) {
            HICON hIcon = ExtractIconFromResourceSpec(icon, iconExtent * 2);
            if (hIcon) {
                auto source = HIconToImageSource(hIcon, iconExtent * 2);
                DestroyIcon(hIcon);
                if (source) {
                    image.Source(source);
                    loaded = true;
                }
            }
        }
    }

    // No icon configured, or the configured one failed: fall back to the
    // folder's own shell icon. Skip UNC/network paths — SHGetFileInfo can
    // block the taskbar UI thread for a long time on unavailable shares.
    if (!loaded) {
        std::wstring resolved = ResolveFolderPath(entry.path);
        if (!resolved.empty()) {
            if (PathIsUNCW(resolved.c_str())) {
                Wh_Log(L"Skipping sync shell icon for UNC path %s",
                       resolved.c_str());
            } else {
                HICON hIcon = GetShellIconForPath(resolved, iconExtent * 2);
                if (hIcon) {
                    auto source = HIconToImageSource(hIcon, iconExtent * 2);
                    DestroyIcon(hIcon);
                    if (source) {
                        image.Source(source);
                        loaded = true;
                    }
                }
            }
        }
    }

    if (!loaded) {
        TextBlock fallback;
        fallback.Text(L"\U0001F4C1");
        fallback.FontFamily(FontFamily(L"Segoe UI Emoji"));
        fallback.FontSize(iconExtent * 0.92);
        fallback.HorizontalAlignment(HorizontalAlignment::Center);
        fallback.VerticalAlignment(VerticalAlignment::Center);
        return fallback;
    }

    return image;
}

RECT GetElementScreenRect(FrameworkElement const& element) {
    RECT rect{};
    if (!element || !g_taskbarWnd) {
        return rect;
    }
    try {
        auto transform = element.TransformToVisual(nullptr);
        auto point = transform.TransformPoint({0, 0});

        double scale = 1.0;
        if (auto xamlRoot = element.XamlRoot()) {
            scale = xamlRoot.RasterizationScale();
        }
        if (scale <= 0.0) {
            scale = 1.0;
        }

        POINT origin{0, 0};
        ClientToScreen(g_taskbarWnd, &origin);

        rect.left = origin.x + (LONG)std::lround(point.X * scale);
        rect.top = origin.y + (LONG)std::lround(point.Y * scale);
        rect.right =
            rect.left + (LONG)std::lround(element.ActualWidth() * scale);
        rect.bottom =
            rect.top + (LONG)std::lround(element.ActualHeight() * scale);
    } catch (...) {
        rect = RECT{};
    }
    return rect;
}

FrameworkElement FindTaskbarRepeater(Grid const& rootGrid) {
    return FindChildByName(rootGrid, L"TaskbarFrameRepeater", 6);
}

// Matching a live taskbar button means the folder buttons stay the right size
// across taskbar-size changes, DPI changes, and the small-icons setting.
void UpdateButtonSizeFromTaskbar(FrameworkElement const& repeater,
                                 double* outWidth,
                                 double* outHeight) {
    std::vector<FrameworkElement> children;
    EnumRepeaterChildren(repeater, &children);

    double width = 0.0;
    double height = 0.0;

    for (const auto& child : children) {
        auto className = winrt::get_class_name(child);
        if (className != L"Taskbar.TaskListButton") {
            continue;
        }
        if (child.ActualWidth() > 1.0 && child.ActualHeight() > 1.0) {
            width = child.ActualWidth();
            height = child.ActualHeight();
            break;
        }
    }

    if (width <= 1.0) {
        for (const auto& child : children) {
            auto className = winrt::get_class_name(child);
            if (className != L"Taskbar.ExperienceToggleButton" &&
                className != L"Taskbar.AugmentedEntryPointButton") {
                continue;
            }
            if (child.ActualWidth() > 1.0 && child.ActualHeight() > 1.0) {
                width = child.ActualWidth();
                height = child.ActualHeight();
                break;
            }
        }
    }

    if (width <= 1.0 || height <= 1.0) {
        return;
    }

    if (outWidth) {
        *outWidth = width;
    }
    if (outHeight) {
        *outHeight = height;
    }
}

FrameworkElement ResolveAnchor(FrameworkElement const& repeater,
                               Grid const& rootGrid) {
    std::vector<FrameworkElement> children;
    EnumRepeaterChildren(repeater, &children);
    if (children.empty()) {
        return nullptr;
    }

    bool before = g_settings.position != L"afterApps";

    auto findByClass = [&](PCWSTR className, int skip) -> FrameworkElement {
        int seen = 0;
        for (const auto& child : children) {
            if (winrt::get_class_name(child) != className) {
                continue;
            }
            if (seen++ < skip) {
                continue;
            }
            return child;
        }
        return nullptr;
    };

    if (g_settings.anchor == L"widgets") {
        if (auto found = findByClass(L"Taskbar.AugmentedEntryPointButton", 0)) {
            return found;
        }
    } else if (g_settings.anchor == L"taskView") {
        if (auto found = findByClass(L"Taskbar.ExperienceToggleButton", 1)) {
            return found;
        }
    } else if (g_settings.anchor == L"start") {
        if (auto found = findByClass(L"Taskbar.ExperienceToggleButton", 0)) {
            return found;
        }
    }

    // Auto: the outermost app button on the requested side. The repeater's
    // realized children are not in layout order, so compare actual positions.
    FrameworkElement best = nullptr;
    double bestX = 0.0;
    for (const auto& child : children) {
        if (winrt::get_class_name(child) != L"Taskbar.TaskListButton") {
            continue;
        }
        if (child.Visibility() != Visibility::Visible ||
            child.ActualWidth() <= 1.0) {
            continue;
        }
        try {
            auto point =
                child.TransformToVisual(rootGrid).TransformPoint({0, 0});
            if (!best || (before ? point.X < bestX : point.X > bestX)) {
                best = child;
                bestX = point.X;
            }
        } catch (...) {
        }
    }
    if (best) {
        return best;
    }

    for (PCWSTR className : {L"Taskbar.AugmentedEntryPointButton",
                             L"Taskbar.ExperienceToggleButton"}) {
        if (auto found = findByClass(className, 0)) {
            return found;
        }
    }

    return nullptr;
}

double DesiredHostWidth(TaskbarHost* host) {
    if (!host || host->buttonStates.empty()) {
        return 0.0;
    }
    return host->buttonWidth * (double)host->buttonStates.size();
}

void RestoreAnchorMargin(TaskbarHost* host) {
    if (!host) {
        return;
    }
    if (host->anchor && host->hasAnchorOriginalMargin) {
        try {
            host->anchor.Margin(host->anchorOriginalMargin);
        } catch (...) {
        }
    }
    host->anchor = nullptr;
    host->hasAnchorOriginalMargin = false;
}

void AdoptAnchor(TaskbarHost* host, FrameworkElement const& anchor) {
    if (!host) {
        return;
    }
    if (host->anchor == anchor) {
        return;
    }
    RestoreAnchorMargin(host);
    host->anchor = anchor;
    if (anchor) {
        try {
            host->anchorOriginalMargin = anchor.Margin();
            host->hasAnchorOriginalMargin = true;
        } catch (...) {
            host->hasAnchorOriginalMargin = false;
        }
    }
}

TaskbarHost* FindTaskbarHost(HWND hwnd) {
    if (!g_taskbarHosts) {
        return nullptr;
    }
    for (auto& host : *g_taskbarHosts) {
        if (host && host->hwnd == hwnd) {
            return host.get();
        }
    }
    return nullptr;
}

TaskbarHost* FindTaskbarHostByRootGrid(Grid const& rootGrid) {
    if (!g_taskbarHosts) {
        return nullptr;
    }
    for (auto& host : *g_taskbarHosts) {
        if (host && host->trackedRootGrid == rootGrid) {
            return host.get();
        }
    }
    return nullptr;
}

// The vertical extent comes from the taskbar window rather than the button, so
// the corridor covers the whole taskbar height under the button. Horizontally
// the XAML transform is trusted only if it lands inside the taskbar; otherwise
// the cursor position, which is definitely over the button, is used instead.
RECT ComputeHoverAnchorRect(Button const& button) {
    RECT taskbarRect{};
    if (!g_taskbarWnd || !GetWindowRect(g_taskbarWnd, &taskbarRect)) {
        return RECT{};
    }

    RECT rect = GetElementScreenRect(button);
    LONG width = rect.right - rect.left;

    if (width <= 0 || rect.left < taskbarRect.left ||
        rect.right > taskbarRect.right) {
        POINT cursor{};
        if (!GetCursorPos(&cursor)) {
            return RECT{};
        }
        LONG half = width > 0 ? width / 2 : 22;
        if (half <= 0) {
            half = 22;
        }
        rect.left = cursor.x - half;
        rect.right = cursor.x + half;
        Wh_Log(L"Button rect fell outside the taskbar, using the cursor instead");
    }

    rect.top = taskbarRect.top;
    rect.bottom = taskbarRect.bottom;
    return rect;
}

std::wstring FolderPathForButton(int folderIndex) {
    if (folderIndex < 0 || folderIndex >= (int)g_settings.folders.size()) {
        return L"";
    }
    std::wstring path = ResolveFolderPath(g_settings.folders[folderIndex].path);
    return path.empty() ? g_settings.folders[folderIndex].path : path;
}

std::wstring FolderNameForButton(int folderIndex) {
    if (folderIndex < 0 || folderIndex >= (int)g_settings.folders.size()) {
        return L"";
    }
    return g_settings.folders[folderIndex].name;
}

void OnPointerEnteredButton(int folderIndex,
                             Button const& button,
                             HWND taskbarWnd) {
    if (g_unloading) {
        return;
    }

    if (taskbarWnd) {
        g_taskbarWnd = taskbarWnd;
    }

    std::wstring path = FolderPathForButton(folderIndex);
    if (path.empty()) {
        return;
    }
    std::wstring title = FolderNameForButton(folderIndex);

    RECT buttonRect = ComputeHoverAnchorRect(button);
    if (buttonRect.right <= buttonRect.left) {
        return;
    }

    if (g_settings.hoverDelayMs <= 0) {
        OpenRootLevel(std::move(path), buttonRect, std::move(title));
        return;
    }

    HWND hWnd = EnsureLevelWindow(0);
    if (!hWnd) {
        return;
    }
    g_pendingRootPath = std::move(path);
    g_pendingRootTitle = std::move(title);
    g_pendingRootRect = buttonRect;
    SetTimer(hWnd, kOpenTimerId, (UINT)g_settings.hoverDelayMs, nullptr);
}

// Leaving the button cancels a pending delayed open. Closing an already-open
// cascade is left to OnTick so moving up into the grid does not dismiss it.
void OnPointerExitedButton() {
    if (g_pendingRootPath.empty()) {
        return;
    }
    g_pendingRootPath.clear();
    g_pendingRootTitle.clear();
    if (!g_levelWindows.empty() && g_levelWindows[0]) {
        KillTimer(g_levelWindows[0], kOpenTimerId);
    }
}

void OnButtonClicked(int folderIndex) {
    if (!g_settings.openFolderOnClick) {
        return;
    }
    std::wstring path = FolderPathForButton(folderIndex);
    if (path.empty()) {
        return;
    }
    CloseChain();
    LaunchPath(path);
}

Grid BuildHostGrid(TaskbarHost* host) {
    Grid hostGrid;
    hostGrid.Name(kHostGridName);
    hostGrid.HorizontalAlignment(HorizontalAlignment::Left);
    hostGrid.VerticalAlignment(VerticalAlignment::Center);
    hostGrid.Height(host->buttonHeight);

    host->buttonStates.clear();

    for (size_t i = 0; i < g_settings.folders.size(); i++) {
        const auto& entry = g_settings.folders[i];

        ColumnDefinition column;
        column.Width({1.0, GridUnitType::Auto});
        hostGrid.ColumnDefinitions().Append(column);

        Button button;
        button.Name(L"FolderHoverTrayButton_" + winrt::to_hstring((int)i));
        button.Width(host->buttonWidth);
        button.Height(host->buttonHeight);
        ApplyNativeButtonStyle(button);
        button.Content(MakeButtonContent(entry, host->buttonHeight));

        // No taskbar-button tooltip; the folder name is shown in the hover
        // grid.
        ToolTipService::SetToolTip(button, nullptr);

        Grid::SetColumn(button, (int)i);
        hostGrid.Children().Append(button);

        ButtonState state;
        state.button = button;
        state.folderIndex = (int)i;

        int folderIndex = (int)i;
        HWND taskbarWnd = host->hwnd;
        state.clickToken = button.Click(
            [folderIndex](winrt::Windows::Foundation::IInspectable const&,
                          RoutedEventArgs const&) {
                OnButtonClicked(folderIndex);
            });
        state.enterToken = button.PointerEntered(
            [folderIndex, taskbarWnd](
                winrt::Windows::Foundation::IInspectable const& sender,
                winrt::Windows::UI::Xaml::Input::
                    PointerRoutedEventArgs const&) {
                if (auto button = sender.try_as<Button>()) {
                    OnPointerEnteredButton(folderIndex, button, taskbarWnd);
                }
            });
        state.exitToken = button.PointerExited(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                OnPointerExitedButton();
            });

        host->buttonStates.push_back(std::move(state));
    }

    hostGrid.Width(DesiredHostWidth(host));
    return hostGrid;
}

void ClearButtonState(TaskbarHost* host) {
    if (!host) {
        return;
    }
    for (auto& state : host->buttonStates) {
        if (!state.button) {
            continue;
        }
        try {
            if (state.clickToken.value) {
                state.button.Click(state.clickToken);
            }
            if (state.enterToken.value) {
                state.button.PointerEntered(state.enterToken);
            }
            if (state.exitToken.value) {
                state.button.PointerExited(state.exitToken);
            }
            ToolTipService::SetToolTip(state.button, nullptr);
            state.button.Content(nullptr);
        } catch (...) {
        }
    }
    host->buttonStates.clear();
}

void RemoveHostGrid(TaskbarHost* host) {
    if (!host) {
        return;
    }

    if (host->trackedRootGrid && host->layoutUpdatedToken.value) {
        try {
            host->trackedRootGrid.LayoutUpdated(host->layoutUpdatedToken);
        } catch (...) {
        }
    }
    host->layoutUpdatedToken = {};

    RestoreAnchorMargin(host);
    ClearButtonState(host);

    if (host->trackedRootGrid) {
        try {
            auto children = host->trackedRootGrid.Children();
            for (int i = (int)children.Size() - 1; i >= 0; i--) {
                auto child = children.GetAt(i).try_as<FrameworkElement>();
                if (child && child.Name() == kHostGridName) {
                    children.RemoveAt(i);
                }
            }
        } catch (...) {
        }
    }

    host->hostGrid = nullptr;
    host->trackedRootGrid = nullptr;
    host->cachedRepeater = nullptr;
}

void RemoveAllHostGrids() {
    if (!g_taskbarHosts) {
        g_injectionLive = false;
        return;
    }
    for (auto& host : *g_taskbarHosts) {
        RemoveHostGrid(host.get());
    }
    g_taskbarHosts->clear();
    g_injectionLive = false;
}

// Runs on every layout pass, so every branch is guarded by a tolerance check to
// avoid feeding layout back into itself.
void OnRootGridLayoutUpdated(TaskbarHost* host) {
    if (g_unloading || !host || !host->hostGrid || !host->trackedRootGrid) {
        return;
    }

    try {
        FrameworkElement repeater = host->cachedRepeater;
        bool repeaterLooksDead = false;
        try {
            repeaterLooksDead = !repeater || repeater.ActualWidth() <= 1.0;
        } catch (...) {
            repeaterLooksDead = true;
        }
        if (repeaterLooksDead) {
            repeater = FindTaskbarRepeater(host->trackedRootGrid);
            host->cachedRepeater = repeater;
        }
        if (!repeater) {
            return;
        }

        ULONGLONG now = GetTickCount64();
        if (now - host->lastButtonSizeTick > 250) {
            host->lastButtonSizeTick = now;
            double previousWidth = host->buttonWidth;
            double previousHeight = host->buttonHeight;
            UpdateButtonSizeFromTaskbar(repeater, &host->buttonWidth,
                                        &host->buttonHeight);

            if (std::abs(previousWidth - host->buttonWidth) > 0.5 ||
                std::abs(previousHeight - host->buttonHeight) > 0.5) {
                for (auto& state : host->buttonStates) {
                    if (state.button) {
                        state.button.Width(host->buttonWidth);
                        state.button.Height(host->buttonHeight);
                    }
                }
                host->hostGrid.Height(host->buttonHeight);
                host->hostGrid.Width(DesiredHostWidth(host));
            }
        }

        bool anchorLooksDead =
            !host->anchor || host->anchor.ActualWidth() <= 1.0;
        if (anchorLooksDead || now - host->lastAnchorResolveTick > 250) {
            host->lastAnchorResolveTick = now;
            if (auto resolved =
                    ResolveAnchor(repeater, host->trackedRootGrid)) {
                AdoptAnchor(host, resolved);
            }
        }

        if (!host->anchor || !host->hasAnchorOriginalMargin) {
            return;
        }

        bool before = g_settings.position != L"afterApps";
        double desiredGap =
            DesiredHostWidth(host) + g_settings.gapBefore + g_settings.gapAfter;

        auto currentMargin = host->anchor.Margin();
        auto target = host->anchorOriginalMargin;
        if (before) {
            target.Left = host->anchorOriginalMargin.Left + desiredGap;
            if (std::abs(currentMargin.Left - target.Left) > 1.0) {
                host->anchor.Margin(target);
            }
        } else {
            target.Right = host->anchorOriginalMargin.Right + desiredGap;
            if (std::abs(currentMargin.Right - target.Right) > 1.0) {
                host->anchor.Margin(target);
            }
        }

        auto point = host->anchor.TransformToVisual(host->trackedRootGrid)
                         .TransformPoint({0, 0});
        double left;
        if (before) {
            left = point.X - desiredGap + g_settings.gapBefore;
        } else {
            left = point.X + host->anchor.ActualWidth() + g_settings.gapBefore;
        }

        auto hostMargin = host->hostGrid.Margin();
        if (std::abs(hostMargin.Left - left) > 1.0) {
            host->hostGrid.Margin({left, 0, 0, 0});
        }
    } catch (...) {
        // The anchor was recycled out from under us; restore its original
        // margin (if we still can) and re-resolve on the next pass.
        RestoreAnchorMargin(host);
        host->cachedRepeater = nullptr;
    }
}

bool InjectHostGridForTaskbar(HWND taskbarWnd) {
    if (!taskbarWnd || g_settings.folders.empty()) {
        return false;
    }

    auto xamlRoot = GetTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"No taskbar XamlRoot yet for %p", taskbarWnd);
        return false;
    }

    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) {
        return false;
    }

    auto rootGrid = FindTaskbarRootGrid(root);
    if (!rootGrid) {
        Wh_Log(L"Taskbar RootGrid not found yet for %p", taskbarWnd);
        return false;
    }

    auto repeater = FindTaskbarRepeater(rootGrid);
    if (!repeater) {
        Wh_Log(L"TaskbarFrameRepeater not found yet for %p", taskbarWnd);
        return false;
    }

    // Replace an existing host for this HWND if we are re-injecting.
    if (auto* existing = FindTaskbarHost(taskbarWnd)) {
        RemoveHostGrid(existing);
        auto& hosts = TaskbarHosts();
        hosts.erase(std::remove_if(hosts.begin(), hosts.end(),
                                   [taskbarWnd](
                                       const std::unique_ptr<TaskbarHost>& h) {
                                       return h && h->hwnd == taskbarWnd;
                                   }),
                    hosts.end());
    }

    auto host = std::make_unique<TaskbarHost>();
    host->hwnd = taskbarWnd;
    host->cachedRepeater = repeater;
    host->lastButtonSizeTick = GetTickCount64();
    UpdateButtonSizeFromTaskbar(repeater, &host->buttonWidth,
                                &host->buttonHeight);

    host->trackedRootGrid = rootGrid;
    host->hostGrid = BuildHostGrid(host.get());

    Grid::SetColumn(host->hostGrid, 0);
    Canvas::SetZIndex(host->hostGrid, 1000);
    host->hostGrid.Margin({0, 0, 0, 0});
    rootGrid.Children().Append(host->hostGrid);

    if (auto anchor = ResolveAnchor(repeater, rootGrid)) {
        AdoptAnchor(host.get(), anchor);
    }
    host->lastAnchorResolveTick = GetTickCount64();

    TaskbarHost* raw = host.get();
    host->layoutUpdatedToken = rootGrid.LayoutUpdated(
        [raw](winrt::Windows::Foundation::IInspectable const&,
              winrt::Windows::Foundation::IInspectable const&) {
            OnRootGridLayoutUpdated(raw);
        });

    Wh_Log(L"Injected %d folder button(s) on taskbar %p, size %.1fx%.1f",
           (int)host->buttonStates.size(), taskbarWnd, host->buttonWidth,
           host->buttonHeight);

    TaskbarHosts().push_back(std::move(host));
    return true;
}

bool InjectHostGridsOnTaskbarThread() {
    // Reached via RunFromWindowThread / WH_CALLWNDPROC — not a XAML delegate —
    // so WinRT failures must not unwind through user32's callback frame.
    try {
        // Register classes and create the menu owner on this (taskbar) thread so
        // DestroyWindow / UnregisterClass later stay same-thread-safe.
        EnsureMenuOwnerWindow();

        if (g_settings.folders.empty()) {
            Wh_Log(L"No folders configured, nothing to inject");
            RemoveAllHostGrids();
            return true;
        }

        RemoveAllHostGrids();

        struct EnumState {
            int found = 0;
            int injected = 0;
        } state;

        // Primary and secondary tray windows share the explorer taskbar UI thread
        // on Windows 11, matching other Windhawk taskbar mods.
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                auto* state = reinterpret_cast<EnumState*>(lParam);
                WCHAR className[32];
                if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                    return TRUE;
                }
                if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
                    _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                    return TRUE;
                }
                state->found++;
                if (InjectHostGridForTaskbar(hWnd)) {
                    state->injected++;
                } else {
                    Wh_Log(L"Inject failed for %s %p", className, hWnd);
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&state));

        if (g_taskbarHosts && !g_taskbarHosts->empty()) {
            g_taskbarWnd = (*g_taskbarHosts)[0]->hwnd;
            UpdatePopupDpi();
            int iconPixelSize = ScaleForPopup(g_settings.iconSize);
            for (size_t i = 0; i < g_settings.folders.size(); i++) {
                RequestScan(FolderPathForButton((int)i), iconPixelSize);
            }
        }

        g_injectionLive = state.found > 0 && state.injected == state.found;
        Wh_Log(L"Taskbar inject: %d/%d windows, %d hosts", state.injected,
               state.found,
               g_taskbarHosts ? (int)g_taskbarHosts->size() : 0);
        return g_injectionLive;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Injection failed: %s", e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Injection failed");
        return false;
    }
}

bool ApplyOnWindowThread(RunFromWindowThreadProc_t proc) {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        Wh_Log(L"ApplyOnWindowThread: no taskbar window found");
        return false;
    }
    if (!g_taskbarWnd) {
        g_taskbarWnd = taskbarWnd;
    }
    if (!RunFromWindowThread(taskbarWnd, proc, nullptr)) {
        Wh_Log(L"ApplyOnWindowThread: RunFromWindowThread failed (error %u)",
               GetLastError());
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Injection retries
//
// The taskbar is rebuilt on theme changes, Explorer restarts and DPI changes,
// so injection is retried with a backoff until it sticks.

DWORD WINAPI RetryThreadProc(void* param) {
    HANDLE stopEvent = static_cast<HANDLE>(param);
    // Capture kick once; StopRetryThread closes it only after this thread joins.
    HANDLE kickEvent = g_retryKickEvent;
    static const DWORD delays[] = {0,    500,  1000,  2000,
                                   4000, 8000, 15000, 30000};

    for (;;) {
        HANDLE waits[] = {stopEvent, kickEvent};
        DWORD wake = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (wake == WAIT_OBJECT_0 || wake == WAIT_FAILED) {
            return 0;  // stop (or wait failed)
        }
        if (g_unloading ||
            WaitForSingleObject(stopEvent, 0) != WAIT_TIMEOUT) {
            return 0;
        }

        g_injectionLive = false;

        for (size_t i = 0; i < ARRAYSIZE(delays) && !g_unloading; i++) {
            if (delays[i] &&
                WaitForSingleObject(stopEvent, delays[i]) != WAIT_TIMEOUT) {
                return 0;
            }
            if (g_unloading) {
                return 0;
            }

            ApplyOnWindowThread([](void*) { InjectHostGridsOnTaskbarThread(); });

            if (g_injectionLive || g_unloading) {
                break;
            }
        }
        // Success or exhaustion: wait for the next kick (e.g. secondary
        // TrayUI::StartTaskbar) instead of exiting — avoids the one-shot
        // exit race that could drop a re-injection request.
    }
}

void StopRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    HANDLE retryThread = g_retryThread;
    HANDLE retryStopEvent = g_retryStopEvent;
    HANDLE retryKickEvent = g_retryKickEvent;
    g_retryThread = nullptr;
    g_retryStopEvent = nullptr;
    g_retryKickEvent = nullptr;
    if (retryStopEvent) {
        SetEvent(retryStopEvent);
    }
    ReleaseSRWLockExclusive(&g_retryLock);

    if (retryThread) {
        // Pump sent messages while waiting so a worker blocked in SendMessage
        // to this UI thread cannot deadlock the join. Only settings/uninit
        // should call this; window procs use StartRetryThread (non-blocking).
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &retryThread, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(retryThread);
    }
    if (retryStopEvent) {
        CloseHandle(retryStopEvent);
    }
    if (retryKickEvent) {
        CloseHandle(retryKickEvent);
    }
}

// Non-blocking: safe from window procedures. Ensures the lifetime worker exists
// and kicks it. StopRetryThread still joins for Wh_ModSettingsChanged / uninit.
void StartRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    if (g_unloading) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    if (!g_retryStopEvent) {
        g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_retryStopEvent) {
            Wh_Log(L"CreateEventW for retry stop failed (%u)", GetLastError());
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }
    if (!g_retryKickEvent) {
        g_retryKickEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_retryKickEvent) {
            Wh_Log(L"CreateEventW for retry kick failed (%u)", GetLastError());
            CloseHandle(g_retryStopEvent);
            g_retryStopEvent = nullptr;
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }

    if (!g_retryThread) {
        HANDLE stopEvent = g_retryStopEvent;
        g_retryThread =
            CreateThread(nullptr, 0, RetryThreadProc, stopEvent, 0, nullptr);
        if (!g_retryThread) {
            Wh_Log(L"CreateThread for retry failed (%u)", GetLastError());
            CloseHandle(g_retryStopEvent);
            CloseHandle(g_retryKickEvent);
            g_retryStopEvent = nullptr;
            g_retryKickEvent = nullptr;
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }

    SetEvent(g_retryKickEvent);
    ReleaseSRWLockExclusive(&g_retryLock);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L"TrayUI::StartTaskbar");
    TrayUI_StartTaskbar_Original(pThis);
    if (!g_unloading) {
        StartRetryThread();
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();
    ResetFolderData();

    Gdiplus::GdiplusStartupInput startupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(
            L"Failed to hook taskbar.dll, the taskbar XamlRoot is unreachable");
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        return FALSE;
    }

    // Scan thread starts lazily on the first RequestScan (hover / inject).
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    // Menu owner / popup classes are created on the taskbar UI thread during
    // InjectHostGridsOnTaskbarThread (and EnsureLevelWindow), not here.
    StartRetryThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    StopRetryThread();
    // The scan thread reads g_settings, so it has to be parked before
    // reloading.
    StopScanThread();

    if (!ApplyOnWindowThread([](void*) {
            CloseChain();
            RemoveAllHostGrids();
            // Popup HWNDs keep the acrylic accent applied at create time;
            // destroy them so the next hover recreates with the current setting.
            for (HWND hWnd : g_levelWindows) {
                if (hWnd) {
                    DestroyWindow(hWnd);
                }
            }
            g_levelWindows.clear();
        })) {
        Wh_Log(L"SettingsChanged: failed to tear down UI on taskbar thread");
    }

    LoadSettings();
    ResetFolderData();

    // Scan thread restarts lazily on the next RequestScan.
    StartRetryThread();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;
    StopRetryThread();
    StopScanThread();

    // DestroyWindow / XAML teardown must run on the creating UI thread. Prefer
    // a mod-owned window (level popup or menu owner) when the taskbar HWND is
    // already gone — never DestroyWindow from this Windhawk callback thread.
    auto teardownOnUiThread = [](void*) {
        CloseChain();
        RemoveAllHostGrids();

        for (HWND hWnd : g_levelWindows) {
            if (hWnd) {
                DestroyWindow(hWnd);
            }
        }
        g_levelWindows.clear();

        if (g_menuOwnerWnd) {
            DestroyWindow(g_menuOwnerWnd);
            g_menuOwnerWnd = nullptr;
        }

        g_taskbarHosts.reset();
    };

    HWND threadWnd = nullptr;
    for (HWND h : g_levelWindows) {
        if (h) {
            threadWnd = h;
            break;
        }
    }
    if (!threadWnd) {
        threadWnd = g_menuOwnerWnd;
    }
    if (!threadWnd) {
        threadWnd = FindCurrentProcessTaskbarWnd();
    }

    if (threadWnd) {
        if (!RunFromWindowThread(threadWnd, teardownOnUiThread, nullptr)) {
            Wh_Log(L"Uninit: RunFromWindowThread failed (error %u); windows "
                   L"may leak and UnregisterClass may fail",
                   GetLastError());
        }
    } else {
        // No mod-owned or taskbar window — nothing left that can dangle into
        // unmapped code; retain no_destroy host state rather than tearing it
        // down from the wrong thread.
        Wh_Log(L"No mod-owned or taskbar window at uninit; retaining host state");
    }

    g_levels.reset();
    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_folderCache.reset();
    }

    // The window procedures live in this DLL, so the classes must not outlive
    // it.
    HINSTANCE instance = GetModuleHandleW(nullptr);
    UnregisterClassW(kPopupClassName, instance);
    UnregisterClassW(kMenuOwnerClassName, instance);

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
