// ==WindhawkMod==
// @id              mic-mute-hotkey-overlay
// @name            Global Hotkey Mute Microphone
// @description     Global hotkey to mute/unmute the default microphone, with a floating always-on-top overlay that animates while the mic is active. Optional: show-only-when-muted, mod on/off hotkey, red screen border, all-monitor overlay, mute all microphones
// @description:id-ID Hotkey global untuk mute/unmute mikrofon default, dengan indikator overlay melayang yang selalu di atas dan beranimasi saat mikrofon aktif. Opsional: ikon hanya saat mute, hotkey on/off mod, garis merah di pinggir layar, overlay di semua monitor, mute semua mikrofon
// @version         1.1.0
// @author          Farael Hanafi
// @github          https://github.com/Eliasilyz
// @homepage        https://farelhanafi.my.id/
// @include         windhawk.exe
// @compilerOptions -lole32 -luser32 -lgdi32 -lgdiplus -lshcore -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Hotkey Mute Microphone

Binds a global hotkey to toggle mute on your default (or all) microphones, with a highly customizable floating overlay or screen border indicator.

### Preview
| Muted | Unmuted |
| :---: | :---: |
| ![Muted](https://raw.githubusercontent.com/Eliasilyz/global-hotkey-mute/refs/heads/main/9fnqpbp.png) | ![Unmuted](https://raw.githubusercontent.com/Eliasilyz/global-hotkey-mute/refs/heads/main/74488pt.png) |

## Features

- **Intuitive Global Hotkeys**: Uses plain strings (e.g. `"Ctrl+Alt+M"`). Includes an optional hotkey to toggle the entire mod on/off on the fly.
- **Event-Driven & In-Sync**: Instant UI updates powered by Core Audio APIs (`IMMNotificationClient`). Zero polling, zero CPU waste.
- **Real-Time Visual Feedback**: Pulsing ring driven by actual mic peak levels (`IAudioMeterInformation`).
- **Flexible Display Options**: 
  - Show floating overlay or a full-screen **Red Border** frame.
  - Option to display overlay on **all active monitors**.
  - Option to show the indicator **only while muted**.
- **Multi-Device Support**: Can mute all active audio capture devices simultaneously or stick to the default microphone.
- **Seamless Drag & Drop**: Easily reposition the overlay when click-through is disabled.
- **DPI-Aware & Performance Optimized**: Double-buffered render target with automatic taskbar work-area alignment.

## How this differs from similar mods

- [mic-tray-control](https://windhawk.net/mods/mic-tray-control) and
  [mutealert](https://windhawk.net/mods/mutealert) both surface the default
  mic's mute state and let you toggle it, but through the tray/taskbar —
  neither has a global hotkey or a floating overlay.
- [keyboard-shortcut-actions](https://windhawk.net/mods/keyboard-shortcut-actions)
  is a general hotkey→action framework and already includes a "Mute system
  volume" action using the same `Ctrl+Alt+M`-style hotkey string format.
  This mod overlaps it on the hotkey side, but its actions target output
  (render) devices, not the default microphone (capture), and it has no
  overlay indicator. The floating, animated overlay is this mod's actual
  differentiator; the hotkey parsing exists mainly to drive that overlay
  reliably rather than to duplicate a general-purpose action framework.

## Known Limitations

- **Exclusive Fullscreen**: Normal topmost windows won't render over exclusive-fullscreen games.
- **Voice Agnostic**: Peak metering reflects raw input level and cannot distinguish voice from background noise.
- **Multi-Mic Scope**: When "Mute all microphones" is enabled, external system-level mute changes only sync with the default mic.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkey: "Ctrl+Alt+M"
  $name: Hotkey
  $description: >-
    Combine modifiers with '+': Ctrl, Alt, Shift, Win, plus exactly one
    key — a letter, digit, F1-F24, a name like Space/Tab/Enter/Esc/Up/
    Down/Left/Right/Insert/Delete/Home/End/PageUp/PageDown/Menu, a numpad
    key like Num0-Num9/NumAdd/NumSub/NumMul/NumDiv, or a single
    punctuation/OEM character (e.g. ';', '/', '[') resolved for your
    current keyboard layout. At least one modifier is required — a hotkey
    with no modifier is rejected instead
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
  $description: >-
    How long the overlay stays up after a toggle (or after a drag ends)
    before it auto-hides. If "Keep visible while mic is active" is on, an
    unmuted mic with signal keeps the overlay up past this delay for as
    long as that continues.
- alwaysShow: false
  $name: Always show overlay (ignore auto-hide)
- keepVisibleWhileActive: false
  $name: Keep visible while mic is active
  $description: >-
    When enabled, the overlay stays on screen for as long as the mic is
    unmuted and producing signal, instead of hiding after the auto-hide
    delay above. Has no effect when "Always show overlay" is on.
- clickThrough: true
  $name: Click-through overlay (don't block mouse)
  $description: >-
    When enabled, clicks pass through the overlay to whatever's underneath.
    When disabled, the overlay blocks clicks and can be dragged to
    reposition it.
- showOnlyWhenMuted: false
  $name: Show icon only while muted
  $description: >-
    The floating icon is hidden while the mic is active and shown for as
    long as the mic is muted. Overrides "Always show overlay" and the
    auto-hide delay while the mic is active.
- modToggleHotkey: ""
  $name: Hotkey to turn the whole mod on/off
  $description: >-
    Optional, empty (disabled) by default. Same format as the main hotkey,
    e.g. "Ctrl+Alt+Shift+M"; at least one modifier is required and it must
    differ from the main hotkey. While the mod is off, the mute hotkey is
    released and the overlay/border are hidden. A system beep plays on every
    on/off toggle.
- redBorderMode: false
  $name: Red screen border instead of the mute icon
  $description: >-
    While muted, draw a red frame around the screen edge instead of the
    floating icon. While the mic is active, the normal icon behaviour
    applies.
- redBorderThickness: 6
  $name: Red border thickness (px)
  $description: "Logical pixels at 96 DPI (1-64); scaled per monitor DPI"
- allMonitors: false
  $name: Show overlay on all monitors
  $description: >-
    Show the icon (and the red border, if enabled) on every active monitor,
    not just one. Copies on secondary monitors are auto-positioned at the
    bottom-right of their work area and are always click-through.
- muteAllMicrophones: false
  $name: Mute/unmute all microphones
  $description: >-
    The hotkey toggles every active capture device at once instead of only
    the default one. Note that unmuting also unmutes every active capture
    device, including ones you may have muted on purpose.
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
#include <windhawk_utils.h>

using namespace Gdiplus;

// ---------------------------------------------------------------------------
// Manually-declared interface
// ---------------------------------------------------------------------------
// IAudioMeterInformation is only forward-declared (no method table) by this
// compiler's endpointvolume.h. Declared here under its canonical name,
// guarded so it's skipped once the toolchain header provides a full
// definition — same approach as mutealert.wh.cpp in this repo.
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
MIDL_INTERFACE("c02216f6-8c67-4b5b-9d00-d008e73e0064")
IAudioMeterInformation : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioMeterInformation,
                 0xc02216f6,
                 0x8c67,
                 0x4b5b,
                 0x9d,
                 0x00,
                 0xd0,
                 0x08,
                 0xe7,
                 0x3e,
                 0x00,
                 0x64)
#endif
#endif

// ---------------------------------------------------------------------------
// Module handle — NOT GetModuleHandle(nullptr), which would return the
// host windhawk.exe's handle rather than this mod DLL's.
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
        {L"MENU", VK_APPS},
        {L"NUM0", VK_NUMPAD0},      {L"NUM1", VK_NUMPAD1},
        {L"NUM2", VK_NUMPAD2},      {L"NUM3", VK_NUMPAD3},
        {L"NUM4", VK_NUMPAD4},      {L"NUM5", VK_NUMPAD5},
        {L"NUM6", VK_NUMPAD6},      {L"NUM7", VK_NUMPAD7},
        {L"NUM8", VK_NUMPAD8},      {L"NUM9", VK_NUMPAD9},
        {L"NUMADD", VK_ADD},        {L"NUMSUB", VK_SUBTRACT},
        {L"NUMMUL", VK_MULTIPLY},   {L"NUMDIV", VK_DIVIDE},
        {L"NUMDECIMAL", VK_DECIMAL},
    };

    wchar_t upper[32];
    int i = 0;
    for (; name[i] && i < 31; i++) upper[i] = towupper(name[i]);
    upper[i] = L'\0';

    for (const auto& e : kNamedKeys) {
        if (wcscmp(upper, e.name) == 0) return e.vk;
    }

    // Fallback for anything not covered above: punctuation and other
    // OEM-layout keys (';', '/', '[', etc.) that VkKeyScanW can resolve
    // for the current keyboard layout, letters/digits already handled
    // earlier. Only applies to single-character tokens.
    if (name[0] != L'\0' && name[1] == L'\0') {
        SHORT scan = VkKeyScanW(name[0]);
        if (scan != -1) {
            return LOBYTE(scan);
        }
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
    bool keepVisibleWhileActive;
    bool clickThrough;
    bool showOnlyWhenMuted;
    UINT toggleModifiers;
    UINT toggleVK;
    wchar_t toggleRaw[64];
    bool redBorderMode;
    int redBorderThickness;
    bool allMonitors;
    bool muteAllMicrophones;
} g_settings;

void LoadSettings() {
    // Wh_GetStringSetting never returns NULL (it returns L"" on error/unset).
    PCWSTR hotkeyStr = Wh_GetStringSetting(L"hotkey");
    wcsncpy(g_settings.hotkeyRaw, hotkeyStr,
            ARRAYSIZE(g_settings.hotkeyRaw) - 1);
    g_settings.hotkeyRaw[ARRAYSIZE(g_settings.hotkeyRaw) - 1] = L'\0';
    g_settings.hotkeyModifiers = 0;
    g_settings.hotkeyVK = 0;
    ParseHotkeyString(hotkeyStr, &g_settings.hotkeyModifiers,
                       &g_settings.hotkeyVK);
    Wh_FreeStringSetting(hotkeyStr);

    g_settings.overlaySize = (int)Wh_GetIntSetting(L"overlaySize");
    g_settings.overlayAutoPosition =
        Wh_GetIntSetting(L"overlayAutoPosition") != 0;
    g_settings.overlayPosX = (int)Wh_GetIntSetting(L"overlayPosX");
    g_settings.overlayPosY = (int)Wh_GetIntSetting(L"overlayPosY");
    g_settings.overlayMargin = (int)Wh_GetIntSetting(L"overlayMargin");
    g_settings.overlayDurationMs = (int)Wh_GetIntSetting(L"overlayDurationMs");
    g_settings.alwaysShow = Wh_GetIntSetting(L"alwaysShow") != 0;
    g_settings.keepVisibleWhileActive =
        Wh_GetIntSetting(L"keepVisibleWhileActive") != 0;
    g_settings.clickThrough = Wh_GetIntSetting(L"clickThrough") != 0;

    g_settings.showOnlyWhenMuted =
        Wh_GetIntSetting(L"showOnlyWhenMuted") != 0;
    g_settings.redBorderMode = Wh_GetIntSetting(L"redBorderMode") != 0;
    g_settings.redBorderThickness =
        (int)Wh_GetIntSetting(L"redBorderThickness");
    g_settings.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    g_settings.muteAllMicrophones =
        Wh_GetIntSetting(L"muteAllMicrophones") != 0;

    WindhawkUtils::StringSetting toggleStr(
        Wh_GetStringSetting(L"modToggleHotkey"));
    wcsncpy(g_settings.toggleRaw, toggleStr.get(),
            ARRAYSIZE(g_settings.toggleRaw) - 1);
    g_settings.toggleRaw[ARRAYSIZE(g_settings.toggleRaw) - 1] = L'\0';
    g_settings.toggleModifiers = 0;
    g_settings.toggleVK = 0;
    if (toggleStr.get()[0] != L'\0') {
        ParseHotkeyString(toggleStr.get(), &g_settings.toggleModifiers,
                           &g_settings.toggleVK);
    }

    if (g_settings.redBorderThickness < 1) g_settings.redBorderThickness = 1;
    if (g_settings.redBorderThickness > 64)
        g_settings.redBorderThickness = 64;

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
// ResolveAudioEndpoint, ToggleMicMute, GetMicPeak, and CleanupAudio only
// ever run on the overlay thread; the two COM notification callbacks below
// deliberately only PostMessage rather than touching these pointers
// directly, so no locking is needed here.
IMMDeviceEnumerator* g_pEnumerator = nullptr;
IMMDevice* g_pCaptureDevice = nullptr;
IAudioEndpointVolume* g_pEndpointVolume = nullptr;
IAudioMeterInformation* g_pMeterInfo = nullptr;
bool g_micMuted = false;
// False while the whole mod is switched off with the toggle hotkey.
// Runtime only: the mod always starts enabled.
bool g_modEnabled = true;

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

// Re-resolves the default capture device and its interfaces. Called once
// at startup, on an OnDefaultDeviceChanged event, and as a one-time
// fallback from ToggleMicMute if nothing has been resolved yet — not on
// every toggle, since the cached endpoint is kept fresh by the event path.
bool ResolveAudioEndpoint() {
    ReleaseAudioEndpoint_NoLock();

    if (!g_pEnumerator) {
        HRESULT hr =
            CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                              CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                              (void**)&g_pEnumerator);
        if (FAILED(hr) || !g_pEnumerator) {
            Wh_Log(L"Failed to create device enumerator: 0x%08X", hr);
            return false;
        }
        g_pEnumerator->RegisterEndpointNotificationCallback(
            &g_notificationClient);
    }

    HRESULT hr = g_pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole,
                                                          &g_pCaptureDevice);
    if (FAILED(hr) || !g_pCaptureDevice) {
        Wh_Log(L"No default capture device: 0x%08X", hr);
        return false;
    }

    hr = g_pCaptureDevice->Activate(__uuidof(IAudioEndpointVolume),
                                     CLSCTX_ALL, nullptr,
                                     (void**)&g_pEndpointVolume);
    if (SUCCEEDED(hr) && g_pEndpointVolume) {
        g_pEndpointVolume->RegisterControlChangeNotify(&g_volumeCallback);
        BOOL muted = FALSE;
        if (SUCCEEDED(g_pEndpointVolume->GetMute(&muted))) {
            g_micMuted = muted != FALSE;
        } else {
            Wh_Log(L"GetMute failed; assuming unmuted until the next update");
        }
    } else {
        Wh_Log(L"Failed to activate IAudioEndpointVolume: 0x%08X", hr);
    }

    HRESULT hrMeter = g_pCaptureDevice->Activate(
        __uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr,
        (void**)&g_pMeterInfo);
    if (FAILED(hrMeter)) {
        Wh_Log(
            L"Peak metering unavailable (0x%08X); falling back to a "
            L"synthetic pulse",
            hrMeter);
        g_pMeterInfo = nullptr;
    }

    return true;
}

void CleanupAudio() {
    ReleaseAudioEndpoint_NoLock();
    if (g_pEnumerator) {
        g_pEnumerator->UnregisterEndpointNotificationCallback(
            &g_notificationClient);
        g_pEnumerator->Release();
        g_pEnumerator = nullptr;
    }
}

// Sets mute on EVERY active capture device (used by "Mute all microphones").
// Endpoints are enumerated on demand so devices plugged in / removed since
// the last press are handled without extra notification plumbing.
void SetAllCaptureMute(BOOL mute) {
    if (!g_pEnumerator) return;

    IMMDeviceCollection* pCollection = nullptr;
    HRESULT hr = g_pEnumerator->EnumAudioEndpoints(
        eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr) || !pCollection) {
        Wh_Log(L"EnumAudioEndpoints failed: 0x%08X", hr);
        return;
    }

    UINT count = 0;
    pCollection->GetCount(&count);
    for (UINT i = 0; i < count; i++) {
        IMMDevice* pDevice = nullptr;
        if (FAILED(pCollection->Item(i, &pDevice)) || !pDevice) continue;

        IAudioEndpointVolume* pVolume = nullptr;
        if (SUCCEEDED(pDevice->Activate(__uuidof(IAudioEndpointVolume),
                                         CLSCTX_ALL, nullptr,
                                         (void**)&pVolume)) &&
            pVolume) {
            if (FAILED(pVolume->SetMute(mute, nullptr))) {
                Wh_Log(L"SetMute failed on capture device #%u", i);
            }
            pVolume->Release();
        }
        pDevice->Release();
    }
    pCollection->Release();
}

void ToggleMicMute() {
    // The cached endpoint is kept fresh by OnDefaultDeviceChanged (event-
    // driven), so a full re-resolve here isn't needed on the hot path —
    // only as a one-time fallback if nothing has been resolved yet.
    if (!g_pEndpointVolume) {
        ResolveAudioEndpoint();
    }

    if (g_pEndpointVolume) {
        BOOL muted = FALSE;
        if (SUCCEEDED(g_pEndpointVolume->GetMute(&muted)) &&
            SUCCEEDED(g_pEndpointVolume->SetMute(!muted, nullptr))) {
            g_micMuted = !muted;
            if (g_settings.muteAllMicrophones) {
                SetAllCaptureMute(g_micMuted ? TRUE : FALSE);
            }
        } else {
            Wh_Log(L"Mute toggle failed; leaving indicator state unchanged");
        }
    }
}

float g_animPhase = 0.0f;

float GetMicPeak() {
    if (g_pMeterInfo) {
        float peak = 0.0f;
        g_pMeterInfo->GetPeakValue(&peak);
        return peak;
    }
    // Fallback so the ring still animates if real peak metering isn't
    // available on this system/build.
    return (sinf(g_animPhase * 6.2831853f) * 0.5f + 0.5f) * 0.5f;
}

// ---------------------------------------------------------------------------
// Monitor / DPI helpers — work area (not raw screen size), multi-monitor,
// and per-monitor DPI scaling.
// ---------------------------------------------------------------------------
UINT MonitorDpi(HMONITOR hMonitor) {
    UINT dpiX = 96, dpiY = 96;
    if (hMonitor) {
        UINT resultX = 96, resultY = 96;
        if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &resultX,
                                        &resultY)) &&
            resultX > 0) {
            dpiX = resultX;
        }
    }
    return dpiX;
}

