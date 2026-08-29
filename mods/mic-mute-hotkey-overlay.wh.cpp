// ==WindhawkMod==
// @id              mic-mute-hotkey-overlay
// @name            Global Hotkey Mute Microphone + Floating Overlay
// @description     Global hotkey to mute/unmute the default microphone, with a floating always-on-top overlay that animates while the mic is active
// @description:id-ID Hotkey global untuk mute/unmute mikrofon default, dengan indikator overlay melayang yang selalu di atas dan beranimasi saat mikrofon aktif
// @version         2.0.0
// @author          Farel
// @github          https://github.com/Eliasilyz
// @homepage        https://farelhanafi.my.id/
// @include         windhawk.exe
// @compilerOptions -lole32 -luser32 -lgdi32 -lgdiplus -lshcore -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Hotkey Mute Microphone + Floating Overlay

Binds a global hotkey to toggle mute on your default microphone, and shows
a floating, always-on-top overlay indicator while it does.

## Features

- Global hotkey (default `Ctrl+Alt+M`), configured as a plain string like
  `"Ctrl+Alt+Shift+M"` rather than a raw modifier bitmask + VK code.
- Toggles mute on the default capture device via the Core Audio API
  (`IAudioEndpointVolume`). The endpoint is resolved once and kept cached;
  it's only re-resolved when the default device actually changes (event-
  driven via `IMMNotificationClient`), not on every hotkey press.
- The overlay and mute state stay in sync with **any** source of mute
  changes (this hotkey, Windows Sound settings, another app) and with the
  **default microphone changing** (headset plugged in, device switched in
  Settings) — both are event-driven, not polled.
- Real peak-level metering (`IAudioMeterInformation`) drives a pulsing ring
  while unmuted, so the animation reflects actual mic input, not a fixed
  pattern. Falls back to a synthetic pulse only if peak metering isn't
  available on a given system.
- The render target (DIB section + memory DC) is cached and only rebuilt
  when the overlay's pixel size changes, instead of being recreated on
  every animation frame.
- Per-monitor DPI aware; auto-position respects the taskbar (work area),
  not just the raw screen size.
- When click-through is turned off, the overlay can be dragged to a custom
  position instead of just blocking clicks with no other purpose.
- Configurable size, position, hotkey, and auto-hide delay, all applied
  live without reloading the mod.

## Settings

See the settings panel for the full list and description of each option.
The hotkey is a single string, e.g. `"Ctrl+Alt+M"`; at least one modifier
is required — a hotkey with no modifier is rejected rather than silently
grabbing a bare key system-wide.

## Known limitations

- The animation reflects the microphone's own peak level; it does not
  distinguish your voice from other captured noise.
- A topmost overlay in a normal window won't render over exclusive-
  fullscreen apps/games — worth knowing since a call/game is often the
  moment the indicator matters most.
- No drag-to-reposition when click-through is on — by design, click-through
  means the overlay doesn't intercept the mouse at all, so dragging only
  works with click-through disabled.

