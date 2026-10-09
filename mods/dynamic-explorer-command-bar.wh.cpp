// ==WindhawkMod==
// @id              dynamic-explorer-command-bar
// @name            Dynamic Explorer Command Bar
// @description     Add custom buttons and menus to the Windows 11 File Explorer command bar with dynamic file-type filtering, tokens, and secondary bar docking
// @version         1.0.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Dynamic Explorer Command Bar

Customize, automate, and extend the **Windows 11 File Explorer command bar** with your own buttons, dropdown menus, smart argument tokens, and dynamic extension-based visibility rules.

![Dynamic Command Bar Demo](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/preview-command-bar.gif)

Designed for Windows 11 with the WinAppSDK (WinUI 3) File Explorer.

---

## Key Features

* **Custom Buttons & Dropdown Menus** — Add single-click toolbar buttons or nested dropdown menus (up to three levels deep) running commands, scripts, or internal shell actions.
* **Context-Aware Dynamic Visibility** — Automatically show or hide individual buttons depending on the file extensions or item types highlighted in Explorer.
* **Flexible Inactive State** — Choose between completely hiding (`Collapsed`) or graying out (`Disabled`) buttons when selection conditions are not met.
* **Advanced Command Tokens** — Pass active folder paths, file names, counts, extensions, parent directories, or selected file lists straight into your apps.
* **Batch Spawning (`%sel_each%`)** — Automatically launch an application instance for each highlighted item with built-in safety throttling.
* **Secondary Command Bar Placement** — Dock custom actions on the right side of the command bar (next to the Details/Preview pane toggles) for a clean dual-toolbar layout.
* **Theme-Aware Icons** — Support fluent Segoe glyphs, local `.exe` / `.dll` / `.ico` paths, Store apps (`shell:AppsFolder\...`), and dual Light/Dark icons via `LightPath | DarkPath` syntax.
* **PowerToys New+ Emulation** — Replace the native New button with an instant New+ template generator.
* **Integrated Shell Context Menu** — Append a dedicated toolbar button that triggers the full native Windows context menu (with full Nilesoft Shell support).
* **Custom Toolbar Spacing & Open on Hover** — Adjust pixel spacing between buttons or open dropdown menus smoothly on cursor hover.

---

## Command Parameters & Dynamic Tokens

The mod supports rich parameter expansions that dynamically resolve the state of the active File Explorer tab:

| Token | Description | Example Output |
| :--- | :--- | :--- |
| `%path%` | Full path of the currently active folder | `C:\Projects\App` |
| `%sel%` | All selected items as space-separated quoted paths | `"C:\a.txt" "C:\b.png"` |
| `%sel_each%` | Executes a distinct process instance per selected item (capped at 20) | `"C:\Item.txt"` |
| `%files` | Space-separated quoted paths of selected regular files only | `"C:\doc.txt"` |
| `%folders` | Space-separated quoted paths of selected directories only | `"C:\FolderA"` |
| `%n` | Selected file names only (without absolute directory path) | `"Notes.txt"` |
| `%ext` | File extension of the first selected item | `.png` |
| `%c` | Total number of selected items | `3` |
| `%d_name` | Name of the active folder only | `App` |
| `%p` | Parent folder path of the active tab (falls back cleanly at drive root) | `C:\Projects` |

Wrap placeholders in double quotes when necessary (e.g., `-d "%path%"` or `code.exe "%sel%"`).

---

## Dynamic File Extension Filtering

The **File extension filter** (`matchExtensions`) field lets you display toolbar actions contextually:

* **Leave blank or `*`** — Always show the button regardless of selection.
* **Whitelist specific extensions** — e.g. `txt, js, cpp` (Button only appears when selected items match these formats).
* **Exclude specific extensions** — e.g. `*,-exe,-zip` (Shows for all files except executables and archives).
* **Target files only** — e.g. `files` or `*,-folder` (Hides button whenever a folder is selected).
* **Target folders only** — e.g. `folder` or `dir` (Button appears exclusively when directories are selected).

---

## Built-in Shell Actions

Instead of an executable path, you can set **Command** to one of these built-in actions:
* `internal:OpenWith` — Opens Windows native Open With dialog for the active item.
* `internal:FolderOptions` — Opens Classic Folder Options dialog.
* `internal:TogglePreview` — Toggles the File Explorer Preview pane.
* `internal:ToggleDetails` — Toggles the File Explorer Details pane.

---

### Comparison with Existing Mods

* **vs. `explorer-command-bar` (DanRotaru)**: 
  The original mod provides static command buttons and hides built-in buttons. **Dynamic Explorer Command Bar** extends this with real-time selection-based extension filtering, per-item execution (`%sel_each%`), secondary toolbar placement, dual Light/Dark icons, and an expanded token engine (`%files`, `%folders`, `%n`, `%ext`, `%c`, `%d_name`, `%p`).
* **vs. `explorer-custom-shortcuts` (ArvindSaini978)**:
  While *Explorer Custom Shortcuts* provides background keyboard hotkeys, this mod brings that dynamic parameter evaluation into the visual Windows 11 UI toolbar with clickable actions and dropdown flyouts.
* **vs. `explorer-folder-bookmarks-bar` (Maxim Fomin)**:
  *Explorer Folder Bookmarks Bar* adds a dedicated horizontal folder shortcut strip. This mod focuses on customizable action buttons, command execution, and deep integration into the native command bar.

---

### Attribution & Acknowledgments

* Based on and extended from **Explorer Command Bar** by DanRotaru (MIT License), which provides the foundational XAML visual tree hooks and command bar injection architecture.
* Parameter token expansions and batch-launch concepts adapted from **Explorer Custom Shortcuts** by ArvindSaini978.
* Window thread dispatch and refresh helper logic adapted from **Explorer Folder Bookmarks Bar** by Maxim Fomin.
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
    - name: Folder Options
      $name: Name
      $description: Shown as the button tooltip.
    - command: internal:FolderOptions
      $name: Command
      $description: >-
        The executable to run, or an internal command (internal:TogglePreview, internal:ToggleDetails, internal:FolderOptions, internal:OpenWith).
    - parameters: ""
      $name: Parameters
      $description: >-
        Command line arguments. Supports tokens: %path% (current folder), %sel% (selected items), %sel_each% (run app per item), %files (files only), %folders (folders only), %n (file names only), %ext (extension), %c (count), %d_name (folder name), and %p (parent path).
    - matchExtensions: ""
      $name: "File extension filter"
      $description: >-
        Comma-separated list of extensions (e.g. txt, js, cpp). 
        Prefix with '-' to exclude (e.g. -exe, -zip). 
        Prefix with '*' or leave blank to show for all files.
    - iconGlyph: E713
      $name: Icon glyph or icon path
      $description: >-
        Fluent icon glyph (e.g. E713), file path (.ico/.dll/.exe), or empty for executable icon. 
        Separate with | for theme-aware icons (Light | Dark, or | Dark).
    - hideIcon: false
      $name: Hide icon
      $description: Don't show an icon for this item.
    - showLabel: false
      $name: Show label
      $description: Show text label next to the icon.
    - labelText: ""
      $name: Custom Label Text
      $description: Text displayed next to the icon. If empty, the Name field is used.
    - separatorAfter: true
      $name: Vertical separator after
      $description: Show a vertical separator line after this button.
    - subItems:
      - - type: button
          $name: Sub-item Type
          $options:
          - button: Single button
          - menu: Submenu
        - name: ""
          $name: Name
        - command: ""
          $name: Command
        - parameters: ""
          $name: Parameters
        - iconGlyph: ""
          $name: Icon glyph or icon path
        - hideIcon: false
          $name: Hide icon
        - separatorAfter: false
          $name: Vertical separator after
        - subItems:
          - - name: ""
              $name: Name
            - command: ""
              $name: Command
            - parameters: ""
              $name: Parameters
            - iconGlyph: ""
              $name: Icon glyph or icon path
            - hideIcon: false
              $name: Hide icon
            - separatorAfter: false
              $name: Vertical separator after
          $name: Nested items
          $description: Items inside this nested submenu.
      $name: Menu items
      $description: Items shown in the dropdown menu.
  - - type: button
    - name: Open Terminal Here
    - command: wt.exe
    - parameters: -d "%path%"
    - matchExtensions: ""
    - iconGlyph: ""
    - hideIcon: false
    - showLabel: false
    - labelText: ""
    - separatorAfter: false
  - - type: button
    - name: Open With
    - command: internal:OpenWith
    - parameters: ''
    - matchExtensions: "files"
    - iconGlyph: 'E7AC'
    - hideIcon: true
    - showLabel: true
    - labelText: "Open With"
    - separatorAfter: false
  - - type: button
    - name: Open in Notepad
    - command: notepad.exe
    - parameters: '%sel_each%'
    - matchExtensions: "files"
    - iconGlyph: 'shell:AppsFolder\Microsoft.WindowsNotepad_8wekyb3d8bbwe!App'
    - hideIcon: true
    - showLabel: true
    - labelText: "Notepad"
    - separatorAfter: false
  - - type: menu
    - name: Additional
    - command: ""
    - parameters: ""
    - matchExtensions: ""
    - iconGlyph: E81E
    - hideIcon: false
    - showLabel: false
    - labelText: ""
    - separatorAfter: false
    - subItems:
      - - type: button
        - name: Open in VS Code
        - command: code.exe
        - parameters: '%sel%'
        - iconGlyph: ""
        - separatorAfter: false
        - subItems:
          - - name: ""
            - command: ""
            - parameters: ""
            - iconGlyph: ""
            - hideIcon: false
            - separatorAfter: false
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
        - name: Commands
        - command: ""
        - parameters: ""
        - iconGlyph: EC7A
        - separatorAfter: false
        - subItems:
          - - name: vite
            - command: cmd.exe
            - parameters: /k vite
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: true
          - - name: npm init
            - command: cmd.exe
            - parameters: /k npm init
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm install
            - command: cmd.exe
            - parameters: /k npm install
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run dev
            - command: cmd.exe
            - parameters: /k npm run dev
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run build
            - command: cmd.exe
            - parameters: /k npm run build
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run start
            - command: cmd.exe
            - parameters: /k npm run start
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
      - - type: menu
        - name: AI
        - command: ""
        - parameters: ""
        - iconGlyph: E794
        - separatorAfter: false
        - subItems:
          - - name: Claude
            - command: cmd.exe
            - parameters: /k claude
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: Codex
            - command: cmd.exe
            - parameters: /k codex
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
  $name: Toolbar items
  $description: Custom buttons and dropdown menus added to the command bar.
- placeOnSecondaryBar: true
  $name: Place buttons on the right side
  $description: >-
    Show custom buttons on the right side of File Explorer (next to Details
    and Preview pane toggles) instead of the main left toolbar.
- disabledInsteadOfHidden: false
  $name: Disable buttons instead of hiding them
  $description: Gray out inactive buttons instead of removing them from the toolbar.
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
  - rotateLeft: false
    $name: Rotate left
  - rotateRight: false
    $name: Rotate right
  - extractAll: false
    $name: Extract all
  - details: false
    $name: Details pane toggle
  $name: Hide default toolbar buttons
- newPlus:
  - enabled: false
    $name: Replace New with New+
  - showLabel: true
    $name: Show the button label
  - buttonLabel: New
    $name: Button label
  - buttonIcon: ""
    $name: Button icon glyph or icon path
  - templateFolder: ""
    $name: Templates folder
  - showIcons: true
    $name: Show template icons
  - showTemplatesFolderItem: true
    $name: Add an "Open templates folder" entry
  - keepOriginalNewButton: false
    $name: Keep Explorer's New button
  $name: Replace New to New+
- contextMenuItem:
  - enabled: false
    $name: Add the context menu item
  - useNilesoftShell: false
    $name: Let File Explorer show the menu (for Nilesoft Shell)
  - showLabel: false
    $name: Show the item label
  - buttonLabel: Context menu
    $name: Item label
  - buttonIcon: E8FD
    $name: Item icon glyph or icon path
  $name: Context menu item
- openMenuOnHover: false
  $name: Open menus on hover
- menuHoverDelay: 400
  $name: Hover delay (milliseconds)
- itemSpacing: -1
  $name: Item spacing (pixels at 100% scaling)
*/
// ==/WindhawkModSettings==

// clang-format on

#include <commctrl.h>
#include <windhawk_utils.h>
#include <windows.h>

#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>
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
    std::wstring labelText;
    std::wstring command;
    std::wstring parameters;
    std::wstring matchExtensions;
    std::wstring icon;
    bool hideIcon = false;
    bool showLabel = false;
    bool isMenu = false;
    bool separatorAfter = false;
    std::vector<ActionItem> subItems;
};

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

struct NewPlusSettings {
    bool enabled = false;
    bool showLabel = true;
    std::wstring buttonLabel = L"New";
    std::wstring buttonIcon;
    std::wstring templateFolder;
    bool showIcons = true;
    bool showTemplatesFolderItem = true;
    bool keepOriginalNewButton = false;
};

struct ContextMenuItemSettings {
    bool enabled = false;
    bool useNilesoftShell = false;
    bool showLabel = false;
    std::wstring buttonLabel = L"Context menu";
    std::wstring buttonIcon;
};

struct {
    std::mutex mutex;
    bool placeOnSecondaryBar = true;
    bool disabledInsteadOfHidden = false;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideSeparatorAfterButton[kDefaultButtonCount] = {};
    bool hideMoreButton = false;
    bool hideDetailsButton = false;
    int itemSpacing = -1;
    std::vector<ActionItem> items;
    NewPlusSettings newPlus;
    ContextMenuItemSettings contextMenuItem;
} g_settings;

// clang-format off

#pragma region winrt_hpp
#include <Unknwn.h>
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

namespace wdj = winrt::Windows::Data::Json;
namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxd = winrt::Microsoft::UI::Xaml::Documents;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
#pragma endregion

// clang-format on

constexpr PCWSTR kButtonNamePrefix = L"WindhawkActionButton";
constexpr PCWSTR kNewPlusButtonName = L"WindhawkNewPlusButton";
constexpr PCWSTR kContextMenuButtonName = L"WindhawkContextMenuButton";

struct CommandBarEntry {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token vectorChangedToken{};
};

thread_local std::vector<CommandBarEntry> g_entries;
thread_local bool g_threadScanned;