// Set when the user drags the overlay (only possible while click-through
// is off — see WM_NCHITTEST/WM_EXITSIZEMOVE). Overrides the settings-computed
// position until the next settings change, which snaps back to configured
// placement.
bool g_manualPosition = false;
POINT g_manualPos = {0, 0};

// True for the duration of a user-driven move loop (WM_ENTERSIZEMOVE to
// WM_EXITSIZEMOVE). While dragging, RenderOverlay must not pass pptDst to
// UpdateLayeredWindow — the move loop already owns the window's position,
// and doing both fights every ~33ms timer tick, producing a jittering drag
// whose drop position is whichever repositioning happened to run last.
bool g_dragging = false;

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
        int workW = mi.rcWork.right - mi.rcWork.left;
        int workH = mi.rcWork.bottom - mi.rcWork.top;
        int smallerDim = workW < workH ? workW : workH;
        int maxMargin = (smallerDim - size) / 2;
        if (maxMargin < 0) maxMargin = 0;
        if (margin > maxMargin) margin = maxMargin;
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
constexpr UINT_PTR kToggleHotkeyId = 2;

constexpr int kMaxAuxMonitors = 16;

// Cached render targets, one per overlay window (slot 0 = main overlay,
// slots 1..N = copies on other monitors). Each is recreated only when its
// pixel size changes (settings/DPI/monitor change), not on every animation
// frame, and monitors with different DPI don't thrash a shared buffer.
struct RenderTarget {
    HDC dc;
    HBITMAP bmp;
    void* bits;
    int size;
};
RenderTarget g_targets[kMaxAuxMonitors + 1] = {};

