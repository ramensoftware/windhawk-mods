// ==WindhawkMod==
// @id              taskview-button-next-desktop
// @name            Task View Button: Next Desktop
// @description     Left-clicking the taskbar Task View button switches to the next virtual desktop instead of opening Task View (Windows Server 2022)
// @version         1.0
// @author          SurpriseAwofemi
// @github          https://github.com/surpriseawofemi
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task View Button: Next Desktop

Changes what the taskbar **Task View** button does:

- **Left click** → switch to the next virtual desktop instead of opening
  Task View.

Right click and other mouse buttons are left untouched, and Task View can
still be opened with **Win+Tab**.

When **Wrap around** is enabled (the default), clicking the button on the last
desktop goes back to the first one.

## Compatibility

**Windows Server 2022 (build 20348) only.** Desktop switching uses an
undocumented internal shell interface whose ID differs between Windows
versions. On other versions the mod does nothing and logs a message.

## Usage

Create at least two virtual desktops (Win+Tab → "New desktop"), then click the
Task View button on the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wrapAround: true
  $name: Wrap around
  $description: When on the last desktop, go back to the first one.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <servprov.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct {
    bool wrapAround;
} g_settings;

// Control ID of the Task View "TrayButton" in the classic taskbar.
constexpr int kTaskViewButtonId = 4113;

constexpr UINT WM_APP_NEXT_DESKTOP = WM_APP + 1;

HANDLE g_thread;
DWORD g_threadId;
HHOOK g_mouseHook;
bool g_swallowNextLeftUp;

// ---------------------------------------------------------------------------
// Hit testing

bool IsPointOnTaskViewButton(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return false;
    }

    // The button must belong to this explorer process.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hwnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"TrayButton") != 0) {
        return false;
    }

    return GetDlgCtrlID(hwnd) == kTaskViewButtonId;
}

// ---------------------------------------------------------------------------
// Desktop switching
//
// Uses the shell's internal virtual desktop manager (undocumented). The GUID
// and method layout below are for Windows Server 2022 (build 20348).

const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
const GUID SID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};
const IID IID_IVirtualDesktopManagerInternal = {
    0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}};

enum AdjacentDesktop {
    LeftDirection = 3,
    RightDirection = 4,
};

struct IVirtualDesktopManagerInternal : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(HMONITOR monitor,
                                               int* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IUnknown* view,
                                                        IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IUnknown* view,
                                                          BOOL* can) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetCurrentDesktop(HMONITOR monitor, IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(HMONITOR monitor,
                                                  IUnknown** desktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetAdjacentDesktop(IUnknown* from, int direction, IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(HMONITOR monitor,
                                                    IUnknown* desktop) = 0;
};

ComPtr<IVirtualDesktopManagerInternal> GetDesktopManager() {
    ComPtr<IServiceProvider> serviceProvider;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr,
                                  CLSCTX_LOCAL_SERVER,
                                  IID_PPV_ARGS(&serviceProvider));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(ImmersiveShell) failed: %08X", hr);
        return nullptr;
    }

    ComPtr<IVirtualDesktopManagerInternal> manager;
    hr = serviceProvider->QueryService(SID_VirtualDesktopManagerInternal,
                                       IID_IVirtualDesktopManagerInternal,
                                       (void**)manager.GetAddressOf());
    if (FAILED(hr)) {
        Wh_Log(L"QueryService(VirtualDesktopManagerInternal) failed: %08X",
               hr);
        return nullptr;
    }

    return manager;
}

void SwitchToNextDesktop() {
    ComPtr<IVirtualDesktopManagerInternal> manager = GetDesktopManager();
    if (!manager) {
        return;
    }

    ComPtr<IUnknown> current;
    HRESULT hr = manager->GetCurrentDesktop(nullptr, &current);
    if (FAILED(hr)) {
        Wh_Log(L"GetCurrentDesktop failed: %08X", hr);
        return;
    }

    ComPtr<IUnknown> target;
    hr = manager->GetAdjacentDesktop(current.Get(), RightDirection, &target);
    if (FAILED(hr)) {
        // No desktop to the right: we're on the last one.
        if (!g_settings.wrapAround) {
            return;
        }

        // Walk left until reaching the first desktop.
        ComPtr<IUnknown> desktop = current;
        ComPtr<IUnknown> left;
        while (SUCCEEDED(manager->GetAdjacentDesktop(
            desktop.Get(), LeftDirection, left.ReleaseAndGetAddressOf()))) {
            desktop = left;
        }

        if (desktop == current) {
            return;  // Only one desktop.
        }
        target = desktop;
    }

    hr = manager->SwitchDesktop(nullptr, target.Get());
    if (FAILED(hr)) {
        Wh_Log(L"SwitchDesktop failed: %08X", hr);
    }
}

// ---------------------------------------------------------------------------
// Mouse hook

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        bool injected = info->flags & LLMHF_INJECTED;

        if (!injected) {
            if (wParam == WM_LBUTTONDOWN) {
                if (IsPointOnTaskViewButton(info->pt)) {
                    g_swallowNextLeftUp = true;
                    PostThreadMessage(g_threadId, WM_APP_NEXT_DESKTOP, 0, 0);
                    return 1;  // Swallow: don't open Task View.
                }
            } else if (wParam == WM_LBUTTONUP && g_swallowNextLeftUp) {
                g_swallowNextLeftUp = false;
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                                   GetModuleHandle(nullptr), 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        if (SUCCEEDED(hrCo)) {
            CoUninitialize();
        }
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_NEXT_DESKTOP) {
            SwitchToNextDesktop();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points

// Windows Server 2022.
constexpr DWORD kSupportedBuild = 20348;

DWORD GetWindowsBuildNumber() {
    using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
    auto pRtlGetVersion = (RtlGetVersion_t)GetProcAddress(
        GetModuleHandle(L"ntdll.dll"), "RtlGetVersion");
    if (!pRtlGetVersion) {
        return 0;
    }

    RTL_OSVERSIONINFOW info = {};
    info.dwOSVersionInfoSize = sizeof(info);
    if (pRtlGetVersion(&info) != 0) {
        return 0;
    }

    return info.dwBuildNumber;
}

void LoadSettings() {
    g_settings.wrapAround = Wh_GetIntSetting(L"wrapAround");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    DWORD build = GetWindowsBuildNumber();
    if (build != kSupportedBuild) {
        Wh_Log(L"Unsupported Windows build %u (only %u is supported)", build,
               kSupportedBuild);
        return FALSE;
    }

    LoadSettings();

    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_thread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
