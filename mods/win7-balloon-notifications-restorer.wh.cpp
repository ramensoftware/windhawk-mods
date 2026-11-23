// ==WindhawkMod==
// @id              win7-balloon-notifications-restorer
// @name            Windows Vista/7/8.1 Balloon Notifications Restorer
// @description     This mod converts supported Windows 10 and 11 toast notifications into classic notification-area balloons inspired by Windows Vista, 7 and 8.1.
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @architecture    x86-64
// @include         *
// @include         svchost.exe
// @exclude         MusNotificationUx.exe
// @exclude         MusNotification.exe
// @exclude         MoUsoCoreWorker.exe
// @exclude         UsoClient.exe
// @exclude         wuauclt.exe
// @exclude         dwm.exe
// @exclude         smss.exe
// @exclude         csrss.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         fontdrvhost.exe
// @exclude         audiodg.exe
// @exclude         LogonUI.exe
// @exclude         consent.exe
// @exclude         CredentialUIBroker.exe
// @exclude         LsaIso.exe
// @exclude         NgcIso.exe
// @exclude         BioIso.exe
// @exclude         SecurityHealthService.exe
// @exclude         MsMpEng.exe
// @exclude         NisSrv.exe
// @exclude         UserOOBEBroker.exe
// @exclude         smartscreen.exe
// @exclude         WerFault.exe
// @exclude         sihost.exe
// @exclude         ctfmon.exe
// @exclude         RuntimeBroker.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         SearchApp.exe
// @exclude         SearchUI.exe
// @exclude         TextInputHost.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @exclude         windhawk-mod.exe
// @exclude         git.exe
// @exclude         git-remote-https.exe
// @exclude         conhost.exe
// @compilerOptions -lole32 -lshlwapi -lshell32 -lversion -lgdiplus -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Vista/7/8.1 Balloon Notifications Restorer

This mod tries to convert supported Windows 10 and Windows 11 toast notifications
into classic notification-area balloons inspired by Windows Vista, 7 and 8.1. This modification has been tested on Windows 11 24H2.

![Classic balloon notification](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/notification.png)

## Setup

- Use a compatible notification area, such as Windows 7/8/8.1 Taskbar Recreation.
- Restart Explorer after installing the mod. The mod includes an Explorer hook
  for legacy balloon notifications; an existing Windows legacy balloon policy
  is also recognized.

Supported notifications are converted on a best-effort basis. Apps can be
excluded using Windhawk's advanced process exclusions; the notification-service
fallback may still process their notifications.

The mod tries to turn supported toast content into notification-area requests.
The notification area determines how those requests are displayed.

## Symbols

The notification-service fallback uses undocumented symbols in `wpncore.dll`,
including `SessionManager::ProcessToastDelivery`. Windhawk can retrieve matching
symbols from the [Microsoft public symbol server](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/microsoft-public-symbols).

Symbol availability and compatibility may vary between Windows builds.

## Credits

