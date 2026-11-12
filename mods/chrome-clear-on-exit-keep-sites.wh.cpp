// ==WindhawkMod==
// @id              chrome-clear-on-exit-keep-sites
// @name            Chrome clear on exit with keep sites
// @description     Configures Chrome's native exit cleanup while retaining selected website logins.
// @version         0.3.0
// @author          haniobaid
// @github          https://github.com/haniobaid
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -luser32 -lshell32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome clear on exit with keep sites

This mod configures Chrome's native Windows policies. Chrome performs the
cleanup; the mod does not inject code into Chrome or edit its profile databases.
It runs in a dedicated Windhawk tool process and requires no Windows/Chrome debug
symbols. The embedded tool-host compatibility code supports Windhawk 1.7.3 and 2.0.

**Automatic cleanup is enabled by default.** Before closing Chrome, use this mod's
**Settings** tab to review the keep list, initially `chatgpt.com` and `openai.com`.
Chrome applies the cleanup to every Chrome profile for this Windows user. Removed
browsing data cannot be recovered by disabling the mod.

## Installation and configuration
Install the mod with Windhawk, then approve the Windows administrator prompt to
apply its Chrome policies. In the mod's **Settings** tab, edit the keep list and
save your changes; changing that list can request administrator approval again.

For manual installation from this source file, choose **Create a new mod** in
Windhawk, replace the entire example source with this file, and save/compile it.
The same configuration steps apply.

The Windows policy branch requires administrator access on many installations,
even under the current user's registry hive. The mod starts a temporary Windows
PowerShell 5.1 helper through the normal UAC prompt when a policy write is needed.
Windows PowerShell 5.1 and its standard .NET/Add-Type functionality must be available.
The helper is embedded in this source, exits after the operation, and only writes
the three policy objects described below. It does not change registry permissions,
install a service or scheduled task, or require a separately backed-up helper file.
It targets the original Windows user's hive even if different administrator
credentials are supplied. Existing matching settings need no elevation at startup.

In Chrome open `chrome://policy`, click **Reload policies**, and check
`DefaultCookiesSetting`, `ClearBrowsingDataOnExitList`, and
`CookiesAllowedForUrls`. Disable your old clearing extension so it cannot
delete the cookies this configuration preserves. Test by closing the last
Chrome window normally, reopening Chrome, and checking your login and history.

## Chrome's On-device site data page
The same Windhawk keep list controls `chrome://settings/content/siteData`:
- **Delete data sites have saved to your device when you close all windows**
  is enforced by `DefaultCookiesSetting = 4`.
- **Allowed to save data on your device** receives each keep site through
  `CookiesAllowedForUrls`, including its subdomains (`[*.]chatgpt.com`, for example).
Edit these managed entries in the mod's Settings tab, then Save. Chrome may show
policy/managed icons or lock the corresponding controls. Reload Chrome's policies
to check changes immediately. There is no second keep list to maintain.
Keep sites are not added to **Always delete site data**; that would conflict
with preserving their login cookies.

## What is cleared
All-time browsing history, download history, cached images/files, autofill,
site settings and hosted-app data are cleared globally. Cookies and ordinary
website storage are session-only except for the sites in the keep list.
Downloaded files, bookmarks and saved passwords are not selected for deletion.
Keep-list entries cover the domain and its subdomains; they preserve login
cookies/storage, not that site's history, cached files or permissions.
An account may still expire its login independently of this mod.

Chrome's normal last-window-close path waits for its cleanup. Crashes, forced
termination, OS shutdown and some restart/quit paths can prevent completion or
defer selective site-data cleanup until the next startup. This mod does not
change Chrome's background/startup settings and cannot guarantee those paths.
Chrome can report **Managed by your organization** for these local policies.
Chrome automatically disables synchronization for affected data categories;
this mod does not disable all synchronization.

## Controls and removal
Turning **Enable automatic cleanup** off restores the policy objects this mod
created. Disabling/uninstalling/reloading the mod also requests their removal.
Removal requires administrator approval too. Canceling that prompt leaves the
previous policy configuration active; enable the mod again and turn **Enable
automatic cleanup** off to retry removal before uninstalling. Recompiling an
installed mod can request approval for both removal and reapplication.
The tool process exiting or Windows rebooting preserves the configuration;
it is checked again when the mod loads. An explicit Windhawk mod unload, including
a reload for an update, requests removal and can display a UAC prompt. A pending
configuration prompt must finish before unloading can complete.
If something else changes one of the owned
policies, this mod refuses to overwrite it and reports the conflict instead.

## Backup and a fresh Windows installation
Keep this one source file and export/back up its Windhawk settings if you edit
the keep list. Source alone contains the two default sites, not later Settings
edits. On a new installation, import the source and restore those settings.
Per-machine recovery state is in
`HKEY_CURRENT_USER\Software\WindhawkMods\ChromeClearOnExitKeepSites`.
Do not copy that state key to another Windows installation.

The mod owns only three objects below
`HKEY_CURRENT_USER\Software\Policies\Google\Chrome`: the
`DefaultCookiesSetting` value and the `ClearBrowsingDataOnExitList` and
`CookiesAllowedForUrls` subkeys. It refuses to take over existing objects or
conflicting local/machine policies. Check `chrome://policy` for additional
cloud/organization policies that cannot be discovered from these registry keys.
All Chrome profiles belonging to this Windows user receive these policies.

Removing the policies does not recover browsing data already deleted by Chrome.
Legacy protected hosted-app storage is cleared globally; the keep list preserves
ordinary website storage and persistent login cookies. A keep rule does not
extend a website's own cookie expiry or prevent a server-side logout.

