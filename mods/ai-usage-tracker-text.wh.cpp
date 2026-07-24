// ==WindhawkMod==
// @id              ai-usage-tracker-text
// @name            AI Usage Tray Text
// @description     Shows Claude Code and Codex usage and activity in the Windows 11 system tray
// @version         0.10.0
// @author          mitko
// @github          https://github.com/medenmite
// @homepage        https://github.com/medenmite/ai-usage-tracker
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshlwapi -lwininet
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0 only.
// The WinINet fetching implementation is adapted from Taskbar Clock
// Customization by m417z. See THIRD_PARTY_NOTICES.md in the project repository.

// ==WindhawkModReadme==
/*
# AI Usage Tray Text

Adds live Claude Code and Codex usage text directly to the Windows 11
notification area. Hovering the text shows separate provider sections with
usage windows, reset times, active root tasks, and nested subagents. Codex
activity supports both the CLI and desktop app.

![Taskbar preview](https://raw.githubusercontent.com/medenmite/ai-usage-tracker/main/assets/taskbar-inline.png)

## Required helper app

This mod only renders text. Install and run the companion Python helper from:

https://github.com/medenmite/ai-usage-tracker

The helper reads usage data locally and serves the rendered payload on
`http://127.0.0.1:8765/usage`. The server binds only to the loopback interface.

## Default setup

1. Install and start the helper app.
2. Keep URL set to `http://127.0.0.1:8765/usage`.
3. Keep Extract after set to `[ALL]` and Extract before set to `[/ALL]`.
4. Set Position to the tray element after which the text should appear.
5. Leave Dump diagnostics disabled unless troubleshooting.

## Compatibility warning

This mod relies on undocumented Windows 11 XAML internals and private symbols.
A Windows feature update can rename those symbols and temporarily break the
mod. When reporting a problem, include the Windows build number, Windhawk
version, mod version, and detailed Windhawk log.

## Privacy and network behavior

The mod fetches only the configured URL. The default URL is local loopback.
The companion helper contacts Anthropic for Claude Code usage and starts the
local Codex app-server to read Codex rate limits. The project is not affiliated
with Microsoft, Anthropic, or OpenAI.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Url: http://127.0.0.1:8765/usage
  $name: URL
  $description: Fetched on the interval below. Must return HTTP 200.
- Start: '[ALL]'
  $name: Extract after
  $description: Text just before the part you want. Empty = from the start.
- End: '[/ALL]'
  $name: Extract before
  $description: Text just after the part you want. Empty = to the end.
- UpdateInterval: 5
  $name: Update interval (seconds)
- MaxLength: 32
  $name: Maximum length
- FontSize: 0
  $name: Font size
  $description: Zero uses the tray default.
- InsertAfter: NotifyIconStack
  $name: Position (insert after)
  $description: >-
    Name of the tray element to sit immediately to the right of.
    NotifyIconStack is the hidden-icons chevron, NonActivatableStack is the
    language indicator, ControlCenterButton is the wifi/volume/battery group,
    NotificationCenterButton is the clock, ShowDesktopStack is the far right
    edge. Leave empty to append at the very end.
- DumpDiagnostics: false
  $name: Dump diagnostics to the log
  $description: >-
    Logs the tray visual tree. Enable only while troubleshooting because it is
    noisy.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <wininet.h>

// winbase.h defines GetCurrentTime() as a macro, and
// Windows.UI.Xaml.Media.Animation declares a method with the same name. The
// macro wins and mangles the declaration, so it has to go before any winrt
// XAML header is pulled in. Nothing here uses it.
#undef GetCurrentTime

#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <winrt/Windows.Foundation.h>
// Needed for IVector<T>::Append. The .0.h forward-declaration header declares
// it with a deduced return type; the definition only arrives with this header.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
// PointerEventHandler's templated constructor - the one that takes a lambda -
// is only defined here, not in the .2.h forward declarations.
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace Input = winrt::Windows::UI::Xaml::Input;

// ---------------------------------------------------------------- settings --

struct {
    WindhawkUtils::StringSetting url;
    WindhawkUtils::StringSetting start;
    WindhawkUtils::StringSetting end;
    int updateInterval;
    int maxLength;
    int fontSize;
    WindhawkUtils::StringSetting insertAfter;
    bool dumpDiagnostics;
} g_settings;

static void LoadSettings() {
    g_settings.url = WindhawkUtils::StringSetting::make(L"Url");
    g_settings.start = WindhawkUtils::StringSetting::make(L"Start");
    g_settings.end = WindhawkUtils::StringSetting::make(L"End");
    g_settings.updateInterval = Wh_GetIntSetting(L"UpdateInterval");
    g_settings.maxLength = Wh_GetIntSetting(L"MaxLength");
    g_settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_settings.insertAfter = WindhawkUtils::StringSetting::make(L"InsertAfter");
    g_settings.dumpDiagnostics = Wh_GetIntSetting(L"DumpDiagnostics") != 0;

    if (g_settings.updateInterval < 5) {
        g_settings.updateInterval = 5;
    }
}

// ------------------------------------------------------------ fetch thread --

std::atomic<HANDLE> g_fetchThread;
HANDLE g_fetchStopEvent = nullptr;
HANDLE g_fetchRefreshEvent = nullptr;

std::mutex g_textMutex;
std::wstring g_currentText;
std::wstring g_detailText;
std::wstring g_activityText;
std::atomic<bool> g_textDirty{false};

// Defined further down with the XAML code; the fetch thread calls it as soon
// as new text arrives.
static void PostTextUpdate();
static void RefreshCard();

// Lifted from taskbar-clock-customization.wh.cpp (GPL-3.0, m417z). Using the
// same WinINet path as the clock mod means it behaves identically with respect
// to proxies and caching - worth keeping if you ever compare the two.
static std::optional<std::wstring> GetUrlContent(PCWSTR url) {
    HINTERNET open = InternetOpen(L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG,
                                  nullptr, nullptr, 0);
    if (!open) {
        return std::nullopt;
    }

    HINTERNET handle = InternetOpenUrl(
        open, url, nullptr, 0,
        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
        0);
    if (!handle) {
        InternetCloseHandle(open);
        return std::nullopt;
    }

    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if (!HttpQueryInfo(handle, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &status, &statusSize, nullptr) ||
        status != 200) {
        InternetCloseHandle(handle);
        InternetCloseHandle(open);
        return std::nullopt;
    }

    std::string bytes;
    char buffer[1024];
    DWORD read = 0;
    while (InternetReadFile(handle, buffer, sizeof(buffer), &read) && read) {
        bytes.append(buffer, read);
    }

    InternetCloseHandle(handle);
    InternetCloseHandle(open);

    int needed = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                     (int)bytes.size(), nullptr, 0);
    std::wstring wide(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        wide.data(), needed);
    return wide;
}

static std::wstring ExtractBetween(std::wstring_view content, PCWSTR start,
                                   PCWSTR end) {
    size_t from = 0;
    if (*start) {
        auto found = content.find(start);
        if (found == content.npos) {
            return {};
        }
        from = found + wcslen(start);
    }
    size_t to = *end ? content.find(end, from) : content.length();
    if (to == content.npos) {
        return {};
    }
    return std::wstring(content.substr(from, to - from));
}

static DWORD WINAPI FetchThreadProc(LPVOID) {
    Wh_Log(L"fetch thread started, interval=%d s", g_settings.updateInterval);
    HANDLE waits[2] = {g_fetchStopEvent, g_fetchRefreshEvent};

    for (;;) {
        std::wstring text;
        std::wstring detail;
        std::wstring activity;
        if (auto content = GetUrlContent(g_settings.url.get())) {
            Wh_Log(L"fetched %zu chars from %s", content->length(),
                   g_settings.url.get());
            text = ExtractBetween(*content, g_settings.start.get(),
                                  g_settings.end.get());
            if (text.empty()) {
                // Distinguishes "the server is unreachable" from "the server
                // answered but the markers did not match" - two very different
                // problems that otherwise look identical.
                Wh_Log(L"markers not found: start=\"%s\" end=\"%s\"",
                       g_settings.start.get(), g_settings.end.get());
            }
            if (g_settings.maxLength > 0 &&
                (int)text.length() > g_settings.maxLength) {
                text.resize(g_settings.maxLength);
            }
            Wh_Log(L"extracted: \"%s\"", text.c_str());
            detail = ExtractBetween(*content, L"[DETAIL]", L"[/DETAIL]");
            activity =
                ExtractBetween(*content, L"[ACTIVITY]", L"[/ACTIVITY]");
        } else {
            // Deliberately blank rather than stale: a frozen number is worse
            // than no number, because you cannot tell it has stopped updating.
            Wh_Log(L"fetch FAILED for %s", g_settings.url.get());
        }

        {
            std::lock_guard<std::mutex> lock(g_textMutex);
            if (g_currentText != text || g_detailText != detail ||
                g_activityText != activity) {
                g_currentText = text;
                g_detailText = detail;
                g_activityText = activity;
                g_textDirty = true;
            }
        }
        // Keep retrying a pending update. The first fetch can finish before
        // the taskbar XAML dispatcher is available, and RunAsync can also
        // transiently reject work while Explorer rebuilds the tray.
        if (g_textDirty.load()) {
            PostTextUpdate();
        }

        DWORD result = WaitForMultipleObjects(
            2, waits, FALSE, g_settings.updateInterval * 1000);
        if (result == WAIT_OBJECT_0) {
            break;  // stop
        }
    }
    return 0;
}

static void StartFetchThread() {
    if (g_fetchThread.load()) {
        return;
    }
    g_fetchStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_fetchRefreshEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    HANDLE thread = CreateThread(nullptr, 0, FetchThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread FAILED, err=%lu", GetLastError());
        return;
    }
    g_fetchThread = thread;
}

static void StopFetchThread() {
    HANDLE thread = g_fetchThread.exchange(nullptr);
    if (!thread) {
        return;
    }
    SetEvent(g_fetchStopEvent);
    WaitForSingleObject(thread, 5000);
    CloseHandle(thread);
    CloseHandle(g_fetchStopEvent);
    CloseHandle(g_fetchRefreshEvent);
    g_fetchStopEvent = g_fetchRefreshEvent = nullptr;
}

// ------------------------------------------------------- phase 1: dumping --

// Recursively logs the visual tree so you can see the real class names and
// element names on YOUR Windows build. Microsoft renames these between
// releases; do not trust any hardcoded path, including the one below.
static void DumpVisualTree(const DependencyObject& node, int depth = 0,
                           int maxDepth = 22) {
    if (!node || depth > maxDepth) {
        return;
    }

    std::wstring indent(depth * 2, L' ');
    std::wstring className;
    try {
        className = winrt::get_class_name(node).c_str();
    } catch (...) {
        className = L"<unknown>";
    }

    std::wstring name;
    if (auto element = node.try_as<FrameworkElement>()) {
        name = element.Name().c_str();
    }

    std::wstring text;
    if (auto block = node.try_as<Controls::TextBlock>()) {
        text = L"  text=\"" + std::wstring(block.Text().c_str()) + L"\"";
    }

    std::wstring line = indent + className;
    if (!name.empty()) {
        line += L"#" + name;
    }
    line += text;
    Wh_Log(L"%s", line.c_str());

    int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++) {
        DumpVisualTree(Media::VisualTreeHelper::GetChild(node, i), depth + 1,
                       maxDepth);
    }
}

// --------------------------------------------- phase 2: element insertion --

// Keep a strong reference while the element is installed. A weak_ref can resolve
// to null even while Explorer still displays the element, which prevents later
// HTTP refreshes from updating the visible text.
Controls::TextBlock g_ourTextBlock{nullptr};
std::atomic<bool> g_inserted{false};
Controls::Grid g_ourGrid{nullptr};
Controls::Border g_ourBorder{nullptr};
winrt::Windows::UI::Core::CoreDispatcher g_uiDispatcher{nullptr};

// Find a descendant by its XAML Name.
static FrameworkElement FindByName(const DependencyObject& node, PCWSTR name,
                                   int depth = 0) {
    if (!node || depth > 22) {
        return nullptr;
    }
    if (auto element = node.try_as<FrameworkElement>()) {
        if (element.Name() == name) {
            return element;
        }
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++) {
        if (auto found = FindByName(Media::VisualTreeHelper::GetChild(node, i),
                                    name, depth + 1)) {
            return found;
        }
    }
    return nullptr;
}

// Grid.Column is set on the grid's immediate children, so the anchor lookup
// must not descend into the tree or we would read the wrong column.
static FrameworkElement FindDirectChildByName(const Controls::Grid& grid,
                                              PCWSTR name) {
    auto children = grid.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        if (auto element = children.GetAt(i).try_as<FrameworkElement>()) {
            if (element.Name() == name) {
                return element;
            }
        }
    }
    return nullptr;
}

// Insert into SystemTrayFrameGrid - the frame's top-level Grid, whose children
// are the tray stacks and buttons. It is a plain Grid, not a virtualising
// panel, so a hand-added child is not recycled away on relayout.
// Prefer the shell's own hover brush so we match the other tray buttons in
// both themes; fall back to a low-alpha white if the resource is missing.
static Media::Brush HoverBrush() {
    try {
        auto resources = Application::Current().Resources();
        auto key = winrt::box_value(winrt::hstring(L"SubtleFillColorSecondaryBrush"));
        if (resources.HasKey(key)) {
            if (auto brush = resources.Lookup(key).try_as<Media::Brush>()) {
                return brush;
            }
        }
    } catch (...) {
    }
    return Media::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(0x20, 0xFF, 0xFF, 0xFF));
}

static Media::SolidColorBrush SeverityBrush(double percent) {
    if (percent >= 85.0) {
        return Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xE1, 0x50, 0x50));
    }
    if (percent >= 70.0) {
        return Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xE6, 0xAA, 0x3C));
    }
    return Media::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0x6E, 0xC8, 0xBE));
}

struct UsageRow {
    std::wstring key;
    std::wstring provider;
    bool running = false;
    std::wstring label;
    std::wstring percentText;
    double percent = 0;
    std::wstring reset;
};

struct ActivityAgentRow {
    int depth = 1;
    std::wstring status;
    std::wstring label;
};

struct ActivityTaskRow {
    int index = 0;
    std::wstring status;
    std::wstring title;
    std::vector<ActivityAgentRow> agents;
};

struct ProviderActivity {
    bool present = false;
    bool running = false;
    bool available = false;
    int overflowTasks = 0;
    int overflowAgents = 0;
    std::vector<ActivityTaskRow> tasks;
};

static std::vector<std::wstring> SplitFields(const std::wstring& line) {
    std::vector<std::wstring> fields;
    size_t start = 0;
    for (;;) {
        size_t bar = line.find(L'|', start);
        fields.push_back(line.substr(
            start, bar == std::wstring::npos ? std::wstring::npos : bar - start));
        if (bar == std::wstring::npos) {
            return fields;
        }
        start = bar + 1;
    }
}

static int HexDigit(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') {
        return ch - L'0';
    }
    if (ch >= L'a' && ch <= L'f') {
        return ch - L'a' + 10;
    }
    if (ch >= L'A' && ch <= L'F') {
        return ch - L'A' + 10;
    }
    return -1;
}

static std::wstring DecodeField(const std::wstring& encoded) {
    std::string bytes;
    bytes.reserve(encoded.size());
    for (size_t i = 0; i < encoded.size(); i++) {
        if (encoded[i] == L'%' && i + 2 < encoded.size()) {
            int high = HexDigit(encoded[i + 1]);
            int low = HexDigit(encoded[i + 2]);
            if (high >= 0 && low >= 0) {
                bytes.push_back((char)((high << 4) | low));
                i += 2;
                continue;
            }
        }
        if (encoded[i] <= 0x7F) {
            bytes.push_back((char)encoded[i]);
        }
    }
    if (bytes.empty()) {
        return {};
    }
    int needed = MultiByteToWideChar(
        CP_UTF8, 0, bytes.data(), (int)bytes.size(), nullptr, 0);
    if (needed <= 0) {
        return encoded;
    }
    std::wstring decoded(needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        decoded.data(), needed);
    return decoded;
}

static void ForEachLine(
    const std::wstring& text,
    const std::function<void(const std::wstring&)>& callback) {
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t nl = text.find(L'\n', pos);
        std::wstring line = text.substr(
            pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
        if (!line.empty()) {
            callback(line);
        }
        if (nl == std::wstring::npos) {
            break;
        }
        pos = nl + 1;
    }
}

static Media::SolidColorBrush ActivityBrush(const std::wstring& status) {
    if (status == L"waiting_approval" || status == L"waiting_input") {
        return Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xE6, 0xAA, 0x3C));
    }
    return Media::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0x6E, 0xC8, 0xBE));
}

static std::wstring StatusLabel(const std::wstring& status) {
    if (status == L"waiting_approval") {
        return L"Waiting for approval";
    }
    if (status == L"waiting_input") {
        return L"Waiting for input";
    }
    return L"Working";
}

static void AddProviderSection(
    Controls::StackPanel& root, const std::wstring& key,
    const std::wstring& displayName, const std::vector<UsageRow>& usage,
    const ProviderActivity& activity, bool addSeparator) {
    Controls::StackPanel section;
    section.Orientation(Controls::Orientation::Vertical);
    section.Spacing(4);

    bool running = activity.running;
    for (const auto& row : usage) {
        if (row.key == key) {
            running = running || row.running;
        }
    }

    Controls::TextBlock header;
    header.Text(winrt::hstring(
        displayName + (running ? L"  \u25CF" : L"  \u25CB")));
    header.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    header.FontSize(13);
    section.Children().Append(header);

    bool hasUsage = false;
    for (const auto& row : usage) {
        if (row.key != key) {
            continue;
        }
        hasUsage = true;
        Controls::TextBlock caption;
        caption.Text(winrt::hstring(
            row.label + L"   " + row.percentText + L"%" +
            (row.reset.empty() ? L"" : (L"   resets in " + row.reset))));
        caption.FontSize(11);
        caption.Opacity(0.75);
        section.Children().Append(caption);

        Controls::ProgressBar bar;
        bar.Minimum(0);
        bar.Maximum(100);
        bar.Value(row.percent);
        bar.Height(4);
        bar.Foreground(SeverityBrush(row.percent));
        bar.Margin(Thickness{0, 1, 0, 2});
        section.Children().Append(bar);
    }
    if (!hasUsage) {
        Controls::TextBlock unavailable;
        unavailable.Text(L"Usage unavailable");
        unavailable.FontSize(11);
        unavailable.Opacity(0.65);
        section.Children().Append(unavailable);
    }

    Controls::TextBlock activityHeader;
    activityHeader.Text(L"Activity");
    activityHeader.FontSize(11);
    activityHeader.FontWeight(
        winrt::Windows::UI::Text::FontWeights::SemiBold());
    activityHeader.Margin(Thickness{0, 5, 0, 0});
    section.Children().Append(activityHeader);

    if (!activity.available) {
        Controls::TextBlock unavailable;
        unavailable.Text(L"Activity unavailable");
        unavailable.FontSize(11);
        unavailable.Opacity(0.65);
        section.Children().Append(unavailable);
    } else if (activity.tasks.empty()) {
        Controls::TextBlock empty;
        empty.Text(L"No active tasks");
        empty.FontSize(11);
        empty.Opacity(0.65);
        section.Children().Append(empty);
    } else {
        for (const auto& task : activity.tasks) {
            Controls::TextBlock taskLine;
            taskLine.Text(winrt::hstring(
                L"\u25CF " + StatusLabel(task.status) + L"  " + task.title));
            taskLine.FontSize(11);
            taskLine.TextWrapping(TextWrapping::NoWrap);
            taskLine.TextTrimming(TextTrimming::CharacterEllipsis);
            taskLine.MaxWidth(380);
            taskLine.Foreground(ActivityBrush(task.status));
            section.Children().Append(taskLine);

            for (const auto& agent : task.agents) {
                Controls::TextBlock agentLine;
                std::wstring indent((size_t)agent.depth * 2, L' ');
                agentLine.Text(winrt::hstring(
                    indent + L"\u21B3 " + agent.label + L"  \u00B7  " +
                    StatusLabel(agent.status)));
                agentLine.FontSize(10);
                agentLine.Opacity(0.85);
                agentLine.TextWrapping(TextWrapping::NoWrap);
                agentLine.TextTrimming(TextTrimming::CharacterEllipsis);
                agentLine.MaxWidth(380);
                agentLine.Foreground(ActivityBrush(agent.status));
                section.Children().Append(agentLine);
            }
        }
        if (activity.overflowTasks || activity.overflowAgents) {
            std::wstring overflow;
            if (activity.overflowTasks) {
                overflow += L"+" + std::to_wstring(activity.overflowTasks) +
                            L" task" +
                            (activity.overflowTasks == 1 ? L"" : L"s");
            }
            if (activity.overflowAgents) {
                if (!overflow.empty()) {
                    overflow += L", ";
                }
                overflow += L"+" + std::to_wstring(activity.overflowAgents) +
                            L" agent" +
                            (activity.overflowAgents == 1 ? L"" : L"s");
            }
            Controls::TextBlock more;
            more.Text(winrt::hstring(overflow + L" more"));
            more.FontSize(10);
            more.Opacity(0.65);
            more.Margin(Thickness{0, 2, 0, 0});
            section.Children().Append(more);
        }
    }

    Controls::Border container;
    container.Child(section);
    container.Padding(Thickness{0, 2, 0, addSeparator ? 10.0 : 2.0});
    if (addSeparator) {
        container.BorderThickness(Thickness{0, 0, 0, 1});
        container.BorderBrush(Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(
                0x30, 0x80, 0x80, 0x80)));
        container.Margin(Thickness{0, 0, 0, 8});
    }
    root.Children().Append(container);
}

// Usage rows and activity records are emitted independently so older helpers
// still produce a valid card, while newer helpers can add task hierarchy.
static Controls::StackPanel BuildCard() {
    Controls::StackPanel root;
    root.Orientation(Controls::Orientation::Vertical);
    root.MinWidth(300);
    root.MaxWidth(420);

    std::wstring detail;
    std::wstring activityText;
    {
        std::lock_guard<std::mutex> lock(g_textMutex);
        detail = g_detailText;
        activityText = g_activityText;
    }

    std::vector<UsageRow> usage;
    ForEachLine(detail, [&](const std::wstring& line) {
        auto fields = SplitFields(line);
        if (fields.size() < 6) {
            return;
        }
        usage.push_back(UsageRow{
            fields[0], fields[1], fields[2] == L"1", fields[3],
            fields[4], _wtof(fields[4].c_str()), fields[5],
        });
    });

    ProviderActivity claude;
    ProviderActivity codex;
    auto providerFor = [&](const std::wstring& key) -> ProviderActivity* {
        return key == L"CC" ? &claude : key == L"CX" ? &codex : nullptr;
    };
    ForEachLine(activityText, [&](const std::wstring& line) {
        auto fields = SplitFields(line);
        if (fields.size() < 2) {
            return;
        }
        ProviderActivity* provider = providerFor(fields[1]);
        if (!provider) {
            return;
        }
        if (fields[0] == L"S" && fields.size() >= 6) {
            provider->present = true;
            provider->running = fields[2] == L"1";
            provider->available = fields[3] == L"1";
            provider->overflowTasks = _wtoi(fields[4].c_str());
            provider->overflowAgents = _wtoi(fields[5].c_str());
        } else if (fields[0] == L"T" && fields.size() >= 5) {
            provider->tasks.push_back(ActivityTaskRow{
                _wtoi(fields[2].c_str()), fields[3],
                DecodeField(fields[4]), {},
            });
        } else if (fields[0] == L"A" && fields.size() >= 6) {
            int taskIndex = _wtoi(fields[2].c_str());
            for (auto& task : provider->tasks) {
                if (task.index == taskIndex) {
                    task.agents.push_back(ActivityAgentRow{
                        _wtoi(fields[3].c_str()), fields[4],
                        DecodeField(fields[5]),
                    });
                    break;
                }
            }
        }
    });

    AddProviderSection(root, L"CC", L"Claude Code", usage, claude, true);
    AddProviderSection(root, L"CX", L"Codex", usage, codex, false);
    return root;
}

static void RefreshCard() {
    if (g_ourBorder) {
        Controls::ToolTip tip;
        tip.Content(BuildCard());
        Controls::ToolTipService::SetToolTip(g_ourBorder, tip);
    }
}

// Remove every copy of our TextBlock from the grid. Called before inserting so
// leftovers from a previous mod session are swept up automatically.
static int RemoveOurTextBlocks(const Controls::Grid& grid) {
    if (!grid) {
        return 0;
    }
    auto children = grid.Children();
    int removed = 0;
    for (int i = (int)children.Size() - 1; i >= 0; i--) {
        if (auto element = children.GetAt(i).try_as<FrameworkElement>()) {
            if (element.Name() == L"AiUsageTrayText") {
                children.RemoveAt(i);
                removed++;
            }
        }
    }
    return removed;
}

static bool TryInsertIntoFrameGrid(const FrameworkElement& frame) {
    if (g_inserted.exchange(true)) {
        return true;  // already done; do not add a second one
    }

    auto element = FindByName(frame, L"SystemTrayFrameGrid");
    if (!element) {
        Wh_Log(L"SystemTrayFrameGrid not found");
        g_inserted = false;
        return false;
    }

    auto grid = element.try_as<Controls::Grid>();
    if (!grid) {
        Wh_Log(L"SystemTrayFrameGrid is not a Grid");
        g_inserted = false;
        return false;
    }

    if (int stale = RemoveOurTextBlocks(grid)) {
        Wh_Log(L"swept %d stale text block(s) from a previous session", stale);
    }

    Controls::TextBlock block;
    block.Name(L"AiUsageTrayTextInner");
    block.VerticalAlignment(VerticalAlignment::Center);
    block.HorizontalAlignment(HorizontalAlignment::Center);
    block.TextAlignment(TextAlignment::Center);
    if (g_settings.fontSize > 0) {
        block.FontSize(g_settings.fontSize);
    }

    // Borrow the clock's brush so we follow the theme instead of hardcoding a
    // colour that looks wrong half the time.
    if (auto clock = FindByName(frame, L"TimeInnerTextBlock")) {
        if (auto clockText = clock.try_as<Controls::TextBlock>()) {
            block.Foreground(clockText.Foreground());
            if (g_settings.fontSize <= 0) {
                block.FontSize(clockText.FontSize());
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_textMutex);
        block.Text(winrt::hstring(g_currentText));
        g_textDirty = false;
    }

    // A bare TextBlock has no visual states, so it never shows the rounded
    // hover fill the other tray buttons get. Wrapping it in a Border and
    // swapping the background on pointer enter/exit reproduces that.
    Controls::Border border;
    border.Name(L"AiUsageTrayText");
    border.Child(block);
    border.CornerRadius(CornerRadius{4, 4, 4, 4});
    border.Padding(Thickness{8, 2, 8, 2});
    border.Margin(Thickness{2, 0, 2, 0});
    border.VerticalAlignment(VerticalAlignment::Center);
    // A null background would make the Border invisible to hit-testing, so it
    // has to be explicitly transparent rather than unset.
    border.Background(Media::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0)));

    border.PointerEntered([](winrt::Windows::Foundation::IInspectable const& sender,
                             Input::PointerRoutedEventArgs const&) {
        if (auto self = sender.try_as<Controls::Border>()) {
            self.Background(HoverBrush());
        }
    });
    border.PointerExited([](winrt::Windows::Foundation::IInspectable const& sender,
                            Input::PointerRoutedEventArgs const&) {
        if (auto self = sender.try_as<Controls::Border>()) {
            self.Background(Media::SolidColorBrush(
                winrt::Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0)));
        }
    });

    // Each existing child sits in its own column. To land in the middle of the
    // row rather than at the far end, insert a column after the anchor and
    // push everything to its right one place along.
    Controls::ColumnDefinition column;
    column.Width(GridLength{1.0, GridUnitType::Auto});

    int columnIndex = -1;
    PCWSTR anchorName = g_settings.insertAfter.get();
    if (anchorName && *anchorName) {
        if (auto anchor = FindDirectChildByName(grid, anchorName)) {
            columnIndex = Controls::Grid::GetColumn(anchor) + 1;
        } else {
            Wh_Log(L"anchor \"%s\" not found, appending at the end",
                   anchorName);
        }
    }

    int columnCount = (int)grid.ColumnDefinitions().Size();
    if (columnIndex < 0 || columnIndex > columnCount) {
        columnIndex = columnCount;
        grid.ColumnDefinitions().Append(column);
    } else {
        auto children = grid.Children();
        for (uint32_t i = 0; i < children.Size(); i++) {
            if (auto child = children.GetAt(i).try_as<FrameworkElement>()) {
                int existing = Controls::Grid::GetColumn(child);
                if (existing >= columnIndex) {
                    Controls::Grid::SetColumn(child, existing + 1);
                }
            }
        }
        grid.ColumnDefinitions().InsertAt((uint32_t)columnIndex, column);
    }

    Controls::Grid::SetColumn(border, columnIndex);

    grid.Children().Append(border);
    g_ourTextBlock = block;
    g_ourBorder = border;
    g_ourGrid = grid;
    g_uiDispatcher = block.Dispatcher();
    RefreshCard();

    Wh_Log(L"INSERTED at column %d of %d (after \"%s\")", columnIndex,
           (int)grid.ColumnDefinitions().Size(), anchorName);
    return true;
}

static void UpdateOurText() {
    if (g_ourTextBlock) {
        std::lock_guard<std::mutex> lock(g_textMutex);
        g_ourTextBlock.Text(winrt::hstring(g_currentText));
        g_textDirty = false;
    }
    RefreshCard();
}

// The fetch thread cannot touch XAML directly - every UI object here has
// thread affinity, and writing from the worker thread is an access violation
// waiting to happen. Marshal onto the UI thread instead.
static void PostTextUpdate() {
    if (!g_uiDispatcher) {
        return;
    }
    try {
        g_uiDispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            winrt::Windows::UI::Core::DispatchedHandler([] {
                try {
                    UpdateOurText();
                } catch (winrt::hresult_error const& error) {
                    Wh_Log(L"XAML text update FAILED: 0x%08X %s",
                           error.code().value, error.message().c_str());
                } catch (...) {
                    Wh_Log(L"XAML text update FAILED");
                }
            }));
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"XAML dispatch FAILED: 0x%08X %s",
               error.code().value, error.message().c_str());
    } catch (...) {
        Wh_Log(L"XAML dispatch FAILED");
    }
}

// ------------------------------------------- getting into the XAML tree --

// Taskbar Styler injects a XAML Diagnostics TAP to reach the visual tree.
// That is the general solution, and it is a lot of code. For our purposes
// there is a shortcut: the `this` pointer of a SystemTray XAML method already
// IS a FrameworkElement in the tree. Hook any such method, adopt `this`, and
// walk from there. No bridge required.

// Shared handler for hooks whose `this` really is a XAML ABI interface
// pointer. Only the property setters qualify - see the OnApplyTemplate note.
static void DumpFromAbi(void* abi, PCWSTR source) {
    if (g_inserted.load()) {
        return;
    }
    // During Explorer startup the frame height is often set before the tray's
    // children exist. Firing once and giving up would dump an empty tree and
    // never recover, so keep trying on later fires until the insert succeeds.
    if (g_ourTextBlock) {
        return;
    }

    static std::atomic<int> attempts{0};
    int attempt = ++attempts;
    if (attempt > 8) {
        return;  // something is wrong; stop flooding the log
    }

    Wh_Log(L"hook fired via %s (attempt %d)", source, attempt);

    FrameworkElement frame = nullptr;
    winrt::copy_from_abi(frame, abi);
    if (!frame) {
        Wh_Log(L"could not adopt the frame pointer");
        return;
    }

    // Full tree only on the first couple of attempts - it is long.
    if (g_settings.dumpDiagnostics && attempt <= 2) {
        Wh_Log(L"=== SystemTrayFrame subtree (attempt %d) ===", attempt);
        DumpVisualTree(frame);
        Wh_Log(L"=== end of subtree ===");
    }

    TryInsertIntoFrameGrid(frame);
}

// `this` on a produce<D,I> adapter is a COM interface pointer for interface I
// on object D. Adopting it as IUnknown is valid for any COM pointer, and
// try_as then does a real QueryInterface - no unchecked reinterpret_cast.
static void TryAttachFromProduce(void* abi, PCWSTR source) {
    if (g_inserted.load()) {
        return;
    }

    static std::atomic<int> attempts{0};
    int attempt = ++attempts;
    if (attempt > 40) {
        return;
    }

    winrt::Windows::Foundation::IUnknown unknown{nullptr};
    winrt::copy_from_abi(unknown, abi);
    auto frame = unknown.try_as<FrameworkElement>();
    if (!frame) {
        if (attempt <= 3) {
            Wh_Log(L"%s: not a FrameworkElement", source);
        }
        return;
    }

    if (attempt <= 3) {
        Wh_Log(L"attached via %s (attempt %d)", source, attempt);
    }

    if (g_settings.dumpDiagnostics && attempt <= 1) {
        Wh_Log(L"=== SystemTrayFrame subtree ===");
        DumpVisualTree(frame);
        Wh_Log(L"=== end of subtree ===");
    }

    TryInsertIntoFrameGrid(frame);
}

using ControlOverride_t = int(WINAPI*)(void*, void*);
ControlOverride_t SystemTrayFrame_OnPointerMoved_Original;
ControlOverride_t SystemTrayFrame_OnPointerPressed_Original;
ControlOverride_t SystemTrayFrame_OnPointerExited_Original;

int WINAPI SystemTrayFrame_OnPointerMoved_Hook(void* pThis, void* args) {
    int result = SystemTrayFrame_OnPointerMoved_Original(pThis, args);
    TryAttachFromProduce(pThis, L"OnPointerMoved");
    return result;
}

int WINAPI SystemTrayFrame_OnPointerPressed_Hook(void* pThis, void* args) {
    int result = SystemTrayFrame_OnPointerPressed_Original(pThis, args);
    TryAttachFromProduce(pThis, L"OnPointerPressed");
    return result;
}

int WINAPI SystemTrayFrame_OnPointerExited_Hook(void* pThis, void* args) {
    int result = SystemTrayFrame_OnPointerExited_Original(pThis, args);
    TryAttachFromProduce(pThis, L"OnPointerExited");
    return result;
}

using FrameDouble_t = void(WINAPI*)(void*, double);
FrameDouble_t SystemTrayFrame_Height_Original;
FrameDouble_t SystemTrayFrame_MinHeight_Original;

void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    SystemTrayFrame_Height_Original(pThis, value);
    DumpFromAbi(pThis, L"Height");
}

void WINAPI SystemTrayFrame_MinHeight_Hook(void* pThis, double value) {
    SystemTrayFrame_MinHeight_Original(pThis, value);
    DumpFromAbi(pThis, L"MinHeight");
}

using FrameVoid_t = void(WINAPI*)(void*);
FrameVoid_t SystemTrayFrame_OnApplyTemplate_Original;

// Kept separate on purpose. `this` here is the C++ implementation object, not
// an ABI interface pointer, so handing it to copy_from_abi would AddRef a
// non-COM address and crash Explorer. It only reports that it fired.
void WINAPI SystemTrayFrame_OnApplyTemplate_Hook(void* pThis) {
    SystemTrayFrame_OnApplyTemplate_Original(pThis);
    static bool logged = false;
    if (!logged) {
        logged = true;
        Wh_Log(L"OnApplyTemplate fired (impl `this`, not usable as ABI)");
    }
}

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void*);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;

void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    SystemTrayController_UpdateFrameSize_Original(pThis);
    static bool logged = false;
    if (!logged) {
        logged = true;
        Wh_Log(L"canary: UpdateFrameSize fired");
    }
}

static bool TryHookIn(HMODULE module, PCWSTR moduleName) {
    if (!SystemTrayFrame_Height_Original) {
        WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
            {
                {
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )",
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::implementation::SystemTrayFrame>::Height(double)const )",
                },
                (void**)&SystemTrayFrame_Height_Original,
                (void*)SystemTrayFrame_Height_Hook,
                true,
            },
        };
        WindhawkUtils::HookSymbols(
            module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
    }

    if (!SystemTrayFrame_MinHeight_Original) {
        WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
            {
                {
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::MinHeight(double)const )",
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::implementation::SystemTrayFrame>::MinHeight(double)const )",
                },
                (void**)&SystemTrayFrame_MinHeight_Original,
                (void*)SystemTrayFrame_MinHeight_Hook,
                true,
            },
        };
        WindhawkUtils::HookSymbols(
            module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
    }

    if (!SystemTrayFrame_OnApplyTemplate_Original) {
        WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
            {
                {
                    LR"(public: void __cdecl winrt::SystemTray::implementation::SystemTrayFrame::OnApplyTemplate(void))",
                },
                (void**)&SystemTrayFrame_OnApplyTemplate_Original,
                (void*)SystemTrayFrame_OnApplyTemplate_Hook,
                true,
            },
        };
        WindhawkUtils::HookSymbols(
            module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
    }

    if (!SystemTrayController_UpdateFrameSize_Original) {
        WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))",
                },
                (void**)&SystemTrayController_UpdateFrameSize_Original,
                (void*)SystemTrayController_UpdateFrameSize_Hook,
                true,
            },
        };
        WindhawkUtils::HookSymbols(
            module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
    }

    struct {
        PCWSTR name;
        std::wstring_view symbol;
        void** original;
        void* hook;
    } pointerHooks[] = {
        {L"OnPointerMoved",
         LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))",
         (void**)&SystemTrayFrame_OnPointerMoved_Original,
         (void*)SystemTrayFrame_OnPointerMoved_Hook},
        {L"OnPointerPressed",
         LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))",
         (void**)&SystemTrayFrame_OnPointerPressed_Original,
         (void*)SystemTrayFrame_OnPointerPressed_Hook},
        {L"OnPointerExited",
         LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))",
         (void**)&SystemTrayFrame_OnPointerExited_Original,
         (void*)SystemTrayFrame_OnPointerExited_Hook},
    };

    for (auto& entry : pointerHooks) {
        if (*entry.original) {
            continue;
        }
        WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
            {{entry.symbol}, entry.original, entry.hook, true},
        };
        WindhawkUtils::HookSymbols(
            module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
        Wh_Log(L"  %s -> %s", entry.name, *entry.original ? L"YES" : L"no");
    }

    Wh_Log(L"%s -> Height:%s MinHeight:%s OnApplyTemplate:%s canary:%s",
           moduleName,
           SystemTrayFrame_Height_Original ? L"YES" : L"no",
           SystemTrayFrame_MinHeight_Original ? L"YES" : L"no",
           SystemTrayFrame_OnApplyTemplate_Original ? L"YES" : L"no",
           SystemTrayController_UpdateFrameSize_Original ? L"YES" : L"no");

    return SystemTrayFrame_OnPointerMoved_Original != nullptr;
}

static void HookTrayFrame() {
    // SystemTray.dll first: on this build the tray is its own module, not part
    // of Taskbar.View.dll. GetModuleHandle rather than LoadLibraryEx because
    // these live under SystemApps, not System32, and they are already loaded
    // anyway - LoadLibraryEx was silently returning the loaded handle and
    // masking the fact that the path flag was wrong.
    PCWSTR moduleNames[] = {L"SystemTray.dll", L"Taskbar.View.dll",
                            L"Taskbar.dll"};

    for (PCWSTR name : moduleNames) {
        HMODULE module = GetModuleHandle(name);
        if (!module) {
            Wh_Log(L"%s not loaded, skipping", name);
            continue;
        }
        if (TryHookIn(module, name)) {
            return;
        }
    }

    Wh_Log(L"no usable SystemTray hook found in any tray module");
}

// ---------------------------------------------------- taskbar window hook --

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

std::atomic<bool> g_trayWindowSeen{false};

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent,
                                 HMENU menu, HINSTANCE instance,
                                 LPVOID param) {
    HWND result =
        CreateWindowExW_Original(exStyle, className, windowName, style, x, y,
                                 width, height, parent, menu, instance, param);

    if (result && className && !IS_INTRESOURCE(className) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0 &&
        !g_trayWindowSeen.exchange(true)) {
        Wh_Log(L"Shell_TrayWnd created: %p", result);
        // The XAML island is not ready at this instant. Taskbar Styler solves
        // this by injecting a XAML Diagnostics TAP here (InjectWindhawkTAP)
        // and waiting for the visual tree to report elements. Vendor that in
        // from windows-11-taskbar-styler.wh.cpp rather than reinventing it -
        // it is the load-bearing part of every mod that touches the tree.
    }

    return result;
}

// ------------------------------------------------------------- lifecycle --

BOOL Wh_ModInit() {
    // Build stamp: if this number does not change after you paste a new
    // version, Windhawk compiled the old source and nothing else in the log
    // means anything.
    Wh_Log(L"init - BUILD 0.10.0");
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    HookTrayFrame();

    if (!g_settings.dumpDiagnostics) {
        Wh_Log(L"DumpDiagnostics is off - visual tree logging is disabled");
    } else {
        Wh_Log(L"DumpDiagnostics is on - visual tree logging is enabled");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // The CreateWindowExW hook only fires for windows created after we load.
    // Enabling a mod on a running Explorer - the normal case - means the
    // taskbar already exists and the hook will never see it. Look directly.
    if (HWND existing = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        Wh_Log(L"Shell_TrayWnd already exists: %p", existing);
        g_trayWindowSeen = true;
        // The repeated SystemTray pointer hooks handle the normal case where
        // Explorer was already running before the mod was enabled.
    } else {
        Wh_Log(L"no Shell_TrayWnd yet, waiting for the hook");
    }
    StartFetchThread();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"settings changed");
    StopFetchThread();
    LoadSettings();
    StartFetchThread();
}

void Wh_ModUninit() {
    Wh_Log(L"uninit");
    StopFetchThread();

    // Actually remove the element rather than just blanking it, or every
    // recompile leaves another invisible child behind in Explorer's grid.
    // XAML has thread affinity, so this has to run on the UI thread.
    if (g_uiDispatcher && g_ourGrid) {
        auto grid = g_ourGrid;
        HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        g_uiDispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::High,
            winrt::Windows::UI::Core::DispatchedHandler([grid, done] {
                RemoveOurTextBlocks(grid);
                SetEvent(done);
            }));
        WaitForSingleObject(done, 2000);
        CloseHandle(done);
    }

    g_ourTextBlock = nullptr;
    g_ourGrid = nullptr;
    g_ourBorder = nullptr;
    g_uiDispatcher = nullptr;
    g_inserted = false;
}
