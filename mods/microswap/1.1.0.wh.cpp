// ==WindhawkMod==
// @id              microswap
// @name            MicSwitch
// @description     Tray icon to instantly toggle between two preferred audio inputs (microphones).
// @version         1.1.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32 -lcomdlg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MicSwitch
Instantly toggle between two audio input devices (microphones) from your system tray — no diving into Sound settings.

---

## How to Use

1. **Choose Icons** — Open the **Settings** tab and pick an icon for each of your two devices.
2. **Select Your Devices** — Right-click the tray icon. Use **Set as Input 1** and **Set as Input 2** to assign your inputs from the live device list.
3. **Toggle** — Left-click the tray icon at any time to swap between the two devices instantly.

> The tray tooltip always shows the active input. On first run it will read *"Right-click to configure"* until both devices are assigned.

---

## Changelog

### v1.1.0
- Custom icon support — pick your own image for each device via Settings or right-click.
- Renamed from MicroSwap to MicSwitch.
- Added donate button on the mod page.
- Context menu now follows your Windows dark/light theme.
- Active device shown at the top of the right-click menu.
- Fixed: rare crash when switching devices rapidly.
- Fixed: occasional hang when Windhawk unloads the mod.
- Fixed: tray window appeared in Alt+Tab.
- Fixed: tray icon failed to load on some Windows configurations.
- Tray / Taskbar icon is now independent, no longer linked to the Windhawk icon.

### v1.0.0
- Initial release.
- Right-click the tray icon to assign your two microphones. Left-click to swap between them.
- Device selections are remembered across restarts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icon1: mic_classic
  $name: First Device Icon
  $options:
    - mic_classic: Classic Microphone
    - mic_modern: Modern Microphone
    - headset_gaming: Gaming Headset
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
    - custom: Custom Icon
- icon2: headset_gaming
  $name: Second Device Icon
  $options:
    - mic_classic: Classic Microphone
    - mic_modern: Modern Microphone
    - headset_gaming: Gaming Headset
    - headset_modern: Modern Gaming Headset
    - earphones: Earphones
    - custom: Custom Icon
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <shobjidl.h>

#include <propkey.h>
#include <mmdeviceapi.h>
#include <commdlg.h>
#include <propidl.h>
#include <functiondiscoverykeys_devpkey.h>

#define TRAY_ICON_ID          1
#define WM_TRAY_CALLBACK      (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE  (WM_USER + 2)
#define WM_SHOW_FILE_PICKER   (WM_USER + 5)
#define WM_RELOAD_ICONS       (WM_USER + 6)
#define MENU_DEVICE1_BASE     1000
#define MENU_DEVICE2_BASE     2000
#define MENU_OPEN_WINDHAWK    3000
#define MENU_CUSTOM_ICON_BASE 8000
#define MENU_MAX_DEVICES      32

// Stable GUID that gives our tray icon a process-independent identity.
static const GUID MICSWITCH_TRAY_GUID =
    {0x7174FA7E, 0x93F1, 0x4110, {0x8B, 0x83, 0xA4, 0xAD, 0x2C, 0x76, 0x9C, 0x3B}};

const DWORD CLICK_DEBOUNCE_MS = 500;

static volatile LONG   g_isProcessingClick = 0;
static HANDLE          g_trayThread        = nullptr;
static HANDLE          g_workerThread      = nullptr;
static volatile HWND   g_trayHwnd          = nullptr;
static HINSTANCE       g_hInstance         = nullptr;
static WCHAR           g_windhawkPath[MAX_PATH]  = {};
static WCHAR           g_ddoresDllPath[MAX_PATH] = {};
static HICON           g_hWindHawkIcon     = nullptr;
static HBITMAP         g_hWindHawkBmp      = nullptr;
static HICON           g_iconDev1          = nullptr;
static HBITMAP         g_hIconDev1Bmp      = nullptr;
static HICON           g_iconDev2          = nullptr;
static HBITMAP         g_hIconDev2Bmp      = nullptr;
static DWORD           g_lastClickTime     = 0;
static UINT            g_taskbarCreatedMsg = 0;
static WCHAR           g_lastIconSetting[2][32] = {};

