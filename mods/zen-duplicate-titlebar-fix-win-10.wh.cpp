// ==WindhawkMod==
// @id              zen-duplicate-titlebar-fix-win-10
// @name            Zen Duplicate Titlebar Fix (Win10)
// @description     Removes the extra Windows titlebar buttons that can appear over Zen's own controls when using transparency mods on Windows 10.
// @version         1.0.0
// @author          landnthrn
// @github          https://github.com/landnthrn
// @homepage        https://github.com/landnthrn/zen-duplicate-titlebar-fix-win10
// @include         zen.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Duplicate titlebar buttons can occur on Zen when using transparent browser mods with DWMBlurGlass or OpenGlass, on Windows 10.     
This mod hides the extra Windows-native titlebar buttons and lets Zen's remain.

### Before
![Before fix](https://raw.githubusercontent.com/landnthrnnn/DUMP/refs/heads/main/GitHub/repos/zen-duplicate-titlebar-fix/before.png)

### After
![After fix](https://raw.githubusercontent.com/landnthrnnn/DUMP/refs/heads/main/GitHub/repos/zen-duplicate-titlebar-fix/after.png)

## How It Works

- Removes the `WS_SYSMENU` window style from Zen's main browser windows, preventing Windows from drawing its native titlebar buttons over Zen's.   
- Prevents Zen from restoring that style while the mod is active.
- Involves removing the native Windows system menu from being used on Zen, normally accessible with `alt + space`.
- Only targets `zen.exe` for it's main window(s), external windows from Zen such as Library or Browser Console are not targetted.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <atomic>
#include <cwchar>
#include <windhawk_utils.h>

namespace {

constexpr wchar_t kWindowClass[] = L"MozillaWindowClass";
constexpr wchar_t kMarker[] =
    L"Windhawk.ZenHideDuplicateCaptionButtons";

constexpr LONG_PTR kRequiredStyles =
    WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

constexpr UINT kRefreshFlags =
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
    SWP_FRAMECHANGED;

using SetWindowPos_t = decltype(&SetWindowPos);
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);

SetWindowPos_t SetWindowPos_Original = nullptr;
SetWindowLongPtrW_t SetWindowLongPtrW_Original = nullptr;

std::atomic<bool> g_unloading{false};
thread_local bool g_internalFrameChange = false;

class InternalFrameChangeGuard {
public:
    InternalFrameChangeGuard() : previous_(g_internalFrameChange) {
        g_internalFrameChange = true;
    }

    ~InternalFrameChangeGuard() {
        g_internalFrameChange = previous_;
    }

private:
    bool previous_;
};

bool IsWindows10() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
        return false;

    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
        GetProcAddress(ntdll, "RtlGetVersion")
    );
    if (!rtlGetVersion)
        return false;

    OSVERSIONINFOW version = {};
    version.dwOSVersionInfoSize = sizeof(version);
    return rtlGetVersion(&version) == 0 &&
           version.dwMajorVersion == 10 &&
           version.dwBuildNumber < 22000;
}

bool IsZenMainProcess() {
    const wchar_t* commandLine = GetCommandLineW();
    return commandLine && !wcsstr(commandLine, L"-contentproc");
}

// Identity check only; a window can temporarily lose caption styles in F11.
bool IsZenTopLevelWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != GetCurrentProcessId())
        return false;

    if (GetWindow(hwnd, GW_OWNER) != nullptr ||
        GetAncestor(hwnd, GA_ROOT) != hwnd)
        return false;

    wchar_t className[128] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, kWindowClass) != 0)
        return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    return !(style & WS_CHILD) && !(exStyle & WS_EX_TOOLWINDOW);
}

// A custom titlebar extends the client area into the usual caption region.
// Native-titlebar utility windows keep a full caption-height offset.
bool HasCustomTitlebar(HWND hwnd) {
    RECT windowRect = {};
    RECT clientRect = {};
    POINT clientTopLeft = {};
    if (!GetWindowRect(hwnd, &windowRect) ||
        !GetClientRect(hwnd, &clientRect) ||
        !ClientToScreen(hwnd, &clientTopLeft) ||
        IsRectEmpty(&windowRect) || IsRectEmpty(&clientRect)) {
        return false;
    }

    int captionHeight = GetSystemMetricsForDpi(
        SM_CYCAPTION, GetDpiForWindow(hwnd)
    );
    int clientTopOffset = clientTopLeft.y - windowRect.top;
    return captionHeight > 0 && clientTopOffset < captionHeight;
}