struct TrackedRevoker {
    winrt::weak_ref<wf::IInspectable> source;
    std::function<void()> revoke;
};

thread_local std::vector<TrackedRevoker> g_revokers;
constexpr size_t kRevokersPruneMin = 64;
thread_local size_t g_revokersPruneAt = kRevokersPruneMin;

template <typename T, typename Revoker>
void TrackRevoker(T const& source, Revoker&& revoker) {
    if (g_revokers.size() >= g_revokersPruneAt) {
        std::erase_if(g_revokers, [](TrackedRevoker const& tracked) {
            return !tracked.source.get();
        });
        g_revokersPruneAt = std::max(kRevokersPruneMin, g_revokers.size() * 2);
    }

    auto held =
        std::make_shared<std::decay_t<Revoker>>(std::forward<Revoker>(revoker));
    g_revokers.push_back({winrt::weak_ref<wf::IInspectable>{source},
                          [held]() { held->revoke(); }});
}

void RevokeHandlersForCurrentThread() {
    std::vector<TrackedRevoker> taken;
    taken.swap(g_revokers);
    g_revokersPruneAt = kRevokersPruneMin;

    for (auto const& tracked : taken) {
        try {
            tracked.revoke();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

struct ManagedElement {
    winrt::weak_ref<mux::UIElement> element;
    bool hasOriginalVisibility = false;
    mux::Visibility originalVisibility = mux::Visibility::Visible;
    bool hasOriginalSpacing = false;
    mux::Thickness originalMargin{};
    double originalMinWidth = 0;
    bool hasOriginalOverflow = false;
    muxc::CommandBarOverflowButtonVisibility originalOverflow =
        muxc::CommandBarOverflowButtonVisibility::Auto;
    bool watched = false;
    int64_t visibilityToken = 0;
};

thread_local std::vector<ManagedElement> g_managedElements;

ManagedElement* FindManagedElement(mux::UIElement const& element) {
    for (auto& entry : g_managedElements) {
        if (entry.element.get() == element) {
            return &entry;
        }
    }
    return nullptr;
}

ManagedElement& GetManagedElement(mux::UIElement const& element) {
    for (auto it = g_managedElements.begin(); it != g_managedElements.end();) {
        if (!it->element.get()) {
            it = g_managedElements.erase(it);
        } else {
            ++it;
        }
    }

    if (auto* entry = FindManagedElement(element)) {
        return *entry;
    }

    g_managedElements.push_back({winrt::make_weak(element)});
    return g_managedElements.back();
}

std::wstring ExpandEnvVars(std::wstring const& str) {
    if (str.empty())
        return str;
    WCHAR buffer[MAX_PATH * 2];
    DWORD length =
        ExpandEnvironmentStringsW(str.c_str(), buffer, ARRAYSIZE(buffer));
    if (length == 0 || length > ARRAYSIZE(buffer))
        return str;
    return buffer;
}

std::wstring ToLower(std::wstring str) {
    for (auto& c : str)
        c = towlower(c);
    return str;
}

std::wstring TrimQuotesAndSpaces(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return std::wstring();
    size_t last = str.find_last_not_of(L" \t");
    str = str.substr(first, last - first + 1);
    if (str.size() >= 2 && str.front() == L'"' && str.back() == L'"') {
        str = str.substr(1, str.size() - 2);
    }
    return str;
}

std::wstring JoinPath(std::wstring const& folder, std::wstring const& name) {
    std::wstring result = folder;
    if (!result.empty() && result.back() != L'\\' && result.back() != L'/') {
        result += L'\\';
    }
    return result + name;
}

bool DirectoryExists(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ResolveCommandPath(std::wstring const& command) {
    std::wstring expanded = ExpandEnvVars(command);
    if (expanded.find(L'\\') != std::wstring::npos)
        return expanded;

    WCHAR resolved[MAX_PATH];
    if (SearchPathW(nullptr, expanded.c_str(), L".exe", ARRAYSIZE(resolved),
                    resolved, nullptr)) {
        return resolved;
    }

    std::wstring keyPath =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + expanded;
    if (!ToLower(expanded).ends_with(L".exe"))
        keyPath += L".exe";

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, buffer, &size) == ERROR_SUCCESS &&
            buffer[0]) {
            std::wstring appPath = ExpandEnvVars(buffer);
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }
            return appPath;
        }
    }

    return expanded;
}

class ComApartmentScope {
   public:
    explicit ComApartmentScope(DWORD coInit)
        : m_hr(CoInitializeEx(nullptr, coInit)) {}
    ~ComApartmentScope() {
        if (SUCCEEDED(m_hr))
            CoUninitialize();
    }
    ComApartmentScope(const ComApartmentScope&) = delete;
    ComApartmentScope& operator=(const ComApartmentScope&) = delete;

   private:
    HRESULT m_hr;
};

struct ExplorerContext {
    std::wstring folderPath;
    std::wstring selectedPath;
    std::vector<std::wstring> allSelectedPaths;
    winrt::com_ptr<IShellView> shellView;
};

// ============================================================================
// Shell View & Selection Inspection (Direct Focus Traversal & CF_HDROP)
// ============================================================================

winrt::com_ptr<IShellView> GetActiveShellViewFromFocus(HWND hExplorerWnd,
                                                       HWND hCapturedFocus) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put()))) ||
        !shellWindows) {
        return nullptr;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    HWND hActiveTabWnd = nullptr;
    if (hExplorerWnd) {
        DWORD threadId = GetWindowThreadProcessId(hExplorerWnd, nullptr);
        GUITHREADINFO gti = {sizeof(gti)};
        HWND hFocus = hCapturedFocus
                          ? hCapturedFocus
                          : ((GetGUIThreadInfo(threadId, &gti) && gti.hwndFocus)
                                 ? gti.hwndFocus
                                 : nullptr);

        for (HWND h = hFocus; h && h != hExplorerWnd; h = GetParent(h)) {
            WCHAR cls[64];
            if (GetClassNameW(h, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, L"ShellTabWindowClass") == 0) {
                hActiveTabWnd = h;
                break;
            }
        }
        if (!hActiveTabWnd) {
            hActiveTabWnd = FindWindowExW(hExplorerWnd, nullptr,
                                          L"ShellTabWindowClass", nullptr);
        }
    }

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch)
            continue;

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser)
            continue;

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw)))
            continue;

        HWND hBrowserRoot = GetAncestor((HWND)hWndRaw, GA_ROOT);
        HWND hTargetRoot = GetAncestor(hExplorerWnd, GA_ROOT);
        if (hExplorerWnd && hBrowserRoot != hTargetRoot)
            continue;

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider)
            continue;

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser)
            continue;

        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd && hTabWnd != hActiveTabWnd)
                continue;
        }

        IShellView* pShellView = nullptr;
        if (SUCCEEDED(shellBrowser->QueryActiveShellView(&pShellView)) &&
            pShellView) {
            winrt::com_ptr<IShellView> sv;
            sv.attach(pShellView);
            return sv;
        }
    }

    return nullptr;
}

winrt::com_ptr<IShellView> GetActiveShellView(HWND hExplorerWnd) {
    return GetActiveShellViewFromFocus(GetAncestor(hExplorerWnd, GA_ROOT),
                                       GetFocus());
}

bool ShellViewHasSelection(winrt::com_ptr<IShellView> const& shellView) {
    auto folderView = shellView.try_as<IFolderView>();
    if (!folderView)
        return false;
    int count = 0;
    return SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &count)) &&
           count > 0;
}

struct FindWindowByClassParam {
    PCWSTR className;
    HWND result;
};

HWND FindDescendantWindow(HWND hParentWnd, PCWSTR className) {
    FindWindowByClassParam param{className, nullptr};
    EnumChildWindows(
        hParentWnd,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& p = *(FindWindowByClassParam*)lParam;
            WCHAR buffer[64];
            if (GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer)) &&
                _wcsicmp(buffer, p.className) == 0) {
                p.result = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&param);
    return param.result;
}

HWND FindShellViewWindow(HWND hExplorerWnd) {
    HWND hTabWnd =
        FindWindowExW(hExplorerWnd, nullptr, L"ShellTabWindowClass", nullptr);
    if (hTabWnd) {
        if (HWND hViewWnd = FindDescendantWindow(hTabWnd, L"SHELLDLL_DefView"))
            return hViewWnd;
    }
    return FindDescendantWindow(hExplorerWnd, L"SHELLDLL_DefView");
}

std::vector<std::wstring> GetSelectedPathsHDROP(IShellView* psv) {
    std::vector<std::wstring> files;
    if (!psv)
        return files;

    winrt::com_ptr<IDataObject> pdo;
    if (SUCCEEDED(
            psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(pdo.put()))) &&
        pdo) {
        FORMATETC fmt = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                         TYMED_HGLOBAL};
        STGMEDIUM stg = {0};
        if (SUCCEEDED(pdo->GetData(&fmt, &stg))) {
            HDROP hDrop = (HDROP)stg.hGlobal;
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT j = 0; j < fileCount; j++) {
                UINT requiredLen = DragQueryFileW(hDrop, j, nullptr, 0);
                if (requiredLen > 0) {
                    std::vector<WCHAR> pathBuf(requiredLen + 1);
                    if (DragQueryFileW(hDrop, j, pathBuf.data(),
                                       requiredLen + 1)) {
                        files.push_back(pathBuf.data());
                    }
                }
            }
            ReleaseStgMedium(&stg);
        }
    }
    return files;
}

ExplorerContext GetExplorerContext(HWND hExplorerWnd) {
    ExplorerContext result;
    if (!hExplorerWnd)
        return result;

    HWND hRoot = GetAncestor(hExplorerWnd, GA_ROOT);
    result.shellView = GetActiveShellViewFromFocus(hRoot, GetFocus());
    if (!result.shellView)
        return result;

    result.allSelectedPaths = GetSelectedPathsHDROP(result.shellView.get());
    if (!result.allSelectedPaths.empty()) {
        result.selectedPath = result.allSelectedPaths[0];
    }

    if (auto folderView = result.shellView.try_as<IFolderView>()) {
        winrt::com_ptr<IPersistFolder2> persistFolder;
        LPITEMIDLIST pidl = nullptr;
        if (SUCCEEDED(
                folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) &&
            persistFolder && SUCCEEDED(persistFolder->GetCurFolder(&pidl)) &&
            pidl) {
            WCHAR path[MAX_PATH];
            if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                      GPFIDL_DEFAULT)) {
                result.folderPath = path;
            }
            CoTaskMemFree(pidl);
        }
    }

    return result;
}

