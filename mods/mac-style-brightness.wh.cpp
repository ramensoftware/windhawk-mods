// ==WindhawkMod==
// @id            mac-style-brightness
// @name          Mac-Style Smooth Brightness
// @description   Smooth brightness via taskbar scroll, F10/F11, or keyboard. Software overlay below Windows minimum.
// @version       12.1
// @author        kivsak
// @github        https://github.com/kivsak
// @include       explorer.exe
// @compilerOptions -lgdi32 -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
👉 PLS GIB MONI FOR 2X SPEED BOOST: https://www.patreon.com/c/kivsak

Sick of fumbling with the tiny Windows brightness slider buried in the Action Center? This mod turns your **taskbar into a giant brightness dial** — just hover and scroll. Zero clicks, zero menus, instant response.

Built with a **triple-fallback brightness engine** (direct IOCTL → WMI → PowerScheme) that automatically picks the smoothest, least-jerky method your hardware supports — the same WMI backend used by pro tools like Monitorian and Twinkle Tray, but without installing a separate app.

🌟 **Features**
* **Taskbar Scroll Engine**: Hover the taskbar, scroll up/down. That's it. No modifier keys, no popups, no delay. Brightness changes instantly.
* **Keyboard Shortcuts**: Ctrl+Alt + Arrow Up/Down for precise step-by-step control. Ctrl+Alt+Shift+B for instant panic reset to 100%.
* **Software Dimming Overlay**: A black overlay that goes **below** your hardware minimum brightness. On **OLED laptops** this means deeper blacks, reduced burn-in risk, and significantly better battery life (OLED pixels emit less light = less power draw). On **IPS/VA panels** it kills that eye-searing glow when even 0% is still too bright at 3 AM.
* **Smart Backend Selection**: Tries direct hardware IOCTL first (bypasses Windows brightness pipeline entirely), falls back to WMI with Timeout=0 (no driver animation fighting), and only uses PowerScheme as a last resort with built-in rate limiting to prevent the infamous brightness "jerking" bug.
* **Anti-Jerk Architecture**: Rate-limited writes, poll cooldowns, and input protection ensure the mod never fights with Windows or your BIOS over who controls the backlight.

⚠️ **Note about Fn Brightness Keys**: On most laptops, hardware brightness keys (Fn+F10/F11) are handled by the BIOS firmware via ACPI and completely bypass Windows. No app can intercept them — not even PowerToys or AutoHotkey. Use **taskbar scroll** or **Ctrl+Alt+Arrows** for full control including the software overlay.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbar_scroll: true
  $name: Scroll on Taskbar
  $description: Just hover your mouse over the taskbar and scroll up/down. No extra keys needed! This is the most reliable method.
- f10f11_keys: false
  $name: F10 / F11 Keys (Fn Lock only!)
  $description: ONLY works if Fn Lock is ON (so F10/F11 send normal key codes). Does NOT work with hardware brightness keys — those bypass Windows entirely and no app can intercept them.
- ctrlalt_scroll: false
  $name: Ctrl+Alt + Scroll Wheel
  $description: Hold Ctrl+Alt and scroll anywhere on screen. Useful if you don't want bare scroll on the taskbar.
- keyboard_arrows: true
  $name: Ctrl+Alt + Arrow Keys
  $description: Ctrl+Alt + Up = brighter, Ctrl+Alt + Down = darker. Works everywhere, always reliable.
- step_size: 5
  $name: Brightness Step (%)
  $description: How much brightness changes per scroll tick or key press. Smaller = more precise, bigger = faster. Range 1-25.
- software_dimming: true
  $name: Software Dimming Overlay
  $description: Adds a black overlay that goes BELOW hardware minimum brightness. For OLED laptops this means deeper blacks, less burn-in, and much longer battery life (darker pixels = less power). For IPS/VA panels this reduces eye strain at night when even 0% is still too bright.
- hw_minimum: 0
  $name: Hardware Minimum (0-100)
  $description: At what hardware brightness level the software overlay starts. 0 = overlay kicks in only after hardware is at absolute minimum. Set higher if your screen's lowest setting is still too bright.
- overlay_max: 230
  $name: Max Overlay Darkness (0-255)
  $description: How dark the overlay can go. 200 = very dark but still visible. 230 = almost black (default). 255 = completely black screen. For OLED use 240-255, for IPS 200-230 is usually enough.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <objbase.h>
#include <cmath>

