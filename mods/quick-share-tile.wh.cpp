// ==WindhawkMod==
// @id              quick-share-tile
// @name            Quick Share Tile
// @description     Replaces the Windows 11 Nearby sharing tile with a Quick Share launcher.
// @version         1.0.0
// @author          ThePsychof
// @github          https://github.com/ThePsychof
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lruntimeobject -lole32 -loleaut32 -lshell32 -lgdi32 -luuid
// @license         GPL-3.0-only
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Quick Share Tile

Replace the Windows 11 **Nearby sharing** Quick Settings tile
with a **Quick Share** launcher.

Keep your Quick Settings layout intact while giving the native
Nearby sharing tile a new purpose.

## Preview

![Before and after](https://raw.githubusercontent.com/ThePsychof/QuickShare-Tile/main/Before%26After.png)

## Features

- Replaces the Nearby sharing tile with Quick Share.
- Changes the tile's name and icon.
- Preserves native tile dimensions, position and styling.
- Launches Quick Share with a single click.
- Automatically restores the original tile when disabled.

## Requirements

- Windows 11 x64.
- Windhawk.
- Quick Share installed.
- Quick Share shortcut located at:

  `C:\ProgramData\Microsoft\Windows\Start Menu\Quick Share.lnk`

## Settings

### Native tile to replace
The accessibility name of the tile to replace.

Default: `Nearby sharing`

The name is language-dependent. Change it if your Windows
installation uses another display language.

### Log the visual tree
Logs the Quick Settings visual tree for troubleshooting.

## Known Limitations

- Tested on Windows 11 x64 build 26300.
- Other Windows builds are not guaranteed to work.
- The tile's active appearance still reflects Nearby sharing,
  not Quick Share's actual running state.
- The Quick Share shortcut path is currently hardcoded.
- This mod launches Quick Share; it does not implement
  Quick Share functionality.

## Credits

Parts of the ControlCenter.dll hooking and cleanup logic are based on
[refined-quick-settings-and-sound](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/refined-quick-settings-and-sound.wh.cpp)
(GPL-3.0-only). This mod is therefore also released under GPL-3.0-only.

## Uninstallation

Disable or remove the mod through Windhawk.

The original Nearby sharing tile is restored, including
its label, icon, click behavior and keyboard navigation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetTile: Nearby sharing
  $name: Native tile to replace
  $description: Accessibility name of the tile to take over. Language dependent.
- logTree: false
  $name: Log the visual tree
  $description: Writes the Quick Settings layout to the mod log (for troubleshooting).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <unknwn.h>
#include <shellapi.h>
#include <shobjidl.h>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

namespace xaml = winrt::Windows::UI::Xaml;
namespace media = winrt::Windows::UI::Xaml::Media;
namespace controls = winrt::Windows::UI::Xaml::Controls;
namespace automation = winrt::Windows::UI::Xaml::Automation;

namespace {

constexpr wchar_t kQuickShareShortcut[] =
    L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Quick Share.lnk";
constexpr wchar_t kOverlayName[] = L"QNSwapperOverlay";
constexpr wchar_t kIconName[] = L"QNSwapperIcon";
constexpr wchar_t kToggleClass[] = L"ControlCenter.PaginatedToggleButton";
constexpr int kIconSize = 20;

using ViewGotFocus = int(WINAPI*)(void*, void*);
using LoadLibraryExFn = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
using CoreDispatcher = winrt::Windows::UI::Core::CoreDispatcher;

ViewGotFocus g_viewGotFocusOriginal = nullptr;
LoadLibraryExFn g_loadLibraryExOriginal = nullptr;
std::atomic<bool> g_stopping{false};
std::atomic<int> g_controlCenterHookState{0};
thread_local bool g_inViewGotFocus = false;
std::mutex g_viewsMutex;

struct IBufferByteAccess : ::IUnknown {
    virtual HRESULT __stdcall Buffer(uint8_t** value) = 0;
};

constexpr GUID kBufferByteAccessGuid = {
    0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    std::wstring targetTile;
    bool logTree = false;
};

Settings g_settings;

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    g_settings.targetTile = GetStringSetting(L"targetTile");
    g_settings.logTree = Wh_GetIntSetting(L"logTree") != 0;

    if (g_settings.targetTile.empty()) {
        g_settings.targetTile = L"Nearby sharing";
    }
}

bool FileExists(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void LaunchQuickShare() {
    if (!FileExists(kQuickShareShortcut)) {
        Wh_Log(L"Quick Share shortcut does not exist: %s",
               kQuickShareShortcut);
        return;
    }

    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    info.lpVerb = L"open";
    info.lpFile = kQuickShareShortcut;
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"Launching Quick Share failed (error %lu): %s", GetLastError(),
               kQuickShareShortcut);
    }
}

