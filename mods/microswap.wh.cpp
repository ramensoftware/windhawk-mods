// ==WindhawkMod==
// @id              microswap
// @name            MicSwitch
// @description     Tray icon to cycle between multiple preferred microphones. Supports up to 6 devices with click or scroll to swap.
// @version         2.0.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32 -lcomdlg32 -ladvapi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MicSwitch
Instantly cycle between multiple microphones from your system tray — no diving into Sound settings.

> Works great alongside **[AudioSwap](https://windhawk.net/mods/audioswap)** — the companion mod that brings the same tray experience to audio output control.

---

## How to Use

1. **Open Settings** — Right-click the tray icon and select **Mod Settings** to open the configuration dashboard.
2. **Assign Devices** — Use the dropdown in each slot to pick a device from the live list of active microphones.
3. **Choose Icons** — Select a preset icon per slot, or click **Browse...** to load a custom `.ico` file.
4. **Choose Mode** — Pick **Click to Swap** (left-click cycles mics) or **Scroll to Swap** (mouse wheel cycles; left-click mutes).
5. **Set Device Count** — Choose how many slots to cycle through (2–6).
6. Click **Save and Apply** — the tray icon updates immediately, no restart needed.

### Volume Control

- **Scroll wheel** over the tray icon (in Click to Swap mode) adjusts microphone volume — just like the native Windows sound icon.
- **Middle-click** the tray icon to open a compact volume slider. Drag or use the arrow keys to adjust microphone volume live — click anywhere outside or press Escape to close.

### Scroll to Swap mode extras

- **Left-click** mutes the current microphone. Click again to unmute. The tray tooltip shows *(Muted)* and the icon gains a red dot while active.
- **Scrolling** to a different microphone automatically unmutes the previous one before switching.

### Device Priority (Advanced Mode)

Enable **Advanced Mode** in the Windhawk settings panel (gear icon → Settings tab) to show the Device Priority section inside Mod Settings.

- Rank up to 6 preferred microphones in order of priority.
- When MicSwitch loads, the highest-priority microphone that is currently connected is automatically placed into a swap slot.
- Disconnected priority devices are remembered and restored to their slot when reconnected.
- Each priority entry has its own icon picker.

> The tray tooltip always shows the active microphone. On first run it reads *"Right-click to configure"* until at least two slots are assigned.

---

## Known Bugs

- **Scroll to Swap may not respond over elevated windows** such as Task Manager, Windhawk, or other admin-elevated applications. Switch focus away from the elevated window and scrolling will work normally.

---

## Changelog

### v2.0.0
- **Complete rebuild.** Full feature parity with AudioSwap: 6 device slots, configurable cycle order, native dark-themed settings dashboard, scroll-to-swap mode, push-to-mute, instant device-change updates via the MMDevice notification API, and Advanced Mode with priority routing.
- New: Right-click → **Sound Settings...** opens Windows Sound dialog directly on the Recording tab.
- New: Middle-click tray icon → compact volume slider popup with a custom dark design. Drag to adjust microphone volume live; click outside or press Escape to close.
- New: Scroll wheel over the tray icon (Click to Swap mode) adjusts microphone volume, matching native Windows sound icon behaviour.

### v1.4.0
- Context menu now follows your Windows dark/light theme.
- Active device shown at the top of the right-click menu.

### v1.3.0
- Renamed from MicroSwap to MicSwitch.
- Added donate button on the mod page.

### v1.2.0
- Fixed: rare crash when switching devices rapidly.
- Fixed: occasional hang when Windhawk unloads the mod.
- Fixed: tray window appeared in Alt+Tab.
- Fixed: tray icon failed to load on some Windows configurations.

### v1.1.0
- Custom icon support — pick your own image for each device via Settings or right-click.

### v1.0.0
- Initial release.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- advancedMode: false
  $name: Advanced Mode
  $description: Shows the Device Priority panel in Mod Settings, letting you rank preferred microphones for automatic slot assignment.
*/
// ==/WindhawkModSettings==

// Dashboard-level settings (device slots, icons, priority list) are managed via
// right-click → Mod Settings and persisted with Wh_SetStringValue / Wh_GetStringValue.
// The advancedMode flag above uses Wh_GetIntSetting, which reads from Windhawk's
// separate YAML store — no conflict with dashboard keys.

#define NOMINMAX
#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <endpointvolume.h>
#include <commctrl.h>
#include <dwmapi.h>

#include <propkey.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <functiondiscoverykeys_devpkey.h>
#include <commdlg.h>
#include <algorithm>
#include <vector>
#include <string>

// Stable GUID that gives our tray icon a process-independent identity.
// Windows uses this to track pin/unpin separately from windhawk.exe.
static const GUID MICSWITCH_TRAY_GUID =
    {0x7174FA7E, 0x93F1, 0x4110, {0x8B, 0x83, 0xA4, 0xAD, 0x2C, 0x76, 0x9C, 0x3B}};

#define TRAY_ICON_ID         1
#define WM_TRAY_CALLBACK     (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define WM_TRAY_SCROLL       (WM_USER + 3)  // wParam = direction (+1 or -1)
#define WM_UPDATE_HOOK_STATE (WM_USER + 4)
#define WM_SHOW_FILE_PICKER  (WM_USER + 5)  // lParam = bitmask of slots needing pickers
#define WM_RELOAD_ICONS      (WM_USER + 6)  // reload icons on tray thread (eliminates cross-thread handle race)
#define WM_RELOAD_ALL              (WM_USER + 7)  // full reload after dashboard save
#define WM_PRIORITY_DEVICE_ACTIVE  (WM_USER + 8)  // lParam = heap-alloc'd WCHAR* device ID
#define WM_REBIND_VOLUME_CALLBACK  (WM_USER + 9)  // rebind IAudioEndpointVolumeCallback after default-device change
#define TRAY_RECT_INIT_TIMER 99   // one-shot retry timer for Shell_NotifyIconGetRect

// Sentinel context GUID — distinguishes volume changes triggered by our own
// VolumePopup slider from external (volume keys / other apps)
// when IAudioEndpointVolumeCallback::OnNotify fires.
static const GUID kVolumeChangeCtx =
    {0x4C3B2A1D, 0x9E8F, 0x4D7A, {0xB6, 0x5C, 0x3F, 0x2D, 0x1E, 0x4B, 0x5A, 0x6C}};

#define MENU_OPEN_SETTINGS   9001
#define MENU_OPEN_WINDHAWK   9000
#define IDC_BTN_KOFI         9002
#define MENU_SOUND_SETTINGS  9003

// With NOTIFYICON_VERSION_4 active Windows changes the tray notifications:
//   left-click  → NIN_SELECT       (instead of / alongside WM_LBUTTONUP)
//   right-click → WM_CONTEXTMENU   (instead of / alongside WM_RBUTTONUP)
//   keyboard    → NIN_KEYSELECT
// We accept both old and new forms so the icon behaves the same in either mode.
#ifndef NIN_SELECT
#define NIN_SELECT     (WM_USER + 0)
#endif
#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT  (NIN_SELECT | 0x1)
#endif

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
static WCHAR          g_lastIconSetting[MAX_DEVICE_SLOTS][32] = {};
static HICON          g_hWindHawkIcon     = nullptr;
static HBITMAP        g_hWindHawkBmp      = nullptr;
static DWORD          g_lastClickTime     = 0;
static DWORD          g_lastScrollTime    = 0;
static UINT           g_taskbarCreatedMsg = 0;

// Tray icon position (used by Raw Input scroll handler)
static RECT           g_trayIconRect      = {};

// Per-slot icon handles
static HICON          g_iconDev[MAX_DEVICE_SLOTS]     = {};
static HBITMAP        g_hIconDevBmp[MAX_DEVICE_SLOTS] = {};

// ── Shared state — protected by g_stateLock ──────────────────────────────────
// Read by the worker thread (CycleAudioDevice) and tray thread (UpdateTrayTip,
// BuildAndShowContextMenu). Written by LoadDeviceSelections/LoadUserIconsAndSettings
// (main thread or tray thread via WM_RELOAD_ALL) and ToggleMuteCurrentDevice (tray thread).
static CRITICAL_SECTION g_stateLock;
static WCHAR  g_cachedDevId[MAX_DEVICE_SLOTS][512]  = {};
static WCHAR  g_cachedDevName[MAX_DEVICE_SLOTS][256] = {};
static int    g_deviceSlotCount = 2;
static bool   g_isMutedByUs     = false;
static WCHAR  g_mutedDeviceId[512] = {};
// ─────────────────────────────────────────────────────────────────────────────

// Priority device list — up to MAX_DEVICE_SLOTS entries, index 0 = highest priority.
// Written on main/tray thread under no lock (only touched during reload or init).
static WCHAR  g_priorityDevIds[MAX_DEVICE_SLOTS][512] = {};
static int    g_priorityCount = 0;

// Mode flag: set/read atomically with InterlockedExchange/InterlockedRead.
static volatile LONG  g_scrollToSwap = 0;  // 1 = scroll mode

// IMMNotificationClient registration
class MicDeviceNotifier;
class VolNotifier;
static IMMDeviceEnumerator*  g_notifEnum      = nullptr;
static MicDeviceNotifier*    g_deviceNotifier = nullptr;
static IAudioEndpointVolume*  g_pEndpointVol  = nullptr;  // tray-thread owned; current default endpoint
static VolNotifier*           g_pVolNotifier  = nullptr;  // registered on g_pEndpointVol

// Dashboard GUI thread — written only from the tray thread (BuildAndShowContextMenu)
// and read only from the main thread (WhTool_ModUninit), which runs after the
// tray thread has been waited for. No concurrent access → no lock needed.
static HANDLE        g_guiThread  = nullptr;
static volatile LONG g_guiRunning = 0;  // 1 while dashboard window is open

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
        if (wcscmp(s, L"mic_classic") == 0)           return 3;
        if (wcscmp(s, L"mic_modern") == 0)            return 92;
        if (wcscmp(s, L"headphones") == 0)            return 2;
        if (wcscmp(s, L"headset_gaming") == 0)        return 8;
        if (wcscmp(s, L"headphones_modern") == 0)     return 91;
        if (wcscmp(s, L"headset_modern") == 0)        return 95;
        if (wcscmp(s, L"earphones") == 0)             return 6;
    }
    return 3;
}

// ─── Settings dashboard ───────────────────────────────────────────────────────

namespace MicSwitchGui {

    // ── Palette ───────────────────────────────────────────────────────────────
    static const COLORREF kClrBg      = RGB(24, 24, 24);
    static const COLORREF kClrSurface = RGB(36, 36, 36);
    static const COLORREF kClrBorder  = RGB(56, 56, 56);
    static const COLORREF kClrText    = RGB(224, 224, 224);
    static const COLORREF kClrDim     = RGB(128, 128, 128);
    static const COLORREF kClrInput   = RGB(28, 28, 28);
    static const COLORREF kClrAccent  = RGB(0, 120, 212);
    static const COLORREF kClrAccentP = RGB(0,  96, 170);  // pressed
    static const COLORREF kClrDarkBtn = RGB(44, 44, 44);
    static const COLORREF kClrDarkBP  = RGB(32, 32, 32);   // pressed

    // ── Layout constants ──────────────────────────────────────────────────────
    static const int kSW       = 414;  // slots panel width (right)
    static const int kPW       = 220;  // priority panel width (left)
    static const int kGap      =  12;  // gap between panels
    static const int kCW       = kSW + kGap + kPW;  // total client width (646)
    static const int kTopH     =  72;  // height of global-settings section
    static const int kSlotH    =  76;  // height per slot row
    static const int kPrioRowH =  52;  // height per priority row (device + icon lines)
    static const int kIconSz   =  28;  // icon drawn size