## Documentation
- [Chrome's on-exit clearing policy](https://chromeenterprise.google/policies/clear-browsing-data-on-exit-list/)
- [Google's registry/policy setup guide](https://support.google.com/chrome/a/answer/10686330?hl=en)
- [Chromium's selective site-data cleanup](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/chrome/browser/sessions/session_data_deleter.cc)
- [Windhawk settings and APIs](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod)
- [Windhawk dedicated tool-process hosting](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)
- [Windhawk unload versus process exit](https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown)
- [Windows administrator elevation](https://learn.microsoft.com/en-us/windows/win32/shell/launch)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable automatic cleanup
  $description: Applying changes or removing these policies can show a Windows administrator prompt. Canceling leaves the previous configuration active.
- keepSites:
  - chatgpt.com
  - openai.com
  $name: Keep signed in on these sites
  $description: ASCII/punycode domains or full http/https URLs using those hosts. Subdomains are included. Blank rows are ignored. At least one valid site is required; maximum 256 rows.
- notifyErrors: true
  $name: Show error notifications
  $description: Show a message box when configuration cannot be applied or restored. Also logged when Windhawk logging is enabled.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <sddl.h>
#include <objbase.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <functional>
#include <map>
#include <mutex>
#include <exception>
#include <string>
#include <vector>

namespace {
constexpr wchar_t kChrome[] = L"Software\\Policies\\Google\\Chrome";
constexpr wchar_t kState[] = L"Software\\WindhawkMods\\ChromeClearOnExitKeepSites";
constexpr wchar_t kOwnerMutex[] = L"Local\\Windhawk.ChromeClearOnExitKeepSites.PolicyOwner";
constexpr const wchar_t* kListNames[] = {L"ClearBrowsingDataOnExitList", L"CookiesAllowedForUrls"};
constexpr const wchar_t* kTypes[] = {
    L"browsing_history", L"download_history", L"cached_images_and_files",
    L"autofill", L"site_settings", L"hosted_app_data"};
constexpr size_t kMaxSites = 256;
using Bytes = std::vector<BYTE>;

struct Error { std::wstring text; };
[[noreturn]] void Fail(const std::wstring& text) { throw Error{text}; }
void Check(LONG rc, const wchar_t* action) {
    if (rc != ERROR_SUCCESS)
        Fail(std::wstring(action) + L" (Windows error " + std::to_wstring(rc) + L").");
}
bool Missing(LONG rc) { return rc == ERROR_FILE_NOT_FOUND || rc == ERROR_PATH_NOT_FOUND; }
REGSAM View() { return KEY_WOW64_64KEY; }
struct Key {
    HKEY h = nullptr;
    Key() = default;
    Key(const Key&) = delete;
    Key& operator=(const Key&) = delete;
    ~Key() { if (h) RegCloseKey(h); }
};
struct Value {
    DWORD type = REG_NONE;
    Bytes data;
    bool operator==(const Value& b) const { return type == b.type && data == b.data; }
};
struct Object {
    bool present = false;
    std::map<std::wstring, Value> values;
    bool operator==(const Object& b) const { return present == b.present && values == b.values; }
};
using Snapshot = std::array<Object, 3>;
struct State { Snapshot expected; std::wstring phase; Snapshot rollback; bool hasRollback = false; };
struct Settings { bool enabled; bool notify; std::vector<std::wstring> sites; };
std::mutex g_lock;
HANDLE g_owner = nullptr;
bool g_notify = true;
std::wstring g_pendingError;

// BEGIN_ELEVATED_POLICY_BROKER
// Short-lived elevated writer. Ownership, verification and recovery stay in
// the controller; the helper accepts only the fixed Chrome policy operations.

// The mod journals helper PID + creation time before every mutation. A new
// tool process must not recover that journal while this exact helper lives.
void RecordElevatedHelper(HANDLE process);
void ClearElevatedHelperRecord();

struct ElevatedBrokerIoError { std::wstring message; };

class ElevatedPolicyBroker {
public:
    ElevatedPolicyBroker() = default;
    ElevatedPolicyBroker(const ElevatedPolicyBroker&) = delete;
    ElevatedPolicyBroker& operator=(const ElevatedPolicyBroker&) = delete;
    ~ElevatedPolicyBroker() { Close(); }
    bool HasUnconfirmedWrite() const { return unconfirmedWrite_; }

    void Request(const std::wstring& command) {
        if (unavailable_)
            Fail(L"The elevated helper is unavailable for this operation. No additional UAC prompt was opened. Retry the operation after resolving the earlier error.");
        if (command.empty() || command.size() > 4096)
            Fail(L"Internal error: invalid elevated-helper command length.");
        std::string wire;
        for (wchar_t c : command) {
            if ((c < 32 && c != L'\t') || c > 126)
                Fail(L"Internal error: elevated-helper commands must contain only ASCII text and tab separators.");
            wire.push_back(static_cast<char>(c));
        }
        std::string reply;
        try {
            if (pipe_ == INVALID_HANDLE_VALUE) Start();
            RecordElevatedHelper(process_);
            unconfirmedWrite_ = true;
            WriteLine(wire);
            reply = ReadLine();
            if (reply != "OK" && reply.compare(0, 4, "ERR\t") != 0)
                IoFail(L"The elevated helper returned an invalid response.");
            unconfirmedWrite_ = false;
            ClearElevatedHelperRecord();
        } catch (const ElevatedBrokerIoError& e) {
            // A canceled/lost session must not open another UAC prompt during
            // the caller's rollback attempt. A new transaction gets a new object.
            unavailable_ = true;
            Close();
            Fail(e.message);
        }
        if (reply.compare(0, 4, "ERR\t") == 0) {
            // A normal registry/validation failure leaves the broker available
            // so the existing C++ transaction can roll back using this process.
            Fail(L"Elevated policy operation failed: " + Utf8(reply.substr(4)));
        }
    }

private:
    static constexpr DWORD kIoTimeoutMs = 30000;
    HANDLE pipe_ = INVALID_HANDLE_VALUE;
    HANDLE process_ = nullptr;
    bool unavailable_ = false;
    bool unconfirmedWrite_ = false;
    std::string input_;

    [[noreturn]] static void IoFail(const std::wstring& message) {
        throw ElevatedBrokerIoError{message};
    }
    [[noreturn]] static void WinFail(const wchar_t* action, DWORD error = GetLastError()) {
        IoFail(std::wstring(action) + L" (Windows error " + std::to_wstring(error) + L").");
    }
    struct Handle {
        HANDLE value = nullptr;
        ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    };
    struct Apartment {
        bool initialized = false;
        Apartment() {
            HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            initialized = SUCCEEDED(result);
            if (FAILED(result) && result != RPC_E_CHANGED_MODE)
                IoFail(L"Unable to initialize Windows shell elevation (HRESULT " + std::to_wstring(static_cast<unsigned long>(result)) + L").");
        }
        ~Apartment() { if (initialized) CoUninitialize(); }
    };

    // Every pending operation is canceled AND drained before its OVERLAPPED,
    // event and I/O buffer can go out of scope.
    struct Operation {
        HANDLE pipe;
        OVERLAPPED overlapped{};
        bool pending = false;
        explicit Operation(HANDLE handle) : pipe(handle) {
            overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            if (!overlapped.hEvent) WinFail(L"Creating a helper communication event");
        }
        ~Operation() {
            if (pending) {
                CancelIoEx(pipe, &overlapped);
                DWORD ignored = 0;
                GetOverlappedResult(pipe, &overlapped, &ignored, TRUE);
            }
            CloseHandle(overlapped.hEvent);
        }
        DWORD Wait(HANDLE helper, DWORD milliseconds) {
            HANDLE handles[] = {overlapped.hEvent, helper};
            DWORD result = WaitForMultipleObjects(helper ? 2 : 1, handles, FALSE, milliseconds);
            if (result == WAIT_TIMEOUT)
                IoFail(L"The elevated helper did not respond within 30 seconds. Recovery information was retained.");
            if (helper && result == WAIT_OBJECT_0 + 1) {
                DWORD code = 0;
                GetExitCodeProcess(helper, &code);
                IoFail(L"The elevated helper exited before completing the operation (exit code " + std::to_wstring(code) + L").");
            }
            if (result != WAIT_OBJECT_0) WinFail(L"Waiting for the elevated helper");
            DWORD transferred = 0;
            BOOL ok = GetOverlappedResult(pipe, &overlapped, &transferred, FALSE);
            DWORD error = ok ? ERROR_SUCCESS : GetLastError();
            if (ok || error != ERROR_IO_INCOMPLETE) pending = false;
            if (!ok) WinFail(L"Communicating with the elevated helper", error);
            return transferred;
        }
    };

    static DWORD Remaining(ULONGLONG deadline) {
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) IoFail(L"The elevated helper communication timed out. Recovery information was retained.");
        return static_cast<DWORD>(deadline - now);
    }
    static std::wstring Utf8(const std::string& text) {
        if (text.empty()) return L"No additional information was returned.";
        int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
        if (!count) return L"The helper returned an unreadable error message.";
        std::wstring result(count, L'\0');
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), &result[0], count);
        return result;
    }
    static std::wstring Base64Utf16(const std::wstring& text) {
        static constexpr wchar_t alphabet[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::vector<BYTE> bytes;
        bytes.reserve(text.size() * 2);
        for (wchar_t c : text) {
            bytes.push_back(static_cast<BYTE>(c & 255));
            bytes.push_back(static_cast<BYTE>((static_cast<unsigned>(c) >> 8) & 255));
        }
        std::wstring output;
        output.reserve(((bytes.size() + 2) / 3) * 4);
        for (size_t i = 0; i < bytes.size(); i += 3) {
            unsigned n = unsigned(bytes[i]) << 16;
            if (i + 1 < bytes.size()) n |= unsigned(bytes[i + 1]) << 8;
            if (i + 2 < bytes.size()) n |= unsigned(bytes[i + 2]);
            output += alphabet[(n >> 18) & 63];
            output += alphabet[(n >> 12) & 63];
            output += i + 1 < bytes.size() ? alphabet[(n >> 6) & 63] : L'=';
            output += i + 2 < bytes.size() ? alphabet[n & 63] : L'=';
        }
        return output;
    }
    static std::wstring OriginalUserSid() {
        Handle token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token.value))
            WinFail(L"Reading the original Windows user's identity");
        DWORD bytes = 0;
        GetTokenInformation(token.value, TokenUser, nullptr, 0, &bytes);
        if (!bytes || bytes > 65536) WinFail(L"Reading the original Windows user's identity");
        std::vector<BYTE> buffer(bytes);
        if (!GetTokenInformation(token.value, TokenUser, buffer.data(), bytes, &bytes))
            WinFail(L"Reading the original Windows user's identity");
        LPWSTR raw = nullptr;
        if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer.data())->User.Sid, &raw))
            WinFail(L"Formatting the original Windows user's identity");
        std::wstring result;
        try { result = raw; } catch (...) { LocalFree(raw); throw; }
        LocalFree(raw);
        return result;
    }

    static std::wstring Script(const std::wstring& sid, const std::wstring& pipeName) {
        // These substitutions come from Windows, not from settings or RPC data.
        std::wstring script = L"$targetSid='" + sid + L"'\n$pipeName='" + pipeName +
            L"'\n$expectedServerPid=" + std::to_wstring(GetCurrentProcessId()) + L"\n";
        script += LR"PS($ErrorActionPreference='Stop'
$pipe=$null; $reader=$null; $writer=$null; $users=$null; $userKey=$null
function ErrorLine($message) {
  $safe=([string]$message) -replace '[\r\n\t]',' '
  if($safe.Length -gt 1800){$safe=$safe.Substring(0,1800)}
  return "ERR`t$safe"
}
function CheckName($object,$name) {
  if($object -eq 0){if($name -cne '-'){throw 'Invalid policy value name.'}; return}
  if($name -cnotmatch '^[1-9][0-9]{0,2}$' -or [int]$name -gt 512){throw 'Invalid policy list index.'}
}
function CheckDomain($pattern) {
  if($pattern.Length -gt 257 -or !$pattern.StartsWith('[*.]')){throw 'Invalid keep-site pattern.'}
  $keepHost=$pattern.Substring(4)
  if($keepHost -cnotmatch '^(?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$'){throw 'Invalid keep-site domain.'}
  $lastLabel=$keepHost.Substring($keepHost.LastIndexOf('.')+1)
  if($lastLabel.Length -lt 2 -or $lastLabel -cnotmatch '[a-z]'){throw 'Invalid keep-site domain suffix.'}
}
try {
  $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
  try {$principal=[Security.Principal.WindowsPrincipal]::new($identity)
    if(!$principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)){throw 'Administrator approval is required.'}
  } finally {$identity.Dispose()}
  $sidCheck=[Security.Principal.SecurityIdentifier]::new($targetSid)
  if($sidCheck.Value -cne $targetSid){throw 'Invalid original-user identity.'}
  # Identification SQOS prevents the unelevated server from impersonating our token.
  $pipe=[IO.Pipes.NamedPipeClientStream]::new('.', $pipeName, [IO.Pipes.PipeDirection]::InOut, [IO.Pipes.PipeOptions]::None, [Security.Principal.TokenImpersonationLevel]::Identification)
  $pipe.Connect(30000)
  $reader=[IO.StreamReader]::new($pipe,[Text.UTF8Encoding]::new($false,$true),$false,4096,$true)
  $writer=[IO.StreamWriter]::new($pipe,[Text.UTF8Encoding]::new($false),4096,$true)
  $writer.AutoFlush=$true; $writer.NewLine="`n"
  Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
public static class WhCleanupPipePeer {
  [DllImport("kernel32.dll", SetLastError=true)]
  [return: MarshalAs(UnmanagedType.Bool)]
  public static extern bool GetNamedPipeServerProcessId(SafePipeHandle pipe, out uint processId);
}
'@
  [uint32]$actualServerPid=0
  if(![WhCleanupPipePeer]::GetNamedPipeServerProcessId($pipe.SafePipeHandle,[ref]$actualServerPid) -or $actualServerPid -ne $expectedServerPid){throw 'The helper pipe server identity did not match.'}
  $users=[Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::Users,[Microsoft.Win32.RegistryView]::Registry64)
  $userKey=$users.OpenSubKey($targetSid,$false)
  if($null -eq $userKey){throw 'The original Windows user registry hive is not loaded.'}
  $basePath=$targetSid+'\Software\Policies\Google\Chrome'
  $listNames=@('','ClearBrowsingDataOnExitList','CookiesAllowedForUrls')
  $allowedTypes=@('browsing_history','download_history','cached_images_and_files','autofill','site_settings','hosted_app_data')
  $writer.WriteLine('OK')
  while($null -ne ($line=$reader.ReadLine())) {
    $operation='request'; $path=$basePath
    try {
      if($line.Length -gt 4096){throw 'Helper command is too long.'}
      $parts=$line.Split([char]9)
      if($parts.Count -lt 2 -or $parts[1] -cnotmatch '^[012]$'){throw 'Invalid policy object.'}
      $operation=$parts[0]; $object=[int]$parts[1]
      $path=$basePath
      if($object -ne 0){$path+='\'+$listNames[$object]}
      switch -CaseSensitive ($operation) {
        'create' {
          if($parts.Count -ne 2 -or $object -eq 0){throw 'Invalid create request.'}
          $key=$users.CreateSubKey($path,$true)
          try {$key.Flush()} finally {$key.Dispose()}
        }
        'set' {
          if($parts.Count -ne 4){throw 'Invalid set request.'}
          CheckName $object $parts[2]
          $name=$parts[2]; $value=$parts[3]
          if($object -eq 0){
            if($value -cne '4'){throw 'Only the session-only cookie default is supported.'}
            $name='DefaultCookiesSetting'; $value=[int]4
            $kind=[Microsoft.Win32.RegistryValueKind]::DWord
          } else {
            if($object -eq 1 -and $allowedTypes -cnotcontains $value){throw 'Unsupported clear-on-exit category.'}
            if($object -eq 2){CheckDomain $value}
            $kind=[Microsoft.Win32.RegistryValueKind]::String
          }
          $key=$users.CreateSubKey($path,$true)
          try {$key.SetValue($name,$value,$kind); $key.Flush()} finally {$key.Dispose()}
        }
        'delval' {
          if($parts.Count -ne 3){throw 'Invalid delete-value request.'}
          CheckName $object $parts[2]
          $name=$parts[2]
          if($object -eq 0){$name='DefaultCookiesSetting'}
          $key=$users.OpenSubKey($path,$true)
          if($null -eq $key){throw 'The owned policy key disappeared.'}
          try {$key.DeleteValue($name,$true); $key.Flush()} finally {$key.Dispose()}
        }
        'delkey' {
          if($parts.Count -ne 2 -or $object -eq 0){throw 'Invalid delete-key request.'}
          $key=$users.OpenSubKey($path,$false)
          if($null -eq $key){throw 'The owned policy key disappeared.'}
          try {if($key.ValueCount -ne 0 -or $key.SubKeyCount -ne 0){throw 'Refusing to delete a nonempty policy key.'}}
          finally {$key.Dispose()}
          $users.DeleteSubKey($path,$true)
          $userKey.Flush()
        }
        default {throw 'Unsupported helper operation.'}
      }
      $writer.WriteLine('OK')
    } catch {
      $writer.WriteLine((ErrorLine ($operation+' HKEY_USERS\'+$path+': '+$_.Exception.Message)))
    }
  }
} catch {
  if($null -ne $writer){try {$writer.WriteLine((ErrorLine $_.Exception.Message))} catch {}}
  exit 1
} finally {
  if($null -ne $userKey){$userKey.Dispose()}
  if($null -ne $users){$users.Dispose()}
  if($null -ne $writer){$writer.Dispose()}
  if($null -ne $reader){$reader.Dispose()}
  if($null -ne $pipe){$pipe.Dispose()}
}
)PS";
        return script;
    }

    void Start() {
        const std::wstring sid = OriginalUserSid();
        GUID guid{};
        HRESULT hr = CoCreateGuid(&guid);
        if (FAILED(hr)) IoFail(L"Unable to generate a private helper pipe name.");
        wchar_t guidText[40]{};
        if (!StringFromGUID2(guid, guidText, static_cast<int>(sizeof(guidText) / sizeof(guidText[0]))))
            IoFail(L"Unable to format the private helper pipe name.");
        std::wstring pipeName = std::wstring(L"WhChromeCleanup.") + guidText;
        std::wstring fullPipe = L"\\\\.\\pipe\\" + pipeName;
        std::wstring dacl = L"D:P(A;;GA;;;" + sid + L")(A;;GA;;;BA)(A;;GA;;;SY)";
        PSECURITY_DESCRIPTOR descriptor = nullptr;
        if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(dacl.c_str(), SDDL_REVISION_1, &descriptor, nullptr))
            WinFail(L"Securing the elevated-helper pipe");
        SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), descriptor, FALSE};
        pipe_ = CreateNamedPipeW(fullPipe.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
            1, 4096, 4096, kIoTimeoutMs, &attributes);
        DWORD pipeError = GetLastError();
        LocalFree(descriptor);
        if (pipe_ == INVALID_HANDLE_VALUE) WinFail(L"Creating the elevated-helper pipe", pipeError);

        // Listen before launching the helper, and keep this OVERLAPPED alive
        // through UAC. Its destructor drains cancellation on every error path.
        Operation connect(pipe_);
        if (!ConnectNamedPipe(pipe_, &connect.overlapped)) {
            DWORD error = GetLastError();
            if (error == ERROR_IO_PENDING) connect.pending = true;
            else if (error != ERROR_PIPE_CONNECTED) WinFail(L"Listening for the elevated helper", error);
        }
        wchar_t windowsDirectory[MAX_PATH + 1]{};
        UINT count = GetWindowsDirectoryW(windowsDirectory, static_cast<UINT>(sizeof(windowsDirectory) / sizeof(windowsDirectory[0])));
        if (!count || count >= sizeof(windowsDirectory) / sizeof(windowsDirectory[0]))
            WinFail(L"Locating Windows PowerShell");
        std::wstring executable = std::wstring(windowsDirectory) + L"\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
        std::wstring parameters = L"-NoLogo -NoProfile -NonInteractive -WindowStyle Hidden -EncodedCommand " + Base64Utf16(Script(sid, pipeName));
        if (parameters.size() > 30000) IoFail(L"The embedded administrator helper exceeds the Windows command-line limit.");
        Apartment apartment;
        SHELLEXECUTEINFOW launch{};
        launch.cbSize = sizeof(launch);
        launch.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
        launch.lpVerb = L"runas";
        launch.lpFile = executable.c_str();
        launch.lpParameters = parameters.c_str();
        launch.nShow = SW_HIDE;
        Wh_Log(L"Requesting administrator approval to update the original Windows user's three Chrome policy objects.");
        if (!ShellExecuteExW(&launch)) {
            DWORD error = GetLastError();
            if (error == ERROR_CANCELLED)
                IoFail(L"Administrator approval was canceled. The requested policy operation was not performed.");
            WinFail(L"Starting the administrator helper", error);
        }
        process_ = launch.hProcess;
        if (!process_) IoFail(L"Windows did not return a process handle for the administrator helper.");
        const ULONGLONG handshakeDeadline = GetTickCount64() + kIoTimeoutMs;
        if (connect.pending) connect.Wait(process_, Remaining(handshakeDeadline));
        ULONG clientPid = 0;
        DWORD helperPid = GetProcessId(process_);
        if (!helperPid || !GetNamedPipeClientProcessId(pipe_, &clientPid))
            WinFail(L"Verifying the administrator helper's identity");
        if (clientPid != helperPid) IoFail(L"An unexpected process connected to the private helper pipe. No policy command was sent.");
        std::string hello = ReadLine(handshakeDeadline);
        if (hello.compare(0, 4, "ERR\t") == 0)
            IoFail(L"The administrator helper could not start: " + Utf8(hello.substr(4)));
        if (hello != "OK") IoFail(L"The administrator helper did not complete its identity checks.");
    }

    void WriteLine(const std::string& text) {
        std::string bytes = text + '\n';
        size_t offset = 0;
        const ULONGLONG deadline = GetTickCount64() + kIoTimeoutMs;
        while (offset < bytes.size()) {
            Operation operation(pipe_);
            DWORD written = 0;
            BOOL ok = WriteFile(pipe_, bytes.data() + offset, static_cast<DWORD>(bytes.size() - offset), &written, &operation.overlapped);
            if (!ok) {
                DWORD error = GetLastError();
                if (error != ERROR_IO_PENDING) WinFail(L"Sending a policy command to the administrator helper", error);
                operation.pending = true;
                written = operation.Wait(process_, Remaining(deadline));
            }
            if (!written) IoFail(L"The administrator helper closed its communication channel.");
            offset += written;
        }
    }
    std::string ReadLine(ULONGLONG deadline = 0) {
        if (!deadline) deadline = GetTickCount64() + kIoTimeoutMs;
        for (;;) {
            size_t newline = input_.find('\n');
            if (newline != std::string::npos) {
                std::string line = input_.substr(0, newline);
                input_.erase(0, newline + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                return line;
            }
            if (input_.size() > 8192) IoFail(L"The administrator helper returned an oversized response.");
            char buffer[512];
            Operation operation(pipe_);
            DWORD read = 0;
            BOOL ok = ReadFile(pipe_, buffer, sizeof(buffer), &read, &operation.overlapped);
            if (!ok) {
                DWORD error = GetLastError();
                if (error != ERROR_IO_PENDING) WinFail(L"Reading the administrator helper response", error);
                operation.pending = true;
                read = operation.Wait(process_, Remaining(deadline));
            }
            if (!read) IoFail(L"The administrator helper closed its communication channel.");
            input_.append(buffer, read);
        }
    }
    void Close() noexcept {
        // Request/Start have already unwound and drained their OVERLAPPED I/O.
        // Closing this sole server connection gives the helper EOF; it exits.
        if (pipe_ != INVALID_HANDLE_VALUE) {
            DisconnectNamedPipe(pipe_);
            CloseHandle(pipe_);
            pipe_ = INVALID_HANDLE_VALUE;
        }
        input_.clear();
        if (process_) {
            if (WaitForSingleObject(process_, 2000) == WAIT_TIMEOUT)
                Wh_Log(L"The disconnected administrator helper is still exiting; it cannot receive further commands.");
            CloseHandle(process_);
            process_ = nullptr;
        }
    }
};

