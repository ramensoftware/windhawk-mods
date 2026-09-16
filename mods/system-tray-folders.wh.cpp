// ==WindhawkMod==
// @id              system-tray-folders
// @name            System Tray Folders
// @description     Group icons in the Windows 11 "Show hidden icons" tray flyout into folders
// @version         1.0.0
// @author          Ris Peng
// @github          https://github.com/RisPNG
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Parts of the module loading and window thread helpers are based on
// m417z's Windows 11 tray mods (Taskbar tray icon spacing and grid, Taskbar
// tray system icon tweaks), which are published under the same license.

// ==WindhawkModReadme==
/*
# System Tray Folders

Adds folders to the Windows 11 tray overflow flyout (the "^" / "Show hidden
icons" popup). Each folder appears as a tile in the icon grid with a preview of
the icons inside. Clicking it shows the folder's icons in place, with a back
tile to return.

![Without the mod](https://raw.githubusercontent.com/RisPNG/windhawk-mods/assets/system-tray-folders/without-mod.png) \
*Without the mod*

![Folders](https://raw.githubusercontent.com/RisPNG/windhawk-mods/assets/system-tray-folders/folders.png) \
*With folders*

![Inside a folder](https://raw.githubusercontent.com/RisPNG/windhawk-mods/assets/system-tray-folders/inside-folder.png) \
*Inside a folder*

Icons inside a folder are the real tray icons, so clicking, right clicking and
tooltips keep working. Icons can't be dragged into or out of folders; choose
them in the settings instead.

## Choosing icons

Each pattern is compared (case insensitive) with two names of every icon:

* **App name**: the program name Windows keeps for the icon, usually the
  executable's description or file name, e.g. `RazerAppEngine`,
  `Docker Desktop` or `PowerToys.Runner`. It doesn't change, so it's the most
  reliable choice.
* **Tooltip**: the text you see when hovering the icon, e.g. `NVIDIA Settings`.
  Some apps change it or leave it empty.

A pattern without wildcards must match the whole name. Use `*` for any text
and `?` for a single character, e.g. `Razer*` or `*Graphics Command Center`.

An icon goes into the first folder with a matching pattern. Icons that match
nothing stay in the main grid.

To see the exact names of your icons, turn on **Debug logging** in the mod's
**Advanced** tab, open the tray overflow once, then click **Show log output**.
Each icon is listed as `Icon: app="..." tooltip="..."`.

Windows 11 only. Tested on build 26300, where the tray is hosted in
`SystemTray.dll`. Older builds that host it in `Taskbar.View.dll` may work, but
haven't been tested.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folders:
  - - name: Folder
      $name: Folder name
    - glyph: ""
      $name: Folder glyph
      $description: >-
        Optional. A Segoe Fluent Icons code point such as E7FC, or any short
        text/emoji. Leave empty to show a preview of the icons in the folder.
    - icons: [""]
      $name: Icons
      $description: >-
        App names or tooltips of the icons in this folder, e.g. Razer* or
        Docker Desktop. Use * and ? as wildcards.
  $name: Folders
- folderPosition: start
  $name: Folder position
  $options:
  - start: Before other icons
  - end: After other icons
- showEmptyFolders: false
  $name: Show empty folders
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;

std::atomic<bool> g_unloading;
std::atomic<bool> g_keepModuleLoaded;
HMODULE g_moduleReference;

// Logging is skipped while unloading, when the mod may outlive Windhawk's
// bookkeeping for it.
#define LOG(...)                 \
    do {                         \
        if (!g_unloading) {      \
            Wh_Log(__VA_ARGS__); \
        }                        \
    } while (0)

// Runs a function when leaving the enclosing scope.
template <typename Function>
class ScopeExit {
   public:
    explicit ScopeExit(Function function) : m_function(std::move(function)) {}
    ~ScopeExit() { m_function(); }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;

   private:
    Function m_function;
};

////////////////////////////////////////////////////////////////////////////////
// Settings

struct FolderConfig {
    std::wstring name;
    std::wstring glyph;
    std::vector<std::wstring> patterns;  // Lowercase.
};

struct Settings {
    std::vector<FolderConfig> folders;
    bool foldersAtEnd = false;
    bool showEmptyFolders = false;
};

Settings g_settings;

std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuff(s.data(), (DWORD)s.size());
    }
    return s;
}

std::wstring Trim(std::wstring_view s) {
    size_t begin = 0;
    size_t end = s.size();
    while (begin < end && iswspace(s[begin])) {
        begin++;
    }
    while (end > begin && iswspace(s[end - 1])) {
        end--;
    }
    return std::wstring(s.substr(begin, end - begin));
}

// Tooltips can contain line breaks and repeated spaces; collapse them so that
// patterns can be written on a single line.
std::wstring NormalizeName(std::wstring_view s) {
    std::wstring result;
    result.reserve(s.size());
    bool lastWasSpace = false;
    for (WCHAR c : s) {
        if (iswspace(c)) {
            if (!lastWasSpace) {
                result.push_back(L' ');
            }
            lastWasSpace = true;
        } else {
            result.push_back(c);
            lastWasSpace = false;
        }
    }
    return Trim(result);
}

std::wstring ParseGlyph(std::wstring_view value) {
    std::wstring glyph = Trim(value);
    std::wstring_view hex = glyph;
    if (hex.starts_with(L"U+") || hex.starts_with(L"u+") ||
        hex.starts_with(L"0x") || hex.starts_with(L"0X")) {
        hex.remove_prefix(2);
    }
    if (hex.size() >= 4 && hex.size() <= 5 &&
        std::all_of(hex.begin(), hex.end(),
                    [](WCHAR c) { return !!iswxdigit(c); })) {
        UINT32 codePoint = wcstoul(std::wstring(hex).c_str(), nullptr, 16);
        if (codePoint <= 0xFFFF) {
            return std::wstring(1, (WCHAR)codePoint);
        }
        codePoint -= 0x10000;
        return {(WCHAR)(0xD800 + (codePoint >> 10)),
                (WCHAR)(0xDC00 + (codePoint & 0x3FF))};
    }
    return glyph;
}

Settings LoadSettings() {
    Settings settings;

    for (int i = 0; i < 100; i++) {
        FolderConfig folder;
        folder.name = Trim(
            WindhawkUtils::StringSetting::make(L"folders[%d].name", i).get());
        folder.glyph = ParseGlyph(
            WindhawkUtils::StringSetting::make(L"folders[%d].glyph", i).get());

        for (int j = 0; j < 500; j++) {
            std::wstring pattern = NormalizeName(
                WindhawkUtils::StringSetting::make(L"folders[%d].icons[%d]", i,
                                                   j)
                    .get());
            if (pattern.empty()) {
                // Keep going past a single blank entry, stop after two.
                std::wstring next = NormalizeName(
                    WindhawkUtils::StringSetting::make(L"folders[%d].icons[%d]",
                                                       i, j + 1)
                        .get());
                if (next.empty()) {
                    break;
                }
                continue;
            }
            folder.patterns.push_back(ToLower(pattern));
        }

        if (folder.name.empty() && folder.glyph.empty() &&
            folder.patterns.empty()) {
            break;
        }

        if (folder.name.empty()) {
            folder.name = L"Folder";
        }

        settings.folders.push_back(std::move(folder));
    }

    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"folderPosition");
    settings.foldersAtEnd = wcscmp(position.get(), L"end") == 0;
    settings.showEmptyFolders = Wh_GetIntSetting(L"showEmptyFolders");

    return settings;
}

bool WildcardMatch(std::wstring_view pattern, std::wstring_view text) {
    size_t p = 0;
    size_t t = 0;
    size_t starP = std::wstring_view::npos;
    size_t starT = 0;
    while (t < text.size()) {
        if (p < pattern.size() &&
            (pattern[p] == L'?' || pattern[p] == text[t])) {
            p++;
            t++;
        } else if (p < pattern.size() && pattern[p] == L'*') {
            starP = p++;
            starT = t;
        } else if (starP != std::wstring_view::npos) {
            p = starP + 1;
            t = ++starT;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == L'*') {
        p++;
    }
    return p == pattern.size();
}

// An icon matches a pattern if its app name, tooltip or accessible name does.
int FindFolderForIcon(std::initializer_list<std::wstring_view> lowerNames) {
    for (size_t i = 0; i < g_settings.folders.size(); i++) {
        for (const auto& pattern : g_settings.folders[i].patterns) {
            for (auto name : lowerNames) {
                if (!name.empty() && WildcardMatch(pattern, name)) {
                    return (int)i;
                }
            }
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// SystemTray.dll internals

// WindowsUdk.UI.Shell.INotificationAreaIcon6 (public WinRT metadata).
constexpr GUID IID_INotificationAreaIcon6 = {
    0xD76CAD80,
    0x3103,
    0x54F4,
    {0x9A, 0xBE, 0x5F, 0x04, 0x7C, 0xBF, 0xE8, 0x5C}};
constexpr int kNotificationAreaIcon6_get_AppFriendlyName = 6;

// Resolved from symbols.
void* g_overflowInterfaceVftable;
GUID* g_iconConfigurationIid;
void* g_iconConfigurationVftable;
void* g_iconConfiguration_get_DataModel;
GUID* g_notificationAreaIconsDataModelIid;
void* g_notificationAreaIconsDataModelVftable;
void* g_notificationAreaIconsDataModel_get_UDKObject;
GUID* g_iconContentViewModelIid;
void* g_imageContentViewModelBaseVftable;
void* g_imageContentViewModel_get_ToolTipText;
GUID* g_imageContentViewModelIid;
void* g_imageContentViewModelVftable;
void* g_imageContentViewModel_get_AutomationPropertyName;
void* g_imageContentViewModel_get_IconImageSource;

int g_dataModelSlot = -1;
int g_udkObjectSlot = -1;
int g_toolTipSlot = -1;
int g_nameSlot = -1;
int g_imageSlot = -1;

bool IsReadable(void* address, size_t size) {
    MEMORY_BASIC_INFORMATION mbi;
    if (!address || !VirtualQuery(address, &mbi, sizeof(mbi))) {
        return false;
    }
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
                           PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                           PAGE_EXECUTE_WRITECOPY;
    if (!(mbi.Protect & readable)) {
        return false;
    }
    return (BYTE*)address + size <= (BYTE*)mbi.BaseAddress + mbi.RegionSize;
}

int FindVtableSlot(void* vtable, void* function) {
    if (!vtable || !function) {
        return -1;
    }
    void** slots = (void**)vtable;
    for (int i = 6; i < 64; i++) {
        if (!IsReadable(&slots[i], sizeof(void*))) {
            break;
        }
        if (slots[i] == function) {
            return i;
        }
    }
    return -1;
}

// The overflow manager is a plain C++ object, so its layout is not part of any
// contract. Instead of relying on a fixed field offset, look for a field that
// points at the NotificationAreaOverflow interface, verified by its vtable,
// before calling into it.
Controls::Control FindOverflowControl(void* manager) {
    if (!manager || !g_overflowInterfaceVftable) {
        return nullptr;
    }

    for (size_t offset = 0; offset < 0x200; offset += sizeof(void*)) {
        void** field = (void**)((BYTE*)manager + offset);
        if (!IsReadable(field, sizeof(void*))) {
            break;
        }

        void* candidate = *field;
        if (!candidate || ((ULONG_PTR)candidate & (sizeof(void*) - 1)) ||
            !IsReadable(candidate, sizeof(void*)) ||
            *(void**)candidate != g_overflowInterfaceVftable) {
            continue;
        }

        Controls::Control control{nullptr};
        ((IUnknown*)candidate)
            ->QueryInterface(winrt::guid_of<Controls::Control>(),
                             winrt::put_abi(control));
        if (control) {
            return control;
        }
    }

    return nullptr;
}

Markup::IXamlMember GetXamlMember(PCWSTR typeName, PCWSTR memberName) {
    auto provider =
        Application::Current().try_as<Markup::IXamlMetadataProvider>();
    if (!provider) {
        return nullptr;
    }
    auto xamlType = provider.GetXamlType(typeName);
    if (!xamlType) {
        return nullptr;
    }
    return xamlType.GetMember(memberName);
}

struct IconInfo {
    wf::IInspectable viewModel{nullptr};
    std::wstring lowerAppName;
    std::wstring lowerToolTip;
    std::wstring lowerName;
    Media::ImageSource image{nullptr};
    int folder = -1;
};

Markup::IXamlMember g_contentVmMember{nullptr};
Markup::IXamlMember g_configurationMember{nullptr};

// Queries an interface of an internal SystemTray.dll type. The result is only
// returned if it's backed by the expected implementation vtable, which makes
// calling its methods by slot index safe.
winrt::com_ptr<IUnknown> QueryVerifiedInterface(wf::IInspectable const& object,
                                                GUID const* iid,
                                                void* expectedVftable) {
    winrt::com_ptr<IUnknown> result;
    if (!object || !iid || !expectedVftable) {
        return result;
    }
    ((IUnknown*)winrt::get_abi(object))->QueryInterface(*iid, result.put_void());
    if (result && *(void**)result.get() != expectedVftable) {
        result = nullptr;
    }
    return result;
}

void* CallAbiGetter(IUnknown* object, int slot) {
    void** vtable = *(void***)object;
    void* value = nullptr;
    HRESULT hr = ((HRESULT(WINAPI*)(void*, void**))vtable[slot])(object, &value);
    return SUCCEEDED(hr) ? value : nullptr;
}

std::wstring CallStringGetter(IUnknown* object, int slot) {
    winrt::hstring value;
    winrt::attach_abi(value, CallAbiGetter(object, slot));
    return ToLower(NormalizeName(value));
}

// The app name comes from the executable, so it stays the same regardless of
// what the app puts in its tooltip.
void ReadAppName(IconInfo& info) {
    if (g_dataModelSlot < 0 || g_udkObjectSlot < 0) {
        return;
    }

    if (!g_configurationMember) {
        g_configurationMember =
            GetXamlMember(L"SystemTray.IconViewModel", L"Configuration");
        if (!g_configurationMember) {
            return;
        }
    }

    wf::IInspectable configuration{nullptr};
    try {
        configuration = g_configurationMember.GetValue(info.viewModel);
    } catch (...) {
        return;
    }

    auto configurationInterface = QueryVerifiedInterface(
        configuration, g_iconConfigurationIid, g_iconConfigurationVftable);
    if (!configurationInterface) {
        return;
    }

    wf::IInspectable dataModel{nullptr};
    winrt::attach_abi(dataModel, CallAbiGetter(configurationInterface.get(),
                                               g_dataModelSlot));

    auto dataModelInterface =
        QueryVerifiedInterface(dataModel, g_notificationAreaIconsDataModelIid,
                               g_notificationAreaIconsDataModelVftable);
    if (!dataModelInterface) {
        return;
    }

    wf::IInspectable udkIcon{nullptr};
    winrt::attach_abi(udkIcon, CallAbiGetter(dataModelInterface.get(),
                                             g_udkObjectSlot));
    if (!udkIcon) {
        return;
    }

    // Public interface, no vtable verification needed.
    winrt::com_ptr<IUnknown> icon6;
    ((IUnknown*)winrt::get_abi(udkIcon))
        ->QueryInterface(IID_INotificationAreaIcon6, icon6.put_void());
    if (icon6) {
        info.lowerAppName = CallStringGetter(
            icon6.get(), kNotificationAreaIcon6_get_AppFriendlyName);
    }
}

void ReadIconInfo(IconInfo& info) {
    ReadAppName(info);

    if (!g_contentVmMember) {
        g_contentVmMember =
            GetXamlMember(L"SystemTray.IconViewModel", L"ContentVM");
        if (!g_contentVmMember) {
            return;
        }
    }

    wf::IInspectable contentViewModel{nullptr};
    try {
        contentViewModel = g_contentVmMember.GetValue(info.viewModel);
    } catch (...) {
        return;
    }

    if (g_toolTipSlot > 0) {
        if (auto baseInterface = QueryVerifiedInterface(
                contentViewModel, g_iconContentViewModelIid,
                g_imageContentViewModelBaseVftable)) {
            info.lowerToolTip =
                CallStringGetter(baseInterface.get(), g_toolTipSlot);
        }
    }

    // Only image icons (regular app icons) implement this interface.
    auto imageInterface =
        QueryVerifiedInterface(contentViewModel, g_imageContentViewModelIid,
                               g_imageContentViewModelVftable);
    if (!imageInterface) {
        return;
    }

    if (g_nameSlot > 0) {
        info.lowerName = CallStringGetter(imageInterface.get(), g_nameSlot);
    }

    if (g_imageSlot > 0) {
        winrt::attach_abi(info.image,
                          CallAbiGetter(imageInterface.get(), g_imageSlot));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Overflow state

struct Tile {
    Controls::Button button{nullptr};
    winrt::event_token clickToken{};
};

struct OverflowState : std::enable_shared_from_this<OverflowState> {
    void* manager = nullptr;
    winrt::weak_ref<Controls::Control> overflow;
    winrt::weak_ref<Controls::ItemsControl> itemsControl;

    wfc::IObservableVector<wf::IInspectable> originalIcons{nullptr};
    winrt::event_token originalIconsChangedToken{};
    int64_t itemsSourceCallbackToken = 0;
    bool itemsSourceCallbackRegistered = false;
    winrt::event_token actualThemeChangedToken{};
    winrt::event_token loadedToken{};

    wfc::IObservableVector<wf::IInspectable> displayed{nullptr};
    wf::IInspectable originalItemsSource{nullptr};
    Data::Binding originalItemsSourceBinding{nullptr};

    std::vector<Tile> folderTiles;
    Tile backTile;
    ElementTheme tilesTheme = ElementTheme::Default;

    int openFolder = -1;
    bool settingItemsSource = false;
    bool rebuilding = false;
    bool attached = false;
    bool detaching = false;
    bool itemsSourceOverridden = false;
};

std::vector<std::shared_ptr<OverflowState>> g_states;
std::atomic<bool> g_overflowAttachmentStarted;

void Rebuild(OverflowState& state, bool logIcons = false);
void SubscribeOriginalIcons(OverflowState& state,
                            wfc::IObservableVector<wf::IInspectable> icons);

Markup::IXamlMember g_overflowIconsMember{nullptr};

wfc::IObservableVector<wf::IInspectable> GetOverflowIcons(
    Controls::Control const& overflow) {
    if (!g_overflowIconsMember) {
        g_overflowIconsMember = GetXamlMember(
            L"SystemTray.NotificationAreaOverflow", L"OverflowIcons");
        if (!g_overflowIconsMember) {
            return nullptr;
        }
    }
    try {
        return g_overflowIconsMember.GetValue(overflow)
            .try_as<wfc::IObservableVector<wf::IInspectable>>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindDescendantByName(DependencyObject const& element,
                                      std::wstring_view name,
                                      int depth) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        if (auto frameworkElement = child.try_as<FrameworkElement>()) {
            if (frameworkElement.Name() == name) {
                return frameworkElement;
            }
        }
        if (depth > 0) {
            if (auto found = FindDescendantByName(child, name, depth - 1)) {
                return found;
            }
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Tiles

struct TileColors {
    winrt::Windows::UI::Color hover;
    winrt::Windows::UI::Color pressed;
    winrt::Windows::UI::Color plate;
    winrt::Windows::UI::Color plateBorder;
    winrt::Windows::UI::Color foreground;
};

TileColors GetTileColors(ElementTheme theme) {
    if (theme == ElementTheme::Light) {
        return {
            .hover = {0x09, 0x00, 0x00, 0x00},
            .pressed = {0x06, 0x00, 0x00, 0x00},
            .plate = {0x0F, 0x00, 0x00, 0x00},
            .plateBorder = {0x0F, 0x00, 0x00, 0x00},
            .foreground = {0xE4, 0x00, 0x00, 0x00},
        };
    }
    return {
        .hover = {0x0F, 0xFF, 0xFF, 0xFF},
        .pressed = {0x0A, 0xFF, 0xFF, 0xFF},
        .plate = {0x17, 0xFF, 0xFF, 0xFF},
        .plateBorder = {0x12, 0xFF, 0xFF, 0xFF},
        .foreground = {0xFF, 0xFF, 0xFF, 0xFF},
    };
}

std::wstring ColorToXaml(winrt::Windows::UI::Color color) {
    WCHAR buffer[16];
    swprintf(buffer, ARRAYSIZE(buffer), L"#%02X%02X%02X%02X", color.A,
             color.R, color.G, color.B);
    return buffer;
}

void GetItemSize(OverflowState& state, double* width, double* height) {
    *width = 40;
    *height = 40;
    if (auto itemsControl = state.itemsControl.get()) {
        if (auto wrapGrid =
                itemsControl.ItemsPanelRoot().try_as<Controls::WrapGrid>()) {
            double w = wrapGrid.ItemWidth();
            double h = wrapGrid.ItemHeight();
            if (w == w && w > 0) {  // Not NaN.
                *width = w;
            }
            if (h == h && h > 0) {
                *height = h;
            }
        }
    }
}

Controls::Button CreateTileButton(OverflowState& state, ElementTheme theme) {
    TileColors colors = GetTileColors(theme);
    double width;
    double height;
    GetItemSize(state, &width, &height);

    std::wstring xaml = LR"(
<Button xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        Padding="0" BorderThickness="0" Background="Transparent"
        UseSystemFocusVisuals="True" IsTabStop="True">
  <Button.Template>
    <ControlTemplate TargetType="Button">
      <Grid Background="Transparent">
        <VisualStateManager.VisualStateGroups>
          <VisualStateGroup x:Name="CommonStates">
            <VisualState x:Name="Normal"/>
            <VisualState x:Name="PointerOver">
              <VisualState.Setters>
                <Setter Target="HoverBorder.Background" Value="{HOVER}"/>
              </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="Pressed">
              <VisualState.Setters>
                <Setter Target="HoverBorder.Background" Value="{PRESSED}"/>
                <Setter Target="Presenter.Opacity" Value="0.786"/>
              </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="Disabled"/>
          </VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
        <Border x:Name="HoverBorder" CornerRadius="4" Background="Transparent"
                Width="{HOVERWIDTH}" Height="{HOVERHEIGHT}"
                HorizontalAlignment="Center" VerticalAlignment="Center"/>
        <ContentPresenter x:Name="Presenter" Content="{TemplateBinding Content}"
                          HorizontalAlignment="Center"
                          VerticalAlignment="Center"/>
      </Grid>
    </ControlTemplate>
  </Button.Template>
</Button>)";

    auto replace = [&xaml](std::wstring_view from, std::wstring const& to) {
        size_t pos = xaml.find(from);
        if (pos != std::wstring::npos) {
            xaml.replace(pos, from.size(), to);
        }
    };
    replace(L"{HOVER}", ColorToXaml(colors.hover));
    replace(L"{PRESSED}", ColorToXaml(colors.pressed));
    replace(L"{HOVERWIDTH}", std::to_wstring((int)std::max(width - 4, 16.0)));
    replace(L"{HOVERHEIGHT}",
            std::to_wstring((int)std::max(height - 10, 16.0)));

    auto button = Markup::XamlReader::Load(xaml).as<Controls::Button>();
    button.Width(width);
    button.Height(height);
    button.Foreground(Media::SolidColorBrush(colors.foreground));
    return button;
}

Controls::TextBlock CreateGlyph(std::wstring const& glyph, double fontSize) {
    Controls::TextBlock textBlock;
    textBlock.Text(glyph);
    textBlock.FontFamily(Media::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    textBlock.FontSize(fontSize);
    textBlock.HorizontalAlignment(HorizontalAlignment::Center);
    textBlock.VerticalAlignment(VerticalAlignment::Center);
    textBlock.TextLineBounds(TextLineBounds::Tight);
    textBlock.IsHitTestVisible(false);
    return textBlock;
}

UIElement CreateFolderPreview(std::vector<Media::ImageSource> const& images,
                              ElementTheme theme) {
    TileColors colors = GetTileColors(theme);

    Controls::Grid plate;
    plate.Width(22);
    plate.Height(22);
    plate.CornerRadius(CornerRadius{5, 5, 5, 5});
    plate.Background(Media::SolidColorBrush(colors.plate));
    plate.BorderBrush(Media::SolidColorBrush(colors.plateBorder));
    plate.BorderThickness(Thickness{1, 1, 1, 1});
    plate.Padding(Thickness{1, 1, 1, 1});
    plate.IsHitTestVisible(false);

    for (int i = 0; i < 2; i++) {
        Controls::RowDefinition row;
        row.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        plate.RowDefinitions().Append(row);
        Controls::ColumnDefinition column;
        column.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        plate.ColumnDefinitions().Append(column);
    }

    for (size_t i = 0; i < images.size() && i < 4; i++) {
        Controls::Image image;
        image.Source(images[i]);
        image.Width(8);
        image.Height(8);
        image.Stretch(Media::Stretch::Uniform);
        image.HorizontalAlignment(HorizontalAlignment::Center);
        image.VerticalAlignment(VerticalAlignment::Center);
        Controls::Grid::SetRow(image, (int)i / 2);
        Controls::Grid::SetColumn(image, (int)i % 2);
        plate.Children().Append(image);
    }

    return plate;
}

void OpenFolder(OverflowState& state, int folder);
void CloseFolder(OverflowState& state);

void ClearTile(Tile& tile) {
    if (tile.button) {
        tile.button.Click(tile.clickToken);
    }
    tile = {};
}

void ClearTiles(OverflowState& state) {
    for (auto& tile : state.folderTiles) {
        ClearTile(tile);
    }
    state.folderTiles.clear();
    ClearTile(state.backTile);
}

ElementTheme GetOverflowTheme(OverflowState& state) {
    if (auto overflow = state.overflow.get()) {
        return overflow.ActualTheme();
    }
    return ElementTheme::Dark;
}

Tile& EnsureFolderTile(OverflowState& state, int folder) {
    if (state.folderTiles.size() < g_settings.folders.size()) {
        state.folderTiles.resize(g_settings.folders.size());
    }

    Tile& tile = state.folderTiles[folder];
    if (!tile.button) {
        tile.button = CreateTileButton(state, state.tilesTheme);
        tile.clickToken = tile.button.Click(
            [weakState = state.weak_from_this(), folder](
                wf::IInspectable const&, RoutedEventArgs const&) {
                if (auto state = weakState.lock();
                    state && state->attached && !state->detaching &&
                    !g_unloading) {
                    OpenFolder(*state, folder);
                }
            });
    }
    return tile;
}

Tile& EnsureBackTile(OverflowState& state) {
    Tile& tile = state.backTile;
    if (!tile.button) {
        tile.button = CreateTileButton(state, state.tilesTheme);
        tile.button.Content(CreateGlyph(L"\uE72B", 14));
        tile.clickToken = tile.button.Click(
            [weakState = state.weak_from_this()](
                wf::IInspectable const&, RoutedEventArgs const&) {
                if (auto state = weakState.lock();
                    state && state->attached && !state->detaching &&
                    !g_unloading) {
                    CloseFolder(*state);
                }
            });
    }
    return tile;
}

void UpdateFolderTile(OverflowState& state,
                      Tile& tile,
                      FolderConfig const& folder,
                      std::vector<Media::ImageSource> const& images,
                      int iconCount) {
    if (!folder.glyph.empty()) {
        tile.button.Content(CreateGlyph(folder.glyph, 16));
    } else {
        tile.button.Content(CreateFolderPreview(images, state.tilesTheme));
    }

    std::wstring label = folder.name;
    Automation::AutomationProperties::SetName(tile.button, label);
    Controls::ToolTipService::SetToolTip(
        tile.button,
        winrt::box_value(label + L" (" + std::to_wstring(iconCount) + L")"));
}

////////////////////////////////////////////////////////////////////////////////
// Rebuilding the displayed items

void SetDisplayedItems(OverflowState& state,
                       std::vector<wf::IInspectable> const& items) {
    auto& displayed = state.displayed;

    // Apply minimal edits so that unchanged icons keep their containers.
    for (uint32_t i = 0; i < items.size(); i++) {
        while (i < displayed.Size() && displayed.GetAt(i) != items[i]) {
            bool existsLater = false;
            for (uint32_t j = i + 1; j < displayed.Size(); j++) {
                if (displayed.GetAt(j) == items[i]) {
                    existsLater = true;
                    break;
                }
            }
            if (!existsLater) {
                break;
            }
            displayed.RemoveAt(i);
        }

        if (i < displayed.Size() && displayed.GetAt(i) == items[i]) {
            continue;
        }

        // Make sure the item isn't present further down before inserting it,
        // a UIElement can't be in the list twice.
        for (uint32_t j = i; j < displayed.Size(); j++) {
            if (displayed.GetAt(j) == items[i]) {
                displayed.RemoveAt(j);
                break;
            }
        }
        displayed.InsertAt(i, items[i]);
    }

    while (displayed.Size() > items.size()) {
        displayed.RemoveAtEnd();
    }
}

void EnsureItemsSource(OverflowState& state) {
    if (state.itemsSourceOverridden || state.detaching || g_unloading) {
        return;
    }
    auto itemsControl = state.itemsControl.get();
    if (!itemsControl) {
        return;
    }
    if (itemsControl.ItemsSource() != state.displayed) {
        state.settingItemsSource = true;
        auto reset =
            ScopeExit([&state] { state.settingItemsSource = false; });
        itemsControl.ItemsSource(state.displayed);
    }
}

void Rebuild(OverflowState& state, bool logIcons) {
    if (state.rebuilding || state.detaching || state.itemsSourceOverridden ||
        g_unloading || !state.displayed) {
        return;
    }
    state.rebuilding = true;
    auto reset = ScopeExit([&state] { state.rebuilding = false; });

    try {
        if (auto overflow = state.overflow.get()) {
            auto icons = GetOverflowIcons(overflow);
            if (icons != state.originalIcons) {
                if (!state.originalItemsSourceBinding &&
                    state.originalItemsSource == state.originalIcons) {
                    state.originalItemsSource = icons;
                }
                SubscribeOriginalIcons(state, icons);
            }
        }
        if (!state.originalIcons) {
            return;
        }
        ElementTheme theme = GetOverflowTheme(state);
        if (theme != state.tilesTheme) {
            ClearTiles(state);
            state.tilesTheme = theme;
        }

        const auto& folders = g_settings.folders;
        if (state.folderTiles.size() != folders.size()) {
            ClearTiles(state);
            state.folderTiles.resize(folders.size());
        }

        std::vector<IconInfo> icons;
        uint32_t count = state.originalIcons.Size();
        icons.reserve(count);
        for (uint32_t i = 0; i < count; i++) {
            IconInfo info;
            info.viewModel = state.originalIcons.GetAt(i);
            ReadIconInfo(info);
            info.folder = FindFolderForIcon(
                {info.lowerAppName, info.lowerToolTip, info.lowerName});
            icons.push_back(std::move(info));
        }

        std::vector<int> folderCounts(folders.size());
        for (const auto& icon : icons) {
            if (icon.folder >= 0) {
                folderCounts[icon.folder]++;
            }
        }

        if (state.openFolder >= (int)folders.size() ||
            (state.openFolder >= 0 && folderCounts[state.openFolder] == 0 &&
             !g_settings.showEmptyFolders)) {
            state.openFolder = -1;
        }

        if (logIcons) {
            for (const auto& icon : icons) {
                LOG(L"Icon: app=\"%s\" tooltip=\"%s\" folder=%d",
                    icon.lowerAppName.c_str(),
                    (icon.lowerToolTip.empty() ? icon.lowerName
                                               : icon.lowerToolTip)
                        .c_str(),
                    icon.folder);
            }
        }

        std::vector<wf::IInspectable> items;

        if (state.openFolder >= 0) {
            Tile& backTile = EnsureBackTile(state);
            const auto& folder = folders[state.openFolder];
            Automation::AutomationProperties::SetName(backTile.button,
                                                      L"Back");
            Controls::ToolTipService::SetToolTip(
                backTile.button, winrt::box_value(L"Back (" + folder.name + L")"));
            items.push_back(backTile.button);
            for (const auto& icon : icons) {
                if (icon.folder == state.openFolder) {
                    items.push_back(icon.viewModel);
                }
            }
        } else {
            std::vector<wf::IInspectable> folderItems;
            for (size_t k = 0; k < folders.size(); k++) {
                if (folderCounts[k] == 0 && !g_settings.showEmptyFolders) {
                    continue;
                }
                std::vector<Media::ImageSource> images;
                for (const auto& icon : icons) {
                    if (icon.folder == (int)k && icon.image) {
                        images.push_back(icon.image);
                        if (images.size() == 4) {
                            break;
                        }
                    }
                }
                Tile& tile = EnsureFolderTile(state, (int)k);
                UpdateFolderTile(state, tile, folders[k], images,
                                 folderCounts[k]);
                folderItems.push_back(tile.button);
            }

            if (!g_settings.foldersAtEnd) {
                items = folderItems;
            }
            for (const auto& icon : icons) {
                if (icon.folder < 0) {
                    items.push_back(icon.viewModel);
                }
            }
            if (g_settings.foldersAtEnd) {
                items.insert(items.end(), folderItems.begin(),
                             folderItems.end());
            }
        }

        SetDisplayedItems(state, items);
        EnsureItemsSource(state);
    } catch (winrt::hresult_error const& ex) {
        LOG(L"Rebuild failed: %08X %s", ex.code(), ex.message().c_str());
    } catch (...) {
        LOG(L"Rebuild failed");
    }
}

void FocusFirstItem(OverflowState& state) {
    auto itemsControl = state.itemsControl.get();
    if (!itemsControl || state.displayed.Size() == 0) {
        return;
    }
    itemsControl.UpdateLayout();
    if (auto button = state.displayed.GetAt(0).try_as<Controls::Button>()) {
        button.Focus(FocusState::Programmatic);
    }
}

void OpenFolder(OverflowState& state, int folder) {
    LOG(L"Opening folder %d", folder);
    state.openFolder = folder;
    Rebuild(state);
    FocusFirstItem(state);
}

void CloseFolder(OverflowState& state) {
    LOG(L"Closing folder %d", state.openFolder);
    int previous = state.openFolder;
    state.openFolder = -1;
    Rebuild(state);

    if (previous >= 0 && previous < (int)state.folderTiles.size()) {
        if (auto itemsControl = state.itemsControl.get()) {
            itemsControl.UpdateLayout();
        }
        if (auto button = state.folderTiles[previous].button) {
            button.Focus(FocusState::Programmatic);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Attaching to the overflow

void SubscribeOriginalIcons(OverflowState& state,
                            wfc::IObservableVector<wf::IInspectable> icons) {
    if (state.originalIcons == icons) {
        return;
    }

    if (state.originalIcons && state.originalIconsChangedToken) {
        state.originalIcons.VectorChanged(state.originalIconsChangedToken);
        state.originalIconsChangedToken = {};
    }

    state.originalIcons = icons;
    if (!icons) {
        return;
    }

    state.originalIconsChangedToken = icons.VectorChanged(
        [weakState = state.weak_from_this()](
            wfc::IObservableVector<wf::IInspectable> const&,
            wfc::IVectorChangedEventArgs const&) {
            if (auto state = weakState.lock();
                state && state->attached && !state->detaching && !g_unloading) {
                Rebuild(*state);
            }
        });
}

void OnItemsSourceChanged(OverflowState& state) {
    if (state.settingItemsSource || state.detaching || !state.attached ||
        g_unloading) {
        return;
    }
    auto itemsControl = state.itemsControl.get();
    auto overflow = state.overflow.get();
    if (!itemsControl || !overflow ||
        itemsControl.ItemsSource() == state.displayed) {
        return;
    }

    // Adopt external property changes and only replace the shell's own list.
    LOG(L"ItemsSource replaced externally");
    auto property = Controls::ItemsControl::ItemsSourceProperty();
    auto binding = itemsControl.GetBindingExpression(property);
    state.originalItemsSourceBinding =
        binding ? binding.ParentBinding() : nullptr;
    state.originalItemsSource = itemsControl.ReadLocalValue(property);
    auto icons = GetOverflowIcons(overflow);
    state.itemsSourceOverridden = !icons || itemsControl.ItemsSource() != icons;
    SubscribeOriginalIcons(state, icons);
    Rebuild(state);
    EnsureItemsSource(state);
}

void Detach(OverflowState& state);

void Attach(OverflowState& state,
            Controls::Control const& overflow,
            Controls::ItemsControl const& itemsControl) try {
    LOG(L"Attaching to overflow");

    // From here on handlers may be registered, so unloading must clean up.
    g_overflowAttachmentStarted = true;
    state.overflow = overflow;
    state.itemsControl = itemsControl;
    auto property = Controls::ItemsControl::ItemsSourceProperty();
    auto binding = itemsControl.GetBindingExpression(property);
    state.originalItemsSourceBinding =
        binding ? binding.ParentBinding() : nullptr;
    state.originalItemsSource = itemsControl.ReadLocalValue(property);
    state.displayed = winrt::single_threaded_observable_vector<wf::IInspectable>();
    state.tilesTheme = overflow.ActualTheme();
    state.detaching = false;

    state.itemsSourceCallbackToken = itemsControl.RegisterPropertyChangedCallback(
        Controls::ItemsControl::ItemsSourceProperty(),
        [weakState = state.weak_from_this()](
            DependencyObject const&, DependencyProperty const&) {
            if (auto state = weakState.lock()) {
                OnItemsSourceChanged(*state);
            }
        });
    state.itemsSourceCallbackRegistered = true;

    state.actualThemeChangedToken = overflow.ActualThemeChanged(
        [weakState = state.weak_from_this()](
            FrameworkElement const&, wf::IInspectable const&) {
            if (auto state = weakState.lock();
                state && state->attached && !state->detaching && !g_unloading) {
                Rebuild(*state);
            }
        });

    auto icons = GetOverflowIcons(overflow);
    state.itemsSourceOverridden = itemsControl.ItemsSource() != icons;
    SubscribeOriginalIcons(state, icons);
    state.attached = true;
    Rebuild(state);
} catch (...) {
    Detach(state);
    throw;
}

void Detach(OverflowState& state) {
    LOG(L"Detaching from overflow");
    state.detaching = true;
    auto resetDetaching = ScopeExit([&state] { state.detaching = false; });
    state.attached = false;

    auto overflow = state.overflow.get();
    auto itemsControl = state.itemsControl.get();
    auto originalIcons = state.originalIcons;

    // Cleanup steps that can fail don't stop the ones after them; the first
    // failure is reported at the end. (Event handler removal can't fail.)
    std::exception_ptr error;
    auto step = [&error](auto&& function) {
        try {
            function();
            return true;
        } catch (...) {
            if (!error) {
                error = std::current_exception();
            }
            return false;
        }
    };

    if (itemsControl && state.itemsSourceCallbackRegistered) {
        step([&] {
            itemsControl.UnregisterPropertyChangedCallback(
                Controls::ItemsControl::ItemsSourceProperty(),
                state.itemsSourceCallbackToken);
            state.itemsSourceCallbackRegistered = false;
        });
    }

    if (overflow) {
        if (auto token = std::exchange(state.actualThemeChangedToken, {})) {
            overflow.ActualThemeChanged(token);
        }
        if (auto token = std::exchange(state.loadedToken, {})) {
            overflow.Loaded(token);
        }
    }

    if (state.originalIcons && state.originalIconsChangedToken) {
        state.originalIcons.VectorChanged(
            std::exchange(state.originalIconsChangedToken, {}));
    }
    ClearTiles(state);
    state.openFolder = -1;

    // Try to give the shell its own list back before reporting any failure.
    // The ItemsSource callback ignores this change while detaching.
    bool restored = true;
    if (itemsControl && state.displayed) {
        restored = step([&] {
            if (itemsControl.ItemsSource() != state.displayed) {
                return;
            }
            state.settingItemsSource = true;
            auto reset =
                ScopeExit([&state] { state.settingItemsSource = false; });
            auto property = Controls::ItemsControl::ItemsSourceProperty();
            if (state.originalItemsSourceBinding) {
                itemsControl.SetBinding(property,
                                        state.originalItemsSourceBinding);
            } else if (state.originalItemsSource ==
                       DependencyProperty::UnsetValue()) {
                itemsControl.ClearValue(property);
            } else {
                if (overflow && state.originalItemsSource == originalIcons) {
                    if (auto icons = GetOverflowIcons(overflow)) {
                        state.originalItemsSource = icons;
                    }
                }
                itemsControl.SetValue(property, state.originalItemsSource);
            }
        });
    }

    // If the shell's list couldn't be restored, show all original icons in
    // ours so navigation isn't needed to reach them.
    step([&] {
        if (state.displayed) {
            if (restored) {
                state.displayed.Clear();
            } else {
                std::vector<wf::IInspectable> icons;
                if (originalIcons) {
                    for (auto const& icon : originalIcons) {
                        icons.push_back(icon);
                    }
                }
                SetDisplayedItems(state, icons);
            }
        }
    });

    if (error) {
        std::rethrow_exception(error);
    }

    state.overflow = nullptr;
    state.itemsControl = nullptr;
    state.displayed = nullptr;
    state.originalIcons = nullptr;
    state.originalItemsSource = nullptr;
    state.originalItemsSourceBinding = nullptr;
    state.itemsSourceCallbackRegistered = false;
    state.itemsSourceOverridden = false;
    state.itemsSourceCallbackToken = 0;
    state.openFolder = -1;
}

std::shared_ptr<OverflowState> FindState(void* manager) {
    for (auto& state : g_states) {
        if (state->manager == manager) {
            return state;
        }
    }
    return nullptr;
}

std::shared_ptr<OverflowState> EnsureAttached(void* manager) {
    if (!manager || g_unloading) {
        return nullptr;
    }

    auto overflow = FindOverflowControl(manager);
    if (!overflow) {
        return nullptr;
    }

    auto states = g_states;
    for (auto& state : states) {
        if (state->manager != manager && !state->detaching &&
            !state->overflow.get()) {
            Detach(*state);
            std::erase(g_states, state);
        }
    }

    auto state = FindState(manager);
    if (state && state->detaching) {
        return nullptr;
    }
    overflow.ApplyTemplate();
    auto itemsControl =
        FindDescendantByName(overflow, L"OverflowItemsControl", 3)
            .try_as<Controls::ItemsControl>();
    if (state && state->attached && state->overflow.get() == overflow &&
        itemsControl && state->itemsControl.get() == itemsControl) {
        return state;
    }
    if (state) {
        if (!itemsControl && state->overflow.get() == overflow &&
            state->loadedToken) {
            return nullptr;
        }
        Detach(*state);
    } else {
        state = std::make_shared<OverflowState>();
        state->manager = manager;
        g_states.push_back(state);
    }
    if (!itemsControl) {
        if (!state->loadedToken) {
            LOG(L"Overflow template not ready, waiting for Loaded");
            state->overflow = overflow;
            state->detaching = false;
            g_overflowAttachmentStarted = true;
            state->loadedToken = overflow.Loaded(
                [weakState = state->weak_from_this()](
                    wf::IInspectable const& sender, RoutedEventArgs const&) {
                    auto state = weakState.lock();
                    if (!state || state->detaching || g_unloading) {
                        return;
                    }
                    if (auto overflow = state->overflow.get()) {
                        overflow.Loaded(state->loadedToken);
                    }
                    state->loadedToken = {};
                    try {
                        auto overflow = sender.as<Controls::Control>();
                        if (state->overflow.get() != overflow) {
                            return;
                        }
                        auto itemsControl =
                            FindDescendantByName(overflow, L"OverflowItemsControl", 3)
                                .try_as<Controls::ItemsControl>();
                        if (itemsControl) {
                            Attach(*state, overflow, itemsControl);
                        }
                    } catch (...) {
                        LOG(L"Loaded: attach failed");
                    }
                });
        }
        return nullptr;
    }

    Attach(*state, overflow, itemsControl);
    return state;
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using OverflowXamlIslandManager_InitializeIfNeeded_t =
    void(WINAPI*)(void* pThis);
OverflowXamlIslandManager_InitializeIfNeeded_t
    OverflowXamlIslandManager_InitializeIfNeeded_Original;
void WINAPI OverflowXamlIslandManager_InitializeIfNeeded_Hook(void* pThis) {
    OverflowXamlIslandManager_InitializeIfNeeded_Original(pThis);

    try {
        EnsureAttached(pThis);
    } catch (...) {
        LOG(L"InitializeIfNeeded: attach failed");
    }
}

using OverflowXamlIslandManager_Show_t =
    void(WINAPI*)(void* pThis, POINT pt, int inputDeviceKind);
OverflowXamlIslandManager_Show_t OverflowXamlIslandManager_Show_Original;
void WINAPI OverflowXamlIslandManager_Show_Hook(void* pThis,
                                                POINT pt,
                                                int inputDeviceKind) {
    LOG(L"Show %d,%d kind=%d", pt.x, pt.y, inputDeviceKind);

    try {
        if (auto state = EnsureAttached(pThis)) {
            // Always open at the top level, and pick up renamed icons.
            state->openFolder = -1;
            Rebuild(*state, /*logIcons=*/true);
            if (auto overflow = state->overflow.get()) {
                overflow.UpdateLayout();
            }
        }
    } catch (...) {
        LOG(L"Show: update failed");
    }

    OverflowXamlIslandManager_Show_Original(pThis, pt, inputDeviceKind);
}

