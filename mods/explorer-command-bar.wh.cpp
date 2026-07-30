// ==WindhawkMod==
// @id              explorer-command-bar
// @name            Explorer Command Bar
// @description     Add your own buttons and dropdown menus to the Windows 11 File Explorer command bar, and hide the built-in ones
// @version         1.0.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @homepage        https://dan13.me/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Command Bar

Add your own buttons and dropdown menus to the **Windows 11 File Explorer
command bar** (the toolbar with New / Sort / View), and hide the built-in
buttons, separators and spacing you don't want.

![Explorer Command Bar demo](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/exporer-command-bar/screenshots/main.gif)

Designed for Windows 11 24H2 / 25H2 with the WinAppSDK (WinUI 3) File
Explorer.

## Features

- **Custom buttons** - add as many toolbar buttons as you like, each running a
  command of your choice.
- **Dropdown menus & submenus** - a button can open a menu, and menu entries
  can themselves be submenus (three levels deep).
- **Path & selection placeholders** - `%path%` (active tab folder) and `%sel%`
  (selected file/folder) are substituted into the command parameters.
- **Flexible icons** - a Segoe Fluent Icons glyph, an `.exe` / `.dll` / `.ico`
  file, a Store-app icon (`shell:AppsFolder\…`), the command executable's own
  icon, or no icon at all.
- **Hide built-in elements** - individually hide New, Cut, Copy, Paste, Rename,
  Share, Delete, Sort, View, the group separators, the "See more" (…) overflow
  menu, the contextual commands (Set as background, Rotate left, Rotate right,
  Extract all) and the Details pane toggle.
- **Custom item spacing** - set the exact spacing between the command bar
  buttons.
- **Open menus on hover** - optionally open dropdowns on hover, with a
  configurable delay.
- **Rock solid** - buttons are re-applied automatically across tab switches,
  navigation, new tabs and new windows, and everything is cleanly restored when
  the mod is disabled.

## Screenshots

Hiding built‑in buttons and separators:

![Hide buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/exporer-command-bar/screenshots/hide-buttons.gif)

You may hide even all options, and use only your custom ones:

![Hide All buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/exporer-command-bar/screenshots/hide-all-buttons.jpg)

## Command parameters

The following placeholders can be used in an item's parameters:

* `%path%` - the folder path of the currently active tab.
* `%sel%` - the full path of the selected file or folder in the active tab (the
  first one, if several are selected).

Wrap placeholders in quotes so paths with spaces work, e.g. `-d "%path%"`. If a
used placeholder has no value (nothing selected, or a non-filesystem location
like *This PC*), the command is launched without any parameters. Commands run
with the active tab's folder as their working directory.

## Icons

The **Icon glyph or icon path** field accepts several forms:

* **A glyph** - a hex code point of a
  [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font)
  glyph, e.g. `E756`.
* **A file path** - an `.exe`, `.dll` or `.ico` file to take the icon from,
  optionally with an icon index, e.g. `C:\Windows\System32\shell32.dll,3`.
* **A Store app** - `shell:AppsFolder\<AppUserModelID>` to use a modern Store
  app's icon (useful for apps whose `.exe` stub carries a legacy icon, such as
  Notepad and Calculator).
* **Empty** - the icon is extracted from the command's executable (app
  execution aliases such as `wt.exe` are resolved to their real target).
* **Hide icon** - enable the toggle to show no icon at all.

## Default configuration

Out of the box the mod adds only apps which are part of a stock Windows 11
install:

* **Open in Terminal** - `wt.exe -d "%path%"`
* **Open in Notepad** - `notepad.exe "%sel%"`
* **Additional** ▾ (dropdown)
  * Open Paint - `mspaint.exe`
  * Open Calculator - `calc.exe`
  * **System** ▸ - Task Manager (`taskmgr.exe`), Control Panel (`control.exe`)

All of the built-in buttons stay visible by default. Everything is configurable
in the mod settings.

## Example configurations

A few items worth adding if you have the corresponding tools installed. Set
*Command* and *Parameters* as shown; leave *Icon glyph or icon path* empty to
use the executable's own icon.

* **Open in VS Code** - command `code.exe`, parameters `"%path%"`.
* **Open in your editor of choice** - e.g. command
  `%LOCALAPPDATA%\Programs\<editor>\<editor>.exe`, parameters `"%path%"`.
* **A dropdown of project commands** (type *Dropdown menu*), each entry with
  command `cmd.exe` and parameters `/k npm install`, `/k npm run dev`,
  `/k npm run build`, and so on. `cmd.exe /k` keeps the console open so you can
  see the output.
* **A dropdown of CLI tools** - command `cmd.exe` with parameters `/k claude`,
  `/k codex`, `/k gh pr list`, ...
* **Open a terminal as administrator** - command
  `powershell.exe`, parameters
  `-Command "Start-Process wt.exe -ArgumentList '-d \"%path%\"' -Verb RunAs"`.

Enable *Hide icon* for the menu entries of such commands: an `.exe` like
`cmd.exe` contributes little as an icon next to a text label.

## How it works

The mod hooks a couple of functions of File Explorer's own WinUI 3 code
(`FileExplorerExtensions.dll`) which run when the command bar is built, and
finds the command bars from there by walking the XAML tree. The configured
buttons are then inserted and the visibility / spacing settings are applied. The
mod also listens for the command bar being rebuilt so the buttons stay in place
across navigation, new tabs and new windows, and it restores the original state
of any element it touches when disabled.

Notably, the mod does **not** use XAML Diagnostics
(`InitializeXamlDiagnosticsEx`), since only one XAML diagnostics consumer can be
active in a process at a time. That makes it compatible with mods and tools
which do use it, such as **Windows 11 File Explorer Styler**, ExplorerBlurMica
and TranslucentTB.

File Explorer windows which are already open when the mod is enabled are
handled too, but if the buttons don't show up in one of them right away, opening
a new tab or navigating to another folder makes them appear.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - - type: button
      $name: Main Item Type
      $options:
      - button: Single button
      - menu: Dropdown menu
    - name: Open in Terminal
      $name: Name
      $description: Shown as the button tooltip.
    - command: wt.exe
      $name: Command
      $description: >-
        The executable to run. For dropdown menus it's only used as an icon
        source.
    - parameters: -d "%path%"
      $name: Parameters
      $description: >-
        Command line parameters. %path% is replaced with the current folder
        path, %sel% with the path of the selected file/folder. If a used
        placeholder has no value (e.g. no selection, or a location like This
        PC), the command is launched without parameters.
    - iconGlyph: ""
      $name: Icon glyph or icon path
      $description: >-
        Hex code point of a Segoe Fluent Icons glyph, e.g. E756
        (CommandPrompt) or EC7A. The list of glyphs:
        https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font
        Alternatively, a path to an .exe, .dll or
        .ico file to use its icon, optionally with an icon index, e.g.
        C:\Windows\System32\shell32.dll,3. Leave empty to use the icon of the
        command's executable.
    - hideIcon: false
      $name: Hide icon
      $description: Don't show an icon for this item.
    - separatorAfter: false
      $name: Vertical separator after
      $description: Show a vertical separator line after this button.
    - subItems:
      - - type: button
          $name: Menu Items Item Type
          $options:
          - button: Menu item
          - menu: Submenu
        - name: ""
          $name: Name
          $description: The menu item text.
        - command: ""
          $name: Command
          $description: >-
            The executable to run. For submenus it's only used as an icon
            source.
        - parameters: ""
          $name: Parameters
          $description: >-
            Command line parameters. %path% is replaced with the current
            folder path, %sel% with the path of the selected file/folder.
        - iconGlyph: ""
          $name: Icon glyph or icon path
          $description: >-
            Hex code point of a Segoe Fluent Icons glyph
            (https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font),
            or a path to an .exe, .dll or .ico file, optionally with an icon
            index. Leave empty to use the icon of the command's executable.
        - hideIcon: false
          $name: Hide icon
          $description: Don't show an icon for this menu item.
        - separatorAfter: false
          $name: Separator after
          $description: Show a separator line below this menu item.
        - subItems:
          - - name: ""
              $name: Name
              $description: The menu item text.
            - command: ""
              $name: Command
            - parameters: ""
              $name: Parameters
              $description: >-
                Command line parameters. %path% is replaced with the current
                folder path, %sel% with the path of the selected file/folder.
            - iconGlyph: ""
              $name: Icon glyph or icon path
              $description: >-
                Hex code point of a Segoe Fluent Icons glyph
                (https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font),
                or a path to an .exe, .dll or .ico file, optionally with an
                icon index. Leave empty to use the icon of the command's
                executable.
            - hideIcon: false
              $name: Hide icon
              $description: Don't show an icon for this menu item.
            - separatorAfter: false
              $name: Separator after
              $description: Show a separator line below this menu item.
          $name: Submenu items
          $description: The entries of the submenu, for the submenu type.
      $name: Menu items
      $description: The entries of the dropdown menu, for the dropdown type.
  - - type: button
    - name: Open in Notepad
    - command: notepad.exe
    - parameters: '"%sel%"'
    - iconGlyph: 'shell:AppsFolder\Microsoft.WindowsNotepad_8wekyb3d8bbwe!App'
    - separatorAfter: false
  - - type: menu
    - name: Additional
    - command: ""
    - parameters: ""
    - iconGlyph: E81E
    - separatorAfter: false
    - subItems:
      - - type: button
        - name: Open Paint
        - command: mspaint.exe
        - parameters: ""
        - iconGlyph: ""
        - separatorAfter: false
      - - type: button
        - name: Open Calculator
        - command: calc.exe
        - parameters: ""
        - iconGlyph: 'shell:AppsFolder\Microsoft.WindowsCalculator_8wekyb3d8bbwe!App'
        - separatorAfter: true
      - - type: menu
        - name: System
        - command: ""
        - parameters: ""
        - iconGlyph: E713
        - separatorAfter: false
        - subItems:
          - - name: Task Manager
            - command: taskmgr.exe
            - parameters: ""
            - iconGlyph: ""
            - hideIcon: false
            - separatorAfter: false
          - - name: Control Panel
            - command: control.exe
            - parameters: ""
            - iconGlyph: ""
            - hideIcon: false
            - separatorAfter: false
  $name: Toolbar items
  $description: >-
    Each toolbar item is either a single button or a dropdown menu. Use the
    plus button to add another one.
