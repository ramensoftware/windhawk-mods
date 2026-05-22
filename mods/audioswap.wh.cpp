// ==WindhawkMod==
// @id              audioswap
// @name            AudioSwap
// @description     Tray icon to cycle between multiple preferred audio outputs. Supports up to 6 devices with click or scroll to swap.
// @version         2.0.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32 -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AudioSwap
Instantly cycle between multiple audio output devices from your system tray — no diving into Sound settings.

---

## How to Use

1. **Open Settings** — Right-click the tray icon and select **Mod Settings** to open the configuration dashboard.
2. **Assign Devices** — Use the dropdown in each slot to pick a device from the live list of active audio outputs.
3. **Choose Icons** — Select a preset icon per slot, or click **Browse...** to load a custom `.ico` file.
4. **Choose Mode** — Pick **Click to Swap** (left-click cycles devices) or **Scroll to Swap** (mouse wheel cycles; left-click mutes).
5. **Set Device Count** — Choose how many slots to cycle through (2–6).
6. Click **Save and Apply** — the tray icon updates immediately, no restart needed.

### Scroll to Swap mode extras

- **Left-click** mutes the current device. Click again to unmute. The tray tooltip shows *(Muted)* and the icon gains a red dot while active.
- **Scrolling** to a different device automatically unmutes the previous one before switching.

> The tray tooltip always shows the active device. On first run it reads *"Right-click to configure"* until at least two slots are assigned.

---

## Known Bugs

- **Scroll to Swap may not respond over elevated windows** such as Task Manager, Windhawk, or other admin-elevated applications. Switch focus away from the elevated window and scrolling will work normally.

---

## Changelog

### v2.0.0
- **New:** Native dashboard — right-click → **Mod Settings**.
- **New:** All configuration (device slots, icons, mode, count) lives in the dashboard; Windhawk's settings panel is no longer used.


### v1.4.0
- Custom .ico support — selecting "Custom Icon" in settings auto-opens a file picker; also available via right-click → "Custom Icon for Device X..."
- Custom icon paths stored internally (no separate text setting in the UI).
- Fixed: Scroll to Swap didn't work after a reboot until the icon was clicked.
- Fixed: Crash with audio devices that have very long names.
- Fixed: Adding more devices in settings didn't take effect immediately.
- Fixed: Cycling devices would sometimes skip the first slot.

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

// All settings are managed via right-click → Mod Settings (dashboard).
// No WindhawkModSettings block: Wh_GetStringSetting reads a separate
// YAML store that Wh_SetStringValue cannot write to, so schema entries
// would silently conflict with dashboard saves.

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <endpointvolume.h>

#include <propkey.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <functiondiscoverykeys_devpkey.h>
#include <commdlg.h>
#include <vector>
#include <string>

#define TRAY_ICON_ID         1
#define WM_TRAY_CALLBACK     (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define WM_TRAY_SCROLL       (WM_USER + 3)  // wParam = direction (+1 or -1)
#define WM_UPDATE_HOOK_STATE (WM_USER + 4)
#define WM_SHOW_FILE_PICKER  (WM_USER + 5)  // lParam = bitmask of slots needing pickers
#define WM_RELOAD_ICONS      (WM_USER + 6)  // reload icons on tray thread (eliminates cross-thread handle race)
#define WM_RELOAD_ALL        (WM_USER + 7)  // full reload after dashboard save
#define TRAY_RECT_INIT_TIMER 99   // one-shot retry timer for Shell_NotifyIconGetRect

#define MENU_OPEN_SETTINGS   9001
#define MENU_OPEN_WINDHAWK   9000

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

// Mouse hook
static HHOOK          g_mouseHook         = nullptr;
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

// Mode flag: set/read atomically with InterlockedExchange/InterlockedRead.
static volatile LONG  g_scrollToSwap = 0;  // 1 = scroll mode

// IMMNotificationClient registration
class AudioDeviceNotifier;
static IMMDeviceEnumerator* g_notifEnum      = nullptr;
static AudioDeviceNotifier* g_deviceNotifier = nullptr;

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

// ─── Settings dashboard ───────────────────────────────────────────────────────

