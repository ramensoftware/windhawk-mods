// ==WindhawkMod==
// @id              calendar-agenda-plus
// @name            Windows Calendar Plus
// @description     Native Windows calendar agenda with Google accounts, ICS feeds, daylight saving handling and reminders.
// @version         0.2.1
// @author          Jip (fork of lonfro and Rubens Nascimento)
// @github          https://github.com/iiRedPanda
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lntdll -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lwinhttp -lbcrypt -lcrypt32 -lws2_32 -ladvapi32 -lole32 -loleaut32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Modified fork; changes and upstream attribution are described in the embedded README.

// ==WindhawkModReadme==
/*
# Windows Calendar Plus

Display Google Calendar and ICS events in the native Windows calendar pane, with account management, refresh controls, and optional reminders.

## Features

- Read-only Google Calendar access with up to four accounts.
- HTTPS and webcal ICS feeds, with optional feed labels.
- Date-aware timezone conversion and supported recurring events.
- Configurable event range, reminders, and refresh controls.
- Optional companion integration with Desktop Island for a passive event preview.

## Usage

Configure a Google account or ICS feed as described below, then open the Windows calendar pane to view events. The three-dot menu provides **Google accounts**, **Refresh events**, and **Go to today**. Reminder preferences and the downloaded date range are configured in the mod's settings.

## Compatibility and status

Experimental Windhawk fork. Compilation and calendar parser/cache regression tests have passed; a full live Windows compatibility matrix has not been established. The original Agenda in Calendar View and Tray Agenda can produce duplicate controls or reminders when enabled concurrently with this fork.

## Google Calendar

Use a Google Cloud Desktop-app OAuth client with Calendar API enabled. Paste its client ID and secret into settings, then open the Windows calendar pane and select the three-dot menu > Google accounts > Sign in with Google. Authentication uses the system browser. Use the same menu to see connected accounts and sign out. Calendar access is read-only; up to four accounts are supported. Calendar IDs are optional; otherwise selected calendars are used. Existing Tray Agenda sign-ins are separate and are not copied.

Alternatively paste a Google Calendar secret iCal address under ICS feed URLs. Use Label|https://... to name a feed. Outlook HTTPS feeds and webcal links also work. Local ICS files are not supported by this provider. Never post private feed URLs publicly.

## Timezones

Timed events are resolved by date-aware IANA/Windows timezone rules, then displayed in the Windows timezone. Recurring instances are expanded before display, so each date has its own daylight saving conversion. All-day end dates remain exclusive. Manual correction defaults to zero and applies only to timed events in the native pane; use it only for a broken feed. Unknown named timezones and unsupported recurrence rules are skipped with a partial-data warning rather than guessed. Custom VTIMEZONE definitions are not interpreted; their TZID must match a known IANA or Windows zone. The ICS expander supports DAILY, WEEKLY, MONTHLY and basic YEARLY rules, COUNT/UNTIL, BYDAY/BYMONTHDAY/BYMONTH, WKST, EXDATE/RDATE and individual overrides. BYSETPOS and other advanced rules are unsupported.

## Range and refresh

Default range is 30 days back and 90 days ahead, configurable up to 90/180. Provider limits: 16 calendars, 2000 events per calendar, 4000 events total, 512 occurrences per ICS event. Dates outside the downloaded range can be empty. The native calendar reads updates every 1.5 seconds while its process is running. The three-dot menu offers Refresh events (normally within 15 seconds, plus network time) and Go to today. Account requests are also handled by the provider in Explorer, normally within 15 seconds. Reopen Google accounts to see the latest account list and sign-in status. Partial/stale states are shown. Shared snapshots are limited to 32 million UTF-16 code units; a larger or unreadable snapshot retains previously readable data and shows an error.

## Privacy

Google sign-in runs in Explorer with the system browser, loopback redirect and PKCE. Tokens are DPAPI encrypted; no tokens are sent to the calendar process. The mod caches event titles, descriptions, locations and meeting links in per-Windows-user-named Windhawk storage so ShellHost can read them through Windhawk. This cache is local and is not encrypted. Signing out clears the shared event cache and prevents an in-flight fetch from restoring the removed account's events. Remaining accounts and configured ICS feeds are then fetched again.

## Desktop Island preview
When **Share event preview with Desktop Island** is enabled, the active provider writes a small, unencrypted, per-user snapshot at `%LOCALAPPDATA%\WindhawkPersonal\calendar-island-v1.tsv`. It contains up to six event titles, start/end times and all-day flags, with an 8 KB limit. It contains no credentials, event descriptions or meeting links. Desktop Island reads it only when its Calendar tab is enabled. Sharing is disabled by default. Disabling it removes the snapshot. Clearing the event cache or unloading the active provider removes the preview. The preview adds no separate reminders.

## Reminders

Reminder settings apply to the calendar provider. Disable reminders if another app already supplies them. Google reminders are for accepted timed events; ICS reminders can be toggled separately. This fork uses its own Windows Calendar Plus notification identity.

## Credits and license
Native calendar code: Copyright (C) 2026 lonfro and m417z, GPL-3.0. Calendar provider code adapted from Tray Agenda: Rubens Nascimento (chambber), MIT. The combined fork is distributed under GPL-3.0-only, retaining the original notices.

MIT License (Tray Agenda portions)
Copyright (c) 2026 Rubens Nascimento
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the Software), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enabled
  $description: Enable calendar fetching, the agenda and reminders.
- google_client_id: ""
  $name: Google OAuth client ID
  $description: Client ID of your own Google Cloud "Desktop app" OAuth client (see the mod description).
- google_client_secret: ""
  $name: Google OAuth client secret
  $description: Client secret of the same OAuth client.
- calendar_ids: [""]
  $name: Calendar IDs (optional)
  $description: Calendars to show. Leave empty to use the calendars ticked in Google Calendar.
- ics_feeds: [""]
  $name: ICS feed URLs (optional)
  $description: Secret iCal/ICS URLs (Google, Outlook, ...), one per item, as "Label|https://..." or just the URL. Stored as plain text in Windhawk settings. Up to 8 feeds.
- ics_notifications: true
  $name: Remind for ICS feed events
  $description: ICS feeds carry no accept/decline status, so their timed events are treated as accepted for reminders.
- poll_seconds: 300
  $name: Refresh interval, seconds
  $description: How often Google Calendar and the ICS feeds are read. Values below 60 seconds are raised to 60.
- island_preview_enabled: false
  $name: Share event preview with Desktop Island
  $description: Write a local unencrypted preview with up to six event titles and times. Disable to remove the preview file. Desktop Island is optional.
- notifications_enabled: true
  $name: Reminder notifications
  $description: Show a native Windows notification before and at the start of accepted events.
- notify_lead_minutes: 10
  $name: Reminder lead time, minutes
  $description: How long before an event the first reminder is shown.
- show_location: true
  $name: Show location
  $description: Show the event location in the calendar pane when available.
- use_24_hour_time: true
  $name: Use 24-hour time
  $description: Use 14:00 instead of 2:00 PM.
- max_snapshot_age_seconds: 1800
  $name: Maximum data age, seconds
  $description: Calendar data older than this is shown as stale (for example when Google cannot be reached).
- history_days: 30
  $name: Calendar history, days
  $description: Fetch events this many days back (1-90).
- future_days: 90
  $name: Calendar future range, days
  $description: Fetch events this many days ahead (1-180).
- show_calendar_name: true
  $name: Show calendar name in native pane
- show_description: true
  $name: Show event description in native pane
- time_offset_minutes: 0
  $name: Manual time correction in native pane, minutes
  $description: Keep at 0 for normal Google and ICS calendars. Applies only to timed events in the native pane (-1440 to 1440).
- calendarPosition: above
  $name: Calendar position
  $description: Show calendar above or below the agenda
  $options:
  - above: Show calendar above agenda
  - below: Show calendar below agenda
- hideCompletedEvents: false
  $name: Hide completed events
  $description: |
    Hide events upon completion time. Events without ending times are hidden on the next day.
- groupingMode: inline
  $name: Calendar pane mode
  $description: |
    When in inline mode, the calendar pane is shown in its own row, rather than as a popup.
    Note that this may cause the notifications pane to not be visible on shorter monitors. You can mitigate this by setting a constraint on the height of the calendar pane when it is expanded.
  $options:
  - inline: "Inline: show in separate row"
  - popup: "Popup: show as dismissable overlay"
- maxHeightCollapsed: 0
  $name: Max height of agenda (calendar collapsed/popup)
  $description: |
    Maximum visible height of the events list in pixels before the list starts to scroll. Set to 0 for no limit.
    This constraint applies for when the calendar is collapsed in inline mode or when the calendar is in popup mode.
- maxHeightExpanded: 200
  $name: Max height of agenda (calendar expanded)
  $description: |
    Maximum visible height of the events list in pixels before the list starts to scroll. Set to 0 for no limit.
    This constraint applies for when the calendar is expanded in inline mode. It has no effect when the calendar is in popup mode.
- timeColumnWidth: 65
  $name: Time column width (in pixels)
  $description: |
    Width of the time column in pixels to keep event titles aligned across cards.
    Set to 0 for automatic width.
- hideFocusSession: true
  $name: Hide Focus Session
  $description: Hide the Focus Session control in the calendar/notification center flyout.
- keyboardShortcuts: true
  $name: Keyboard shortcuts
  $description: Use left and right arrow keys to move backward and forward by a single day respectively
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <bcrypt.h>
#include <objbase.h>
#include <dpapi.h>
#include <sddl.h>
#include <shellapi.h>
#include <winhttp.h>
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/base.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#undef GetCurrentTime
#include <winrt/Windows.Data.Json.h>
#undef GetCurrentTime
#include <winrt/Windows.Data.Xml.Dom.h>
#undef GetCurrentTime
#include <winrt/Windows.Globalization.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Notifications.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Core.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Text.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.ViewManagement.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Controls.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Hosting.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Interop.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Markup.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Input.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Media.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <ctime>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <thread>
#include <utility>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#undef GetCurrentTime
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#undef GetCurrentTime
#include <winrt/Windows.System.h>
#undef GetCurrentTime
namespace calendar_provider {






// Windows 11 only. The calendar provider talks to Google Calendar over REST
// (OAuth 2.0 loopback + PKCE, WinHTTP) on dedicated worker threads.

#undef GetCurrentTime




using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Media;

namespace {

constexpr int64_t kUnixToFileTimeTicks = 116444736000000000LL;
constexpr int64_t kFileTimeTicksPerSecond = 10000000LL;
constexpr int64_t kSecondsPerMinute = 60;
constexpr int64_t kSecondsPerHour = 60 * kSecondsPerMinute;
constexpr int64_t kSecondsPerDay = 24 * kSecondsPerHour;

struct IcsFeedConfig {
    std::wstring label;
    std::wstring url;
};

struct ModSettings {
    bool enabled = true;
    bool island_preview_enabled = false;
    std::wstring google_client_id;
    std::wstring google_client_secret;
    std::vector<std::wstring> calendar_ids;
    std::vector<IcsFeedConfig> ics_feeds;
    bool ics_notifications = true;
    int poll_seconds = 300;
    bool notifications_enabled = true;
    int notify_lead_minutes = 10;
    bool use_24_hour_time = true;
};

struct AgendaEntry {
    std::wstring title;
    std::wstring description;
    std::wstring location;
    std::wstring source;
    int64_t startUnix = 0;
    int64_t endUnix = 0;
    bool isActive = false;
    bool allDay = false;
    std::wstring googleMeetCode;
    bool hasMeetCode = false;
    enum class ResponseState {
        Neutral,
        NeedsResponse,
        Accepted,
        Declined,
        Tentative,
    } responseState = ResponseState::Neutral;
    bool hasResponseState = false;
    std::wstring meetingProvider;
    std::wstring meetingUrl;
    bool hasMeetingProvider = false;
    bool hasMeetingUrl = false;
    bool outOfOffice = false;
    bool fromIcs = false;
    std::wstring dedupKey;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

std::atomic<bool> g_hasPublishedEvents{false};
std::mutex g_publishMutex;
std::atomic<uint64_t> g_accountGeneration{0};

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_workerStop{false};
HANDLE g_workerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_workerThread;
[[clang::no_destroy]] std::optional<std::thread> g_startupThread;

HWND FindCurrentProcessTaskbarWnd();

std::wstring ReadStringSetting(PCWSTR name, PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    if (!value) {
        return fallback;
    }

    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return result.empty() ? std::wstring(fallback) : result;
}

std::wstring Trim(std::wstring text);

void LoadSettings() {
    ModSettings s;
    s.enabled = Wh_GetIntSetting(L"enabled") != 0;
    s.island_preview_enabled = Wh_GetIntSetting(L"island_preview_enabled") != 0;
    s.google_client_id = Trim(ReadStringSetting(L"google_client_id", L""));
    s.google_client_secret = Trim(ReadStringSetting(L"google_client_secret", L""));
    auto printableAscii = [](const std::wstring& v, size_t maxLen) {
        if (v.empty() || v.size() > maxLen) return false;
        for (wchar_t ch : v) {
            if (ch < 0x21 || ch > 0x7E) return false;
        }
        return true;
    };
    if (!printableAscii(s.google_client_id, 256)) s.google_client_id.clear();
    if (!printableAscii(s.google_client_secret, 256)) s.google_client_secret.clear();
    for (int i = 0; i < 16; ++i) {
        PCWSTR value = Wh_GetStringSetting(L"calendar_ids[%d]", i);
        if (!value) break;
        std::wstring id = Trim(value);
        Wh_FreeStringSetting(value);
        if (id.empty()) continue;
        if (id.size() <= 256 && printableAscii(id, 256)) s.calendar_ids.push_back(std::move(id));
    }
    for (int i = 0; i < 8; ++i) {
        PCWSTR value = Wh_GetStringSetting(L"ics_feeds[%d]", i);
        if (!value) break;
        std::wstring item = Trim(value);
        Wh_FreeStringSetting(value);
        if (item.empty()) continue;
        IcsFeedConfig feed;
        size_t bar = item.find(L'|');
        if (bar != std::wstring::npos) {
            feed.label = Trim(item.substr(0, bar));
            feed.url = Trim(item.substr(bar + 1));
        } else {
            feed.url = item;
        }
        if (feed.url.size() > 2048) continue;
        if (feed.label.empty()) feed.label = L"Calendar feed " + std::to_wstring(s.ics_feeds.size() + 1);
        if (feed.label.size() > 96) feed.label.resize(96);
        s.ics_feeds.push_back(std::move(feed));
    }
    s.ics_notifications = Wh_GetIntSetting(L"ics_notifications") != 0;
    s.poll_seconds = std::clamp(Wh_GetIntSetting(L"poll_seconds"), 60, 24 * 60 * 60);
    s.notifications_enabled = Wh_GetIntSetting(L"notifications_enabled") != 0;
    s.notify_lead_minutes = std::clamp(Wh_GetIntSetting(L"notify_lead_minutes"), 1, 60);
    s.use_24_hour_time = Wh_GetIntSetting(L"use_24_hour_time") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

ModSettings SettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring HResultHex(HRESULT hr) {
    wchar_t buffer[32]{};
    std::swprintf(buffer, ARRAYSIZE(buffer), L"0x%08lX", static_cast<unsigned long>(hr));
    return buffer;
}

void LogCaughtException(PCWSTR context) {
    try {
        throw;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"%s: C++/WinRT exception %s: %s", context,
               HResultHex(static_cast<HRESULT>(e.code())).c_str(),
               e.message().c_str());
    } catch (...) {
        Wh_Log(L"%s: unexpected exception", context);
    }
}

int64_t NowUnix() {
    std::time_t now = std::time(nullptr);
    if (now <= 0) {
        return 0;
    }
    return static_cast<int64_t>(now);
}

std::wstring Trim(std::wstring text) {
    auto isSpace = [](wchar_t ch) { return std::iswspace(ch) != 0; };
    while (!text.empty() && isSpace(text.front())) {
        text.erase(text.begin());
    }
    while (!text.empty() && isSpace(text.back())) {
        text.pop_back();
    }
    return text;
}

std::wstring SanitizeUiText(std::wstring text, size_t maxCharacters) {
    std::wstring sanitized;
    sanitized.reserve(std::min(text.size(), maxCharacters + 1));
    bool previousSpace = false;
    for (wchar_t ch : text) {
        bool makeSpace = ch == L'\r' || ch == L'\n' || ch == L'\t';
        bool drop = (ch < 0x20 && !makeSpace) || ch == 0x7f;
        if (drop) {
            continue;
        }

        if (makeSpace || std::iswspace(ch)) {
            if (!previousSpace && !sanitized.empty()) {
                sanitized.push_back(L' ');
                previousSpace = true;
            }
        } else {
            sanitized.push_back(ch);
            previousSpace = false;
        }

        if (sanitized.size() > maxCharacters) {
            break;
        }
    }

    sanitized = Trim(std::move(sanitized));
    if (sanitized.size() > maxCharacters) {
        if (maxCharacters >= 3) {
            sanitized.resize(maxCharacters - 3);
            sanitized += L"...";
        } else {
            sanitized.resize(maxCharacters);
        }
    }
    return sanitized;
}

std::wstring LimitTitle(std::wstring text, int maxCharacters) {
    text = SanitizeUiText(std::move(text), 512);
    if (text.empty()) {
        text = L"Untitled event";
    }

    maxCharacters = std::max(5, maxCharacters);
    if (text.size() > static_cast<size_t>(maxCharacters)) {
        text.resize(static_cast<size_t>(maxCharacters - 3));
        text += L"...";
    }
    return text;
}

bool UnixToLocalSystemTime(int64_t unixSeconds, SYSTEMTIME* localTime) {
    if (!localTime || unixSeconds <= 0 ||
        unixSeconds > (std::numeric_limits<int64_t>::max() - kUnixToFileTimeTicks) /
                          kFileTimeTicksPerSecond) {
        return false;
    }

    int64_t fileTimeTicks = kUnixToFileTimeTicks + unixSeconds * kFileTimeTicksPerSecond;
    ULARGE_INTEGER value{};
    value.QuadPart = static_cast<ULONGLONG>(fileTimeTicks);

    FILETIME utc{};
    utc.dwLowDateTime = value.LowPart;
    utc.dwHighDateTime = value.HighPart;

    SYSTEMTIME utcTime{};
    if (!FileTimeToSystemTime(&utc, &utcTime)) {
        return false;
    }
    // Use the event's date when applying daylight saving rules, not today's offset.
    return SystemTimeToTzSpecificLocalTimeEx(nullptr, &utcTime, localTime) != FALSE;
}

std::wstring FormatTime(const SYSTEMTIME& st, bool use24Hour) {
    wchar_t buffer[32]{};
    if (use24Hour) {
        std::swprintf(buffer, ARRAYSIZE(buffer), L"%02u:%02u", st.wHour, st.wMinute);
    } else {
        int hour = st.wHour % 12;
        if (hour == 0) {
            hour = 12;
        }
        std::swprintf(buffer, ARRAYSIZE(buffer), L"%d:%02u %s", hour,
                      st.wMinute, st.wHour >= 12 ? L"PM" : L"AM");
    }
    return buffer;
}

std::wstring FormatUnixTime(int64_t unixSeconds, bool use24Hour) {
    SYSTEMTIME local{};
    if (!UnixToLocalSystemTime(unixSeconds, &local)) {
        return L"";
    }
    return FormatTime(local, use24Hour);
}

int LocalDateKey(int64_t unixSeconds) {
    SYSTEMTIME local{};
    if (!UnixToLocalSystemTime(unixSeconds, &local)) return 0;
    return static_cast<int>(local.wYear) * 10000 +
           static_cast<int>(local.wMonth) * 100 + static_cast<int>(local.wDay);
}

std::wstring FormatRelativeToNow(int64_t targetUnix) {
    int64_t now = NowUnix();
    if (targetUnix <= 0 || now <= 0 || targetUnix <= now) {
        return L"now";
    }

    int64_t seconds = targetUnix - now;
    int64_t minutes = (seconds + kSecondsPerMinute - 1) / kSecondsPerMinute;
    wchar_t buffer[64]{};
    if (minutes < 60) {
        std::swprintf(buffer, ARRAYSIZE(buffer), L"in %lldm", static_cast<long long>(minutes));
    } else if (minutes < 24 * 60) {
        int64_t hours = minutes / 60;
        int64_t mins = minutes % 60;
        if (mins) {
            std::swprintf(buffer, ARRAYSIZE(buffer), L"in %lldh %lldm",
                          static_cast<long long>(hours), static_cast<long long>(mins));
        } else {
            std::swprintf(buffer, ARRAYSIZE(buffer), L"in %lldh", static_cast<long long>(hours));
        }
    } else {
        int64_t days = (minutes + 24 * 60 - 1) / (24 * 60);
        std::swprintf(buffer, ARRAYSIZE(buffer), L"in %lldd", static_cast<long long>(days));
    }
    return buffer;
}

bool IsValidMeetCode(const std::wstring& code) {
    if (code.empty() || code.size() > 128) return false;
    int separators = 0;
    bool segmentHasCharacter = false;
    for (wchar_t ch : code) {
        if (ch == L'-') {
            if (!segmentHasCharacter || ++separators > 2) return false;
            segmentHasCharacter = false;
        } else if (ch >= L'a' && ch <= L'z') {
            segmentHasCharacter = true;
        } else {
            return false;
        }
    }
    return separators == 2 && segmentHasCharacter;
}

bool IsUrlSafeChar(wchar_t ch, bool allowQuestionMark) {
    if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') ||
        (ch >= L'0' && ch <= L'9')) return true;
    if (ch == L'?') return allowQuestionMark;
    return std::wstring_view(L"%._~!$&'()*+,;=:@/-").find(ch) != std::wstring_view::npos;
}

bool IsDigitsInRange(std::wstring_view text, size_t minLen, size_t maxLen) {
    if (text.size() < minLen || text.size() > maxLen) return false;
    for (wchar_t ch : text) if (ch < L'0' || ch > L'9') return false;
    return true;
}

bool AllSafeUrlChars(std::wstring_view text, size_t maxLen, bool allowQuestionMark) {
    if (text.empty() || text.size() > maxLen) return false;
    for (wchar_t ch : text) if (!IsUrlSafeChar(ch, allowQuestionMark)) return false;
    return true;
}

// Strict allowlist check: returns L"zoom" or L"teams" only for a
// normalized https join URL on an allowlisted host/path, otherwise empty.
std::wstring MeetingProviderFromUrl(const std::wstring& url) {
    constexpr std::wstring_view kScheme = L"https://";
    if (url.size() <= kScheme.size() || url.size() > 512) return L"";
    for (wchar_t ch : url) if (ch < 0x21 || ch > 0x7E) return L"";
    if (std::wstring_view(url).substr(0, kScheme.size()) != kScheme) return L"";
    size_t hostEnd = url.find_first_of(L"/?", kScheme.size());
    if (hostEnd == std::wstring::npos || url[hostEnd] != L'/') return L"";
    std::wstring host = url.substr(kScheme.size(), hostEnd - kScheme.size());
    if (host.empty()) return L"";
    for (wchar_t ch : host) {
        if (!((ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9') || ch == L'.' ||
              ch == L'-')) return L"";
    }
    size_t queryPos = url.find(L'?', hostEnd);
    std::wstring_view path = std::wstring_view(url).substr(
        hostEnd, queryPos == std::wstring::npos ? std::wstring::npos : queryPos - hostEnd);
    std::wstring_view query = queryPos == std::wstring::npos
                                  ? std::wstring_view()
                                  : std::wstring_view(url).substr(queryPos + 1);

    auto endsWith = [&](std::wstring_view suffix) {
        return host.size() > suffix.size() &&
               std::wstring_view(host).substr(host.size() - suffix.size()) == suffix;
    };
    auto startsWith = [](std::wstring_view text, std::wstring_view prefix) {
        return text.substr(0, prefix.size()) == prefix;
    };

    if (host == L"zoom.us" || endsWith(L".zoom.us")) {
        if (!startsWith(path, L"/j/") || !IsDigitsInRange(path.substr(3), 9, 12)) return L"";
        if (!query.empty()) {
            if (!startsWith(query, L"pwd=")) return L"";
            std::wstring_view pwd = query.substr(4);
            if (pwd.empty() || pwd.size() > 128) return L"";
            for (wchar_t ch : pwd) {
                bool ok = (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') ||
                          (ch >= L'0' && ch <= L'9') || ch == L'_' || ch == L'.' || ch == L'-';
                if (!ok) return L"";
            }
        }
        return L"zoom";
    }
    if (host == L"teams.microsoft.com") {
        if (!startsWith(path, L"/l/meetup-join/") ||
            !AllSafeUrlChars(path.substr(15), 400, false)) return L"";
        if (!query.empty() && !AllSafeUrlChars(query, 300, true)) return L"";
        return L"teams";
    }
    if (host == L"teams.live.com") {
        if (!startsWith(path, L"/meet/") || !IsDigitsInRange(path.substr(6), 8, 20)) return L"";
        if (!query.empty() && !AllSafeUrlChars(query, 300, true)) return L"";
        return L"teams";
    }
    if (host == L"aka.ms") {
        if (path != L"/JoinTeamsMeeting") return L"";
        if (!query.empty()) {
            if (!startsWith(query, L"omkt=") || query.size() > 16) return L"";
            for (wchar_t ch : query.substr(5)) {
                bool ok = (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') || ch == L'-';
                if (!ok) return L"";
            }
        }
        return L"teams";
    }
    return L"";
}

// A "join target" is either a Google Meet code or an allowlisted Zoom/Teams URL.
std::wstring MeetingTarget(const AgendaEntry& entry) {
    if (IsValidMeetCode(entry.googleMeetCode)) return entry.googleMeetCode;
    if (!MeetingProviderFromUrl(entry.meetingUrl).empty()) return entry.meetingUrl;
    return L"";
}

std::wstring MeetingProviderOfTarget(const std::wstring& target) {
    if (IsValidMeetCode(target)) return L"meet";
    return MeetingProviderFromUrl(target);
}

std::wstring MeetingJoinUrl(const std::wstring& target) {
    if (IsValidMeetCode(target)) return L"https://meet.google.com/" + target;
    return MeetingProviderFromUrl(target).empty() ? std::wstring() : target;
}

std::wstring UserValueName(PCWSTR name);
void RemoveIslandPreview();
void PublishStatus(const std::wstring& message, bool keepEvents) {
    Wh_SetStringValue(UserValueName(L"calendar_status").c_str(),
                      SanitizeUiText(message, 256).c_str());
    if (!keepEvents) {
        Wh_DeleteValue(UserValueName(L"calendar_events").c_str());
        Wh_SetIntValue(UserValueName(L"calendar_events_chars").c_str(), 0);
        RemoveIslandPreview();
    }
    g_hasPublishedEvents.store(keepEvents);
}

// ===========================================================================
// Native Google Calendar provider: OAuth (loopback + PKCE), WinHTTP client,
// Calendar REST ingestion, reminder toasts. Network work runs on worker threads.
// ===========================================================================

using winrt::Windows::Data::Json::JsonArray;
using winrt::Windows::Data::Json::JsonObject;
using winrt::Windows::Data::Json::JsonValueType;

constexpr wchar_t kGoogleHostApi[] = L"www.googleapis.com";
constexpr wchar_t kGoogleHostOAuthToken[] = L"oauth2.googleapis.com";
constexpr char kGoogleAuthEndpoint[] = "https://accounts.google.com/o/oauth2/v2/auth";
constexpr char kEventsReadonlyScope[] = "https://www.googleapis.com/auth/calendar.events.readonly";
constexpr char kCalendarListReadonlyScope[] =
    "https://www.googleapis.com/auth/calendar.calendarlist.readonly";
constexpr wchar_t kNotifiedValueName[] = L"notified_v1";
// Notification identity. It is registered only when a reminder is about to be shown and removed
// again when the mod unloads (see EnsureToastRegistration / RemoveToastRegistration).
constexpr wchar_t kToastAumid[] = L"CalendarAgendaPlus";
constexpr wchar_t kToastDisplayName[] = L"Windows Calendar Plus";
constexpr wchar_t kToastRegisteredFlag[] = L"toast_registered";
constexpr size_t kMaxApiResponseBytes = 4 * 1024 * 1024;
constexpr size_t kMaxTokenResponseBytes = 64 * 1024;
constexpr int kMaxCalendars = 16;
constexpr int kMaxEventsPerCalendar = 2000;
constexpr int kMaxEventsTotal = 4000;
constexpr size_t kMaxCalendarSnapshotChars = 32 * 1024 * 1024;
constexpr int kMaxEventPages = 8;
int64_t kFetchPastSeconds = 30 * kSecondsPerDay;
int64_t kFetchFutureSeconds = 90 * kSecondsPerDay;
constexpr int64_t kNotificationGraceSeconds = 120;
constexpr int64_t kNotificationRetentionSeconds = kSecondsPerDay;
constexpr int kSignInTimeoutMs = 3 * 60 * 1000;

std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) return {};
    int required = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                                       nullptr, 0, nullptr, nullptr);
    if (required <= 0) return {};
    std::string out(static_cast<size_t>(required), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(),
                        required, nullptr, nullptr);
    return out;
}

std::wstring Utf8ToWide(std::string_view text) {
    if (text.empty()) return {};
    int required = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                                       nullptr, 0);
    if (required <= 0) return {};
    std::wstring out(static_cast<size_t>(required), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(),
                        required);
    return out;
}

std::string Base64UrlEncode(const unsigned char* data, size_t size) {
    static constexpr char kAlphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string out;
    out.reserve((size * 4 + 2) / 3);
    size_t i = 0;
    for (; i + 2 < size; i += 3) {
        uint32_t v = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
        out.push_back(kAlphabet[(v >> 18) & 63]);
        out.push_back(kAlphabet[(v >> 12) & 63]);
        out.push_back(kAlphabet[(v >> 6) & 63]);
        out.push_back(kAlphabet[v & 63]);
    }
    if (i + 1 == size) {
        uint32_t v = data[i] << 16;
        out.push_back(kAlphabet[(v >> 18) & 63]);
        out.push_back(kAlphabet[(v >> 12) & 63]);
    } else if (i + 2 == size) {
        uint32_t v = (data[i] << 16) | (data[i + 1] << 8);
        out.push_back(kAlphabet[(v >> 18) & 63]);
        out.push_back(kAlphabet[(v >> 12) & 63]);
        out.push_back(kAlphabet[(v >> 6) & 63]);
    }
    return out;
}

bool RandomBytes(unsigned char* buffer, ULONG size) {
    return BCRYPT_SUCCESS(
        BCryptGenRandom(nullptr, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG));
}

bool Sha256(std::string_view input, unsigned char out[32]) {
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
        return false;
    }
    bool ok = BCRYPT_SUCCESS(BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0)) &&
              BCRYPT_SUCCESS(BCryptHashData(hash, reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())),
                                            static_cast<ULONG>(input.size()), 0)) &&
              BCRYPT_SUCCESS(BCryptFinishHash(hash, out, 32, 0));
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(alg, 0);
    return ok;
}

std::string UrlEncode(std::string_view text) {
    static constexpr char kHex[] = "0123456789ABCDEF";
    std::string out;
    for (unsigned char ch : text) {
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') ||
            ch == '-' || ch == '.' || ch == '_' || ch == '~') {
            out.push_back(static_cast<char>(ch));
        } else {
            out.push_back('%');
            out.push_back(kHex[ch >> 4]);
            out.push_back(kHex[ch & 15]);
        }
    }
    return out;
}

std::string UrlDecode(std::string_view text) {
    auto hex = [](char ch) -> int {
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
        if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
        return -1;
    };
    std::string out;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '%' && i + 2 < text.size() && hex(text[i + 1]) >= 0 &&
            hex(text[i + 2]) >= 0) {
            out.push_back(static_cast<char>(hex(text[i + 1]) * 16 + hex(text[i + 2])));
            i += 2;
        } else if (text[i] == '+') {
            out.push_back(' ');
        } else {
            out.push_back(text[i]);
        }
    }
    return out;
}

std::wstring LowerCopy(std::wstring text) {
    for (wchar_t& ch : text) ch = static_cast<wchar_t>(std::towlower(ch));
    return text;
}

uint64_t Fnv1a64(std::string_view data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::atomic<bool> g_forceRefresh{false};

// --- JSON helpers (Windows.Data.Json) ---------------------------------------

bool ParseJsonObject(const std::string& body, JsonObject* out) {
    try {
        JsonObject parsed{nullptr};
        if (!JsonObject::TryParse(winrt::hstring(Utf8ToWide(body)), parsed)) return false;
        *out = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

std::wstring JsonString(JsonObject const& obj, PCWSTR key) {
    try {
        if (!obj || !obj.HasKey(key)) return L"";
        auto value = obj.GetNamedValue(key);
        if (value.ValueType() != JsonValueType::String) return L"";
        return std::wstring(value.GetString());
    } catch (...) {
        return L"";
    }
}

bool JsonBool(JsonObject const& obj, PCWSTR key, bool fallback = false) {
    try {
        if (!obj || !obj.HasKey(key)) return fallback;
        auto value = obj.GetNamedValue(key);
        if (value.ValueType() != JsonValueType::Boolean) return fallback;
        return value.GetBoolean();
    } catch (...) {
        return fallback;
    }
}

JsonObject JsonObjectOf(JsonObject const& obj, PCWSTR key) {
    try {
        if (!obj || !obj.HasKey(key)) return JsonObject{nullptr};
        auto value = obj.GetNamedValue(key);
        if (value.ValueType() != JsonValueType::Object) return JsonObject{nullptr};
        return value.GetObject();
    } catch (...) {
        return JsonObject{nullptr};
    }
}

JsonArray JsonArrayOf(JsonObject const& obj, PCWSTR key) {
    try {
        if (!obj || !obj.HasKey(key)) return JsonArray{nullptr};
        auto value = obj.GetNamedValue(key);
        if (value.ValueType() != JsonValueType::Array) return JsonArray{nullptr};
        return value.GetArray();
    } catch (...) {
        return JsonArray{nullptr};
    }
}

// --- Credential storage (DPAPI, Windhawk mod storage) ------------------------

constexpr char kDpapiEntropy[] = "tray-agenda/google-refresh-token";

bool DpapiProtect(const std::string& plain, std::string* blob) {
    DATA_BLOB in{static_cast<DWORD>(plain.size()),
                 reinterpret_cast<BYTE*>(const_cast<char*>(plain.data()))};
    DATA_BLOB entropy{static_cast<DWORD>(sizeof(kDpapiEntropy) - 1),
                      reinterpret_cast<BYTE*>(const_cast<char*>(kDpapiEntropy))};
    DATA_BLOB out{};
    if (!CryptProtectData(&in, L"Tray Agenda", &entropy, nullptr, nullptr,
                          CRYPTPROTECT_UI_FORBIDDEN, &out)) {
        return false;
    }
    blob->assign(reinterpret_cast<char*>(out.pbData), out.cbData);
    LocalFree(out.pbData);
    return true;
}

bool DpapiUnprotect(const std::string& blob, std::string* plain) {
    DATA_BLOB in{static_cast<DWORD>(blob.size()),
                 reinterpret_cast<BYTE*>(const_cast<char*>(blob.data()))};
    DATA_BLOB entropy{static_cast<DWORD>(sizeof(kDpapiEntropy) - 1),
                      reinterpret_cast<BYTE*>(const_cast<char*>(kDpapiEntropy))};
    DATA_BLOB out{};
    if (!CryptUnprotectData(&in, nullptr, &entropy, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN,
                            &out)) {
        return false;
    }
    plain->assign(reinterpret_cast<char*>(out.pbData), out.cbData);
    SecureZeroMemory(out.pbData, out.cbData);
    LocalFree(out.pbData);
    return true;
}

// Windhawk's mod storage is machine-wide (HKLM), while every signed-in Windows user runs their
// own explorer.exe with this mod. All stored values therefore carry the user's SID.
std::wstring g_userKey;  // set once in Init, before any thread starts

std::wstring CurrentUserSid() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return L"";
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    std::wstring result;
    if (size && GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        LPWSTR text = nullptr;
        if (ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &text)) {
            result = text;
            LocalFree(text);
        }
    }
    CloseHandle(token);
    return result;
}

std::wstring UserValueName(PCWSTR base) {
    return std::wstring(base) + L"_" + g_userKey;
}

std::wstring ReadShortUserValue(const std::wstring& key) {
    wchar_t buffer[512]{};
    size_t length = Wh_GetStringValue(key.c_str(), buffer, ARRAYSIZE(buffer));
    return length ? std::wstring(buffer, length) : std::wstring();
}

bool SubmitAccountRequest(const std::wstring& key) {
    if (g_userKey.empty()) return false;
    GUID id{};
    if (FAILED(CoCreateGuid(&id))) return false;
    wchar_t nonce[40]{};
    if (!StringFromGUID2(id, nonce, ARRAYSIZE(nonce))) return false;
    return Wh_SetStringValue(UserValueName(key.c_str()).c_str(), nonce) != FALSE;
}

struct GoogleAccount {
    std::string id;      // 8 hex chars derived from the account email
    std::wstring label;  // account email (or a fallback name)
};

constexpr wchar_t kAccountsValueName[] = L"google_accounts_v1";
constexpr size_t kMaxAccounts = 4;
std::mutex g_accountsMutex;

bool IsHexId(const std::string& id) {
    if (id.size() != 8) return false;
    for (char ch : id) {
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'))) return false;
    }
    return true;
}

std::vector<GoogleAccount> LoadAccountsLocked() {
    std::vector<GoogleAccount> accounts;
    wchar_t buffer[2048];
    size_t chars = Wh_GetStringValue(UserValueName(kAccountsValueName).c_str(), buffer, ARRAYSIZE(buffer));
    if (!chars) return accounts;
    std::string data = WideToUtf8(std::wstring(buffer, chars));
    size_t pos = 0;
    while (pos < data.size() && accounts.size() < kMaxAccounts) {
        size_t end = data.find(';', pos);
        std::string item = data.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
        if (item.size() > 9 && item[8] == '=' && IsHexId(item.substr(0, 8))) {
            accounts.push_back({item.substr(0, 8), Utf8ToWide(UrlDecode(item.substr(9)))});
        }
        if (end == std::string::npos) break;
        pos = end + 1;
    }
    return accounts;
}

std::vector<GoogleAccount> LoadAccounts() {
    std::lock_guard<std::mutex> lock(g_accountsMutex);
    return LoadAccountsLocked();
}

void SaveAccountsLocked(const std::vector<GoogleAccount>& accounts) {
    std::string data;
    for (const auto& a : accounts) {
        data += a.id + "=" + UrlEncode(WideToUtf8(a.label)) + ";";
    }
    Wh_SetStringValue(UserValueName(kAccountsValueName).c_str(), Utf8ToWide(data).c_str());
}

std::wstring AccountTokenValueName(const std::string& id) {
    return UserValueName(L"google_rt") + L"_" + Utf8ToWide(id);
}

bool LoadRefreshToken(const std::string& id, std::string* token) {
    if (!IsHexId(id)) return false;
    char buffer[8192];
    size_t size = Wh_GetBinaryValue(AccountTokenValueName(id).c_str(), buffer, sizeof(buffer));
    if (size == 0) return false;
    std::string plain;
    if (!DpapiUnprotect(std::string(buffer, size), &plain) || plain.empty() ||
        plain.size() > 2048) {
        return false;
    }
    *token = std::move(plain);
    return true;
}

void ClearRefreshToken(const std::string& id) {
    if (IsHexId(id)) Wh_DeleteValue(AccountTokenValueName(id).c_str());
}

// Adds a new account or replaces the token of an existing one (same id).
bool AddOrUpdateAccount(const GoogleAccount& account, const std::string& refreshToken) {
    std::string blob;
    if (!IsHexId(account.id) || !DpapiProtect(refreshToken, &blob)) return false;
    std::lock_guard<std::mutex> lock(g_accountsMutex);
    std::vector<GoogleAccount> accounts = LoadAccountsLocked();
    auto it = std::find_if(accounts.begin(), accounts.end(),
                           [&](const GoogleAccount& a) { return a.id == account.id; });
    if (it == accounts.end()) {
        if (accounts.size() >= kMaxAccounts) return false;
        accounts.push_back(account);
    } else {
        it->label = account.label;
    }
    if (!Wh_SetBinaryValue(AccountTokenValueName(account.id).c_str(), blob.data(), blob.size())) {
        return false;
    }
    SaveAccountsLocked(accounts);
    return true;
}

void RemoveAccount(const std::string& id) {
    std::lock_guard<std::mutex> lock(g_accountsMutex);
    std::vector<GoogleAccount> accounts = LoadAccountsLocked();
    accounts.erase(std::remove_if(accounts.begin(), accounts.end(),
                                  [&](const GoogleAccount& a) { return a.id == id; }),
                   accounts.end());
    ClearRefreshToken(id);
    SaveAccountsLocked(accounts);
}

// --- Auth state -------------------------------------------------------------

// SignedIn means "at least one calendar source is configured" (Google account or ICS feed).
enum class AuthState : int {
    NoClient = 0,
    SignedOut = 1,
    SigningIn = 2,
    SignedIn = 3,
};

std::atomic<int> g_authState{static_cast<int>(AuthState::SignedOut)};
std::atomic<bool> g_signingIn{false};
std::mutex g_authNoteMutex;
std::wstring g_authNote;
std::map<std::string, std::wstring> g_accountNotes;

AuthState GetAuthState() {
    return static_cast<AuthState>(g_authState.load(std::memory_order_relaxed));
}

void SetAuthNote(std::wstring note) {
    std::lock_guard<std::mutex> lock(g_authNoteMutex);
    g_authNote = std::move(note);
    Wh_SetStringValue(UserValueName(L"account_status").c_str(), g_authNote.c_str());
}

std::wstring GetAuthNote() {
    std::lock_guard<std::mutex> lock(g_authNoteMutex);
    return g_authNote;
}

void SetAccountNote(const std::string& id, std::wstring note) {
    std::lock_guard<std::mutex> lock(g_authNoteMutex);
    if (note.empty()) g_accountNotes.erase(id);
    else g_accountNotes[id] = std::move(note);
}

// "user@example.com: Google sign-in expired..." lines for accounts that need attention.
std::vector<std::wstring> AccountNoteLines() {
    std::vector<GoogleAccount> accounts = LoadAccounts();
    std::lock_guard<std::mutex> lock(g_authNoteMutex);
    std::vector<std::wstring> lines;
    for (const auto& a : accounts) {
        auto it = g_accountNotes.find(a.id);
        if (it != g_accountNotes.end()) lines.push_back(a.label + L": " + it->second);
    }
    return lines;
}

bool ClientConfigured(const ModSettings& s) {
    return !s.google_client_id.empty() && !s.google_client_secret.empty();
}

void ReevaluateAuthState(const ModSettings& s) {
    AuthState state;
    if (!s.ics_feeds.empty() || !LoadAccounts().empty()) {
        state = AuthState::SignedIn;
    } else if (!ClientConfigured(s)) {
        state = AuthState::NoClient;
    } else if (g_signingIn.load()) {
        state = AuthState::SigningIn;
    } else {
        state = AuthState::SignedOut;
    }
    g_authState.store(static_cast<int>(state), std::memory_order_relaxed);
}

// --- WinHTTP client -----------------------------------------------------------

struct HttpResponse {
    bool completed = false;
    DWORD status = 0;
    bool tooLarge = false;
    std::string body;
    std::wstring location;  // Location header of a 3xx response
};

std::mutex g_httpMutex;
std::vector<HINTERNET> g_httpSessions;

// Closing a WinHTTP session handle cancels any request in flight on it, which
// keeps mod unload from waiting on a slow network.
void AbortAllHttp() {
    std::lock_guard<std::mutex> lock(g_httpMutex);
    for (HINTERNET session : g_httpSessions) {
        WinHttpCloseHandle(session);
    }
    g_httpSessions.clear();
}

// Google endpoints are allowlisted; `anyHost` is used only for user-configured ICS feeds.
HttpResponse HttpsRequest(PCWSTR host, const std::wstring& path, PCWSTR method,
                          const std::wstring& headers, const std::string& body,
                          size_t maxBytes, bool anyHost = false) {
    HttpResponse response;
    if (!anyHost && wcscmp(host, kGoogleHostApi) != 0 &&
        wcscmp(host, kGoogleHostOAuthToken) != 0) {
        return response;
    }
    if (g_workerStop.load()) return response;

    HINTERNET session = WinHttpOpen(L"WindowsCalendarPlus/0.2", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return response;
    {
        std::lock_guard<std::mutex> lock(g_httpMutex);
        if (g_workerStop.load()) {
            WinHttpCloseHandle(session);
            return response;
        }
        g_httpSessions.push_back(session);
    }

    HINTERNET connect = nullptr;
    HINTERNET request = nullptr;
    auto cleanup = [&]() {
        std::lock_guard<std::mutex> lock(g_httpMutex);
        auto it = std::find(g_httpSessions.begin(), g_httpSessions.end(), session);
        if (it == g_httpSessions.end()) return;  // already closed by AbortAllHttp
        g_httpSessions.erase(it);
        if (request) WinHttpCloseHandle(request);
        if (connect) WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
    };

    WinHttpSetTimeouts(session, 10000, 10000, 15000, 30000);
    connect = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (connect) {
        request = WinHttpOpenRequest(connect, method, path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    }
    if (!request) {
        cleanup();
        return response;
    }

    DWORD disableRedirects = WINHTTP_DISABLE_REDIRECTS;
    WinHttpSetOption(request, WINHTTP_OPTION_DISABLE_FEATURE, &disableRedirects,
                     sizeof(disableRedirects));

    BOOL sent = WinHttpSendRequest(
        request, headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headers.c_str(),
        headers.empty() ? 0 : static_cast<DWORD>(-1L),
        body.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char*>(body.data()),
        static_cast<DWORD>(body.size()), static_cast<DWORD>(body.size()), 0);
    if (sent && WinHttpReceiveResponse(request, nullptr)) {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        if (WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                                WINHTTP_NO_HEADER_INDEX)) {
            response.status = status;
            if (status == 301 || status == 302 || status == 303 || status == 307 || status == 308) {
                wchar_t location[2048];
                DWORD locationSize = sizeof(location);
                if (WinHttpQueryHeaders(request, WINHTTP_QUERY_LOCATION,
                                        WINHTTP_HEADER_NAME_BY_INDEX, location, &locationSize,
                                        WINHTTP_NO_HEADER_INDEX)) {
                    response.location.assign(location, locationSize / sizeof(wchar_t));
                }
            }
            bool ok = true;
            for (;;) {
                DWORD available = 0;
                if (!WinHttpQueryDataAvailable(request, &available)) {
                    ok = false;
                    break;
                }
                if (available == 0) break;
                if (response.body.size() + available > maxBytes) {
                    response.tooLarge = true;
                    ok = false;
                    break;
                }
                size_t offset = response.body.size();
                response.body.resize(offset + available);
                DWORD read = 0;
                if (!WinHttpReadData(request, response.body.data() + offset, available, &read)) {
                    ok = false;
                    break;
                }
                response.body.resize(offset + read);
                if (read == 0) break;
            }
            response.completed = ok;
        }
    }
    cleanup();
    return response;
}

// --- OAuth tokens -------------------------------------------------------------

struct TokenResult {
    bool ok = false;
    bool invalidGrant = false;
    bool transient = true;
    std::string accessToken;
    std::string refreshToken;
    std::string scope;
    int64_t expiresIn = 0;
};

TokenResult RequestToken(const std::string& form) {
    TokenResult result;
    HttpResponse http = HttpsRequest(
        kGoogleHostOAuthToken, L"/token", L"POST",
        L"Content-Type: application/x-www-form-urlencoded\r\n", form, kMaxTokenResponseBytes);
    if (!http.completed && http.status == 0) return result;

    JsonObject obj{nullptr};
    bool parsed = ParseJsonObject(http.body, &obj);
    if (http.status == 200 && parsed) {
        result.accessToken = WideToUtf8(JsonString(obj, L"access_token"));
        result.refreshToken = WideToUtf8(JsonString(obj, L"refresh_token"));
        result.scope = WideToUtf8(JsonString(obj, L"scope"));
        try {
            result.expiresIn = static_cast<int64_t>(obj.GetNamedNumber(L"expires_in", 0));
        } catch (...) {
        }
        result.ok = !result.accessToken.empty();
        result.transient = !result.ok;
        return result;
    }
    result.transient = !(http.status >= 400 && http.status < 500);
    if (parsed && JsonString(obj, L"error") == L"invalid_grant") result.invalidGrant = true;
    return result;
}

struct CachedAccessToken {
    std::string token;
    int64_t expiryUnix = 0;
};
std::mutex g_accessTokenMutex;
std::map<std::string, CachedAccessToken> g_accessTokens;

void CacheAccessToken(const std::string& accountId, const std::string& token, int64_t expiresIn) {
    std::lock_guard<std::mutex> lock(g_accessTokenMutex);
    g_accessTokens[accountId] = {token, NowUnix() + std::max<int64_t>(60, expiresIn)};
}

void ClearAccessToken(const std::string& accountId) {
    std::lock_guard<std::mutex> lock(g_accessTokenMutex);
    auto it = g_accessTokens.find(accountId);
    if (it == g_accessTokens.end()) return;
    SecureZeroMemory(it->second.token.data(), it->second.token.size());
    g_accessTokens.erase(it);
}

enum class TokenStatus { Ok, NeedsSignIn, Transient };

TokenStatus EnsureAccessToken(const ModSettings& s, const GoogleAccount& account,
                              std::string* token) {
    int64_t now = NowUnix();
    {
        std::lock_guard<std::mutex> lock(g_accessTokenMutex);
        auto it = g_accessTokens.find(account.id);
        if (it != g_accessTokens.end() && !it->second.token.empty() &&
            it->second.expiryUnix - 60 > now) {
            *token = it->second.token;
            return TokenStatus::Ok;
        }
    }
    std::string refresh;
    if (!LoadRefreshToken(account.id, &refresh)) {
        SetAccountNote(account.id, L"Google sign-in expired. Use Add Google account to sign in again.");
        return TokenStatus::NeedsSignIn;
    }

    std::string form = "grant_type=refresh_token&client_id=" +
                       UrlEncode(WideToUtf8(s.google_client_id)) +
                       "&client_secret=" + UrlEncode(WideToUtf8(s.google_client_secret)) +
                       "&refresh_token=" + UrlEncode(refresh);
    TokenResult result = RequestToken(form);
    SecureZeroMemory(refresh.data(), refresh.size());
    if (result.ok) {
        CacheAccessToken(account.id, result.accessToken, result.expiresIn);
        SetAccountNote(account.id, L"");
        *token = result.accessToken;
        return TokenStatus::Ok;
    }
    if (!result.transient) {
        // invalid_grant / invalid_client: the stored refresh token is unusable. The account
        // stays listed so the calendar pane can offer sign-in again.
        ClearRefreshToken(account.id);
        ClearAccessToken(account.id);
        SetAccountNote(account.id,
                       result.invalidGrant
                            ? L"Google sign-in expired. Open the three-dot menu and Google accounts to sign in again."
                           : L"Google rejected the client ID or secret. Check the mod settings.");
        return TokenStatus::NeedsSignIn;
    }
    return TokenStatus::Transient;
}

std::wstring FetchPrimaryCalendarId(const std::string& accessToken) {
    HttpResponse http = HttpsRequest(
        kGoogleHostApi, L"/calendar/v3/users/me/calendarList/primary?fields=id", L"GET",
        Utf8ToWide("Authorization: Bearer " + accessToken + "\r\n"), "", kMaxTokenResponseBytes);
    if (!http.completed || http.status != 200) return L"";
    JsonObject obj{nullptr};
    if (!ParseJsonObject(http.body, &obj)) return L"";
    return JsonString(obj, L"id");
}

// --- Interactive sign-in (system browser + loopback redirect + PKCE) -----------

std::atomic<bool> g_signInCancel{false};
[[clang::no_destroy]] std::optional<std::thread> g_signInThread;
std::mutex g_signInMutex;

struct ScopedSocket {
    SOCKET handle = INVALID_SOCKET;
    ~ScopedSocket() {
        if (handle != INVALID_SOCKET) closesocket(handle);
    }
};

void SendHtmlResponse(SOCKET client, int status, const char* statusText, const char* message) {
    std::string body = std::string("<!doctype html><meta charset=\"utf-8\"><title>Windows Calendar Plus</title>"
                                   "<body style=\"font-family:Segoe UI,sans-serif;margin:3em\"><h2>") +
                       message + "</h2><p>You can close this tab.</p></body>";
    std::string head = "HTTP/1.1 " + std::to_string(status) + " " + statusText +
                       "\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: " +
                       std::to_string(body.size()) +
                       "\r\nConnection: close\r\nCache-Control: no-store\r\n\r\n";
    std::string all = head + body;
    send(client, all.data(), static_cast<int>(all.size()), 0);
}

// Reads the request line of a loopback HTTP request. Returns the request target.
bool ReadRequestTarget(SOCKET client, std::string* target) {
    DWORD timeoutMs = 5000;
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutMs),
               sizeof(timeoutMs));
    std::string data;
    char buffer[1024];
    while (data.size() < 8192 && data.find("\r\n") == std::string::npos) {
        int n = recv(client, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        data.append(buffer, static_cast<size_t>(n));
    }
    size_t lineEnd = data.find("\r\n");
    if (lineEnd == std::string::npos) return false;
    std::string line = data.substr(0, lineEnd);
    if (line.rfind("GET ", 0) != 0) return false;
    size_t second = line.find(' ', 4);
    if (second == std::string::npos) return false;
    *target = line.substr(4, second - 4);
    return true;
}

std::string QueryParam(const std::string& target, const std::string& key) {
    size_t q = target.find('?');
    if (q == std::string::npos) return "";
    std::string query = target.substr(q + 1);
    size_t pos = 0;
    while (pos <= query.size()) {
        size_t amp = query.find('&', pos);
        std::string pair = query.substr(pos, amp == std::string::npos ? std::string::npos : amp - pos);
        size_t eq = pair.find('=');
        if (eq != std::string::npos && UrlDecode(pair.substr(0, eq)) == key) {
            return UrlDecode(pair.substr(eq + 1));
        }
        if (amp == std::string::npos) break;
        pos = amp + 1;
    }
    return "";
}

void SignInThreadProc() {
    const bool comInitialized =
        SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
    ModSettings settings = SettingsCopy();
    std::wstring failure;
    bool wsaStarted = false;
    bool success = false;
    do {
        if (!ClientConfigured(settings)) {
            failure = L"Set the Google client ID and secret in the mod settings first.";
            break;
        }
        WSADATA wsa{};
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            failure = L"Network stack could not start.";
            break;
        }
        wsaStarted = true;

        ScopedSocket listener;
        listener.handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = 0;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (listener.handle == INVALID_SOCKET ||
            bind(listener.handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0 ||
            listen(listener.handle, 4) != 0) {
            failure = L"Could not open the local sign-in listener.";
            break;
        }
        int addrLen = sizeof(addr);
        if (getsockname(listener.handle, reinterpret_cast<sockaddr*>(&addr), &addrLen) != 0) {
            failure = L"Could not open the local sign-in listener.";
            break;
        }
        std::string redirectUri = "http://127.0.0.1:" + std::to_string(ntohs(addr.sin_port));

        unsigned char raw[32];
        unsigned char stateRaw[16];
        unsigned char digest[32];
        if (!RandomBytes(raw, sizeof(raw)) || !RandomBytes(stateRaw, sizeof(stateRaw))) {
            failure = L"Could not generate secure random data.";
            break;
        }
        std::string verifier = Base64UrlEncode(raw, sizeof(raw));
        if (!Sha256(verifier, digest)) {
            failure = L"Could not compute the sign-in challenge.";
            break;
        }
        std::string challenge = Base64UrlEncode(digest, sizeof(digest));
        std::string state = Base64UrlEncode(stateRaw, sizeof(stateRaw));

        std::string scope = std::string(kEventsReadonlyScope) + " " + kCalendarListReadonlyScope;
        std::string url = std::string(kGoogleAuthEndpoint) +
                          "?client_id=" + UrlEncode(WideToUtf8(settings.google_client_id)) +
                          "&redirect_uri=" + UrlEncode(redirectUri) +
                          "&response_type=code&scope=" + UrlEncode(scope) +
                          "&code_challenge=" + challenge + "&code_challenge_method=S256" +
                          "&state=" + state + "&access_type=offline&prompt=consent";
        std::wstring wideUrl = Utf8ToWide(url);
        if (reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", wideUrl.c_str(), nullptr,
                                                    nullptr, SW_SHOWNORMAL)) <= 32) {
            failure = L"Could not open the browser.";
            break;
        }

        std::string code;
        ULONGLONG deadline = GetTickCount64() + kSignInTimeoutMs;
        int connections = 0;
        bool gotAnswer = false;
        while (!gotAnswer && connections < 8 && GetTickCount64() < deadline &&
               !g_workerStop.load() && !g_signInCancel.load()) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(listener.handle, &readSet);
            timeval tv{0, 250000};
            int ready = select(0, &readSet, nullptr, nullptr, &tv);
            if (ready <= 0) continue;
            ScopedSocket client;
            client.handle = accept(listener.handle, nullptr, nullptr);
            if (client.handle == INVALID_SOCKET) continue;
            ++connections;
            std::string target;
            if (!ReadRequestTarget(client.handle, &target)) {
                SendHtmlResponse(client.handle, 400, "Bad Request", "Bad request");
                continue;
            }
            std::string error = QueryParam(target, "error");
            std::string gotCode = QueryParam(target, "code");
            if (error.empty() && gotCode.empty()) {
                SendHtmlResponse(client.handle, 404, "Not Found", "Not found");
                continue;
            }
            if (QueryParam(target, "state") != state) {
                // Not our request (stray or forged): ignore it and keep waiting.
                SendHtmlResponse(client.handle, 400, "Bad Request", "Sign-in state mismatch");
                continue;
            }
            if (!error.empty()) {
                SendHtmlResponse(client.handle, 200, "OK", "Sign-in was cancelled");
                failure = L"Google sign-in was cancelled.";
                gotAnswer = true;
                continue;
            }
            SendHtmlResponse(client.handle, 200, "OK", "Signed in to Windows Calendar Plus");
            code = gotCode;
            gotAnswer = true;
        }
        if (code.empty()) {
            if (failure.empty()) {
                failure = (g_workerStop.load() || g_signInCancel.load())
                              ? L"Sign-in was cancelled."
                              : L"Sign-in timed out.";
            }
            break;
        }

        std::string form = "grant_type=authorization_code&code=" + UrlEncode(code) +
                           "&client_id=" + UrlEncode(WideToUtf8(settings.google_client_id)) +
                           "&client_secret=" + UrlEncode(WideToUtf8(settings.google_client_secret)) +
                           "&code_verifier=" + verifier +
                           "&redirect_uri=" + UrlEncode(redirectUri);
        TokenResult token = RequestToken(form);
        if (!token.ok || token.refreshToken.empty()) {
            failure = L"Google did not return a refresh token. Try again.";
            break;
        }
        if (token.scope.find(kEventsReadonlyScope) == std::string::npos ||
            token.scope.find(kCalendarListReadonlyScope) == std::string::npos) {
            failure = L"The required calendar permissions were not granted.";
            break;
        }
        // The primary calendar id of an account is its email address.
        std::wstring email = FetchPrimaryCalendarId(token.accessToken);
        GoogleAccount account;
        if (!email.empty() && email.size() <= 128) {
            std::string idSource = WideToUtf8(LowerCopy(email));
            char hex[24];
            std::snprintf(hex, sizeof(hex), "%08x",
                          static_cast<unsigned>(Fnv1a64(idSource) & 0xFFFFFFFFu));
            account.id = hex;
            account.label = email;
        } else {
            unsigned char rnd[4] = {};
            RandomBytes(rnd, sizeof(rnd));
            char hex[24];
            std::snprintf(hex, sizeof(hex), "%02x%02x%02x%02x", rnd[0], rnd[1], rnd[2], rnd[3]);
            account.id = hex;
            account.label = L"Google account";
        }
        if (!AddOrUpdateAccount(account, token.refreshToken)) {
            failure = LoadAccounts().size() >= kMaxAccounts
                          ? L"The maximum number of Google accounts is already connected."
                          : L"Could not store the Google credentials securely.";
            break;
        }
        CacheAccessToken(account.id, token.accessToken, token.expiresIn);
        SetAccountNote(account.id, L"");
        success = true;
    } while (false);

    if (wsaStarted) WSACleanup();
    SetAuthNote(success ? L"Google account connected." : failure);
    if (!success) Wh_Log(L"sign-in failed (%s)", failure.c_str());
    g_signingIn = false;
    ReevaluateAuthState(SettingsCopy());
    if (success) g_forceRefresh = true;
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
    if (comInitialized) CoUninitialize();
}

void StartSignIn() {
    std::lock_guard<std::mutex> lock(g_signInMutex);
    if (g_workerStop.load()) return;
    ModSettings settings = SettingsCopy();
    if (!ClientConfigured(settings)) {
        SetAuthNote(L"Set a Google Desktop-app OAuth client ID and secret in Windhawk settings first.");
        ReevaluateAuthState(settings);
        return;
    }
    if (g_signingIn.load()) return;
    if (LoadAccounts().size() >= kMaxAccounts) {
        SetAuthNote(L"The maximum number of Google accounts is already connected.");
        return;
    }
    if (g_signInThread) {
        if (g_signInThread->joinable()) g_signInThread->join();
        g_signInThread.reset();
    }
    g_signInCancel = false;
    SetAuthNote(L"Waiting for Google sign-in in your browser...");
    g_signingIn = true;
    ReevaluateAuthState(settings);
    g_signInThread.emplace(SignInThreadProc);
}

std::mutex g_revokeMutex;
std::vector<std::string> g_revokeTokens;

void SignOutAccount(const std::string& id) {
    // Serializes clearing the cache against publishing an in-flight fetch.
    std::lock_guard<std::mutex> publishLock(g_publishMutex);
    g_accountGeneration.fetch_add(1);
    std::string refresh;
    if (LoadRefreshToken(id, &refresh)) {
        std::lock_guard<std::mutex> lock(g_revokeMutex);
        g_revokeTokens.push_back(refresh);
    }
    RemoveAccount(id);
    ClearAccessToken(id);
    SetAccountNote(id, L"");
    SetAuthNote(L"");
    ReevaluateAuthState(SettingsCopy());
    PublishStatus(LoadAccounts().empty()
        ? L"Signed out" : L"Refreshing remaining calendars", false);
    g_forceRefresh = true;
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
}

// ShellHost writes a per-action nonce; the Explorer provider acknowledges it only
// after executing the action. Separate keys avoid losing rapid requests for
// different accounts, and old requests stay acknowledged across Explorer restarts.
void ProcessAccountRequests() {
    for (const auto& account : LoadAccounts()) {
        if (!IsHexId(account.id)) continue;
        std::wstring suffix = L"_" + Utf8ToWide(account.id);
        auto requestBase = L"google_signout_request" + suffix;
        auto doneBase = L"google_signout_handled" + suffix;
        auto requestKey = UserValueName(requestBase.c_str());
        auto doneKey = UserValueName(doneBase.c_str());
        std::wstring nonce = ReadShortUserValue(requestKey);
        if (nonce.empty() || nonce == ReadShortUserValue(doneKey)) continue;
        SignOutAccount(account.id);
        SetAuthNote(L"Google account signed out.");
        Wh_SetStringValue(doneKey.c_str(), nonce.c_str());
    }

    const auto signInKey = UserValueName(L"google_signin_request");
    const auto signInDoneKey = UserValueName(L"google_signin_handled");
    std::wstring signInNonce = ReadShortUserValue(signInKey);
    if (!signInNonce.empty() && signInNonce != ReadShortUserValue(signInDoneKey)) {
        StartSignIn();
        Wh_SetStringValue(signInDoneKey.c_str(), signInNonce.c_str());
    }
}

void RevokePendingToken() {
    std::vector<std::string> tokens;
    {
        std::lock_guard<std::mutex> lock(g_revokeMutex);
        tokens.swap(g_revokeTokens);
    }
    for (auto& token : tokens) {
        HttpsRequest(kGoogleHostOAuthToken, L"/revoke", L"POST",
                     L"Content-Type: application/x-www-form-urlencoded\r\n",
                     "token=" + UrlEncode(token), kMaxTokenResponseBytes);
        SecureZeroMemory(token.data(), token.size());
    }
}

// --- Time parsing -----------------------------------------------------------------

int64_t DaysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const int yoe = y - era * 400;
    const int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return static_cast<int64_t>(era) * 146097 + doe - 719468;
}

bool ParseFixedDigits(const std::wstring& s, size_t pos, size_t len, int* out) {
    if (pos + len > s.size()) return false;
    int value = 0;
    for (size_t i = 0; i < len; ++i) {
        wchar_t ch = s[pos + i];
        if (ch < L'0' || ch > L'9') return false;
        value = value * 10 + (ch - L'0');
    }
    *out = value;
    return true;
}

bool ValidCivilDate(int y, int m, int d) {
    if (y < 1970 || y > 2200 || m < 1 || m > 12 || d < 1 || d > 31) return false;
    static constexpr int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int limit = kDays[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) limit = 29;
    return d <= limit;
}

// RFC 3339 with an explicit offset (Z or +-HH:MM), as returned by Google Calendar.
bool ParseRfc3339(const std::wstring& s, int64_t* unix) {
    int y, mo, d, h, mi, sec;
    if (s.size() < 20 || !ParseFixedDigits(s, 0, 4, &y) || s[4] != L'-' ||
        !ParseFixedDigits(s, 5, 2, &mo) || s[7] != L'-' || !ParseFixedDigits(s, 8, 2, &d) ||
        (s[10] != L'T' && s[10] != L't') || !ParseFixedDigits(s, 11, 2, &h) || s[13] != L':' ||
        !ParseFixedDigits(s, 14, 2, &mi) || s[16] != L':' || !ParseFixedDigits(s, 17, 2, &sec)) {
        return false;
    }
    if (!ValidCivilDate(y, mo, d) || h > 23 || mi > 59 || sec > 60) return false;
    size_t pos = 19;
    if (pos < s.size() && s[pos] == L'.') {
        ++pos;
        while (pos < s.size() && s[pos] >= L'0' && s[pos] <= L'9') ++pos;
    }
    if (pos >= s.size()) return false;
    int64_t offsetSeconds = 0;
    if (s[pos] == L'Z' || s[pos] == L'z') {
        if (pos + 1 != s.size()) return false;
    } else if (s[pos] == L'+' || s[pos] == L'-') {
        int oh, om;
        if (pos + 6 != s.size() || !ParseFixedDigits(s, pos + 1, 2, &oh) || s[pos + 3] != L':' ||
            !ParseFixedDigits(s, pos + 4, 2, &om) || oh > 23 || om > 59) {
            return false;
        }
        offsetSeconds = (oh * 3600 + om * 60) * (s[pos] == L'-' ? -1 : 1);
    } else {
        return false;
    }
    *unix = DaysFromCivil(y, mo, d) * kSecondsPerDay + h * 3600 + mi * 60 + std::min(sec, 59) -
            offsetSeconds;
    return true;
}

// "YYYY-MM-DD" (all-day events) -> local midnight.
bool ParseLocalDate(const std::wstring& s, int64_t* unix) {
    int y, m, d;
    if (s.size() != 10 || !ParseFixedDigits(s, 0, 4, &y) || s[4] != L'-' ||
        !ParseFixedDigits(s, 5, 2, &m) || s[7] != L'-' || !ParseFixedDigits(s, 8, 2, &d) ||
        !ValidCivilDate(y, m, d)) {
        return false;
    }
    SYSTEMTIME local{};
    local.wYear = static_cast<WORD>(y);
    local.wMonth = static_cast<WORD>(m);
    local.wDay = static_cast<WORD>(d);
    SYSTEMTIME utc{};
    if (!TzSpecificLocalTimeToSystemTimeEx(nullptr, &local, &utc)) return false;
    FILETIME ft{};
    if (!SystemTimeToFileTime(&utc, &ft)) return false;
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    *unix = static_cast<int64_t>((static_cast<int64_t>(ticks.QuadPart) - kUnixToFileTimeTicks) /
                                 kFileTimeTicksPerSecond);
    return true;
}

std::wstring FormatRfc3339Utc(int64_t unix) {
    int64_t days = unix / kSecondsPerDay;
    int64_t rem = unix % kSecondsPerDay;
    if (rem < 0) {
        rem += kSecondsPerDay;
        --days;
    }
    // civil_from_days
    int64_t z = days + 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    int64_t doe = z - era * 146097;
    int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int64_t y = yoe + era * 400;
    int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int64_t mp = (5 * doy + 2) / 153;
    int64_t d = doy - (153 * mp + 2) / 5 + 1;
    int64_t m = mp < 10 ? mp + 3 : mp - 9;
    if (m <= 2) ++y;
    wchar_t buffer[40];
    std::swprintf(buffer, ARRAYSIZE(buffer), L"%04lld-%02lld-%02lldT%02lld:%02lld:%02lldZ",
                  static_cast<long long>(y), static_cast<long long>(m),
                  static_cast<long long>(d), static_cast<long long>(rem / 3600),
                  static_cast<long long>((rem % 3600) / 60), static_cast<long long>(rem % 60));
    return buffer;
}

// --- Meeting link detection --------------------------------------------------------

bool IsUrlTerminator(wchar_t ch) {
    return ch <= L' ' || ch == L'<' || ch == L'>' || ch == L'"' || ch == L'\'' || ch == L'`' ||
           ch == L'\\';
}

std::vector<std::wstring> ExtractUrlCandidates(const std::wstring& text, size_t maxCandidates) {
    std::vector<std::wstring> out;
    std::wstring lower;
    lower.reserve(text.size());
    for (wchar_t ch : text) lower.push_back(static_cast<wchar_t>(std::towlower(ch)));
    size_t pos = 0;
    while (out.size() < maxCandidates) {
        size_t found = lower.find(L"http", pos);
        if (found == std::wstring::npos) break;
        size_t schemeEnd = 0;
        if (lower.compare(found, 8, L"https://") == 0) schemeEnd = found + 8;
        else if (lower.compare(found, 7, L"http://") == 0) schemeEnd = found + 7;
        if (!schemeEnd) {
            pos = found + 4;
            continue;
        }
        size_t end = schemeEnd;
        while (end < text.size() && !IsUrlTerminator(text[end])) ++end;
        out.push_back(text.substr(found, end - found));
        pos = end;
    }
    return out;
}

std::wstring StripTrailingPunctuation(std::wstring value) {
    while (!value.empty() && std::wstring_view(L".,;:!?)]}>").find(value.back()) !=
                                 std::wstring_view::npos) {
        value.pop_back();
    }
    return value;
}

std::wstring MeetCodeFromUrl(const std::wstring& rawUrl) {
    std::wstring url = StripTrailingPunctuation(rawUrl);
    constexpr std::wstring_view kPrefix = L"https://meet.google.com/";
    std::wstring prefix;
    for (size_t i = 0; i < kPrefix.size() && i < url.size(); ++i) {
        prefix.push_back(static_cast<wchar_t>(std::towlower(url[i])));
    }
    if (prefix != kPrefix) return L"";
    std::wstring rest = url.substr(kPrefix.size());
    size_t cut = rest.find_first_of(L"?#");
    if (cut != std::wstring::npos) rest.resize(cut);
    if (!rest.empty() && rest.back() == L'/') rest.pop_back();
    if (rest.empty() || rest.find(L'/') != std::wstring::npos ||
        rest.find(L'%') != std::wstring::npos) {
        return L"";
    }
    for (wchar_t& ch : rest) ch = static_cast<wchar_t>(std::towlower(ch));
    return IsValidMeetCode(rest) ? rest : L"";
}

// Returns the normalized URL when `rawUrl` is an allowlisted Zoom/Teams join link.
bool CanonicalZoomTeamsUrl(const std::wstring& rawUrl, std::wstring* provider,
                           std::wstring* canonical) {
    std::wstring url = StripTrailingPunctuation(rawUrl);
    if (url.size() < 9 || url.size() > 512) return false;
    for (wchar_t ch : url) {
        if (ch < 0x21 || ch > 0x7E) return false;
    }
    std::wstring scheme = url.substr(0, 8);
    for (wchar_t& ch : scheme) ch = static_cast<wchar_t>(std::towlower(ch));
    if (scheme != L"https://") return false;
    size_t hostEnd = url.find_first_of(L"/?", 8);
    if (hostEnd == std::wstring::npos || url[hostEnd] != L'/') return false;
    std::wstring host = url.substr(8, hostEnd - 8);
    for (wchar_t& ch : host) ch = static_cast<wchar_t>(std::towlower(ch));
    std::wstring rest = url.substr(hostEnd);
    if (host == L"aka.ms") {
        size_t q = rest.find(L'?');
        std::wstring path = rest.substr(0, q);
        std::wstring lowerPath = path;
        for (wchar_t& ch : lowerPath) ch = static_cast<wchar_t>(std::towlower(ch));
        if (lowerPath == L"/jointeamsmeeting") {
            rest = L"/JoinTeamsMeeting" + (q == std::wstring::npos ? std::wstring() : rest.substr(q));
        }
    }
    std::wstring candidate = L"https://" + host + rest;
    std::wstring found = MeetingProviderFromUrl(candidate);
    if (found.empty()) return false;
    *provider = found;
    *canonical = candidate;
    return true;
}

// Scans text values (bounded) for the first Meet code, else the first Zoom/Teams link.
void ScanMeetingLinks(const std::vector<std::wstring>& values, std::wstring* meetCode,
                      std::wstring* provider, std::wstring* url) {
    constexpr size_t kScanChars = 16 * 1024;
    constexpr size_t kScanCandidates = 16;
    size_t scanned = 0;
    size_t candidates = 0;
    for (const auto& value : values) {
        if (scanned >= kScanChars || candidates >= kScanCandidates) break;
        std::wstring text = value.substr(0, kScanChars - scanned);
        scanned += text.size();
        for (const auto& candidate : ExtractUrlCandidates(text, kScanCandidates - candidates)) {
            ++candidates;
            if (meetCode->empty()) {
                std::wstring code = MeetCodeFromUrl(candidate);
                if (!code.empty()) *meetCode = code;
            }
            if (provider->empty()) {
                std::wstring p, u;
                if (CanonicalZoomTeamsUrl(candidate, &p, &u)) {
                    *provider = p;
                    *url = u;
                }
            }
        }
    }
}

// --- Google Calendar ingestion -------------------------------------------------------

enum class FetchFailure { None, Auth, Transient, Config };

struct CalendarSelection {
    std::wstring id;
    std::wstring label;
};

struct FetchOutcome {
    FetchFailure failure = FetchFailure::None;
    bool partial = false;
    std::wstring message;
    std::vector<AgendaEntry> events;
};

std::wstring SafeText(std::wstring text, size_t maxChars) {
    for (auto& ch : text) {
        if (ch == L'\r' || ch == L'\n' || ch == L'\t') ch = L' ';
    }
    text = Trim(std::move(text));
    if (text.size() > maxChars) text.resize(maxChars);
    return text;
}

// One authenticated GET. On 401 the cached access token is dropped and retried once.
FetchFailure GoogleGet(const ModSettings& settings, const GoogleAccount& account,
                       const std::wstring& path, JsonObject* out) {
    for (int attempt = 0; attempt < 2; ++attempt) {
        std::string token;
        TokenStatus status = EnsureAccessToken(settings, account, &token);
        if (status == TokenStatus::NeedsSignIn) return FetchFailure::Auth;
        if (status == TokenStatus::Transient) return FetchFailure::Transient;

        HttpResponse http = HttpsRequest(kGoogleHostApi, path, L"GET",
                                         Utf8ToWide("Authorization: Bearer " + token + "\r\n"),
                                         "", kMaxApiResponseBytes);
        SecureZeroMemory(token.data(), token.size());
        if (http.tooLarge) return FetchFailure::Config;
        if (!http.completed) return FetchFailure::Transient;
        if (http.status == 200) {
            return ParseJsonObject(http.body, out) ? FetchFailure::None : FetchFailure::Config;
        }
        if (http.status == 401 && attempt == 0) {
            ClearAccessToken(account.id);
            continue;
        }
        if (http.status == 401 || http.status == 403) {
            SetAccountNote(account.id,
                           L"Google denied access. Enable the Google Calendar API in your Cloud "
                           L"project, then add the account again.");
            return FetchFailure::Auth;
        }
        if (http.status == 429 || http.status >= 500) return FetchFailure::Transient;
        return FetchFailure::Config;
    }
    return FetchFailure::Transient;
}

AgendaEntry::ResponseState ResponseStateFromAttendees(JsonObject const& item) {
    JsonArray attendees = JsonArrayOf(item, L"attendees");
    if (!attendees) {
        // Google returns no attendee list for events without guests. Such an event on the
        // user's own calendar is the user's own event: treat it as accepted so it gets reminders.
        bool own = JsonBool(JsonObjectOf(item, L"organizer"), L"self") ||
                   JsonBool(JsonObjectOf(item, L"creator"), L"self");
        return own ? AgendaEntry::ResponseState::Accepted : AgendaEntry::ResponseState::Neutral;
    }
    int selfCount = 0;
    std::wstring status;
    for (uint32_t i = 0; i < attendees.Size(); ++i) {
        auto value = attendees.GetAt(i);
        if (value.ValueType() != JsonValueType::Object) continue;
        JsonObject attendee = value.GetObject();
        if (JsonBool(attendee, L"self")) {
            ++selfCount;
            status = JsonString(attendee, L"responseStatus");
        }
    }
    if (selfCount != 1) return AgendaEntry::ResponseState::Neutral;
    if (status == L"needsAction") return AgendaEntry::ResponseState::NeedsResponse;
    if (status == L"accepted") return AgendaEntry::ResponseState::Accepted;
    if (status == L"tentative") return AgendaEntry::ResponseState::Tentative;
    if (status == L"declined") return AgendaEntry::ResponseState::Declined;
    return AgendaEntry::ResponseState::Neutral;
}

bool ParseGoogleTime(JsonObject const& value, int64_t* unix, bool* allDay) {
    if (!value) return false;
    std::wstring dateTime = JsonString(value, L"dateTime");
    if (!dateTime.empty()) {
        *allDay = false;
        return ParseRfc3339(dateTime, unix);
    }
    std::wstring date = JsonString(value, L"date");
    if (!date.empty()) {
        *allDay = true;
        return ParseLocalDate(date, unix);
    }
    return false;
}

bool GoogleItemToEntry(JsonObject const& item, const std::wstring& source,
                       const std::wstring& calendarId, int64_t now, AgendaEntry* out) {
    if (JsonString(item, L"status") == L"cancelled") return false;
    int64_t start = 0, end = 0;
    bool startAllDay = false, endAllDay = false;
    if (!ParseGoogleTime(JsonObjectOf(item, L"start"), &start, &startAllDay)) return false;
    JsonObject endObj = JsonObjectOf(item, L"end");
    if (endObj) {
        if (!ParseGoogleTime(endObj, &end, &endAllDay)) return false;
    } else {
        endAllDay = startAllDay;
        end = start + (startAllDay ? kSecondsPerDay : kSecondsPerHour);
    }
    bool allDay = startAllDay && endAllDay;
    if (end <= start) end = start + (allDay ? kSecondsPerDay : kSecondsPerHour);

    AgendaEntry entry;
    entry.title = SafeText(JsonString(item, L"summary"), 180);
    if (entry.title.empty()) entry.title = L"Untitled event";
    entry.location = SafeText(JsonString(item, L"location"), 180);
    entry.source = SafeText(source, 96);
    if (entry.source.empty()) entry.source = L"Google Calendar";
    entry.startUnix = start;
    entry.endUnix = end;
    entry.allDay = allDay;
    entry.isActive = !allDay && start <= now && now < end;
    entry.responseState = ResponseStateFromAttendees(item);
    entry.hasResponseState = true;
    entry.outOfOffice = JsonString(item, L"eventType") == L"outOfOffice";

    std::vector<std::wstring> meetValues;
    JsonObject conference = JsonObjectOf(item, L"conferenceData");
    JsonArray entryPoints = JsonArrayOf(conference, L"entryPoints");
    if (entryPoints) {
        for (uint32_t i = 0; i < entryPoints.Size(); ++i) {
            auto value = entryPoints.GetAt(i);
            if (value.ValueType() != JsonValueType::Object) continue;
            std::wstring uri = JsonString(value.GetObject(), L"uri");
            if (!uri.empty()) meetValues.push_back(uri);
        }
    }
    std::wstring hangout = JsonString(item, L"hangoutLink");
    if (!hangout.empty()) meetValues.push_back(hangout);
    if (!JsonString(item, L"location").empty()) meetValues.push_back(JsonString(item, L"location"));
    std::vector<std::wstring> allValues = meetValues;
    std::wstring description = JsonString(item, L"description");
    entry.description = SafeText(description, 1000);
    if (!description.empty()) allValues.push_back(description);

    std::wstring meetCode, provider, url;
    ScanMeetingLinks(allValues, &meetCode, &provider, &url);
    if (!meetCode.empty()) {
        entry.googleMeetCode = meetCode;  // Meet wins over Zoom/Teams
    } else if (!provider.empty()) {
        entry.meetingProvider = provider;
        entry.meetingUrl = url;
    }
    entry.hasMeetCode = entry.hasMeetingProvider = entry.hasMeetingUrl = true;
    // Copies of one meeting in several calendars share the iCalUID; a shared calendar read
    // through two accounts shares calendar id + event id.
    std::wstring uid = JsonString(item, L"iCalUID");
    entry.dedupKey = uid.empty() ? calendarId + L"|" + JsonString(item, L"id") + L"|" +
                                       std::to_wstring(start)
                                 : uid + L"|" + std::to_wstring(start);
    *out = std::move(entry);
    return true;
}

FetchOutcome FetchGoogleEvents(const ModSettings& settings, int64_t now,
                               const GoogleAccount& account) {
    FetchOutcome outcome;

    // 1. Calendars to read: the configured ids, or the ones ticked in Google Calendar.
    std::vector<CalendarSelection> selections;
    std::wstring pageToken;
    std::vector<CalendarSelection> listed;
    std::vector<CalendarSelection> ticked;
    for (int page = 0; page < 4; ++page) {
        std::wstring path =
            L"/calendar/v3/users/me/calendarList?minAccessRole=reader&showDeleted=false"
            L"&showHidden=false&maxResults=250&fields=" +
            Utf8ToWide(UrlEncode(
                "items(id,summary,summaryOverride,primary,selected),nextPageToken"));
        if (!pageToken.empty()) path += L"&pageToken=" + Utf8ToWide(UrlEncode(WideToUtf8(pageToken)));
        JsonObject data{nullptr};
        FetchFailure failure = GoogleGet(settings, account, path, &data);
        if (failure != FetchFailure::None) {
            outcome.failure = failure;
            outcome.message = L"Could not list Google calendars";
            return outcome;
        }
        JsonArray items = JsonArrayOf(data, L"items");
        if (items) {
            for (uint32_t i = 0; i < items.Size(); ++i) {
                auto value = items.GetAt(i);
                if (value.ValueType() != JsonValueType::Object) continue;
                JsonObject item = value.GetObject();
                std::wstring id = JsonString(item, L"id");
                if (id.empty()) continue;
                std::wstring label = JsonString(item, L"summaryOverride");
                if (label.empty()) label = JsonString(item, L"summary");
                if (label.empty()) label = L"Google Calendar";
                listed.push_back({id, label});
                if (JsonBool(item, L"selected") || JsonBool(item, L"primary")) {
                    ticked.push_back({id, label});
                }
            }
        }
        pageToken = JsonString(data, L"nextPageToken");
        if (pageToken.empty()) break;
    }
    if (!settings.calendar_ids.empty()) {
        for (const auto& wanted : settings.calendar_ids) {
            for (const auto& cal : listed) {
                if (cal.id == wanted) {
                    selections.push_back(cal);
                    break;
                }
            }
        }
    } else {
        selections = ticked;
    }
    if (selections.size() > kMaxCalendars) selections.resize(kMaxCalendars);
    if (selections.empty()) {
        outcome.failure = FetchFailure::Config;
        outcome.message = L"No calendars selected";
        return outcome;
    }

    // 2. Events (singleEvents=true lets Google expand recurring events for us).
    const std::wstring timeMin = FormatRfc3339Utc(now - kFetchPastSeconds);
    const std::wstring timeMax = FormatRfc3339Utc(now + kFetchFutureSeconds);
    const std::string fields = UrlEncode(
        "items(id,iCalUID,status,eventType,summary,location,description,hangoutLink,"
        "conferenceData(entryPoints(uri)),attendees(self,responseStatus),organizer(self),"
        "creator(self),start,end),"
        "nextPageToken");
    int succeeded = 0;
    FetchFailure firstFailure = FetchFailure::None;
    for (const auto& cal : selections) {
        std::wstring base = L"/calendar/v3/calendars/" +
                            Utf8ToWide(UrlEncode(WideToUtf8(cal.id))) +
                            L"/events?singleEvents=true&orderBy=startTime&showDeleted=false"
                            L"&maxResults=250&maxAttendees=1&timeMin=" +
                            Utf8ToWide(UrlEncode(WideToUtf8(timeMin))) + L"&timeMax=" +
                            Utf8ToWide(UrlEncode(WideToUtf8(timeMax))) + L"&fields=" +
                            Utf8ToWide(fields);
        std::wstring calToken;
        int calEvents = 0;
        bool calOk = true;
        for (int page = 0; page < kMaxEventPages; ++page) {
            std::wstring path = base;
            if (!calToken.empty()) path += L"&pageToken=" + Utf8ToWide(UrlEncode(WideToUtf8(calToken)));
            JsonObject data{nullptr};
            FetchFailure failure = GoogleGet(settings, account, path, &data);
            if (failure == FetchFailure::Auth) {
                outcome.failure = FetchFailure::Auth;
                outcome.message = L"Google denied access to the calendar";
                outcome.events.clear();
                return outcome;
            }
            if (failure != FetchFailure::None) {
                if (firstFailure == FetchFailure::None) firstFailure = failure;
                calOk = false;
                break;
            }
            JsonArray items = JsonArrayOf(data, L"items");
            if (items) {
                for (uint32_t i = 0; i < items.Size(); ++i) {
                    if (calEvents >= kMaxEventsPerCalendar ||
                        static_cast<int>(outcome.events.size()) >= kMaxEventsTotal) {
                        break;
                    }
                    auto value = items.GetAt(i);
                    if (value.ValueType() != JsonValueType::Object) continue;
                    AgendaEntry entry;
                    if (GoogleItemToEntry(value.GetObject(), cal.label, cal.id, now, &entry)) {
                        outcome.events.push_back(std::move(entry));
                        ++calEvents;
                    }
                }
            }
            calToken = JsonString(data, L"nextPageToken");
            if (calToken.empty()) break;
        }
        if (calOk) {
            ++succeeded;
        } else {
            outcome.partial = true;
        }
    }
    if (succeeded == 0) {
        outcome.failure = firstFailure == FetchFailure::None ? FetchFailure::Transient : firstFailure;
        outcome.message = L"Could not read Google Calendar";
        outcome.events.clear();
    }
    return outcome;
}

std::vector<AgendaEntry> DedupeEvents(std::vector<AgendaEntry> events) {
    std::vector<AgendaEntry> out;
    std::map<std::wstring, size_t> seen;
    for (auto& e : events) {
        if (e.dedupKey.empty()) {
            out.push_back(std::move(e));
            continue;
        }
        auto it = seen.find(e.dedupKey);
        if (it == seen.end()) {
            seen[e.dedupKey] = out.size();
            out.push_back(std::move(e));
        } else if (out[it->second].responseState != AgendaEntry::ResponseState::Accepted &&
                   e.responseState == AgendaEntry::ResponseState::Accepted) {
            out[it->second] = std::move(e);  // keep the copy you accepted
        }
    }
    return out;
}

FetchOutcome FetchIcsFeed(const IcsFeedConfig& feed, int64_t now);

// Reads every Google account and ICS feed. A failing source never hides the others.
FetchOutcome FetchAllSources(const ModSettings& settings, int64_t now) {
    FetchOutcome total;
    int attempted = 0;
    int succeeded = 0;
    FetchOutcome firstFailure;
    for (const auto& account : LoadAccounts()) {
        if (g_workerStop.load()) break;
        ++attempted;
        FetchOutcome one = FetchGoogleEvents(settings, now, account);
        if (one.failure == FetchFailure::None) {
            ++succeeded;
            if (one.partial) total.partial = true;
            for (auto& e : one.events) total.events.push_back(std::move(e));
        } else {
            total.partial = true;
            if (firstFailure.failure == FetchFailure::None) firstFailure = one;
        }
    }
    for (const auto& feed : settings.ics_feeds) {
        if (g_workerStop.load()) break;
        ++attempted;
        FetchOutcome one = FetchIcsFeed(feed, now);
        if (one.failure == FetchFailure::None) {
            ++succeeded;
            if (one.partial) total.partial = true;
            for (auto& e : one.events) total.events.push_back(std::move(e));
        } else {
            total.partial = true;
            if (firstFailure.failure == FetchFailure::None) firstFailure = one;
        }
    }
    total.events = DedupeEvents(std::move(total.events));
    if (total.events.size() > kMaxEventsTotal) { total.events.resize(kMaxEventsTotal); total.partial = true; }
    if (attempted > 0 && succeeded == 0) {
        total.failure = firstFailure.failure;
        total.message = firstFailure.message;
        total.events.clear();
    }
    return total;
}

// --- ICS feeds ----------------------------------------------------------------------------
// Parses iCalendar text (secret feed URLs from Google/Outlook/...). Recurrence rules,
// EXDATE/RDATE and RECURRENCE-ID overrides are expanded for the fetch window only.

constexpr size_t kMaxIcsBytes = 8 * 1024 * 1024;
constexpr int kMaxIcsRedirects = 4;
constexpr size_t kMaxOccurrencesPerEvent = 512;
constexpr int kMaxIcsComponents = 5000;
constexpr int64_t kIcsGuardCandidates = 100000;

struct IcsTime {
    bool valid = false;
    bool dateOnly = false;
    bool utc = false;
    std::string tzid;  // empty: floating (local time)
    int y = 0, mo = 0, d = 0, h = 0, mi = 0, s = 0;
};

int64_t CivilNaiveSeconds(int y, int mo, int d, int h, int mi, int s) {
    return DaysFromCivil(y, mo, d) * kSecondsPerDay + h * 3600 + mi * 60 + s;
}

void CivilFromDays(int64_t z, int* y, int* m, int* d) {
    z += 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    int64_t doe = z - era * 146097;
    int64_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int64_t yy = yoe + era * 400;
    int64_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int64_t mp = (5 * doy + 2) / 153;
    *d = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
    *m = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
    *y = static_cast<int>(*m <= 2 ? yy + 1 : yy);
}

int WeekdayOfDay(int64_t days) {  // 0 = Sunday
    return static_cast<int>(((days % 7) + 7 + 4) % 7);
}

int DaysInMonth(int y, int m) {
    static constexpr int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return kDays[m - 1];
}

// RFC 5545 selects the first occurrence of a repeated wall-clock time, and
// the pre-transition offset for a nonexistent time. Sample both sides of DST.
template <typename ProjectToLocal>
bool ResolveCivilTime(const IcsTime& t, ProjectToLocal project, int64_t* unix, bool allowGap = true) {
    const int64_t naive = CivilNaiveSeconds(t.y, t.mo, t.d, t.h, t.mi, t.s);
    std::set<int64_t> candidates;
    for (int delta : {-2, 0, 2}) {
        const int64_t sample = naive + delta * kSecondsPerDay;
        int64_t local = 0;
        if (!project(sample, &local)) return false;
        candidates.insert(naive - (local - sample));
    }
    for (int64_t candidate : candidates) {
        int64_t local = 0;
        if (!project(candidate, &local)) return false;
        if (local == naive) {
            *unix = candidate;
            return true;
        }
    }
    if (allowGap && candidates.size() > 1) {
        *unix = *candidates.rbegin();  // spring-forward gap: use the earlier offset
        return true;
    }
    return false;
}

bool ProjectWindowsTime(int64_t unix, const DYNAMIC_TIME_ZONE_INFORMATION* zone,
                        int64_t* localNaive) {
    ULARGE_INTEGER ticks{};
    ticks.QuadPart = static_cast<ULONGLONG>(kUnixToFileTimeTicks + unix * kFileTimeTicksPerSecond);
    FILETIME ft{ticks.LowPart, ticks.HighPart};
    SYSTEMTIME utc{}, local{};
    if (!FileTimeToSystemTime(&ft, &utc) ||
        !SystemTimeToTzSpecificLocalTimeEx(zone, &utc, &local)) return false;
    *localNaive = CivilNaiveSeconds(local.wYear, local.wMonth, local.wDay,
                                   local.wHour, local.wMinute, local.wSecond);
    return true;
}

bool LocalCivilToUnix(int y, int mo, int d, int h, int mi, int s, int64_t* unix, bool allowGap = true) {
    IcsTime t;
    t.y = y; t.mo = mo; t.d = d; t.h = h; t.mi = mi; t.s = s;
    return ResolveCivilTime(t, [](int64_t value, int64_t* local) {
        return ProjectWindowsTime(value, nullptr, local);
    }, unix, allowGap);
}

// Windows time zone names ("W. Europe Standard Time"), as written by Outlook.
bool WindowsZoneToUnix(const std::string& name, const IcsTime& t, int64_t* unix, bool allowGap = true) {
    std::wstring wide = Utf8ToWide(name);
    if (wide.empty() || wide.size() > 128) return false;
    for (DWORD index = 0; ; ++index) {
        DYNAMIC_TIME_ZONE_INFORMATION zone{};
        DWORD error = EnumDynamicTimeZoneInformation(index, &zone);
        if (error != ERROR_SUCCESS) return false;
        if (_wcsicmp(zone.TimeZoneKeyName, wide.c_str()) != 0) continue;
        return ResolveCivilTime(t, [&zone](int64_t value, int64_t* local) {
            return ProjectWindowsTime(value, &zone, local);
        }, unix, allowGap);
    }
}

// IANA zone ids ("America/Sao_Paulo") via Windows.Globalization.Calendar. The offset is found
// by asking the calendar for the local wall time of a candidate instant.
[[clang::no_destroy]] std::optional<std::map<std::string, winrt::Windows::Globalization::Calendar>>
    g_zoneCalendars{std::in_place};

// Called on the worker thread before its WinRT apartment is torn down.
void ClearZoneCalendars() {
    g_zoneCalendars.reset();
}

bool IanaZoneToUnix(const std::string& tzid, const IcsTime& t, int64_t* unix, bool allowGap = true) {
    using winrt::Windows::Globalization::Calendar;
    using winrt::Windows::Globalization::CalendarIdentifiers;
    using winrt::Windows::Globalization::ClockIdentifiers;
    if (!g_zoneCalendars) g_zoneCalendars.emplace();
    auto* cache = &*g_zoneCalendars;
    try {
        auto it = cache->find(tzid);
        if (it == cache->end()) {
            if (cache->size() > 64) cache->clear();
            Calendar created(std::vector<winrt::hstring>{L"en-US"}, CalendarIdentifiers::Gregorian(),
                             ClockIdentifiers::TwentyFourHour(), winrt::hstring(Utf8ToWide(tzid)));
            it = cache->emplace(tzid, created).first;
        }
        Calendar cal = it->second;
        return ResolveCivilTime(t, [&cal](int64_t value, int64_t* local) {
            int64_t ticks = (value + 11644473600LL) * kFileTimeTicksPerSecond;
            cal.SetDateTime(winrt::Windows::Foundation::DateTime{
                winrt::Windows::Foundation::TimeSpan{ticks}});
            *local = CivilNaiveSeconds(cal.Year(), cal.Month(), cal.Day(), cal.Hour(),
                                       cal.Minute(), cal.Second());
            return true;
        }, unix, allowGap);
    } catch (...) {
        return false;
    }
}

bool IcsTimeToUnix(const IcsTime& t, int64_t* unix, bool allowGap = true) {
    if (!t.valid) return false;
    if (t.utc) {
        *unix = CivilNaiveSeconds(t.y, t.mo, t.d, t.h, t.mi, t.s);
        return true;
    }
    if (!t.dateOnly && !t.tzid.empty()) {
        std::string upper = WideToUtf8(LowerCopy(Utf8ToWide(t.tzid)));
        if (upper == "utc" || upper == "gmt" || upper == "etc/utc" || upper == "z") {
            *unix = CivilNaiveSeconds(t.y, t.mo, t.d, t.h, t.mi, t.s);
            return true;
        }
        if (t.tzid.find('/') != std::string::npos) {
            if (IanaZoneToUnix(t.tzid, t, unix, allowGap)) return true;
        } else if (WindowsZoneToUnix(t.tzid, t, unix, allowGap) || IanaZoneToUnix(t.tzid, t, unix, allowGap)) {
            return true;
        }
        return false;  // explicit unknown TZID must never become a floating time
    }
    return LocalCivilToUnix(t.y, t.mo, t.d, t.h, t.mi, t.s, unix, t.dateOnly || allowGap);
}

struct IcsProp {
    std::string name;  // upper-case
    std::map<std::string, std::string> params;  // upper-case keys
    std::string value;
};

std::string UpperAscii(std::string text) {
    for (char& ch : text) {
        if (ch >= 'a' && ch <= 'z') ch = static_cast<char>(ch - 32);
    }
    return text;
}

bool ParseIcsLine(const std::string& line, IcsProp* prop) {
    size_t pos = 0;
    while (pos < line.size() && line[pos] != ':' && line[pos] != ';') ++pos;
    if (pos == 0 || pos >= line.size()) return false;
    prop->name = UpperAscii(line.substr(0, pos));
    prop->params.clear();
    while (pos < line.size() && line[pos] == ';') {
        ++pos;
        size_t eq = pos;
        while (eq < line.size() && line[eq] != '=' && line[eq] != ':' && line[eq] != ';') ++eq;
        if (eq >= line.size() || line[eq] != '=') return false;
        std::string key = UpperAscii(line.substr(pos, eq - pos));
        pos = eq + 1;
        std::string value;
        if (pos < line.size() && line[pos] == '"') {
            size_t close = line.find('"', pos + 1);
            if (close == std::string::npos) return false;
            value = line.substr(pos + 1, close - pos - 1);
            pos = close + 1;
        } else {
            size_t end = pos;
            while (end < line.size() && line[end] != ':' && line[end] != ';') ++end;
            value = line.substr(pos, end - pos);
            pos = end;
        }
        prop->params[key] = value;
    }
    if (pos >= line.size() || line[pos] != ':') return false;
    prop->value = line.substr(pos + 1);
    return true;
}

bool ParseIcsTimeValue(const IcsProp& prop, IcsTime* t) {
    const std::string& v = prop.value;
    IcsTime out;
    auto digits = [&](size_t pos, size_t len, int* dst) {
        if (pos + len > v.size()) return false;
        int value = 0;
        for (size_t i = 0; i < len; ++i) {
            if (v[pos + i] < '0' || v[pos + i] > '9') return false;
            value = value * 10 + (v[pos + i] - '0');
        }
        *dst = value;
        return true;
    };
    if (!digits(0, 4, &out.y) || !digits(4, 2, &out.mo) || !digits(6, 2, &out.d)) return false;
    if (!ValidCivilDate(out.y, out.mo, out.d)) return false;
    auto valueType = prop.params.find("VALUE");
    bool dateValue = valueType != prop.params.end() && UpperAscii(valueType->second) == "DATE";
    if (v.size() == 8 || dateValue) {
        if (v.size() != 8) return false;
        out.dateOnly = true;
    } else {
        if (v.size() < 15 || v[8] != 'T' || !digits(9, 2, &out.h) || !digits(11, 2, &out.mi) ||
            !digits(13, 2, &out.s) || out.h > 23 || out.mi > 59 || out.s > 60) {
            return false;
        }
        out.s = std::min(out.s, 59);
        if (v.size() == 16 && v[15] == 'Z') out.utc = true;
        else if (v.size() != 15) return false;
        auto tz = prop.params.find("TZID");
        if (!out.utc && tz != prop.params.end() && tz->second.size() <= 128) out.tzid = tz->second;
    }
    out.valid = true;
    *t = out;
    return true;
}

std::wstring IcsUnescape(const std::string& raw) {
    std::string out;
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char next = raw[i + 1];
            if (next == 'n' || next == 'N') out.push_back('\n');
            else out.push_back(next);
            ++i;
        } else {
            out.push_back(raw[i]);
        }
    }
    return Utf8ToWide(out);
}

// "PT1H30M", "P1D", "P1W" -> seconds, or -1.
int64_t ParseIcsDuration(const std::string& v) {
    size_t i = 0;
    int sign = 1;
    if (i < v.size() && (v[i] == '+' || v[i] == '-')) sign = v[i++] == '-' ? -1 : 1;
    if (i >= v.size() || v[i] != 'P') return -1;
    ++i;
    bool inTime = false;
    int64_t total = 0;
    int64_t number = 0;
    bool haveNumber = false;
    for (; i < v.size(); ++i) {
        char ch = v[i];
        if (ch == 'T') {
            inTime = true;
        } else if (ch >= '0' && ch <= '9') {
            number = number * 10 + (ch - '0');
            haveNumber = true;
            if (number > 100000000) return -1;
        } else if (haveNumber) {
            switch (ch) {
                case 'W': total += number * 7 * kSecondsPerDay; break;
                case 'D': total += number * kSecondsPerDay; break;
                case 'H': if (!inTime) return -1; total += number * 3600; break;
                case 'M': if (!inTime) return -1; total += number * 60; break;
                case 'S': if (!inTime) return -1; total += number; break;
                default: return -1;
            }
            number = 0;
            haveNumber = false;
        } else {
            return -1;
        }
    }
    return haveNumber ? -1 : sign * total;
}

struct IcsRule {
    std::string freq;
    int interval = 1;
    int count = -1;
    bool hasUntil = false;
    IcsTime until;
    std::vector<std::pair<int, int>> byDay;  // (ordinal, weekday 0=Sunday)
    std::vector<int> byMonthDay;
    std::vector<int> byMonth;
    int wkst = 1;  // Monday
};

int WeekdayFromCode(const std::string& code) {
    static constexpr const char* kCodes[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
    for (int i = 0; i < 7; ++i) {
        if (code == kCodes[i]) return i;
    }
    return -1;
}

std::vector<std::string> SplitString(const std::string& text, char sep) {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t end = text.find(sep, pos);
        parts.push_back(text.substr(pos, end == std::string::npos ? std::string::npos : end - pos));
        if (end == std::string::npos) break;
        pos = end + 1;
    }
    return parts;
}

bool ParseIntStrict(const std::string& text, int* out) {
    if (text.empty() || text.size() > 6) return false;
    size_t i = 0;
    int sign = 1;
    if (text[0] == '+' || text[0] == '-') {
        sign = text[0] == '-' ? -1 : 1;
        i = 1;
    }
    if (i >= text.size()) return false;
    int value = 0;
    for (; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
        value = value * 10 + (text[i] - '0');
    }
    *out = sign * value;
    return true;
}

bool ParseIcsRule(const std::string& text, IcsRule* rule) {
    IcsRule out;
    for (const auto& part : SplitString(text, ';')) {
        size_t eq = part.find('=');
        if (eq == std::string::npos) continue;
        std::string key = UpperAscii(part.substr(0, eq));
        std::string value = UpperAscii(part.substr(eq + 1));
        if (key == "FREQ") {
            out.freq = value;
        } else if (key == "INTERVAL") {
            if (!ParseIntStrict(value, &out.interval) || out.interval < 1 || out.interval > 1000) return false;
        } else if (key == "COUNT") {
            if (!ParseIntStrict(value, &out.count) || out.count < 1) return false;
        } else if (key == "UNTIL") {
            IcsProp p;
            p.value = value;
            if (!ParseIcsTimeValue(p, &out.until)) return false;
            out.hasUntil = true;
        } else if (key == "WKST") {
            int wd = WeekdayFromCode(value);
            if (wd >= 0) out.wkst = wd;
        } else if (key == "BYDAY") {
            for (const auto& item : SplitString(value, ',')) {
                if (item.size() < 2) return false;
                int wd = WeekdayFromCode(item.substr(item.size() - 2));
                if (wd < 0) return false;
                int ord = 0;
                std::string ordText = item.substr(0, item.size() - 2);
                if (!ordText.empty() && !ParseIntStrict(ordText, &ord)) return false;
                if (ord < -53 || ord > 53) return false;
                out.byDay.push_back({ord, wd});
            }
        } else if (key == "BYMONTHDAY") {
            for (const auto& item : SplitString(value, ',')) {
                int v;
                if (!ParseIntStrict(item, &v) || v == 0 || v < -31 || v > 31) return false;
                out.byMonthDay.push_back(v);
            }
        } else if (key == "BYMONTH") {
            for (const auto& item : SplitString(value, ',')) {
                int v;
                if (!ParseIntStrict(item, &v) || v < 1 || v > 12) return false;
                out.byMonth.push_back(v);
            }
        } else {
            // Do not silently expand a different schedule (e.g. ignored BYSETPOS).
            return false;
        }
    }
    if (out.freq != "DAILY" && out.freq != "WEEKLY" && out.freq != "MONTHLY" &&
        out.freq != "YEARLY") {
        return false;
    }
    if (out.hasUntil && out.count > 0) return false;
    if (out.freq == "WEEKLY" && !out.byMonthDay.empty()) return false;
    for (const auto& [ordinal, weekday] : out.byDay) {
        if ((out.freq == "DAILY" || out.freq == "WEEKLY") && ordinal != 0) return false;
        if (out.freq == "MONTHLY" && (ordinal < -5 || ordinal > 5)) return false;
    }
    // This expander interprets BYDAY within a month, not within an entire year.
    if (out.freq == "YEARLY" && out.byMonth.empty() &&
        (!out.byDay.empty() || !out.byMonthDay.empty())) return false;
    if (out.freq == "YEARLY") {
        for (const auto& [ordinal, weekday] : out.byDay)
            if (ordinal < -5 || ordinal > 5) return false;
    }
    *rule = std::move(out);
    return true;
}

// Days (since epoch) of month `m` of year `y` matching BYMONTHDAY / BYDAY, ascending.
std::vector<int64_t> MonthDays(int y, int m, const IcsRule& rule, int defaultDay) {
    std::vector<int64_t> days;
    const int dim = DaysInMonth(y, m);
    const int64_t first = DaysFromCivil(y, m, 1);
    if (!rule.byMonthDay.empty()) {
        for (int v : rule.byMonthDay) {
            int day = v > 0 ? v : dim + v + 1;
            if (day >= 1 && day <= dim) days.push_back(first + day - 1);
        }
    } else if (!rule.byDay.empty()) {
        for (const auto& [ord, wd] : rule.byDay) {
            std::vector<int64_t> matches;
            for (int day = 1; day <= dim; ++day) {
                if (WeekdayOfDay(first + day - 1) == wd) matches.push_back(first + day - 1);
            }
            if (ord == 0) {
                days.insert(days.end(), matches.begin(), matches.end());
            } else if (ord > 0 && ord <= static_cast<int>(matches.size())) {
                days.push_back(matches[ord - 1]);
            } else if (ord < 0 && -ord <= static_cast<int>(matches.size())) {
                days.push_back(matches[matches.size() + ord]);
            }
        }
    } else if (defaultDay <= dim) {
        days.push_back(first + defaultDay - 1);
    }
    if (!rule.byMonthDay.empty() && !rule.byDay.empty()) {
        std::erase_if(days, [&](int64_t value) {
            const int day = static_cast<int>(value - first + 1);
            const int weekday = WeekdayOfDay(value);
            for (const auto& [ordinal, wd] : rule.byDay) {
                if (wd == weekday && (ordinal == 0 || ordinal == (day - 1) / 7 + 1 ||
                                      ordinal == -((dim - day) / 7 + 1))) return false;
            }
            return true;
        });
    }
    std::sort(days.begin(), days.end());
    days.erase(std::unique(days.begin(), days.end()), days.end());
    return days;
}

struct IcsEvent {
    std::string uid;
    std::wstring summary, description, location;
    std::vector<std::wstring> linkValues;
    std::string status;
    IcsTime start, end;
    bool hasEnd = false;
    int64_t durationSeconds = -1;
    std::string rrule;
    std::vector<IcsTime> exdates, rdates;
    bool hasRecId = false;
    IcsTime recId;
};

struct IcsOccurrence {
    int64_t start = 0;
    int64_t end = 0;
};

// Occurrence starts (unix) of one event that fall inside [winStart, winEnd).
std::vector<IcsOccurrence> ExpandIcsEvent(const IcsEvent& ev, int64_t winStart, int64_t winEnd) {
    std::vector<IcsOccurrence> out;
    int64_t s0 = 0;
    if (!IcsTimeToUnix(ev.start, &s0)) return out;
    const bool allDay = ev.start.dateOnly;

    int64_t durationSeconds = 0;
    int64_t durationDays = 0;
    if (ev.hasEnd) {
        int64_t e0 = 0;
        if (IcsTimeToUnix(ev.end, &e0) && e0 > s0) {
            durationSeconds = e0 - s0;
            if (allDay) {
                durationDays = DaysFromCivil(ev.end.y, ev.end.mo, ev.end.d) -
                               DaysFromCivil(ev.start.y, ev.start.mo, ev.start.d);
            }
        }
    } else if (ev.durationSeconds > 0) {
        durationSeconds = ev.durationSeconds;
        durationDays = durationSeconds / kSecondsPerDay;
    }
    if (durationSeconds <= 0) durationSeconds = allDay ? kSecondsPerDay : kSecondsPerHour;
    if (allDay && durationDays <= 0) durationDays = 1;
    const int64_t lookbackDays = std::max(durationDays,
        (durationSeconds + kSecondsPerDay - 1) / kSecondsPerDay);

    auto makeOccurrence = [&](const IcsTime& startTime, IcsOccurrence* occ) {
        int64_t start = 0;
        if (!IcsTimeToUnix(startTime, &start)) return false;
        occ->start = start;
        if (allDay) {
            int y, m, d;
            CivilFromDays(DaysFromCivil(startTime.y, startTime.mo, startTime.d) + durationDays, &y, &m, &d);
            IcsTime endTime = startTime;
            endTime.y = y; endTime.mo = m; endTime.d = d;
            int64_t end = 0;
            if (!IcsTimeToUnix(endTime, &end) || end <= start) end = start + durationSeconds;
            occ->end = end;
        } else {
            occ->end = start + durationSeconds;
        }
        return true;
    };
    auto overlaps = [&](const IcsOccurrence& occ) {
        return occ.end > winStart && occ.start < winEnd;
    };

    std::set<int64_t> excluded;
    for (const auto& ex : ev.exdates) {
        int64_t u = 0;
        if (IcsTimeToUnix(ex, &u)) excluded.insert(u);
    }

    IcsRule rule;
    bool recurring = !ev.rrule.empty() && ParseIcsRule(ev.rrule, &rule);
    if (!recurring) {
        IcsOccurrence occ;
        if (makeOccurrence(ev.start, &occ) && overlaps(occ) && !excluded.count(occ.start)) {
            out.push_back(occ);
        }
    } else {
        const int64_t startDay = DaysFromCivil(ev.start.y, ev.start.mo, ev.start.d);
        const int64_t windowEndDay = winEnd / kSecondsPerDay + 2;
        const int64_t todOfDay = ev.start.h * 3600 + ev.start.mi * 60 + ev.start.s;
        int64_t untilNaive = 0;
        if (rule.hasUntil) {
            untilNaive = CivilNaiveSeconds(rule.until.y, rule.until.mo, rule.until.d,
                                           rule.until.dateOnly ? 23 : rule.until.h,
                                           rule.until.dateOnly ? 59 : rule.until.mi,
                                           rule.until.dateOnly ? 59 : rule.until.s);
        }
        auto monthAllowed = [&](int m) {
            return rule.byMonth.empty() ||
                   std::find(rule.byMonth.begin(), rule.byMonth.end(), m) != rule.byMonth.end();
        };

        int emitted = 0;       // counts toward COUNT
        bool finished = false;
        auto handleDay = [&](int64_t day) {
            if (finished) return;
            if (day < startDay) return;
            if (day > windowEndDay) {
                finished = true;
                return;
            }
            if (rule.hasUntil && day * kSecondsPerDay > untilNaive + 2 * kSecondsPerDay) {
                finished = true;
                return;
            }
            // Unbounded rules do not need to resolve decades of old instances.
            if (rule.count <= 0 && day < winStart / kSecondsPerDay - 2 - lookbackDays) return;
            IcsTime t = ev.start;
            CivilFromDays(day, &t.y, &t.mo, &t.d);
            int64_t occurrenceStart = 0;
            // RRULE-generated nonexistent local times are ignored and do not
            // count toward COUNT (unlike explicit DTSTART/RDATE values).
            if (!IcsTimeToUnix(t, &occurrenceStart, false)) return;
            const int64_t comparison = rule.hasUntil && rule.until.utc
                ? occurrenceStart : day * kSecondsPerDay + todOfDay;
            if (rule.hasUntil && comparison > untilNaive) {
                finished = true;
                return;
            }
            if (rule.count > 0 && emitted >= rule.count) {
                finished = true;
                return;
            }
            ++emitted;
            if (day < winStart / kSecondsPerDay - 2 - lookbackDays) return;
            IcsOccurrence occ;
            if (makeOccurrence(t, &occ) && overlaps(occ) && !excluded.count(occ.start) &&
                out.size() < kMaxOccurrencesPerEvent) {
                out.push_back(occ);
            }
        };

        for (int64_t k = 0; k < kIcsGuardCandidates && !finished; ++k) {
            if (rule.freq == "DAILY") {
                int64_t day = startDay + k * rule.interval;
                int y, m, d;
                CivilFromDays(day, &y, &m, &d);
                if (!monthAllowed(m)) { if (day > windowEndDay) finished = true; continue; }
                bool dayOk = true;
                if (!rule.byDay.empty()) {
                    dayOk = false;
                    for (const auto& [ord, wd] : rule.byDay) {
                        if (wd == WeekdayOfDay(day)) dayOk = true;
                    }
                }
                if (dayOk && !rule.byMonthDay.empty()) {
                    dayOk = false;
                    for (int v : rule.byMonthDay) {
                        if ((v > 0 && v == d) || (v < 0 && DaysInMonth(y, m) + v + 1 == d)) dayOk = true;
                    }
                }
                if (dayOk) handleDay(day);
                else if (day > windowEndDay) finished = true;
            } else if (rule.freq == "WEEKLY") {
                int64_t weekBase = startDay - ((WeekdayOfDay(startDay) - rule.wkst + 7) % 7) +
                                   7 * static_cast<int64_t>(rule.interval) * k;
                std::vector<int64_t> days;
                if (rule.byDay.empty()) {
                    days.push_back(weekBase + ((WeekdayOfDay(startDay) - rule.wkst + 7) % 7));
                } else {
                    for (const auto& [ord, wd] : rule.byDay) {
                        days.push_back(weekBase + ((wd - rule.wkst + 7) % 7));
                    }
                }
                std::sort(days.begin(), days.end());
                days.erase(std::unique(days.begin(), days.end()), days.end());
                for (int64_t day : days) {
                    int y, m, d;
                    CivilFromDays(day, &y, &m, &d);
                    if (monthAllowed(m)) handleDay(day);
                }
                if (weekBase > windowEndDay) finished = true;
            } else if (rule.freq == "MONTHLY") {
                int64_t index = static_cast<int64_t>(ev.start.y) * 12 + (ev.start.mo - 1) +
                                k * rule.interval;
                int y = static_cast<int>(index / 12);
                int m = static_cast<int>(index % 12) + 1;
                if (monthAllowed(m)) {
                    for (int64_t day : MonthDays(y, m, rule, ev.start.d)) handleDay(day);
                }
                if (DaysFromCivil(y, m, 1) > windowEndDay) finished = true;
            } else {  // YEARLY
                int y = ev.start.y + static_cast<int>(k * rule.interval);
                std::vector<int> months = rule.byMonth;
                if (months.empty()) months.push_back(ev.start.mo);
                std::sort(months.begin(), months.end());
                for (int m : months) {
                    for (int64_t day : MonthDays(y, m, rule, ev.start.d)) handleDay(day);
                }
                if (DaysFromCivil(y, 1, 1) > windowEndDay) finished = true;
            }
        }
    }

    for (const auto& rd : ev.rdates) {
        IcsOccurrence occ;
        if (makeOccurrence(rd, &occ) && overlaps(occ) && !excluded.count(occ.start) &&
            out.size() < kMaxOccurrencesPerEvent) {
            out.push_back(occ);
        }
    }
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
        return a.start < b.start;
    });
    out.erase(std::unique(out.begin(), out.end(), [](const auto& a, const auto& b) {
        return a.start == b.start;
    }), out.end());
    return out;
}

// Parses ICS text into events overlapping [now - 1 day, now + 2 days].
FetchOutcome ParseIcsFeed(const std::string& text, const std::wstring& label, int64_t now) {
    FetchOutcome outcome;
    if (text.find("BEGIN:VCALENDAR") == std::string::npos &&
        text.find("begin:vcalendar") == std::string::npos) {
        outcome.failure = FetchFailure::Config;
        outcome.message = L"The feed is not an ICS calendar";
        return outcome;
    }

    // Unfold continuation lines.
    std::vector<std::string> lines;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find('\n', pos);
        std::string line = text.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && (line[0] == ' ' || line[0] == '\t') && !lines.empty()) {
            lines.back() += line.substr(1);
        } else {
            lines.push_back(std::move(line));
        }
        if (end == std::string::npos) break;
        pos = end + 1;
    }

    const int64_t winStart = now - kFetchPastSeconds;
    const int64_t winEnd = now + kFetchFutureSeconds;
    std::vector<IcsEvent> events;
    IcsEvent current;
    bool inEvent = false;
    int nestedDepth = 0;
    int components = 0;
    for (const auto& line : lines) {
        if (line.empty()) continue;
        if (!inEvent) {
            if (UpperAscii(line) == "BEGIN:VEVENT") {
                inEvent = true;
                nestedDepth = 0;
                current = IcsEvent();
                if (++components > kMaxIcsComponents) { outcome.partial = true; break; }
            }
            continue;
        }
        const std::string upperLine = UpperAscii(line);
        if (upperLine.rfind("BEGIN:", 0) == 0) {
            ++nestedDepth;
            continue;
        }
        if (nestedDepth > 0) {
            if (upperLine.rfind("END:", 0) == 0) --nestedDepth;
            continue;
        }
        if (upperLine == "END:VEVENT") {
            inEvent = false;
            if (current.start.valid || (current.status == "CANCELLED" && current.hasRecId))
                events.push_back(current);
            continue;
        }
        IcsProp prop;
        if (!ParseIcsLine(line, &prop)) continue;
        if (prop.name == "UID") current.uid = prop.value.substr(0, 256);
        else if (prop.name == "SUMMARY") current.summary = IcsUnescape(prop.value);
        else if (prop.name == "LOCATION") {
            current.location = IcsUnescape(prop.value);
            current.linkValues.push_back(current.location);
        } else if (prop.name == "DESCRIPTION") {
            current.description = IcsUnescape(prop.value);
            current.linkValues.push_back(current.description);
        } else if (prop.name == "URL" || prop.name == "CONFERENCE" ||
                   prop.name == "X-GOOGLE-CONFERENCE") {
            current.linkValues.push_back(IcsUnescape(prop.value));
        } else if (prop.name == "STATUS") current.status = UpperAscii(prop.value);
        else if (prop.name == "DTSTART") ParseIcsTimeValue(prop, &current.start);
        else if (prop.name == "DTEND") current.hasEnd = ParseIcsTimeValue(prop, &current.end);
        else if (prop.name == "DURATION") current.durationSeconds = ParseIcsDuration(prop.value);
        else if (prop.name == "RRULE") current.rrule = prop.value;
        else if (prop.name == "EXDATE" || prop.name == "RDATE") {
            for (const auto& piece : SplitString(prop.value, ',')) {
                IcsProp one = prop;
                one.value = piece;
                IcsTime t;
                if (!ParseIcsTimeValue(one, &t)) continue;
                (prop.name == "EXDATE" ? current.exdates : current.rdates).push_back(t);
            }
        } else if (prop.name == "RECURRENCE-ID") {
            current.hasRecId = ParseIcsTimeValue(prop, &current.recId);
        }
    }

    if (inEvent) outcome.partial = true;  // truncated/malformed component
    std::erase_if(events, [&](const IcsEvent& ev) {
        int64_t value = 0;
        bool valid = ev.status == "CANCELLED" || IcsTimeToUnix(ev.start, &value);
        if (ev.hasEnd) valid = valid && IcsTimeToUnix(ev.end, &value);
        if (ev.hasRecId) valid = valid && IcsTimeToUnix(ev.recId, &value);
        for (const auto& time : ev.exdates) valid = valid && IcsTimeToUnix(time, &value);
        for (const auto& time : ev.rdates) valid = valid && IcsTimeToUnix(time, &value);
        if (!ev.rrule.empty()) {
            IcsRule rule;
            valid = valid && ParseIcsRule(ev.rrule, &rule);
        }
        if (!valid) outcome.partial = true;
        return !valid;
    });

    // Overridden occurrences (RECURRENCE-ID) replace the generated ones.
    std::map<std::string, std::set<int64_t>> overridden;
    for (const auto& ev : events) {
        int64_t u = 0;
        if (ev.hasRecId && !ev.uid.empty() && IcsTimeToUnix(ev.recId, &u)) overridden[ev.uid].insert(u);
    }

    for (const auto& ev : events) {
        if (ev.status == "CANCELLED") continue;
        std::vector<IcsOccurrence> occurrences = ExpandIcsEvent(ev, winStart, winEnd);
        for (const auto& occ : occurrences) {
            if (!ev.hasRecId) {
                auto it = overridden.find(ev.uid);
                if (it != overridden.end() && it->second.count(occ.start)) continue;
            }
            if (static_cast<int>(outcome.events.size()) >= kMaxEventsTotal) {
                outcome.partial = true;
                break;
            }
            AgendaEntry entry;
            entry.description = SafeText(ev.description, 1000);
            entry.title = SafeText(ev.summary, 180);
            if (entry.title.empty()) entry.title = L"Untitled event";
            entry.location = SafeText(ev.location, 180);
            entry.source = SafeText(label, 96);
            entry.startUnix = occ.start;
            entry.endUnix = occ.end;
            entry.allDay = ev.start.dateOnly;
            entry.isActive = !entry.allDay && occ.start <= now && now < occ.end;
            entry.fromIcs = true;
            entry.hasResponseState = true;
            std::wstring meetCode, provider, url;
            ScanMeetingLinks(ev.linkValues, &meetCode, &provider, &url);
            if (!meetCode.empty()) {
                entry.googleMeetCode = meetCode;
            } else if (!provider.empty()) {
                entry.meetingProvider = provider;
                entry.meetingUrl = url;
            }
            entry.hasMeetCode = entry.hasMeetingProvider = entry.hasMeetingUrl = true;
            entry.dedupKey = Utf8ToWide(ev.uid) + L"|" + std::to_wstring(occ.start);
            if (ev.uid.empty()) entry.dedupKey.clear();
            outcome.events.push_back(std::move(entry));
        }
    }
    return outcome;
}

// https only (webcal:// is mapped to https://), default port, no credentials in the URL.
bool SplitFeedUrl(std::wstring url, std::wstring* host, std::wstring* path) {
    std::wstring lower = LowerCopy(url);
    if (lower.rfind(L"webcals://", 0) == 0) url = L"https://" + url.substr(10);
    else if (lower.rfind(L"webcal://", 0) == 0) url = L"https://" + url.substr(9);
    if (LowerCopy(url).rfind(L"https://", 0) != 0 || url.size() > 2048) return false;
    for (wchar_t ch : url) {
        if (ch < 0x21 || ch > 0x7E) return false;
    }
    size_t hostEnd = url.find_first_of(L"/?#", 8);
    std::wstring h = url.substr(8, hostEnd == std::wstring::npos ? std::wstring::npos : hostEnd - 8);
    if (h.empty() || h.find(L'@') != std::wstring::npos || h.find(L':') != std::wstring::npos) return false;
    std::wstring p = hostEnd == std::wstring::npos ? L"/" : url.substr(hostEnd);
    size_t hash = p.find(L'#');
    if (hash != std::wstring::npos) p.resize(hash);
    if (p.empty() || p[0] != L'/') p = L"/" + p;
    *host = LowerCopy(h);
    *path = p;
    return true;
}

FetchOutcome FetchIcsFeed(const IcsFeedConfig& feed, int64_t now) {
    FetchOutcome outcome;
    std::wstring host, path;
    if (!SplitFeedUrl(feed.url, &host, &path)) {
        outcome.failure = FetchFailure::Config;
        outcome.message = L"Invalid ICS feed URL";
        return outcome;
    }
    for (int hop = 0; hop <= kMaxIcsRedirects; ++hop) {
        HttpResponse http = HttpsRequest(host.c_str(), path, L"GET",
                                         L"Accept: text/calendar, text/plain, */*\r\n", "",
                                         kMaxIcsBytes, true);
        if (http.tooLarge) {
            outcome.failure = FetchFailure::Config;
            outcome.message = L"ICS feed is too large";
            return outcome;
        }
        if (!http.completed && http.status == 0) {
            outcome.failure = FetchFailure::Transient;
            outcome.message = L"Could not reach the ICS feed";
            return outcome;
        }
        if (http.status >= 300 && http.status < 400 && !http.location.empty()) {
            std::wstring next = http.location;
            if (next[0] == L'/') next = L"https://" + host + next;
            if (!SplitFeedUrl(next, &host, &path)) {
                outcome.failure = FetchFailure::Config;
                outcome.message = L"The ICS feed redirected somewhere unsupported";
                return outcome;
            }
            continue;
        }
        if (http.status != 200) {
            outcome.failure = (http.status == 429 || http.status >= 500) ? FetchFailure::Transient
                                                                         : FetchFailure::Config;
            outcome.message = L"The ICS feed returned HTTP " + std::to_wstring(http.status);
            return outcome;
        }
        std::string body = std::move(http.body);
        if (body.size() >= 3 && static_cast<unsigned char>(body[0]) == 0xEF &&
            static_cast<unsigned char>(body[1]) == 0xBB && static_cast<unsigned char>(body[2]) == 0xBF) {
            body.erase(0, 3);
        }
        return ParseIcsFeed(body, feed.label, now);
    }
    outcome.failure = FetchFailure::Config;
    outcome.message = L"Too many redirects fetching the ICS feed";
    return outcome;
}

