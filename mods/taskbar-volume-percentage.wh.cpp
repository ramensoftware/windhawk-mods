// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Displays the exact master volume percentage in the system tray natively inside the Windows 11 volume button with real-time sync.
// @version         2.0.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Percentage Indicator

Displays the master volume percentage directly inside the Windows 11 system tray volume icon in real time.

## Display styles

- **Percentage**: Displays the volume percentage (e.g. `50%`).
- **Number only**: Displays the numerical volume level (e.g. `50`).
- **Prefix and Percentage**: Displays a custom prefix before the percentage (e.g. `Vol 50%`).
- **Icon and Percentage**: Displays a speaker icon with the percentage (e.g. `🔊 50%`).
- **Windows Default**: Displays the vanilla native Windows 11 speaker icon with volume waves.

## Credits

- Based on the [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler) mod by [m417z](https://github.com/m417z).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayStyle: percentage
  $name: Display Style
  $description: Format of the volume indicator in the taskbar.
  $options:
    - percentage: Percentage (e.g. 50%)
    - number: Number only (e.g. 50)
    - prefix: Prefix and Percentage (e.g. Vol 50%)
    - emoji: Icon and Percentage (e.g. 🔊 50%)
    - vanilla: Windows Default (exact vanilla Windows speaker icon)
- customPrefix: "Vol "
  $name: Custom Prefix
  $description: Prefix text used when the display style is set to 'Prefix and Percentage'.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>

#include <windhawk_api.h>
#include <windhawk_utils.h>

// Reference: C++/WinRT base_string.h shared_hstring_header binary layout
struct hstring_header {
    uint32_t flags;
    uint32_t length;
    uint32_t padding1;
    uint32_t padding2;
    wchar_t const* ptr;
};

struct shared_hstring_header : hstring_header {
    int32_t count;
    wchar_t buffer[1];
};

static shared_hstring_header* CreateSharedHString(const std::wstring& str) {
    uint32_t len = static_cast<uint32_t>(str.length());
    size_t bytes = sizeof(shared_hstring_header) + sizeof(wchar_t) * len;
    shared_hstring_header* header = static_cast<shared_hstring_header*>(
        HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes));
    if (!header) {
        return nullptr;
    }
    header->flags = 0;
    header->length = len;
    header->padding1 = 0;
    header->padding2 = 0;
    header->ptr = header->buffer;
    header->count = 1;
    memcpy(header->buffer, str.c_str(), sizeof(wchar_t) * (len + 1));
    return header;
}

static void ReleaseSharedHString(shared_hstring_header* header) {
    if (!header) {
        return;
    }
    // Only heap-allocated strings (flags == 0) have an allocated reference count.
    // Fast-pass / static string references (flags != 0) must never be freed.
    if (header->flags != 0) {
        return;
    }
    if (InterlockedDecrement(reinterpret_cast<volatile LONG*>(&header->count)) == 0) {
        HeapFree(GetProcessHeap(), 0, header);
    }
}

enum class DisplayStyle {
    Percentage,
    Number,
    Prefix,
    Emoji,
    Vanilla,
};

struct ModSettings {
    DisplayStyle displayStyle;
    std::wstring customPrefix;
};

static ModSettings g_settings;
static std::mutex g_dataModelMutex;
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};
static void* g_pVolumeDataModel = nullptr;
static float g_lastVolumeLevel = 0.50f;
static bool g_lastIsMuted = false;

// Windows 11 Segoe Fluent Icons volume glyphs
constexpr wchar_t GLYPH_MUTE = 0xE74F;
constexpr wchar_t GLYPH_VOL_3 = 0xE995;

static wchar_t GetVanillaVolumeGlyph(int percentage, bool isMuted) {
    if (isMuted) {
        return GLYPH_MUTE;
    }
    if (percentage <= 0) {
        return 0xE992;
    }
    if (percentage <= 33) {
        return 0xE993;
    }
    if (percentage <= 66) {
        return 0xE994;
    }
    return GLYPH_VOL_3;
}

