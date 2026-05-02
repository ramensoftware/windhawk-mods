// ==WindhawkMod==
// @id              replace-windows-search-with-app
// @name            Windows Search Redirector
// @description     Redirects Windows Search to Command Palette, PowerToys Run, or a custom launcher and forwards initial text
// @version         1.0.0
// @author          Fefedu973
// @github          https://github.com/Fefedu973
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lshlwapi -lshell32 -loleaut32 -lole32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Search Redirector

Replaces Windows Search entry points with PowerToys Command Palette or another
configured launcher, and forwards the text that was already typed or pasted
while the replacement launcher is opening.

Main interception layers:

- `explorer.exe`: low-level keyboard/mouse hooks, Search protocol fallback,
  `SearchUx.UI.dll` and `twinui.pcshell.dll` hooks when those modules are loaded.
- `StartMenuExperienceHost.exe`: `StartDocked.dll` hooks for Start menu search
  transitions and Start search box pointer/tap paths.
- `SearchHost.exe`: `SearchUx.UI.dll` hooks for taskbar/SearchHost paths.

See the repository README for the full hook matrix, transaction diagrams,
settings reference and testing commands.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- launcher: commandPalette
  $name: Replacement launcher
  $description: >-
    Target to open instead of Windows Search. Command Palette uses the
    x-cmdpal: URI first. The custom hotkey fallback is only used when this
    setting is Custom hotkey, PowerToys Run, or when Custom hotkey is filled.
  $options:
    - commandPalette: PowerToys Command Palette
    - powerToysRun: PowerToys Run
    - customHotkey: Custom hotkey
    - customCommand: Custom command or URI
- requireLauncherAvailable: true
  $name: Require launcher process to be running
  $description: >-
    If enabled, Windows Search is left untouched when the configured launcher
    process is not running. For Command Palette this requires
    Microsoft.CmdPal.UI.exe to already exist.
- customProcessName: ""
  $name: Custom launcher process name
  $description: >-
    Optional process name used for availability and foreground detection, for
    example Everything.exe. Multiple names can be separated by comma or
    semicolon. Leave empty for the built-in launchers.
- customHotkey: ""
  $name: Custom launcher hotkey
  $description: >-
    Optional hotkey such as win+alt+space, ctrl+space, or alt+space. For
    Command Palette, leave empty to avoid falling back to the PowerToys hotkey.
- customCommand: ""
  $name: Custom command or URI
  $description: >-
    Command, executable path, or URI to open when Replacement launcher is set
    to Custom command or URI. Example: x-cmdpal: or C:\Path\App.exe.
- customCommandArgs: ""
  $name: Custom command arguments
  $description: Arguments passed to the custom executable, ignored for URI use.
- textCaptureDelayMs: 180
  $name: Text injection delay in milliseconds
  $description: >-
    Delay after the replacement launcher reaches the foreground before buffered
    text is pasted or injected.
- debounceMs: 300
  $name: Launch debounce in milliseconds
  $description: Minimum delay between independent launcher activation attempts.
- transitionCaptureMs: 3500
  $name: Transition capture window in milliseconds
  $description: >-
    While the launcher is opening, keystrokes are buffered and swallowed for up
    to this duration so fast typing doesn't leak into Windows Search.
- transitionIdleMs: 80
  $name: Transition idle threshold in milliseconds
  $description: >-
    Once no more input was captured for this duration and the launcher is
    foreground, the buffered text transaction is considered complete.
- redirectWinS: true
  $name: Redirect Win+S
  $description: Open the replacement launcher when Win+S is pressed.
- redirectStartMenuTyping: true
  $name: Redirect typing in Start menu
  $description: >-
    Redirect direct typing, paste and backspace while StartMenuExperienceHost.exe
    is foreground. Transition capture after another enabled trigger still works.
- redirectSearchHostTyping: true
  $name: Redirect typing in Windows Search
  $description: >-
    Redirect direct typing, paste and backspace while SearchHost.exe is
    foreground. This is independent from taskbar Search redirection. Disable it
    if you only want Start menu typing redirection.
- redirectStartMenuSearchBoxClick: true
  $name: Redirect Start search box clicks
  $description: >-
    Redirect clicks/taps on the Start menu search box, including the UI
    Automation fallback.
- redirectStartMenuSearchTransitions: true
  $name: Redirect Start search transitions
  $description: >-
    Redirect private StartDocked focus/open-search requests that normally move
    the Start menu into Windows Search.
- redirectTaskbarSearch: true
  $name: Redirect taskbar Search
  $description: >-
    Redirect taskbar Search button and SearchHost activation paths. Broader
    fallback layers can still apply if enabled.
- redirectUndockedSearch: true
  $name: Redirect undocked Windows Search
  $description: >-
    Redirect twinui/undocked Windows Search activation paths when those private
    hooks are available.
- redirectSearchProtocol: true
  $name: Redirect search protocols
  $description: >-
    Redirect ms-search:, search-ms: and ms-searchassistant: launches.
- allowInjectedInput: true
  $name: Capture injected input
  $description: >-
    Allows synthetic keyboard input from external tools to be captured. Input
    generated by this mod is always ignored.
- log: false
  $name: Debug logging
  $description: Emit detailed [ReplaceSearch] logs to OutputDebugString.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <initguid.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <uiautomation.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cwctype>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// -------------------- Settings --------------------

enum class LauncherTarget {
    PowerToysRun = 0,
    CommandPalette = 1,
    CustomHotkey = 2,
    CustomCommand = 3,
};

struct SettingsSnapshot {
    DWORD debounceMs = 300;
    DWORD textCaptureDelayMs = 180;
    DWORD transitionCaptureMs = 3500;
    DWORD transitionIdleMs = 80;
    bool allowInjectedInput = true;
    bool requireLauncherAvailable = true;
    bool redirectWinS = true;
    bool redirectStartMenuTyping = true;
    bool redirectSearchHostTyping = true;
    bool redirectStartMenuSearchBoxClick = true;
    bool redirectStartMenuSearchTransitions = true;
    bool redirectTaskbarSearch = true;
    bool redirectUndockedSearch = true;
    bool redirectSearchProtocol = true;
    LauncherTarget launcherTarget = LauncherTarget::CommandPalette;
    std::wstring customHotkey;
    std::wstring customCommand;
    std::wstring customCommandArgs;
    std::wstring customProcessName;
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static SettingsSnapshot g_settings;
static volatile LONG g_logEnabled = 0;
static volatile LONG g_redirectWinSEnabled = 1;
static volatile LONG g_redirectStartMenuTypingEnabled = 1;
static volatile LONG g_redirectSearchHostTypingEnabled = 1;
static volatile LONG g_redirectStartMenuSearchBoxClickEnabled = 1;
static volatile LONG g_redirectStartMenuSearchTransitionsEnabled = 1;
static volatile LONG g_redirectTaskbarSearchEnabled = 1;
static volatile LONG g_redirectUndockedSearchEnabled = 1;
static volatile LONG g_redirectSearchProtocolEnabled = 1;
static volatile LONG64 g_targetAvailableCacheTick = 0;
static volatile LONG g_targetAvailableCacheValue = 0;
static volatile LONG64 g_ignoreInjectedUntilTick = 0;
static volatile LONG g_unloading = 0;
constexpr ULONG_PTR kOwnInjectedInputMarker = 0x575253484B494E50ULL;

static SettingsSnapshot GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    SettingsSnapshot snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

static bool IsLogEnabled() {
    return InterlockedCompareExchange(&g_logEnabled, 0, 0) != 0;
}

static bool IsUnloading() {
    return InterlockedCompareExchange(&g_unloading, 0, 0) != 0;
}

static void log_if(PCWSTR fmt, ...) {
    if (!IsLogEnabled()) {
        return;
    }

    wchar_t buffer[1024];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, fmt, args);
    va_end(args);
    Wh_Log(L"[ReplaceSearch] %s", buffer);
}

static DWORD ClampDwordSetting(int value,
                               DWORD minValue,
                               DWORD maxValue,
                               DWORD fallbackValue) {
    if (value < 0 || static_cast<DWORD>(value) < minValue) {
        return fallbackValue;
    }

    if (static_cast<DWORD>(value) > maxValue) {
        return maxValue;
    }

    return static_cast<DWORD>(value);
}

static void InvalidateReplacementTargetAvailabilityCache() {
    InterlockedExchange64(&g_targetAvailableCacheTick, 0);
}

static void LoadSettings() {
    auto readStringSetting = [](PCWSTR name) -> std::wstring {
        PCWSTR value = Wh_GetStringSetting(name);
        std::wstring result = value ? value : L"";
        if (value) {
            Wh_FreeStringSetting(value);
        }
        return result;
    };

    SettingsSnapshot next;

    std::wstring launcher = readStringSetting(L"launcher");
    std::transform(launcher.begin(), launcher.end(), launcher.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });

    if (launcher == L"powertoysrun" || launcher == L"0") {
        next.launcherTarget = LauncherTarget::PowerToysRun;
    } else if (launcher == L"commandpalette" || launcher == L"1") {
        next.launcherTarget = LauncherTarget::CommandPalette;
    } else if (launcher == L"customhotkey" || launcher == L"2") {
        next.launcherTarget = LauncherTarget::CustomHotkey;
    } else if (launcher == L"customcommand" || launcher == L"3") {
        next.launcherTarget = LauncherTarget::CustomCommand;
    } else {
        next.launcherTarget = LauncherTarget::CommandPalette;
    }

    next.textCaptureDelayMs = ClampDwordSetting(
        Wh_GetIntSetting(L"textCaptureDelayMs"), 0, 2000, 180);
    next.debounceMs =
        ClampDwordSetting(Wh_GetIntSetting(L"debounceMs"), 0, 5000, 300);
    next.transitionCaptureMs = ClampDwordSetting(
        Wh_GetIntSetting(L"transitionCaptureMs"), 50, 10000, 3500);
    next.transitionIdleMs = ClampDwordSetting(
        Wh_GetIntSetting(L"transitionIdleMs"), 0, 1000, 80);

    next.allowInjectedInput = Wh_GetIntSetting(L"allowInjectedInput") != 0;
    next.requireLauncherAvailable =
        Wh_GetIntSetting(L"requireLauncherAvailable") != 0;
    next.redirectWinS = Wh_GetIntSetting(L"redirectWinS") != 0;
    next.redirectStartMenuTyping =
        Wh_GetIntSetting(L"redirectStartMenuTyping") != 0;
    next.redirectSearchHostTyping =
        Wh_GetIntSetting(L"redirectSearchHostTyping") != 0;
    next.redirectStartMenuSearchBoxClick =
        Wh_GetIntSetting(L"redirectStartMenuSearchBoxClick") != 0;
    next.redirectStartMenuSearchTransitions =
        Wh_GetIntSetting(L"redirectStartMenuSearchTransitions") != 0;
    next.redirectTaskbarSearch =
        Wh_GetIntSetting(L"redirectTaskbarSearch") != 0;
    next.redirectUndockedSearch =
        Wh_GetIntSetting(L"redirectUndockedSearch") != 0;
    next.redirectSearchProtocol =
        Wh_GetIntSetting(L"redirectSearchProtocol") != 0;
    bool logEnabled = Wh_GetIntSetting(L"log") != 0;

    next.customHotkey = readStringSetting(L"customHotkey");
    next.customCommand = readStringSetting(L"customCommand");
    next.customCommandArgs = readStringSetting(L"customCommandArgs");
    next.customProcessName = readStringSetting(L"customProcessName");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(next);
    ReleaseSRWLockExclusive(&g_settingsLock);

    InterlockedExchange(&g_logEnabled, logEnabled ? 1 : 0);
    InterlockedExchange(&g_redirectWinSEnabled,
                        next.redirectWinS ? 1 : 0);
    InterlockedExchange(&g_redirectStartMenuTypingEnabled,
                        next.redirectStartMenuTyping ? 1 : 0);
    InterlockedExchange(&g_redirectSearchHostTypingEnabled,
                        next.redirectSearchHostTyping ? 1 : 0);
    InterlockedExchange(&g_redirectStartMenuSearchBoxClickEnabled,
                        next.redirectStartMenuSearchBoxClick ? 1 : 0);
    InterlockedExchange(&g_redirectStartMenuSearchTransitionsEnabled,
                        next.redirectStartMenuSearchTransitions ? 1 : 0);
    InterlockedExchange(&g_redirectTaskbarSearchEnabled,
                        next.redirectTaskbarSearch ? 1 : 0);
    InterlockedExchange(&g_redirectUndockedSearchEnabled,
                        next.redirectUndockedSearch ? 1 : 0);
    InterlockedExchange(&g_redirectSearchProtocolEnabled,
                        next.redirectSearchProtocol ? 1 : 0);
    InvalidateReplacementTargetAvailabilityCache();
}

// -------------------- Process / target detection --------------------

enum class TargetProcess {
    Explorer,
    StartMenu,
    SearchHost,
    Unknown,
};

static TargetProcess g_targetProcess = TargetProcess::Unknown;

static std::wstring GetCurrentProcessName() {
    wchar_t modulePath[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, modulePath, _countof(modulePath));
    if (length == 0 || length >= _countof(modulePath)) {
        return L"";
    }

    return PathFindFileNameW(modulePath);
}

static TargetProcess DetectTargetProcess() {
    std::wstring processName = GetCurrentProcessName();
    if (_wcsicmp(processName.c_str(), L"explorer.exe") == 0) {
        return TargetProcess::Explorer;
    }

    if (_wcsicmp(processName.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        return TargetProcess::StartMenu;
    }

    if (_wcsicmp(processName.c_str(), L"SearchHost.exe") == 0) {
        return TargetProcess::SearchHost;
    }

    return TargetProcess::Unknown;
}

// -------------------- Shared cross-process state --------------------

constexpr wchar_t kSharedStateName[] =
    L"Local\\Windhawk.ReplaceWindowsSearchWithApp.SharedState.v5";
constexpr wchar_t kSharedMutexName[] =
    L"Local\\Windhawk.ReplaceWindowsSearchWithApp.SharedState.Mutex.v5";
constexpr size_t kMaxPendingTextLength = 512;
constexpr ULONGLONG kPendingTextMaxAgeMs = 10000;

struct SharedState {
    volatile LONG initialized;
    volatile LONG launchInProgress;
    volatile LONG64 lastLaunchTick;
    volatile LONG64 pendingTextTick;
    volatile LONG64 captureUntilTick;
    volatile LONG64 lastInputTick;
    volatile LONG pendingTextCanPasteOriginal;
    wchar_t pendingText[kMaxPendingTextLength];
};

static HANDLE g_sharedMapping = nullptr;
static HANDLE g_sharedMutex = nullptr;
static SharedState* g_sharedState = nullptr;

class SharedStateLock {
   public:
    explicit SharedStateLock(DWORD timeoutMs = 50) {
        if (!g_sharedMutex) {
            return;
        }

        DWORD wait = WaitForSingleObject(g_sharedMutex, timeoutMs);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED) {
            m_locked = true;
        }
    }

    ~SharedStateLock() {
        if (m_locked && g_sharedMutex) {
            ReleaseMutex(g_sharedMutex);
        }
    }

    SharedStateLock(const SharedStateLock&) = delete;
    SharedStateLock& operator=(const SharedStateLock&) = delete;

    bool locked() const {
        return m_locked;
    }

   private:
    bool m_locked = false;
};

