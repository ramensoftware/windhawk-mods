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

Unlike the built-in "Hide empty drives" option, this mod is intended for
optical CD/DVD/BD drives that still remain visible in This PC when empty.

The mod only affects File Explorer (`explorer.exe`). File dialogs and
third-party file managers are not modified.

Media detection is event-driven. There is no permanent polling. After Windows
reports media insertion, the mod retries briefly while an optical disc spins up.

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

constexpr UINT_PTR kRetryTimerId = 1;
constexpr UINT kRetryIntervalMs = 500;
constexpr int kMaxRetryAttempts = 20;

constexpr UINT kMsgRefreshThisPc = WM_APP + 1;
constexpr UINT kMsgShutdown = WM_APP + 2;
constexpr UINT kMsgInitialScan = WM_APP + 3;

std::atomic<bool> g_enabled{true};
std::atomic<DWORD> g_managedMask{0x03FFFFFFu};
std::atomic<DWORD> g_opticalMask{0};

std::atomic<LONG> g_mediaState[26];
DWORD g_pendingInsertMask = 0;
int g_retryAttempts[26] = {};

HANDLE g_notificationThread = nullptr;
HANDLE g_notificationReadyEvent = nullptr;
DWORD g_notificationThreadId = 0;
std::atomic<HWND> g_notificationWindow{nullptr};
std::atomic<bool> g_notificationThreadReady{false};

using CDrivesViewCallback_ShouldShow_t =
    HRESULT(STDMETHODCALLTYPE*)(
        void*,
        IShellFolder*,
        LPCITEMIDLIST,
        LPCITEMIDLIST);

CDrivesViewCallback_ShouldShow_t
    CDrivesViewCallback_ShouldShow_Original = nullptr;

static void MakeRootPath(WCHAR letter, WCHAR (&root)[4]) {
    root[0] = letter;
    root[1] = L':';
    root[2] = L'\\';
    root[3] = L'\0';
}

static DWORD LetterBit(WCHAR letter) {
    return 1u << (letter - L'A');
}