### Preview
| Muted | Unmuted |
| :---: | :---: |
| ![Muted](https://raw.githubusercontent.com/Eliasilyz/global-hotkey-mute/refs/heads/main/9fnqpbp.png) | ![Unmuted](https://raw.githubusercontent.com/Eliasilyz/global-hotkey-mute/refs/heads/main/74488pt.png) |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: "Ctrl+Alt+M"
  $name: Hotkey
  $description: >-
    Combine modifiers with '+': Ctrl, Alt, Shift, Win, plus exactly one
    key (a letter, digit, F1-F24, or a name like Space/Tab/Enter/Esc/Up/
    Down/Left/Right/Insert/Delete/Home/End/PageUp/PageDown). At least one
    modifier is required — a hotkey with no modifier is rejected instead
    of registering a bare key system-wide. Example: "Ctrl+Alt+M".
- overlaySize: 72
  $name: Overlay size (px)
  $description: "Logical pixels at 96 DPI; scaled automatically for the target monitor's DPI"
- overlayAutoPosition: true
  $name: Auto-position overlay
  $description: >-
    Place the overlay at the bottom-right of the primary monitor's work
    area (excludes the taskbar). Disable to set an exact position with
    overlayPosX/overlayPosY, or drag the overlay directly (requires
    click-through to be off).
- overlayPosX: 0
  $name: Overlay X position
  $description: >-
    Used only when auto-position is off. Virtual-screen coordinate (can
    be negative for a monitor positioned left of the primary).
- overlayPosY: 0
  $name: Overlay Y position
  $description: >-
    Used only when auto-position is off. Virtual-screen coordinate (can
    be negative for a monitor positioned above the primary).
- overlayMargin: 24
  $name: Overlay margin from screen edge (px)
  $description: "Used only when auto-position is on"
- overlayDurationMs: 1500
  $name: Overlay auto-hide delay after toggle (ms)
- alwaysShow: false
  $name: Always show overlay (ignore auto-hide)
- clickThrough: true
  $name: Click-through overlay (don't block mouse)
  $description: >-
    When enabled, clicks pass through the overlay to whatever's underneath.
    When disabled, the overlay blocks clicks and can be dragged to
    reposition it.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <gdiplus.h>
#include <cmath>
#include <cwchar>
#include <cwctype>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// ---------------------------------------------------------------------------
// Manually-declared interface
// ---------------------------------------------------------------------------
// IAudioMeterInformation is only forward-declared (no method table) by this
// compiler's endpointvolume.h, so we declare our own copy under a different
// name and activate it via an explicit IID, the same pattern used elsewhere
// in this repo for undocumented/incompletely-declared COM interfaces (see
// audio-scroll-switcher.wh.cpp's IPolicyConfig).
MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IAudioMeterInformation2 : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};

const IID IID_IAudioMeterInformation2 = {
    0xc02216f6,
    0x8c67,
    0x4b5b,
    {0x9d, 0x00, 0xd0, 0x08, 0xe7, 0x3e, 0x00, 0x64}};

// ---------------------------------------------------------------------------
// Module handle (NOT GetModuleHandle(nullptr) — see item 7 of the review)
// ---------------------------------------------------------------------------
HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

// ---------------------------------------------------------------------------
// Hotkey string parsing ("Ctrl+Alt+M" -> MOD_* bitmask + VK code)
// ---------------------------------------------------------------------------
UINT VKFromKeyName(const wchar_t* name) {
    if (name[0] != L'\0' && name[1] == L'\0') {
        wchar_t c = towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return (UINT)c;
        }
    }

    if ((name[0] == L'F' || name[0] == L'f') && name[1] != L'\0') {
        int num = 0;
        const wchar_t* d = name + 1;
        bool allDigits = true;
        for (; *d; d++) {
            if (*d < L'0' || *d > L'9') {
                allDigits = false;
                break;
            }
            num = num * 10 + (*d - L'0');
        }
        if (allDigits && num >= 1 && num <= 24) {
            return VK_F1 + (num - 1);
        }
    }

    struct NamedKey {
        const wchar_t* name;
        UINT vk;
    };
    static const NamedKey kNamedKeys[] = {
        {L"SPACE", VK_SPACE},       {L"TAB", VK_TAB},
        {L"ENTER", VK_RETURN},      {L"RETURN", VK_RETURN},
        {L"ESC", VK_ESCAPE},        {L"ESCAPE", VK_ESCAPE},
        {L"UP", VK_UP},             {L"DOWN", VK_DOWN},
        {L"LEFT", VK_LEFT},         {L"RIGHT", VK_RIGHT},
        {L"INSERT", VK_INSERT},     {L"DELETE", VK_DELETE},
        {L"HOME", VK_HOME},         {L"END", VK_END},
        {L"PAGEUP", VK_PRIOR},      {L"PAGEDOWN", VK_NEXT},
        {L"BACKSPACE", VK_BACK},    {L"CAPSLOCK", VK_CAPITAL},
        {L"NUMLOCK", VK_NUMLOCK},   {L"SCROLLLOCK", VK_SCROLL},
        {L"PRINTSCREEN", VK_SNAPSHOT}, {L"PAUSE", VK_PAUSE},
    };

    wchar_t upper[32];
    int i = 0;
    for (; name[i] && i < 31; i++) upper[i] = towupper(name[i]);
    upper[i] = L'\0';

    for (const auto& e : kNamedKeys) {
        if (wcscmp(upper, e.name) == 0) return e.vk;
    }
    return 0;
}

