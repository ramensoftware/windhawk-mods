// ==WindhawkMod==
// @id              lock-screen-splashes
// @name            Lock Screen Splashes
// @name:zh-CN      锁屏闪烁标语
// @description     Minecraft-style animated splash texts below the lock screen date.
// @description:zh-CN 在锁屏日期下方显示 Minecraft 风格的动态标语。
// @version         0.4.6
// @author          4tubborn
// @github          https://github.com/4tubborn
// @license         GPL-3.0-or-later
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Splashes

Adds Minecraft-style animated splash texts to the Windows lock screen.

## Features

- Randomized splash texts
- Custom splash text list
- External UTF-8 splash text file
- Custom fonts and fallback fonts
- Customizable text color, shadow, rotation, and position
- Configurable pulse animation
- Automatic splash refresh when locking the screen

The mod preserves the original Windows clock and date.

### External splash file

Since LockApp.exe runs in an AppContainer, it may not
have permission to read arbitrary files.

Ensure that the splash file is accessible to the lock
screen process. Otherwise, the mod will fall back to
the configured splash text list.

The file is checked on startup, when settings are saved, and each time the
lock screen becomes visible. Unchanged files reuse cached splash texts.

## Compatibility

Tested on Windows 11 25H2 (x64).

After installing new fonts, you may need to sign out of Windows
and sign back in for them to become available on the lock screen.

## Credits

Developed by 4tubborn with assistance from ChatGPT.

The implementation is based in part on the following mods:

