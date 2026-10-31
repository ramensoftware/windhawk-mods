// ==WindhawkMod==
// @id              bt-battery-monitor
// @name            BT Battery Monitor
// @description     Shows Bluetooth device battery levels in the system tray with battery overview
// @version         1.0.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @license         MIT
// @compilerOptions -lbluetoothapis -lbthprops -lsetupapi -lcfgmgr32 -lgdi32 -luser32 -lshell32 -lole32 -luuid -lcomctl32 -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# BT Battery Monitor
See every connected Bluetooth device's battery level right in your system tray — no more digging into Bluetooth settings to find out which keyboard or mouse needs charging.

> Works great alongside **[AudioSwap](https://windhawk.net/mods/audioswap)** and **[MicSwitch](https://windhawk.net/mods/microswap)** — companion mods that bring the same tray experience to audio output and microphone control.

![Controller connected](https://i.imgur.com/QDHrOTO.png)

![Right-click menu](https://i.imgur.com/GWzrttu.png)

---

## How to Use

1. **Open Settings** — Right-click the tray icon and select **Mod Settings** to open the configuration dashboard.
2. **Set Polling Interval** — Choose how often to scan for battery updates (default: 10 seconds).
3. **Set Warning Threshold** — Pick the battery percentage that triggers the red/black flashing icon.
4. **Choose Icons** — Pick a preset icon per device type, or click **Browse...** to load a custom `.ico` file.
5. Click **Save and Apply** — the tray icon updates immediately, no restart needed.

### Left-click to rescan

Left-click the tray icon to trigger an immediate Bluetooth rescan. Use it when you've just paired or disconnected a device and want the battery readout to refresh without waiting for the next polling interval.

### Right-click device list

Right-click the tray icon to see every connected Bluetooth device with its battery percentage, rescan immediately, or open Bluetooth Settings.

### Low Battery Warning

When any connected device drops below the configured threshold (default 30%), the tray icon starts alternating between solid red and black every second. Hover over the icon to see which device is running low and exactly how much juice it has left.

### Device Icons

The mod detects device types automatically and lets you pick a different icon for each:
- **Keyboard**, **Mouse**, **Headphones**, **Controller** — each gets its own icon so you can tell them apart instantly.
- **Multiple Devices** — shown when more than one device is connected.
- **No Connected Devices** — a clear visual indicator that nothing's paired.

### Polling Interval

Pick how often the mod checks for updates, from **every second** to **once an hour** (default: 10 seconds). Quick polling keeps the battery readout fresh, while slower polling saves a tiny bit of CPU.

---

## Known Bugs

- **Bluetooth devices that don't report battery**: Some older Bluetooth peripherals simply don't broadcast battery info — the mod can't show what the hardware doesn't tell it. Check your device specs to see if battery reporting is supported.

---

## Changelog

# 1.0.0
- **New:** Tray icon shows battery percentage for every connected Bluetooth device — keyboard, mouse, headphones, controller, you name it.
- **New:** Left-click triggers an immediate Bluetooth rescan to refresh battery readouts.
- **New:** Low battery warning — tray icon flashes red and black when any device drops below your threshold.
- **New:** Custom Settings Dashboard — dark themed, DPI-aware, with per-device icon picker.
- **New:** Configurable polling interval (1s–1h) and warning threshold (0–50%).
- **New:** Automatic Bluetooth rescan on device arrival/removal — no manual refresh needed.
- **New:** Right-click → Bluetooth Settings shortcut for quick pairing.
- **New:** Choose preset icons from `ddores.dll` per device type or browse for custom `.ico` files.
- **New:** Clean tray icon identity — independent from Windhawk, no taskbar grouping issues.
- **New:** Safe tool mod — runs in a dedicated `windhawk.exe` child process, no Explorer injection needed.
*/
// ==/WindhawkModReadme==

// ─── Win32 / CRT Headers ────────────────────────────────────────────────────

#define NOMINMAX
#include <windows.h>
#include <bluetoothapis.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <shellapi.h>
#include <strsafe.h>
#include <dbt.h>
#include <commctrl.h>
#include <commdlg.h>

#include <vector>
#include <string>
#include <utility>
#include <atomic>
#include <process.h>
#include <shobjidl.h>
#include <propkey.h>
#include <propsys.h>


// ─── Message & Menu Constants ────────────────────────────────────────────────

#define WM_TRAY_CALLBACK (WM_USER + 1)
#define WM_UPDATE_DEVICES (WM_USER + 2)
#define WM_RELOAD_ALL (WM_USER + 3)
#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif
#define IDM_RESCAN 2001
#define IDM_BT_SETTINGS 2002
#define MENU_OPEN_SETTINGS 9001
#define MENU_OPEN_WINDHAWK 9000

// ─── Bluetooth Device Property Keys ─────────────────────────────────────────

static const DEVPROPKEY DEVPKEY_Bluetooth_BatteryLevel  = { {0x104ea319, 0x6ee2, 0x4701, {0xbd, 0x47, 0x8d, 0xdb, 0xf4, 0x25, 0xbb, 0xe5}}, 2 };
static const DEVPROPKEY DEVPKEY_Bluetooth_DeviceAddress = { {0xE57A6B4A, 0x21B8, 0x4B8A, {0xB4, 0xB4, 0x73, 0xB9, 0xF3, 0x58, 0xED, 0x60}}, 1 };
static const DEVPROPKEY DEVPKEY_Device_FriendlyName   = { {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14 };
static const DEVPROPKEY DEVPKEY_Device_BusReportedDeviceDesc = { {0x540b947e, 0x8b40, 0x45bc, {0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2}}, 4 };
static const DEVPROPKEY DEVPKEY_Bluetooth_IsConnected = { {0x2bd67d8b, 0x8beb, 0x48d5, {0x87, 0xe0, 0x6c, 0xda, 0x34, 0x28, 0x04, 0x0a}}, 1 };

// ─── Device Interface GUIDs ──────────────────────────────────────────────────

static const WCHAR MEDIA_CLASS_GUID_STRING[] = L"{4d36e96c-e325-11ce-bfc1-08002be10318}";
static const GUID GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE = {0x6E1BB058, 0x02B4, 0x4B0C, {0x9F, 0xDE, 0x6C, 0x9A, 0x04, 0xCE, 0xB7, 0x12}};
static const GUID GUID_DEVINTERFACE_HID = {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}};

// Stable GUID that gives our tray icon a process-independent identity.
static const GUID GUID_BTBAT_TRAY =
    {0x7b1e9a3f, 0x2c5d, 0x4a8e, {0x9b, 0x6f, 0x1d, 0x3e, 0x5a, 0x7c, 0x8b, 0x4d}};

static const GUID GUID_BTBAT_BTHPORT = {0x0850302A, 0xB344, 0x4fda, {0x9B, 0xE5, 0x22, 0x5C, 0x96, 0x69, 0x50, 0x20}};

// ─── DeviceInfo ──────────────────────────────────────────────────────────────

struct DeviceInfo {
    std::wstring name;
    BYTE address[6] = {0};
    int batteryPercent = -1;
    bool connected = false;
    ULONG classOfDevice = 0;
    bool isKeyboard = false;
};

// ─── Globals ─────────────────────────────────────────────────────────────────

static std::atomic<HWND> g_hwnd{nullptr};
static std::vector<DeviceInfo> g_devices;
static CRITICAL_SECTION g_devicesLock;

// Thread sync
static HANDLE g_shutdownEvent = NULL;
static HANDLE g_rescanEvent = NULL;
static HANDLE g_scannerThread = NULL;
static HANDLE g_trayThread = NULL;

// Cached icon handles (per device type)
static HICON g_hIconDisconnected = NULL;
static HICON g_hIconKeyboard = NULL;
static HICON g_hIconMouse = NULL;
static HICON g_hIconHeadphones = NULL;
static HICON g_hIconController = NULL;
static HICON g_hIconMulti = NULL;
static HICON g_hIconLowBatRed = NULL;
static HICON g_hIconLowBatBlack = NULL;
static HBITMAP g_hWindHawkBmp = nullptr;

static WCHAR g_windhawkPath[MAX_PATH] = {};
static UINT g_taskbarCreatedMsg = 0;

// Settings (atomically readable from any thread)
static std::atomic<LONG> g_refreshIntervalMs{10000};
static std::atomic<int> g_warningThreshold{30};

// Tray icon cache to avoid redundant NIM_MODIFY calls
static WCHAR g_lastTip[128] = {};
static HICON g_lastIcon = NULL;

// Dashboard GUI thread
static HANDLE g_guiThread = nullptr;
static HWND g_dashboardHwnd = nullptr;
static LONG g_guiRunning = 0;

static std::atomic<bool> g_iconsAvailable{true};

// Device notification handles
static HDEVNOTIFY g_hNotifyHid = nullptr;
static HDEVNOTIFY g_hNotifyGatt = nullptr;
static HDEVNOTIFY g_hNotifyBthPort = nullptr;

namespace BTBatGui {
    HANDLE LaunchDashboard(HWND hTrayHwnd);
}

// ─── Icon Helpers ────────────────────────────────────────────────────────────

static int GetIconIndex(PCWSTR s) {
    if (!s) return 108; // default to controller
    if (wcscmp(s, L"keyboard") == 0)   return 30;
    if (wcscmp(s, L"mouse") == 0)      return 110;
    if (wcscmp(s, L"headphones") == 0) return 91;
    if (wcscmp(s, L"controller") == 0) return 108;
    if (wcscmp(s, L"multiple") == 0)   return 94;
    if (wcscmp(s, L"unknown") == 0)    return 130;
    return 108;
}

// Check a custom .ico path for basic sanity — no traversal, must end in .ico
static bool IsValidIconPath(PCWSTR path) {
    if (!path || !path[0]) return false;
    if (wcsstr(path, L"..") != NULL) return false;
    size_t len = wcslen(path);
    if (len < 5) return false;
    return _wcsicmp(path + len - 4, L".ico") == 0;
}

// Generate a solid-color circle icon programmatically (used for low-battery flash states)
static HICON CreateColorIcon(BYTE r, BYTE g, BYTE b, int size) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;

    DWORD* pixels;
    HDC hdc = GetDC(NULL);
    HBITMAP hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
    ReleaseDC(NULL, hdc);
    if (!hbm) return NULL;

    DWORD color = RGB(r, g, b) | 0xFF000000;
    DWORD transparent = 0x00000000;

    int cx = size / 2;
    int cy = size / 2;
    int radius = size / 2 - 1;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int dx = x - cx;
            int dy = y - cy;
            bool filled = (dx * dx + dy * dy) <= radius * radius;
            pixels[y * size + x] = filled ? color : transparent;
        }
    }

    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);
    if (!hbmMask) { DeleteObject(hbm); return NULL; }

    HDC hdcMask = CreateCompatibleDC(NULL);
    if (!hdcMask) { DeleteObject(hbm); DeleteObject(hbmMask); return NULL; }
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMask, hbmMask);
    RECT rect = {0, 0, size, size};
    FillRect(hdcMask, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    HBRUSH hbrBlack = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMask, hbrBlack);
    HPEN hOldPen = (HPEN)SelectObject(hdcMask, GetStockObject(NULL_PEN));
    Ellipse(hdcMask, 1, 1, size - 1, size - 1);
    SelectObject(hdcMask, hOldBrush);
    SelectObject(hdcMask, hOldPen);
    SelectObject(hdcMask, hOldBmp);
    DeleteObject(hbrBlack);
    DeleteDC(hdcMask);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = hbm;
    ii.hbmMask = hbmMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hbm);
    DeleteObject(hbmMask);
    return hIcon;
}