static bool InitSharedState() {
    g_sharedMutex = CreateMutexW(nullptr, FALSE, kSharedMutexName);
    if (!g_sharedMutex) {
        Wh_Log(L"[ReplaceSearch] CreateMutexW failed: %u", GetLastError());
        return false;
    }

    g_sharedMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                         0, sizeof(SharedState), kSharedStateName);
    if (!g_sharedMapping) {
        Wh_Log(L"[ReplaceSearch] CreateFileMappingW failed: %u", GetLastError());
        CloseHandle(g_sharedMutex);
        g_sharedMutex = nullptr;
        return false;
    }

    g_sharedState = reinterpret_cast<SharedState*>(MapViewOfFile(
        g_sharedMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState)));
    if (!g_sharedState) {
        Wh_Log(L"[ReplaceSearch] MapViewOfFile failed: %u", GetLastError());
        CloseHandle(g_sharedMapping);
        g_sharedMapping = nullptr;
        CloseHandle(g_sharedMutex);
        g_sharedMutex = nullptr;
        return false;
    }

    {
        SharedStateLock lock(2000);
        if (!lock.locked()) {
            Wh_Log(L"[ReplaceSearch] Failed to lock shared state during init");
            UnmapViewOfFile(g_sharedState);
            g_sharedState = nullptr;
            CloseHandle(g_sharedMapping);
            g_sharedMapping = nullptr;
            CloseHandle(g_sharedMutex);
            g_sharedMutex = nullptr;
            return false;
        }

        if (InterlockedCompareExchange(&g_sharedState->initialized, 0, 0) != 1) {
            ZeroMemory(g_sharedState, sizeof(*g_sharedState));
            InterlockedExchange(&g_sharedState->initialized, 1);
        }
    }

    return true;
}

static void UninitSharedState() {
    if (g_sharedState) {
        UnmapViewOfFile(g_sharedState);
        g_sharedState = nullptr;
    }

    if (g_sharedMapping) {
        CloseHandle(g_sharedMapping);
        g_sharedMapping = nullptr;
    }

    if (g_sharedMutex) {
        CloseHandle(g_sharedMutex);
        g_sharedMutex = nullptr;
    }
}

static void ClearPendingTextLocked() {
    if (!g_sharedState) {
        return;
    }

    g_sharedState->pendingText[0] = L'\0';
    InterlockedExchange(&g_sharedState->pendingTextCanPasteOriginal, 0);
    InterlockedExchange64(&g_sharedState->pendingTextTick, 0);
}

static void ClearPendingText() {
    if (!g_sharedState) {
        return;
    }

    SharedStateLock lock;
    if (!lock.locked()) {
        return;
    }

    ClearPendingTextLocked();
}

static ULONGLONG ReadTick(volatile LONG64* tick) {
    return static_cast<ULONGLONG>(InterlockedCompareExchange64(tick, 0, 0));
}

static void ExtendSharedTickTo(volatile LONG64* tick, ULONGLONG value) {
    LONG64 oldValue = InterlockedCompareExchange64(tick, 0, 0);
    while (static_cast<ULONGLONG>(oldValue) < value) {
        LONG64 previous = InterlockedCompareExchange64(
            tick, static_cast<LONG64>(value), oldValue);
        if (previous == oldValue) {
            break;
        }

        oldValue = previous;
    }
}

static void ExtendInputCaptureWindow(DWORD durationMs) {
    if (!g_sharedState || durationMs == 0) {
        return;
    }

    ExtendSharedTickTo(&g_sharedState->captureUntilTick,
                       GetTickCount64() + durationMs);
}

static void StopInputCaptureWindow() {
    if (!g_sharedState) {
        return;
    }

    InterlockedExchange64(&g_sharedState->captureUntilTick, 0);
}

static bool IsInputCaptureActive() {
    if (!g_sharedState) {
        return false;
    }

    ULONGLONG until = ReadTick(&g_sharedState->captureUntilTick);
    return until != 0 && GetTickCount64() < until;
}

static ULONGLONG GetLastCapturedInputTick() {
    if (!g_sharedState) {
        return 0;
    }

    return ReadTick(&g_sharedState->lastInputTick);
}

static void MarkCapturedInputTick(ULONGLONG now, DWORD transitionCaptureMs) {
    if (!g_sharedState) {
        return;
    }

    InterlockedExchange64(&g_sharedState->lastInputTick,
                          static_cast<LONG64>(now));
    ExtendSharedTickTo(&g_sharedState->captureUntilTick,
                       now + transitionCaptureMs);
}

enum class PendingTextSource {
    Typed,
    ClipboardPaste,
};

static void AppendPendingText(const std::wstring& text,
                              DWORD transitionCaptureMs,
                              PendingTextSource source = PendingTextSource::Typed) {
    if (!g_sharedState || text.empty()) {
        return;
    }

    SharedStateLock lock;
    if (!lock.locked()) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    MarkCapturedInputTick(now, transitionCaptureMs);

    ULONGLONG existingTick = ReadTick(&g_sharedState->pendingTextTick);
    if (existingTick != 0 && (now - existingTick) > kPendingTextMaxAgeMs) {
        ClearPendingTextLocked();
    }

    size_t existingLength = wcsnlen(g_sharedState->pendingText, kMaxPendingTextLength);
    size_t remaining = (kMaxPendingTextLength - 1) - existingLength;
    if (remaining == 0) {
        return;
    }

    if (source == PendingTextSource::ClipboardPaste && existingLength == 0) {
        InterlockedExchange(&g_sharedState->pendingTextCanPasteOriginal, 1);
    } else {
        InterlockedExchange(&g_sharedState->pendingTextCanPasteOriginal, 0);
    }

    size_t copyLength = std::min(remaining, text.size());
    wcsncpy_s(g_sharedState->pendingText + existingLength,
              kMaxPendingTextLength - existingLength, text.c_str(), copyLength);
    InterlockedExchange64(&g_sharedState->pendingTextTick, static_cast<LONG64>(now));
    log_if(L"Captured pending text length=%zu",
           wcsnlen(g_sharedState->pendingText, kMaxPendingTextLength));
}

static bool RemoveLastPendingChar(DWORD transitionCaptureMs) {
    if (!g_sharedState) {
        return false;
    }

    SharedStateLock lock;
    if (!lock.locked()) {
        return false;
    }

    ULONGLONG now = GetTickCount64();
    MarkCapturedInputTick(now, transitionCaptureMs);

    ULONGLONG existingTick = ReadTick(&g_sharedState->pendingTextTick);
    if (existingTick != 0 && (now - existingTick) > kPendingTextMaxAgeMs) {
        ClearPendingTextLocked();
        return false;
    }

    size_t length = wcsnlen(g_sharedState->pendingText, kMaxPendingTextLength);
    if (length == 0) {
        return false;
    }

    InterlockedExchange(&g_sharedState->pendingTextCanPasteOriginal, 0);

    length--;
    if (length > 0 && g_sharedState->pendingText[length] >= 0xDC00 &&
        g_sharedState->pendingText[length] <= 0xDFFF &&
        g_sharedState->pendingText[length - 1] >= 0xD800 &&
        g_sharedState->pendingText[length - 1] <= 0xDBFF) {
        length--;
    }

    g_sharedState->pendingText[length] = L'\0';
    InterlockedExchange64(&g_sharedState->pendingTextTick,
                          static_cast<LONG64>(now));
    log_if(L"Backspace applied to pending text length=%zu",
           wcsnlen(g_sharedState->pendingText, kMaxPendingTextLength));
    return true;
}

struct PendingTextBatch {
    std::wstring text;
    bool canPasteOriginal = false;
};