- [Lock Screen Date and Time Format](https://windhawk.net/mods/lockscreen-datetime-format) by grn15RenS
- [Lock Screen Clock Styler](https://windhawk.net/mods/lock-screen-clock-styler) by ali0zdas

Licensed under GPL-3.0-or-later.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fontFamily: Segoe UI
  $name: Font family
- fallbackFontFamily: ""
  $name: Fallback font families
  $description: "Optional comma-separated list, e.g. Microsoft YaHei UI, Segoe UI Emoji."
- fontSize: 22
  $name: Font size
- adaptiveScale: true
  $name: Adaptive text scaling
  $description: "Adjusts the splash size based on its text width, like Minecraft."
- shadowOffsetMultiplier: 100
  $name: Shadow offset multiplier (%)
  $description: "100 = default offset, 50 = half offset, 0 = no offset."
- color: "#FFFF00"
  $name: "Color (#RRGGBB or #AARRGGBB)"
- rotation: -20
  $name: Rotation (degrees)
- animation: true
  $name: Enable pulse animation
- minimumScale: 100
  $name: Minimum scale (%)
- maximumScale: 106
  $name: Maximum scale (%)
- halfCycleMs: 250
  $name: Pulse half-cycle (milliseconds)
- offsetX: 100
  $name: Horizontal offset
- offsetY: -40
  $name: Vertical offset
- splashFile: ""
  $name: Optional splashes.txt path (UTF-8)
  $description: "Use one splash per line. A readable, nonempty file replaces the list below. The file is checked each time the lock screen appears."
- splashTexts:
  - "Windows!"
  - "Windhawk!"
  - "Works on my machine"
  - "Also try Linux!"
  - "Also try Explorer Patcher!"
  - "Also try MS-DOS!"
  - "Also try OS/2!"
  - "Now with more WebView2"
  - "Java 5 Compatible"
  - "AI-generated"
  - "AI slop"
  - "Made of 32-bit components"
  - "It is now unsafe to turn off your computer."
  - "Make Windows Great Again!"
  - "Removed Herobrine"
  - "Also try Windows 7!"
  - "You swan, he frog!"
  - "Surf the Internet"
  - "No more UWP"
  - "You are undefined!"
  - "Life is a memory leak"
  - "Hurry up!"
  - "Write once, crash everywhere"
  - "Legacy Innovations!"
  - "Innovating since 1995!"
  - "No Copilot required"
  - "Native code"
  - "Innovation through archaeology"
  - "Task failed successfully."
  - "Thinking"
  - "Searching"
  - "Right-handed"
  - "Left-handed"
  - "Status: Won't Fix"
  - "Breaking changes"
  - "By design"
  - "Not a bug, a feature"
  - "Coming s∞n"
  - "Deprecated but supported"
  - "Nothing is more permanent than a temporary fix"
  - "Please wait..."
  - "Failed to fail."
  - "Backward compatible with the future"
  - "Now with more Win32"
  - "Now with rounded corners"
  - "Reinventing the Control Panel"
  - "Powered by technical debt"
  - "Runtime Error :("
  - "Compile Error :("
  - "Build successful :)"
  - "42 Warnings. Push anyway?"
  - "TODO: Remove this"
  - "TODO: Rewrite this"
  - "Intentionally left unimplemented"
  - "It should be removed"
  - "It should not be removed"
  - "Now with brackets"
  - "Reset to default"
  - "Indecisiveness"
  - "Decidophobia"
  - "Choice paralysis"
  - "Splash!"
  - "Written in C++"
  - "Worse than Windows 7"
  - "Only for Games"
  - "Don't forget to download .NET"
  - "Windows, not widows"
  - "NT 6.5!"
  - "Truly Indie!"
  - "chest_locked_aprilfools_super_old_legacy_we_should_not_even_have_this"
  - "Unlock AppContainer!"
  - "We need locale!"
  - "Think different."
  - "Get a Mac!"
  - "I'm a PC."
  - "Windows One Core!"
  - "Portable executable!"
  - "This program cannot be run in DOS mode."
  - "MZ!"
  - "Contact your system administrator."
  - "0x8000FFFF"
  - "New Modern UI (New)"
  - "New Modern UI (Classic)"
  - "Modern since 2012!"
  - "One more time!"
  - "Never gonna give you up"
  - "0% complete"
  - "Sit back and relax"
  - "Hi."
  - "Don't turn off your PC."
  - "You're almost there."
  $name: Splash texts
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <string>
#include <vector>
#include <limits>
#include <optional>

using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;

struct Settings {
    std::vector<std::wstring> texts;
    std::wstring font;
    Windows::UI::Color color{255, 255, 255, 0};
    double size, rotation, low, high, x, y, shadowOffsetMultiplier;
    int halfCycle;
    bool animate, adaptiveScale;

    bool operator==(Settings const&) const = default;
};
static Settings g_settings;
static std::mutex g_mutex;
static std::wstring g_filePath;
static std::vector<std::wstring> g_fileTexts;
static uint64_t g_filePathRevision;
static bool g_dirty = true;
static HANDLE g_stop, g_reloadFile, g_worker;
static std::atomic<HWND> g_dispatchWindow{nullptr};
static UINT g_message;
static std::atomic<bool> g_observing{false};
static std::atomic<bool> g_stopping{false};
// All XAML state is accessed and released on the CoreWindow thread.
[[clang::no_destroy]] static CoreWindow g_coreWindow{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_attachTimer{nullptr};
static event_token g_visibilityToken, g_attachTickToken;
static int g_attachAttempts;
[[clang::no_destroy]] static StackPanel g_panel{nullptr};
[[clang::no_destroy]] static Grid g_splash{nullptr};
[[clang::no_destroy]] static TextBlock g_text{nullptr};
[[clang::no_destroy]] static TextBlock g_shadow{nullptr};
[[clang::no_destroy]] static Storyboard g_story{nullptr};
static constexpr wchar_t kSplashName[] = L"WindhawkLockScreenSplash";
static bool g_visible = false, g_refresh = true;
static std::wstring g_previous;

static std::wstring StringSetting(PCWSTR key) {
    auto value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    if (value) Wh_FreeStringSetting(value);
    return result;
}

static void AddLine(std::vector<std::wstring>& lines, std::wstring line) {
    auto first = line.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return;
    auto last = line.find_last_not_of(L" \t\r\n");
    lines.push_back(line.substr(first, (std::min)(last - first + 1, size_t(512))));
}

struct FileStamp {
    FILE_ID_INFO id{};
    LONGLONG size;
    LONGLONG written;
    LONGLONG changed;

    bool operator==(FileStamp const& other) const {
        return id.VolumeSerialNumber == other.id.VolumeSerialNumber &&
            std::equal(std::begin(id.FileId.Identifier), std::end(id.FileId.Identifier),
                       std::begin(other.id.FileId.Identifier)) &&
            size == other.size && written == other.written && changed == other.changed;
    }
};

struct FileCache {
    uint64_t pathRevision = 0;
    std::optional<FileStamp> stamp;
};

static std::vector<std::wstring> ParseFileTexts(std::string const& bytes) {
    std::vector<std::wstring> lines;
    int length = static_cast<int>(bytes.size());
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), length, nullptr, 0);
    if (!count) { Wh_Log(L"Splash file is not valid UTF-8"); return lines; }
    std::wstring text(count, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), length, text.data(), count);
    size_t start = text[0] == 0xFEFF ? 1 : 0;
    while (start < text.size() && lines.size() < 4096) {
        auto end = text.find(L'\n', start);
        AddLine(lines, text.substr(start, end == std::wstring::npos ? end : end - start));
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    if (lines.empty()) Wh_Log(L"Splash file has no nonblank lines; using the settings list");
    else Wh_Log(L"Loaded %u splash texts from file", static_cast<unsigned>(lines.size()));
    return lines;
}

// No value means that the caller can keep its current parsed text list.
static std::optional<std::vector<std::wstring>> ReadFileTexts(
    std::wstring const& path, FileCache& cache) {
    auto failed = [&cache] {
        cache.stamp.reset();
        return std::vector<std::wstring>{};
    };
    if (path.empty()) return failed();
    HANDLE rawFile = CreateFileW(path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (rawFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        Wh_Log(L"Splash file open failed: %u; using the settings list", error);
        return failed();
    }
    winrt::handle file{rawFile};
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 || size.QuadPart > 1024 * 1024) {
        Wh_Log(L"Splash file empty, too large, or unreadable");
        return failed();
    }

    FILE_BASIC_INFO basic{};
    FileStamp current{};
    std::optional<FileStamp> stamp;
    if (GetFileInformationByHandleEx(file.get(), FileBasicInfo, &basic, sizeof(basic)) &&
        GetFileInformationByHandleEx(file.get(), FileIdInfo, &current.id, sizeof(current.id))) {
        current.size = size.QuadPart;
        current.written = basic.LastWriteTime.QuadPart;
        current.changed = basic.ChangeTime.QuadPart;
        stamp = current;
        if (cache.stamp == stamp) return std::nullopt;
    }

    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD read = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()), &read, nullptr) ||
        read != bytes.size()) {
        Wh_Log(L"Splash file read incomplete; using the settings list");
        return failed();
    }

    auto texts = ParseFileTexts(bytes);
    cache.stamp = stamp;
    return texts;
}

