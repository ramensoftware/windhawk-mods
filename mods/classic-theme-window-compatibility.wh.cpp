// ==WindhawkMod==
// @id              classic-theme-window-compatibility
// @name            Classic Theme Window Compatibility
// @description     Turns on the Classic theme and fixes the windows that draw their own title bar: the empty strips along their edges and the content that spills past the screen when they are maximized
// @name:ru         Совместимость окон с классической темой
// @description:ru  Включает классическую тему и исправляет окна с собственным заголовком: пустые полосы по краям и содержимое, вылезающее за экран в развёрнутом виде
// @version         1.0
// @author          appEW
// @github          https://github.com/appEW
// @include         *
// @include         winlogon.exe
// @exclude         dwm.exe
// @exclude         csrss.exe
// @exclude         smss.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         fontdrvhost.exe
// @architecture    amd64
// @compilerOptions -lntdll -ladvapi32 -ldwmapi -luxtheme -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Theme Window Compatibility

One mod for the Classic theme on 64-bit Windows 11. It turns the Classic theme
on and fixes the windows that draw their own title bar: the empty strips along
their edges, and the content that spills past the screen when they are
maximized. It replaces:

- Classic Theme (`classic-theme-enable`);
- Classic Theme DWM Compatibility Fix;
- Classic Theme Frameless Window Fix.

Do not enable those mods together with this one.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

## Which windows are fixed

There is no list of programs or window classes in the code. The fix applies to
windows that draw their own client area while keeping `WS_CAPTION` and
`WS_THICKFRAME`, and that use `WS_EX_NOREDIRECTIONBITMAP` with the DWM
`MAINWINDOW` backdrop. The geometry is checked as well: in the normal state there
is no top margin, and there is the same narrow non-client strip on the left, the
right and the bottom. For a window that starts out maximized, the mod checks
that the window matches the work area of its monitor with small symmetric
margins.

This is a global fix for that one mechanism. Other causes of a frame - a frame
drawn inside the application's own content, say - are a different case.

## What is fixed

In `winlogon.exe` the mod turns the Classic theme on through the theme section
and keeps DWM's access to the theme resources. In applications it watches the
original `WCA_NCRENDERING_EXILED` requests and lifts that ban only for the
windows that qualify.

The fix is re-applied on resizing, minimizing and maximizing, and on DPI and
theme changes. What the application asked for and what the mod substituted are
kept apart, so a repeated request while the window is maximized no longer
undoes the fix.

The client area of a qualifying maximized window is limited to the work area of
its monitor after the normal `WM_NCCALCSIZE`. The window's normal size, the
application's logical size, its own hit testing and resize zones are kept. The
usual outer margin of a maximized window may still lie outside the work area;
that alone does not mean anything is visibly spilling over.

For qualifying maximized windows the mod also zeroes the DWM frame extension
the application asked for. The last original request is kept so it can be
restored. If the original margins were never seen, the mod does not guess them
when rolling back.

## Installing

1. Turn off the three mods it replaces, listed above.
2. Install this mod in Windhawk and enable it.
3. Sign out of Windows and sign in again. This recreates the theme section and
   lets the mod see the requests of new windows from the very start.
4. Fully restart the applications you care about if they were running while the
   mod was installed or updated.

Windhawk's global exclusion list takes priority over the mod: a program
excluded there never gets the fix loaded. The mod does not change that list.
Turn off any old `strip`/`darkframe` modes of the No System Frame mod for the
programs this mod fixes; `nopaint` for other programs can stay.

## Checking and rolling back

Check a normal window, several maximize/restore cycles, minimizing and each
monitor. The log records the WCA fix, the clipping of the client overflow and
the handling of DWM margins separately. A successful build and a successful API
call are no substitute for looking at your application.

When the mod is disabled only the requests it actually replaced are restored.
The theme section handle is put back only if nobody else changed it. Full
rollback: disable the mod, enable the previous Classic Theme mod, and sign out
and back in. No code runs inside `dwm.exe`.

---

## По-русски

Единый мод для Windows 11 x64 с классической темой. Заменяет:
- Classic Theme (classic-theme-enable);
- Classic Theme DWM Compatibility Fix;
- Classic Theme Frameless Window Fix.

Не включайте перечисленные моды одновременно с этим.

## Как выбираются окна

В коде нет списка приложений или оконных классов. Исправление применяется к
окнам с собственной клиентской отрисовкой, которые сохраняют WS_CAPTION и
WS_THICKFRAME, используют WS_EX_NOREDIRECTIONBITMAP и фон DWM MAINWINDOW.
Проверяется также геометрия: в обычном состоянии нет верхнего отступа,
а слева, справа и снизу есть одинаковая узкая неклиентская полоса.
Для запуска сразу в развёрнутом состоянии проверяется соответствие окна
рабочей области монитора с небольшими симметричными отступами.

Это глобальное исправление данного механизма. Другие причины рамок
(например, рамка внутри содержимого приложения) не считаются тем же случаем.

## Что исправлено

В winlogon.exe мод включает классическую тему через ThemeSection и сохраняет
доступ DWM к ресурсам темы. В приложениях он наблюдает исходные запросы
WCA_NCRENDERING_EXILED и снимает этот запрет только у подходящих окон.

Исправление повторяется при изменении размеров, сворачивании/разворачивании,
смене DPI и темы. Намерение приложения и факт нашей подмены сохраняются
отдельно: повторный запрос при развёрнутом окне больше не отменяет исправление.