    struct DeviceInfo { std::wstring id, name; };

    struct State {
        HWND hTrayHwnd = nullptr;
        HWND hMainWnd  = nullptr;

        HFONT  hFont      = nullptr;
        HBRUSH hBgBrush   = nullptr;   // kClrBg  — returned from WM_CTLCOLOR*
        HBRUSH hInpBrush  = nullptr;   // kClrInput
        HBRUSH hSurfBrush = nullptr;   // kClrSurface

        int deviceCount = 2;
        int swapMode    = 0;

        std::vector<DeviceInfo> activeDevices;

        struct Slot {
            HWND  hDevCombo    = nullptr;
            HWND  hIconCombo   = nullptr;
            HWND  hBrowseBtn   = nullptr;
            HICON hPreviewIcon = nullptr;
            std::wstring id, name, iconKey;
            WCHAR customPath[MAX_PATH] = {};
        } slots[6];

        HWND hCountCombo = nullptr;
        HWND hModeCombo  = nullptr;
        HWND hSaveBtn    = nullptr;
        HWND hCancelBtn  = nullptr;
        HWND hKoFiBtn    = nullptr;

        UINT dpi = 96;
        bool advancedMode = false;

        // Priority section
        struct PrioSlot {
            HWND hDevCombo  = nullptr;
            HWND hIconCombo = nullptr;
            HWND hBrowseBtn = nullptr;
            std::wstring id, name, iconKey;
            WCHAR customPath[MAX_PATH] = {};
            bool isOffline = false;
        } prioSlots[MAX_DEVICE_SLOTS];
    };

    static const WCHAR* kIconKeys[] = {
        L"mic_classic", L"mic_modern", L"headphones",
        L"headset_gaming", L"headphones_modern", L"headset_modern",
        L"earphones", L"custom"
    };
    static const WCHAR* kIconLabels[] = {
        L"Classic Microphone", L"Modern Microphone", L"Headphones",
        L"Gaming Headset", L"Modern Headphones", L"Modern Gaming Headset",
        L"Earphones", L"Custom Icon..."
    };
    static const int kIconCount = 8;

    // ── Helpers ───────────────────────────────────────────────────────────────

    // Scale a base-96-DPI pixel value to the current DPI.
    static int Sc(int px, UINT dpi) { return MulDiv(px, (int)dpi, 96); }

    // x-origin of the slots (right) panel.
    static int SlotsX(UINT dpi) { return Sc(kPW + kGap, dpi); }

    // y-coordinate of slot i's top edge in client space (slots panel).
    static int SlotY(int i, UINT dpi){ return Sc(kTopH,dpi) + i * Sc(kSlotH,dpi); }

    // y-coordinate of priority row i's top edge in client space (priority panel).
    static int PrioRowY(int i, UINT dpi){ return Sc(32,dpi) + i * Sc(kPrioRowH,dpi); }

