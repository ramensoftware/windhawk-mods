// ==WindhawkMod==
// @id           explorer-soft-dedup-per-folder  
// @name         Explorer: Soft Single-Instance per Folder (Windows 11/10)
// @description  Keep only one Explorer window per folder path, with gentle closing and safety guards.
// @version      1.7.0
// @author       XandriW
// @github       xandri19wang
// @include      C:\Windows\explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Gentler dedup, fewer "restart-like" hiccups:
- Double stability checks (new + existing window).
- Gentle close: minimize first, then async WM_CLOSE.
- Never close last window (per-process) option.
- Rate limiter: cap closes/min; fall back to focusing.
- Special handling for Downloads (shell:Downloads + GUID).

Fix 1.7.0:
- Only the truly NEWER window (by firstSeenTick; tie → larger HWND) is allowed to resolve the duplicate in a scan cycle.
  This prevents the "both windows get closed" race.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CheckIntervalMs: 250
  $name: Polling interval (ms)

- NewWindowGraceMs: 550
  $name: Grace time for NEW window navigation to settle (ms)

- ExistingWindowStableMs: 400
  $name: Require EXISTING window path to be stable for (ms)

- GentleCloseDelayMs: 160
  $name: Delay before closing the other window (ms). Window will be minimized first.

- MaxAutoClosesPerMinute: 3
  $name: Safety fuse (max closes/minute; 0 = unlimited)

- NeverCloseLastWindow: true
  $name: Never close if that would leave zero Explorer windows in the process

- BypassModifier: Shift
  $name: Bypass modifier key
  $options:
  - None: Never bypass
  - Shift: Hold Shift to bypass
  - Ctrl: Hold Ctrl to bypass
  - Alt: Hold Alt to bypass

- DuplicatePolicy: KeepNew
  $name: Duplicate resolution
  $options:
  - KeepNew: Keep the new window (replace the old)
  - KeepOld: Keep the existing window (close the new)

- ActivateKeptWindow: true
  $name: Bring the kept window to front
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <exdisp.h> // interfaces only; we define GUIDs ourselves
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cwctype>

// ---------------- Settings ----------------
static int  gIntervalMs             = 250;
static int  gNewWindowGraceMs       = 550;
static int  gExistingStableMs       = 400;
static int  gGentleCloseDelayMs     = 160;
static int  gMaxClosesPerMinute     = 3;
static bool gNeverCloseLastWindow   = true;
static int  gBypassModifier         = 1;   // 0=None, 1=Shift, 2=Ctrl, 3=Alt
static int  gDuplicatePolicy        = 1;   // 0=KeepOld, 1=KeepNew
static bool gActivateKeptWindow     = true;

// ---------------- State ----------------
static HANDLE gTimerQueue = nullptr;
static HANDLE gTimer      = nullptr;
static LONG   gScanRunning = 0;
static CRITICAL_SECTION gLock;

static std::unordered_map<HWND, std::wstring> gKnownPath;     // hwnd -> path
static std::unordered_map<HWND, DWORD>        gFirstSeenTick; // hwnd -> tick first seen at this path
static std::unordered_map<HWND, DWORD>        gLastChangeTick;// hwnd -> last time path changed
static std::unordered_map<std::wstring, HWND> gPathIndex;     // normalized path -> canonical hwnd

// Rate limiter
static int   gCloseCount = 0;
static DWORD gCloseMinuteStart = 0;

// ---------------- Dynamic COM / OleAut / Shell32 ----------------
static HMODULE gOle32    = nullptr;
static HMODULE gOleAut32 = nullptr;
static HMODULE gShell32  = nullptr;

