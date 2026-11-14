// ==WindhawkMod==
// @id              one-letter-language-indicator
// @name            1 Letter Language Indicator
// @description     Shortens the taskbar language indicator ("ENG", "ENG (US)") to a single letter ("E")
// @version         1.0
// @author          Skept
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @architecture    arm64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// The taskbar XamlRoot lookup, the system tray module handling and the
// RunFromWindowThread helper are based on m417z's "Taskbar tray icon spacing
// and grid" mod (GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-notification-icon-spacing.wh.cpp

// ==WindhawkModReadme==
/*
# 1 Letter Language Indicator

Simplifies the input language indicator in the Windows 11 taskbar tray to a
single letter: `ENG` becomes `E`, and `ENG (US)` becomes `E`.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/one-letter-language-indicator/demo.png)

The indicator keeps updating when you switch input languages, and the original
text is restored when the mod is disabled.

## Settings

- **Letter size**: the font size of the letter. Set it to 0 to keep the
  Windows size (12).
- **Letter color**: separate colors for light mode and dark mode, chosen by the
  taskbar's theme (the Windows mode). Each one can be:
  - A hex color (`#RRGGBB` or `#AARRGGBB`) or a color name (`Red`, `White`).
  - `accent` for the Windows accent color, or one of its shades:
    `accentLight1` to `accentLight3`, `accentDark1` to `accentDark3`. The
    letter follows accent color changes.
  - Empty, to keep the Windows color in that mode.
- **Vertical offset**: moves the letter up (negative) or down (positive), in
  pixels. Useful for re-centering the letter after changing its size.
- **Indicator width**: the language indicator normally reserves room for three
  letters. With a single letter that leaves empty space, so by default the
  indicator is narrowed to 32 pixels, the width of a regular tray icon. Set it
  to 0 to keep the Windows width.

## Target

```
SystemTray.LanguageTextIconContent > Grid#ContainerGrid >
SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock
```

Only Windows 11 is supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fontSize: 0
  $name: Letter size
  $description: >-
    Font size of the letter. Set to 0 to keep the Windows default size (12).
- textColor:
  - light: ""
    $name: Light mode
    $description: Color of the letter when the taskbar is light.
  - dark: ""
    $name: Dark mode
    $description: Color of the letter when the taskbar is dark.
  $name: Letter color
  $description: >-
    A hex color (#RRGGBB or #AARRGGBB), a color name (e.g. Red), or accent,
    accentLight1-3, accentDark1-3 for the Windows accent color. Leave a mode
    empty to keep the Windows color in that mode.
- verticalOffset: 0
  $name: Vertical offset
  $description: >-
    Moves the letter vertically, in pixels. Negative values move it up,
    positive values move it down.
- iconWidth: 32
  $name: Indicator width
  $description: >-
    Width of the language indicator, in pixels. 32 matches the other tray
    icons. Set to 0 to keep the Windows default width.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cwctype>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <optional>
#include <string>
#include <string_view>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

using WinrtInspectable = winrt::Windows::Foundation::IInspectable;
using winrt::Windows::UI::Color;
using winrt::Windows::UI::ViewManagement::UIColorType;
using winrt::Windows::UI::ViewManagement::UISettings;

struct {
    int fontSize;
    std::wstring textColorLight;
    std::wstring textColorDark;
    int verticalOffset;
    int iconWidth;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

// Loaded handlers of tray icons which haven't been loaded yet.
std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

// Only used when a color setting refers to the accent color. Created and
// released on the taskbar UI thread.
UISettings g_uiSettings{nullptr};
winrt::event_token g_colorValuesChangedToken{};

////////////////////////////////////////////////////////////////////////////////
// Helpers.

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

// Runs the procedure synchronously on the thread of the given window.
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

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == className) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindDescendantByName(DependencyObject element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        auto childElement = child.try_as<FrameworkElement>();
        if (childElement && childElement.Name() == name) {
            return childElement;
        }

        if (auto result = FindDescendantByName(child, name)) {
            return result;
        }
    }

    return nullptr;
}

// Calls the callback for every descendant of the given class, without
// descending into matching elements. Returns the number of matches.
int EnumDescendantsByClassName(
    DependencyObject element,
    PCWSTR className,
    const std::function<void(FrameworkElement)>& callback) {
    int count = 0;

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        auto childElement = child.try_as<FrameworkElement>();
        if (childElement && winrt::get_class_name(childElement) == className) {
            callback(childElement);
            count++;
            continue;
        }

        count += EnumDescendantsByClassName(child, className, callback);
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////
// Property overrides.
//
// Each override watches a dependency property with a property changed callback.
// Whenever the property gets a value which doesn't match the override (e.g. the
// taskbar sets the text of a new input language), the value is replaced. The
// taskbar's value is kept, so that it can be restored when the mod is unloaded
// or the settings change.

// Returns one of:
// - The value to set.
// - nullptr if the current value is fine.
// - DependencyProperty::UnsetValue() if the property shouldn't be overridden
//   right now (e.g. no color is set for the current theme). An existing
//   override is reverted to the taskbar's value.
using ComputeOverride_t =
    std::function<WinrtInspectable(const DependencyObject& element,
                                   const WinrtInspectable& currentValue)>;

struct PropertyOverride {
    winrt::weak_ref<DependencyObject> element;
    DependencyProperty property{nullptr};
    ComputeOverride_t computeOverride;
    int64_t callbackToken = 0;

    // Whether the value currently set is ours.
    bool overridden = false;
    // The taskbar's local value, restored when the override is removed.
    WinrtInspectable originalLocalValue{nullptr};
    // The value we set, to tell our value apart from the taskbar's.
    WinrtInspectable lastSetValue{nullptr};

    // For overrides which depend on the theme (the letter color).
    bool refreshOnThemeChange = false;
    winrt::event_token themeChangedToken{};
};

// Only accessed from the taskbar UI thread.
std::list<PropertyOverride> g_propertyOverrides;

void RestoreOriginalValue(PropertyOverride& propertyOverride,
                          const DependencyObject& element) {
    if (!propertyOverride.overridden) {
        return;
    }

    // Reset first: setting the value runs the property changed callback
    // synchronously, and it must not see the override as active.
    propertyOverride.overridden = false;

    const auto& original = propertyOverride.originalLocalValue;
    if (!original || original == DependencyProperty::UnsetValue() ||
        original.try_as<Data::BindingExpressionBase>()) {
        element.ClearValue(propertyOverride.property);
    } else {
        element.SetValue(propertyOverride.property, original);
    }
}

void ApplyPropertyOverride(PropertyOverride& propertyOverride,
                           const DependencyObject& element) {
    WinrtInspectable currentValue = element.GetValue(propertyOverride.property);
    WinrtInspectable newValue =
        propertyOverride.computeOverride(element, currentValue);
    if (!newValue) {
        return;
    }

    if (newValue == DependencyProperty::UnsetValue()) {
        RestoreOriginalValue(propertyOverride, element);
        return;
    }

    // Remember the taskbar's value. When an override is already active, the
    // local value is either the taskbar's new value (e.g. a language switch),
    // or our own previous value being replaced (e.g. the accent color changed),
    // in which case the remembered value stays.
    WinrtInspectable localValue =
        element.ReadLocalValue(propertyOverride.property);
    if (!propertyOverride.overridden ||
        localValue != propertyOverride.lastSetValue) {
        propertyOverride.originalLocalValue = localValue;
    }

    propertyOverride.overridden = true;
    propertyOverride.lastSetValue = newValue;

    // Runs the property changed callback again, which is a no-op since the
    // value now matches the override.
    element.SetValue(propertyOverride.property, newValue);
}

void RefreshAllPropertyOverrides() {
    for (auto& propertyOverride : g_propertyOverrides) {
        auto element = propertyOverride.element.get();
        if (!element) {
            continue;
        }

        try {
            ApplyPropertyOverride(propertyOverride, element);
        } catch (const winrt::hresult_error& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

void TrackProperty(const DependencyObject& element,
                   const DependencyProperty& property,
                   ComputeOverride_t computeOverride,
                   bool refreshOnThemeChange = false) {
    // Drop entries of elements which no longer exist.
    for (auto it = g_propertyOverrides.begin();
         it != g_propertyOverrides.end();) {
        if (!it->element.get()) {
            it = g_propertyOverrides.erase(it);
        } else {
            ++it;
        }
    }

    // Already tracked, e.g. the Loaded event fired more than once.
    for (auto& propertyOverride : g_propertyOverrides) {
        if (propertyOverride.property == property &&
            propertyOverride.element.get() == element) {
            ApplyPropertyOverride(propertyOverride, element);
            return;
        }
    }

    g_propertyOverrides.emplace_back();
    auto it = std::prev(g_propertyOverrides.end());

    it->element = winrt::make_weak(element);
    it->property = property;
    it->computeOverride = std::move(computeOverride);
    it->callbackToken = element.RegisterPropertyChangedCallback(
        property,
        [it](const DependencyObject& sender, const DependencyProperty&) {
            if (g_unloading) {
                return;
            }

            try {
                ApplyPropertyOverride(*it, sender);
            } catch (const winrt::hresult_error& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        });

    // While the property is overridden, a theme change doesn't change the
    // property, so the callback above doesn't run. Re-evaluate on theme change.
    if (refreshOnThemeChange) {
        if (auto frameworkElement = element.try_as<FrameworkElement>()) {
            it->refreshOnThemeChange = true;
            it->themeChangedToken = frameworkElement.ActualThemeChanged(
                [it](const FrameworkElement& sender, const WinrtInspectable&) {
                    if (g_unloading) {
                        return;
                    }

                    try {
                        ApplyPropertyOverride(*it, sender);
                    } catch (const winrt::hresult_error& ex) {
                        Wh_Log(L"Error %08X: %s", ex.code(),
                               ex.message().c_str());
                    }
                });
        }
    }

    ApplyPropertyOverride(*it, element);
}

void RestoreAllPropertyOverrides() {
    for (auto& propertyOverride : g_propertyOverrides) {
        auto element = propertyOverride.element.get();
        if (!element) {
            continue;
        }

        try {
            element.UnregisterPropertyChangedCallback(
                propertyOverride.property, propertyOverride.callbackToken);

            if (propertyOverride.refreshOnThemeChange) {
                if (auto frameworkElement = element.try_as<FrameworkElement>()) {
                    frameworkElement.ActualThemeChanged(
                        propertyOverride.themeChangedToken);
                }
            }

            RestoreOriginalValue(propertyOverride, element);
        } catch (const winrt::hresult_error& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_propertyOverrides.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Colors.

// Accepts anything XAML accepts for a color: "#RRGGBB", "#AARRGGBB", "Red".
std::optional<Color> ParseColor(const std::wstring& str) {
    try {
        auto value = Markup::XamlBindingHelper::ConvertValue(
            winrt::xaml_typename<Color>(),
            winrt::box_value(winrt::hstring(str)));
        return winrt::unbox_value<Color>(value);
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Invalid color \"%s\": %08X", str.c_str(), ex.code());
        return std::nullopt;
    }
}

// "accent", "accentLight1".."accentLight3", "accentDark1".."accentDark3"
// (case-insensitive).
std::optional<UIColorType> GetAccentColorType(const std::wstring& setting) {
    static const struct {
        PCWSTR name;
        UIColorType type;
    } accentColors[] = {
        {L"accent", UIColorType::Accent},
        {L"accentLight1", UIColorType::AccentLight1},
        {L"accentLight2", UIColorType::AccentLight2},
        {L"accentLight3", UIColorType::AccentLight3},
        {L"accentDark1", UIColorType::AccentDark1},
        {L"accentDark2", UIColorType::AccentDark2},
        {L"accentDark3", UIColorType::AccentDark3},
    };

    for (const auto& accentColor : accentColors) {
        if (_wcsicmp(setting.c_str(), accentColor.name) == 0) {
            return accentColor.type;
        }
    }

    return std::nullopt;
}

bool UsesAccentColor() {
    return GetAccentColorType(g_settings.textColorLight) ||
           GetAccentColorType(g_settings.textColorDark);
}

void RegisterAccentColorChanges();

// Called on the taskbar UI thread.
std::optional<Color> ResolveColorSetting(const std::wstring& setting) {
    auto accentColorType = GetAccentColorType(setting);
    if (!accentColorType) {
        return ParseColor(setting);
    }

    // Usually registered by ApplySettings already, but not when the taskbar
    // didn't exist yet at that point (e.g. right after explorer starts).
    RegisterAccentColorChanges();
    if (!g_uiSettings) {
        Wh_Log(L"UISettings unavailable for %s", setting.c_str());
        return std::nullopt;
    }

    try {
        return g_uiSettings.GetColorValue(*accentColorType);
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"GetColorValue failed: %08X", ex.code());
        return std::nullopt;
    }
}

// Must be called on the taskbar UI thread. The ColorValuesChanged event fires
// on a background thread, so the refresh is sent back to the taskbar thread.
void RegisterAccentColorChanges() {
    if (g_uiSettings) {
        return;
    }

    try {
        g_uiSettings = UISettings();
        g_colorValuesChangedToken = g_uiSettings.ColorValuesChanged(
            [](const UISettings&, const WinrtInspectable&) {
                if (g_unloading) {
                    return;
                }

                HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                if (!hTaskbarWnd) {
                    return;
                }

                Wh_Log(L"Color values changed");

                RunFromWindowThread(
                    hTaskbarWnd,
                    [](void*) {
                        if (!g_unloading) {
                            RefreshAllPropertyOverrides();
                        }
                    },
                    nullptr);
            });
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"UISettings failed: %08X", ex.code());
        g_uiSettings = nullptr;
    }
}

// Must be called on the taskbar UI thread.
void UnregisterAccentColorChanges() {
    if (!g_uiSettings) {
        return;
    }

    try {
        g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    g_uiSettings = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Language indicator.

// "ENG" -> "E", "ENG (US)" -> "E", "ENG\nUS" -> "E". Returns std::nullopt if
// the text is empty or already a single letter.
std::optional<std::wstring> ShortenLanguageText(std::wstring_view text) {
    size_t start = 0;
    while (start < text.size() && std::iswspace(text[start])) {
        start++;
    }

    if (start == text.size()) {
        return std::nullopt;
    }

    // Keep surrogate pairs whole.
    size_t length = 1;
    if (IS_HIGH_SURROGATE(text[start]) && start + 1 < text.size() &&
        IS_LOW_SURROGATE(text[start + 1])) {
        length = 2;
    }

    if (start == 0 && length == text.size()) {
        return std::nullopt;
    }

    return std::wstring(text.substr(start, length));
}

WinrtInspectable ComputeShortText(const DependencyObject&,
                                  const WinrtInspectable& value) {
    auto text = winrt::unbox_value_or<winrt::hstring>(value, L"");
    auto shortText = ShortenLanguageText(text);
    if (!shortText) {
        return nullptr;
    }

    Wh_Log(L"\"%s\" -> \"%s\"", text.c_str(), shortText->c_str());

    return winrt::box_value(winrt::hstring(*shortText));
}

ComputeOverride_t MakeDoubleOverride(double desiredValue) {
    return [desiredValue](const DependencyObject&,
                          const WinrtInspectable& value) -> WinrtInspectable {
        double currentValue = winrt::unbox_value_or<double>(
            value, std::numeric_limits<double>::quiet_NaN());
        if (currentValue == desiredValue) {
            return nullptr;
        }

        return winrt::box_value(desiredValue);
    };
}

// Picks the light or dark mode color by the TextBlock's theme, which follows
// the taskbar (the Windows mode), not the apps mode.
WinrtInspectable ComputeForeground(const DependencyObject& element,
                                   const WinrtInspectable& value) {
    auto frameworkElement = element.try_as<FrameworkElement>();
    bool light =
        frameworkElement && frameworkElement.ActualTheme() == ElementTheme::Light;

    const std::wstring& setting =
        light ? g_settings.textColorLight : g_settings.textColorDark;
    if (setting.empty()) {
        return DependencyProperty::UnsetValue();
    }

    auto color = ResolveColorSetting(setting);
    if (!color) {
        return DependencyProperty::UnsetValue();
    }

    if (auto brush = value ? value.try_as<Media::SolidColorBrush>() : nullptr) {
        if (brush.Color() == *color) {
            return nullptr;
        }
    }

    Wh_Log(L"Setting %s mode color %s", light ? L"light" : L"dark",
           setting.c_str());

    return Media::SolidColorBrush{*color};
}

// A render transform moves the letter without affecting the layout, so it
// doesn't conflict with the margin the taskbar sets on the TextBlock.
ComputeOverride_t MakeVerticalOffsetOverride(double offsetY) {
    return [offsetY](const DependencyObject&,
                     const WinrtInspectable& value) -> WinrtInspectable {
        if (auto translateTransform =
                value ? value.try_as<Media::TranslateTransform>() : nullptr) {
            if (translateTransform.X() == 0 &&
                translateTransform.Y() == offsetY) {
                return nullptr;
            }
        }

        Media::TranslateTransform newTransform;
        newTransform.Y(offsetY);
        return newTransform;
    };
}

void TrackLanguageIndicator(FrameworkElement languageTextIconContent) {
    Wh_Log(L"Found LanguageTextIconContent");

    if (g_settings.iconWidth > 0) {
        TrackProperty(languageTextIconContent,
                      FrameworkElement::WidthProperty(),
                      MakeDoubleOverride(g_settings.iconWidth));
        TrackProperty(languageTextIconContent,
                      FrameworkElement::MinWidthProperty(),
                      MakeDoubleOverride(0));
    }

    Controls::TextBlock innerTextBlock = nullptr;
    if (auto adaptiveTextBlock = FindDescendantByName(
            languageTextIconContent, L"LanguageInnerTextBlock")) {
        innerTextBlock =
            FindDescendantByName(adaptiveTextBlock, L"InnerTextBlock")
                .try_as<Controls::TextBlock>();
    }

    if (!innerTextBlock) {
        Wh_Log(L"InnerTextBlock not found");
        return;
    }

    if (innerTextBlock.GetBindingExpression(
            Controls::TextBlock::TextProperty())) {
        // Overriding a data-bound Text replaces the binding, which would stop
        // the indicator from updating on language switches.
        Wh_Log(L"Warning: InnerTextBlock.Text is data-bound");
    }

    TrackProperty(innerTextBlock, Controls::TextBlock::TextProperty(),
                  ComputeShortText);

    if (g_settings.fontSize > 0) {
        TrackProperty(innerTextBlock, Controls::TextBlock::FontSizeProperty(),
                      MakeDoubleOverride(g_settings.fontSize));
    }

    if (!g_settings.textColorLight.empty() ||
        !g_settings.textColorDark.empty()) {
        TrackProperty(innerTextBlock, Controls::TextBlock::ForegroundProperty(),
                      ComputeForeground, /*refreshOnThemeChange=*/true);
    }

    if (g_settings.verticalOffset != 0) {
        TrackProperty(innerTextBlock, UIElement::RenderTransformProperty(),
                      MakeVerticalOffsetOverride(g_settings.verticalOffset));
    }
}