- hideDefaultButtons:
  - new: false
    $name: New
  - separatorAfterNew: false
    $name: Vertical separator after New
  - cut: false
    $name: Cut
  - copy: false
    $name: Copy
  - paste: false
    $name: Paste
  - rename: false
    $name: Rename
  - share: false
    $name: Share
  - delete: false
    $name: Delete
  - separatorAfterDelete: false
    $name: Vertical separator after Delete
  - sort: false
    $name: Sort
  - view: false
    $name: View
  - separatorAfterView: false
    $name: Vertical separator after View
  - moreOptions: false
    $name: See more (the three dots menu)
  - setAsBackground: false
    $name: Set as background
    $description: Shown when an image file is selected.
  - rotateLeft: false
    $name: Rotate left
    $description: Shown when an image file is selected.
  - rotateRight: false
    $name: Rotate right
    $description: Shown when an image file is selected.
  - extractAll: false
    $name: Extract all
    $description: Shown when a zip or other archive file is selected.
  - details: false
    $name: Details pane toggle
  $name: Hide default toolbar buttons
  $description: >-
    Hide the built-in buttons of the File Explorer command bar. The vertical
    separator of the contextual commands (Set as background, Rotate left,
    Rotate right, Extract all) is hidden along with them, once none of them is
    left to show.
- openMenuOnHover: false
  $name: Open menus on hover
  $description: >-
    Open dropdown menus by hovering over the button instead of clicking it.
- menuHoverDelay: 400
  $name: Hover delay (milliseconds)
  $description: >-
    How long the cursor has to stay over the button before the menu opens.
    Only used when "Open menus on hover" is enabled.
- itemSpacing: -1
  $name: Item spacing (pixels at 100% scaling)
  $description: >-
    Horizontal spacing between the command bar buttons. -1 leaves the default
    spacing unchanged; 0 places the buttons right next to each other; 5 means 5
    pixels between buttons, and so on. The value is in effective pixels, so at
    150% display scaling it results in 1.5 times as many physical pixels. Note
    that any value other than -1 also removes the built-in buttons' minimum
    width, which shrinks their click area down to the icon.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

std::atomic<bool> g_unloading;

static constexpr CLSID kCLSID_ShellWindows = {
    0x9ba05972,
    0xf6a8,
    0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};

static constexpr GUID kSID_STopLevelBrowser = {
    0x4c96be40,
    0x915c,
    0x11cf,
    {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

struct ActionItem {
    std::wstring name;
    std::wstring command;
    std::wstring parameters;
    std::wstring icon;  // Raw icon setting: a glyph code point, a path to an
                        // icon file, or empty to use the command's
                        // executable icon.
    bool hideIcon = false;
    bool isMenu = false;
    bool separatorAfter = false;
    std::vector<ActionItem> subItems;
};

// The built-in command bar buttons. They're identified at runtime by the SVG
// file name of their icon, which is stable across languages.
struct DefaultButtonDef {
    PCWSTR settingKey;
    PCWSTR svgFileName;
};

constexpr DefaultButtonDef kDefaultButtons[] = {
    {L"new", L"windows.newitem.svg"},
    {L"cut", L"windows.cut.svg"},
    {L"copy", L"windows.copy.svg"},
    {L"paste", L"windows.paste.svg"},
    {L"rename", L"windows.rename.svg"},
    {L"share", L"windows.modernshare.svg"},
    {L"delete", L"windows.ribbondelete.svg"},
    {L"sort", L"sortby.svg"},
    {L"view", L"view.svg"},
    // Contextual buttons, shown only for the matching selection: an image
    // file for the first three, an archive for Extract all.
    {L"setAsBackground", L"windows.setdesktopwallpaper.svg"},
    {L"rotateLeft", L"windows.rotate270.svg"},
    {L"rotateRight", L"windows.rotate90.svg"},
    {L"extractAll", L"windows.compressedfile.extract.svg"},
};

constexpr int kDefaultButtonCount = ARRAYSIZE(kDefaultButtons);
constexpr int kNewButtonIndex = 0;
constexpr int kDeleteButtonIndex = 6;
constexpr int kSortButtonIndex = 7;
constexpr int kViewButtonIndex = 8;

struct {
    std::mutex mutex;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideSeparatorAfterButton[kDefaultButtonCount] = {};
    bool hideMoreButton = false;
    bool hideDetailsButton = false;
    int itemSpacing = -1;
    std::vector<ActionItem> items;
} g_settings;

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <Unknwn.h>

// Conflicts with a winrt method of the same name.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

#pragma endregion  // winrt_hpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

constexpr PCWSTR kButtonNamePrefix = L"WindhawkActionButton";

struct CommandBarEntry {
    DWORD threadId;
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token vectorChangedToken{};
};

std::mutex g_entriesMutex;
std::vector<CommandBarEntry> g_entries;

// The Explorer elements we touch, each with the original state captured the
// first time we touched it, so it can be restored exactly instead of forcing a
// guessed default (which would reveal elements Explorer keeps collapsed), and
// with the visibility watcher registered on it (see below).
//
// The element is held as a weak reference and entries are matched by resolving
// it, never by its address: XAML objects die with their window or tab, and the
// heap happily hands the same address to an unrelated element of the next one.
struct ManagedElement {
    DWORD threadId;
    winrt::weak_ref<mux::UIElement> element;

    bool hasOriginalVisibility = false;
    mux::Visibility originalVisibility = mux::Visibility::Visible;

    bool hasOriginalSpacing = false;
    mux::Thickness originalMargin{};
    double originalMinWidth = 0;

    // Command bars only.
    bool hasOriginalOverflow = false;
    muxc::CommandBarOverflowButtonVisibility originalOverflow =
        muxc::CommandBarOverflowButtonVisibility::Auto;

    bool watched = false;
    int64_t visibilityToken = 0;
};

std::mutex g_managedElementsMutex;
std::vector<ManagedElement> g_managedElements;

// The lookups below must run on the element's own UI thread, and the caller
// must hold g_managedElementsMutex. A weak reference to a live non-agile XAML
// element can only be resolved from its own thread, which is also why entries
// of other threads are skipped rather than treated as dead.

ManagedElement* FindManagedElement(mux::UIElement const& element) {
    DWORD threadId = GetCurrentThreadId();

    for (auto& entry : g_managedElements) {
        if (entry.threadId == threadId && entry.element.get() == element) {
            return &entry;
        }
    }

    return nullptr;
}

ManagedElement& GetManagedElement(mux::UIElement const& element) {
    DWORD threadId = GetCurrentThreadId();

    // Drop the entries of this thread whose element is gone, so the list
    // doesn't grow for the whole Explorer session.
    for (auto it = g_managedElements.begin();
         it != g_managedElements.end();) {
        if (it->threadId == threadId && !it->element.get()) {
            it = g_managedElements.erase(it);
        } else {
            ++it;
        }
    }

    if (auto* entry = FindManagedElement(element)) {
        return *entry;
    }

    g_managedElements.push_back({threadId, winrt::make_weak(element)});
    return g_managedElements.back();
}

////////////////////////////////////////////////////////////////////////////////
// Getting the current folder path and launching commands.

struct ExplorerContext {
    std::wstring folderPath;
    std::wstring selectedPath;
};

ExplorerContext GetExplorerContext(HWND hExplorerWnd) {
    ExplorerContext result;

    winrt::com_ptr<IShellWindows> shellWindows;
    HRESULT hr = CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(shellWindows.put()));
    if (FAILED(hr) || !shellWindows) {
        Wh_Log(L"CoCreateInstance(ShellWindows) failed: %08X", hr);
        return result;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    // The active tab's ShellTabWindowClass window is the first one in the
    // Z-order of the CabinetWClass window's children.
    HWND hActiveTabWnd =
        hExplorerWnd ? FindWindowExW(hExplorerWnd, nullptr,
                                     L"ShellTabWindowClass", nullptr)
                     : nullptr;

    for (long i = 0; i < count && result.folderPath.empty(); i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch) {
            continue;
        }

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser) {
            continue;
        }

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw))) {
            continue;
        }

        if (hExplorerWnd && (HWND)hWndRaw != hExplorerWnd) {
            continue;
        }

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider) {
            continue;
        }

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser) {
            continue;
        }

        // With tabs, every tab is a separate ShellWindows entry with the same
        // top-level window. Skip the entries of inactive tabs.
        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd) {
                if (hTabWnd != hActiveTabWnd) {
                    continue;
                }
            } else if (!IsWindowVisible(hTabWnd)) {
                continue;
            }
        }

        winrt::com_ptr<IShellView> shellView;
        if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())) ||
            !shellView) {
            continue;
        }

        auto folderView = shellView.try_as<IFolderView>();
        if (!folderView) {
            continue;
        }

        winrt::com_ptr<IPersistFolder2> persistFolder;
        if (FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) ||
            !persistFolder) {
            continue;
        }

        LPITEMIDLIST pidl = nullptr;
        if (FAILED(persistFolder->GetCurFolder(&pidl)) || !pidl) {
            continue;
        }

        WCHAR path[MAX_PATH];
        if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                  GPFIDL_DEFAULT)) {
            result.folderPath = path;
        }

        CoTaskMemFree(pidl);

        // The first selected file or folder, if any.
        winrt::com_ptr<IShellItemArray> selection;
        if (SUCCEEDED(shellView->GetItemObject(
                SVGIO_SELECTION, IID_PPV_ARGS(selection.put()))) &&
            selection) {
            winrt::com_ptr<IShellItem> shellItem;
            if (SUCCEEDED(selection->GetItemAt(0, shellItem.put())) &&
                shellItem) {
                PWSTR selectedPath = nullptr;
                if (SUCCEEDED(shellItem->GetDisplayName(
                        SIGDN_FILESYSPATH, &selectedPath)) &&
                    selectedPath) {
                    result.selectedPath = selectedPath;
                    CoTaskMemFree(selectedPath);
                }
            }
        }
    }

    return result;
}

// Replaces all occurrences of the placeholder. Returns false if the
// placeholder is used but has no value.
bool ReplacePlaceholder(std::wstring& parameters,
                        std::wstring_view placeholder,
                        std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos) {
        return true;
    }

    if (value.empty()) {
        return false;
    }

    while (pos != std::wstring::npos) {
        std::wstring replacement = value;
        // A trailing backslash would escape a closing quote right after the
        // placeholder (e.g. for C:\).
        if (replacement.back() == L'\\' &&
            pos + placeholder.size() < parameters.size() &&
            parameters[pos + placeholder.size()] == L'"') {
            replacement += L'\\';
        }

        parameters.replace(pos, placeholder.size(), replacement);
        pos = parameters.find(placeholder, pos + replacement.size());
    }

    return true;
}