// ===================================================================
// WMI COM interfaces — manual declarations for MinGW compatibility
// ===================================================================

// GUIDs
static const GUID CLSID_WbemLocator_  = {0x4590f811,0x1d3a,0x11d0,{0x89,0x1f,0x00,0xaa,0x00,0x4b,0x2e,0x24}};
static const GUID IID_IWbemLocator_   = {0xdc12a687,0x737f,0x11cf,{0x88,0x4d,0x00,0xaa,0x00,0x4b,0x2e,0x24}};

// Minimal IWbemLocator / IWbemServices / IWbemClassObject / IEnumWbemClassObject
// We use these via their vtable to avoid wbemidl.h / wbemuuid.lib dependency

#ifndef __IWbemClassObject_FWD_DEFINED__
#define __IWbemClassObject_FWD_DEFINED__
typedef interface IWbemClassObject IWbemClassObject;
#endif
#ifndef __IEnumWbemClassObject_FWD_DEFINED__
#define __IEnumWbemClassObject_FWD_DEFINED__
typedef interface IEnumWbemClassObject IEnumWbemClassObject;
#endif
#ifndef __IWbemServices_FWD_DEFINED__
#define __IWbemServices_FWD_DEFINED__
typedef interface IWbemServices IWbemServices;
#endif
#ifndef __IWbemLocator_FWD_DEFINED__
#define __IWbemLocator_FWD_DEFINED__
typedef interface IWbemLocator IWbemLocator;
#endif

// We include the real header if available, otherwise define minimally
#include <wbemidl.h>

#define WBEM_FLAG_FORWARD_ONLY          0x20
#define WBEM_FLAG_RETURN_IMMEDIATELY    0x10
#ifndef WBEM_INFINITE
#define WBEM_INFINITE 0xFFFFFFFF
#endif

// ===================================================================
// IOCTL \\.\LCD — direct backlight, bypasses WDDM smoothing
// ===================================================================

#pragma pack(push, 1)
struct DISPLAY_BRIGHTNESS_S {
    BYTE ucDisplayPolicy;
    BYTE ucACBrightness;
    BYTE ucDCBrightness;
};
#pragma pack(pop)

// IOCTL codes (FILE_DEVICE_VIDEO=0x23, METHOD_BUFFERED=0, FILE_ANY_ACCESS=0)
#define IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS 0x00230498UL
#define IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS   0x0023049CUL

// ===================================================================
// Dynamic PowrProf (last-resort fallback)
// ===================================================================

typedef DWORD(WINAPI *FN_GetScheme)(HKEY, GUID **);
typedef DWORD(WINAPI *FN_ReadAC)(HKEY, const GUID *, const GUID *, const GUID *, LPDWORD);
typedef DWORD(WINAPI *FN_WriteAC)(HKEY, const GUID *, const GUID *, const GUID *, DWORD);
typedef DWORD(WINAPI *FN_WriteDC)(HKEY, const GUID *, const GUID *, const GUID *, DWORD);
typedef DWORD(WINAPI *FN_SetScheme)(HKEY, const GUID *);

static FN_GetScheme   pGetScheme;
static FN_ReadAC      pReadAC;
static FN_WriteAC     pWriteAC;
static FN_WriteDC     pWriteDC;
static FN_SetScheme   pSetScheme;
static HMODULE g_hPowrProf;

static const GUID VID_SUB = {0x7516b95f,0xf776,0x4464,{0x8c,0x53,0x06,0x16,0x7f,0x40,0xcc,0x99}};
static const GUID VID_BRI = {0xaded5e82,0xb909,0x4619,{0x99,0x49,0xf5,0xd7,0x1d,0xac,0x0b,0xcb}};

// ===================================================================
// Backend selection
// ===================================================================

enum BkType { BK_IOCTL, BK_WMI, BK_POWER, BK_NONE };
static BkType g_backend = BK_NONE;

// IOCTL state
static HANDLE g_hLCD = INVALID_HANDLE_VALUE;

// WMI state
static IWbemServices     *g_pWmiSvc       = NULL;
static BSTR               g_wmiObjPath    = NULL;
static IWbemClassObject  *g_pWmiInTpl     = NULL;  // input params template

// ===================================================================
// Self module handle for hooks
// ===================================================================

static HMODULE GetSelfModule() {
    HMODULE h = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetSelfModule, &h);
    return h;
}

// ===================================================================
// Backend: IOCTL
// ===================================================================