// ── Shared state — protected by g_stateLock ──────────────────────────────────
// Written by main thread (LoadDeviceSelections via WhTool_ModSettingsChanged)
// and tray thread (SaveDeviceSelection). Read by worker thread (ToggleAudioDevice).
static CRITICAL_SECTION g_stateLock;
static WCHAR g_cachedDev1Id[512]   = {};
static WCHAR g_cachedDev1Name[256] = {};
static WCHAR g_cachedDev2Id[512]   = {};
static WCHAR g_cachedDev2Name[256] = {};
// ─────────────────────────────────────────────────────────────────────────────

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

int GetIconIndex(PCWSTR iconSetting) {
    if (iconSetting) {
        if (wcscmp(iconSetting, L"mic_classic")    == 0) return 3;
        if (wcscmp(iconSetting, L"mic_modern")     == 0) return 92;
        if (wcscmp(iconSetting, L"headset_gaming") == 0) return 8;
        if (wcscmp(iconSetting, L"headset_modern") == 0) return 95;
        if (wcscmp(iconSetting, L"earphones")      == 0) return 6;
    }
    return 3;
}

// ─── Device selection data ────────────────────────────────────────────────────

void LoadDeviceSelections() {
    // Read outside lock — Wh_GetStringValue is thread-safe.
    WCHAR tmpId1[512]   = {};
    WCHAR tmpName1[256] = {};
    WCHAR tmpId2[512]   = {};
    WCHAR tmpName2[256] = {};

    Wh_GetStringValue(L"Device1Id",   tmpId1,   512);
    Wh_GetStringValue(L"Device1Name", tmpName1, 256);
    Wh_GetStringValue(L"Device2Id",   tmpId2,   512);
    Wh_GetStringValue(L"Device2Name", tmpName2, 256);

    EnterCriticalSection(&g_stateLock);
    lstrcpynW(g_cachedDev1Id,   tmpId1,   512);
    lstrcpynW(g_cachedDev1Name, tmpName1, 256);
    lstrcpynW(g_cachedDev2Id,   tmpId2,   512);
    lstrcpynW(g_cachedDev2Name, tmpName2, 256);
    LeaveCriticalSection(&g_stateLock);
}

void SaveDeviceSelection(int slot, PCWSTR deviceId, PCWSTR friendlyName) {
    if (slot == 1) {
        Wh_SetStringValue(L"Device1Id",   deviceId);
        Wh_SetStringValue(L"Device1Name", friendlyName);
        EnterCriticalSection(&g_stateLock);
        lstrcpynW(g_cachedDev1Id,   deviceId,     512);
        lstrcpynW(g_cachedDev1Name, friendlyName, 256);
        LeaveCriticalSection(&g_stateLock);
    } else {
        Wh_SetStringValue(L"Device2Id",   deviceId);
        Wh_SetStringValue(L"Device2Name", friendlyName);
        EnterCriticalSection(&g_stateLock);
        lstrcpynW(g_cachedDev2Id,   deviceId,     512);
        lstrcpynW(g_cachedDev2Name, friendlyName, 256);
        LeaveCriticalSection(&g_stateLock);
    }
}

// ─── Icon loading ─────────────────────────────────────────────────────────────

static HICON LoadSingleIcon(PCWSTR iconSetting, int slotIndex) {
    if (iconSetting && wcscmp(iconSetting, L"custom") == 0) {
        WCHAR pathKey[32];
        swprintf_s(pathKey, L"icon%d_custom_path", slotIndex);
        WCHAR customPath[MAX_PATH] = {};
        Wh_GetStringValue(pathKey, customPath, MAX_PATH);
        if (customPath[0]) {
            HICON hIcon = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            if (hIcon) return hIcon;
        }
    }
    HICON hIcon = nullptr;
    ExtractIconExW(g_ddoresDllPath, GetIconIndex(iconSetting), nullptr, &hIcon, 1);
    return hIcon;
}

