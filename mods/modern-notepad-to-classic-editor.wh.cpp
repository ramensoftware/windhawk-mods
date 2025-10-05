// ==WindhawkMod==
// @id              modern-notepad-to-classic-editor
// @name            Redirect Modern Notepad to Classic Editor
// @description     Redirects Windows 11's Notepad to a configurable classic editor (defaulting to classic notepad), useful to keep the default "Edit in Notepad" context menu item. Allows auto-elevation using Ctrl+Shift.
// @version         2.0
// @author          David Trapp (CherryDT)
// @github          https://github.com/CherryDT
// @include         %programfiles%\WindowsApps\Microsoft.WindowsNotepad_*\Notepad\Notepad.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- classicPath: "%SystemRoot%\\System32\\notepad.exe"
  $name: Classic editor path
  $description: The executable path of the classic editor to run instead of the modern Notepad. Environment variables like %SystemRoot% are supported.

- ctrlShiftElevate: true
  $name: Ctrl+Shift elevation
  $description: If enabled, holding Ctrl+Shift while launching will run the classic editor elevated (if not already elevated).

- waitForClassic: false
  $name: Wait for classic editor
  $description: If enabled, modern Notepad will wait until the classic editor exits and mirror its exit code. Otherwise, it will exit immediately. This might be helpful for some cases in which the caller expects the process to stay alive until the editor exits.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Redirect Modern Notepad to Classic Editor

Windows 11 replaces the classic Notepad with a Microsoft Store app.  
This mod redirects all launches of the **modern Notepad** (including context menu items like “Edit in Notepad”) to your chosen **classic editor** instead.

There is already a way to do this "half-way" natively by disabling the `notepad.exe` app execution alias in Windows' settings, but this doesnt work for the default "Edit in Notepad" context menu item, default file associations and other entry points. This mod handles all of these cases seamlessly.

By default, it launches the old `notepad.exe` from `%SystemRoot%\System32`, but you can configure it to run **any editor of your choice** (e.g. Notepad2, Notepad++, VS Code).

The basic way to use this mod is to **disable the app execution alias for Notepad** in Windows Settings > Apps > Advanced app settings > App execution aliases and keep the classic editor path setting at its default value. This way, all entry points that would normally launch modern Notepad will launch classic Notepad instead, and direct invocations of `notepad.exe` will go to the classic Notepad as well and even bypass this mod, speeding up launches. If you configure a different editor path, it will only take effect when using the "Edit with Notepad" shell extension or a file association that launches modern Notepad, not when directly launching `notepad.exe`.

For advanced usage scenarios such as globally overriding `notepad.exe` with a custom editor, see below.

---

## Features

- **Seamless redirection**: Any time Windows tries to open the modern Notepad, your chosen editor is launched instead.
- **Configurable editor path**: Pick any executable, with environment variables supported (e.g. `%SystemRoot%\System32\notepad.exe`).
- **Optional Ctrl+Shift elevation**: If enabled, holding **Ctrl+Shift** while launching opens the editor **as Administrator** (if not already elevated). This allows for easily editing files that require elevation: Just hold Ctrl+Shift while using the “Edit in Notepad” context menu option!
- **Optional waiting behavior**:  
  - **Wait enabled:** The modern Notepad process stays open until the editor exits, and passes along its exit code.  
  - **Wait disabled (default):** The modern Notepad process exits immediately after starting the editor.
- **Optional global override of `notepad.exe`**: By leaving the app execution alias enabled and configuring a different editor path, you can make all invocations of `notepad.exe` (including direct launches) go to your chosen editor instead of classic Notepad. This is useful if you want to completely replace Notepad with a different editor of your choice, but please see caveats below.

## Settings

- **Classic editor path**  
  Path to the editor executable you want to run instead of modern Notepad.  
  Defaults to `%SystemRoot%\System32\notepad.exe`.

- **Ctrl+Shift elevation**  
  If enabled, holding Ctrl+Shift while opening Notepad will launch the editor elevated.  
  Default: Enabled.

