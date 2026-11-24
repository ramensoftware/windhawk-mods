// ==WindhawkMod==
// @id adaptive-tray-contrast
// @name Adaptive Tray Contrast
// @description Adapts tray icons, individual clock and date characters, taskbar buttons and supported app icons to the background behind each.
// @description:zh-CN 根据各自背后的画面调整托盘图标、任务栏按钮和受支持的应用图标，时间与日期逐字符变色。 Adapts tray icons, individual clock and date characters, taskbar buttons and supported app icons to the background behind each.
// @version 0.3.15
// @author StillDialing
// @github https://github.com/StillDialing
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Taskbar host lookup follows the ABI documented in m417z's
// taskbar-tray-show-on-hover Windhawk mod (GPL-3.0).

// ==WindhawkModReadme==
/*
# Adaptive tray contrast

For transparent taskbars on Windows 11. Drag a light window
behind the clock on a dark desktop: digits over the window can turn black
while digits over the desktop stay white. The date is evaluated separately,
one character at a time.

The mod adapts supported monochrome tray glyphs and the input indicator, selects
light or dark themes for Start, Search, Task View, Widgets and app buttons, and
switches packaged app icons when matching light and dark assets are available.
Colored tray artwork and the colored battery layer retain their colors.
A packaged app's own dynamic icon can be covered by its themed package icon.

The default active sampling interval is 80 ms. Increase it in Settings to
sample less frequently. A sharp background boundary can cause brief flicker.

**App icons**

![App icon variants switching as the window moves](https://raw.githubusercontent.com/StillDialing/adaptive-tray-contrast/30bf07f5435b1cdcedbba3fda84d6ee295572809/images/app-icons.gif)

**Tray, clock and date**

![Tray icons and individual date digits adapting across a window edge](https://raw.githubusercontent.com/StillDialing/adaptive-tray-contrast/30bf07f5435b1cdcedbba3fda84d6ee295572809/images/tray-clock-date.gif)

## 中文说明

适用于 Windows 11 透明任务栏。把亮色窗口拖到深色桌面上的时钟后方，
同一行数字会按各自背后的明暗显示黑色或白色，日期也逐字符独立判断。

模组还会调整受支持的单色托盘图标、输入法指示器，以及开始、搜索、任务视图、小组件和应用按钮的主题。
自带深浅两套资源的打包应用可以切换图标，这可能盖住应用自行设置的动态图标。
彩色托盘图案和电池的彩色部分保留原色。默认每 80 毫秒采样一次，可在设置中调长间隔。
深浅交界处偶尔会短暂闪烁。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- samplingInterval: 80
  $name: Active sampling interval (milliseconds)
  $name:zh-CN: 活动采样间隔（毫秒）
  $description: Adjust between 40 and 2000. Larger values reduce sampling and respond more slowly.
  $description:zh-CN: 可设置为 40 至 2000，数值越大，采样越少，响应越慢。
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windhawk_utils.h>
#include <shellapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

namespace {

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::UI::Color;
using winrt::Windows::UI::Colors;
using winrt::Windows::Foundation::IInspectable;
namespace Imaging = winrt::Windows::UI::Xaml::Media::Imaging;
using winrt::Windows::Foundation::AsyncStatus;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::IAsyncOperation;
using winrt::Windows::Storage::Streams::IBuffer;

using GetHost = void*(WINAPI*)(void*, void**);
using ReleaseHost = void(WINAPI*)(void*);
using StartTaskbar = void(WINAPI*)(void*);
using GetPackagesByFamilyFn = LONG(WINAPI*)(PCWSTR, UINT32*, PWSTR*, UINT32*, WCHAR*);
using GetPackagePathByFullNameFn = LONG(WINAPI*)(PCWSTR, UINT32*, PWSTR);

GetHost g_primaryHost;
GetHost g_secondaryHost;
ReleaseHost g_releaseHost;
StartTaskbar g_originalStart;
void* g_primarySite;
void* g_secondarySite;
void* g_frameHeight;
std::atomic<bool> g_unloading;
std::atomic<HWND> g_samplerWindow;
std::mutex g_samplerLifecycle;
HMODULE g_module;
UINT g_controlMessage;
constexpr UINT kPollMs = 80;
std::atomic<UINT> g_activePollMs{kPollMs};
constexpr UINT kIdlePollMs = 240;
constexpr UINT_PTR kSampleTimer = 1;
constexpr wchar_t kSamplerClass[] = L"Windhawk.AdaptiveTrayContrast.Sampler";
UINT g_interval = kPollMs;

struct ClockCharacter {
    float left;
    float right;
    int index;
    int white = -1;
};

using ClockColors = std::vector<int>;

struct Contrast {
    double black = 0;
    double white = 0;
    bool valid = false;
};

struct Glyph {
    Contrast cachedContrast;
    winrt::weak_ref<FrameworkElement> element;
    DependencyProperty property{nullptr};
    IInspectable original{nullptr};
    Media::Brush applied{nullptr};
    Media::ImageSource imageSource{nullptr};
    Controls::Image imageOverlay{nullptr};
    Imaging::RenderTargetBitmap capture{nullptr};
    IAsyncAction render{nullptr};
    IAsyncOperation<IBuffer> readback{nullptr};
    Imaging::WriteableBitmap imageTint{nullptr};
    std::vector<uint8_t> imageAlpha;
    ULONGLONG nextImageCapture = 0;
    int paintedTint = -1;
    RECT bounds{};
    int chosen = -1;  // 0 = black, 1 = white.
    int pending = -1;
    int votes = 0;
    ULONGLONG nextClockLog = 0;
    bool clock = false;
    winrt::hstring clockText;
    double clockWidth = 0;
    double clockHeight = 0;
    std::vector<ClockCharacter> characters;
    ClockColors clockColors;
    std::array<Documents::TextHighlighter, 2> clockHighlights{nullptr, nullptr};
    bool present = false;
    bool ime = false;
    int lastImeState = -1;
};

struct DualIconPaths {
    bool valid = false;
    std::wstring darkBgPath;   // Icon for dark background (altform-unplated / theme-dark)
    std::wstring lightBgPath;  // Icon for light background (altform-lightunplated / theme-light)
};

struct ThemeButton {
    Contrast cachedMarginContrast;
    winrt::weak_ref<FrameworkElement> element;
    IInspectable original{nullptr};
    ElementTheme last = ElementTheme::Default;
    RECT bounds{};
    bool seen = false;
    bool isTaskList = false;
    bool iconResolved = false;
    int iconTargetPx = 0;
    ULONGLONG iconStableAfter = 0;
    Media::ImageSource nativeSource{nullptr};
    bool iconHidden = false;
    winrt::hstring autoId;
    winrt::weak_ref<Controls::Image> iconImage;
    IInspectable originalIconOpacity{nullptr};
    Controls::Image iconOverlay{nullptr};
    Media::ImageSource appliedIcon{nullptr};
    Imaging::BitmapImage darkBgIcon{nullptr};
    Imaging::BitmapImage lightBgIcon{nullptr};
};

Controls::Panel GetVisualParentPanel(DependencyObject const& child) {
    if (!child) return nullptr;
    if (auto p = Media::VisualTreeHelper::GetParent(child).try_as<Controls::Panel>()) {
        return p;
    }
    if (auto fe = child.try_as<FrameworkElement>()) {
        return fe.Parent().try_as<Controls::Panel>();
    }
    return nullptr;
}

void RemoveAllDualOverlays(DependencyObject const& node, int depth = 0) {
    if (!node || depth > 8) return;
    try {
        if (auto panel = node.try_as<Controls::Panel>()) {
            auto children = panel.Children();
            for (int i = static_cast<int>(children.Size()) - 1; i >= 0; --i) {
                if (auto img = children.GetAt(static_cast<uint32_t>(i)).try_as<Controls::Image>()) {
                    const auto name = img.Name();
                    if (name == L"AdaptiveTrayDualIcon") {
                        img.Visibility(Visibility::Collapsed);
                        children.RemoveAt(static_cast<uint32_t>(i));
                    }
                }
            }
        }
        const int count = Media::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < count; ++i) {
            RemoveAllDualOverlays(Media::VisualTreeHelper::GetChild(node, i), depth + 1);
        }
    } catch (...) {
    }
}

void RestoreIconOpacity(ThemeButton& button) {
    if (button.iconHidden) {
        if (auto img = button.iconImage.get()) {
            try {
                const auto prop = UIElement::OpacityProperty();
                if (img.Opacity() != 0.0) {
                    // Explorer already supplied a new value.
                } else if (!button.originalIconOpacity ||
                    button.originalIconOpacity == DependencyProperty::UnsetValue()) {
                    img.ClearValue(prop);
                } else {
                    img.SetValue(prop, button.originalIconOpacity);
                }
            } catch (...) {
            }
        }
        button.iconHidden = false;
    }
}

void ReleaseIconVisual(ThemeButton& button) {
    if (button.iconOverlay) {
        try {
            button.iconOverlay.Visibility(Visibility::Collapsed);
            if (auto parent = GetVisualParentPanel(button.iconOverlay)) {
                uint32_t index = 0;
                if (parent.Children().IndexOf(button.iconOverlay, index)) {
                    parent.Children().RemoveAt(index);
                }
            }
        } catch (...) {
        }
        button.iconOverlay = nullptr;
    }
    RestoreIconOpacity(button);
    button.nativeSource = nullptr;
    button.iconImage = {};
    button.originalIconOpacity = nullptr;
    button.appliedIcon = nullptr;
}

void ResetButtonDualIcons(ThemeButton& button) {
    ReleaseIconVisual(button);
    button.iconResolved = false;
    button.darkBgIcon = nullptr;
    button.lightBgIcon = nullptr;
}

void RestoreButton(ThemeButton& button) {
    ResetButtonDualIcons(button);
    auto element = button.element.get();
    if (!element || button.last == ElementTheme::Default || element.RequestedTheme() != button.last) return;
    const auto property = FrameworkElement::RequestedThemeProperty();
    if (button.original == DependencyProperty::UnsetValue()) element.ClearValue(property);
    else element.SetValue(property, button.original);
}

struct Tray {
    std::vector<uint32_t> previousPixels;
    RECT previousRegion{};
    std::vector<ThemeButton> buttons;
    HWND window{};
    winrt::weak_ref<FrameworkElement> root;
    std::vector<Glyph> glyphs;
    size_t lastCount = static_cast<size_t>(-1);
    ULONGLONG nextDiscovery = 0;
};

// Accessed only on the primary taskbar's UI thread.
std::vector<Tray>& Trays() {
    [[clang::no_destroy]] static std::vector<Tray> trays;
    return trays;
}

std::unordered_map<std::wstring, DualIconPaths>& DualIconCache() {
    static std::unordered_map<std::wstring, DualIconPaths> cache;
    return cache;
}

std::wstring ToLower(std::wstring_view s) {
    std::wstring out(s);
    for (auto& ch : out) {
        ch = static_cast<wchar_t>(std::towlower(ch));
    }
    return out;
}

std::wstring FilePathToUri(std::wstring const& path) {
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, path.data(), static_cast<int>(path.size()),
                                      nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) return {};
    std::string utf8(static_cast<size_t>(utf8Len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, path.data(), static_cast<int>(path.size()),
                        utf8.data(), utf8Len, nullptr, nullptr);
    std::wstring uri = L"file:///";
    static constexpr wchar_t kHex[] = L"0123456789ABCDEF";
    for (unsigned char c : utf8) {
        if (c == '\\') {
            uri.push_back(L'/');
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9') || c == '-' || c == '_' ||
                   c == '.' || c == '~' || c == '/' || c == ':') {
            uri.push_back(static_cast<wchar_t>(c));
        } else {
            uri.push_back(L'%');
            uri.push_back(kHex[(c >> 4) & 0xF]);
            uri.push_back(kHex[c & 0xF]);
        }
    }
    return uri;
}

std::wstring GetPackageInstallPath(std::wstring const& familyName) {
    HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    if (!kernel) return {};
    static const auto getByFamily = reinterpret_cast<GetPackagesByFamilyFn>(
        GetProcAddress(kernel, "GetPackagesByPackageFamily"));
    static const auto getPath = reinterpret_cast<GetPackagePathByFullNameFn>(
        GetProcAddress(kernel, "GetPackagePathByFullName"));
    if (!getByFamily || !getPath) return {};

    UINT32 count = 0;
    UINT32 bufLen = 0;
    LONG rc = getByFamily(familyName.c_str(), &count, nullptr, &bufLen, nullptr);
    if (rc != ERROR_INSUFFICIENT_BUFFER || count == 0 || bufLen == 0) {
        return {};
    }
    std::vector<PWSTR> names(count);
    std::vector<WCHAR> buf(bufLen);
    if (getByFamily(familyName.c_str(), &count, names.data(), &bufLen, buf.data()) != ERROR_SUCCESS ||
        count == 0 || !names[0]) {
        return {};
    }
    UINT32 pathLen = 0;
    rc = getPath(names[0], &pathLen, nullptr);
    if (rc != ERROR_INSUFFICIENT_BUFFER || pathLen == 0) {
        return {};
    }
    std::wstring path(pathLen, L'\0');
    if (getPath(names[0], &pathLen, path.data()) != ERROR_SUCCESS) {
        return {};
    }
    if (!path.empty() && path.back() == L'\0') {
        path.pop_back();
    }
    return path;
}

std::wstring ExtractSquare44Logo(std::wstring const& manifestPath, std::wstring const& appId) {
    std::ifstream file(manifestPath.c_str(), std::ios::binary);
    if (!file) return {};
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    if (content.empty()) return {};

    using namespace winrt::Windows::Data::Xml::Dom;
    winrt::Windows::Storage::Streams::DataWriter writer;
    const auto* bytes = reinterpret_cast<const uint8_t*>(content.data());
    writer.WriteBytes(winrt::array_view<const uint8_t>(bytes, bytes + content.size()));
    XmlDocument doc;
    XmlLoadSettings settings;
    settings.ProhibitDtd(true);
    doc.LoadXmlFromBuffer(writer.DetachBuffer(), settings);
    const auto apps = doc.SelectNodes(
        L"/*[local-name()='Package']/*[local-name()='Applications']/*[local-name()='Application']");
    for (auto const& node : apps) {
        const auto app = node.as<XmlElement>();
        if (app.GetAttribute(L"Id") != winrt::hstring(appId)) continue;
        auto visual = app.SelectSingleNode(L"./*[local-name()='VisualElements']");
        if (visual) return std::wstring(visual.as<XmlElement>().GetAttribute(L"Square44x44Logo"));
        break;
    }
    return {};
}

int ParseNumberAfter(std::wstring_view text, std::wstring_view prefix) {
    const size_t pos = text.find(prefix);
    if (pos == std::wstring_view::npos) return -1;
    size_t idx = pos + prefix.size();
    int value = 0;
    bool any = false;
    while (idx < text.size() && text[idx] >= L'0' && text[idx] <= L'9') {
        if (value > 10000) return -1;
        value = value * 10 + (text[idx] - L'0');
        any = true;
        ++idx;
    }
    return any ? value : -1;
}

int IconResolutionScore(std::wstring_view lowerName, int targetPx) {
    int px = ParseNumberAfter(lowerName, L"targetsize-");
    if (px <= 0) {
        const int scale = ParseNumberAfter(lowerName, L"scale-");
        px = scale > 0 ? (44 * scale) / 100 : 44;
    }
    return px >= targetPx ? (px - targetPx) : (1000 + (targetPx - px));
}

void ScanIconFolder(std::wstring const& folder,
                    std::wstring const& qualifierPrefix,
                    std::wstring const& stemLower,
                    int targetPx,
                    int depth,
                    std::pair<int, std::wstring>& bestDarkUnplated,
                    std::pair<int, std::wstring>& bestLightUnplated,
                    std::pair<int, std::wstring>& bestThemeDark,
                    std::pair<int, std::wstring>& bestThemeLight) {
    if (depth > 2) return;
    WIN32_FIND_DATAW fd{};
    HANDLE find = FindFirstFileW((folder + L"\\*").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        const std::wstring name(fd.cFileName);
        const std::wstring lowerName = ToLower(name);
        const std::wstring fullPath = folder + L"\\" + name;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // A sibling app can use the very same logo basename in its own subfolder.
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
                !(lowerName.starts_with(L"scale-") || lowerName.starts_with(L"targetsize-") ||
                  lowerName.starts_with(L"altform-") || lowerName.starts_with(L"theme-"))) continue;
            const std::wstring subPrefix = qualifierPrefix.empty()
                ? lowerName : (qualifierPrefix + L"/" + lowerName);
            ScanIconFolder(fullPath, subPrefix, stemLower, targetPx, depth + 1,
                           bestDarkUnplated, bestLightUnplated, bestThemeDark, bestThemeLight);
            continue;
        }
        if (lowerName.size() <= 4 ||
            lowerName.compare(lowerName.size() - 4, 4, L".png") != 0) {
            continue;
        }
        const bool stemMatches =
            lowerName == (stemLower + L".png") ||
            (lowerName.size() > stemLower.size() + 1 &&
             lowerName.compare(0, stemLower.size(), stemLower) == 0 &&
             lowerName[stemLower.size()] == L'.');
        if (!stemMatches) continue;

        const std::wstring combined = qualifierPrefix.empty()
            ? lowerName : (qualifierPrefix + L"/" + lowerName);
        if (combined.find(L"contrast-black") != std::wstring::npos ||
            combined.find(L"contrast-white") != std::wstring::npos ||
            combined.find(L"contrast-high") != std::wstring::npos) {
            continue;
        }
        const int score = IconResolutionScore(combined, targetPx);
        if (combined.find(L"altform-lightunplated") != std::wstring::npos) {
            if (score < bestLightUnplated.first) bestLightUnplated = {score, fullPath};
        } else if (combined.find(L"altform-unplated") != std::wstring::npos) {
            if (score < bestDarkUnplated.first) bestDarkUnplated = {score, fullPath};
        } else if (combined.find(L"theme-light") != std::wstring::npos) {
            if (score < bestThemeLight.first) bestThemeLight = {score, fullPath};
        } else if (combined.find(L"theme-dark") != std::wstring::npos) {
            if (score < bestThemeDark.first) bestThemeDark = {score, fullPath};
        }
    } while (FindNextFileW(find, &fd));
    FindClose(find);
}

DualIconPaths ResolveDualIcons(std::wstring const& autoId, double scale) {
    const int targetPx = std::max(24, static_cast<int>(std::lround(24.0 * scale)));
    const auto cacheKey = autoId + L"|" + std::to_wstring(targetPx);
    auto& cache = DualIconCache();
    if (auto it = cache.find(cacheKey); it != cache.end()) {
        return it->second;
    }
    DualIconPaths result;

    constexpr std::wstring_view kPrefix = L"Appid: ";
    std::wstring_view view(autoId);
    if (view.size() > kPrefix.size() && view.substr(0, kPrefix.size()) == kPrefix) {
        view.remove_prefix(kPrefix.size());
    }
    const size_t bang = view.find(L'!');
    if (bang == std::wstring_view::npos || bang == 0 || bang + 1 == view.size()) {
        cache.emplace(cacheKey, result);
        return result;
    }
    const std::wstring family(view.substr(0, bang));
    const std::wstring appId(view.substr(bang + 1));
    const std::wstring pkgPath = GetPackageInstallPath(family);
    if (pkgPath.empty()) {
        cache.emplace(cacheKey, result);
        return result;
    }
    std::wstring relLogo = ExtractSquare44Logo(pkgPath + L"\\AppxManifest.xml", appId);
    if (relLogo.empty()) {
        cache.emplace(cacheKey, result);
        return result;
    }
    for (auto& ch : relLogo) {
        if (ch == L'/') ch = L'\\';
    }
    std::wstring relDir;
    std::wstring fileName = relLogo;
    if (const size_t slash = relLogo.find_last_of(L'\\'); slash != std::wstring::npos) {
        relDir = relLogo.substr(0, slash);
        fileName = relLogo.substr(slash + 1);
    }
    std::wstring stem = fileName;
    if (const size_t dot = fileName.find_last_of(L'.'); dot != std::wstring::npos) {
        stem = fileName.substr(0, dot);
    }
    const std::wstring searchDir = relDir.empty() ? pkgPath : (pkgPath + L"\\" + relDir);

    std::pair<int, std::wstring> bestDarkUnplated{INT_MAX, {}};
    std::pair<int, std::wstring> bestLightUnplated{INT_MAX, {}};
    std::pair<int, std::wstring> bestThemeDark{INT_MAX, {}};
    std::pair<int, std::wstring> bestThemeLight{INT_MAX, {}};
    ScanIconFolder(searchDir, {}, ToLower(stem), targetPx, 0,
                   bestDarkUnplated, bestLightUnplated, bestThemeDark, bestThemeLight);

    if (!bestDarkUnplated.second.empty() && !bestLightUnplated.second.empty()) {
        result.darkBgPath = bestDarkUnplated.second;
        result.lightBgPath = bestLightUnplated.second;
        result.valid = true;
    } else if (!bestThemeDark.second.empty() && !bestThemeLight.second.empty()) {
        result.darkBgPath = bestThemeDark.second;
        result.lightBgPath = bestThemeLight.second;
        result.valid = true;
    }
    if (result.valid) {
        Wh_Log(L"Dual icon ready [%s]: dark=[%s] light=[%s]",
               family.c_str(), result.darkBgPath.c_str(), result.lightBgPath.c_str());
    }
    cache.emplace(cacheKey, result);
    return result;
}

Controls::Image FindButtonIconImage(DependencyObject const& node, int depth = 0) {
    if (!node || depth > 6) return nullptr;
    if (auto img = node.try_as<Controls::Image>()) {
        if (img.Name() == L"Icon") return img;
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindButtonIconImage(Media::VisualTreeHelper::GetChild(node, i), depth + 1)) {
            return found;
        }
    }
    return nullptr;
}

void AttachDualIcons(ThemeButton& button, double scale) {
    button.iconResolved = true;
    try {
        if (button.autoId.empty()) return;
        const auto paths = ResolveDualIcons(std::wstring(button.autoId), scale);
        if (!paths.valid) return;

        const auto darkUri = FilePathToUri(paths.darkBgPath);
        const auto lightUri = FilePathToUri(paths.lightBgPath);
        if (darkUri.empty() || lightUri.empty()) return;
        Imaging::BitmapImage darkBmp{winrt::Windows::Foundation::Uri(darkUri)};
        Imaging::BitmapImage lightBmp{winrt::Windows::Foundation::Uri(lightUri)};
        button.darkBgIcon = darkBmp;
        button.lightBgIcon = lightBmp;
    } catch (winrt::hresult_error const& err) {
        Wh_Log(L"Dual icon attach failed: 0x%08X", static_cast<unsigned>(err.code().value));
    }
}

// Also run on neutral backgrounds, before any sampling or theme change.
void SyncButtonIcon(ThemeButton& button, FrameworkElement const& element) {
    try {
        const auto id = Automation::AutomationProperties::GetAutomationId(element);
        const double scale = element.XamlRoot() ? element.XamlRoot().RasterizationScale() : 1.0;
        const int target = std::max(24, static_cast<int>(std::lround(24.0 * scale)));
        if (id != button.autoId || target != button.iconTargetPx) {
            ResetButtonDualIcons(button);
            button.autoId = id;
            button.iconTargetPx = target;
        }
        if (!button.iconResolved) AttachDualIcons(button, scale);
        if (!button.darkBgIcon) return;
        auto img = FindButtonIconImage(element);
        auto old = button.iconImage.get();
        const auto source = img ? img.Source() : nullptr;
        bool changed = old != img || button.nativeSource != source;
        if (button.iconOverlay && !changed) {
            auto parent = img ? GetVisualParentPanel(img) : nullptr;
            changed = !parent || parent != GetVisualParentPanel(button.iconOverlay);
        }
        if (changed) {
            ReleaseIconVisual(button);
            if (img) button.iconImage = winrt::make_weak(img);
            button.nativeSource = source;
            // Explorer can publish the new image and automation ID in separate ticks.
            button.iconStableAfter = GetTickCount64() + 2 * kPollMs;
        }
    } catch (...) {
        ResetButtonDualIcons(button);
    }
}

void ApplyButtonIcon(ThemeButton& button, bool darkBackground) {
    try {
        if (GetTickCount64() < button.iconStableAfter) return;
        auto img = button.iconImage.get();
        if (!img || !button.darkBgIcon || !button.lightBgIcon) return;
        if (!button.iconHidden && (img.Opacity() != 1.0 || !img.Source())) return;
        const auto desired = darkBackground
            ? button.darkBgIcon.as<Media::ImageSource>()
            : button.lightBgIcon.as<Media::ImageSource>();

        if (!button.iconOverlay) {
            auto parent = GetVisualParentPanel(img);
            if (!parent) return;
            auto siblings = parent.Children();
            uint32_t index = 0;
            if (!siblings.IndexOf(img, index)) return;

            Controls::Image overlay;
            overlay.Name(L"AdaptiveTrayDualIcon");
            overlay.IsHitTestVisible(false);
            overlay.Stretch(img.Stretch());
            overlay.HorizontalAlignment(img.HorizontalAlignment());
            overlay.VerticalAlignment(img.VerticalAlignment());
            overlay.Margin(img.Margin());
            if (img.ActualWidth() > 0 && img.ActualHeight() > 0) {
                overlay.Width(img.ActualWidth());
                overlay.Height(img.ActualHeight());
            }
            overlay.Opacity(0.0);
            overlay.Source(desired);
            Controls::Grid::SetColumn(overlay, Controls::Grid::GetColumn(img));
            Controls::Grid::SetRow(overlay, Controls::Grid::GetRow(img));
            Controls::Grid::SetColumnSpan(overlay, Controls::Grid::GetColumnSpan(img));
            Controls::Grid::SetRowSpan(overlay, Controls::Grid::GetRowSpan(img));
            siblings.InsertAt(index + 1, overlay);
            button.iconOverlay = overlay;
            button.appliedIcon = desired;
        } else {
            if (img.ActualWidth() > 0 && img.ActualHeight() > 0) {
                if (button.iconOverlay.Width() != img.ActualWidth()) {
                    button.iconOverlay.Width(img.ActualWidth());
                }
                if (button.iconOverlay.Height() != img.ActualHeight()) {
                    button.iconOverlay.Height(img.ActualHeight());
                }
            }
            if (button.appliedIcon != desired) {
                button.iconOverlay.Opacity(0.0);
                RestoreIconOpacity(button);
                button.iconOverlay.Source(desired);
                button.appliedIcon = desired;
            }
        }

        auto bitmap = darkBackground ? button.darkBgIcon : button.lightBgIcon;
        if (bitmap.PixelWidth() <= 0 || bitmap.PixelHeight() <= 0 || !img.Source()) {
            button.iconOverlay.Opacity(0.0);
            RestoreIconOpacity(button);
            return;
        }
        if (!button.iconHidden) {
            auto local = img.ReadLocalValue(UIElement::OpacityProperty());
            if (local != DependencyProperty::UnsetValue() &&
                !local.try_as<winrt::Windows::Foundation::IPropertyValue>()) return;
            button.originalIconOpacity = local;
            img.Opacity(0.0);
            button.iconHidden = true;
        } else if (img.Opacity() != 0.0) {
            button.iconOverlay.Opacity(0.0);
            RestoreIconOpacity(button);
            return;
        }
        if (button.iconOverlay.Opacity() != 1.0) button.iconOverlay.Opacity(1.0);
    } catch (...) {
        ResetButtonDualIcons(button);
    }
}

bool IsSecondary(HWND window) {
    wchar_t name[40]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           wcscmp(name, L"Shell_SecondaryTrayWnd") == 0;
}

XamlRoot ReadRoot(HWND window) {
    const bool secondary = IsSecondary(window);
    const auto getHost = secondary ? g_secondaryHost : g_primaryHost;
    const auto site = secondary ? g_secondarySite : g_primarySite;
    if (!getHost || !site) {
        return nullptr;
    }

    size_t offset = 0;
#if defined(_M_X64)
    const auto* code = static_cast<const BYTE*>(g_frameHeight);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7F) {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    // Match the FrameHeight prologue and its positive pre-indexed LDR offset.
    const auto* code = static_cast<const DWORD*>(g_frameHeight);
    if (code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD && (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (code[3] >> 12) & 0xFF;
    }
#else
#error Unsupported architecture
#endif
    if (offset == 0 || offset % sizeof(void*) != 0) {
        static bool logged = false;
        if (!logged) {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight layout. Skipping.");
            logged = true;
        }
        return nullptr;
    }

    HWND bandWindow = secondary
        ? FindWindowExW(window, nullptr, L"WorkerW", nullptr)
        : static_cast<HWND>(GetPropW(window, L"TaskbandHWND"));
    if (!bandWindow) {
        return nullptr;
    }
    auto band = reinterpret_cast<void**>(GetWindowLongPtrW(bandWindow, 0));
    if (!band) {
        return nullptr;
    }
    for (size_t slot = 0; slot < 20; ++slot) {
        void* candidate = nullptr;
        SIZE_T bytesRead = 0;
        if (!ReadProcessMemory(GetCurrentProcess(), band + slot, &candidate,
                               sizeof(candidate), &bytesRead)) {
            break;
        }
        if (candidate != site) {
            continue;
        }
        void* host[2]{};
        getHost(band + slot, host);
        XamlRoot root{nullptr};
        IUnknown* element = nullptr;
        if (host[0] &&
            ReadProcessMemory(GetCurrentProcess(),
                              static_cast<const BYTE*>(host[0]) + offset,
                              &element, sizeof(element), &bytesRead) &&
            element) {
            FrameworkElement xaml{nullptr};
            if (SUCCEEDED(element->QueryInterface(
                    winrt::guid_of<FrameworkElement>(), winrt::put_abi(xaml))) &&
                xaml) {
                root = xaml.XamlRoot();
            }
        }
        if (host[1]) {
            g_releaseHost(host[1]);
        }
        return root;
    }
    return nullptr;
}

FrameworkElement FindTray(DependencyObject const& node, int depth = 0) {
    if (!node || depth > 12) {
        return nullptr;
    }
    if (winrt::get_class_name(node) == L"SystemTray.SystemTrayFrame") {
        return node.try_as<FrameworkElement>();
    }
    const auto count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto found = FindTray(Media::VisualTreeHelper::GetChild(node, i), depth + 1);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

void ClearClockHighlights(Glyph& glyph, FrameworkElement const& element) {
    if (glyph.clockHighlights[0] || glyph.clockHighlights[1]) {
        if (auto text = element ? element.try_as<Controls::TextBlock>() : nullptr) {
            auto highlighters = text.TextHighlighters();
            for (auto const& item : glyph.clockHighlights) {
                uint32_t index = 0;
                if (item && highlighters.IndexOf(item, index)) {
                    highlighters.RemoveAt(index);
                }
            }
        }
        glyph.clockHighlights = {nullptr, nullptr};
    }
    glyph.clockColors.clear();
}

void Restore(Glyph& glyph) {
    auto element = glyph.element.get();
    ClearClockHighlights(glyph, element);
    if (glyph.imageOverlay) {
        if (auto parent = glyph.imageOverlay.Parent().try_as<Controls::Grid>()) {
            uint32_t index = 0;
            if (parent.Children().IndexOf(glyph.imageOverlay, index)) {
                parent.Children().RemoveAt(index);
            }
        }
        glyph.imageOverlay = nullptr;
    }
    glyph.imageTint = nullptr;
    glyph.imageSource = nullptr;
    glyph.capture = nullptr;
    glyph.render = nullptr;
    glyph.readback = nullptr;
    glyph.imageAlpha.clear();
    glyph.nextImageCapture = 0;
    glyph.paintedTint = -1;
    if (element && glyph.applied &&
        element.ReadLocalValue(glyph.property) == glyph.applied) {
        if (glyph.original == DependencyProperty::UnsetValue()) {
            element.ClearValue(glyph.property);
        } else if (glyph.original) {
            element.SetValue(glyph.property, glyph.original);
        }
    }
    glyph.applied = nullptr;
}

void RestoreTray(Tray& tray) {
    for (auto& glyph : tray.glyphs) {
        Restore(glyph);
    }
    for (auto& button : tray.buttons) RestoreButton(button);
    tray.buttons.clear();
    tray.glyphs.clear();
    tray.root = {};
}

bool ScreenBounds(FrameworkElement const& element, HWND window, RECT& bounds) {
    const auto root = element.XamlRoot();
    const double width = element.ActualWidth();
    const double height = element.ActualHeight();
    if (!root || !(width > 0) || !(height > 0)) {
        return false;
    }
    const auto transform = element.TransformToVisual(nullptr);
    const auto box = transform.TransformBounds(
        {0, 0, static_cast<float>(width), static_cast<float>(height)});
    const double scale = root.RasterizationScale();
    POINT origin{};
    if (!ClientToScreen(window, &origin)) {
        return false;
    }
    bounds = {
        origin.x + static_cast<LONG>(std::floor(box.X * scale)),
        origin.y + static_cast<LONG>(std::floor(box.Y * scale)),
        origin.x + static_cast<LONG>(std::ceil((box.X + box.Width) * scale)),
        origin.y + static_cast<LONG>(std::ceil((box.Y + box.Height) * scale)),
    };
    return bounds.right > bounds.left && bounds.bottom > bounds.top;
}

bool Monochrome(Media::SolidColorBrush const& brush) {
    if (!brush) {
        return false;
    }
    const auto c = brush.Color();
    return c.A != 0 &&
           std::max({c.R, c.G, c.B}) - std::min({c.R, c.G, c.B}) <= 8;
}

int RelativeLuminance(uint32_t pixel) {
    static const auto linear = [] {
        std::array<double, 256> table{};
        for (size_t i = 0; i < table.size(); ++i) {
            const double channel = i / 255.0;
            table[i] = channel <= 0.04045 ? channel / 12.92
                                         : std::pow((channel + 0.055) / 1.055, 2.4);
        }
        return table;
    }();
    return static_cast<int>(255 * (0.2126 * linear[(pixel >> 16) & 255] +
                                   0.7152 * linear[(pixel >> 8) & 255] +
                                   0.0722 * linear[pixel & 255]) + 0.5);
}

Contrast ScoreContrast(std::array<int, 256> const& histogram, int count) {
    if (count < 8) {
        return {};
    }
    double blackAverage = 0;
    double whiteAverage = 0;
    double blackTail = 0;
    double whiteTail = 0;
    int cumulative = 0;
    const int lowRank = (count + 4) / 5;
    const int highRank = count - lowRank + 1;
    for (int value = 0; value < 256; ++value) {
        const int weight = histogram[value];
        if (!weight) {
            continue;
        }
        const double luminance = static_cast<double>(value) / 255;
        const double black = std::log((luminance + 0.05) / 0.05);
        const double white = std::log(1.05 / (luminance + 0.05));
        blackAverage += weight * black;
        whiteAverage += weight * white;
        if (cumulative < lowRank && cumulative + weight >= lowRank) {
            blackTail = black;
        }
        if (cumulative < highRank && cumulative + weight >= highRank) {
            whiteTail = white;
        }
        cumulative += weight;
    }
    return {0.7 * blackTail + 0.3 * blackAverage / count,
            0.7 * whiteTail + 0.3 * whiteAverage / count, true};
}

bool IsChineseImeModeGlyph(winrt::hstring const& label) {
    if (label.size() != 1) {
        return false;
    }
    switch (label.c_str()[0]) {
        case L'中':
        case L'英':
        case L'简':
        case L'繁':
        case L'\uE982':
        case L'\uE983':
        case L'\uE4D7':
        case L'\uE4D8':
        case L'\uE5BF':
            return true;
        default:
            return false;
    }
}

void Collect(Tray& tray, DependencyObject const& node, bool systemGroup = false,
             bool inputStack = false, bool inputContent = false,
             bool inputModeImage = false, int depth = 0) {
    if (!node || depth > 32) {
        return;
    }
    auto element = node.try_as<FrameworkElement>();
    if (element) {
        if (element.Visibility() != Visibility::Visible || element.Opacity() == 0 ||
            element.Name() == L"AdaptiveTrayImeTint") {
            return;
        }
        const auto name = element.Name();
        const auto type = winrt::get_class_name(element);
        const bool clock = name == L"TimeInnerTextBlock" || name == L"DateInnerTextBlock";
        inputStack |= name == L"NonActivatableStack";
        inputContent |= type == L"SystemTray.LanguageTextIconContent" ||
                        (inputStack && type == L"SystemTray.TextIconContent");
        if (type == L"SystemTray.IconView" || type == L"SystemTray.LanguageImageIconContent") {
            inputModeImage = false;
        }
        inputModeImage |= inputStack && type == L"SystemTray.ImageIconContent";
        const bool isImage = !!element.try_as<Controls::Image>();
        if (isImage && !inputModeImage) {
            return;
        }
        systemGroup |= type == L"SystemTray.ChevronIconView" ||
                       type == L"SystemTray.LanguageTextIconContent" ||
                       type == L"SystemTray.BatteryIconContent" ||
                       name == L"ControlCenterButton" ||
                       name == L"NotificationCenterButton";
        DependencyProperty property{nullptr};
        winrt::hstring label;
        bool ime = inputContent || (isImage && inputModeImage);
        if (auto text = element.try_as<Controls::TextBlock>()) {
            label = text.Text();
            ime |= IsChineseImeModeGlyph(label);
            if (!label.empty() &&
                (systemGroup || ime || clock)) {
                property = Controls::TextBlock::ForegroundProperty();
            }
        } else if (auto icon = element.try_as<Controls::FontIcon>()) {
            label = icon.Glyph();
            ime |= IsChineseImeModeGlyph(label);
            if (systemGroup || ime) {
                property = Controls::IconElement::ForegroundProperty();
            }
        }

        RECT bounds{};
        const bool hasBounds = (property || (isImage && inputModeImage)) &&
                               ScreenBounds(element, tray.window, bounds);
        if (hasBounds) {
            const auto it = std::find_if(tray.glyphs.begin(), tray.glyphs.end(),
                [&](Glyph const& item) { return item.element.get() == element; });
            if (it != tray.glyphs.end()) {
                it->bounds = bounds;
                it->present = true;
                it->ime = ime;
                it->clock = clock;
            } else {
                Glyph glyph;
                glyph.element = winrt::make_weak(element);
                glyph.property = property;
                glyph.bounds = bounds;
                glyph.present = true;
                glyph.ime = ime;
                glyph.clock = clock;
                tray.glyphs.push_back(std::move(glyph));
                Wh_Log(L"Matched tray foreground: %s / %s%s", type.c_str(),
                       name.c_str(), isImage ? L" (IME mode image)" : L"");
            }
        }
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        Collect(tray, Media::VisualTreeHelper::GetChild(node, i),
                systemGroup, inputStack, inputContent, inputModeImage, depth + 1);
    }
}

class DesktopStrip {
public:
    DesktopStrip() = default;
    DesktopStrip(DesktopStrip const&) = delete;
    DesktopStrip& operator=(DesktopStrip const&) = delete;

    ~DesktopStrip() {
        if (previous_) {
            SelectObject(memory_, previous_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        if (memory_) {
            DeleteDC(memory_);
        }
        if (desktop_) {
            ReleaseDC(nullptr, desktop_);
        }
    }

    bool Capture(RECT rect) {
        const int height = rect.bottom - rect.top;
        width_ = rect.right - rect.left;
        if (width_ <= 0 || height <= 0 || width_ > 8192 || height > 512) {
            return false;
        }
        rect_ = rect;
        desktop_ = GetDC(nullptr);
        if (!desktop_) {
            return false;
        }
        memory_ = CreateCompatibleDC(desktop_);
        if (!memory_) {
            return false;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width_;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* data = nullptr;
        bitmap_ = CreateDIBSection(memory_, &info, DIB_RGB_COLORS, &data, nullptr, 0);
        if (!bitmap_ || !data) {
            return false;
        }
        previous_ = SelectObject(memory_, bitmap_);
        if (!previous_) {
            return false;
        }
        if (!BitBlt(memory_, 0, 0, width_, height, desktop_, rect.left, rect.top,
                    SRCCOPY | CAPTUREBLT)) {
            return false;
        }
        GdiFlush();
        pixels_ = static_cast<const uint32_t*>(data);
        return true;
    }

    bool MatchesPreviousFrame(Tray& tray) const {
        const auto count = static_cast<size_t>(width_) * (rect_.bottom - rect_.top);
        if (EqualRect(&rect_, &tray.previousRegion) && tray.previousPixels.size() == count &&
            std::equal(tray.previousPixels.begin(), tray.previousPixels.end(), pixels_)) {
            return true;
        }
        tray.previousPixels.assign(pixels_, pixels_ + count);
        tray.previousRegion = rect_;
        return false;
    }

    Contrast ClockCellContrast(RECT cell) const {
        RECT area{};
        if (!IntersectRect(&area, &cell, &rect_)) return {};
        std::array<int, 256> histogram{};
        int count = 0;
        for (LONG y = area.top; y < area.bottom; ++y) {
            for (LONG x = area.left; x < area.right; ++x) {
                ++histogram[RelativeLuminance(pixels_[(y - rect_.top) * width_ + x - rect_.left])];
                ++count;
            }
        }
        int accumulated = 0;
        for (int level = 0; level < 256; ++level) {
            accumulated += histogram[level];
            if (2 * accumulated >= count) {
                const double luminance = level / 255.0 + 0.05;
                return {luminance / 0.05, 1.05 / luminance, true};
            }
        }
        return {};
    }

    Contrast BackgroundContrast(RECT glyph, std::vector<Glyph> const& occupied) const {
        std::array<int, 256> histogram{};
        int count = 0;
        auto level = [&](int x, int y, bool excludeGlyphs) {
            if (x < rect_.left || x >= rect_.right || y < rect_.top || y >= rect_.bottom) {
                return -1;
            }
            if (excludeGlyphs && std::any_of(occupied.begin(), occupied.end(),
                    [x, y](Glyph const& other) {
                        return other.present && x >= other.bounds.left && x < other.bounds.right &&
                               y >= other.bounds.top && y < other.bounds.bottom;
                    })) {
                return -1;
            }
            return RelativeLuminance(pixels_[(y - rect_.top) * width_ + (x - rect_.left)]);
        };
        auto add = [&](int value, int weight) {
            if (value >= 0) {
                histogram[value] += weight;
                count += weight;
            }
        };
        auto closeInBrightness = [](int value, int reference) {
            return reference >= 0 && std::abs(value - reference) <= 9;
        };
        const auto xStep = std::max(2L, (glyph.right - glyph.left) / 30);
        const auto yStep = std::max(2L, (glyph.bottom - glyph.top) / 16);
        for (LONG y = glyph.top; y < glyph.bottom; y += yStep) {
            const int left = level(glyph.left - 2, y, true);
            const int right = level(glyph.right + 1, y, true);
            add(left, 2);
            add(right, 2);
            for (LONG x = glyph.left; x < glyph.right; x += xStep) {
                const int value = level(x, y, false);
                if (value >= 0 &&
                    (closeInBrightness(value, left) || closeInBrightness(value, right) ||
                     closeInBrightness(value, level(x, glyph.top - 2, true)) ||
                     closeInBrightness(value, level(x, glyph.bottom + 1, true)))) {
                    add(value, 2);
                }
            }
        }
        for (LONG x = glyph.left; x < glyph.right; x += xStep) {
            add(level(x, glyph.top - 2, true), 1);
            add(level(x, glyph.bottom + 1, true), 1);
        }
        return ScoreContrast(histogram, count);
    }

private:
    HDC desktop_{};
    HDC memory_{};
    HBITMAP bitmap_{};
    HGDIOBJ previous_{};
    RECT rect_{};
    int width_{};
    const uint32_t* pixels_{};
};

winrt::hstring ExtractCleanClockText(Controls::TextBlock const& text) {
    std::wstring raw(text.Text());
    if (raw.empty()) {
        for (auto const& inlineObj : text.Inlines()) {
            if (auto run = inlineObj.try_as<Documents::Run>()) {
                raw.append(run.Text());
            }
        }
    }
    if (const auto nul = raw.find(L'\0'); nul != std::wstring::npos) {
        raw.resize(nul);
    }
    while (!raw.empty() && raw.back() <= L' ') {
        raw.pop_back();
    }
    return winrt::hstring(raw);
}

bool MeasureClockCharactersGdi(Controls::TextBlock const& text,
                               std::wstring_view characters,
                               double dipWidth,
                               std::vector<ClockCharacter>& measured) {
    HDC dc = GetDC(nullptr);
    if (!dc) return false;
    const double scale = text.XamlRoot() ? text.XamlRoot().RasterizationScale() : 1.0;
    const int pixelHeight = -std::max(1, static_cast<int>(std::lround(text.FontSize() * scale)));
    const int weight = text.FontWeight().Weight;
    const auto family = text.FontFamily().Source();
    HFONT font = CreateFontW(pixelHeight, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                             family.empty() ? L"Segoe UI" : family.c_str());
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    float previous = 0;
    std::vector<ClockCharacter> result;
    for (size_t index = 0; index < characters.size(); ++index) {
        SIZE sz{};
        if (GetTextExtentPoint32W(dc, characters.data(), static_cast<int>(index + 1), &sz)) {
            const float next = static_cast<float>(sz.cx / scale);
            if (next > previous) {
                result.push_back({previous, next, static_cast<int>(index)});
                previous = next;
            }
        }
    }
    if (oldFont) SelectObject(dc, oldFont);
    if (font) DeleteObject(font);
    ReleaseDC(nullptr, dc);
    if (result.empty()) return false;
    if (dipWidth > 0 && previous > 0) {
        const float factor = static_cast<float>(dipWidth / previous);
        for (auto& item : result) {
            item.left *= factor;
            item.right *= factor;
        }
    }
    measured = std::move(result);
    return true;
}

bool MeasureClockCharacters(Glyph& glyph, Controls::TextBlock const& text) {
    const auto label = ExtractCleanClockText(text);
    const double width = text.ActualWidth();
    const double height = text.ActualHeight();
    if (glyph.clockText == label && glyph.clockWidth == width &&
        glyph.clockHeight == height && !glyph.characters.empty()) {
        return true;
    }
    glyph.clockColors.clear();
    glyph.clockText = label;
    glyph.clockWidth = width;
    glyph.clockHeight = height;
    glyph.characters.clear();
    if (label.empty() || label.size() > 256 || width <= 0 || height <= 0) {
        return false;
    }

    const std::wstring_view characters(label);
    std::vector<ClockCharacter> measured;
    try {
        Controls::TextBlock probe;
        probe.FontFamily(text.FontFamily());
        probe.FontSize(text.FontSize());
        probe.FontWeight(text.FontWeight());
        probe.FontStyle(text.FontStyle());
        probe.FontStretch(text.FontStretch());
        probe.CharacterSpacing(text.CharacterSpacing());
        probe.IsTextScaleFactorEnabled(text.IsTextScaleFactorEnabled());
        const auto language = text.Language();
        if (!language.empty()) probe.Language(language);
        probe.FlowDirection(text.FlowDirection());

        float previous = 0;
        for (size_t index = 0; index < characters.size(); ++index) {
            probe.Text(winrt::hstring(characters.substr(0, index + 1)));
            probe.Measure({4096, 256});
            const float next = probe.DesiredSize().Width;
            if (next > previous) {
                measured.push_back({previous, next, static_cast<int>(index)});
                previous = next;
            }
        }
    } catch (winrt::hresult_error const&) {
        measured.clear();
    }
    if (measured.empty()) {
        MeasureClockCharactersGdi(text, characters, width, measured);
    }
    glyph.characters = std::move(measured);
    return !glyph.characters.empty();
}

ClockColors ClockPaint(Glyph& glyph, DesktopStrip const& strip,
                      LONG clockRight, bool refresh) {
    try {
        auto element = glyph.element.get();
        auto text = element ? element.try_as<Controls::TextBlock>() : nullptr;
        if (!text) {
            return {};
        }
        const bool log = GetTickCount64() >= glyph.nextClockLog;
        if (log) glyph.nextClockLog = GetTickCount64() + 5000;
        if (!MeasureClockCharacters(glyph, text)) {
            if (log) {
                Wh_Log(L"Clock layout unavailable %s len=%u w=%.1f h=%.1f",
                       text.Name().c_str(),
                       static_cast<unsigned>(glyph.clockText.size()),
                       text.ActualWidth(), text.ActualHeight());
            }
            return {};
        }
        if (!refresh && !glyph.clockColors.empty()) return glyph.clockColors;
        ClockColors colors(glyph.clockText.size(), -1);
        bool sampled = false;
        const double scale = text.XamlRoot().RasterizationScale();
        const double right = clockRight - text.Padding().Right * scale;
        const double total = glyph.characters.back().right;
        for (auto& character : glyph.characters) {
            RECT cell = glyph.bounds;
            cell.left = static_cast<LONG>(std::floor(right - (total - character.left) * scale));
            cell.right = static_cast<LONG>(std::ceil(right - (total - character.right) * scale));
            const auto local = strip.ClockCellContrast(cell);
            sampled |= local.valid;
            if (local.valid) {
                const double difference = local.white - local.black;
                if (character.white < 0 || std::abs(difference) >= 0.08) {
                    character.white = difference > 0 ? 1 : 0;
                }
            }
            colors[character.index] = character.white;
        }
        if (sampled) {
            return colors;
        }
    } catch (winrt::hresult_error const&) {
        // XAML can rebuild the clock while its layout is being read.
    }
    return {};
}

std::array<std::vector<Documents::TextRange>, 2> ClockColorRanges(ClockColors const& colors) {
    std::array<std::vector<Documents::TextRange>, 2> ranges;
    for (int index = 0; index < static_cast<int>(colors.size());) {
        const int color = colors[index];
        int end = index + 1;
        while (end < static_cast<int>(colors.size()) && colors[end] == color) {
            ++end;
        }
        if (color >= 0) {
            ranges[color].push_back({index, end - index});
        }
        index = end;
    }
    return ranges;
}

int DesiredTint(Glyph& glyph, Contrast contrast) {
    if (!contrast.valid) {
        if (glyph.ime && glyph.lastImeState != 3) {
            glyph.lastImeState = 3;
            Wh_Log(L"IME result: insufficient background samples");
        }
        glyph.pending = -1;
        glyph.votes = 0;
        return -1;
    }
    const double difference = contrast.white - contrast.black;
    const double advantage = std::abs(difference);
    const int next = (glyph.chosen >= 0 && advantage < 0.04)
                         ? glyph.chosen : (difference > 0 ? 1 : 0);
    if (next != glyph.pending) {
        glyph.pending = next;
        glyph.votes = 0;
    }
    glyph.votes = std::min(glyph.votes + 1, 3);
    const int required = advantage >= 0.24 ? 1 : advantage >= 0.12 ? 2 : 3;
    return glyph.votes >= required ? next : -1;
}

void ReadInputImage(Glyph& glyph, Controls::Image const& image) {
    if (glyph.readback) {
        const auto status = glyph.readback.Status();
        if (status == AsyncStatus::Started) {
            return;
        }
        auto finished = std::exchange(glyph.readback, nullptr);
        if (status != AsyncStatus::Completed) {
            return;
        }
        auto buffer = finished.GetResults();
        const int width = glyph.capture.PixelWidth();
        const int height = glyph.capture.PixelHeight();
        if (!buffer || width < 1 || height < 1 || width > 128 || height > 128 ||
            buffer.Length() < static_cast<uint32_t>(width * height * 4)) {
            return;
        }
        const auto* pixels = buffer.data();
        std::vector<uint8_t> alpha(static_cast<size_t>(width) * height);
        for (size_t i = 0; i < alpha.size(); ++i) {
            alpha[i] = pixels[4 * i + 3];
        }
        const auto [low, high] = std::minmax_element(alpha.begin(), alpha.end());
        if (*high == 0 || *low == 255) {
            if (glyph.lastImeState != 5) {
                glyph.lastImeState = 5;
                Wh_Log(L"IME result: captured icon had no usable transparency");
            }
            return;
        }
        if (!glyph.imageTint || glyph.imageTint.PixelWidth() != width ||
            glyph.imageTint.PixelHeight() != height) {
            glyph.imageTint = Imaging::WriteableBitmap(width, height);
            if (glyph.imageOverlay) {
                glyph.imageOverlay.Source(glyph.imageTint);
            }
            glyph.paintedTint = -1;
        }
        if (glyph.imageAlpha != alpha) {
            glyph.imageAlpha = std::move(alpha);
            glyph.paintedTint = -1;
        }
        return;
    }
    if (glyph.render) {
        const auto status = glyph.render.Status();
        if (status == AsyncStatus::Started) {
            return;
        }
        glyph.render = nullptr;
        if (status == AsyncStatus::Completed) {
            glyph.readback = glyph.capture.GetPixelsAsync();
        }
        return;
    }
    const auto now = GetTickCount64();
    if (now >= glyph.nextImageCapture) {
        glyph.nextImageCapture = now + 750;
        if (!glyph.capture) {
            glyph.capture = Imaging::RenderTargetBitmap();
        }
        glyph.render = glyph.capture.RenderAsync(image);
    }
}

void PaintInputPixels(uint8_t* pixels, std::vector<uint8_t> const& alpha, bool white) {
    for (size_t i = 0; i < alpha.size(); ++i) {
        const uint8_t opacity = alpha[i];
        const uint8_t channel = white ? opacity : 0;
        pixels[4 * i] = channel;
        pixels[4 * i + 1] = channel;
        pixels[4 * i + 2] = channel;
        pixels[4 * i + 3] = opacity;
    }
}

void TintInputModeImage(Glyph& glyph, Controls::Image const& image, Contrast contrast) {
    const auto source = image.Source();
    if (source != glyph.imageSource) {
        Restore(glyph);
        glyph.chosen = -1;
        glyph.imageSource = source;
    }
    if (!source) {
        return;
    }
    const int next = DesiredTint(glyph, contrast);
    if (next >= 0) {
        glyph.chosen = next;
    }
    ReadInputImage(glyph, image);
    if (glyph.chosen < 0 || glyph.imageAlpha.empty() ||
        glyph.paintedTint == glyph.chosen) {
        return;
    }
    auto parent = image.Parent().try_as<Controls::Grid>();
    if (!parent) {
        return;
    }
    auto buffer = glyph.imageTint.PixelBuffer();
    PaintInputPixels(buffer.data(), glyph.imageAlpha, glyph.chosen != 0);
    glyph.imageTint.Invalidate();
    if (!glyph.imageOverlay) {
        uint32_t index = 0;
        auto siblings = parent.Children();
        if (!siblings.IndexOf(image, index)) {
            return;
        }
        Controls::Image overlay;
        overlay.Name(L"AdaptiveTrayImeTint");
        overlay.IsHitTestVisible(false);
        overlay.Stretch(Media::Stretch::Fill);
        overlay.HorizontalAlignment(image.HorizontalAlignment());
        overlay.VerticalAlignment(image.VerticalAlignment());
        overlay.Margin(image.Margin());
        overlay.Width(image.ActualWidth());
        overlay.Height(image.ActualHeight());
        overlay.Source(glyph.imageTint);
        Controls::Grid::SetColumn(overlay, Controls::Grid::GetColumn(image));
        Controls::Grid::SetRow(overlay, Controls::Grid::GetRow(image));
        siblings.InsertAt(index + 1, overlay);
        glyph.imageOverlay = overlay;
    } else {
        glyph.imageOverlay.Width(image.ActualWidth());
        glyph.imageOverlay.Height(image.ActualHeight());
    }
    glyph.paintedTint = glyph.chosen;
    if (glyph.lastImeState != glyph.chosen) {
        glyph.lastImeState = glyph.chosen;
        Wh_Log(L"IME result: tinted captured icon %s",
               glyph.chosen ? L"white" : L"black");
    }
}

void ApplySample(Glyph& glyph, Contrast contrast, ClockColors const& clockPaint) {
    auto element = glyph.element.get();
    if (!element) {
        return;
    }
    if (!glyph.property) {
        if (auto image = element.try_as<Controls::Image>()) {
            try {
                TintInputModeImage(glyph, image, contrast);
            } catch (winrt::hresult_error const& error) {
                if (glyph.lastImeState != 4) {
                    glyph.lastImeState = 4;
                    Wh_Log(L"IME result: image composition failed 0x%08X",
                           static_cast<unsigned>(error.code().value));
                }
            }
        }
        return;
    }
    if (glyph.clock && !clockPaint.empty()) {
        const auto text = element.as<Controls::TextBlock>();
        const auto highlighters = text.TextHighlighters();
        if (clockPaint == glyph.clockColors) {
            uint32_t index = 0;
            bool attached = true;
            for (auto const& item : glyph.clockHighlights)
                if (item && !highlighters.IndexOf(item, index)) attached = false;
            if (attached) return;
        }
        ClearClockHighlights(glyph, element);
        const auto ranges = ClockColorRanges(clockPaint);
        for (int color = 0; color < 2; ++color) {
            if (ranges[color].empty()) continue;
            Documents::TextHighlighter item;
            item.Background(Media::SolidColorBrush(Colors::Transparent()));
            item.Foreground(Media::SolidColorBrush(color ? Colors::White() : Colors::Black()));
            for (auto const& range : ranges[color]) item.Ranges().Append(range);
            highlighters.Append(item);
            glyph.clockHighlights[color] = item;
        }
        glyph.clockColors = clockPaint;
        return;
    }
    const auto local = element.ReadLocalValue(glyph.property);
    if (!glyph.applied || local != glyph.applied) {
        glyph.applied = nullptr;
        const auto brush = element.GetValue(glyph.property)
                               .try_as<Media::SolidColorBrush>();
        if (!Monochrome(brush) ||
            (local != DependencyProperty::UnsetValue() && !local.try_as<Media::Brush>())) {
            if (glyph.ime && glyph.lastImeState != 2) {
                glyph.lastImeState = 2;
                const auto color = brush ? brush.Color() : Color{};
                Wh_Log(L"IME result: native brush skipped solid=%d argb=%02X%02X%02X%02X local=%s",
                       static_cast<int>(!!brush), static_cast<unsigned>(color.A),
                       static_cast<unsigned>(color.R), static_cast<unsigned>(color.G),
                       static_cast<unsigned>(color.B),
                       local ? winrt::get_class_name(local).c_str() : L"(none)");
            }
            glyph.chosen = -1;
            glyph.pending = -1;
            glyph.votes = 0;
            return;
        }
        glyph.original = local;
    }
    ClearClockHighlights(glyph, element);
    const int next = DesiredTint(glyph, contrast);
    if (next < 0 || (glyph.applied && next == glyph.chosen)) {
        return;
    }
    glyph.chosen = next;
    glyph.applied = Media::SolidColorBrush(next ? Colors::White() : Colors::Black());
    element.SetValue(glyph.property, glyph.applied);
    if (glyph.ime && glyph.lastImeState != next) {
        glyph.lastImeState = next;
        Wh_Log(L"IME result: set %s, black=%.2f white=%.2f, rect=%ld,%ld,%ld,%ld",
               next ? L"white" : L"black", contrast.black, contrast.white,
               glyph.bounds.left, glyph.bounds.top, glyph.bounds.right, glyph.bounds.bottom);
    }
}

bool IsFullscreenOn(HMONITOR monitor, RECT display) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || MonitorFromWindow(foreground, MONITOR_DEFAULTTONULL) != monitor) {
        return false;
    }
    wchar_t name[32]{};
    GetClassNameW(foreground, name, ARRAYSIZE(name));
    if (!wcscmp(name, L"Progman") || !wcscmp(name, L"WorkerW") ||
        !wcscmp(name, L"Shell_TrayWnd") || !wcscmp(name, L"Shell_SecondaryTrayWnd")) {
        return false;
    }
    RECT bounds{};
    if (!GetWindowRect(foreground, &bounds)) {
        return false;
    }
    const auto style = GetWindowLongPtrW(foreground, GWL_STYLE);
    return (style & WS_CAPTION) != WS_CAPTION &&
           bounds.left <= display.left + 2 && bounds.top <= display.top + 2 &&
           bounds.right >= display.right - 2 && bounds.bottom >= display.bottom - 2;
}

void DiscoverButtons(Tray& tray, DependencyObject const& node, int depth = 0) {
    if (!node || depth > 40) return;
    const auto type = winrt::get_class_name(node);
    const bool isTaskList = (type == L"Taskbar.TaskListButton");
    if (isTaskList || type == L"Taskbar.ExperienceToggleButton" ||
        type == L"Taskbar.SearchBoxButton" || type == L"Taskbar.AugmentedEntryPointButton") {
        auto element = node.try_as<FrameworkElement>();
        if (!element) return;
        for (auto& button : tray.buttons) {
            if (button.element.get() == element) {
                button.seen = true;
                return;
            }
        }
        auto local = element.ReadLocalValue(FrameworkElement::RequestedThemeProperty());
        if (local != DependencyProperty::UnsetValue() && !local.try_as<winrt::Windows::Foundation::IPropertyValue>()) return;
        ThemeButton button;
        button.element = winrt::make_weak(element);
        button.original = local;
        button.seen = true;
        button.isTaskList = isTaskList;
        if (isTaskList) {
            RemoveAllDualOverlays(element);
        }
        tray.buttons.push_back(std::move(button));
        return;
    }
    if (type == L"SystemTray.SystemTrayFrame") return;
    const int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) DiscoverButtons(tray, Media::VisualTreeHelper::GetChild(node, i), depth + 1);
}

void ThemeButtons(Tray& tray, DesktopStrip const& strip, bool refresh) {
    for (auto& button : tray.buttons) {
        auto element = button.element.get();
        if (!element || IsRectEmpty(&button.bounds)) continue;
        // Sample the top margin above the artwork and the running indicator.
        RECT margin = button.bounds;
        margin.bottom = margin.top + std::max(1L, (margin.bottom - margin.top) / 10);
        if (refresh) button.cachedMarginContrast = strip.ClockCellContrast(margin);
        const auto contrast = button.cachedMarginContrast;
        if (!contrast.valid || std::abs(contrast.black - contrast.white) < 0.3) continue;
        const bool darkBg = contrast.white > contrast.black;
        const auto theme = darkBg ? ElementTheme::Dark : ElementTheme::Light;
        if (element.RequestedTheme() != theme) {
            element.RequestedTheme(theme);
            button.last = theme;
            if (button.isTaskList) SyncButtonIcon(button, element);
        }
        if (button.isTaskList) {
            ApplyButtonIcon(button, darkBg);
        }
    }
}

bool UpdateTray(Tray& tray) {
    const HMONITOR monitor = MonitorFromWindow(tray.window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO screen{sizeof(screen)};
    RECT taskbar{}, visible{};
    if (!IsWindowVisible(tray.window) || !GetWindowRect(tray.window, &taskbar) ||
        !GetMonitorInfoW(monitor, &screen) ||
        !IntersectRect(&visible, &taskbar, &screen.rcMonitor)) {
        return false;
    }
    const LONG width = taskbar.right - taskbar.left;
    const LONG height = taskbar.bottom - taskbar.top;
    const bool mostlyVisible = width >= height
        ? (visible.bottom - visible.top) * 4 >= height * 3
        : (visible.right - visible.left) * 4 >= width * 3;
    if (!mostlyVisible || IsFullscreenOn(monitor, screen.rcMonitor)) {
        return false;
    }
    auto root = tray.root.get();
    if (!root || !root.XamlRoot()) {
        auto xaml = ReadRoot(tray.window);
        root = xaml ? FindTray(xaml.Content().try_as<DependencyObject>()) : nullptr;
        if (!root) {
            return false;
        }
        tray.root = winrt::make_weak(root);
        tray.nextDiscovery = 0;
    }
    const auto now = GetTickCount64();
    bool layoutChanged = now >= tray.nextDiscovery;
    if (layoutChanged) {
        for (auto& glyph : tray.glyphs) {
            glyph.present = false;
        }
        Collect(tray, root);
        for (auto& button : tray.buttons) button.seen = false;
        DiscoverButtons(tray, root.XamlRoot().Content());
        std::erase_if(tray.buttons, [](ThemeButton& b) {
            if (!b.seen) RestoreButton(b);
            return !b.seen;
        });
        for (auto& glyph : tray.glyphs) {
            if (!glyph.present) {
                Restore(glyph);
            }
        }
        std::erase_if(tray.glyphs, [](Glyph const& glyph) { return !glyph.present; });
        tray.nextDiscovery = now + 500;
        if (tray.lastCount != tray.glyphs.size()) {
            tray.lastCount = tray.glyphs.size();
            Wh_Log(L"Taskbar %p: %u foreground candidates", tray.window,
                   static_cast<unsigned>(tray.lastCount));
        }
    }
    RECT region{};
    for (auto& button : tray.buttons) {
        auto element = button.element.get();
        if (element && button.isTaskList) SyncButtonIcon(button, element);
        const RECT oldBounds = button.bounds;
        button.bounds = {};
        if (element && element.Visibility() == Visibility::Visible &&
            ScreenBounds(element, tray.window, button.bounds))
            UnionRect(&region, &region, &button.bounds);
        layoutChanged |= !EqualRect(&oldBounds, &button.bounds);
    }
    LONG clockRight = visible.left;
    for (auto& glyph : tray.glyphs) {
        auto element = glyph.element.get();
        const RECT oldBounds = glyph.bounds;
        const bool wasPresent = glyph.present;
        glyph.present = element && element.Visibility() == Visibility::Visible &&
                        element.Opacity() > 0 &&
                        ScreenBounds(element, tray.window, glyph.bounds);
        layoutChanged |= wasPresent != glyph.present || !EqualRect(&oldBounds, &glyph.bounds);
        if (glyph.present) {
            if (glyph.clock) {
                clockRight = std::max(clockRight, glyph.bounds.right);
            }
            UnionRect(&region, &region, &glyph.bounds);
        }
    }
    if (IsRectEmpty(&region)) {
        return false;
    }
    if (clockRight > visible.left) {
        region.left = std::min(region.left, clockRight - 512);
    }
    InflateRect(&region, 4, 4);
    if (!IntersectRect(&region, &region, &visible)) {
        return false;
    }
    DesktopStrip strip;
    if (strip.Capture(region)) {
        const bool refresh = !strip.MatchesPreviousFrame(tray) || layoutChanged;
        if (refresh) tray.previousRegion = {};  // Retry if a XAML operation below throws.
        ThemeButtons(tray, strip, refresh);
        for (auto& glyph : tray.glyphs) {
            if (glyph.present) {
                if (refresh) glyph.cachedContrast = strip.BackgroundContrast(glyph.bounds, tray.glyphs);
                const auto clockPaint = glyph.clock
                    ? ClockPaint(glyph, strip, clockRight, refresh) : ClockColors{};
                ApplySample(glyph, glyph.cachedContrast, clockPaint);
            }
        }
        tray.previousRegion = region;
    }
    return true;
}

bool Tick() {
    try {
        QUERY_USER_NOTIFICATION_STATE notificationState{};
        if (SUCCEEDED(SHQueryUserNotificationState(&notificationState)) &&
            notificationState == QUNS_RUNNING_D3D_FULL_SCREEN) {
            return false;
        }
        auto& trays = Trays();
        std::erase_if(trays, [](Tray& tray) {
            if (IsWindow(tray.window)) {
                return false;
            }
            RestoreTray(tray);
            return true;
        });
        EnumThreadWindows(GetCurrentThreadId(), [](HWND window, LPARAM) -> BOOL {
            wchar_t cls[40]{};
            if (!GetClassNameW(window, cls, ARRAYSIZE(cls)) ||
                (wcscmp(cls, L"Shell_TrayWnd") &&
                 wcscmp(cls, L"Shell_SecondaryTrayWnd"))) {
                return TRUE;
            }
            auto& current = Trays();
            if (std::none_of(current.begin(), current.end(),
                    [window](Tray const& tray) { return tray.window == window; })) {
                try {
                    current.push_back(Tray{.window = window});
                } catch (...) {
                    return FALSE;
                }
            }
            return TRUE;
        }, 0);
        bool active = false;
        for (auto& tray : trays) {
            active = UpdateTray(tray) || active;
        }
        return active;
    } catch (...) {
        // Tray elements may be rebuilt while being inspected. Retry next tick.
        return false;
    }
}

void RestoreAll() {
    for (auto& tray : Trays()) {
        try {
            RestoreTray(tray);
        } catch (...) {
        }
    }
    std::vector<Tray>().swap(Trays());
}

LRESULT CALLBACK SamplerProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == kSampleTimer) {
        if (!g_unloading.load()) {
            const UINT active = g_activePollMs.load();
            const UINT interval = Tick() ? active : std::max(active, kIdlePollMs);
            if (interval != g_interval && SetTimer(window, kSampleTimer, interval, nullptr)) {
                g_interval = interval;
            }
        }
        return 0;
    }
    if (msg == WM_DESTROY) {
        KillTimer(window, kSampleTimer);
        RestoreAll();
        g_samplerWindow = nullptr;
    }
    return DefWindowProcW(window, msg, wParam, lParam);
}

void StartOnTaskbarThread() {
    std::lock_guard lock(g_samplerLifecycle);
    if (g_samplerWindow.load() || g_unloading.load()) {
        return;
    }
    HWND sampler = CreateWindowExW(0, kSamplerClass, L"", 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, g_module, nullptr);
    const UINT interval = g_activePollMs.load();
    if (!sampler || !SetTimer(sampler, kSampleTimer, interval, nullptr)) {
        if (sampler) {
            DestroyWindow(sampler);
        }
        Wh_Log(L"Could not start the taskbar timer.");
        return;
    }
    g_samplerWindow = sampler;
    g_interval = interval;
}

void StartForWindow(HWND window) {
    DWORD pid = 0;
    const DWORD tid = GetWindowThreadProcessId(window, &pid);
    if (!tid || pid != GetCurrentProcessId()) {
        return;
    }
    if (tid == GetCurrentThreadId()) {
        StartOnTaskbarThread();
        return;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto& msg = *reinterpret_cast<CWPSTRUCT*>(lParam);
                if (msg.message == g_controlMessage) {
                    StartOnTaskbarThread();
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }, nullptr, tid);
    if (hook) {
        SendMessageW(window, g_controlMessage, 0, 0);
        UnhookWindowsHookEx(hook);
    }
}

void WINAPI StartHook(void* object) {
    g_originalStart(object);
    StartOnTaskbarThread();
}

}  // namespace

void Wh_ModSettingsChanged() {
    g_activePollMs = static_cast<UINT>(std::clamp(Wh_GetIntSetting(L"samplingInterval"), 40, 2000));
}

BOOL Wh_ModInit() {
    Wh_ModSettingsChanged();
    Wh_Log(L"Adaptive tray contrast v" WH_MOD_VERSION L" loaded");
    g_controlMessage = RegisterWindowMessageW(L"Windhawk.AdaptiveTrayContrast.Control");
    auto module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module || !g_controlMessage ||
        !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            kSamplerClass, &g_module)) {
        return FALSE;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_primarySite},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &g_secondarySite, nullptr, true},
        {{L"public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const "},
         &g_primaryHost},
        {{L"public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const "},
         &g_secondaryHost, nullptr, true},
        {{L"public: int __cdecl TaskbarHost::FrameHeight(void)const "}, &g_frameHeight},
        {{L"public: void __cdecl std::_Ref_count_base::_Decref(void)"}, &g_releaseHost},
        {{L"public: virtual void __cdecl TrayUI::StartTaskbar(void)"},
         &g_originalStart, StartHook, true},
    };
    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Could not resolve the Windows 11 XAML taskbar symbols.");
        return FALSE;
    }
    WNDCLASSW sampler{};
    sampler.lpfnWndProc = SamplerProc;
    sampler.hInstance = g_module;
    sampler.lpszClassName = kSamplerClass;
    if (!RegisterClassW(&sampler)) {
        Wh_Log(L"Could not register the taskbar sampler window.");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    StartForWindow(FindWindowW(L"Shell_TrayWnd", nullptr));
}

void Wh_ModBeforeUninit() {
    HWND sampler;
    {
        std::lock_guard lock(g_samplerLifecycle);
        g_unloading = true;
        sampler = g_samplerWindow.load();
    }
    // Close on the owner thread without holding the startup lock.
    if (sampler) {
        SendMessageW(sampler, WM_CLOSE, 0, 0);
    }
    UnregisterClassW(kSamplerClass, g_module);
}
