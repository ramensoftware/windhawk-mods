// ==WindhawkMod==
// @id              codex-usage-taskbar
// @name            Codex Usage Taskbar
// @description     Shows Codex usage limits and active/input-waiting session counts on the Windows 11 taskbar.
// @version         0.4.1
// @author          LazyHeidi
// @github          https://github.com/LazyHeidi
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Codex Usage Taskbar

Shows the Codex 5-hour and weekly usage limits in two rows on the left side of the Windows 11 taskbar.
It also shows Codex CLI activity detected from terminal tab/window titles.

Example:

```
5H 100%  ↻ 09/26 14:25 | 01h 32m  ● 2
W  100%  ↻ 09/02 09:25 | 01h 32m  ■
```

- `●` means a Codex CLI session is running.
- `■` means a Codex CLI session is waiting for input or approval.
- The action-required indicator pulses between full and 25% opacity to make it easier to notice.
- When two or more sessions are in the same state, the count is shown after the icon.
- When no session is in a state, that icon is hidden.

Windows Terminal tabs are inspected with UI Automation so background tabs are included. Classic console
windows are inspected from their window titles. Detection uses Codex's terminal-title `activity` item,
which is enabled by default. If `tui.terminal_title` is customized without `activity`/`spinner`, activity
cannot be detected by this mod.

Remaining-time display granularity:

- More than 24 hours: `05d 10h`
- 24 hours or less: `10h 55m`
- 1 hour or less: `55m` (for example, `05m` below 10 minutes)

Reads the access token from `$CODEX_HOME/auth.json` (or `%USERPROFILE%\\.codex\\auth.json`
when `CODEX_HOME` is not set) and queries the ChatGPT usage endpoint used by Codex
at the configured refresh interval (5 minutes by default). Codex activity is checked every five seconds
without making additional network requests.

If retrieval fails, stale values are cleared and the widget shows `5H %  ↻` / `W %  ↻`.
Repeated identical errors are suppressed; an error is logged only when the state changes.

This mod never writes authentication data. If Codex refreshes the token, the mod reads the updated
auth.json on the next refresh. Authentication data is cleared from memory after use.

Designed for the standard centered Windows 11 taskbar and injected into the taskbar on every monitor.

XAML root discovery and UI-thread execution are based on the implementation pattern used by
Windhawk Taskbar Folder Menus (GPL-3.0).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftMargin: 20
  $name: Left margin (px)
  $description: Space between the left edge of the taskbar and the widget.

- topFontSize: "12"
  $name: Top font size
  $description: Font size for the 5-hour limit row. Decimal values are allowed.

- bottomFontSize: "12"
  $name: Bottom font size
  $description: Font size for the weekly limit row. Decimal values are allowed.

- refreshIntervalMinutes: 5
  $name: Refresh interval (minutes)
  $description: Interval between Codex usage requests, in minutes.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <winhttp.h>
#include <UIAutomation.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cwchar>
#include <functional>
#include <string>
#include <utility>

#undef GetCurrentTime

// Keep the include structure consistent with official Windhawk Windows 11 taskbar mods.
// Collections.h is required for definitions such as Children().Append/RemoveAt.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

namespace {

constexpr wchar_t kWidgetName[] = L"CodexUsageWidget";
constexpr wchar_t kTopPercentName[] = L"CodexUsageTopPercent";
constexpr wchar_t kTopResetName[] = L"CodexUsageTopReset";
constexpr wchar_t kTopSeparatorName[] = L"CodexUsageTopSeparator";
constexpr wchar_t kTopRemainingName[] = L"CodexUsageTopRemaining";
constexpr wchar_t kTopStatusName[] = L"CodexUsageTopStatus";
constexpr wchar_t kBottomPercentName[] = L"CodexUsageBottomPercent";
constexpr wchar_t kBottomResetName[] = L"CodexUsageBottomReset";
constexpr wchar_t kBottomSeparatorName[] = L"CodexUsageBottomSeparator";
constexpr wchar_t kBottomRemainingName[] = L"CodexUsageBottomRemaining";
constexpr wchar_t kBottomStatusName[] = L"CodexUsageBottomStatus";
constexpr size_t kMaxHttpResponseBytes = 256 * 1024;
constexpr ULONGLONG kCodexStatusPollIntervalMs = 5000;
constexpr ULONGLONG kDisplayRefreshIntervalMs = 60 * 1000;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_refreshSettingsChanged{false};
HANDLE g_workerThread = nullptr;
HANDLE g_workerStopEvent = nullptr;
HANDLE g_workerWakeEvent = nullptr;
SRWLOCK g_usageLock = SRWLOCK_INIT;
SRWLOCK g_codexStatusLock = SRWLOCK_INIT;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

struct UsageLimitSnapshot {
    bool valid = false;
    int remainingPercent = 0;
    int64_t resetAt = 0;
};

struct UsageSnapshot {
    UsageLimitSnapshot fiveHour;
    UsageLimitSnapshot weekly;
};

struct CodexStatusSnapshot {
    int running = 0;
    int actionRequired = 0;
};

struct ModSettings {
    int leftMargin = 20;
    double topFontSize = 12.0;
    double bottomFontSize = 12.0;
    int refreshIntervalMinutes = 5;
};

enum class UsageError {
    None,
    AuthUnavailable,
    AuthInvalid,
    HttpRequest,
    HttpStatus,
    ResponseTooLarge,
    ParseFailed,
};

UsageSnapshot g_usage;
CodexStatusSnapshot g_codexStatus;
ModSettings g_settings;
UsageError g_lastUsageError = UsageError::None;
DWORD g_lastUsageErrorDetail = 0;

// -----------------------------------------------------------------------------
// Strings and files
// -----------------------------------------------------------------------------

std::wstring GetEnvironmentVariableString(const wchar_t* name) {
    DWORD required = GetEnvironmentVariableW(name, nullptr, 0);
    if (!required) {
        return {};
    }

    std::wstring value(required, L'\0');
    DWORD written = GetEnvironmentVariableW(name, value.data(), required);
    if (!written || written >= required) {
        return {};
    }

    value.resize(written);
    return value;
}

std::wstring GetCodexAuthPath() {
    std::wstring codexHome = GetEnvironmentVariableString(L"CODEX_HOME");
    if (codexHome.empty()) {
        codexHome = GetEnvironmentVariableString(L"USERPROFILE");
        if (codexHome.empty()) {
            return {};
        }
        codexHome += L"\\.codex";
    }

    if (!codexHome.empty() && codexHome.back() != L'\\' && codexHome.back() != L'/') {
        codexHome += L'\\';
    }
    codexHome += L"auth.json";
    return codexHome;
}

bool ReadUtf8File(const std::wstring& path, std::string* output) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 || size.QuadPart > 16 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }

    output->assign(static_cast<size_t>(size.QuadPart), '\0');
    DWORD totalRead = 0;
    while (totalRead < output->size()) {
        DWORD chunk = 0;
        DWORD requested = static_cast<DWORD>(
            std::min<size_t>(output->size() - totalRead, 1024 * 1024));
        if (!ReadFile(file, output->data() + totalRead, requested, &chunk, nullptr)) {
            CloseHandle(file);
            return false;
        }
        if (!chunk) {
            break;
        }
        totalRead += chunk;
    }
    CloseHandle(file);

    output->resize(totalRead);
    return true;
}