static std::wstring FormatVolumeText(int percentage, bool isMuted) {
    if (isMuted) {
        switch (g_settings.displayStyle) {
            case DisplayStyle::Vanilla:
                return std::wstring(1, GLYPH_MUTE);
            case DisplayStyle::Emoji:
                return L"\xD83D\xDD07 MUT";
            case DisplayStyle::Prefix:
                return g_settings.customPrefix + L"MUT";
            case DisplayStyle::Number:
            case DisplayStyle::Percentage:
            default:
                return L"MUT";
        }
    }

    switch (g_settings.displayStyle) {
        case DisplayStyle::Vanilla:
            return std::wstring(1, GetVanillaVolumeGlyph(percentage, false));

        case DisplayStyle::Number:
            return std::to_wstring(percentage);

        case DisplayStyle::Prefix:
            return g_settings.customPrefix + std::to_wstring(percentage) + L"%";

        case DisplayStyle::Emoji: {
            std::wstring s;
            if (percentage <= 0) {
                s = L"\xD83D\xDD08 ";
            } else if (percentage <= 50) {
                s = L"\xD83D\xDD09 ";
            } else {
                s = L"\xD83D\xDD0A ";
            }
            s += std::to_wstring(percentage) + L"%";
            return s;
        }

        case DisplayStyle::Percentage:
        default:
            return std::to_wstring(percentage) + L"%";
    }
}

static void ApplyCustomVolumeText(void* pThis, float volumeLevel, bool isMuted) {
    if (!pThis || g_unloading) {
        return;
    }

    int percentage = static_cast<int>(std::round(volumeLevel * 100.0f));
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    std::wstring customText = FormatVolumeText(percentage, isMuted);

    // Offset 0x90 in VolumeSystemTrayIconDataModel points to the icon text shared_hstring_header.
    shared_hstring_header** ppHeader = reinterpret_cast<shared_hstring_header**>(
        reinterpret_cast<char*>(pThis) + 0x90);

    shared_hstring_header* newHeader = CreateSharedHString(customText);
    if (!newHeader) {
        return;
    }

    shared_hstring_header* oldHeader = *ppHeader;
    *ppHeader = newHeader;

    if (oldHeader && oldHeader != newHeader) {
        ReleaseSharedHString(oldHeader);
    }
}

static void TriggerImmediateVolumeSync() {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (SUCCEEDED(hr) && pEnumerator) {
        IMMDevice* pDevice = nullptr;
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
        if (SUCCEEDED(hr) && pDevice) {
            IAudioEndpointVolume* pEndpointVolume = nullptr;
            hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&pEndpointVolume);
            if (SUCCEEDED(hr) && pEndpointVolume) {
                float vol = 0.0f;
                if (SUCCEEDED(pEndpointVolume->GetMasterVolumeLevelScalar(&vol))) {
                    // Temporarily nudge volume to force an audio state change notification
                    float nudge = (vol > 0.01f) ? (vol - 0.0005f) : (vol + 0.0005f);
                    pEndpointVolume->SetMasterVolumeLevelScalar(nudge, nullptr);
                    pEndpointVolume->SetMasterVolumeLevelScalar(vol, nullptr);
                    Wh_Log(L"Triggered immediate volume sync (vol=%.4f)", vol);
                }
                pEndpointVolume->Release();
            }
            pDevice->Release();
        }
        pEnumerator->Release();
    }

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
}

// Hooks

using VolumeSystemTrayIconDataModel_ctor_t = void*(WINAPI*)(void* pThis);
static VolumeSystemTrayIconDataModel_ctor_t VolumeSystemTrayIconDataModel_ctor_Original = nullptr;

static void* WINAPI VolumeSystemTrayIconDataModel_ctor_Hook(void* pThis) {
    void* result = VolumeSystemTrayIconDataModel_ctor_Original(pThis);
    if (!g_unloading) {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = pThis;
        ApplyCustomVolumeText(pThis, g_lastVolumeLevel, g_lastIsMuted);
    }
    return result;
}

