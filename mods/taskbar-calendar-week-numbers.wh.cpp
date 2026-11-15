// ==WindhawkMod==
// @id              taskbar-calendar-week-numbers
// @name            Taskbar Calendar Week Numbers
// @description     Shows calendar week numbers (ISO 8601 or regional) in the Windows 11 taskbar calendar flyout
// @version         1.0
// @author          Maveric2005
// @github          https://github.com/Maveric2005
// @homepage        https://github.com/Maveric2005/windhawk-calendar-week-numbers
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Calendar Week Numbers

Adds a week number column to the calendar flyout that opens when you click the
clock in the Windows 11 taskbar, like the week numbers in Outlook.

![Calendar flyout with week numbers](https://raw.githubusercontent.com/Maveric2005/windhawk-calendar-week-numbers/main/images/screenshot.png)

## Settings

- **Week numbering**: ISO 8601 (default; used in Germany and most of Europe)
  or the first-week-of-year rule from the Windows regional settings.
- **Column header**: text above the column. Empty means automatic ("KW" for a
  German Windows display language, "Wk" otherwise). A single space hides it.
- **Week number color**: the Windows accent color (default, the same color as
  today's date) or the normal text color.
- **Column width**, **font size** and **opacity** of the week numbers.

## Notes

- Only the calendar flyout is changed. The mod writes nothing to disk and makes
  no network requests.
- The calendar's day cells get slightly narrower to make room for the column.
- The calendar flyout is hosted in `ShellExperienceHost.exe`. Tested on
  Windows 11 build 26220.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- weekNumbering: iso
  $name: Week numbering
  $options:
  - iso: ISO 8601 (Germany, most of Europe)
  - system: Windows regional settings
- headerText: ""
  $name: Column header
  $description: Empty for automatic ("KW" or "Wk"), a single space to hide
- color: accent
  $name: Week number color
  $options:
  - accent: Accent color (like today's date)
  - text: Text color
- columnWidth: 24
  $name: Column width
  $description: In pixels, 10 to 60
- fontSize: 11
  $name: Font size
  $description: 6 to 30
- opacity: 100
  $name: Opacity (percent)
  $description: 5 to 100
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
// For Popup::Child() on the result of VisualTreeHelper::GetOpenPopups().
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings

// 0 = the week containing 1 January is week 1, 1 = first full week,
// 2 = first four-day week.
int RegionalFirstWeekRule() {
    WCHAR value[4];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTWEEKOFYEAR, value,
                        ARRAYSIZE(value))) {
        int rule = value[0] - L'0';
        if (rule >= 0 && rule <= 2) {
            return rule;
        }
    }
    return 0;
}

enum class WeekNumbering { Iso, System };

struct Settings {
    WeekNumbering weekNumbering = WeekNumbering::Iso;
    std::wstring headerText;
    bool accentColor = true;
    double columnWidth = 24;
    double fontSize = 11;
    double opacity = 1;
    // LOCALE_IFIRSTWEEKOFYEAR, read once instead of for every calendar row.
    int firstWeekRule = 0;
};

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

void LoadSettings() {
    Settings s;

    auto numbering = WindhawkUtils::StringSetting::make(L"weekNumbering");
    s.weekNumbering = wcscmp(numbering, L"system") == 0 ? WeekNumbering::System
                                                        : WeekNumbering::Iso;

    s.headerText = WindhawkUtils::StringSetting::make(L"headerText").get();
    if (s.headerText.empty()) {
        s.headerText =
            PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_GERMAN ? L"KW" : L"Wk";
    }

    auto color = WindhawkUtils::StringSetting::make(L"color");
    s.accentColor = wcscmp(color, L"text") != 0;

    int columnWidth = Wh_GetIntSetting(L"columnWidth");
    s.columnWidth = columnWidth >= 10 && columnWidth <= 60 ? columnWidth : 24;

    int fontSize = Wh_GetIntSetting(L"fontSize");
    s.fontSize = fontSize >= 6 && fontSize <= 30 ? fontSize : 11;

    int opacity = Wh_GetIntSetting(L"opacity");
    s.opacity = (opacity >= 5 && opacity <= 100 ? opacity : 100) / 100.0;

    s.firstWeekRule = RegionalFirstWeekRule();

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(s);
}

////////////////////////////////////////////////////////////////////////////////
// Dates and week numbers

// Days since 1970-01-01 for a proleptic Gregorian date (H. Hinnant).
int DaysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<int>(doe) - 719468;
}