int TrackLanguageIndicatorsIn(DependencyObject root) {
    return EnumDescendantsByClassName(root,
                                      L"SystemTray.LanguageTextIconContent",
                                      TrackLanguageIndicator);
}

////////////////////////////////////////////////////////////////////////////////
// New tray icons, e.g. after explorer restarts or an input language is added.

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    // The icon's content isn't created yet, look for the language indicator
    // once the icon is loaded.
    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](const WinrtInspectable& sender, const RoutedEventArgs&) {
            g_autoRevokerList.erase(autoRevokerIt);

            if (g_unloading) {
                return;
            }

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            try {
                if (TrackLanguageIndicatorsIn(iconView) > 0) {
                    Wh_Log(L"Language indicator loaded");
                }
            } catch (const winrt::hresult_error& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        });

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar XamlRoot, for applying to the existing taskbar.

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
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
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // The offset of the taskbar element in TaskbarHost, read from the code of
    // TaskbarHost::FrameHeight.
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

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Settings.

std::wstring GetTrimmedStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value;
    Wh_FreeStringSetting(value);

    size_t first = result.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return L"";
    }

    size_t last = result.find_last_not_of(L" \t");
    return result.substr(first, last - first + 1);
}

void LoadSettings() {
    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_settings.fontSize < 0) {
        g_settings.fontSize = 0;
    }

    g_settings.textColorLight = GetTrimmedStringSetting(L"textColor.light");
    g_settings.textColorDark = GetTrimmedStringSetting(L"textColor.dark");

    g_settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");

    g_settings.iconWidth = Wh_GetIntSetting(L"iconWidth");
    if (g_settings.iconWidth < 0) {
        g_settings.iconWidth = 0;
    }
}