using VolumeSystemTrayIconDataModel_UpdateVolume_t = void(WINAPI*)(
    void* pThis,
    float volumeLevel,
    bool isMuted,
    void* deviceName
);
static VolumeSystemTrayIconDataModel_UpdateVolume_t VolumeSystemTrayIconDataModel_UpdateVolume_Original = nullptr;

using VolumeSystemTrayIconDataModel_OnDataModelChanged_t = void(WINAPI*)(
    void* pThis,
    const void* propertyName
);
static VolumeSystemTrayIconDataModel_OnDataModelChanged_t VolumeSystemTrayIconDataModel_OnDataModelChanged_Original = nullptr;

static void WINAPI VolumeSystemTrayIconDataModel_UpdateVolume_Hook(
    void* pThis,
    float volumeLevel,
    bool isMuted,
    void* deviceName)
{
    VolumeSystemTrayIconDataModel_UpdateVolume_Original(pThis, volumeLevel, isMuted, deviceName);

    if (g_unloading) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = pThis;
        g_lastVolumeLevel = volumeLevel;
        g_lastIsMuted = isMuted;

        ApplyCustomVolumeText(pThis, volumeLevel, isMuted);
    }

    // Trigger UI redraw via CurrentData property notification
    if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
        std::wstring_view propName = L"CurrentData";
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, &propName);
    }
}

// Symbol resolution & module detection

static bool HookSystemTraySymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume(float,bool,struct winrt::hstring))",
                LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume(float,bool,struct winrt::hstring) __ptr64)",
                LR"(?UpdateVolume@VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@QEAAXM_NUhstring@4@@Z)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume)",
            },
            &VolumeSystemTrayIconDataModel_UpdateVolume_Original,
            VolumeSystemTrayIconDataModel_UpdateVolume_Hook,
            false,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged(class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > const &))",
                LR"(private: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged(class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > const & __ptr64) __ptr64)",
                LR"(?OnDataModelChanged@VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@AEAAXAEBV?$basic_string_view@_WU?$char_traits@_W@std@@@std@@@Z)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged)",
            },
            &VolumeSystemTrayIconDataModel_OnDataModelChanged_Original,
            nullptr, // Resolved for invocation only, not hooked
            true,
        },
        {
            {
                LR"(public: __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::VolumeSystemTrayIconDataModel(struct winrt::SystemTray::IIconDataModel const &))",
                LR"(public: __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::VolumeSystemTrayIconDataModel(struct winrt::SystemTray::IIconDataModel const & __ptr64) __ptr64)",
                LR"(?0VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@QEAA@AEBUIIconDataModel@23@@Z)",
                LR"(public: __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::VolumeSystemTrayIconDataModel(void))",
                LR"(public: __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::VolumeSystemTrayIconDataModel(void) __ptr64)",
                LR"(??0VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@QEAA@XZ)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::VolumeSystemTrayIconDataModel)",
            },
            &VolumeSystemTrayIconDataModel_ctor_Original,
            VolumeSystemTrayIconDataModel_ctor_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks))) {
        Wh_Log(L"Failed to hook SystemTray volume symbols");
        return false;
    }

    Wh_Log(L"Successfully hooked SystemTray volume symbols");
    return true;
}

static HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
    }
    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                         HANDLE hFile,
                                         DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!hModule || ((ULONG_PTR)hModule & 3)) {
        return hModule;
    }

    if (!g_systemTrayModuleHooked && lpLibFileName) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"SystemTray.dll") == 0 ||
            _wcsicmp(fileName, L"Taskbar.View.dll") == 0) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"SystemTray module loaded dynamically: %s", lpLibFileName);
                if (HookSystemTraySymbols(hModule)) {
                    Wh_ApplyHookOperations();
                    TriggerImmediateVolumeSync();
                } else {
                    g_systemTrayModuleHooked = false;
                }
            }
        }
    }

    return hModule;
}

