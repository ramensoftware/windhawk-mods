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
// @compilerOptions -lshell32 -lshlwapi -lcomctl32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Empty Optical Drives

Hides empty optical drives from **This PC** while leaving the device and its
drive letter fully available to Windows.

![Demo](https://raw.githubusercontent.com/Solomag/windhawk-assets/main/hide-empty-optical-drives/demo.gif)

Unlike the built-in "Hide empty drives" option, this mod is intended for
optical CD/DVD/BD drives that still remain visible in This PC when empty.

The mod only affects the main **This PC** view in File Explorer
(`explorer.exe`). The navigation pane, address-bar dropdown, file dialogs and
third-party file managers are not modified.

When an empty drive is hidden, Explorer's context-menu **Eject** command is no
longer available for it. Use the drive's physical eject button instead.

### Compatibility

Tested on:
- Windows 10 22H2 (build 19045)
- Windows 11 25H2

Verified behavior includes hiding an empty optical drive, showing it after media
insertion, hiding it again after eject, and keeping the correct state after
restarting Explorer.

Media detection is event-driven. There is no permanent polling. After Windows
reports media insertion, a background worker retries briefly while an optical
disc spins up.

Detection behavior:
- media confirmed present -> show;
- no media, or a drive that remains not ready after the spin-up grace window -> hide;
- other inconclusive probe failures -> show (fail open).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- driveLetters: ""
  $name: Optical drive letters
  $description: >-
    Optional drive letters to manage, for example G or DE.
    Spaces and separators (comma, semicolon, colon) are allowed.
    Leave empty to manage all optical drives.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dbt.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winioctl.h>
#include <atomic>
#include <cwctype>

enum class MediaState : LONG {
    Unknown = 0,
    Empty = 1,
    Present = 2,
};

enum class ProbeResult {
    Unknown,
    NotReady,
    Empty,
    Present,
};

constexpr DWORD kAllDriveBits = (1u << 26) - 1;
constexpr DWORD kRetryIntervalMs = 500;
constexpr int kMaxRetryAttempts = 20;

constexpr UINT kMsgRefreshThisPc = WM_APP + 1;
constexpr UINT kMsgStop = WM_APP + 2;

std::atomic<DWORD> g_managedMask{kAllDriveBits};
std::atomic<DWORD> g_opticalMask{0};
std::atomic<MediaState> g_mediaState[26]{};

std::atomic<DWORD> g_arrivalRequestMask{0};
std::atomic<DWORD> g_removalRequestMask{0};
std::atomic<bool> g_initialScanRequested{false};
std::atomic<bool> g_initialScanAllowGrace{false};

HANDLE g_notificationThread = nullptr;
DWORD g_notificationThreadId = 0;
std::atomic<HWND> g_notificationWindow{nullptr};
std::atomic<PIDLIST_ABSOLUTE> g_thisPcPidl{nullptr};
std::atomic<bool> g_refreshPending{false};
std::atomic<bool> g_notificationStopRequested{false};

HANDLE g_workerThread = nullptr;
HANDLE g_workerWakeEvent = nullptr;
HANDLE g_workerStopEvent = nullptr;

using CDrivesViewCallback_ShouldShow_t = HRESULT(
    STDMETHODCALLTYPE*)(void*, IShellFolder*, LPCITEMIDLIST, LPCITEMIDLIST);

CDrivesViewCallback_ShouldShow_t CDrivesViewCallback_ShouldShow_Original =
    nullptr;

static bool IsWorkerStopRequested() {
    return g_workerStopEvent &&
           WaitForSingleObject(g_workerStopEvent, 0) == WAIT_OBJECT_0;
}

static DWORD LetterBit(WCHAR letter) {
    if (letter < L'A' || letter > L'Z') {
        return 0;
    }

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

    return g_mediaState[letter - L'A'].load(std::memory_order_acquire);
}

static bool SetCachedMediaState(WCHAR letter, MediaState state) {
    if (letter < L'A' || letter > L'Z') {
        return false;
    }

    MediaState old =
        g_mediaState[letter - L'A'].exchange(state, std::memory_order_acq_rel);

    if (old == state) {
        return false;
    }

    Wh_Log(L"%c: state %d -> %d", letter, static_cast<LONG>(old),
           static_cast<LONG>(state));
    return true;
}

static bool SetOpticalDrivePresent(WCHAR letter, bool optical) {
    if (letter < L'A' || letter > L'Z') {
        return false;
    }

    DWORD bit = LetterBit(letter);
    DWORD oldMask = optical
                         ? g_opticalMask.fetch_or(
                               bit, std::memory_order_acq_rel)
                         : g_opticalMask.fetch_and(
                               ~bit, std::memory_order_acq_rel);

    return ((oldMask & bit) != 0) != optical;
}

static ProbeResult ProbeOpticalMediaState(WCHAR letter) {
    if (IsWorkerStopRequested()) {
        return ProbeResult::Unknown;
    }

    WCHAR root[4];
    MakeRootPath(letter, root);

    if (GetDriveTypeW(root) != DRIVE_CDROM) {
        return ProbeResult::Unknown;
    }

    WCHAR devicePath[] = L"\\\\.\\X:";
    devicePath[4] = letter;

    HANDLE device = CreateFileW(
        devicePath, FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0,
        nullptr);

    if (device == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();

        if (error == ERROR_NO_MEDIA_IN_DRIVE) {
            return ProbeResult::Empty;
        }

        if (error == ERROR_NOT_READY) {
            return ProbeResult::NotReady;
        }

        if (error == ERROR_OPERATION_ABORTED && IsWorkerStopRequested()) {
            return ProbeResult::Unknown;
        }

        Wh_Log(L"%c: unable to open device, error=%u", letter, error);
        return ProbeResult::Unknown;
    }

    if (IsWorkerStopRequested()) {
        CloseHandle(device);
        return ProbeResult::Unknown;
    }

    DWORD bytesReturned = 0;
    BOOL present = DeviceIoControl(
        device, IOCTL_STORAGE_CHECK_VERIFY2, nullptr, 0, nullptr, 0,
        &bytesReturned, nullptr);

    DWORD error = present ? ERROR_SUCCESS : GetLastError();

    CloseHandle(device);

    if (!present) {
        if (error == ERROR_NO_MEDIA_IN_DRIVE) {
            return ProbeResult::Empty;
        }

        if (error == ERROR_NOT_READY) {
            return ProbeResult::NotReady;
        }

        if (error == ERROR_OPERATION_ABORTED && IsWorkerStopRequested()) {
            return ProbeResult::Unknown;
        }

        Wh_Log(L"%c: media probe inconclusive, error=%u", letter, error);
        return ProbeResult::Unknown;
    }

    Wh_Log(L"%c: media present", letter);
    return ProbeResult::Present;
}

static void RequestThisPcRefresh() {
    // Set the pending bit before publishing/looking up the window. This avoids
    // a lost-wakeup race if the notification thread becomes ready at exactly
    // the same time as the worker requests the first refresh.
    g_refreshPending.store(true, std::memory_order_release);

    HWND hwnd = g_notificationWindow.load(std::memory_order_acquire);
    if (!hwnd) {
        return;
    }

    // Multiple state changes can be coalesced into one folder update.
    if (!g_refreshPending.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    if (!PostMessageW(hwnd, kMsgRefreshThisPc, 0, 0)) {
        // The window can disappear during teardown between the load and post.
        g_refreshPending.store(true, std::memory_order_release);
    }
}

static bool GetDriveLetterFromItem(IShellFolder* folder,
                                   LPCITEMIDLIST pidlItem,
                                   WCHAR* letter) {
    if (!folder || !pidlItem || !letter) {
        return false;
    }

    STRRET strret = {};

    if (FAILED(folder->GetDisplayNameOf(pidlItem, SHGDN_FORPARSING, &strret))) {
        return false;
    }

    WCHAR parsingName[MAX_PATH] = {};

    if (FAILED(StrRetToBufW(&strret, pidlItem, parsingName,
                            ARRAYSIZE(parsingName)))) {
        return false;
    }

    WCHAR candidate = static_cast<WCHAR>(towupper(parsingName[0]));

    if (candidate < L'A' || candidate > L'Z' || parsingName[1] != L':' ||
        !((parsingName[2] == L'\0') ||
          (parsingName[2] == L'\\' && parsingName[3] == L'\0'))) {
        return false;
    }

    *letter = candidate;
    return true;
}

static HRESULT STDMETHODCALLTYPE
CDrivesViewCallback_ShouldShow_Hook(void* self,
                                    IShellFolder* folder,
                                    LPCITEMIDLIST pidlFolder,
                                    LPCITEMIDLIST pidlItem) {
    HRESULT hr = CDrivesViewCallback_ShouldShow_Original(self, folder,
                                                         pidlFolder, pidlItem);

    if (FAILED(hr) || hr == S_FALSE ||
        g_opticalMask.load(std::memory_order_acquire) == 0) {
        return hr;
    }

    WCHAR letter = 0;

    if (!GetDriveLetterFromItem(folder, pidlItem, &letter)) {
        return hr;
    }

    if (!IsManagedLetter(letter) || !IsCachedOpticalDrive(letter)) {
        return hr;
    }

    return GetCachedMediaState(letter) == MediaState::Empty ? S_FALSE : hr;
}

static void QueueArrivalMask(DWORD mask) {
    if (!mask) {
        return;
    }

    g_arrivalRequestMask.fetch_or(mask, std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static void QueueRemovalMask(DWORD mask) {
    if (!mask) {
        return;
    }

    g_removalRequestMask.fetch_or(mask, std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static void QueueInitialScan(bool allowGrace = false) {
    if (allowGrace) {
        g_initialScanAllowGrace.store(true, std::memory_order_release);
    }

    g_initialScanRequested.store(true, std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static bool ProcessInitialScan(DWORD* retryMask,
                               DWORD* graceRetryMask,
                               DWORD* arrivalRetryMask,
                               int (&retryAttempts)[26],
                               bool allowGrace) {
    bool changed = false;
    DWORD managedMask = g_managedMask.load(std::memory_order_acquire);
    DWORD logicalDrives = GetLogicalDrives();

    if (logicalDrives == 0) {
        Wh_Log(L"GetLogicalDrives returned no drives; falling back to full scan");
        logicalDrives = kAllDriveBits;
    }

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        if (IsWorkerStopRequested()) {
            return changed;
        }

        DWORD bit = LetterBit(letter);
        int index = letter - L'A';

        if (!(managedMask & bit) || !(logicalDrives & bit)) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            changed |= SetOpticalDrivePresent(letter, false);
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool optical = GetDriveTypeW(root) == DRIVE_CDROM;
        changed |= SetOpticalDrivePresent(letter, optical);

        if (!optical) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        // Arrival can be queued concurrently after WorkerThreadProc takes its
        // current arrival-mask snapshot. Let the event-specific path handle
        // that letter instead of racing it with a broad device-tree scan.
        if (g_arrivalRequestMask.load(std::memory_order_acquire) & bit) {
            continue;
        }

        // An existing bounded grace window owns this drive's probe cadence and
        // retry budget. Neither a broad device-tree rescan nor a repeated
        // startup/resume scan should cancel or restart it.
        if (*graceRetryMask & bit) {
            continue;
        }

        // A non-grace retry (currently used after an inconclusive removal
        // probe) also owns its retry budget. A generic DBT_DEVNODES_CHANGED
        // rescan must not replace that bounded retry series with one probe.
        if (!allowGrace && (*retryMask & bit)) {
            continue;
        }

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (IsWorkerStopRequested()) {
            return changed;
        }

        if (!allowGrace) {
            // A generic device-tree change gets exactly one probe. In
            // particular, a settled empty/not-ready drive must not be poked
            // every 500 ms for ten seconds because an unrelated USB/Bluetooth
            // device changed elsewhere in the system.
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;

            if (result == ProbeResult::Present) {
                changed |= SetCachedMediaState(letter, MediaState::Present);
            } else if (result == ProbeResult::Empty) {
                changed |= SetCachedMediaState(letter, MediaState::Empty);
            } else if (result == ProbeResult::NotReady) {
                // ERROR_NOT_READY is inconclusive. Some optical drives return
                // it while spinning up or waking from idle even with media
                // inserted. Do not hide a previously visible disc after a
                // single generic device-tree probe.
                *retryMask |= bit;
            } else {
                changed |= SetCachedMediaState(letter, MediaState::Unknown);
            }

            continue;
        }

        // Startup/resume can race disc spin-up. These scans are the cases where
        // a bounded retry window is useful.
        if (result == ProbeResult::Present) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Present);
        } else if (result == ProbeResult::Empty ||
                   result == ProbeResult::NotReady) {
            *retryMask |= bit;
            *graceRetryMask |= bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Empty);
        } else {
            // An inconclusive probe fails open, but retry briefly so a
            // transient startup/resume failure can still settle.
            *retryMask |= bit;
            *graceRetryMask |= bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
        }
    }

    return changed;
}

static bool ProcessRemovalMask(DWORD mask,
                               DWORD* retryMask,
                               DWORD* graceRetryMask,
                               DWORD* arrivalRetryMask,
                               int (&retryAttempts)[26]) {
    bool changed = false;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        if (IsWorkerStopRequested()) {
            return changed;
        }

        DWORD bit = LetterBit(letter);

        if (!(mask & bit) || !IsManagedLetter(letter)) {
            continue;
        }

        int index = letter - L'A';
        *graceRetryMask &= ~bit;
        *arrivalRetryMask &= ~bit;

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool wasOptical = IsCachedOpticalDrive(letter);
        bool stillOptical = GetDriveTypeW(root) == DRIVE_CDROM;

        if (!stillOptical) {
            if (wasOptical) {
                Wh_Log(L"%c: optical drive removed", letter);
            }
            *retryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetOpticalDrivePresent(letter, false);
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        Wh_Log(L"%c: volume removal, drive remains optical", letter);
        changed |= SetOpticalDrivePresent(letter, true);

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (IsWorkerStopRequested()) {
            return changed;
        }

        if (result == ProbeResult::Present) {
            *retryMask &= ~bit;
            changed |= SetCachedMediaState(letter, MediaState::Present);
        } else if (result == ProbeResult::Empty ||
                   result == ProbeResult::NotReady) {
            // Removal is strong evidence that the medium is gone.
            *retryMask &= ~bit;
            changed |= SetCachedMediaState(letter, MediaState::Empty);
        } else {
            // Preserve fail-open semantics: an inconclusive probe must not
            // hide the drive.
            *retryMask |= bit;
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
        }

        retryAttempts[index] = 0;
    }

    return changed;
}

static bool ProcessArrivalMask(DWORD mask,
                               DWORD* retryMask,
                               DWORD* graceRetryMask,
                               DWORD* arrivalRetryMask,
                               int (&retryAttempts)[26]) {
    bool changed = false;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        if (IsWorkerStopRequested()) {
            return changed;
        }

        DWORD bit = LetterBit(letter);

        if (!(mask & bit) || !IsManagedLetter(letter)) {
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        if (GetDriveTypeW(root) != DRIVE_CDROM) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[letter - L'A'] = 0;
            changed |= SetOpticalDrivePresent(letter, false);
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        changed |= SetOpticalDrivePresent(letter, true);

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (IsWorkerStopRequested()) {
            return changed;
        }

        if (result == ProbeResult::Present) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[letter - L'A'] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Present);
            continue;
        }

        if (result == ProbeResult::Unknown) {
            // A genuinely inconclusive query fails open.
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
        }

        // NOT_READY and NO_MEDIA can both be transient immediately after an
        // arrival event. Keep the previous cached state and retry for the full
        // media-ready window to avoid flicker or premature hiding.
        bool alreadyArrivalRetrying = (*arrivalRetryMask & bit) != 0;

        *retryMask |= bit;
        *graceRetryMask |= bit;
        *arrivalRetryMask |= bit;

        if (!alreadyArrivalRetrying) {
            retryAttempts[letter - L'A'] = 0;
        }
    }

    return changed;
}

static bool ProcessRetryMask(DWORD* retryMask,
                             DWORD* graceRetryMask,
                             DWORD* arrivalRetryMask,
                             int (&retryAttempts)[26]) {
    bool changed = false;
    DWORD pending = *retryMask;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        if (IsWorkerStopRequested()) {
            return changed;
        }

        DWORD bit = LetterBit(letter);

        if (!(pending & bit)) {
            continue;
        }

        int index = letter - L'A';

        if (!IsManagedLetter(letter)) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        if (GetDriveTypeW(root) != DRIVE_CDROM) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetOpticalDrivePresent(letter, false);
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (IsWorkerStopRequested()) {
            return changed;
        }
        bool graceRetry = (*graceRetryMask & bit) != 0;

        if (result == ProbeResult::Present) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Present);
            continue;
        }

        if (result == ProbeResult::Empty && !graceRetry) {
            *retryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;
            changed |= SetCachedMediaState(letter, MediaState::Empty);
            continue;
        }

        if (result == ProbeResult::Unknown && !graceRetry) {
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
        }

        if (++retryAttempts[index] >= kMaxRetryAttempts) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            *arrivalRetryMask &= ~bit;
            retryAttempts[index] = 0;

            if (result == ProbeResult::Empty ||
                result == ProbeResult::NotReady) {
                // Some optical drives report ERROR_NOT_READY while genuinely
                // empty. After the full spin-up grace window, treat a
                // persistent NOT_READY the same as NO_MEDIA so an empty drive
                // doesn't remain visible forever.
                changed |= SetCachedMediaState(letter, MediaState::Empty);
            } else {
                changed |= SetCachedMediaState(letter, MediaState::Unknown);
            }

            Wh_Log(
                L"%c: media-ready retry window expired; "
                L"using the latest settled state",
                letter);
        }
    }

    return changed;
}