int YearFromDays(int z) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    return static_cast<int>(yoe) + era * 400 + (mp >= 10);
}

// 0 = Sunday ... 6 = Saturday, same as Windows.Globalization.DayOfWeek.
int WeekdayFromDays(int z) {
    return z >= -4 ? (z + 4) % 7 : (z + 5) % 7 + 6;
}

int IsoWeek(int days) {
    // The ISO week of a day is the week of the Thursday of its Monday-based week.
    const int weekday = (WeekdayFromDays(days) + 6) % 7;  // 0 = Monday.
    const int thursday = days - weekday + 3;
    const int jan1 = DaysFromCivil(YearFromDays(thursday), 1, 1);
    return (thursday - jan1) / 7 + 1;
}

// Same algorithm as .NET's Calendar.GetWeekOfYear. rule: 0 = the week
// containing 1 January is week 1, 1 = first full week, 2 = first four-day week.
int RegionalWeek(int days, int firstDayOfWeek, int rule) {
    const int jan1 = DaysFromCivil(YearFromDays(days), 1, 1);
    const int dayOfYear = days - jan1;  // 0-based.

    if (rule == 0) {
        const int offset = (WeekdayFromDays(jan1) - firstDayOfWeek + 7) % 7;
        return (dayOfYear + offset) / 7 + 1;
    }

    const int fullDays = rule == 1 ? 7 : 4;
    const int dayForJan1 = WeekdayFromDays(days) - (dayOfYear % 7);
    int offset = (firstDayOfWeek - dayForJan1 + 14) % 7;
    if (offset != 0 && offset >= fullDays) {
        offset -= 7;
    }

    const int day = dayOfYear - offset;
    if (day >= 0) {
        return day / 7 + 1;
    }

    // The day belongs to the last week of the previous year.
    return RegionalWeek(jan1 - 1, firstDayOfWeek, rule);
}

// The week number of the calendar row which starts on rowStartDays.
int WeekNumberOfRow(int rowStartDays,
                    int firstDayOfWeek,
                    WeekNumbering numbering,
                    int firstWeekRule) {
    if (numbering == WeekNumbering::Iso) {
        // A row starting on Monday is exactly one ISO week. With another first
        // day of the week the row spans two ISO weeks, and the one containing
        // the row's Monday has the most days in it (at least five).
        const int toMonday = (1 - WeekdayFromDays(rowStartDays) + 7) % 7;
        return IsoWeek(rowStartDays + toMonday);
    }

    // With the regional rules a week starts on the calendar's first day, so
    // every day of the row has the same number. The last day of the row is
    // used so that the row containing 1 January is week 1 with the "week
    // containing 1 January" rule.
    return RegionalWeek(rowStartDays + 6, firstDayOfWeek, firstWeekRule);
}

// CalendarViewDayItem.Date carries no time zone. Depending on how the shell
// fills it, the ticks are local midnight or UTC midnight of the cell's date.
// West of UTC the UTC reading gives the right date for both, east of UTC (or
// at UTC) the local reading does.
bool DaysFromDateTime(wf::DateTime const& dateTime, int& days) {
    ULARGE_INTEGER uli;
    uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());
    FILETIME fileTime{uli.LowPart, uli.HighPart};

    SYSTEMTIME utc;
    if (!FileTimeToSystemTime(&fileTime, &utc)) {
        return false;
    }

    SYSTEMTIME local;
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local)) {
        return false;
    }

    FILETIME localFileTime;
    if (!SystemTimeToFileTime(&local, &localFileTime)) {
        return false;
    }

    const SYSTEMTIME& date =
        CompareFileTime(&localFileTime, &fileTime) < 0 ? utc : local;
    days = DaysFromCivil(date.wYear, date.wMonth, date.wDay);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Calendar UI (all of this runs on the UI thread of the calendar window)
//
// The CalendarView template has a "MonthView" grid with the weekday names in
// the first row and the scrolling day panel in the second. The mod moves both
// to the right by the column width and adds a header and a canvas in the freed
// space. The canvas holds one label per visible calendar row, positioned next
// to the row's first day whenever the panel scrolls or is laid out.

constexpr PCWSTR kOverlayName = L"WhWeekNumberColumn";
constexpr PCWSTR kHeaderName = L"WhWeekNumberHeader";