// END_ELEVATED_POLICY_BROKER

ElevatedPolicyBroker* g_policyBroker = nullptr;
struct PolicyOperation {
    ElevatedPolicyBroker broker;
    PolicyOperation() {
        if (g_policyBroker) Fail(L"An internal policy operation is already running.");
        g_policyBroker = &broker;
    }
    ~PolicyOperation() { g_policyBroker = nullptr; }
};
void PolicyRequest(const std::wstring& request) {
    if (!g_policyBroker) Fail(L"No policy-writing operation is active.");
    g_policyBroker->Request(request);
}

std::wstring Lower(std::wstring value) {
    for (auto& c : value) if (c >= L'A' && c <= L'Z') c += L'a' - L'A';
    return value;
}
std::wstring Trim(std::wstring value) {
    size_t first = 0, last = value.size();
    while (first < last && iswspace(value[first])) ++first;
    while (last > first && iswspace(value[last - 1])) --last;
    return value.substr(first, last - first);
}
std::wstring NormalizeSite(std::wstring input) {
    input = Lower(Trim(input));
    if (input.compare(0, 4, L"[*.]") == 0) input.erase(0, 4);
    if (input.compare(0, 7, L"http://") == 0 || input.compare(0, 8, L"https://") == 0) {
        input.erase(0, input.compare(0, 8, L"https://") == 0 ? 8 : 7);
        input = input.substr(0, input.find_first_of(L"/?#"));
        if (input.find(L'@') != std::wstring::npos) Fail(L"Keep-site URLs cannot contain credentials.");
        size_t colon = input.find(L':');
        if (colon != std::wstring::npos) {
            std::wstring port = input.substr(colon + 1);
            if (port.empty() || port.size() > 5 || port.find_first_not_of(L"0123456789") != std::wstring::npos)
                Fail(L"Invalid port in a keep-site URL.");
            unsigned number = 0;
            for (auto c : port) number = number * 10 + unsigned(c - L'0');
            if (!number || number > 65535) Fail(L"Invalid port in a keep-site URL.");
            input.resize(colon);
        }
    }
    if (!input.empty() && input.back() == L'.') input.pop_back();
    if (input.empty() || input.size() > 253 || input.find(L'.') == std::wstring::npos)
        Fail(L"A keep-site entry must be a complete domain such as chatgpt.com.");
    size_t start = 0;
    while (start < input.size()) {
        size_t end = input.find(L'.', start);
        if (end == std::wstring::npos) end = input.size();
        if (end == start || end - start > 63 || input[start] == L'-' || input[end - 1] == L'-')
            Fail(L"Invalid domain in the keep list: " + input);
        for (size_t i = start; i < end; ++i) {
            wchar_t c = input[i];
            if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || c == L'-'))
                Fail(L"Use an ASCII/punycode domain or an http/https URL; wildcards and IP addresses are not supported: " + input);
        }
        start = end + 1;
    }
    auto tld = input.substr(input.rfind(L'.') + 1);
    if (tld.size() < 2 || tld.find_first_of(L"abcdefghijklmnopqrstuvwxyz") == std::wstring::npos)
        Fail(L"A keep-site entry must end in a domain name, not an IP address: " + input);
    return input;
}
Settings ReadSettings() {
    Settings s{Wh_GetIntSetting(L"enabled") != 0, Wh_GetIntSetting(L"notifyErrors") != 0, {}};
    if (!s.enabled) return s;
    for (size_t i = 0; i <= kMaxSites; ++i) {
        PCWSTR raw = Wh_GetStringSetting(L"keepSites[%d]", int(i));
        std::wstring text = raw ? raw : L"";
        if (raw) Wh_FreeStringSetting(raw);
        // Windhawk arrays can contain blank rows before later valid entries.
        if (text.empty()) continue;
        if (i == kMaxSites) Fail(L"The keep list is limited to 256 entries.");
        auto site = NormalizeSite(text);
        if (std::find(s.sites.begin(), s.sites.end(), site) == s.sites.end()) s.sites.push_back(site);
    }
    if (s.sites.empty()) Fail(L"The keep list cannot be empty. Add at least one site before enabling cleanup.");
    return s;
}