static void LoadSettings() {
    // Serialize reads so a delayed settings callback cannot publish older values.
    std::lock_guard lock(g_mutex);
    Settings s{};
    for (int i = 0; i < 4096; ++i) {
        auto key = L"splashTexts[" + std::to_wstring(i) + L"]";
        auto value = StringSetting(key.c_str());
        if (value.empty()) break;
        AddLine(s.texts, value);
    }
    auto file = StringSetting(L"splashFile");
    if (s.texts.empty()) s.texts.push_back(L"Removed Herobrine");
    s.font = StringSetting(L"fontFamily");
    if (s.font.empty()) s.font = L"Segoe UI";
    auto fallback = StringSetting(L"fallbackFontFamily");
    if (fallback.find_first_not_of(L" \t\r\n") != std::wstring::npos) {
        s.font += L", ";
        s.font += fallback;
    }
    auto color = StringSetting(L"color");
    if ((color.size() == 7 || color.size() == 9) && color[0] == L'#' &&
        color.find_first_not_of(L"0123456789abcdefABCDEF", 1) == std::wstring::npos) {
        auto v = std::stoul(color.substr(1), nullptr, 16);
        s.color = {static_cast<BYTE>(color.size() == 9 ? v >> 24 : 255),
            static_cast<BYTE>(v >> 16), static_cast<BYTE>(v >> 8), static_cast<BYTE>(v)};
    } else Wh_Log(L"Invalid splash color; using yellow");
    s.size = std::clamp(Wh_GetIntSetting(L"fontSize"), 8, 200);
    s.adaptiveScale = Wh_GetIntSetting(L"adaptiveScale") != 0;
    s.shadowOffsetMultiplier = std::clamp(Wh_GetIntSetting(L"shadowOffsetMultiplier"), 0, 400) / 100.0;
    s.rotation = std::clamp(Wh_GetIntSetting(L"rotation"), -180, 180);
    s.low = std::clamp(Wh_GetIntSetting(L"minimumScale"), 10, 300) / 100.0;
    s.high = std::clamp(Wh_GetIntSetting(L"maximumScale"), 10, 300) / 100.0;
    if (s.low > s.high) std::swap(s.low, s.high);
    s.halfCycle = std::clamp(Wh_GetIntSetting(L"halfCycleMs"), 100, 10000);
    s.x = std::clamp(Wh_GetIntSetting(L"offsetX"), -2000, 2000);
    s.y = std::clamp(Wh_GetIntSetting(L"offsetY"), -2000, 2000);
    s.animate = Wh_GetIntSetting(L"animation") != 0;
    if (s != g_settings) {
        g_settings = std::move(s);
        g_dirty = true;
    }
    if (file != g_filePath) {
        g_filePath = std::move(file);
        g_fileTexts.clear();
        ++g_filePathRevision;
        g_dirty = true;
    }
}

