// ==WindhawkMod==
// @id              system-tray-network-speedtest
// @name            Ookla Speed Test for Network Menu
// @description     Replaces the network menu's web speed test with a native flyout powered by the Ookla Speedtest CLI: live gauge, ping, jitter, download, upload and a test history.
// @version         1.0
// @author          joaomgabaldi
// @github          https://github.com/joaomgabaldi
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @include         SystemSettings.exe
// @include         RuntimeBroker.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lshlwapi -lshell32 -lole32 -ladvapi32 -luser32 -ld2d1 -ld3d11 -ldcomp -ldwrite -ldwmapi -liphlpapi -lshcore -luuid -loleacc -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ookla Speed Test for Network Menu

Right-clicking the network icon in the Windows 11 taskbar and choosing
**Run internet speed test** normally opens a web page in your browser. This mod
replaces it with a native flyout above the taskbar that runs the official
**Ookla Speedtest CLI** in the background and shows the test live.

## Screenshots

| Download | Upload | History |
| --- | --- | --- |
| ![Download](https://raw.githubusercontent.com/joaomgabaldi/windhawk-mods/assets/system-tray-network-speedtest/download.png) | ![Upload](https://raw.githubusercontent.com/joaomgabaldi/windhawk-mods/assets/system-tray-network-speedtest/upload.png) | ![History](https://raw.githubusercontent.com/joaomgabaldi/windhawk-mods/assets/system-tray-network-speedtest/history.png) |

## Features

- Live gauge (0 to 2,000+ Mbps) drawn as a soft light bar with smooth motion:
  blue for download, purple for upload.
- Ping, jitter, download and upload measured by Ookla. The server is always
  picked automatically.
- **Test history**: when a test finishes, the flyout moves to a list of the last
  100 tests (date and time, IP, server, download, upload, ping) with the new
  test on top. Click a row to open its result page on speedtest.net.
- **Copy** puts the result, including the speedtest.net link, on the clipboard.
- Follows the Windows light/dark theme, transparency, DPI and the
  "Animation effects" setting. Keyboard (Tab, arrows, Enter, Esc) and screen
  reader accessible.
- Interface in English or Portuguese, following the Windows display language.

## Requirements

1. Download the **Speedtest CLI for Windows** from
   [speedtest.net/apps/cli](https://www.speedtest.net/apps/cli) and extract
   `speedtest.exe`.
2. Put it at `%LOCALAPPDATA%\Programs\speedtest\speedtest.exe` (the default),
   or set its path in the mod settings.

## Ookla license and privacy

Before the first test, the flyout explains what Ookla receives and links
Ookla's [EULA](https://www.speedtest.net/about/eula),
[Terms of Use](https://www.speedtest.net/about/terms) and
[Privacy Policy](https://www.speedtest.net/about/privacy). No test runs until
you click **Accept and test**. Only then does the mod start `speedtest.exe`
with `--accept-license --accept-gdpr`. The acceptance is stored separately for
each Windows user.

The Speedtest CLI also records the acceptance in its own file,
`%APPDATA%\Ookla\Speedtest CLI\speedtest-cli.ini`, which stays after the mod
is removed. Delete that folder to reset it.

## Network access

The mod opens no connections itself. It runs `speedtest.exe`, which:

- contacts Ookla's servers to pick a test server, then downloads from and
  uploads to that Speedtest server;
- uploads the result to speedtest.net, where Ookla stores it (the result link
  in the history points there).

Ookla receives your public IP address, your internet provider and the test
results, under its privacy policy.

A test runs every time the flyout opens, and on Start, Repeat or Retry. Set
**Start the test automatically** to **Never** to make tests click-only (*Auto*
already waits for a click on metered connections). Clicking a history row or
a terms link opens speedtest.net in your browser.

## Data usage

Ookla does not cap a test: on a 2 Gbps line one test moves about 4-5 GB. With
**Start the test automatically** set to *Auto*, the flyout waits for a click
when Windows reports the connection as metered.

## Notes

- Stop, Esc, closing the flyout or disabling the mod ends `speedtest.exe`
  immediately (it runs inside a kill-on-close job object).
- The history lives in the mod's own Windhawk storage, separately for each
  Windows user, and is removed with the mod.
- [Network Menu Speed Test](https://windhawk.net/mods/network-speedtest-native)
  intercepts the same link. If both are installed, disable one of them.

## Credits

Based on [Network Menu Speed Test](https://windhawk.net/mods/network-speedtest-native)
by Alessandro-Vecchio, which provides the hooks and the original flyout. This
mod swaps its Cloudflare engine for the Ookla Speedtest CLI and adds the light
bar, the smooth motion and the test history.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- speedtestPath: '%LOCALAPPDATA%\Programs\speedtest\speedtest.exe'
  $name: speedtest.exe path
  $description: Full path of the Ookla Speedtest CLI. Environment variables such as %LOCALAPPDATA% are expanded.
- extraPatterns: ""
  $name: Extra URL patterns
  $description: Additional URL substrings to intercept, separated by | (example speedtest.net|ookla.com). Applies to links opened through the shell, not to direct browser launches.
- fallbackToOriginal: true
  $name: Fall back to the web page
  $description: If the native flyout cannot open, open the original web page instead of doing nothing.
- interceptProcessLaunch: true
  $name: Also intercept browser launches
  $description: Catch the network menu launching the browser directly. The launch returns ERROR_CANCELLED because no browser is opened.
- autoStart: auto
  $name: Start the test automatically
  $description: Starts the test when the flyout opens. Auto waits for a click on metered connections; Never always waits for a click.
  $options:
  - auto: Auto (waits on metered connections)
  - always: Always
  - never: Never (click to start)
*/
// ==/WindhawkModSettings==

// Portions derived from network-speedtest-native by Alessandro-Vecchio
// (https://windhawk.net/mods/network-speedtest-native), MIT license.

#include <windows.h>
// WIN32_LEAN_AND_MEAN (set in @compilerOptions) makes windows.h skip these:
// - shellapi.h: the hooks use SHELLEXECUTEINFOW/ShellExecuteW.
// - winsock2.h/ws2tcpip.h: iptypes.h (IP_ADAPTER_ADDRESSES) is built on the
//   Winsock address types. The mod itself never calls Winsock functions.
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <oleacc.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <netlistmgr.h>
#include <sddl.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <initializer_list>
#include <string>
#include <vector>
#include <cstdio>
#include <cwchar>
#include <new>
#include <atomic>

#include <windhawk_utils.h>

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
using ShellExecuteW_t = decltype(&ShellExecuteW);
using CreateProcessW_t = decltype(&CreateProcessW);

static ShellExecuteExW_t ShellExecuteExW_Original;
static ShellExecuteW_t ShellExecuteW_Original;
static CreateProcessW_t CreateProcessW_Original;

static std::vector<std::wstring> g_patterns;
static std::wstring g_speedtestPath;
// Settings are reloaded by Windhawk on its own thread while the hooks and the
// test thread read them from arbitrary threads.
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<bool> g_fallbackToOriginal = true;
static std::atomic<bool> g_interceptProcessLaunch = true;
// Start mode for the flyout: 0 = auto (start unless the connection is
// metered), 1 = always start, 2 = never start (wait for the Start button).
static std::atomic<int> g_autoStart = 0;

// ---------------------------------------------------------------------------
// UI strings
// ---------------------------------------------------------------------------

struct UiStrings {
    PCWSTR ready;
    PCWSTR starting;
    PCWSTR connecting;
    PCWSTR completed;
    PCWSTR canceled;
    PCWSTR stopping;
    PCWSTR error;
    PCWSTR ping;
    PCWSTR download;
    PCWSTR upload;
    PCWSTR jitter;
    PCWSTR copy;
    PCWSTR copied;
    PCWSTR start;
    PCWSTR stop;
    PCWSTR repeat;
    PCWSTR failedTitle;
    PCWSTR failedHint;
    PCWSTR retry;
    PCWSTR closeName;
    PCWSTR errNotFound;
    PCWSTR errNoResult;
    PCWSTR errCanceled;
    PCWSTR errUnexpected;
    PCWSTR shortcut;
    PCWSTR defaultAction;
    PCWSTR historyTitle;
    PCWSTR testSingular;
    PCWSTR testPlural;
    PCWSTR historyEmpty;
    PCWSTR viewResult;
    PCWSTR today;
    PCWSTR yesterday;
    PCWSTR dataSuffix;
    PCWSTR consentTitle;
    PCWSTR consentBody;
    PCWSTR consentLicense;
    PCWSTR consentTerms;
    PCWSTR consentPrivacy;
    PCWSTR consentAccept;
    PCWSTR accDownload;
    PCWSTR accUpload;
    PCWSTR accPing;
    PCWSTR locale;
    bool decimalComma;
    bool dayFirst;
};

static const UiStrings kStringsPt = {
    .ready = L"Pronto",
    .starting = L"Iniciando",
    .connecting = L"Escolhendo servidor",
    .completed = L"Concluído",
    .canceled = L"Cancelado",
    .stopping = L"Parando\x2026",
    .error = L"Erro",
    .ping = L"Ping",
    .download = L"Download",
    .upload = L"Upload",
    .jitter = L"Jitter",
    .copy = L"Copiar",
    .copied = L"Copiado",
    .start = L"Iniciar",
    .stop = L"Parar",
    .repeat = L"Repetir",
    .failedTitle = L"O teste falhou",
    .failedHint = L"Tente de novo em instantes.",
    .retry = L"Tentar de novo",
    .closeName = L"Fechar",
    .errNotFound = L"speedtest.exe não encontrado em:\n",
    .errNoResult = L"O Speedtest terminou sem resultado. Verifique a conexão e tente de novo.",
    .errCanceled = L"Teste cancelado.",
    .errUnexpected = L"Erro inesperado durante o teste.",
    .shortcut = L"Tab / Enter",
    .defaultAction = L"Ativar",
    .historyTitle = L"Hist\x00F3rico",
    .testSingular = L" teste",
    .testPlural = L" testes",
    .historyEmpty = L"Nenhum teste ainda.",
    .viewResult = L"Ver resultado",
    .today = L"Hoje",
    .yesterday = L"Ontem",
    .dataSuffix = L" de dados",
    .consentTitle = L"Antes do primeiro teste",
    .consentBody = L"O teste usa o Speedtest CLI da Ookla. Durante o teste, a Ookla recebe o seu IP, o seu provedor e os resultados, e guarda o resultado em speedtest.net. Para continuar, aceite a licen\u00E7a e a pol\u00EDtica de privacidade da Ookla.",
    .consentLicense = L"Licen\u00E7a",
    .consentTerms = L"Termos",
    .consentPrivacy = L"Privacidade",
    .consentAccept = L"Aceitar e testar",
    .accDownload = L", download ",
    .accUpload = L" Mbps, upload ",
    .accPing = L" Mbps, ping ",
    .locale = L"pt-BR",
    .decimalComma = true,
    .dayFirst = true,
};

static const UiStrings kStringsEn = {
    .ready = L"Ready",
    .starting = L"Starting",
    .connecting = L"Picking a server",
    .completed = L"Completed",
    .canceled = L"Canceled",
    .stopping = L"Stopping\x2026",
    .error = L"Error",
    .ping = L"Ping",
    .download = L"Download",
    .upload = L"Upload",
    .jitter = L"Jitter",
    .copy = L"Copy",
    .copied = L"Copied",
    .start = L"Start",
    .stop = L"Stop",
    .repeat = L"Repeat",
    .failedTitle = L"Test failed",
    .failedHint = L"Try again in a moment.",
    .retry = L"Retry",
    .closeName = L"Close",
    .errNotFound = L"speedtest.exe not found at:\n",
    .errNoResult = L"Speedtest ended without a result. Check the connection and try again.",
    .errCanceled = L"Test canceled.",
    .errUnexpected = L"Unexpected error during the test.",
    .shortcut = L"Tab / Enter",
    .defaultAction = L"Activate",
    .historyTitle = L"History",
    .testSingular = L" test",
    .testPlural = L" tests",
    .historyEmpty = L"No tests yet.",
    .viewResult = L"View result",
    .today = L"Today",
    .yesterday = L"Yesterday",
    .dataSuffix = L" of data",
    .consentTitle = L"Before your first test",
    .consentBody = L"This test uses the Ookla Speedtest CLI. During the test, Ookla receives your IP address, your internet provider and the results, and stores the result on speedtest.net. To continue, accept Ookla's license and privacy policy.",
    .consentLicense = L"License",
    .consentTerms = L"Terms",
    .consentPrivacy = L"Privacy",
    .consentAccept = L"Accept and test",
    .accDownload = L", download ",
    .accUpload = L" Mbps, upload ",
    .accPing = L" Mbps, ping ",
    .locale = L"en-US",
    .decimalComma = false,
    .dayFirst = false,
};

// Chosen once in Wh_ModInit, before any thread reads it: Portuguese when the
// Windows display language is Portuguese, English otherwise.
static const UiStrings* g_str = &kStringsEn;

static void SelectLanguage() {
    g_str = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE ? &kStringsPt
                                                                         : &kStringsEn;
}

static std::vector<std::wstring> SplitPatterns(PCWSTR text) {
    std::vector<std::wstring> result;
    std::wstring current;
    for (PCWSTR p = text; p && *p; p++) {
        if (*p == L'|') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else if (*p != L' ' && *p != L'\t') {
            current.push_back(*p);
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

static void LoadSettings() {
    auto extra = WindhawkUtils::StringSetting::make(L"extraPatterns");
    std::vector<std::wstring> patterns = SplitPatterns(extra.get());
    auto path = WindhawkUtils::StringSetting::make(L"speedtestPath");
    std::wstring speedtestPath = path.get();
    // Pasted paths often keep their quotes.
    speedtestPath.erase(std::remove(speedtestPath.begin(), speedtestPath.end(), L'"'),
                        speedtestPath.end());
    wchar_t expanded[MAX_PATH * 2];
    DWORD expandedLength =
        ExpandEnvironmentStringsW(speedtestPath.c_str(), expanded, ARRAYSIZE(expanded));
    if (expandedLength && expandedLength <= ARRAYSIZE(expanded)) {
        speedtestPath = expanded;
    }
    AcquireSRWLockExclusive(&g_settingsLock);
    g_patterns.swap(patterns);
    g_speedtestPath.swap(speedtestPath);
    ReleaseSRWLockExclusive(&g_settingsLock);

    g_fallbackToOriginal.store(Wh_GetIntSetting(L"fallbackToOriginal") != 0,
                               std::memory_order_relaxed);
    g_interceptProcessLaunch.store(
        Wh_GetIntSetting(L"interceptProcessLaunch") != 0,
        std::memory_order_relaxed);
    auto autoStart = WindhawkUtils::StringSetting::make(L"autoStart");
    int startMode = 0;
    if (wcscmp(autoStart.get(), L"always") == 0) {
        startMode = 1;
    } else if (wcscmp(autoStart.get(), L"never") == 0) {
        startMode = 2;
    }
    g_autoStart.store(startMode, std::memory_order_relaxed);
}

static std::wstring SpeedtestPath() {
    AcquireSRWLockShared(&g_settingsLock);
    std::wstring path = g_speedtestPath;
    ReleaseSRWLockShared(&g_settingsLock);
    return path;
}

// ---------------------------------------------------------------------------
// URL matching
// ---------------------------------------------------------------------------

static bool IsSpeedTestUrl(PCWSTR url) {
    if (!url || !*url) {
        return false;
    }

    // The links shipped by the Windows 11 network flyout/tray menu and the
    // page they redirect to. Keep this list narrow: the hook must never touch
    // ordinary web searches.
    static const PCWSTR kDefaultPatterns[] = {
        L"linkid=2325015",
        L"linkid=2324916",
        L"form=wspeed",
    };
    for (PCWSTR pattern : kDefaultPatterns) {
        if (StrStrIW(url, pattern)) {
            return true;
        }
    }

    // User-supplied extra patterns only apply to actual web URLs: the hooks
    // also see file names and command lines, and a file whose name contains
    // a pattern must not be cancelled.
    if (!(url[0] == L'h' || url[0] == L'H')) {
        return false;
    }
    if (!StrStrIW(url, L"://")) {
        return false;
    }

    bool matched = false;
    AcquireSRWLockShared(&g_settingsLock);
    for (const auto& pattern : g_patterns) {
        if (StrStrIW(url, pattern.c_str())) {
            matched = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_settingsLock);
    return matched;
}

// ---------------------------------------------------------------------------
// Flyout activation across the shell processes
// ---------------------------------------------------------------------------

static constexpr PCWSTR kFlyoutClass = L"SpeedTestFlyout_Ookla";
static UINT g_showMessage = 0;

static BOOL CALLBACK FindFlyoutCallback(HWND hwnd, LPARAM param) {
    wchar_t className[64] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, kFlyoutClass) == 0) {
        *(HWND*)param = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindFlyoutHost(DWORD* hostPid = nullptr) {
    // FindWindow is unreliable for this window class across shell processes,
    // so walk the top-level windows instead.
    HWND hwnd = nullptr;
    EnumWindows(FindFlyoutCallback, (LPARAM)&hwnd);
    if (hwnd && hostPid) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        *hostPid = pid;
    }
    return hwnd;
}

static bool ShowNativeFlyout() {
    // Runs inside a shell or process-launch hook: never wait for the UI here.
    if (!g_showMessage) {
        Wh_Log(L"Registered flyout message is unavailable");
        return false;
    }

    DWORD hostPid = 0;
    HWND hwnd = FindFlyoutHost(&hostPid);
    if (!hwnd) {
        Wh_Log(L"Flyout host window not found");
        return false;
    }
    // The foreground grant belongs to the process the click happened in; the
    // flyout lives in explorer.exe, so pass the grant on before posting.
    if (hostPid) {
        AllowSetForegroundWindow(hostPid);
    }
    if (!PostMessageW(hwnd, g_showMessage, 0, 0)) {
        Wh_Log(L"Flyout message post failed (error %u)", GetLastError());
        return false;
    }
    Wh_Log(L"Flyout requested");
    return true;
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* sei) {
    if (sei && IsSpeedTestUrl(sei->lpFile)) {
        Wh_Log(L"ShellExecuteExW match url=%s", sei->lpFile ? sei->lpFile : L"(null)");

        if (ShowNativeFlyout()) {
            if (sei->fMask & SEE_MASK_NOCLOSEPROCESS) {
                sei->hProcess = nullptr;
            }
            sei->hInstApp = (HINSTANCE)33;
            return TRUE;
        }

        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return FALSE;
        }
    }

    return ShellExecuteExW_Original(sei);
}

static HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation,
                                           LPCWSTR lpFile, LPCWSTR lpParameters,
                                           LPCWSTR lpDirectory, INT nShowCmd) {
    if (IsSpeedTestUrl(lpFile)) {
        Wh_Log(L"ShellExecuteW url=%s", lpFile);
        if (ShowNativeFlyout()) {
            return (HINSTANCE)33;
        }
        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return (HINSTANCE)ERROR_FILE_NOT_FOUND;
        }
    }

    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters,
                                  lpDirectory, nShowCmd);
}

static BOOL WINAPI CreateProcessW_Hook(LPCWSTR lpApplicationName,
                                       LPWSTR lpCommandLine,
                                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                       BOOL bInheritHandles, DWORD dwCreationFlags,
                                       LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
                                       LPSTARTUPINFOW lpStartupInfo,
                                       LPPROCESS_INFORMATION lpProcessInformation) {
    if (g_interceptProcessLaunch.load(std::memory_order_relaxed) &&
        (IsSpeedTestUrl(lpCommandLine) || IsSpeedTestUrl(lpApplicationName))) {
        Wh_Log(L"CreateProcessW match app=%s",
               lpApplicationName ? lpApplicationName : L"(null)");

        if (ShowNativeFlyout()) {
            if (lpProcessInformation) {
                *lpProcessInformation = {};
            }
            SetLastError(ERROR_CANCELLED);
            return FALSE;
        }

        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return FALSE;
        }
    }

    return CreateProcessW_Original(lpApplicationName, lpCommandLine,
                                   lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment,
                                   lpCurrentDirectory, lpStartupInfo,
                                   lpProcessInformation);
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

static constexpr float kWidth = 400.0f;
// Gauge and results share a height; the error card is much shorter, so the
// window animates between the two layouts instead of leaving an empty gap.
static constexpr float kGaugeHeight = 456.0f;
static constexpr float kErrorHeight = 340.0f;
static constexpr wchar_t kTextFamily[] = L"Segoe UI Variable Text";
static constexpr wchar_t kDisplayFamily[] = L"Segoe UI Variable Display";
static constexpr wchar_t kIconFamily[] = L"Segoe Fluent Icons";

static ULONGLONG Now() {
    return GetTickCount64();
}

static float Clamp01(float v) {
    return v < 0 ? 0 : (v > 1 ? 1 : v);
}

static double Clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Turns a printf "%.Nf" result into the UI locale: decimal mark replaced,
// thousands separator inserted in the integer part ("1500.5" -> "1.500,5").
static std::wstring GroupDigits(std::wstring text, wchar_t decimal, wchar_t thousands) {
    size_t dot = text.find(L'.');
    size_t integerEnd = dot == std::wstring::npos ? text.size() : dot;
    if (dot != std::wstring::npos) {
        text[dot] = decimal;
    }
    size_t start = !text.empty() && text[0] == L'-' ? 1 : 0;
    for (size_t i = integerEnd; i > start + 3;) {
        i -= 3;
        text.insert(i, 1, thousands);
    }
    return text;
}

static std::wstring Localize(const wchar_t* printfNumber) {
    return g_str->decimalComma ? GroupDigits(printfNumber, L',', L'.')
                               : GroupDigits(printfNumber, L'.', L',');
}

static std::wstring FormatNumber(double value) {
    wchar_t buffer[48];
    if (value >= 100) {
        swprintf(buffer, 48, L"%.0f", value);
    } else {
        swprintf(buffer, 48, L"%.1f", value);
    }
    return Localize(buffer);
}

static std::wstring FormatData(LONGLONG bytes) {
    if (bytes >= 1'000'000'000) {
        return FormatNumber(bytes / 1e9) + L" GB";
    }
    return FormatNumber(bytes / 1e6) + L" MB";
}

static std::wstring FormatScale(double value) {
    wchar_t buffer[48];
    swprintf(buffer, 48, L"%.0f", value);
    return Localize(buffer);
}

static std::wstring FormatDate(const SYSTEMTIME& t) {
    wchar_t buffer[24];
    if (g_str->dayFirst) {
        swprintf(buffer, 24, L"%02u/%02u/%04u %02u:%02u", t.wDay, t.wMonth,
                 t.wYear, t.wHour, t.wMinute);
    } else {
        swprintf(buffer, 24, L"%02u/%02u/%04u %02u:%02u", t.wMonth, t.wDay,
                 t.wYear, t.wHour, t.wMinute);
    }
    return buffer;
}

static std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return L"";
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    if (length <= 0) {
        return L"";
    }
    std::wstring result(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), result.data(), length);
    return result;
}

// ---------------------------------------------------------------------------
// Minimal JSON key-path lookup (one object per line, as the CLI prints it).
// shortcut: no array indexing and \u escapes outside the BMP become '?', enough for the Ookla CLI output.
// ---------------------------------------------------------------------------

// i points at an opening quote; returns the index just past the closing one.
static size_t JsonStringEnd(const std::string& s, size_t i) {
    for (size_t j = i + 1; j < s.size(); j++) {
        if (s[j] == '\\') {
            j++;
        } else if (s[j] == '"') {
            return j + 1;
        }
    }
    return std::string::npos;
}

static size_t JsonSkipSpace(const std::string& s, size_t i) {
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) {
        i++;
    }
    return i;
}

// Walks nested objects; each key is matched only among the direct members
// of the current object, so {"upload","latency"} never finds download's.
static bool JsonFindValue(const std::string& s, std::initializer_list<const char*> path,
                          size_t* valueStart) {
    size_t pos = JsonSkipSpace(s, 0);
    for (const char* key : path) {
        if (pos >= s.size() || s[pos] != '{') {
            return false;
        }
        bool found = false;
        int depth = 0;
        for (size_t i = pos; i < s.size() && !found; i++) {
            char c = s[i];
            if (c == '"') {
                size_t end = JsonStringEnd(s, i);
                if (end == std::string::npos) {
                    return false;
                }
                size_t colon = JsonSkipSpace(s, end);
                if (depth == 1 && colon < s.size() && s[colon] == ':' &&
                    s.compare(i + 1, end - i - 2, key) == 0) {
                    pos = JsonSkipSpace(s, colon + 1);
                    found = true;
                }
                i = end - 1;
            } else if (c == '{' || c == '[') {
                depth++;
            } else if (c == '}' || c == ']') {
                if (--depth == 0) {
                    break;
                }
            }
        }
        if (!found) {
            return false;
        }
    }
    *valueStart = pos;
    return true;
}

static bool JsonNumber(const std::string& s, std::initializer_list<const char*> path, double* out) {
    size_t start = 0;
    if (!JsonFindValue(s, path, &start)) {
        return false;
    }
    const char* begin = s.c_str() + start;
    char* end = nullptr;
    double value = std::strtod(begin, &end);
    if (end == begin) {
        return false;
    }
    *out = value;
    return true;
}

// Returns the UTF-8 string value with JSON escapes decoded.
static bool JsonString(const std::string& s, std::initializer_list<const char*> path,
                       std::string* out) {
    size_t start = 0;
    if (!JsonFindValue(s, path, &start) || start >= s.size() || s[start] != '"') {
        return false;
    }
    size_t end = JsonStringEnd(s, start);
    if (end == std::string::npos) {
        return false;
    }
    std::string result;
    for (size_t i = start + 1; i + 1 < end; i++) {
        char c = s[i];
        if (c != '\\') {
            result.push_back(c);
            continue;
        }
        char e = s[++i];
        switch (e) {
            case 'n': result.push_back('\n'); break;
            case 't': result.push_back('\t'); break;
            case 'r': result.push_back('\r'); break;
            case 'b': result.push_back('\b'); break;
            case 'f': result.push_back('\f'); break;
            case 'u': {
                unsigned cp = i + 4 < end ? (unsigned)std::strtoul(s.substr(i + 1, 4).c_str(), nullptr, 16) : '?';
                i += 4;
                if (cp >= 0xD800 && cp <= 0xDFFF) {
                    cp = '?';
                }
                if (cp < 0x80) {
                    result.push_back((char)cp);
                } else if (cp < 0x800) {
                    result.push_back((char)(0xC0 | (cp >> 6)));
                    result.push_back((char)(0x80 | (cp & 0x3F)));
                } else {
                    result.push_back((char)(0xE0 | (cp >> 12)));
                    result.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
                    result.push_back((char)(0x80 | (cp & 0x3F)));
                }
                break;
            }
            default: result.push_back(e); break;  // \" \\ \/
        }
    }
    *out = result;
    return true;
}

// ---------------------------------------------------------------------------
// Motion: critically damped spring and timeline easing
// ---------------------------------------------------------------------------

struct Spring {
    double x = 0;
    double v = 0;
};

// Exact solution of a critically damped spring (omega 9 rad/s): no overshoot
// from rest and stable for any step, so a slow frame never makes it jump.
static void SpringStep(Spring& s, double target, double dt) {
    constexpr double kOmega = 9.0;
    dt = dt < 0 ? 0 : (dt > 0.05 ? 0.05 : dt);
    double c1 = s.x - target;
    double c2 = s.v + kOmega * c1;
    double decay = std::exp(-kOmega * dt);
    s.x = target + (c1 + c2 * dt) * decay;
    s.v = (c2 - kOmega * (c1 + c2 * dt)) * decay;
}

// True when the spring has visually arrived; snaps it exactly onto target.
static bool SpringSettle(Spring& s, double target, double eps) {
    if (std::fabs(s.x - target) < eps && std::fabs(s.v) < eps * 10) {
        s.x = target;
        s.v = 0;
        return true;
    }
    return false;
}

static double EaseOutCubic(double t) {
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    double inv = 1 - t;
    return 1 - inv * inv * inv;
}

// Progress of a timeline segment [startMs, startMs + durMs], clamped to 0..1.
static double Phase01(double nowMs, double startMs, double durMs) {
    if (durMs <= 0) {
        return nowMs >= startMs ? 1 : 0;
    }
    double t = (nowMs - startMs) / durMs;
    return t < 0 ? 0 : (t > 1 ? 1 : t);
}

// ---------------------------------------------------------------------------
// History: one record per test (ends with U+001E), TAB-separated, newest first
// ---------------------------------------------------------------------------

struct HistoryEntry {
    ULONGLONG time;  // local time as FILETIME ticks
    std::wstring ip;
    std::wstring server;
    std::wstring url;
    double download;
    double upload;
    double ping;
    double jitter;
};

static constexpr size_t kHistoryMax = 100;

static std::wstring HistoryClean(const std::wstring& text) {
    std::wstring result = text;
    for (wchar_t& c : result) {
        if (c == L'\t' || c == L'\r' || c == L'\n' || c == L'\x1E') {
            c = L' ';
        }
    }
    return result;
}

static std::wstring SerializeHistory(const std::vector<HistoryEntry>& entries) {
    std::wstring text;
    for (const HistoryEntry& e : entries) {
        wchar_t numbers[160];
        swprintf(numbers, 160, L"%.1f\t%.1f\t%.3f\t%.3f", e.download, e.upload, e.ping, e.jitter);
        text += std::to_wstring(e.time) + L"\t" + HistoryClean(e.ip) + L"\t" +
                HistoryClean(e.server) + L"\t" + HistoryClean(e.url) + L"\t" + numbers + L"\x1E";
    }
    return text;
}

// Malformed lines are skipped so one bad line never loses the rest.
static std::vector<HistoryEntry> ParseHistory(const std::wstring& text) {
    std::vector<HistoryEntry> entries;
    size_t start = 0;
    while (start < text.size() && entries.size() < kHistoryMax) {
        size_t end = text.find(L'\x1E', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::vector<std::wstring> fields;
        size_t fieldStart = start;
        for (size_t i = start; i <= end; i++) {
            if (i == end || text[i] == L'\t') {
                fields.push_back(text.substr(fieldStart, i - fieldStart));
                fieldStart = i + 1;
            }
        }
        start = end + 1;
        if (fields.size() != 8) {
            continue;
        }
        HistoryEntry e = {};
        wchar_t* stop = nullptr;
        e.time = wcstoull(fields[0].c_str(), &stop, 10);
        if (!e.time || *stop) {
            continue;
        }
        e.ip = fields[1];
        e.server = fields[2];
        e.url = fields[3];
        double* numbers[] = {&e.download, &e.upload, &e.ping, &e.jitter};
        bool ok = true;
        for (int n = 0; n < 4 && ok; n++) {
            *numbers[n] = wcstod(fields[4 + n].c_str(), &stop);
            ok = stop != fields[4 + n].c_str() && !*stop;
        }
        if (ok) {
            entries.push_back(e);
        }
    }
    return entries;
}

static void PushHistory(std::vector<HistoryEntry>& entries, HistoryEntry entry,
                        size_t max = kHistoryMax) {
    entries.insert(entries.begin(), std::move(entry));
    if (entries.size() > max) {
        entries.resize(max);
    }
}

// Storage value names are per Windows user: the mod's storage is machine-wide,
// and each user's Explorer hosts its own flyout and history.
static std::wstring StorageName(PCWSTR base, const std::wstring& sid) {
    return sid.empty() ? std::wstring(base) : std::wstring(base) + L"-" + sid;
}

// "Today · 03:16", "Yesterday · 21:42" or a short date ("17/11 · 09:05" day
// first, "11/17 · 09:05" month first). Both times are local.
static std::wstring DayLabel(ULONGLONG entryTime, ULONGLONG nowTime, PCWSTR today,
                             PCWSTR yesterday, bool dayFirst) {
    constexpr ULONGLONG kTicksPerDay = 864000000000ULL;
    FILETIME ft = {(DWORD)entryTime, (DWORD)(entryTime >> 32)};
    SYSTEMTIME t = {};
    FileTimeToSystemTime(&ft, &t);
    long long days = (long long)(nowTime / kTicksPerDay) - (long long)(entryTime / kTicksPerDay);
    wchar_t buffer[64];
    if (days == 0 || days == 1) {
        swprintf(buffer, 64, L"%ls \x00B7 %02u:%02u", days == 0 ? today : yesterday, t.wHour,
                 t.wMinute);
    } else {
        swprintf(buffer, 64, L"%02u/%02u \x00B7 %02u:%02u", dayFirst ? t.wDay : t.wMonth,
                 dayFirst ? t.wMonth : t.wDay, t.wHour, t.wMinute);
    }
    return buffer;
}

// DayLabel in the UI language.
static std::wstring EntryDay(ULONGLONG entryTime, ULONGLONG nowTime) {
    return DayLabel(entryTime, nowTime, g_str->today, g_str->yesterday, g_str->dayFirst);
}

static ULONGLONG LocalTicks(const SYSTEMTIME& t) {
    FILETIME ft = {};
    SystemTimeToFileTime(&t, &ft);
    return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

// String SID of the user this process runs as ("" if it cannot be read).
static std::wstring CurrentUserSid() {
    std::wstring result;
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return result;
    }
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    PWSTR text = nullptr;
    if (size && GetTokenInformation(token, TokenUser, buffer.data(), size, &size) &&
        ConvertSidToStringSidW(((TOKEN_USER*)buffer.data())->User.Sid, &text)) {
        result = text;
        LocalFree(text);
    }
    CloseHandle(token);
    return result;
}

// Set once in Wh_ModInit. History and the Ookla consent are stored per user.
static std::wstring g_userSid;

// Loaded once on the UI thread. Only the primary explorer of this user hosts
// the flyout, so it is the single writer of this user's history value.
static std::vector<HistoryEntry> g_history;

static void LoadHistory() {
    // shortcut: 128K chars covers 100 entries (~22K) with room; Windhawk returns "" if it overflows.
    std::vector<wchar_t> buffer(128 * 1024);
    size_t length = Wh_GetStringValue(StorageName(L"history", g_userSid).c_str(), buffer.data(),
                                      buffer.size());
    g_history = ParseHistory(std::wstring(buffer.data(), std::min(length, buffer.size() - 1)));
}

// The user accepted Ookla's license and privacy policy in the flyout.
static bool HasOoklaConsent() {
    return Wh_GetIntValue(StorageName(L"consent", g_userSid).c_str(), 0) == 1;
}

static bool SaveOoklaConsent() {
    if (!Wh_SetIntValue(StorageName(L"consent", g_userSid).c_str(), 1)) {
        Wh_Log(L"Saving the Ookla consent failed");
        return false;
    }
    return true;
}

static void SaveHistory() {
    if (!Wh_SetStringValue(StorageName(L"history", g_userSid).c_str(),
                           SerializeHistory(g_history).c_str())) {
        Wh_Log(L"Saving the history failed");
    }
}

// ---------------------------------------------------------------------------
// Palette
// ---------------------------------------------------------------------------

struct Palette {
    D2D1_COLOR_F background;
    D2D1_COLOR_F text;
    D2D1_COLOR_F secondary;
    D2D1_COLOR_F tertiary;
    D2D1_COLOR_F cardFill;
    D2D1_COLOR_F cardStroke;
    D2D1_COLOR_F controlFill;
    D2D1_COLOR_F track;
    D2D1_COLOR_F download;
    D2D1_COLOR_F upload;
    D2D1_COLOR_F jitter;
    D2D1_COLOR_F positive;
    D2D1_COLOR_F negative;
    D2D1_COLOR_F accent;
    D2D1_COLOR_F accentText;
    D2D1_COLOR_F label;
};

static D2D1_COLOR_F Col(UINT32 rgb, float alpha = 1.0f) {
    D2D1_COLOR_F c;
    c.r = ((rgb >> 16) & 0xFF) / 255.0f;
    c.g = ((rgb >> 8) & 0xFF) / 255.0f;
    c.b = (rgb & 0xFF) / 255.0f;
    c.a = alpha;
    return c;
}

static D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float alpha) {
    c.a *= alpha;
    return c;
}

static D2D1_COLOR_F Lighten(D2D1_COLOR_F c, float amount) {
    c.r += (1.0f - c.r) * amount;
    c.g += (1.0f - c.g) * amount;
    c.b += (1.0f - c.b) * amount;
    return c;
}

// Pressed controls get a darker fill in Fluent; Lighten() cannot do that.
static D2D1_COLOR_F Shade(D2D1_COLOR_F c, float amount) {
    c.r *= (1.0f - amount);
    c.g *= (1.0f - amount);
    c.b *= (1.0f - amount);
    return c;
}

// The shell flyouts follow the *system* theme, and the DWM backdrop material is
// tinted from the system palette as well.
static bool ReadSystemLightTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

static bool ReadTransparencyEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

static DWORD WindowsBuild() {
    static DWORD build = 0;
    if (!build) {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
        OSVERSIONINFOW info = {};
        info.dwOSVersionInfoSize = sizeof(info);
        build = (rtlGetVersion && rtlGetVersion(&info) == 0 && info.dwBuildNumber)
                    ? info.dwBuildNumber
                    : 1;
    }
    return build;
}

// DWMWA_SYSTEMBACKDROP_TYPE only exists from build 22621, and DWM draws no
// material when composition is off, on remote sessions or in high contrast.
static bool MaterialSupported() {
    if (WindowsBuild() < 22621) {
        return false;
    }
    BOOL composition = FALSE;
    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition) {
        return false;
    }
    if (GetSystemMetrics(SM_REMOTESESSION)) {
        return false;
    }
    HIGHCONTRASTW contrast = {sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        return false;
    }
    return true;
}

static Palette MakePalette(bool light) {
    Palette p = {};
    if (light) {
        p.background = Col(0xF3F3F3);
        p.text = Col(0x1A1A1A);
        p.secondary = Col(0x5D5D5D);
        p.tertiary = Col(0x767676);
        p.cardFill = Col(0xFFFFFF, 0.70f);
        p.cardStroke = Col(0x000000, 0.06f);
        p.controlFill = Col(0x000000, 0.04f);
        p.track = Col(0xE4E4E4);
        p.download = Col(0x0B6BCB);
        p.upload = Col(0x6B3FA0);
        p.jitter = Col(0x0F6E66);
        p.positive = Col(0x0F7B0F);
        p.negative = Col(0xC42B1C);
        p.accent = Col(0x005FB8);
        p.accentText = Col(0xFFFFFF);
        p.label = Col(0x6B6B6B);
    } else {
        p.background = Col(0x2C2C2C);
        p.text = Col(0xFFFFFF);
        p.secondary = Col(0xC5C5C5);
        p.tertiary = Col(0x8B8B8B);
        p.cardFill = Col(0xFFFFFF, 0.053f);
        p.cardStroke = Col(0xFFFFFF, 0.086f);
        p.controlFill = Col(0xFFFFFF, 0.06f);
        p.track = Col(0x3A3A3A);
        p.download = Col(0x6CB4FF);
        p.upload = Col(0xC9A0FF);
        p.jitter = Col(0x5ED8CC);
        p.positive = Col(0x6CCB5F);
        p.negative = Col(0xFF99A4);
        p.accent = Col(0x4CC2FF);
        p.accentText = Col(0x000000);
        p.label = Col(0xA0A0A0);
    }
    return p;
}

// ---------------------------------------------------------------------------
// Network name for the footline
// ---------------------------------------------------------------------------

struct NetworkInfo {
    std::wstring name;
    const wchar_t* glyph = L"\xE774";
};

static PIP_ADAPTER_ADDRESSES GetAdapterAddressesList(
    std::vector<BYTE>* buffer) {
    ULONG size = (ULONG)buffer->size();
    ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST |
                  GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    for (int attempt = 0; attempt < 3; attempt++) {
        buffer->resize(size);
        ULONG result = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                            (PIP_ADAPTER_ADDRESSES)buffer->data(),
                                            &size);
        if (result == ERROR_SUCCESS) {
            return (PIP_ADAPTER_ADDRESSES)buffer->data();
        }
        if (result != ERROR_BUFFER_OVERFLOW) {
            return nullptr;
        }
    }
    return nullptr;
}