static PendingTextBatch ConsumePendingTextBatch() {
    PendingTextBatch batch;
    if (!g_sharedState) {
        return batch;
    }

    SharedStateLock lock(500);
    if (!lock.locked()) {
        return batch;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG tick = ReadTick(&g_sharedState->pendingTextTick);
    if (tick == 0 || (now - tick) > kPendingTextMaxAgeMs) {
        ClearPendingTextLocked();
        return batch;
    }

    batch.text = g_sharedState->pendingText;
    batch.canPasteOriginal =
        InterlockedCompareExchange(&g_sharedState->pendingTextCanPasteOriginal,
                                   0, 0) != 0;
    ClearPendingTextLocked();
    return batch;
}

static bool HasPendingText() {
    if (!g_sharedState) {
        return false;
    }

    SharedStateLock lock;
    if (!lock.locked()) {
        return false;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG tick = ReadTick(&g_sharedState->pendingTextTick);
    if (tick != 0 && (now - tick) > kPendingTextMaxAgeMs) {
        ClearPendingTextLocked();
        return false;
    }

    return g_sharedState->pendingText[0] != L'\0';
}

static bool TryBeginLaunch(DWORD debounceMs) {
    if (!g_sharedState) {
        return true;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG last =
        static_cast<ULONGLONG>(InterlockedCompareExchange64(&g_sharedState->lastLaunchTick, 0, 0));
    if (debounceMs > 0 && last != 0 && (now - last) < debounceMs) {
        log_if(L"Debounced global launch");
        return false;
    }

    if (InterlockedCompareExchange(&g_sharedState->launchInProgress, 1, 0) != 0) {
        if (last != 0 && (now - last) > 8000) {
            log_if(L"Recovering stale launch state");
            InterlockedExchange(&g_sharedState->launchInProgress, 0);
            if (InterlockedCompareExchange(&g_sharedState->launchInProgress, 1,
                                           0) == 0) {
                InterlockedExchange64(&g_sharedState->lastLaunchTick,
                                      static_cast<LONG64>(now));
                return true;
            }
        }

        log_if(L"Launch already in progress in another process");
        return false;
    }

    InterlockedExchange64(&g_sharedState->lastLaunchTick, static_cast<LONG64>(now));
    return true;
}

static void EndLaunch() {
    if (g_sharedState) {
        InterlockedExchange(&g_sharedState->launchInProgress, 0);
    }
}

// -------------------- Input capture --------------------

static bool LaunchReplacement();
static bool IsReplacementTargetAvailable();

static bool CanRedirectSearch(PCWSTR reason) {
    if (IsReplacementTargetAvailable()) {
        return true;
    }

    log_if(L"%s left to Windows Search: replacement target is unavailable",
           reason ? reason : L"Search request");
    return false;
}

static bool RequestReplacement(PCWSTR reason) {
    if (IsUnloading()) {
        return false;
    }

    if (!CanRedirectSearch(reason)) {
        return false;
    }

    bool launched = LaunchReplacement();
    log_if(L"%s %s", reason ? reason : L"Search request",
           launched ? L"intercepted" : L"left to Windows Search: launch busy");
    return launched;
}

enum class RedirectCategory {
    WinS,
    StartMenuTyping,
    SearchHostTyping,
    StartMenuSearchBoxClick,
    StartMenuSearchTransitions,
    TaskbarSearch,
    UndockedSearch,
    SearchProtocol,
};

static bool IsFlagEnabled(volatile LONG* flag) {
    return InterlockedCompareExchange(flag, 0, 0) != 0;
}

static bool IsRedirectCategoryEnabled(RedirectCategory category) {
    switch (category) {
        case RedirectCategory::WinS:
            return IsFlagEnabled(&g_redirectWinSEnabled);

        case RedirectCategory::StartMenuTyping:
            return IsFlagEnabled(&g_redirectStartMenuTypingEnabled);

        case RedirectCategory::SearchHostTyping:
            return IsFlagEnabled(&g_redirectSearchHostTypingEnabled);

        case RedirectCategory::StartMenuSearchBoxClick:
            return IsFlagEnabled(&g_redirectStartMenuSearchBoxClickEnabled);

        case RedirectCategory::StartMenuSearchTransitions:
            return IsFlagEnabled(&g_redirectStartMenuSearchTransitionsEnabled);

        case RedirectCategory::TaskbarSearch:
            return IsFlagEnabled(&g_redirectTaskbarSearchEnabled);

        case RedirectCategory::UndockedSearch:
            return IsFlagEnabled(&g_redirectUndockedSearchEnabled);

        case RedirectCategory::SearchProtocol:
            return IsFlagEnabled(&g_redirectSearchProtocolEnabled);
    }

    return true;
}

static bool RequestReplacementIfCategoryEnabled(RedirectCategory category,
                                                PCWSTR reason) {
    return IsRedirectCategoryEnabled(category) && RequestReplacement(reason);
}

static std::wstring GetProcessNameFromPid(DWORD pid) {
    if (!pid) {
        return L"";
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }

    wchar_t path[MAX_PATH];
    DWORD size = _countof(path);
    std::wstring name;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        name = PathFindFileNameW(path);
    }

    CloseHandle(process);
    return name;
}

static bool IsExplorerShellSurface(HWND hwnd) {
    wchar_t className[256] = {};
    if (!GetClassNameW(hwnd, className, _countof(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           StrStrIW(className, L"Xaml") != nullptr ||
           StrStrIW(className, L"Windows.UI") != nullptr;
}

struct ForegroundSnapshot {
    HWND hwnd = nullptr;
    std::wstring processName;
};

static ForegroundSnapshot CaptureForegroundSnapshot() {
    ForegroundSnapshot snapshot;
    snapshot.hwnd = GetForegroundWindow();
    if (!snapshot.hwnd) {
        return snapshot;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(snapshot.hwnd, &pid);
    snapshot.processName = GetProcessNameFromPid(pid);
    return snapshot;
}

static ForegroundSnapshot CaptureForegroundSnapshotCached(DWORD maxAgeMs) {
    // Only used from the low-level keyboard hook thread.
    static HWND cachedHwnd = nullptr;
    static ULONGLONG cachedTick = 0;
    static ForegroundSnapshot cachedSnapshot;

    HWND hwnd = GetForegroundWindow();
    ULONGLONG now = GetTickCount64();
    if (hwnd && hwnd == cachedHwnd && cachedTick != 0 &&
        (now - cachedTick) <= maxAgeMs) {
        return cachedSnapshot;
    }

    cachedSnapshot = CaptureForegroundSnapshot();
    cachedHwnd = cachedSnapshot.hwnd;
    cachedTick = now;
    return cachedSnapshot;
}

static bool IsShellForeground(HWND hwnd, const std::wstring& processName) {
    if (processName.empty()) {
        return false;
    }

    if (_wcsicmp(processName.c_str(), L"StartMenuExperienceHost.exe") == 0 ||
        _wcsicmp(processName.c_str(), L"SearchHost.exe") == 0) {
        return true;
    }

    return _wcsicmp(processName.c_str(), L"explorer.exe") == 0 &&
           IsExplorerShellSurface(hwnd);
}

static bool ShouldCaptureTypedSearchInput() {
    ForegroundSnapshot snapshot = CaptureForegroundSnapshotCached(75);
    if (_wcsicmp(snapshot.processName.c_str(),
                 L"StartMenuExperienceHost.exe") == 0) {
        return IsRedirectCategoryEnabled(RedirectCategory::StartMenuTyping);
    }

    if (_wcsicmp(snapshot.processName.c_str(), L"SearchHost.exe") == 0) {
        return IsRedirectCategoryEnabled(RedirectCategory::SearchHostTyping);
    }

    return false;
}

static std::wstring TrimString(std::wstring value);

static std::wstring SanitizeSearchText(const std::wstring& input) {
    std::wstring output;
    output.reserve(std::min(input.size(), kMaxPendingTextLength - 1));

    bool lastWasSpace = false;
    for (wchar_t ch : input) {
        if (output.size() >= kMaxPendingTextLength - 1) {
            break;
        }

        if (ch == L'\r' || ch == L'\n' || ch == L'\t') {
            if (!lastWasSpace) {
                output.push_back(L' ');
                lastWasSpace = true;
            }
            continue;
        }

        if (iswcntrl(ch)) {
            continue;
        }

        output.push_back(ch);
        lastWasSpace = iswspace(ch) != 0;
    }

    return TrimString(output);
}

static std::wstring ReadClipboardRawTextBounded(bool* truncated = nullptr) {
    if (truncated) {
        *truncated = false;
    }

    if (!OpenClipboard(nullptr)) {
        return L"";
    }

    std::wstring result;
    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (data) {
        const wchar_t* text = static_cast<const wchar_t*>(GlobalLock(data));
        if (text) {
            constexpr size_t maxCopy = kMaxPendingTextLength - 1;
            size_t length = wcsnlen(text, maxCopy + 1);
            if (length > maxCopy) {
                if (truncated) {
                    *truncated = true;
                }
                length = maxCopy;
            }
            result.assign(text, length);
            GlobalUnlock(data);
        }
    }

    CloseClipboard();
    return result;
}

static std::wstring ReadClipboardText(bool* canPasteOriginal = nullptr) {
    bool truncated = false;
    std::wstring rawText = ReadClipboardRawTextBounded(&truncated);
    std::wstring sanitizedText = SanitizeSearchText(rawText);
    if (canPasteOriginal) {
        *canPasteOriginal = !truncated && !sanitizedText.empty() &&
                            sanitizedText == rawText;
    }

    return sanitizedText;
}

static bool ClipboardTextEquals(const std::wstring& expected) {
    bool truncated = false;
    std::wstring raw = ReadClipboardRawTextBounded(&truncated);
    return !truncated && raw == expected;
}

static BYTE g_trackedKeyboardState[256] = {};

static bool IsVirtualKeyDown(UINT vk) {
    return vk < ARRAYSIZE(g_trackedKeyboardState) &&
           (g_trackedKeyboardState[vk] & 0x80) != 0;
}

static void RefreshKeyboardToggleState() {
    g_trackedKeyboardState[VK_CAPITAL] =
        (GetKeyState(VK_CAPITAL) & 0x0001) ? 0x01 : 0;
    g_trackedKeyboardState[VK_NUMLOCK] =
        (GetKeyState(VK_NUMLOCK) & 0x0001) ? 0x01 : 0;
    g_trackedKeyboardState[VK_SCROLL] =
        (GetKeyState(VK_SCROLL) & 0x0001) ? 0x01 : 0;
}

static void RefreshTrackedKeyboardState() {
    ZeroMemory(g_trackedKeyboardState, sizeof(g_trackedKeyboardState));
    for (UINT vk = 0; vk < ARRAYSIZE(g_trackedKeyboardState); vk++) {
        if (GetAsyncKeyState(vk) & 0x8000) {
            g_trackedKeyboardState[vk] = 0x80;
        }
    }

    RefreshKeyboardToggleState();
}

static void UpdateAggregateModifierState(UINT vk) {
    switch (vk) {
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
            g_trackedKeyboardState[VK_SHIFT] =
                (IsVirtualKeyDown(VK_LSHIFT) || IsVirtualKeyDown(VK_RSHIFT) ||
                 (vk == VK_SHIFT && IsVirtualKeyDown(VK_SHIFT)))
                    ? 0x80
                    : 0;
            break;

        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_CONTROL:
            g_trackedKeyboardState[VK_CONTROL] =
                (IsVirtualKeyDown(VK_LCONTROL) ||
                 IsVirtualKeyDown(VK_RCONTROL) ||
                 (vk == VK_CONTROL && IsVirtualKeyDown(VK_CONTROL)))
                    ? 0x80
                    : 0;
            break;

        case VK_LMENU:
        case VK_RMENU:
        case VK_MENU:
            g_trackedKeyboardState[VK_MENU] =
                (IsVirtualKeyDown(VK_LMENU) || IsVirtualKeyDown(VK_RMENU) ||
                 (vk == VK_MENU && IsVirtualKeyDown(VK_MENU)))
                    ? 0x80
                    : 0;
            break;
    }
}

static void UpdateTrackedKeyboardState(const KBDLLHOOKSTRUCT* keyboardInfo,
                                       bool keyDown) {
    if (!keyboardInfo || keyboardInfo->vkCode >= ARRAYSIZE(g_trackedKeyboardState)) {
        return;
    }

    UINT vk = keyboardInfo->vkCode;
    if (keyDown) {
        g_trackedKeyboardState[vk] |= 0x80;
    } else {
        g_trackedKeyboardState[vk] &= ~0x80;
    }

    UpdateAggregateModifierState(vk);
    RefreshKeyboardToggleState();
}

static std::wstring TranslateKeyToUnicode(const KBDLLHOOKSTRUCT* keyboardInfo) {
    if (!keyboardInfo) {
        return L"";
    }

    if (IsVirtualKeyDown(VK_LWIN) || IsVirtualKeyDown(VK_RWIN)) {
        return L"";
    }

    HWND foreground = GetForegroundWindow();
    DWORD foregroundThreadId =
        foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    HKL keyboardLayout =
        GetKeyboardLayout(foregroundThreadId ? foregroundThreadId : 0);

    BYTE keyboardState[256] = {};
    CopyMemory(keyboardState, g_trackedKeyboardState, sizeof(keyboardState));
    RefreshKeyboardToggleState();
    keyboardState[VK_CAPITAL] = g_trackedKeyboardState[VK_CAPITAL];
    keyboardState[VK_NUMLOCK] = g_trackedKeyboardState[VK_NUMLOCK];
    keyboardState[VK_SCROLL] = g_trackedKeyboardState[VK_SCROLL];
    if (keyboardInfo->vkCode < ARRAYSIZE(keyboardState)) {
        keyboardState[keyboardInfo->vkCode] = 0x80;
    }

    UINT scanCode = keyboardInfo->scanCode;
    if (keyboardInfo->flags & LLKHF_EXTENDED) {
        scanCode |= 0x100;
    }

    wchar_t translated[8] = {};
    constexpr UINT kDoNotMutateKeyboardState = 0x4;
    int rc = ToUnicodeEx(static_cast<UINT>(keyboardInfo->vkCode), scanCode,
                         keyboardState, translated, _countof(translated),
                         kDoNotMutateKeyboardState, keyboardLayout);
    if (rc == 1 && !iswcntrl(translated[0])) {
        return std::wstring(1, translated[0]);
    }

    if (rc > 1) {
        return std::wstring(translated, translated + rc);
    }

    return L"";
}

static bool IsPasteShortcut(const KBDLLHOOKSTRUCT* keyboardInfo) {
    if (!keyboardInfo) {
        return false;
    }

    const bool ctrlDown = IsVirtualKeyDown(VK_CONTROL);
    const bool altDown = IsVirtualKeyDown(VK_MENU);
    const bool shiftDown = IsVirtualKeyDown(VK_SHIFT);

    // AltGr surfaces as Ctrl+Alt; that's a layout-defined dead key combination,
    // not a paste request.
    if (ctrlDown && !altDown && keyboardInfo->vkCode == 'V') {
        return true;
    }

    if (shiftDown && keyboardInfo->vkCode == VK_INSERT) {
        return true;
    }

    return false;
}

static void SuppressOwnInjectedInputFor(DWORD durationMs) {
    if (durationMs == 0) {
        return;
    }

    ULONGLONG until = GetTickCount64() + durationMs;
    ExtendSharedTickTo(&g_ignoreInjectedUntilTick, until);
}

static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;
static IUIAutomation* g_uiAutomation = nullptr;
static bool g_uiAutomationComInitialized = false;
static HANDLE g_keyboardThread = nullptr;
static HANDLE g_keyboardThreadReadyEvent = nullptr;
static DWORD g_keyboardThreadId = 0;
static HANDLE g_moduleWatcherThread = nullptr;
static HANDLE g_moduleWatcherStopEvent = nullptr;
static HANDLE g_noLaunchThreadsEvent = nullptr;
static volatile LONG g_activeLaunchThreads = 0;
static SRWLOCK g_launchTrackingLock = SRWLOCK_INIT;
static std::atomic<bool> g_searchUxSymbolsHooked = false;
static std::atomic<bool> g_startDockedSymbolsHooked = false;
static std::atomic<bool> g_twinuiSymbolsHooked = false;
static volatile LONG64 g_ignoreUndockedSearchUntilTick = 0;

static void ExtendLocalTickTo(volatile LONG64* target, ULONGLONG until) {
    for (;;) {
        LONG64 current = InterlockedCompareExchange64(target, 0, 0);
        if (static_cast<ULONGLONG>(current) >= until) {
            return;
        }

        if (InterlockedCompareExchange64(target, static_cast<LONG64>(until),
                                         current) == current) {
            return;
        }
    }
}

static bool IsLocalTickActive(volatile LONG64* target) {
    ULONGLONG until =
        static_cast<ULONGLONG>(InterlockedCompareExchange64(target, 0, 0));
    return until != 0 && GetTickCount64() < until;
}

static void IgnoreUndockedSearchFor(DWORD durationMs = 1500) {
    ExtendLocalTickTo(&g_ignoreUndockedSearchUntilTick,
                      GetTickCount64() + durationMs);
}

static void StopIgnoringUndockedSearch() {
    InterlockedExchange64(&g_ignoreUndockedSearchUntilTick, 0);
}

static bool IsUndockedSearchIgnored() {
    return IsLocalTickActive(&g_ignoreUndockedSearchUntilTick);
}

static bool IsStartMenuWindowAtPoint(POINT pt) {
    HWND hit = WindowFromPoint(pt);
    HWND root = hit ? GetAncestor(hit, GA_ROOT) : nullptr;
    if (!root) {
        root = hit;
    }

    DWORD pid = 0;
    if (root) {
        GetWindowThreadProcessId(root, &pid);
    }

    std::wstring processName = GetProcessNameFromPid(pid);
    return _wcsicmp(processName.c_str(), L"StartMenuExperienceHost.exe") == 0;
}

static bool UiaStringContainsAny(BSTR value) {
    if (!value) {
        return false;
    }

    return StrStrIW(value, L"Search") || StrStrIW(value, L"Rechercher") ||
           StrStrIW(value, L"SearchBox") || StrStrIW(value, L"FakeSearchBox");
}

static bool UiaElementLooksLikeStartSearchBox(IUIAutomationElement* element) {
    if (!element) {
        return false;
    }

    BSTR name = nullptr;
    BSTR automationId = nullptr;
    BSTR className = nullptr;
    CONTROLTYPEID controlType = 0;

    element->get_CurrentName(&name);
    element->get_CurrentAutomationId(&automationId);
    element->get_CurrentClassName(&className);
    element->get_CurrentControlType(&controlType);

    bool textMatches = UiaStringContainsAny(name) ||
                       UiaStringContainsAny(automationId) ||
                       UiaStringContainsAny(className);
    bool typeMatches = controlType == UIA_EditControlTypeId ||
                       controlType == UIA_ButtonControlTypeId ||
                       controlType == UIA_PaneControlTypeId ||
                       controlType == UIA_GroupControlTypeId;

    if (name) {
        SysFreeString(name);
    }
    if (automationId) {
        SysFreeString(automationId);
    }
    if (className) {
        SysFreeString(className);
    }

    return textMatches && typeMatches;
}

static bool IsStartSearchBoxAtPoint(POINT pt) {
    if (!g_uiAutomation || !IsStartMenuWindowAtPoint(pt)) {
        return false;
    }

    IUIAutomationElement* element = nullptr;
    HRESULT hr = g_uiAutomation->ElementFromPoint(pt, &element);
    if (FAILED(hr) || !element) {
        return false;
    }

    bool matched = false;
    IUIAutomationTreeWalker* walker = nullptr;
    g_uiAutomation->get_ControlViewWalker(&walker);

    IUIAutomationElement* current = element;
    for (int depth = 0; current && depth < 8; depth++) {
        if (UiaElementLooksLikeStartSearchBox(current)) {
            matched = true;
            break;
        }

        if (!walker) {
            break;
        }

        IUIAutomationElement* parent = nullptr;
        if (FAILED(walker->GetParentElement(current, &parent)) || !parent) {
            break;
        }

        if (current != element) {
            current->Release();
        }
        current = parent;
    }

    if (current && current != element) {
        current->Release();
    }
    if (walker) {
        walker->Release();
    }
    element->Release();

    return matched;
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_LBUTTONDOWN && lParam) {
        const auto* mouseInfo = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        if (IsRedirectCategoryEnabled(
                RedirectCategory::StartMenuSearchBoxClick) &&
            IsStartSearchBoxAtPoint(mouseInfo->pt) &&
            RequestReplacement(L"Start menu search box UIA click")) {
            return 1;
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION &&
        (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN ||
         wParam == WM_KEYUP || wParam == WM_SYSKEYUP) &&
        lParam) {
        const auto* keyboardInfo = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        const bool injected = (keyboardInfo->flags & LLKHF_INJECTED) != 0;
        if (injected && keyboardInfo->dwExtraInfo == kOwnInjectedInputMarker) {
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }

        if (injected && IsLocalTickActive(&g_ignoreInjectedUntilTick)) {
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }

        if (injected) {
            SettingsSnapshot injectedSettings = GetSettingsSnapshot();
            if (!injectedSettings.allowInjectedInput) {
                return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
            }
        }

        const bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        UpdateTrackedKeyboardState(keyboardInfo, keyDown);

        if (!keyDown) {
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }

        const bool leftWinDown = IsVirtualKeyDown(VK_LWIN);
        const bool rightWinDown = IsVirtualKeyDown(VK_RWIN);
        const bool ctrlDown = IsVirtualKeyDown(VK_CONTROL);
        const bool altDown = IsVirtualKeyDown(VK_MENU);
        const bool shiftDown = IsVirtualKeyDown(VK_SHIFT);

        if ((keyboardInfo->vkCode == VK_LWIN ||
             keyboardInfo->vkCode == VK_RWIN) &&
            !ctrlDown && !altDown && !shiftDown) {
            IgnoreUndockedSearchFor();
        }

        if (keyboardInfo->vkCode == 'S' && (leftWinDown || rightWinDown) &&
            !ctrlDown && !altDown && !shiftDown) {
            if (IsRedirectCategoryEnabled(RedirectCategory::WinS) &&
                RequestReplacement(L"Win+S")) {
                ClearPendingText();
                return 1;
            }

            StopIgnoringUndockedSearch();
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }

        const bool captureActive = IsInputCaptureActive();
        const bool searchTypingContext =
            captureActive ? false : ShouldCaptureTypedSearchInput();
        if (captureActive || searchTypingContext) {
            const SettingsSnapshot settings = GetSettingsSnapshot();
            if (IsPasteShortcut(keyboardInfo)) {
                bool canPasteOriginal = false;
                std::wstring clipboardText = ReadClipboardText(&canPasteOriginal);
                if (!clipboardText.empty()) {
                    if (!CanRedirectSearch(L"Start/search paste")) {
                        return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                              lParam);
                    }

                    log_if(L"Start/search paste intercepted");
                    AppendPendingText(clipboardText,
                                      settings.transitionCaptureMs,
                                      canPasteOriginal
                                          ? PendingTextSource::ClipboardPaste
                                          : PendingTextSource::Typed);
                    if (LaunchReplacement()) {
                        return 1;
                    }

                    ClearPendingText();
                    return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                          lParam);
                }
            } else if (keyboardInfo->vkCode == VK_BACK) {
                if (!captureActive && !HasPendingText()) {
                    return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                          lParam);
                }

                if (!CanRedirectSearch(L"Start/search backspace")) {
                    return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                          lParam);
                }

                log_if(L"Backspace intercepted during search redirection");
                RemoveLastPendingChar(settings.transitionCaptureMs);
                if (LaunchReplacement()) {
                    return 1;
                }

                ClearPendingText();
                return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
            } else {
                std::wstring translated = TranslateKeyToUnicode(keyboardInfo);
                if (!translated.empty()) {
                    if (!CanRedirectSearch(L"Start/search typing")) {
                        return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                              lParam);
                    }

                    log_if(L"Start/search typing intercepted length=%zu",
                           translated.size());
                    AppendPendingText(translated, settings.transitionCaptureMs);
                    if (LaunchReplacement()) {
                        return 1;
                    }

                    ClearPendingText();
                    return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                          lParam);
                }
            }

            if (captureActive) {
                if (!CanRedirectSearch(L"Transition key")) {
                    StopInputCaptureWindow();
                    return CallNextHookEx(g_keyboardHook, nCode, wParam,
                                          lParam);
                }

                ExtendInputCaptureWindow(settings.transitionCaptureMs);
                log_if(L"Swallowed transition key vk=%u", keyboardInfo->vkCode);
                return 1;
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static DWORD WINAPI KeyboardHookThreadProc(LPVOID) {
    MSG dummy;
    PeekMessageW(&dummy, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_uiAutomationComInitialized = SUCCEEDED(coInit);
    HRESULT uiaHr = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                     CLSCTX_INPROC_SERVER,
                                     IID_PPV_ARGS(&g_uiAutomation));
    if (FAILED(uiaHr)) {
        g_uiAutomation = nullptr;
        log_if(L"UI Automation unavailable for Start search click fallback: 0x%08X",
               static_cast<unsigned>(uiaHr));
    }

    RefreshTrackedKeyboardState();

    HMODULE currentModule = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&KeyboardHookThreadProc),
                            &currentModule)) {
        currentModule = nullptr;
    }

    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, currentModule, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"[ReplaceSearch] SetWindowsHookExW failed: %u", GetLastError());
    }

    g_mouseHook =
        SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, currentModule, 0);
    if (!g_mouseHook) {
        Wh_Log(L"[ReplaceSearch] SetWindowsHookExW mouse failed: %u",
               GetLastError());
    }

    SetEvent(g_keyboardThreadReadyEvent);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
    }

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    if (g_uiAutomation) {
        g_uiAutomation->Release();
        g_uiAutomation = nullptr;
    }

    if (g_uiAutomationComInitialized) {
        CoUninitialize();
        g_uiAutomationComInitialized = false;
    }

    return 0;
}

