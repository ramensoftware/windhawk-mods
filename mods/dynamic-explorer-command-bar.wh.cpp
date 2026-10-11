// ==WindhawkMod==
// @id              dynamic-explorer-command-bar
// @name            Dynamic Explorer Command Bar
// @description     Add custom buttons and menus to the Windows 11 File Explorer command bar with dynamic file-type filtering, tokens, and secondary bar docking
// @version         1.0.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid
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
* **Open on Hover** — Open dropdown menus smoothly on cursor hover.

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
  While *Explorer Custom Shortcuts* provides background keyboard hotkeys, this mod brings dynamic parameter evaluation directly to the visual WinUI command bar.
* **vs. `explorer-folder-bookmarks-bar` (Maxim Fomin)**:
  *Explorer Folder Bookmarks Bar* adds a dedicated horizontal folder strip. This mod focuses on customizable action buttons, command execution, and deep integration into the native command bar.

---

### Author & Attribution

* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
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
- menuHover:
  - openOnHover: false
    $name: Open menus on hover
    $description: Open dropdown flyout menus automatically when hovering over toolbar buttons.
  - delay: 400
    $name: Hover delay (milliseconds)
    $description: Time in milliseconds to hover before opening the menu flyout.
  $name: Menu hover behavior
  $description: Configure automatic flyout expansion when hovering over dropdown buttons.
*/
// ==/WindhawkModSettings==

// clang-format on

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
std::atomic<int> g_pendingDispatches{0};

struct PendingDispatchScope {
    PendingDispatchScope() { g_pendingDispatches++; }
    ~PendingDispatchScope() { g_pendingDispatches--; }
    PendingDispatchScope(const PendingDispatchScope&) = delete;
    PendingDispatchScope& operator=(const PendingDispatchScope&) = delete;
};

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

struct {
    std::mutex mutex;
    bool placeOnSecondaryBar = true;
    bool disabledInsteadOfHidden = false;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    std::vector<ActionItem> items;
} g_settings;

// clang-format off

#pragma region winrt_hpp
#include <Unknwn.h>
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
#pragma endregion

// clang-format on

const std::wstring kButtonNamePrefix =
    std::wstring(L"WH_") + WH_MOD_ID + L"_Btn";

