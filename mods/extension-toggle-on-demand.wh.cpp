// ==WindhawkMod==
// @id           extension-toggle-on-demand
// @name         File Extensions Toggle Shortcut
// @version      1.0.0
// @description  Toggles Windows file extensions via a customizable hotkey.
// @author       AuralSX
// @github       https://github.com/AuralSX
// @include      explorer.exe
// @compilerOptions -luser32 -lshell32 -ladvapi32 -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A simple mod that allows you to toggle on/off extensions on files using a hotkey shortcut.

## Features:

* **Customizable Modifiers:** Toggle individual modifier keys (`Ctrl`, `Alt`, `Shift`, `Win`) to tailor the shortcut to your liking.
* **Admin Compatibility Mode (Optional):** Toggle to support File Explorer Dialogs inside elevated apps (Apps Running as Admin).
## Limitations:
* **Third-Party File Browsers:** Because this mod relies on native Windows shell components and views, it will not work in third-party applications that use completely custom file browsers (such as but not limited to **WinRAR**, **7-Zip**).

![Demo](https://i.imgur.com/4v1NsWP.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifier_ctrl: true
  $name: Ctrl
  $description: Require Ctrl key modifier.
- modifier_shift: true
  $name: Shift
  $description: Require Shift key modifier.
- modifier_alt: false
  $name: Alt
  $description: Require Alt key modifier.
- modifier_win: false
  $name: Win
  $description: Require Windows key modifier.
- key_char: e
  $name: Key Character
  $description: The letter or key to press alongside the selected modifiers (e.g., e, r, space).
- admin_compatibility: true
  $name: Admin App Compatibility
  $description: "Enable to work inside administrator-elevated windows (like RTSS Add Dialog). May conflict if another app claims the hotkey. Tip: You can leave this off and toggle on your Desktop; the change will take effect to elevated apps too."
*/
// ==/WindhawkModSettings==

#define INITGUID
#include <windhawk_api.h>
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <algorithm>
#include <cwctype>

const UINT WM_UPDATE_HOOK = WM_APP + 999;
const int HOTKEY_ID = 1001;
const UINT FCIDM_SHVIEW_REFRESH = 28673; // Standard Explorer refresh command ID

HHOOK g_hHook = nullptr;
HANDLE g_hThread = nullptr;
DWORD g_threadId = 0;

struct HotkeyConfig {
    UINT modifiers;
    UINT vk;
    bool adminCompat;
};

// Structure and helper for recursive child window searching
struct FindWindowData {
    PCWSTR targetClassName;
    HWND hwndFound;
};

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    FindWindowData* data = (FindWindowData*)lParam;
    WCHAR className[256];
    if (GetClassNameW(hwnd, className, 256) > 0) {
        if (_wcsicmp(className, data->targetClassName) == 0) {
            data->hwndFound = hwnd;
            return FALSE; // Stop enumeration once found
        }
    }
    return TRUE; // Continue searching
}

HWND FindWindowRecursive(HWND hwndParent, PCWSTR targetClassName) {
    FindWindowData data = { targetClassName, NULL };
    EnumChildWindows(hwndParent, EnumChildProc, (LPARAM)&data);
    return data.hwndFound;
}

// Reads toggle switches, key character, and admin compatibility mode dynamically from settings
HotkeyConfig GetCurrentConfig() {
    bool ctrl  = Wh_GetIntSetting(L"modifier_ctrl") != 0;
    bool shift = Wh_GetIntSetting(L"modifier_shift") != 0;
    bool alt   = Wh_GetIntSetting(L"modifier_alt") != 0;
    bool win   = Wh_GetIntSetting(L"modifier_win") != 0;
    bool admin = Wh_GetIntSetting(L"admin_compatibility") != 0;

    UINT mods = 0;
    if (ctrl)  mods |= MOD_CONTROL;
    if (shift) mods |= MOD_SHIFT;
    if (alt)   mods |= MOD_ALT;
    if (win)   mods |= MOD_WIN;

    if (mods == 0) {
        mods = MOD_CONTROL | MOD_SHIFT;
    }

    UINT vk = 'E';
    PCWSTR keyStr = Wh_GetStringSetting(L"key_char");
    if (keyStr && *keyStr) {
        std::wstring s(keyStr);
        s.erase(0, s.find_first_not_of(L" \t"));
        s.erase(s.find_last_not_of(L" \t") + 1);
        if (!s.empty()) {
            vk = towupper(s[0]);
        }
        Wh_FreeStringSetting(keyStr);
    }
    return { mods, vk, admin };
}