bool IsExplorerFrame(HWND window) {
    WCHAR className[64]{};
    return window && GetClassNameW(window, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CabinetWClass") == 0;
}

HWND GetExplorerWindowForElement(mux::FrameworkElement const& element) {
    try {
        if (auto root = element.XamlRoot()) {
            if (auto environment = root.ContentIslandEnvironment()) {
                HWND window =
                    GetAncestor(reinterpret_cast<HWND>(static_cast<uintptr_t>(
                                    environment.AppWindowId().Value)),
                                GA_ROOT);
                if (IsExplorerFrame(window))
                    return window;
            }
        }
    } catch (...) {
    }

    HWND window = GetActiveWindow();
    window = window ? GetAncestor(window, GA_ROOT) : nullptr;
    return IsExplorerFrame(window) ? window : nullptr;
}

// ============================================================================
// Shell Context Menu Handlers
// ============================================================================

bool RequestShellViewContextMenu(HWND hExplorerWnd) {
    HWND hViewWnd = FindShellViewWindow(hExplorerWnd);
    if (!hViewWnd) {
        Wh_Log(L"No shell view window for %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return false;
    }
    return PostMessageW(hViewWnd, WM_CONTEXTMENU, (WPARAM)hViewWnd,
                        (LPARAM)-1) != FALSE;
}

winrt::com_ptr<IContextMenu> GetShellContextMenu(HWND hExplorerWnd,
                                                 bool* isItemMenu) {
    *isItemMenu = false;
    auto shellView = GetActiveShellView(hExplorerWnd);
    if (!shellView) {
        Wh_Log(L"No shell view for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return nullptr;
    }

    *isItemMenu = ShellViewHasSelection(shellView);
    UINT viewObject = *isItemMenu ? SVGIO_SELECTION : SVGIO_BACKGROUND;

    winrt::com_ptr<IContextMenu> contextMenu;
    HRESULT hr = shellView->GetItemObject(viewObject, __uuidof(IContextMenu),
                                          contextMenu.put_void());
    if (FAILED(hr) || !contextMenu) {
        Wh_Log(L"GetItemObject(%u) failed: %08X", viewObject, hr);
        return nullptr;
    }
    return contextMenu;
}

constexpr UINT kContextMenuFirstCmdId = 1;
constexpr UINT kContextMenuLastCmdId = 0x7FFF;
thread_local IContextMenu2* g_trackedContextMenu2;
thread_local IContextMenu3* g_trackedContextMenu3;
thread_local bool g_contextMenuIsOpen;
std::atomic<int> g_openContextMenuCount;

struct OpenContextMenuScope {
    OpenContextMenuScope() { g_openContextMenuCount++; }
    ~OpenContextMenuScope() { g_openContextMenuCount--; }
    OpenContextMenuScope(const OpenContextMenuScope&) = delete;
    OpenContextMenuScope& operator=(const OpenContextMenuScope&) = delete;
};

LRESULT CALLBACK ContextMenuOwnerWndProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam) {
    switch (uMsg) {
        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            if (g_trackedContextMenu3) {
                LRESULT result = 0;
                if (SUCCEEDED(g_trackedContextMenu3->HandleMenuMsg2(
                        uMsg, wParam, lParam, &result)))
                    return result;
            } else if (g_trackedContextMenu2 &&
                       SUCCEEDED(g_trackedContextMenu2->HandleMenuMsg(
                           uMsg, wParam, lParam))) {
                return uMsg == WM_INITMENUPOPUP ? 0 : TRUE;
            }
            break;
        case WM_MENUCHAR:
            if (g_trackedContextMenu3) {
                LRESULT result = 0;
                if (SUCCEEDED(g_trackedContextMenu3->HandleMenuMsg2(
                        uMsg, wParam, lParam, &result)) &&
                    result)
                    return result;
            }
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

std::wstring ContextMenuOwnerClassName() {
    return std::wstring(L"WindhawkContextMenuOwner_") + WH_MOD_ID;
}

std::atomic<bool> g_contextMenuOwnerClassRegistered;
std::mutex g_contextMenuOwnersMutex;
std::unordered_map<DWORD, HWND> g_contextMenuOwners;

bool IsContextMenuOwnerWindow(DWORD threadId, HWND hWnd) {
    if (!IsWindow(hWnd) || GetWindowThreadProcessId(hWnd, nullptr) != threadId)
        return false;
    WCHAR className[128];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, ContextMenuOwnerClassName().c_str()) == 0;
}

void RegisterContextMenuOwnerClass() {
    std::wstring className = ContextMenuOwnerClassName();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ContextMenuOwnerWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = className.c_str();
    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        Wh_Log(L"RegisterClassEx failed: %u%s", error,
               error == ERROR_CLASS_ALREADY_EXISTS
                   ? L" - context menu class already exists"
                   : L"");
        return;
    }
    g_contextMenuOwnerClassRegistered = true;
}

HWND EnsureContextMenuOwnerWindow() {
    DWORD threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
        auto it = g_contextMenuOwners.find(threadId);
        if (it != g_contextMenuOwners.end() &&
            IsContextMenuOwnerWindow(threadId, it->second)) {
            return it->second;
        }
        if (it != g_contextMenuOwners.end())
            g_contextMenuOwners.erase(it);
    }

    if (!g_contextMenuOwnerClassRegistered)
        return nullptr;

    HWND hWnd = CreateWindowExW(0, ContextMenuOwnerClassName().c_str(), nullptr,
                                0, 0, 0, 0, 0, nullptr, nullptr,
                                GetCurrentModuleHandle(), nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
    g_contextMenuOwners[threadId] = hWnd;
    return hWnd;
}

void DismissOpenContextMenus() {
    // Allow a maximum timeout of 2 seconds (200 * 10ms) to prevent infinite
    // loops
    for (int i = 0; g_openContextMenuCount > 0 && i < 200; i++) {
        {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            for (auto const& [threadId, hWnd] : g_contextMenuOwners) {
                if (IsContextMenuOwnerWindow(threadId, hWnd))
                    PostMessageW(hWnd, WM_CANCELMODE, 0, 0);
            }
        }
        if (i > 0 && i % 100 == 0) {
            Wh_Log(L"Still waiting for %d shell context menu(s)",
                   g_openContextMenuCount.load());
        }
        Sleep(10);
    }
}

void DestroyContextMenuOwnerWindowForCurrentThread() {
    if (g_contextMenuIsOpen)
        return;
    HWND hWnd = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
        auto it = g_contextMenuOwners.find(GetCurrentThreadId());
        if (it == g_contextMenuOwners.end())
            return;
        hWnd = it->second;
        g_contextMenuOwners.erase(it);
    }
    DestroyWindow(hWnd);
}

void InvokeShellContextMenuCommand(
    winrt::com_ptr<IContextMenu> const& contextMenu,
    UINT cmdId,
    HWND hExplorerWnd,
    POINT point) {
    CMINVOKECOMMANDINFOEX info{};
    info.cbSize = sizeof(info);
    info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
    info.hwnd = hExplorerWnd;
    info.lpVerb = (LPCSTR)(UINT_PTR)(cmdId - kContextMenuFirstCmdId);
    info.lpVerbW = (LPCWSTR)(UINT_PTR)(cmdId - kContextMenuFirstCmdId);
    info.nShow = SW_SHOWNORMAL;
    info.ptInvoke = point;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        info.fMask |= CMIC_MASK_CONTROL_DOWN;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        info.fMask |= CMIC_MASK_SHIFT_DOWN;
    HRESULT hr = contextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
    if (FAILED(hr))
        Wh_Log(L"InvokeCommand failed: %08X", hr);
}

void ShowShellContextMenu(HWND hExplorerWnd, POINT point) {
    if (g_contextMenuIsOpen || g_unloading)
        return;
    OpenContextMenuScope openScope;

    bool useNilesoftShell;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        useNilesoftShell = g_settings.contextMenuItem.useNilesoftShell;
    }

    if (useNilesoftShell && RequestShellViewContextMenu(hExplorerWnd))
        return;

    HWND hOwnerWnd = EnsureContextMenuOwnerWindow();
    if (!hOwnerWnd)
        return;

    bool isItemMenu = false;
    auto contextMenu = GetShellContextMenu(hExplorerWnd, &isItemMenu);
    if (!contextMenu)
        return;

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;

    UINT flags = isItemMenu ? CMF_CANRENAME : CMF_NORMAL;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        flags |= CMF_EXTENDEDVERBS;

    HRESULT hr = contextMenu->QueryContextMenu(hMenu, 0, kContextMenuFirstCmdId,
                                               kContextMenuLastCmdId, flags);
    if (FAILED(hr)) {
        Wh_Log(L"QueryContextMenu failed: %08X", hr);
        DestroyMenu(hMenu);
        return;
    }

    if (g_unloading) {
        DestroyMenu(hMenu);
        return;
    }

    auto contextMenu2 = contextMenu.try_as<IContextMenu2>();
    auto contextMenu3 = contextMenu.try_as<IContextMenu3>();
    g_trackedContextMenu2 = contextMenu2.get();
    g_trackedContextMenu3 = contextMenu3.get();
    g_contextMenuIsOpen = true;

    UINT cmdId = (UINT)TrackPopupMenuEx(
        hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN,
        point.x, point.y, hOwnerWnd, nullptr);

    g_contextMenuIsOpen = false;
    g_trackedContextMenu2 = nullptr;
    g_trackedContextMenu3 = nullptr;

    if (cmdId >= kContextMenuFirstCmdId && cmdId <= kContextMenuLastCmdId &&
        !g_unloading) {
        InvokeShellContextMenuCommand(contextMenu, cmdId, hExplorerWnd, point);
    }

    DestroyMenu(hMenu);
}

// ============================================================================
// Parameter Placeholders & Process Execution
// ============================================================================

bool ReplacePlaceholder(std::wstring& parameters,
                        std::wstring_view placeholder,
                        std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos)
        return true;
    if (value.empty())
        return false;

    while (pos != std::wstring::npos) {
        std::wstring replacement = value;
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

std::wstring FormatSelectedItemsQuoted(std::vector<std::wstring> const& paths) {
    std::wstring formatted;
    for (auto const& p : paths) {
        std::wstring safe = p;
        if (!safe.empty() && safe.back() == L'\\' && safe.size() > 3)
            safe += L'\\';
        formatted += L"\"" + safe + L"\" ";
    }
    if (!formatted.empty())
        formatted.pop_back();
    return formatted;
}

std::wstring BuildParameters(std::wstring parameters,
                             ExplorerContext const& context) {
    if (parameters.empty())
        return parameters;

    // Partition selected paths into files and folders
    std::vector<std::wstring> filesOnly;
    std::vector<std::wstring> foldersOnly;
    for (const auto& item : context.allSelectedPaths) {
        DWORD attr = GetFileAttributesW(item.c_str());
        if (attr != INVALID_FILE_ATTRIBUTES &&
            (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            foldersOnly.push_back(item);
        } else {
            filesOnly.push_back(item);
        }
    }

    // 1. %files and %folders
    ReplacePlaceholder(parameters, L"%files",
                       FormatSelectedItemsQuoted(filesOnly));
    ReplacePlaceholder(parameters, L"%folders",
                       FormatSelectedItemsQuoted(foldersOnly));

    // 2. %n (Selected filenames only, without parent directory path)
    if (parameters.find(L"%n") != std::wstring::npos) {
        std::vector<std::wstring> names;
        for (const auto& p : context.allSelectedPaths) {
            PCWSTR name = PathFindFileNameW(p.c_str());
            names.push_back(name ? name : L"");
        }
        ReplacePlaceholder(parameters, L"%n", FormatSelectedItemsQuoted(names));
    }

    // 3. %ext (Extension of first selected item)
    if (parameters.find(L"%ext") != std::wstring::npos) {
        std::wstring ext = L"";
        if (!context.allSelectedPaths.empty()) {
            PCWSTR extPtr =
                PathFindExtensionW(context.allSelectedPaths[0].c_str());
            if (extPtr)
                ext = extPtr;
        }
        ReplacePlaceholder(parameters, L"%ext", ext);
    }

    // 4. %c (Count of selected items)
    ReplacePlaceholder(parameters, L"%c",
                       std::to_wstring(context.allSelectedPaths.size()));

    // 5. %d_name (Name of current folder only)
    if (parameters.find(L"%d_name") != std::wstring::npos) {
        std::wstring dir = context.folderPath;
        while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
            dir.pop_back();
        PCWSTR namePtr = PathFindFileNameW(dir.c_str());
        ReplacePlaceholder(parameters, L"%d_name", namePtr ? namePtr : L"");
    }

    // 6. Retain original %sel% compatibility (Must be before %path%
    // early-return)
    if (parameters.find(L"%sel%") != std::wstring::npos) {
        if (context.allSelectedPaths.empty())
            return std::wstring();
        std::wstring formatted =
            FormatSelectedItemsQuoted(context.allSelectedPaths);
        ReplacePlaceholder(parameters, L"\"%sel%\"", formatted);
        ReplacePlaceholder(parameters, L"%sel%", formatted);
    }

    // Replace %path% BEFORE %p to prevent %p from eating the "%p" in "%path%"
    if (!ReplacePlaceholder(parameters, L"%path%", context.folderPath)) {
        return std::wstring();
    }

    // 7. %p (Parent folder directory path)
    if (parameters.find(L"%p") != std::wstring::npos) {
        std::wstring parent = context.folderPath;
        while (!parent.empty() &&
               (parent.back() == L'\\' || parent.back() == L'/'))
            parent.pop_back();
        std::wstring p;
        // If already at drive root (e.g. "C:"), fallback to the root itself
        // ("C:\")
        if (parent.length() == 2 && parent[1] == L':') {
            p = parent + L'\\';
        } else {
            size_t slash = parent.find_last_of(L"\\/");
            if (slash != std::wstring::npos) {
                p = parent.substr(0, slash);
                if (p.length() == 2 && p[1] == L':')
                    p += L'\\';
            }
        }

        if (!p.empty()) {
            ReplacePlaceholder(parameters, L"%p", p);
        } else {
            // If in a non-filesystem folder, cleanly erase %p instead of
            // leaving raw text
            size_t pos = parameters.find(L"%p");
            while (pos != std::wstring::npos) {
                parameters.replace(pos, 2, L"");
                pos = parameters.find(L"%p", pos);
            }
        }
    }

    return parameters;
}

void ExecuteProcess(std::wstring const& command,
                    std::wstring const& parameters,
                    std::wstring const& workingDir) {
    bool isPath = command.find(L'\\') != std::wstring::npos;
    SHELLEXECUTEINFOW execInfo{};
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    execInfo.lpFile = command.c_str();
    execInfo.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    execInfo.lpDirectory =
        (isPath && !workingDir.empty()) ? workingDir.c_str() : nullptr;
    execInfo.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&execInfo)) {
        Wh_Log(L"ShellExecuteExW failed for %s: %u", command.c_str(),
               GetLastError());
    }
}

void LaunchItemForWindow(HWND hExplorerWnd,
                         ActionItem const& item,
                         ExplorerContext const& capturedContext) {
    std::wstring command = ResolveCommandPath(item.command);
    Wh_Log(L"Launching %s for window %08X, path: %s", item.command.c_str(),
           (DWORD)(ULONG_PTR)hExplorerWnd, capturedContext.folderPath.c_str());

    // %sel_each%: run an individual process instance per selected item
    if (item.parameters.find(L"%sel_each%") != std::wstring::npos) {
        if (capturedContext.allSelectedPaths.empty()) {
            std::wstring param = item.parameters;
            ReplacePlaceholder(param, L"\"%sel_each%\"", L"");
            ReplacePlaceholder(param, L"%sel_each%", L"");
            ReplacePlaceholder(param, L"%path%", capturedContext.folderPath);
            ExecuteProcess(command, param, capturedContext.folderPath);
            return;
        }

        constexpr size_t kMaxBatch = 20;
        size_t count =
            std::min(capturedContext.allSelectedPaths.size(), kMaxBatch);
        for (size_t i = 0; i < count; i++) {
            const auto& path = capturedContext.allSelectedPaths[i];
            std::wstring param = item.parameters;
            std::wstring safePath = path;
            if (!safePath.empty() && safePath.back() == L'\\' &&
                safePath.size() > 3) {
                safePath += L'\\';
            }

            size_t qPos = param.find(L"\"%sel_each%\"");
            if (qPos != std::wstring::npos) {
                param.replace(qPos, 12, L"\"" + safePath + L"\"");
            } else {
                size_t uPos = param.find(L"%sel_each%");
                if (uPos != std::wstring::npos) {
                    param.replace(uPos, 10, L"\"" + safePath + L"\"");
                }
            }

            PCWSTR namePtr = PathFindFileNameW(safePath.c_str());
            PCWSTR extPtr = PathFindExtensionW(safePath.c_str());

            std::wstring nameStr = namePtr ? namePtr : L"";
            if (!nameStr.empty() && nameStr.find(L' ') != std::wstring::npos) {
                nameStr = L"\"" + nameStr + L"\"";
            }
            ReplacePlaceholder(param, L"%n", nameStr);
            ReplacePlaceholder(param, L"%ext", extPtr ? extPtr : L"");

            param = BuildParameters(param, capturedContext);

            ExecuteProcess(command, param, capturedContext.folderPath);
        }
        return;
    }

    std::wstring parameters = BuildParameters(item.parameters, capturedContext);
    ExecuteProcess(command, parameters, capturedContext.folderPath);
}

std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

void TrackLaunchThread(HANDLE thread) {
    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
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

void RunShellWorkOnWorkerThread(std::function<void()> work) {
    auto* pWork = new std::function<void()>(std::move(work));
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<std::function<void()>> work(
                reinterpret_cast<std::function<void()>*>(lpParam));
            ComApartmentScope comScope(COINIT_APARTMENTTHREADED |
                                       COINIT_DISABLE_OLE1DDE);
            try {
                (*work)();
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
            return 0;
        },
        pWork, 0, nullptr);

    if (thread) {
        TrackLaunchThread(thread);
    } else {
        delete pWork;
    }
}

