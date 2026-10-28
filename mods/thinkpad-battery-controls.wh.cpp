// ==WindhawkMod==
// @id thinkpad-battery-controls
// @name ThinkPad Battery Controls
// @description Charge to full temporarily and restore Lenovo battery protection.
// @version 0.6.6
// @author Bill Bauman
// @github https://github.com/billbauman
// @license Apache-2.0
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lshell32 -luser32 -lole32 -lruntimeobject -lwindowsapp -lbcrypt -ladvapi32 -lcrypt32 -luuid -lkernel32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# ThinkPad Battery Controls
Author and publisher: Bill Bauman. Copyright 2026 Bill Bauman.
Apache-2.0. The full project name is claimed as a trademark of Bill Bauman,
subject to applicable law. Fair use and truthful attribution are welcome.
Modified forks and commercial sellers must use distinct product branding;
this does not restrict commercial use of the Apache-licensed code.
ThinkPad is Lenovo's trademark. No Lenovo or Microsoft endorsement is implied.

Requires Lenovo Power Manager and Lenovo's signed ChargeThreshold.exe.
Use Setup and diagnostics to download it into Program Files. Leave the path blank in Settings for automatic discovery. Right-click the Windows battery icon for controls.
Temporary overrides restore the saved limits at 100% or after eight hours.
If Vantage has already enabled a range, that newer choice is preserved. State
survives restarts. Disabling the mod attempts restoration.
Only a single battery is supported. Unknown responses fail closed.
Controls are integrated into the existing Windows battery icon.
Open Manage presets in the battery menu to edit up to 20 saved presets and choose up to five quick favorites.
Lenovo's utility is separately licensed.

## Setup help
Use Setup and diagnostics in the battery menu to check your model, utility,
Lenovo PM Service, and battery communication. Missing or inaccessible components
are explained with official links. Download and set up installs the utility after Windows administrator approval.