void* g_lastManagerFromWindowMessage;

using OverflowXamlIslandManager_OnWindowMessage_t =
    INT64(WINAPI*)(void* pThis, HWND hWnd, UINT msg, UINT64 wParam, INT64 lParam);
OverflowXamlIslandManager_OnWindowMessage_t
    OverflowXamlIslandManager_OnWindowMessage_Original;
INT64 WINAPI OverflowXamlIslandManager_OnWindowMessage_Hook(void* pThis,
                                                          HWND hWnd,
                                                          UINT msg,
                                                          UINT64 wParam,
                                                          INT64 lParam) {
    // Only used to find the manager when the mod is loaded while explorer is
    // already running.
    g_lastManagerFromWindowMessage = pThis;
    return OverflowXamlIslandManager_OnWindowMessage_Original(pThis, hWnd, msg,
                                                             wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Threading helpers

HWND FindCurrentProcessWindow(PCWSTR wantedClassName) {
    struct Param {
        PCWSTR className;
        HWND result;
    } param{wantedClassName, nullptr};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *(Param*)lParam;
            DWORD processId;
            WCHAR className[64];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, param.className) == 0) {
                param.result = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&param);

    return param.result;
}

struct WindowThreadRequest {
    HWND window;
    std::function<void()> proc;
    std::atomic<bool> completed = false;
    std::atomic<bool> succeeded = false;
};

