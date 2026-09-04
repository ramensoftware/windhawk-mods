// ==WindhawkMod==
// @id              windows-11-china-holiday-calendar
// @name            Windows 11 Native China Holiday Calendar
// @name:zh-CN      Windows 11 原生中国节假日日历
// @description     Marks mainland China statutory holidays and make-up workdays in the native Windows 11 calendar while preserving Windows' lunar, festival, and solar-term labels.
// @description:zh-CN 在 Windows 11 原生日历中标记中国法定节假日与调休工作日，并保留 Windows 原生农历、节日和节气文字。
// @version         1.1.0
// @author          Zep
// @github          https://github.com/Zeptol
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Native China Holiday Calendar

Marks mainland China statutory holidays and make-up workdays directly in the
native Windows 11 taskbar calendar.

## Features

- Holiday dates get a soft red background and a red density marker.
- Make-up workdays get a soft amber background and an amber density marker.
- Windows keeps its original day-item template and lunar, traditional-festival,
  and solar-term text; the markers are layered onto the native controls.
- Selecting a date updates the date and Chinese-lunar text in the calendar
  header. This can be disabled in the mod settings.
- Supports both the pre-24H2 `ShellExperienceHost.exe` calendar and the
  24H2-and-later `ShellHost.exe` calendar.

## Holiday data