typedef HRESULT (WINAPI *PFN_CoInitializeEx)(LPVOID, DWORD);
typedef void    (WINAPI *PFN_CoUninitialize)(void);
typedef HRESULT (WINAPI *PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef UINT    (WINAPI *PFN_SysStringLen)(BSTR);
typedef void    (WINAPI *PFN_SysFreeString)(BSTR);
typedef HRESULT (WINAPI *PFN_SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
typedef void    (WINAPI *PFN_CoTaskMemFree)(LPVOID);

static PFN_CoInitializeEx       pCoInitializeEx       = nullptr;
static PFN_CoUninitialize       pCoUninitialize       = nullptr;
static PFN_CoCreateInstance     pCoCreateInstance     = nullptr;
static PFN_SysStringLen         pSysStringLen         = nullptr;
static PFN_SysFreeString        pSysFreeString        = nullptr;
static PFN_SHGetKnownFolderPath pSHGetKnownFolderPath = nullptr;
static PFN_CoTaskMemFree        pCoTaskMemFree        = nullptr;

static void LoadComProcs() {
    if (!gOle32)    gOle32    = LoadLibraryW(L"ole32.dll");
    if (!gOleAut32) gOleAut32 = LoadLibraryW(L"oleaut32.dll");
    if (!gShell32)  gShell32  = LoadLibraryW(L"shell32.dll");

    if (gOle32) {
        pCoInitializeEx       = (PFN_CoInitializeEx)      GetProcAddress(gOle32,   "CoInitializeEx");
        pCoUninitialize       = (PFN_CoUninitialize)      GetProcAddress(gOle32,   "CoUninitialize");
        pCoCreateInstance     = (PFN_CoCreateInstance)    GetProcAddress(gOle32,   "CoCreateInstance");
        pCoTaskMemFree        = (PFN_CoTaskMemFree)       GetProcAddress(gOle32,   "CoTaskMemFree");
    }
    if (gOleAut32) {
        pSysStringLen         = (PFN_SysStringLen)        GetProcAddress(gOleAut32,"SysStringLen");
        pSysFreeString        = (PFN_SysFreeString)       GetProcAddress(gOleAut32,"SysFreeString");
    }
    if (gShell32) {
        pSHGetKnownFolderPath = (PFN_SHGetKnownFolderPath)GetProcAddress(gShell32,"SHGetKnownFolderPath");
    }
}

// ---------------- Inline GUIDs (no uuid.lib needed) ----------------
// CLSID_ShellWindows: 9BA05972-F6A8-11CF-A442-00A0C90A8F39
static const CLSID MY_CLSID_ShellWindows =
{ 0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39} };
// IID_IShellWindows: 85CB6900-4D95-11CF-960C-0080C7F4EE85
static const IID   MY_IID_IShellWindows =
{ 0x85cb6900, 0x4d95, 0x11cf, {0x96, 0x0c, 0x00, 0x80, 0xc7, 0xf4, 0xee, 0x85} };
// IID_IWebBrowser2: D30C1661-CDAF-11d0-8A3E-00C04FC9E26E
static const IID   MY_IID_IWebBrowser2 =
{ 0xd30c1661, 0xcdaf, 0x11d0, {0x8a, 0x3e, 0x00, 0xc0, 0x4f, 0xc9, 0xe2, 0x6e} };
// FOLDERID_Downloads: 374DE290-123F-4565-9164-39C4925E467B
static const GUID  MY_FOLDERID_Downloads =
{ 0x374de290, 0x123f, 0x4565, {0x91, 0x64, 0x39, 0xc4, 0x92, 0x5e, 0x46, 0x7b} };

// ---------------- Utils ----------------
static bool IsBypassHeld() {
    int vk = 0;
    switch (gBypassModifier) {
        case 1: vk = VK_SHIFT;   break;
        case 2: vk = VK_CONTROL; break;
        case 3: vk = VK_MENU;    break;
        default: return false;
    }
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
static bool IsDriveRoot(const std::wstring& p) {
    return p.size() == 3 && p[1] == L':' && (p[2] == L'\\' || p[2] == L'/');
}
static std::wstring GetDownloadsPhysicalPath() {
    if (!pSHGetKnownFolderPath || !pCoTaskMemFree) return L"";
    PWSTR w = nullptr;
    if (SUCCEEDED(pSHGetKnownFolderPath(MY_FOLDERID_Downloads, 0, nullptr, &w)) && w) {
        std::wstring out(w);
        pCoTaskMemFree(w);
        std::transform(out.begin(), out.end(), out.begin(), [](wchar_t c){ return (wchar_t)towlower(c); });
        if (!out.empty() && !IsDriveRoot(out)) {
            while (!out.empty() && (out.back() == L'\\' || out.back() == L'/')) out.pop_back();
        }
        return out;
    }
    return L"";
}
static std::wstring UrlPercentDecode(const std::wstring& s) {
    std::wstring out; out.reserve(s.size());
    auto hex = [](wchar_t c)->int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == L'%' && i + 2 < s.size()) {
            int a = hex(s[i+1]), b = hex(s[i+2]);
            if (a >= 0 && b >= 0) { out.push_back((wchar_t)((a<<4)|b)); i += 2; continue; }
        }
        out.push_back(s[i]);
    }
    return out;
}
static std::wstring WStrFromBSTR(BSTR b) { return b ? std::wstring(b) : L""; }
static std::wstring NormalizeFromLocationUrl(BSTR burl) {
    std::wstring u = WStrFromBSTR(burl);
    if (u.empty()) return L"";
    std::wstring low = u; std::transform(low.begin(), low.end(), low.begin(), [](wchar_t c){ return (wchar_t)towlower(c); });

    const std::wstring prefix = L"file:///";
    if (low.rfind(prefix, 0) == 0) {
        std::wstring path = UrlPercentDecode(u.substr(prefix.size()));
        std::replace(path.begin(), path.end(), L'/', L'\\');
        std::transform(path.begin(), path.end(), path.begin(), [](wchar_t c){ return (wchar_t)towlower(c); });
        if (!path.empty() && !IsDriveRoot(path)) {
            while (!path.empty() && (path.back() == L'\\' || path.back() == L'/')) path.pop_back();
        }
        return path;
    }
    if (low.rfind(L"shell:downloads", 0) == 0 ||
        low.find(L"374de290-123f-4565-9164-39c4925e467b") != std::wstring::npos) {
        return GetDownloadsPhysicalPath();
    }
    return L"";
}

