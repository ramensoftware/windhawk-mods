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
  (for example 128×128) with a transparent background works best.
* **Hidden**: removes the logo, the search box moves to the left.

The size of the logo can be changed as well.

## Notes

* The Command Palette has no setting for this logo, so the mod changes it at
  runtime through XAML diagnostics, the same technique used by the Windows 11
  Styler mods. No app files are modified.
* Changes apply immediately. Disabling the mod restores the original logo.
* Only one XAML diagnostics consumer can be active per process, so tools such as
  UWPSpy can't be attached to the Command Palette while the mod is enabled.
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
  $description: Width of the logo in pixels. The default logo is 20.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <shcore.h>
#include <shlwapi.h>
#include <xamlom.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace wss = winrt::Windows::Storage::Streams;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxi = winrt::Microsoft::UI::Xaml::Media::Imaging;

enum class LogoMode { Arrow, Custom, Hidden };

struct {
    LogoMode logo;
    std::wstring customImagePath;
    int size;
} g_settings;

std::atomic<bool> g_tapInjected;

// The logo element. Only touched on its UI thread.
winrt::weak_ref<muxc::Image> g_logoImage;
winrt::event_token g_themeChangedToken{};

struct {
    bool saved = false;
    mux::Media::ImageSource source{nullptr};
    double width = 20;
    double opacity = 1;
    mux::Thickness margin{0, 0, 6, 0};
} g_original;

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

int LogoSize() {
    return g_settings.size > 0 ? g_settings.size : 20;
}