The mod is self-contained and never accesses the network. Official schedules
for 2024, 2025, and 2026 are bundled from
[NateScarlet/holiday-cn](https://github.com/NateScarlet/holiday-cn), whose data
is compiled from State Council notices. The mod should be updated after the
State Council publishes a new annual schedule.

Dates outside the bundled schedules remain completely native and unmodified.

## Compatibility

The implementation does **not** use XAML Diagnostics. It hooks the native
`CalendarView` lifecycle, then uses public Windows XAML APIs, so it can be
used together with Windows 11 Notification Center Styler and XAML inspection
tools.

The visual markers use `CalendarViewDayItem::SetDensityColors` and local
background values instead of replacing the day-item template. This is what
allows Windows to keep its own lunar/festival/solar-term text.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- syncSelectedDateHeader: true
  $name: Update header for selected date
  $description: Show the selected Gregorian and Chinese-lunar date in the calendar header instead of keeping the header pinned to today.
  $name:zh-CN: 顶部日期跟随选择
  $description:zh-CN: 点击日期后，让日历顶部同步显示所选公历和农历日期，而不是始终显示今天。
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <windhawk_utils.h>

// windows.h defines GetCurrentTime as a macro.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wf = winrt::Windows::Foundation;
namespace wg = winrt::Windows::Globalization;
namespace wui = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wxc = winrt::Windows::UI::Xaml::Controls;
namespace wxm = winrt::Windows::UI::Xaml::Media;

enum class TargetProcess {
    ShellExperienceHost,
    ShellHost,
};

enum class DayKind {
    Normal,
    Holiday,
    MakeUpWorkday,
};

struct HolidayEntry {
    int date;
    DayKind kind;
    wchar_t const* name;
};

// Source: NateScarlet/holiday-cn (MIT), generated from State Council notices.
// Keep this table sorted by date so FindHoliday can use lower_bound.
constexpr HolidayEntry kHolidayEntries[] = {
    {20240101, DayKind::Holiday, L"元旦"},
    {20240204, DayKind::MakeUpWorkday, L"春节"},
    {20240210, DayKind::Holiday, L"春节"},
    {20240211, DayKind::Holiday, L"春节"},
    {20240212, DayKind::Holiday, L"春节"},
    {20240213, DayKind::Holiday, L"春节"},
    {20240214, DayKind::Holiday, L"春节"},
    {20240215, DayKind::Holiday, L"春节"},
    {20240216, DayKind::Holiday, L"春节"},
    {20240217, DayKind::Holiday, L"春节"},
    {20240218, DayKind::MakeUpWorkday, L"春节"},
    {20240404, DayKind::Holiday, L"清明节"},
    {20240405, DayKind::Holiday, L"清明节"},
    {20240406, DayKind::Holiday, L"清明节"},
    {20240407, DayKind::MakeUpWorkday, L"清明节"},
    {20240428, DayKind::MakeUpWorkday, L"劳动节"},
    {20240501, DayKind::Holiday, L"劳动节"},
    {20240502, DayKind::Holiday, L"劳动节"},
    {20240503, DayKind::Holiday, L"劳动节"},
    {20240504, DayKind::Holiday, L"劳动节"},
    {20240505, DayKind::Holiday, L"劳动节"},
    {20240511, DayKind::MakeUpWorkday, L"劳动节"},
    {20240610, DayKind::Holiday, L"端午节"},
    {20240914, DayKind::MakeUpWorkday, L"中秋节"},
    {20240915, DayKind::Holiday, L"中秋节"},
    {20240916, DayKind::Holiday, L"中秋节"},
    {20240917, DayKind::Holiday, L"中秋节"},
    {20240929, DayKind::MakeUpWorkday, L"国庆节"},
    {20241001, DayKind::Holiday, L"国庆节"},
    {20241002, DayKind::Holiday, L"国庆节"},
    {20241003, DayKind::Holiday, L"国庆节"},
    {20241004, DayKind::Holiday, L"国庆节"},
    {20241005, DayKind::Holiday, L"国庆节"},
    {20241006, DayKind::Holiday, L"国庆节"},
    {20241007, DayKind::Holiday, L"国庆节"},
    {20241012, DayKind::MakeUpWorkday, L"国庆节"},

    {20250101, DayKind::Holiday, L"元旦"},
    {20250126, DayKind::MakeUpWorkday, L"春节"},
    {20250128, DayKind::Holiday, L"春节"},
    {20250129, DayKind::Holiday, L"春节"},
    {20250130, DayKind::Holiday, L"春节"},
    {20250131, DayKind::Holiday, L"春节"},
    {20250201, DayKind::Holiday, L"春节"},
    {20250202, DayKind::Holiday, L"春节"},
    {20250203, DayKind::Holiday, L"春节"},
    {20250204, DayKind::Holiday, L"春节"},
    {20250208, DayKind::MakeUpWorkday, L"春节"},
    {20250404, DayKind::Holiday, L"清明节"},
    {20250405, DayKind::Holiday, L"清明节"},
    {20250406, DayKind::Holiday, L"清明节"},
    {20250427, DayKind::MakeUpWorkday, L"劳动节"},
    {20250501, DayKind::Holiday, L"劳动节"},
    {20250502, DayKind::Holiday, L"劳动节"},
    {20250503, DayKind::Holiday, L"劳动节"},
    {20250504, DayKind::Holiday, L"劳动节"},
    {20250505, DayKind::Holiday, L"劳动节"},
    {20250531, DayKind::Holiday, L"端午节"},
    {20250601, DayKind::Holiday, L"端午节"},
    {20250602, DayKind::Holiday, L"端午节"},
    {20250928, DayKind::MakeUpWorkday, L"国庆节、中秋节"},
    {20251001, DayKind::Holiday, L"国庆节、中秋节"},
    {20251002, DayKind::Holiday, L"国庆节、中秋节"},
    {20251003, DayKind::Holiday, L"国庆节、中秋节"},
    {20251004, DayKind::Holiday, L"国庆节、中秋节"},
    {20251005, DayKind::Holiday, L"国庆节、中秋节"},
    {20251006, DayKind::Holiday, L"国庆节、中秋节"},
    {20251007, DayKind::Holiday, L"国庆节、中秋节"},
    {20251008, DayKind::Holiday, L"国庆节、中秋节"},
    {20251011, DayKind::MakeUpWorkday, L"国庆节、中秋节"},

    {20260101, DayKind::Holiday, L"元旦"},
    {20260102, DayKind::Holiday, L"元旦"},
    {20260103, DayKind::Holiday, L"元旦"},
    {20260104, DayKind::MakeUpWorkday, L"元旦"},
    {20260214, DayKind::MakeUpWorkday, L"春节"},
    {20260215, DayKind::Holiday, L"春节"},
    {20260216, DayKind::Holiday, L"春节"},
    {20260217, DayKind::Holiday, L"春节"},
    {20260218, DayKind::Holiday, L"春节"},
    {20260219, DayKind::Holiday, L"春节"},
    {20260220, DayKind::Holiday, L"春节"},
    {20260221, DayKind::Holiday, L"春节"},
    {20260222, DayKind::Holiday, L"春节"},
    {20260223, DayKind::Holiday, L"春节"},
    {20260228, DayKind::MakeUpWorkday, L"春节"},
    {20260404, DayKind::Holiday, L"清明节"},
    {20260405, DayKind::Holiday, L"清明节"},
    {20260406, DayKind::Holiday, L"清明节"},
    {20260501, DayKind::Holiday, L"劳动节"},
    {20260502, DayKind::Holiday, L"劳动节"},
    {20260503, DayKind::Holiday, L"劳动节"},
    {20260504, DayKind::Holiday, L"劳动节"},
    {20260505, DayKind::Holiday, L"劳动节"},
    {20260509, DayKind::MakeUpWorkday, L"劳动节"},
    {20260619, DayKind::Holiday, L"端午节"},
    {20260620, DayKind::Holiday, L"端午节"},
    {20260621, DayKind::Holiday, L"端午节"},
    {20260920, DayKind::MakeUpWorkday, L"国庆节"},
    {20260925, DayKind::Holiday, L"中秋节"},
    {20260926, DayKind::Holiday, L"中秋节"},
    {20260927, DayKind::Holiday, L"中秋节"},
    {20261001, DayKind::Holiday, L"国庆节"},
    {20261002, DayKind::Holiday, L"国庆节"},
    {20261003, DayKind::Holiday, L"国庆节"},
    {20261004, DayKind::Holiday, L"国庆节"},
    {20261005, DayKind::Holiday, L"国庆节"},
    {20261006, DayKind::Holiday, L"国庆节"},
    {20261007, DayKind::Holiday, L"国庆节"},
    {20261010, DayKind::MakeUpWorkday, L"国庆节"},
};

TargetProcess g_target = TargetProcess::ShellExperienceHost;
std::atomic<bool> g_unloading{false};

std::atomic<bool> g_syncSelectedDateHeader{true};

constexpr int DateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

HolidayEntry const* FindHoliday(int date) {
    auto it = std::lower_bound(
        std::begin(kHolidayEntries),
        std::end(kHolidayEntries),
        date,
        [](HolidayEntry const& entry, int value) {
            return entry.date < value;
        });

    return it != std::end(kHolidayEntries) && it->date == date ? it : nullptr;
}

bool DateTimeToLocalSystemTime(wf::DateTime const& dateTime,
                               SYSTEMTIME* localTime) {
    auto rawTicks = dateTime.time_since_epoch().count();
    if (rawTicks < 0) {
        return false;
    }

    ULARGE_INTEGER ticks{};
    ticks.QuadPart = static_cast<ULONGLONG>(rawTicks);

    FILETIME fileTime{
        .dwLowDateTime = ticks.LowPart,
        .dwHighDateTime = ticks.HighPart,
    };

    SYSTEMTIME utcTime{};
    if (!FileTimeToSystemTime(&fileTime, &utcTime)) {
        return false;
    }

    if (!SystemTimeToTzSpecificLocalTime(nullptr, &utcTime, localTime)) {
        *localTime = utcTime;
    }

    return true;
}

HolidayEntry const* FindHoliday(wf::DateTime const& dateTime) {
    SYSTEMTIME localTime{};
    if (!DateTimeToLocalSystemTime(dateTime, &localTime)) {
        return nullptr;
    }

    return FindHoliday(DateKey(localTime.wYear,
                               localTime.wMonth,
                               localTime.wDay));
}

wux::DependencyObject FindNamedDescendant(
    wux::DependencyObject const& root,
    std::wstring_view name,
    int depth = 0) {
    if (!root || depth > 32) {
        return nullptr;
    }

    if (auto element = root.try_as<wux::FrameworkElement>()) {
        if (element.Name() == name) {
            return root;
        }
    }

    int childCount = 0;
    try {
        childCount = wxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < childCount; i++) {
        auto child = wxm::VisualTreeHelper::GetChild(root, i);
        if (auto result = FindNamedDescendant(child, name, depth + 1)) {
            return result;
        }
    }

    return nullptr;
}

struct HeaderTargets {
    winrt::weak_ref<wxc::TextBlock> primary;
    winrt::weak_ref<wxc::TextBlock> secondary;
};

struct TouchedDayItem {
    winrt::weak_ref<wxc::CalendarViewDayItem> item;
};

struct CalendarSubscription {
    winrt::weak_ref<wxc::CalendarView> calendar;
    winrt::event_token dayItemChangingToken{};
    winrt::event_token selectedDatesChangedToken{};
    std::unordered_map<void*, TouchedDayItem> touchedItems;
    HeaderTargets header;
};

struct ThreadState {
    std::unordered_map<void*, CalendarSubscription> subscriptions;
    wxm::SolidColorBrush holidayBackground{nullptr};
    wxm::SolidColorBrush workdayBackground{nullptr};
};

thread_local ThreadState* g_threadState = nullptr;

ThreadState& GetThreadState() {
    if (!g_threadState) {
        g_threadState = new ThreadState;
    }
    return *g_threadState;
}

void EnsureBrushes(ThreadState& state) {
    if (!state.holidayBackground) {
        state.holidayBackground =
            wxm::SolidColorBrush(wui::Color{0x26, 0xFF, 0x4D, 0x4F});
    }

    if (!state.workdayBackground) {
        state.workdayBackground =
            wxm::SolidColorBrush(wui::Color{0x22, 0xF5, 0x9E, 0x0B});
    }
}

void ClearDensityColors(wxc::CalendarViewDayItem const& item) {
    auto colors = winrt::single_threaded_vector<wui::Color>();
    item.SetDensityColors(colors);
}

void RestoreDayItem(wxc::CalendarViewDayItem const& item) {
    if (!item) {
        return;
    }

    try {
        item.ClearValue(wxc::Control::BackgroundProperty());
        item.ClearValue(wxc::ToolTipService::ToolTipProperty());
        ClearDensityColors(item);
    } catch (...) {
    }
}

void RestoreTouchedItems(CalendarSubscription& subscription) {
    for (auto const& [key, touched] : subscription.touchedItems) {
        if (auto item = touched.item.get()) {
            RestoreDayItem(item);
        }
    }

    subscription.touchedItems.clear();
}

std::wstring HolidayToolTip(HolidayEntry const& entry) {
    std::wstring text = entry.name;
    text += entry.kind == DayKind::Holiday ? L" · 休息日"
                                            : L" · 调休工作日";
    return text;
}

void ApplyDayStyle(void* calendarKey,
                   wxc::CalendarViewDayItem const& item) {
    if (!item || !g_threadState) {
        return;
    }

    auto subscriptionIt = g_threadState->subscriptions.find(calendarKey);
    if (subscriptionIt == g_threadState->subscriptions.end()) {
        return;
    }

    auto& subscription = subscriptionIt->second;
    void* itemKey = winrt::get_abi(item);
    auto entry = FindHoliday(item.Date());

    if (!entry) {
        if (subscription.touchedItems.erase(itemKey)) {
            RestoreDayItem(item);
        }
        return;
    }

    EnsureBrushes(*g_threadState);

    item.Background(entry->kind == DayKind::Holiday
                        ? g_threadState->holidayBackground
                        : g_threadState->workdayBackground);

    auto densityColors = winrt::single_threaded_vector<wui::Color>();
    densityColors.Append(
        entry->kind == DayKind::Holiday
            ? wui::Color{0xFF, 0xF0, 0x44, 0x44}
            : wui::Color{0xFF, 0xD9, 0x8A, 0x16});
    item.SetDensityColors(densityColors);

    wxc::ToolTipService::SetToolTip(
        item,
        winrt::box_value(winrt::hstring{HolidayToolTip(*entry)}));

    subscription.touchedItems.insert_or_assign(
        itemKey,
        TouchedDayItem{winrt::make_weak(item)});
}

void RefreshDayItems(void* calendarKey,
                     wux::DependencyObject const& root,
                     int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    if (auto dayItem = root.try_as<wxc::CalendarViewDayItem>()) {
        ApplyDayStyle(calendarKey, dayItem);
    }

    int childCount = 0;
    try {
        childCount = wxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < childCount; i++) {
        RefreshDayItems(calendarKey,
                        wxm::VisualTreeHelper::GetChild(root, i),
                        depth + 1);
    }
}

wxc::TextBlock TextBlockFromButton(
    wux::DependencyObject const& root,
    std::wstring_view buttonName) {
    auto object = FindNamedDescendant(root, buttonName);
    auto button = object.try_as<wxc::Button>();
    if (!button) {
        return nullptr;
    }

    return button.Content().try_as<wxc::TextBlock>();
}

HeaderTargets ResolveHeaderTargets(wxc::CalendarView const& calendar) {
    HeaderTargets targets;

    auto xamlRoot = calendar.XamlRoot();
    if (!xamlRoot) {
        return targets;
    }

    auto root = xamlRoot.Content();
    if (!root) {
        return targets;
    }

    auto primary = TextBlockFromButton(root, L"DateTextButtonWithClock");
    if (!primary) {
        primary = TextBlockFromButton(root, L"DateTextButton");
    }

    auto secondary =
        FindNamedDescendant(root, L"CurrentLunarDateTextBlock")
            .try_as<wxc::TextBlock>();

    if (primary) {
        targets.primary = winrt::make_weak(primary);
    }
    if (secondary) {
        targets.secondary = winrt::make_weak(secondary);
    }

    return targets;
}

void RestoreHeader(HeaderTargets& targets) {
    try {
        if (auto primary = targets.primary.get()) {
            primary.ClearValue(wxc::TextBlock::TextProperty());
        }
    } catch (...) {
    }

    try {
        if (auto secondary = targets.secondary.get()) {
            secondary.ClearValue(wxc::TextBlock::TextProperty());
        }
    } catch (...) {
    }

    targets = {};
}

std::wstring WeekdayText(WORD dayOfWeek) {
    constexpr wchar_t const* weekdays[] = {
        L"星期日", L"星期一", L"星期二", L"星期三",
        L"星期四", L"星期五", L"星期六",
    };

    return weekdays[dayOfWeek % 7];
}

std::wstring PrimaryHeaderText(wf::DateTime const& dateTime) {
    SYSTEMTIME localTime{};
    if (!DateTimeToLocalSystemTime(dateTime, &localTime)) {
        return {};
    }

    wchar_t text[64]{};
    swprintf_s(text,
               L"%u月%u日, %s",
               localTime.wMonth,
               localTime.wDay,
               WeekdayText(localTime.wDayOfWeek).c_str());
    return text;
}

std::wstring LunarHeaderText(wf::DateTime const& dateTime) {
    try {
        auto languages =
            winrt::single_threaded_vector<winrt::hstring>({L"zh-CN"});
        wg::Calendar calendar(languages);
        calendar.ChangeCalendarSystem(
            wg::CalendarIdentifiers::ChineseLunar());
        calendar.SetDateTime(dateTime);

        // Let Windows format the actual lunar month. In leap years this
        // correctly returns strings such as "闰六月", with no shifted indexes.
        std::wstring text = calendar.MonthAsSoloString().c_str();
        text += calendar.DayAsString().c_str();
        return text;
    } catch (...) {
        return {};
    }
}

void UpdateHeader(void* calendarKey,
                  wxc::CalendarView const& calendar,
                  wf::DateTime const& dateTime) {
    if (!g_syncSelectedDateHeader.load() || !g_threadState) {
        return;
    }

    auto it = g_threadState->subscriptions.find(calendarKey);
    if (it == g_threadState->subscriptions.end()) {
        return;
    }

    auto& targets = it->second.header;
    if (!targets.primary.get() || !targets.secondary.get()) {
        targets = ResolveHeaderTargets(calendar);
    }

    if (auto primary = targets.primary.get()) {
        auto text = PrimaryHeaderText(dateTime);
        if (!text.empty()) {
            primary.Text(text);
        }
    }

    if (auto secondary = targets.secondary.get()) {
        auto text = LunarHeaderText(dateTime);
        if (!text.empty()) {
            secondary.Text(text);
        }
    }
}

void PruneSubscriptions() {
    if (!g_threadState) {
        return;
    }

    for (auto it = g_threadState->subscriptions.begin();
         it != g_threadState->subscriptions.end();) {
        if (!it->second.calendar.get()) {
            it = g_threadState->subscriptions.erase(it);
        } else {
            ++it;
        }
    }
}

void RegisterCalendar(wxc::CalendarView const& calendar) {
    if (!calendar || g_unloading.load()) {
        return;
    }

    // Only touch the taskbar calendar. Other CalendarView controls which might
    // exist in the shell process retain their original behavior.
    if (calendar.Name() != L"CalendarControl") {
        return;
    }

    PruneSubscriptions();

    auto& state = GetThreadState();
    void* calendarKey = winrt::get_abi(calendar);
    if (state.subscriptions.contains(calendarKey)) {
        return;
    }

    CalendarSubscription subscription;
    subscription.calendar = winrt::make_weak(calendar);

    subscription.dayItemChangingToken =
        calendar.CalendarViewDayItemChanging(
            [calendarKey](
                wxc::CalendarView const&,
                wxc::CalendarViewDayItemChangingEventArgs const& args) {
                if (g_unloading.load()) {
                    return;
                }

                try {
                    auto item = args.Item();
                    if (!item) {
                        return;
                    }

                    if (args.InRecycleQueue()) {
                        if (g_threadState) {
                            auto it =
                                g_threadState->subscriptions.find(calendarKey);
                            if (it !=
                                g_threadState->subscriptions.end()) {
                                it->second.touchedItems.erase(
                                    winrt::get_abi(item));
                            }
                        }
                        RestoreDayItem(item);
                    } else {
                        ApplyDayStyle(calendarKey, item);
                    }
                } catch (winrt::hresult_error const& error) {
                    Wh_Log(L"Day item callback failed: %08X %s",
                           error.code().value,
                           error.message().c_str());
                } catch (...) {
                    Wh_Log(L"Day item callback failed");
                }
            });

    subscription.selectedDatesChangedToken =
        calendar.SelectedDatesChanged(
            [calendarKey](
                wxc::CalendarView const& sender,
                wxc::CalendarViewSelectedDatesChangedEventArgs const& args) {
                if (g_unloading.load()) {
                    return;
                }

                try {
                    auto addedDates = args.AddedDates();
                    if (addedDates.Size() > 0) {
                        UpdateHeader(calendarKey,
                                     sender,
                                     addedDates.GetAt(0));
                        return;
                    }

                    auto selectedDates = sender.SelectedDates();
                    if (selectedDates.Size() > 0) {
                        UpdateHeader(calendarKey,
                                     sender,
                                     selectedDates.GetAt(0));
                    }
                } catch (winrt::hresult_error const& error) {
                    Wh_Log(L"Selected date callback failed: %08X %s",
                           error.code().value,
                           error.message().c_str());
                } catch (...) {
                    Wh_Log(L"Selected date callback failed");
                }
            });

    state.subscriptions.emplace(calendarKey, std::move(subscription));

    // OnApplyTemplate can run after containers already exist. Style the current
    // month immediately; future/recycled containers come through the event.
    RefreshDayItems(calendarKey, calendar);

    Wh_Log(L"Registered taskbar CalendarView on thread %u",
           GetCurrentThreadId());
}

wxc::CalendarView TryGetCalendarFromThis(void* pThis) {
    if (!pThis) {
        return nullptr;
    }

    wxc::CalendarView calendar{nullptr};
    reinterpret_cast<::IUnknown*>(pThis)->QueryInterface(
        winrt::guid_of<wxc::CalendarView>(),
        winrt::put_abi(calendar));
    return calendar;
}

void TryRegisterCalendarFromThis(void* pThis) {
    if (g_unloading.load()) {
        return;
    }

    try {
        if (auto calendar = TryGetCalendarFromThis(pThis)) {
            RegisterCalendar(calendar);
        }
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Calendar discovery failed: %08X %s",
               error.code().value,
               error.message().c_str());
    } catch (...) {
        Wh_Log(L"Calendar discovery failed");
    }
}

