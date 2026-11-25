// ==WindhawkMod==
// @id              command-palette-logo
// @name            Command Palette Logo
// @description     Replace or hide the logo next to the search box in the PowerToys Command Palette
// @version         1.0
// @author          Don den Boef
// @github          https://github.com/dondboef
// @license         MIT
// @include         Microsoft.CmdPal.UI.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Command Palette Logo

Replace or hide the logo shown to the left of the search box in the
[PowerToys Command Palette](https://learn.microsoft.com/windows/powertoys/command-palette/overview).

![Screenshot](https://raw.githubusercontent.com/dondboef/command-palette-logo/main/screenshot.png)

## Options

* **Built-in arrow**: an "enter" arrow icon that follows the light or dark
  theme.
* **Custom image**: any `.png`, `.jpg`, `.ico` or `.svg` file. A square image
  (for example 128x128) with a transparent background works best.
* **Hidden**: removes the logo, the search box moves to the left.

The size of the logo can be changed as well.

## Notes

* If the Command Palette is already running when the mod is enabled, restart
  the Command Palette once (for example from its tray icon, or by ending
  `Microsoft.CmdPal.UI.exe`) so that the mod can find the logo.
* Settings changes apply immediately. Disabling the mod restores the original
  logo.
* The Command Palette has no setting for this logo, so the mod changes it at
  runtime. No app files are modified.
* Tested with Command Palette 0.13 on Windows 11.

## Credits

The built-in arrow is the Arrow Enter icon from
[Fluent UI System Icons](https://github.com/microsoft/fluentui-system-icons)
by Microsoft (MIT License), mirrored horizontally.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logo: arrow
  $name: Logo
  $options:
  - arrow: Built-in arrow
  - custom: Custom image
  - hidden: Hidden
- customImagePath: ""
  $name: Custom image path
  $description: >-
    Full path to a .png, .jpg, .ico or .svg file, used when Logo is set to
    Custom image. Example: C:\Users\Public\Pictures\logo.png
- size: 20
  $name: Size
  $description: Size of the logo in pixels. The default logo is 20.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <mutex>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <shcore.h>
#include <shlwapi.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace wss = winrt::Windows::Storage::Streams;
namespace mud = winrt::Microsoft::UI::Dispatching;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxi = winrt::Microsoft::UI::Xaml::Media::Imaging;

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class LogoMode { Arrow, Custom, Hidden };

struct Settings {
    LogoMode logo = LogoMode::Arrow;
    std::wstring customImagePath;
    int size = 20;
};

// Written on the Windhawk thread, read on the UI thread.
std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

int LogoSize(const Settings& settings) {
    return settings.size > 0 ? settings.size : 20;
}

void LoadSettings() {
    Settings settings;

    WindhawkUtils::StringSetting logo(Wh_GetStringSetting(L"logo"));
    if (wcscmp(logo, L"custom") == 0) {
        settings.logo = LogoMode::Custom;
    } else if (wcscmp(logo, L"hidden") == 0) {
        settings.logo = LogoMode::Hidden;
    } else {
        settings.logo = LogoMode::Arrow;
    }

    WindhawkUtils::StringSetting path(Wh_GetStringSetting(L"customImagePath"));
    settings.customImagePath = path.get();

    // Accept paths copied with "Copy as path", which adds quotes.
    auto& p = settings.customImagePath;
    if (p.size() >= 2 && p.front() == L'"' && p.back() == L'"') {
        p = p.substr(1, p.size() - 2);
    }

    settings.size = Wh_GetIntSetting(L"size");

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(settings);
}

////////////////////////////////////////////////////////////////////////////////
// State
//
// XAML objects are only created, used and released on the UI thread. The
// globals that hold them have their destructors suppressed, since a destructor
// would run on the wrong thread or after XAML has shut down.

// The UI thread's dispatcher, usable from any thread.
[[clang::no_destroy]] mud::DispatcherQueue g_dispatcher{nullptr};
std::mutex g_dispatcherMutex;

// Window contents being watched for the logo.
struct WatchedRoot {
    winrt::weak_ref<mux::FrameworkElement> root;
    winrt::event_token layoutUpdatedToken;
    ULONGLONG lastSearch = 0;
};
[[clang::no_destroy]] std::vector<WatchedRoot> g_watchedRoots;

[[clang::no_destroy]] winrt::weak_ref<muxc::Image> g_logoImage;
winrt::event_token g_themeChangedToken{};

// The logo's original properties, restored when the mod is disabled.
[[clang::no_destroy]] muxm::ImageSource g_originalSource{nullptr};
struct {
    bool saved = false;
    double width = 20;
    double height = 0;
    double opacity = 1;
    mux::Thickness margin{0, 0, 6, 0};
} g_original;

bool g_unloading = false;

////////////////////////////////////////////////////////////////////////////////
// Image sources

// Arrow Enter (24 filled) from Fluent UI System Icons, mirrored horizontally.
// Copyright (c) 2020 Microsoft Corporation, MIT License:
// https://github.com/microsoft/fluentui-system-icons
// %s is replaced by the fill color.
constexpr char kArrowSvg[] =
    R"svg(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">)svg"
    R"svg(<g transform="matrix(-1 0 0 1 24 0)" fill="%s" fill-rule="nonzero">)svg"
    R"svg(<path d="M21,4 C21.5128358,4 21.9355072,4.38604019 21.9932723,4.88337887 )svg"
    R"svg(L22,5 L22,11.5 C22,13.3685634 20.5357224,14.8951264 18.6920352,14.9948211 )svg"
    R"svg(L18.5,15 L5.415,15 L8.70710678,18.2928932 C9.06759074,18.6533772 )svg"
    R"svg(9.09532028,19.2206082 8.79029539,19.6128994 L8.70710678,19.7071068 )svg"
    R"svg(C8.34662282,20.0675907 7.77939176,20.0953203 7.38710056,19.7902954 )svg"
    R"svg(L7.29289322,19.7071068 L2.29289322,14.7071068 C2.25749917,14.6717127 )svg"
    R"svg(2.22531295,14.6343256 2.19633458,14.5953066 L2.12467117,14.4840621 )svg"
    R"svg(L2.07122549,14.371336 L2.03584514,14.265993 L2.0110178,14.1484669 )svg"
    R"svg(L2.00397748,14.0898018 L2,14 L2.00278786,13.9247615 )svg"
    R"svg(L2.02024007,13.7992742 L2.04973809,13.6878575 L2.09367336,13.5767785 )svg"
    R"svg(L2.14599545,13.4792912 L2.20970461,13.3871006 L2.29289322,13.2928932 )svg"
    R"svg(L7.29289322,8.29289322 C7.68341751,7.90236893 8.31658249,7.90236893 )svg"
    R"svg(8.70710678,8.29289322 C9.06759074,8.65337718 9.09532028,9.22060824 )svg"
    R"svg(8.79029539,9.61289944 L8.70710678,9.70710678 L5.415,13 L18.5,13 )svg"
    R"svg(C19.2796961,13 19.9204487,12.4051119 19.9931334,11.64446 L20,11.5 )svg"
    R"svg(L20,5 C20,4.44771525 20.4477153,4 21,4 Z"/></g></svg>)svg";