The Explorer registry-query hook is inspired by
[Legacy XP-style balloon notifications](https://windhawk.net/mods/legacy-balloon-notifications) by Anixx.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <gdiplus.h>
#include <windhawk_api.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <objbase.h>
#include <roapi.h>
#include <winstring.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cwchar>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string.h>

#ifndef NIN_BALLOONSHOW
#define NIN_BALLOONSHOW (WM_USER + 2)
#define NIN_BALLOONHIDE (WM_USER + 3)
#define NIN_BALLOONTIMEOUT (WM_USER + 4)
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif

#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif

namespace balloon
{


namespace config
{
constexpr WCHAR kHostWindowClass[] = L"ClassicTrayNotifierV3Host";
constexpr WCHAR kLocalWindowClass[] = L"ClassicTrayNotifierV3Local";
constexpr WCHAR kHostMutexName[] = L"Local\\ClassicTrayNotifierV3HostMutex";

constexpr UINT kTrayDeliveryMessage = WM_APP + 0x140;
constexpr UINT kTrayEventMessage = WM_APP + 0x141;
constexpr UINT_PTR kCleanupTimerId = 1;
constexpr UINT kCleanupIntervalMs = 2001;

constexpr size_t kTraySlotCount = 8;
constexpr size_t kDeliveryQueueDepth = 8;
constexpr size_t kRecentToasts = 8;

constexpr size_t kXmlBufferChars = 16384;
constexpr size_t kPayloadBufferBytes = 64 * 1024;

constexpr DWORD kBalloonRequestMagic = 0x54524E59;
constexpr DWORD kBalloonRequestVersion = 3;
constexpr DWORD kSuppressSoundFlag = 0x0001;

constexpr DWORD kEngineStartTimeoutMs = 5001;
constexpr DWORD kHostProbeTimeoutMs = 1000;
constexpr DWORD kWatcherMaxDelayMs = 25601;

constexpr ULONGLONG kRepeatWindowMs = 5001;
constexpr ULONGLONG kProgressRepeatWindowMs = 60001;
constexpr ULONGLONG kSlotIdleTimeoutMs = 120001;
constexpr ULONGLONG kBalloonCloseDelayMs = 501;
constexpr ULONGLONG kPolicyCacheLifetimeMs = 10001;

constexpr PCWSTR kExcludedImages[] = {
    L"windhawk.exe",
    L"shellexperiencehost.exe",
    L"startmenuexperiencehost.exe",
    L"textinputhost.exe",
    L"searchui.exe",
    L"searchapp.exe",
    L"searchhost.exe",
    L"sihost.exe",
    L"csrss.exe",
    L"wininit.exe",
    L"lsass.exe",
    L"services.exe",
    L"smss.exe",
    L"musnotificationux.exe",
    L"musnotification.exe",
    L"mousocoreworker.exe",
    L"usoclient.exe",
    L"wuauclt.exe",
    L"dwm.exe",
    L"winlogon.exe",
    L"fontdrvhost.exe",
    L"audiodg.exe",
    L"logonui.exe",
    L"consent.exe",
    L"credentialuibroker.exe",
    L"lsaiso.exe",
    L"ngciso.exe",
    L"bioiso.exe",
    L"securityhealthservice.exe",
    L"msmpeng.exe",
    L"nissrv.exe",
    L"useroobebroker.exe",
    L"smartscreen.exe",
    L"werfault.exe",
    L"ctfmon.exe",
    L"runtimebroker.exe",
    L"windhawk-ui.exe",
    L"windhawk-mod.exe",
    L"git.exe",
    L"git-remote-https.exe",
    L"conhost.exe",
};

constexpr PCWSTR kBlockedSenders[] = {
    L"windowsupdate",
    L"windows.update",
    L"windows-update",
    L"musnotification",
    L"usoclient",
    L"mousocoreworker",
    L"wuauclt",
};
}


namespace text
{
void copyTruncated(WCHAR *destination, size_t capacity, PCWSTR source)
{
    if (!destination || capacity == 0)
    {
        return;
    }
    if (capacity == 1)
    {
        destination[0] = L'\0';
        return;
    }
    if (!source || !source[0])
    {
        destination[0] = L'\0';
        return;
    }

    const size_t length = wcslen(source);
    if (length < capacity)
    {
        wmemcpy(destination, source, length + 1);
        return;
    }

    const size_t keep = capacity - 2;
    wmemcpy(destination, source, keep);
    destination[keep] = L'\x2026';
    destination[keep + 1] = L'\0';
}

template <size_t N> void copyTruncated(WCHAR (&destination)[N], PCWSTR source)
{
    copyTruncated(destination, N, source);
}

bool equalsIgnoreCase(PCWSTR left, PCWSTR right)
{
    return _wcsicmp(left, right) == 0;
}


void normalizeForBalloon(WCHAR *str)
{
    if (!str)
    {
        return;
    }

    WCHAR *read = str;
    WCHAR *write = str;

    while (*read)
    {
        const WCHAR c = *read;

        if (c == L'\r')
        {
            read++;
            continue;
        }
        if (c == L'\t')
        {
            *write++ = L' ';
            read++;
            continue;
        }
        if (c >= 0xFE00 && c <= 0xFE0F)
        {
            read++;
            continue;
        }
        if (c >= 0x200B && c <= 0x200F)
        {
            read++;
            continue;
        }
        if (c >= 0x2028 && c <= 0x202F)
        {
            read++;
            continue;
        }
        if (c >= 0x2060 && c <= 0x2069)
        {
            read++;
            continue;
        }
        if (c == 0xFEFF || c == 0xFFFC || c == 0xFFFD)
        {
            read++;
            continue;
        }

        if (c >= 0xD800 && c <= 0xDBFF)
        {
            if (read[1] >= 0xDC00 && read[1] <= 0xDFFF)
            {
                read += 2;
            }
            else
            {
                read++;
            }
            continue;
        }
        if (c >= 0xDC00 && c <= 0xDFFF)
        {
            read++;
            continue;
        }

        *write++ = *read++;
    }
    *write = L'\0';

    read = str;
    write = str;
    bool lastWasSpace = false;
    while (*read)
    {
        if (*read == L' ')
        {
            if (!lastWasSpace)
            {
                *write++ = *read;
            }
            lastWasSpace = true;
        }
        else
        {
            *write++ = *read;
            lastWasSpace = (*read == L'\n');
        }
        read++;
    }
    *write = L'\0';

    WCHAR *start = str;
    while (*start == L' ' || *start == L'\n')
    {
        start++;
    }
    if (start != str)
    {
        WCHAR *destination = str;
        while (*start)
        {
            *destination++ = *start++;
        }
        *destination = L'\0';
    }

    size_t length = wcslen(str);
    while (length > 0 && (str[length - 1] == L' ' || str[length - 1] == L'\n'))
    {
        str[--length] = L'\0';
    }
}
}


namespace com
{
class UnknownOwner
{
  public:
    explicit UnknownOwner(void *value) noexcept : value_(static_cast<IUnknown *>(value)) {}
    ~UnknownOwner()
    {
        if (value_)
        {
            value_->Release();
        }
    }
    UnknownOwner(const UnknownOwner &) = delete;
    UnknownOwner &operator=(const UnknownOwner &) = delete;

  private:
    IUnknown *value_;
};

void *queryInterface(void *unknown, REFIID iid)
{
    if (!unknown)
    {
        return nullptr;
    }

    void *result = nullptr;
    if (FAILED(static_cast<IUnknown *>(unknown)->QueryInterface(iid, &result)))
    {
        return nullptr;
    }
    return result;
}

class StreamOwner
{
  public:
    explicit StreamOwner(IStream *stream) noexcept : stream_(stream) {}

    ~StreamOwner()
    {
        if (stream_)
        {
            stream_->Release();
        }
    }

    StreamOwner(const StreamOwner &) = delete;
    StreamOwner &operator=(const StreamOwner &) = delete;

    IStream *get() const noexcept
    {
        return stream_;
    }

  private:
    IStream *stream_;
};
}


namespace hash
{
constexpr DWORD kFnvOffsetBasis = 2166136261u;
constexpr DWORD kFnvPrime = 16777619u;

DWORD fnv1a(PCWSTR str)
{
    DWORD value = kFnvOffsetBasis;
    for (; *str; ++str)
    {
        value = (value ^ static_cast<DWORD>(*str)) * kFnvPrime;
    }
    return value;
}
}


// https://learn.microsoft.com/en-us/uwp/api/windows.ui.notifications.toastnotifier.show
namespace iid
{
constexpr GUID IXmlNodeSerializer = {
    0x5CC5B382, 0xE6DD, 0x4991, {0xAB, 0xEF, 0x06, 0xD8, 0xD2, 0xE7, 0xBD, 0x0C}};
constexpr GUID IToastNotifier = {
    0x75927B93, 0x03F3, 0x41EC, {0x91, 0xD3, 0x6E, 0x5B, 0xAC, 0x1B, 0x38, 0xE7}};
constexpr GUID IToastNotification = {
    0x997E2675, 0x059E, 0x4E60, {0x8B, 0x06, 0x17, 0x60, 0x91, 0x7C, 0x8B, 0x80}};
constexpr GUID IToastNotificationManagerStatics = {
    0x50AC103F, 0xD235, 0x4598, {0xBB, 0xEF, 0x98, 0xFE, 0x4D, 0x1A, 0x3A, 0xD4}};
}


namespace winrt
{
constexpr int kFirstMethod = 6;

using GetStringRawBufferFn = PCWSTR(WINAPI *)(HSTRING, UINT32 *);
using CreateStringFn = HRESULT(WINAPI *)(PCNZWCH, UINT32, HSTRING *);
using DeleteStringFn = HRESULT(WINAPI *)(HSTRING);
using GetActivationFactoryFn = HRESULT(WINAPI *)(HSTRING, REFIID, void **);

namespace
{
GetStringRawBufferFn g_get_string_raw_buffer = nullptr;
CreateStringFn g_create_string = nullptr;
DeleteStringFn g_delete_string = nullptr;
}

bool resolve()
{
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase)
    {
        return false;
    }

    g_get_string_raw_buffer = reinterpret_cast<GetStringRawBufferFn>(
        GetProcAddress(combase, "WindowsGetStringRawBuffer"));
    g_create_string =
        reinterpret_cast<CreateStringFn>(GetProcAddress(combase, "WindowsCreateString"));
    g_delete_string =
        reinterpret_cast<DeleteStringFn>(GetProcAddress(combase, "WindowsDeleteString"));
    return g_get_string_raw_buffer && g_create_string && g_delete_string;
}

bool isResolved()
{
    return g_create_string != nullptr;
}

PCWSTR rawBuffer(HSTRING value, UINT32 *length)
{
    return g_get_string_raw_buffer ? g_get_string_raw_buffer(value, length) : nullptr;
}

void deleteString(HSTRING value)
{
    if (value && g_delete_string)
    {
        g_delete_string(value);
    }
}

class HString
{
  public:
    HString() = default;
    ~HString()
    {
        reset();
    }

    HString(const HString &) = delete;
    HString &operator=(const HString &) = delete;

    bool assign(PCWSTR value)
    {
        reset();
        return SUCCEEDED(g_create_string(value, static_cast<UINT32>(wcslen(value)), &value_));
    }

    HSTRING get() const noexcept
    {
        return value_;
    }

    void reset()
    {
        if (value_)
        {
            deleteString(value_);
            value_ = nullptr;
        }
    }

  private:
    HSTRING value_ = nullptr;
};

bool toastXml(void *notification, WCHAR *out, size_t capacity)
{
    if (!out || capacity == 0)
    {
        return false;
    }
    out[0] = L'\0';

    void *toast = com::queryInterface(notification, iid::IToastNotification);
    if (!toast)
    {
        return false;
    }

    using GetContentFn = HRESULT(STDMETHODCALLTYPE *)(void *, void **);
    void **toastVtbl = *static_cast<void ***>(toast);
    void *xmlDocument = nullptr;
    HRESULT result =
        reinterpret_cast<GetContentFn>(toastVtbl[kFirstMethod])(toast, &xmlDocument);
    static_cast<IUnknown *>(toast)->Release();
    if (FAILED(result) || !xmlDocument)
    {
        return false;
    }

    void *serializer = com::queryInterface(xmlDocument, iid::IXmlNodeSerializer);
    static_cast<IUnknown *>(xmlDocument)->Release();
    if (!serializer)
    {
        return false;
    }

    using GetXmlFn = HRESULT(STDMETHODCALLTYPE *)(void *, HSTRING *);
    void **serializerVtbl = *static_cast<void ***>(serializer);
    HSTRING xmlString = nullptr;
    result =
        reinterpret_cast<GetXmlFn>(serializerVtbl[kFirstMethod])(serializer, &xmlString);
    static_cast<IUnknown *>(serializer)->Release();
    if (FAILED(result) || !xmlString)
    {
        return false;
    }

    UINT32 length = 0;
    PCWSTR raw = rawBuffer(xmlString, &length);
    if (raw && length > 0)
    {
        const size_t count = std::min(static_cast<size_t>(length), capacity - 1);
        wmemcpy(out, raw, count);
        out[count] = L'\0';
    }
    deleteString(xmlString);

    return out[0] != L'\0';
}
}


class ProcessImage
{
  public:
    bool initialize()
    {
        const DWORD length = GetModuleFileNameW(nullptr, path_, ARRAYSIZE(path_));
        if (length == 0 || length >= ARRAYSIZE(path_))
        {
            return false;
        }

        const PCWSTR separator = wcsrchr(path_, L'\\');
        name_ = separator ? separator + 1 : path_;
        queryDescription();
        return true;
    }

    bool isExcluded() const
    {
        for (PCWSTR candidate : config::kExcludedImages)
        {
            if (text::equalsIgnoreCase(name_, candidate))
            {
                return true;
            }
        }
        return false;
    }

    bool isExplorer() const
    {
        return text::equalsIgnoreCase(name_, L"explorer.exe");
    }

    bool isServiceHost() const
    {
        return text::equalsIgnoreCase(name_, L"svchost.exe");
    }

    bool isPowerShell() const
    {
        return text::equalsIgnoreCase(name_, L"powershell.exe") ||
               text::equalsIgnoreCase(name_, L"pwsh.exe");
    }

    PCWSTR path() const noexcept
    {
        return path_;
    }

    PCWSTR name() const noexcept
    {
        return name_;
    }

    PCWSTR description() const noexcept
    {
        return description_;
    }

  private:
    void queryDescription()
    {
        description_[0] = L'\0';

        DWORD handle = 0;
        const DWORD size = GetFileVersionInfoSizeW(path_, &handle);
        if (size == 0 || size >= 1024 * 1024)
        {
            useFileName();
            return;
        }

        BYTE *data = static_cast<BYTE *>(HeapAlloc(GetProcessHeap(), 0, size));
        if (!data)
        {
            useFileName();
            return;
        }

        if (GetFileVersionInfoW(path_, 0, size, data))
        {
            struct Translation
            {
                WORD language;
                WORD codePage;
            } *translation = nullptr;

            UINT translationLength = 0;
            if (VerQueryValueW(data, L"\\VarFileInfo\\Translation",
                               reinterpret_cast<void **>(&translation), &translationLength) &&
                translationLength >= sizeof(*translation))
            {
                WCHAR subBlock[64];
                wsprintfW(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                          translation[0].language, translation[0].codePage);

                PWSTR value = nullptr;
                UINT valueLength = 0;
                if (VerQueryValueW(data, subBlock, reinterpret_cast<void **>(&value),
                                   &valueLength) &&
                    value && value[0])
                {
                    text::copyTruncated(description_, value);
                }
            }
        }

        HeapFree(GetProcessHeap(), 0, data);

        if (!description_[0])
        {
            useFileName();
        }
    }

    void useFileName()
    {
        text::copyTruncated(description_, name_);
        const size_t length = wcslen(description_);
        if (length > 4 && text::equalsIgnoreCase(description_ + length - 4, L".exe"))
        {
            description_[length - 4] = L'\0';
        }
    }

    WCHAR path_[MAX_PATH]{};
    PCWSTR name_ = L"";
    WCHAR description_[96]{};
};


typedef LSTATUS(WINAPI *registry_query_function)(HKEY, LPCWSTR, LPDWORD, LPDWORD,
                                                  LPBYTE, LPDWORD);
static registry_query_function original_registry_query = nullptr;
static registry_query_function registry_query_entry = nullptr;
static bool registry_interceptor_registered = false;
static std::atomic<bool> registry_interceptor_verified{false};
static std::atomic<unsigned long> registry_interceptor_calls{0};

// https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regqueryvalueexw
static LSTATUS WINAPI intercept_legacy_registry_query(HKEY key, LPCWSTR value_name,
                                                       LPDWORD reserved, LPDWORD value_type,
                                                       LPBYTE value_data, LPDWORD data_size)
{
    if (value_name && !reserved && (!value_data || data_size) &&
        (_wcsicmp(value_name, L"EnableLegacyBalloonNotifications") == 0 ||
         _wcsicmp(value_name, L"EnableBalloonTips") == 0))
    {
        registry_interceptor_calls.fetch_add(1, std::memory_order_relaxed);
        if (value_type)
        {
            *value_type = REG_DWORD;
        }
        if (data_size)
        {
            DWORD available = *data_size;
            *data_size = sizeof(DWORD);
            if (value_data)
            {
                if (available < sizeof(DWORD))
                {
                    return ERROR_MORE_DATA;
                }
                DWORD enabled = 1;
                memcpy(value_data, &enabled, sizeof(enabled));
            }
        }
        return ERROR_SUCCESS;
    }
    return original_registry_query(key, value_name, reserved, value_type, value_data, data_size);
}

static void register_explorer_registry_interceptor()
{
    HMODULE library = GetModuleHandleW(L"kernelbase.dll");
    registry_query_entry = library ? (registry_query_function)GetProcAddress(library, "RegQueryValueExW") : nullptr;
    if (!registry_query_entry ||
        !Wh_SetFunctionHook((void *)registry_query_entry, (void *)intercept_legacy_registry_query,
                            (void **)&original_registry_query))
    {
        Wh_Log(L"[balloon] Explorer registry interceptor registration failed");
        registry_query_entry = nullptr;
        return;
    }
    registry_interceptor_registered = true;
    Wh_Log(L"[balloon] Explorer registry interceptor registered");
}

static void verify_explorer_registry_interceptor()
{
    if (!registry_interceptor_registered || !registry_query_entry)
    {
        return;
    }
    unsigned long previous_calls = registry_interceptor_calls.load(std::memory_order_relaxed);
    DWORD type = 0;
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS status = registry_query_entry(HKEY_CURRENT_USER, L"EnableLegacyBalloonNotifications",
                                         nullptr, &type, (LPBYTE)&value, &size);
    bool verified = status == ERROR_SUCCESS && type == REG_DWORD && value == 1 &&
                    size == sizeof(value) &&
                    registry_interceptor_calls.load(std::memory_order_relaxed) != previous_calls;
    registry_interceptor_verified.store(verified);
    Wh_Log(L"[balloon] Explorer registry interceptor verification: %s (status=%ld)",
           verified ? L"passed" : L"failed", status);
}


namespace policy
{
namespace
{
constexpr PCWSTR kPolicyKey = L"Software\\Policies\\Microsoft\\Windows\\Explorer";
constexpr PCWSTR kSettingsKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings";
constexpr PCWSTR kValueName = L"EnableLegacyBalloonNotifications";

bool readSetting()
{
    const struct
    {
        HKEY root;
        PCWSTR key;
    } locations[] = {
        {HKEY_CURRENT_USER, kPolicyKey},
        {HKEY_CURRENT_USER, kSettingsKey},
        {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer"},
        {HKEY_LOCAL_MACHINE,
         L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings"},
    };

    for (const auto &location : locations)
    {
        DWORD value = 0;
        DWORD size = sizeof(value);
        if (RegGetValueW(location.root, location.key, kValueName, RRF_RT_REG_DWORD, nullptr,
                         &value, &size) == ERROR_SUCCESS)
        {
            return value != 0;
        }
    }
    return false;
}
}

bool legacyBalloonsEnabled()
{
    static std::atomic<ULONGLONG> checkedAt{0};
    static std::atomic<bool> cachedValue{false};

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = checkedAt.load(std::memory_order_relaxed);
    if (previous == 0 || now - previous > config::kPolicyCacheLifetimeMs)
    {
        const bool enabled = readSetting();
        static std::atomic<int> reported{-1};
        const int current = enabled ? 1 : 0;
        if (reported.exchange(current) != current)
        {
            Wh_Log(L"[balloon] legacy policy check: %s", enabled ? L"enabled" : L"not enabled");
        }
        cachedValue.store(enabled, std::memory_order_relaxed);
        checkedAt.store(now, std::memory_order_relaxed);
    }
    return cachedValue.load(std::memory_order_relaxed) || registry_interceptor_verified.load();
}

bool conversionAllowed()
{
    return legacyBalloonsEnabled() ||
           FindWindowW(config::kHostWindowClass, nullptr) != nullptr;
}
}


namespace xml
{
void decodeEntities(WCHAR *str)
{
    if (!str)
    {
        return;
    }

    WCHAR *read = str;
    WCHAR *write = str;

    while (*read)
    {
        if (*read != L'&')
        {
            *write++ = *read++;
            continue;
        }

        if (wcsncmp(read, L"&amp;", 5) == 0)
        {
            *write++ = L'&';
            read += 5;
        }
        else if (wcsncmp(read, L"&lt;", 4) == 0)
        {
            *write++ = L'<';
            read += 4;
        }
        else if (wcsncmp(read, L"&gt;", 4) == 0)
        {
            *write++ = L'>';
            read += 4;
        }
        else if (wcsncmp(read, L"&apos;", 6) == 0)
        {
            *write++ = L'\'';
            read += 6;
        }
        else if (wcsncmp(read, L"&quot;", 6) == 0)
        {
            *write++ = L'"';
            read += 6;
        }
        else if (wcsncmp(read, L"&#x", 3) == 0 || wcsncmp(read, L"&#X", 3) == 0)
        {
            WCHAR *afterNumber = nullptr;
            const unsigned long value = wcstoul(read + 3, &afterNumber, 16);
            if (afterNumber && *afterNumber == L';' && value > 0 && value <= 0xFFFF)
            {
                *write++ = static_cast<WCHAR>(value);
                read = afterNumber + 1;
            }
            else
            {
                *write++ = *read++;
            }
        }
        else if (wcsncmp(read, L"&#", 2) == 0)
        {
            WCHAR *afterNumber = nullptr;
            const unsigned long value = wcstoul(read + 2, &afterNumber, 10);
            if (afterNumber && *afterNumber == L';' && value > 0 && value <= 0xFFFF)
            {
                *write++ = static_cast<WCHAR>(value);
                read = afterNumber + 1;
            }
            else
            {
                *write++ = *read++;
            }
        }
        else
        {
            *write++ = *read++;
        }
    }
    *write = L'\0';
}

bool readAttribute(PCWSTR tagStart, PCWSTR tagEnd, PCWSTR name, WCHAR *out, size_t capacity)
{
    if (!out || capacity == 0)
    {
        return false;
    }
    out[0] = L'\0';

    const size_t nameLength = wcslen(name);
    for (PCWSTR p = tagStart; p + nameLength + 2 < tagEnd; p++)
    {
        if (p != tagStart && p[-1] != L' ')
        {
            continue;
        }
        if (wcsncmp(p, name, nameLength) != 0)
        {
            continue;
        }

        PCWSTR q = p + nameLength;
        while (q < tagEnd && *q == L' ')
        {
            q++;
        }
        if (q >= tagEnd || *q != L'=')
        {
            continue;
        }
        q++;
        while (q < tagEnd && *q == L' ')
        {
            q++;
        }
        if (q >= tagEnd || (*q != L'"' && *q != L'\''))
        {
            continue;
        }

        const WCHAR quote = *q++;
        PCWSTR valueEnd = q;
        while (valueEnd < tagEnd && *valueEnd != quote)
        {
            valueEnd++;
        }
        if (valueEnd >= tagEnd)
        {
            return false;
        }

        const size_t length = std::min(static_cast<size_t>(valueEnd - q), capacity - 1);
        wmemcpy(out, q, length);
        out[length] = L'\0';
        decodeEntities(out);
        return true;
    }

    return false;
}
}


struct ToastContent
{
    WCHAR title[512]{};
    WCHAR body[1024]{};
    WCHAR tag[128]{};
    WCHAR group[128]{};
    bool silent = false;
    bool hasProgress = false;

    bool isEmpty() const
    {
        return title[0] == L'\0' && body[0] == L'\0';
    }
};

namespace toast
{
std::optional<ToastContent> parse(PCWSTR xmlText)
{
    if (!xmlText)
    {
        return std::nullopt;
    }

    ToastContent content;
    content.hasProgress = wcsstr(xmlText, L"<progress") != nullptr;

    if (PCWSTR audio = wcsstr(xmlText, L"<audio"))
    {
        if (PCWSTR audioEnd = wcschr(audio, L'>'))
        {
            WCHAR silent[16];
            if (xml::readAttribute(audio, audioEnd, L"silent", silent, ARRAYSIZE(silent)))
            {
                content.silent =
                    text::equalsIgnoreCase(silent, L"true") || wcscmp(silent, L"1") == 0;
            }
        }
    }

    if (PCWSTR toastTag = wcsstr(xmlText, L"<toast"))
    {
        if (PCWSTR toastEnd = wcschr(toastTag, L'>'))
        {
            xml::readAttribute(toastTag, toastEnd, L"tag", content.tag,
                               ARRAYSIZE(content.tag));
            xml::readAttribute(toastTag, toastEnd, L"group", content.group,
                               ARRAYSIZE(content.group));
        }
    }

    PCWSTR position = xmlText;
    bool haveTitle = false;
    while ((position = wcsstr(position, L"<text")) != nullptr)
    {
        PCWSTR tagEnd = wcschr(position, L'>');
        if (!tagEnd)
        {
            break;
        }

        if (tagEnd[-1] == L'/')
        {
            position = tagEnd + 1;
            continue;
        }

        WCHAR placement[32];
        xml::readAttribute(position, tagEnd, L"placement", placement, ARRAYSIZE(placement));

        PCWSTR contentStart = tagEnd + 1;
        PCWSTR contentEnd = wcsstr(contentStart, L"</text>");
        if (!contentEnd)
        {
            break;
        }

        const size_t contentLength = static_cast<size_t>(contentEnd - contentStart);
        if (contentLength > 0 && !text::equalsIgnoreCase(placement, L"attribution"))
        {
            if (!haveTitle)
            {
                const size_t length =
                    std::min(contentLength, static_cast<size_t>(ARRAYSIZE(content.title) - 1));
                wmemcpy(content.title, contentStart, length);
                content.title[length] = L'\0';
                haveTitle = true;
            }
            else
            {
                size_t current = wcslen(content.body);
                if (current > 0 && current + 2 < ARRAYSIZE(content.body))
                {
                    content.body[current++] = L'\n';
                }
                const size_t length = std::min(
                    contentLength, static_cast<size_t>(ARRAYSIZE(content.body) - current - 1));
                wmemcpy(content.body + current, contentStart, length);
                content.body[current + length] = L'\0';
            }
        }

        position = contentEnd + 7;
    }

    xml::decodeEntities(content.title);
    xml::decodeEntities(content.body);
    text::normalizeForBalloon(content.title);
    text::normalizeForBalloon(content.body);

    if (!content.body[0] && content.title[0])
    {
        wcscpy_s(content.body, content.title);
        content.title[0] = L'\0';
    }

    if (content.isEmpty())
    {
        return std::nullopt;
    }
    return content;
}

DWORD dedupeKey(const ToastContent &content)
{
    WCHAR key[512];

    if (content.tag[0] || content.group[0])
    {
        text::copyTruncated(key, content.tag);
    }
    else
    {
        text::copyTruncated(key, content.title);
    }

    const size_t length = wcslen(key);
    if (length + 2 < ARRAYSIZE(key))
    {
        key[length] = L'|';
        text::copyTruncated(key + length + 1, ARRAYSIZE(key) - length - 1,
                            (content.tag[0] || content.group[0]) ? content.group
                                                                 : content.body);
    }

    return hash::fnv1a(key);
}
}


namespace icon
{
HICON defaultSmall()
{
    static const HICON icon = static_cast<HICON>(LoadImageW(
        nullptr, IDI_INFORMATION, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON), LR_SHARED));
    return icon;
}

void extractFromExecutable(PCWSTR path, HICON &smallIcon, HICON &largeIcon)
{
    smallIcon = nullptr;
    largeIcon = nullptr;

    if (path && path[0])
    {
        HICON large = nullptr;
        HICON small = nullptr;
        if (ExtractIconExW(path, 0, &large, &small, 1) != UINT_MAX)
        {
            largeIcon = large;
            smallIcon = small;
        }
    }

    if (!smallIcon)
    {
        smallIcon = defaultSmall();
    }
}

void destroyOwned(HICON icon)
{
    if (icon && icon != defaultSmall())
    {
        DestroyIcon(icon);
    }
}


// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-notifyicondataw
HICON enlargeForBalloon(HICON source)
{
    if (!source)
    {
        return nullptr;
    }

    Gdiplus::GdiplusStartupInput input;
    ULONG_PTR token = 0;
    if (Gdiplus::GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok)
    {
        return nullptr;
    }

    struct GdiplusScope
    {
        ULONG_PTR token;
        ~GdiplusScope() { Gdiplus::GdiplusShutdown(token); }
    } gdiplusScope{token};

    HICON result = nullptr;
    {
        const int width = GetSystemMetrics(SM_CXSMICON);
        const int height = GetSystemMetrics(SM_CYSMICON);

        std::unique_ptr<Gdiplus::Bitmap> original{Gdiplus::Bitmap::FromHICON(source)};
        if (width > 0 && height > 0 && original &&
            original->GetLastStatus() == Gdiplus::Ok)
        {
            Gdiplus::Bitmap output(width, height, PixelFormat32bppARGB);
            if (output.GetLastStatus() == Gdiplus::Ok)
            {
                bool drawn = false;
                {
                    Gdiplus::Graphics graphics(&output);
                    graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
                    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
                    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

                    const Gdiplus::RectF destination(-width * 0.025f, -height * 0.025f,
                                                     width * 1.05f, height * 1.05f);
                    if (graphics.DrawImage(original.get(), destination, 0.0f, 0.0f,
                                           static_cast<Gdiplus::REAL>(original->GetWidth()),
                                           static_cast<Gdiplus::REAL>(original->GetHeight()),
                                           Gdiplus::UnitPixel) == Gdiplus::Ok)
                    {
                        drawn = true;
                    }
                }

                if (drawn)
                {
                    output.GetHICON(&result);
                }
            }
        }
    }

    return result;
}
}


struct BalloonRequest
{
    DWORD size;
    DWORD magic;
    DWORD version;
    DWORD flags;
    DWORD pid;
    WCHAR title[64];
    WCHAR body[256];
    WCHAR appName[96];
    WCHAR exePath[MAX_PATH];
};

namespace ipc
{
bool isValid(const BalloonRequest *request, DWORD bytes)
{
    if (!request || bytes != sizeof(BalloonRequest))
    {
        return false;
    }
    return request->size == sizeof(BalloonRequest) &&
           request->magic == config::kBalloonRequestMagic &&
           request->version == config::kBalloonRequestVersion;
}

void ensureTerminated(BalloonRequest &request)
{
    request.title[ARRAYSIZE(request.title) - 1] = L'\0';
    request.body[ARRAYSIZE(request.body) - 1] = L'\0';
    request.appName[ARRAYSIZE(request.appName) - 1] = L'\0';
    request.exePath[ARRAYSIZE(request.exePath) - 1] = L'\0';
}
}


namespace window
{
namespace
{
struct LargestWindowSearch
{
    DWORD processId;
    HWND result;
    LONG bestArea;
};

BOOL CALLBACK findLargestWindow(HWND hwnd, LPARAM parameter)
{
    auto *search = reinterpret_cast<LargestWindowSearch *>(parameter);

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != search->processId || !IsWindowVisible(hwnd) ||
        GetWindow(hwnd, GW_OWNER))
    {
        return TRUE;
    }
    if (GetWindowTextLengthW(hwnd) == 0)
    {
        return TRUE;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect))
    {
        return TRUE;
    }

    const LONG area = (rect.right - rect.left) * (rect.bottom - rect.top);
    if (area > search->bestArea)
    {
        search->bestArea = area;
        search->result = hwnd;
    }
    return TRUE;
}
}

void activateProcess(DWORD processId)
{
    if (!processId)
    {
        return;
    }

    LargestWindowSearch search{processId, nullptr, 0};
    EnumWindows(findLargestWindow, reinterpret_cast<LPARAM>(&search));
    if (!search.result)
    {
        return;
    }

    HWND window = search.result;
    if (IsIconic(window))
    {
        ShowWindow(window, SW_RESTORE);
    }


    const DWORD targetThread = GetWindowThreadProcessId(window, nullptr);
    const DWORD foregroundThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    const DWORD currentThread = GetCurrentThreadId();

    if (foregroundThread && foregroundThread != currentThread)
    {
        AttachThreadInput(currentThread, foregroundThread, TRUE);
    }
    if (targetThread && targetThread != currentThread)
    {
        AttachThreadInput(currentThread, targetThread, TRUE);
    }

    AllowSetForegroundWindow(processId);
    BringWindowToTop(window);
    SetForegroundWindow(window);

    if (targetThread && targetThread != currentThread)
    {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
    if (foregroundThread && foregroundThread != currentThread)
    {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
}
}


using ChangeWindowMessageFilterExFn = BOOL(WINAPI *)(HWND, UINT, DWORD, void *);

class TrayEngine
{
  public:
    TrayEngine() = default;
    TrayEngine(const TrayEngine &) = delete;
    TrayEngine &operator=(const TrayEngine &) = delete;

    void setStopEvent(HANDLE stopEvent) noexcept
    {
        stopEvent_ = stopEvent;
    }

    void initialize();
    bool start();
    void stop();
    void close();

    bool isHost() const noexcept
    {
        return isHost_;
    }

    HWND window() const noexcept
    {
        return window_.load(std::memory_order_acquire);
    }

    bool enqueue(const BalloonRequest &request);

  private:
    struct Slot
    {
        bool inUse = false;
        bool added = false;
        bool balloonActive = false;
        UINT id = 0;
        DWORD pid = 0;
        ULONGLONG lastUse = 0;
        ULONGLONG removeAt = 0;
        WCHAR exePath[MAX_PATH]{};
        HICON trayIcon = nullptr;
        HICON balloonIcon = nullptr;
    };

    struct StartContext
    {
        TrayEngine *engine;
        HANDLE ready;
    };

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    static DWORD WINAPI threadMain(LPVOID parameter);

    DWORD run(HANDLE ready);
    LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT handleCopyData(const COPYDATASTRUCT &data);
    void handleTrayCallback(UINT event, UINT id);
    void deliverNext();
    void present(const BalloonRequest &request);
    void reapExpired(bool force);
    void resetAfterTaskbarCreated();
    Slot *acquireSlot(PCWSTR exePath);
    Slot *findSlot(UINT id);
    void releaseSlot(Slot &slot);

    std::array<Slot, config::kTraySlotCount> slots_{};
    std::atomic<HWND> window_{nullptr};
    HANDLE thread_ = nullptr;
    DWORD threadId_ = 0;
    HANDLE stopEvent_ = nullptr;
    HANDLE hostMutex_ = nullptr;
    UINT taskbarCreatedMessage_ = 0;
    bool isHost_ = false;
    std::recursive_mutex engineMutex_;
    std::recursive_mutex queueMutex_;
    std::array<BalloonRequest, config::kDeliveryQueueDepth> queue_{};
    size_t queueCount_ = 0;
};


class Runtime
{
  public:
    static Runtime &instance()
    {
        static Runtime runtime;
        return runtime;
    }

    Runtime(const Runtime &) = delete;
    Runtime &operator=(const Runtime &) = delete;

    bool initialize()
    {
        if (!image().initialize())
        {
            return false;
        }
        if (image().isExcluded())
        {
            return false;
        }

        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent_)
        {
            return false;
        }

        tray().setStopEvent(stopEvent_);
        tray().initialize();
        return true;
    }

    void startWatcher(LPTHREAD_START_ROUTINE routine, PCWSTR description)
    {
        watcherThread_ = CreateThread(nullptr, 0, routine, nullptr, 0, nullptr);
        if (!watcherThread_)
        {
            Wh_Log(L"[balloon] %s worker creation failed", description);
        }
    }

    void requestStop()
    {
        if (stopEvent_)
        {
            SetEvent(stopEvent_);
        }
        if (watcherThread_)
        {
            WaitForSingleObject(watcherThread_, INFINITE);
        }
    }

    void close()
    {
        if (watcherThread_)
        {
            CloseHandle(watcherThread_);
            watcherThread_ = nullptr;
        }
        if (stopEvent_)
        {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
        tray().close();
    }

    ProcessImage &image() noexcept
    {
        return image_;
    }

    TrayEngine &tray() noexcept
    {
        return tray_;
    }

    HANDLE stopEvent() const noexcept
    {
        return stopEvent_;
    }

  private:
    Runtime() = default;

    ProcessImage image_;
    TrayEngine tray_;
    HANDLE stopEvent_ = nullptr;
    HANDLE watcherThread_ = nullptr;
};

Runtime &runtime()
{
    return Runtime::instance();
}


void TrayEngine::initialize()
{
    taskbarCreatedMessage_ = RegisterWindowMessageW(L"TaskbarCreated");

    for (size_t i = 0; i < slots_.size(); ++i)
    {
        slots_[i].id = static_cast<UINT>(i + 1);
    }

    if (!runtime().image().isExplorer())
    {
        return;
    }

    hostMutex_ = CreateMutexW(nullptr, TRUE, config::kHostMutexName);
    if (hostMutex_ && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        isHost_ = true;
    }
    else if (hostMutex_)
    {
        CloseHandle(hostMutex_);
        hostMutex_ = nullptr;
    }
}

bool TrayEngine::start()
{
    std::lock_guard<std::recursive_mutex> lock(engineMutex_);
    if (!stopEvent_ || WaitForSingleObject(stopEvent_, 0) != WAIT_TIMEOUT)
    {
        return false;
    }
    if (window())
    {
        return true;
    }

    if (thread_)
    {
        if (WaitForSingleObject(thread_, 0) != WAIT_OBJECT_0)
        {
            return false;
        }
        CloseHandle(thread_);
        thread_ = nullptr;
        threadId_ = 0;
    }

    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ready)
    {
        return false;
    }

    HANDLE threadReady = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), ready, GetCurrentProcess(), &threadReady, 0,
                         FALSE, DUPLICATE_SAME_ACCESS))
    {
        CloseHandle(ready);
        return false;
    }

    auto *context = new (std::nothrow) StartContext{this, threadReady};
    if (!context)
    {
        CloseHandle(threadReady);
        CloseHandle(ready);
        return false;
    }

    thread_ = CreateThread(nullptr, 0, &TrayEngine::threadMain, context, 0, &threadId_);
    if (!thread_)
    {
        CloseHandle(context->ready);
        delete context;
        CloseHandle(ready);
        return false;
    }

    WaitForSingleObject(ready, config::kEngineStartTimeoutMs);
    CloseHandle(ready);
    return window() != nullptr;
}

