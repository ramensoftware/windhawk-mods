// ==WindhawkMod==
// @id              hide-volume-brightness-osd
// @name            Hide Volume/Brightness OSD
// @description     Hide the Windows 11 volume and brightness on-screen indicators, together or separately
// @version         1.0
// @author          Shehab
// @github          https://github.com/Shadavellir
// @include         explorer.exe
// @compilerOptions -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Volume/Brightness OSD

Hides the popup that appears when you change the volume or the screen
brightness (media keys, Fn keys, etc.). The volume and brightness still change
normally, only the on-screen indicator is hidden.

## Features
- Hide the **volume** indicator and the **brightness** indicator independently.
- Turn either one back on at any time from the mod settings, with no restart
  needed. Settings apply instantly.
- Quick settings, the calendar and other flyouts are left untouched.
- Optional support for the older (Windows 10 style) volume popup that some
  Windows 11 builds still use.

## Settings
- **Hide volume indicator:** hides the popup when volume or mute changes.
- **Hide brightness indicator:** hides the popup when brightness changes.
- **Also hide the legacy OSD:** for builds that use the older DirectUI popup.
- **Max OSD width / height:** windows bigger than this are never treated as
  the OSD. Lower these if other flyouts disappear.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideVolume: true
  $name: Hide volume indicator
  $description: Hide the on-screen popup shown when the volume or mute changes.
- hideBrightness: true
  $name: Hide brightness indicator
  $description: Hide the on-screen popup shown when the brightness changes.
- hideLegacyOsd: true
  $name: Also hide the legacy (Windows 10 style) OSD
  $description: Hides the older DirectUI based popup, used on some Windows 11 builds.
- maxWidth: 450
  $name: Max OSD width (DIP)
  $description: Windows wider than this are never treated as the OSD.
- maxHeight: 150
  $name: Max OSD height (DIP)
  $description: Windows taller than this are never treated as the OSD. Keeps quick settings and other flyouts untouched.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

struct {
    bool hideVolume;
    bool hideBrightness;
    bool hideLegacyOsd;
    int maxWidth;
    int maxHeight;
} settings;

// ---------------------------------------------------------------------------
// Volume change tracking
// ---------------------------------------------------------------------------

static const ULONGLONG kVolumeWindowMs = 800;

static CRITICAL_SECTION g_cs;
static bool g_haveState = false;
static float g_lastVolume = 0.0f;
static BOOL g_lastMute = FALSE;
static ULONGLONG g_lastChangeTick = 0;

static HANDLE g_stopEvent = nullptr;
static HANDLE g_thread = nullptr;

static bool ReadVolumeState(float* volume, BOOL* mute) {
    bool ok = false;
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioEndpointVolume* endpoint = nullptr;

    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                   CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator),
                                   (void**)&enumerator)) &&
        SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                      &device)) &&
        SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                   nullptr, (void**)&endpoint))) {
        ok = SUCCEEDED(endpoint->GetMasterVolumeLevelScalar(volume)) &&
             SUCCEEDED(endpoint->GetMute(mute));
    }

    if (endpoint) endpoint->Release();
    if (device) device->Release();
    if (enumerator) enumerator->Release();
    return ok;
}

// Compares the current state to the last known one and records the time of
// the last change. Called by the polling thread and right before a decision.
static void CheckVolumeChanged() {
    float volume = 0.0f;
    BOOL mute = FALSE;
    if (!ReadVolumeState(&volume, &mute)) {
        return;
    }

    EnterCriticalSection(&g_cs);
    if (g_haveState && (volume != g_lastVolume || mute != g_lastMute)) {
        g_lastChangeTick = GetTickCount64();
    }
    g_lastVolume = volume;
    g_lastMute = mute;
    g_haveState = true;
    LeaveCriticalSection(&g_cs);
}

static bool VolumeChangedRecently() {
    CheckVolumeChanged();

    EnterCriticalSection(&g_cs);
    bool recent = g_lastChangeTick != 0 &&
                  (GetTickCount64() - g_lastChangeTick) < kVolumeWindowMs;
    LeaveCriticalSection(&g_cs);
    return recent;
}

static DWORD WINAPI PollThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (WaitForSingleObject(g_stopEvent, 100) == WAIT_TIMEOUT) {
        CheckVolumeChanged();
    }
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// OSD detection
// ---------------------------------------------------------------------------

static bool LooksLikeOsdWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }

    bool isXaml = _wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0;
    bool isLegacy = settings.hideLegacyOsd &&
                    _wcsicmp(cls, L"NativeHWNDHost") == 0 &&
                    FindWindowExW(hWnd, nullptr, L"DirectUIHWND", nullptr);

    if (!isXaml && !isLegacy) {
        return false;
    }

    // The OSD is a non-activating tool window.
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_NOACTIVATE)) {
        return false;
    }

    // The OSD is small; quick settings, calendar etc. are much bigger.
    RECT rc;
    if (GetWindowRect(hWnd, &rc)) {
        UINT dpi = GetDpiForWindow(hWnd);
        if (!dpi) {
            dpi = 96;
        }
        int w = (rc.right - rc.left) * 96 / (int)dpi;
        int h = (rc.bottom - rc.top) * 96 / (int)dpi;
        if (w > settings.maxWidth || h > settings.maxHeight) {
            return false;
        }
    }

    return true;
}

// Returns true if this window is an OSD that should be hidden.
static bool ShouldHideOsd(HWND hWnd) {
    // Nothing enabled: never hide anything.
    if (!settings.hideVolume && !settings.hideBrightness) {
        return false;
    }

    if (!LooksLikeOsdWindow(hWnd)) {
        return false;
    }

    // Both enabled: hide every OSD, no need to tell them apart.
    if (settings.hideVolume && settings.hideBrightness) {
        Wh_Log(L"Hiding OSD hwnd=%p", hWnd);
        return true;
    }

    // Only one enabled: work out which kind of OSD this is.
    bool isVolume = VolumeChangedRecently();

    if (isVolume && settings.hideVolume) {
        Wh_Log(L"Hiding volume OSD hwnd=%p", hWnd);
        return true;
    }
    if (!isVolume && settings.hideBrightness) {
        Wh_Log(L"Hiding brightness OSD hwnd=%p", hWnd);
        return true;
    }

    Wh_Log(L"Allowing %s OSD hwnd=%p", isVolume ? L"volume" : L"brightness",
           hWnd);
    return false;
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && nCmdShow != SW_MINIMIZE &&
        ShouldHideOsd(hWnd)) {
        nCmdShow = SW_HIDE;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if ((uFlags & SWP_SHOWWINDOW) && ShouldHideOsd(hWnd)) {
        uFlags = (uFlags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

void LoadSettings() {
    settings.hideVolume = Wh_GetIntSetting(L"hideVolume");
    settings.hideBrightness = Wh_GetIntSetting(L"hideBrightness");
    settings.hideLegacyOsd = Wh_GetIntSetting(L"hideLegacyOsd");
    settings.maxWidth = Wh_GetIntSetting(L"maxWidth");
    settings.maxHeight = Wh_GetIntSetting(L"maxHeight");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    InitializeCriticalSection(&g_cs);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_thread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    }

    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                       (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                       (void**)&SetWindowPos_Original);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    DeleteCriticalSection(&g_cs);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
