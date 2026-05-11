// ==WindhawkMod==
// @id              audioswap
// @name            AudioSwap
// @description     Tray icon to cycle between multiple preferred audio outputs. Supports up to 6 devices with click or scroll to swap.
// @version         1.3.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AudioSwap
Instantly cycle between multiple audio output devices from your system tray — no diving into Sound settings.

---

## How to Use

1. **Choose Device Count** — Open the **Settings** tab and pick how many devices to cycle through (2–6).
2. **Choose Interaction Mode** — Select **Click to Swap** (left-click the icon) or **Scroll to Swap** (mouse wheel over the icon).
3. **Choose Icons** — Pick an icon for each device slot. Icons for slots 3–6 are only used when the device count is set high enough.
4. **Assign Devices** — Right-click the tray icon. Use **Set as Device 1 / 2 / 3...** to assign each slot from the live device list.
5. **Swap** — Click or scroll the tray icon to cycle through your assigned devices.

### Scroll to Swap mode extras

- **Left-click** mutes the current device. Click again to unmute. The tray tooltip shows *(Muted)* and the icon gains a red dot while active.
- **Scrolling** to a different device automatically unmutes the previous one before switching.

> The tray tooltip always shows the active device. On first run it reads *"Right-click to configure"* until at least two slots are assigned.

---

## Known Bugs

- **Scroll to Swap may not respond over elevated windows.** such as **Task Manager**, **Windhawk** or other admin-elevated applications. Switch focus away from an elevated window and scrolling will work normally.

---

## Changelog

### v1.3.0
- Up to 6 devices; scroll-to-swap mode; dynamic right-click menu.
- Left-click mutes in scroll mode; red dot overlay on tray icon.
- Cycling auto-unmutes previous device.
- Inactive/disconnected devices skipped when cycling.
- Mute state persisted across mod restarts.

### v1.2.0
- **New:** Can now open WindHawk directly from the icon using right click
- **New:** Added new icons to select from
- **Improved:** added icons in the right click menu

### v1.1.0
- **New:** Right-click context menu — auto-detects all active audio outputs and lets you assign Device 1 and Device 2 directly from a live list. No more typing device names manually.
- **New:** Device selections persist across restarts.
- **Improved:** Toggle now matches devices by their unique system ID instead of a name substring search — works correctly regardless of how Windows names your device.
- **Improved:** Tray tooltip prompts you to configure on first run instead of showing "Unknown Device".
- **Removed:** Device name text fields from the Settings tab (replaced by the right-click menu).

### v1.0.0
- Initial release.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- deviceCount: 2
  $name: Number of Devices
  $description: How many audio devices to cycle through (2 to 6)
  $options:
    - 2: 2 Devices
    - 3: 3 Devices
    - 4: 4 Devices
    - 5: 5 Devices
    - 6: 6 Devices
- swapMode: click_to_swap
  $name: Interaction Mode
  $description: How to cycle through devices
  $options:
    - click_to_swap: Click to Swap
    - scroll_to_swap: Scroll to Swap
- icon1: speaker_normal
  $name: Device 1 Icon
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
- icon2: speaker_square
  $name: Device 2 Icon
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
- icon3: headphones
  $name: Device 3 Icon
  $description: Used when Number of Devices is 3 or more
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
- icon4: headset_gaming
  $name: Device 4 Icon
  $description: Used when Number of Devices is 4 or more
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
- icon5: headphones_modern
  $name: Device 5 Icon
  $description: Used when Number of Devices is 5 or more
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
- icon6: headset_modern
  $name: Device 6 Icon
  $description: Used when Number of Devices is 6
  $options:
    - speaker_normal: Normal Speaker
    - speaker_square: Square Speaker
    - speaker_modern: Modern Speaker
    - speaker_modern_square: Modern Square Speaker
    - audio_wave: Audio Wave
    - headphones: Headphones
    - headset_gaming: Gaming Headset
    - headphones_modern: Modern Headphones
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <endpointvolume.h>

#include <propkey.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <functiondiscoverykeys_devpkey.h>

#define TRAY_ICON_ID         1
#define WM_TRAY_CALLBACK     (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define WM_TRAY_SCROLL       (WM_USER + 3)  // wParam = direction (+1 or -1)
#define WM_UPDATE_HOOK_STATE (WM_USER + 4)

// Slot N uses menu IDs: N*100 + device_index (0..31). Max slot 6 → ID 631.
#define MENU_SLOT_BASE     100
#define MENU_MAX_DEVICES    32
#define MENU_OPEN_WINDHAWK 9000

#define MAX_DEVICE_SLOTS 6

const DWORD CLICK_DEBOUNCE_MS  = 500;
const DWORD SCROLL_DEBOUNCE_MS = 300;

// ─── Globals ──────────────────────────────────────────────────────────────────

