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
// @compilerOptions -lole32 -lshell32 -lshlwapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Empty Optical Drives

Hides empty optical drives only from **This PC**.

The physical drive and drive letter are never disabled or removed. Explorer,
AutoPlay, mounting and device notifications continue to see the drive normally.

Implementation:
- filters optical-drive PIDLs from `IShellFolder::EnumObjects` for This PC;
- listens for `WM_DEVICECHANGE` instead of polling continuously;
- retries briefly after insertion while the disc spins up;
- intercepts optical-media removal in Explorer windows early enough to avoid
  most of the transient generic-drive redraw.

Media detection is conservative:
- readable/mounted volume -> show;
- ERROR_NOT_READY / ERROR_NO_MEDIA_IN_DRIVE -> hide;
- any other error -> show (fail open).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- driveLetters: ""
  $name: Optical drive letters
  $description: >-
    Optional letters to manage, for example G or DE.
    Leave empty to manage all optical drives.

- verboseLogging: false
  $name: Verbose logging
  $description: Log media state transitions and notifier activity.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dbt.h>
#include <commctrl.h>
#include <cwctype>
#include <new>

struct Settings {
    WCHAR driveLetters[27];
    bool verboseLogging;
};

Settings g_settings = {};

using EnumObjects_t = HRESULT(STDMETHODCALLTYPE*)(
    IShellFolder*, HWND, SHCONTF, IEnumIDList**);

EnumObjects_t g_EnumObjectsOriginal = nullptr;

PIDLIST_ABSOLUTE g_thisPcPidl = nullptr;

HANDLE g_notificationThread = nullptr;
DWORD g_notificationThreadId = 0;
HWND g_notificationWindow = nullptr;
HWINEVENTHOOK g_windowCreateHook = nullptr;

constexpr UINT_PTR kRetryTimerId = 1;
constexpr UINT kRetryIntervalMs = 500;
constexpr int kMaxRetryAttempts = 20;
constexpr UINT_PTR kExplorerSubclassId = 0x48454F44; // HEOD

DWORD g_pendingInsertMask = 0;
int g_retryAttempts[26] = {};

ULONGLONG g_lastArrivalTick[26] = {};
ULONGLONG g_lastRemovalTick[26] = {};
constexpr ULONGLONG kDuplicateEventWindowMs = 250;

struct DecisionLogState {
    bool initialized;
    bool hide;
    DWORD error;
};

DecisionLogState g_decisionLogState[26] = {};

enum class MediaState {
    Unknown,
    Empty,
    Present,
};

static bool IsManagedLetter(WCHAR letter);


static bool IsDuplicateEvent(
    WCHAR letter,
    bool arrival) {
    int index = letter - L'A';
    ULONGLONG now = GetTickCount64();
    ULONGLONG* ticks =
        arrival ? g_lastArrivalTick : g_lastRemovalTick;

    if (ticks[index] &&
        now - ticks[index] < kDuplicateEventWindowMs) {
        return true;
    }

    ticks[index] = now;
    return false;
}

static void MakeRootPath(WCHAR letter, WCHAR (&root)[4]) {
    root[0] = (WCHAR)towupper(letter);
    root[1] = L':';
    root[2] = L'\\';
    root[3] = L'\0';
}

static bool IsOpticalDrive(WCHAR letter) {
    WCHAR root[4];
    MakeRootPath(letter, root);
    return GetDriveTypeW(root) == DRIVE_CDROM;
}

static MediaState ProbeMediaState(WCHAR letter) {
    if (!IsOpticalDrive(letter)) {
        return MediaState::Unknown;
    }

    WCHAR root[4];
    MakeRootPath(letter, root);

    WCHAR volumeName[MAX_PATH + 1] = {};
    WCHAR fileSystemName[MAX_PATH + 1] = {};
    DWORD serial = 0;
    DWORD maxComponentLength = 0;
    DWORD fileSystemFlags = 0;

    SetLastError(ERROR_SUCCESS);

    if (GetVolumeInformationW(
            root,
            volumeName,
            ARRAYSIZE(volumeName),
            &serial,
            &maxComponentLength,
            &fileSystemFlags,
            fileSystemName,
            ARRAYSIZE(fileSystemName))) {
        return MediaState::Present;
    }

    DWORD error = GetLastError();

    if (error == ERROR_NOT_READY ||
        error == ERROR_NO_MEDIA_IN_DRIVE) {
        return MediaState::Empty;
    }

    return MediaState::Unknown;
}