void OnActionInvoked(mux::FrameworkElement const& elementForWindow,
                     ActionItem const& item) {
    if (item.command.empty() || g_unloading)
        return;

    HWND hWnd = GetExplorerWindowForElement(elementForWindow);

    if (item.command == L"internal:TogglePreview") {
        INPUT inputs[4] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_MENU;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = 'P';
        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'P';
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wVk = VK_MENU;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, inputs, sizeof(INPUT));
        return;
    }

    if (item.command == L"internal:ToggleDetails") {
        INPUT inputs[6] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_MENU;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_SHIFT;
        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'P';
        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wVk = 'P';
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[4].type = INPUT_KEYBOARD;
        inputs[4].ki.wVk = VK_SHIFT;
        inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[5].type = INPUT_KEYBOARD;
        inputs[5].ki.wVk = VK_MENU;
        inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(6, inputs, sizeof(INPUT));
        return;
    }

    if (item.command == L"internal:FolderOptions") {
        RunShellWorkOnWorkerThread([]() {
            ShellExecuteW(nullptr, L"open", L"control.exe", L"folders", nullptr,
                          SW_SHOWNORMAL);
        });
        return;
    }

    if (item.command == L"internal:OpenWith") {
        ExplorerContext context = GetExplorerContext(hWnd);
        if (!context.selectedPath.empty() &&
            !DirectoryExists(context.selectedPath)) {
            std::wstring targetPath = context.selectedPath;
            RunShellWorkOnWorkerThread([hWnd, targetPath]() {
                HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
                if (!hShell32)
                    hShell32 = LoadLibraryW(L"shell32.dll");
                if (hShell32) {
                    typedef void(WINAPI * OpenAs_RunDLL_t)(HWND, HINSTANCE,
                                                           LPCWSTR, int);
                    auto pOpenAs = (OpenAs_RunDLL_t)GetProcAddress(
                        hShell32, "OpenAs_RunDLLW");
                    if (pOpenAs) {
                        pOpenAs(hWnd, nullptr, targetPath.c_str(),
                                SW_SHOWNORMAL);
                    }
                }
            });
        }
        return;
    }

    ExplorerContext context = GetExplorerContext(hWnd);

    RunShellWorkOnWorkerThread(
        [hWnd, item, context]() { LaunchItemForWindow(hWnd, item, context); });
}

// ============================================================================
// PowerToys New+ Emulation
// ============================================================================

std::wstring GetPowerToysNewPlusFolder() {
    return ExpandEnvVars(L"%LOCALAPPDATA%\\Microsoft\\PowerToys\\NewPlus");
}

std::wstring DefaultTemplatesFolder() {
    return JoinPath(GetPowerToysNewPlusFolder(), L"Templates");
}

std::wstring ReadFileAsWideString(std::wstring const& path) {
    HANDLE hFile =
        CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return std::wstring();

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(hFile, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 1024 * 1024) {
        CloseHandle(hFile);
        return std::wstring();
    }

    std::string bytes((size_t)size.QuadPart, '\0');
    DWORD bytesRead = 0;
    BOOL succeeded =
        ReadFile(hFile, bytes.data(), (DWORD)bytes.size(), &bytesRead, nullptr);
    CloseHandle(hFile);
    if (!succeeded)
        return std::wstring();

    bytes.resize(bytesRead);
    if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF &&
        (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF) {
        bytes.erase(0, 3);
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                     (int)bytes.size(), nullptr, 0);
    if (length <= 0)
        return std::wstring();

    std::wstring result((size_t)length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        result.data(), length);
    return result;
}

wdj::IJsonValue FindNewPlusSetting(wdj::JsonObject const& root, PCWSTR name) {
    wdj::JsonObject properties = root;
    if (auto nested = root.TryLookup(L"properties");
        nested && nested.ValueType() == wdj::JsonValueType::Object) {
        properties = nested.GetObject();
    }
    auto value = properties.TryLookup(name);
    if (value && value.ValueType() == wdj::JsonValueType::Object) {
        return value.GetObject().TryLookup(L"value");
    }
    return value;
}

struct PowerToysConfig {
    std::wstring templateFolder;
    bool hideFileExtension = true;
    bool hideStartingDigits = true;
    bool replaceVariables = false;
};

PowerToysConfig ReadPowerToysConfig() {
    PowerToysConfig config;
    std::wstring json = ReadFileAsWideString(
        JoinPath(GetPowerToysNewPlusFolder(), L"settings.json"));
    if (json.empty())
        return config;

    wdj::JsonObject root{nullptr};
    if (!wdj::JsonObject::TryParse(json, root)) {
        Wh_Log(L"Couldn't parse the New+ settings file");
        return config;
    }

    auto readBool = [&root](PCWSTR name, bool fallback) {
        auto value = FindNewPlusSetting(root, name);
        if (!value)
            return fallback;
        switch (value.ValueType()) {
            case wdj::JsonValueType::Boolean:
                return value.GetBoolean();
            case wdj::JsonValueType::Number:
                return value.GetNumber() != 0;
            case wdj::JsonValueType::String: {
                std::wstring text = ToLower(std::wstring(value.GetString()));
                if (text == L"true" || text == L"1")
                    return true;
                if (text == L"false" || text == L"0")
                    return false;
                return fallback;
            }
            default:
                return fallback;
        }
    };

    if (auto templateLocation = FindNewPlusSetting(root, L"TemplateLocation");
        templateLocation &&
        templateLocation.ValueType() == wdj::JsonValueType::String) {
        config.templateFolder =
            TrimQuotesAndSpaces(std::wstring(templateLocation.GetString()));
    }

    config.hideFileExtension =
        readBool(L"HideFileExtension", config.hideFileExtension);
    config.hideStartingDigits =
        readBool(L"HideStartingDigits", config.hideStartingDigits);
    config.replaceVariables =
        readBool(L"ReplaceVariables", config.replaceVariables);
    return config;
}

struct EffectiveConfig {
    std::wstring templateFolder;
    bool hideFileExtension;
    bool hideStartingDigits;
    bool replaceVariables;
    bool showIcons;
    bool showTemplatesFolderItem;
};

EffectiveConfig GetEffectiveConfig() {
    std::wstring templateFolderSetting;
    EffectiveConfig config{};
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        templateFolderSetting = g_settings.newPlus.templateFolder;
        config.showIcons = g_settings.newPlus.showIcons;
        config.showTemplatesFolderItem =
            g_settings.newPlus.showTemplatesFolderItem;
    }

    PowerToysConfig powerToys = ReadPowerToysConfig();
    config.templateFolder = ExpandEnvVars(
        !templateFolderSetting.empty()
            ? templateFolderSetting
            : (!powerToys.templateFolder.empty() ? powerToys.templateFolder
                                                 : DefaultTemplatesFolder()));
    config.hideFileExtension = powerToys.hideFileExtension;
    config.hideStartingDigits = powerToys.hideStartingDigits;
    config.replaceVariables = powerToys.replaceVariables;
    return config;
}

struct TemplateEntry {
    std::wstring path;
    std::wstring fileName;
    std::wstring displayName;
    bool isDirectory = false;
};

std::wstring StripStartingDigits(std::wstring const& name) {
    size_t pos = 0;
    while (pos < name.size() && iswdigit(name[pos]))
        pos++;
    if (pos == 0)
        return name;
    while (pos < name.size() && wcschr(L" .-_", name[pos]))
        pos++;
    if (pos >= name.size())
        return name;
    return name.substr(pos);
}

std::wstring MakeDisplayName(std::wstring const& fileName,
                             bool isDirectory,
                             EffectiveConfig const& config) {
    std::wstring name = fileName;
    if (!isDirectory && config.hideFileExtension) {
        size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0)
            name.resize(dot);
    }
    if (config.hideStartingDigits)
        name = StripStartingDigits(name);
    return name;
}

std::vector<TemplateEntry> EnumerateTemplates(EffectiveConfig const& config) {
    std::vector<TemplateEntry> entries;
    std::wstring pattern = JoinPath(config.templateFolder, L"*");

    WIN32_FIND_DATAW findData{};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &findData,
                                    FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Couldn't enumerate %s: %u", config.templateFolder.c_str(),
               GetLastError());
        return entries;
    }

    do {
        std::wstring fileName = findData.cFileName;
        if (fileName == L"." || fileName == L".." ||
            _wcsicmp(fileName.c_str(), L"desktop.ini") == 0)
            continue;
        if (findData.dwFileAttributes &
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
            continue;

        TemplateEntry entry;
        entry.isDirectory =
            (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry.fileName = fileName;
        entry.path = JoinPath(config.templateFolder, fileName);
        entry.displayName =
            MakeDisplayName(fileName, entry.isDirectory, config);
        entries.push_back(std::move(entry));
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);

    std::sort(entries.begin(), entries.end(),
              [](TemplateEntry const& a, TemplateEntry const& b) {
                  if (a.isDirectory != b.isDirectory)
                      return a.isDirectory;
                  return StrCmpLogicalW(a.fileName.c_str(),
                                        b.fileName.c_str()) < 0;
              });

    return entries;
}

std::wstring ReplaceAll(std::wstring str,
                        std::wstring_view from,
                        std::wstring const& to) {
    if (from.empty())
        return str;
    size_t pos = str.find(from);
    while (pos != std::wstring::npos) {
        str.replace(pos, from.size(), to);
        pos = str.find(from, pos + to.size());
    }
    return str;
}

std::wstring TwoDigits(int value) {
    WCHAR buffer[8];
    swprintf(buffer, ARRAYSIZE(buffer), L"%02d", value);
    return buffer;
}

std::wstring ReplaceNameVariables(std::wstring const& fileName,
                                  std::wstring const& targetFolder) {
    SYSTEMTIME time{};
    GetLocalTime(&time);

    std::wstring parentFolderName;
    {
        std::wstring folder = targetFolder;
        while (!folder.empty() &&
               (folder.back() == L'\\' || folder.back() == L'/'))
            folder.pop_back();
        size_t slash = folder.find_last_of(L"\\/");
        parentFolderName =
            slash == std::wstring::npos ? folder : folder.substr(slash + 1);
    }

    std::wstring result = fileName;
    result = ReplaceAll(result, L"$PARENT_FOLDER_NAME", parentFolderName);
    result = ReplaceAll(result, L"$YYYY", std::to_wstring(time.wYear));
    result = ReplaceAll(result, L"$YY", TwoDigits(time.wYear % 100));
    result = ReplaceAll(result, L"$MM", TwoDigits(time.wMonth));
    result = ReplaceAll(result, L"$DD", TwoDigits(time.wDay));
    result = ReplaceAll(result, L"$hh", TwoDigits(time.wHour));
    result = ReplaceAll(result, L"$mm", TwoDigits(time.wMinute));
    result = ReplaceAll(result, L"$ss", TwoDigits(time.wSecond));
    return result;
}

void SplitFileName(std::wstring const& fileName,
                   bool isDirectory,
                   std::wstring* baseName,
                   std::wstring* extension) {
    size_t dot = isDirectory ? std::wstring::npos : fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        *baseName = fileName;
        extension->clear();
        return;
    }
    *baseName = fileName.substr(0, dot);
    *extension = fileName.substr(dot);
}

std::wstring MakeUniquePath(std::wstring const& targetFolder,
                            std::wstring const& fileName,
                            bool isDirectory) {
    std::wstring path = JoinPath(targetFolder, fileName);
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return path;

    std::wstring baseName, extension;
    SplitFileName(fileName, isDirectory, &baseName, &extension);

    for (int i = 2; i < 10000; i++) {
        std::wstring candidate =
            baseName + L" (" + std::to_wstring(i) + L")" + extension;
        path = JoinPath(targetFolder, candidate);
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
            return path;
    }
    return std::wstring();
}

bool CopyTemplateItem(std::wstring const& source,
                      std::wstring const& targetFolder,
                      std::wstring const& targetName) {
    winrt::com_ptr<IFileOperation> operation;
    HRESULT hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(operation.put()));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(FileOperation) failed: %08X", hr);
        return false;
    }

    hr = operation->SetOperationFlags(FOF_NO_UI);
    if (FAILED(hr)) {
        Wh_Log(L"SetOperationFlags failed: %08X", hr);
        return false;
    }

    winrt::com_ptr<IShellItem> sourceItem, targetFolderItem;
    if (FAILED(SHCreateItemFromParsingName(source.c_str(), nullptr,
                                           IID_PPV_ARGS(sourceItem.put()))) ||
        FAILED(SHCreateItemFromParsingName(
            targetFolder.c_str(), nullptr,
            IID_PPV_ARGS(targetFolderItem.put()))) ||
        FAILED(operation->CopyItem(sourceItem.get(), targetFolderItem.get(),
                                   targetName.c_str(), nullptr)) ||
        FAILED(operation->PerformOperations()))
        return false;

    BOOL aborted = FALSE;
    return SUCCEEDED(operation->GetAnyOperationsAborted(&aborted)) && !aborted;
}

void SelectAndRename(winrt::com_ptr<IShellView> const& shellView,
                     std::wstring const& path) {
    if (!shellView)
        return;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl)
        return;

    PCUITEMID_CHILD child = ILFindLastID(pidl);
    auto folderView = shellView.try_as<IFolderView>();

    for (int attempt = 0; attempt < 20 && !g_unloading; attempt++) {
        Sleep(50);
        HRESULT hr = shellView->SelectItem(
            child, SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE |
                       SVSI_FOCUSED | SVSI_EDIT);
        if (FAILED(hr))
            continue;
        if (!folderView)
            break;
        int selectedCount = 0;
        if (SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &selectedCount)) &&
            selectedCount > 0)
            break;
    }

    CoTaskMemFree(pidl);
}