// Generate a red X icon for the "no connected devices" state
static HICON CreateXIcon(int size) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;

    DWORD* pixels;
    HDC hdc = GetDC(NULL);
    HBITMAP hbmColor = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
    ReleaseDC(NULL, hdc);
    if (!hbmColor) return NULL;

    memset(pixels, 0, size * size * 4);

    int t = std::max(2, size / 5);
    int last = size - 1;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (abs(x - y) <= t / 2 || abs(x + y - last) <= t / 2)
                pixels[y * size + x] = 0xFFFF0000;
        }
    }

    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);
    if (!hbmMask) { DeleteObject(hbmColor); return NULL; }

    HDC hdcMask = CreateCompatibleDC(NULL);
    if (!hdcMask) { DeleteObject(hbmColor); DeleteObject(hbmMask); return NULL; }
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMask, hbmMask);
    RECT r = {0, 0, size, size};
    FillRect(hdcMask, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    HBRUSH hbrWhite = CreateSolidBrush(RGB(255, 255, 255));
    HPEN hWhitePen = CreatePen(PS_SOLID, t, RGB(255, 255, 255));
    SelectObject(hdcMask, hbrWhite);
    SelectObject(hdcMask, hWhitePen);
    MoveToEx(hdcMask, 0, 0, NULL); LineTo(hdcMask, size, size);
    MoveToEx(hdcMask, size, 0, NULL); LineTo(hdcMask, 0, size);
    SelectObject(hdcMask, hOldBmp);
    DeleteObject(hbrWhite);
    DeleteObject(hWhitePen);
    DeleteDC(hdcMask);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = hbmColor;
    ii.hbmMask = hbmMask;
    HICON hIcon = CreateIconIndirect(&ii);
    DeleteObject(hbmColor);
    DeleteObject(hbmMask);
    return hIcon;
}

// Free all cached icon / GDI resources
static void DestroyIcons() {
    if (g_hIconDisconnected) { DestroyIcon(g_hIconDisconnected); g_hIconDisconnected = NULL; }
    if (g_hIconKeyboard) { DestroyIcon(g_hIconKeyboard); g_hIconKeyboard = NULL; }
    if (g_hIconMouse) { DestroyIcon(g_hIconMouse); g_hIconMouse = NULL; }
    if (g_hIconHeadphones) { DestroyIcon(g_hIconHeadphones); g_hIconHeadphones = NULL; }
    if (g_hIconController) { DestroyIcon(g_hIconController); g_hIconController = NULL; }
    if (g_hIconMulti) { DestroyIcon(g_hIconMulti); g_hIconMulti = NULL; }
    if (g_hIconLowBatRed) { DestroyIcon(g_hIconLowBatRed); g_hIconLowBatRed = NULL; }
    if (g_hIconLowBatBlack) { DestroyIcon(g_hIconLowBatBlack); g_hIconLowBatBlack = NULL; }
    if (g_hWindHawkBmp) { DeleteObject(g_hWindHawkBmp); g_hWindHawkBmp = nullptr; }
}

static void LoadSettings();

// Load or generate all tray icons — falls back to programmatic icons if ddores.dll unavailable
static bool CreateIcons() {
    WCHAR sysPath[MAX_PATH];
    if (!GetSystemDirectoryW(sysPath, MAX_PATH)) return false;

    WCHAR ddoresPath[MAX_PATH];
    StringCchCopyW(ddoresPath, MAX_PATH, sysPath);
    StringCchCatW(ddoresPath, MAX_PATH, L"\\ddores.dll");

    DestroyIcons();
    LoadSettings();

    if (!g_hIconDisconnected)
        g_hIconDisconnected = CreateXIcon(16);

    g_hIconLowBatRed = CreateColorIcon(255, 0, 0, 16);
    g_hIconLowBatBlack = CreateColorIcon(0, 0, 0, 16);

    HICON hWhIcon = nullptr;
    int whIconIndices[] = {98, 94, 95, 6};
    for (int idx : whIconIndices) {
        ExtractIconExW(ddoresPath, idx, nullptr, &hWhIcon, 1);
        if (hWhIcon) break;
    }
    if (hWhIcon) {
        ICONINFO ii = {};
        if (GetIconInfo(hWhIcon, &ii)) {
            g_hWindHawkBmp = ii.hbmColor;
            if (!g_hWindHawkBmp)
                g_hWindHawkBmp = ii.hbmMask;
            else if (ii.hbmMask)
                DeleteObject(ii.hbmMask);
        }
        DestroyIcon(hWhIcon);
    }

    return true;
}

// ─── Bluetooth Battery Reading ────────────────────────────────────────────

// Poll battery from Bluetooth media class device tree (most accurate source)
static void GetBatteryFromMediaClass(std::vector<DeviceInfo>& devices) {
    ULONG listSize = 0;
    CONFIGRET cr = CM_Get_Device_ID_List_SizeW(&listSize, MEDIA_CLASS_GUID_STRING, CM_GETIDLIST_FILTER_PRESENT);
    if (cr != CR_SUCCESS || listSize == 0) return;

    std::vector<WCHAR> idList(listSize);
    cr = CM_Get_Device_ID_ListW(MEDIA_CLASS_GUID_STRING, idList.data(), listSize, CM_GETIDLIST_FILTER_PRESENT);
    if (cr != CR_SUCCESS) return;

    for (const WCHAR* deviceId = idList.data(); *deviceId; deviceId += wcslen(deviceId) + 1) {
        DEVINST devInst = 0;
        if (CM_Locate_DevNodeW(&devInst, (DEVINSTID_W)deviceId, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS)
            continue;

        BYTE battery = 0;
        DEVPROPTYPE propType = DEVPROP_TYPE_EMPTY;
        ULONG propSize = sizeof(battery);
        if (CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Bluetooth_BatteryLevel, &propType, (PBYTE)&battery, &propSize, 0) != CR_SUCCESS
            || propType != DEVPROP_TYPE_BYTE
            || propSize != sizeof(battery))
            continue;

        WCHAR btAddr[13] = {};
        propType = DEVPROP_TYPE_EMPTY;
        propSize = sizeof(btAddr);
        if (CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Bluetooth_DeviceAddress, &propType, (PBYTE)btAddr, &propSize, 0) != CR_SUCCESS
            || propType != DEVPROP_TYPE_STRING)
            continue;

        for (auto& dev : devices) {
            WCHAR addrStr[13];
            StringCchPrintfW(addrStr, ARRAYSIZE(addrStr), L"%02X%02X%02X%02X%02X%02X",
                dev.address[5], dev.address[4], dev.address[3],
                dev.address[2], dev.address[1], dev.address[0]);

            if (_wcsicmp(btAddr, addrStr) == 0) {
                if (battery <= 100)
                    dev.batteryPercent = battery;
                break;
            }
        }
    }
}

// Fallback: read battery from BTHLE registry DeviceBattery values
static void GetBatteryFromRegistry(std::vector<DeviceInfo>& devices) {
    for (auto& dev : devices) {
        if (dev.batteryPercent >= 0) continue;

        WCHAR addrStr[13];
        StringCchPrintfW(addrStr, ARRAYSIZE(addrStr), L"%02X%02X%02X%02X%02X%02X",
            dev.address[5], dev.address[4], dev.address[3],
            dev.address[2], dev.address[1], dev.address[0]);

        WCHAR regPath[256];
        StringCchPrintfW(regPath, ARRAYSIZE(regPath), L"SYSTEM\\CurrentControlSet\\Enum\\BTHLE\\Dev_%s\\Device Parameters", addrStr);

        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD battery = 0;
            DWORD size = sizeof(battery);
            DWORD type = 0;
            if (RegQueryValueExW(hKey, L"DeviceBattery", NULL, &type, (PBYTE)&battery, &size) == ERROR_SUCCESS
                && type == REG_DWORD && battery <= 100) {
                dev.batteryPercent = (int)battery;
            }
            RegCloseKey(hKey);
        }
    }
}

// Walk the devnode tree to see if this device has a Keyboard-class child
static bool IsKeyboardDevice(DEVINST devInst) {
    std::vector<DEVINST> stack;
    stack.push_back(devInst);
    while (!stack.empty()) {
        DEVINST current = stack.back();
        stack.pop_back();

        WCHAR className[MAX_PATH] = {};
        ULONG len = sizeof(className);
        if (CM_Get_DevNode_Registry_PropertyW(current, CM_DRP_CLASS, NULL, className, &len, 0) == CR_SUCCESS) {
            if (_wcsicmp(className, L"Keyboard") == 0) return true;
        }

        DEVINST childInst = 0;
        if (CM_Get_Child(&childInst, current, 0) == CR_SUCCESS) {
            do {
                stack.push_back(childInst);
            } while (CM_Get_Sibling(&childInst, childInst, 0) == CR_SUCCESS);
        }
    }
    return false;
}