void TrayEngine::stop()
{
    std::lock_guard<std::recursive_mutex> lock(engineMutex_);
    if (threadId_)
    {
        PostThreadMessageW(threadId_, WM_QUIT, 0, 0);
    }
    if (thread_)
    {
        WaitForSingleObject(thread_, INFINITE);
    }
}

void TrayEngine::close()
{
    if (thread_)
    {
        CloseHandle(thread_);
        thread_ = nullptr;
    }
    threadId_ = 0;

    if (hostMutex_)
    {
        ReleaseMutex(hostMutex_);
        CloseHandle(hostMutex_);
        hostMutex_ = nullptr;
    }
}

DWORD WINAPI TrayEngine::threadMain(LPVOID parameter)
{
    auto *context = static_cast<StartContext *>(parameter);
    TrayEngine *engine = context->engine;
    HANDLE ready = context->ready;
    delete context;
    try
    {
        return engine->run(ready);
    }
    catch (...)
    {
        Wh_Log(L"[balloon] notification host raised a C++ exception");
        return 1;
    }
}

DWORD TrayEngine::run(HANDLE ready)
{
    struct ReadyScope
    {
        HANDLE value;
        void signal()
        {
            if (value)
            {
                SetEvent(value);
                CloseHandle(value);
                value = nullptr;
            }
        }
        ~ReadyScope() { signal(); }
    } readyScope{ready};
    const PCWSTR className = isHost_ ? config::kHostWindowClass : config::kLocalWindowClass;
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = &TrayEngine::windowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    RegisterClassW(&windowClass);

    HWND window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, className, L"",
                                  WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance,
                                  nullptr);
    if (!window)
    {
        Wh_Log(L"[balloon] notification host window creation failed");
        readyScope.signal();
        return 1;
    }

    if (isHost_)
    {

        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        auto changeFilter =
            user32 ? reinterpret_cast<ChangeWindowMessageFilterExFn>(
                         GetProcAddress(user32, "ChangeWindowMessageFilterEx"))
                   : nullptr;
        if (changeFilter)
        {
            changeFilter(window, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
        }
    }

    window_.store(window, std::memory_order_release);
    Wh_Log(L"[balloon] notification window active (host=%d)", isHost_ ? 1 : 0);
    readyScope.signal();

    if (stopEvent_ && WaitForSingleObject(stopEvent_, 0) == WAIT_OBJECT_0)
    {
        PostQuitMessage(0);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    reapExpired(true);
    window_.store(nullptr, std::memory_order_release);
    DestroyWindow(window);
    UnregisterClassW(className, instance);
    return 0;
}

LRESULT CALLBACK TrayEngine::windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    try
    {
        return runtime().tray().handleMessage(hwnd, message, wparam, lparam);
    }
    catch (...)
    {
        Wh_Log(L"[balloon] window callback raised a C++ exception: message=%u", message);
        return 0;
    }
}