// Returns true only if at least one modifier AND a recognized key were
// found — a hotkey with no modifier is rejected by the caller rather than
// grabbing a bare key system-wide.
bool ParseHotkeyString(const wchar_t* str, UINT* outMods, UINT* outVK) {
    UINT mods = 0;
    UINT vk = 0;
    const wchar_t* p = str;

    while (*p) {
        while (*p == L' ') p++;

        wchar_t token[32];
        int len = 0;
        while (*p && *p != L'+' && len < 31) token[len++] = *p++;
        token[len] = L'\0';
        while (len > 0 && token[len - 1] == L' ') token[--len] = L'\0';
        if (*p == L'+') p++;
        if (len == 0) continue;

        wchar_t upper[32];
        int i = 0;
        for (; token[i] && i < 31; i++) upper[i] = towupper(token[i]);
        upper[i] = L'\0';

        if (wcscmp(upper, L"CTRL") == 0 || wcscmp(upper, L"CONTROL") == 0) {
            mods |= MOD_CONTROL;
        } else if (wcscmp(upper, L"ALT") == 0) {
            mods |= MOD_ALT;
        } else if (wcscmp(upper, L"SHIFT") == 0) {
            mods |= MOD_SHIFT;
        } else if (wcscmp(upper, L"WIN") == 0 ||
                   wcscmp(upper, L"WINDOWS") == 0) {
            mods |= MOD_WIN;
        } else {
            vk = VKFromKeyName(token);
        }
    }

    *outMods = mods;
    *outVK = vk;
    return mods != 0 && vk != 0;
}

struct ModSettings {
    UINT hotkeyModifiers;
    UINT hotkeyVK;
    wchar_t hotkeyRaw[64];
    int overlaySize;
    bool overlayAutoPosition;
    int overlayPosX;
    int overlayPosY;
    int overlayMargin;
    int overlayDurationMs;
    bool alwaysShow;
    bool clickThrough;
} g_settings;

void LoadSettings() {
    PCWSTR hotkeyStr = Wh_GetStringSetting(L"hotkey");
    g_settings.hotkeyModifiers = 0;
    g_settings.hotkeyVK = 0;
    g_settings.hotkeyRaw[0] = L'\0';
    if (hotkeyStr) {
        wcsncpy(g_settings.hotkeyRaw, hotkeyStr,
                ARRAYSIZE(g_settings.hotkeyRaw) - 1);
        g_settings.hotkeyRaw[ARRAYSIZE(g_settings.hotkeyRaw) - 1] = L'\0';
        ParseHotkeyString(hotkeyStr, &g_settings.hotkeyModifiers,
                           &g_settings.hotkeyVK);
        Wh_FreeStringSetting(hotkeyStr);
    }

    g_settings.overlaySize = (int)Wh_GetIntSetting(L"overlaySize");
    g_settings.overlayAutoPosition =
        Wh_GetIntSetting(L"overlayAutoPosition") != 0;
    g_settings.overlayPosX = (int)Wh_GetIntSetting(L"overlayPosX");
    g_settings.overlayPosY = (int)Wh_GetIntSetting(L"overlayPosY");
    g_settings.overlayMargin = (int)Wh_GetIntSetting(L"overlayMargin");
    g_settings.overlayDurationMs = (int)Wh_GetIntSetting(L"overlayDurationMs");
    g_settings.alwaysShow = Wh_GetIntSetting(L"alwaysShow") != 0;
    g_settings.clickThrough = Wh_GetIntSetting(L"clickThrough") != 0;

    if (g_settings.overlaySize < 24) g_settings.overlaySize = 24;
    if (g_settings.overlaySize > 512) g_settings.overlaySize = 512;
    if (g_settings.overlayMargin < 0) g_settings.overlayMargin = 0;
    if (g_settings.overlayDurationMs < 200) g_settings.overlayDurationMs = 200;
    if (g_settings.overlayDurationMs > 60000)
        g_settings.overlayDurationMs = 60000;
}

// ---------------------------------------------------------------------------
// Audio (Core Audio API) — resolved fresh on demand, event-driven updates
// ---------------------------------------------------------------------------
CRITICAL_SECTION g_audioLock;
IMMDeviceEnumerator* g_pEnumerator = nullptr;
IMMDevice* g_pCaptureDevice = nullptr;
IAudioEndpointVolume* g_pEndpointVolume = nullptr;
IAudioMeterInformation2* g_pMeterInfo = nullptr;
bool g_micMuted = false;