static void ActivateFront(HWND hwnd) {
    if (!gActivateKeptWindow) return;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    SetForegroundWindow(hwnd);
}

// ---- gentle close ----
static DWORD WINAPI GentleCloseThread(LPVOID lp) {
    HWND hwnd = (HWND)lp;
    Sleep(gGentleCloseDelayMs);
    if (IsWindow(hwnd)) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return 0;
}
static void GentleClose(HWND hwnd) {
    if (!IsWindow(hwnd)) return;
    ShowWindow(hwnd, SW_MINIMIZE);
    HANDLE h = CreateThread(nullptr, 0, GentleCloseThread, hwnd, 0, nullptr);
    if (h) CloseHandle(h);
}

// ---- safety: rate limit + never close last window ----
static bool RateAllow(DWORD now) {
    if (!gMaxClosesPerMinute) return true;
    if (!gCloseMinuteStart) gCloseMinuteStart = now;
    if (now - gCloseMinuteStart >= 60*1000) { gCloseMinuteStart = now; gCloseCount = 0; }
    return gCloseCount < gMaxClosesPerMinute;
}
static int CountExplorerWindowsOfPid(const std::unordered_map<HWND,std::wstring>& current, DWORD pid) {
    int n = 0;
    for (auto& kv : current) {
        DWORD p=0; GetWindowThreadProcessId(kv.first, &p);
        if (p == pid) n++;
    }
    return n;
}

// ---- NEW: decide who is the real "newer" window for this path ----
static bool IsNewerWindow(HWND a, HWND b) {
    if (a == b) return false;
    auto ia = gFirstSeenTick.find(a);
    auto ib = gFirstSeenTick.find(b);
    if (ia != gFirstSeenTick.end() && ib != gFirstSeenTick.end()) {
        // positive if a seen later than b
        LONG delta = (LONG)(ia->second - ib->second);
        if (delta != 0) return delta > 0;
    }
    // tie-breaker: higher HWND considered newer (deterministic)
    return (UINT_PTR)a > (UINT_PTR)b;
}

// Main duplicate resolver (assumes caller passed NEWER as hwndNew)
static void ResolveDuplicate(HWND hwndNew, HWND hwndOld, const std::wstring& path,
                             const std::unordered_map<HWND,std::wstring>& current, DWORD now) {
    // Stability checks
    auto itNewFS = gFirstSeenTick.find(hwndNew);
    if (itNewFS == gFirstSeenTick.end() || (int)(now - itNewFS->second) < gNewWindowGraceMs) return;

    auto itOldLC = gLastChangeTick.find(hwndOld);
    if (itOldLC != gLastChangeTick.end()) {
        if ((int)(now - itOldLC->second) < gExistingStableMs) return;
    }

    // Rate limit: fallback to focusing if exceeded
    if (!RateAllow(now)) { ActivateFront(hwndOld); return; }

    // Never close last window of the process (optional)
    if (gNeverCloseLastWindow) {
        DWORD pidOld=0, pidNew=0;
        GetWindowThreadProcessId(hwndOld, &pidOld);
        GetWindowThreadProcessId(hwndNew, &pidNew);
        if (gDuplicatePolicy==0) { // KeepOld -> close new
            if (CountExplorerWindowsOfPid(current, pidNew) <= 1) { ActivateFront(hwndOld); return; }
        } else { // KeepNew -> close old
            if (CountExplorerWindowsOfPid(current, pidOld) <= 1) { ActivateFront(hwndNew); return; }
        }
    }

    // Do the action
    if (gDuplicatePolicy == 0) {
        // KeepOld
        ActivateFront(hwndOld);
        GentleClose(hwndNew);
        gCloseCount++;
        // keep index pointing to old
    } else {
        // KeepNew
        ActivateFront(hwndNew);
        GentleClose(hwndOld);
        gCloseCount++;
        gPathIndex[path] = hwndNew;
    }
}

