// ==WindhawkMod==
// @id              touchpad-corner-tap-and-two-finger-drag
// @name            Touchpad Corner Tap Right-Click & Two-Finger Drag
// @description     Tap a touchpad corner to right-click, and hold two fingers to drag. Works on any Precision Touchpad, including ones whose click buttons no longer work.
// @version         1.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -lhid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Touchpad Corner Tap Right-Click & Two-Finger Drag

For laptops whose touchpad no longer clicks physically (worn hinge, broken switch,
or a battery pushing the pad up), or for anyone who prefers tapping to pressing.

* **Corner tap right-click** - a quick one-finger tap in the bottom-right (or
  bottom-left) corner of the touchpad becomes a right click. Windows only honors its
  right-click zone for physical presses; this makes it work for taps too.
* **Two-finger hold-and-drag** - put two fingers down, hold still for a moment, then
  move them: the left button is held while you move, so you can select text, move
  windows or pan maps. Moving two fingers straight away still scrolls as usual.

Works with any Windows **Precision Touchpad** (Synaptics, ELAN, Alps, Goodix, Surface,
...). Older non-precision touchpads that use vendor-only drivers don't send the raw
finger positions this mod needs.

Tip: with this mod on you may want to turn off Windows' own "Tap with two fingers to
right-click" and "Tap twice and drag" in Settings > Bluetooth & devices > Touchpad.

## How it works
The mod reads raw finger positions from the touchpad (HID Raw Input) and watches mouse
events with a low-level hook. When Windows turns a corner tap into a left click, the
click is swapped for a right click. Two fingers held still start a drag that the mod
drives itself, and Windows' scroll events are dropped for the duration of the drag.

It runs as a Windhawk tool mod in its own background process (with UIAccess when
available, so it also works over windows of apps running as administrator). It doesn't
inject into or change any other program.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerTap: enabled
  $name: Corner tap right-click
  $description: A quick one-finger tap in the chosen corner right-clicks
  $options:
  - enabled: Enabled
  - disabled: Disabled
- corner: bottomRight
  $name: Corner
  $options:
  - bottomRight: Bottom right
  - bottomLeft: Bottom left
- zoneWidth: 0
  $name: Corner width (% of touchpad)
  $description: 0 uses the size of Windows' own right-click zone
- zoneHeight: 0
  $name: Corner height (% of touchpad)
  $description: 0 uses the size of Windows' own right-click zone
- maxTapMs: 400
  $name: Maximum tap length (ms)
  $description: A touch longer than this is not treated as a tap
- twoFingerDrag: enabled
  $name: Two-finger hold-and-drag
  $description: Hold two fingers still, then move them, to drag
  $options:
  - enabled: Enabled
  - disabled: Disabled
- holdMs: 350
  $name: Hold time before drag (ms)
  $description: Longer avoids starting a drag when you rest two fingers before scrolling
- dragSpeed: 130
  $name: Drag pointer speed (%)
  $description: 100 means moving across the whole touchpad moves the pointer across the whole screen
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// HID parser declarations (hid.dll). Declared here to avoid depending on DDK headers.
// ---------------------------------------------------------------------------
extern "C" {
struct TP_HIDP_CAPS {
    USHORT Usage, UsagePage, InputReportByteLength, OutputReportByteLength,
        FeatureReportByteLength;
    USHORT Reserved[17];
    USHORT NumberLinkCollectionNodes, NumberInputButtonCaps, NumberInputValueCaps,
        NumberInputDataIndices, NumberOutputButtonCaps, NumberOutputValueCaps,
        NumberOutputDataIndices, NumberFeatureButtonCaps, NumberFeatureValueCaps,
        NumberFeatureDataIndices;
};
LONG __stdcall HidP_GetCaps(PVOID preparsed, TP_HIDP_CAPS* caps);
LONG __stdcall HidP_GetValueCaps(int reportType, PVOID valueCaps, PUSHORT length,
                                 PVOID preparsed);
LONG __stdcall HidP_GetUsageValue(int reportType, USHORT usagePage, USHORT linkCollection,
                                  USHORT usage, PULONG value, PVOID preparsed,
                                  PCHAR report, ULONG reportLength);
LONG __stdcall HidP_GetUsages(int reportType, USHORT usagePage, USHORT linkCollection,
                              PUSHORT usageList, PULONG usageLength, PVOID preparsed,
                              PCHAR report, ULONG reportLength);
}