LRESULT TrayEngine::handleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case config::kTrayDeliveryMessage:
        deliverNext();
        return 0;

    case WM_COPYDATA:
        if (!lparam)
        {
            return 0;
        }
        return handleCopyData(*reinterpret_cast<const COPYDATASTRUCT *>(lparam));

    case config::kTrayEventMessage:
        handleTrayCallback(LOWORD(lparam), HIWORD(lparam));
        return 0;

    case WM_TIMER:
        if (wparam == config::kCleanupTimerId)
        {
            reapExpired(false);
        }
        return 0;

    case WM_DESTROY:
        reapExpired(true);
        return 0;

    default:
        if (message == taskbarCreatedMessage_ && taskbarCreatedMessage_ != 0)
        {
            resetAfterTaskbarCreated();
            return 0;
        }
        return DefWindowProcW(hwnd, message, wparam, lparam);
    }
}

LRESULT TrayEngine::handleCopyData(const COPYDATASTRUCT &data)
{
    if (!data.lpData || data.dwData != config::kBalloonRequestMagic)
    {
        return 0;
    }

    const auto *request = static_cast<const BalloonRequest *>(data.lpData);
    if (!ipc::isValid(request, data.cbData))
    {
        return 0;
    }
    if (!policy::legacyBalloonsEnabled())
    {
        return 0;
    }

    BalloonRequest copy = *request;
    ipc::ensureTerminated(copy);
    return enqueue(copy) ? 1 : 0;
}