static volatile LONG  g_isProcessingClick = 0;
static HANDLE         g_trayThread        = nullptr;
static HANDLE         g_workerThread      = nullptr;
static volatile HWND  g_trayHwnd          = nullptr;
static HINSTANCE      g_hInstance         = nullptr;
static WCHAR          g_windhawkPath[MAX_PATH] = {};
static WCHAR          g_ddoresDllPath[MAX_PATH] = {};  // full system32 path
static HICON          g_hWindHawkIcon     = nullptr;
static HBITMAP        g_hWindHawkBmp      = nullptr;
static DWORD          g_lastClickTime     = 0;
static DWORD          g_lastScrollTime    = 0;
static UINT           g_taskbarCreatedMsg = 0;

// Mouse hook
static HHOOK          g_mouseHook         = nullptr;
static RECT           g_trayIconRect      = {};

// Per-slot icon handles
static HICON          g_iconDev[MAX_DEVICE_SLOTS]     = {};
static HBITMAP        g_hIconDevBmp[MAX_DEVICE_SLOTS] = {};

// ── Shared state — protected by g_stateLock ──────────────────────────────────
// Read by the worker thread (CycleAudioDevice) and tray thread (UpdateTrayTip,
// BuildAndShowContextMenu). Written by the main thread (LoadDeviceSelections,
// LoadUserIconsAndSettings, SaveDeviceSelection) and tray thread (mute toggle).
static CRITICAL_SECTION g_stateLock;
static WCHAR  g_cachedDevId[MAX_DEVICE_SLOTS][512]  = {};
static WCHAR  g_cachedDevName[MAX_DEVICE_SLOTS][256] = {};
static int    g_deviceSlotCount = 2;
static bool   g_isMutedByUs     = false;
static WCHAR  g_mutedDeviceId[512] = {};
// ─────────────────────────────────────────────────────────────────────────────

// Mode flag: set/read atomically with InterlockedExchange/InterlockedRead.
static volatile LONG  g_scrollToSwap = 0;  // 1 = scroll mode

// IMMNotificationClient registration
class AudioDeviceNotifier;
static IMMDeviceEnumerator* g_notifEnum      = nullptr;
static AudioDeviceNotifier* g_deviceNotifier = nullptr;

const CLSID CLSID_CPolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}
};
const IID IID_IPolicyConfig_Win10_11 = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}
};

MIDL_INTERFACE("f8679f50-850a-41cf-9c72-430f290290c8")
IPolicyConfig : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT, PINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR wszDeviceId, ERole eRole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

int GetIconIndex(PCWSTR s) {
    if (s) {
        if (wcscmp(s, L"speaker_normal") == 0)        return 1;
        if (wcscmp(s, L"speaker_square") == 0)        return 4;
        if (wcscmp(s, L"speaker_modern") == 0)        return 90;
        if (wcscmp(s, L"speaker_modern_square") == 0) return 93;
        if (wcscmp(s, L"audio_wave") == 0)            return 94;
        if (wcscmp(s, L"headphones") == 0)            return 2;
        if (wcscmp(s, L"headset_gaming") == 0)        return 8;
        if (wcscmp(s, L"headphones_modern") == 0)     return 91;
        if (wcscmp(s, L"headset_modern") == 0)        return 95;
        if (wcscmp(s, L"earphones") == 0)             return 6;
    }
    return 4;
}

// ─── Device selection data ────────────────────────────────────────────────────
// All functions below acquire g_stateLock for writes to shared slot data.

void LoadDeviceSelections() {
    // Read from Windhawk storage outside the lock (Wh_GetStringValue is thread-safe).
    WCHAR tmpIds[MAX_DEVICE_SLOTS][512]   = {};
    WCHAR tmpNames[MAX_DEVICE_SLOTS][256] = {};

    int count;
    EnterCriticalSection(&g_stateLock);
    count = g_deviceSlotCount;
    LeaveCriticalSection(&g_stateLock);

    WCHAR keyId[32], keyName[32];
    for (int i = 0; i < count; i++) {
        swprintf_s(keyId,   L"Device%dId",   i + 1);
        swprintf_s(keyName, L"Device%dName", i + 1);
        Wh_GetStringValue(keyId,   tmpIds[i],   512);
        Wh_GetStringValue(keyName, tmpNames[i], 256);
    }

    EnterCriticalSection(&g_stateLock);
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
        lstrcpynW(g_cachedDevId[i],   tmpIds[i],   512);
        lstrcpynW(g_cachedDevName[i], tmpNames[i], 256);
    }
    LeaveCriticalSection(&g_stateLock);
}

void SaveDeviceSelection(int slot, PCWSTR deviceId, PCWSTR friendlyName) {
    if (slot < 1 || slot > MAX_DEVICE_SLOTS) return;
    int idx = slot - 1;
    WCHAR keyId[32], keyName[32];
    swprintf_s(keyId,   L"Device%dId",   slot);
    swprintf_s(keyName, L"Device%dName", slot);
    Wh_SetStringValue(keyId,   deviceId);
    Wh_SetStringValue(keyName, friendlyName);

    EnterCriticalSection(&g_stateLock);
    lstrcpynW(g_cachedDevId[idx],   deviceId,     512);
    lstrcpynW(g_cachedDevName[idx], friendlyName, 256);
    LeaveCriticalSection(&g_stateLock);
}