static bool InitIoctl() {
    g_hLCD = CreateFileW(L"\\\\.\\LCD", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                         OPEN_EXISTING, 0, NULL);
    if (g_hLCD == INVALID_HANDLE_VALUE) {
        Wh_Log(L"[IOCTL] CreateFile \\\\.\\LCD failed: %u", GetLastError());
        return false;
    }
    Wh_Log(L"[IOCTL] \\\\.\\LCD opened OK");
    return true;
}

static int IoctlRead() {
    DISPLAY_BRIGHTNESS_S db = {};
    DWORD ret = 0;
    if (!DeviceIoControl(g_hLCD, IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,
                         NULL, 0, &db, sizeof(db), &ret, NULL))
        return -1;
    return (int)db.ucACBrightness;
}

static bool IoctlWrite(int v) {
    DISPLAY_BRIGHTNESS_S db;
    db.ucDisplayPolicy = 3; // AC + DC
    db.ucACBrightness  = (BYTE)(v < 0 ? 0 : (v > 100 ? 100 : v));
    db.ucDCBrightness  = db.ucACBrightness;
    DWORD ret = 0;
    return DeviceIoControl(g_hLCD, IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS,
                           &db, sizeof(db), NULL, 0, &ret, NULL) != 0;
}

// ===================================================================
// Backend: WMI
// ===================================================================

static bool InitWmi() {
    HRESULT hr;

    // COM init (may already be initialized)
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    bool comOwned = SUCCEEDED(hr);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE && hr != S_FALSE) {
        Wh_Log(L"[WMI] CoInitializeEx failed: 0x%08X", hr);
        return false;
    }

    // Security (may already be set — ignore errors)
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                         RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create locator
    IWbemLocator *pLoc = NULL;
    hr = CoCreateInstance(CLSID_WbemLocator_, NULL, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator_, (void **)&pLoc);
    if (FAILED(hr) || !pLoc) {
        Wh_Log(L"[WMI] CoCreateInstance WbemLocator failed: 0x%08X", hr);
        return false;
    }

    // Connect to ROOT\WMI
    BSTR bsRoot = SysAllocString(L"ROOT\\WMI");
    hr = pLoc->ConnectServer(bsRoot, NULL, NULL, 0, 0, 0, 0, &g_pWmiSvc);
    SysFreeString(bsRoot);
    pLoc->Release();
    if (FAILED(hr) || !g_pWmiSvc) {
        Wh_Log(L"[WMI] ConnectServer failed: 0x%08X", hr);
        return false;
    }

    // Set security on proxy
    CoSetProxyBlanket(g_pWmiSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                      NULL, EOAC_NONE);

    // Find the first WmiMonitorBrightnessMethods instance
    BSTR bsWql   = SysAllocString(L"WQL");
    BSTR bsQuery = SysAllocString(L"SELECT * FROM WmiMonitorBrightnessMethods");
    IEnumWbemClassObject *pEnum = NULL;
    hr = g_pWmiSvc->ExecQuery(bsWql, bsQuery,
                               WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                               NULL, &pEnum);
    SysFreeString(bsWql);
    SysFreeString(bsQuery);
    if (FAILED(hr) || !pEnum) {
        Wh_Log(L"[WMI] ExecQuery BrightnessMethods failed: 0x%08X", hr);
        return false;
    }

    IWbemClassObject *pObj = NULL;
    ULONG count = 0;
    hr = pEnum->Next(10000, 1, &pObj, &count);
    if (FAILED(hr) || count == 0 || !pObj) {
        Wh_Log(L"[WMI] No WmiMonitorBrightnessMethods instance found");
        pEnum->Release();
        return false;
    }

    // Get __PATH
    VARIANT vPath;
    VariantInit(&vPath);
    pObj->Get(L"__PATH", 0, &vPath, NULL, NULL);
    if (vPath.vt == VT_BSTR && vPath.bstrVal)
        g_wmiObjPath = SysAllocString(vPath.bstrVal);
    VariantClear(&vPath);
    pObj->Release();
    pEnum->Release();

    if (!g_wmiObjPath) {
        Wh_Log(L"[WMI] Could not get instance path");
        return false;
    }
    Wh_Log(L"[WMI] Instance: %s", g_wmiObjPath);

    // Get method input params template
    BSTR bsClass = SysAllocString(L"WmiMonitorBrightnessMethods");
    IWbemClassObject *pClass = NULL;
    hr = g_pWmiSvc->GetObject(bsClass, 0, NULL, &pClass, NULL);
    SysFreeString(bsClass);
    if (FAILED(hr) || !pClass) {
        Wh_Log(L"[WMI] GetObject class failed: 0x%08X", hr);
        return false;
    }

    BSTR bsMethod = SysAllocString(L"WmiSetBrightness");
    hr = pClass->GetMethod(bsMethod, 0, &g_pWmiInTpl, NULL);
    SysFreeString(bsMethod);
    pClass->Release();
    if (FAILED(hr) || !g_pWmiInTpl) {
        Wh_Log(L"[WMI] GetMethod WmiSetBrightness failed: 0x%08X", hr);
        return false;
    }

    Wh_Log(L"[WMI] Backend ready");
    return true;
}