static DWORD WINAPI WorkerThreadProc(void*) {
    DWORD retryMask = 0;
    DWORD graceRetryMask = 0;
    DWORD arrivalRetryMask = 0;
    int retryAttempts[26] = {};
    ULONGLONG nextRetryTick = 0;

    HANDLE waits[] = {
        g_workerStopEvent,
        g_workerWakeEvent,
    };

    for (;;) {
        DWORD timeout = INFINITE;

        if (retryMask) {
            ULONGLONG now = GetTickCount64();

            if (nextRetryTick <= now) {
                timeout = 0;
            } else {
                timeout = static_cast<DWORD>(nextRetryTick - now);
            }
        }

        DWORD waitResult =
            WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, timeout);

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Worker wait failed: %u", GetLastError());
            break;
        }

        bool refresh = false;

        DWORD removalMask =
            g_removalRequestMask.exchange(0, std::memory_order_acq_rel);

        if (removalMask) {
            refresh |= ProcessRemovalMask(
                removalMask, &retryMask, &graceRetryMask,
                &arrivalRetryMask, retryAttempts);
        }

        if (IsWorkerStopRequested()) {
            break;
        }

        DWORD arrivalMask =
            g_arrivalRequestMask.exchange(0, std::memory_order_acq_rel);

        if (arrivalMask) {
            refresh |= ProcessArrivalMask(
                arrivalMask, &retryMask, &graceRetryMask,
                &arrivalRetryMask, retryAttempts);
        }

        if (IsWorkerStopRequested()) {
            break;
        }

        // Event-specific media arrival/removal handling must run before a
        // broad device-tree scan. In particular, arrival establishes its grace
        // window first, so a simultaneous DBT_DEVNODES_CHANGED can't settle a
        // spinning-up disc as Empty.
        if (g_initialScanRequested.exchange(false, std::memory_order_acq_rel)) {
            bool allowGrace = g_initialScanAllowGrace.exchange(
                false, std::memory_order_acq_rel);

            refresh |= ProcessInitialScan(&retryMask, &graceRetryMask,
                                           &arrivalRetryMask, retryAttempts,
                                           allowGrace);
        }

        if (IsWorkerStopRequested()) {
            break;
        }

        if (retryMask) {
            ULONGLONG now = GetTickCount64();

            if (!nextRetryTick) {
                nextRetryTick = now + kRetryIntervalMs;
            }

            if (now >= nextRetryTick) {
                refresh |= ProcessRetryMask(
                    &retryMask, &graceRetryMask, &arrivalRetryMask,
                    retryAttempts);

                nextRetryTick =
                    retryMask ? GetTickCount64() + kRetryIntervalMs : 0;
            }
        } else {
            nextRetryTick = 0;
        }

        if (IsWorkerStopRequested()) {
            break;
        }

        if (refresh) {
            RequestThisPcRefresh();
        }
    }

    return 0;
}