static int ReadDeviceCountSetting() {
    PCWSTR s = Wh_GetStringSetting(L"deviceCount");
    int n = 2;
    if (s && s[0] >= L'2' && s[0] <= L'9') {
        n = 0;
        for (const WCHAR* p = s; *p >= L'0' && *p <= L'9'; p++)
            n = n * 10 + (*p - L'0');
    }
    if (s) Wh_FreeStringSetting(s);
    if (n < 2) n = 2;
    if (n > MAX_DEVICE_SLOTS) n = MAX_DEVICE_SLOTS;
    return n;
}

void LoadUserIconsAndSettings() {
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
        if (g_iconDev[i])    { DestroyIcon(g_iconDev[i]);      g_iconDev[i]     = nullptr; }
        if (g_hIconDevBmp[i]){ DeleteObject(g_hIconDevBmp[i]); g_hIconDevBmp[i] = nullptr; }
    }

    int newCount = ReadDeviceCountSetting();

    PCWSTR swapMode = Wh_GetStringSetting(L"swapMode");
    LONG   newScroll = (swapMode && wcscmp(swapMode, L"scroll_to_swap") == 0) ? 1 : 0;
    if (swapMode) Wh_FreeStringSetting(swapMode);

    EnterCriticalSection(&g_stateLock);
    g_deviceSlotCount = newCount;
    LeaveCriticalSection(&g_stateLock);

    InterlockedExchange(&g_scrollToSwap, newScroll);

    WCHAR iconKey[16];
    for (int i = 0; i < newCount; i++) {
        swprintf_s(iconKey, L"icon%d", i + 1);
        PCWSTR s = Wh_GetStringSetting(iconKey);
        ExtractIconExW(g_ddoresDllPath, GetIconIndex(s), nullptr, &g_iconDev[i], 1);
        if (s) Wh_FreeStringSetting(s);

        if (g_iconDev[i]) {
            ICONINFO ii = {};
            if (GetIconInfo(g_iconDev[i], &ii)) {
                g_hIconDevBmp[i] = ii.hbmColor;
                if (!g_hIconDevBmp[i]) {
                    g_hIconDevBmp[i] = ii.hbmMask;
                } else if (ii.hbmMask) {
                    DeleteObject(ii.hbmMask);
                }
            }
        }
    }
}


// ─── Tray icon rect ────────────────────────────────────────────────────────────

static void RefreshTrayIconRect() {
    HWND hwnd = g_trayHwnd;
    if (!hwnd) return;
    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.hWnd = hwnd;
    nii.uID  = TRAY_ICON_ID;
    Shell_NotifyIconGetRect(&nii, &g_trayIconRect);
}

// ─── Mute helpers ─────────────────────────────────────────────────────────────
// Every function requires COM already initialized on the calling thread.

static BOOL ApplyMute(PCWSTR deviceId, BOOL mute) {
    IMMDeviceEnumerator* pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnum)))
        return FALSE;

    IMMDevice* pDevice = nullptr;
    HRESULT hr = (deviceId && deviceId[0])
                 ? pEnum->GetDevice(deviceId, &pDevice)
                 : pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    pEnum->Release();
    if (FAILED(hr) || !pDevice) return FALSE;

    IAudioEndpointVolume* pVol = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&pVol);
    pDevice->Release();
    if (FAILED(hr) || !pVol) return FALSE;

    pVol->SetMute(mute, nullptr);
    pVol->Release();
    return TRUE;
}


// Call from tray thread (COM already initialized) or worker thread (has its own COM).
static void RestoreMute() {
    EnterCriticalSection(&g_stateLock);
    if (!g_isMutedByUs) { LeaveCriticalSection(&g_stateLock); return; }
    WCHAR localId[512];
    lstrcpynW(localId, g_mutedDeviceId, 512);
    g_isMutedByUs      = false;
    g_mutedDeviceId[0] = L'\0';
    LeaveCriticalSection(&g_stateLock);
    ApplyMute(localId, FALSE);
    Wh_SetStringValue(L"MutedDeviceId", L"");
}

// For callers that lack a COM apartment (main thread callbacks).
static void RestoreMuteExternal() {
    EnterCriticalSection(&g_stateLock);
    bool wasMuted = g_isMutedByUs;
    WCHAR localId[512] = {};
    if (wasMuted) {
        lstrcpynW(localId, g_mutedDeviceId, 512);
        g_isMutedByUs      = false;
        g_mutedDeviceId[0] = L'\0';
    }
    LeaveCriticalSection(&g_stateLock);
    if (wasMuted) {
        CoInitialize(nullptr);
        ApplyMute(localId, FALSE);
        CoUninitialize();
        Wh_SetStringValue(L"MutedDeviceId", L"");
    }
}

