// ==WindhawkMod==
// @id              taskbar-open-window-underline
// @name            Taskbar Open Window Underline
// @description     Underlines the labels (and optionally the icons) of taskbar buttons whose windows are shown on the desktop, so open and minimized windows can be told apart at a glance
// @version         2.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Open Window Underline

Underlines the label of every Windows 11 taskbar button that has a window
currently shown on the desktop. Buttons whose windows are all minimized keep a
plain label, so you can tell at a glance which apps actually have something
open in front of you.

![Labels underlined](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/taskbar-open-window-underline/demo.png)
*Whole label*

![First letter underlined](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/taskbar-open-window-underline/demo2.png)
*First letter only*

![Icons underlined](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/taskbar-open-window-underline/demo3.png)
*Icon underline (experimental), here with hidden labels*

Requirements:

* Windows 11 with the XAML taskbar (22H2 or newer).
* For the label underline, taskbar labels must be visible: Settings →
  Personalization → Taskbar → Taskbar behaviors → "Combine taskbar buttons and
  hide labels" set to **Never** (or **When taskbar is full**). The "Show
  labels, combine taskbar buttons" mode of the Taskbar Labels mod is supported
  as well; a combined group is underlined when any (or, optionally, every) one
  of its windows is shown.

Icon underline (experimental): the "Icon underline" section adds a line under
the button's icon with the same rule, for taskbars that hide their labels or
as a stronger cue next to the label. Thickness, width, distance from the icon,
corner radius, color and opacity are configurable. The color can differ
between the light and dark theme, can be a horizontal gradient, and defaults
to the system accent color. Icons aren't text, so this line is a composition
visual attached to the icon element; it follows the icon's size and position
without touching the taskbar's layout. With hidden labels it relies on the
view-model symbols described below: on builds where they can't be resolved,
the title fallback has no label text to work with, and those buttons stay
plain.

Hide the running indicator: hides the taskbar's own indicator under running
buttons, so that the underline is the only cue. Independent of the icon
underline.

How it works:

* The taskbar's buttons live in an `ItemsRepeater`; each button's item is its
  view model (`TaskListWindowViewModel` for one window, `TaskListGroupViewModel`
  for a group), and the view model knows its windows through the shell's
  `TaskItem` / `TaskGroup` objects. The mod resolves every button's windows
  that way and judges them with `IsIconic` (plus DWM cloaking for windows on
  other virtual desktops). Win+D counts as minimized.
* A WinEvent hook (minimize / restore / show / hide / cloak, and foreground or
  title changes when a setting needs them) invalidates the buttons; events are
  coalesced and every button is re-evaluated on the taskbar thread, on every
  taskbar.
* The underline is the native `TextBlock` underline, so it follows the text
  color, trimming and theme. It can cover the whole label or just its first
  letter. It is removed when the mod is disabled.
* The icon underline is a `SpriteVisual` attached to the `Image#Icon` element
  with `ElementCompositionPreview::SetElementChildVisual`, sized relative to
  the icon so it needs no layout work; rounded corners are a geometric clip
  whose geometry follows the icon's size. The running indicator is hidden
  through its element's composition visual, so the taskbar's visual states
  are left untouched. Both are undone when the mod is disabled.
* On builds where the view-model symbols can't be resolved, the mod falls back
  to matching label text against window titles, which is exact for "Never
  combine" but blind to combined groups.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: open
  $name: What to underline
  $options:
  - open: Buttons with a window shown on the desktop
  - minimized: Buttons whose windows are all minimized
- underlineStyle: full
  $name: Underline style
  $options:
  - full: The whole label
  - firstLetter: The first letter only
- groupRule: any
  $name: Buttons with several windows
  $description: >-
    Applies to "Buttons with a window shown on the desktop".
  $options:
  - any: Shown if any of the windows is shown
  - all: Shown only if every window is shown
- includeActive: true
  $name: Count the foreground window as shown
  $description: >-
    Applies to "Buttons with a window shown on the desktop". When disabled, a
    button whose only shown window is the foreground one is not underlined,
    since it already has the active indicator.
- cloakedIsHidden: true
  $name: Treat cloaked windows as not shown
  $description: >-
    Cloaked windows include windows on other virtual desktops and suspended
    Store apps.
- refreshDelayMs: 60
  $name: Refresh delay (ms)
  $description: >-
    Window-state events are coalesced for this long before the buttons are
    re-evaluated. Win+M and Win+D fire dozens of events at once.
- hideRunningIndicator: false
  $name: Hide the running indicator
  $description: >-
    Hides the taskbar's own indicator under running buttons, so the underline
    is the only cue. Works with or without the icon underline.
- iconUnderline:
  - enabled: false
    $name: Enabled
    $description: >-
      Draws a line under the button's icon with the same rule as the label
      underline. Works when labels are hidden, too.
  - thickness: 2
    $name: Thickness (px)
  - widthPercent: 100
    $name: Width (% of the icon width)
    $description: >-
      100 matches the icon; smaller values center a shorter line under it,
      larger values extend past it.
  - offset: 2
    $name: Distance from the icon (px)
    $description: >-
      Gap between the bottom of the icon and the line. Negative values move
      the line up over the icon.
  - color: ""
    $name: Color
    $description: >-
      Hex color code. Supports Light | Dark separation (e.g.
      '#000000|#FFFFFF') and multi-color horizontal gradients (e.g.
      '#FF0000,#00FF00'). Both can be combined ('light1,light2|dark1,dark2').
      Leave empty to use the system accent color.
  - opacity: 100
    $name: Opacity (%)
  - cornerRadius: 1
    $name: Corner radius (px)
    $description: >-
      0 for square ends. Values above half the thickness are clamped, so a
      radius of at least half the thickness gives a pill.
  $name: Icon underline (experimental)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <inspectable.h>

#include <atomic>
#include <cstdio>
#include <cwctype>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

// The taskbar items live in a WinUI 2 ItemsRepeater built on system XAML;
// its projection is needed to ask the repeater which item a button shows.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace Xaml = winrt::Windows::UI::Xaml;
namespace Composition = winrt::Windows::UI::Composition;
using winrt::Windows::UI::Text::TextDecorations;

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

// Posted to the event thread after a settings change, to re-register the
// WinEvent hooks for the new settings.
#define WM_APP_RECONFIGURE_HOOKS (WM_APP + 1)

// Posted to the event thread when the system colors changed, to schedule a
// refresh pass that picks up the new icon underline color.
#define WM_APP_COLORS_CHANGED (WM_APP + 2)

// Delay for the second pass that follows every coalesced refresh, to catch
// window-state transitions that were still in flight during the first pass.
#define FOLLOW_UP_DELAY_MS 300

// ============================================================================
// Settings
// ============================================================================

enum class Mode { kOpen, kMinimized };
enum class UnderlineStyle { kFull, kFirstLetter };
enum class GroupRule { kAny, kAll };

struct IconUnderlineSettings {
    bool enabled;
    float thickness;
    float widthFraction;
    float offset;
    // Colors for the light and the dark system theme. One color is a solid
    // line, several make a horizontal gradient, none means the accent color.
    std::vector<winrt::Windows::UI::Color> lightColors;
    std::vector<winrt::Windows::UI::Color> darkColors;
    float opacity;
    float cornerRadius;
};

// Read on the taskbar thread and the event thread; written on the taskbar
// thread (LoadSettings runs there via RunFromWindowThread) and, before any
// hook is applied, in Wh_ModInit.
struct {
    Mode mode;
    UnderlineStyle underlineStyle;
    GroupRule groupRule;
    bool includeActive;
    bool cloakedIsHidden;
    int refreshDelayMs;
    bool hideRunningIndicator;
    IconUnderlineSettings iconUnderline;
} g_settings;

std::wstring_view TrimSpaces(std::wstring_view text) {
    while (!text.empty() && text.front() == L' ') {
        text.remove_prefix(1);
    }
    while (!text.empty() && text.back() == L' ') {
        text.remove_suffix(1);
    }
    return text;
}

// Parses "#RRGGBB" or "#AARRGGBB" (the '#' is optional). Returns false for
// anything else, including an empty string.
bool ParseColor(std::wstring_view text, winrt::Windows::UI::Color* color) {
    text = TrimSpaces(text);
    if (!text.empty() && text.front() == L'#') {
        text.remove_prefix(1);
    }

    size_t length = text.size();
    if (length != 6 && length != 8) {
        return false;
    }

    uint32_t value = 0;
    for (size_t i = 0; i < length; i++) {
        wchar_t c = text[i];
        uint32_t digit;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            digit = c - L'a' + 10;
        } else if (c >= L'A' && c <= L'F') {
            digit = c - L'A' + 10;
        } else {
            return false;
        }
        value = (value << 4) | digit;
    }

    if (length == 6) {
        value |= 0xFF000000;
    }

    color->A = static_cast<uint8_t>(value >> 24);
    color->R = static_cast<uint8_t>(value >> 16);
    color->G = static_cast<uint8_t>(value >> 8);
    color->B = static_cast<uint8_t>(value);
    return true;
}

