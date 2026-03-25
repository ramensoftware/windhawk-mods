// ==WindhawkMod==
// @id              auto-time-sync-on-startup
// @name            Auto Time Sync On Startup
// @description     Requests a Windows time synchronization when Explorer starts after sign-in.
// @version         1.0
// @author          communism420
// @github          https://github.com/communism420
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -ladvapi32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Time Sync On Startup

This mod asks Windows to synchronize the clock once after sign-in. It runs the
actual sync logic in a separate `explorer.exe` host process instead of the main
shell process. It doesn't set a custom NTP server or change the time zone. It
simply triggers the standard Windows time sync flow, so the current system date
and time settings continue to apply.

The sync request is attempted in the background with an optional delay and
retries, which helps when the network or Windows Time service isn't ready
immediately after startup.

## Notes

- Windhawk mods are user-mode mods, so this runs after sign-in, not before the
  logon screen.
- The mod is intended for 64-bit Windows.
- The primary method is `SystemSettingsAdminFlows.exe ForceTimeSync 0`, which
  matches the Windows Settings "Sync now" flow.
- If enabled and the mod is already running elevated, it can fall back to
  `w32tm /resync /nowait`.
- After a successful synchronization, the mod shows a notification.
- Windows can still show a UAC prompt for the sync request because time sync is
  a privileged operation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- initialDelaySeconds: 30
  $name: Initial delay in seconds
  $description: Wait this many seconds after Explorer starts before the first synchronization attempt.

- retryCount: 4
  $name: Additional retries
  $description: How many extra attempts to make if the first synchronization request fails.

- retryIntervalSeconds: 30
  $name: Retry interval in seconds
  $description: Wait this many seconds between retry attempts.

- useScheduledTaskFallback: true
  $name: Use elevated w32tm fallback
  $description: If the Windows sync broker isn't available, try w32tm only when the dedicated host process is already elevated.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <oleauto.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <string>
#include <vector>

#include <windhawk_api.h>

struct Settings {
    int initialDelaySeconds;
    int retryCount;
    int retryIntervalSeconds;
    bool useScheduledTaskFallback;
};

namespace {

constexpr DWORD kCommandTimeoutMs = 15000;
constexpr DWORD kSleepPollMs = 200;
constexpr DWORD kNotificationLifetimeMs = 5000;
constexpr DWORD kSyncConfirmationTimeoutMs = 45000;
constexpr DWORD kSyncConfirmationPollMs = 2000;
constexpr PCWSTR kAdminBrokerArguments = L"ForceTimeSync 0";
constexpr PCWSTR kRunGuardKey = L"Software";
constexpr PCWSTR kRunGuardValueName =
    L"Windhawk\\" WH_MOD_ID L"_SessionRunGuard_Claimed";
constexpr UINT kNotificationIconId = 1;
constexpr PCWSTR kNotificationWindowClassName =
    L"AutoTimeSyncNotificationWindow_" WH_MOD_ID;
constexpr GUID kNotificationGuid = {
    0x0c69f59d, 0x59f7, 0x42dd, {0x85, 0xd6, 0xc7, 0x77, 0x64, 0x9c, 0x8a, 0x1f}
};

Settings g_settings{};
SRWLOCK g_settingsLock = SRWLOCK_INIT;
std::atomic<bool> g_stopWorker = false;
HANDLE g_workerThread = nullptr;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

enum class SyncAttemptResult {
    Success,
    RetryableFailure,
    PermanentFailure,
};

int ClampSetting(int value, int minValue, int maxValue) {
    return std::clamp(value, minValue, maxValue);
}

void LoadSettings() {
    Settings newSettings{};

    newSettings.initialDelaySeconds =
        ClampSetting(Wh_GetIntSetting(L"initialDelaySeconds"), 0, 3600);
    newSettings.retryCount =
        ClampSetting(Wh_GetIntSetting(L"retryCount"), 0, 20);
    newSettings.retryIntervalSeconds =
        ClampSetting(Wh_GetIntSetting(L"retryIntervalSeconds"), 1, 3600);
    newSettings.useScheduledTaskFallback =
        Wh_GetIntSetting(L"useScheduledTaskFallback") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = newSettings;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

bool SleepWithStopCheck(DWORD durationMs) {
    DWORD elapsedMs = 0;

    while (elapsedMs < durationMs) {
        if (g_stopWorker.load()) {
            return false;
        }

        DWORD remainingMs = durationMs - elapsedMs;
        DWORD sleepMs = std::min(remainingMs, kSleepPollMs);
        Sleep(sleepMs);
        elapsedMs += sleepMs;
    }

    return !g_stopWorker.load();
}

bool ClaimCurrentLogonRun() {
    HKEY key = nullptr;
    DWORD disposition = 0;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER,
                                  kRunGuardKey,
                                  0,
                                  nullptr,
                                  REG_OPTION_VOLATILE,
                                  KEY_QUERY_VALUE | KEY_SET_VALUE,
                                  nullptr,
                                  &key,
                                  &disposition);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"RegCreateKeyExW failed for the session guard (error %ld)",
               result);
        return true;
    }

    DWORD claimed = 0;
    DWORD type = 0;
    DWORD size = sizeof(claimed);
    result = RegQueryValueExW(key,
                              kRunGuardValueName,
                              nullptr,
                              &type,
                              reinterpret_cast<BYTE*>(&claimed),
                              &size);
    if (result == ERROR_SUCCESS && type == REG_DWORD && claimed != 0) {
        RegCloseKey(key);
        return false;
    }

    claimed = 1;
    result = RegSetValueExW(key,
                            kRunGuardValueName,
                            0,
                            REG_DWORD,
                            reinterpret_cast<const BYTE*>(&claimed),
                            sizeof(claimed));
    RegCloseKey(key);

    if (result != ERROR_SUCCESS) {
        Wh_Log(L"RegSetValueExW failed for the session guard (error %ld)",
               result);
    }

    return true;
}

