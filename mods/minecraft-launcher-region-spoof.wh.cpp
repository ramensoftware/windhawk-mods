// ==WindhawkMod==
// @id              minecraft-launcher-region-spoof
// @name            Minecraft launcher region detection spoof
// @name:zh-CN      启动器设备区域伪装
// @description     Spoof process region signals for PCL CE and HMCL.
// @description:zh-CN 伪装PCL CE和HMCL读取到的设备信息
// @version         0.1.0
// @author          Creeam
// @github          https://github.com/HaHaWTH
// @include         PCL2_CE*.exe
// @include         HMCL*.exe
// @include         java.exe
// @include         javaw.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minecraft launcher region detection spoof

Presents Singapore region information to PCL CE and HMCL to bypass their
mainland China restrictions on downloading, installing and repairing Minecraft.
The purpose is to give users the same choice of launcher features across regions.

## Region and time zone

- PCL CE: user region `SG`, Windows device region GeoID `215`, and
  `Singapore Standard Time`.
- HMCL: Windows user GeoID `215`, JVM country `SG`, and JVM time zone
  `Asia/Singapore`. Existing `user.country` and `user.timezone` JVM options are
  replaced, and legacy `user.region` options are removed.
- The affected launcher's time zone is fixed to UTC+8, without daylight saving
  time. Local times shown by the launcher use UTC+8 even when Windows is set to
  another time zone.
- Language settings are preserved. Locale-dependent formats can reflect the
  Singapore region. The overrides apply within the target processes.

## Target processes

Supports 64-bit x64 and ARM64 processes with these executable names:

- `PCL2_CE*.exe` for PCL CE.
- `HMCL*.exe` for HMCL executables.
- `java.exe` and `javaw.exe` when the command line launches `HMCL*.jar`,
  a JAR embedded in `HMCL*.exe`, or the main class `org.jackhuang.hmcl.Main`.

## Usage

Enable the mod before starting HMCL so that its region options are set before
JVM creation. Restart PCL CE after enabling the mod to refresh its cached region
and local time zone. To restore the original region behavior, disable the mod and
fully restart the affected launcher.

## Upstream changes

