// ==WindhawkMod==
// @id thinkpad-battery-controls
// @name ThinkPad Battery Controls
// @description Charge to full temporarily and restore Lenovo battery protection.
// @version 0.9.6
// @author Bill Bauman
// @github https://github.com/billbauman
// @license Apache-2.0
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lshell32 -luser32 -lole32 -lruntimeobject -lwindowsapp -lbcrypt -ladvapi32 -lcrypt32 -luuid -lkernel32 -lrpcrt4 -fms-extensions
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# ThinkPad Battery Controls

Right-click the Windows battery icon to set charging limits, apply saved presets,
or temporarily charge to full. Uses Windows 11 menus without an extra tray icon.

The manufacturer should have made these everyday controls part of the Windows
battery menu. It is a shame they did not. Feedback and compatibility reports are welcome.

## Setup
Choose a backend in Windhawk Settings:
- **Integrated Lenovo service** (default): no utility download. Requires compatible
  Lenovo Power Manager service and drivers.
- **Lenovo ChargeThreshold.exe**: [download Lenovo's utility](https://download.lenovo.com/pccbbs/thinkvantage_en/metroapps/Vantage/ChargeThreshold/ChargeThreshold.exe),
  save it wherever you prefer on a local drive, and enter its full path in Settings.
  The mod verifies the reviewed executable before running it. It never downloads,
  installs or elevates anything. Setup and diagnostics includes the download link.

The gear beside the charge summary opens diagnostics and a driver link based on
this machine's detected model. No serial number is read or sent. Drivers are installed separately.

## Charging and presets
Save up to 20 named presets and choose up to five quick favorites. Manage presets
is available in Set charge threshold and at the bottom of Apply charging preset.
Quick presets apply to both batteries. Set charge threshold can edit each battery
separately or link their limits with the integrated backend. Each result is read back.

Charge to full saves the original limits, disables them temporarily, then restores
at 100% or after eight hours. Cancel restores immediately. A newer enabled range
from Vantage is preserved. The integrated backend restores each battery separately;
Lenovo's executable cannot restore different ranges on two batteries. Select the
integrated backend for that situation. Missing batteries defer restoration safely.

**Thresholds persist after this mod is disabled or removed.** Disabling attempts
restoration of a pending full-charge request. If restoration fails, Explorer crashes,
or the mod is removed while it cannot run, limits may remain off and charging can
continue to 100% until the mod runs again or you set limits in Vantage. Removing
the mod also removes its Windhawk recovery data. Restore protection before removal.

## Advanced controls
Battery health shows supported readings. Power offers the verified three-state USB
control. Calibration requires a separate confirmation and connected power; it saves
current limits and blocks charging edits while running. Cancellation also requires
confirmation. Removing the mod during calibration loses saved recovery: check its
state and charging limits in Vantage. Cooling and airplane controls remain omitted
until supported behavior is verified. Calibration is not a battery-health treatment.

## Compatibility
Tested on a ThinkPad P14s Gen 6 AMD with one battery. Two-battery behavior has
mock coverage but needs a two-battery hardware report. The executable backend
requires English output. Private taskbar symbols can change after Windows updates.

Author/publisher: Bill Bauman. Apache-2.0. Project-name trademark and attribution
terms are in the source repository; Lenovo's executable remains separately licensed.
No Lenovo or Microsoft endorsement is implied.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- backend: native
  $name: Charging backend
  $description: Integrated calls use Lenovo Power Manager directly. The executable option uses Lenovo's separately downloaded utility. No automatic fallback occurs.
  $options:
  - native: Integrated Lenovo service
  - executable: Lenovo ChargeThreshold.exe
- utilityPath: ""
  $name: Full path to ChargeThreshold.exe (executable backend only)
  $description: Download the linked Lenovo utility yourself, save it in a local folder of your choice, then enter the full executable path here. The integrated backend ignores this setting.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windhawk_api.h>
// Windhawk 1.6.1 puts WINAPI after a lambda parameter list. With MS extensions
// that expands to syntax Clang rejects. On our 64-bit targets WINAPI does not
// select a different calling convention; scope this compatibility fix to the header.
#pragma push_macro("WINAPI")
#undef WINAPI
#define WINAPI
#include <windhawk_utils.h>
#pragma pop_macro("WINAPI")
#include <optional>
#include <list>
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
#include <rpc.h>
#include <rpcndr.h>
#include <sstream>

void AttachNativeMenu();
std::atomic<bool> unloading = false, symbolsReady = false;
namespace BatteryControls {
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
HANDLE threadHandle = nullptr, stopEvent = nullptr, wakeEvent = nullptr;
std::mutex queueMutex;
std::deque<std::pair<int, int>> commands;
std::atomic<bool> active = false, busy = false, refreshing = false;
std::atomic<ULONGLONG> shutdownDeadline = 0;
struct Pack {
  DWORD present = 0, enabled = 0;
  int start = 0, stop = 0;
};
struct State {
  bool valid = false, enabled = false;
  int start = 0, stop = 0;
  bool mixed = false;
  Pack packs[2]{};
};
constexpr bool ValidRange(int start, int stop) {
  return start >= 5 && start < stop && stop <= 100;
}
bool ValidPack(const Pack& p) {
  return p.present <= 1 && p.enabled <= 1 &&
         (!p.enabled || (p.present && ValidRange(p.start, p.stop)));
}
void Summarize(State& s) {
  const Pack* first = nullptr;
  s.enabled = false;
  s.mixed = false;
  s.start = s.stop = 0;
  for (auto& p : s.packs)
    if (p.present) {
      s.enabled = s.enabled || p.enabled;
      if (!first) {
        first = &p;
        s.start = p.start;
        s.stop = p.stop;
      } else if (p.enabled != first->enabled ||
                 (p.enabled && (p.start != first->start || p.stop != first->stop)))
        s.mixed = true;
    }
  s.valid = first != nullptr;
  if (s.mixed)
    s.start = s.stop = 0;
}
struct Recovery {
  DWORD magic = 0x54424332, version = 2, active = 0, reserved = 0;
  ULONGLONG deadline = 0;
  Pack prior[2]{};
};
bool ValidRecovery(const Recovery& r, ULONGLONG) {
  if (r.magic != 0x54424332 || r.version != 2 || r.active > 1 || r.reserved ||
      !ValidPack(r.prior[0]) || !ValidPack(r.prior[1]))
    return false;
  return !r.active || ((r.prior[0].enabled || r.prior[1].enabled) && r.deadline > 0 &&
                       r.deadline <= 0x7fffffffffffffffULL);
}
bool RecoveryDue(const Recovery& r, ULONGLONG now) {
  return r.active && (now >= r.deadline || r.deadline - now > 8 * 3600 + 300);
}
State Parse(std::string_view text) {
  State s;
  auto number = [](std::string_view value, int& out) {
    auto r = std::from_chars(value.data(), value.data() + value.size(), out);
    return r.ec == std::errc{} && r.ptr == value.data() + value.size();
  };
  size_t pos = 0;
  while ((pos = text.find("Battery #", pos)) != text.npos) {
    auto line = text.substr(pos);
    line = line.substr(0, line.find_first_of("\r\n"));
    pos += line.size();
    if (line.size() < 12 || (line[9] != '1' && line[9] != '2') || line.substr(10, 2) != ": ")
      return {};
    auto& pack = s.packs[line[9] - '1'];
    if (pack.present)
      return {};
    pack.present = 1;
    auto value = line.substr(12);
    if (value == "OFF.")
      continue;
    constexpr std::string_view prefix = "Start at ", middle = "%, Stop at ";
    if (!value.starts_with(prefix) || !value.ends_with("%."))
      return {};
    value.remove_prefix(prefix.size());
    value.remove_suffix(2);
    auto split = value.find(middle);
    if (split == value.npos || !number(value.substr(0, split), pack.start) ||
        !number(value.substr(split + middle.size()), pack.stop) ||
        !ValidRange(pack.start, pack.stop))
      return {};
    pack.enabled = 1;
  }
  Summarize(s);
  return s;
}
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
std::wstring operationStatus;
ULONGLONG statusAt = 0;
Snapshot ReadSnapshot() {
  std::lock_guard lock(stateMutex);
  return snapshot;
}
void SetUtility(int value) {
  {
    std::lock_guard lock(stateMutex);
    snapshot.utility = value;
  }
  Wh_Log(L"utilityCheck=%d", int(value));
}
void Status(PCWSTR value) noexcept {
  try {
    {
      std::lock_guard lock(stateMutex);
      operationStatus = value;
      statusAt = GetTickCount64();
    }
    Wh_Log(L"operationStatus: %s", value);
  } catch (...) {
  }
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
std::wstring UtilityPath() {
  return std::wstring(WindhawkUtils::StringSetting::make(L"utilityPath").get());
}
bool LockUtility(const std::wstring& path, Handle& file, std::vector<HANDLE>& directories) {
  if (path.size() < 4 || path[1] != L':' || path[2] != L'\\' || path.find(L'"') != path.npos)
    return false;
  DWORD n = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
  if (!n || n > 32768)
    return false;
  std::wstring full(n, L'\0');
  DWORD copied = GetFullPathNameW(path.c_str(), n, full.data(), nullptr);
  if (!copied || copied >= n || _wcsicmp(full.c_str(), path.c_str()))
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
        (info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
      return false;
    pos = end + 1;
  }
  file.value = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
  if (!file)
    return false;
  FILE_ATTRIBUTE_TAG_INFO info{};

  if (!GetFileInformationByHandleEx(file, FileAttributeTagInfo, &info, sizeof(info)) ||
      (info.FileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY)))
    return false;
  n = GetFinalPathNameByHandleW(file, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
  if (!n || n > 32768)
    return false;
  std::wstring finalPath(n, L'\0');
  copied =
      GetFinalPathNameByHandleW(file, finalPath.data(), n, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
  if (!copied || copied >= n || _wcsicmp(finalPath.c_str(), (L"\\\\?\\" + path).c_str()))
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
  wchar_t system[MAX_PATH];
  if (!GetSystemDirectoryW(system, MAX_PATH))
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
  Wh_Log(L"utilityExit=%d", int(code));
  return done && !overflow && code == 0;
}
namespace NativeRpc {
// Generated NDR descriptors from research/lenovo-rpc/native.idl using Windows SDK MIDL.
// Only explicitly named, typed operations are callable. No Lenovo executable code is embedded.
const unsigned char types[] = {
    0x00, 0x00, 0x11, 0x04, 0x02, 0x00, 0x30, 0xa0, 0x00, 0x00, 0x11, 0x04, 0x02, 0x00, 0x30, 0xe1,
    0x00, 0x00, 0x30, 0x41, 0x00, 0x00, 0x11, 0x0c, 0x06, 0x5c, 0x11, 0x0c, 0x08, 0x5c, 0x00,
};
const unsigned char procOpen[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x32, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x40, 0x01, 0x0a, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procClose[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x30, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x00, 0x38, 0x00, 0x40, 0x01, 0x0a, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x0e, 0x00,
};
const unsigned char procCoolCap[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procCoolingCap[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procAirplaneCap[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procUsbCap[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procUsbValue[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1c, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x08, 0x00,
};
const unsigned char procAirplaneValue[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procCoolingAuto[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procCoolingValue[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procCoolValue[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x00, 0x1a, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procSetAirplane[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2a, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procSetCoolingAuto[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2a, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procSetCooling[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2a, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x06, 0x00,
};
const unsigned char procSetUsb[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2c, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x08, 0x00,
};
const unsigned char procResetCap[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x18, 0x00, 0x30, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x2c, 0x00, 0x1a, 0x00, 0x40, 0x03, 0x0a, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50,
    0x21, 0x08, 0x00, 0x06, 0x00, 0x48, 0x00, 0x10, 0x00, 0x08, 0x00,
};
const unsigned char procQuery[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x30, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x6c, 0x00, 0x40, 0x06, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x06, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
    0x50, 0x21, 0x20, 0x00, 0x08, 0x00, 0x50, 0x21, 0x28, 0x00, 0x08, 0x00,
};
const unsigned char procResetState[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x24, 0x00, 0x54, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x50, 0x21, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x08, 0x00,
};
const unsigned char procStartReset[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2c, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x08, 0x00,
};
const unsigned char procStopReset[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x10, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x2c, 0x00, 0x00, 0x00, 0x40, 0x02, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00, 0x08, 0x00,
};
const unsigned char procSetThreshold[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x48, 0x00, 0x10, 0x00, 0x08, 0x00, 0x48, 0x00, 0x18, 0x00, 0x08, 0x00,
};
const unsigned char procFullCapacity[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procDesignCapacity[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procDesignVoltage[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procAdapterWatts[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procVoltage[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procCurrent[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procTemperature[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procCycles[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procPercent[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procRemaining[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procRemainingTime[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procCompletionTime[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x35, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procChargeStatus[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procChemistry[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x37, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const unsigned char procHealth[] = {
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x20, 0x00, 0x30, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x2c, 0x00, 0x36, 0x00, 0x40, 0x04, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x48, 0x00, 0x08, 0x00,
    0x08, 0x00, 0x50, 0x21, 0x10, 0x00, 0x08, 0x00, 0x50, 0x21, 0x18, 0x00, 0x06, 0x00,
};
const RPC_CLIENT_INTERFACE client = {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0xaf8abfc6, 0x2132, 0x4870, {0xbf, 0x8d, 0xbc, 0xa5, 0x41, 0xff, 0xcf, 0x0b}}, {1, 0}},
    {{0x8a885d04, 0x1ceb, 0x11c9, {0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60}}, {2, 0}},
    nullptr,
    0,
    nullptr,
    0,
    nullptr,
    0};
void* __RPC_USER Allocate(size_t size) {
  return malloc(size);
}
void __RPC_USER Free(void* p) {
  free(p);
}
RPC_BINDING_HANDLE autoBinding = nullptr;
const MIDL_STUB_DESC stub = {const_cast<RPC_CLIENT_INTERFACE*>(&client),
                             Allocate,
                             Free,
                             {&autoBinding},
                             nullptr,
                             nullptr,
                             nullptr,
                             nullptr,
                             types,
                             1,
                             0x50002,
                             nullptr,
                             0x8010274,
                             nullptr,
                             nullptr,
                             nullptr,
                             1,
                             nullptr,
                             nullptr,
                             nullptr};
// These tiny SEH boundaries contain only plain data, never C++ objects with destructors.
DWORD Open(RPC_BINDING_HANDLE binding, void** context) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procOpen), binding, context);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
void Close(void** context) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procClose), context);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    RpcSmDestroyClientContext(context);
  }
}
struct Battery {
  short capable = 0, enabled = 0;
  long start = 0, stop = 0;
};
DWORD Read(void* context, long battery, Battery* result) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procQuery), context, battery, &result->capable,
                   &result->enabled, &result->start, &result->stop);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD Write(void* context, long battery, long start, long stop) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procSetThreshold), context, battery, start,
                   stop);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
struct Session {
  RPC_BINDING_HANDLE binding = nullptr;
  void* context = nullptr;
  ~Session() { Reset(); }
  void Reset() noexcept {
    if (context)
      Close(&context);
    context = nullptr;
    if (binding)
      RpcBindingFree(&binding);
  }
  Session() = default;
  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;
  DWORD Connect(ULONGLONG deadline) {
    DWORD error = RPC_S_SERVER_UNAVAILABLE;
    for (int i = 0; i < 10 && GetTickCount64() < deadline; i++) {
      wchar_t endpoint[64];
      swprintf_s(endpoint, L"BaseModuleRpcEndpoint_%d", i);
      RPC_WSTR text = nullptr;
      error = RpcStringBindingComposeW(
          nullptr, reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(L"ncalrpc")), nullptr,
          reinterpret_cast<RPC_WSTR>(endpoint), nullptr, &text);
      if (!error)
        error = RpcBindingFromStringBindingW(text, &binding);
      if (text)
        RpcStringFreeW(&text);
      if (!error)
        error = RpcBindingSetOption(binding, RPC_C_OPT_CALL_TIMEOUT, 1000);
      if (!error)
        error = Open(binding, &context);
      if (!error && context)
        return 0;
      if (!error)
        error = RPC_X_SS_CONTEXT_MISMATCH;
      Reset();
      // Never try another endpoint after access denial or an unexpected protocol fault.
      if (error != RPC_S_SERVER_UNAVAILABLE && error != RPC_S_UNKNOWN_IF &&
          error != EPT_S_NOT_REGISTERED)
        break;
    }
    return error;
  }
};

// Advanced calls use fixed, generated signatures. No caller-controlled operation numbers.
DWORD ShortRead(void* ctx, const unsigned char* proc, short* value) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(proc), ctx, value);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD LongRead(void* ctx, const unsigned char* proc, long* value) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(proc), ctx, value);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD MetricRead(void* ctx,
                 const unsigned char* proc,
                 long slot,
                 long* value,
                 short* capable) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(proc), ctx, slot, value, capable);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD ResetCapability(void* ctx, long slot, short* capable) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procResetCap), ctx, capable, slot);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD CalibrationState(void* ctx, long* battery, long* stage, long* stages) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(procResetState), ctx, battery, stage, stages);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD ShortWrite(void* ctx, const unsigned char* proc, short value) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(proc), ctx, value);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
DWORD LongWrite(void* ctx, const unsigned char* proc, long value) noexcept {
  __try {
    NdrClientCall2(&stub, const_cast<unsigned char*>(proc), ctx, value);
    return 0;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return GetExceptionCode();
  }
}
struct Metric {
  bool available = false;
  long value = 0;
};
struct AdvancedBattery {
  bool present = false, resetCapable = false;
  Metric full, design, temperature, cycles, percent, voltage, adapter;
};
struct Advanced {
  bool connected = false, usbSupported = false, airplaneSupported = false, coolingSupported = false;
  long usb = 0;
  short airplane = 0, cooling = 0;
  bool resetKnown = false;
  long resetBattery = 0, resetStage = 0, resetStages = 0;
  AdvancedBattery batteries[2];
};
Advanced ReadAdvanced(ULONGLONG deadline) {
  Advanced a;
  Session session;
  if (session.Connect(deadline))
    return a;
  a.connected = true;
  auto boolean = [&](const unsigned char* cap, const unsigned char* getter, short& value) {
    short supported = 0;
    return GetTickCount64() < deadline && !ShortRead(session.context, cap, &supported) &&
           supported == 1 && !ShortRead(session.context, getter, &value) &&
           (value == 0 || value == 1);
  };
  short usb = 0;
  a.usbSupported = GetTickCount64() < deadline && !ShortRead(session.context, procUsbCap, &usb) &&
                   usb == 1 && !LongRead(session.context, procUsbValue, &a.usb) && a.usb >= 0 &&
                   a.usb <= 2;
  a.airplaneSupported = boolean(procAirplaneCap, procAirplaneValue, a.airplane);
  a.coolingSupported = boolean(procCoolingCap, procCoolingValue, a.cooling);
  a.resetKnown =
      GetTickCount64() < deadline &&
      !CalibrationState(session.context, &a.resetBattery, &a.resetStage, &a.resetStages) &&
      a.resetBattery >= 0 && a.resetBattery <= 2 && a.resetStage >= 0 && a.resetStages >= 0 &&
      a.resetStage <= a.resetStages;
  for (long slot = 1; slot <= 2 && GetTickCount64() < deadline; slot++) {
    auto& b = a.batteries[slot - 1];
    Battery thresholds;
    b.present = !Read(session.context, slot, &thresholds) && thresholds.capable == 1;
    if (!b.present)
      continue;
    short reset = 0;
    b.resetCapable = !ResetCapability(session.context, slot, &reset) && reset == 1;
    auto metric = [&](const unsigned char* proc, long min, long max) {
      Metric v;
      short cap = 0;
      v.available = GetTickCount64() < deadline &&
                    !MetricRead(session.context, proc, slot, &v.value, &cap) && cap == 1 &&
                    v.value >= min && v.value <= max;
      return v;
    };
    b.full = metric(procFullCapacity, 1, 1000000);
    b.design = metric(procDesignCapacity, 1, 1000000);
    b.temperature = metric(procTemperature, -40, 120);
    b.cycles = metric(procCycles, 0, 100000);
    b.percent = metric(procPercent, 0, 100);
    b.voltage = metric(procVoltage, 1, 100000);
    b.adapter = metric(procAdapterWatts, 1, 1000);
  }
  return a;
}
// Implements the utility's documented status/off/on STOP [START] command contract.
// Both battery slots are enumerated, just as in the utility. The app's Parse gate
// retains both battery slots before any UI-originated write.
bool Run(const std::wstring& command, std::string& output, ULONGLONG deadline, DWORD& error) {
  error = ERROR_INVALID_PARAMETER;
  std::wistringstream input(command);
  std::wstring verb, extra;
  input >> verb;
  bool setting = false;
  int start = 0, stop = 0;
  if (verb == L"on") {
    setting = true;
    if (!(input >> stop))
      return false;
    if (input >> start) {
    } else {
      if (!input.eof())
        return false;
      input.clear();
      start = stop - 5;
    }
    if (start < 5 || start >= stop || stop > 100)
      return false;
  } else if (verb == L"off")
    setting = true;
  else if (verb != L"status")
    return false;
  if (input >> extra)
    return false;
  if (GetTickCount64() >= deadline) {
    error = ERROR_TIMEOUT;
    return false;
  }
  Session session;
  error = session.Connect(deadline);
  if (error)
    return false;
  Battery batteries[2];
  int count = 0;
  for (long i = 0; i < 2; i++) {
    if (GetTickCount64() >= deadline) {
      error = ERROR_TIMEOUT;
      return false;
    }
    error = Read(session.context, i + 1, &batteries[i]);
    if (error)
      return false;
    auto& b = batteries[i];
    if ((b.capable != 0 && b.capable != 1) || (b.enabled != 0 && b.enabled != 1) ||
        (b.capable && b.enabled && (b.start < 5 || b.start >= b.stop || b.stop > 100))) {
      error = ERROR_INVALID_DATA;
      return false;
    }
    if (b.capable)
      count++;
  }
  if (!count) {
    error = ERROR_NOT_SUPPORTED;
    return false;
  }
  for (long i = 0; i < 2; i++) {
    auto& b = batteries[i];
    if (!b.capable)
      continue;
    if (setting) {
      if (GetTickCount64() >= deadline) {
        error = ERROR_TIMEOUT;
        return false;
      }
      error = Write(session.context, i + 1, start, stop);
      if (error)
        return false;
      // Verify the service accepted the requested state, including off.
      Battery actual;
      error = Read(session.context, i + 1, &actual);
      if (error)
        return false;
      if (actual.capable != 1 || actual.enabled != (verb == L"on" ? 1 : 0) ||
          (actual.enabled && (actual.start != start || actual.stop != stop))) {
        error = ERROR_WRITE_FAULT;
        return false;
      }
    } else {
      output += "Charge threshold for Battery #" + std::to_string(i + 1) + ": ";
      output += b.enabled ? "Start at " + std::to_string(b.start) + "%, Stop at " +
                                std::to_string(b.stop) + "%.\n"
                          : "OFF.\n";
    }
  }
  return true;
}
}  // namespace NativeRpc

// Snapshot the setting once per operation so a settings change cannot split a write/readback.
bool IntegratedBackend() {
  return std::wstring(WindhawkUtils::StringSetting::make(L"backend").get()) != L"executable";
}
bool RunSelected(bool integrated, const std::wstring& args, std::string& output) {
  if (!integrated)
    return Run(args, output);
  auto shutdown = shutdownDeadline.load();
  if ((shutdown && GetTickCount64() >= shutdown) ||
      (!shutdown && WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0))
    return false;
  DWORD error = 0;
  bool ok = NativeRpc::Run(args, output, shutdown ? shutdown : GetTickCount64() + 5000, error);
  Wh_Log(L"nativeRpcError=%d", int(error));
  SetUtility(ok ? 5 : 6);
  return ok;
}
State QueryWith(bool integrated) {
  std::string output;
  if (!RunSelected(integrated, L"status", output))
    return {};
  return Parse(output);
}
State Query() {
  return QueryWith(IntegratedBackend());
}
bool Set(State target) {
  if (target.enabled && !ValidRange(target.start, target.stop))
    return false;
  bool integrated = IntegratedBackend();
  State before = QueryWith(integrated);
  if (!before.valid)
    return false;
  std::string output;
  bool accepted = RunSelected(
      integrated,
      target.enabled ? L"on " + std::to_wstring(target.stop) + L" " + std::to_wstring(target.start)
                     : L"off",
      output);
  State actual = QueryWith(integrated);
  if (!accepted || !actual.valid)
    return false;
  for (int i = 0; i < 2; i++) {
    auto& p = actual.packs[i];
    if (p.present != before.packs[i].present ||
        (p.present && (bool(p.enabled) != target.enabled ||
                       (target.enabled && (p.start != target.start || p.stop != target.stop)))))
      return false;
  }
  return true;
}
bool RestorePack(int slot, const Pack& target) {
  NativeRpc::Session session;
  auto deadline = shutdownDeadline.load();
  if (!deadline)
    deadline = GetTickCount64() + 5000;
  if (session.Connect(deadline))
    return false;
  NativeRpc::Battery current;
  if (NativeRpc::Read(session.context, slot, &current) || current.capable != 1)
    return false;
  // Recheck Vantage precedence immediately before the write.
  if (current.enabled == 1)
    return true;
  if (current.enabled != 0 || !target.enabled || GetTickCount64() >= deadline)
    return false;
  if (NativeRpc::Write(session.context, slot, target.start, target.stop))
    return false;
  NativeRpc::Battery actual;
  return !NativeRpc::Read(session.context, slot, &actual) && actual.capable == 1 &&
         actual.enabled == 1 && actual.start == target.start && actual.stop == target.stop;
}
bool Save(const Recovery& value) {
  if (!ValidRecovery(value, Now()) || (!Wh_SetIntValue(L"RecoveryPresent", 1) ||
                                       !Wh_SetBinaryValue(L"Recovery", &value, sizeof(value))))
    return false;
  recovery = value;
  active = value.active != 0;
  return true;
}
void Load() {
  Recovery candidate{};
  auto bytes = Wh_GetBinaryValue(L"Recovery", &candidate, sizeof(candidate));
  if (!bytes) {
    recoveryInvalid = Wh_GetIntValue(L"RecoveryPresent", 0) != 0;
    return;
  }
  if (bytes != sizeof(candidate) || !ValidRecovery(candidate, Now())) {
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
bool Restore() {
  if (recoveryInvalid || !ValidRecovery(recovery, Now()))
    return false;
  if (!Active())
    return true;
  State current = Query();
  if (!current.valid)
    return false;
  bool needed = false;
  for (int i = 0; i < 2; i++) {
    if (current.packs[i].present != recovery.prior[i].present)
      return false;
    if (recovery.prior[i].enabled && !current.packs[i].enabled)
      needed = true;
  }
  if (!needed)
    return Clear();
  if (IntegratedBackend()) {
    for (int i = 0; i < 2; i++)
      if (recovery.prior[i].enabled && !current.packs[i].enabled)
        if (!RestorePack(i + 1, recovery.prior[i]))
          return false;
  } else {
    State prior;
    std::copy(std::begin(recovery.prior), std::end(recovery.prior), prior.packs);
    Summarize(prior);
    // The vendor CLI cannot select a battery. Never overwrite another battery's newer range.
    if (prior.mixed || current.enabled) {
      Status(
          L"Individual battery restoration requires the integrated backend. Select it in Windhawk "
          L"Settings or restore the limits in Vantage.");
      return false;
    }
    if (!Set({true, true, prior.start, prior.stop}))
      return false;
  }
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
  if (!IntegratedBackend() && state.mixed) {
    Status(
        L"These batteries have different limits. Select the integrated backend in Windhawk "
        L"Settings to preserve and restore each battery's limits.");
    return;
  }
  Recovery next;
  next.active = 1;
  next.deadline = Now() + 8 * 3600;
  std::copy(std::begin(state.packs), std::end(state.packs), next.prior);
  if (!Save(next) || !Set({true, false, 0, 0})) {
    Error();
    return;
  }
  Status(L"Charge-to-full request saved.");
}
struct RememberedRanges {
  DWORD version = 1;
  int start[2]{}, stop[2]{};
};
RememberedRanges ReadRememberedRanges() {
  RememberedRanges value;
  auto bytes = Wh_GetBinaryValue(L"RememberedRanges", &value, sizeof(value));
  if (bytes != sizeof(value) || value.version != 1)
    return {};
  for (int i = 0; i < 2; ++i)
    if ((value.start[i] || value.stop[i]) && !ValidRange(value.start[i], value.stop[i]))
      return {};
  return value;
}
bool RememberRanges(const State& state, bool includeDisabled = false) {
  if (!state.valid)
    return false;
  auto value = ReadRememberedRanges(), before = value;
  for (int i = 0; i < 2; ++i)
    if (state.packs[i].present && (state.packs[i].enabled || includeDisabled) &&
        ValidRange(state.packs[i].start, state.packs[i].stop)) {
      value.start[i] = state.packs[i].start;
      value.stop[i] = state.packs[i].stop;
    }
  return memcmp(&value, &before, sizeof(value)) == 0 ||
         Wh_SetBinaryValue(L"RememberedRanges", &value, sizeof(value));
}
bool RememberOffDraft(int values) {
  auto state = Query();
  int start = LOWORD(values), stop = HIWORD(values);
  if (ValidRange(start, stop))
    for (auto& p : state.packs)
      if (p.present) {
        p.start = start;
        p.stop = stop;
      }
  return RememberRanges(state, true);
}
void RestoreRememberedRanges(State& state, const RememberedRanges& remembered) {
  if (!state.valid)
    return;
  for (int i = 0; i < 2; ++i) {
    auto& p = state.packs[i];
    if (p.present && !p.enabled && ValidRange(remembered.start[i], remembered.stop[i])) {
      p.start = remembered.start[i];
      p.stop = remembered.stop[i];
    }
  }
  Summarize(state);
}
void Refresh() {
  State state = Query();
  // Firmware can return zeroes when disabled. Remember ranges independently
  // without claiming that disabled limits are active or writing to hardware.
  if (state.valid && state.enabled)
    RememberRanges(state);
  RestoreRememberedRanges(state, ReadRememberedRanges());
  {
    std::lock_guard lock(stateMutex);
    snapshot.state = state;
    snapshot.revision++;
    refreshing = false;
  }
  Wh_Log(L"cacheValid=%d", int(state.valid));
  Wh_Log(L"cacheEnabled=%d", int(state.enabled));
  if (state.valid && state.enabled) {
    Wh_Log(L"cacheStart=%d", int(state.start));
    Wh_Log(L"cacheStop=%d", int(state.stop));
  }
}
namespace AdvancedControls {
struct Snapshot {
  NativeRpc::Advanced data;
  ULONGLONG revision = 0;
  std::wstring message;
};
std::mutex mutex;
Snapshot snapshot;
struct CalibrationRecord {
  DWORD magic = 0x43414c31, slot = 0, observed = 0, reserved = 0;
  Pack prior[2]{};
};
CalibrationRecord calibration;
bool invalidCalibration = false;
Snapshot Read() {
  std::lock_guard lock(mutex);
  return snapshot;
}
void Publish(const NativeRpc::Advanced& data, const std::wstring& message = L"") {
  std::lock_guard lock(mutex);
  snapshot.data = data;
  snapshot.message = message;
  snapshot.revision++;
}
bool SaveCalibration(CalibrationRecord next) {
  if (!Wh_SetIntValue(L"CalibrationPresent", 1) ||
      !Wh_SetBinaryValue(L"Calibration", &next, sizeof(next)))
    return false;
  calibration = next;
  invalidCalibration = false;
  return true;
}
void LoadCalibration() {
  DWORD n = Wh_GetBinaryValue(L"Calibration", &calibration, sizeof(calibration));
  invalidCalibration =
      (n != sizeof(calibration) && (n || Wh_GetIntValue(L"CalibrationPresent", 0))) ||
      calibration.magic != 0x43414c31 || calibration.slot > 2 || calibration.observed > 1 ||
      calibration.reserved;
  for (auto p : calibration.prior)
    if (p.present > 1 || p.enabled > 1 || (p.enabled && !ValidRange(p.start, p.stop)))
      invalidCalibration = true;
  if (invalidCalibration)
    Status(L"Calibration recovery is invalid. Check charging limits in Lenovo Vantage.");
}
bool ThresholdWritesAllowed() {
  if (!IntegratedBackend())
    return !calibration.slot && !invalidCalibration;
  if (calibration.slot || invalidCalibration)
    return false;
  NativeRpc::Session s;
  if (s.Connect(GetTickCount64() + 3000))
    return false;
  long b = 0, stage = 0, stages = 0;
  return !NativeRpc::CalibrationState(s.context, &b, &stage, &stages) && b == 0 && stage == 0;
}
void RefreshAdvanced() {
  if (!IntegratedBackend()) {
    Publish({}, L"Select Integrated Lenovo service in Windhawk Settings to use Advanced controls.");
    return;
  }
  auto a = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  Publish(a,
          a.connected ? L"" : L"Lenovo service could not be reached. No controls are available.");
}
// Run only on the single worker. Revalidate support immediately before every write.
void ApplyUsb(int value) {
  auto a = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  if (!IntegratedBackend() || !a.usbSupported || value < 0 || value > 2) {
    Publish(a, L"USB setting is unavailable. Nothing changed.");
    return;
  }
  NativeRpc::Session s;
  long actual = -1;
  bool ok = !s.Connect(GetTickCount64() + 3000) &&
            !NativeRpc::LongWrite(s.context, NativeRpc::procSetUsb, value) &&
            !NativeRpc::LongRead(s.context, NativeRpc::procUsbValue, &actual) && actual == value;
  a = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  Publish(a, ok ? L"USB power setting applied."
                : L"USB change was not confirmed. Refresh to check its current value.");
}
void CheckCalibration() {
  if (!calibration.slot || invalidCalibration || !IntegratedBackend())
    return;
  auto a = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  if (!a.resetKnown) {
    Publish(a, L"Calibration status unavailable. Saved limits are retained.");
    return;
  }
  if (a.resetBattery == long(calibration.slot)) {
    if (!calibration.observed) {
      auto next = calibration;
      next.observed = 1;
      SaveCalibration(next);
    }
    Publish(a, L"Calibration is running. Keep your laptop plugged in.");
    return;
  }
  if (a.resetBattery != 0) {
    Publish(a, L"Another battery is calibrating. Saved limits are retained.");
    return;
  }
  // An ambiguous start is never retried. Once idle is confirmed, preserve any newer enabled range.
  auto state = Query();
  bool restored = state.valid;
  for (int i = 0; i < 2; i++)
    if (state.packs[i].present != calibration.prior[i].present)
      restored = false;
  if (restored)
    for (int i = 0; i < 2; i++)
      if (calibration.prior[i].present && calibration.prior[i].enabled)
        restored = RestorePack(i + 1, calibration.prior[i]) && restored;
  if (restored && SaveCalibration({}))
    Publish(
        a,
        L"Calibration is idle. Charging protection checked; newer enabled ranges were preserved.");
  else
    Publish(a,
            L"Calibration is idle, but protection could not be verified. Saved limits are "
            L"retained. Check Lenovo Vantage.");
}
void Calibration(int slot, bool stop) {
  auto a = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  if (!IntegratedBackend() || slot < 1 || slot > 2 || !a.batteries[slot - 1].present ||
      !a.batteries[slot - 1].resetCapable || !a.resetKnown) {
    Publish(a, L"Calibration is unavailable for this battery.");
    return;
  }
  if (!stop) {
    SYSTEM_POWER_STATUS power{};
    if (Active() || calibration.slot || invalidCalibration || a.resetBattery != 0 ||
        !GetSystemPowerStatus(&power) || power.ACLineStatus != 1) {
      Publish(
          a, L"Connect power and end any charge-to-full or calibration operation before starting.");
      return;
    }
    auto state = Query();
    if (!state.valid) {
      Publish(a, L"Charging limits could not be saved. Calibration did not start.");
      return;
    }
    CalibrationRecord next;
    next.slot = slot;
    std::copy(std::begin(state.packs), std::end(state.packs), next.prior);
    if (!SaveCalibration(next)) {
      Publish(a, L"Charging limits could not be saved. Calibration did not start.");
      return;
    }
  } else if (a.resetBattery != slot) {
    Publish(a, L"This battery is not currently calibrating.");
    return;
  }
  NativeRpc::Session s;
  DWORD error = s.Connect(GetTickCount64() + 3000);
  if (!error)
    error = NativeRpc::LongWrite(s.context,
                                 stop ? NativeRpc::procStopReset : NativeRpc::procStartReset, slot);
  auto after = NativeRpc::ReadAdvanced(GetTickCount64() + 5000);
  if (!stop && after.resetKnown && after.resetBattery == slot) {
    auto next = calibration;
    next.observed = 1;
    SaveCalibration(next);
  }
  Publish(after,
          error ? L"Calibration command was not confirmed. Refresh before taking another action."
          : stop
              ? L"Cancellation requested. Refresh to verify that calibration has stopped."
              : L"Calibration requested. Keep the laptop connected and refresh to see its state.");
}
bool ApplySeparate(unsigned value) {
  if (!IntegratedBackend() || !ThresholdWritesAllowed())
    return false;
  Pack target[2];
  for (int i = 0; i < 2; i++) {
    int start = (value >> (i * 16)) & 255, stop = (value >> (i * 16 + 8)) & 255;
    if ((start || stop) && !ValidRange(start, stop))
      return false;
    target[i] = {1, DWORD(stop != 0), start, stop};
  }
  auto before = Query();
  if (!before.valid || !before.packs[0].present || !before.packs[1].present ||
      !RememberRanges(before))
    return false;
  NativeRpc::Session s;
  if (s.Connect(GetTickCount64() + 3000))
    return false;
  bool ok = true;
  int attempted = 0;
  for (int i = 0; i < 2; i++) {
    attempted = i + 1;
    NativeRpc::Battery actual;
    if (NativeRpc::Write(s.context, i + 1, target[i].start, target[i].stop) ||
        NativeRpc::Read(s.context, i + 1, &actual) || actual.capable != 1 ||
        actual.enabled != target[i].enabled ||
        (actual.enabled && (actual.start != target[i].start || actual.stop != target[i].stop))) {
      ok = false;
      break;
    }
  }
  if (!ok) {
    // Best-effort rollback only when this operation's exact value is still present.
    for (int i = 0; i < attempted; i++) {
      NativeRpc::Battery now;
      if (!NativeRpc::Read(s.context, i + 1, &now) && now.capable == 1 &&
          now.enabled == target[i].enabled &&
          (!now.enabled || (now.start == target[i].start && now.stop == target[i].stop))) {
        auto p = before.packs[i];
        NativeRpc::Write(s.context, i + 1, p.enabled ? p.start : 0, p.enabled ? p.stop : 0);
      }
    }
    Status(
        L"Both ranges were not confirmed. Original limits were restored where possible. Refresh "
        L"and check each battery.");
    return false;
  }
  return Clear();
}
}  // namespace AdvancedControls

struct ApplyResult {
  ULONGLONG revision = 0;
  std::wstring message;
};
std::mutex applyMutex;
std::atomic<bool> applyPending = false;
ApplyResult applyResult;
ApplyResult ReadApplyResult() {
  std::lock_guard lock(applyMutex);
  return applyResult;
}
void Dispatch(int action, int values) {
  if (action == 31 || action == 32 || action == 34) {
    try {
      Dispatch(action == 31 ? 5 : action == 32 ? 6 : 24, values);
    } catch (...) {
      Error();
    }
    auto message = RecentStatus();
    std::lock_guard lock(applyMutex);
    applyResult.message =
        message.empty() ? L"Apply was not confirmed. Check diagnostics." : message;
    ++applyResult.revision;
    applyPending = false;
    return;
  }

  if (action >= 20 && action <= 24) {
    if (action == 20)
      AdvancedControls::RefreshAdvanced();
    if (action == 21)
      AdvancedControls::ApplyUsb(values);
    if (action == 22 || action == 23)
      AdvancedControls::Calibration(values, action == 23);
    if (action == 24) {
      if (AdvancedControls::ApplySeparate(static_cast<unsigned>(values)))
        Status(L"Both battery ranges applied.");
      else
        Error();
    }
    Refresh();
    return;
  }
  if ((action == 1 || action == 2 || action == 5 || action == 6) &&
      !AdvancedControls::ThresholdWritesAllowed()) {
    Status(
        L"Charging changes are unavailable while calibration is active or its status cannot be "
        L"confirmed.");
    Refresh();
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
      Status((L"Applied: " + std::to_wstring(start) + L" to " + std::to_wstring(stop) + L"%.")
                 .c_str());
    else
      Error();
  } else if (action == 6) {
    if (RememberOffDraft(values) && Set({true, false, 0, 0}) && Clear())
      Status(L"Charge limits off.");
    else
      Error();
  }
  Refresh();
}
bool Submit(int action, int values = 0) noexcept {
  try {
    if (unloading || !wakeEvent) {
      return false;
    }
    std::lock_guard lock(queueMutex);
    // Bound work and coalesce repeated UI commands, including refresh requests.

    if (action == 0)
      refreshing = true;
    if (!commands.empty() && commands.back().first == action) {
      commands.back().second = values;
      return true;
    }
    if (commands.size() >= 8) {
      Status(L"Another battery operation is pending.");
      return false;
    }
    commands.emplace_back(action, values);
    SetEvent(wakeEvent);
    return true;
  } catch (...) {
    Status(L"Unable to queue battery operation.");
    return false;
  }
}
DWORD WINAPI Thread(void*) noexcept {
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
    AdvancedControls::LoadCalibration();

    if (recoveryInvalid)
      Status(L"Recovery data invalid. Apply your desired limits manually.");
    ULONGLONG checkAt = 0, calibrationCheckAt = 0;
    Submit(0);
    for (;;) {
      if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
        break;
      auto now = GetTickCount64();

      if (AdvancedControls::calibration.slot && now >= calibrationCheckAt) {
        AdvancedControls::CheckCalibration();
        calibrationCheckAt = GetTickCount64() + 30000;
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
      DWORD delay = AdvancedControls::calibration.slot
                        ? DWORD(calibrationCheckAt > now ? calibrationCheckAt - now : 0)
                        : INFINITE;
      if (Active())
        delay = (std::min)(delay, DWORD(checkAt > now ? checkAt - now : 0));

      HANDLE events[] = {stopEvent, wakeEvent};
      DWORD result = MsgWaitForMultipleObjects(2, events, FALSE, delay, QS_ALLINPUT);
      if (result == WAIT_OBJECT_0)
        break;
      if (result == WAIT_OBJECT_0 + 2) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
          TranslateMessage(&msg);
          DispatchMessageW(&msg);
        }
      }
    }
    ULONGLONG empty = 0;
    shutdownDeadline.compare_exchange_strong(empty, GetTickCount64() + 4000);

    if (Active() && !Restore())
      Status(L"Restoration deferred. Saved recovery will be checked next time the mod runs.");
  } catch (...) {
    Error();
  }
  return 0;
}
}  // namespace BatteryControls

// Taskbar XAML access helpers adapted from Taskbar tray system icon tweaks.
// Copyright (c) Michael Maltsev (m417z), MIT; see THIRD-PARTY-NOTICES.txt.
// Windhawk 1.6.1 puts WINAPI after a lambda parameter list. With MS extensions
// that expands to syntax Clang rejects. On our 64-bit targets WINAPI does not
// select a different calling convention; scope this compatibility fix to the header.
#pragma push_macro("WINAPI")
#undef WINAPI
#define WINAPI
#include <windhawk_utils.h>
#pragma pop_macro("WINAPI")
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
      Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
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
      Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
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
std::mutex uiInvokeMutex;
struct UiRequest {
  RunFromWindowThreadProc_t proc;
  void* parameter;
  bool done = false;
};
UiRequest* pendingUiRequest = nullptr;
const UINT uiMessage = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
LRESULT CALLBACK UiHook(int code, WPARAM w, LPARAM l) {
  if (code == HC_ACTION) {
    auto msg = reinterpret_cast<CWPSTRUCT*>(l);
    if (msg->message == uiMessage && pendingUiRequest && !pendingUiRequest->done) {
      auto request = pendingUiRequest;
      request->done = true;
      request->proc(request->parameter);
    }
  }
  return CallNextHookEx(nullptr, code, w, l);
}
bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t proc, void* parameter) {
  DWORD pid = 0, thread = GetWindowThreadProcessId(window, &pid);
  if (!thread || pid != GetCurrentProcessId())
    return false;
  if (thread == GetCurrentThreadId()) {
    proc(parameter);
    return true;
  }
  std::lock_guard lock(uiInvokeMutex);
  HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, UiHook, nullptr, thread);
  if (!hook)
    return false;
  UiRequest request{proc, parameter};
  pendingUiRequest = &request;
  SendMessageW(window, uiMessage, 0, 0);
  pendingUiRequest = nullptr;
  UnhookWindowsHookEx(hook);
  return request.done;
}

bool HookTaskbarDllSymbols() {
  HMODULE module = GetModuleHandleW(L"taskbar.dll");
  if (!module) {
    Wh_Log(L"Failed to load taskbar.dll");
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
[[clang::no_destroy]] std::optional<std::vector<std::function<void()>>> uiRevokers{std::in_place};
template <class T>
void TrackEvent(T revoker) {
  auto owned = std::make_shared<T>(std::move(revoker));
  uiRevokers->emplace_back([owned] { owned->revoke(); });
}
void RevokeUiEvents() {
  auto pending = std::move(*uiRevokers);
  uiRevokers->clear();
  for (auto& revoke : pending)
    revoke();
}
namespace C = winrt::Windows::UI::Xaml::Controls;
namespace P = winrt::Windows::UI::Xaml::Controls::Primitives;
[[clang::no_destroy]] C::MenuFlyout menu{nullptr};
[[clang::no_destroy]] C::Flyout flyout{nullptr};
[[clang::no_destroy]] DispatcherTimer pendingMenu{nullptr}, statusTimer{nullptr};
struct PresetEditor;
[[clang::no_destroy]] std::shared_ptr<PresetEditor> presetEditor;
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
std::wstring PresetStorageKey() {
  BatteryControls::Handle token;
  DWORD bytes = 0;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, token.out()))
    return {};
  GetTokenInformation(token, TokenUser, nullptr, 0, &bytes);
  if (!bytes || bytes > 65536)
    return {};
  std::vector<BYTE> buffer(bytes);
  if (!GetTokenInformation(token, TokenUser, buffer.data(), bytes, &bytes))
    return {};
  LPWSTR sid = nullptr;
  if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &sid))
    return {};
  std::wstring result = L"PresetLibrary." + std::wstring(sid);
  LocalFree(sid);
  return result;
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
  auto key = PresetStorageKey();
  return !key.empty() && Wh_SetIntValue((key + L".present").c_str(), 1) &&
         Wh_SetBinaryValue(key.c_str(), &data, sizeof(data));
}
std::vector<Preset> ReadPresets() {
  presetNotice.clear();
  auto key = PresetStorageKey();
  if (key.empty()) {
    presetNotice = L"Unable to identify the current user's preset storage.";
    return {};
  }
  PresetStore data;
  auto bytes = Wh_GetBinaryValue(key.c_str(), &data, sizeof(data));
  if (!bytes) {
    if (Wh_GetIntValue((key + L".present").c_str(), 0)) {
      presetNotice =
          L"Saved presets are unavailable. Restore defaults or add replacements, then Save.";
      return {};
    }
    return DefaultPresets();
  }
  std::vector<Preset> rows;
  if (bytes == sizeof(data) && DecodePresets(data, rows))
    return rows;
  presetNotice = L"Saved presets are invalid. Restore defaults or add replacements, then Save.";
  return {};
}
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
[[clang::no_destroy]] std::shared_ptr<Panel> panel;
struct ThresholdDraft {
  bool valid = false, enabled = false;
  int start = 75, stop = 80;
};
ThresholdDraft returningDraft, addPresetDraft;
int presetReturnView = 0;
bool preservePanelHost = false;
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
  if (flyout && !preservePanelHost) {
    flyout.Hide();
    flyout.Content(nullptr);
  }
  if (menu)
    menu.Items().Clear();
  menu = nullptr;
  if (!preservePanelHost)
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
void PositionPanel(FrameworkElement target, P::FlyoutBase current) {
  auto root = target.XamlRoot();
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  if (!root || !GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar) ||
      !GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi))
    return;
  double scale = root.RasterizationScale(), gap = 12 * scale;
  for (auto popup : Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
    auto presenter = popup.Child().try_as<FrameworkElement>();
    bool owned = false;
    if (auto panel = current.try_as<C::Flyout>()) {
      auto p = presenter.try_as<C::FlyoutPresenter>();
      owned = p && p.Content() == panel.Content();
    } else if (auto quick = current.try_as<C::MenuFlyout>()) {
      auto p = presenter.try_as<C::MenuFlyoutPresenter>();
      owned = p && p.Items().Size() && quick.Items().Size() &&
              p.Items().GetAt(0) == quick.Items().GetAt(0);
    }
    if (!owned)
      continue;
    presenter.UpdateLayout();
    auto origin = presenter.TransformToVisual(root.Content()).TransformPoint({0, 0});
    double width = presenter.ActualWidth() * scale, height = presenter.ActualHeight() * scale;
    if (width <= 0 || height <= 0)
      return;
    double x = mi.rcWork.right - gap - width;
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
    Wh_Log(L"panelPlacement: %s",
           (std::to_wstring(int(x)) + L"," + std::to_wstring(int(y)) + L" " +
            std::to_wstring(int(width)) + L"x" + std::to_wstring(int(height)))
               .c_str());
  }
}
void QueuePanel(FrameworkElement target, int view);
void ShowMenu(FrameworkElement target);
void SetPanelContent(C::ScrollViewer scroll,
                     FrameworkElement target,
                     int returnView = -1,
                     C::StackPanel actions = nullptr) {
  C::StackPanel host;
  // All settings pages share one work-area-clamped viewport. Content may
  // scroll, but switching tabs or editing presets never resizes the shell.
  double width = 460, height = 410;
  auto presets = ReadPresets();
  auto favorites =
      std::count_if(presets.begin(), presets.end(), [](const Preset& p) { return p.quick; });
  if (favorites > 3)
    height += 28 * (favorites - 3);
  const auto batteries = BatteryControls::ReadSnapshot().state;
  if (batteries.packs[0].present && batteries.packs[1].present)
    height = (std::max)(height, 420.0);
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  if (GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar) &&
      GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi)) {
    double scale = target.XamlRoot().RasterizationScale();
    width = (std::min)(width, (std::max)(160.0, (mi.rcWork.right - mi.rcWork.left) / scale - 80));
    height = (std::min)(height, (std::max)(160.0, (mi.rcWork.bottom - mi.rcWork.top) / scale - 80));
  }
  host.Width(width);
  host.Height(height);
  Style presenter;
  presenter.TargetType(winrt::Windows::UI::Xaml::Interop::TypeName{
      L"Windows.UI.Xaml.Controls.FlyoutPresenter",
      winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata});
  presenter.Setters().Append(
      Setter(C::Control::PaddingProperty(), winrt::box_value(Thickness{24, 20, 24, 20})));
  presenter.Setters().Append(Setter(FrameworkElement::MinWidthProperty(), winrt::box_value(0.0)));
  presenter.Setters().Append(Setter(FrameworkElement::MinHeightProperty(), winrt::box_value(0.0)));
  presenter.Setters().Append(
      Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(width + 50)));
  presenter.Setters().Append(
      Setter(FrameworkElement::MaxHeightProperty(), winrt::box_value(height + 42)));
  flyout.FlyoutPresenterStyle(presenter);
  host.Spacing(20);

  C::StackPanel navigation;
  navigation.Orientation(C::Orientation::Horizontal);
  navigation.Spacing(8);
  navigation.HorizontalAlignment(HorizontalAlignment::Right);
  C::Button back, close;
  // Use a text arrow, keeping the button label in the normal UI font.
  back.Content(
      winrt::box_value(returnView == 0 ? L"\u2190  Charge thresholds" : L"\u2190  Battery menu"));
  back.HorizontalAlignment(HorizontalAlignment::Left);
  close.Content(winrt::box_value(L"\u00d7"));
  close.HorizontalAlignment(HorizontalAlignment::Right);
  Automation::AutomationProperties::SetName(close, L"Close battery controls");
  TrackEvent(back.Click(winrt::auto_revoke, SafeUi([target, returnView](auto const&, auto const&) {
                          QueuePanel(target, returnView);
                        })));
  TrackEvent(close.Click(winrt::auto_revoke, SafeUi([](auto const&, auto const&) { Close(); })));
  navigation.Children().Append(back);
  navigation.Children().Append(close);
  navigation.Height(36);
  scroll.MaxWidth(width);
  scroll.Width(width);
  double footerHeight = actions && width < 450 ? 80 : 36;
  scroll.MaxHeight(height - footerHeight - 12);
  scroll.Height(height - footerHeight - 12);
  scroll.HorizontalContentAlignment(HorizontalAlignment::Stretch);
  scroll.VerticalContentAlignment(VerticalAlignment::Top);
  if (auto page = scroll.Content().try_as<FrameworkElement>()) {
    page.Width((std::max)(120.0, width - 12));
    page.HorizontalAlignment(HorizontalAlignment::Left);
    page.Margin(Thickness{0, 0, 0, 0});
  }
  scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  scroll.HorizontalScrollMode(C::ScrollMode::Disabled);
  scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  host.Spacing(6);
  host.Children().Append(scroll);
  auto more = Text(L"", 12);
  more.Height(18);
  more.Visibility(Visibility::Collapsed);
  auto updateHint = [scroll, more, height, footerHeight]() {
    bool overflow = scroll.ExtentHeight() > height - footerHeight - 12 + 1;
    more.Visibility(overflow ? Visibility::Visible : Visibility::Collapsed);
    double viewport = height - footerHeight - (overflow ? 36 : 12);
    scroll.MaxHeight(viewport);
    scroll.Height(viewport);
    more.Text(scroll.ScrollableHeight() > 1
                  ? (scroll.VerticalOffset() < scroll.ScrollableHeight() - 1
                         ? L"Scroll for more settings \u2193"
                         : L"Scroll up for earlier settings \u2191")
                  : L"");
  };
  TrackEvent(scroll.SizeChanged(winrt::auto_revoke,
                                SafeUi([updateHint](auto const&, auto const&) { updateHint(); })));
  TrackEvent(scroll.ViewChanged(winrt::auto_revoke,
                                SafeUi([updateHint](auto const&, auto const&) { updateHint(); })));
  if (auto page = scroll.Content().try_as<FrameworkElement>())
    TrackEvent(page.SizeChanged(winrt::auto_revoke,
                                SafeUi([updateHint](auto const&, auto const&) { updateHint(); })));
  host.Children().Append(more);
  C::Grid footer;
  footer.Height(footerHeight);
  if (actions) {
    actions.HorizontalAlignment(HorizontalAlignment::Left);
    actions.VerticalAlignment(VerticalAlignment::Top);
    footer.Children().Append(actions);
  }
  navigation.VerticalAlignment(VerticalAlignment::Bottom);
  footer.Children().Append(navigation);
  host.Children().Append(footer);
  flyout.Content(host);
}
void ShowPanelHost(FrameworkElement target) {
  if (!flyout.IsOpen())
    flyout.ShowAt(target);
  else {
    if (auto root = flyout.Content().try_as<FrameworkElement>())
      root.UpdateLayout();
    PositionPanel(target, flyout);
  }
}

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
  editor->scroll.Height((std::min)(140.0, (std::max)(38.0, double(editor->rows.size()) * 38 - 6)));
}
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
  for (double width : {28.0, 40.0, 0.0, 64.0, 64.0, 28.0}) {
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
  Cell(row->grid, row->grip, 5);
  std::weak_ptr<PresetEditor> weak = editor;
  std::weak_ptr<PresetEditorRow> weakRow = row;
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
  if (!preservePanelHost || !flyout)
    flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(460);
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
  for (double width : {28.0, 40.0, 0.0, 64.0, 64.0, 28.0}) {
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
  editor->scroll.Height(140);
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
  SetPanelContent(scroll, target, editor->returnView, actions);
  flyout.Placement(P::FlyoutPlacementMode::TopEdgeAlignedRight);
  TrackEvent(flyout.Opened(
      winrt::auto_revoke, SafeUi([weak, addedCurrent, target](auto const& sender, auto const&) {
        PositionPanel(target, sender.template as<P::FlyoutBase>());
        if (addedCurrent)
          if (auto editor = weak.lock())
            if (!editor->rows.empty()) {
              editor->list.UpdateLayout();
              editor->scroll.ChangeView(nullptr, editor->scroll.ScrollableHeight(), nullptr, true);
              editor->rows.back()->name.Focus(FocusState::Programmatic);
              editor->rows.back()->name.SelectAll();
            }
      })));
  ShowPanelHost(target);
}
// The Advanced panel is rebuilt only on a completed worker request, never on a hardware poll.
int advancedTab = 0, advancedBattery = 0;
void RenderAdvanced(C::StackPanel body, FrameworkElement target) {
  body.Children().Clear();
  body.Spacing(8);
  auto snapshot = BatteryControls::AdvancedControls::Read();
  auto a = snapshot.data;
  if (!snapshot.message.empty())
    body.Children().Append(Text(snapshot.message, 12));
  if (!BatteryControls::IntegratedBackend()) {
    body.Children().Append(
        Text(L"Choose Integrated Lenovo service in Windhawk Settings to use these controls."));
    return;
  }
  if (!a.connected) {
    body.Children().Append(Text(
        L"No verified Lenovo service capabilities are available. Select Refresh to check again."));
    return;
  }
  auto button = [&](PCWSTR label, auto callback) {
    C::Button b;
    b.Content(winrt::box_value(label));
    TrackEvent(b.Click(winrt::auto_revoke, SafeUi(callback)));
    body.Children().Append(b);
    return b;
  };
  if (advancedTab != 1) {
    C::StackPanel selectors;
    selectors.Orientation(C::Orientation::Horizontal);
    selectors.Spacing(8);
    if (!a.batteries[advancedBattery].present)
      advancedBattery = a.batteries[0].present ? 0 : 1;
    for (int i = 0; i < 2; i++)
      if (a.batteries[i].present) {
        P::ToggleButton b;
        b.Content(winrt::box_value(L"Battery " + std::to_wstring(i + 1)));
        b.IsChecked(i == advancedBattery);
        TrackEvent(b.Click(winrt::auto_revoke, SafeUi([target, body, i](auto const&, auto const&) {
                             advancedBattery = i;
                             RenderAdvanced(body, target);
                           })));
        selectors.Children().Append(b);
      }
    if (a.batteries[0].present && a.batteries[1].present)
      body.Children().Append(selectors);
  }
  auto b = a.batteries[advancedBattery];
  if (advancedTab == 0) {
    auto row = [&](PCWSTR label, std::wstring value) {
      C::Grid g;
      C::ColumnDefinition left, right;
      left.Width(GridLength{1, GridUnitType::Star});
      right.Width(GridLength{1, GridUnitType::Auto});
      g.ColumnDefinitions().Append(left);
      g.ColumnDefinitions().Append(right);
      auto l = Text(label), r = Text(value);
      C::Grid::SetColumn(r, 1);
      g.Children().Append(l);
      g.Children().Append(r);
      body.Children().Append(g);
    };
    auto decimal = [](long value) {
      wchar_t text[32];
      swprintf_s(text, L"%.1f", value / 1000.0);
      return std::wstring(text);
    };
    if (b.percent.available)
      row(L"Charge", std::to_wstring(b.percent.value) + L"%");
    if (b.full.available && b.design.available)
      row(L"Full / design capacity",
          decimal(b.full.value) + L" / " + decimal(b.design.value) + L" Wh");
    if (b.full.available && b.design.available)
      row(L"Capacity retained", std::to_wstring((b.full.value * 100LL) / b.design.value) + L"%");
    if (b.cycles.available)
      row(L"Charge cycles", std::to_wstring(b.cycles.value));
    if (b.temperature.available)
      row(L"Temperature", std::to_wstring(b.temperature.value) + L" \u00b0C");
    if (b.voltage.available)
      row(L"Voltage", decimal(b.voltage.value) + L" V");
    if (b.adapter.available)
      row(L"Power adapter", std::to_wstring(b.adapter.value) + L" W");
    body.Children().Append(
        Text(L"Only supported readings are shown. Refresh updates them on demand.", 12));
  } else if (advancedTab == 1) {
    if (a.usbSupported) {
      body.Children().Append(Text(L"Always-on USB", 16));
      body.Children().Append(Text(
          L"Supply power to supported USB ports while the laptop is sleeping or shut down.", 12));
      C::ComboBox choices;
      for (auto label : {L"Off", L"During sleep", L"During sleep and shutdown"})
        choices.Items().Append(winrt::box_value(label));
      choices.SelectedIndex(a.usb);
      choices.HorizontalAlignment(HorizontalAlignment::Stretch);
      body.Children().Append(choices);
      auto apply = button(L"Apply USB setting", [choices](auto const&, auto const&) {
        Send(21, choices.SelectedIndex());
      });
      body.Children().Append(
          Text(L"This firmware setting can remain active after the mod is removed.", 12));
    } else
      body.Children().Append(Text(L"No verified power controls are available on this machine."));
    body.Children().Append(
        Text(L"Cooling and airplane controls are omitted until their model-specific behavior is "
             L"verified.",
             12));
  } else {
    body.Children().Append(Text(L"Battery-gauge reset", 17));
    body.Children().Append(
        Text(L"Recalibrate an inaccurate charge estimate. This does not restore worn battery "
             L"capacity. A full discharge and recharge can take several hours.",
             12));
    if (!b.resetCapable || !a.resetKnown) {
      body.Children().Append(Text(L"Calibration is not available for this battery."));
      return;
    }
    if (a.resetBattery) {
      body.Children().Append(Text(L"Battery " + std::to_wstring(a.resetBattery) +
                                  L" is calibrating. Lenovo reports stage " +
                                  std::to_wstring(a.resetStage) + L" of " +
                                  std::to_wstring(a.resetStages) + L". Keep power connected."));
      button(L"Review cancellation...", [body, target, a](auto const&, auto const&) {
        body.Children().Clear();
        body.Children().Append(
            Text(L"Cancel calibration for Battery " + std::to_wstring(a.resetBattery) + L"?", 18));
        body.Children().Append(
            Text(L"The charge estimate may remain inaccurate. Charging protection will be checked "
                 L"once Lenovo confirms calibration is idle.",
                 12));
        C::Button confirm;
        confirm.Content(winrt::box_value(L"Confirm cancellation"));
        TrackEvent(confirm.Click(winrt::auto_revoke, SafeUi([a, confirm](auto const&, auto const&) {
                                   confirm.IsEnabled(false);
                                   Send(23, a.resetBattery);
                                 })));
        body.Children().Append(confirm);
        C::Button back;
        back.Content(winrt::box_value(L"Back"));
        TrackEvent(back.Click(winrt::auto_revoke, SafeUi([target, body](auto const&, auto const&) {
                                RenderAdvanced(body, target);
                              })));
        body.Children().Append(back);
      });
    } else {
      button(L"Review and start...", [body, target, slot = advancedBattery + 1](auto const&,
                                                                                auto const&) {
        body.Children().Clear();
        body.Children().Append(Text(L"Calibrate Battery " + std::to_wstring(slot) + L"?", 18));
        body.Children().Append(Text(
            L"Save your work and leave the laptop plugged in for several hours. Lenovo controls "
            L"the discharge and recharge cycle. Charging edits are blocked while it runs.",
            12));
        body.Children().Append(
            Text(L"Current limits are saved first. When calibration ends, disabled protection is "
                 L"restored; any newer enabled limits are kept. If the mod is removed, check "
                 L"calibration and limits in Vantage.",
                 12));
        C::CheckBox consent;
        consent.Content(winrt::box_value(L"My work is saved and I can leave power connected."));
        body.Children().Append(consent);
        C::Button start;
        start.Content(winrt::box_value(L"Start calibration"));
        start.IsEnabled(false);
        TrackEvent(consent.Checked(winrt::auto_revoke, SafeUi([start](auto const&, auto const&) {
                                     start.IsEnabled(true);
                                   })));
        TrackEvent(consent.Unchecked(winrt::auto_revoke, SafeUi([start](auto const&, auto const&) {
                                       start.IsEnabled(false);
                                     })));
        TrackEvent(start.Click(winrt::auto_revoke,
                               SafeUi([start, consent, slot](auto const&, auto const&) {
                                 if (consent.IsChecked().Value()) {
                                   start.IsEnabled(false);
                                   Send(22, slot);
                                 }
                               })));
        body.Children().Append(start);
        C::Button cancel;
        cancel.Content(winrt::box_value(L"Cancel"));
        TrackEvent(cancel.Click(
            winrt::auto_revoke,
            SafeUi([target, body](auto const&, auto const&) { RenderAdvanced(body, target); })));
        body.Children().Append(cancel);
      });
    }
  }
}
void PresentContent(FrameworkElement target,
                    C::StackPanel content,
                    C::StackPanel actions = nullptr) {
  if (!preservePanelHost || !flyout)
    flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::ScrollViewer scroll;
  scroll.Content(content);
  scroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  scroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
  RECT bar{};
  MONITORINFO mi{sizeof(mi)};
  GetWindowRect(FindWindowW(L"Shell_TrayWnd", nullptr), &bar);
  if (GetMonitorInfoW(MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST), &mi)) {
    double scale = target.XamlRoot().RasterizationScale();
    scroll.MaxHeight((std::max)(120.0, (mi.rcWork.bottom - mi.rcWork.top) / scale - 64));
    content.Width((std::min)(390.0, (mi.rcWork.right - mi.rcWork.left) / scale - 64));
  }
  SetPanelContent(scroll, target, -1, actions);
  flyout.Placement(P::FlyoutPlacementMode::Top);
  TrackEvent(flyout.Opened(winrt::auto_revoke, SafeUi([weak = winrt::make_weak(target)](
                                                          auto const& sender, auto const&) {
                             if (auto t = weak.get())
                               PositionPanel(t, sender.template as<C::Flyout>());
                           })));
  ShowPanelHost(target);
}
void ShowAdvanced(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  C::StackPanel content;
  content.Width(390);
  content.Spacing(8);
  content.Children().Append(Text(L"Advanced power controls", 22));
  C::StackPanel body;
  body.Spacing(12);
  C::StackPanel tabs;
  tabs.Orientation(C::Orientation::Horizontal);
  tabs.Spacing(6);
  int index = 0;
  std::vector<PCWSTR> labels = {L"Battery health", L"Power", L"Calibration"};
  if (advancedTab >= int(labels.size()))
    advancedTab = 0;
  for (auto label : labels) {
    P::ToggleButton tab;
    tab.Content(winrt::box_value(label));
    tab.IsChecked(index == advancedTab);
    TrackEvent(tab.Click(
        winrt::auto_revoke, SafeUi([target, body, tabs, index](auto const&, auto const&) {
          // Tabs are mutually exclusive; clicking the active tab cannot turn it off.
          for (unsigned i = 0; i < tabs.Children().Size(); ++i)
            tabs.Children().GetAt(i).as<P::ToggleButton>().IsChecked(i == unsigned(index));
          advancedTab = index;
          RenderAdvanced(body, target);
        })));
    tabs.Children().Append(tab);
    index++;
  }
  content.Children().Append(tabs);
  content.Children().Append(body);
  C::Button refresh;
  refresh.Content(winrt::box_value(L"Refresh"));
  TrackEvent(refresh.Click(winrt::auto_revoke, SafeUi([body](auto const&, auto const&) {
                             body.Children().Clear();
                             body.Children().Append(Text(L"Checking supported features..."));
                             Send(20);
                           })));
  body.Children().Append(Text(L"Checking supported features..."));
  auto revision = std::make_shared<ULONGLONG>(BatteryControls::AdvancedControls::Read().revision);
  statusTimer = DispatcherTimer();
  statusTimer.Interval(std::chrono::milliseconds(200));
  TrackEvent(statusTimer.Tick(winrt::auto_revoke,
                              SafeUi([body, target, revision](auto const&, auto const&) {
                                auto next = BatteryControls::AdvancedControls::Read().revision;
                                if (next != *revision) {
                                  *revision = next;
                                  RenderAdvanced(body, target);
                                }
                              })));
  statusTimer.Start();
  C::StackPanel actions;
  actions.Children().Append(refresh);
  PresentContent(target, content, actions);
  TrackEvent(flyout.Closed(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                             if (statusTimer)
                               statusTimer.Stop();
                           })));
  Send(20);
}
void ApplyInPlace(C::Button button, C::TextBlock feedback, int action, int values) {
  bool expected = false;
  if (!BatteryControls::applyPending.compare_exchange_strong(expected, true)) {
    feedback.Text(L"Previous changes are still being applied. Please wait.");
    return;
  }
  auto revision = BatteryControls::ReadApplyResult().revision;
  if (!BatteryControls::Submit(action, values)) {
    BatteryControls::applyPending = false;
    feedback.Text(
        L"Could not queue changes. Please try again when the current operation finishes.");
    return;
  }
  button.IsEnabled(false);
  feedback.Opacity(1);
  feedback.Text(L"Applying changes...");
  if (statusTimer)
    statusTimer.Stop();
  statusTimer = DispatcherTimer();
  statusTimer.Interval(std::chrono::milliseconds(200));
  TrackEvent(statusTimer.Tick(winrt::auto_revoke,
                              SafeUi([button, feedback, revision](auto const&, auto const&) {
                                auto result = BatteryControls::ReadApplyResult();
                                if (result.revision != revision) {
                                  feedback.Text(result.message);
                                  button.IsEnabled(true);
                                  statusTimer.Stop();
                                }
                              })));
  statusTimer.Start();
}
struct DualDraft {
  bool valid = false, linked = false;
  int selected = 0;
  BatteryControls::Pack packs[2]{};
};
DualDraft dualDraft;
void ShowDualPanel(FrameworkElement target) {
  Close();
  auto state = BatteryControls::ReadSnapshot().state;
  if (!dualDraft.valid) {
    dualDraft.valid = true;
    dualDraft.selected = 0;
    dualDraft.linked = !state.mixed;
    std::copy(std::begin(state.packs), std::end(state.packs), dualDraft.packs);
  }
  C::StackPanel content;
  content.Width(390);
  content.Spacing(12);
  content.Children().Append(Text(L"Battery protection", 22));
  content.Children().Append(Text(L"Choose each battery's range. Apply saves both.", 12));
  if (!BatteryControls::IntegratedBackend()) {
    content.Children().Append(
        Text(L"Separate battery ranges require Integrated Lenovo service. Select it in Windhawk "
             L"Settings."));
    PresentContent(target, content);
    return;
  }
  C::ToggleSwitch linked;
  linked.Header(winrt::box_value(L"Use the same limits"));
  linked.IsOn(dualDraft.linked);
  content.Children().Append(linked);
  C::StackPanel cards;
  cards.Orientation(C::Orientation::Horizontal);
  cards.Spacing(8);
  P::ToggleButton first, second;
  first.Width(180);
  second.Width(180);
  cards.Children().Append(first);
  cards.Children().Append(second);
  content.Children().Append(cards);
  C::TextBlock editing;
  content.Children().Append(editing);
  C::ToggleSwitch enabled;
  enabled.Header(winrt::box_value(L"Limit charging"));
  content.Children().Append(enabled);
  C::TextBlock startLabel, stopLabel, summary;
  C::Slider start, stop;
  start.Minimum(5);
  start.Maximum(99);
  stop.Minimum(6);
  stop.Maximum(100);
  start.StepFrequency(1);
  stop.StepFrequency(1);
  Automation::AutomationProperties::SetName(start, L"Start charging below percentage");
  Automation::AutomationProperties::SetName(stop, L"Stop charging at percentage");
  auto adjusting = std::make_shared<bool>(false);
  auto selections = std::make_shared<std::vector<std::tuple<P::ToggleButton, int, int>>>();
  auto render = [=]() {
    *adjusting = true;
    auto p = dualDraft.packs[dualDraft.selected];
    enabled.IsOn(p.enabled);
    start.Value(BatteryControls::ValidRange(p.start, p.stop) ? p.start : 75);
    stop.Value(BatteryControls::ValidRange(p.start, p.stop) ? p.stop : 80);
    auto range = [](auto p) {
      return p.enabled ? std::to_wstring(p.start) + L" to " + std::to_wstring(p.stop) + L"%"
                       : L"Limits off";
    };
    first.Content(winrt::box_value(L"Battery 1\n" + range(dualDraft.packs[0])));
    second.Content(winrt::box_value(L"Battery 2\n" + range(dualDraft.packs[1])));
    first.IsChecked(dualDraft.selected == 0);
    second.IsChecked(dualDraft.selected == 1);
    editing.Text(dualDraft.linked ? L"Both batteries"
                                  : L"Battery " + std::to_wstring(dualDraft.selected + 1));
    startLabel.Text(L"Start below " + std::to_wstring(int(start.Value())) + L"%");
    stopLabel.Text(L"Stop at " + std::to_wstring(int(stop.Value())) + L"%");
    summary.Text(L"Battery 1: " + range(dualDraft.packs[0]) + L"\nBattery 2: " +
                 range(dualDraft.packs[1]));
    for (auto& [button, a, b] : *selections)
      button.IsChecked(p.enabled && p.start == a && p.stop == b);
    *adjusting = false;
  };
  auto update = [=](bool changedStart) {
    if (*adjusting)
      return;
    int a = int(start.Value()), b = int(stop.Value());
    if (a >= b) {
      if (changedStart)
        b = a + 1;
      else
        a = b - 1;
    }
    auto& p = dualDraft.packs[dualDraft.selected];
    p = {1, DWORD(enabled.IsOn()), a, b};
    if (dualDraft.linked)
      dualDraft.packs[1 - dualDraft.selected] = p;
    render();
  };
  TrackEvent(first.Click(winrt::auto_revoke, SafeUi([render](auto const&, auto const&) {
                           dualDraft.selected = 0;
                           render();
                         })));
  TrackEvent(second.Click(winrt::auto_revoke, SafeUi([render](auto const&, auto const&) {
                            dualDraft.selected = 1;
                            render();
                          })));
  TrackEvent(linked.Toggled(winrt::auto_revoke, SafeUi([linked, render](auto const&, auto const&) {
                              dualDraft.linked = linked.IsOn();
                              if (dualDraft.linked)
                                dualDraft.packs[1 - dualDraft.selected] =
                                    dualDraft.packs[dualDraft.selected];
                              render();
                            })));
  C::StackPanel choices;
  choices.Spacing(4);
  for (auto preset : ReadPresets())
    if (preset.quick) {
      P::ToggleButton button;
      selections->emplace_back(button, preset.start, preset.stop);
      button.Content(PresetDisplay(preset));
      button.HorizontalAlignment(HorizontalAlignment::Stretch);
      button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
      TrackEvent(button.Click(
          winrt::auto_revoke, SafeUi([preset, render](auto const&, auto const&) {
            dualDraft.packs[dualDraft.selected] = {1, 1, preset.start, preset.stop};
            if (dualDraft.linked)
              dualDraft.packs[1 - dualDraft.selected] = dualDraft.packs[dualDraft.selected];
            render();
          })));
      choices.Children().Append(button);
    }
  C::ScrollViewer presets;
  presets.Content(choices);
  presets.MaxHeight(150);
  presets.VerticalScrollBarVisibility(C::ScrollBarVisibility::Auto);
  content.Children().Append(presets);
  content.Children().Append(startLabel);
  content.Children().Append(start);
  content.Children().Append(stopLabel);
  content.Children().Append(stop);
  TrackEvent(start.ValueChanged(winrt::auto_revoke,
                                SafeUi([update](auto const&, auto const&) { update(true); })));
  TrackEvent(stop.ValueChanged(winrt::auto_revoke,
                               SafeUi([update](auto const&, auto const&) { update(false); })));
  TrackEvent(enabled.Toggled(winrt::auto_revoke,
                             SafeUi([update](auto const&, auto const&) { update(true); })));
  render();
  C::Button manage;
  manage.Content(winrt::box_value(L"Manage presets..."));
  TrackEvent(manage.Click(winrt::auto_revoke, SafeUi([target](auto const&, auto const&) {
                            presetReturnView = 0;
                            QueuePanel(target, 2);
                          })));
  content.Children().Append(manage);
  C::Button save;
  save.Content(winrt::box_value(L"+ Save selected range as preset..."));
  save.IsEnabled(ReadPresets().size() < maxPresets);
  TrackEvent(save.Click(winrt::auto_revoke, SafeUi([target](auto const&, auto const&) {
                          auto p = dualDraft.packs[dualDraft.selected];
                          addPresetDraft = {true, bool(p.enabled), p.start, p.stop};
                          presetReturnView = 0;
                          QueuePanel(target, 2);
                        })));
  content.Children().Append(save);
  summary.FontSize(12);
  content.Children().Append(summary);
  content.Children().Append(
      Text(L"Above the limit? The level may fall slowly while plugged in. Unplug to reach your "
           L"selected range sooner.",
           12));
  C::StackPanel actions;
  actions.Orientation(C::Orientation::Horizontal);
  actions.Spacing(8);
  C::Button apply, cancel;
  apply.Content(winrt::box_value(L"Apply both ranges"));
  cancel.Content(winrt::box_value(L"Close"));
  TrackEvent(apply.Click(winrt::auto_revoke, SafeUi([apply, summary](auto const&, auto const&) {
                           unsigned value = 0;
                           for (int i = 0; i < 2; i++) {
                             auto p = dualDraft.packs[i];
                             if (p.enabled)
                               value |= (unsigned(p.start) | (unsigned(p.stop) << 8)) << (i * 16);
                           }
                           ApplyInPlace(apply, summary, 34, int(value));
                         })));
  TrackEvent(cancel.Click(winrt::auto_revoke, SafeUi([](auto const&, auto const&) {
                            dualDraft.valid = false;
                            flyout.Hide();
                          })));
  actions.Children().Append(apply);
  actions.Children().Append(cancel);
  PresentContent(target, content, actions);
}