// --- Reminder toasts --------------------------------------------------------------------------

struct ToastItem {
    std::string key;
    std::wstring title;
    std::wstring body;
    std::wstring url;
};

std::map<std::string, int64_t> g_notified;

std::string NotificationKey(const AgendaEntry& e, const char* kind) {
    std::string raw = WideToUtf8(e.source) + '\x1f' + WideToUtf8(e.title) + '\x1f' +
                      std::to_string(e.startUnix) + '\x1f' + kind;
    char buffer[24];
    std::snprintf(buffer, sizeof(buffer), "%016llx",
                  static_cast<unsigned long long>(Fnv1a64(raw)));
    return buffer;
}

void LoadNotifiedState() {
    g_notified.clear();
    wchar_t buffer[8192];
    size_t chars = Wh_GetStringValue(UserValueName(kNotifiedValueName).c_str(), buffer, ARRAYSIZE(buffer));
    if (!chars) return;
    std::string data = WideToUtf8(std::wstring(buffer, chars));
    size_t pos = 0;
    while (pos < data.size()) {
        size_t end = data.find(';', pos);
        std::string item = data.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
        size_t eq = item.find('=');
        if (eq == 16) {
            char* stop = nullptr;
            long long ts = std::strtoll(item.c_str() + eq + 1, &stop, 10);
            if (stop && *stop == '\0' && ts > 0) g_notified[item.substr(0, eq)] = ts;
        }
        if (end == std::string::npos) break;
        pos = end + 1;
    }
}