static PIDLIST_ABSOLUTE AcquireThisPcPidl() {
    PIDLIST_ABSOLUTE thisPcPidl = nullptr;
    HRESULT hr = SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr,
                                        &thisPcPidl);

    if (SUCCEEDED(hr) && thisPcPidl) {
        return thisPcPidl;
    }

    Wh_Log(L"SHGetKnownFolderIDList failed: 0x%08X; trying CSIDL_DRIVES", hr);

    PIDLIST_ABSOLUTE legacyPidl = nullptr;
    hr = SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &legacyPidl);

    if (SUCCEEDED(hr) && legacyPidl) {
        return legacyPidl;
    }

    Wh_Log(L"Unable to resolve This PC PIDL: 0x%08X", hr);
    return nullptr;
}

static void NotifyThisPcUpdated(PIDLIST_ABSOLUTE thisPcPidl) {
    if (!thisPcPidl) {
        return;
    }

    // Let Explorer refresh its own This PC views on their owning threads.
    // Unlike the previous IShellWindows/IShellView COM walk, this doesn't make
    // the device-notification thread synchronously enter another apartment.
    SHChangeNotify(SHCNE_UPDATEDIR,
                   SHCNF_IDLIST | SHCNF_FLUSHNOWAIT,
                   thisPcPidl,
                   nullptr);
}

