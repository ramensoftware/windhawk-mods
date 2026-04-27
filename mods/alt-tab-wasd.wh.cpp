// ==WindhawkMod==
// @id              alt-tab-wasd
// @name            AltTab WASD
// @description     Use WASD instead Arrow to guide to your Tabs seamlessly
// @version         1.0.0
// @author          Achrllrogia
// @github          https://github.com/achrllrogia45
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AltTab WASD
This mod adds WASD navigation to the Alt+Tab switcher so W, A, S, and D behave
like the Up, Left, Down, and Right arrow keys while the switcher is visible.

It also supports a configurable runtime toggle hotkey. Enable the feature in the
settings, then press the configured hotkey to turn WASD navigation on or off by 
keep holding Alt after Alt+Tab then Ctrl+Shift+\
without restarting Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableWasd: false
  $name: Enable WASD Navigation
  $description: Master switch. When enabled, WASD keys navigate the Alt+Tab switcher like arrow keys.
- hotkey: Ctrl+Alt+Shift+\
  $name: Toggle Hotkey
  $description: Press this hotkey to toggle WASD navigation on/off at runtime. Only works when Enable WASD is on.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <string>

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;
std::atomic<bool> g_altTabVisible{false};
std::atomic<bool> g_wasdActive{true};
bool g_enableWasd = false;

struct Hotkey {
    UINT vk = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
    bool win = false;
};
Hotkey g_hotkey;

HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
std::atomic<DWORD> g_threadIdForAltTabShowWindow{0};
HWND g_taskSwitcherHwnd = nullptr;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

bool IsAlphaToken(const wchar_t* token) {
    return token[0] >= L'A' && token[0] <= L'Z' && token[1] == L'\0';
}

bool IsDigitToken(const wchar_t* token) {
    return token[0] >= L'0' && token[0] <= L'9' && token[1] == L'\0';
}

bool EqualsToken(const wchar_t* left, const wchar_t* right) {
    return _wcsicmp(left, right) == 0;
}

UINT ParseHotkeyVirtualKey(const wchar_t* token) {
    if (!token || token[0] == L'\0') {
        return 0;
    }

    if (IsAlphaToken(token)) {
        return token[0];
    }

    if (IsDigitToken(token)) {
        return token[0];
    }

    if (EqualsToken(token, L"\\") || EqualsToken(token, L"Backslash")) {
        return VK_OEM_5;
    }

    if ((token[0] == L'F' || token[0] == L'f') && token[1] != L'\0' &&
        token[2] == L'\0') {
        if (token[1] >= L'1' && token[1] <= L'9') {
            return VK_F1 + (token[1] - L'1');
        }
    }

    if ((token[0] == L'F' || token[0] == L'f') && token[1] != L'\0' &&
        token[2] != L'\0' && token[3] == L'\0') {
        if (token[1] == L'1' && token[2] >= L'0' && token[2] <= L'2') {
            return VK_F10 + (token[2] - L'0');
        }
    }

    return 0;
}

Hotkey ParseHotkey(const wchar_t* str) {
    Hotkey hotkey;

    if (!str || str[0] == L'\0') {
        return hotkey;
    }

    std::wstring input(str);
    size_t start = 0;

    while (start < input.length()) {
        size_t end = input.find(L'+', start);
        if (end == std::wstring::npos) {
            end = input.length();
        }

        std::wstring token = input.substr(start, end - start);
        if (!token.empty()) {
            if (EqualsToken(token.c_str(), L"Ctrl")) {
                hotkey.ctrl = true;
            } else if (EqualsToken(token.c_str(), L"Alt")) {
                hotkey.alt = true;
            } else if (EqualsToken(token.c_str(), L"Shift")) {
                hotkey.shift = true;
            } else if (EqualsToken(token.c_str(), L"Win")) {
                hotkey.win = true;
            } else {
                hotkey.vk = ParseHotkeyVirtualKey(token.c_str());
            }
        }

        start = end + 1;
    }

    return hotkey;
}

bool IsModifierPressed(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool AreHotkeyModifiersPressed(const Hotkey& hotkey) {
    bool ctrlPressed = IsModifierPressed(VK_CONTROL);
    bool altPressed = IsModifierPressed(VK_MENU);
    bool shiftPressed = IsModifierPressed(VK_SHIFT);
    bool winPressed =
        IsModifierPressed(VK_LWIN) || IsModifierPressed(VK_RWIN);

    return hotkey.ctrl == ctrlPressed && hotkey.alt == altPressed &&
           hotkey.shift == shiftPressed && hotkey.win == winPressed;
}

UINT MapWasdToArrow(UINT vkCode) {
    switch (vkCode) {
        case 'W':
            return VK_UP;
        case 'A':
            return VK_LEFT;
        case 'S':
            return VK_DOWN;
        case 'D':
            return VK_RIGHT;
        default:
            return 0;
    }
}

using XamlAltTabViewHost_ViewLoaded_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_ViewLoaded_t XamlAltTabViewHost_ViewLoaded_Original;
void WINAPI XamlAltTabViewHost_ViewLoaded_Hook(void* pThis) {
    Wh_Log(L">");
    g_altTabVisible = true;
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_ViewLoaded_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using XamlAltTabViewHost_DisplayAltTab_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_DisplayAltTab_t XamlAltTabViewHost_DisplayAltTab_Original;
void WINAPI XamlAltTabViewHost_DisplayAltTab_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_threadIdForAltTabShowWindow) {
        return XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    }

    g_altTabVisible = true;
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    XamlAltTabViewHost_DisplayAltTab_Original(pThis);
    g_threadIdForAltTabShowWindow = 0;
}

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                void* param1,
                                                void* param2,
                                                void* param3);
