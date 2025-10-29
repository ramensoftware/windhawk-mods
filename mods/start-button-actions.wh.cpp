// ==WindhawkMod==
// @id           start-button-actions
// @name         Start Button Actions
// @description  Assign custom actions to Start button clicks and Windows key press
// @version      1.0.0
// @author       Asteski
// @github       https://github.com/Asteski
// @include      explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -ladvapi32 -lole32 -loleaut32 -luuid
// @architecture x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Button Actions

This mod allows you to assign custom actions to various Start button clicks and Windows key press while preserving all default Windows key combinations (Win+R, Win+L, Win+D, etc.).

## Features

* **Start Button Click Actions**: Configure different actions for left-click, right-click, middle-click, Shift+click, and Ctrl+click on the Start button
* **Windows Key Custom Action**: Run any command when pressing Windows key alone
* **Preserves All Shortcuts**: All Windows key combinations (Win+R, Win+L, Win+D, etc.) continue to work normally  
* **Environment Variables**: Support for environment variables in custom commands (e.g., %USERPROFILE%)

## Settings

### Start Button Actions
Each type of click on the Start button can be configured independently:
- **Left Click**: Default Start menu behavior or custom command
- **Right Click**: Default WinX menu behavior or custom command
- **Middle Click**: Custom command or disabled
- **Shift+Left Click**: Custom command or disabled
- **Ctrl+Left Click**: Custom command or disabled

### Windows Key Action
- **Default**: Opens the default Windows start menu
- **Custom Command**: Executes a custom command specified below
- **Disabled**: Windows key behaves normally (no custom action)

### Elevation for Custom Commands
- **Run Custom Actions As**: Choose whether custom commands run as standard user (unelevated) or as administrator. When Windhawk runs elevated, selecting "Standard user" ensures your commands run without admin rights.

### Command Examples
- `notepad.exe` - Opens Notepad
- `cmd.exe` - Opens Command Prompt
- `%PROGRAMFILES%\Everything\Everything.exe` - Launch Everything search
- `powershell.exe -Command "Get-Process"` - PowerShell command

## How It Works

The mod installs low-level keyboard and mouse hooks that monitor Start button clicks and Windows key events:

1. **Start Button**: Detects mouse clicks and modifier keys, executes configured actions
2. **Windows Key**: Detects solo key press vs combinations, executes custom action only when pressed alone
3. All default Windows functionality is preserved

## Compatibility

- Works on Windows 10 and Windows 11
- Does not interfere with existing Windows functionality
- Can be safely disabled/enabled without system restart

## Issues

- Custom action bound to the Windows key doesn’t activate on the first attempt when an elevated window is in focus

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startButtonLeftClickAction: default
  $name: Start Button Left Click Action
  $description: What to do when Start button is left-clicked
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- startButtonLeftClickCommand: explorer.exe %USERPROFILE%
  $name: Start Button Left Click Command
  $description: Command to execute for Start button left click (default opens user profile folder)
- startButtonRightClickAction: default
  $name: Start Button Right Click Action
  $description: What to do when Start button is right-clicked
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- startButtonRightClickCommand: cmd.exe /k systeminfo
  $name: Start Button Right Click Command
  $description: Command to execute for Start button right click (default shows system information)
- startButtonMiddleClickAction: default
  $name: Start Button Middle Click Action
  $description: What to do when Start button is middle-clicked
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- startButtonMiddleClickCommand: calc.exe
  $name: Start Button Middle Click Command
  $description: Command to execute for Start button middle click (default opens calculator)
- startButtonShiftLeftClickAction: default
  $name: Start Button Shift+Left Click Action
  $description: What to do when Start button is Shift+left-clicked
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- startButtonShiftLeftClickCommand: taskmgr.exe
  $name: Start Button Shift+Left Click Command
  $description: Command to execute for Start button Shift+left click (default opens Task Manager)
- startButtonCtrlLeftClickAction: default
  $name: Start Button Ctrl+Left Click Action
  $description: What to do when Start button is Ctrl+left-clicked
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- startButtonCtrlLeftClickCommand: notepad.exe
  $name: Start Button Ctrl+Left Click Command
  $description: Command to execute for Start button Ctrl+left click (default opens Notepad)
- windowsKeyAction: default
  $name: Windows Key Action
  $description: What to do when Windows key is pressed alone
  $options:
  - default: Default (normal behavior)
  - custom: Custom Command
  - disabled: Disabled (do nothing)
- windowsKeyCommand: control.exe
  $name: Windows Key Custom Command
  $description: Command to execute for Windows key action (default opens Control Panel)
- keyTimeout: 100
  $name: Key Timeout (ms)
  $description: Minimum time Windows key must be held (shorter = more responsive)
  $options:
  - 50: 50ms (Very Fast)
  - 100: 100ms (Fast)
  - 150: 150ms (Default)
  - 200: 200ms (Conservative)
- runCustomActionsAs: user
  $name: Run Custom Actions As
  $description: Elevation to use when launching custom commands
  $options:
   - user: Standard user (unelevated)
   - admin: Administrator (elevated)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shldisp.h>
#include <ole2.h>
#include <oleauto.h>
#include <string>
#include <atomic>
#ifndef Wh_Log
#define Wh_Log(...) do {} while (0)
#endif

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