// Scan loop
static void ScanNow() {
    // COM session (dynamic)
    if (!pCoInitializeEx || !pCoCreateInstance || !pCoUninitialize) return;
    if (FAILED(pCoInitializeEx(nullptr, COINIT_MULTITHREADED))) return;

    IShellWindows* psw = nullptr;
    if (SUCCEEDED(pCoCreateInstance(MY_CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                                    MY_IID_IShellWindows, (void**)&psw))) {
        VARIANT v; ZeroMemory(&v, sizeof(v)); v.vt = VT_I4;
        long count = 0;
        if (SUCCEEDED(psw->get_Count(&count))) {
            std::unordered_map<HWND, std::wstring> current;

            for (long i = 0; i < count; ++i) {
                v.lVal = i;
                IDispatch* pDisp = nullptr;
                if (S_OK != psw->Item(v, &pDisp) || !pDisp) continue;

                IWebBrowser2* pWB = nullptr;
                if (SUCCEEDED(pDisp->QueryInterface(MY_IID_IWebBrowser2, (void**)&pWB)) && pWB) {
                    LONG_PTR raw = 0;
                    if (SUCCEEDED(pWB->get_HWND(&raw))) {
                        HWND hwnd = (HWND)raw;
                        BSTR url = nullptr;
                        if (SUCCEEDED(pWB->get_LocationURL(&url))) {
                            std::wstring norm = NormalizeFromLocationUrl(url);
                            current[hwnd] = norm;
                            if (pSysFreeString) pSysFreeString(url);
                        }
                    }
                    pWB->Release();
                }
                pDisp->Release();
            }

            DWORD now = GetTickCount();
            EnterCriticalSection(&gLock);

            // Track & handle
            for (auto& kv : current) {
                HWND hwnd = kv.first;
                const std::wstring& path = kv.second;

                if (IsBypassHeld()) {
                    // update tracking only
                    auto it = gKnownPath.find(hwnd);
                    if (it == gKnownPath.end()) {
                        gKnownPath[hwnd] = path;
                        gFirstSeenTick[hwnd] = now;
                        gLastChangeTick[hwnd] = now;
                    } else if (it->second != path) {
                        auto itIdx = gPathIndex.find(it->second);
                        if (itIdx != gPathIndex.end() && itIdx->second == hwnd) gPathIndex.erase(itIdx);
                        it->second = path;
                        gFirstSeenTick[hwnd] = now;
                        gLastChangeTick[hwnd] = now;
                    }
                    if (!path.empty()) gPathIndex[path] = hwnd;
                    continue;
                }

                auto itKnown = gKnownPath.find(hwnd);
                if (itKnown == gKnownPath.end()) {
                    gKnownPath[hwnd] = path;
                    gFirstSeenTick[hwnd] = now;
                    gLastChangeTick[hwnd] = now;
                } else if (itKnown->second != path) {
                    auto itIdx = gPathIndex.find(itKnown->second);
                    if (itIdx != gPathIndex.end() && itIdx->second == hwnd) gPathIndex.erase(itIdx);
                    itKnown->second = path;
                    gFirstSeenTick[hwnd] = now;     // restart grace for new path
                    gLastChangeTick[hwnd] = now;    // mark change time
                }

                // Index this path if not yet
                if (!path.empty() && !gPathIndex.count(path)) gPathIndex[path] = hwnd;

                // Try to resolve duplicates (only when NEWER window handles it)
                auto itIdx = gPathIndex.find(path);
                if (!path.empty() && itIdx != gPathIndex.end()) {
                    HWND hwndOld = itIdx->second;
                    if (hwndOld && hwndOld != hwnd) {
                        // 关键修复：只有真正“更新”的那个窗口触发去重
                        if (IsNewerWindow(hwnd, hwndOld)) {
                            ResolveDuplicate(hwnd, hwndOld, path, current, now);
                        }
                    }
                }
            }

            // Cleanup closed windows
            std::vector<HWND> toErase;
            for (auto& kv2 : gKnownPath) {
                if (!current.count(kv2.first)) {
                    auto itIdx = gPathIndex.find(kv2.second);
                    if (itIdx != gPathIndex.end() && itIdx->second == kv2.first) gPathIndex.erase(itIdx);
                    toErase.push_back(kv2.first);
                }
            }
            for (HWND h : toErase) {
                gKnownPath.erase(h);
                gFirstSeenTick.erase(h);
                gLastChangeTick.erase(h);
            }

            LeaveCriticalSection(&gLock);
        }
        psw->Release();
    }

    pCoUninitialize();
}