void CreateFromTemplateForWindow(HWND hExplorerWnd,
                                 TemplateEntry const& entry,
                                 bool replaceVariables) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd);
    if (context.folderPath.empty()) {
        Wh_Log(L"No filesystem folder for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return;
    }

    std::wstring fileName =
        replaceVariables
            ? ReplaceNameVariables(entry.fileName, context.folderPath)
            : entry.fileName;
    std::wstring targetPath =
        MakeUniquePath(context.folderPath, fileName, entry.isDirectory);
    if (targetPath.empty()) {
        Wh_Log(L"Couldn't find a free name for %s", fileName.c_str());
        return;
    }

    Wh_Log(L"Creating %s from %s", targetPath.c_str(), entry.path.c_str());

    bool succeeded = CopyTemplateItem(entry.path, context.folderPath,
                                      PathFindFileNameW(targetPath.c_str()));
    if (!succeeded &&
        GetFileAttributesW(targetPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;
    SelectAndRename(context.shellView, targetPath);
}

#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

std::wstring ResolveAppExecutionAlias(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_REPARSE_POINT))
        return path;

    HANDLE hFile = CreateFileW(
        path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return path;

    struct AppExecLinkReparseBuffer {
        ULONG reparseTag;
        USHORT reparseDataLength;
        USHORT reserved;
        ULONG version;
        WCHAR stringList[1];
    };

    alignas(8) BYTE buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
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
    const WCHAR* end = reinterpret_cast<const WCHAR*>(buffer + bytesReturned);
    for (int i = 0; i < 3 && p < end; i++) {
        size_t length = wcsnlen(p, end - p);
        if (p + length >= end)
            break;
        if (i == 2)
            return std::wstring(p, length);
        p += length + 1;
    }

    return path;
}

// ============================================================================
// Icons Handling & Rendering
// ============================================================================

HICON ExtractCommandIcon(std::wstring const& command) {
    std::wstring path = ResolveAppExecutionAlias(ResolveCommandPath(command));
    HICON hIcon = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hIcon, nullptr, 1) && hIcon)
        return hIcon;
    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(path.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }
    Wh_Log(L"Couldn't get an icon for %s (%s)", command.c_str(), path.c_str());
    return nullptr;
}

struct DecodedIcon {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
    bool empty() const { return pixels.empty(); }
};

bool ReadBitmapPixels(HBITMAP hBitmap, DecodedIcon* decoded) {
    BITMAP bm{};
    if (!GetObject(hBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0)
        return false;

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> pixels((size_t)width * height * 4);
    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc)
        return false;

    bool succeeded = GetDIBits(hdc, hBitmap, 0, height, pixels.data(), &bmi,
                               DIB_RGB_COLORS) != 0;
    DeleteDC(hdc);

    if (!succeeded)
        return false;
    decoded->width = width;
    decoded->height = height;
    decoded->pixels = std::move(pixels);
    return true;
}

bool HasNoAlphaChannel(std::vector<uint8_t> const& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p])
            return false;
    }
    return true;
}

void MakeOpaque(std::vector<uint8_t>& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4)
        pixels[p] = 255;
}

bool DecodeMonochromeIcon(HBITMAP hbmMask, DecodedIcon* decoded) {
    DecodedIcon mask;
    if (!ReadBitmapPixels(hbmMask, &mask) || mask.height % 2 != 0)
        return false;

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
            if (mask.pixels[andIndex])
                continue;
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
    if (!GetIconInfo(hIcon, &iconInfo))
        return false;

    bool succeeded = false;
    if (iconInfo.hbmColor) {
        if (ReadBitmapPixels(iconInfo.hbmColor, decoded)) {
            if (HasNoAlphaChannel(decoded->pixels)) {
                MakeOpaque(decoded->pixels);
            } else {
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

    if (iconInfo.hbmColor)
        DeleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask)
        DeleteObject(iconInfo.hbmMask);
    return succeeded;
}

muxm::ImageSource CreateImageSource(DecodedIcon const& decoded) try {
    if (decoded.empty())
        return nullptr;
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
    if (!glyphSetting[0])
        return std::wstring();
    if (!glyphSetting[1])
        return std::wstring(1, glyphSetting[0]);
    for (PCWSTR p = glyphSetting; *p; p++) {
        if (!iswxdigit(*p)) {
            Wh_Log(L"%s is not a glyph code point", glyphSetting);
            return std::wstring();
        }
    }
    unsigned long parsed = wcstoul(glyphSetting, nullptr, 16);
    if (parsed > 0 && parsed <= 0xFFFF)
        return std::wstring(1, (WCHAR)parsed);
    return std::wstring();
}

bool LooksLikeIconPath(std::wstring const& iconSetting) {
    if (iconSetting.find(L'\\') != std::wstring::npos ||
        iconSetting.find(L'/') != std::wstring::npos ||
        iconSetting.find(L':') != std::wstring::npos)
        return true;

    std::wstring lower = ToLower(iconSetting);
    size_t comma = lower.rfind(L',');
    if (comma != std::wstring::npos)
        lower.resize(comma);

    return lower.ends_with(L".exe") || lower.ends_with(L".dll") ||
           lower.ends_with(L".ico");
}

HICON LoadIconFromPath(std::wstring const& iconPath) {
    std::wstring expanded = ExpandEnvVars(iconPath);
    int iconIndex = 0;
    size_t comma = expanded.rfind(L',');
    if (comma != std::wstring::npos && comma + 1 < expanded.size()) {
        bool isNumber = true;
        for (size_t i = comma + 1; i < expanded.size(); i++) {
            WCHAR c = expanded[i];
            if ((c < L'0' || c > L'9') && !(i == comma + 1 && c == L'-')) {
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
        hIcon)
        return hIcon;

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(expanded.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }
    Wh_Log(L"Couldn't load an icon from %s", iconPath.c_str());
    return nullptr;
}

bool IsShellPath(std::wstring const& s) {
    return s.size() >= 6 && _wcsnicmp(s.c_str(), L"shell:", 6) == 0;
}

bool DecodeShellPathIcon(std::wstring const& path, DecodedIcon* decoded) {
    std::wstring expanded = ExpandEnvVars(path);
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(
            SHParseDisplayName(expanded.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl) {
        Wh_Log(L"Couldn't parse shell path %s", path.c_str());
        return false;
    }

    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !factory)
        return false;

    SIZE size = {32, 32};
    HBITMAP hBitmap = nullptr;
    hr = factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &hBitmap);
    if (FAILED(hr) || !hBitmap)
        return false;

    bool succeeded = ReadBitmapPixels(hBitmap, decoded);
    DeleteObject(hBitmap);
    if (succeeded && HasNoAlphaChannel(decoded->pixels))
        MakeOpaque(decoded->pixels);
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

std::mutex g_iconCacheMutex;
std::unordered_map<std::wstring, std::shared_ptr<DecodedIcon>> g_iconCache;

std::shared_ptr<DecodedIcon> GetIcon(std::wstring const& iconSetting,
                                     std::wstring const& command) {
    std::wstring key = iconSetting + L'\n' + command;
    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        auto it = g_iconCache.find(key);
        if (it != g_iconCache.end())
            return it->second;
    }

    auto decoded = ResolveIcon(iconSetting, command);
    std::lock_guard<std::mutex> lock(g_iconCacheMutex);
    return g_iconCache.insert_or_assign(std::move(key), std::move(decoded))
        .first->second;
}

muxc::IconElement CreateGlyphIcon(PCWSTR glyph) {
    muxc::FontIcon fontIcon;
    fontIcon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    fontIcon.Glyph(glyph);
    return fontIcon;
}

bool IsSystemDarkModeActive() {
    DWORD useLightTheme = 1;
    DWORD size = sizeof(useLightTheme);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &useLightTheme, &size);
    return useLightTheme == 0;
}

std::wstring TrimWhitespaceAndQuotes(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return L"";
    size_t last = str.find_last_not_of(L" \t");
    str = str.substr(first, last - first + 1);
    if (str.size() >= 2 && str.front() == L'"' && str.back() == L'"') {
        str = str.substr(1, str.size() - 2);
        first = str.find_first_not_of(L" \t");
        if (first == std::wstring::npos)
            return L"";
        last = str.find_last_not_of(L" \t");
        str = str.substr(first, last - first + 1);
    }
    return str;
}

std::wstring ResolveThemedIconString(const std::wstring& rawSetting) {
    size_t pipePos = rawSetting.find(L'|');

    if (pipePos == std::wstring::npos) {
        return TrimWhitespaceAndQuotes(rawSetting);
    }

    std::wstring lightTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(0, pipePos));
    std::wstring darkTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(pipePos + 1));

    if (lightTheme.empty())
        lightTheme = darkTheme;
    if (darkTheme.empty())
        darkTheme = lightTheme;

    return IsSystemDarkModeActive() ? darkTheme : lightTheme;
}

muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command) {
    std::wstring effectiveSetting = ResolveThemedIconString(iconSetting);

    bool isPath =
        !effectiveSetting.empty() && LooksLikeIconPath(effectiveSetting);
    if (isPath || effectiveSetting.empty()) {
        if (auto source =
                CreateImageSource(*GetIcon(effectiveSetting, command))) {
            muxc::ImageIcon imageIcon;
            imageIcon.Source(source);
            return imageIcon;
        }
    }

    std::wstring glyph;
    if (!isPath)
        glyph = ParseGlyphSetting(effectiveSetting.c_str());
    if (glyph.empty())
        return nullptr;
    return CreateGlyphIcon(glyph.c_str());
}

muxc::IconElement CreateIconElement(std::wstring const& iconSetting,
                                    std::wstring const& command,
                                    PCWSTR defaultGlyph) {
    if (auto icon = TryCreateIconElement(iconSetting, command))
        return icon;
    return CreateGlyphIcon(defaultGlyph);
}

muxc::IconElement MakeCommandButtonIcon(ActionItem const& item) {
    if (item.hideIcon)
        return nullptr;
    PCWSTR defaultGlyph = L"";
    if (item.command == L"internal:OpenWith")
        defaultGlyph = L"\uE7AC";
    else if (item.command == L"internal:TogglePreview")
        defaultGlyph = L"\uE8A1";
    else if (item.command == L"internal:ToggleDetails")
        defaultGlyph = L"\uE9F9";
    else if (item.command == L"internal:FolderOptions")
        defaultGlyph = L"\uE713";

    return CreateIconElement(item.icon, item.command, defaultGlyph);
}

muxc::IconElement MakeNewPlusButtonIcon(std::wstring const& iconSetting,
                                        std::wstring const& originalIconUri) {
    if (iconSetting.empty() && !originalIconUri.empty()) {
        try {
            muxm::Imaging::SvgImageSource svgSource(
                wf::Uri(winrt::hstring{originalIconUri}));
            muxc::ImageIcon imageIcon;
            imageIcon.Source(svgSource);
            return imageIcon;
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
    return CreateIconElement(iconSetting, std::wstring(), L"");
}

muxm::ImageSource LoadShellItemIcon(std::wstring const& path) {
    DecodedIcon decoded;
    if (!DecodeShellPathIcon(path, &decoded))
        return nullptr;
    return CreateImageSource(decoded);
}

bool IsOurNewPlusButton(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    return element && element.Name() == kNewPlusButtonName;
}

bool IsOurElement(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    if (!element)
        return false;
    std::wstring_view name{element.Name()};
    return name.starts_with(kButtonNamePrefix) || name == kNewPlusButtonName ||
           name == kContextMenuButtonName;
}

bool HasElement(muxc::CommandBar const& commandBar,
                bool (*predicate)(muxc::ICommandBarElement const&)) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (predicate(command))
            return true;
    }
    return false;
}

std::wstring GetButtonIconUri(muxc::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon)
        return std::wstring();

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

    if (!uri)
        return std::wstring();
    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

int IdentifyDefaultButton(muxc::AppBarButton const& button) {
    std::wstring uri = ToLower(GetButtonIconUri(button));
    if (!uri.empty()) {
        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);
        if (fileName.starts_with(L"windows.iconsize."))
            return kViewButtonIndex;

        for (int i = 0; i < kDefaultButtonCount; i++) {
            if (fileName == kDefaultButtons[i].svgFileName)
                return i;
        }
    }

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

enum class ManagedKind {
    Button,
    SeparatorAfter,
    GroupSeparator,
    DetailsToggle,
    OverflowElement,
};

struct GroupMember {
    winrt::weak_ref<mux::UIElement> element;
    int defaultButtonIndex;
};

struct ManagedTarget {
    ManagedKind kind;
    int index = 0;
    bool newPlusPresent = false;
    std::shared_ptr<std::vector<GroupMember>> group;
};

bool ShouldHide(ManagedTarget const& target);

mux::Visibility EffectiveVisibility(mux::UIElement const& element) {
    auto* entry = FindManagedElement(element);
    if (entry && entry->hasOriginalVisibility)
        return entry->originalVisibility;
    return element.Visibility();
}

bool IsGroupHidden(std::shared_ptr<std::vector<GroupMember>> const& group) {
    if (!group || group->empty())
        return false;
    for (auto const& member : *group) {
        auto element = member.element.get();
        if (!element)
            continue;
        if (member.defaultButtonIndex >= 0 &&
            ShouldHide({ManagedKind::Button, member.defaultButtonIndex}))
            continue;
        if (EffectiveVisibility(element) == mux::Visibility::Collapsed)
            continue;
        return false;
    }
    return true;
}