// Global variables
HHOOK g_keyboardHook = nullptr;
HHOOK g_mouseHook = nullptr;
std::atomic<bool> g_windowsKeyPressed{false};
std::atomic<bool> g_otherKeyPressed{false};
std::atomic<bool> g_actionExecuted{false};
DWORD g_windowsKeyPressTime = 0;
std::atomic<bool> g_quit{false};
HANDLE g_executeEvent = nullptr;
HANDLE g_executeStartButtonEvent = nullptr;
std::atomic<int> g_startButtonClickType{0}; // 0=none, 1=left, 2=right, 3=middle, 4=shift+left, 5=ctrl+left
std::atomic<bool> g_suppressNextMouseUp{false}; // To suppress UP events after suppressing DOWN
std::atomic<bool> g_suppressCtrlKey{false}; // To suppress Ctrl key when handling Ctrl+Click
HANDLE g_workerThread = nullptr;
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HANDLE g_hookStartEvent = nullptr;
HWND g_startButtonHwnd = nullptr;
RECT g_startButtonRect = {0};
std::atomic<bool> g_hotkeysDisabled{false}; // Track if we've disabled shell hotkeys
// No startup grace period needed; handle first key immediately

// Settings
std::string g_windowsKeyAction = "default";
std::string g_windowsKeyCommand = "control.exe";
std::string g_runCustomActionsAs = "user"; // user | admin
std::string g_startButtonLeftClickAction = "default";
std::string g_startButtonLeftClickCommand = "explorer.exe %USERPROFILE%";
std::string g_startButtonRightClickAction = "default";
std::string g_startButtonRightClickCommand = "cmd.exe /k systeminfo";
std::string g_startButtonMiddleClickAction = "default";
std::string g_startButtonMiddleClickCommand = "calc.exe";
std::string g_startButtonShiftLeftClickAction = "default";
std::string g_startButtonShiftLeftClickCommand = "taskmgr.exe";
std::string g_startButtonCtrlLeftClickAction = "default";
std::string g_startButtonCtrlLeftClickCommand = "notepad.exe";
int g_keyTimeout = 150;

// Original RegisterHotKey function pointer
using RegisterHotKey_t = decltype(&RegisterHotKey);
RegisterHotKey_t g_pOriginalRegisterHotKey = nullptr;

// Hooked RegisterHotKey to prevent shell from registering Win key and Ctrl+Esc
BOOL WINAPI RegisterHotKeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
    // Block Win key (MOD_WIN + vk=0) and Ctrl+Esc if in custom mode
    if (g_windowsKeyAction == "custom") {
        if ((fsModifiers == MOD_WIN && vk == 0) || 
            (fsModifiers == MOD_CONTROL && vk == VK_ESCAPE)) {
            Wh_Log(L"Windows Key Actions: Blocked RegisterHotKey (mod=%u, vk=%u)", fsModifiers, vk);
            return FALSE; // Pretend it failed
        }
    }
    
    // Allow other hotkeys through
    return g_pOriginalRegisterHotKey(hWnd, id, fsModifiers, vk);
}

// Forward declarations
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
void ExecuteCustomAction();
void ExecuteStartButtonAction();
void LoadSettings();
bool IsWindowsKey(DWORD vkCode);
void ExecuteCommand(const std::string& command);
HMODULE GetThisModule();
std::wstring GetModuleDirectory();
std::string WideToUtf8(const std::wstring& w);
std::wstring Utf8ToWide(const std::string& s);
DWORD WINAPI WorkerThreadProc(LPVOID);
DWORD WINAPI HookThreadProc(LPVOID);
void CloseStartMenuIfOpen();
void SendVirtualKey(WORD vk, bool down);
void ClickStartButton();
void UpdateStartButtonInfo();
void DisableShellHotkeys();
void RestoreShellHotkeys();
bool ShellExecuteUnelevated(const wchar_t* file, const wchar_t* params, int nShow = SW_SHOWNORMAL);
bool CreateProcessWithExplorerToken(const wchar_t* app, wchar_t* cmdMutable, int nShow = SW_SHOWNORMAL);

// APC callback to unregister hotkeys from the correct thread context
void NTAPI UnregisterHotkeysAPC(ULONG_PTR Parameter) {
    UnregisterHotKey(NULL, 1);  // Win key
    UnregisterHotKey(NULL, 2);  // Ctrl+Esc
    Wh_Log(L"Windows Key Actions: Unregistered hotkeys via APC");
}

// Disable Windows shell hotkeys (Win key and Ctrl+Esc)
void DisableShellHotkeys() {
    if (g_hotkeysDisabled.load() || g_windowsKeyAction != "custom") {
        return;
    }
    
    // Find the window that registered the Start menu hotkeys
    // On Windows 10/11, this is typically the ApplicationManager window
    HWND startMenuWnd = FindWindowW(L"Windows.UI.Core.CoreWindow", L"Start");
    if (!startMenuWnd) {
        // Try alternate window class for Windows 11
        startMenuWnd = FindWindowW(L"ApplicationFrameWindow", nullptr);
    }
    if (!startMenuWnd) {
        // Fallback to Immersive Shell window
        startMenuWnd = FindWindowW(L"ApplicationManager_ImmersiveShellWindow", nullptr);
    }
    
    if (startMenuWnd) {
        // Get the thread that owns the Start menu window
        DWORD threadId = GetWindowThreadProcessId(startMenuWnd, nullptr);
        if (threadId) {
            HANDLE thread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadId);
            if (thread) {
                // Queue an APC to unregister hotkeys in the correct thread context
                QueueUserAPC(UnregisterHotkeysAPC, thread, 0);
                CloseHandle(thread);
                g_hotkeysDisabled = true;
                Wh_Log(L"Windows Key Actions: Queued APC to disable shell hotkeys");
            } else {
                Wh_Log(L"Windows Key Actions: Failed to open thread, error=%lu", GetLastError());
            }
        }
    } else {
        Wh_Log(L"Windows Key Actions: Could not find Start menu window");
    }
}