// Called from TrayWndProc — tray thread has COM initialized by TrayThreadProc.
static void ToggleMuteCurrentDevice() {
    EnterCriticalSection(&g_stateLock);
    bool already = g_isMutedByUs;
    WCHAR localId[512] = {};
    if (already) {
        lstrcpynW(localId, g_mutedDeviceId, 512);
        g_isMutedByUs      = false;
        g_mutedDeviceId[0] = L'\0';
    }
    LeaveCriticalSection(&g_stateLock);

    if (already) {
        ApplyMute(localId, FALSE);
        Wh_SetStringValue(L"MutedDeviceId", L"");
        return;
    }

    // Mute the current default device
    IMMDeviceEnumerator* pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnum)))
        return;

    IMMDevice* pDefault = nullptr;
    if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefault))) {
        LPWSTR pId = nullptr;
        if (SUCCEEDED(pDefault->GetId(&pId))) {
            if (ApplyMute(pId, TRUE)) {
                EnterCriticalSection(&g_stateLock);
                lstrcpynW(g_mutedDeviceId, pId, 512);
                g_isMutedByUs = true;
                LeaveCriticalSection(&g_stateLock);
                Wh_SetStringValue(L"MutedDeviceId", pId);
            }
            CoTaskMemFree(pId);
        }
        pDefault->Release();
    }
    pEnum->Release();
}

// ─── Device-state check ───────────────────────────────────────────────────────

static bool IsDeviceActive(IMMDeviceEnumerator* pEnum, PCWSTR deviceId) {
    if (!deviceId || !deviceId[0]) return false;
    IMMDevice* pDevice = nullptr;
    if (FAILED(pEnum->GetDevice(deviceId, &pDevice))) return false;
    DWORD state = 0;
    bool active = SUCCEEDED(pDevice->GetState(&state)) && (state == DEVICE_STATE_ACTIVE);
    pDevice->Release();
    return active;
}

// ─── IMMNotificationClient ────────────────────────────────────────────────────