constexpr LONG kHidpOk = 0x00110000;
constexpr int kHidpInput = 0;
constexpr int kValueCapsSize = 72;  // sizeof(HIDP_VALUE_CAPS)
constexpr ULONG_PTR kMarker = 0xC0FFEE42;  // tags input we inject ourselves
constexpr UINT WM_TP_RIGHTCLICK = WM_APP + 1;
constexpr DWORD kClickAfterLiftMs = 800;
constexpr double kMaxTapMove = 0.04;
constexpr double kHoldMove = 0.015;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct Settings {
    bool cornerTap = true;
    bool bottomLeft = false;
    double zoneW = 0.5, zoneH = 0.33;
    DWORD maxTapMs = 400;
    bool twoFingerDrag = true;
    DWORD holdMs = 350;
    double dragSpeed = 1.3;
};
Settings g_s;

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// A setting the user never changed reads back empty (0 for an int), not the
// default declared above, so 0 means "use the default" for every number here.
static int IntOr(PCWSTR name, int def, int lo, int hi) {
    int v = Wh_GetIntSetting(name);
    return v == 0 ? def : Clamp(v, lo, hi);
}

// Empty also means untouched, so only an explicit "disabled" turns a feature off.
static bool Enabled(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    bool off = v && wcscmp(v, L"disabled") == 0;
    Wh_FreeStringSetting(v);
    return !off;
}

static void ReadWindowsZone(double* w, double* h) {
    const wchar_t* key = L"Software\\Microsoft\\Windows\\CurrentVersion\\PrecisionTouchPad";
    DWORD v, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"RightClickZoneWidth", RRF_RT_REG_DWORD,
                     nullptr, &v, &cb) == ERROR_SUCCESS && v > 0 && v <= 100)
        *w = v / 100.0;
    cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"RightClickZoneHeight", RRF_RT_REG_DWORD,
                     nullptr, &v, &cb) == ERROR_SUCCESS && v > 0 && v <= 100)
        *h = v / 100.0;
}

static void LoadSettings() {
    Settings s;
    s.cornerTap = Enabled(L"cornerTap");
    PCWSTR corner = Wh_GetStringSetting(L"corner");
    s.bottomLeft = corner && wcscmp(corner, L"bottomLeft") == 0;
    Wh_FreeStringSetting(corner);

    double ww = 0.5, wh = 0.33;
    ReadWindowsZone(&ww, &wh);
    int zw = Wh_GetIntSetting(L"zoneWidth"), zh = Wh_GetIntSetting(L"zoneHeight");
    s.zoneW = (zw > 0 && zw <= 100) ? zw / 100.0 : ww;
    s.zoneH = (zh > 0 && zh <= 100) ? zh / 100.0 : wh;

    s.maxTapMs = IntOr(L"maxTapMs", 400, 100, 1500);
    s.twoFingerDrag = Enabled(L"twoFingerDrag");
    s.holdMs = IntOr(L"holdMs", 350, 100, 2000);
    s.dragSpeed = IntOr(L"dragSpeed", 130, 20, 500) / 100.0;
    g_s = s;
    Wh_Log(L"Settings: corner %s %.2fx%.2f, cornerTap %d, twoFingerDrag %d, hold %u ms",
           s.bottomLeft ? L"bottomLeft" : L"bottomRight", s.zoneW, s.zoneH, s.cornerTap,
           s.twoFingerDrag, s.holdMs);
}

// ---------------------------------------------------------------------------
// Touch tracking (worker thread only)
// ---------------------------------------------------------------------------
struct Dev {
    bool valid = false;
    std::vector<BYTE> preparsed;
    std::vector<USHORT> slots;  // link collections that each hold one finger
    LONG xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    int frameRemaining = 0;
};

std::unordered_map<HANDLE, Dev> g_devs;
std::unordered_set<ULONG> g_down;
std::unordered_map<ULONG, std::pair<double, double>> g_pos;  // raw pad units

bool g_haveSession, g_sessionEnded, g_consumed;
DWORD g_startTick, g_endTick;
ULONG g_primaryId;
double g_sx, g_sy, g_lx, g_ly;  // normalized 0..1
int g_maxContacts;

bool g_swallowUp;
DWORD g_swallowTick;

int g_twoMode;  // 0 idle, 1 holding, 2 dragging, 3 done until all fingers lift
DWORD g_twoStart, g_dragEndTick;
double g_startCx, g_startCy, g_refX, g_refY, g_accX, g_accY;
int g_lastCount;
bool g_buttonHeld;
Dev* g_lastDev;

HWND g_hwnd;
HHOOK g_hook;
HANDLE g_thread, g_stopEvent;