// Profile name for the footline: SSID on Wi-Fi, the connection name on
// Ethernet - what the Windows network flyout shows.
static std::wstring NetworkProfileName(PIP_ADAPTER_ADDRESSES adapter) {
    INetworkListManager* manager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&manager));
    if (FAILED(hr)) {
        return L"";
    }

    std::wstring name;
    wchar_t adapterName[64] = {};
    GUID adapterGuid = {};
    if (adapter->AdapterName &&
        MultiByteToWideChar(CP_ACP, 0, adapter->AdapterName, -1, adapterName,
                            64) > 0 &&
        SUCCEEDED(CLSIDFromString(adapterName, &adapterGuid))) {
        IEnumNetworkConnections* enumerator = nullptr;
        if (SUCCEEDED(manager->GetNetworkConnections(&enumerator))) {
            INetworkConnection* connection = nullptr;
            while (enumerator->Next(1, &connection, nullptr) == S_OK) {
                VARIANT_BOOL connected = VARIANT_FALSE;
                GUID id = {};
                if (connection->get_IsConnected(&connected) == S_OK &&
                    connected != VARIANT_FALSE &&
                    connection->GetAdapterId(&id) == S_OK &&
                    IsEqualGUID(id, adapterGuid)) {
                    INetwork* network = nullptr;
                    BSTR connectionName = nullptr;
                    if (connection->GetNetwork(&network) == S_OK && network) {
                        if (network->GetName(&connectionName) == S_OK &&
                            connectionName) {
                            name = connectionName;
                        }
                        if (connectionName) {
                            SysFreeString(connectionName);
                        }
                        network->Release();
                    }
                }
                connection->Release();
                if (!name.empty()) {
                    break;
                }
            }
            enumerator->Release();
        }
    }
    manager->Release();
    return name;
}