std::wstring BuildParameters(std::wstring parameters,
                             ExplorerContext const& context) {
    // If a used placeholder has no value (no filesystem path, no selection),
    // launch without parameters.
    if (!ReplacePlaceholder(parameters, L"%path%", context.folderPath) ||
        !ReplacePlaceholder(parameters, L"%sel%", context.selectedPath)) {
        return std::wstring();
    }

    return parameters;
}

// The threads which are busy resolving the Explorer context and launching a
// command. Their code lives in this DLL, so the mod can't be unloaded while
// one of them is still running - see
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

void TrackLaunchThread(HANDLE thread) {
    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);

    // Reap the ones which already finished.
    for (auto it = g_launchThreads.begin(); it != g_launchThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_launchThreads.erase(it);
        } else {
            ++it;
        }
    }

    g_launchThreads.push_back(thread);
}

void WaitForLaunchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        threads.swap(g_launchThreads);
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void LaunchItemForWindow(HWND hExplorerWnd, ActionItem const& item) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd);
    Wh_Log(L"Launching %s for window %08X, path: %s, selection: %s",
           item.command.c_str(), (DWORD)(ULONG_PTR)hExplorerWnd,
           context.folderPath.c_str(), context.selectedPath.c_str());

    std::wstring parameters = BuildParameters(item.parameters, context);

    HINSTANCE hInst = ShellExecuteW(
        nullptr, nullptr, item.command.c_str(),
        parameters.empty() ? nullptr : parameters.c_str(),
        context.folderPath.empty() ? nullptr : context.folderPath.c_str(),
        SW_SHOWNORMAL);
    if ((INT_PTR)hInst <= 32) {
        Wh_Log(L"ShellExecuteW failed: %d", (int)(INT_PTR)hInst);
    }
}

void OnActionInvoked(mux::FrameworkElement const& elementForWindow,
                     ActionItem const& item) {
    if (item.command.empty() || g_unloading) {
        return;
    }

    HWND hWnd = nullptr;

    try {
        if (auto xamlRoot = elementForWindow.XamlRoot()) {
            if (auto environment = xamlRoot.ContentIslandEnvironment()) {
                hWnd = (HWND)(uintptr_t)environment.AppWindowId().Value;
            }
        }
    } catch (...) {
        Wh_Log(L"Failed to get window from XamlRoot: %08X",
               winrt::to_hresult().value);
    }

    if (!hWnd) {
        hWnd = GetActiveWindow();
    }

    if (!hWnd) {
        hWnd = GetForegroundWindow();
    }

    if (hWnd) {
        hWnd = GetAncestor(hWnd, GA_ROOT);
    }

    struct LaunchParams {
        HWND hExplorerWnd;
        ActionItem item;
    };

    auto launchParams = std::make_unique<LaunchParams>(hWnd, item);

    // Do the shell COM work off the UI thread, so a slow or unresponsive
    // shell can't block the click handler. Note that IShellBrowser and
    // friends are owned by the Explorer UI thread, so the calls marshal back
    // to it; only the waiting happens elsewhere.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<LaunchParams> launchParams(
                reinterpret_cast<LaunchParams*>(lpParam));

            HRESULT hrInit = CoInitializeEx(
                nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            LaunchItemForWindow(launchParams->hExplorerWnd,
                                launchParams->item);
            if (SUCCEEDED(hrInit)) {
                CoUninitialize();
            }
            return 0;
        },
        launchParams.get(), 0, nullptr);
    if (thread) {
        launchParams.release();  // Owned by the thread now.
        // The handle is closed once the thread finished, either here on a
        // later launch or in Wh_ModUninit, which waits for it.
        TrackLaunchThread(thread);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Icons.

std::wstring ExpandEnvVars(std::wstring const& str) {
    WCHAR buffer[MAX_PATH];
    DWORD length =
        ExpandEnvironmentStringsW(str.c_str(), buffer, ARRAYSIZE(buffer));
    if (length == 0 || length > ARRAYSIZE(buffer)) {
        return str;
    }

    return buffer;
}

// Resolves a bare executable name to a full path the way ShellExecute does:
// through the search path, then through the App Paths registry keys.
std::wstring ResolveCommandPath(std::wstring const& command) {
    std::wstring expanded = ExpandEnvVars(command);

    if (expanded.find(L'\\') != std::wstring::npos) {
        return expanded;
    }

    WCHAR resolved[MAX_PATH];
    if (SearchPathW(nullptr, expanded.c_str(), L".exe", ARRAYSIZE(resolved),
                    resolved, nullptr)) {
        return resolved;
    }

    std::wstring keyPath =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" +
        expanded;
    std::wstring lower = expanded;
    for (auto& c : lower) {
        c = towlower(c);
    }
    if (!lower.ends_with(L".exe")) {
        keyPath += L".exe";
    }

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, buffer, &size) == ERROR_SUCCESS &&
            buffer[0]) {
            std::wstring appPath = ExpandEnvVars(buffer);

            // The value is sometimes quoted.
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }

            return appPath;
        }
    }

    return expanded;
}

#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

// App execution aliases (e.g. wt.exe) are zero-byte reparse points without
// icons. Resolve them to the target executable.
std::wstring ResolveAppExecutionAlias(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        return path;
    }

    HANDLE hFile = CreateFileW(
        path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return path;
    }

    struct AppExecLinkReparseBuffer {
        ULONG reparseTag;
        USHORT reparseDataLength;
        USHORT reserved;
        ULONG version;
        // Four NUL-separated strings follow: package id, entry point,
        // executable path, application type.
        WCHAR stringList[1];
    };

    alignas(8) BYTE buffer[16 * 1024];
    DWORD bytesReturned = 0;
    BOOL succeeded =
        DeviceIoControl(hFile, FSCTL_GET_REPARSE_POINT, nullptr, 0, buffer,
                        sizeof(buffer), &bytesReturned, nullptr);
    CloseHandle(hFile);

    auto* reparse = reinterpret_cast<AppExecLinkReparseBuffer*>(buffer);
    if (!succeeded || bytesReturned < sizeof(AppExecLinkReparseBuffer) ||
        reparse->reparseTag != IO_REPARSE_TAG_APPEXECLINK) {
        return path;
    }

    const WCHAR* p = reparse->stringList;
    const WCHAR* end =
        reinterpret_cast<const WCHAR*>(buffer + bytesReturned);
    for (int i = 0; i < 3 && p < end; i++) {
        size_t length = wcsnlen(p, end - p);
        if (p + length >= end) {
            break;
        }

        if (i == 2) {
            // The executable path.
            return std::wstring(p, length);
        }

        p += length + 1;
    }

    return path;
}

HICON ExtractCommandIcon(std::wstring const& command) {
    std::wstring path = ResolveAppExecutionAlias(ResolveCommandPath(command));

    HICON hIcon = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hIcon, nullptr, 1) && hIcon) {
        return hIcon;
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(path.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }

    Wh_Log(L"Couldn't get an icon for %s (%s)", command.c_str(),
           path.c_str());
    return nullptr;
}

// A decoded icon: premultiplied BGRA pixels, top-down. Plain pixel data with
// no thread affinity, which is what lets it be cached globally and turned into
// a WriteableBitmap per XamlRoot.
struct DecodedIcon {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;

    bool empty() const { return pixels.empty(); }
};

// Reads a bitmap as 32-bit top-down pixels, whatever its own format is.
bool ReadBitmapPixels(HBITMAP hBitmap, DecodedIcon* decoded) {
    BITMAP bm{};
    if (!GetObject(hBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return false;
    }

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Top-down.
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> pixels((size_t)width * height * 4);

    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) {
        return false;
    }

    bool succeeded = GetDIBits(hdc, hBitmap, 0, height, pixels.data(), &bmi,
                               DIB_RGB_COLORS) != 0;
    DeleteDC(hdc);

    if (!succeeded) {
        return false;
    }

    decoded->width = width;
    decoded->height = height;
    decoded->pixels = std::move(pixels);
    return true;
}

// True if every pixel is fully transparent, which is how a bitmap without an
// alpha channel comes back.
bool HasNoAlphaChannel(std::vector<uint8_t> const& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p]) {
            return false;
        }
    }

    return true;
}

void MakeOpaque(std::vector<uint8_t>& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        pixels[p] = 255;
    }
}

// 1-bpp icons have no color bitmap at all: their mask holds an AND mask
// stacked on top of an XOR (monochrome color) mask.
bool DecodeMonochromeIcon(HBITMAP hbmMask, DecodedIcon* decoded) {
    DecodedIcon mask;
    if (!ReadBitmapPixels(hbmMask, &mask) || mask.height % 2 != 0) {
        return false;
    }

    int width = mask.width;
    int height = mask.height / 2;

    DecodedIcon result;
    result.width = width;
    result.height = height;
    result.pixels.assign((size_t)width * height * 4, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t andIndex = ((size_t)y * width + x) * 4;
            size_t xorIndex = ((size_t)(y + height) * width + x) * 4;

            // A set AND mask bit means "leave the background alone", i.e.
            // transparent - the pixel stays zeroed.
            if (mask.pixels[andIndex]) {
                continue;
            }

            uint8_t value = mask.pixels[xorIndex] ? 255 : 0;
            result.pixels[andIndex + 0] = value;
            result.pixels[andIndex + 1] = value;
            result.pixels[andIndex + 2] = value;
            result.pixels[andIndex + 3] = 255;
        }
    }

    *decoded = std::move(result);
    return true;
}

bool DecodeIcon(HICON hIcon, DecodedIcon* decoded) {
    ICONINFO iconInfo{};
    if (!GetIconInfo(hIcon, &iconInfo)) {
        return false;
    }

    bool succeeded = false;

    if (iconInfo.hbmColor) {
        if (ReadBitmapPixels(iconInfo.hbmColor, decoded)) {
            if (HasNoAlphaChannel(decoded->pixels)) {
                MakeOpaque(decoded->pixels);
            } else {
                // The XAML bitmap expects premultiplied alpha.
                for (size_t p = 0; p < decoded->pixels.size(); p += 4) {
                    uint8_t alpha = decoded->pixels[p + 3];
                    if (alpha != 255) {
                        decoded->pixels[p] = decoded->pixels[p] * alpha / 255;
                        decoded->pixels[p + 1] =
                            decoded->pixels[p + 1] * alpha / 255;
                        decoded->pixels[p + 2] =
                            decoded->pixels[p + 2] * alpha / 255;
                    }
                }
            }

            succeeded = true;
        }
    } else if (iconInfo.hbmMask) {
        succeeded = DecodeMonochromeIcon(iconInfo.hbmMask, decoded);
    }

    if (iconInfo.hbmColor) {
        DeleteObject(iconInfo.hbmColor);
    }
    if (iconInfo.hbmMask) {
        DeleteObject(iconInfo.hbmMask);
    }

    return succeeded;
}