static void SendMouse(DWORD flags, int dx = 0, int dy = 0) {
    INPUT in = {};
    in.type = INPUT_MOUSE;
    in.mi.dx = dx;
    in.mi.dy = dy;
    in.mi.dwFlags = flags;
    in.mi.dwExtraInfo = kMarker;
    SendInput(1, &in, sizeof(in));
}

static Dev* GetDev(HANDLE hDevice) {
    auto it = g_devs.find(hDevice);
    if (it != g_devs.end())
        return it->second.valid ? &it->second : nullptr;

    Dev& d = g_devs[hDevice];
    UINT size = 0;
    GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, nullptr, &size);
    if (!size)
        return nullptr;
    d.preparsed.resize(size);
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, d.preparsed.data(), &size) ==
        (UINT)-1)
        return nullptr;
    PVOID pp = d.preparsed.data();

    TP_HIDP_CAPS caps;
    if (HidP_GetCaps(pp, &caps) != kHidpOk)
        return nullptr;
    USHORT n = caps.NumberInputValueCaps;
    std::vector<BYTE> vc(kValueCapsSize * (n ? n : 1));
    if (HidP_GetValueCaps(kHidpInput, vc.data(), &n, pp) != kHidpOk)
        return nullptr;

    bool gotY = false;
    for (int i = 0; i < n; i++) {
        const BYTE* p = vc.data() + i * kValueCapsSize;
        USHORT page = *(const USHORT*)(p + 0);
        USHORT link = *(const USHORT*)(p + 6);
        bool isRange = p[12] != 0;
        LONG lmin = *(const LONG*)(p + 40), lmax = *(const LONG*)(p + 44);
        USHORT usage = *(const USHORT*)(p + 56);
        if (page != 0x01 || isRange)
            continue;
        if (usage == 0x30) {
            d.slots.push_back(link);
            d.xMin = lmin;
            d.xMax = lmax;
        } else if (usage == 0x31 && !gotY) {
            d.yMin = lmin;
            d.yMax = lmax;
            gotY = true;
        }
    }

    d.valid = !d.slots.empty() && d.xMax > d.xMin && d.yMax > d.yMin;
    if (d.valid)
        Wh_Log(L"Touchpad found: %zu contact slots, X %ld..%ld, Y %ld..%ld", d.slots.size(),
               d.xMin, d.xMax, d.yMin, d.yMax);
    else
        Wh_Log(L"Device %p ignored (no usable X/Y contacts)", hDevice);
    return d.valid ? &d : nullptr;
}

static void Centroid(double* cx, double* cy) {
    *cx = *cy = 0;
    int n = 0;
    for (ULONG id : g_down) {
        auto it = g_pos.find(id);
        if (it == g_pos.end())
            continue;
        *cx += it->second.first;
        *cy += it->second.second;
        n++;
    }
    if (n) {
        *cx /= n;
        *cy /= n;
    }
}

static void EndDrag() {
    if (!g_buttonHeld)
        return;
    g_buttonHeld = false;
    g_dragEndTick = GetTickCount();
    SendMouse(MOUSEEVENTF_LEFTUP);
    Wh_Log(L"Two-finger drag end");
}

static void TryStartDrag() {
    if (g_twoMode != 1 || !g_s.twoFingerDrag || g_down.size() != 2 || !g_lastDev)
        return;
    if (GetTickCount() - g_twoStart < g_s.holdMs)
        return;
    g_twoMode = 2;
    Centroid(&g_refX, &g_refY);
    g_lastCount = 2;
    g_accX = g_accY = 0;
    SendMouse(MOUSEEVENTF_LEFTDOWN);
    g_buttonHeld = true;
    Wh_Log(L"Two-finger drag start");
}