static NetworkInfo NetworkInfoFromAdapter(PIP_ADAPTER_ADDRESSES adapter) {
    NetworkInfo info;
    info.name = NetworkProfileName(adapter);
    if (info.name.empty()) {
        info.name = adapter->Description ? adapter->Description : L"";
    }
    info.glyph =
        adapter->IfType == IF_TYPE_IEEE80211 ? L"\xE701" : L"\xE839";
    return info;
}

static NetworkInfo ReadNetwork() {
    NetworkInfo info;
    std::vector<BYTE> buffer(16 * 1024);
    PIP_ADAPTER_ADDRESSES addresses = GetAdapterAddressesList(&buffer);
    if (!addresses) {
        return info;
    }

    PIP_ADAPTER_ADDRESSES best = nullptr;
    PIP_ADAPTER_ADDRESSES fallback = nullptr;
    for (auto* a = addresses; a; a = a->Next) {
        if (a->OperStatus != IfOperStatusUp ||
            a->IfType == IF_TYPE_SOFTWARE_LOOPBACK ||
            a->IfType == IF_TYPE_TUNNEL) {
            continue;
        }
        if (!fallback) {
            fallback = a;
        }
        if (a->FirstGatewayAddress) {
            best = a;
            break;
        }
    }
    if (!best) {
        best = fallback;
    }
    if (!best) {
        return info;
    }

    return NetworkInfoFromAdapter(best);
}

// ---------------------------------------------------------------------------
// Test state shared between the worker and the UI thread
// ---------------------------------------------------------------------------

enum class Phase {
    Idle,
    Warmup,
    Latency,
    Download,
    Upload,
    Stopping,
    Done,
    Error,
    Canceled,
};

struct TestState {
    Phase phase = Phase::Idle;
    double currentMbps = 0;
    double download = 0;
    double upload = 0;
    double ping = 0;
    double jitter = 0;
    bool hasPing = false;
    bool hasJitter = false;
    bool hasResult = false;
    std::wstring caption;
    std::wstring error;
    std::wstring server = L"Speedtest";
    std::wstring resultUrl;
    std::wstring externalIp;
    SYSTEMTIME when = {};
    LONGLONG dataUsed = 0;
};

static SRWLOCK g_stateLock = SRWLOCK_INIT;
static TestState g_state;
static std::atomic<LONG> g_cancelRequested = 0;
// Every speedtest.exe runs in this kill-on-close job: cancel terminates the
// job, and the handle closing (mod unload or explorer exit) kills it too.
static HANDLE g_job = nullptr;

static TestState CopyState() {
    TestState copy;
    AcquireSRWLockShared(&g_stateLock);
    copy = g_state;
    ReleaseSRWLockShared(&g_stateLock);
    return copy;
}

static bool StateRunning(const TestState& s) {
    return s.phase == Phase::Warmup || s.phase == Phase::Latency ||
           s.phase == Phase::Download || s.phase == Phase::Upload ||
           s.phase == Phase::Stopping;
}

static bool TestRunning() {
    return StateRunning(CopyState());
}

static void StateReset() {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state = TestState();
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetPhase(Phase phase, PCWSTR caption) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.phase = phase;
        g_state.caption = caption ? caption : L"";
        g_state.currentMbps = 0;
        g_state.error.clear();
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetCaption(PCWSTR caption) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.caption = caption ? caption : L"";
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetCurrent(double mbps) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.currentMbps = mbps;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetLatency(double ping, bool hasPing, double jitter, bool hasJitter) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.ping = ping;
        g_state.hasPing = hasPing;
        g_state.jitter = jitter;
        g_state.hasJitter = hasJitter;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetMeasured(bool upload, double mbps) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        if (upload) {
            g_state.upload = mbps;
        } else {
            g_state.download = mbps;
        }
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetServer(const std::wstring& server) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.server = server.empty() ? L"Speedtest" : server;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateFinish(double download, double upload, double ping, double jitter,
                        bool hasLatency, LONGLONG dataUsed,
                        const std::wstring& resultUrl, const std::wstring& externalIp) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase == Phase::Stopping) {
        g_state.phase = Phase::Canceled;
        g_state.caption = g_str->canceled;
        g_state.currentMbps = 0;
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }
    g_state.phase = Phase::Done;
    g_state.caption = g_str->completed;
    g_state.download = download;
    g_state.upload = upload;
    g_state.currentMbps = download;
    g_state.ping = ping;
    g_state.jitter = jitter;
    g_state.hasPing = hasLatency;
    g_state.hasJitter = hasLatency;
    g_state.hasResult = true;
    g_state.dataUsed = dataUsed;
    g_state.resultUrl = resultUrl;
    g_state.externalIp = externalIp;
    GetLocalTime(&g_state.when);
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateFail(const std::wstring& message) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.phase = Phase::Error;
    g_state.caption = g_str->error;
    g_state.error = message;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateCanceled() {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.phase = Phase::Canceled;
    g_state.caption = g_str->canceled;
    g_state.currentMbps = 0;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static HANDLE g_testThread = nullptr;

// Kills speedtest.exe and unblocks the worker's pipe read even if another
// process inherited the pipe's write end and keeps it open.
static void KillSpeedtest() {
    if (g_job) {
        TerminateJobObject(g_job, 1);
    }
    if (g_testThread) {
        CancelSynchronousIo(g_testThread);
    }
}

static void RequestTestCancel() {
    g_cancelRequested.store(1, std::memory_order_relaxed);
    KillSpeedtest();
    AcquireSRWLockExclusive(&g_stateLock);
    if (StateRunning(g_state)) {
        g_state.phase = Phase::Stopping;
        g_state.caption = g_str->stopping;
        g_state.currentMbps = 0;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

// ---------------------------------------------------------------------------
// Ookla Speedtest CLI engine
// ---------------------------------------------------------------------------

static HWND g_flyoutHwnd = nullptr;

static bool CancelRequested() {
    return g_cancelRequested.load(std::memory_order_relaxed) != 0;
}

static constexpr wchar_t kSpeedtestArgs[] =
    L" --format=jsonl --progress=yes --progress-update-interval=100";
// Passed only after the user accepted in the flyout (HasOoklaConsent); the CLI
// would otherwise wait for an answer on a console nobody can see.
static constexpr wchar_t kSpeedtestConsentArgs[] = L" --accept-license --accept-gdpr";

struct OoklaRun {
    Phase phase = Phase::Warmup;
    double lastDownload = 0;
    bool gotResult = false;
    bool hasPing = false;
    double download = 0;
    double upload = 0;
    double ping = 0;
    double jitter = 0;
    LONGLONG bytes = 0;
    std::wstring url;
    std::wstring ip;
    std::wstring lastError;
};

static double BandwidthMbps(const std::string& line, const char* direction) {
    double bandwidth = 0;
    return JsonNumber(line, {direction, "bandwidth"}, &bandwidth)
               ? bandwidth * 8.0 / 1'000'000.0
               : -1;
}

static std::wstring ServerText(const std::string& line) {
    std::string name, location;
    JsonString(line, {"server", "name"}, &name);
    JsonString(line, {"server", "location"}, &location);
    std::wstring text = Utf8ToWide(name);
    if (!location.empty()) {
        text += (text.empty() ? L"" : L" (") + Utf8ToWide(location) +
                (text.empty() ? L"" : L")");
    }
    return text;
}

static void HandleOoklaLine(const std::string& line, OoklaRun* run) {
    std::string type;
    if (!JsonString(line, {"type"}, &type)) {
        return;
    }

    if (type == "testStart") {
        StateSetServer(ServerText(line));
    } else if (type == "ping") {
        if (run->phase != Phase::Latency) {
            run->phase = Phase::Latency;
            StateSetPhase(Phase::Latency, g_str->ping);
        }
        double latency = 0, jitter = 0;
        if (JsonNumber(line, {"ping", "latency"}, &latency)) {
            bool hasJitter = JsonNumber(line, {"ping", "jitter"}, &jitter);
            StateSetLatency(latency, true, jitter, hasJitter);
        }
    } else if (type == "download" || type == "upload") {
        bool upload = type == "upload";
        Phase phase = upload ? Phase::Upload : Phase::Download;
        if (run->phase != phase) {
            if (upload) {
                // The last progress line carries the final download figure;
                // the live card shows it while the upload runs.
                StateSetMeasured(false, run->lastDownload);
            }
            run->phase = phase;
            StateSetPhase(phase, upload ? g_str->upload : g_str->download);
        }
        double mbps = BandwidthMbps(line, upload ? "upload" : "download");
        if (mbps >= 0) {
            if (!upload) {
                run->lastDownload = mbps;
            }
            StateSetCurrent(mbps);
        }
    } else if (type == "result") {
        run->download = std::max(0.0, BandwidthMbps(line, "download"));
        run->upload = std::max(0.0, BandwidthMbps(line, "upload"));
        run->hasPing = JsonNumber(line, {"ping", "latency"}, &run->ping);
        JsonNumber(line, {"ping", "jitter"}, &run->jitter);
        double downBytes = 0, upBytes = 0;
        JsonNumber(line, {"download", "bytes"}, &downBytes);
        JsonNumber(line, {"upload", "bytes"}, &upBytes);
        run->bytes = (LONGLONG)(downBytes + upBytes);
        std::string url;
        if (JsonString(line, {"result", "url"}, &url)) {
            run->url = Utf8ToWide(url);
        }
        std::string ip;
        if (JsonString(line, {"interface", "externalIp"}, &ip)) {
            run->ip = Utf8ToWide(ip);
        }
        std::wstring server = ServerText(line);
        if (!server.empty()) {
            StateSetServer(server);
        }
        run->gotResult = true;
    } else if (type == "log") {
        std::string level, message;
        if (JsonString(line, {"level"}, &level) && level == "error" &&
            JsonString(line, {"message"}, &message)) {
            run->lastError = Utf8ToWide(message);
        }
    }
}

// Starts speedtest.exe hidden, in g_job, with stdout+stderr on one pipe.
// Returns the process handle and the read end, or throws a UI message.
static void LaunchSpeedtest(const std::wstring& exe, HANDLE* process, HANDLE* readPipe) {
    // Last line of defence, before anything is allocated.
    if (!HasOoklaConsent()) {
        Wh_Log(L"Refusing to start speedtest.exe without the user's Ookla consent");
        throw std::wstring(g_str->errUnexpected);
    }
    DWORD attributes = GetFileAttributesW(exe.c_str());
    if (exe.empty() || attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        throw std::wstring(g_str->errNotFound) + exe;
    }

    SECURITY_ATTRIBUTES security = {sizeof(security), nullptr, TRUE};
    HANDLE readEnd = nullptr;
    HANDLE writeEnd = nullptr;
    if (!CreatePipe(&readEnd, &writeEnd, &security, 0)) {
        Wh_Log(L"CreatePipe failed (error %u)", GetLastError());
        throw std::wstring(g_str->errUnexpected);
    }
    SetHandleInformation(readEnd, HANDLE_FLAG_INHERIT, 0);

    // Inherit only the pipe, not whatever inheritable handles explorer holds.
    SIZE_T listSize = 0;
    InitializeProcThreadAttributeList(nullptr, 1, 0, &listSize);
    std::vector<BYTE> listBuffer(listSize);
    auto attributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)listBuffer.data();
    bool listReady =
        InitializeProcThreadAttributeList(attributeList, 1, 0, &listSize) &&
        UpdateProcThreadAttribute(attributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                  &writeEnd, sizeof(writeEnd), nullptr, nullptr);
    if (!listReady) {
        Wh_Log(L"Handle list setup failed (error %u)", GetLastError());
        CloseHandle(writeEnd);
        CloseHandle(readEnd);
        throw std::wstring(g_str->errUnexpected);
    }

    STARTUPINFOEXW startup = {};
    startup.StartupInfo.cb = sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdOutput = writeEnd;
    startup.StartupInfo.hStdError = writeEnd;
    startup.lpAttributeList = attributeList;

    std::wstring commandLine = L"\"" + exe + L"\"" + kSpeedtestArgs + kSpeedtestConsentArgs;
    PROCESS_INFORMATION info = {};
    // The original pointer bypasses this mod's own CreateProcessW hook.
    CreateProcessW_t create = CreateProcessW_Original ? CreateProcessW_Original : CreateProcessW;
    BOOL created = create(exe.c_str(), commandLine.data(), nullptr, nullptr, TRUE,
                          CREATE_NO_WINDOW | CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT,
                          nullptr, nullptr, &startup.StartupInfo, &info);
    DWORD createError = GetLastError();
    DeleteProcThreadAttributeList(attributeList);
    CloseHandle(writeEnd);
    if (!created) {
        CloseHandle(readEnd);
        Wh_Log(L"speedtest.exe launch failed (error %u)", createError);
        throw std::wstring(g_str->errNotFound) + exe;
    }

    if (!g_job || !AssignProcessToJobObject(g_job, info.hProcess)) {
        Wh_Log(L"AssignProcessToJobObject failed (error %u)", GetLastError());
        TerminateProcess(info.hProcess, 1);
        CloseHandle(info.hThread);
        CloseHandle(info.hProcess);
        CloseHandle(readEnd);
        throw std::wstring(g_str->errUnexpected);
    }
    ResumeThread(info.hThread);
    CloseHandle(info.hThread);
    *process = info.hProcess;
    *readPipe = readEnd;
}

static void RunTestWorker() {
    try {
        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }
        StateSetPhase(Phase::Warmup, g_str->connecting);

        // Closed on every path, including an exception while parsing.
        struct HandleGuard {
            HANDLE handle = nullptr;
            ~HandleGuard() {
                if (handle) {
                    CloseHandle(handle);
                }
            }
        };
        HandleGuard processGuard;
        HandleGuard pipeGuard;
        LaunchSpeedtest(SpeedtestPath(), &processGuard.handle, &pipeGuard.handle);
        HANDLE process = processGuard.handle;
        HANDLE readPipe = pipeGuard.handle;
        // A cancel that raced the launch found no process in the job yet.
        if (CancelRequested()) {
            TerminateJobObject(g_job, 1);
        }

        OoklaRun run;
        std::string pending;
        char buffer[4096];
        DWORD read = 0;
        // ReadFile fails with ERROR_BROKEN_PIPE once the CLI exits or the job
        // is terminated, so cancel unblocks this loop.
        while (!CancelRequested() &&
               ReadFile(readPipe, buffer, sizeof(buffer), &read, nullptr) && read) {
            pending.append(buffer, read);
            size_t newline;
            while ((newline = pending.find('\n')) != std::string::npos) {
                std::string line = pending.substr(0, newline);
                pending.erase(0, newline + 1);
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                HandleOoklaLine(line, &run);
            }
        }
        if (!pending.empty()) {
            HandleOoklaLine(pending, &run);
        }
        CloseHandle(readPipe);
        pipeGuard.handle = nullptr;
        WaitForSingleObject(process, 5000);
        DWORD exitCode = 0;
        GetExitCodeProcess(process, &exitCode);

        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }
        if (!run.gotResult) {
            Wh_Log(L"speedtest.exe exit %u without result; last error: %s", exitCode,
                   run.lastError.c_str());
            throw std::wstring(g_str->errNoResult);
        }

        Wh_Log(L"result: %.1f / %.1f Mbps, ping %.2f ms, %s", run.download,
               run.upload, run.ping, run.url.c_str());
        StateFinish(run.download, run.upload, run.ping, run.jitter, run.hasPing,
                    run.bytes, run.url, run.ip);
    } catch (const std::wstring& error) {
        if (CancelRequested()) {
            StateCanceled();
        } else {
            StateFail(error);
        }
    } catch (...) {
        if (CancelRequested()) {
            StateCanceled();
        } else {
            StateFail(g_str->errUnexpected);
        }
    }
    if (g_flyoutHwnd) {
        PostMessageW(g_flyoutHwnd, WM_APP + 1, 0, 0);
    }
}

static DWORD WINAPI TestWorkerThread(LPVOID) {
    RunTestWorker();
    return 0;
}

// ---------------------------------------------------------------------------
// Direct2D surface
// ---------------------------------------------------------------------------

static ID2D1Factory* g_d2dFactory = nullptr;
static IDWriteFactory* g_dwriteFactory = nullptr;
static ID2D1RenderTarget* g_renderTarget = nullptr;
static ID2D1HwndRenderTarget* g_hwndTarget = nullptr;
static ID2D1StrokeStyle* g_roundStroke = nullptr;

// DirectComposition path: the flyout keeps WS_EX_NOREDIRECTIONBITMAP, so the
// HWND target is only a fallback when D3D/DComp is unavailable.
static bool g_composition = false;
static ID3D11Device* g_d3dDevice = nullptr;
static IDXGIDevice* g_dxgiDevice = nullptr;
static IDXGISwapChain1* g_swapChain = nullptr;
static IDCompositionDevice* g_dcompDevice = nullptr;
static IDCompositionTarget* g_dcompTarget = nullptr;
static IDCompositionVisual* g_dcompVisual = nullptr;
static ID2D1DeviceContext* g_context = nullptr;
static ID2D1Bitmap1* g_contextBitmap = nullptr;
static UINT g_targetWidth = 0;
static UINT g_targetHeight = 0;
static float g_targetDpi = 0;

// Acrylic grain drawn under the content on the dark tint.
static ID2D1Bitmap* g_noiseBitmap = nullptr;
static ID2D1BitmapBrush* g_noiseBrush = nullptr;
static constexpr int kNoiseSize = 128;
static constexpr float kNoiseOpacity = 0.06f;

static Palette g_palette = {};
static bool g_dark = true;
static bool g_systemLight = false;
static bool g_transparencyEnabled = false;
static bool g_materialSupported = false;
static bool g_translucent = false;
static float g_dpi = 96.0f;
static float g_scale = 1.0f;

static void RefreshAppearance() {
    g_systemLight = ReadSystemLightTheme();
    g_dark = !g_systemLight;
    g_palette = MakePalette(g_systemLight);
    g_transparencyEnabled = ReadTransparencyEnabled();
    g_materialSupported = MaterialSupported();
    g_translucent = g_materialSupported && g_transparencyEnabled;
}

struct TextKey {
    float size;
    int weight;
    int align;
    int valign;
    bool wrap;
    int family;
    const UiStrings* lang;

    bool operator==(const TextKey& other) const {
        return size == other.size && weight == other.weight &&
               align == other.align && valign == other.valign &&
               wrap == other.wrap && family == other.family &&
               lang == other.lang;
    }
};