static void StartKeyboardHookThread() {
    if (g_keyboardThread) {
        return;
    }

    g_keyboardThreadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_keyboardThreadReadyEvent) {
        Wh_Log(L"[ReplaceSearch] CreateEventW failed: %u", GetLastError());
        return;
    }

    g_keyboardThread = CreateThread(nullptr, 0, KeyboardHookThreadProc, nullptr, 0,
                                    &g_keyboardThreadId);
    if (!g_keyboardThread) {
        Wh_Log(L"[ReplaceSearch] CreateThread failed: %u", GetLastError());
        CloseHandle(g_keyboardThreadReadyEvent);
        g_keyboardThreadReadyEvent = nullptr;
        return;
    }

    WaitForSingleObject(g_keyboardThreadReadyEvent, 5000);
    CloseHandle(g_keyboardThreadReadyEvent);
    g_keyboardThreadReadyEvent = nullptr;
}

static void StopKeyboardHookThread() {
    if (!g_keyboardThread) {
        return;
    }

    if (!PostThreadMessageW(g_keyboardThreadId, WM_QUIT, 0, 0)) {
        Wh_Log(L"[ReplaceSearch] PostThreadMessageW(WM_QUIT) failed: %u",
               GetLastError());
    }
    WaitForSingleObject(g_keyboardThread, INFINITE);
    CloseHandle(g_keyboardThread);
    g_keyboardThread = nullptr;
    g_keyboardThreadId = 0;
}

// -------------------- PowerToys launcher helpers --------------------

constexpr DWORD kLauncherForegroundTimeoutMs = 2500;

static std::wstring TrimString(std::wstring value) {
    auto isSpace = [](wchar_t ch) { return iswspace(ch) != 0; };
    value.erase(value.begin(),
                std::find_if(value.begin(), value.end(),
                             [&](wchar_t ch) { return !isSpace(ch); }));
    value.erase(std::find_if(value.rbegin(), value.rend(),
                             [&](wchar_t ch) { return !isSpace(ch); })
                    .base(),
                value.end());
    return value;
}

static std::vector<std::wstring> SplitProcessNameList(
    const std::wstring& value) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= value.size()) {
        size_t end = value.find_first_of(L";,", start);
        std::wstring part =
            TrimString(value.substr(start, end == std::wstring::npos
                                               ? std::wstring::npos
                                               : end - start));
        if (!part.empty()) {
            result.push_back(part);
        }

        if (end == std::wstring::npos) {
            break;
        }

        start = end + 1;
    }

    return result;
}