Value Dword(DWORD n) {
    Value v{REG_DWORD, Bytes(sizeof(n))};
    std::memcpy(v.data.data(), &n, sizeof(n));
    return v;
}
Value String(const std::wstring& text) {
    Value v{REG_SZ, Bytes((text.size() + 1) * sizeof(wchar_t))};
    std::memcpy(v.data.data(), text.c_str(), v.data.size());
    return v;
}
bool ReadValue(HKEY key, const wchar_t* name, Value& v) {
    DWORD count = 0;
    LONG rc = RegQueryValueExW(key, name, nullptr, &v.type, nullptr, &count);
    if (Missing(rc)) return false;
    Check(rc, L"Reading a registry value");
    if (count > 1024 * 1024) Fail(L"A registry value is unexpectedly large.");
    v.data.resize(count);
    Check(RegQueryValueExW(key, name, nullptr, &v.type, v.data.empty() ? nullptr : v.data.data(), &count), L"Reading a registry value");
    v.data.resize(count);
    return true;
}
std::wstring Text(const Value& v) {
    if (v.type != REG_SZ || v.data.size() < sizeof(wchar_t) || v.data.size() % sizeof(wchar_t))
        Fail(L"The mod's recovery state is invalid; no policies were changed.");
    std::wstring s(v.data.size() / sizeof(wchar_t), L'\0');
    std::memcpy(&s[0], v.data.data(), v.data.size());
    if (s.back() != L'\0') Fail(L"The mod's recovery state is invalid.");
    s.pop_back();
    return s;
}
void SetValue(HKEY key, const std::wstring& name, const Value& v) {
    Check(RegSetValueExW(key, name.c_str(), 0, v.type, v.data.empty() ? nullptr : v.data.data(), DWORD(v.data.size())), L"Writing a registry value");
}
void DeleteValue(HKEY key, const wchar_t* name) {
    LONG rc = RegDeleteValueW(key, name);
    if (!Missing(rc)) Check(rc, L"Removing a registry value");
}
bool Open(HKEY hive, const std::wstring& path, REGSAM rights, Key& key, REGSAM view = View()) {
    LONG rc = RegOpenKeyExW(hive, path.c_str(), 0, rights | view, &key.h);
    if (Missing(rc)) return false;
    auto action = L"Opening registry key " + std::wstring(hive == HKEY_LOCAL_MACHINE ? L"HKLM\\" : L"HKCU\\") + path;
    Check(rc, action.c_str());
    return true;
}
void Create(HKEY hive, const std::wstring& path, Key& key) {
    auto action = L"Creating registry key " + std::wstring(hive == HKEY_LOCAL_MACHINE ? L"HKLM\\" : L"HKCU\\") + path;
    Check(RegCreateKeyExW(hive, path.c_str(), 0, nullptr, 0, KEY_READ | KEY_WRITE | View(), nullptr, &key.h, nullptr), action.c_str());
}
Object ReadList(HKEY hive, const std::wstring& path, REGSAM view = View()) {
    Object result;
    Key key;
    if (!Open(hive, path, KEY_READ, key, view)) return result;
    result.present = true;
    DWORD children = 0;
    Check(RegQueryInfoKeyW(key.h, nullptr, nullptr, nullptr, &children, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr), L"Inspecting a policy key");
    if (children) Fail(L"An unexpected nested registry key exists below " + path + L". Check chrome://policy.");
    for (DWORD index = 0;; ++index) {
        wchar_t name[16384];
        DWORD chars = DWORD(sizeof(name) / sizeof(name[0]));
        DWORD type = 0, size = 0;
        LONG rc = RegEnumValueW(key.h, index, name, &chars, nullptr, &type, nullptr, &size);
        if (rc == ERROR_NO_MORE_ITEMS) break;
        Check(rc, L"Listing policy values");
        Value v;
        if (!ReadValue(key.h, name, v)) Fail(L"A policy changed while it was being read; try again.");
        result.values.emplace(Lower(std::wstring(name, chars)), std::move(v));
    }
    return result;
}
Snapshot Capture() {
    Snapshot snap;
    Key key;
    if (Open(HKEY_CURRENT_USER, kChrome, KEY_READ, key)) {
        Value v;
        if (ReadValue(key.h, L"DefaultCookiesSetting", v)) { snap[0].present = true; snap[0].values[L""] = std::move(v); }
    }
    for (int i = 1; i < 3; ++i) snap[i] = ReadList(HKEY_CURRENT_USER, std::wstring(kChrome) + L"\\" + kListNames[i - 1]);
    return snap;
}
bool Empty(const Snapshot& s) { return !s[0].present && !s[1].present && !s[2].present; }