// ─── Utilities ────────────────────────────────────────────────────────────

// RAII wrapper around CRITICAL_SECTION
struct CsLock {
    CRITICAL_SECTION& cs;
    CsLock(CRITICAL_SECTION& c) : cs(c) { EnterCriticalSection(&cs); }
    ~CsLock() { LeaveCriticalSection(&cs); }
    CsLock(const CsLock&) = delete;
    CsLock& operator=(const CsLock&) = delete;
};

// Verify a devnode has at least one active HID or BLE GATT interface.
// DN_STARTED alone is unreliable for BLE devices (e.g. Xbox controller
// stays started when powered off). An active device interface proves
// the device is physically connected and communicating.
static bool HasActiveInterface(DEVINST devInst) {

    const GUID* guids[] = { &GUID_DEVINTERFACE_HID, &GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE };

    for (const GUID* guid : guids) {
        HDEVINFO hDevs = SetupDiGetClassDevsW(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (hDevs == INVALID_HANDLE_VALUE) continue;

        SP_DEVICE_INTERFACE_DATA dia = { sizeof(SP_DEVICE_INTERFACE_DATA) };
        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevs, NULL, guid, i, &dia); i++) {
            DWORD reqSize = 0;
            SetupDiGetDeviceInterfaceDetailW(hDevs, &dia, NULL, 0, &reqSize, NULL);
            if (reqSize == 0) continue;

            std::vector<BYTE> buf(reqSize);
            PSP_DEVICE_INTERFACE_DETAIL_DATA_W detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)buf.data();
            detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

            SP_DEVINFO_DATA devData = { sizeof(SP_DEVINFO_DATA) };
            if (SetupDiGetDeviceInterfaceDetailW(hDevs, &dia, detail, reqSize, NULL, &devData)) {
                DEVINST curr = devData.DevInst;
                bool match = false;
                while (curr != 0) {
                    if (curr == devInst) {
                        match = true;
                        break;
                    }
                    DEVINST parent = 0;
                    if (CM_Get_Parent(&parent, curr, 0) != CR_SUCCESS) break;
                    curr = parent;
                }
                if (match) {
                    SetupDiDestroyDeviceInfoList(hDevs);
                    return true;
                }
            }
        }
        SetupDiDestroyDeviceInfoList(hDevs);
    }
    return false;
}

enum DeviceType { DEVICE_UNKNOWN, DEVICE_KEYBOARD, DEVICE_MOUSE, DEVICE_HEADPHONES, DEVICE_CONTROLLER };

// Classify a device by its Class of Device and keyboard flag
static DeviceType GetDeviceType(const DeviceInfo& d) {
    if (d.isKeyboard) return DEVICE_KEYBOARD;
    ULONG major = (d.classOfDevice & 0x1F00) >> 8;
    ULONG minor = d.classOfDevice & 0xFC;

    if (major == 5) {
        if (minor & 0x80) return DEVICE_MOUSE;
        if (minor & 0x40) return DEVICE_KEYBOARD;
        ULONG gamepad = (minor >> 3) & 3;
        if (gamepad == 1 || gamepad == 2) return DEVICE_CONTROLLER;
    }

    if (major == 4) return DEVICE_HEADPHONES;

    return DEVICE_UNKNOWN;
}

// Enumerate BTHLE registry entries for BLE devices, read names + battery + connection
static void EnumerateBthleDevices(std::vector<DeviceInfo>& devices, size_t maxCount) {
    HKEY hBthle;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\BTHLE", 0, KEY_READ, &hBthle) != ERROR_SUCCESS)
        return;

    WCHAR devKeyName[MAX_PATH];
    DWORD devIndex = 0;
    DWORD devKeyLen = ARRAYSIZE(devKeyName);

    while (RegEnumKeyExW(hBthle, devIndex, devKeyName, &devKeyLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        if (wcslen(devKeyName) == 16 && _wcsnicmp(devKeyName, L"Dev_", 4) == 0) {
            const WCHAR* btAddrStr = devKeyName + 4;

            HKEY hDev;
            if (RegOpenKeyExW(hBthle, devKeyName, 0, KEY_READ, &hDev) == ERROR_SUCCESS) {
                WCHAR instanceName[MAX_PATH];
                DWORD instLen = ARRAYSIZE(instanceName);

                if (RegEnumKeyExW(hDev, 0, instanceName, &instLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                    WCHAR fullId[MAX_PATH];
                    StringCchPrintfW(fullId, ARRAYSIZE(fullId), L"BTHLE\\%s\\%s", devKeyName, instanceName);

                    DEVINST devInst;
                    if (CM_Locate_DevNodeW(&devInst, fullId, CM_LOCATE_DEVNODE_NORMAL) == CR_SUCCESS) {
                        int existingIdx = -1;
                        for (size_t i = 0; i < devices.size() && existingIdx < 0; i++) {
                            WCHAR addrBuf[13];
                            StringCchPrintfW(addrBuf, ARRAYSIZE(addrBuf), L"%02X%02X%02X%02X%02X%02X",
                                devices[i].address[5], devices[i].address[4], devices[i].address[3],
                                devices[i].address[2], devices[i].address[1], devices[i].address[0]);
                            if (_wcsicmp(addrBuf, btAddrStr) == 0)
                                existingIdx = (int)i;
                        }

                        DEVPROP_BOOLEAN bConnected = DEVPROP_FALSE;
                        DEVPROPTYPE ct = DEVPROP_TYPE_EMPTY;
                        ULONG cs = sizeof(bConnected);
                        bool devStarted = false;
                        if (CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Bluetooth_IsConnected, &ct, (PBYTE)&bConnected, &cs, 0) == CR_SUCCESS
                            && ct == DEVPROP_TYPE_BOOLEAN) {
                            devStarted = (bConnected == DEVPROP_TRUE);
                        } else {
                            DEVINST childInst = 0;
                            if (CM_Get_Child(&childInst, devInst, 0) == CR_SUCCESS) {
                                do {
                                    ULONG status = 0, problem = 0;
                                    if (CM_Get_DevNode_Status(&status, &problem, childInst, 0) == CR_SUCCESS) {
                                        if ((status & DN_STARTED) && problem == 0 && HasActiveInterface(childInst)) {
                                            devStarted = true;
                                            break;
                                        }
                                    }
                                } while (CM_Get_Sibling(&childInst, childInst, 0) == CR_SUCCESS);
                            }
                            if (!devStarted) {
                                ULONG status = 0, problem = 0;
                                if (CM_Get_DevNode_Status(&status, &problem, devInst, 0) == CR_SUCCESS) {
                                    if ((status & DN_STARTED) && problem == 0 && HasActiveInterface(devInst)) {
                                        devStarted = true;
                                    }
                                }
                            }
                        }

                        DeviceInfo* di = nullptr;
                        if (existingIdx >= 0) {
                            di = &devices[existingIdx];
                        } else if (devices.size() < maxCount) {
                            devices.emplace_back();
                            di = &devices.back();
                            for (int i = 0; i < 6; i++) {
                                WCHAR byteStr[3] = { btAddrStr[i*2], btAddrStr[i*2+1], 0 };
                                di->address[5-i] = (BYTE)wcstoul(byteStr, NULL, 16);
                            }
                            di->connected = devStarted;
                            di->batteryPercent = -1;
                            di->classOfDevice = 0;
                        }

                        if (di) {
                            if (di->name.empty()) {
                                WCHAR name[128] = {};
                                DEVPROPTYPE pt = DEVPROP_TYPE_EMPTY;
                                ULONG ns = sizeof(name);
                                CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Device_FriendlyName, &pt, (PBYTE)name, &ns, 0);
                                if (pt != DEVPROP_TYPE_STRING || !name[0]) {
                                    ns = sizeof(name);
                                    CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Device_BusReportedDeviceDesc, &pt, (PBYTE)name, &ns, 0);
                                }
                                if (pt == DEVPROP_TYPE_STRING && name[0])
                                    di->name = name;
                                else
                                    di->name = btAddrStr;
                            }

                            if (di->batteryPercent < 0) {
                                BYTE battery = 0;
                                DEVPROPTYPE pt = DEVPROP_TYPE_EMPTY;
                                ULONG ps = sizeof(battery);
                                if (CM_Get_DevNode_PropertyW(devInst, &DEVPKEY_Bluetooth_BatteryLevel, &pt, (PBYTE)&battery, &ps, 0) == CR_SUCCESS
                                    && pt == DEVPROP_TYPE_BYTE
                                    && ps == sizeof(battery)
                                    && battery <= 100) {
                                    di->batteryPercent = battery;
                                }
                            }
                            
                            di->isKeyboard = IsKeyboardDevice(devInst);
                        }
                    }
                }
                RegCloseKey(hDev);
            }
        }
        devKeyLen = ARRAYSIZE(devKeyName);
        devIndex++;
    }
    RegCloseKey(hBthle);
}

// Verify each device is still connected against the Bluetooth radio
static void RefreshConnectedState(std::vector<DeviceInfo>& devices) {
    BLUETOOTH_FIND_RADIO_PARAMS params = {sizeof(params)};
    HANDLE hRadio;
    HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);
    if (!hFind) return;

    do {
        for (auto& dev : devices) {
            if (!dev.connected) continue;
            BLUETOOTH_DEVICE_INFO info = {};
            info.dwSize = sizeof(info);
            memcpy(info.Address.rgBytes, dev.address, 6);
            if (BluetoothGetDeviceInfo(hRadio, &info) != ERROR_SUCCESS)
                continue;
            if (!info.fConnected)
                dev.connected = false;
        }
        CloseHandle(hRadio);
    } while (BluetoothFindNextRadio(hFind, &hRadio));

    BluetoothFindRadioClose(hFind);
}