// ---------------------------------------------------------------------------
// Icon loading (shell icon -> XAML bitmap)
// ---------------------------------------------------------------------------

media::ImageSource LoadShellIcon(std::wstring const& path, int size) {
    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromParsingName(path.c_str(), nullptr,
                                             __uuidof(IShellItemImageFactory),
                                             factory.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"Icon lookup failed for %s: 0x%08X", path.c_str(),
               static_cast<unsigned>(hr));
        return nullptr;
    }

    HBITMAP bitmapHandle = nullptr;
    SIZE requested{size, size};
    hr = factory->GetImage(requested, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &bitmapHandle);
    if (FAILED(hr) || !bitmapHandle) {
        Wh_Log(L"Icon extraction failed for %s: 0x%08X", path.c_str(),
               static_cast<unsigned>(hr));
        return nullptr;
    }

    BITMAP info{};
    if (!GetObjectW(bitmapHandle, sizeof(info), &info) || info.bmWidth <= 0 ||
        info.bmHeight <= 0) {
        DeleteObject(bitmapHandle);
        Wh_Log(L"Icon bitmap query failed for %s", path.c_str());
        return nullptr;
    }

    const int width = info.bmWidth;
    const int height = info.bmHeight;
    std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4);

    BITMAPINFO header{};
    header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    header.bmiHeader.biWidth = width;
    header.bmiHeader.biHeight = -height;  // top-down
    header.bmiHeader.biPlanes = 1;
    header.bmiHeader.biBitCount = 32;
    header.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    int lines = GetDIBits(screen, bitmapHandle, 0, height, pixels.data(), &header,
                          DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    DeleteObject(bitmapHandle);
    if (lines != height) {
        Wh_Log(L"Icon pixel read failed for %s", path.c_str());
        return nullptr;
    }

    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        if (pixels[i]) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        for (size_t i = 3; i < pixels.size(); i += 4) {
            pixels[i] = 255;
        }
    }

    media::Imaging::WriteableBitmap bitmap(width, height);
    auto buffer = bitmap.PixelBuffer();
    winrt::com_ptr<IBufferByteAccess> access;
    winrt::check_hresult(winrt::get_unknown(buffer)->QueryInterface(
        kBufferByteAccessGuid, access.put_void()));
    uint8_t* destination = nullptr;
    winrt::check_hresult(access->Buffer(&destination));
    memcpy(destination, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

// ---------------------------------------------------------------------------
// Visual tree helpers
// ---------------------------------------------------------------------------

template <typename Predicate>
xaml::FrameworkElement FindElement(xaml::DependencyObject const& root,
                                   Predicate&& predicate) {
    if (!root) {
        return nullptr;
    }
    if (auto element = root.try_as<xaml::FrameworkElement>()) {
        if (predicate(element)) {
            return element;
        }
    }

    int count = media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found =
                FindElement(media::VisualTreeHelper::GetChild(root, i), predicate)) {
            return found;
        }
    }
    return nullptr;
}

std::wstring ClassNameOf(winrt::Windows::Foundation::IInspectable const& object) {
    if (!object) {
        return L"(null)";
    }
    try {
        return std::wstring(winrt::get_class_name(object).c_str());
    } catch (...) {
        return L"(unknown)";
    }
}

void LogVisualTree(xaml::DependencyObject const& root, int depth, int maxDepth,
                   int& remaining) {
    if (!root || remaining <= 0) {
        return;
    }

    --remaining;
    if (auto element = root.try_as<xaml::FrameworkElement>()) {
        auto name = element.Name();
        std::wstring indent(static_cast<size_t>(depth) * 2, L' ');
        std::wstring extra;
        if (auto textBlock = element.try_as<controls::TextBlock>()) {
            extra += L" text=\"";
            extra += textBlock.Text().c_str();
            extra += L"\"";
        }
        auto automationName = automation::AutomationProperties::GetName(element);
        if (!automationName.empty()) {
            extra += L" automationName=\"";
            extra += automationName.c_str();
            extra += L"\"";
        }
        Wh_Log(L"%s%s (%s)%s", indent.c_str(),
               name.empty() ? L"(unnamed)" : name.c_str(),
               winrt::get_class_name(element).c_str(), extra.c_str());
    }

    if (depth >= maxDepth) {
        return;
    }
    int count = media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count && remaining > 0; ++i) {
        LogVisualTree(media::VisualTreeHelper::GetChild(root, i), depth + 1,
                      maxDepth, remaining);
    }
}

