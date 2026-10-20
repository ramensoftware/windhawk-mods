// ==WindhawkMod==
// @id              win2000-hung-app-dialog
// @name            Windows 2000 End Program Dialog
// @description     Replaces the Windows 11 "not responding" dialog with the End Program dialog of Windows 2000, in English or Russian
// @name:ru         Окно «Завершение программы» из Windows 2000
// @description:ru  Заменяет окно Windows 11 о зависшей программе окном «Завершение программы» из Windows 2000 - на русском или английском
// @version         1.6.0
// @author          appEW
// @github          https://github.com/appEW
// @include         windhawk.exe
// @include         WerFault.exe
// @compilerOptions -ldwmapi -ladvapi32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 2000 End Program Dialog

Replaces the Windows Error Reporting dialog shown when you try to close a
program that is not responding. It does not change the Windows shutdown screen.

![Windows 2000 End Program dialog in Russian](https://raw.githubusercontent.com/appEW/windhawk-mods/6beb29187942eb1da43a4350405d2b2d62a79b14/media/win2000-end-program-ru.png)

Example with Russian Windows resources and a classic theme. Frame colours
follow your current theme. / Пример с русскими ресурсами Windows и классической
темой. Цвета рамки зависят от текущей темы.

The replacement uses the original `DIALOG #10` and messages from the installed
`winsrv.dll.mui`, with the Windows 2000 warning overlay on the application's
icon. The layout, font and wording come from Windows, not a recreated dialog.
The display language is selected automatically, with English as a fallback;
the language setting can also request English or Russian. That language must
be installed in Windows. The window frame follows your current Windows theme.

**Tested on Windows 11 24H2 (build 26100).** Other Windows versions are not
verified. If the resources or WER window cannot be validated, the mod leaves
the original dialog available.

## Behavior and safety

The original WER dialog stays alive. End Now forwards WER's Close button;
Cancel, Escape and the title-bar close button forward WER's Wait button.
Windows performs these actions; the mod never terminates the hung process
directly. End Now can still lose unsaved data, just as the original button can.
After Cancel, a later close attempt can show the replacement again even when
WER reuses the same native dialog. An acknowledged Wait is not timed out.

A broker thread owns the replacement windows, accessibility notifications and
fallback timers. Public ShowWindow/SetWindowPos hooks try to cloak the original
dialog before display. No TaskDialog callback is replaced, and no DLL is pinned.
Disabling or updating the mod joins the worker and restores any original
dialog that is still alive before the DLL can unload.

Windhawk can inject into WER after its window is already visible. In that case
a brief appearance of the modern dialog is possible.

## Optional pre-warming

**Keep Windows Error Reporting service running** is off by default. Enabling
it runs the pre-warm worker in a dedicated Windhawk tool host, not in Explorer,
WerFault, the hung application or the main Windhawk UI process. The same mod
handles the dialog in WerFault and hosts pre-warming separately; only this one
mod needs to be installed. The host is bundled with Windhawk (1.7.3 or later),
so no extra helper download is needed.

The worker reads WerSvc's existing custom start-trigger GUID from SCM and
writes its ETW event immediately, then once per minute. Disabling the option
stops and joins the worker and ends only its dedicated host. No service startup
type, registry policy, system file or event-log setting is changed; WerSvc is
never forcibly stopped. Windows decides when the shared service can idle out.

This opt-in feature keeps the service and a tool host in memory. The event
descriptor matches WER's private trigger manifest, not a public WER contract,
and may break after Windows updates. If its event-log channel is enabled, the
heartbeat can generate about 1,440 entries per day; this mod never enables it.
Errors are retried only at the ordinary minute interval. Pre-warming can reduce
late injection but does not guarantee flash-free behavior on every system.

## По-русски

Заменяет окно Windows о зависшей программе, которое появляется при попытке
её закрыть. Экран завершения работы Windows мод не меняет.

Используется оригинальное окно `DIALOG #10` и тексты из установленного
`winsrv.dll.mui`, со знаком предупреждения Windows 2000 поверх значка программы.
Расположение, шрифт и формулировки берутся из системных ресурсов. Язык
выбирается автоматически с английским в качестве резервного. В настройках можно
запросить русский или английский; этот язык должен быть установлен в Windows.
Оформление рамки зависит от текущей темы Windows.

«Завершить сейчас» нажимает штатную кнопку закрытия WER. «Отмена», Escape
и крестик означают ожидание отклика. Мод сам не завершает зависший процесс,
но при выборе завершения несохранённые данные могут потеряться.
После «Отмена» повторная попытка закрытия снова показывает замену, даже если
WER использует то же штатное окно. Принятое ожидание не отменяется по таймауту.
При отключении мода живое штатное окно возвращается, а DLL выгружается.

Опция **«Поддерживать службу отчётов об ошибках Windows активной»** по умолчанию
выключена. Прогрев выполняется в отдельном tool-host Windhawk, а не в Проводнике,
WerFault, зависшем приложении или основном процессе интерфейса Windhawk.
Устанавливается только один мод; EXE-хост уже входит в Windhawk 1.7.3 или новее.

Мод читает существующий GUID триггера WerSvc из SCM и посылает ETW-событие сразу,
затем раз в минуту. Отключение опции останавливает поток и только собственный
tool-host. Тип запуска службы, политики, системные файлы и настройки журнала не
меняются; WerSvc не останавливается принудительно. Прогрев оставляет службу и
хост в памяти. Дескриптор события соответствует частному манифесту WER и может
измениться после обновления Windows. Если включён канал провайдера, возможно
около 1 440 записей в сутки; мод сам канал не включает. Ошибки повторяются не
чаще раза в минуту. Опция может уменьшить вспышку при позднем внедрении, но не
гарантирует её отсутствие на любой системе.

**Проверено на Windows 11 24H2 (сборка 26100).** Другие версии не проверены.
При позднем внедрении возможно краткое появление современного окна.
Дополнительные файлы или загрузки для мода не нужны.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language of the dialog
  $name:ru: Язык окна
  $description: >-
    The dialog is taken from the winsrv.dll.mui of this language. Automatic
    uses the Windows display language, and English when that one has no
    such file.
  $description:ru: >-
    Окно берётся из winsrv.dll.mui этого языка. «Автоматически» - язык
    интерфейса Windows, а если для него такого файла нет - английский.
  $options:
  - auto: Automatic
  - en-US: English
  - ru-RU: Russian
  $options:ru:
  - auto: Автоматически
  - en-US: Английский
  - ru-RU: Русский
- keepWerSvcRunning: false
  $name: Keep Windows Error Reporting service running
  $name:ru: Поддерживать службу отчётов об ошибках Windows активной
  $description: >-
    Opt in to permanent pre-warming in a separate Windhawk tool host (one
    ETW trigger per minute). Keeps the service and host in memory; can
    reduce the modern-dialog flash, but does not guarantee its absence.
  $description:ru: >-
    Постоянный прогрев в отдельном tool-host Windhawk (одно ETW-событие
    в минуту). Оставляет службу и хост в памяти; может уменьшить вспышку
    современного окна, но не гарантирует её отсутствие.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <evntprov.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <windows.h>

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr UINT kDialogResourceId = 10;
constexpr int kStatusControlId = 257;
constexpr int kEndNowButtonId = 259;
constexpr int kIconControlId = 0x7FFE;
constexpr UINT_PTR kActionTimerId = 1;
constexpr UINT_PTR kDiscoveryTimerId = 2;
constexpr UINT_PTR kPendingCloakTimerId = 3;
constexpr UINT kActionPollMs = 250;
constexpr UINT kDiscoveryPollMs = 100;
constexpr DWORD kBrokerStopFallbackMs = 1000;
constexpr UINT kEarlyCloakFallbackMs = 1500;
constexpr ULONGLONG kDiscoveryLifetimeMs = 10000;
constexpr ULONGLONG kActionTimeoutMs = 15000;
constexpr DWORD kTextTimeoutMs = 100;
constexpr int kWerCloseButtonId = 10;
constexpr int kWerWaitButtonId = 8;

constexpr UINT kBrokerCandidateMessage = WM_APP + 1;
constexpr UINT kBrokerStockDestroyedMessage = WM_APP + 2;
constexpr UINT kBrokerFinalizeMessage = WM_APP + 3;
constexpr UINT kBrokerPrepareMessage = WM_APP + 4;
constexpr UINT kBrokerStockHiddenMessage = WM_APP + 5;

constexpr wchar_t kBrokerWindowClass[] = L"Win2000HungAppDialogBrokerWindow";
constexpr wchar_t kReleasedDialogProperty[] =
    L"Win2000HungAppDialog.ReleasedDialog";
constexpr wchar_t kOwnedStockProperty[] = L"Win2000HungAppDialog.OwnedStock";
constexpr wchar_t kClassicDialogProperty[] =
    L"Win2000HungAppDialog.ClassicDialog";
// winsrv.dll.mui keeps the Windows 2000 texts of this dialog in its message
// table. They are read from the same MUI file as the dialog, so the status
// line always matches the language of the dialog itself; the English texts
// below are only used if a message cannot be read.
constexpr DWORD kNotRespondingMessageId = 19;
constexpr DWORD kEndingMessageId = 20;
constexpr wchar_t kDefaultNotResponding[] = L"This program is not responding.";
constexpr wchar_t kDefaultEnding[] = L"Ending Program...Please wait";
constexpr wchar_t kDefaultAppName[] = L"Program";
constexpr wchar_t kRussianAppName[] = L"Программа";

// The original 16x16 four-bit Windows 2000 warning overlay. The bytes are
// the RT_ICON payload, so the mod stays a single self-contained source file.
constexpr BYTE kWin2000WarningIcon16[] = {
    0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00,
    0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88,
    0x3B, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x08, 0x3B, 0xBB, 0xBB, 0x80,
    0x8B, 0xBB, 0xBB, 0x08, 0x3B, 0xBB, 0xBB, 0x80, 0x8B, 0xBB, 0xBB, 0x00,
    0x03, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xB0, 0x80, 0x03, 0xBB, 0xBB, 0xB0,
    0xBB, 0xBB, 0xB0, 0x00, 0x00, 0x3B, 0xBB, 0x30, 0x3B, 0xBB, 0x08, 0x00,
    0x00, 0x3B, 0xBB, 0x00, 0x0B, 0xBB, 0x00, 0x00, 0x00, 0x03, 0xBB, 0x00,
    0x0B, 0xB0, 0x80, 0x00, 0x00, 0x03, 0xBB, 0x00, 0x0B, 0xB0, 0x00, 0x00,
    0x00, 0x00, 0x3B, 0xB0, 0xBB, 0x08, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xBB,
    0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xBB, 0xB0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xBB, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
    0x30, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x80, 0x01, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00,
    0xC0, 0x07, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00,
    0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x1F, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00,
    0xF8, 0x3F, 0x00, 0x00, 0xFC, 0x7F, 0x00, 0x00,
};

struct PendingCloak {
    DWORD threadId = 0;
    HWND ghostOwner = nullptr;
    HWND hungWindow = nullptr;
    ULONGLONG deadline = 0;
};

struct Session {
    HWND stockDialog = nullptr;
    HWND classicDialog = nullptr;
    HWND ghostOwner = nullptr;
    HWND hungWindow = nullptr;
    DWORD stockProcessId = 0;
    DWORD stockThreadId = 0;
    std::wstring applicationName;
    std::wstring caption;
    HICON applicationIcon = nullptr;
    HICON warningIcon = nullptr;
    bool stockWasVisible = false;
    bool stockCloakedByUs = false;
    bool stockHiddenByUs = false;
    bool selectionSent = false;
    int selectionButtonId = 0;
    DWORD selectionTick = 0;
    bool waitAcknowledged = false;
    bool closing = false;
    bool initialized = false;
    ULONGLONG actionDeadline = 0;

    ~Session() {
        if (warningIcon) {
            DestroyIcon(warningIcon);
        }
        if (applicationIcon) {
            DestroyIcon(applicationIcon);
        }
    }
};

HMODULE g_resourceModule = nullptr;
std::wstring g_notResponding = kDefaultNotResponding;
std::wstring g_ending = kDefaultEnding;
std::wstring g_fallbackAppName = kDefaultAppName;
HMODULE g_selfModule = nullptr;
decltype(&ShowWindow) g_showWindowOriginal = nullptr;
decltype(&ShowWindowAsync) g_showWindowAsyncOriginal = nullptr;
decltype(&SetWindowPos) g_setWindowPosOriginal = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_brokerThread = nullptr;
std::atomic<HWND> g_brokerWindow{nullptr};
std::atomic<bool> g_active{false};

// The following state is accessed only on the broker thread.
HWINEVENTHOOK g_winEventHook = nullptr;
std::map<HWND, std::unique_ptr<Session>> g_sessions;
std::map<HWND, PendingCloak> g_pendingCloaks;
HINSTANCE g_windowClassInstance = nullptr;
bool g_windowClassRegistered = false;
ULONGLONG g_discoveryDeadline = 0;

void Trim(std::wstring& value) {
    constexpr wchar_t kWhitespace[] = L" \t\r\n";
    const size_t first = value.find_first_not_of(kWhitespace);
    if (first == std::wstring::npos) {
        value.clear();
        return;
    }
    const size_t last = value.find_last_not_of(kWhitespace);
    value = value.substr(first, last - first + 1);
}

std::wstring ProcessPath(DWORD processId) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }

    wchar_t path[32768] = {};
    DWORD length = ARRAYSIZE(path);
    const BOOL ok = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    return ok ? std::wstring(path, length) : std::wstring{};
}