- **Wait for classic editor**  
  If enabled, the modern Notepad process will wait until the editor closes and mirror its exit code.  
  If disabled, it will exit right after starting the editor.  
  Default: Disabled.

## Advanced Usage

If support for the Ctrl+Shift elevation feature even during direct `notepad.exe` invocations or total replacement of Notepad by some other editor is desired (through configuring a different classic editor path), the app execution alias must be left **enabled**. During the first launch of notepad with this mod enabled, you will then be prompted to **confirm disabling the app execution alias locally** within the immersive notepad package.

This will create what's called a "tombstone" in a "layered key" of the registry (such as `\REGISTRY\WC\Silo24fba818-4b95-b33d-ce9e-e481fda525fauser_sid\Software\Microsoft\Windows\CurrentVersion\App Paths\Notepad.exe`).

There are no known adverse effects of this known, because immersive notepad will not try to launch the classic notepad itself anyway, only the mod does that, however this is a **semi-permanent modification** which would require the use of "Settings > Apps > Installed Apps > Notepad (...) > Advanced > Reset" to undo, which also clears any custom immersive notepad settings.

## Notes

- Works with all entry points that would normally launch modern Notepad, including the **right-click “Edit in Notepad”** menu option.  
- If the mod is disabled while redirected Notepad is running, the modern Notepad process simply exits to avoid instability.  
- Designed for **Windows 11** where modern Notepad is installed from the Microsoft Store.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>
#include <winreg.h>
#include <shellapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <memory>

// ---------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------
static std::wstring g_classicPathSetting; // resolved at init (env-expanded)
static bool g_ctrlShiftElevate;
static bool g_waitForClassic;

// ---------------------------------------------------------------------
// Globals / state
// ---------------------------------------------------------------------
static volatile LONG g_handoffStarted  = 0;
static volatile LONG g_handoffFinished = 0;

// Original function pointer for CreateProcessAsUserW
static decltype(&CreateProcessAsUserW) pCreateProcessAsUserW = nullptr;

// Original function pointer for CreateWindowExW
static decltype(&CreateWindowExW) pCreateWindowExW = nullptr;

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

// Expand environment variables (e.g., %SystemRoot%)
static std::wstring ExpandEnv(const std::wstring& s) {
  if (s.empty()) return L"";
  DWORD needed = ExpandEnvironmentStringsW(s.c_str(), NULL, 0);
  if (!needed) return L"";
  std::unique_ptr<wchar_t[]> buf(new wchar_t[needed]);
  DWORD got = ExpandEnvironmentStringsW(s.c_str(), buf.get(), needed);
  if (!got || got > needed) return L"";
  return std::wstring(buf.get(), got - 1);
}

// Get current executable full path
static std::wstring GetCurrentExePath() {
  wchar_t buf[MAX_PATH];
  DWORD len = GetModuleFileNameW(NULL, buf, ARRAYSIZE(buf));
  if (!len) return L"";
  return std::wstring(buf, len);
}

// Parse a command line into argv (wide)
static std::vector<std::wstring> ParseCommandLineW(LPCWSTR cmdLine) {
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(cmdLine ? cmdLine : L"", &argc);
  std::vector<std::wstring> out;
  if (!argv) return out;
  out.reserve(argc);
  for (int i = 0; i < argc; ++i) out.emplace_back(argv[i]);
  LocalFree(argv);
  return out;
}

static std::wstring TrimLeft(const std::wstring& s) {
  size_t i = 0;
  while (i < s.size() && iswspace(s[i])) ++i;
  return s.substr(i);
}

static bool CiStartsWith(const std::wstring& s, const std::wstring& prefix) {
  if (s.size() < prefix.size()) return false;
  for (size_t i = 0; i < prefix.size(); ++i)
    if (towlower(s[i]) != towlower(prefix[i])) return false;
  return true;
}

// Return true if any arg after argv[0] starts with "/SESSION" (case-insensitive)
static bool ArgsContainSession(const std::vector<std::wstring>& argv) {
  for (size_t i = 1; i < argv.size(); ++i) {
    std::wstring a = TrimLeft(argv[i]);
    if (CiStartsWith(a, L"/SESSION")) return true;
  }
  return false;
}