muxm::ImageSource CreateImageSource(DecodedIcon const& decoded) try {
    if (decoded.empty()) {
        return nullptr;
    }

    muxm::Imaging::WriteableBitmap bitmap(decoded.width, decoded.height);
    memcpy(bitmap.PixelBuffer().data(), decoded.pixels.data(),
           decoded.pixels.size());
    bitmap.Invalidate();
    return bitmap;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
    return nullptr;
}

std::wstring ParseGlyphSetting(PCWSTR glyphSetting) {
    if (!glyphSetting[0]) {
        return std::wstring();
    }

    if (!glyphSetting[1]) {
        // A literal glyph character.
        return std::wstring(1, glyphSetting[0]);
    }

    unsigned long parsed = wcstoul(glyphSetting, nullptr, 16);
    if (parsed > 0 && parsed <= 0xFFFF) {
        return std::wstring(1, (WCHAR)parsed);
    }

    return std::wstring();
}

bool LooksLikeIconPath(std::wstring const& iconSetting) {
    if (iconSetting.find(L'\\') != std::wstring::npos ||
        iconSetting.find(L'/') != std::wstring::npos ||
        iconSetting.find(L':') != std::wstring::npos) {
        return true;
    }

    std::wstring lower = iconSetting;
    for (auto& c : lower) {
        c = towlower(c);
    }

    // Also allow a bare file name, possibly with an icon index.
    size_t comma = lower.rfind(L',');
    if (comma != std::wstring::npos) {
        lower.resize(comma);
    }

    return lower.ends_with(L".exe") || lower.ends_with(L".dll") ||
           lower.ends_with(L".ico");
}

HICON LoadIconFromPath(std::wstring const& iconPath) {
    std::wstring expanded = ExpandEnvVars(iconPath);

    // Support an icon index suffix, e.g. shell32.dll,3.
    int iconIndex = 0;
    size_t comma = expanded.rfind(L',');
    if (comma != std::wstring::npos && comma + 1 < expanded.size()) {
        bool isNumber = true;
        for (size_t i = comma + 1; i < expanded.size(); i++) {
            WCHAR c = expanded[i];
            bool isDigit = c >= L'0' && c <= L'9';
            if (!isDigit && !(i == comma + 1 && c == L'-')) {
                isNumber = false;
                break;
            }
        }

        if (isNumber) {
            iconIndex = _wtoi(expanded.c_str() + comma + 1);
            expanded.resize(comma);
        }
    }

    expanded = ResolveAppExecutionAlias(expanded);

    HICON hIcon = nullptr;
    if (ExtractIconExW(expanded.c_str(), iconIndex, &hIcon, nullptr, 1) &&
        hIcon) {
        return hIcon;
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(expanded.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }

    Wh_Log(L"Couldn't load an icon from %s", iconPath.c_str());
    return nullptr;
}

// A "shell:" path, e.g. shell:AppsFolder\<AUMID> for a Store app whose icon
// (Notepad, Calculator, ...) can't be extracted from its legacy .exe stub.
bool IsShellPath(std::wstring const& s) {
    return s.size() >= 6 && _wcsnicmp(s.c_str(), L"shell:", 6) == 0;
}

bool DecodeShellPathIcon(std::wstring const& path, DecodedIcon* decoded) {
    std::wstring expanded = ExpandEnvVars(path);

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(expanded.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        Wh_Log(L"Couldn't parse shell path %s", path.c_str());
        return false;
    }

    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !factory) {
        return false;
    }

    SIZE size = {32, 32};
    HBITMAP hBitmap = nullptr;
    hr = factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &hBitmap);
    if (FAILED(hr) || !hBitmap) {
        return false;
    }

    bool succeeded = ReadBitmapPixels(hBitmap, decoded);
    DeleteObject(hBitmap);

    // The shell returns premultiplied alpha already; just handle the
    // fully-opaque (no alpha channel) case.
    if (succeeded && HasNoAlphaChannel(decoded->pixels)) {
        MakeOpaque(decoded->pixels);
    }

    return succeeded;
}

std::shared_ptr<DecodedIcon> ResolveIcon(std::wstring const& iconSetting,
                                         std::wstring const& command) {
    auto decoded = std::make_shared<DecodedIcon>();

    bool isPath = !iconSetting.empty() && LooksLikeIconPath(iconSetting);
    if (isPath && IsShellPath(iconSetting)) {
        DecodeShellPathIcon(iconSetting, decoded.get());
        return decoded;
    }

    HICON hIcon = nullptr;
    if (isPath) {
        hIcon = LoadIconFromPath(iconSetting);
    } else if (iconSetting.empty() && !command.empty()) {
        hIcon = ExtractCommandIcon(command);
    }

    if (hIcon) {
        DecodeIcon(hIcon, decoded.get());
        DestroyIcon(hIcon);
    }

    return decoded;
}

// Resolving an icon is expensive - the search path, the App Paths registry
// keys, a reparse point, and for a shell path the package metadata of a Store
// app - and it happens on the Explorer UI thread while a window or a tab is
// being built. The decoded pixels don't depend on the thread or the window, so
// they're resolved once and reused; the cache is dropped when the settings
// change. An entry with no pixels is a remembered failure.
std::mutex g_iconCacheMutex;
std::unordered_map<std::wstring, std::shared_ptr<DecodedIcon>> g_iconCache;

std::shared_ptr<DecodedIcon> GetIcon(std::wstring const& iconSetting,
                                     std::wstring const& command) {
    // '\n' can't appear in either part, so it's an unambiguous separator.
    std::wstring key = iconSetting + L'\n' + command;

    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        auto it = g_iconCache.find(key);
        if (it != g_iconCache.end()) {
            return it->second;
        }
    }

    auto decoded = ResolveIcon(iconSetting, command);

    std::lock_guard<std::mutex> lock(g_iconCacheMutex);
    return g_iconCache.insert_or_assign(std::move(key), std::move(decoded))
        .first->second;
}

// Returns nullptr if no icon could be resolved from the settings.
muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command) {
    bool isPath = !iconSetting.empty() && LooksLikeIconPath(iconSetting);

    if (isPath || iconSetting.empty()) {
        if (auto source = CreateImageSource(*GetIcon(iconSetting, command))) {
            muxc::ImageIcon imageIcon;
            imageIcon.Source(source);
            return imageIcon;
        }
    }

    std::wstring glyph;
    if (!isPath) {
        glyph = ParseGlyphSetting(iconSetting.c_str());
    }

    if (glyph.empty()) {
        return nullptr;
    }

    muxc::FontIcon fontIcon;
    fontIcon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    fontIcon.Glyph(glyph.c_str());
    return fontIcon;
}

muxc::IconElement CreateIconElement(std::wstring const& iconSetting,
                                    std::wstring const& command,
                                    PCWSTR defaultGlyph) {
    if (auto icon = TryCreateIconElement(iconSetting, command)) {
        return icon;
    }

    muxc::FontIcon fontIcon;
    fontIcon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    fontIcon.Glyph(defaultGlyph);
    return fontIcon;
}

// Icon for a command bar button, honoring the item's "Hide icon" option.
// Since the label is collapsed, a button without any icon would render as an
// empty box, so an icon which couldn't be resolved (a mistyped command, a
// missing executable) falls back to a visible placeholder glyph.
muxc::IconElement MakeCommandButtonIcon(ActionItem const& item) {
    if (item.hideIcon) {
        return nullptr;
    }
    return CreateIconElement(item.icon, item.command, L"");
}

////////////////////////////////////////////////////////////////////////////////
// Managing the buttons in the command bar.

bool IsOurElement(muxc::ICommandBarElement const& command) {
    // Matches both our buttons and our separators.
    auto element = command.try_as<mux::FrameworkElement>();
    return element &&
           std::wstring_view(element.Name()).starts_with(kButtonNamePrefix);
}

bool HasOurButtons(muxc::CommandBar const& commandBar) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (IsOurElement(command)) {
            return true;
        }
    }

    return false;
}

std::wstring GetButtonIconUri(muxc::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon) {
        return std::wstring();
    }

    wf::Uri uri{nullptr};
    if (auto imageIcon = icon.try_as<muxc::ImageIcon>()) {
        if (auto source = imageIcon.Source()) {
            if (auto svgSource =
                    source.try_as<muxm::Imaging::SvgImageSource>()) {
                uri = svgSource.UriSource();
            }
        }
    } else if (auto iconSourceElement =
                   icon.try_as<muxc::IconSourceElement>()) {
        if (auto iconSource = iconSourceElement.IconSource()) {
            if (auto imageIconSource =
                    iconSource.try_as<muxc::ImageIconSource>()) {
                if (auto source = imageIconSource.ImageSource()) {
                    if (auto svgSource =
                            source.try_as<muxm::Imaging::SvgImageSource>()) {
                        uri = svgSource.UriSource();
                    }
                }
            }
        }
    } else if (auto bitmapIcon = icon.try_as<muxc::BitmapIcon>()) {
        uri = bitmapIcon.UriSource();
    }

    if (!uri) {
        return std::wstring();
    }

    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

// Returns an index into kDefaultButtons, or -1.
int IdentifyDefaultButton(muxc::AppBarButton const& button) {
    std::wstring uri = GetButtonIconUri(button);
    if (!uri.empty()) {
        for (auto& c : uri) {
            c = towlower(c);
        }

        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);

        // The View button's icon reflects the current view mode
        // (windows.iconsize.list.svg, windows.iconsize.details.svg, etc.).
        if (fileName.starts_with(L"windows.iconsize.")) {
            return kViewButtonIndex;
        }

        for (int i = 0; i < kDefaultButtonCount; i++) {
            if (fileName == kDefaultButtons[i].svgFileName) {
                return i;
            }
        }
    }

    // The sort button has an automation id.
    try {
        if (mux::Automation::AutomationProperties::GetAutomationId(button) ==
            L"SortAndGroupButton") {
            return kSortButtonIndex;
        }
    } catch (...) {
    }

    return -1;
}

