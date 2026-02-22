// ==WindhawkMod==
// @id              win10-taskbar-per-app-volume-control
// @name            Win10 Taskbar Volume Control
// @description     Adjust per-app volume by scrolling the mouse wheel over Win10 taskbar buttons
// @version         1.0.1
// @author          Anixx
// @github 			https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lcomctl32 -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Control

**!Important!** This mod works with the Windows 10 taskbar running either under Windows 10 or Windows 11.
For Windows 11 taskbar, use this mod: [Taskbar Volume Control Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app).

Scroll the mouse wheel over an application's taskbar button to adjust its
volume. A tooltip follows your cursor showing the current volume level.

- **Scroll Up**: Increase volume
- **Scroll Down**: Decrease volume
- Only affects apps that have an active audio session
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeStep: 2
  $name: Volume Step (%)
  $description: How much to change volume per scroll tick (1-20)
*/
// ==/WindhawkModSettings==


#include <windowsx.h>
#include <commctrl.h>
#include <oleacc.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_set>

struct {
    int volumeStep;
} g_settings;

static HWND g_tooltipWnd = nullptr;
static UINT_PTR g_hideTimerId = 0;
static bool g_initialized = false;
static WCHAR g_volumeLabel[64] = L"Volume";

// ─── Tooltip ────────────────────────────────────────────────────

static void CALLBACK HideTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    g_hideTimerId = 0;
    if (g_tooltipWnd) {
        TTTOOLINFOW ti{};
        ti.cbSize = sizeof(ti);
        ti.hwnd = nullptr;
        ti.uId = 1;
        SendMessageW(g_tooltipWnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    }
}

static void ShowVolumeTooltip(int volumePercent, POINT pt) {
    if (!g_tooltipWnd) {
        g_tooltipWnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT,
            TOOLTIPS_CLASSW, nullptr,
            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
        if (!g_tooltipWnd) return;

        TTTOOLINFOW ti{};
        ti.cbSize = sizeof(ti);
        ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
        ti.hwnd = nullptr;
        ti.uId = 1;
        ti.lpszText = (LPWSTR)L"";
        SendMessageW(g_tooltipWnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    }

    WCHAR buf[128];
    wsprintfW(buf, L"%s: %d%%", g_volumeLabel, volumePercent);

    TTTOOLINFOW ti{};
    ti.cbSize = sizeof(ti);
    ti.hwnd = nullptr;
    ti.uId = 1;
    ti.lpszText = buf;
    SendMessageW(g_tooltipWnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    SendMessageW(g_tooltipWnd, TTM_TRACKPOSITION, 0,
                 MAKELPARAM(pt.x + 16, pt.y - 40));
    SendMessageW(g_tooltipWnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);

    if (g_hideTimerId) KillTimer(nullptr, g_hideTimerId);
    g_hideTimerId = SetTimer(nullptr, 0, 1500, HideTimerProc);
}

// ─── Process tree ───────────────────────────────────────────────

static std::unordered_set<DWORD> GetProcessTree(DWORD rootPid) {
    std::unordered_set<DWORD> pids;
    pids.insert(rootPid);

    std::vector<std::pair<DWORD, DWORD>> procList;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(snap, &pe)) {
            do {
                procList.push_back({ pe.th32ProcessID, pe.th32ParentProcessID });
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& p : procList) {
            if (pids.count(p.second) && !pids.count(p.first)) {
                pids.insert(p.first);
                changed = true;
            }
        }
    }
    return pids;
}

// ─── Audio volume ───────────────────────────────────────────────

static bool AdjustProcessVolume(DWORD targetPid, float newVolume, float* outVolume) {
    auto pids = GetProcessTree(targetPid);
    bool found = false;

    IMMDeviceEnumerator* pEnum = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                  (void**)&pEnum);
    if (FAILED(hr) || !pEnum) return false;

    IMMDevice* pDevice = nullptr;
    hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr) || !pDevice) { pEnum->Release(); return false; }

    IAudioSessionManager2* pMgr = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                           nullptr, (void**)&pMgr);
    if (FAILED(hr) || !pMgr) { pDevice->Release(); pEnum->Release(); return false; }

    IAudioSessionEnumerator* pSessions = nullptr;
    hr = pMgr->GetSessionEnumerator(&pSessions);
    if (FAILED(hr) || !pSessions) {
        pMgr->Release(); pDevice->Release(); pEnum->Release(); return false;
    }

    int count = 0;
    pSessions->GetCount(&count);

    for (int i = 0; i < count; i++) {
        IAudioSessionControl* pCtl = nullptr;
        if (FAILED(pSessions->GetSession(i, &pCtl)) || !pCtl) continue;

        IAudioSessionControl2* pCtl2 = nullptr;
        if (SUCCEEDED(pCtl->QueryInterface(__uuidof(IAudioSessionControl2),
                                           (void**)&pCtl2)) && pCtl2) {
            DWORD sessionPid = 0;
            if (SUCCEEDED(pCtl2->GetProcessId(&sessionPid)) && pids.count(sessionPid)) {
                ISimpleAudioVolume* pVol = nullptr;
                if (SUCCEEDED(pCtl2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                    (void**)&pVol)) && pVol) {
                    if (newVolume >= 0.0f) {
                        pVol->SetMasterVolume(
                            (std::max)(0.0f, (std::min)(1.0f, newVolume)), nullptr);
                    }
                    float vol = 0.0f;
                    pVol->GetMasterVolume(&vol);
                    if (outVolume) *outVolume = vol;
                    found = true;
                    pVol->Release();
                }
            }
            pCtl2->Release();
        }
        pCtl->Release();
        if (found) break;
    }

    pSessions->Release();
    pMgr->Release();
    pDevice->Release();
    pEnum->Release();
    return found;
}