void LoadUserIconsAndSettings() {
    if (g_iconDev1)    { DestroyIcon(g_iconDev1);      g_iconDev1    = nullptr; }
    if (g_iconDev2)    { DestroyIcon(g_iconDev2);      g_iconDev2    = nullptr; }
    if (g_hIconDev1Bmp){ DeleteObject(g_hIconDev1Bmp); g_hIconDev1Bmp = nullptr; }
    if (g_hIconDev2Bmp){ DeleteObject(g_hIconDev2Bmp); g_hIconDev2Bmp = nullptr; }

    PCWSTR s1 = Wh_GetStringSetting(L"icon1");
    PCWSTR s2 = Wh_GetStringSetting(L"icon2");
    g_iconDev1 = LoadSingleIcon(s1, 1);
    g_iconDev2 = LoadSingleIcon(s2, 2);

    auto setupBmp = [](HICON hIcon, HBITMAP* pBmp) {
        if (!hIcon) return;
        ICONINFO ii = {};
        if (GetIconInfo(hIcon, &ii)) {
            *pBmp = ii.hbmColor;
            if (!*pBmp) {
                *pBmp = ii.hbmMask;
            } else if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
        }
    };
    setupBmp(g_iconDev1, &g_hIconDev1Bmp);
    setupBmp(g_iconDev2, &g_hIconDev2Bmp);

    if (s1) wcscpy_s(g_lastIconSetting[0], 32, s1); else g_lastIconSetting[0][0] = L'\0';
    if (s2) wcscpy_s(g_lastIconSetting[1], 32, s2); else g_lastIconSetting[1][0] = L'\0';

    if (s1) Wh_FreeStringSetting(s1);
    if (s2) Wh_FreeStringSetting(s2);
}

// ─── Tray tip ─────────────────────────────────────────────────────────────────

void UpdateTrayTip(HWND hWnd, BOOL isAdd) {
    // Snapshot shared state under lock before any COM calls.
    WCHAR localId1[512] = {};
    WCHAR localId2[512] = {};
    EnterCriticalSection(&g_stateLock);
    lstrcpynW(localId1, g_cachedDev1Id, 512);
    lstrcpynW(localId2, g_cachedDev2Id, 512);
    LeaveCriticalSection(&g_stateLock);

    WCHAR currentDev[256] = L"Unknown Device";
    WCHAR currentId[512]  = {};
    HICON currentIcon = g_iconDev1;

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
            pDev->Release();
        }
        pEnum->Release();
    }

    if (localId1[0] != L'\0' && wcscmp(currentId, localId1) == 0)
        currentIcon = g_iconDev1;
    else if (localId2[0] != L'\0' && wcscmp(currentId, localId2) == 0)
        currentIcon = g_iconDev2;

    static WCHAR s_lastDev[256] = {};
    static HICON s_lastIcon = nullptr;
    if (!isAdd && wcscmp(currentDev, s_lastDev) == 0 && currentIcon == s_lastIcon)
        return;
    lstrcpynW(s_lastDev, currentDev, ARRAYSIZE(s_lastDev));
    s_lastIcon = currentIcon;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd             = hWnd;
    nid.uID              = TRAY_ICON_ID;
    nid.uFlags           = NIF_MESSAGE | NIF_TIP | NIF_ICON | NIF_GUID;
    nid.guidItem         = MICSWITCH_TRAY_GUID;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    if (localId1[0] == L'\0' || localId2[0] == L'\0')
        swprintf_s(nid.szTip, L"MicSwitch: Right-click to configure");
    else
        swprintf_s(nid.szTip, L"Mic: %s", currentDev);

    nid.hIcon = currentIcon;
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &nid);

    if (isAdd) {
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd     = hWnd;
        nidVer.uID      = TRAY_ICON_ID;
        nidVer.uFlags   = NIF_GUID;
        nidVer.guidItem = MICSWITCH_TRAY_GUID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    }
}

// ─── Audio toggle ─────────────────────────────────────────────────────────────