static bool IsManagedLetter(WCHAR letter) {
    if (letter < L'A' || letter > L'Z') {
        return false;
    }

    return (g_managedMask.load(std::memory_order_relaxed) &
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

static bool SetCachedMediaState(
    WCHAR letter,
    MediaState state
) {
    LONG old = g_mediaState[letter - L'A'].exchange(
        static_cast<LONG>(state),
        std::memory_order_acq_rel);

    if (old != static_cast<LONG>(state)) {
        Wh_Log(
            L"%c: state %d -> %d",
            letter,
            old,
            static_cast<LONG>(state));
        return true;
    }

    return false;
}

static bool SetOpticalDrivePresent(
    WCHAR letter,
    bool optical
) {
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
    DWORD serial = 0;
    DWORD maxComponentLength = 0;
    DWORD fileSystemFlags = 0;
    WCHAR fileSystemName[MAX_PATH + 1] = {};

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

static void RefreshThisPc(PIDLIST_ABSOLUTE thisPcPidl) {
    if (!thisPcPidl) {
        return;
    }

    SHChangeNotify(
        SHCNE_UPDATEDIR,
        SHCNF_IDLIST | SHCNF_FLUSHNOWAIT,
        thisPcPidl,
        nullptr);
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
        static_cast<WCHAR>(towupper(parsingName[0]));

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
        !g_enabled.load(std::memory_order_acquire)) {
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

    if (GetCachedMediaState(letter) ==
        MediaState::Empty) {
        return S_FALSE;
    }

    return hr;
}

static void StartInsertionRetry(
    HWND hwnd,
    WCHAR letter
) {
    int index = letter - L'A';
    DWORD bit = LetterBit(letter);

    g_pendingInsertMask |= bit;
    g_retryAttempts[index] = 0;

    SetTimer(
        hwnd,
        kRetryTimerId,
        kRetryIntervalMs,
        nullptr);
}

static void InitialScan(
    PIDLIST_ABSOLUTE thisPcPidl
) {
    bool changed = false;

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        if (!IsManagedLetter(letter)) {
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

        changed |= SetCachedMediaState(
            letter,
            state);
    }

    if (changed) {
        RefreshThisPc(thisPcPidl);
    }
}

static void ProcessPendingInsertions(
    HWND hwnd,
    PIDLIST_ABSOLUTE thisPcPidl
) {
    DWORD pending = g_pendingInsertMask;
    bool refresh = false;

    for (WCHAR letter = L'A';
         letter <= L'Z';
         letter++) {
        DWORD bit = LetterBit(letter);

        if (!(pending & bit)) {
            continue;
        }

        int index = letter - L'A';

        WCHAR root[4];
        MakeRootPath(letter, root);

        if (GetDriveTypeW(root) != DRIVE_CDROM) {
            g_pendingInsertMask &= ~bit;
            g_retryAttempts[index] = 0;
            refresh |= SetOpticalDrivePresent(
                letter,
                false);
            refresh |= SetCachedMediaState(
                letter,
                MediaState::Unknown);
            continue;
        }

        SetOpticalDrivePresent(letter, true);

        MediaState state =
            ProbeOpticalMediaState(letter);

        if (state == MediaState::Present) {
            g_pendingInsertMask &= ~bit;
            g_retryAttempts[index] = 0;

            refresh |= SetCachedMediaState(
                letter,
                MediaState::Present);
            continue;
        }

        if (++g_retryAttempts[index] >=
            kMaxRetryAttempts) {
            g_pendingInsertMask &= ~bit;
            g_retryAttempts[index] = 0;

            refresh |= SetCachedMediaState(
                letter,
                state);
        }
    }

    if (!g_pendingInsertMask) {
        KillTimer(hwnd, kRetryTimerId);
    }

    if (refresh) {
        RefreshThisPc(thisPcPidl);
    }
}

static void HandleVolumeDeviceChange(
    HWND hwnd,
    WPARAM eventType,
    LPARAM lParam,
    PIDLIST_ABSOLUTE thisPcPidl
) {
    if (!lParam) {
        return;
    }

    auto* header =
        reinterpret_cast<const DEV_BROADCAST_HDR*>(
            lParam);

    if (header->dbch_devicetype !=
        DBT_DEVTYP_VOLUME) {
        return;
    }

    auto* volume =
        reinterpret_cast<const DEV_BROADCAST_VOLUME*>(
            lParam);

    DWORD mask = volume->dbcv_unitmask;
    bool refresh = false;

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

        if (eventType == DBT_DEVICEARRIVAL) {
            if (GetDriveTypeW(root) !=
                DRIVE_CDROM) {
                continue;
            }

            refresh |= SetOpticalDrivePresent(
                letter,
                true);

            Wh_Log(
                L"%c: WM_DEVICECHANGE arrival%s",
                letter,
                (volume->dbcv_flags & DBTF_MEDIA)
                    ? L" (media)"
                    : L"");

            MediaState state =
                ProbeOpticalMediaState(letter);

            if (state == MediaState::Present) {
                g_pendingInsertMask &= ~bit;
                g_retryAttempts[
                    letter - L'A'] = 0;

                refresh |= SetCachedMediaState(
                    letter,
                    MediaState::Present);
            } else {
                StartInsertionRetry(
                    hwnd,
                    letter);
            }
        } else if (
            eventType ==
            DBT_DEVICEREMOVECOMPLETE) {
            bool stillOptical =
                GetDriveTypeW(root) ==
                DRIVE_CDROM;

            if (stillOptical) {
                Wh_Log(
                    L"%c: WM_DEVICECHANGE removal%s",
                    letter,
                    (volume->dbcv_flags &
                     DBTF_MEDIA)
                        ? L" (media)"
                        : L"");

                g_pendingInsertMask &= ~bit;
                g_retryAttempts[
                    letter - L'A'] = 0;

                refresh |=
                    SetOpticalDrivePresent(
                        letter,
                        true);

                refresh |=
                    SetCachedMediaState(
                        letter,
                        MediaState::Empty);
            } else if (
                IsCachedOpticalDrive(letter)) {
                g_pendingInsertMask &= ~bit;
                g_retryAttempts[
                    letter - L'A'] = 0;

                refresh |=
                    SetOpticalDrivePresent(
                        letter,
                        false);

                refresh |=
                    SetCachedMediaState(
                        letter,
                        MediaState::Unknown);
            }
        }
    }

    if (refresh) {
        RefreshThisPc(thisPcPidl);
    }
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
            if (wParam == DBT_DEVICEARRIVAL ||
                wParam ==
                    DBT_DEVICEREMOVECOMPLETE) {
                HandleVolumeDeviceChange(
                    hwnd,
                    wParam,
                    lParam,
                    thisPcPidl);
            }
            return TRUE;

        case WM_TIMER:
            if (wParam == kRetryTimerId) {
                ProcessPendingInsertions(
                    hwnd,
                    thisPcPidl);
                return 0;
            }
            break;

        case kMsgInitialScan:
            InitialScan(thisPcPidl);
            return 0;

        case kMsgRefreshThisPc:
            RefreshThisPc(thisPcPidl);
            return 0;

        case kMsgShutdown:
            KillTimer(hwnd, kRetryTimerId);
            RefreshThisPc(thisPcPidl);
            DestroyWindow(hwnd);
            return 0;

        case WM_CLOSE:
            KillTimer(hwnd, kRetryTimerId);
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY:
            RemoveWindowSubclass(
                hwnd,
                NotificationWindowSubclassProc,
                1);
            g_notificationWindow.store(nullptr, std::memory_order_release);
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
    MSG msg = {};
    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE);

    PIDLIST_ABSOLUTE thisPcPidl = nullptr;

    HRESULT hr = SHGetKnownFolderIDList(
        FOLDERID_ComputerFolder,
        0,
        nullptr,
        &thisPcPidl);

    if (FAILED(hr) || !thisPcPidl) {
        Wh_Log(
            L"SHGetKnownFolderIDList failed: 0x%08X",
            hr);
        g_notificationThreadReady.store(false, std::memory_order_release);
        SetEvent(g_notificationReadyEvent);
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
        g_notificationThreadReady.store(false, std::memory_order_release);
        SetEvent(g_notificationReadyEvent);
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
        g_notificationThreadReady.store(false, std::memory_order_release);
        SetEvent(g_notificationReadyEvent);
        return 1;
    }

    g_notificationWindow.store(hwnd, std::memory_order_release);
    g_notificationThreadReady.store(true, std::memory_order_release);
    SetEvent(g_notificationReadyEvent);

    PostMessageW(
        hwnd,
        kMsgInitialScan,
        0,
        0);

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoTaskMemFree(thisPcPidl);
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
                towupper(letters.get()[i]));

        if (ch >= L'A' &&
            ch <= L'Z') {
            mask |= LetterBit(ch);
        }
    }

    return mask
        ? mask
        : 0x03FFFFFFu;
}