void TrayEngine::handleTrayCallback(UINT event, UINT id)
{
    Slot *slot = findSlot(id);
    if (!slot)
    {
        return;
    }

    switch (event)
    {
    case NIN_BALLOONUSERCLICK:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
        if (slot->pid)
        {
            window::activateProcess(slot->pid);
        }
        slot->balloonActive = false;
        slot->removeAt = GetTickCount64() + config::kBalloonCloseDelayMs;
        break;

    case NIN_BALLOONTIMEOUT:
    case NIN_BALLOONHIDE:
        slot->balloonActive = false;
        slot->removeAt = GetTickCount64() + config::kBalloonCloseDelayMs;
        break;
    }
}

void TrayEngine::deliverNext()
{
    BalloonRequest request{};
    bool have = false;

    {
        std::lock_guard<std::recursive_mutex> lock(queueMutex_);
        if (queueCount_ > 0)
        {
            request = queue_[0];
            for (size_t i = 1; i < queueCount_; ++i)
            {
                queue_[i - 1] = queue_[i];
            }
            --queueCount_;
            have = true;
        }
    }

    if (have)
    {
        present(request);
    }
}

void TrayEngine::present(const BalloonRequest &request)
{
    if (!window())
    {
        return;
    }

    Slot &slot = *acquireSlot(request.exePath);
    slot.pid = request.pid;
    slot.lastUse = GetTickCount64();
    slot.removeAt = 0;

    HICON appSmall = nullptr;
    HICON appLarge = nullptr;
    if (!slot.trayIcon)
    {
        icon::extractFromExecutable(request.exePath, appSmall, appLarge);
        slot.trayIcon = appSmall;
        if (appSmall && appSmall != icon::defaultSmall())
        {
            slot.balloonIcon = icon::enlargeForBalloon(appSmall);
        }
    }

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = window();
    nid.uID = slot.id;

    if (!slot.added)
    {
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
        nid.uCallbackMessage = config::kTrayEventMessage;
        nid.hIcon = slot.trayIcon;
        text::copyTruncated(nid.szTip,
                            request.appName[0] ? request.appName : L"Notification");

        if (!Shell_NotifyIconW(NIM_ADD, &nid))
        {
            icon::destroyOwned(appLarge);
            Wh_Log(L"[balloon] tray icon registration failed: %s", request.exePath);
            releaseSlot(slot);
            return;
        }

        slot.added = true;
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
    }

    icon::destroyOwned(appLarge);

    NOTIFYICONDATAW balloon{};
    balloon.cbSize = sizeof(balloon);
    balloon.hWnd = window();
    balloon.uID = slot.id;
    balloon.uFlags = NIF_INFO;


    if (slot.trayIcon && slot.trayIcon != icon::defaultSmall())
    {
        balloon.dwInfoFlags = NIIF_USER;
        balloon.hBalloonIcon = slot.balloonIcon ? slot.balloonIcon : slot.trayIcon;
    }
    else
    {
        balloon.dwInfoFlags = NIIF_INFO;
    }

    if (request.flags & config::kSuppressSoundFlag)
    {
        balloon.dwInfoFlags |= NIIF_NOSOUND;
    }

    text::copyTruncated(balloon.szInfoTitle,
                        request.title[0] ? request.title
                                         : (request.appName[0] ? request.appName
                                                               : L"Notification"));
    text::copyTruncated(balloon.szInfo, request.body[0] ? request.body : L" ");

    if (Shell_NotifyIconW(NIM_MODIFY, &balloon))
    {
        slot.balloonActive = true;
        Wh_Log(L"[balloon] shell accepted the balloon request");
    }
    else
    {
        Wh_Log(L"[balloon] shell rejected the balloon request: %s", request.exePath);
        slot.removeAt = GetTickCount64();
    }

    SetTimer(window(), config::kCleanupTimerId, config::kCleanupIntervalMs, nullptr);
}