static bool ReloadFileTexts(FileCache& cache) {
    std::wstring path;
    uint64_t revision;
    {
        std::lock_guard lock(g_mutex);
        path = g_filePath;
        revision = g_filePathRevision;
    }

    if (cache.pathRevision != revision) {
        cache = {};
        cache.pathRevision = revision;
    }
    auto texts = ReadFileTexts(path, cache);
    std::lock_guard lock(g_mutex);
    if (revision != g_filePathRevision) return false;
    if (texts && *texts != g_fileTexts) {
        g_fileTexts = std::move(*texts);
        g_dirty = true;
    }
    return true;
}

static StackPanel FindPanel(DependencyObject const& node, int depth = 0) {
    if (!node || depth > 128) return nullptr;
    if (auto panel = node.try_as<StackPanel>(); panel && panel.Name() == L"TimeAndDatePanel") return panel;
    int count = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto found = FindPanel(VisualTreeHelper::GetChild(node, i), depth + 1);
        if (found) return found;
    }
    return nullptr;
}

static void Detach() {
    if (g_splash) {
        // Hide first so it disappears even if the parent rejects removal.
        g_splash.Visibility(Visibility::Collapsed);
        try {
            auto parent = VisualTreeHelper::GetParent(g_splash);
            if (auto panel = parent.try_as<Panel>()) {
                auto children = panel.Children();
                uint32_t index;
                if (children.IndexOf(g_splash, index)) children.RemoveAt(index);
            }
        } catch (hresult_error const& e) {
            Wh_Log(L"Removing hidden splash failed: %s", e.message().c_str());
        }
    }
    // An animation failure must not prevent removing the visual.
    if (g_story) {
        try { g_story.Stop(); }
        catch (hresult_error const& e) { Wh_Log(L"Stopping splash animation failed: %s", e.message().c_str()); }
        g_story = nullptr;
    }
    g_shadow = nullptr;
    g_text = nullptr;
    g_splash = nullptr;
    g_panel = nullptr;
}