static void UpdateTwoFinger(Dev& d) {
    int count = (int)g_down.size();
    if (count == 0) {
        EndDrag();
        g_twoMode = 0;
        return;
    }
    double w = (double)(d.xMax - d.xMin);
    double cx, cy;

    if (g_twoMode == 2) {
        if (count < 2) {
            EndDrag();
            g_twoMode = 3;
            return;
        }
        Centroid(&cx, &cy);
        if (count != g_lastCount) {
            g_refX = cx;
            g_refY = cy;
            g_lastCount = count;
            return;
        }
        double scale = GetSystemMetrics(SM_CXSCREEN) * g_s.dragSpeed / w;
        g_accX += (cx - g_refX) * scale;
        g_accY += (cy - g_refY) * scale;
        g_refX = cx;
        g_refY = cy;
        int mx = (int)g_accX, my = (int)g_accY;
        g_accX -= mx;
        g_accY -= my;
        if (mx || my)
            SendMouse(MOUSEEVENTF_MOVE, mx, my);
        return;
    }
    if (g_twoMode == 0) {
        if (count == 2 && g_s.twoFingerDrag) {
            g_twoMode = 1;
            g_twoStart = GetTickCount();
            Centroid(&g_startCx, &g_startCy);
        } else if (count > 2) {
            g_twoMode = 3;
        }
        return;
    }
    if (g_twoMode == 1) {
        if (count != 2) {
            g_twoMode = 3;
            return;
        }
        Centroid(&cx, &cy);
        double dx = (cx - g_startCx) / w, dy = (cy - g_startCy) / w;
        if (std::sqrt(dx * dx + dy * dy) > kHoldMove) {
            g_twoMode = 3;  // moved before the hold finished: it's a scroll
            return;
        }
        TryStartDrag();
    }
}

static void ProcessReport(Dev& d, BYTE* report, ULONG len) {
    PVOID pp = d.preparsed.data();
    PCHAR r = (PCHAR)report;

    ULONG cc = 0;
    bool hasCount =
        HidP_GetUsageValue(kHidpInput, 0x0D, 0, 0x54, &cc, pp, r, len) == kHidpOk;
    if (!hasCount)
        d.frameRemaining = (int)d.slots.size();  // each report is a whole frame
    else if (cc > 0)
        d.frameRemaining = (int)cc;  // first report of a (hybrid mode) frame

    int todo = d.frameRemaining < (int)d.slots.size() ? d.frameRemaining : (int)d.slots.size();
    d.frameRemaining -= todo;

    for (int i = 0; i < todo; i++) {
        USHORT lc = d.slots[i];
        USHORT usages[16];
        ULONG ulen = ARRAYSIZE(usages);
        bool tip = false;
        if (HidP_GetUsages(kHidpInput, 0x0D, lc, usages, &ulen, pp, r, len) == kHidpOk)
            for (ULONG u = 0; u < ulen; u++)
                if (usages[u] == 0x42)
                    tip = true;
        ULONG id, x, y;
        if (HidP_GetUsageValue(kHidpInput, 0x0D, lc, 0x51, &id, pp, r, len) != kHidpOk)
            id = lc;
        if (HidP_GetUsageValue(kHidpInput, 0x01, lc, 0x30, &x, pp, r, len) != kHidpOk)
            continue;
        if (HidP_GetUsageValue(kHidpInput, 0x01, lc, 0x31, &y, pp, r, len) != kHidpOk)
            continue;
        double nx = ((LONG)x - d.xMin) / (double)(d.xMax - d.xMin);
        double ny = ((LONG)y - d.yMin) / (double)(d.yMax - d.yMin);

        if (tip) {
            if (g_down.empty() && (!g_haveSession || g_sessionEnded)) {
                g_haveSession = true;
                g_sessionEnded = false;
                g_consumed = false;
                g_startTick = GetTickCount();
                g_primaryId = id;
                g_maxContacts = 0;
                g_sx = g_lx = nx;
                g_sy = g_ly = ny;
            }
            g_down.insert(id);
            g_pos[id] = {(double)(LONG)x, (double)(LONG)y};
            if (id == g_primaryId) {
                g_lx = nx;
                g_ly = ny;
            }
        } else {
            g_down.erase(id);
            g_pos.erase(id);
        }
    }

    if ((int)g_down.size() > g_maxContacts)
        g_maxContacts = (int)g_down.size();
    if (d.frameRemaining == 0 && g_down.empty() && g_haveSession && !g_sessionEnded) {
        g_sessionEnded = true;
        g_endTick = GetTickCount();
    }
    if (d.frameRemaining == 0) {
        g_lastDev = &d;
        UpdateTwoFinger(d);
    }
}

static void OnRawInput(HRAWINPUT hRaw) {
    UINT size = 0;
    GetRawInputData(hRaw, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
    if (!size)
        return;
    std::vector<BYTE> buf(size);
    if (GetRawInputData(hRaw, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER)) != size)
        return;
    RAWINPUT* ri = (RAWINPUT*)buf.data();
    if (ri->header.dwType != RIM_TYPEHID)
        return;
    Dev* d = GetDev(ri->header.hDevice);
    if (!d)
        return;
    DWORD sz = ri->data.hid.dwSizeHid, cnt = ri->data.hid.dwCount;
    for (DWORD i = 0; i < cnt; i++)
        ProcessReport(*d, ri->data.hid.bRawData + i * sz, sz);
}