// ─── Get PID from taskbar button ────────────────────────────────

struct WindowFindCtx {
    const WCHAR* name;
    size_t nameLen;
    DWORD bestPid;
    size_t bestMatchLen;
};

static BOOL CALLBACK EnumWindowsFindProc(HWND hwnd, LPARAM lp) {
    WindowFindCtx* c = (WindowFindCtx*)lp;
    if (!IsWindowVisible(hwnd)) return TRUE;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner && !(exStyle & WS_EX_APPWINDOW)) return TRUE;

    WCHAR title[512];
    int len = GetWindowTextW(hwnd, title, 512);
    if (len == 0) return TRUE;

    std::wstring accName(c->name, c->nameLen);
    std::wstring wTitle(title, len);

    size_t matchLen = 0;
    bool match = false;

    if (accName == wTitle) {
        match = true;
        matchLen = wTitle.length() * 2;
    } else if (accName.find(wTitle) != std::wstring::npos) {
        match = true;
        matchLen = wTitle.length();
    } else if (wTitle.find(accName) != std::wstring::npos) {
        match = true;
        matchLen = accName.length();
    }

    if (match && matchLen > c->bestMatchLen) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        c->bestPid = pid;
        c->bestMatchLen = matchLen;
    }
    return TRUE;
}

static DWORD GetPidFromTaskbarPoint(POINT screenPt) {
    IAccessible* pAcc = nullptr;
    VARIANT varChild;
    VariantInit(&varChild);

    HRESULT hr = AccessibleObjectFromPoint(screenPt, &pAcc, &varChild);
    if (FAILED(hr) || !pAcc) return 0;

    VARIANT varRole;
    VariantInit(&varRole);
    hr = pAcc->get_accRole(varChild, &varRole);
    bool isButton = false;
    if (SUCCEEDED(hr) && varRole.vt == VT_I4) {
        isButton = (varRole.lVal == ROLE_SYSTEM_PUSHBUTTON ||
                    varRole.lVal == ROLE_SYSTEM_LISTITEM ||
                    varRole.lVal == ROLE_SYSTEM_BUTTONMENU);
    }
    VariantClear(&varRole);

    if (!isButton) {
        VariantClear(&varChild);
        pAcc->Release();
        return 0;
    }

    BSTR bstrName = nullptr;
    hr = pAcc->get_accName(varChild, &bstrName);
    VariantClear(&varChild);
    pAcc->Release();

    if (FAILED(hr) || !bstrName) return 0;

    size_t nameLen = SysStringLen(bstrName);
    if (nameLen == 0) { SysFreeString(bstrName); return 0; }

    WindowFindCtx ctx;
    ctx.name = bstrName;
    ctx.nameLen = nameLen;
    ctx.bestPid = 0;
    ctx.bestMatchLen = 0;
    EnumWindows(EnumWindowsFindProc, (LPARAM)&ctx);

    SysFreeString(bstrName);
    return ctx.bestPid;
}