static void Attach(StackPanel const& panel) {
    auto children = panel.Children();
    // Older versions may have left a TextBlock or Grid in the tree.
    for (uint32_t i = children.Size(); i > 0; --i) {
        auto oldSplash = children.GetAt(i - 1).try_as<FrameworkElement>();
        if (oldSplash && oldSplash.Name() == kSplashName) children.RemoveAt(i - 1);
    }

    Grid splash;
    splash.Name(kSplashName);
    splash.IsHitTestVisible(false);
    splash.HorizontalAlignment(HorizontalAlignment::Center);
    splash.Margin({12, 20, 12, 20});
    splash.RenderTransformOrigin({0.5f, 0.5f});
    TextBlock shadow;
    TextBlock text;
    shadow.TextAlignment(TextAlignment::Center);
    text.TextAlignment(TextAlignment::Center);

    splash.Children().Append(shadow);
    splash.Children().Append(text);
    children.Append(splash);
    g_panel = panel;
    g_splash = splash;
    g_shadow = shadow;
    g_text = text;
    Wh_Log(L"Splash attached to TimeAndDatePanel");
}

static void Apply(Settings const& s) {
    if (g_story) { g_story.Stop(); g_story = nullptr; }

    FontFamily font(s.font);
    g_text.FontFamily(font);
    g_shadow.FontFamily(font);
    g_text.FontSize(s.size);
    g_shadow.FontSize(s.size);

    double scale = 1.0;

    if (s.adaptiveScale) {
        g_text.Measure({
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()
        });

        double width = g_text.DesiredSize().Width;
        scale = 170.0 / (width * 8.0 / s.size + 32.0);
    }

    TranslateTransform shadowOffset;
    double offset = s.size / 8.0 * s.shadowOffsetMultiplier;
    shadowOffset.X(offset);
    shadowOffset.Y(offset);
    g_shadow.RenderTransform(shadowOffset);

    g_text.Foreground(SolidColorBrush(s.color));
    g_shadow.Foreground(SolidColorBrush(Windows::UI::Color{
        s.color.A,
        static_cast<uint8_t>(s.color.R / 4),
        static_cast<uint8_t>(s.color.G / 4),
        static_cast<uint8_t>(s.color.B / 4)
    }));
    CompositeTransform transform;
    transform.Rotation(s.rotation);
    transform.TranslateX(s.x);
    transform.TranslateY(s.y);
    transform.ScaleX((s.animate ? s.low : 1.0) * scale);
    transform.ScaleY((s.animate ? s.low : 1.0) * scale);
    g_splash.RenderTransform(transform);
    if (!s.animate || s.low == s.high) return;
    g_story = Storyboard();
    for (auto property : {L"ScaleX", L"ScaleY"}) {
        DoubleAnimation animation;
        animation.From(s.low * scale);
        animation.To(s.high * scale);
        animation.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(s.halfCycle)));
        animation.AutoReverse(true);
        animation.RepeatBehavior(RepeatBehaviorHelper::Forever());

        SineEase easing;
        easing.EasingMode(EasingMode::EaseIn);
        animation.EasingFunction(easing);

        Storyboard::SetTarget(animation, transform);
        Storyboard::SetTargetProperty(animation, property);
        g_story.Children().Append(animation);
    }
    g_story.Begin();
}