void TrayEngine::reapExpired(bool force)
{
    const ULONGLONG now = GetTickCount64();
    bool anyLeft = false;

    for (auto &slot : slots_)
    {
        if (!slot.inUse)
        {
            continue;
        }

        if (force)
        {
            releaseSlot(slot);
            continue;
        }

        const bool expired = slot.removeAt != 0 && now >= slot.removeAt;
        const bool stale = now - slot.lastUse > config::kSlotIdleTimeoutMs;
        if (expired || stale)
        {
            releaseSlot(slot);
        }
        else
        {
            anyLeft = true;
        }
    }

    if (!anyLeft && window())
    {
        KillTimer(window(), config::kCleanupTimerId);
    }
}

void TrayEngine::resetAfterTaskbarCreated()
{
    for (auto &slot : slots_)
    {
        if (slot.inUse)
        {
            slot.added = false;
            slot.balloonActive = false;
            releaseSlot(slot);
        }
    }
}

TrayEngine::Slot *TrayEngine::acquireSlot(PCWSTR exePath)
{
    for (auto &slot : slots_)
    {
        if (slot.inUse && text::equalsIgnoreCase(slot.exePath, exePath))
        {
            return &slot;
        }
    }

    for (auto &slot : slots_)
    {
        if (!slot.inUse)
        {
            slot.inUse = true;
            slot.id = static_cast<UINT>(&slot - slots_.data() + 1);
            text::copyTruncated(slot.exePath, exePath);
            return &slot;
        }
    }


    Slot *oldest = &slots_[0];
    for (auto &slot : slots_)
    {
        if (slot.lastUse < oldest->lastUse)
        {
            oldest = &slot;
        }
    }

    releaseSlot(*oldest);
    oldest->inUse = true;
    text::copyTruncated(oldest->exePath, exePath);
    return oldest;
}

TrayEngine::Slot *TrayEngine::findSlot(UINT id)
{
    for (auto &slot : slots_)
    {
        if (slot.inUse && slot.id == id)
        {
            return &slot;
        }
    }
    return nullptr;
}

void TrayEngine::releaseSlot(Slot &slot)
{
    if (slot.added && window())
    {
        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd = window();
        nid.uID = slot.id;
        Shell_NotifyIconW(NIM_DELETE, &nid);
    }

    icon::destroyOwned(slot.balloonIcon);
    icon::destroyOwned(slot.trayIcon);

    const UINT id = slot.id;
    slot = Slot{};
    slot.id = id;
}

bool TrayEngine::enqueue(const BalloonRequest &request)
{
    std::lock_guard<std::recursive_mutex> lock(queueMutex_);
    HWND target = window();
    if (!target || queueCount_ >= queue_.size())
    {
        return false;
    }

    queue_[queueCount_++] = request;
    if (!PostMessageW(target, config::kTrayDeliveryMessage, 0, 0))
    {
        --queueCount_;
        return false;
    }
    return true;
}


namespace ipc
{
// https://learn.microsoft.com/en-us/windows/win32/dataxchg/wm-copydata
bool deliver(BalloonRequest &request)
{
    request.size = sizeof(request);
    request.magic = config::kBalloonRequestMagic;
    request.version = config::kBalloonRequestVersion;

    TrayEngine &engine = runtime().tray();
    if (engine.isHost())
    {
        if (!policy::legacyBalloonsEnabled() || !engine.start())
        {
            return false;
        }
        return engine.enqueue(request);
    }

    HWND host = FindWindowW(config::kHostWindowClass, nullptr);
    if (host)
    {
        COPYDATASTRUCT data{};
        data.dwData = config::kBalloonRequestMagic;
        data.cbData = static_cast<DWORD>(sizeof(request));
        data.lpData = &request;

        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(host, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&data),
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, config::kHostProbeTimeoutMs,
                                &result))
        {
            return result != 0;
        }
        Wh_Log(L"[balloon] host delivery did not complete within the wait period");
    }

    if (!policy::legacyBalloonsEnabled() || !engine.start())
    {
        return false;
    }
    return engine.enqueue(request);
}
}


class RecentToasts
{
  public:
    bool isRecent(DWORD key, bool hasProgress)
    {
        const ULONGLONG window =
            hasProgress ? config::kProgressRepeatWindowMs : config::kRepeatWindowMs;
        const ULONGLONG now = GetTickCount64();
        bool repeat = false;

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (auto &entry : entries_)
        {
            if (entry.key == key && entry.time != 0 && now - entry.time < window)
            {
                entry.time = now;
                repeat = true;
                break;
            }
        }
        return repeat;
    }

    void remember(DWORD key)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        entries_[next_].key = key;
        entries_[next_].time = GetTickCount64();
        next_ = (next_ + 1) % entries_.size();
    }

  private:
    struct Entry
    {
        ULONGLONG time = 0;
        DWORD key = 0;
    };

    std::array<Entry, config::kRecentToasts> entries_{};
    size_t next_ = 0;
    std::recursive_mutex mutex_;
};

RecentToasts &recentToasts()
{
    static RecentToasts instance;
    return instance;
}


namespace conversion
{
enum class Result
{
    Converted,
    Duplicate,
    NotConverted,
};

Result convert(const ToastContent &content, PCWSTR exePath, PCWSTR appName, DWORD processId)
{
    const DWORD key = toast::dedupeKey(content);
    if (recentToasts().isRecent(key, content.hasProgress))
    {
        return Result::Duplicate;
    }

    BalloonRequest request{};
    request.pid = processId;
    request.flags = content.silent ? config::kSuppressSoundFlag : 0;
    text::copyTruncated(request.title, content.title);
    text::copyTruncated(request.body, content.body);
    text::copyTruncated(request.appName, appName);
    text::copyTruncated(request.exePath, exePath);

    if (!ipc::deliver(request))
    {
        Wh_Log(L"[balloon] queue unavailable; original notification continues");
        return Result::NotConverted;
    }

    recentToasts().remember(key);
    Wh_Log(L"[balloon] request queued for delivery");
    return Result::Converted;
}
}


