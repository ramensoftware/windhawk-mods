// ==WindhawkMod==
// @id           modern-notepad-to-classic-editor
// @name         Redirect Modern Notepad to Classic Editor
// @description  Redirects Windows 11's Notepad to a configurable classic editor (defaulting to classic notepad), useful to keep the default "Edit in Notepad" context menu item. Allows auto-elevation using Ctrl+Shift.
// @version      1.9
// @author       David Trapp (CherryDT)
// @github       https://github.com/CherryDT
// @include      %programfiles%\WindowsApps\Microsoft.WindowsNotepad_*\Notepad\Notepad.exe
// @architecture x86-64
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

## ⚠️ IMPORTANT: You need to also disable the app execution alias for Notepad in Windows Settings > Apps > Advanced app settings > App execution aliases, otherwise this mod won't work!

---

## Features

- **Seamless redirection**: Any time Windows tries to open the modern Notepad, your chosen editor is launched instead.
- **Configurable editor path**: Pick any executable, with environment variables supported (e.g. `%SystemRoot%\System32\notepad.exe`).
- **Optional Ctrl+Shift elevation**: If enabled, holding **Ctrl+Shift** while launching opens the editor **as Administrator** (if not already elevated). This allows for easily editing files that require elevation: Just hold Ctrl+Shift while using the “Edit in Notepad” context menu option!
- **Optional waiting behavior**:  
  - **Wait enabled:** The modern Notepad process stays open until the editor exits, and passes along its exit code.  
  - **Wait disabled (default):** The modern Notepad process exits immediately after starting the editor.

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
  Default: Enabled.

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

    g_ctrlShiftElevate = Wh_GetIntSetting(L"ctrlShiftElevate", 1) != 0;
    g_waitForClassic   = Wh_GetIntSetting(L"waitForClassic", 1) != 0;
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
          if (!isSessionLaunch) {
            // Ask the user to disable app alias for notepad
            MessageBoxW(NULL, L"The app execution alias for Notepad is enabled, which interferes with this mod. Please disable it in Windows Settings > Apps > Advanced app settings > App execution aliases.", L"Modern Notepad to Classic Editor", MB_ICONWARNING);
          }
          return FALSE; // Don't patch
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