void SaveNotifiedState() {
    std::string data;
    for (const auto& [key, ts] : g_notified) {
        data += key + "=" + std::to_string(ts) + ";";
    }
    if (data.size() > 7000) {  // keep the stored value bounded; drop oldest first
        std::vector<std::pair<int64_t, std::string>> byAge;
        for (const auto& [key, ts] : g_notified) byAge.push_back({ts, key});
        std::sort(byAge.begin(), byAge.end());
        while (data.size() > 7000 && !byAge.empty()) {
            g_notified.erase(byAge.front().second);
            byAge.erase(byAge.begin());
            data.clear();
            for (const auto& [key, ts] : g_notified) data += key + "=" + std::to_string(ts) + ";";
        }
    }
    Wh_SetStringValue(UserValueName(kNotifiedValueName).c_str(), Utf8ToWide(data).c_str());
}

std::wstring XmlEscape(const std::wstring& text) {
    std::wstring out;
    for (wchar_t ch : text) {
        switch (ch) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'"': out += L"&quot;"; break;
            case L'\'': out += L"&apos;"; break;
            default: out.push_back(ch);
        }
    }
    return out;
}

std::atomic<bool> g_toastRegistered{false};

// Registers the Windows Calendar Plus notification identity when a reminder is
// first shown. The storage flag lets
// the next start clean up after an unclean exit, when the unload path never ran.
bool EnsureToastRegistration() {
    if (g_toastRegistered.load()) return true;
    Wh_SetIntValue(UserValueName(kToastRegisteredFlag).c_str(), 1);
    std::wstring subKey = std::wstring(L"Software\\Classes\\AppUserModelId\\") + kToastAumid;
    LSTATUS status = RegSetKeyValueW(
        HKEY_CURRENT_USER, subKey.c_str(), L"DisplayName", REG_SZ, kToastDisplayName,
        static_cast<DWORD>((wcslen(kToastDisplayName) + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return false;
    g_toastRegistered = true;
    return true;
}

// Removes the key above and the per-app notification settings Windows creates under
// HKCU notification settings for this mod when the first toast is shown.
void RemoveToastRegistration() {
    RegDeleteTreeW(HKEY_CURRENT_USER,
                   (std::wstring(L"Software\\Classes\\AppUserModelId\\") + kToastAumid).c_str());
    RegDeleteTreeW(HKEY_CURRENT_USER,
                   (std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\"
                                 L"Settings\\") +
                    kToastAumid)
                       .c_str());
    Wh_DeleteValue(UserValueName(kToastRegisteredFlag).c_str());
    g_toastRegistered = false;
}

bool ShowToast(const ToastItem& item) {
    if (!EnsureToastRegistration()) return false;
    try {
        std::wstring xml = L"<toast";
        if (!item.url.empty()) {
            xml += L" activationType=\"protocol\" launch=\"" + XmlEscape(item.url) + L"\"";
        }
        xml += L"><visual><binding template=\"ToastGeneric\"><text>" + XmlEscape(item.title) +
               L"</text><text>" + XmlEscape(item.body) + L"</text></binding></visual>";
        if (!item.url.empty()) {
            xml += L"<actions><action content=\"Join\" activationType=\"protocol\" arguments=\"" +
                   XmlEscape(item.url) + L"\"/></actions>";
        }
        xml += L"</toast>";
        winrt::Windows::Data::Xml::Dom::XmlDocument doc;
        doc.LoadXml(xml);
        winrt::Windows::UI::Notifications::ToastNotification toast(doc);
        winrt::Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier(kToastAumid)
            .Show(toast);
        return true;
    } catch (...) {
        LogCaughtException(L"show toast");
        return false;
    }
}

bool NotifiableEntry(const AgendaEntry& e, bool icsNotifications) {
    if (e.allDay || e.outOfOffice || e.startUnix <= 0) return false;
    if (e.fromIcs) return icsNotifications;  // ICS feeds carry no accept/decline status
    return e.responseState == AgendaEntry::ResponseState::Accepted;
}

std::vector<ToastItem> DueToasts(const std::vector<AgendaEntry>& entries, int64_t now,
                                 const ModSettings& settings) {
    std::vector<ToastItem> due;
    const int64_t lead = static_cast<int64_t>(settings.notify_lead_minutes) * kSecondsPerMinute;
    for (const auto& e : entries) {
        if (!NotifiableEntry(e, settings.ics_notifications)) continue;
        std::wstring url = MeetingJoinUrl(MeetingTarget(e));
        std::wstring clock = FormatUnixTime(e.startUnix, settings.use_24_hour_time);
        if (e.startUnix - lead <= now && now < e.startUnix) {
            std::string key = NotificationKey(e, "lead");
            if (!g_notified.count(key)) {
                int64_t minutes = std::max<int64_t>(1, (e.startUnix - now + 59) / 60);
                due.push_back({key, e.title,
                               L"Starts in " + std::to_wstring(minutes) + L" min \u00B7 " + clock,
                               url});
            }
        } else if (e.startUnix <= now && now < e.startUnix + kNotificationGraceSeconds) {
            std::string key = NotificationKey(e, "start");
            if (!g_notified.count(key)) {
                due.push_back({key, e.title, L"Starting now \u00B7 " + clock, url});
            }
        }
    }
    return due;
}

void RunNotifications(const ModSettings& settings, const std::vector<AgendaEntry>& entries,
                      int64_t now) {
    bool changed = false;
    for (auto it = g_notified.begin(); it != g_notified.end();) {
        if (it->second < now - kNotificationRetentionSeconds) {
            it = g_notified.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    for (const auto& item : DueToasts(entries, now, settings)) {
        if (g_workerStop.load()) break;
        if (ShowToast(item)) {
            g_notified[item.key] = now;
            changed = true;
        }
    }
    if (changed) SaveNotifiedState();
}

// Seconds until the next reminder boundary (lead start or event start), or -1.
int64_t NextNotificationBoundary(const ModSettings& settings,
                                 const std::vector<AgendaEntry>& entries, int64_t now) {
    const int64_t lead = static_cast<int64_t>(settings.notify_lead_minutes) * kSecondsPerMinute;
    int64_t best = -1;
    for (const auto& e : entries) {
        if (!NotifiableEntry(e, settings.ics_notifications)) continue;
        for (int64_t boundary : {e.startUnix - lead, e.startUnix}) {
            if (boundary > now && (best < 0 || boundary - now < best)) best = boundary - now;
        }
    }
    return best;
}

// --- Provider worker --------------------------------------------------------------------------------

void PublishFetchFailure(const std::wstring& message) {
    PublishStatus(message, g_hasPublishedEvents.load());
}

// A small, per-user, versioned interface for the independent Desktop Island.
// No account IDs, URLs, credentials, descriptions or calendar feed settings
// leave this provider. Failure here must never fail the native calendar.
std::wstring IslandPreviewPath(bool createDirectory) {
    DWORD needed = GetEnvironmentVariableW(L"LOCALAPPDATA", nullptr, 0);
    if (needed < 2 || needed > 32768) return {};
    std::wstring base(needed, L'\0');
    DWORD actual = GetEnvironmentVariableW(L"LOCALAPPDATA", base.data(), needed);
    if (!actual || actual >= needed) return {};
    base.resize(actual);
    base += L"\\WindhawkPersonal";
    if (createDirectory && !CreateDirectoryW(base.c_str(), nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS) return {};
    return base + L"\\calendar-island-v1.tsv";
}

void RemoveIslandPreview() {
    const std::wstring path = IslandPreviewPath(false);
    if (!path.empty()) DeleteFileW(path.c_str());
}

bool PublishIslandPreview(const std::vector<AgendaEntry>& events, int64_t now) {
    bool previewEnabled;
    {
        std::lock_guard lock(g_settingsMutex);
        previewEnabled = g_settings.island_preview_enabled;
    }
    if (!previewEnabled) {
        RemoveIslandPreview();
        return true;
    }
    const std::wstring path = IslandPreviewPath(true);
    if (path.empty()) return false;

    std::vector<const AgendaEntry*> upcoming;
    for (const auto& event : events) {
        if (event.endUnix > now && event.startUnix >= now - 86400 &&
            event.startUnix < now + 48 * 3600 &&
            !event.title.empty()) upcoming.push_back(&event);
    }
    std::sort(upcoming.begin(), upcoming.end(),
              [](const AgendaEntry* a, const AgendaEntry* b) {
                  return a->startUnix < b->startUnix;
              });

    std::string payload = "WH-CALENDAR-ISLAND-1\n" + std::to_string(now) + "\n";
    for (size_t i = 0; i < upcoming.size() && i < 6; ++i) {
        std::wstring title = upcoming[i]->title.substr(0, 100);
        if (!title.empty() && title.back() >= 0xD800 && title.back() <= 0xDBFF)
            title.pop_back(); // Do not split a UTF-16 surrogate pair at the limit.
        for (wchar_t& c : title) {
            if (c == L'\t' || c == L'\r' || c == L'\n' || c < 0x20) c = L' ';
        }
        payload += std::to_string(upcoming[i]->startUnix) + "\t" +
                   std::to_string(upcoming[i]->endUnix) + "\t" +
                   (upcoming[i]->allDay ? "1" : "0") + "\t" +
                   WideToUtf8(title) + "\n";
    }
    if (payload.size() > 8192) return false;

    const std::wstring temp = path + L".tmp" + std::to_wstring(GetCurrentProcessId());
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = WriteFile(file, payload.data(), static_cast<DWORD>(payload.size()),
                        &written, nullptr) && written == payload.size();
    if (ok) ok = FlushFileBuffers(file) != FALSE;
    CloseHandle(file);
    if (ok) ok = MoveFileExW(temp.c_str(), path.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
    if (!ok) DeleteFileW(temp.c_str());
    return ok;
}


// Same mod ID in Explorer and ShellHost: Windhawk brokers this per-user cache.
// Tokens remain DPAPI-encrypted and are never included in this event snapshot.
bool PublishCalendarEvents(const std::vector<AgendaEntry>& events, int64_t now, bool partial) {
    using winrt::Windows::Data::Json::JsonValue;
    JsonObject root; JsonArray items;
    root.SetNamedValue(L"generated", JsonValue::CreateNumberValue(static_cast<double>(now)));
    root.SetNamedValue(L"partial", JsonValue::CreateBooleanValue(partial));
    for (const auto& e : events) {
        JsonObject item;
        item.SetNamedValue(L"title", JsonValue::CreateStringValue(e.title));
        item.SetNamedValue(L"description", JsonValue::CreateStringValue(e.description));
        item.SetNamedValue(L"location", JsonValue::CreateStringValue(e.location));
        item.SetNamedValue(L"source", JsonValue::CreateStringValue(e.source));
        item.SetNamedValue(L"start", JsonValue::CreateNumberValue(static_cast<double>(e.startUnix)));
        item.SetNamedValue(L"end", JsonValue::CreateNumberValue(static_cast<double>(e.endUnix)));
        item.SetNamedValue(L"allDay", JsonValue::CreateBooleanValue(e.allDay));
        std::wstring url = e.meetingUrl;
        if (!e.googleMeetCode.empty()) url = L"https://meet.google.com/" + e.googleMeetCode;
        item.SetNamedValue(L"url", JsonValue::CreateStringValue(url));
        items.Append(item);
    }
    root.SetNamedValue(L"items", items);
    std::wstring payload(root.Stringify());
    if (payload.size() >= kMaxCalendarSnapshotChars) return false;
    // Keep a high-water size hint. Publishing a smaller snapshot must not leave
    // a concurrent reader with a buffer too small for the preceding snapshot.
    const auto sizeKey = UserValueName(L"calendar_events_chars");
    int previous = Wh_GetIntValue(sizeKey.c_str(), 0);
    size_t capacity = std::max(payload.size() + 1, static_cast<size_t>(
        std::clamp(previous, 0, static_cast<int>(kMaxCalendarSnapshotChars))));
    if (!Wh_SetIntValue(sizeKey.c_str(), static_cast<int>(capacity))) return false;
    if (!Wh_SetStringValue(UserValueName(L"calendar_events").c_str(), payload.c_str()))
        return false;
    try {
        PublishIslandPreview(events, now);
    } catch (...) {
        // The optional companion must not invalidate the native calendar cache.
        Wh_Log(L"Desktop Island preview could not be published");
    }
    return true;
}

bool ReadCalendarPayload(std::wstring& payload, std::vector<wchar_t>& buffer) {
    const auto sizeKey = UserValueName(L"calendar_events_chars");
    for (int attempt = 0; attempt < 2; ++attempt) {
        int hint = Wh_GetIntValue(sizeKey.c_str(), 0);
        size_t capacity = static_cast<size_t>(std::clamp(hint, 4 * 1024 * 1024,
            static_cast<int>(kMaxCalendarSnapshotChars)));
        if (buffer.size() < capacity) buffer.resize(capacity);
        size_t n = Wh_GetStringValue(UserValueName(L"calendar_events").c_str(),
                                    buffer.data(), buffer.size());
        if (n) { payload.assign(buffer.data(), n); return true; }
        if (hint == 0 || Wh_GetIntValue(sizeKey.c_str(), 0) == 0) {
            payload.clear();
            return true;  // cache cleared after disabling/signing out
        }
    }
    return false;  // keep the last readable events on transient/storage errors
}

void ProviderWorkerMain() {
    int lastCalendarRefresh = Wh_GetIntValue(UserValueName(L"calendar_refresh").c_str(), 0);
    bool apartmentReady = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentReady = true;
    } catch (...) {
        LogCaughtException(L"worker apartment");
        PublishStatus(L"Could not initialize calendar provider", false);
        return;
    }
    try { LoadNotifiedState(); } catch (...) { LogCaughtException(L"load reminder state"); }

    int64_t nextFetch = 0;
    std::vector<AgendaEntry> notifyEntries;
    uint64_t notifyGeneration = g_accountGeneration.load();

    while (!g_workerStop.load()) {
        try {
            ProcessAccountRequests();
            uint64_t generation = g_accountGeneration.load();
            ModSettings settings = SettingsCopy();
            kFetchPastSeconds = std::clamp(Wh_GetIntSetting(L"history_days"), 1, 90) * 86400LL;
            kFetchFutureSeconds = std::clamp(Wh_GetIntSetting(L"future_days"), 1, 180) * 86400LL;
            ReevaluateAuthState(settings);
            RevokePendingToken();
            int64_t now = NowUnix();
            int request = Wh_GetIntValue(UserValueName(L"calendar_refresh").c_str(), 0);
            bool requested = request != lastCalendarRefresh;
            lastCalendarRefresh = request;
            bool force = g_forceRefresh.exchange(false) || requested;
    
            if (!settings.enabled) {
                PublishStatus(L"Disabled", false);
                notifyEntries.clear();
            } else if (GetAuthState() == AuthState::SignedIn) {
                if (force || now >= nextFetch || nextFetch == 0) {
                    FetchOutcome outcome = FetchAllSources(settings, now);
                    if (g_workerStop.load()) break;
                    std::lock_guard<std::mutex> publishLock(g_publishMutex);
                    if (generation != g_accountGeneration.load()) {
                        notifyEntries.clear();
                        nextFetch = 0;
                        continue;  // signed-out sources must never be republished
                    }
                    if (outcome.failure == FetchFailure::None) {
                        if (!PublishCalendarEvents(outcome.events, now, outcome.partial)) {
                            PublishFetchFailure(L"Could not save calendar data for the native pane");
                            nextFetch = now + 60;
                            continue;
                        }
                        PublishStatus(outcome.partial
                            ? L"Some calendars or events could not be loaded" : L"", true);
                        notifyEntries.clear();
                        notifyGeneration = generation;
                        for (const auto& e : outcome.events) {
                            if (!e.allDay) notifyEntries.push_back(e);
                        }
                        nextFetch = now + settings.poll_seconds;
                    } else {
                        PublishFetchFailure(outcome.failure == FetchFailure::Auth
                                                ? L"Google sign-in needed. Open Google accounts from the three-dot menu."
                                                : outcome.message);
                        nextFetch = now + std::min(settings.poll_seconds, 60);
                    }
                }
            } else {
                PublishStatus(L"Not signed in", false);
                notifyEntries.clear();
                nextFetch = 0;
            }
    
            if (settings.enabled && settings.notifications_enabled) {
                std::lock_guard<std::mutex> publishLock(g_publishMutex);
                if (notifyGeneration != g_accountGeneration.load()) notifyEntries.clear();
                RunNotifications(settings, notifyEntries, NowUnix());
            }
    
            // Sleep until the next fetch or reminder boundary, in short slices so the
            // loop also tolerates system sleep/resume clock jumps.
            int64_t waitSeconds = 15;
            int64_t after = NowUnix();
            if (nextFetch > after) waitSeconds = std::min(waitSeconds, nextFetch - after);
            if (settings.enabled && settings.notifications_enabled) {
                int64_t boundary = NextNotificationBoundary(settings, notifyEntries, after);
                if (boundary > 0) waitSeconds = std::min(waitSeconds, boundary);
            }
            DWORD waitMs = static_cast<DWORD>(std::max<int64_t>(1, waitSeconds) * 1000);
            WaitForSingleObject(g_workerWakeEvent, waitMs);
        } catch (...) {
            LogCaughtException(L"calendar provider iteration");
            try {
                std::lock_guard<std::mutex> publishLock(g_publishMutex);
                PublishFetchFailure(L"Calendar refresh failed. Retrying shortly.");
            } catch (...) {}
            WaitForSingleObject(g_workerWakeEvent, 15000);
        }
    }

    ClearZoneCalendars();
    if (apartmentReady) winrt::uninit_apartment();
}

void ProviderShutdown() {
    g_signInCancel = true;
    AbortAllHttp();
    std::lock_guard<std::mutex> lock(g_signInMutex);
    if (g_signInThread) {
        if (g_signInThread->joinable()) g_signInThread->join();
        g_signInThread.reset();
    }
}

void WorkerThreadProc() {
    ProviderWorkerMain();
}

void StartWorkerThread() {
    if (g_workerThread) {
        return;
    }
    g_workerStop = false;
    g_workerThread.emplace(WorkerThreadProc);
}

std::atomic<bool> g_providerStarted{false};

// Windhawk loads the mod into every explorer.exe (folder windows in their own process, COM
// servers, ...). Only the process that hosts the taskbar runs the provider, so calendars are
// polled once and reminders are shown once.
void StartProviderInShellProcess() {
    if (g_userKey.empty()) {
        return;  // without the user's SID the per-user storage cannot be addressed safely
    }
    if (g_providerStarted.exchange(true)) {
        return;
    }
    {
        std::lock_guard lock(g_settingsMutex);
        if (!g_settings.island_preview_enabled) RemoveIslandPreview();
    }
    // A previous shell that never reached Uninit (crash, killed Explorer) left its
    // notification registration behind: remove it before registering again.
    if (Wh_GetIntValue(UserValueName(kToastRegisteredFlag).c_str(), 0) != 0) {
        RemoveToastRegistration();
    }
    try { StartWorkerThread(); }
    catch (...) {
        g_providerStarted = false;
        LogCaughtException(L"start calendar provider");
    }
}

void StopWorkerThread() {
    g_workerStop = true;
    g_signInCancel = true;
    AbortAllHttp();
    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }
    if (g_workerThread) {
        if (g_workerThread->joinable()) g_workerThread->join();
        g_workerThread.reset();
    }
    ProviderShutdown();
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) CALLBACK -> BOOL {
            DWORD pid = 0;
            wchar_t className[64]{};
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

}  // namespace

BOOL Init() {
    g_unloading = false;
    g_workerStop = false;
    g_userKey = CurrentUserSid();
    if (g_userKey.empty()) {
        Wh_Log(L"could not read the user SID; the calendar provider stays off");
    }
    LoadSettings();

    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workerWakeEvent) {
        Wh_Log(L"failed to create worker wake event");
        return FALSE;
    }
    return TRUE;
}

void AfterInit() {
    // Explorer can load the mod before it creates Shell_TrayWnd. Wait for the
    // owning shell process so separate folder-window processes never fetch or toast.
    g_startupThread.emplace([] {
        while (!g_unloading.load()) {
            if (FindCurrentProcessTaskbarWnd()) {
                StartProviderInShellProcess();
                break;
            }
            if (WaitForSingleObject(g_workerWakeEvent, 1000) == WAIT_FAILED) break;
        }
    });
}

void Uninit() {
    g_unloading = true;
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
    if (g_startupThread) {
        if (g_startupThread->joinable()) g_startupThread->join();
        g_startupThread.reset();
    }
    StopWorkerThread();
    if (g_providerStarted.load()) {
        RemoveIslandPreview();
        RemoveToastRegistration();
    }

    if (g_workerWakeEvent) {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }
}

}
namespace native_agenda {








// Parts of the mod were ported from the "Start Menu Styler" mod by m417z
// This includes:
// - The injection hook into the Notification Centre
// For this reason, the original author's license has been attached:

// Copyright (C) 2026 m417z
// Copyright (C) 2026 lonfro
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

// Modified by lonfro in 2026.
// Original work by m417z.


#undef GetCurrentTime






namespace wf = winrt::Windows::Foundation;
namespace ws = winrt::Windows::System;
namespace wadt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuc = winrt::Windows::UI::Core;

std::atomic<ULONGLONG> g_lastOpenTick = 0;
std::atomic<ULONGLONG> g_lastFetchTick = 0;

inline void NormalizeSystemTime(SYSTEMTIME& st) {
    FILETIME ft{};
    if (SystemTimeToFileTime(&st, &ft)) {
        FileTimeToSystemTime(&ft, &st);
    }
}

inline void CopyTextToClipboard(const std::wstring& text) {
    if (text.empty()) return;
    try {
        wadt::DataPackage package;
        package.SetText(text);
        wadt::Clipboard::SetContent(package);
    } catch (...) {}
}

inline SYSTEMTIME ShiftLocalDate(const SYSTEMTIME& stLocal, int deltaDays) {
    if (deltaDays == 0)
        return stLocal;
    FILETIME ft{};
    SystemTimeToFileTime(&stLocal, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    ULONGLONG dayTicks = 864000000000ULL;
    if (deltaDays > 0) {
        uli.QuadPart += (ULONGLONG)deltaDays * dayTicks;
    } else {
        uli.QuadPart -= (ULONGLONG)(-deltaDays) * dayTicks;
    }
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    SYSTEMTIME result{};
    FileTimeToSystemTime(&ft, &result);
    return result;
}

inline int64_t ToFileTimeDays(const SYSTEMTIME& st) {
    SYSTEMTIME dOnly = st;
    dOnly.wHour = dOnly.wMinute = dOnly.wSecond = dOnly.wMilliseconds = 0;
    FILETIME ft{};
    SystemTimeToFileTime(&dOnly, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return static_cast<int64_t>(uli.QuadPart / 864000000000ULL);
}

inline int DaysBetween(const SYSTEMTIME& from, const SYSTEMTIME& to) {
    return static_cast<int>(ToFileTimeDays(to) - ToFileTimeDays(from));
}

inline int CompareDateOnly(const SYSTEMTIME& a, const SYSTEMTIME& b) {
    if (a.wYear != b.wYear)
        return (a.wYear < b.wYear) ? -1 : 1;
    if (a.wMonth != b.wMonth)
        return (a.wMonth < b.wMonth) ? -1 : 1;
    if (a.wDay != b.wDay)
        return (a.wDay < b.wDay) ? -1 : 1;
    return 0;
}

inline int GetDaysInMonth(int year, int month) {
    static const int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        return isLeap ? 29 : 28;
    }
    if (month >= 1 && month <= 12) return days[month];
    return 30;
}

inline std::wstring FormatDateYmd(const SYSTEMTIME& st) {
    WCHAR buf[16];
    swprintf_s(buf, L"%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
    return std::wstring(buf);
}

inline SYSTEMTIME ApplyOffsetToUtc(const SYSTEMTIME& stRaw, int offsetMinutes) {
    FILETIME ft{};
    SystemTimeToFileTime(&stRaw, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    int64_t offsetTicks = static_cast<int64_t>(offsetMinutes) * 600000000LL;
    int64_t utcTicks = static_cast<int64_t>(uli.QuadPart) - offsetTicks;
    if (utcTicks < 0)
        utcTicks = 0;
    uli.QuadPart = static_cast<ULONGLONG>(utcTicks);

    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    SYSTEMTIME stUtc{};
    FileTimeToSystemTime(&ft, &stUtc);
    return stUtc;
}

inline SYSTEMTIME TzToSystemLocal(const SYSTEMTIME& stRaw, int offsetMinutes) {
    SYSTEMTIME stUtc = ApplyOffsetToUtc(stRaw, offsetMinutes);
    SYSTEMTIME stLocal{};
    if (SystemTimeToTzSpecificLocalTime(nullptr, &stUtc, &stLocal)) {
        return stLocal;
    }
    return stUtc;
}

inline SYSTEMTIME ToLocal(const SYSTEMTIME& stUtc, bool isUtc) {
    if (!isUtc)
        return stUtc;
    SYSTEMTIME stLocal{};
    if (SystemTimeToTzSpecificLocalTime(nullptr, &stUtc, &stLocal)) {
        return stLocal;
    }
    return stUtc;
}

enum class RecurrenceFreq {
    None,
    Daily,
    Weekly,
    Monthly,
    Yearly
};

struct ByDayItem {
    int ord = 0;        // 0 = every; +1 = 1st, +2 = 2nd, -1 = last, -2 = 2nd-to-last, etc.
    int dayOfWeek = 0;  // 0 = Sun, 1 = Mon, ..., 6 = Sat (matching SYSTEMTIME wDayOfWeek)
};

struct RecurrenceRule {
    RecurrenceFreq freq = RecurrenceFreq::None;
    int interval = 1;
    int count = 0;  // 0 = unlimited
    SYSTEMTIME untilUtc{};
    bool hasUntil = false;
    uint8_t byDayMask = 0;  // bit 0 = Sun, 1 = Mon, ..., 6 = Sat
    std::vector<ByDayItem> byDays;
    std::vector<int> byMonthDays;
    int wkst = 1; // 0 = Sun, 1 = Mon, ..., 6 = Sat (RFC 5545 default: 1 = Monday)
};

struct CalendarEvent {
    std::wstring source;
    std::wstring uid;
    std::wstring recurrenceId;
    std::wstring status;
    SYSTEMTIME startLocal{};
    SYSTEMTIME endLocal{};
    bool hasEnd = false;
    bool isAllDay = false;
    ULONGLONG sortKey = 0;
    std::wstring name;
    std::wstring location;
    std::wstring notes;
    std::wstring url;
    bool hasRRule = false;
    RecurrenceRule rrule;
    std::vector<std::wstring> exDates;
};


std::mutex g_eventsMutex;
std::wstring g_providerStatus;
int64_t g_snapshotUnix = 0;
bool g_partialSnapshot = false;
SYSTEMTIME UnixToLocal(int64_t unixSeconds, bool allDay) {
    SYSTEMTIME local{};
    if (!calendar_provider::UnixToLocalSystemTime(unixSeconds, &local)) return {};
    // Calendar dates stay dates. The provider stores all-day boundaries as local midnight.
    if (!allDay) {
        int correction=std::clamp(Wh_GetIntSetting(L"time_offset_minutes"),-1440,1440);
        if (correction) { FILETIME wall{}; SystemTimeToFileTime(&local,&wall);
            ULARGE_INTEGER value{}; value.LowPart=wall.dwLowDateTime; value.HighPart=wall.dwHighDateTime;
            value.QuadPart=static_cast<ULONGLONG>(static_cast<int64_t>(value.QuadPart)+correction*600000000LL);
            wall={value.LowPart,value.HighPart}; FileTimeToSystemTime(&wall,&local);
        }
    }
    return local;
}
std::wstring CalendarStatusText() {
    std::lock_guard<std::mutex> lock(g_eventsMutex);
    if (!g_providerStatus.empty()) return g_providerStatus;
    if (!g_snapshotUnix) return L"Waiting for calendar data. Configure Google or ICS feeds, then open Google accounts from the three-dot menu.";
    auto now=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (now-g_snapshotUnix > std::max(60,Wh_GetIntSetting(L"max_snapshot_age_seconds")))
        return L"Showing saved events. Calendar data is out of date; choose Refresh events from the three-dot menu.";
    if (g_partialSnapshot) return L"Some calendars or events could not be loaded. Showing available events.";
    return L"";
}

std::vector<CalendarEvent> g_allParsedEvents;


struct CalendarCacheEntry {
    std::wstring cachedContent;
    std::vector<CalendarEvent> cachedEvents;
    FILETIME lastLocalFileWriteTime{};
    ULONGLONG lastSuccessfulFetchTick = 0;
};

std::mutex g_cacheMutex;
std::mutex g_watcherMutex;
std::unordered_map<std::wstring, CalendarCacheEntry> g_calendarCaches;

inline int SafeParseIntW(const std::wstring& s, size_t pos, size_t len) {
    if (pos + len > s.size())
        return 0;
    int val = 0;
    for (size_t i = 0; i < len; ++i) {
        wchar_t c = s[pos + i];
        if (c < L'0' || c > L'9')
            return 0;
        val = val * 10 + (c - L'0');
    }
    return val;
}

inline bool ParseIcsDateTimeW(const std::wstring& val, SYSTEMTIME& st, bool& isUtc) {
    ZeroMemory(&st, sizeof(st));
    isUtc = false;
    bool hasTime = false;
    if (val.length() >= 8) {
        st.wYear = (WORD)SafeParseIntW(val, 0, 4);
        st.wMonth = (WORD)SafeParseIntW(val, 4, 2);
        st.wDay = (WORD)SafeParseIntW(val, 6, 2);
    }
    if (val.length() >= 15 && (val[8] == L'T' || val[8] == L't')) {
        hasTime = true;
        st.wHour = (WORD)SafeParseIntW(val, 9, 2);
        st.wMinute = (WORD)SafeParseIntW(val, 11, 2);
        st.wSecond = (WORD)SafeParseIntW(val, 13, 2);
    }
    if (!val.empty() && (val.back() == L'Z' || val.back() == L'z')) {
        isUtc = true;
    }
    NormalizeSystemTime(st);
    return hasTime;
}

inline bool MatchesExDate(const std::wstring& exDateStr, const SYSTEMTIME& tDate, const std::wstring& targetYmd) {
    if (exDateStr.find(targetYmd) != std::wstring::npos) {
        return true;
    }
    SYSTEMTIME stEx{};
    bool isUtc = false;
    if (ParseIcsDateTimeW(exDateStr, stEx, isUtc)) {
        SYSTEMTIME localEx = isUtc ? ToLocal(stEx, true) : stEx;
        if (localEx.wYear == tDate.wYear && localEx.wMonth == tDate.wMonth && localEx.wDay == tDate.wDay) {
            return true;
        }
    }
    return false;
}

inline int ParseUtcOffsetMinutes(const std::wstring& s) {
    size_t first = s.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return 0;
    size_t last = s.find_last_not_of(L" \t\r\n");
    std::wstring offsetStr = s.substr(first, last - first + 1);

    size_t start = 0;
    int sign = 1;
    if (offsetStr[0] == L'+') {
        sign = 1;
        start = 1;
    } else if (offsetStr[0] == L'-') {
        sign = -1;
        start = 1;
    }
    if (start + 4 <= offsetStr.size()) {
        if (iswdigit(offsetStr[start]) && iswdigit(offsetStr[start + 1]) &&
            iswdigit(offsetStr[start + 2]) && iswdigit(offsetStr[start + 3])) {
            int hours = (offsetStr[start] - L'0') * 10 + (offsetStr[start + 1] - L'0');
            int mins = (offsetStr[start + 2] - L'0') * 10 + (offsetStr[start + 3] - L'0');
            return sign * (hours * 60 + mins);
        }
    }
    return 0;
}

inline std::wstring TrimW(const std::wstring& s) {
    size_t start = 0;
    while (start < s.size() && (s[start] == L' ' || s[start] == L'\t' ||
                                s[start] == L'\r' || s[start] == L'\n')) {
        start++;
    }
    size_t end = s.size();
    while (end > start &&
           (s[end - 1] == L' ' || s[end - 1] == L'\t' ||
            s[end - 1] == L'\r' || s[end - 1] == L'\n')) {
        end--;
    }
    return s.substr(start, end - start);
}

inline bool ParseByDayToken(const std::wstring& rawToken, ByDayItem& item) {
    std::wstring token = TrimW(rawToken);
    if (token.size() < 2)
        return false;
    std::wstring dayCode = token.substr(token.size() - 2);
    int dow = -1;
    if (dayCode == L"SU") dow = 0;
    else if (dayCode == L"MO") dow = 1;
    else if (dayCode == L"TU") dow = 2;
    else if (dayCode == L"WE") dow = 3;
    else if (dayCode == L"TH") dow = 4;
    else if (dayCode == L"FR") dow = 5;
    else if (dayCode == L"SA") dow = 6;
    else return false;

    item.dayOfWeek = dow;
    item.ord = 0;
    if (token.size() > 2) {
        std::wstring prefix = token.substr(0, token.size() - 2);
        try {
            item.ord = std::stoi(prefix);
        } catch (...) {
            item.ord = 0;
        }
    }
    return true;
}

inline RecurrenceRule ParseRRule(const std::wstring& rruleStr) {
    RecurrenceRule rule;
    std::wstringstream ss(rruleStr);
    std::wstring part;
    while (std::getline(ss, part, L';')) {
        size_t eq = part.find(L'=');
        if (eq == std::wstring::npos)
            continue;
        std::wstring key = part.substr(0, eq);
        std::wstring val = part.substr(eq + 1);

        if (key == L"FREQ") {
            if (val == L"DAILY") rule.freq = RecurrenceFreq::Daily;
            else if (val == L"WEEKLY") rule.freq = RecurrenceFreq::Weekly;
            else if (val == L"MONTHLY") rule.freq = RecurrenceFreq::Monthly;
            else if (val == L"YEARLY") rule.freq = RecurrenceFreq::Yearly;
        } else if (key == L"INTERVAL") {
            try { rule.interval = std::stoi(val); } catch (...) { rule.interval = 1; }
            if (rule.interval <= 0) rule.interval = 1;
        } else if (key == L"COUNT") {
            try { rule.count = std::stoi(val); } catch (...) { rule.count = 0; }
        } else if (key == L"UNTIL") {
            bool isUtc = false;
            SYSTEMTIME stUntil{};
            ParseIcsDateTimeW(val, stUntil, isUtc);
            if (stUntil.wYear > 0 && stUntil.wMonth > 0 && stUntil.wDay > 0) {
                rule.untilUtc = stUntil;
                rule.hasUntil = true;
            }
        } else if (key == L"BYDAY") {
            std::wstringstream dayss(val);
            std::wstring dayToken;
            while (std::getline(dayss, dayToken, L',')) {
                ByDayItem item;
                if (ParseByDayToken(dayToken, item)) {
                    rule.byDays.push_back(item);
                    rule.byDayMask |= (1 << item.dayOfWeek);
                }
            }
        } else if (key == L"BYMONTHDAY") {
            std::wstringstream mds(val);
            std::wstring mdToken;
            while (std::getline(mds, mdToken, L',')) {
                std::wstring token = TrimW(mdToken);
                try {
                    int dayVal = std::stoi(token);
                    if (dayVal >= -31 && dayVal <= 31 && dayVal != 0) {
                        rule.byMonthDays.push_back(dayVal);
                    }
                } catch (...) {
                }
            }
        } else if (key == L"WKST") {
            std::wstring w = TrimW(val);
            if (w == L"SU") rule.wkst = 0;
            else if (w == L"MO") rule.wkst = 1;
            else if (w == L"TU") rule.wkst = 2;
            else if (w == L"WE") rule.wkst = 3;
            else if (w == L"TH") rule.wkst = 4;
            else if (w == L"FR") rule.wkst = 5;
            else if (w == L"SA") rule.wkst = 6;
        }
    }
    return rule;
}

inline bool IsEventCompleted(const CalendarEvent& ev, const SYSTEMTIME& nowLocal) {
    if (ev.hasEnd && !ev.isAllDay) {
        FILETIME ftEnd{}, ftNow{};
        if (SystemTimeToFileTime(&ev.endLocal, &ftEnd) && SystemTimeToFileTime(&nowLocal, &ftNow)) {
            ULARGE_INTEGER uEnd, uNow;
            uEnd.LowPart = ftEnd.dwLowDateTime; uEnd.HighPart = ftEnd.dwHighDateTime;
            uNow.LowPart = ftNow.dwLowDateTime; uNow.HighPart = ftNow.dwHighDateTime;
            return uNow.QuadPart >= uEnd.QuadPart;
        }
        return false;
    } else {
        if (ev.isAllDay && ev.hasEnd) {
            if (CompareDateOnly(ev.startLocal, ev.endLocal) < 0) {
                return CompareDateOnly(nowLocal, ev.endLocal) >= 0;
            } else {
                return CompareDateOnly(nowLocal, ev.endLocal) > 0;
            }
        }
        return CompareDateOnly(nowLocal, ev.startLocal) > 0;
    }
}

inline bool GetHideCompletedEventsSetting() {
    return Wh_GetIntSetting(L"hideCompletedEvents") != 0;
}

inline bool EventOccursOnDate(const CalendarEvent& ev, const SYSTEMTIME& tDate) {
    if (ev.isAllDay && ev.hasEnd) {
        FILETIME ftStart{}, ftEnd{}, ftTarget{};
        SYSTEMTIME sOnly = ev.startLocal;
        sOnly.wHour = sOnly.wMinute = sOnly.wSecond = sOnly.wMilliseconds = 0;
        SYSTEMTIME eOnly = ev.endLocal;
        eOnly.wHour = eOnly.wMinute = eOnly.wSecond = eOnly.wMilliseconds = 0;
        SYSTEMTIME tOnly = tDate;
        tOnly.wHour = tOnly.wMinute = tOnly.wSecond = tOnly.wMilliseconds = 0;
        SystemTimeToFileTime(&sOnly, &ftStart);
        SystemTimeToFileTime(&eOnly, &ftEnd);
        SystemTimeToFileTime(&tOnly, &ftTarget);
        ULARGE_INTEGER uStart{}, uEnd{}, uTarget{};
        uStart.LowPart = ftStart.dwLowDateTime;
        uStart.HighPart = ftStart.dwHighDateTime;
        uEnd.LowPart = ftEnd.dwLowDateTime;
        uEnd.HighPart = ftEnd.dwHighDateTime;
        uTarget.LowPart = ftTarget.dwLowDateTime;
        uTarget.HighPart = ftTarget.dwHighDateTime;

        if (uTarget.QuadPart >= uStart.QuadPart &&
            uTarget.QuadPart < uEnd.QuadPart) {
            return true;
        } else if (uStart.QuadPart == uEnd.QuadPart &&
                   uTarget.QuadPart == uStart.QuadPart) {
            return true;
        }
    } else {
        if (ev.startLocal.wYear == tDate.wYear &&
            ev.startLocal.wMonth == tDate.wMonth &&
            ev.startLocal.wDay == tDate.wDay) {
            return true;
        } else if (ev.hasEnd) {
            if (CompareDateOnly(ev.startLocal, tDate) < 0 &&
                CompareDateOnly(tDate, ev.endLocal) <= 0) {
                if (CompareDateOnly(tDate, ev.endLocal) < 0 ||
                    (ev.endLocal.wHour > 0 || ev.endLocal.wMinute > 0 ||
                     ev.endLocal.wSecond > 0)) {
                    return true;
                }
            }
        }
    }
    return false;
}

inline bool MatchesByDayItem(const ByDayItem& item, const SYSTEMTIME& candDate) {
    if (candDate.wDayOfWeek != item.dayOfWeek) {
        return false;
    }
    if (item.ord == 0) {
        return true;
    }
    if (item.ord > 0) {
        int nth = (candDate.wDay - 1) / 7 + 1;
        return nth == item.ord;
    } else {
        int daysInMonth = GetDaysInMonth(candDate.wYear, candDate.wMonth);
        int daysFromEnd = daysInMonth - candDate.wDay;
        int negNth = -(daysFromEnd / 7 + 1);
        return negNth == item.ord;
    }
}

inline bool MatchesByMonthDay(int d, const SYSTEMTIME& candDate) {
    if (d > 0) {
        return candDate.wDay == d;
    } else if (d < 0) {
        int daysInMonth = GetDaysInMonth(candDate.wYear, candDate.wMonth);
        return candDate.wDay == (daysInMonth + 1 + d);
    }
    return false;
}

inline bool RecurrenceMatchesDate(const CalendarEvent& ev, const SYSTEMTIME& candDate) {
    if (CompareDateOnly(candDate, ev.startLocal) < 0) {
        return false;
    }

    if (ev.rrule.hasUntil) {
        SYSTEMTIME untilLocal = TzToSystemLocal(ev.rrule.untilUtc, 0);
        if (CompareDateOnly(candDate, untilLocal) > 0) {
            return false;
        }
    }

    int interval = (ev.rrule.interval > 0) ? ev.rrule.interval : 1;

    if (ev.rrule.freq == RecurrenceFreq::Daily) {
        if (ev.rrule.byDayMask != 0) {
            if ((ev.rrule.byDayMask & (1 << candDate.wDayOfWeek)) == 0) {
                return false;
            }
        }
        int days = DaysBetween(ev.startLocal, candDate);
        if (days < 0 || (days % interval != 0)) {
            return false;
        }
        int occIndex = days / interval;
        if (ev.rrule.count > 0 && occIndex >= ev.rrule.count) {
            return false;
        }
        return true;
    } else if (ev.rrule.freq == RecurrenceFreq::Weekly) {
        uint8_t mask = ev.rrule.byDayMask;
        if (mask == 0) {
            mask = (1 << ev.startLocal.wDayOfWeek);
        }
        if ((mask & (1 << candDate.wDayOfWeek)) == 0) {
            return false;
        }

        int wkst = (ev.rrule.wkst >= 0 && ev.rrule.wkst <= 6) ? ev.rrule.wkst : 1;
        int startDayInWeek = (ev.startLocal.wDayOfWeek - wkst + 7) % 7;
        int candDayInWeek = (candDate.wDayOfWeek - wkst + 7) % 7;
        int64_t startWeekStartDays = ToFileTimeDays(ev.startLocal) - startDayInWeek;
        int64_t candWeekStartDays = ToFileTimeDays(candDate) - candDayInWeek;
        int64_t diffDays = candWeekStartDays - startWeekStartDays;
        if (diffDays < 0) {
            return false;
        }
        int weekDiff = static_cast<int>(diffDays / 7);
        if (weekDiff % interval != 0) {
            return false;
        }

        if (ev.rrule.count > 0) {
            int countSoFar = 0;
            for (int w = 0; w <= weekDiff; w += interval) {
                for (int i = 0; i < 7; ++i) {
                    int d = (wkst + i) % 7;
                    if ((mask & (1 << d)) != 0) {
                        if (w == 0 && ((d - wkst + 7) % 7) < startDayInWeek)
                            continue;
                        if (w == weekDiff && ((d - wkst + 7) % 7) > candDayInWeek)
                            break;
                        countSoFar++;
                    }
                }
            }
            if (countSoFar > ev.rrule.count) {
                return false;
            }
        }
        return true;
    } else if (ev.rrule.freq == RecurrenceFreq::Monthly) {
        int monthDiff = (candDate.wYear - ev.startLocal.wYear) * 12 +
                        (candDate.wMonth - ev.startLocal.wMonth);
        if (monthDiff < 0 || (monthDiff % interval != 0)) {
            return false;
        }

        bool dayMatches = false;
        if (!ev.rrule.byDays.empty()) {
            for (const auto& item : ev.rrule.byDays) {
                if (MatchesByDayItem(item, candDate)) {
                    dayMatches = true;
                    break;
                }
            }
        } else if (!ev.rrule.byMonthDays.empty()) {
            for (int d : ev.rrule.byMonthDays) {
                if (MatchesByMonthDay(d, candDate)) {
                    dayMatches = true;
                    break;
                }
            }
        } else {
            if (candDate.wDay == ev.startLocal.wDay) {
                dayMatches = true;
            }
        }

        if (!dayMatches) {
            return false;
        }

        if (ev.rrule.count > 0) {
            int occIndex = monthDiff / interval;
            if (occIndex >= ev.rrule.count) {
                return false;
            }
        }
        return true;
    } else if (ev.rrule.freq == RecurrenceFreq::Yearly) {
        int yearDiff = candDate.wYear - ev.startLocal.wYear;
        if (yearDiff < 0 || (yearDiff % interval != 0)) {
            return false;
        }
        if (candDate.wMonth != ev.startLocal.wMonth) {
            return false;
        }

        bool dayMatches = false;
        if (!ev.rrule.byDays.empty()) {
            for (const auto& item : ev.rrule.byDays) {
                if (MatchesByDayItem(item, candDate)) {
                    dayMatches = true;
                    break;
                }
            }
        } else if (!ev.rrule.byMonthDays.empty()) {
            for (int d : ev.rrule.byMonthDays) {
                if (MatchesByMonthDay(d, candDate)) {
                    dayMatches = true;
                    break;
                }
            }
        } else {
            if (candDate.wDay == ev.startLocal.wDay) {
                dayMatches = true;
            }
        }

        if (!dayMatches) {
            return false;
        }

        if (ev.rrule.count > 0) {
            int occIndex = yearDiff / interval;
            if (occIndex >= ev.rrule.count) {
                return false;
            }
        }
        return true;
    }

    return false;
}

std::vector<CalendarEvent> FilterEventsForDate(
    std::vector<CalendarEvent> const& allEvents,
    SYSTEMTIME const& targetDate) {
    bool hideCompleted = GetHideCompletedEventsSetting();
    SYSTEMTIME nowLocal{};
    if (hideCompleted) {
        GetLocalTime(&nowLocal);
    }

    SYSTEMTIME tDate = targetDate;
    NormalizeSystemTime(tDate);
    std::wstring targetYmd = FormatDateYmd(tDate);

    std::unordered_set<std::wstring> cancelledMasterUids;
    // Map/set of overridden occurrence start dates: key = uid + L"#" + YYYYMMDD
    std::unordered_set<std::wstring> overriddenOccurrences;

    // Pass 1: find cancellations and overridden instances
    for (const auto& ev : allEvents) {
        if (ev.uid.empty())
            continue;

        if (ev.status == L"CANCELLED") {
            if (ev.recurrenceId.empty()) {
                cancelledMasterUids.insert(ev.uid);
            } else {
                SYSTEMTIME stRec{};
                bool isUtc = false;
                if (ParseIcsDateTimeW(ev.recurrenceId, stRec, isUtc)) {
                    SYSTEMTIME localRec = isUtc ? ToLocal(stRec, true) : stRec;
                    overriddenOccurrences.insert(ev.uid + L"#" +
                                                 FormatDateYmd(localRec));
                }
                if (ev.recurrenceId.size() >= 8) {
                    overriddenOccurrences.insert(
                        ev.uid + L"#" + ev.recurrenceId.substr(0, 8));
                }
            }
        } else {
            if (!ev.recurrenceId.empty()) {
                SYSTEMTIME stRec{};
                bool isUtc = false;
                if (ParseIcsDateTimeW(ev.recurrenceId, stRec, isUtc)) {
                    SYSTEMTIME localRec = isUtc ? ToLocal(stRec, true) : stRec;
                    overriddenOccurrences.insert(ev.uid + L"#" +
                                                 FormatDateYmd(localRec));
                }
                if (ev.recurrenceId.size() >= 8) {
                    overriddenOccurrences.insert(
                        ev.uid + L"#" + ev.recurrenceId.substr(0, 8));
                }
            }
        }
    }

    // Pass 2: filter single events and expand recurrences
    std::vector<CalendarEvent> result;

    for (const auto& ev : allEvents) {
        if (!ev.uid.empty() && cancelledMasterUids.count(ev.uid)) {
            continue;
        }
        if (ev.status == L"CANCELLED") {
            continue;
        }

        if (!ev.hasRRule) {
            if (EventOccursOnDate(ev, tDate)) {
                if (hideCompleted && IsEventCompleted(ev, nowLocal)) {
                    continue;
                }
                result.push_back(ev);
            }
            continue;
        }

        // Recurring master event:
        // Duration of an occurrence in days:
        int spanDays = 0;
        if (ev.hasEnd) {
            spanDays = DaysBetween(ev.startLocal, ev.endLocal);
            if (spanDays < 0) spanDays = 0;
            if (spanDays > 366) spanDays = 366;
        }

        for (int offset = 0; offset <= spanDays; ++offset) {
            SYSTEMTIME candDate = ShiftLocalDate(tDate, -offset);

            if (CompareDateOnly(candDate, ev.startLocal) < 0) {
                break; // Earlier offsets will also be before startLocal
            }

            std::wstring candYmd = FormatDateYmd(candDate);
            if (!ev.uid.empty() &&
                overriddenOccurrences.count(ev.uid + L"#" + candYmd)) {
                continue;
            }

            bool isExcluded = false;
            for (const auto& ex : ev.exDates) {
                if (MatchesExDate(ex, candDate, candYmd)) {
                    isExcluded = true;
                    break;
                }
            }
            if (isExcluded) {
                continue;
            }

            if (RecurrenceMatchesDate(ev, candDate)) {
                int days = DaysBetween(ev.startLocal, candDate);
                CalendarEvent occ = ev;
                occ.startLocal = ShiftLocalDate(ev.startLocal, days);
                if (ev.hasEnd) {
                    occ.endLocal = ShiftLocalDate(ev.endLocal, days);
                }
                if (EventOccursOnDate(occ, tDate)) {
                    if (hideCompleted && IsEventCompleted(occ, nowLocal)) {
                        continue;
                    }
                    FILETIME ft{};
                    SystemTimeToFileTime(&occ.startLocal, &ft);
                    occ.sortKey =
                        ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
                    result.push_back(occ);
                }
            }
        }
    }

    std::sort(result.begin(), result.end(),
              [](const CalendarEvent& a, const CalendarEvent& b) {
                  if (a.isAllDay != b.isAllDay) {
                      return a.isAllDay > b.isAllDay;
                  }
                  return a.sortKey < b.sortKey;
              });
    return result;
}

void OnCalendarOpened();
void RegisterCoreWindowEvents();
void TriggerBackgroundFetch(bool force = false);
bool StartWorkerThread();
void StopWorkerThread();

std::vector<std::wstring> GetIcsPathsSetting() {
    std::vector<std::wstring> paths;
    for (int i = 0;; i++) {
        PCWSTR val = Wh_GetStringSetting(L"icsPath[%d]", i);
        bool hasVal = (val && *val);
        if (hasVal) {
            std::wstring clean = val;
            while (!clean.empty() && (clean.front() == L' ' || clean.front() == L'\t' || clean.front() == L'"')) {
                clean.erase(clean.begin());
            }
            while (!clean.empty() && (clean.back() == L' ' || clean.back() == L'\t' || clean.back() == L'"')) {
                clean.pop_back();
            }
            if (!clean.empty()) {
                paths.push_back(clean);
            }
        }
        if (val) {
            Wh_FreeStringSetting(val);
        }
        if (!hasVal) {
            break;
        }
    }
    if (paths.empty()) {
        PCWSTR val = Wh_GetStringSetting(L"icsPath");
        if (val && *val) {
            std::wstring clean = val;
            while (!clean.empty() && (clean.front() == L' ' || clean.front() == L'\t' || clean.front() == L'"')) {
                clean.erase(clean.begin());
            }
            while (!clean.empty() && (clean.back() == L' ' || clean.back() == L'\t' || clean.back() == L'"')) {
                clean.pop_back();
            }
            if (!clean.empty()) {
                paths.push_back(clean);
            }
        }
        if (val) {
            Wh_FreeStringSetting(val);
        }
    }
    return paths;
}

std::wstring GetIcsPathSetting() {
    auto paths = GetIcsPathsSetting();
    return paths.empty() ? L"" : paths.front();
}

bool IsCalendarAboveAgenda() {
    PCWSTR pos = Wh_GetStringSetting(L"calendarPosition");
    bool isAbove = true;
    if (pos) {
        if (wcscmp(pos, L"below") == 0) {
            isAbove = false;
        }
        Wh_FreeStringSetting(pos);
    }
    return isAbove;
}

bool IsInlineCalendarMode() {
    PCWSTR mode = Wh_GetStringSetting(L"groupingMode");
    bool isInline = true;
    if (mode) {
        if (wcscmp(mode, L"popup") == 0) {
            isInline = false;
        }
        Wh_FreeStringSetting(mode);
    }
    return isInline;
}

int GetMaxHeightCollapsedSetting() {
    int val = Wh_GetIntSetting(L"maxHeightCollapsed");
    return (val < 0) ? 0 : val;
}

int GetMaxHeightExpandedSetting() {
    int val = Wh_GetIntSetting(L"maxHeightExpanded");
    return (val < 0) ? 0 : val;
}

int GetTimeColumnWidthSetting() {
    return Wh_GetIntSetting(L"timeColumnWidth");
}

bool ShouldHideFocusSession() {
    return Wh_GetIntSetting(L"hideFocusSession") != 0;
}

int GetMinFetchIntervalSetting() {
    int val = Wh_GetIntSetting(L"minFetchInterval");
    return (val < 0) ? 0 : val;
}

bool GetKeyboardShortcutsSetting() {
    return Wh_GetIntSetting(L"keyboardShortcuts") != 0;
}


std::wstring DecodeIcsBytes(const char* data, size_t size) {
    if (!data || size == 0)
        return {};

    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data);

    // Check UTF-16 LE BOM: FF FE
    if (size >= 2 && bytes[0] == 0xFF && bytes[1] == 0xFE) {
        size_t wcharCount = (size - 2) / sizeof(wchar_t);
        return std::wstring(reinterpret_cast<const wchar_t*>(data + 2),
                            wcharCount);
    }

    // Check UTF-16 BE BOM: FE FF
    if (size >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) {
        size_t wcharCount = (size - 2) / sizeof(wchar_t);
        std::wstring result(wcharCount, L'\0');
        for (size_t i = 0; i < wcharCount; ++i) {
            unsigned char b1 = bytes[2 + i * 2];
            unsigned char b2 = bytes[2 + i * 2 + 1];
            result[i] = static_cast<wchar_t>((b1 << 8) | b2);
        }
        return result;
    }

    // Check UTF-8 BOM: EF BB BF
    size_t offset = 0;
    if (size >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        offset = 3;
    }

    const char* utf8Data = data + offset;
    size_t utf8Size = size - offset;

    // Try decoding as UTF-8
    int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8Data,
                                   static_cast<int>(utf8Size), nullptr, 0);
    if (wlen > 0) {
        std::wstring result(wlen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Data, static_cast<int>(utf8Size),
                            &result[0], wlen);
        return result;
    }

    // Fallback: CP_ACP (ANSI)
    wlen = MultiByteToWideChar(CP_ACP, 0, utf8Data,
                               static_cast<int>(utf8Size), nullptr, 0);
    if (wlen > 0) {
        std::wstring result(wlen, L'\0');
        MultiByteToWideChar(CP_ACP, 0, utf8Data, static_cast<int>(utf8Size),
                            &result[0], wlen);
        return result;
    }

    return {};
}

std::wstring UnescapeIcsText(const std::wstring& str) {
    std::wstring result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == L'\\' && i + 1 < str.size()) {
            wchar_t next = str[i + 1];
            if (next == L'n' || next == L'N') {
                result.push_back(L'\n');
                ++i;
            } else if (next == L',' || next == L';' || next == L'\\') {
                result.push_back(next);
                ++i;
            } else {
                result.push_back(str[i]);
            }
        } else {
            result.push_back(str[i]);
        }
    }
    return result;
}

std::wstring FindUrlInText(const std::wstring& text) {
    if (text.empty()) return L"";

    const std::wstring schemes[] = { L"https://", L"http://" };
    size_t bestPos = std::wstring::npos;
    for (const auto& scheme : schemes) {
        for (size_t i = 0; i + scheme.size() <= text.size(); ++i) {
            bool match = true;
            for (size_t j = 0; j < scheme.size(); ++j) {
                if (towlower(text[i + j]) != scheme[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                if (bestPos == std::wstring::npos || i < bestPos) {
                    bestPos = i;
                }
                break;
            }
        }
    }

    if (bestPos == std::wstring::npos) return L"";

    size_t endPos = bestPos;
    while (endPos < text.size()) {
        wchar_t c = text[endPos];
        if (iswspace(c) || c == L'"' || c == L'\'' || c == L'<' || c == L'>' ||
            c == L'(' || c == L')' || c == L'[' || c == L']' || c == L'{' || c == L'}') {
            break;
        }
        ++endPos;
    }

    while (endPos > bestPos) {
        wchar_t c = text[endPos - 1];
        if (c == L'.' || c == L',' || c == L';' || c == L':' || c == L'?' || c == L'!') {
            --endPos;
        } else {
            break;
        }
    }

    return text.substr(bestPos, endPos - bestPos);
}

std::vector<CalendarEvent> ParseIcs(const std::wstring& icsContent) {
    std::wstring unfolded;
    unfolded.reserve(icsContent.size());
    for (size_t i = 0; i < icsContent.size(); ++i) {
        if ((icsContent[i] == L'\r' && i + 1 < icsContent.size() &&
             icsContent[i + 1] == L'\n') ||
            icsContent[i] == L'\n') {
            size_t nextPos = (icsContent[i] == L'\r') ? i + 2 : i + 1;
            if (nextPos < icsContent.size() &&
                (icsContent[nextPos] == L' ' || icsContent[nextPos] == L'\t')) {
                i = nextPos;
                continue;
            }
        }
        unfolded.push_back(icsContent[i]);
    }

    std::unordered_map<std::wstring, int> tzOffsets;
    tzOffsets[L"UTC"] = 0;
    tzOffsets[L"GMT"] = 0;
    tzOffsets[L"Z"] = 0;

    std::wstring currentTzid;
    bool inVTimezone = false;

    std::vector<CalendarEvent> events;
    std::wistringstream stream(unfolded);
    std::wstring line;
    bool inEvent = false;
    CalendarEvent currentEvent;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == L'\r')
            line.pop_back();
        if (line.empty())
            continue;

        if (line == L"BEGIN:VTIMEZONE") {
            inVTimezone = true;
            currentTzid.clear();
            continue;
        }
        if (line == L"END:VTIMEZONE") {
            inVTimezone = false;
            currentTzid.clear();
            continue;
        }

        if (inVTimezone) {
            size_t colon = line.find(L':');
            if (colon != std::wstring::npos) {
                std::wstring keyPart = line.substr(0, colon);
                std::wstring valPart = line.substr(colon + 1);
                size_t semi = keyPart.find(L';');
                std::wstring key =
                    (semi != std::wstring::npos) ? keyPart.substr(0, semi) : keyPart;

                if (key == L"TZID") {
                    currentTzid = valPart;
                    if (currentTzid.size() >= 2 && currentTzid.front() == L'"' &&
                        currentTzid.back() == L'"') {
                        currentTzid =
                            currentTzid.substr(1, currentTzid.size() - 2);
                    }
                } else if (key == L"TZOFFSETTO" && !currentTzid.empty()) {
                    tzOffsets[currentTzid] = ParseUtcOffsetMinutes(valPart);
                }
            }
            continue;
        }

        if (line == L"BEGIN:VEVENT") {
            inEvent = true;
            currentEvent = CalendarEvent{};
            continue;
        }
        if (line == L"END:VEVENT") {
            if (inEvent) {
                if (currentEvent.url.empty()) {
                    currentEvent.url = FindUrlInText(currentEvent.location);
                    if (currentEvent.url.empty()) {
                        currentEvent.url = FindUrlInText(currentEvent.notes);
                    }
                }
                events.push_back(currentEvent);
                inEvent = false;
            }
            continue;
        }

        if (!inEvent)
            continue;

        size_t colon = line.find(L':');
        if (colon == std::wstring::npos)
            continue;

        std::wstring keyPart = line.substr(0, colon);
        std::wstring valPart = line.substr(colon + 1);

        size_t semi = keyPart.find(L';');
        std::wstring key =
            (semi != std::wstring::npos) ? keyPart.substr(0, semi) : keyPart;

        if (key == L"UID") {
            currentEvent.uid = valPart;
        } else if (key == L"STATUS") {
            currentEvent.status = valPart;
            std::transform(currentEvent.status.begin(),
                           currentEvent.status.end(),
                           currentEvent.status.begin(), ::towupper);
        } else if (key == L"RECURRENCE-ID") {
            currentEvent.recurrenceId = valPart;
        } else if (key == L"RRULE") {
            currentEvent.rrule = ParseRRule(valPart);
            currentEvent.hasRRule =
                (currentEvent.rrule.freq != RecurrenceFreq::None);
        } else if (key == L"EXDATE") {
            std::wstringstream exss(valPart);
            std::wstring exToken;
            while (std::getline(exss, exToken, L',')) {
                if (!exToken.empty()) {
                    currentEvent.exDates.push_back(exToken);
                }
            }
        } else if (key == L"SUMMARY") {
            currentEvent.name = UnescapeIcsText(valPart);
        } else if (key == L"LOCATION") {
            currentEvent.location = UnescapeIcsText(valPart);
        } else if (key == L"DESCRIPTION") {
            currentEvent.notes = UnescapeIcsText(valPart);
            for (auto& ch : currentEvent.notes) {
                if (ch == L'\r' || ch == L'\n')
                    ch = L' ';
            }
        } else if (key == L"URL") {
            std::wstring u = TrimW(valPart);
            if (_wcsnicmp(u.c_str(), L"http://", 7) == 0 ||
                _wcsnicmp(u.c_str(), L"https://", 8) == 0) {
                currentEvent.url = u;
            }
        } else if (key == L"X-MICROSOFT-CDO-ALLDAYEVENT" ||
                   key == L"X-MICROSOFT-MSNCALENDAR-ALL-DAY-EVENT") {
            if (_wcsicmp(valPart.c_str(), L"TRUE") == 0 || valPart == L"1") {
                currentEvent.isAllDay = true;
            }
        } else if (key == L"DTSTART") {
            bool isValueDate =
                (keyPart.find(L"VALUE=DATE") != std::wstring::npos);
            std::wstring tzid;
            size_t tzidPos = keyPart.find(L"TZID=");
            if (tzidPos != std::wstring::npos) {
                tzid = keyPart.substr(tzidPos + 5);
                size_t semi2 = tzid.find(L';');
                if (semi2 != std::wstring::npos)
                    tzid = tzid.substr(0, semi2);
                if (tzid.size() >= 2 && tzid.front() == L'"' &&
                    tzid.back() == L'"')
                    tzid = tzid.substr(1, tzid.size() - 2);
            }

            SYSTEMTIME stUtc{};
            bool isUtc = false;
            bool hasTime = ParseIcsDateTimeW(valPart, stUtc, isUtc);
            if (!hasTime || isValueDate) {
                currentEvent.isAllDay = true;
                currentEvent.startLocal = stUtc;
            } else if (isUtc) {
                currentEvent.startLocal = TzToSystemLocal(stUtc, 0);
            } else if (!tzid.empty()) {
                auto it = tzOffsets.find(tzid);
                if (it != tzOffsets.end()) {
                    currentEvent.startLocal =
                        TzToSystemLocal(stUtc, it->second);
                } else {
                    currentEvent.startLocal = stUtc;
                }
            } else {
                currentEvent.startLocal = stUtc;
            }

            FILETIME ft{};
            SystemTimeToFileTime(&currentEvent.startLocal, &ft);
            currentEvent.sortKey =
                ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        } else if (key == L"DTEND") {
            bool isValueDate =
                (keyPart.find(L"VALUE=DATE") != std::wstring::npos);
            std::wstring tzid;
            size_t tzidPos = keyPart.find(L"TZID=");
            if (tzidPos != std::wstring::npos) {
                tzid = keyPart.substr(tzidPos + 5);
                size_t semi2 = tzid.find(L';');
                if (semi2 != std::wstring::npos)
                    tzid = tzid.substr(0, semi2);
                if (tzid.size() >= 2 && tzid.front() == L'"' &&
                    tzid.back() == L'"')
                    tzid = tzid.substr(1, tzid.size() - 2);
            }

            SYSTEMTIME stUtc{};
            bool isUtc = false;
            bool hasTime = ParseIcsDateTimeW(valPart, stUtc, isUtc);
            if (!hasTime || isValueDate) {
                currentEvent.isAllDay = true;
                currentEvent.endLocal = stUtc;
            } else if (isUtc) {
                currentEvent.endLocal = TzToSystemLocal(stUtc, 0);
            } else if (!tzid.empty()) {
                auto it = tzOffsets.find(tzid);
                if (it != tzOffsets.end()) {
                    currentEvent.endLocal =
                        TzToSystemLocal(stUtc, it->second);
                } else {
                    currentEvent.endLocal = stUtc;
                }
            } else {
                currentEvent.endLocal = stUtc;
            }
            currentEvent.hasEnd = true;
        }
    }

    return events;
}

std::wstring FetchIcsContent(std::wstring const& pathOrUrl,
                             bool force,
                             bool& outFromCache) {
    outFromCache = false;
    bool isRemote = (_wcsnicmp(pathOrUrl.c_str(), L"http://", 7) == 0 ||
                     _wcsnicmp(pathOrUrl.c_str(), L"https://", 8) == 0);

    int minFetchInterval = GetMinFetchIntervalSetting();
    ULONGLONG ttlMs = static_cast<ULONGLONG>(minFetchInterval) * 60ULL * 1000ULL;
    ULONGLONG now = GetTickCount64();

    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_calendarCaches.find(pathOrUrl);
        if (it != g_calendarCaches.end()) {
            auto& entry = it->second;
            if (!force && minFetchInterval > 0 && !entry.cachedContent.empty() &&
                (now - entry.lastSuccessfulFetchTick < ttlMs)) {
                if (isRemote) {
                    Wh_Log(L"Using cached remote ICS content for %s (TTL remaining)", pathOrUrl.c_str());
                    outFromCache = true;
                    return entry.cachedContent;
                } else {
                    std::wstring localPath = pathOrUrl;
                    if (_wcsnicmp(localPath.c_str(), L"file:///", 8) == 0)
                        localPath = localPath.substr(8);
                    else if (_wcsnicmp(localPath.c_str(), L"file://", 7) == 0)
                        localPath = localPath.substr(7);
                    for (auto& ch : localPath) {
                        if (ch == L'/')
                            ch = L'\\';
                    }

                    WIN32_FILE_ATTRIBUTE_DATA attr{};
                    if (GetFileAttributesExW(localPath.c_str(),
                                             GetFileExInfoStandard, &attr)) {
                        if (attr.ftLastWriteTime.dwLowDateTime ==
                                entry.lastLocalFileWriteTime.dwLowDateTime &&
                            attr.ftLastWriteTime.dwHighDateTime ==
                                entry.lastLocalFileWriteTime.dwHighDateTime) {
                            Wh_Log(
                                L"Using cached local ICS content for %s (TTL remaining & "
                                L"file write time unchanged)", pathOrUrl.c_str());
                            outFromCache = true;
                            return entry.cachedContent;
                        }
                    }
                }
            }
        }
    }

    std::wstring decodedContent;
    FILETIME newWriteTime{};

    if (isRemote) {
        Wh_Log(L"Fetching remote URL: %s", pathOrUrl.c_str());
        const WH_URL_CONTENT* content =
            Wh_GetUrlContent(pathOrUrl.c_str(), nullptr);
        if (content && content->statusCode == 200 && content->data) {
            decodedContent = DecodeIcsBytes(
                reinterpret_cast<const char*>(content->data), content->length);
            Wh_FreeUrlContent(content);
            Wh_Log(L"Successfully fetched %zu characters from remote URL",
                   decodedContent.size());
        } else {
            if (content) {
                Wh_Log(L"Wh_GetUrlContent returned HTTP status %d",
                       content->statusCode);
                Wh_FreeUrlContent(content);
            } else {
                Wh_Log(L"Wh_GetUrlContent returned null");
            }
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            auto it = g_calendarCaches.find(pathOrUrl);
            if (it != g_calendarCaches.end() && !it->second.cachedContent.empty()) {
                Wh_Log(L"Fetch failed; using cached content for %s as fallback", pathOrUrl.c_str());
                outFromCache = true;
                return it->second.cachedContent;
            }
            return {};
        }
    } else {
        std::wstring localPath = pathOrUrl;
        if (_wcsnicmp(localPath.c_str(), L"file:///", 8) == 0)
            localPath = localPath.substr(8);
        else if (_wcsnicmp(localPath.c_str(), L"file://", 7) == 0)
            localPath = localPath.substr(7);

        for (auto& ch : localPath) {
            if (ch == L'/')
                ch = L'\\';
        }

        Wh_Log(L"Reading local file: %s", localPath.c_str());

        WIN32_FILE_ATTRIBUTE_DATA attr{};
        if (GetFileAttributesExW(localPath.c_str(), GetFileExInfoStandard,
                                 &attr)) {
            newWriteTime = attr.ftLastWriteTime;
        }

        HANDLE hFile = CreateFileW(
            localPath.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            Wh_Log(L"CreateFileW failed (%u) for local path: %s",
                   GetLastError(), localPath.c_str());
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            auto it = g_calendarCaches.find(pathOrUrl);
            if (it != g_calendarCaches.end() && !it->second.cachedContent.empty()) {
                Wh_Log(L"Read failed; using cached content for %s as fallback", pathOrUrl.c_str());
                outFromCache = true;
                return it->second.cachedContent;
            }
            return {};
        }

        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart <= 0 ||
            fileSize.QuadPart > 50 * 1024 * 1024) {
            CloseHandle(hFile);
            Wh_Log(L"Local file is empty, invalid, or exceeds 50MB: %s",
                   localPath.c_str());
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            auto it = g_calendarCaches.find(pathOrUrl);
            if (it != g_calendarCaches.end() && !it->second.cachedContent.empty()) {
                outFromCache = true;
                return it->second.cachedContent;
            }
            return {};
        }

        std::vector<char> buf(static_cast<size_t>(fileSize.QuadPart));
        DWORD bytesRead = 0;
        if (!ReadFile(hFile, buf.data(), static_cast<DWORD>(fileSize.QuadPart),
                      &bytesRead, nullptr)) {
            CloseHandle(hFile);
            Wh_Log(L"ReadFile failed (%u) for local path: %s", GetLastError(),
                   localPath.c_str());
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            auto it = g_calendarCaches.find(pathOrUrl);
            if (it != g_calendarCaches.end() && !it->second.cachedContent.empty()) {
                outFromCache = true;
                return it->second.cachedContent;
            }
            return {};
        }

        CloseHandle(hFile);
        decodedContent = DecodeIcsBytes(buf.data(), bytesRead);
        Wh_Log(
            L"Successfully read %u bytes, decoded %zu characters from local "
            L"file",
            bytesRead, decodedContent.size());
    }

    if (!decodedContent.empty()) {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto& entry = g_calendarCaches[pathOrUrl];
        entry.cachedContent = decodedContent;
        entry.lastLocalFileWriteTime = newWriteTime;
        entry.lastSuccessfulFetchTick = GetTickCount64();
    }

    return decodedContent;
}