mux::Media::ImageSource CreateArrowSource(mux::ElementTheme theme) {
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

mux::Media::ImageSource CreateCustomSource() {
    if (g_settings.customImagePath.empty()) {
        Wh_Log(L"No custom image path set");
        return nullptr;
    }

    auto data = ReadFileBytes(g_settings.customImagePath);
    if (data.empty()) {
        return nullptr;
    }

    auto stream = StreamFromBytes(data.data(), data.size());
    if (!stream) {
        return nullptr;
    }

    if (IsSvgPath(g_settings.customImagePath)) {
        muxi::SvgImageSource source;
        source.SetSourceAsync(stream);
        return source;
    }

    muxi::BitmapImage source;
    // Decode at a higher resolution for sharp results on high DPI displays.
    source.DecodePixelWidth(LogoSize() * 3);
    source.SetSource(stream);
    return source;
}

////////////////////////////////////////////////////////////////////////////////
// Applying to the logo

void SaveOriginal(muxc::Image const& image) {
    if (g_original.saved) {
        return;
    }
    g_original.source = image.Source();
    g_original.width = image.Width();
    g_original.opacity = image.Opacity();
    g_original.margin = image.Margin();
    g_original.saved = true;
}

void RestoreOriginal(muxc::Image const& image) {
    if (!g_original.saved) {
        return;
    }
    image.Source(g_original.source);
    image.Width(g_original.width);
    image.Opacity(g_original.opacity);
    image.Margin(g_original.margin);
}

void ApplyToLogo(muxc::Image const& image) {
    SaveOriginal(image);

    if (g_settings.logo == LogoMode::Hidden) {
        // Visibility is bound to the current page, so collapse the element by
        // size instead.
        image.Width(0);
        image.Opacity(0);
        image.Margin(mux::Thickness{0, 0, 0, 0});
        return;
    }

    mux::Media::ImageSource source{nullptr};
    if (g_settings.logo == LogoMode::Arrow) {
        source = CreateArrowSource(image.ActualTheme());
    } else {
        source = CreateCustomSource();
    }

    image.Source(source ? source : g_original.source);
    image.Width(LogoSize());
    image.Opacity(g_original.opacity);
    image.Margin(g_original.margin);
}

void AttachToLogo(muxc::Image const& image) {
    if (auto previous = g_logoImage.get(); previous && previous != image) {
        previous.ActualThemeChanged(g_themeChangedToken);
    }

    g_logoImage = winrt::make_weak(image);

    // The built-in arrow is drawn in a color that depends on the theme.
    g_themeChangedToken = image.ActualThemeChanged(
        [](mux::FrameworkElement const& sender, wf::IInspectable const&) {
            if (g_settings.logo == LogoMode::Arrow) {
                if (auto img = sender.try_as<muxc::Image>()) {
                    ApplyToLogo(img);
                }
            }
        });

    ApplyToLogo(image);
}

void DetachFromLogo(bool restore) {
    auto image = g_logoImage.get();
    if (!image) {
        return;
    }
    image.ActualThemeChanged(g_themeChangedToken);
    if (restore) {
        RestoreOriginal(image);
    }
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

// Runs a function on the logo's UI thread and waits for it.
template <typename F>
void RunOnLogoThread(F&& f) {
    auto image = g_logoImage.get();
    if (!image) {
        return;
    }

    auto dispatcher = image.DispatcherQueue();
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

    if (queued) {
        WaitForSingleObject(done, 5000);
    }
    CloseHandle(done);
}

////////////////////////////////////////////////////////////////////////////////
// XAML diagnostics. Based on the Windows 11 Styler mods by m417z.

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()) {
        // Calling AdviseVisualTreeChange from the current thread can hang, so
        // it's called from a new thread.
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID param) -> DWORD {
                auto watcher = reinterpret_cast<VisualTreeWatcher*>(param);
                HRESULT hr = watcher->m_xamlDiagnostics.as<IVisualTreeService3>()
                                 ->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
                }
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    void UnadviseVisualTreeChange() {
        HRESULT hr = m_xamlDiagnostics.as<IVisualTreeService3>()
                         ->UnadviseVisualTreeChange(this);
        if (FAILED(hr)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: %08X", hr);
        }
    }

   private:
    HRESULT STDMETHODCALLTYPE
    OnVisualTreeChange(ParentChildRelation,
                       VisualElement element,
                       VisualMutationType mutationType) override try {
        if (mutationType != Add || !element.Type ||
            wcscmp(element.Type, L"Microsoft.UI.Xaml.Controls.Image") != 0) {
            return S_OK;
        }

        wf::IInspectable object;
        HRESULT hr = m_xamlDiagnostics->GetIInspectableFromHandle(
            element.Handle,
            reinterpret_cast<::IInspectable**>(winrt::put_abi(object)));
        if (FAILED(hr) || !object) {
            return S_OK;
        }

        auto image = object.try_as<muxc::Image>();
        if (image && IsLogoImage(image)) {
            Wh_Log(L"Found the Command Palette logo");
            AttachToLogo(image);
        }

        return S_OK;
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
        // Returning an error stops further notifications.
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_xamlDiagnostics;
};

[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {6B1E2D4A-7C3F-4E85-9A10-2F6D8C4B9E31}
constexpr CLSID CLSID_CommandPaletteLogoTAP = {
    0x6b1e2d4a,
    0x7c3f,
    0x4e85,
    {0x9a, 0x10, 0x2f, 0x6d, 0x8c, 0x4b, 0x9e, 0x31}};

class CommandPaletteLogoTAP
    : public winrt::
          implements<CommandPaletteLogoTAP, IObjectWithSite, winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override try {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->UnadviseVisualTreeChange();
            g_visualTreeWatcher = nullptr;
        }

        m_site.copy_from(pUnkSite);

        if (m_site) {
            // Decrease the refcount increased by InitializeXamlDiagnosticsEx.
            FreeLibrary(GetCurrentModuleHandle());
            g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(m_site);
        }

        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid,
                                      void** ppvSite) noexcept override {
        return m_site.as(riid, ppvSite);
    }

   private:
    winrt::com_ptr<IUnknown> m_site;
};

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter,
                                             REFIID riid,
                                             void** ppvObject) override try {
        *ppvObject = nullptr;
        if (pUnkOuter) {
            return CLASS_E_NOAGGREGATION;
        }
        return winrt::make<T>().as(riid, ppvObject);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID rclsid,
                                               REFIID riid,
                                               LPVOID* ppv) try {
    if (rclsid == CLSID_CommandPaletteLogoTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<CommandPaletteLogoTAP>>().as(riid,
                                                                       ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

using InitializeXamlDiagnosticsEx_t = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    HMODULE udk = GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!udk) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    auto initializeXamlDiagnosticsEx =
        reinterpret_cast<InitializeXamlDiagnosticsEx_t>(
            GetProcAddress(udk, "InitializeXamlDiagnosticsEx"));
    if (!initializeXamlDiagnosticsEx) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // The connection name isn't known, so try until one works. Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"WinUIVisualDiagConnection%d", i + 1);

        hr = initializeXamlDiagnosticsEx(connectionName, GetCurrentProcessId(),
                                         L"", location,
                                         CLSID_CommandPaletteLogoTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return hr;
}

void InjectTAPOnce() {
    if (g_tapInjected.exchange(true)) {
        return;
    }

    HRESULT hr = InjectTAP();
    Wh_Log(L"InjectTAP result: %08X", hr);
    if (FAILED(hr)) {
        g_tapInjected = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windows

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

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

bool IsXamlWindow(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcsncmp(className, L"WinUIDesktopWin32WindowClass", 28) == 0;
}

HWND FindXamlWindow() {
    HWND found = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() && IsXamlWindow(hWnd)) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&found));
    return found;
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
    if (hWnd && !g_tapInjected && IsXamlWindow(hWnd)) {
        Wh_Log(L"XAML window created");
        InjectTAPOnce();
    }
    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

void LoadSettings() {
    PCWSTR logo = Wh_GetStringSetting(L"logo");
    if (wcscmp(logo, L"custom") == 0) {
        g_settings.logo = LogoMode::Custom;
    } else if (wcscmp(logo, L"hidden") == 0) {
        g_settings.logo = LogoMode::Hidden;
    } else {
        g_settings.logo = LogoMode::Arrow;
    }
    Wh_FreeStringSetting(logo);

    PCWSTR path = Wh_GetStringSetting(L"customImagePath");
    std::wstring customImagePath = path;
    Wh_FreeStringSetting(path);

    // Accept paths copied with "Copy as path", which adds quotes.
    if (customImagePath.size() >= 2 && customImagePath.front() == L'"' &&
        customImagePath.back() == L'"') {
        customImagePath = customImagePath.substr(1, customImagePath.size() - 2);
    }
    g_settings.customImagePath = customImagePath;

    g_settings.size = Wh_GetIntSetting(L"size");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The Command Palette may be running already.
    if (HWND hWnd = FindXamlWindow()) {
        RunFromWindowThread(hWnd, [](PVOID) { InjectTAPOnce(); }, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    RunOnLogoThread([]() {
        if (auto image = g_logoImage.get()) {
            ApplyToLogo(image);
        }
    });
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    RunOnLogoThread([]() { DetachFromLogo(/*restore=*/true); });

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }
}