std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return {};
    }

    int required = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                       text.data(), static_cast<int>(text.size()),
                                       nullptr, 0);
    if (!required) {
        return {};
    }

    std::wstring result(required, L'\0');
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                             text.data(), static_cast<int>(text.size()),
                             result.data(), required)) {
        return {};
    }
    return result;
}


void SecureClearString(std::string* value) {
    if (!value || value->empty()) {
        return;
    }
    SecureZeroMemory(value->data(), value->size());
    value->clear();
}

void SecureClearWString(std::wstring* value) {
    if (!value || value->empty()) {
        return;
    }
    SecureZeroMemory(value->data(), value->size() * sizeof(wchar_t));
    value->clear();
}

double ReadDoubleSetting(const wchar_t* name, double fallback) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring value = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    if (value.empty()) {
        return fallback;
    }

    wchar_t* end = nullptr;
    double parsed = std::wcstod(value.c_str(), &end);
    if (end == value.c_str() || (end && *end != L'\0') || !std::isfinite(parsed)) {
        return fallback;
    }
    return parsed;
}

void LoadSettings() {
    ModSettings settings;
    settings.leftMargin = std::clamp(Wh_GetIntSetting(L"leftMargin"), 0, 500);
    settings.topFontSize = std::clamp(ReadDoubleSetting(L"topFontSize", 12.0), 6.0, 48.0);
    settings.bottomFontSize = std::clamp(ReadDoubleSetting(L"bottomFontSize", 12.0), 6.0, 48.0);
    settings.refreshIntervalMinutes = std::clamp(Wh_GetIntSetting(L"refreshIntervalMinutes"), 1, 1440);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = settings;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

ModSettings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

// -----------------------------------------------------------------------------
// Minimal JSON parsing
// Parse only the required fields without adding an external JSON library.
// -----------------------------------------------------------------------------

size_t SkipJsonWhitespace(const std::string& json, size_t pos) {
    while (pos < json.size()) {
        char c = json[pos];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            break;
        }
        ++pos;
    }
    return pos;
}

bool FindJsonKey(const std::string& json, const char* key,
                 size_t begin, size_t end, size_t* valuePos) {
    std::string needle = "\"";
    needle += key;
    needle += "\"";

    size_t pos = begin;
    while (pos < end) {
        pos = json.find(needle, pos);
        if (pos == std::string::npos || pos >= end) {
            return false;
        }

        size_t colon = SkipJsonWhitespace(json, pos + needle.size());
        if (colon < end && json[colon] == ':') {
            *valuePos = SkipJsonWhitespace(json, colon + 1);
            return *valuePos < end;
        }
        pos += needle.size();
    }
    return false;
}

bool FindMatchingJsonBrace(const std::string& json, size_t openPos,
                           size_t limit, size_t* closePos) {
    if (openPos >= limit || json[openPos] != '{') {
        return false;
    }

    int depth = 0;
    bool inString = false;
    bool escaped = false;

    for (size_t i = openPos; i < limit; ++i) {
        char c = json[i];
        if (inString) {
            if (escaped) {
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }

        if (c == '"') {
            inString = true;
        } else if (c == '{') {
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0) {
                *closePos = i + 1;
                return true;
            }
        }
    }
    return false;
}

bool FindJsonObjectRange(const std::string& json, const char* key,
                         size_t begin, size_t end,
                         size_t* objectBegin, size_t* objectEnd) {
    size_t valuePos = 0;
    if (!FindJsonKey(json, key, begin, end, &valuePos) ||
        valuePos >= end || json[valuePos] != '{') {
        return false;
    }

    size_t closePos = 0;
    if (!FindMatchingJsonBrace(json, valuePos, end, &closePos)) {
        return false;
    }

    *objectBegin = valuePos;
    *objectEnd = closePos;
    return true;
}

bool ExtractJsonString(const std::string& json, const char* key,
                       size_t begin, size_t end, std::string* result) {
    size_t valuePos = 0;
    if (!FindJsonKey(json, key, begin, end, &valuePos) ||
        valuePos >= end || json[valuePos] != '"') {
        return false;
    }

    ++valuePos;
    std::string out;
    bool escaped = false;

    for (size_t i = valuePos; i < end; ++i) {
        char c = json[i];
        if (escaped) {
            switch (c) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                default:
                    // Unicode escapes normally do not appear in access_token/account_id.
                    // Preserve the escaped character for unknown escape sequences.
                    out.push_back(c);
                    break;
            }
            escaped = false;
            continue;
        }

        if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            *result = std::move(out);
            return true;
        } else {
            out.push_back(c);
        }
    }
    return false;
}

bool ExtractJsonDouble(const std::string& json, const char* key,
                       size_t begin, size_t end, double* result) {
    size_t valuePos = 0;
    if (!FindJsonKey(json, key, begin, end, &valuePos)) {
        return false;
    }

    const char* start = json.c_str() + valuePos;
    char* finish = nullptr;
    double value = std::strtod(start, &finish);
    if (finish == start || static_cast<size_t>(finish - json.c_str()) > end) {
        return false;
    }

    *result = value;
    return true;
}

bool ExtractJsonInt64(const std::string& json, const char* key,
                      size_t begin, size_t end, int64_t* result) {
    size_t valuePos = 0;
    if (!FindJsonKey(json, key, begin, end, &valuePos)) {
        return false;
    }

    const char* start = json.c_str() + valuePos;
    char* finish = nullptr;
    long long value = std::strtoll(start, &finish, 10);
    if (finish == start || static_cast<size_t>(finish - json.c_str()) > end) {
        return false;
    }

    *result = static_cast<int64_t>(value);
    return true;
}