wf::DateTime SystemTimeToWinRtDateTime(SYSTEMTIME const& stLocal) {
    SYSTEMTIME stUtc{};
    TzSpecificLocalTimeToSystemTime(nullptr, &stLocal, &stUtc);
    FILETIME ft{};
    SystemTimeToFileTime(&stUtc, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return wf::DateTime{wf::TimeSpan{static_cast<int64_t>(uli.QuadPart)}};
}

wux::CornerRadius GetStandardCornerRadius() {
    try {
        auto res = wux::Application::Current().Resources();
        if (res.HasKey(winrt::box_value(L"ControlCornerRadius"))) {
            return winrt::unbox_value<wux::CornerRadius>(
                res.Lookup(winrt::box_value(L"ControlCornerRadius")));
        }
    } catch (...) {
    }
    return wux::CornerRadius{4, 4, 4, 4};
}

wuxm::Brush GetCardBackgroundBrush() {
    try {
        auto res = wux::Application::Current().Resources();
        if (res.HasKey(
                winrt::box_value(L"CardBackgroundFillColorDefaultBrush"))) {
            auto brush = res.Lookup(winrt::box_value(
                                        L"CardBackgroundFillColorDefaultBrush"))
                             .try_as<wuxm::Brush>();
            if (brush)
                return brush;
        }
        if (res.HasKey(
                winrt::box_value(L"SystemControlBackgroundBaseLowBrush"))) {
            auto brush = res.Lookup(winrt::box_value(
                                        L"SystemControlBackgroundBaseLowBrush"))
                             .try_as<wuxm::Brush>();
            if (brush)
                return brush;
        }
        if (res.HasKey(winrt::box_value(
                L"SystemControlBackgroundAltMediumLowBrush"))) {
            auto brush =
                res.Lookup(winrt::box_value(
                               L"SystemControlBackgroundAltMediumLowBrush"))
                    .try_as<wuxm::Brush>();
            if (brush)
                return brush;
        }
    } catch (...) {
    }

    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{20, 128, 128, 128});
}