struct CalendarState {
    winrt::weak_ref<wuxc::CalendarView> calendar;
    winrt::weak_ref<wuxc::Grid> monthView;
    winrt::weak_ref<wux::FrameworkElement> weekDayNames;
    winrt::weak_ref<wuxc::ScrollViewer> scrollViewer;
    winrt::weak_ref<wux::FrameworkElement> panel;
    winrt::weak_ref<wuxc::Canvas> overlay;
    winrt::weak_ref<wuxc::TextBlock> header;
    std::vector<winrt::weak_ref<wuxc::Border>> labels;

    wux::Thickness weekDayNamesMargin{};
    wux::Thickness scrollViewerMargin{};
    // The horizontal margin of the day cells before the mod narrowed them.
    double itemMargin = -1;

    winrt::event_token viewChangingToken{};
    winrt::event_token viewChangedToken{};
    winrt::event_token panelLayoutUpdatedToken{};
    winrt::event_token overlaySizeChangedToken{};
};

// The event handlers of a calendar hold weak pointers to its state, so a state
// which is dropped can't be used by a handler which is still subscribed.
thread_local std::vector<std::shared_ptr<CalendarState>> t_calendars;
thread_local wuc::CoreWindow t_coreWindow{nullptr};
thread_local winrt::event_token t_visibilityChangedToken{};
thread_local winrt::weak_ref<wux::FrameworkElement> t_layoutRoot;
thread_local winrt::event_token t_layoutUpdatedToken{};
thread_local ULONGLONG t_lastScanTick = 0;
// Whether a calendar was ever attached on this thread, i.e. this is the
// calendar flyout's thread.
thread_local bool t_hadCalendar = false;

template <typename T>
T FindByName(wux::DependencyObject const& root, PCWSTR name, int depth = 0) {
    if (!root || depth > 16) {
        return nullptr;
    }
    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        if (fe.Name() == name) {
            if (auto result = fe.try_as<T>()) {
                return result;
            }
        }
    }
    // Day cells don't contain anything the mod looks for.
    if (root.try_as<wuxc::CalendarViewDayItem>()) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        if (auto result =
                FindByName<T>(wuxm::VisualTreeHelper::GetChild(root, i), name, depth + 1)) {
            return result;
        }
    }
    return nullptr;
}

// The brush of today's date: the calendar's TodayBackground if the shell set
// one, otherwise the accent color shade Windows uses for it in this theme.
wuxm::Brush GetAccentBrush(wuxc::CalendarView const& calendar) {
    if (auto brush = calendar.TodayBackground()) {
        return brush;
    }
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    const bool dark = calendar.ActualTheme() == wux::ElementTheme::Dark;
    return wuxm::SolidColorBrush(uiSettings.GetColorValue(
        dark ? winrt::Windows::UI::ViewManagement::UIColorType::AccentLight2
             : winrt::Windows::UI::ViewManagement::UIColorType::AccentDark1));
}

void ApplyLabelStyle(wuxc::TextBlock const& text,
                     Settings const& settings,
                     wuxm::Brush const& accentBrush) {
    text.FontSize(settings.fontSize);
    text.Opacity(settings.opacity);
    if (accentBrush) {
        text.Foreground(accentBrush);
    } else {
        text.ClearValue(wuxc::TextBlock::ForegroundProperty());
    }
}

wuxm::Brush LabelBrush(CalendarState const& state, Settings const& settings) {
    if (!settings.accentColor) {
        return nullptr;
    }
    auto calendar = state.calendar.get();
    return calendar ? GetAccentBrush(calendar) : nullptr;
}

void ApplySettings(CalendarState& state) {
    const Settings settings = GetSettings();
    const double width = settings.columnWidth;

    if (auto weekDayNames = state.weekDayNames.get()) {
        auto m = state.weekDayNamesMargin;
        weekDayNames.Margin({m.Left + width, m.Top, m.Right, m.Bottom});
    }
    if (auto scrollViewer = state.scrollViewer.get()) {
        auto m = state.scrollViewerMargin;
        scrollViewer.Margin({m.Left + width, m.Top, m.Right, m.Bottom});
    }
    if (auto overlay = state.overlay.get()) {
        overlay.Width(width);
    }
    if (auto header = state.header.get()) {
        header.Width(width);
        header.Text(settings.headerText);
    }
    auto brush = LabelBrush(state, settings);
    for (auto const& weakLabel : state.labels) {
        if (auto label = weakLabel.get()) {
            label.Width(width);
            if (auto text = label.Child().try_as<wuxc::TextBlock>()) {
                ApplyLabelStyle(text, settings, brush);
            }
        }
    }
}