static VOID CALLBACK TimerCb(PVOID, BOOLEAN) {
    if (InterlockedExchange(&gScanRunning, 1)) return;
    ScanNow();
    InterlockedExchange(&gScanRunning, 0);
}

// ---------------- Windhawk entry ----------------
static void LoadSettings() {
    gIntervalMs         = Wh_GetIntSetting(L"CheckIntervalMs");
    if (gIntervalMs < 50) gIntervalMs = 50; if (gIntervalMs > 5000) gIntervalMs = 5000;

    gNewWindowGraceMs   = Wh_GetIntSetting(L"NewWindowGraceMs");
    if (gNewWindowGraceMs < 0) gNewWindowGraceMs = 0; if (gNewWindowGraceMs > 3000) gNewWindowGraceMs = 3000;

    gExistingStableMs   = Wh_GetIntSetting(L"ExistingWindowStableMs");
    if (gExistingStableMs < 0) gExistingStableMs = 0; if (gExistingStableMs > 3000) gExistingStableMs = 3000;

    gGentleCloseDelayMs = Wh_GetIntSetting(L"GentleCloseDelayMs");
    if (gGentleCloseDelayMs < 60) gGentleCloseDelayMs = 60; if (gGentleCloseDelayMs > 1000) gGentleCloseDelayMs = 1000;

    gMaxClosesPerMinute = Wh_GetIntSetting(L"MaxAutoClosesPerMinute");
    if (gMaxClosesPerMinute < 0) gMaxClosesPerMinute = 0; if (gMaxClosesPerMinute > 60) gMaxClosesPerMinute = 60;

    gNeverCloseLastWindow = Wh_GetIntSetting(L"NeverCloseLastWindow") != 0;

    if (PCWSTR s = Wh_GetStringSetting(L"BypassModifier")) {
        if      (_wcsicmp(s, L"None")  == 0) gBypassModifier = 0;
        else if (_wcsicmp(s, L"Shift") == 0) gBypassModifier = 1;
        else if (_wcsicmp(s, L"Ctrl")  == 0) gBypassModifier = 2;
        else if (_wcsicmp(s, L"Alt")   == 0) gBypassModifier = 3;
        Wh_FreeStringSetting(const_cast<PWSTR>(s));
    }
    if (PCWSTR p = Wh_GetStringSetting(L"DuplicatePolicy")) {
        if (_wcsicmp(p, L"KeepOld") == 0) gDuplicatePolicy = 0; else gDuplicatePolicy = 1;
        Wh_FreeStringSetting(const_cast<PWSTR>(p));
    }

    gActivateKeptWindow = Wh_GetIntSetting(L"ActivateKeptWindow") != 0;
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&gLock);
    LoadSettings();
    LoadComProcs();

    gTimerQueue = CreateTimerQueue();
    if (!gTimerQueue) return TRUE;

    if (!CreateTimerQueueTimer(&gTimer, gTimerQueue, TimerCb, nullptr, 500, gIntervalMs, WT_EXECUTEDEFAULT)) {
        DeleteTimerQueueEx(gTimerQueue, nullptr);
        gTimerQueue = nullptr;
    }
    return TRUE;
}
void Wh_ModUninit() {
    if (gTimer) { DeleteTimerQueueTimer(gTimerQueue, gTimer, nullptr); gTimer = nullptr; }
    if (gTimerQueue) { DeleteTimerQueueEx(gTimerQueue, nullptr); gTimerQueue = nullptr; }
    DeleteCriticalSection(&gLock);

    if (gOle32)    { FreeLibrary(gOle32);    gOle32 = nullptr; }
    if (gOleAut32) { FreeLibrary(gOleAut32); gOleAut32 = nullptr; }
    if (gShell32)  { FreeLibrary(gShell32);  gShell32 = nullptr; }
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    if (gTimer && gTimerQueue) {
        DeleteTimerQueueTimer(gTimerQueue, gTimer, nullptr);
        gTimer = nullptr;
        CreateTimerQueueTimer(&gTimer, gTimerQueue, TimerCb, nullptr, gIntervalMs, gIntervalMs, WT_EXECUTEDEFAULT);
    }
}