wuxm::Brush GetCardBorderBrush() {
    try {
        auto res = wux::Application::Current().Resources();
        if (res.HasKey(winrt::box_value(L"CardStrokeColorDefaultBrush"))) {
            auto brush =
                res.Lookup(winrt::box_value(L"CardStrokeColorDefaultBrush"))
                    .try_as<wuxm::Brush>();
            if (brush)
                return brush;
        }
        if (res.HasKey(winrt::box_value(
                L"SystemControlBackgroundBaseMediumLowBrush"))) {
            auto brush =
                res.Lookup(winrt::box_value(
                               L"SystemControlBackgroundBaseMediumLowBrush"))
                    .try_as<wuxm::Brush>();
            if (brush)
                return brush;
        }
    } catch (...) {
    }

    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{15, 128, 128, 128});
}


inline std::wstring RemoveYearFromDateString(std::wstring str, int year) {
    std::wstring yearStr = std::to_wstring(year);
    size_t pos = str.find(yearStr);
    if (pos == std::wstring::npos) {
        return str;
    }

    size_t start = pos;
    size_t end = pos + yearStr.length();

    // Check for trailing characters/words associated with year in certain locales
    if (end < str.length() && (str[end] == L'年' || str[end] == L'년')) {
        end++;
    } else if (end + 1 < str.length() && str[end] == L'.' && iswspace(str[end + 1])) {
        end++;
    } else if (end + 2 < str.length() && str.substr(end, 3) == L" г.") {
        end += 3;
    } else if (end + 1 < str.length() && str.substr(end, 2) == L" г") {
        end += 2;
    } else if (end + 2 < str.length() && str.substr(end, 3) == L" m.") {
        end += 3;
    } else if (end + 1 < str.length() && str.substr(end, 2) == L" m") {
        end += 2;
    }

    bool atEnd = true;
    for (size_t i = end; i < str.length(); ++i) {
        if (!iswspace(str[i]) && str[i] != L'.' && str[i] != L',') {
            atEnd = false;
            break;
        }
    }

    if (atEnd) {
        while (start > 0 && (str[start - 1] == L' ' || str[start - 1] == L',' || str[start - 1] == L'.')) {
            start--;
        }
        str.erase(start);
    } else if (start == 0) {
        while (end < str.length() && (iswspace(str[end]) || str[end] == L'.' || str[end] == L'-')) {
            end++;
        }
        str.erase(0, end);
    } else {
        if (start >= 2 && str[start - 2] == L',' && str[start - 1] == L' ') {
            start -= 2;
        } else if (start > 0 && str[start - 1] == L' ') {
            start--;
        }
        while (end < str.length() && (str[end] == L' ' || str[end] == L',')) {
            end++;
        }
        str.erase(start, end - start);
    }

    while (!str.empty() && (str.back() == L' ' || str.back() == L',')) {
        str.pop_back();
    }
    size_t lead = 0;
    while (lead < str.length() && (str[lead] == L' ' || str[lead] == L',')) {
        lead++;
    }
    if (lead > 0) {
        str.erase(0, lead);
    }

    return str;
}