namespace AudioSwapGui {

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
    static const int kCW    = 414;  // client width
    static const int kTopH  =  72;  // height of global-settings section
    static const int kSlotH =  76;  // height per slot row
    static const int kBtnH  =  52;  // button-row height
    static const int kIconSz=  28;  // icon drawn size

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
    };

    static const WCHAR* kIconKeys[] = {
        L"speaker_normal", L"speaker_square", L"speaker_modern",
        L"speaker_modern_square", L"audio_wave", L"headphones",
        L"headset_gaming", L"headphones_modern", L"headset_modern",
        L"earphones", L"custom"
    };
    static const WCHAR* kIconLabels[] = {
        L"Normal Speaker", L"Square Speaker", L"Modern Speaker",
        L"Modern Square Speaker", L"Audio Wave", L"Headphones",
        L"Gaming Headset", L"Modern Headphones", L"Modern Gaming Headset",
        L"Earphones", L"Custom Icon..."
    };
    static const int kIconCount = 11;

    // ── Helpers ───────────────────────────────────────────────────────────────

    // y-coordinate of slot i's top edge in client space.
    static int SlotY(int i) { return kTopH + i * kSlotH; }

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

    // Move buttons and resize the window to fit exactly deviceCount slots.
    static void UpdateLayout(State* s) {
        if (!s->hMainWnd) return;
        int btnY = kTopH + s->deviceCount * kSlotH + 12;
        SetWindowPos(s->hSaveBtn,   nullptr, 12,  btnY, 148, 28, SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hCancelBtn, nullptr, 168, btnY,  88, 28, SWP_NOZORDER|SWP_NOACTIVATE);
        int clientH = kTopH + s->deviceCount * kSlotH + kBtnH;
        RECT rc = {0, 0, kCW, clientH};
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
        if (s->hMainWnd) InvalidateRect(s->hMainWnd, nullptr, TRUE);
    }

    static BOOL CALLBACK ApplyFontProc(HWND child, LPARAM hf) {
        SendMessageW(child, WM_SETFONT, (WPARAM)hf, TRUE);
        return TRUE;
    }

    // ── State I/O ─────────────────────────────────────────────────────────────

    static void LoadState(State& s) {
        IMMDeviceEnumerator* pEnum = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                       CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                       (void**)&pEnum))) {
            IMMDeviceCollection* pColl = nullptr;
            if (SUCCEEDED(pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pColl))) {
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
            s.slots[i].iconKey = ikBuf[0] ? ikBuf : L"speaker_normal";
            Wh_GetStringValue(kPth, s.slots[i].customPath, MAX_PATH);
            s.slots[i].hPreviewIcon = LoadSlotPreview(s.slots[i].iconKey, s.slots[i].customPath);
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
            s->hMainWnd  = hWnd;
            s->hFont     = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
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
            // Slot i base y = kTopH + i*kSlotH.
            // y+0  to y+20 : header band (painted in WM_PAINT), "Slot N" text + icon
            // y+22 to y+46 : device combo (icon preview drawn left of combo at x=12)
            // y+50 to y+74 : icon combo | browse btn
            for (int i = 0; i < 6; i++) {
                int y = SlotY(i);

                s->slots[i].hDevCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|CBS_DROPDOWNLIST, 46, y+22, 356, 300,
                    hWnd, (HMENU)(UINT_PTR)(200+i), hInst, nullptr);
                for (int j = 0; j < (int)s->activeDevices.size(); j++) {
                    int idx = (int)SendMessageW(s->slots[i].hDevCombo, CB_ADDSTRING,
                                                0, (LPARAM)s->activeDevices[j].name.c_str());
                    if (s->slots[i].id == s->activeDevices[j].id)
                        SendMessageW(s->slots[i].hDevCombo, CB_SETCURSEL, idx, 0);
                }
                DarkCombo(s->slots[i].hDevCombo);

                s->slots[i].hIconCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|CBS_DROPDOWNLIST, 46, y+50, 258, 300,
                    hWnd, (HMENU)(UINT_PTR)(300+i), hInst, nullptr);
                int isel = 0;
                for (int j = 0; j < kIconCount; j++) {
                    SendMessageW(s->slots[i].hIconCombo, CB_ADDSTRING, 0, (LPARAM)kIconLabels[j]);
                    if (s->slots[i].iconKey == kIconKeys[j]) isel = j;
                }
                SendMessageW(s->slots[i].hIconCombo, CB_SETCURSEL, isel, 0);
                DarkCombo(s->slots[i].hIconCombo);

                // Browse btn — owner-drawn, shown only when iconKey == "custom"
                s->slots[i].hBrowseBtn = CreateWindowExW(0, L"BUTTON", L"Browse...",
                    WS_CHILD|BS_OWNERDRAW, 310, y+48, 92, 26,
                    hWnd, (HMENU)(UINT_PTR)(400+i), hInst, nullptr);
            }

            // ── Bottom buttons — owner-drawn, positioned for initial deviceCount ─
            int btnY = kTopH + s->deviceCount * kSlotH + 12;
            s->hSaveBtn = CreateWindowExW(0, L"BUTTON", L"Save and Apply",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 12, btnY, 148, 28,
                hWnd, (HMENU)IDOK, hInst, nullptr);
            s->hCancelBtn = CreateWindowExW(0, L"BUTTON", L"Cancel",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 168, btnY, 88, 28,
                hWnd, (HMENU)IDCANCEL, hInst, nullptr);

            EnumChildWindows(hWnd, ApplyFontProc, reinterpret_cast<LPARAM>(s->hFont));
            UpdateSlotVisibility(s);

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

            // ── Top-section labels ────────────────────────────────────────────
            SetTextColor(hdc, kClrDim);
            RECT r;
            r = {12, 22, 78, 38};  DrawTextW(hdc, L"Devices:",  -1, &r, DT_LEFT|DT_TOP);
            r = {156, 22, 196, 38}; DrawTextW(hdc, L"Mode:",    -1, &r, DT_LEFT|DT_TOP);

            // Separator between top controls and slot section
            {
                HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                HPEN op = (HPEN)SelectObject(hdc, p);
                MoveToEx(hdc, 12, kTopH - 6, nullptr);
                LineTo(hdc, cr.right - 12, kTopH - 6);
                SelectObject(hdc, op); DeleteObject(p);
            }

            // ── Per-slot headers, icons, and row labels ───────────────────────
            for (int i = 0; i < s->deviceCount; i++) {
                int y = SlotY(i);

                // Slot header band
                RECT hdr = {0, y, cr.right, y + 20};
                FillRect(hdc, &hdr, s->hSurfBrush);

                // "Slot N" label in header
                SetTextColor(hdc, kClrDim);
                RECT sl = {12, y + 2, 100, y + 18};
                WCHAR lbl[16]; swprintf_s(lbl, L"Slot %d", i + 1);
                DrawTextW(hdc, lbl, -1, &sl, DT_LEFT|DT_TOP);

                // Slot icon (drawn left of the device combo, vertically centred)
                if (s->slots[i].hPreviewIcon)
                    DrawIconEx(hdc, 12, y + 22, s->slots[i].hPreviewIcon,
                               kIconSz, kIconSz, 0, nullptr, DI_NORMAL);

                // "Icon:" row label
                SetTextColor(hdc, kClrDim);
                RECT il = {12, y + 54, 44, y + 68};
                DrawTextW(hdc, L"Icon:", -1, &il, DT_LEFT|DT_TOP);

                // Row separator (between slots, not after the last visible one)
                if (i < s->deviceCount - 1) {
                    HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                    HPEN op = (HPEN)SelectObject(hdc, p);
                    MoveToEx(hdc, 0, y + kSlotH - 1, nullptr);
                    LineTo(hdc, cr.right, y + kSlotH - 1);
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
            bool isBrowse= (dis->CtlID >= 400 && dis->CtlID < 406);

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
                UpdateSlotVisibility(s);
                UpdateLayout(s);

            } else if (id >= 300 && id < 306 && HIWORD(wParam) == CBN_SELCHANGE) {
                // Icon style changed for slot (id-300) — live preview update
                int slot = id - 300;
                int sel  = (int)SendMessageW(s->slots[slot].hIconCombo, CB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < kIconCount) s->slots[slot].iconKey = kIconKeys[sel];
                RefreshPreview(s, slot);
                // Repaint the icon area only
                RECT ir = {12, SlotY(slot)+20, 12+kIconSz+2, SlotY(slot)+20+kIconSz+2};
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
                    RECT ir = {12, SlotY(slot)+20, 12+kIconSz+2, SlotY(slot)+20+kIconSz+2};
                    InvalidateRect(hWnd, &ir, TRUE);
                }

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
                if (s->hTrayHwnd) PostMessageW(s->hTrayHwnd, WM_RELOAD_ALL, 0, 0);
                DestroyWindow(hWnd);

            } else if (id == IDCANCEL) {
                DestroyWindow(hWnd);
            }
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

        static const WCHAR kClass[] = L"AudioSwapDashboard";
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

        // Size window to exactly fit the initial device count.
        int clientH = kTopH + state.deviceCount * kSlotH + kBtnH;
        RECT rc = {0, 0, kCW, clientH};
        AdjustWindowRectEx(&rc,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            FALSE, WS_EX_DLGMODALFRAME);
        int winW = rc.right - rc.left;
        int winH = rc.bottom - rc.top;
        int sx   = (GetSystemMetrics(SM_CXSCREEN) - winW) / 2;
        int sy   = (GetSystemMetrics(SM_CYSCREEN) - winH) / 2;

        HWND hWnd = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            kClass, L"AudioSwap Settings",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            sx, sy, winW, winH,
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

} // namespace AudioSwapGui