void ReleaseTarget(RenderTarget& t) {
    if (t.dc) {
        DeleteDC(t.dc);
        t.dc = nullptr;
    }
    if (t.bmp) {
        DeleteObject(t.bmp);
        t.bmp = nullptr;
    }
    t.bits = nullptr;
    t.size = 0;
}

bool EnsureRenderTarget(RenderTarget& t, int size) {
    if (t.bmp && t.size == size) return true;
    ReleaseTarget(t);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    t.bmp = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &t.bits,
                              nullptr, 0);
    ReleaseDC(nullptr, screenDC);
    if (!t.bmp || !t.bits) {
        ReleaseTarget(t);
        return false;
    }

    t.dc = CreateCompatibleDC(nullptr);
    if (!t.dc) {
        ReleaseTarget(t);
        return false;
    }
    SelectObject(t.dc, t.bmp);
    t.size = size;
    return true;
}

void ReleaseRenderTarget() {
    for (auto& t : g_targets) ReleaseTarget(t);
}

void RenderOverlay(HWND hwnd,
                   const RECT& rc,
                   float peak,
                   bool muted,
                   int slot = 0) {
    int size = rc.right - rc.left;
    if (size < 1) return;
    RenderTarget& rt = g_targets[slot];
    if (!EnsureRenderTarget(rt, size)) return;

    {
        Bitmap bmp(size, size, size * 4, PixelFormat32bppPARGB, (BYTE*)rt.bits);
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

    // Only the main overlay (slot 0) can be dragged by the user.
    UpdateLayeredWindow(hwnd, nullptr,
                         (slot == 0 && g_dragging) ? nullptr : &ptDst,
                         &sizeWnd, rt.dc, &ptSrc, 0, &blend, ULW_ALPHA);
}

// ---------------------------------------------------------------------------
// Auxiliary windows: overlay copies on other monitors + red screen border
// ---------------------------------------------------------------------------
const wchar_t kExtraClassName[] = L"WH_MicMuteOverlayExtraWnd";
const wchar_t kBorderClassName[] = L"WH_MicMuteBorderWnd";

struct ExtraOverlay {
    HWND hwnd;
    HMONITOR mon;
};
ExtraOverlay g_extras[kMaxAuxMonitors] = {};
int g_extraCount = 0;

HWND g_borderWnds[kMaxAuxMonitors * 4] = {};
int g_borderCount = 0;
bool g_borderShown = false;

HMONITOR g_auxMainMon = nullptr;  // main overlay monitor at last rebuild
HBRUSH g_borderBrush = nullptr;
bool g_auxClassesRegistered = false;

// Auto placement (bottom-right of the work area) for an arbitrary monitor,
// same rules as the auto-position branch of ComputeOverlayRect.
void ComputeAutoRectForMonitor(HMONITOR hMonitor, RECT* out) {
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(hMonitor, &mi);

    UINT dpi = MonitorDpi(hMonitor);
    int size = MulDiv(g_settings.overlaySize, dpi, 96);
    int margin = MulDiv(g_settings.overlayMargin, dpi, 96);
    int workW = mi.rcWork.right - mi.rcWork.left;
    int workH = mi.rcWork.bottom - mi.rcWork.top;
    int smallerDim = workW < workH ? workW : workH;
    int maxMargin = (smallerDim - size) / 2;
    if (maxMargin < 0) maxMargin = 0;
    if (margin > maxMargin) margin = maxMargin;

    out->left = mi.rcWork.right - size - margin;
    out->top = mi.rcWork.bottom - size - margin;
    out->right = out->left + size;
    out->bottom = out->top + size;
}

// Never activates, never takes the mouse.
LRESULT CALLBACK PassiveWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool EnsureAuxClasses() {
    if (g_auxClassesRegistered) return true;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = PassiveWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = kExtraClassName;
    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx (extra) failed: %u", GetLastError());
        return false;
    }

    g_borderBrush = CreateSolidBrush(RGB(220, 30, 30));
    wc.lpszClassName = kBorderClassName;
    wc.hbrBackground = g_borderBrush;
    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx (border) failed: %u", GetLastError());
        UnregisterClass(kExtraClassName, GetCurrentModuleHandle());
        DeleteObject(g_borderBrush);
        g_borderBrush = nullptr;
        return false;
    }

    g_auxClassesRegistered = true;
    return true;
}