static bool Update() {
    auto window = Window::Current();
    if (!window) return false;

    auto panel = FindPanel(window.Content());
    bool attached = panel && panel != g_panel;

    if (panel != g_panel) {
        Detach();
        if (panel) Attach(panel);
    }

    if (!g_text) return false;

    bool refreshed = g_refresh;
    bool dirty;
    Settings s;
    {
        std::lock_guard lock(g_mutex);
        dirty = g_dirty;
        if (!dirty && !attached && !refreshed) return true;
        s = g_settings;
        if (!g_fileTexts.empty()) s.texts = g_fileTexts;
        g_dirty = false;
    }

    if (attached || dirty || refreshed) {
        static std::mt19937 random(GetTickCount());
        std::vector<size_t> candidates;

        for (size_t i = 0; i < s.texts.size(); ++i)
            if (s.texts[i] != g_previous)
                candidates.push_back(i);

        size_t index = candidates.empty() ? 0 : candidates[
            std::uniform_int_distribution<size_t>(
                0, candidates.size() - 1
            )(random)
        ];

        g_previous = s.texts[index];
        g_text.Text(g_previous);
        g_shadow.Text(g_previous);
        //g_text.Text(s.texts.front());
        //g_shadow.Text(s.texts.front());
        g_refresh = false;
    }

    if (attached || dirty || (s.adaptiveScale && refreshed)) {
        Apply(s);
    }

    return true;
}

static void StopAttachTimer() {
    if (!g_attachTimer) return;
    g_attachTimer.Stop();
    g_attachTimer.Tick(g_attachTickToken);
    g_attachTimer = nullptr;
}

static void UpdateVisibleSplash() {
    if (!g_visible || g_stopping) return;
    if (Update()) {
        StopAttachTimer();
    } else if (!g_attachTimer) {
        // The visibility event can arrive before XAML has created its content.
        DispatcherTimer timer;
        timer.Interval(std::chrono::milliseconds(50));
        g_attachAttempts = 0;
        g_attachTickToken = timer.Tick([](auto const&, auto const&) {
            try {
                if (g_stopping || !g_visible || ++g_attachAttempts == 56 || Update()) {
                    StopAttachTimer();
                } else if (g_attachAttempts == 20) {
                    g_attachTimer.Interval(std::chrono::milliseconds(250));
                }
            } catch (hresult_error const& e) {
                StopAttachTimer();
                Wh_Log(L"Attaching splash after XAML startup failed: %s", e.message().c_str());
            } catch (...) {
                StopAttachTimer();
                Wh_Log(L"Attaching splash after XAML startup failed");
            }
        });
        g_attachTimer = timer;
        timer.Start();
    }
}

static void RefreshOnShow() {
    // LockApp can resume on this callback before Windhawk delivers SettingsChanged.
    LoadSettings();
    g_refresh = true;
    SetEvent(g_reloadFile);
}

static void StartObserving() {
    if (g_coreWindow) return;
    auto core = CoreWindow::GetForCurrentThread();
    if (!core) return;
    if (!g_dispatchWindow) {
        // Give teardown a message target even if LockApp destroys its CoreWindow.
        g_dispatchWindow = CreateWindowExW(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                                           HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!g_dispatchWindow) {
            Wh_Log(L"Creating splash dispatch window failed: %u", GetLastError());
            return;
        }
    }
    g_visible = core.Visible();
    g_visibilityToken = core.VisibilityChanged([](CoreWindow const&, VisibilityChangedEventArgs const& args) {
        if (g_stopping) return;
        try {
            g_visible = args.Visible();
            if (g_visible) {
                RefreshOnShow();
            } else {
                StopAttachTimer();
            }
        } catch (hresult_error const& e) {
            Wh_Log(L"Splash visibility update failed: %s", e.message().c_str());
        } catch (...) {
            Wh_Log(L"Splash visibility update failed");
        }
    });
    g_coreWindow = core;
    g_observing = true;
    if (g_visible) RefreshOnShow();
}

static void StopObserving() {
    if (g_coreWindow) {
        g_coreWindow.VisibilityChanged(g_visibilityToken);
        g_coreWindow = nullptr;
    }
    StopAttachTimer();
    g_observing = false;
    Detach();
}