static std::vector<std::pair<TextKey, IDWriteTextFormat*>> g_textFormats;

static PCWSTR FamilyName(int family) {
    switch (family) {
        case 1:
            return kDisplayFamily;
        case 2:
            return kIconFamily;
        default:
            return kTextFamily;
    }
}

static IDWriteTextFormat* GetTextFormat(float size, DWRITE_FONT_WEIGHT weight,
                                        DWRITE_TEXT_ALIGNMENT align,
                                        DWRITE_PARAGRAPH_ALIGNMENT valign,
                                        bool wrap, int family) {
    const UiStrings* lang = g_str;
    TextKey key = {size, (int)weight, (int)align, (int)valign, wrap, family, lang};
    for (const auto& entry : g_textFormats) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    if (!g_dwriteFactory) {
        return nullptr;
    }

    IDWriteTextFormat* format = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextFormat(
            FamilyName(family), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, lang->locale, &format))) {
        return nullptr;
    }
    format->SetTextAlignment(align);
    format->SetParagraphAlignment(valign);
    format->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP
                                 : DWRITE_WORD_WRAPPING_NO_WRAP);
    g_textFormats.push_back({key, format});
    return format;
}

static D2D1_RECT_F Rect(float left, float top, float right, float bottom) {
    D2D1_RECT_F r;
    r.left = left;
    r.top = top;
    r.right = right;
    r.bottom = bottom;
    return r;
}

struct BrushEntry {
    D2D1_COLOR_F color;
    ID2D1SolidColorBrush* brush;
};

static std::vector<BrushEntry> g_brushes;

static bool SameColor(D2D1_COLOR_F a, D2D1_COLOR_F b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

static void ReleaseBrushes() {
    for (auto& entry : g_brushes) {
        entry.brush->Release();
    }
    g_brushes.clear();
}

// Brushes are cached per colour and handed out with an extra AddRef, so the
// call sites can keep releasing the returned brush.
static ID2D1SolidColorBrush* Brush(D2D1_COLOR_F color) {
    if (!g_renderTarget) {
        return nullptr;
    }
    for (const auto& entry : g_brushes) {
        if (SameColor(entry.color, color)) {
            entry.brush->AddRef();
            return entry.brush;
        }
    }
    if (g_brushes.size() >= 128) {
        ReleaseBrushes();
    }
    ID2D1SolidColorBrush* brush = nullptr;
    if (FAILED(g_renderTarget->CreateSolidColorBrush(color, &brush))) {
        return nullptr;
    }
    g_brushes.push_back({color, brush});
    brush->AddRef();
    return brush;
}

static void FillRounded(D2D1_RECT_F rect, float radius, D2D1_COLOR_F color) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    D2D1_ROUNDED_RECT rr = {rect, radius, radius};
    g_renderTarget->FillRoundedRectangle(rr, brush);
    brush->Release();
}

static void StrokeRounded(D2D1_RECT_F rect, float radius, D2D1_COLOR_F color,
                          float width = 1.0f) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    D2D1_ROUNDED_RECT rr = {rect, radius, radius};
    g_renderTarget->DrawRoundedRectangle(rr, brush, width);
    brush->Release();
}

// WinUI focus visual: 2 px neutral ring offset by 1 px plus a 1 px inner
// stroke in the opposite tone.
static void DrawFocusRing(D2D1_RECT_F rect, float radius, float alpha) {
    D2D1_RECT_F outer = Rect(rect.left - 3.0f, rect.top - 3.0f,
                             rect.right + 3.0f, rect.bottom + 3.0f);
    D2D1_COLOR_F ring = WithAlpha(g_palette.text, alpha);
    D2D1_COLOR_F inner = g_systemLight ? Col(0xFFFFFF, alpha)
                                       : Col(0x000000, alpha);
    StrokeRounded(outer, radius + 3.0f, ring, 2.0f);
    StrokeRounded(Rect(outer.left + 1.0f, outer.top + 1.0f, outer.right - 1.0f,
                       outer.bottom - 1.0f),
                  radius + 2.0f, inner, 1.0f);
}

static void FillRect(D2D1_RECT_F rect, D2D1_COLOR_F color) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    g_renderTarget->FillRectangle(rect, brush);
    brush->Release();
}

static void ReleaseNoise() {
    if (g_noiseBrush) {
        g_noiseBrush->Release();
        g_noiseBrush = nullptr;
    }
    if (g_noiseBitmap) {
        g_noiseBitmap->Release();
        g_noiseBitmap = nullptr;
    }
}

static bool EnsureNoise() {
    if (g_noiseBrush) {
        return true;
    }
    if (!g_renderTarget) {
        return false;
    }

    std::vector<UINT32> pixels((size_t)kNoiseSize * kNoiseSize);
    UINT32 state = 0x9E3779B9u;
    for (auto& pixel : pixels) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        int level = 0x2C + (int)(state % 61) - 30;
        if (level < 0) {
            level = 0;
        } else if (level > 255) {
            level = 255;
        }
        pixel = 0xFF000000u | ((UINT32)level << 16) | ((UINT32)level << 8) |
                (UINT32)level;
    }

    D2D1_BITMAP_PROPERTIES props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    if (FAILED(g_renderTarget->CreateBitmap(
            D2D1::SizeU(kNoiseSize, kNoiseSize), pixels.data(),
            kNoiseSize * sizeof(UINT32), &props, &g_noiseBitmap))) {
        return false;
    }

    D2D1_BITMAP_BRUSH_PROPERTIES brushProps = {};
    brushProps.extendModeX = D2D1_EXTEND_MODE_WRAP;
    brushProps.extendModeY = D2D1_EXTEND_MODE_WRAP;
    brushProps.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
    if (FAILED(g_renderTarget->CreateBitmapBrush(g_noiseBitmap, &brushProps,
                                                nullptr, &g_noiseBrush))) {
        ReleaseNoise();
        return false;
    }
    return true;
}

static void DrawLine(float x1, float y1, float x2, float y2, D2D1_COLOR_F color,
                     float width = 1.0f) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    g_renderTarget->DrawLine({x1, y1}, {x2, y2}, brush, width);
    brush->Release();
}

static void Text(const std::wstring& text, D2D1_RECT_F rect, float size,
                 D2D1_COLOR_F color, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
                 DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING,
                 DWRITE_PARAGRAPH_ALIGNMENT valign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                 int family = 0, bool wrap = false, bool tabular = false) {
    if (text.empty()) {
        return;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight, align, valign, wrap, family);
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!format || !brush) {
        if (brush) {
            brush->Release();
        }
        return;
    }

    if (!tabular || !g_dwriteFactory) {
        g_renderTarget->DrawTextW(text.c_str(), (UINT32)text.size(), format, rect, brush,
                                  D2D1_DRAW_TEXT_OPTIONS_NONE);
        brush->Release();
        return;
    }

    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(
            text.c_str(), (UINT32)text.size(), format,
            std::max(1.0f, rect.right - rect.left), std::max(1.0f, rect.bottom - rect.top),
            &layout))) {
        brush->Release();
        return;
    }
    // Tabular figures keep every digit the same width so live values do not jitter.
    IDWriteTypography* typography = nullptr;
    if (SUCCEEDED(g_dwriteFactory->CreateTypography(&typography))) {
        DWRITE_FONT_FEATURE feature = {DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1};
        typography->AddFontFeature(feature);
        layout->SetTypography(typography, {0, (UINT32)text.size()});
        typography->Release();
    }
    g_renderTarget->DrawTextLayout({rect.left, rect.top}, layout, brush,
                                   D2D1_DRAW_TEXT_OPTIONS_NONE);
    layout->Release();
    brush->Release();
}

static float MeasureWidth(const std::wstring& text, float size,
                          DWRITE_FONT_WEIGHT weight, int family = 0,
                          bool tabular = false) {
    if (!g_dwriteFactory || text.empty()) {
        return 0;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight,
                                              DWRITE_TEXT_ALIGNMENT_LEADING,
                                              DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                                              false, family);
    if (!format) {
        return 0;
    }
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(
            text.c_str(), (UINT32)text.size(), format, 1000, 1000, &layout))) {
        return 0;
    }
    if (tabular) {
        IDWriteTypography* typography = nullptr;
        if (SUCCEEDED(g_dwriteFactory->CreateTypography(&typography))) {
            DWRITE_FONT_FEATURE feature = {DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1};
            typography->AddFontFeature(feature);
            layout->SetTypography(typography, {0, (UINT32)text.size()});
            typography->Release();
        }
    }
    DWRITE_TEXT_METRICS metrics = {};
    layout->GetMetrics(&metrics);
    layout->Release();
    return metrics.width;
}

// Distance between the top of a single-line layout and its baseline, used to
// put a value and its unit on the same baseline.
static float BaselineOffset(float size, int family = 0) {
    if (!g_dwriteFactory) {
        return size * 0.9f;
    }
    IDWriteTextFormat* format = GetTextFormat(size, DWRITE_FONT_WEIGHT_NORMAL,
                                              DWRITE_TEXT_ALIGNMENT_LEADING,
                                              DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                                              false, family);
    if (!format) {
        return size * 0.9f;
    }
    float baseline = size * 0.9f;
    IDWriteTextLayout* layout = nullptr;
    if (SUCCEEDED(g_dwriteFactory->CreateTextLayout(L"0", 1, format, 1000, 1000,
                                                    &layout))) {
        DWRITE_LINE_METRICS metrics = {};
        UINT32 count = 0;
        if (SUCCEEDED(layout->GetLineMetrics(&metrics, 1, &count)) && count > 0) {
            baseline = metrics.baseline;
        }
        layout->Release();
    }
    return baseline;
}

// Fits one line into maxWidth, adding an ellipsis only when it does not fit.
static std::wstring Ellipsize(const std::wstring& text, float maxWidth, float size,
                              DWRITE_FONT_WEIGHT weight, int family = 0) {
    if (text.empty() || MeasureWidth(text, size, weight, family) <= maxWidth) {
        return text;
    }
    std::wstring result = text;
    while (!result.empty() &&
           MeasureWidth(result + L"\x2026", size, weight, family) > maxWidth) {
        result.pop_back();
    }
    return result + L"\x2026";
}

static ID2D1PathGeometry* CreateArcPath(float cx, float cy, float radius,
                                        float startDegrees, float sweepDegrees) {
    if (!g_d2dFactory || sweepDegrees <= 0.1f) {
        return nullptr;
    }

    ID2D1PathGeometry* geometry = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry))) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geometry->Open(&sink))) {
        geometry->Release();
        return nullptr;
    }

    const float pi = 3.14159265358979323846f;
    auto point = [&](float angle) {
        float radians = angle * pi / 180.0f;
        return D2D1_POINT_2F{cx + radius * cosf(radians), cy + radius * sinf(radians)};
    };

    sink->BeginFigure(point(startDegrees), D2D1_FIGURE_BEGIN_HOLLOW);
    D2D1_ARC_SEGMENT arc = {};
    arc.point = point(startDegrees + sweepDegrees);
    arc.size = {radius, radius};
    arc.rotationAngle = 0;
    arc.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc.arcSize = sweepDegrees > 180.0f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc);
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    sink->Release();
    return geometry;
}

static void DrawArc(float cx, float cy, float radius, float fraction,
                    D2D1_COLOR_F color, float width) {
    ID2D1PathGeometry* path = CreateArcPath(cx, cy, radius, 135.0f, 270.0f * fraction);
    if (!path) {
        return;
    }
    ID2D1SolidColorBrush* brush = Brush(color);
    if (brush) {
        g_renderTarget->DrawGeometry(path, brush, width, g_roundStroke);
        brush->Release();
    }
    path->Release();
}

// Gaussian blur for the light bar. Device-dependent: released with the targets.
static ID2D1Effect* g_blurEffect = nullptr;
static const GUID kClsidGaussianBlur = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

// "Suave" light bar: a blurred copy of the arc under the sharp one, so the
// light leaks a little to the sides. Without a device context (HWND
// fallback) a few wider, fainter strokes stand in for the blur.
static void DrawGlowArc(float cx, float cy, float radius, float fraction,
                        D2D1_COLOR_F color, float width) {
    if (fraction <= 0.0005f) {
        return;
    }
    bool glowDrawn = false;
    if (g_context) {
        if (!g_blurEffect &&
            SUCCEEDED(g_context->CreateEffect(kClsidGaussianBlur, &g_blurEffect))) {
            // Standard deviation is in DIPs, so it scales with the DPI.
            g_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 6.0f);
        }
        ID2D1CommandList* list = nullptr;
        if (g_blurEffect && SUCCEEDED(g_context->CreateCommandList(&list))) {
            // Record without the world transform: DrawImage applies it once.
            D2D1_MATRIX_3X2_F transform;
            g_context->GetTransform(&transform);
            ID2D1Image* target = nullptr;
            g_context->GetTarget(&target);
            g_context->SetTarget(list);
            g_context->SetTransform(D2D1::Matrix3x2F::Identity());
            DrawArc(cx, cy, radius, fraction, WithAlpha(color, 0.7f), width);
            g_context->SetTransform(transform);
            g_context->SetTarget(target);
            if (target) {
                target->Release();
            }
            if (SUCCEEDED(list->Close())) {
                g_blurEffect->SetInput(0, list);
                g_context->DrawImage(g_blurEffect);
                g_blurEffect->SetInput(0, nullptr);
                glowDrawn = true;
            }
            list->Release();
        }
    }
    if (!glowDrawn) {
        static constexpr float kExtra[] = {16.0f, 12.0f, 8.0f, 4.0f};
        static constexpr float kAlpha[] = {0.04f, 0.07f, 0.12f, 0.18f};
        for (int i = 0; i < 4; i++) {
            DrawArc(cx, cy, radius, fraction, WithAlpha(color, kAlpha[i]), width + kExtra[i]);
        }
    }
    DrawArc(cx, cy, radius, fraction, color, width);
}

// ---------------------------------------------------------------------------
// Flyout UI state and drawing
// ---------------------------------------------------------------------------

enum class View {
    Gauge,
    History,
    Error,
    Consent,
};

enum class Hot {
    None,
    Start,
    Copy,
    Close,
    Retry,
    Row,
    Accept,
    LinkLicense,
    LinkTerms,
    LinkPrivacy,
};

static View g_view = View::Gauge;
static View g_animFrom = View::Gauge;
static bool g_animating = false;
static ULONGLONG g_animStart = 0;
// DWM applies the backdrop asynchronously, so the first frames are painted
// with an opaque scrim that then fades out.
static ULONGLONG g_scrimFadeAt = 0;

static constexpr float kScrimSettleMs = 60.0f;
static constexpr float kScrimFadeMs = 200.0f;

static constexpr float kLightTint = 0.60f;
static constexpr float kDarkTint = 0.42f;

static bool ScrimFading() {
    return g_scrimFadeAt &&
           Now() - g_scrimFadeAt <
               (ULONGLONG)(kScrimSettleMs + kScrimFadeMs + 50.0f);
}

static float ScrimAlpha() {
    if (!g_scrimFadeAt) {
        return 1.0f;
    }
    float elapsed = (float)(Now() - g_scrimFadeAt);
    if (elapsed <= kScrimSettleMs) {
        return 1.0f;
    }
    float t = (elapsed - kScrimSettleMs) / kScrimFadeMs;
    if (t >= 1.0f) {
        return 0.0f;
    }
    float eased = 1.0f - (1.0f - t) * (1.0f - t);
    return 1.0f - eased;
}

static ULONGLONG g_copyUntil = 0;
static Spring g_gaugeSpring;   // gauge arc, fraction 0..1
static Spring g_numberSpring;  // centre number, Mbps
static Spring g_scroll;        // history list offset, DIP
static double g_scrollTarget = 0;
static bool g_motionActive = false;
// Origin of the end-of-test history timeline; 0 = already finished.
static ULONGLONG g_historyShownAt = 0;
// The finished run is already in g_history (one entry per run).
static bool g_historySaved = false;
// When the download / upload result became final (0 = not yet). The cards
// under the gauge show only final values; the gauge shows the live one.
static ULONGLONG g_downloadFinalAt = 0;
static ULONGLONG g_uploadFinalAt = 0;
static constexpr double kCardFadeMs = 300.0;
static Hot g_hot = Hot::None;
static Hot g_pressed = Hot::None;
static Hot g_focus = Hot::None;
static int g_hotRow = -1;
static int g_pressedRow = -1;
static int g_focusRow = 0;
static NetworkInfo g_network;
static bool g_uiVisible = false;
static HWND g_previousForeground = nullptr;
static UINT g_mouseButtonsDown = 0;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static volatile LONG g_shutdown = 0;

static UINT MouseButtonsDown() {
    constexpr int buttons[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1,
                               VK_XBUTTON2};
    UINT state = 0;
    for (size_t i = 0; i < ARRAYSIZE(buttons); i++) {
        if (GetAsyncKeyState(buttons[i]) & 0x8000) {
            state |= 1u << i;
        }
    }
    return state;
}

static constexpr float kAnimMs = 250.0f;
static constexpr D2D1_RECT_F kCloseRect = {352.0f, 9.0f, 384.0f, 39.0f};
static constexpr D2D1_RECT_F kCopyRect = {308.0f, 47.0f, 380.0f, 73.0f};
static constexpr D2D1_RECT_F kErrorRect = {20.0f, 56.0f, 380.0f, 296.0f};
static constexpr D2D1_RECT_F kRetryRect = {136.0f, 230.0f, 264.0f, 262.0f};

// First-run Ookla consent card (DIP).
static constexpr D2D1_RECT_F kConsentRect = {20.0f, 56.0f, 380.0f, 372.0f};
static constexpr D2D1_RECT_F kAcceptRect = {110.0f, 310.0f, 290.0f, 344.0f};
static constexpr D2D1_RECT_F kLinkRects[3] = {
    {40.0f, 262.0f, 140.0f, 284.0f},
    {150.0f, 262.0f, 250.0f, 284.0f},
    {260.0f, 262.0f, 360.0f, 284.0f},
};
static constexpr PCWSTR kOoklaUrls[3] = {
    L"https://www.speedtest.net/about/eula",
    L"https://www.speedtest.net/about/terms",
    L"https://www.speedtest.net/about/privacy",
};
static constexpr Hot kLinkHots[3] = {Hot::LinkLicense, Hot::LinkTerms, Hot::LinkPrivacy};

static int LinkIndex(Hot hot) {
    for (int i = 0; i < 3; i++) {
        if (kLinkHots[i] == hot) {
            return i;
        }
    }
    return -1;
}

// History list geometry (DIP).
static constexpr float kListTop = 84.0f;
static constexpr float kListBottom = 372.0f;
static constexpr float kRowHeight = 70.0f;
static constexpr float kRowStride = 76.0f;
// Total length of the end-of-test timeline (last fade ends at 1400 ms).
static constexpr double kHistoryTimelineMs = 1450.0;

static float g_layoutHeight = kGaugeHeight;
static float g_heightFrom = kGaugeHeight;

static float ViewHeight(View view) {
    return view == View::Error ? kErrorHeight : kGaugeHeight;
}

static D2D1_RECT_F FootRect() {
    return Rect(20.0f, g_layoutHeight - 32.0f, 384.0f, g_layoutHeight - 14.0f);
}

static bool AnimationsEnabled() {
    BOOL enabled = TRUE;
    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0)) {
        return true;
    }
    return enabled != FALSE;
}

static bool FocusVisible() {
    return g_focus != Hot::None && g_flyoutHwnd && GetFocus() == g_flyoutHwnd;
}

static float AnimT() {
    return Clamp01((float)(Now() - g_animStart) / kAnimMs);
}

static float AnimOut() {
    return 1.0f - Clamp01(AnimT() / 0.38f);
}

static float AnimIn() {
    return Clamp01((AnimT() - 0.38f) / 0.62f);
}

// Milliseconds into the end-of-test timeline; past the end when finished.
static double HistoryT() {
    return g_historyShownAt ? (double)(Now() - g_historyShownAt) : 1e9;
}

static float HistoryMaxScroll() {
    float content = (float)g_history.size() * kRowStride - (kRowStride - kRowHeight);
    return std::max(0.0f, content - (kListBottom - kListTop));
}