bool ShouldHide(ManagedTarget const& target) {
    if (target.kind == ManagedKind::GroupSeparator)
        return IsGroupHidden(target.group);

    std::lock_guard<std::mutex> lock(g_settings.mutex);
    switch (target.kind) {
        case ManagedKind::Button:
            if (target.index == kNewButtonIndex && target.newPlusPresent &&
                g_settings.newPlus.enabled &&
                !g_settings.newPlus.keepOriginalNewButton) {
                return true;
            }
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

// ============================================================================
// Dynamic Contextual Buttons
// ============================================================================
bool ItemMatchesExtensionFilter(
    const std::wstring& filterPattern,
    const std::vector<std::wstring>& selectedPaths) {
    std::wstring filter = TrimWhitespaceAndQuotes(filterPattern);

    // Rule 1: Blank filter means always show (default behavior)
    if (filter.empty()) {
        return true;
    }

    // Rule 2: If a filter is specified but nothing is selected, hide the button
    if (selectedPaths.empty()) {
        return false;
    }

    std::vector<std::wstring> positiveExts;
    std::vector<std::wstring> negativeExts;
    bool matchAllExceptNegatives = false;

    // Parse comma-separated tokens (supports separators: ',', ';', ' ')
    size_t start = 0;
    while (start < filter.size()) {
        size_t end = filter.find_first_of(L",; ", start);
        if (end == std::wstring::npos)
            end = filter.size();

        std::wstring token = filter.substr(start, end - start);
        start = end + 1;

        token = TrimWhitespaceAndQuotes(token);
        if (token.empty())
            continue;

        token = ToLower(token);

        bool isNegative = false;
        if (token.front() == L'-' || token.front() == L'~') {
            isNegative = true;
            token.erase(token.begin());
        }

        // Strip leading '.' if user typed ".txt" instead of "txt"
        if (!token.empty() && token.front() == L'.') {
            token.erase(token.begin());
        }

        // Normalize folder aliases to "folder"
        if (token == L"dir" || token == L"directory" || token == L"folders") {
            token = L"folder";
        }

        bool isFileKeyword = (token == L"file" || token == L"files");

        if (token == L"*") {
            if (!isNegative)
                matchAllExceptNegatives = true;
            continue;
        }

        // "files" or "file" matches any file while automatically excluding
        // folders
        if (isFileKeyword) {
            if (isNegative) {
                negativeExts.push_back(L"file");
            } else {
                matchAllExceptNegatives = true;
                negativeExts.push_back(L"folder");
            }
            continue;
        }

        if (isNegative) {
            negativeExts.push_back(token);
        } else {
            positiveExts.push_back(token);
        }
    }

    // Rule 3: Validate EVERY selected item strictly
    for (const auto& path : selectedPaths) {
        DWORD attr = GetFileAttributesW(path.c_str());
        bool isDirectory = (attr != INVALID_FILE_ATTRIBUTES) &&
                           (attr & FILE_ATTRIBUTE_DIRECTORY);

        std::wstring ext;
        if (isDirectory) {
            ext = L"folder";
        } else {
            PCWSTR extPtr = PathFindExtensionW(path.c_str());
            ext = extPtr ? ToLower(extPtr) : L"";
            if (!ext.empty() && ext.front() == L'.') {
                ext.erase(ext.begin());
            }
        }

        // Strict Exclusion: abort if item matches a negative extension
        for (const auto& neg : negativeExts) {
            if (ext == neg || (neg == L"file" && !isDirectory)) {
                return false;
            }
        }

        // Strict Inclusion: if positive list exists, item must match at least
        // one
        if (!matchAllExceptNegatives && !positiveExts.empty()) {
            bool matched = false;
            for (const auto& pos : positiveExts) {
                if (ext == pos) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
        }
    }

    return true;
}

void UpdateDynamicButtonStates(muxc::CommandBar const& commandBar,
                               HWND hExplorerWnd) {
    if (g_unloading || !commandBar)
        return;

    if (!hExplorerWnd) {
        hExplorerWnd = GetExplorerWindowForElement(commandBar);
    }
    if (!hExplorerWnd)
        return;

    ExplorerContext context = GetExplorerContext(hExplorerWnd);

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    for (uint32_t i = 0; i < count; i++) {
        auto cmd = commands.GetAt(i);
        auto element = cmd.try_as<mux::FrameworkElement>();
        if (!element)
            continue;

        std::wstring name{element.Name()};
        if (!name.starts_with(kButtonNamePrefix))
            continue;

        auto button = cmd.try_as<muxc::AppBarButton>();
        if (!button)
            continue;

        size_t underscore = name.find(L'_');
        if (underscore == std::wstring::npos)
            continue;

        int itemIndex = _wtoi(name.c_str() + underscore + 1);

        ActionItem item;
        {
            std::lock_guard<std::mutex> lock(g_settings.mutex);
            if (itemIndex >= 0 && itemIndex < (int)g_settings.items.size()) {
                item = g_settings.items[itemIndex];
            } else {
                continue;
            }
        }

        // Evaluate dynamic filter rules
        bool isVisible = ItemMatchesExtensionFilter(item.matchExtensions,
                                                    context.allSelectedPaths);

        bool disabledInsteadOfHidden;
        {
            std::lock_guard<std::mutex> lock(g_settings.mutex);
            disabledInsteadOfHidden = g_settings.disabledInsteadOfHidden;
        }

        if (disabledInsteadOfHidden) {
            button.Visibility(mux::Visibility::Visible);
            button.IsEnabled(isVisible);
        } else {
            button.Visibility(isVisible ? mux::Visibility::Visible
                                        : mux::Visibility::Collapsed);
            button.IsEnabled(isVisible);
        }
    }
}

// ============================================================================
// CommandBar Layout, Visibility & Management
// ============================================================================

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow = false);
void UpdateCommandBar(muxc::CommandBar const& commandBar);

thread_local std::unordered_map<void*, bool> g_pendingUpdates;

void QueueCommandBarUpdate(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar,
    bool fullUpdate) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar)
        return;

    void* key = winrt::get_abi(commandBar);
    auto [it, inserted] = g_pendingUpdates.insert({key, fullUpdate});
    if (!inserted) {
        it->second = it->second || fullUpdate;
        return;
    }

    auto takePending = [key]() {
        bool full = false;
        auto it = g_pendingUpdates.find(key);
        if (it != g_pendingUpdates.end()) {
            full = it->second;
            g_pendingUpdates.erase(it);
        }
        return full;
    };

    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    dispatcherQueue.TryEnqueue([weakCommandBar, takePending]() {
        bool full = takePending();
        if (g_unloading)
            return;
        if (auto commandBar = weakCommandBar.get()) {
            try {
                if (full)
                    UpdateCommandBar(commandBar);
                else
                    ApplyDefaultButtonVisibility(commandBar);
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
        }
    });
}

void QueueVisibilityRecompute(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar) {
    QueueCommandBarUpdate(weakCommandBar, false);
}

void WatchVisibility(mux::UIElement const& element,
                     ManagedTarget const& target,
                     winrt::weak_ref<muxc::CommandBar> const& owner) {
    int64_t token = element.RegisterPropertyChangedCallback(
        mux::UIElement::VisibilityProperty(),
        [target, owner](mux::DependencyObject const& sender,
                        mux::DependencyProperty const&) {
            if (g_unloading || g_settingVisibilityDepth > 0)
                return;
            auto element = sender.try_as<mux::UIElement>();
            if (!element)
                return;

            mux::Visibility visibility = element.Visibility();
            {
                auto& entry = GetManagedElement(element);
                entry.originalVisibility = visibility;
                entry.hasOriginalVisibility = true;
            }

            if (target.kind != ManagedKind::GroupSeparator &&
                visibility != mux::Visibility::Collapsed &&
                ShouldHide(target)) {
                SetVisibilityInternal(element, mux::Visibility::Collapsed);
            }
            QueueVisibilityRecompute(owner);
        });

    auto& entry = GetManagedElement(element);
    entry.watched = true;
    entry.visibilityToken = token;
}

void UnwatchVisibilityForCurrentThread() {
    for (auto& entry : g_managedElements) {
        if (!entry.watched)
            continue;
        auto element = entry.element.get();
        int64_t token = entry.visibilityToken;
        entry.watched = false;
        entry.visibilityToken = 0;
        if (!element)
            continue;
        try {
            element.UnregisterPropertyChangedCallback(
                mux::UIElement::VisibilityProperty(), token);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

void ForgetManagedElementsForCurrentThread() {
    g_managedElements.clear();
}

void SetManagedVisibility(mux::UIElement const& element,
                          ManagedTarget const& target,
                          bool forceShow,
                          winrt::weak_ref<muxc::CommandBar> const& owner) {
    mux::Visibility original;
    bool watch;
    {
        auto& entry = GetManagedElement(element);
        if (!entry.hasOriginalVisibility) {
            entry.originalVisibility = element.Visibility();
            entry.hasOriginalVisibility = true;
        }
        original = entry.originalVisibility;
        watch = !forceShow && !entry.watched;
    }

    if (watch)
        WatchVisibility(element, target, owner);
    SetVisibilityInternal(element, !forceShow && ShouldHide(target)
                                       ? mux::Visibility::Collapsed
                                       : original);
}

void ApplyItemSpacing(muxc::AppBarButton const& button,
                      int spacing,
                      bool reset) {
    mux::Thickness originalMargin;
    double originalMinWidth;
    {
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
        if (auto found = FindDescendantByName(child, name))
            return found;
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

    struct Entry {
        muxc::ICommandBarElement command;
        bool isOurs = false;
        bool isNewPlus = false;
        bool isSeparator = false;
        bool isDetailsToggle = false;
        int defaultIndex = -1;
    };

    std::vector<Entry> entries;
    entries.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        Entry entry;
        entry.command = commands.GetAt(i);
        entry.isOurs = IsOurElement(entry.command);
        entry.isNewPlus = entry.isOurs && IsOurNewPlusButton(entry.command);

        bool isSepType =
            static_cast<bool>(entry.command.try_as<muxc::AppBarSeparator>());
        entry.isSeparator = !entry.isOurs && isSepType;

        if (!entry.isOurs && !entry.isSeparator) {
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                if (GetAutomationId(button) == L"DetailsPaneToggleButton") {
                    entry.isDetailsToggle = true;
                } else {
                    entry.defaultIndex = IdentifyDefaultButton(button);
                    if (entry.defaultIndex < 0) {
                        Wh_Log(
                            L"Unrecognized command %u: icon %s, automation id "
                            L"%s",
                            i, GetButtonIconUri(button).c_str(),
                            GetAutomationId(button).c_str());
                    }
                }
            }
        }
        entries.push_back(std::move(entry));
    }

    uint32_t firstCustomIndex = count;
    bool hasNewPlusButton = false;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isNewPlus)
            hasNewPlusButton = true;
        else if (entries[i].isOurs && firstCustomIndex == count)
            firstCustomIndex = i;
    }

    uint32_t viewSeparatorIndex = count;
    if (firstCustomIndex > 0 && firstCustomIndex < count &&
        entries[firstCustomIndex - 1].isSeparator) {
        viewSeparatorIndex = firstCustomIndex - 1;
    } else {
        for (uint32_t i = count; i > 0; i--) {
            if (entries[i - 1].isSeparator) {
                viewSeparatorIndex = i - 1;
                break;
            }
        }
    }

    auto collectGroup = [&entries, count](uint32_t separatorIndex) {
        auto group = std::make_shared<std::vector<GroupMember>>();
        for (uint32_t i = separatorIndex + 1; i < count; i++) {
            auto const& entry = entries[i];
            if (entry.isNewPlus)
                continue;
            if (entry.isSeparator || entry.isOurs)
                break;
            if (auto element = entry.command.try_as<mux::UIElement>()) {
                group->push_back(
                    {winrt::make_weak(element), entry.defaultIndex});
            }
        }
        return group;
    };

    int prevIndex = -1;
    for (uint32_t i = 0; i < count; i++) {
        auto const& entry = entries[i];
        if (entry.isOurs) {
            // Apply spacing only if this is an action button, not a separator
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                ApplyItemSpacing(button, itemSpacing, forceShow);
            }
            // If our element was a separator, do not disturb prevIndex tracking
            if (!entry.command.try_as<muxc::AppBarSeparator>()) {
                prevIndex = -1;
            }
            continue;
        }

        if (entry.isSeparator) {
            auto separator = entry.command.as<muxc::AppBarSeparator>();
            int target = -1;
            if (prevIndex == kNewButtonIndex)
                target = kNewButtonIndex;
            else if (prevIndex == kDeleteButtonIndex)
                target = kDeleteButtonIndex;
            else if (i == viewSeparatorIndex)
                target = kViewButtonIndex;

            if (target >= 0) {
                setVisibility(separator, {ManagedKind::SeparatorAfter, target});
            } else {
                setVisibility(separator, {ManagedKind::GroupSeparator, -1,
                                          false, collectGroup(i)});
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
            setVisibility(button, {ManagedKind::Button, entry.defaultIndex,
                                   hasNewPlusButton});
            ApplyItemSpacing(button, itemSpacing, forceShow);
        }

        prevIndex = entry.defaultIndex;
    }

    if (isPrimary) {
        muxc::CommandBarOverflowButtonVisibility originalOverflow;
        {
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

    UpdateDynamicButtonStates(commandBar, nullptr);
}

muxc::AppBarButton CreateBareButton(int index,
                                    std::wstring const& tooltip,
                                    std::wstring const& labelText,
                                    muxc::IconElement const& icon,
                                    bool showLabel = false) {
    std::wstring name = kButtonNamePrefix;
    name += L'_';
    name += std::to_wstring(index);

    muxc::AppBarButton button;
    button.Name(name.c_str());

    std::wstring displayLabel = labelText.empty() ? tooltip : labelText;
    button.Label(displayLabel.c_str());
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(icon);

    if (!tooltip.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{tooltip}));
    }

    return button;
}

muxc::AppBarButton CreateActionButton(ActionItem const& item, int index) {
    muxc::AppBarButton button =
        CreateBareButton(index, item.name, item.labelText,
                         MakeCommandButtonIcon(item), item.showLabel);

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke, [item](wf::IInspectable const& sender,
                                                mux::RoutedEventArgs const&) {
            if (auto element = sender.try_as<mux::FrameworkElement>()) {
                OnActionInvoked(element, item);
            }
        }));

    return button;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton);

muxc::MenuFlyoutItemBase CreateMenuEntry(
    ActionItem const& item,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    if (item.isMenu && !item.subItems.empty()) {
        muxc::MenuFlyoutSubItem subMenu;
        subMenu.Text(item.name.c_str());
        if (!item.hideIcon) {
            if (auto icon = TryCreateIconElement(item.icon, item.command))
                subMenu.Icon(icon);
        }
        AppendMenuEntries(item.subItems, subMenu.Items(), weakButton);
        return subMenu;
    }

    muxc::MenuFlyoutItem menuItem;
    menuItem.Text(item.name.c_str());
    if (!item.hideIcon) {
        if (auto icon = TryCreateIconElement(item.icon, item.command))
            menuItem.Icon(icon);
    }

    TrackRevoker(menuItem, menuItem.Click(
                               winrt::auto_revoke,
                               [item, weakButton](wf::IInspectable const&,
                                                  mux::RoutedEventArgs const&) {
                                   if (auto button = weakButton.get())
                                       OnActionInvoked(button, item);
                               }));

    return menuItem;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    for (auto const& item : items) {
        target.Append(CreateMenuEntry(item, weakButton));
        if (item.separatorAfter)
            target.Append(muxc::MenuFlyoutSeparator());
    }
}

struct HoverTimerEntry {
    winrt::weak_ref<muxc::AppBarButton> button;
    mux::DispatcherTimer timer;
    winrt::event_token tickToken;
};

thread_local std::vector<HoverTimerEntry> g_hoverTimers;