struct CommandBarEntry {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    mux::DispatcherTimer selectionTimer{nullptr};
    winrt::event_token selectionTickToken{};
    HWND cachedTabWnd = nullptr;
    winrt::com_ptr<IShellBrowser> cachedBrowser{nullptr};
    winrt::com_ptr<IShellView> cachedShellView{nullptr};
    int lastSelectionCount = -1;
    int lastFocusedItem = -1;
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

void StopSelectionTimer(CommandBarEntry& entry) {
    if (entry.selectionTimer) {
        try {
            entry.selectionTimer.Stop();
            entry.selectionTimer.Tick(entry.selectionTickToken);
        } catch (...) {
        }
        entry.selectionTimer = nullptr;
        entry.selectionTickToken = {};
    }
    entry.cachedBrowser = nullptr;
    entry.cachedShellView = nullptr;
    entry.cachedTabWnd = nullptr;
    entry.lastSelectionCount = -1;
    entry.lastFocusedItem = -1;
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
// Shell Browser & Selection Inspection
// ============================================================================

winrt::com_ptr<IShellBrowser> GetActiveShellBrowser(
    HWND hExplorerWnd,
    HWND hCapturedFocus = nullptr) {
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
            HWND hTab = nullptr;
            while ((hTab = FindWindowExW(hExplorerWnd, hTab,
                                         L"ShellTabWindowClass", nullptr)) !=
                   nullptr) {
                if (IsWindowVisible(hTab)) {
                    hActiveTabWnd = hTab;
                    break;
                }
            }
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

        return shellBrowser;
    }

    return nullptr;
}

winrt::com_ptr<IShellView> GetActiveShellView(HWND hExplorerWnd) {
    if (auto browser = GetActiveShellBrowser(GetAncestor(hExplorerWnd, GA_ROOT),
                                             GetFocus())) {
        IShellView* pView = nullptr;
        if (SUCCEEDED(browser->QueryActiveShellView(&pView)) && pView) {
            winrt::com_ptr<IShellView> sv;
            sv.attach(pView);
            return sv;
        }
    }
    return nullptr;
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

    result.shellView = GetActiveShellView(hExplorerWnd);
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
// Parameter Placeholders & Process Execution
// ============================================================================

bool ReplacePlaceholder(std::wstring& parameters,
                        std::wstring_view placeholder,
                        std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos)
        return true;

    while (pos != std::wstring::npos) {
        std::wstring replacement = value;
        if (!replacement.empty() && replacement.back() == L'\\' &&
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
        if (!safe.empty() && safe.back() == L'\\')
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

    std::wstring formattedFiles = FormatSelectedItemsQuoted(filesOnly);
    ReplacePlaceholder(parameters, L"\"%files\"", formattedFiles);
    ReplacePlaceholder(parameters, L"%files", formattedFiles);

    std::wstring formattedFolders = FormatSelectedItemsQuoted(foldersOnly);
    ReplacePlaceholder(parameters, L"\"%folders\"", formattedFolders);
    ReplacePlaceholder(parameters, L"%folders", formattedFolders);

    if (parameters.find(L"%n") != std::wstring::npos) {
        std::vector<std::wstring> names;
        for (const auto& p : context.allSelectedPaths) {
            PCWSTR name = PathFindFileNameW(p.c_str());
            names.push_back(name ? name : L"");
        }
        std::wstring quotedNames = FormatSelectedItemsQuoted(names);
        ReplacePlaceholder(parameters, L"\"%n\"", quotedNames);
        ReplacePlaceholder(parameters, L"%n", quotedNames);
    }

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

    ReplacePlaceholder(parameters, L"%c",
                       std::to_wstring(context.allSelectedPaths.size()));

    if (parameters.find(L"%d_name") != std::wstring::npos) {
        std::wstring dir = context.folderPath;
        while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
            dir.pop_back();
        PCWSTR namePtr = PathFindFileNameW(dir.c_str());
        ReplacePlaceholder(parameters, L"%d_name", namePtr ? namePtr : L"");
    }

    if (parameters.find(L"%sel%") != std::wstring::npos) {
        if (context.allSelectedPaths.empty())
            return std::wstring();
        std::wstring formatted =
            FormatSelectedItemsQuoted(context.allSelectedPaths);
        ReplacePlaceholder(parameters, L"\"%sel%\"", formatted);
        ReplacePlaceholder(parameters, L"%sel%", formatted);
    }

    ReplacePlaceholder(parameters, L"%path%", context.folderPath);

    if (parameters.find(L"%p") != std::wstring::npos) {
        std::wstring parent = context.folderPath;
        while (!parent.empty() &&
               (parent.back() == L'\\' || parent.back() == L'/'))
            parent.pop_back();
        std::wstring p;
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

        ReplacePlaceholder(parameters, L"%p", p);
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
            if (!safePath.empty() && safePath.back() == L'\\') {
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
    PCWSTR cmd = item.command.c_str();

    if (_wcsicmp(cmd, L"internal:TogglePreview") == 0) {
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

    if (_wcsicmp(cmd, L"internal:ToggleDetails") == 0) {
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

    if (_wcsicmp(cmd, L"internal:FolderOptions") == 0) {
        RunShellWorkOnWorkerThread([]() {
            ShellExecuteW(nullptr, L"open", L"control.exe", L"folders", nullptr,
                          SW_SHOWNORMAL);
        });
        return;
    }

    if (_wcsicmp(cmd, L"internal:OpenWith") == 0) {
        ExplorerContext context = GetExplorerContext(hWnd);
        if (!context.selectedPath.empty() &&
            PathIsDirectoryW(context.selectedPath.c_str()) == FALSE) {
            std::wstring targetPath = context.selectedPath;
            RunShellWorkOnWorkerThread([hWnd, targetPath]() {
                OPENASINFO oai = {};
                oai.pcszFile = targetPath.c_str();
                oai.oaifInFlags = OAIF_EXEC | OAIF_ALLOW_REGISTRATION;
                SHOpenWithDialog(hWnd, &oai);
            });
        }
        return;
    }

    ExplorerContext context = GetExplorerContext(hWnd);
    context.shellView = nullptr;
    RunShellWorkOnWorkerThread(
        [hWnd, item, context]() { LaunchItemForWindow(hWnd, item, context); });
}

// ============================================================================
// Icons Handling & Rendering
// ============================================================================
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
    PCWSTR cmd = item.command.c_str();

    if (_wcsicmp(cmd, L"internal:OpenWith") == 0)
        defaultGlyph = L"\uE7AC";
    else if (_wcsicmp(cmd, L"internal:TogglePreview") == 0)
        defaultGlyph = L"\uE8A1";
    else if (_wcsicmp(cmd, L"internal:ToggleDetails") == 0)
        defaultGlyph = L"\uE9F9";
    else if (_wcsicmp(cmd, L"internal:FolderOptions") == 0)
        defaultGlyph = L"\uE713";

    return CreateIconElement(item.icon, item.command, defaultGlyph);
}

bool IsOurElement(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    if (!element)
        return false;
    std::wstring_view name{element.Name()};
    return name.starts_with(kButtonNamePrefix);
}

bool HasElement(muxc::CommandBar const& commandBar,
                bool (*predicate)(muxc::ICommandBarElement const&)) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (predicate(command))
            return true;
    }
    return false;
}

// ============================================================================
// Dynamic Contextual Buttons
// ============================================================================
struct SelectionSummary {
    size_t count = 0;
    std::vector<std::wstring> extensions;
};

SelectionSummary SummarizeSelection(IShellView* psv) {
    SelectionSummary summary;
    if (!psv)
        return summary;

    winrt::com_ptr<IShellItemArray> items;
    if (FAILED(
            psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) ||
        !items)
        return summary;

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)) || count == 0)
        return summary;

    summary.count = count;
    summary.extensions.reserve(count);

    for (DWORD i = 0; i < count; i++) {
        winrt::com_ptr<IShellItem> item;
        if (FAILED(items->GetItemAt(i, item.put())) || !item)
            continue;

        SFGAOF attrs = 0;
        if (SUCCEEDED(
                item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &attrs))) {
            bool isFolder = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
            if (isFolder) {
                summary.extensions.push_back(L"folder");
            } else {
                PWSTR displayName = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,
                                                   &displayName)) &&
                    displayName) {
                    PCWSTR ext = PathFindExtensionW(displayName);
                    if (ext && *ext == L'.') {
                        summary.extensions.push_back(ToLower(ext + 1));
                    } else {
                        summary.extensions.push_back(L"file");
                    }
                    CoTaskMemFree(displayName);
                }
            }
        }
    }
    return summary;
}

