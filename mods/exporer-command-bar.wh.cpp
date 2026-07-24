// ==WindhawkMod==
// @id              exporer-command-bar
// @name            Explorer Command Bar
// @description     Add your own buttons and dropdown menus to the Windows 11 File Explorer command bar, and hide the built-in ones
// @version         1.0.0
// @author          DanRotaru
// @github          https://github.com/danrotaru
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

![Explorer Command Bar demo](https://cepret.md/img/placeholder.svg)

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
  buttons, in pixels.
- **Open menus on hover** - optionally open dropdowns on hover, with a
  configurable delay.
- **Rock solid** - buttons are re-applied automatically across tab switches,
  navigation, new tabs and new windows, and everything is cleanly restored when
  the mod is disabled.

## Screenshots

Custom buttons on the command bar:

![Buttons](https://cepret.md/img/placeholder.svg)

Dropdown menu with submenus:

![Dropdown menu](https://cepret.md/img/placeholder.svg)

Hiding built-in buttons and separators:

![Hide buttons](https://cepret.md/img/placeholder.svg)

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

Out of the box the mod adds:

* **Open in Terminal** - `wt.exe -d "%path%"`
* **Open in Notepad** - `notepad.exe "%sel%"`
* **Additional** ▾ (dropdown)
  * Open in VS Code - `code.exe "%path%"`
  * Open Paint - `mspaint.exe`
  * Open Calculator - `calc.exe`
  * **Commands** ▸ - `vite`, `npm init`, `npm install`, `npm run dev`,
    `npm run build`, `npm run start`
  * **AI** ▸ - `Claude`, `Codex`

All of the built-in buttons stay visible by default. Everything is configurable
in the mod settings.

## How it works

The mod injects a XAML diagnostics provider into Explorer and watches the WinUI
3 visual tree for the File Explorer command bar. When it appears, the configured
buttons are inserted and the visibility / spacing settings are applied. The mod
listens for the command bar being rebuilt so the buttons stay in place across
navigation, new tabs and new windows, and it restores the original state of any
element it touches when disabled.

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
        - name: Open in VS Code
        - command: code.exe
        - parameters: '"%path%"'
        - iconGlyph: ""
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
  $name: Item spacing (pixels)
  $description: >-
    Horizontal spacing between the command bar buttons, in pixels. -1 leaves
    the default spacing unchanged; 0 places the buttons right next to each
    other; 5 means 5 pixels between buttons, and so on.
*/
// ==/WindhawkModSettings==

#include <initguid.h>

#include <windows.h>

#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <xamlom.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::atomic<bool> g_initialized;
std::atomic<bool> g_unloading;

// {7B0A0F2C-3D1E-4C8B-9A65-2E8F41D0B7A3}
static constexpr CLSID CLSID_WindhawkTAP = {
    0x7b0a0f2c,
    0x3d1e,
    0x4c8b,
    {0x9a, 0x65, 0x2e, 0x8f, 0x41, 0xd0, 0xb7, 0xa3}};

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

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

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

void OnCommandBarAdded(muxc::CommandBar const& commandBar);

#pragma region visualtreewatcher_hpp

class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang sometimes. Creating a new thread and calling it from there fixes
    // it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"Error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    if (g_unloading)
    {
        return S_OK;
    }

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle(element.Handle);
        if (auto commandBar = inspectable.try_as<muxc::CommandBar>())
        {
            auto name = commandBar.Name();
            if (name == L"FileExplorerCommandBar" ||
                name == L"FileExplorerSecondaryCommandBar")
            {
                Wh_Log(L"%s added, thread %u", name.c_str(), GetCurrentThreadId());
                OnCommandBarAdded(commandBar);
            }
        }
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    winrt::com_ptr<IUnknown> site;
};

#pragma endregion  // tap_hpp

#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll"));
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    // Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"WinUIVisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    return hr;
}

#pragma endregion  // api_cpp

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

// Original visibility state of Explorer's own elements, captured the first
// time we touch each one, so we can restore it exactly instead of forcing a
// guessed default (which would reveal elements Explorer keeps collapsed).
std::mutex g_originalStateMutex;
std::unordered_map<void*, mux::Visibility> g_originalVisibility;
std::unordered_map<void*, muxc::CommandBarOverflowButtonVisibility>
    g_originalOverflow;
std::unordered_map<void*, mux::Thickness> g_originalMargin;
std::unordered_map<void*, double> g_originalMinWidth;

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
    if (item.command.empty()) {
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

    // Do the shell COM work off the UI thread to avoid blocking or
    // deadlocking it.
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
        CloseHandle(thread);
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

muxm::ImageSource CreateImageSourceFromIcon(HICON hIcon) {
    muxm::ImageSource result = nullptr;

    ICONINFO iconInfo{};
    if (!GetIconInfo(hIcon, &iconInfo)) {
        return result;
    }

    if (iconInfo.hbmColor) {
        BITMAP bm{};
        if (GetObject(iconInfo.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 &&
            bm.bmHeight > 0) {
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
            if (hdc) {
                if (GetDIBits(hdc, iconInfo.hbmColor, 0, height, pixels.data(),
                              &bmi, DIB_RGB_COLORS)) {
                    // Icons without an alpha channel come back fully
                    // transparent, make them opaque.
                    bool hasAlpha = false;
                    for (size_t p = 3; p < pixels.size(); p += 4) {
                        if (pixels[p]) {
                            hasAlpha = true;
                            break;
                        }
                    }

                    if (!hasAlpha) {
                        for (size_t p = 3; p < pixels.size(); p += 4) {
                            pixels[p] = 255;
                        }
                    }

                    // The XAML bitmap expects premultiplied alpha.
                    for (size_t p = 0; p < pixels.size(); p += 4) {
                        uint8_t alpha = pixels[p + 3];
                        if (alpha != 255) {
                            pixels[p] = pixels[p] * alpha / 255;
                            pixels[p + 1] = pixels[p + 1] * alpha / 255;
                            pixels[p + 2] = pixels[p + 2] * alpha / 255;
                        }
                    }

                    try {
                        muxm::Imaging::WriteableBitmap bitmap(width, height);
                        memcpy(bitmap.PixelBuffer().data(), pixels.data(),
                               pixels.size());
                        bitmap.Invalidate();
                        result = bitmap;
                    } catch (...) {
                        Wh_Log(L"Error %08X", winrt::to_hresult().value);
                    }
                }

                DeleteDC(hdc);
            }
        }
    }

    if (iconInfo.hbmColor) {
        DeleteObject(iconInfo.hbmColor);
    }
    if (iconInfo.hbmMask) {
        DeleteObject(iconInfo.hbmMask);
    }

    return result;
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

muxm::ImageSource CreateImageSourceFromHBitmap(HBITMAP hBitmap) {
    muxm::ImageSource result = nullptr;

    BITMAP bm{};
    if (!GetObject(hBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return result;
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
        return result;
    }

    if (GetDIBits(hdc, hBitmap, 0, height, pixels.data(), &bmi,
                  DIB_RGB_COLORS)) {
        // The shell returns premultiplied alpha already; just handle the
        // fully-opaque (no alpha channel) case.
        bool hasAlpha = false;
        for (size_t p = 3; p < pixels.size(); p += 4) {
            if (pixels[p]) {
                hasAlpha = true;
                break;
            }
        }

        if (!hasAlpha) {
            for (size_t p = 3; p < pixels.size(); p += 4) {
                pixels[p] = 255;
            }
        }

        try {
            muxm::Imaging::WriteableBitmap bitmap(width, height);
            memcpy(bitmap.PixelBuffer().data(), pixels.data(), pixels.size());
            bitmap.Invalidate();
            result = bitmap;
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    DeleteDC(hdc);
    return result;
}

muxm::ImageSource LoadImageSourceFromShellPath(std::wstring const& path) {
    std::wstring expanded = ExpandEnvVars(path);

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(expanded.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        Wh_Log(L"Couldn't parse shell path %s", path.c_str());
        return nullptr;
    }

    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !factory) {
        return nullptr;
    }

    SIZE size = {32, 32};
    HBITMAP hBitmap = nullptr;
    hr = factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &hBitmap);
    if (FAILED(hr) || !hBitmap) {
        return nullptr;
    }

    auto source = CreateImageSourceFromHBitmap(hBitmap);
    DeleteObject(hBitmap);
    return source;
}

// Returns nullptr if no icon could be resolved from the settings.
muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command) {
    bool isPath = !iconSetting.empty() && LooksLikeIconPath(iconSetting);

    muxm::ImageSource source = nullptr;
    if (isPath && IsShellPath(iconSetting)) {
        source = LoadImageSourceFromShellPath(iconSetting);
    } else {
        HICON hIcon = nullptr;
        if (isPath) {
            hIcon = LoadIconFromPath(iconSetting);
        } else if (iconSetting.empty() && !command.empty()) {
            hIcon = ExtractCommandIcon(command);
        }

        if (hIcon) {
            source = CreateImageSourceFromIcon(hIcon);
            DestroyIcon(hIcon);
        }
    }

    if (source) {
        muxc::ImageIcon imageIcon;
        imageIcon.Source(source);
        return imageIcon;
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
muxc::IconElement MakeCommandButtonIcon(ActionItem const& item) {
    if (item.hideIcon) {
        return nullptr;
    }
    return CreateIconElement(item.icon, item.command, L"");
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
        std::lock_guard<std::mutex> lock(g_originalStateMutex);
        auto it = g_originalVisibility.find(winrt::get_abi(element));
        if (it != g_originalVisibility.end()) {
            return it->second;
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

// Explorer shows and hides its contextual commands (Set as background, Rotate
// left/right, Extract all, ...) as the selection changes, which overwrites our
// collapse. Watching the property lets us re-apply immediately, and keeps the
// remembered original in sync so disabling the mod restores what Explorer
// wanted rather than what it happened to be when we first saw the element.
struct VisibilityWatch {
    DWORD threadId;
    void* key;
    winrt::weak_ref<mux::UIElement> element;
    int64_t token;
};

std::mutex g_watchesMutex;
std::vector<VisibilityWatch> g_watches;

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow = false);

// Hiding a group separator depends on the whole group, so a single element
// changing isn't enough to decide: re-run the pass for the command bar once the
// current batch of changes has settled. Coalesced per command bar, since
// Explorer updates a whole run of commands on every selection change.
std::mutex g_pendingRecomputeMutex;
std::unordered_set<void*> g_pendingRecompute;

void QueueVisibilityRecompute(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar) {
        return;
    }

    void* key = winrt::get_abi(commandBar);
    {
        std::lock_guard<std::mutex> lock(g_pendingRecomputeMutex);
        if (!g_pendingRecompute.insert(key).second) {
            return;  // Already queued.
        }
    }

    auto forgetPending = [key]() {
        std::lock_guard<std::mutex> lock(g_pendingRecomputeMutex);
        g_pendingRecompute.erase(key);
    };

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::
            GetForCurrentThread();
    if (!dispatcherQueue) {
        forgetPending();
        return;
    }

    if (!dispatcherQueue.TryEnqueue([weakCommandBar, forgetPending]() {
            forgetPending();

            if (g_unloading) {
                return;
            }

            if (auto commandBar = weakCommandBar.get()) {
                try {
                    ApplyDefaultButtonVisibility(commandBar);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        })) {
        forgetPending();
    }
}

void WatchVisibility(mux::UIElement const& element,
                     void* key,
                     ManagedTarget const& target,
                     winrt::weak_ref<muxc::CommandBar> const& owner) {
    {
        std::lock_guard<std::mutex> lock(g_watchesMutex);

        DWORD threadId = GetCurrentThreadId();
        for (auto it = g_watches.begin(); it != g_watches.end();) {
            if (it->key == key) {
                return;  // Already watched.
            }

            // Drop entries of elements that are gone. Only our own thread's,
            // since a weak reference to a live non-agile element can't be
            // resolved from another thread.
            if (it->threadId == threadId && !it->element.get()) {
                it = g_watches.erase(it);
            } else {
                ++it;
            }
        }
    }

    int64_t token = element.RegisterPropertyChangedCallback(
        mux::UIElement::VisibilityProperty(),
        [key, target, owner](mux::DependencyObject const& sender,
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
                std::lock_guard<std::mutex> lock(g_originalStateMutex);
                g_originalVisibility[key] = visibility;
            }

            if (visibility != mux::Visibility::Collapsed &&
                ShouldHide(target)) {
                SetVisibilityInternal(element, mux::Visibility::Collapsed);
            }

            // This element may belong to a group whose separator now has to
            // appear or disappear with it.
            QueueVisibilityRecompute(owner);
        });

    std::lock_guard<std::mutex> lock(g_watchesMutex);
    g_watches.push_back({GetCurrentThreadId(), key,
                         winrt::make_weak(element), token});
}

void UnwatchVisibilityForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<VisibilityWatch> taken;
    {
        std::lock_guard<std::mutex> lock(g_watchesMutex);
        for (auto it = g_watches.begin(); it != g_watches.end();) {
            if (it->threadId == threadId) {
                taken.push_back(std::move(*it));
                it = g_watches.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto const& watch : taken) {
        if (auto element = watch.element.get()) {
            try {
                element.UnregisterPropertyChangedCallback(
                    mux::UIElement::VisibilityProperty(), watch.token);
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
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
    void* key = winrt::get_abi(element);

    mux::Visibility original;
    {
        std::lock_guard<std::mutex> lock(g_originalStateMutex);
        auto it = g_originalVisibility.find(key);
        if (it == g_originalVisibility.end()) {
            it = g_originalVisibility.emplace(key, element.Visibility()).first;
        }
        original = it->second;
    }

    if (!forceShow) {
        WatchVisibility(element, key, target, owner);
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
    void* key = winrt::get_abi(button);

    mux::Thickness originalMargin;
    double originalMinWidth;
    {
        std::lock_guard<std::mutex> lock(g_originalStateMutex);

        auto marginIt = g_originalMargin.find(key);
        if (marginIt == g_originalMargin.end()) {
            marginIt = g_originalMargin.emplace(key, button.Margin()).first;
        }
        originalMargin = marginIt->second;

        auto minWidthIt = g_originalMinWidth.find(key);
        if (minWidthIt == g_originalMinWidth.end()) {
            minWidthIt =
                g_originalMinWidth.emplace(key, button.MinWidth()).first;
        }
        originalMinWidth = minWidthIt->second;
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
                }
            }
        }

        entries.push_back(std::move(entry));
    }

    // Explorer keeps ~20 contextual, zero-width commands (Extract, Eject, ...)
    // in the list between the View group and the overflow region. The vertical
    // line the user sees "after View" is really the separator right before our
    // first custom button, so locate that.
    uint32_t firstCustomIndex = count;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isOurs) {
            firstCustomIndex = i;
            break;
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
            } else if (firstCustomIndex < count && i + 1 == firstCustomIndex) {
                // The separator right before our custom buttons.
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
        void* key = winrt::get_abi(commandBar);
        muxc::CommandBarOverflowButtonVisibility originalOverflow;
        {
            std::lock_guard<std::mutex> lock(g_originalStateMutex);
            auto it = g_originalOverflow.find(key);
            if (it == g_originalOverflow.end()) {
                it = g_originalOverflow
                         .emplace(key, commandBar.OverflowButtonVisibility())
                         .first;
            }
            originalOverflow = it->second;
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
    AppendMenuEntries(item.subItems, menu.Items(), weakButton);

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

            // Defer the update; mutating the vector from within its own
            // change notification isn't allowed.
            auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::
                DispatcherQueue::GetForCurrentThread();
            if (dispatcherQueue) {
                dispatcherQueue.TryEnqueue([weakCommandBar]() {
                    if (auto commandBar = weakCommandBar.get()) {
                        UpdateCommandBar(commandBar);
                    }
                });
            }
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
}

void RefreshButtonsForCurrentThread() {
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
// Initialization plumbing.

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"InjectWindhawkTAP failed: %08X", hr);
        // Allow retrying, e.g. if the WinUI runtime wasn't loaded yet.
        g_initialized = false;
    }
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

bool IsTargetTriggerWindow(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    if (_wcsicmp(className, L"CabinetWClass") == 0) {
        return true;
    }

    // The WinUI content bridge windows of a File Explorer window. By the time
    // they're created, the WinUI runtime is loaded, so use them as a fallback
    // trigger in case injection failed for the top-level window.
    if (_wcsicmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") ==
        0) {
        HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
        WCHAR rootClassName[64];
        if (hRootWnd &&
            GetClassName(hRootWnd, rootClassName, ARRAYSIZE(rootClassName)) &&
            _wcsicmp(rootClassName, L"CabinetWClass") == 0) {
            return true;
        }
    }

    return false;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && !g_unloading && IsTargetTriggerWindow(hWnd)) {
        InitializeSettingsAndTap();
    }

    return hWnd;
}

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
    PCWSTR type = Wh_GetStringSetting(L"%s.type", prefix);
    PCWSTR name = Wh_GetStringSetting(L"%s.name", prefix);
    PCWSTR command = Wh_GetStringSetting(L"%s.command", prefix);
    PCWSTR parameters = Wh_GetStringSetting(L"%s.parameters", prefix);
    PCWSTR iconGlyph = Wh_GetStringSetting(L"%s.iconGlyph", prefix);

    ActionItem item;
    item.name = name;
    item.command = command;
    item.parameters = parameters;
    item.icon = iconGlyph;
    item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
    item.isMenu = wcscmp(type, L"menu") == 0;
    item.separatorAfter =
        Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;

    Wh_FreeStringSetting(type);
    Wh_FreeStringSetting(name);
    Wh_FreeStringSetting(command);
    Wh_FreeStringSetting(parameters);
    Wh_FreeStringSetting(iconGlyph);

    if (depth < kMaxMenuDepth) {
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

    if (g_settings.items.empty()) {
        ActionItem item;
        item.name = L"Open in Terminal";
        item.command = L"wt.exe";
        item.parameters = L"-d \"%path%\"";
        item.icon = L"";
        g_settings.items.push_back(std::move(item));
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!GetFileExplorerWnds().empty()) {
        Wh_Log(L"Found existing File Explorer windows, injecting TAP");
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    UninitializeSettingsAndTap();

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X", (DWORD)(ULONG_PTR)hWnd);
        RunFromWindowThread(
            hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); }, nullptr);
    }

    {
        std::lock_guard<std::mutex> lock(g_watchesMutex);
        g_watches.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_pendingRecomputeMutex);
        g_pendingRecompute.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_originalStateMutex);
        g_originalVisibility.clear();
        g_originalOverflow.clear();
        g_originalMargin.clear();
        g_originalMinWidth.clear();
    }

    std::lock_guard<std::mutex> lock(g_entriesMutex);
    g_entries.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { RefreshButtonsForCurrentThread(); }, nullptr);
    }
}