template <size_t N>
void CopyToBuffer(WCHAR (&buffer)[N], const wchar_t* text) {
    if (!text) {
        buffer[0] = L'\0';
        return;
    }

    wcsncpy_s(buffer, text, _TRUNCATE);
}

LRESULT CALLBACK NotificationWindowProc(HWND hwnd,
                                        UINT msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HWND CreateNotificationWindowForCurrentThread() {
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = NotificationWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kNotificationWindowClassName;

    if (!RegisterClassExW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW failed for the notification window (error %lu)",
               GetLastError());
        return nullptr;
    }

    HWND notificationWindow = CreateWindowExW(0,
                                              kNotificationWindowClassName,
                                              kNotificationWindowClassName,
                                              WS_OVERLAPPED,
                                              0,
                                              0,
                                              0,
                                              0,
                                              nullptr,
                                              nullptr,
                                              instance,
                                              nullptr);
    if (!notificationWindow) {
        Wh_Log(L"CreateWindowExW failed for the notification window "
               L"(error %lu)",
               GetLastError());
        return nullptr;
    }

    return notificationWindow;
}

bool WaitWithMessageLoop(DWORD durationMs) {
    DWORD elapsedMs = 0;

    while (elapsedMs < durationMs) {
        if (g_stopWorker.load()) {
            return false;
        }

        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        DWORD remainingMs = durationMs - elapsedMs;
        DWORD sleepMs = std::min<DWORD>(50, remainingMs);
        Sleep(sleepMs);
        elapsedMs += sleepMs;
    }

    return !g_stopWorker.load();
}

std::wstring TrimString(const std::wstring& text) {
    size_t start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return {};
    }

    size_t end = text.find_last_not_of(L" \t\r\n");
    return text.substr(start, end - start + 1);
}