inline std::wstring FormatFilterDate(SYSTEMTIME const& st) {
    SYSTEMTIME validSt = st;
    if (validSt.wYear == 0) {
        GetLocalTime(&validSt);
    }
    wchar_t dateBuf[256]{};
    if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &validSt, nullptr, dateBuf, ARRAYSIZE(dateBuf), nullptr) == 0) {
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &validSt, L"dddd, MMMM d", dateBuf, ARRAYSIZE(dateBuf), nullptr);
    }
    return RemoveYearFromDateString(dateBuf, validSt.wYear);
}

inline std::wstring GetCalendarDatePickerFormat() {
    wchar_t patternBuf[128]{};
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SLONGDATE, patternBuf, ARRAYSIZE(patternBuf)) == 0) {
        return L"{dayofweek.full}, {month.full} {day.integer}";
    }

    std::wstring p = patternBuf;

    size_t yPos = p.find_first_of(L"yY");
    if (yPos != std::wstring::npos) {
        size_t yEnd = p.find_last_of(L"yY") + 1;
        if (yEnd < p.length() && p[yEnd] == L'\'') {
            size_t qClose = p.find(L'\'', yEnd + 1);
            if (qClose != std::wstring::npos) {
                yEnd = qClose + 1;
            }
        }
        if (yEnd < p.length() && (p[yEnd] == L'年' || p[yEnd] == L'년')) {
            yEnd++;
        }

        bool atEnd = true;
        for (size_t i = yEnd; i < p.length(); ++i) {
            if (iswalpha(p[i])) { atEnd = false; break; }
        }

        if (atEnd) {
            size_t yStart = yPos;
            while (yStart > 0 && (p[yStart - 1] == L' ' || p[yStart - 1] == L',' || p[yStart - 1] == L'.' || p[yStart - 1] == L'\'')) {
                yStart--;
            }
            p.erase(yStart);
        } else if (yPos == 0) {
            while (yEnd < p.length() && (p[yEnd] == L' ' || p[yEnd] == L'.' || p[yEnd] == L'-')) {
                yEnd++;
            }
            p.erase(0, yEnd);
        } else {
            p.erase(yPos, yEnd - yPos);
        }
    }

    std::wstring result;
    for (size_t i = 0; i < p.length();) {
        if (p.substr(i, 4) == L"dddd") {
            result += L"{dayofweek.full}";
            i += 4;
        } else if (p.substr(i, 3) == L"ddd") {
            result += L"{dayofweek.abbreviated}";
            i += 3;
        } else if (p.substr(i, 4) == L"MMMM") {
            result += L"{month.full}";
            i += 4;
        } else if (p.substr(i, 3) == L"MMM") {
            result += L"{month.abbreviated}";
            i += 3;
        } else if (p.substr(i, 2) == L"MM") {
            result += L"{month.integer}";
            i += 2;
        } else if (p[i] == L'M') {
            result += L"{month.integer}";
            i += 1;
        } else if (p.substr(i, 2) == L"dd") {
            result += L"{day.integer}";
            i += 2;
        } else if (p[i] == L'd') {
            result += L"{day.integer}";
            i += 1;
        } else if (p[i] == L'\'') {
            i += 1;
        } else {
            result += p[i];
            i += 1;
        }
    }

    while (!result.empty() && (result.back() == L' ' || result.back() == L',')) {
        result.pop_back();
    }
    size_t lead = 0;
    while (lead < result.length() && (result[lead] == L' ' || result[lead] == L',')) {
        lead++;
    }
    if (lead > 0) {
        result.erase(0, lead);
    }

    return result.empty() ? L"{dayofweek.full}, {month.full} {day.integer}" : result;
}