bool ItemMatchesExtensionFilter(const std::wstring& filterPattern,
                                const SelectionSummary& summary) {
    std::wstring filter = TrimWhitespaceAndQuotes(filterPattern);

    if (filter.empty() || filter == L"*") {
        return true;
    }

    if (summary.count == 0) {
        return false;
    }

    std::vector<std::wstring> positiveExts;
    std::vector<std::wstring> negativeExts;
    bool matchAllExceptNegatives = false;

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

        if (!token.empty() && token.front() == L'.') {
            token.erase(token.begin());
        }

        if (token == L"dir" || token == L"directory" || token == L"folders") {
            token = L"folder";
        }

        bool isFileKeyword = (token == L"file" || token == L"files");

        if (token == L"*") {
            if (!isNegative)
                matchAllExceptNegatives = true;
            continue;
        }

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

    for (const auto& ext : summary.extensions) {
        bool isDir = (ext == L"folder");

        for (const auto& neg : negativeExts) {
            if (ext == neg || (neg == L"file" && !isDir)) {
                return false;
            }
        }

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
                               HWND hExplorerWnd,
                               const ExplorerContext* optContext = nullptr) {
    if (g_unloading || !commandBar)
        return;

    if (!hExplorerWnd) {
        hExplorerWnd = GetExplorerWindowForElement(commandBar);
    }
    if (!hExplorerWnd)
        return;

    ExplorerContext localContext;
    const ExplorerContext& context =
        optContext ? *optContext
                   : (localContext = GetExplorerContext(hExplorerWnd));

    SelectionSummary summary = SummarizeSelection(context.shellView.get());

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    bool disabledInsteadOfHidden;
    std::vector<ActionItem> currentItems;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        disabledInsteadOfHidden = g_settings.disabledInsteadOfHidden;
        currentItems = g_settings.items;
    }

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

        size_t underscore = name.rfind(L'_');
        if (underscore == std::wstring::npos)
            continue;

        int itemIndex = _wtoi(name.c_str() + underscore + 1);
        if (itemIndex < 0 || itemIndex >= (int)currentItems.size())
            continue;

        const auto& item = currentItems[itemIndex];

        bool isVisible =
            ItemMatchesExtensionFilter(item.matchExtensions, summary);

        if (disabledInsteadOfHidden) {
            button.Visibility(mux::Visibility::Visible);
            button.IsEnabled(isVisible);
        } else {
            auto targetVis = isVisible ? mux::Visibility::Visible
                                       : mux::Visibility::Collapsed;
            button.Visibility(targetVis);
            button.IsEnabled(isVisible);

            std::wstring sepName =
                kButtonNamePrefix + L"_Sep_" + std::to_wstring(itemIndex);
            for (uint32_t j = 0; j < count; j++) {
                if (auto sepEl =
                        commands.GetAt(j).try_as<mux::FrameworkElement>()) {
                    if (sepEl.Name() == sepName) {
                        sepEl.Visibility(targetVis);
                        break;
                    }
                }
            }
        }
    }
}