    // Extract the preview icon for a slot (32px). Caller owns the HICON.
    static HICON LoadSlotPreview(const std::wstring& key, const WCHAR* customPath) {
        HICON h = nullptr;
        if (key == L"custom" && customPath && customPath[0])
            h = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON,
                                  kIconSz, kIconSz, LR_LOADFROMFILE);
        if (!h) ExtractIconExW(g_ddoresDllPath,
                               GetIconIndex(key.empty() ? nullptr : key.c_str()),
                               nullptr, &h, 1);
        return h;
    }

    static void RefreshPreview(State* s, int i) {
        if (s->slots[i].hPreviewIcon) { DestroyIcon(s->slots[i].hPreviewIcon); }
        s->slots[i].hPreviewIcon = LoadSlotPreview(s->slots[i].iconKey, s->slots[i].customPath);
    }

    // Apply DarkMode_CFD theme to a combo so its dropdown renders dark on Win10+.
    static void DarkCombo(HWND h) {
        HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
        if (!ux) ux = LoadLibraryW(L"uxtheme.dll");
        if (!ux) return;
        using Fn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
        auto fn = reinterpret_cast<Fn>(GetProcAddress(ux, "SetWindowTheme"));
        if (fn) fn(h, L"DarkMode_CFD", nullptr);
    }

    // Reposition all child controls and resize the window using the current DPI.
    static void LayoutControls(State* s) {
        if (!s->hMainWnd) return;
        UINT d = s->dpi;
        int sx = s->advancedMode ? SlotsX(d) : 0;

        // Top row (right panel)
        SetWindowPos(s->hCountCombo, nullptr, sx+Sc(80,d), Sc(18,d), Sc(62,d), Sc(200,d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hModeCombo,  nullptr, sx+Sc(198,d), Sc(18,d), Sc(204,d), Sc(200,d), SWP_NOZORDER|SWP_NOACTIVATE);

        // Slot controls (right panel)
        for (int i = 0; i < 6; i++) {
            int y = SlotY(i, d);
            SetWindowPos(s->slots[i].hDevCombo,  nullptr, sx+Sc(46,d), y+Sc(22,d), Sc(356,d), Sc(300,d), SWP_NOZORDER|SWP_NOACTIVATE);
            SetWindowPos(s->slots[i].hIconCombo, nullptr, sx+Sc(46,d), y+Sc(50,d), Sc(258,d), Sc(300,d), SWP_NOZORDER|SWP_NOACTIVATE);
            SetWindowPos(s->slots[i].hBrowseBtn, nullptr, sx+Sc(310,d), y+Sc(48,d), Sc(92,d), Sc(26,d), SWP_NOZORDER|SWP_NOACTIVATE);
        }

        // Priority controls (left panel)
        // x=50 leaves room for the "1st:" etc. rank label to the left of each device combo.
        for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
            int rowY = PrioRowY(i, d);
            bool showBrowse = s->advancedMode && (s->prioSlots[i].iconKey == L"custom");
            int iconW = showBrowse ? Sc(106,d) : Sc(158,d);
            SetWindowPos(s->prioSlots[i].hDevCombo,  nullptr, Sc(50,d),  rowY,          Sc(158,d), Sc(200,d), SWP_NOZORDER|SWP_NOACTIVATE);
            SetWindowPos(s->prioSlots[i].hIconCombo, nullptr, Sc(50,d),  rowY+Sc(26,d), iconW,     Sc(200,d), SWP_NOZORDER|SWP_NOACTIVATE);
            SetWindowPos(s->prioSlots[i].hBrowseBtn, nullptr, Sc(158,d), rowY+Sc(24,d), Sc(50,d),  Sc(24,d),  SWP_NOZORDER|SWP_NOACTIVATE);
        }

        // Button row (right panel, below visible slots)
        int btnY = SlotY(s->deviceCount, d) + Sc(8,d);
        SetWindowPos(s->hSaveBtn,   nullptr, sx+Sc(12,d),  btnY, Sc(148,d), Sc(28,d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hCancelBtn, nullptr, sx+Sc(168,d), btnY, Sc(88,d),  Sc(28,d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hKoFiBtn,   nullptr, sx+Sc(264,d), btnY, Sc(138,d), Sc(28,d), SWP_NOZORDER|SWP_NOACTIVATE);

        int prioPanelH = Sc(32,d) + MAX_DEVICE_SLOTS * Sc(kPrioRowH,d) + Sc(12,d);
        int slotsPanelH = btnY + Sc(28,d) + Sc(12,d);
        int clientH = prioPanelH > slotsPanelH ? prioPanelH : slotsPanelH;

        RECT rc = {0, 0, Sc(s->advancedMode ? kCW : kSW, d), clientH};
        AdjustWindowRectEx(&rc,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            FALSE, WS_EX_DLGMODALFRAME);
        RECT wr; GetWindowRect(s->hMainWnd, &wr);
        SetWindowPos(s->hMainWnd, nullptr, wr.left, wr.top,
                     rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    static void UpdateSlotVisibility(State* s) {
        for (int i = 0; i < 6; i++) {
            bool vis = (i < s->deviceCount);
            ShowWindow(s->slots[i].hDevCombo,  vis ? SW_SHOW : SW_HIDE);
            ShowWindow(s->slots[i].hIconCombo, vis ? SW_SHOW : SW_HIDE);
            ShowWindow(s->slots[i].hBrowseBtn,
                       (vis && s->slots[i].iconKey == L"custom") ? SW_SHOW : SW_HIDE);
        }
        for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
            ShowWindow(s->prioSlots[i].hDevCombo,  s->advancedMode ? SW_SHOW : SW_HIDE);
            ShowWindow(s->prioSlots[i].hIconCombo, s->advancedMode ? SW_SHOW : SW_HIDE);
            ShowWindow(s->prioSlots[i].hBrowseBtn,
                       (s->advancedMode && s->prioSlots[i].iconKey == L"custom") ? SW_SHOW : SW_HIDE);
        }
        LayoutControls(s);
        if (s->hMainWnd) InvalidateRect(s->hMainWnd, nullptr, TRUE);
    }

    static BOOL CALLBACK ApplyFontProc(HWND child, LPARAM hf) {
        SendMessageW(child, WM_SETFONT, (WPARAM)hf, TRUE);
        return TRUE;
    }

    // ── State I/O ─────────────────────────────────────────────────────────────

    static void LoadState(State& s) {
        s.advancedMode = (Wh_GetIntSetting(L"advancedMode") != 0);

        IMMDeviceEnumerator* pEnum = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                       CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                       (void**)&pEnum))) {
            IMMDeviceCollection* pColl = nullptr;
            if (SUCCEEDED(pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pColl))) {
                UINT count = 0; pColl->GetCount(&count);
                for (UINT i = 0; i < count; i++) {
                    IMMDevice* pDev = nullptr;
                    if (SUCCEEDED(pColl->Item(i, &pDev))) {
                        LPWSTR pId = nullptr;
                        if (SUCCEEDED(pDev->GetId(&pId))) {
                            IPropertyStore* pStore = nullptr;
                            if (SUCCEEDED(pDev->OpenPropertyStore(STGM_READ, &pStore))) {
                                PROPVARIANT v; PropVariantInit(&v);
                                if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &v))
                                    && v.pwszVal)
                                    s.activeDevices.push_back({pId, v.pwszVal});
                                PropVariantClear(&v); pStore->Release();
                            }
                            CoTaskMemFree(pId);
                        }
                        pDev->Release();
                    }
                }
                pColl->Release();
            }
            pEnum->Release();
        }

        {
            WCHAR dcBuf[8] = {};
            Wh_GetStringValue(L"deviceCount", dcBuf, 8);
            int n = 0;
            for (const WCHAR* p = dcBuf; *p >= L'0' && *p <= L'9'; p++) n = n*10 + (*p-L'0');
            s.deviceCount = (n >= 2 && n <= 6) ? n : 2;
        }
        {
            WCHAR modeBuf[32] = {};
            Wh_GetStringValue(L"swapMode", modeBuf, 32);
            s.swapMode = (wcscmp(modeBuf, L"scroll_to_swap") == 0) ? 1 : 0;
        }

        for (int i = 0; i < 6; i++) {
            WCHAR kId[32], kNm[32], kPth[32], buf[512];
            swprintf_s(kId,  L"Device%dId",         i+1);
            swprintf_s(kNm,  L"Device%dName",       i+1);
            swprintf_s(kPth, L"icon%d_custom_path", i+1);
            Wh_GetStringValue(kId,  buf, 512); s.slots[i].id   = buf;
            Wh_GetStringValue(kNm,  buf, 256); s.slots[i].name = buf;
            WCHAR kIco[16]; swprintf_s(kIco, L"icon%d", i+1);
            WCHAR ikBuf[32] = {};
            Wh_GetStringValue(kIco, ikBuf, 32);
            s.slots[i].iconKey = ikBuf[0] ? ikBuf : L"mic_classic";
            Wh_GetStringValue(kPth, s.slots[i].customPath, MAX_PATH);
            s.slots[i].hPreviewIcon = LoadSlotPreview(s.slots[i].iconKey, s.slots[i].customPath);
        }

        for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
            WCHAR kId[32], kNm[32], kIco[32], kPth[32], buf[512];
            swprintf_s(kId,  L"priority%d",                 i+1);
            swprintf_s(kNm,  L"priority%d_name",             i+1);
            swprintf_s(kIco, L"priority%d_icon",             i+1);
            swprintf_s(kPth, L"priority%d_icon_custom_path", i+1);
            Wh_GetStringValue(kId, buf, 512); s.prioSlots[i].id   = buf;
            Wh_GetStringValue(kNm, buf, 512); s.prioSlots[i].name = buf;
            WCHAR ikBuf[32] = {};
            Wh_GetStringValue(kIco, ikBuf, 32);
            s.prioSlots[i].iconKey = ikBuf[0] ? ikBuf : L"mic_classic";
            Wh_GetStringValue(kPth, s.prioSlots[i].customPath, MAX_PATH);
            s.prioSlots[i].isOffline = false;
            if (!s.prioSlots[i].id.empty()) {
                bool found = false;
                for (auto& dev : s.activeDevices)
                    if (dev.id == s.prioSlots[i].id) { found = true; break; }
                if (!found) s.prioSlots[i].isOffline = true;
            }
        }
    }

    // ── Window procedure ──────────────────────────────────────────────────────

    static LRESULT CALLBACK DashboardWndProc(HWND hWnd, UINT msg,
                                             WPARAM wParam, LPARAM lParam) {
        State* s = reinterpret_cast<State*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        switch (msg) {

        // ── Initialise ────────────────────────────────────────────────────────
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            s = reinterpret_cast<State*>(cs->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s));
            s->hMainWnd = hWnd;
            s->dpi      = GetDpiForWindow(hWnd);
            {
                LOGFONTW lf = {};
                lf.lfHeight  = -MulDiv(10, (int)s->dpi, 72);  // 10pt at current DPI
                lf.lfWeight  = FW_NORMAL;
                lf.lfQuality = CLEARTYPE_QUALITY;
                lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
                wcscpy_s(lf.lfFaceName, L"Segoe UI");
                s->hFont = CreateFontIndirectW(&lf);
            }
            s->hBgBrush   = CreateSolidBrush(kClrBg);
            s->hInpBrush  = CreateSolidBrush(kClrInput);
            s->hSurfBrush = CreateSolidBrush(kClrSurface);

            HINSTANCE hInst = GetModuleHandleW(nullptr);

            // ── Top row: Devices count + Interaction Mode ─────────────────────
            // All labels are painted in WM_PAINT; only combos are child HWNDs.
            // Layout (x): "Devices:" @12 → combo @80 w=62 → "Mode:" @156 → combo @198 w=204
            s->hCountCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 80, 18, 62, 200,
                hWnd, (HMENU)(UINT_PTR)100, hInst, nullptr);
            for (int i = 2; i <= 6; i++) {
                WCHAR b[4]; swprintf_s(b, L"%d", i);
                SendMessageW(s->hCountCombo, CB_ADDSTRING, 0, (LPARAM)b);
            }
            SendMessageW(s->hCountCombo, CB_SETCURSEL, s->deviceCount - 2, 0);
            DarkCombo(s->hCountCombo);

            s->hModeCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 198, 18, 204, 200,
                hWnd, (HMENU)(UINT_PTR)101, hInst, nullptr);
            SendMessageW(s->hModeCombo, CB_ADDSTRING, 0, (LPARAM)L"Click to Swap");
            SendMessageW(s->hModeCombo, CB_ADDSTRING, 0, (LPARAM)L"Scroll to Swap");
            SendMessageW(s->hModeCombo, CB_SETCURSEL, s->swapMode, 0);
            DarkCombo(s->hModeCombo);

            // ── Per-slot controls ─────────────────────────────────────────────
            // Controls are created at (0,0) and repositioned by LayoutControls below.
            for (int i = 0; i < 6; i++) {
                s->slots[i].hDevCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|CBS_DROPDOWNLIST, 0, 0, 10, 300,
                    hWnd, (HMENU)(UINT_PTR)(200+i), hInst, nullptr);
                for (int j = 0; j < (int)s->activeDevices.size(); j++) {
                    int idx = (int)SendMessageW(s->slots[i].hDevCombo, CB_ADDSTRING,
                                                0, (LPARAM)s->activeDevices[j].name.c_str());
                    if (s->slots[i].id == s->activeDevices[j].id)
                        SendMessageW(s->slots[i].hDevCombo, CB_SETCURSEL, idx, 0);
                }
                DarkCombo(s->slots[i].hDevCombo);

                s->slots[i].hIconCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|CBS_DROPDOWNLIST, 0, 0, 10, 300,
                    hWnd, (HMENU)(UINT_PTR)(300+i), hInst, nullptr);
                int isel = 0;
                for (int j = 0; j < kIconCount; j++) {
                    SendMessageW(s->slots[i].hIconCombo, CB_ADDSTRING, 0, (LPARAM)kIconLabels[j]);
                    if (s->slots[i].iconKey == kIconKeys[j]) isel = j;
                }
                SendMessageW(s->slots[i].hIconCombo, CB_SETCURSEL, isel, 0);
                DarkCombo(s->slots[i].hIconCombo);

                s->slots[i].hBrowseBtn = CreateWindowExW(0, L"BUTTON", L"Browse...",
                    WS_CHILD|BS_OWNERDRAW, 0, 0, 10, 10,
                    hWnd, (HMENU)(UINT_PTR)(400+i), hInst, nullptr);
            }

            // ── Priority controls (left panel) ────────────────────────────────
            for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
                // Device combo (500+i)
                s->prioSlots[i].hDevCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 0, 0, 10, 300,
                    hWnd, (HMENU)(UINT_PTR)(500+i), hInst, nullptr);
                SendMessageW(s->prioSlots[i].hDevCombo, CB_ADDSTRING, 0, (LPARAM)L"None");
                int psel = 0;
                for (int j = 0; j < (int)s->activeDevices.size(); j++) {
                    int idx = (int)SendMessageW(s->prioSlots[i].hDevCombo, CB_ADDSTRING,
                                                0, (LPARAM)s->activeDevices[j].name.c_str());
                    if (s->prioSlots[i].id == s->activeDevices[j].id) psel = idx;
                }
                if (s->prioSlots[i].isOffline && !s->prioSlots[i].name.empty()) {
                    WCHAR offLabel[320];
                    swprintf_s(offLabel, L"%s (offline)", s->prioSlots[i].name.c_str());
                    int idx = (int)SendMessageW(s->prioSlots[i].hDevCombo, CB_ADDSTRING,
                                                0, (LPARAM)offLabel);
                    psel = idx;
                }
                SendMessageW(s->prioSlots[i].hDevCombo, CB_SETCURSEL, psel, 0);
                DarkCombo(s->prioSlots[i].hDevCombo);

                // Icon combo (600+i)
                s->prioSlots[i].hIconCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 0, 0, 10, 300,
                    hWnd, (HMENU)(UINT_PTR)(600+i), hInst, nullptr);
                int isel = 0;
                for (int j = 0; j < kIconCount; j++) {
                    SendMessageW(s->prioSlots[i].hIconCombo, CB_ADDSTRING, 0, (LPARAM)kIconLabels[j]);
                    if (s->prioSlots[i].iconKey == kIconKeys[j]) isel = j;
                }
                SendMessageW(s->prioSlots[i].hIconCombo, CB_SETCURSEL, isel, 0);
                DarkCombo(s->prioSlots[i].hIconCombo);

                // Browse button (700+i) — visible only when iconKey == "custom"
                s->prioSlots[i].hBrowseBtn = CreateWindowExW(0, L"BUTTON", L"Browse...",
                    WS_CHILD|BS_OWNERDRAW|(s->prioSlots[i].iconKey == L"custom" ? WS_VISIBLE : 0),
                    0, 0, 10, 10,
                    hWnd, (HMENU)(UINT_PTR)(700+i), hInst, nullptr);
            }

            // ── Bottom buttons ────────────────────────────────────────────────
            s->hSaveBtn = CreateWindowExW(0, L"BUTTON", L"Save and Apply",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)IDOK, hInst, nullptr);
            s->hCancelBtn = CreateWindowExW(0, L"BUTTON", L"Cancel",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)IDCANCEL, hInst, nullptr);
            s->hKoFiBtn = CreateWindowExW(0, L"BUTTON", L"Buy Me Coffee",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)IDC_BTN_KOFI, hInst, nullptr);

            EnumChildWindows(hWnd, ApplyFontProc, reinterpret_cast<LPARAM>(s->hFont));
            UpdateSlotVisibility(s);  // also calls LayoutControls → positions everything

            // Request dark title bar from DWM (Win10 1903+, silently fails earlier)
            {
                HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
                if (!dwm) dwm = LoadLibraryW(L"dwmapi.dll");
                if (dwm) {
                    using DwmFn = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
                    auto fn = reinterpret_cast<DwmFn>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
                    if (fn) { BOOL dark = TRUE; fn(hWnd, 20, &dark, sizeof(dark)); }
                }
            }
            return 0;
        }

        // ── Background & painting ─────────────────────────────────────────────
        case WM_ERASEBKGND: {
            RECT rc; GetClientRect(hWnd, &rc);
            FillRect((HDC)wParam, &rc, s ? s->hBgBrush : (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SelectObject(hdc, s->hFont);
            SetBkMode(hdc, TRANSPARENT);
            RECT cr; GetClientRect(hWnd, &cr);
            UINT d = s->dpi;
            int sx = s->advancedMode ? SlotsX(d) : 0;

            if (s->advancedMode) {
                // ── Left panel background + vertical separator ────────────────
                {
                    RECT lp = {0, 0, Sc(kPW,d), cr.bottom};
                    FillRect(hdc, &lp, s->hSurfBrush);
                    HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                    HPEN op = (HPEN)SelectObject(hdc, p);
                    MoveToEx(hdc, Sc(kPW,d), 0, nullptr);
                    LineTo(hdc, Sc(kPW,d), cr.bottom);
                    SelectObject(hdc, op); DeleteObject(p);
                }

                // ── Left panel: Device Priority header + rank labels ──────────
                static const WCHAR* kRankLabels[] = {L"1st:", L"2nd:", L"3rd:", L"4th:", L"5th:", L"6th:"};
                SetTextColor(hdc, kClrText);
                {
                    RECT r2 = {Sc(12,d), Sc(10,d), Sc(kPW-4,d), Sc(28,d)};
                    DrawTextW(hdc, L"Device Priority", -1, &r2, DT_LEFT|DT_TOP);
                }
                SetTextColor(hdc, kClrDim);
                for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
                    int rowY = PrioRowY(i, d);
                    RECT rk = {Sc(12,d), rowY + Sc(3,d),  Sc(48,d), rowY + Sc(19,d)};
                    RECT ri = {Sc(12,d), rowY + Sc(29,d), Sc(48,d), rowY + Sc(45,d)};
                    DrawTextW(hdc, kRankLabels[i], -1, &rk, DT_LEFT|DT_TOP);
                    DrawTextW(hdc, L"icon:",        -1, &ri, DT_LEFT|DT_TOP);
                }
            }

            // ── Right panel: top-section labels ───────────────────────────────
            SetTextColor(hdc, kClrDim);
            RECT r;
            r = {sx+Sc(12,d), Sc(22,d), sx+Sc(78,d), Sc(38,d)};
            DrawTextW(hdc, L"Devices:", -1, &r, DT_LEFT|DT_TOP);
            r = {sx+Sc(156,d), Sc(22,d), sx+Sc(196,d), Sc(38,d)};
            DrawTextW(hdc, L"Mode:", -1, &r, DT_LEFT|DT_TOP);

            // Separator between top controls and slot section
            {
                HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                HPEN op = (HPEN)SelectObject(hdc, p);
                MoveToEx(hdc, sx+Sc(12,d), Sc(kTopH,d) - Sc(6,d), nullptr);
                LineTo(hdc, cr.right - Sc(12,d), Sc(kTopH,d) - Sc(6,d));
                SelectObject(hdc, op); DeleteObject(p);
            }

            // ── Per-slot headers, icons, and row labels ───────────────────────
            for (int i = 0; i < s->deviceCount; i++) {
                int y = SlotY(i, d);

                RECT hdr = {sx, y, cr.right, y + Sc(20,d)};
                FillRect(hdc, &hdr, s->hSurfBrush);

                SetTextColor(hdc, kClrDim);
                RECT sl = {sx+Sc(12,d), y + Sc(2,d), sx+Sc(100,d), y + Sc(18,d)};
                WCHAR lbl[16]; swprintf_s(lbl, L"Slot %d", i + 1);
                DrawTextW(hdc, lbl, -1, &sl, DT_LEFT|DT_TOP);

                if (s->slots[i].hPreviewIcon)
                    DrawIconEx(hdc, sx+Sc(12,d), y + Sc(22,d), s->slots[i].hPreviewIcon,
                               Sc(kIconSz,d), Sc(kIconSz,d), 0, nullptr, DI_NORMAL);

                SetTextColor(hdc, kClrDim);
                RECT il = {sx+Sc(12,d), y + Sc(54,d), sx+Sc(44,d), y + Sc(68,d)};
                DrawTextW(hdc, L"Icon:", -1, &il, DT_LEFT|DT_TOP);

                if (i < s->deviceCount - 1) {
                    HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                    HPEN op = (HPEN)SelectObject(hdc, p);
                    MoveToEx(hdc, sx, y + Sc(kSlotH,d) - 1, nullptr);
                    LineTo(hdc, cr.right, y + Sc(kSlotH,d) - 1);
                    SelectObject(hdc, op); DeleteObject(p);
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        // ── Dark theming for child controls ───────────────────────────────────
        case WM_CTLCOLORSTATIC:
            if (s) {
                SetTextColor((HDC)wParam, kClrDim);
                SetBkColor((HDC)wParam, kClrBg);
                return (LRESULT)s->hBgBrush;
            }
            break;
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
            if (s) {
                SetTextColor((HDC)wParam, kClrText);
                SetBkColor((HDC)wParam, kClrInput);
                return (LRESULT)s->hInpBrush;
            }
            break;
        case WM_CTLCOLORBTN:
            if (s) return (LRESULT)s->hBgBrush;
            break;

        // ── Owner-draw buttons ────────────────────────────────────────────────
        case WM_DRAWITEM: {
            auto* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            if (dis->CtlType != ODT_BUTTON || !s) break;
            bool pressed = (dis->itemState & ODS_SELECTED) != 0;
            bool isSave  = (dis->CtlID == IDOK);
            bool isBrowse= (dis->CtlID >= 400 && dis->CtlID < 406) ||
                           (dis->CtlID >= 700 && dis->CtlID < 706);

            COLORREF bg;
            if      (isSave)   bg = pressed ? kClrAccentP : kClrAccent;
            else if (isBrowse) bg = pressed ? kClrDarkBP  : kClrSurface;
            else               bg = pressed ? kClrDarkBP  : kClrDarkBtn;

            HBRUSH hFill = CreateSolidBrush(bg);
            FillRect(dis->hDC, &dis->rcItem, hFill);
            DeleteObject(hFill);

            // 1px border
            HPEN hPen = CreatePen(PS_SOLID, 1,
                isSave ? kClrAccentP : kClrBorder);
            HPEN hOp  = (HPEN)SelectObject(dis->hDC, hPen);
            SelectObject(dis->hDC, GetStockObject(NULL_BRUSH));
            Rectangle(dis->hDC, dis->rcItem.left, dis->rcItem.top,
                                 dis->rcItem.right, dis->rcItem.bottom);
            SelectObject(dis->hDC, hOp); DeleteObject(hPen);

            // Button label
            WCHAR txt[64] = {}; GetWindowTextW(dis->hwndItem, txt, 64);
            SetTextColor(dis->hDC, kClrText);
            SetBkMode(dis->hDC, TRANSPARENT);
            SelectObject(dis->hDC, s->hFont);
            DrawTextW(dis->hDC, txt, -1, &dis->rcItem,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            if (dis->itemState & ODS_FOCUS) DrawFocusRect(dis->hDC, &dis->rcItem);
            return TRUE;
        }

        // ── User interaction ──────────────────────────────────────────────────
        case WM_COMMAND: {
            int id = LOWORD(wParam);

            if (id == 100 && HIWORD(wParam) == CBN_SELCHANGE) {
                // Device count changed — resize window and show/hide slot rows
                s->deviceCount = (int)SendMessageW(s->hCountCombo, CB_GETCURSEL, 0, 0) + 2;
                UpdateSlotVisibility(s);  // calls LayoutControls internally

            } else if (id >= 300 && id < 306 && HIWORD(wParam) == CBN_SELCHANGE) {
                // Icon style changed for slot (id-300) — live preview update
                int slot = id - 300;
                int sel  = (int)SendMessageW(s->slots[slot].hIconCombo, CB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < kIconCount) s->slots[slot].iconKey = kIconKeys[sel];
                RefreshPreview(s, slot);
                UINT d = s->dpi;
                int sx = s->advancedMode ? SlotsX(d) : 0;
                int y = SlotY(slot, d);
                RECT ir = {sx+Sc(12,d), y+Sc(20,d), sx+Sc(12,d)+Sc(kIconSz,d)+2, y+Sc(20,d)+Sc(kIconSz,d)+2};
                InvalidateRect(hWnd, &ir, TRUE);
                UpdateSlotVisibility(s);  // toggles Browse button

            } else if (id >= 400 && id < 406) {
                // Browse for custom .ico
                int slot = id - 400;
                WCHAR path[MAX_PATH] = {};
                wcscpy_s(path, s->slots[slot].customPath);
                WCHAR title[64]; swprintf_s(title, L"Select Icon for Slot %d", slot + 1);
                OPENFILENAMEW ofn = {sizeof(ofn)};
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter  = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFile    = path;
                ofn.nMaxFile     = MAX_PATH;
                ofn.lpstrTitle   = title;
                ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR;
                if (GetOpenFileNameW(&ofn)) {
                    wcscpy_s(s->slots[slot].customPath, path);
                    RefreshPreview(s, slot);
                    UINT d = s->dpi;
                    int sx = s->advancedMode ? SlotsX(d) : 0;
                    int y = SlotY(slot, d);
                    RECT ir = {sx+Sc(12,d), y+Sc(20,d), sx+Sc(12,d)+Sc(kIconSz,d)+2, y+Sc(20,d)+Sc(kIconSz,d)+2};
                    InvalidateRect(hWnd, &ir, TRUE);
                }

            } else if (id >= 600 && id < 606 && HIWORD(wParam) == CBN_SELCHANGE) {
                // Priority icon changed
                int slot = id - 600;
                int sel  = (int)SendMessageW(s->prioSlots[slot].hIconCombo, CB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < kIconCount) s->prioSlots[slot].iconKey = kIconKeys[sel];
                ShowWindow(s->prioSlots[slot].hBrowseBtn,
                           s->prioSlots[slot].iconKey == L"custom" ? SW_SHOW : SW_HIDE);
                LayoutControls(s);

            } else if (id >= 700 && id < 706) {
                // Browse for custom priority icon
                int slot = id - 700;
                WCHAR path[MAX_PATH] = {};
                wcscpy_s(path, s->prioSlots[slot].customPath);
                WCHAR title[64]; swprintf_s(title, L"Select Icon for Priority %d", slot + 1);
                OPENFILENAMEW ofn = {sizeof(ofn)};
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter  = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFile    = path;
                ofn.nMaxFile     = MAX_PATH;
                ofn.lpstrTitle   = title;
                ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR;
                if (GetOpenFileNameW(&ofn))
                    wcscpy_s(s->prioSlots[slot].customPath, path);

            } else if (id == IDOK) {
                s->deviceCount = (int)SendMessageW(s->hCountCombo, CB_GETCURSEL, 0, 0) + 2;
                s->swapMode    = (int)SendMessageW(s->hModeCombo,  CB_GETCURSEL, 0, 0);
                for (int i = 0; i < 6; i++) {
                    int ds = (int)SendMessageW(s->slots[i].hDevCombo, CB_GETCURSEL, 0, 0);
                    if (ds >= 0 && ds < (int)s->activeDevices.size()) {
                        s->slots[i].id   = s->activeDevices[ds].id;
                        s->slots[i].name = s->activeDevices[ds].name;
                    }
                    int is = (int)SendMessageW(s->slots[i].hIconCombo, CB_GETCURSEL, 0, 0);
                    if (is >= 0 && is < kIconCount) s->slots[i].iconKey = kIconKeys[is];
                }
                WCHAR num[4]; swprintf_s(num, L"%d", s->deviceCount);
                Wh_SetStringValue(L"deviceCount", num);
                Wh_SetStringValue(L"swapMode", s->swapMode ? L"scroll_to_swap" : L"click_to_swap");
                for (int i = 0; i < 6; i++) {
                    WCHAR kId[32], kNm[32], kIco[32], kPth[32];
                    swprintf_s(kId,  L"Device%dId",         i+1);
                    swprintf_s(kNm,  L"Device%dName",       i+1);
                    swprintf_s(kIco, L"icon%d",             i+1);
                    swprintf_s(kPth, L"icon%d_custom_path", i+1);
                    Wh_SetStringValue(kId,  s->slots[i].id.c_str());
                    Wh_SetStringValue(kNm,  s->slots[i].name.c_str());
                    Wh_SetStringValue(kIco, s->slots[i].iconKey.c_str());
                    Wh_SetStringValue(kPth, s->slots[i].customPath);
                }
                // Save priority list
                for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
                    WCHAR kId[32], kNm[32], kIco[32], kPth[32];
                    swprintf_s(kId,  L"priority%d",                 i+1);
                    swprintf_s(kNm,  L"priority%d_name",             i+1);
                    swprintf_s(kIco, L"priority%d_icon",             i+1);
                    swprintf_s(kPth, L"priority%d_icon_custom_path", i+1);
                    int psel = (int)SendMessageW(s->prioSlots[i].hDevCombo, CB_GETCURSEL, 0, 0);
                    if (psel == 0) {
                        Wh_SetStringValue(kId, L""); Wh_SetStringValue(kNm, L"");
                    } else if (psel - 1 < (int)s->activeDevices.size()) {
                        Wh_SetStringValue(kId, s->activeDevices[psel-1].id.c_str());
                        Wh_SetStringValue(kNm, s->activeDevices[psel-1].name.c_str());
                    } else {
                        // offline entry selected — preserve saved ID/name
                        Wh_SetStringValue(kId, s->prioSlots[i].id.c_str());
                        Wh_SetStringValue(kNm, s->prioSlots[i].name.c_str());
                    }
                    int pisel = (int)SendMessageW(s->prioSlots[i].hIconCombo, CB_GETCURSEL, 0, 0);
                    if (pisel >= 0 && pisel < kIconCount) {
                        Wh_SetStringValue(kIco, kIconKeys[pisel]);
                        s->prioSlots[i].iconKey = kIconKeys[pisel];
                    }
                    Wh_SetStringValue(kPth, s->prioSlots[i].customPath);
                }
                if (s->hTrayHwnd) PostMessageW(s->hTrayHwnd, WM_RELOAD_ALL, 0, 0);
                DestroyWindow(hWnd);

            } else if (id == IDCANCEL) {
                DestroyWindow(hWnd);
            } else if (id == IDC_BTN_KOFI) {
                ShellExecuteW(nullptr, L"open", L"https://ko-fi.com/blackpaw21",
                              nullptr, nullptr, SW_SHOWNORMAL);
            }
            return 0;
        }

        // ── DPI change ────────────────────────────────────────────────────────
        case WM_DPICHANGED: {
            s->dpi = HIWORD(wParam);
            // Rebuild font at new DPI.
            DeleteObject(s->hFont);
            LOGFONTW lf = {};
            lf.lfHeight  = -MulDiv(10, (int)s->dpi, 72);
            lf.lfWeight  = FW_NORMAL;
            lf.lfQuality = CLEARTYPE_QUALITY;
            lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
            s->hFont = CreateFontIndirectW(&lf);
            EnumChildWindows(hWnd, ApplyFontProc, (LPARAM)s->hFont);
            // Move to OS-suggested rect (avoids visual jump on DPI boundary crossing).
            const RECT* prc = reinterpret_cast<const RECT*>(lParam);
            SetWindowPos(hWnd, nullptr, prc->left, prc->top,
                         prc->right - prc->left, prc->bottom - prc->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            LayoutControls(s);
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }

        // ── Cleanup ───────────────────────────────────────────────────────────
        case WM_DESTROY:
            if (s) {
                DeleteObject(s->hFont);     s->hFont     = nullptr;
                DeleteObject(s->hBgBrush);  s->hBgBrush  = nullptr;
                DeleteObject(s->hInpBrush); s->hInpBrush = nullptr;
                DeleteObject(s->hSurfBrush);s->hSurfBrush= nullptr;
                for (int i = 0; i < 6; i++) {
                    if (s->slots[i].hPreviewIcon) {
                        DestroyIcon(s->slots[i].hPreviewIcon);
                        s->slots[i].hPreviewIcon = nullptr;
                    }
                }
            }
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    // ── GUI thread ────────────────────────────────────────────────────────────

    static DWORD WINAPI GuiThreadProc(LPVOID lpParam) {
        CoInitialize(nullptr);

        // Set Per-Monitor DPI Awareness V2 so Windows doesn't bitmap-scale the window.
        using SetTDACFn = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        if (auto fn = (SetTDACFn)GetProcAddress(
                GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"))
            fn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        static const WCHAR kClass[] = L"MicSwitchDashboard";
        HINSTANCE hInst = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc   = {sizeof(wc)};
        wc.lpfnWndProc   = DashboardWndProc;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;  // WM_ERASEBKGND fills background
        wc.lpszClassName = kClass;
        RegisterClassExW(&wc);

        State state;
        state.hTrayHwnd = reinterpret_cast<HWND>(lpParam);
        LoadState(state);

        // Create at center of primary monitor (LayoutControls resizes in WM_CREATE
        // before ShowWindow, so the initial size is just a placeholder).
        int cxScr = GetSystemMetrics(SM_CXSCREEN);
        int cyScr = GetSystemMetrics(SM_CYSCREEN);
        int sx = cxScr / 4;
        int sy = cyScr / 4;

        HWND hWnd = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            kClass, L"MicSwitch Settings",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            sx, sy, cxScr / 2, cyScr / 2,
            nullptr, nullptr, hInst, &state);

        if (hWnd) {
            ShowWindow(hWnd, SW_SHOW);
            UpdateWindow(hWnd);
            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0)) {
                if (!IsDialogMessageW(hWnd, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }

        // Icons are destroyed in WM_DESTROY; clean up here only if window never opened.
        for (int i = 0; i < 6; i++) {
            if (state.slots[i].hPreviewIcon) {
                DestroyIcon(state.slots[i].hPreviewIcon);
                state.slots[i].hPreviewIcon = nullptr;
            }
        }

        UnregisterClassW(kClass, hInst);
        CoUninitialize();
        InterlockedExchange(&g_guiRunning, 0);
        return 0;
    }

    HANDLE LaunchDashboard(HWND hTrayHwnd) {
        if (InterlockedCompareExchange(&g_guiRunning, 1, 0) != 0)
            return nullptr;
        HANDLE h = CreateThread(nullptr, 0, GuiThreadProc,
                                reinterpret_cast<LPVOID>(hTrayHwnd), 0, nullptr);
        if (!h) InterlockedExchange(&g_guiRunning, 0);
        return h;
    }

} // namespace MicSwitchGui

// ─── Device selection data ────────────────────────────────────────────────────
// All functions below acquire g_stateLock for writes to shared slot data.

void LoadPriorityList() {
    WCHAR tmpIds[MAX_DEVICE_SLOTS][512] = {};
    int count = 0;
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
        WCHAR key[32];
        swprintf_s(key, L"priority%d", i + 1);
        Wh_GetStringValue(key, tmpIds[i], 512);
        if (tmpIds[i][0]) count = i + 1;
    }
    EnterCriticalSection(&g_stateLock);
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++)
        lstrcpynW(g_priorityDevIds[i], tmpIds[i], 512);
    g_priorityCount = count;
    LeaveCriticalSection(&g_stateLock);
}

void LoadDeviceSelections() {
    // Read from Windhawk storage outside the lock (Wh_GetStringValue is thread-safe).
    WCHAR tmpIds[MAX_DEVICE_SLOTS][512]   = {};
    WCHAR tmpNames[MAX_DEVICE_SLOTS][256] = {};

    WCHAR keyId[32], keyName[32];
    for (int i = 0; i < MAX_DEVICE_SLOTS; i++) {
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


static int ReadDeviceCountSetting() {
    WCHAR buf[8] = {};
    Wh_GetStringValue(L"deviceCount", buf, 8);
    int n = 0;
    for (const WCHAR* p = buf; *p >= L'0' && *p <= L'9'; p++)
        n = n * 10 + (*p - L'0');
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

    WCHAR swapModeBuf[32] = {};
    Wh_GetStringValue(L"swapMode", swapModeBuf, 32);
    LONG newScroll = (wcscmp(swapModeBuf, L"scroll_to_swap") == 0) ? 1 : 0;

    EnterCriticalSection(&g_stateLock);
    g_deviceSlotCount = newCount;
    LeaveCriticalSection(&g_stateLock);

    InterlockedExchange(&g_scrollToSwap, newScroll);

    WCHAR iconKey[16], pathKey[32];
    for (int i = 0; i < newCount; i++) {
        swprintf_s(iconKey, L"icon%d", i + 1);
        WCHAR iconVal[32] = {};
        Wh_GetStringValue(iconKey, iconVal, 32);

        if (wcscmp(iconVal, L"custom") == 0) {
            swprintf_s(pathKey, L"icon%d_custom_path", i + 1);
            WCHAR customPath[MAX_PATH] = {};
            Wh_GetStringValue(pathKey, customPath, MAX_PATH);
            if (customPath[0]) {
                g_iconDev[i] = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            }
            if (!g_iconDev[i]) {
                ExtractIconExW(g_ddoresDllPath, 4, nullptr, &g_iconDev[i], 1);
            }
        } else {
            ExtractIconExW(g_ddoresDllPath, GetIconIndex(iconVal), nullptr, &g_iconDev[i], 1);
        }

        wcscpy_s(g_lastIconSetting[i], 32, iconVal);

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
    nii.guidItem = MICSWITCH_TRAY_GUID;
    RECT newRect = {};
    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &newRect)) &&
        newRect.right > newRect.left) {
        g_trayIconRect = newRect;
        KillTimer(hwnd, TRAY_RECT_INIT_TIMER);   // rect valid — stop retrying
    } else {
        // Explorer hasn't assigned screen coordinates to the icon yet.
        // Shell_NotifyIconGetRect is a query — it won't force Explorer to
        // finish its layout pass. Send NIM_MODIFY to give Explorer the
        // necessary nudge to compute the icon's position.
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd     = hwnd;
        nid.uID      = TRAY_ICON_ID;
        nid.uFlags   = NIF_SHOWTIP | NIF_GUID;
        nid.guidItem = MICSWITCH_TRAY_GUID;
        Shell_NotifyIconW(NIM_MODIFY, &nid);
        SetTimer(hwnd, TRAY_RECT_INIT_TIMER, 200, nullptr);
    }
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
                 : pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDevice);
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
    if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDefault))) {
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

class MicDeviceNotifier : public IMMNotificationClient {
    volatile LONG m_ref;
public:
    MicDeviceNotifier() : m_ref(1) {}
    virtual ~MicDeviceNotifier() = default;

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
        if (flow == eCapture && role == eMultimedia) {
            HWND hwnd = (HWND)InterlockedCompareExchangePointer(
                (volatile PVOID*)&g_trayHwnd, nullptr, nullptr);
            if (hwnd) {
                // Rebind the IAudioEndpointVolumeCallback before refreshing the
                // tooltip — the live dB readout depends on tracking the *current*
                // default endpoint, not the previous one.
                PostMessageW(hwnd, WM_REBIND_VOLUME_CALLBACK, 0, 0);
                PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
        HWND hwnd = (HWND)InterlockedCompareExchangePointer(
            (volatile PVOID*)&g_trayHwnd, nullptr, nullptr);
        if (hwnd) {
            PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
            if (pwstrDeviceId && pwstrDeviceId[0]) {
                size_t idLen = wcslen(pwstrDeviceId) + 1;
                WCHAR* idCopy = new WCHAR[idLen];
                wcscpy_s(idCopy, idLen, pwstrDeviceId);
                if (!PostMessageW(hwnd, WM_PRIORITY_DEVICE_ACTIVE, 0, (LPARAM)idCopy))
                    delete[] idCopy;
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        HWND hwnd = (HWND)InterlockedCompareExchangePointer(
            (volatile PVOID*)&g_trayHwnd, nullptr, nullptr);
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD newState) override {
        HWND hwnd = (HWND)InterlockedCompareExchangePointer(
            (volatile PVOID*)&g_trayHwnd, nullptr, nullptr);
        if (hwnd) {
            PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
            if (newState == DEVICE_STATE_ACTIVE && pwstrDeviceId && pwstrDeviceId[0]) {
                size_t idLen = wcslen(pwstrDeviceId) + 1;
                WCHAR* idCopy = new WCHAR[idLen];
                wcscpy_s(idCopy, idLen, pwstrDeviceId);
                if (!PostMessageW(hwnd, WM_PRIORITY_DEVICE_ACTIVE, 0, (LPARAM)idCopy))
                    delete[] idCopy;
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
        LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};

// IAudioEndpointVolumeCallback — fires on gain / mute changes from ANY source
// (volume keys, slider popup, other apps). Posts WM_UPDATE_TRAY_STATE to refresh
// the tooltip's live dB readout. Self-triggered changes carry kVolumeChangeCtx —
// we still refresh on those so the tooltip reflects the new value.
class VolNotifier : public IAudioEndpointVolumeCallback {
    volatile LONG m_ref;
public:
    VolNotifier() : m_ref(1) {}
    virtual ~VolNotifier() = default;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppv = static_cast<IAudioEndpointVolumeCallback*>(this);
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

    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        HWND hwnd = (HWND)InterlockedCompareExchangePointer(
            (volatile PVOID*)&g_trayHwnd, nullptr, nullptr);
        if (hwnd) PostMessageW(hwnd, WM_UPDATE_TRAY_STATE, 0, 0);
        return S_OK;
    }
};

// Acquire IAudioEndpointVolume on the current default capture endpoint and register
// VolNotifier. Idempotent — caller is responsible for first releasing the previous
// binding via UnbindEndpointVolume(). Tray-thread only.
static void BindEndpointVolume() {
    if (!g_notifEnum) return;
    IMMDevice* pDev = nullptr;
    if (FAILED(g_notifEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDev)) || !pDev)
        return;
    IAudioEndpointVolume* pVol = nullptr;
    if (SUCCEEDED(pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                 nullptr, (void**)&pVol)) && pVol) {
        g_pEndpointVol = pVol;
        if (!g_pVolNotifier) g_pVolNotifier = new VolNotifier();
        g_pEndpointVol->RegisterControlChangeNotify(g_pVolNotifier);
    }
    pDev->Release();
}

static void UnbindEndpointVolume() {
    if (g_pEndpointVol && g_pVolNotifier) {
        g_pEndpointVol->UnregisterControlChangeNotify(g_pVolNotifier);
    }
    if (g_pVolNotifier) { g_pVolNotifier->Release(); g_pVolNotifier = nullptr; }
    if (g_pEndpointVol) { g_pEndpointVol->Release(); g_pEndpointVol = nullptr; }
}

// Scroll-to-swap uses Raw Input (RegisterRawInputDevices + WM_INPUT in TrayWndProc)
// instead of WH_MOUSE_LL. Raw Input is delivered outside the hook chain, so no
// other WH_MOUSE_LL hook (e.g., Monitor Sleep Button) can block it.

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

static WCHAR s_lastDev[256]  = {};
static HICON s_lastIcon      = nullptr;
static bool  s_lastMuted     = false;
static int   s_lastVolume    = -1;

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
    int   currentVolume   = -1;     // -1 sentinel = query failed; omit from tooltip

    // Fast path: read scalar + dB from the cached endpoint pointer without re-doing
    // CoCreateInstance + Activate. Falls back to the slow per-call path if the
    // tray thread hasn't bound an endpoint yet (e.g., startup race).
    if (g_pEndpointVol) {
        float scalar = 0.0f;
        if (SUCCEEDED(g_pEndpointVol->GetMasterVolumeLevelScalar(&scalar))) {
            currentVolume = (int)(scalar * 100.0f + 0.5f);
            if (currentVolume < 0)   currentVolume = 0;
            if (currentVolume > 100) currentVolume = 100;
        }
    }

    // Slow path: still needed for the device name + ID. Always run this block.
    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice* pDev = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDev))) {
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
            // Fallback: if the cached endpoint wasn't bound yet, query volume inline.
            if (currentVolume < 0) {
                IAudioEndpointVolume* pVol = nullptr;
                if (SUCCEEDED(pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                             nullptr, (void**)&pVol))) {
                    float scalar = 0.0f;
                    if (SUCCEEDED(pVol->GetMasterVolumeLevelScalar(&scalar))) {
                        currentVolume = (int)(scalar * 100.0f + 0.5f);
                        if (currentVolume < 0)   currentVolume = 0;
                        if (currentVolume > 100) currentVolume = 100;
                    }
                    pVol->Release();
                }
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

    // s_lastDev / s_lastIcon / s_lastMuted / s_lastVolume are file-scope to allow external reset.
    if (!isAdd &&
        wcscmp(currentDev, s_lastDev) == 0 &&
        currentIcon == s_lastIcon &&
        s_lastMuted == localMuted &&
        s_lastVolume == currentVolume)
    {
        RefreshTrayIconRect();
        return;
    }
    lstrcpyW(s_lastDev, currentDev);
    s_lastIcon   = currentIcon;
    s_lastMuted  = localMuted;
    s_lastVolume = currentVolume;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd             = hWnd;
    nid.uID              = TRAY_ICON_ID;
    // NIF_SHOWTIP is required under NOTIFYICON_VERSION_4 — without it the shell
    // assumes the app draws its own tooltip and the standard one never appears on hover.
    nid.uFlags           = NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP | NIF_ICON | NIF_GUID;
    nid.guidItem         = MICSWITCH_TRAY_GUID;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    if (configuredCount < 2)
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"MicSwitch: Right-click to configure");
    else if (localMuted && currentVolume >= 0)
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Mic: %.82s (%d%%, Muted)",
                   currentDev, currentVolume);
    else if (localMuted)
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Mic: %.102s (Muted)", currentDev);
    else if (currentVolume >= 0)
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Mic: %.92s (%d%%)",
                   currentDev, currentVolume);
    else
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Mic: %.112s", currentDev);

    HICON hOverlay = localMuted ? CreateMutedOverlayIcon(currentIcon) : nullptr;
    nid.hIcon = hOverlay ? hOverlay : currentIcon;
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &nid);
    if (hOverlay) { DestroyIcon(hOverlay); }

    if (isAdd) {
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd      = hWnd;
        nidVer.uID       = TRAY_ICON_ID;
        nidVer.uFlags    = NIF_GUID;
        nidVer.guidItem  = MICSWITCH_TRAY_GUID;
        nidVer.uVersion  = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    }

    RefreshTrayIconRect();
}