void ShowPanel(FrameworkElement target) {
  auto dualState = BatteryControls::ReadSnapshot().state;
  if (dualState.packs[0].present && dualState.packs[1].present) {
    ShowDualPanel(target);
    return;
  }
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  auto initial = BatteryControls::ReadSnapshot().state;
  panel = std::make_shared<Panel>();
  auto p = panel;
  std::weak_ptr<Panel> weak = p;
  if (!preservePanelHost || !flyout)
    flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(340);
  content.Spacing(6);
  auto title = Text(L"Battery protection", 22);
  title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
  C::Grid header;
  header.Children().Append(title);
  title.VerticalAlignment(VerticalAlignment::Center);
  auto batteryState = BatteryControls::ReadSnapshot().state;
  if (batteryState.packs[0].present && batteryState.packs[1].present)
    content.Children().Append(Text(
        L"Changes apply to both batteries. Charge to full saves each battery's limits separately.",
        12));
  if (batteryState.mixed) {
    for (int i = 0; i < 2; i++)
      if (batteryState.packs[i].present) {
        auto& pack = batteryState.packs[i];
        content.Children().Append(Text(L"Battery " + std::to_wstring(i + 1) + L": " +
                                           (pack.enabled ? std::to_wstring(pack.start) + L" to " +
                                                               std::to_wstring(pack.stop) + L"%"
                                                         : L"limits off"),
                                       12));
      }
  }
  p->enabled.OnContent(winrt::box_value(L"Limit charging"));
  p->enabled.OffContent(winrt::box_value(L"Limits off"));
  p->enabled.HorizontalAlignment(HorizontalAlignment::Right);
  Automation::AutomationProperties::SetName(p->enabled, L"Limit charging");
  p->enabled.IsOn(initial.enabled || BatteryControls::Active());
  header.Children().Append(p->enabled);
  content.Children().Append(header);
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
  choices.Spacing(3);
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
    button.Padding(Thickness{10, 2, 10, 2});
    button.MinHeight(0);
    button.Height(28);
    Automation::AutomationProperties::SetName(button, PresetLabel(preset));
    TrackEvent(button.Click(winrt::auto_revoke, SafeUi([choose, preset](auto const&, auto const&) {
                              choose(preset);
                            })));
    p->choices.emplace_back(button, preset.start, preset.stop);
    choices.Children().Append(button);
  }
  content.Children().Append(choices);
  C::Button manage;
  manage.Content(winrt::box_value(L"Manage presets..."));
  TrackEvent(manage.Click(winrt::auto_revoke, SafeUi([target, weak](auto const&, auto const&) {
                            if (auto p = weak.lock())
                              returningDraft = {true, p->enabled.IsOn(), int(p->start.Value()),
                                                int(p->stop.Value())};
                            presetReturnView = 0;
                            QueuePanel(target, 2);
                          })));

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
  C::StackPanel ranges;
  ranges.Spacing(8);
  C::StackPanel startRange, stopRange;
  startRange.Children().Append(p->startLabel);
  startRange.Children().Append(p->start);
  stopRange.Children().Append(p->stopLabel);
  stopRange.Children().Append(p->stop);
  p->start.Height(28);
  p->stop.Height(28);
  ranges.Children().Append(startRange);
  ranges.Children().Append(stopRange);
  C::Border rangeGroup;
  rangeGroup.BorderThickness(Thickness{0, 1, 0, 1});
  rangeGroup.Padding(Thickness{0, 8, 0, 8});
  rangeGroup.BorderBrush(Media::SolidColorBrush(winrt::Windows::UI::Colors::Gray()));
  rangeGroup.Child(ranges);
  content.Children().Append(rangeGroup);
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
  C::StackPanel presetTools;
  presetTools.Orientation(C::Orientation::Horizontal);
  presetTools.Spacing(8);
  presetTools.Children().Append(manage);
  presetTools.Children().Append(saveCurrent);
  content.Children().Append(presetTools);
  auto hint = Text(L"Above your limit? Charging pauses. Unplug to reach your range sooner.", 12);
  hint.Opacity(.75);
  if (BatteryControls::Active())
    hint.Text(L"Applying these limits ends the temporary full-charge override.");
  content.Children().Append(hint);
  C::StackPanel actions;
  actions.Orientation(C::Orientation::Horizontal);
  actions.Spacing(8);
  C::Button apply;
  apply.Content(winrt::box_value(L"Apply changes"));
  TrackEvent(apply.Click(winrt::auto_revoke, SafeUi([weak, apply, hint](auto const&, auto const&) {
                           if (auto p = weak.lock()) {
                             ApplyInPlace(apply, hint, p->enabled.IsOn() ? 31 : 32,
                                          MAKELONG(int(p->start.Value()), int(p->stop.Value())));
                           }
                         })));
  C::Button cancel;
  cancel.Content(winrt::box_value(L"Close"));
  TrackEvent(
      cancel.Click(winrt::auto_revoke, SafeUi([](auto const&, auto const&) { flyout.Hide(); })));
  actions.Children().Append(apply);
  actions.Children().Append(cancel);
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
  SetPanelContent(scroll, target, -1, actions);
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
  ShowPanelHost(target);
}
std::wstring MachineValue(PCWSTR name) {
  wchar_t text[256]{};
  DWORD bytes = sizeof(text);
  if (RegGetValueW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", name, RRF_RT_REG_SZ,
                   nullptr, text, &bytes) != ERROR_SUCCESS)
    return L"Unknown";
  return text;
}
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
void ShowSetup(FrameworkElement target) {
  if (unloading || !target || !target.XamlRoot())
    return;
  Close();
  if (!preservePanelHost || !flyout)
    flyout = C::Flyout();
  flyout.ShouldConstrainToRootBounds(false);
  C::StackPanel content;
  content.Width(340);
  content.Spacing(8);
  C::StackPanel status, help;
  status.Spacing(8);
  help.Spacing(6);
  help.Visibility(Visibility::Collapsed);
  auto title = Text(L"Setup and diagnostics", 22);
  content.Children().Append(title);
  C::StackPanel tabs;
  tabs.Orientation(C::Orientation::Horizontal);
  tabs.Spacing(6);
  P::ToggleButton statusTab, helpTab;
  statusTab.Content(winrt::box_value(L"Status"));
  helpTab.Content(winrt::box_value(L"Downloads and help"));
  statusTab.IsChecked(true);
  auto select = [status, help, statusTab, helpTab](bool showHelp) {
    status.Visibility(showHelp ? Visibility::Collapsed : Visibility::Visible);
    help.Visibility(showHelp ? Visibility::Visible : Visibility::Collapsed);
    statusTab.IsChecked(!showHelp);
    helpTab.IsChecked(showHelp);
  };
  TrackEvent(statusTab.Click(winrt::auto_revoke,
                             SafeUi([select](auto const&, auto const&) { select(false); })));
  TrackEvent(helpTab.Click(winrt::auto_revoke,
                           SafeUi([select](auto const&, auto const&) { select(true); })));
  tabs.Children().Append(statusTab);
  tabs.Children().Append(helpTab);
  content.Children().Append(tabs);
  content.Children().Append(status);
  content.Children().Append(help);
  auto recent = BatteryControls::RecentStatus();
  if (!recent.empty())
    status.Children().Append(Text(recent, 12));
  auto maker = MachineValue(L"SystemManufacturer"), model = MachineValue(L"SystemProductName"),
       family = MachineValue(L"SystemFamily");
  status.Children().Append(Text(maker + L" " + family + L"\nModel: " + model));
  bool lenovo = _wcsicmp(maker.c_str(), L"LENOVO") == 0;
  if (!lenovo)
    status.Children().Append(
        Text(L"A Lenovo manufacturer could not be confirmed. This mod requires compatible Lenovo "
             L"hardware."));
  bool integrated = BatteryControls::IntegratedBackend();
  status.Children().Append(
      Text(integrated ? L"Backend: integrated Lenovo service. No utility download is needed."
                      : L"Backend: Lenovo ChargeThreshold.exe."));
  status.Children().Append(
      Text(L"Choose the charging backend in this mod's Windhawk Settings.", 12));
  if (!integrated) {
    help.Children().Append(
        Text(L"Download Lenovo's utility using the link below, save it in a folder of your choice, "
             L"and set its full path in this mod's Windhawk Settings.",
             12));
    auto path = BatteryControls::UtilityPath();
    help.Children().Append(
        Text(path.empty() ? L"Utility path: not configured." : L"Utility path: " + path, 12));
    help.Children().Append(Text(BatteryControls::ReadSnapshot().utility == 4
                                    ? L"Utility: verified."
                                    : L"Utility has not been verified. Check the path and "
                                      L"download the linked Lenovo version.",
                                12));
  }
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
  status.Children().Append(Text(service));
  auto current = BatteryControls::ReadSnapshot().state;
  for (int i = 0; i < 2; i++)
    if (current.packs[i].present) {
      const auto& pack = current.packs[i];
      status.Children().Append(Text(L"Battery " + std::to_wstring(i + 1) + L": " +
                                        (pack.enabled ? std::to_wstring(pack.start) + L" to " +
                                                            std::to_wstring(pack.stop) + L"%"
                                                      : L"limits off"),
                                    12));
    }
  bool valid = (BatteryControls::ReadSnapshot().state.valid && !BatteryControls::refreshing);
  status.Children().Append(Text(
      valid ? L"Battery communication: working."
            : L"Battery communication: unavailable. This can mean missing drivers, a stopped "
              L"component, an unsupported battery, or a communication error. Service detection "
              L"alone does not prove compatibility."));
  auto link = [&](PCWSTR label, PCWSTR url) {
    C::HyperlinkButton b;
    b.Content(winrt::box_value(label));
    b.NavigateUri(winrt::Windows::Foundation::Uri(url));
    help.Children().Append(b);
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
    help.Children().Append(
        Text(L"Lenovo selects the driver page from your machine type when you open this link. Only "
             L"the four-character machine type is included, not your serial number.",
             12));
  } else
    help.Children().Append(Text(
        L"The machine type could not be identified. Use Lenovo Support below to select your model.",
        12));
  link(L"Find your model on Lenovo Support", L"https://pcsupport.lenovo.com/");
  help.Children().Append(
      Text(L"Check supported models before installing a driver. If Lenovo cannot resolve the link, "
           L"select your model on its support site. Download the utility manually and set its path "
           L"in Windhawk Settings. "
           L"Drivers must be "
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
  SetPanelContent(scroll, target);
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
  ShowPanelHost(target);
}
void QueuePanel(FrameworkElement target, int view) {
  if (unloading || !target || !target.XamlRoot())
    return;
  preservePanelHost = flyout && flyout.IsOpen() && view != -1;
  try {
    if (view == -1)
      ShowMenu(target);
    else if (view == 4)
      ShowAdvanced(target);
    else if (view == 2)
      ShowPresetEditor(target);
    else if (view == 1)
      ShowSetup(target);
    else
      ShowPanel(target);
  } catch (...) {
    preservePanelHost = false;
    Close();
    throw;
  }
  preservePanelHost = false;
}
void MarkApplied(C::MenuFlyoutItem item, const Preset& preset) {
  if (MatchesPreset(preset,
                    (BatteryControls::ReadSnapshot().state.valid && !BatteryControls::refreshing),
                    BatteryControls::ReadSnapshot().state.enabled, BatteryControls::Active(),
                    BatteryControls::ReadSnapshot().state.start,
                    BatteryControls::ReadSnapshot().state.stop)) {
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
  bool valid = (BatteryControls::ReadSnapshot().state.valid && !BatteryControls::refreshing),
       enabled = BatteryControls::ReadSnapshot().state.enabled, active = BatteryControls::Active();
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
      active                                        ? L"Charge to 100% (temporary)"
      : !valid                                      ? L"Charging status unavailable"
      : BatteryControls::ReadSnapshot().state.mixed ? L"Different battery limits"
      : enabled ? L"Charge: " + std::to_wstring(BatteryControls::ReadSnapshot().state.start) +
                      L" to " + std::to_wstring(BatteryControls::ReadSnapshot().state.stop) + L"%"
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
                                   presetReturnView = -1;
                                   QueuePanel(target, 2);
                                 })));
  quick.Items().Append(managePresets);
  menu.Items().Append(quick);
  menu.Items().Append(C::MenuFlyoutSeparator());
  auto threshold =
      add(L"Set charge threshold...", L'\uE9E9', valid, [target](auto const&, auto const&) {
        dualDraft.valid = false;
        QueuePanel(target, false);
      });
  add(L"Advanced power controls...", L'\uE9D9', true,
      [target](auto const&, auto const&) { QueuePanel(target, 4); });
  add(L"Windows battery settings", L'\uE8A5', true, [](auto const&, auto const&) { Send(4); });
  auto update = [header, full, threshold, quickItems, fullLabel = std::wstring(fullLabel)]() {
    auto snap = BatteryControls::ReadSnapshot();
    bool waiting = BatteryControls::refreshing, valid = snap.state.valid && !waiting,
         active = BatteryControls::Active();
    std::wstring text = waiting              ? L"Checking charging..."
                        : active             ? L"Charge to 100% (temporary)"
                        : !valid             ? L"Charging status unavailable"
                        : snap.state.mixed   ? L"Different battery limits"
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
  menu.ShouldConstrainToRootBounds(false);
  TrackEvent(menu.Opened(winrt::auto_revoke, SafeUi([target](auto const& sender, auto const&) {
                           PositionPanel(target, sender.template as<P::FlyoutBase>());
                         })));
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
                                  Wh_Log(L"uiError: %s", e.message().c_str());
                                  Close();
                                }
                              })));
  pendingMenu.Start();
}
}  // namespace ModernUI