wuxc::Border GetLabel(CalendarState& state, wuxc::Canvas const& overlay, size_t index) {
    if (index < state.labels.size()) {
        if (auto label = state.labels[index].get()) {
            return label;
        }
    }

    const Settings settings = GetSettings();

    wuxc::TextBlock text;
    text.HorizontalAlignment(wux::HorizontalAlignment::Center);
    text.VerticalAlignment(wux::VerticalAlignment::Center);
    ApplyLabelStyle(text, settings, LabelBrush(state, settings));

    wuxc::Border label;
    label.Width(settings.columnWidth);
    label.Child(text);
    overlay.Children().Append(label);

    if (index < state.labels.size()) {
        state.labels[index] = winrt::make_weak(label);
    } else {
        state.labels.push_back(winrt::make_weak(label));
    }
    return label;
}

void Reposition(CalendarState& state) {
    auto calendar = state.calendar.get();
    auto panel = state.panel.get();
    auto overlay = state.overlay.get();
    if (!calendar || !panel || !overlay) {
        return;
    }

    const double overlayHeight = overlay.ActualHeight();
    const int firstDayOfWeek = static_cast<int>(calendar.FirstDayOfWeek());
    const Settings settings = GetSettings();

    // The column makes the panel narrower, but the day cells keep their size.
    // Their horizontal margin is reduced so that they still fit, otherwise the
    // hover and selection circles get cut off.
    const double cellWidth = panel.ActualWidth() / 7;

    size_t used = 0;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(panel);
    for (int i = 0; i < count; i++) {
        auto item = wuxm::VisualTreeHelper::GetChild(panel, i)
                        .try_as<wuxc::CalendarViewDayItem>();
        if (!item) {
            continue;
        }

        auto margin = item.Margin();
        if (state.itemMargin < 0) {
            state.itemMargin = margin.Left;
        }
        if (cellWidth > 0 && item.ActualWidth() > 0) {
            const double fit = std::floor((cellWidth - item.ActualWidth()) / 2);
            const double horizontal = std::clamp(fit, 0.0, state.itemMargin);
            if (margin.Left != horizontal || margin.Right != horizontal) {
                item.Margin({horizontal, margin.Top, horizontal, margin.Bottom});
            }
        }

        if (item.Visibility() != wux::Visibility::Visible) {
            continue;
        }

        int days;
        if (!DaysFromDateTime(item.Date(), days) ||
            WeekdayFromDays(days) != firstDayOfWeek) {
            continue;
        }

        const double height = item.ActualHeight();
        if (height <= 0) {
            continue;
        }

        const auto top = item.TransformToVisual(overlay).TransformPoint({0, 0}).Y;
        if (top + height <= 0 || top >= overlayHeight) {
            continue;  // Scrolled out of view, or a recycled cell.
        }

        auto label = GetLabel(state, overlay, used++);
        auto text = label.Child().as<wuxc::TextBlock>();
        auto week = std::to_wstring(WeekNumberOfRow(
            days, firstDayOfWeek, settings.weekNumbering, settings.firstWeekRule));
        if (text.Text() != week) {
            text.Text(week);
        }
        if (wuxc::Canvas::GetTop(label) != top) {
            wuxc::Canvas::SetTop(label, top);
        }
        if (label.Height() != height) {
            label.Height(height);
        }
        if (label.Visibility() != wux::Visibility::Visible) {
            label.Visibility(wux::Visibility::Visible);
        }
    }

    for (size_t i = used; i < state.labels.size(); i++) {
        if (auto label = state.labels[i].get()) {
            if (label.Visibility() != wux::Visibility::Collapsed) {
                label.Visibility(wux::Visibility::Collapsed);
            }
        }
    }
}

void SafeReposition(std::weak_ptr<CalendarState> const& weakState) {
    auto state = weakState.lock();
    if (!state) {
        return;
    }
    try {
        Reposition(*state);
    } catch (...) {
    }
}