void LogTileCandidates(xaml::DependencyObject const& root, int depth,
                      int maxDepth, int& remaining) {
    if (!root || depth > maxDepth || remaining <= 0) {
        return;
    }

    --remaining;
    if (auto element = root.try_as<xaml::FrameworkElement>()) {
        auto className = winrt::get_class_name(element);
        std::wstring classNameText = className.c_str();
        auto name = element.Name();
        auto automationName = automation::AutomationProperties::GetName(element);
        std::wstring text;
        if (auto textBlock = element.try_as<controls::TextBlock>()) {
            text = textBlock.Text();
        }

        bool isCandidate = classNameText.find(L"Toggle") != std::wstring::npos ||
                           name == L"TitleText" || !automationName.empty() ||
                           !text.empty();
        if (isCandidate) {
            Wh_Log(L"Quick Settings tile candidate: depth=%d name='%s' class='%s' "
                   L"automationName='%s' text='%s'",
                   depth, name.empty() ? L"" : name.c_str(), className.c_str(),
                   automationName.c_str(), text.c_str());
        }
    }

    int count = media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count && remaining > 0; ++i) {
        LogTileCandidates(media::VisualTreeHelper::GetChild(root, i), depth + 1,
                          maxDepth, remaining);
    }
}

// ---------------------------------------------------------------------------
// The replaced tile
// ---------------------------------------------------------------------------

struct AppliedTile {
    winrt::weak_ref<controls::Grid> root;
    winrt::weak_ref<controls::Button> overlay;
    winrt::weak_ref<controls::Image> icon;
    winrt::weak_ref<controls::Grid> iconHost;
    winrt::weak_ref<xaml::UIElement> nativeIcon;
    winrt::weak_ref<controls::TextBlock> nativeTitle;
    winrt::weak_ref<controls::Control> toggle;
    winrt::weak_ref<controls::Control> labelButton;
    winrt::event_token clickToken{};
    std::wstring nativeTitleText;
    double nativeIconOpacity = 1.0;
    bool toggleHitTest = true;
    bool toggleTabStop = true;
    bool labelHitTest = true;
    bool labelTabStop = true;
};

void RemoveChild(controls::Grid const& grid, xaml::UIElement const& child) {
    if (!grid || !child) {
        return;
    }
    uint32_t index = 0;
    auto children = grid.Children();
    if (children.IndexOf(child, index)) {
        children.RemoveAt(index);
    }
}

void RestoreTile(AppliedTile& tile) {
    if (auto overlay = tile.overlay.get()) {
        if (tile.clickToken.value) {
            overlay.Click(tile.clickToken);
        }
        RemoveChild(tile.root.get(), overlay);
    }
    RemoveChild(tile.iconHost.get(), tile.icon.get());

    if (auto nativeTitle = tile.nativeTitle.get()) {
        nativeTitle.Text(tile.nativeTitleText);
    }
    if (auto nativeIcon = tile.nativeIcon.get()) {
        nativeIcon.Opacity(tile.nativeIconOpacity);
    }
    if (auto toggle = tile.toggle.get()) {
        toggle.IsHitTestVisible(tile.toggleHitTest);
        toggle.IsTabStop(tile.toggleTabStop);
    }
    if (auto labelButton = tile.labelButton.get()) {
        labelButton.IsHitTestVisible(tile.labelHitTest);
        labelButton.IsTabStop(tile.labelTabStop);
    }
}

class QuickSettingsViewHook {
public:
    explicit QuickSettingsViewHook(xaml::FrameworkElement const& view)
        : view_(winrt::make_weak(view)), dispatcher_(view.Dispatcher()) {}

    xaml::FrameworkElement View() const {
        return view_.get();
    }

    CoreDispatcher Dispatcher() const {
        return dispatcher_;
    }