enum class UiAction : WPARAM { Start, Stop, Refresh };

static LRESULT CALLBACK WindowHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<CWPSTRUCT const*>(lParam);
        try {
            if (message->message == g_message &&
                message->lParam == reinterpret_cast<LPARAM>(&g_message)) {
                if (message->wParam == static_cast<WPARAM>(UiAction::Stop)) {
                    StopObserving();
                    DestroyWindow(g_dispatchWindow);
                    g_dispatchWindow = nullptr;
                    Wh_Log(L"Splash detached on the lock screen UI thread");
                } else if (message->wParam == static_cast<WPARAM>(UiAction::Start) && !g_stopping) {
                    StartObserving();
                } else if (message->wParam == static_cast<WPARAM>(UiAction::Refresh)) {
                    UpdateVisibleSplash();
                }
            }
        } catch (hresult_error const& e) { Wh_Log(L"Splash XAML error: %s", e.message().c_str()); }
        catch (...) { Wh_Log(L"Splash update failed"); }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static void RunOnWindowThread(HWND hwnd, UiAction action) {
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(hwnd, &pid);
    if (!thread || pid != GetCurrentProcessId()) return;
    // The calling thread owns the hook. Keep it alive until delivery completes.
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, WindowHook, nullptr, thread);
    if (!hook) {
        Wh_Log(L"Installing splash UI dispatch hook failed: %u", GetLastError());
        return;
    }
    SendMessageW(hwnd, g_message, static_cast<WPARAM>(action), reinterpret_cast<LPARAM>(&g_message));
    UnhookWindowsHookEx(hook);
}

static BOOL CALLBACK FindCoreWindowProc(HWND hwnd, LPARAM parameter) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    wchar_t name[128]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    if (pid == GetCurrentProcessId() && wcscmp(name, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(parameter) = hwnd;
        return FALSE;
    }
    return TRUE;
}

static DWORD WINAPI Worker(void*) {
    while (!g_observing && !g_stopping) {
        HWND hwnd = nullptr;
        EnumWindows(FindCoreWindowProc, reinterpret_cast<LPARAM>(&hwnd));
        if (hwnd) RunOnWindowThread(hwnd, UiAction::Start);
        if (g_observing || WaitForSingleObject(g_stop, 250) != WAIT_TIMEOUT) break;
    }
    FileCache cache;
    const HANDLE events[] = {g_stop, g_reloadFile};
    while (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
        if (ReloadFileTexts(cache) && !g_stopping) {
            if (auto hwnd = g_dispatchWindow.load()) RunOnWindowThread(hwnd, UiAction::Refresh);
        }
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_message = RegisterWindowMessageW(L"Windhawk.LockScreenSplashes." WH_MOD_ID);
    if (!g_message) return FALSE;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop) return FALSE;
    g_reloadFile = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    if (!g_reloadFile) {
        CloseHandle(g_stop);
        return FALSE;
    }
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_worker) {
        CloseHandle(g_reloadFile);
        CloseHandle(g_stop);
        return FALSE;
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    SetEvent(g_stop);
    if (auto hwnd = g_dispatchWindow.load(); hwnd && IsWindow(hwnd)) {
        RunOnWindowThread(hwnd, UiAction::Stop);
    }
    WaitForSingleObject(g_worker, INFINITE);
    // Bootstrap may have been in progress when the stop request arrived.
    if (auto hwnd = g_dispatchWindow.load(); hwnd && IsWindow(hwnd)) {
        RunOnWindowThread(hwnd, UiAction::Stop);
    }
    if (g_observing) Wh_Log(L"Splash event cleanup was not confirmed by the UI thread");
}

void Wh_ModUninit() {
    CloseHandle(g_worker);
    CloseHandle(g_reloadFile);
    CloseHandle(g_stop);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    SetEvent(g_reloadFile);
}