bool AttachCalendar(wuxc::CalendarView const& calendar) {
    auto monthView = FindByName<wuxc::Grid>(calendar, L"MonthView");
    auto weekDayNames =
        monthView ? FindByName<wux::FrameworkElement>(monthView, L"WeekDayNames") : nullptr;
    auto scrollViewer =
        monthView ? FindByName<wuxc::ScrollViewer>(monthView, L"MonthViewScrollViewer")
                  : nullptr;
    auto panel = scrollViewer
                     ? FindByName<wux::FrameworkElement>(scrollViewer, L"MonthViewPanel")
                     : nullptr;
    if (!monthView || !weekDayNames || !scrollViewer || !panel) {
        // Not (fully) templated yet; the next scan tries again.
        return false;
    }

    if (FindByName<wuxc::Canvas>(monthView, kOverlayName)) {
        return true;  // Already attached.
    }

    t_hadCalendar = true;
    Wh_Log(L"Attaching to calendar, first day of week %d",
           static_cast<int>(calendar.FirstDayOfWeek()));

    auto state = std::make_shared<CalendarState>();
    state->calendar = winrt::make_weak(calendar);
    state->monthView = winrt::make_weak(monthView);
    state->weekDayNames = winrt::make_weak(weekDayNames);
    state->scrollViewer = winrt::make_weak(scrollViewer);
    state->panel = winrt::make_weak(panel);
    state->weekDayNamesMargin = weekDayNames.Margin();
    state->scrollViewerMargin = scrollViewer.Margin();

    wuxc::Canvas overlay;
    overlay.Name(kOverlayName);
    overlay.IsHitTestVisible(false);
    overlay.HorizontalAlignment(wux::HorizontalAlignment::Left);
    {
        auto m = state->scrollViewerMargin;
        overlay.Margin({m.Left, m.Top, 0, m.Bottom});
    }
    wuxc::Grid::SetRow(overlay, wuxc::Grid::GetRow(scrollViewer));
    wuxc::Grid::SetRowSpan(overlay, wuxc::Grid::GetRowSpan(scrollViewer));
    state->overlay = winrt::make_weak(overlay);

    wuxc::TextBlock header;
    header.Name(kHeaderName);
    header.IsHitTestVisible(false);
    header.HorizontalAlignment(wux::HorizontalAlignment::Left);
    header.VerticalAlignment(wux::VerticalAlignment::Center);
    header.TextAlignment(wux::TextAlignment::Center);
    {
        auto m = state->weekDayNamesMargin;
        header.Margin({m.Left, m.Top, 0, m.Bottom});
    }
    wuxc::Grid::SetRow(header, wuxc::Grid::GetRow(weekDayNames));
    // Look like the weekday names.
    if (auto weekDay = FindByName<wuxc::TextBlock>(weekDayNames, L"WeekDay1")) {
        header.FontSize(weekDay.FontSize());
        header.FontWeight(weekDay.FontWeight());
        header.Foreground(weekDay.Foreground());
        header.Opacity(weekDay.Opacity());
    }
    state->header = winrt::make_weak(header);

    monthView.Children().Append(overlay);
    monthView.Children().Append(header);

    std::weak_ptr<CalendarState> weakState = state;
    state->viewChangingToken = scrollViewer.ViewChanging(
        [weakState](auto const&, auto const&) { SafeReposition(weakState); });
    state->viewChangedToken = scrollViewer.ViewChanged(
        [weakState](auto const&, auto const&) { SafeReposition(weakState); });
    state->panelLayoutUpdatedToken = panel.LayoutUpdated(
        [weakState](auto const&, auto const&) { SafeReposition(weakState); });
    state->overlaySizeChangedToken = overlay.SizeChanged(
        [weakState](auto const& sender, wux::SizeChangedEventArgs const& args) {
            // Hide labels of rows which are scrolled partly out of view.
            wuxm::RectangleGeometry clip;
            clip.Rect({0, 0, static_cast<float>(args.NewSize().Width),
                       static_cast<float>(args.NewSize().Height)});
            sender.template as<wux::UIElement>().Clip(clip);
            SafeReposition(weakState);
        });

    ApplySettings(*state);
    Reposition(*state);
    t_calendars.push_back(std::move(state));
    return true;
}

