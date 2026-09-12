// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Displays the exact master volume percentage in the system tray natively inside the Windows 11 volume button with real-time sync.
// @version         1.3.6
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Percentage Indicator

Replaces the default Windows 11 taskbar volume icon with the current volume level in real time directly inside the system tray button.

![Taskbar Volume Percentage Preview 1](https://i.imgur.com/vjwali8.png)
![Taskbar Volume Percentage Preview 2](https://i.imgur.com/XEf8m50.png)
![Taskbar Volume Percentage Preview 3](https://i.imgur.com/27iyViO.png)
![Taskbar Volume Percentage Preview 4](https://i.imgur.com/1alQd1j.png)

## How It Works

- **Real-Time Volume Display**: Displays the current master audio level as a percentage (`50%`), pure number (`50`), with a custom prefix (`Vol 50%`), with an emoji (`🔊 50%`), or using the default Windows speaker icon.
- **Mute Indicator**: When muted, automatically switches to a customizable mute display (`MUT`, `Mute`, `0%`, `✕`, `🔇`, native glyph, or custom text).
- **Layout Stabilization**: Automatically sizes the container to fit the percentage text smoothly without shifting adjacent taskbar icons. A fixed container width can also be set manually in the settings.

## Compatibility

Only Windows 11 is supported.

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
- fixedContainerWidth: 0
  $name: Fixed Container Width
  $description: Minimum width in pixels for the volume container to eliminate layout jitter. Set to 0 for automatic optimal width based on style (e.g. 42 for percentage), or enter a custom width (e.g. 50). Set to -1 to disable.
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
#include <winstring.h>
#include <winver.h>

#include <atomic>
#include <cmath>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

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
    DisplayStyle displayStyle{DisplayStyle::Percentage};
    int fixedContainerWidth{0};
    std::wstring customPrefix{L"Vol "};
    MuteStyle muteStyle{MuteStyle::Mut};
    std::wstring customMuteText{L"Mute"};
};

static Settings g_settings;
static std::mutex g_settingsMutex;

// Segoe Fluent Icons volume glyphs
constexpr wchar_t GLYPH_MUTE = 0xE74F;  // Crossed-out speaker
constexpr wchar_t GLYPH_VOL_0 = 0xE992; // 0 bars
constexpr wchar_t GLYPH_VOL_1 = 0xE993; // 1 bar
constexpr wchar_t GLYPH_VOL_2 = 0xE994; // 2 bars
constexpr wchar_t GLYPH_VOL_3 = 0xE995; // 3 bars

static void* g_pVolumeDataModel = nullptr;
static float g_lastVolumeLevel = 0.5f;
static bool g_lastIsMuted = false;
static std::mutex g_dataModelMutex;

static winrt::weak_ref<FrameworkElement> g_volumeIconViewWeak;
static std::atomic<double> g_lastAppliedIconViewWidth{-1.0};

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};

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

// ---------------------------------------------------------------------------
// WinUI Visual Tree inspection helpers
// ---------------------------------------------------------------------------

static FrameworkElement GetParentElementByName(const FrameworkElement& element, PCWSTR name) {
    auto parent = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    while (parent) {
        if (parent.Name() == name) {
            return parent;
        }
        parent = Media::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
    }
    return nullptr;
}

static bool IsChildOfElementByName(const FrameworkElement& element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

static FrameworkElement EnumChildElements(
    const FrameworkElement& element,
    const std::function<bool(const FrameworkElement&)>& enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

static FrameworkElement FindChildByName(const FrameworkElement& element, PCWSTR name) {
    return EnumChildElements(element, [name](const FrameworkElement& child) {
        return child.Name() == name;
    });
}

static FrameworkElement FindChildByClassName(const FrameworkElement& element, PCWSTR className) {
    return EnumChildElements(element, [className](const FrameworkElement& child) {
        return winrt::get_class_name(child) == className;
    });
}

static bool IsVolumeIconElement(const FrameworkElement& iconView) {
    auto containerGrid = FindChildByName(iconView, L"ContainerGrid");
    if (!containerGrid) {
        return false;
    }
    auto contentGrid = FindChildByName(containerGrid, L"ContentGrid");
    if (!contentGrid) {
        return false;
    }

    // Battery icon uses BatteryIconContent, not TextIconContent
    if (FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent")) {
        return false;
    }

    auto textIconContent = FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
    if (!textIconContent) {
        return false;
    }

    auto textContainer = FindChildByName(textIconContent, L"ContainerGrid");
    if (!textContainer) {
        return false;
    }
    auto baseGrid = FindChildByName(textContainer, L"Base");
    if (!baseGrid) {
        return false;
    }
    auto textBlockEl = FindChildByName(baseGrid, L"InnerTextBlock");
    if (!textBlockEl) {
        return false;
    }

    auto textBlock = textBlockEl.try_as<Controls::TextBlock>();
    if (!textBlock) {
        return false;
    }

    auto text = textBlock.Text();
    if (text.empty()) {
        return false;
    }

    std::wstring_view textView = text;

    wchar_t firstChar = textView[0];
    switch (firstChar) {
    case GLYPH_MUTE:
    case GLYPH_VOL_0:
    case GLYPH_VOL_1:
    case GLYPH_VOL_2:
    case GLYPH_VOL_3:
    case 0xEA85: // VolumeDisabled
    case 0xEBC5: // VolumeBars
        return true;
    default:
        break;
    }

    // Already customized by mod
    if (textView.find(L'%') != std::wstring_view::npos ||
        textView.find(L"MUT") != std::wstring_view::npos ||
        textView.find(L"Mute") != std::wstring_view::npos ||
        textView.find(L"Vol") != std::wstring_view::npos) {
        return true;
    }

    return false;
}

static void ApplyVolumeContainerWidth(const FrameworkElement& iconView) {
    if (!iconView || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    Settings settingsCopy;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        settingsCopy = g_settings;
    }

    double targetWidth = GetTargetContainerWidth(settingsCopy);
    double prevWidth = g_lastAppliedIconViewWidth.load(std::memory_order_relaxed);
    if (prevWidth != targetWidth) {
        g_lastAppliedIconViewWidth.store(targetWidth, std::memory_order_relaxed);
        iconView.MinWidth(targetWidth);
    }
}

// ---------------------------------------------------------------------------
// Text Formatting
// ---------------------------------------------------------------------------

static std::wstring FormatDigits(int value) {
    if (value <= 0)
        return L"0";
    std::wstring result;
    int temp = value;
    while (temp > 0) {
        result.push_back(static_cast<wchar_t>(L'0' + (temp % 10)));
        temp /= 10;
    }
    for (size_t i = 0, j = result.length() - 1; i < j; ++i, --j) {
        wchar_t c = result[i];
        result[i] = result[j];
        result[j] = c;
    }
    return result;
}

static std::wstring FormatMuteString(MuteStyle style, const std::wstring& customText) {
    switch (style) {
    case MuteStyle::Mute:
        return L"Mute";
    case MuteStyle::Zero:
        return L"0%";
    case MuteStyle::Cross:
        return L"\x2715"; // Unicode heavy multiplication x
    case MuteStyle::Emoji:
        return L"\xD83D\xDD07"; // Speaker with cancellation stroke
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
        return FormatMuteString(settingsCopy.muteStyle, settingsCopy.customMuteText);
    }

    std::wstring s = FormatDigits(percentage);

    switch (settingsCopy.displayStyle) {
    case DisplayStyle::Vanilla: {
        wchar_t glyph = GLYPH_MUTE;
        if (!isMuted) {
            if (percentage == 0)
                glyph = GLYPH_VOL_0;
            else if (percentage < 33)
                glyph = GLYPH_VOL_1;
            else if (percentage < 66)
                glyph = GLYPH_VOL_2;
            else
                glyph = GLYPH_VOL_3;
        }
        return std::wstring(1, glyph);
    }
    case DisplayStyle::Number:
        return s;
    case DisplayStyle::Prefix: {
        std::wstring result = settingsCopy.customPrefix;
        result += s;
        result += L"%";
        return result;
    }
    case DisplayStyle::Emoji: {
        std::wstring result = L"\xD83D\xDD0A ";
        result += s;
        result += L"%";
        return result;
    }
    case DisplayStyle::Percentage:
    default: {
        std::wstring result = s;
        result += L"%";
        return result;
    }
    }
}

// ---------------------------------------------------------------------------
// Thread dispatching
// ---------------------------------------------------------------------------

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

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

static UINT g_runFromWindowThreadRegisteredMsg = 0;

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_runFromWindowThreadRegisteredMsg != 0) {
        const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == g_runFromWindowThreadRegisteredMsg) {
            auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            if (param && param->proc) {
                param->proc(param->procParam);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
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
        RunFromWindowThreadHookProc,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, g_runFromWindowThreadRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
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

using IconView_UpdateHostedContent_t = void(__cdecl*)(void* pThis);
static IconView_UpdateHostedContent_t
    IconView_UpdateHostedContent_Original = nullptr;

using IconView_dtor_t = void(__cdecl*)(void* pThis);
static IconView_dtor_t IconView_dtor_Original = nullptr;

static std::atomic<size_t> g_iconTextOffset{0x90};

static size_t GetIconTextOffset(void* pThis) {
    if (!pThis) {
        return 0;
    }

    auto matchesGlyphOrText = [](HSTRING hStr) -> bool {
        if (!hStr) {
            return false;
        }
        UINT32 len = 0;
        PCWSTR raw = WindowsGetStringRawBuffer(hStr, &len);
        if (!raw || len == 0) {
            return false;
        }
        wchar_t firstChar = raw[0];
        return (firstChar >= 0xE700 && firstChar <= 0xE9FF) ||
               (firstChar >= L'0' && firstChar <= L'9') ||
               (firstChar >= L'a' && firstChar <= L'z') ||
               (firstChar >= L'A' && firstChar <= L'Z') ||
               firstChar == L' ' || firstChar == 0x2715 ||
               firstChar == 0x2007 || firstChar == 0x00A0 ||
               firstChar == 0xD83D || firstChar == 0xEA85 ||
               firstChar == 0xEBC5;
    };

    size_t cached = g_iconTextOffset.load(std::memory_order_relaxed);
    if (cached != 0) {
        HSTRING candidate = *reinterpret_cast<HSTRING*>(reinterpret_cast<char*>(pThis) + cached);
        if (matchesGlyphOrText(candidate)) {
            return cached;
        }
    }

    const size_t preferredOffsets[] = {0x90, 0xB8, 0x88};
    for (size_t prefOffset : preferredOffsets) {
        HSTRING candidate = *reinterpret_cast<HSTRING*>(reinterpret_cast<char*>(pThis) + prefOffset);
        if (matchesGlyphOrText(candidate)) {
            g_iconTextOffset.store(prefOffset, std::memory_order_relaxed);
            return prefOffset;
        }
    }

    return 0x90;
}

static void ApplyCustomVolumeText(void* pThis, float volumeLevel, bool isMuted) {
    if (!pThis || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    size_t offset = GetIconTextOffset(pThis);
    if (offset == 0) {
        return;
    }

    int percentage = static_cast<int>(std::round(volumeLevel * 100.0f));
    if (percentage < 0)
        percentage = 0;
    if (percentage > 100)
        percentage = 100;

    std::wstring customText = FormatVolumeText(percentage, isMuted);

    HSTRING* ppHString = reinterpret_cast<HSTRING*>(reinterpret_cast<char*>(pThis) + offset);
    HSTRING oldHString = *ppHString;

    if (oldHString) {
        UINT32 len = 0;
        PCWSTR raw = WindowsGetStringRawBuffer(oldHString, &len);
        if (raw && len == customText.length() && wcsncmp(raw, customText.c_str(), len) == 0) {
            return;
        }
    }

    HSTRING newHString = nullptr;
    if (SUCCEEDED(WindowsCreateString(customText.c_str(), static_cast<UINT32>(customText.length()), &newHString))) {
        *ppHString = newHString;
        if (oldHString) {
            WindowsDeleteString(oldHString);
        }
    }
}

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

static void __cdecl IconView_dtor_Hook(void* pThis) {
    if (auto currentView = g_volumeIconViewWeak.get()) {
        IUnknown* inner = pThis ? reinterpret_cast<IUnknown**>(pThis)[1] : nullptr;
        if (inner) {
            FrameworkElement thisElement = nullptr;
            inner->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(thisElement));
            if (thisElement && thisElement == currentView) {
                g_volumeIconViewWeak = nullptr;
                g_lastAppliedIconViewWidth.store(-1.0, std::memory_order_relaxed);
            }
        }
    }
    if (IconView_dtor_Original) {
        IconView_dtor_Original(pThis);
    }
}

static void __cdecl VolumeSystemTrayIconDataModel_OnDataModelChanged_Hook(
    void* pThis, const std::wstring_view* propertyName) {
    static thread_local bool s_inOnDataModelChanged = false;

    if (!s_inOnDataModelChanged && !g_unloading.load(std::memory_order_relaxed) &&
        pThis && propertyName && *propertyName == L"CurrentData") {
        float volumeLevel = 0.0f;
        bool isMuted = false;
        {
            std::lock_guard<std::mutex> lock(g_dataModelMutex);
            volumeLevel = g_lastVolumeLevel;
            isMuted = g_lastIsMuted;
            ApplyCustomVolumeText(pThis, volumeLevel, isMuted);
        }
    }

    if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
        s_inOnDataModelChanged = true;
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, propertyName);
        s_inOnDataModelChanged = false;
    }
}

static void __cdecl VolumeSystemTrayIconDataModel_UpdateVolume_Hook(
    void* pThis, float volumeLevel, bool isMuted, void* hstringIcon) {
    static thread_local bool s_inHook = false;

    if (g_unloading.load(std::memory_order_relaxed)) {
        if (VolumeSystemTrayIconDataModel_UpdateVolume_Original) {
            VolumeSystemTrayIconDataModel_UpdateVolume_Original(
                pThis, volumeLevel, isMuted, hstringIcon);
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        g_pVolumeDataModel = pThis;
        g_lastVolumeLevel = volumeLevel;
        g_lastIsMuted = isMuted;
    }

    // Always invoke the original Windows system function first
    if (VolumeSystemTrayIconDataModel_UpdateVolume_Original) {
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(
            pThis, volumeLevel, isMuted, hstringIcon);
    }

    if (s_inHook) {
        return;
    }

    Settings settingsCopy;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        settingsCopy = g_settings;
    }

    if (settingsCopy.displayStyle != DisplayStyle::Vanilla) {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        ApplyCustomVolumeText(pThis, volumeLevel, isMuted);
    }

    // Apply container width to volume icon if captured
    if (auto iconView = g_volumeIconViewWeak.get()) {
        ApplyVolumeContainerWidth(iconView);
    }

    // Trigger UI redraw via CurrentData property notification with recursion guard
    if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
        s_inHook = true;
        std::wstring_view propName = L"CurrentData";
        VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(pThis, &propName);
        s_inHook = false;
    }
}

static void __cdecl IconView_UpdateHostedContent_Hook(void* pThis) {
    if (IconView_UpdateHostedContent_Original) {
        IconView_UpdateHostedContent_Original(pThis);
    }

    if (g_unloading.load(std::memory_order_relaxed) || !pThis) {
        return;
    }

    IUnknown* inner = reinterpret_cast<IUnknown**>(pThis)[1];
    if (!inner) {
        return;
    }

    FrameworkElement iconView = nullptr;
    inner->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));
    if (!iconView || winrt::get_class_name(iconView) != L"SystemTray.IconView") {
        return;
    }

    if (!IsChildOfElementByName(iconView, L"ControlCenterButton")) {
        return;
    }

    if (IsVolumeIconElement(iconView)) {
        g_volumeIconViewWeak = iconView;
        ApplyVolumeContainerWidth(iconView);
    }
}