static void SetScrollTarget(double target) {
    g_scrollTarget = Clamp(target, 0.0, HistoryMaxScroll());
}

static float RowTop(int index) {
    return kListTop + (float)index * kRowStride - (float)g_scroll.x;
}

static D2D1_RECT_F RowRect(int index) {
    float top = RowTop(index);
    return Rect(20.0f, top, 380.0f, top + kRowHeight);
}

static void ScrollRowIntoView(int index) {
    double top = (double)index * kRowStride;
    double visible = kListBottom - kListTop;
    if (top < g_scrollTarget) {
        SetScrollTarget(top);
    } else if (top + kRowHeight > g_scrollTarget + visible) {
        SetScrollTarget(top + kRowHeight - visible);
    }
}

// Seconds since the previous frame, from the performance counter. The first
// frame after an idle stretch counts as one 60 Hz frame.
static double FrameDt() {
    static LARGE_INTEGER frequency = {};
    static LARGE_INTEGER last = {};
    LARGE_INTEGER now = {};
    if (!frequency.QuadPart) {
        QueryPerformanceFrequency(&frequency);
    }
    QueryPerformanceCounter(&now);
    double dt = last.QuadPart && frequency.QuadPart
                    ? (double)(now.QuadPart - last.QuadPart) / (double)frequency.QuadPart
                    : 1.0 / 60.0;
    last = now;
    return dt > 0.25 ? 1.0 / 60.0 : dt;
}

struct GaugeTick {
    double mbps;
    double fraction;
    float width;
};

// Labels are formatted at draw time so they get the thousands separator.
static constexpr GaugeTick kGaugeTicks[] = {
    {0, 0.0, 28},      {5, 0.12, 28},    {10, 0.24, 32},
    {50, 0.36, 32},    {100, 0.50, 38},  {250, 0.625, 38},
    {500, 0.75, 38},   {1000, 0.88, 44}, {2000, 1.0, 50},
};

static double GaugeFraction(double mbps) {
    constexpr double kGaugeMaxMbps = 2000.0;
    double value = Clamp(mbps, 0.0, kGaugeMaxMbps);
    if (!std::isfinite(value) || value <= 0) {
        return 0;
    }

    for (size_t i = 1; i < ARRAYSIZE(kGaugeTicks); i++) {
        const GaugeTick& lower = kGaugeTicks[i - 1];
        const GaugeTick& upper = kGaugeTicks[i];
        if (value <= upper.mbps) {
            double progress = lower.mbps == 0
                                  ? std::log1p(value) / std::log1p(upper.mbps)
                                  : std::log(value / lower.mbps) /
                                        std::log(upper.mbps / lower.mbps);
            return lower.fraction +
                   (upper.fraction - lower.fraction) * progress;
        }
    }
    return 1.0;
}

static bool SyncPhase(const TestState& state);

// Steps every spring by the real frame time. Called once per painted frame.
static void AdvanceAnimations(const TestState& state) {
    SyncPhase(state);
    double number = 0;
    if (state.phase == Phase::Download || state.phase == Phase::Upload) {
        number = state.currentMbps;
    } else if (state.phase == Phase::Done) {
        number = state.download;
    }
    double gauge = GaugeFraction(number);
    double dt = FrameDt();
    if (!AnimationsEnabled()) {
        g_gaugeSpring = {gauge, 0};
        g_numberSpring = {number, 0};
        g_scroll = {g_scrollTarget, 0};
        g_motionActive = false;
        return;
    }
    SpringStep(g_gaugeSpring, gauge, dt);
    SpringStep(g_numberSpring, number, dt);
    SpringStep(g_scroll, g_scrollTarget, dt);
    // Every spring must get the chance to settle, so no short-circuit.
    bool gaugeSettled = SpringSettle(g_gaugeSpring, gauge, 0.0005);
    bool numberSettled = SpringSettle(g_numberSpring, number, 0.05);
    bool scrollSettled = SpringSettle(g_scroll, g_scrollTarget, 0.25);
    g_motionActive = !(gaugeSettled && numberSettled && scrollSettled);
}

static bool CardFading(ULONGLONG finalAt) {
    return finalAt && (double)(Now() - finalAt) < kCardFadeMs;
}

// 0 while the value is not final, then eases to 1 over kCardFadeMs.
static float CardIn(ULONGLONG finalAt) {
    return finalAt ? (float)EaseOutCubic(Phase01((double)(Now() - finalAt), 0, kCardFadeMs))
                   : 0.0f;
}

static ULONGLONG CardFinalNow() {
    // With animations off the value appears already faded in.
    return AnimationsEnabled() ? Now() : Now() - (ULONGLONG)kCardFadeMs;
}

// Applies a phase change to the animation state. Called from the frame that
// draws the new phase (AdvanceAnimations) and from TickFlyout, whichever sees
// it first, so a frame never shows the new phase with the old phase's gauge.
static bool SyncPhase(const TestState& state) {
    static Phase syncedPhase = Phase::Idle;
    if (state.phase == syncedPhase) {
        return false;
    }
    syncedPhase = state.phase;
    // Upload starting means the download figure is final; Done, the upload.
    if (state.phase == Phase::Upload && !g_downloadFinalAt) {
        g_downloadFinalAt = CardFinalNow();
    } else if (state.phase == Phase::Done && !g_uploadFinalAt) {
        g_uploadFinalAt = CardFinalNow();
    }
    // Start each transfer phase from an empty ring.
    if (state.phase == Phase::Warmup || state.phase == Phase::Download ||
        state.phase == Phase::Upload || state.phase == Phase::Stopping ||
        state.phase == Phase::Idle || state.phase == Phase::Canceled) {
        g_gaugeSpring = {};
        g_numberSpring = {};
    }
    return true;
}

static bool AnyAnimationRunning() {
    return g_motionActive || g_animating || ScrimFading() ||
           (g_view == View::History && HistoryT() < kHistoryTimelineMs) ||
           CardFading(g_downloadFinalAt) || CardFading(g_uploadFinalAt);
}

static PCWSTR StartButtonLabel(const TestState& state) {
    if (StateRunning(state)) {
        return g_str->stop;
    }
    if (state.phase == Phase::Done || state.phase == Phase::Canceled ||
        g_view == View::History) {
        return g_str->repeat;
    }
    return g_str->start;
}

static bool IsRepeatButton(const TestState& state) {
    return !StateRunning(state) &&
           (state.phase == Phase::Done || state.phase == Phase::Canceled ||
            g_view == View::History);
}

static D2D1_RECT_F StartButtonRect(const TestState& state) {
    PCWSTR label = StartButtonLabel(state);
    DWRITE_FONT_WEIGHT weight = StateRunning(state)
                                   ? DWRITE_FONT_WEIGHT_NORMAL
                                   : DWRITE_FONT_WEIGHT_SEMI_BOLD;
    float width = MeasureWidth(label, 13, weight) + 32.0f;
    if (IsRepeatButton(state)) {
        width += 22.0f;
    }
    width = std::max(width, 76.0f);
    float left = (kWidth - width) / 2.0f;
    return Rect(left, 382.0f, left + width, 416.0f);
}

static bool InRect(D2D1_RECT_F rect, float x, float y) {
    return x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom;
}

static Hot HitTest(float x, float y, int* row = nullptr) {
    if (row) {
        *row = -1;
    }
    if (InRect(kCloseRect, x, y)) {
        return Hot::Close;
    }
    if (g_view == View::Error) {
        return InRect(kRetryRect, x, y) ? Hot::Retry : Hot::None;
    }
    if (g_view == View::Consent) {
        if (InRect(kAcceptRect, x, y)) {
            return Hot::Accept;
        }
        for (int i = 0; i < 3; i++) {
            if (InRect(kLinkRects[i], x, y)) {
                return kLinkHots[i];
            }
        }
        return Hot::None;
    }
    if (g_view == View::History) {
        if (InRect(kCopyRect, x, y)) {
            return Hot::Copy;
        }
        if (y >= kListTop && y <= kListBottom && x >= 20.0f && x <= 380.0f) {
            float offset = y - kListTop + (float)g_scroll.x;
            int index = (int)(offset / kRowStride);
            if (index >= 0 && index < (int)g_history.size() &&
                offset - (float)index * kRowStride <= kRowHeight) {
                if (row) {
                    *row = index;
                }
                return Hot::Row;
            }
        }
    }
    // Repetir only takes clicks once it has faded in (1000 ms into the
    // history timeline): a stray click must not start another ~4 GB test.
    bool startVisible = g_view != View::History || HistoryT() >= 1000.0;
    if (startVisible && InRect(StartButtonRect(CopyState()), x, y)) {
        return Hot::Start;
    }
    return Hot::None;
}

