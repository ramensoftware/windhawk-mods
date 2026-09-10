// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Displays the exact master volume percentage in the system tray natively inside the Windows 11 volume button with real-time sync.
// @version         1.3.0
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
- **Zero-Jitter Structural Stabilization**: Locks the XAML container minimum width (`IFrameworkElement::put_MinWidth`) to prevent neighboring system tray icons from shifting during volume changes.

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
- padStyle: none
  $name: Number Padding
  $description: Format padding to minimize icon movement during volume changes.
  $options:
    - none: No Padding (e.g. 50%)
    - space: Space Padded (e.g. " 50%")
    - zero: Zero Padded (e.g. 050%)
- fixedContainerWidth: 0
  $name: Fixed Container Width
  $description: Minimum width in pixels for the volume container to eliminate layout jitter. Set to 0 for automatic optimal width based on style (e.g. 42 for percentage), or enter a custom width (e.g. 42). Set to -1 to disable.
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

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
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

enum class PadStyle {
    None,
    Space,
    Zero,
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
    PadStyle padStyle;
    int fixedContainerWidth;
    std::wstring customPrefix;
    MuteStyle muteStyle;
    std::wstring customMuteText;
};

static Settings g_settings;
static std::mutex g_settingsMutex;

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

static void* g_pVolumeIconView = nullptr;
static std::mutex g_iconViewMutex;

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};

// ---------------------------------------------------------------------------
// WinRT ABI definitions for XAML FrameworkElement and SystemTray IconView
// ---------------------------------------------------------------------------

static inline bool IsValidUserPointer(const void* ptr) {
    uintptr_t u = reinterpret_cast<uintptr_t>(ptr);
    if ((u % sizeof(void*)) != 0) return false;
#if defined(_WIN64) || defined(__x86_64__) || defined(_M_AMD64) || defined(__aarch64__)
    return u >= 0x10000 && u < 0x00007FFFFFF00000ULL;
#else
    return u >= 0x10000 && u < 0x7FFF0000UL;
#endif
}

// IFrameworkElement GUID: a391d09b-4a99-4b7c-9d8d-6fa5d01f6fbf
static constexpr GUID IID_IFrameworkElement = {
    0xa391d09b, 0x4a99, 0x4b7c, { 0x9d, 0x8d, 0x6f, 0xa5, 0xd0, 0x1f, 0x6f, 0xbf }
};

// VolumeSystemTrayIconDataModel unique identifier GUID in SystemTray.dll
static constexpr GUID GUID_VolumeIcon = {
    0x7820ae73, 0x23e3, 0x4229, { 0x82, 0xc1, 0xe4, 0x1c, 0xb6, 0x7d, 0x5b, 0x9c }
};

struct IFrameworkElementVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void* This, REFIID riid, void** ppvObject);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void* This);
    ULONG   (STDMETHODCALLTYPE* Release)(void* This);
    HRESULT (STDMETHODCALLTYPE* GetIids)(void* This, ULONG* iidCount, IID** iids);
    HRESULT (STDMETHODCALLTYPE* GetRuntimeClassName)(void* This, void** className);
    HRESULT (STDMETHODCALLTYPE* GetTrustLevel)(void* This, int* trustLevel);
    HRESULT (STDMETHODCALLTYPE* get_Triggers)(void* This, void** value);
    HRESULT (STDMETHODCALLTYPE* get_Resources)(void* This, void** value);
    HRESULT (STDMETHODCALLTYPE* put_Resources)(void* This, void* value);
    HRESULT (STDMETHODCALLTYPE* get_Tag)(void* This, void** value);
    HRESULT (STDMETHODCALLTYPE* put_Tag)(void* This, void* value);
    HRESULT (STDMETHODCALLTYPE* get_Language)(void* This, void** value);
    HRESULT (STDMETHODCALLTYPE* put_Language)(void* This, void* value);
    HRESULT (STDMETHODCALLTYPE* get_ActualWidth)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* get_ActualHeight)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* get_Width)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* put_Width)(void* This, double value);
    HRESULT (STDMETHODCALLTYPE* get_Height)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* put_Height)(void* This, double value);
    HRESULT (STDMETHODCALLTYPE* get_MinWidth)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* put_MinWidth)(void* This, double value);
    HRESULT (STDMETHODCALLTYPE* get_MaxWidth)(void* This, double* value);
    HRESULT (STDMETHODCALLTYPE* put_MaxWidth)(void* This, double value);
};