Клиентская область развёрнутого подходящего окна ограничивается рабочей
областью его монитора после штатного WM_NCCALCSIZE. Обычные размеры HWND,
логические размеры приложения, собственные hit-test и зоны resize сохраняются.
Штатный внешний отступ развёрнутого HWND может оставаться за рабочей областью;
это само по себе не означает видимый свес.

Для развёрнутых подходящих окон мод также обнуляет запрошенное приложением
расширение DWM-рамки. Последний исходный запрос сохраняется для восстановления.
Если исходные margins не были замечены, мод не угадывает их для отката.

## Установка

1. Выключите три заменяемых мода, перечисленных выше.
2. Установите этот исходник в Windhawk и включите мод.
3. Выйдите из учётной записи Windows и войдите снова. Это пересоздаст
   ThemeSection и позволит наблюдать запросы новых окон с самого начала.
4. Полностью перезапустите нужные приложения, если они остались запущены
   при установке или обновлении мода.

Глобальный список исключений Windhawk имеет приоритет над модом. Если приложение
исключено там, исправление в его процесс не загрузится. Сам мод этот список
не изменяет. Старые strip/darkframe режимы для проверяемых приложений выключите;
nopaint для других приложений можно оставить.

## Проверка и откат

Проверьте обычное окно, несколько циклов разворачивания/восстановления,
сворачивание и оба монитора. В журнале раздельно отмечаются исправление WCA,
обрезка клиентского свеса и обработка DWM margins. Успешная сборка и успешный
вызов API не заменяют визуальную проверку на вашем приложении.

При выключении восстанавливаются только действительно подменённые запросы.
Дескриптор ThemeSection возвращается только если его не изменил кто-то ещё.
Полный откат: выключить мод, включить прежний Classic Theme и выйти/войти в Windows.
Внутри dwm.exe код не исполняется.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

#define NOMINMAX
#include <windows.h>
#include <sddl.h>
#include <winternl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <windhawk_utils.h>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

extern "C" NTSTATUS NTAPI NtOpenSection(
    PHANDLE sectionHandle,
    ACCESS_MASK desiredAccess,
    POBJECT_ATTRIBUTES objectAttributes);

namespace SessionTheme {
namespace {

constexpr int kMaxAttempts = 20;
constexpr DWORD kRetryDelayMs = 100;

HANDLE g_stopEvent;
HANDLE g_retryThread;
SRWLOCK g_stateLock = SRWLOCK_INIT;
PSECURITY_DESCRIPTOR g_originalSecurityDescriptor;
PSECURITY_DESCRIPTOR g_appliedSecurityDescriptor;
bool g_securityChanged;

bool BuildThemeSectionObjectAttributes(
    DWORD sessionId,
    wchar_t (&sectionName)[256],
    UNICODE_STRING* objectName,
    OBJECT_ATTRIBUTES* objectAttributes) {
    const int length = swprintf_s(
        sectionName, ARRAYSIZE(sectionName),
        L"\\Sessions\\%lu\\Windows\\ThemeSection", sessionId);
    if (length <= 0 || static_cast<size_t>(length) >= ARRAYSIZE(sectionName)) {
        return false;
    }

    RtlInitUnicodeString(objectName, sectionName);
    InitializeObjectAttributes(objectAttributes, objectName,
                               OBJ_CASE_INSENSITIVE, nullptr, nullptr);
    return true;
}

bool ReadSecurityDescriptor(HANDLE section,
                            PSECURITY_DESCRIPTOR* descriptor,
                            DWORD* descriptorSize) {
    DWORD bytesNeeded = 0;
    const BOOL sizeResult = GetKernelObjectSecurity(
        section, DACL_SECURITY_INFORMATION, nullptr, 0, &bytesNeeded);
    const DWORD sizeError = sizeResult ? ERROR_SUCCESS : GetLastError();
    if (sizeResult || sizeError != ERROR_INSUFFICIENT_BUFFER ||
        bytesNeeded == 0) {
        Wh_Log(L"Couldn't size ThemeSection security descriptor: %lu",
               sizeError);
        return false;
    }

    auto* buffer = static_cast<PSECURITY_DESCRIPTOR>(
        LocalAlloc(LMEM_FIXED, bytesNeeded));
    if (!buffer) {
        Wh_Log(L"Couldn't allocate %lu bytes for the security descriptor",
               bytesNeeded);
        return false;
    }

    if (!GetKernelObjectSecurity(section, DACL_SECURITY_INFORMATION, buffer,
                                 bytesNeeded, &bytesNeeded)) {
        Wh_Log(L"Couldn't read ThemeSection security descriptor: %lu",
               GetLastError());
        LocalFree(buffer);
        return false;
    }

    *descriptor = buffer;
    *descriptorSize = bytesNeeded;
    return true;
}

void LogSecurityDescriptor(const wchar_t* prefix,
                           PSECURITY_DESCRIPTOR descriptor) {
    LPWSTR sddl = nullptr;
    if (ConvertSecurityDescriptorToStringSecurityDescriptorW(
            descriptor, SDDL_REVISION_1, DACL_SECURITY_INFORMATION, &sddl,
            nullptr)) {
        Wh_Log(L"%s: %s", prefix, sddl);
        LocalFree(sddl);
    }
}

bool SecurityDescriptorsHaveEqualDacls(PSECURITY_DESCRIPTOR first,
                                       PSECURITY_DESCRIPTOR second) {
    BOOL firstPresent = FALSE;
    BOOL firstDefaulted = FALSE;
    PACL firstDacl = nullptr;
    BOOL secondPresent = FALSE;
    BOOL secondDefaulted = FALSE;
    PACL secondDacl = nullptr;
    if (!GetSecurityDescriptorDacl(first, &firstPresent, &firstDacl,
                                   &firstDefaulted) ||
        !GetSecurityDescriptorDacl(second, &secondPresent, &secondDacl,
                                   &secondDefaulted)) {
        return false;
    }

    if (firstPresent != secondPresent) {
        return false;
    }
    if (!firstPresent) {
        return true;
    }
    if (!firstDacl || !secondDacl) {
        return firstDacl == secondDacl;
    }
    return firstDacl->AclSize == secondDacl->AclSize &&
           std::memcmp(firstDacl, secondDacl, firstDacl->AclSize) == 0;
}

bool SaveOriginalSecurityDescriptor(HANDLE section) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_originalSecurityDescriptor) {
        ReleaseSRWLockExclusive(&g_stateLock);
        return true;
    }