struct NativeBinding {
  winrt::weak_ref<UIElement> element;
  winrt::event_token token;
  winrt::Windows::Foundation::IInspectable rightHandler, keyHandler;
};
[[clang::no_destroy]] std::optional<std::vector<NativeBinding>> bindings{std::in_place};
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
    for (auto& binding : *bindings)
      if (binding.element.get() == element)
        return;
    auto lastRequest = std::make_shared<ULONGLONG>(0);
    auto show = [lastRequest](FrameworkElement target) {
      if (unloading)
        return;
      auto now = GetTickCount64();
      if (now - *lastRequest < 120)
        return;
      *lastRequest = now;
      ModernUI::QueueMenu(target);
    };
    winrt::Windows::Foundation::TypedEventHandler<UIElement, Input::ContextRequestedEventArgs>
        context = [show](auto const& sender, Input::ContextRequestedEventArgs const& args) {
          if (unloading)
            return;
          try {
            show(sender.template as<FrameworkElement>());
            args.Handled(true);
          } catch (...) {
            BatteryControls::Status(L"Unable to display battery controls.");
          }
        };
    // Explorer can mark pointer/key routed events handled before ContextRequested fires.
    // Keep handled-events-too routes alongside the accessibility context request.
    Input::RightTappedEventHandler right = [show](auto const& sender, auto const& args) {
      if (unloading)
        return;
      try {
        show(sender.template as<FrameworkElement>());
        args.Handled(true);
      } catch (...) {
        BatteryControls::Status(L"Unable to display battery controls.");
      }
    };
    Input::KeyEventHandler key = [show](auto const& sender, auto const& args) {
      if (unloading)
        return;
      int k = int(args.Key());
      if (k != VK_APPS && !(k == VK_F10 && (GetKeyState(VK_SHIFT) & 0x8000)))
        return;
      try {
        show(sender.template as<FrameworkElement>());
        args.Handled(true);
      } catch (...) {
        BatteryControls::Status(L"Unable to display battery controls.");
      }
    };
    auto token = element.ContextRequested(context);
    auto rightBox = winrt::box_value(right), keyBox = winrt::box_value(key);
    element.AddHandler(UIElement::RightTappedEvent(), rightBox, true);
    element.AddHandler(UIElement::KeyDownEvent(), keyBox, true);
    bindings->push_back({winrt::make_weak(element.as<UIElement>()), token, rightBox, keyBox});
    Wh_Log(L"Native battery context menu attached");
    Wh_Log(L"nativeAttached=%d", int(1));
    Wh_Log(L"attachedVersion: %s", WH_MOD_VERSION);
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
          for (auto& binding : *bindings)
            if (auto e = binding.element.get())
              if (auto f = e.try_as<FrameworkElement>())
                if (f.XamlRoot())
                  return;
          for (auto& binding : *bindings)
            if (auto e = binding.element.get()) {
              e.ContextRequested(binding.token);
              e.RemoveHandler(UIElement::RightTappedEvent(), binding.rightHandler);
              e.RemoveHandler(UIElement::KeyDownEvent(), binding.keyHandler);
            }
          bindings->clear();
          auto root = GetTaskbarXamlRoot(taskbar.load());
          if (root)
            Scan(root.Content());
        } catch (...) {
          BatteryControls::Status(L"Taskbar integration unavailable.");
        }
      },
      nullptr);
}
using IconViewConstructor = void*(WINAPI*)(void*);
IconViewConstructor iconViewConstructorOriginal = nullptr;
[[clang::no_destroy]] std::optional<std::list<FrameworkElement::Loaded_revoker>> iconLoadedEvents{
    std::in_place};