void ReleaseAuxClasses() {
    if (!g_auxClassesRegistered) return;
    UnregisterClass(kExtraClassName, GetCurrentModuleHandle());
    UnregisterClass(kBorderClassName, GetCurrentModuleHandle());
    // The class owns hbrBackground and frees it in UnregisterClass.
    g_borderBrush = nullptr;
    g_auxClassesRegistered = false;
}

void DestroyExtras() {
    for (int i = 0; i < g_extraCount; i++) {
        if (g_extras[i].hwnd) DestroyWindow(g_extras[i].hwnd);
        g_extras[i] = {};
        ReleaseTarget(g_targets[i + 1]);
    }
    g_extraCount = 0;
}

void DestroyBorders() {
    for (int i = 0; i < g_borderCount; i++) {
        if (g_borderWnds[i]) DestroyWindow(g_borderWnds[i]);
        g_borderWnds[i] = nullptr;
    }
    g_borderCount = 0;
    g_borderShown = false;
}

BOOL CALLBACK ExtraMonitorProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
    if (hMonitor == (HMONITOR)lParam) return TRUE;  // main overlay's monitor
    if (g_extraCount >= kMaxAuxMonitors) return FALSE;

    RECT rc;
    ComputeAutoRectForMonitor(hMonitor, &rc);
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kExtraClassName, L"MicMuteOverlayExtra", WS_POPUP, rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
        GetCurrentModuleHandle(), nullptr);
    if (hwnd) {
        g_extras[g_extraCount].hwnd = hwnd;
        g_extras[g_extraCount].mon = hMonitor;
        g_extraCount++;
    }
    return TRUE;
}