struct RateLimitWindow {
    bool valid = false;
    double usedPercent = 0;
    int64_t windowSeconds = 0;
    int64_t resetAt = 0;
};

RateLimitWindow ParseRateLimitWindow(const std::string& json,
                                     size_t begin, size_t end,
                                     const char* key) {
    RateLimitWindow window;
    size_t objectBegin = 0;
    size_t objectEnd = 0;
    if (!FindJsonObjectRange(json, key, begin, end, &objectBegin, &objectEnd)) {
        return window;
    }

    if (!ExtractJsonDouble(json, "used_percent", objectBegin, objectEnd,
                           &window.usedPercent) ||
        !ExtractJsonInt64(json, "limit_window_seconds", objectBegin, objectEnd,
                          &window.windowSeconds) ||
        !ExtractJsonInt64(json, "reset_at", objectBegin, objectEnd,
                          &window.resetAt)) {
        return window;
    }

    window.valid = window.windowSeconds > 0 && window.resetAt > 0;
    return window;
}

UsageLimitSnapshot ToUsageLimitSnapshot(const RateLimitWindow& window) {
    UsageLimitSnapshot result;
    if (!window.valid) {
        return result;
    }

    int remaining = static_cast<int>(std::lround(100.0 - window.usedPercent));
    result.valid = true;
    result.remainingPercent = std::clamp(remaining, 0, 100);
    result.resetAt = window.resetAt;
    return result;
}

bool ParseUsage(const std::string& json, UsageSnapshot* snapshot) {
    size_t rateLimitBegin = 0;
    size_t rateLimitEnd = 0;
    if (!FindJsonObjectRange(json, "rate_limit", 0, json.size(),
                             &rateLimitBegin, &rateLimitEnd)) {
        return false;
    }

    RateLimitWindow primary = ParseRateLimitWindow(
        json, rateLimitBegin, rateLimitEnd, "primary_window");
    RateLimitWindow secondary = ParseRateLimitWindow(
        json, rateLimitBegin, rateLimitEnd, "secondary_window");

    constexpr int64_t kFiveHourWindowSeconds = 5 * 60 * 60;
    constexpr int64_t kWeeklyWindowSeconds = 7 * 24 * 60 * 60;

    // Identify the 5-hour and weekly windows by duration, independent of primary/secondary order.
    const RateLimitWindow* windows[] = {&primary, &secondary};
    for (const RateLimitWindow* window : windows) {
        if (!window->valid) {
            continue;
        }
        if (window->windowSeconds == kFiveHourWindowSeconds) {
            snapshot->fiveHour = ToUsageLimitSnapshot(*window);
        } else if (window->windowSeconds == kWeeklyWindowSeconds) {
            snapshot->weekly = ToUsageLimitSnapshot(*window);
        }
    }

    // If only one window is available, display the one that was retrieved.
    return snapshot->fiveHour.valid || snapshot->weekly.valid;
}

// -----------------------------------------------------------------------------
// WinHTTP
// -----------------------------------------------------------------------------

struct WinHttpHandle {
    HINTERNET value = nullptr;
    ~WinHttpHandle() {
        if (value) {
            WinHttpCloseHandle(value);
        }
    }
    WinHttpHandle() = default;
    explicit WinHttpHandle(HINTERNET h) : value(h) {}
    WinHttpHandle(const WinHttpHandle&) = delete;
    WinHttpHandle& operator=(const WinHttpHandle&) = delete;
};

enum class HttpReadResult {
    Success,
    RequestFailed,
    ResponseTooLarge,
};

HttpReadResult ReadHttpResponseBody(HINTERNET request, std::string* body) {
    body->clear();
    for (;;) {
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) {
            return HttpReadResult::RequestFailed;
        }
        if (!available) {
            return HttpReadResult::Success;
        }

        if (body->size() > kMaxHttpResponseBytes ||
            available > kMaxHttpResponseBytes - body->size()) {
            body->clear();
            return HttpReadResult::ResponseTooLarge;
        }

        size_t oldSize = body->size();
        body->resize(oldSize + available);
        DWORD read = 0;
        if (!WinHttpReadData(request, body->data() + oldSize, available, &read)) {
            body->clear();
            return HttpReadResult::RequestFailed;
        }
        body->resize(oldSize + read);
        if (!read) {
            return HttpReadResult::Success;
        }
    }
}