// Parses a comma-separated list of colors. An empty list is valid (it means
// "accent color"); a list with an unparsable entry is not.
bool ParseColorList(std::wstring_view text,
                    std::vector<winrt::Windows::UI::Color>* colors) {
    colors->clear();
    text = TrimSpaces(text);
    if (text.empty()) {
        return true;
    }

    while (true) {
        size_t comma = text.find(L',');
        std::wstring_view token =
            comma == std::wstring_view::npos ? text : text.substr(0, comma);

        winrt::Windows::UI::Color color;
        if (!ParseColor(token, &color)) {
            colors->clear();
            return false;
        }
        colors->push_back(color);

        if (comma == std::wstring_view::npos) {
            return true;
        }
        text = text.substr(comma + 1);
    }
}

// Parses the color setting: "light|dark", each side a color list. Without a
// '|' both themes share the list.
void ParseIconColorSetting(const wchar_t* text, IconUnderlineSettings* icon) {
    std::wstring_view spec = text ? text : L"";
    size_t bar = spec.find(L'|');
    std::wstring_view lightSpec =
        bar == std::wstring_view::npos ? spec : spec.substr(0, bar);
    std::wstring_view darkSpec =
        bar == std::wstring_view::npos ? spec : spec.substr(bar + 1);

    if (!ParseColorList(lightSpec, &icon->lightColors) ||
        !ParseColorList(darkSpec, &icon->darkColors)) {
        Wh_Log(L"Icon underline color not recognized: %s", spec.data());
        icon->lightColors.clear();
        icon->darkColors.clear();
    }
}

int ClampInt(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}

void LoadSettings() {
    auto mode = WindhawkUtils::StringSetting::make(L"mode");
    g_settings.mode =
        wcscmp(mode.get(), L"minimized") == 0 ? Mode::kMinimized : Mode::kOpen;

    auto underlineStyle = WindhawkUtils::StringSetting::make(L"underlineStyle");
    g_settings.underlineStyle =
        wcscmp(underlineStyle.get(), L"firstLetter") == 0
            ? UnderlineStyle::kFirstLetter
            : UnderlineStyle::kFull;

    auto groupRule = WindhawkUtils::StringSetting::make(L"groupRule");
    g_settings.groupRule =
        wcscmp(groupRule.get(), L"all") == 0 ? GroupRule::kAll : GroupRule::kAny;

    g_settings.includeActive = Wh_GetIntSetting(L"includeActive") != 0;
    g_settings.cloakedIsHidden = Wh_GetIntSetting(L"cloakedIsHidden") != 0;

    g_settings.refreshDelayMs =
        ClampInt(Wh_GetIntSetting(L"refreshDelayMs"), 10, 2000);
    g_settings.hideRunningIndicator =
        Wh_GetIntSetting(L"hideRunningIndicator") != 0;

    IconUnderlineSettings& icon = g_settings.iconUnderline;
    icon.enabled = Wh_GetIntSetting(L"iconUnderline.enabled") != 0;
    icon.thickness = static_cast<float>(
        ClampInt(Wh_GetIntSetting(L"iconUnderline.thickness"), 1, 20));
    icon.widthFraction =
        ClampInt(Wh_GetIntSetting(L"iconUnderline.widthPercent"), 1, 300) /
        100.0f;
    icon.offset = static_cast<float>(
        ClampInt(Wh_GetIntSetting(L"iconUnderline.offset"), -64, 64));
    auto color = WindhawkUtils::StringSetting::make(L"iconUnderline.color");
    ParseIconColorSetting(color.get(), &icon);
    icon.opacity =
        ClampInt(Wh_GetIntSetting(L"iconUnderline.opacity"), 0, 100) / 100.0f;
    icon.cornerRadius = static_cast<float>(
        ClampInt(Wh_GetIntSetting(L"iconUnderline.cornerRadius"), 0, 10));
    if (icon.cornerRadius > icon.thickness / 2.0f) {
        icon.cornerRadius = icon.thickness / 2.0f;
    }
}

// ============================================================================
// Global state
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Set once the first XAML tree root is registered. Until then there is no
// taskbar to refresh, so the WinEvent hooks stay unregistered. The event
// thread id is published by that thread itself; RegisterRoot uses it to ask
// the thread to re-register its hooks.
std::atomic<bool> g_treeKnown;
std::atomic<DWORD> g_eventThreadId;

// Everything below is touched on the taskbar thread only: the hooks run
// there, and refresh, restore and settings reload are marshaled to it with
// RunFromWindowThread. The containers hold only weak references and plain
// data, which are safe to destroy at process exit.

// One entry per XAML tree we know of (one per taskbar). Every refresh pass
// enumerates the buttons from the root, so recycled or newly created buttons
// are always found.
struct RootEntry {
    void* key;
    winrt::weak_ref<Xaml::DependencyObject> root;
};
std::vector<RootEntry> g_roots;

// Every LabelControl TextBlock we subscribed to, so the subscription can be
// removed when the mod unloads.
struct LabelEntry {
    winrt::weak_ref<Xaml::Controls::TextBlock> label;
    int64_t textChangedToken;
};
std::unordered_map<void*, LabelEntry> g_labels;

// Every icon Image we attached an underline visual to, so the visual can be
// detached when the mod unloads. The visual itself is not kept here; it is
// found again through the element and recognized by its Comment.
std::unordered_map<void*, winrt::weak_ref<Xaml::Controls::Image>> g_icons;

// Every RunningIndicator element we hid, so it can be shown again when the
// option is turned off or the mod unloads.
std::unordered_map<void*, winrt::weak_ref<Xaml::UIElement>> g_indicators;

// The icon colors in use, resolved once and kept until the settings, the
// system theme or the accent color change. The accent color is tracked with
// UISettings::ColorValuesChanged (raised on a worker thread, which only sets
// the dirty flag and nudges the event thread); the theme is a registry value
// that is re-read on every evaluation, which is cheap and also covers a theme
// switch that doesn't raise that event.
[[clang::no_destroy]] winrt::Windows::UI::ViewManagement::UISettings
    g_uiSettings{nullptr};
winrt::event_token g_colorValuesChangedToken;
std::optional<std::vector<winrt::Windows::UI::Color>> g_iconColors;
bool g_iconColorsLightTheme;
std::atomic<bool> g_iconColorsDirty;

// ============================================================================
// Running code on the taskbar thread
// ============================================================================

// Looks the taskbar window up among this process's own windows. FindWindow
// would return the first Shell_TrayWnd in Z-order, which can belong to
// another process (alternative shells such as RetroBar or Cairo Shell create
// their own Shell_TrayWnd to host tray icons).
HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs `proc` synchronously on the thread that owns `hWnd`. All taskbars,
// primary and secondary, run on the Shell_TrayWnd thread, so one call covers
// them all.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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

bool RunOnTaskbarThread(RunFromWindowThreadProc_t proc, void* procParam) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }
    return RunFromWindowThread(hTaskbarWnd, proc, procParam);
}

// ============================================================================
// Window-state source
// ============================================================================

struct WindowSnapshot {
    // Title -> top-level windows carrying that title, taskbar-eligible only.
    std::unordered_map<std::wstring, std::vector<HWND>> byTitle;
    // The same for every visible top-level window, used when the strict
    // map has no match (elevated windows such as Task Manager, say).
    std::unordered_map<std::wstring, std::vector<HWND>> byTitleAll;
};

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

// Approximates the shell's own rules for which top-level windows get a
// taskbar button.
bool IsTaskbarCandidate(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_APPWINDOW) {
        return true;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER)) {
        return false;
    }

    return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* snapshot = reinterpret_cast<WindowSnapshot*>(lParam);

    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    // InternalGetWindowText never sends messages, unlike GetWindowText for
    // windows of this process (each File Explorer window has its own thread,
    // and a hung one would freeze the taskbar thread this runs on).
    WCHAR title[1024];
    int length = InternalGetWindowText(hwnd, title, ARRAYSIZE(title));
    if (length <= 0) {
        return TRUE;
    }

    std::wstring titleString(title, length);
    snapshot->byTitleAll[titleString].push_back(hwnd);
    if (IsTaskbarCandidate(hwnd)) {
        snapshot->byTitle[titleString].push_back(hwnd);
    }
    return TRUE;
}

WindowSnapshot BuildWindowSnapshot() {
    WindowSnapshot snapshot;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&snapshot));
    return snapshot;
}

// Builds the title snapshot on first use only; the view-model bridge doesn't
// need it at all.
struct LazyWindowSnapshot {
    const WindowSnapshot& get() {
        if (!snapshot) {
            snapshot.emplace(BuildWindowSnapshot());
        }
        return *snapshot;
    }

   private:
    std::optional<WindowSnapshot> snapshot;
};

bool IsWindowShown(HWND hwnd) {
    // Covers hidden windows and windows that were just closed but are still
    // listed in their group until the taskbar catches up: for a destroyed
    // HWND, IsIconic returns FALSE and DWMWA_CLOAKED fails, which would count
    // it as shown.
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    // IsIconic is the source of truth. Win+D minimizes windows for real, so
    // it is covered here without special handling.
    if (IsIconic(hwnd)) {
        return false;
    }

    if (g_settings.cloakedIsHidden && IsWindowCloaked(hwnd)) {
        return false;
    }

    return true;
}

// ============================================================================
// Decision logic
// ============================================================================

