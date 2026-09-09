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

Detection is conservative:
- media confirmed present -> show;
- media confirmed absent -> hide;
- inconclusive device state -> show (fail open).
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
std::atomic<MediaState> g_mediaState[26] = {
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown, MediaState::Unknown,
    MediaState::Unknown, MediaState::Unknown,
};

std::atomic<DWORD> g_arrivalRequestMask{0};
std::atomic<DWORD> g_removalRequestMask{0};
std::atomic<bool> g_initialScanRequested{false};
std::atomic<LONG> g_hookCallCount{0};

struct HookCallGuard {
    HookCallGuard() {
        g_hookCallCount.fetch_add(1, std::memory_order_acq_rel);
    }

    ~HookCallGuard() {
        g_hookCallCount.fetch_sub(1, std::memory_order_acq_rel);
    }
};

HANDLE g_notificationThread = nullptr;
DWORD g_notificationThreadId = 0;
std::atomic<HWND> g_notificationWindow{nullptr};
HANDLE g_notificationReadyEvent = nullptr;
std::atomic<bool> g_notificationReady{false};

HANDLE g_workerThread = nullptr;
HANDLE g_workerWakeEvent = nullptr;
HANDLE g_workerStopEvent = nullptr;

using CDrivesViewCallback_ShouldShow_t = HRESULT(
    STDMETHODCALLTYPE*)(void*, IShellFolder*, LPCITEMIDLIST, LPCITEMIDLIST);

CDrivesViewCallback_ShouldShow_t CDrivesViewCallback_ShouldShow_Original =
    nullptr;

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
    DWORD bit = LetterBit(letter);
    DWORD oldMask = optical
                         ? g_opticalMask.fetch_or(
                               bit, std::memory_order_acq_rel)
                         : g_opticalMask.fetch_and(
                               ~bit, std::memory_order_acq_rel);

    return ((oldMask & bit) != 0) != optical;
}

static ProbeResult ProbeOpticalMediaState(WCHAR letter) {
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
        Wh_Log(L"%c: unable to open device, error=%u", letter, error);
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

        Wh_Log(L"%c: media probe inconclusive, error=%u", letter, error);
        return ProbeResult::Unknown;
    }

    Wh_Log(L"%c: media present", letter);
    return ProbeResult::Present;
}