// Windows.UI.Xaml's own CalendarView lifecycle is a narrow, non-diagnostic
// discovery point. OnApplyTemplate handles creation/recreation; MeasureOverride
// also picks up a CalendarView which existed before the mod was enabled.
using CalendarView_OnApplyTemplate_t = HRESULT(WINAPI*)(void* pThis);
CalendarView_OnApplyTemplate_t CalendarView_OnApplyTemplate_Original;

HRESULT WINAPI CalendarView_OnApplyTemplate_Hook(void* pThis) {
    HRESULT result = CalendarView_OnApplyTemplate_Original(pThis);
    if (SUCCEEDED(result)) {
        TryRegisterCalendarFromThis(pThis);
    }
    return result;
}

using CalendarView_MeasureOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     wf::Size availableSize,
                     wf::Size* desiredSize);
CalendarView_MeasureOverride_t CalendarView_MeasureOverride_Original;

HRESULT WINAPI CalendarView_MeasureOverride_Hook(
    void* pThis,
    wf::Size availableSize,
    wf::Size* desiredSize) {
    HRESULT result =
        CalendarView_MeasureOverride_Original(
            pThis,
            availableSize,
            desiredSize);
    if (SUCCEEDED(result)) {
        TryRegisterCalendarFromThis(pThis);
    }
    return result;
}