std::wstring JoinPath(const wchar_t* directory, const wchar_t* fileName) {
    std::wstring path = directory;
    if (!path.empty() && path.back() != L'\\') {
        path += L'\\';
    }
    path += fileName;
    return path;
}

bool EqualPath(const std::wstring& left, const std::wstring& right) {
    return !left.empty() && !right.empty() &&
           _wcsicmp(left.c_str(), right.c_str()) == 0;
}

bool IsSystemWerProcess(DWORD processId) {
    const std::wstring processPath = ProcessPath(processId);
    if (processPath.empty()) {
        return false;
    }

    wchar_t systemDirectory[MAX_PATH] = {};
    if (GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory))) {
        if (EqualPath(processPath,
                      JoinPath(systemDirectory, L"WerFault.exe"))) {
            return true;
        }
    }

    wchar_t wow64Directory[MAX_PATH] = {};
    if (GetSystemWow64DirectoryW(wow64Directory, ARRAYSIZE(wow64Directory))) {
        if (EqualPath(processPath, JoinPath(wow64Directory, L"WerFault.exe"))) {
            return true;
        }
    }

    return false;
}

struct WerTreeSearchContext {
    int visibleButtonCount = 0;
    bool hasDirectUiWindow = false;
};

BOOL CALLBACK InspectWerTreeCallback(HWND window, LPARAM parameter) {
    auto* context = reinterpret_cast<WerTreeSearchContext*>(parameter);
    wchar_t className[64] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"DirectUIHWND") == 0) {
        context->hasDirectUiWindow = true;
    } else if (_wcsicmp(className, L"Button") == 0 && IsWindowVisible(window)) {
        ++context->visibleButtonCount;
    }
    return TRUE;
}