    bool Attach() {
        auto view = view_.get();
        if (!view) {
            return false;
        }

        try {
            if (g_settings.logTree) {
                auto layout = FindElement(view, [](xaml::FrameworkElement const& element) {
                    return element.Name() == L"L1Grid";
                });
                int remaining = 1500;
                Wh_Log(L"Quick Settings layout dump");
                LogVisualTree(layout ? layout : view, 0, 32, remaining);
            }
            ApplyTile(view);
            return true;
        } catch (winrt::hresult_error const& error) {
            Wh_Log(L"Quick Settings customization failed: 0x%08X %s",
                   static_cast<unsigned>(error.code()), error.message().c_str());
            return false;
        } catch (...) {
            Wh_Log(L"Quick Settings customization failed with an unexpected error");
            return false;
        }
    }

    void Restore() {
        for (auto& tile : tiles_) {
            try {
                RestoreTile(tile);
            } catch (winrt::hresult_error const& error) {
                Wh_Log(L"Restoring the Quick Settings tile failed: 0x%08X",
                       static_cast<unsigned>(error.code()));
            }
        }
        tiles_.clear();
        Wh_Log(L"Quick Settings tile restored");
    }

private:
    void ApplyTile(xaml::FrameworkElement const& view) {
        // Forget tiles whose visuals were recreated by the panel.
        tiles_.erase(std::remove_if(tiles_.begin(), tiles_.end(),
                                    [](AppliedTile const& tile) {
                                        return !tile.root.get();
                                    }),
                     tiles_.end());

        auto toggle = FindElement(view, [](xaml::FrameworkElement const& element) {
            return element.try_as<controls::Control>() &&
                   winrt::get_class_name(element) == kToggleClass &&
                   std::wstring(automation::AutomationProperties::GetName(element).c_str()) ==
                       g_settings.targetTile;
        });
        if (!toggle) {
            Wh_Log(L"Tile '%s' was not found in Quick Settings", g_settings.targetTile.c_str());
            auto layout = FindElement(view, [](xaml::FrameworkElement const& element) {
                return element.Name() == L"L1Grid";
            });
            int remaining = 1200;
            LogTileCandidates(layout ? layout : view, 0, 24, remaining);
            if (remaining == 0) {
                Wh_Log(L"Quick Settings tile candidate log reached its 1200-element limit");
            }
            return;
        }

        auto toggleParent = media::VisualTreeHelper::GetParent(toggle);
        auto root = toggleParent
                        ? media::VisualTreeHelper::GetParent(toggleParent).try_as<controls::Grid>()
                        : nullptr;
        if (!root) {
            Wh_Log(L"Unexpected layout around tile '%s'; leaving it unchanged",
                   g_settings.targetTile.c_str());
            return;
        }

        auto nativeTitle = FindElement(root, [](xaml::FrameworkElement const& element) {
            return element.Name() == L"TitleText";
        });

        // Keep the native title element so its typography and layout remain intact.
        auto existingOverlay = FindElement(root, [](xaml::FrameworkElement const& element) {
            return element.Name() == kOverlayName;
        });
        if (existingOverlay) {
            if (nativeTitle) {
                if (auto title = nativeTitle.try_as<controls::TextBlock>()) {
                    title.Text(L"Quick Share");
                }
            }
            return;
        }

        if (!nativeTitle) {
            Wh_Log(L"Tile '%s' has no recognizable title; leaving it unchanged",
                   g_settings.targetTile.c_str());
            return;
        }
        auto nativeTitleBlock = nativeTitle.try_as<controls::TextBlock>();
        if (!nativeTitleBlock) {
            Wh_Log(L"Tile '%s' title is not a text block; leaving it unchanged",
                   g_settings.targetTile.c_str());
            return;
        }
        controls::Button labelButton{nullptr};
        int childCount = media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < childCount && !labelButton; ++i) {
            labelButton = media::VisualTreeHelper::GetChild(root, i).try_as<controls::Button>();
        }
        if (!labelButton) {
            Wh_Log(L"Tile '%s' has no recognizable label; leaving it unchanged",
                   g_settings.targetTile.c_str());
            return;
        }