static LRESULT CALLBACK NotificationWindowSubclassProc(HWND hwnd,
                                                       UINT message,
                                                       WPARAM wParam,
                                                       LPARAM lParam,
                                                       UINT_PTR,
                                                       DWORD_PTR refData) {
    auto thisPcPidl = reinterpret_cast<PIDLIST_ABSOLUTE>(refData);

    switch (message) {
        case WM_DEVICECHANGE:
            if (wParam == DBT_DEVNODES_CHANGED) {
                // Device-tree changes are broadcast even when an optical drive
                // has no mounted volume. Re-scan so hot-plugged or removed
                // external optical drives are reflected in the cache.
                QueueInitialScan();
                break;
            }

            if ((wParam == DBT_DEVICEARRIVAL ||
                 wParam == DBT_DEVICEREMOVECOMPLETE) &&
                lParam) {
                auto* header =
                    reinterpret_cast<const DEV_BROADCAST_HDR*>(lParam);

                if (header->dbch_size >= sizeof(DEV_BROADCAST_VOLUME) &&
                    header->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                    auto* volume =
                        reinterpret_cast<const DEV_BROADCAST_VOLUME*>(lParam);

                    DWORD mask = volume->dbcv_unitmask;

                    if (wParam == DBT_DEVICEARRIVAL) {
                        QueueArrivalMask(mask);
                    } else {
                        QueueRemovalMask(mask);
                    }
                }
            }

            // Don't swallow any WM_DEVICECHANGE subtype.
            break;

        case WM_POWERBROADCAST:
            if (wParam == PBT_APMRESUMEAUTOMATIC) {
                QueueInitialScan(true);
            }
            break;

        case kMsgRefreshThisPc:
            NotifyThisPcUpdated(thisPcPidl);
            return 0;

        case kMsgStop:
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, NotificationWindowSubclassProc, 1);
            g_notificationWindow.store(nullptr, std::memory_order_release);
            PostQuitMessage(0);
            break;
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

static DWORD WINAPI NotificationThreadProc(void*) {
    // Establish the thread message queue immediately. StopNotificationThread
    // can then queue WM_QUIT even if the window hasn't been created yet.
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Build the This PC PIDL before creating the window. If this shell lookup
    // ever stalls, there is no window yet to stall WM_DEVICECHANGE broadcasts,
    // and Wh_ModInit doesn't wait for this thread.
    PIDLIST_ABSOLUTE thisPcPidl = AcquireThisPcPidl();

    if (!thisPcPidl) {
        return 1;
    }

    g_thisPcPidl.store(thisPcPidl, std::memory_order_release);

    if (g_notificationStopRequested.load(std::memory_order_acquire)) {
        return 0;
    }

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, L"STATIC",
                                L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                nullptr, nullptr);

    if (!hwnd) {
        Wh_Log(L"Notification window creation failed: %u", GetLastError());
        return 1;
    }

    if (!SetWindowSubclass(hwnd, NotificationWindowSubclassProc, 1,
                           reinterpret_cast<DWORD_PTR>(thisPcPidl))) {
        Wh_Log(L"Notification window subclass failed: %u", GetLastError());
        DestroyWindow(hwnd);
        return 1;
    }

    g_notificationWindow.store(hwnd, std::memory_order_release);

    if (g_notificationStopRequested.load(std::memory_order_acquire)) {
        DestroyWindow(hwnd);
        return 0;
    }

    if (g_refreshPending.exchange(false, std::memory_order_acq_rel) &&
        !PostMessageW(hwnd, kMsgRefreshThisPc, 0, 0)) {
        g_refreshPending.store(true, std::memory_order_release);
    }

    for (;;) {
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);

        if (result == 0) {
            break;
        }

        if (result == -1) {
            Wh_Log(L"GetMessageW failed: %u", GetLastError());
            break;
        }

        DispatchMessageW(&msg);
    }

    if (HWND remaining =
            g_notificationWindow.load(std::memory_order_acquire)) {
        DestroyWindow(remaining);
    }

    return 0;
}

