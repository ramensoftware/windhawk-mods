// ==WindhawkMod==
// @id              start-button-raycast
// @name            Start button opens Raycast
// @description     Start button and Win key open or toggle Raycast instead of the Start menu
// @version         1.0
// @author          Hirnaymay Bhaskar
// @github          https://github.com/octopols
// @twitter         https://twitter.com/octopols
// @homepage        https://hirnaymay.com/
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start button opens Raycast

Makes the taskbar Start button and the Win key open
[Raycast](https://www.raycast.com/windows) instead of the Windows Start menu.

## Requirements

- Windows 11 x64
- Raycast for Windows (Microsoft Store version)

If Raycast is not installed, the Start menu opens normally, so the mod is
safe to keep enabled either way.

## How it works

The mod intercepts the request to open the Start menu inside Explorer. When
the trigger is the Start button or the Win key, it instead sends Raycast's
own inter-instance "bring to front" message over its named pipe, which is
instant. If Raycast is installed but not running, it gets launched.

## Settings

- **Start button opens Raycast** / **Win key opens Raycast**: disable either
  one and that trigger opens the regular Start menu instead.
- **Toggle**: if Raycast is focused, the trigger closes it (sends Esc) rather
  than reopening it. With text typed in the bar, Raycast treats the first Esc
  as "clear", so a second press may be needed.

All other ways of opening the Start menu (for example on session unlock) are
unaffected. Disable the mod to restore the default behavior completely.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- startButton: true
  $name: Start button opens Raycast
- winKey: true
  $name: Win key opens Raycast
- toggle: true
  $name: Toggle
  $description: Close Raycast if it is already focused
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <appmodel.h>

#include <string>
#include <vector>

// IMMERSIVELAUNCHERSHOWMETHOD values.
enum {
    ILSM_HSHELLTASKMAN = 1,  // Win key / Ctrl+Esc
    ILSM_STARTBUTTON = 11,   // Taskbar Start button click
};

struct {
    bool startButton;
    bool winKey;
    bool toggle;
} g_settings;

// Raycast's single-instance message: one JSON line per pipe connection,
// {"type":"<assembly-qualified type>","payload":"<inner JSON>"}.
// BringRaycastToFront is the message a second instance sends on startup.
constexpr char kPipeName[] = "\\\\.\\pipe\\InterRaycastProductionNamedPipe";
constexpr char kBringToFrontMsg[] =
    "{\"type\":\"Raycast.InterRaycastPipe.BringRaycastToFront, Raycast\","
    "\"payload\":\"{}\"}\n";

// Store package family of Raycast for Windows.
constexpr WCHAR kRaycastPackageFamily[] = L"Raycast.Raycast_qypenmj9wpt2a";

bool IsRaycastRunning() {
    if (WaitNamedPipeA(kPipeName, 1)) {
        return true;
    }
    // ERROR_SEM_TIMEOUT means the pipe exists but is momentarily busy, i.e.
    // Raycast is running.
    return GetLastError() != ERROR_FILE_NOT_FOUND;
}

bool SendBringToFront() {
    for (int attempt = 0; attempt < 3; attempt++) {
        HANDLE pipe = CreateFileA(kPipeName, GENERIC_WRITE, 0, nullptr,
                                  OPEN_EXISTING, 0, nullptr);
        if (pipe != INVALID_HANDLE_VALUE) {
            DWORD written;
            BOOL ok = WriteFile(pipe, kBringToFrontMsg,
                                sizeof(kBringToFrontMsg) - 1, &written,
                                nullptr);
            FlushFileBuffers(pipe);
            CloseHandle(pipe);
            return ok;
        }

        DWORD error = GetLastError();
        if (error == ERROR_PIPE_BUSY) {
            // The server handles one connection at a time; wait for the next
            // instance to become available.
            WaitNamedPipeA(kPipeName, 500);
            continue;
        }

        // ERROR_FILE_NOT_FOUND etc. - Raycast isn't running.
        Wh_Log(L"Pipe unavailable (err=%u)", error);
        return false;
    }
    return false;
}

bool IsRaycastForeground() {
    HWND fg = GetForegroundWindow();
    if (!fg) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (!pid) {
        return false;
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD len = ARRAYSIZE(path);
    bool result = false;
    if (QueryFullProcessImageNameW(process, 0, path, &len)) {
        PCWSTR name = wcsrchr(path, L'\\');
        name = name ? name + 1 : path;
        result = _wcsicmp(name, L"Raycast.exe") == 0;
    }
    CloseHandle(process);
    return result;
}

void SendEscToRaycast() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_ESCAPE;
    inputs[0].ki.wScan = (WORD)MapVirtualKeyW(VK_ESCAPE, MAPVK_VK_TO_VSC);
    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

// Resolves the installed Raycast Store package folder (it changes on every
// update, so it can't be hardcoded). Empty if Raycast isn't installed.
std::wstring GetRaycastExePath() {
    UINT32 count = 0, bufLen = 0;
    LONG rc =
        FindPackagesByPackageFamily(kRaycastPackageFamily, PACKAGE_FILTER_HEAD,
                                    &count, nullptr, &bufLen, nullptr, nullptr);
    if (rc != ERROR_INSUFFICIENT_BUFFER || count == 0) {
        return L"";
    }

    std::vector<PWSTR> fullNames(count);
    std::vector<WCHAR> nameBuffer(bufLen);
    rc = FindPackagesByPackageFamily(kRaycastPackageFamily,
                                     PACKAGE_FILTER_HEAD, &count,
                                     fullNames.data(), &bufLen,
                                     nameBuffer.data(), nullptr);
    if (rc != ERROR_SUCCESS || count == 0) {
        return L"";
    }

    UINT32 pathLen = 0;
    GetPackagePathByFullName(fullNames[0], &pathLen, nullptr);
    if (pathLen == 0) {
        return L"";
    }

    std::vector<WCHAR> path(pathLen);
    if (GetPackagePathByFullName(fullNames[0], &pathLen, path.data()) !=
        ERROR_SUCCESS) {
        return L"";
    }

    return std::wstring(path.data()) + L"\\Raycast\\Raycast.exe";
}

DWORD WINAPI OpenRaycastThread(LPVOID) {
    if (g_settings.toggle && IsRaycastForeground()) {
        SendEscToRaycast();
        return 0;
    }

    if (SendBringToFront()) {
        return 0;
    }

    // Raycast isn't running - start it.
    std::wstring exe = GetRaycastExePath();
    if (exe.empty()) {
        Wh_Log(L"Raycast package not found");
        return 0;
    }

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(exe.c_str(), nullptr, nullptr, nullptr, FALSE, 0,
                       nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        Wh_Log(L"CreateProcessW failed (err=%u)", GetLastError());
    }
    return 0;
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis,
                                                      int showMethod,
                                                      int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int showMethod,
                                               int showFlags) {
    bool redirect =
        (showMethod == ILSM_STARTBUTTON && g_settings.startButton) ||
        (showMethod == ILSM_HSHELLTASKMAN && g_settings.winKey);

    // If Raycast is neither running nor installed, let the Start menu open
    // normally so the trigger is never a dead end.
    if (redirect && !IsRaycastRunning() && GetRaycastExePath().empty()) {
        redirect = false;
    }

    if (!redirect) {
        return XamlLauncher_ShowStartView_Original(pThis, showMethod,
                                                   showFlags);
    }

    // Act from a worker thread so the shell thread is never blocked.
    HANDLE thread = CreateThread(nullptr, 0, OpenRaycastThread, nullptr, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    }
    return S_OK;
}

void LoadSettings() {
    g_settings.startButton = Wh_GetIntSetting(L"startButton");
    g_settings.winKey = Wh_GetIntSetting(L"winKey");
    g_settings.toggle = Wh_GetIntSetting(L"toggle");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // CImmersiveLauncher on older builds, XamlLauncher on current ones.
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))",
             LR"(public: virtual long __cdecl CImmersiveLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                     ARRAYSIZE(twinuiPcshellSymbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