bool HasInitialWerConsentTree(HWND taskDialog) {
    WerTreeSearchContext context;
    EnumChildWindows(taskDialog, InspectWerTreeCallback,
                     reinterpret_cast<LPARAM>(&context));
    return context.hasDirectUiWindow && context.visibleButtonCount == 2;
}

using HungWindowFromGhostWindow_t = HWND(WINAPI*)(HWND);

HWND ResolveHungWindow(HWND ghostWindow) {
    static const auto function =
        reinterpret_cast<HungWindowFromGhostWindow_t>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "HungWindowFromGhostWindow"));
    return function ? function(ghostWindow) : nullptr;
}

bool IsPotentialWerHangDialog(HWND window) {
    if (!window || !IsWindow(window) ||
        GetAncestor(window, GA_ROOT) != window ||
        GetPropW(window, kClassicDialogProperty)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"#32770") != 0) {
        return false;
    }

    const HWND owner = GetWindow(window, GW_OWNER);
    wchar_t ownerClass[64] = {};
    return owner && GetClassNameW(owner, ownerClass, ARRAYSIZE(ownerClass)) &&
           wcscmp(ownerClass, L"Ghost") == 0 &&
           ResolveHungWindow(owner) != nullptr;
}

bool IsOwnedStockWindow(HWND window, DWORD expectedThreadId) {
    DWORD processId = 0;
    wchar_t className[64] = {};
    return window && IsWindow(window) &&
           GetPropW(window, kOwnedStockProperty) ==
               reinterpret_cast<HANDLE>(g_selfModule) &&
           GetWindowThreadProcessId(window, &processId) == expectedThreadId &&
           processId == GetCurrentProcessId() &&
           GetClassNameW(window, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"#32770") == 0;
}

bool IsSamePendingWindow(HWND window, const PendingCloak& pending) {
    return IsOwnedStockWindow(window, pending.threadId) &&
           IsPotentialWerHangDialog(window) &&
           GetWindow(window, GW_OWNER) == pending.ghostOwner &&
           ResolveHungWindow(pending.ghostOwner) == pending.hungWindow;
}

void ReleasePendingCloak(HWND window, bool suppressRetry) {
    const auto iterator = g_pendingCloaks.find(window);
    if (iterator == g_pendingCloaks.end()) {
        return;
    }
    const PendingCloak pending = iterator->second;
    g_pendingCloaks.erase(iterator);
    // The ghost may disappear when the app recovers. That must not prevent
    // rollback of our cloak on a still-live, identity-marked stock window.
    if (IsOwnedStockWindow(window, pending.threadId)) {
        if (suppressRetry) {
            SetPropW(window, kReleasedDialogProperty,
                     reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(1)));
        }
        BOOL cloak = FALSE;
        DwmSetWindowAttribute(window, DWMWA_CLOAK, &cloak, sizeof(cloak));
        RemovePropW(window, kOwnedStockProperty);
    }
    if (g_pendingCloaks.empty()) {
        KillTimer(g_brokerWindow.load(), kPendingCloakTimerId);
    }
}

bool PrepareStockDialog(HWND window) {
    // This function and the pending map are confined to the broker thread.
    // No callback or timer is installed on a WER-owned window.
    if (!g_active.load() || GetPropW(window, kReleasedDialogProperty) ||
        !IsPotentialWerHangDialog(window)) {
        return false;
    }
    if (g_sessions.contains(window) || g_pendingCloaks.contains(window)) {
        return true;
    }

    DWORD cloaked = 0;
    if (FAILED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked,
                                     sizeof(cloaked))) ||
        cloaked) {
        return false;  // Do not take ownership of another component's cloak.
    }

    try {
        PendingCloak pending;
        pending.threadId = GetWindowThreadProcessId(window, nullptr);
        pending.ghostOwner = GetWindow(window, GW_OWNER);
        pending.hungWindow = ResolveHungWindow(pending.ghostOwner);
        pending.deadline = GetTickCount64() + kEarlyCloakFallbackMs;
        g_pendingCloaks.emplace(window, pending);
    } catch (...) {
        return false;
    }

    const HWND broker = g_brokerWindow.load();
    BOOL cloak = TRUE;
    if (!SetTimer(broker, kPendingCloakTimerId, kDiscoveryPollMs, nullptr) ||
        !SetPropW(window, kOwnedStockProperty,
                  reinterpret_cast<HANDLE>(g_selfModule)) ||
        FAILED(DwmSetWindowAttribute(window, DWMWA_CLOAK, &cloak,
                                     sizeof(cloak)))) {
        ReleasePendingCloak(window, false);
        return false;
    }
    return true;
}

void PrepareToShowDialog(HWND window) {
    const HWND broker = g_brokerWindow.load();
    if (!g_active.load() || !broker ||
        GetCurrentThreadId() == GetWindowThreadProcessId(broker, nullptr) ||
        !IsPotentialWerHangDialog(window)) {
        return;
    }
    DWORD_PTR ignored = 0;
    // Bound the delay to the WER UI thread. A timed-out request is still owned
    // by the broker and will be rolled back by its timer or during unload.
    SendMessageTimeoutW(
        broker, kBrokerPrepareMessage, reinterpret_cast<WPARAM>(window), 0,
        SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT, 250, &ignored);
}

BOOL WINAPI ShowWindowHook(HWND window, int command) {
    if (command != SW_HIDE) {
        PrepareToShowDialog(window);
    }
    return g_showWindowOriginal(window, command);
}

BOOL WINAPI ShowWindowAsyncHook(HWND window, int command) {
    if (command != SW_HIDE) {
        PrepareToShowDialog(window);
    }
    return g_showWindowAsyncOriginal(window, command);
}

BOOL WINAPI SetWindowPosHook(HWND window,
                             HWND insertAfter,
                             int x,
                             int y,
                             int width,
                             int height,
                             UINT flags) {
    // TaskDialog's internal display path can bypass exported ShowWindow.
    // Its initial positioning occurs before display, without SWP_SHOWWINDOW.
    // Ignore broker-owned windows above so we never cloak our own DIALOG #10.
    if (!(flags & SWP_HIDEWINDOW)) {
        PrepareToShowDialog(window);
    }
    return g_setWindowPosOriginal(window, insertAfter, x, y, width, height,
                                  flags);
}

std::wstring GetApplicationName(HWND hungWindow) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hungWindow, &processId);

    // For a top-level window in another process, GetWindowText reads the
    // cached caption instead of sending WM_GETTEXT to its hung UI thread.
    wchar_t caption[2048] = {};
    if (processId != GetCurrentProcessId() &&
        GetWindowTextW(hungWindow, caption, ARRAYSIZE(caption)) > 0) {
        return caption;
    }

    const std::wstring path = ProcessPath(processId);
    const size_t separator = path.find_last_of(L"\\/");
    const std::wstring name =
        separator == std::wstring::npos ? path : path.substr(separator + 1);
    return name.empty() ? g_fallbackAppName : name;
}