- [ChargeThreshold.exe](https://download.lenovo.com/pccbbs/thinkvantage_en/metroapps/Vantage/ChargeThreshold/ChargeThreshold.exe): use the setup button, or place it under Program Files/ThinkPad Battery Controls with administrator-only write access. A blank path setting discovers it automatically.
- [Lenovo Power and Battery Driver](https://support.lenovo.com/us/en/downloads/ds541411-lenovo-settings-power-manager-for-windows-10-version-1703-or-later-thinkpad): check supported models first.
- [Lenovo PC Support](https://pcsupport.lenovo.com/): select your model, then Drivers & Software.

## Compatibility and setup details
Tested on a ThinkPad P14s Gen 6 AMD with English utility output. Other models
and localized output have not been verified. Only a single battery is supported.
Taskbar integration uses private Windows symbols and can stop working after a
Windows update. The recovery worker remains available if symbols are missing.
If an outstanding worker or UI delegate prevents safe unloading, the mod retains
its DLL. Restart Explorer or sign out before enabling that same version again.
The administrator prompt for setup names Windows Command Processor, published
by Microsoft. It launches the verified setup script through system PowerShell;
it is not a Lenovo installer prompt. Drivers must be installed separately.

Driver links use your detected machine type and Lenovo's model redirect.
Unknown models have a product-selector fallback. No serial number is read or sent.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- utilityPath: ""
  $name: Utility path (leave blank for automatic location)
  $description: Use Setup and diagnostics in the battery menu to download and install the utility. Custom paths must be administrator-protected.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windhawk_api.h>
#include <string>
#include <string_view>
#include <charconv>
#include <ctime>
#include <bcrypt.h>
#include <wincrypt.h>
#include <aclapi.h>
#include <sddl.h>
#include <vector>
#include <atomic>
#include <mutex>
#include <deque>
#include <algorithm>
#include <functional>

// BEGIN TESTABLE ENGINE
namespace Engine {
std::mutex mutex;
bool alive = true;
void SetInt(PCWSTR key, int value) {
  std::lock_guard lock(mutex);
  if (alive)
    Wh_SetIntValue(key, value);
}
void SetString(PCWSTR key, PCWSTR value) {
  std::lock_guard lock(mutex);
  if (alive)
    Wh_SetStringValue(key, value);
}
int SettingInt(PCWSTR key) {
  std::lock_guard lock(mutex);
  return alive ? Wh_GetIntSetting(key) : 0;
}
std::wstring Setting(PCWSTR key) {
  std::lock_guard lock(mutex);
  if (!alive)
    return {};
  auto raw = Wh_GetStringSetting(key);
  std::wstring value = raw ? raw : L"";
  Wh_FreeStringSetting(raw);
  return value;
}
void Log(PCWSTR text) {
  std::lock_guard lock(mutex);
  if (alive)
    Wh_Log(L"%s", text);
}
void Close() {
  std::lock_guard lock(mutex);
  alive = false;
}
}  // namespace Engine
// END TESTABLE ENGINE
void AttachNativeMenu();
std::atomic<bool> unloading = false;
std::atomic<bool> retainModule = false;
std::atomic<bool> symbolsReady = false;
void RetainModule(PCWSTR reason) noexcept {
  OutputDebugStringW(L"ThinkPad Battery Controls: retaining module: ");
  OutputDebugStringW(reason);
  OutputDebugStringW(L"\n");
  HMODULE module = nullptr;
  GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                     reinterpret_cast<PCWSTR>(&RetainModule), &module);
  retainModule = true;
}
namespace BatteryControls {
// BEGIN TESTABLE HANDLE
struct Handle {
  HANDLE value = nullptr;
  Handle() = default;
  explicit Handle(HANDLE v) : value(v) {}
  ~Handle() {
    if (value && value != INVALID_HANDLE_VALUE)
      CloseHandle(value);
  }
  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;
  operator HANDLE() const { return value; }
  HANDLE* out() { return &value; }
  explicit operator bool() const { return value && value != INVALID_HANDLE_VALUE; }
};
// END TESTABLE HANDLE
HANDLE threadHandle = nullptr, stopEvent = nullptr, wakeEvent = nullptr;
std::atomic<HANDLE> setupThread = nullptr;
std::atomic<bool> setupRunning = false, setupCompleted = false;
std::mutex queueMutex;
std::deque<std::pair<int, int>> commands;
std::atomic<bool> active = false, busy = false, refreshing = false;
std::atomic<ULONGLONG> shutdownDeadline = 0;
struct State {
  bool valid = false, enabled = false;
  int start = 0, stop = 0;
};
// BEGIN TESTABLE CORE
constexpr bool ValidRange(int start, int stop) {
  return start >= 5 && start < stop && stop <= 100;
}
struct Recovery {
  DWORD magic = 0x54424332, version = 1, active = 0, start = 0, stop = 0;
  ULONGLONG deadline = 0;
};
bool ValidRecovery(const Recovery& r, ULONGLONG now) {
  return r.magic == 0x54424332 && r.version == 1 && r.active <= 1 &&
         (!r.active ||
          (ValidRange(r.start, r.stop) && r.deadline > 0 && r.deadline <= 0x7fffffffffffffffULL));
}
bool RecoveryDue(const Recovery& r, ULONGLONG now) {
  return r.active && (now >= r.deadline || r.deadline - now > 8 * 3600 + 300);
}
State Parse(std::string_view text) {
  State s;
  auto pos = text.find("Battery #");
  if (pos == text.npos || text.find("Battery #", pos + 1) != text.npos)
    return s;
  auto line = text.substr(pos);
  line = line.substr(0, line.find_first_of("\r\n"));
  if (line == "Battery #1: OFF.") {
    s.valid = true;
    return s;
  }
  constexpr std::string_view prefix = "Battery #1: Start at ", middle = "%, Stop at ";
  if (!line.starts_with(prefix) || !line.ends_with("%."))
    return s;
  line.remove_prefix(prefix.size());
  line.remove_suffix(2);
  auto split = line.find(middle);
  if (split == line.npos)
    return s;
  auto number = [](std::string_view value, int& out) {
    auto r = std::from_chars(value.data(), value.data() + value.size(), out);
    return r.ec == std::errc{} && r.ptr == value.data() + value.size();
  };
  if (!number(line.substr(0, split), s.start) ||
      !number(line.substr(split + middle.size()), s.stop) || !ValidRange(s.start, s.stop))
    return {};
  s.valid = s.enabled = true;
  return s;
}
// END TESTABLE CORE
Recovery recovery;
bool recoveryInvalid = false;
ULONGLONG Now() {
  return static_cast<ULONGLONG>(time(nullptr));
}
struct Snapshot {
  State state;
  int utility = 0;
  ULONGLONG revision = 0;
};
std::mutex stateMutex;
Snapshot snapshot;
std::wstring operationStatus, setupStatus;
ULONGLONG statusAt = 0;
Snapshot ReadSnapshot() {
  std::lock_guard lock(stateMutex);
  return snapshot;
}
int UiInt(PCWSTR key, int fallback = 0) {
  auto snap = ReadSnapshot();
  if (wcscmp(key, L"cacheValid") == 0)
    return snap.state.valid && !refreshing;
  if (wcscmp(key, L"cacheEnabled") == 0)
    return snap.state.enabled;
  if (wcscmp(key, L"cacheStart") == 0)
    return snap.state.valid && ValidRange(snap.state.start, snap.state.stop) ? snap.state.start
                                                                             : fallback;
  if (wcscmp(key, L"cacheStop") == 0)
    return snap.state.valid && ValidRange(snap.state.start, snap.state.stop) ? snap.state.stop
                                                                             : fallback;
  if (wcscmp(key, L"utilityCheck") == 0)
    return snap.utility;
  return fallback;
}
void SetUtility(int value) {
  {
    std::lock_guard lock(stateMutex);
    snapshot.utility = value;
  }
  Engine::SetInt(L"utilityCheck", value);
}
void Status(PCWSTR value) noexcept {
  try {
    {
      std::lock_guard lock(stateMutex);
      operationStatus = value;
      statusAt = GetTickCount64();
    }
    Engine::SetString(L"operationStatus", value);
  } catch (...) {
  }
}
void SetupStatus(PCWSTR value) noexcept {
  try {
    std::lock_guard lock(stateMutex);
    setupStatus = value;
  } catch (...) {
  }
}
std::wstring SetupText() {
  std::lock_guard lock(stateMutex);
  return setupStatus;
}
std::wstring RecentStatus() {
  std::lock_guard lock(stateMutex);
  return GetTickCount64() - statusAt < 30000 ? operationStatus : L"";
}
void Error() noexcept {
  Status(L"Operation not confirmed. Open Setup and diagnostics. Recovery information is retained.");
}
bool Active() {
  return active.load();
}
std::wstring DefaultUtility() {
  PWSTR raw = nullptr;
  if (FAILED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, nullptr, &raw)))
    return {};
  std::wstring path(raw);
  CoTaskMemFree(raw);
  return path + L"\\ThinkPad Battery Controls\\ChargeThreshold.exe";
}
std::wstring UtilityPath() {
  std::wstring path = Engine::Setting(L"utilityPath");
  return path.empty() ? DefaultUtility() : path;
}
// The executable and all ancestors must have trusted owners and no untrusted
// allow ACE granting modification. Reject unfamiliar ACE forms conservatively.
// BEGIN TESTABLE TRUST
bool TrustedSid(PSID sid) {
  BYTE system[SECURITY_MAX_SID_SIZE], admins[SECURITY_MAX_SID_SIZE];
  DWORD size = sizeof(system);
  CreateWellKnownSid(WinLocalSystemSid, nullptr, system, &size);
  size = sizeof(admins);
  CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, admins, &size);
  if (EqualSid(sid, system) || EqualSid(sid, admins))
    return true;
  PSID installer = nullptr;
  bool result = false;
  if (ConvertStringSidToSidW(L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464",
                             &installer)) {
    result = EqualSid(sid, installer);
    LocalFree(installer);
  }
  return result;
}
bool ProtectedObject(const std::wstring& path, bool volumeRoot = false) {
  PSECURITY_DESCRIPTOR sd = nullptr;
  PACL acl = nullptr;
  PSID owner = nullptr;
  if (GetNamedSecurityInfoW(path.c_str(), SE_FILE_OBJECT,
                            OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &owner, nullptr,
                            &acl, nullptr, &sd) != ERROR_SUCCESS)
    return false;
  bool ok = owner && TrustedSid(owner) && acl;
  DWORD writes = FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_EA | FILE_WRITE_ATTRIBUTES |
                 FILE_DELETE_CHILD | DELETE | WRITE_DAC | WRITE_OWNER | GENERIC_WRITE | GENERIC_ALL;
  // Creating unrelated folders on C:\ does not grant modification of an
  // existing protected Program Files tree. Delete-child/ACL writes still fail.
  if (volumeRoot)
    writes &= ~(FILE_WRITE_DATA | FILE_APPEND_DATA);
  for (DWORD i = 0; ok && i < acl->AceCount; i++) {
    ACE_HEADER* head = nullptr;
    if (!GetAce(acl, i, reinterpret_cast<void**>(&head))) {
      ok = false;
      break;
    }
    if (head->AceFlags & INHERIT_ONLY_ACE)
      continue;
    if (head->AceType == ACCESS_ALLOWED_ACE_TYPE) {
      auto ace = reinterpret_cast<ACCESS_ALLOWED_ACE*>(head);
      if ((ace->Mask & writes) && !TrustedSid(&ace->SidStart))
        ok = false;
    } else if (head->AceType != ACCESS_DENIED_ACE_TYPE)
      ok = false;
  }
  LocalFree(sd);
  return ok;
}
bool LockUtility(const std::wstring& path, Handle& file, std::vector<HANDLE>& directories) {
  if (path.size() < 4 || path[1] != L':' || path[2] != L'\\' || path.find(L'"') != path.npos)
    return false;
  wchar_t full[32768];
  DWORD n = GetFullPathNameW(path.c_str(), 32768, full, nullptr);
  if (!n || n >= 32768 || _wcsicmp(full, path.c_str()))
    return false;
  // Lock every parent against deletion/renaming for the whole child lifetime.
  for (size_t pos = 3;;) {
    auto end = path.find(L'\\', pos);
    if (end == path.npos)
      break;
    std::wstring parent = path.substr(0, end);
    HANDLE h = CreateFileW(parent.c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING,
                           FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (h == INVALID_HANDLE_VALUE)
      return false;
    directories.push_back(h);
    FILE_ATTRIBUTE_TAG_INFO info{};
    if (!GetFileInformationByHandleEx(h, FileAttributeTagInfo, &info, sizeof(info)) ||
        (info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) || !ProtectedObject(parent))
      return false;
    pos = end + 1;
  }
  if (!ProtectedObject(path.substr(0, 3), true))
    return false;
  file.value = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
  if (!file)
    return false;
  FILE_ATTRIBUTE_TAG_INFO info{};
  wchar_t finalPath[32768];
  if (!GetFileInformationByHandleEx(file, FileAttributeTagInfo, &info, sizeof(info)) ||
      (info.FileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY)) ||
      !ProtectedObject(path))
    return false;
  n = GetFinalPathNameByHandleW(file, finalPath, 32768, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
  if (!n || n >= 32768 || _wcsicmp(finalPath, (L"\\\\?\\" + path).c_str()))
    return false;
  BCRYPT_ALG_HANDLE algorithm = nullptr;
  BCRYPT_HASH_HANDLE hash = nullptr;
  unsigned char digest[32]{};
  bool ok = false;
  if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) == 0 &&
      BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) == 0) {
    ok = true;
    unsigned char buffer[4096];
    DWORD count;
    for (;;) {
      if (!ReadFile(file, buffer, sizeof(buffer), &count, nullptr)) {
        ok = false;
        break;
      }
      if (!count)
        break;
      if (BCryptHashData(hash, buffer, count, 0) != 0) {
        ok = false;
        break;
      }
    }
    if (ok)
      ok = BCryptFinishHash(hash, digest, 32, 0) == 0;
  }
  if (hash)
    BCryptDestroyHash(hash);
  if (algorithm)
    BCryptCloseAlgorithmProvider(algorithm, 0);
  const unsigned char expected[] = {0xC9, 0x19, 0xEE, 0x2F, 0xAE, 0xA9, 0x07, 0x16,
                                    0x9F, 0xE0, 0x22, 0x2B, 0xD8, 0xB1, 0xBD, 0x07,
                                    0xE0, 0x3E, 0x6A, 0x8E, 0x10, 0x6D, 0xFE, 0xA7,
                                    0xEE, 0x0A, 0x9A, 0x9C, 0xE5, 0xEE, 0x83, 0x80};
  return ok && memcmp(digest, expected, 32) == 0;
}
// END TESTABLE TRUST
// BEGIN TESTABLE RUNNER
bool Run(const std::wstring& args, std::string& output) {
  auto shutdown = shutdownDeadline.load();
  if ((shutdown && GetTickCount64() >= shutdown) ||
      (!shutdown && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0))
    return false;
  auto path = UtilityPath();
  Handle file;
  struct DirectoryLocks {
    std::vector<HANDLE> values;
    ~DirectoryLocks() {
      for (auto h : values)
        CloseHandle(h);
    }
  } locks;
  if (!LockUtility(path, file, locks.values)) {
    SetUtility(3);
    return false;
  }
  SetUtility(4);
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
  Handle readPipe, writePipe, input;
  if (!CreatePipe(readPipe.out(), writePipe.out(), &sa, 0) ||
      !SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0))
    return false;
  input.value = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa,
                            OPEN_EXISTING, 0, nullptr);
  if (!input)
    return false;
  STARTUPINFOEXW si{};
  si.StartupInfo.cb = sizeof(si);
  si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
  si.StartupInfo.hStdInput = input;
  si.StartupInfo.hStdOutput = writePipe;
  si.StartupInfo.hStdError = writePipe;
  SIZE_T bytes = 0;
  InitializeProcThreadAttributeList(nullptr, 1, 0, &bytes);
  std::vector<BYTE> storage(bytes);
  si.lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(storage.data());
  if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &bytes))
    return false;
  struct AttrCleanup {
    LPPROC_THREAD_ATTRIBUTE_LIST p;
    ~AttrCleanup() { DeleteProcThreadAttributeList(p); }
  } attrs{si.lpAttributeList};
  HANDLE allowed[] = {writePipe.value, input.value};
  if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, allowed,
                                 sizeof(allowed), nullptr, nullptr))
    return false;
  Handle job(CreateJobObjectW(nullptr, nullptr));
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
  limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  if (!job ||
      !SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
    return false;
  wchar_t system[32768];
  if (!GetSystemDirectoryW(system, 32768))
    return false;
  PROCESS_INFORMATION pi{};
  std::wstring command = L"\"" + path + L"\" " + args;
  if (!CreateProcessW(path.c_str(), command.data(), nullptr, nullptr, TRUE,
                      CREATE_NO_WINDOW | CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT, nullptr,
                      system, &si.StartupInfo, &pi))
    return false;
  Handle process(pi.hProcess), childThread(pi.hThread);
  if (!AssignProcessToJobObject(job, process)) {
    TerminateProcess(process, 1);
    WaitForSingleObject(process, 2000);
    return false;
  }
  if (ResumeThread(childThread) == DWORD(-1)) {
    TerminateJobObject(job, 1);
    return false;
  }
  CloseHandle(writePipe.value);
  writePipe.value = nullptr;
  auto end = GetTickCount64() + 10000;
  bool done = false, overflow = false;
  for (;;) {
    DWORD available = 0, count = 0;
    char buffer[1024];
    if (PeekNamedPipe(readPipe, nullptr, 0, nullptr, &available, nullptr) && available) {
      if (!ReadFile(readPipe, buffer, (std::min)(available, DWORD(sizeof(buffer))), &count,
                    nullptr))
        break;
      if (output.size() + count > 16384) {
        overflow = true;
        break;
      }
      output.append(buffer, count);
    }
    if (WaitForSingleObject(process, 0) == WAIT_OBJECT_0) {
      DWORD remaining = 0;
      if (!PeekNamedPipe(readPipe, nullptr, 0, nullptr, &remaining, nullptr) || !remaining) {
        done = true;
        break;
      }
    }
    auto now = GetTickCount64(), shutdown = shutdownDeadline.load();
    if (now >= end || (shutdown && now >= shutdown))
      break;
    if (!shutdown && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
      break;
    if (!available)
      Sleep(20);
  }
  if (!done) {
    TerminateJobObject(job, 1);
    WaitForSingleObject(process, 1000);
  }
  DWORD code = 1;
  if (!GetExitCodeProcess(process, &code))
    return false;
  Engine::SetInt(L"utilityExit", code);
  return done && !overflow && code == 0;
}
// END TESTABLE RUNNER
State Query() {
  std::string output;
  if (!Run(L"status", output))
    return {};
  return Parse(output);
}
bool Set(State target) {
  if (target.enabled && !ValidRange(target.start, target.stop))
    return false;
  std::string output;
  Run(target.enabled ? L"on " + std::to_wstring(target.stop) + L" " + std::to_wstring(target.start)
                     : L"off",
      output);
  State actual = Query();
  return actual.valid && actual.enabled == target.enabled &&
         (!target.enabled || (actual.start == target.start && actual.stop == target.stop));
}
constexpr PCWSTR recoveryKey = L"Software\\BillBauman\\ThinkPadBatteryControls";
bool OpenRecovery(HKEY& key) {
  Handle token;
  DWORD bytes = 0;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.out()))
    return false;
  GetTokenInformation(token, TokenUser, nullptr, 0, &bytes);
  std::vector<BYTE> buffer(bytes);
  if (!GetTokenInformation(token, TokenUser, buffer.data(), bytes, &bytes))
    return false;
  LPWSTR sid = nullptr;
  if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &sid))
    return false;
  std::wstring sddl = L"D:P(A;;KA;;;SY)(A;;KA;;;BA)(A;;KA;;;" + std::wstring(sid) + L")";
  LocalFree(sid);
  PSECURITY_DESCRIPTOR sd = nullptr;
  if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &sd,
                                                            nullptr))
    return false;
  SECURITY_ATTRIBUTES sa{sizeof(sa), sd, FALSE};
  DWORD disposition;
  auto rc = RegCreateKeyExW(HKEY_CURRENT_USER, recoveryKey, 0, nullptr, 0, KEY_READ | KEY_WRITE,
                            &sa, &key, &disposition);
  LocalFree(sd);
  return rc == ERROR_SUCCESS;
}
bool Save(const Recovery& value) {
  if (!ValidRecovery(value, Now()))
    return false;
  DATA_BLOB input{sizeof(value), reinterpret_cast<BYTE*>(const_cast<Recovery*>(&value))}, output{};
  if (!CryptProtectData(&input, L"ThinkPad Battery Controls recovery", nullptr, nullptr, nullptr,
                        CRYPTPROTECT_UI_FORBIDDEN, &output))
    return false;
  HKEY key = nullptr;
  bool ok = OpenRecovery(key);
  if (ok) {
    ok = RegSetValueExW(key, L"Recovery", 0, REG_BINARY, output.pbData, output.cbData) ==
         ERROR_SUCCESS;
    RegCloseKey(key);
  }
  LocalFree(output.pbData);
  if (ok) {
    recovery = value;
    active = value.active != 0;
  }
  return ok;
}
void Load() {
  HKEY key = nullptr;
  if (!OpenRecovery(key)) {
    recoveryInvalid = true;
    return;
  }
  BYTE buffer[4096];
  DWORD bytes = sizeof(buffer), type = 0;
  auto rc = RegQueryValueExW(key, L"Recovery", nullptr, &type, buffer, &bytes);
  RegCloseKey(key);
  if (rc == ERROR_FILE_NOT_FOUND) {
    // Do not import unauthenticated legacy state. Surface it for manual recovery.
    return;  // Shared diagnostic keys are never trusted as recovery input.
  }
  DATA_BLOB input{bytes, buffer}, output{};
  if (rc != ERROR_SUCCESS || type != REG_BINARY ||
      !CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN,
                          &output)) {
    recoveryInvalid = true;
    return;
  }
  Recovery candidate{};
  bool ok = output.cbData == sizeof(candidate);
  if (ok)
    memcpy(&candidate, output.pbData, sizeof(candidate));
  LocalFree(output.pbData);
  if (!ok || !ValidRecovery(candidate, Now())) {
    recoveryInvalid = true;
    return;
  }
  recovery = candidate;
  active = candidate.active != 0;
}
bool Clear() {
  Recovery empty;
  if (!Save(empty))
    return false;
  recoveryInvalid = false;
  return true;
}
// BEGIN TESTABLE RESTORE
bool Restore() {
  if (recoveryInvalid || !ValidRecovery(recovery, Now())) {
    Status(L"Recovery data invalid. Apply your desired limits manually.");
    return false;
  }
  if (!Active())
    return true;
  State current = Query();
  if (!current.valid)
    return false;
  if (!current.enabled && !Set({true, true, int(recovery.start), int(recovery.stop)}))
    return false;
  return Clear();
}
void Full() {
  if (Active() || recoveryInvalid)
    return;
  State state = Query();
  if (!state.valid || !state.enabled) {
    Error();
    return;
  }
  Recovery next;
  next.active = 1;
  next.start = state.start;
  next.stop = state.stop;
  next.deadline = Now() + 8 * 3600;
  if (!Save(next) || !Set({true, false, 0, 0})) {
    Error();
    return;
  }
  Status(L"Charge-to-full request saved.");
}
// END TESTABLE RESTORE
void Refresh() {
  State state = Query();
  {
    std::lock_guard lock(stateMutex);
    if (state.valid && !state.enabled) {
      if (Active() && ValidRange(recovery.start, recovery.stop)) {
        state.start = recovery.start;
        state.stop = recovery.stop;
      } else if (ValidRange(snapshot.state.start, snapshot.state.stop)) {
        state.start = snapshot.state.start;
        state.stop = snapshot.state.stop;
      }
    }
    snapshot.state = state;
    snapshot.revision++;
    refreshing = false;
  }
  Engine::SetInt(L"cacheValid", state.valid);
  Engine::SetInt(L"cacheEnabled", state.enabled);
  if (state.valid && state.enabled) {
    Engine::SetInt(L"cacheStart", state.start);
    Engine::SetInt(L"cacheStop", state.stop);
  }
}
// BEGIN TESTABLE SETUP COMMAND
std::wstring SetupCommand(const std::wstring& system,
                          const std::wstring& args,
                          const std::wstring& scratch) {
  auto powershell = system + L"\\WindowsPowerShell\\v1.0\\powershell.exe";
  auto native = system + L"\\cmd.exe";
  auto windows = system.substr(0, system.find_last_of(L'\\'));
  // No command-form FOR: even a /d parent otherwise spawns an AutoRun-enabled child.
  // The caller supplies a fresh directory beneath a verified protected parent.
  if (system.find_first_of(L"\"%&|<>^!()") != system.npos ||
      scratch.find_first_of(L"\"%&|<>^!()") != scratch.npos || scratch.empty())
    throw 1;
  auto file = scratch + L"\\e";
  auto acl = L"\"" + system + L"\\icacls.exe\" \"" + scratch + L"\"";
  auto fail = L"(rmdir \"" + scratch + L"\" 2>nul & exit /b 20)";
  auto protect =
      acl +
      L" /inheritance:r /grant:r \"*S-1-5-32-544:(OI)(CI)F\" \"*S-1-5-18:(OI)(CI)F\" >nul || " +
      fail + L" & " + acl + L" /setowner *S-1-5-32-544 >nul || " + fail + L" & ";
  std::wstring command =
      L"/d /e:on /v:off /s /c \"mkdir \"" + scratch + L"\" || exit /b 20 & " + protect +
      L"set > \"" + file + L"\" || (del \"" + file + L"\" 2>nul & rmdir \"" + scratch +
      L"\" 2>nul & exit /b 20) & (for /f \"usebackq delims==\" %V in (\"" + file +
      L"\") do @set \"%V=\") & del \"" + file + L"\" & rmdir \"" + scratch +
      L"\" & set \"SystemRoot=" + windows + L"\" & set \"WINDIR=" + windows +
      L"\" & set \"ComSpec=" + native + L"\" & set \"PATH=" + system +
      L"\" & set \"PATHEXT=.COM;.EXE;.BAT;.CMD\" & set \"TEMP=" + windows +
      L"\\Temp\" & set \"TMP=" + windows + L"\\Temp\" & set \"PSModulePath=" + system +
      L"\\WindowsPowerShell\\v1.0\\Modules\" & \"" + powershell + L"\" " + args + L"\"";
  if (command.size() > 8000)
    throw 1;
  return command;
}
PCWSTR SetupResult(DWORD code) {
  switch (code) {
    case 0:
      return L"Lenovo utility is installed and verified.";
    case 20:
      return L"Setup could not create a protected folder. Check folder permissions.";
    case 21:
      return L"Download failed. Check your connection and try again.";
    case 22:
      return L"Lenovo's download has changed. Update this mod for a reviewed utility version.";
    case 23:
      return L"Lenovo's publisher signature could not be verified. Nothing was installed.";
    case 24:
      return L"The utility could not be installed. Check folder permissions or whether it is in "
             L"use.";
    default:
      return L"Setup did not complete. Check Lenovo support or try again.";
  }
}
// END TESTABLE SETUP COMMAND
std::wstring SetupScratch() {
  PWSTR raw = nullptr;
  if (FAILED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, nullptr, &raw)))
    throw 1;
  std::wstring root(raw);
  CoTaskMemFree(raw);
  auto attributes = GetFileAttributesW(root.c_str());
  if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
      !ProtectedObject(root) || !ProtectedObject(root.substr(0, 3), true))
    throw 1;
  GUID id{};
  if (FAILED(CoCreateGuid(&id)))
    throw 1;
  wchar_t text[40]{};
  StringFromGUID2(id, text, 40);
  return root + L"\\TBC-setup-" + text;
}
// BEGIN TESTABLE SETUP FLOW
DWORD WINAPI SetupThread(void*) noexcept {
  try {
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    struct ComCleanup {
      HRESULT hr;
      ~ComCleanup() {
        if (SUCCEEDED(hr))
          CoUninitialize();
      }
    } comCleanup{com};
    wchar_t system[32768];
    if (!GetSystemDirectoryW(system, 32768))
      throw 1;
    std::wstring powershell = std::wstring(system) + L"\\WindowsPowerShell\\v1.0\\powershell.exe";
    std::wstring args =
        L"-NoProfile -NonInteractive -EncodedCommand "
        L"JABtAD0AWwBJAE8ALgBNAGUAbQBvAHIAeQBTAHQAcgBlAGEAbQBdADoAOgBuAGUAdwAoAFsAQwBvAG4AdgBlAHIAd"
        L"ABdADoA"
        L"OgBGAHIAbwBtAEIAYQBzAGUANgA0AFMAdAByAGkAbgBnACgAJwBwAFYAZAA1AGMAOQBwAEcARgBQACsAZgBUADcAR"
        L"gBOAG0A"
        L"SgBHAFkAVwBrAEoAZwB3AEkAQQBuADAAdwBvAFEAQwBaADMAWQBVAEcATQBuADAAMwBFADgAbQBVAFYANgB3AEQAW"
        L"gBDAHEA"
        L"NgB4AFcASABFADMAOAAzAGYAdABXAEUAZwBKAHMANAA4AFEAdABNADcAYQBPAGQAKwB3ADcAZgB1AC8AUQBhACsAS"
        L"QBzAEoA"
        L"dQBCADUANABKAEUASQBaAEIAeQBhAHAATQBkAEoAdwBDAFUAQgBIADUAWgBVAEEAcQBGAGsAeABjAFcAWABLAEsAU"
        L"QB1AEUA"
        L"SgBlAEgARwA3AE0AUQBVAGsARQBYACsAbQAyADAAWQB0AEsAZAAzAHgAWAAvAGoAQgBuAEkAVQBxAEgAbwBDAE0AR"
        L"wBGADcA"
        L"VQByAEcAZwA1AEcAQQBLAFEAZwBJAFUATwBRAE4AMABjAGEAUwBoADEAcQBoAEMATQBHAHkAUABSAHAAZgBjAEMAL"
        L"wAyAFkA"
        L"VQBUAGwASABFAG4ARgAwAGYAagBkADgATQBJAGgAdgB4AEwAdABVADAAcQBJAEYATwBNADYAQgBGAGUAQwA5ADQAN"
        L"QBHAGkA"
        L"awBuAHIAdABpAG8AdAB4ADYAbgAyAGIAYwBkAHUAVwBXAGUAVgBSAHEAdgB2AFcATgBWAHEAdABkAE4AcgBkAGkAc"
        L"QBkAG4A"
        L"bgBYAG0AVwBLAGQATwB3ADIANAA2AEYAYQB2AFIANgB6AHYAMgBtAGUATgBZAGQAcwB0AHUAZABaADIANgA0AHoAU"
        L"gBQAG0A"
        L"eABhAHEAVABGAHkANwA0AHUAagBXAEcALwBJAEgAWgA0AEcAUgBHAEsARABmAE8AcwBHAFMAQwBSADQAcwBJAEoAQ"
        L"gAzADcA"
        L"ZgBaAGIAawBIADMAdQBlAHkAQQBVAFYAZABkAEcAZwBzAC8AUQAwAHoANQBUAFoAcABWAEsAUgBMAHUAZQBzACsAR"
        L"ABMAGkA"
        L"SABxAGsAUQA2AFUARQBzAFMARgBkAEgAawBqAEIAZgBXAFcAMABGAEgARwBFAE4AbgA4AFUARABDAGsAUgBIAHYAT"
        L"wA3AHIA"
        L"bwAyAE4AaQBsAEUAMwBLAGsAMwB0AEoATABzADkAcgBSAHIAMQBXAGkAMQAvAGIARgBwAEcAcQA5ADYAdwByAEcAY"
        L"QB6AGkA"
        L"YgBSAGEAcABWAG0AdgBWAGgAdQAxAEYAawBxAGMAVgBpAHcAMAB2AEYAYgBEADIALwBwAHAAdABWAFYAdABuAEYAY"
        L"QBNAGEA"
        L"dgBXAHMAVQBqAHQAcgAxAGgAbwAxAHIAVgBTAFkAeABrAEUAUwBhAFcASgBIAEUAUQBoAHAAbwBLADAAeQBDAFYAc"
        L"QBQAEMA"
        L"YgB4AHkAcwBjAEUARQBTAGMARwBDADIAVgAwAHgAUgBHADkASwA1AEYAdQBCADQASwArAEkANQBpADMAUQBPAG4AV"
        L"ABWAEcA"
        L"SwBoAGIANAA3ADAAeQBtAFAAcABKAFEAQgBKAFcAWQB2AFMANQBjAEMARgBoAFoAMQBPAGkALwA1AEwASQBtAEsAU"
        L"AB4AEkA"
        L"RgBMACsAVQBoAGEAQQBJAEEAWQBYAFIARQA4AEoAdABzAFIAegBKAHIARwBFAGkAQgBnAFQARwBuAGoAawBkAGoAQ"
        L"QAwAFYA"
        L"ZABCADIAQgBJAHoAdABGAFMAQgBzAEkAaABoAGgAOABDAFgARwA4AGgAdQBSAGMAOABGAFgANQBOAFYATgBFAE4AR"
        L"QBwAHAA"
        L"TgBnAGoAMwB0AGIANABkAG0AcgBLAEsAMwBLAGYAVwBrADEAZABQAHoAUABhAHgAcgB2AEgATgBxAGQAYwBmAEsAV"
        L"gBNAGUA"
        L"NQBPAHcAbQA4AGcAOABWAE0ALwA2ADcAUgBqAGMARwBOAE8AeQBNAFUAYwBZAEQANQBlAEYAMQBEAGUAMwByAHcAW"
        L"QBlAFoA"
        L"cAA1AE4ARwBZAGkANwBrAHYAbQBCACsAdgBIAE8ANwBVAHkAYgBnAGIAWABBAEEAdgBJAGcAdgB3AGMATwBaAEMAU"
        L"wBTAEMA"
        L"bQBCAE8AbgBuAGQAbQB5AGcAVgBRAEYAOQBGAFgARgBBAGgANABvAHAAUgBuADkAdABxAHUAQwAxAEYAMABwAGMAc"
        L"ABBAFYA"
        L"dwBmAEMAUwBmAHIALwBaAHoAMwBZAEoAagBsADMAUQBSADIAQQBNAGoAeQBrAE0ANgByAFEAMAB2AGYAcABMAE0AO"
        L"QBTAHIA"
        L"agBRADkATgBzAFAAeQBJADMAYgBNADMAUwBDAFkAQQAzAEkATwBBADMAKwBqAEgASABlAFIAawB3AFUAeABaAFAAN"
        L"gBvADMA"
        L"MgB1AFMAQgBLAGIAcwBZAGQAZgBBAGkAOQAxADkAWAAwADYAagBJAGQAaABzAEwAcQBNAFQAQQBtAHYAWABqAHoAM"
        L"gBFAEkA"
        L"MABIAG4AdgBlAHkAMQBtAGMAcwBYAFYAMABwAGMAbABjADAAeABzAHgAUwBnAHgAaAB1AE0AOAArAEkAcQBrAGIAM"
        L"gB6ADEA"
        L"cABaAGwAOQBhAHgASwB2AGYARwBFAHoAdwBlAHkAMQA1AHMAUQBpAEEARgBmAGkAVwBiADcAUABsADkAcAB4AEYAR"
        L"AArAFoA"
        L"NQB3AFAAOQBXAGIAUgB5AFEAdwBxAHAAYgB3AHAAYQB4AHAAcQB1AGIAbgBhAE4AcgBjAFUATQBjAGMAaABrAGgAd"
        L"QBtAGYA"
        L"aABsAGMAeABvAGYAWQBJAEMANABOAHkAQQBUAEkAZwBuAHMAcQBpAFIANgBaAGIAQQBoAEYAZgBSAGgAdwBRAGwAM"
        L"gBYAHgA"
        L"NABIAGMAdwBpAGYAWABsAG8AYgA5AHYAbgBDAC8ANgB3AEsARABnAEUAbABHAGYAZgBZAFAALwBIAFEAbgBTAEUAc"
        L"ABiAHYA"
        L"NABaAEkAcABzADMAdwBjAFYARQBkADQAQwBrAHIAdwBVAHQAWQBHAGMAUABKADMANgBpAFkASABFAGwAVQBmAHUAe"
        L"QBXAGYA"
        L"cQBBAEMAYwBTAHUAMwBoAEIANQBFAHIAbQBBAGgAOABvADQAOQB6ADgAZQBtAHMAOQBDADEAWQBiAHQAagB2ADgAV"
        L"wAvAFgA"
        L"bgB1AGsAMgArAGYARABRAFgAZAB3ADMAcgBmAFAAegA4AC8ASABmADUAVQBPAG4AagB0ADIALwBtAHkAdABLADEAW"
        L"ABMAG8A"
        L"aQAzADEAOQBxAGEARQB2AFgARgA3AG4AdQBwAEQAdQBUAFUASQA0AHkANQBDAFQAOABJAHUATABJAG0AVABKADgAc"
        L"QBxAEUA"
        L"dgBsAE8AaAByAEUAMABMAG0AUABmAEwAKwB4AEMAZgBMAFMAMwBaAGsAMwBuAHYAbABDAGMAVQB1AGIASABBAHIAc"
        L"gBjAFUA"
        L"NgBPAHUAYQBoAFUAawBVAHIALwA5AFEAUABwAGwAYwB5AGQAUgA5AG0AeQBDAHkAVwA2ACsAcABZADMAUQB3ADYAe"
        L"QB5AEkA"
        L"SwBuAGgAZwA0AEcAMwBOAHcAZQAxADcAcAB5AEsARwBWAHoAUABCAFUAUgB6AFAATgB5AEUATgBXAGcANQBNAEkAN"
        L"wBpAFkA"
        L"awA5AHoAVgBoAHYANgBrAFYARwB5AGIAMABNADYAVQBVAHIALwBaAFYAVABrAGoAUgBhAG4ATAAyAEEAYgBZAFUAc"
        L"gByAGoA"
        L"TQBTAFMAKwBRAGcAaAB3AGkASgBjAFUARQBRAHEAUQBFAEkAbABZAFcAcgBiAG8AWQBFADIAaABWAHgAUQBqAE0AK"
        L"wBSAEcA"
        L"TwBpADMAYgAyAFAAbQA0AFkAbQBJAGEAbgBXAG4AbAA4AHgAcgBQAGsANQBxAFIAUwArAHAAcABjAFQAMABzAEkAd"
        L"gA1AG4A"
        L"SABvAFoAcQBuAGIASgBUAGYAQgAxAEMAUgBMAHgATABKAGIATQBUAFkAMgA5AG8AQQBHAGQAWQBTAE4AdQB0ADcAY"
        L"wBRAFYA"
        L"NwBuAGkATABsAGUAVgBrADMARQBmAHYAbABlAHQAQwBkAG0AdgAvAGEAaABTADMAZABYAEoAQQAxAEIAVgBjAHMAb"
        L"wBnAFcA"
        L"UABJAHYAWQBIAHkARQB5AFIAVgA4AGoAVABIAEEAeABMAGkASgBvAEUATQBqADUAbwA0AHcAQQBpAG8AdwB4AG8AM"
        L"QBnAFIA"
        L"SgB0AEwARwBVAGIAdABjAG4AbAByAHYAKwBsAEQAdwBKAGYAYwBkAFAAbQBpAEgATAByAHUAWgBCAEsAVgBwAFYAc"
        L"ABtAGwA"
        L"aABRAEgAKwBnAHcAKwBRADEAQgBlAEEASgBZAHYARABjAE8AbwAvAEMARgA5AFcAWAA2AEEAawBQAEoAVABpAEMAR"
        L"QBHADEA"
        L"bwAzAEsANABIADYANABqAFcAdQAyAEEAQgA2AHIAUQBpAGMATgA2ADYAaABmADEAWQBQAEcAbgBlAEIASQBhAFUAc"
        L"gBXAHYA"
        L"awBNAHMANwBhAG0AMgAvAFIAbgBIAEMATQA0AFgAWgBQAHoATwByAHQAWQBiAEoAVABQAGwARAA5AEMAQwAvAGMAV"
        L"gB4AEQA"
        L"egBnADMAZwBZAEEAbABnAHgAVgAyADEAegBsAFMAYwBvAEEAOABaAGQAUABwAGoAaABLAHgARwBjAEkAWABhAGQAd"
        L"gBOAEkA"
        L"OABhAHUAagBBAFAAQQBSAGQAWgB4AFQAagB0AGkANgBPAEYATQB5AGwAVwBaAFkANABuAFgASwBEAEYAWAB3ACsAb"
        L"gBCAFAA"
        L"QwAxAFoAcABmAFkANQA4AEEANQBFAEYAMQBzAEgARABnAFEAWABHADUAWQA1AGoAdABPAEcAcQArAGIATQBnAHUAT"
        L"ABPAFQA"
        L"VABUADkATgAxAGIANgBOAEgAbQBmAEoAUABYAFQAUgBOAHQAegBGAGkARwBpADEASgBLAFUAUgBuAFoANgBuAC8AT"
        L"wA2AGwA"
        L"bABNAHUAKwBCAEsAZQBxAHUAYQA5AEQAUABUADIAQwB2AHUASgBLAG4ALwA1AHgARQBoAEcAOABQADgAZgBGAHYAd"
        L"QBEAFkA"
        L"bgA5AEkASABCADgAUQA0AHkAZgAzAHkAUQBPAFgAawBKAHcAWgByAHcAegBLAEoAYgBXAGIAcgBQADgAcwBjAFQAR"
        L"gBTAGMA"
        L"OQBzAGsAVwBJAGkARQB4AHAASgBqAGgAcABpAEwAQQB6ADUAeQBNAFoAUgBpAGcAMwBIAC8AbgBoAHkAawB2AEUAW"
        L"AA0AEgA"
        L"QgA2AG0AUAA5ADIAYgBrADEAcwBEADYAeQB6ADIAcABTAG4AWABjAHMALwBtAGQAQwBOAFAAdgA3AGsAZQBiAEIAa"
        L"QAyADUA"
        L"eABuAHAAcgBxAE4ARwB6ADIATABpAGIAeQA3AHAAQQBrAGkANgAzAFoAZwBmAFcAWQBEAGQASQBEAEoAVgA2AEsAS"
        L"QBEAHUA"
        L"ZABzAEQAawBuAG0AQgBxAGMASAB5ADcALwBDADEAYQBtAFIAegB2AHQASwAxAEQARQB6AGIAcABxAHQAVwB1AE0AM"
        L"AB6AEgA"
        L"cAA4ADgAOAA2AFgAMABhAE4AVAB1AGIAVABSAGsAaQBuAEgAMwAvAGMAMwBEAE4AWABZAFAAZgBNAG4ARQBPAFQAY"
        L"QBjAGMA"
        L"cgA3AGsAQQArAE0ASwBGAGoAOABHAGMAdwBMAFoAMwBVAGEARgA2ADUAaQBxAHIAOQAyAEcAbABFAGQAYQBqAFIAN"
        L"QA1AFcA"
        L"RABqAFoAawB2AEQAQwBxAFAAOAB3ADIAdgBqAGQAbQBPADIASQBYAHYASQBoAGoAagAwADcAVgBGAHQAMQBtADIAZ"
        L"wA0AGsA"
        L"NABxAGYAVABXAGYAdABRAHEAagBnAHUAUgBVAHUAdgBTAGoAZwBqAHgAegBCAFAAZQBaAGYAJwApACkAOwAkAGQAP"
        L"QBbAEkA"
        L"TwAuAEMAbwBtAHAAcgBlAHMAcwBpAG8AbgAuAEQAZQBmAGwAYQB0AGUAUwB0AHIAZQBhAG0AXQA6ADoAbgBlAHcAK"
        L"AAkAG0A"
        L"LABbAEkATwAuAEMAbwBtAHAAcgBlAHMAcwBpAG8AbgAuAEMAbwBtAHAAcgBlAHMAcwBpAG8AbgBNAG8AZABlAF0AM"
        L"AApADsA"
        L"JgAoAFsAcwBjAHIAaQBwAHQAYgBsAG8AYwBrAF0AOgA6AEMAcgBlAGEAdABlACgAWwBJAE8ALgBTAHQAcgBlAGEAb"
        L"QBSAGUA"
        L"YQBkAGUAcgBdADoAOgBuAGUAdwAoACQAZAApAC4AUgBlAGEAZABUAG8ARQBuAGQAKAApACkAKQAgAC0AUQB1AGkAZ"
        L"QB0AA==";
    std::wstring native = std::wstring(system) + L"\\cmd.exe";
    auto command = SetupCommand(system, args, SetupScratch());
    SHELLEXECUTEINFOW info{sizeof(info)};
    info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    info.lpVerb = L"runas";
    info.lpFile = native.c_str();
    info.lpParameters = command.c_str();
    info.lpDirectory = system;
    info.nShow = SW_HIDE;
    SetupStatus(L"Waiting for Windows setup approval.");
    if (!ShellExecuteExW(&info) || !info.hProcess) {
      SetupStatus(L"Setup was cancelled or could not start.");
      setupRunning = false;
      return 0;
    }
    Handle process(info.hProcess);
    SetupStatus(L"Installing Lenovo utility...");
    HANDLE waits[] = {stopEvent, process.value};
    auto result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    if (result == WAIT_OBJECT_0 + 1) {
      DWORD code = 1;
      GetExitCodeProcess(process, &code);
      SetupStatus(SetupResult(code));
    } else
      SetupStatus(L"Setup continues separately while the mod stops.");
  } catch (...) {
    SetupStatus(L"Unable to start Lenovo utility setup.");
  }
  setupCompleted = true;
  setupRunning = false;
  if (!unloading)
    SetEvent(wakeEvent);
  return 0;
}
void SetupUtility(bool redo = false) {
  if (setupRunning.exchange(true))
    return;
  if (setupThread) {
    if (WaitForSingleObject(setupThread.load(), 0) != WAIT_OBJECT_0) {
      setupRunning = false;
      SetupStatus(L"Previous setup is finishing. Try again shortly.");
      return;
    }
    CloseHandle(setupThread.load());
    setupThread = nullptr;
  }
  if (!redo && ReadSnapshot().state.valid) {
    SetupStatus(L"Lenovo utility is already ready.");
    setupRunning = false;
    return;
  }
  SetupStatus(L"Starting setup...");
  setupThread = CreateThread(nullptr, 0, SetupThread, nullptr, 0, nullptr);
  if (!setupThread) {
    setupRunning = false;
    SetupStatus(L"Unable to start Lenovo utility setup.");
  }
}
// BEGIN TESTABLE DISPATCH
// END TESTABLE SETUP FLOW
void Dispatch(int action, int values) {
  if (action == 7) {
    SetupUtility(values != 0);
    return;
  }
  if (action == 1)
    Full();
  else if (action == 2) {
    if (!Restore())
      Error();
    else
      Status(L"Battery protection restored.");
  } else if (action == 5) {
    int start = LOWORD(values), stop = HIWORD(values);
    if (ValidRange(start, stop) && Set({true, true, start, stop}) && Clear())
      Status(L"Charge thresholds applied.");
    else
      Error();
  } else if (action == 6) {
    if (Clear() && Set({true, false, 0, 0}))
      Status(L"Charge limits off.");
    else
      Error();
  }
  Refresh();
}
void Submit(int action, int values = 0) noexcept {
  try {
    if (unloading || !wakeEvent) {
      if (action == 7)
        SetupStatus(L"Setup is unavailable while the mod stops.");
      return;
    }
    std::lock_guard lock(queueMutex);
    // Bound work and coalesce repeated UI commands, including refresh requests.

    if (action == 0)
      refreshing = true;
    if (!commands.empty() && commands.back().first == action) {
      commands.back().second = values;
      return;
    }
    if (commands.size() >= 8) {
      if (action == 7)
        SetupStatus(L"Another battery operation is pending. Try setup again shortly.");
      Status(L"Another battery operation is pending.");
      return;
    }
    commands.emplace_back(action, values);
    SetEvent(wakeEvent);
  } catch (...) {
    if (action == 7)
      SetupStatus(L"Unable to queue setup. Try again.");
    Status(L"Unable to queue battery operation.");
  }
}
// END TESTABLE DISPATCH
void CALLBACK
ShellEvent(HWINEVENTHOOK, DWORD, HWND window, LONG object, LONG, DWORD, DWORD) noexcept {
  if (object != OBJID_WINDOW || unloading)
    return;
  wchar_t name[64]{};
  if (GetClassNameW(window, name, 64) && wcscmp(name, L"Shell_TrayWnd") == 0)
    SetEvent(wakeEvent);
}
DWORD WINAPI Thread(void*) noexcept {
  HWINEVENTHOOK shellHook = nullptr;
  try {
    // Only the actual shell process owns charging/recovery, even at shell startup.
    for (;;) {
      HWND bar = FindWindowW(L"Shell_TrayWnd", nullptr);
      DWORD pid = 0;
      if (bar && GetWindowThreadProcessId(bar, &pid)) {
        if (pid != GetCurrentProcessId())
          return 0;
        break;
      }
      if (WaitForSingleObject(stopEvent, 500) == WAIT_OBJECT_0)
        return 0;
    }
    Status(L"");
    Load();
    if (recoveryInvalid)
      Status(L"Recovery data invalid. Apply your desired limits manually.");
    shellHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, nullptr, ShellEvent,
                                GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    ULONGLONG attachAt = 0, checkAt = 0;
    int attempts = 0;
    Submit(0);
    for (;;) {
      if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
        break;
      if (setupCompleted.exchange(false))
        Refresh();
      auto now = GetTickCount64();
      if (now >= attachAt) {
        AttachNativeMenu();
        attachAt = now + (attempts++ < 10 ? 2000 : 60000);
      }
      if (Active() && now >= checkAt) {
        SYSTEM_POWER_STATUS power{};
        if ((GetSystemPowerStatus(&power) && power.BatteryLifePercent == 100) ||
            RecoveryDue(recovery, Now())) {
          if (!Restore())
            Error();
          Refresh();
        }
        checkAt = now + 30000;
      }
      std::pair<int, int> command{};
      bool have = false;
      {
        std::lock_guard lock(queueMutex);
        if (!commands.empty()) {
          command = commands.front();
          commands.pop_front();
          have = true;
        }
      }
      if (have) {
        busy = true;
        try {
          Dispatch(command.first, command.second);
        } catch (...) {
          Error();
        }
        busy = false;
        continue;
      }
      DWORD delay = DWORD((std::min)(ULONGLONG(60000), attachAt > now ? attachAt - now : 0));
      if (Active())
        delay = (std::min)(delay, DWORD(checkAt > now ? checkAt - now : 0));

      HANDLE events[] = {stopEvent, wakeEvent};
      DWORD result = MsgWaitForMultipleObjects(2, events, FALSE, delay, QS_ALLINPUT);
      if (result == WAIT_OBJECT_0)
        break;
      if (result == WAIT_OBJECT_0 + 1)
        attachAt = 0;
      if (result == WAIT_OBJECT_0 + 2) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
          TranslateMessage(&msg);
          DispatchMessageW(&msg);
        }
      }
    }
    if (shellHook) {
      UnhookWinEvent(shellHook);
      shellHook = nullptr;
    }
    ULONGLONG empty = 0;
    shutdownDeadline.compare_exchange_strong(empty, GetTickCount64() + 4000);
    if (Active() && !Restore())
      Status(L"Restoration deferred. Saved recovery will be checked next time the mod runs.");
  } catch (...) {
    Error();
  }
  if (shellHook)
    UnhookWinEvent(shellHook);
  return 0;
}
}  // namespace BatteryControls