BOOL ToggleAudioDevice() {
    // Snapshot shared device IDs under lock before any COM calls.
    WCHAR localId1[512] = {};
    WCHAR localId2[512] = {};
    EnterCriticalSection(&g_stateLock);
    lstrcpynW(localId1, g_cachedDev1Id, 512);
    lstrcpynW(localId2, g_cachedDev2Id, 512);
    LeaveCriticalSection(&g_stateLock);

    if (localId1[0] == L'\0' || localId2[0] == L'\0') return FALSE;

    HRESULT comHr = CoInitialize(nullptr);
    bool comOk = SUCCEEDED(comHr) || comHr == S_FALSE;
    if (!comOk) return FALSE;

    IMMDeviceEnumerator* pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        CoUninitialize(); return FALSE;
    }

    IMMDevice* pDef = nullptr;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDef))) {
        pEnum->Release(); CoUninitialize(); return FALSE;
    }

    LPWSTR currentId = nullptr;
    HRESULT hr = pDef->GetId(&currentId);
    pDef->Release();
    if (FAILED(hr) || !currentId) {
        pEnum->Release(); CoUninitialize(); return FALSE;
    }

    // Toggle: if current is slot 1 go to slot 2, otherwise go to slot 1.
    PCWSTR targetId = (wcscmp(currentId, localId1) == 0) ? localId2 : localId1;
    CoTaskMemFree(currentId);

    IPolicyConfig* pPol = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL,
                                   IID_IPolicyConfig_Win10_11, (void**)&pPol))) {
        pPol->SetDefaultEndpoint(targetId, eConsole);
        pPol->SetDefaultEndpoint(targetId, eMultimedia);
        pPol->SetDefaultEndpoint(targetId, eCommunications);
        pPol->Release();
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

struct AudioDevice { WCHAR id[512]; WCHAR name[256]; };

void BuildAndShowContextMenu(HWND hWnd) {
    // Snapshot shared state under lock.
    WCHAR localId1[512] = {};
    WCHAR localId2[512] = {};
    EnterCriticalSection(&g_stateLock);
    lstrcpynW(localId1, g_cachedDev1Id, 512);
    lstrcpynW(localId2, g_cachedDev2Id, 512);
    LeaveCriticalSection(&g_stateLock);

    AudioDevice devices[MENU_MAX_DEVICES];
    int deviceCount = 0;

    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDeviceCollection* pColl = nullptr;
        if (SUCCEEDED(pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pColl))) {
            UINT count = 0;
            pColl->GetCount(&count);
            if (count > MENU_MAX_DEVICES) count = MENU_MAX_DEVICES;
            for (UINT i = 0; i < count; i++) {
                IMMDevice* pDevice = nullptr;
                if (FAILED(pColl->Item(i, &pDevice))) continue;
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
            pColl->Release();
        }
        pEnum->Release();
    }

    // Build status text before constructing the menu so it can go at the top.
    WCHAR statusText[300];
    if (localId1[0] == L'\0' || localId2[0] == L'\0') {
        lstrcpyW(statusText, L"Right-click to configure inputs");
    } else {
        WCHAR activeName[256] = L"Unknown";
        IMMDeviceEnumerator* pEnum2 = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                       __uuidof(IMMDeviceEnumerator), (void**)&pEnum2))) {
            IMMDevice* pDef = nullptr;
            if (SUCCEEDED(pEnum2->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDef))) {
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
        swprintf_s(statusText, L"Active: %s", activeName);
    }

    HMENU hSub1 = CreatePopupMenu();
    HMENU hSub2 = CreatePopupMenu();
    for (int i = 0; i < deviceCount; i++) {
        UINT f1 = MF_STRING | (wcscmp(devices[i].id, localId1) == 0 ? MF_CHECKED : 0);
        UINT f2 = MF_STRING | (wcscmp(devices[i].id, localId2) == 0 ? MF_CHECKED : 0);
        AppendMenuW(hSub1, f1, MENU_DEVICE1_BASE + i, devices[i].name);
        AppendMenuW(hSub2, f2, MENU_DEVICE2_BASE + i, devices[i].name);
    }

    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW mii1 = {sizeof(mii1)};
    mii1.fMask      = MIIM_SUBMENU | MIIM_STRING | MIIM_BITMAP;
    mii1.hSubMenu   = hSub1;
    mii1.dwTypeData = (LPWSTR)L"Set as Input 1";
    mii1.hbmpItem   = g_hIconDev1Bmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &mii1);

    MENUITEMINFOW mii2 = {sizeof(mii2)};
    mii2.fMask      = MIIM_SUBMENU | MIIM_STRING | MIIM_BITMAP;
    mii2.hSubMenu   = hSub2;
    mii2.dwTypeData = (LPWSTR)L"Set as Input 2";
    mii2.hbmpItem   = g_hIconDev2Bmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &mii2);

    AppendMenuW(hMenu, MF_STRING, MENU_CUSTOM_ICON_BASE + 1, L"Custom Icon for Device 1...");
    AppendMenuW(hMenu, MF_STRING, MENU_CUSTOM_ICON_BASE + 2, L"Custom Icon for Device 2...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open Windhawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hWnd);
    bool dark = IsSystemDarkMode();
    ApplyContextMenuTheme(hWnd, dark);
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                             pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(hMenu);

    if (cmd >= MENU_DEVICE1_BASE && cmd < MENU_DEVICE1_BASE + deviceCount) {
        int idx = cmd - MENU_DEVICE1_BASE;
        SaveDeviceSelection(1, devices[idx].id, devices[idx].name);
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
    } else if (cmd >= MENU_DEVICE2_BASE && cmd < MENU_DEVICE2_BASE + deviceCount) {
        int idx = cmd - MENU_DEVICE2_BASE;
        SaveDeviceSelection(2, devices[idx].id, devices[idx].name);
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
    } else if (cmd == MENU_OPEN_WINDHAWK) {
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpFile = g_windhawkPath;
        sei.nShow  = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    } else if (cmd >= MENU_CUSTOM_ICON_BASE && cmd < MENU_CUSTOM_ICON_BASE + 2) {
        int slot = cmd - MENU_CUSTOM_ICON_BASE + 1;
        WCHAR path[MAX_PATH] = {};
        WCHAR title[64];
        swprintf_s(title, L"Select Icon for Device %d", slot);
        OPENFILENAMEW ofn = {sizeof(ofn)};
        ofn.hwndOwner    = hWnd;
        ofn.lpstrFilter  = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFile    = path;
        ofn.nMaxFile     = MAX_PATH;
        ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
        ofn.lpstrTitle   = title;
        if (GetOpenFileNameW(&ofn)) {
            WCHAR pathKey[32];
            swprintf_s(pathKey, L"icon%d_custom_path", slot);
            Wh_SetStringValue(pathKey, path);
            LoadUserIconsAndSettings();
            PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
        }
    }
}