// Restore Windows shell hotkeys (called on cleanup)
void RestoreShellHotkeys() {
    if (!g_hotkeysDisabled.load()) {
        return;
    }
    
    // Note: We don't need to re-register the hotkeys on cleanup
    // Windows will re-register them automatically when needed
    
    g_hotkeysDisabled = false;
    Wh_Log(L"Windows Key Actions: Hotkeys will be restored by Windows");
}

// Settings loading
void LoadSettings() {
    // Load from Windhawk settings
    PCWSTR windowsKeyAction = Wh_GetStringSetting(L"windowsKeyAction");
    PCWSTR windowsKeyCommand = Wh_GetStringSetting(L"windowsKeyCommand");
    PCWSTR runCustomActionsAs = Wh_GetStringSetting(L"runCustomActionsAs");
    PCWSTR startButtonLeftClickAction = Wh_GetStringSetting(L"startButtonLeftClickAction");
    PCWSTR startButtonLeftClickCommand = Wh_GetStringSetting(L"startButtonLeftClickCommand");
    PCWSTR startButtonRightClickAction = Wh_GetStringSetting(L"startButtonRightClickAction");
    PCWSTR startButtonRightClickCommand = Wh_GetStringSetting(L"startButtonRightClickCommand");
    PCWSTR startButtonMiddleClickAction = Wh_GetStringSetting(L"startButtonMiddleClickAction");
    PCWSTR startButtonMiddleClickCommand = Wh_GetStringSetting(L"startButtonMiddleClickCommand");
    PCWSTR startButtonShiftLeftClickAction = Wh_GetStringSetting(L"startButtonShiftLeftClickAction");
    PCWSTR startButtonShiftLeftClickCommand = Wh_GetStringSetting(L"startButtonShiftLeftClickCommand");
    PCWSTR startButtonCtrlLeftClickAction = Wh_GetStringSetting(L"startButtonCtrlLeftClickAction");
    PCWSTR startButtonCtrlLeftClickCommand = Wh_GetStringSetting(L"startButtonCtrlLeftClickCommand");
    g_keyTimeout = Wh_GetIntSetting(L"keyTimeout");
    
    // Convert wide strings to UTF-8
    if (windowsKeyAction) {
        g_windowsKeyAction = WideToUtf8(windowsKeyAction);
        Wh_FreeStringSetting(windowsKeyAction);
    } else {
        g_windowsKeyAction = "default";
    }
    
    if (windowsKeyCommand) {
        g_windowsKeyCommand = WideToUtf8(windowsKeyCommand);
        Wh_FreeStringSetting(windowsKeyCommand);
    } else {
        g_windowsKeyCommand = "control.exe";
    }
    
    if (runCustomActionsAs) {
        g_runCustomActionsAs = WideToUtf8(runCustomActionsAs);
        Wh_FreeStringSetting(runCustomActionsAs);
    } else {
        g_runCustomActionsAs = "user";
    }
    
    if (startButtonLeftClickAction) {
        g_startButtonLeftClickAction = WideToUtf8(startButtonLeftClickAction);
        Wh_FreeStringSetting(startButtonLeftClickAction);
    } else {
        g_startButtonLeftClickAction = "default";
    }
    
    if (startButtonLeftClickCommand) {
        g_startButtonLeftClickCommand = WideToUtf8(startButtonLeftClickCommand);
        Wh_FreeStringSetting(startButtonLeftClickCommand);
    } else {
        g_startButtonLeftClickCommand = "explorer.exe %USERPROFILE%";
    }
    
    if (startButtonRightClickAction) {
        g_startButtonRightClickAction = WideToUtf8(startButtonRightClickAction);
        Wh_FreeStringSetting(startButtonRightClickAction);
    } else {
        g_startButtonRightClickAction = "default";
    }
    
    if (startButtonRightClickCommand) {
        g_startButtonRightClickCommand = WideToUtf8(startButtonRightClickCommand);
        Wh_FreeStringSetting(startButtonRightClickCommand);
    } else {
        g_startButtonRightClickCommand = "cmd.exe /k systeminfo";
    }
    
    if (startButtonMiddleClickAction) {
        g_startButtonMiddleClickAction = WideToUtf8(startButtonMiddleClickAction);
        Wh_FreeStringSetting(startButtonMiddleClickAction);
    } else {
        g_startButtonMiddleClickAction = "default";
    }
    
    if (startButtonMiddleClickCommand) {
        g_startButtonMiddleClickCommand = WideToUtf8(startButtonMiddleClickCommand);
        Wh_FreeStringSetting(startButtonMiddleClickCommand);
    } else {
        g_startButtonMiddleClickCommand = "calc.exe";
    }
    
    if (startButtonShiftLeftClickAction) {
        g_startButtonShiftLeftClickAction = WideToUtf8(startButtonShiftLeftClickAction);
        Wh_FreeStringSetting(startButtonShiftLeftClickAction);
    } else {
        g_startButtonShiftLeftClickAction = "default";
    }
    
    if (startButtonShiftLeftClickCommand) {
        g_startButtonShiftLeftClickCommand = WideToUtf8(startButtonShiftLeftClickCommand);
        Wh_FreeStringSetting(startButtonShiftLeftClickCommand);
    } else {
        g_startButtonShiftLeftClickCommand = "taskmgr.exe";
    }
    
    if (startButtonCtrlLeftClickAction) {
        g_startButtonCtrlLeftClickAction = WideToUtf8(startButtonCtrlLeftClickAction);
        Wh_FreeStringSetting(startButtonCtrlLeftClickAction);
    } else {
        g_startButtonCtrlLeftClickAction = "default";
    }
    
    if (startButtonCtrlLeftClickCommand) {
        g_startButtonCtrlLeftClickCommand = WideToUtf8(startButtonCtrlLeftClickCommand);
        Wh_FreeStringSetting(startButtonCtrlLeftClickCommand);
    } else {
        g_startButtonCtrlLeftClickCommand = "notepad.exe";
    }
    
    // Set default timeout if not specified
    if (g_keyTimeout == 0) {
        g_keyTimeout = 100;
    }
    
    Wh_Log(L"Windows Key Actions: Settings loaded - timeout=%d", g_keyTimeout);
}