void Put(Bytes& out, uint32_t n) {
    for (unsigned i = 0; i < 4; ++i) out.push_back(BYTE(n >> (i * 8)));
}
uint32_t Take(const Bytes& b, size_t& pos) {
    if (pos > b.size() || b.size() - pos < 4) Fail(L"Incomplete recovery state; no policies were changed.");
    uint32_t n = 0;
    for (unsigned i = 0; i < 4; ++i) n |= uint32_t(b[pos++]) << (8 * i);
    return n;
}
Value Encode(const Snapshot& s) {
    Bytes bytes;
    Put(bytes, 1);
    for (auto& object : s) {
        Put(bytes, object.present ? 1 : 0);
        Put(bytes, uint32_t(object.values.size()));
        for (auto& entry : object.values) {
            Put(bytes, uint32_t(entry.first.size()));
            auto raw = reinterpret_cast<const BYTE*>(entry.first.data());
            bytes.insert(bytes.end(), raw, raw + entry.first.size() * sizeof(wchar_t));
            Put(bytes, entry.second.type);
            Put(bytes, uint32_t(entry.second.data.size()));
            bytes.insert(bytes.end(), entry.second.data.begin(), entry.second.data.end());
        }
    }
    return {REG_BINARY, std::move(bytes)};
}
Snapshot Decode(const Value& value) {
    if (value.type != REG_BINARY) Fail(L"Invalid recovery state type.");
    const auto& bytes = value.data;
    size_t pos = 0;
    if (Take(bytes, pos) != 1) Fail(L"Unsupported recovery state version.");
    Snapshot result;
    for (auto& object : result) {
        uint32_t present = Take(bytes, pos), count = Take(bytes, pos);
        if (present > 1 || count > 1024 || (!present && count)) Fail(L"Invalid recovery state.");
        object.present = present != 0;
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t chars = Take(bytes, pos);
            if (chars > 16383 || chars * sizeof(wchar_t) > bytes.size() - pos) Fail(L"Invalid recovery state.");
            std::wstring name(chars, L'\0');
            if (chars) std::memcpy(&name[0], bytes.data() + pos, chars * sizeof(wchar_t));
            pos += chars * sizeof(wchar_t);
            Value v;
            v.type = Take(bytes, pos);
            uint32_t size = Take(bytes, pos);
            if (size > bytes.size() - pos) Fail(L"Invalid recovery state.");
            v.data.assign(bytes.begin() + pos, bytes.begin() + pos + size);
            pos += size;
            if (!object.values.emplace(std::move(name), std::move(v)).second) Fail(L"Invalid recovery state.");
        }
    }
    if (pos != bytes.size() || (result[0].present && result[0].values.size() != 1) ||
        (result[0].present && result[0].values.find(L"") == result[0].values.end())) Fail(L"Invalid recovery state.");
    return result;
}
void StateWrite(const wchar_t* name, const Value& value) {
    Key key;
    Create(HKEY_CURRENT_USER, kState, key);
    SetValue(key.h, name, value);
    Check(RegFlushKey(key.h), L"Saving recovery state");
}
void StateDelete(const wchar_t* name) {
    Key key;
    if (Open(HKEY_CURRENT_USER, kState, KEY_READ | KEY_WRITE, key)) {
        DeleteValue(key.h, name);
        Check(RegFlushKey(key.h), L"Saving recovery state");
    }
}