ULONGLONG FileTimeToUInt64(const FILETIME& fileTime) {
    ULARGE_INTEGER value{};
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart;
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

std::wstring GetNativeSystem32ExecutablePath(PCWSTR executableName) {
    WCHAR windowsDirectory[MAX_PATH];
    UINT length =
        GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
    if (length == 0 || length >= ARRAYSIZE(windowsDirectory)) {
        return {};
    }

    std::wstring path(windowsDirectory);
    path += L"\\System32\\";
    path += executableName;
    return path;
}

void ShowSuccessNotification() {
    HWND hwnd = CreateNotificationWindowForCurrentThread();
    if (!hwnd) {
        Wh_Log(L"Couldn't create a notification window");
        return;
    }

    NOTIFYICONDATAW notifyIcon{};
    notifyIcon.cbSize = sizeof(notifyIcon);
    notifyIcon.hWnd = hwnd;
    notifyIcon.uID = kNotificationIconId;
    notifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_STATE;
    notifyIcon.uCallbackMessage = WM_APP + 1;
    notifyIcon.hIcon = LoadIconW(nullptr, IDI_INFORMATION);
    notifyIcon.guidItem = kNotificationGuid;
    notifyIcon.dwState = NIS_HIDDEN;
    notifyIcon.dwStateMask = NIS_HIDDEN;
    CopyToBuffer(notifyIcon.szTip, L"Auto Time Sync On Startup");

    if (!Shell_NotifyIconW(NIM_ADD, &notifyIcon)) {
        Wh_Log(L"Shell_NotifyIconW(NIM_ADD) failed");
        DestroyWindow(hwnd);
        return;
    }

    notifyIcon.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &notifyIcon);

    notifyIcon.uFlags = NIF_INFO | NIF_GUID;
    notifyIcon.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
    CopyToBuffer(notifyIcon.szInfoTitle, L"Time synchronized");
    CopyToBuffer(notifyIcon.szInfo,
                 L"The Windows time synchronization request completed "
                 L"successfully.");

    if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
        Wh_Log(L"Shell_NotifyIconW(NIM_MODIFY) failed");
        Shell_NotifyIconW(NIM_DELETE, &notifyIcon);
        DestroyWindow(hwnd);
        return;
    }

    WaitWithMessageLoop(kNotificationLifetimeMs);
    Shell_NotifyIconW(NIM_DELETE, &notifyIcon);
    DestroyWindow(hwnd);
}

bool IsCurrentProcessElevated() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"OpenProcessToken failed (error %lu)", GetLastError());
        return false;
    }

    TOKEN_ELEVATION elevation{};
    DWORD returnedLength = 0;
    BOOL succeeded = GetTokenInformation(token,
                                         TokenElevation,
                                         &elevation,
                                         sizeof(elevation),
                                         &returnedLength);
    CloseHandle(token);

    if (!succeeded) {
        Wh_Log(L"GetTokenInformation(TokenElevation) failed (error %lu)",
               GetLastError());
        return false;
    }

    return elevation.TokenIsElevated != 0;
}

std::wstring GetSystemExecutablePath(PCWSTR executableName) {
    WCHAR windowsDirectory[MAX_PATH];
    UINT length =
        GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
    if (length == 0 || length >= ARRAYSIZE(windowsDirectory)) {
        return {};
    }

    std::wstring windowsDirectoryPath(windowsDirectory);

    std::wstring system32Path = windowsDirectoryPath + L"\\System32\\";
    system32Path += executableName;
    if (FileExists(system32Path)) {
        return system32Path;
    }

    WCHAR systemDirectory[MAX_PATH];
    length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (length == 0 || length >= ARRAYSIZE(systemDirectory)) {
        return system32Path;
    }

    std::wstring redirectedPath(systemDirectory);
    redirectedPath += L"\\";
    redirectedPath += executableName;
    return redirectedPath;
}

bool RunHiddenProcess(const std::wstring& executablePath,
                      const std::wstring& arguments,
                      DWORD timeoutMs,
                      DWORD* exitCode) {
    if (executablePath.empty()) {
        return false;
    }

    std::wstring commandLine = L"\"" + executablePath + L"\"";
    if (!arguments.empty()) {
        commandLine += L" ";
        commandLine += arguments;
    }

    std::vector<WCHAR> mutableCommandLine(commandLine.begin(),
                                          commandLine.end());
    mutableCommandLine.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(nullptr,
                        mutableCommandLine.data(),
                        nullptr,
                        nullptr,
                        FALSE,
                        CREATE_NO_WINDOW,
                        nullptr,
                        nullptr,
                        &startupInfo,
                        &processInfo)) {
        Wh_Log(L"CreateProcessW failed for %s (error %lu)",
               executablePath.c_str(),
               GetLastError());
        return false;
    }

    DWORD waitResult = WaitForSingleObject(processInfo.hProcess, timeoutMs);
    bool succeeded = false;
    DWORD localExitCode = static_cast<DWORD>(-1);

    if (waitResult == WAIT_OBJECT_0) {
        if (GetExitCodeProcess(processInfo.hProcess, &localExitCode)) {
            succeeded = (localExitCode == 0);
        } else {
            Wh_Log(L"GetExitCodeProcess failed (error %lu)", GetLastError());
        }
    } else if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Timed out waiting for %s", executablePath.c_str());
    } else {
        Wh_Log(L"WaitForSingleObject failed for %s (error %lu)",
               executablePath.c_str(),
               GetLastError());
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    if (exitCode) {
        *exitCode = localExitCode;
    }

    return succeeded;
}