        AppliedTile tile;
        tile.root = winrt::make_weak(root);
        tile.toggle = winrt::make_weak(toggle.as<controls::Control>());
        tile.labelButton = winrt::make_weak(labelButton.as<controls::Control>());
        tile.nativeTitle = winrt::make_weak(nativeTitleBlock);
        tile.nativeTitleText = nativeTitleBlock.Text();
        tile.toggleHitTest = toggle.IsHitTestVisible();
        tile.toggleTabStop = toggle.as<controls::Control>().IsTabStop();
        tile.labelHitTest = labelButton.IsHitTestVisible();
        tile.labelTabStop = labelButton.IsTabStop();

        // Icon: replace the native animated icon with the Quick Share icon.
        auto iconHost = FindElement(toggle, [](xaml::FrameworkElement const& element) {
            return element.Name() == L"ToggleButtonContent";
        }).try_as<controls::Grid>();
        auto nativeIcon = FindElement(toggle, [](xaml::FrameworkElement const& element) {
            return winrt::get_class_name(element) == L"Microsoft.UI.Xaml.Controls.AnimatedIcon";
        });

        if (iconHost && nativeIcon) {
            if (auto source = LoadShellIcon(kQuickShareShortcut, kIconSize * 2)) {
                controls::Image icon;
                icon.Name(kIconName);
                icon.Source(source);
                icon.Width(kIconSize);
                icon.Height(kIconSize);
                icon.Stretch(media::Stretch::Uniform);
                icon.HorizontalAlignment(xaml::HorizontalAlignment::Center);
                icon.VerticalAlignment(xaml::VerticalAlignment::Center);
                icon.IsHitTestVisible(false);
                iconHost.Children().Append(icon);

                tile.icon = winrt::make_weak(icon);
                tile.iconHost = winrt::make_weak(iconHost);
                tile.nativeIcon = winrt::make_weak(nativeIcon.as<xaml::UIElement>());
                tile.nativeIconOpacity = nativeIcon.Opacity();
                nativeIcon.Opacity(0.0);
            }
        } else {
            Wh_Log(L"Quick Share icon not applied (icon host or source missing)");
        }

        // Keep the native tile visuals and use an invisible button only for clicks.
        controls::Button overlay;
        overlay.Name(kOverlayName);
        overlay.Background(
            media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
        overlay.BorderThickness(xaml::Thickness{0, 0, 0, 0});
        overlay.Padding(xaml::Thickness{0, 0, 0, 0});
        overlay.Opacity(0.0);
        overlay.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
        overlay.VerticalAlignment(xaml::VerticalAlignment::Stretch);
        automation::AutomationProperties::SetName(overlay, L"Quick Share");

        int32_t rowSpan = std::max<int32_t>(2, controls::Grid::GetRow(labelButton) +
                                                   controls::Grid::GetRowSpan(labelButton));
        controls::Grid::SetRow(overlay, 0);
        controls::Grid::SetRowSpan(overlay, rowSpan);
        controls::Grid::SetColumn(overlay, 0);
        controls::Grid::SetColumnSpan(
            overlay, std::max<int32_t>(1, static_cast<int32_t>(root.ColumnDefinitions().Size())));
        tile.clickToken = overlay.Click(
            [](winrt::Windows::Foundation::IInspectable const&,
               xaml::RoutedEventArgs const&) {
                LaunchQuickShare();
            });
        // Take over input from the native tile only once everything is ready.
        nativeTitleBlock.Text(L"Quick Share");
        toggle.IsHitTestVisible(false);
        toggle.as<controls::Control>().IsTabStop(false);
        labelButton.IsHitTestVisible(false);
        labelButton.IsTabStop(false);

        root.Children().Append(overlay);
        tile.overlay = winrt::make_weak(overlay);

        tiles_.push_back(std::move(tile));
        Wh_Log(L"Tile '%s' is now 'Quick Share'", g_settings.targetTile.c_str());
    }

    winrt::weak_ref<xaml::FrameworkElement> view_;
    CoreDispatcher dispatcher_{nullptr};
    std::vector<AppliedTile> tiles_;
};

[[clang::no_destroy]] std::optional<
    std::vector<std::unique_ptr<QuickSettingsViewHook>>>
    g_views{std::in_place};