namespace interception
{
namespace
{
using ToastNotifierShowFn =
    HRESULT(STDMETHODCALLTYPE *)(void *, void *, void *, void *, void *, void *);
using CreateToastNotifierWithIdFn = HRESULT(STDMETHODCALLTYPE *)(void *, HSTRING, void **);
using GetActivationFactoryFn = HRESULT(WINAPI *)(HSTRING, REFIID, void **);

std::atomic<bool> g_showHooked{false};
std::atomic<bool> g_activationFactoryHooked{false};
std::atomic<void *> g_notifierVtable{nullptr};
ToastNotifierShowFn g_originalShow = nullptr;
GetActivationFactoryFn g_originalActivationFactory = nullptr;

constexpr WCHAR kPowerShellAppId[] =
    L"{1AC14E77-02E7-4E5D-B744-2EB1AE5198B7}\\WindowsPowerShell\\v1.0\\powershell.exe";
}

HRESULT onNotifierShow(void *notifier, void *notification, void *arg3, void *arg4,
                       void *arg5, void *arg6)
{


    if (!notifier ||
        *static_cast<void **>(notifier) != g_notifierVtable.load(std::memory_order_acquire))
    {
        static std::atomic<bool> logged{false};
        if (!logged.exchange(true))
        {
            Wh_Log(L"[balloon] notifier table differs; original call continues");
        }
        return g_originalShow(notifier, notification, arg3, arg4, arg5, arg6);
    }
    if (!notification || !policy::conversionAllowed())
    {
        return g_originalShow(notifier, notification, arg3, arg4, arg5, arg6);
    }

    bool handled = false;
    try
    {
        Wh_Log(L"[balloon] application notification received");
        thread_local WCHAR xml[config::kXmlBufferChars];
        if (winrt::toastXml(notification, xml, ARRAYSIZE(xml)))
        {
            const auto content = toast::parse(xml);
            if (content)
            {
                handled = conversion::convert(*content, runtime().image().path(),
                                              runtime().image().description(),
                                              GetCurrentProcessId()) != conversion::Result::NotConverted;
            }
        }
        else
        {
            Wh_Log(L"[balloon] notification content unavailable; original call continues");
        }
    }
    catch (...)
    {
        Wh_Log(L"[balloon] application conversion raised a C++ exception");
    }
    if (handled)
    {
        return S_OK;
    }
    return g_originalShow(notifier, notification, arg3, arg4, arg5, arg6);
}

void patchNotifierShow(void *notifier)
{
    if (!notifier)
    {
        return;
    }

    bool expected = false;
    if (!g_showHooked.compare_exchange_strong(expected, true))
    {
        return;
    }

    void **vtable = *static_cast<void ***>(notifier);
    void *showAddress = vtable[winrt::kFirstMethod];
    g_notifierVtable.store(vtable, std::memory_order_release);

    if (!Wh_SetFunctionHook(showAddress, reinterpret_cast<void *>(&onNotifierShow),
                            reinterpret_cast<void **>(&g_originalShow)))
    {
        g_showHooked.store(false);
        Wh_Log(L"[balloon] notifier interception registration failed");
        return;
    }
    if (!Wh_ApplyHookOperations())
    {
        g_showHooked.store(false);
        Wh_Log(L"[balloon] notifier interception activation failed");
        return;
    }

    Wh_Log(L"[balloon] notifier interception ready: address=%p vtable=%p", showAddress, vtable);
}

void resolveNotifierShow(void *statics)
{
    if (g_showHooked.load())
    {
        return;
    }

    void **vtable = *static_cast<void ***>(statics);
    auto createWithId =
        reinterpret_cast<CreateToastNotifierWithIdFn>(vtable[winrt::kFirstMethod + 1]);

    const bool isPowerShell = runtime().image().isPowerShell();
    const PCWSTR candidates[] = {isPowerShell ? kPowerShellAppId : runtime().image().path(),
                                 runtime().image().path()};
    const int candidateCount = isPowerShell ? 2 : 1;

    void *notifier = nullptr;
    HRESULT result = E_FAIL;
    for (int i = 0; i < candidateCount; ++i)
    {
        winrt::HString appId;
        if (!appId.assign(candidates[i]))
        {
            continue;
        }

        result = createWithId(statics, appId.get(), &notifier);
        if (SUCCEEDED(result) && notifier)
        {
            break;
        }

        if (notifier)
        {
            static_cast<IUnknown *>(notifier)->Release();
            notifier = nullptr;
        }
    }

    if (!notifier)
    {
        Wh_Log(L"[balloon] notifier discovery failed: HRESULT=0x%08X",
               static_cast<unsigned int>(result));
        return;
    }


    com::UnknownOwner notifierOwner(notifier);
    void *toastNotifier = com::queryInterface(notifier, iid::IToastNotifier);
    if (toastNotifier)
    {
        com::UnknownOwner interfaceOwner(toastNotifier);
        patchNotifierShow(toastNotifier);
    }
}

void onToastManagerFactory(void *factory)
{


    void *statics = com::queryInterface(factory, iid::IToastNotificationManagerStatics);
    if (!statics)
    {
        return;
    }

    struct ReferenceScope
    {
        IUnknown *value;
        ~ReferenceScope() { value->Release(); }
    } reference{static_cast<IUnknown *>(statics)};
    resolveNotifierShow(statics);
}

HRESULT WINAPI activationFactory(HSTRING classId, REFIID iid, void **factory)
{
    const HRESULT result = g_originalActivationFactory(classId, iid, factory);

    try
    {
        if (SUCCEEDED(result) && factory && *factory && !g_showHooked.load())
        {
            UINT32 length = 0;
            PCWSTR name = winrt::rawBuffer(classId, &length);
            if (name && wcscmp(name, L"Windows.UI.Notifications.ToastNotificationManager") == 0)
            {
                Wh_Log(L"[balloon] notification manager factory observed");
                onToastManagerFactory(*factory);
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"[balloon] factory inspection raised a C++ exception");
    }
    return result;
}

bool installActivationFactoryHook(bool applyNow)
{
    if (g_activationFactoryHooked.load() || !winrt::resolve())
    {
        return false;
    }

    HMODULE combase = GetModuleHandleW(L"combase.dll");
    if (!combase)
    {
        return false;
    }

    void *target =
        reinterpret_cast<void *>(GetProcAddress(combase, "RoGetActivationFactory"));
    if (!target)
    {
        return false;
    }

    bool expected = false;
    if (!g_activationFactoryHooked.compare_exchange_strong(expected, true))
    {
        return true;
    }

    if (!Wh_SetFunctionHook(target, reinterpret_cast<void *>(&activationFactory),
                            reinterpret_cast<void **>(&g_originalActivationFactory)))
    {
        g_activationFactoryHooked.store(false);
        return false;
    }

    if (applyNow && !Wh_ApplyHookOperations())
    {
        Wh_Log(L"[balloon] factory interception activation failed");
        return false;
    }
    return true;
}

// https://learn.microsoft.com/en-us/windows/win32/api/roapi/nf-roapi-roinitialize
void attachExistingClient()
{
    if (g_showHooked.load() || !winrt::isResolved())
    {
        return;
    }

    HMODULE combase = GetModuleHandleW(L"combase.dll");
    auto getActivationFactory =
        g_originalActivationFactory ? g_originalActivationFactory :
        (combase ? reinterpret_cast<GetActivationFactoryFn>(
                       GetProcAddress(combase, "RoGetActivationFactory"))
                 : nullptr);
    if (!getActivationFactory)
    {
        return;
    }

    Wh_Log(L"[balloon] inspecting the notification factory");
    const bool isPowerShell = runtime().image().isPowerShell();
    const HRESULT result = RoInitialize(isPowerShell ? RO_INIT_SINGLETHREADED : RO_INIT_MULTITHREADED);
    if (FAILED(result))
    {
        Wh_Log(L"[balloon] WinRT initialization failed: HRESULT=0x%08X",
               static_cast<unsigned int>(result));
        return;
    }

    struct WinrtScope
    {
        ~WinrtScope() { RoUninitialize(); }
    } winrtScope;

    winrt::HString className;
    if (className.assign(L"Windows.UI.Notifications.ToastNotificationManager"))
    {
        void *statics = nullptr;
        if (SUCCEEDED(getActivationFactory(className.get(),
                                          iid::IToastNotificationManagerStatics, &statics)) &&
            statics)
        {
            com::UnknownOwner factoryOwner(statics);
            onToastManagerFactory(statics);
        }
    }

}

DWORD WINAPI deferredWorker(LPVOID)
{
    try
    {
        const bool isPowerShell = runtime().image().isPowerShell();

        for (DWORD delay = 201; delay <= 12801; delay = (delay - 1) * 2 + 1)
        {
            if (!g_activationFactoryHooked.load() && installActivationFactoryHook(true))
            {
                Wh_Log(L"[balloon] deferred factory interception ready");
            }

            if (g_activationFactoryHooked.load() && !g_showHooked.load() &&
                (isPowerShell || GetModuleHandleW(L"Windows.UI.dll")))
            {
                attachExistingClient();
            }

            if (g_showHooked.load())
            {
                break;
            }
            if (WaitForSingleObject(runtime().stopEvent(), delay) != WAIT_TIMEOUT)
            {
                return 0;
            }
        }
        return 0;
    }
    catch (...)
    {
        Wh_Log(L"[balloon] background worker raised a C++ exception");
        return 0;
    }
}

bool isShowHooked()
{
    return g_showHooked.load();
}
}


namespace platform
{
namespace
{
using NotificationGetStringFn = HRESULT (*)(void *, WCHAR **);
using NotificationGetPayloadFn = HRESULT (*)(void *, IStream **);
using ProcessToastDeliveryFn = void (*)(void *, void *, void *, void *, PCWSTR);

constexpr WCHAR kSymbolProcessToastDelivery[] =
    L"?ProcessToastDelivery@SessionManager@@AEAAXPEAVNotification@@PEAUINotificationHandler@@"
    L"PEAVTransientNotificationDetails@@PEBG@Z";
constexpr WCHAR kSymbolGetPayload[] =
    L"?get_Payload@Notification@@UEAAJPEAPEAUIStream@@@Z";
constexpr WCHAR kSymbolGetNotificationType[] =
    L"?get_NotificationType@Notification@@UEAAJPEAPEAG@Z";
constexpr WCHAR kSymbolGetHandlerKey[] =
    L"?get_HandlerKey@Notification@@UEAAJPEAPEAG@Z";

std::atomic<bool> g_active{false};
NotificationGetStringFn g_getNotificationType = nullptr;
NotificationGetStringFn g_getHandlerKey = nullptr;
NotificationGetPayloadFn g_getPayload = nullptr;
ProcessToastDeliveryFn g_originalDelivery = nullptr;

bool isBlockedSender(PCWSTR sender)
{
    for (PCWSTR term : config::kBlockedSenders)
    {
        if (wcsstr(sender, term))
        {
            return true;
        }
    }
    return false;
}
}

bool readPayload(IStream *stream, WCHAR *out, size_t capacity)
{
    if (!stream || !out || capacity == 0)
    {
        return false;
    }
    out[0] = L'\0';

    thread_local BYTE raw[config::kPayloadBufferBytes];

    LARGE_INTEGER zero{};
    ULARGE_INTEGER originalPosition{};
    if (FAILED(stream->Seek(zero, STREAM_SEEK_CUR, &originalPosition)))
    {
        return false;
    }

    struct RestorePosition
    {
        IStream *stream;
        ULARGE_INTEGER position;

        ~RestorePosition()
        {
            LARGE_INTEGER offset{};
            offset.QuadPart = static_cast<LONGLONG>(position.QuadPart);
            stream->Seek(offset, STREAM_SEEK_SET, nullptr);
        }
    } restore{stream, originalPosition};

    if (FAILED(stream->Seek(zero, STREAM_SEEK_SET, nullptr)))
    {
        return false;
    }

    ULONG total = 0;
    while (total < sizeof(raw))
    {
        ULONG got = 0;
        if (FAILED(stream->Read(raw + total, static_cast<ULONG>(sizeof(raw) - total), &got)) ||
            got == 0)
        {
            break;
        }
        total += got;
    }

    if (total < 4)
    {
        return false;
    }

    const BYTE *data = raw;
    ULONG size = total;

    if (size >= 2 && data[0] == 0xFF && data[1] == 0xFE)
    {
        data += 2;
        size -= 2;
    }
    else if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
    {
        data += 3;
        size -= 3;
        const int written = MultiByteToWideChar(
            CP_UTF8, 0, reinterpret_cast<const char *>(data), static_cast<int>(size), out,
            static_cast<int>(capacity) - 1);
        out[written > 0 ? written : 0] = L'\0';
        return out[0] != L'\0';
    }
    else if (size >= 2 && data[1] != 0x00)
    {
        const int written = MultiByteToWideChar(
            CP_UTF8, 0, reinterpret_cast<const char *>(data), static_cast<int>(size), out,
            static_cast<int>(capacity) - 1);
        out[written > 0 ? written : 0] = L'\0';
        return out[0] != L'\0';
    }

    const size_t chars =
        std::min(static_cast<size_t>(size / sizeof(WCHAR)), capacity - 1);
    memcpy(out, data, chars * sizeof(WCHAR));
    out[chars] = L'\0';
    return out[0] != L'\0';
}

void resolveDisplayName(PCWSTR aumid, WCHAR *out, size_t capacity)
{
    out[0] = L'\0';
    if (!aumid || !aumid[0])
    {
        return;
    }

    WCHAR key[512];
    text::copyTruncated(key, L"AppUserModelId\\");
    const size_t used = wcslen(key);
    text::copyTruncated(key + used, ARRAYSIZE(key) - used, aumid);

    WCHAR value[256]{};
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CLASSES_ROOT, key, L"DisplayName", RRF_RT_REG_SZ, nullptr, value,
                     &size) == ERROR_SUCCESS &&
        value[0])
    {
        if (value[0] == L'@')
        {
            WCHAR resolved[256]{};
            if (SUCCEEDED(SHLoadIndirectString(value, resolved, ARRAYSIZE(resolved), nullptr)) &&
                resolved[0])
            {
                text::copyTruncated(out, capacity, resolved);
                return;
            }
        }
        else
        {
            text::copyTruncated(out, capacity, value);
            return;
        }
    }