bool RunHiddenProcessCaptureOutput(const std::wstring& executablePath,
                                   const std::wstring& arguments,
                                   DWORD timeoutMs,
                                   std::wstring* output,
                                   DWORD* exitCode) {
    if (output) {
        output->clear();
    }

    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;

    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    if (!CreatePipe(&readPipe, &writePipe, &securityAttributes, 0)) {
        Wh_Log(L"CreatePipe failed (error %lu)", GetLastError());
        return false;
    }

    if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0)) {
        Wh_Log(L"SetHandleInformation failed (error %lu)", GetLastError());
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return false;
    }

    std::wstring commandLine = L"\"" + executablePath + L"\"";
    if (!arguments.empty()) {
        commandLine += L" ";
        commandLine += arguments;
    }

    std::vector<WCHAR> mutableCommandLine(commandLine.begin(),
                                          commandLine.end());
    mutableCommandLine.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    startupInfo.wShowWindow = SW_HIDE;
    startupInfo.hStdOutput = writePipe;
    startupInfo.hStdError = writePipe;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(nullptr,
                        mutableCommandLine.data(),
                        nullptr,
                        nullptr,
                        TRUE,
                        CREATE_NO_WINDOW,
                        nullptr,
                        nullptr,
                        &startupInfo,
                        &processInfo)) {
        Wh_Log(L"CreateProcessW failed for %s (error %lu)",
               executablePath.c_str(),
               GetLastError());
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return false;
    }

    CloseHandle(writePipe);

    DWORD waitResult = WaitForSingleObject(processInfo.hProcess, timeoutMs);
    DWORD localExitCode = static_cast<DWORD>(-1);
    bool processSucceeded = false;

    if (waitResult == WAIT_OBJECT_0) {
        if (GetExitCodeProcess(processInfo.hProcess, &localExitCode)) {
            processSucceeded = (localExitCode == 0);
        } else {
            Wh_Log(L"GetExitCodeProcess failed (error %lu)", GetLastError());
        }
    } else if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Timed out waiting for %s", executablePath.c_str());
        TerminateProcess(processInfo.hProcess, WAIT_TIMEOUT);
    } else {
        Wh_Log(L"WaitForSingleObject failed for %s (error %lu)",
               executablePath.c_str(),
               GetLastError());
    }

    std::string outputBytes;
    char buffer[4096];
    DWORD bytesRead = 0;
    while (ReadFile(readPipe, buffer, sizeof(buffer), &bytesRead, nullptr) &&
           bytesRead > 0) {
        outputBytes.append(buffer, bytesRead);
    }

    if (output && !outputBytes.empty()) {
        int requiredChars = MultiByteToWideChar(CP_OEMCP,
                                                0,
                                                outputBytes.data(),
                                                static_cast<int>(outputBytes.size()),
                                                nullptr,
                                                0);
        if (requiredChars > 0) {
            output->resize(requiredChars);
            MultiByteToWideChar(CP_OEMCP,
                                0,
                                outputBytes.data(),
                                static_cast<int>(outputBytes.size()),
                                output->data(),
                                requiredChars);
        }
    }

    CloseHandle(readPipe);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    if (exitCode) {
        *exitCode = localExitCode;
    }

    return processSucceeded;
}

bool TryParseLocalizedDateTime(const std::wstring& text, FILETIME* fileTime) {
    if (!fileTime) {
        return false;
    }

    DATE variantDate = 0;
    HRESULT hr = VarDateFromStr(text.c_str(), LOCALE_USER_DEFAULT, 0, &variantDate);
    if (FAILED(hr)) {
        return false;
    }

    SYSTEMTIME systemTime{};
    if (!VariantTimeToSystemTime(variantDate, &systemTime)) {
        return false;
    }

    return SystemTimeToFileTime(&systemTime, fileTime) != FALSE;
}