static bool WmiWrite(int val) {
    if (!g_pWmiSvc || !g_wmiObjPath || !g_pWmiInTpl) return false;
    if (val < 0) val = 0; if (val > 100) val = 100;

    IWbemClassObject *pIn = NULL;
    HRESULT hr = g_pWmiInTpl->SpawnInstance(0, &pIn);
    if (FAILED(hr) || !pIn) return false;

    VARIANT vTimeout;
    VariantInit(&vTimeout);
    vTimeout.vt   = VT_I4;
    vTimeout.lVal = 0;   // Timeout=0 → apply ASAP, no driver animation
    pIn->Put(L"Timeout", 0, &vTimeout, 0);

    VARIANT vBri;
    VariantInit(&vBri);
    vBri.vt   = VT_UI1;
    vBri.bVal = (BYTE)val;
    pIn->Put(L"Brightness", 0, &vBri, 0);

    BSTR bsMethod = SysAllocString(L"WmiSetBrightness");
    hr = g_pWmiSvc->ExecMethod(g_wmiObjPath, bsMethod, 0, NULL, pIn, NULL, NULL);
    SysFreeString(bsMethod);
    pIn->Release();

    return SUCCEEDED(hr);
}

static int WmiRead() {
    if (!g_pWmiSvc) return -1;

    BSTR bsWql   = SysAllocString(L"WQL");
    BSTR bsQuery = SysAllocString(L"SELECT CurrentBrightness FROM WmiMonitorBrightness");
    IEnumWbemClassObject *pEnum = NULL;
    HRESULT hr = g_pWmiSvc->ExecQuery(bsWql, bsQuery,
                                       WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                       NULL, &pEnum);
    SysFreeString(bsWql);
    SysFreeString(bsQuery);
    if (FAILED(hr) || !pEnum) return -1;

    int result = -1;
    IWbemClassObject *pObj = NULL;
    ULONG count = 0;
    if (pEnum->Next(5000, 1, &pObj, &count) == S_OK && count > 0 && pObj) {
        VARIANT v;
        VariantInit(&v);
        pObj->Get(L"CurrentBrightness", 0, &v, NULL, NULL);
        if (v.vt == VT_UI1) result = v.bVal;
        VariantClear(&v);
        pObj->Release();
    }
    pEnum->Release();
    return result;
}

// ===================================================================
// Backend: PowerScheme (last resort, rate-limited)
// ===================================================================

static bool InitPowerApi() {
    g_hPowrProf = LoadLibraryW(L"PowrProf.dll");
    if (!g_hPowrProf) return false;
    pGetScheme = (FN_GetScheme)GetProcAddress(g_hPowrProf, "PowerGetActiveScheme");
    pReadAC    = (FN_ReadAC)GetProcAddress(g_hPowrProf, "PowerReadACValueIndex");
    pWriteAC   = (FN_WriteAC)GetProcAddress(g_hPowrProf, "PowerWriteACValueIndex");
    pWriteDC   = (FN_WriteDC)GetProcAddress(g_hPowrProf, "PowerWriteDCValueIndex");
    pSetScheme = (FN_SetScheme)GetProcAddress(g_hPowrProf, "PowerSetActiveScheme");
    if (!pGetScheme || !pReadAC || !pWriteAC || !pSetScheme) return false;
    Wh_Log(L"[PWR] PowerScheme API: OK");
    return true;
}

static int PowerRead() {
    if (!pGetScheme || !pReadAC) return -1;
    GUID *s = NULL; DWORD v = 0;
    if (pGetScheme(NULL, &s) != 0) return -1;
    DWORD r = pReadAC(NULL, s, &VID_SUB, &VID_BRI, &v);
    LocalFree(s);
    return (r == 0) ? (int)v : -1;
}