void DetachCalendar(CalendarState& state) {
    if (auto scrollViewer = state.scrollViewer.get()) {
        scrollViewer.ViewChanging(state.viewChangingToken);
        scrollViewer.ViewChanged(state.viewChangedToken);
        scrollViewer.Margin(state.scrollViewerMargin);
    }
    if (auto panel = state.panel.get()) {
        panel.LayoutUpdated(state.panelLayoutUpdatedToken);
        if (state.itemMargin >= 0) {
            int count = wuxm::VisualTreeHelper::GetChildrenCount(panel);
            for (int i = 0; i < count; i++) {
                if (auto item = wuxm::VisualTreeHelper::GetChild(panel, i)
                                    .try_as<wuxc::CalendarViewDayItem>()) {
                    auto m = item.Margin();
                    item.Margin({state.itemMargin, m.Top, state.itemMargin, m.Bottom});
                }
            }
        }
    }
    if (auto overlay = state.overlay.get()) {
        overlay.SizeChanged(state.overlaySizeChangedToken);
    }
    if (auto weekDayNames = state.weekDayNames.get()) {
        weekDayNames.Margin(state.weekDayNamesMargin);
    }
    if (auto monthView = state.monthView.get()) {
        auto children = monthView.Children();
        for (uint32_t i = children.Size(); i-- > 0;) {
            if (auto fe = children.GetAt(i).try_as<wux::FrameworkElement>()) {
                auto name = fe.Name();
                if (name == kOverlayName || name == kHeaderName) {
                    children.RemoveAt(i);
                }
            }
        }
    }
}

void CollectCalendars(wux::DependencyObject const& element,
                      std::vector<wuxc::CalendarView>& calendars,
                      size_t& budget,
                      int depth) {
    if (!element || budget == 0 || depth > 96) {
        return;
    }
    budget--;

    if (auto calendar = element.try_as<wuxc::CalendarView>()) {
        calendars.push_back(calendar);
        return;
    }

    int count = wuxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        CollectCalendars(wuxm::VisualTreeHelper::GetChild(element, i), calendars, budget,
                         depth + 1);
    }
}

bool HasLiveCalendar() {
    for (auto const& state : t_calendars) {
        if (auto calendar = state->calendar.get()) {
            if (calendar.IsLoaded()) {
                return true;
            }
        }
    }
    return false;
}

void ScanForCalendars() {
    t_lastScanTick = GetTickCount64();

    auto window = wux::Window::Current();
    if (!window) {
        return;
    }

    // Drop the state of destroyed calendars. Their template parts can outlive
    // them, so the handlers on the parts which are still alive are revoked.
    std::erase_if(t_calendars, [](auto const& state) {
        if (state->calendar.get()) {
            return false;
        }
        try {
            DetachCalendar(*state);
        } catch (...) {
        }
        return true;
    });

    std::vector<wuxc::CalendarView> calendars;
    size_t budget = 20000;
    if (auto content = window.Content()) {
        CollectCalendars(content, calendars, budget, 0);
    }
    for (auto const& popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
        if (auto child = popup.Child()) {
            CollectCalendars(child, calendars, budget, 0);
        }
    }

    for (auto const& calendar : calendars) {
        try {
            AttachCalendar(calendar);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Attaching failed: %08X", static_cast<DWORD>(e.code().value));
        }
    }
}

void RefreshCalendars() {
    for (auto const& state : t_calendars) {
        try {
            ApplySettings(*state);
            Reposition(*state);
        } catch (...) {
        }
    }
}

// How long after the flyout is shown the window is watched for a calendar
// which isn't there yet. Other flyouts of the process never get one, so the
// watch isn't kept armed for good there. The calendar flyout's own thread
// keeps watching, so that a calendar which is collapsed and expanded again
// while the flyout is open gets its column back.
constexpr ULONGLONG kLayoutWatchMs = 5000;
thread_local ULONGLONG t_layoutWatchUntilTick = 0;

void OnLayoutUpdated() {
    // Once the calendar is attached, its own events keep the column up to
    // date. The window is only searched again when there's no calendar, e.g.
    // before it's created or after the flyout was rebuilt.
    const ULONGLONG now = GetTickCount64();
    if ((now > t_layoutWatchUntilTick && !t_hadCalendar) || HasLiveCalendar() ||
        now - t_lastScanTick < 500) {
        return;
    }
    try {
        ScanForCalendars();
    } catch (...) {
    }
}

// Runs on the UI thread whenever the flyout window is shown.
void OnFlyoutShown() {
    t_layoutWatchUntilTick = GetTickCount64() + kLayoutWatchMs;
    try {
        // The regional setting can have changed since.
        {
            const int rule = RegionalFirstWeekRule();
            std::lock_guard lock(g_settingsMutex);
            g_settings.firstWeekRule = rule;
        }
        if (!HasLiveCalendar()) {
            ScanForCalendars();
        }
        RefreshCalendars();
    } catch (...) {
    }
}