std::optional<FILETIME> QueryLastSuccessfulSyncTime() {
    std::wstring output;
    DWORD exitCode = static_cast<DWORD>(-1);
    bool succeeded = RunHiddenProcessCaptureOutput(
        GetSystemExecutablePath(L"w32tm.exe"),
        L"/query /status",
        kCommandTimeoutMs,
        &output,
        &exitCode);

    if (!succeeded) {
        Wh_Log(L"w32tm /query /status returned exit code %lu", exitCode);
        return std::nullopt;
    }

    size_t lineStart = 0;
    while (lineStart < output.size()) {
        size_t lineEnd = output.find_first_of(L"\r\n", lineStart);
        if (lineEnd == std::wstring::npos) {
            lineEnd = output.size();
        }

        std::wstring line = output.substr(lineStart, lineEnd - lineStart);
        size_t colon = line.find(L':');
        if (colon != std::wstring::npos) {
            std::wstring value = TrimString(line.substr(colon + 1));
            FILETIME parsedTime{};
            if (!value.empty() && TryParseLocalizedDateTime(value, &parsedTime)) {
                return parsedTime;
            }
        }

        lineStart = output.find_first_not_of(L"\r\n", lineEnd);
        if (lineStart == std::wstring::npos) {
            break;
        }
    }

    Wh_Log(L"Couldn't parse the last successful sync time from w32tm output");
    return std::nullopt;
}

bool WaitForConfirmedSync(const std::optional<FILETIME>& previousSyncTime,
                          const FILETIME& attemptStartTime) {
    ULONGLONG previousValue =
        previousSyncTime ? FileTimeToUInt64(*previousSyncTime) : 0;
    ULONGLONG attemptStartValue = FileTimeToUInt64(attemptStartTime);
    DWORD elapsedMs = 0;

    while (elapsedMs <= kSyncConfirmationTimeoutMs) {
        if (g_stopWorker.load()) {
            return false;
        }

        std::optional<FILETIME> currentSyncTime = QueryLastSuccessfulSyncTime();
        if (currentSyncTime) {
            ULONGLONG currentValue = FileTimeToUInt64(*currentSyncTime);
            if ((previousSyncTime && currentValue > previousValue) ||
                (!previousSyncTime && currentValue >= attemptStartValue)) {
                return true;
            }
        }

        if (elapsedMs == kSyncConfirmationTimeoutMs) {
            break;
        }

        DWORD sleepMs = std::min(kSyncConfirmationPollMs,
                                 kSyncConfirmationTimeoutMs - elapsedMs);
        if (!SleepWithStopCheck(sleepMs)) {
            return false;
        }
        elapsedMs += sleepMs;
    }

    return false;
}

SyncAttemptResult RequestSyncViaSettingsBroker() {
    std::wstring executablePath =
        GetNativeSystem32ExecutablePath(L"SystemSettingsAdminFlows.exe");
    if (executablePath.empty()) {
        Wh_Log(L"SystemSettingsAdminFlows.exe path couldn't be resolved");
        return SyncAttemptResult::RetryableFailure;
    }

    HINSTANCE result = ShellExecuteW(nullptr,
                                     L"open",
                                     executablePath.c_str(),
                                     kAdminBrokerArguments,
                                     nullptr,
                                     SW_HIDE);
    INT_PTR shellResult = reinterpret_cast<INT_PTR>(result);
    if (shellResult <= 32) {
        Wh_Log(L"ShellExecuteW failed for SystemSettingsAdminFlows.exe "
               L"(path: %s, code: %Id)",
               executablePath.c_str(),
               shellResult);

        if (shellResult == SE_ERR_ACCESSDENIED ||
            shellResult == ERROR_ACCESS_DENIED ||
            shellResult == ERROR_CANCELLED) {
            return SyncAttemptResult::PermanentFailure;
        }

        return SyncAttemptResult::RetryableFailure;
    }

    Wh_Log(L"ShellExecuteW launched SystemSettingsAdminFlows.exe "
           L"(path: %s)",
           executablePath.c_str());
    return SyncAttemptResult::Success;
}