void* WINAPI IconViewConstructorHook(void* self) {
  void* result = iconViewConstructorOriginal(self);
  if (unloading)
    return result;
  try {
    FrameworkElement view = nullptr;
    reinterpret_cast<IUnknown**>(self)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                                          winrt::put_abi(view));
    if (!view)
      return result;
    auto item = iconLoadedEvents->emplace(iconLoadedEvents->end());
    *item = view.Loaded(winrt::auto_revoke, [item](auto const& sender, auto const&) noexcept {
      iconLoadedEvents->erase(item);
      if (unloading)
        return;
      try {
        taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        Scan(sender.template as<DependencyObject>());
      } catch (...) {
        Wh_Log(L"Unable to attach newly loaded tray icon");
      }
    });
  } catch (...) {
    Wh_Log(L"Unable to watch tray icon creation");
  }
  return result;
}
std::atomic<bool> trayHooked = false;
HMODULE TrayModule() {
  auto module = GetModuleHandleW(L"SystemTray.dll");
  if (!module)
    module = GetModuleHandleW(L"Taskbar.View.dll");
  return module;
}
bool HookTray(HMODULE module) {
  // SystemTray.dll, Taskbar.View.dll
  WindhawkUtils::SYMBOL_HOOK hooks[] = {
      {{LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
       &iconViewConstructorOriginal,
       IconViewConstructorHook}};
  return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}
decltype(&LoadLibraryExW) loadLibraryOriginal = nullptr;
HMODULE WINAPI LoadLibraryHook(PCWSTR name, HANDLE file, DWORD flags) {
  HMODULE module = loadLibraryOriginal(name, file, flags);
  if (!unloading && module && module == TrayModule() && !trayHooked.exchange(true)) {
    if (HookTray(module))
      Wh_ApplyHookOperations();
    else
      Wh_Log(L"Tray creation hook unavailable");
  }
  return module;
}
BOOL Wh_ModInit() {
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
      Wh_Log(L"Taskbar symbols unavailable; recovery worker remains active.");
    auto tray = TrayModule();
    if (tray) {
      trayHooked = true;
      if (!HookTray(tray))
        Wh_Log(L"Tray creation hook unavailable");
    } else {
      auto load = reinterpret_cast<decltype(&LoadLibraryExW)>(
          GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW"));
      if (!WindhawkUtils::SetFunctionHook(load, LoadLibraryHook, &loadLibraryOriginal))
        return FALSE;
    }
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
void Wh_ModAfterInit() {
  if (!trayHooked)
    if (auto tray = TrayModule())
      if (!trayHooked.exchange(true)) {
        if (HookTray(tray))
          Wh_ApplyHookOperations();
      }
  AttachNativeMenu();
}
void Wh_ModSettingsChanged() {
  BatteryControls::Submit(0);
}
void Wh_ModBeforeUninit() {
  unloading = true;
  using namespace BatteryControls;
  shutdownDeadline = GetTickCount64() + 4000;
  if (stopEvent)
    SetEvent(stopEvent);
  if (threadHandle)
    WaitForSingleObject(threadHandle, INFINITE);
  auto cleanup = [](void*) {
    ModernUI::Close();
    ModernUI::uiRevokers.reset();
    for (auto& binding : *bindings)
      if (auto element = binding.element.get()) {
        element.ContextRequested(binding.token);
        element.RemoveHandler(UIElement::RightTappedEvent(), binding.rightHandler);
        element.RemoveHandler(UIElement::KeyDownEvent(), binding.keyHandler);
      }
    bindings.reset();
    iconLoadedEvents.reset();
  };
  if (taskbar)
    RunFromWindowThread(taskbar, cleanup, nullptr);
}
void Wh_ModUninit() {
  using namespace BatteryControls;
  // UI handlers and worker are gone; wait for delegates already being released by XAML.
  while (winrt::get_module_lock() != 0)
    Sleep(10);
  if (threadHandle)
    CloseHandle(threadHandle);
  if (stopEvent)
    CloseHandle(stopEvent);
  if (wakeEvent)
    CloseHandle(wakeEvent);
  threadHandle = stopEvent = wakeEvent = nullptr;
}