static bool ProcessNameMatchesAny(const std::wstring& processName,
                                  const std::vector<std::wstring>& names) {
    if (processName.empty()) {
        return false;
    }

    for (const auto& name : names) {
        if (_wcsicmp(processName.c_str(), name.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

static bool LooksLikeUri(const std::wstring& value) {
    std::wstring trimmed = TrimString(value);
    size_t colon = trimmed.find(L':');
    if (colon == std::wstring::npos || colon == 0) {
        return false;
    }

    if (colon == 1 && iswalpha(trimmed[0]) &&
        trimmed.size() > 2 &&
        (trimmed[2] == L'\\' || trimmed[2] == L'/')) {
        return false;
    }

    for (size_t i = 0; i < colon; i++) {
        wchar_t ch = trimmed[i];
        if (!iswalnum(ch) && ch != L'+' && ch != L'-' && ch != L'.') {
            return false;
        }
    }

    return true;
}

static std::wstring ProcessNameFromCommandPath(const std::wstring& command) {
    if (command.empty()) {
        return L"";
    }

    std::wstring trimmed = TrimString(command);
    if (LooksLikeUri(trimmed)) {
        return L"";
    }

    if (trimmed.size() >= 2 && trimmed.front() == L'"') {
        size_t closingQuote = trimmed.find(L'"', 1);
        if (closingQuote != std::wstring::npos) {
            trimmed = trimmed.substr(1, closingQuote - 1);
        }
    }

    if (trimmed.size() > 1 && trimmed[1] == L':') {
        return PathFindFileNameW(trimmed.c_str());
    }

    if (trimmed.find(L'\\') != std::wstring::npos ||
        trimmed.find(L'/') != std::wstring::npos) {
        return PathFindFileNameW(trimmed.c_str());
    }

    if (_wcsicmp(PathFindExtensionW(trimmed.c_str()), L".exe") == 0) {
        return trimmed;
    }

    return L"";
}

static std::vector<std::wstring> GetConfiguredReplacementProcessNames(
    const SettingsSnapshot& settings) {
    std::vector<std::wstring> configured =
        SplitProcessNameList(settings.customProcessName);
    if (!configured.empty()) {
        return configured;
    }

    switch (settings.launcherTarget) {
        case LauncherTarget::PowerToysRun:
            return {L"PowerToys.PowerLauncher.exe"};

        case LauncherTarget::CommandPalette:
            return {L"Microsoft.CmdPal.UI.exe"};

        case LauncherTarget::CustomCommand: {
            std::wstring inferred =
                ProcessNameFromCommandPath(settings.customCommand);
            return inferred.empty() ? std::vector<std::wstring>{}
                                    : std::vector<std::wstring>{inferred};
        }

        case LauncherTarget::CustomHotkey:
        default:
            return {};
    }
}

static bool IsProcessRunningByName(const std::wstring& processName) {
    if (processName.empty()) {
        return false;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);
    bool found = false;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return found;
}

static PCWSTR GetLauncherDebugName(LauncherTarget target);

static bool IsReplacementTargetAvailableUncached(
    const SettingsSnapshot& settings) {
    if (settings.launcherTarget == LauncherTarget::CustomCommand &&
        TrimString(settings.customCommand).empty()) {
        return false;
    }

    std::vector<std::wstring> processNames =
        GetConfiguredReplacementProcessNames(settings);
    if ((settings.launcherTarget == LauncherTarget::CustomHotkey ||
         settings.launcherTarget == LauncherTarget::CustomCommand) &&
        processNames.empty()) {
        log_if(L"%s requires customProcessName for foreground detection",
               GetLauncherDebugName(settings.launcherTarget));
        return false;
    }

    if (!settings.requireLauncherAvailable) {
        return true;
    }

    if (processNames.empty()) {
        return true;
    }

    for (const auto& name : processNames) {
        if (IsProcessRunningByName(name)) {
            return true;
        }
    }

    return false;
}

static bool IsReplacementTargetAvailable() {
    ULONGLONG now = GetTickCount64();
    ULONGLONG tick = static_cast<ULONGLONG>(
        InterlockedCompareExchange64(&g_targetAvailableCacheTick, 0, 0));
    if (tick != 0 && (now - tick) < 1000) {
        return InterlockedCompareExchange(&g_targetAvailableCacheValue, 0, 0) !=
               0;
    }

    SettingsSnapshot settings = GetSettingsSnapshot();
    bool available = IsReplacementTargetAvailableUncached(settings);
    InterlockedExchange(&g_targetAvailableCacheValue, available ? 1 : 0);
    InterlockedExchange64(&g_targetAvailableCacheTick,
                          static_cast<LONG64>(now));
    return available;
}

static PCWSTR GetLauncherDebugName(LauncherTarget target) {
    switch (target) {
        case LauncherTarget::PowerToysRun:
            return L"PowerToys Run";

        case LauncherTarget::CommandPalette:
            return L"Command Palette";

        case LauncherTarget::CustomHotkey:
            return L"custom hotkey";

        case LauncherTarget::CustomCommand:
            return L"custom command";
    }

    return L"replacement launcher";
}

static bool IsConfiguredReplacementForegroundProcess(
    const std::wstring& processName,
    const SettingsSnapshot& settings) {
    std::vector<std::wstring> names =
        GetConfiguredReplacementProcessNames(settings);
    if (settings.launcherTarget == LauncherTarget::CommandPalette &&
        names.size() > 1 &&
        _wcsicmp(processName.c_str(), L"PowerToys.exe") == 0) {
        return false;
    }

    return ProcessNameMatchesAny(processName, names);
}

static bool SleepUnlessUnloading(DWORD totalMs,
                                 DWORD granularityMs = 25) {
    if (totalMs == 0) {
        return !IsUnloading();
    }

    ULONGLONG deadline = GetTickCount64() + totalMs;
    while (!IsUnloading()) {
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) {
            return true;
        }

        DWORD sleepMs = static_cast<DWORD>(
            std::min<ULONGLONG>(granularityMs, deadline - now));
        Sleep(sleepMs);
    }

    return false;
}

static bool WaitForReplacementForeground(DWORD timeoutMs,
                                         const SettingsSnapshot& settings) {
    ULONGLONG deadline = GetTickCount64() + timeoutMs;
    while (GetTickCount64() < deadline) {
        if (IsUnloading()) {
            return false;
        }

        ForegroundSnapshot current = CaptureForegroundSnapshot();
        if (IsConfiguredReplacementForegroundProcess(current.processName,
                                                     settings)) {
            return true;
        }

        if (!SleepUnlessUnloading(25)) {
            return false;
        }
    }

    return false;
}

static void SendUnicodeText(const std::wstring& text) {
    SuppressOwnInjectedInputFor(static_cast<DWORD>(
        std::min<size_t>(5000, 700 + text.size() * 8)));

    for (wchar_t ch : text) {
        INPUT inputs[2] = {};

        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wScan = ch;
        inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;
        inputs[0].ki.dwExtraInfo = kOwnInjectedInputMarker;

        inputs[1] = inputs[0];
        inputs[1].ki.dwFlags |= KEYEVENTF_KEYUP;

        SuppressOwnInjectedInputFor(700);
        UINT sent = SendInput(2, inputs, sizeof(INPUT));
        if (sent != 2) {
            Wh_Log(L"[ReplaceSearch] SendInput unicode failed: sent=%u error=%u",
                   sent, GetLastError());
        }
        Sleep(2);
    }
}

static void SendVirtualKey(WORD virtualKey, bool keyUp) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    input.ki.dwExtraInfo = kOwnInjectedInputMarker;

    SuppressOwnInjectedInputFor(700);
    UINT sent = SendInput(1, &input, sizeof(input));
    if (sent != 1) {
        Wh_Log(L"[ReplaceSearch] SendInput(%u, keyUp=%d) failed: %u",
               virtualKey, keyUp ? 1 : 0, GetLastError());
    }
}

static void TapVirtualKey(WORD virtualKey) {
    SendVirtualKey(virtualKey, false);
    Sleep(15);
    SendVirtualKey(virtualKey, true);
}

static void ReleaseLauncherModifiers() {
    SendVirtualKey(VK_CONTROL, true);
    SendVirtualKey(VK_LCONTROL, true);
    SendVirtualKey(VK_RCONTROL, true);
    SendVirtualKey(VK_MENU, true);
    SendVirtualKey(VK_LMENU, true);
    SendVirtualKey(VK_RMENU, true);
    SendVirtualKey(VK_LWIN, true);
    SendVirtualKey(VK_RWIN, true);
}

static void SendPasteShortcut() {
    ReleaseLauncherModifiers();
    Sleep(15);
    SendVirtualKey(VK_CONTROL, false);
    Sleep(15);
    TapVirtualKey(L'V');
    Sleep(15);
    SendVirtualKey(VK_CONTROL, true);
}

static void SendPendingTextBatch(const PendingTextBatch& batch,
                                 LauncherTarget target) {
    if (batch.text.empty()) {
        return;
    }

    PCWSTR launcherName = GetLauncherDebugName(target);
    if (batch.canPasteOriginal && ClipboardTextEquals(batch.text)) {
        log_if(L"Pasting original clipboard text to %s", launcherName);
        SendPasteShortcut();
        return;
    }

    log_if(L"Sending pending text to %s length=%zu", launcherName,
           batch.text.size());
    SendUnicodeText(batch.text);
}

static std::wstring ToLowerString(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

static bool ParseHotkeyToken(const std::wstring& token,
                             std::vector<WORD>* modifiers,
                             WORD* key) {
    if (token.empty()) {
        return false;
    }

    std::wstring lower = ToLowerString(token);
    if (lower == L"ctrl" || lower == L"control") {
        modifiers->push_back(VK_CONTROL);
        return true;
    }

    if (lower == L"alt" || lower == L"menu") {
        modifiers->push_back(VK_MENU);
        return true;
    }

    if (lower == L"shift") {
        modifiers->push_back(VK_SHIFT);
        return true;
    }

    if (lower == L"win" || lower == L"windows" || lower == L"super") {
        modifiers->push_back(VK_LWIN);
        return true;
    }

    WORD parsedKey = 0;
    if (lower == L"space") {
        parsedKey = VK_SPACE;
    } else if (lower == L"enter" || lower == L"return") {
        parsedKey = VK_RETURN;
    } else if (lower == L"tab") {
        parsedKey = VK_TAB;
    } else if (lower == L"esc" || lower == L"escape") {
        parsedKey = VK_ESCAPE;
    } else if (lower == L"backspace") {
        parsedKey = VK_BACK;
    } else if (lower == L"delete" || lower == L"del") {
        parsedKey = VK_DELETE;
    } else if (lower == L"insert" || lower == L"ins") {
        parsedKey = VK_INSERT;
    } else if (lower == L"left") {
        parsedKey = VK_LEFT;
    } else if (lower == L"right") {
        parsedKey = VK_RIGHT;
    } else if (lower == L"up") {
        parsedKey = VK_UP;
    } else if (lower == L"down") {
        parsedKey = VK_DOWN;
    } else if (lower.size() >= 2 && lower[0] == L'f') {
        int functionKey = _wtoi(lower.c_str() + 1);
        if (functionKey >= 1 && functionKey <= 24) {
            parsedKey = static_cast<WORD>(VK_F1 + functionKey - 1);
        }
    } else if (lower.size() == 1) {
        wchar_t ch = lower[0];
        if ((ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9')) {
            parsedKey = static_cast<WORD>(towupper(ch));
        }
    }

    if (!parsedKey || !key || *key) {
        return false;
    }

    *key = parsedKey;
    return true;
}

static bool SendConfiguredHotkey(const std::wstring& hotkey) {
    std::vector<WORD> modifiers;
    WORD key = 0;

    size_t start = 0;
    while (start <= hotkey.size()) {
        size_t end = hotkey.find(L'+', start);
        std::wstring token =
            TrimString(hotkey.substr(start, end == std::wstring::npos
                                                ? std::wstring::npos
                                                : end - start));
        if (!ParseHotkeyToken(token, &modifiers, &key)) {
            return false;
        }

        if (end == std::wstring::npos) {
            break;
        }

        start = end + 1;
    }

    if (!key) {
        return false;
    }

    ReleaseLauncherModifiers();
    Sleep(40);

    for (WORD modifier : modifiers) {
        SendVirtualKey(modifier, false);
        Sleep(20);
    }

    TapVirtualKey(key);
    Sleep(20);

    for (auto it = modifiers.rbegin(); it != modifiers.rend(); ++it) {
        SendVirtualKey(*it, true);
        Sleep(20);
    }

    return true;
}

static void DismissWindowsSearchSurfaceIfForeground() {
    ForegroundSnapshot current = CaptureForegroundSnapshot();
    if (!IsShellForeground(current.hwnd, current.processName)) {
        return;
    }

    log_if(L"Dismissing shell surface before launcher summon");
    TapVirtualKey(VK_ESCAPE);
    Sleep(80);
}

static void SendLauncherHotkey(const SettingsSnapshot& settings) {
    std::wstring hotkey = TrimString(settings.customHotkey);
    if (hotkey.empty()) {
        switch (settings.launcherTarget) {
            case LauncherTarget::PowerToysRun:
                hotkey = L"alt+space";
                break;

            default:
                break;
        }
    }

    PCWSTR launcherName = GetLauncherDebugName(settings.launcherTarget);
    if (hotkey.empty()) {
        log_if(L"No hotkey configured for %s", launcherName);
        return;
    }

    log_if(L"Sending %s hotkey %s", launcherName, hotkey.c_str());
    if (!SendConfiguredHotkey(hotkey)) {
        Wh_Log(L"[ReplaceSearch] Invalid hotkey setting: %s", hotkey.c_str());
    }
}

static bool ShellExecuteCommand(PCWSTR file, PCWSTR parameters = nullptr) {
    if (!file || !*file) {
        return false;
    }

    SHELLEXECUTEINFOW execInfo = {};
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_NOASYNC;
    execInfo.lpVerb = L"open";
    execInfo.lpFile = file;
    execInfo.lpParameters = parameters && *parameters ? parameters : nullptr;
    execInfo.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&execInfo)) {
        log_if(L"ShellExecuteExW launched %s", file);
        return true;
    }

    Wh_Log(L"[ReplaceSearch] ShellExecuteExW(%s) failed: %u", file,
           GetLastError());
    return false;
}

static bool ShellExecuteUri(PCWSTR uri) {
    return ShellExecuteCommand(uri);
}

struct ReplacementWindowCandidate {
    HWND hwnd = nullptr;
    bool visible = false;
    RECT rect = {};
    std::wstring className;
};

static bool IsLikelyReplacementWindowClass(const std::wstring& className,
                                           LauncherTarget target) {
    if (target == LauncherTarget::PowerToysRun) {
        return StrStrIW(className.c_str(), L"HwndWrapper[PowerToys.PowerLauncher") ==
               className.c_str();
    }

    if (target == LauncherTarget::CustomHotkey ||
        target == LauncherTarget::CustomCommand) {
        return true;
    }

    return _wcsicmp(className.c_str(), L"WinUIDesktopWin32WindowClass") == 0;
}

static int ScoreReplacementWindowCandidate(bool visible, const RECT& rect,
                                           const std::wstring& className,
                                           LauncherTarget target) {
    if (!IsLikelyReplacementWindowClass(className, target)) {
        return -1;
    }

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 120 || height <= 40) {
        return -1;
    }

    int score = visible ? 1000 : 0;
    score += std::min((width * height) / 1000, 500);
    return score;
}

static bool FindBestReplacementWindowCandidate(
    const SettingsSnapshot& settings,
    ReplacementWindowCandidate* candidate) {
    if (!candidate) {
        return false;
    }

    struct SearchContext {
        ReplacementWindowCandidate best;
        int bestScore = -1;
        const SettingsSnapshot* settings = nullptr;
    } context;
    context.settings = &settings;

    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(lParam);

            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            std::wstring processName = GetProcessNameFromPid(pid);
            if (!IsConfiguredReplacementForegroundProcess(processName,
                                                          *context->settings)) {
                return TRUE;
            }

            wchar_t classBuffer[256] = {};
            if (!GetClassNameW(hwnd, classBuffer, ARRAYSIZE(classBuffer))) {
                return TRUE;
            }

            RECT rect = {};
            if (!GetWindowRect(hwnd, &rect)) {
                return TRUE;
            }

            const bool visible = IsWindowVisible(hwnd) != FALSE;
            std::wstring className = classBuffer;
            int score = ScoreReplacementWindowCandidate(
                visible, rect, className, context->settings->launcherTarget);
            if (score <= context->bestScore) {
                return TRUE;
            }

            context->bestScore = score;
            context->best.hwnd = hwnd;
            context->best.visible = visible;
            context->best.rect = rect;
            context->best.className = className;
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));

    if (!context.best.hwnd) {
        return false;
    }

    *candidate = context.best;
    return true;
}

static bool ForceForegroundWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    HWND foreground = GetForegroundWindow();
    DWORD foregroundThreadId = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD targetThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    bool attachedForeground = false;
    bool attachedTarget = false;
    if (foregroundThreadId && foregroundThreadId != currentThreadId) {
        attachedForeground =
            AttachThreadInput(currentThreadId, foregroundThreadId, TRUE) != FALSE;
    }

    if (targetThreadId && targetThreadId != currentThreadId &&
        targetThreadId != foregroundThreadId) {
        attachedTarget = AttachThreadInput(currentThreadId, targetThreadId, TRUE) != FALSE;
    }

    AllowSetForegroundWindow(ASFW_ANY);

    ShowWindow(hwnd, SW_RESTORE);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    BringWindowToTop(hwnd);
    SetActiveWindow(hwnd);
    BOOL foregroundSet = SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    if (attachedTarget) {
        AttachThreadInput(currentThreadId, targetThreadId, FALSE);
    }

    if (attachedForeground) {
        AttachThreadInput(currentThreadId, foregroundThreadId, FALSE);
    }

    return foregroundSet || GetForegroundWindow() == hwnd;
}

static bool ActivateConfiguredReplacementWindow(
    const SettingsSnapshot& settings) {
    ReplacementWindowCandidate candidate;
    PCWSTR launcherName = GetLauncherDebugName(settings.launcherTarget);
    if (!FindBestReplacementWindowCandidate(settings, &candidate)) {
        log_if(L"No %s window candidate was found", launcherName);
        return false;
    }

    log_if(L"Trying %s window %p (%s, visible=%d, rect=%ld,%ld,%ld,%ld)",
           launcherName, candidate.hwnd, candidate.className.c_str(),
           candidate.visible ? 1 : 0, candidate.rect.left, candidate.rect.top,
           candidate.rect.right, candidate.rect.bottom);

    return ForceForegroundWindow(candidate.hwnd);
}

static bool OpenReplacementWindow(const SettingsSnapshot& settings) {
    if (IsUnloading()) {
        return false;
    }

    DismissWindowsSearchSurfaceIfForeground();

    if (IsUnloading()) {
        return false;
    }

    if (settings.launcherTarget == LauncherTarget::CommandPalette) {
        if (ShellExecuteUri(L"x-cmdpal:") &&
            WaitForReplacementForeground(3000, settings)) {
            return true;
        }

        log_if(L"Command Palette URI did not reach foreground");
    }

    if (settings.launcherTarget == LauncherTarget::CustomCommand &&
        !TrimString(settings.customCommand).empty()) {
        PCWSTR args = LooksLikeUri(settings.customCommand)
                          ? nullptr
                          : settings.customCommandArgs.c_str();
        if (ShellExecuteCommand(settings.customCommand.c_str(), args) &&
            WaitForReplacementForeground(kLauncherForegroundTimeoutMs,
                                         settings)) {
            return true;
        }

        log_if(L"Custom command did not reach foreground");
    }

    if (!IsUnloading() && ActivateConfiguredReplacementWindow(settings) &&
        WaitForReplacementForeground(700, settings)) {
        return true;
    }

    if (!IsUnloading() &&
        (settings.launcherTarget != LauncherTarget::CommandPalette ||
         !TrimString(settings.customHotkey).empty())) {
        SendLauncherHotkey(settings);
        if (WaitForReplacementForeground(1500, settings)) {
            return true;
        }
    }

    log_if(L"%s foreground was not observed",
           GetLauncherDebugName(settings.launcherTarget));
    return false;
}

static bool IsCaptureIdle(DWORD transitionIdleMs) {
    if (transitionIdleMs == 0) {
        return true;
    }

    ULONGLONG lastInputTick = GetLastCapturedInputTick();
    return lastInputTick == 0 ||
           (GetTickCount64() - lastInputTick) >= transitionIdleMs;
}

static void SignalNoLaunchThreadsIfNeeded(LONG remaining) {
    AcquireSRWLockShared(&g_launchTrackingLock);
    HANDLE event = g_noLaunchThreadsEvent;
    if (remaining == 0 && event) {
        SetEvent(event);
    }
    ReleaseSRWLockShared(&g_launchTrackingLock);
}

class LaunchThreadLifetime {
   public:
    ~LaunchThreadLifetime() {
        SignalNoLaunchThreadsIfNeeded(
            InterlockedDecrement(&g_activeLaunchThreads));
    }
};

static bool InitLaunchThreadTracking() {
    if (g_noLaunchThreadsEvent) {
        return true;
    }

    g_noLaunchThreadsEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_noLaunchThreadsEvent) {
        Wh_Log(L"[ReplaceSearch] CreateEventW for launch tracking failed: %u",
               GetLastError());
        return false;
    }

    return true;
}

