// ==WindhawkMod==
// @id              window-opacity
// @name            Window Opacity
// @description     Set any window's transparency with global hotkeys: Modifier+1 (10%) through Modifier+9 (90%), Modifier+0 resets to 100%
// @version         0.0.1
// @author          Jogai
// @github          https://github.com/Jogai
// @homepage        https://www.scott-software.nl/
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Opacity Hotkeys

Control any window's transparency with global keyboard shortcuts.

## Usage

- **Modifier+1** through **Modifier+9**: Sets the foreground window to
  10%–90% opacity
- **Modifier+0**: Resets the foreground window to 100% (fully opaque)

The default modifier is **Ctrl+Alt**. You can change it in the mod settings.

## How it works

When a hotkey is pressed, the mod applies the `WS_EX_LAYERED` window style and
calls `SetLayeredWindowAttributes` on the current foreground window. When
resetting to 100%, the `WS_EX_LAYERED` style is removed to avoid rendering
artifacts.

All modified windows are automatically restored to full opacity when the mod is
disabled or unloaded.

## Notes

- Windows that already use `WS_EX_LAYERED` with `LWA_COLORKEY` (color-key
  transparency) are skipped to avoid overriding their existing transparency
  mechanism.
- The taskbar and desktop windows are excluded from opacity changes.
- Some UWP/WinUI windows may not respond to layered window attributes due to
  `WS_EX_NOREDIRECTIONBITMAP`. In that case the hotkey will have no visible
  effect.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Modifier: ctrl_alt
  $name: Hotkey Modifier
  $description: >-
    The modifier key combination used with the 0-9 number keys.
  $options:
  - ctrl_alt: Ctrl+Alt
  - ctrl_shift: Ctrl+Shift
  - alt_shift: Alt+Shift
  - win_alt: Win+Alt
  - win_ctrl: Win+Ctrl
  - win_shift: Win+Shift
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <unordered_map>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

// =====================================================================
// Global state
// =====================================================================

static HWND g_hTaskbarWnd = nullptr;

// Unique base ID for our 10 hotkeys (keys 0-9).
static const int kHotkeyIdBase = 0x574F4800;  // "WOH\0"
static const int kHotkeyCount = 10;

// Custom window messages for cross-thread hotkey management.
static UINT g_registerMsg =
    RegisterWindowMessage(L"Windhawk_hotkey_" WH_MOD_ID);
static UINT g_restoreAllMsg =
    RegisterWindowMessage(L"Windhawk_restoreAll_" WH_MOD_ID);

enum { HOTKEY_REGISTER, HOTKEY_UNREGISTER };

// Track windows we have modified so we can restore them on unload.
// Value: true if the window already had WS_EX_LAYERED before we touched it.
static std::unordered_map<HWND, bool> g_modifiedWindows;

// Current modifier flags parsed from settings.
static UINT g_currentModifiers = MOD_CONTROL | MOD_ALT;

// =====================================================================
// Settings
// =====================================================================

static UINT ParseModifier(const wchar_t* value) {
    if (wcscmp(value, L"ctrl_alt") == 0)   return MOD_CONTROL | MOD_ALT;
    if (wcscmp(value, L"ctrl_shift") == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(value, L"alt_shift") == 0)  return MOD_ALT | MOD_SHIFT;
    if (wcscmp(value, L"win_alt") == 0)    return MOD_WIN | MOD_ALT;
    if (wcscmp(value, L"win_ctrl") == 0)   return MOD_WIN | MOD_CONTROL;
    if (wcscmp(value, L"win_shift") == 0)  return MOD_WIN | MOD_SHIFT;
    return MOD_CONTROL | MOD_ALT;
}

static void LoadSettings() {
    WindhawkUtils::StringSetting modifier =
        WindhawkUtils::StringSetting::make(L"Modifier");
    g_currentModifiers = ParseModifier(modifier.get());
    Wh_Log(L"Loaded modifier: 0x%X", g_currentModifiers);
}

// =====================================================================
// Hotkey registration
// =====================================================================

static void RegisterHotkeys(HWND hWnd) {
    for (int i = 0; i < kHotkeyCount; i++) {
        int id = kHotkeyIdBase + i;
        UINT vk = 0x30 + i;  // VK for '0' through '9'
        if (RegisterHotKey(hWnd, id,
                           g_currentModifiers | MOD_NOREPEAT, vk)) {
            Wh_Log(L"Registered hotkey for key %d", i);
        } else {
            Wh_Log(L"Failed to register hotkey for key %d (error=%lu)",
                   i, GetLastError());
        }
    }
}