    PSECURITY_DESCRIPTOR descriptor = nullptr;
    DWORD descriptorSize = 0;
    const bool result =
        ReadSecurityDescriptor(section, &descriptor, &descriptorSize);
    if (result) {
        g_originalSecurityDescriptor = descriptor;
        LogSecurityDescriptor(L"Captured original ThemeSection DACL",
                              descriptor);
    }
    ReleaseSRWLockExclusive(&g_stateLock);
    return result;
}

bool SetDwmCompatibleClassicDacl() {
    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        Wh_Log(L"ProcessIdToSessionId failed: %lu", GetLastError());
        return false;
    }

    wchar_t sectionName[256];
    UNICODE_STRING objectName;
    OBJECT_ATTRIBUTES objectAttributes;
    if (!BuildThemeSectionObjectAttributes(sessionId, sectionName, &objectName,
                                           &objectAttributes)) {
        Wh_Log(L"Couldn't construct the ThemeSection object name");
        return false;
    }

    HANDLE section = nullptr;
    const NTSTATUS status = NtOpenSection(
        &section, READ_CONTROL | WRITE_DAC, &objectAttributes);
    if (status < 0) {
        Wh_Log(L"NtOpenSection(%s) failed: 0x%08X", sectionName,
               static_cast<unsigned int>(status));
        return false;
    }

    if (!SaveOriginalSecurityDescriptor(section)) {
        CloseHandle(section);
        return false;
    }

    // Stock uxtheme creates ThemeSection with GENERIC_READ for the Window
    // Manager group S-1-5-90-0. Use its already-mapped numeric equivalent
    // here because this DACL is applied to an existing kernel object:
    // READ_CONTROL | SECTION_QUERY | SECTION_MAP_READ = 0x00020005.
    // Ordinary interactive applications still receive READ_CONTROL only and
    // therefore cannot map the theme section.
    wchar_t sddl[512];
    const int sddlLength = swprintf_s(
        sddl, ARRAYSIZE(sddl),
        L"O:BAG:SYD:"
        L"(A;;RC;;;IU)"
        L"(A;;0x00020005;;;S-1-5-90-0)"
        L"(A;;DCSWRPSDRCWDWO;;;SY)");
    if (sddlLength <= 0 ||
        static_cast<size_t>(sddlLength) >= ARRAYSIZE(sddl)) {
        Wh_Log(L"Couldn't construct the replacement DACL");
        CloseHandle(section);
        return false;
    }

    PSECURITY_DESCRIPTOR replacement = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl, SDDL_REVISION_1, &replacement, nullptr)) {
        Wh_Log(L"Couldn't parse the replacement DACL: %lu", GetLastError());
        CloseHandle(section);
        return false;
    }

    const BOOL setResult = SetKernelObjectSecurity(
        section, DACL_SECURITY_INFORMATION, replacement);
    const DWORD setError = setResult ? ERROR_SUCCESS : GetLastError();
    if (setResult) {
        AcquireSRWLockExclusive(&g_stateLock);
        g_appliedSecurityDescriptor = replacement;
        g_securityChanged = true;
        ReleaseSRWLockExclusive(&g_stateLock);
        LogSecurityDescriptor(L"Applied DWM-compatible classic DACL",
                              replacement);
    } else {
        Wh_Log(L"SetKernelObjectSecurity failed: %lu", setError);
    }

    if (!setResult) {
        LocalFree(replacement);
    }
    CloseHandle(section);
    return setResult != FALSE;
}

DWORD WINAPI RetryThreadProc(void*) {
    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            return ERROR_CANCELLED;
        }

        Wh_Log(L"ThemeSection attempt %d/%d", attempt, kMaxAttempts);
        if (SetDwmCompatibleClassicDacl()) {
            return ERROR_SUCCESS;
        }

        if (attempt != kMaxAttempts &&
            WaitForSingleObject(g_stopEvent, kRetryDelayMs) == WAIT_OBJECT_0) {
            return ERROR_CANCELLED;
        }
    }

    Wh_Log(L"All ThemeSection attempts failed");
    return ERROR_RETRY;
}