static void RequestThisPcRefresh() {
    HWND hwnd = g_notificationWindow.load(std::memory_order_acquire);

    if (hwnd) {
        PostMessageW(hwnd, kMsgRefreshThisPc, 0, 0);
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
    HookCallGuard hookCallGuard;

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

static void QueueInitialScan() {
    g_initialScanRequested.store(true, std::memory_order_release);
    SetEvent(g_workerWakeEvent);
}

static void ProcessInitialScan(DWORD* retryMask,
                               DWORD* graceRetryMask,
                               DWORD* arrivalRetryMask,
                               int (&retryAttempts)[26]) {
    DWORD managedMask = g_managedMask.load(std::memory_order_acquire);
    DWORD logicalDrives = GetLogicalDrives();

    if (logicalDrives == 0) {
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS) {
            Wh_Log(L"GetLogicalDrives failed: %u; falling back to full scan",
                   error);
            logicalDrives = kAllDriveBits;
        }
    }

    *graceRetryMask = 0;
    *arrivalRetryMask = 0;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        DWORD bit = LetterBit(letter);
        int index = letter - L'A';

        if (!(managedMask & bit) || !(logicalDrives & bit)) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            retryAttempts[index] = 0;
            SetOpticalDrivePresent(letter, false);
            SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool optical = GetDriveTypeW(root) == DRIVE_CDROM;

        SetOpticalDrivePresent(letter, optical);

        if (!optical) {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            retryAttempts[index] = 0;
            SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (result == ProbeResult::Unknown) {
            *retryMask |= bit;
            *graceRetryMask &= ~bit;
            retryAttempts[index] = 0;
            SetCachedMediaState(letter, MediaState::Unknown);
        } else if (result == ProbeResult::NotReady) {
            *retryMask |= bit;
            *graceRetryMask |= bit;
            retryAttempts[index] = 0;
            SetCachedMediaState(letter, MediaState::Unknown);
        } else if (result == ProbeResult::Empty) {
            // An optical drive can transiently report NO_MEDIA while media is
            // becoming ready during Explorer startup/resume. Hide it now, but
            // keep probing for the full grace window so a present disc can
            // recover without waiting for another device event.
            *retryMask |= bit;
            *graceRetryMask |= bit;
            retryAttempts[index] = 0;
            SetCachedMediaState(letter, MediaState::Empty);
        } else {
            *retryMask &= ~bit;
            *graceRetryMask &= ~bit;
            retryAttempts[index] = 0;
            SetCachedMediaState(letter, MediaState::Present);
        }
    }

}

static bool ProcessRemovalMask(DWORD mask,
                               DWORD* retryMask,
                               DWORD* graceRetryMask,
                               DWORD* arrivalRetryMask,
                               int (&retryAttempts)[26]) {
    bool changed = false;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        DWORD bit = LetterBit(letter);

        if (!(mask & bit) || !IsManagedLetter(letter)) {
            continue;
        }

        int index = letter - L'A';
        *graceRetryMask &= ~bit;
        *arrivalRetryMask &= ~bit;

        WCHAR root[4];
        MakeRootPath(letter, root);

        bool stillOptical = GetDriveTypeW(root) == DRIVE_CDROM;

        if (!stillOptical) {
            Wh_Log(L"%c: optical drive removed", letter);
            *retryMask &= ~bit;
            retryAttempts[index] = 0;

            changed |= SetOpticalDrivePresent(letter, false);
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
            continue;
        }

        Wh_Log(L"%c: volume removal, drive remains optical", letter);
        changed |= SetOpticalDrivePresent(letter, true);

        ProbeResult result = ProbeOpticalMediaState(letter);

        if (result == ProbeResult::NotReady) {
            *retryMask |= bit;
        } else if (result == ProbeResult::Unknown) {
            *retryMask |= bit;
            changed |= SetCachedMediaState(letter, MediaState::Unknown);
        } else {
            *retryMask &= ~bit;
            changed |= SetCachedMediaState(
                letter, result == ProbeResult::Present
                            ? MediaState::Present
                            : MediaState::Empty);
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

            if (result == ProbeResult::Empty) {
                changed |= SetCachedMediaState(letter, MediaState::Empty);
            } else if (result == ProbeResult::NotReady) {
                changed |= SetCachedMediaState(letter, MediaState::Unknown);
            }

            Wh_Log(
                L"%c: media-ready retry window expired; "
                L"using the latest conservative state",
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
                ULONGLONG remaining = nextRetryTick - now;
                timeout = remaining >= MAXDWORD
                              ? MAXDWORD - 1
                              : static_cast<DWORD>(remaining);
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

        if (g_initialScanRequested.exchange(false, std::memory_order_acq_rel)) {
            retryMask = 0;
            graceRetryMask = 0;
            arrivalRetryMask = 0;
            nextRetryTick = 0;
            ZeroMemory(retryAttempts, sizeof(retryAttempts));

            ProcessInitialScan(&retryMask, &graceRetryMask,
                               &arrivalRetryMask, retryAttempts);
            refresh = true;
        }

        DWORD removalMask =
            g_removalRequestMask.exchange(0, std::memory_order_acq_rel);

        if (removalMask) {
            refresh |= ProcessRemovalMask(
                removalMask, &retryMask, &graceRetryMask,
                &arrivalRetryMask, retryAttempts);
        }

        DWORD arrivalMask =
            g_arrivalRequestMask.exchange(0, std::memory_order_acq_rel);

        if (arrivalMask) {
            refresh |= ProcessArrivalMask(
                arrivalMask, &retryMask, &graceRetryMask,
                &arrivalRetryMask, retryAttempts);
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

        if (refresh) {
            RequestThisPcRefresh();
        }
    }

    return 0;
}

static void RefreshThisPc(PIDLIST_ABSOLUTE thisPcPidl) {
    if (!thisPcPidl) {
        return;
    }

    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT,
                   thisPcPidl, nullptr);
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
                QueueInitialScan();
            }
            break;

        case kMsgRefreshThisPc:
            RefreshThisPc(thisPcPidl);
            return 0;

        case kMsgStop:
            if (wParam) {
                RefreshThisPc(thisPcPidl);
            }
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
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (FAILED(coHr)) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", coHr);
        SetEvent(g_notificationReadyEvent);
        return 1;
    }

    MSG msg = {};

    PIDLIST_ABSOLUTE thisPcPidl = nullptr;

    HRESULT hr = SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr,
                                        &thisPcPidl);

    if (FAILED(hr) || !thisPcPidl) {
        Wh_Log(L"SHGetKnownFolderIDList failed: 0x%08X", hr);
        CoUninitialize();

        SetEvent(g_notificationReadyEvent);
        return 1;
    }

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, L"STATIC",
                                L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                nullptr, nullptr);

    if (!hwnd) {
        Wh_Log(L"Notification window creation failed: %u", GetLastError());

        CoTaskMemFree(thisPcPidl);
        CoUninitialize();

        SetEvent(g_notificationReadyEvent);
        return 1;
    }

    if (!SetWindowSubclass(hwnd, NotificationWindowSubclassProc, 1,
                           reinterpret_cast<DWORD_PTR>(thisPcPidl))) {
        Wh_Log(L"Notification window subclass failed: %u", GetLastError());

        DestroyWindow(hwnd);
        CoTaskMemFree(thisPcPidl);
        CoUninitialize();

        SetEvent(g_notificationReadyEvent);
        return 1;
    }

    g_notificationWindow.store(hwnd, std::memory_order_release);
    g_notificationReady.store(true, std::memory_order_release);
    SetEvent(g_notificationReadyEvent);

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

    CoTaskMemFree(thisPcPidl);

    CoUninitialize();

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

    WaitForSingleObject(g_workerThread, INFINITE);

    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