// Check which BLE GATT interfaces are active right now
static void RefreshBthleConnectedState(std::vector<DeviceInfo>& devices) {
    HDEVINFO hDevs = SetupDiGetClassDevsW(&GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevs == INVALID_HANDLE_VALUE) return;

    auto wcsistr = [](const WCHAR* str1, const WCHAR* str2) -> const WCHAR* {
        if (!*str2) return str1;
        for (; *str1; ++str1) {
            if (towlower(*str1) == towlower(*str2)) {
                const WCHAR* h = str1, *n = str2;
                while (*h && *n && towlower(*h) == towlower(*n)) { ++h; ++n; }
                if (!*n) return str1;
            }
        }
        return nullptr;
    };

    SP_DEVICE_INTERFACE_DATA dia = { sizeof(SP_DEVICE_INTERFACE_DATA) };
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevs, NULL, &GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE, i, &dia); i++) {
        DWORD reqSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevs, &dia, NULL, 0, &reqSize, NULL);
        if (reqSize == 0) continue;

        std::vector<BYTE> buf(reqSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)buf.data();
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        if (SetupDiGetDeviceInterfaceDetailW(hDevs, &dia, detail, reqSize, NULL, NULL)) {
            for (auto& dev : devices) {
                if (dev.connected) continue; // Already marked connected
                
                WCHAR addrStr[13];
                StringCchPrintfW(addrStr, ARRAYSIZE(addrStr), L"%02X%02X%02X%02X%02X%02X",
                    dev.address[5], dev.address[4], dev.address[3],
                    dev.address[2], dev.address[1], dev.address[0]);

                if (wcsistr(detail->DevicePath, addrStr) != NULL) {
                    dev.connected = true;
                }
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hDevs);
}

// ─── Scanner Thread ──────────────────────────────────────────────────────────

// Background polling thread — collects Bluetooth device data on interval or rescan signal
static unsigned int __stdcall ScannerProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", hrCo);
        return 1;
    }
    HANDLE events[] = { g_shutdownEvent, g_rescanEvent };

    while (true) {
        try {
            std::vector<DeviceInfo> newDevices;
            newDevices.reserve(32);

            BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = {};
            searchParams.dwSize = sizeof(searchParams);
            searchParams.fReturnAuthenticated = TRUE;
            searchParams.fReturnRemembered = FALSE;
            searchParams.fReturnConnected = TRUE;
            searchParams.fReturnUnknown = FALSE;
            searchParams.hRadio = NULL;

            BLUETOOTH_DEVICE_INFO deviceInfo = {};
            deviceInfo.dwSize = sizeof(deviceInfo);

            HBLUETOOTH_DEVICE_FIND hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
            if (hFind) {
                do {
                    if (newDevices.size() >= 32) break;
                    DeviceInfo di;
                    di.name = deviceInfo.szName;
                    memcpy(di.address, deviceInfo.Address.rgBytes, 6);
                    di.batteryPercent = -1;
                    di.connected = !!deviceInfo.fConnected;
                    di.classOfDevice = deviceInfo.ulClassofDevice;
                    di.isKeyboard = false;
                    newDevices.push_back(di);
                } while (BluetoothFindNextDevice(hFind, &deviceInfo));
                BluetoothFindDeviceClose(hFind);
            }

            EnumerateBthleDevices(newDevices, 32);
            RefreshBthleConnectedState(newDevices);
            RefreshConnectedState(newDevices);
            GetBatteryFromMediaClass(newDevices);
            GetBatteryFromRegistry(newDevices);

            {   CsLock lock(g_devicesLock);
                g_devices = std::move(newDevices);
            }

            HWND hwnd = g_hwnd.load();
            if (IsWindow(hwnd))
                PostMessageW(hwnd, WM_UPDATE_DEVICES, 0, 0);

            DWORD interval = (DWORD)g_refreshIntervalMs.load();
            DWORD wait = WaitForMultipleObjects(2, events, FALSE, interval);
            if (wait == WAIT_OBJECT_0) break;
            
            if (wait == WAIT_OBJECT_0 + 1) {
                if (WaitForSingleObject(g_shutdownEvent, 1000) == WAIT_OBJECT_0) break;
                WaitForSingleObject(g_rescanEvent, 0);
            }
        } catch (const std::bad_alloc&) {
            Wh_Log(L"Scanner allocation failure, retrying");
            if (WaitForSingleObject(g_shutdownEvent, g_refreshIntervalMs.load()) == WAIT_OBJECT_0)
                break;
        }
    }

    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// ─── Tray Icon ───────────────────────────────────────────────────────────

// Mark icons as off-limits (during shutdown, before DestroyIcons)
static void DisableIcons() { g_iconsAvailable.store(false); }

// Refresh the shell tray icon to reflect the current lowest battery
static void UpdateTrayIcon() {
    if (!g_iconsAvailable.load()) return;
    HWND hwnd = g_hwnd.load();
    if (!IsWindow(hwnd)) return;


    std::vector<DeviceInfo> currentDevices;
    {   CsLock lock(g_devicesLock);
        currentDevices = g_devices;
    }
    int lowest = -1;
    int devCount = 0;
    DeviceType singleType = DEVICE_UNKNOWN;
    for (const auto& d : currentDevices) {
        if (d.connected) {
            devCount++;
            DeviceType dt = GetDeviceType(d);
            if (devCount == 1) singleType = dt;
            if (d.batteryPercent >= 0 && (lowest < 0 || d.batteryPercent < lowest))
                lowest = d.batteryPercent;
        }
    }

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.guidItem = GUID_BTBAT_TRAY;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    
    int threshold = g_warningThreshold.load();
    if (lowest >= 0 && lowest < threshold) {
        bool flashState = (GetTickCount64() / 1000) % 2 == 0;
        nid.hIcon = flashState ? g_hIconLowBatRed : g_hIconLowBatBlack;
    } else if (devCount == 0) {
        nid.hIcon = g_hIconDisconnected;
    } else if (devCount == 1) {
        switch (singleType) {
            case DEVICE_KEYBOARD: nid.hIcon = g_hIconKeyboard; break;
            case DEVICE_MOUSE: nid.hIcon = g_hIconMouse; break;
            case DEVICE_HEADPHONES: nid.hIcon = g_hIconHeadphones; break;
            case DEVICE_CONTROLLER: nid.hIcon = g_hIconController; break;
            default: nid.hIcon = g_hIconController; break;
        }
    } else {
        nid.hIcon = g_hIconMulti;
    }

    if (devCount == 0) {
        lstrcpynW(nid.szTip, L"No connected devices", ARRAYSIZE(nid.szTip));
    } else if (devCount == 1) {
        for (const auto& d : currentDevices) {
            if (d.connected) {
                if (d.batteryPercent >= 0)
                    StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"%s — %d%%", d.name.c_str(), d.batteryPercent);
                else
                    StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"%s — ??", d.name.c_str());
                break;
            }
        }
    } else {
        if (lowest >= 0)
            StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"%d devices — lowest %d%%", devCount, lowest);
        else
            StringCchPrintfW(nid.szTip, ARRAYSIZE(nid.szTip), L"%d devices", devCount);
    }

    if (wcscmp(nid.szTip, g_lastTip) != 0 || nid.hIcon != g_lastIcon) {
        lstrcpynW(g_lastTip, nid.szTip, ARRAYSIZE(g_lastTip));
        g_lastIcon = nid.hIcon;
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }
}

// Check Windows personalization setting for dark/light theme
static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

// Apply dark/light theme to the popup context menu via uxtheme ordinals
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

// Build and show the right-click context menu with device list, rescan, and shortcuts
static void ShowPopupMenu() {
    HWND hwnd = g_hwnd.load();
    if (!IsWindow(hwnd)) return;

    HMENU hMenu = CreatePopupMenu();
    std::vector<DeviceInfo> currentDevices;
    {   CsLock lock(g_devicesLock);
        currentDevices = g_devices;
    }

    int shown = 0;
    int id = 1;
    for (const auto& dev : currentDevices) {
        if (!dev.connected) continue;
        WCHAR item[256];
        if (dev.batteryPercent >= 0)
            StringCchPrintfW(item, ARRAYSIZE(item), L"%s\t%d%%", dev.name.c_str(), dev.batteryPercent);
        else
            StringCchPrintfW(item, ARRAYSIZE(item), L"%s\t--", dev.name.c_str());
        AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, id++, item);
        shown++;
    }

    if (shown == 0)
        AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, L"No connected devices");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_RESCAN, L"Rescan Now");
    AppendMenuW(hMenu, MF_STRING, IDM_BT_SETTINGS, L"Bluetooth Settings");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_SETTINGS, L"Mod Settings...");

    MENUITEMINFOW miiWH = {sizeof(miiWH)};
    miiWH.fMask      = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
    miiWH.wID        = MENU_OPEN_WINDHAWK;
    miiWH.dwTypeData = (LPWSTR)L"Open WindHawk";
    miiWH.hbmpItem   = g_hWindHawkBmp;
    InsertMenuItemW(hMenu, (UINT)-1, TRUE, &miiWH);

    POINT pt;
    GetCursorPos(&pt);

    bool dark = IsSystemDarkMode();
    ApplyContextMenuTheme(hwnd, dark);
    SetForegroundWindow(hwnd);

    UINT cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
        pt.x, pt.y, 0, hwnd, NULL);
    PostMessageW(hwnd, WM_NULL, 0, 0); // focus restoration workaround after TrackPopupMenu
    DestroyMenu(hMenu);

    switch (cmd) {
        case IDM_RESCAN:
            if (g_rescanEvent) SetEvent(g_rescanEvent);
            break;
        case IDM_BT_SETTINGS:
            ShellExecuteW(NULL, L"open", L"ms-settings:bluetooth", NULL, NULL, SW_SHOW);
            break;
        case MENU_OPEN_SETTINGS: {
            HWND dh = (HWND)InterlockedCompareExchangePointer((PVOID*)&g_dashboardHwnd, nullptr, nullptr);
            if (dh && IsWindow(dh)) {
                SetForegroundWindow(dh);
                break;
            }
            HANDLE hOldThread = (HANDLE)InterlockedCompareExchangePointer(
                (PVOID*)&g_guiThread, NULL, g_guiThread);
            if (hOldThread) {
                dh = (HWND)InterlockedExchangePointer((PVOID*)&g_dashboardHwnd, nullptr);
                if (dh && IsWindow(dh)) { PostMessageW(dh, WM_CLOSE, 0, 0); }
                WaitForSingleObject(hOldThread, 3000);
                CloseHandle(hOldThread);
            }
            g_guiThread = BTBatGui::LaunchDashboard(hwnd);
            break;
        }
        case MENU_OPEN_WINDHAWK: {
            SHELLEXECUTEINFOW sei = {sizeof(sei)};
            sei.lpFile = g_windhawkPath;
            sei.nShow  = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
            break;
        }
    }
}