HWND MakeBorderStrip(int x, int y, int w, int h) {
    if (w < 1 || h < 1) return nullptr;
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kBorderClassName, L"MicMuteBorder", WS_POPUP, x, y, w, h, nullptr,
        nullptr, GetCurrentModuleHandle(), nullptr);
    if (hwnd) SetLayeredWindowAttributes(hwnd, 0, 235, LWA_ALPHA);
    return hwnd;
}

// lParam: the only monitor to frame, or nullptr for every monitor.
BOOL CALLBACK BorderMonitorProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
    HMONITOR only = (HMONITOR)lParam;
    if (only && hMonitor != only) return TRUE;
    if (g_borderCount + 4 > (int)ARRAYSIZE(g_borderWnds)) return FALSE;

    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMonitor, &mi)) return TRUE;
    const RECT& r = mi.rcMonitor;
    int W = r.right - r.left;
    int H = r.bottom - r.top;

    int th = MulDiv(g_settings.redBorderThickness, MonitorDpi(hMonitor), 96);
    if (th < 1) th = 1;
    int maxTh = (W < H ? W : H) / 2;
    if (maxTh < 1) maxTh = 1;
    if (th > maxTh) th = maxTh;

    HWND strips[4] = {
        MakeBorderStrip(r.left, r.top, W, th),                     // top
        MakeBorderStrip(r.left, r.bottom - th, W, th),             // bottom
        MakeBorderStrip(r.left, r.top + th, th, H - 2 * th),       // left
        MakeBorderStrip(r.right - th, r.top + th, th, H - 2 * th)  // right
    };
    for (HWND h : strips) {
        if (h) g_borderWnds[g_borderCount++] = h;
    }
    return TRUE;
}