HttpReadResult FetchUsageJson(const std::string& accessToken,
                              const std::string& accountId,
                              std::string* responseBody,
                              DWORD* statusCode) {
    WinHttpHandle session(WinHttpOpen(
        L"codex_cli_rs", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session.value) {
        return HttpReadResult::RequestFailed;
    }

    WinHttpSetTimeouts(session.value, 3000, 3000, 5000, 5000);

    WinHttpHandle connection(WinHttpConnect(
        session.value, L"chatgpt.com", INTERNET_DEFAULT_HTTPS_PORT, 0));
    if (!connection.value) {
        return HttpReadResult::RequestFailed;
    }

    WinHttpHandle request(WinHttpOpenRequest(
        connection.value, L"GET", L"/backend-api/wham/usage",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE));
    if (!request.value) {
        return HttpReadResult::RequestFailed;
    }

    std::wstring tokenWide = Utf8ToWide(accessToken);
    std::wstring accountWide = Utf8ToWide(accountId);
    std::wstring headers;
    // Avoid reallocating intermediate buffers that contain authentication data.
    headers.reserve(tokenWide.size() + accountWide.size() + 128);
    headers = L"Authorization: Bearer ";
    headers += tokenWide;
    headers += L"\r\n";
    if (!accountWide.empty()) {
        headers += L"ChatGPT-Account-ID: ";
        headers += accountWide;
        headers += L"\r\n";
    }
    headers += L"originator: codex_cli_rs\r\n";

    BOOL headersAdded = WinHttpAddRequestHeaders(
        request.value, headers.c_str(), static_cast<DWORD>(-1L),
        WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);

    // Clear plaintext authentication data after WinHTTP has copied the headers.
    SecureClearWString(&headers);
    SecureClearWString(&tokenWide);
    SecureClearWString(&accountWide);

    if (!headersAdded) {
        return HttpReadResult::RequestFailed;
    }

    if (!WinHttpSendRequest(request.value, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request.value, nullptr)) {
        return HttpReadResult::RequestFailed;
    }

    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if (!WinHttpQueryHeaders(request.value,
                             WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status,
                             &statusSize, WINHTTP_NO_HEADER_INDEX)) {
        return HttpReadResult::RequestFailed;
    }
    *statusCode = status;

    return ReadHttpResponseBody(request.value, responseBody);
}

enum class CredentialLoadResult {
    Success,
    Unavailable,
    Invalid,
};

CredentialLoadResult LoadCodexCredentials(std::string* accessToken,
                                          std::string* accountId) {
    std::wstring authPath = GetCodexAuthPath();
    if (authPath.empty()) {
        return CredentialLoadResult::Unavailable;
    }

    std::string json;
    if (!ReadUtf8File(authPath, &json)) {
        SecureClearString(&json);
        return CredentialLoadResult::Unavailable;
    }

    size_t tokensBegin = 0;
    size_t tokensEnd = 0;
    if (!FindJsonObjectRange(json, "tokens", 0, json.size(),
                             &tokensBegin, &tokensEnd)) {
        SecureClearString(&json);
        return CredentialLoadResult::Invalid;
    }

    if (!ExtractJsonString(json, "access_token", tokensBegin, tokensEnd,
                           accessToken) || accessToken->empty()) {
        SecureClearString(&json);
        return CredentialLoadResult::Invalid;
    }

    accountId->clear();
    ExtractJsonString(json, "account_id", tokensBegin, tokensEnd, accountId);
    // auth.json can contain refresh tokens, so do not leave the full contents in memory.
    SecureClearString(&json);
    return CredentialLoadResult::Success;
}

void InvalidateUsageSnapshot() {
    AcquireSRWLockExclusive(&g_usageLock);
    g_usage = UsageSnapshot{};
    ReleaseSRWLockExclusive(&g_usageLock);
}

void SetUsageError(UsageError error, DWORD detail = 0) {
    InvalidateUsageSnapshot();

    if (g_lastUsageError == error && g_lastUsageErrorDetail == detail) {
        return;
    }

    g_lastUsageError = error;
    g_lastUsageErrorDetail = detail;

    switch (error) {
        case UsageError::AuthUnavailable:
            Wh_Log(L"[Usage] auth.json not found/readable");
            break;
        case UsageError::AuthInvalid:
            Wh_Log(L"[Usage] auth.json is missing usable credentials");
            break;
        case UsageError::HttpRequest:
            Wh_Log(L"[Usage] HTTP request failed");
            break;
        case UsageError::HttpStatus:
            Wh_Log(L"[Usage] HTTP status %u", detail);
            break;
        case UsageError::ResponseTooLarge:
            Wh_Log(L"[Usage] HTTP response exceeded 256 KiB");
            break;
        case UsageError::ParseFailed:
            Wh_Log(L"[Usage] Failed to parse Codex rate limits");
            break;
        case UsageError::None:
            break;
    }
}

void ClearUsageError() {
    if (g_lastUsageError != UsageError::None) {
        Wh_Log(L"[Usage] Recovered");
    }
    g_lastUsageError = UsageError::None;
    g_lastUsageErrorDetail = 0;
}

bool RefreshUsageSnapshot() {
    std::string accessToken;
    std::string accountId;
    CredentialLoadResult credentials =
        LoadCodexCredentials(&accessToken, &accountId);
    if (credentials != CredentialLoadResult::Success) {
        SecureClearString(&accessToken);
        SecureClearString(&accountId);
        SetUsageError(credentials == CredentialLoadResult::Unavailable
                          ? UsageError::AuthUnavailable
                          : UsageError::AuthInvalid);
        return false;
    }

    std::string body;
    DWORD status = 0;
    HttpReadResult fetchResult =
        FetchUsageJson(accessToken, accountId, &body, &status);

    // Explicitly clear authentication data as soon as the request completes.
    SecureClearString(&accessToken);
    SecureClearString(&accountId);

    if (fetchResult == HttpReadResult::ResponseTooLarge) {
        SetUsageError(UsageError::ResponseTooLarge);
        return false;
    }
    if (fetchResult != HttpReadResult::Success) {
        SetUsageError(UsageError::HttpRequest);
        return false;
    }

    if (status != 200) {
        // Do not modify credentials; read Codex-updated auth.json again on the next refresh.
        SetUsageError(UsageError::HttpStatus, status);
        return false;
    }

    UsageSnapshot newSnapshot;
    if (!ParseUsage(body, &newSnapshot)) {
        SetUsageError(UsageError::ParseFailed);
        return false;
    }

    AcquireSRWLockExclusive(&g_usageLock);
    g_usage = newSnapshot;
    ReleaseSRWLockExclusive(&g_usageLock);
    ClearUsageError();
    return true;
}

UsageSnapshot GetUsageSnapshot() {
    AcquireSRWLockShared(&g_usageLock);
    UsageSnapshot snapshot = g_usage;
    ReleaseSRWLockShared(&g_usageLock);
    return snapshot;
}

// -----------------------------------------------------------------------------
// Codex terminal status
// -----------------------------------------------------------------------------

bool HasCodexSpinnerFrame(const std::wstring& title) {
    // Codex cycles through these Braille frames while the agent is running.
    static constexpr wchar_t kSpinnerFrames[] = L"⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";
    return title.find_first_of(kSpinnerFrames) != std::wstring::npos;
}

bool HasCodexActionRequiredTitle(const std::wstring& title) {
    // With animations enabled Codex alternates between [ ! ] and [ . ] once per second.
    return title.find(L"[ ! ] Action Required") != std::wstring::npos ||
           title.find(L"[ . ] Action Required") != std::wstring::npos;
}

void ClassifyCodexTerminalTitle(const std::wstring& title,
                                CodexStatusSnapshot* snapshot) {
    if (HasCodexActionRequiredTitle(title)) {
        ++snapshot->actionRequired;
    } else if (HasCodexSpinnerFrame(title)) {
        ++snapshot->running;
    }
}

bool ClassifyWindowTitle(HWND hWnd, CodexStatusSnapshot* snapshot) {
    wchar_t title[512]{};
    int length = GetWindowTextW(hWnd, title, ARRAYSIZE(title));
    if (length <= 0) {
        return false;
    }

    ClassifyCodexTerminalTitle(std::wstring(title, static_cast<size_t>(length)),
                               snapshot);
    return true;
}

bool ClassifyWindowsTerminalTabs(IUIAutomation* automation,
                                 HWND hWnd,
                                 CodexStatusSnapshot* snapshot) {
    if (!automation) {
        return false;
    }

    IUIAutomationElement* windowElement = nullptr;
    if (FAILED(automation->ElementFromHandle(hWnd, &windowElement)) ||
        !windowElement) {
        return false;
    }

    VARIANT controlType{};
    VariantInit(&controlType);
    controlType.vt = VT_I4;
    controlType.lVal = UIA_TabItemControlTypeId;

    IUIAutomationCondition* condition = nullptr;
    HRESULT hr = automation->CreatePropertyCondition(
        UIA_ControlTypePropertyId, controlType, &condition);
    VariantClear(&controlType);
    if (FAILED(hr) || !condition) {
        windowElement->Release();
        return false;
    }

    IUIAutomationElementArray* tabs = nullptr;
    hr = windowElement->FindAll(TreeScope_Descendants, condition, &tabs);
    condition->Release();
    windowElement->Release();
    if (FAILED(hr) || !tabs) {
        return false;
    }

    int tabCount = 0;
    if (FAILED(tabs->get_Length(&tabCount))) {
        tabs->Release();
        return false;
    }

    int inspectedTabs = 0;
    for (int i = 0; i < tabCount; ++i) {
        IUIAutomationElement* tab = nullptr;
        if (FAILED(tabs->GetElement(i, &tab)) || !tab) {
            continue;
        }

        BSTR name = nullptr;
        if (SUCCEEDED(tab->get_CurrentName(&name)) && name) {
            ClassifyCodexTerminalTitle(
                std::wstring(name, static_cast<size_t>(SysStringLen(name))),
                snapshot);
            SysFreeString(name);
            ++inspectedTabs;
        }
        tab->Release();
    }

    tabs->Release();
    return inspectedTabs > 0;
}

struct TerminalEnumerationContext {
    IUIAutomation* automation = nullptr;
    CodexStatusSnapshot snapshot;
};

BOOL CALLBACK EnumerateTerminalWindows(HWND hWnd, LPARAM lParam) {
    if (!IsWindowVisible(hWnd)) {
        return TRUE;
    }

    wchar_t className[64]{};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    auto* context = reinterpret_cast<TerminalEnumerationContext*>(lParam);
    if (_wcsicmp(className, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0) {
        // Read every tab, not only the active Windows Terminal title.
        if (!ClassifyWindowsTerminalTabs(context->automation, hWnd,
                                         &context->snapshot)) {
            ClassifyWindowTitle(hWnd, &context->snapshot);
        }
    } else if (_wcsicmp(className, L"ConsoleWindowClass") == 0) {
        // Covers the legacy console host when it is used instead of Windows Terminal.
        ClassifyWindowTitle(hWnd, &context->snapshot);
    }

    return TRUE;
}

bool RefreshCodexStatusSnapshot(IUIAutomation* automation) {
    TerminalEnumerationContext context;
    context.automation = automation;
    EnumWindows(EnumerateTerminalWindows, reinterpret_cast<LPARAM>(&context));

    AcquireSRWLockExclusive(&g_codexStatusLock);
    bool changed = g_codexStatus.running != context.snapshot.running ||
                   g_codexStatus.actionRequired != context.snapshot.actionRequired;
    g_codexStatus = context.snapshot;
    ReleaseSRWLockExclusive(&g_codexStatusLock);
    return changed;
}

CodexStatusSnapshot GetCodexStatusSnapshot() {
    AcquireSRWLockShared(&g_codexStatusLock);
    CodexStatusSnapshot snapshot = g_codexStatus;
    ReleaseSRWLockShared(&g_codexStatusLock);
    return snapshot;
}

// -----------------------------------------------------------------------------
// Display text
// -----------------------------------------------------------------------------

std::wstring FormatRemainingTime(int64_t resetAt, int64_t now) {
    int64_t seconds = std::max<int64_t>(0, resetAt - now);
    int64_t totalMinutes = (seconds + 59) / 60;  // Round up seconds to avoid showing 0m too early.

    wchar_t buffer[64]{};
    if (totalMinutes > 24 * 60) {
        int64_t days = totalMinutes / (24 * 60);
        int64_t hours = (totalMinutes % (24 * 60)) / 60;
        swprintf_s(buffer, ARRAYSIZE(buffer), L"%02lldd %02lldh",
                   static_cast<long long>(days),
                   static_cast<long long>(hours));
    } else if (totalMinutes > 60) {
        int64_t hours = totalMinutes / 60;
        int64_t minutes = totalMinutes % 60;
        swprintf_s(buffer, ARRAYSIZE(buffer), L"%02lldh %02lldm",
                   static_cast<long long>(hours),
                   static_cast<long long>(minutes));
    } else {
        swprintf_s(buffer, ARRAYSIZE(buffer), L"%02lldm", static_cast<long long>(totalMinutes));
    }
    return buffer;
}

struct UsageLineText {
    std::wstring percent;
    std::wstring reset;
    std::wstring separator;
    std::wstring remaining;
    std::wstring status;
};

std::wstring FormatCodexStatus(const wchar_t* icon, int count) {
    if (count <= 0) {
        return {};
    }

    std::wstring text = icon;
    if (count > 1) {
        text += L" ";
        text += std::to_wstring(count);
    }
    return text;
}

UsageLineText BuildUsageLineText(const UsageLimitSnapshot& usage, int64_t now) {
    UsageLineText line;
    line.percent = L"%";

    if (!usage.valid) {
        return line;
    }

    __time64_t resetTime = static_cast<__time64_t>(usage.resetAt);
    tm localReset{};
    if (_localtime64_s(&localReset, &resetTime) != 0) {
        return line;
    }

    wchar_t percentBuffer[16]{};
    swprintf_s(percentBuffer, ARRAYSIZE(percentBuffer), L"%d%%",
               usage.remainingPercent);
    line.percent = percentBuffer;

    wchar_t resetBuffer[32]{};
    swprintf_s(resetBuffer, ARRAYSIZE(resetBuffer), L"%02d/%02d %02d:%02d",
               localReset.tm_mon + 1, localReset.tm_mday,
               localReset.tm_hour, localReset.tm_min);
    line.reset = resetBuffer;
    line.separator = L"|";
    line.remaining = FormatRemainingTime(usage.resetAt, now);
    return line;
}

void BuildDisplayText(const UsageSnapshot& snapshot,
                      const CodexStatusSnapshot& codexStatus,
                      UsageLineText* top, UsageLineText* bottom) {
    __time64_t now = _time64(nullptr);
    *top = BuildUsageLineText(snapshot.fiveHour, now);
    *bottom = BuildUsageLineText(snapshot.weekly, now);
    top->status = FormatCodexStatus(L"●", codexStatus.running);
    bottom->status = FormatCodexStatus(L"■", codexStatus.actionRequired);
}

// -----------------------------------------------------------------------------
// Taskbar XAML
//
// XamlRoot discovery and UI-thread marshaling are based on the approach used by
// Windhawk Taskbar Folder Menus.
// -----------------------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        wchar_t className[32]{};
        if (GetWindowThreadProcessId(hWnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1] ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        if (taskbarHostSharedPtr[1] && std__Ref_count_base__Decref_Original) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t offset = 0x10;
#if defined(_M_X64)
    {
        const BYTE* bytes = reinterpret_cast<const BYTE*>(
            TaskbarHost_FrameHeight_Original);
        if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
            bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
            bytes[7] <= 0x7F) {
            offset = bytes[7];
        } else {
            Wh_Log(L"[XAML] Unsupported TaskbarHost::FrameHeight prologue");
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
            return nullptr;
        }
    }
#else
#error This mod currently supports x86-64 only.
#endif

    auto* unknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);
    if (!unknown) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(taskbarElement));
    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd = reinterpret_cast<HWND>(
        GetPropW(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForSite) !=
             CTaskBand_ITaskListWndSite_vftable;
         ++i) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (!CSecondaryTaskBand_GetTaskbarHost_Original ||
        !CSecondaryTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd = FindWindowExW(
        hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForSite) !=
             CSecondaryTaskBand_ITaskListWndSite_vftable;
         ++i) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(
        taskBandForSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void (*)(void*);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_codex-usage-taskbar");

    struct Param {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == RegisterWindowMessageW(
                        L"Windhawk_RunFromWindowThread_codex-usage-taskbar")) {
                    auto* param = reinterpret_cast<Param*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    Param param{proc, procParam};
    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

FrameworkElement FindChildRecursive(
    FrameworkElement const& element,
    const std::function<bool(FrameworkElement)>& predicate,
    int maxDepth = 20) {
    if (!element || maxDepth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (predicate(child)) {
            return child;
        }
        auto nested = FindChildRecursive(child, predicate, maxDepth - 1);
        if (nested) {
            return nested;
        }
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& xamlRootContent) {
    FrameworkElement taskbarFrame = nullptr;
    if (xamlRootContent &&
        winrt::get_class_name(xamlRootContent) == L"Taskbar.TaskbarFrame") {
        taskbarFrame = xamlRootContent;
    } else {
        taskbarFrame = FindChildRecursive(
            xamlRootContent,
            [](FrameworkElement element) {
                return winrt::get_class_name(element) == L"Taskbar.TaskbarFrame";
            });
    }
    if (!taskbarFrame) {
        return nullptr;
    }

    int childCount = VisualTreeHelper::GetChildrenCount(taskbarFrame);
    for (int i = 0; i < childCount; ++i) {
        auto child = VisualTreeHelper::GetChild(taskbarFrame, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == L"RootGrid") {
            return child.try_as<Grid>();
        }
    }

    // Fallback in case a Windows update adds another wrapper level.
    return FindChildRecursive(
               taskbarFrame,
               [](FrameworkElement element) {
                   return element.Name() == L"RootGrid";
               },
               4)
        .try_as<Grid>();
}

FrameworkElement FindNamedDirectChild(Grid const& parent, const wchar_t* name) {
    if (!parent) {
        return nullptr;
    }

    // Traverse with VisualTreeHelper instead of range-for over UIElementCollection.
    // This is less sensitive to C++/WinRT header differences in the Windhawk compiler.
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; ++i) {
        auto element = VisualTreeHelper::GetChild(parent, i)
                           .try_as<FrameworkElement>();
        if (element && element.Name() == name) {
            return element;
        }
    }
    return nullptr;
}

SolidColorBrush GetTaskbarTextBrush() {
    winrt::Windows::UI::Color color{255, 255, 255, 255};
    try {
        winrt::Windows::UI::ViewManagement::UISettings settings;
        color = settings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Foreground);
    } catch (...) {
        // Fall back to white if the taskbar foreground color cannot be retrieved.
    }
    return SolidColorBrush(color);
}

TextBlock CreateUsageCell(const wchar_t* name,
                              const std::wstring& text,
                              double fontSize,
                              SolidColorBrush const& brush,
                              int row,
                              int column,
                              Thickness margin) {
    TextBlock cell;
    if (name && *name) {
        cell.Name(name);
    }
    cell.Text(text);
    cell.FontSize(fontSize);
    cell.Foreground(brush);
    cell.TextWrapping(TextWrapping::NoWrap);
    cell.VerticalAlignment(VerticalAlignment::Center);
    cell.Margin(margin);
    Grid::SetRow(cell, row);
    Grid::SetColumn(cell, column);
    return cell;
}

void StartActionRequiredPulse(TextBlock const& cell) {
    if (winrt::unbox_value_or<bool>(cell.Tag(), false)) {
        return;
    }
    cell.Tag(winrt::box_value(true));

    Media::Animation::DoubleAnimation animation;
    animation.From(1.0);
    animation.To(0.25);
    animation.Duration(
        DurationHelper::FromTimeSpan(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(750)}));
    animation.AutoReverse(true);
    animation.RepeatBehavior(Media::Animation::RepeatBehaviorHelper::Forever());

    Media::Animation::Storyboard storyboard;
    storyboard.Children().Append(animation);
    Media::Animation::Storyboard::SetTarget(animation, cell);
    Media::Animation::Storyboard::SetTargetProperty(animation, L"Opacity");
    storyboard.Begin();
}