// Core execution logic for toggling extensions
void ToggleFileExtensions() {
    SHELLSTATE ss = { 0 };
    SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, FALSE);
    ss.fShowExtensions = !ss.fShowExtensions;
    SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, TRUE);

    PostMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Explorer");
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH, NULL, NULL);

    HWND hwndForeground = GetForegroundWindow();
    if (hwndForeground) {
        WCHAR className[256];
        if (GetClassNameW(hwndForeground, className, 256) > 0) {
            if (_wcsicmp(className, L"CabinetWClass") == 0 || 
                _wcsicmp(className, L"Progman") == 0 || 
                _wcsicmp(className, L"WorkerW") == 0 || 
                _wcsicmp(className, L"#32770") == 0) {
                
                HWND hShellView = FindWindowRecursive(hwndForeground, L"SHELLDLL_DefView");
                if (hShellView) {
                    SendMessageW(hShellView, WM_COMMAND, FCIDM_SHVIEW_REFRESH, 0);
                }
            }
        }
    }

    HWND hwndFocusCheck = GetForegroundWindow();
    if (hwndFocusCheck) {
        WCHAR className[256];
        if (GetClassNameW(hwndFocusCheck, className, 256) > 0) {
            if (_wcsicmp(className, L"Progman") == 0 || _wcsicmp(className, L"WorkerW") == 0) {
                HWND hProgman = FindWindowW(L"Progman", NULL);
                if (hProgman) {
                    HWND hShellView = FindWindowRecursive(hProgman, L"SHELLDLL_DefView");
                    if (hShellView) {
                        HWND hList = FindWindowRecursive(hShellView, L"SysListView32");
                        if (hList) {
                            DWORD targetThreadId = GetWindowThreadProcessId(hList, NULL);
                            DWORD callingThreadId = GetCurrentThreadId();
                            if (targetThreadId && targetThreadId != callingThreadId) {
                                AttachThreadInput(callingThreadId, targetThreadId, TRUE);
                                SetFocus(hList);
                                AttachThreadInput(callingThreadId, targetThreadId, FALSE);
                            } else {
                                SetFocus(hList);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Low-level keyboard hook procedure (used when admin compatibility is disabled)
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            HotkeyConfig cfg = GetCurrentConfig();
            
            bool ctrlReq  = (cfg.modifiers & MOD_CONTROL) != 0;
            bool shiftReq = (cfg.modifiers & MOD_SHIFT) != 0;
            bool altReq   = (cfg.modifiers & MOD_ALT) != 0;
            bool winReq   = (cfg.modifiers & MOD_WIN) != 0;

            bool ctrlDown  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool altDown   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            bool winDown   = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);

            if (pKey->vkCode == cfg.vk &&
                ctrlDown == ctrlReq &&
                shiftDown == shiftReq &&
                altDown == altReq &&
                winDown == winReq) {

                HWND hwndFocus = GetFocus();
                if (hwndFocus) {
                    WCHAR className[256];
                    if (GetClassNameW(hwndFocus, className, 256) > 0) {
                        if (_wcsicmp(className, L"Edit") == 0 || wcsstr(className, L"Edit") != nullptr) {
                            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
                        }
                    }
                }

                ToggleFileExtensions();
                return 1;
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Dedicated message loop thread supporting both hook methods dynamically
DWORD WINAPI MessageLoopThread(LPVOID lpParam) {
    HotkeyConfig cfg = GetCurrentConfig();
    BOOL hotkeyRegistered = FALSE;

    if (cfg.adminCompat) {
        hotkeyRegistered = RegisterHotKey(NULL, HOTKEY_ID, cfg.modifiers, cfg.vk);
    } else {
        g_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            HWND hwndFocus = GetFocus();
            if (hwndFocus) {
                WCHAR className[256];
                if (GetClassNameW(hwndFocus, className, 256) > 0) {
                    if (_wcsicmp(className, L"Edit") == 0 || wcsstr(className, L"Edit") != nullptr) {
                        continue;
                    }
                }
            }
            ToggleFileExtensions();
        } 
        else if (msg.message == WM_UPDATE_HOOK) {
            if (hotkeyRegistered) {
                UnregisterHotKey(NULL, HOTKEY_ID);
                hotkeyRegistered = FALSE;
            }
            if (g_hHook) {
                UnhookWindowsHookEx(g_hHook);
                g_hHook = nullptr;
            }

            cfg = GetCurrentConfig();
            if (cfg.adminCompat) {
                hotkeyRegistered = RegisterHotKey(NULL, HOTKEY_ID, cfg.modifiers, cfg.vk);
            } else {
                g_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
            }
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hotkeyRegistered) {
        UnregisterHotKey(NULL, HOTKEY_ID);
    }
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
    return 0;
}

BOOL Wh_ModInit() {
    g_hThread = CreateThread(NULL, 0, MessageLoopThread, NULL, 0, &g_threadId);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_UPDATE_HOOK, 0, 0);
    }
}

void Wh_ModUninit() {
    if (g_threadId) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        if (g_hThread) {
            WaitForSingleObject(g_hThread, 2000);
            CloseHandle(g_hThread);
            g_hThread = nullptr;
        }
        g_threadId = 0;
    }
}
