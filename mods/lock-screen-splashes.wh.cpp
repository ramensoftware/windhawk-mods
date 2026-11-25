// ==WindhawkMod==
// @id              lock-screen-splashes
// @name            Lock Screen Splashes
// @name:zh-CN      锁屏闪烁标语
// @description     Minecraft-style animated splash texts below the lock screen date.
// @description:zh-CN 在锁屏日期下方显示 Minecraft 风格的动态标语。
// @version         0.3.0
// @author          4tubborn
// @github          https://github.com/4tubborn
// @license         GPL-3.0-or-later
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lwtsapi32
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
- splashTexts:
  - "Works on my machine"
  - "Also try Linux"
  - "Also try Explorer Patcher"
  - "Now with more WebView2"
  - "Java 5 Compatible"
  - "AI-generated"
  - "AI slop"
  - "Made of 32-bit components"
  - "It is now unsafe to turn off your computer."
  - "Make Windows Great Again!"
  - "Removed Herobrine"
  - "Also try Windows 7"
  - "You swan, he frog!"
  - "Surf the Internet"
  - "No more UWP"
  - "You are undefined!"
  - "Life is a memory leak"
  - "Hurry up!"
  - "Switch to Mac"
  - "Write once, crash everywhere"
  - "Legacy Innovations!"
  - "Innovating since 1995!"
  - "No Copilot required"
  - "Native code"
  - "Innovation through archaeology"
  - "Task failed successfully"
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
  - "Please wait"
  - "Failed to fail"
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
  $name: Splash texts
- splashFile: ""
  $name: Optional splashes.txt path (UTF-8)
- fontFamily: Segoe UI
  $name: Font family
- fallbackFontFamily: ""
  $name: Fallback font families
  $description: "Optional comma-separated list, e.g. Microsoft YaHei UI, Segoe UI Emoji."
- fontSize: 24
  $name: Font size
- color: "# "
  $name: Color (#RRGGBB or #AARRGGBB)
- rotation: -20
  $name: Rotation (degrees)
- animation: true
  $name: Enable pulse animation
- minimumScale: 100
  $name: Minimum scale (%)
- maximumScale: 120
  $name: Maximum scale (%)
- halfCycleMs: 500
  $name: Pulse half-cycle (milliseconds)
- offsetX: 100
  $name: Horizontal offset
- offsetY: -40
  $name: Vertical offset
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wtsapi32.h>
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
    double size, rotation, low, high, x, y;
    int halfCycle;
    bool animate;
};
static Settings g_settings;
static std::mutex g_mutex;
static std::atomic<bool> g_dirty{true};
static HANDLE g_stop, g_worker;
static std::atomic<HWND> g_window{nullptr};
static HHOOK g_hook;
static UINT g_message;
static bool g_registered;
static std::atomic<bool> g_detached{false};
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

static std::vector<std::wstring> ReadFileTexts(std::wstring const& path) {
    std::vector<std::wstring> lines;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Splash file open failed: %u", GetLastError());
        return lines;
    }
    LARGE_INTEGER size{};
    bool valid = GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart <= 1024 * 1024;
    std::string bytes(valid ? static_cast<size_t>(size.QuadPart) : 0, '\0');
    DWORD read = 0;
    valid = valid && ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read, nullptr)
        && read == bytes.size();
    CloseHandle(file);
    if (!valid) { Wh_Log(L"Splash file empty, too large, or unreadable"); return lines; }
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), read, nullptr, 0);
    if (!count) { Wh_Log(L"Splash file is not valid UTF-8"); return lines; }
    std::wstring text(count, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), read, text.data(), count);
    size_t start = text[0] == 0xFEFF ? 1 : 0;
    while (start < text.size() && lines.size() < 4096) {
        auto end = text.find(L'\n', start);
        AddLine(lines, text.substr(start, end == std::wstring::npos ? end : end - start));
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return lines;
}