// Given the windows behind one button, decides whether its label should be
// underlined under the current settings.
bool ShouldUnderline(const std::vector<HWND>& windows) {
    if (windows.empty()) {
        return false;
    }

    size_t shownCount = 0;
    for (HWND hwnd : windows) {
        if (IsWindowShown(hwnd)) {
            shownCount++;
        }
    }

    if (g_settings.mode == Mode::kMinimized) {
        return shownCount == 0;
    }

    // Shown mode. The foreground window is shown by definition, but can be
    // excluded from being the sole reason for an underline.
    bool foregroundShown = false;
    if (!g_settings.includeActive) {
        HWND foreground = GetForegroundWindow();
        for (HWND hwnd : windows) {
            if (hwnd == foreground && IsWindowShown(hwnd)) {
                foregroundShown = true;
                break;
            }
        }
    }
    size_t shownBesidesForeground = shownCount - (foregroundShown ? 1 : 0);

    if (g_settings.groupRule == GroupRule::kAll) {
        return shownCount == windows.size() &&
               (g_settings.includeActive || shownBesidesForeground > 0);
    }

    return shownBesidesForeground > 0;
}

// ============================================================================
// Button -> windows resolver, part 1: the taskbar's own view model
// ============================================================================
//
// Every TaskListButton is created by an ItemsRepeater for an item, and the
// item is its view model: a TaskListWindowViewModel for one window, or a
// TaskListGroupViewModel for a combined group (always the case with Taskbar
// Labels' "Show labels, combine taskbar buttons" mode). Both are backed by
// WindowsUdk.UI.Shell objects that know their HWNDs. The interface layouts
// and symbols below are the ones the Taskbar Labels mod uses.

// WindowsUdk.UI.Shell interfaces, declared up to the last used method. A WinRT
// interface doesn't change once published, so a successful query by IID
// guarantees the layout.

// {b081d9d6-9b45-5363-8a4d-854add6abe7e}
constexpr winrt::guid IID_ITaskItem{
    0xb081d9d6,
    0x9b45,
    0x5363,
    {0x8a, 0x4d, 0x85, 0x4a, 0xdd, 0x6a, 0xbe, 0x7e}};

struct ITaskItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(HWND* value) = 0;
};

// {a152e779-93df-5be2-af2c-d342452b0ce0}
constexpr winrt::guid IID_ITaskGroup{
    0xa152e779,
    0x93df,
    0x5be2,
    {0xaf, 0x2c, 0xd3, 0x42, 0x45, 0x2b, 0x0c, 0xe0}};

struct ITaskGroup : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Icon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIcon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIconDescription() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Badge() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRequestingAttention() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Items(void** value) = 0;
};

// Taskbar.View.dll symbols, all optional: without them the title fallback
// is used.

void* TaskListWindowViewModel_ITaskListWindowViewModel_vftable;
void* TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskListGroupViewModel_vftable;

// produce<TaskListWindowViewModel, ITaskListWindowViewModel>::get_TaskItem
using ITaskListWindowViewModel_get_TaskItem_t = HRESULT(WINAPI*)(void* pThis,
                                                                 void** taskItem);
ITaskListWindowViewModel_get_TaskItem_t ITaskListWindowViewModel_get_TaskItem;

// produce<TaskListGroupViewModel, ITaskbarAppItemViewModel>::get_TaskGroup
using TaskListGroupViewModel_get_TaskGroup_t = HRESULT(WINAPI*)(void* pThis,
                                                                void** taskGroup);
TaskListGroupViewModel_get_TaskGroup_t TaskListGroupViewModel_get_TaskGroup;

bool IsViewModelBridgeAvailable() {
    return TaskListWindowViewModel_ITaskListWindowViewModel_vftable &&
           ITaskListWindowViewModel_get_TaskItem &&
           TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable &&
           TaskListGroupViewModel_get_TaskGroup;
}

HWND GetTaskItemWindow(IUnknown* taskItemUnknown) {
    winrt::com_ptr<ITaskItem> taskItem;
    HRESULT hr =
        taskItemUnknown->QueryInterface(IID_ITaskItem, taskItem.put_void());
    if (FAILED(hr)) {
        return nullptr;
    }

    HWND hWnd = nullptr;
    if (FAILED(taskItem->get_WindowId(&hWnd)) || !hWnd) {
        return nullptr;
    }

    // The taskbar may hand back a non-root window (a UWP core window inside
    // its frame, say); the minimized state lives on the top-level window.
    if (HWND root = GetAncestor(hWnd, GA_ROOT)) {
        hWnd = root;
    }

    return hWnd;
}

// Reads one pointer-sized value without faulting on a bad address.
bool SafeReadPointer(const void* address, void** value) {
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, value, sizeof(*value),
                             &read) &&
           read == sizeof(*value);
}

// Logs a fixed message the first time it is reached.
#define LogOnce(message)                     \
    do {                                     \
        static bool logged_ = false;         \
        if (!logged_) {                      \
            logged_ = true;                  \
            Wh_Log(message);                 \
        }                                    \
    } while (0)

enum class ViewModelKind { kNone, kWindow, kGroup };

// Decides whether the object behind `object` is a TaskListWindowViewModel or
// a TaskListGroupViewModel from the known vtable it already points to. A
// C++/WinRT object starts with one vtable pointer per implemented interface,
// so one of the known vtables is among the first slots whichever interface
// the pointer refers to.
ViewModelKind GetViewModelKind(void* object) {
    auto slots = static_cast<void**>(object);
    for (int i = 0; i < 12; i++) {
        void* vftable = nullptr;
        if (!SafeReadPointer(&slots[i], &vftable)) {
            break;
        }
        if (!vftable) {
            continue;
        }
        if (vftable == TaskListWindowViewModel_ITaskListWindowViewModel_vftable ||
            vftable == TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable) {
            return ViewModelKind::kWindow;
        }
        if (vftable == TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable ||
            vftable == TaskListGroupViewModel_ITaskListGroupViewModel_vftable) {
            return ViewModelKind::kGroup;
        }
    }
    return ViewModelKind::kNone;
}

// Finds the interface sub-object whose vtable is `vftable`, which must be a
// vtable this object's kind actually has. The pointer the items source hands
// out may be an upcast interface pointer rather than the object's first
// slot, so the wanted interface can sit on either side of it (Taskbar Labels
// walks backward from ITaskbarAppItemViewModel to ITaskListWindowViewModel
// for that reason). Nearest slot first, so a slot of this object is always
// found before anything outside it.
void* FindInterfaceByVtable(void* object, void* vftable) {
    if (!object || !vftable) {
        return nullptr;
    }

    auto slots = static_cast<void**>(object);
    for (int distance = 0; distance <= 16; distance++) {
        for (int i : {distance, -distance}) {
            void* value = nullptr;
            if (SafeReadPointer(&slots[i], &value) && value == vftable) {
                if (i != 0) {
                    static bool logged = false;
                    if (!logged) {
                        logged = true;
                        Wh_Log(L"Interface found at slot %d", i);
                    }
                }
                return &slots[i];
            }
        }
    }

    return nullptr;
}

// The ItemsRepeater hosting a button, or null when its parent isn't one.
winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater GetHostingRepeater(
    const Xaml::FrameworkElement& button) {
    auto parent = Xaml::Media::VisualTreeHelper::GetParent(button);
    if (!parent) {
        return nullptr;
    }
    return parent.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
}

// The index of the item a realized button shows, or -1 if the repeater
// doesn't currently associate the button with any item: an ItemsRepeater
// keeps recycled elements in its visual tree for reuse, and those carry
// stale labels but aren't shown.
int GetRepeaterItemIndex(
    const winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater& repeater,
    const Xaml::FrameworkElement& button,
    int count) {
    int index = repeater.GetElementIndex(button);
    if (index >= 0) {
        return index < count ? index : -1;
    }

    // GetElementIndex can decline for elements it does hold; the inverse
    // query is authoritative. Compare COM identities, since the handles may
    // hold different interfaces of the same object.
    void* buttonIdentity =
        winrt::get_abi(button.as<winrt::Windows::Foundation::IUnknown>());
    for (int i = 0; i < count; i++) {
        auto element = repeater.TryGetElement(i);
        if (element &&
            winrt::get_abi(element.as<winrt::Windows::Foundation::IUnknown>()) ==
                buttonIdentity) {
            return i;
        }
    }

    return -1;
}

enum class ButtonWindowsState {
    kUnrealized,   // A recycled repeater child: not shown, nothing to do.
    kUnavailable,  // No view model reachable; use the title fallback.
    kResolved,     // `windows` holds the button's windows (possibly none).
};

struct ButtonWindows {
    ButtonWindowsState state;
    std::vector<HWND> windows;
};