HICON CopyWindowIcon(HWND window) {
    if (!window || !IsWindow(window)) {
        return nullptr;
    }

    DWORD_PTR result = 0;
    constexpr WPARAM kIconTypes[] = {ICON_BIG, ICON_SMALL2, ICON_SMALL};
    for (WPARAM iconType : kIconTypes) {
        if (SendMessageTimeoutW(window, WM_GETICON, iconType, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, kTextTimeoutMs,
                                &result) &&
            result) {
            return CopyIcon(reinterpret_cast<HICON>(result));
        }
    }

    HICON icon = reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICON));
    if (!icon) {
        icon = reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICONSM));
    }
    return icon ? CopyIcon(icon) : nullptr;
}

HICON GetApplicationIcon(HWND hungWindow, HWND taskDialog) {
    if (HICON icon = CopyWindowIcon(hungWindow)) {
        return icon;
    }
    if (HICON icon = CopyWindowIcon(taskDialog)) {
        return icon;
    }
    HICON shared = LoadIconW(nullptr, IDI_APPLICATION);
    return shared ? CopyIcon(shared) : nullptr;
}

HICON LoadWindows2000WarningIcon() {
    return CreateIconFromResourceEx(const_cast<PBYTE>(kWin2000WarningIcon16),
                                    sizeof(kWin2000WarningIcon16), TRUE,
                                    0x00030000, 16, 16, LR_DEFAULTCOLOR);
}