bool HookCalendarViewLifecycle() {
    HMODULE xamlModule = LoadLibraryExW(
        L"Windows.UI.Xaml.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xamlModule) {
        Wh_Log(L"Failed to load Windows.UI.Xaml.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: virtual long __cdecl DirectUI::CalendarView::OnApplyTemplate(void))",
                LR"(public: virtual long __cdecl DirectUI::CalendarView::OnApplyTemplate(void) __ptr64)",
                LR"(public: virtual int __cdecl DirectUI::CalendarView::OnApplyTemplate(void))",
            },
            &CalendarView_OnApplyTemplate_Original,
            CalendarView_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(protected: virtual long __cdecl DirectUI::CalendarView::MeasureOverride(struct ABI::Windows::Foundation::Size,struct ABI::Windows::Foundation::Size *))",
                LR"(protected: virtual long __cdecl DirectUI::CalendarView::MeasureOverride(struct ABI::Windows::Foundation::Size,struct ABI::Windows::Foundation::Size * __ptr64) __ptr64)",
                LR"(public: virtual long __cdecl DirectUI::CalendarView::MeasureOverride(struct ABI::Windows::Foundation::Size,struct ABI::Windows::Foundation::Size *))",
                LR"(protected: virtual int __cdecl DirectUI::CalendarView::MeasureOverride(struct ABI::Windows::Foundation::Size,struct ABI::Windows::Foundation::Size *))",
            },
            &CalendarView_MeasureOverride_Original,
            CalendarView_MeasureOverride_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            xamlModule,
            hooks,
            ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to resolve Windows.UI.Xaml CalendarView symbols");
        return false;
    }

    if (!CalendarView_OnApplyTemplate_Original &&
        !CalendarView_MeasureOverride_Original) {
        Wh_Log(L"No compatible CalendarView lifecycle symbol was found");
        return false;
    }

    return true;
}