// The windows behind a button, from its view model.
ButtonWindows ResolveButtonWindows(const Xaml::FrameworkElement& button) {
    ButtonWindows result{ButtonWindowsState::kUnavailable, {}};

    winrt::com_ptr<IUnknown> viewModel;
    ViewModelKind kind = ViewModelKind::kNone;
    try {
        auto repeater = GetHostingRepeater(button);
        if (repeater) {
            auto itemsSourceView = repeater.ItemsSourceView();
            int count = itemsSourceView ? itemsSourceView.Count() : 0;
            int index = GetRepeaterItemIndex(repeater, button, count);
            if (index < 0) {
                result.state = ButtonWindowsState::kUnrealized;
                return result;
            }

            auto item = itemsSourceView.GetAt(index);
            if (item) {
                kind = GetViewModelKind(winrt::get_abi(item));
            }
            if (kind != ViewModelKind::kNone) {
                viewModel.copy_from(static_cast<IUnknown*>(winrt::get_abi(item)));
            } else if (item) {
                static std::atomic<bool> logged;
                if (!logged.exchange(true)) {
                    Wh_Log(L"Repeater item not recognized: %s",
                           winrt::get_class_name(item).c_str());
                }
            }
        } else {
            static std::atomic<bool> logged;
            if (!logged.exchange(true)) {
                auto parent = Xaml::Media::VisualTreeHelper::GetParent(button);
                Wh_Log(L"Button parent is not an ItemsRepeater: %s",
                       parent ? winrt::get_class_name(parent).c_str()
                              : L"(none)");
            }
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Repeater lookup error: %08X",
               static_cast<unsigned int>(e.code()));
    }

    if (!viewModel || !IsViewModelBridgeAvailable()) {
        return result;
    }

    // One window. Only a window view model is searched for the window
    // interface: a group view model never has that vtable, and a search for
    // it there could only match outside the object.
    if (kind == ViewModelKind::kWindow) {
        void* windowViewModel = FindInterfaceByVtable(
            viewModel.get(),
            TaskListWindowViewModel_ITaskListWindowViewModel_vftable);
        if (!windowViewModel) {
            LogOnce(L"Window view model: interface not found; title fallback");
            return result;
        }

        winrt::com_ptr<IUnknown> taskItem;
        HRESULT hr = ITaskListWindowViewModel_get_TaskItem(windowViewModel,
                                                           taskItem.put_void());
        if (SUCCEEDED(hr) && taskItem) {
            if (HWND hWnd = GetTaskItemWindow(taskItem.get())) {
                result.windows.push_back(hWnd);
            }
        }
        result.state = ButtonWindowsState::kResolved;
        LogOnce(L"Resolved a single-window button through the view model");
        return result;
    }

    // A group: all of its windows.
    if (kind == ViewModelKind::kGroup) {
        void* groupViewModel = FindInterfaceByVtable(
            viewModel.get(),
            TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable);
        if (!groupViewModel) {
            LogOnce(L"Group view model: interface not found; title fallback");
            return result;
        }

        result.state = ButtonWindowsState::kResolved;
        LogOnce(L"Resolved a group button through the view model");

        winrt::com_ptr<IUnknown> taskGroupUnknown;
        HRESULT hr = TaskListGroupViewModel_get_TaskGroup(
            groupViewModel, taskGroupUnknown.put_void());
        if (FAILED(hr) || !taskGroupUnknown) {
            return result;
        }

        winrt::com_ptr<ITaskGroup> taskGroup;
        hr = taskGroupUnknown->QueryInterface(IID_ITaskGroup,
                                              taskGroup.put_void());
        if (FAILED(hr)) {
            return result;
        }

        // An IVectorView<TaskItem>, which has the same ABI.
        winrt::Windows::Foundation::Collections::IVectorView<
            winrt::Windows::Foundation::IInspectable>
            items;
        hr = taskGroup->get_Items(winrt::put_abi(items));
        if (SUCCEEDED(hr) && items) {
            uint32_t count = items.Size();
            for (uint32_t i = 0; i < count; i++) {
                auto item = items.GetAt(i);
                if (HWND hWnd = GetTaskItemWindow(
                        static_cast<IUnknown*>(winrt::get_abi(item)))) {
                    result.windows.push_back(hWnd);
                }
            }
        }
        return result;
    }

    return result;
}

// ============================================================================
// Button -> windows resolver, part 2: title matching (fallback)
// ============================================================================
//
// Used only when the view model can't be reached. Exact for "Never combine",
// where the label is the window title; blind to combined groups.

const std::vector<HWND>* ResolveWindowsForLabel(
    const Xaml::Controls::TextBlock& label,
    const WindowSnapshot& snapshot) {
    winrt::hstring text = label.Text();
    if (text.empty()) {
        return nullptr;
    }

    // TextBlock.Text returns the full string even when the visual is
    // trimmed with an ellipsis, so this compares against the real title.
    std::wstring title(text.c_str(), text.size());
    auto it = snapshot.byTitle.find(title);
    if (it != snapshot.byTitle.end()) {
        return &it->second;
    }

    it = snapshot.byTitleAll.find(title);
    if (it != snapshot.byTitleAll.end()) {
        return &it->second;
    }

    return nullptr;
}

// ============================================================================
// XAML helpers (taskbar thread only)
// ============================================================================