struct HelperIdentity {
    DWORD version = 1;
    DWORD pid = 0;
    FILETIME created{};
};
static_assert(sizeof(HelperIdentity) == 16);
constexpr wchar_t kActiveHelper[] = L"ActiveElevatedHelper";

void RecordElevatedHelper(HANDLE process) {
    HelperIdentity identity;
    identity.pid = GetProcessId(process);
    FILETIME exited{}, kernel{}, user{};
    if (!identity.pid || !GetProcessTimes(process, &identity.created, &exited, &kernel, &user))
        Fail(L"Unable to record the administrator helper's identity (Windows error " + std::to_wstring(GetLastError()) + L"). No policy command was sent.");
    Value value{REG_BINARY, Bytes(sizeof(identity))};
    std::memcpy(value.data.data(), &identity, sizeof(identity));
    // Flush the identity before every request. A late write after a lost reply
    // must remain owned even if the tool host exits or the helper is still running.
    StateWrite(kActiveHelper, value);
}
void ClearElevatedHelperRecord() { StateDelete(kActiveHelper); }

void VerifyPreviousHelperStopped() {
    Value value;
    {
        Key state;
        if (!Open(HKEY_CURRENT_USER, kState, KEY_READ, state) || !ReadValue(state.h, kActiveHelper, value)) return;
    }
    if (value.type != REG_BINARY || value.data.size() != sizeof(HelperIdentity))
        Fail(L"The interrupted administrator-helper record is invalid. Recovery state was retained.");
    HelperIdentity identity;
    std::memcpy(&identity, value.data.data(), sizeof(identity));
    if (identity.version != 1 || !identity.pid)
        Fail(L"The interrupted administrator-helper identity is invalid. Recovery state was retained.");
    HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, identity.pid);
    if (!process) {
        DWORD error = GetLastError();
        if (error != ERROR_INVALID_PARAMETER)
            Fail(L"Cannot check whether the previous administrator helper has exited (Windows error " + std::to_wstring(error) + L"). Recovery was postponed.");
        ClearElevatedHelperRecord();
        return;
    }
    struct ProcessHandle { HANDLE h; ~ProcessHandle() { CloseHandle(h); } } handle{process};
    FILETIME created{}, exited{}, kernel{}, user{};
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user))
        Fail(L"Cannot verify the previous administrator helper's identity (Windows error " + std::to_wstring(GetLastError()) + L"). Recovery was postponed.");
    if (created.dwLowDateTime == identity.created.dwLowDateTime && created.dwHighDateTime == identity.created.dwHighDateTime) {
        DWORD wait = WaitForSingleObject(process, 0);
        if (wait == WAIT_TIMEOUT)
            Fail(L"The previous administrator helper is still finishing an interrupted operation. Recovery state was retained. Wait for it to exit, then save the mod's settings again.");
        if (wait != WAIT_OBJECT_0)
            Fail(L"Cannot confirm that the previous administrator helper has exited (Windows error " + std::to_wstring(GetLastError()) + L"). Recovery was postponed.");
    }
    // A different creation time means the recorded PID has been reused.
    ClearElevatedHelperRecord();
}