void CollectCalendars(wux::DependencyObject const& root,
                      std::vector<wxc::CalendarView>* calendars,
                      int depth = 0) {
    if (!root || depth > 32) {
        return;
    }

    if (auto calendar = root.try_as<wxc::CalendarView>()) {
        if (calendar.Name() == L"CalendarControl") {
            calendars->push_back(calendar);
        }
    }

    int childCount = 0;
    try {
        childCount = wxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < childCount; i++) {
        CollectCalendars(wxm::VisualTreeHelper::GetChild(root, i),
                         calendars,
                         depth + 1);
    }
}

void ScanCurrentThreadWindow() {
    try {
        auto window = wux::Window::Current();
        if (!window || !window.Content()) {
            return;
        }

        std::vector<wxc::CalendarView> calendars;
        CollectCalendars(window.Content(), &calendars);
        for (auto const& calendar : calendars) {
            RegisterCalendar(calendar);
        }
    } catch (...) {
        // ShellHost uses a desktop XAML island on some builds, where
        // Window::Current is unavailable. The lifecycle hooks still discover
        // the CalendarView directly.
    }
}

void UnregisterAllCalendarsForCurrentThread() {
    if (!g_threadState) {
        return;
    }

    for (auto& [calendarKey, subscription] :
         g_threadState->subscriptions) {
        RestoreHeader(subscription.header);
        RestoreTouchedItems(subscription);

        if (auto calendar = subscription.calendar.get()) {
            try {
                calendar.CalendarViewDayItemChanging(
                    subscription.dayItemChangingToken);
            } catch (...) {
            }

            try {
                calendar.SelectedDatesChanged(
                    subscription.selectedDatesChangedToken);
            } catch (...) {
            }
        }
    }

    g_threadState->subscriptions.clear();
    delete g_threadState;
    g_threadState = nullptr;
}