void AppendUsageRow(Grid const& grid,
                    int row,
                    double fontSize,
                    const wchar_t* label,
                    const UsageLineText& text,
                    const wchar_t* percentName,
                    const wchar_t* resetName,
                    const wchar_t* separatorName,
                    const wchar_t* remainingName,
                    const wchar_t* statusName,
                    SolidColorBrush const& brush) {
    // Put each field in a separate column so both rows align even with a proportional font.
    double bottomMargin = row == 0 ? -1.0 : 0.0;

    grid.Children().Append(CreateUsageCell(
        nullptr, label, fontSize, brush, row, 0,
        {0.0, 0.0, 0.0, bottomMargin}));
    auto percentCell = CreateUsageCell(
        percentName, text.percent, fontSize, brush, row, 1,
        {5.0, 0.0, 0.0, bottomMargin});
    percentCell.TextAlignment(TextAlignment::Right);
    grid.Children().Append(percentCell);
    grid.Children().Append(CreateUsageCell(
        nullptr, L"\u21BB", fontSize, brush, row, 2,
        {8.0, 0.0, 0.0, bottomMargin}));
    grid.Children().Append(CreateUsageCell(
        resetName, text.reset, fontSize, brush, row, 3,
        {4.0, 0.0, 0.0, bottomMargin}));
    grid.Children().Append(CreateUsageCell(
        separatorName, text.separator, fontSize, brush, row, 4,
        {7.0, 0.0, 7.0, bottomMargin}));
    grid.Children().Append(CreateUsageCell(
        remainingName, text.remaining, fontSize, brush, row, 5,
        {0.0, 0.0, 0.0, bottomMargin}));
    auto statusCell = CreateUsageCell(
        statusName, text.status, fontSize, brush, row, 6,
        {10.0, 0.0, 0.0, bottomMargin});
    grid.Children().Append(statusCell);
    if (row == 1 && !text.status.empty()) {
        StartActionRequiredPulse(statusCell);
    }
}