static void UnregisterHotkeys(HWND hWnd) {
    for (int i = 0; i < kHotkeyCount; i++) {
        UnregisterHotKey(hWnd, kHotkeyIdBase + i);
    }
}

// =====================================================================
// Opacity logic
// =====================================================================

static bool ShouldExcludeWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return true;
    if (hwnd == GetDesktopWindow() || hwnd == GetShellWindow())
        return true;

    WCHAR className[64];
    if (GetClassName(hwnd, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
            _wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0) {
            return true;
        }
    }
    return false;
}

static bool UsesColorKey(HWND hwnd) {
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED))
        return false;

    DWORD dwFlags = 0;
    if (GetLayeredWindowAttributes(hwnd, nullptr, nullptr, &dwFlags)) {
        return (dwFlags & LWA_COLORKEY) != 0;
    }
    return false;
}

static void ApplyOpacity(int digitKey) {
    HWND hwnd = GetForegroundWindow();

    if (ShouldExcludeWindow(hwnd)) {
        Wh_Log(L"Excluded window %p", hwnd);
        return;
    }

    if (UsesColorKey(hwnd)) {
        Wh_Log(L"Window %p uses LWA_COLORKEY, skipping", hwnd);
        return;
    }

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    bool wasLayered = (exStyle & WS_EX_LAYERED) != 0;

    if (digitKey == 0) {
        // Reset to 100% opacity.
        auto it = g_modifiedWindows.find(hwnd);
        if (it != g_modifiedWindows.end()) {
            bool originallyLayered = it->second;
            if (originallyLayered) {
                SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
            } else {
                SetWindowLongPtr(hwnd, GWL_EXSTYLE,
                                 exStyle & ~WS_EX_LAYERED);
            }
            g_modifiedWindows.erase(it);
            Wh_Log(L"Reset window %p to 100%% opacity", hwnd);
        } else if (wasLayered) {
            // Not tracked by us but is layered -- just restore alpha.
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        }
    } else {
        // Set opacity: digit 1 = 10%, digit 9 = 90%.
        BYTE alpha = static_cast<BYTE>((digitKey * 255) / 10);

        // Record original state on first modification.
        if (g_modifiedWindows.find(hwnd) == g_modifiedWindows.end()) {
            g_modifiedWindows[hwnd] = wasLayered;
        }

        if (!wasLayered) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
        Wh_Log(L"Set window %p to %d%% opacity (alpha=%d)",
               hwnd, digitKey * 10, alpha);
    }
}

static void RestoreAllWindows() {
    for (auto& [hwnd, originallyLayered] : g_modifiedWindows) {
        if (!IsWindow(hwnd))
            continue;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (originallyLayered) {
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        } else {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    }
    g_modifiedWindows.clear();
    Wh_Log(L"Restored all modified windows");
}

// =====================================================================
// Taskbar subclass
// =====================================================================

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_HOTKEY: {
            int hotkeyId = static_cast<int>(wParam);
            int digitKey = hotkeyId - kHotkeyIdBase;
            if (digitKey >= 0 && digitKey <= 9) {
                ApplyOpacity(digitKey);
                return 0;
            }
            break;
        }

        default:
            if (uMsg == g_registerMsg) {
                switch (wParam) {
                    case HOTKEY_REGISTER:
                        RegisterHotkeys(hWnd);
                        break;
                    case HOTKEY_UNREGISTER:
                        UnregisterHotkeys(hWnd);
                        break;
                }
                return 0;
            }
            if (uMsg == g_restoreAllMsg) {
                RestoreAllWindows();
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// =====================================================================
// Shell_TrayWnd detection and hooking
// =====================================================================

static void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, TaskbarWindowSubclassProc, 0)) {
        Wh_Log(L"Taskbar window %p subclassed", hWnd);
    } else {
        Wh_Log(L"Failed to subclass taskbar window %p", hWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName =
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Shell_TrayWnd created: %p", hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
        RegisterHotkeys(hWnd);
    }

    return hWnd;
}

static HWND FindCurrentProcessTaskbarWindow() {
    HWND hWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *(HWND*)lParam = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&hWnd);
    return hWnd;
}

// =====================================================================
// Windhawk exports
// =====================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW,
                            (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd = FindCurrentProcessTaskbarWindow();
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_registerMsg, HOTKEY_REGISTER, 0);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_registerMsg, HOTKEY_UNREGISTER, 0);
        SendMessage(g_hTaskbarWnd, g_restoreAllMsg, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTaskbarWnd, TaskbarWindowSubclassProc);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_registerMsg, HOTKEY_UNREGISTER, 0);
    }

    LoadSettings();

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_registerMsg, HOTKEY_REGISTER, 0);
    }
}