struct IFrameworkElementCustom {
    IFrameworkElementVtbl* lpVtbl;
};

static double GetTargetContainerWidth(const Settings& settings) {
    if (settings.fixedContainerWidth < 0) {
        return 0.0; // Disabled
    }
    if (settings.fixedContainerWidth > 0) {
        return static_cast<double>(settings.fixedContainerWidth);
    }
    // Optimal automatic width in DIPs based on display style
    switch (settings.displayStyle) {
        case DisplayStyle::Vanilla:
            return 0.0; // Native auto-size
        case DisplayStyle::Number:
            return 30.0;
        case DisplayStyle::Prefix:
            return 72.0;
        case DisplayStyle::Emoji:
            return 66.0;
        case DisplayStyle::Percentage:
        default:
            return 42.0;
    }
}

static void ApplyVolumeContainerWidth(void* pIconView) {
    if (!IsValidUserPointer(pIconView) || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    Settings settingsCopy;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        settingsCopy = g_settings;
    }

    double targetWidth = GetTargetContainerWidth(settingsCopy);

    // Apply put_MinWidth on the IconView FrameworkElement
    IUnknown* pUnk = reinterpret_cast<IUnknown*>(pIconView);
    void* pFEVoid = nullptr;
    if (SUCCEEDED(pUnk->QueryInterface(IID_IFrameworkElement, &pFEVoid)) && pFEVoid) {
        IFrameworkElementCustom* pFE = reinterpret_cast<IFrameworkElementCustom*>(pFEVoid);
        pFE->lpVtbl->put_MinWidth(pFE, targetWidth);
        pFE->lpVtbl->Release(pFE);
    }

    // Also apply put_MinWidth on the hosted content element (Grid / TextIconContent) if present
    if (targetWidth > 0.0) {
        void* pHostedContent = *reinterpret_cast<void**>(reinterpret_cast<char*>(pIconView) + 0x1b0);
        if (IsValidUserPointer(pHostedContent)) {
            void* pHostedFEVoid = nullptr;
            IUnknown* pHostedUnk = reinterpret_cast<IUnknown*>(pHostedContent);
            if (SUCCEEDED(pHostedUnk->QueryInterface(IID_IFrameworkElement, &pHostedFEVoid)) && pHostedFEVoid) {
                IFrameworkElementCustom* pHostedFE = reinterpret_cast<IFrameworkElementCustom*>(pHostedFEVoid);
                pHostedFE->lpVtbl->put_MinWidth(pHostedFE, targetWidth);
                pHostedFE->lpVtbl->Release(pHostedFE);
            }
        }
    }
}