static void LoadSettings() {
    Settings s{};
    for (int i = 0; i < 4096; ++i) {
        auto key = L"splashTexts[" + std::to_wstring(i) + L"]";
        auto value = StringSetting(key.c_str());
        if (value.empty()) break;
        AddLine(s.texts, value);
    }
    auto file = StringSetting(L"splashFile");
    if (!file.empty()) {
        auto texts = ReadFileTexts(file);
        if (!texts.empty()) s.texts = std::move(texts);
    }
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
    s.rotation = std::clamp(Wh_GetIntSetting(L"rotation"), -180, 180);
    s.low = std::clamp(Wh_GetIntSetting(L"minimumScale"), 10, 300) / 100.0;
    s.high = std::clamp(Wh_GetIntSetting(L"maximumScale"), 10, 300) / 100.0;
    if (s.low > s.high) std::swap(s.low, s.high);
    s.halfCycle = std::clamp(Wh_GetIntSetting(L"halfCycleMs"), 100, 10000);
    s.x = std::clamp(Wh_GetIntSetting(L"offsetX"), -2000, 2000);
    s.y = std::clamp(Wh_GetIntSetting(L"offsetY"), -2000, 2000);
    s.animate = Wh_GetIntSetting(L"animation") != 0;
    std::lock_guard lock(g_mutex);
    g_settings = std::move(s);
    g_dirty = true;
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
    TranslateTransform shadowOffset;
    shadowOffset.X(2);
    shadowOffset.Y(2);
    shadow.RenderTransform(shadowOffset);
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
    g_text.Foreground(SolidColorBrush(s.color));
    g_shadow.Foreground(SolidColorBrush(Windows::UI::Color{
        static_cast<uint8_t>(s.color.A * 3 / 4), 0, 0, 0}));
    CompositeTransform transform;
    transform.Rotation(s.rotation);
    transform.TranslateX(s.x);
    transform.TranslateY(s.y);
    transform.ScaleX(s.animate ? s.low : 1.0);
    transform.ScaleY(s.animate ? s.low : 1.0);
    g_splash.RenderTransform(transform);
    if (!s.animate || s.low == s.high) return;
    g_story = Storyboard();
    for (auto property : {L"ScaleX", L"ScaleY"}) {
        DoubleAnimation animation;
        animation.From(s.low);
        animation.To(s.high);
        animation.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(s.halfCycle)));
        animation.AutoReverse(true);
        animation.RepeatBehavior(RepeatBehaviorHelper::Forever());
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
    bool dirty = g_dirty.exchange(false);
    if (!dirty && !attached && !g_refresh) return true;
    Settings s;
    { std::lock_guard lock(g_mutex); s = g_settings; }
    if (attached || dirty) Apply(s);
    if (attached || dirty || g_refresh) {
        static std::mt19937 random(GetTickCount());
        std::vector<size_t> candidates;
        for (size_t i = 0; i < s.texts.size(); ++i)
            if (s.texts[i] != g_previous) candidates.push_back(i);
        size_t index = candidates.empty() ? 0 : candidates[
            std::uniform_int_distribution<size_t>(0, candidates.size() - 1)(random)];
        g_previous = s.texts[index];
        g_text.Text(g_previous);
        g_shadow.Text(g_previous);
        g_refresh = false;
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
        timer.Interval(std::chrono::milliseconds(250));
        g_attachAttempts = 0;
        g_attachTickToken = timer.Tick([](auto const&, auto const&) {
            try {
                if (g_stopping || !g_visible || ++g_attachAttempts == 40 || Update()) {
                    StopAttachTimer();
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

static void StartObserving() {
    if (g_coreWindow) return;
    auto core = CoreWindow::GetForCurrentThread();
    if (!core) return;
    g_visible = core.Visible();
    g_visibilityToken = core.VisibilityChanged([](CoreWindow const&, VisibilityChangedEventArgs const& args) {
        if (g_stopping) return;
        try {
            g_visible = args.Visible();
            if (g_visible) {
                g_refresh = true;
                UpdateVisibleSplash();
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
}

static void StopObserving() {
    if (g_coreWindow) {
        g_coreWindow.VisibilityChanged(g_visibilityToken);
        g_coreWindow = nullptr;
    }
    StopAttachTimer();
    Detach();
}

static LRESULT CALLBACK WindowHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<CWPSTRUCT const*>(lParam);
        try {
            if (message->message == WM_WTSSESSION_CHANGE && message->wParam == WTS_SESSION_LOCK) {
                g_refresh = true;
                UpdateVisibleSplash();
            }
            // Hook wParam is zero for our cross-thread SendMessage calls.
            if (message->message == g_message &&
                message->lParam == reinterpret_cast<LPARAM>(&g_message)) {
                if (message->wParam == 1) {
                    StopObserving();
                    g_detached = true;
                    Wh_Log(L"Splash detached on the lock screen UI thread");
                } else if (!g_stopping) {
                    StartObserving();
                    UpdateVisibleSplash();
                }
            }
        } catch (hresult_error const& e) { Wh_Log(L"Splash XAML error: %s", e.message().c_str()); }
        catch (...) { Wh_Log(L"Splash update failed"); }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static BOOL CALLBACK FindWindow(HWND hwnd, LPARAM parameter) {
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
    while (!g_observing && WaitForSingleObject(g_stop, 250) == WAIT_TIMEOUT) {
        if (!g_hook) {
            HWND hwnd = nullptr;
            EnumWindows(FindWindow, reinterpret_cast<LPARAM>(&hwnd));
            if (!hwnd) continue;
            g_hook = SetWindowsHookExW(WH_CALLWNDPROC, WindowHook, nullptr,
                GetWindowThreadProcessId(hwnd, nullptr));
            if (!g_hook) continue;
            g_window = hwnd;
            g_registered = WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION) != FALSE;
            Wh_Log(L"Splash UI hook installed; session notifications: %d", g_registered);
        }
        // Only bootstrap the subscription. Once registered, UI events drive updates.
        SendMessageW(g_window, g_message, 0, reinterpret_cast<LPARAM>(&g_message));
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_message = RegisterWindowMessageW(L"Windhawk.LockScreenSplashes." WH_MOD_ID);
    if (!g_message) return FALSE;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop) return FALSE;
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_worker) { CloseHandle(g_stop); g_stop = nullptr; return FALSE; }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    SetEvent(g_stop);
    WaitForSingleObject(g_worker, INFINITE);
    if (g_window && IsWindow(g_window)) {
        SendMessageW(g_window, g_message, 1, reinterpret_cast<LPARAM>(&g_message));
        if (!g_detached) Wh_Log(L"Splash teardown was not confirmed by the UI thread");
        if (g_registered) WTSUnRegisterSessionNotification(g_window);
    }
    if (g_hook) UnhookWindowsHookEx(g_hook);
}

void Wh_ModUninit() {
    CloseHandle(g_worker);
    CloseHandle(g_stop);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (auto hwnd = g_window.load()) {
        SendNotifyMessageW(hwnd, g_message, 0, reinterpret_cast<LPARAM>(&g_message));
    }
}