void RenderExtras(float peak) {
    for (int i = 0; i < g_extraCount; i++) {
        RECT rc;
        ComputeAutoRectForMonitor(g_extras[i].mon, &rc);
        RenderOverlay(g_extras[i].hwnd, rc, peak, g_micMuted, i + 1);
    }
}

void ShowExtras(bool show) {
    for (int i = 0; i < g_extraCount; i++) {
        if (show) {
            SetWindowPos(g_extras[i].hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW);
        } else {
            ShowWindow(g_extras[i].hwnd, SW_HIDE);
        }
    }
}

// Border is shown while muted (persistently) whenever the mod is enabled.
void SyncBorder() {
    bool want = g_modEnabled && g_settings.redBorderMode && g_micMuted &&
                g_borderCount > 0;
    if (want == g_borderShown) return;
    for (int i = 0; i < g_borderCount; i++) {
        if (want) {
            SetWindowPos(g_borderWnds[i], HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW);
        } else {
            ShowWindow(g_borderWnds[i], SW_HIDE);
        }
    }
    g_borderShown = want;
}

// (Re)creates the monitor copies and the border strips for the current
// settings and monitor layout. Called on startup, settings change, display
// change, and after a drag ends (the main overlay may have moved to another
// monitor).
void RebuildAuxWindows() {
    DestroyExtras();
    DestroyBorders();

    RECT mainRc;
    ComputeOverlayRect(&mainRc);
    HMONITOR mainMon = MonitorFromRect(&mainRc, MONITOR_DEFAULTTONEAREST);
    g_auxMainMon = mainMon;

    if (!g_settings.allMonitors && !g_settings.redBorderMode) return;
    if (!EnsureAuxClasses()) return;

    if (g_settings.allMonitors) {
        EnumDisplayMonitors(nullptr, nullptr, ExtraMonitorProc,
                            (LPARAM)mainMon);
    }
    if (g_settings.redBorderMode) {
        EnumDisplayMonitors(nullptr, nullptr, BorderMonitorProc,
                            (LPARAM)(g_settings.allMonitors ? nullptr
                                                            : mainMon));
    }

    if (g_overlayVisible && g_extraCount > 0) {
        RenderExtras(g_micMuted ? 0.0f : GetMicPeak());
        ShowExtras(true);
    }
    SyncBorder();
}

void RenderCurrentState(HWND hwnd, float peak) {
    RECT rc;
    ComputeOverlayRect(&rc);
    RenderOverlay(hwnd, rc, peak, g_micMuted, 0);
    RenderExtras(peak);
}