Grid CreateUsageWidget(const UsageLineText& topText,
                       const UsageLineText& bottomText) {
    ModSettings settings = GetSettingsSnapshot();

    Grid grid;
    grid.Name(kWidgetName);
    grid.HorizontalAlignment(HorizontalAlignment::Left);
    grid.VerticalAlignment(VerticalAlignment::Center);
    grid.Margin({static_cast<double>(settings.leftMargin), 0.0, 0.0, 0.0});
    grid.MinWidth(122.0);
    grid.IsHitTestVisible(false);
    Canvas::SetZIndex(grid, 1000);

    // Use seven auto-sized columns shared by both rows.
    // Shared columns align the percent, reset icon, date/time, separator, countdown, and status.
    for (int i = 0; i < 7; ++i) {
        ColumnDefinition column;
        column.Width(GridLength{0.0, GridUnitType::Auto});
        grid.ColumnDefinitions().Append(column);
    }
    for (int i = 0; i < 2; ++i) {
        RowDefinition row;
        row.Height(GridLength{0.0, GridUnitType::Auto});
        grid.RowDefinitions().Append(row);
    }

    auto brush = GetTaskbarTextBrush();
    AppendUsageRow(grid, 0, settings.topFontSize, L"5H", topText,
                   kTopPercentName, kTopResetName, kTopSeparatorName,
                   kTopRemainingName, kTopStatusName, brush);
    AppendUsageRow(grid, 1, settings.bottomFontSize, L"W", bottomText,
                   kBottomPercentName, kBottomResetName, kBottomSeparatorName,
                   kBottomRemainingName, kBottomStatusName, brush);
    return grid;
}