    text::copyTruncated(out, capacity, aumid);
    WCHAR *bang = wcschr(out, L'!');
    if (bang)
    {
        *bang = L'\0';
    }
}

bool convert(void *notification)
{
    if (!notification || !g_getPayload || !g_getNotificationType)
    {
        return false;
    }
    if (!policy::conversionAllowed())
    {
        return false;
    }

    WCHAR *type = nullptr;
    if (FAILED(g_getNotificationType(notification, &type)) || !type)
    {
        return false;
    }

    const bool isToast = text::equalsIgnoreCase(type, L"toast");
    CoTaskMemFree(type);
    if (!isToast)
    {
        return false;
    }

    if (!g_getHandlerKey)
    {
        return false;
    }

    WCHAR *sender = nullptr;
    if (FAILED(g_getHandlerKey(notification, &sender)) || !sender || !sender[0])
    {
        if (sender)
        {
            CoTaskMemFree(sender);
        }
        return false;
    }

    WCHAR senderLower[512]{};
    if (wcslen(sender) >= ARRAYSIZE(senderLower))
    {
        CoTaskMemFree(sender);
        return false;
    }

    text::copyTruncated(senderLower, sender);
    CoTaskMemFree(sender);
    _wcslwr_s(senderLower, ARRAYSIZE(senderLower));

    if (isBlockedSender(senderLower))
    {
        return false;
    }

    IStream *stream = nullptr;
    if (FAILED(g_getPayload(notification, &stream)) || !stream)
    {
        return false;
    }
    com::StreamOwner streamOwner(stream);

    thread_local WCHAR xml[config::kXmlBufferChars];
    if (!readPayload(stream, xml, ARRAYSIZE(xml)))
    {
        return false;
    }

    const auto content = toast::parse(xml);
    if (!content)
    {
        return false;
    }

    WCHAR appName[96]{};
    resolveDisplayName(senderLower, appName, ARRAYSIZE(appName));

    const auto result = conversion::convert(*content, L"", appName, 0);
    if (result == conversion::Result::Converted)
    {
        Wh_Log(L"[balloon] platform request queued: %s", appName);
        return true;
    }
    return result == conversion::Result::Duplicate;
}

void processToastDeliveryHook(void *pThis, void *notification, void *handler, void *details,
                              PCWSTR arg)
{
    if (!g_originalDelivery)
    {
        Wh_Log(L"[balloon] original platform delivery unavailable");
        return;
    }
    try
    {
        if (convert(notification))
        {
            return;
        }
    }
    catch (...)
    {
        Wh_Log(L"[balloon] service conversion raised a C++ exception; original call continues");
    }
    g_originalDelivery(pThis, notification, handler, details, arg);
}

bool installHook(HMODULE wpncore)
{
    if (g_active.load())
    {
        return true;
    }

    WH_FIND_SYMBOL_OPTIONS options{};
    options.optionsSize = sizeof(options);

    WH_FIND_SYMBOL symbol{};
    HANDLE find = Wh_FindFirstSymbol(wpncore, &options, &symbol);
    if (!find)
    {
        Wh_Log(L"[balloon] notification platform symbols unavailable");
        return false;
    }

    struct SymbolSearchScope
    {
        HANDLE handle;
        void close()
        {
            if (handle)
            {
                Wh_FindCloseSymbol(handle);
                handle = nullptr;
            }
        }
        ~SymbolSearchScope() { close(); }
    } symbolSearch{find};

    void *deliver = nullptr;
    do
    {
        if (!symbol.symbolDecorated)
        {
            continue;
        }

        if (wcscmp(symbol.symbolDecorated, kSymbolProcessToastDelivery) == 0)
        {
            deliver = symbol.address;
        }
        else if (wcscmp(symbol.symbolDecorated, kSymbolGetPayload) == 0)
        {
            g_getPayload = reinterpret_cast<NotificationGetPayloadFn>(symbol.address);
        }
        else if (wcscmp(symbol.symbolDecorated, kSymbolGetNotificationType) == 0)
        {
            g_getNotificationType =
                reinterpret_cast<NotificationGetStringFn>(symbol.address);
        }
        else if (wcscmp(symbol.symbolDecorated, kSymbolGetHandlerKey) == 0)
        {
            g_getHandlerKey = reinterpret_cast<NotificationGetStringFn>(symbol.address);
        }
    } while (Wh_FindNextSymbol(find, &symbol));
    symbolSearch.close();

    if (!deliver || !g_getPayload || !g_getNotificationType || !g_getHandlerKey)
    {
        Wh_Log(L"[balloon] platform interception requires additional symbols");
        return false;
    }

    if (!Wh_SetFunctionHook(deliver, reinterpret_cast<void *>(&processToastDeliveryHook),
                            reinterpret_cast<void **>(&g_originalDelivery)))
    {
        Wh_Log(L"[balloon] platform interception registration failed");
        return false;
    }

    if (!Wh_ApplyHookOperations())
    {
        Wh_Log(L"[balloon] platform interception activation failed; requesting rollback");
        if (!Wh_RemoveFunctionHook(deliver))
        {
            Wh_Log(L"[balloon] platform interception rollback could not be registered");
        }
        else if (!Wh_ApplyHookOperations())
        {
            Wh_Log(L"[balloon] platform interception rollback could not be applied");
        }
        return false;
    }

    g_active.store(true);
    Wh_Log(L"[balloon] platform interception ready: address=%p", deliver);
    return true;
}

DWORD WINAPI watchWorker(LPVOID)
{
    try
    {
        for (DWORD delay = 0; delay <= config::kWatcherMaxDelayMs;
             delay = delay ? (delay - 1) * 2 + 1 : 201)
        {
            if (delay && WaitForSingleObject(runtime().stopEvent(), delay) != WAIT_TIMEOUT)
            {
                return 0;
            }

            HMODULE wpncore = GetModuleHandleW(L"wpncore.dll");
            if (wpncore)
            {
                installHook(wpncore);
                return 0;
            }
        }
        return 0;
    }
    catch (...)
    {
        Wh_Log(L"[balloon] background worker raised a C++ exception");
        return 0;
    }
}

}


namespace lifecycle
{
BOOL initialize()
{
    Runtime &rt = runtime();
    if (!rt.initialize())
    {
        return FALSE;
    }

    if (rt.image().isExplorer())
    {
        register_explorer_registry_interceptor();
    }

    if (rt.image().isServiceHost())
    {
        Wh_Log(L"[balloon] service process initialized");
        return TRUE;
    }

    if (interception::installActivationFactoryHook(false))
    {
        Wh_Log(L"[balloon] factory interception registered");
    }
    else
    {
        Wh_Log(L"[balloon] factory module pending; worker will retry");
    }

    Wh_Log(L"[balloon] process initialized: %s%s", rt.image().name(),
           rt.tray().isHost() ? L" (balloon host)" : L"");
    return TRUE;
}

void afterInitialize()
{
    verify_explorer_registry_interceptor();
    Runtime &rt = runtime();

    if (rt.tray().isHost())
    {
        rt.tray().start();
    }

    if (rt.image().isServiceHost())
    {
        rt.startWatcher(&platform::watchWorker, L"platform");
        return;
    }

    if (!interception::isShowHooked())
    {
        rt.startWatcher(&interception::deferredWorker, L"API");
    }
}

void beforeShutdown()
{
    registry_interceptor_verified.store(false);
    Runtime &rt = runtime();
    rt.requestStop();
    rt.tray().stop();
}

void shutdown()
{
    runtime().close();
}
}

}

BOOL Wh_ModInit()
{
    try
    {
        return balloon::lifecycle::initialize();
    }
    catch (...)
    {
        Wh_Log(L"[balloon] initialization raised a C++ exception");
        try
        {
            balloon::runtime().close();
        }
        catch (...)
        {
            Wh_Log(L"[balloon] initialization cleanup raised a C++ exception");
        }
        return FALSE;
    }
}

void Wh_ModAfterInit()
{
    try
    {
        balloon::lifecycle::afterInitialize();
    }
    catch (...)
    {
        Wh_Log(L"[balloon] startup raised a C++ exception");
    }
}

void Wh_ModBeforeUninit()
{
    try
    {
        balloon::lifecycle::beforeShutdown();
    }
    catch (...)
    {
        Wh_Log(L"[balloon] stopping workers raised a C++ exception");
    }
}

void Wh_ModUninit()
{
    try
    {
        balloon::lifecycle::shutdown();
    }
    catch (...)
    {
        Wh_Log(L"[balloon] resource cleanup raised a C++ exception");
    }
}