// Settings & lifecycle

static void LoadSettings() {
    PCWSTR displayStyleStr = Wh_GetStringSetting(L"displayStyle");
    if (displayStyleStr) {
        if (wcscmp(displayStyleStr, L"vanilla") == 0 ||
            wcscmp(displayStyleStr, L"default") == 0 ||
            wcscmp(displayStyleStr, L"vanillaOnly") == 0) {
            g_settings.displayStyle = DisplayStyle::Vanilla;
        } else if (wcscmp(displayStyleStr, L"number") == 0 ||
                   wcscmp(displayStyleStr, L"numberOnly") == 0) {
            g_settings.displayStyle = DisplayStyle::Number;
        } else if (wcscmp(displayStyleStr, L"prefix") == 0 ||
                   wcscmp(displayStyleStr, L"volPrefix") == 0) {
            g_settings.displayStyle = DisplayStyle::Prefix;
        } else if (wcscmp(displayStyleStr, L"emoji") == 0 ||
                   wcscmp(displayStyleStr, L"emojiAndPercent") == 0 ||
                   wcscmp(displayStyleStr, L"iconAndPercent") == 0) {
            g_settings.displayStyle = DisplayStyle::Emoji;
        } else {
            g_settings.displayStyle = DisplayStyle::Percentage;
        }
        Wh_FreeStringSetting(displayStyleStr);
    } else {
        g_settings.displayStyle = DisplayStyle::Percentage;
    }

    PCWSTR prefixStr = Wh_GetStringSetting(L"customPrefix");
    if (prefixStr) {
        g_settings.customPrefix = prefixStr;
        Wh_FreeStringSetting(prefixStr);
    } else {
        g_settings.customPrefix = L"Vol ";
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Volume Percentage Indicator mod in explorer.exe");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            g_systemTrayModuleHooked = false;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet, hooking LoadLibraryExW");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        if (pKernelBaseLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        } else {
            WindhawkUtils::SetFunctionHook(LoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }

    Wh_Log(L"Taskbar Volume Percentage Indicator mod initialized successfully");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"System tray module found in Wh_ModAfterInit");
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_systemTrayModuleHooked = false;
                }
            }
        }
    }

    TriggerImmediateVolumeSync();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Preparing mod unload, restoring native icon");

    g_unloading = true;

    // Restore original native volume glyph before unloading
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel && ((ULONG_PTR)g_pVolumeDataModel & (sizeof(void*) - 1)) == 0) {
            wchar_t defaultGlyph = g_lastIsMuted ? GLYPH_MUTE : GLYPH_VOL_3;
            std::wstring restoreStr(1, defaultGlyph);

            shared_hstring_header** ppHeader = reinterpret_cast<shared_hstring_header**>(
                reinterpret_cast<char*>(g_pVolumeDataModel) + 0x90);

            shared_hstring_header* oldHeader = *ppHeader;
            shared_hstring_header* newHeader = CreateSharedHString(restoreStr);
            if (newHeader) {
                *ppHeader = newHeader;
                if (oldHeader && oldHeader != newHeader) {
                    ReleaseSharedHString(oldHeader);
                }
            }

            if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
                std::wstring_view propName = L"CurrentData";
                VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(g_pVolumeDataModel, &propName);
            }
        }
        g_pVolumeDataModel = nullptr;
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Volume Percentage Indicator mod");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();

    if (g_unloading) {
        return;
    }

    bool hasDataModel = false;
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel) {
            hasDataModel = true;
            ApplyCustomVolumeText(g_pVolumeDataModel, g_lastVolumeLevel, g_lastIsMuted);
            if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
                std::wstring_view propName = L"CurrentData";
                VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(g_pVolumeDataModel, &propName);
            }
        }
    }

    if (!hasDataModel) {
        TriggerImmediateVolumeSync();
    }
}