void ApplyOverlayPositionAndSize(HWND hwnd) {
    RECT rc;
    ComputeOverlayRect(&rc);
    SetWindowPos(hwnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_NOACTIVATE);
}

// The timer only needs to run while the overlay is actually visible —
// nothing animates or auto-hides while it's hidden, and peak level never
// triggers a show by itself (only the hotkey and mute-change messages do).
void EnsureTimerState(HWND hwnd) {
    // In "show only while muted" mode the overlay is only up while muted,
    // where nothing animates or auto-hides, so no timer is needed.
    bool needTimer = g_modEnabled && g_overlayVisible &&
                     !g_settings.showOnlyWhenMuted &&
                     (!g_micMuted || !g_settings.alwaysShow);
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
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        ShowExtras(true);
    }
}

void HideIconOverlay(HWND hwnd) {
    if (g_overlayVisible) {
        ShowWindow(hwnd, SW_HIDE);
        ShowExtras(false);
        g_overlayVisible = false;
    }
}

// True when the floating icon must not be on screen regardless of the
// auto-hide / always-show settings.
bool IconSuppressed() {
    if (!g_modEnabled) return true;
    if (g_settings.showOnlyWhenMuted && !g_micMuted) return true;
    if (g_settings.redBorderMode && g_micMuted) return true;  // border instead
    return false;
}

// Reconciles icon, red border and timer with the current state. Callers
// render first (RenderCurrentState) so a newly shown window never flashes
// stale content. `flash` = a toggle just happened: show the icon for the
// auto-hide delay (the original behaviour).
void ApplyVisibility(HWND hwnd, bool flash) {
    SyncBorder();
    if (IconSuppressed()) {
        HideIconOverlay(hwnd);
    } else if (flash || g_settings.alwaysShow || g_settings.showOnlyWhenMuted) {
        ShowOverlayTemporarily(hwnd);
    }
    EnsureTimerState(hwnd);
}

void RegisterGlobalHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, kHotkeyId);
    UnregisterHotKey(hwnd, kToggleHotkeyId);

    // Mute hotkey: released entirely while the mod is switched off, so the
    // combo is free for other apps.
    if (g_modEnabled) {
        UINT mods = g_settings.hotkeyModifiers;
        if (mods == 0 || g_settings.hotkeyVK == 0) {
            Wh_Log(
                L"Hotkey not registered: could not parse a valid hotkey from "
                L"'%s' (need at least one modifier and one key)",
                g_settings.hotkeyRaw);
        } else {
            mods |= MOD_NOREPEAT;
            if (!RegisterHotKey(hwnd, kHotkeyId, mods,
                                 g_settings.hotkeyVK)) {
                Wh_Log(
                    L"RegisterHotKey failed: %u (another app may already use "
                    L"this combo)",
                    GetLastError());
            }
        }
    }

    // Mod on/off hotkey: always registered (when configured), otherwise the
    // mod could never be switched back on.
    if (g_settings.toggleRaw[0] != L'\0') {
        if (g_settings.toggleModifiers == 0 || g_settings.toggleVK == 0) {
            Wh_Log(
                L"Mod on/off hotkey not registered: could not parse a valid "
                L"hotkey from '%s'",
                g_settings.toggleRaw);
        } else if (g_modEnabled &&
                   g_settings.toggleModifiers == g_settings.hotkeyModifiers &&
                   g_settings.toggleVK == g_settings.hotkeyVK) {
            Wh_Log(
                L"Mod on/off hotkey not registered: '%s' is the same combo "
                L"as the mute hotkey",
                g_settings.toggleRaw);
        } else if (!RegisterHotKey(hwnd, kToggleHotkeyId,
                                    g_settings.toggleModifiers | MOD_NOREPEAT,
                                    g_settings.toggleVK)) {
            Wh_Log(L"RegisterHotKey (mod on/off) failed: %u",
                   GetLastError());
        }
    }
}