void RemoveState() {
    Key parent;
    if (Open(HKEY_CURRENT_USER, L"Software\\WindhawkMods", DELETE | KEY_READ | KEY_WRITE, parent)) {
        LONG rc = RegDeleteTreeW(parent.h, L"ChromeClearOnExitKeepSites");
        if (!Missing(rc)) Check(rc, L"Removing the recovery state");
        Check(RegFlushKey(parent.h), L"Saving recovery state removal");
    }
}
bool LoadState(State& s) {
    Key key;
    if (!Open(HKEY_CURRENT_USER, kState, KEY_READ, key)) return false;
    Value version, expected, phase, rollback;
    if (!ReadValue(key.h, L"Version", version) || !(version == Dword(1)) || !ReadValue(key.h, L"Expected", expected)) {
        // Initialization can stop before the first Chrome policy is written.
        // With all three objects absent there is nothing to recover or own.
        if (Empty(Capture())) {
            RegCloseKey(key.h); key.h = nullptr;
            RemoveState();
            Wh_Log(L"Removed incomplete recovery state; no Chrome policies existed.");
            return false;
        }
        Fail(L"The mod's recovery state is incomplete or unsupported. No Chrome policies were changed.");
    }
    s.expected = Decode(expected);
    s.phase = ReadValue(key.h, L"Phase", phase) ? Text(phase) : L"stable";
    if (s.phase != L"stable" && s.phase != L"apply" && s.phase != L"remove" && s.phase != L"rollback") Fail(L"Invalid recovery operation.");
    s.hasRollback = ReadValue(key.h, L"Rollback", rollback);
    if (s.hasRollback) s.rollback = Decode(rollback);
    return true;
}
void ResolvePending(State& s) {
    Value pending;
    bool found;
    { Key key; if (!Open(HKEY_CURRENT_USER, kState, KEY_READ, key)) Fail(L"The recovery state was removed externally."); found = ReadValue(key.h, L"PendingAfter", pending); }
    Snapshot actual = Capture();
    if (found) {
        auto after = Decode(pending);
        if (actual == after) { StateWrite(L"Expected", Encode(after)); s.expected = after; }
        else if (!(actual == s.expected)) Fail(L"Chrome policies changed outside this mod. Refusing to overwrite them. Check chrome://policy and the mod log.");
        StateDelete(L"PendingAfter");
    } else if (!(actual == s.expected)) Fail(L"Chrome policies changed outside this mod. Refusing to overwrite them. Check chrome://policy and the mod log.");
}
void Step(State& s, const Snapshot& after, const std::function<void()>& write) {
    ResolvePending(s);
    StateWrite(L"PendingAfter", Encode(after));
    write();
    if (!(Capture() == after)) Fail(L"Policy verification failed; a policy may have changed concurrently.");
    StateWrite(L"Expected", Encode(after));
    s.expected = after;
    StateDelete(L"PendingAfter");
}
void SetPolicy(State& s, int object, const std::wstring& name, const Value& value) {
    auto found = s.expected[object].values.find(name);
    if (found != s.expected[object].values.end() && found->second == value) return;
    Snapshot after = s.expected;
    after[object].present = true;
    after[object].values[name] = value;
    Step(s, after, [&] {
        std::wstring text;
        if (object == 0) {
            if (!(value == Dword(4))) Fail(L"The cookie-default policy value is invalid.");
            text = L"4";
        } else {
            text = Text(value);
            if (text.find_first_of(L"\t\r\n") != std::wstring::npos || text.find(L'\0') != std::wstring::npos)
                Fail(L"A policy value contains an invalid control character.");
        }
        PolicyRequest(L"set\t" + std::to_wstring(object) + L"\t" + (object ? name : L"-") + L"\t" + text);
    });
}
void DeletePolicyValue(State& s, int object, const std::wstring& name) {
    if (!s.expected[object].values.count(name)) return;
    Snapshot after = s.expected;
    after[object].values.erase(name);
    if (!object) after[0].present = false;
    Step(s, after, [&] {
        PolicyRequest(L"delval\t" + std::to_wstring(object) + L"\t" + (object ? name : L"-"));
    });
}
void CreatePolicyKey(State& s, int object) {
    if (s.expected[object].present) return;
    Snapshot after = s.expected; after[object].present = true;
    Step(s, after, [&] { PolicyRequest(L"create\t" + std::to_wstring(object)); });
}
void DeletePolicyKey(State& s, int object) {
    if (!s.expected[object].present) return;
    if (!s.expected[object].values.empty()) Fail(L"Internal error: attempted to remove a nonempty policy key.");
    Snapshot after = s.expected; after[object].present = false;
    Step(s, after, [&] {
        PolicyRequest(L"delkey\t" + std::to_wstring(object));
    });
}
void ApplySnapshot(State& s, const Snapshot& target) {
    // Stop the session-only default first when uninstalling. Add keep sites
    // before enabling cleanup or removing previous keep entries.
    if (!target[0].present) DeletePolicyValue(s, 0, L"");
    for (int object : {2, 1}) {
        if (target[object].present) CreatePolicyKey(s, object);
        for (auto& v : target[object].values) SetPolicy(s, object, v.first, v.second);
        auto old = s.expected[object].values;
        for (auto& v : old) if (!target[object].values.count(v.first)) DeletePolicyValue(s, object, v.first);
        if (!target[object].present) DeletePolicyKey(s, object);
    }
    if (target[0].present) SetPolicy(s, 0, L"", target[0].values.at(L""));
    ResolvePending(s);
    if (!(s.expected == target)) Fail(L"Policy verification did not match the requested configuration.");
}
void MarkStable(State& s) {
    StateWrite(L"Phase", String(L"stable"));
    s.phase = L"stable";
    StateDelete(L"Rollback");
    s.hasRollback = false;
}
void Transition(State& s, const Snapshot& target, const wchar_t* phase) {
    ResolvePending(s);
    const Snapshot before = s.expected;
    StateWrite(L"Rollback", Encode(before));
    StateWrite(L"Phase", String(phase));
    s.rollback = before; s.hasRollback = true; s.phase = phase;
    try {
        ApplySnapshot(s, target);
        MarkStable(s);
        if (Empty(target)) RemoveState();
    } catch (const Error& failure) {
        if (g_policyBroker && g_policyBroker->HasUnconfirmedWrite())
            Fail(failure.text + L" The last policy write could not be confirmed. Recovery state was retained; no rollback will run until that helper has exited. Retry by saving the mod's settings again.");
        std::wstring reason = failure.text;
        try {
            ResolvePending(s);
            StateWrite(L"Phase", String(L"rollback"));
            ApplySnapshot(s, before);
            if (std::wstring(phase) == L"remove") {
                StateWrite(L"Phase", String(L"remove"));
                s.phase = L"remove";
            } else {
                MarkStable(s);
                if (Empty(before)) RemoveState();
            }
        } catch (const Error& rollbackError) {
            Fail(reason + L" Recovery could not finish: " + rollbackError.text + L" Recovery state was retained; reload the mod after resolving the problem.");
        }
        Fail(reason + L" The previous policy configuration was restored." +
             (std::wstring(phase) == L"remove" ? L" Removal will be retried when the mod next loads." : L""));
    }
}
bool Recover(State& s) {
    VerifyPreviousHelperStopped();
    if (!LoadState(s)) return false;
    ResolvePending(s);
    if (s.phase == L"remove") {
        ApplySnapshot(s, Snapshot{});
        RemoveState();
        Wh_Log(L"Recovered an interrupted policy removal.");
        return false;
    }
    if (s.phase != L"stable") {
        if (!s.hasRollback) Fail(L"Recovery information is incomplete; refusing to change Chrome policies.");
        StateWrite(L"Phase", String(L"rollback"));
        ApplySnapshot(s, s.rollback);
        MarkStable(s);
        Wh_Log(L"Recovered the previous configuration after an interrupted update.");
        if (Empty(s.expected)) { RemoveState(); return false; }
    }
    return true;
}