// ─── Tray Window Proc ────────────────────────────────────────────────────────

// Message handler for the hidden tray window — receives TaskbarCreated, timer, device change events
static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_taskbarCreatedMsg != 0 && msg == g_taskbarCreatedMsg) {
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.guidItem = GUID_BTBAT_TRAY;
        nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_GUID | NIF_SHOWTIP;
        nid.hWnd = hWnd;
        nid.uCallbackMessage = WM_TRAY_CALLBACK;
        nid.hIcon = g_hIconDisconnected ? g_hIconDisconnected : g_hIconMulti;
        lstrcpynW(nid.szTip, L"BT Battery Monitor", ARRAYSIZE(nid.szTip));
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            Shell_NotifyIconW(NIM_DELETE, &nid);
            Shell_NotifyIconW(NIM_ADD, &nid);
        }
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
        PostMessageW(hWnd, WM_UPDATE_DEVICES, 0, 0);
        return 0;
    }

    switch (msg) {
        case WM_TRAY_CALLBACK:
            if (lParam == WM_LBUTTONUP || lParam == NIN_SELECT) {
                if (g_rescanEvent) SetEvent(g_rescanEvent);
            } else if (lParam == WM_CONTEXTMENU) {
                ShowPopupMenu();
            }
            return 0;

        case WM_UPDATE_DEVICES:
            UpdateTrayIcon();
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                UpdateTrayIcon();
            }
            return 0;

        case WM_DEVICECHANGE:
            if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
                SetEvent(g_rescanEvent);
            }
            return 0;

        case WM_RELOAD_ALL:
            g_lastIcon = NULL;
            g_lastTip[0] = L'\0';
            CreateIcons();
            UpdateTrayIcon();
            return 0;

        case WM_DESTROY: {
            KillTimer(hWnd, 1);
            NOTIFYICONDATAW nid = {sizeof(nid)};
            nid.guidItem = GUID_BTBAT_TRAY;
            nid.uFlags = NIF_GUID;
            Shell_NotifyIconW(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Settings Persistence ─────────────────────────────────────────────────

// Load a device icon from storage (custom .ico path or ddores.dll preset)
static HICON LoadCustomIcon(PCWSTR storageKey, PCWSTR defaultDll, int defaultIndex) {
    WCHAR iconKey[32] = {};
    HICON hIcon = nullptr;
    if (Wh_GetStringValue(storageKey, iconKey, 32) && iconKey[0]) {
        if (wcscmp(iconKey, L"custom") == 0) {
            WCHAR customKey[64];
            swprintf_s(customKey, L"%s_custom_path", storageKey);
            WCHAR path[MAX_PATH] = {};
            if (Wh_GetStringValue(customKey, path, MAX_PATH) && path[0]) {
                if (IsValidIconPath(path))
                    hIcon = (HICON)LoadImageW(NULL, path, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            }
        } else if (wcscmp(iconKey, L"unknown") == 0) {
            return CreateXIcon(16);
        } else {
            int index = GetIconIndex(iconKey);
            ExtractIconExW(defaultDll, index, nullptr, &hIcon, 1);
        }
    }
    if (!hIcon) {
        ExtractIconExW(defaultDll, defaultIndex, nullptr, &hIcon, 1);
    }
    return hIcon;
}

// Read an integer setting from Windhawk string storage with a fallback default
static int ReadIntStorage(PCWSTR storageKey, int defaultVal) {
    WCHAR buf[32] = {};
    if (Wh_GetStringValue(storageKey, buf, 32) && buf[0]) {
        return wcstol(buf, nullptr, 10);
    }
    return defaultVal;
}

// Load all persisted settings from Windhawk storage and update cached globals + icon handles
static void LoadSettings() {
    LONG val = ReadIntStorage(L"refreshIntervalMs", 10000);
    if (val < 1000) val = 1000;
    if (val > 3600000) val = 3600000;
    g_refreshIntervalMs.store(val);

    int threshold = ReadIntStorage(L"warningThreshold", 30);
    if (threshold < 0) threshold = 0;
    if (threshold > 100) threshold = 100;
    g_warningThreshold.store(threshold);

    WCHAR sysPath[MAX_PATH];
    WCHAR ddoresPath[MAX_PATH] = L"ddores.dll";
    if (GetSystemDirectoryW(sysPath, MAX_PATH)) {
        StringCchCopyW(ddoresPath, MAX_PATH, sysPath);
        StringCchCatW(ddoresPath, MAX_PATH, L"\\ddores.dll");
    }

    if (g_hIconKeyboard) DestroyIcon(g_hIconKeyboard);
    g_hIconKeyboard = LoadCustomIcon(L"d_iconKeyboard", ddoresPath, 30);
    
    if (g_hIconMouse) DestroyIcon(g_hIconMouse);
    g_hIconMouse = LoadCustomIcon(L"d_iconMouse", ddoresPath, 110);
    
    if (g_hIconHeadphones) DestroyIcon(g_hIconHeadphones);
    g_hIconHeadphones = LoadCustomIcon(L"d_iconHeadphones", ddoresPath, 91);
    
    if (g_hIconController) DestroyIcon(g_hIconController);
    g_hIconController = LoadCustomIcon(L"d_iconController", ddoresPath, 108);

    if (g_hIconMulti) DestroyIcon(g_hIconMulti);
    g_hIconMulti = LoadCustomIcon(L"d_iconMultiple", ddoresPath, 94);

    WCHAR discoVal[32] = {};
    if (Wh_GetStringValue(L"d_iconDisconnected", discoVal, 32) && discoVal[0]) {
        if (g_hIconDisconnected) DestroyIcon(g_hIconDisconnected);
        g_hIconDisconnected = LoadCustomIcon(L"d_iconDisconnected", ddoresPath, 130);
    } else {
        // No user configuration — let CreateIcons() use the programmatic red X
        if (g_hIconDisconnected) DestroyIcon(g_hIconDisconnected);
        g_hIconDisconnected = nullptr;
    }
}

namespace BTBatGui {

    // ─── Settings Dashboard (GUI) ──────────────────────────────────────────────

    // ── Palette ───────────────────────────────────────────────────────────────
    static const COLORREF kClrBg      = RGB(24, 24, 24);
    static const COLORREF kClrSurface = RGB(36, 36, 36);
    static const COLORREF kClrBorder  = RGB(56, 56, 56);
    static const COLORREF kClrText    = RGB(224, 224, 224);
    static const COLORREF kClrDim     = RGB(128, 128, 128);
    static const COLORREF kClrInput   = RGB(28, 28, 28);
    static const COLORREF kClrAccent  = RGB(0, 120, 212);
    static const COLORREF kClrAccentP = RGB(0,  96, 170);
    static const COLORREF kClrDarkBtn = RGB(44, 44, 44);
    static const COLORREF kClrDarkBP  = RGB(32, 32, 32);

    // ── Layout ────────────────────────────────────────────────────────────────
    static const int kWinW   = 440;
    static const int kTopH   =  72;
    static const int kRowH   =  72;
    static const int kIconSz =  28;
    static const int kIconX  =  14;
    static const int kComboW = 280;

    static const WCHAR* kIconKeys[] = {
        L"keyboard", L"mouse", L"headphones",
        L"controller", L"multiple", L"unknown", L"custom"
    };
    static const WCHAR* kIconLabels[] = {
        L"Keyboard Preset", L"Mouse Preset", L"Headphones Preset",
        L"Controller Preset", L"Multiple Devices Preset", L"No Connected Devices Preset", L"Custom Icon..."
    };
    static const int kIconCount = 7;

    static const WCHAR* kDefaultIconKeys[] = {
        L"keyboard", L"mouse", L"headphones",
        L"controller", L"multiple", L"unknown"
    };

    static const WCHAR* kDeviceLabels[] = {
        L"Keyboard", L"Mouse", L"Headphones",
        L"Controller", L"Multiple Devices", L"No Connected Devices"
    };
    static const WCHAR* kStorageKeys[] = {
        L"d_iconKeyboard", L"d_iconMouse", L"d_iconHeadphones",
        L"d_iconController", L"d_iconMultiple", L"d_iconDisconnected"
    };

    struct State {
        HWND hTrayHwnd = nullptr;
        HWND hMainWnd  = nullptr;
        HFONT  hFont      = nullptr;
        HBRUSH hBgBrush   = nullptr;
        HBRUSH hInpBrush  = nullptr;
        HBRUSH hSurfBrush = nullptr;

        int refreshIntervalIdx = 2;  // default: 10s
        int warningThresholdIdx = 3; // default: 30%

        struct IconRow {
            HWND  hIconCombo   = nullptr;
            HWND  hBrowseBtn   = nullptr;
            HICON hPreviewIcon = nullptr;
            std::wstring iconKey;
            WCHAR customPath[MAX_PATH] = {};
        } rows[6];

        HWND hRefreshCombo = nullptr;
        HWND hThresholdCombo = nullptr;
        HWND hSaveBtn   = nullptr;
        HWND hCancelBtn = nullptr;
        HWND hKoFiBtn   = nullptr;

        UINT dpi = 96;
    };

    static int Sc(int px, UINT dpi) { return MulDiv(px, (int)dpi, 96); }

    static int RowY(int i, UINT dpi) { return Sc(kTopH, dpi) + i * Sc(kRowH, dpi); }

    static HICON LoadSlotPreview(const std::wstring& key, const WCHAR* customPath) {
        HICON h = nullptr;
        if (key == L"unknown") {
            h = CreateXIcon(kIconSz);
        } else if (key == L"custom" && customPath && customPath[0] && IsValidIconPath(customPath))
            h = (HICON)LoadImageW(NULL, customPath, IMAGE_ICON, kIconSz, kIconSz, LR_LOADFROMFILE);
        if (!h) {
            WCHAR sysPath[MAX_PATH];
            if (GetSystemDirectoryW(sysPath, MAX_PATH)) {
                WCHAR ddoresPath[MAX_PATH];
                StringCchCopyW(ddoresPath, MAX_PATH, sysPath);
                StringCchCatW(ddoresPath, MAX_PATH, L"\\ddores.dll");
                ExtractIconExW(ddoresPath, GetIconIndex(key.empty() ? nullptr : key.c_str()), nullptr, &h, 1);
            }
        }
        return h;
    }

    static void DarkCombo(HWND h) {
        bool loaded = false;
        HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
        if (!ux) { ux = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32); loaded = true; }
        if (!ux) return;
        using Fn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
        auto fn = reinterpret_cast<Fn>(GetProcAddress(ux, "SetWindowTheme"));
        if (fn) fn(h, L"DarkMode_CFD", nullptr);
        if (loaded) FreeLibrary(ux);
    }

    static void LayoutControls(State* s) {
        if (!s->hMainWnd) return;
        UINT d = s->dpi;

        // Top row
        SetWindowPos(s->hRefreshCombo,  nullptr, Sc(72, d), Sc(18, d), Sc(130, d), Sc(200, d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hThresholdCombo, nullptr, Sc(268, d), Sc(18, d), Sc(90, d),  Sc(200, d), SWP_NOZORDER|SWP_NOACTIVATE);

        for (int i = 0; i < 6; i++) {
            int y = RowY(i, d);
            SetWindowPos(s->rows[i].hIconCombo, nullptr, Sc(kIconX + kIconSz + 10, d), y + Sc(22, d), Sc(kComboW, d), Sc(300, d), SWP_NOZORDER|SWP_NOACTIVATE);
            bool showBr = s->rows[i].iconKey == L"custom";
            SetWindowPos(s->rows[i].hBrowseBtn, nullptr, Sc(kIconX + kIconSz + 10 + kComboW + 6, d), y + Sc(48, d), Sc(60, d), Sc(24, d), SWP_NOZORDER|SWP_NOACTIVATE);
            ShowWindow(s->rows[i].hBrowseBtn, showBr ? SW_SHOW : SW_HIDE);
        }

        int btnY = RowY(5, d) + Sc(kRowH, d) + Sc(8, d);
        SetWindowPos(s->hSaveBtn,   nullptr, Sc(20, d),   btnY, Sc(130, d), Sc(28, d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hCancelBtn, nullptr, Sc(158, d),  btnY, Sc(88, d),  Sc(28, d), SWP_NOZORDER|SWP_NOACTIVATE);
        SetWindowPos(s->hKoFiBtn,   nullptr, Sc(254, d),  btnY, Sc(140, d), Sc(28, d), SWP_NOZORDER|SWP_NOACTIVATE);

        int clientH = btnY + Sc(28, d) + Sc(12, d);
        RECT rc = {0, 0, Sc(kWinW, d), clientH};
        AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE, WS_EX_DLGMODALFRAME);
        RECT wr; GetWindowRect(s->hMainWnd, &wr);
        SetWindowPos(s->hMainWnd, nullptr, wr.left, wr.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    static BOOL CALLBACK ApplyFontProc(HWND child, LPARAM hf) {
        SendMessageW(child, WM_SETFONT, (WPARAM)hf, TRUE);
        return TRUE;
    }

    static void LoadState(State& s) {
        // Read current settings from storage
        int refreshDefs[] = {1000, 5000, 10000, 30000, 60000, 300000, 3600000};
        s.refreshIntervalIdx = 2; // default 10s

        int currentRefresh = ReadIntStorage(L"refreshIntervalMs", 10000);
        for (int i = 0; i < 7; i++) {
            if (currentRefresh == refreshDefs[i]) { s.refreshIntervalIdx = i; break; }
        }

        int currentThreshold = ReadIntStorage(L"warningThreshold", 30);
        int threshVals[] = {0, 10, 20, 30, 40, 50};
        s.warningThresholdIdx = 3; // default 30
        for (int i = 0; i < 6; i++) {
            if (currentThreshold == threshVals[i]) { s.warningThresholdIdx = i; break; }
        }

        for (int i = 0; i < 6; i++) {
            WCHAR icoKey[32] = {};
            WCHAR customKey[64];
            swprintf_s(customKey, L"%s_custom_path", kStorageKeys[i]);
            if (Wh_GetStringValue(kStorageKeys[i], icoKey, 32) && icoKey[0]) {
                s.rows[i].iconKey = icoKey;
            } else {
                // Fall back to preset default
                s.rows[i].iconKey = kDefaultIconKeys[i];
            }
            Wh_GetStringValue(customKey, s.rows[i].customPath, MAX_PATH);
            s.rows[i].hPreviewIcon = LoadSlotPreview(s.rows[i].iconKey, s.rows[i].customPath);
        }
    }

    static LRESULT CALLBACK DashboardWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        State* s = reinterpret_cast<State*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        switch (msg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            s = reinterpret_cast<State*>(cs->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s));
            s->hMainWnd = hWnd;
            s->dpi = GetDpiForWindow(hWnd);

            LOGFONTW lf = {};
            lf.lfHeight = -MulDiv(10, (int)s->dpi, 72);
            lf.lfWeight = FW_NORMAL;
            lf.lfQuality = CLEARTYPE_QUALITY;
            lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
            s->hFont = CreateFontIndirectW(&lf);
            s->hBgBrush   = CreateSolidBrush(kClrBg);
            s->hInpBrush  = CreateSolidBrush(kClrInput);
            s->hSurfBrush = CreateSolidBrush(kClrSurface);

            HINSTANCE hInst = GetModuleHandleW(nullptr);

            // Refresh interval combo
            s->hRefreshCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 0, 0, 100, 200,
                hWnd, (HMENU)(UINT_PTR)100, hInst, nullptr);
            static const WCHAR* kRefreshLabels[] = {
                L"1 second", L"5 seconds", L"10 seconds", L"30 seconds",
                L"1 minute", L"5 minutes", L"1 hour"
            };
            for (int i = 0; i < 7; i++)
                SendMessageW(s->hRefreshCombo, CB_ADDSTRING, 0, (LPARAM)kRefreshLabels[i]);
            SendMessageW(s->hRefreshCombo, CB_SETCURSEL, s->refreshIntervalIdx, 0);
            DarkCombo(s->hRefreshCombo);

            // Threshold combo
            s->hThresholdCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 0, 0, 80, 200,
                hWnd, (HMENU)(UINT_PTR)101, hInst, nullptr);
            static const WCHAR* kThreshLabels[] = {
                L"0%", L"10%", L"20%", L"30%", L"40%", L"50%"
            };
            for (int i = 0; i < 6; i++)
                SendMessageW(s->hThresholdCombo, CB_ADDSTRING, 0, (LPARAM)kThreshLabels[i]);
            SendMessageW(s->hThresholdCombo, CB_SETCURSEL, s->warningThresholdIdx, 0);
            DarkCombo(s->hThresholdCombo);

            // Per-device rows
            for (int i = 0; i < 6; i++) {
                s->rows[i].hIconCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr,
                    WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, 0, 0, 10, 300,
                    hWnd, (HMENU)(UINT_PTR)(200+i), hInst, nullptr);
                int isel = 0;
                for (int j = 0; j < kIconCount; j++) {
                    SendMessageW(s->rows[i].hIconCombo, CB_ADDSTRING, 0, (LPARAM)kIconLabels[j]);
                    if (s->rows[i].iconKey == kIconKeys[j]) isel = j;
                }
                SendMessageW(s->rows[i].hIconCombo, CB_SETCURSEL, isel, 0);
                DarkCombo(s->rows[i].hIconCombo);

                s->rows[i].hBrowseBtn = CreateWindowExW(0, L"BUTTON", L"Browse...",
                    WS_CHILD|BS_OWNERDRAW, 0, 0, 10, 10,
                    hWnd, (HMENU)(UINT_PTR)(300+i), hInst, nullptr);
            }

            // Bottom buttons
            s->hSaveBtn   = CreateWindowExW(0, L"BUTTON", L"Save and Apply",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)IDOK, hInst, nullptr);
            s->hCancelBtn = CreateWindowExW(0, L"BUTTON", L"Cancel",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)IDCANCEL, hInst, nullptr);
            s->hKoFiBtn   = CreateWindowExW(0, L"BUTTON", L"Buy Me Coffee",
                WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 0, 0, 10, 10,
                hWnd, (HMENU)(UINT_PTR)9002, hInst, nullptr);

            EnumChildWindows(hWnd, ApplyFontProc, reinterpret_cast<LPARAM>(s->hFont));
            LayoutControls(s);

            // Dark title bar — DWMWA_USE_IMMERSIVE_DARK_MODE is 20 on Win11 24H2+
            bool dwmLoaded = false;
            HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
            if (!dwm) { dwm = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32); dwmLoaded = true; }
            if (dwm) {
                using DwmFn = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
                auto fn = reinterpret_cast<DwmFn>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
                if (fn) { BOOL dark = TRUE; fn(hWnd, 20, &dark, sizeof(dark)); }
                if (dwmLoaded) FreeLibrary(dwm);
            }
            return 0;
        }

        case WM_ERASEBKGND: {
            RECT rc; GetClientRect(hWnd, &rc);
            FillRect((HDC)wParam, &rc, s ? s->hBgBrush : (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }

        case WM_PAINT: {
            if (!s) { PAINTSTRUCT ps; BeginPaint(hWnd, &ps); EndPaint(hWnd, &ps); return 0; }
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SelectObject(hdc, s->hFont);
            SetBkMode(hdc, TRANSPARENT);
            UINT d = s->dpi;

            // Top section labels
            SetTextColor(hdc, kClrDim);
            RECT r;
            r = {Sc(12, d), Sc(22, d), Sc(70, d), Sc(38, d)};
            DrawTextW(hdc, L"Polling:", -1, &r, DT_LEFT|DT_TOP);
            r = {Sc(216, d), Sc(22, d), Sc(266, d), Sc(38, d)};
            DrawTextW(hdc, L"Warning:", -1, &r, DT_LEFT|DT_TOP);

            // Separator
            {
                HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                HPEN op = (HPEN)SelectObject(hdc, p);
                MoveToEx(hdc, Sc(12, d), Sc(kTopH, d) - Sc(6, d), nullptr);
                LineTo(hdc, Sc(kWinW - 12, d), Sc(kTopH, d) - Sc(6, d));
                SelectObject(hdc, op); DeleteObject(p);
            }

            // Per-device rows
            for (int i = 0; i < 6; i++) {
                int y = RowY(i, d);

                RECT hdr = {0, y, Sc(kWinW, d), y + Sc(20, d)};
                FillRect(hdc, &hdr, s->hSurfBrush);

                // Device label
                SetTextColor(hdc, kClrText);
                RECT dl = {Sc(kIconX, d), y + Sc(2, d), Sc(kWinW - kIconX, d), y + Sc(18, d)};
                DrawTextW(hdc, kDeviceLabels[i], -1, &dl, DT_LEFT|DT_TOP);

                // Preview icon
                if (s->rows[i].hPreviewIcon)
                    DrawIconEx(hdc, Sc(kIconX, d), y + Sc(22, d), s->rows[i].hPreviewIcon,
                               Sc(kIconSz, d), Sc(kIconSz, d), 0, nullptr, DI_NORMAL);

                // Icon label
                SetTextColor(hdc, kClrDim);
                RECT il = {Sc(kIconX, d), y + Sc(52, d), Sc(kIconX + 60, d), y + Sc(66, d)};
                DrawTextW(hdc, L"Icon:", -1, &il, DT_LEFT|DT_TOP);

                if (i < 5) {
                    HPEN p = CreatePen(PS_SOLID, 1, kClrBorder);
                    HPEN op = (HPEN)SelectObject(hdc, p);
                    MoveToEx(hdc, 0, y + Sc(kRowH, d) - 1, nullptr);
                    LineTo(hdc, Sc(kWinW, d), y + Sc(kRowH, d) - 1);
                    SelectObject(hdc, op); DeleteObject(p);
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
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

        case WM_DRAWITEM: {
            auto* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            if (dis->CtlType != ODT_BUTTON || !s) break;
            bool pressed = (dis->itemState & ODS_SELECTED) != 0;
            bool isSave  = (dis->CtlID == IDOK);

            COLORREF bg;
            if      (isSave)   bg = pressed ? kClrAccentP : kClrAccent;
            else               bg = pressed ? kClrDarkBP  : kClrDarkBtn;

            HBRUSH hFill = CreateSolidBrush(bg);
            FillRect(dis->hDC, &dis->rcItem, hFill);
            DeleteObject(hFill);

            HPEN hPen = CreatePen(PS_SOLID, 1, isSave ? kClrAccentP : kClrBorder);
            HPEN hOp  = (HPEN)SelectObject(dis->hDC, hPen);
            SelectObject(dis->hDC, GetStockObject(NULL_BRUSH));
            Rectangle(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);
            SelectObject(dis->hDC, hOp); DeleteObject(hPen);

            WCHAR txt[64] = {}; GetWindowTextW(dis->hwndItem, txt, 64);
            SetTextColor(dis->hDC, kClrText);
            SetBkMode(dis->hDC, TRANSPARENT);
            SelectObject(dis->hDC, s->hFont);
            DrawTextW(dis->hDC, txt, -1, &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (dis->itemState & ODS_FOCUS) DrawFocusRect(dis->hDC, &dis->rcItem);
            return TRUE;
        }

        case WM_COMMAND: {
            if (!s) return 0;
            int id = LOWORD(wParam);

            if (id >= 200 && id < 206 && HIWORD(wParam) == CBN_SELCHANGE) {
                int slot = id - 200;
                int sel = (int)SendMessageW(s->rows[slot].hIconCombo, CB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < kIconCount) s->rows[slot].iconKey = kIconKeys[sel];
                if (s->rows[slot].hPreviewIcon) { DestroyIcon(s->rows[slot].hPreviewIcon); }
                s->rows[slot].hPreviewIcon = LoadSlotPreview(s->rows[slot].iconKey, s->rows[slot].customPath);
                bool showBr = s->rows[slot].iconKey == L"custom";
                ShowWindow(s->rows[slot].hBrowseBtn, showBr ? SW_SHOW : SW_HIDE);
                LayoutControls(s);
                int y = RowY(slot, s->dpi);
                RECT ir = {Sc(kIconX, s->dpi), y + Sc(20, s->dpi),
                           Sc(kIconX + kIconSz + 2, s->dpi), y + Sc(20 + kIconSz + 2, s->dpi)};
                InvalidateRect(hWnd, &ir, TRUE);

            } else if (id >= 300 && id < 306) {
                int slot = id - 300;
                WCHAR path[MAX_PATH] = {};
                lstrcpynW(path, s->rows[slot].customPath, MAX_PATH);
                WCHAR title[64]; swprintf_s(title, L"Select Icon for %s", kDeviceLabels[slot]);
                OPENFILENAMEW ofn = {sizeof(ofn)};
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter  = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFile    = path;
                ofn.nMaxFile     = MAX_PATH;
                ofn.lpstrTitle   = title;
                ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR;
                if (GetOpenFileNameW(&ofn)) {
                    lstrcpynW(s->rows[slot].customPath, path, MAX_PATH);
                    if (s->rows[slot].hPreviewIcon) { DestroyIcon(s->rows[slot].hPreviewIcon); }
                    s->rows[slot].hPreviewIcon = LoadSlotPreview(s->rows[slot].iconKey, s->rows[slot].customPath);
                    int y = RowY(slot, s->dpi);
                    RECT ir = {Sc(kIconX, s->dpi), y + Sc(20, s->dpi),
                               Sc(kIconX + kIconSz + 2, s->dpi), y + Sc(20 + kIconSz + 2, s->dpi)};
                    InvalidateRect(hWnd, &ir, TRUE);
                }

            } else if (id == IDOK) {
                // Save settings
                int refreshVals[] = {1000, 5000, 10000, 30000, 60000, 300000, 3600000};
                int ri = (int)SendMessageW(s->hRefreshCombo, CB_GETCURSEL, 0, 0);
                if (ri != CB_ERR && ri < 7) {
                    WCHAR num[16];
                    swprintf_s(num, L"%d", refreshVals[ri]);
                    Wh_SetStringValue(L"refreshIntervalMs", num);
                }

                int threshVals[] = {0, 10, 20, 30, 40, 50};
                int ti = (int)SendMessageW(s->hThresholdCombo, CB_GETCURSEL, 0, 0);
                if (ti != CB_ERR && ti < 6) {
                    WCHAR num[16];
                    swprintf_s(num, L"%d", threshVals[ti]);
                    Wh_SetStringValue(L"warningThreshold", num);
                }

                for (int i = 0; i < 6; i++) {
                    int is = (int)SendMessageW(s->rows[i].hIconCombo, CB_GETCURSEL, 0, 0);
                    if (is >= 0 && is < kIconCount) s->rows[i].iconKey = kIconKeys[is];
                    Wh_SetStringValue(kStorageKeys[i], s->rows[i].iconKey.c_str());
                    WCHAR customKey[64];
                    swprintf_s(customKey, L"%s_custom_path", kStorageKeys[i]);
                    Wh_SetStringValue(customKey, s->rows[i].customPath);
                }

                if (s->hTrayHwnd && IsWindow(s->hTrayHwnd)) PostMessageW(s->hTrayHwnd, WM_RELOAD_ALL, 0, 0);
                DestroyWindow(hWnd);

            } else if (id == IDCANCEL) {
                DestroyWindow(hWnd);
            } else if (id == 9002) {
                ShellExecuteW(nullptr, L"open", L"https://ko-fi.com/blackpaw21",
                              nullptr, nullptr, SW_SHOWNORMAL);
            }
            return 0;
        }

        case WM_DPICHANGED: {
            if (!s) return 0;
            s->dpi = HIWORD(wParam);
            DeleteObject(s->hFont);
            LOGFONTW lf = {};
            lf.lfHeight = -MulDiv(10, (int)s->dpi, 72);
            lf.lfWeight = FW_NORMAL;
            lf.lfQuality = CLEARTYPE_QUALITY;
            lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
            s->hFont = CreateFontIndirectW(&lf);
            EnumChildWindows(hWnd, ApplyFontProc, (LPARAM)s->hFont);
            const RECT* prc = reinterpret_cast<const RECT*>(lParam);
            SetWindowPos(hWnd, nullptr, prc->left, prc->top,
                         prc->right - prc->left, prc->bottom - prc->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            LayoutControls(s);
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }

        case WM_DESTROY:
            InterlockedExchangePointer((volatile PVOID*)&g_dashboardHwnd, nullptr);
            if (s) {
                DeleteObject(s->hFont);     s->hFont     = nullptr;
                DeleteObject(s->hBgBrush);  s->hBgBrush  = nullptr;
                DeleteObject(s->hInpBrush); s->hInpBrush = nullptr;
                DeleteObject(s->hSurfBrush);s->hSurfBrush= nullptr;
                for (int i = 0; i < 6; i++) {
                    if (s->rows[i].hPreviewIcon) {
                        DestroyIcon(s->rows[i].hPreviewIcon);
                        s->rows[i].hPreviewIcon = nullptr;
                    }
                }
            }
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    // Dashboard window thread — creates the Win32 GUI, runs message loop, cleans up on close
    static unsigned int __stdcall GuiThreadProc(void* lpParam) {
        HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
            Wh_Log(L"CoInitializeEx failed (0x%X)", hrCo);
            return 1;
        }

        using SetTDACFn = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        if (auto fn = (SetTDACFn)GetProcAddress(
                GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"))
            fn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        static const WCHAR kClass[] = L"BTBatDashboard";
        HINSTANCE hInst = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc   = {sizeof(wc)};
        wc.lpfnWndProc   = DashboardWndProc;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = kClass;
        RegisterClassExW(&wc);

        State state;
        state.hTrayHwnd = reinterpret_cast<HWND>(lpParam);
        LoadState(state);

        int cxScr = GetSystemMetrics(SM_CXSCREEN);
        int cyScr = GetSystemMetrics(SM_CYSCREEN);
        int sx = cxScr / 4;
        int sy = cyScr / 4;

        HWND hWnd = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            kClass, L"BT Battery Monitor Settings",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            sx, sy, cxScr / 2, cyScr / 2,
            nullptr, nullptr, hInst, &state);

        if (hWnd) {
            InterlockedExchangePointer((volatile PVOID*)&g_dashboardHwnd, hWnd);
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

        for (int i = 0; i < 6; i++) {
            if (state.rows[i].hPreviewIcon) {
                DestroyIcon(state.rows[i].hPreviewIcon);
                state.rows[i].hPreviewIcon = nullptr;
            }
        }

        UnregisterClassW(kClass, hInst);
        if (SUCCEEDED(hrCo)) CoUninitialize();
        InterlockedExchange(&g_guiRunning, 0);
        return 0;
    }

    // Spawn the dashboard GUI thread (single-instance guarded by g_guiRunning)
    HANDLE LaunchDashboard(HWND hTrayHwnd) {
        if (InterlockedCompareExchange(&g_guiRunning, 1, 0) != 0)
            return nullptr;
        HANDLE h = (HANDLE)_beginthreadex(nullptr, 0, GuiThreadProc,
                                reinterpret_cast<LPVOID>(hTrayHwnd), 0, nullptr);
        if (!h) InterlockedExchange(&g_guiRunning, 0);
        return h;
    }

} // namespace BTBatGui

// ─── Tray Thread ─────────────────────────────────────────────────────────

// Hidden window thread — registers the tray icon, listens for events, runs message pump
static unsigned int __stdcall TrayThreadProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", hrCo);
        return 1;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"BTBatTrayWindow";
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassW failed (%u)", GetLastError());
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"BTBatTrayWindow",
        L"BT Battery Monitor",
        WS_POPUP,
        0, 0, 1, 1,
        NULL, NULL,
        wc.hInstance,
        NULL);

    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed (%u)", GetLastError());
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }
    g_hwnd.store(hWnd);
    SetTimer(hWnd, 1, 1000, NULL);

    // AUMID for taskbar grouping (must use wchar_t[] to avoid -Wwritable-strings)
    wchar_t aumid[] = L"BlackPaw.BTBatteryMonitor";
    PROPVARIANT pvAumid = {};
    pvAumid.vt = VT_LPWSTR;
    pvAumid.pwszVal = aumid;
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps)))) {
        if (SUCCEEDED(pps->SetValue(PKEY_AppUserModel_ID, pvAumid)))
            pps->Commit();
        pps->Release();
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.guidItem = GUID_BTBAT_TRAY;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_GUID | NIF_SHOWTIP;
    nid.hWnd = hWnd;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    nid.hIcon = g_hIconDisconnected ? g_hIconDisconnected : g_hIconMulti;
    lstrcpynW(nid.szTip, L"BT Battery Monitor", ARRAYSIZE(nid.szTip));
    if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
        Shell_NotifyIconW(NIM_DELETE, &nid);
        Shell_NotifyIconW(NIM_ADD, &nid);
    }
    nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nid);

    DEV_BROADCAST_DEVICEINTERFACE_W filter = { sizeof(filter) };
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    
    filter.dbcc_classguid = GUID_DEVINTERFACE_HID;
    g_hNotifyHid = RegisterDeviceNotificationW(hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
    
    filter.dbcc_classguid = GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE;
    g_hNotifyGatt = RegisterDeviceNotificationW(hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    filter.dbcc_classguid = GUID_BTBAT_BTHPORT;
    g_hNotifyBthPort = RegisterDeviceNotificationW(hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    PostMessageW(hWnd, WM_UPDATE_DEVICES, 0, 0);
    SetEvent(g_rescanEvent);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hwnd.store(nullptr);

    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// ─── Mod Lifecycle ───────────────────────────────────────────────────────

static LONG g_initComplete = FALSE;

// Windhawk tool mod entry point — create threads, load icons, start scanning
BOOL WhTool_ModInit() {
    if (InterlockedCompareExchange(&g_initComplete, TRUE, FALSE))
        return TRUE;

    Wh_Log(L"Initializing");

    INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icex);

    DWORD len = GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));
    switch (len) {
        case 0: return FALSE;
        case ARRAYSIZE(g_windhawkPath): return FALSE;
    }

    if (!CreateIcons()) {
        Wh_Log(L"Failed to create icons");
        return FALSE;
    }

    InitializeCriticalSection(&g_devicesLock);

    g_shutdownEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_rescanEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    if (!g_shutdownEvent || !g_rescanEvent) {
        Wh_Log(L"Failed to create events (%u)", GetLastError());
        if (g_shutdownEvent) CloseHandle(g_shutdownEvent);
        if (g_rescanEvent) CloseHandle(g_rescanEvent);
        g_shutdownEvent = g_rescanEvent = NULL;
        DeleteCriticalSection(&g_devicesLock);
        DestroyIcons();
        return FALSE;
    }

    g_scannerThread = (HANDLE)_beginthreadex(NULL, 0, ScannerProc, NULL, 0, NULL);
    if (!g_scannerThread) {
        Wh_Log(L"Failed to create scanner thread");
        SetEvent(g_shutdownEvent);
        if (g_shutdownEvent) CloseHandle(g_shutdownEvent);
        if (g_rescanEvent) CloseHandle(g_rescanEvent);
        g_shutdownEvent = g_rescanEvent = NULL;
        DeleteCriticalSection(&g_devicesLock);
        DestroyIcons();
        return FALSE;
    }

    g_trayThread = (HANDLE)_beginthreadex(NULL, 0, TrayThreadProc, NULL, 0, NULL);
    if (!g_trayThread) {
        Wh_Log(L"Failed to create tray thread");
        SetEvent(g_shutdownEvent);
        WaitForSingleObject(g_scannerThread, 3000);
        CloseHandle(g_scannerThread);
        g_scannerThread = NULL;
        if (g_shutdownEvent) CloseHandle(g_shutdownEvent);
        if (g_rescanEvent) CloseHandle(g_rescanEvent);
        g_shutdownEvent = g_rescanEvent = NULL;
        DeleteCriticalSection(&g_devicesLock);
        DestroyIcons();
        return FALSE;
    }

    Wh_Log(L"Initialized");
    return TRUE;
}