// ─── Worker thread ────────────────────────────────────────────────────────────

DWORD WINAPI WorkerThreadProc(LPVOID) {
    if (ToggleAudioDevice() && g_trayHwnd)
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, 0, 0);
    InterlockedExchange(&g_isProcessingClick, 0);
    return 0;
}

// ─── Tray window ──────────────────────────────────────────────────────────────

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CALLBACK && LOWORD(lParam) == WM_RBUTTONUP) {
        BuildAndShowContextMenu(hWnd);
    } else if (msg == WM_TRAY_CALLBACK && LOWORD(lParam) == WM_LBUTTONUP) {
        if (InterlockedExchange(&g_isProcessingClick, 1) == 0) {
            DWORD now = GetTickCount();
            if (now - g_lastClickTime > CLICK_DEBOUNCE_MS) {
                g_lastClickTime = now;
                if (g_workerThread) { CloseHandle(g_workerThread); g_workerThread = nullptr; }
                g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
            } else {
                InterlockedExchange(&g_isProcessingClick, 0);
            }
        }
    } else if (msg == WM_UPDATE_TRAY_STATE || (msg == WM_TIMER && wParam == 1)) {
        UpdateTrayTip(hWnd, FALSE);
    } else if (msg == WM_RELOAD_ICONS) {
        WCHAR prev[2][32] = {};
        wcscpy_s(prev[0], 32, g_lastIconSetting[0]);
        wcscpy_s(prev[1], 32, g_lastIconSetting[1]);
        LoadUserIconsAndSettings();
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
        DWORD pickerSlots = 0;
        for (int slot = 1; slot <= 2; slot++) {
            WCHAR key[16];
            swprintf_s(key, L"icon%d", slot);
            PCWSTR s = Wh_GetStringSetting(key);
            BOOL isCustom  = (s && wcscmp(s, L"custom") == 0);
            BOOL wasCustom = (wcscmp(prev[slot - 1], L"custom") == 0);
            if (s) Wh_FreeStringSetting(s);
            if (isCustom && !wasCustom)
                pickerSlots |= (1u << (slot - 1));
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
            ofn.hwndOwner    = hWnd;
            ofn.lpstrFilter  = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFile    = path;
            ofn.nMaxFile     = MAX_PATH;
            ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrTitle   = title;
            if (GetOpenFileNameW(&ofn)) {
                WCHAR pathKey[32];
                swprintf_s(pathKey, L"icon%d_custom_path", slot);
                Wh_SetStringValue(pathKey, path);
                LoadUserIconsAndSettings();
                PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
            }
        }
    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        UpdateTrayTip(hWnd, TRUE);
    } else if (msg == WM_CLOSE) {
        // Orderly shutdown: kill timer, remove tray icon, then destroy window.
        KillTimer(hWnd, 1);
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd     = hWnd;
        nid.uID      = TRAY_ICON_ID;
        nid.uFlags   = NIF_GUID;
        nid.guidItem = MICSWITCH_TRAY_GUID;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DestroyWindow(hWnd);
        return 0;
    } else if (msg == WM_DESTROY) {
        g_trayHwnd = nullptr;
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

    // WS_EX_TOOLWINDOW + WS_EX_NOACTIVATE: hidden utility window, never in Alt+Tab.
    g_trayHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicSwitch",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hInstance, nullptr);
    if (!g_trayHwnd) { CoUninitialize(); return 1; }

    // Unique AUMID so the OS doesn't group this icon with Windhawk's main window.
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt      = VT_LPWSTR;
        var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (var.pwszVal) {
            lstrcpyW(var.pwszVal, L"BlackPaw.MicSwitch");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit();
        pps->Release();
    }

    // 1500ms poll timer keeps tooltip in sync without IMMNotificationClient overhead.
    SetTimer(g_trayHwnd, 1, 1500, nullptr);
    UpdateTrayTip(g_trayHwnd, TRUE);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { DispatchMessageW(&msg); }

    CoUninitialize();
    return 0;
}