static bool PowerWrite(int v) {
    if (!pGetScheme || !pWriteAC || !pSetScheme) return false;
    if (v < 0) v = 0; if (v > 100) v = 100;
    GUID *s = NULL;
    if (pGetScheme(NULL, &s) != 0) return false;
    pWriteAC(NULL, s, &VID_SUB, &VID_BRI, (DWORD)v);
    if (pWriteDC) pWriteDC(NULL, s, &VID_SUB, &VID_BRI, (DWORD)v);
    DWORD r = pSetScheme(NULL, s);
    LocalFree(s);
    return (r == 0);
}

// ===================================================================
// Unified read/write using best available backend
// ===================================================================

static int ReadBrightness() {
    switch (g_backend) {
        case BK_IOCTL: return IoctlRead();
        case BK_WMI:   return WmiRead();
        case BK_POWER: return PowerRead();
        default: return -1;
    }
}

// Rate limiting for PowerScheme backend
static DWORD g_lastWriteMs  = 0;
static int   g_pendingWrite = -1;
static UINT_PTR g_writeTimer = 0;

static void DoWrite(int v);  // forward

static void CALLBACK OnFlushWrite(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(NULL, id);
    g_writeTimer = 0;
    if (g_pendingWrite >= 0) {
        int v = g_pendingWrite;
        g_pendingWrite = -1;
        DoWrite(v);
    }
}

static void DoWrite(int v) {
    bool ok = false;
    switch (g_backend) {
        case BK_IOCTL: ok = IoctlWrite(v); break;
        case BK_WMI:   ok = WmiWrite(v);   break;
        case BK_POWER: ok = PowerWrite(v);  break;
        default: break;
    }
    if (ok) g_lastWriteMs = GetTickCount();
}

static void WriteBrightness(int v) {
    // IOCTL and WMI: write immediately (no driver fighting)
    if (g_backend != BK_POWER) {
        DoWrite(v);
        return;
    }
    // PowerScheme: rate-limit writes to max 1 per 250ms to avoid
    // triggering multiple competing WDDM smooth transitions
    DWORD now = GetTickCount();
    DWORD elapsed = now - g_lastWriteMs;
    if (elapsed >= 250) {
        DoWrite(v);
    } else {
        g_pendingWrite = v;
        if (!g_writeTimer)
            g_writeTimer = SetTimer(NULL, 0, 250 - elapsed, OnFlushWrite);
    }
}

// ===================================================================
// Globals
// ===================================================================

static HWND   g_hOverlay;
static HWND   g_hMsgWnd;      // message-only window for pump robustness
static HANDLE g_hMainThread;
static DWORD  g_dwMainTid;
static HHOOK  g_hMouseHook;
static HHOOK  g_hKbHook;

// Settings
static bool g_taskbarScroll = true;
static bool g_f10f11        = true;
static bool g_ctrlaltScroll = false;
static bool g_kbArrows      = true;
static int  g_step          = 5;
static bool g_softDim       = true;
static int  g_hwMin         = 0;
static int  g_oMax          = 230;

// Virtual brightness model
static float g_brightness = 100.0f;
static int   g_lastHw     = -1;
static DWORD g_lastInputMs = 0;  // last user input (ANY method) — protects overlay from poll

// Overlay animation
static int   g_alphaTarget = 0;
static float g_alphaSmooth = 0.0f;

// Custom messages for async hook → message loop
#define WM_BRIGHT_ADJUST  (WM_USER + 200)
#define WM_BRIGHT_PANIC   (WM_USER + 201)

#define HK_UP    1
#define HK_DOWN  2
#define HK_PANIC 3
#define TM_ANIM  1
#define TM_POLL  2
#define TM_TOP   3

// ===================================================================
// Settings
// ===================================================================

static void LoadSettings() {
    g_taskbarScroll = Wh_GetIntSetting(L"taskbar_scroll") != 0;
    g_f10f11        = Wh_GetIntSetting(L"f10f11_keys") != 0;
    g_ctrlaltScroll = Wh_GetIntSetting(L"ctrlalt_scroll") != 0;
    g_kbArrows      = Wh_GetIntSetting(L"keyboard_arrows") != 0;
    g_step          = Wh_GetIntSetting(L"step_size");
    g_softDim       = Wh_GetIntSetting(L"software_dimming") != 0;
    g_hwMin         = Wh_GetIntSetting(L"hw_minimum");
    g_oMax          = Wh_GetIntSetting(L"overlay_max");
    if (g_step < 1) g_step = 1; if (g_step > 25) g_step = 25;
    if (g_hwMin < 0) g_hwMin = 0; if (g_hwMin > 100) g_hwMin = 100;
    if (g_oMax < 0) g_oMax = 0; if (g_oMax > 255) g_oMax = 255;
}