static void RefreshThisPc() {
    if (!g_thisPcPidl) {
        return;
    }

    SHChangeNotify(
        SHCNE_UPDATEDIR,
        SHCNF_IDLIST | SHCNF_FLUSHNOWAIT,
        g_thisPcPidl,
        nullptr);
}

static void NotifyMediaInserted(WCHAR letter) {
    WCHAR root[4];
    MakeRootPath(letter, root);

    if (g_settings.verboseLogging) {
        Wh_Log(L"%c: notifier: media inserted", letter);
    }

    SHChangeNotify(
        SHCNE_MEDIAINSERTED,
        SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
        root,
        nullptr);

    RefreshThisPc();
}

static void NotifyMediaRemoved(WCHAR letter) {
    WCHAR root[4];
    MakeRootPath(letter, root);

    if (g_settings.verboseLogging) {
        Wh_Log(L"%c: notifier: media removed", letter);
    }

    // Shell-only notification. The device and drive letter stay untouched.
    SHChangeNotify(
        SHCNE_DRIVEREMOVED,
        SHCNF_PATHW | SHCNF_FLUSH,
        root,
        nullptr);

    RefreshThisPc();
}

static bool IsThisPcFolder(IShellFolder* folder) {
    if (!folder || !g_thisPcPidl) {
        return false;
    }

    IPersistFolder2* persist = nullptr;
    HRESULT hr = folder->QueryInterface(
        __uuidof(IPersistFolder2),
        reinterpret_cast<void**>(&persist));

    if (FAILED(hr) || !persist) {
        return false;
    }

    PIDLIST_ABSOLUTE currentPidl = nullptr;
    hr = persist->GetCurFolder(&currentPidl);
    persist->Release();

    if (FAILED(hr) || !currentPidl) {
        return false;
    }

    BOOL equal = ILIsEqual(currentPidl, g_thisPcPidl);
    CoTaskMemFree(currentPidl);
    return equal != FALSE;
}

static void StartInsertionRetry(WCHAR letter) {
    int index = letter - L'A';
    DWORD bit = 1u << index;

    g_pendingInsertMask |= bit;
    g_retryAttempts[index] = 0;

    if (g_notificationWindow) {
        SetTimer(
            g_notificationWindow,
            kRetryTimerId,
            kRetryIntervalMs,
            nullptr);
    }
}

static void HandleMediaArrival(WCHAR letter) {
    if (IsDuplicateEvent(letter, true)) {
        return;
    }

    if (g_settings.verboseLogging) {
        Wh_Log(L"%c: WM_DEVICECHANGE media arrival", letter);
    }

    if (ProbeMediaState(letter) == MediaState::Present) {
        g_pendingInsertMask &= ~(1u << (letter - L'A'));
        g_retryAttempts[letter - L'A'] = 0;
        NotifyMediaInserted(letter);
        return;
    }

    StartInsertionRetry(letter);
    RefreshThisPc();
}

static void ProcessPendingInsertions() {
    DWORD pending = g_pendingInsertMask;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        DWORD bit = 1u << (letter - L'A');

        if (!(pending & bit)) {
            continue;
        }

        int index = letter - L'A';

        if (ProbeMediaState(letter) == MediaState::Present) {
            if (g_settings.verboseLogging) {
                Wh_Log(
                    L"%c: media ready after %d retry/retries",
                    letter,
                    g_retryAttempts[index] + 1);
            }

            g_pendingInsertMask &= ~bit;
            g_retryAttempts[index] = 0;
            NotifyMediaInserted(letter);
            continue;
        }

        if (++g_retryAttempts[index] >= kMaxRetryAttempts) {
            if (g_settings.verboseLogging) {
                Wh_Log(L"%c: media-ready retry window expired", letter);
            }

            g_pendingInsertMask &= ~bit;
            g_retryAttempts[index] = 0;
            RefreshThisPc();
        }
    }

    if (!g_pendingInsertMask && g_notificationWindow) {
        KillTimer(g_notificationWindow, kRetryTimerId);
    }
}