// Check if the virtual key code is a Windows key
bool IsWindowsKey(DWORD vkCode) {
    return (vkCode == VK_LWIN || vkCode == VK_RWIN);
}

// Execute the configured custom action
void ExecuteCustomAction() {
    if (g_windowsKeyAction == "disabled") {
        return;
    }
    
    if (g_windowsKeyAction == "default") {
        // Simulate Windows key press to open Start Menu
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    }
    else if (g_windowsKeyAction == "custom") {
        // Execute custom command directly - Start menu should already be suppressed by the hook
        Wh_Log(L"Windows Key Actions: executing Windows key command: %S", g_windowsKeyCommand.c_str());
        ExecuteCommand(g_windowsKeyCommand);
    }
}

// Execute a command with environment variable expansion
void ExecuteCommand(const std::string& command) {
    if (command.empty()) {
        return;
    }
    
    // Convert to wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, nullptr, 0);
    if (wlen <= 0) return;
    
    std::wstring wcommand(wlen - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, &wcommand[0], wlen);
    
    // Expand environment variables
    wchar_t expandedCommand[MAX_PATH * 2];
    DWORD result = ExpandEnvironmentStringsW(wcommand.c_str(), expandedCommand, sizeof(expandedCommand)/sizeof(wchar_t));
    if (result == 0 || result > sizeof(expandedCommand)/sizeof(wchar_t)) {
        // If expansion fails, use original command
        wcscpy_s(expandedCommand, wcommand.c_str());
    }
    
    // Parse command into executable and parameters (robustly handles quotes)
    std::wstring expandedStr(expandedCommand);
    std::wstring executable;
    std::wstring parameters;

    if (!expandedStr.empty() && expandedStr[0] == L'"') {
        // Quoted executable path
        size_t endQuote = expandedStr.find(L'"', 1);
        if (endQuote != std::wstring::npos) {
            executable = expandedStr.substr(1, endQuote - 1);
            if (endQuote + 1 < expandedStr.size()) {
                if (expandedStr[endQuote + 1] == L' ') {
                    parameters = expandedStr.substr(endQuote + 2);
                } else {
                    parameters = expandedStr.substr(endQuote + 1);
                }
            }
        } else {
            // Malformed quotes; fallback to whole string
            executable = expandedStr;
        }
    } else {
        size_t spacePos = expandedStr.find(L' ');
        if (spacePos != std::wstring::npos) {
            executable = expandedStr.substr(0, spacePos);
            parameters = expandedStr.substr(spacePos + 1);
        } else {
            executable = expandedStr;
        }
    }

    // Decide elevation behavior
    bool wantUnelevated = (g_runCustomActionsAs == "user");

    if (wantUnelevated) {
        // Try launching unelevated via Explorer's COM automation first
        if (ShellExecuteUnelevated(executable.c_str(), parameters.empty() ? nullptr : parameters.c_str(), SW_SHOWNORMAL)) {
            return;
        }
        // Fallback: try token duplication from explorer
        std::wstring cmdLine = L"\"" + executable + L"\"";
        if (!parameters.empty()) {
            cmdLine.push_back(L' ');
            cmdLine += parameters;
        }
        wchar_t mutableCmd[MAX_PATH * 2];
        wcsncpy_s(mutableCmd, cmdLine.c_str(), _TRUNCATE);
        if (CreateProcessWithExplorerToken(executable.c_str(), mutableCmd, SW_SHOWNORMAL)) {
            return;
        }
        // As a last resort, fall back to elevated launch to avoid total failure
    }

    // Elevated or fallback path: use ShellExecute, then CreateProcess if needed
    HINSTANCE hResult = ShellExecuteW(nullptr, L"open", executable.c_str(),
                                      parameters.empty() ? nullptr : parameters.c_str(),
                                      nullptr, SW_SHOWNORMAL);

    if ((INT_PTR)hResult <= 32) {
        STARTUPINFOW si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);
        wchar_t cmdCopy[MAX_PATH * 2];
        wcscpy_s(cmdCopy, expandedCommand);
        if (CreateProcessW(nullptr, cmdCopy, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

// Low-level keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
        
        // Suppress Ctrl key if we're handling Ctrl+Click on Start button
        if (g_suppressCtrlKey && (kbd->vkCode == VK_CONTROL || kbd->vkCode == VK_LCONTROL || kbd->vkCode == VK_RCONTROL)) {
            if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                // Reset flag on Ctrl key up
                g_suppressCtrlKey = false;
            }
            Wh_Log(L"Windows Key Actions: Suppressing Ctrl key event");
            return 1;
        }
        
        // If disabled, pass everything through unmodified
        if (g_windowsKeyAction == "disabled") {
            return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
        }
        
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            if (IsWindowsKey(kbd->vkCode)) {
                Wh_Log(L"Windows Key Actions: Win key down received (vk=%u, flags=0x%08X, injected=%d)", 
                       kbd->vkCode, kbd->flags, !!(kbd->flags & LLKHF_INJECTED));
                
                // Ignore injected Win key downs (from our own re-injection or system)
                if (kbd->flags & LLKHF_INJECTED) {
                    Wh_Log(L"Windows Key Actions: ignoring injected Win key down");
                    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
                }
                
                // Windows key pressed
                if (!g_windowsKeyPressed) {
                    g_windowsKeyPressed = true;
                    g_otherKeyPressed = false;
                    g_actionExecuted = false;
                    g_windowsKeyPressTime = GetTickCount();
                    Wh_Log(L"Windows Key Actions: Win key down (vk=%u, flags=0x%08X) - blocking in custom mode", kbd->vkCode, kbd->flags);
                    
                    // Block Win key down in custom mode to prevent Start menu
                    // Since we've also hooked RegisterHotKey and unregistered existing hotkeys,
                    // blocking the key should be sufficient
                    if (g_windowsKeyAction == "custom") {
                        return 1;
                    }
                }
            }
            else if (g_windowsKeyPressed) {
                // Ignore injected keys in combos (from our re-injection)
                if (kbd->flags & LLKHF_INJECTED) {
                    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
                }
                
                // Another key pressed while Windows key is down - this is a combination
                if (!g_otherKeyPressed) {
                    g_otherKeyPressed = true;
                    Wh_Log(L"Windows Key Actions: combo detected (vk=%u) - re-injecting Win key", kbd->vkCode);
                    
                    // Re-inject the Win key down we blocked so the combo works
                    if (g_windowsKeyAction == "custom") {
                        INPUT in{};
                        in.type = INPUT_KEYBOARD;
                        in.ki.wVk = VK_LWIN;
                        in.ki.dwFlags = 0;
                        SendInput(1, &in, sizeof(in));
                    }
                }
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            if (IsWindowsKey(kbd->vkCode)) {
                // Ignore injected Win key ups
                if (kbd->flags & LLKHF_INJECTED) {
                    Wh_Log(L"Windows Key Actions: ignoring injected Win key up");
                    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
                }
                
                if (g_windowsKeyPressed) {
                    bool wasSolo = !g_otherKeyPressed;
                    
                    // Reset state before potentially blocking
                    g_windowsKeyPressed = false;
                    g_otherKeyPressed = false;
                    
                    // Check if it was pressed alone
                    if (wasSolo && !g_actionExecuted) {
                        // This was a solo Windows key press - execute custom action
                        Wh_Log(L"Windows Key Actions: Win key solo pressed - executing action");
                        if (g_executeEvent) {
                            SetEvent(g_executeEvent); // signal worker to execute
                        } else {
                            // Fallback if event/thread not created
                            ExecuteCustomAction();
                        }
                        g_actionExecuted = true;
                        
                        // Block the Windows key UP to prevent start menu from opening
                        if (g_windowsKeyAction == "custom") {
                            Wh_Log(L"Windows Key Actions: suppressing Win key up to prevent Start");
                            return 1; // Suppress this key event
                        }
                    } else if (!wasSolo && g_windowsKeyAction == "custom") {
                        // For combos, we need to suppress the real UP and send injected UP
                        // to properly close the combo
                        INPUT in{};
                        in.type = INPUT_KEYBOARD;
                        in.ki.wVk = VK_LWIN;
                        in.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &in, sizeof(in));
                        return 1; // Block the real UP
                    }
                    
                    Wh_Log(L"Windows Key Actions: Win key up (solo=%d)", wasSolo);
                }
            }
        }
    }
    
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Windows Key Actions: initializing");
    
    // Create worker synchronization objects
    g_executeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_executeEvent) {
        Wh_Log(L"Windows Key Actions: CreateEvent failed, error=%lu", GetLastError());
        return FALSE;
    }
    
    g_executeStartButtonEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_executeStartButtonEvent) {
        Wh_Log(L"Windows Key Actions: CreateEvent (Start button) failed, error=%lu", GetLastError());
        CloseHandle(g_executeEvent);
        g_executeEvent = nullptr;
        return FALSE;
    }
    
    g_quit = false;
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Windows Key Actions: CreateThread failed, error=%lu", GetLastError());
        CloseHandle(g_executeEvent);
        g_executeEvent = nullptr;
        CloseHandle(g_executeStartButtonEvent);
        g_executeStartButtonEvent = nullptr;
        return FALSE;
    }

    // Start hook thread which installs WH_KEYBOARD_LL and pumps messages
    g_hookStartEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr); // manual-reset, non-signaled
    if (!g_hookStartEvent) {
        Wh_Log(L"Windows Key Actions: CreateEvent (hook start) failed, error=%lu", GetLastError());
        // cleanup worker
        g_quit = true;
        SetEvent(g_executeEvent);
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, 2000);
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
        }
        if (g_executeEvent) { CloseHandle(g_executeEvent); g_executeEvent = nullptr; }
        if (g_executeStartButtonEvent) { CloseHandle(g_executeStartButtonEvent); g_executeStartButtonEvent = nullptr; }
        return FALSE;
    }
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"Windows Key Actions: CreateThread (hook) failed, error=%lu", GetLastError());
        CloseHandle(g_hookStartEvent);
        g_hookStartEvent = nullptr;
        // cleanup worker
        g_quit = true;
        SetEvent(g_executeEvent);
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, 2000);
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
        }
        if (g_executeEvent) { CloseHandle(g_executeEvent); g_executeEvent = nullptr; }
        if (g_executeStartButtonEvent) { CloseHandle(g_executeStartButtonEvent); g_executeStartButtonEvent = nullptr; }
        return FALSE;
    }
    // Wait briefly for hook to install
    DWORD wait = WaitForSingleObject(g_hookStartEvent, 3000);
    if (wait != WAIT_OBJECT_0 || g_keyboardHook == nullptr) {
        Wh_Log(L"Windows Key Actions: hook thread didn't initialize in time (wait=%lu)", wait);
        // cleanup hook thread
        if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        CloseHandle(g_hookStartEvent);
        g_hookStartEvent = nullptr;
        // cleanup worker
        g_quit = true;
        SetEvent(g_executeEvent);
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, 2000);
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
        }
        if (g_executeEvent) { CloseHandle(g_executeEvent); g_executeEvent = nullptr; }
        return FALSE;
    }
    
    // Hook RegisterHotKey to prevent shell from re-registering Win key hotkeys
    Wh_SetFunctionHook((void*)RegisterHotKey, (void*)RegisterHotKeyHook, (void**)&g_pOriginalRegisterHotKey);
    
    // Disable Windows shell hotkeys if in custom mode
    DisableShellHotkeys();
    
    Wh_Log(L"Windows Key Actions: initialized successfully");
    // No grace period; hook active immediately
    return TRUE;
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    // Stop worker
    g_quit = true;
    if (g_executeEvent) SetEvent(g_executeEvent);
    if (g_executeStartButtonEvent) SetEvent(g_executeStartButtonEvent);
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_executeEvent) {
        CloseHandle(g_executeEvent);
        g_executeEvent = nullptr;
    }
    if (g_executeStartButtonEvent) {
        CloseHandle(g_executeStartButtonEvent);
        g_executeStartButtonEvent = nullptr;
    }
    // Stop hook thread and unhook in that thread
    if (g_hookThread) {
        if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }
    if (g_hookStartEvent) {
        CloseHandle(g_hookStartEvent);
        g_hookStartEvent = nullptr;
    }
    // Fallback: ensure hooks are removed if still set
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    
    // Restore Windows shell hotkeys
    RestoreShellHotkeys();
    
    Wh_Log(L"Windows Key Actions: uninitialized");
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    std::string oldAction = g_windowsKeyAction;
    LoadSettings();
    UpdateStartButtonInfo();
    
    // Enable/disable hotkeys based on mode change
    if (oldAction != g_windowsKeyAction) {
        if (g_windowsKeyAction == "custom") {
            DisableShellHotkeys();
        } else {
            RestoreShellHotkeys();
        }
    }
}