// Reload settings and refresh all icons
void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    HWND hwnd = g_hwnd.load();
    if (IsWindow(hwnd))
        PostMessageW(hwnd, WM_RELOAD_ALL, 0, 0);
}

void WhTool_ModUninit() {
    if (!InterlockedCompareExchange(&g_initComplete, FALSE, TRUE))
        return;

    Wh_Log(L"Uninitializing");

    // Step 1: Signal scanner thread to stop
    if (g_shutdownEvent)
        SetEvent(g_shutdownEvent);

    // Step 2: Join scanner thread (no longer uses events)
    if (g_scannerThread) {
        WaitForSingleObject(g_scannerThread, 3000);
        CloseHandle(g_scannerThread);
        g_scannerThread = NULL;
    }

    // Step 3: Post WM_CLOSE to tray window FIRST (before closing events or CS)
    HWND hwnd = g_hwnd.exchange(nullptr);
    if (hwnd && IsWindow(hwnd))
        PostMessageW(hwnd, WM_CLOSE, 0, 0);

    // Step 4: Wait for tray thread to process WM_CLOSE → WM_DESTROY → PostQuitMessage
    if (g_trayThread) {
        DWORD wr = WaitForSingleObject(g_trayThread, 3000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"tray thread did not exit within 3 s — leaking handle to avoid race");
        } else {
            CloseHandle(g_trayThread);
        }
        g_trayThread = NULL;
    }

    // Step 5: Now safe to close events (no threads reference them)
    if (g_shutdownEvent) { CloseHandle(g_shutdownEvent); g_shutdownEvent = NULL; }
    if (g_rescanEvent) { CloseHandle(g_rescanEvent); g_rescanEvent = NULL; }

    // Step 6: Unregister device notifications
    if (g_hNotifyHid) { UnregisterDeviceNotification(g_hNotifyHid); g_hNotifyHid = nullptr; }
    if (g_hNotifyGatt) { UnregisterDeviceNotification(g_hNotifyGatt); g_hNotifyGatt = nullptr; }
    if (g_hNotifyBthPort) { UnregisterDeviceNotification(g_hNotifyBthPort); g_hNotifyBthPort = nullptr; }

    // Step 7: Join GUI thread (atomically claim the handle to avoid double-close race)
    HANDLE hGuiThread = (HANDLE)InterlockedCompareExchangePointer(
        (PVOID*)&g_guiThread, NULL, g_guiThread);
    if (hGuiThread) {
        HWND dh = (HWND)InterlockedExchangePointer((PVOID*)&g_dashboardHwnd, nullptr);
        if (dh && IsWindow(dh)) {
            PostMessageW(dh, WM_CLOSE, 0, 0);
        }
        DWORD wr = WaitForSingleObject(hGuiThread, 3000);
        if (wr == WAIT_TIMEOUT) {
            Wh_Log(L"GUI thread did not exit within 3 s — leaking handle to avoid race");
        } else {
            CloseHandle(hGuiThread);
        }
    }

    // Step 8: Unregister window class
    UnregisterClassW(L"BTBatTrayWindow", GetModuleHandleW(NULL));

    // Step 9: Clear device vector inside critical section, then delete CS
    {
        std::vector<DeviceInfo> empty;
        {   CsLock lock(g_devicesLock);
            g_devices.swap(empty);
        }
    }
    DeleteCriticalSection(&g_devicesLock);

    // Step 10: Disable icons (tray thread may still be running on timeout), then destroy
    DisableIcons();
    DestroyIcons();

    Wh_Log(L"Uninitialized");
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