constexpr UINT kMsgMuteChanged = WM_APP + 2;
constexpr UINT kMsgDeviceChanged = WM_APP + 3;
constexpr UINT kMsgSettingsChanged = WM_APP + 1;

HWND g_hOverlay = nullptr;

// Notified on volume/mute changes from ANY source (this mod, Windows Sound
// settings, another app). Only posts a message — never touches COM
// interfaces here, since this can run on an arbitrary/MTA thread.
class EndpointVolumeCallback : public IAudioEndpointVolumeCallback {
   public:
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = this;
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override {
        if (g_hOverlay && pNotify) {
            PostMessage(g_hOverlay, kMsgMuteChanged, pNotify->bMuted ? 1 : 0,
                        0);
        }
        return S_OK;
    }
};

// Notified when the default capture device changes (headset plugged in,
// device switched in Settings). Also posts-only, per the same MTA caveat.
class DefaultDeviceNotificationClient : public IMMNotificationClient {
   public:
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IMMNotificationClient)) {
            *ppv = this;
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow,
                                         ERole role,
                                         LPCWSTR) override {
        if (flow == eCapture && role == eConsole && g_hOverlay) {
            PostMessage(g_hOverlay, kMsgDeviceChanged, 0, 0);
        }
        return S_OK;
    }
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) override {
        return S_OK;
    }
    STDMETHODIMP OnDeviceAdded(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR,
                                         const PROPERTYKEY) override {
        return S_OK;
    }
};

EndpointVolumeCallback g_volumeCallback;
DefaultDeviceNotificationClient g_notificationClient;

void ReleaseAudioEndpoint_NoLock() {
    if (g_pEndpointVolume) {
        g_pEndpointVolume->UnregisterControlChangeNotify(&g_volumeCallback);
    }
    if (g_pMeterInfo) {
        g_pMeterInfo->Release();
        g_pMeterInfo = nullptr;
    }
    if (g_pEndpointVolume) {
        g_pEndpointVolume->Release();
        g_pEndpointVolume = nullptr;
    }
    if (g_pCaptureDevice) {
        g_pCaptureDevice->Release();
        g_pCaptureDevice = nullptr;
    }
}

// Re-resolves the default capture device and its interfaces. Cheap enough
// to call on every toggle (rare, user-triggered), which also guards
// against a stale device if the event-driven path is ever missed.
bool ResolveAudioEndpoint() {
    EnterCriticalSection(&g_audioLock);
    ReleaseAudioEndpoint_NoLock();

    if (!g_pEnumerator) {
        HRESULT hr =
            CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                              CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                              (void**)&g_pEnumerator);
        if (FAILED(hr) || !g_pEnumerator) {
            Wh_Log(L"Failed to create device enumerator: 0x%08X", hr);
            LeaveCriticalSection(&g_audioLock);
            return false;
        }
        g_pEnumerator->RegisterEndpointNotificationCallback(
            &g_notificationClient);
    }

    HRESULT hr = g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole,
                                                          &g_pCaptureDevice);
    if (FAILED(hr) || !g_pCaptureDevice) {
        Wh_Log(L"No default capture device: 0x%08X", hr);
        LeaveCriticalSection(&g_audioLock);
        return false;
    }

    hr = g_pCaptureDevice->Activate(__uuidof(IAudioEndpointVolume),
                                     CLSCTX_ALL, nullptr,
                                     (void**)&g_pEndpointVolume);
    if (SUCCEEDED(hr) && g_pEndpointVolume) {
        g_pEndpointVolume->RegisterControlChangeNotify(&g_volumeCallback);
        BOOL muted = FALSE;
        g_pEndpointVolume->GetMute(&muted);
        g_micMuted = muted != FALSE;
    } else {
        Wh_Log(L"Failed to activate IAudioEndpointVolume: 0x%08X", hr);
    }

    HRESULT hrMeter = g_pCaptureDevice->Activate(
        IID_IAudioMeterInformation2, CLSCTX_ALL, nullptr,
        (void**)&g_pMeterInfo);
    if (FAILED(hrMeter)) {
        Wh_Log(
            L"Peak metering unavailable (0x%08X); falling back to a "
            L"synthetic pulse",
            hrMeter);
        g_pMeterInfo = nullptr;
    }

    LeaveCriticalSection(&g_audioLock);
    return true;
}