// Helpers
HMODULE GetThisModule() {
    HMODULE h = nullptr;
    // Use any function from this module to query its HMODULE
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&Wh_ModInit), &h)) {
        return h;
    }
    return nullptr;
}

std::wstring GetModuleDirectory() {
    wchar_t path[MAX_PATH];
    path[0] = 0;
    HMODULE h = GetThisModule();
    if (h && GetModuleFileNameW(h, path, _countof(path))) {
        wchar_t* filePart = wcsrchr(path, L'\\');
        if (filePart) *(filePart + 1) = 0; // keep trailing backslash
        return path;
    }
    // Fallback to system temp if something goes wrong
    wchar_t tmp[MAX_PATH];
    GetTempPathW(_countof(tmp), tmp);
    return tmp;
}

std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return std::string();
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(len ? len - 1 : 0, '\0');
    if (len)
        WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &s[0], len, nullptr, nullptr);
    return s;
}

std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty()) return std::wstring();
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(len ? len - 1 : 0, L'\0');
    if (len)
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], len);
    return w;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Dedicated thread to execute actions outside of the hook context
    HANDLE events[] = { g_executeEvent, g_executeStartButtonEvent };
    for (;;) {
        DWORD w = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (g_quit.load()) break;
        
        if (w == WAIT_OBJECT_0) {
            // Windows key action
            Wh_Log(L"Windows Key Actions: worker executing Windows key action");
            ExecuteCustomAction();
        } else if (w == WAIT_OBJECT_0 + 1) {
            // Start button action
            Wh_Log(L"Windows Key Actions: worker executing Start button action");
            ExecuteStartButtonAction();
        }
    }
    return 0;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    Wh_Log(L"Windows Key Actions: hook thread starting");
    HMODULE hMod = GetThisModule();
    
    // Install keyboard hook
    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    if (!g_keyboardHook) {
        DWORD err = GetLastError();
        Wh_Log(L"Windows Key Actions: SetWindowsHookEx (keyboard) failed, error=%lu", err);
        if (g_hookStartEvent) SetEvent(g_hookStartEvent);
        return 0;
    }
    Wh_Log(L"Windows Key Actions: keyboard hook installed successfully");
    
    // Install mouse hook
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
    if (!g_mouseHook) {
        DWORD err = GetLastError();
        Wh_Log(L"Windows Key Actions: SetWindowsHookEx (mouse) failed, error=%lu", err);
    } else {
        Wh_Log(L"Windows Key Actions: mouse hook installed successfully");
    }
    
    // Update Start button info initially
    UpdateStartButtonInfo();
    
    if (g_hookStartEvent) SetEvent(g_hookStartEvent);

    // Pump messages to receive low-level hook callbacks
    MSG msg;
    while (!g_quit.load()) {
        BOOL gm = GetMessageW(&msg, nullptr, 0, 0);
        if (gm == 0 || gm == -1) break; // WM_QUIT or error
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    Wh_Log(L"Windows Key Actions: hook thread exiting");
    return 0;
}