void RefreshFrame(HWND hwnd) {
    SetWindowPos_Original(hwnd, nullptr, 0, 0, 0, 0, kRefreshFlags);
}

bool ChangeWindowStyle(HWND hwnd, LONG_PTR updatedStyle) {
    SetLastError(ERROR_SUCCESS);
    LONG_PTR previous = SetWindowLongPtrW_Original(
        hwnd, GWL_STYLE, updatedStyle
    );
    return previous != 0 || GetLastError() == ERROR_SUCCESS;
}

void ApplyFix(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_SYSMENU))
        return;

    InternalFrameChangeGuard guard;
    if (!ChangeWindowStyle(
            hwnd, style & ~static_cast<LONG_PTR>(WS_SYSMENU))) {
        Wh_Log(L"Failed to hide Zen caption buttons: %p", hwnd);
        return;
    }

    if (!GetPropW(hwnd, kMarker) &&
        !SetPropW(hwnd, kMarker, reinterpret_cast<HANDLE>(1))) {
        ChangeWindowStyle(hwnd, style);
        RefreshFrame(hwnd);
        Wh_Log(L"Failed to mark Zen window: %p", hwnd);
        return;
    }

    RefreshFrame(hwnd);
    Wh_Log(L"Removed duplicate caption buttons: %p", hwnd);
}

void RestoreFix(HWND hwnd) {
    if (!IsZenTopLevelWindow(hwnd) || !GetPropW(hwnd, kMarker))
        return;

    InternalFrameChangeGuard guard;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_SYSMENU) &&
        !ChangeWindowStyle(
            hwnd, style | static_cast<LONG_PTR>(WS_SYSMENU))) {
        Wh_Log(L"Failed to restore Zen window style: %p", hwnd);
        return;
    }

    RemovePropW(hwnd, kMarker);
    RefreshFrame(hwnd);
    Wh_Log(L"Restored native window controls: %p", hwnd);
}

void SyncWindow(HWND hwnd) {
    if (g_unloading || g_internalFrameChange || !IsZenTopLevelWindow(hwnd))
        return;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & kRequiredStyles) != kRequiredStyles) {
        // Fullscreen or a transitional frame: keep the marker until exit.
        return;
    }

    if (HasCustomTitlebar(hwnd))
        ApplyFix(hwnd);
    else
        RestoreFix(hwnd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND insertAfter, int x, int y,
                             int width, int height, UINT flags) {
    BOOL result = SetWindowPos_Original(
        hwnd, insertAfter, x, y, width, height, flags
    );

    // Gecko updates its custom non-client margins on frame changes.
    if (result && !g_internalFrameChange &&
        (flags & (SWP_FRAMECHANGED | SWP_SHOWWINDOW))) {
        SyncWindow(hwnd);
    }
    return result;
}

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hwnd, int index,
                                      LONG_PTR newValue) {
    // Only keep WS_SYSMENU suppressed on windows we already identified.
    // Inspect the incoming style, not the current one (F11 restores it).
    if (index == GWL_STYLE && !g_unloading && !g_internalFrameChange &&
        (newValue & kRequiredStyles) == kRequiredStyles &&
        IsZenTopLevelWindow(hwnd) && GetPropW(hwnd, kMarker)) {
        newValue &= ~static_cast<LONG_PTR>(WS_SYSMENU);
    }
    return SetWindowLongPtrW_Original(hwnd, index, newValue);
}

BOOL CALLBACK SyncExistingWindow(HWND hwnd, LPARAM) {
    SyncWindow(hwnd);
    return TRUE;
}

BOOL CALLBACK RestoreExistingWindow(HWND hwnd, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId())
        RestoreFix(hwnd);
    return TRUE;
}

} // namespace

BOOL Wh_ModInit() {
    if (!IsWindows10()) {
        Wh_Log(L"Windows 10 only; no hooks installed");
        return FALSE;
    }
    if (!IsZenMainProcess()) {
        // Content, GPU and utility processes don't own browser windows.
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
        Wh_Log(L"Failed to hook SetWindowPos");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(
            SetWindowLongPtrW, SetWindowLongPtrW_Hook,
            &SetWindowLongPtrW_Original)) {
        Wh_Log(L"Failed to hook SetWindowLongPtrW");
        return FALSE;
    }

    Wh_Log(L"Zen duplicate titlebar fix initialized");
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(SyncExistingWindow, 0);
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    EnumWindows(RestoreExistingWindow, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Zen duplicate titlebar fix unloaded");
}