// ===================================================================
// Core brightness logic
// ===================================================================

static float GetFloor() {
    return g_softDim ? (float)g_hwMin - (g_oMax * 100.0f / 255.0f) : (float)g_hwMin;
}

static void ApplyBrightness() {
    int hw, alpha;
    if (g_brightness >= (float)g_hwMin) {
        hw = (int)(g_brightness + 0.5f);
        if (hw > 100) hw = 100;
        alpha = 0;
    } else {
        hw = g_hwMin;
        float below = (float)g_hwMin - g_brightness;
        alpha = (int)(below * 255.0f / 100.0f + 0.5f);
        if (alpha > g_oMax) alpha = g_oMax;
        if (alpha < 0) alpha = 0;
    }

    if (hw != g_lastHw) {
        WriteBrightness(hw);
        g_lastHw = hw;
    }

    if (alpha != g_alphaTarget) {
        Wh_Log(L"[Overlay] alphaTarget: %d → %d (bright=%.1f hwMin=%d)",
               g_alphaTarget, alpha, g_brightness, g_hwMin);
    }
    g_alphaTarget = alpha;
}

static void Adjust(int delta) {
    g_brightness += (float)delta;
    float floor = GetFloor();
    if (g_brightness < floor) g_brightness = floor;
    if (g_brightness > 100.0f) g_brightness = 100.0f;

    g_lastInputMs = GetTickCount();  // protect from poll override

    Wh_Log(L"Adjust(%d) → bright=%.1f alphaT=%d [%s]",
           delta, g_brightness, g_alphaTarget,
           g_backend == BK_IOCTL ? L"IOCTL" :
           g_backend == BK_WMI   ? L"WMI" : L"PowerScheme");

    ApplyBrightness();
}

static void Panic() {
    Wh_Log(L"PANIC → 100%%");
    g_brightness = 100.0f;
    g_lastInputMs = GetTickCount();
    ApplyBrightness();
}

// ===================================================================
// Taskbar detection
// ===================================================================

static bool OnTaskbar() {
    POINT pt;
    GetCursorPos(&pt);
    HWND h = WindowFromPoint(pt);
    if (!h) return false;
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root) root = h;
    WCHAR c[64] = {};
    GetClassNameW(root, c, 64);
    return (_wcsicmp(c, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(c, L"Shell_SecondaryTrayWnd") == 0);
}

// ===================================================================
// Mouse hook — posts message, returns FAST
// ===================================================================