static bool HookDrivesViewShouldShow() {
    HMODULE shell32 =
        LoadLibraryExW(
            L"shell32.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!shell32) {
        Wh_Log(
            L"Failed to load shell32.dll: %u",
            GetLastError());
        return false;
    }

    const WindhawkUtils::SYMBOL_HOOK hooks[] = {
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
        hooks,
        ARRAYSIZE(hooks));
}

static void StopNotificationThread(bool restoreView) {
    if (!g_notificationThread) {
        return;
    }

    HWND hwnd =
        g_notificationWindow.load(
            std::memory_order_acquire);

    if (hwnd) {
        if (restoreView) {
            PostMessageW(
                hwnd,
                kMsgShutdown,
                0,
                0);
        } else {
            PostMessageW(
                hwnd,
                WM_CLOSE,
                0,
                0);
        }
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
    g_pendingInsertMask = 0;
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

    g_enabled.store(
        true,
        std::memory_order_release);

    g_notificationReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_notificationReadyEvent) {
        Wh_Log(
            L"CreateEvent failed: %u",
            GetLastError());
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
            L"CreateThread failed: %u",
            GetLastError());
        CloseHandle(
            g_notificationReadyEvent);
        g_notificationReadyEvent =
            nullptr;
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
        return FALSE;
    }

    if (!HookDrivesViewShouldShow()) {
        Wh_Log(
            L"Failed to hook "
            L"CDrivesViewCallback::ShouldShow");
        StopNotificationThread(false);
        return FALSE;
    }

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

    return TRUE;
}

void Wh_ModSettingsChanged() {
    g_managedMask.store(
        LoadManagedMask(),
        std::memory_order_release);

    HWND hwnd =
        g_notificationWindow.load(
            std::memory_order_acquire);

    if (hwnd) {
        PostMessageW(
            hwnd,
            kMsgInitialScan,
            0,
            0);

        // A settings change can alter visibility even when media state
        // itself did not change.
        PostMessageW(
            hwnd,
            kMsgRefreshThisPc,
            0,
            0);
    }
}

void Wh_ModUninit() {
    Wh_Log(
        L"Uninitializing Hide Empty Optical Drives");

    // Make the hook pass through immediately. The notification thread then
    // refreshes This PC before it exits, restoring items hidden by the mod.
    g_enabled.store(
        false,
        std::memory_order_release);

    StopNotificationThread(true);
}