// Taskbar XAML access helpers adapted from Taskbar tray system icon tweaks.
// Copyright (c) Michael Maltsev (m417z), MIT; see THIRD-PARTY-NOTICES.txt.
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Text.h>
#include <memory>
#include <tuple>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <vector>
#include <atomic>
using namespace winrt::Windows::UI::Xaml;
using WindhawkUtils::HookSymbols;
void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
  HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
  if (!hTaskSwWnd) {
    return nullptr;
  }

  void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
  if (!taskBand)
    return nullptr;
  void* taskBandForTaskListWndSite = taskBand;
  bool found = false;
  for (int i = 0; i <= 20; i++) {
    void* table = nullptr;
    SIZE_T read = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), taskBandForTaskListWndSite, &table, sizeof(table),
                           &read) ||
        read != sizeof(table))
      return nullptr;
    if (table == CTaskBand_ITaskListWndSite_vftable) {
      found = true;
      break;
    }
    taskBandForTaskListWndSite = static_cast<BYTE*>(taskBandForTaskListWndSite) + sizeof(void*);
  }
  if (!found)
    return nullptr;

  void* taskbarHostSharedPtr[2]{};
  CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);
  if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
    return nullptr;
  }

  size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
  {
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 &&
        b[6] == 0xC1 && b[7] <= 0x7F) {
      taskbarElementIUnknownOffset = b[7];
    } else {
      Engine::Log(L"Unsupported TaskbarHost::FrameHeight");
      std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
      return nullptr;
    }
  }