void RestoreOriginalSecurityDescriptor() {
    AcquireSRWLockShared(&g_stateLock);
    const bool shouldRestore =
        g_securityChanged && g_originalSecurityDescriptor != nullptr &&
        g_appliedSecurityDescriptor != nullptr;
    PSECURITY_DESCRIPTOR original = g_originalSecurityDescriptor;
    PSECURITY_DESCRIPTOR applied = g_appliedSecurityDescriptor;
    ReleaseSRWLockShared(&g_stateLock);

    if (!shouldRestore) {
        return;
    }

    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        Wh_Log(L"Can't restore DACL; ProcessIdToSessionId failed: %lu",
               GetLastError());
        return;
    }

    wchar_t sectionName[256];
    UNICODE_STRING objectName;
    OBJECT_ATTRIBUTES objectAttributes;
    if (!BuildThemeSectionObjectAttributes(sessionId, sectionName, &objectName,
                                           &objectAttributes)) {
        return;
    }

    HANDLE section = nullptr;
    const NTSTATUS status = NtOpenSection(
        &section, READ_CONTROL | WRITE_DAC, &objectAttributes);
    if (status < 0) {
        Wh_Log(L"Can't restore DACL; NtOpenSection failed: 0x%08X",
               static_cast<unsigned int>(status));
        return;
    }

    PSECURITY_DESCRIPTOR current = nullptr;
    DWORD currentSize = 0;
    if (!ReadSecurityDescriptor(section, &current, &currentSize)) {
        CloseHandle(section);
        return;
    }

    if (!SecurityDescriptorsHaveEqualDacls(current, applied)) {
        Wh_Log(L"ThemeSection DACL changed after this mod applied it; "
               L"skipping stale rollback");
        LogSecurityDescriptor(L"Current ThemeSection DACL", current);
        LocalFree(current);
        CloseHandle(section);
        return;
    }

    LocalFree(current);
    if (SetKernelObjectSecurity(section, DACL_SECURITY_INFORMATION, original)) {
        AcquireSRWLockExclusive(&g_stateLock);
        g_securityChanged = false;
        ReleaseSRWLockExclusive(&g_stateLock);
        Wh_Log(L"Restored the captured ThemeSection DACL");
    } else {
        Wh_Log(L"Couldn't restore ThemeSection DACL: %lu", GetLastError());
    }
    CloseHandle(section);
}

}  // namespace

BOOL Init() {
    Wh_Log(L"Initializing");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed: %lu", GetLastError());
        return FALSE;
    }

    if (SetDwmCompatibleClassicDacl()) {
        return TRUE;
    }

    g_retryThread =
        CreateThread(nullptr, 0, RetryThreadProc, nullptr, 0, nullptr);
    if (!g_retryThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());

        AcquireSRWLockExclusive(&g_stateLock);
        PSECURITY_DESCRIPTOR original = g_originalSecurityDescriptor;
        g_originalSecurityDescriptor = nullptr;
        ReleaseSRWLockExclusive(&g_stateLock);
        if (original) {
            LocalFree(original);
        }

        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Uninit() {
    Wh_Log(L"Uninitializing");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_retryThread) {
        WaitForSingleObject(g_retryThread, INFINITE);
        CloseHandle(g_retryThread);
        g_retryThread = nullptr;
    }

    RestoreOriginalSecurityDescriptor();

    AcquireSRWLockExclusive(&g_stateLock);
    PSECURITY_DESCRIPTOR original = g_originalSecurityDescriptor;
    PSECURITY_DESCRIPTOR applied = g_appliedSecurityDescriptor;
    g_originalSecurityDescriptor = nullptr;
    g_appliedSecurityDescriptor = nullptr;
    ReleaseSRWLockExclusive(&g_stateLock);
    if (original) {
        LocalFree(original);
    }
    if (applied) {
        LocalFree(applied);
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}


} // namespace SessionTheme

// App-side component. Requires Windhawk, user32, dwmapi, uxtheme, comctl32.
#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <windhawk_utils.h>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace AppFrame {
constexpr DWORD kBackdropAttribute = 38, kMainBackdrop = 2, kExileAttribute = 11;
constexpr wchar_t kIdentityProperty[] = L"Windhawk.ClassicThemeWindowCompatibility.State.1";
struct CompositionData { DWORD attribute; void* value; SIZE_T size; };
using SetComposition = BOOL(WINAPI*)(HWND, CompositionData*);
SetComposition originalComposition;
decltype(&DwmSetWindowAttribute) originalAttribute;
decltype(&DwmExtendFrameIntoClientArea) originalExtend;
decltype(&CreateWindowExW) originalCreateW;
decltype(&CreateWindowExA) originalCreateA;
std::atomic<bool> unloading{false};
UINT reconcileMessage;
std::mutex stateMutex;
struct State {
    HWND window{};
    bool registered{}, subclassed{}, attaching{}, pending{}, destroyed{}, recognized{}, active{}, warnedAttach{};
    bool loggedClamp{};
    bool exileKnown{}, exileModified{}, exileDirty{}, warnedExile{}, warnedMargins{};
    BOOL wantedExile{};
    uint64_t exileEpoch{}, exileDesiredSerial{};
    bool marginsKnown{}, marginsModified{}, marginsDirty{};
    MARGINS wantedMargins{};
    uint64_t marginsEpoch{}, marginsDesiredSerial{};
    unsigned internalDepth{};
};
using StatePtr = std::shared_ptr<State>;
std::unordered_map<HWND, StatePtr> states;