void SendVirtualKey(WORD vk, bool down) {
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &in, sizeof(in));
}

void CloseStartMenuIfOpen() {
    // Sending ESC will close the Start menu if it's open; harmless otherwise
    SendVirtualKey(VK_ESCAPE, true);
    SendVirtualKey(VK_ESCAPE, false);
}

void ClickStartButton() {
    // Find the Start button window
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        Wh_Log(L"Windows Key Actions: taskbar not found");
        return;
    }
    
    HWND startButton = FindWindowExW(taskbar, nullptr, L"Start", nullptr);
    if (!startButton) {
        // On Windows 11, try alternate class name
        startButton = FindWindowExW(taskbar, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
    }
    
    if (!startButton) {
        Wh_Log(L"Windows Key Actions: Start button not found");
        return;
    }
    
    // Get the button's rectangle
    RECT rect;
    if (!GetWindowRect(startButton, &rect)) {
        Wh_Log(L"Windows Key Actions: failed to get Start button rect");
        return;
    }
    
    // Calculate center point
    int x = (rect.left + rect.right) / 2;
    int y = (rect.top + rect.bottom) / 2;
    
    Wh_Log(L"Windows Key Actions: clicking Start button at (%d, %d)", x, y);
    
    // Save current cursor position
    POINT oldPos;
    GetCursorPos(&oldPos);
    
    // Move to Start button and click
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    
    // Restore cursor position
    SetCursorPos(oldPos.x, oldPos.y);
}