void CleanupAudio() {
    EnterCriticalSection(&g_audioLock);
    ReleaseAudioEndpoint_NoLock();
    if (g_pEnumerator) {
        g_pEnumerator->UnregisterEndpointNotificationCallback(
            &g_notificationClient);
        g_pEnumerator->Release();
        g_pEnumerator = nullptr;
    }
    LeaveCriticalSection(&g_audioLock);
}

void ToggleMicMute() {
    EnterCriticalSection(&g_audioLock);
    bool haveEndpoint = g_pEndpointVolume != nullptr;
    LeaveCriticalSection(&g_audioLock);

    // The cached endpoint is kept fresh by OnDefaultDeviceChanged (event-
    // driven), so a full re-resolve here isn't needed on the hot path —
    // only as a one-time fallback if nothing has been resolved yet.
    if (!haveEndpoint) {
        ResolveAudioEndpoint();
    }

    EnterCriticalSection(&g_audioLock);
    if (g_pEndpointVolume) {
        BOOL muted = FALSE;
        g_pEndpointVolume->GetMute(&muted);
        g_pEndpointVolume->SetMute(!muted, nullptr);
        g_micMuted = !muted;
    }
    LeaveCriticalSection(&g_audioLock);
}

float g_animPhase = 0.0f;

float GetMicPeak() {
    float peak = 0.0f;
    bool haveMeter;
    EnterCriticalSection(&g_audioLock);
    haveMeter = g_pMeterInfo != nullptr;
    if (haveMeter) {
        g_pMeterInfo->GetPeakValue(&peak);
    }
    LeaveCriticalSection(&g_audioLock);
    if (!haveMeter) {
        // Fallback so the ring still animates if real peak metering isn't
        // available on this system/build.
        peak = (sinf(g_animPhase * 6.2831853f) * 0.5f + 0.5f) * 0.5f;
    }
    return peak;
}

// ---------------------------------------------------------------------------
// Monitor / DPI helpers (item 6: work area, multi-monitor, DPI awareness)
// ---------------------------------------------------------------------------
UINT MonitorDpi(HMONITOR hMonitor) {
    UINT dpiX = 96, dpiY = 96;
    if (hMonitor) {
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    }
    return dpiX;
}

// Set when the user drags the overlay (only possible while click-through
// is off — see WM_NCHITTEST/WM_MOVE). Overrides the settings-computed
// position until the next settings change, which snaps back to configured
// placement.
bool g_manualPosition = false;
POINT g_manualPos = {0, 0};

void ComputeOverlayRect(RECT* out) {
    if (g_manualPosition) {
        HMONITOR hMonitor =
            MonitorFromPoint(g_manualPos, MONITOR_DEFAULTTONEAREST);
        UINT dpi = MonitorDpi(hMonitor);
        int size = MulDiv(g_settings.overlaySize, dpi, 96);
        out->left = g_manualPos.x;
        out->top = g_manualPos.y;
        out->right = g_manualPos.x + size;
        out->bottom = g_manualPos.y + size;
        return;
    }

    POINT pt;
    HMONITOR hMonitor;
    if (g_settings.overlayAutoPosition) {
        pt = {0, 0};
        hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    } else {
        pt = {g_settings.overlayPosX, g_settings.overlayPosY};
        hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMonitor, &mi);

    UINT dpi = MonitorDpi(hMonitor);
    int size = MulDiv(g_settings.overlaySize, dpi, 96);

    int x, y;
    if (g_settings.overlayAutoPosition) {
        int margin = MulDiv(g_settings.overlayMargin, dpi, 96);
        x = mi.rcWork.right - size - margin;
        y = mi.rcWork.bottom - size - margin;
    } else {
        x = g_settings.overlayPosX;
        y = g_settings.overlayPosY;
    }

    out->left = x;
    out->top = y;
    out->right = x + size;
    out->bottom = y + size;
}

// ---------------------------------------------------------------------------
// Overlay rendering
// ---------------------------------------------------------------------------
ULONG_PTR g_gdiplusToken = 0;
bool g_overlayVisible = false;
bool g_timerActive = false;
DWORD g_lastActivityTick = 0;

constexpr UINT_PTR TIMER_ANIM = 1;
constexpr UINT ANIM_MS = 33;  // ~30 fps
constexpr UINT_PTR kHotkeyId = 1;