bool DispatchCleanup(CoreDispatcher const& dispatcher, std::function<void()> action) {
    if (!dispatcher) {
        return false;
    }
    try {
        if (dispatcher.HasThreadAccess()) {
            action();
            return true;
        }

        auto cleaned = std::make_shared<std::atomic<bool>>(false);
        auto operation = dispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::High,
            [action = std::move(action), cleaned] {
                try {
                    action();
                    *cleaned = true;
                } catch (winrt::hresult_error const& error) {
                    Wh_Log(L"Quick Settings UI cleanup failed: 0x%08X",
                           static_cast<unsigned>(error.code()));
                }
            });
        while (operation.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            Sleep(1);
        }
        return operation.Status() ==
                   winrt::Windows::Foundation::AsyncStatus::Completed &&
               cleaned->load();
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Quick Settings UI dispatcher failed: 0x%08X",
               static_cast<unsigned>(error.code()));
        return false;
    }
}

void DiscoverView(void* object) {
    xaml::FrameworkElement view{nullptr};
    auto unknown = static_cast<::IUnknown*>(object);
    HRESULT hr = unknown->QueryInterface(winrt::guid_of<xaml::FrameworkElement>(),
                                         winrt::put_abi(view));
    if (FAILED(hr) || !view) {
        Wh_Log(L"Quick Settings view query failed: 0x%08X",
               static_cast<unsigned>(hr));
        return;
    }

    std::lock_guard lock(g_viewsMutex);
    if (g_stopping) {
        return;
    }

    for (auto it = g_views->begin(); it != g_views->end();) {
        auto existingView = (*it)->View();
        if (!existingView) {
            (*it)->Restore();
            it = g_views->erase(it);
            continue;
        }
        if (existingView == view) {
            (*it)->Attach();
            return;
        }
        ++it;
    }

    auto state = std::make_unique<QuickSettingsViewHook>(view);
    if (state->Attach()) {
        g_views->push_back(std::move(state));
    }
}

int WINAPI ViewGotFocusHook(void* object, void* args) {
    int result = g_viewGotFocusOriginal(object, args);
    if (g_stopping || g_inViewGotFocus) {
        return result;
    }

    g_inViewGotFocus = true;
    try {
        DiscoverView(object);
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Quick Settings discovery failed: 0x%08X",
               static_cast<unsigned>(error.code()));
    } catch (...) {
        Wh_Log(L"Quick Settings discovery failed with an unexpected error");
    }
    g_inViewGotFocus = false;
    return result;
}

bool HookControlCenter(HMODULE module, bool applyNow) {
    int expected = 0;
    if (!g_controlCenterHookState.compare_exchange_strong(expected, 1)) {
        return expected == 2;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &g_viewGotFocusOriginal, ViewGotFocusHook},
    };

    bool success = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
    if (success && applyNow) {
        success = Wh_ApplyHookOperations();
    }

    g_controlCenterHookState = success ? 2 : 3;
    Wh_Log(L"ControlCenter hook %s", success ? L"installed" : L"failed");
    return success;
}

HMODULE WINAPI LoadLibraryExHook(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE result = g_loadLibraryExOriginal(path, file, flags);
    if (result && !g_stopping &&
        !(flags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                   LOAD_LIBRARY_AS_IMAGE_RESOURCE)) &&
        result == GetModuleHandleW(L"ControlCenter.dll")) {
        HookControlCenter(result, true);
    }
    return result;
}

bool InitializeHook() {
    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
        return HookControlCenter(module, false);
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryEx =
        kernelBase ? reinterpret_cast<LoadLibraryExFn>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (!loadLibraryEx ||
        !WindhawkUtils::SetFunctionHook(loadLibraryEx, LoadLibraryExHook,
                                        &g_loadLibraryExOriginal)) {
        Wh_Log(L"Unable to monitor ControlCenter.dll loading");
        return false;
    }

    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
        return HookControlCenter(module, false);
    }

    Wh_Log(L"Waiting for ControlCenter.dll");
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    g_stopping = false;
    g_controlCenterHookState = 0;
    LoadSettings();
    return InitializeHook() ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (!g_stopping) {
        if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
            HookControlCenter(module, true);
        }
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Reloading runs the normal uninit path, which restores the native tile.
    *bReload = TRUE;
    return TRUE;
}

void Wh_ModUninit() {
    g_stopping = true;

    std::vector<std::unique_ptr<QuickSettingsViewHook>> views;
    {
        std::lock_guard lock(g_viewsMutex);
        views.swap(*g_views);
        g_views.reset();
    }

    for (auto& view : views) {
        auto state = view.get();
        if (!DispatchCleanup(view->Dispatcher(), [state] { state->Restore(); })) {
            Wh_Log(L"Quick Settings UI thread is shutting down; tile restore could not run");
        }
    }
}