#elif defined(_M_ARM64)
  {
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
        (p[3] & 0xFFF00FE0) == 0xF8400C00) {
      taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
    } else {
      Engine::Log(L"Unsupported TaskbarHost::FrameHeight");
    }
  }
#else
#error "Unsupported architecture"
#endif

  IUnknown* taskbarElementIUnknown = nullptr;
  SIZE_T read = 0;
  if (!ReadProcessMemory(GetCurrentProcess(),
                         static_cast<BYTE*>(taskbarHostSharedPtr[0]) + taskbarElementIUnknownOffset,
                         &taskbarElementIUnknown, sizeof(taskbarElementIUnknown), &read)) {
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return nullptr;
  }

  if (!taskbarElementIUnknown) {
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return nullptr;
  }
  FrameworkElement taskbarElement = nullptr;
  taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                         winrt::put_abi(taskbarElement));

  auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

  std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

  return result;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void*);
struct UiRequest {
  std::mutex mutex;
  bool enabled = true;
  RunFromWindowThreadProc_t proc;
  void* parameter;
  ULONG_PTR nonce;
};
std::mutex uiRequestMutex, uiInvokeMutex;
std::shared_ptr<UiRequest> uiRequest;
const UINT uiMessage = RegisterWindowMessageW(L"ThinkPadBatteryControls.Internal.Dispatch.v2");
LRESULT CALLBACK UiHook(int code, WPARAM w, LPARAM l) noexcept {
  try {
    if (code == HC_ACTION) {
      auto message = reinterpret_cast<CWPSTRUCT*>(l);
      if (message->message == uiMessage) {
        std::shared_ptr<UiRequest> request;
        {
          std::lock_guard lock(uiRequestMutex);
          request = uiRequest;
        }
        if (request) {
          std::lock_guard lock(request->mutex);
          if (request->enabled && message->wParam == request->nonce) {
            request->enabled = false;
            request->proc(request->parameter);
          }
        }
      }
    }
  } catch (...) {
    BatteryControls::Status(L"Taskbar integration failed.");
  }
  return CallNextHookEx(nullptr, code, w, l);
}
bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t proc, void* parameter) {
  DWORD thread = GetWindowThreadProcessId(window, nullptr);
  if (!thread)
    return false;
  if (thread == GetCurrentThreadId()) {
    proc(parameter);
    return true;
  }
  std::lock_guard invocation(uiInvokeMutex);
  auto request = std::make_shared<UiRequest>();
  request->proc = proc;
  request->parameter = parameter;
  if (BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&request->nonce), sizeof(request->nonce),
                      BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0)
    return false;
  HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, UiHook, nullptr, thread);
  if (!hook)
    return false;
  {
    std::lock_guard lock(uiRequestMutex);
    uiRequest = request;
  }
  DWORD_PTR result = 0;
  bool sent = SendMessageTimeoutW(window, uiMessage, request->nonce, 0,
                                  SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result) != 0;
  {
    std::lock_guard lock(request->mutex);
    request->enabled = false;
  }
  {
    std::lock_guard lock(uiRequestMutex);
    uiRequest.reset();
  }
  UnhookWindowsHookEx(hook);
  return sent;
}