// Cached render target: recreated only when the overlay's pixel size
// changes (e.g. a settings change or a DPI/monitor change), not on every
// animation frame — avoids ~30 GDI object create/destroy pairs per second
// while the overlay is up.
HDC g_memDC = nullptr;
HBITMAP g_hBitmap = nullptr;
void* g_bits = nullptr;
int g_renderTargetSize = 0;

bool EnsureRenderTarget(int size) {
    if (g_hBitmap && g_renderTargetSize == size) return true;

    if (g_memDC) {
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
    if (g_hBitmap) {
        DeleteObject(g_hBitmap);
        g_hBitmap = nullptr;
        g_bits = nullptr;
    }
    g_renderTargetSize = 0;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    g_hBitmap =
        CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &g_bits, nullptr, 0);
    ReleaseDC(nullptr, screenDC);
    if (!g_hBitmap || !g_bits) {
        g_hBitmap = nullptr;
        g_bits = nullptr;
        return false;
    }

    g_memDC = CreateCompatibleDC(nullptr);
    if (!g_memDC) {
        DeleteObject(g_hBitmap);
        g_hBitmap = nullptr;
        g_bits = nullptr;
        return false;
    }
    SelectObject(g_memDC, g_hBitmap);
    g_renderTargetSize = size;
    return true;
}