static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *)lParam;
        bool c = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool a = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;
        bool s = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;

        bool go = false;
        if (g_taskbarScroll && !c && !a && !s && OnTaskbar()) go = true;
        if (g_ctrlaltScroll && c && a && !s) go = true;

        if (go) {
            short d = GET_WHEEL_DELTA_WPARAM(p->mouseData);
            // Post async — DO NOT call Adjust here (blocks hook → Windows kills it)
            PostMessage(g_hMsgWnd, WM_BRIGHT_ADJUST, (WPARAM)(d > 0 ? 1 : -1), 0);
            return 1;
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

// ===================================================================
// Keyboard hook — posts message, returns FAST
// ===================================================================

static LRESULT CALLBACK KbProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;

        // Debug: log function keys to find what F10/F11 actually send
        if (p->vkCode >= VK_F1 && p->vkCode <= VK_F24) {
            Wh_Log(L"[KB] vk=0x%X(%d) scan=0x%X flags=0x%X",
                   p->vkCode, p->vkCode, p->scanCode, p->flags);
        }

        if (g_f10f11) {
            bool alt  = (p->flags & LLKHF_ALTDOWN) != 0;
            bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shft = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;

            if (!alt && !ctrl && !shft) {
                if (p->vkCode == VK_F10) {
                    PostMessage(g_hMsgWnd, WM_BRIGHT_ADJUST, (WPARAM)(-1), 0);
                    return 1;
                }
                if (p->vkCode == VK_F11) {
                    PostMessage(g_hMsgWnd, WM_BRIGHT_ADJUST, (WPARAM)(1), 0);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
}

// ===================================================================
// Timers
// ===================================================================

// Overlay alpha animation (~60 FPS)
static void CALLBACK OnAnim(HWND, UINT, UINT_PTR, DWORD) {
    float t = (float)g_alphaTarget;
    float d = t - g_alphaSmooth;
    if (d > 0.5f || d < -0.5f)
        g_alphaSmooth += d * 0.25f;
    else if (g_alphaSmooth != t)
        g_alphaSmooth = t;
    else
        return;

    if (g_hOverlay)
        SetLayeredWindowAttributes(g_hOverlay, 0, (BYTE)(g_alphaSmooth + 0.5f), LWA_ALPHA);
}

// Poll: sync with external changes (Windows slider, Action Center)
// Protected by TWO cooldowns:
//  1. g_lastWriteMs  — don't read back right after we wrote (value still settling)
//  2. g_lastInputMs  — don't override user while they're actively adjusting
// Also: NEVER override when user is in the overlay zone (brightness < hwMin)
static void CALLBACK OnPoll(HWND, UINT, UINT_PTR, DWORD) {
    DWORD now = GetTickCount();

    // Cooldown after writes (value still settling in hardware)
    if (now - g_lastWriteMs < 5000) return;

    // Cooldown after ANY user input (don't fight the user)
    if (now - g_lastInputMs < 8000) return;

    // NEVER override when user is in overlay zone — that's intentional
    if (g_brightness < (float)g_hwMin) return;

    int real = ReadBrightness();
    if (real < 0) return;

    int diff = real - g_lastHw;
    if (diff >= 3 || diff <= -3) {
        Wh_Log(L"[Poll] external: %d → %d", g_lastHw, real);
        g_brightness = (float)real;
        g_lastHw = real;
        g_alphaTarget = 0;
        g_lastWriteMs = now;
    }
}

static void CALLBACK OnTop(HWND, UINT, UINT_PTR, DWORD) {
    if (g_hOverlay)
        SetWindowPos(g_hOverlay, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ===================================================================
// Message-only window — keeps message pump alive for hooks
// ===================================================================

static LRESULT CALLBACK MsgWndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_BRIGHT_ADJUST: {
            int dir = (int)(INT_PTR)wp;  // +1 or -1
            Adjust(dir * g_step);
            return 0;
        }
        case WM_BRIGHT_PANIC:
            Panic();
            return 0;
    }
    return DefWindowProcW(hw, msg, wp, lp);
}

// ===================================================================
// Overlay WndProc
// ===================================================================

static LRESULT CALLBACK OverlayProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_DISPLAYCHANGE) {
        SetWindowPos(hw, HWND_TOPMOST,
                     GetSystemMetrics(SM_XVIRTUALSCREEN),
                     GetSystemMetrics(SM_YVIRTUALSCREEN),
                     GetSystemMetrics(SM_CXVIRTUALSCREEN),
                     GetSystemMetrics(SM_CYVIRTUALSCREEN),
                     SWP_NOACTIVATE);
        return 0;
    }
    return DefWindowProcW(hw, msg, wp, lp);
}

// ===================================================================
// Main thread
// ===================================================================

static DWORD WINAPI MainThread(LPVOID) {
    Wh_Log(L"[Main] v12.0 starting");

    // ------- Init backends: try IOCTL → WMI → PowerScheme -------
    if (InitIoctl()) {
        g_backend = BK_IOCTL;
        Wh_Log(L"[Main] Using IOCTL backend (direct backlight)");
    } else if (InitWmi()) {
        g_backend = BK_WMI;
        Wh_Log(L"[Main] Using WMI backend (WmiSetBrightness)");
    } else if (InitPowerApi()) {
        g_backend = BK_POWER;
        Wh_Log(L"[Main] Using PowerScheme backend (rate-limited fallback)");
    } else {
        Wh_Log(L"[Main] ABORT: no brightness backend available!");
        return 1;
    }

    // Read initial brightness
    int hw = ReadBrightness();
    Wh_Log(L"[Main] Initial brightness = %d", hw);
    if (hw < 0) hw = 50;
    g_brightness = (float)hw;
    g_lastHw = hw;
    g_lastWriteMs = GetTickCount();
    g_lastInputMs = g_lastWriteMs;

    HINSTANCE hInst = GetModuleHandleW(NULL);

    // ------- Create message-only window (keeps hooks alive) -------
    WNDCLASSEXW wcMsg = {sizeof(wcMsg)};
    wcMsg.lpfnWndProc  = MsgWndProc;
    wcMsg.hInstance     = hInst;
    wcMsg.lpszClassName = L"WHBright12Msg";
    RegisterClassExW(&wcMsg);

    g_hMsgWnd = CreateWindowExW(0, wcMsg.lpszClassName, NULL, 0,
                                0, 0, 0, 0,
                                HWND_MESSAGE, NULL, hInst, NULL);
    Wh_Log(L"[Main] MsgWnd: %p", g_hMsgWnd);

    // ------- Create overlay window -------
    WNDCLASSEXW wcOvr = {sizeof(wcOvr)};
    wcOvr.lpfnWndProc  = OverlayProc;
    wcOvr.hInstance     = hInst;
    wcOvr.lpszClassName = L"WHBright12Ovr";
    wcOvr.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassExW(&wcOvr);

    g_hOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED |
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wcOvr.lpszClassName, NULL, WS_POPUP,
        GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_CXVIRTUALSCREEN),
        GetSystemMetrics(SM_CYVIRTUALSCREEN),
        NULL, NULL, hInst, NULL);

    if (g_hOverlay) {
        SetLayeredWindowAttributes(g_hOverlay, 0, 0, LWA_ALPHA);
        ShowWindow(g_hOverlay, SW_SHOWNOACTIVATE);
    }
    Wh_Log(L"[Main] Overlay: %p", g_hOverlay);

    // ------- Timers -------
    SetTimer(g_hMsgWnd, TM_ANIM, 16,   OnAnim);
    SetTimer(g_hMsgWnd, TM_POLL, 2000,  OnPoll);
    SetTimer(g_hMsgWnd, TM_TOP,  3000,  OnTop);

    // ------- Hotkeys -------
    if (g_kbArrows) {
        RegisterHotKey(NULL, HK_UP,   MOD_CONTROL | MOD_ALT, VK_UP);
        RegisterHotKey(NULL, HK_DOWN, MOD_CONTROL | MOD_ALT, VK_DOWN);
    }
    RegisterHotKey(NULL, HK_PANIC, MOD_CONTROL | MOD_ALT | MOD_SHIFT, 'B');

    // ------- Low-level hooks -------
    HMODULE self = GetSelfModule();
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, self, 0);
    g_hKbHook    = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, self, 0);
    Wh_Log(L"[Main] Mouse=%p KB=%p self=%p", g_hMouseHook, g_hKbHook, self);
    Wh_Log(L"[Main] Ready — entering message loop");

    // ------- Message loop -------
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == HK_UP)         Adjust(g_step);
            else if (msg.wParam == HK_DOWN)  Adjust(-g_step);
            else if (msg.wParam == HK_PANIC) Panic();
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ------- Cleanup -------
    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hKbHook)    UnhookWindowsHookEx(g_hKbHook);
    KillTimer(g_hMsgWnd, TM_ANIM);
    KillTimer(g_hMsgWnd, TM_POLL);
    KillTimer(g_hMsgWnd, TM_TOP);
    UnregisterHotKey(NULL, HK_UP);
    UnregisterHotKey(NULL, HK_DOWN);
    UnregisterHotKey(NULL, HK_PANIC);

    // Destroy windows
    if (g_hOverlay) DestroyWindow(g_hOverlay);
    if (g_hMsgWnd)  DestroyWindow(g_hMsgWnd);
    UnregisterClassW(L"WHBright12Ovr", hInst);
    UnregisterClassW(L"WHBright12Msg", hInst);

    // Release WMI
    if (g_pWmiInTpl)  g_pWmiInTpl->Release();
    if (g_pWmiSvc)    g_pWmiSvc->Release();
    if (g_wmiObjPath) SysFreeString(g_wmiObjPath);

    // Close IOCTL
    if (g_hLCD != INVALID_HANDLE_VALUE) CloseHandle(g_hLCD);

    // Release PowerScheme
    if (g_hPowrProf) FreeLibrary(g_hPowrProf);

    return 0;
}

// ===================================================================
// Lifecycle
// ===================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"=== Brightness v12.0 INIT ===");
    LoadSettings();
    g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, &g_dwMainTid);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"=== Brightness v12.0 UNINIT ===");
    if (g_dwMainTid) {
        PostThreadMessage(g_dwMainTid, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hMainThread, 5000);
        CloseHandle(g_hMainThread);
    }
}

void Wh_ModSettingsChanged() { LoadSettings(); }