- [PCL CE #3673](https://github.com/PCL-Community/PCL-CE/pull/3673) introduces
  mainland China restrictions for Minecraft and modpack downloads, installation
  and repair, using Windows region, locale, language and time zone signals.
- [PCL CE #3676](https://github.com/PCL-Community/PCL-CE/pull/3676) adds the Windows
  device region as an additional detection signal.
- [HMCL #7017](https://github.com/HMCL-dev/HMCL/pull/7017) introduces mainland China
  restrictions for game downloads.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <cstring>
#include <cwchar>
#include <vector>

namespace PclCe {

constexpr WCHAR kCountry[] = L"SG";
constexpr WCHAR kTimeZone[] = L"Singapore Standard Time";
constexpr DWORD kDeviceGeoId = 215;
constexpr WCHAR kDeviceKey64[] = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\DeviceRegion";
constexpr WCHAR kDeviceKey32[] = L"\\REGISTRY\\MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\DeviceRegion";

using NtQueryKey_t = LONG(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);
NtQueryKey_t g_ntQueryKey;
decltype(&RegQueryValueExW) g_regQueryValueExW;

int WINAPI GeoNameHook(PWSTR buffer, int count) {
    constexpr int required = ARRAYSIZE(kCountry); // also NUL
    if (count < 0 || (count != 0 && buffer == nullptr)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (count == 0) return required;
    if (count < required) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::memcpy(buffer, kCountry, sizeof(kCountry));
    Wh_Log(L"GetUserDefaultGeoName -> SG");
    return required;
}

DWORD WINAPI DynamicTimeZoneHook(PDYNAMIC_TIME_ZONE_INFORMATION info) {
    *info = {};
    info->Bias = -480;
    std::memcpy(info->StandardName, kTimeZone, sizeof(kTimeZone));
    std::memcpy(info->DaylightName, kTimeZone, sizeof(kTimeZone));
    std::memcpy(info->TimeZoneKeyName, kTimeZone, sizeof(kTimeZone));
    info->DynamicDaylightTimeDisabled = TRUE;
    Wh_Log(L"GetDynamicTimeZoneInformation -> Singapore Standard Time");
    return TIME_ZONE_ID_UNKNOWN; // A valid fixed-offset zone without DST
}

bool IsDeviceRegionKey(HKEY key) {
    struct {
        ULONG nameBytes;
        WCHAR name[512];
    } info{};
    ULONG resultBytes = 0;
    if (g_ntQueryKey(key, 3, &info, sizeof(info), &resultBytes) < 0)
        return false;

    const int chars = static_cast<int>(info.nameBytes / sizeof(WCHAR));
    return CompareStringOrdinal(info.name, chars, kDeviceKey64, -1, TRUE) == CSTR_EQUAL ||
           CompareStringOrdinal(info.name, chars, kDeviceKey32, -1, TRUE) == CSTR_EQUAL;
}

LSTATUS WINAPI RegistryValueHook(HKEY key, LPCWSTR valueName, LPDWORD reserved,
                                LPDWORD type, LPBYTE data, LPDWORD dataBytes) {
    if (valueName == nullptr || _wcsicmp(valueName, L"DeviceRegion") != 0 ||
        reserved != nullptr || !IsDeviceRegionKey(key) ||
        (data != nullptr && dataBytes == nullptr)) {
        return g_regQueryValueExW(key, valueName, reserved, type, data, dataBytes);
    }

    if (type != nullptr) *type = REG_DWORD;
    if (data == nullptr) {
        if (dataBytes != nullptr) *dataBytes = sizeof(kDeviceGeoId);
        return ERROR_SUCCESS;
    }
    const DWORD capacity = *dataBytes;
    *dataBytes = sizeof(kDeviceGeoId);
    if (capacity < sizeof(kDeviceGeoId)) return ERROR_MORE_DATA;
    std::memcpy(data, &kDeviceGeoId, sizeof(kDeviceGeoId));
    Wh_Log(L"DeviceRegion -> 215 (Singapore), process only");
    return ERROR_SUCCESS;
}

bool InstallHook(HMODULE module, const char* name, void* detour, void** original) {
    const auto target = module ? GetProcAddress(module, name) : nullptr;
    if (target == nullptr) {
        Wh_Log(L"Missing API: %S", name);
        return false;
    }
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(target), detour, original)) {
        Wh_Log(L"Hook registration failed: %S", name);
        return false;
    }
    return true;
}

BOOL Init() {
    g_ntQueryKey = reinterpret_cast<NtQueryKey_t>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryKey"));
    if (g_ntQueryKey == nullptr) {
        Wh_Log(L"Missing API: NtQueryKey");
        return FALSE;
    }

    const auto kernel = GetModuleHandleW(L"kernel32.dll");
    const auto registry = LoadLibraryW(L"advapi32.dll");
    if (!InstallHook(kernel, "GetUserDefaultGeoName",
                     reinterpret_cast<void*>(GeoNameHook), nullptr) ||
        !InstallHook(kernel, "GetDynamicTimeZoneInformation",
                     reinterpret_cast<void*>(DynamicTimeZoneHook), nullptr) ||
        !InstallHook(registry, "RegQueryValueExW",
                     reinterpret_cast<void*>(RegistryValueHook),
                     reinterpret_cast<void**>(&g_regQueryValueExW))) {
        return FALSE;
    }
    Wh_Log(L"Registered all 3 region hooks");
    return TRUE;
}

} // namespace PclCe

namespace Hmcl {

// JNI Invocation API ABI
struct VmOption {
    char* optionString;
    void* extraInfo;
};
struct VmInitArgs {
    int version;
    int nOptions;
    VmOption* options;
    unsigned char ignoreUnrecognized;
};
static_assert(sizeof(VmOption) == 16 && sizeof(VmInitArgs) == 24);

using CreateVm_t = int(WINAPI*)(void**, void**, void*);
using GetCreatedVms_t = int(WINAPI*)(void**, int, int*);
decltype(&GetProcAddress) g_getProcAddress;
decltype(&GetUserGeoID) g_getUserGeoId;
CreateVm_t g_createVm;
SRWLOCK g_vmHookLock = SRWLOCK_INIT;
bool g_vmHookAttempted = false;
bool g_vmHookReady = false;

const WCHAR* FileName(const WCHAR* path) {
    const WCHAR* name = path;
    for (const WCHAR* p = path; *p; ++p)
        if (*p == L'\\' || *p == L'/') name = p + 1;
    return name;
}

bool IsHmclFile(const WCHAR* path, const WCHAR* extension) {
    const auto name = FileName(path);
    const size_t length = std::wcslen(name);
    const size_t extLength = std::wcslen(extension);
    return length >= 4 + extLength && _wcsnicmp(name, L"HMCL", 4) == 0 &&
           _wcsicmp(name + length - extLength, extension) == 0;
}

bool IsHmclTarget(const WCHAR* executable, const WCHAR* commandLine) {
    if (IsHmclFile(executable, L".exe")) return true;
    const auto name = FileName(executable);
    if (_wcsicmp(name, L"java.exe") != 0 && _wcsicmp(name, L"javaw.exe") != 0)
        return false;

    int argc = 0;
    auto argv = CommandLineToArgvW(commandLine, &argc);
    if (argv == nullptr) return false;
    bool matched = false;
    for (int i = 1; i < argc; ++i) {
        if (std::wcscmp(argv[i], L"-jar") == 0) {
            // The Java launcher treats the remaining tokens as application args
            matched = i + 1 < argc &&
                (IsHmclFile(argv[i + 1], L".jar") || IsHmclFile(argv[i + 1], L".exe"));
            break;
        }
        if (argv[i][0] != L'-') {
            matched = std::wcscmp(argv[i], L"org.jackhuang.hmcl.Main") == 0;
            break;
        }
        if (std::wcscmp(argv[i], L"-m") == 0 || std::wcscmp(argv[i], L"--module") == 0)
            break;
        // These Java launcher options take their operand as the next token
        // so stop at the actual main class rather than scanning application args
        constexpr const WCHAR* withOperand[] = {
            L"-cp", L"-classpath", L"--class-path", L"-p", L"--module-path",
            L"--upgrade-module-path", L"--add-modules", L"--limit-modules",
            L"--add-exports", L"--add-opens", L"--add-reads", L"--patch-module",
            L"--enable-native-access", L"--source"
        };
        for (const WCHAR* option : withOperand) {
            if (std::wcscmp(argv[i], option) == 0) {
                ++i;
                break;
            }
        }
    }
    LocalFree(argv);
    return matched;
}

bool IsRegionOption(const char* option) {
    constexpr const char* properties[] = {
        "-Duser.country", "-Duser.timezone", "-Duser.region"
    };
    for (const char* property : properties) {
        const size_t length = std::strlen(property);
        if (std::strncmp(option, property, length) == 0 &&
            (option[length] == '=' || option[length] == '\0')) return true;
    }
    return false;
}

std::vector<VmOption> RegionOptions(const VmInitArgs& args) {
    static char country[] = "-Duser.country=SG";
    static char timeZone[] = "-Duser.timezone=Asia/Singapore";
    std::vector<VmOption> options;
    options.reserve(static_cast<size_t>(args.nOptions) + 2);
    for (int i = 0; i < args.nOptions; ++i) {
        if (!IsRegionOption(args.options[i].optionString))
            options.push_back(args.options[i]);
    }
    // Legacy user.region is removed cuz Java gives it precedence over user.country.
    options.push_back({country, nullptr});
    options.push_back({timeZone, nullptr});
    return options;
}

int WINAPI CreateVmHook(void** vm, void** env, void* rawArgs) {
    const auto& original = *static_cast<const VmInitArgs*>(rawArgs);
    auto options = RegionOptions(original);
    auto modified = original;
    modified.nOptions = static_cast<int>(options.size());
    modified.options = options.data();
    Wh_Log(L"JNI_CreateJavaVM: country=SG, timezone=Asia/Singapore");
    const int result = g_createVm(vm, env, &modified);
    Wh_Log(L"JNI_CreateJavaVM returned %d", result);
    return result;
}

GEOID WINAPI UserGeoIdHook(GEOCLASS geoClass) {
    if (geoClass == GEOCLASS_NATION) {
        Wh_Log(L"GetUserGeoID(GEOCLASS_NATION) -> 215");
        return 215;
    }
    return g_getUserGeoId(geoClass);
}

bool InstallVmHook(FARPROC address, bool applyNow) {
    AcquireSRWLockExclusive(&g_vmHookLock);
    if (!g_vmHookAttempted) {
        g_vmHookAttempted = true;
        if (Wh_SetFunctionHook(reinterpret_cast<void*>(address),
                               reinterpret_cast<void*>(CreateVmHook),
                               reinterpret_cast<void**>(&g_createVm))) {
            g_vmHookReady = !applyNow || Wh_ApplyHookOperations();
            Wh_Log(L"JNI_CreateJavaVM hook ready=%d", g_vmHookReady);
        } else {
            Wh_Log(L"JNI_CreateJavaVM hook registration failed");
        }
    }
    const bool ready = g_vmHookReady;
    ReleaseSRWLockExclusive(&g_vmHookLock);
    return ready;
}

FARPROC WINAPI ProcAddressHook(HMODULE module, LPCSTR name) {
    const auto address = g_getProcAddress(module, name);
    if (address != nullptr && reinterpret_cast<ULONG_PTR>(name) > 0xFFFF &&
        std::strcmp(name, "JNI_CreateJavaVM") == 0 &&
        module == GetModuleHandleW(L"jvm.dll")) {
        InstallVmHook(address, true);
    }
    return address;
}

BOOL Init() {
    const auto jvm = GetModuleHandleW(L"jvm.dll");
    if (jvm != nullptr) {
        const auto getCreated = reinterpret_cast<GetCreatedVms_t>(
            GetProcAddress(jvm, "JNI_GetCreatedJavaVMs"));
        if (getCreated != nullptr) {
            void* vm = nullptr;
            int count = 0;
            if (getCreated(&vm, 1, &count) == 0 && count != 0) {
                Wh_Log(L"JVM is already running! enable the mod before starting a fresh HMCL process");
                return FALSE;
            }
        }
    }

    const auto kernel = GetModuleHandleW(L"kernel32.dll");
    const auto procAddress = GetProcAddress(kernel, "GetProcAddress");
    const auto userGeoId = GetProcAddress(kernel, "GetUserGeoID");
    if (procAddress == nullptr || userGeoId == nullptr) return FALSE;

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(procAddress),
                            reinterpret_cast<void*>(ProcAddressHook),
                            reinterpret_cast<void**>(&g_getProcAddress)) ||
        !Wh_SetFunctionHook(reinterpret_cast<void*>(userGeoId),
                            reinterpret_cast<void*>(UserGeoIdHook),
                            reinterpret_cast<void**>(&g_getUserGeoId))) return FALSE;

    if (jvm != nullptr) {
        const auto createVm = GetProcAddress(jvm, "JNI_CreateJavaVM");
        if (createVm == nullptr || !InstallVmHook(createVm, false)) return FALSE;
    }

    Wh_Log(L"HMCL startup detected, watching JNI_CreateJavaVM");
    return TRUE;
}

} // namespace Hmcl

enum class TargetKind { None, PclCe, Hmcl };

TargetKind SelectTarget(const WCHAR* executable, const WCHAR* commandLine) {
    const auto name = Hmcl::FileName(executable);
    const size_t length = std::wcslen(name);
    if (length >= 11 && _wcsnicmp(name, L"PCL2_CE", 7) == 0 &&
        _wcsicmp(name + length - 4, L".exe") == 0) return TargetKind::PclCe;
    return Hmcl::IsHmclTarget(executable, commandLine) ? TargetKind::Hmcl : TargetKind::None;
}

BOOL Wh_ModInit() {
    WCHAR executable[32768];
    const auto length = GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable));
    if (length == 0 || length == ARRAYSIZE(executable)) return FALSE;
    switch (SelectTarget(executable, GetCommandLineW())) {
        case TargetKind::PclCe:
            Wh_Log(L"Selected PCL CE region hooks");
            return PclCe::Init();
        case TargetKind::Hmcl:
            Wh_Log(L"Selected HMCL JVM region hooks");
            return Hmcl::Init();
        default:
            return FALSE;
    }
}