wss::IRandomAccessStream StreamFromBytes(const void* data, size_t size) {
    winrt::com_ptr<IStream> memStream;
    memStream.attach(SHCreateMemStream(static_cast<const BYTE*>(data),
                                       static_cast<UINT>(size)));
    if (!memStream) {
        return nullptr;
    }

    wss::IRandomAccessStream stream;
    HRESULT hr = CreateRandomAccessStreamOverStream(
        memStream.get(), BSOS_DEFAULT, winrt::guid_of<wss::IRandomAccessStream>(),
        winrt::put_abi(stream));
    if (FAILED(hr)) {
        Wh_Log(L"CreateRandomAccessStreamOverStream failed: %08X", hr);
        return nullptr;
    }
    return stream;
}

std::vector<BYTE> ReadFileBytes(const std::wstring& path) {
    std::vector<BYTE> data;

    HANDLE file = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                             nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Can't open %s: %u", path.c_str(), GetLastError());
        return data;
    }

    LARGE_INTEGER fileSize{};
    if (GetFileSizeEx(file, &fileSize) && fileSize.QuadPart > 0 &&
        fileSize.QuadPart < 32 * 1024 * 1024) {
        data.resize(static_cast<size_t>(fileSize.QuadPart));
        DWORD read = 0;
        if (ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &read,
                     nullptr)) {
            data.resize(read);
        } else {
            data.clear();
        }
    }

    CloseHandle(file);
    return data;
}

bool IsSvgPath(const std::wstring& path) {
    PCWSTR ext = PathFindExtension(path.c_str());
    return ext && _wcsicmp(ext, L".svg") == 0;
}

muxm::ImageSource CreateArrowSource(mux::ElementTheme theme) {
    PCSTR color = theme == mux::ElementTheme::Light ? "#1F1F1F" : "#F2F2F2";

    char svg[sizeof(kArrowSvg) + 16];
    snprintf(svg, sizeof(svg), kArrowSvg, color);

    auto stream = StreamFromBytes(svg, strlen(svg));
    if (!stream) {
        return nullptr;
    }

    muxi::SvgImageSource source;
    source.SetSourceAsync(stream);
    return source;
}