bool IsTargetWindow(HWND window) {
    DWORD processId = 0;
    if (!window ||
        !GetWindowThreadProcessId(window, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    wchar_t className[64]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return false;
    }

    return g_target == TargetProcess::ShellHost
               ? _wcsicmp(className, L"ControlCenterWindow") == 0
               : _wcsicmp(className,
                          L"Windows.UI.Core.CoreWindow") == 0;
}

std::vector<HWND> GetTargetWindows() {
    std::vector<HWND> windows;

    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            if (IsTargetWindow(window)) {
                reinterpret_cast<std::vector<HWND>*>(parameter)
                    ->push_back(window);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));

    return windows;
}

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc proc,
                         void* procParameter) {
    static UINT const message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct HookParameter {
        RunFromWindowThreadProc proc;
        void* procParameter;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM parameter) -> LRESULT {
            if (code == HC_ACTION) {
                auto messageData =
                    reinterpret_cast<CWPSTRUCT*>(parameter);
                if (messageData->message == message) {
                    auto hookParameter =
                        reinterpret_cast<HookParameter*>(
                            messageData->lParam);
                    hookParameter->proc(
                        hookParameter->procParameter);
                }
            }

            return CallNextHookEx(nullptr, code, wParam, parameter);
        },
        nullptr,
        threadId);
    if (!hook) {
        return false;
    }

    HookParameter hookParameter{proc, procParameter};
    DWORD_PTR ignored = 0;
    BOOL sent = SendMessageTimeoutW(
        window,
        message,
        0,
        reinterpret_cast<LPARAM>(&hookParameter),
        SMTO_ABORTIFHUNG | SMTO_BLOCK,
        2000,
        &ignored);

    UnhookWindowsHookEx(hook);
    return sent != 0;
}