// ─── Mod lifecycle ────────────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"MicSwitch Mod Init");

    InitializeCriticalSection(&g_stateLock);

    g_hInstance = GetModuleHandleW(nullptr);
    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    // Full System32 path — ExtractIconExW handles the .mun redirect on Win11.
    UINT sysLen = GetSystemDirectoryW(g_ddoresDllPath, MAX_PATH);
    if (sysLen > 0 && sysLen < MAX_PATH - 12)
        lstrcatW(g_ddoresDllPath, L"\\ddores.dll");
    else
        lstrcpyW(g_ddoresDllPath, L"ddores.dll");

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

    LoadUserIconsAndSettings();
    LoadDeviceSelections();
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadDeviceSelections();
    HWND hwnd = g_trayHwnd;
    if (hwnd) PostMessageW(hwnd, WM_RELOAD_ICONS, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"MicSwitch Mod Uninit");

    // Capture hwnd before posting — tray thread clears g_trayHwnd in WM_DESTROY.
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

    if (g_iconDev1)     { DestroyIcon(g_iconDev1);       g_iconDev1     = nullptr; }
    if (g_iconDev2)     { DestroyIcon(g_iconDev2);       g_iconDev2     = nullptr; }
    if (g_hIconDev1Bmp) { DeleteObject(g_hIconDev1Bmp);  g_hIconDev1Bmp = nullptr; }
    if (g_hIconDev2Bmp) { DeleteObject(g_hIconDev2Bmp);  g_hIconDev2Bmp = nullptr; }
    if (g_hWindHawkIcon){ DestroyIcon(g_hWindHawkIcon);  g_hWindHawkIcon = nullptr; }
    if (g_hWindHawkBmp) { DeleteObject(g_hWindHawkBmp);  g_hWindHawkBmp  = nullptr; }

    DeleteCriticalSection(&g_stateLock);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod boilerplate — do not modify.
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

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

    if (isExcluded) return FALSE;

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
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

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
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) { Wh_Log(L"No kernelbase.dll/kernel32.dll"); return; }
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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) { Wh_Log(L"No CreateProcessInternalW"); return; }

    STARTUPINFOW si{.cb = sizeof(STARTUPINFOW), .dwFlags = STARTF_FORCEOFFFEEDBACK};
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
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