// ============================================================================
// CommandBar Layout & Management
// ============================================================================

void UpdateCommandBar(muxc::CommandBar const& commandBar);

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

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;

    bool placeOnSecondary;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
    }

    if (commandBar.Name() == L"FileExplorerCommandBar") {
        if (!placeOnSecondary) {
            EnsureActionButtons(commandBar);
        }
    } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        if (placeOnSecondary) {
            commandBar.DefaultLabelPosition(
                muxc::CommandBarDefaultLabelPosition::Right);
            EnsureActionButtons(commandBar);
        }
    }

    UpdateDynamicButtonStates(commandBar, nullptr);
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
        if (!it->commandBar.get()) {
            StopSelectionTimer(*it);
            it = g_entries.erase(it);
        } else {
            ++it;
        }
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

    bool placeOnSecondary;
    bool hasFilters = false;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
        for (const auto& itm : g_settings.items) {
            if (!TrimWhitespaceAndQuotes(itm.matchExtensions).empty()) {
                hasFilters = true;
                break;
            }
        }
    }

    bool isOurTargetBar =
        placeOnSecondary
            ? (commandBar.Name() == L"FileExplorerSecondaryCommandBar")
            : (commandBar.Name() == L"FileExplorerCommandBar");

    if (isOurTargetBar && hasFilters) {
        mux::DispatcherTimer selectionTimer;
        selectionTimer.Interval(std::chrono::milliseconds(250));

        auto weakBar = winrt::make_weak(commandBar);

        auto timerToken = selectionTimer.Tick(
            [weakBar](wf::IInspectable const&, wf::IInspectable const&) {
                if (g_unloading)
                    return;

                auto cb = weakBar.get();
                if (!cb)
                    return;

                HWND hWnd = GetExplorerWindowForElement(cb);
                if (!hWnd || GetForegroundWindow() != hWnd)
                    return;

                CommandBarEntry* currentEntry = nullptr;
                for (auto& ent : g_entries) {
                    if (ent.commandBar.get() == cb) {
                        currentEntry = &ent;
                        break;
                    }
                }
                if (!currentEntry)
                    return;

                HWND hActiveTab = FindWindowExW(
                    hWnd, nullptr, L"ShellTabWindowClass", nullptr);
                if (hActiveTab != currentEntry->cachedTabWnd ||
                    !currentEntry->cachedBrowser) {
                    auto browser = GetActiveShellBrowser(hWnd);

                    // Re-lookup entry in case g_entries reallocated during COM
                    // calls
                    currentEntry = nullptr;
                    for (auto& ent : g_entries) {
                        if (ent.commandBar.get() == cb) {
                            currentEntry = &ent;
                            break;
                        }
                    }
                    if (!currentEntry)
                        return;

                    currentEntry->cachedTabWnd = hActiveTab;
                    currentEntry->cachedBrowser = std::move(browser);
                    currentEntry->cachedShellView = nullptr;
                }

                if (!currentEntry->cachedBrowser)
                    return;

                winrt::com_ptr<IShellView> view;
                if (FAILED(currentEntry->cachedBrowser->QueryActiveShellView(
                        view.put())) ||
                    !view) {
                    currentEntry->cachedBrowser = nullptr;
                    currentEntry->cachedShellView = nullptr;
                    return;
                }

                auto fv = view.try_as<IFolderView>();
                int count = 0, focused = -1;
                if (!fv || FAILED(fv->ItemCount(SVGIO_SELECTION, &count)))
                    return;

                fv->GetFocusedItem(&focused);

                if (view == currentEntry->cachedShellView &&
                    count == currentEntry->lastSelectionCount &&
                    focused == currentEntry->lastFocusedItem) {
                    return;
                }

                currentEntry->cachedShellView = view;
                currentEntry->lastSelectionCount = count;
                currentEntry->lastFocusedItem = focused;

                ExplorerContext ctx;
                ctx.shellView = view;
                UpdateDynamicButtonStates(cb, hWnd, &ctx);
            });

        selectionTimer.Start();
        entry.selectionTimer = selectionTimer;
        entry.selectionTickToken = timerToken;

        TrackRevoker(commandBar,
                     commandBar.Unloaded(
                         winrt::auto_revoke, [](wf::IInspectable const& sender,
                                                mux::RoutedEventArgs const&) {
                             auto cb = sender.try_as<muxc::CommandBar>();
                             for (auto& entry : g_entries) {
                                 if (entry.commandBar.get() == cb) {
                                     StopSelectionTimer(entry);
                                     break;
                                 }
                             }
                         }));
    }

    g_entries.push_back(std::move(entry));
    UpdateCommandBar(commandBar);
}

