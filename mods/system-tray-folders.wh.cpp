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
tooltips keep working. Dragging icons between the taskbar and the flyout and
reordering them work as usual, and an icon dragged back into the flyout returns
to its folder. Which folder an icon belongs to comes from the settings, not
from dragging.

## Choosing icons

Each pattern is compared (case insensitive) with three names of every icon:

* **App name**: the program name Windows keeps for the icon, usually the
  executable's description or file name, e.g. `RazerAppEngine`,
  `Docker Desktop` or `PowerToys.Runner`. It doesn't change, so it's the most
  reliable choice.
* **Tooltip**: the text you see when hovering the icon, e.g. `NVIDIA Settings`.
  Some apps change it or leave it empty. A changed tooltip is picked up the
  next time the flyout opens.
* **Accessible name**: a name Windows keeps for the icon for accessibility
  tools. It's usually the same as one of the other two names.

A few icons aren't image icons; they have no tooltip or accessible name, so
only their app name can match.

A pattern without wildcards must match the whole name. Use `*` for any text
and `?` for a single character, e.g. `Razer*` or `*Graphics Command Center`.

An icon goes into the first folder with a matching pattern. Icons that match
nothing stay in the main grid. Until a folder has icons (or **Show empty
folders** is on), the mod unloads itself and leaves the flyout alone.

An empty row in the **Folders** or **Icons** list is skipped, but two empty rows
in a row end the list.

To see the exact names of your icons, set **Debug logging** to **Mod logs** in
the mod's **Advanced** tab, open the tray overflow once, then click **Show log
output**.
Each icon is listed as `Icon: app="..." tooltip="..." name="..." folder=N`,
where `N` is the folder it went into, counting from 0, or -1 for none.

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
        Optional. A Segoe Fluent Icons code point such as U+E7FC, or any short
        text/emoji. Leave empty to show a preview of the icons in the folder,
        or a folder icon when there's nothing to preview.
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
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
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
    if (!hex.starts_with(L"U+") && !hex.starts_with(L"u+") &&
        !hex.starts_with(L"0x") && !hex.starts_with(L"0X")) {
        return glyph;
    }
    hex.remove_prefix(2);
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

    for (int i = 0, blankFolders = 0; i < 100 && blankFolders < 2; i++) {
        FolderConfig folder;
        folder.name = Trim(
            WindhawkUtils::StringSetting::make(L"folders[%d].name", i).get());
        folder.glyph = ParseGlyph(
            WindhawkUtils::StringSetting::make(L"folders[%d].glyph", i).get());

        for (int j = 0, blankIcons = 0; j < 500 && blankIcons < 2; j++) {
            std::wstring pattern = NormalizeName(
                WindhawkUtils::StringSetting::make(L"folders[%d].icons[%d]", i,
                                                   j)
                    .get());
            if (pattern.empty()) {
                // Keep going past a single blank entry, stop after two.
                blankIcons++;
                continue;
            }
            blankIcons = 0;
            folder.patterns.push_back(ToLower(pattern));
        }

        if (folder.name.empty() && folder.glyph.empty() &&
            folder.patterns.empty()) {
            blankFolders++;
            continue;
        }
        blankFolders = 0;

        if (folder.name.empty()) {
            folder.name = L"Folder";
        }

        settings.folders.push_back(std::move(folder));
    }

    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"folderPosition");
    settings.foldersAtEnd = wcscmp(position.get(), L"end") == 0;
    settings.showEmptyFolders = Wh_GetIntSetting(L"showEmptyFolders");

    if (!settings.showEmptyFolders &&
        std::all_of(settings.folders.begin(), settings.folders.end(),
                    [](FolderConfig const& folder) {
                        return folder.patterns.empty();
                    })) {
        settings.folders.clear();
    }

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

// WindowsUdk.UI.Shell.INotificationAreaIcon6, from the public WinRT metadata in
// windowsudk.winmd, which is also where the IID and the member order come from.
constexpr GUID IID_INotificationAreaIcon6 = {
    0xD76CAD80,
    0x3103,
    0x54F4,
    {0x9A, 0xBE, 0x5F, 0x04, 0x7C, 0xBF, 0xE8, 0x5C}};