class AudioDeviceNotifier : public IMMNotificationClient {
    volatile LONG m_ref;
public:
    AudioDeviceNotifier() : m_ref(1) {}
    virtual ~AudioDeviceNotifier() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef(); return S_OK;
        }
        *ppv = nullptr; return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_ref));
    }
    ULONG STDMETHODCALLTYPE Release() override {
        LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) delete this;
        return static_cast<ULONG>(r);
    }

    // Callbacks are fired on an OS thread — only PostMessageW is safe here.
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
        EDataFlow flow, ERole role, LPCWSTR) override
    {
        if (flow == eRender && role == eMultimedia) {
            HWND hwnd = g_trayHwnd;
            if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        HWND hwnd = g_trayHwnd;
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        HWND hwnd = g_trayHwnd;
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        HWND hwnd = g_trayHwnd;
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
        LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};

// ─── Mouse hook ───────────────────────────────────────────────────────────────

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL &&
        InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 1)
    {
        HWND hwnd = g_trayHwnd;
        if (hwnd) {
            const MSLLHOOKSTRUCT* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
            if (PtInRect(&g_trayIconRect, ms->pt)) {
                short delta     = static_cast<short>(HIWORD(ms->mouseData));
                int   direction = (delta > 0) ? 1 : -1;
                PostMessageW(hwnd, WM_TRAY_SCROLL,
                             static_cast<WPARAM>(static_cast<LONG_PTR>(direction)), 0);
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// ─── Muted icon overlay ───────────────────────────────────────────────────────

static HICON CreateMutedOverlayIcon(HICON hBase) {
    if (!hBase) return nullptr;

    const int SZ = 32;

    HDC hScreen = GetDC(nullptr);
    if (!hScreen) return nullptr;

    // 32bpp DIBSECTION so we can fix the alpha channel post-GDI.
    // GDI's Ellipse sets RGB but leaves alpha = 0; CreateIconIndirect
    // uses the alpha channel for 32bpp color bitmaps, making the dot
    // invisible without this fixup.
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = SZ;
    bmi.bmiHeader.biHeight      = -SZ;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hColor = CreateDIBSection(hScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HDC hColorDC = CreateCompatibleDC(hScreen);
    HBITMAP hOldColor = (HBITMAP)SelectObject(hColorDC, hColor);
    memset(pBits, 0, SZ * SZ * 4);

    HDC     hMaskDC   = CreateCompatibleDC(hScreen);
    HBITMAP hMask     = CreateBitmap(SZ, SZ, 1, 1, nullptr);
    HBITMAP hOldMask  = (HBITMAP)SelectObject(hMaskDC, hMask);

    PatBlt(hMaskDC, 0, 0, SZ, SZ, WHITENESS);
    DrawIconEx(hColorDC, 0, 0, hBase, SZ, SZ, 0, nullptr, DI_NORMAL);
    DrawIconEx(hMaskDC,  0, 0, hBase, SZ, SZ, 0, nullptr, DI_MASK);

    const int D  = 20;
    const int MG = 1;
    const int X  = SZ - D - MG;
    const int Y  = SZ - D - MG;

    HPEN hNoPen = (HPEN)GetStockObject(NULL_PEN);
    SelectObject(hColorDC, hNoPen);
    SelectObject(hMaskDC,  hNoPen);

    HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hColorDC, hWhite);
    Ellipse(hColorDC, X - 1, Y - 1, X + D + 2, Y + D + 2);
    SelectObject(hMaskDC, GetStockObject(WHITE_BRUSH));
    Ellipse(hMaskDC,  X - 1, Y - 1, X + D + 2, Y + D + 2);
    DeleteObject(hWhite);

    HBRUSH hRed = CreateSolidBrush(RGB(220, 30, 30));
    SelectObject(hColorDC, hRed);
    Ellipse(hColorDC, X, Y, X + D, Y + D);
    DeleteObject(hRed);

    // GDI drew RGB pixels with alpha = 0 — stamp alpha = 255 over the
    // overlay dot region so it is actually visible on 32bpp icons.
    {
        int y0 = Y - 2; if (y0 < 0) y0 = 0;
        int y1 = Y + D + 3; if (y1 > SZ) y1 = SZ;
        int x0 = X - 2; if (x0 < 0) x0 = 0;
        int x1 = X + D + 3; if (x1 > SZ) x1 = SZ;
        DWORD* pixels = (DWORD*)pBits;
        for (int y = y0; y < y1; y++)
            for (int x = x0; x < x1; x++) {
                DWORD pixel = pixels[y * SZ + x];
                if (pixel & 0x00FFFFFF)
                    pixels[y * SZ + x] = pixel | 0xFF000000;
            }
    }

    SelectObject(hColorDC, hOldColor);
    SelectObject(hMaskDC,  hOldMask);

    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask  = hMask;
    HICON hResult = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    DeleteObject(hMask);
    DeleteDC(hColorDC);
    DeleteDC(hMaskDC);
    ReleaseDC(nullptr, hScreen);
    return hResult;
}

// ─── Tray tip ─────────────────────────────────────────────────────────────────

void UpdateTrayTip(HWND hWnd, BOOL isAdd) {
    // Snapshot shared state under lock (no COM inside the lock).
    WCHAR localDevIds[MAX_DEVICE_SLOTS][512] = {};
    int   localSlotCount;
    bool  localMuted;

    EnterCriticalSection(&g_stateLock);
    localSlotCount = g_deviceSlotCount;
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++)
        lstrcpynW(localDevIds[i], g_cachedDevId[i], 512);
    localMuted = g_isMutedByUs;
    LeaveCriticalSection(&g_stateLock);

    WCHAR currentDev[256] = L"Unknown Device";
    WCHAR currentId[512]  = {};

    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice* pDev = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDev))) {
            LPWSTR pId = nullptr;
            if (SUCCEEDED(pDev->GetId(&pId))) {
                lstrcpynW(currentId, pId, 512);
                CoTaskMemFree(pId);
            }
            IPropertyStore* pStore = nullptr;
            if (SUCCEEDED(pDev->OpenPropertyStore(STGM_READ, &pStore))) {
                PROPVARIANT v; PropVariantInit(&v);
                if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &v)) && v.pwszVal)
                    lstrcpynW(currentDev, v.pwszVal, 256);
                PropVariantClear(&v);
                pStore->Release();
            }
            pDev->Release();
        }
        pEnum->Release();
    }

    HICON currentIcon = g_iconDev[0];
    for (int i = 0; i < localSlotCount; i++) {
        if (localDevIds[i][0] != L'\0' && wcscmp(currentId, localDevIds[i]) == 0) {
            currentIcon = g_iconDev[i];
            break;
        }
    }

    // Count configured slots from local snapshot
    int configuredCount = 0;
    for (int i = 0; i < localSlotCount; i++)
        if (localDevIds[i][0] != L'\0') configuredCount++;

    static WCHAR s_lastDev[256] = {};
    static HICON s_lastIcon     = nullptr;
    static bool  s_lastMuted    = false;
    if (!isAdd &&
        wcscmp(currentDev, s_lastDev) == 0 &&
        currentIcon == s_lastIcon &&
        s_lastMuted == localMuted)
    {
        RefreshTrayIconRect();
        return;
    }
    lstrcpyW(s_lastDev, currentDev);
    s_lastIcon  = currentIcon;
    s_lastMuted = localMuted;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd             = hWnd;
    nid.uID              = TRAY_ICON_ID;
    nid.uFlags           = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    if (configuredCount < 2)
        swprintf_s(nid.szTip, L"AudioSwap: Right-click to configure");
    else if (localMuted)
        swprintf_s(nid.szTip, L"Audio: %s (Muted)", currentDev);
    else
        swprintf_s(nid.szTip, L"Audio: %s", currentDev);

    HICON hOverlay = localMuted ? CreateMutedOverlayIcon(currentIcon) : nullptr;
    nid.hIcon = hOverlay ? hOverlay : currentIcon;
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &nid);
    if (hOverlay) { DestroyIcon(hOverlay); }

    if (isAdd) {
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd     = hWnd;
        nidVer.uID      = TRAY_ICON_ID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    }

    RefreshTrayIconRect();
}

// ─── Audio cycling ────────────────────────────────────────────────────────────

