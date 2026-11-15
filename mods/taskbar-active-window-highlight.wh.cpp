// ==WindhawkMod==
// @id              taskbar-active-window-highlight
// @name            Taskbar Active Window Highlight
// @description     Highlights the active window's card in the taskbar thumbnail preview
// @version         0.1.1
// @author          Ephrem T.
// @github          https://github.com/EphremTil17
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
// Repository, diagnostics and tooling:
// https://github.com/EphremTil17/taskbar-active-window-highlight

// ==WindhawkModReadme==
/*
# Taskbar Active Window Highlight

When an app has several windows, Windows 11's thumbnail preview gives no hint
about which one is active. This mod adds an accent-colored highlight to the
active window's card. The taskbar button keeps Windows' native indicator.

Requires the Windows 11 taskbar's XAML thumbnail previews (tested on Windows 11
build 26300). With the classic thumbnails, or with StartAllBack or
ExplorerPatcher, the mod has no effect.

![Active window highlighted in the thumbnail preview](https://raw.githubusercontent.com/EphremTil17/taskbar-active-window-highlight/main/screenshots/preview.png)

## How it works

The mod does not track focus itself. It reads the state Explorer already keeps:

- The taskbar's active task item (`TaskbarAppsList.ActiveItem`), which names
  the active window. The mod follows Explorer's own change notifications.
- Each thumbnail card's view model, which names the window the card shows.

The highlight is a separate element placed right above the card's own
background, below the title, icon and preview image. Its size and corner
radius are bound to that background, so it follows themes such as
Windows 11 Taskbar Styler.

Clicking the taskbar or a preview list briefly moves focus to the taskbar
itself. By default the last active window stays highlighted until another
window or the desktop takes focus, or the window is minimized or closed.

## How it differs from Taskbar Recent Focus Highlight

[Taskbar Recent Focus Highlight](https://windhawk.net/mods/taskbar-recent-focus-highlight)
ranks apps and windows by recent use, from its own focus history. This mod
answers a narrower question, which window is active right now, by reading
Explorer's own active task item, with no focus tracking of its own. While
another app has focus it marks nothing in this app's preview, and it also
marks single-window previews.

## Performance and footprint

- **Event-driven only.** No background thread, timers, polling or focus
  tracking. The mod runs only when Explorer reports a new active window or
  binds a preview card to a window.
- **Nothing on hover or layout.** Moving the mouse over the taskbar or
  previews runs no mod code.
- **Under 1 ms, usually about 0.01 ms.** Measured in normal use: typical
  0.004–0.013 ms per event, worst case 0.67 ms (the first time a card's
  highlight is created).
- **One element per card**, created once and then only shown or hidden.
- **Safe to unload.** No event handlers are left in Explorer, so disabling the
  mod cleanly removes it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- color: ""
  $name: Highlight color
  $description: >-
    Leave empty to follow the Windows accent color. Otherwise enter a hex color
    such as #3A96DD or #FF3A96DD (alpha first).
- fillOpacity: 20
  $name: Fill opacity (0-100)
- borderThickness: 2
  $name: Outline thickness (px, 0 for none)
- keepWhileTaskbarFocused: true
  $name: Keep the highlight while using the taskbar
  $description: >-
    Clicking the taskbar or a preview list briefly takes focus away from your
    window. Keep showing it as active until another window or the desktop
    takes focus, or the window is minimized or closed.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <inspectable.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// ---------------------------------------------------------------------------
// Explorer interfaces
// ---------------------------------------------------------------------------
// WindowsUdk.UI.Shell interfaces are declared in the system's
// windowsudk.winmd; their IIDs and method order come from there. The
// Taskbar.* IIDs are the guid_v constants from Taskbar.View.dll's public
// symbols. If a future build changes an interface, its IID changes too, the
// QueryInterface fails, and no highlight is shown.

constexpr GUID IID_ITaskItemThumbnailView = {
    0xF093D5DE, 0xB516, 0x55A2, {0xB1, 0x37, 0x4E, 0x12, 0x5E, 0x37, 0xCA, 0xF4}};
constexpr GUID IID_ITaskItemThumbnailViewModel = {
    0xA5D30F97, 0x5BC4, 0x58AE, {0xB4, 0x48, 0xB1, 0x54, 0x8A, 0x1A, 0x92, 0x9D}};
constexpr GUID IID_ITaskItemThumbnail = {
    0xD7DC08AE, 0x776E, 0x5871, {0xBC, 0xEE, 0x25, 0x0D, 0xBC, 0x94, 0xD0, 0x37}};
constexpr GUID IID_ITaskbarAppsList = {
    0xABD50A67, 0x1E54, 0x536C, {0x82, 0x27, 0xB4, 0xF1, 0xA1, 0x25, 0xA7, 0xFE}};

// Leading members only; the declared order matches windowsudk.winmd.
// ITaskItem (B081D9D6-9B45-5363-8A4D-854ADD6ABE7E) is only received as a
// typed return value, so it is never queried.
struct ITaskItemAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId(UINT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(UINT64* value) = 0;
};

struct ITaskItemThumbnailAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ParentItem(ITaskItemAbi** value) = 0;
};

struct ITaskbarAppsListAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Groups(IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ActiveGroup(IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ActiveItem(ITaskItemAbi** value) = 0;
};

// Interface getters of Taskbar.View.dll types, resolved from symbols. Each is
// called with the interface pointer obtained by QueryInterface for that
// interface's IID.
using GetObject_t = int32_t(WINAPI*)(void* pThis, void** value);

GetObject_t TaskItemThumbnailView_get_ThumbnailSource;
GetObject_t TaskItemThumbnailViewModel_get_TaskItemThumbnail;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    std::wstring color;  // "#AARRGGBB", or empty for the accent color
    int fillOpacity = 20;
    int borderThickness = 2;
    bool keepWhileTaskbarFocused = true;
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> CurrentSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring NormalizeHexColor(std::wstring text) {
    std::wstring hex;
    for (wchar_t c : text) {
        if (c == L'#' || iswspace(c)) {
            continue;
        }
        if (!iswxdigit(c)) {
            return L"";
        }
        hex += static_cast<wchar_t>(towupper(c));
    }
    if (hex.size() == 6) {
        hex = L"FF" + hex;
    }
    return hex.size() == 8 ? L"#" + hex : L"";
}

void LoadSettings() {
    auto s = std::make_shared<Settings>();
    auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); };

    WindhawkUtils::StringSetting color = WindhawkUtils::StringSetting::make(L"color");
    s->color = NormalizeHexColor(color.get());
    s->fillOpacity = clamp(Wh_GetIntSetting(L"fillOpacity"), 0, 100);
    s->borderThickness = clamp(Wh_GetIntSetting(L"borderThickness"), 0, 8);
    s->keepWhileTaskbarFocused = Wh_GetIntSetting(L"keepWhileTaskbarFocused") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
// Hooks run on the taskbar's UI thread. The mutex covers the hand-off from
// Windhawk's thread on settings changes and unload. (Windhawk removes the
// hooks before Wh_ModUninit runs, so no hook runs during teardown.)

std::mutex g_stateMutex;

// Active window per Explorer's active task item, and which apps list
// reported it (each taskbar has its own list).
HWND g_activeWindow = nullptr;
void* g_activeSource = nullptr;
bool g_haveActiveState = false;

struct OverlayEntry {
    winrt::weak_ref<Controls::Border> overlay;
    DWORD threadId;
};
std::vector<OverlayEntry> g_overlays;
std::vector<winrt::weak_ref<FrameworkElement>> g_cards;
std::set<DWORD> g_uiThreads;

void NoteUiThread() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_uiThreads.insert(GetCurrentThreadId());
}

// ---------------------------------------------------------------------------
// The highlight element
// ---------------------------------------------------------------------------

constexpr wchar_t kOverlayName[] = L"WhActiveWindowHighlight";
constexpr wchar_t kCardBackgroundName[] = L"BackgroundBorder";

FrameworkElement FindChildByName(Controls::Panel const& panel, PCWSTR name) {
    for (auto const& child : panel.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name) {
            return element;
        }
    }
    return nullptr;
}

Controls::Panel TemplateRootPanel(FrameworkElement const& element) {
    if (Media::VisualTreeHelper::GetChildrenCount(element) < 1) {
        return nullptr;
    }
    return Media::VisualTreeHelper::GetChild(element, 0).try_as<Controls::Panel>();
}

void BindTo(FrameworkElement const& target,
            DependencyObject const& source,
            DependencyProperty const& property,
            PCWSTR path) {
    Data::Binding binding;
    binding.Source(source);
    binding.Path(PropertyPath(path));
    binding.Mode(Data::BindingMode::OneWay);
    target.SetBinding(property, binding);
}

Controls::Border CreateOverlayElement(const Settings& s) {
    // A theme resource keeps the accent color live when the user changes it.
    const std::wstring color =
        s.color.empty() ? L"{ThemeResource SystemAccentColor}" : s.color;
    wchar_t opacity[16];
    swprintf_s(opacity, L"%d.%02d", s.fillOpacity / 100, s.fillOpacity % 100);
    std::wstring xaml =
        L"<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"Name=\"" + std::wstring(kOverlayName) + L"\" IsHitTestVisible=\"False\" "
        L"BorderThickness=\"" + std::to_wstring(s.borderThickness) + L"\">"
        L"<Border.Background><SolidColorBrush Color=\"" + color + L"\" Opacity=\"" +
        opacity + L"\"/></Border.Background>"
        L"<Border.BorderBrush><SolidColorBrush Color=\"" + color +
        L"\"/></Border.BorderBrush></Border>";
    return Markup::XamlReader::Load(xaml).as<Controls::Border>();
}

// Inserts the overlay directly above the card's own background plate and
// binds its geometry to it.
Controls::Border CreateOverlay(Controls::Panel const& panel,
                               FrameworkElement const& background) {
    auto overlay = CreateOverlayElement(*CurrentSettings());

    Controls::Grid::SetRow(overlay, Controls::Grid::GetRow(background));
    Controls::Grid::SetColumn(overlay, Controls::Grid::GetColumn(background));
    Controls::Grid::SetRowSpan(overlay, Controls::Grid::GetRowSpan(background));
    Controls::Grid::SetColumnSpan(overlay, Controls::Grid::GetColumnSpan(background));
    BindTo(overlay, background, FrameworkElement::WidthProperty(), L"Width");
    BindTo(overlay, background, FrameworkElement::HeightProperty(), L"Height");
    BindTo(overlay, background, FrameworkElement::MarginProperty(), L"Margin");
    BindTo(overlay, background, FrameworkElement::HorizontalAlignmentProperty(),
           L"HorizontalAlignment");
    BindTo(overlay, background, FrameworkElement::VerticalAlignmentProperty(),
           L"VerticalAlignment");
    if (background.try_as<Controls::Border>()) {
        BindTo(overlay, background, Controls::Border::CornerRadiusProperty(),
               L"CornerRadius");
    }

    auto children = panel.Children();
    uint32_t index = 0;
    const bool found = children.IndexOf(background, index);
    // Explicit insertion index; never Append into Explorer's panels.
    children.InsertAt(found ? index + 1 : 0, overlay);

    std::lock_guard<std::mutex> lock(g_stateMutex);
    std::erase_if(g_overlays, [](const OverlayEntry& e) { return !e.overlay.get(); });
    g_overlays.push_back({winrt::make_weak(overlay), GetCurrentThreadId()});
    return overlay;
}

void SetHighlight(FrameworkElement const& card, bool show) {
    auto panel = TemplateRootPanel(card);
    if (!panel) {
        return;  // Template not applied yet; OnApplyTemplate comes later.
    }
    auto overlay = FindChildByName(panel, kOverlayName);
    if (!show) {
        if (overlay && overlay.Visibility() != Visibility::Collapsed) {
            overlay.Visibility(Visibility::Collapsed);
        }
        return;
    }
    if (!overlay) {
        auto background = FindChildByName(panel, kCardBackgroundName);
        if (!background) {
            return;  // Unknown template: leave it untouched.
        }
        overlay = CreateOverlay(panel, background);
    }
    if (overlay.Visibility() != Visibility::Visible) {
        overlay.Visibility(Visibility::Visible);
    }
}

// ---------------------------------------------------------------------------
// Cards and the active window
// ---------------------------------------------------------------------------

// The window a thumbnail card shows, via the card's view model:
// card -> ThumbnailSource -> TaskItemThumbnail -> ParentItem -> WindowId.
HWND CardWindow(FrameworkElement const& card) {
    auto* unknown = static_cast<IUnknown*>(winrt::get_abi(card));
    winrt::com_ptr<IUnknown> view;
    if (FAILED(unknown->QueryInterface(IID_ITaskItemThumbnailView, view.put_void()))) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> source;
    if (TaskItemThumbnailView_get_ThumbnailSource(view.get(), source.put_void()) < 0 ||
        !source) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> viewModel;
    if (FAILED(source->QueryInterface(IID_ITaskItemThumbnailViewModel,
                                      viewModel.put_void()))) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> thumbnailObject;
    if (TaskItemThumbnailViewModel_get_TaskItemThumbnail(
            viewModel.get(), thumbnailObject.put_void()) < 0 ||
        !thumbnailObject) {
        return nullptr;
    }
    winrt::com_ptr<ITaskItemThumbnailAbi> thumbnail;
    if (FAILED(thumbnailObject->QueryInterface(IID_ITaskItemThumbnail,
                                               thumbnail.put_void()))) {
        return nullptr;
    }
    winrt::com_ptr<ITaskItemAbi> item;
    UINT64 windowId = 0;
    if (FAILED(thumbnail->get_ParentItem(item.put())) || !item ||
        FAILED(item->get_WindowId(&windowId))) {
        return nullptr;
    }
    return reinterpret_cast<HWND>(windowId);
}

bool IsTaskbarUiWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }
    wchar_t cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           _wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
           _wcsicmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0;
}

// A minimized or closed window is not active, even while it is the window
// kept for taskbar focus. Checked at paint time too, because minimizing or
// closing a kept window (from the preview's context menu, or after Explorer's
// event) does not always produce another active-item change.
bool CanBeActive(HWND hwnd) {
    return hwnd && IsWindow(hwnd) && !IsIconic(hwnd);
}

HWND ActiveWindow() {
    HWND window = nullptr;
    bool haveState = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        haveState = g_haveActiveState;
        window = g_activeWindow;
    }
    if (!haveState) {
        // Before Explorer's first active-item change after the mod loaded,
        // its active item is the foreground window, unless the taskbar has
        // focus.
        window = GetForegroundWindow();
        if (window && IsTaskbarUiWindow(window)) {
            window = nullptr;
        }
    }
    return CanBeActive(window) ? window : nullptr;
}

void UpdateCard(FrameworkElement const& card) {
    HWND window = CardWindow(card);
    SetHighlight(card, window && window == ActiveWindow());
}

void RegisterCard(FrameworkElement const& card) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    bool known = false;
    std::erase_if(g_cards, [&](const winrt::weak_ref<FrameworkElement>& weak) {
        auto element = weak.get();
        known = known || element == card;
        return !element;
    });
    if (!known) {
        g_cards.push_back(winrt::make_weak(card));
    }
}

std::vector<FrameworkElement> LiveCardsOnThisThread() {
    std::vector<winrt::weak_ref<FrameworkElement>> weaks;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        weaks = g_cards;
    }
    std::vector<FrameworkElement> cards;
    for (auto& weak : weaks) {
        if (auto card = weak.get(); card && card.Dispatcher().HasThreadAccess()) {
            cards.push_back(card);
        }
    }
    return cards;
}

void OnActiveItemChanged(IUnknown* appsListAbi) {
    winrt::com_ptr<ITaskbarAppsListAbi> appsList;
    if (!appsListAbi || FAILED(appsListAbi->QueryInterface(IID_ITaskbarAppsList,
                                                           appsList.put_void()))) {
        return;
    }
    winrt::com_ptr<ITaskItemAbi> item;
    UINT64 windowId = 0;
    if (FAILED(appsList->get_ActiveItem(item.put())) ||
        (item && FAILED(item->get_WindowId(&windowId)))) {
        return;
    }
    HWND window = reinterpret_cast<HWND>(windowId);

    // Explorer reports no active item while the taskbar itself has focus.
    bool taskbarFocused = false;
    if (!window && CurrentSettings()->keepWhileTaskbarFocused) {
        HWND fg = GetForegroundWindow();
        // No foreground window is a transition; the next change resolves it.
        taskbarFocused = !fg || IsTaskbarUiWindow(fg);
    }

    bool changed = false;
    HWND activeNow = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        const HWND previous = g_activeWindow;
        const bool hadState = g_haveActiveState;
        g_haveActiveState = true;
        if (window) {
            g_activeWindow = window;
            g_activeSource = appsListAbi;
        } else if (!g_activeSource || g_activeSource == appsListAbi) {
            // Only the list that reported the active window clears it; another
            // taskbar's list being empty says nothing about it. While the
            // taskbar has focus, keep the last window unless it was minimized
            // (for example by clicking its taskbar button) or closed.
            if (!taskbarFocused || !CanBeActive(g_activeWindow)) {
                g_activeWindow = nullptr;
                g_activeSource = nullptr;
            }
        }
        changed = !hadState || previous != g_activeWindow;
        activeNow = g_activeWindow;
    }
    if (!changed) {
        return;
    }
    Wh_Log(L"Active window %p", activeNow);
    for (auto& card : LiveCardsOnThisThread()) {
        UpdateCard(card);
    }
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

void OnCardChanged(void* pThis) {
    try {
        FrameworkElement card = nullptr;
        static_cast<IUnknown*>(pThis)->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(card));
        if (card) {
            NoteUiThread();
            RegisterCard(card);
            UpdateCard(card);
        }
    } catch (...) {
        Wh_Log(L"Thumbnail hook error %08X", winrt::to_hresult());
    }
}

// The card's template is ready (or re-applied).
using TaskItemThumbnailView_OnApplyTemplate_t = int32_t(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t TaskItemThumbnailView_OnApplyTemplate_Original;
int32_t WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    int32_t result = TaskItemThumbnailView_OnApplyTemplate_Original(pThis);
    OnCardChanged(pThis);
    return result;
}

// The card is bound to a (possibly different) window.
using TaskItemThumbnailView_put_ThumbnailSource_t = int32_t(WINAPI*)(void* pThis,
                                                                     void* source);
TaskItemThumbnailView_put_ThumbnailSource_t
    TaskItemThumbnailView_put_ThumbnailSource_Original;
int32_t WINAPI TaskItemThumbnailView_put_ThumbnailSource_Hook(void* pThis,
                                                              void* source) {
    int32_t result = TaskItemThumbnailView_put_ThumbnailSource_Original(pThis, source);
    OnCardChanged(pThis);
    return result;
}

// Explorer's handler for "the active task item changed".
using TaskbarController_OnAppsListActiveItemChanged_t = void(WINAPI*)(void* pThis,
                                                                      void* appsList,
                                                                      void* args);
TaskbarController_OnAppsListActiveItemChanged_t
    TaskbarController_OnAppsListActiveItemChanged_Original;
void WINAPI TaskbarController_OnAppsListActiveItemChanged_Hook(void* pThis,
                                                               void* appsList,
                                                               void* args) {
    TaskbarController_OnAppsListActiveItemChanged_Original(pThis, appsList, args);
    if (appsList) {
        try {
            NoteUiThread();
            // `TaskbarAppsList const&` refers to a projected object, which
            // holds a single pointer: its default interface. Verified by QI.
            OnActiveItemChanged(*static_cast<IUnknown**>(appsList));
        } catch (...) {
            Wh_Log(L"Active item hook error %08X", winrt::to_hresult());
        }
    }
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnAppsListActiveItemChanged(struct winrt::WindowsUdk::UI::Shell::TaskbarAppsList const &,struct winrt::Windows::Foundation::IInspectable const &))"},
         &TaskbarController_OnAppsListActiveItemChanged_Original,
         TaskbarController_OnAppsListActiveItemChanged_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailView,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
         &TaskItemThumbnailView_OnApplyTemplate_Original,
         TaskItemThumbnailView_OnApplyTemplate_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailView,struct winrt::Taskbar::ITaskItemThumbnailView>::put_ThumbnailSource(void *))"},
         &TaskItemThumbnailView_put_ThumbnailSource_Original,
         TaskItemThumbnailView_put_ThumbnailSource_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailView,struct winrt::Taskbar::ITaskItemThumbnailView>::get_ThumbnailSource(void * *))"},
         &TaskItemThumbnailView_get_ThumbnailSource},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailViewModel,struct winrt::Taskbar::ITaskItemThumbnailViewModel>::get_TaskItemThumbnail(void * *))"},
         &TaskItemThumbnailViewModel_get_TaskItemThumbnail},
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Taskbar.View.dll symbols not found");
        return false;
    }
    Wh_Log(L"Hooked Taskbar.View.dll");
    return true;
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

std::atomic<bool> g_taskbarViewHooked{false};

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && !g_taskbarViewHooked && module == GetTaskbarViewModule() &&
        !g_taskbarViewHooked.exchange(true)) {
        Wh_Log(L"Taskbar view module loaded");
        if (HookTaskbarViewSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}

// ---------------------------------------------------------------------------
// Running code on the taskbar UI thread (settings changes, unload)
// ---------------------------------------------------------------------------

using UiThreadProc_t = void (*)();
constexpr WPARAM kRunOnThreadTag = 0x57484157;

UINT RunOnThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

LRESULT CALLBACK RunOnThreadHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* msg = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (msg->message == RunOnThreadMessage() && msg->wParam == kRunOnThreadTag) {
            reinterpret_cast<UiThreadProc_t>(msg->lParam)();
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs `proc` synchronously on every taskbar UI thread the mod has touched.
// That thread owns the taskbar windows themselves, so it always has a
// top-level window to receive the message.
void RunOnUiThreads(UiThreadProc_t proc) {
    std::set<DWORD> threads;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        threads = g_uiThreads;
    }
    for (DWORD threadId : threads) {
        HWND hwnd = nullptr;
        EnumThreadWindows(
            threadId,
            [](HWND h, LPARAM param) -> BOOL {
                *reinterpret_cast<HWND*>(param) = h;
                return FALSE;
            },
            reinterpret_cast<LPARAM>(&hwnd));
        if (!hwnd) {
            continue;
        }
        HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnThreadHookProc, nullptr,
                                       threadId);
        if (!hook) {
            continue;
        }
        SendMessageW(hwnd, RunOnThreadMessage(), kRunOnThreadTag,
                     reinterpret_cast<LPARAM>(proc));
        UnhookWindowsHookEx(hook);
    }
}

void RemoveOverlaysOnThisThread() {
    std::vector<OverlayEntry> mine;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        const DWORD threadId = GetCurrentThreadId();
        std::erase_if(g_overlays, [&](OverlayEntry& e) {
            if (e.threadId != threadId) {
                return false;
            }
            mine.push_back(e);
            return true;
        });
    }
    for (auto& entry : mine) {
        try {
            auto overlay = entry.overlay.get();
            if (!overlay) {
                continue;
            }
            auto panel = Media::VisualTreeHelper::GetParent(overlay)
                             .try_as<Controls::Panel>();
            uint32_t index = 0;
            if (panel && panel.Children().IndexOf(overlay, index)) {
                panel.Children().RemoveAt(index);
            }
        } catch (...) {
        }
    }
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (HMODULE module = GetTaskbarViewModule()) {
        g_taskbarViewHooked = true;
        return HookTaskbarViewSymbols(module);
    }

    // Explorer is starting; hook when the taskbar view module loads.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW =
        reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW) {
        return FALSE;
    }
    WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    RunOnUiThreads([] { RemoveOverlaysOnThisThread(); });
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    // Rebuild the highlight with the new look on any open cards.
    RunOnUiThreads([] {
        try {
            RemoveOverlaysOnThisThread();
            for (auto& card : LiveCardsOnThisThread()) {
                UpdateCard(card);
            }
        } catch (...) {
            Wh_Log(L"Settings refresh error %08X", winrt::to_hresult());
        }
    });
}
