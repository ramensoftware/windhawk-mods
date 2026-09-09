// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Displays the exact master volume percentage in the system tray natively inside the Windows 11 volume button with real-time sync.
// @version         1.2.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @compilerOptions -lole32 -lshlwapi -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Percentage Indicator

Replaces the Windows 11 taskbar volume icon with the current volume level in real time.

## Features

- **Display styles**: Percentage (`50%`), number only (`50`), custom prefix (`Vol 50%`), emoji (`🔊 50%`), or default icon.
- **Mute indicator**: Customizable mute display (`MUT`, `Mute`, `0%`, `✕`, `🔇`, or native glyph).

## Credits

- Inspired by the taskbar visual customization concepts from [m417z](https://github.com/m417z).
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

constexpr uint32_t MOD_HSTRING_MAGIC = 0x57484F4B; // "WHOK" tag to verify ownership before HeapFree

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
    header->padding1 = MOD_HSTRING_MAGIC;
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
    if (header->flags != 0 || header->padding1 != MOD_HSTRING_MAGIC) {
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

enum class MuteStyle {
    Mut,
    Mute,
    Zero,
    Cross,
    Emoji,
    Glyph,
    Custom,
};

struct Settings {
    DisplayStyle displayStyle;
    std::wstring customPrefix;
    MuteStyle muteStyle;
    std::wstring customMuteText;
};

static Settings g_settings;

// Segoe Fluent Icons volume glyphs
constexpr wchar_t GLYPH_MUTE  = 0xE74F; // Crossed-out speaker
constexpr wchar_t GLYPH_VOL_0 = 0xE992; // 0 bars
constexpr wchar_t GLYPH_VOL_1 = 0xE993; // 1 bar
constexpr wchar_t GLYPH_VOL_2 = 0xE994; // 2 bars
constexpr wchar_t GLYPH_VOL_3 = 0xE995; // 3 bars

static void* g_pVolumeDataModel = nullptr;
static float g_lastVolumeLevel = 0.5f;
static bool  g_lastIsMuted = false;
static std::mutex g_dataModelMutex;
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};

static std::wstring FormatMuteString(MuteStyle style, const std::wstring& customText) {
    switch (style) {
        case MuteStyle::Mute:
            return L"Mute";
        case MuteStyle::Zero:
            return L"0%";
        case MuteStyle::Cross:
            return L"\u2715";
        case MuteStyle::Emoji:
            return L"\xD83D\xDD07";
        case MuteStyle::Glyph:
            return std::wstring(1, GLYPH_MUTE);
        case MuteStyle::Custom:
            return customText.empty() ? L"Mute" : customText;
        case MuteStyle::Mut:
        default:
            return L"MUT";
    }
}

static std::wstring FormatVolumeText(int percentage, bool isMuted) {
    if (isMuted && g_settings.displayStyle != DisplayStyle::Vanilla) {
        return FormatMuteString(g_settings.muteStyle, g_settings.customMuteText);
    }

    std::wstring s = std::to_wstring(percentage);

    switch (g_settings.displayStyle) {
        case DisplayStyle::Vanilla: {
            wchar_t glyph = GLYPH_MUTE;
            if (!isMuted) {
                if (percentage == 0)      glyph = GLYPH_VOL_0;
                else if (percentage < 33) glyph = GLYPH_VOL_1;
                else if (percentage < 66) glyph = GLYPH_VOL_2;
                else                      glyph = GLYPH_VOL_3;
            }
            return std::wstring(1, glyph);
        }

        case DisplayStyle::Number:
            return s;

        case DisplayStyle::Prefix:
            return g_settings.customPrefix + s + L"%";

        case DisplayStyle::Emoji: {
            std::wstring prefix;
            if (percentage == 0) {
                prefix = L"\xD83D\xDD08 ";
            } else if (percentage <= 50) {
                prefix = L"\xD83D\xDD09 ";
            } else {
                prefix = L"\xD83D\xDD0A ";
            }
            return prefix + s + L"%";
        }

        case DisplayStyle::Percentage:
        default:
            return s + L"%";
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
        if (h->length > 256) {
            return false;
        }
        if (!h->ptr) {
            return false;
        }
        return true;
    };

    // Primary offset in standard Windows 11 SystemTray builds is 0xB8
    constexpr size_t defaultOffset = 0xB8;
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
                firstChar == 0x2007 || firstChar == 0x00A0 ||
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

static void QueryInitialSystemVolume(float* pLevel, bool* pMuted) {
    *pLevel = 0.5f;
    *pMuted = false;

    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&pEnumerator));
    if (FAILED(hr) || !pEnumerator) {
        return;
    }

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (SUCCEEDED(hr) && pDevice) {
        IAudioEndpointVolume* pEndpoint = nullptr;
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER,
                               nullptr, reinterpret_cast<void**>(&pEndpoint));
        if (SUCCEEDED(hr) && pEndpoint) {
            float masterVol = 0.5f;
            BOOL bMute = FALSE;
            if (SUCCEEDED(pEndpoint->GetMasterVolumeLevelScalar(&masterVol))) {
                *pLevel = masterVol;
            }
            if (SUCCEEDED(pEndpoint->GetMute(&bMute))) {
                *pMuted = (bMute != FALSE);
            }
            pEndpoint->Release();
        }
        pDevice->Release();
    }
    pEnumerator->Release();
}