void CenterDialog(HWND dialog, HWND referenceWindow) {
    RECT dialogRect = {};
    if (!GetWindowRect(dialog, &dialogRect)) {
        return;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromWindow(
        referenceWindow ? referenceWindow : dialog, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    const int width = dialogRect.right - dialogRect.left;
    const int height = dialogRect.bottom - dialogRect.top;
    const RECT& work = monitorInfo.rcWork;
    const int x = work.left + ((work.right - work.left) - width) / 2;
    const int y = work.top + ((work.bottom - work.top) - height) / 2;
    SetWindowPos(dialog, HWND_TOP, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

void AddApplicationIcon(HWND dialog, Session* session) {
    if (!session->applicationIcon) {
        return;
    }

    // DIALOG #10 deliberately has no icon in its caption. Windows 2000 drew
    // the hung application's 32x32 icon in the body and overlaid a 16x16
    // warning badge in the lower-left corner.
    RECT iconOrigin = {8, 8, 8, 8};
    if (!MapDialogRect(dialog, &iconOrigin)) {
        return;
    }

    UINT dpi = GetDpiForWindow(dialog);
    if (!dpi) {
        dpi = USER_DEFAULT_SCREEN_DPI;
    }
    int iconWidth = GetSystemMetricsForDpi(SM_CXICON, dpi);
    int iconHeight = GetSystemMetricsForDpi(SM_CYICON, dpi);
    if (iconWidth <= 0) {
        iconWidth = MulDiv(32, dpi, USER_DEFAULT_SCREEN_DPI);
    }
    if (iconHeight <= 0) {
        iconHeight = MulDiv(32, dpi, USER_DEFAULT_SCREEN_DPI);
    }

    CreateWindowExW(
        0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        iconOrigin.left, iconOrigin.top, iconWidth, iconHeight, dialog,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIconControlId)), nullptr,
        nullptr);
}

bool IsSameStockWindow(const Session* session) {
    if (!session ||
        !IsOwnedStockWindow(session->stockDialog, session->stockThreadId)) {
        return false;
    }

    DWORD processId = 0;
    const DWORD threadId =
        GetWindowThreadProcessId(session->stockDialog, &processId);
    if (processId != session->stockProcessId ||
        threadId != session->stockThreadId) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(session->stockDialog, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"#32770") != 0 ||
        GetWindow(session->stockDialog, GW_OWNER) != session->ghostOwner) {
        return false;
    }

    return IsWindow(session->ghostOwner) &&
           ResolveHungWindow(session->ghostOwner) == session->hungWindow;
}

bool HideStockDialog(Session* session) {
    if (!session || !IsSameStockWindow(session)) {
        return false;
    }

    session->stockWasVisible =
        session->stockWasVisible || IsWindowVisible(session->stockDialog);
    BOOL cloak = TRUE;
    if (SUCCEEDED(DwmSetWindowAttribute(session->stockDialog, DWMWA_CLOAK,
                                        &cloak, sizeof(cloak)))) {
        session->stockCloakedByUs = true;
        return true;
    }

    if (session->stockWasVisible &&
        ShowWindowAsync(session->stockDialog, SW_HIDE)) {
        session->stockHiddenByUs = true;
        return true;
    }
    return false;
}

void RestoreStockDialog(Session* session) {
    if (!session ||
        !IsOwnedStockWindow(session->stockDialog, session->stockThreadId)) {
        return;
    }

    if (session->stockCloakedByUs) {
        BOOL cloak = FALSE;
        DwmSetWindowAttribute(session->stockDialog, DWMWA_CLOAK, &cloak,
                              sizeof(cloak));
        session->stockCloakedByUs = false;
    }
    if (session->stockHiddenByUs && session->stockWasVisible) {
        ShowWindowAsync(session->stockDialog, SW_SHOW);
        session->stockHiddenByUs = false;
    }
    SetForegroundWindow(session->stockDialog);
    RemovePropW(session->stockDialog, kOwnedStockProperty);
}

void RequestFinalize(Session* session, bool restoreStock) {
    const HWND broker = g_brokerWindow.load();
    if (session && broker) {
        PostMessageW(broker, kBrokerFinalizeMessage,
                     reinterpret_cast<WPARAM>(session->stockDialog),
                     restoreStock);
    }
}

void AcknowledgeWait(Session* session) {
    session->waitAcknowledged = true;
    session->actionDeadline = 0;
    // WER can retain the same hidden dialog after Wait. Its hide acknowledges
    // the action; do not time it out or suppress replacement of a later SHOW.
    KillTimer(session->classicDialog, kActionTimerId);
}

void SendSelection(Session* session, int buttonId, bool hideClassicDialog) {
    if (!session || session->selectionSent) {
        return;
    }
    if (!IsSameStockWindow(session)) {
        // Recovery can remove the ghost mapping before WER closes its dialog.
        // Do not leave a replacement with inert buttons and a cloaked stock UI.
        RequestFinalize(session, true);
        return;
    }

    session->selectionSent = true;
    session->selectionButtonId = buttonId;
    session->selectionTick = GetTickCount();
    session->waitAcknowledged = false;
    EnableWindow(GetDlgItem(session->classicDialog, kEndNowButtonId), FALSE);
    EnableWindow(GetDlgItem(session->classicDialog, IDCANCEL), FALSE);

    if (hideClassicDialog) {
        ShowWindow(session->classicDialog, SW_HIDE);
    } else {
        SetDlgItemTextW(session->classicDialog, kStatusControlId,
                        g_ending.c_str());
        UpdateWindow(session->classicDialog);
    }

    session->actionDeadline = GetTickCount64() + kActionTimeoutMs;
    if (!SetTimer(session->classicDialog, kActionTimerId, kActionPollMs,
                  nullptr)) {
        Wh_Log(L"Failed to create the WER action timer, GLE=%u",
               GetLastError());
        session->selectionSent = false;
        session->selectionButtonId = 0;
        EnableWindow(GetDlgItem(session->classicDialog, kEndNowButtonId), TRUE);
        EnableWindow(GetDlgItem(session->classicDialog, IDCANCEL), TRUE);
        SetDlgItemTextW(session->classicDialog, kStatusControlId,
                        g_notResponding.c_str());
        ShowWindow(session->classicDialog, SW_SHOW);
        SetForegroundWindow(session->classicDialog);
        return;
    }

    if (!PostMessageW(session->stockDialog, TDM_CLICK_BUTTON, buttonId, 0)) {
        Wh_Log(L"TDM_CLICK_BUTTON failed for hwnd=%p, GLE=%u",
               session->stockDialog, GetLastError());
        RequestFinalize(session, true);
    }
}

INT_PTR CALLBACK ClassicDialogProc(HWND dialog,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    auto* session =
        reinterpret_cast<Session*>(GetWindowLongPtrW(dialog, DWLP_USER));

    switch (message) {
        case WM_INITDIALOG:
            session = reinterpret_cast<Session*>(lParam);
            SetWindowLongPtrW(dialog, DWLP_USER,
                              reinterpret_cast<LONG_PTR>(session));
            if (!session) {
                return FALSE;
            }
            SetPropW(dialog, kClassicDialogProperty,
                     reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(1)));
            {
                BOOL cloak = FALSE;
                DwmSetWindowAttribute(dialog, DWMWA_CLOAK, &cloak,
                                      sizeof(cloak));
            }
            session->classicDialog = dialog;
            {
                // The caption of DIALOG #10 is the "End Program - " prefix
                // in the language of the MUI file it was loaded from.
                wchar_t prefix[256] = {};
                GetWindowTextW(dialog, prefix, ARRAYSIZE(prefix));
                try {
                    session->caption = prefix;
                    session->caption += session->applicationName;
                } catch (...) {
                    return FALSE;
                }
            }
            SetWindowTextW(dialog, session->caption.c_str());
            SetDlgItemTextW(dialog, kStatusControlId, g_notResponding.c_str());
            AddApplicationIcon(dialog, session);
            CenterDialog(dialog, session->ghostOwner);
            SendMessageW(dialog, DM_SETDEFID, IDCANCEL, 0);
            SetFocus(GetDlgItem(dialog, IDCANCEL));
            session->initialized = true;
            return FALSE;

        case WM_COMMAND:
            if (!session) {
                break;
            }
            if (LOWORD(wParam) == kEndNowButtonId) {
                SendSelection(session, kWerCloseButtonId, false);
                return TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                SendSelection(session, kWerWaitButtonId, true);
                return TRUE;
            }
            break;

        case WM_DRAWITEM:
            if (session && wParam == kIconControlId && lParam) {
                auto* draw = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
                if (draw->CtlType != ODT_STATIC) {
                    break;
                }
                FillRect(draw->hDC, &draw->rcItem,
                         GetSysColorBrush(COLOR_3DFACE));
                const int iconWidth = draw->rcItem.right - draw->rcItem.left;
                const int iconHeight = draw->rcItem.bottom - draw->rcItem.top;
                if (session->applicationIcon) {
                    DrawIconEx(draw->hDC, draw->rcItem.left, draw->rcItem.top,
                               session->applicationIcon, iconWidth, iconHeight,
                               0, nullptr, DI_NORMAL);
                }
                if (session->warningIcon) {
                    UINT dpi = GetDpiForWindow(dialog);
                    if (!dpi) {
                        dpi = USER_DEFAULT_SCREEN_DPI;
                    }
                    int badgeWidth = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
                    int badgeHeight = GetSystemMetricsForDpi(SM_CYSMICON, dpi);
                    if (badgeWidth <= 0) {
                        badgeWidth = MulDiv(16, dpi, USER_DEFAULT_SCREEN_DPI);
                    }
                    if (badgeHeight <= 0) {
                        badgeHeight = MulDiv(16, dpi, USER_DEFAULT_SCREEN_DPI);
                    }
                    badgeWidth =
                        badgeWidth < iconWidth ? badgeWidth : iconWidth;
                    badgeHeight =
                        badgeHeight < iconHeight ? badgeHeight : iconHeight;
                    DrawIconEx(draw->hDC, draw->rcItem.left,
                               draw->rcItem.bottom - badgeHeight,
                               session->warningIcon, badgeWidth, badgeHeight, 0,
                               nullptr, DI_NORMAL);
                }
                return TRUE;
            }
            break;

        case WM_CLOSE:
            if (session) {
                SendSelection(session, kWerWaitButtonId, true);
            }
            return TRUE;

        case WM_TIMER:
            if (wParam == kActionTimerId && session && session->selectionSent) {
                if (!IsSameStockWindow(session)) {
                    KillTimer(dialog, kActionTimerId);
                    RequestFinalize(session, true);
                    return TRUE;
                }

                if (session->waitAcknowledged) {
                    return TRUE;  // A queued timer can outlive KillTimer.
                }
                if (session->selectionButtonId == kWerWaitButtonId &&
                    !IsWindowVisible(session->stockDialog)) {
                    AcknowledgeWait(session);
                    return TRUE;
                }

                if (GetTickCount64() >= session->actionDeadline) {
                    KillTimer(dialog, kActionTimerId);
                    Wh_Log(
                        L"WER action timed out for hwnd=%p; restoring stock UI",
                        session->stockDialog);
                    RequestFinalize(session, true);
                    return TRUE;
                }
            }
            break;

        case WM_NCDESTROY:
            if (session) {
                RemovePropW(dialog, kClassicDialogProperty);
                session->classicDialog = nullptr;
                if (!session->closing) {
                    RequestFinalize(session, !session->selectionSent);
                }
            }
            break;
    }

    return FALSE;
}

void CloseSession(HWND stockDialog, bool restoreStock) {
    const auto iterator = g_sessions.find(stockDialog);
    if (iterator == g_sessions.end()) {
        return;
    }

    std::unique_ptr<Session> session = std::move(iterator->second);
    g_sessions.erase(iterator);
    session->closing = true;
    if (restoreStock) {
        // Do not re-hide a fail-open dialog on its next SHOW notification.
        if (IsOwnedStockWindow(session->stockDialog, session->stockThreadId)) {
            SetPropW(session->stockDialog, kReleasedDialogProperty,
                     reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(1)));
        }
    }
    // Even an unexpected replacement-window destruction must not orphan a
    // live stock dialog. Destroyed/reused HWNDs fail the ownership check.
    RestoreStockDialog(session.get());
    if (session->classicDialog && IsWindow(session->classicDialog)) {
        KillTimer(session->classicDialog, kActionTimerId);
        DestroyWindow(session->classicDialog);
    }
}

bool IsWerHangTaskDialog(HWND window,
                         DWORD* processId,
                         HWND* ghostOwner,
                         HWND* hungWindow) {
    if (!IsPotentialWerHangDialog(window) || !IsWindowVisible(window) ||
        !HasInitialWerConsentTree(window)) {
        return false;
    }

    *processId = GetCurrentProcessId();
    *ghostOwner = GetWindow(window, GW_OWNER);
    *hungWindow = ResolveHungWindow(*ghostOwner);
    return *hungWindow && IsWindow(*hungWindow);
}