void ReleaseHoverTimer(HoverTimerEntry const& entry) {
    try {
        entry.timer.Stop();
        entry.timer.Tick(entry.tickToken);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void TrackHoverTimer(muxc::AppBarButton const& button,
                     mux::DispatcherTimer const& timer,
                     winrt::event_token tickToken) {
    for (auto it = g_hoverTimers.begin(); it != g_hoverTimers.end();) {
        if (!it->button.get()) {
            ReleaseHoverTimer(*it);
            it = g_hoverTimers.erase(it);
        } else {
            ++it;
        }
    }
    g_hoverTimers.push_back({winrt::make_weak(button), timer, tickToken});
}

void StopHoverTimersForCurrentThread() {
    std::vector<HoverTimerEntry> taken;
    taken.swap(g_hoverTimers);
    for (auto const& entry : taken)
        ReleaseHoverTimer(entry);
}

void SetUpOpenOnHover(muxc::AppBarButton const& button,
                      int hoverDelayMs,
                      std::function<void()> const& open) {
    if (hoverDelayMs <= 0) {
        TrackRevoker(
            button,
            button.PointerEntered(
                winrt::auto_revoke,
                [open](wf::IInspectable const&,
                       mux::Input::PointerRoutedEventArgs const&) { open(); }));
        return;
    }

    mux::DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(hoverDelayMs));

    auto tickToken = timer.Tick(
        [open](wf::IInspectable const& sender, wf::IInspectable const&) {
            if (auto timer = sender.try_as<mux::DispatcherTimer>())
                timer.Stop();
            open();
        });

    TrackHoverTimer(button, timer, tickToken);

    TrackRevoker(button,
                 button.PointerEntered(
                     winrt::auto_revoke,
                     [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
                         timer.Stop();
                         timer.Start();
                     }));

    auto stopTimer = [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
        timer.Stop();
    };
    TrackRevoker(button, button.PointerExited(winrt::auto_revoke, stopTimer));
    TrackRevoker(button, button.PointerCanceled(winrt::auto_revoke, stopTimer));
}

std::function<void()> MakeShowFlyoutAction(muxc::AppBarButton const& button) {
    return [weakButton = winrt::make_weak(button)]() {
        auto button = weakButton.get();
        if (!button || g_unloading)
            return;
        if (auto flyout = button.Flyout(); flyout && !flyout.IsOpen())
            flyout.ShowAt(button);
    };
}

muxc::AppBarButton CreateMenuButton(ActionItem const& item,
                                    int index,
                                    bool openOnHover,
                                    int hoverDelayMs) {
    muxc::AppBarButton button =
        CreateBareButton(index, item.name, item.labelText,
                         MakeCommandButtonIcon(item), item.showLabel);
    auto weakButton = winrt::make_weak(button);

    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    auto ensureMenuEntries = [subItems = item.subItems,
                              weakButton](muxc::MenuFlyout const& menu) {
        if (!menu || menu.Items().Size() > 0)
            return;
        try {
            AppendMenuEntries(subItems, menu.Items(), weakButton);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    };

    TrackRevoker(
        menu,
        menu.Opening(winrt::auto_revoke,
                     [ensureMenuEntries](wf::IInspectable const& sender,
                                         wf::IInspectable const&) {
                         ensureMenuEntries(sender.try_as<muxc::MenuFlyout>());
                     }));

    TrackRevoker(
        button,
        button.PointerEntered(
            winrt::auto_revoke, [ensureMenuEntries, weakButton](
                                    wf::IInspectable const&,
                                    mux::Input::PointerRoutedEventArgs const&) {
                if (auto btn = weakButton.get())
                    ensureMenuEntries(btn.Flyout().try_as<muxc::MenuFlyout>());
            }));

    button.Flyout(menu);
    if (openOnHover)
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    return button;
}

void OpenContextMenuForElement(mux::FrameworkElement const& element) {
    if (g_unloading || !element)
        return;
    HWND hExplorerWnd = GetExplorerWindowForElement(element);
    if (!hExplorerWnd) {
        Wh_Log(L"No File Explorer window for the context menu item");
        return;
    }
    POINT point{};
    GetCursorPos(&point);
    ShowShellContextMenu(hExplorerWnd, point);
}

muxc::AppBarButton CreateContextMenuButton(bool openOnHover, int hoverDelayMs) {
    ContextMenuItemSettings settings;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        settings = g_settings.contextMenuItem;
    }

    muxc::AppBarButton button;
    button.Name(kContextMenuButtonName);
    button.Label(settings.buttonLabel.c_str());
    button.LabelPosition(settings.showLabel
                             ? muxc::CommandBarLabelPosition::Default
                             : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(
        CreateIconElement(settings.buttonIcon, std::wstring(), L"\uE8FD"));

    if (!settings.showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                            mux::RoutedEventArgs const&) {
            OpenContextMenuForElement(sender.try_as<mux::FrameworkElement>());
        }));

    if (openOnHover) {
        auto open = [weakButton = winrt::make_weak(button)]() {
            if (auto button = weakButton.get())
                OpenContextMenuForElement(button);
        };
        SetUpOpenOnHover(button, hoverDelayMs, open);
    }

    return button;
}

void PopulateNewPlusMenu(
    muxc::MenuFlyout const& menu,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) try {
    auto items = menu.Items();
    items.Clear();

    EffectiveConfig config = GetEffectiveConfig();
    std::vector<TemplateEntry> templates = EnumerateTemplates(config);

    if (templates.empty()) {
        muxc::MenuFlyoutItem placeholder;
        placeholder.Text(DirectoryExists(config.templateFolder)
                             ? L"No templates"
                             : L"Templates folder not found");
        placeholder.IsEnabled(false);
        items.Append(placeholder);
    }

    bool replaceVariables = config.replaceVariables;
    for (auto const& entry : templates) {
        muxc::MenuFlyoutItem menuItem;
        menuItem.Text(entry.displayName.c_str());
        if (config.showIcons) {
            if (auto source = LoadShellItemIcon(entry.path)) {
                muxc::ImageIcon imageIcon;
                imageIcon.Source(source);
                menuItem.Icon(imageIcon);
            }
        }
        if (entry.displayName != entry.fileName) {
            muxc::ToolTipService::SetToolTip(
                menuItem, winrt::box_value(winrt::hstring{entry.fileName}));
        }

        TrackRevoker(
            menuItem,
            menuItem.Click(
                winrt::auto_revoke,
                [entry, replaceVariables, weakButton](
                    wf::IInspectable const&, mux::RoutedEventArgs const&) {
                    if (g_unloading)
                        return;
                    auto button = weakButton.get();
                    if (!button)
                        return;
                    HWND hWnd = GetExplorerWindowForElement(button);
                    RunShellWorkOnWorkerThread(
                        [hWnd, entry, replaceVariables]() {
                            CreateFromTemplateForWindow(hWnd, entry,
                                                        replaceVariables);
                        });
                }));

        items.Append(menuItem);
    }

    if (config.showTemplatesFolderItem) {
        if (!templates.empty())
            items.Append(muxc::MenuFlyoutSeparator());
        muxc::MenuFlyoutItem openFolderItem;
        openFolderItem.Text(L"Open templates folder");
        openFolderItem.Icon(CreateGlyphIcon(L""));

        TrackRevoker(openFolderItem,
                     openFolderItem.Click(
                         winrt::auto_revoke, [folder = config.templateFolder](
                                                 wf::IInspectable const&,
                                                 mux::RoutedEventArgs const&) {
                             if (g_unloading)
                                 return;
                             RunShellWorkOnWorkerThread([folder]() {
                                 if (!DirectoryExists(folder))
                                     SHCreateDirectoryExW(
                                         nullptr, folder.c_str(), nullptr);
                                 ShellExecuteW(nullptr, L"open", folder.c_str(),
                                               nullptr, nullptr, SW_SHOWNORMAL);
                             });
                         }));

        items.Append(openFolderItem);
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

void AddNewPlusChevron(muxc::AppBarButton const& button) try {
    auto label =
        FindDescendantByName(button, L"TextLabel").try_as<muxc::TextBlock>();
    if (!label) {
        Wh_Log(L"The New+ label wasn't found");
        return;
    }

    std::wstring currentText = label.Text().c_str();
    if (!currentText.empty() && currentText.back() == L'\uE70D')
        return;

    auto inlines = label.Inlines();
    inlines.Clear();

    muxd::Run text;
    std::wstring labelText = button.Label().c_str();
    labelText += L"  ";
    text.Text(labelText);
    inlines.Append(text);

    muxd::Run chevron;
    chevron.Text(L"\uE70D");
    chevron.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    chevron.FontSize(8);
    inlines.Append(chevron);
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::AppBarButton CreateNewPlusButton(std::wstring const& originalIconUri,
                                       bool openOnHover,
                                       int hoverDelayMs) {
    NewPlusSettings settings;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        settings = g_settings.newPlus;
    }

    bool showLabel = settings.showLabel && !settings.buttonLabel.empty();
    muxc::AppBarButton button;
    button.Name(kNewPlusButtonName);
    button.Label(settings.buttonLabel.c_str());
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(MakeNewPlusButtonIcon(settings.buttonIcon, originalIconUri));

    if (showLabel) {
        TrackRevoker(
            button,
            button.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                                 mux::RoutedEventArgs const&) {
                if (auto button = sender.try_as<muxc::AppBarButton>())
                    AddNewPlusChevron(button);
            }));
    }

    if (!showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    auto weakButton = winrt::make_weak(button);
    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    TrackRevoker(
        menu,
        menu.Opening(winrt::auto_revoke,
                     [weakButton](wf::IInspectable const& sender,
                                  wf::IInspectable const&) {
                         if (g_unloading)
                             return;
                         if (auto menu = sender.try_as<muxc::MenuFlyout>())
                             PopulateNewPlusMenu(menu, weakButton);
                     }));

    button.Flyout(menu);
    if (openOnHover)
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    return button;
}

void EnsureActionButtons(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;
    if (HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && std::wstring_view(element.Name())
                                  .starts_with(kButtonNamePrefix);
        }))
        return;

    bool openMenuOnHover;
    int menuHoverDelay;
    std::vector<ActionItem> items;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
        items = g_settings.items;
    }

    if (items.empty())
        return;

    Wh_Log(L"Adding %zu items to command bar", items.size());
    auto commands = commandBar.PrimaryCommands();

    if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        uint32_t insertIndex = 0;
        for (size_t i = 0; i < items.size(); i++) {
            auto const& item = items[i];
            if (item.isMenu && !item.subItems.empty()) {
                commands.InsertAt(
                    insertIndex++,
                    CreateMenuButton(item, (int)i, openMenuOnHover,
                                     menuHoverDelay));
            } else {
                commands.InsertAt(insertIndex++,
                                  CreateActionButton(item, (int)i));
            }

            if (item.separatorAfter) {
                muxc::AppBarSeparator separator;
                std::wstring name = kButtonNamePrefix;
                name += L"_Sep_";
                name += std::to_wstring(i);
                separator.Name(name.c_str());
                commands.InsertAt(insertIndex++, separator);
            }
        }
        return;
    }

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

void EnsureContextMenuButton(muxc::CommandBar const& commandBar) {
    bool enabled, openMenuOnHover;
    int menuHoverDelay;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.contextMenuItem.enabled;
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
    }

    if (!enabled ||
        HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && element.Name() == kContextMenuButtonName;
        }))
        return;

    Wh_Log(L"Adding the context menu item");
    commandBar.PrimaryCommands().Append(
        CreateContextMenuButton(openMenuOnHover, menuHoverDelay));
}

void EnsureNewPlusButton(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;
    bool openMenuOnHover, enabled;
    int menuHoverDelay;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.newPlus.enabled;
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
    }

    if (!enabled || HasElement(commandBar, IsOurNewPlusButton))
        return;

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();
    uint32_t newButtonIndex = count;
    std::wstring originalIconUri;

    for (uint32_t i = 0; i < count; i++) {
        auto command = commands.GetAt(i);
        if (IsOurElement(command))
            continue;
        if (auto button = command.try_as<muxc::AppBarButton>();
            button && IdentifyDefaultButton(button) == kNewButtonIndex) {
            newButtonIndex = i;
            originalIconUri = GetButtonIconUri(button);
            break;
        }
    }

    if (newButtonIndex == count) {
        Wh_Log(L"The New button isn't in the command bar (yet)");
        return;
    }

    Wh_Log(L"Adding the New+ button");
    commands.InsertAt(
        newButtonIndex,
        CreateNewPlusButton(originalIconUri, openMenuOnHover, menuHoverDelay));
}

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;

    bool placeOnSecondary;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
    }

    if (commandBar.Name() == L"FileExplorerCommandBar") {
        EnsureNewPlusButton(commandBar);
        if (!placeOnSecondary) {
            EnsureActionButtons(commandBar);
        }
        EnsureContextMenuButton(commandBar);
    } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        if (placeOnSecondary) {
            commandBar.DefaultLabelPosition(
                muxc::CommandBarDefaultLabelPosition::Right);
            EnsureActionButtons(commandBar);
        }
    }

    ApplyDefaultButtonVisibility(commandBar);
}