// ─── Volume helpers ───────────────────────────────────────────────────────────

static int GetCurrentVolumePct() {
    int result = -1;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != S_FALSE) return result;
    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice* pDev = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDev))) {
            IAudioEndpointVolume* pVol = nullptr;
            if (SUCCEEDED(pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                         nullptr, (void**)&pVol))) {
                float scalar = 0.0f;
                if (SUCCEEDED(pVol->GetMasterVolumeLevelScalar(&scalar))) {
                    result = (int)(scalar * 100.0f + 0.5f);
                    if (result < 0)   result = 0;
                    if (result > 100) result = 100;
                }
                pVol->Release();
            }
            pDev->Release();
        }
        pEnum->Release();
    }
    CoUninitialize();
    return result;
}

static void SetCurrentDeviceVolume(float scalar) {
    scalar = std::max(0.0f, std::min(1.0f, scalar));
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != S_FALSE) return;
    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice* pDev = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDev))) {
            IAudioEndpointVolume* pVol = nullptr;
            if (SUCCEEDED(pDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                         nullptr, (void**)&pVol))) {
                pVol->SetMasterVolumeLevelScalar(scalar, nullptr);
                pVol->Release();
            }
            pDev->Release();
        }
        pEnum->Release();
    }
    CoUninitialize();
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
    if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDefault))) {
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

    int startSlot = (currentSlot >= 0) ? currentSlot : (localCount - 1);
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

static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static void ApplyContextMenuTheme(HWND hWnd, bool dark) {
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (!ux) return;
    using Fn135 = int(WINAPI*)(int);
    using Fn133 = bool(WINAPI*)(HWND, bool);
    using Fn136 = void(WINAPI*)();
    if (auto f = (Fn135)GetProcAddress(ux, MAKEINTRESOURCEA(135))) f(dark ? 2 : 0);
    if (auto f = (Fn133)GetProcAddress(ux, MAKEINTRESOURCEA(133))) f(hWnd, dark);
    if (auto f = (Fn136)GetProcAddress(ux, MAKEINTRESOURCEA(136))) f();
}

void BuildAndShowContextMenu(HWND hWnd) {
    HMENU hMenu = CreatePopupMenu();

    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_SETTINGS,  L"Mod Settings...");
    AppendMenuW(hMenu, MF_STRING, MENU_SOUND_SETTINGS, L"Input Settings...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open WindHawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    POINT pt; GetCursorPos(&pt);
    bool dark = IsSystemDarkMode();
    ApplyContextMenuTheme(hWnd, dark);
    SetForegroundWindow(hWnd);
    int cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    if (cmd == MENU_OPEN_SETTINGS) {
        HANDLE h = MicSwitchGui::LaunchDashboard(hWnd);
        if (h) {
            if (g_guiThread) CloseHandle(g_guiThread);
            g_guiThread = h;
        }
    } else if (cmd == MENU_SOUND_SETTINGS) {
        ShellExecuteW(nullptr, L"open", L"control.exe", L"mmsys.cpl,,1", nullptr, SW_SHOWNORMAL);
    } else if (cmd == MENU_OPEN_WINDHAWK) {
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpFile = g_windhawkPath;
        sei.nShow  = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    }
}

// ─── Volume slider popup ──────────────────────────────────────────────────────

namespace VolumePopup {

static constexpr WCHAR    kClass[]    = L"MicSwitchVolumePopup";
static constexpr WCHAR    kLabel[]    = L"Mic Level";
static constexpr int      kW         = 240;
static constexpr int      kH         = 72;
static constexpr int      kPadX      = 16;   // left/right padding
static constexpr int      kTitleY    = 12;   // title row top Y
static constexpr int      kTitleH    = 18;   // title row height
static constexpr int      kTrackY    = 52;   // track center Y
static constexpr int      kTrackH    = 6;    // track height in px
static constexpr int      kThumbR    = 9;    // thumb radius (normal)
static constexpr int      kThumbRHov = 10;   // thumb radius (hover / drag)
static constexpr UINT     kVolTimerId  = 102;  // throttled audio commit
static constexpr COLORREF kBg        = RGB(28,  28,  28 );
static constexpr COLORREF kBorder    = RGB(50,  50,  50 );
static constexpr COLORREF kTrackBg   = RGB(58,  58,  58 );
static constexpr COLORREF kTrackFill = RGB(0,   120, 215);
static constexpr COLORREF kThumbCol  = RGB(255, 255, 255);
static constexpr COLORREF kTextPri   = RGB(235, 235, 235);
static constexpr COLORREF kTextSec   = RGB(130, 130, 130);

static HWND   g_hwnd     = nullptr;
static bool   g_volumeDirty = false;
static HWND   g_trayWnd  = nullptr;
static int    g_value    = 0;
static bool   g_dragging = false;
static bool   g_hover    = false;
static HFONT  g_font     = nullptr;

static int ValueToThumbX() {
    return kPadX + (g_value * (kW - 2 * kPadX)) / 100;
}

static int XToValue(int x) {
    int v = ((x - kPadX) * 100 + (kW - 2 * kPadX) / 2) / (kW - 2 * kPadX);
    return v < 0 ? 0 : (v > 100 ? 100 : v);
}

static bool IsOverThumb(int mx, int my) {
    int dx = mx - ValueToThumbX(), dy = my - kTrackY;
    int r  = kThumbRHov + 4;
    return dx * dx + dy * dy <= r * r;
}

static void ApplyVolume() {
    float scalar = g_value / 100.0f;
    if (g_pEndpointVol) {
        g_pEndpointVol->SetMasterVolumeLevelScalar(scalar, &kVolumeChangeCtx);
    } else {
        SetCurrentDeviceVolume(scalar);
    }
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE: {
        int vol    = (int)(INT_PTR)((CREATESTRUCTW*)lParam)->lpCreateParams;
        g_value    = vol < 0 ? 0 : (vol > 100 ? 100 : vol);
        g_dragging = false;
        g_hover    = false;
        g_font     = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        // Win11 rounded corners (DWMWA_WINDOW_CORNER_PREFERENCE=33, DWMWCP_ROUNDSMALL=3).
        // GetModuleHandleW — dwmapi is already loaded; LoadLibraryW would leak a refcount.
        {
            using PFN = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
            HMODULE hDwm = GetModuleHandleW(L"dwmapi.dll");
            if (hDwm) {
                if (auto pfn = (PFN)GetProcAddress(hDwm, "DwmSetWindowAttribute")) {
                    UINT pref = 3;
                    pfn(hWnd, 33, &pref, sizeof(pref));
                }
            }
        }
        SetTimer(hWnd, kVolTimerId, 16, nullptr);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;   // suppress default erase — WM_PAINT covers the full client rect

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc  = BeginPaint(hWnd, &ps);

        // Double-buffer to eliminate flicker
        HDC     hMem    = CreateCompatibleDC(hdc);
        HBITMAP hBmp    = CreateCompatibleBitmap(hdc, kW, kH);
        auto    hOldBmp = (HBITMAP)SelectObject(hMem, hBmp);

        // ── Background ────────────────────────────────────────────────
        RECT rcAll = {0, 0, kW, kH};
        HBRUSH hbBg = CreateSolidBrush(kBg);
        FillRect(hMem, &rcAll, hbBg);
        DeleteObject(hbBg);

        // Border (1 px)
        HBRUSH hbBdr = CreateSolidBrush(kBorder);
        FrameRect(hMem, &rcAll, hbBdr);
        DeleteObject(hbBdr);

        // ── Title row ─────────────────────────────────────────────────
        HFONT hOldFont = (HFONT)SelectObject(hMem, g_font ? g_font
                                                           : GetStockObject(DEFAULT_GUI_FONT));
        SetBkMode(hMem, TRANSPARENT);

        RECT rcL = {kPadX, kTitleY, kW - kPadX, kTitleY + kTitleH};
        SetTextColor(hMem, kTextSec);
        DrawTextW(hMem, kLabel, -1, &rcL,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        WCHAR valBuf[8];
        swprintf_s(valBuf, ARRAYSIZE(valBuf), L"%d%%", g_value);
        RECT rcR = {kPadX, kTitleY, kW - kPadX, kTitleY + kTitleH};
        SetTextColor(hMem, kTextPri);
        DrawTextW(hMem, valBuf, -1, &rcR,
                  DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        SelectObject(hMem, hOldFont);

        // ── Track ─────────────────────────────────────────────────────
        int tLeft  = kPadX;
        int tRight = kW - kPadX;
        int tTop   = kTrackY - kTrackH / 2;
        int tBot   = kTrackY + kTrackH / 2;
        int tX     = ValueToThumbX();

        SelectObject(hMem, GetStockObject(NULL_PEN));

        // Unfilled portion
        HBRUSH hbTrack = CreateSolidBrush(kTrackBg);
        SelectObject(hMem, hbTrack);
        RoundRect(hMem, tLeft, tTop, tRight, tBot, kTrackH, kTrackH);
        DeleteObject(hbTrack);

        // Accent-filled (left of thumb)
        if (tX > tLeft) {
            HBRUSH hbFill = CreateSolidBrush(kTrackFill);
            SelectObject(hMem, hbFill);
            RoundRect(hMem, tLeft, tTop, tX, tBot, kTrackH, kTrackH);
            DeleteObject(hbFill);
        }

        // Thumb
        int r = (g_dragging || g_hover) ? kThumbRHov : kThumbR;
        HBRUSH hbThumb = CreateSolidBrush(kThumbCol);
        SelectObject(hMem, hbThumb);
        Ellipse(hMem, tX - r, kTrackY - r, tX + r, kTrackY + r);
        DeleteObject(hbThumb);

        // ── Blit ──────────────────────────────────────────────────────
        BitBlt(hdc, 0, 0, kW, kH, hMem, 0, 0, SRCCOPY);
        SelectObject(hMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hMem);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_SETCURSOR: {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        bool overTrack = (pt.y >= kTrackY - kThumbRHov * 2 &&
                          pt.y <= kTrackY + kThumbRHov * 2 &&
                          pt.x >= kPadX && pt.x <= kW - kPadX);
        SetCursor(LoadCursor(nullptr, overTrack ? IDC_HAND : IDC_ARROW));
        return TRUE;
    }

    case WM_LBUTTONDOWN: {
        int mx = (short)LOWORD(lParam), my = (short)HIWORD(lParam);
        bool onTrack = (my >= kTrackY - kThumbRHov * 2 &&
                        my <= kTrackY + kThumbRHov * 2 &&
                        mx >= kPadX && mx <= kW - kPadX);
        if (onTrack || IsOverThumb(mx, my)) {
            g_value    = XToValue(mx);
            g_dragging = true;
            g_hover    = true;
            SetCapture(hWnd);
            ApplyVolume();
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        int mx = (short)LOWORD(lParam), my = (short)HIWORD(lParam);
        // Re-register for WM_MOUSELEAVE each time (TME_LEAVE is one-shot)
        TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
        TrackMouseEvent(&tme);
        if (g_dragging) {
            int newVal = XToValue(mx);
            if (newVal != g_value) {
                g_value = newVal;
                g_volumeDirty = true;   // throttled — kVolTimerId commits to audio
                InvalidateRect(hWnd, nullptr, FALSE);
            }
        } else {
            bool wasHover = g_hover;
            g_hover = IsOverThumb(mx, my);
            if (g_hover != wasHover)
                InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONUP:
        if (g_dragging) {
            g_dragging = false;
            ReleaseCapture();
            g_value       = XToValue((short)LOWORD(lParam));
            g_volumeDirty = false;
            ApplyVolume();          // force-commit final drag position
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;

    case WM_MOUSELEAVE:
        if (!g_dragging && g_hover) {
            g_hover = false;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;

    case WM_MOUSEWHEEL: {
        int step = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 3 : -3;
        int newVal = std::max(0, std::min(100, g_value + step));
        if (newVal != g_value) {
            g_value = newVal;
            ApplyVolume();
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
            DestroyWindow(hWnd);
        return 0;

    case WM_TIMER:
        if (wParam == kVolTimerId) {
            if (g_volumeDirty) { g_volumeDirty = false; ApplyVolume(); }
        }
        return 0;

    case WM_KEYDOWN: {
        int v = g_value;
        switch (wParam) {
        case VK_LEFT:   v = std::max(0,   v - 1); break;
        case VK_RIGHT:  v = std::min(100, v + 1); break;
        case VK_HOME:   v = 0;   break;
        case VK_END:    v = 100; break;
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            return 0;
        default: return 0;
        }
        if (v != g_value) {
            g_value = v;
            ApplyVolume();
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, kVolTimerId);
        if (g_volumeDirty) { g_volumeDirty = false; ApplyVolume(); }
        if (g_font) { DeleteObject(g_font); g_font = nullptr; }
        g_hwnd        = nullptr;
        g_trayWnd     = nullptr;
        g_dragging    = false;
        g_hover       = false;
        return 0;

    } // switch
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void RegisterClass() {
    WNDCLASSEXW wc   = {sizeof(wc)};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = g_hInstance;
    wc.lpszClassName = kClass;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);
}

static void UnregisterClass() {
    ::UnregisterClassW(kClass, g_hInstance);
}

static void Show(HWND hTrayWnd, int volPct) {
    if (g_hwnd) { SetForegroundWindow(g_hwnd); return; }

    int x = (g_trayIconRect.left + g_trayIconRect.right) / 2 - kW / 2;
    int y =  g_trayIconRect.top - kH - 8;

    POINT pt    = { (g_trayIconRect.left + g_trayIconRect.right) / 2, g_trayIconRect.top };
    HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hm, &mi)) {
        RECT& wa = mi.rcWork;
        if (x < wa.left)       x = wa.left;
        if (x + kW > wa.right) x = wa.right - kW;
        if (y < wa.top)        y = g_trayIconRect.bottom + 8;
    }

    g_trayWnd = hTrayWnd;
    g_hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kClass, nullptr,
        WS_POPUP,
        x, y, kW, kH,
        hTrayWnd, nullptr, g_hInstance, (LPVOID)(INT_PTR)volPct);

    if (g_hwnd) {
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        SetForegroundWindow(g_hwnd);
    }
}

} // namespace VolumePopup

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
    if (g_workerThread) {
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc,
                                  reinterpret_cast<LPVOID>(static_cast<LONG_PTR>(direction)),
                                  0, nullptr);
    // If CreateThread fails the worker never resets the lock — do it here.
    if (!g_workerThread) InterlockedExchange(&g_isProcessingClick, 0);
}

// ─── Priority device routing ──────────────────────────────────────────────────

static void HandlePriorityDeviceConnected(HWND hWnd, const WCHAR* devId) {
    // Snapshot all shared state under the lock before doing any work.
    EnterCriticalSection(&g_stateLock);
    int priorityCount = g_priorityCount;
    WCHAR localPrio[MAX_DEVICE_SLOTS][512] = {};
    for (int i = 0; i < priorityCount; i++)
        lstrcpynW(localPrio[i], g_priorityDevIds[i], 512);
    int slotCount = g_deviceSlotCount;
    WCHAR localIds[MAX_DEVICE_SLOTS][512] = {};
    for (int i = 0; i < slotCount; i++)
        lstrcpynW(localIds[i], g_cachedDevId[i], 512);
    LeaveCriticalSection(&g_stateLock);

    if (!devId || !devId[0] || priorityCount == 0) return;

    // Find rank of the newly connected device in the priority list.
    int newRank = -1;
    for (int i = 0; i < priorityCount; i++) {
        if (localPrio[i][0] && wcscmp(localPrio[i], devId) == 0) {
            newRank = i; break;
        }
    }
    if (newRank < 0) return;  // not in the priority list

    // Find the slot currently occupied by the lowest-priority (highest index) device.
    int worstSlot = -1, worstRank = -1;
    for (int s = 0; s < slotCount; s++) {
        int rank = INT_MAX;
        for (int p = 0; p < priorityCount; p++) {
            if (localPrio[p][0] && wcscmp(localPrio[p], localIds[s]) == 0) {
                rank = p; break;
            }
        }
        if (rank > worstRank) { worstRank = rank; worstSlot = s; }
    }

    if (worstSlot < 0 || worstRank <= newRank) return;  // new device is not higher priority

    // Replace the worst slot with the new priority device.
    WCHAR kId[32], kNm[32];
    swprintf_s(kId, L"Device%dId",   worstSlot + 1);
    swprintf_s(kNm, L"Device%dName", worstSlot + 1);
    Wh_SetStringValue(kId, devId);
    Wh_SetStringValue(kNm, L"");

    EnterCriticalSection(&g_stateLock);
    lstrcpynW(g_cachedDevId[worstSlot],   devId, 512);
    g_cachedDevName[worstSlot][0] = L'\0';
    LeaveCriticalSection(&g_stateLock);

    PostMessageW(hWnd, WM_RELOAD_ALL, 0, 0);
}

// ─── Tray window ──────────────────────────────────────────────────────────────

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CALLBACK) {
        UINT event = LOWORD(lParam);  // NOTIFYICON_VERSION_4: event in LOWORD(lParam)

        if (event == WM_MOUSEMOVE) {
            POINT pt = { (short)LOWORD(wParam), (short)HIWORD(wParam) };
            if (!PtInRect(&g_trayIconRect, pt)) {
                RefreshTrayIconRect();
            }
        } else if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU) {
            // Right-click — opens settings menu.
            // v4 sends WM_CONTEXTMENU; pre-v4 sends WM_RBUTTONUP.
            BuildAndShowContextMenu(hWnd);

        } else if ((event == WM_LBUTTONUP || event == NIN_SELECT ||
                    event == NIN_KEYSELECT) &&
                   InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 0) {
            // Click mode: debounced forward cycle.
            // v4 sends NIN_SELECT (or NIN_KEYSELECT for keyboard); pre-v4 sends WM_LBUTTONUP.
            if (InterlockedExchange(&g_isProcessingClick, 1) == 0) {
                DWORD now = GetTickCount();
                if (now - g_lastClickTime > CLICK_DEBOUNCE_MS) {
                    g_lastClickTime = now;
                    SpawnCycleThread(1);
                } else {
                    InterlockedExchange(&g_isProcessingClick, 0);
                }
            }

        } else if ((event == WM_LBUTTONUP || event == NIN_SELECT ||
                    event == NIN_KEYSELECT) &&
                   InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 1) {
            // Scroll mode: left-click toggles mute.
            // Same debounce as click mode: WM_LBUTTONUP and NIN_SELECT are BOTH
            // delivered for the same physical click under NOTIFYICON_VERSION_4.
            // Without debouncing, mute is toggled on then immediately off (net nothing).
            if (InterlockedExchange(&g_isProcessingClick, 1) == 0) {
                DWORD now = GetTickCount();
                if (now - g_lastClickTime > CLICK_DEBOUNCE_MS) {
                    g_lastClickTime = now;
                    ToggleMuteCurrentDevice();
                    PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
                }
                InterlockedExchange(&g_isProcessingClick, 0);
            }
        } else if (event == WM_MBUTTONUP) {
            VolumePopup::Show(hWnd, GetCurrentVolumePct());
        }

    } else if (msg == WM_INPUT) {
        UINT sz = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &sz, sizeof(RAWINPUTHEADER));
        if (sz > 0) {
            std::vector<BYTE> buf(sz);
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buf.data(), &sz,
                                sizeof(RAWINPUTHEADER)) == sz) {
                auto* raw = reinterpret_cast<RAWINPUT*>(buf.data());
                if (raw->header.dwType == RIM_TYPEMOUSE &&
                    (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)) {
                    POINT pt; GetCursorPos(&pt);
                    if (PtInRect(&g_trayIconRect, pt)) {
                        short delta = (short)raw->data.mouse.usButtonData;
                        int dir = (delta > 0) ? 1 : -1;
                        if (InterlockedCompareExchange(&g_scrollToSwap, 0, 0) == 1) {
                            PostMessageW(hWnd, WM_TRAY_SCROLL,
                                         (WPARAM)(LONG_PTR)dir, 0);
                        } else if (g_pEndpointVol) {
                            float scalar = 0.0f;
                            if (SUCCEEDED(g_pEndpointVol->GetMasterVolumeLevelScalar(&scalar))) {
                                scalar = std::max(0.0f, std::min(1.0f, scalar + dir * 0.02f));
                                g_pEndpointVol->SetMasterVolumeLevelScalar(scalar, nullptr);
                                PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
                            }
                        }
                    }
                }
            }
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);

    } else if (msg == WM_TRAY_SCROLL) {
        // Posted by Raw Input handler when the user scrolls over the icon
        DWORD now = GetTickCount();
        if (now - g_lastScrollTime > SCROLL_DEBOUNCE_MS &&
            InterlockedExchange(&g_isProcessingClick, 1) == 0) {
            g_lastScrollTime = now;
            int direction = static_cast<int>(static_cast<LONG_PTR>(wParam));
            SpawnCycleThread(direction);
        }

    } else if (msg == WM_TIMER && wParam == TRAY_RECT_INIT_TIMER) {
        // Fired by RefreshTrayIconRect when Shell_NotifyIconGetRect returned empty.
        // By now Explorer has had time to complete its paint cycle and assign real
        // screen coordinates to the icon. RefreshTrayIconRect kills the timer on
        // success, or re-arms it for another 200ms if Explorer is still not ready.
        RefreshTrayIconRect();

    } else if (msg == WM_UPDATE_TRAY_STATE) {
        UpdateTrayTip(hWnd, FALSE);

    } else if (msg == WM_UPDATE_HOOK_STATE) {
        RAWINPUTDEVICE rid = { 1, 2, RIDEV_INPUTSINK, hWnd };
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

    } else if (msg == WM_RELOAD_ICONS) {
        // Run icon reload on tray thread to eliminate cross-thread handle race.
        WCHAR prev[MAX_DEVICE_SLOTS][32] = {};
        for (int i = 0; i < MAX_DEVICE_SLOTS; i++)
            wcscpy_s(prev[i], 32, g_lastIconSetting[i]);

        LoadUserIconsAndSettings();
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);

        DWORD pickerSlots = 0;
        for (int i = 0; i < g_deviceSlotCount; i++) {
            WCHAR key[16];
            swprintf_s(key, L"icon%d", i + 1);
            WCHAR icoVal[32] = {};
            Wh_GetStringValue(key, icoVal, 32);
            BOOL isCustom  = (wcscmp(icoVal,  L"custom") == 0);
            BOOL wasCustom = (wcscmp(prev[i], L"custom") == 0);
            if (isCustom && !wasCustom)
                pickerSlots |= (1u << i);
        }
        if (pickerSlots)
            PostMessageW(hWnd, WM_SHOW_FILE_PICKER, 0, (LPARAM)pickerSlots);

    } else if (msg == WM_SHOW_FILE_PICKER) {
        DWORD slots = (DWORD)lParam;
        for (int slot = 1; slots; slot++, slots >>= 1) {
            if (!(slots & 1)) continue;
            WCHAR path[MAX_PATH] = {};
            WCHAR title[64];
            swprintf_s(title, L"Select Icon for Device %d", slot);
            OPENFILENAMEW ofn = {sizeof(ofn)};
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFile = path;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrTitle = title;
            if (GetOpenFileNameW(&ofn)) {
                WCHAR customPathKey[32];
                swprintf_s(customPathKey, L"icon%d_custom_path", slot);
                Wh_SetStringValue(customPathKey, path);
                LoadUserIconsAndSettings();
                PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
            }
        }

    } else if (msg == WM_PRIORITY_DEVICE_ACTIVE) {
        WCHAR* devId = reinterpret_cast<WCHAR*>(lParam);
        if (devId) {
            HandlePriorityDeviceConnected(hWnd, devId);
            delete[] devId;
        }

    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        UpdateTrayTip(hWnd, TRUE);  // re-add icon after explorer restart
        RefreshTrayIconRect();      // re-acquire icon screen rect after explorer restart

    } else if (msg == WM_REBIND_VOLUME_CALLBACK) {
        // Default device changed — re-bind to new endpoint.
        UnbindEndpointVolume();
        BindEndpointVolume();

    } else if (msg == WM_RELOAD_ALL) {
        // Full reload triggered by the settings dashboard after Save and Apply.
        // Picks up new device assignments, icon choices, device count, swap mode, and priority list.
        LoadDeviceSelections();
        LoadPriorityList();
        LoadUserIconsAndSettings();
        s_lastIcon = nullptr;  // icon handles were recreated; force UpdateTrayTip to refresh
        UpdateTrayTip(hWnd, FALSE);
        PostMessageW(hWnd, WM_UPDATE_HOOK_STATE, 0, 0);

    } else if (msg == WM_CLOSE) {
        // Orderly shutdown: remove tray icon, then destroy window.
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd     = hWnd;
        nid.uID      = TRAY_ICON_ID;
        nid.uFlags   = NIF_GUID;
        nid.guidItem = MICSWITCH_TRAY_GUID;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        if (VolumePopup::g_hwnd) DestroyWindow(VolumePopup::g_hwnd);
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
    wc.lpszClassName = L"MicSwitchWindowClass";
    RegisterClassW(&wc);
    VolumePopup::RegisterClass();

    // WS_EX_TOOLWINDOW: hidden popup window that Shell_NotifyIconW can track properly.
    // We used HWND_MESSAGE originally, but message-only windows can lose their
    // tray icon after reboot because the shell cannot reliably associate the icon with
    // the window across session boundaries.
    g_trayHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicSwitch",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hInstance, nullptr
    );
    if (!g_trayHwnd) {
        CoUninitialize();
        return 1;
    }

    // Register for instant device-change notifications.
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&g_notifEnum))) {
        g_deviceNotifier = new MicDeviceNotifier();
        g_notifEnum->RegisterEndpointNotificationCallback(g_deviceNotifier);
        BindEndpointVolume();
    }
    // Register Raw Input for scroll over the tray icon (bypasses WH_MOUSE_LL hook chain).
    // Always active: scroll-to-swap mode cycles devices; click mode adjusts volume.
    {
        RAWINPUTDEVICE rid = { 1, 2, RIDEV_INPUTSINK, g_trayHwnd };
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    UpdateTrayTip(g_trayHwnd, TRUE);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { DispatchMessageW(&msg); }

    // Unbind volume callback BEFORE unregistering the device-enumerator notifications,
    // so OnNotify cannot fire against a half-torn-down object.
    UnbindEndpointVolume();
    // Unregister notifications before releasing the enumerator.
    if (g_notifEnum && g_deviceNotifier) {
        g_notifEnum->UnregisterEndpointNotificationCallback(g_deviceNotifier);
        g_deviceNotifier->Release(); g_deviceNotifier = nullptr;
        g_notifEnum->Release();      g_notifEnum      = nullptr;
    }
    // Unregister Raw Input (harmless if not registered).
    {
        RAWINPUTDEVICE rid = { 1, 2, RIDEV_REMOVE, nullptr };
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }
    VolumePopup::UnregisterClass();
    CoUninitialize();
    return 0;
}