std::mutex g_windowThreadRequestsMutex;
std::map<UINT_PTR, std::shared_ptr<WindowThreadRequest>> g_windowThreadRequests;
std::atomic<UINT_PTR> g_nextWindowThreadRequest = 1;

bool RunFromWindowThread(HWND hWnd, std::function<void()> proc) {
    static const UINT message =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId || !message) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        try {
            proc();
            return true;
        } catch (...) {
            LOG(L"Window thread operation failed");
            return false;
        }
    }

    auto request = std::make_shared<WindowThreadRequest>();
    request->window = hWnd;
    request->proc = std::move(proc);
    UINT_PTR id = g_nextWindowThreadRequest.fetch_add(1);
    {
        std::lock_guard lock(g_windowThreadRequestsMutex);
        g_windowThreadRequests.emplace(id, request);
    }
    auto removeRequest = ScopeExit([id] {
        std::lock_guard lock(g_windowThreadRequestsMutex);
        g_windowThreadRequests.erase(id);
    });

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto& cwp = *reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp.message == message) {
                    std::shared_ptr<WindowThreadRequest> request;
                    {
                        std::lock_guard lock(g_windowThreadRequestsMutex);
                        auto it = g_windowThreadRequests.find(cwp.wParam);
                        if (it != g_windowThreadRequests.end() &&
                            it->second->window == cwp.hwnd) {
                            request = std::move(it->second);
                            g_windowThreadRequests.erase(it);
                        }
                    }
                    if (request) {
                        try {
                            request->proc();
                            request->succeeded = true;
                        } catch (...) {
                            LOG(L"Window thread operation failed");
                        }
                        request->completed = true;
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    if (!SendMessageTimeout(hWnd, message, id, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 5000, nullptr)) {
        g_keepModuleLoaded = true;
    }
    if (!UnhookWindowsHookEx(hook)) {
        // The hook procedure can still be called.
        g_keepModuleLoaded = true;
        return false;
    }
    // Check responsiveness after removing the hook. A nested message loop can
    // process this message before the earlier hook chain has returned, so it
    // doesn't undo the decision to retain the module after a timeout.
    if (!SendMessageTimeout(hWnd, WM_NULL, 0, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 5000, nullptr)) {
        g_keepModuleLoaded = true;
        return false;
    }
    bool started;
    {
        std::lock_guard lock(g_windowThreadRequestsMutex);
        started = !g_windowThreadRequests.contains(id);
    }
    if (started && !request->completed) {
        g_keepModuleLoaded = true;
    }
    return request->completed && request->succeeded;
}

HWND GetOverflowThreadWindow() {
    HWND hWnd = FindCurrentProcessWindow(L"TopLevelWindowForOverflowXamlIsland");
    if (!hWnd) {
        hWnd = FindCurrentProcessWindow(L"Shell_TrayWnd");
    }
    return hWnd;
}

// Attach to an overflow that was created before the mod was loaded.
void AttachToExistingOverflow() {
    HWND hOverflowWnd =
        FindCurrentProcessWindow(L"TopLevelWindowForOverflowXamlIsland");
    if (!hOverflowWnd) {
        LOG(L"No overflow window yet");
        return;
    }

    if (!RunFromWindowThread(
        hOverflowWnd,
        [hOverflowWnd] {
            if (g_unloading) {
                return;
            }
            g_lastManagerFromWindowMessage = nullptr;
            SendMessage(hOverflowWnd, WM_NULL, 0, 0);
            void* manager = g_lastManagerFromWindowMessage;
            LOG(L"Existing overflow manager: %p", manager);
            try {
                EnsureAttached(manager);
            } catch (...) {
                LOG(L"Attach to existing overflow failed");
            }
        })) {
        LOG(L"Attach to existing overflow could not complete");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Module loading

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Newer Taskbar.View.dll versions no longer contain the tray.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }

    return module;
}

std::atomic<bool> g_systemTrayModuleHooked;

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::InitializeIfNeeded(void))"},
            &OverflowXamlIslandManager_InitializeIfNeeded_Original,
            OverflowXamlIslandManager_InitializeIfNeeded_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::Show(struct tagPOINT,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &OverflowXamlIslandManager_Show_Original,
            OverflowXamlIslandManager_Show_Hook,
        },
        {
            {LR"(private: __int64 __cdecl winrt::SystemTray::OverflowXamlIslandManager::OnWindowMessage(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &OverflowXamlIslandManager_OnWindowMessage_Original,
            OverflowXamlIslandManager_OnWindowMessage_Hook,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::NotificationAreaOverflow,struct winrt::SystemTray::INotificationAreaOverflow>::`vftable')"},
            &g_overflowInterfaceVftable,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IIconConfiguration>)"},
            &g_iconConfigurationIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::IconConfiguration,struct winrt::SystemTray::IIconConfiguration>::`vftable')"},
            &g_iconConfigurationVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconConfiguration,struct winrt::SystemTray::IIconConfiguration>::get_DataModel(void * *))"},
            &g_iconConfiguration_get_DataModel,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::INotificationAreaIconsDataModel>)"},
            &g_notificationAreaIconsDataModelIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::NotificationAreaIconsDataModel,struct winrt::SystemTray::INotificationAreaIconsDataModel>::`vftable')"},
            &g_notificationAreaIconsDataModelVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::NotificationAreaIconsDataModel,struct winrt::SystemTray::INotificationAreaIconsDataModel>::get_UDKObject(void * *))"},
            &g_notificationAreaIconsDataModel_get_UDKObject,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IIconContentViewModel>)"},
            &g_iconContentViewModelIid,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelBaseVftable,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::get_ToolTipText(void * *))"},
            &g_imageContentViewModel_get_ToolTipText,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IImageIconContentViewModel>)"},
            &g_imageContentViewModelIid,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelVftable,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::get_AutomationPropertyName(void * *))"},
            &g_imageContentViewModel_get_AutomationPropertyName,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::get_IconImageSource(void * *))"},
            &g_imageContentViewModel_get_IconImageSource,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        LOG(L"HookSymbols failed");
        return false;
    }

    g_dataModelSlot = FindVtableSlot(g_iconConfigurationVftable,
                                     g_iconConfiguration_get_DataModel);
    g_udkObjectSlot =
        FindVtableSlot(g_notificationAreaIconsDataModelVftable,
                       g_notificationAreaIconsDataModel_get_UDKObject);
    g_toolTipSlot = FindVtableSlot(g_imageContentViewModelBaseVftable,
                                   g_imageContentViewModel_get_ToolTipText);
    g_nameSlot = FindVtableSlot(g_imageContentViewModelVftable,
                                g_imageContentViewModel_get_AutomationPropertyName);
    g_imageSlot = FindVtableSlot(g_imageContentViewModelVftable,
                                 g_imageContentViewModel_get_IconImageSource);
    LOG(L"Vtable slots: dataModel=%d udk=%d tooltip=%d name=%d image=%d",
        g_dataModelSlot, g_udkObjectSlot, g_toolTipSlot, g_nameSlot,
        g_imageSlot);

    if (g_toolTipSlot < 0 && g_nameSlot < 0) {
        LOG(L"Couldn't find the icon tooltip getters");
        return false;
    }

    return true;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        LOG(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    LOG(L">");

    // An extra reference to this module, released in Wh_ModUninit unless
    // cleanup couldn't complete.
    HMODULE module = nullptr;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                           reinterpret_cast<PCWSTR>(&g_unloading), &module)) {
        return FALSE;
    }
    auto releaseModule = ScopeExit([&module] {
        if (module) {
            FreeLibrary(module);
        }
    });

    g_settings = LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        LOG(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    g_moduleReference = std::exchange(module, nullptr);
    return TRUE;
}

void Wh_ModAfterInit() {
    LOG(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    AttachToExistingOverflow();
}

void Wh_ModBeforeUninit() {
    LOG(L">");

    g_unloading = true;

    HWND hWnd = GetOverflowThreadWindow();
    if (!hWnd) {
        if (g_overflowAttachmentStarted) {
            g_keepModuleLoaded = true;
        }
        return;
    }

    if (!RunFromWindowThread(
        hWnd,
        [] {
            auto states = std::move(g_states);
            g_states.clear();
            for (auto& state : states) {
                try {
                    Detach(*state);
                } catch (...) {
                    g_states.push_back(state);
                    g_keepModuleLoaded = true;
                }
            }
            g_contentVmMember = nullptr;
            g_configurationMember = nullptr;
            g_overflowIconsMember = nullptr;
        })) {
        g_keepModuleLoaded = true;
    }
}

void Wh_ModUninit() {
    if (g_keepModuleLoaded) {
        OutputDebugString(L"System Tray Folders: cleanup incomplete; "
                          L"retaining module until Explorer exits.\n");
    } else if (g_moduleReference) {
        FreeLibrary(std::exchange(g_moduleReference, nullptr));
    }
}

void Wh_ModSettingsChanged() {
    LOG(L">");

    Settings settings = LoadSettings();

    HWND hWnd = GetOverflowThreadWindow();
    if (!hWnd) {
        g_settings = std::move(settings);
        return;
    }

    if (!RunFromWindowThread(
        hWnd,
        [settings = std::move(settings)]() mutable {
            if (g_unloading) {
                return;
            }
            g_settings = std::move(settings);
            auto states = g_states;
            for (auto& state : states) {
                if (!state->attached || state->detaching) {
                    continue;
                }
                try {
                    ClearTiles(*state);
                    state->openFolder = -1;
                    Rebuild(*state);
                } catch (...) {
                    LOG(L"Applying settings failed");
                }
            }
        })) {
        LOG(L"Applying settings could not complete");
    }
}