void ReleaseRenderTarget() {
    if (g_memDC) {
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
    if (g_hBitmap) {
        DeleteObject(g_hBitmap);
        g_hBitmap = nullptr;
        g_bits = nullptr;
    }
    g_renderTargetSize = 0;
}

void RenderOverlay(HWND hwnd, const RECT& rc, float peak, bool muted) {
    int size = rc.right - rc.left;
    if (size < 1) return;
    if (!EnsureRenderTarget(size)) return;

    {
        Bitmap bmp(size, size, size * 4, PixelFormat32bppPARGB, (BYTE*)g_bits);
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.Clear(Color(0, 0, 0, 0));

        float cx = size / 2.0f;
        float cy = size / 2.0f;
        float baseR = size * 0.42f;

        Color colActive(230, 46, 204, 113);
        Color colIdle(210, 90, 100, 110);
        Color colMuted(230, 231, 76, 60);

        if (!muted) {
            float level = peak * 1.6f;
            if (level > 1.0f) level = 1.0f;
            for (int ring = 0; ring < 3; ring++) {
                float t = g_animPhase - ring * 0.33f;
                t = t - floorf(t);
                float ringR = baseR + t * baseR * 0.9f * (0.3f + level);
                int alpha = (int)((1.0f - t) * 120 * (0.25f + level));
                if (alpha < 0) alpha = 0;
                if (level > 0.03f) {
                    Pen pen(Color(alpha, 46, 204, 113), 3.0f);
                    g.DrawEllipse(&pen, cx - ringR, cy - ringR, ringR * 2,
                                  ringR * 2);
                }
            }
        }

        SolidBrush bgBrush(muted ? colMuted
                                  : (peak > 0.03f ? colActive : colIdle));
        g.FillEllipse(&bgBrush, cx - baseR, cy - baseR, baseR * 2, baseR * 2);

        Pen ringPen(Color(255, 255, 255), size * 0.03f);
        g.DrawEllipse(&ringPen, cx - baseR, cy - baseR, baseR * 2, baseR * 2);

        float bodyW = baseR * 0.55f;
        float bodyH = baseR * 1.05f;
        RectF bodyRect(cx - bodyW / 2, cy - bodyH * 0.62f, bodyW, bodyH);
        GraphicsPath body;
        float rad = bodyW / 2.0f;
        body.AddArc(bodyRect.X, bodyRect.Y, bodyW, rad * 2, 180, 180);
        body.AddArc(bodyRect.X, bodyRect.Y + bodyRect.Height - rad * 2,
                    bodyW, rad * 2, 0, 180);
        body.CloseFigure();
        SolidBrush white(Color(255, 255, 255, 255));
        g.FillPath(&white, &body);

        Pen standPen(Color(255, 255, 255, 255), size * 0.045f);
        standPen.SetStartCap(LineCapRound);
        standPen.SetEndCap(LineCapRound);
        float standTop = cy + bodyH * 0.15f;
        float standBottom = cy + baseR * 0.55f;
        g.DrawArc(&standPen, cx - bodyW * 0.85f, cy - bodyH * 0.05f,
                  bodyW * 1.7f, bodyH * 0.75f, 20, 140);
        g.DrawLine(&standPen, cx, standTop + bodyH * 0.30f, cx, standBottom);
        g.DrawLine(&standPen, cx - baseR * 0.32f, standBottom,
                    cx + baseR * 0.32f, standBottom);

        if (muted) {
            Pen slashPen(Color(255, 255, 255, 255), size * 0.06f);
            slashPen.SetStartCap(LineCapRound);
            slashPen.SetEndCap(LineCapRound);
            g.DrawLine(&slashPen, cx - baseR * 0.9f, cy - baseR * 0.9f,
                       cx + baseR * 0.9f, cy + baseR * 0.9f);
        }
    }

    POINT ptDst = {rc.left, rc.top};
    SIZE sizeWnd = {size, size};
    POINT ptSrc = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    UpdateLayeredWindow(hwnd, nullptr, &ptDst, &sizeWnd, g_memDC, &ptSrc, 0,
                         &blend, ULW_ALPHA);
}

void RenderCurrentState(HWND hwnd, float peak) {
    RECT rc;
    ComputeOverlayRect(&rc);
    RenderOverlay(hwnd, rc, peak, g_micMuted);
}

void ApplyOverlayPositionAndSize(HWND hwnd) {
    RECT rc;
    ComputeOverlayRect(&rc);
    SetWindowPos(hwnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOACTIVATE);
}

// Timer only needs to run while there's something to animate (unmuted) or
// an auto-hide countdown in flight — never spins forever in the muted +
// alwaysShow case (item 8).
void EnsureTimerState(HWND hwnd) {
    bool needTimer =
        !g_micMuted || (g_overlayVisible && !g_settings.alwaysShow);
    if (needTimer && !g_timerActive) {
        SetTimer(hwnd, TIMER_ANIM, ANIM_MS, nullptr);
        g_timerActive = true;
    } else if (!needTimer && g_timerActive) {
        KillTimer(hwnd, TIMER_ANIM);
        g_timerActive = false;
    }
}

void ShowOverlayTemporarily(HWND hwnd) {
    g_lastActivityTick = GetTickCount();
    if (!g_overlayVisible) {
        g_overlayVisible = true;
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }
}

void RegisterGlobalHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, kHotkeyId);

    UINT mods = 0;
    if (g_settings.hotkeyModifiers & 1) mods |= MOD_ALT;
    if (g_settings.hotkeyModifiers & 2) mods |= MOD_CONTROL;
    if (g_settings.hotkeyModifiers & 4) mods |= MOD_SHIFT;
    if (g_settings.hotkeyModifiers & 8) mods |= MOD_WIN;

    if (mods == 0 || g_settings.hotkeyVK == 0) {
        Wh_Log(
            L"Hotkey not registered: could not parse a valid hotkey from "
            L"'%s' (need at least one modifier and one key)",
            g_settings.hotkeyRaw);
        return;
    }

    mods |= MOD_NOREPEAT;
    if (!RegisterHotKey(hwnd, kHotkeyId, mods, g_settings.hotkeyVK)) {
        Wh_Log(
            L"RegisterHotKey failed: %u (another app may already use this "
            L"combo)",
            GetLastError());
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY: {
            if (wParam == kHotkeyId) {
                ToggleMicMute();
                ShowOverlayTemporarily(hwnd);
                RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
                EnsureTimerState(hwnd);
            }
            return 0;
        }
        case kMsgMuteChanged: {
            g_micMuted = wParam != 0;
            ShowOverlayTemporarily(hwnd);
            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            EnsureTimerState(hwnd);
            return 0;
        }
        case kMsgDeviceChanged: {
            ResolveAudioEndpoint();
            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            EnsureTimerState(hwnd);
            return 0;
        }
        case kMsgSettingsChanged: {
            RegisterGlobalHotkey(hwnd);

            LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            ex = g_settings.clickThrough ? (ex | WS_EX_TRANSPARENT)
                                          : (ex & ~WS_EX_TRANSPARENT);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex);

            g_manualPosition = false;
            ApplyOverlayPositionAndSize(hwnd);

            if (g_settings.alwaysShow && !g_overlayVisible) {
                ShowOverlayTemporarily(hwnd);
            }

            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            EnsureTimerState(hwnd);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_ANIM) {
                if (!g_micMuted) {
                    g_animPhase += 0.045f;
                    if (g_animPhase > 1.0f) g_animPhase -= 1.0f;
                }
                float peak = g_micMuted ? 0.0f : GetMicPeak();
                RenderCurrentState(hwnd, peak);

                if (!g_settings.alwaysShow && g_overlayVisible) {
                    bool activeSignal = !g_micMuted && peak > 0.03f;
                    DWORD elapsed = GetTickCount() - g_lastActivityTick;
                    if (!activeSignal &&
                        elapsed > (DWORD)g_settings.overlayDurationMs) {
                        ShowWindow(hwnd, SW_HIDE);
                        g_overlayVisible = false;
                    }
                }
                EnsureTimerState(hwnd);
            }
            return 0;
        }
        case WM_NCHITTEST: {
            // Only draggable while click-through is off — with it on, the
            // window is WS_EX_TRANSPARENT and never receives mouse input
            // anyway, so this only matters in the non-click-through case.
            if (!g_settings.clickThrough) {
                return HTCAPTION;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        case WM_MOVE: {
            if (!g_settings.clickThrough) {
                g_manualPos.x = (short)LOWORD(lParam);
                g_manualPos.y = (short)HIWORD(lParam);
                g_manualPosition = true;
            }
            return 0;
        }
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ANIM);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

const wchar_t kClassName[] = L"WH_MicMuteOverlayWnd";

HWND CreateOverlayWindow() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
        return nullptr;
    }

    DWORD exStyle =
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    if (g_settings.clickThrough) exStyle |= WS_EX_TRANSPARENT;

    RECT rc;
    ComputeOverlayRect(&rc);

    HWND hwnd = CreateWindowEx(exStyle, kClassName, L"MicMuteOverlay",
                                WS_POPUP, rc.left, rc.top, rc.right - rc.left,
                                rc.bottom - rc.top, nullptr, nullptr,
                                GetCurrentModuleHandle(), nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
    }
    return hwnd;
}

