// ==WindhawkMod==
// @id              tray-agenda
// @name            Tray Agenda
// @description     Calendar agenda widget in the Windows 11 tray (Google Calendar accounts and ICS feeds) with native reminder notifications.
// @version         1.0.0
// @author          Rubens Nascimento
// @github          https://github.com/chambber
// @homepage        https://github.com/chambber/tray-agenda
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lwinhttp -lbcrypt -lcrypt32 -lws2_32 -ladvapi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Agenda

![Tray widget](https://raw.githubusercontent.com/chambber/tray-agenda/55ec9a66eb459d45779657b0640f303dd7fff4c9/docs/widget.png)

![Popup agenda](https://raw.githubusercontent.com/chambber/tray-agenda/55ec9a66eb459d45779657b0640f303dd7fff4c9/docs/popup.png)

A compact agenda widget for the Windows 11 system tray, with a Notion-Calendar-style
popup and native Windows reminders. It reads **Google Calendar** (up to 4 accounts) and
**ICS feeds** directly. There is no helper program: the mod signs in with your own Google
OAuth client and keeps only read-only refresh tokens, encrypted with Windows DPAPI in
Windhawk's mod storage.

## Features

- Tray widget showing the current or next meeting. An event starting within the
  reminder lead time takes over from the one in progress. Out-of-office events never
  win over an overlapping regular event.
- Popup agenda grouped by day, with a dotted bar for events you have not answered or
  accepted tentatively.
- Join buttons for Google Meet, Zoom and Microsoft Teams links.
- Native Windows toast reminders for **accepted** events, once before the start
  (10 minutes by default) and again at the start time. Out-of-office events are never
  announced. The toast has a **Join** button when the event has a meeting link.
- Several sources at once: up to 4 Google accounts plus ICS feed URLs (Google secret
  address, Outlook/Microsoft 365 published calendar, Apple, Nextcloud, ...). The same
  meeting appearing in two sources is shown once.

## Setup

You need a Google Cloud OAuth client of type *Desktop app*. It is free and takes a few
minutes. Every user creates their own, so nothing is shared with anyone else.

1. Open <https://console.cloud.google.com/> and create a project (or pick one).
2. Go to **APIs & Services > Library**, search for **Google Calendar API** and click
   **Enable**.
3. Go to **APIs & Services > OAuth consent screen** (called *Google Auth Platform* in
   newer consoles). Choose **External**, fill in an app name and your email, and save.
4. Under **Audience** (or *Test users*), add your own Google account as a test user.
   To avoid Google expiring the sign-in every 7 days, set the publishing status to
   **In production**. For personal use Google shows an "unverified app" warning that
   you can accept.
5. Go to **Credentials > Create credentials > OAuth client ID**, choose **Desktop app**
   and create it. Copy the **Client ID** and **Client secret**.
6. In this mod's settings paste the client ID and secret.
7. Click the widget in the tray and choose **Sign in with Google**. Approve the
   read-only calendar access in your browser.

By default the mod reads the calendars ticked in Google Calendar. To pick specific
calendars, add their IDs (shown in each calendar's settings) to *Calendar IDs*.

To connect another Google account (for example a work account), open the popup and choose
**Add Google account**; sign in with the other account in the browser. The same OAuth client
works for every account. If your organisation restricts third-party apps, a Workspace admin
may need to allow the client. **Sign out ...** entries remove one account at a time.

## ICS feeds

Add secret calendar URLs under *ICS feed URLs*, one per item, as `https://...` or
`Label|https://...` (`webcal://` also works). ICS carries no accept/decline status, so by
default every timed feed event gets reminders; turn that off with *Remind for ICS feed
events*. Recurring events, exceptions, RECURRENCE-ID overrides and IANA or Windows time
zones are supported. Feed URLs are stored as plain text in Windhawk's settings, like any
other setting, so treat them as secrets.

## Privacy and security

- The mod requests only `calendar.events.readonly` and `calendar.calendarlist.readonly`.
- Sign-in uses the system browser, a loopback redirect on `127.0.0.1` and PKCE. The
  refresh token is encrypted with DPAPI for your Windows account.
- The client secret you paste is stored in Windhawk's settings, like any other mod
  setting. For a Desktop-app client it is not a confidential secret.
- Network traffic goes to Google over HTTPS and to the ICS feed URLs you configure (HTTPS
  only, no credentials in the URL; up to 4 redirects are followed, which may lead to other
  HTTPS hosts). Nothing is sent until you configure an OAuth client and sign in, or add an
  ICS URL. Meeting links are shown only if they match a strict allowlist: Google Meet,
  `zoom.us` and Microsoft Teams join URLs.
- Reminders use their own notification identity, so they appear as "Tray Agenda". The first time a
  reminder is about to be shown, the mod writes `HKCU\Software\Classes\AppUserModelId\TrayAgenda`
  (display name only), and Windows adds
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Notifications\Settings\TrayAgenda`. Nothing is
  written if no reminder is ever shown (for example with *Reminder notifications* off). Both keys are
  deleted when the mod is disabled or unloaded. Explorer exiting (sign-out, shutdown, restart, crash)
  does not run that path, so after a session with a reminder the keys stay until the mod next loads
  into Explorer (and for good if the mod or Windhawk is removed while the mod is not loaded). For the
  same reason, per-app notification choices made in Windows Settings are reset at every sign-in or
  Explorer restart; use the mod's own *Reminder notifications* setting instead.
- *Sign out ...* in the popup removes that account's stored token and revokes it at Google.

## Notes

- Windows 11 only. Reminders follow your Windows notification and Focus settings. The widget
  is shown on the primary taskbar only.
- Taskbar XAML-root and tray insertion strategy is adapted from Salyts' MIT-licensed
  Taskbar Fluent Media Player Windhawk mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enabled
  $description: Show the agenda widget in the Windows 11 system tray.
- google_client_id: ""
  $name: Google OAuth client ID
  $description: Client ID of your own Google Cloud "Desktop app" OAuth client (see the mod description).
- google_client_secret: ""
  $name: Google OAuth client secret
  $description: Client secret of the same OAuth client.
- calendar_ids: [""]
  $name: Calendar IDs (optional)
  $description: Calendars to show. Leave empty to use the calendars ticked in Google Calendar.
- position: "tray_before_clock"
  $name: Position
  $description: Where to inject the widget in the system tray.
  $options:
  - "taskbar_left_start": "Taskbar - Left of Start button"
  - "taskbar_right_start": "Taskbar - Right of Start button"
  - "taskbar_after_search_left": "Taskbar - Left of Search button"
  - "taskbar_after_search_right": "Taskbar - Right of Search button"
  - "taskbar_after_taskview_left": "Taskbar - Left of Task View button"
  - "taskbar_after_taskview_right": "Taskbar - Right of Task View button"
  - "taskbar_after_widgets_left": "Taskbar - Left of Widgets button"
  - "taskbar_after_widgets_right": "Taskbar - Right of Widgets button"
  - "tray_left": "Tray - Far left"
  - "tray_right": "Tray - Far right"
  - "tray_before_clock": "Tray - before clock"
  - "tray_after_clock": "Tray - after clock"
  - "tray_before_omni_left": "Tray - Left of Network/Volume button"
  - "tray_before_omni_right": "Tray - Right of Network/Volume button"
  - "tray_language_left": "Tray - Left of Language button"
  - "tray_language_right": "Tray - Right of Language button"
  - "tray_icons_left": "Tray - Left of Tray Icons"
  - "tray_icons_right": "Tray - Right of Tray Icons"
  - "tray_hidden_icons_left": "Tray - Left of Hidden icons button"
  - "tray_hidden_icons_right": "Tray - Right of Hidden icons button"
  - "tray_after_showdesktop_left": "Tray - Left of Show Desktop"
  - "tray_after_showdesktop_right": "Tray - Right of Show Desktop"
- ics_feeds: [""]
  $name: ICS feed URLs (optional)
  $description: Secret iCal/ICS URLs (Google, Outlook, ...), one per item, as "Label|https://..." or just the URL. Stored as plain text in Windhawk settings. Up to 8 feeds.
- ics_notifications: true
  $name: Remind for ICS feed events
  $description: ICS feeds carry no accept/decline status, so their timed events are treated as accepted for reminders.
- poll_seconds: 300
  $name: Refresh interval, seconds
  $description: How often Google Calendar and the ICS feeds are read. Values below 60 seconds are raised to 60.
- notifications_enabled: true
  $name: Reminder notifications
  $description: Show a native Windows notification before and at the start of accepted events.
- notify_lead_minutes: 10
  $name: Reminder lead time, minutes
  $description: How long before an event the first reminder is shown. It is also when the event takes over the widget.
- max_title_characters: 64
  $name: Maximum title characters
  $description: Event titles are shortened before XAML trimming is applied.
- show_location: true
  $name: Show location
  $description: Show the event location in the popup when available.
- use_24_hour_time: true
  $name: Use 24-hour time
  $description: Use 14:00 instead of 2:00 PM.
- display_when_empty: true
  $name: Display when empty/unavailable
  $description: Show a compact empty or sign-in message instead of hiding the widget.
- max_snapshot_age_seconds: 1800
  $name: Maximum data age, seconds
  $description: Calendar data older than this is shown as stale (for example when Google cannot be reached).
*/
// ==/WindhawkModSettings==

// Windows 11 only. Self-contained: the mod talks to the Google Calendar REST API
// itself (OAuth 2.0 loopback + PKCE, WinHTTP) from dedicated worker threads and
// never from the taskbar UI thread. No external helper process is required.
//
// Credit: taskbar XAML-root discovery, SystemTrayFrameGrid Grid/StackPanel
// insertion, NotificationCenterButton/ClockButton anchoring, Taskbar.dll symbol
// hooks, and taskbar lifecycle reinjection are adapted from the MIT-licensed
// Taskbar Fluent Media Player Windhawk mod by Salyts:
// https://github.com/Salyts/Taskbar-Fluent-Media-Player

#undef GetCurrentTime

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <bcrypt.h>
#include <objbase.h>
#include <dpapi.h>
#include <shellapi.h>
#include <winhttp.h>
#include <windhawk_utils.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
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

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Media;

namespace {

constexpr wchar_t kWidgetName[] = L"TrayAgenda_Widget";
constexpr wchar_t kTitleName[] = L"TrayAgenda_Title";
constexpr wchar_t kAccentName[] = L"TrayAgenda_Accent";
constexpr wchar_t kTimeName[] = L"TrayAgenda_Time";

constexpr int kUiRefreshSeconds = 1;
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
    std::wstring position = L"tray_before_clock";
    std::wstring google_client_id;
    std::wstring google_client_secret;
    std::vector<std::wstring> calendar_ids;
    std::vector<IcsFeedConfig> ics_feeds;
    bool ics_notifications = true;
    int poll_seconds = 300;
    bool notifications_enabled = true;
    int notify_lead_minutes = 10;
    int max_title_characters = 64;
    bool show_location = true;
    bool use_24_hour_time = true;
    bool display_when_empty = true;
    int max_snapshot_age_seconds = 1800;
};

struct AgendaEntry {
    std::wstring title;
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

enum class AgendaStatus {
    Unavailable,
    Empty,
    Event,
    Stale,
    Error,
};

struct AgendaSnapshot {
    bool validSnapshot = false;
    bool isV2 = false;
    AgendaStatus status = AgendaStatus::Unavailable;
    std::wstring title;
    std::wstring location;
    std::wstring source;
    std::wstring errorText;
    int64_t generatedUnix = 0;
    int64_t startUnix = 0;
    int64_t endUnix = 0;
    bool allDay = false;
    std::vector<AgendaEntry> agenda;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

std::mutex g_snapshotMutex;
AgendaSnapshot g_snapshot;
std::atomic<uint64_t> g_snapshotGeneration{0};
std::atomic<uint64_t> g_settingsGeneration{0};

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_workerStop{false};
HANDLE g_workerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_workerThread;

std::atomic<HWND> g_taskbarWnd{nullptr};
[[clang::no_destroy]] Button g_agendaGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_injectionParent{nullptr};
int g_agendaColumn = -1;

[[clang::no_destroy]] DispatcherTimer g_retryTimer{nullptr};
winrt::event_token g_retryTimerToken{};
bool g_retryTimerHasToken = false;
[[clang::no_destroy]] FrameworkElement g_retryRoot{nullptr};
int g_retryCount = 0;

[[clang::no_destroy]] DispatcherTimer g_uiTimer{nullptr};
winrt::event_token g_uiTimerToken{};
bool g_uiTimerHasToken = false;
winrt::event_token g_widgetClickToken{};
bool g_widgetClickHasToken = false;
winrt::event_token g_widgetEnterToken{};
winrt::event_token g_widgetExitToken{};
bool g_widgetHoverHasTokens = false;
[[clang::no_destroy]] Border g_trayVisual{nullptr};
bool g_trayHovered = false;
bool g_trayPopupOpen = false;
[[clang::no_destroy]] Popup g_agendaPopup{nullptr};
[[clang::no_destroy]] Button g_enterMeetingButton{nullptr};
winrt::event_token g_enterMeetingToken{};
bool g_enterMeetingHasToken = false;
[[clang::no_destroy]] Popup g_meetingSubmenu{nullptr};
winrt::event_token g_popupClosedToken{};
bool g_popupClosedHasToken = false;
struct RowActionBinding {
    Button button{nullptr};
    winrt::event_token token{};
    int64_t startUnix = 0;
    int64_t endUnix = 0;
    int64_t generatedUnix = 0;
    std::wstring title;
    std::wstring meetCode;
};
[[clang::no_destroy]] std::vector<RowActionBinding> g_rowActionBindings;

enum class RefreshUiState : int {
    None = 0,
    Pending = 1,
    NotReady = 2,
    TimedOut = 3,
};

std::atomic<int> g_refreshUiState{static_cast<int>(RefreshUiState::None)};
std::atomic<int64_t> g_refreshBaselineGenerated{0};
std::atomic<ULONGLONG> g_refreshDeadlineTick{0};

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);

CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
CTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

using WindowThreadProc = void (*)(void*);

struct WindowThreadRunResult {
    bool dispatched = false;
    bool callbackRan = false;
    bool callbackSucceeded = false;

    bool Succeeded() const {
        return dispatched && callbackRan && callbackSucceeded;
    }
};

void ApplySettingsWithRetry(FrameworkElement xamlRootContent, int retryCount = 0);
HWND FindCurrentProcessTaskbarWnd();
bool StopRetryTimer();
bool StopUiTimer();
bool CleanupTaskbarResources(HWND hWnd);
void UpdateAgendaWidgetFromSnapshot();

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
    s.position = ReadStringSetting(L"position", L"tray_before_clock");
    static constexpr std::wstring_view kPositions[] = {
        L"taskbar_left_start", L"taskbar_right_start",
        L"taskbar_after_search_left", L"taskbar_after_search_right",
        L"taskbar_after_taskview_left", L"taskbar_after_taskview_right",
        L"taskbar_after_widgets_left", L"taskbar_after_widgets_right",
        L"tray_left", L"tray_right", L"tray_before_clock", L"tray_after_clock",
        L"tray_before_omni_left", L"tray_before_omni_right",
        L"tray_language_left", L"tray_language_right",
        L"tray_icons_left", L"tray_icons_right",
        L"tray_hidden_icons_left", L"tray_hidden_icons_right",
        L"tray_after_showdesktop_left", L"tray_after_showdesktop_right",
    };
    if (std::find(std::begin(kPositions), std::end(kPositions), s.position) ==
        std::end(kPositions)) {
        s.position = L"tray_before_clock";
    }

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
    s.max_title_characters = std::clamp(Wh_GetIntSetting(L"max_title_characters"), 5, 200);
    s.show_location = Wh_GetIntSetting(L"show_location") != 0;
    s.use_24_hour_time = Wh_GetIntSetting(L"use_24_hour_time") != 0;
    s.display_when_empty = Wh_GetIntSetting(L"display_when_empty") != 0;
    s.max_snapshot_age_seconds = std::clamp(
        Wh_GetIntSetting(L"max_snapshot_age_seconds"), 60, 7 * 24 * 60 * 60);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
    g_settingsGeneration.fetch_add(1, std::memory_order_relaxed);
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

SolidColorBrush MakeBrush(winrt::Windows::UI::Color color) {
    SolidColorBrush brush;
    brush.Color(color);
    return brush;
}

winrt::Windows::UI::Color ThemeForegroundColor() {
    try {
        winrt::Windows::UI::ViewManagement::UISettings uiSettings;
        return uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Foreground);
    } catch (...) {
        return {255, 255, 255, 255};
    }
}

winrt::Windows::UI::Color Dimmed(winrt::Windows::UI::Color color, BYTE alpha) {
    color.A = alpha;
    return color;
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

    FILETIME local{};
    if (!FileTimeToLocalFileTime(&utc, &local)) {
        return false;
    }
    return FileTimeToSystemTime(&local, localTime) != FALSE;
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

PCWSTR MeetingJoinLabel(const std::wstring& provider) {
    if (provider == L"zoom") return L"Join Zoom meeting";
    if (provider == L"teams") return L"Join Microsoft Teams meeting";
    return L"Join Google Meet";
}

AgendaSnapshot MakeUnavailableSnapshot(const std::wstring& message) {
    AgendaSnapshot snapshot;
    snapshot.validSnapshot = false;
    snapshot.status = AgendaStatus::Unavailable;
    snapshot.errorText = SanitizeUiText(message, 256);
    return snapshot;
}

AgendaSnapshot SnapshotWithAgeLimit(AgendaSnapshot snapshot,
                                    const ModSettings& settings) {
    if (!snapshot.validSnapshot) {
        return snapshot;
    }

    int64_t now = NowUnix();
    int64_t maxAge = std::max(60, settings.max_snapshot_age_seconds);
    bool stale = snapshot.generatedUnix <= 0 || now <= 0;
    if (!stale) {
        if (snapshot.generatedUnix > now + maxAge || now - snapshot.generatedUnix > maxAge) {
            stale = true;
        }
    }

    if (stale) {
        snapshot.status = AgendaStatus::Stale;
        snapshot.allDay = false;
        if (snapshot.errorText.empty()) {
            snapshot.errorText = L"Agenda snapshot is stale";
        }
    }

    return snapshot;
}

void PublishSnapshot(AgendaSnapshot snapshot) {
    std::lock_guard<std::mutex> lock(g_snapshotMutex);
    g_snapshot = std::move(snapshot);
    g_snapshotGeneration.fetch_add(1, std::memory_order_relaxed);
}

AgendaSnapshot SnapshotCopy() {
    std::lock_guard<std::mutex> lock(g_snapshotMutex);
    return g_snapshot;
}

// ===========================================================================
// Native Google Calendar provider: OAuth (loopback + PKCE), WinHTTP client,
// Calendar REST ingestion, reminder toasts. Everything runs on worker threads;
// nothing here touches the taskbar UI thread.
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
constexpr wchar_t kToastAumid[] = L"TrayAgenda";
constexpr wchar_t kToastDisplayName[] = L"Tray Agenda";
constexpr wchar_t kToastRegisteredFlag[] = L"toast_registered";
constexpr size_t kMaxApiResponseBytes = 4 * 1024 * 1024;
constexpr size_t kMaxTokenResponseBytes = 64 * 1024;
constexpr int kMaxCalendars = 16;
constexpr int kMaxEventsPerCalendar = 256;
constexpr int kMaxEventsTotal = 512;
constexpr int kMaxEventPages = 8;
constexpr int kMaxAgendaItems = 20;
constexpr int64_t kFetchPastSeconds = kSecondsPerDay;
constexpr int64_t kFetchFutureSeconds = 2 * kSecondsPerDay;
constexpr int64_t kAgendaWindowSeconds = kSecondsPerDay;
constexpr int64_t kPreviewWindowSeconds = kSecondsPerHour;
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
    size_t chars = Wh_GetStringValue(kAccountsValueName, buffer, ARRAYSIZE(buffer));
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
    Wh_SetStringValue(kAccountsValueName, Utf8ToWide(data).c_str());
}

std::wstring AccountTokenValueName(const std::string& id) {
    return L"google_rt_" + Utf8ToWide(id);
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

    HINTERNET session = WinHttpOpen(L"TrayAgenda/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
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
        // stays listed so the popup can tell the user to sign in again.
        ClearRefreshToken(account.id);
        ClearAccessToken(account.id);
        SetAccountNote(account.id,
                       result.invalidGrant
                           ? L"Google sign-in expired. Use Add Google account to sign in again."
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
    std::string body = std::string("<!doctype html><meta charset=\"utf-8\"><title>Tray Agenda</title>"
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
            SendHtmlResponse(client.handle, 200, "OK", "Signed in to Tray Agenda");
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
    SetAuthNote(success ? std::wstring() : failure);
    if (!success) Wh_Log(L"sign-in failed (%s)", failure.c_str());
    g_signingIn = false;
    ReevaluateAuthState(SettingsCopy());
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
    if (comInitialized) CoUninitialize();
}

void StartSignIn() {
    std::lock_guard<std::mutex> lock(g_signInMutex);
    if (g_workerStop.load()) return;
    ModSettings settings = SettingsCopy();
    if (!ClientConfigured(settings)) {
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
    g_refreshUiState = static_cast<int>(RefreshUiState::None);
    SetAuthNote(L"");
    g_signingIn = true;
    ReevaluateAuthState(settings);
    g_signInThread.emplace(SignInThreadProc);
}

std::mutex g_revokeMutex;
std::vector<std::string> g_revokeTokens;

void SignOutAccount(const std::string& id) {
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
    if (LoadAccounts().empty()) PublishSnapshot(MakeUnavailableSnapshot(L"Signed out"));
    g_forceRefresh = true;
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
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
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &local, &utc)) return false;
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
            for (auto& e : one.events) total.events.push_back(std::move(e));
        } else {
            total.partial = true;
            if (firstFailure.failure == FetchFailure::None) firstFailure = one;
        }
    }
    total.events = DedupeEvents(std::move(total.events));
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

bool LocalCivilToUnix(int y, int mo, int d, int h, int mi, int s, int64_t* unix) {
    SYSTEMTIME local{};
    local.wYear = static_cast<WORD>(y);
    local.wMonth = static_cast<WORD>(mo);
    local.wDay = static_cast<WORD>(d);
    local.wHour = static_cast<WORD>(h);
    local.wMinute = static_cast<WORD>(mi);
    local.wSecond = static_cast<WORD>(s);
    SYSTEMTIME utc{};
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &local, &utc)) return false;
    FILETIME ft{};
    if (!SystemTimeToFileTime(&utc, &ft)) return false;
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    *unix = (static_cast<int64_t>(ticks.QuadPart) - kUnixToFileTimeTicks) / kFileTimeTicksPerSecond;
    return true;
}

// Windows time zone names ("W. Europe Standard Time"), as written by Outlook.
bool WindowsZoneToUnix(const std::string& name, const IcsTime& t, int64_t* unix) {
    std::wstring wide = Utf8ToWide(name);
    if (wide.empty() || wide.size() > 128) return false;
    for (wchar_t ch : wide) {
        if (ch == L'\\' || ch == L'/') return false;
    }
    std::wstring key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\" + wide;
    BYTE buffer[64];
    DWORD size = sizeof(buffer);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, key.c_str(), L"TZI", RRF_RT_REG_BINARY, nullptr, buffer,
                     &size) != ERROR_SUCCESS ||
        size != 44) {
        return false;
    }
    TIME_ZONE_INFORMATION tzi{};
    std::memcpy(&tzi.Bias, buffer, 4);
    std::memcpy(&tzi.StandardBias, buffer + 4, 4);
    std::memcpy(&tzi.DaylightBias, buffer + 8, 4);
    std::memcpy(&tzi.StandardDate, buffer + 12, 16);
    std::memcpy(&tzi.DaylightDate, buffer + 28, 16);
    SYSTEMTIME local{};
    local.wYear = static_cast<WORD>(t.y);
    local.wMonth = static_cast<WORD>(t.mo);
    local.wDay = static_cast<WORD>(t.d);
    local.wHour = static_cast<WORD>(t.h);
    local.wMinute = static_cast<WORD>(t.mi);
    local.wSecond = static_cast<WORD>(t.s);
    SYSTEMTIME utc{};
    FILETIME ft{};
    if (!TzSpecificLocalTimeToSystemTime(&tzi, &local, &utc) || !SystemTimeToFileTime(&utc, &ft)) {
        return false;
    }
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    *unix = (static_cast<int64_t>(ticks.QuadPart) - kUnixToFileTimeTicks) / kFileTimeTicksPerSecond;
    return true;
}

// IANA zone ids ("America/Sao_Paulo") via Windows.Globalization.Calendar. The offset is found
// by asking the calendar for the local wall time of a candidate instant.
[[clang::no_destroy]] std::optional<std::map<std::string, winrt::Windows::Globalization::Calendar>>
    g_zoneCalendars{std::in_place};

// Called on the worker thread before its WinRT apartment is torn down.
void ClearZoneCalendars() {
    g_zoneCalendars.reset();
}

bool IanaZoneToUnix(const std::string& tzid, const IcsTime& t, int64_t* unix) {
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
        const int64_t naive = CivilNaiveSeconds(t.y, t.mo, t.d, t.h, t.mi, t.s);
        int64_t guess = naive;
        for (int i = 0; i < 3; ++i) {
            int64_t ticks = (guess + 11644473600LL) * kFileTimeTicksPerSecond;
            cal.SetDateTime(winrt::Windows::Foundation::DateTime{
                winrt::Windows::Foundation::TimeSpan{ticks}});
            int64_t localNaive = CivilNaiveSeconds(cal.Year(), cal.Month(), cal.Day(), cal.Hour(),
                                                   cal.Minute(), cal.Second());
            int64_t next = naive - (localNaive - guess);
            if (next == guess) break;
            guess = next;
        }
        *unix = guess;
        return true;
    } catch (...) {
        return false;
    }
}

bool IcsTimeToUnix(const IcsTime& t, int64_t* unix) {
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
            if (IanaZoneToUnix(t.tzid, t, unix)) return true;
        } else if (WindowsZoneToUnix(t.tzid, t, unix) || IanaZoneToUnix(t.tzid, t, unix)) {
            return true;
        }
    }
    return LocalCivilToUnix(t.y, t.mo, t.d, t.h, t.mi, t.s, unix);  // floating or unknown zone
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
        }
    }
    if (out.freq != "DAILY" && out.freq != "WEEKLY" && out.freq != "MONTHLY" &&
        out.freq != "YEARLY") {
        return false;
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
    std::sort(days.begin(), days.end());
    days.erase(std::unique(days.begin(), days.end()), days.end());
    return days;
}

struct IcsEvent {
    std::string uid;
    std::wstring summary, location;
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
            if (rule.hasUntil && day * kSecondsPerDay + todOfDay > untilNaive) {
                finished = true;
                return;
            }
            if (rule.count > 0 && emitted >= rule.count) {
                finished = true;
                return;
            }
            ++emitted;
            if (day < winStart / kSecondsPerDay - 2 - durationDays) return;  // far before window
            IcsTime t = ev.start;
            CivilFromDays(day, &t.y, &t.mo, &t.d);
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
    int components = 0;
    for (const auto& line : lines) {
        if (line.empty()) continue;
        if (!inEvent) {
            if (UpperAscii(line) == "BEGIN:VEVENT") {
                inEvent = true;
                current = IcsEvent();
                if (++components > kMaxIcsComponents) break;
            }
            continue;
        }
        if (UpperAscii(line) == "END:VEVENT") {
            inEvent = false;
            if (current.start.valid) events.push_back(current);
            continue;
        }
        if (UpperAscii(line).rfind("BEGIN:", 0) == 0) continue;  // e.g. VALARM, ignore content
        IcsProp prop;
        if (!ParseIcsLine(line, &prop)) continue;
        if (prop.name == "UID") current.uid = prop.value.substr(0, 256);
        else if (prop.name == "SUMMARY") current.summary = IcsUnescape(prop.value);
        else if (prop.name == "LOCATION") {
            current.location = IcsUnescape(prop.value);
            current.linkValues.push_back(current.location);
        } else if (prop.name == "DESCRIPTION" || prop.name == "URL" || prop.name == "CONFERENCE" ||
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
            if (static_cast<int>(outcome.events.size()) >= kMaxEventsTotal) break;
            AgendaEntry entry;
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

// --- Agenda building and widget event selection ----------------------------------------

// Sort key: start, then regular events before out-of-office, then end/source/title.
bool TimedEntryLess(const AgendaEntry& a, const AgendaEntry& b) {
    if (a.startUnix != b.startUnix) return a.startUnix < b.startUnix;
    if (a.outOfOffice != b.outOfOffice) return !a.outOfOffice;
    if (a.endUnix != b.endUnix) return a.endUnix < b.endUnix;
    std::wstring sa = LowerCopy(a.source), sb = LowerCopy(b.source);
    if (sa != sb) return sa < sb;
    return LowerCopy(a.title) < LowerCopy(b.title);
}

bool AllDayEntryLess(const AgendaEntry& a, const AgendaEntry& b) {
    std::wstring sa = LowerCopy(a.source), sb = LowerCopy(b.source);
    if (sa != sb) return sa < sb;
    std::wstring ta = LowerCopy(a.title), tb = LowerCopy(b.title);
    if (ta != tb) return ta < tb;
    if (a.startUnix != b.startUnix) return a.startUnix < b.startUnix;
    return a.endUnix < b.endUnix;
}

// Out-of-office events never win over a regular event that overlaps them.
std::vector<AgendaEntry> PreferNonOverlappedOutOfOffice(std::vector<AgendaEntry> candidates) {
    std::vector<AgendaEntry> regular;
    for (const auto& e : candidates) {
        if (!e.outOfOffice) regular.push_back(e);
    }
    if (regular.empty()) return candidates;
    std::vector<AgendaEntry> kept;
    for (const auto& e : candidates) {
        bool overlapped = false;
        if (e.outOfOffice) {
            for (const auto& other : regular) {
                if (other.startUnix < e.endUnix && e.startUnix < other.endUnix) {
                    overlapped = true;
                    break;
                }
            }
        }
        if (!overlapped) kept.push_back(e);
    }
    return kept;
}

// What the tray widget (and the popup headline) show:
//  1. an event starting within `leadSeconds` takes over from the one in progress;
//  2. otherwise the most recently started event in progress;
//  3. otherwise the next event within the preview window.
bool SelectWidgetEntry(const std::vector<AgendaEntry>& agenda, int64_t now,
                       int64_t leadSeconds, AgendaEntry* out) {
    std::vector<AgendaEntry> active, soon, upcoming;
    for (const auto& e : agenda) {
        if (e.allDay || e.endUnix <= now) continue;
        if (e.responseState == AgendaEntry::ResponseState::Declined) continue;  // never headline
        if (e.startUnix <= now) active.push_back(e);
        else if (e.startUnix <= now + leadSeconds) soon.push_back(e);
        else if (e.startUnix <= now + kPreviewWindowSeconds) upcoming.push_back(e);
    }
    std::vector<AgendaEntry> pool = soon;
    pool.insert(pool.end(), active.begin(), active.end());
    pool = PreferNonOverlappedOutOfOffice(std::move(pool));
    std::vector<AgendaEntry> poolSoon, poolActive;
    for (const auto& e : pool) (e.startUnix <= now ? poolActive : poolSoon).push_back(e);

    if (!poolSoon.empty()) {
        *out = *std::min_element(poolSoon.begin(), poolSoon.end(), TimedEntryLess);
        return true;
    }
    if (!poolActive.empty()) {
        int64_t latest = 0;
        for (const auto& e : poolActive) latest = std::max(latest, e.startUnix);
        std::vector<AgendaEntry> newest;
        for (const auto& e : poolActive) {
            if (e.startUnix == latest) newest.push_back(e);
        }
        *out = *std::min_element(newest.begin(), newest.end(), TimedEntryLess);
        return true;
    }
    upcoming = PreferNonOverlappedOutOfOffice(std::move(upcoming));
    if (!upcoming.empty()) {
        *out = *std::min_element(upcoming.begin(), upcoming.end(), TimedEntryLess);
        return true;
    }
    return false;
}

std::vector<AgendaEntry> BuildAgenda(const std::vector<AgendaEntry>& events, int64_t now) {
    std::vector<AgendaEntry> allDay, timed;
    for (const auto& e : events) {
        if (e.endUnix <= now || e.startUnix >= now + kAgendaWindowSeconds) continue;
        (e.allDay ? allDay : timed).push_back(e);
    }
    std::sort(allDay.begin(), allDay.end(), AllDayEntryLess);
    std::sort(timed.begin(), timed.end(), TimedEntryLess);
    size_t timedReserve = std::min<size_t>(timed.size(), kMaxAgendaItems / 2);
    size_t allDayTake = std::min<size_t>(allDay.size(), kMaxAgendaItems - timedReserve);
    size_t timedTake = std::min<size_t>(timed.size(), kMaxAgendaItems - allDayTake);
    std::vector<AgendaEntry> agenda(allDay.begin(), allDay.begin() + allDayTake);
    agenda.insert(agenda.end(), timed.begin(), timed.begin() + timedTake);
    return agenda;
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
    size_t chars = Wh_GetStringValue(kNotifiedValueName, buffer, ARRAYSIZE(buffer));
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
    Wh_SetStringValue(kNotifiedValueName, Utf8ToWide(data).c_str());
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

// Writes HKCU\Software\Classes\AppUserModelId\TrayAgenda so Windows attributes the toast to
// "Tray Agenda". Called lazily, right before the first reminder is shown. The storage flag lets
// the next start clean up after an unclean exit, when the unload path never ran.
bool EnsureToastRegistration() {
    if (g_toastRegistered.load()) return true;
    Wh_SetIntValue(kToastRegisteredFlag, 1);
    std::wstring subKey = std::wstring(L"Software\\Classes\\AppUserModelId\\") + kToastAumid;
    LSTATUS status = RegSetKeyValueW(
        HKEY_CURRENT_USER, subKey.c_str(), L"DisplayName", REG_SZ, kToastDisplayName,
        static_cast<DWORD>((wcslen(kToastDisplayName) + 1) * sizeof(wchar_t)));
    if (status != ERROR_SUCCESS) return false;
    g_toastRegistered = true;
    return true;
}

// Removes the key above and the per-app notification settings Windows creates under
// HKCU\...\Notifications\Settings\TrayAgenda when the first toast is shown.
void RemoveToastRegistration() {
    RegDeleteTreeW(HKEY_CURRENT_USER,
                   (std::wstring(L"Software\\Classes\\AppUserModelId\\") + kToastAumid).c_str());
    RegDeleteTreeW(HKEY_CURRENT_USER,
                   (std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\"
                                 L"Settings\\") +
                    kToastAumid)
                       .c_str());
    Wh_DeleteValue(kToastRegisteredFlag);
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
    AgendaSnapshot current = SnapshotCopy();
    if (current.validSnapshot) {
        current.errorText = SanitizeUiText(message, 256);
        PublishSnapshot(std::move(current));
    } else {
        AgendaSnapshot snapshot;
        snapshot.status = AgendaStatus::Error;
        snapshot.errorText = SanitizeUiText(message, 256);
        PublishSnapshot(std::move(snapshot));
    }
}

void ProviderWorkerMain() {
    bool apartmentReady = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentReady = true;
    } catch (...) {
        LogCaughtException(L"worker apartment");
    }
    LoadNotifiedState();

    int64_t nextFetch = 0;
    std::vector<AgendaEntry> notifyEntries;

    while (!g_workerStop.load()) {
        ModSettings settings = SettingsCopy();
        ReevaluateAuthState(settings);
        RevokePendingToken();
        int64_t now = NowUnix();
        bool force = g_forceRefresh.exchange(false);

        if (!settings.enabled) {
            PublishSnapshot(MakeUnavailableSnapshot(L"Disabled"));
            notifyEntries.clear();
        } else if (GetAuthState() == AuthState::SignedIn) {
            if (force || now >= nextFetch || nextFetch == 0) {
                FetchOutcome outcome = FetchAllSources(settings, now);
                if (g_workerStop.load()) break;
                if (outcome.failure == FetchFailure::None) {
                    AgendaSnapshot snapshot;
                    snapshot.validSnapshot = true;
                    snapshot.isV2 = true;
                    snapshot.generatedUnix = now;
                    snapshot.agenda = BuildAgenda(outcome.events, now);
                    snapshot.status = snapshot.agenda.empty() ? AgendaStatus::Empty
                                                              : AgendaStatus::Event;
                    if (outcome.partial) snapshot.errorText = L"Some calendars unavailable";
                    PublishSnapshot(std::move(snapshot));
                    notifyEntries.clear();
                    for (const auto& e : outcome.events) {
                        if (!e.allDay) notifyEntries.push_back(e);
                    }
                    g_refreshUiState = static_cast<int>(RefreshUiState::None);
                    g_refreshDeadlineTick = 0;
                    nextFetch = now + settings.poll_seconds;
                } else {
                    PublishFetchFailure(outcome.failure == FetchFailure::Auth
                                            ? L"Google sign-in needed. Open the agenda."
                                            : outcome.message);
                    if (g_refreshUiState.load() == static_cast<int>(RefreshUiState::Pending)) {
                        g_refreshUiState = static_cast<int>(RefreshUiState::TimedOut);
                        g_refreshDeadlineTick = 0;
                    }
                    nextFetch = now + std::min(settings.poll_seconds, 60);
                }
            }
        } else {
            PublishSnapshot(MakeUnavailableSnapshot(L"Not signed in"));
            notifyEntries.clear();
            nextFetch = 0;
        }

        if (settings.enabled && settings.notifications_enabled) {
            RunNotifications(settings, notifyEntries, NowUnix());
        }

        if (g_refreshUiState.load() == static_cast<int>(RefreshUiState::Pending)) {
            ULONGLONG deadline = g_refreshDeadlineTick.load();
            if (deadline && GetTickCount64() >= deadline) {
                g_refreshUiState = static_cast<int>(RefreshUiState::TimedOut);
                g_refreshDeadlineTick = 0;
            }
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
        if (g_refreshUiState.load() == static_cast<int>(RefreshUiState::Pending)) {
            waitMs = std::min<DWORD>(waitMs, 1000);
        }
        WaitForSingleObject(g_workerWakeEvent, waitMs);
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

WindowThreadRunResult RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    static const UINT kMsg = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct Payload {
        WindowThreadProc proc;
        void* param;
        bool callbackRan = false;
        bool callbackSucceeded = false;
    };

    WindowThreadRunResult result;

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) {
        return result;
    }

    if (tid == GetCurrentThreadId()) {
        result.dispatched = true;
        result.callbackRan = true;
        try {
            proc(param);
            result.callbackSucceeded = true;
        } catch (...) {
            LogCaughtException(L"RunFromWindowThread direct callback");
        }
        return result;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                static const UINT kMessage = RegisterWindowMessage(
                    L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == kMessage) {
                    auto* payload = reinterpret_cast<Payload*>(cwp->lParam);
                    payload->callbackRan = true;
                    try {
                        payload->proc(payload->param);
                        payload->callbackSucceeded = true;
                    } catch (...) {
                        payload->callbackSucceeded = false;
                        LogCaughtException(L"RunFromWindowThread hook callback");
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, tid);

    if (!hook) {
        return result;
    }

    Payload payload{proc, param};
    result.dispatched = true;
    SendMessageW(hWnd, kMsg, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    result.callbackRan = payload.callbackRan;
    result.callbackSucceeded = payload.callbackSucceeded;
    return result;
}

FrameworkElement FindChildByName(FrameworkElement const& root,
                                 std::wstring_view name,
                                 int depth = 32) {
    if (!root || depth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement const& parent,
                                      const wchar_t* className,
                                      int depth = 32) {
    if (!parent || depth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (winrt::get_class_name(child) == className) {
            return child;
        }
        if (auto found = FindChildByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    auto taskbarFrame = FindChildByClassName(root, L"Taskbar.TaskbarFrame", 8);
    auto rootGrid = taskbarFrame ? FindChildByName(taskbarFrame, L"RootGrid") : nullptr;
    return rootGrid ? rootGrid.try_as<Grid>() : Grid{nullptr};
}

FrameworkElement FindElementInRepeater(FrameworkElement const& repeater,
                                       const wchar_t* const* names, int nameCount) {
    if (!repeater) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        for (int j = 0; j < nameCount; ++j)
            if (child.Name() == names[j]) return child;
        int nestedCount = VisualTreeHelper::GetChildrenCount(child);
        for (int k = 0; k < nestedCount; ++k) {
            auto nested = VisualTreeHelper::GetChild(child, k).try_as<FrameworkElement>();
            if (!nested) continue;
            for (int j = 0; j < nameCount; ++j)
                if (nested.Name() == names[j]) return nested;
        }
    }
    return nullptr;
}

FrameworkElement FindNthChildByClassName(FrameworkElement const& parent,
                                          const wchar_t* className, int wanted) {
    if (!parent || wanted < 0) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    int seen = 0;
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (winrt::get_class_name(child) == className && seen++ == wanted) return child;
        if (auto nested = FindNthChildByClassName(child, className, wanted - seen)) {
            return nested;
        }
    }
    return nullptr;
}

// Grid.Column belongs to the direct child of the layout panel. New Windows 11
// builds can use a horizontal StackPanel with the same SystemTrayFrameGrid name.
FrameworkElement FindTrayElement(FrameworkElement const& trayPanel,
                                 const wchar_t* name) {
    auto elem = FindChildByName(trayPanel, name);
    while (elem) {
        auto parent = VisualTreeHelper::GetParent(elem);
        if (parent == trayPanel) {
            return elem;
        }
        elem = parent.try_as<FrameworkElement>();
    }
    return nullptr;
}

int TrayInsertionSlot(Panel const& panel, FrameworkElement const& anchor,
                      bool after) {
    if (!anchor) {
        return -1;
    }

    if (panel.try_as<StackPanel>()) {
        uint32_t index = 0;
        if (panel.Children().IndexOf(anchor, index)) {
            return static_cast<int>(index) + (after ? 1 : 0);
        }
        return -1;
    }

    return Grid::GetColumn(anchor) + (after ? Grid::GetColumnSpan(anchor) : 0);
}

int TrayEndSlot(Panel const& panel) {
    if (auto grid = panel.try_as<Grid>()) {
        return static_cast<int>(grid.ColumnDefinitions().Size());
    }
    return static_cast<int>(panel.Children().Size());
}

struct InjectionTarget {
    Panel panel{nullptr};
    int insertSlot = 0;
    bool taskbarRoot = false;
    FrameworkElement anchor{nullptr};
    bool afterAnchor = false;
};

bool g_taskbarRootInjection = false;
[[clang::no_destroy]] Grid g_taskbarRootGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_taskbarAnchor{nullptr};
bool g_taskbarAfterAnchor = false;
winrt::event_token g_taskbarLayoutToken{};
bool g_taskbarLayoutHasToken = false;

InjectionTarget ResolveInjectionTarget(FrameworkElement const& root,
                                       std::wstring_view position) {
    const bool isTaskbarPosition = position == L"taskbar_left_start" ||
                                   position == L"taskbar_right_start" ||
                                   position == L"taskbar_after_search_left" ||
                                   position == L"taskbar_after_search_right" ||
                                   position == L"taskbar_after_taskview_left" ||
                                   position == L"taskbar_after_taskview_right" ||
                                   position == L"taskbar_after_widgets_left" ||
                                   position == L"taskbar_after_widgets_right";
    if (isTaskbarPosition) {
        auto rootGrid = FindTaskbarRootGrid(root);
        auto repeater = rootGrid ? FindChildByName(rootGrid, L"TaskbarFrameRepeater") : nullptr;
        FrameworkElement anchor{nullptr};
        bool after = false;
        static const wchar_t* kStartNames[] = {
            L"StartButton", L"StartMenuButton", L"StartMenuLaunchButton", L"LaunchListButton"};
        if (position == L"taskbar_left_start" || position == L"taskbar_right_start") {
            anchor = FindElementInRepeater(repeater, kStartNames, ARRAYSIZE(kStartNames));
            after = position == L"taskbar_right_start";
        } else if (position == L"taskbar_after_search_left" ||
                   position == L"taskbar_after_search_right") {
            anchor = FindChildByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
            after = position == L"taskbar_after_search_right";
        } else if (position == L"taskbar_after_taskview_left" ||
                   position == L"taskbar_after_taskview_right") {
            anchor = FindNthChildByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
            after = position == L"taskbar_after_taskview_right";
        } else {
            anchor = FindChildByName(repeater, L"AugmentedEntryPointButton");
            if (!anchor) anchor = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
            after = position == L"taskbar_after_widgets_right";
        }
        if (rootGrid && anchor) {
            return {rootGrid, -1, true, anchor, after};
        }
        Wh_Log(L"taskbar anchor unavailable for selected position; falling back");
        return {};
    }

    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    auto trayPanel = trayFrame ? trayFrame.try_as<Panel>() : Panel{nullptr};
    if (!trayPanel || (!trayPanel.try_as<Grid>() && !trayPanel.try_as<StackPanel>())) {
        return {};
    }

    int slot = -1;
    if (position == L"tray_left") return {trayPanel, 0};
    if (position == L"tray_right") return {trayPanel, TrayEndSlot(trayPanel)};

    const bool after = position == L"tray_after_clock" ||
                       position == L"tray_before_omni_right" ||
                       position == L"tray_language_right" ||
                       position == L"tray_icons_right" ||
                       position == L"tray_hidden_icons_right" ||
                       position == L"tray_after_showdesktop_right";
    const wchar_t* anchorName = L"NotificationCenterButton";
    if (position == L"tray_before_omni_left" || position == L"tray_before_omni_right")
        anchorName = L"ControlCenterButton";
    else if (position == L"tray_language_left" || position == L"tray_language_right")
        anchorName = L"NonActivatableStack";
    else if (position == L"tray_icons_left" || position == L"tray_icons_right")
        anchorName = L"NotificationAreaIcons";
    else if (position == L"tray_hidden_icons_left" || position == L"tray_hidden_icons_right")
        anchorName = L"NotifyIconStack";
    else if (position == L"tray_after_clock" || position == L"tray_after_showdesktop_left" ||
             position == L"tray_after_showdesktop_right")
        anchorName = L"ShowDesktopStack";
    auto anchor = FindTrayElement(trayPanel, anchorName);
    if (!anchor && (position == L"tray_before_clock" || position == L"tray_after_clock")) {
        anchor = FindTrayElement(trayPanel, L"ClockButton");
    }

    slot = TrayInsertionSlot(trayPanel, anchor, after);
    if (slot >= 0) {
        return {trayPanel, slot};
    }

    Wh_Log(L"tray anchor unavailable for selected position; falling back");
    return {};
}

int RemoveAgendaWidgetChildren(Panel const& panel) {
    if (!panel) {
        return -1;
    }

    int firstColumn = -1;
    for (int i = static_cast<int>(panel.Children().Size()) - 1; i >= 0; --i) {
        auto fe = panel.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kWidgetName) {
            if (firstColumn < 0) {
                firstColumn = Grid::GetColumn(fe);
            }
            try {
                panel.Children().RemoveAt(i);
            } catch (...) {
            }
        }
    }
    return firstColumn;
}

void UpdateTaskbarWidgetPosition() {
    if (!g_taskbarRootInjection || !g_taskbarRootGrid || !g_taskbarAnchor || !g_agendaGrid) {
        return;
    }
    try {
        auto point = g_taskbarAnchor.TransformToVisual(g_taskbarRootGrid).TransformPoint({0, 0});
        double x = g_taskbarAfterAnchor
                       ? point.X + g_taskbarAnchor.ActualWidth()
                       : point.X - g_agendaGrid.ActualWidth();
        auto margin = g_agendaGrid.Margin();
        if (std::abs(margin.Left - x) > 1.0) {
            g_agendaGrid.Margin({x, margin.Top, margin.Right, margin.Bottom});
        }
    } catch (...) {
        Wh_Log(L"taskbar anchor position update failed");
    }
}

TextBlock MakeTextBlock(PCWSTR name, double fontSize, bool bold) {
    TextBlock text;
    text.Name(name);
    text.FontFamily(FontFamily(L"Segoe UI Variable Text"));
    text.FontSize(fontSize);
    text.FontWeight(bold ? winrt::Windows::UI::Text::FontWeights::SemiBold()
                         : winrt::Windows::UI::Text::FontWeights::Normal());
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.TextWrapping(TextWrapping::NoWrap);
    text.MaxWidth(240);
    text.VerticalAlignment(VerticalAlignment::Center);
    return text;
}

void ShowAgendaFlyout();

// The tray button draws no state visuals of its own: hover/press/"popup open"
// are painted on the inner border by UpdateTrayVisual so they can be combined
// (a VisualStateManager group would reset the other group's setters).
Style MakeTrayButtonStyle() {
    const wchar_t* xaml =
        L"<Style TargetType=\"Button\" "
        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
        L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
        L"<Setter Property=\"Background\" Value=\"Transparent\"/>"
        L"<Setter Property=\"BorderThickness\" Value=\"0\"/>"
        L"<Setter Property=\"UseSystemFocusVisuals\" Value=\"False\"/>"
        L"<Setter Property=\"Template\"><Setter.Value>"
        L"<ControlTemplate TargetType=\"Button\">"
        L"<Border x:Name=\"Root\" Background=\"{TemplateBinding Background}\" "
        L"CornerRadius=\"4\" Padding=\"{TemplateBinding Padding}\">"
        L"<ContentPresenter Content=\"{TemplateBinding Content}\" "
        L"ContentTemplate=\"{TemplateBinding ContentTemplate}\" "
        L"HorizontalContentAlignment=\"{TemplateBinding HorizontalContentAlignment}\" "
        L"VerticalContentAlignment=\"{TemplateBinding VerticalContentAlignment}\"/>"
        L"</Border></ControlTemplate></Setter.Value></Setter></Style>";
    return winrt::Windows::UI::Xaml::Markup::XamlReader::Load(winrt::hstring(xaml)).as<Style>();
}

// Same look as the taskbar media player mod: subtle fill plus a 1px
// elevation border while hovered, pressed, or while the popup is open.
void UpdateTrayVisual() {
    if (!g_trayVisual) return;
    using Color = winrt::Windows::UI::Color;
    const bool active = g_trayHovered || g_trayPopupOpen;
    auto fg = ThemeForegroundColor();
    const bool dark = static_cast<int>(fg.R) + fg.G + fg.B > 420;
    const Color topColor = dark ? Color{0x28, 255, 255, 255} : Color{0x08, 0, 0, 0};
    const Color bottomColor = dark ? Color{0x0A, 255, 255, 255} : Color{0x10, 0, 0, 0};

    try {
        g_trayVisual.Background(MakeBrush(
            !active ? Color{0, 255, 255, 255}
                    : (dark ? Color{0x0F, 255, 255, 255} : Color{0x99, 255, 255, 255})));
    } catch (...) {
        LogCaughtException(L"tray hover fill");
    }

    // The 1px elevation border is set on its own so a gradient failure can
    // never take the fill with it; fall back to a flat stroke.
    try {
        if (!active) {
            g_trayVisual.BorderBrush(MakeBrush({0, 255, 255, 255}));
            return;
        }
        LinearGradientBrush gradient;
        gradient.MappingMode(BrushMappingMode::RelativeToBoundingBox);
        gradient.StartPoint({0.5, 0.0});
        gradient.EndPoint({0.5, 1.0});
        GradientStop top;
        top.Offset(0.0);
        top.Color(topColor);
        GradientStop bottom;
        bottom.Offset(1.0);
        bottom.Color(bottomColor);
        gradient.GradientStops().Append(top);
        gradient.GradientStops().Append(bottom);
        g_trayVisual.BorderBrush(gradient);
    } catch (...) {
        LogCaughtException(L"tray hover border");
        try {
            g_trayVisual.BorderBrush(MakeBrush(active ? topColor : Color{0, 255, 255, 255}));
        } catch (...) {
        }
    }
}

Button BuildAgendaWidget() {
    Button outer;
    outer.Name(kWidgetName);
    // Fill the taskbar height (up to 40 DIPs) so the rounded hover corners are
    // not clipped on compact taskbars.
    outer.VerticalAlignment(VerticalAlignment::Stretch);
    outer.HorizontalAlignment(HorizontalAlignment::Left);
    outer.MinWidth(0);
    outer.MaxWidth(300);
    outer.MaxHeight(40);
    outer.Margin({4, 0, 4, 0});
    outer.Padding({0, 0, 0, 0});
    try {
        outer.Style(MakeTrayButtonStyle());
    } catch (...) {
        LogCaughtException(L"tray button style");
        outer.BorderThickness({0, 0, 0, 0});
        outer.Background(MakeBrush({0, 0, 0, 0}));
    }
    outer.CornerRadius({4, 4, 4, 4});
    outer.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    outer.VerticalContentAlignment(VerticalAlignment::Stretch);
    g_widgetClickToken = outer.Click([](winrt::Windows::Foundation::IInspectable const&,
                                        RoutedEventArgs const&) {
        if (!g_unloading) {
            if (g_agendaPopup && g_agendaPopup.IsOpen()) {
                g_agendaPopup.IsOpen(false);
            } else {
                ShowAgendaFlyout();
            }
        }
    });
    g_widgetClickHasToken = true;

    Border border;
    border.Background(MakeBrush({0, 255, 255, 255}));
    border.BorderBrush(MakeBrush({0, 255, 255, 255}));
    border.Padding({7, 0, 5, 0});
    border.BorderThickness({1, 1, 1, 1});
    border.CornerRadius({4, 4, 4, 4});
    border.VerticalAlignment(VerticalAlignment::Stretch);
    try {
        BrushTransition transition;
        transition.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(83)));
        border.BackgroundTransition(transition);
    } catch (...) {
    }
    g_trayVisual = border;
    g_trayHovered = false;
    g_trayPopupOpen = false;
    g_widgetEnterToken = outer.PointerEntered(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            g_trayHovered = true;
            UpdateTrayVisual();
        });
    g_widgetExitToken = outer.PointerExited(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            g_trayHovered = false;
            UpdateTrayVisual();
        });
    g_widgetHoverHasTokens = true;

    Grid content;
    content.VerticalAlignment(VerticalAlignment::Center);
    content.ColumnDefinitions().Append(ColumnDefinition());
    content.ColumnDefinitions().GetAt(0).Width({1.0, GridUnitType::Auto});
    content.ColumnDefinitions().Append(ColumnDefinition());
    content.ColumnDefinitions().GetAt(1).Width({1.0, GridUnitType::Star});
    content.ColumnDefinitions().Append(ColumnDefinition());
    content.ColumnDefinitions().GetAt(2).Width({1.0, GridUnitType::Auto});

    Border marker;
    marker.Name(kAccentName);
    marker.Width(4);
    marker.Height(18);
    marker.CornerRadius({2, 2, 2, 2});
    marker.Margin({0, 0, 7, 0});
    marker.VerticalAlignment(VerticalAlignment::Center);
    marker.Background(MakeBrush(Dimmed(ThemeForegroundColor(), 105)));
    Grid::SetColumn(marker, 0);
    content.Children().Append(marker);

    auto title = MakeTextBlock(kTitleName, 12.0, true);
    title.MaxWidth(1000);
    title.HorizontalAlignment(HorizontalAlignment::Stretch);
    Grid::SetColumn(title, 1);
    content.Children().Append(title);

    auto time = MakeTextBlock(kTimeName, 10.5, true);
    time.MinWidth(52);
    time.MaxWidth(72);
    time.TextAlignment(TextAlignment::Right);
    time.TextTrimming(TextTrimming::Clip);
    time.Margin({8, 0, 0, 0});
    Grid::SetColumn(time, 2);
    content.Children().Append(time);
    border.Child(content);
    outer.Content(border);

    return outer;
}

void SetColumnVisible(bool visible) {
    if (!g_agendaGrid) {
        return;
    }

    try {
        g_agendaGrid.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
        g_agendaGrid.Opacity(visible ? 1.0 : 0.0);

        if (auto grid = g_injectionParent ? g_injectionParent.try_as<Grid>() : Grid{nullptr}) {
            if (g_agendaColumn >= 0 &&
                g_agendaColumn < static_cast<int>(grid.ColumnDefinitions().Size())) {
                auto col = grid.ColumnDefinitions().GetAt(g_agendaColumn);
                col.Width(visible ? GridLength{1.0, GridUnitType::Auto}
                                  : GridLength{0.0, GridUnitType::Pixel});
            }
        }
    } catch (...) {
    }
}

void SetText(PCWSTR name, const std::wstring& value,
             winrt::Windows::UI::Color color, bool visible = true) {
    if (!g_agendaGrid) {
        return;
    }

    try {
        auto fe = FindChildByName(g_agendaGrid, name);
        if (auto text = fe ? fe.try_as<TextBlock>() : TextBlock{nullptr}) {
            text.Text(winrt::hstring(value));
            text.Foreground(MakeBrush(color));
            text.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
        }
    } catch (...) {
    }
}

void SetAccentColor(winrt::Windows::UI::Color color) {
    if (!g_agendaGrid) return;
    try {
        auto fe = FindChildByName(g_agendaGrid, kAccentName);
        if (auto border = fe ? fe.try_as<Border>() : Border{nullptr}) {
            border.Background(MakeBrush(color));
        }
    } catch (...) {
    }
}

void AppendDetailPart(std::wstring* detail, const std::wstring& part) {
    if (!detail || part.empty()) {
        return;
    }
    if (!detail->empty()) {
        *detail += L" - ";
    }
    *detail += part;
}

std::wstring BuildEventDetail(const AgendaSnapshot& snapshot,
                              const ModSettings& settings) {
    std::wstring detail;
    if (settings.show_location) {
        AppendDetailPart(&detail, snapshot.location);
    }
    AppendDetailPart(&detail, snapshot.source);
    return detail;
}

winrt::Windows::UI::Color SourceAccentColor(
    std::wstring_view source, winrt::Windows::UI::Color foreground) {
    if (source.empty()) {
        return Dimmed(foreground, 105);
    }

    // Keep source identity stable without pretending the feed supplied a color.
    static constexpr winrt::Windows::UI::Color kPalette[] = {
        {255, 45, 133, 221}, {255, 0, 153, 129}, {255, 137, 92, 214},
        {255, 219, 148, 38}, {255, 224, 81, 109}, {255, 58, 155, 92},
    };
    uint32_t hash = 2166136261u;
    for (wchar_t ch : source) {
        hash ^= static_cast<uint32_t>(ch);
        hash *= 16777619u;
    }
    return kPalette[hash % ARRAYSIZE(kPalette)];
}

// Menu-style agenda popup (Notion Calendar tray menu look).
constexpr double kMenuWidth = 400;
constexpr double kMenuItemHeight = 30;
constexpr double kMenuTextInset = 48;
using UiColor = winrt::Windows::UI::Color;

struct MenuPalette {
    UiColor surface;
    UiColor border;
    UiColor text;
    UiColor muted;
    UiColor hover;
    UiColor marker;
};

MenuPalette GetMenuPalette() {
    auto fg = ThemeForegroundColor();
    bool dark = static_cast<int>(fg.R) + fg.G + fg.B > 420;
    if (dark) {
        return {{255, 32, 32, 32}, {255, 62, 62, 62}, {255, 240, 240, 240},
                {255, 152, 152, 152}, {22, 255, 255, 255}, {255, 150, 150, 150}};
    }
    return {{255, 249, 249, 249}, {255, 214, 214, 214}, {255, 28, 28, 28},
            {255, 108, 108, 108}, {16, 0, 0, 0}, {255, 130, 130, 130}};
}

TextBlock MenuText(const std::wstring& text, double size, UiColor color) {
    TextBlock block = MakeTextBlock(L"", size, false);
    block.Text(text);
    block.Foreground(MakeBrush(color));
    block.MaxWidth(std::numeric_limits<double>::infinity());
    return block;
}

void AddGridColumn(Grid const& grid, GridLength width) {
    ColumnDefinition column;
    column.Width(width);
    grid.ColumnDefinitions().Append(column);
}

std::wstring FormatDurationUntil(int64_t targetUnix) {
    int64_t now = NowUnix();
    int64_t seconds = targetUnix > now ? targetUnix - now : 0;
    int64_t minutes = (seconds + kSecondsPerMinute - 1) / kSecondsPerMinute;
    wchar_t buffer[64]{};
    if (minutes < 60) {
        std::swprintf(buffer, ARRAYSIZE(buffer), L"%lld min", static_cast<long long>(minutes));
    } else if (minutes < 24 * 60) {
        int64_t hours = minutes / 60;
        int64_t mins = minutes % 60;
        if (mins) {
            std::swprintf(buffer, ARRAYSIZE(buffer), L"%lld h %lld min",
                          static_cast<long long>(hours), static_cast<long long>(mins));
        } else {
            std::swprintf(buffer, ARRAYSIZE(buffer), L"%lld h", static_cast<long long>(hours));
        }
    } else {
        std::swprintf(buffer, ARRAYSIZE(buffer), L"%lld d",
                      static_cast<long long>((minutes + 24 * 60 - 1) / (24 * 60)));
    }
    return buffer;
}

std::wstring FormatDayHeading(int64_t unixSeconds) {
    static constexpr const wchar_t* kDays[] = {L"Sun", L"Mon", L"Tue", L"Wed",
                                               L"Thu", L"Fri", L"Sat"};
    static constexpr const wchar_t* kMonths[] = {L"Jan", L"Feb", L"Mar", L"Apr",
                                                 L"May", L"Jun", L"Jul", L"Aug",
                                                 L"Sep", L"Oct", L"Nov", L"Dec"};
    SYSTEMTIME st{};
    if (!UnixToLocalSystemTime(unixSeconds, &st)) return L"Later";
    wchar_t buffer[48]{};
    std::swprintf(buffer, ARRAYSIZE(buffer), L"%s %s %u", kDays[st.wDayOfWeek % 7],
                  kMonths[(st.wMonth + 11) % 12], static_cast<unsigned>(st.wDay));
    return buffer;
}

void ShowMeetingSubmenu(Button const& source, int64_t startUnix, int64_t endUnix,
                        int64_t generatedUnix, std::wstring title,
                        std::wstring meetCode);
void OpenMeetingBinding(int64_t startUnix, int64_t endUnix, int64_t generatedUnix,
                        const std::wstring& title, const std::wstring& meetCode);

FrameworkElement MakeFlyoutRow(const AgendaEntry& entry, const ModSettings& settings,
                               bool highlight, int64_t generatedUnix,
                               const MenuPalette& pal) {
    const std::wstring meetTarget = MeetingTarget(entry);
    const bool hasMeet = !meetTarget.empty();

    Border row;
    row.Height(kMenuItemHeight);
    row.Background(MakeBrush(highlight ? pal.hover : UiColor{0, 0, 0, 0}));

    Grid body;
    AddGridColumn(body, {kMenuTextInset, GridUnitType::Pixel});
    AddGridColumn(body, {1.0, GridUnitType::Star});
    AddGridColumn(body, {32.0, GridUnitType::Pixel});

    const bool outlined = entry.responseState == AgendaEntry::ResponseState::NeedsResponse ||
                          entry.responseState == AgendaEntry::ResponseState::Tentative;
    FrameworkElement marker{nullptr};
    if (outlined) {
        // Unconfirmed events get a dotted bar, like Notion Calendar.
        StackPanel dots;
        for (int i = 0; i < 4; ++i) {
            Border dot;
            dot.Width(3);
            dot.Height(3);
            dot.Margin({0, i ? 1.33 : 0, 0, 0});
            dot.CornerRadius({1, 1, 1, 1});
            dot.Background(MakeBrush(Dimmed(pal.marker, 110)));
            dots.Children().Append(dot);
        }
        marker = dots;
    } else {
        Border bar;
        bar.Width(3);
        bar.Height(16);
        bar.CornerRadius({1.5, 1.5, 1.5, 1.5});
        bar.Background(MakeBrush(pal.marker));
        marker = bar;
    }
    marker.Margin({27, 0, 0, 0});
    marker.HorizontalAlignment(HorizontalAlignment::Left);
    marker.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(marker, 0);
    body.Children().Append(marker);

    std::wstring time = entry.allDay ? std::wstring(L"All day")
                                     : FormatUnixTime(entry.startUnix, settings.use_24_hour_time);
    std::wstring line = time + L" \u00B7 " + LimitTitle(entry.title, 120);
    auto label = MenuText(line, 13, pal.text);
    Grid::SetColumn(label, 1);
    body.Children().Append(label);

    if (hasMeet) {
        auto chevron = MenuText(L"\uE76C", 10, pal.muted);
        chevron.FontFamily(FontFamily(L"Segoe Fluent Icons"));
        chevron.HorizontalAlignment(HorizontalAlignment::Center);
        Grid::SetColumn(chevron, 2);
        body.Children().Append(chevron);
    }
    row.Child(body);

    Button rowButton;
    rowButton.Padding({0, 0, 0, 0});
    rowButton.MinHeight(0);
    rowButton.MinWidth(0);
    rowButton.Margin({0, 0, 0, 0});
    rowButton.CornerRadius({0, 0, 0, 0});
    rowButton.BorderThickness({0, 0, 0, 0});
    rowButton.Background(MakeBrush({0, 0, 0, 0}));
    rowButton.HorizontalAlignment(HorizontalAlignment::Stretch);
    rowButton.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    rowButton.Content(row);
    if (!hasMeet) return rowButton;
    RowActionBinding binding;
    binding.button = rowButton;
    binding.startUnix = entry.startUnix;
    binding.endUnix = entry.endUnix;
    binding.generatedUnix = generatedUnix;
    binding.title = entry.title;
    binding.meetCode = meetTarget;
    binding.token = rowButton.Click(
        [binding](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
            ShowMeetingSubmenu(binding.button, binding.startUnix, binding.endUnix,
                               binding.generatedUnix, binding.title, binding.meetCode);
        });
    g_rowActionBindings.push_back(std::move(binding));
    return rowButton;
}

FrameworkElement MakeProviderBadge(const std::wstring& provider) {
    UiColor color{255, 0, 131, 45};
    PCWSTR letter = L"M";
    if (provider == L"zoom") {
        color = {255, 45, 140, 255};
        letter = L"Z";
    } else if (provider == L"teams") {
        color = {255, 80, 89, 201};
        letter = L"T";
    }
    Border badge;
    badge.Width(18);
    badge.Height(18);
    badge.CornerRadius({4, 4, 4, 4});
    badge.Background(MakeBrush(color));
    badge.VerticalAlignment(VerticalAlignment::Center);
    TextBlock glyph = MakeTextBlock(L"", 11, true);
    glyph.Text(letter);
    glyph.Foreground(MakeBrush({255, 255, 255, 255}));
    glyph.MaxWidth(std::numeric_limits<double>::infinity());
    glyph.HorizontalAlignment(HorizontalAlignment::Center);
    badge.Child(glyph);
    return badge;
}

// Plain menu item (no marker): text aligned with the event titles. With a
// provider, a colored badge sits in the marker column like Notion's app icon.
Button MakeMenuAction(const std::wstring& text, const MenuPalette& pal,
                      const std::wstring& provider = std::wstring()) {
    Button button;
    button.Padding({0, 0, 0, 0});
    button.MinHeight(0);
    button.MinWidth(0);
    button.Height(kMenuItemHeight);
    button.Margin({0, 0, 0, 0});
    button.CornerRadius({0, 0, 0, 0});
    button.BorderThickness({0, 0, 0, 0});
    button.Background(MakeBrush({0, 0, 0, 0}));
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    auto label = MenuText(text, 13, pal.text);
    label.Margin({kMenuTextInset, 0, 12, 0});
    if (provider.empty()) {
        button.Content(label);
        return button;
    }
    Grid content;
    auto badge = MakeProviderBadge(provider);
    badge.HorizontalAlignment(HorizontalAlignment::Left);
    badge.Margin({22, 0, 0, 0});
    content.Children().Append(badge);
    content.Children().Append(label);
    button.Content(content);
    return button;
}

void AppendFlyoutHeading(StackPanel const& panel, const std::wstring& text,
                         const MenuPalette& pal) {
    auto heading = MenuText(text, 12, pal.muted);
    heading.Margin({kMenuTextInset, 8, 12, 5});
    panel.Children().Append(heading);
}

Border MakeMenuSeparator(const MenuPalette& pal) {
    Border line;
    line.Height(1);
    line.Margin({2, 6, 2, 6});
    line.Background(MakeBrush(Dimmed(pal.muted, 70)));
    return line;
}

bool ShowAgendaPopupAtTray(Button const& trayWidget, Popup const& popup,
                            FrameworkElement const& popupContent) {
    if (!trayWidget || !popup || !popupContent) return false;
    try {
        HWND taskbar = g_taskbarWnd.load(std::memory_order_relaxed);
        if (!taskbar) return false;

        auto root = trayWidget.XamlRoot()
                        ? trayWidget.XamlRoot().Content().try_as<FrameworkElement>()
                        : FrameworkElement{nullptr};
        if (!root) return false;
        root.UpdateLayout();
        popupContent.Measure({kMenuWidth, 620});
        double popupHeight = popupContent.DesiredSize().Height;
        if (popupHeight <= 0) popupHeight = 620;

        // Popup offsets are DIPs in the taskbar XAML-root coordinate space.
        // Unlike FlyoutShowOptions::Position, these are explicit and do not
        // get reinterpreted as screen coordinates by the placement engine.
        auto widgetPoint = trayWidget.TransformToVisual(root).TransformPoint({0, 0});
        double rootWidth = root.ActualWidth();
        if (rootWidth <= 0) return false;

        HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{sizeof(monitorInfo)};
        if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) return false;
        RECT work = monitorInfo.rcWork;
        RECT taskbarRect{};
        if (!GetWindowRect(taskbar, &taskbarRect)) return false;
        bool bottomTaskbar = taskbarRect.top >= work.bottom - 128;

        constexpr double kPopupWidth = kMenuWidth;
        constexpr double kGap = 6;
        double x = widgetPoint.X + trayWidget.ActualWidth() - kPopupWidth;
        x = std::clamp(x, 0.0, std::max(0.0, rootWidth - kPopupWidth));
        double y = bottomTaskbar
                       ? widgetPoint.Y - popupHeight - kGap
                       : widgetPoint.Y + trayWidget.ActualHeight() + kGap;
        popup.HorizontalOffset(x);
        popup.VerticalOffset(y);
        popup.IsOpen(true);
        return true;
    } catch (...) {
        LogCaughtException(L"position agenda popup");
        return false;
    }
}

void RequestCalendarRefresh() {
    if (GetAuthState() != AuthState::SignedIn) {
        g_refreshUiState = static_cast<int>(RefreshUiState::NotReady);
        g_refreshDeadlineTick = 0;
        UpdateAgendaWidgetFromSnapshot();
        return;
    }
    g_refreshBaselineGenerated = SnapshotCopy().generatedUnix;
    g_refreshDeadlineTick = GetTickCount64() + 20000;
    g_refreshUiState = static_cast<int>(RefreshUiState::Pending);
    g_forceRefresh = true;
    if (g_workerWakeEvent) SetEvent(g_workerWakeEvent);
    UpdateAgendaWidgetFromSnapshot();
}

void SetTrayButtonOpenState(bool open) {
    g_trayPopupOpen = open;
    UpdateTrayVisual();
}

void RevokePopupActionHandlers() {
    SetTrayButtonOpenState(false);
    try {
        if (g_meetingSubmenu) g_meetingSubmenu.IsOpen(false);
    } catch (...) {
        LogCaughtException(L"close meeting submenu");
    }
    g_meetingSubmenu = nullptr;

    for (auto& binding : g_rowActionBindings) {
        try {
            if (binding.button) binding.button.Click(binding.token);
        } catch (...) {
            LogCaughtException(L"revoke agenda row handler");
        }
    }
    g_rowActionBindings.clear();


    try {
        if (g_enterMeetingButton && g_enterMeetingHasToken) {
            g_enterMeetingButton.Click(g_enterMeetingToken);
        }
    } catch (...) {
        LogCaughtException(L"revoke meeting handler");
    }
    g_enterMeetingToken = {};
    g_enterMeetingHasToken = false;
    g_enterMeetingButton = nullptr;
}

void CloseAgendaPopup() {
    RevokePopupActionHandlers();
    try {
        if (g_agendaPopup) {
            g_agendaPopup.IsOpen(false);
            if (g_popupClosedHasToken) g_agendaPopup.Closed(g_popupClosedToken);
            g_agendaPopup.Child(nullptr);
        }
    } catch (...) {
        LogCaughtException(L"close agenda popup");
    }
    g_popupClosedToken = {};
    g_popupClosedHasToken = false;
    g_agendaPopup = nullptr;
}

bool FindMatchingMeetingEntry(const AgendaSnapshot& snapshot, int64_t now,
                              int64_t generatedUnix, int64_t startUnix,
                              int64_t endUnix, const std::wstring& title,
                              const std::wstring& meetCode) {
    if (!snapshot.validSnapshot ||
        (snapshot.status != AgendaStatus::Event && snapshot.status != AgendaStatus::Empty) ||
        snapshot.generatedUnix != generatedUnix || now <= 0 || endUnix <= now ||
        MeetingProviderOfTarget(meetCode).empty()) return false;
    for (const auto& entry : snapshot.agenda) {
        if (entry.startUnix == startUnix && entry.endUnix == endUnix &&
            entry.title == title && MeetingTarget(entry) == meetCode && entry.endUnix > now) {
            return true;
        }
    }
    return false;
}

void OpenMeetingBinding(int64_t startUnix, int64_t endUnix, int64_t generatedUnix,
                        const std::wstring& title, const std::wstring& meetCode) {
    if (g_unloading) return;
    try {
        ModSettings settings = SettingsCopy();
        AgendaSnapshot snapshot = SnapshotWithAgeLimit(SnapshotCopy(), settings);
        if (!FindMatchingMeetingEntry(snapshot, NowUnix(), generatedUnix, startUnix,
                                      endUnix, title, meetCode)) {
            return;
        }
        std::wstring url = MeetingJoinUrl(meetCode);
        if (url.empty()) return;
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        if (g_agendaPopup) g_agendaPopup.IsOpen(false);
    } catch (...) {
        LogCaughtException(L"open meeting");
    }
}

void ShowMeetingSubmenu(Button const& source, int64_t startUnix, int64_t endUnix,
                        int64_t generatedUnix, std::wstring title,
                        std::wstring meetCode) {
    if (g_unloading || !source || MeetingProviderOfTarget(meetCode).empty()) return;
    try {
        AgendaSnapshot snapshot = SnapshotWithAgeLimit(SnapshotCopy(), SettingsCopy());
        if (!FindMatchingMeetingEntry(snapshot, NowUnix(), generatedUnix, startUnix,
                                      endUnix, title, meetCode)) return;
        if (g_meetingSubmenu) g_meetingSubmenu.IsOpen(false);
        g_meetingSubmenu = nullptr;
        if (g_enterMeetingButton && g_enterMeetingHasToken)
            g_enterMeetingButton.Click(g_enterMeetingToken);
        g_enterMeetingButton = nullptr;
        g_enterMeetingHasToken = false;

        Button enter;
        const MenuPalette pal = GetMenuPalette();
        const std::wstring provider = MeetingProviderOfTarget(meetCode);
        StackPanel enterContent;
        enterContent.Orientation(Orientation::Horizontal);
        auto enterBadge = MakeProviderBadge(provider);
        enterBadge.Margin({0, 0, 10, 0});
        enterContent.Children().Append(enterBadge);
        enterContent.Children().Append(MenuText(MeetingJoinLabel(provider), 13, pal.text));
        enter.Content(enterContent);
        enter.Padding({16, 6, 16, 6});
        enter.MinWidth(220);
        enter.MinHeight(0);
        enter.CornerRadius({0, 0, 0, 0});
        enter.HorizontalAlignment(HorizontalAlignment::Stretch);
        enter.HorizontalContentAlignment(HorizontalAlignment::Left);
        enter.BorderThickness({0, 0, 0, 0});
        enter.Background(MakeBrush({0, 0, 0, 0}));
        g_enterMeetingToken = enter.Click(
            [startUnix, endUnix, generatedUnix, title, meetCode](
                winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
                OpenMeetingBinding(startUnix, endUnix, generatedUnix, title, meetCode);
            });
        g_enterMeetingHasToken = true;
        g_enterMeetingButton = enter;
        Border surface;
        surface.Padding({0, 4, 0, 4});
        surface.Background(MakeBrush(pal.surface));
        surface.BorderBrush(MakeBrush(pal.border));
        surface.BorderThickness({1, 1, 1, 1});
        surface.CornerRadius({8, 8, 8, 8});
        surface.Child(enter);

        // Position next to the agenda popup in the taskbar XAML-root space
        // (same space ShowAgendaPopupAtTray uses); a Flyout anchored to a
        // popup child lands at the bottom of the taskbar instead.
        if (!g_agendaPopup) return;
        auto popupChild = g_agendaPopup.Child().try_as<UIElement>();
        if (!popupChild) return;
        surface.Measure({1000, 1000});
        double subWidth = surface.DesiredSize().Width;
        // Popup offsets live in the taskbar root space (negative Y when the
        // popup opens above the taskbar), so anchor on the popup's own offset
        // plus the row's position inside the popup content.
        auto rowPoint = source.TransformToVisual(popupChild).TransformPoint({0, 0});
        double mainX = g_agendaPopup.HorizontalOffset();
        double x = mainX - subWidth - 4;
        if (x < 0) x = mainX + kMenuWidth + 4;
        double y = g_agendaPopup.VerticalOffset() + rowPoint.Y;

        g_meetingSubmenu = Popup();
        g_meetingSubmenu.XamlRoot(source.XamlRoot());
        g_meetingSubmenu.ShouldConstrainToRootBounds(false);
        g_meetingSubmenu.IsLightDismissEnabled(false);
        g_meetingSubmenu.Child(surface);
        g_meetingSubmenu.HorizontalOffset(x);
        g_meetingSubmenu.VerticalOffset(y);
        g_meetingSubmenu.IsOpen(true);
    } catch (...) {
        LogCaughtException(L"show meeting submenu");
    }
}

void ShowAgendaFlyout() {
    if (g_unloading || !g_agendaGrid) return;
    try {
        CloseAgendaPopup();
        ModSettings settings = SettingsCopy();
        AgendaSnapshot snapshot = SnapshotWithAgeLimit(SnapshotCopy(), settings);
        int64_t now = NowUnix();
        int64_t horizon = now > 0 ? now + kSecondsPerDay : 0;
        std::vector<AgendaEntry> allDay;
        std::vector<AgendaEntry> timed;
        for (const auto& entry : snapshot.agenda) {
            if (entry.endUnix <= now || (horizon > 0 && entry.startUnix >= horizon)) continue;
            if (entry.allDay) allDay.push_back(entry); else timed.push_back(entry);
        }
        std::sort(allDay.begin(), allDay.end(), [](const AgendaEntry& a, const AgendaEntry& b) {
            return a.startUnix < b.startUnix;
        });
        std::sort(timed.begin(), timed.end(), TimedEntryLess);

        const int todayKey = LocalDateKey(now);
        const int tomorrowKey = LocalDateKey(now + kSecondsPerDay);

        const MenuPalette pal = GetMenuPalette();
        auto dayKeyOf = [&](const AgendaEntry& entry) {
            return entry.startUnix < now ? todayKey : LocalDateKey(entry.startUnix);
        };

        // The next/current timed event leads the menu, like Notion's "Upcoming in ..."
        // section; everything else is grouped by day below it.
        AgendaEntry headline;
        bool hasHeadline = !timed.empty();
        if (hasHeadline && !SelectWidgetEntry(
                               timed, now,
                               static_cast<int64_t>(settings.notify_lead_minutes) *
                                   kSecondsPerMinute,
                               &headline)) {
            auto firstKept = std::find_if(timed.begin(), timed.end(), [](const AgendaEntry& e) {
                return e.responseState != AgendaEntry::ResponseState::Declined;
            });
            if (firstKept != timed.end()) {
                headline = *firstKept;
            } else {
                hasHeadline = false;
            }
        }

        std::vector<AgendaEntry> rest;
        bool skipped = false;
        for (const auto& entry : allDay) rest.push_back(entry);
        for (const auto& entry : timed) {
            if (hasHeadline && !skipped && entry.startUnix == headline.startUnix &&
                entry.endUnix == headline.endUnix && entry.title == headline.title) {
                skipped = true;
                continue;
            }
            rest.push_back(entry);
        }
        std::stable_sort(rest.begin(), rest.end(), [&](const AgendaEntry& a, const AgendaEntry& b) {
            int ka = dayKeyOf(a), kb = dayKeyOf(b);
            if (ka != kb) return ka < kb;
            if (a.allDay != b.allDay) return a.allDay;
            return a.startUnix < b.startUnix;
        });

        StackPanel list;
        list.Padding({0, 4, 0, 4});

        const AuthState authState = GetAuthState();
        const bool signedIn = authState == AuthState::SignedIn;
        if (!signedIn) {
            hasHeadline = false;
            rest.clear();
            allDay.clear();
            timed.clear();
            const std::wstring note = GetAuthNote();
            std::wstring message;
            if (authState == AuthState::NoClient) {
                message = L"Add your Google client ID and secret (or an ICS feed URL) in the mod "
                          L"settings to connect your calendar.";
            } else if (authState == AuthState::SigningIn) {
                message = L"Finish signing in with Google in your browser.";
            } else {
                message = note.empty() ? std::wstring(L"Sign in with Google to see your agenda.")
                                       : note;
            }
            auto text = MenuText(message, 13, pal.muted);
            text.TextWrapping(TextWrapping::Wrap);
            text.Margin({kMenuTextInset, 10, 12, 10});
            list.Children().Append(text);
        }

        if (hasHeadline) {
            bool active = headline.startUnix <= now && now < headline.endUnix;
            AppendFlyoutHeading(list,
                                active ? std::wstring(L"Happening now")
                                       : L"Upcoming in " + FormatDurationUntil(headline.startUnix),
                                pal);
            list.Children().Append(MakeFlyoutRow(headline, settings, false,
                                                 snapshot.generatedUnix, pal));
            const std::wstring headlineTarget = MeetingTarget(headline);
            if (!headlineTarget.empty()) {
                const std::wstring headlineProvider = MeetingProviderOfTarget(headlineTarget);
                Button join = MakeMenuAction(MeetingJoinLabel(headlineProvider), pal,
                                             headlineProvider);
                RowActionBinding binding;
                binding.button = join;
                binding.startUnix = headline.startUnix;
                binding.endUnix = headline.endUnix;
                binding.generatedUnix = snapshot.generatedUnix;
                binding.title = headline.title;
                binding.meetCode = headlineTarget;
                binding.token = join.Click(
                    [binding](winrt::Windows::Foundation::IInspectable const&,
                              RoutedEventArgs const&) {
                        OpenMeetingBinding(binding.startUnix, binding.endUnix,
                                           binding.generatedUnix, binding.title,
                                           binding.meetCode);
                    });
                g_rowActionBindings.push_back(std::move(binding));
                list.Children().Append(join);
            }
            if (settings.show_location && !headline.location.empty()) {
                auto location = MenuText(SanitizeUiText(headline.location, 120), 12, pal.muted);
                location.Margin({kMenuTextInset, 6, 12, 6});
                list.Children().Append(location);
            }
        }

        int lastKey = 0;
        for (const auto& entry : rest) {
            int key = dayKeyOf(entry);
            if (key != lastKey) {
                lastKey = key;
                std::wstring label = key == todayKey      ? std::wstring(L"Today")
                                     : key == tomorrowKey ? std::wstring(L"Tomorrow")
                                                          : FormatDayHeading(entry.startUnix);
                AppendFlyoutHeading(list, label, pal);
            }
            list.Children().Append(MakeFlyoutRow(entry, settings, false,
                                                 snapshot.generatedUnix, pal));
        }
        if (signedIn && allDay.empty() && timed.empty()) {
            auto empty = MenuText(L"No events in the next 24 hours", 13, pal.muted);
            empty.Margin({kMenuTextInset, 10, 12, 10});
            list.Children().Append(empty);
        }
        if (signedIn) {
            std::vector<std::wstring> notes = AccountNoteLines();
            const std::wstring general = GetAuthNote();
            if (!general.empty()) notes.push_back(general);
            for (const auto& line : notes) {
                auto note = MenuText(SanitizeUiText(line, 200), 12, pal.muted);
                note.TextWrapping(TextWrapping::Wrap);
                note.Margin({kMenuTextInset, 8, 12, 4});
                list.Children().Append(note);
            }
        }
        if (signedIn && (snapshot.status == AgendaStatus::Stale ||
                         snapshot.status == AgendaStatus::Error ||
                         snapshot.status == AgendaStatus::Unavailable)) {
            auto stale = MenuText(L"Calendar data may be out of date.", 12, pal.muted);
            stale.Margin({kMenuTextInset, 8, 12, 4});
            list.Children().Append(stale);
        }

        ScrollViewer scroller;
        scroller.VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
        scroller.Content(list);

        StackPanel footer;
        footer.Padding({0, 0, 0, 6});
        footer.Children().Append(MakeMenuSeparator(pal));
        auto addFooterAction = [&](const std::wstring& text, std::function<void()> handler) {
            Button button = MakeMenuAction(text, pal);
            RowActionBinding binding;
            binding.button = button;
            binding.token = button.Click(
                [handler](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
                    handler();
                });
            g_rowActionBindings.push_back(std::move(binding));
            footer.Children().Append(button);
        };
        const std::vector<GoogleAccount> accounts = LoadAccounts();
        if (signedIn) {
            addFooterAction(L"Refresh calendar", [] { RequestCalendarRefresh(); });
        }
        if (g_signingIn.load()) {
            addFooterAction(L"Cancel sign-in", [] {
                g_signInCancel = true;
                CloseAgendaPopup();
            });
        } else if (ClientConfigured(settings) && accounts.size() < kMaxAccounts) {
            addFooterAction(accounts.empty() ? L"Sign in with Google" : L"Add Google account", [] {
                CloseAgendaPopup();
                StartSignIn();
                UpdateAgendaWidgetFromSnapshot();
            });
        }
        for (const auto& account : accounts) {
            std::wstring who = account.label;
            if (who.size() > 36) who = who.substr(0, 35) + L"\u2026";
            const std::string id = account.id;
            addFooterAction(L"Sign out " + who, [id] {
                CloseAgendaPopup();
                SignOutAccount(id);
                UpdateAgendaWidgetFromSnapshot();
            });
        }

        Grid shellBody;
        RowDefinition listRow;
        listRow.Height({1.0, GridUnitType::Star});
        RowDefinition footerRow;
        footerRow.Height({1.0, GridUnitType::Auto});
        shellBody.RowDefinitions().Append(listRow);
        shellBody.RowDefinitions().Append(footerRow);
        Grid::SetRow(scroller, 0);
        Grid::SetRow(footer, 1);
        shellBody.Children().Append(scroller);
        shellBody.Children().Append(footer);

        Border shell;
        shell.Width(kMenuWidth);
        shell.MaxHeight(620);
        shell.CornerRadius({8, 8, 8, 8});
        shell.Background(MakeBrush(pal.surface));
        shell.BorderBrush(MakeBrush(pal.border));
        shell.BorderThickness({1, 1, 1, 1});
        shell.Child(shellBody);

        g_agendaPopup = Popup();
        g_agendaPopup.XamlRoot(g_agendaGrid.XamlRoot());
        g_agendaPopup.ShouldConstrainToRootBounds(false);
        g_agendaPopup.IsLightDismissEnabled(true);
        g_popupClosedToken = g_agendaPopup.Closed(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::Foundation::IInspectable const&) {
                RevokePopupActionHandlers();
            });
        g_popupClosedHasToken = true;
        g_agendaPopup.Child(shell);
        if (ShowAgendaPopupAtTray(g_agendaGrid, g_agendaPopup, shell)) {
            SetTrayButtonOpenState(true);
        } else {
            // A transient taskbar rebuild can briefly hide the XAML root. Keep
            // the popup closed rather than opening it in an unknown coordinate
            // space; the next click can retry with the rebuilt root.
            g_agendaPopup.IsOpen(false);
        }
    } catch (...) {
        LogCaughtException(L"show flyout");
    }
}

void UpdateAgendaWidgetFromSnapshot() {
    if (g_unloading || !g_agendaGrid) {
        return;
    }

    ModSettings settings = SettingsCopy();
    AgendaSnapshot snapshot = SnapshotWithAgeLimit(SnapshotCopy(), settings);

    if (!settings.enabled) {
        SetColumnVisible(false);
        return;
    }

    std::wstring title;
    std::wstring detail;
    std::wstring compactSource;
    std::wstring compactTime = L"--";
    bool visible = true;
    bool accentGlyph = true;
    bool hasAgendaEntries = snapshot.isV2 && !snapshot.agenda.empty();

    switch (snapshot.status) {
        case AgendaStatus::Event: {
            int64_t now = NowUnix();
            AgendaEntry headline;
            bool hasHeadline = SelectWidgetEntry(
                snapshot.agenda, now,
                static_cast<int64_t>(settings.notify_lead_minutes) * kSecondsPerMinute, &headline);
            bool active = hasHeadline && headline.startUnix <= now;
            if (!hasHeadline) {
                accentGlyph = false;
                title = L"No upcoming meeting";
                detail = L"Open agenda";
                break;
            }
            std::wstring subject = LimitTitle(headline.title, settings.max_title_characters);
            compactTime = active ? L"now" : FormatRelativeToNow(headline.startUnix);
            title = subject;
            AgendaSnapshot headlineSnapshot = snapshot;
            headlineSnapshot.location = headline.location;
            headlineSnapshot.source = headline.source;
            compactSource = headline.source;
            detail = BuildEventDetail(headlineSnapshot, settings);
            if (!snapshot.errorText.empty()) {
                AppendDetailPart(&detail, L"Some calendars unavailable");
            }
            break;
        }
        case AgendaStatus::Empty:
            if (!settings.display_when_empty && snapshot.errorText.empty() && !hasAgendaEntries) {
                visible = false;
            }
            accentGlyph = false;
            title = snapshot.errorText.empty() ? L"No upcoming meeting"
                                               : L"Calendar data incomplete";
            detail = snapshot.source.empty() ? L"Open agenda" : snapshot.source;
            if (!snapshot.errorText.empty()) {
                AppendDetailPart(&detail, L"Some calendars unavailable");
            }
            break;
        case AgendaStatus::Stale:
            if (!settings.display_when_empty && !hasAgendaEntries) {
                visible = false;
            }
            accentGlyph = false;
            title = L"Calendar data is stale";
            detail = snapshot.source.empty() ? L"Could not refresh from Google" : snapshot.source;
            break;
        case AgendaStatus::Error:
            if (!settings.display_when_empty && !hasAgendaEntries) {
                visible = false;
            }
            accentGlyph = false;
            title = snapshot.errorText.empty() ? L"Calendar error" : snapshot.errorText;
            detail = snapshot.source.empty() ? L"Check Google Calendar access" : snapshot.source;
            break;
        case AgendaStatus::Unavailable:
        default:
            if (!settings.display_when_empty && !hasAgendaEntries) {
                visible = false;
            }
            accentGlyph = false;
            title = snapshot.errorText.empty() ? L"Calendar unavailable" : snapshot.errorText;
            detail = L"Waiting for first sync";
            break;
    }

    AuthState authState = GetAuthState();
    if (authState != AuthState::SignedIn) {
        visible = true;
        accentGlyph = false;
        compactSource.clear();
        compactTime = L"--";
        std::wstring note = GetAuthNote();
        if (authState == AuthState::NoClient) {
            title = L"Google client not set";
            detail = L"Open the mod settings";
        } else if (authState == AuthState::SigningIn) {
            title = L"Waiting for Google sign-in";
            detail = L"Finish in your browser";
            compactTime = L"...";
        } else {
            title = L"Sign in to Google Calendar";
            detail = note.empty() ? std::wstring(L"Open agenda to sign in") : note;
        }
    }

    RefreshUiState refreshState = static_cast<RefreshUiState>(
        g_refreshUiState.load(std::memory_order_relaxed));
    if (refreshState == RefreshUiState::Pending && authState == AuthState::SignedIn) {
        visible = true;
        accentGlyph = false;
        compactSource.clear();
        title = L"Refreshing calendar";
        detail = L"Fetching from Google";
        compactTime = L"...";
    } else if (refreshState == RefreshUiState::NotReady) {
        visible = true;
        accentGlyph = false;
        compactSource.clear();
        title = L"Not signed in";
        detail = L"Open agenda to sign in";
        compactTime = L"--";
    } else if (refreshState == RefreshUiState::TimedOut) {
        visible = true;
        accentGlyph = false;
        compactSource.clear();
        title = L"Refresh failed";
        detail = L"Could not reach Google Calendar";
        compactTime = L"--";
    }

    title = LimitTitle(title, std::max(settings.max_title_characters + 24,
                                      settings.max_title_characters));
    detail = SanitizeUiText(std::move(detail), 180);

    SetColumnVisible(visible);
    if (!visible) {
        return;
    }

    auto foreground = ThemeForegroundColor();
    auto secondary = Dimmed(foreground, 190);
    SetText(kTitleName, title, foreground, !title.empty());
    SetText(kTimeName, compactTime, secondary, true);
    SetAccentColor(accentGlyph ? SourceAccentColor(compactSource, foreground)
                               : Dimmed(foreground, 105));
}

bool StopUiTimer() {
    try {
        if (g_uiTimer) {
            g_uiTimer.Stop();
            if (g_uiTimerHasToken) {
                g_uiTimer.Tick(g_uiTimerToken);
            }
        }
    } catch (...) {
        LogCaughtException(L"stop UI timer");
        return false;
    }

    g_uiTimer = nullptr;
    g_uiTimerToken = {};
    g_uiTimerHasToken = false;
    return true;
}

std::tuple<uint64_t, uint64_t, int, int, int64_t> g_uiLastKey;
bool g_uiLastKeyValid = false;

void UiTimerTick(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading || !g_agendaGrid) {
        return;
    }

    // The widget only changes when a new snapshot or settings arrive, when the auth or
    // refresh state changes, or as the clock moves ("in 12m"), so most ticks do nothing.
    const auto key = std::make_tuple(g_snapshotGeneration.load(std::memory_order_relaxed),
                                     g_settingsGeneration.load(std::memory_order_relaxed),
                                     static_cast<int>(GetAuthState()),
                                     g_refreshUiState.load(std::memory_order_relaxed),
                                     NowUnix() / 15);
    if (g_uiLastKeyValid && key == g_uiLastKey) {
        return;
    }
    try {
        UpdateAgendaWidgetFromSnapshot();
        g_uiLastKey = key;
        g_uiLastKeyValid = true;
    } catch (...) {
        LogCaughtException(L"UI timer update");
    }
}

void StartUiTimer() {
    if (g_unloading || !g_agendaGrid) {
        return;
    }

    if (!StopUiTimer()) {
        return;
    }
    g_uiLastKeyValid = false;
    try {
        g_uiTimer = DispatcherTimer();
        g_uiTimer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::seconds(kUiRefreshSeconds)});
        g_uiTimerToken = g_uiTimer.Tick(&UiTimerTick);
        g_uiTimerHasToken = true;
        g_uiTimer.Start();
    } catch (...) {
        StopUiTimer();
        LogCaughtException(L"start UI timer");
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
    if (g_providerStarted.exchange(true)) {
        return;
    }
    // A previous shell that never reached Wh_ModUninit (crash, killed Explorer) left its
    // notification registration behind: remove it before registering again.
    if (Wh_GetIntValue(kToastRegisteredFlag, 0) != 0) {
        RemoveToastRegistration();
    }
    StartWorkerThread();
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

bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) {
        return false;
    }

    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(address);
    uintptr_t regionStart = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    uintptr_t regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd &&
           size <= regionEnd - start;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    wchar_t className[64]{};
    GetClassNameW(hTaskbarWnd, className, ARRAYSIZE(className));
    bool isSecondary = _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;

    HWND hTaskSwWnd = isSecondary
                          ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
                          : reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        Wh_Log(L"could not find taskband host window");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
    if (!taskBand) {
        Wh_Log(L"taskBand pointer is null");
        return nullptr;
    }

    void* expectedVftable = isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                        : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = isSecondary ? CSecondaryTaskBand_GetTaskbarHost_Original
                                      : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost || !TaskbarHost_FrameHeight_Original) {
        Wh_Log(L"required taskbar symbols were not resolved");
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    constexpr int kMaxSlotsToScan = 20;
    int slot = 0;
    for (;; ++slot) {
        if (!IsReadableMemoryRange(taskBandForTaskListWndSite, sizeof(void*))) {
            Wh_Log(L"unreadable taskBand slot %d", slot);
            return nullptr;
        }
        if (*reinterpret_cast<void**>(taskBandForTaskListWndSite) == expectedVftable) {
            break;
        }
        if (slot == kMaxSlotsToScan) {
            Wh_Log(L"ITaskListWndSite vftable not found");
            return nullptr;
        }
        taskBandForTaskListWndSite = reinterpret_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        Wh_Log(L"TaskbarHost shared_ptr is empty");
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    {
        const BYTE* bytes = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemoryRange(bytes, 8) && bytes[0] == 0x48 &&
            bytes[1] == 0x83 && bytes[2] == 0xEC && bytes[4] == 0x48 &&
            bytes[5] == 0x83 && bytes[6] == 0xC1 && bytes[7] <= 0x7F) {
            taskbarElementIUnknownOffset = bytes[7];
            frameHeightPatternRecognized = true;
        }
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    {
        const DWORD* words = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
        if (IsReadableMemoryRange(words, sizeof(DWORD) * 4) &&
            words[0] == 0xD503237F &&
            (words[1] & 0xFFC07FFF) == 0xA9807BFD &&
            words[2] == 0x910003FD &&
            (words[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (words[3] >> 12) & 0xFF;
            frameHeightPatternRecognized = true;
        }
    }
#else
    taskbarElementIUnknownOffset = 0x10;
    frameHeightPatternRecognized = true;
#endif

    if (!frameHeightPatternRecognized ||
        !IsReadableMemoryRange(static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
                                   taskbarElementIUnknownOffset,
                               sizeof(IUnknown*))) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        Wh_Log(L"unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        Wh_Log(L"taskbarElementIUnknown is null");
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    }
    return SUCCEEDED(hr) ? result : nullptr;
}

bool RemoveAgendaWidget() {
    bool cleanupSucceeded = true;
    bool popupClosed = true;
    bool clickHandlerRevoked = true;
    bool taskbarLayoutRevoked = true;
    try {
        CloseAgendaPopup();
    } catch (...) {
        popupClosed = false;
        cleanupSucceeded = false;
        LogCaughtException(L"close agenda popup");
    }

    try {
        if (g_taskbarRootGrid && g_taskbarLayoutHasToken) {
            g_taskbarRootGrid.LayoutUpdated(g_taskbarLayoutToken);
        }
    } catch (...) {
        taskbarLayoutRevoked = false;
        cleanupSucceeded = false;
        LogCaughtException(L"revoke taskbar position handler");
    }
    if (taskbarLayoutRevoked) g_taskbarLayoutToken = {};
    g_taskbarLayoutHasToken = !taskbarLayoutRevoked;
    g_taskbarRootGrid = nullptr;
    g_taskbarAnchor = nullptr;
    g_taskbarAfterAnchor = false;
    g_taskbarRootInjection = false;

    try {
        if (g_agendaGrid && g_widgetClickHasToken) {
            g_agendaGrid.Click(g_widgetClickToken);
        }
    } catch (...) {
        clickHandlerRevoked = false;
        cleanupSucceeded = false;
        LogCaughtException(L"revoke widget click handler");
    }
    if (clickHandlerRevoked) {
        g_widgetClickToken = {};
    }
    try {
        if (g_agendaGrid && g_widgetHoverHasTokens) {
            g_agendaGrid.PointerEntered(g_widgetEnterToken);
            g_agendaGrid.PointerExited(g_widgetExitToken);
        }
    } catch (...) {
        LogCaughtException(L"revoke widget hover handlers");
    }
    g_widgetEnterToken = {};
    g_widgetExitToken = {};
    g_widgetHoverHasTokens = false;
    g_trayVisual = nullptr;
    g_trayHovered = false;
    g_trayPopupOpen = false;
    g_widgetClickHasToken = !clickHandlerRevoked;
    try {
        if (!g_injectionParent) {
            g_agendaGrid = nullptr;
            g_injectionParent = nullptr;
            g_agendaColumn = -1;
            if (!cleanupSucceeded) {
                Wh_Log(L"cleanup incomplete (popup=%d, click=%d, taskbarPosition=%d)",
                       popupClosed ? 1 : 0, clickHandlerRevoked ? 1 : 0,
                       taskbarLayoutRevoked ? 1 : 0);
            }
            return cleanupSucceeded;
        }

        auto panel = g_injectionParent.try_as<Panel>();
        auto grid = g_injectionParent.try_as<Grid>();
        int column = g_agendaColumn;

        RemoveAgendaWidgetChildren(panel);
        if (grid && column >= 0 &&
            column < static_cast<int>(grid.ColumnDefinitions().Size())) {
            for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
                auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }
                int childColumn = Grid::GetColumn(child);
                if (childColumn > column) {
                    Grid::SetColumn(child, childColumn - 1);
                }
            }
            grid.ColumnDefinitions().RemoveAt(column);
        }
    } catch (...) {
        Wh_Log(L"exception while removing widget");
        cleanupSucceeded = false;
    }

    g_agendaGrid = nullptr;
    g_injectionParent = nullptr;
    g_agendaColumn = -1;
    if (!cleanupSucceeded) {
        Wh_Log(L"cleanup incomplete (popup=%d, click=%d, taskbarPosition=%d)",
               popupClosed ? 1 : 0, clickHandlerRevoked ? 1 : 0,
               taskbarLayoutRevoked ? 1 : 0);
    }
    return cleanupSucceeded;
}

bool InjectAgendaWidget() {
    ModSettings settings = SettingsCopy();
    if (!settings.enabled || g_unloading) {
        return false;
    }

    HWND hWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    if (!hWnd) {
        hWnd = FindCurrentProcessTaskbarWnd();
    }
    if (!hWnd) {
        Wh_Log(L"taskbar window not found");
        return false;
    }
    g_taskbarWnd.store(hWnd, std::memory_order_relaxed);

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            return false;
        }

        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            Wh_Log(L"taskbar XAML root content missing");
            return false;
        }

        auto target = ResolveInjectionTarget(root, settings.position);
        if (!target.panel) {
            if (settings.position != L"tray_before_clock") {
                Wh_Log(L"selected position unavailable; falling back to tray_before_clock");
                target = ResolveInjectionTarget(root, L"tray_before_clock");
            }
            if (!target.panel) {
                Wh_Log(L"tray_before_clock anchor unavailable");
                return false;
            }
        }

        RemoveAgendaWidgetChildren(target.panel);

        Button widget = BuildAgendaWidget();
        if (!widget) {
            return false;
        }

        if (target.taskbarRoot) {
            auto rootGrid = target.panel.try_as<Grid>();
            if (!rootGrid) return false;
            widget.HorizontalAlignment(HorizontalAlignment::Left);
            widget.VerticalAlignment(VerticalAlignment::Center);
            Grid::SetColumn(widget, 0);
            rootGrid.Children().Append(widget);
            g_taskbarRootInjection = true;
            g_taskbarRootGrid = rootGrid;
            g_taskbarAnchor = target.anchor;
            g_taskbarAfterAnchor = target.afterAnchor;
            g_taskbarLayoutToken = rootGrid.LayoutUpdated(
                [](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&) {
                    UpdateTaskbarWidgetPosition();
                });
            g_taskbarLayoutHasToken = true;
            g_agendaColumn = -1;
        } else if (auto stackPanel = target.panel.try_as<StackPanel>()) {
            if (target.insertSlot < 0 ||
                target.insertSlot > static_cast<int>(stackPanel.Children().Size())) {
                return false;
            }
            stackPanel.Children().InsertAt(target.insertSlot, widget);
            g_agendaColumn = -1;
        } else if (auto grid = target.panel.try_as<Grid>()) {
            ColumnDefinition column;
            column.Width({1.0, GridUnitType::Auto});
            if (target.insertSlot >= static_cast<int>(grid.ColumnDefinitions().Size())) {
                grid.ColumnDefinitions().Append(column);
            } else {
                grid.ColumnDefinitions().InsertAt(target.insertSlot, column);
                for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
                    auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (!child) {
                        continue;
                    }
                    int childColumn = Grid::GetColumn(child);
                    if (childColumn >= target.insertSlot) {
                        Grid::SetColumn(child, childColumn + 1);
                    }
                }
            }
            Grid::SetColumn(widget, target.insertSlot);
            grid.Children().Append(widget);
            g_agendaColumn = target.insertSlot;
        } else {
            return false;
        }

        g_agendaGrid = widget;
        g_injectionParent = target.panel;
        if (g_taskbarRootInjection) UpdateTaskbarWidgetPosition();
        UpdateAgendaWidgetFromSnapshot();
        StartUiTimer();
        return true;
    } catch (...) {
        Wh_Log(L"exception while injecting widget");
        g_agendaGrid = nullptr;
        g_injectionParent = nullptr;
        g_agendaColumn = -1;
        return false;
    }
}

void ApplySettingsOnTaskbarThread() {
    StopRetryTimer();
    StopUiTimer();

    RemoveAgendaWidget();
    if (!g_unloading) {
        InjectAgendaWidget();
    }
}

bool StopRetryTimer() {
    try {
        if (g_retryTimer) {
            g_retryTimer.Stop();
            if (g_retryTimerHasToken) {
                g_retryTimer.Tick(g_retryTimerToken);
            }
        }
    } catch (...) {
        LogCaughtException(L"stop retry timer");
        return false;
    }

    g_retryTimer = nullptr;
    g_retryTimerToken = {};
    g_retryTimerHasToken = false;
    g_retryRoot = nullptr;
    g_retryCount = 0;
    return true;
}

void RetryTimerTick(winrt::Windows::Foundation::IInspectable const&,
                    winrt::Windows::Foundation::IInspectable const&) {
    auto root = g_retryRoot;
    int nextRetryCount = g_retryCount + 1;
    StopRetryTimer();
    if (!g_unloading && root) {
        ApplySettingsWithRetry(root, nextRetryCount);
    }
}

void ApplySettingsWithRetry(FrameworkElement xamlRootContent, int retryCount) {
    constexpr int kMaxRetries = 50;

    if (g_unloading) {
        return;
    }

    auto systemTrayFrame = FindChildByClassName(xamlRootContent,
                                                L"SystemTray.SystemTrayFrame");
    auto systemTrayFrameGrid = systemTrayFrame
                                   ? FindChildByName(systemTrayFrame,
                                                     L"SystemTrayFrameGrid")
                                   : nullptr;
    if (!systemTrayFrameGrid) {
        if (retryCount >= kMaxRetries) {
            Wh_Log(L"SystemTrayFrameGrid not found after retries");
            return;
        }

        StopRetryTimer();
        g_retryRoot = xamlRootContent;
        g_retryCount = retryCount;
        g_retryTimer = DispatcherTimer();
        g_retryTimer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(100)});
        g_retryTimerToken = g_retryTimer.Tick(&RetryTimerTick);
        g_retryTimerHasToken = true;
        g_retryTimer.Start();
        return;
    }

    StopRetryTimer();
    ApplySettingsOnTaskbarThread();
}

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    // The old XAML tree is about to be replaced. Revoke every module-owned
    // delegate while it is still alive, before Taskbar::StartTaskbar tears it
    // down and before invoking the original rebuild path.
    HWND oldTaskbar = g_taskbarWnd.load(std::memory_order_relaxed);
    if (oldTaskbar && !CleanupTaskbarResources(oldTaskbar)) {
        Wh_Log(L"taskbar rebuild started with incomplete cleanup");
    }
    TrayUI_StartTaskbar_Original(pThis);
    try {
        if (g_unloading) {
            return;
        }

        HWND hWnd = FindCurrentProcessTaskbarWnd();
        if (!hWnd) {
            Wh_Log(L"TrayUI::StartTaskbar hook could not find taskbar window");
            return;
        }

        g_taskbarWnd.store(hWnd, std::memory_order_relaxed);
        StartProviderInShellProcess();
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            return;
        }
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) {
            return;
        }

        ApplySettingsWithRetry(content);
        if (g_workerWakeEvent) {
            SetEvent(g_workerWakeEvent);
        }
    } catch (...) {
        LogCaughtException(L"TrayUI_StartTaskbar_Hook post-original logic");
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE taskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarDll) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };

    return WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

struct TaskbarCleanupResult {
    bool retryTimerStopped = false;
    bool uiTimerStopped = false;
    bool widgetRemoved = false;

    bool Succeeded() const {
        return retryTimerStopped && uiTimerStopped && widgetRemoved;
    }
};

void CleanupTaskbarResourcesOnTaskbarThread(void* param) {
    auto* result = static_cast<TaskbarCleanupResult*>(param);
    if (!result) {
        return;
    }

    result->retryTimerStopped = StopRetryTimer();
    result->uiTimerStopped = StopUiTimer();
    result->widgetRemoved = RemoveAgendaWidget();
}

bool CleanupTaskbarResources(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    TaskbarCleanupResult cleanup;
    WindowThreadRunResult runResult = RunFromWindowThread(
        hWnd, CleanupTaskbarResourcesOnTaskbarThread, &cleanup);
    if (!runResult.Succeeded()) {
        Wh_Log(L"taskbar cleanup marshal failed (dispatched=%d, ran=%d, callback=%d)",
               runResult.dispatched ? 1 : 0, runResult.callbackRan ? 1 : 0,
               runResult.callbackSucceeded ? 1 : 0);
        return false;
    }

    if (!cleanup.Succeeded()) {
        Wh_Log(L"taskbar cleanup incomplete (retryTimer=%d, uiTimer=%d, widget=%d)",
               cleanup.retryTimerStopped ? 1 : 0, cleanup.uiTimerStopped ? 1 : 0,
               cleanup.widgetRemoved ? 1 : 0);
        return false;
    }

    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    g_unloading = false;
    g_workerStop = false;
    LoadSettings();

    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workerWakeEvent) {
        Wh_Log(L"failed to create worker wake event");
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"failed to hook Taskbar.dll symbols");
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    g_taskbarWnd.store(hWnd, std::memory_order_relaxed);
    if (!hWnd) {
        return;
    }
    StartProviderInShellProcess();

    RunFromWindowThread(hWnd, [](void* param) {
        try {
            if (g_unloading) {
                return;
            }
            HWND taskbarWnd = static_cast<HWND>(param);
            auto xamlRoot = GetTaskbarXamlRoot(taskbarWnd);
            if (!xamlRoot) {
                return;
            }

            auto content = xamlRoot.Content().try_as<FrameworkElement>();
            if (!content) {
                return;
            }

            ApplySettingsWithRetry(content);
        } catch (...) {
            LogCaughtException(L"after-init taskbar callback");
        }
    }, hWnd);

    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }
}

void Wh_ModUninit() {
    g_unloading = true;
    StopWorkerThread();
    if (g_providerStarted.load()) {
        RemoveToastRegistration();
    }

    HWND cachedHWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    bool taskbarCleanupSucceeded = CleanupTaskbarResources(cachedHWnd);
    if (!taskbarCleanupSucceeded) {
        HWND freshHWnd = FindCurrentProcessTaskbarWnd();
        if (freshHWnd && freshHWnd != cachedHWnd) {
            g_taskbarWnd.store(freshHWnd, std::memory_order_relaxed);
            taskbarCleanupSucceeded = CleanupTaskbarResources(freshHWnd);
        }
    }

    if (!taskbarCleanupSucceeded) {
        Wh_Log(L"unsafe unload condition: taskbar-thread cleanup failed; skipping off-thread XAML/timer cleanup");
    }

    if (g_workerWakeEvent) {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        hWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    }
    if (!hWnd) {
        return;
    }

    g_taskbarWnd.store(hWnd, std::memory_order_relaxed);
    RunFromWindowThread(hWnd, [](void*) {
        if (g_unloading) {
            return;
        }
        ApplySettingsOnTaskbarThread();
        UpdateAgendaWidgetFromSnapshot();
    }, nullptr);
}