static bool RegisterLaunchThreadIfAllowed() {
    AcquireSRWLockExclusive(&g_launchTrackingLock);

    if (IsUnloading() || !g_noLaunchThreadsEvent) {
        ReleaseSRWLockExclusive(&g_launchTrackingLock);
        return false;
    }

    InterlockedIncrement(&g_activeLaunchThreads);
    ResetEvent(g_noLaunchThreadsEvent);

    ReleaseSRWLockExclusive(&g_launchTrackingLock);
    return true;
}

static void UnregisterFailedLaunchThreadStart() {
    SignalNoLaunchThreadsIfNeeded(
        InterlockedDecrement(&g_activeLaunchThreads));
}

static void WaitForLaunchThreadsAndCloseTracking() {
    HANDLE event = nullptr;

    AcquireSRWLockShared(&g_launchTrackingLock);
    event = g_noLaunchThreadsEvent;
    ReleaseSRWLockShared(&g_launchTrackingLock);

    if (!event) {
        return;
    }

    WaitForSingleObject(event, INFINITE);

    AcquireSRWLockExclusive(&g_launchTrackingLock);
    if (g_noLaunchThreadsEvent) {
        CloseHandle(g_noLaunchThreadsEvent);
        g_noLaunchThreadsEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_launchTrackingLock);
}

static DWORD WINAPI LaunchThreadProc(LPVOID) {
    LaunchThreadLifetime lifetime;
    SettingsSnapshot settings = GetSettingsSnapshot();
    PCWSTR launcherName = GetLauncherDebugName(settings.launcherTarget);

    if (IsUnloading() || !OpenReplacementWindow(settings)) {
        Wh_Log(L"[ReplaceSearch] Failed to activate %s", launcherName);
        ClearPendingText();
        StopInputCaptureWindow();
        EndLaunch();
        return 0;
    }

    if (settings.textCaptureDelayMs > 0 &&
        !SleepUnlessUnloading(settings.textCaptureDelayMs)) {
        ClearPendingText();
        StopInputCaptureWindow();
        EndLaunch();
        return 0;
    }

    ULONGLONG hardDeadline = GetTickCount64() + 10000;
    for (;;) {
        if (IsUnloading()) {
            ClearPendingText();
            break;
        }

        ForegroundSnapshot current = CaptureForegroundSnapshot();
        if (!IsConfiguredReplacementForegroundProcess(current.processName,
                                                      settings)) {
            PendingTextBatch batch = ConsumePendingTextBatch();
            if (!batch.text.empty()) {
                log_if(L"Dropping pending text because %s lost foreground length=%zu",
                       launcherName, batch.text.size());
            }

            log_if(L"%s is not foreground (%s), ending transaction",
                   launcherName, current.processName.c_str());
            break;
        }

        PendingTextBatch batch = ConsumePendingTextBatch();
        if (!batch.text.empty()) {
            SendPendingTextBatch(batch, settings.launcherTarget);
            if (!SleepUnlessUnloading(15)) {
                ClearPendingText();
                break;
            }
            continue;
        }

        if (!IsInputCaptureActive() || IsCaptureIdle(settings.transitionIdleMs) ||
            GetTickCount64() >= hardDeadline) {
            break;
        } else if (!SleepUnlessUnloading(15)) {
            ClearPendingText();
            break;
        }
    }

    PendingTextBatch finalBatch = IsUnloading() ? PendingTextBatch{}
                                                : ConsumePendingTextBatch();
    if (!finalBatch.text.empty()) {
        ForegroundSnapshot current = CaptureForegroundSnapshot();
        if (IsConfiguredReplacementForegroundProcess(current.processName,
                                                     settings)) {
            SendPendingTextBatch(finalBatch, settings.launcherTarget);
        } else {
            log_if(L"Dropping final pending text because %s is not foreground length=%zu",
                   launcherName, finalBatch.text.size());
        }
    }

    StopInputCaptureWindow();
    EndLaunch();
    return 0;
}

static bool LaunchReplacement() {
    if (IsUnloading()) {
        return false;
    }

    if (!IsReplacementTargetAvailable()) {
        log_if(L"Launch skipped because replacement target is unavailable");
        return false;
    }

    SettingsSnapshot settings = GetSettingsSnapshot();
    ForegroundSnapshot current = CaptureForegroundSnapshot();
    if (IsConfiguredReplacementForegroundProcess(current.processName, settings)) {
        ExtendInputCaptureWindow(settings.transitionCaptureMs);
        return true;
    }

    ExtendInputCaptureWindow(settings.transitionCaptureMs);

    if (!TryBeginLaunch(settings.debounceMs)) {
        return true;
    }

    if (!RegisterLaunchThreadIfAllowed()) {
        StopInputCaptureWindow();
        EndLaunch();
        return false;
    }

    HANDLE thread = CreateThread(nullptr, 0, LaunchThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        UnregisterFailedLaunchThreadStart();
        Wh_Log(L"[ReplaceSearch] CreateThread failed: %u", GetLastError());
        StopInputCaptureWindow();
        EndLaunch();
        return false;
    }

    CloseHandle(thread);
    return true;
}

// -------------------- Start menu XAML helpers --------------------

template <typename T>
static T WinRtObjectFromAbi(void* abi) {
    T object{nullptr};
    if (abi) {
        winrt::copy_from_abi(object, abi);
    }
    return object;
}

static wf::IInspectable InspectableFromAbi(void* abi) {
    return WinRtObjectFromAbi<wf::IInspectable>(abi);
}

static bool ContainsInsensitive(winrt::hstring const& value, PCWSTR needle) {
    return !value.empty() && StrStrIW(value.c_str(), needle) != nullptr;
}

static bool XamlElementLooksLikeStartSearchBox(
    winrt::hstring const& className,
    winrt::hstring const& elementName) {
    return ContainsInsensitive(className, L"SearchBoxToggleButton") ||
           ContainsInsensitive(className, L"FakeSearchBox") ||
           ContainsInsensitive(elementName, L"FakeSearchBox") ||
           ContainsInsensitive(elementName, L"SearchBox");
}

static std::wstring DescribeXamlElement(wf::IInspectable const& object) {
    if (!object) {
        return L"<null>";
    }

    std::wstring description;
    try {
        winrt::hstring className = winrt::get_class_name(object);
        description.assign(className.c_str(), className.size());

        if (auto element = object.try_as<wux::FrameworkElement>()) {
            winrt::hstring name = element.Name();
            if (!name.empty()) {
                description += L"#";
                description.append(name.c_str(), name.size());
            }
        }
    } catch (...) {
        description = L"<inspect failed>";
    }

    return description;
}

static bool IsStartSearchBoxInXamlAncestry(wf::IInspectable const& object,
                                           std::wstring* ancestry) {
    if (!object) {
        return false;
    }

    try {
        auto dependencyObject = object.try_as<wux::DependencyObject>();
        for (int depth = 0; dependencyObject && depth < 20; depth++) {
            wf::IInspectable current =
                dependencyObject.try_as<wf::IInspectable>();
            if (!current) {
                break;
            }

            winrt::hstring className = winrt::get_class_name(current);
            winrt::hstring elementName;
            if (auto element = current.try_as<wux::FrameworkElement>()) {
                elementName = element.Name();
            }

            if (ancestry) {
                if (!ancestry->empty()) {
                    *ancestry += L" <- ";
                }

                ancestry->append(className.c_str(), className.size());
                if (!elementName.empty()) {
                    *ancestry += L"#";
                    ancestry->append(elementName.c_str(), elementName.size());
                }
            }

            if (XamlElementLooksLikeStartSearchBox(className, elementName)) {
                return true;
            }

            dependencyObject = wuxm::VisualTreeHelper::GetParent(dependencyObject);
        }
    } catch (winrt::hresult_error const& e) {
        log_if(L"XAML ancestry inspect failed: 0x%08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        log_if(L"XAML ancestry inspect failed");
    }

    return false;
}

static bool IsStartSearchBoxPointerEvent(void* senderAbi,
                                         wf::IInspectable const& originalSource,
                                         PCWSTR hookName) {
    std::wstring ancestry;
    bool isSearchBox =
        IsStartSearchBoxInXamlAncestry(originalSource, &ancestry);

    if (!isSearchBox && senderAbi) {
        wf::IInspectable sender = InspectableFromAbi(senderAbi);
        isSearchBox = IsStartSearchBoxInXamlAncestry(sender, nullptr);
        if (isSearchBox && ancestry.empty()) {
            ancestry = DescribeXamlElement(sender);
        }
    }

    if (isSearchBox) {
        log_if(L"%s matched Start search box: %s", hookName,
               ancestry.empty() ? L"<sender>" : ancestry.c_str());
    }

    return isSearchBox;
}

static bool HandleStartSearchBoxPointerEvent(void* senderAbi,
                                             void* argsAbi,
                                             PCWSTR hookName) {
    if (!IsRedirectCategoryEnabled(
            RedirectCategory::StartMenuSearchBoxClick) ||
        !argsAbi) {
        return false;
    }

    try {
        auto args = WinRtObjectFromAbi<wuxi::PointerRoutedEventArgs>(argsAbi);
        if (!args) {
            return false;
        }

        if (!IsStartSearchBoxPointerEvent(senderAbi, args.OriginalSource(),
                                          hookName)) {
            return false;
        }

        if (!CanRedirectSearch(hookName)) {
            return false;
        }

        if (!LaunchReplacement()) {
            return false;
        }

        args.Handled(true);
        return true;
    } catch (winrt::hresult_error const& e) {
        log_if(L"%s pointer handling failed: 0x%08X %s", hookName,
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        log_if(L"%s pointer handling failed", hookName);
    }

    return false;
}

static bool HandleStartSearchBoxTappedEvent(void* senderAbi,
                                            void* argsAbi,
                                            PCWSTR hookName) {
    if (!IsRedirectCategoryEnabled(
            RedirectCategory::StartMenuSearchBoxClick) ||
        !argsAbi) {
        return false;
    }

    try {
        auto args = WinRtObjectFromAbi<wuxi::TappedRoutedEventArgs>(argsAbi);
        if (!args) {
            return false;
        }

        if (!IsStartSearchBoxPointerEvent(senderAbi, args.OriginalSource(),
                                          hookName)) {
            return false;
        }

        if (!CanRedirectSearch(hookName)) {
            return false;
        }

        if (!LaunchReplacement()) {
            return false;
        }

        args.Handled(true);
        return true;
    } catch (winrt::hresult_error const& e) {
        log_if(L"%s tapped handling failed: 0x%08X %s", hookName,
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        log_if(L"%s tapped handling failed", hookName);
    }

    return false;
}

static volatile LONG64 g_startSearchBoxPointerActivationUntilTick = 0;

static void MarkStartSearchBoxPointerActivation(DWORD durationMs = 2000) {
    InterlockedExchange64(
        &g_startSearchBoxPointerActivationUntilTick,
        static_cast<LONG64>(GetTickCount64() + durationMs));
}

static bool IsStartSearchBoxPointerActivationLikely() {
    ULONGLONG until =
        static_cast<ULONGLONG>(InterlockedCompareExchange64(
            &g_startSearchBoxPointerActivationUntilTick, 0, 0));
    if (until != 0 && GetTickCount64() < until) {
        return true;
    }

    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
}

// -------------------- Hooks --------------------

using OpenSearchView_t = void(WINAPI*)(void* pThis);
static OpenSearchView_t OpenSearchView_Original;

static void WINAPI OpenSearchView_Hook(void* pThis) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::LauncherFrame::OpenSearchView")) {
        OpenSearchView_Original(pThis);
    }
}

using StartDockedExecuteSearchRequest_t = long(WINAPI*)(void* pThis,
                                                        void* presenter);
static StartDockedExecuteSearchRequest_t StartSearchRequestExecute_Original;

static long WINAPI StartSearchRequestExecute_Hook(void* pThis,
                                                  void* presenter) {
    (void)pThis;
    (void)presenter;
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::StartSearchRequest::Execute")) {
        return StartSearchRequestExecute_Original(pThis, presenter);
    }
    return S_OK;
}

using StartDockedNoArgHook_t = void(WINAPI*)(void* pThis);
static StartDockedNoArgHook_t UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Original;
static StartDockedNoArgHook_t LauncherFrameMoveFocusToSearch_Original;
static StartDockedNoArgHook_t NavigationPaneMoveFocusToSearch_Original;

static void WINAPI UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Hook(
    void* pThis) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::LauncherFrame::UpdateSearchBoxOffsetAndRequestMoveFocusToSearch")) {
        UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Original(pThis);
    }
}

static void WINAPI LauncherFrameMoveFocusToSearch_Hook(void* pThis) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::LauncherFrame::MoveFocusToSearch")) {
        LauncherFrameMoveFocusToSearch_Original(pThis);
    }
}

static void WINAPI NavigationPaneMoveFocusToSearch_Hook(void* pThis) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::NavigationPaneView::MoveFocusToSearch")) {
        NavigationPaneMoveFocusToSearch_Original(pThis);
    }
}

using StartDockedPayloadHook_t = void(WINAPI*)(void* pThis, void* payload);
static StartDockedPayloadHook_t OnTryToMoveFocusToSearchEvent_Original;
static StartDockedPayloadHook_t OnShellHandwritingTryToOpenSearchViewEvent_Original;

using StartDockedObjectEventHook_t = void(WINAPI*)(void* pThis, void* sender,
                                                   void* args);
static StartDockedObjectEventHook_t OnFakeSearchBoxClicked_Original;

static void WINAPI OnTryToMoveFocusToSearchEvent_Hook(void* pThis, void* payload) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::LauncherFrame::OnTryToMoveFocusToSearchEvent")) {
        OnTryToMoveFocusToSearchEvent_Original(pThis, payload);
    }
}

static void WINAPI OnShellHandwritingTryToOpenSearchViewEvent_Hook(void* pThis,
                                                                   void* payload) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchTransitions,
            L"StartDocked::LauncherFrame::OnShellHandwritingTryToOpenSearchViewEvent")) {
        OnShellHandwritingTryToOpenSearchViewEvent_Original(pThis, payload);
    }
}

static void WINAPI OnFakeSearchBoxClicked_Hook(void* pThis, void* sender,
                                               void* args) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::StartMenuSearchBoxClick,
            L"StartDocked::LauncherFrame::OnFakeSearchBoxClicked")) {
        OnFakeSearchBoxClicked_Original(pThis, sender, args);
    }
}

using StartDockedPointerEventHook_t = void(WINAPI*)(void* pThis, void* sender,
                                                    void* args);
static StartDockedPointerEventHook_t
    LauncherFrameOnAllRoutedPointerPressedEvents_Original;
static StartDockedPointerEventHook_t
    NavigationPaneOnAllRoutedPointerPressedEvents_Original;