void InitializeForCurrentThread() {
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
    }
    if (!window) {
        return;
    }

    try {
        // The flyout window is created once and then only hidden and shown.
        if (!t_coreWindow) {
            if (auto coreWindow = wuc::CoreWindow::GetForCurrentThread()) {
                t_coreWindow = coreWindow;
                t_visibilityChangedToken = coreWindow.VisibilityChanged(
                    [](auto const&, wuc::VisibilityChangedEventArgs const& args) {
                        if (args.Visible()) {
                            OnFlyoutShown();
                        }
                    });
            }
        }

        auto root = window.Content().try_as<wux::FrameworkElement>();
        if (root && t_layoutRoot.get() != root) {
            if (auto oldRoot = t_layoutRoot.get()) {
                oldRoot.LayoutUpdated(t_layoutUpdatedToken);
            }
            t_layoutRoot = winrt::make_weak(root);
            t_layoutUpdatedToken = root.LayoutUpdated(
                [](auto const&, auto const&) { OnLayoutUpdated(); });
        }

        OnFlyoutShown();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Initialization failed: %08X", static_cast<DWORD>(e.code().value));
    }
}

void UninitializeForCurrentThread() {
    if (t_coreWindow) {
        try {
            t_coreWindow.VisibilityChanged(t_visibilityChangedToken);
        } catch (...) {
        }
        t_coreWindow = nullptr;
    }

    if (auto root = t_layoutRoot.get()) {
        try {
            root.LayoutUpdated(t_layoutUpdatedToken);
        } catch (...) {
        }
    }
    t_layoutRoot = nullptr;

    for (auto const& state : t_calendars) {
        try {
            DetachCalendar(*state);
        } catch (...) {
        }
    }
    // Assigned rather than cleared, to free the vector's buffer as well.
    t_calendars = {};
}

////////////////////////////////////////////////////////////////////////////////
// Threading helpers

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

// The windows which can host the calendar: the CoreWindows of
// ShellExperienceHost.
bool IsFlyoutWindow(HWND hWnd) {
    WCHAR className[32];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0;
}

// The current flyout windows of this process, one per thread: the state of
// the mod is per thread. Every thread which can have state of the mod or a
// pending initialization timer has such a window.
std::vector<HWND> GetFlyoutWindows() {
    struct Param {
        std::unordered_set<DWORD> threads;
        std::vector<HWND> windows;
    } param;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& param = *reinterpret_cast<Param*>(lParam);
            DWORD processId = 0;
            DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
            if (threadId && processId == GetCurrentProcessId() &&
                IsFlyoutWindow(hWnd) && param.threads.insert(threadId).second) {
                param.windows.push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return param.windows;
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

std::atomic<bool> g_unloading{false};

// Initialization is deferred to the thread's message loop, so that it doesn't
// run in the middle of whatever is showing the window.
thread_local UINT_PTR t_pendingInitTimer = 0;

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

// Catches flyout windows which are created after the mod was loaded. Once a
// thread is set up, later shows are handled by its CoreWindow's
// VisibilityChanged handler.
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    BOOL result = ShowWindow_Original(hWnd, nCmdShow);

    if (nCmdShow != SW_HIDE && !g_unloading && !t_layoutRoot.get() &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId() &&
        IsFlyoutWindow(hWnd)) {
        if (!t_pendingInitTimer) {
            t_pendingInitTimer =
                SetTimer(nullptr, 0, 50, [](HWND, UINT, UINT_PTR idEvent, DWORD) {
                    KillTimer(nullptr, idEvent);
                    t_pendingInitTimer = 0;
                    if (!g_unloading) {
                        InitializeForCurrentThread();
                    }
                });
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                        &ShowWindow_Original)) {
        Wh_Log(L"Hooking ShowWindow failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    for (HWND hWnd : GetFlyoutWindows()) {
        RunFromWindowThread(hWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    for (HWND hWnd : GetFlyoutWindows()) {
        RunFromWindowThread(
            hWnd,
            [](PVOID) {
                if (t_pendingInitTimer) {
                    KillTimer(nullptr, t_pendingInitTimer);
                    t_pendingInitTimer = 0;
                }
                UninitializeForCurrentThread();
            },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    for (HWND hWnd : GetFlyoutWindows()) {
        RunFromWindowThread(hWnd, [](PVOID) { RefreshCalendars(); }, nullptr);
    }
}