CAltTabViewHost_Show_t CAltTabViewHost_Show_Original;
HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis,
                                         void* param1,
                                         void* param2,
                                         void* param3) {
    Wh_Log(L">");
    g_altTabVisible = true;
    g_threadIdForAltTabShowWindow = GetCurrentThreadId();
    HRESULT ret = CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    g_threadIdForAltTabShowWindow = 0;
    return ret;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (hWnd == g_taskSwitcherHwnd && nCmdShow == SW_HIDE) {
        g_altTabVisible = false;
        g_taskSwitcherHwnd = nullptr;
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (g_threadIdForAltTabShowWindow == GetCurrentThreadId() &&
        nCmdShow != SW_HIDE) {
        g_taskSwitcherHwnd = hWnd;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* keyboardInfo = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (keyboardInfo->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    bool isKeyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool isKeyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
    UINT vkCode = keyboardInfo->vkCode;

    if (isKeyDown && g_enableWasd && g_hotkey.vk != 0 &&
        vkCode == g_hotkey.vk && AreHotkeyModifiersPressed(g_hotkey)) {
        g_wasdActive = !g_wasdActive.load();
        Wh_Log(L"WASD navigation %s",
               g_wasdActive ? L"enabled" : L"disabled");
        return 1;
    }

    if (!g_altTabVisible || !g_wasdActive) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    UINT arrowVk = MapWasdToArrow(vkCode);
    if (arrowVk == 0 || (!isKeyDown && !isKeyUp)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(arrowVk);
    input.ki.wScan =
        static_cast<WORD>(MapVirtualKey(arrowVk, MAPVK_VK_TO_VSC));
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    if (isKeyUp) {
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
    }

    if (SendInput(1, &input, sizeof(INPUT)) != 1) {
        Wh_Log(L"SendInput failed: %u", GetLastError());
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    return 1;
}

DWORD WINAPI KeyboardHookThread(LPVOID lpParam) {
    HHOOK hook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (!hook) {
        Wh_Log(L"Failed to set keyboard hook: %u", GetLastError());
        return 1;
    }
    Wh_Log(L"Keyboard hook installed");
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hook);
    Wh_Log(L"Keyboard hook removed");
    return 0;
}

void StartHookThread() {
    if (g_hookThread) {
        return;
    }

    g_wasdActive = true;
    g_hookThread =
        CreateThread(nullptr, 0, KeyboardHookThread, nullptr, 0,
                     &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"Failed to create hook thread: %u", GetLastError());
        g_hookThreadId = 0;
    }
}

void StopHookThread() {
    if (!g_hookThread) {
        return;
    }

    PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hookThread, 5000);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    g_hookThreadId = 0;
}

void LoadSettings() {
    g_enableWasd = !!Wh_GetIntSetting(L"enableWasd");

    LPCWSTR hotkeySetting = Wh_GetStringSetting(L"hotkey");
    g_hotkey = ParseHotkey(hotkeySetting);
    Wh_FreeStringSetting(hotkeySetting);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_winVersion = GetWindowsVersion();
    LoadSettings();

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (g_winVersion == WinVersion::Win11) {
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl XamlAltTabViewHost::ViewLoaded(void))"},
                &XamlAltTabViewHost_ViewLoaded_Original,
                XamlAltTabViewHost_ViewLoaded_Hook,
            },
            {
                {LR"(private: void __cdecl XamlAltTabViewHost::DisplayAltTab(void))"},
                &XamlAltTabViewHost_DisplayAltTab_Original,
                XamlAltTabViewHost_DisplayAltTab_Hook,
                true,
            },
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
                true,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            return FALSE;
        }
    } else if (g_winVersion == WinVersion::Win10) {
        WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                         ARRAYSIZE(twinuiPcshellSymbolHooks))) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(ShowWindow, ShowWindow_Hook,
                                       &ShowWindow_Original);

    if (g_enableWasd) {
        StartHookThread();
    }

    return TRUE;
}

void Wh_ModUninit() {
    StopHookThread();
}

void Wh_ModSettingsChanged() {
    bool wasEnabled = g_enableWasd;
    LoadSettings();

    if (g_enableWasd && !wasEnabled) {
        StartHookThread();
    }

    if (!g_enableWasd && wasEnabled) {
        StopHookThread();
    }
}