// Restores everything, then, unless unloading, applies the current settings to
// the existing taskbar.
void ApplySettings() {
    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            HWND hTaskbarWnd = (HWND)pParam;

            RestoreAllPropertyOverrides();

            if (g_unloading) {
                UnregisterAccentColorChanges();
                g_autoRevokerList.clear();
                return;
            }

            if (UsesAccentColor()) {
                RegisterAccentColorChanges();
            } else {
                UnregisterAccentColorChanges();
            }

            try {
                auto xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
                if (!xamlRoot) {
                    Wh_Log(L"Getting XamlRoot failed");
                    return;
                }

                auto content = xamlRoot.Content().try_as<FrameworkElement>();
                if (!content) {
                    Wh_Log(L"No XamlRoot content");
                    return;
                }

                FrameworkElement searchRoot = FindChildByClassName(
                    content, L"SystemTray.SystemTrayFrame");
                if (!searchRoot) {
                    Wh_Log(L"SystemTrayFrame not found, searching everything");
                    searchRoot = content;
                }

                int count = TrackLanguageIndicatorsIn(searchRoot);
                Wh_Log(L"Language indicators found: %d", count);
            } catch (const winrt::hresult_error& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        },
        hTaskbarWnd);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks.

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

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
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
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

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle.

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

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
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The system tray module might have been loaded after Wh_ModInit checked.
    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}