// Turns the whole mod on/off from the toggle hotkey.
void SetModEnabled(HWND hwnd, bool enabled) {
    g_modEnabled = enabled;
    Wh_Log(L"Mod %s", enabled ? L"enabled" : L"disabled");

    RegisterGlobalHotkey(hwnd);  // (un)registers the mute hotkey
    RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
    ApplyVisibility(hwnd, enabled);  // show briefly when turned on
    MessageBeep(enabled ? MB_OK : MB_ICONEXCLAMATION);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY: {
            if (wParam == kToggleHotkeyId) {
                SetModEnabled(hwnd, !g_modEnabled);
            } else if (wParam == kHotkeyId && g_modEnabled) {
                ToggleMicMute();
                RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
                ApplyVisibility(hwnd, true);
            }
            return 0;
        }
        case kMsgMuteChanged: {
            bool muted = wParam != 0;
            if (muted == g_micMuted) {
                // Volume-only notification (e.g. slider moved in Sound
                // settings, or an app's AGC) — mute state didn't actually
                // change, so don't flash the overlay. Also covers the
                // duplicate OnNotify that fires right after our own
                // SetMute in ToggleMicMute, which already rendered.
                return 0;
            }
            g_micMuted = muted;
            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            ApplyVisibility(hwnd, true);
            return 0;
        }
        case kMsgDeviceChanged: {
            ResolveAudioEndpoint();
            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            ApplyVisibility(hwnd, false);
            return 0;
        }
        case kMsgSettingsChanged: {
            LoadSettings();
            RegisterGlobalHotkey(hwnd);

            LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            ex = g_settings.clickThrough ? (ex | WS_EX_TRANSPARENT)
                                          : (ex & ~WS_EX_TRANSPARENT);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex);

            g_manualPosition = false;
            ApplyOverlayPositionAndSize(hwnd);
            RebuildAuxWindows();
            RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());

            ApplyVisibility(hwnd, false);
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

                if (!g_settings.alwaysShow && !g_settings.showOnlyWhenMuted &&
                    g_overlayVisible && !g_dragging) {
                    bool activeSignal = g_settings.keepVisibleWhileActive &&
                                        !g_micMuted && peak > 0.03f;

                    // Refresh the tick while there is live signal
                    if (activeSignal) {
                        g_lastActivityTick = GetTickCount();
                    }

                    DWORD elapsed = GetTickCount() - g_lastActivityTick;
                    if (!activeSignal &&
                        elapsed > (DWORD)g_settings.overlayDurationMs) {
                        HideIconOverlay(hwnd);
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
        case WM_DISPLAYCHANGE:
            // Monitors added/removed/rearranged: rebuild the per-monitor
            // copies and border strips, then fall through to re-anchor.
            RebuildAuxWindows();
            [[fallthrough]];
        case WM_SETTINGCHANGE: {
            // Resolution, monitor arrangement, or work-area (taskbar
            // size/auto-hide) changes need to re-anchor the overlay even
            // when EnsureTimerState has the render timer off (alwaysShow
            // + muted), since nothing else would trigger a reposition.
            if (g_overlayVisible) {
                if (!g_manualPosition) ApplyOverlayPositionAndSize(hwnd);
                RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
            }
            return 0;
        }
        case WM_ENTERSIZEMOVE: {
            g_dragging = true;
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            // Fires only at the end of a real user-driven move loop
            // (started via the WM_NCHITTEST -> HTCAPTION drag below) —
            // unlike WM_MOVE, this can't be confused with the
            // programmatic repositioning that ApplyOverlayPositionAndSize
            // and UpdateLayeredWindow's pptDst also trigger.
            g_dragging = false;
            // Restart the auto-hide countdown from the drop, not from
            // whatever toggle/signal last touched it — otherwise a drag
            // that outlasts overlayDurationMs hides the overlay on the
            // very next timer tick after the user lets go.
            g_lastActivityTick = GetTickCount();
            if (!g_settings.clickThrough) {
                RECT wr;
                GetWindowRect(hwnd, &wr);
                g_manualPos = {wr.left, wr.top};
                g_manualPosition = true;
                // Rebuild only if the main overlay landed on another
                // monitor (changes the "other monitors" set / border).
                if (MonitorFromRect(&wr, MONITOR_DEFAULTTONEAREST) !=
                    g_auxMainMon) {
                    RebuildAuxWindows();
                }
                // Resume owning the position for subsequent frames now
                // that the move loop has released it.
                RenderCurrentState(hwnd, g_micMuted ? 0.0f : GetMicPeak());
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
    if (GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed; the overlay will render nothing");
        g_gdiplusToken = 0;
    }

    g_hOverlay = CreateOverlayWindow();
    if (g_hOverlay) {
        ResolveAudioEndpoint();
        RegisterGlobalHotkey(g_hOverlay);
        RebuildAuxWindows();
        RenderCurrentState(g_hOverlay, g_micMuted ? 0.0f : GetMicPeak());
        ApplyVisibility(g_hOverlay, false);
    }

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanupAudio();

    DestroyExtras();
    DestroyBorders();
    if (g_hOverlay) {
        UnregisterHotKey(g_hOverlay, kHotkeyId);
        UnregisterHotKey(g_hOverlay, kToggleHotkeyId);
        DestroyWindow(g_hOverlay);
        g_hOverlay = nullptr;
    }
    UnregisterClass(kClassName, GetCurrentModuleHandle());
    ReleaseAuxClasses();

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
    g_hThread = CreateThread(nullptr, 0, ModThreadProc, nullptr, 0,
                              &g_threadId);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_hOverlay) {
        // Loaded on the overlay thread itself (kMsgSettingsChanged handler)
        // rather than here on Windhawk's callback thread, so g_settings
        // only ever has one writer.
        PostMessage(g_hOverlay, kMsgSettingsChanged, 0, 0);
    } else {
        // Window not created yet — load directly so ModThreadProc picks up
        // fresh settings when it creates the window (no second writer once
        // the window exists, since that only happens before the thread's
        // message loop is reachable).
        LoadSettings();
    }
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
}

// clang-format off
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
// clang-format on