static bool HookSystemTraySymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
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
            reinterpret_cast<void*>(VolumeSystemTrayIconDataModel_OnDataModelChanged_Hook),
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
        {
            {
                LR"(??1VolumeSystemTrayIconDataModel@implementation@SystemTray@winrt@@UEAA@XZ)",
            },
            reinterpret_cast<void**>(&VolumeSystemTrayIconDataModel_dtor_Original),
            reinterpret_cast<void*>(VolumeSystemTrayIconDataModel_dtor_Hook),
            true,
        },
        {
            {
                LR"(??1IconView@implementation@SystemTray@winrt@@UEAA@XZ)",
            },
            reinterpret_cast<void**>(&IconView_dtor_Original),
            reinterpret_cast<void*>(IconView_dtor_Hook),
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
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
// Settings & Lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
    Settings newSettings;

    auto displayStyleSetting = WindhawkUtils::StringSetting::make(L"displayStyle");
    if (displayStyleSetting.get()) {
        if (wcscmp(displayStyleSetting.get(), L"number") == 0) {
            newSettings.displayStyle = DisplayStyle::Number;
        } else if (wcscmp(displayStyleSetting.get(), L"prefix") == 0) {
            newSettings.displayStyle = DisplayStyle::Prefix;
        } else if (wcscmp(displayStyleSetting.get(), L"emoji") == 0) {
            newSettings.displayStyle = DisplayStyle::Emoji;
        } else if (wcscmp(displayStyleSetting.get(), L"vanilla") == 0) {
            newSettings.displayStyle = DisplayStyle::Vanilla;
        } else {
            newSettings.displayStyle = DisplayStyle::Percentage;
        }
    }

    newSettings.fixedContainerWidth = Wh_GetIntSetting(L"fixedContainerWidth");

    auto customPrefixSetting = WindhawkUtils::StringSetting::make(L"customPrefix");
    if (customPrefixSetting.get()) {
        newSettings.customPrefix = customPrefixSetting.get();
    } else {
        newSettings.customPrefix = L"Vol ";
    }

    auto muteStyleSetting = WindhawkUtils::StringSetting::make(L"muteStyle");
    if (muteStyleSetting.get()) {
        if (wcscmp(muteStyleSetting.get(), L"mute") == 0) {
            newSettings.muteStyle = MuteStyle::Mute;
        } else if (wcscmp(muteStyleSetting.get(), L"zero") == 0) {
            newSettings.muteStyle = MuteStyle::Zero;
        } else if (wcscmp(muteStyleSetting.get(), L"cross") == 0) {
            newSettings.muteStyle = MuteStyle::Cross;
        } else if (wcscmp(muteStyleSetting.get(), L"emoji") == 0) {
            newSettings.muteStyle = MuteStyle::Emoji;
        } else if (wcscmp(muteStyleSetting.get(), L"glyph") == 0) {
            newSettings.muteStyle = MuteStyle::Glyph;
        } else if (wcscmp(muteStyleSetting.get(), L"custom") == 0) {
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

static void WINAPI BeforeUninitOnUIThread(void* /*parameter*/) {
    if (auto iconView = g_volumeIconViewWeak.get()) {
        iconView.MinWidth(0.0);
    }
    g_volumeIconViewWeak = nullptr;
    g_lastAppliedIconViewWidth.store(-1.0, std::memory_order_relaxed);

    void* dataModel = nullptr;
    float vol = 0.0f;
    bool muted = false;
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        dataModel = g_pVolumeDataModel;
        vol = g_lastVolumeLevel;
        muted = g_lastIsMuted;
        g_pVolumeDataModel = nullptr;
    }

    if (dataModel) {
        wchar_t defaultGlyph = muted ? GLYPH_MUTE : GLYPH_VOL_3;
        if (!muted) {
            int percentage = static_cast<int>(std::round(vol * 100.0f));
            if (percentage <= 0)
                defaultGlyph = GLYPH_VOL_0;
            else if (percentage < 33)
                defaultGlyph = GLYPH_VOL_1;
            else if (percentage < 66)
                defaultGlyph = GLYPH_VOL_2;
            else
                defaultGlyph = GLYPH_VOL_3;
        }
        std::wstring restoreStr(1, defaultGlyph);
        size_t offset = g_iconTextOffset.load(std::memory_order_relaxed);
        HSTRING* ppHString = reinterpret_cast<HSTRING*>(reinterpret_cast<char*>(dataModel) + offset);
        HSTRING oldHString = *ppHString;
        HSTRING newHString = nullptr;
        if (SUCCEEDED(WindowsCreateString(restoreStr.c_str(), 1, &newHString))) {
            *ppHString = newHString;
            if (oldHString) {
                WindowsDeleteString(oldHString);
            }
        }

        if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
            std::wstring_view propName = L"CurrentData";
            VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(dataModel, &propName);
        }
    }
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

static void WINAPI SettingsChangedOnUIThread(void* /*parameter*/) {
    if (auto iconView = g_volumeIconViewWeak.get()) {
        ApplyVolumeContainerWidth(iconView);
    }

    void* dataModel = nullptr;
    float vol = 0.0f;
    bool muted = false;
    {
        std::lock_guard<std::mutex> lock(g_dataModelMutex);
        dataModel = g_pVolumeDataModel;
        vol = g_lastVolumeLevel;
        muted = g_lastIsMuted;
    }

    if (dataModel && !g_unloading.load(std::memory_order_relaxed)) {
        Settings settingsCopy;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            settingsCopy = g_settings;
        }

        if (settingsCopy.displayStyle == DisplayStyle::Vanilla) {
            wchar_t defaultGlyph = muted ? GLYPH_MUTE : GLYPH_VOL_3;
            if (!muted) {
                int percentage = static_cast<int>(std::round(vol * 100.0f));
                if (percentage <= 0)
                    defaultGlyph = GLYPH_VOL_0;
                else if (percentage < 33)
                    defaultGlyph = GLYPH_VOL_1;
                else if (percentage < 66)
                    defaultGlyph = GLYPH_VOL_2;
                else
                    defaultGlyph = GLYPH_VOL_3;
            }
            std::wstring restoreStr(1, defaultGlyph);
            size_t offset = g_iconTextOffset.load(std::memory_order_relaxed);
            HSTRING* ppHString = reinterpret_cast<HSTRING*>(reinterpret_cast<char*>(dataModel) + offset);
            HSTRING oldHString = *ppHString;
            HSTRING newHString = nullptr;
            if (SUCCEEDED(WindowsCreateString(restoreStr.c_str(), 1, &newHString))) {
                *ppHString = newHString;
                if (oldHString) {
                    WindowsDeleteString(oldHString);
                }
            }
        } else {
            ApplyCustomVolumeText(dataModel, vol, muted);
        }

        if (VolumeSystemTrayIconDataModel_OnDataModelChanged_Original) {
            std::wstring_view propName = L"CurrentData";
            VolumeSystemTrayIconDataModel_OnDataModelChanged_Original(dataModel, &propName);
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