void LoadSettings() {
    g_syncSelectedDateHeader =
        Wh_GetIntSetting(L"syncSelectedDateHeader") != 0;
}

BOOL Wh_ModInit() {
    wchar_t processPath[MAX_PATH]{};
    DWORD length =
        GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    if (!length || length == ARRAYSIZE(processPath)) {
        return FALSE;
    }

    wchar_t const* fileName = wcsrchr(processPath, L'\\');
    fileName = fileName ? fileName + 1 : processPath;
    if (_wcsicmp(fileName, L"ShellHost.exe") == 0) {
        g_target = TargetProcess::ShellHost;
    } else if (_wcsicmp(fileName, L"ShellExperienceHost.exe") == 0) {
        g_target = TargetProcess::ShellExperienceHost;
    } else {
        return FALSE;
    }

    LoadSettings();
    g_unloading = false;

    Wh_Log(L"Initializing China holiday calendar 1.1.0 in %s",
           fileName);

    return HookCalendarViewLifecycle();
}

void Wh_ModAfterInit() {
    for (HWND window : GetTargetWindows()) {
        RunFromWindowThread(
            window,
            [](void*) { ScanCurrentThreadWindow(); },
            nullptr);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;

    for (HWND window : GetTargetWindows()) {
        RunFromWindowThread(
            window,
            [](void*) {
                UnregisterAllCalendarsForCurrentThread();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitialized China holiday calendar 1.1.0");
}

BOOL Wh_ModSettingsChanged(BOOL*) {
    bool previousHeaderSync = g_syncSelectedDateHeader.load();
    LoadSettings();

    if (previousHeaderSync &&
        !g_syncSelectedDateHeader.load()) {
        for (HWND window : GetTargetWindows()) {
            RunFromWindowThread(
                window,
                [](void*) {
                    if (!g_threadState) {
                        return;
                    }

                    for (auto& [key, subscription] :
                         g_threadState->subscriptions) {
                        RestoreHeader(subscription.header);
                    }
                },
                nullptr);
        }
    }

    return TRUE;
}