static std::wstring QuoteArgIfNeeded(const std::wstring& s) {
  if (s.empty()) return L"\"\"";
  bool need = false;
  for (wchar_t c : s) if (c == L' ' || c == L'\t' || c == L'"') { need = true; break; }
  if (!need) return s;
  std::wstring out; out.push_back(L'"');
  for (wchar_t c : s) {
    if (c == L'"') out.append(L"\\\"");
    else out.push_back(c);
  }
  out.push_back(L'"');
  return out;
}

// Build only the argument tail (argv[1..]) for ShellExecute parameters
static std::wstring BuildArgsOnly() {
  std::wstring cmd = GetCommandLineW();
  auto argv = ParseCommandLineW(cmd.c_str());
  std::wstring args;
  for (size_t i = 1; i < argv.size(); ++i) {
    if (i > 1) args += L" ";
    args += QuoteArgIfNeeded(argv[i]);
  }
  return args;
}

// Check if current process is elevated
static bool IsProcessElevated() {
  HANDLE hToken = NULL;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return false;
  TOKEN_ELEVATION elev = {};
  DWORD retLen = 0;
  BOOL ok = GetTokenInformation(hToken, TokenElevation, &elev, sizeof(elev), &retLen);
  CloseHandle(hToken);
  return ok && elev.TokenIsElevated;
}

// Check Ctrl+Shift pressed
static bool IsCtrlShiftPressed() {
  return (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
         (GetAsyncKeyState(VK_SHIFT) & 0x8000);
}


// Check if current process already has a Notepad window
static bool HasNotepadWindowInCurrentProcess() {
  DWORD currentPid = GetCurrentProcessId();
  struct EnumData {
    DWORD pid;
    bool found;
  } data = { currentPid, false };

  EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
    EnumData* pData = (EnumData*)lParam;
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == pData->pid) {
      WCHAR className[256];
      if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) && wcscmp(className, L"Notepad") == 0) {
        pData->found = true;
        return FALSE; // stop enumeration
      }
    }
    return TRUE;
  }, (LPARAM)&data);

  return data.found;
}

// ---------------------------------------------------------------------
// Hook: CreateProcessAsUserW  (black-hole `/SESSION` relaunch attempts)
// ---------------------------------------------------------------------
static BOOL WINAPI Hook_CreateProcessAsUserW(
  HANDLE hToken,
  LPCWSTR lpApplicationName,
  LPWSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCWSTR lpCurrentDirectory,
  LPSTARTUPINFOW lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation)
{
  auto argv = ParseCommandLineW(lpCommandLine);
  if (ArgsContainSession(argv)) {
    Sleep(INFINITE); // black-hole any relaunch carrying `/SESSION`
    return FALSE;    // not reached
  }

  return pCreateProcessAsUserW(
    hToken,
    lpApplicationName,
    lpCommandLine,
    lpProcessAttributes,
    lpThreadAttributes,
    bInheritHandles,
    dwCreationFlags,
    lpEnvironment,
    lpCurrentDirectory,
    lpStartupInfo,
    lpProcessInformation
  );
}

