// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Displays the exact master volume percentage in the system tray natively inside the Windows 11 volume button with real-time sync.
// @version         1.1.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lversion
// @license         GPL-3.0
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

## Jitter Prevention (Padding)

Stabilizes the button width and prevents adjacent tray icons (Wi-Fi, Battery, Clock) from shifting when the volume level changes between 1, 2, or 3 digits:
- **None**: Standard variable width (e.g. `5%`, `50%`, `100%`).
- **Zero-padded**: Fixed 2-digit format (e.g. `05%`, `50%`, `100%`).
- **Space-padded**: Balanced space format (e.g. ` 5%`, `50%`, `100%`).

## Mute Display Styles

Choose how the muted state is represented:
- **MUT**: Standard abbreviation (`MUT`).
- **Mute**: Full word (`Mute`).
- **0%**: Zero volume representation (`0%` / `00%`).
- **✕**: Modern fine cross symbol (`✕`).
- **🔇**: Mute speaker emoji (`🔇`).
- **Native Glyph**: Windows 11 Segoe Fluent crossed-out speaker glyph.
- **Custom Text**: Custom user-defined string.

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
- paddingMode: none
  $name: Jitter Prevention (Padding)
  $description: Stabilizes taskbar button width and prevents adjacent tray icons from shifting when volume changes between 1 and 2 digits.
  $options:
    - none: None (Standard width, e.g. 5%, 50%, 100%)
    - zero: Zero-padded (Fixed 2 digits, e.g. 05%, 50%, 100%)
    - space: Space-padded (Balanced space, e.g. ' 5%', '50%', '100%')
- muteStyle: mut
  $name: Mute Display Style
  $description: Format shown when system master volume is muted.
  $options:
    - mut: MUT (Standard abbreviation)
    - mute: Mute (Full word)
    - zero: 0% (Zero volume display)
    - cross: "✕ (Cross symbol)"
    - emoji: "🔇 (Mute emoji)"
    - glyph: Native Glyph (Segoe Fluent crossed-out speaker icon)
    - custom: Custom Text (Uses text specified below)
- customMuteText: "Mute"
  $name: Custom Mute Text
  $description: Custom string displayed when Mute Display Style is set to 'Custom Text'.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <winver.h>
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

enum class PaddingMode {
    None,
    Zero,
    Space,
};

enum class MuteStyle {
    Mut,
    Mute,
    Zero,
    Cross,
    Emoji,
    Glyph,
    Custom,
};

struct ModSettings {
    DisplayStyle displayStyle;
    std::wstring customPrefix;
    PaddingMode paddingMode;
    MuteStyle muteStyle;
    std::wstring customMuteText;
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
        if (g_settings.displayStyle == DisplayStyle::Vanilla) {
            return std::wstring(1, GLYPH_MUTE);
        }

        std::wstring muteText;
        switch (g_settings.muteStyle) {
            case MuteStyle::Mute:
                muteText = L"Mute";
                break;
            case MuteStyle::Zero:
                if (g_settings.displayStyle == DisplayStyle::Number) {
                    muteText = (g_settings.paddingMode == PaddingMode::Zero) ? L"00" :
                               ((g_settings.paddingMode == PaddingMode::Space) ? L" 0" : L"0");
                } else {
                    muteText = (g_settings.paddingMode == PaddingMode::Zero) ? L"00%" :
                               ((g_settings.paddingMode == PaddingMode::Space) ? L" 0%" : L"0%");
                }
                break;
            case MuteStyle::Cross:
                muteText = L"\x2715"; // ✕
                break;
            case MuteStyle::Emoji:
                muteText = L"\xD83D\xDD07"; // 🔇
                break;
            case MuteStyle::Glyph:
                muteText = std::wstring(1, GLYPH_MUTE);
                break;
            case MuteStyle::Custom:
                muteText = g_settings.customMuteText;
                break;
            case MuteStyle::Mut:
            default:
                muteText = L"MUT";
                break;
        }