void UpdateExistingWidget(Grid const& grid,
                          const UsageLineText& topText,
                          const UsageLineText& bottomText) {
    ModSettings settings = GetSettingsSnapshot();
    grid.Margin({static_cast<double>(settings.leftMargin), 0.0, 0.0, 0.0});

    auto brush = GetTaskbarTextBrush();
    int childCount = VisualTreeHelper::GetChildrenCount(grid);
    for (int i = 0; i < childCount; ++i) {
        auto text = VisualTreeHelper::GetChild(grid, i).try_as<TextBlock>();
        if (!text) {
            continue;
        }

        int row = Grid::GetRow(text);
        text.FontSize(row == 0 ? settings.topFontSize : settings.bottomFontSize);
        text.Foreground(brush);

        auto name = text.Name();
        if (name == kTopPercentName) {
            text.Text(topText.percent);
        } else if (name == kTopResetName) {
            text.Text(topText.reset);
        } else if (name == kTopSeparatorName) {
            text.Text(topText.separator);
        } else if (name == kTopRemainingName) {
            text.Text(topText.remaining);
        } else if (name == kTopStatusName) {
            text.Text(topText.status);
        } else if (name == kBottomPercentName) {
            text.Text(bottomText.percent);
        } else if (name == kBottomResetName) {
            text.Text(bottomText.reset);
        } else if (name == kBottomSeparatorName) {
            text.Text(bottomText.separator);
        } else if (name == kBottomRemainingName) {
            text.Text(bottomText.remaining);
        } else if (name == kBottomStatusName) {
            bool startPulse = text.Text().empty() && !bottomText.status.empty();
            text.Text(bottomText.status);
            if (startPulse) {
                StartActionRequiredPulse(text);
            }
        }
    }
}

bool EnsureAndUpdateWidgetForXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return false;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        return false;
    }

    auto rootGrid = FindTaskbarRootGrid(content);
    if (!rootGrid) {
        return false;
    }

    UsageSnapshot snapshot = GetUsageSnapshot();
    CodexStatusSnapshot codexStatus = GetCodexStatusSnapshot();
    UsageLineText topText;
    UsageLineText bottomText;
    BuildDisplayText(snapshot, codexStatus, &topText, &bottomText);

    auto existingElement = FindNamedDirectChild(rootGrid, kWidgetName);
    auto existing = existingElement.try_as<Grid>();
    if (existing &&
        FindNamedDirectChild(existing, kTopStatusName) &&
        FindNamedDirectChild(existing, kBottomStatusName)) {
        UpdateExistingWidget(existing, topText, bottomText);
        return true;
    }

    // Replace a leftover widget from an older layout before adding the status column.
    if (existingElement) {
        int childCount = VisualTreeHelper::GetChildrenCount(rootGrid);
        for (int i = 0; i < childCount; ++i) {
            auto child = VisualTreeHelper::GetChild(rootGrid, i)
                             .try_as<FrameworkElement>();
            if (child && child.Name() == kWidgetName) {
                rootGrid.Children().RemoveAt(static_cast<uint32_t>(i));
                break;
            }
        }
    }

    Grid widget = CreateUsageWidget(topText, bottomText);
    Grid::SetColumn(widget, 0);
    Grid::SetColumnSpan(widget, 1024);
    Grid::SetRow(widget, 0);
    Grid::SetRowSpan(widget, 1024);
    rootGrid.Children().Append(widget);

    Wh_Log(L"[XAML] Widget injected");
    return true;
}

void RemoveWidgetForXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        return;
    }

    auto rootGrid = FindTaskbarRootGrid(content);
    if (!rootGrid) {
        return;
    }

    int childCount = VisualTreeHelper::GetChildrenCount(rootGrid);
    for (int i = 0; i < childCount; ++i) {
        auto element = VisualTreeHelper::GetChild(rootGrid, i)
                           .try_as<FrameworkElement>();
        if (element && element.Name() == kWidgetName) {
            rootGrid.Children().RemoveAt(static_cast<uint32_t>(i));
            Wh_Log(L"[XAML] Widget removed");
            return;
        }
    }
}

void ForEachTaskbarOnCurrentThread(bool remove) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            bool remove = lParam != 0;
            wchar_t className[32]{};
            if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                return TRUE;
            }

            try {
                if (remove) {
                    RemoveWidgetForXamlRoot(xamlRoot);
                } else {
                    EnsureAndUpdateWidgetForXamlRoot(xamlRoot);
                }
            } catch (...) {
                if (remove) {
                    Wh_Log(L"[XAML] Widget cleanup failed");
                } else {
                    Wh_Log(L"[XAML] Widget update failed");
                }
            }
            return TRUE;
        },
        remove ? 1 : 0);
}

void UpdateWidgetsOnWindowThread() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd || g_unloading) {
        return;
    }
    RunFromWindowThread(
        hWnd,
        [](void*) {
            if (!g_unloading) {
                ForEachTaskbarOnCurrentThread(false);
            }
        },
        nullptr);
}

void RemoveWidgetsOnWindowThread() {
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        return;
    }
    RunFromWindowThread(
        hWnd,
        [](void*) { ForEachTaskbarOnCurrentThread(true); },
        nullptr);
}

// -----------------------------------------------------------------------------
// Worker thread
// -----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(void*) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool shouldUninitializeCom = SUCCEEDED(comResult);

    IUIAutomation* automation = nullptr;
    if (SUCCEEDED(comResult) || comResult == RPC_E_CHANGED_MODE) {
        HRESULT automationResult = CoCreateInstance(
            __uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IUIAutomation), reinterpret_cast<void**>(&automation));
        if (FAILED(automationResult)) {
            Wh_Log(L"[Status] Failed to initialize UI Automation: 0x%08X",
                   static_cast<unsigned>(automationResult));
        }
    } else {
        Wh_Log(L"[Status] Failed to initialize COM: 0x%08X",
               static_cast<unsigned>(comResult));
    }

    ULONGLONG nextFetchAt = 0;
    ULONGLONG nextStatusPollAt = 0;
    ULONGLONG nextDisplayRefreshAt = 0;
    bool forceUpdate = true;
    HANDLE events[] = {g_workerStopEvent, g_workerWakeEvent};

    while (!g_unloading) {
        bool shouldUpdateWidget = forceUpdate;
        forceUpdate = false;

        if (g_refreshSettingsChanged.exchange(false)) {
            // Apply a changed refresh interval immediately.
            nextFetchAt = 0;
            shouldUpdateWidget = true;
        }

        ULONGLONG now = GetTickCount64();
        if (now >= nextFetchAt) {
            // Fetch before the first render to avoid briefly showing the empty placeholder.
            RefreshUsageSnapshot();
            ModSettings settings = GetSettingsSnapshot();
            const ULONGLONG refreshIntervalMs =
                static_cast<ULONGLONG>(settings.refreshIntervalMinutes) * 60 * 1000;
            nextFetchAt = GetTickCount64() + refreshIntervalMs;
            shouldUpdateWidget = true;
        }

        now = GetTickCount64();
        if (now >= nextStatusPollAt) {
            if (RefreshCodexStatusSnapshot(automation)) {
                shouldUpdateWidget = true;
            }
            nextStatusPollAt = GetTickCount64() + kCodexStatusPollIntervalMs;
        }

        now = GetTickCount64();
        if (now >= nextDisplayRefreshAt) {
            // The reset countdown has minute granularity, so a 60-second refresh is sufficient.
            nextDisplayRefreshAt = GetTickCount64() + kDisplayRefreshIntervalMs;
            shouldUpdateWidget = true;
        }

        if (shouldUpdateWidget) {
            UpdateWidgetsOnWindowThread();
        }

        now = GetTickCount64();
        ULONGLONG nextWakeAt = std::min({nextFetchAt, nextStatusPollAt,
                                        nextDisplayRefreshAt});
        DWORD timeout = nextWakeAt > now
                            ? static_cast<DWORD>(std::min<ULONGLONG>(
                                  nextWakeAt - now, MAXDWORD))
                            : 0;

        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events,
                                              FALSE, timeout);
        if (result == WAIT_OBJECT_0 || g_unloading) {
            break;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            // wakeEvent is auto-reset; retry injection immediately after taskbar rebuilds/settings changes.
            forceUpdate = true;
        }
    }

    if (automation) {
        automation->Release();
    }
    if (shouldUninitializeCom) {
        CoUninitialize();
    }
    return 0;
}

bool StartWorker() {
    if (g_workerThread) {
        return true;
    }

    g_workerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workerStopEvent || !g_workerWakeEvent) {
        if (g_workerStopEvent) CloseHandle(g_workerStopEvent);
        if (g_workerWakeEvent) CloseHandle(g_workerWakeEvent);
        g_workerStopEvent = nullptr;
        g_workerWakeEvent = nullptr;
        return false;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        CloseHandle(g_workerStopEvent);
        CloseHandle(g_workerWakeEvent);
        g_workerStopEvent = nullptr;
        g_workerWakeEvent = nullptr;
        return false;
    }
    return true;
}

void StopWorker() {
    if (g_workerStopEvent) {
        SetEvent(g_workerStopEvent);
    }

    if (g_workerThread) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(
                1, &g_workerThread, FALSE, INFINITE, QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);

        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_workerStopEvent) {
        CloseHandle(g_workerStopEvent);
        g_workerStopEvent = nullptr;
    }
    if (g_workerWakeEvent) {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Hook
// -----------------------------------------------------------------------------

using TrayUI_StartTaskbar_t = void (WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (!g_unloading && g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
    };

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Codex Usage Taskbar v0.4.1");
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] Failed to hook taskbar.dll symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!StartWorker()) {
        Wh_Log(L"[Init] Failed to start worker thread");
        return;
    }

    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"[Uninit]");
    g_unloading = true;
    StopWorker();
    RemoveWidgetsOnWindowThread();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_refreshSettingsChanged = true;
    if (g_workerWakeEvent) {
        SetEvent(g_workerWakeEvent);
    } else {
        UpdateWidgetsOnWindowThread();
    }
}