// Update Start button window and rect info
void UpdateStartButtonInfo() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        g_startButtonHwnd = nullptr;
        return;
    }
    
    HWND startButton = FindWindowExW(taskbar, nullptr, L"Start", nullptr);
    if (!startButton) {
        // On Windows 11, try alternate class name
        startButton = FindWindowExW(taskbar, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
    }
    
    if (startButton) {
        g_startButtonHwnd = startButton;
        GetWindowRect(startButton, &g_startButtonRect);
    } else {
        g_startButtonHwnd = nullptr;
    }
}

// Try to run unelevated via the shell's COM automation (Explorer), per Raymond Chen's guidance
bool ShellExecuteUnelevated(const wchar_t* file, const wchar_t* params, int nShow) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInit = SUCCEEDED(hr);
    // If already initialized (S_FALSE), we still need to call CoUninitialize at the end
    if (hr == RPC_E_CHANGED_MODE) {
        // Fallback: try MTA
        hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        coInit = SUCCEEDED(hr);
    }

    IShellWindows* psw = nullptr;
    hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
                           IID_PPV_ARGS(&psw));
    if (FAILED(hr) || !psw) {
        if (coInit) CoUninitialize();
        return false;
    }

    VARIANT vtLoc{}; vtLoc.vt = VT_I4; vtLoc.lVal = CSIDL_DESKTOP;
    VARIANT vtEmpty{}; vtEmpty.vt = VT_EMPTY;
    long lhwnd = 0;
    IDispatch* pdisp = nullptr;
    hr = psw->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &pdisp);
    if (FAILED(hr) || !pdisp) {
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    IServiceProvider* psp = nullptr;
    hr = pdisp->QueryInterface(IID_PPV_ARGS(&psp));
    if (FAILED(hr) || !psp) {
        pdisp->Release();
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    IShellBrowser* psb = nullptr;
    hr = psp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&psb));
    if (FAILED(hr) || !psb) {
        psp->Release();
        pdisp->Release();
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    IShellView* psv = nullptr;
    hr = psb->QueryActiveShellView(&psv);
    if (FAILED(hr) || !psv) {
        psb->Release();
        psp->Release();
        pdisp->Release();
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    IDispatch* pdispView = nullptr;
    hr = psv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pdispView));
    if (FAILED(hr) || !pdispView) {
        psv->Release();
        psb->Release();
        psp->Release();
        pdisp->Release();
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    IShellDispatch2* psd = nullptr;
    hr = pdispView->QueryInterface(IID_PPV_ARGS(&psd));
    if (FAILED(hr) || !psd) {
        pdispView->Release();
        psv->Release();
        psb->Release();
        psp->Release();
        pdisp->Release();
        psw->Release();
        if (coInit) CoUninitialize();
        return false;
    }

    VARIANT vFile{}; vFile.vt = VT_BSTR; vFile.bstrVal = SysAllocString(file);
    VARIANT vArgs{}; if (params) { vArgs.vt = VT_BSTR; vArgs.bstrVal = SysAllocString(params); } else { vArgs.vt = VT_EMPTY; }
    VARIANT vDir{}; vDir.vt = VT_EMPTY;
    VARIANT vOp{}; vOp.vt = VT_BSTR; vOp.bstrVal = SysAllocString(L"open");
    VARIANT vShow{}; vShow.vt = VT_I4; vShow.lVal = nShow;

    hr = psd->ShellExecute(vFile.bstrVal, vArgs, vDir, vOp, vShow);

    if (vFile.vt == VT_BSTR) SysFreeString(vFile.bstrVal);
    if (vArgs.vt == VT_BSTR) SysFreeString(vArgs.bstrVal);
    if (vOp.vt == VT_BSTR) SysFreeString(vOp.bstrVal);

    psd->Release();
    pdispView->Release();
    psv->Release();
    psb->Release();
    psp->Release();
    pdisp->Release();
    psw->Release();

    if (coInit) CoUninitialize();
    return SUCCEEDED(hr);
}