static void StopNotificationThread(bool restoreView) {
    if (!g_notificationThread) {
        return;
    }

    HWND hwnd = g_notificationWindow.load(std::memory_order_acquire);

    bool stopPosted = false;

    if (hwnd) {
        stopPosted = PostMessageW(hwnd, kMsgStop, restoreView ? TRUE : FALSE, 0);
    }

    if (!stopPosted && g_notificationThreadId &&
        WaitForSingleObject(g_notificationThread, 0) == WAIT_TIMEOUT) {
        while (WaitForSingleObject(g_notificationThread, 0) == WAIT_TIMEOUT &&
               !PostThreadMessageW(g_notificationThreadId, WM_QUIT, 0, 0)) {
            Sleep(50);
        }
    }

    WaitForSingleObject(g_notificationThread, INFINITE);

    CloseHandle(g_notificationThread);
    g_notificationThread = nullptr;
    g_notificationThreadId = 0;
    g_notificationWindow.store(nullptr, std::memory_order_release);
}

static void CloseWorkerObjects() {
    if (g_notificationReadyEvent) {
        CloseHandle(g_notificationReadyEvent);
        g_notificationReadyEvent = nullptr;
    }

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

    g_notificationReady.store(false, std::memory_order_relaxed);
    g_opticalMask.store(0, std::memory_order_relaxed);
    g_arrivalRequestMask.store(0, std::memory_order_relaxed);
    g_removalRequestMask.store(0, std::memory_order_relaxed);
    g_initialScanRequested.store(false, std::memory_order_relaxed);

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

    g_notificationReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_workerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    g_workerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_notificationReadyEvent ||
        !g_workerWakeEvent || !g_workerStopEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());

        CloseWorkerObjects();
        return FALSE;
    }

    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc,
                                        nullptr, 0, &g_notificationThreadId);

    if (!g_notificationThread) {
        Wh_Log(L"CreateThread(notification) failed: %u", GetLastError());

        CloseWorkerObjects();
        return FALSE;
    }

    DWORD notificationWaitResult =
        WaitForSingleObject(g_notificationReadyEvent, INFINITE);

    if (notificationWaitResult != WAIT_OBJECT_0 ||
        !g_notificationReady.load(std::memory_order_acquire)) {
        if (notificationWaitResult == WAIT_FAILED) {
            Wh_Log(L"Waiting for notification thread failed: %u",
                   GetLastError());
        } else {
            Wh_Log(L"Notification window initialization failed");
        }

        StopNotificationThread(false);
        CloseWorkerObjects();
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"CreateThread(worker) failed: %u", GetLastError());

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
    // Hooks are active at this point. Perform the initial scan and then
    // refresh This PC unconditionally so the hooked view reflects the cache.
    QueueInitialScan();
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

    QueueInitialScan();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Hide Empty Optical Drives");

    // Windhawk has already removed the symbol hook before Wh_ModUninit, so no
    // new hook calls can start. Wait for any call that was already in flight
    // to leave the detour before the mod DLL can be unloaded.
    while (g_hookCallCount.load(std::memory_order_acquire) > 0) {
        Sleep(10);
    }

    // Stop device probing first, then refresh This PC while shutting down
    // the notification window so items hidden by the mod reappear.
    StopWorkerThread();
    StopNotificationThread(true);
    CloseWorkerObjects();
}