static void RecheckManagedOpticalDrives() {
    bool retryNeeded = false;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        if (!IsManagedLetter(letter) || !IsOpticalDrive(letter)) {
            continue;
        }

        MediaState state = ProbeMediaState(letter);

        if (state == MediaState::Present) {
            DWORD bit = 1u << (letter - L'A');
            g_pendingInsertMask &= ~bit;
            g_retryAttempts[letter - L'A'] = 0;

            // DBT_DEVNODES_CHANGED is the fallback for optical stacks that
            // occasionally omit a repeated media-arrival notification.
            NotifyMediaInserted(letter);
        } else if (state != MediaState::Empty) {
            StartInsertionRetry(letter);
            retryNeeded = true;
        }
    }

    if (retryNeeded && g_settings.verboseLogging) {
        Wh_Log(L"DBT_DEVNODES_CHANGED -> short optical recheck window");
    }
}

static bool GetManagedOpticalLetterFromVolume(
    LPARAM lParam,
    WCHAR* matchedLetter) {
    if (!lParam) {
        return false;
    }

    auto* header =
        reinterpret_cast<const DEV_BROADCAST_HDR*>(lParam);

    if (header->dbch_devicetype != DBT_DEVTYP_VOLUME) {
        return false;
    }

    auto* volume =
        reinterpret_cast<const DEV_BROADCAST_VOLUME*>(lParam);

    DWORD mask = volume->dbcv_unitmask;

    for (WCHAR letter = L'A'; letter <= L'Z'; letter++) {
        DWORD bit = 1u << (letter - L'A');

        if (!(mask & bit) ||
            !IsManagedLetter(letter) ||
            !IsOpticalDrive(letter)) {
            continue;
        }

        if (matchedLetter) {
            *matchedLetter = letter;
        }

        return true;
    }

    return false;
}

static LRESULT CALLBACK ExplorerWindowSubclassProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR,
    DWORD_PTR) {
    if (message == WM_DEVICECHANGE &&
        wParam == DBT_DEVICEREMOVECOMPLETE) {
        WCHAR letter = 0;

        if (GetManagedOpticalLetterFromVolume(lParam, &letter)) {
            if (!IsDuplicateEvent(letter, false)) {
                if (g_settings.verboseLogging) {
                    Wh_Log(
                        L"%c: Explorer intercepted media removal",
                        letter);
                }

                NotifyMediaRemoved(letter);
            }

            return TRUE;
        }
    }

    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(
            hwnd,
            ExplorerWindowSubclassProc,
            kExplorerSubclassId);
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

static void TrySubclassExplorerWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId != GetCurrentProcessId()) {
        return;
    }

    WCHAR className[64] = {};

    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        lstrcmpW(className, L"CabinetWClass") != 0) {
        return;
    }

    SetWindowSubclass(
        hwnd,
        ExplorerWindowSubclassProc,
        kExplorerSubclassId,
        0);
}

static BOOL CALLBACK EnumExplorerWindowsProc(HWND hwnd, LPARAM) {
    TrySubclassExplorerWindow(hwnd);
    return TRUE;
}

static void CALLBACK WindowCreateWinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD) {
    if (event == EVENT_OBJECT_CREATE &&
        idObject == OBJID_WINDOW) {
        TrySubclassExplorerWindow(hwnd);
    }
}