// ─── Device selection data ────────────────────────────────────────────────────
// All functions below acquire g_stateLock for writes to shared slot data.

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
    nii.hWnd = hwnd;
    nii.uID  = TRAY_ICON_ID;
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
        nid.hWnd = hwnd;
        nid.uID  = TRAY_ICON_ID;
        nid.uFlags = NIF_SHOWTIP;
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
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"AudioSwap: Right-click to configure");
    else if (localMuted)
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Audio: %.100s (Muted)", currentDev);
    else
        swprintf_s(nid.szTip, ARRAYSIZE(nid.szTip), L"Audio: %.110s", currentDev);

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

void BuildAndShowContextMenu(HWND hWnd) {
    HMENU hMenu = CreatePopupMenu();

    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_SETTINGS, L"Mod Settings...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open WindHawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    POINT pt; GetCursorPos(&pt);
    SetForegroundWindow(hWnd);
    int cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    if (cmd == MENU_OPEN_SETTINGS) {
        HANDLE h = AudioSwapGui::LaunchDashboard(hWnd);
        if (h) {
            if (g_guiThread) CloseHandle(g_guiThread);
            g_guiThread = h;
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
    // If CreateThread fails the worker never resets the lock — do it here.
    if (!g_workerThread) InterlockedExchange(&g_isProcessingClick, 0);
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

    } else if (msg == WM_TIMER && wParam == TRAY_RECT_INIT_TIMER) {
        // Fired by RefreshTrayIconRect when Shell_NotifyIconGetRect returned empty.
        // By now Explorer has had time to complete its paint cycle and assign real
        // screen coordinates to the icon. RefreshTrayIconRect kills the timer on
        // success, or re-arms it for another 200ms if Explorer is still not ready.
        RefreshTrayIconRect();

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

    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        UpdateTrayTip(hWnd, TRUE);  // re-add icon after explorer restart

    } else if (msg == WM_RELOAD_ALL) {
        // Full reload triggered by the settings dashboard after Save and Apply.
        // Picks up new device assignments, icon choices, device count, and swap mode.
        LoadDeviceSelections();
        LoadUserIconsAndSettings();
        UpdateTrayTip(hWnd, FALSE);
        PostMessageW(hWnd, WM_UPDATE_HOOK_STATE, 0, 0);

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
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    RestoreMuteExternal();
    LoadDeviceSelections();
    HWND hwnd = g_trayHwnd;
    if (hwnd) {
        PostMessageW(hwnd, WM_RELOAD_ICONS, 0, 0);
        PostMessageW(hwnd, WM_UPDATE_HOOK_STATE, 0, 0);
    }
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
    if (g_guiThread) {
        // Ask the dashboard's message loop to quit, then wait for it.
        // PostThreadMessageW with WM_QUIT causes GetMessageW to return 0,
        // exiting the loop regardless of IsDialogMessageW.
        DWORD guiId = GetThreadId(g_guiThread);
        if (guiId) PostThreadMessageW(guiId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_guiThread, 2000);
        CloseHandle(g_guiThread);
        g_guiThread = nullptr;
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