std::wstring GetAutomationId(mux::FrameworkElement const& element) {
    try {
        return std::wstring{
            mux::Automation::AutomationProperties::GetAutomationId(element)};
    } catch (...) {
        return std::wstring();
    }
}

// Which "hide" setting an element we touch is governed by. Looked up live
// rather than snapshotted, because the visibility watcher below re-applies the
// setting long after the command bar was processed.
enum class ManagedKind {
    Button,           // A default button, index into kDefaultButtons.
    SeparatorAfter,   // A separator, index of the button it follows.
    GroupSeparator,   // A separator that's redundant once its group is empty.
    DetailsToggle,    // The Details pane toggle.
    OverflowElement,  // The "See more" button's separator.
};

// A command a group separator delimits, remembered so the separator can be
// hidden once nothing in its group is left to show.
struct GroupMember {
    winrt::weak_ref<mux::UIElement> element;
    int defaultButtonIndex;  // -1 for commands we don't manage.
};

struct ManagedTarget {
    ManagedKind kind;
    int index = 0;
    // GroupSeparator only. Shared so the target stays cheap to copy into the
    // visibility watcher below.
    std::shared_ptr<std::vector<GroupMember>> group;
};

bool ShouldHide(ManagedTarget const& target);

// The visibility an element would have if we weren't hiding it: the value
// Explorer last set, which we track, or its current value if we've never
// touched it.
mux::Visibility EffectiveVisibility(mux::UIElement const& element) {
    {
        std::lock_guard<std::mutex> lock(g_managedElementsMutex);
        auto* entry = FindManagedElement(element);
        if (entry && entry->hasOriginalVisibility) {
            return entry->originalVisibility;
        }
    }

    return element.Visibility();
}

// True when every command in the group is hidden, by us or by Explorer, so its
// separator would leave a double line behind.
bool IsGroupHidden(std::shared_ptr<std::vector<GroupMember>> const& group) {
    if (!group || group->empty()) {
        return false;
    }

    for (auto const& member : *group) {
        auto element = member.element.get();
        if (!element) {
            continue;
        }

        if (member.defaultButtonIndex >= 0 &&
            ShouldHide({ManagedKind::Button, member.defaultButtonIndex})) {
            continue;
        }

        if (EffectiveVisibility(element) == mux::Visibility::Collapsed) {
            continue;
        }

        return false;
    }

    return true;
}

bool ShouldHide(ManagedTarget const& target) {
    // Evaluated before taking the settings lock: it consults the setting of
    // each group member itself.
    if (target.kind == ManagedKind::GroupSeparator) {
        return IsGroupHidden(target.group);
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);
    switch (target.kind) {
        case ManagedKind::Button:
            return g_settings.hideDefaultButtons[target.index];
        case ManagedKind::SeparatorAfter:
            return g_settings.hideSeparatorAfterButton[target.index];
        case ManagedKind::DetailsToggle:
            return g_settings.hideDetailsButton;
        case ManagedKind::OverflowElement:
            return g_settings.hideMoreButton;
        case ManagedKind::GroupSeparator:
            break;
    }

    return false;
}

// Set while we're the ones assigning Visibility, so the watcher can tell our
// own writes apart from Explorer's. The callback runs synchronously on the
// thread doing the write, hence thread_local.
thread_local int g_settingVisibilityDepth;

void SetVisibilityInternal(mux::UIElement const& element,
                           mux::Visibility visibility) {
    g_settingVisibilityDepth++;
    try {
        element.Visibility(visibility);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
    g_settingVisibilityDepth--;
}

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow = false);

void UpdateCommandBar(muxc::CommandBar const& commandBar);

// Both a visibility recompute (hiding a group separator depends on the whole
// group, so a single element changing isn't enough to decide) and a full
// update after Explorer touched the command list are deferred and coalesced
// per command bar: Explorer updates a whole run of commands on every selection
// change, and appending our own buttons raises one change notification each.
//
// The pending entries are keyed by address, which is safe here because the
// queued work always removes its own entry: an entry can't outlive the command
// bar it belongs to and be matched against a later one at the same address.
std::mutex g_pendingUpdatesMutex;
std::unordered_map<void*, bool> g_pendingUpdates;  // Value: full update.