static StartDockedPointerEventHook_t SearchBoxOnPointerEntered_Original;
static StartDockedPointerEventHook_t SearchBoxOnPointerExited_Original;

static void WINAPI LauncherFrameOnAllRoutedPointerPressedEvents_Hook(
    void* pThis,
    void* sender,
    void* args) {
    if (HandleStartSearchBoxPointerEvent(
            sender, args,
            L"StartDocked::LauncherFrame::OnAllRoutedPointerPressedEvents")) {
        return;
    }

    LauncherFrameOnAllRoutedPointerPressedEvents_Original(pThis, sender, args);
}

static void WINAPI NavigationPaneOnAllRoutedPointerPressedEvents_Hook(
    void* pThis,
    void* sender,
    void* args) {
    if (HandleStartSearchBoxPointerEvent(
            sender, args,
            L"StartDocked::NavigationPaneView::OnAllRoutedPointerPressedEvents")) {
        return;
    }

    NavigationPaneOnAllRoutedPointerPressedEvents_Original(pThis, sender, args);
}

static void WINAPI SearchBoxOnPointerEntered_Hook(void* pThis,
                                                  void* sender,
                                                  void* args) {
    log_if(L"StartDocked::SearchBoxToggleButton::OnPointerEntered");
    SearchBoxOnPointerEntered_Original(pThis, sender, args);
}

static void WINAPI SearchBoxOnPointerExited_Hook(void* pThis,
                                                 void* sender,
                                                 void* args) {
    log_if(L"StartDocked::SearchBoxToggleButton::OnPointerExited");
    SearchBoxOnPointerExited_Original(pThis, sender, args);
}

using StartDockedXamlPointerOverrideHook_t = long(WINAPI*)(void* pThis,
                                                           void* args);
static StartDockedXamlPointerOverrideHook_t
    LauncherFrameOnPointerPressed_Original;
static StartDockedXamlPointerOverrideHook_t LauncherFrameOnTapped_Original;
static StartDockedXamlPointerOverrideHook_t
    StartSizingFrameOnPointerPressed_Original;
static StartDockedXamlPointerOverrideHook_t StartSizingFrameOnTapped_Original;

static long WINAPI LauncherFrameOnPointerPressed_Hook(void* pThis,
                                                      void* args) {
    if (HandleStartSearchBoxPointerEvent(
            nullptr, args,
            L"StartDocked::LauncherFrame::IControlOverrides::OnPointerPressed")) {
        return S_OK;
    }

    return LauncherFrameOnPointerPressed_Original(pThis, args);
}

static long WINAPI LauncherFrameOnTapped_Hook(void* pThis, void* args) {
    if (HandleStartSearchBoxTappedEvent(
            nullptr, args,
            L"StartDocked::LauncherFrame::IControlOverrides::OnTapped")) {
        return S_OK;
    }

    return LauncherFrameOnTapped_Original(pThis, args);
}

static long WINAPI StartSizingFrameOnPointerPressed_Hook(void* pThis,
                                                         void* args) {
    if (HandleStartSearchBoxPointerEvent(
            nullptr, args,
            L"StartDocked::StartSizingFrame::IControlOverrides::OnPointerPressed")) {
        return S_OK;
    }

    return StartSizingFrameOnPointerPressed_Original(pThis, args);
}

static long WINAPI StartSizingFrameOnTapped_Hook(void* pThis, void* args) {
    if (HandleStartSearchBoxTappedEvent(
            nullptr, args,
            L"StartDocked::StartSizingFrame::IControlOverrides::OnTapped")) {
        return S_OK;
    }

    return StartSizingFrameOnTapped_Original(pThis, args);
}

using SearchBoxSetObject_t = long(WINAPI*)(void* pThis, void* value);
using SearchBoxGetObject_t = long(WINAPI*)(void* pThis, void** value);
static SearchBoxSetObject_t SearchBoxSetIsChecked_Original;
static SearchBoxGetObject_t SearchBoxGetCommand_Original;
constexpr bool kEnableRiskyStartCommandHook = true;

static long WINAPI SearchBoxSetIsChecked_Hook(void* pThis, void* value) {
    if (!IsRedirectCategoryEnabled(
            RedirectCategory::StartMenuSearchBoxClick)) {
        return SearchBoxSetIsChecked_Original(pThis, value);
    }

    bool activationLikely = IsStartSearchBoxPointerActivationLikely();
    log_if(L"StartDocked::SearchBoxToggleButton::set_IsChecked activation=%d",
           activationLikely ? 1 : 0);

    if (activationLikely) {
        MarkStartSearchBoxPointerActivation();
        if (RequestReplacement(
                L"StartDocked::SearchBoxToggleButton::set_IsChecked")) {
            return S_OK;
        }
    }

    return SearchBoxSetIsChecked_Original(pThis, value);
}

// Risky fallback: returns a null ICommand to suppress the native search
// invocation when an activation is in progress. Kept gated by the activation
// heuristic to limit blast radius, but should be re-evaluated if the
// pointer/tap hooks cover all real-world paths.
static long WINAPI SearchBoxGetCommand_Hook(void* pThis, void** value) {
    if (!kEnableRiskyStartCommandHook ||
        !IsRedirectCategoryEnabled(
            RedirectCategory::StartMenuSearchBoxClick)) {
        return SearchBoxGetCommand_Original(pThis, value);
    }

    bool activationLikely = IsStartSearchBoxPointerActivationLikely();
    if (activationLikely) {
        log_if(L"StartDocked::SearchBoxToggleButton::get_Command blocked during activation");
        MarkStartSearchBoxPointerActivation();
        if (RequestReplacement(
                L"StartDocked::SearchBoxToggleButton::get_Command")) {
            if (value) {
                *value = nullptr;
            }
            return S_OK;
        }
    }

    return SearchBoxGetCommand_Original(pThis, value);
}

using OnTaskbarSearchTapped_t = void(WINAPI*)(void* pThis, void* sender,
                                              void* args);
static OnTaskbarSearchTapped_t OnTaskbarSearchTapped_Original;

static void WINAPI OnTaskbarSearchTapped_Hook(void* pThis, void* sender,
                                              void* args) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::TaskbarSearch,
            L"TaskbarSearchPageViewModel::OnTaskbarSearchTapped")) {
        OnTaskbarSearchTapped_Original(pThis, sender, args);
    }
}

using LaunchToSearch_t = void(WINAPI*)(void* pThis, void* appShownEventArgs,
                                       void* initialQuery);
static LaunchToSearch_t LaunchToSearch_Original;

static void WINAPI LaunchToSearch_Hook(void* pThis, void* appShownEventArgs,
                                       void* initialQuery) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::TaskbarSearch,
            L"TaskbarSearchPageViewModel::LaunchToSearch")) {
        LaunchToSearch_Original(pThis, appShownEventArgs, initialQuery);
    }
}

using NotifyNavigationToFindInStartRequested_t = void(WINAPI*)(void* pThis);
static NotifyNavigationToFindInStartRequested_t
    NotifyNavigationToFindInStartRequested_Original;

static void WINAPI NotifyNavigationToFindInStartRequested_Hook(void* pThis) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::TaskbarSearch,
            L"TaskbarSearchPageViewModel::NotifyNavigationToFindInStartRequested")) {
        NotifyNavigationToFindInStartRequested_Original(pThis);
    }
}

using SearchButtonOnTapped_t = long(WINAPI*)(void* pThis, void* args);
static SearchButtonOnTapped_t SearchButtonControlOnTapped_Original;
static SearchButtonOnTapped_t SearchV2ButtonOnTapped_Original;

static long WINAPI SearchButtonControlOnTapped_Hook(void* pThis, void* args) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::TaskbarSearch,
            L"SearchButtonControl::OnTapped")) {
        return SearchButtonControlOnTapped_Original(pThis, args);
    }
    return S_OK;
}

static long WINAPI SearchV2ButtonOnTapped_Hook(void* pThis, void* args) {
    if (!RequestReplacementIfCategoryEnabled(
            RedirectCategory::TaskbarSearch,
            L"SearchV2Button::OnTapped")) {
        return SearchV2ButtonOnTapped_Original(pThis, args);
    }
    return S_OK;
}

using UndockedActivateSearch_t = void(WINAPI*)(void* pThis,
                                               void* activatedEventArgs,
                                               bool unknown);
static UndockedActivateSearch_t UndockedActivateSearch_Original;
static UndockedActivateSearch_t UndockedActivate_Original;

using UndockedShowWithStart_t = long(WINAPI*)(void* pThis,
                                              unsigned char value);
static UndockedShowWithStart_t UndockedShowWithStart_Original;

static bool HandleUndockedSearchActivation(PCWSTR hookName) {
    if (!IsRedirectCategoryEnabled(RedirectCategory::UndockedSearch)) {
        return false;
    }

    if (IsUndockedSearchIgnored()) {
        log_if(L"%s suppressed as Start/taskbar-open side effect", hookName);
        return true;
    }

    return RequestReplacement(hookName);
}

static void WINAPI UndockedActivateSearch_Hook(void* pThis,
                                               void* activatedEventArgs,
                                               bool unknown) {
    if (!HandleUndockedSearchActivation(
            L"UndockedSearchAppExperienceManager::ActivateSearch")) {
        UndockedActivateSearch_Original(pThis, activatedEventArgs, unknown);
    }
}

static void WINAPI UndockedActivate_Hook(void* pThis,
                                         void* activatedEventArgs,
                                         bool unknown) {
    if (!HandleUndockedSearchActivation(
            L"UndockedSearchAppExperienceManager::Activate")) {
        UndockedActivate_Original(pThis, activatedEventArgs, unknown);
    }
}

static long WINAPI UndockedShowWithStart_Hook(void* pThis,
                                              unsigned char value) {
    if (!HandleUndockedSearchActivation(
            L"UndockedSearchAppExperienceManager::ShowWithStart")) {
        return UndockedShowWithStart_Original(pThis, value);
    }
    return S_OK;
}

using UndockedNoArg_t = void(WINAPI*)(void* pThis);
static UndockedNoArg_t UndockedShowSearchRequest_Original;
static UndockedNoArg_t UndockedOnShowSearchRequest_Original;
static UndockedNoArg_t StartDockedSearchAppShowSearchRequest_Original;

static void WINAPI UndockedShowSearchRequest_Hook(void* pThis) {
    if (!HandleUndockedSearchActivation(
            L"UndockedSearchAppExperienceManager::ShowSearchRequest")) {
        UndockedShowSearchRequest_Original(pThis);
    }
}

static void WINAPI UndockedOnShowSearchRequest_Hook(void* pThis) {
    if (!HandleUndockedSearchActivation(
            L"UndockedSearchAppExperienceManager::OnShowSearchRequest")) {
        UndockedOnShowSearchRequest_Original(pThis);
    }
}

static void WINAPI StartDockedSearchAppShowSearchRequest_Hook(void* pThis) {
    if (!HandleUndockedSearchActivation(
            L"StartDocked::SearchAppExperienceManager::ShowSearchRequest")) {
        StartDockedSearchAppShowSearchRequest_Original(pThis);
    }
}

using UndockedSearchBoxExecute_t = long(WINAPI*)(void* pThis, void* args);
static UndockedSearchBoxExecute_t UndockedSearchBoxExecute_Original;

static long WINAPI UndockedSearchBoxExecute_Hook(void* pThis, void* args) {
    if (!HandleUndockedSearchActivation(L"UndockedSearchBox::Execute")) {
        return UndockedSearchBoxExecute_Original(pThis, args);
    }
    return S_OK;
}

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_Original;

static bool IsSearchProtocol(PCWSTR file) {
    return file &&
           (StrStrIW(file, L"ms-search:") == file ||
            StrStrIW(file, L"search-ms:") == file ||
            StrStrIW(file, L"ms-searchassistant:") == file);
}