static HWND FindCurrentProcessTaskbarWnd() {
    DWORD currentProcessId = GetCurrentProcessId();
    HWND hTaskbarWnd = nullptr;

    while ((hTaskbarWnd = FindWindowEx(nullptr, hTaskbarWnd, L"Shell_TrayWnd", nullptr)) != nullptr) {
        DWORD processId = 0;
        GetWindowThreadProcessId(hTaskbarWnd, &processId);
        if (processId == currentProcessId) {
            return hTaskbarWnd;
        }
    }

    return nullptr;
}

using RunFromWindowThreadProc_t = void (*)(void* param);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static UINT g_runFromWindowThreadMsg = 0;

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_runFromWindowThreadMsg != 0) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == g_runFromWindowThreadMsg) {
            auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            if (param && param->proc) {
                param->proc(param->procParam);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    if (g_runFromWindowThreadMsg == 0) {
        g_runFromWindowThreadMsg = RegisterWindowMessage(
            L"Windhawk_RunFromWindowThread_taskbar-volume-percentage");
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
        RunFromWindowThreadHookProc,
        nullptr,
        dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, g_runFromWindowThreadMsg, 0, reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return reinterpret_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

// ---------------------------------------------------------------------------
// VolumeSystemTrayIconDataModel hooks
// ---------------------------------------------------------------------------

using VolumeSystemTrayIconDataModel_UpdateVolume_t = void(__cdecl*)(
    void* pThis, float volumeLevel, bool isMuted, void* hstringIcon);
static VolumeSystemTrayIconDataModel_UpdateVolume_t
    VolumeSystemTrayIconDataModel_UpdateVolume_Original = nullptr;

using VolumeSystemTrayIconDataModel_OnDataModelChanged_t = void(__cdecl*)(
    void* pThis, const std::wstring_view* propertyName);
static VolumeSystemTrayIconDataModel_OnDataModelChanged_t
    VolumeSystemTrayIconDataModel_OnDataModelChanged_Original = nullptr;

using VolumeSystemTrayIconDataModel_dtor_t = void(__cdecl*)(void* pThis);
static VolumeSystemTrayIconDataModel_dtor_t
    VolumeSystemTrayIconDataModel_dtor_Original = nullptr;

static void __cdecl VolumeSystemTrayIconDataModel_dtor_Hook(void* pThis) {
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel == pThis) {
            g_pVolumeDataModel = nullptr;
        }
    }
    if (VolumeSystemTrayIconDataModel_dtor_Original) {
        VolumeSystemTrayIconDataModel_dtor_Original(pThis);
    }
}

static void __cdecl VolumeSystemTrayIconDataModel_UpdateVolume_Hook(
    void* pThis, float volumeLevel, bool isMuted, void* hstringIcon) {
    static thread_local bool s_inHook = false;

    if (VolumeSystemTrayIconDataModel_UpdateVolume_Original) {
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(
            pThis, volumeLevel, isMuted, hstringIcon);
    }

    if (s_inHook || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = pThis;
        g_lastVolumeLevel = volumeLevel;
        g_lastIsMuted = isMuted;

        ApplyCustomVolumeText(pThis, volumeLevel, isMuted);
    }

    // Trigger UI redraw via CurrentData property notification with recursion guard
    if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
        s_inHook = true;
        std::wstring_view propName = L"CurrentData";
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, &propName);
        s_inHook = false;
    }
}

static bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume(float,bool,struct winrt::hstring))",
                LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume(float,bool,struct winrt::hstring) __ptr64)",
                LR"(?UpdateVolume@VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@QEAAXM_NUhstring@4@@Z)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume)",
            },
            reinterpret_cast<void**>(&VolumeSystemTrayIconDataModel_UpdateVolume_Original),
            reinterpret_cast<void*>(VolumeSystemTrayIconDataModel_UpdateVolume_Hook),
            false,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged(class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > const &))",
                LR"(private: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged(class std::basic_string_view<wchar_t,struct std::char_traits<wchar_t> > const & __ptr64) __ptr64)",
                LR"(?OnDataModelChanged@VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@AEAAXAEBV?$basic_string_view@_WU?$char_traits@_W@std@@@std@@@Z)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnDataModelChanged)",
            },
            reinterpret_cast<void**>(&VolumeSystemTrayIconDataModel_OnDataModelChanged_Original),
            nullptr, // Resolved for invocation only, not hooked
            true,
        },
        {
            {
                LR"(public: virtual __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel(void))",
                LR"(public: virtual __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel(void) __ptr64)",
                LR"(??1VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UEAA@XZ)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel)",
            },
            reinterpret_cast<void**>(&VolumeSystemTrayIconDataModel_dtor_Original),
            reinterpret_cast<void*>(VolumeSystemTrayIconDataModel_dtor_Hook),
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, systemTrayHooks, ARRAYSIZE(systemTrayHooks))) {
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

// ---------------------------------------------------------------------------
// Settings & lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
    auto displayStyleSetting = WindhawkUtils::StringSetting::make(L"displayStyle");
    PCWSTR displayStyleStr = displayStyleSetting.get();
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
    } else {
        g_settings.displayStyle = DisplayStyle::Percentage;
    }

    auto prefixSetting = WindhawkUtils::StringSetting::make(L"customPrefix");
    if (prefixSetting.get()) {
        g_settings.customPrefix = prefixSetting.get();
    } else {
        g_settings.customPrefix = L"Vol ";
    }

    auto muteStyleSetting = WindhawkUtils::StringSetting::make(L"muteStyle");
    PCWSTR muteStyleStr = muteStyleSetting.get();
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
    } else {
        g_settings.muteStyle = MuteStyle::Mut;
    }

    auto customMuteSetting = WindhawkUtils::StringSetting::make(L"customMuteText");
    if (customMuteSetting.get()) {
        g_settings.customMuteText = customMuteSetting.get();
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