static void DrawGauge(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }

    const float cx = kWidth / 2;
    const float cy = 162.0f;
    const float radius = 96.0f;
    const float stroke = 13.0f;

    DrawArc(cx, cy, radius, 1.0f, WithAlpha(g_palette.track, alpha), stroke);
    D2D1_COLOR_F color =
        state.phase == Phase::Upload ? g_palette.upload : g_palette.download;
    DrawGlowArc(cx, cy, radius, (float)g_gaugeSpring.x, WithAlpha(color, alpha), stroke);

    std::wstring value = L"\x2014";
    std::wstring unit = L"Mbps";
    if (state.phase != Phase::Latency && state.phase != Phase::Warmup &&
        state.phase != Phase::Stopping && state.phase != Phase::Canceled &&
        state.phase != Phase::Idle) {
        value = FormatNumber(std::max(0.0, g_numberSpring.x));
    }

    Text(value, Rect(20, 124, 380, 178), 44, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 1, false, true);
    Text(unit, Rect(20, 178, 380, 198), 15, WithAlpha(g_palette.secondary, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    Text(state.caption, Rect(20, 202, 380, 220), 13,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_CENTER);

    const float labelRadius = radius + 22.0f;
    for (const GaugeTick& tick : kGaugeTicks) {
        double angle = (135.0 + 270.0 * tick.fraction) *
                       3.14159265358979323846 / 180.0;
        float x = cx + labelRadius * (float)std::cos(angle);
        float y = cy + labelRadius * (float)std::sin(angle);
        std::wstring label = FormatScale(tick.mbps) + (tick.fraction >= 1.0 ? L"+" : L"");
        Text(label, Rect(x - tick.width / 2, y - 9,
                         x + tick.width / 2, y + 9),
             11, WithAlpha(g_palette.tertiary, alpha),
             DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
             DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

static void DrawMetricLabel(float x, float y, PCWSTR glyph, D2D1_COLOR_F color,
                            PCWSTR label, float alpha) {
    Text(glyph, Rect(x, y - 1, x + 18, y + 17), 13, WithAlpha(color, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(label, Rect(x + 19, y, x + 160, y + 17), 12,
         WithAlpha(g_palette.secondary, alpha));
}

static void DrawValue(float x, float y, bool hasValue, double value, PCWSTR unit,
                      float fontSize, float alpha) {
    if (alpha <= 0.001f) {
        return;
    }
    std::wstring text = hasValue ? FormatNumber(value) : L"\x2014";
    Text(text, Rect(x, y, x + 170, y + fontSize * 1.35f), fontSize,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_SEMI_BOLD,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, 1, false, true);
    float width = MeasureWidth(text, fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD, 1, true);
    float unitTop = y + BaselineOffset(fontSize, 1) - BaselineOffset(12, 0);
    Text(unit, Rect(x + width + 5, unitTop, x + width + 80, unitTop + 18), 12,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

static void DrawLatency(float x, float y, PCWSTR label, bool hasValue, double value,
                        float alpha) {
    Text(label, Rect(x, y + 3, x + 60, y + 20), 12,
         WithAlpha(g_palette.secondary, alpha));
    std::wstring text = hasValue ? FormatNumber(value) : L"\x2014";
    Text(text, Rect(x + 42, y, x + 120, y + 20), 13,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_MEDIUM,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, 0, false, true);
    float width = MeasureWidth(text, 13, DWRITE_FONT_WEIGHT_MEDIUM, 0, true);
    float unitTop = y + BaselineOffset(13, 0) - BaselineOffset(11, 0);
    Text(L"ms", Rect(x + 42 + width + 4, unitTop, x + 150, unitTop + 17), 11,
         WithAlpha(g_palette.secondary, alpha));
}

static void DrawLiveStats(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }

    DrawLine(20, 282, 380, 282, WithAlpha(g_palette.cardStroke, alpha * 1.6f));

    // Only final results here; the live value is the gauge's. Each card
    // crossfades from the dash to its final value.
    float downIn = CardIn(g_downloadFinalAt);
    float upIn = CardIn(g_uploadFinalAt);

    DrawMetricLabel(20, 288, L"\xE896", g_palette.download, g_str->download, alpha);
    DrawMetricLabel(200, 288, L"\xE898", g_palette.upload, g_str->upload, alpha);
    DrawValue(20, 304, false, 0, L"Mbps", 32, alpha * (1.0f - downIn));
    DrawValue(20, 304, true, state.download, L"Mbps", 32, alpha * downIn);
    DrawValue(200, 304, false, 0, L"Mbps", 32, alpha * (1.0f - upIn));
    DrawValue(200, 304, true, state.upload, L"Mbps", 32, alpha * upIn);

    DrawLatency(20, 352, g_str->ping, state.hasPing, state.ping, alpha);
    DrawLatency(200, 352, g_str->jitter, state.hasJitter, state.jitter, alpha);
}

static void DrawSmallButton(D2D1_RECT_F rect, Hot id, PCWSTR glyph, PCWSTR label,
                            float alpha) {
    bool hover = g_hot == id;
    bool pressed = g_pressed == id;

    FillRounded(rect, 4,
                WithAlpha(g_palette.controlFill, alpha * (pressed ? 2.4f : (hover ? 1.8f : 1.0f))));
    StrokeRounded(rect, 4, WithAlpha(g_palette.cardStroke, alpha * 1.8f));

    Text(glyph, Rect(rect.left + 9, rect.top, rect.left + 24, rect.bottom), 12,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(label, Rect(rect.left + 26, rect.top, rect.right - 4, rect.bottom), 12,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (g_focus == id && FocusVisible()) {
        DrawFocusRing(rect, 4, alpha);
    }
}

// "↓ 1.870 Mbps": arrow in the series colour, tabular number, small unit on
// the number's baseline right after it.
static void DrawRowValue(float x, float y, PCWSTR arrow, D2D1_COLOR_F color, double value,
                         float alpha) {
    Text(arrow, Rect(x, y, x + 13, y + 22), 15, WithAlpha(color, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD);
    std::wstring number = FormatNumber(value);
    Text(number, Rect(x + 13, y, x + 100, y + 22), 16, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING,
         DWRITE_PARAGRAPH_ALIGNMENT_NEAR, 0, false, true);
    float width = MeasureWidth(number, 16, DWRITE_FONT_WEIGHT_SEMI_BOLD, 0, true);
    float unitTop = y + BaselineOffset(16, 0) - BaselineOffset(11, 0);
    Text(L"Mbps", Rect(x + 13 + width + 3, unitTop, x + 13 + width + 40, unitTop + 16), 11,
         WithAlpha(g_palette.secondary, alpha));
}

static void DrawHistoryRow(int index, const HistoryEntry& entry, float y, float alpha,
                           double count, ULONGLONG now) {
    D2D1_RECT_F rect = Rect(20.0f, y, 380.0f, y + kRowHeight);
    bool hover = g_hot == Hot::Row && g_hotRow == index;
    bool pressed = g_pressed == Hot::Row && g_pressedRow == index;
    FillRounded(rect, 6,
                WithAlpha(g_palette.cardFill, alpha * (pressed ? 1.9f : (hover ? 1.5f : 1.0f))));
    StrokeRounded(rect, 6,
                  index == 0 ? WithAlpha(g_palette.download, alpha * 0.45f)
                             : WithAlpha(g_palette.cardStroke, alpha));

    Text(EntryDay(entry.time, now), Rect(32, y + 8, 200, y + 24), 12,
         WithAlpha(g_palette.secondary, alpha));
    Text(Ellipsize(entry.server, 164.0f, 12, DWRITE_FONT_WEIGHT_NORMAL),
         Rect(200, y + 8, 368, y + 24), 12, WithAlpha(g_palette.secondary, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_TRAILING);

    // Download in an 84 DIP column, upload 6 DIP after it.
    DrawRowValue(32, y + 25, L"\x2193", g_palette.download, entry.download * count, alpha);
    DrawRowValue(32 + 84 + 6, y + 25, L"\x2191", g_palette.upload, entry.upload * count, alpha);
    Text(L"Ping " + FormatNumber(entry.ping) + L" ms", Rect(250, y + 29, 368, y + 45), 12,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_TRAILING);

    if (!entry.ip.empty()) {
        Text(Ellipsize(L"IP " + entry.ip, 200.0f, 11, DWRITE_FONT_WEIGHT_NORMAL),
             Rect(32, y + 49, 240, y + 64), 11, WithAlpha(g_palette.tertiary, alpha));
    }
    Text(std::wstring(g_str->viewResult) + L" \x2197", Rect(240, y + 49, 368, y + 64), 11,
         WithAlpha(g_palette.accent, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_TRAILING);

    if (g_focus == Hot::Row && g_focusRow == index && FocusVisible()) {
        DrawFocusRing(rect, 6, alpha);
    }
}

// End-of-test screen: header, then the rows cascading in (newest first).
// alpha < 1 is the 250 ms crossfade back to the gauge (Repetir).
static void DrawHistory(float alpha) {
    if (alpha <= 0.001f) {
        return;
    }
    double t = HistoryT();
    float headerAlpha = alpha * (float)EaseOutCubic(Phase01(t, 200, 350));
    if (headerAlpha > 0.001f) {
        Text(g_str->historyTitle, Rect(20, 47, 200, 73), 14, WithAlpha(g_palette.text, headerAlpha),
             DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING,
             DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        size_t count = g_history.size();
        std::wstring countText =
            std::to_wstring(count) + (count == 1 ? g_str->testSingular : g_str->testPlural);
        Text(countText, Rect(200, 47, kCopyRect.left - 10, 73), 12,
             WithAlpha(g_palette.secondary, headerAlpha), DWRITE_FONT_WEIGHT_NORMAL,
             DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        bool copied = Now() < g_copyUntil;
        DrawSmallButton(kCopyRect, Hot::Copy, copied ? L"\xE73E" : L"\xE8C8",
                        copied ? g_str->copied : g_str->copy, headerAlpha);
    }

    if (g_history.empty()) {
        Text(g_str->historyEmpty, Rect(20, kListTop, 380, kListBottom), 13,
             WithAlpha(g_palette.secondary, headerAlpha), DWRITE_FONT_WEIGHT_NORMAL,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        return;
    }

    SYSTEMTIME localNow = {};
    GetLocalTime(&localNow);
    ULONGLONG now = LocalTicks(localNow);
    double count = EaseOutCubic(Phase01(t, 400, 800));

    // Rows fade in with a small drop, so the clip leaves room above the list.
    g_renderTarget->PushAxisAlignedClip(Rect(0, kListTop - 2, kWidth, kListBottom),
                                        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    for (int i = 0; i < (int)g_history.size(); i++) {
        float top = RowTop(i);
        if (top > kListBottom || top + kRowHeight < kListTop - 16) {
            continue;
        }
        // Row 0 (this test) first; the others cascade 60 ms apart. The stagger
        // is capped so rows scrolled into view later are not left waiting.
        double start = i == 0 ? 250.0 : 350.0 + 60.0 * std::min(i, 6);
        float entered = (float)EaseOutCubic(Phase01(t, start, 400));
        float rowAlpha = alpha * entered;
        if (rowAlpha <= 0.001f) {
            continue;
        }
        float y = top - 14.0f * (1.0f - entered);
        DrawHistoryRow(i, g_history[(size_t)i], y, rowAlpha, i == 0 ? count : 1.0, now);
    }
    g_renderTarget->PopAxisAlignedClip();
}

// First run: explains what Ookla receives, links its terms, and asks for an
// explicit accept before the CLI is ever started with --accept-license/gdpr.
static void DrawConsent(float alpha) {
    FillRounded(kConsentRect, 8, WithAlpha(g_palette.cardFill, alpha));
    StrokeRounded(kConsentRect, 8, WithAlpha(g_palette.cardStroke, alpha));

    Text(L"\xE946", Rect(20, 76, 380, 106), 24, WithAlpha(g_palette.accent, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(g_str->consentTitle, Rect(20, 112, 380, 134), 15, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER);
    Text(g_str->consentBody, Rect(40, 142, 360, 252), 12,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 0, true);

    PCWSTR labels[3] = {g_str->consentLicense, g_str->consentTerms, g_str->consentPrivacy};
    for (int i = 0; i < 3; i++) {
        bool hover = g_hot == kLinkHots[i];
        Text(std::wstring(labels[i]) + L" \x2197", kLinkRects[i], 12,
             WithAlpha(hover ? Lighten(g_palette.accent, 0.2f) : g_palette.accent, alpha),
             DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
             DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if (g_focus == kLinkHots[i] && FocusVisible()) {
            DrawFocusRing(kLinkRects[i], 4, alpha);
        }
    }

    bool hover = g_hot == Hot::Accept;
    bool pressed = g_pressed == Hot::Accept;
    FillRounded(kAcceptRect, 4,
                WithAlpha(pressed ? Shade(g_palette.accent, 0.18f)
                                  : Lighten(g_palette.accent, hover ? 0.18f : 0.0f),
                          alpha));
    Text(g_str->consentAccept, kAcceptRect, 13, WithAlpha(g_palette.accentText, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (g_focus == Hot::Accept && FocusVisible()) {
        DrawFocusRing(kAcceptRect, 4, alpha);
    }
}

static void DrawError(float alpha, const TestState& state) {
    FillRounded(kErrorRect, 8, WithAlpha(g_palette.cardFill, alpha));
    StrokeRounded(kErrorRect, 8, WithAlpha(g_palette.cardStroke, alpha));

    Text(L"\xEA39", Rect(20, 74, 380, 106), 26, WithAlpha(g_palette.negative, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(g_str->failedTitle, Rect(20, 112, 380, 134), 15,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_SEMI_BOLD,
         DWRITE_TEXT_ALIGNMENT_CENTER);
    Text(state.error.empty() ? g_str->failedHint : state.error,
         Rect(40, 142, 360, 218), 12, WithAlpha(g_palette.secondary, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 0, true);

    bool hover = g_hot == Hot::Retry;
    bool pressed = g_pressed == Hot::Retry;
    FillRounded(kRetryRect, 4,
                WithAlpha(pressed ? Shade(g_palette.accent, 0.18f)
                                  : Lighten(g_palette.accent, hover ? 0.18f : 0.0f),
                          alpha));
    Text(g_str->retry, kRetryRect, 13, WithAlpha(g_palette.accentText, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (g_focus == Hot::Retry && FocusVisible()) {
        DrawFocusRing(kRetryRect, 4, alpha);
    }
}

static void DrawButton(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }
    bool running = StateRunning(state);
    PCWSTR label = StartButtonLabel(state);
    bool repeat = IsRepeatButton(state);
    bool hover = g_hot == Hot::Start;
    bool pressed = g_pressed == Hot::Start;

    D2D1_RECT_F rect = StartButtonRect(state);
    DWRITE_FONT_WEIGHT weight = running ? DWRITE_FONT_WEIGHT_NORMAL
                                        : DWRITE_FONT_WEIGHT_SEMI_BOLD;
    if (running) {
        float fill = pressed ? 3.0f : (hover ? 2.2f : 1.2f);
        FillRounded(rect, 4, WithAlpha(g_palette.controlFill, alpha * fill));
        StrokeRounded(rect, 4, WithAlpha(g_palette.cardStroke, alpha * 2.0f));
    } else {
        D2D1_COLOR_F fill = pressed ? Shade(g_palette.accent, 0.18f)
                                    : Lighten(g_palette.accent, hover ? 0.18f : 0.0f);
        FillRounded(rect, 4, WithAlpha(fill, alpha));
    }
    D2D1_COLOR_F textColor =
        running ? g_palette.text : g_palette.accentText;
    if (repeat) {
        float labelWidth = MeasureWidth(label, 13, weight);
        float contentWidth = 16.0f + 6.0f + labelWidth;
        float contentLeft = (rect.left + rect.right - contentWidth) / 2.0f;
        Text(L"\xE72C", Rect(contentLeft, rect.top, contentLeft + 16.0f, rect.bottom),
             14, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
        Text(label,
             Rect(contentLeft + 22.0f, rect.top,
                  contentLeft + contentWidth, rect.bottom),
             13, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    } else {
        Text(label, rect, 13, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    if (g_focus == Hot::Start && FocusVisible()) {
        DrawFocusRing(rect, 4, alpha);
    }
}

static void DrawTitle(float alpha) {
    Text(L"Speedtest by Ookla", Rect(16, 15, 240, 33), 12,
         WithAlpha(g_palette.text, alpha * 0.85f));

    bool hover = g_hot == Hot::Close;
    bool pressed = g_pressed == Hot::Close;
    if (hover || pressed) {
        FillRounded(kCloseRect, 4,
                    WithAlpha(g_palette.controlFill, alpha * (pressed ? 2.4f : 1.6f)));
    }
    Text(L"\xE711", kCloseRect, 11, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    if (g_focus == Hot::Close && FocusVisible()) {
        DrawFocusRing(kCloseRect, 4, alpha);
    }
}

static void DrawNetwork(float alpha) {
    D2D1_RECT_F row = FootRect();
    Text(g_network.glyph, Rect(row.left, row.top, row.left + 16, row.bottom), 11,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    if (!g_network.name.empty()) {
        std::wstring name = Ellipsize(g_network.name, row.right - row.left - 17.0f, 11,
                                      DWRITE_FONT_WEIGHT_NORMAL);
        Text(name, Rect(row.left + 17, row.top, row.right, row.bottom), 11,
             WithAlpha(g_palette.secondary, alpha));
    }
}

static void DrawView(View view, float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }
    switch (view) {
        case View::Gauge:
            DrawGauge(alpha, state);
            DrawLiveStats(alpha, state);
            break;
        case View::History:
            DrawHistory(alpha);
            break;
        case View::Error:
            DrawError(alpha, state);
            break;
        case View::Consent:
            DrawConsent(alpha);
            break;
    }
}

static void DrawFrame(const TestState& state) {
    if (!g_renderTarget) {
        return;
    }

    bool translucent = g_composition && g_translucent;
    D2D1_COLOR_F background = g_palette.background;
    float scrim = translucent ? ScrimAlpha() : 1.0f;
    background.a = scrim;
    g_renderTarget->Clear(background);

    if (translucent && scrim < 1.0f) {
        float tintAlpha =
            (g_dark ? kDarkTint : kLightTint) * (1.0f - scrim);
        D2D1_COLOR_F tint =
            g_dark ? Col(0x000000, tintAlpha) : Col(0xFFFFFF, tintAlpha);
        FillRect(Rect(-2.0f, -2.0f, kWidth + 2.0f, g_layoutHeight + 2.0f), tint);

        if (g_dark && EnsureNoise()) {
            g_noiseBrush->SetOpacity(kNoiseOpacity * (1.0f - scrim));
            g_renderTarget->FillRectangle(
                Rect(-2.0f, -2.0f, kWidth + 2.0f, g_layoutHeight + 2.0f),
                g_noiseBrush);
        }
    }

    float buttonAlpha = g_animating ? AnimIn() : 1.0f;
    if (g_view == View::History) {
        // End-of-test timeline: the gauge rises and fades out while the
        // history header and rows fade in (see DrawHistory).
        double t = HistoryT();
        float out = 1.0f - (float)EaseOutCubic(Phase01(t, 0, 350));
        if (out > 0.001f && g_animFrom == View::Gauge) {
            D2D1_MATRIX_3X2_F transform;
            g_renderTarget->GetTransform(&transform);
            g_renderTarget->SetTransform(
                D2D1::Matrix3x2F::Translation(0, -12.0f * (1.0f - out)) * transform);
            DrawView(View::Gauge, out, state);
            g_renderTarget->SetTransform(transform);
        }
        DrawHistory(1.0f);
        buttonAlpha = (float)EaseOutCubic(Phase01(t, 1000, 400));
    } else if (g_animating) {
        DrawView(g_animFrom, AnimOut(), state);
        DrawView(g_view, AnimIn(), state);
    } else {
        DrawView(g_view, 1.0f, state);
    }

    // The consent card has its own Accept button and the error card its Retry.
    auto hasStartButton = [](View view) {
        return view != View::Error && view != View::Consent;
    };
    if (hasStartButton(g_view) || (g_animating && hasStartButton(g_animFrom))) {
        DrawButton(buttonAlpha, state);
    }

    DrawNetwork(1.0f);
    DrawTitle(1.0f);
}

// ---------------------------------------------------------------------------
// Direct2D lifetime
// ---------------------------------------------------------------------------

static void ReleaseTargets() {
    ReleaseBrushes();
    ReleaseNoise();
    if (g_blurEffect) {
        g_blurEffect->Release();
        g_blurEffect = nullptr;
    }
    if (g_contextBitmap) {
        g_contextBitmap->Release();
        g_contextBitmap = nullptr;
    }
    if (g_context) {
        g_context->SetTarget(nullptr);
        g_context->Release();
        g_context = nullptr;
    }
    if (g_renderTarget) {
        g_renderTarget->Release();
        g_renderTarget = nullptr;
    }
    if (g_hwndTarget) {
        g_hwndTarget->Release();
        g_hwndTarget = nullptr;
    }
    if (g_dcompVisual) {
        g_dcompVisual->Release();
        g_dcompVisual = nullptr;
    }
    if (g_dcompTarget) {
        g_dcompTarget->Release();
        g_dcompTarget = nullptr;
    }
    if (g_dcompDevice) {
        g_dcompDevice->Release();
        g_dcompDevice = nullptr;
    }
    if (g_swapChain) {
        g_swapChain->Release();
        g_swapChain = nullptr;
    }
    if (g_dxgiDevice) {
        g_dxgiDevice->Release();
        g_dxgiDevice = nullptr;
    }
    if (g_d3dDevice) {
        g_d3dDevice->Release();
        g_d3dDevice = nullptr;
    }
    g_composition = false;
    g_targetWidth = 0;
    g_targetHeight = 0;
    g_targetDpi = 0;
}

static void ReleaseGraphics() {
    ReleaseTargets();
    if (g_roundStroke) {
        g_roundStroke->Release();
        g_roundStroke = nullptr;
    }
    for (auto& entry : g_textFormats) {
        entry.second->Release();
    }
    g_textFormats.clear();
    if (g_dwriteFactory) {
        g_dwriteFactory->Release();
        g_dwriteFactory = nullptr;
    }
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
}

static bool BindCompositionTarget() {
    if (!g_context || !g_swapChain) {
        return false;
    }
    if (g_contextBitmap) {
        g_contextBitmap->Release();
        g_contextBitmap = nullptr;
    }

    IDXGISurface* surface = nullptr;
    if (FAILED(g_swapChain->GetBuffer(0, __uuidof(IDXGISurface),
                                      (void**)&surface))) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 properties = {};
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.dpiX = g_dpi;
    properties.dpiY = g_dpi;
    properties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    HRESULT hr = g_context->CreateBitmapFromDxgiSurface(surface, &properties,
                                                        &g_contextBitmap);
    surface->Release();
    if (FAILED(hr)) {
        return false;
    }

    g_context->SetTarget(g_contextBitmap);
    g_context->SetDpi(g_dpi, g_dpi);
    g_context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    g_context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_targetDpi = g_dpi;
    return true;
}

static bool CreateCompositionTarget(HWND hwnd, UINT width, UINT height) {
    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0,
                                  D3D_FEATURE_LEVEL_10_1};
    // Hardware first; WARP keeps VMs and RDP sessions on the composition path.
    HRESULT d3dHr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 2,
                                   D3D11_SDK_VERSION, &g_d3dDevice, nullptr,
                                   nullptr);
    if (FAILED(d3dHr)) {
        d3dHr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 2,
                               D3D11_SDK_VERSION, &g_d3dDevice, nullptr,
                               nullptr);
    }
    if (FAILED(d3dHr)) {
        return false;
    }
    if (FAILED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice),
                                           (void**)&g_dxgiDevice))) {
        return false;
    }

    ID2D1Device* d2dDevice = nullptr;
    ID2D1Factory1* factory1 = nullptr;
    if (FAILED(g_d2dFactory->QueryInterface(__uuidof(ID2D1Factory1),
                                            (void**)&factory1))) {
        return false;
    }
    // Same factory as the stroke style and geometries, or draws fail with WRONG_FACTORY.
    HRESULT hr = factory1->CreateDevice(g_dxgiDevice, &d2dDevice);
    factory1->Release();
    if (FAILED(hr)) {
        return false;
    }
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                        &g_context);
    d2dDevice->Release();
    if (FAILED(hr)) {
        return false;
    }

    IDXGIAdapter* adapter = nullptr;
    if (FAILED(g_dxgiDevice->GetAdapter(&adapter))) {
        return false;
    }
    IDXGIFactory2* factory = nullptr;
    hr = adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory);
    adapter->Release();
    if (FAILED(hr)) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 description = {};
    description.Width = width;
    description.Height = height;
    description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.SampleDesc.Count = 1;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = 2;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    hr = factory->CreateSwapChainForComposition(g_d3dDevice, &description,
                                                nullptr, &g_swapChain);
    factory->Release();
    if (FAILED(hr)) {
        return false;
    }

    if (FAILED(DCompositionCreateDevice(g_dxgiDevice,
                                        __uuidof(IDCompositionDevice),
                                        (void**)&g_dcompDevice))) {
        return false;
    }
    if (FAILED(g_dcompDevice->CreateTargetForHwnd(hwnd, TRUE, &g_dcompTarget)) ||
        FAILED(g_dcompDevice->CreateVisual(&g_dcompVisual))) {
        return false;
    }
    if (FAILED(g_dcompVisual->SetContent(g_swapChain)) ||
        FAILED(g_dcompTarget->SetRoot(g_dcompVisual)) ||
        FAILED(g_dcompDevice->Commit())) {
        return false;
    }

    g_composition = true;
    g_targetWidth = width;
    g_targetHeight = height;
    if (!BindCompositionTarget()) {
        return false;
    }

    return SUCCEEDED(g_context->QueryInterface(__uuidof(ID2D1RenderTarget),
                                               (void**)&g_renderTarget));
}

static bool EnsureRenderTarget() {
    if (g_renderTarget) {
        return true;
    }
    if (!g_d2dFactory || !g_flyoutHwnd) {
        return false;
    }

    RECT client = {};
    GetClientRect(g_flyoutHwnd, &client);
    UINT width = (UINT)std::max<LONG>(1, client.right - client.left);
    UINT height = (UINT)std::max<LONG>(1, client.bottom - client.top);

    if (CreateCompositionTarget(g_flyoutHwnd, width, height)) {
        Wh_Log(L"render target: composition (acrylic) %ux%u", width, height);
        return true;
    }
    ReleaseTargets();

    D2D1_RENDER_TARGET_PROPERTIES properties = {};
    properties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    properties.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    properties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    properties.dpiX = g_dpi;
    properties.dpiY = g_dpi;

    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProperties = {};
    hwndProperties.hwnd = g_flyoutHwnd;
    hwndProperties.pixelSize = {width, height};
    hwndProperties.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

    if (FAILED(g_d2dFactory->CreateHwndRenderTarget(properties, hwndProperties,
                                                    &g_hwndTarget))) {
        g_hwndTarget = nullptr;
        return false;
    }
    if (FAILED(g_hwndTarget->QueryInterface(__uuidof(ID2D1RenderTarget),
                                            (void**)&g_renderTarget))) {
        return false;
    }
    g_targetWidth = width;
    g_targetHeight = height;
    Wh_Log(L"render target: hwnd %ux%u (translucent %d)", width, height,
        g_translucent ? 1 : 0);
    return true;
}

static void ResizeRenderTarget() {
    if (!g_renderTarget || !g_flyoutHwnd) {
        return;
    }
    RECT client = {};
    GetClientRect(g_flyoutHwnd, &client);
    UINT width = (UINT)std::max<LONG>(1, client.right - client.left);
    UINT height = (UINT)std::max<LONG>(1, client.bottom - client.top);

    if (g_composition && g_context && g_swapChain) {
        if (width == g_targetWidth && height == g_targetHeight &&
            g_dpi == g_targetDpi) {
            return;
        }
        g_context->SetTarget(nullptr);
        if (g_contextBitmap) {
            g_contextBitmap->Release();
            g_contextBitmap = nullptr;
        }
        if (SUCCEEDED(g_swapChain->ResizeBuffers(0, width, height,
                                                 DXGI_FORMAT_UNKNOWN, 0)) &&
            BindCompositionTarget()) {
            g_targetWidth = width;
            g_targetHeight = height;
            return;
        }
        ReleaseTargets();
        return;
    }

    if (g_hwndTarget) {
        g_hwndTarget->Resize({width, height});
        g_hwndTarget->SetDpi(g_dpi, g_dpi);
        g_targetWidth = width;
        g_targetHeight = height;
        g_targetDpi = g_dpi;
    }
}

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

static void BeginView(View target, bool animate = true) {
    if (target == g_view && !g_animating) {
        return;
    }
    View previous = g_animating ? g_animFrom : g_view;
    g_animFrom = previous;
    g_view = target;
    g_heightFrom = g_layoutHeight;
    g_animStart = Now();
    g_animating = animate && previous != target && AnimationsEnabled();
    if (target == View::History) {
        // The history runs its own longer timeline instead of the crossfade.
        g_animating = false;
        g_historyShownAt = animate && AnimationsEnabled() ? Now() : 0;
        g_scroll = {};
        g_scrollTarget = 0;
        g_focusRow = 0;
    }
    if (g_view != previous) {
        g_focus = Hot::None;
        if (g_flyoutHwnd) {
            NotifyWinEvent(EVENT_OBJECT_REORDER, g_flyoutHwnd, OBJID_CLIENT,
                           CHILDID_SELF);
        }
    }
}

// Adds the run that just finished to the top of the history and saves it.
static void RecordHistory(const TestState& state) {
    HistoryEntry entry = {LocalTicks(state.when), state.externalIp, state.server,
                          state.resultUrl,        state.download,   state.upload,
                          state.hasPing ? state.ping : 0.0,
                          state.hasJitter ? state.jitter : 0.0};
    PushHistory(g_history, std::move(entry));
    SaveHistory();
}

static void CopyResult(HWND hwnd, const TestState& state) {
    if (!state.hasResult) {
        return;
    }
    std::wstring text =
        L"Speedtest by Ookla \x00B7 Download " + FormatNumber(state.download) +
        L" Mbps \x00B7 Upload " + FormatNumber(state.upload) + L" Mbps \x00B7 Ping " +
        (state.hasPing ? FormatNumber(state.ping) : L"\x2014") + L" ms \x00B7 Jitter " +
        (state.hasJitter ? FormatNumber(state.jitter) : L"\x2014") + L" ms \x00B7 " +
        FormatData(state.dataUsed) + g_str->dataSuffix;
    text += L"\r\n";
    text += state.server + L" \x00B7 " + FormatDate(state.when);
    if (!state.resultUrl.empty()) {
        text += L"\r\n" + state.resultUrl;
    }

    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) {
        Wh_Log(L"Clipboard allocation failed (error %u)", GetLastError());
        return;
    }

    void* target = GlobalLock(memory);
    if (!target) {
        Wh_Log(L"Clipboard memory lock failed (error %u)", GetLastError());
        GlobalFree(memory);
        return;
    }
    memcpy(target, text.c_str(), bytes);
    GlobalUnlock(memory);

    bool copied = false;
    DWORD error = ERROR_SUCCESS;
    for (int attempt = 0; attempt < 5; attempt++) {
        if (OpenClipboard(hwnd)) {
            if (EmptyClipboard()) {
                if (SetClipboardData(CF_UNICODETEXT, memory)) {
                    memory = nullptr;
                    copied = true;
                } else {
                    error = GetLastError();
                }
            } else {
                error = GetLastError();
            }
            CloseClipboard();
            break;
        }
        error = GetLastError();
        Sleep(20);
    }
    if (memory) {
        GlobalFree(memory);
    }
    if (copied) {
        g_copyUntil = Now() + 1600;
    } else {
        Wh_Log(L"Copying test results to the clipboard failed (error %u)",
               error);
    }
    InvalidateRect(hwnd, nullptr, FALSE);
}

static void OpenResultPage(const std::wstring& url) {
    // Only an https speedtest.net link from the CLI is ever opened.
    if (url.rfind(L"https://www.speedtest.net/", 0) != 0) {
        return;
    }
    ShellExecuteW_t open = ShellExecuteW_Original ? ShellExecuteW_Original : ShellExecuteW;
    INT_PTR result = (INT_PTR)open(nullptr, L"open", url.c_str(), nullptr,
                                   nullptr, SW_SHOWNORMAL);
    if (result <= 32) {
        Wh_Log(L"Opening the result page failed (%d)", (int)result);
    }
}

static void StartOrCancelTest(HWND hwnd) {
    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        return;
    }

    TestState state = CopyState();
    if (StateRunning(state)) {
        RequestTestCancel();
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    // No test (and no license/GDPR flags) until the user accepted in the flyout.
    // Focus is left empty on purpose: a stray or auto-repeated Enter must not
    // accept a card the user has not read; Tab reaches the button.
    if (!HasOoklaConsent()) {
        BeginView(View::Consent);
        g_focus = Hot::None;
        SetTimer(hwnd, 1, 16, nullptr);
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    if (g_testThread) {
        // The previous worker may still be unwinding; it only cleans up now.
        DWORD wait = WaitForSingleObject(g_testThread, 2000);
        if (wait != WAIT_OBJECT_0) {
            Wh_Log(L"Previous test thread wait result %lu (error %u)", wait,
                   GetLastError());
        }
        CloseHandle(g_testThread);
        g_testThread = nullptr;
    }

    StateReset();
    // Mark the run as active before the worker starts, so an early focus
    // change cannot dismiss the flyout mid-test.
    StateSetPhase(Phase::Warmup, g_str->starting);
    g_cancelRequested.store(0, std::memory_order_relaxed);
    g_copyUntil = 0;
    g_gaugeSpring = {};
    g_numberSpring = {};
    g_historySaved = false;
    g_downloadFinalAt = 0;
    g_uploadFinalAt = 0;
    g_network = ReadNetwork();
    BeginView(View::Gauge);

    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        g_cancelRequested.store(1, std::memory_order_relaxed);
        StateCanceled();
        return;
    }

    g_testThread = CreateThread(nullptr, 0, TestWorkerThread, nullptr, 0, nullptr);
    if (!g_testThread) {
        DWORD error = GetLastError();
        Wh_Log(L"Speed test worker creation failed (error %u)", error);
        StateFail(g_str->errUnexpected);
    }
    SetTimer(hwnd, 1, 16, nullptr);
    InvalidateRect(hwnd, nullptr, FALSE);
}

// ---------------------------------------------------------------------------
// Show / hide / paint
// ---------------------------------------------------------------------------

// DWM Desktop Acrylic; the attribute must *change* for DWM to recompute the
// material, so it is always set to NONE while hidden.
static void ApplyBackdrop(HWND hwnd, bool enable) {
    BOOL dark = g_dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark,
                          sizeof(dark));

    int backdrop = enable ? 3 /* DWMSBT_TRANSIENTWINDOW */
                          : 1 /* DWMSBT_NONE */;
    DwmSetWindowAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop,
                          sizeof(backdrop));
}

static void RefreshBackdrop(HWND hwnd) {
    bool wasTranslucent = g_translucent;
    RefreshAppearance();
    if (g_translucent == wasTranslucent) {
        ApplyBackdrop(hwnd, g_translucent);
        return;
    }
    ApplyBackdrop(hwnd, false);
    if (g_translucent) {
        ApplyBackdrop(hwnd, true);
        g_scrimFadeAt = Now();
    } else {
        g_scrimFadeAt = 0;
    }
    InvalidateRect(hwnd, nullptr, FALSE);
}

static HMONITOR MonitorUnderCursor() {
    POINT cursor = {};
    GetCursorPos(&cursor);
    return MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
}

static void PositionFlyout(HWND hwnd, HMONITOR monitor) {
    MONITORINFO info = {sizeof(info)};
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return;
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    g_dpi = (float)(dpiX ? dpiX : 96);
    g_scale = g_dpi / 96.0f;

    int width = (int)std::lround(kWidth * g_scale);
    int height = (int)std::lround(g_layoutHeight * g_scale);
    int margin = (int)std::lround(12 * g_scale);
    int x = info.rcWork.right - width - margin;
    int y = info.rcWork.bottom - height - margin;

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE);
    ResizeRenderTarget();
}

// True when Windows reports the connection as metered; failures count as unmetered.
static bool IsConnectionMetered() {
    INetworkCostManager* costManager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&costManager));
    if (FAILED(hr)) {
        Wh_Log(L"NetworkCostManager unavailable (hr 0x%08X)", (unsigned)hr);
        return false;
    }
    DWORD cost = NLM_CONNECTION_COST_UNKNOWN;
    hr = costManager->GetCost(&cost, nullptr);
    costManager->Release();
    if (FAILED(hr)) {
        Wh_Log(L"GetCost failed (hr 0x%08X)", (unsigned)hr);
        return false;
    }
    bool metered = (cost & (NLM_CONNECTION_COST_FIXED |
                            NLM_CONNECTION_COST_VARIABLE |
                            NLM_CONNECTION_COST_ROAMING)) != 0;
    Wh_Log(L"connection cost %u -> metered %d", (unsigned)cost, metered);
    return metered;
}

static void ShowFlyoutWindow(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    bool startTest = !TestRunning();
    if (startTest) {
        StateReset();
        g_gaugeSpring = {};
        g_numberSpring = {};
        g_historySaved = false;
        g_historyShownAt = 0;
        g_downloadFinalAt = 0;
        g_uploadFinalAt = 0;
        g_view = View::Gauge;
        g_animating = false;
        g_copyUntil = 0;
        g_hot = Hot::None;
        g_pressed = Hot::None;
        g_focus = Hot::None;
        g_network = ReadNetwork();

        int startMode = g_autoStart.load(std::memory_order_relaxed);
        if (!HasOoklaConsent()) {
            // First run: the consent card instead of a test (no initial focus,
            // see StartOrCancelTest).
            g_view = View::Consent;
            g_focus = Hot::None;
            startTest = false;
        } else if (startMode == 0 && IsConnectionMetered()) {
            startMode = 2;
        }
        if (startTest && startMode == 2) {
            StateSetCaption(g_str->ready);
            g_focus = Hot::Start;
            startTest = false;
        }
    }
    g_layoutHeight = ViewHeight(g_view);
    g_heightFrom = g_layoutHeight;

    RefreshAppearance();

    // Present one opaque frame before the window is visible; the material is
    // turned on only afterwards, when DWM will attach it.
    ApplyBackdrop(hwnd, false);
    g_scrimFadeAt = 0;

    PositionFlyout(hwnd, MonitorUnderCursor());
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd);

    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_SHOWWINDOW);
    g_previousForeground = GetForegroundWindow();
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    g_uiVisible = true;
    g_mouseButtonsDown = MouseButtonsDown();
    SetTimer(hwnd, 1, 16, nullptr);
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd);

    if (g_translucent) {
        ApplyBackdrop(hwnd, true);
        g_scrimFadeAt = Now();
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    if (startTest) {
        StartOrCancelTest(hwnd);
    }
}

static void HideFlyoutWindow(HWND hwnd, bool restoreForeground = true) {
    if (!hwnd) {
        return;
    }
    ShowWindow(hwnd, SW_HIDE);
    KillTimer(hwnd, 1);
    // Free the D3D device, swap chain and DirectComposition objects once the
    // flyout has stayed hidden for a minute (timer 2): reopening right away
    // keeps them, so it is fast and never shows an empty first frame.
    SetTimer(hwnd, 2, 60 * 1000, nullptr);
    ApplyBackdrop(hwnd, false);
    g_scrimFadeAt = 0;
    g_uiVisible = false;
    g_hot = Hot::None;
    g_pressed = Hot::None;
    g_focus = Hot::None;
    if (restoreForeground && g_previousForeground && g_previousForeground != hwnd &&
        IsWindow(g_previousForeground)) {
        SetForegroundWindow(g_previousForeground);
    }
    g_previousForeground = nullptr;
}

// ---------------------------------------------------------------------------
// Actions and keyboard focus
// ---------------------------------------------------------------------------

// One entry point for mouse clicks, keyboard activation and MSAA.
static void ActivateHot(HWND hwnd, Hot hot) {
    switch (hot) {
        case Hot::Close:
            if (TestRunning()) {
                RequestTestCancel();
            }
            HideFlyoutWindow(hwnd);
            break;
        case Hot::Start:
        case Hot::Retry:
            StartOrCancelTest(hwnd);
            break;
        case Hot::Copy:
            CopyResult(hwnd, CopyState());
            break;
        case Hot::Row:
            if (g_focusRow >= 0 && g_focusRow < (int)g_history.size()) {
                OpenResultPage(g_history[(size_t)g_focusRow].url);
            }
            break;
        case Hot::Accept:
            if (SaveOoklaConsent()) {
                StartOrCancelTest(hwnd);
            } else {
                // Without a stored consent Accept would only loop back here.
                StateFail(g_str->errUnexpected);
                BeginView(View::Error);
            }
            break;
        case Hot::LinkLicense:
        case Hot::LinkTerms:
        case Hot::LinkPrivacy:
            OpenResultPage(kOoklaUrls[LinkIndex(hot)]);
            break;
        default:
            break;
    }
}

static int AccChildId(Hot hot) {
    switch (hot) {
        case Hot::Close:
            return 1;
        case Hot::Start:
        case Hot::Retry:
            return 2;
        case Hot::Copy:
            return 3;
        case Hot::Row:
            return 4;
        case Hot::Accept:
            return 5;
        case Hot::LinkLicense:
            return 6;
        case Hot::LinkTerms:
            return 7;
        case Hot::LinkPrivacy:
            return 8;
        default:
            return 0;
    }
}

static Hot HotFromAccChild(int id) {
    switch (id) {
        case 1:
            return Hot::Close;
        case 2:
            return g_view == View::Error ? Hot::Retry : Hot::Start;
        case 3:
            return Hot::Copy;
        case 4:
            return Hot::Row;
        case 5:
            return Hot::Accept;
        case 6:
            return Hot::LinkLicense;
        case 7:
            return Hot::LinkTerms;
        case 8:
            return Hot::LinkPrivacy;
        default:
            return Hot::None;
    }
}

// The history list is one Tab stop; arrows move between its rows.
static std::vector<Hot> FocusOrder() {
    std::vector<Hot> order;
    order.push_back(Hot::Close);
    if (g_view == View::Error) {
        order.push_back(Hot::Retry);
        return order;
    }
    if (g_view == View::Consent) {
        order.insert(order.end(), std::begin(kLinkHots), std::end(kLinkHots));
        order.push_back(Hot::Accept);
        return order;
    }
    if (g_view == View::History) {
        order.push_back(Hot::Copy);
        if (!g_history.empty()) {
            order.push_back(Hot::Row);
        }
    }
    order.push_back(Hot::Start);
    return order;
}

// Screen-reader text for the focused history row.
static std::wstring RowAccName(int index) {
    if (index < 0 || index >= (int)g_history.size()) {
        return L"";
    }
    const HistoryEntry& e = g_history[(size_t)index];
    SYSTEMTIME localNow = {};
    GetLocalTime(&localNow);
    return EntryDay(e.time, LocalTicks(localNow)) + g_str->accDownload + FormatNumber(e.download) +
           g_str->accUpload + FormatNumber(e.upload) + g_str->accPing + FormatNumber(e.ping) +
           L" ms, " + e.server;
}

static void MoveFocus(int delta) {
    std::vector<Hot> order = FocusOrder();
    if (order.empty()) {
        return;
    }
    int index = -1;
    for (size_t i = 0; i < order.size(); i++) {
        if (order[i] == g_focus) {
            index = (int)i;
        }
    }
    int next = index < 0 ? (delta > 0 ? 0 : (int)order.size() - 1)
                         : (index + delta + (int)order.size()) % (int)order.size();
    if (order[next] == g_focus) {
        return;
    }
    g_focus = order[next];
    NotifyWinEvent(EVENT_OBJECT_FOCUS, g_flyoutHwnd, OBJID_CLIENT,
                   AccChildId(g_focus));
    InvalidateRect(g_flyoutHwnd, nullptr, FALSE);
}

static bool IsAvailableHot(Hot hot) {
    switch (hot) {
        case Hot::Close:
            return true;
        case Hot::Start:
            return g_view != View::Error && g_view != View::Consent;
        case Hot::Accept:
        case Hot::LinkLicense:
        case Hot::LinkTerms:
        case Hot::LinkPrivacy:
            return g_view == View::Consent;
        case Hot::Copy:
            return g_view == View::History;
        case Hot::Row:
            return g_view == View::History && !g_history.empty();
        case Hot::Retry:
            return g_view == View::Error;
        default:
            return false;
    }
}

static D2D1_RECT_F HotRect(Hot hot) {
    switch (hot) {
        case Hot::Start:
            return StartButtonRect(CopyState());
        case Hot::Copy:
            return kCopyRect;
        case Hot::Row:
            return RowRect(g_focusRow);
        case Hot::Accept:
            return kAcceptRect;
        case Hot::LinkLicense:
        case Hot::LinkTerms:
        case Hot::LinkPrivacy:
            return kLinkRects[LinkIndex(hot)];
        case Hot::Close:
            return kCloseRect;
        case Hot::Retry:
            return kRetryRect;
        default:
            return Rect(0, 0, 0, 0);
    }
}

class FlyoutAccessible final : public IAccessible {
public:
    explicit FlyoutAccessible(HWND hwnd) : hwnd_(hwnd) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }
        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IAccessible) {
            *object = static_cast<IAccessible*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&references_);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG remaining = InterlockedDecrement(&references_);
        if (remaining == 0) {
            delete this;
        }
        return (ULONG)remaining;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (!count) {
            return E_POINTER;
        }
        *count = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID,
                                             DISPID*) override {
        return DISP_E_UNKNOWNNAME;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*,
                                     VARIANT*, EXCEPINFO*, UINT*) override {
        return DISP_E_MEMBERNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE get_accParent(IDispatch** parent) override {
        if (!parent) {
            return E_POINTER;
        }
        *parent = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accChildCount(long* count) override {
        if (!count) {
            return E_POINTER;
        }
        *count = (long)FocusOrder().size();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accChild(VARIANT, IDispatch** child) override {
        if (!child) {
            return E_POINTER;
        }
        *child = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accName(VARIANT child, BSTR* name) override {
        if (!name) {
            return E_POINTER;
        }
        PCWSTR value = nullptr;
        if (IsSelf(child)) {
            value = L"Speedtest";
        } else {
            Hot hot = ChildHot(child);
            if (!IsAvailableHot(hot)) {
                return E_INVALIDARG;
            }
            switch (hot) {
                case Hot::Close:
                    value = g_str->closeName;
                    break;
                case Hot::Copy:
                    value = g_str->copy;
                    break;
                case Hot::Row: {
                    std::wstring rowName = RowAccName(g_focusRow);
                    *name = SysAllocString(rowName.c_str());
                    return *name ? S_OK : E_OUTOFMEMORY;
                }
                case Hot::Retry:
                    value = g_str->retry;
                    break;
                case Hot::Accept:
                    value = g_str->consentAccept;
                    break;
                case Hot::LinkLicense:
                    value = g_str->consentLicense;
                    break;
                case Hot::LinkTerms:
                    value = g_str->consentTerms;
                    break;
                case Hot::LinkPrivacy:
                    value = g_str->consentPrivacy;
                    break;
                case Hot::Start: {
                    TestState state = CopyState();
                    value = StartButtonLabel(state);
                    break;
                }
                default:
                    return E_INVALIDARG;
            }
        }
        *name = SysAllocString(value);
        return *name ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE get_accValue(VARIANT child, BSTR* value) override {
        if (!value) {
            return E_POINTER;
        }
        if (!IsSelf(child)) {
            *value = nullptr;
            return S_FALSE;
        }
        TestState state = CopyState();
        std::wstring status = state.caption;
        if (state.phase == Phase::Download || state.phase == Phase::Upload) {
            status += L" ";
            status += FormatNumber(state.currentMbps);
            status += L" Mbps";
        }
        *value = SysAllocString(status.c_str());
        return *value ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE put_accValue(VARIANT, BSTR) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE get_accDescription(VARIANT child,
                                                  BSTR* description) override {
        if (!description) {
            return E_POINTER;
        }
        *description = nullptr;
        return IsValidChild(child) ? S_FALSE : E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE get_accRole(VARIANT child, VARIANT* role) override {
        if (!role) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }
        VariantInit(role);
        role->vt = VT_I4;
        role->lVal = IsSelf(child) ? ROLE_SYSTEM_WINDOW : ROLE_SYSTEM_PUSHBUTTON;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accState(VARIANT child, VARIANT* state) override {
        if (!state) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }
        VariantInit(state);
        state->vt = VT_I4;
        if (!IsSelf(child)) {
            Hot hot = ChildHot(child);
            state->lVal = STATE_SYSTEM_FOCUSABLE;
            if (hot == g_focus && GetFocus() == hwnd_) {
                state->lVal |= STATE_SYSTEM_FOCUSED;
            }
        } else {
            state->lVal = 0;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accHelp(VARIANT, BSTR* help) override {
        if (!help) {
            return E_POINTER;
        }
        *help = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accHelpTopic(BSTR* helpFile, VARIANT,
                                                long* topic) override {
        if (!helpFile || !topic) {
            return E_POINTER;
        }
        *helpFile = nullptr;
        *topic = 0;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut(VARIANT child,
                                                       BSTR* shortcut) override {
        if (!shortcut) {
            return E_POINTER;
        }
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        *shortcut = SysAllocString(g_str->shortcut);
        return *shortcut ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE get_accFocus(VARIANT* child) override {
        if (!child) {
            return E_POINTER;
        }
        VariantInit(child);
        if (g_focus == Hot::None || !IsAvailableHot(g_focus)) {
            child->vt = VT_EMPTY;
        } else {
            child->vt = VT_I4;
            child->lVal = AccChildId(g_focus);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accSelection(VARIANT* selection) override {
        return get_accFocus(selection);
    }

    HRESULT STDMETHODCALLTYPE get_accDefaultAction(VARIANT child,
                                                    BSTR* action) override {
        if (!action) {
            return E_POINTER;
        }
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        *action = SysAllocString(g_str->defaultAction);
        return *action ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE accSelect(long flags, VARIANT child) override {
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        if (flags & SELFLAG_TAKEFOCUS) {
            Hot hot = ChildHot(child);
            g_focus = hot;
            SetFocus(hwnd_);
            NotifyWinEvent(EVENT_OBJECT_FOCUS, hwnd_, OBJID_CLIENT, AccChildId(hot));
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accLocation(long* left, long* top, long* width,
                                          long* height, VARIANT child) override {
        if (!left || !top || !width || !height) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }

        RECT windowRect = {};
        if (!GetWindowRect(hwnd_, &windowRect)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        if (IsSelf(child)) {
            *left = windowRect.left;
            *top = windowRect.top;
            *width = windowRect.right - windowRect.left;
            *height = windowRect.bottom - windowRect.top;
            return S_OK;
        }

        D2D1_RECT_F rect = HotRect(ChildHot(child));
        *left = windowRect.left + (long)std::lround(rect.left * g_scale);
        *top = windowRect.top + (long)std::lround(rect.top * g_scale);
        *width = (long)std::lround((rect.right - rect.left) * g_scale);
        *height = (long)std::lround((rect.bottom - rect.top) * g_scale);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accNavigate(long direction, VARIANT start,
                                           VARIANT* end) override {
        if (!end) {
            return E_POINTER;
        }
        VariantInit(end);
        std::vector<Hot> order = FocusOrder();
        if (order.empty()) {
            return S_FALSE;
        }
        int index = -1;
        if (start.vt == VT_I4 && start.lVal != CHILDID_SELF) {
            Hot current = HotFromAccChild(start.lVal);
            for (size_t i = 0; i < order.size(); i++) {
                if (order[i] == current) {
                    index = (int)i;
                    break;
                }
            }
        }
        int next = -1;
        if (direction == NAVDIR_FIRSTCHILD) {
            next = 0;
        } else if (direction == NAVDIR_LASTCHILD) {
            next = (int)order.size() - 1;
        } else if (direction == NAVDIR_NEXT && index >= 0 &&
                   index + 1 < (int)order.size()) {
            next = index + 1;
        } else if (direction == NAVDIR_PREVIOUS && index > 0) {
            next = index - 1;
        }
        if (next < 0) {
            return S_FALSE;
        }
        end->vt = VT_I4;
        end->lVal = AccChildId(order[(size_t)next]);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accHitTest(long x, long y, VARIANT* child) override {
        if (!child) {
            return E_POINTER;
        }
        VariantInit(child);
        POINT point = {x, y};
        if (!ScreenToClient(hwnd_, &point)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        Hot hot = HitTest((float)point.x / g_scale, (float)point.y / g_scale);
        child->vt = VT_I4;
        child->lVal = hot == Hot::None ? CHILDID_SELF : AccChildId(hot);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accDoDefaultAction(VARIANT child) override {
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        return PostMessageW(hwnd_, WM_APP + 3, AccChildId(ChildHot(child)), 0)
                   ? S_OK
                   : HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT STDMETHODCALLTYPE put_accName(VARIANT, BSTR) override {
        return E_NOTIMPL;
    }

private:
    bool IsSelf(const VARIANT& child) const {
        return child.vt == VT_I4 && child.lVal == CHILDID_SELF;
    }

    Hot ChildHot(const VARIANT& child) const {
        return child.vt == VT_I4 ? HotFromAccChild(child.lVal) : Hot::None;
    }

    bool IsValidChild(const VARIANT& child) const {
        return IsSelf(child) || IsAvailableHot(ChildHot(child));
    }

    volatile LONG references_ = 1;
    HWND hwnd_;
};

static FlyoutAccessible* g_accessible = nullptr;

static void TickFlyout(HWND hwnd) {
    // Repaint only when something changes: idle frames make DWM re-composite
    // the acrylic material for nothing.
    bool repaint = ScrimFading();

    TestState state = CopyState();
    UINT mouseButtonsDown = MouseButtonsDown();
    UINT newlyPressedButtons = mouseButtonsDown & ~g_mouseButtonsDown;
    g_mouseButtonsDown = mouseButtonsDown;
    if (g_uiVisible && newlyPressedButtons && !StateRunning(state)) {
        POINT cursor = {};
        RECT flyout = {};
        if (GetCursorPos(&cursor) && GetWindowRect(hwnd, &flyout) &&
            !PtInRect(&flyout, cursor)) {
            HideFlyoutWindow(hwnd, false);
            return;
        }
    }

    static Phase lastPhase = Phase::Idle;
    if (state.phase != lastPhase) {
        NotifyWinEvent(EVENT_OBJECT_VALUECHANGE, hwnd, OBJID_CLIENT, CHILDID_SELF);
        lastPhase = state.phase;
    }
    if (SyncPhase(state)) {
        repaint = true;
    }

    if (g_testThread && !StateRunning(state)) {
        if (WaitForSingleObject(g_testThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_testThread);
            g_testThread = nullptr;
        }
    }

    View previousView = g_view;
    bool previousAnimating = g_animating;
    if (state.phase == Phase::Done && !g_historySaved) {
        // Saved before the transition starts, so the new row is already first.
        RecordHistory(state);
        g_historySaved = true;
        BeginView(View::History);
    } else if (state.phase == Phase::Error && g_view != View::Error) {
        BeginView(View::Error);
        g_hot = Hot::None;
    } else if (state.phase == Phase::Canceled && g_view == View::History) {
        BeginView(View::Gauge);
    }

    float previousHeight = g_layoutHeight;
    if (g_animating) {
        float t = AnimT();
        float eased = t * t * (3.0f - 2.0f * t);
        float targetHeight = ViewHeight(g_view);
        g_layoutHeight = g_heightFrom + (targetHeight - g_heightFrom) * eased;
    } else {
        g_layoutHeight = ViewHeight(g_view);
    }
    if (std::fabs(g_layoutHeight - previousHeight) > 0.25f) {
        PositionFlyout(hwnd, MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
        repaint = true;
    }

    if (g_view != previousView || g_animating != previousAnimating) {
        repaint = true;
    }

    // The springs and timelines advance per painted frame (WM_PAINT keeps
    // painting while AnyAnimationRunning); this tick only wakes it up.
    if (AnyAnimationRunning()) {
        repaint = true;
    }

    if (g_animating && AnimT() >= 1.0f) {
        g_animating = false;
        repaint = true;
    }

    if (StateRunning(state) || g_animating) {
        repaint = true;
    }
    if (g_copyUntil) {
        if (Now() >= g_copyUntil) {
            g_copyUntil = 0;
        }
        repaint = true;
    }

    if (repaint) {
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

static LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_GETOBJECT:
            if ((LONG)lParam == OBJID_CLIENT) {
                if (!g_accessible) {
                    g_accessible = new (std::nothrow) FlyoutAccessible(hwnd);
                }
                return g_accessible
                           ? LresultFromObject(IID_IAccessible, wParam, g_accessible)
                           : 0;
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            BeginPaint(hwnd, &paint);
            if (EnsureRenderTarget()) {
                TestState state = CopyState();
                AdvanceAnimations(state);
                g_renderTarget->BeginDraw();
                DrawFrame(state);
                HRESULT result = g_renderTarget->EndDraw();
                if (result == D2DERR_RECREATE_TARGET) {
                    ReleaseTargets();
                } else if (FAILED(result)) {
                    static int endDrawFailures = 0;
                    if (endDrawFailures++ < 3) {
                        Wh_Log(L"EndDraw failed 0x%08X", (unsigned)result);
                    }
                    ReleaseTargets();
                } else if (g_composition && g_swapChain) {
                    HRESULT present = g_swapChain->Present(0, 0);
                    if (FAILED(present)) {
                        Wh_Log(L"Present failed 0x%08X", (unsigned)present);
                        ReleaseTargets();
                    }
                }
                // While something moves, run the next frame in step with DWM
                // composition; when everything has settled, stop: an idle
                // flyout draws nothing. The next frame goes through a posted
                // message (TickFlyout, which repaints) rather than a bare
                // InvalidateRect: WM_PAINT outranks WM_TIMER, so constant
                // repaints would starve the timer and with it TickFlyout.
                if (g_renderTarget && g_uiVisible && AnyAnimationRunning()) {
                    DwmFlush();
                    PostMessageW(hwnd, WM_APP + 5, 0, 0);
                }
            }
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_SIZE:
            if (g_renderTarget) {
                ResizeRenderTarget();
            }
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                TickFlyout(hwnd);
            } else if (wParam == 2) {
                KillTimer(hwnd, 2);
                if (!g_uiVisible) {
                    ReleaseTargets();
                }
            }
            return 0;

        case WM_MOUSEMOVE: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            int row = -1;
            Hot hot = HitTest(x, y, &row);
            bool focusChanged = g_focus != Hot::None;
            if (hot != g_hot || row != g_hotRow || focusChanged) {
                g_hot = hot;
                g_hotRow = row;
                g_focus = Hot::None;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hot != Hot::None) {
                g_hot = Hot::None;
                g_hotRow = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_MOUSEWHEEL:
            if (g_view == View::History) {
                SetScrollTarget(g_scrollTarget -
                                GET_WHEEL_DELTA_WPARAM(wParam) / (double)WHEEL_DELTA * kRowStride);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT point = {};
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                Hot hot = HitTest((float)point.x / g_scale, (float)point.y / g_scale);
                if (hot != Hot::None) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            g_pressed = HitTest(x, y, &g_pressedRow);
            if (g_pressed != Hot::None) {
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            int row = -1;
            Hot hot = HitTest(x, y, &row);
            Hot pressed = g_pressed;
            int pressedRow = g_pressedRow;
            g_pressed = Hot::None;
            g_pressedRow = -1;
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            if (hot != Hot::None && hot == pressed && row == pressedRow) {
                if (hot == Hot::Row) {
                    g_focusRow = row;
                }
                ActivateHot(hwnd, hot);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_TAB) {
                MoveFocus((GetKeyState(VK_SHIFT) & 0x8000) ? -1 : 1);
                if (g_focus == Hot::Row) {
                    ScrollRowIntoView(g_focusRow);
                }
                return 0;
            }
            if (g_view == View::History && !g_history.empty()) {
                int last = (int)g_history.size() - 1;
                double page = kListBottom - kListTop;
                bool handled = true;
                switch (wParam) {
                    case VK_UP:
                    case VK_DOWN: {
                        int step = wParam == VK_UP ? -1 : 1;
                        if (g_focus == Hot::Row) {
                            g_focusRow = std::clamp(g_focusRow + step, 0, last);
                            ScrollRowIntoView(g_focusRow);
                            NotifyWinEvent(EVENT_OBJECT_FOCUS, hwnd, OBJID_CLIENT,
                                           AccChildId(Hot::Row));
                        } else {
                            SetScrollTarget(g_scrollTarget + step * kRowStride);
                        }
                        break;
                    }
                    case VK_PRIOR:
                        SetScrollTarget(g_scrollTarget - page);
                        break;
                    case VK_NEXT:
                        SetScrollTarget(g_scrollTarget + page);
                        break;
                    case VK_HOME:
                        SetScrollTarget(0);
                        break;
                    case VK_END:
                        SetScrollTarget(HistoryMaxScroll());
                        break;
                    default:
                        handled = false;
                        break;
                }
                if (handled) {
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
            if ((wParam == VK_RETURN || wParam == VK_SPACE) &&
                g_focus != Hot::None) {
                ActivateHot(hwnd, g_focus);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                if (TestRunning()) {
                    RequestTestCancel();
                } else {
                    HideFlyoutWindow(hwnd);
                }
                return 0;
            }
            break;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_uiVisible &&
                !TestRunning()) {
                HideFlyoutWindow(hwnd, false);
            }
            break;

        case WM_DPICHANGED: {
            UINT dpi = HIWORD(wParam);
            if (dpi == 0) {
                dpi = LOWORD(wParam);
            }
            if (dpi) {
                g_dpi = (float)dpi;
                g_scale = g_dpi / 96.0f;
            }
            PositionFlyout(hwnd, MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_SETTINGCHANGE:
            if (wParam == 0 && lParam &&
                _wcsicmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                RefreshBackdrop(hwnd);
            }
            return 0;

        case WM_THEMECHANGED:
        case WM_DWMCOMPOSITIONCHANGED:
            RefreshBackdrop(hwnd);
            return 0;

        case WM_CLOSE:
            if (TestRunning()) {
                RequestTestCancel();
            }
            HideFlyoutWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_accessible) {
                // Disconnect clients (Narrator, Inspect) so a late call after
                // the DLL unloads cannot crash the host.
                CoDisconnectObject(g_accessible, 0);
                g_accessible->Release();
                g_accessible = nullptr;
            }
            g_flyoutHwnd = nullptr;
            PostQuitMessage(0);
            return 0;

        default:
            if (message == WM_APP + 2) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (message == g_showMessage) {
                ShowFlyoutWindow(hwnd);
                return 0;
            }
            if (message == WM_APP + 1 || message == WM_APP + 5) {
                TickFlyout(hwnd);
                return 0;
            }
            if (message == WM_APP + 3) {
                Hot hot = HotFromAccChild((int)wParam);
                if (IsAvailableHot(hot)) {
                    ActivateHot(hwnd, hot);
                }
                return 0;
            }
            break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// ---------------------------------------------------------------------------
// UI thread and module lifecycle
// ---------------------------------------------------------------------------

// The mod's own image: the window class belongs to the mod, not the host.
extern "C" IMAGE_DOS_HEADER __ImageBase;

static DWORD WINAPI UiThreadProc(LPVOID) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"COM initialization failed (error 0x%08X)", (unsigned)comResult);
    }

    RefreshAppearance();

    WNDCLASSEXW windowClass = {sizeof(windowClass)};
    windowClass.style = 0;
    windowClass.lpfnWndProc = FlyoutWndProc;
    windowClass.hInstance = (HINSTANCE)&__ImageBase;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = nullptr;
    windowClass.lpszClassName = kFlyoutClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed (error %u)", GetLastError());
        if (comInitialized) {
            CoUninitialize();
        }
        return 1;
    }

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                    WS_EX_NOREDIRECTIONBITMAP,
                                kFlyoutClass, L"Speedtest", WS_POPUP, 0, 0,
                                (int)std::lround(kWidth), (int)std::lround(kGaugeHeight),
                                nullptr, nullptr, windowClass.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed (error %u)", GetLastError());
        UnregisterClassW(kFlyoutClass, windowClass.hInstance);
        if (comInitialized) {
            CoUninitialize();
        }
        return 1;
    }
    g_flyoutHwnd = hwnd;
    LoadHistory();

    int corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner,
                          sizeof(corner));

    // Desktop Acrylic covers the whole popup only with the frame extended.
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    ApplyBackdrop(hwnd, false);

    D2D1_FACTORY_OPTIONS options = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory), &options,
                                 (void**)&g_d2dFactory))) {
        g_d2dFactory = nullptr;
    }
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                  __uuidof(IDWriteFactory),
                                  (IUnknown**)&g_dwriteFactory))) {
        g_dwriteFactory = nullptr;
    }

    D2D1_STROKE_STYLE_PROPERTIES strokeProperties = {};
    strokeProperties.startCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.endCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.dashCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
    if (g_d2dFactory) {
        g_d2dFactory->CreateStrokeStyle(strokeProperties, nullptr, 0, &g_roundStroke);
    }

    Wh_Log(L"Flyout window ready");

    // An unload can arrive while this thread is starting: then WM_QUIT may
    // never reach the loop, so check the flag first.
    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        DestroyWindow(hwnd);
    } else {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }
    g_cancelRequested.store(1, std::memory_order_relaxed);
    ReleaseGraphics();
    g_flyoutHwnd = nullptr;
    UnregisterClassW(kFlyoutClass, windowClass.hInstance);
    if (comInitialized) {
        CoUninitialize();
    }
    return 0;
}

static bool IsExplorerPath(PCWSTR path) {
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

static bool ProcessIsExplorer(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    wchar_t path[MAX_PATH * 2] = {};
    DWORD size = ARRAYSIZE(path);
    bool explorer = QueryFullProcessImageNameW(process, 0, path, &size) && IsExplorerPath(path);
    CloseHandle(process);
    return explorer;
}

struct TrayScan {
    DWORD self;
    bool mine;
    bool otherExplorer;
};

static BOOL CALLBACK ScanTrayWindows(HWND hwnd, LPARAM param) {
    auto* scan = (TrayScan*)param;
    wchar_t className[32] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"Shell_TrayWnd") != 0) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == scan->self) {
        scan->mine = true;
        return FALSE;
    }
    if (ProcessIsExplorer(pid)) {
        scan->otherExplorer = true;
    }
    return TRUE;
}

static bool IsPrimaryShellProcess() {
    // Only the explorer.exe that owns the taskbar hosts the flyout. Some
    // customization tools create a fake Shell_TrayWnd in their own process,
    // so every taskbar window is checked instead of the first FindWindow hit.
    TrayScan scan = {GetCurrentProcessId(), false, false};
    EnumWindows(ScanTrayWindows, (LPARAM)&scan);
    if (scan.mine) {
        return true;
    }
    if (scan.otherExplorer) {
        return false;
    }
    // No real taskbar yet (early injection): the first explorer.exe hosts it.
    wchar_t path[MAX_PATH * 2] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return false;
    }
    return IsExplorerPath(path);
}

static HANDLE CreateKillOnCloseJob() {
    HANDLE job = CreateJobObjectW(nullptr, nullptr);
    if (!job) {
        Wh_Log(L"CreateJobObject failed (error %u)", GetLastError());
        return nullptr;
    }
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits,
                                 sizeof(limits))) {
        Wh_Log(L"SetInformationJobObject failed (error %u)", GetLastError());
        CloseHandle(job);
        return nullptr;
    }
    return job;
}

BOOL Wh_ModInit(void) {
    SelectLanguage();
    g_userSid = CurrentUserSid();
    if (g_userSid.empty()) {
        Wh_Log(L"User SID unavailable; history and consent use shared names");
    }
    LoadSettings();
    InterlockedExchange(&g_shutdown, 0);

    g_showMessage = RegisterWindowMessageW(L"SpeedTestFlyoutOokla.Show");
    if (!g_showMessage) {
        Wh_Log(L"RegisterWindowMessage failed (error %u)", GetLastError());
    }

    if (IsPrimaryShellProcess() && g_showMessage) {
        g_job = CreateKillOnCloseJob();
        g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0,
                                  &g_uiThreadId);
        if (!g_uiThread) {
            Wh_Log(L"UI thread creation failed (error %u)", GetLastError());
        }
    }

    // Import-form hooks: referencing ShellExecuteExW/ShellExecuteW makes the
    // mod import shell32, so the hooks cannot miss a late shell32 load.
    if (!WindhawkUtils::SetFunctionHook(ShellExecuteExW, ShellExecuteExW_Hook,
                                        &ShellExecuteExW_Original)) {
        Wh_Log(L"Unable to hook ShellExecuteExW");
    }
    if (!WindhawkUtils::SetFunctionHook(ShellExecuteW, ShellExecuteW_Hook,
                                        &ShellExecuteW_Original)) {
        Wh_Log(L"Unable to hook ShellExecuteW");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE kernel = kernelBase ? kernelBase : kernel32;
    auto createProcessW = kernel
                              ? (CreateProcessW_t)GetProcAddress(kernel, "CreateProcessW")
                              : nullptr;
    if (createProcessW &&
        !WindhawkUtils::SetFunctionHook(createProcessW, CreateProcessW_Hook,
                                        &CreateProcessW_Original)) {
        Wh_Log(L"Unable to hook CreateProcessW");
    }

    Wh_Log(L"Ookla speed test loaded (pid %u)", GetCurrentProcessId());
    return TRUE;
}

void Wh_ModUninit(void) {
    g_cancelRequested.store(1, std::memory_order_relaxed);
    InterlockedExchange(&g_shutdown, 1);
    // Unblocks the test thread's pipe read right away.
    if (g_job) {
        TerminateJobObject(g_job, 1);
    }

    if (g_flyoutHwnd && !PostMessageW(g_flyoutHwnd, WM_APP + 2, 0, 0)) {
        Wh_Log(L"Posting flyout shutdown message failed (error %u)",
               GetLastError());
    }

    if (g_uiThread && g_uiThreadId) {
        // The thread queue may not exist yet; retry until it is ready or the
        // startup path observes g_shutdown and exits.
        while (WaitForSingleObject(g_uiThread, 0) == WAIT_TIMEOUT) {
            if (PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0)) {
                break;
            }
            Sleep(10);
        }
        // Never unload code while either thread can still be executing it.
        DWORD result = WaitForSingleObject(g_uiThread, INFINITE);
        if (result != WAIT_OBJECT_0) {
            Wh_Log(L"Waiting for UI thread failed (error %u)", GetLastError());
            return;
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        g_uiThreadId = 0;
    }

    // A test the UI thread started while unload was beginning may have
    // launched after the first kill: kill again before the infinite wait.
    g_cancelRequested.store(1, std::memory_order_relaxed);
    KillSpeedtest();

    if (g_testThread) {
        DWORD result = WaitForSingleObject(g_testThread, INFINITE);
        if (result != WAIT_OBJECT_0) {
            Wh_Log(L"Waiting for test thread failed (error %u)", GetLastError());
            return;
        }
        CloseHandle(g_testThread);
        g_testThread = nullptr;
    }

    if (g_job) {
        CloseHandle(g_job);
        g_job = nullptr;
    }
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}