bool OwnWindow(HWND window) {
    DWORD pid{};
    return window && GetWindowThreadProcessId(window, &pid) && pid == GetCurrentProcessId();
}
bool OwnRoot(HWND window) {
    return OwnWindow(window) && GetAncestor(window, GA_ROOT) == window &&
           !(GetWindowLongPtrW(window, GWL_STYLE) & WS_CHILD) && !GetWindow(window, GW_OWNER);
}
StatePtr Find(HWND window) {
    std::lock_guard lock(stateMutex);
    auto it = states.find(window);
    return it == states.end() ? nullptr : it->second;
}
StatePtr Track(HWND window) {
    if (!OwnWindow(window)) return nullptr;
    StatePtr state = Find(window);
    if (state) {
        bool registered;
        { std::lock_guard lock(stateMutex); registered = state->registered; }
        // Registration can overlap an early application request. Such a request is
        // observed, but cannot be replaced until the HWND identity is secured.
        if (!registered || GetPropW(window, kIdentityProperty) == state.get()) return state;
        std::lock_guard lock(stateMutex);
        state->destroyed = true;
        auto it = states.find(window);
        if (it != states.end() && it->second == state) states.erase(it);
    }
    if (unloading || !OwnRoot(window)) return nullptr;
    state = std::make_shared<State>(); state->window = window;
    {
        std::lock_guard lock(stateMutex);
        if (unloading) return nullptr;
        auto result = states.emplace(window, state);
        if (!result.second) return result.first->second;
    }
    const bool success = SetPropW(window, kIdentityProperty, state.get());
    bool discard;
    {
        std::lock_guard lock(stateMutex);
        discard = !success || unloading;
        state->registered = !discard; state->destroyed = discard;
        if (discard) {
            auto it = states.find(window);
            if (it != states.end() && it->second == state) states.erase(it);
        }
    }
    if (discard) {
        if (success && GetPropW(window, kIdentityProperty) == state.get()) RemovePropW(window, kIdentityProperty);
        return nullptr;
    }
    return state;
}
State Snapshot(const StatePtr& state) {
    std::lock_guard lock(stateMutex);
    return *state;
}
bool SameWindow(const StatePtr& state) {
    const State value = Snapshot(state);
    return value.registered && !value.destroyed && OwnWindow(value.window) &&
           GetPropW(value.window, kIdentityProperty) == state.get();
}
bool CopyInput(const void* source, void* target, SIZE_T bytes) {
    SIZE_T copied{};
    return source && ReadProcessMemory(GetCurrentProcess(), source, target, bytes, &copied) && copied == bytes;
}
bool SameMargins(const MARGINS& a, const MARGINS& b) {
    return a.cxLeftWidth == b.cxLeftWidth && a.cxRightWidth == b.cxRightWidth &&
           a.cyTopHeight == b.cyTopHeight && a.cyBottomHeight == b.cyBottomHeight;
}
struct GeometrySnapshot {
    bool structural{}, classic{}, backdrop{}, valid{}, zoomed{}, iconic{};
    RECT window{}, client{}, work{};
};
GeometrySnapshot Inspect(HWND window) {
    GeometrySnapshot s;
    s.classic = !IsThemeActive(); // Theme fallback can change after DLL initialization.
    if (!OwnRoot(window)) return s;
    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    s.structural = (style & (WS_CAPTION | WS_THICKFRAME)) == (WS_CAPTION | WS_THICKFRAME) &&
                   (exStyle & WS_EX_NOREDIRECTIONBITMAP) && !(exStyle & WS_EX_TOOLWINDOW);
    if (!s.structural) return s;
    DWORD backdrop{};
    s.backdrop = SUCCEEDED(DwmGetWindowAttribute(window, kBackdropAttribute, &backdrop, sizeof(backdrop))) &&
                 backdrop == kMainBackdrop;
    s.zoomed = IsZoomed(window); s.iconic = IsIconic(window);
    const auto previousDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WINDOWINFO info{sizeof(info)};
    MONITORINFO monitor{sizeof(monitor)};
    s.valid = GetWindowInfo(window, &info) &&
              GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor);
    if (s.valid) { s.window = info.rcWindow; s.client = info.rcClient; s.work = monitor.rcWork; }
    if (previousDpi) SetThreadDpiAwarenessContext(previousDpi);
    return s;
}
bool Near(LONG a, LONG b) { return a - b >= -1 && a - b <= 1; }
bool Narrow(LONG value) { return value > 0 && value <= 32; }
// Pure geometry selector, shared with the offline regression harness.
bool ShouldTarget(const GeometrySnapshot& s, bool recognized) {
    if (!s.classic || !s.structural || !s.backdrop) return false;
    if (recognized && (s.zoomed || s.iconic)) return true;
    if (!s.valid || s.iconic) return false;
    const LONG l = s.client.left-s.window.left, t = s.client.top-s.window.top;
    const LONG r = s.window.right-s.client.right, b = s.window.bottom-s.client.bottom;
    if (!Narrow(l) || !Narrow(r) || !Narrow(b) || !Near(l,r) || !Near(l,b)) return false;
    if (!s.zoomed) return t == 0;
    if (!Narrow(t) || !Near(l,t)) return false;
    const LONG over[] = {s.work.left-s.window.left, s.work.top-s.window.top,
                         s.window.right-s.work.right, s.window.bottom-s.work.bottom};
    bool extends = false;
    for (LONG amount : over) {
        if (amount < 0 || amount > 32) return false;
        extends = extends || amount > 0;
    }
    return extends;
}
// Only shrink the client rectangle returned by the application; never enlarge it.
bool ClampClientRect(RECT& client, const RECT& work) {
    RECT clipped;
    if (!IntersectRect(&clipped, &client, &work) || EqualRect(&clipped, &client)) return false;
    client = clipped;
    return true;
}
LRESULT CALLBACK Subclass(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
void Schedule(const StatePtr& state) {
    if (!state || unloading) return;
    bool attach = false;
    {
        std::lock_guard lock(stateMutex);
        if (unloading || state->destroyed || !state->registered || state->pending || state->attaching) return;
        if (!state->subclassed) { state->attaching = true; attach = true; }
    }
    if (attach) {
        // This helper installs on the owning UI thread; no state lock crosses SendMessage.
        const bool success = WindhawkUtils::SetWindowSubclassFromAnyThread(state->window, Subclass, 0);
        bool detach, warn = false;
        {
            std::lock_guard lock(stateMutex);
            state->attaching = false;
            detach = success && (unloading || state->destroyed);
            state->subclassed = success && !detach;
            if (!success) {
                warn = !state->warnedAttach; state->warnedAttach = true;
            }
        }
        if (warn) Wh_Log(L"AppFrame: subclass attachment failed hwnd=%p; keeping observed requests for rollback", state->window);
        if (detach) WindhawkUtils::RemoveWindowSubclassFromAnyThread(state->window, Subclass);
        if (!success || detach) return;
    }
    {
        std::lock_guard lock(stateMutex);
        if (unloading || state->destroyed || state->pending || !state->subclassed) return;
        state->pending = true;
    }
    if (!PostMessageW(state->window, reconcileMessage, 0, 0)) {
        std::lock_guard lock(stateMutex);
        state->pending = false;
    }
}
BOOL WriteExile(HWND window, BOOL exile) {
    CompositionData data{kExileAttribute, &exile, sizeof(exile)};
    return originalComposition(window, &data);
}
void RefreshFrame(HWND window) {
    if (OwnRoot(window)) SetWindowPos(window, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}
// A snapshot/revision pairs an internal write with the last observed application request.
// If an application request reenters an API call, the next posted pass converges to it.
bool SyncExile(const StatePtr& state, bool active) {
    bool changed = false, newerRequest = false;
    for (unsigned attempt = 0; attempt < 2; ++attempt) {
        const State before = Snapshot(state);
        if (before.destroyed || !before.exileKnown || !SameWindow(state)) return changed;
        const BOOL desired = active && !unloading && before.wantedExile ? FALSE : before.wantedExile;
        const bool modify = desired != before.wantedExile;
        if (!before.exileDirty && before.exileModified == modify) return changed;
        uint64_t serial;
        {
            std::lock_guard lock(stateMutex);
            if (state->exileEpoch != before.exileEpoch) { newerRequest = true; continue; }
            serial = ++state->exileEpoch; ++state->internalDepth;
        }
        const bool success = WriteExile(before.window, desired);
        {
            std::lock_guard lock(stateMutex);
            --state->internalDepth;
            newerRequest = state->exileEpoch != serial;
            if (success) {
                state->exileModified = modify;
                state->exileDirty = newerRequest;
            }
        }
        // An API failure waits for the next external window event, never a busy retry loop.
        if (!success) return changed;
        changed = true;
        Wh_Log(L"AppFrame: %s NC exile hwnd=%p", modify ? L"cleared" : L"restored", before.window);
        if (!newerRequest && !(unloading && modify)) return changed;
    }
    if (newerRequest && !unloading) Schedule(state);
    return changed;
}
bool SyncMargins(const StatePtr& state, bool zero) {
    bool changed = false, newerRequest = false;
    for (unsigned attempt = 0; attempt < 2; ++attempt) {
        const State before = Snapshot(state);
        if (before.destroyed || !before.marginsKnown || !SameWindow(state)) return changed;
        const MARGINS desired = zero && !unloading ? MARGINS{} : before.wantedMargins;
        const bool modify = !SameMargins(desired, before.wantedMargins);
        if (!before.marginsDirty && before.marginsModified == modify) return changed;
        uint64_t serial;
        {
            std::lock_guard lock(stateMutex);
            if (state->marginsEpoch != before.marginsEpoch) { newerRequest = true; continue; }
            serial = ++state->marginsEpoch; ++state->internalDepth;
        }
        const bool success = SUCCEEDED(originalExtend(before.window, &desired));
        {
            std::lock_guard lock(stateMutex);
            --state->internalDepth;
            newerRequest = state->marginsEpoch != serial;
            if (success) {
                state->marginsModified = modify;
                state->marginsDirty = newerRequest;
            }
        }
        if (!success) return changed;
        changed = true;
        Wh_Log(L"AppFrame: %s DWM extension hwnd=%p", modify ? L"zeroed maximized" : L"restored requested", before.window);
        if (!newerRequest && !(unloading && modify)) return changed;
    }
    if (newerRequest && !unloading) Schedule(state);
    return changed;
}
void Reconcile(const StatePtr& state) {
    if (unloading) return;
    const State before = Snapshot(state);
    if (before.destroyed) return;
    const auto geometry = Inspect(before.window);
    const bool active = ShouldTarget(geometry, before.recognized);
    bool warnExile = false, warnMargins = false;
    {
        std::lock_guard lock(stateMutex);
        if (unloading || state->destroyed) return;
        state->active = active;
        state->recognized = active || (before.recognized && geometry.structural && geometry.backdrop &&
                                      (geometry.iconic || geometry.zoomed));
        if (active && !state->exileKnown && !state->warnedExile)
            warnExile = state->warnedExile = true;
        if (active && geometry.zoomed && !state->marginsKnown && !state->warnedMargins)
            warnMargins = state->warnedMargins = true;
    }
    if (warnExile) Wh_Log(L"AppFrame: matching hwnd=%p has unknown exile state; fully restart the application", before.window);
    if (warnMargins) Wh_Log(L"AppFrame: hwnd=%p has unknown DWM extension margins; preserving them until observed", before.window);
    const bool exileChanged = SyncExile(state, active);
    const bool marginsChanged = SyncMargins(state, active && geometry.zoomed);
    if (!unloading && (active != before.active || exileChanged || marginsChanged)) RefreshFrame(before.window);
}
LRESULT CALLBACK Subclass(HWND window, UINT message, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    const StatePtr state = Find(window);
    if (message == reconcileMessage && state) {
        { std::lock_guard lock(stateMutex); state->pending = false; }
        if (!unloading) Reconcile(state);
        return 0;
    }
    RECT proposed{};
    if (message == WM_NCCALCSIZE && lParam)
        proposed = wParam ? reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0] : *reinterpret_cast<RECT*>(lParam);
    const LRESULT result = DefSubclassProc(window, message, wParam, lParam);
    if (!state) return result;
    if (message == WM_NCDESTROY) {
        std::lock_guard lock(stateMutex);
        state->destroyed = true; state->subclassed = false;
        auto it = states.find(window);
        if (it != states.end() && it->second == state) states.erase(it);
        return result;
    }
    if (unloading) return result;
    if (message == WM_NCCALCSIZE && lParam && IsZoomed(window)) {
        const State current = Snapshot(state);
        const auto geometry = Inspect(window);
        if (ShouldTarget(geometry, current.recognized)) {
            MONITORINFO monitor{sizeof(monitor)};
            if (GetMonitorInfoW(MonitorFromRect(&proposed, MONITOR_DEFAULTTONEAREST), &monitor)) {
                RECT& client = wParam ? reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0] : *reinterpret_cast<RECT*>(lParam);
                // WVR_VALIDRECTS/ALIGN apply to the old returned rectangle. After shrinking it,
                // request a repaint with 0 instead of retaining invalid copy-alignment flags.
                const RECT originalClient = client;
                if (ClampClientRect(client, monitor.rcWork)) {
                    bool log;
                    { std::lock_guard lock(stateMutex); log = !state->loggedClamp; state->loggedClamp = true; }
                    if (log) Wh_Log(L"AppFrame: clamped maximized client hwnd=%p [%ld,%ld,%ld,%ld] -> [%ld,%ld,%ld,%ld]",
                        window, originalClient.left, originalClient.top, originalClient.right, originalClient.bottom,
                        client.left, client.top, client.right, client.bottom);
                    return 0;
                }
            }
        }
    }
    switch (message) {
        case WM_WINDOWPOSCHANGED: case WM_SIZE: case WM_STYLECHANGED:
        case WM_THEMECHANGED: case WM_DWMCOMPOSITIONCHANGED: case WM_DWMNCRENDERINGCHANGED:
        case WM_DPICHANGED: case WM_DISPLAYCHANGE: case WM_SETTINGCHANGE: case WM_SHOWWINDOW:
            if (!Snapshot(state).internalDepth) Schedule(state);
            break;
    }
    return result;
}
BOOL WINAPI CompositionHook(HWND window, CompositionData* input) {
    const DWORD callerError = GetLastError();
    CompositionData copied{}; BOOL requested{};
    const bool valid = CopyInput(input, &copied, sizeof(copied)) && copied.attribute == kExileAttribute &&
                       copied.size == sizeof(requested) && CopyInput(copied.value, &requested, sizeof(requested));
    const StatePtr state = valid ? Track(window) : nullptr;
    if (!state) { SetLastError(callerError); return originalComposition(window, input); }
    const State before = Snapshot(state);
    const bool replace = !unloading && requested && SameWindow(state) && ShouldTarget(Inspect(window), before.recognized);
    uint64_t serial;
    { std::lock_guard lock(stateMutex); serial = ++state->exileEpoch; }
    BOOL clear = FALSE; CompositionData replacement{kExileAttribute, &clear, sizeof(clear)};
    SetLastError(callerError);
    const BOOL result = originalComposition(window, replace ? &replacement : input);
    const DWORD error = GetLastError();
    if (result) {
        {
            std::lock_guard lock(stateMutex);
            if (serial >= state->exileDesiredSerial) {
                state->exileKnown = true; state->wantedExile = requested ? TRUE : FALSE;
                state->exileDesiredSerial = serial;
            }
            if (serial == state->exileEpoch) {
                state->exileModified = replace; state->exileDirty = false;
            } else state->exileDirty = true;
        }
        if (replace && !before.exileModified)
            Wh_Log(L"AppFrame: intercepted NC exile TRUE hwnd=%p", window);
        else if (requested && !before.exileKnown)
            Wh_Log(L"AppFrame: observed early NC exile TRUE hwnd=%p", window);
        if (unloading) SyncExile(state, false);
        else Schedule(state);
    }
    SetLastError(error);
    return result;
}
HRESULT WINAPI AttributeHook(HWND window, DWORD attribute, LPCVOID value, DWORD size) {
    const HRESULT result = originalAttribute(window, attribute, value, size);
    const DWORD error = GetLastError();
    if (SUCCEEDED(result) && attribute == kBackdropAttribute) Schedule(Track(window));
    SetLastError(error);
    return result;
}
HRESULT WINAPI ExtendHook(HWND window, const MARGINS* input) {
    const DWORD callerError = GetLastError();
    MARGINS requested{};
    const StatePtr state = CopyInput(input, &requested, sizeof(requested)) ? Track(window) : nullptr;
    if (!state) { SetLastError(callerError); return originalExtend(window, input); }
    const State before = Snapshot(state);
    const auto geometry = Inspect(window);
    const bool replace = !unloading && SameWindow(state) && geometry.zoomed && ShouldTarget(geometry, before.recognized);
    uint64_t serial;
    { std::lock_guard lock(stateMutex); serial = ++state->marginsEpoch; }
    const MARGINS zero{};
    SetLastError(callerError);
    const HRESULT result = originalExtend(window, replace ? &zero : input);
    const DWORD error = GetLastError();
    if (SUCCEEDED(result)) {
        {
            std::lock_guard lock(stateMutex);
            if (serial >= state->marginsDesiredSerial) {
                state->marginsKnown = true; state->wantedMargins = requested; state->marginsDesiredSerial = serial;
            }
            if (serial == state->marginsEpoch) {
                state->marginsModified = replace && !SameMargins(requested, zero); state->marginsDirty = false;
            } else state->marginsDirty = true;
        }
        if (replace && !SameMargins(requested, zero) && !before.marginsModified)
            Wh_Log(L"AppFrame: intercepted maximized DWM extension hwnd=%p", window);
        if (unloading) SyncMargins(state, false);
        else Schedule(state);
    }
    SetLastError(error);
    return result;
}
HWND WINAPI CreateHookW(DWORD ex, LPCWSTR cls, LPCWSTR title, DWORD style, int x, int y, int width,
                        int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    const HWND window = originalCreateW(ex, cls, title, style, x, y, width, height, parent, menu, instance, param);
    const DWORD error = GetLastError(); Schedule(Track(window)); SetLastError(error); return window;
}
HWND WINAPI CreateHookA(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style, int x, int y, int width,
                        int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    const HWND window = originalCreateA(ex, cls, title, style, x, y, width, height, parent, menu, instance, param);
    const DWORD error = GetLastError(); Schedule(Track(window)); SetLastError(error); return window;
}
bool Init() {
    reconcileMessage = RegisterWindowMessageW(L"Windhawk.ClassicThemeUnifiedFrameFix.Reconcile.1");
    const HMODULE user32 = GetModuleHandleW(L"user32.dll");
    const auto composition = user32 ? reinterpret_cast<SetComposition>(GetProcAddress(user32, "SetWindowCompositionAttribute")) : nullptr;
    return reconcileMessage && composition &&
        WindhawkUtils::SetFunctionHook(composition, CompositionHook, &originalComposition) &&
        WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute, AttributeHook, &originalAttribute) &&
        WindhawkUtils::SetFunctionHook(DwmExtendFrameIntoClientArea, ExtendHook, &originalExtend) &&
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateHookW, &originalCreateW) &&
        WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateHookA, &originalCreateA);
}
BOOL CALLBACK Enumerate(HWND window, LPARAM) { Schedule(Track(window)); return TRUE; }
void AfterInit() { if (!unloading) EnumWindows(Enumerate, 0); }
void BeforeUninit() {
    unloading = true;
    std::vector<StatePtr> windows;
    { std::lock_guard lock(stateMutex); for (const auto& entry : states) windows.push_back(entry.second); }
    for (const StatePtr& state : windows) {
        const State before = Snapshot(state);
        if (before.destroyed || !OwnWindow(before.window)) continue;
        if (SameWindow(state)) { SyncExile(state, false); SyncMargins(state, false); }
        if (before.subclassed) WindhawkUtils::RemoveWindowSubclassFromAnyThread(before.window, Subclass);
        if (before.active) RefreshFrame(before.window);
    }
}
void Uninit() {
    // Original hooks have drained now. Keep identities until this point so an
    // in-flight replaced request can still restore the latest application value.
    std::vector<StatePtr> windows;
    { std::lock_guard lock(stateMutex); for (const auto& entry : states) windows.push_back(entry.second); }
    for (const StatePtr& state : windows)
        if (OwnWindow(state->window) && GetPropW(state->window, kIdentityProperty) == state.get())
            RemovePropW(state->window, kIdentityProperty);
    std::lock_guard lock(stateMutex); states.clear();
}
} // namespace AppFrame

namespace {
bool g_isSessionThemeProcess = false;
bool g_isApplicationProcess = false;
}

BOOL Wh_ModInit() {
    wchar_t imagePath[32768]{};
    if (!GetModuleFileNameW(nullptr, imagePath, ARRAYSIZE(imagePath))) {
        return FALSE;
    }
    const wchar_t* fileName = wcsrchr(imagePath, L'\\');
    fileName = fileName ? fileName + 1 : imagePath;
    g_isSessionThemeProcess = _wcsicmp(fileName, L"winlogon.exe") == 0;
    if (g_isSessionThemeProcess) {
        return SessionTheme::Init();
    }

    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) || !sessionId ||
        wcsstr(GetCommandLineW(), L"--type=")) {
        return TRUE;
    }
    g_isApplicationProcess = true;
    return AppFrame::Init();
}

void Wh_ModAfterInit() {
    if (g_isApplicationProcess) AppFrame::AfterInit();
}

void Wh_ModBeforeUninit() {
    if (g_isApplicationProcess) AppFrame::BeforeUninit();
}

void Wh_ModUninit() {
    if (g_isSessionThemeProcess) SessionTheme::Uninit();
    if (g_isApplicationProcess) AppFrame::Uninit();
}