void QueueCommandBarUpdate(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar,
    bool fullUpdate) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar) {
        return;
    }

    void* key = winrt::get_abi(commandBar);
    {
        std::lock_guard<std::mutex> lock(g_pendingUpdatesMutex);
        auto [it, inserted] = g_pendingUpdates.insert({key, fullUpdate});
        if (!inserted) {
            // Already queued; the queued work reads the flag when it runs, so
            // a full update can still be requested on top of a recompute.
            it->second = it->second || fullUpdate;
            return;
        }
    }

    auto takePending = [key]() {
        std::lock_guard<std::mutex> lock(g_pendingUpdatesMutex);
        bool full = false;
        auto it = g_pendingUpdates.find(key);
        if (it != g_pendingUpdates.end()) {
            full = it->second;
            g_pendingUpdates.erase(it);
        }

        return full;
    };

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::
            GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    if (!dispatcherQueue.TryEnqueue([weakCommandBar, takePending]() {
            bool full = takePending();

            if (g_unloading) {
                return;
            }

            if (auto commandBar = weakCommandBar.get()) {
                try {
                    if (full) {
                        UpdateCommandBar(commandBar);
                    } else {
                        ApplyDefaultButtonVisibility(commandBar);
                    }
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        })) {
        takePending();
    }
}

void QueueVisibilityRecompute(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar) {
    QueueCommandBarUpdate(weakCommandBar, /*fullUpdate=*/false);
}

// Explorer shows and hides its contextual commands (Set as background, Rotate
// left/right, Extract all, ...) as the selection changes, which overwrites our
// collapse. Watching the property lets us re-apply immediately, and keeps the
// remembered original in sync so disabling the mod restores what Explorer
// wanted rather than what it happened to be when we first saw the element.
// The caller has established, under the lock, that the element isn't watched
// yet.
void WatchVisibility(mux::UIElement const& element,
                     ManagedTarget const& target,
                     winrt::weak_ref<muxc::CommandBar> const& owner) {
    int64_t token = element.RegisterPropertyChangedCallback(
        mux::UIElement::VisibilityProperty(),
        [target, owner](mux::DependencyObject const& sender,
                        mux::DependencyProperty const&) {
            if (g_unloading || g_settingVisibilityDepth > 0) {
                return;
            }

            auto element = sender.try_as<mux::UIElement>();
            if (!element) {
                return;
            }

            // Explorer changed it, so this is the new baseline to restore.
            mux::Visibility visibility = element.Visibility();
            {
                std::lock_guard<std::mutex> lock(g_managedElementsMutex);
                auto& entry = GetManagedElement(element);
                entry.originalVisibility = visibility;
                entry.hasOriginalVisibility = true;
            }

            if (visibility != mux::Visibility::Collapsed &&
                ShouldHide(target)) {
                SetVisibilityInternal(element, mux::Visibility::Collapsed);
            }

            // This element may belong to a group whose separator now has to
            // appear or disappear with it.
            QueueVisibilityRecompute(owner);
        });

    std::lock_guard<std::mutex> lock(g_managedElementsMutex);
    auto& entry = GetManagedElement(element);
    entry.watched = true;
    entry.visibilityToken = token;
}

// Unregisters the watchers of this thread's elements, keeping their remembered
// original state for the restore which follows.
void UnwatchVisibilityForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();

    std::vector<std::pair<mux::UIElement, int64_t>> taken;
    {
        std::lock_guard<std::mutex> lock(g_managedElementsMutex);
        for (auto& entry : g_managedElements) {
            if (entry.threadId != threadId || !entry.watched) {
                continue;
            }

            if (auto element = entry.element.get()) {
                taken.push_back({element, entry.visibilityToken});
            }

            entry.watched = false;
            entry.visibilityToken = 0;
        }
    }

    for (auto const& [element, token] : taken) {
        try {
            element.UnregisterPropertyChangedCallback(
                mux::UIElement::VisibilityProperty(), token);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

void ForgetManagedElementsForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();

    std::lock_guard<std::mutex> lock(g_managedElementsMutex);
    for (auto it = g_managedElements.begin();
         it != g_managedElements.end();) {
        if (it->threadId == threadId) {
            it = g_managedElements.erase(it);
        } else {
            ++it;
        }
    }
}

// Sets an element's visibility to Collapsed when hiding, or restores its
// original (pre-modification) visibility otherwise. The original is captured
// the first time the element is seen, so we never force-show something
// Explorer intentionally kept collapsed.
void SetManagedVisibility(mux::UIElement const& element,
                          ManagedTarget const& target,
                          bool forceShow,
                          winrt::weak_ref<muxc::CommandBar> const& owner) {
    mux::Visibility original;
    bool watch;
    {
        std::lock_guard<std::mutex> lock(g_managedElementsMutex);
        auto& entry = GetManagedElement(element);
        if (!entry.hasOriginalVisibility) {
            entry.originalVisibility = element.Visibility();
            entry.hasOriginalVisibility = true;
        }

        original = entry.originalVisibility;
        watch = !forceShow && !entry.watched;
    }

    if (watch) {
        WatchVisibility(element, target, owner);
    }

    SetVisibilityInternal(element, !forceShow && ShouldHide(target)
                                       ? mux::Visibility::Collapsed
                                       : original);
}

// Controls the gap between command bar buttons. Command bar buttons have a
// fixed MinWidth that leaves padding around the icon, so we drop it to let the
// button hug its content and use the horizontal margin (half of `spacing` on
// each side) as the actual gap. A negative spacing (or reset) restores the
// button's original margin and MinWidth, leaving the default untouched.
void ApplyItemSpacing(muxc::AppBarButton const& button,
                      int spacing,
                      bool reset) {
    mux::Thickness originalMargin;
    double originalMinWidth;
    {
        std::lock_guard<std::mutex> lock(g_managedElementsMutex);
        auto& entry = GetManagedElement(button);
        if (!entry.hasOriginalSpacing) {
            entry.originalMargin = button.Margin();
            entry.originalMinWidth = button.MinWidth();
            entry.hasOriginalSpacing = true;
        }

        originalMargin = entry.originalMargin;
        originalMinWidth = entry.originalMinWidth;
    }

    if (reset || spacing < 0) {
        button.Margin(originalMargin);
        button.MinWidth(originalMinWidth);
        return;
    }

    double half = spacing / 2.0;
    mux::Thickness margin = originalMargin;
    margin.Left = half;
    margin.Right = half;
    button.Margin(margin);
    button.MinWidth(0);
}

mux::FrameworkElement FindDescendantByName(mux::DependencyObject const& root,
                                           std::wstring_view name) {
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto element = child.try_as<mux::FrameworkElement>();
            element && element.Name() == name) {
            return element;
        }

        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }

    return nullptr;
}

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow) {
    bool hideMore;
    int itemSpacing;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        hideMore = g_settings.hideMoreButton;
        itemSpacing = g_settings.itemSpacing;
    }

    bool isPrimary = commandBar.Name() == L"FileExplorerCommandBar";

    auto weakCommandBar = winrt::make_weak(commandBar);
    auto setVisibility = [&weakCommandBar, forceShow](
                             mux::UIElement const& element,
                             ManagedTarget const& target) {
        SetManagedVisibility(element, target, forceShow, weakCommandBar);
    };

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    // Classify everything up front: identifying a button is relatively
    // expensive, and the separator handling below needs to look ahead.
    struct Entry {
        muxc::ICommandBarElement command;
        bool isOurs = false;
        bool isSeparator = false;
        bool isDetailsToggle = false;
        int defaultIndex = -1;  // Index into kDefaultButtons, or -1.
    };

    std::vector<Entry> entries;
    entries.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        Entry entry;
        entry.command = commands.GetAt(i);
        entry.isOurs = IsOurElement(entry.command);
        entry.isSeparator =
            !entry.isOurs &&
            static_cast<bool>(entry.command.try_as<muxc::AppBarSeparator>());

        if (!entry.isOurs && !entry.isSeparator) {
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                // The Details pane toggle lives in the secondary command bar.
                if (GetAutomationId(button) == L"DetailsPaneToggleButton") {
                    entry.isDetailsToggle = true;
                } else {
                    entry.defaultIndex = IdentifyDefaultButton(button);
                    if (entry.defaultIndex < 0) {
                        // Expected for the contextual commands Explorer keeps
                        // in the list, but it's also what a built-in button
                        // whose icon file was renamed in a new Windows build
                        // looks like, so log enough to tell the two apart.
                        Wh_Log(L"Unrecognized command %u: icon %s, "
                               L"automation id %s",
                               i, GetButtonIconUri(button).c_str(),
                               GetAutomationId(button).c_str());
                    }
                }
            }
        }

        entries.push_back(std::move(entry));
    }

    // Explorer keeps ~20 contextual, zero-width commands (Extract, Eject, ...)
    // in the list between the View group and the overflow region. The vertical
    // line the user sees "after View" is really the separator right before our
    // first custom button, so locate that one.
    uint32_t firstCustomIndex = count;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isOurs) {
            firstCustomIndex = i;
            break;
        }
    }

    uint32_t viewSeparatorIndex = count;
    if (firstCustomIndex > 0 && firstCustomIndex < count &&
        entries[firstCustomIndex - 1].isSeparator) {
        viewSeparatorIndex = firstCustomIndex - 1;
    } else {
        // Without custom buttons (the mod can be used just to hide the
        // built-in ones) it's the first separator after the View button.
        for (uint32_t i = 0; i < count && viewSeparatorIndex == count; i++) {
            if (entries[i].defaultIndex != kViewButtonIndex) {
                continue;
            }

            for (uint32_t j = i + 1; j < count; j++) {
                if (entries[j].isSeparator) {
                    viewSeparatorIndex = j;
                    break;
                }
            }
        }
    }

    // The commands a separator introduces, up to the next separator (or our
    // own buttons, or the end of the list). Explorer puts a separator in front
    // of its contextual commands; once all of them are hidden, that separator
    // would sit right next to the next one as a double line, so it's hidden
    // together with the group it belongs to.
    auto collectGroup = [&entries, count](uint32_t separatorIndex) {
        auto group = std::make_shared<std::vector<GroupMember>>();

        for (uint32_t i = separatorIndex + 1; i < count; i++) {
            auto const& entry = entries[i];
            if (entry.isSeparator || entry.isOurs) {
                break;
            }

            if (auto element = entry.command.try_as<mux::UIElement>()) {
                group->push_back(
                    {winrt::make_weak(element), entry.defaultIndex});
            }
        }

        return group;
    };

    // The default-button index of the immediately preceding element, so a
    // separator can be attributed to the button right before it.
    int prevIndex = -1;
    for (uint32_t i = 0; i < count; i++) {
        auto const& entry = entries[i];

        if (entry.isOurs) {
            // Our custom buttons get the spacing too, for a uniform look.
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                ApplyItemSpacing(button, itemSpacing, forceShow);
            }
            prevIndex = -1;
            continue;
        }

        if (entry.isSeparator) {
            auto separator = entry.command.as<muxc::AppBarSeparator>();

            int target = -1;
            if (prevIndex == kNewButtonIndex) {
                target = kNewButtonIndex;
            } else if (prevIndex == kDeleteButtonIndex) {
                target = kDeleteButtonIndex;
            } else if (i == viewSeparatorIndex) {
                target = kViewButtonIndex;
            }

            if (target >= 0) {
                setVisibility(separator,
                              {ManagedKind::SeparatorAfter, target});
            } else {
                // One of Explorer's contextual separators. Hidden only when
                // everything it introduces is hidden, so we never reveal a
                // separator Explorer keeps collapsed.
                setVisibility(
                    separator,
                    {ManagedKind::GroupSeparator, -1, collectGroup(i)});
            }

            prevIndex = -1;
            continue;
        }

        if (entry.isDetailsToggle) {
            setVisibility(entry.command.as<mux::UIElement>(),
                          {ManagedKind::DetailsToggle});
            prevIndex = -1;
            continue;
        }

        if (entry.defaultIndex >= 0) {
            auto button = entry.command.as<muxc::AppBarButton>();
            setVisibility(button, {ManagedKind::Button, entry.defaultIndex});
            // Spacing is applied only to real buttons, never the zero-width
            // contextual commands (which would otherwise occupy space).
            ApplyItemSpacing(button, itemSpacing, forceShow);
        }

        prevIndex = entry.defaultIndex;
    }

    // The "See more" (...) overflow button and its separator (a template
    // part). OverflowButtonVisibility alone doesn't hide the separator in
    // Explorer's template, so collapse it explicitly too.
    if (isPrimary) {
        muxc::CommandBarOverflowButtonVisibility originalOverflow;
        {
            std::lock_guard<std::mutex> lock(g_managedElementsMutex);
            auto& entry = GetManagedElement(commandBar);
            if (!entry.hasOriginalOverflow) {
                entry.originalOverflow = commandBar.OverflowButtonVisibility();
                entry.hasOriginalOverflow = true;
            }

            originalOverflow = entry.originalOverflow;
        }

        commandBar.OverflowButtonVisibility(
            !forceShow && hideMore
                ? muxc::CommandBarOverflowButtonVisibility::Collapsed
                : originalOverflow);

        if (auto overflowSeparator =
                FindDescendantByName(commandBar, L"OverflowSeparator")) {
            setVisibility(overflowSeparator, {ManagedKind::OverflowElement});
        }
    }
}

muxc::AppBarButton CreateBareButton(int index,
                                    std::wstring const& tooltip,
                                    muxc::IconElement const& icon) {
    std::wstring name = kButtonNamePrefix;
    name += L'_';
    name += std::to_wstring(index);

    muxc::AppBarButton button;
    button.Name(name.c_str());
    button.Label(tooltip.c_str());
    button.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(icon);

    if (!tooltip.empty()) {
        muxc::ToolTipService::SetToolTip(button, winrt::box_value(
                                                     winrt::hstring{tooltip}));
    }

    return button;
}

muxc::AppBarButton CreateActionButton(ActionItem const& item, int index) {
    muxc::AppBarButton button = CreateBareButton(index, item.name, MakeCommandButtonIcon(item));

    button.Click([item](wf::IInspectable const& sender,
                        mux::RoutedEventArgs const&) {
        if (auto element = sender.try_as<mux::FrameworkElement>()) {
            OnActionInvoked(element, item);
        }
    });

    return button;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton);

// A regular menu item, or a submenu with a further level of entries.
muxc::MenuFlyoutItemBase CreateMenuEntry(
    ActionItem const& item,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    if (item.isMenu && !item.subItems.empty()) {
        muxc::MenuFlyoutSubItem subMenu;
        subMenu.Text(item.name.c_str());
        if (!item.hideIcon) {
            if (auto icon = TryCreateIconElement(item.icon, item.command)) {
                subMenu.Icon(icon);
            }
        }

        AppendMenuEntries(item.subItems, subMenu.Items(), weakButton);
        return subMenu;
    }

    muxc::MenuFlyoutItem menuItem;
    menuItem.Text(item.name.c_str());
    if (!item.hideIcon) {
        if (auto icon = TryCreateIconElement(item.icon, item.command)) {
            menuItem.Icon(icon);
        }
    }

    menuItem.Click([item, weakButton](wf::IInspectable const&,
                                      mux::RoutedEventArgs const&) {
        if (auto button = weakButton.get()) {
            OnActionInvoked(button, item);
        }
    });

    return menuItem;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    for (auto const& item : items) {
        target.Append(CreateMenuEntry(item, weakButton));

        if (item.separatorAfter) {
            target.Append(muxc::MenuFlyoutSeparator());
        }
    }
}

// The hover timers of our menu buttons. A started timer is rooted by the
// dispatcher queue and its Tick handler lives in this DLL, so the timers have
// to be stopped before the mod is unloaded.
struct HoverTimerEntry {
    DWORD threadId;
    winrt::weak_ref<mux::DispatcherTimer> timer;
};

std::mutex g_hoverTimersMutex;
std::vector<HoverTimerEntry> g_hoverTimers;

void TrackHoverTimer(mux::DispatcherTimer const& timer) {
    DWORD threadId = GetCurrentThreadId();

    std::lock_guard<std::mutex> lock(g_hoverTimersMutex);

    // Drop this thread's timers which are gone with their button.
    for (auto it = g_hoverTimers.begin(); it != g_hoverTimers.end();) {
        if (it->threadId == threadId && !it->timer.get()) {
            it = g_hoverTimers.erase(it);
        } else {
            ++it;
        }
    }

    g_hoverTimers.push_back({threadId, winrt::make_weak(timer)});
}

void StopHoverTimersForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();

    std::vector<mux::DispatcherTimer> taken;
    {
        std::lock_guard<std::mutex> lock(g_hoverTimersMutex);
        for (auto it = g_hoverTimers.begin(); it != g_hoverTimers.end();) {
            if (it->threadId != threadId) {
                ++it;
                continue;
            }

            if (auto timer = it->timer.get()) {
                taken.push_back(std::move(timer));
            }

            it = g_hoverTimers.erase(it);
        }
    }

    for (auto const& timer : taken) {
        try {
            timer.Stop();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

muxc::AppBarButton CreateMenuButton(ActionItem const& item,
                                    int index,
                                    bool openOnHover,
                                    int hoverDelayMs) {
    muxc::AppBarButton button = CreateBareButton(index, item.name, MakeCommandButtonIcon(item));

    // The menu flyout is shown in its own popup window, so resolve the
    // Explorer window from the anchor button, not from the clicked menu item.
    auto weakButton = winrt::make_weak(button);

    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    // Every menu entry resolves an icon, which is the expensive part of
    // building the menu, so the entries are created the first time the menu is
    // needed instead of while the window or the tab is being built.
    auto ensureMenuEntries = [subItems = item.subItems, weakButton](
                                 muxc::MenuFlyout const& menu) {
        if (!menu || menu.Items().Size() > 0) {
            return;
        }

        try {
            AppendMenuEntries(subItems, menu.Items(), weakButton);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    };

    menu.Opening([ensureMenuEntries](wf::IInspectable const& sender,
                                     wf::IInspectable const&) {
        ensureMenuEntries(sender.try_as<muxc::MenuFlyout>());
    });

    // Also while the pointer is on its way to the button, which is both a
    // little earlier than the click and a safety net for the case above.
    button.PointerEntered(
        [ensureMenuEntries, weakButton](
            wf::IInspectable const&,
            mux::Input::PointerRoutedEventArgs const&) {
            if (auto button = weakButton.get()) {
                ensureMenuEntries(button.Flyout().try_as<muxc::MenuFlyout>());
            }
        });

    button.Flyout(menu);

    if (openOnHover) {
        auto showMenu = [weakButton]() {
            auto button = weakButton.get();
            if (!button) {
                return;
            }

            if (auto flyout = button.Flyout(); flyout && !flyout.IsOpen()) {
                flyout.ShowAt(button);
            }
        };

        if (hoverDelayMs <= 0) {
            button.PointerEntered(
                [showMenu](wf::IInspectable const&,
                           mux::Input::PointerRoutedEventArgs const&) {
                    showMenu();
                });
        } else {
            mux::DispatcherTimer timer;
            timer.Interval(std::chrono::milliseconds(hoverDelayMs));
            TrackHoverTimer(timer);

            // Capture the timer weakly in its own Tick handler to avoid a
            // reference cycle.
            timer.Tick([weakTimer = winrt::make_weak(timer), showMenu](
                           wf::IInspectable const&,
                           wf::IInspectable const&) {
                if (auto timer = weakTimer.get()) {
                    timer.Stop();
                }
                showMenu();
            });

            button.PointerEntered(
                [timer](wf::IInspectable const&,
                        mux::Input::PointerRoutedEventArgs const&) {
                    timer.Stop();  // Restart the delay.
                    timer.Start();
                });

            auto stopTimer =
                [timer](wf::IInspectable const&,
                        mux::Input::PointerRoutedEventArgs const&) {
                    timer.Stop();
                };
            button.PointerExited(stopTimer);
            button.PointerCanceled(stopTimer);
        }
    }

    return button;
}

void EnsureButtons(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    if (HasOurButtons(commandBar)) {
        return;
    }

    bool openMenuOnHover;
    int menuHoverDelay;
    std::vector<ActionItem> items;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
        items = g_settings.items;
    }

    if (items.empty()) {
        return;
    }

    Wh_Log(L"Adding %zu items to command bar", items.size());

    auto commands = commandBar.PrimaryCommands();
    for (size_t i = 0; i < items.size(); i++) {
        auto const& item = items[i];
        if (item.isMenu && !item.subItems.empty()) {
            commands.Append(CreateMenuButton(item, (int)i, openMenuOnHover,
                                             menuHoverDelay));
        } else {
            commands.Append(CreateActionButton(item, (int)i));
        }

        if (item.separatorAfter) {
            muxc::AppBarSeparator separator;
            std::wstring name = kButtonNamePrefix;
            name += L"_Sep_";
            name += std::to_wstring(i);
            separator.Name(name.c_str());
            commands.Append(separator);
        }
    }
}

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    // Custom buttons only go in the primary command bar; the secondary bar
    // just holds the Details pane toggle.
    if (commandBar.Name() == L"FileExplorerCommandBar") {
        EnsureButtons(commandBar);
    }

    ApplyDefaultButtonVisibility(commandBar);
}

void RemoveOurButtons(muxc::CommandBar const& commandBar) {
    auto commands = commandBar.PrimaryCommands();
    for (uint32_t i = commands.Size(); i > 0; i--) {
        if (IsOurElement(commands.GetAt(i - 1))) {
            commands.RemoveAt(i - 1);
        }
    }
}

void OnCommandBarAdded(muxc::CommandBar const& commandBar) {
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);

        // Prune entries whose command bar is gone.
        for (auto it = g_entries.begin(); it != g_entries.end();) {
            if (!it->commandBar.get()) {
                it = g_entries.erase(it);
            } else {
                ++it;
            }
        }

        for (auto const& entry : g_entries) {
            if (entry.commandBar.get() == commandBar) {
                // Already tracked (e.g. re-added to the tree).
                return;
            }
        }
    }

    CommandBarEntry entry;
    entry.threadId = GetCurrentThreadId();
    entry.commandBar = winrt::make_weak(commandBar);

    // Re-add the buttons whenever the command bar reloads.
    entry.loadedToken = commandBar.Loaded(
        [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            if (auto commandBar = sender.try_as<muxc::CommandBar>()) {
                UpdateCommandBar(commandBar);
            }
        });

    // Re-add the buttons and reapply visibility if Explorer ever rebuilds
    // the command list.
    entry.vectorChangedToken = commandBar.PrimaryCommands().VectorChanged(
        [weakCommandBar = winrt::make_weak(commandBar)](
            wfc::IObservableVector<muxc::ICommandBarElement> const&,
            wfc::IVectorChangedEventArgs const&) {
            if (g_unloading) {
                return;
            }

            // Defer the update; mutating the vector from within its own change
            // notification isn't allowed. Coalesced, since adding our own
            // buttons raises one notification per button.
            QueueCommandBarUpdate(weakCommandBar, /*fullUpdate=*/true);
        });

    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        g_entries.push_back(std::move(entry));
    }

    UpdateCommandBar(commandBar);
}

std::vector<CommandBarEntry> TakeEntriesForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<CommandBarEntry> taken;

    std::lock_guard<std::mutex> lock(g_entriesMutex);
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (it->threadId == threadId) {
            taken.push_back(std::move(*it));
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }

    return taken;
}