BOOL CycleAudioDevice(int direction) {
    // Snapshot shared device data under lock before any COM calls.
    WCHAR localIds[MAX_DEVICE_SLOTS][512] = {};
    int   localCount;

    EnterCriticalSection(&g_stateLock);
    localCount = g_deviceSlotCount;
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++)
        lstrcpynW(localIds[i], g_cachedDevId[i], 512);
    LeaveCriticalSection(&g_stateLock);

    int configuredCount = 0;
    for (int i = 0; i < localCount; i++)
        if (localIds[i][0] != L'\0') configuredCount++;
    if (configuredCount < 2) return FALSE;

    HRESULT comHr = CoInitialize(nullptr);
    bool comOk = SUCCEEDED(comHr) || comHr == S_FALSE;
    if (!comOk) return FALSE;

    // Undo click-mute before switching (COM is now available).
    RestoreMute();
    HWND hwnd = g_trayHwnd;
    if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);

    IMMDeviceEnumerator* pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        CoUninitialize(); return FALSE;
    }

    WCHAR currentId[512] = {};
    IMMDevice* pDefault = nullptr;
    if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefault))) {
        LPWSTR pId = nullptr;
        if (SUCCEEDED(pDefault->GetId(&pId))) {
            lstrcpynW(currentId, pId, 512);
            CoTaskMemFree(pId);
        }
        pDefault->Release();
    }

    int currentSlot = -1;
    for (int i = 0; i < localCount; i++) {
        if (localIds[i][0] != L'\0' && wcscmp(currentId, localIds[i]) == 0) {
            currentSlot = i; break;
        }
    }

    int startSlot = (currentSlot >= 0) ? currentSlot : 0;
    int validSlot = -1;

    for (int tries = 1; tries <= localCount; tries++) {
        int candidate = ((startSlot + tries * direction) % localCount + localCount) % localCount;
        if (IsDeviceActive(pEnum, localIds[candidate])) {
            validSlot = candidate; break;
        }
    }

    if (validSlot == -1) {
        pEnum->Release(); CoUninitialize(); return FALSE;
    }

    IPolicyConfig* pPolicyConfig = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL,
                                   IID_IPolicyConfig_Win10_11, (void**)&pPolicyConfig))) {
        pPolicyConfig->SetDefaultEndpoint(localIds[validSlot], eConsole);
        pPolicyConfig->SetDefaultEndpoint(localIds[validSlot], eMultimedia);
        pPolicyConfig->SetDefaultEndpoint(localIds[validSlot], eCommunications);
        pPolicyConfig->Release();
    }

    pEnum->Release();
    CoUninitialize();
    return TRUE;
}

// ─── Context menu ─────────────────────────────────────────────────────────────

struct AudioDevice { WCHAR id[512]; WCHAR name[256]; };

void BuildAndShowContextMenu(HWND hWnd) {
    // Snapshot shared data under lock.
    WCHAR localIds[MAX_DEVICE_SLOTS][512] = {};
    int   localCount;
    bool  localMuted;

    EnterCriticalSection(&g_stateLock);
    localCount = g_deviceSlotCount;
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++)
        lstrcpynW(localIds[i], g_cachedDevId[i], 512);
    localMuted = g_isMutedByUs;
    LeaveCriticalSection(&g_stateLock);

    AudioDevice devices[MENU_MAX_DEVICES];
    int deviceCount = 0;

    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDeviceCollection* pDevices = nullptr;
        if (SUCCEEDED(pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices))) {
            UINT count = 0;
            pDevices->GetCount(&count);
            if (count > MENU_MAX_DEVICES) count = MENU_MAX_DEVICES;
            for (UINT i = 0; i < count; i++) {
                IMMDevice* pDevice = nullptr;
                if (FAILED(pDevices->Item(i, &pDevice))) continue;
                LPWSTR pId = nullptr;
                if (SUCCEEDED(pDevice->GetId(&pId))) {
                    lstrcpynW(devices[deviceCount].id, pId, 512);
                    CoTaskMemFree(pId);
                    IPropertyStore* pStore = nullptr;
                    if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pStore))) {
                        PROPVARIANT v; PropVariantInit(&v);
                        if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &v)) && v.pwszVal) {
                            lstrcpynW(devices[deviceCount].name, v.pwszVal, 256);
                            deviceCount++;
                        }
                        PropVariantClear(&v);
                        pStore->Release();
                    }
                }
                pDevice->Release();
            }
            pDevices->Release();
        }
        pEnum->Release();
    }

    HMENU hMenu = CreatePopupMenu();

    for (int slot = 0; slot < localCount; slot++) {
        HMENU hSub = CreatePopupMenu();
        for (int i = 0; i < deviceCount; i++) {
            UINT flags = MF_STRING | (wcscmp(devices[i].id, localIds[slot]) == 0 ? MF_CHECKED : 0);
            AppendMenuW(hSub, flags, MENU_SLOT_BASE * (slot + 1) + i, devices[i].name);
        }
        WCHAR label[32];
        swprintf_s(label, L"Set as Device %d", slot + 1);
        MENUITEMINFOW mii = {sizeof(mii)};
        mii.fMask      = MIIM_SUBMENU | MIIM_STRING | MIIM_BITMAP;
        mii.hSubMenu   = hSub;
        mii.dwTypeData = label;
        mii.hbmpItem   = g_hIconDevBmp[slot];
        InsertMenuItemW(hMenu, (UINT)-1, TRUE, &mii);
    }

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open WindHawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    WCHAR statusText[300];
    int configuredCount = 0;
    for (int i = 0; i < localCount; i++) if (localIds[i][0] != L'\0') configuredCount++;
    if (configuredCount < 2) {
        lstrcpyW(statusText, L"Right-click to configure devices");
    } else {
        WCHAR activeName[256] = L"Unknown";
        IMMDeviceEnumerator* pEnum2 = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                       __uuidof(IMMDeviceEnumerator), (void**)&pEnum2))) {
            IMMDevice* pDef = nullptr;
            if (SUCCEEDED(pEnum2->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDef))) {
                IPropertyStore* pStore = nullptr;
                if (SUCCEEDED(pDef->OpenPropertyStore(STGM_READ, &pStore))) {
                    PROPVARIANT v; PropVariantInit(&v);
                    if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &v)) && v.pwszVal)
                        lstrcpynW(activeName, v.pwszVal, 256);
                    PropVariantClear(&v);
                    pStore->Release();
                }
                pDef->Release();
            }
            pEnum2->Release();
        }
        if (localMuted)
            swprintf_s(statusText, L"Active: %s (Muted)", activeName);
        else
            swprintf_s(statusText, L"Active: %s", activeName);
    }
    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hWnd);
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                             pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    if (cmd > 0 && cmd != MENU_OPEN_WINDHAWK) {
        int slot      = cmd / MENU_SLOT_BASE;
        int deviceIdx = cmd % MENU_SLOT_BASE;
        if (slot >= 1 && slot <= localCount && deviceIdx < deviceCount) {
            SaveDeviceSelection(slot, devices[deviceIdx].id, devices[deviceIdx].name);
            PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
        }
    } else if (cmd == MENU_OPEN_WINDHAWK) {
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpFile = g_windhawkPath;
        sei.nShow  = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    }
}