namespace {
    [[clang::no_destroy]] wuxc::Grid m_rootGrid{nullptr};
    [[clang::no_destroy]] wuxc::Grid m_headerGrid{nullptr};
    [[clang::no_destroy]] wuxc::Button m_prevDayButton{nullptr};
    [[clang::no_destroy]] wuxc::Button m_nextDayButton{nullptr};
    [[clang::no_destroy]] wuxi::KeyboardAccelerator m_prevDayAccel{nullptr};
    [[clang::no_destroy]] wuxi::KeyboardAccelerator m_nextDayAccel{nullptr};
    [[clang::no_destroy]] wuxc::Button m_menuButton{nullptr};
    [[clang::no_destroy]] wuxc::Button m_dateButton{nullptr};
    [[clang::no_destroy]] wuxc::TextBlock m_dateButtonText{nullptr};
    [[clang::no_destroy]] wuxc::CalendarView m_calendarView{nullptr};
    [[clang::no_destroy]] wuxc::CalendarDatePicker m_datePicker{nullptr};
    [[clang::no_destroy]] wuxc::TextBlock m_datePickerText{nullptr};
    winrt::event_token m_datePickerLoadedToken{};
    winrt::event_token m_dateChangedToken{};
    [[clang::no_destroy]] wuxc::ScrollViewer m_eventsScrollViewer{nullptr};
    [[clang::no_destroy]] wuxc::ItemsControl m_itemsControl{nullptr};
    SYSTEMTIME m_currentFilterDate{};
    winrt::event_token m_calendarViewSelectionChangedToken{};
    winrt::event_token m_dateBtnClickToken{};
    bool m_isUpdatingCalendarSelection{false};
    winrt::event_token m_prevBtnToken{};
    winrt::event_token m_nextBtnToken{};
    winrt::event_token m_menuBtnToken{};
    [[clang::no_destroy]] wux::FrameworkElement m_focusSessionControl{nullptr};
    int64_t m_focusSessionVisibilityToken{0};
    [[clang::no_destroy]] wuxc::ScrollViewer m_hostScrollViewer{nullptr};
    [[clang::no_destroy]] winrt::Windows::Foundation::IInspectable m_originalCalendarContent{nullptr};
    std::mutex g_pendingActionsMutex;
    [[clang::no_destroy]] std::vector<winrt::Windows::Foundation::IAsyncOperation<bool>> g_pendingDispatcherActions;

    wuxc::ScrollBarVisibility m_originalVerticalScrollBarVisibility{wuxc::ScrollBarVisibility::Auto};
    wuxc::ScrollBarVisibility m_originalHorizontalScrollBarVisibility{wuxc::ScrollBarVisibility::Disabled};
    wux::Visibility m_originalFocusSessionVisibility{wux::Visibility::Visible};
    std::atomic<DWORD> g_ownerThreadId{0};
    std::atomic<HANDLE> g_hWorkerThread{nullptr};
    std::atomic<HANDLE> g_hWorkEvent{nullptr};
    std::atomic<HANDLE> g_hStopEvent{nullptr};
    std::atomic<bool> g_workerForceFetch{false};
}

struct CoreWindowData {
    wuc::CoreWindow coreWindow{nullptr};
    winrt::event_token activatedToken{};
    winrt::event_token visibilityChangedToken{};
    wux::FrameworkElement layoutUpdatedFe{nullptr};
    winrt::event_token layoutUpdatedToken{};
};
thread_local CoreWindowData t_coreWindowData;

void UnregisterFocusSessionControl();
void PopulateItemsControl(std::vector<CalendarEvent> const& events);
void ChangeSelectedDay(int deltaDays);
void OnDatePickerDateChanged(wf::IReference<wf::DateTime> const& newDate);
void ResetDatePickerToToday();
void DispatchUpdateEvents();
void UpdateMaxHeight(int maxHeight);
void ReplaceCalendarContent(wuxc::ScrollViewer const& host);
void ShowCalendarMenu();
void HandleFocusSessionControl(wux::FrameworkElement const& element);
void WalkVisualTree(winrt::Windows::UI::Xaml::DependencyObject const& root);
void RegisterCoreWindowEvents();
void UnregisterCoreWindowEvents();
void RevokeLayoutUpdated();

void RevokeLayoutUpdated() {
    if (t_coreWindowData.layoutUpdatedFe && t_coreWindowData.layoutUpdatedToken.value != 0) {
        try {
            t_coreWindowData.layoutUpdatedFe.LayoutUpdated(t_coreWindowData.layoutUpdatedToken);
        } catch (...) {}
    }
    t_coreWindowData.layoutUpdatedToken = {};
    t_coreWindowData.layoutUpdatedFe = nullptr;
}

void UnregisterFocusSessionControl() {
    if (m_focusSessionControl && m_focusSessionVisibilityToken != 0) {
        try {
            if (auto dispatcher = m_focusSessionControl.Dispatcher()) {
                if (!dispatcher.HasThreadAccess()) {
                    return;
                }
            }
            m_focusSessionControl.UnregisterPropertyChangedCallback(
                wux::UIElement::VisibilityProperty(),
                m_focusSessionVisibilityToken);
        } catch (...) {}
        m_focusSessionVisibilityToken = 0;
    }
}

void RestoreCalendarContent() {
    DWORD owner = g_ownerThreadId.load();
    if (owner != 0 && owner != GetCurrentThreadId()) {
        return;
    }

    RevokeLayoutUpdated();
    UnregisterFocusSessionControl();

    if (m_focusSessionControl) {
        try {
            m_focusSessionControl.Visibility(m_originalFocusSessionVisibility);
            Wh_Log(L"Restored FocusSessionControl visibility to original value");
        } catch (...) {}
        m_focusSessionControl = nullptr;
    }

    try {
        if (m_prevDayButton) {
            if (m_prevBtnToken.value != 0) { m_prevDayButton.Click(m_prevBtnToken); m_prevBtnToken = {}; }
            m_prevDayButton.KeyboardAccelerators().Clear();
        }
    } catch (...) {}
    try {
        if (m_nextDayButton) {
            if (m_nextBtnToken.value != 0) { m_nextDayButton.Click(m_nextBtnToken); m_nextBtnToken = {}; }
            m_nextDayButton.KeyboardAccelerators().Clear();
        }
    } catch (...) {}
    try {
        if (m_menuButton && m_menuBtnToken.value != 0) {
            m_menuButton.Click(m_menuBtnToken); m_menuBtnToken = {};
        }
    } catch (...) {}
    try {
        if (m_dateButton && m_dateBtnClickToken.value != 0) {
            m_dateButton.Click(m_dateBtnClickToken);
            m_dateBtnClickToken = {};
        }
        if (m_calendarView && m_calendarViewSelectionChangedToken.value != 0) {
            m_calendarView.SelectedDatesChanged(m_calendarViewSelectionChangedToken);
            m_calendarViewSelectionChangedToken = {};
        }
        if (m_datePicker) {
            if (m_datePickerLoadedToken.value != 0) {
                m_datePicker.Loaded(m_datePickerLoadedToken);
                m_datePickerLoadedToken = {};
            }
            if (m_dateChangedToken.value != 0) {
                m_datePicker.DateChanged(m_dateChangedToken);
                m_dateChangedToken = {};
            }
        }
    } catch (...) {}

    if (m_hostScrollViewer) {
        try {
            if (m_originalCalendarContent) {
                m_hostScrollViewer.Content(m_originalCalendarContent);
            } else if (m_hostScrollViewer.Content() == m_rootGrid) {
                m_hostScrollViewer.Content(nullptr);
            }
            m_hostScrollViewer.VerticalScrollBarVisibility(m_originalVerticalScrollBarVisibility);
            m_hostScrollViewer.HorizontalScrollBarVisibility(m_originalHorizontalScrollBarVisibility);
        } catch (...) {}
        m_originalCalendarContent = nullptr;
        m_hostScrollViewer = nullptr;
    }

    if (m_rootGrid) {
        try { m_rootGrid.Children().Clear(); } catch (...) {}
        m_rootGrid = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_watcherMutex);
        if (m_itemsControl) {
            try { m_itemsControl.Items().Clear(); } catch (...) {}
            m_itemsControl = nullptr;
        }
    }
    m_eventsScrollViewer = nullptr;
    m_headerGrid = nullptr;
    m_prevDayButton = nullptr;
    m_nextDayButton = nullptr;
    m_menuButton = nullptr;
    m_dateButton = nullptr;
    m_dateButtonText = nullptr;
    m_calendarView = nullptr;
    m_datePicker = nullptr;
    m_datePickerText = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_watcherMutex);
        m_prevDayAccel = nullptr;
        m_nextDayAccel = nullptr;
    }
    g_ownerThreadId.store(0);
    g_lastOpenTick = 0;
}

void PopulateItemsControl(std::vector<CalendarEvent> const& events) {
    if (!m_itemsControl) return;
    m_itemsControl.Items().Clear();
    auto status=CalendarStatusText();
    if (!status.empty()) { auto text=wuxc::TextBlock(); text.Text(status); text.TextWrapping(wux::TextWrapping::Wrap); text.Opacity(0.7); text.Margin(wux::Thickness{0,4,0,8}); m_itemsControl.Items().Append(text); }

    if (events.empty()) {
        auto border = wuxc::Border();
        border.Margin(wux::Thickness{0, 2, 0, 4});
        border.Padding(wux::Thickness{12, 16, 12, 16});
        border.CornerRadius(GetStandardCornerRadius());
        border.Background(GetCardBackgroundBrush());
        border.BorderBrush(GetCardBorderBrush());
        border.BorderThickness(wux::Thickness{1, 1, 1, 1});
        auto tb = wuxc::TextBlock();
        auto message = CalendarStatusText();
        tb.Text(message.empty() ? L"No events scheduled for this date" : message);
        tb.Opacity(0.7);
        tb.TextWrapping(wux::TextWrapping::Wrap);
        tb.HorizontalAlignment(wux::HorizontalAlignment::Center);
        tb.IsTextSelectionEnabled(true);
        border.Child(tb);
        m_itemsControl.Items().Append(border);
        return;
    }

    int timeColumnWidth = GetTimeColumnWidthSetting();

    for (const auto& ev : events) {
        auto border = wuxc::Border();
        border.Margin(wux::Thickness{0, 2, 0, 4});
        border.Padding(wux::Thickness{8, 6, 8, 6});
        border.CornerRadius(GetStandardCornerRadius());
        border.Background(GetCardBackgroundBrush());
        border.BorderBrush(GetCardBorderBrush());
        border.BorderThickness(wux::Thickness{1, 1, 1, 1});

        auto eventGrid = wuxc::Grid();

        wuxc::ColumnDefinition col0{};
        if (timeColumnWidth > 0) {
            col0.Width(wux::GridLength{(double)timeColumnWidth, wux::GridUnitType::Pixel});
        } else {
            col0.Width(wux::GridLength{0, wux::GridUnitType::Auto});
        }
        eventGrid.ColumnDefinitions().Append(col0);

        wuxc::ColumnDefinition col1{};
        col1.Width(wux::GridLength{1, wux::GridUnitType::Star});
        eventGrid.ColumnDefinitions().Append(col1);

        wuxc::RowDefinition row0{};
        row0.Height(wux::GridLength{0, wux::GridUnitType::Auto});
        eventGrid.RowDefinitions().Append(row0);

        wuxc::RowDefinition row1{};
        row1.Height(wux::GridLength{0, wux::GridUnitType::Auto});
        eventGrid.RowDefinitions().Append(row1);

        auto startTimeTb = wuxc::TextBlock();
        startTimeTb.Margin(wux::Thickness{0, 0, 8, 0});
        startTimeTb.VerticalAlignment(wux::VerticalAlignment::Center);
        startTimeTb.IsTextSelectionEnabled(true);
        wuxc::Grid::SetRow(startTimeTb, 0);
        wuxc::Grid::SetColumn(startTimeTb, 0);
        eventGrid.Children().Append(startTimeTb);

        auto nameTb = wuxc::TextBlock();
        nameTb.Text(winrt::hstring(ev.name));
        nameTb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        nameTb.TextWrapping(wux::TextWrapping::Wrap);
        nameTb.VerticalAlignment(wux::VerticalAlignment::Center);
        nameTb.IsTextSelectionEnabled(true);
        wuxc::Grid::SetRow(nameTb, 0);
        wuxc::Grid::SetColumn(nameTb, 1);
        eventGrid.Children().Append(nameTb);

        auto endTimeTb = wuxc::TextBlock();
        endTimeTb.Margin(wux::Thickness{0, 0, 8, 0});
        endTimeTb.VerticalAlignment(wux::VerticalAlignment::Center);
        endTimeTb.IsTextSelectionEnabled(true);
        wuxc::Grid::SetRow(endTimeTb, 1);
        wuxc::Grid::SetColumn(endTimeTb, 0);
        eventGrid.Children().Append(endTimeTb);

        if (ev.isAllDay) {
            startTimeTb.Text(L"All");
            endTimeTb.Text(L"Day");
            endTimeTb.Opacity(0.8);
        } else {
            WCHAR buf[64];
            if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &ev.startLocal, Wh_GetIntSetting(L"use_24_hour_time") ? L"HH:mm" : L"h:mm tt", buf, ARRAYSIZE(buf)) != 0) {
                startTimeTb.Text(winrt::hstring(buf));
            }
            if (ev.hasEnd && GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &ev.endLocal, Wh_GetIntSetting(L"use_24_hour_time") ? L"HH:mm" : L"h:mm tt", buf, ARRAYSIZE(buf)) != 0) {
                endTimeTb.Text(winrt::hstring(buf));
            }
            endTimeTb.Opacity(0.7);
        }

        std::wstring displayLocation = Wh_GetIntSetting(L"show_location") ? ev.location : L"";
        if (!ev.url.empty() && displayLocation == ev.url) {
            displayLocation.clear();
        }

        std::wstring detailsText;
        if (!displayLocation.empty() && !ev.notes.empty()) {
            detailsText = displayLocation + L" - " + ev.notes;
        } else if (!displayLocation.empty()) {
            detailsText = displayLocation;
        } else if (!ev.notes.empty()) {
            detailsText = ev.notes;
        }

        if (!Wh_GetIntSetting(L"show_description")) detailsText = displayLocation;
        if (Wh_GetIntSetting(L"show_calendar_name") && !ev.source.empty()) detailsText = ev.source + (detailsText.empty() ? L"" : L" - " + detailsText);

        auto createLinkButton = [&](const std::wstring& url, bool inSeparateRow) {
            auto linkBtn = wuxc::HyperlinkButton();
            linkBtn.Padding(wux::Thickness{0, 0, 0, 0});
            linkBtn.Margin(inSeparateRow ? wux::Thickness{0, 2, 0, 0} : wux::Thickness{0, 0, 0, 0});
            linkBtn.HorizontalAlignment(wux::HorizontalAlignment::Left);
            linkBtn.VerticalAlignment(wux::VerticalAlignment::Center);

            auto urlTb = wuxc::TextBlock();
            urlTb.Text(winrt::hstring(url));
            urlTb.TextWrapping(wux::TextWrapping::Wrap);
            linkBtn.Content(urlTb);

            try {
                linkBtn.NavigateUri(winrt::Windows::Foundation::Uri{url});
            } catch (...) {}

            wuxc::ToolTipService::SetToolTip(linkBtn, winrt::box_value(winrt::hstring(url)));

            auto flyout = wuxc::MenuFlyout();
            auto copyItem = wuxc::MenuFlyoutItem();
            copyItem.Text(L"Copy");
            try {
                copyItem.Icon(wuxc::SymbolIcon(wuxc::Symbol::Copy));
            } catch (...) {}
            copyItem.Click([url](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                CopyTextToClipboard(url);
            });
            flyout.Items().Append(copyItem);
            linkBtn.ContextFlyout(flyout);

            return linkBtn;
        };

        if (!detailsText.empty()) {
            auto detailsTb = wuxc::TextBlock();
            detailsTb.Text(winrt::hstring(detailsText));
            detailsTb.Opacity(0.7);
            detailsTb.TextWrapping(wux::TextWrapping::Wrap);
            detailsTb.VerticalAlignment(wux::VerticalAlignment::Center);
            detailsTb.IsTextSelectionEnabled(true);
            wuxc::Grid::SetRow(detailsTb, 1);
            wuxc::Grid::SetColumn(detailsTb, 1);
            eventGrid.Children().Append(detailsTb);

            if (!ev.url.empty()) {
                wuxc::RowDefinition row2{};
                row2.Height(wux::GridLength{0, wux::GridUnitType::Auto});
                eventGrid.RowDefinitions().Append(row2);

                auto linkBtn = createLinkButton(ev.url, true);
                wuxc::Grid::SetRow(linkBtn, 2);
                wuxc::Grid::SetColumn(linkBtn, 1);
                eventGrid.Children().Append(linkBtn);
            }
        } else if (!ev.url.empty()) {
            auto linkBtn = createLinkButton(ev.url, false);
            wuxc::Grid::SetRow(linkBtn, 1);
            wuxc::Grid::SetColumn(linkBtn, 1);
            eventGrid.Children().Append(linkBtn);
        } else {
            if (!ev.hasEnd && !ev.isAllDay) {
                endTimeTb.Visibility(wux::Visibility::Collapsed);
            }
        }

        border.Child(eventGrid);
        m_itemsControl.Items().Append(border);
    }
}

void UpdateDateDisplayAndCalendar(SYSTEMTIME const& st) {
    if (m_dateButtonText) {
        m_dateButtonText.Text(FormatFilterDate(st));
    }
    if (m_datePicker) {
        try {
            m_datePicker.Date(SystemTimeToWinRtDateTime(st));
        } catch (...) {}
    }
    if (m_datePickerText) {
        try {
            m_datePickerText.Text(FormatFilterDate(st));
        } catch (...) {}
    }
    if (m_calendarView) {
        auto winrtDt = SystemTimeToWinRtDateTime(st);
        m_isUpdatingCalendarSelection = true;
        try {
            m_calendarView.SetDisplayDate(winrtDt);
            m_calendarView.SelectedDates().Clear();
            m_calendarView.SelectedDates().Append(winrtDt);
        } catch (...) {}
        m_isUpdatingCalendarSelection = false;
    }
}

void ChangeSelectedDay(int deltaDays) {
    SYSTEMTIME newDate = ShiftLocalDate(m_currentFilterDate, deltaDays);
    m_currentFilterDate = newDate;

    UpdateDateDisplayAndCalendar(newDate);

    std::vector<CalendarEvent> filtered;
    {
        std::lock_guard<std::mutex> lock(g_eventsMutex);
        filtered = FilterEventsForDate(g_allParsedEvents, newDate);
    }
    PopulateItemsControl(filtered);
}

void OnDatePickerDateChanged(wf::IReference<wf::DateTime> const& newDate) {
    SYSTEMTIME selected{};
    if (newDate) {
        auto dt = newDate.Value();
        auto ticks = dt.time_since_epoch().count();
        ULARGE_INTEGER uli;
        uli.QuadPart = static_cast<ULONGLONG>(ticks);
        FILETIME ft;
        ft.dwLowDateTime = uli.LowPart;
        ft.dwHighDateTime = uli.HighPart;

        SYSTEMTIME stUtc{}, stLocal{};
        FileTimeToSystemTime(&ft, &stUtc);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUtc, &stLocal);
        selected = stLocal;
    } else {
        GetLocalTime(&selected);
    }

    if (selected.wYear == m_currentFilterDate.wYear &&
        selected.wMonth == m_currentFilterDate.wMonth &&
        selected.wDay == m_currentFilterDate.wDay) return;

    m_currentFilterDate = selected;
    if (m_dateButtonText) {
        m_dateButtonText.Text(FormatFilterDate(selected));
    }
    if (m_datePickerText) {
        try {
            m_datePickerText.Text(FormatFilterDate(selected));
        } catch (...) {}
    }
    std::vector<CalendarEvent> filtered;
    {
        std::lock_guard<std::mutex> lock(g_eventsMutex);
        filtered = FilterEventsForDate(g_allParsedEvents, selected);
    }
    PopulateItemsControl(filtered);
}

void OnCalendarViewSelectedDatesChanged(wf::IReference<wf::DateTime> const& newDate) {
    if (m_isUpdatingCalendarSelection) return;
    SYSTEMTIME selected{};
    if (newDate) {
        auto dt = newDate.Value();
        auto ticks = dt.time_since_epoch().count();
        ULARGE_INTEGER uli;
        uli.QuadPart = static_cast<ULONGLONG>(ticks);
        FILETIME ft;
        ft.dwLowDateTime = uli.LowPart;
        ft.dwHighDateTime = uli.HighPart;

        SYSTEMTIME stUtc{}, stLocal{};
        FileTimeToSystemTime(&ft, &stUtc);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUtc, &stLocal);
        selected = stLocal;
    } else {
        GetLocalTime(&selected);
    }

    if (selected.wYear == m_currentFilterDate.wYear &&
        selected.wMonth == m_currentFilterDate.wMonth &&
        selected.wDay == m_currentFilterDate.wDay) return;

    m_currentFilterDate = selected;
    if (m_dateButtonText) {
        m_dateButtonText.Text(FormatFilterDate(selected));
    }
    if (m_datePickerText) {
        try {
            m_datePickerText.Text(FormatFilterDate(selected));
        } catch (...) {}
    }
    std::vector<CalendarEvent> filtered;
    {
        std::lock_guard<std::mutex> lock(g_eventsMutex);
        filtered = FilterEventsForDate(g_allParsedEvents, selected);
    }
    PopulateItemsControl(filtered);
}

void ResetDatePickerToToday() {
    SYSTEMTIME today;
    GetLocalTime(&today);
    m_currentFilterDate = today;
    UpdateDateDisplayAndCalendar(today);
    std::vector<CalendarEvent> events;
    {
        std::lock_guard<std::mutex> lock(g_eventsMutex);
        events = FilterEventsForDate(g_allParsedEvents, today);
    }
    PopulateItemsControl(events);
}

void DispatchUpdateEvents() {
    wuxc::ItemsControl itemsControl{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_watcherMutex);
        itemsControl = m_itemsControl;
    }
    if (!itemsControl) return;
    auto dispatcher = itemsControl.Dispatcher();
    if (!dispatcher) return;

    auto action = dispatcher.TryRunAsync(wuc::CoreDispatcherPriority::Normal, []() {
        if (HANDLE s = g_hStopEvent.load(); s && WaitForSingleObject(s, 0) == WAIT_OBJECT_0) {
            return;
        }
        std::vector<CalendarEvent> filtered;
        {
            std::lock_guard<std::mutex> lock(g_eventsMutex);
            filtered = FilterEventsForDate(g_allParsedEvents, m_currentFilterDate);
        }
        try { PopulateItemsControl(filtered); } catch (...) {}
    });

    if (action) {
        std::lock_guard<std::mutex> lock(g_pendingActionsMutex);
        std::erase_if(
            g_pendingDispatcherActions,
            [](winrt::Windows::Foundation::IAsyncOperation<bool> const& a) {
                return !a || a.Status() !=
                                 winrt::Windows::Foundation::AsyncStatus::Started;
            });
        g_pendingDispatcherActions.push_back(std::move(action));
    }
}

void UpdateMaxHeight(int maxHeight) {
    wuxc::ScrollViewer sv{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_watcherMutex);
        sv = m_eventsScrollViewer;
    }
    if (sv) {
        sv.MaxHeight(maxHeight > 0 ? (double)maxHeight : std::numeric_limits<double>::infinity());
        sv.Height(std::numeric_limits<double>::quiet_NaN());
    }
}

void ShowCalendarMenu() {
    if (!m_menuButton) return;
    try {
        auto menu = wuxc::MenuFlyout();
        auto accountsMenu = wuxc::MenuFlyoutSubItem();
        accountsMenu.Text(L"Google accounts");
        auto accounts = calendar_provider::LoadAccounts();
        auto settings = calendar_provider::SettingsCopy();

        if (accounts.empty()) {
            auto empty = wuxc::MenuFlyoutItem();
            empty.Text(L"No Google accounts connected");
            empty.IsEnabled(false);
            accountsMenu.Items().Append(empty);
        } else {
            for (const auto& account : accounts) {
                std::wstring label = account.label;
                if (label.size() > 64) label = label.substr(0, 61) + L"...";
                auto connected = wuxc::MenuFlyoutItem();
                connected.Text(L"Connected: " + label);
                connected.IsEnabled(false);
                accountsMenu.Items().Append(connected);

                auto signOut = wuxc::MenuFlyoutItem();
                signOut.Text(L"Sign out: " + label);
                std::string accountId = account.id;
                signOut.Click([accountId](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    if (!calendar_provider::IsHexId(accountId)) return;
                    std::wstring key = L"google_signout_request_" +
                                       calendar_provider::Utf8ToWide(accountId);
                    auto statusKey = calendar_provider::UserValueName(L"account_status");
                    Wh_SetStringValue(statusKey.c_str(),
                                      L"Sign-out requested. Reopen Google accounts shortly.");
                    if (!calendar_provider::SubmitAccountRequest(key)) {
                        Wh_SetStringValue(statusKey.c_str(), L"Could not request Google sign-out.");
                        Wh_Log(L"could not request Google sign-out");
                    }
                });
                accountsMenu.Items().Append(signOut);
            }
        }

        accountsMenu.Items().Append(wuxc::MenuFlyoutSeparator());
        auto signIn = wuxc::MenuFlyoutItem();
        signIn.Text(L"Sign in with Google");
        signIn.IsEnabled(calendar_provider::ClientConfigured(settings) &&
                         accounts.size() < calendar_provider::kMaxAccounts);
        signIn.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            auto statusKey = calendar_provider::UserValueName(L"account_status");
            Wh_SetStringValue(statusKey.c_str(),
                              L"Sign-in requested. Your browser will open shortly.");
            if (!calendar_provider::SubmitAccountRequest(L"google_signin_request")) {
                Wh_SetStringValue(statusKey.c_str(), L"Could not request Google sign-in.");
                Wh_Log(L"could not request Google sign-in");
            }
        });
        accountsMenu.Items().Append(signIn);

        if (!calendar_provider::ClientConfigured(settings)) {
            auto hint = wuxc::MenuFlyoutItem();
            hint.Text(L"Set your Google OAuth client in mod settings");
            hint.IsEnabled(false);
            accountsMenu.Items().Append(hint);
        } else if (accounts.size() >= calendar_provider::kMaxAccounts) {
            auto hint = wuxc::MenuFlyoutItem();
            hint.Text(L"Four-account limit reached");
            hint.IsEnabled(false);
            accountsMenu.Items().Append(hint);
        }

        std::wstring status = calendar_provider::ReadShortUserValue(
            calendar_provider::UserValueName(L"account_status"));
        if (!status.empty()) {
            if (status.size() > 120) status = status.substr(0, 117) + L"...";
            accountsMenu.Items().Append(wuxc::MenuFlyoutSeparator());
            auto statusItem = wuxc::MenuFlyoutItem();
            statusItem.Text(status);
            statusItem.IsEnabled(false);
            accountsMenu.Items().Append(statusItem);
        }
        menu.Items().Append(accountsMenu);

        auto refresh = wuxc::MenuFlyoutItem();
        refresh.Text(L"Refresh events");
        refresh.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            TriggerBackgroundFetch(true);
        });
        menu.Items().Append(refresh);

        auto today = wuxc::MenuFlyoutItem();
        today.Text(L"Go to today");
        today.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ResetDatePickerToToday();
        });
        menu.Items().Append(today);

        menu.ShowAt(m_menuButton);
    } catch (...) {
        Wh_Log(L"could not show calendar menu");
    }
}