// ---------------------------------------------------------------------------
// Worker thread: owns the overlay/hotkey window and its message loop
// ---------------------------------------------------------------------------
HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;

DWORD WINAPI ModThreadProc(LPVOID) {
    MSG msg;
    // Ensure a message queue exists before WhTool_ModUninit's
    // PostThreadMessageW can be relied on to reach it.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput gdiInput;
    GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr);

    g_hOverlay = CreateOverlayWindow();
    if (g_hOverlay) {
        ResolveAudioEndpoint();
        RegisterGlobalHotkey(g_hOverlay);
        if (g_settings.alwaysShow) {
            ShowOverlayTemporarily(g_hOverlay);
        }
        RenderCurrentState(g_hOverlay, g_micMuted ? 0.0f : GetMicPeak());
        EnsureTimerState(g_hOverlay);
    }

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanupAudio();

    if (g_hOverlay) {
        UnregisterHotKey(g_hOverlay, kHotkeyId);
        DestroyWindow(g_hOverlay);
        g_hOverlay = nullptr;
    }
    UnregisterClass(kClassName, GetCurrentModuleHandle());

    ReleaseRenderTarget();
    if (g_gdiplusToken) GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Tool-mod entry points (called by the launcher snippet below)
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    LoadSettings();
    InitializeCriticalSection(&g_audioLock);
    g_hThread = CreateThread(nullptr, 0, ModThreadProc, nullptr, 0,
                              &g_threadId);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hOverlay) {
        PostMessage(g_hOverlay, kMsgSettingsChanged, 0, 0);
    }
    // If g_hOverlay isn't created yet, ModThreadProc will pick up the
    // already-updated g_settings when it creates the window.
}

void WhTool_ModUninit() {
    if (g_threadId) {
        // Retry until the thread's message queue exists (see the
        // PeekMessageW call at the top of ModThreadProc) so this can't be
        // dropped, then wait for a clean exit — never a timeout, which
        // would risk FreeLibrary-ing the mod out from under a running
        // thread.
        while (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0) &&
               WaitForSingleObject(g_hThread, 10) == WAIT_TIMEOUT) {
        }
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    DeleteCriticalSection(&g_audioLock);
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

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
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
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                1];
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
        DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
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
                                  nullptr, nullptr, FALSE,
                                  NORMAL_PRIORITY_CLASS, nullptr, nullptr,
                                  &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
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