// ---------------------------------------------------------------------
// Hook: CreateWindowExW (stall window creation forever)
// ---------------------------------------------------------------------
static HWND WINAPI Hook_CreateWindowExW(
  DWORD dwExStyle,
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
  LPVOID lpParam)
{
  if (lpClassName && (ULONG_PTR)lpClassName >= 0x10000 && wcscmp(lpClassName, L"Notepad") == 0) {
    Sleep(INFINITE); // stall forever
    return NULL;     // not reached
  }

  return pCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

// ---------------------------------------------------------------------
// Handoff worker: launch classic editor and (optionally) wait
// ---------------------------------------------------------------------
static DWORD WINAPI HandoffThreadProc(LPVOID) {
  InterlockedExchange(&g_handoffStarted, 1);

  // Resolve classic path from settings (expand env), fallback if empty
  std::wstring classicPath = ExpandEnv(g_classicPathSetting);
  if (classicPath.empty()) {
    classicPath = ExpandEnv(L"%SystemRoot%\\System32\\notepad.exe");
  }

  // Avoid recursion if somehow we're already that exe
  {
    std::wstring cur = GetCurrentExePath();
    std::wstring a = cur, b = classicPath;
    for (auto &c : a) c = towlower(c);
    for (auto &c : b) c = towlower(c);
    if (a == b) {
      InterlockedExchange(&g_handoffFinished, 1);
      return 0;
    }
  }

  // Avoid app-launching cursor (see https://stackoverflow.com/a/3865519/1871033)
  MSG msg;
  PostMessage(NULL, WM_NULL, 0, 0);
  GetMessage(&msg, NULL, 0, 0);

  // Elevate with Ctrl+Shift if enabled and not elevated
  if (g_ctrlShiftElevate && IsCtrlShiftPressed() && !IsProcessElevated()) {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = classicPath.c_str();
    std::wstring argTail = BuildArgsOnly();
    sei.lpParameters = argTail.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
      if (g_waitForClassic && sei.hProcess) {
        WaitForSingleObject(sei.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(sei.hProcess, &exitCode);
        CloseHandle(sei.hProcess);
        InterlockedExchange(&g_handoffFinished, 1);
        ExitProcess(exitCode);
      } else {
        if (sei.hProcess) CloseHandle(sei.hProcess);
        InterlockedExchange(&g_handoffFinished, 1);
        ExitProcess(0);
      }
    }
    // If elevation failed, fall through to non-elevated launch
  }

  // Non-elevated launch: build full command line with classic exe + original args
  std::wstring newCmd = L"\"" + classicPath + L"\"";
  {
    std::wstring argTail = BuildArgsOnly();
    if (!argTail.empty()) {
      newCmd += L" ";
      newCmd += argTail;
    }
  }

  std::unique_ptr<wchar_t[]> buf(new wchar_t[newCmd.size() + 1]);
  wcscpy_s(buf.get(), newCmd.size() + 1, newCmd.c_str());

  STARTUPINFOW si = { 0 }; si.cb = sizeof(si);
  PROCESS_INFORMATION pi = { 0 };

  if (!CreateProcessW(NULL, buf.get(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    MessageBoxW(NULL, (std::wstring(L"Failed to launch classic editor, please check path setting!\r\n\r\n") + buf.get()).c_str(), L"Modern Notepad to Classic Editor", MB_ICONERROR);
    InterlockedExchange(&g_handoffFinished, 1);
    ExitProcess(1);
  }

  if (g_waitForClassic) {
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    InterlockedExchange(&g_handoffFinished, 1);
    ExitProcess(exitCode);
  } else {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    InterlockedExchange(&g_handoffFinished, 1);
    ExitProcess(0);
  }

  return 0;
}

// ---------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------
BOOL Wh_ModInit() {
  // Load settings
  {
    g_classicPathSetting = WindhawkUtils::StringSetting::make(L"classicPath");
    if (g_classicPathSetting.empty()) g_classicPathSetting = L"%SystemRoot%\\System32\\notepad.exe";

    g_ctrlShiftElevate = Wh_GetIntSetting(L"ctrlShiftElevate") != 0;
    g_waitForClassic   = Wh_GetIntSetting(L"waitForClassic") != 0;
  }

  // Check if a Notepad window already exists in this process
  if (HasNotepadWindowInCurrentProcess()) {
    return FALSE; // Exit without patching
  }

  // Detect whether our own first real arg starts with /SESSION, exit quietly with 0
  auto argv = ParseCommandLineW(GetCommandLineW());
  bool isSessionLaunch = ArgsContainSession(argv);

  // Check for app execution alias
  {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\notepad.exe", 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
      WCHAR value[1024] = {};
      DWORD size = sizeof(value);
      DWORD type;
      LONG queryRet = RegQueryValueExW(hKey, NULL, NULL, &type, (BYTE*)value, &size);
      RegCloseKey(hKey);
      if (queryRet == ERROR_SUCCESS && type == REG_SZ) {
        std::wstring val(value);
        if (val.find(L"WindowsApps") != std::wstring::npos) {
          if (isSessionLaunch) return FALSE;

          // Show task dialog
          TASKDIALOGCONFIG tdc = { sizeof(TASKDIALOGCONFIG) };
          tdc.hwndParent = NULL;
          tdc.hInstance = NULL;
          tdc.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
          tdc.dwCommonButtons = TDCBF_CANCEL_BUTTON;
          tdc.pszWindowTitle = L"Modern Notepad to Classic Editor";
          tdc.pszMainIcon = TD_WARNING_ICON;
          tdc.pszMainInstruction = L"App execution alias for Notepad is enabled";
          tdc.pszContent = L"The app execution alias for Notepad is enabled, which interferes with this mod. To make this mod work, you need to either disable the app execution alias globally or install a local bypass.\n\nDisabling the alias globally should be fine in most cases, but it doesn't allow overriding notepad with a custom editor when invoked directly. Installing a local bypass can be useful in advanced scenarios and creates a registry entry that overrides the alias for this app only, but this modification persists even after disabling the mod, even though it should not have any effect on normal operations. To undo it, reset the Notepad app in Settings > Apps > Installed apps > Notepad (...) > Advanced > Reset.\n\nChoose an option below:";
          TASKDIALOG_BUTTON buttons[] = {
            { 1, L"Open Advanced app settings to disable the alias globally" },
            { 2, L"Install local app execution alias bypass" }
          };
          tdc.pButtons = buttons;
          tdc.cButtons = 2;
          int nButton;
          HRESULT hr = TaskDialogIndirect(&tdc, &nButton, NULL, NULL);
          if (SUCCEEDED(hr)) {
            if (nButton == 1) {
              // Open settings
              ShellExecuteW(NULL, L"open", L"ms-settings:advanced-apps", NULL, NULL, SW_SHOWNORMAL);
              return FALSE;
            } else if (nButton == 2) {
              // Install bypass
              // This will not modify the global registry because we run in the app container of immersive notepad.
              // Instead, it will create a so-called "tombstone" in a "layered key" such as `\REGISTRY\WC\Silo24fba818-4b95-b33d-ce9e-e481fda525fauser_sid\Software\Microsoft\Windows\CurrentVersion\App Paths\Notepad.exe` that overrides the alias for this app only.
              RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\notepad.exe");

              MessageBoxW(NULL, L"The local app execution alias bypass has been installed successfully.\n\nIf you want to undo this modification later, go to Settings > Apps > Installed apps > Notepad (...) > Advanced > Reset.", L"Modern Notepad to Classic Editor", MB_ICONINFORMATION);

              // Continue with init
            } else {
              // Cancel
              return FALSE;
            }
          } else {
            // Task dialog failed
            return FALSE;
          }
        }
      }
    }
  }

  // If this is already an "inner" launch with /SESSION, exit quietly
  if (isSessionLaunch) {
    ExitProcess(0);
  }

  // Install hook for the session launch using Windhawk utils, stalling the original process forever
  WindhawkUtils::SetFunctionHook(CreateProcessAsUserW, Hook_CreateProcessAsUserW, &pCreateProcessAsUserW);

  // Install hook for CreateWindowExW to stall Notepad window creation forever as well
  WindhawkUtils::SetFunctionHook(CreateWindowExW, Hook_CreateWindowExW, &pCreateWindowExW);

  // Start the handoff asynchronously and return quickly (avoid Windhawk progress UI)
  HANDLE hThread = CreateThread(NULL, 0, HandoffThreadProc, NULL, 0, NULL);
  if (hThread) CloseHandle(hThread);
  return TRUE;
}

void Wh_ModBeforeUninit() {
  // If handoff isn't finished yet, exit now to avoid unloading while threads run
  if (InterlockedCompareExchange(&g_handoffFinished, 0, 0) == 1) return;
  ExitProcess(0);
}

void Wh_ModUninit() {
  // no-op
}