static bool IsVolumeIconViewModel(void* pIconViewModel) {
    if (!IsValidUserPointer(pIconViewModel)) {
        return false;
    }

    void* pInterface = *reinterpret_cast<void**>(pIconViewModel);
    if (!IsValidUserPointer(pInterface)) {
        pInterface = pIconViewModel;
    }

    if (!IsValidUserPointer(pInterface)) {
        return false;
    }

    void** vtbl = *reinterpret_cast<void***>(pInterface);
    if (!IsValidUserPointer(vtbl)) {
        return false;
    }

    // Slot 6: get_Configuration(void** ppConfig)
    using get_Configuration_t = HRESULT(STDMETHODCALLTYPE*)(void* This, void** ppConfig);
    if (!IsValidUserPointer(reinterpret_cast<void*>(vtbl[6]))) {
        return false;
    }
    auto get_Configuration = reinterpret_cast<get_Configuration_t>(vtbl[6]);

    void* pConfig = nullptr;
    if (FAILED(get_Configuration(pInterface, &pConfig)) || !IsValidUserPointer(pConfig)) {
        return false;
    }

    void** configVtbl = *reinterpret_cast<void***>(pConfig);
    bool isVolume = false;
    if (IsValidUserPointer(configVtbl)) {
        // Slot 9: get_Id(GUID* pGuid)
        if (IsValidUserPointer(reinterpret_cast<void*>(configVtbl[9]))) {
            using get_Id_t = HRESULT(STDMETHODCALLTYPE*)(void* This, GUID* pGuid);
            auto get_Id = reinterpret_cast<get_Id_t>(configVtbl[9]);
            GUID guid = {};
            if (SUCCEEDED(get_Id(pConfig, &guid))) {
                if (memcmp(&guid, &GUID_VolumeIcon, sizeof(GUID)) == 0) {
                    isVolume = true;
                }
            }
        }

        // Also check Slot 6: get_DataModel(void** ppDataModel)
        if (!isVolume && IsValidUserPointer(reinterpret_cast<void*>(configVtbl[6]))) {
            using get_DataModel_t = HRESULT(STDMETHODCALLTYPE*)(void* This, void** ppDataModel);
            auto get_DataModel = reinterpret_cast<get_DataModel_t>(configVtbl[6]);
            void* pDataModel = nullptr;
            if (SUCCEEDED(get_DataModel(pConfig, &pDataModel)) && pDataModel) {
                {
                    std::lock_guard<std::mutex> lock(g_dataModelMutex);
                    if (pDataModel == g_pVolumeDataModel) {
                        isVolume = true;
                    }
                }
                reinterpret_cast<IUnknown*>(pDataModel)->Release();
            }
        }

        reinterpret_cast<IUnknown*>(pConfig)->Release();
    }

    return isVolume;
}

// ---------------------------------------------------------------------------
// Formatting logic
// ---------------------------------------------------------------------------

static std::wstring FormatDigits(int percentage, PadStyle padStyle) {
    std::wstring s = std::to_wstring(percentage);
    if (padStyle == PadStyle::None) {
        return s;
    }

    constexpr size_t kTotalDigits = 3;
    if (s.length() < kTotalDigits) {
        wchar_t padChar = (padStyle == PadStyle::Zero) ? L'0' : L' ';
        s.insert(0, kTotalDigits - s.length(), padChar);
    }
    return s;
}

static std::wstring FormatMuteString(MuteStyle style, const std::wstring& customText, PadStyle padStyle) {
    switch (style) {
        case MuteStyle::Mute:
            return L"Mute";
        case MuteStyle::Zero:
            return FormatDigits(0, padStyle) + L"%";
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
    Settings settingsCopy;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        settingsCopy = g_settings;
    }

    if (isMuted && settingsCopy.displayStyle != DisplayStyle::Vanilla) {
        return FormatMuteString(settingsCopy.muteStyle, settingsCopy.customMuteText, settingsCopy.padStyle);
    }

    std::wstring s = FormatDigits(percentage, settingsCopy.padStyle);

    switch (settingsCopy.displayStyle) {
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
            return settingsCopy.customPrefix + s + L"%";

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

    shared_hstring_header* oldHeader = *ppHeader;

    // If the current header was already created by us and matches the desired text, reuse it
    if (oldHeader && oldHeader->flags == 0 && oldHeader->padding1 == MOD_HSTRING_MAGIC &&
        oldHeader->length == customText.length() && oldHeader->ptr &&
        wcsncmp(oldHeader->ptr, customText.c_str(), customText.length()) == 0) {
        return;
    }

    shared_hstring_header* newHeader = CreateSharedHString(customText);
    if (!newHeader) {
        return;
    }

    *ppHeader = newHeader;

    if (oldHeader && oldHeader != newHeader) {
        ReleaseSharedHString(oldHeader);
    }
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

    DWORD_PTR dwResult = 0;
    LRESULT lr = SendMessageTimeout(
        hWnd,
        g_runFromWindowThreadMsg,
        0,
        reinterpret_cast<LPARAM>(&param),
        SMTO_NORMAL | SMTO_ABORTIFHUNG,
        2000,
        &dwResult);

    UnhookWindowsHookEx(hook);

    if (lr == 0) {
        Wh_Log(L"SendMessageTimeout failed or timed out in RunFromWindowThread (error: %lu)", GetLastError());
        return false;
    }

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
// VolumeSystemTrayIconDataModel & IconView hooks
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

using IconView_OnViewModelChanged_t = void(__cdecl*)(void* pThis, void* pIconViewModel);
static IconView_OnViewModelChanged_t
    IconView_OnViewModelChanged_Original = nullptr;

using IconView_UpdateHostedContent_t = void(__cdecl*)(void* pThis);
static IconView_UpdateHostedContent_t
    IconView_UpdateHostedContent_Original = nullptr;

static void __cdecl VolumeSystemTrayIconDataModel_dtor_Hook(void* pThis) {
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel == pThis) {
            g_pVolumeDataModel = nullptr;
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_iconViewMutex);
        g_pVolumeIconView = nullptr;
    }
    if (VolumeSystemTrayIconDataModel_dtor_Original) {
        VolumeSystemTrayIconDataModel_dtor_Original(pThis);
    }
}