SyncAttemptResult RequestSyncViaW32tm() {
    DWORD exitCode = static_cast<DWORD>(-1);
    bool succeeded = RunHiddenProcess(GetSystemExecutablePath(L"w32tm.exe"),
                                      L"/resync /nowait",
                                      kCommandTimeoutMs,
                                      &exitCode);

    Wh_Log(L"w32tm /resync /nowait returned exit code %lu", exitCode);
    if (succeeded) {
        return SyncAttemptResult::Success;
    }

    if (exitCode == ERROR_ACCESS_DENIED ||
        exitCode == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)) {
        return SyncAttemptResult::PermanentFailure;
    }

    return SyncAttemptResult::RetryableFailure;
}

SyncAttemptResult RequestTimeSynchronization(const Settings& settings) {
    SyncAttemptResult brokerResult = RequestSyncViaSettingsBroker();
    if (brokerResult == SyncAttemptResult::Success ||
        brokerResult == SyncAttemptResult::PermanentFailure) {
        return brokerResult;
    }

    if (settings.useScheduledTaskFallback && IsCurrentProcessElevated()) {
        return RequestSyncViaW32tm();
    }

    if (!IsCurrentProcessElevated()) {
        Wh_Log(L"Explorer process isn't elevated, skipping w32tm fallback");
        return SyncAttemptResult::PermanentFailure;
    }

    return SyncAttemptResult::RetryableFailure;
}

DWORD WINAPI WorkerThreadProc(void*) {
    if (!ClaimCurrentLogonRun()) {
        Wh_Log(L"Skipping time sync because it already ran in this logon "
               L"session");
        ExitProcess(0);
    }

    Settings settings = GetSettingsSnapshot();

    if (settings.initialDelaySeconds > 0) {
        DWORD delayMs =
            static_cast<DWORD>(settings.initialDelaySeconds) * 1000;
        if (!SleepWithStopCheck(delayMs)) {
            ExitProcess(0);
        }
    }

    int totalAttempts = settings.retryCount + 1;
    for (int attempt = 1; attempt <= totalAttempts; ++attempt) {
        if (g_stopWorker.load()) {
            ExitProcess(0);
        }

        Wh_Log(L"Time sync attempt %d of %d", attempt, totalAttempts);
        std::optional<FILETIME> previousSyncTime = QueryLastSuccessfulSyncTime();
        FILETIME attemptStartTime{};
        GetSystemTimeAsFileTime(&attemptStartTime);

        SyncAttemptResult result = RequestTimeSynchronization(settings);
        if (result == SyncAttemptResult::Success) {
            if (WaitForConfirmedSync(previousSyncTime, attemptStartTime)) {
                Wh_Log(L"Time sync completed successfully on attempt %d",
                       attempt);
                ShowSuccessNotification();
                ExitProcess(0);
            }

            Wh_Log(L"Time sync request succeeded, but completion wasn't "
                   L"confirmed on attempt %d",
                   attempt);
        }

        if (result == SyncAttemptResult::PermanentFailure) {
            Wh_Log(L"Time sync failed permanently, stopping retries");
            ExitProcess(0);
        }

        if (attempt == totalAttempts) {
            break;
        }

        DWORD retryDelayMs =
            static_cast<DWORD>(settings.retryIntervalSeconds) * 1000;
        if (!SleepWithStopCheck(retryDelayMs)) {
            ExitProcess(0);
        }
    }

    Wh_Log(L"All time sync attempts failed");
    ExitProcess(0);
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopWorker = false;
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed (error %lu)", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    g_stopWorker = true;

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 20000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to
// other processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated explorer.exe process.
//
// Use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr,
                               currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileNameW failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW startupInfo{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION processInfo{};
    if (!pCreateProcessInternalW(nullptr,
                                 currentProcessPath,
                                 commandLine,
                                 nullptr,
                                 nullptr,
                                 FALSE,
                                 NORMAL_PRIORITY_CLASS,
                                 nullptr,
                                 nullptr,
                                 &startupInfo,
                                 &processInfo,
                                 nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