void RemoveButtonsForCurrentThread() {
    // Before restoring anything, so the watcher can't fight the restore.
    UnwatchVisibilityForCurrentThread();
    StopHoverTimersForCurrentThread();

    for (auto& entry : TakeEntriesForCurrentThread()) {
        auto commandBar = entry.commandBar.get();
        if (!commandBar) {
            continue;
        }

        try {
            commandBar.Loaded(entry.loadedToken);
            commandBar.PrimaryCommands().VectorChanged(
                entry.vectorChangedToken);
            RemoveOurButtons(commandBar);
            ApplyDefaultButtonVisibility(commandBar, /*forceShow=*/true);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    // Everything has been restored, so the remembered original state of this
    // thread's elements isn't needed anymore.
    ForgetManagedElementsForCurrentThread();
}

void RefreshButtonsForCurrentThread() {
    // The buttons are recreated below, so the hover timers of the old ones are
    // stopped and forgotten first.
    StopHoverTimersForCurrentThread();

    std::vector<winrt::weak_ref<muxc::CommandBar>> commandBars;
    {
        DWORD threadId = GetCurrentThreadId();
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        for (auto const& entry : g_entries) {
            if (entry.threadId == threadId) {
                commandBars.push_back(entry.commandBar);
            }
        }
    }

    for (auto const& weakCommandBar : commandBars) {
        auto commandBar = weakCommandBar.get();
        if (!commandBar) {
            continue;
        }

        try {
            // Recreate the buttons with the new settings.
            RemoveOurButtons(commandBar);
            UpdateCommandBar(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Command bar discovery.
//
// The command bars are found by hooking a couple of functions of File
// Explorer's own WinUI 3 code (FileExplorerExtensions.dll) and by walking the
// XAML tree from there with the public VisualTreeHelper API.
//
// Note: XAML Diagnostics (InitializeXamlDiagnosticsEx) would be an easier way
// to watch for the command bar, but only one XAML diagnostics consumer can be
// active per process, which makes it conflict with other tools and mods, such
// as Windows 11 File Explorer Styler. That's why it's not used here.

// Note: the `this` pointer our hooks get is a C++/WinRT implementation object,
// not a XAML object, and there's no supported way to turn one into the other.
// Guessing at its layout is a good way to crash Explorer, so the hooks which
// only have a `this` pointer are used as a signal to rescan the thread, and the
// XAML objects themselves only ever come from typed parameters, from the
// command bars we already know about, or from the focused element.

bool IsTargetCommandBarName(std::wstring_view name) {
    return name == L"FileExplorerCommandBar" ||
           name == L"FileExplorerSecondaryCommandBar";
}

// Both command bars are found, so there's nothing left to look for.
bool FoundAllCommandBars(std::vector<muxc::CommandBar> const& commandBars) {
    bool primary = false;
    bool secondary = false;

    for (auto const& commandBar : commandBars) {
        if (commandBar.Name() == L"FileExplorerCommandBar") {
            primary = true;
        } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
            secondary = true;
        }
    }

    return primary && secondary;
}

void CollectCommandBars(mux::DependencyObject const& root,
                        int depth,
                        std::vector<muxc::CommandBar>* commandBars) {
    if (depth > 64) {
        return;
    }

    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        // The scan runs on every focus event, so stop walking the tree as soon
        // as there's nothing left to find.
        if (FoundAllCommandBars(*commandBars)) {
            return;
        }

        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto commandBar = child.try_as<muxc::CommandBar>();
            commandBar && IsTargetCommandBarName(commandBar.Name())) {
            commandBars->push_back(std::move(commandBar));
            // No need to descend into a command bar we already found.
            continue;
        }

        CollectCommandBars(child, depth + 1, commandBars);
    }
}

// Finds all of the File Explorer command bars of the XAML island the given
// element belongs to. Must run on the element's UI thread.
void ScanXamlRootForCommandBars(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content();
    if (!content) {
        return;
    }

    std::vector<muxc::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars) {
        OnCommandBarAdded(commandBar);
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Same as above, but deferred, for the cases where the command bar isn't in
// the tree yet by the time our hook runs.
void ScheduleXamlRootScan(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }

    dispatcherQueue.TryEnqueue([weakElement = winrt::make_weak(element)]() {
        if (auto element = weakElement.get()) {
            ScanXamlRootForCommandBars(element);
        }
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::CommandBar GetKnownCommandBarForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();

    std::lock_guard<std::mutex> lock(g_entriesMutex);
    for (auto const& entry : g_entries) {
        if (entry.threadId != threadId) {
            continue;
        }

        if (auto commandBar = entry.commandBar.get()) {
            return commandBar;
        }
    }

    return nullptr;
}

// Looks for the command bars of the current thread's XAML island without an
// element to start from: either from a command bar which is already known for
// this thread, or from the focused element. Must run on the UI thread.
void ScanCurrentThreadForCommandBars() try {
    if (g_unloading) {
        return;
    }

    if (auto knownCommandBar = GetKnownCommandBarForCurrentThread()) {
        ScanXamlRootForCommandBars(knownCommandBar);
        return;
    }

    auto focused = mux::Input::FocusManager::GetFocusedElement();
    auto element = focused ? focused.try_as<mux::UIElement>() : nullptr;
    if (!element) {
        Wh_Log(L"No XAML element to start from on thread %u",
               GetCurrentThreadId());
        return;
    }

    ScanXamlRootForCommandBars(element);
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Same as above, deferred to after the current layout pass.
void ScheduleCurrentThreadScan() try {
    if (g_unloading) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }

    dispatcherQueue.TryEnqueue([]() { ScanCurrentThreadForCommandBars(); });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks of File Explorer's WinUI 3 code.

// void CommandBarManager::CommandBar(muxc::CommandBar const& value)
//
// Called with the command bar element itself, which is the most direct way to
// get hold of it.
using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original;
void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    Wh_Log(L">");

    CommandBarManager_CommandBar_Original(pThis, commandBar);

    if (g_unloading || !commandBar) {
        return;
    }

    try {
        auto const& element =
            *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!element) {
            return;
        }

        Wh_Log(L"Command bar %s, thread %u", element.Name().c_str(),
               GetCurrentThreadId());
        OnCommandBarAdded(element);

        // The secondary command bar has no manager of its own, and the bar
        // isn't necessarily attached to the tree yet, so also scan the island
        // once the current layout pass is done.
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

// void CommandBarControl::OnApplyTemplate()
//
// Runs when the control which hosts the command bar builds its contents, both
// for new windows and tabs and when Explorer rebuilds it.
using CommandBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
CommandBarControl_OnApplyTemplate_t CommandBarControl_OnApplyTemplate_Original;
CommandBarControl_OnApplyTemplate_t
    CommandBarControl_Wave1_OnApplyTemplate_Original;

void WINAPI CommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    CommandBarControl_OnApplyTemplate_Original(pThis);
    ScheduleCurrentThreadScan();
}

void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    ScheduleCurrentThreadScan();
}

// void CommandBarControl::CommandBarControlGotFocusHandler(
//     IInspectable const& sender, RoutedEventArgs const&)
//
// A cheap extra chance to pick up a command bar we haven't seen yet, e.g. in a
// window which was already open when the mod was loaded.
using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
CommandBarControl_GotFocusHandler_t
    CommandBarControl_GotFocusHandler_Original;
CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original;

void HandleCommandBarControlGotFocus(void* sender) {
    if (g_unloading || !sender) {
        return;
    }

    try {
        auto const& inspectable = *reinterpret_cast<wf::IInspectable const*>(
            sender);
        if (auto element = inspectable ? inspectable.try_as<mux::UIElement>()
                                       : nullptr) {
            ScanXamlRootForCommandBars(element);
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis,
                                                   void* sender,
                                                   void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis,
                                                         void* sender,
                                                         void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

std::atomic<bool> g_symbolsHooked;

bool HookFileExplorerExtensionsSymbols(HMODULE module) {
    // All hooks are optional, since the set of functions differs between
    // Windows builds, but at least one of the discovery hooks must be found.
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const & __ptr64) __ptr64)",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!HookSymbols(module, fileExplorerExtensionsDllHooks,
                     ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    if (!CommandBarManager_CommandBar_Original &&
        !CommandBarControl_OnApplyTemplate_Original &&
        !CommandBarControl_Wave1_OnApplyTemplate_Original) {
        Wh_Log(L"No command bar symbol was found");
        return false;
    }

    return true;
}

HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandle(L"FileExplorerExtensions.dll");
}

// Returns false only if the module is loaded but hooking it failed.
bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_symbolsHooked) {
        return true;
    }

    HMODULE module = GetFileExplorerExtensionsModuleHandle();
    if (!module) {
        return true;
    }

    if (g_symbolsHooked.exchange(true)) {
        return true;
    }

    Wh_Log(L"Hooking FileExplorerExtensions.dll");

    if (!HookFileExplorerExtensionsSymbols(module)) {
        // Let a later attempt (e.g. the next LoadLibraryExW) try again instead
        // of leaving the mod permanently disabled for this process.
        g_symbolsHooked = false;
        return false;
    }

    if (applyHooks) {
        Wh_ApplyHookOperations();
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_unloading) {
        HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Initialization plumbing.

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
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

std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& hWnds = *(std::vector<HWND>*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                hWnds.push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&hWnds);

    return hWnds;
}

// Levels: command bar items (0) -> menu items (1) -> submenu items (2).
constexpr int kMaxMenuDepth = 2;

ActionItem LoadActionItem(PCWSTR prefix, int depth, bool* isEmpty) {
    auto name = WindhawkUtils::StringSetting::make(L"%s.name", prefix);
    auto command = WindhawkUtils::StringSetting::make(L"%s.command", prefix);
    auto parameters =
        WindhawkUtils::StringSetting::make(L"%s.parameters", prefix);
    auto iconGlyph =
        WindhawkUtils::StringSetting::make(L"%s.iconGlyph", prefix);

    ActionItem item;
    item.name = name.get();
    item.command = command.get();
    item.parameters = parameters.get();
    item.icon = iconGlyph.get();
    item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
    item.separatorAfter =
        Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;

    // The deepest level can't hold a submenu, so it declares no type at all.
    if (depth < kMaxMenuDepth) {
        auto type = WindhawkUtils::StringSetting::make(L"%s.type", prefix);
        item.isMenu = wcscmp(type.get(), L"menu") == 0;

        for (int i = 0; i < 100; i++) {
            WCHAR subPrefix[256];
            swprintf(subPrefix, ARRAYSIZE(subPrefix), L"%s.subItems[%d]",
                     prefix, i);

            bool subEmpty = false;
            ActionItem subItem = LoadActionItem(subPrefix, depth + 1,
                                                &subEmpty);
            if (subEmpty) {
                break;
            }

            item.subItems.push_back(std::move(subItem));
        }
    }

    *isEmpty =
        item.name.empty() && item.command.empty() && item.subItems.empty();
    return item;
}

void LoadSettings() {
    {
        // The items, and with them the icons they ask for, may have changed.
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);

    g_settings.openMenuOnHover = Wh_GetIntSetting(L"openMenuOnHover") != 0;

    int menuHoverDelay = Wh_GetIntSetting(L"menuHoverDelay");
    g_settings.menuHoverDelay = menuHoverDelay >= 0 ? menuHoverDelay : 0;

    for (int i = 0; i < kDefaultButtonCount; i++) {
        g_settings.hideDefaultButtons[i] =
            Wh_GetIntSetting(L"hideDefaultButtons.%s",
                             kDefaultButtons[i].settingKey) != 0;
    }

    g_settings.hideMoreButton =
        Wh_GetIntSetting(L"hideDefaultButtons.moreOptions") != 0;
    g_settings.hideDetailsButton =
        Wh_GetIntSetting(L"hideDefaultButtons.details") != 0;

    memset(g_settings.hideSeparatorAfterButton, 0,
           sizeof(g_settings.hideSeparatorAfterButton));
    g_settings.hideSeparatorAfterButton[kNewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterNew") != 0;
    g_settings.hideSeparatorAfterButton[kDeleteButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterDelete") != 0;
    g_settings.hideSeparatorAfterButton[kViewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterView") != 0;

    int itemSpacing = Wh_GetIntSetting(L"itemSpacing");
    g_settings.itemSpacing = itemSpacing < 0 ? -1 : itemSpacing;

    g_settings.items.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR prefix[64];
        swprintf(prefix, ARRAYSIZE(prefix), L"items[%d]", i);

        bool isEmpty = false;
        ActionItem item = LoadActionItem(prefix, 0, &isEmpty);
        if (isEmpty) {
            break;
        }

        g_settings.items.push_back(std::move(item));
    }

    // No fallback item if the list is empty: using the mod only to hide
    // built-in buttons is a supported configuration.
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/false)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW) {
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);

    // Windows which were already open when the mod was loaded won't
    // necessarily rebuild their command bar, so look for it explicitly.
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { ScanCurrentThreadForCommandBars(); }, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   (DWORD)(ULONG_PTR)hWnd);
        }
    }

    // Anything left here belongs to a thread which couldn't be reached above.
    // Its watchers and timers can only be released from that thread, so all
    // that's left to do is to drop the records.
    {
        std::lock_guard<std::mutex> lock(g_managedElementsMutex);
        if (!g_managedElements.empty()) {
            Wh_Log(L"%zu elements couldn't be restored",
                   g_managedElements.size());
        }

        g_managedElements.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_hoverTimersMutex);
        g_hoverTimers.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_pendingUpdatesMutex);
        g_pendingUpdates.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        g_entries.clear();
    }

    // Last, since the launch threads run our code: the DLL can't be unmapped
    // while one of them is still working.
    WaitForLaunchThreads();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { RefreshButtonsForCurrentThread(); }, nullptr);
    }
}