void StartSessionImpl(HWND stockDialog) {
    if (!g_active.load() || GetPropW(stockDialog, kReleasedDialogProperty)) {
        return;
    }
    const auto existingSession = g_sessions.find(stockDialog);
    if (existingSession != g_sessions.end()) {
        Session* session = existingSession->second.get();
        if (!IsSameStockWindow(session) || !HideStockDialog(session)) {
            CloseSession(stockDialog, true);
            return;
        }
        if (session->selectionSent && session->waitAcknowledged &&
            session->selectionButtonId == kWerWaitButtonId &&
            IsWindowVisible(stockDialog)) {
            if (!IsWindow(session->classicDialog) ||
                !HasInitialWerConsentTree(stockDialog)) {
                CloseSession(stockDialog, true);  // Unexpected page: fail open.
                return;
            }
            KillTimer(session->classicDialog, kActionTimerId);
            session->selectionSent = false;
            session->selectionButtonId = 0;
            session->waitAcknowledged = false;
            session->actionDeadline = 0;
            EnableWindow(GetDlgItem(session->classicDialog, kEndNowButtonId), TRUE);
            EnableWindow(GetDlgItem(session->classicDialog, IDCANCEL), TRUE);
            SetDlgItemTextW(session->classicDialog, kStatusControlId,
                            g_notResponding.c_str());
            CenterDialog(session->classicDialog, session->ghostOwner);
            ShowWindow(session->classicDialog, SW_SHOW);
            SetForegroundWindow(session->classicDialog);
            SetActiveWindow(session->classicDialog);
        }
        return;
    }

    DWORD processId = 0;
    HWND ghostOwner = nullptr;
    HWND hungWindow = nullptr;
    if (!IsWerHangTaskDialog(stockDialog, &processId, &ghostOwner,
                             &hungWindow)) {
        return;
    }
    if (!g_pendingCloaks.contains(stockDialog)) {
        DWORD cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(stockDialog, DWMWA_CLOAKED, &cloaked,
                                         sizeof(cloaked))) ||
            cloaked) {
            return;
        }
    }

    auto session = std::make_unique<Session>();
    session->stockDialog = stockDialog;
    session->stockProcessId = processId;
    session->stockThreadId = GetWindowThreadProcessId(stockDialog, nullptr);
    session->ghostOwner = ghostOwner;
    session->hungWindow = hungWindow;
    session->applicationName = GetApplicationName(hungWindow);
    session->applicationIcon = GetApplicationIcon(hungWindow, stockDialog);
    session->warningIcon = LoadWindows2000WarningIcon();

    Session* sessionPointer = session.get();
    g_sessions.emplace(stockDialog, std::move(session));
    if (!SetPropW(stockDialog, kOwnedStockProperty,
                  reinterpret_cast<HANDLE>(g_selfModule))) {
        CloseSession(stockDialog, true);
        ReleasePendingCloak(stockDialog, true);
        return;
    }
    const auto pending = g_pendingCloaks.find(stockDialog);
    if (pending != g_pendingCloaks.end()) {
        sessionPointer->stockCloakedByUs =
            IsSamePendingWindow(stockDialog, pending->second);
        g_pendingCloaks.erase(pending);
        if (g_pendingCloaks.empty()) {
            KillTimer(g_brokerWindow.load(), kPendingCloakTimerId);
        }
    }

    HWND classicDialog = CreateDialogParamW(
        g_resourceModule, MAKEINTRESOURCEW(kDialogResourceId),
        sessionPointer->ghostOwner, ClassicDialogProc,
        reinterpret_cast<LPARAM>(sessionPointer));
    if (!classicDialog || !sessionPointer->initialized) {
        Wh_Log(L"CreateDialogParamW(DIALOG #10) failed, GLE=%u",
               GetLastError());
        CloseSession(stockDialog, true);
        return;
    }

    if (!HideStockDialog(sessionPointer)) {
        CloseSession(stockDialog, true);
        return;
    }
    ShowWindow(classicDialog, SW_SHOW);
    SetForegroundWindow(classicDialog);
    SetActiveWindow(classicDialog);
    Wh_Log(L"Attached to WER hwnd=%p pid=%u app=%s", stockDialog, processId,
           sessionPointer->applicationName.c_str());
}

void StartSession(HWND stockDialog) {
    try {
        StartSessionImpl(stockDialog);
    } catch (...) {
        Wh_Log(L"WER replacement failed; restoring stock UI");
        CloseSession(stockDialog, true);
        ReleasePendingCloak(stockDialog, true);
    }
}

BOOL CALLBACK EnumerateExistingWindowsCallback(HWND window, LPARAM) {
    StartSession(window);
    return TRUE;
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND window,
                           LONG objectId,
                           LONG childId,
                           DWORD,
                           DWORD eventTime) {
    if (!g_active.load() || !window || objectId != OBJID_WINDOW ||
        childId != CHILDID_SELF) {
        return;
    }

    const HWND broker = g_brokerWindow.load();
    if (!broker) {
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
        // OUTOFCONTEXT dispatches on this broker's message-loop thread.
        const HWND rootWindow = GetAncestor(window, GA_ROOT);
        PostMessageW(broker, kBrokerCandidateMessage,
                     reinterpret_cast<WPARAM>(rootWindow ? rootWindow : window),
                     0);
    } else if (event == EVENT_OBJECT_HIDE) {
        PostMessageW(broker, kBrokerStockHiddenMessage,
                     reinterpret_cast<WPARAM>(window), eventTime);
    } else if (event == EVENT_OBJECT_DESTROY) {
        PostMessageW(broker, kBrokerStockDestroyedMessage,
                     reinterpret_cast<WPARAM>(window), 0);
    }
}