muxm::ImageSource CreateCustomSource(const Settings& settings) {
    if (settings.customImagePath.empty()) {
        Wh_Log(L"No custom image path set");
        return nullptr;
    }

    auto data = ReadFileBytes(settings.customImagePath);
    if (data.empty()) {
        return nullptr;
    }

    auto stream = StreamFromBytes(data.data(), data.size());
    if (!stream) {
        return nullptr;
    }

    if (IsSvgPath(settings.customImagePath)) {
        muxi::SvgImageSource source;
        source.SetSourceAsync(stream);
        return source;
    }

    muxi::BitmapImage source;
    // Decode at a higher resolution for sharp results on high DPI displays.
    source.DecodePixelWidth(LogoSize(settings) * 3);
    source.SetSource(stream);
    return source;
}

////////////////////////////////////////////////////////////////////////////////
// Applying to the logo (UI thread)

void SaveOriginal(muxc::Image const& image) {
    if (g_original.saved) {
        return;
    }
    g_originalSource = image.Source();
    g_original.width = image.Width();
    g_original.height = image.Height();
    g_original.opacity = image.Opacity();
    g_original.margin = image.Margin();
    g_original.saved = true;
}

void RestoreOriginal(muxc::Image const& image) {
    if (!g_original.saved) {
        return;
    }
    image.Source(g_originalSource);
    image.Width(g_original.width);
    image.Height(g_original.height);
    image.Opacity(g_original.opacity);
    image.Margin(g_original.margin);
}

void ApplyToLogo(muxc::Image const& image) {
    SaveOriginal(image);

    const Settings settings = GetSettings();

    if (settings.logo == LogoMode::Hidden) {
        // Visibility is bound to the current page, so collapse the element by
        // size instead.
        image.Width(0);
        image.Height(0);
        image.Opacity(0);
        image.Margin(mux::Thickness{0, 0, 0, 0});
        return;
    }

    muxm::ImageSource source{nullptr};
    if (settings.logo == LogoMode::Arrow) {
        source = CreateArrowSource(image.ActualTheme());
    } else {
        source = CreateCustomSource(settings);
    }

    // Set the height as well, so that a tall image doesn't make the top bar
    // taller. The default Stretch (Uniform) keeps the aspect ratio.
    const int size = LogoSize(settings);
    image.Source(source ? source : g_originalSource);
    image.Width(size);
    image.Height(size);
    image.Opacity(g_original.opacity);
    image.Margin(g_original.margin);
}

void AttachToLogo(muxc::Image const& image) {
    Wh_Log(L"Found the Command Palette logo");

    g_logoImage = winrt::make_weak(image);

    // The built-in arrow is drawn in a color that depends on the theme.
    g_themeChangedToken = image.ActualThemeChanged(
        [](mux::FrameworkElement const& sender, wf::IInspectable const&) {
            try {
                if (GetSettings().logo == LogoMode::Arrow) {
                    if (auto img = sender.try_as<muxc::Image>()) {
                        ApplyToLogo(img);
                    }
                }
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult());
            }
        });

    ApplyToLogo(image);
}

bool IsLogoImage(muxc::Image const& image) {
    // The logo is defined in ShellPage.xaml as an Image with the source
    // ms-appx:///Assets/icon.svg and a width of 20.
    auto svg = image.Source().try_as<muxi::SvgImageSource>();
    if (!svg || !svg.UriSource()) {
        return false;
    }

    std::wstring uri{svg.UriSource().AbsoluteUri()};
    for (auto& c : uri) {
        c = towlower(c);
    }

    return uri.ends_with(L"/assets/icon.svg") && image.Width() <= 24;
}