bool HookTaskbarDllSymbols() {
  HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!module) {
    Engine::Log(L"Failed to load taskbar.dll");
    return false;
  }

  WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
      {
          {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CTaskBand_ITaskListWndSite_vftable,
      },
      {
          {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
          &CTaskBand_GetTaskbarHost_Original,
      },
      {
          {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
          &TaskbarHost_FrameHeight_Original,
      },
      {
          {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
          &std__Ref_count_base__Decref_Original,
      },
  };

  return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

template <class F>
auto SafeUi(F fn) {
  return [fn = std::move(fn)](auto const& a, auto const& b) noexcept {
    if (unloading)
      return;
    try {
      fn(a, b);
    } catch (...) {
      BatteryControls::Status(L"Unable to display or update battery controls.");
    }
  };
}
namespace ModernUI {
// BEGIN TESTABLE UI REVOCATION
std::vector<std::function<void()>> uiRevokers;
template <class T>
void TrackEvent(T revoker) {
  auto owned = std::make_shared<T>(std::move(revoker));
  uiRevokers.emplace_back([owned] { owned->revoke(); });
}
void RevokeUiEvents() {
  auto pending = std::move(uiRevokers);
  uiRevokers.clear();
  for (auto& revoke : pending)
    revoke();
}
// END TESTABLE UI REVOCATION
namespace C = winrt::Windows::UI::Xaml::Controls;
namespace P = winrt::Windows::UI::Xaml::Controls::Primitives;
C::MenuFlyout menu{nullptr};
C::Flyout flyout{nullptr};
DispatcherTimer pendingMenu{nullptr}, statusTimer{nullptr};
struct PresetEditor;
std::shared_ptr<PresetEditor> presetEditor;
// BEGIN TESTABLE PRESETS
struct Preset {
  std::wstring name;
  int start, stop;
  bool quick;
};
std::vector<Preset> DefaultPresets() {
  return {{L"Desk", 75, 80, true},
          {L"On the go", 85, 90, true},
          {L"Full", 95, 100, true},
          {L"Custom", 60, 65, true}};
}
bool MatchesPreset(const Preset& p, bool valid, bool enabled, bool temporary, int start, int stop) {
  return valid && enabled && !temporary && p.start == start && p.stop == stop;
}
constexpr size_t maxPresets = 20, maxQuickPresets = 5;
struct PresetRecordV1 {
  wchar_t name[41]{};
  DWORD start = 0, stop = 0, quick = 0;
};
struct PresetStoreV1 {
  DWORD version = 1, count = 0;
  PresetRecordV1 rows[maxPresets]{};
};
struct PresetRecord {
  wchar_t name[81]{};
  DWORD start = 0, stop = 0, quick = 0;
};
struct PresetStore {
  DWORD version = 2, count = 0;
  PresetRecord rows[maxPresets]{};
};
bool NameSpace(wchar_t c) {
  return c == L' ' || c == L'\t' || c == 0x00a0 || c == 0x1680 || (c >= 0x2000 && c <= 0x200a) ||
         c == 0x202f || c == 0x205f || c == 0x3000;
}
std::wstring NormalizeName(std::wstring name) {
  std::erase_if(name, [](wchar_t c) {
    return c == 0x061c || c == 0x200e || c == 0x200f || (c >= 0x202a && c <= 0x202e) ||
           (c >= 0x2066 && c <= 0x2069);
  });
  while (!name.empty() && NameSpace(name.front()))
    name.erase(name.begin());
  while (!name.empty() && NameSpace(name.back()))
    name.pop_back();
  return name;
}
bool ValidPresetName(const std::wstring& name) {
  if (name.empty() || name.size() > 80 || NormalizeName(name) != name)
    return false;
  size_t count = 0;
  for (size_t i = 0; i < name.size(); i++) {
    auto c = name[i];
    if (c < 32 || (c >= 0x7f && c <= 0x9f) || c == 0x00ad || c == 0x034f || c == 0x061c ||
        c == 0x180e || (c == 0x200b || c == 0x200e || c == 0x200f) ||
        (c >= 0x2028 && c <= 0x202e) || (c >= 0x2060 && c <= 0x206f) || c == 0xfeff)
      return false;
    if (c >= 0xd800 && c <= 0xdbff) {
      if (++i >= name.size() || name[i] < 0xdc00 || name[i] > 0xdfff)
        return false;
    } else if (c >= 0xdc00 && c <= 0xdfff)
      return false;
    if (++count > 40)
      return false;
  }
  return true;
}
std::wstring presetNotice;
std::wstring RepairPresetName(std::wstring name, size_t index) {
  name = NormalizeName(name);
  std::wstring clean;
  size_t count = 0;
  for (size_t i = 0; i < name.size() && count < 40; i++) {
    std::wstring unit(1, name[i]);
    if (name[i] >= 0xd800 && name[i] <= 0xdbff && i + 1 < name.size() && name[i + 1] >= 0xdc00 &&
        name[i + 1] <= 0xdfff)
      unit += name[++i];
    // A space is valid within a name even though not alone.
    if (NameSpace(unit[0]) || ValidPresetName(unit)) {
      clean += unit;
      count++;
    }
  }
  clean = NormalizeName(clean);
  return ValidPresetName(clean) ? clean : L"Preset " + std::to_wstring(index + 1);
}
bool DecodePresets(const PresetStore& data, std::vector<Preset>& result) {
  if (data.version != 2 || data.count > maxPresets)
    return false;
  std::vector<Preset> decoded;
  size_t quick = 0;
  for (DWORD i = 0; i < data.count; i++) {
    const auto& row = data.rows[i];
    size_t length = 0;
    while (length < 81 && row.name[length])
      length++;
    auto name = RepairPresetName(std::wstring(row.name, length), i);
    if (name != std::wstring(row.name, length))
      presetNotice = L"Some preset names were cleaned up. Save to keep the cleaned names.";
    if (row.start > 99 || row.stop > 100 ||
        !BatteryControls::ValidRange(int(row.start), int(row.stop)) || row.quick > 1)
      return false;
    quick += row.quick;
    if (quick > maxQuickPresets)
      return false;
    decoded.push_back({name, int(row.start), int(row.stop), row.quick != 0});
  }
  result = std::move(decoded);
  return true;
}
bool SavePresets(const std::vector<Preset>& rows) {
  if (rows.size() > maxPresets)
    return false;
  PresetStore data;
  data.count = DWORD(rows.size());
  for (size_t i = 0; i < rows.size(); i++) {
    const auto& row = rows[i];
    if (!ValidPresetName(row.name) || !BatteryControls::ValidRange(row.start, row.stop))
      return false;
    std::copy(row.name.begin(), row.name.end(), data.rows[i].name);
    data.rows[i].start = row.start;
    data.rows[i].stop = row.stop;
    data.rows[i].quick = row.quick;
  }
  std::vector<Preset> checked;
  if (!DecodePresets(data, checked))
    return false;
  HKEY key = nullptr;
  if (!BatteryControls::OpenRecovery(key))
    return false;
  auto rc = RegSetValueExW(key, L"PresetLibrary", 0, REG_BINARY,
                           reinterpret_cast<const BYTE*>(&data), sizeof(data));
  RegCloseKey(key);
  return rc == ERROR_SUCCESS;
}
std::vector<Preset> ReadPresets() {
  presetNotice.clear();
  PresetStore data;
  DWORD bytes = sizeof(data), type = 0;
  HKEY key = nullptr;
  auto rc =
      RegOpenKeyExW(HKEY_CURRENT_USER, BatteryControls::recoveryKey, 0, KEY_QUERY_VALUE, &key);
  if (rc == ERROR_SUCCESS) {
    rc = RegQueryValueExW(key, L"PresetLibrary", nullptr, &type, reinterpret_cast<BYTE*>(&data),
                          &bytes);
    RegCloseKey(key);
  }
  if (rc == ERROR_SUCCESS && type == REG_BINARY && bytes == sizeof(PresetStoreV1) &&
      data.version == 1) {
    PresetStoreV1 old;
    memcpy(&old, &data, sizeof(old));
    data = PresetStore{};
    data.count = old.count;
    if (old.count <= maxPresets)
      for (size_t i = 0; i < old.count; i++) {
        if (old.rows[i].name[40]) {
          data.version = 0;
          break;
        }
        std::copy(std::begin(old.rows[i].name), std::end(old.rows[i].name), data.rows[i].name);
        data.rows[i].start = old.rows[i].start;
        data.rows[i].stop = old.rows[i].stop;
        data.rows[i].quick = old.rows[i].quick;
      }
    bytes = sizeof(data);
  }
  if (rc == ERROR_SUCCESS) {
    std::vector<Preset> rows;
    if (type == REG_BINARY && bytes == sizeof(data) && DecodePresets(data, rows))
      return rows;
    presetNotice =
        L"Saved presets could not be read safely. Restore defaults or add replacements, then Save.";
    BatteryControls::Status(presetNotice.c_str());
    return {};
  }
  if (rc != ERROR_FILE_NOT_FOUND) {
    presetNotice = L"Unable to read saved presets.";
    BatteryControls::Status(presetNotice.c_str());
    return {};
  }
  // Migrate the previous Windhawk settings without modifying charging or deleting old entries.
  std::vector<Preset> result;
  size_t favorites = 0, ignored = 0;
  bool any = false;
  for (int i = 0; i < 128; i++) {
    auto base = L"presets[" + std::to_wstring(i) + L"].";
    std::wstring name = Engine::Setting((base + L"name").c_str());
    int start = Engine::SettingInt((base + L"start").c_str()),
        stop = Engine::SettingInt((base + L"stop").c_str());
    if (name.empty() && !start && !stop)
      continue;
    any = true;
    auto originalName = name;
    name = NormalizeName(name);
    if (originalName != name)
      presetNotice = L"Legacy preset names were cleaned up. Review before saving.";
    bool quick = Engine::SettingInt((base + L"quick").c_str()) != 0 && favorites < maxQuickPresets;
    if (!ValidPresetName(name) || !BatteryControls::ValidRange(start, stop) ||
        result.size() >= maxPresets) {
      ignored++;
      continue;
    }
    if (quick)
      favorites++;
    result.push_back({name, start, stop, quick});
  }
  if (!any)
    result = DefaultPresets();
  if (ignored)
    presetNotice = std::to_wstring(ignored) +
                   L" legacy presets ignored: invalid name/range or more than 20 entries. Review "
                   L"before saving.";
  return result;
}
// END TESTABLE PRESETS
std::wstring PresetLabel(const Preset& p) {
  return p.name + L" (" + std::to_wstring(p.start) + L" to " + std::to_wstring(p.stop) + L"%)";
}
struct Panel {
  C::Slider start, stop;
  C::TextBlock startLabel, stopLabel;
  C::ToggleSwitch enabled;
  std::vector<std::tuple<P::ToggleButton, int, int>> choices;
  bool adjusting = false, updatingPresets = false;
  void UpdatePresets() {
    for (auto& [button, startLimit, stopLimit] : choices) {
      bool selected = MatchesPreset({L"", startLimit, stopLimit, false}, true, enabled.IsOn(),
                                    false, int(start.Value()), int(stop.Value()));
      if (button.IsChecked().Value() != selected)
        button.IsChecked(selected);
    }
  }
};
std::shared_ptr<Panel> panel;
struct ThresholdDraft {
  bool valid = false, enabled = false;
  int start = 75, stop = 80;
};
ThresholdDraft returningDraft, addPresetDraft;
int presetReturnView = 0;
void Close() {
  RevokeUiEvents();
  if (statusTimer)
    statusTimer.Stop();
  statusTimer = nullptr;
  presetEditor.reset();
  if (pendingMenu)
    pendingMenu.Stop();
  pendingMenu = nullptr;
  if (menu)
    menu.Hide();
  if (flyout) {
    flyout.Hide();
    flyout.Content(nullptr);
  }
  if (menu)
    menu.Items().Clear();
  menu = nullptr;
  flyout = nullptr;
  panel.reset();
}
C::TextBlock Text(const std::wstring& value, double size = 14) {
  C::TextBlock t;
  t.Text(value);
  t.FontSize(size);
  t.TextWrapping(TextWrapping::Wrap);
  return t;
}
void Send(int command, int limits = 0) {
  if (command == 4)
    ShellExecuteW(nullptr, L"open", L"ms-settings:batterysaver", nullptr, nullptr, SW_SHOWNORMAL);
  else
    BatteryControls::Submit(command, limits);
}
// Taskbar XAML islands are only taskbar-sized. Place the measured popup in
// physical monitor coordinates instead of trusting their automatic placement.
void PositionPanel(FrameworkElement target, C::Flyout current) {
  auto root = target.XamlRoot();
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  if (!root || !GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar) ||
      !GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi))
    return;
  double scale = root.RasterizationScale(), gap = 12 * scale;
  for (auto popup : Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
    auto presenter = popup.Child().try_as<C::FlyoutPresenter>();
    if (!presenter || presenter.Content() != current.Content())
      continue;
    presenter.UpdateLayout();
    auto origin = presenter.TransformToVisual(root.Content()).TransformPoint({0, 0});
    auto anchor = target.TransformToVisual(root.Content()).TransformPoint({0, 0});
    double width = presenter.ActualWidth() * scale, height = presenter.ActualHeight() * scale;
    if (width <= 0 || height <= 0)
      return;
    double x = bar.left + (anchor.X + target.ActualWidth()) * scale - width;
    double y = bar.top - gap - height;
    if (bar.right - bar.left >= bar.bottom - bar.top) {
      if (bar.top - mi.rcMonitor.top <= mi.rcMonitor.bottom - bar.bottom)
        y = bar.bottom + gap;
    } else {
      x = bar.left - mi.rcMonitor.left <= mi.rcMonitor.right - bar.right ? bar.right + gap
                                                                         : bar.left - gap - width;
      y = mi.rcWork.bottom - gap - height;
    }
    x = (std::max)(mi.rcWork.left + gap, (std::min)(x, mi.rcWork.right - gap - width));
    y = (std::max)(mi.rcWork.top + gap, (std::min)(y, mi.rcWork.bottom - gap - height));
    popup.HorizontalOffset(popup.HorizontalOffset() + (x - bar.left - origin.X * scale) / scale);
    popup.VerticalOffset(popup.VerticalOffset() + (y - bar.top - origin.Y * scale) / scale);
    Engine::SetString(L"panelPlacement",
                      (std::to_wstring(int(x)) + L"," + std::to_wstring(int(y)) + L" " +
                       std::to_wstring(int(width)) + L"x" + std::to_wstring(int(height)))
                          .c_str());
  }
}
void QueuePanel(FrameworkElement target, int view);
void ShowQuickMenu(FrameworkElement target);
C::Grid PresetGrid(bool editing = false) {
  C::Grid grid;
  grid.ColumnSpacing(8);
  for (double width :
       editing ? std::vector<double>{0, 64, 64, 84, 34, 34, 76} : std::vector<double>{0, 64, 64}) {
    C::ColumnDefinition c;
    c.Width(GridLength{width ? width : 1, width ? GridUnitType::Pixel : GridUnitType::Star});
    grid.ColumnDefinitions().Append(c);
  }
  return grid;
}
void Cell(C::Grid grid, FrameworkElement value, int column) {
  C::Grid::SetColumn(value, column);
  grid.Children().Append(value);
}
C::Grid PresetHeading(bool editing = false) {
  auto grid = PresetGrid(editing);
  Cell(grid, Text(L"Name", 12), 0);
  Cell(grid, Text(L"Start %", 12), 1);
  Cell(grid, Text(L"Stop %", 12), 2);
  if (editing) {
    Cell(grid, Text(L"Quick menu", 12), 3);
    auto order = Text(L"Order", 12);
    C::Grid::SetColumnSpan(order, 2);
    Cell(grid, order, 4);
  }
  return grid;
}
C::Grid PresetDisplay(const Preset& preset) {
  auto grid = PresetGrid();
  auto name = Text(preset.name);
  name.TextTrimming(TextTrimming::CharacterEllipsis);
  name.TextWrapping(TextWrapping::NoWrap);
  Cell(grid, name, 0);
  Cell(grid, Text(std::to_wstring(preset.start) + L"%"), 1);
  Cell(grid, Text(std::to_wstring(preset.stop) + L"%"), 2);
  C::ToolTipService::SetToolTip(grid, winrt::box_value(PresetLabel(preset)));
  return grid;
}
struct PresetEditorRow {
  C::Grid grid{nullptr};
  C::TextBox name, start, stop;
  C::CheckBox quick;
  C::Button up, down;
  C::Border grip;
  Media::TranslateTransform translation;
};
struct PresetEditor {
  std::vector<std::shared_ptr<PresetEditorRow>> rows;
  C::StackPanel list;
  C::TextBlock message, count;
  C::Button add;
  C::ScrollViewer scroll;
  P::ScrollBar scrollbar;
  C::Border dropLine;
  Media::TranslateTransform dropTranslation;
  std::weak_ptr<PresetEditorRow> dragged;
  ULONGLONG lastDragScroll = 0;
  double dragStartY = 0;
  size_t dragSlot = 0;
  uint32_t pointerId = 0;
  int returnView = 0;
  bool changing = false, dragMoved = false;
};
void EditorCounts(const std::shared_ptr<PresetEditor>& editor) {
  int favorites = 0;
  for (auto& row : editor->rows)
    if (row->quick.IsChecked().Value())
      favorites++;
  editor->count.Text(std::to_wstring(editor->rows.size()) + L" of 20 presets  |  " +
                     std::to_wstring(favorites) + L" of 5 quick favorites");
  editor->add.IsEnabled(editor->rows.size() < maxPresets);
  editor->scroll.Height((std::min)(220.0, (std::max)(38.0, double(editor->rows.size()) * 38 - 6)));
  for (size_t i = 0; i < editor->rows.size(); i++) {
    editor->rows[i]->up.IsEnabled(i > 0);
    editor->rows[i]->down.IsEnabled(i + 1 < editor->rows.size());
  }
}
// BEGIN TESTABLE PRESET ORDER
template <class T>
bool MovePresetRow(std::vector<T>& rows, size_t from, size_t slot) {
  if (from >= rows.size() || slot > rows.size())
    return false;
  size_t to = slot > from ? slot - 1 : slot;
  if (to == from)
    return false;
  auto item = std::move(rows[from]);
  rows.erase(rows.begin() + from);
  rows.insert(rows.begin() + to, std::move(item));
  return true;
}
// END TESTABLE PRESET ORDER
void MoveEditorRow(const std::shared_ptr<PresetEditor>& editor,
                   const std::shared_ptr<PresetEditorRow>& row,
                   size_t slot) {
  auto it = std::find(editor->rows.begin(), editor->rows.end(), row);
  if (it == editor->rows.end())
    return;
  if (!MovePresetRow(editor->rows, size_t(it - editor->rows.begin()), slot))
    return;
  editor->list.Children().Clear();
  for (auto& item : editor->rows)
    editor->list.Children().Append(item->grid);
  EditorCounts(editor);
}
void AddEditorRow(const std::shared_ptr<PresetEditor>& editor, const Preset& preset) {
  if (editor->rows.size() >= maxPresets)
    return;
  auto row = std::make_shared<PresetEditorRow>();
  row->grid = C::Grid();
  row->grid.ColumnSpacing(6);
  for (double width : {28.0, 40.0, 0.0, 64.0, 64.0, 28.0, 28.0}) {
    C::ColumnDefinition column;
    column.Width(GridLength{width ? width : 1, width ? GridUnitType::Pixel : GridUnitType::Star});
    row->grid.ColumnDefinitions().Append(column);
  }
  row->name.MaxLength(80);
  row->name.Text(preset.name);
  row->name.PlaceholderText(L"Preset name");
  row->start.MaxLength(2);
  row->start.Text(std::to_wstring(preset.start));
  row->stop.MaxLength(3);
  row->stop.Text(std::to_wstring(preset.stop));
  row->name.MinWidth(0);
  row->start.MinWidth(0);
  row->stop.MinWidth(0);
  row->grid.RenderTransform(row->translation);
  for (auto box : {row->name, row->start, row->stop}) {
    box.MinHeight(0);
    box.Height(32);
    box.Padding(Thickness{6, 4, 6, 4});
  }
  row->quick.MinWidth(0);
  row->quick.Width(28);
  row->quick.Padding(Thickness{0, 0, 0, 0});
  row->quick.IsChecked(preset.quick);
  row->quick.VerticalAlignment(VerticalAlignment::Center);
  row->quick.HorizontalAlignment(HorizontalAlignment::Center);
  Automation::AutomationProperties::SetName(row->name, L"Preset name");
  Automation::AutomationProperties::SetName(row->start, L"Start charging below percent");
  Automation::AutomationProperties::SetName(row->stop, L"Stop charging at percent");
  Automation::AutomationProperties::SetName(row->quick, L"Show in quick menu");
  C::Button remove;
  auto deleteLabel = Text(L"\u00D7", 22);
  deleteLabel.Foreground(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 235, 90, 90}));
  remove.Content(deleteLabel);
  remove.FontSize(22);
  remove.MinWidth(0);
  remove.Padding(Thickness{2, 0, 2, 0});
  remove.Foreground(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 235, 90, 90}));
  Automation::AutomationProperties::SetName(remove, L"Delete this preset");
  C::ToolTipService::SetToolTip(remove, winrt::box_value(L"Delete this entire preset row"));
  auto gripLabel = Text(L"\u2630", 20);
  gripLabel.VerticalAlignment(VerticalAlignment::Center);
  gripLabel.HorizontalAlignment(HorizontalAlignment::Center);
  row->grip.Child(gripLabel);
  row->grip.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
  row->grip.Height(32);
  Automation::AutomationProperties::SetName(row->grip, L"Drag to reorder preset");
  C::ToolTipService::SetToolTip(row->grip, winrt::box_value(L"Drag to reorder"));
  C::ToolTipService::SetToolTip(row->quick, winrt::box_value(L"Show in quick menu"));
  Cell(row->grid, remove, 0);
  Cell(row->grid, row->quick, 1);
  Cell(row->grid, row->name, 2);
  Cell(row->grid, row->start, 3);
  Cell(row->grid, row->stop, 4);
  C::StackPanel arrows;
  arrows.Children().Append(row->up);
  arrows.Children().Append(row->down);
  Cell(row->grid, arrows, 5);
  Cell(row->grid, row->grip, 6);
  row->up.Content(winrt::box_value(L"\u2191"));
  row->down.Content(winrt::box_value(L"\u2193"));
  for (auto button : {row->up, row->down}) {
    button.MinWidth(0);
    button.MinHeight(0);
    button.Height(16);
    button.Width(28);
    button.FontSize(12);
    button.Padding(Thickness{0, 0, 0, 0});
  }
  Automation::AutomationProperties::SetName(row->up, L"Move preset up");
  Automation::AutomationProperties::SetName(row->down, L"Move preset down");
  C::ToolTipService::SetToolTip(row->up, winrt::box_value(L"Move up"));
  C::ToolTipService::SetToolTip(row->down, winrt::box_value(L"Move down"));
  std::weak_ptr<PresetEditor> weak = editor;
  std::weak_ptr<PresetEditorRow> weakRow = row;
  auto move = [weak, weakRow](int delta) {
    if (auto editor = weak.lock())
      if (auto row = weakRow.lock()) {
        auto it = std::find(editor->rows.begin(), editor->rows.end(), row);
        if (it == editor->rows.end())
          return;
        int from = int(it - editor->rows.begin()), to = from + delta;
        if (to < 0 || to >= int(editor->rows.size()))
          return;
        MoveEditorRow(editor, row, size_t(delta > 0 ? from + 2 : from - 1));
      }
  };
  TrackEvent(
      row->up.Click(winrt::auto_revoke, SafeUi([move](auto const&, auto const&) { move(-1); })));
  TrackEvent(
      row->down.Click(winrt::auto_revoke, SafeUi([move](auto const&, auto const&) { move(1); })));
  // Pointer capture avoids OS drag/drop initialization inside Explorer's taskbar XAML island.
  TrackEvent(row->grip.PointerPressed(
      winrt::auto_revoke, SafeUi([weak, weakRow](auto const&, auto const& args) {
        if (auto editor = weak.lock())
          if (auto row = weakRow.lock()) {
            auto point = args.GetCurrentPoint(editor->list);
            if (!point.IsInContact())
              return;
            if (!row->grip.CapturePointer(args.Pointer()))
              return;
            editor->dragged = row;
            editor->pointerId = args.Pointer().PointerId();
            editor->dragStartY = point.Position().Y;
            editor->dragMoved = false;
            auto it = std::find(editor->rows.begin(), editor->rows.end(), row);
            editor->dragSlot = size_t(it - editor->rows.begin());
            args.Handled(true);
          }
      })));
  TrackEvent(row->grip.PointerMoved(
      winrt::auto_revoke, SafeUi([weak, weakRow](auto const&, auto const& args) {
        if (auto editor = weak.lock())
          if (auto row = weakRow.lock()) {
            if (editor->dragged.lock() != row || editor->pointerId != args.Pointer().PointerId())
              return;
            double y = args.GetCurrentPoint(editor->list).Position().Y;
            if (!editor->dragMoved && y < editor->dragStartY + 4 && y > editor->dragStartY - 4)
              return;
            editor->dragMoved = true;
            size_t slot = editor->rows.size();
            for (size_t i = 0; i < editor->rows.size(); i++) {
              auto& candidate = editor->rows[i];
              candidate->grid.Opacity(1);
              auto top = candidate->grid.TransformToVisual(editor->list).TransformPoint({0, 0}).Y -
                         candidate->translation.Y();
              if (slot == editor->rows.size() && y < top + candidate->grid.ActualHeight() / 2)
                slot = i;
            }
            editor->dragSlot = slot;
            row->grid.Opacity(.95);
            row->translation.Y(y - editor->dragStartY);
            C::Canvas::SetZIndex(row->grid, 10);
            row->grid.Background(
                Media::SolidColorBrush(row->grid.ActualTheme() == ElementTheme::Light
                                           ? winrt::Windows::UI::Color{255, 245, 245, 245}
                                           : winrt::Windows::UI::Color{255, 45, 45, 45}));
            auto edge = slot < editor->rows.size() ? editor->rows[slot] : editor->rows.back();
            double lineY = edge->grid.TransformToVisual(editor->list).TransformPoint({0, 0}).Y -
                           edge->translation.Y();
            if (slot == editor->rows.size())
              lineY += edge->grid.ActualHeight();
            editor->dropTranslation.Y(
                (std::max)(0.0, (std::min)(editor->scroll.ActualHeight() - 3,
                                           lineY - editor->scroll.VerticalOffset())));
            editor->dropLine.Visibility(Visibility::Visible);
            editor->message.Text(L"Move to position " +
                                 std::to_wstring((std::min)(slot + 1, editor->rows.size())));
            double viewY = args.GetCurrentPoint(editor->scroll).Position().Y;
            auto now = GetTickCount64();
            if (now - editor->lastDragScroll > 80 &&
                (viewY < 28 || viewY > editor->scroll.ActualHeight() - 28)) {
              editor->lastDragScroll = now;
              editor->scroll.ChangeView(
                  nullptr, editor->scroll.VerticalOffset() + (viewY < 28 ? -24.0 : 24.0), nullptr,
                  true);
            }
            args.Handled(true);
          }
      })));
  TrackEvent(row->grip.PointerReleased(
      winrt::auto_revoke, SafeUi([weak, weakRow](auto const&, auto const& args) {
        if (auto editor = weak.lock())
          if (auto row = weakRow.lock()) {
            if (editor->dragged.lock() != row || editor->pointerId != args.Pointer().PointerId())
              return;
            auto slot = editor->dragSlot;
            bool moved = editor->dragMoved;
            editor->dragged.reset();
            row->grip.ReleasePointerCapture(args.Pointer());
            row->grid.Opacity(1);
            row->translation.Y(0);
            C::Canvas::SetZIndex(row->grid, 0);
            row->grid.Background(nullptr);
            editor->dropLine.Visibility(Visibility::Collapsed);
            if (moved)
              MoveEditorRow(editor, row, slot);
            editor->message.Text(L"");
            args.Handled(true);
          }
      })));
  TrackEvent(row->grip.PointerCaptureLost(winrt::auto_revoke,
                                          SafeUi([weak, weakRow](auto const&, auto const&) {
                                            if (auto editor = weak.lock())
                                              if (auto row = weakRow.lock()) {
                                                if (editor->dragged.lock() == row)
                                                  editor->dragged.reset();
                                                row->grid.Opacity(1);
                                                row->translation.Y(0);
                                                C::Canvas::SetZIndex(row->grid, 0);
                                                row->grid.Background(nullptr);
                                                editor->dropLine.Visibility(Visibility::Collapsed);
                                                editor->message.Text(L"");
                                              }
                                          })));
  TrackEvent(remove.Click(
      winrt::auto_revoke, SafeUi([weak, weakRow](auto const&, auto const&) {
        if (auto editor = weak.lock())
          if (auto row = weakRow.lock()) {
            uint32_t index;
            if (editor->list.Children().IndexOf(row->grid, index))
              editor->list.Children().RemoveAt(index);
            std::erase(editor->rows, row);
            EditorCounts(editor);
            editor->message.Text(
                L"Preset deleted from this draft. Save to keep the change, or Cancel to undo.");
          }
      })));
  auto changed = [weak, weakRow](auto const&, auto const&) {
    if (auto editor = weak.lock())
      if (auto row = weakRow.lock()) {
        if (editor->changing)
          return;
        int count = 0;
        for (auto& item : editor->rows)
          if (item->quick.IsChecked().Value())
            count++;
        if (count > 5) {
          editor->changing = true;
          row->quick.IsChecked(false);
          editor->changing = false;
          editor->message.Text(L"Choose at most five quick favorites. Uncheck another first.");
        } else
          editor->message.Text(L"");
        EditorCounts(editor);
      }
  };
  TrackEvent(row->quick.Checked(winrt::auto_revoke, SafeUi(changed)));
  TrackEvent(row->quick.Unchecked(winrt::auto_revoke, SafeUi(changed)));
  editor->rows.push_back(row);
  editor->list.Children().Append(row->grid);
  EditorCounts(editor);
}
bool ReadPercent(winrt::hstring text, int& value) {
  value = 0;
  if (text.empty() || text.size() > 3)
    return false;
  for (auto c : text) {
    if (c < L'0' || c > L'9')
      return false;
    value = value * 10 + c - L'0';
  }
  return true;
}
void ShowPresetEditor(FrameworkElement target) {
  auto requested = addPresetDraft;
  addPresetDraft.valid = false;
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  presetEditor = std::make_shared<PresetEditor>();
  auto editor = presetEditor;
  editor->returnView = presetReturnView;
  std::weak_ptr<PresetEditor> weak = editor;
  flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(520);
  content.Spacing(10);
  Style presenter;
  presenter.TargetType(winrt::Windows::UI::Xaml::Interop::TypeName{
      L"Windows.UI.Xaml.Controls.FlyoutPresenter",
      winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata});
  presenter.Setters().Append(Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(600.0)));
  flyout.FlyoutPresenterStyle(presenter);
  content.Children().Append(Text(L"Manage presets", 22));
  content.Children().Append(
      Text(L"Check up to five favorites. Drag to reorder. Saving does not change charging.", 12));
  content.Children().Append(editor->count);
  C::Grid heading;
  heading.ColumnSpacing(6);
  heading.Margin(Thickness{0, 0, 26, 0});
  for (double width : {28.0, 40.0, 0.0, 64.0, 64.0, 28.0, 28.0}) {
    C::ColumnDefinition column;
    column.Width(GridLength{width ? width : 1, width ? GridUnitType::Pixel : GridUnitType::Star});
    heading.ColumnDefinitions().Append(column);
  }
  Cell(heading, Text(L"Quick", 11), 1);
  Cell(heading, Text(L"Name", 12), 2);
  Cell(heading, Text(L"Start %", 12), 3);
  Cell(heading, Text(L"Stop %", 12), 4);
  content.Children().Append(heading);
  editor->list.Spacing(6);
  editor->scroll.Content(editor->list);
  editor->scroll.Height(220);
  editor->scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Hidden);
  editor->scroll.VerticalScrollMode(C::ScrollMode::Enabled);
  editor->scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  editor->scrollbar.Orientation(C::Orientation::Vertical);
  editor->scrollbar.IndicatorMode(P::ScrollingIndicatorMode::MouseIndicator);
  editor->scrollbar.Width(18);
  Automation::AutomationProperties::SetName(editor->scrollbar, L"Scroll saved presets");
  C::Grid viewport;
  viewport.ColumnSpacing(8);
  C::ColumnDefinition listColumn;
  listColumn.Width(GridLength{1, GridUnitType::Star});
  viewport.ColumnDefinitions().Append(listColumn);
  C::ColumnDefinition barColumn;
  barColumn.Width(GridLength{18, GridUnitType::Pixel});
  viewport.ColumnDefinitions().Append(barColumn);
  Cell(viewport, editor->scroll, 0);
  Cell(viewport, editor->scrollbar, 1);
  editor->dropLine.Height(3);
  editor->dropLine.VerticalAlignment(VerticalAlignment::Top);
  editor->dropLine.IsHitTestVisible(false);
  editor->dropLine.Background(
      Media::SolidColorBrush(winrt::Windows::UI::Color{255, 100, 190, 240}));
  editor->dropLine.RenderTransform(editor->dropTranslation);
  editor->dropLine.Visibility(Visibility::Collapsed);
  Cell(viewport, editor->dropLine, 0);
  content.Children().Append(viewport);
  TrackEvent(editor->scrollbar.Scroll(
      winrt::auto_revoke, SafeUi([weak](auto const&, auto const& args) {
        if (auto editor = weak.lock())
          editor->scroll.ChangeView(nullptr, args.NewValue(), nullptr, true);
      })));
  TrackEvent(editor->scroll.LayoutUpdated(
      winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
        if (auto editor = weak.lock()) {
          double maximum = editor->scroll.ScrollableHeight();
          if (editor->scrollbar.Maximum() != maximum)
            editor->scrollbar.Maximum(maximum);
          if (editor->scrollbar.ViewportSize() != editor->scroll.ViewportHeight())
            editor->scrollbar.ViewportSize(editor->scroll.ViewportHeight());
          if (editor->scrollbar.Value() != editor->scroll.VerticalOffset())
            editor->scrollbar.Value(editor->scroll.VerticalOffset());
        }
      })));
  for (auto& preset : ReadPresets())
    AddEditorRow(editor, preset);
  editor->add.Content(winrt::box_value(L"+ Add preset"));
  TrackEvent(editor->add.Click(winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
                                 if (auto editor = weak.lock()) {
                                   AddEditorRow(editor, {L"New preset", 75, 80, false});
                                   if (!editor->rows.empty())
                                     editor->rows.back()->name.Focus(FocusState::Programmatic);
                                 }
                               })));
  C::StackPanel libraryActions;
  libraryActions.Orientation(C::Orientation::Horizontal);
  libraryActions.Spacing(8);
  libraryActions.Children().Append(editor->add);
  C::Button defaults;
  defaults.Content(winrt::box_value(L"Restore defaults"));
  TrackEvent(defaults.Click(
      winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
        if (auto editor = weak.lock()) {
          editor->list.Children().Clear();
          editor->rows.clear();
          for (auto& preset : DefaultPresets())
            AddEditorRow(editor, preset);
          editor->message.Text(L"Default presets restored. Save to keep them, or Cancel to undo.");
        }
      })));
  libraryActions.Children().Append(defaults);
  content.Children().Append(libraryActions);
  EditorCounts(editor);
  editor->message.Text(presetNotice);
  bool addedCurrent = false;
  if (requested.valid && BatteryControls::ValidRange(requested.start, requested.stop)) {
    if (editor->rows.size() < maxPresets) {
      int favorites = 0;
      for (auto& row : editor->rows)
        if (row->quick.IsChecked().Value())
          favorites++;
      AddEditorRow(editor, {L"New preset", requested.start, requested.stop,
                            favorites < int(maxQuickPresets)});
      addedCurrent = true;
      editor->message.Text(L"Name your new preset, then Save presets. Charging is unchanged.");
    } else
      editor->message.Text(
          L"All 20 preset slots are in use. Remove a preset and save, then add your current "
          L"settings.");
  }
  editor->message.TextWrapping(TextWrapping::Wrap);
  content.Children().Append(editor->message);
  C::StackPanel actions;
  actions.Orientation(C::Orientation::Horizontal);
  actions.Spacing(8);
  C::Button save;
  save.Content(winrt::box_value(L"Save presets"));
  TrackEvent(save.Click(
      winrt::auto_revoke, SafeUi([weak, target](auto const&, auto const&) {
        if (auto editor = weak.lock()) {
          std::vector<Preset> rows;
          size_t index = 0;
          for (auto& row : editor->rows) {
            index++;
            std::wstring name = NormalizeName(std::wstring(row->name.Text()));
            int start, stop;
            if (!ValidPresetName(name)) {
              editor->message.Text(
                  L"Row " + std::to_wstring(index) +
                  L": enter a name of 1 to 40 characters, without leading or trailing spaces.");
              row->name.Focus(FocusState::Programmatic);
              return;
            }
            if (!ReadPercent(row->start.Text(), start) || !ReadPercent(row->stop.Text(), stop) ||
                !BatteryControls::ValidRange(start, stop)) {
              editor->message.Text(
                  L"Row " + std::to_wstring(index) +
                  L": use 5 to 99 for start and 6 to 100 for stop. Start must be lower than stop.");
              return;
            }
            rows.push_back({name, start, stop, row->quick.IsChecked().Value()});
          }
          if (!SavePresets(rows)) {
            editor->message.Text(
                L"Could not save presets. Check that at most five are favorites; otherwise storage "
                L"may be unavailable. Your draft is still open.");
            return;
          }
          BatteryControls::Status(L"Presets saved. Charging limits are unchanged.");
          QueuePanel(target, editor->returnView);
        }
      })));
  C::Button cancel;
  cancel.Content(winrt::box_value(L"Cancel"));
  TrackEvent(cancel.Click(winrt::auto_revoke, SafeUi([target, weak](auto const&, auto const&) {
                            if (auto editor = weak.lock())
                              QueuePanel(target, editor->returnView);
                          })));
  actions.Children().Append(save);
  actions.Children().Append(cancel);
  content.Children().Append(actions);
  C::ScrollViewer scroll;
  scroll.Content(content);
  scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar);
  if (GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi)) {
    double scale = target.XamlRoot().RasterizationScale();
    scroll.MaxHeight((std::max)(120.0, (mi.rcWork.bottom - mi.rcWork.top) / scale - 64));
    scroll.MaxWidth((std::max)(200.0, (mi.rcWork.right - mi.rcWork.left) / scale - 64));
  }
  flyout.Content(scroll);
  flyout.Placement(P::FlyoutPlacementMode::TopEdgeAlignedRight);
  TrackEvent(flyout.Closed(winrt::auto_revoke, SafeUi([weak, weakTarget = winrt::make_weak(target)](
                                                          auto const&, auto const&) {
                             if (auto editor = weak.lock())
                               if (presetEditor == editor)
                                 if (auto target = weakTarget.get())
                                   QueuePanel(target, editor->returnView);
                           })));
  TrackEvent(flyout.Opened(
      winrt::auto_revoke, SafeUi([weak, addedCurrent](auto const&, auto const&) {
        if (addedCurrent)
          if (auto editor = weak.lock())
            if (!editor->rows.empty()) {
              editor->list.UpdateLayout();
              editor->scroll.ChangeView(nullptr, editor->scroll.ScrollableHeight(), nullptr, true);
              editor->rows.back()->name.Focus(FocusState::Programmatic);
              editor->rows.back()->name.SelectAll();
            }
      })));
  flyout.ShowAt(target);
}
void ShowPanel(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  auto initial = BatteryControls::ReadSnapshot().state;
  panel = std::make_shared<Panel>();
  auto p = panel;
  std::weak_ptr<Panel> weak = p;
  flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(340);
  content.Spacing(14);
  auto title = Text(L"Battery protection", 22);
  title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
  content.Children().Append(title);
  content.Children().Append(Text(L"Choose when charging starts and stops."));
  p->enabled.Header(winrt::box_value(L"Limit charging"));
  p->enabled.IsOn(initial.enabled || BatteryControls::Active());
  content.Children().Append(p->enabled);
  auto library = ReadPresets();
  std::erase_if(library, [](const Preset& preset) { return !preset.quick; });
  auto choose = [weak](const Preset& preset) {
    if (auto p = weak.lock()) {
      if (p->updatingPresets)
        return;
      p->adjusting = true;
      p->start.Value(preset.start);
      p->stop.Value(preset.stop);
      p->adjusting = false;
      p->enabled.IsOn(true);
      p->startLabel.Text(L"Start below " + std::to_wstring(preset.start) + L"%");
      p->stopLabel.Text(L"Stop at " + std::to_wstring(preset.stop) + L"%");
      p->UpdatePresets();
    }
  };
  auto heading = PresetHeading();
  heading.Margin(Thickness{10, 0, 10, 0});
  if (!library.empty())
    content.Children().Append(heading);
  C::StackPanel choices;
  choices.Spacing(4);
  for (const auto& preset : library) {
    P::ToggleButton button;
    // Selection is a native checked state, including hover and pressed states.
    // Set local theme resources before template creation; do not repaint on pointer events.
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
    if (!(contrast.dwFlags & HCF_HIGHCONTRASTON)) {
      auto blue = Media::SolidColorBrush(winrt::Windows::UI::Color{255, 0, 95, 184});
      auto white = Media::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 255, 255});
      for (auto key :
           {L"ToggleButtonBackgroundChecked", L"ToggleButtonBackgroundCheckedPointerOver",
            L"ToggleButtonBackgroundCheckedPressed"})
        button.Resources().Insert(winrt::box_value(key), blue);
      for (auto key :
           {L"ToggleButtonForegroundChecked", L"ToggleButtonForegroundCheckedPointerOver",
            L"ToggleButtonForegroundCheckedPressed"})
        button.Resources().Insert(winrt::box_value(key), white);
    }
    button.Content(PresetDisplay(preset));
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 6, 10, 6});
    Automation::AutomationProperties::SetName(button, PresetLabel(preset));
    TrackEvent(button.Click(winrt::auto_revoke, SafeUi([choose, preset](auto const&, auto const&) {
                              choose(preset);
                            })));
    p->choices.emplace_back(button, preset.start, preset.stop);
    choices.Children().Append(button);
  }
  C::ScrollViewer presetList;
  presetList.Content(choices);
  presetList.MaxHeight(164);
  presetList.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  presetList.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  content.Children().Append(presetList);
  C::Button manage;
  manage.Content(winrt::box_value(L"Manage presets..."));
  TrackEvent(manage.Click(winrt::auto_revoke, SafeUi([target, weak](auto const&, auto const&) {
                            if (auto p = weak.lock())
                              returningDraft = {true, p->enabled.IsOn(), int(p->start.Value()),
                                                int(p->stop.Value())};
                            presetReturnView = 0;
                            QueuePanel(target, 2);
                          })));
  content.Children().Append(manage);
  if (library.empty())
    content.Children().Append(Text(
        L"No quick presets selected. Choose favorites in Manage presets, or use the sliders below.",
        12));
  p->start.Minimum(5);
  p->start.Maximum(99);
  p->start.StepFrequency(1);
  p->start.SmallChange(1);
  p->start.LargeChange(5);
  p->stop.Minimum(6);
  p->stop.Maximum(100);
  p->stop.StepFrequency(1);
  p->stop.SmallChange(1);
  p->stop.LargeChange(5);
  p->start.Value(BatteryControls::ValidRange(initial.start, initial.stop) ? initial.start : 75);
  p->stop.Value(BatteryControls::ValidRange(initial.start, initial.stop) ? initial.stop : 80);
  p->startLabel.Text(L"Start below " + std::to_wstring(int(p->start.Value())) + L"%");
  p->stopLabel.Text(L"Stop at " + std::to_wstring(int(p->stop.Value())) + L"%");
  Automation::AutomationProperties::SetName(p->start, L"Start charging below percentage");
  Automation::AutomationProperties::SetName(p->stop, L"Stop charging at percentage");
  content.Children().Append(p->startLabel);
  content.Children().Append(p->start);
  content.Children().Append(p->stopLabel);
  content.Children().Append(p->stop);
  TrackEvent(p->start.ValueChanged(
      winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
        if (auto p = weak.lock()) {
          if (p->adjusting)
            return;
          p->adjusting = true;
          if (p->start.Value() >= p->stop.Value())
            p->stop.Value(p->start.Value() + 1);
          p->startLabel.Text(L"Start below " + std::to_wstring(int(p->start.Value())) + L"%");
          p->stopLabel.Text(L"Stop at " + std::to_wstring(int(p->stop.Value())) + L"%");
          p->adjusting = false;
          p->UpdatePresets();
        }
      })));
  TrackEvent(p->stop.ValueChanged(
      winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
        if (auto p = weak.lock()) {
          if (p->adjusting)
            return;
          p->adjusting = true;
          if (p->stop.Value() <= p->start.Value())
            p->start.Value(p->stop.Value() - 1);
          p->startLabel.Text(L"Start below " + std::to_wstring(int(p->start.Value())) + L"%");
          p->stopLabel.Text(L"Stop at " + std::to_wstring(int(p->stop.Value())) + L"%");
          p->adjusting = false;
          p->UpdatePresets();
        }
      })));
  TrackEvent(p->enabled.Toggled(winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
                                  if (auto p = weak.lock())
                                    p->UpdatePresets();
                                })));
  if (returningDraft.valid) {
    p->adjusting = true;
    p->start.Value(returningDraft.start);
    p->stop.Value(returningDraft.stop);
    p->enabled.IsOn(returningDraft.enabled);
    p->adjusting = false;
    p->startLabel.Text(L"Start below " + std::to_wstring(returningDraft.start) + L"%");
    p->stopLabel.Text(L"Stop at " + std::to_wstring(returningDraft.stop) + L"%");
    returningDraft.valid = false;
  }
  p->UpdatePresets();
  C::Button saveCurrent;
  saveCurrent.Content(winrt::box_value(L"+ Save as preset..."));
  saveCurrent.IsEnabled(ReadPresets().size() < maxPresets);
  C::ToolTipService::SetToolTip(
      saveCurrent,
      winrt::box_value(
          saveCurrent.IsEnabled()
              ? L"Name and save the current slider values as a preset"
              : L"All 20 preset slots are in use. Remove one in Manage presets first."));
  TrackEvent(saveCurrent.Click(winrt::auto_revoke, SafeUi([target, weak](auto const&, auto const&) {
                                 if (auto p = weak.lock()) {
                                   returningDraft = {true, p->enabled.IsOn(), int(p->start.Value()),
                                                     int(p->stop.Value())};
                                   addPresetDraft = returningDraft;
                                   presetReturnView = 0;
                                   QueuePanel(target, 2);
                                 }
                               })));
  content.Children().Append(saveCurrent);
  auto hint = Text(
      L"Above the limit? Charging pauses and the level may fall slowly while plugged in. Unplug to "
      L"reach your selected range sooner.",
      12);
  hint.Opacity(.75);
  content.Children().Append(hint);
  if (BatteryControls::Active()) {
    auto notice = Text(L"A full-charge override is active. Applying changes will end it.", 12);
    content.Children().Append(notice);
  }
  C::StackPanel actions;
  actions.Orientation(C::Orientation::Horizontal);
  actions.Spacing(8);
  C::Button apply;
  apply.Content(winrt::box_value(L"Apply changes"));
  TrackEvent(apply.Click(winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
                           if (auto p = weak.lock()) {
                             Send(p->enabled.IsOn() ? 5 : 6,
                                  MAKELONG(int(p->start.Value()), int(p->stop.Value())));
                             flyout.Hide();
                           }
                         })));
  C::Button cancel;
  cancel.Content(winrt::box_value(L"Cancel"));
  TrackEvent(
      cancel.Click(winrt::auto_revoke, SafeUi([](auto const&, auto const&) { flyout.Hide(); })));
  actions.Children().Append(apply);
  actions.Children().Append(cancel);
  content.Children().Append(actions);
  C::ScrollViewer scroll;
  scroll.Content(content);
  scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar);
  if (GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi)) {
    double scale = target.XamlRoot().RasterizationScale();
    scroll.MaxHeight((std::max)(120.0, (mi.rcWork.bottom - mi.rcWork.top) / scale - 64));
    content.Width((std::min)(340.0, (mi.rcWork.right - mi.rcWork.left) / scale - 64));
  }
  flyout.Content(scroll);
  flyout.Placement(P::FlyoutPlacementMode::Top);
  TrackEvent(flyout.Opened(winrt::auto_revoke, SafeUi([weakTarget = winrt::make_weak(target)](
                                                          auto const& sender, auto const&) {
                             try {
                               if (auto target = weakTarget.get())
                                 PositionPanel(target, sender.template as<C::Flyout>());
                             } catch (...) {
                               BatteryControls::Status(L"Unable to display battery controls.");
                             }
                           })));
  flyout.ShowAt(target);
}
std::wstring MachineValue(PCWSTR name) {
  wchar_t text[256]{};
  DWORD bytes = sizeof(text);
  if (RegGetValueW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", name, RRF_RT_REG_SZ,
                   nullptr, text, &bytes) != ERROR_SUCCESS)
    return L"Unknown";
  return text;
}
// BEGIN TESTABLE MODEL LINK
std::wstring LenovoMachineType(bool lenovo, const std::wstring& model) {
  if (!lenovo || model.size() < 4 || model.size() > 20)
    return {};
  // SMBIOS product name is an MT or MTM, never a URL or serial number.
  for (auto c : model)
    if (!((c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')))
      return {};
  auto type = model.substr(0, 4);
  bool digit = false;
  for (auto& c : type) {
    if (c >= L'0' && c <= L'9')
      digit = true;
    if (c >= L'A' && c <= L'Z')
      c += L'a' - L'A';
  }
  return digit ? type : std::wstring{};
}
std::wstring LenovoDriverUrl(const std::wstring& type) {
  if (type.size() != 4 || LenovoMachineType(true, type) != type)
    return L"https://pcsupport.lenovo.com/";
  return L"https://pcsupport.lenovo.com/us/en/products/" + type + L"/downloads/driver-list";
}
// END TESTABLE MODEL LINK
void ShowSetup(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(340);
  content.Spacing(12);
  auto title = Text(L"Setup and diagnostics", 22);
  content.Children().Append(title);
  auto recent = BatteryControls::RecentStatus();
  if (!recent.empty())
    content.Children().Append(Text(recent, 12));
  auto maker = MachineValue(L"SystemManufacturer"), model = MachineValue(L"SystemProductName"),
       family = MachineValue(L"SystemFamily");
  content.Children().Append(Text(maker + L" " + family + L"\nModel: " + model));
  bool lenovo = _wcsicmp(maker.c_str(), L"LENOVO") == 0;
  if (!lenovo)
    content.Children().Append(
        Text(L"A Lenovo manufacturer could not be confirmed. This mod requires compatible Lenovo "
             L"hardware."));
  content.Children().Append(
      Text(L"Default utility location: " + BatteryControls::DefaultUtility(), 12));
  bool ready =
      BatteryControls::UiInt(L"cacheValid", 0) && BatteryControls::UiInt(L"utilityCheck", 0) == 4;
  auto setupFeedback = Text(ready ? L"Lenovo utility is ready. No setup is needed."
                                  : L"Set up the Lenovo utility below.");
  content.Children().Append(setupFeedback);
  C::Button setup;
  setup.Content(
      winrt::box_value(ready ? L"Run setup again" : L"Download and set up Lenovo utility"));
  TrackEvent(setup.Click(
      winrt::auto_revoke,
      SafeUi([setupFeedback, weakSetup = winrt::make_weak(setup)](auto const&, auto const&) {
        auto setup = weakSetup.get();
        if (!setup)
          return;
        setupFeedback.Text(
            L"Windows may ask you to approve Windows Command Processor, published by Microsoft, to "
            L"install the Lenovo utility.");
        setup.IsEnabled(false);
        BatteryControls::SetupStatus(L"Setup requested...");
        Send(7, 1);
        if (statusTimer)
          statusTimer.Stop();
        statusTimer = DispatcherTimer();
        statusTimer.Interval(std::chrono::milliseconds(500));
        TrackEvent(statusTimer.Tick(
            winrt::auto_revoke, SafeUi([setupFeedback, weakSetup](auto const&, auto const&) {
              auto setup = weakSetup.get();
              if (!setup)
                return;
              auto result = BatteryControls::SetupText();
              setupFeedback.Text(result);
              bool busy = BatteryControls::setupRunning.load() || result == L"Starting setup..." ||
                          result == L"Setup requested...";
              if (!busy) {
                setup.IsEnabled(true);
                if (statusTimer)
                  statusTimer.Stop();
              }
            })));
        statusTimer.Start();
      })));
  content.Children().Append(setup);
  TrackEvent(flyout.Closed(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                             if (statusTimer)
                               statusTimer.Stop();
                           })));
  content.Children().Append(Text(
      L"Downloads the reviewed utility directly from Lenovo, verifies it, and installs it in "
      L"Program Files. Windows asks for administrator approval. Leave the utility path blank in "
      L"Windhawk Settings to use this location.",
      12));
  int utility = BatteryControls::UiInt(L"utilityCheck", 0);
  content.Children().Append(Text(
      utility == 4 ? L"ChargeThreshold utility: verified."
      : utility == 1
          ? L"Action needed: set the full ChargeThreshold.exe path in this mod's Windhawk Settings."
      : utility == 2 ? L"Action needed: the configured utility file is missing or inaccessible. "
                       L"Download it and update its path in Windhawk Settings."
      : utility == 3 ? L"Action needed: the utility is missing, unverified, or in a writable "
                       L"location. Use Download and set up above, and leave the path setting blank."
                     : L"Utility check has not completed. Reopen this panel shortly."));
  SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
  std::wstring service = L"Lenovo PM Service: status could not be checked.";
  if (manager) {
    SC_HANDLE handle = OpenServiceW(manager, L"IBMPMSVC", SERVICE_QUERY_STATUS);
    if (handle) {
      SERVICE_STATUS state{};
      if (QueryServiceStatus(handle, &state))
        service =
            state.dwCurrentState == SERVICE_RUNNING
                ? L"Lenovo PM Service: running."
                : L"Lenovo PM Service: installed but not running. Try restarting Windows, then "
                  L"check Lenovo updates.";
      CloseServiceHandle(handle);
    } else if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
      service =
          L"Lenovo PM Service was not found. Check the power-management packages for your model.";
    CloseServiceHandle(manager);
  }
  content.Children().Append(Text(service));
  bool valid = BatteryControls::UiInt(L"cacheValid", 0);
  content.Children().Append(Text(
      valid ? L"Battery communication: working."
            : L"Battery communication: unavailable. This can mean missing drivers, a stopped "
              L"component, an unsupported battery, or a communication error. Service detection "
              L"alone does not prove compatibility."));
  auto link = [&](PCWSTR label, PCWSTR url) {
    C::HyperlinkButton b;
    b.Content(winrt::box_value(label));
    b.NavigateUri(winrt::Windows::Foundation::Uri(url));
    content.Children().Append(b);
  };
  link(L"Download Lenovo ChargeThreshold.exe",
       L"https://download.lenovo.com/pccbbs/thinkvantage_en/metroapps/Vantage/ChargeThreshold/"
       L"ChargeThreshold.exe");
  auto machineType = LenovoMachineType(lenovo, model);
  if (!machineType.empty()) {
    auto label =
        L"Drivers for " + (family == L"Unknown" ? model : family) + L" (" + machineType + L")";
    auto url = LenovoDriverUrl(machineType);
    link(label.c_str(), url.c_str());
    content.Children().Append(
        Text(L"Lenovo selects the driver page from your machine type when you open this link. Only "
             L"the four-character machine type is included, not your serial number.",
             12));
  } else
    content.Children().Append(Text(
        L"The machine type could not be identified. Use Lenovo Support below to select your model.",
        12));
  link(L"Find your model on Lenovo Support", L"https://pcsupport.lenovo.com/");
  content.Children().Append(Text(
      L"Check supported models before installing a driver. If Lenovo cannot resolve the link, "
      L"select your model on its support site. The setup button installs only the utility after "
      L"administrator approval (Windows Command Processor, Microsoft); drivers must be "
      L"installed separately. Links open only when clicked. No serial number is read or sent.",
      12));
  C::ScrollViewer scroll;
  scroll.Content(content);
  scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar);
  double available = 600;
  if (GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi))
    available = (std::max)(120.0, (mi.rcWork.bottom - mi.rcWork.top) /
                                          target.XamlRoot().RasterizationScale() -
                                      64);
  scroll.MaxHeight(available);
  flyout.Content(scroll);
  flyout.Placement(P::FlyoutPlacementMode::Top);
  TrackEvent(flyout.Opened(winrt::auto_revoke, SafeUi([weak = winrt::make_weak(target)](
                                                          auto const& sender, auto const&) {
                             try {
                               if (auto target = weak.get())
                                 PositionPanel(target, sender.template as<C::Flyout>());
                             } catch (...) {
                               BatteryControls::Status(L"Unable to display battery controls.");
                             }
                           })));
  flyout.ShowAt(target);
}
void QueuePanel(FrameworkElement target, int view) {
  if (unloading)
    return;
  Close();
  pendingMenu = DispatcherTimer();
  pendingMenu.Interval(std::chrono::milliseconds(50));
  TrackEvent(pendingMenu.Tick(
      winrt::auto_revoke, SafeUi([weak = winrt::make_weak(target), view](auto const&, auto const&) {
        if (auto target = weak.get()) {
          if (view == 3)
            ShowQuickMenu(target);
          else if (view == 2)
            ShowPresetEditor(target);
          else if (view == 1)
            ShowSetup(target);
          else
            ShowPanel(target);
        } else
          Close();
      })));
  pendingMenu.Start();
}
void MarkApplied(C::MenuFlyoutItem item, const Preset& preset) {
  if (MatchesPreset(preset, BatteryControls::UiInt(L"cacheValid", 0),
                    BatteryControls::UiInt(L"cacheEnabled", 0), BatteryControls::Active(),
                    BatteryControls::UiInt(L"cacheStart", 0),
                    BatteryControls::UiInt(L"cacheStop", 0))) {
    C::FontIcon icon;
    icon.Glyph(L"\uE73E");
    item.Icon(icon);
  }
}
void ShowMenu(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  menu = C::MenuFlyout();
  SYSTEM_POWER_STATUS power{};
  BYTE ac = GetSystemPowerStatus(&power) ? power.ACLineStatus : 255;
  const wchar_t* fullLabel = ac == 0   ? L"Charge to full when plugged in"
                             : ac == 1 ? L"Charge to full now"
                                       : L"Charge to full";
  bool valid = BatteryControls::UiInt(L"cacheValid", 0),
       enabled = BatteryControls::UiInt(L"cacheEnabled", 0), active = BatteryControls::Active();
  auto add = [&](std::wstring text, wchar_t glyph, bool enabled, auto callback) {
    C::MenuFlyoutItem item;
    item.Text(text);
    item.IsEnabled(enabled);
    C::FontIcon icon;
    icon.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(std::wstring(1, glyph));
    item.Icon(icon);
    TrackEvent(item.Click(winrt::auto_revoke, SafeUi(callback)));
    menu.Items().Append(item);
    return item;
  };
  std::wstring status =
      active    ? L"Charge to 100% (temporary)"
      : !valid  ? L"Charging status unavailable"
      : enabled ? L"Charge: " + std::to_wstring(BatteryControls::UiInt(L"cacheStart", 75)) +
                      L" to " + std::to_wstring(BatteryControls::UiInt(L"cacheStop", 80)) + L"%"
                : L"Charge to 100% (limits off)";
  C::MenuFlyoutItem header;
  header.Text(status);
  header.FontSize(12);
  // Keep this clickable summary visually distinct from commands. Leave contrast themes native.
  HIGHCONTRASTW contrast{sizeof(contrast)};
  if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
      !(contrast.dwFlags & HCF_HIGHCONTRASTON)) {
    auto muted = Media::SolidColorBrush(target.ActualTheme() == ElementTheme::Light
                                            ? winrt::Windows::UI::Color{255, 96, 96, 96}
                                            : winrt::Windows::UI::Color{255, 180, 180, 180});
    header.Foreground(muted);
    for (auto key : {L"MenuFlyoutItemForeground", L"MenuFlyoutItemForegroundPointerOver",
                     L"MenuFlyoutItemForegroundPressed"})
      header.Resources().Insert(winrt::box_value(key), muted);
  }
  C::FontIcon gear;
  gear.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
  gear.Glyph(L"\uE713");
  header.Icon(gear);
  if (!(contrast.dwFlags & HCF_HIGHCONTRASTON))
    gear.Foreground(Media::SolidColorBrush(target.ActualTheme() == ElementTheme::Light
                                               ? winrt::Windows::UI::Color{255, 32, 32, 32}
                                               : winrt::Windows::UI::Color{255, 240, 240, 240}));
  C::ToolTipService::SetToolTip(header, winrt::box_value(L"Setup and diagnostics"));
  Automation::AutomationProperties::SetName(header, status + L". Open setup and diagnostics");
  TrackEvent(header.Click(winrt::auto_revoke,
                          SafeUi([target](auto const&, auto const&) { QueuePanel(target, 1); })));
  menu.Items().Append(header);
  menu.Items().Append(C::MenuFlyoutSeparator());
  C::MenuFlyoutItem full;
  full.Text(active ? L"Cancel charge to full" : fullLabel);
  full.IsEnabled(active || (valid && enabled));
  C::FontIcon fullIcon;
  fullIcon.Glyph(active ? L"\uE711" : L"\uE945");
  full.Icon(fullIcon);
  TrackEvent(full.Click(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                          Send(BatteryControls::Active() ? 2 : 1);
                        })));
  menu.Items().Append(full);
  std::vector<std::pair<C::MenuFlyoutItem, Preset>> quickItems;
  C::MenuFlyoutSubItem quick;
  quick.Text(L"Apply charging preset");
  C::FontIcon presetIcon;
  presetIcon.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
  presetIcon.Glyph(L"\uE9D5");  // CheckList: choose from saved charging presets.
  quick.Icon(presetIcon);
  int shown = 0;
  for (const auto& preset : ReadPresets()) {
    if (!preset.quick || shown >= 5)
      continue;
    shown++;
    C::MenuFlyoutItem item;
    item.Text(PresetLabel(preset));
    item.IsEnabled(valid);
    MarkApplied(item, preset);
    TrackEvent(item.Click(winrt::auto_revoke, SafeUi([preset](auto const&, auto const&) {
                            Send(5, MAKELONG(preset.start, preset.stop));
                          })));
    quick.Items().Append(item);
    quickItems.emplace_back(item, preset);
  }
  quick.Items().Append(C::MenuFlyoutSeparator());
  C::MenuFlyoutItem managePresets;
  managePresets.Text(L"Manage presets...");
  TrackEvent(managePresets.Click(winrt::auto_revoke, SafeUi([target](auto const&, auto const&) {
                                   returningDraft.valid = false;
                                   presetReturnView = 3;
                                   QueuePanel(target, 2);
                                 })));
  quick.Items().Append(managePresets);
  menu.Items().Append(quick);
  menu.Items().Append(C::MenuFlyoutSeparator());
  auto threshold = add(L"Set charge threshold...", L'\uE9E9', valid,
                       [target](auto const&, auto const&) { QueuePanel(target, false); });
  add(L"Windows battery settings", L'\uE8A5', true, [](auto const&, auto const&) { Send(4); });
  auto update = [header, full, threshold, quickItems, fullLabel = std::wstring(fullLabel)]() {
    auto snap = BatteryControls::ReadSnapshot();
    bool waiting = BatteryControls::refreshing, valid = snap.state.valid && !waiting,
         active = BatteryControls::Active();
    std::wstring text = waiting              ? L"Checking charging..."
                        : active             ? L"Charge to 100% (temporary)"
                        : !valid             ? L"Charging status unavailable"
                        : snap.state.enabled ? L"Charge: " + std::to_wstring(snap.state.start) +
                                                   L" to " + std::to_wstring(snap.state.stop) + L"%"
                                             : L"Charge to 100% (limits off)";
    header.Text(text);
    Automation::AutomationProperties::SetName(header, text + L". Open setup and diagnostics");
    full.Text(active ? L"Cancel charge to full" : fullLabel);
    if (auto icon = full.Icon().try_as<C::FontIcon>())
      icon.Glyph(active ? L"\uE711" : L"\uE945");
    full.IsEnabled(active || (valid && snap.state.enabled));
    threshold.IsEnabled(valid);
    for (auto& [item, preset] : quickItems) {
      item.IsEnabled(valid);
      item.Icon(nullptr);
      if (valid)
        MarkApplied(item, preset);
    }
  };
  update();
  statusTimer = DispatcherTimer();
  statusTimer.Interval(std::chrono::milliseconds(200));
  auto stamp = std::make_shared<ULONGLONG>(~0ULL);
  TrackEvent(statusTimer.Tick(winrt::auto_revoke, SafeUi([update, stamp](auto const&, auto const&) {
                                auto snap = BatteryControls::ReadSnapshot();
                                auto current = snap.revision * 4 +
                                               (BatteryControls::refreshing ? 2 : 0) +
                                               (BatteryControls::Active() ? 1 : 0);
                                if (current != *stamp) {
                                  *stamp = current;
                                  update();
                                }
                              })));
  statusTimer.Start();
  TrackEvent(menu.Closed(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                           if (statusTimer)
                             statusTimer.Stop();
                         })));
  menu.Placement(P::FlyoutPlacementMode::Top);
  menu.ShowAt(target);
}
void ShowQuickMenu(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  menu = C::MenuFlyout();
  C::MenuFlyoutItem heading;
  heading.Text(L"Apply charging preset");
  heading.IsEnabled(false);
  menu.Items().Append(heading);
  std::vector<std::pair<C::MenuFlyoutItem, Preset>> quickItems;
  bool valid = BatteryControls::UiInt(L"cacheValid", 0);
  int shown = 0;
  for (const auto& preset : ReadPresets()) {
    if (!preset.quick || shown++ >= 5)
      continue;
    C::MenuFlyoutItem item;
    item.Text(PresetLabel(preset));
    item.IsEnabled(valid);
    MarkApplied(item, preset);
    TrackEvent(item.Click(winrt::auto_revoke, SafeUi([preset](auto const&, auto const&) {
                            Send(5, MAKELONG(preset.start, preset.stop));
                          })));
    menu.Items().Append(item);
    quickItems.emplace_back(item, preset);
  }
  menu.Items().Append(C::MenuFlyoutSeparator());
  C::MenuFlyoutItem manage;
  manage.Text(L"Manage presets...");
  TrackEvent(manage.Click(winrt::auto_revoke, SafeUi([target](auto const&, auto const&) {
                            returningDraft.valid = false;
                            presetReturnView = 3;
                            QueuePanel(target, 2);
                          })));
  menu.Items().Append(manage);
  statusTimer = DispatcherTimer();
  statusTimer.Interval(std::chrono::milliseconds(200));
  auto stamp = std::make_shared<ULONGLONG>(~0ULL);
  TrackEvent(
      statusTimer.Tick(winrt::auto_revoke, SafeUi([quickItems, stamp](auto const&, auto const&) {
                         auto snap = BatteryControls::ReadSnapshot();
                         auto current = snap.revision * 4 + (BatteryControls::refreshing ? 2 : 0) +
                                        (BatteryControls::Active() ? 1 : 0);
                         if (current == *stamp)
                           return;
                         *stamp = current;
                         for (auto& [item, preset] : quickItems) {
                           item.IsEnabled(BatteryControls::UiInt(L"cacheValid", 0));
                           item.Icon(nullptr);
                           MarkApplied(item, preset);
                         }
                       })));
  statusTimer.Start();
  TrackEvent(menu.Closed(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                           if (statusTimer)
                             statusTimer.Stop();
                         })));
  menu.Placement(P::FlyoutPlacementMode::Top);
  menu.ShowAt(target);
}
void QueueMenu(FrameworkElement target) {
  if (unloading)
    return;
  BatteryControls::Submit(0);
  Close();
  pendingMenu = DispatcherTimer();
  pendingMenu.Interval(std::chrono::milliseconds(50));
  auto weak = winrt::make_weak(target);
  TrackEvent(pendingMenu.Tick(winrt::auto_revoke, SafeUi([weak](auto const&, auto const&) {
                                try {
                                  if (auto target = weak.get())
                                    ShowMenu(target);
                                  else
                                    Close();
                                } catch (winrt::hresult_error const& e) {
                                  Engine::SetString(L"uiError", e.message().c_str());
                                  Close();
                                }
                              })));
  pendingMenu.Start();
}
}  // namespace ModernUI