// ---------------------------------------------------------------------------
// Corner tap + mouse hook
// ---------------------------------------------------------------------------
static bool InZone(double x, double y) {
    bool inX = g_s.bottomLeft ? x <= g_s.zoneW : x >= 1.0 - g_s.zoneW;
    return inX && y >= 1.0 - g_s.zoneH;
}

static bool IsCornerTap() {
    // A tap's click arrives only after the finger lifts. A button-down while a finger is
    // still on the pad is a drag or a held press, so leave it alone.
    if (!g_s.cornerTap || !g_haveSession || g_consumed || !g_sessionEnded || !g_down.empty())
        return false;
    DWORD now = GetTickCount();
    if (now - g_endTick > kClickAfterLiftMs)
        return false;
    if (g_endTick - g_startTick > g_s.maxTapMs)
        return false;
    if (g_maxContacts != 1 || !InZone(g_sx, g_sy))
        return false;
    double dx = g_lx - g_sx, dy = g_ly - g_sy;
    if (std::sqrt(dx * dx + dy * dy) > kMaxTapMove)
        return false;
    g_consumed = true;
    return true;
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        auto* m = (MSLLHOOKSTRUCT*)lParam;
        if (m->dwExtraInfo != kMarker) {
            DWORD now = GetTickCount();
            if (g_swallowUp && now - g_swallowTick > 1500)
                g_swallowUp = false;  // safety reset

            // During (and just after) a two-finger drag Windows still thinks the fingers
            // are scrolling or two-finger-tapping: drop those events.
            bool dragging = g_twoMode == 2 || (g_twoMode == 3 && now - g_dragEndTick < 600);
            if (dragging && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL ||
                             wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP))
                return 1;

            if (wParam == WM_LBUTTONDOWN && IsCornerTap()) {
                g_swallowUp = true;
                g_swallowTick = now;
                Wh_Log(L"Corner tap -> right click");
                return 1;
            }
            if (wParam == WM_LBUTTONUP && g_swallowUp) {
                g_swallowUp = false;
                PostMessageW(g_hwnd, WM_TP_RIGHTCLICK, 0, 0);
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawInput((HRAWINPUT)lParam);
            break;
        case WM_TIMER:
            TryStartDrag();
            return 0;
        case WM_TP_RIGHTCLICK:
            SendMouse(MOUSEEVENTF_RIGHTDOWN);
            SendMouse(MOUSEEVENTF_RIGHTUP);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
static HMODULE ThisModule() {
    HMODULE h = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WndProc, &h);
    return h;
}

static void RunEngine() {
    HMODULE mod = ThisModule();
    const wchar_t* cls = L"WH_TouchpadCornerTap";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = mod;
    wc.lpszClassName = cls;
    RegisterClassW(&wc);
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, cls, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                             nullptr, mod, nullptr);
    if (!g_hwnd) {
        Wh_Log(L"CreateWindow failed: %u", GetLastError());
        UnregisterClassW(cls, mod);
        return;
    }

    RAWINPUTDEVICE rid = {0x0D, 0x05, RIDEV_INPUTSINK, g_hwnd};
    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());

    g_hook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, GetModuleHandleW(nullptr), 0);
    if (!g_hook)
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());

    SetTimer(g_hwnd, 1, 20, nullptr);
    Wh_Log(L"Running");

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0)
            break;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    EndDrag();  // never leave the left button stuck down
    KillTimer(g_hwnd, 1);
    if (g_hook)
        UnhookWindowsHookEx(g_hook);
    g_hook = nullptr;

    RAWINPUTDEVICE rm = {0x0D, 0x05, RIDEV_REMOVE, nullptr};
    RegisterRawInputDevices(&rm, 1, sizeof(rm));

    DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(cls, mod);
    g_devs.clear();
}

static DWORD WINAPI WorkerThread(LPVOID) {
    // Shared with the standalone CornerTapRightClick tray app, so the two never handle
    // the touchpad at the same time: while the app runs, the mod waits for it to exit.
    HANDLE mutex = CreateMutexW(nullptr, FALSE, L"Local\\CornerTapRightClick_Singleton");
    if (!mutex) {
        Wh_Log(L"CreateMutex failed: %u", GetLastError());
        return 0;
    }
    HANDLE waits[2] = {g_stopEvent, mutex};
    DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    if (r == WAIT_OBJECT_0 + 1 || r == WAIT_ABANDONED_0 + 1) {
        RunEngine();
        ReleaseMutex(mutex);
    }
    CloseHandle(mutex);
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod entry points
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent)
        return FALSE;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
