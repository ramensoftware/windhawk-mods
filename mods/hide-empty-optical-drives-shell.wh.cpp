// ==WindhawkMod==
// @id              hide-empty-optical-drives-shell
// @name            Hide Empty Optical Drives
// @description     Hide empty CD/DVD/BD drives from This PC while keeping the drives fully available to Windows.
// @version         1.0.0
// @author          Solo_mag
// @github          https://github.com/Solomag
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshell32 -lshlwapi -lcomctl32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Empty Optical Drives

Hides empty optical drives from **This PC** while leaving the device and its
drive letter fully available to Windows.

![Demo](https://raw.githubusercontent.com/Solomag/windhawk-assets/main/hide-empty-optical-drives/demo.gif)

Unlike the built-in "Hide empty drives" option, this mod is intended for
optical CD/DVD/BD drives that still remain visible in This PC when empty.

The mod only affects File Explorer (`explorer.exe`). File dialogs and
third-party file managers are not modified.

Media detection is event-driven. There is no permanent polling. After Windows
reports media insertion, a background worker retries briefly while an optical
disc spins up.

Detection is conservative:
- readable/mounted media -> show;
- no ready media -> hide;
- unknown/error state -> show (fail open).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- driveLetters: ""
  $name: Optical drive letters
  $description: >-
    Optional letters to manage, for example G or DE.
    Leave empty to manage all optical drives.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dbt.h>
#include <commctrl.h>
#include <atomic>
#include <cwctype>
#include <windhawk_utils.h>

enum class MediaState : LONG {
    Unknown = 0,
    Empty = 1,
    Present = 2,
};

constexpr DWORD kAllDriveBits = 0x03FFFFFFu;
constexpr DWORD kRetryIntervalMs = 500;
constexpr int kMaxRetryAttempts = 20;

constexpr UINT kMsgRefreshThisPc = WM_APP + 1;
constexpr UINT kMsgShutdown = WM_APP + 2;
constexpr UINT kMsgQuit = WM_APP + 3;

std::atomic<DWORD> g_managedMask{kAllDriveBits};
std::atomic<DWORD> g_opticalMask{0};
std::atomic<LONG> g_mediaState[26];

std::atomic<DWORD> g_arrivalRequestMask{0};
std::atomic<DWORD> g_removalRequestMask{0};
std::atomic<bool> g_initialScanRequested{false};

HANDLE g_notificationThread = nullptr;
HANDLE g_notificationReadyEvent = nullptr;
DWORD g_notificationThreadId = 0;
std::atomic<HWND> g_notificationWindow{nullptr};
std::atomic<bool> g_notificationThreadReady{false};

HANDLE g_workerThread = nullptr;
HANDLE g_workerWakeEvent = nullptr;
HANDLE g_workerStopEvent = nullptr;

using CDrivesViewCallback_ShouldShow_t =
    HRESULT(STDMETHODCALLTYPE*)(
        void*,
        IShellFolder*,
        LPCITEMIDLIST,
        LPCITEMIDLIST);

CDrivesViewCallback_ShouldShow_t
    CDrivesViewCallback_ShouldShow_Original = nullptr;

static DWORD LetterBit(WCHAR letter) {
    return 1u << (letter - L'A');
}

static void MakeRootPath(WCHAR letter, WCHAR (&root)[4]) {
    root[0] = letter;
    root[1] = L':';
    root[2] = L'\\';
    root[3] = L'\0';
}

static bool IsManagedLetter(WCHAR letter) {
    if (letter < L'A' || letter > L'Z') {
        return false;
    }

    return (g_managedMask.load(std::memory_order_acquire) &
            LetterBit(letter)) != 0;
}

static bool IsCachedOpticalDrive(WCHAR letter) {
    if (letter < L'A' || letter > L'Z') {
        return false;
    }

    return (g_opticalMask.load(std::memory_order_acquire) &
            LetterBit(letter)) != 0;
}

static MediaState GetCachedMediaState(WCHAR letter) {
    if (letter < L'A' || letter > L'Z') {
        return MediaState::Unknown;
    }

    return static_cast<MediaState>(
        g_mediaState[letter - L'A'].load(std::memory_order_acquire));
}

static bool SetCachedMediaState(WCHAR letter, MediaState state) {
    LONG old = g_mediaState[letter - L'A'].exchange(
        static_cast<LONG>(state),
        std::memory_order_acq_rel);

    if (old == static_cast<LONG>(state)) {
        return false;
    }

    Wh_Log(
        L"%c: state %d -> %d",
        letter,
        old,
        static_cast<LONG>(state));
    return true;
}

static bool SetOpticalDrivePresent(WCHAR letter, bool optical) {
    DWORD bit = LetterBit(letter);
    DWORD oldMask = g_opticalMask.load(std::memory_order_relaxed);

    for (;;) {
        DWORD newMask =
            optical ? (oldMask | bit) : (oldMask & ~bit);

        if (newMask == oldMask) {
            return false;
        }

        if (g_opticalMask.compare_exchange_weak(
                oldMask,
                newMask,
                std::memory_order_release,
                std::memory_order_relaxed)) {
            return true;
        }
    }
}

static MediaState ProbeOpticalMediaState(WCHAR letter) {
    WCHAR root[4];
    MakeRootPath(letter, root);

    if (GetDriveTypeW(root) != DRIVE_CDROM) {
        return MediaState::Unknown;
    }

    DWORD previousErrorMode = 0;
    bool errorModeChanged =
        SetThreadErrorMode(
            SEM_FAILCRITICALERRORS,
            &previousErrorMode) != FALSE;

    WCHAR volumeName[MAX_PATH + 1] = {};
    WCHAR fileSystemName[MAX_PATH + 1] = {};
    DWORD serial = 0;
    DWORD maxComponentLength = 0;
    DWORD fileSystemFlags = 0;

    SetLastError(ERROR_SUCCESS);

    BOOL ok = GetVolumeInformationW(
        root,
        volumeName,
        ARRAYSIZE(volumeName),
        &serial,
        &maxComponentLength,
        &fileSystemFlags,
        fileSystemName,
        ARRAYSIZE(fileSystemName));

    DWORD error = ok ? ERROR_SUCCESS : GetLastError();

    if (errorModeChanged) {
        SetThreadErrorMode(previousErrorMode, nullptr);
    }

    if (ok) {
        Wh_Log(
            L"%c: media present, volume=\"%s\", fs=\"%s\"",
            letter,
            volumeName,
            fileSystemName);
        return MediaState::Present;
    }

    if (error == ERROR_NOT_READY ||
        error == ERROR_NO_MEDIA_IN_DRIVE) {
        return MediaState::Empty;
    }

    Wh_Log(
        L"%c: media probe inconclusive, error=%u",
        letter,
        error);
    return MediaState::Unknown;
}

static void RequestThisPcRefresh() {
    HWND hwnd =
        g_notificationWindow.load(
            std::memory_order_acquire);

    if (hwnd) {
        PostMessageW(
            hwnd,
            kMsgRefreshThisPc,
            0,
            0);
    }
}

static bool GetDriveLetterFromItem(
    IShellFolder* folder,
    LPCITEMIDLIST pidlItem,
    WCHAR* letter
) {
    if (!folder || !pidlItem || !letter) {
        return false;
    }

    STRRET strret = {};

    if (FAILED(folder->GetDisplayNameOf(
            pidlItem,
            SHGDN_FORPARSING,
            &strret))) {
        return false;
    }

    WCHAR parsingName[MAX_PATH] = {};

    if (FAILED(StrRetToBufW(
            &strret,
            pidlItem,
            parsingName,
            ARRAYSIZE(parsingName)))) {
        return false;
    }

    WCHAR candidate =
        static_cast<WCHAR>(
            towupper(parsingName[0]));

    if (candidate < L'A' ||
        candidate > L'Z' ||
        parsingName[1] != L':') {
        return false;
    }

    *letter = candidate;
    return true;
}

static HRESULT STDMETHODCALLTYPE
CDrivesViewCallback_ShouldShow_Hook(
    void* self,
    IShellFolder* folder,
    LPCITEMIDLIST pidlFolder,
    LPCITEMIDLIST pidlItem
) {
    HRESULT hr =
        CDrivesViewCallback_ShouldShow_Original(
            self,
            folder,
            pidlFolder,
            pidlItem);

    if (hr == S_FALSE ||
        g_opticalMask.load(
            std::memory_order_acquire) == 0) {
        return hr;
    }

    WCHAR letter = 0;

    if (!GetDriveLetterFromItem(
            folder,
            pidlItem,
            &letter)) {
        return hr;
    }

    if (!IsManagedLetter(letter) ||
        !IsCachedOpticalDrive(letter)) {
        return hr;
    }

    return GetCachedMediaState(letter) ==
                   MediaState::Empty
               ? S_FALSE
               : hr;
}

static void QueueArrivalMask(DWORD mask) {
    if (!mask) {
        return;
    }

    g_arrivalRequestMask.fetch_or(
        mask,
        std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static void QueueRemovalMask(DWORD mask) {
    if (!mask) {
        return;
    }

    g_removalRequestMask.fetch_or(
        mask,
        std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static void QueueInitialScan() {
    g_initialScanRequested.store(
        true,
        std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static bool ProcessInitialScan() {
    bool changed = false;
    DWORD managedMask =
        g_managedMask.load(
            std::memory_order_acquire);

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        DWORD bit = LetterBit(letter);

        if (!(managedMask & bit)) {
            changed |= SetOpticalDrivePresent(
                letter,
                false);
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool optical =
            GetDriveTypeW(root) == DRIVE_CDROM;

        changed |= SetOpticalDrivePresent(
            letter,
            optical);

        if (!optical) {
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
            continue;
        }

        changed |= SetCachedMediaState(
            letter,
            ProbeOpticalMediaState(letter));
    }

    return changed;
}

static bool ProcessRemovalMask(DWORD mask) {
    bool changed = false;

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        DWORD bit = LetterBit(letter);

        if (!(mask & bit) ||
            !IsManagedLetter(letter)) {
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool stillOptical =
            GetDriveTypeW(root) == DRIVE_CDROM;

        if (stillOptical) {
            Wh_Log(
                L"%c: media/device removal, drive remains optical",
                letter);

            changed |= SetOpticalDrivePresent(
                letter,
                true);
            changed |= SetCachedMediaState(
                letter,
                MediaState::Empty);
        } else {
            Wh_Log(
                L"%c: optical drive no longer present",
                letter);

            changed |= SetOpticalDrivePresent(
                letter,
                false);
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
        }
    }

    return changed;
}

static bool ProcessArrivalMask(
    DWORD mask,
    DWORD* retryMask,
    int (&retryAttempts)[26]
) {
    bool changed = false;

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        DWORD bit = LetterBit(letter);

        if (!(mask & bit) ||
            !IsManagedLetter(letter)) {
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        if (GetDriveTypeW(root) != DRIVE_CDROM) {
            continue;
        }

        changed |= SetOpticalDrivePresent(
            letter,
            true);

        MediaState state =
            ProbeOpticalMediaState(letter);

        if (state == MediaState::Present) {
            *retryMask &= ~bit;
            retryAttempts[letter - L'A'] = 0;

            changed |= SetCachedMediaState(
                letter,
                MediaState::Present);
            continue;
        }

        // Fail open immediately for unreadable/blank media while still
        // retrying in case the drive becomes fully readable moments later.
        if (state == MediaState::Unknown) {
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
        }

        *retryMask |= bit;
        retryAttempts[letter - L'A'] = 0;
    }

    return changed;
}

static bool ProcessRetryMask(
    DWORD* retryMask,
    int (&retryAttempts)[26]
) {
    bool changed = false;
    DWORD pending = *retryMask;

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        DWORD bit = LetterBit(letter);

        if (!(pending & bit)) {
            continue;
        }

        int index = letter - L'A';

        if (!IsManagedLetter(letter)) {
            *retryMask &= ~bit;
            retryAttempts[index] = 0;
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        if (GetDriveTypeW(root) != DRIVE_CDROM) {
            *retryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetOpticalDrivePresent(
                letter,
                false);
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
            continue;
        }

        MediaState state =
            ProbeOpticalMediaState(letter);

        if (state == MediaState::Present) {
            *retryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetCachedMediaState(
                letter,
                MediaState::Present);
            continue;
        }

        if (state == MediaState::Unknown) {
            changed |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
        }

        if (++retryAttempts[index] >=
            kMaxRetryAttempts) {
            *retryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetCachedMediaState(
                letter,
                state);

            Wh_Log(
                L"%c: media-ready retry window expired",
                letter);
        }
    }

    return changed;
}

static DWORD WINAPI WorkerThreadProc(void*) {
    DWORD retryMask = 0;
    int retryAttempts[26] = {};

    HANDLE waits[] = {
        g_workerStopEvent,
        g_workerWakeEvent,
    };

    for (;;) {
        DWORD timeout =
            retryMask
                ? kRetryIntervalMs
                : INFINITE;

        DWORD waitResult =
            WaitForMultipleObjects(
                ARRAYSIZE(waits),
                waits,
                FALSE,
                timeout);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        bool refresh = false;

        if (g_initialScanRequested.exchange(
                false,
                std::memory_order_acq_rel)) {
            retryMask = 0;
            ZeroMemory(
                retryAttempts,
                sizeof(retryAttempts));

            refresh |= ProcessInitialScan();
        }

        DWORD removalMask =
            g_removalRequestMask.exchange(
                0,
                std::memory_order_acq_rel);

        if (removalMask) {
            retryMask &= ~removalMask;

            for (WCHAR letter = L'A';
                 letter <= L'Z';
                 letter++) {
                if (removalMask &
                    LetterBit(letter)) {
                    retryAttempts[
                        letter - L'A'] = 0;
                }
            }

            refresh |=
                ProcessRemovalMask(
                    removalMask);
        }

        DWORD arrivalMask =
            g_arrivalRequestMask.exchange(
                0,
                std::memory_order_acq_rel);

        if (arrivalMask) {
            refresh |=
                ProcessArrivalMask(
                    arrivalMask,
                    &retryMask,
                    retryAttempts);
        }

        if (waitResult == WAIT_TIMEOUT &&
            retryMask) {
            refresh |=
                ProcessRetryMask(
                    &retryMask,
                    retryAttempts);
        }

        if (refresh) {
            RequestThisPcRefresh();
        }
    }

    return 0;
}

static void RefreshThisPc(
    PIDLIST_ABSOLUTE thisPcPidl
) {
    if (!thisPcPidl) {
        return;
    }

    SHChangeNotify(
        SHCNE_UPDATEDIR,
        SHCNF_IDLIST |
            SHCNF_FLUSHNOWAIT,
        thisPcPidl,
        nullptr);
}

static LRESULT CALLBACK
NotificationWindowSubclassProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR,
    DWORD_PTR refData
) {
    auto thisPcPidl =
        reinterpret_cast<PIDLIST_ABSOLUTE>(
            refData);

    switch (message) {
        case WM_DEVICECHANGE:
            if ((wParam ==
                     DBT_DEVICEARRIVAL ||
                 wParam ==
                     DBT_DEVICEREMOVECOMPLETE) &&
                lParam) {
                auto* header =
                    reinterpret_cast<
                        const DEV_BROADCAST_HDR*>(
                        lParam);

                if (header->dbch_devicetype ==
                    DBT_DEVTYP_VOLUME) {
                    auto* volume =
                        reinterpret_cast<
                            const DEV_BROADCAST_VOLUME*>(
                            lParam);

                    DWORD mask =
                        volume->dbcv_unitmask;

                    if (wParam ==
                        DBT_DEVICEARRIVAL) {
                        QueueArrivalMask(mask);
                    } else {
                        QueueRemovalMask(mask);
                    }
                }
            }

            // Don't swallow any WM_DEVICECHANGE subtype.
            break;

        case kMsgRefreshThisPc:
            RefreshThisPc(thisPcPidl);
            return 0;

        case kMsgShutdown:
            RefreshThisPc(thisPcPidl);
            DestroyWindow(hwnd);
            return 0;

        case kMsgQuit:
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY:
            RemoveWindowSubclass(
                hwnd,
                NotificationWindowSubclassProc,
                1);
            g_notificationWindow.store(
                nullptr,
                std::memory_order_release);
            PostQuitMessage(0);
            break;
    }

    return DefSubclassProc(
        hwnd,
        message,
        wParam,
        lParam);
}

static DWORD WINAPI NotificationThreadProc(void*) {
    HRESULT coHr =
        CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

    bool uninitializeCom =
        coHr == S_OK ||
        coHr == S_FALSE;

    MSG msg = {};
    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE);

    PIDLIST_ABSOLUTE thisPcPidl = nullptr;

    HRESULT hr =
        SHGetKnownFolderIDList(
            FOLDERID_ComputerFolder,
            0,
            nullptr,
            &thisPcPidl);

    if (FAILED(hr) || !thisPcPidl) {
        Wh_Log(
            L"SHGetKnownFolderIDList failed: 0x%08X",
            hr);

        g_notificationThreadReady.store(
            false,
            std::memory_order_release);
        SetEvent(g_notificationReadyEvent);

        if (uninitializeCom) {
            CoUninitialize();
        }

        return 1;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        L"STATIC",
        L"",
        WS_POPUP,
        0,
        0,
        0,
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr);

    if (!hwnd) {
        Wh_Log(
            L"Notification window creation failed: %u",
            GetLastError());

        CoTaskMemFree(thisPcPidl);

        g_notificationThreadReady.store(
            false,
            std::memory_order_release);
        SetEvent(g_notificationReadyEvent);

        if (uninitializeCom) {
            CoUninitialize();
        }

        return 1;
    }

    if (!SetWindowSubclass(
            hwnd,
            NotificationWindowSubclassProc,
            1,
            reinterpret_cast<DWORD_PTR>(
                thisPcPidl))) {
        Wh_Log(
            L"Notification window subclass failed: %u",
            GetLastError());

        DestroyWindow(hwnd);
        CoTaskMemFree(thisPcPidl);

        g_notificationThreadReady.store(
            false,
            std::memory_order_release);
        SetEvent(g_notificationReadyEvent);

        if (uninitializeCom) {
            CoUninitialize();
        }

        return 1;
    }

    g_notificationWindow.store(
        hwnd,
        std::memory_order_release);
    g_notificationThreadReady.store(
        true,
        std::memory_order_release);
    SetEvent(g_notificationReadyEvent);

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoTaskMemFree(thisPcPidl);

    if (uninitializeCom) {
        CoUninitialize();
    }

    return 0;
}

static DWORD LoadManagedMask() {
    WindhawkUtils::StringSetting letters =
        WindhawkUtils::StringSetting::make(
            L"driveLetters");

    DWORD mask = 0;

    for (size_t i = 0;
         letters.get()[i];
         i++) {
        WCHAR ch =
            static_cast<WCHAR>(
                towupper(
                    letters.get()[i]));

        if (ch >= L'A' &&
            ch <= L'Z') {
            mask |= LetterBit(ch);
        }
    }

    return mask
        ? mask
        : kAllDriveBits;
}

static bool HookDrivesViewShouldShow() {
    HMODULE shell32 =
        GetModuleHandleW(
            L"shell32.dll");

    if (!shell32) {
        Wh_Log(
            L"shell32.dll isn't loaded");
        return false;
    }

    const WindhawkUtils::SYMBOL_HOOK
        shell32DllHooks[] = {
            {
                {
                    L"public: virtual long __cdecl "
                    L"CDrivesViewCallback::ShouldShow("
                    L"struct IShellFolder *,"
                    L"struct _ITEMIDLIST_ABSOLUTE const *,"
                    L"struct _ITEMID_CHILD const __unaligned *)"
                },
                &CDrivesViewCallback_ShouldShow_Original,
                CDrivesViewCallback_ShouldShow_Hook,
                false
            },
        };

    return WindhawkUtils::HookSymbols(
        shell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks));
}

static void StopWorkerThread() {
    if (!g_workerThread) {
        return;
    }

    SetEvent(g_workerStopEvent);
    SetEvent(g_workerWakeEvent);

    WaitForSingleObject(
        g_workerThread,
        INFINITE);

    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

static void StopNotificationThread(
    bool restoreView
) {
    if (!g_notificationThread) {
        return;
    }

    HWND hwnd =
        g_notificationWindow.load(
            std::memory_order_acquire);

    if (hwnd) {
        PostMessageW(
            hwnd,
            restoreView
                ? kMsgShutdown
                : kMsgQuit,
            0,
            0);
    } else if (g_notificationThreadId) {
        while (!PostThreadMessageW(
                    g_notificationThreadId,
                    WM_QUIT,
                    0,
                    0) &&
               WaitForSingleObject(
                   g_notificationThread,
                   50) ==
                   WAIT_TIMEOUT) {
        }
    }

    WaitForSingleObject(
        g_notificationThread,
        INFINITE);

    CloseHandle(g_notificationThread);
    g_notificationThread = nullptr;
    g_notificationThreadId = 0;
    g_notificationWindow.store(
        nullptr,
        std::memory_order_release);
}

static void CloseWorkerObjects() {
    if (g_workerWakeEvent) {
        CloseHandle(g_workerWakeEvent);
        g_workerWakeEvent = nullptr;
    }

    if (g_workerStopEvent) {
        CloseHandle(g_workerStopEvent);
        g_workerStopEvent = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(
        L"Initializing Hide Empty Optical Drives");

    for (auto& state : g_mediaState) {
        state.store(
            static_cast<LONG>(
                MediaState::Unknown),
            std::memory_order_relaxed);
    }

    g_opticalMask.store(
        0,
        std::memory_order_relaxed);

    g_managedMask.store(
        LoadManagedMask(),
        std::memory_order_release);

    g_arrivalRequestMask.store(
        0,
        std::memory_order_relaxed);
    g_removalRequestMask.store(
        0,
        std::memory_order_relaxed);
    g_initialScanRequested.store(
        false,
        std::memory_order_relaxed);

    g_notificationReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_notificationReadyEvent) {
        Wh_Log(
            L"CreateEvent(notification ready) failed: %u",
            GetLastError());
        return FALSE;
    }

    g_workerWakeEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr);

    g_workerStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_workerWakeEvent ||
        !g_workerStopEvent) {
        Wh_Log(
            L"CreateEvent(worker) failed: %u",
            GetLastError());

        CloseHandle(
            g_notificationReadyEvent);
        g_notificationReadyEvent =
            nullptr;
        CloseWorkerObjects();
        return FALSE;
    }

    g_notificationThread =
        CreateThread(
            nullptr,
            0,
            NotificationThreadProc,
            nullptr,
            0,
            &g_notificationThreadId);

    if (!g_notificationThread) {
        Wh_Log(
            L"CreateThread(notification) failed: %u",
            GetLastError());

        CloseHandle(
            g_notificationReadyEvent);
        g_notificationReadyEvent =
            nullptr;
        CloseWorkerObjects();
        return FALSE;
    }

    WaitForSingleObject(
        g_notificationReadyEvent,
        INFINITE);

    CloseHandle(
        g_notificationReadyEvent);
    g_notificationReadyEvent =
        nullptr;

    if (!g_notificationThreadReady.load(
            std::memory_order_acquire)) {
        StopNotificationThread(false);
        CloseWorkerObjects();
        return FALSE;
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_workerThread) {
        Wh_Log(
            L"CreateThread(worker) failed: %u",
            GetLastError());

        StopNotificationThread(false);
        CloseWorkerObjects();
        return FALSE;
    }

    if (!HookDrivesViewShouldShow()) {
        Wh_Log(
            L"Failed to hook "
            L"CDrivesViewCallback::ShouldShow");

        StopWorkerThread();
        StopNotificationThread(false);
        CloseWorkerObjects();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Hooks are active at this point, so refreshing an already-open This PC
    // window can no longer race ahead of CDrivesViewCallback::ShouldShow.
    QueueInitialScan();
    RequestThisPcRefresh();
}

void Wh_ModSettingsChanged() {
    g_managedMask.store(
        LoadManagedMask(),
        std::memory_order_release);

    QueueInitialScan();
    RequestThisPcRefresh();
}

void Wh_ModUninit() {
    Wh_Log(
        L"Uninitializing Hide Empty Optical Drives");

    // Windhawk has already removed the symbol hook before Wh_ModUninit.
    // Stop device probing first, then refresh This PC while shutting down
    // the notification window so items hidden by the mod reappear.
    StopWorkerThread();
    StopNotificationThread(true);
    CloseWorkerObjects();
}