static LRESULT CALLBACK NotificationWindowSubclassProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR,
    DWORD_PTR) {
    switch (message) {
        case WM_DEVICECHANGE: {
            if (wParam == DBT_DEVICEARRIVAL) {
                WCHAR letter = 0;

                if (GetManagedOpticalLetterFromVolume(
                        lParam,
                        &letter)) {
                    HandleMediaArrival(letter);
                }
            } else if (wParam == DBT_DEVNODES_CHANGED) {
                RecheckManagedOpticalDrives();
            }

            return TRUE;
        }

        case WM_TIMER:
            if (wParam == kRetryTimerId) {
                ProcessPendingInsertions();
                return 0;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_NCDESTROY:
            RemoveWindowSubclass(
                hwnd,
                NotificationWindowSubclassProc,
                1);
            g_notificationWindow = nullptr;
            PostQuitMessage(0);
            break;
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

static DWORD WINAPI NotificationThreadProc(void*) {
    // Use a built-in system window class instead of registering our own.
    // Predefined system classes must be created with a null hInstance.
    // This also keeps Windhawk hot reloads free of stale class state.
    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
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
        Wh_Log(L"Notification window creation failed: %u", GetLastError());
        return 1;
    }

    if (!SetWindowSubclass(
            hwnd,
            NotificationWindowSubclassProc,
            1,
            0)) {
        Wh_Log(L"Notification window subclass failed: %u", GetLastError());
        DestroyWindow(hwnd);
        return 1;
    }

    g_notificationWindow = hwnd;

    if (g_settings.verboseLogging) {
        Wh_Log(L"Device notification window ready: %p", hwnd);
    }

    MSG msg = {};

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

static void LoadSettings() {
    g_settings.driveLetters[0] = L'\0';

    PCWSTR letters = Wh_GetStringSetting(L"driveLetters");

    if (letters) {
        size_t out = 0;

        for (size_t i = 0;
             letters[i] &&
             out < ARRAYSIZE(g_settings.driveLetters) - 1;
             i++) {
            WCHAR ch = (WCHAR)towupper(letters[i]);

            if (ch < L'A' || ch > L'Z') {
                continue;
            }

            bool duplicate = false;

            for (size_t j = 0; j < out; j++) {
                if (g_settings.driveLetters[j] == ch) {
                    duplicate = true;
                    break;
                }
            }

            if (!duplicate) {
                g_settings.driveLetters[out++] = ch;
            }
        }

        g_settings.driveLetters[out] = L'\0';
        Wh_FreeStringSetting(letters);
    }

    g_settings.verboseLogging =
        Wh_GetIntSetting(L"verboseLogging") != 0;
}

static bool IsManagedLetter(WCHAR letter) {
    letter = (WCHAR)towupper(letter);

    if (!g_settings.driveLetters[0]) {
        return true;
    }

    for (size_t i = 0; g_settings.driveLetters[i]; i++) {
        if (g_settings.driveLetters[i] == letter) {
            return true;
        }
    }

    return false;
}

static bool IsEmptyOpticalDrive(
    IShellFolder* parent,
    PCUITEMID_CHILD pidl) {
    STRRET strret = {};

    HRESULT hr = parent->GetDisplayNameOf(
        pidl,
        SHGDN_FORPARSING,
        &strret);

    if (FAILED(hr)) {
        return false;
    }

    WCHAR parsingName[MAX_PATH] = {};

    hr = StrRetToBufW(
        &strret,
        pidl,
        parsingName,
        ARRAYSIZE(parsingName));

    if (FAILED(hr)) {
        return false;
    }

    WCHAR letter = (WCHAR)towupper(parsingName[0]);

    if (letter < L'A' ||
        letter > L'Z' ||
        parsingName[1] != L':' ||
        !IsManagedLetter(letter) ||
        !IsOpticalDrive(letter)) {
        return false;
    }

    WCHAR root[4];
    MakeRootPath(letter, root);

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

    bool hide =
        !ok &&
        (error == ERROR_NOT_READY ||
         error == ERROR_NO_MEDIA_IN_DRIVE);

    if (g_settings.verboseLogging) {
        int index = letter - L'A';
        DecisionLogState& state = g_decisionLogState[index];

        if (!state.initialized ||
            state.hide != hide ||
            state.error != error) {
            if (ok) {
                Wh_Log(
                    L"%c: SHOW volume=\"%s\" fs=\"%s\"",
                    letter,
                    volumeName,
                    fileSystemName);
            } else if (hide) {
                Wh_Log(
                    L"%c: HIDE no ready media (error=%u)",
                    letter,
                    error);
            } else {
                Wh_Log(
                    L"%c: SHOW fail-open (error=%u)",
                    letter,
                    error);
            }

            state.initialized = true;
            state.hide = hide;
            state.error = error;
        }
    }

    return hide;
}

class FilteredEnumIDList final : public IEnumIDList {
public:
    FilteredEnumIDList(
        IEnumIDList* inner,
        IShellFolder* parent)
        : m_refCount(1),
          m_inner(inner),
          m_parent(parent) {
        m_inner->AddRef();
        m_parent->AddRef();
    }

    ~FilteredEnumIDList() {
        m_parent->Release();
        m_inner->Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;

        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IEnumIDList)) {
            *object = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG ref = InterlockedDecrement(&m_refCount);

        if (!ref) {
            delete this;
            return 0;
        }

        return (ULONG)ref;
    }

    HRESULT STDMETHODCALLTYPE Next(
        ULONG count,
        PITEMID_CHILD* items,
        ULONG* fetched) override {
        if (!items) {
            return E_POINTER;
        }

        if (count != 1 && !fetched) {
            return E_POINTER;
        }

        ULONG visible = 0;

        while (visible < count) {
            PITEMID_CHILD child = nullptr;
            ULONG got = 0;

            HRESULT hr = m_inner->Next(1, &child, &got);

            if (hr != S_OK || !got || !child) {
                if (fetched) {
                    *fetched = visible;
                }

                return visible == count ? S_OK : S_FALSE;
            }

            if (IsEmptyOpticalDrive(m_parent, child)) {
                CoTaskMemFree(child);
                continue;
            }

            items[visible++] = child;
        }

        if (fetched) {
            *fetched = visible;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Skip(ULONG count) override {
        while (count--) {
            PITEMID_CHILD child = nullptr;
            ULONG got = 0;

            if (Next(1, &child, &got) != S_OK ||
                !got ||
                !child) {
                return S_FALSE;
            }

            CoTaskMemFree(child);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Reset() override {
        return m_inner->Reset();
    }

    HRESULT STDMETHODCALLTYPE Clone(
        IEnumIDList** output) override {
        if (!output) {
            return E_POINTER;
        }

        *output = nullptr;

        IEnumIDList* cloned = nullptr;
        HRESULT hr = m_inner->Clone(&cloned);

        if (FAILED(hr) || !cloned) {
            return hr;
        }

        auto* wrapper =
            new (std::nothrow) FilteredEnumIDList(
                cloned,
                m_parent);

        cloned->Release();

        if (!wrapper) {
            return E_OUTOFMEMORY;
        }

        *output = wrapper;
        return S_OK;
    }

private:
    LONG m_refCount;
    IEnumIDList* m_inner;
    IShellFolder* m_parent;
};

static HRESULT STDMETHODCALLTYPE EnumObjects_Hook(
    IShellFolder* self,
    HWND hwnd,
    SHCONTF flags,
    IEnumIDList** enumList) {
    HRESULT hr =
        g_EnumObjectsOriginal(
            self,
            hwnd,
            flags,
            enumList);

    if (hr != S_OK ||
        !enumList ||
        !*enumList ||
        !IsThisPcFolder(self)) {
        return hr;
    }

    auto* wrapper =
        new (std::nothrow) FilteredEnumIDList(
            *enumList,
            self);

    if (!wrapper) {
        return hr;
    }

    (*enumList)->Release();
    *enumList = wrapper;
    return hr;
}

static bool HookThisPcEnumObjects() {
    IShellFolder* desktop = nullptr;
    HRESULT hr = SHGetDesktopFolder(&desktop);

    if (FAILED(hr) || !desktop) {
        Wh_Log(L"SHGetDesktopFolder failed: 0x%08X", hr);
        return false;
    }

    hr = SHGetFolderLocation(
        nullptr,
        CSIDL_DRIVES,
        nullptr,
        0,
        &g_thisPcPidl);

    if (FAILED(hr) || !g_thisPcPidl) {
        Wh_Log(
            L"SHGetFolderLocation(CSIDL_DRIVES) failed: 0x%08X",
            hr);
        desktop->Release();
        return false;
    }

    IShellFolder* thisPcFolder = nullptr;

    hr = desktop->BindToObject(
        g_thisPcPidl,
        nullptr,
        __uuidof(IShellFolder),
        reinterpret_cast<void**>(&thisPcFolder));

    desktop->Release();

    if (FAILED(hr) || !thisPcFolder) {
        Wh_Log(L"BindToObject(This PC) failed: 0x%08X", hr);
        return false;
    }

    void** vtable =
        *reinterpret_cast<void***>(thisPcFolder);

    bool hooked = Wh_SetFunctionHook(
        vtable[4], // IShellFolder::EnumObjects
        reinterpret_cast<void*>(EnumObjects_Hook),
        reinterpret_cast<void**>(&g_EnumObjectsOriginal));

    if (g_settings.verboseLogging) {
        Wh_Log(
            L"This PC EnumObjects hook: function=%p hooked=%d",
            vtable[4],
            hooked);
    }

    thisPcFolder->Release();
    return hooked;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Hide Empty Optical Drives v1.0.0");

    LoadSettings();

    HRESULT coHr =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool uninitializeCom =
        coHr == S_OK || coHr == S_FALSE;

    bool hooked = HookThisPcEnumObjects();

    if (uninitializeCom) {
        CoUninitialize();
    }

    if (!hooked) {
        Wh_Log(L"Failed to hook This PC EnumObjects");
        return FALSE;
    }

    EnumWindows(EnumExplorerWindowsProc, 0);

    g_windowCreateHook = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_CREATE,
        nullptr,
        WindowCreateWinEventProc,
        GetCurrentProcessId(),
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!g_windowCreateHook) {
        Wh_Log(
            L"SetWinEventHook failed: %u",
            GetLastError());
    }

    g_notificationThread = CreateThread(
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
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    ZeroMemory(
        g_decisionLogState,
        sizeof(g_decisionLogState));
    ZeroMemory(g_lastArrivalTick, sizeof(g_lastArrivalTick));
    ZeroMemory(g_lastRemovalTick, sizeof(g_lastRemovalTick));
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Hide Empty Optical Drives v1.0.0");

    if (g_windowCreateHook) {
        UnhookWinEvent(g_windowCreateHook);
        g_windowCreateHook = nullptr;
    }

    if (g_notificationWindow) {
        PostMessageW(
            g_notificationWindow,
            WM_CLOSE,
            0,
            0);
    } else if (g_notificationThreadId) {
        PostThreadMessageW(
            g_notificationThreadId,
            WM_QUIT,
            0,
            0);
    }

    if (g_notificationThread) {
        WaitForSingleObject(g_notificationThread, 3000);
        CloseHandle(g_notificationThread);
        g_notificationThread = nullptr;
    }

    g_notificationThreadId = 0;
    g_notificationWindow = nullptr;
    g_pendingInsertMask = 0;

    ZeroMemory(g_retryAttempts, sizeof(g_retryAttempts));
    ZeroMemory(g_lastArrivalTick, sizeof(g_lastArrivalTick));
    ZeroMemory(g_lastRemovalTick, sizeof(g_lastRemovalTick));
    ZeroMemory(
        g_decisionLogState,
        sizeof(g_decisionLogState));

    if (g_thisPcPidl) {
        CoTaskMemFree(g_thisPcPidl);
        g_thisPcPidl = nullptr;
    }
}