static bool TryLoadManagedMask(DWORD* mask) {
    if (!mask) {
        return false;
    }

    WindhawkUtils::StringSetting letters =
        WindhawkUtils::StringSetting::make(L"driveLetters");

    const WCHAR* value = letters.get();

    if (!value[0]) {
        *mask = kAllDriveBits;
        return true;
    }

    DWORD parsedMask = 0;

    for (size_t i = 0; value[i]; i++) {
        WCHAR ch = static_cast<WCHAR>(towupper(value[i]));

        if (ch == L' ' || ch == L',' || ch == L';' || ch == L':') {
            continue;
        }

        if (ch < L'A' || ch > L'Z') {
            Wh_Log(L"Ignoring invalid driveLetters character: %c", ch);
            continue;
        }

        parsedMask |= LetterBit(ch);
    }

    if (!parsedMask) {
        Wh_Log(L"Invalid driveLetters setting: no drive letters found");
        return false;
    }

    *mask = parsedMask;
    return true;
}

static bool HookDrivesViewShouldShow() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");

    if (!shell32) {
        Wh_Log(L"shell32.dll isn't loaded");
        return false;
    }

    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {{L"public: virtual long __cdecl "
          L"CDrivesViewCallback::ShouldShow("
          L"struct IShellFolder *,"
          L"struct _ITEMIDLIST_ABSOLUTE const *,"
          L"struct _ITEMID_CHILD const __unaligned *)"},
         &CDrivesViewCallback_ShouldShow_Original,
         CDrivesViewCallback_ShouldShow_Hook,
         false},
    };

    return WindhawkUtils::HookSymbols(shell32, shell32DllHooks,
                                      ARRAYSIZE(shell32DllHooks));
}