void ReplaceCalendarContent(wuxc::ScrollViewer const& host) {
    if (!host) return;
    DWORD currentThread = GetCurrentThreadId();
    DWORD expected = 0;
    if (!g_ownerThreadId.compare_exchange_strong(expected, currentThread) && expected != currentThread) {
        return;
    }
    RegisterCoreWindowEvents();

    if (m_currentFilterDate.wYear == 0) {
        GetLocalTime(&m_currentFilterDate);
    }

    try {
        if (m_hostScrollViewer && m_hostScrollViewer != host) {
            try {
                if (m_originalCalendarContent) m_hostScrollViewer.Content(m_originalCalendarContent);
                m_hostScrollViewer.VerticalScrollBarVisibility(m_originalVerticalScrollBarVisibility);
                m_hostScrollViewer.HorizontalScrollBarVisibility(m_originalHorizontalScrollBarVisibility);
            } catch (...) {}
            m_originalCalendarContent = nullptr;
            m_hostScrollViewer = nullptr;
        }

        {
            std::lock_guard<std::mutex> lock(g_watcherMutex);
            if (!m_itemsControl) m_itemsControl = wuxc::ItemsControl();
        }

        bool isInline = IsInlineCalendarMode();

        if (!m_eventsScrollViewer) {
            m_eventsScrollViewer = wuxc::ScrollViewer();
            m_eventsScrollViewer.Name(L"CustomCalendarScrollViewer");
            if (isInline) {
                m_eventsScrollViewer.Margin(wux::Thickness{12, 0, 12, 0});
            }
            m_eventsScrollViewer.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
            m_eventsScrollViewer.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
            m_eventsScrollViewer.Content(m_itemsControl);
        }

        int maxHeight = GetMaxHeightCollapsedSetting();
        if (maxHeight > 0) m_eventsScrollViewer.MaxHeight((double)maxHeight);
        else m_eventsScrollViewer.MaxHeight(std::numeric_limits<double>::infinity());
        m_eventsScrollViewer.Height(std::numeric_limits<double>::quiet_NaN());

        if (isInline) {
            if (!m_dateButton) {
                m_dateButton = wuxc::Button();
                m_dateButton.Name(L"CustomCalendarDateButton");
                m_dateButton.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                m_dateButton.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
                m_dateButton.VerticalAlignment(wux::VerticalAlignment::Center);
                m_dateButton.MinWidth(0);
                m_dateButton.Margin(wux::Thickness{0, 0, 0, 6});
                wuxc::ToolTipService::SetToolTip(m_dateButton, winrt::box_value(L"Show calendar"));

                m_dateButtonText = wuxc::TextBlock();
                m_dateButtonText.Text(FormatFilterDate(m_currentFilterDate));
                m_dateButtonText.VerticalAlignment(wux::VerticalAlignment::Center);
                m_dateButtonText.HorizontalTextAlignment(wux::TextAlignment::Left);
                m_dateButtonText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
                m_dateButtonText.TextWrapping(wux::TextWrapping::NoWrap);
                m_dateButton.Content(m_dateButtonText);

                m_dateBtnClickToken = m_dateButton.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    if (!m_calendarView) return;
                    bool isAbove = IsCalendarAboveAgenda();
                    if (m_calendarView.Visibility() == wux::Visibility::Visible) {
                        m_calendarView.Visibility(wux::Visibility::Collapsed);
                        wuxc::ToolTipService::SetToolTip(m_dateButton, winrt::box_value(L"Show calendar"));
                        if (isAbove) {
                            if (m_headerGrid) m_headerGrid.Margin(wux::Thickness{12, 10, 12, 0});
                        }
                        UpdateMaxHeight(GetMaxHeightCollapsedSetting());
                    } else {
                        m_calendarView.Visibility(wux::Visibility::Visible);
                        wuxc::ToolTipService::SetToolTip(m_dateButton, winrt::box_value(L"Hide calendar"));
                        if (isAbove) {
                            if (m_headerGrid) m_headerGrid.Margin(wux::Thickness{12, 2, 12, 0});
                        }
                        UpdateMaxHeight(GetMaxHeightExpandedSetting());
                    }
                });
            }

            if (!m_calendarView) {
                m_calendarView = wuxc::CalendarView();
                m_calendarView.Name(L"CustomCalendarView");
                m_calendarView.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                m_calendarView.Margin(wux::Thickness{0, 0, 0, 4});
                m_calendarView.BorderThickness(wux::Thickness{0, 0, 0, 0});
                m_calendarView.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
                m_calendarView.IsTodayHighlighted(true);
                m_calendarView.SelectionMode(wuxc::CalendarViewSelectionMode::Single);
                m_calendarView.Visibility(wux::Visibility::Collapsed);
                m_calendarViewSelectionChangedToken = m_calendarView.SelectedDatesChanged([](wuxc::CalendarView const& cv, wuxc::CalendarViewSelectedDatesChangedEventArgs const& args) {
                    if (args.AddedDates().Size() > 0) {
                        OnCalendarViewSelectedDatesChanged(args.AddedDates().GetAt(0));
                    }
                });
            }
        } else {
            if (!m_datePicker) {
                m_datePicker = wuxc::CalendarDatePicker();
                m_datePicker.Name(L"CustomCalendarDatePicker");
                m_datePicker.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
                m_datePicker.VerticalAlignment(wux::VerticalAlignment::Center);
                m_datePicker.Margin(wux::Thickness{0, 0, 0, 6});
                m_datePicker.IsTodayHighlighted(true);
                m_datePicker.DateFormat(GetCalendarDatePickerFormat());
                m_datePicker.Date(winrt::clock::now());
                m_dateChangedToken = m_datePicker.DateChanged([](wuxc::CalendarDatePicker const&, wuxc::CalendarDatePickerDateChangedEventArgs const& args) {
                    OnDatePickerDateChanged(args.NewDate());
                });
                m_datePickerLoadedToken = m_datePicker.Loaded([](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
                    try {
                        if (auto cdp = sender.try_as<wux::FrameworkElement>()) {
                            std::vector<wux::DependencyObject> queue;
                            queue.push_back(cdp);
                            while (!queue.empty()) {
                                auto current = queue.back();
                                queue.pop_back();

                                if (auto grid = current.try_as<wuxc::Grid>()) {
                                    if (grid.ColumnDefinitions().Size() >= 3) {
                                        grid.ColumnDefinitions().GetAt(2).Width(
                                            wux::GridLength{0.0, wux::GridUnitType::Pixel}
                                        );
                                    }
                                }

                                int count = wuxm::VisualTreeHelper::GetChildrenCount(current);
                                for (int i = 0; i < count; ++i) {
                                    auto child = wuxm::VisualTreeHelper::GetChild(current, i);
                                    if (auto fe = child.try_as<wux::FrameworkElement>()) {
                                        if (fe.Name() == L"CalendarGlyph") {
                                            fe.Visibility(wux::Visibility::Collapsed);
                                        } else if (fe.Name() == L"DateText") {
                                            m_datePickerText = fe.try_as<wuxc::TextBlock>();
                                            wuxc::Grid::SetColumnSpan(fe, 2);
                                            if (m_datePickerText) {
                                                try {
                                                    m_datePickerText.Text(FormatFilterDate(m_currentFilterDate));
                                                } catch (...) {}
                                            }
                                        }
                                    }
                                    queue.push_back(child);
                                }
                            }
                        }
                    } catch (...) {}
                });
            }
        }

        if (!m_prevDayButton) {
            m_prevDayButton = wuxc::Button();
            m_prevDayButton.Name(L"CustomCalendarPrevDayButton");
            m_prevDayButton.Width(32);
            m_prevDayButton.Height(32);
            m_prevDayButton.Padding(wux::Thickness{0, 0, 0, 0});
            m_prevDayButton.Margin(wux::Thickness{0, 0, 6, 6});
            m_prevDayButton.VerticalAlignment(wux::VerticalAlignment::Center);
            auto prevIcon = wuxc::FontIcon(); prevIcon.Glyph(L"\uE76B"); prevIcon.FontSize(12);
            m_prevDayButton.Content(prevIcon);
            m_prevBtnToken = m_prevDayButton.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) { ChangeSelectedDay(-1); });
            m_prevDayAccel = wuxi::KeyboardAccelerator();
            m_prevDayAccel.Key(ws::VirtualKey::Left);
            wuxc::ToolTipService::SetToolTip(
                m_prevDayButton,
                winrt::box_value(L"Previous day")
            );
            m_prevDayAccel.IsEnabled(GetKeyboardShortcutsSetting());
            m_prevDayAccel.Invoked([](wuxi::KeyboardAccelerator const&, wuxi::KeyboardAcceleratorInvokedEventArgs const& args) {
                if (!GetKeyboardShortcutsSetting()) return;
                if (m_calendarView && m_calendarView.Visibility() == wux::Visibility::Visible) return;
                if (m_datePicker && m_datePicker.IsCalendarOpen()) return;
                ChangeSelectedDay(-1); args.Handled(true);
            });
            m_prevDayButton.KeyboardAccelerators().Append(m_prevDayAccel);
        }

        if (!m_nextDayButton) {
            m_nextDayButton = wuxc::Button();
            m_nextDayButton.Name(L"CustomCalendarNextDayButton");
            m_nextDayButton.Width(32);
            m_nextDayButton.Height(32);
            m_nextDayButton.Padding(wux::Thickness{0, 0, 0, 0});
            m_nextDayButton.Margin(wux::Thickness{6, 0, 6, 6});
            m_nextDayButton.VerticalAlignment(wux::VerticalAlignment::Center);
            auto nextIcon = wuxc::FontIcon(); nextIcon.Glyph(L"\uE76C"); nextIcon.FontSize(12);
            m_nextDayButton.Content(nextIcon);
            m_nextBtnToken = m_nextDayButton.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) { ChangeSelectedDay(1); });
            m_nextDayAccel = wuxi::KeyboardAccelerator();
            m_nextDayAccel.Key(ws::VirtualKey::Right);
            wuxc::ToolTipService::SetToolTip(
                m_nextDayButton,
                winrt::box_value(L"Next day")
            );
            m_nextDayAccel.IsEnabled(GetKeyboardShortcutsSetting());
            m_nextDayAccel.Invoked([](wuxi::KeyboardAccelerator const&, wuxi::KeyboardAcceleratorInvokedEventArgs const& args) {
                if (!GetKeyboardShortcutsSetting()) return;
                if (m_calendarView && m_calendarView.Visibility() == wux::Visibility::Visible) return;
                if (m_datePicker && m_datePicker.IsCalendarOpen()) return;
                ChangeSelectedDay(1); args.Handled(true);
            });
            m_nextDayButton.KeyboardAccelerators().Append(m_nextDayAccel);
        }

        if (!m_menuButton) {
            m_menuButton = wuxc::Button();
            m_menuButton.Name(L"CustomCalendarMenuButton");
            m_menuButton.Width(32);
            m_menuButton.Height(32);
            m_menuButton.Padding(wux::Thickness{0, 0, 0, 0});
            m_menuButton.Margin(wux::Thickness{0, 0, 0, 6});
            m_menuButton.VerticalAlignment(wux::VerticalAlignment::Center);
            auto moreIcon = wuxc::FontIcon(); moreIcon.Glyph(L"\uE712"); moreIcon.FontSize(16);
            m_menuButton.Content(moreIcon);
            wuxc::ToolTipService::SetToolTip(m_menuButton, winrt::box_value(L"Calendar options"));
            m_menuBtnToken = m_menuButton.Click(
                [](wf::IInspectable const&, wux::RoutedEventArgs const&) { ShowCalendarMenu(); });
        }

        if (!m_headerGrid) {
            m_headerGrid = wuxc::Grid();
            m_headerGrid.Name(L"CustomCalendarHeaderGrid");
            if (isInline) {
                if (IsCalendarAboveAgenda()) {
                    m_headerGrid.Margin(wux::Thickness{12, 10, 12, 0});
                } else {
                    m_headerGrid.Margin(wux::Thickness{12, 6, 12, 0});
                }
            } else {
                m_headerGrid.Margin(wux::Thickness{0, 6, 0, 0});
            }
            wuxc::ColumnDefinition col0{}; col0.Width(wux::GridLength{0, wux::GridUnitType::Auto}); m_headerGrid.ColumnDefinitions().Append(col0);
            wuxc::ColumnDefinition col1{}; col1.Width(wux::GridLength{1, wux::GridUnitType::Star}); m_headerGrid.ColumnDefinitions().Append(col1);
            wuxc::ColumnDefinition col2{}; col2.Width(wux::GridLength{0, wux::GridUnitType::Auto}); m_headerGrid.ColumnDefinitions().Append(col2);
            wuxc::ColumnDefinition col3{}; col3.Width(wux::GridLength{0, wux::GridUnitType::Auto}); m_headerGrid.ColumnDefinitions().Append(col3);
            wuxc::Grid::SetColumn(m_prevDayButton, 0); m_headerGrid.Children().Append(m_prevDayButton);
            if (isInline) {
                wuxc::Grid::SetColumn(m_dateButton, 1); m_headerGrid.Children().Append(m_dateButton);
            } else {
                wuxc::Grid::SetColumn(m_datePicker, 1); m_headerGrid.Children().Append(m_datePicker);
            }
            wuxc::Grid::SetColumn(m_nextDayButton, 2); m_headerGrid.Children().Append(m_nextDayButton);
            wuxc::Grid::SetColumn(m_menuButton, 3); m_headerGrid.Children().Append(m_menuButton);
        }

        if (!m_rootGrid) {
            m_rootGrid = wuxc::Grid();
            m_rootGrid.Name(L"CustomCalendarRootGrid");
            bool isAbove = IsCalendarAboveAgenda();
            if (isInline) {
                m_rootGrid.Margin(wux::Thickness{0, 0, 0, 4});
                wuxc::RowDefinition row0{}; row0.Height(wux::GridLength{0, wux::GridUnitType::Auto}); m_rootGrid.RowDefinitions().Append(row0);
                wuxc::RowDefinition row1{}; row1.Height(wux::GridLength{0, wux::GridUnitType::Auto}); m_rootGrid.RowDefinitions().Append(row1);
                wuxc::RowDefinition row2{}; row2.Height(wux::GridLength{0, wux::GridUnitType::Auto}); m_rootGrid.RowDefinitions().Append(row2);
                if (isAbove) {
                    wuxc::Grid::SetRow(m_calendarView, 0); m_rootGrid.Children().Append(m_calendarView);
                    wuxc::Grid::SetRow(m_headerGrid, 1); m_rootGrid.Children().Append(m_headerGrid);
                    wuxc::Grid::SetRow(m_eventsScrollViewer, 2); m_rootGrid.Children().Append(m_eventsScrollViewer);
                    m_eventsScrollViewer.Margin(wux::Thickness{12, 0, 12, 0});
                } else {
                    wuxc::Grid::SetRow(m_eventsScrollViewer, 0); m_rootGrid.Children().Append(m_eventsScrollViewer);
                    wuxc::Grid::SetRow(m_headerGrid, 1); m_rootGrid.Children().Append(m_headerGrid);
                    wuxc::Grid::SetRow(m_calendarView, 2); m_rootGrid.Children().Append(m_calendarView);
                    m_eventsScrollViewer.Margin(wux::Thickness{12, 10, 12, 0});
                    m_headerGrid.Margin(wux::Thickness{12, 6, 12, 0});
                    m_calendarView.Margin(wux::Thickness{0, 0, 0, 4});
                }
            } else {
                m_rootGrid.Margin(wux::Thickness{12, 4, 12, 4});
                wuxc::RowDefinition row0{}; row0.Height(wux::GridLength{0, wux::GridUnitType::Auto}); m_rootGrid.RowDefinitions().Append(row0);
                wuxc::RowDefinition row1{}; row1.Height(wux::GridLength{0, wux::GridUnitType::Auto}); m_rootGrid.RowDefinitions().Append(row1);
                if (isAbove) {
                    wuxc::Grid::SetRow(m_headerGrid, 0); m_rootGrid.Children().Append(m_headerGrid);
                    wuxc::Grid::SetRow(m_eventsScrollViewer, 1); m_rootGrid.Children().Append(m_eventsScrollViewer);
                    m_headerGrid.Margin(wux::Thickness{0, 6, 0, 0});
                    m_eventsScrollViewer.Margin(wux::Thickness{0, 0, 0, 0});
                } else {
                    wuxc::Grid::SetRow(m_eventsScrollViewer, 0); m_rootGrid.Children().Append(m_eventsScrollViewer);
                    wuxc::Grid::SetRow(m_headerGrid, 1); m_rootGrid.Children().Append(m_headerGrid);
                    m_eventsScrollViewer.Margin(wux::Thickness{0, 6, 0, 0});
                    m_headerGrid.Margin(wux::Thickness{0, 6, 0, 0});
                }
            }
        }

        SYSTEMTIME today; GetLocalTime(&today); m_currentFilterDate = today;
        std::vector<CalendarEvent> currentEvents;
        { std::lock_guard<std::mutex> lock(g_eventsMutex); currentEvents = FilterEventsForDate(g_allParsedEvents, today); }
        PopulateItemsControl(currentEvents);
        UpdateDateDisplayAndCalendar(m_currentFilterDate);

        if (m_rootGrid) {
            if (auto parent = m_rootGrid.Parent()) {
                if (auto parentContentControl = parent.try_as<wuxc::ContentControl>()) {
                    if (parentContentControl.Content() == m_rootGrid) parentContentControl.Content(nullptr);
                } else if (auto parentPanel = parent.try_as<wuxc::Panel>()) {
                    uint32_t index = 0;
                    if (parentPanel.Children().IndexOf(m_rootGrid, index)) parentPanel.Children().RemoveAt(index);
                }
            }
        }

        if (!m_originalCalendarContent && host.Content() != m_rootGrid) {
            m_originalCalendarContent = host.Content();
            m_originalVerticalScrollBarVisibility = host.VerticalScrollBarVisibility();
            m_originalHorizontalScrollBarVisibility = host.HorizontalScrollBarVisibility();
        }
        m_hostScrollViewer = host;

        host.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
        host.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
        host.Content(m_rootGrid);
        RevokeLayoutUpdated();
        OnCalendarOpened();
    } catch (...) {}
}

void HandleFocusSessionControl(wux::FrameworkElement const& element) {
    if (!element || m_focusSessionControl == element) return;
    DWORD currentThread = GetCurrentThreadId();
    DWORD expected = 0;
    if (!g_ownerThreadId.compare_exchange_strong(expected, currentThread) && expected != currentThread) {
        return;
    }

    UnregisterFocusSessionControl();
    m_focusSessionControl = element;
    m_originalFocusSessionVisibility = element.Visibility();

    try {
        bool hide = ShouldHideFocusSession();
        if (hide) element.Visibility(wux::Visibility::Collapsed);

        if (m_focusSessionVisibilityToken == 0) {
            m_focusSessionVisibilityToken = element.RegisterPropertyChangedCallback(
                wux::UIElement::VisibilityProperty(),
                [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                    if (auto fe = sender.try_as<wux::FrameworkElement>()) {
                        if (ShouldHideFocusSession() && fe.Visibility() != wux::Visibility::Collapsed) {
                            fe.Visibility(wux::Visibility::Collapsed);
                        }
                    }
                });
        }
    } catch (...) {}
}

void WalkVisualTree(winrt::Windows::UI::Xaml::DependencyObject const& root) {
    if (!root) return;
    DWORD owner = g_ownerThreadId.load();
    if (owner != 0 && owner != GetCurrentThreadId()) return;

    try {
        if (auto fe = root.try_as<wux::FrameworkElement>()) {
            auto name = fe.Name();
            if (name == L"CalendarControlScrollViewer") {
                if (auto scrollViewer = fe.try_as<wuxc::ScrollViewer>()) {
                    if (scrollViewer != m_hostScrollViewer) ReplaceCalendarContent(scrollViewer);
                }
            } else if (name == L"FocusSessionControl" || (!m_focusSessionControl && winrt::get_class_name(fe) == L"ActionCenter.FocusSessionControl")) {
                HandleFocusSessionControl(fe);
            }
        }
        int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < count; i++) WalkVisualTree(wuxm::VisualTreeHelper::GetChild(root, i));
    } catch (...) {}
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); }
    catch (...) { Wh_Log(L"Could not initialize calendar reader apartment"); return 0; }
    std::wstring lastPayload, lastStatus;
    std::vector<wchar_t> buffer;
    bool readFailed = false;
    int64_t lastMinute = -1;
    while (true) {
        HANDLE stop = g_hStopEvent.load(), work = g_hWorkEvent.load();
        if (!stop || !work) break;
        HANDLE handles[] = {stop, work};
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, 1500);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
        try {
            if (g_workerForceFetch.exchange(false)) {
                Wh_SetIntValue(calendar_provider::UserValueName(L"calendar_refresh").c_str(),
                               static_cast<int>(GetTickCount64() & 0x7fffffff));
            }
            std::wstring payload;
            if (!calendar_provider::ReadCalendarPayload(payload, buffer)) {
                readFailed = true;
                {
                    std::lock_guard<std::mutex> lock(g_eventsMutex);
                    g_providerStatus = L"Calendar cache could not be read. Choose Refresh events from the three-dot menu.";
                }
                DispatchUpdateEvents();
                continue;
            }
            wchar_t statusBuffer[2048]{};
            Wh_GetStringValue(calendar_provider::UserValueName(L"calendar_status").c_str(),statusBuffer,ARRAYSIZE(statusBuffer));
            std::wstring status(statusBuffer);
            int64_t minute = static_cast<int64_t>(GetTickCount64()/60000);
            if (!readFailed && payload == lastPayload && status == lastStatus && minute == lastMinute) continue;
            readFailed = false;
            std::vector<CalendarEvent> converted;
            int64_t generated = 0; bool partial = false; bool parsed = true;
            try {
                using winrt::Windows::Data::Json::JsonObject;
                if (!payload.empty()) {
                    JsonObject root = JsonObject::Parse(payload);
                    const double timestamp = root.GetNamedNumber(L"generated", 0);
                    if (!std::isfinite(timestamp) || timestamp < 0 || timestamp > 7289654400.0)
                        throw winrt::hresult_invalid_argument();
                    generated = static_cast<int64_t>(timestamp);
                    partial = root.GetNamedBoolean(L"partial",false);
                    auto items = root.GetNamedArray(L"items");
                    for (uint32_t i=0; i<items.Size() && i<4000; ++i) {
                        auto item=items.GetObjectAt(i); CalendarEvent e;
                        e.name=item.GetNamedString(L"title",L"Untitled event");
                        e.location=item.GetNamedString(L"location",L"");
                        e.notes=item.GetNamedString(L"description",L"");
                        e.source=item.GetNamedString(L"source",L"");
                        e.url=item.GetNamedString(L"url",L"");
                        e.isAllDay=item.GetNamedBoolean(L"allDay",false);
                        e.hasEnd=true;
                        const double start = item.GetNamedNumber(L"start", 0);
                        const double end = item.GetNamedNumber(L"end", 0);
                        if (!std::isfinite(start) || !std::isfinite(end) || start <= 0 ||
                            end <= start || end > 7289654400.0) { partial = true; continue; }
                        e.startLocal=UnixToLocal(static_cast<int64_t>(start),e.isAllDay);
                        e.endLocal=UnixToLocal(static_cast<int64_t>(end),e.isAllDay);
                        if (!e.startLocal.wYear || !e.endLocal.wYear) { partial = true; continue; }
                        FILETIME ft{}; SystemTimeToFileTime(&e.startLocal,&ft);
                        e.sortKey=(static_cast<ULONGLONG>(ft.dwHighDateTime)<<32)|ft.dwLowDateTime;
                        converted.push_back(std::move(e));
                    }
                }
            } catch (...) {
                parsed = false;
                status=L"Calendar cache could not be read. Choose Refresh events from the three-dot menu.";
            }
            {
                std::lock_guard<std::mutex> lock(g_eventsMutex);
                if (parsed) g_allParsedEvents=std::move(converted);
                g_providerStatus=std::move(status); g_snapshotUnix=generated; g_partialSnapshot=partial;
            }
            lastPayload=std::move(payload); lastStatus=statusBuffer; lastMinute=minute;
            DispatchUpdateEvents();
        } catch (...) {
            Wh_Log(L"Calendar reader iteration failed; retrying");
        }
    }
    winrt::uninit_apartment(); return 0;
}

bool StartWorkerThread() {
    if (g_hWorkerThread.load()) {
        return true;
    }
    g_hWorkEvent.store(CreateEventW(nullptr, FALSE, FALSE, nullptr));
    g_hStopEvent.store(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!g_hWorkEvent.load() || !g_hStopEvent.load()) {
        StopWorkerThread();
        Wh_Log(L"Failed to create native calendar worker events");
        return false;
    }
    g_hWorkerThread.store(CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr));
    if (!g_hWorkerThread.load()) {
        StopWorkerThread();
        Wh_Log(L"Failed to create native calendar worker thread");
        return false;
    }
    return true;
}

void StopWorkerThread() {
    HANDLE workerThread = g_hWorkerThread.exchange(nullptr);
    if (HANDLE stopEvent = g_hStopEvent.load()) {
        SetEvent(stopEvent);
    }
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
        CloseHandle(workerThread);
    }
    if (HANDLE workEvent = g_hWorkEvent.exchange(nullptr)) {
        CloseHandle(workEvent);
    }
    if (HANDLE stopEvent = g_hStopEvent.exchange(nullptr)) {
        CloseHandle(stopEvent);
    }
}

void TriggerBackgroundFetch(bool force) {
    if (force) {
        g_workerForceFetch = true;
    }
    if (HANDLE workEvent = g_hWorkEvent.load()) {
        SetEvent(workEvent);
    }
}

void OnCalendarOpened() {
    DWORD owner = g_ownerThreadId.load();
    if (owner != 0 && owner != GetCurrentThreadId()) {
        return;
    }

    ULONGLONG currentTick = GetTickCount64();
    if (m_rootGrid && currentTick - g_lastOpenTick < 500) {
        return;
    }
    g_lastOpenTick = currentTick;

    Wh_Log(
        L"Calendar opened, resetting date picker to today and triggering "
        L"background fetch");

    ResetDatePickerToToday();

    bool force = false;
    TriggerBackgroundFetch(force);

    try {
        auto window = winrt::Windows::UI::Xaml::Window::Current();
        if (window && window.Content()) {
            if (auto fe = window.Content().try_as<winrt::Windows::UI::Xaml::FrameworkElement>()) {
                if (!m_rootGrid && t_coreWindowData.layoutUpdatedToken.value == 0) {
                    t_coreWindowData.layoutUpdatedFe = fe;
                    t_coreWindowData.layoutUpdatedToken = fe.LayoutUpdated([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
                        if (m_rootGrid) {
                            RevokeLayoutUpdated();
                            return;
                        }
                        auto w = winrt::Windows::UI::Xaml::Window::Current();
                        if (w) {
                            if (w.Content()) WalkVisualTree(w.Content());
                            auto popups = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetOpenPopups(w);
                            for (auto popup : popups) {
                                if (popup.Child()) WalkVisualTree(popup.Child());
                            }
                        }
                        if (m_rootGrid) {
                            RevokeLayoutUpdated();
                        }
                    });
                }
            }
            WalkVisualTree(window.Content());
            auto popups = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetOpenPopups(window);
            for (auto popup : popups) {
                if (popup.Child()) WalkVisualTree(popup.Child());
            }
        }
    } catch (...) {
        Wh_Log(L"Exception getting Window::Current()");
    }
}
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsgViaPostMessage =
        RegisterWindowMessage(
            L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = (MSG*)lParam;
                if (msg->message ==
                    runFromWindowThreadRegisteredMsgViaPostMessage) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{
        .proc = proc,
        .procParam = procParam,
        .hook = hook,
    };
    if (!PostMessage(hWnd, runFromWindowThreadRegisteredMsgViaPostMessage, 0,
                     (LPARAM)param)) {
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
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

void RegisterCoreWindowEvents() {
    try {
        auto coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) {
            return;
        }
        if (t_coreWindowData.coreWindow && t_coreWindowData.coreWindow == coreWindow) {
            return;
        }
        Wh_Log(
            L"Registering CoreWindow Activated & VisibilityChanged for thread %u",
            GetCurrentThreadId());

        auto activatedToken = coreWindow.Activated(
            [](auto&&, wuc::WindowActivatedEventArgs const& args) {
                if (args.WindowActivationState() !=
                    wuc::CoreWindowActivationState::Deactivated) {
                    Wh_Log(L"CoreWindow Activated");
                    OnCalendarOpened();
                }
            });

        auto visibilityChangedToken = coreWindow.VisibilityChanged(
            [](auto&&, wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) {
                    Wh_Log(L"CoreWindow VisibilityChanged: Visible");
                    OnCalendarOpened();
                }
            });

        t_coreWindowData.coreWindow = coreWindow;
        t_coreWindowData.activatedToken = activatedToken;
        t_coreWindowData.visibilityChangedToken = visibilityChangedToken;
    } catch (...) {
        Wh_Log(L"Failed to register CoreWindow events: %08X",
               winrt::to_hresult());
    }
}

void UnregisterCoreWindowEvents() {
    RevokeLayoutUpdated();
    try {
        if (t_coreWindowData.coreWindow) {
            if (t_coreWindowData.activatedToken.value != 0) {
                t_coreWindowData.coreWindow.Activated(t_coreWindowData.activatedToken);
            }
            if (t_coreWindowData.visibilityChangedToken.value != 0) {
                t_coreWindowData.coreWindow.VisibilityChanged(t_coreWindowData.visibilityChangedToken);
            }
            t_coreWindowData = {};
        }
    } catch (...) {}
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);

        RunFromWindowThreadViaPostMessage(
            hWnd,
            [](PVOID) {
                RegisterCoreWindowEvents();
            },
            nullptr);
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
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
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;

HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
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
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

std::vector<HWND> GetCoreWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Init() {
    Wh_Log(L"Windows Calendar Plus native view initializing");
    calendar_provider::g_userKey=calendar_provider::CurrentUserSid();
    if (calendar_provider::g_userKey.empty()) return FALSE;
    calendar_provider::LoadSettings();

    if (!StartWorkerThread()) return FALSE;

    HMODULE user32Module = GetModuleHandleW(L"user32.dll");
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void AfterInit() {
    Wh_Log(L">");

    for (HWND hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Initializing for existing CoreWindow: %08X",
               (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                RegisterCoreWindowEvents();
                OnCalendarOpened();
            },
            nullptr);
    }
}

void Uninit() {
    Wh_Log(L">");

    StopWorkerThread();

    {
        std::lock_guard<std::mutex> lock(g_pendingActionsMutex);
        for (auto const& action : g_pendingDispatcherActions) {
            if (action && action.Status() ==
                              winrt::Windows::Foundation::AsyncStatus::Started) {
                try {
                    action.Cancel();
                } catch (...) {}
            }
        }
        g_pendingDispatcherActions.clear();
    }

    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_calendarCaches.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_eventsMutex);
        g_allParsedEvents.clear();
    }

    for (HWND hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                RevokeLayoutUpdated();
                UnregisterCoreWindowEvents();
                RestoreCalendarContent();
            },
            nullptr);
    }
}

}

namespace { bool g_isExplorer=false; }
BOOL Wh_ModInit() {
    wchar_t path[MAX_PATH]{}; GetModuleFileNameW(nullptr,path,ARRAYSIZE(path));
    const wchar_t* filename=wcsrchr(path,L'\\'); filename=filename?filename+1:path;
    g_isExplorer=_wcsicmp(filename,L"explorer.exe")==0;
    return g_isExplorer?calendar_provider::Init():native_agenda::Init();
}
void Wh_ModAfterInit() { if(g_isExplorer) calendar_provider::AfterInit(); else native_agenda::AfterInit(); }
void Wh_ModUninit() { if(g_isExplorer) calendar_provider::Uninit(); else native_agenda::Uninit(); }
BOOL Wh_ModSettingsChanged(BOOL* reload) { *reload=TRUE; return TRUE; }