LRESULT CALLBACK BrokerWindowProc(HWND window,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            if (wParam == kDiscoveryTimerId) {
                EnumWindows(EnumerateExistingWindowsCallback, 0);
                if (!g_sessions.empty() ||
                    GetTickCount64() >= g_discoveryDeadline) {
                    KillTimer(window, kDiscoveryTimerId);
                }
                return 0;
            }
            if (wParam == kPendingCloakTimerId) {
                for (auto iterator = g_pendingCloaks.begin();
                     iterator != g_pendingCloaks.end();) {
                    const HWND stockDialog = iterator->first;
                    const PendingCloak pending = iterator->second;
                    ++iterator;
                    if (!IsSamePendingWindow(stockDialog, pending)) {
                        ReleasePendingCloak(stockDialog, false);
                    } else if (GetTickCount64() >= pending.deadline) {
                        ReleasePendingCloak(stockDialog, true);
                    } else {
                        StartSession(stockDialog);
                    }
                }
                return 0;
            }
            break;

        case kBrokerPrepareMessage:
            return PrepareStockDialog(reinterpret_cast<HWND>(wParam));

        case kBrokerCandidateMessage:
            StartSession(reinterpret_cast<HWND>(wParam));
            return 0;

        case kBrokerStockDestroyedMessage:
            CloseSession(reinterpret_cast<HWND>(wParam), false);
            ReleasePendingCloak(reinterpret_cast<HWND>(wParam), false);
            return 0;

        case kBrokerStockHiddenMessage: {
            const auto iterator = g_sessions.find(reinterpret_cast<HWND>(wParam));
            if (iterator != g_sessions.end()) {
                Session* session = iterator->second.get();
                // Ignore delayed hides from before this action. DWORD tick
                // subtraction is valid across rollover within the 15s bound.
                if (session->selectionSent && !session->waitAcknowledged &&
                    session->selectionButtonId == kWerWaitButtonId &&
                    static_cast<LONG>(static_cast<DWORD>(lParam) -
                                      session->selectionTick) >= 0 &&
                    IsSameStockWindow(session)) {
                    AcknowledgeWait(session);
                    // A rapid SHOW may precede delivery of this HIDE event.
                    if (IsWindowVisible(session->stockDialog)) {
                        StartSession(session->stockDialog);
                    }
                }
            }
            return 0;
        }

        case kBrokerFinalizeMessage:
            CloseSession(reinterpret_cast<HWND>(wParam), lParam != 0);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RouteDialogMessage(MSG* message) {
    for (const auto& [stockDialog, session] : g_sessions) {
        (void)stockDialog;
        const HWND dialog = session->classicDialog;
        if (dialog && IsWindow(dialog) &&
            (message->hwnd == dialog || IsChild(dialog, message->hwnd)) &&
            IsDialogMessageW(dialog, message)) {
            return true;
        }
    }
    return false;
}

void CloseAllSessions(bool restoreStock) {
    while (!g_sessions.empty()) {
        CloseSession(g_sessions.begin()->first, restoreStock);
    }
}

BOOL CALLBACK RemoveReleasedProperty(HWND window, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == GetCurrentProcessId()) {
        RemovePropW(window, kReleasedDialogProperty);
    }
    return TRUE;
}

DWORD WINAPI BrokerThreadProc(void*) {
    g_windowClassInstance = g_selfModule;
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = BrokerWindowProc;
    windowClass.hInstance = g_windowClassInstance;
    windowClass.lpszClassName = kBrokerWindowClass;
    if (!RegisterClassExW(&windowClass)) {
        SetEvent(g_readyEvent);
        return 1;
    }
    g_windowClassRegistered = true;

    HWND broker =
        CreateWindowExW(0, kBrokerWindowClass, nullptr, 0, 0, 0, 0, 0,
                        HWND_MESSAGE, nullptr, g_windowClassInstance, nullptr);
    if (!broker) {
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
        SetEvent(g_readyEvent);
        return 2;
    }
    g_brokerWindow.store(broker);

    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr, WinEventProc,
        GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook) {
        g_brokerWindow.store(nullptr);
        DestroyWindow(broker);
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
        SetEvent(g_readyEvent);
        return 3;
    }

    SetEvent(g_readyEvent);

    // Installing the hook before enumeration closes the show/enumerate race.
    EnumWindows(EnumerateExistingWindowsCallback, 0);
    g_discoveryDeadline = GetTickCount64() + kDiscoveryLifetimeMs;
    SetTimer(broker, kDiscoveryTimerId, kDiscoveryPollMs, nullptr);

    bool stop = false;
    while (!stop && WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        // Check stop independently of the input queue, including immediately
        // after startup. A bounded message wait is a fallback for a missed
        // queue wake during quick reload; it does not permit early DLL unload.
        const DWORD waitResult = MsgWaitForMultipleObjectsEx(
            1, &g_stopEvent, kBrokerStopFallbackMs, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_TIMEOUT) {
            continue;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            break;
        }

        MSG message;
        while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT &&
               PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                stop = true;
                break;
            }
            if (RouteDialogMessage(&message)) {
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    UnhookWinEvent(g_winEventHook);
    g_winEventHook = nullptr;
    KillTimer(broker, kDiscoveryTimerId);
    KillTimer(broker, kPendingCloakTimerId);
    CloseAllSessions(true);
    while (!g_pendingCloaks.empty()) {
        ReleasePendingCloak(g_pendingCloaks.begin()->first, false);
    }
    EnumWindows(RemoveReleasedProperty, 0);

    g_brokerWindow.store(nullptr);
    DestroyWindow(broker);
    if (g_windowClassRegistered) {
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
    }
    return 0;
}

bool GetCurrentModModule() {
    const auto address = reinterpret_cast<LPCWSTR>(
        reinterpret_cast<ULONG_PTR>(&GetCurrentModModule));
    return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              address, &g_selfModule) != FALSE;
}

bool InstallEarlyShowHooks() {
    return WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindowHook,
                                          &g_showWindowOriginal) &&
           WindhawkUtils::SetFunctionHook(ShowWindowAsync, ShowWindowAsyncHook,
                                          &g_showWindowAsyncOriginal) &&
           WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPosHook,
                                          &g_setWindowPosOriginal);
}

std::wstring LoadResourceMessage(DWORD messageId) {
    wchar_t buffer[512] = {};
    const DWORD length = FormatMessageW(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_MAX_WIDTH_MASK,
        g_resourceModule, messageId, 0, buffer, ARRAYSIZE(buffer), nullptr);
    std::wstring message(buffer, length);
    Trim(message);
    return message;
}

// The languages to take the dialog from, in order: the one chosen in the
// settings, else the Windows display language, then English.
std::vector<std::wstring> DialogLanguages() {
    std::vector<std::wstring> languages;

    const auto setting = WindhawkUtils::StringSetting::make(L"language");
    if (*setting.get() && wcscmp(setting, L"auto") != 0) {
        languages.emplace_back(setting.get());
    }

    wchar_t userLanguage[LOCALE_NAME_MAX_LENGTH] = {};
    if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                         userLanguage, ARRAYSIZE(userLanguage), 0)) {
        languages.emplace_back(userLanguage);
    }

    languages.emplace_back(L"en-US");
    return languages;
}

bool LoadWindows2000DialogResource() {
    wchar_t systemDirectory[MAX_PATH] = {};
    const UINT length =
        GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (!length || length >= ARRAYSIZE(systemDirectory)) {
        return false;
    }

    for (const std::wstring& language : DialogLanguages()) {
        const std::wstring path =
            JoinPath(systemDirectory, (language + L"\\winsrv.dll.mui").c_str());
        HMODULE module = LoadLibraryExW(
            path.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (!module) {
            Wh_Log(L"LoadLibraryExW(%s) failed, GLE=%u", path.c_str(),
                   GetLastError());
            continue;
        }
        if (!FindResourceW(module, MAKEINTRESOURCEW(kDialogResourceId),
                           RT_DIALOG)) {
            Wh_Log(L"DIALOG #10 is missing in %s", path.c_str());
            FreeLibrary(module);
            continue;
        }

        g_resourceModule = module;
        std::wstring message = LoadResourceMessage(kNotRespondingMessageId);
        g_notResponding = message.empty() ? kDefaultNotResponding : message;
        message = LoadResourceMessage(kEndingMessageId);
        g_ending = message.empty() ? kDefaultEnding : message;
        g_fallbackAppName = _wcsnicmp(language.c_str(), L"ru", 2) == 0
                                ? kRussianAppName
                                : kDefaultAppName;
        Wh_Log(L"Using DIALOG #10 from %s", path.c_str());
        return true;
    }
    return false;
}

void CleanupInitializationObjects() {
    if (g_brokerThread) {
        CloseHandle(g_brokerThread);
        g_brokerThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_resourceModule) {
        FreeLibrary(g_resourceModule);
        g_resourceModule = nullptr;
    }
}

}  // namespace