mux::FrameworkElement FindChildByName(mux::DependencyObject const& parent,
                                      std::wstring_view name,
                                      int depth = 0) {
    if (depth > 64) {
        return nullptr;
    }

    int count = muxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<mux::FrameworkElement>()) {
            if (element.Name() == name) {
                return element;
            }
        }
        if (auto found = FindChildByName(child, name, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

muxc::Image FindLogoImage(mux::DependencyObject const& parent, int depth = 0) {
    if (depth > 8) {
        return nullptr;
    }

    int count = muxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(parent, i);
        if (auto image = child.try_as<muxc::Image>()) {
            if (IsLogoImage(image)) {
                return image;
            }
        }
        if (auto found = FindLogoImage(child, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

bool IsLogoAttached() {
    auto image = g_logoImage.get();
    return image && image.XamlRoot();
}

void OnRootLayoutUpdated(WatchedRoot& watched) {
    if (g_unloading || IsLogoAttached()) {
        return;
    }

    // Layout updates can be frequent, don't search on each of them.
    ULONGLONG now = GetTickCount64();
    if (now - watched.lastSearch < 250) {
        return;
    }
    watched.lastSearch = now;

    auto root = watched.root.get();
    if (!root) {
        return;
    }

    // TopBarGrid is the named grid in ShellPage.xaml that holds the logo.
    auto topBar = FindChildByName(root, L"TopBarGrid");
    if (!topBar) {
        return;
    }

    if (auto image = FindLogoImage(topBar)) {
        AttachToLogo(image);
    }
}

void WatchRoot(mux::FrameworkElement const& root) {
    for (auto& watched : g_watchedRoots) {
        if (watched.root.get() == root) {
            return;
        }
    }

    {
        std::lock_guard lock(g_dispatcherMutex);
        if (!g_dispatcher) {
            g_dispatcher = root.DispatcherQueue();
        }
    }

    Wh_Log(L"Watching window content");

    WatchedRoot watched;
    watched.root = winrt::make_weak(root);
    auto weakRoot = watched.root;
    watched.layoutUpdatedToken =
        root.LayoutUpdated([weakRoot](wf::IInspectable const&,
                                      wf::IInspectable const&) {
            try {
                for (auto& w : g_watchedRoots) {
                    if (w.root == weakRoot) {
                        OnRootLayoutUpdated(w);
                        break;
                    }
                }
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult());
            }
        });

    g_watchedRoots.push_back(std::move(watched));
}

// Undoes everything on the UI thread when the mod is disabled.
void CleanupOnUiThread() {
    g_unloading = true;

    for (auto& watched : g_watchedRoots) {
        if (auto root = watched.root.get()) {
            root.LayoutUpdated(watched.layoutUpdatedToken);
        }
    }
    g_watchedRoots.clear();

    if (auto image = g_logoImage.get()) {
        image.ActualThemeChanged(g_themeChangedToken);
        RestoreOriginal(image);
    }
    g_logoImage = nullptr;

    // If the logo is already gone, the original source is kept alive rather
    // than released here, see the comment above the globals.
    if (g_original.saved) {
        g_originalSource = nullptr;
        g_original.saved = false;
    }
}

// Runs a function on the UI thread and waits for it to finish.
template <typename F>
void RunOnUiThread(F f) {
    mud::DispatcherQueue dispatcher{nullptr};
    {
        std::lock_guard lock(g_dispatcherMutex);
        dispatcher = g_dispatcher;
    }
    if (!dispatcher) {
        return;
    }

    if (dispatcher.HasThreadAccess()) {
        f();
        return;
    }

    HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!done) {
        return;
    }

    bool queued = dispatcher.TryEnqueue([f, done]() {
        try {
            f();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
        SetEvent(done);
    });

    // The UI thread never waits for the Windhawk thread, so this can't
    // deadlock. Waiting until the work ran guarantees it doesn't run after the
    // mod is unloaded.
    if (queued) {
        WaitForSingleObject(done, INFINITE);
    }
    CloseHandle(done);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

// Called when the content of a WinUI window is set. The Command Palette's main
// window sets its content once, when it's created.
using WindowGenerated_put_Content_t = HRESULT(WINAPI*)(void* pThis,
                                                       void* content);
WindowGenerated_put_Content_t WindowGenerated_put_Content_Original;
HRESULT WINAPI WindowGenerated_put_Content_Hook(void* pThis, void* content) {
    Wh_Log(L">");

    HRESULT hr = WindowGenerated_put_Content_Original(pThis, content);

    if (SUCCEEDED(hr) && content && !g_unloading) {
        try {
            mux::UIElement element{nullptr};
            winrt::copy_from_abi(element, content);
            if (auto root = element.try_as<mux::FrameworkElement>()) {
                WatchRoot(root);
            }
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    return hr;
}

bool HookMicrosoftUiXamlSymbols(HMODULE module) {
    // Microsoft.UI.Xaml.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl DirectUI::WindowGenerated::put_Content(struct ABI::Microsoft::UI::Xaml::IUIElement *))"},
            &WindowGenerated_put_Content_Original,
            WindowGenerated_put_Content_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool g_xamlHooked = false;

bool HookMicrosoftUiXamlIfLoaded() {
    if (g_xamlHooked) {
        return false;
    }

    HMODULE module = GetModuleHandle(L"Microsoft.UI.Xaml.dll");
    if (!module) {
        return false;
    }

    g_xamlHooked = true;
    return HookMicrosoftUiXamlSymbols(module);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !g_xamlHooked && lpLibFileName) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"Microsoft.UI.Xaml.dll") == 0 &&
            HookMicrosoftUiXamlIfLoaded()) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (GetModuleHandle(L"Microsoft.UI.Xaml.dll")) {
        // The window already exists, so its content won't be set again.
        Wh_Log(L"Command Palette is already running, restart it to apply");
        HookMicrosoftUiXamlIfLoaded();
    } else {
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    RunOnUiThread([]() {
        if (auto image = g_logoImage.get()) {
            ApplyToLogo(image);
        }
    });
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    RunOnUiThread([]() { CleanupOnUiThread(); });
}