static std::wstring ExtractSearchProtocolQuery(PCWSTR file) {
    if (!file) {
        return L"";
    }

    const PCWSTR names[] = {L"query=", L"q=", L"searchTerms="};
    PCWSTR start = nullptr;
    size_t nameLength = 0;

    for (PCWSTR name : names) {
        PCWSTR found = StrStrIW(file, name);
        if (found && (!start || found < start)) {
            start = found;
            nameLength = wcslen(name);
        }
    }

    if (!start) {
        return L"";
    }

    start += nameLength;
    std::wstring value;
    while (*start && *start != L'&' && *start != L'#') {
        value.push_back(*start == L'+' ? L' ' : *start);
        start++;
    }

    if (value.empty()) {
        return value;
    }

    UrlUnescapeW(value.data(), nullptr, nullptr, URL_UNESCAPE_INPLACE);
    value.resize(wcslen(value.c_str()));

    return SanitizeSearchText(value);
}

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* execInfo) {
    if (execInfo && IsSearchProtocol(execInfo->lpFile)) {
        if (!IsRedirectCategoryEnabled(RedirectCategory::SearchProtocol)) {
            return ShellExecuteExW_Original(execInfo);
        }

        log_if(L"Search protocol intercepted");
        if (!CanRedirectSearch(L"Search protocol")) {
            return ShellExecuteExW_Original(execInfo);
        }

        std::wstring query = ExtractSearchProtocolQuery(execInfo->lpFile);
        if (!query.empty()) {
            const SettingsSnapshot settings = GetSettingsSnapshot();
            AppendPendingText(query, settings.transitionCaptureMs);
        }
        if (!LaunchReplacement()) {
            ClearPendingText();
            return ShellExecuteExW_Original(execInfo);
        }

        execInfo->hInstApp = reinterpret_cast<HINSTANCE>(33);
        if (execInfo->fMask & SEE_MASK_NOCLOSEPROCESS) {
            execInfo->hProcess = nullptr;
        }

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    return ShellExecuteExW_Original(execInfo);
}

// -------------------- Symbol hook setup --------------------

static bool HookStartDockedSymbols() {
    HMODULE module = GetModuleHandleW(L"StartDocked.dll");
    if (!module) {
        Wh_Log(L"[ReplaceSearch] StartDocked.dll is not loaded");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::OpenSearchView(void))"},
            &OpenSearchView_Original,
            OpenSearchView_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::StartSearchRequest::Execute(class StartDocked::LauncherUIPresenterWithDataModel *))"},
            &StartSearchRequestExecute_Original,
            StartSearchRequestExecute_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::UpdateSearchBoxOffsetAndRequestMoveFocusToSearch(void))"},
            &UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Original,
            UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl StartDocked::LauncherFrame::[StartDocked::ILauncherFrameKeyboardNavigationServiceCallbacks]::MoveFocusToSearch(void))"},
            &LauncherFrameMoveFocusToSearch_Original,
            LauncherFrameMoveFocusToSearch_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl StartDocked::NavigationPaneView::[StartDocked::__INavigationPaneViewPublicNonVirtuals]::MoveFocusToSearch(void))"},
            &NavigationPaneMoveFocusToSearch_Original,
            NavigationPaneMoveFocusToSearch_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::OnTryToMoveFocusToSearchEvent(struct StartDocked::BaseEventPayload ^))"},
            &OnTryToMoveFocusToSearchEvent_Original,
            OnTryToMoveFocusToSearchEvent_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::OnShellHandwritingTryToOpenSearchViewEvent(struct StartDocked::BaseEventPayload ^))"},
            &OnShellHandwritingTryToOpenSearchViewEvent_Original,
            OnShellHandwritingTryToOpenSearchViewEvent_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::OnFakeSearchBoxClicked(class Platform::Object ^,class Windows::UI::Xaml::RoutedEventHandler ^))"},
            &OnFakeSearchBoxClicked_Original,
            OnFakeSearchBoxClicked_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::LauncherFrame::OnAllRoutedPointerPressedEvents(class Platform::Object ^,class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &LauncherFrameOnAllRoutedPointerPressedEvents_Original,
            LauncherFrameOnAllRoutedPointerPressedEvents_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::NavigationPaneView::OnAllRoutedPointerPressedEvents(class Platform::Object ^,class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &NavigationPaneOnAllRoutedPointerPressedEvents_Original,
            NavigationPaneOnAllRoutedPointerPressedEvents_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::SearchBoxToggleButton::OnPointerEntered(class Platform::Object ^,class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &SearchBoxOnPointerEntered_Original,
            SearchBoxOnPointerEntered_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl StartDocked::SearchBoxToggleButton::OnPointerExited(class Platform::Object ^,class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &SearchBoxOnPointerExited_Original,
            SearchBoxOnPointerExited_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::SearchBoxToggleButton::[Windows::UI::Xaml::Controls::Primitives::IToggleButton]::__abi_Windows_UI_Xaml_Controls_Primitives_IToggleButton____abi_set_IsChecked(struct Platform::IBox<bool> ^))"},
            &SearchBoxSetIsChecked_Original,
            SearchBoxSetIsChecked_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::SearchBoxToggleButton::[Windows::UI::Xaml::Controls::Primitives::IButtonBase]::__abi_Windows_UI_Xaml_Controls_Primitives_IButtonBase____abi_get_Command(struct Windows::UI::Xaml::Input::ICommand ^ *))"},
            &SearchBoxGetCommand_Original,
            SearchBoxGetCommand_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::LauncherFrame::[Windows::UI::Xaml::Controls::IControlOverrides]::__abi_Windows_UI_Xaml_Controls_IControlOverrides____abi_OnPointerPressed(class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &LauncherFrameOnPointerPressed_Original,
            LauncherFrameOnPointerPressed_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::LauncherFrame::[Windows::UI::Xaml::Controls::IControlOverrides]::__abi_Windows_UI_Xaml_Controls_IControlOverrides____abi_OnTapped(class Windows::UI::Xaml::Input::TappedRoutedEventArgs ^))"},
            &LauncherFrameOnTapped_Original,
            LauncherFrameOnTapped_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::StartSizingFrame::[Windows::UI::Xaml::Controls::IControlOverrides]::__abi_Windows_UI_Xaml_Controls_IControlOverrides____abi_OnPointerPressed(class Windows::UI::Xaml::Input::PointerRoutedEventArgs ^))"},
            &StartSizingFrameOnPointerPressed_Original,
            StartSizingFrameOnPointerPressed_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl StartDocked::StartSizingFrame::[Windows::UI::Xaml::Controls::IControlOverrides]::__abi_Windows_UI_Xaml_Controls_IControlOverrides____abi_OnTapped(class Windows::UI::Xaml::Input::TappedRoutedEventArgs ^))"},
            &StartSizingFrameOnTapped_Original,
            StartSizingFrameOnTapped_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return false;
    }

    bool anyInstalled =
        OpenSearchView_Original ||
        StartSearchRequestExecute_Original ||
        UpdateSearchBoxOffsetAndRequestMoveFocusToSearch_Original ||
        LauncherFrameMoveFocusToSearch_Original ||
        NavigationPaneMoveFocusToSearch_Original ||
        OnTryToMoveFocusToSearchEvent_Original ||
        OnShellHandwritingTryToOpenSearchViewEvent_Original ||
        OnFakeSearchBoxClicked_Original ||
        LauncherFrameOnAllRoutedPointerPressedEvents_Original ||
        NavigationPaneOnAllRoutedPointerPressedEvents_Original ||
        SearchBoxOnPointerEntered_Original ||
        SearchBoxOnPointerExited_Original ||
        SearchBoxSetIsChecked_Original ||
        SearchBoxGetCommand_Original ||
        LauncherFrameOnPointerPressed_Original ||
        LauncherFrameOnTapped_Original ||
        StartSizingFrameOnPointerPressed_Original ||
        StartSizingFrameOnTapped_Original;
    if (!anyInstalled) {
        Wh_Log(L"[ReplaceSearch] StartDocked.dll: no symbols were resolved");
    }
    return anyInstalled;
}

static bool HookSearchUxSymbols() {
    HMODULE module = GetModuleHandleW(L"SearchUx.UI.dll");
    if (!module) {
        Wh_Log(L"[ReplaceSearch] SearchUx.UI.dll is not loaded");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl Cortana::UI::ViewModels::TaskbarSearchPageViewModel::OnTaskbarSearchTapped(class Platform::Object ^,class Platform::Object ^))"},
            &OnTaskbarSearchTapped_Original,
            OnTaskbarSearchTapped_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl Cortana::UI::ViewModels::TaskbarSearchPageViewModel::[Cortana::UI::ViewModels::ITaskbarSearchPageViewModel]::LaunchToSearch(class Cortana::UI::ViewModels::AppShownEventArgs ^,class Platform::String ^))"},
            &LaunchToSearch_Original,
            LaunchToSearch_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl Cortana::UI::ViewModels::TaskbarSearchPageViewModel::[Cortana::UI::ViewModels::ITaskbarSearchPageViewModel]::NotifyNavigationToFindInStartRequested(void))"},
            &NotifyNavigationToFindInStartRequested_Original,
            NotifyNavigationToFindInStartRequested_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SearchUx::SearchUI::implementation::SearchButtonControl,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnTapped(void *))"},
            &SearchButtonControlOnTapped_Original,
            SearchButtonControlOnTapped_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SearchUx::SearchUI::implementation::SearchV2Button,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnTapped(void *))"},
            &SearchV2ButtonOnTapped_Original,
            SearchV2ButtonOnTapped_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return false;
    }

    bool anyInstalled =
        OnTaskbarSearchTapped_Original ||
        LaunchToSearch_Original ||
        NotifyNavigationToFindInStartRequested_Original ||
        SearchButtonControlOnTapped_Original ||
        SearchV2ButtonOnTapped_Original;
    if (!anyInstalled) {
        Wh_Log(L"[ReplaceSearch] SearchUx.UI.dll: no symbols were resolved");
    }
    return anyInstalled;
}

static bool HookTwinuiSymbols() {
    HMODULE module = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!module) {
        Wh_Log(L"[ReplaceSearch] twinui.pcshell.dll is not loaded");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl UndockedSearchAppExperienceManager::ActivateSearch(struct winrt::WindowsUdk::UI::Shell::SearchAppActivatedEventArgs const &,bool))"},
            &UndockedActivateSearch_Original,
            UndockedActivateSearch_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl UndockedSearchAppExperienceManager::Activate(struct winrt::WindowsUdk::UI::Shell::SearchAppActivatedEventArgs const &,bool))"},
            &UndockedActivate_Original,
            UndockedActivate_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl UndockedSearchAppExperienceManager::ShowWithStart(unsigned char))"},
            &UndockedShowWithStart_Original,
            UndockedShowWithStart_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl UndockedSearchAppExperienceManager::ShowSearchRequest(void))"},
            &UndockedShowSearchRequest_Original,
            UndockedShowSearchRequest_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl UndockedSearchAppExperienceManager::OnShowSearchRequest(void))"},
            &UndockedOnShowSearchRequest_Original,
            UndockedOnShowSearchRequest_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl StartDocked::SearchAppExperienceManager::ShowSearchRequest(void))"},
            &StartDockedSearchAppShowSearchRequest_Original,
            StartDockedSearchAppShowSearchRequest_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl UndockedSearchBox::Execute(void *))"},
            &UndockedSearchBoxExecute_Original,
            UndockedSearchBoxExecute_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return false;
    }

    bool anyInstalled =
        UndockedActivateSearch_Original ||
        UndockedActivate_Original ||
        UndockedShowWithStart_Original ||
        UndockedShowSearchRequest_Original ||
        UndockedOnShowSearchRequest_Original ||
        StartDockedSearchAppShowSearchRequest_Original ||
        UndockedSearchBoxExecute_Original;
    if (!anyInstalled) {
        Wh_Log(L"[ReplaceSearch] twinui.pcshell.dll: no symbols were resolved");
    }
    return anyInstalled;
}

static bool TryHookStartDockedSymbols() {
    if (g_startDockedSymbolsHooked.load()) {
        return true;
    }

    HMODULE module = GetModuleHandleW(L"StartDocked.dll");
    if (!module) {
        return false;
    }

    if (HookStartDockedSymbols()) {
        g_startDockedSymbolsHooked.store(true);
        log_if(L"Hooked StartDocked.dll after load");
        return true;
    }

    return false;
}

static bool TryHookTwinuiSymbols() {
    if (g_twinuiSymbolsHooked.load()) {
        return true;
    }

    HMODULE module = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!module) {
        return false;
    }

    if (HookTwinuiSymbols()) {
        g_twinuiSymbolsHooked.store(true);
        log_if(L"Hooked twinui.pcshell.dll after load");
        return true;
    }

    return false;
}

static bool TryHookSearchUxSymbols() {
    if (g_searchUxSymbolsHooked.load()) {
        return true;
    }

    HMODULE module = GetModuleHandleW(L"SearchUx.UI.dll");
    if (!module) {
        return false;
    }

    if (HookSearchUxSymbols()) {
        g_searchUxSymbolsHooked.store(true);
        log_if(L"Hooked SearchUx.UI.dll after load");
        return true;
    }

    return false;
}

static DWORD WINAPI ModuleWatcherThreadProc(LPVOID) {
    while (WaitForSingleObject(g_moduleWatcherStopEvent, 200) == WAIT_TIMEOUT) {
        bool done = true;

        switch (g_targetProcess) {
            case TargetProcess::Explorer:
                done = TryHookSearchUxSymbols() && TryHookTwinuiSymbols();
                break;

            case TargetProcess::SearchHost:
                done = TryHookSearchUxSymbols();
                break;

            case TargetProcess::StartMenu:
                done = TryHookStartDockedSymbols();
                break;

            default:
                return 0;
        }

        if (done) {
            return 0;
        }
    }

    return 0;
}

static void StartModuleWatcherThread() {
    if (g_moduleWatcherThread || g_targetProcess == TargetProcess::Unknown) {
        return;
    }

    g_moduleWatcherStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_moduleWatcherStopEvent) {
        Wh_Log(L"[ReplaceSearch] CreateEventW for module watcher failed: %u",
               GetLastError());
        return;
    }

    g_moduleWatcherThread =
        CreateThread(nullptr, 0, ModuleWatcherThreadProc, nullptr, 0, nullptr);
    if (!g_moduleWatcherThread) {
        Wh_Log(L"[ReplaceSearch] CreateThread for module watcher failed: %u",
               GetLastError());
        CloseHandle(g_moduleWatcherStopEvent);
        g_moduleWatcherStopEvent = nullptr;
    }
}

static void StopModuleWatcherThread() {
    if (!g_moduleWatcherThread) {
        return;
    }

    SetEvent(g_moduleWatcherStopEvent);
    WaitForSingleObject(g_moduleWatcherThread, INFINITE);
    CloseHandle(g_moduleWatcherThread);
    g_moduleWatcherThread = nullptr;

    CloseHandle(g_moduleWatcherStopEvent);
    g_moduleWatcherStopEvent = nullptr;
}

static void HookSearchProtocolFallback() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        return;
    }

    auto target = reinterpret_cast<ShellExecuteExW_t>(GetProcAddress(shell32, "ShellExecuteExW"));
    if (!target) {
        return;
    }

    WindhawkUtils::SetFunctionHook(target, ShellExecuteExW_Hook, &ShellExecuteExW_Original);
}

// -------------------- Windhawk entry points --------------------

BOOL Wh_ModInit() {
    InterlockedExchange(&g_unloading, 0);

    g_targetProcess = DetectTargetProcess();
    LoadSettings();
    if (!InitLaunchThreadTracking()) {
        Wh_Log(L"[ReplaceSearch] Launch tracking unavailable, disabling mod");
        return FALSE;
    }
    if (!InitSharedState()) {
        Wh_Log(L"[ReplaceSearch] Shared state unavailable, disabling mod");
        WaitForLaunchThreadsAndCloseTracking();
        return FALSE;
    }

    bool anyHooked = false;

    switch (g_targetProcess) {
        case TargetProcess::Explorer:
            StartKeyboardHookThread();
            anyHooked = TryHookSearchUxSymbols();
            anyHooked = TryHookTwinuiSymbols() || anyHooked;
            HookSearchProtocolFallback();
            if (!g_searchUxSymbolsHooked.load() ||
                !g_twinuiSymbolsHooked.load()) {
                StartModuleWatcherThread();
            }
            break;

        case TargetProcess::StartMenu:
            anyHooked = TryHookStartDockedSymbols();
            if (!g_startDockedSymbolsHooked.load()) {
                StartModuleWatcherThread();
            }
            break;

        case TargetProcess::SearchHost:
            anyHooked = TryHookSearchUxSymbols();
            if (!g_searchUxSymbolsHooked.load()) {
                StartModuleWatcherThread();
            }
            break;

        default:
            break;
    }

    log_if(L"Initialized in %s (hooks=%d)", GetCurrentProcessName().c_str(),
           anyHooked ? 1 : 0);
    return TRUE;
}

void Wh_ModUninit() {
    AcquireSRWLockExclusive(&g_launchTrackingLock);
    InterlockedExchange(&g_unloading, 1);
    ReleaseSRWLockExclusive(&g_launchTrackingLock);

    if (g_targetProcess == TargetProcess::Explorer) {
        StopKeyboardHookThread();
    }

    StopModuleWatcherThread();
    WaitForLaunchThreadsAndCloseTracking();
    UninitSharedState();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