static BOOL InitializeWerDialog() {
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    if (!IsSystemWerProcess(GetCurrentProcessId()) || !GetCurrentModModule() ||
        !LoadWindows2000DialogResource()) {
        CleanupInitializationObjects();
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) {
        CleanupInitializationObjects();
        return FALSE;
    }

    g_active.store(true);
    g_brokerThread =
        CreateThread(nullptr, 0, BrokerThreadProc, nullptr, 0, nullptr);
    if (!g_brokerThread) {
        g_active.store(false);
        CleanupInitializationObjects();
        return FALSE;
    }

    HANDLE events[] = {g_readyEvent, g_brokerThread};
    const DWORD ready =
        WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 5000);
    if (ready != WAIT_OBJECT_0 || !g_brokerWindow.load() ||
        !InstallEarlyShowHooks()) {
        g_active.store(false);
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_brokerThread, INFINITE);
        CleanupInitializationObjects();
        return FALSE;
    }

    Wh_Log(L"WER broker ready; public pre-display hooks queued");
    return TRUE;
}

static void PrepareWerDialogForUninit() {
    // Stop new cloak requests before Windhawk disables the function hooks.
    g_active.store(false);
}

static void UninitializeWerDialog() {
    Wh_Log(L"Uninit");
    g_active.store(false);

    if (g_brokerThread) {
        SetEvent(g_stopEvent);
        const HWND broker = g_brokerWindow.load();
        if (broker) {
            PostMessageW(broker, WM_NULL, 0, 0);
        }

        // The module must not unload while the broker's WinEvent callback or
        // dialog procedures can still execute. The broker never waits on the
        // calling thread, so this join has no lock inversion.
        WaitForSingleObject(g_brokerThread, INFINITE);
    }

    CleanupInitializationObjects();
}

namespace PrewarmTool {
HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
REGHANDLE provider = 0;

// Some bundled MinGW headers omit these service-trigger layouts.
struct ServiceTrigger {
    DWORD type;
    DWORD action;
    GUID* subtype;
    DWORD dataCount;
    void* dataItems;
};
struct ServiceTriggerInfo {
    DWORD count;
    ServiceTrigger* triggers;
    BYTE* reserved;
};

bool QueryServiceTrigger(GUID* providerId) {
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!manager) return false;
    SC_HANDLE service = OpenServiceW(manager, L"WerSvc", SERVICE_QUERY_CONFIG);
    CloseServiceHandle(manager);
    if (!service) return false;
    bool found = false;
    try {
        DWORD size = 0;
        QueryServiceConfig2W(service, SERVICE_CONFIG_TRIGGER_INFO, nullptr, 0,
                             &size);
        if (size >= sizeof(ServiceTriggerInfo) && size <= 65536) {
            std::vector<BYTE> data(size);
            if (QueryServiceConfig2W(service, SERVICE_CONFIG_TRIGGER_INFO,
                                     data.data(), size, &size)) {
                const auto contains = [&data](const void* pointer, size_t bytes) {
                    const auto base = reinterpret_cast<std::uintptr_t>(data.data());
                    const auto address = reinterpret_cast<std::uintptr_t>(pointer);
                    return address >= base && address - base <= data.size() &&
                           bytes <= data.size() - (address - base);
                };
                const auto* info =
                    reinterpret_cast<const ServiceTriggerInfo*>(data.data());
                if (info->count <= data.size() / sizeof(ServiceTrigger) &&
                    contains(info->triggers, info->count * sizeof(ServiceTrigger))) {
                    for (DWORD i = 0; i < info->count; ++i) {
                        const ServiceTrigger& trigger = info->triggers[i];
                        if (trigger.type == SERVICE_TRIGGER_TYPE_CUSTOM &&
                            trigger.action == 1 && // SERVICE_TRIGGER_ACTION_SERVICE_START
                            trigger.dataCount == 0 &&
                            contains(trigger.subtype, sizeof(GUID))) {
                            *providerId = *trigger.subtype;
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    } catch (...) {
        // Optional pre-warming must fail harmlessly on SCM/allocation failure.
    }
    CloseServiceHandle(service);
    return found;
}

DWORD WINAPI Worker(void*) {
    // Component-private WER manifest contract; see the README caveats.
    EVENT_DESCRIPTOR event{};
    event.Channel = 16;
    event.Level = 4;
    event.Task = 1;
    event.Keyword = 0x8000000000000001ULL;
    ULONG previousError = ERROR_SUCCESS;
    Wh_Log(L"WerSvc heartbeat started (once per minute, dedicated tool host)");
    while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT) {
        const ULONG error = EventWrite(provider, &event, 0, nullptr);
        if (error != ERROR_SUCCESS && error != previousError) {
            Wh_Log(L"WerSvc heartbeat failed: %u; retry in one minute", error);
        }
        previousError = error;
        if (WaitForSingleObject(stopEvent, 60000) != WAIT_TIMEOUT) break;
    }
    return 0;
}

void Cleanup() {
    if (worker) {
        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
        worker = nullptr;
    }
    if (provider) {
        EventUnregister(provider);
        provider = 0;
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
    }
}
} // namespace PrewarmTool

// WhTool_ModInit is also the legacy marker recognized by Windhawk 2.0. That
// engine hosts this role itself but still injects the ordinary role in WerFault.
BOOL WhTool_ModInit() {
    using namespace PrewarmTool;
    if (!Wh_GetIntSetting(L"keepWerSvcRunning"))
        return FALSE;
    GUID providerId{};
    if (!QueryServiceTrigger(&providerId)) {
        Wh_Log(L"WerSvc has no supported start trigger");
        Cleanup();
        return FALSE;
    }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) {
        Cleanup();
        return FALSE;
    }
    const ULONG error = EventRegister(&providerId, nullptr, nullptr, &provider);
    if (error != ERROR_SUCCESS) {
        Wh_Log(L"EventRegister failed: %u", error);
        Cleanup();
        return FALSE;
    }
    worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!worker) {
        Cleanup();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    PrewarmTool::Cleanup();
}

void WhTool_ModSettingsChanged() {
    // The lifecycle dispatcher below handles our opt-in setting.
}

// Separate from the standard tool snippet: the WER role must never enter its
// process-exit or entry-point-hook paths, including failed initialization.
bool g_isWerDialogProcess;

void Wh_ModBeforeUninit() {
    if (g_isWerDialogProcess) {
        PrepareWerDialogForUninit();
        return;
    }
    if (PrewarmTool::stopEvent) {
        SetEvent(PrewarmTool::stopEvent);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    if (IsSystemWerProcess(GetCurrentProcessId())) {
        g_isWerDialogProcess = true;
        return InitializeWerDialog();
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
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
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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

    if (!Wh_GetIntSetting(L"keepWerSvcRunning")) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isWerDialogProcess) {
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
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
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    if (g_isWerDialogProcess) {
        *reload = TRUE;
        return TRUE;
    }

    // Preserve an enabled host on language changes to avoid a mutex reload
    // race. Opt-out still reloads/unloads both the launcher and its own host.
    *reload = !Wh_GetIntSetting(L"keepWerSvcRunning");

    if (g_isToolModProcessLauncher) {
        return TRUE;
    }

    WhTool_ModSettingsChanged();
    return TRUE;
}

void Wh_ModUninit() {
    if (g_isWerDialogProcess) {
        UninitializeWerDialog();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