// ─── Worker thread ────────────────────────────────────────────────────────────

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    int direction = static_cast<int>(reinterpret_cast<LONG_PTR>(lpParam));
    if (CycleAudioDevice(direction)) {
        HWND hwnd = g_trayHwnd;
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
    }
    InterlockedExchange(&g_isProcessingClick, 0);
    return 0;
}

static void SpawnCycleThread(int direction) {
    if (g_workerThread) { CloseHandle(g_workerThread); g_workerThread = nullptr; }
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc,
                                  reinterpret_cast<LPVOID>(static_cast<LONG_PTR>(direction)),
                                  0, nullptr);
}

// ─── Tray window ──────────────────────────────────────────────────────────────

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CALLBACK) {
        UINT event = LOWORD(lParam);  // NOTIFYICON_VERSION_4: event in LOWORD(lParam)

        if (event == WM_RBUTTONUP) {
            BuildAndShowContextMenu(hWnd);

        } else if (event == WM_LBUTTONUP &&
                   InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 0) {
            // Click mode: debounced forward cycle
            if (InterlockedExchange(&g_isProcessingClick, 1) == 0) {
                DWORD now = GetTickCount();
                if (now - g_lastClickTime > CLICK_DEBOUNCE_MS) {
                    g_lastClickTime = now;
                    SpawnCycleThread(1);
                } else {
                    InterlockedExchange(&g_isProcessingClick, 0);
                }
            }

        } else if (event == WM_LBUTTONUP &&
                   InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 1) {
            // Scroll mode: left-click toggles mute (COM initialized on tray thread)
            ToggleMuteCurrentDevice();
            PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
        }

    } else if (msg == WM_TRAY_SCROLL) {
        // Posted by LowLevelMouseProc when the user scrolls over the icon
        DWORD now = GetTickCount();
        if (now - g_lastScrollTime > SCROLL_DEBOUNCE_MS &&
            InterlockedExchange(&g_isProcessingClick, 1) == 0) {
            g_lastScrollTime = now;
            int direction = static_cast<int>(static_cast<LONG_PTR>(wParam));
            SpawnCycleThread(direction);
        }

    } else if (msg == WM_UPDATE_TRAY_STATE) {
        UpdateTrayTip(hWnd, FALSE);

    } else if (msg == WM_UPDATE_HOOK_STATE) {
        LONG isScroll = InterlockedCompareExchange(&g_scrollToSwap, 0, 0);
        if (isScroll && !g_mouseHook) {
            g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hInstance, 0);
        } else if (!isScroll && g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
        }

    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        UpdateTrayTip(hWnd, TRUE);  // re-add icon after explorer restart

    } else if (msg == WM_CLOSE) {
        // Orderly shutdown: remove tray icon, then destroy window.
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = hWnd;
        nid.uID  = TRAY_ICON_ID;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DestroyWindow(hWnd);
        return 0;

    } else if (msg == WM_DESTROY) {
        g_trayHwnd = nullptr;  // prevent IMMNotificationClient callbacks from posting
        PostQuitMessage(0);
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Tray thread ──────────────────────────────────────────────────────────────

DWORD WINAPI TrayThreadProc(LPVOID) {
    CoInitialize(nullptr);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = TrayWndProc;
    wc.hInstance     = g_hInstance;
    wc.lpszClassName = L"AudioSwitcherWindowClass";
    RegisterClassW(&wc);

    // WS_EX_TOOLWINDOW: hidden popup window that Shell_NotifyIconW can track properly.
    // AudioSwap used HWND_MESSAGE originally, but message-only windows can lose their
    // tray icon after reboot because the shell cannot reliably associate the icon with
    // the window across session boundaries.
    g_trayHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"Audio Switcher",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hInstance, nullptr
    );
    if (!g_trayHwnd) {
        CoUninitialize();
        return 1;
    }

    // Set a unique AUMID so the OS doesn't group this with the main Windhawk icon.
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt      = VT_LPWSTR;
        var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (var.pwszVal) {
            lstrcpyW(var.pwszVal, L"BlackPaw.AudioSwitcher");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit();
        pps->Release();
    }

    // Register for instant device-change notifications.
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&g_notifEnum))) {
        g_deviceNotifier = new AudioDeviceNotifier();
        g_notifEnum->RegisterEndpointNotificationCallback(g_deviceNotifier);
    }

    // Global mouse hook for scroll-to-swap (only installed in scroll mode).
    if (InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 1) {
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hInstance, 0);
    }

    UpdateTrayTip(g_trayHwnd, TRUE);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { DispatchMessageW(&msg); }

    // Unregister notifications before releasing the enumerator.
    if (g_notifEnum && g_deviceNotifier) {
        g_notifEnum->UnregisterEndpointNotificationCallback(g_deviceNotifier);
        g_deviceNotifier->Release(); g_deviceNotifier = nullptr;
        g_notifEnum->Release();      g_notifEnum      = nullptr;
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    CoUninitialize();
    return 0;
}