static void StopWorkerThread() {
    if (!g_workerThread) {
        return;
    }

    SetEvent(g_workerStopEvent);
    SetEvent(g_workerWakeEvent);

    // Keep cancelling until the worker has actually exited. A single
    // CancelSynchronousIo only affects I/O that is pending at that instant;
    // without the stop checks in the probe/scan loops the worker could
    // otherwise start another blocking drive operation immediately after it.
    ULONGLONG waitStarted = GetTickCount64();
    ULONGLONG nextWarning = waitStarted + 5000;

    for (;;) {
        CancelSynchronousIo(g_workerThread);

        DWORD waitResult = WaitForSingleObject(g_workerThread, 100);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Worker join failed: %u", GetLastError());
            WaitForSingleObject(g_workerThread, INFINITE);
            break;
        }

        ULONGLONG now = GetTickCount64();
        if (now >= nextWarning) {
            Wh_Log(L"Worker thread is still stopping after %llu ms; "
                   L"a drive I/O request may be stuck",
                   static_cast<unsigned long long>(now - waitStarted));
            nextWarning = now + 5000;
        }
    }

    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

static void StopNotificationThread(bool restoreView) {
    if (g_notificationThread) {
        g_notificationStopRequested.store(true, std::memory_order_release);

        HWND hwnd = g_notificationWindow.load(std::memory_order_acquire);
        bool stopPosted = false;

        if (hwnd) {
            stopPosted = PostMessageW(hwnd, kMsgStop, 0, 0) != FALSE;
        }

        if (!stopPosted && g_notificationThreadId &&
            WaitForSingleObject(g_notificationThread, 0) == WAIT_TIMEOUT) {
            // Usually this means the thread is still before window creation.
            // Best-effort cancellation avoids waiting on synchronous shell I/O;
            // WM_QUIT is also queued once the thread has a message queue.
            CancelSynchronousIo(g_notificationThread);
            PostThreadMessageW(g_notificationThreadId, WM_QUIT, 0, 0);
        }

        ULONGLONG waitStarted = GetTickCount64();
        ULONGLONG nextWarning = waitStarted + 5000;

        for (;;) {
            DWORD waitResult = WaitForSingleObject(g_notificationThread, 250);

            if (waitResult == WAIT_OBJECT_0) {
                break;
            }

            if (waitResult == WAIT_FAILED) {
                Wh_Log(L"Notification-thread join failed: %u", GetLastError());
                WaitForSingleObject(g_notificationThread, INFINITE);
                break;
            }

            // If the thread is still resolving the shell PIDL, this is
            // best-effort only: CancelSynchronousIo won't cancel every kind of
            // shell/RPC wait, but retrying it can abort cancellable I/O.
            CancelSynchronousIo(g_notificationThread);

            ULONGLONG now = GetTickCount64();
            if (now >= nextWarning) {
                Wh_Log(L"Notification thread is still stopping after %llu ms; "
                       L"shell PIDL resolution may be stuck",
                       static_cast<unsigned long long>(now - waitStarted));
                nextWarning = now + 5000;
            }
        }

        CloseHandle(g_notificationThread);
        g_notificationThread = nullptr;
        g_notificationThreadId = 0;
        g_notificationWindow.store(nullptr, std::memory_order_release);
    }

    // Restoration is intentionally centralized here, after the notification
    // thread is gone. This covers normal shutdown, early thread failure, and
    // the window-publication/WM_QUIT race with one identical path.
    PIDLIST_ABSOLUTE thisPcPidl =
        g_thisPcPidl.exchange(nullptr, std::memory_order_acq_rel);
    PIDLIST_ABSOLUTE temporaryPidl = nullptr;

    if (restoreView) {
        if (!thisPcPidl) {
            temporaryPidl = AcquireThisPcPidl();
        }

        NotifyThisPcUpdated(thisPcPidl ? thisPcPidl : temporaryPidl);
    }

    if (temporaryPidl) {
        ILFree(temporaryPidl);
    }

    if (thisPcPidl) {
        ILFree(thisPcPidl);
    }
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
    Wh_Log(L"Initializing Hide Empty Optical Drives");

    g_opticalMask.store(0, std::memory_order_relaxed);
    g_arrivalRequestMask.store(0, std::memory_order_relaxed);
    g_removalRequestMask.store(0, std::memory_order_relaxed);
    g_initialScanRequested.store(false, std::memory_order_relaxed);
    g_initialScanAllowGrace.store(false, std::memory_order_relaxed);
    g_refreshPending.store(false, std::memory_order_relaxed);
    g_notificationStopRequested.store(false, std::memory_order_relaxed);
    g_thisPcPidl.store(nullptr, std::memory_order_relaxed);

    for (auto& state : g_mediaState) {
        state.store(MediaState::Unknown, std::memory_order_relaxed);
    }

    DWORD managedMask = kAllDriveBits;

    if (TryLoadManagedMask(&managedMask)) {
        g_managedMask.store(managedMask, std::memory_order_release);
    } else {
        Wh_Log(
            L"Invalid driveLetters setting at startup; "
            L"keeping the default configuration");
    }

    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_workerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_workerWakeEvent || !g_workerStopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());

        CloseWorkerObjects();
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"CreateThread(worker) failed: %u", GetLastError());
        CloseWorkerObjects();
        return FALSE;
    }

    if (!HookDrivesViewShouldShow()) {
        Wh_Log(
            L"Failed to hook "
            L"CDrivesViewCallback::ShouldShow");

        StopWorkerThread();
        CloseWorkerObjects();
        return FALSE;
    }

    // Don't wait for notification-window initialization here. Wh_ModInit can
    // run on Explorer's main thread during process startup; an unbounded (or a
    // teardown-followed) wait would make a shell-side initialization problem
    // prevent Explorer from starting. Refresh requests are coalesced until the
    // notification window becomes available.
    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc,
                                        nullptr, 0, &g_notificationThreadId);

    if (!g_notificationThread) {
        Wh_Log(L"CreateThread(notification) failed: %u", GetLastError());
        StopWorkerThread();
        CloseWorkerObjects();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Explorer startup can race optical-media spin-up, so the initial scan gets
    // the bounded grace window. Refresh only occurs if the cached state changes.
    QueueInitialScan(true);
}

void Wh_ModSettingsChanged() {
    DWORD managedMask = 0;

    if (!TryLoadManagedMask(&managedMask)) {
        Wh_Log(
            L"Invalid driveLetters setting; keeping the previous "
            L"valid configuration");
        return;
    }

    g_managedMask.store(managedMask, std::memory_order_release);

    // A settings change is rare and can happen while an optical drive is
    // waking up or busy. Give this rescan the same bounded grace window as
    // startup/resume so a transient NOT_READY doesn't hide inserted media.
    QueueInitialScan(true);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Hide Empty Optical Drives");

    // Windhawk calls Wh_ModUninit after removing the hooks. Stop device
    // probing first, then send one final asynchronous shell update after
    // shutting down the notification window so previously hidden items are
    // re-enumerated without the ShouldShow hook.
    StopWorkerThread();
    StopNotificationThread(true);
    CloseWorkerObjects();
}