// ─── Mod lifecycle ────────────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"MicSwitch Mod Init");
    InitializeCriticalSection(&g_stateLock);

    // Enable dark mode app-wide via uxtheme ordinal 135 (SetPreferredAppMode), with a
    // fall-back to ordinal 132 (AllowDarkModeForApp) on Win10 builds pre-1903. Graceful
    // no-op when neither ordinal exists. Affects context menus, dialogs, and STATIC
    // controls in the dashboard.
    {
        HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
        if (ux) {
            using FnSetMode = void(WINAPI*)(int);
            using FnAllow   = bool(WINAPI*)(bool);
            if (auto f = (FnSetMode)GetProcAddress(ux, MAKEINTRESOURCEA(135))) {
                f(1);  // PreferredAppMode::AllowDark
            } else if (auto f = (FnAllow)GetProcAddress(ux, MAKEINTRESOURCEA(132))) {
                f(true);
            }
        }
    }

    g_hInstance = GetModuleHandleW(nullptr);
    switch (GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath))) {
        case 0:
        case ARRAYSIZE(g_windhawkPath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;
    }

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
    LoadPriorityList();
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    RestoreMuteExternal();
    LoadDeviceSelections();
    LoadPriorityList();
    HWND hwnd = g_trayHwnd;
    if (hwnd) {
        PostMessageW(hwnd, WM_RELOAD_ICONS, 0, 0);
        PostMessageW(hwnd, WM_UPDATE_HOOK_STATE, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"MicSwitch Mod Uninit");
    RestoreMuteExternal();

    // Send WM_CLOSE so TrayWndProc can delete the tray icon
    // cleanly before DestroyWindow → PostQuitMessage exits the message loop.
    HWND hwnd = g_trayHwnd;
    if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    if (g_trayThread) {
        DWORD wr = WaitForSingleObject(g_trayThread, 3000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"MicSwitch: tray thread did not exit within 3 s — leaking handle to avoid race");
        } else {
            CloseHandle(g_trayThread);
        }
        g_trayThread = nullptr;
    }
    if (g_guiThread) {
        // Ask the dashboard's message loop to quit, then wait for it.
        // PostThreadMessageW with WM_QUIT causes GetMessageW to return 0,
        // exiting the loop regardless of IsDialogMessageW.
        DWORD guiId = GetThreadId(g_guiThread);
        if (guiId) PostThreadMessageW(guiId, WM_QUIT, 0, 0);
        DWORD wr = WaitForSingleObject(g_guiThread, 2000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"MicSwitch: GUI thread did not exit within 2 s — leaking handle to avoid race");
        } else {
            CloseHandle(g_guiThread);
        }
        g_guiThread = nullptr;
    }
    if (g_workerThread) {
        DWORD wr = WaitForSingleObject(g_workerThread, 2000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"MicSwitch: worker thread did not exit within 2 s — leaking handle to avoid race");
        } else {
            CloseHandle(g_workerThread);
        }
        g_workerThread = nullptr;
    }
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