constexpr int kNotificationAreaIcon6_get_AppFriendlyName = 6;

// Resolved from symbols.
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
    std::wstring appName;
    std::wstring toolTip;
    std::wstring name;
    std::wstring lowerAppName;
    std::wstring lowerToolTip;
    std::wstring lowerName;
    Media::ImageSource image{nullptr};
    int folder = -1;
};

[[clang::no_destroy]] Markup::IXamlMember g_contentVmMember{nullptr};
[[clang::no_destroy]] Markup::IXamlMember g_configurationMember{nullptr};

// Queries an interface of an internal SystemTray.dll type. The result is only
// returned if it's backed by the expected implementation vtable, which makes
// calling that implementation's methods, resolved from symbols, on it safe.
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

void* CallAbiGetter(IUnknown* object, void* getter) {
    void* value = nullptr;
    HRESULT hr = ((HRESULT(WINAPI*)(void*, void**))getter)(object, &value);
    return SUCCEEDED(hr) ? value : nullptr;
}

std::wstring CallStringGetter(IUnknown* object, void* getter) {
    winrt::hstring value;
    winrt::attach_abi(value, CallAbiGetter(object, getter));
    return NormalizeName(value);
}

// The app name comes from the executable, so it stays the same regardless of
// what the app puts in its tooltip.
void ReadAppName(IconInfo& info) {
    if (!g_iconConfiguration_get_DataModel ||
        !g_notificationAreaIconsDataModel_get_UDKObject) {
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
    winrt::attach_abi(dataModel,
                      CallAbiGetter(configurationInterface.get(),
                                    g_iconConfiguration_get_DataModel));

    auto dataModelInterface =
        QueryVerifiedInterface(dataModel, g_notificationAreaIconsDataModelIid,
                               g_notificationAreaIconsDataModelVftable);
    if (!dataModelInterface) {
        return;
    }

    wf::IInspectable udkIcon{nullptr};
    winrt::attach_abi(
        udkIcon, CallAbiGetter(dataModelInterface.get(),
                               g_notificationAreaIconsDataModel_get_UDKObject));
    if (!udkIcon) {
        return;
    }

    // Public interface, no vtable verification needed.
    winrt::com_ptr<IUnknown> icon6;
    ((IUnknown*)winrt::get_abi(udkIcon))
        ->QueryInterface(IID_INotificationAreaIcon6, icon6.put_void());
    if (icon6) {
        void* getter =
            (*(void***)icon6.get())[kNotificationAreaIcon6_get_AppFriendlyName];
        info.appName = CallStringGetter(icon6.get(), getter);
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

    if (g_imageContentViewModel_get_ToolTipText) {
        if (auto baseInterface = QueryVerifiedInterface(
                contentViewModel, g_iconContentViewModelIid,
                g_imageContentViewModelBaseVftable)) {
            info.toolTip =
                CallStringGetter(baseInterface.get(),
                                 g_imageContentViewModel_get_ToolTipText);
        }
    }

    // Only image icons (regular app icons) implement this interface.
    auto imageInterface =
        QueryVerifiedInterface(contentViewModel, g_imageContentViewModelIid,
                               g_imageContentViewModelVftable);
    if (!imageInterface) {
        return;
    }

    if (g_imageContentViewModel_get_AutomationPropertyName) {
        info.name = CallStringGetter(
            imageInterface.get(),
            g_imageContentViewModel_get_AutomationPropertyName);
    }

    if (g_imageContentViewModel_get_IconImageSource) {
        winrt::attach_abi(
            info.image,
            CallAbiGetter(imageInterface.get(),
                          g_imageContentViewModel_get_IconImageSource));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Overflow state

struct Tile {
    Controls::Button button{nullptr};
    winrt::event_token clickToken{};
    std::optional<std::wstring> contentGlyph;
};

struct OverflowState : std::enable_shared_from_this<OverflowState> {
    void* manager = nullptr;
    winrt::weak_ref<Controls::Control> overflow;
    winrt::weak_ref<Controls::ItemsControl> itemsControl;

    wfc::IObservableVector<wf::IInspectable> originalIcons{nullptr};
    winrt::event_token originalIconsChangedToken{};
    int64_t itemsSourceCallbackToken = 0;
    bool itemsSourceCallbackRegistered = false;
    winrt::event_token loadedToken{};
    HWND islandWindow = nullptr;

    wfc::IObservableVector<wf::IInspectable> displayed{nullptr};
    wf::IInspectable originalItemsSource{nullptr};
    Data::Binding originalItemsSourceBinding{nullptr};

    std::vector<Tile> folderTiles;
    Tile backTile;
    double tileWidth = 0;
    double tileHeight = 0;

    std::unordered_map<void*, std::shared_ptr<IconInfo>> iconCache;

    int openFolder = -1;
    bool settingItemsSource = false;
    bool rebuilding = false;
    bool attached = false;
    bool detaching = false;
    bool itemsSourceOverridden = false;
};

[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<OverflowState>>>
    g_states{std::in_place};
std::atomic<DWORD> g_overflowThreadId;

void Rebuild(OverflowState& state, bool opening = false);
void SubscribeOriginalIcons(OverflowState& state,
                            wfc::IObservableVector<wf::IInspectable> icons);

[[clang::no_destroy]] Markup::IXamlMember g_overflowIconsMember{nullptr};

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
                                      std::wstring_view name) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        if (auto frameworkElement = child.try_as<FrameworkElement>()) {
            if (frameworkElement.Name() == name) {
                return frameworkElement;
            }
        }
        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Tiles

void GetItemSize(OverflowState& state, double* width, double* height) {
    *width = 40;
    *height = 40;
    if (auto itemsControl = state.itemsControl.get()) {
        if (auto wrapGrid =
                itemsControl.ItemsPanelRoot().try_as<Controls::WrapGrid>()) {
            double w = wrapGrid.ItemWidth();
            double h = wrapGrid.ItemHeight();
            if (!std::isnan(w) && w > 0) {
                *width = w;
            }
            if (!std::isnan(h) && h > 0) {
                *height = h;
            }
        }
    }
}

Controls::Button CreateTileButton(double width, double height) {
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
                <Setter Target="HoverBorder.Background" Value="{ThemeResource SubtleFillColorSecondaryBrush}"/>
              </VisualState.Setters>
            </VisualState>
            <VisualState x:Name="Pressed">
              <VisualState.Setters>
                <Setter Target="HoverBorder.Background" Value="{ThemeResource SubtleFillColorTertiaryBrush}"/>
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
    replace(L"{HOVERWIDTH}", std::to_wstring((int)std::max(width - 4, 16.0)));
    replace(L"{HOVERHEIGHT}",
            std::to_wstring((int)std::max(height - 10, 16.0)));

    auto button = Markup::XamlReader::Load(xaml).as<Controls::Button>();
    button.Width(width);
    button.Height(height);
    return button;
}

Controls::TextBlock CreateGlyph(std::wstring const& glyph, double fontSize) {
    Controls::TextBlock textBlock;
    textBlock.Text(glyph);
    textBlock.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
    textBlock.FontSize(fontSize);
    textBlock.HorizontalAlignment(HorizontalAlignment::Center);
    textBlock.VerticalAlignment(VerticalAlignment::Center);
    textBlock.TextLineBounds(TextLineBounds::Tight);
    textBlock.IsHitTestVisible(false);
    return textBlock;
}

UIElement CreateFolderPreview(double itemSize) {
    std::wstring xaml = LR"(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
      Width="{PLATE}" Height="{PLATE}" CornerRadius="{RADIUS}"
      BorderThickness="1" Padding="1"
      Background="{ThemeResource ControlFillColorDefaultBrush}"
      BorderBrush="{ThemeResource SubtleFillColorTertiaryBrush}"
      IsHitTestVisible="False">
  <Grid.RowDefinitions>
    <RowDefinition Height="*"/>
    <RowDefinition Height="*"/>
  </Grid.RowDefinitions>
  <Grid.ColumnDefinitions>
    <ColumnDefinition Width="*"/>
    <ColumnDefinition Width="*"/>
  </Grid.ColumnDefinitions>
  <Image Grid.Row="0" Grid.Column="0" Width="{ICON}" Height="{ICON}" Stretch="Uniform"
         HorizontalAlignment="Center" VerticalAlignment="Center"/>
  <Image Grid.Row="0" Grid.Column="1" Width="{ICON}" Height="{ICON}" Stretch="Uniform"
         HorizontalAlignment="Center" VerticalAlignment="Center"/>
  <Image Grid.Row="1" Grid.Column="0" Width="{ICON}" Height="{ICON}" Stretch="Uniform"
         HorizontalAlignment="Center" VerticalAlignment="Center"/>
  <Image Grid.Row="1" Grid.Column="1" Width="{ICON}" Height="{ICON}" Stretch="Uniform"
         HorizontalAlignment="Center" VerticalAlignment="Center"/>
</Grid>)";

    auto replace = [&xaml](std::wstring_view from, std::wstring const& to) {
        for (size_t pos = xaml.find(from); pos != std::wstring::npos;
             pos = xaml.find(from, pos + to.size())) {
            xaml.replace(pos, from.size(), to);
        }
    };
    replace(L"{PLATE}", std::to_wstring((int)std::max(itemSize * 0.55, 12.0)));
    replace(L"{RADIUS}", std::to_wstring((int)std::max(itemSize * 0.125, 3.0)));
    replace(L"{ICON}", std::to_wstring((int)std::max(itemSize * 0.2, 5.0)));

    return Markup::XamlReader::Load(xaml).as<UIElement>();
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

Tile& EnsureFolderTile(OverflowState& state, int folder) {
    Tile& tile = state.folderTiles[folder];
    if (!tile.button) {
        tile.button = CreateTileButton(state.tileWidth, state.tileHeight);
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
        tile.button = CreateTileButton(state.tileWidth, state.tileHeight);
        tile.button.Content(CreateGlyph(
            L"\uE72B",
            std::max(std::min(state.tileWidth, state.tileHeight) * 0.35, 8.0)));
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
    std::wstring glyph = folder.glyph;
    if (glyph.empty() && images.empty()) {
        glyph = L"\uE8B7";
    }

    if (tile.contentGlyph != glyph) {
        double itemSize = std::min(state.tileWidth, state.tileHeight);
        if (glyph.empty()) {
            tile.button.Content(CreateFolderPreview(itemSize));
        } else {
            tile.button.Content(CreateGlyph(glyph, std::max(itemSize * 0.4, 8.0)));
        }
        tile.contentGlyph = glyph;
    }

    if (glyph.empty()) {
        auto previews = tile.button.Content().as<Controls::Panel>().Children();
        for (uint32_t i = 0; i < previews.Size(); i++) {
            previews.GetAt(i).as<Controls::Image>().Source(
                i < images.size() ? images[i] : nullptr);
        }
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

void Rebuild(OverflowState& state, bool opening) {
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

        const auto& folders = g_settings.folders;
        double tileWidth;
        double tileHeight;
        GetItemSize(state, &tileWidth, &tileHeight);
        if (state.folderTiles.size() != folders.size() ||
            state.tileWidth != tileWidth || state.tileHeight != tileHeight) {
            ClearTiles(state);
            state.folderTiles.resize(folders.size());
            state.tileWidth = tileWidth;
            state.tileHeight = tileHeight;
        }

        bool logIcons = opening || state.iconCache.empty();
        if (opening) {
            state.iconCache.clear();
        }
        std::unordered_map<void*, std::shared_ptr<IconInfo>> iconCache;
        std::vector<std::shared_ptr<IconInfo>> icons;
        uint32_t count = state.originalIcons.Size();
        icons.reserve(count);
        for (uint32_t i = 0; i < count; i++) {
            auto viewModel = state.originalIcons.GetAt(i);
            void* key = winrt::get_abi(viewModel);
            std::shared_ptr<IconInfo> info;
            if (auto it = state.iconCache.find(key);
                it != state.iconCache.end()) {
                info = it->second;
            } else {
                info = std::make_shared<IconInfo>();
                info->viewModel = viewModel;
                ReadIconInfo(*info);
                info->lowerAppName = ToLower(info->appName);
                info->lowerToolTip = ToLower(info->toolTip);
                info->lowerName = ToLower(info->name);
            }
            info->folder = FindFolderForIcon(
                {info->lowerAppName, info->lowerToolTip, info->lowerName});
            iconCache.emplace(key, info);
            icons.push_back(std::move(info));
        }
        state.iconCache = std::move(iconCache);

        std::vector<int> folderCounts(folders.size());
        for (const auto& icon : icons) {
            if (icon->folder >= 0) {
                folderCounts[icon->folder]++;
            }
        }

        if (state.openFolder >= (int)folders.size() ||
            (state.openFolder >= 0 && folderCounts[state.openFolder] == 0 &&
             !g_settings.showEmptyFolders)) {
            state.openFolder = -1;
        }

        if (logIcons) {
            for (const auto& icon : icons) {
                Wh_Log(L"Icon: app=\"%s\" tooltip=\"%s\" name=\"%s\" folder=%d",
                       icon->appName.c_str(),
                       icon->toolTip.c_str(), icon->name.c_str(),
                       icon->folder);
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
                if (icon->folder == state.openFolder) {
                    items.push_back(icon->viewModel);
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
                    if (icon->folder == (int)k && icon->image) {
                        images.push_back(icon->image);
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
                if (icon->folder < 0) {
                    items.push_back(icon->viewModel);
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
        Wh_Log(L"Rebuild failed: %08X %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Rebuild failed");
    }
}

void RefreshTileSize(OverflowState& state) {
    auto overflow = state.overflow.get();
    if (!overflow) {
        return;
    }

    overflow.UpdateLayout();

    double width;
    double height;
    GetItemSize(state, &width, &height);
    if (width != state.tileWidth || height != state.tileHeight) {
        Rebuild(state);
        overflow.UpdateLayout();
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
    Wh_Log(L"Opening folder %d", folder);
    state.openFolder = folder;
    Rebuild(state);
    FocusFirstItem(state);
}

void CloseFolder(OverflowState& state) {
    Wh_Log(L"Closing folder %d", state.openFolder);
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
            auto state = weakState.lock();
            if (!state || !state->attached || state->detaching || g_unloading) {
                return;
            }
            if (state->islandWindow && !IsWindowVisible(state->islandWindow)) {
                return;
            }
            Rebuild(*state);
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
    Wh_Log(L"ItemsSource replaced externally");
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
    Wh_Log(L"Attaching to overflow");

    state.overflow = overflow;
    state.itemsControl = itemsControl;
    auto property = Controls::ItemsControl::ItemsSourceProperty();
    auto binding = itemsControl.GetBindingExpression(property);
    state.originalItemsSourceBinding =
        binding ? binding.ParentBinding() : nullptr;
    state.originalItemsSource = itemsControl.ReadLocalValue(property);
    state.displayed = winrt::single_threaded_observable_vector<wf::IInspectable>();
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
    Wh_Log(L"Detaching from overflow");
    state.detaching = true;
    auto resetDetaching = ScopeExit([&state] { state.detaching = false; });
    state.attached = false;

    auto overflow = state.overflow.get();
    auto itemsControl = state.itemsControl.get();
    auto originalIcons = state.originalIcons;

    // Cleanup steps that can fail don't stop the ones after them; the first
    // failure is reported at the end. (Removing an event handler doesn't
    // throw, the C++/WinRT event removers are noexcept.)
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

    if (auto token = std::exchange(state.loadedToken, {}); token && overflow) {
        overflow.Loaded(token);
    }

    if (state.originalIcons && state.originalIconsChangedToken) {
        state.originalIcons.VectorChanged(
            std::exchange(state.originalIconsChangedToken, {}));
    }
    ClearTiles(state);
    state.openFolder = -1;

    // Try to give the shell its own list back before reporting any failure. If
    // the original value can't be put back, fall back to the shell's current
    // list, then to no value, so that the mod's list isn't left installed. The
    // ItemsSource callback ignores these changes while detaching.
    if (itemsControl && state.displayed) {
        auto property = Controls::ItemsControl::ItemsSourceProperty();
        auto replaceItemsSource = [&](auto&& apply) {
            step([&] {
                if (itemsControl.ItemsSource() != state.displayed) {
                    return;
                }
                state.settingItemsSource = true;
                auto reset =
                    ScopeExit([&state] { state.settingItemsSource = false; });
                apply();
                itemsControl.UpdateLayout();
            });
        };

        replaceItemsSource([&] {
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

        replaceItemsSource([&] {
            if (auto icons = overflow ? GetOverflowIcons(overflow) : nullptr) {
                itemsControl.SetValue(property, icons);
            } else {
                itemsControl.ClearValue(property);
            }
        });
    }

    // If the mod's list is still installed anyway, show all original icons in
    // it so navigation isn't needed to reach them.
    step([&] {
        if (!state.displayed) {
            return;
        }
        if (itemsControl && itemsControl.ItemsSource() == state.displayed) {
            std::vector<wf::IInspectable> icons;
            if (originalIcons) {
                for (auto const& icon : originalIcons) {
                    icons.push_back(icon);
                }
            }
            SetDisplayedItems(state, icons);
        } else {
            state.displayed.Clear();
        }
    });

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
    state.islandWindow = nullptr;
    state.iconCache.clear();
    state.tileWidth = 0;
    state.tileHeight = 0;

    if (error) {
        std::rethrow_exception(error);
    }
}

std::shared_ptr<OverflowState> FindState(void* manager) {
    for (auto& state : *g_states) {
        if (state->manager == manager) {
            return state;
        }
    }
    return nullptr;
}

std::shared_ptr<OverflowState> EnsureAttached(
    void* manager,
    Controls::Control const& overflow) {
    if (!manager || !overflow || g_unloading) {
        return nullptr;
    }

    g_overflowThreadId = GetCurrentThreadId();
    if (g_settings.folders.empty()) {
        return nullptr;
    }

    auto state = FindState(manager);
    if (state && state->detaching) {
        return nullptr;
    }
    if (state && state->attached && state->overflow.get() == overflow &&
        state->itemsControl.get()) {
        return state;
    }

    auto states = *g_states;
    for (auto& other : states) {
        if (other->manager != manager && !other->detaching &&
            !other->overflow.get()) {
            std::erase(*g_states, other);
            Detach(*other);
        }
    }

    overflow.ApplyTemplate();
    auto itemsControl =
        FindDescendantByName(overflow, L"OverflowItemsControl")
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
        g_states->push_back(state);
    }
    if (!itemsControl) {
        if (!state->loadedToken) {
            Wh_Log(L"Overflow template not ready, waiting for Loaded");
            state->overflow = overflow;
            state->detaching = false;
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
                            FindDescendantByName(overflow, L"OverflowItemsControl")
                                .try_as<Controls::ItemsControl>();
                        if (itemsControl) {
                            Attach(*state, overflow, itemsControl);
                        } else {
                            Wh_Log(L"OverflowItemsControl not found");
                        }
                    } catch (...) {
                        Wh_Log(L"Loaded: attach failed");
                    }
                });
        }
        return nullptr;
    }

    Attach(*state, overflow, itemsControl);
    return state;
}

SRWLOCK g_pendingSettingsLock = SRWLOCK_INIT;
std::optional<Settings> g_pendingSettings;

void ApplyPendingSettings() {
    std::optional<Settings> settings;
    AcquireSRWLockExclusive(&g_pendingSettingsLock);
    settings.swap(g_pendingSettings);
    ReleaseSRWLockExclusive(&g_pendingSettingsLock);
    if (!settings || g_unloading) {
        return;
    }

    g_settings = std::move(*settings);
    auto states = *g_states;
    for (auto& state : states) {
        if (state->detaching) {
            continue;
        }
        try {
            if (g_settings.folders.empty()) {
                std::erase(*g_states, state);
                Detach(*state);
            } else if (state->attached) {
                state->openFolder = -1;
                Rebuild(*state);
            }
        } catch (...) {
            Wh_Log(L"Applying settings failed");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

void* g_showingManager;

using OverflowXamlIslandManager_Show_t =
    void(WINAPI*)(void* pThis, POINT pt, int inputDeviceKind);
OverflowXamlIslandManager_Show_t OverflowXamlIslandManager_Show_Original;
void WINAPI OverflowXamlIslandManager_Show_Hook(void* pThis,
                                                POINT pt,
                                                int inputDeviceKind) {
    Wh_Log(L"Show %d,%d kind=%d", pt.x, pt.y, inputDeviceKind);

    try {
        ApplyPendingSettings();
    } catch (...) {
        Wh_Log(L"Show: applying settings failed");
    }

    {
        g_showingManager = pThis;
        auto reset = ScopeExit([] { g_showingManager = nullptr; });
        OverflowXamlIslandManager_Show_Original(pThis, pt, inputDeviceKind);
    }

    if (g_unloading || !g_states) {
        return;
    }
    auto state = FindState(pThis);
    if (!state || !state->attached) {
        return;
    }

    try {
        RefreshTileSize(*state);
    } catch (...) {
        Wh_Log(L"Show: update failed");
    }

    struct Param {
        HWND window;
        int count;
    } param{};
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *(Param*)lParam;
            WCHAR className[64];
            if (IsWindowVisible(hWnd) &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"TopLevelWindowForOverflowXamlIsland") ==
                    0) {
                param.window = hWnd;
                param.count++;
            }
            return TRUE;
        },
        (LPARAM)&param);
    state->islandWindow = param.count == 1 ? param.window : nullptr;
}

// While Show positions the flyout, the manager measures its
// NotificationAreaOverflow member through this function, before the window is
// shown. Its `this` is that member, a C++/WinRT projected type, which holds
// nothing but the control's interface pointer, so the control is taken from
// there instead of from the manager's layout, which isn't part of any contract.
using NotificationAreaOverflow_Measure_t =
    void(WINAPI*)(void* pThis,
                  winrt::Windows::Foundation::Size const* availableSize);
NotificationAreaOverflow_Measure_t NotificationAreaOverflow_Measure_Original;
void WINAPI NotificationAreaOverflow_Measure_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size const* availableSize) {
    if (void* manager = g_showingManager) {
        try {
            auto overflow = reinterpret_cast<wf::IInspectable const*>(pThis)
                                ->try_as<Controls::Control>();
            if (auto state = EnsureAttached(manager, overflow)) {
                // Always open at the top level, and pick up renamed icons.
                state->openFolder = -1;
                Rebuild(*state, /*opening=*/true);
                RefreshTileSize(*state);
            }
        } catch (...) {
            Wh_Log(L"Show: update failed");
        }
    }

    NotificationAreaOverflow_Measure_Original(pThis, availableSize);
}

////////////////////////////////////////////////////////////////////////////////
// Threading helpers

struct RunFromWindowThreadParam {
    std::function<void()> proc;
    bool succeeded = false;
};

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
            Wh_Log(L"Window thread operation failed");
            return false;
        }
    }

    RunFromWindowThreadParam param{std::move(proc)};
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto& cwp = *reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp.message == message) {
                    auto& param =
                        *reinterpret_cast<RunFromWindowThreadParam*>(
                            cwp.lParam);
                    try {
                        param.proc();
                        param.succeeded = true;
                    } catch (...) {
                        Wh_Log(L"Window thread operation failed");
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    SendMessage(hWnd, message, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return param.succeeded;
}

std::vector<HWND> GetOverflowThreadWindows() {
    std::vector<HWND> windows;
    if (DWORD threadId = g_overflowThreadId) {
        EnumThreadWindows(
            threadId,
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                ((std::vector<HWND>*)lParam)->push_back(hWnd);
                return TRUE;
            },
            (LPARAM)&windows);
    }

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                ((std::vector<HWND>*)lParam)->push_back(hWnd);
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&windows);

    return windows;
}

bool RunFromOverflowThread(std::function<void()> const& proc) {
    for (HWND hWnd : GetOverflowThreadWindows()) {
        bool ran = false;
        bool succeeded = RunFromWindowThread(hWnd, [&ran, &proc] {
            ran = true;
            proc();
        });
        if (ran) {
            return succeeded;
        }
    }
    return false;
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
            {LR"(public: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::Show(struct tagPOINT,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &OverflowXamlIslandManager_Show_Original,
            OverflowXamlIslandManager_Show_Hook,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::SystemTray::NotificationAreaOverflow>::Measure(struct winrt::Windows::Foundation::Size const &)const )"},
            &NotificationAreaOverflow_Measure_Original,
            NotificationAreaOverflow_Measure_Hook,
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
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelBaseVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::get_ToolTipText(void * *))"},
            &g_imageContentViewModel_get_ToolTipText,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IImageIconContentViewModel>)"},
            &g_imageContentViewModelIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::get_AutomationPropertyName(void * *))"},
            &g_imageContentViewModel_get_AutomationPropertyName,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::get_IconImageSource(void * *))"},
            &g_imageContentViewModel_get_IconImageSource,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    bool appNames = g_iconConfigurationIid && g_iconConfigurationVftable &&
                    g_iconConfiguration_get_DataModel &&
                    g_notificationAreaIconsDataModelIid &&
                    g_notificationAreaIconsDataModelVftable &&
                    g_notificationAreaIconsDataModel_get_UDKObject;
    bool toolTips = g_iconContentViewModelIid &&
                    g_imageContentViewModelBaseVftable &&
                    g_imageContentViewModel_get_ToolTipText;
    bool names = g_imageContentViewModelIid && g_imageContentViewModelVftable &&
                 g_imageContentViewModel_get_AutomationPropertyName;
    Wh_Log(L"Icon names: app=%d tooltip=%d name=%d", appNames, toolTips,
           names);

    if (!appNames) {
        Wh_Log(L"App name matching unavailable");
    }

    if (!toolTips && !names) {
        Wh_Log(L"Couldn't find the icon tooltip getters");
        return false;
    }

    return true;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

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
    Wh_Log(L">");

    g_settings = LoadSettings();
    if (g_settings.folders.empty()) {
        Wh_Log(L"No folder has icons, nothing to do");
        return FALSE;
    }

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                            LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            Wh_Log(L"Couldn't hook LoadLibraryExW");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (!g_overflowThreadId) {
        return;
    }

    auto cleanup = [] {
        if (!g_states) {
            return;
        }
        auto states = std::move(*g_states);
        g_states.reset();
        for (auto& state : states) {
            try {
                Detach(*state);
            } catch (...) {
                Wh_Log(L"Detach failed");
            }
        }
        g_contentVmMember = nullptr;
        g_configurationMember = nullptr;
        g_overflowIconsMember = nullptr;
    };

    for (int attempt = 0; attempt < 3; attempt++) {
        if (RunFromOverflowThread(cleanup)) {
            return;
        }
        Sleep(100);
    }

    Wh_Log(L"ERROR: Overflow cleanup couldn't run on the tray thread, so the "
           L"flyout still uses the mod's list and Explorer may crash once the "
           L"mod is unloaded");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    Settings settings = LoadSettings();
    AcquireSRWLockExclusive(&g_pendingSettingsLock);
    g_pendingSettings = std::move(settings);
    ReleaseSRWLockExclusive(&g_pendingSettingsLock);

    if (g_overflowThreadId && !RunFromOverflowThread(ApplyPendingSettings)) {
        Wh_Log(L"Settings will be applied when the flyout opens");
    }
}