static void __cdecl VolumeSystemTrayIconDataModel_UpdateVolume_Hook(
    void* pThis, float volumeLevel, bool isMuted, void* hstringIcon) {
    static thread_local bool s_inHook = false;

    if (s_inHook || g_unloading.load(std::memory_order_relaxed)) {
        if (VolumeSystemTrayIconDataModel_UpdateVolume_Original) {
            VolumeSystemTrayIconDataModel_UpdateVolume_Original(
                pThis, volumeLevel, isMuted, hstringIcon);
        }
        return;
    }

    // Skip redundant volume notifications if data model, volume, and mute status are strictly identical
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        if (g_pVolumeDataModel == pThis &&
            g_lastVolumeLevel == volumeLevel &&
            g_lastIsMuted == isMuted) {
            return;
        }
    }

    if (VolumeSystemTrayIconDataModel_UpdateVolume_Original) {
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(
            pThis, volumeLevel, isMuted, hstringIcon);
    }

    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = pThis;
        g_lastVolumeLevel = volumeLevel;
        g_lastIsMuted = isMuted;

        ApplyCustomVolumeText(pThis, volumeLevel, isMuted);
    }

    // Ensure volume container width constraint is active on UI
    {
        std::lock_guard<std::mutex> lock(g_iconViewMutex);
        if (g_pVolumeIconView) {
            ApplyVolumeContainerWidth(g_pVolumeIconView);
        }
    }

    // Trigger UI redraw via CurrentData property notification with recursion guard
    if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
        s_inHook = true;
        std::wstring_view propName = L"CurrentData";
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, &propName);
        s_inHook = false;
    }
}

static void __cdecl IconView_OnViewModelChanged_Hook(void* pThis, void* pIconViewModel) {
    if (IconView_OnViewModelChanged_Original) {
        IconView_OnViewModelChanged_Original(pThis, pIconViewModel);
    }

    if (g_unloading.load(std::memory_order_relaxed) || !pThis) {
        return;
    }

    if (IsVolumeIconViewModel(pIconViewModel)) {
        {
            std::lock_guard<std::mutex> lock(g_iconViewMutex);
            g_pVolumeIconView = pThis;
        }
        ApplyVolumeContainerWidth(pThis);
    }
}