void RemoveOurButtons(muxc::CommandBar const& commandBar) {
    auto commands = commandBar.PrimaryCommands();
    for (uint32_t i = commands.Size(); i > 0; i--) {
        auto command = commands.GetAt(i - 1);
        if (!IsOurElement(command))
            continue;

        if (auto button = command.try_as<muxc::AppBarButton>()) {
            if (auto flyout = button.Flyout()) {
                try {
                    flyout.Hide();
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        }
        commands.RemoveAt(i - 1);
    }
}

void OnCommandBarAdded(muxc::CommandBar const& commandBar) {
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!it->commandBar.get())
            it = g_entries.erase(it);
        else
            ++it;
    }

    for (auto const& entry : g_entries) {
        if (entry.commandBar.get() == commandBar)
            return;
    }

    CommandBarEntry entry;
    entry.commandBar = winrt::make_weak(commandBar);

    TrackRevoker(
        commandBar,
        commandBar.ActualThemeChanged(
            winrt::auto_revoke,
            [](mux::FrameworkElement const& sender, wf::IInspectable const&) {
                if (g_unloading)
                    return;

                {
                    std::lock_guard<std::mutex> lock(g_iconCacheMutex);
                    g_iconCache.clear();
                }

                if (auto cb = sender.try_as<muxc::CommandBar>()) {
                    try {
                        RemoveOurButtons(cb);
                        UpdateCommandBar(cb);
                    } catch (...) {
                        Wh_Log(L"Error %08X", winrt::to_hresult().value);
                    }
                }
            }));

    TrackRevoker(
        commandBar,
        commandBar.PointerEntered(
            winrt::auto_revoke, [](wf::IInspectable const& sender,
                                   mux::Input::PointerRoutedEventArgs const&) {
                if (g_unloading)
                    return;
                if (auto cb = sender.try_as<muxc::CommandBar>()) {
                    UpdateDynamicButtonStates(cb, nullptr);
                }
            }));

    // Auto-refresh timer: detects file selection clicks
    mux::DispatcherTimer selectionTimer;
    selectionTimer.Interval(std::chrono::milliseconds(250));

    auto weakBar = winrt::make_weak(commandBar);
    auto lastSelectedPath = std::make_shared<std::wstring>();

    auto timerToken = selectionTimer.Tick(
        [weakBar, lastSelectedPath](wf::IInspectable const&,
                                    wf::IInspectable const&) {
            if (g_unloading)
                return;

            auto cb = weakBar.get();
            if (!cb)
                return;

            HWND hWnd = GetExplorerWindowForElement(cb);
            if (!hWnd)
                return;

            if (GetForegroundWindow() != hWnd)
                return;

            ExplorerContext ctx = GetExplorerContext(hWnd);
            std::wstring currentSel = ctx.selectedPath;

            // Quick count + first item comparison
            std::wstring selectionSignature =
                std::to_wstring(ctx.allSelectedPaths.size()) + L"|" +
                ctx.selectedPath;
            if (selectionSignature != *lastSelectedPath) {
                *lastSelectedPath = selectionSignature;
                UpdateDynamicButtonStates(cb, hWnd);
            }
        });

    selectionTimer.Start();

    TrackRevoker(commandBar,
                 commandBar.Unloaded(
                     winrt::auto_revoke,
                     [selectionTimer, timerToken](wf::IInspectable const&,
                                                  mux::RoutedEventArgs const&) {
                         try {
                             selectionTimer.Stop();
                             selectionTimer.Tick(timerToken);
                         } catch (...) {
                         }
                     }));

    g_entries.push_back(std::move(entry));
    UpdateCommandBar(commandBar);
}

void RemoveButtonsForCurrentThread() {
    UnwatchVisibilityForCurrentThread();
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        auto commandBar = entry.commandBar.get();
        if (!commandBar)
            continue;
        try {
            commandBar.Loaded(entry.loadedToken);
            commandBar.PrimaryCommands().VectorChanged(
                entry.vectorChangedToken);
            RemoveOurButtons(commandBar);
            ApplyDefaultButtonVisibility(commandBar, true);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    ForgetManagedElementsForCurrentThread();
    g_pendingUpdates.clear();
    g_threadScanned = false;
    DestroyContextMenuOwnerWindowForCurrentThread();
}

void RefreshButtonsForCurrentThread() {
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<winrt::weak_ref<muxc::CommandBar>> commandBars;
    for (auto const& entry : g_entries)
        commandBars.push_back(entry.commandBar);

    for (auto const& weakCommandBar : commandBars) {
        auto commandBar = weakCommandBar.get();
        if (!commandBar)
            continue;
        try {
            RemoveOurButtons(commandBar);
            UpdateCommandBar(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

// ============================================================================
// Tree Scanning & Discovery
// ============================================================================

bool IsTargetCommandBarName(std::wstring_view name) {
    return name == L"FileExplorerCommandBar" ||
           name == L"FileExplorerSecondaryCommandBar";
}

bool FoundAllCommandBars(std::vector<muxc::CommandBar> const& commandBars) {
    bool primary = false, secondary = false;
    for (auto const& commandBar : commandBars) {
        if (commandBar.Name() == L"FileExplorerCommandBar")
            primary = true;
        else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar")
            secondary = true;
    }
    return primary && secondary;
}

void CollectCommandBars(mux::DependencyObject const& root,
                        int depth,
                        std::vector<muxc::CommandBar>* commandBars) {
    if (depth > 64)
        return;
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        if (FoundAllCommandBars(*commandBars))
            return;
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto commandBar = child.try_as<muxc::CommandBar>();
            commandBar && IsTargetCommandBarName(commandBar.Name())) {
            commandBars->push_back(std::move(commandBar));
            continue;
        }
        CollectCommandBars(child, depth + 1, commandBars);
    }
}

void ScanXamlRootForCommandBars(mux::UIElement const& element) try {
    if (g_unloading || !element)
        return;
    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot)
        return;
    auto content = xamlRoot.Content();
    if (!content)
        return;

    std::vector<muxc::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars)
        OnCommandBarAdded(commandBar);
    if (!commandBars.empty())
        g_threadScanned = true;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

void ScheduleXamlRootScan(mux::UIElement const& element) try {
    if (g_unloading || !element)
        return;
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }
    dispatcherQueue.TryEnqueue([weakElement = winrt::make_weak(element)]() {
        if (auto element = weakElement.get())
            ScanXamlRootForCommandBars(element);
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::CommandBar GetKnownCommandBarForCurrentThread() {
    for (auto const& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get())
            return commandBar;
    }
    return nullptr;
}

void ScanCurrentThreadForCommandBars() try {
    if (g_unloading)
        return;
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

void ScheduleCurrentThreadScan() try {
    if (g_unloading)
        return;
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }
    dispatcherQueue.TryEnqueue([]() { ScanCurrentThreadForCommandBars(); });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// ============================================================================
// FileExplorerExtensions.dll Hooks
// ============================================================================

using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original;

void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    Wh_Log(L">");
    CommandBarManager_CommandBar_Original(pThis, commandBar);
    if (g_unloading || !commandBar)
        return;
    try {
        EnsureContextMenuOwnerWindow();
        auto const& element =
            *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!element)
            return;
        Wh_Log(L"Command bar %s, thread %u", element.Name().c_str(),
               GetCurrentThreadId());
        OnCommandBarAdded(element);
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

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

using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
CommandBarControl_GotFocusHandler_t CommandBarControl_GotFocusHandler_Original;
CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original;

void HandleCommandBarControlGotFocus(void* sender) {
    if (g_unloading || !sender)
        return;
    if (g_threadScanned && GetKnownCommandBarForCurrentThread())
        return;
    try {
        auto const& inspectable =
            *reinterpret_cast<wf::IInspectable const*>(sender);
        if (auto element =
                inspectable ? inspectable.try_as<mux::UIElement>() : nullptr) {
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

enum class SymbolHookResult {
    Success,
    ResolutionFailed,
    NoSymbolFound,
};

SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!HookSymbols(module, fileExplorerExtensionsDllHooks,
                     ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return SymbolHookResult::ResolutionFailed;
    }

    if (!CommandBarManager_CommandBar_Original &&
        !CommandBarControl_OnApplyTemplate_Original &&
        !CommandBarControl_Wave1_OnApplyTemplate_Original) {
        Wh_Log(L"No command bar symbol was found");
        return SymbolHookResult::NoSymbolFound;
    }

    return SymbolHookResult::Success;
}

HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandle(L"FileExplorerExtensions.dll");
}

bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_symbolsHooked)
        return true;
    HMODULE module = GetFileExplorerExtensionsModuleHandle();
    if (!module)
        return true;
    if (g_symbolsHooked.exchange(true))
        return true;

    Wh_Log(L"Hooking FileExplorerExtensions.dll");
    switch (HookFileExplorerExtensionsSymbols(module)) {
        case SymbolHookResult::Success:
            break;
        case SymbolHookResult::ResolutionFailed:
            g_symbolsHooked = false;
            return false;
        case SymbolHookResult::NoSymbolFound:
            return false;
    }

    if (applyHooks)
        Wh_ApplyHookOperations();
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading || !lpLibFileName)
        return module;

    PCWSTR fileName = lpLibFileName;
    for (PCWSTR p = lpLibFileName; *p; p++) {
        if (*p == L'\\' || *p == L'/')
            fileName = p + 1;
    }

    if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(true);
    }
    return module;
}

// ============================================================================
// Multi-Thread Dispatch Helpers & Settings
// ============================================================================

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
    if (dwThreadId == 0)
        return false;
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

    if (!hook)
        return false;

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
            auto& list = *(std::vector<HWND>*)lParam;
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;
            WCHAR className[64];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                list.push_back(hWnd);
            }
            return TRUE;
        },
        (LPARAM)&hWnds);
    return hWnds;
}

constexpr int kMaxMenuDepth = 2;

ActionItem LoadActionItem(PCWSTR prefix, int depth, bool* isEmpty) {
    auto name = WindhawkUtils::StringSetting::make(L"%s.name", prefix);
    auto command = WindhawkUtils::StringSetting::make(L"%s.command", prefix);
    auto parameters =
        WindhawkUtils::StringSetting::make(L"%s.parameters", prefix);
    auto matchExt =
        WindhawkUtils::StringSetting::make(L"%s.matchExtensions", prefix);
    auto iconGlyph =
        WindhawkUtils::StringSetting::make(L"%s.iconGlyph", prefix);

    ActionItem item;
    item.name = name.get();
    item.command = command.get();
    item.parameters = parameters.get();
    item.matchExtensions = matchExt.get();
    item.icon = iconGlyph.get();
    item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
    item.showLabel = Wh_GetIntSetting(L"%s.showLabel", prefix) != 0;
    item.labelText = Wh_GetStringSetting(L"%s.labelText", prefix);
    item.separatorAfter = Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;

    if (depth < kMaxMenuDepth) {
        auto type = WindhawkUtils::StringSetting::make(L"%s.type", prefix);
        item.isMenu = wcscmp(type.get(), L"menu") == 0;
        for (int i = 0; i < 100; i++) {
            WCHAR subPrefix[256];
            swprintf(subPrefix, ARRAYSIZE(subPrefix), L"%s.subItems[%d]",
                     prefix, i);
            bool subEmpty = false;
            ActionItem subItem =
                LoadActionItem(subPrefix, depth + 1, &subEmpty);
            if (subEmpty)
                break;
            item.subItems.push_back(std::move(subItem));
        }
    }

    *isEmpty =
        item.name.empty() && item.command.empty() && item.subItems.empty();
    return item;
}

void LoadSettings() {
    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);

    g_settings.placeOnSecondaryBar =
        Wh_GetIntSetting(L"placeOnSecondaryBar") != 0;
    g_settings.disabledInsteadOfHidden =
        Wh_GetIntSetting(L"disabledInsteadOfHidden") != 0;
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

    g_settings.newPlus = NewPlusSettings{};
    g_settings.newPlus.enabled = Wh_GetIntSetting(L"newPlus.enabled") != 0;
    g_settings.newPlus.showLabel = Wh_GetIntSetting(L"newPlus.showLabel") != 0;
    g_settings.newPlus.buttonLabel =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonLabel").get();
    g_settings.newPlus.buttonIcon =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonIcon").get();
    g_settings.newPlus.templateFolder = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(L"newPlus.templateFolder").get());
    g_settings.newPlus.showIcons = Wh_GetIntSetting(L"newPlus.showIcons") != 0;
    g_settings.newPlus.showTemplatesFolderItem =
        Wh_GetIntSetting(L"newPlus.showTemplatesFolderItem") != 0;
    g_settings.newPlus.keepOriginalNewButton =
        Wh_GetIntSetting(L"newPlus.keepOriginalNewButton") != 0;

    g_settings.contextMenuItem = ContextMenuItemSettings{};
    g_settings.contextMenuItem.enabled =
        Wh_GetIntSetting(L"contextMenuItem.enabled") != 0;
    g_settings.contextMenuItem.useNilesoftShell =
        Wh_GetIntSetting(L"contextMenuItem.useNilesoftShell") != 0;
    g_settings.contextMenuItem.showLabel =
        Wh_GetIntSetting(L"contextMenuItem.showLabel") != 0;
    g_settings.contextMenuItem.buttonLabel =
        WindhawkUtils::StringSetting::make(L"contextMenuItem.buttonLabel")
            .get();
    g_settings.contextMenuItem.buttonIcon = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(L"contextMenuItem.buttonIcon")
            .get());

    g_settings.items.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR prefix[64];
        swprintf(prefix, ARRAYSIZE(prefix), L"items[%d]", i);
        bool isEmpty = false;
        ActionItem item = LoadActionItem(prefix, 0, &isEmpty);
        if (isEmpty)
            break;
        g_settings.items.push_back(std::move(item));
    }
}

// ============================================================================
// Mod Lifecycle Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(false))
            return FALSE;
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW)
            return FALSE;
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    RegisterContextMenuOwnerClass();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    HookFileExplorerExtensionsIfLoaded(true);
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { ScanCurrentThreadForCommandBars(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    DismissOpenContextMenus();

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   (DWORD)(ULONG_PTR)hWnd);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    {
        std::vector<std::pair<DWORD, HWND>> leftovers;
        {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            leftovers.assign(g_contextMenuOwners.begin(),
                             g_contextMenuOwners.end());
        }

        for (auto const& [threadId, hWnd] : leftovers) {
            if (!IsContextMenuOwnerWindow(threadId, hWnd)) {
                std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
                g_contextMenuOwners.erase(threadId);
                continue;
            }
            Wh_Log(
                L"Destroying leftover context menu owner window of thread %u",
                threadId);
            if (!RunFromWindowThread(
                    hWnd,
                    [](PVOID) {
                        DestroyContextMenuOwnerWindowForCurrentThread();
                    },
                    nullptr)) {
                Wh_Log(L"Couldn't reach thread %u", threadId);
            }
        }
    }

    if (g_contextMenuOwnerClassRegistered) {
        if (!UnregisterClassW(ContextMenuOwnerClassName().c_str(),
                              GetCurrentModuleHandle())) {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            Wh_Log(
                L"UnregisterClass failed: %u, %zu owner window(s) left behind",
                GetLastError(), g_contextMenuOwners.size());
        }
        g_contextMenuOwnerClassRegistered = false;
    }

    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        if (!g_launchThreads.empty()) {
            Wh_Log(L"Waiting for %zu launch thread(s)", g_launchThreads.size());
        }
    }

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