void Preflight() {
    // Registry inspection cannot detect policies delivered by a cloud manager.
    for (HKEY hive : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        for (REGSAM view : {REGSAM(KEY_WOW64_64KEY), REGSAM(KEY_WOW64_32KEY)}) {
            Key key;
            if (!Open(hive, kChrome, KEY_READ, key, view)) continue;
            Value v;
            if (ReadValue(key.h, L"AllowDeletingBrowserHistory", v) && v == Dword(0))
                Fail(L"AllowDeletingBrowserHistory blocks cleanup. Resolve it in chrome://policy first.");
            if (ReadValue(key.h, L"BrowsingDataLifetime", v) && !v.data.empty())
                Fail(L"BrowsingDataLifetime is already configured and could clear kept sites. Resolve it in chrome://policy first.");
            if (hive == HKEY_LOCAL_MACHINE && ReadValue(key.h, L"DefaultCookiesSetting", v))
                Fail(L"A machine-level DefaultCookiesSetting policy already exists. Resolve it in chrome://policy first.");
            for (auto name : {L"CookiesBlockedForUrls", L"CookiesSessionOnlyForUrls", L"BrowsingDataLifetime"}) {
                auto list = ReadList(hive, std::wstring(kChrome) + L"\\" + name, view);
                if (!list.values.empty()) Fail(std::wstring(name) + L" is already configured. Resolve the conflict in chrome://policy first.");
            }
            if (hive == HKEY_LOCAL_MACHINE) {
                for (auto name : kListNames) {
                    auto list = ReadList(hive, std::wstring(kChrome) + L"\\" + name, view);
                    if (list.present) Fail(L"A machine-level " + std::wstring(name) + L" policy exists. Resolve it in chrome://policy first.");
                }
            }
        }
    }
}
Snapshot Desired(const Settings& settings, const Snapshot& previous) {
    Snapshot result;
    result[0].present = true; result[0].values[L""] = Dword(4);
    result[1].present = true;
    for (size_t i = 0; i < sizeof(kTypes) / sizeof(kTypes[0]); ++i)
        result[1].values[std::to_wstring(i + 1)] = String(kTypes[i]);
    result[2].present = true;
    auto occupied = previous[2].values;
    for (auto& site : settings.sites) {
        Value value = String(L"[*.]" + site);
        auto match = std::find_if(previous[2].values.begin(), previous[2].values.end(), [&](const auto& entry) { return entry.second == value; });
        if (match != previous[2].values.end()) result[2].values[match->first] = value;
        else {
            unsigned index = 1;
            while (occupied.count(std::to_wstring(index))) ++index;
            auto name = std::to_wstring(index);
            occupied[name] = value;
            result[2].values[name] = value;
        }
    }
    return result;
}
bool ClaimController() {
    if (g_owner) return true;
    HANDLE mutex = CreateMutexW(nullptr, FALSE, kOwnerMutex);
    if (!mutex) Fail(L"Unable to create the policy-controller mutex (Windows error " + std::to_wstring(GetLastError()) + L").");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        Wh_Log(L"Another mod host controls these policies; this instance is standing by.");
        return false;
    }
    g_owner = mutex;
    return true;
}
void Restore() {
    State state;
    if (!Recover(state)) { Wh_Log(L"No owned Chrome policies need removal."); return; }
    Transition(state, Snapshot{}, L"remove");
    Wh_Log(L"Removed the three Chrome policy objects created by this mod.");
}
void Configure(const Settings& settings) {
    if (!ClaimController()) return;
    if (!settings.enabled) { Restore(); return; }
    State state;
    bool installed = Recover(state);
    Preflight();
    if (!installed) {
        state.expected = Capture();
        if (!Empty(state.expected)) Fail(L"One of the three Chrome policy objects already exists. This mod will not take it over. Inspect chrome://policy before trying again.");
        StateWrite(L"Version", Dword(1));
        StateWrite(L"Expected", Encode(state.expected));
        StateWrite(L"Phase", String(L"stable"));
        state.phase = L"stable";
    }
    auto target = Desired(settings, state.expected);
    if (target == state.expected) { Wh_Log(L"Chrome cleanup policies are already correct."); return; }
    Transition(state, target, L"apply");
    Wh_Log(L"Applied native Chrome exit cleanup with %u kept domains. Reload chrome://policy to verify.", unsigned(settings.sites.size()));
}
void Report(const std::wstring& error) {
    g_pendingError = error;
    Wh_Log(L"ERROR: %s", error.c_str());
}
void RunSettings() {
    try {
        PolicyOperation operation;
        g_notify = Wh_GetIntSetting(L"notifyErrors") != 0;
        // Acquire ownership even if edited settings are invalid, so disabling
        // the mod can still remove policies left by an earlier host process.
        if (!ClaimController()) return;
        auto settings = ReadSettings();
        Configure(settings);
        g_pendingError.clear();
    } catch (const Error& e) { Report(e.text); }
      catch (const std::exception&) { Report(L"An unexpected error prevented the policy operation. Recovery state was retained; inspect the mod log before retrying."); }
}
void ShowError() {
    std::wstring message;
    { std::lock_guard<std::mutex> lock(g_lock); if (g_notify) message.swap(g_pendingError); }
    if (!message.empty()) MessageBoxW(nullptr, message.c_str(), L"Chrome cleanup configuration", MB_OK | MB_ICONWARNING | MB_TOPMOST);
}
} // namespace

// BEGIN_TOOL_WORKER
// The stable tool-host adapter below ends its main thread after initialization.
// This worker keeps the dedicated process alive and performs interactive work
// outside initialization. Auto-reset settings events coalesce repeated changes.
namespace {
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsEvent = nullptr;
HANDLE g_workerThread = nullptr;

DWORD WINAPI PolicyWorker(void*) {
    const HANDLE events[] = {g_stopEvent, g_settingsEvent};
    for (;;) {
        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) return 0;
        if (result != WAIT_OBJECT_0 + 1) {
            {
                std::lock_guard<std::mutex> lock(g_lock);
                Report(L"The configuration worker could not wait for events (Windows error " +
                       std::to_wstring(GetLastError()) + L"). Existing policies were retained.");
            }
            ShowError();
            return 1;
        }
        // A stop request takes precedence over a queued settings change.
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) return 0;
        { std::lock_guard<std::mutex> lock(g_lock); RunSettings(); }
        ShowError();
    }
}

void CloseWorkerHandles() {
    if (g_workerThread) { CloseHandle(g_workerThread); g_workerThread = nullptr; }
    if (g_settingsEvent) { CloseHandle(g_settingsEvent); g_settingsEvent = nullptr; }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
}
} // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Loaded Chrome cleanup mod 0.3.0 in its dedicated tool process.");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent)
        g_settingsEvent = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    if (g_settingsEvent)
        g_workerThread = CreateThread(nullptr, 0, PolicyWorker, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"Could not start the configuration worker (Windows error %u).", GetLastError());
        CloseWorkerHandles();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_settingsEvent && !SetEvent(g_settingsEvent))
        Wh_Log(L"Could not notify the configuration worker (Windows error %u).", GetLastError());
}

void WhTool_ModUninit() {
    // Windhawk calls this on explicit unload, not on ordinary process shutdown.
    // Do not restore policies from global destructors or a process-exit handler.
    if (g_workerThread) {
        if (!SetEvent(g_stopEvent) ||
            WaitForSingleObject(g_workerThread, INFINITE) != WAIT_OBJECT_0) {
            Wh_Log(L"Could not stop the configuration worker (Windows error %u). Recovery state was retained.", GetLastError());
            return;
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_lock);
        if (g_owner) {
            try { PolicyOperation operation; Restore(); }
            catch (const Error& e) { Report(L"Could not fully remove the Chrome cleanup policies: " + e.text); }
            catch (const std::exception&) { Report(L"Unexpected error removing policies. Recovery state was retained."); }
            CloseHandle(g_owner);
            g_owner = nullptr;
        }
    }
    ShowError();
    CloseWorkerHandles();
}
// END_TOOL_WORKER

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