void RemoveButtonsForCurrentThread() {
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        StopSelectionTimer(entry);
        auto commandBar = entry.commandBar.get();
        if (!commandBar)
            continue;
        try {
            RemoveOurButtons(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    g_threadScanned = false;
}

void RefreshButtonsForCurrentThread() {
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<winrt::weak_ref<muxc::CommandBar>> commandBars;
    for (auto& entry : g_entries) {
        StopSelectionTimer(entry);
        commandBars.push_back(entry.commandBar);
    }
    g_entries.clear();

    for (auto const& weakCommandBar : commandBars) {
        auto commandBar = weakCommandBar.get();
        if (!commandBar)
            continue;
        try {
            RemoveOurButtons(commandBar);
            OnCommandBarAdded(commandBar);
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
    auto pending = std::make_shared<PendingDispatchScope>();
    dispatcherQueue.TryEnqueue(
        [weakElement = winrt::make_weak(element), pending]() {
            if (g_unloading)
                return;
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
    auto pending = std::make_shared<PendingDispatchScope>();
    dispatcherQueue.TryEnqueue([pending]() {
        if (g_unloading)
            return;
        ScanCurrentThreadForCommandBars();
    });
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
    item.labelText =
        WindhawkUtils::StringSetting::make(L"%s.labelText", prefix).get();
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
    g_settings.openMenuOnHover =
        Wh_GetIntSetting(L"menuHover.openOnHover") != 0;
    int menuHoverDelay = Wh_GetIntSetting(L"menuHover.delay");
    g_settings.menuHoverDelay = menuHoverDelay >= 0 ? menuHoverDelay : 0;

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

    while (g_pendingDispatches > 0) {
        Sleep(10);
    }

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