bool IsTaskListButton(const Xaml::DependencyObject& element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

// The topmost ancestor of `element`, or null if it has no parent and
// `allowSelf` is false (an element that isn't in a tree yet).
Xaml::DependencyObject FindTreeRoot(const Xaml::DependencyObject& element,
                                    bool allowSelf) {
    Xaml::DependencyObject current = element;
    bool movedUp = false;

    for (int i = 0; i < 200; i++) {
        auto parent = Xaml::Media::VisualTreeHelper::GetParent(current);
        if (!parent) {
            break;
        }

        current = parent;
        movedUp = true;
    }

    return (movedUp || allowSelf) ? current : nullptr;
}

// Collects every TaskListButton under `root` without descending into the
// buttons themselves.
void CollectTaskListButtons(const Xaml::DependencyObject& root,
                            std::vector<Xaml::FrameworkElement>& result,
                            int depth = 0) {
    if (depth > 40) {
        return;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (IsTaskListButton(child)) {
            if (auto element = child.try_as<Xaml::FrameworkElement>()) {
                result.push_back(element);
            }
            continue;
        }

        CollectTaskListButtons(child, result, depth + 1);
    }
}

Xaml::Controls::TextBlock FindLabelControl(const Xaml::DependencyObject& root,
                                           int depth = 0) {
    if (depth > 12) {
        return nullptr;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (auto element = child.try_as<Xaml::FrameworkElement>()) {
            if (element.Name() == L"LabelControl") {
                if (auto textBlock = element.try_as<Xaml::Controls::TextBlock>()) {
                    return textBlock;
                }
            }
        }

        if (auto found = FindLabelControl(child, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

// The button's icon: `Image#Icon` under `Grid#IconPanel` (hidden labels) or
// `Taskbar.TaskListLabeledButtonPanel#IconPanel` (labels shown). Falls back
// to the first Image that isn't the overlay icon, for templates that name it
// differently.
Xaml::Controls::Image FindIconImage(const Xaml::DependencyObject& root,
                                    Xaml::Controls::Image* fallback,
                                    int depth = 0) {
    if (depth > 12) {
        return nullptr;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (auto image = child.try_as<Xaml::Controls::Image>()) {
            auto name = image.Name();
            if (name == L"Icon") {
                return image;
            }
            if (!*fallback && name != L"OverlayIcon") {
                *fallback = image;
            }
            continue;
        }

        if (auto found = FindIconImage(child, fallback, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

Xaml::Controls::Image FindIconImage(const Xaml::DependencyObject& root) {
    Xaml::Controls::Image fallback = nullptr;
    if (auto icon = FindIconImage(root, &fallback)) {
        return icon;
    }
    return fallback;
}

// The button's running indicator (`Rectangle#RunningIndicator`, a Border on
// some builds), found by name.
Xaml::UIElement FindRunningIndicator(const Xaml::DependencyObject& root,
                                     int depth = 0) {
    if (depth > 12) {
        return nullptr;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (auto element = child.try_as<Xaml::FrameworkElement>()) {
            if (element.Name() == L"RunningIndicator") {
                return element;
            }
        }

        if (auto found = FindRunningIndicator(child, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

// The TaskListButton that owns a LabelControl, or null.
Xaml::FrameworkElement FindOwningTaskListButton(
    const Xaml::DependencyObject& element) {
    Xaml::DependencyObject current = element;
    for (int i = 0; i < 12 && current; i++) {
        if (IsTaskListButton(current)) {
            return current.try_as<Xaml::FrameworkElement>();
        }
        current = Xaml::Media::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

// ============================================================================
// Renderer (taskbar thread only)
// ============================================================================

void ApplyToButton(const Xaml::FrameworkElement& button,
                   LazyWindowSnapshot& snapshot);

// Set while a label's text is re-set to rebuild its layout, so the Text
// callback below ignores that write.
bool g_rebuildingLabel = false;

// Subscribes to the label's Text property once, so a title change
// re-evaluates the button even when the taskbar doesn't refresh its visual
// states. Returns true if the label was not tracked before.
bool EnsureLabelTracked(const Xaml::Controls::TextBlock& label) {
    void* key = winrt::get_abi(label);

    auto it = g_labels.find(key);
    if (it != g_labels.end()) {
        auto existing = it->second.label.get();
        if (existing && existing == label) {
            return false;
        }

        // Stale entry for a dead TextBlock whose address got reused.
        g_labels.erase(it);
    }

    int64_t token = label.RegisterPropertyChangedCallback(
        Xaml::Controls::TextBlock::TextProperty(),
        [](const Xaml::DependencyObject& sender, const Xaml::DependencyProperty&) {
            if (g_unloading || g_rebuildingLabel) {
                return;
            }

            try {
                if (auto button = FindOwningTaskListButton(sender)) {
                    LazyWindowSnapshot snapshot;
                    ApplyToButton(button, snapshot);
                }
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Text change handler error: %08X",
                       static_cast<unsigned int>(e.code()));
            } catch (...) {
                Wh_Log(L"Text change handler error");
            }
        });

    g_labels.emplace(key, LabelEntry{winrt::make_weak(label), token});
    return true;
}

TextDecorations WithUnderlineBit(TextDecorations value, bool underline) {
    auto bits = static_cast<uint32_t>(value);
    auto bit = static_cast<uint32_t>(TextDecorations::Underline);
    return static_cast<TextDecorations>(underline ? (bits | bit)
                                                  : (bits & ~bit));
}

bool HasUnderlineBit(TextDecorations value) {
    return (static_cast<uint32_t>(value) &
            static_cast<uint32_t>(TextDecorations::Underline)) != 0;
}

// Rebuilds the label's inline content from its text, which replaces any
// Runs (and any cached text layout) with a single plain Run. Setting the
// same string is a no-op, so the text is cleared first; both writes happen
// before the next layout pass, so nothing flickers.
void RebuildLabelText(const Xaml::Controls::TextBlock& label) {
    winrt::hstring text = label.Text();
    g_rebuildingLabel = true;
    label.Text(L"");
    label.Text(text);
    g_rebuildingLabel = false;
    label.InvalidateMeasure();
}

bool IsCombiningMark(wchar_t c) {
    return (c >= 0x0300 && c <= 0x036F) ||  // combining diacriticals
           (c >= 0x0591 && c <= 0x05C7) ||  // Hebrew points
           (c >= 0x0610 && c <= 0x061A) ||  // Arabic marks
           (c >= 0x064B && c <= 0x065F) ||
           (c >= 0x0670 && c <= 0x0670) ||
           (c >= 0x06D6 && c <= 0x06ED) ||
           (c >= 0x1AB0 && c <= 0x1AFF) ||
           (c >= 0x1DC0 && c <= 0x1DFF) ||
           (c >= 0x20D0 && c <= 0x20FF) ||
           (c >= 0xFE00 && c <= 0xFE0F) ||  // variation selectors
           (c >= 0xFE20 && c <= 0xFE2F);
}

// Number of UTF-16 code units making up the first visible letter: leading
// whitespace is skipped, a surrogate pair stays together, and combining
// marks (Hebrew points, Arabic harakat, accents) stay with their base.
// Returns the start offset through `start`.
size_t FirstLetterSpan(std::wstring_view text, size_t* start) {
    size_t i = 0;
    while (i < text.size() && iswspace(text[i])) {
        i++;
    }
    *start = i;
    if (i >= text.size()) {
        return 0;
    }

    size_t n = 1;
    if (IS_HIGH_SURROGATE(text[i]) && i + 1 < text.size() &&
        IS_LOW_SURROGATE(text[i + 1])) {
        n = 2;
    }
    while (i + n < text.size() && IsCombiningMark(text[i + n])) {
        n++;
    }
    return n;
}

// Puts the underline on the first letter only, by splitting the text into
// Runs. Idempotent: a label already split the right way is left alone.
void ApplyFirstLetterUnderline(const Xaml::Controls::TextBlock& label) {
    winrt::hstring text = label.Text();
    std::wstring_view view(text.c_str(), text.size());

    size_t start = 0;
    size_t length = FirstLetterSpan(view, &start);
    if (length == 0) {
        return;
    }

    std::wstring_view head = view.substr(0, start);
    std::wstring_view letter = view.substr(start, length);
    std::wstring_view tail = view.substr(start + length);

    auto inlines = label.Inlines();
    if (!inlines) {
        return;
    }

    // Already in the wanted shape?
    uint32_t wanted = (head.empty() ? 0u : 1u) + 1u + (tail.empty() ? 0u : 1u);
    if (inlines.Size() == wanted) {
        bool matches = true;
        uint32_t index = 0;
        auto check = [&](std::wstring_view expected, bool underlined) {
            if (!matches) {
                return;
            }
            auto run = inlines.GetAt(index++).try_as<Xaml::Documents::Run>();
            if (!run || run.Text() != expected ||
                HasUnderlineBit(run.TextDecorations()) != underlined) {
                matches = false;
            }
        };
        if (!head.empty()) {
            check(head, false);
        }
        check(letter, true);
        if (!tail.empty()) {
            check(tail, false);
        }
        if (matches) {
            return;
        }
    }

    auto makeRun = [](std::wstring_view part, bool underlined) {
        Xaml::Documents::Run run;
        run.Text(winrt::hstring(part));
        if (underlined) {
            run.TextDecorations(TextDecorations::Underline);
        }
        return run;
    };

    g_rebuildingLabel = true;
    inlines.Clear();
    if (!head.empty()) {
        inlines.Append(makeRun(head, false));
    }
    inlines.Append(makeRun(letter, true));
    if (!tail.empty()) {
        inlines.Append(makeRun(tail, false));
    }
    g_rebuildingLabel = false;
    label.InvalidateMeasure();
}

// Sets or clears the underline on the label, and nothing else, so a theme's
// strikethrough survives.
//
// Full style: the TextBlock's Underline bit. First-letter style: the bit on
// a Run holding just the first letter. Clearing does not rely on the
// TextBlock property alone: XAML keeps drawing a decoration that was once
// applied even after the property reads back as None, because a "no
// decoration" value doesn't rebuild the text layout; so clearing also
// rebuilds the inline content from the text.
void SetLabelUnderline(const Xaml::Controls::TextBlock& label,
                       bool underline,
                       bool forceRebuild = false) {
    TextDecorations current = label.TextDecorations();

    bool anyRunUnderlined = false;
    uint32_t inlineCount = 0;
    try {
        auto inlines = label.Inlines();
        inlineCount = inlines ? inlines.Size() : 0;
        for (uint32_t i = 0; i < inlineCount; i++) {
            if (HasUnderlineBit(inlines.GetAt(i).TextDecorations())) {
                anyRunUnderlined = true;
                break;
            }
        }
    } catch (...) {
    }

    if (underline) {
        if (g_settings.underlineStyle == UnderlineStyle::kFirstLetter) {
            if (HasUnderlineBit(current)) {
                label.TextDecorations(WithUnderlineBit(current, false));
            }
            try {
                ApplyFirstLetterUnderline(label);
            } catch (...) {
            }
            return;
        }

        // Full style. A label split for the first-letter style is first put
        // back together.
        if (inlineCount > 1 || anyRunUnderlined) {
            RebuildLabelText(label);
        }
        if (!HasUnderlineBit(current)) {
            label.TextDecorations(WithUnderlineBit(current, true));
        }
        return;
    }

    if (!HasUnderlineBit(current) && !anyRunUnderlined && inlineCount <= 1 &&
        !forceRebuild) {
        return;
    }

    if (HasUnderlineBit(current)) {
        label.TextDecorations(WithUnderlineBit(current, false));
    }

    RebuildLabelText(label);
}

void ApplyUnderline(const Xaml::Controls::TextBlock& label, bool underline) {
    // A label seen for the first time may still carry a decoration drawn by
    // an earlier session that the property no longer reports; rebuild it
    // once when it is meant to be plain.
    bool firstSeen = EnsureLabelTracked(label);
    SetLabelUnderline(label, underline, /*forceRebuild=*/firstSeen);
}

// ----------------------------------------------------------------------------
// Icon underline
// ----------------------------------------------------------------------------
//
// An Image can't carry a text decoration, so the icon's line is a composition
// SpriteVisual attached to the Image element as its child visual. Its size and
// position are expressed relative to the element (RelativeSizeAdjustment /
// RelativeOffsetAdjustment), so it follows the icon through resizes and DPI
// changes without any layout code or SizeChanged handlers, and it never
// enters the taskbar's own layout. Rounded corners are a geometric clip whose
// geometry is bound to the icon's size by an expression. The visual, its
// brush and its clip are recognized by their Comment, so nothing but weak
// element references needs to be kept.

// Marks the visuals this mod created, so they can be told apart from a child
// visual another mod may have attached to the same element.
constexpr wchar_t kIconVisualComment[] = L"TaskbarOpenWindowUnderline";

// The taskbar follows the system theme (not the app theme), which is this
// registry value.
bool IsSystemLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

// Starts tracking the accent color, the first time it is needed. The event is
// raised on a worker thread; it only marks the cached colors dirty and asks
// the event thread for a refresh pass, which recomputes them on the taskbar
// thread.
void EnsureTrackingColors() {
    if (g_uiSettings) {
        return;
    }

    g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
    g_colorValuesChangedToken =
        g_uiSettings.ColorValuesChanged([](auto&&, auto&&) {
            g_iconColorsDirty = true;
            if (DWORD threadId = g_eventThreadId) {
                PostThreadMessage(threadId, WM_APP_COLORS_CHANGED, 0, 0);
            }
        });
}

void StopTrackingColors() {
    if (g_uiSettings) {
        g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
        g_uiSettings = nullptr;
    }
    g_iconColors.reset();
}

// The colors the line should use right now: the configured list for the
// current theme, or the accent color when none is configured. Cached; see
// g_iconColors.
const std::vector<winrt::Windows::UI::Color>& ResolveIconColors() {
    bool lightTheme = IsSystemLightTheme();
    if (g_iconColors && !g_iconColorsDirty.exchange(false) &&
        lightTheme == g_iconColorsLightTheme) {
        return *g_iconColors;
    }

    const IconUnderlineSettings& s = g_settings.iconUnderline;
    const auto& list = lightTheme ? s.lightColors : s.darkColors;
    if (!list.empty()) {
        g_iconColors = list;
    } else {
        using namespace winrt::Windows::UI::ViewManagement;
        EnsureTrackingColors();
        g_iconColors = std::vector<winrt::Windows::UI::Color>{
            g_uiSettings.GetColorValue(UIColorType::Accent)};
    }
    g_iconColorsLightTheme = lightTheme;
    return *g_iconColors;
}

// Gives `sprite` a brush for `colors`: a color brush for one, a horizontal
// linear gradient for several. The color list is recorded in the brush's
// Comment, so an unchanged list keeps the existing brush instead of
// building a new one on every evaluation.
void EnsureIconBrush(const Composition::SpriteVisual& sprite,
                     const std::vector<winrt::Windows::UI::Color>& colors) {
    std::wstring signature;
    for (const auto& c : colors) {
        wchar_t hex[12];
        swprintf_s(hex, L"%02X%02X%02X%02X,", c.A, c.R, c.G, c.B);
        signature += hex;
    }

    auto current = sprite.Brush();
    if (current && current.Comment() == signature) {
        return;
    }

    auto compositor = sprite.Compositor();
    Composition::CompositionBrush brush{nullptr};
    if (colors.size() == 1) {
        brush = compositor.CreateColorBrush(colors[0]);
    } else {
        auto gradient = compositor.CreateLinearGradientBrush();
        gradient.StartPoint({0.0f, 0.0f});
        gradient.EndPoint({1.0f, 0.0f});
        auto stops = gradient.ColorStops();
        for (size_t i = 0; i < colors.size(); i++) {
            float offset = static_cast<float>(i) / (colors.size() - 1);
            stops.Append(compositor.CreateColorGradientStop(offset, colors[i]));
        }
        brush = gradient;
    }
    brush.Comment(signature);
    sprite.Brush(brush);
}

// Returns this mod's underline visual for `icon`, creating it on first use.
// Null if the element already has a child visual that isn't ours.
Composition::SpriteVisual GetOrCreateIconVisual(
    const Xaml::Controls::Image& icon) {
    using Xaml::Hosting::ElementCompositionPreview;

    if (auto existing = ElementCompositionPreview::GetElementChildVisual(icon)) {
        if (existing.Comment() == kIconVisualComment) {
            return existing.try_as<Composition::SpriteVisual>();
        }
        LogOnce(L"Icon already has a child visual from elsewhere; "
                L"icon underline skipped for it");
        return nullptr;
    }

    auto compositor =
        ElementCompositionPreview::GetElementVisual(icon).Compositor();
    auto sprite = compositor.CreateSpriteVisual();
    sprite.Comment(kIconVisualComment);
    sprite.IsVisible(false);
    ElementCompositionPreview::SetElementChildVisual(icon, sprite);

    void* key = winrt::get_abi(icon);
    g_icons[key] = winrt::make_weak(icon);
    return sprite;
}

// Applies the current icon-underline settings and visibility to `sprite`.
// Rounds the line's corners with a geometric clip. The sprite's Size holds
// only the absolute part of its relative size, so the clip geometry can't
// copy it; instead an expression computes the line's real size from the icon
// element's visual, which the compositor re-evaluates whenever the icon is
// resized. Like the brush, the clip is rebuilt only when its parameters
// change, recorded in its Comment; radius 0 means no clip.
void EnsureIconClip(const Composition::SpriteVisual& sprite,
                    const Composition::Visual& iconVisual) {
    const IconUnderlineSettings& s = g_settings.iconUnderline;

    auto current = sprite.Clip();
    if (s.cornerRadius <= 0.0f) {
        if (current) {
            sprite.Clip(nullptr);
        }
        return;
    }

    wchar_t signature[64];
    swprintf_s(signature, L"%g,%g,%g", s.widthFraction, s.thickness,
               s.cornerRadius);
    if (current && current.Comment() == signature) {
        return;
    }

    auto compositor = sprite.Compositor();
    auto geometry = compositor.as<Composition::ICompositor5>()
                        .CreateRoundedRectangleGeometry();
    geometry.CornerRadius({s.cornerRadius, s.cornerRadius});

    auto size = compositor.CreateExpressionAnimation(
        L"Vector2(icon.Size.X * width, thickness)");
    size.SetReferenceParameter(L"icon", iconVisual);
    size.SetScalarParameter(L"width", s.widthFraction);
    size.SetScalarParameter(L"thickness", s.thickness);
    geometry.StartAnimation(L"Size", size);

    auto clip =
        compositor.as<Composition::ICompositor6>().CreateGeometricClip(geometry);
    clip.Comment(signature);
    sprite.Clip(clip);
}

// Applies the current icon-underline settings and visibility to `sprite`.
// Cheap enough to run on every evaluation (property sets only; the brush and
// the clip are rebuilt just when their parameters change), which also makes
// settings and theme changes take effect on the next pass without
// bookkeeping.
void ConfigureIconVisual(const Composition::SpriteVisual& sprite,
                         const Xaml::Controls::Image& icon,
                         bool visible) {
    const IconUnderlineSettings& s = g_settings.iconUnderline;

    // Width as a fraction of the icon, centered under it; thickness and the
    // gap below the icon in logical pixels.
    sprite.Size({0.0f, s.thickness});
    sprite.RelativeSizeAdjustment({s.widthFraction, 0.0f});
    sprite.Offset({0.0f, s.offset, 0.0f});
    sprite.RelativeOffsetAdjustment(
        {(1.0f - s.widthFraction) / 2.0f, 1.0f, 0.0f});
    sprite.Opacity(s.opacity);

    EnsureIconBrush(sprite, ResolveIconColors());
    EnsureIconClip(
        sprite, Xaml::Hosting::ElementCompositionPreview::GetElementVisual(icon));

    sprite.IsVisible(visible);
}

// Hides and detaches this mod's visual from `icon`, if it has one. Returns
// true if a visual of ours was found. Hiding comes first, so the line is gone
// in the same frame whatever happens to the detach; the visual holds no code
// of this mod, only compositor objects.
bool RemoveIconVisual(const Xaml::Controls::Image& icon) {
    using Xaml::Hosting::ElementCompositionPreview;

    auto existing = ElementCompositionPreview::GetElementChildVisual(icon);
    if (!existing || existing.Comment() != kIconVisualComment) {
        return false;
    }

    existing.IsVisible(false);

    try {
        ElementCompositionPreview::SetElementChildVisual(icon, nullptr);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Detaching the icon visual failed: %08X",
               static_cast<unsigned int>(e.code()));
    }
    return true;
}

// Hides and detaches every tracked icon visual (icon underline turned off,
// or unload).
void RemoveAllIconVisuals() {
    int tracked = static_cast<int>(g_icons.size());
    int removed = 0;
    int gone = 0;

    for (auto& [key, weakIcon] : g_icons) {
        try {
            if (auto icon = weakIcon.get()) {
                if (RemoveIconVisual(icon)) {
                    removed++;
                }
            } else {
                gone++;
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Icon visual removal error: %08X",
                   static_cast<unsigned int>(e.code()));
        } catch (...) {
            Wh_Log(L"Icon visual removal error");
        }
    }
    g_icons.clear();

    Wh_Log(L"Icon visuals: %d tracked, %d removed, %d elements gone", tracked,
           removed, gone);
}

// ----------------------------------------------------------------------------
// Running indicator
// ----------------------------------------------------------------------------
//
// The indicator is driven by the button's visual states (color, width,
// visibility per state), so its XAML properties aren't a safe place to hide
// it: the next state change would put them back, and a value left behind on
// unload could show an indicator that shouldn't be there. Its hand-off
// composition visual is safe: XAML never touches IsVisible on it, and setting
// it back to true hands control to XAML completely, whatever state the button
// is in by then.

void HideRunningIndicator(const Xaml::FrameworkElement& button) {
    auto indicator = FindRunningIndicator(button);
    if (!indicator) {
        LogOnce(L"Running indicator not found in a TaskListButton");
        return;
    }

    try {
        Xaml::Hosting::ElementCompositionPreview::GetElementVisual(indicator)
            .IsVisible(false);
        g_indicators[winrt::get_abi(indicator)] = winrt::make_weak(indicator);
    } catch (const winrt::hresult_error& e) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            Wh_Log(L"Running indicator error: %08X",
                   static_cast<unsigned int>(e.code()));
        }
    }
}

// Shows every hidden indicator again (option turned off, or unload).
void RestoreAllRunningIndicators() {
    int restored = 0;
    for (auto& [key, weakIndicator] : g_indicators) {
        try {
            if (auto indicator = weakIndicator.get()) {
                Xaml::Hosting::ElementCompositionPreview::GetElementVisual(
                    indicator)
                    .IsVisible(true);
                restored++;
            }
        } catch (...) {
            Wh_Log(L"Running indicator restore error");
        }
    }
    g_indicators.clear();
    Wh_Log(L"Running indicators restored: %d", restored);
}

void ApplyIconUnderline(const Xaml::FrameworkElement& button, bool underline) {
    auto icon = FindIconImage(button);
    if (!icon) {
        LogOnce(L"Icon image not found in a TaskListButton");
        return;
    }

    try {
        if (auto sprite = GetOrCreateIconVisual(icon)) {
            ConfigureIconVisual(sprite, icon, underline);
        }
    } catch (const winrt::hresult_error& e) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            Wh_Log(L"Icon underline error: %08X",
                   static_cast<unsigned int>(e.code()));
        }
    }
}

// ----------------------------------------------------------------------------
// Per-button evaluation
// ----------------------------------------------------------------------------

void ApplyToButton(const Xaml::FrameworkElement& button,
                   LazyWindowSnapshot& snapshot) {
    if (g_unloading) {
        return;
    }

    // Independent of the underline decision, and re-applied on every
    // evaluation, since the hooked state updates run right before this.
    if (g_settings.hideRunningIndicator) {
        HideRunningIndicator(button);
    }

    // The template may not be applied yet (fresh button) or the label may
    // have been collapsed/recreated (combine setting toggled); re-finding it
    // every time keeps us honest. With labels hidden there is no label at
    // all, and only the icon underline applies.
    auto label = FindLabelControl(button);
    bool wantIcon = g_settings.iconUnderline.enabled;

    if (!label && !wantIcon) {
        return;
    }

    ButtonWindows resolved = ResolveButtonWindows(button);

    // A recycled button isn't shown; leave it alone until the repeater reuses
    // it (its text changes then and its visual states are updated, both of
    // which re-evaluate it).
    if (resolved.state == ButtonWindowsState::kUnrealized) {
        return;
    }

    bool underline = false;
    if (resolved.state == ButtonWindowsState::kResolved) {
        underline = ShouldUnderline(resolved.windows);
    } else if (label) {
        // The title fallback needs the label text; without labels an
        // unresolved button stays plain.
        if (const auto* byTitle =
                ResolveWindowsForLabel(label, snapshot.get())) {
            underline = ShouldUnderline(*byTitle);
        }
    }

    // Logged once per outcome, since the first button evaluated can run
    // before the bridge is fully set up.
    static std::atomic<bool> loggedViewModel, loggedFallback;
    if (resolved.state == ButtonWindowsState::kResolved) {
        if (!loggedViewModel.exchange(true)) {
            Wh_Log(L"Resolver in use: view model");
        }
    } else if (!loggedFallback.exchange(true)) {
        Wh_Log(L"Resolver in use: title matching (fallback)");
    }

    if (label) {
        ApplyUnderline(label, underline);
    }
    if (wantIcon) {
        ApplyIconUnderline(button, underline);
    }
}

// ============================================================================
// Root registry and refresh passes (taskbar thread only)
// ============================================================================

// Registers the tree whose topmost element is `root`. Returns true if the
// tree is new.
bool RegisterRoot(const Xaml::DependencyObject& root) {
    void* key = winrt::get_abi(root);

    for (auto it = g_roots.begin(); it != g_roots.end();) {
        if (it->key == key) {
            auto existing = it->root.get();
            if (existing && existing == root) {
                return false;
            }

            // Dead object at a reused address.
            it = g_roots.erase(it);
            continue;
        }
        ++it;
    }

    g_roots.push_back(RootEntry{key, winrt::make_weak(root)});
    Wh_Log(L"Registered taskbar tree root: %s",
           winrt::get_class_name(root).c_str());

    // The first root makes the event hooks worthwhile. By now the view-model
    // symbols are resolved too, so the NAMECHANGE decision in HookWinEvents
    // is accurate. If the event thread hasn't published its id yet, it will
    // see the flag itself when it registers its hooks.
    if (!g_treeKnown.exchange(true)) {
        if (DWORD threadId = g_eventThreadId) {
            PostThreadMessage(threadId, WM_APP_RECONFIGURE_HOOKS, 0, 0);
        }
    }
    return true;
}

// Registers the tree containing `element`. Returns true if the tree is new.
bool RegisterRootOf(const Xaml::DependencyObject& element, bool allowSelf) {
    auto root = FindTreeRoot(element, allowSelf);
    if (!root) {
        return false;
    }
    return RegisterRoot(root);
}

// Re-evaluates every button in every known tree and prunes entries whose
// objects are gone.
void RefreshTreesOnCurrentThread() {
    if (g_unloading) {
        return;
    }

    LazyWindowSnapshot snapshot;

    for (auto it = g_roots.begin(); it != g_roots.end();) {
        auto root = it->root.get();
        if (!root) {
            it = g_roots.erase(it);
            continue;
        }

        try {
            std::vector<Xaml::FrameworkElement> buttons;
            CollectTaskListButtons(root, buttons);
            for (const auto& button : buttons) {
                ApplyToButton(button, snapshot);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Refresh error: %08X", static_cast<unsigned int>(e.code()));
        } catch (...) {
            Wh_Log(L"Refresh error");
        }

        ++it;
    }

    for (auto it = g_labels.begin(); it != g_labels.end();) {
        if (!it->second.label.get()) {
            it = g_labels.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = g_icons.begin(); it != g_icons.end();) {
        if (!it->second.get()) {
            it = g_icons.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = g_indicators.begin(); it != g_indicators.end();) {
        if (!it->second.get()) {
            it = g_indicators.erase(it);
        } else {
            ++it;
        }
    }
}

void WINAPI RefreshTreesProc(void*) {
    RefreshTreesOnCurrentThread();
}

// Undoes everything on the taskbar thread while unloading: detaches the icon
// visuals, stops the color tracking, shows the running indicators again,
// removes every label's subscription and decoration, and forgets all trees.
// Runs synchronously, so no delegate implemented by the mod stays registered
// afterwards.
void WINAPI RestoreAndClearProc(void*) {
    RemoveAllIconVisuals();
    StopTrackingColors();
    RestoreAllRunningIndicators();

    for (auto& [key, entry] : g_labels) {
        try {
            if (auto label = entry.label.get()) {
                label.UnregisterPropertyChangedCallback(
                    Xaml::Controls::TextBlock::TextProperty(),
                    entry.textChangedToken);
                SetLabelUnderline(label, false, /*forceRebuild=*/true);
            }
        } catch (...) {
            Wh_Log(L"Restore error");
        }
    }

    g_labels.clear();
    g_roots.clear();
}

// ============================================================================
// Finding the taskbars' XAML trees (taskbar.dll)
// ============================================================================
//
// Buttons that existed before the mod loaded are only seen once one of the
// hooked TaskListButton members runs for them. Instead of waiting for that,
// the taskbar's XamlRoot is read directly from its TaskbarHost, the way the
// Taskbar Multirow mod does, and the whole tree is evaluated at once.

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

bool AreTaskbarDllSymbolsAvailable() {
    return CTaskBand_ITaskListWndSite_vftable &&
           CSecondaryTaskBand_ITaskListWndSite_vftable &&
           CTaskBand_GetTaskbarHost_Original && TaskbarHost_FrameHeight_Original &&
           CSecondaryTaskBand_GetTaskbarHost_Original &&
           std__Ref_count_base__Decref_Original;
}

Xaml::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    Xaml::FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<Xaml::FrameworkElement>(),
        winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

Xaml::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

Xaml::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// Registers the XAML tree of every taskbar window on the calling (taskbar)
// thread, then evaluates all their buttons.
void WINAPI RegisterTaskbarTreesProc(void*) {
    if (g_unloading || !AreTaskbarDllSymbolsAvailable()) {
        return;
    }

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            Xaml::XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            try {
                if (auto content = xamlRoot.Content()) {
                    RegisterRootOf(content, /*allowSelf=*/true);
                }
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Registering a taskbar tree failed: %08X",
                       static_cast<unsigned int>(e.code()));
            }

            return TRUE;
        },
        0);

    RefreshTreesOnCurrentThread();
}

// ============================================================================
// WinEvent thread: invalidation signals, coalesced into refresh passes
// ============================================================================

HANDLE g_eventThread;
HANDLE g_eventThreadReady;

// Only touched on the event thread.
UINT_PTR g_refreshTimer;
UINT_PTR g_followUpTimer;
std::vector<HWINEVENTHOOK> g_winEventHooks;

void CALLBACK RefreshTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);

    if (id == g_refreshTimer) {
        g_refreshTimer = 0;
        RunOnTaskbarThread(RefreshTreesProc, nullptr);

        // Second pass a bit later: minimize/restore state can still be
        // changing during the first one.
        if (!g_followUpTimer) {
            g_followUpTimer = SetTimer(nullptr, 0, FOLLOW_UP_DELAY_MS,
                                       RefreshTimerProc);
        }
    } else if (id == g_followUpTimer) {
        g_followUpTimer = 0;
        RunOnTaskbarThread(RefreshTreesProc, nullptr);
    }
}

void ArmRefreshTimer() {
    if (!g_refreshTimer) {
        g_refreshTimer = SetTimer(nullptr, 0, g_settings.refreshDelayMs,
                                  RefreshTimerProc);
    }
}

// Keeps only events that can change a taskbar button: top-level, unowned
// windows that aren't tool windows. SHOW and HIDE fire for every top-level
// window in every process, including tooltips, menus and the taskbar's own
// thumbnail previews, and each pass costs a walk over every button. Styles
// and the owner can still be read after a hide (and after a destroy, which
// a hide of a closing window usually precedes).
void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) {
        return;
    }
    if (GetWindow(hwnd, GW_OWNER) && !(exStyle & WS_EX_APPWINDOW)) {
        return;
    }

    ArmRefreshTimer();
}

void UnhookWinEvents() {
    for (HWINEVENTHOOK hook : g_winEventHooks) {
        UnhookWinEvent(hook);
    }
    g_winEventHooks.clear();
}

// Registers the WinEvent ranges the current settings need. Minimize,
// restore, show, hide and cloaking always; foreground changes only when the
// foreground window is treated specially; title changes only when the title
// fallback is in use (they are frequent, and with the view model a title
// change doesn't change which windows a button has).
void HookWinEvents() {
    UnhookWinEvents();

    // No tree yet: either Taskbar.View.dll hasn't loaded (sign-in), or this
    // explorer.exe has no taskbar at all (e.g. "Launch folder windows in a
    // separate process"). RegisterRoot asks for a re-run once a tree exists.
    if (!g_treeKnown) {
        return;
    }

    struct Range {
        DWORD min;
        DWORD max;
    };
    std::vector<Range> ranges = {
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    if (!g_settings.includeActive && g_settings.mode == Mode::kOpen) {
        ranges.push_back({EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND});
    }
    if (!IsViewModelBridgeAvailable()) {
        ranges.push_back({EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE});
    }

    for (const Range& range : ranges) {
        HWINEVENTHOOK hook = SetWinEventHook(range.min, range.max, nullptr,
                                             WinEventProc, 0, 0,
                                             WINEVENT_OUTOFCONTEXT);
        if (hook) {
            g_winEventHooks.push_back(hook);
        } else {
            Wh_Log(L"SetWinEventHook failed for range %04X-%04X", range.min,
                   range.max);
        }
    }
}

DWORD WINAPI EventThreadProc(LPVOID) {
    // Force the message queue into existence before signaling readiness, so
    // PostThreadMessage from other threads can't be lost.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_eventThreadId = GetCurrentThreadId();
    SetEvent(g_eventThreadReady);

    // Registers nothing until a tree is known; see HookWinEvents.
    HookWinEvents();

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_RECONFIGURE_HOOKS) {
            HookWinEvents();
            continue;
        }
        if (!msg.hwnd && msg.message == WM_APP_COLORS_CHANGED) {
            ArmRefreshTimer();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_refreshTimer) {
        KillTimer(nullptr, g_refreshTimer);
        g_refreshTimer = 0;
    }
    if (g_followUpTimer) {
        KillTimer(nullptr, g_followUpTimer);
        g_followUpTimer = 0;
    }

    UnhookWinEvents();
    return 0;
}

void StartEventThread() {
    if (g_eventThread) {
        return;
    }

    g_eventThreadReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_eventThreadReady) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return;
    }

    g_eventThread =
        CreateThread(nullptr, 0, EventThreadProc, nullptr, 0, nullptr);
    if (!g_eventThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_eventThreadReady);
        g_eventThreadReady = nullptr;
    }
}

// Nothing in the event thread blocks for long (its only cross-thread call is
// a SendMessage to the taskbar thread), so the waits here are unbounded: the
// thread runs code of this DLL and must be gone before it is unmapped.
void StopEventThread() {
    if (!g_eventThread) {
        return;
    }

    WaitForSingleObject(g_eventThreadReady, INFINITE);
    PostThreadMessage(g_eventThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_eventThread, INFINITE);

    CloseHandle(g_eventThread);
    g_eventThread = nullptr;
    g_eventThreadId = 0;

    CloseHandle(g_eventThreadReady);
    g_eventThreadReady = nullptr;
}

// ============================================================================
// Taskbar.View.dll hooks
// ============================================================================

std::atomic<bool> g_elementVerified;
std::atomic<bool> g_elementBroken;

// Gets the XAML element from the TaskListButton implementation object. The
// IInspectable of the composed XAML control sits at this offset in the
// C++/WinRT implementation layout; the same technique is used by the
// Taskbar Labels and Taskbar Icon Size mods. The class name is checked once
// so that a build where the offset points at some other XAML element is
// noticed instead of silently decorating the wrong thing.
Xaml::FrameworkElement TaskListButtonFromThis(void* pThis) {
    if (g_elementBroken) {
        return nullptr;
    }

    winrt::Windows::Foundation::IInspectable inspectable = nullptr;
    winrt::copy_from_abi(inspectable,
                         static_cast<void*>(static_cast<void**>(pThis) + 3));

    auto element = inspectable.try_as<Xaml::FrameworkElement>();
    if (!element) {
        return nullptr;
    }

    if (!g_elementVerified.exchange(true)) {
        winrt::hstring className = winrt::get_class_name(element);
        Wh_Log(L"TaskListButton element class: %s", className.c_str());
        if (className != L"Taskbar.TaskListButton") {
            Wh_Log(L"Unexpected element class, disabling the mod's effect");
            g_elementBroken = true;
            return nullptr;
        }
    }

    return element;
}

void OnTaskListButtonUpdated(void* pThis) {
    if (g_unloading) {
        return;
    }

    try {
        auto element = TaskListButtonFromThis(pThis);
        if (!element) {
            return;
        }

        if (RegisterRootOf(element, /*allowSelf=*/false)) {
            // First time we see this tree (a taskbar created after the mod
            // loaded, say): decorate everything in it.
            RefreshTreesOnCurrentThread();
        } else {
            LazyWindowSnapshot snapshot;
            ApplyToButton(element, snapshot);
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"TaskListButton hook error: %08X",
               static_cast<unsigned int>(e.code()));
    } catch (...) {
        Wh_Log(L"TaskListButton hook error");
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_UpdateIconColumnDefinition_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateIconColumnDefinition_t
    TaskListButton_UpdateIconColumnDefinition_Original;
void WINAPI TaskListButton_UpdateIconColumnDefinition_Hook(void* pThis) {
    TaskListButton_UpdateIconColumnDefinition_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        // The two below are optional: they run for every button on
        // layout/settings changes, which keeps labels right across those.
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
            &TaskListButton_UpdateIconColumnDefinition_Original,
            TaskListButton_UpdateIconColumnDefinition_Hook,
            true,
        },
        // View-model bridge (addresses only, nothing is hooked). All
        // optional: without them the title-matching fallback is used.
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskListWindowViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &ITaskListWindowViewModel_get_TaskItem,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_TaskGroup(void * *))"},
            &TaskListGroupViewModel_get_TaskGroup,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskListGroupViewModel_vftable,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for the taskbar view module");
        return false;
    }

    Wh_Log(L"Optional hooks: UpdateButtonPadding=%s UpdateIconColumnDefinition=%s",
           TaskListButton_UpdateButtonPadding_Original ? L"yes" : L"no",
           TaskListButton_UpdateIconColumnDefinition_Original ? L"yes" : L"no");
    Wh_Log(L"View-model bridge: %s",
           IsViewModelBridgeAvailable() ? L"available"
                                        : L"unavailable, using title matching");

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // Addresses only, nothing is hooked. All optional: without them buttons
    // that existed before the mod loaded are decorated as they refresh.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    Wh_Log(L"Taskbar XAML root lookup: %s",
           AreTaskbarDllSymbolsAvailable() ? L"available" : L"unavailable");

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar view module loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

// ============================================================================
// Mod entry points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // No hook is applied yet, so this read of the settings can't race the
    // taskbar thread; later reloads run on that thread.
    LoadSettings();

    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(module)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, hooking LoadLibraryExW");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    HookTaskbarDllSymbols();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The module may have been loaded between Wh_ModInit and the hooks being
    // applied; catch that case.
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(module)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Existing taskbars: read their XAML trees and evaluate every button.
    // A taskbar that appears later is picked up by the hooks.
    if (g_taskbarViewDllLoaded) {
        RunOnTaskbarThread(RegisterTaskbarTreesProc, nullptr);
    }

    StartEventThread();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Stop the invalidation source first, then undo everything on the
    // taskbar thread synchronously: after this returns, no delegate of the
    // mod is registered anywhere and no refresh is pending.
    StopEventThread();
    RunOnTaskbarThread(RestoreAndClearProc, nullptr);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void WINAPI ReloadSettingsProc(void*) {
    LoadSettings();
    g_iconColors.reset();
    if (!g_settings.iconUnderline.enabled) {
        RemoveAllIconVisuals();
        StopTrackingColors();
    }
    if (!g_settings.hideRunningIndicator) {
        RestoreAllRunningIndicators();
    }
    RefreshTreesOnCurrentThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Settings are read on the taskbar thread, so reload them there.
    if (!RunOnTaskbarThread(ReloadSettingsProc, nullptr)) {
        LoadSettings();
    }

    if (DWORD threadId = g_eventThreadId) {
        PostThreadMessage(threadId, WM_APP_RECONFIGURE_HOOKS, 0, 0);
    }
}