struct NativeBinding {
  winrt::weak_ref<UIElement> element;
  winrt::Windows::Foundation::IInspectable handler;
};
std::vector<NativeBinding> bindings;
std::atomic<HWND> taskbar = nullptr;
void Scan(DependencyObject node, int depth = 0) {
  if (unloading || !node || depth > 128)
    return;
  auto element = node.try_as<FrameworkElement>();
  if (element && winrt::get_class_name(element) == L"SystemTray.BatteryIconContent") {
    auto ancestor = Media::VisualTreeHelper::GetParent(element);
    for (int depth = 0; ancestor && depth < 6; depth++) {
      if (winrt::get_class_name(ancestor) == L"SystemTray.IconView") {
        element = ancestor.as<FrameworkElement>();
        break;
      }
      ancestor = Media::VisualTreeHelper::GetParent(ancestor);
    }
    for (auto& binding : bindings)
      if (binding.element.get() == element)
        return;
    Input::RightTappedEventHandler handler = [](auto const& sender,
                                                Input::RightTappedRoutedEventArgs const& args) {
      if (unloading)
        return;
      args.Handled(true);
      try {
        ModernUI::QueueMenu(sender.template as<FrameworkElement>());
      } catch (...) {
        BatteryControls::Status(L"Unable to display battery controls.");
      }
    };
    auto boxed = winrt::box_value(handler);
    element.AddHandler(UIElement::RightTappedEvent(), boxed, true);
    bindings.push_back({winrt::make_weak(element.as<UIElement>()), boxed});
    Engine::Log(L"Native battery context menu attached");
    Engine::SetInt(L"nativeAttached", 1);
    Engine::SetString(L"attachedVersion", L"0.6.6");
    return;
  }
  for (int i = 0; i < Media::VisualTreeHelper::GetChildrenCount(node); i++)
    Scan(Media::VisualTreeHelper::GetChild(node, i), depth + 1);
}
void AttachNativeMenu() {
  if (unloading || !symbolsReady)
    return;
  HWND found = FindWindowW(L"Shell_TrayWnd", nullptr);
  DWORD pid = 0;
  if (!found || !GetWindowThreadProcessId(found, &pid) || pid != GetCurrentProcessId())
    return;
  taskbar = found;
  RunFromWindowThread(
      found,
      [](void*) {
        if (unloading)
          return;
        try {
          // Most checks stop here: no full-tree traversal while the binding lives.
          for (auto& binding : bindings)
            if (auto e = binding.element.get())
              if (auto f = e.try_as<FrameworkElement>())
                if (f.XamlRoot())
                  return;
          for (auto& binding : bindings)
            if (auto e = binding.element.get())
              e.RemoveHandler(UIElement::RightTappedEvent(), binding.handler);
          bindings.clear();
          auto root = GetTaskbarXamlRoot(taskbar.load());
          if (root)
            Scan(root.Content());
        } catch (...) {
          BatteryControls::Status(L"Taskbar integration unavailable.");
        }
      },
      nullptr);
}
// BEGIN TESTABLE INITIALIZATION GUARD
std::atomic<bool> initializationStarted = false;
bool BeginInitialization() {
  return !initializationStarted.exchange(true);
}
// END TESTABLE INITIALIZATION GUARD
BOOL Wh_ModInit() {
  // Never revive globals while old delegates or workers may still reference them.
  if (!BeginInitialization()) {
    OutputDebugStringW(
        L"ThinkPad Battery Controls: retained instance cannot restart safely. Restart Explorer or "
        L"sign out before enabling again.");
    return FALSE;
  }
  try {
    taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (taskbar) {
      GetWindowThreadProcessId(taskbar.load(), &pid);
      if (pid != GetCurrentProcessId())
        return FALSE;
    }
    try {
      symbolsReady = HookTaskbarDllSymbols();
    } catch (...) {
      symbolsReady = false;
    }
    if (!symbolsReady)
      Engine::Log(L"Taskbar symbols unavailable; recovery worker remains active.");
    using namespace BatteryControls;
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!stopEvent || !wakeEvent) {
      if (stopEvent)
        CloseHandle(stopEvent);
      if (wakeEvent)
        CloseHandle(wakeEvent);
      stopEvent = wakeEvent = nullptr;
      return FALSE;
    }
    threadHandle = CreateThread(nullptr, 0, Thread, nullptr, 0, nullptr);
    if (!threadHandle) {
      CloseHandle(stopEvent);
      CloseHandle(wakeEvent);
      stopEvent = wakeEvent = nullptr;
      return FALSE;
    }
    return TRUE;
  } catch (...) {
    return FALSE;
  }
}
void Wh_ModAfterInit() {}
void Wh_ModSettingsChanged() {
  BatteryControls::Submit(0);
}
void Wh_ModBeforeUninit() {
  unloading = true;
  using namespace BatteryControls;
  shutdownDeadline = GetTickCount64() + 4000;
  if (stopEvent)
    SetEvent(stopEvent);
  // Child execution has bounded cancellation. Pump only sent messages so a
  // worker's already-running taskbar dispatch can finish before UI cleanup.
  if (threadHandle) {
    auto until = GetTickCount64() + 12000;
    while (WaitForSingleObject(threadHandle, 0) == WAIT_TIMEOUT) {
      if (GetTickCount64() >= until) {
        RetainModule(L"charging worker did not stop within 12 seconds");
        Status(L"Worker shutdown deferred; recovery remains saved.");
        Engine::Close();
        return;
      }
      MsgWaitForMultipleObjects(1, &threadHandle, FALSE, 50, QS_SENDMESSAGE);
      MSG message;
      PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    }
  }
  try {
    bool cleaned = !taskbar.load() ||
                   RunFromWindowThread(
                       taskbar.load(),
                       [](void*) {
                         try {
                           ModernUI::Close();
                           for (auto& binding : bindings)
                             if (auto e = binding.element.get())
                               e.RemoveHandler(UIElement::RightTappedEvent(), binding.handler);
                           bindings.clear();
                         } catch (...) {
                           RetainModule(L"UI cleanup callback threw");
                           BatteryControls::Status(
                               L"UI cleanup failed. Module retained until Explorer exits.");
                         }
                       },
                       nullptr);
    if (!cleaned)
      RetainModule(L"taskbar cleanup dispatch failed");
  } catch (...) {
    RetainModule(L"taskbar cleanup dispatch threw");
  }
}
void Wh_ModUninit() {
  using namespace BatteryControls;
  Engine::Close();
  if (setupThread && WaitForSingleObject(setupThread.load(), 0) != WAIT_OBJECT_0)
    RetainModule(L"setup thread still awaiting approval or process completion");
  // Revoked delegates can be released asynchronously by XAML. Do not wait
  // forever, and never unload while a remaining delegate can call our code.
  auto until = GetTickCount64() + 2000;
  while (winrt::get_module_lock() != 0 && GetTickCount64() < until)
    Sleep(20);
  if (winrt::get_module_lock() != 0)
    RetainModule(L"XAML delegates remain after bounded drain");
  else
    OutputDebugStringW(L"ThinkPad Battery Controls: XAML delegates drained.\n");
  if (retainModule)
    return;
  if (setupThread)
    CloseHandle(setupThread.load());
  setupThread = nullptr;
  if (threadHandle)
    CloseHandle(threadHandle);
  if (stopEvent)
    CloseHandle(stopEvent);
  if (wakeEvent)
    CloseHandle(wakeEvent);
  threadHandle = stopEvent = wakeEvent = nullptr;
}