// ─── Point over task list check ─────────────────────────────────

static bool IsPointOverTaskList(POINT screenPt) {
    HWND hwnd = WindowFromPoint(screenPt);
    while (hwnd) {
        WCHAR cls[256];
        GetClassNameW(hwnd, cls, 256);
        if (wcscmp(cls, L"MSTaskListWClass") == 0)
            return true;
        hwnd = GetParent(hwnd);
    }
    return false;
}

// ─── Hook DispatchMessageW ──────────────────────────────────────

using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
DispatchMessageW_t g_origDispatchMessageW = nullptr;

LRESULT WINAPI HookedDispatchMessageW(const MSG* lpMsg) {
    if (g_initialized && lpMsg && lpMsg->message == WM_MOUSEWHEEL) {
        POINT pt;
        pt.x = GET_X_LPARAM(lpMsg->lParam);
        pt.y = GET_Y_LPARAM(lpMsg->lParam);

        if (IsPointOverTaskList(pt)) {
            short delta = GET_WHEEL_DELTA_WPARAM(lpMsg->wParam);
            DWORD pid = GetPidFromTaskbarPoint(pt);
            if (pid) {
                float step = (float)g_settings.volumeStep / 100.0f;
                float currentVol = -1.0f;
                AdjustProcessVolume(pid, -1.0f, &currentVol);

                if (currentVol >= 0.0f) {
                    float newVol = currentVol + (delta > 0 ? step : -step);
                    newVol = (std::max)(0.0f, (std::min)(1.0f, newVol));
                    float resultVol = 0.0f;
                    if (AdjustProcessVolume(pid, newVol, &resultVol)) {
                        int pct = (int)(resultVol * 100.0f + 0.5f);
                        ShowVolumeTooltip(pct, pt);
                        return 0;
                    }
                }
            }
        }
    }
    return g_origDispatchMessageW(lpMsg);
}

// ─── Settings ───────────────────────────────────────────────────

static void LoadSettings() {
    g_settings.volumeStep = Wh_GetIntSetting(L"volumeStep");
    if (g_settings.volumeStep < 1) g_settings.volumeStep = 1;
    if (g_settings.volumeStep > 20) g_settings.volumeStep = 20;
}

static void LoadVolumeLabel() {
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (hShell32) {
        int len = LoadStringW(hShell32, 22066, g_volumeLabel,
                              sizeof(g_volumeLabel) / sizeof(g_volumeLabel[0]));
        if (len == 0) {
            wcscpy(g_volumeLabel, L"Volume");
        }
        FreeLibrary(hShell32);
    }
}

// ─── Mod lifecycle ──────────────────────────────────────────────

BOOL Wh_ModInit() {
    LoadSettings();
    LoadVolumeLabel();

    Wh_SetFunctionHook((void*)DispatchMessageW,
                        (void*)HookedDispatchMessageW,
                        (void**)&g_origDispatchMessageW);

    g_initialized = true;
    return TRUE;
}

void Wh_ModUninit() {
    g_initialized = false;

    if (g_hideTimerId) {
        KillTimer(nullptr, g_hideTimerId);
        g_hideTimerId = 0;
    }
    if (g_tooltipWnd) {
        DestroyWindow(g_tooltipWnd);
        g_tooltipWnd = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