static void __cdecl IconView_UpdateHostedContent_Hook(void* pThis) {
    if (IconView_UpdateHostedContent_Original) {
        IconView_UpdateHostedContent_Original(pThis);
    }

    if (g_unloading.load(std::memory_order_relaxed) || !pThis) {
        return;
    }

    bool isVolume = false;
    {
        std::lock_guard<std::mutex> lock(g_iconViewMutex);
        if (g_pVolumeIconView == pThis) {
            isVolume = true;
        }
    }

    if (!isVolume) {
        // If g_pVolumeIconView was not yet captured, check IconView + 0x78 (IconViewModel)
        void* pVM = *reinterpret_cast<void**>(reinterpret_cast<char*>(pThis) + 0x78);
        if (pVM && IsVolumeIconViewModel(pVM)) {
            {
                std::lock_guard<std::mutex> lock(g_iconViewMutex);
                g_pVolumeIconView = pThis;
            }
            isVolume = true;
        }
    }

    if (isVolume) {
        ApplyVolumeContainerWidth(pThis);
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
                LR"(??_GVolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UEAAPEAXI@Z)",
                LR"(??1VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UAE@XZ)",
                LR"(??_GVolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UAEPAXI@Z)",
                LR"(winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::~VolumeSystemTrayIconDataModel)",
            },
            reinterpret_cast<void**>(&VolumeSystemTrayIconDataModel_dtor_Original),
            reinterpret_cast<void*>(VolumeSystemTrayIconDataModel_dtor_Hook),
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::OnViewModelChanged(struct winrt::SystemTray::IconViewModel const &))",
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::OnViewModelChanged(struct winrt::SystemTray::IconViewModel const & __ptr64) __ptr64)",
                LR"(?OnViewModelChanged@IconView@implementation@SystemTray@winrt@@AEAAXAEBUIconViewModel@34@@Z)",
                LR"(winrt::SystemTray::implementation::IconView::OnViewModelChanged)",
            },
            reinterpret_cast<void**>(&IconView_OnViewModelChanged_Original),
            reinterpret_cast<void*>(IconView_OnViewModelChanged_Hook),
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::UpdateHostedContent(void))",
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::UpdateHostedContent(void) __ptr64)",
                LR"(?UpdateHostedContent@IconView@implementation@SystemTray@winrt@@AEAAXXZ)",
                LR"(winrt::SystemTray::implementation::IconView::UpdateHostedContent)",
            },
            reinterpret_cast<void**>(&IconView_UpdateHostedContent_Original),
            reinterpret_cast<void*>(IconView_UpdateHostedContent_Hook),
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, systemTrayHooks, ARRAYSIZE(systemTrayHooks))) {
        Wh_Log(L"Failed to hook SystemTray volume symbols");
        return false;
    }

    if (!VolumeSystemTrayIconDataModel_dtor_Original) {
        Wh_Log(L"Warning: VolumeSystemTrayIconDataModel destructor symbol could not be hooked; lifetime tracking will rely on validity checks");
    }

    if (IconView_OnViewModelChanged_Original) {
        Wh_Log(L"Successfully hooked IconView::OnViewModelChanged for XAML container stabilization");
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
    Settings newSettings;

    auto displayStyleSetting = WindhawkUtils::StringSetting::make(L"displayStyle");
    PCWSTR displayStyleStr = displayStyleSetting.get();
    if (displayStyleStr) {
        if (wcscmp(displayStyleStr, L"vanilla") == 0 ||
            wcscmp(displayStyleStr, L"default") == 0 ||
            wcscmp(displayStyleStr, L"vanillaOnly") == 0) {
            newSettings.displayStyle = DisplayStyle::Vanilla;
        } else if (wcscmp(displayStyleStr, L"number") == 0 ||
                   wcscmp(displayStyleStr, L"numberOnly") == 0) {
            newSettings.displayStyle = DisplayStyle::Number;
        } else if (wcscmp(displayStyleStr, L"prefix") == 0 ||
                   wcscmp(displayStyleStr, L"volPrefix") == 0) {
            newSettings.displayStyle = DisplayStyle::Prefix;
        } else if (wcscmp(displayStyleStr, L"emoji") == 0 ||
                   wcscmp(displayStyleStr, L"emojiAndPercent") == 0 ||
                   wcscmp(displayStyleStr, L"iconAndPercent") == 0) {
            newSettings.displayStyle = DisplayStyle::Emoji;
        } else {
            newSettings.displayStyle = DisplayStyle::Percentage;
        }
    } else {
        newSettings.displayStyle = DisplayStyle::Percentage;
    }

    auto padStyleSetting = WindhawkUtils::StringSetting::make(L"padStyle");
    PCWSTR padStyleStr = padStyleSetting.get();
    if (padStyleStr) {
        if (wcscmp(padStyleStr, L"zero") == 0) {
            newSettings.padStyle = PadStyle::Zero;
        } else if (wcscmp(padStyleStr, L"space") == 0) {
            newSettings.padStyle = PadStyle::Space;
        } else {
            newSettings.padStyle = PadStyle::None;
        }
    } else {
        newSettings.padStyle = PadStyle::None;
    }

    newSettings.fixedContainerWidth = Wh_GetIntSetting(L"fixedContainerWidth");

    auto prefixSetting = WindhawkUtils::StringSetting::make(L"customPrefix");
    if (prefixSetting.get()) {
        newSettings.customPrefix = prefixSetting.get();
    } else {
        newSettings.customPrefix = L"Vol ";
    }

    auto muteStyleSetting = WindhawkUtils::StringSetting::make(L"muteStyle");
    PCWSTR muteStyleStr = muteStyleSetting.get();
    if (muteStyleStr) {
        if (wcscmp(muteStyleStr, L"mute") == 0) {
            newSettings.muteStyle = MuteStyle::Mute;
        } else if (wcscmp(muteStyleStr, L"zero") == 0) {
            newSettings.muteStyle = MuteStyle::Zero;
        } else if (wcscmp(muteStyleStr, L"cross") == 0) {
            newSettings.muteStyle = MuteStyle::Cross;
        } else if (wcscmp(muteStyleStr, L"emoji") == 0) {
            newSettings.muteStyle = MuteStyle::Emoji;
        } else if (wcscmp(muteStyleStr, L"glyph") == 0) {
            newSettings.muteStyle = MuteStyle::Glyph;
        } else if (wcscmp(muteStyleStr, L"custom") == 0) {
            newSettings.muteStyle = MuteStyle::Custom;
        } else {
            newSettings.muteStyle = MuteStyle::Mut;
        }
    } else {
        newSettings.muteStyle = MuteStyle::Mut;
    }

    auto customMuteSetting = WindhawkUtils::StringSetting::make(L"customMuteText");
    if (customMuteSetting.get()) {
        newSettings.customMuteText = customMuteSetting.get();
    } else {
        newSettings.customMuteText = L"Mute";
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(newSettings);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Volume Percentage Indicator mod in explorer.exe");

    LoadSettings();

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
    // Reset XAML container width to natural vanilla width (0.0)
    {
        std::lock_guard<std::mutex> lock(g_iconViewMutex);
        if (g_pVolumeIconView) {
            IUnknown* pUnk = reinterpret_cast<IUnknown*>(g_pVolumeIconView);
            void* pFEVoid = nullptr;
            if (SUCCEEDED(pUnk->QueryInterface(IID_IFrameworkElement, &pFEVoid)) && pFEVoid) {
                IFrameworkElementCustom* pFE = reinterpret_cast<IFrameworkElementCustom*>(pFEVoid);
                pFE->lpVtbl->put_MinWidth(pFE, 0.0);
                pFE->lpVtbl->Release(pFE);
            }
            void* pHostedContent = *reinterpret_cast<void**>(reinterpret_cast<char*>(g_pVolumeIconView) + 0x1b0);
            if (IsValidUserPointer(pHostedContent)) {
                void* pHostedFEVoid = nullptr;
                IUnknown* pHostedUnk = reinterpret_cast<IUnknown*>(pHostedContent);
                if (SUCCEEDED(pHostedUnk->QueryInterface(IID_IFrameworkElement, &pHostedFEVoid)) && pHostedFEVoid) {
                    IFrameworkElementCustom* pHostedFE = reinterpret_cast<IFrameworkElementCustom*>(pHostedFEVoid);
                    pHostedFE->lpVtbl->put_MinWidth(pHostedFE, 0.0);
                    pHostedFE->lpVtbl->Release(pHostedFE);
                }
            }
            g_pVolumeIconView = nullptr;
        }
    }

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
    Wh_Log(L"Preparing mod unload, restoring native icon and container size");

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
    {
        std::lock_guard<std::mutex> lock(g_iconViewMutex);
        if (g_pVolumeIconView && !g_unloading.load(std::memory_order_relaxed)) {
            ApplyVolumeContainerWidth(g_pVolumeIconView);
        }
    }

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