// Fallback: duplicate Explorer's token and start the process unelevated
bool CreateProcessWithExplorerToken(const wchar_t* app, wchar_t* cmdMutable, int /*nShow*/) {
    HWND hShell = GetShellWindow();
    if (!hShell) {
        hShell = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!hShell) return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hShell, &pid);
    if (!pid) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;
    HANDLE hTok = nullptr;
    if (!OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID, &hTok)) {
        CloseHandle(hProc);
        return false;
    }
    HANDLE hPrimary = nullptr;
    SECURITY_ATTRIBUTES sa{}; sa.nLength = sizeof(sa);
    if (!DuplicateTokenEx(hTok, TOKEN_ALL_ACCESS, &sa, SecurityImpersonation, TokenPrimary, &hPrimary)) {
        CloseHandle(hTok);
        CloseHandle(hProc);
        return false;
    }

    STARTUPINFOW si{}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessWithTokenW(hPrimary, LOGON_WITH_PROFILE, app, cmdMutable, 0, nullptr, nullptr, &si, &pi);
    if (ok) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    CloseHandle(hPrimary);
    CloseHandle(hTok);
    CloseHandle(hProc);
    return ok == TRUE;
}

// Execute action for Start button click
void ExecuteStartButtonAction() {
    int clickType = g_startButtonClickType.load();
    std::string action;
    std::string command;
    const char* clickName = "";
    
    switch (clickType) {
        case 1: // Left click
            action = g_startButtonLeftClickAction;
            command = g_startButtonLeftClickCommand;
            clickName = "left click";
            break;
        case 2: // Right click
            action = g_startButtonRightClickAction;
            command = g_startButtonRightClickCommand;
            clickName = "right click";
            break;
        case 3: // Middle click
            action = g_startButtonMiddleClickAction;
            command = g_startButtonMiddleClickCommand;
            clickName = "middle click";
            break;
        case 4: // Shift+Left click
            action = g_startButtonShiftLeftClickAction;
            command = g_startButtonShiftLeftClickCommand;
            clickName = "Shift+left click";
            break;
        case 5: // Ctrl+Left click
            action = g_startButtonCtrlLeftClickAction;
            command = g_startButtonCtrlLeftClickCommand;
            clickName = "Ctrl+left click";
            break;
        default:
            return;
    }
    
    if (action == "custom") {
        Wh_Log(L"Windows Key Actions: Start button %S - executing custom command: %S", clickName, command.c_str());
        
        // For Ctrl+Click, be more aggressive about closing Start menu
        if (clickType == 5) {
            CloseStartMenuIfOpen();
            Sleep(50); // Give it time to process
            CloseStartMenuIfOpen(); // Try again
        } else {
            CloseStartMenuIfOpen();
        }
        
        ExecuteCommand(command);
    }
    // If "disabled", do nothing (click is suppressed)
    // If "default", the click passes through normally (this function won't be called)
}

// Low-level mouse hook procedure
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* mouse = (MSLLHOOKSTRUCT*)lParam;
        
        if (!g_startButtonHwnd) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        
        // Check if click is within Start button rectangle
        POINT pt = mouse->pt;
        if (!PtInRect(&g_startButtonRect, pt)) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        
        // Handle UP events - suppress if we suppressed the corresponding DOWN
        if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP) {
            if (g_suppressNextMouseUp) {
                g_suppressNextMouseUp = false;
                Wh_Log(L"Windows Key Actions: Suppressing mouse UP event");
                return 1;
            }
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        
        // Determine click type and action for DOWN events
        std::string action;
        int clickType = 0;
        
        if (wParam == WM_LBUTTONDOWN) {
            // Check for modifier keys using GetKeyState for more reliable detection
            bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            
            Wh_Log(L"Windows Key Actions: Left click at (%ld, %ld), Shift=%d, Ctrl=%d", 
                   pt.x, pt.y, shiftPressed, ctrlPressed);
            
            if (shiftPressed) {
                action = g_startButtonShiftLeftClickAction;
                clickType = 4;
            } else if (ctrlPressed) {
                action = g_startButtonCtrlLeftClickAction;
                clickType = 5;
                // For Ctrl+Click, we need to suppress the Ctrl key to prevent Start menu opening
                if (action != "default") {
                    g_suppressCtrlKey = true;
                }
            } else {
                action = g_startButtonLeftClickAction;
                clickType = 1;
            }
        } else if (wParam == WM_RBUTTONDOWN) {
            action = g_startButtonRightClickAction;
            clickType = 2;
            Wh_Log(L"Windows Key Actions: Right click at (%ld, %ld)", pt.x, pt.y);
        } else if (wParam == WM_MBUTTONDOWN) {
            action = g_startButtonMiddleClickAction;
            clickType = 3;
            Wh_Log(L"Windows Key Actions: Middle click at (%ld, %ld)", pt.x, pt.y);
        } else {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        
        // If action is "default", let it pass through
        if (action == "default") {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        
        // For "custom" or "disabled", suppress the click and execute action if needed
        Wh_Log(L"Windows Key Actions: Start button type=%d, action=%S - suppressing", 
               clickType, action.c_str());
        
        if (action == "custom") {
            // Signal worker thread to execute custom action
            g_startButtonClickType = clickType;
            SetEvent(g_executeStartButtonEvent);
        }
        
        // Mark that we need to suppress the corresponding UP event
        g_suppressNextMouseUp = true;
        
        // Suppress the DOWN event for both "custom" and "disabled"
        return 1;
    }
    
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}