        if (g_settings.displayStyle == DisplayStyle::Prefix) {
            return g_settings.customPrefix + muteText;
        }
        if (g_settings.displayStyle == DisplayStyle::Emoji && g_settings.muteStyle != MuteStyle::Emoji) {
            return L"\xD83D\xDD07 " + muteText;
        }
        return muteText;
    }

    if (g_settings.displayStyle == DisplayStyle::Vanilla) {
        return std::wstring(1, GetVanillaVolumeGlyph(percentage, false));
    }

    std::wstring numStr = std::to_wstring(percentage);
    if (percentage < 10) {
        if (g_settings.paddingMode == PaddingMode::Zero) {
            numStr = L"0" + numStr;
        } else if (g_settings.paddingMode == PaddingMode::Space) {
            numStr = L" " + numStr;
        }
    }

    switch (g_settings.displayStyle) {
        case DisplayStyle::Number:
            return numStr;

        case DisplayStyle::Prefix:
            return g_settings.customPrefix + numStr + L"%";

        case DisplayStyle::Emoji: {
            std::wstring s;
            if (percentage <= 0) {
                s = L"\xD83D\xDD08 ";
            } else if (percentage <= 50) {
                s = L"\xD83D\xDD09 ";
            } else {
                s = L"\xD83D\xDD0A ";
            }
            s += numStr + L"%";
            return s;
        }

        case DisplayStyle::Percentage:
        default:
            return numStr + L"%";
    }
}

static size_t GetIconTextOffset(void* pThis) {
    if (!pThis || (reinterpret_cast<uintptr_t>(pThis) % sizeof(void*)) != 0) {
        return 0;
    }

    auto isValidHeader = [](shared_hstring_header* h) -> bool {
        if (!h || (reinterpret_cast<uintptr_t>(h) % sizeof(void*)) != 0) {
            return false;
        }
        if (h->flags > 1) {
            return false;
        }
        if (h->length == 0 || h->length > 64) {
            return false;
        }
        if (h->ptr != h->buffer) {
            return false;
        }
        return true;
    };

    const size_t defaultOffset = sizeof(void*) == 8 ? 0x90 : 0x48;
    shared_hstring_header* defaultCandidate = *reinterpret_cast<shared_hstring_header**>(
        reinterpret_cast<char*>(pThis) + defaultOffset);
    if (isValidHeader(defaultCandidate)) {
        return defaultOffset;
    }

    for (size_t offset = 0x40; offset <= 0x120; offset += sizeof(void*)) {
        shared_hstring_header* candidate = *reinterpret_cast<shared_hstring_header**>(
            reinterpret_cast<char*>(pThis) + offset);
        if (isValidHeader(candidate)) {
            wchar_t firstChar = candidate->buffer[0];
            if ((firstChar >= 0xE700 && firstChar <= 0xE9FF) ||
                (firstChar >= L'0' && firstChar <= L'9') ||
                (firstChar >= L'a' && firstChar <= L'z') ||
                (firstChar >= L'A' && firstChar <= L'Z') ||
                firstChar == L' ' || firstChar == 0x2715 ||
                firstChar == 0xD83D) {
                return offset;
            }
        }
    }

    return 0;
}