// ─── Mod lifecycle ────────────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"AudioSwap Mod Init");
    InitializeCriticalSection(&g_stateLock);

    g_hInstance = GetModuleHandleW(nullptr);
    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    // Build the full system32 path for ddores.dll.
    UINT sysLen = GetSystemDirectoryW(g_ddoresDllPath, MAX_PATH);
    if (sysLen > 0 && sysLen < MAX_PATH - 12)
        lstrcatW(g_ddoresDllPath, L"\\ddores.dll");
    else
        lstrcpyW(g_ddoresDllPath, L"ddores.dll");  // safe fallback

    // Load the Windhawk menu icon.
    int whIconIndices[] = {98, 94, 95, 6};
    for (int idx : whIconIndices) {
        ExtractIconExW(g_ddoresDllPath, idx, nullptr, &g_hWindHawkIcon, 1);
        if (g_hWindHawkIcon) break;
    }
    if (g_hWindHawkIcon) {
        ICONINFO ii = {};
        if (GetIconInfo(g_hWindHawkIcon, &ii)) {
            g_hWindHawkBmp = ii.hbmColor;
            if (!g_hWindHawkBmp) {
                g_hWindHawkBmp = ii.hbmMask;
            } else if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
        }
    }

    // If a previous run muted a device and was killed before unmuting, clean it up.
    WCHAR savedMutedId[512] = {};
    Wh_GetStringValue(L"MutedDeviceId", savedMutedId, 512);
    if (savedMutedId[0] != L'\0') {
        CoInitialize(nullptr);
        ApplyMute(savedMutedId, FALSE);
        CoUninitialize();
        Wh_SetStringValue(L"MutedDeviceId", L"");
    }

    LoadUserIconsAndSettings();   // sets g_deviceSlotCount first
    LoadDeviceSelections();
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    RestoreMuteExternal();         // undo mute before mode may change
    LoadUserIconsAndSettings();
    LoadDeviceSelections();
    HWND hwnd = g_trayHwnd;
    if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
    HWND hwndHook = g_trayHwnd;
    if (hwndHook) PostMessageW(hwndHook, WM_UPDATE_HOOK_STATE, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"AudioSwap Mod Uninit");
    RestoreMuteExternal();

    // Send WM_CLOSE so TrayWndProc can delete the tray icon
    // cleanly before DestroyWindow → PostQuitMessage exits the message loop.
    HWND hwnd = g_trayHwnd;
    if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    if (g_trayThread) {
        WaitForSingleObject(g_trayThread, 3000);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_notifEnum && g_deviceNotifier) {
        g_notifEnum->UnregisterEndpointNotificationCallback(g_deviceNotifier);
        g_deviceNotifier->Release(); g_deviceNotifier = nullptr;
        g_notifEnum->Release();      g_notifEnum      = nullptr;
    }
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
        if (g_iconDev[i])    { DestroyIcon(g_iconDev[i]);      g_iconDev[i]     = nullptr; }
        if (g_hIconDevBmp[i]){ DeleteObject(g_hIconDevBmp[i]); g_hIconDevBmp[i] = nullptr; }
    }
    if (g_hWindHawkIcon){ DestroyIcon(g_hWindHawkIcon);  g_hWindHawkIcon = nullptr; }
    if (g_hWindHawkBmp) { DeleteObject(g_hWindHawkBmp);  g_hWindHawkBmp  = nullptr; }

    DeleteCriticalSection(&g_stateLock);
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
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