static void ApplyCustomVolumeText(void* pThis, float volumeLevel, bool isMuted) {
    if (!pThis || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    size_t offset = GetIconTextOffset(pThis);
    if (offset == 0) {
        Wh_Log(L"Could not safely locate icon text member in VolumeSystemTrayIconDataModel");
        return;
    }

    int percentage = static_cast<int>(std::round(volumeLevel * 100.0f));
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    std::wstring customText = FormatVolumeText(percentage, isMuted);

    shared_hstring_header** ppHeader = reinterpret_cast<shared_hstring_header**>(
        reinterpret_cast<char*>(pThis) + offset);

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

static void QueryInitialSystemVolume(float* pVolumeLevel, bool* pIsMuted) {
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
                    *pVolumeLevel = vol;
                }
                BOOL mute = FALSE;
                if (SUCCEEDED(pEndpointVolume->GetMute(&mute))) {
                    *pIsMuted = (mute != FALSE);
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

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (!hResource) {
        return nullptr;
    }
    HGLOBAL hGlobal = LoadResource(hModule, hResource);
    if (!hGlobal) {
        return nullptr;
    }
    void* pData = LockResource(hGlobal);
    if (!pData) {
        return nullptr;
    }
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
        return nullptr;
    }
    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }
    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

// Window thread marshaling to ensure 100% thread affinity with Explorer's taskbar UI
static BOOL CALLBACK EnumWindowsTaskbarProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    WCHAR className[64];
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumWindowsTaskbarProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void (*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static UINT g_runFromWindowThreadRegisteredMsg = 0;

static LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    (void)wParam;
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == g_runFromWindowThreadRegisteredMsg) {
            auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            param->proc(param->procParam);
        }
    }
    return CallNextHookEx(nullptr, nCode, 0, lParam);
}

static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    if (g_runFromWindowThreadRegisteredMsg == 0) {
        g_runFromWindowThreadRegisteredMsg =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    }

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        CallWndProcHook,
        nullptr,
        dwThreadId);

    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param = {proc, procParam};
    SendMessage(hWnd, g_runFromWindowThreadRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// Hooks

using VolumeSystemTrayIconDataModel_dtor_t = void(WINAPI*)(void* pThis);
static VolumeSystemTrayIconDataModel_dtor_t VolumeSystemTrayIconDataModel_dtor_Original = nullptr;

static void WINAPI VolumeSystemTrayIconDataModel_dtor_Hook(void* pThis) {
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel == pThis) {
            g_pVolumeDataModel = nullptr;
        }
    }
    VolumeSystemTrayIconDataModel_dtor_Original(pThis);
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
    static thread_local bool s_inHook = false;
    if (s_inHook) {
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(pThis, volumeLevel, isMuted, deviceName);
        return;
    }

    VolumeSystemTrayIconDataModel_UpdateVolume_Original(pThis, volumeLevel, isMuted, deviceName);

    if (g_unloading.load(std::memory_order_relaxed)) {
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
        s_inHook = true;
        std::wstring_view propName = L"CurrentData";
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, &propName);
        s_inHook = false;
    }
}

static bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
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
            false,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel(void))",
                LR"(public: virtual __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel(void) __ptr64)",
                LR"(??1VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UEAA@XZ)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel)",
            },
            &VolumeSystemTrayIconDataModel_dtor_Original,
            VolumeSystemTrayIconDataModel_dtor_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
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
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor = fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %u (SystemTray types moved to SystemTray.dll)", moduleMajor);
                module = nullptr;
            }
        }
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
        bool isCandidate = false;

        if (_wcsicmp(fileName, L"SystemTray.dll") == 0) {
            isCandidate = true;
        } else if (_wcsicmp(fileName, L"Taskbar.View.dll") == 0) {
            VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(hModule, nullptr);
            WORD moduleMajor = fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (moduleMajor && moduleMajor < 2604) {
                isCandidate = true;
            } else {
                Wh_Log(L"Ignoring dynamically loaded Taskbar.View.dll (version %u >= 2604)", moduleMajor);
            }
        }

        if (isCandidate) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"SystemTray module loaded dynamically: %s", lpLibFileName);
                if (HookSystemTraySymbols(hModule)) {
                    Wh_ApplyHookOperations();
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

    PCWSTR paddingModeStr = Wh_GetStringSetting(L"paddingMode");
    if (paddingModeStr) {
        if (wcscmp(paddingModeStr, L"zero") == 0) {
            g_settings.paddingMode = PaddingMode::Zero;
        } else if (wcscmp(paddingModeStr, L"space") == 0) {
            g_settings.paddingMode = PaddingMode::Space;
        } else {
            g_settings.paddingMode = PaddingMode::None;
        }
        Wh_FreeStringSetting(paddingModeStr);
    } else {
        g_settings.paddingMode = PaddingMode::None;
    }

    PCWSTR muteStyleStr = Wh_GetStringSetting(L"muteStyle");
    if (muteStyleStr) {
        if (wcscmp(muteStyleStr, L"mute") == 0) {
            g_settings.muteStyle = MuteStyle::Mute;
        } else if (wcscmp(muteStyleStr, L"zero") == 0) {
            g_settings.muteStyle = MuteStyle::Zero;
        } else if (wcscmp(muteStyleStr, L"cross") == 0) {
            g_settings.muteStyle = MuteStyle::Cross;
        } else if (wcscmp(muteStyleStr, L"emoji") == 0) {
            g_settings.muteStyle = MuteStyle::Emoji;
        } else if (wcscmp(muteStyleStr, L"glyph") == 0) {
            g_settings.muteStyle = MuteStyle::Glyph;
        } else if (wcscmp(muteStyleStr, L"custom") == 0) {
            g_settings.muteStyle = MuteStyle::Custom;
        } else {
            g_settings.muteStyle = MuteStyle::Mut;
        }
        Wh_FreeStringSetting(muteStyleStr);
    } else {
        g_settings.muteStyle = MuteStyle::Mut;
    }

    PCWSTR customMuteStr = Wh_GetStringSetting(L"customMuteText");
    if (customMuteStr) {
        g_settings.customMuteText = customMuteStr;
        Wh_FreeStringSetting(customMuteStr);
    } else {
        g_settings.customMuteText = L"Mute";
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Volume Percentage Indicator mod in explorer.exe");

    LoadSettings();
    QueryInitialSystemVolume(&g_lastVolumeLevel, &g_lastIsMuted);

    bool hooked = false;
    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        if (HookSystemTraySymbols(systemTrayModule)) {
            g_systemTrayModuleHooked = true;
            hooked = true;
        }
    }

    if (!hooked) {
        Wh_Log(L"System tray symbols not hooked yet, hooking LoadLibraryExW");
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
}

static void BeforeUninitOnUIThread(void* /*parameter*/) {
    std::lock_guard<std::mutex> lock(g_dataModelMutex);
    if (g_pVolumeDataModel && (reinterpret_cast<uintptr_t>(g_pVolumeDataModel) % sizeof(void*)) == 0) {
        size_t offset = GetIconTextOffset(g_pVolumeDataModel);
        if (offset != 0) {
            wchar_t defaultGlyph = g_lastIsMuted ? GLYPH_MUTE : GLYPH_VOL_3;
            std::wstring restoreStr(1, defaultGlyph);

            shared_hstring_header** ppHeader = reinterpret_cast<shared_hstring_header**>(
                reinterpret_cast<char*>(g_pVolumeDataModel) + offset);

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
    }
    g_pVolumeDataModel = nullptr;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Preparing mod unload, restoring native icon");

    g_unloading = true;

    // Restore original native volume glyph safely on the UI thread before unloading
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(hTaskbarWnd, BeforeUninitOnUIThread, nullptr);
    } else {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = nullptr;
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Volume Percentage Indicator mod");
}

static void SettingsChangedOnUIThread(void* /*parameter*/) {
    std::lock_guard<std::mutex> lock(g_dataModelMutex);
    if (g_pVolumeDataModel && !g_unloading.load(std::memory_order_relaxed)) {
        ApplyCustomVolumeText(g_pVolumeDataModel, g_lastVolumeLevel, g_lastIsMuted);
        if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
            std::wstring_view propName = L"CurrentData";
            VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(g_pVolumeDataModel, &propName);
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();

    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(hTaskbarWnd, SettingsChangedOnUIThread, nullptr);
    }
}
