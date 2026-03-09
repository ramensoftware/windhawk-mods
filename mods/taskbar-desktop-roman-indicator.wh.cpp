// ==WindhawkMod==
// @id              taskbar-desktop-roman-indicator
// @name            Taskbar Desktop Roman Indicator
// @description     Appends the current virtual desktop number in Roman numerals to the Windows 11 taskbar clock
// @version         1.0.0
// @author          Simon Benedict
// @github          https://github.com/simon-ami
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Desktop Roman Indicator

Shows the current virtual desktop number in the Windows 11 taskbar clock area.

## Features

* Roman or Arabic numbering
* Number mode or workspace markers mode
* Custom marker symbol for workspace markers mode
  Example symbols: `●`, `•`, `○`, `◉`
* Configurable left and right padding
* Configurable spacing between indicator characters
* Configurable indicator weight and size
* Notification-based desktop change detection
* Optional polling fallback

## Notes

* Windows 11 only
* The indicator is shown on the date line when available
* The mod uses virtual desktop notifications as the primary update path
* Polling fallback is disabled by default
* If the indicator doesn't update when switching desktops on your system, set the poll interval to `50` or `100` ms
* Lower poll intervals increase registry reads in Explorer
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- indicatorMode: markers
  $name: Indicator mode
  $description: Show either the current desktop number or one marker per desktop with the active desktop highlighted. [Default workspace markers]
  $options:
    - number: Current desktop number
    - markers: Workspace markers
- markerSymbol: ⬤
  $name: Marker symbol
  $description: Symbol or short text used for each workspace marker when indicator mode is set to workspace markers. E.g. ┃, ⬤, ●, •, ○, ◉, ⎕, ∎, ◆, ♦, ★ [Default ⬤]
- numberingFormat: roman
  $name: Numbering format
  $description: Choose whether the desktop indicator uses Roman or Arabic numerals. [Default Roman numerals]
  $options:
    - roman: Roman numerals
    - arabic: Arabic numerals
- leftPadding: 6
  $name: Left padding
  $description: Number of spaces between the clock text and the desktop indicator. [Default 6]
- rightPadding: 0
  $name: Right padding
  $description: Number of spaces after the desktop indicator. [Default 0]
- indicatorCharacterSpacing: 1
  $name: Indicator character spacing
  $description: Number of spaces inserted between characters in the desktop indicator. [Default 1]
- indicatorWeight: normal
  $name: Indicator weight
  $description: Font weight for the desktop indicator. [Default normal]
  $options:
    - normal: Normal
    - bold: Bold
- indicatorSize: normal
  $name: Indicator size
  $description: Relative font size for the desktop indicator. [Default normal]
  $options:
    - smaller: Smaller
    - normal: Normal
    - larger: Larger
- inactiveMarkerOpacity: 20
  $name: Inactive marker opacity
  $description: Opacity percentage for non-active workspace markers. 100 matches the active marker, lower values make inactive markers dimmer. [Default 20]
- pollIntervalMs: 0
  $name: Poll interval (ms)
  $description: Optional polling fallback for desktop changes. 0 disables polling. If notifications don't update on your system, try 50 or 100. [Default 0]
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <servprov.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
using WindhawkUtils::StringSetting;
using namespace winrt::Windows::UI::Text;
namespace Documents = winrt::Windows::UI::Xaml::Documents;

enum class IndicatorMode {
    Number,
    Markers,
};

enum class IndicatorSegmentStyle {
    Normal,
    Dimmed,
    Transparent,
};

struct IndicatorSegment {
    std::wstring text;
    IndicatorSegmentStyle style = IndicatorSegmentStyle::Normal;
};

struct IndicatorLayout {
    std::wstring renderedSuffix;
    double widestSuffixWidth = 0;
    std::vector<IndicatorSegment> suffixSegments;
};

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_22H2,
    Win11_24H2,
};

enum class NumberingFormat {
    Roman,
    Arabic,
};

enum class IndicatorWeight {
    Normal,
    Bold,
};

enum class IndicatorSize {
    Smaller,
    Normal,
    Larger,
};

struct ModSettings {
    IndicatorMode indicatorMode = IndicatorMode::Markers;
    std::wstring markerSymbol = L"\u2b24";
    NumberingFormat numberingFormat = NumberingFormat::Roman;
    int leftPadding = 6;
    int rightPadding = 0;
    int indicatorCharacterSpacing = 1;
    int inactiveMarkerOpacity = 20;
    int pollIntervalMs = 0;
    IndicatorWeight indicatorWeight = IndicatorWeight::Normal;
    IndicatorSize indicatorSize = IndicatorSize::Normal;
};

struct ClockEntry {
    winrt::weak_ref<Controls::TextBlock> dateTextBlock;
    winrt::weak_ref<Controls::TextBlock> timeTextBlock;
    std::wstring baseDateText;
    std::wstring baseTimeText;
    std::wstring lastAppliedSuffix;
    double originalDateMinWidth = 0;
    double originalTimeMinWidth = 0;
    TextAlignment originalDateTextAlignment = TextAlignment::End;
    TextAlignment originalTimeTextAlignment = TextAlignment::End;
    bool originalDateMinWidthCaptured = false;
    bool originalTimeMinWidthCaptured = false;
    bool originalDateTextAlignmentCaptured = false;
    bool originalTimeTextAlignmentCaptured = false;
    std::mutex mutex;
};

using ClockEntryPtr = std::shared_ptr<ClockEntry>;

struct NotificationInterfaceConfig {
    int64_t iidPart1 = 0;
    int64_t iidPart2 = 0;
    int methodCount = 0;
    int currentChangedIndex = -1;
    bool currentChangedHasMonitors = false;
};

struct VirtualDesktopNotificationObject {
    void** vtable = nullptr;
    LONG refCount = 1;
};

WinVersion g_winVersion = WinVersion::Unsupported;
WORD g_explorerBuildNumber = 0;
WORD g_explorerRevisionNumber = 0;
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_unloading = false;
std::atomic<int> g_currentDesktopNumber = 1;
std::atomic<int> g_pollIntervalMs = 0;
std::atomic<bool> g_virtualDesktopNotificationsRegistered = false;

std::mutex g_clockEntriesMutex;
std::vector<ClockEntryPtr> g_clockEntries;

HANDLE g_stopEvent = nullptr;
HANDLE g_pollThread = nullptr;
HANDLE g_notificationStopEvent = nullptr;
HANDLE g_notificationThread = nullptr;
HANDLE g_notificationReadyEvent = nullptr;
ModSettings g_settings;
DWORD g_virtualDesktopNotificationCookie = 0;
VirtualDesktopNotificationObject* g_virtualDesktopNotificationObject = nullptr;

using ClockSystemTrayIconDataModel_RefreshIcon_t = void(WINAPI*)(LPVOID, LPVOID);
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel_RefreshIcon_Original;
ClockSystemTrayIconDataModel_RefreshIcon_t
    ClockSystemTrayIconDataModel2_RefreshIcon_Original;

using DateTimeIconContent_OnApplyTemplate_t = void(WINAPI*)(LPVOID);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;

using BadgeIconContent_get_ViewModel_t = HRESULT(WINAPI*)(LPVOID, LPVOID);
BadgeIconContent_get_ViewModel_t BadgeIconContent_get_ViewModel_Original;

using ClockButton_v_OnDisplayStateChange_t = void(WINAPI*)(LPVOID, bool);
ClockButton_v_OnDisplayStateChange_t ClockButton_v_OnDisplayStateChange_Original;

int ReadCurrentDesktopNumberFromRegistry();
void UpdateAllClockEntries(bool captureBaseText);

const CLSID CLSID_ImmersiveShell = {
    0xc2f03a33,
    0x21f5,
    0x47fa,
    {0xb4, 0xbb, 0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39},
};

const GUID SID_VirtualDesktopNotificationService = {
    0xa501fdec,
    0x4a09,
    0x464c,
    {0xae, 0x4e, 0x1b, 0x9c, 0x21, 0xb8, 0x49, 0x18},
};

const GUID IID_IUnknown_Local = {
    0x00000000,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46},
};

const GUID IID_IVirtualDesktopNotificationService = {
    0x0cd45e71,
    0xd927,
    0x4f15,
    {0x8b, 0x0a, 0x8f, 0xef, 0x52, 0x53, 0x37, 0xbf},
};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Register(IUnknown* pNotification,
                                               DWORD* pdwCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0;
};

bool EndsWith(const std::wstring& value, const std::wstring& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::wstring TrimAppliedSuffix(const std::wstring& text,
                               const std::wstring& suffix) {
    if (suffix.empty()) {
        return text;
    }

    if (text == suffix) {
        return L"";
    }

    if (EndsWith(text, suffix)) {
        return text.substr(0, text.size() - suffix.size());
    }

    return text;
}

std::wstring ToRomanNumeral(int value) {
    if (value <= 0) {
        return L"?";
    }

    struct RomanPart {
        int value;
        PCWSTR numeral;
    };

    static constexpr RomanPart kRomanParts[] = {
        {1000, L"M"}, {900, L"CM"}, {500, L"D"}, {400, L"CD"}, {100, L"C"},
        {90, L"XC"},  {50, L"L"},   {40, L"XL"}, {10, L"X"},   {9, L"IX"},
        {5, L"V"},    {4, L"IV"},   {1, L"I"},
    };

    std::wstring result;
    for (const auto& part : kRomanParts) {
        while (value >= part.value) {
            result += part.numeral;
            value -= part.value;
        }
    }

    return result;
}

std::wstring ToArabicNumeral(int value) {
    return std::to_wstring(std::max(value, 0));
}

std::wstring BuildPadding(int count) {
    return std::wstring(std::max(count, 0), L' ');
}

std::wstring BuildTrailingPadding(int count) {
    return std::wstring(std::max(count, 0), L'\u00A0');
}

std::wstring BuildIndicatorGap() {
    return BuildPadding(std::max(g_settings.indicatorCharacterSpacing, 0));
}

std::wstring GetConfiguredMarkerSymbol() {
    return g_settings.markerSymbol.empty() ? std::wstring(L"\u2b24")
                                           : g_settings.markerSymbol;
}

std::wstring ApplyIndicatorCharacterSpacing(const std::wstring& text) {
    int spacing = std::max(g_settings.indicatorCharacterSpacing, 0);
    if (spacing == 0 || text.size() < 2) {
        return text;
    }

    std::wstring gap = BuildPadding(spacing);
    std::wstring result;
    result.reserve(text.size() + (text.size() - 1) * gap.size());

    for (size_t i = 0; i < text.size(); ++i) {
        if (i > 0) {
            result += gap;
        }
        result += text[i];
    }

    return result;
}

std::wstring FormatDesktopNumber(int value) {
    std::wstring desktopNumber;
    switch (g_settings.numberingFormat) {
        case NumberingFormat::Arabic:
            desktopNumber = ToArabicNumeral(value);
            break;
        case NumberingFormat::Roman:
        default:
            desktopNumber = ToRomanNumeral(value);
            break;
    }

    return ApplyIndicatorCharacterSpacing(desktopNumber);
}

std::wstring BuildIndicatorSuffix(bool hasBaseText,
                                  const std::wstring& desktopNumber) {
    std::wstring leftPadding =
        hasBaseText ? BuildPadding(g_settings.leftPadding) : L"";
    std::wstring rightPadding = BuildTrailingPadding(g_settings.rightPadding);
    return leftPadding + desktopNumber + rightPadding;
}

std::wstring BuildMarkerSequenceText(int desktopCount) {
    desktopCount = std::max(desktopCount, 1);

    std::wstring gap = BuildIndicatorGap();
    std::wstring markerSymbol = GetConfiguredMarkerSymbol();
    std::wstring result;

    for (int i = 0; i < desktopCount; ++i) {
        if (i > 0) {
            result += gap;
        }
        result += markerSymbol;
    }

    return result;
}

std::wstring BuildIndicatorCoreText(int currentDesktopNumber, int desktopCount) {
    if (g_settings.indicatorMode == IndicatorMode::Markers) {
        return BuildMarkerSequenceText(desktopCount);
    }

    return FormatDesktopNumber(currentDesktopNumber);
}

FontWeight GetConfiguredIndicatorFontWeight() {
    switch (g_settings.indicatorWeight) {
        case IndicatorWeight::Bold:
            return FontWeights::Bold();
        case IndicatorWeight::Normal:
        default:
            return FontWeights::Normal();
    }
}

double GetConfiguredIndicatorFontSize(double baseFontSize) {
    switch (g_settings.indicatorSize) {
        case IndicatorSize::Smaller:
            return baseFontSize * 0.9;
        case IndicatorSize::Larger:
            return baseFontSize * 1.15;
        case IndicatorSize::Normal:
        default:
            return baseFontSize;
    }
}

bool UseTabularNumeralsForIndicator() {
    return g_settings.indicatorMode == IndicatorMode::Number &&
           g_settings.numberingFormat == NumberingFormat::Arabic;
}

NotificationInterfaceConfig GetNotificationInterfaceConfig() {
    if (g_explorerBuildNumber < 22000) {
        return {};
    }

    if (g_explorerBuildNumber < 22483 ||
        (g_explorerBuildNumber == 22621 && g_explorerRevisionNumber < 2215)) {
        return {
            5481970284372180562ll,
            -1679294552252794956ll,
            13,
            11,
            true,
        };
    }

    if (g_explorerBuildNumber < 22631 ||
        (g_explorerBuildNumber == 22631 && g_explorerRevisionNumber < 3085)) {
        return {
            5123538856297626140ll,
            8491238173783613346ll,
            14,
            10,
            false,
        };
    }

    return {
        5308375338100058445ll,
        -2401892766147978065ll,
        14,
        10,
        false,
    };
}

bool IsCurrentNotificationInterface(REFIID riid) {
    auto config = GetNotificationInterfaceConfig();
    if (config.methodCount == 0) {
        return false;
    }

    auto riidParts = reinterpret_cast<const int64_t*>(&riid);
    return riidParts[0] == config.iidPart1 && riidParts[1] == config.iidPart2;
}

void HandleVirtualDesktopChangedNotification() {
    if (g_unloading) {
        return;
    }

    int currentDesktopNumber = ReadCurrentDesktopNumberFromRegistry();
    int previousDesktopNumber = g_currentDesktopNumber.exchange(currentDesktopNumber);
    if (currentDesktopNumber != previousDesktopNumber) {
        UpdateAllClockEntries(false);
    }
}

HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_QueryInterface(
    VirtualDesktopNotificationObject* pThis,
    REFIID riid,
    void** ppvObject) {
    if (!ppvObject) {
        return E_POINTER;
    }

    *ppvObject = nullptr;

    if (InlineIsEqualGUID(riid, IID_IUnknown_Local) ||
        IsCurrentNotificationInterface(riid)) {
        *ppvObject = pThis;
        InterlockedIncrement(&pThis->refCount);
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE
VirtualDesktopNotification_AddRef(VirtualDesktopNotificationObject* pThis) {
    return static_cast<ULONG>(InterlockedIncrement(&pThis->refCount));
}

ULONG STDMETHODCALLTYPE
VirtualDesktopNotification_Release(VirtualDesktopNotificationObject* pThis) {
    LONG refCount = InterlockedDecrement(&pThis->refCount);
    if (refCount == 0) {
        delete[] pThis->vtable;
        delete pThis;
    }

    return static_cast<ULONG>(std::max<LONG>(refCount, 0));
}

HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_NoOp() {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_CurrentChanged(
    VirtualDesktopNotificationObject*) {
    HandleVirtualDesktopChangedNotification();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_CurrentChangedWithMonitors(
    VirtualDesktopNotificationObject*,
    void*,
    void*,
    void*) {
    HandleVirtualDesktopChangedNotification();
    return S_OK;
}

VirtualDesktopNotificationObject* CreateVirtualDesktopNotificationObject() {
    auto config = GetNotificationInterfaceConfig();
    if (config.methodCount == 0 || config.currentChangedIndex < 0 ||
        config.currentChangedIndex >= config.methodCount) {
        return nullptr;
    }

    auto object = new (std::nothrow) VirtualDesktopNotificationObject();
    if (!object) {
        return nullptr;
    }

    object->vtable = new (std::nothrow) void*[config.methodCount];
    if (!object->vtable) {
        delete object;
        return nullptr;
    }

    for (int i = 0; i < config.methodCount; ++i) {
        object->vtable[i] = reinterpret_cast<void*>(
            &VirtualDesktopNotification_NoOp);
    }

    object->vtable[0] = reinterpret_cast<void*>(
        &VirtualDesktopNotification_QueryInterface);
    object->vtable[1] = reinterpret_cast<void*>(
        &VirtualDesktopNotification_AddRef);
    object->vtable[2] = reinterpret_cast<void*>(
        &VirtualDesktopNotification_Release);
    if (config.currentChangedHasMonitors) {
        object->vtable[config.currentChangedIndex] = reinterpret_cast<void*>(
            &VirtualDesktopNotification_CurrentChangedWithMonitors);
    } else {
        object->vtable[config.currentChangedIndex] = reinterpret_cast<void*>(
            &VirtualDesktopNotification_CurrentChanged);
    }

    return object;
}

std::vector<BYTE> ReadRegistryValue(HKEY root,
                                    const std::wstring& subKey,
                                    const std::wstring& valueName,
                                    DWORD* valueType) {
    DWORD type = 0;
    DWORD size = 0;

    LONG ret = RegGetValueW(root, subKey.c_str(), valueName.c_str(),
                            RRF_RT_ANY, &type, nullptr, &size);
    if (ret != ERROR_SUCCESS || size == 0) {
        return {};
    }

    std::vector<BYTE> buffer(size);
    ret = RegGetValueW(root, subKey.c_str(), valueName.c_str(), RRF_RT_ANY, &type,
                       buffer.data(), &size);
    if (ret != ERROR_SUCCESS) {
        return {};
    }

    buffer.resize(size);
    if (valueType) {
        *valueType = type;
    }

    return buffer;
}

bool ParseGuidValue(const std::vector<BYTE>& buffer, DWORD type, GUID* guid) {
    if (!guid || buffer.empty()) {
        return false;
    }

    if (type == REG_BINARY && buffer.size() >= sizeof(GUID)) {
        memcpy(guid, buffer.data(), sizeof(GUID));
        return true;
    }

    if (type == REG_SZ || type == REG_EXPAND_SZ) {
        const wchar_t* text = reinterpret_cast<const wchar_t*>(buffer.data());
        if (!text || !*text) {
            return false;
        }

        return SUCCEEDED(CLSIDFromString(text, guid));
    }

    return false;
}

std::vector<GUID> ReadVirtualDesktopIds() {
    std::vector<GUID> ids;
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    std::wstring sessionPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\" +
        std::to_wstring(sessionId) + L"\\VirtualDesktops";

    for (const auto& path : {
             std::wstring(
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops"),
             sessionPath,
         }) {
        DWORD type = 0;
        auto buffer =
            ReadRegistryValue(HKEY_CURRENT_USER, path, L"VirtualDesktopIDs", &type);
        if (type != REG_BINARY || buffer.size() < sizeof(GUID)) {
            continue;
        }

        size_t count = buffer.size() / sizeof(GUID);
        ids.resize(count);
        memcpy(ids.data(), buffer.data(), count * sizeof(GUID));
        return ids;
    }

    return ids;
}

bool ReadCurrentDesktopGuid(GUID* guid) {
    if (!guid) {
        return false;
    }

    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    std::wstring sessionPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\" +
        std::to_wstring(sessionId) + L"\\VirtualDesktops";

    for (const auto& path : {
             sessionPath,
             std::wstring(
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops"),
         }) {
        DWORD type = 0;
        auto buffer = ReadRegistryValue(HKEY_CURRENT_USER, path,
                                        L"CurrentVirtualDesktop", &type);
        if (ParseGuidValue(buffer, type, guid)) {
            return true;
        }
    }

    return false;
}

int ReadCurrentDesktopNumberFromRegistry() {
    auto desktopIds = ReadVirtualDesktopIds();
    if (desktopIds.empty()) {
        return 1;
    }

    GUID currentGuid{};
    if (!ReadCurrentDesktopGuid(&currentGuid)) {
        return 1;
    }

    for (size_t i = 0; i < desktopIds.size(); ++i) {
        if (InlineIsEqualGUID(desktopIds[i], currentGuid)) {
            return static_cast<int>(i + 1);
        }
    }

    return 1;
}

int ReadDesktopCountFromRegistry() {
    auto desktopIds = ReadVirtualDesktopIds();
    return desktopIds.empty() ? 1 : static_cast<int>(desktopIds.size());
}

void LoadSettings() {
    StringSetting indicatorMode = StringSetting::make(L"indicatorMode");
    if (wcscmp(indicatorMode.get(), L"number") == 0) {
        g_settings.indicatorMode = IndicatorMode::Number;
    } else {
        g_settings.indicatorMode = IndicatorMode::Markers;
    }

    StringSetting markerSymbol = StringSetting::make(L"markerSymbol");
    g_settings.markerSymbol = markerSymbol.get();
    if (g_settings.markerSymbol.empty()) {
        g_settings.markerSymbol = L"\u2b24";
    }

    StringSetting numberingFormat = StringSetting::make(L"numberingFormat");
    if (wcscmp(numberingFormat.get(), L"arabic") == 0) {
        g_settings.numberingFormat = NumberingFormat::Arabic;
    } else {
        g_settings.numberingFormat = NumberingFormat::Roman;
    }

    StringSetting indicatorWeight = StringSetting::make(L"indicatorWeight");
    if (wcscmp(indicatorWeight.get(), L"bold") == 0) {
        g_settings.indicatorWeight = IndicatorWeight::Bold;
    } else {
        g_settings.indicatorWeight = IndicatorWeight::Normal;
    }

    StringSetting indicatorSize = StringSetting::make(L"indicatorSize");
    if (wcscmp(indicatorSize.get(), L"smaller") == 0) {
        g_settings.indicatorSize = IndicatorSize::Smaller;
    } else if (wcscmp(indicatorSize.get(), L"larger") == 0) {
        g_settings.indicatorSize = IndicatorSize::Larger;
    } else {
        g_settings.indicatorSize = IndicatorSize::Normal;
    }

    g_settings.leftPadding = std::max(0, Wh_GetIntSetting(L"leftPadding"));
    g_settings.rightPadding = std::max(0, Wh_GetIntSetting(L"rightPadding"));
    g_settings.indicatorCharacterSpacing =
        std::max(0, Wh_GetIntSetting(L"indicatorCharacterSpacing"));
    g_settings.inactiveMarkerOpacity =
        std::clamp(Wh_GetIntSetting(L"inactiveMarkerOpacity"), 0, 100);
    g_settings.pollIntervalMs = std::clamp(Wh_GetIntSetting(L"pollIntervalMs"), 0, 2000);
    g_pollIntervalMs.store(g_settings.pollIntervalMs);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module, UINT* length) {
    void* fixedFileInfo = nullptr;
    UINT fixedFileInfoLength = 0;

    HRSRC versionResource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (versionResource) {
        HGLOBAL loadedResource = LoadResource(module, versionResource);
        if (loadedResource) {
            void* lockedResource = LockResource(loadedResource);
            if (lockedResource &&
                (!VerQueryValueW(lockedResource, L"\\", &fixedFileInfo,
                                 &fixedFileInfoLength) ||
                 fixedFileInfoLength == 0)) {
                fixedFileInfo = nullptr;
                fixedFileInfoLength = 0;
            }
        }
    }

    if (length) {
        *length = fixedFileInfoLength;
    }

    return static_cast<VS_FIXEDFILEINFO*>(fixedFileInfo);
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD revision = LOWORD(fixedFileInfo->dwFileVersionLS);

    g_explorerBuildNumber = build;
    g_explorerRevisionNumber = revision;

    if (major != 10) {
        return WinVersion::Unsupported;
    }

    if (build < 22000) {
        return WinVersion::Win10;
    }

    if (build == 22000) {
        return WinVersion::Win11;
    }

    if (build < 26100) {
        return WinVersion::Win11_22H2;
    }

    return WinVersion::Win11_24H2;
}

bool RegisterVirtualDesktopNotificationsOnCurrentThread() {
    if (g_virtualDesktopNotificationsRegistered) {
        return true;
    }

    auto config = GetNotificationInterfaceConfig();
    if (config.methodCount == 0) {
        Wh_Log(L"Virtual desktop notifications unavailable for Explorer build %u.%u",
               g_explorerBuildNumber, g_explorerRevisionNumber);
        return false;
    }

    IServiceProvider* serviceProvider = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr,
                                  CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                  IID_PPV_ARGS(&serviceProvider));
    if (FAILED(hr)) {
        Wh_Log(L"Virtual desktop notification registration failed at CoCreateInstance: hr=0x%08X",
               hr);
        return false;
    }

    IVirtualDesktopNotificationService* notificationService = nullptr;
    hr = serviceProvider->QueryService(SID_VirtualDesktopNotificationService,
                                       IID_IVirtualDesktopNotificationService,
                                       reinterpret_cast<void**>(&notificationService));
    serviceProvider->Release();
    if (FAILED(hr)) {
        Wh_Log(L"Virtual desktop notification registration failed at QueryService: hr=0x%08X",
               hr);
        return false;
    }

    auto notificationObject = CreateVirtualDesktopNotificationObject();
    if (!notificationObject) {
        Wh_Log(L"Virtual desktop notification registration failed: couldn't create notification object");
        notificationService->Release();
        return false;
    }

    DWORD cookie = 0;
    hr = notificationService->Register(reinterpret_cast<IUnknown*>(notificationObject),
                                       &cookie);
    notificationService->Release();

    if (FAILED(hr) || cookie == 0) {
        Wh_Log(L"Virtual desktop notification registration failed at Register: hr=0x%08X, cookie=%lu",
               hr, cookie);
        VirtualDesktopNotification_Release(notificationObject);
        return false;
    }

    g_virtualDesktopNotificationObject = notificationObject;
    g_virtualDesktopNotificationCookie = cookie;
    g_virtualDesktopNotificationsRegistered = true;
    return true;
}

void UnregisterVirtualDesktopNotificationsOnCurrentThread() {
    if (!g_virtualDesktopNotificationsRegistered) {
        return;
    }

    IServiceProvider* serviceProvider = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr,
                                  CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                  IID_PPV_ARGS(&serviceProvider));
    if (SUCCEEDED(hr)) {
        IVirtualDesktopNotificationService* notificationService = nullptr;
        hr = serviceProvider->QueryService(
            SID_VirtualDesktopNotificationService,
            IID_IVirtualDesktopNotificationService,
            reinterpret_cast<void**>(&notificationService));
        serviceProvider->Release();

        if (SUCCEEDED(hr)) {
            notificationService->Unregister(g_virtualDesktopNotificationCookie);
            notificationService->Release();
        }
    }

    if (g_virtualDesktopNotificationObject) {
        VirtualDesktopNotification_Release(g_virtualDesktopNotificationObject);
        g_virtualDesktopNotificationObject = nullptr;
    }

    g_virtualDesktopNotificationCookie = 0;
    g_virtualDesktopNotificationsRegistered = false;
}

DWORD WINAPI VirtualDesktopNotificationThreadProc(LPVOID) {
    HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(initHr)) {
        Wh_Log(L"Virtual desktop notification thread failed to initialize COM: hr=0x%08X",
               initHr);
        if (g_notificationReadyEvent) {
            SetEvent(g_notificationReadyEvent);
        }
        return 0;
    }

    bool registered = RegisterVirtualDesktopNotificationsOnCurrentThread();
    if (g_notificationReadyEvent) {
        SetEvent(g_notificationReadyEvent);
    }

    if (registered && g_notificationStopEvent) {
        bool stopping = false;
        while (!stopping) {
            DWORD waitResult = MsgWaitForMultipleObjects(
                1, &g_notificationStopEvent, FALSE, INFINITE, QS_ALLINPUT);
            switch (waitResult) {
                case WAIT_OBJECT_0:
                    stopping = true;
                    break;
                case WAIT_OBJECT_0 + 1: {
                    MSG msg;
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    break;
                }
                default:
                    Wh_Log(L"Virtual desktop notification thread wait returned unexpectedly: %lu",
                           waitResult);
                    stopping = true;
                    break;
            }
        }
    }

    if (registered) {
        UnregisterVirtualDesktopNotificationsOnCurrentThread();
    }

    CoUninitialize();
    return 0;
}

bool EnsureVirtualDesktopNotificationThread() {
    if (g_notificationThread) {
        return g_virtualDesktopNotificationsRegistered.load();
    }

    g_notificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_notificationStopEvent) {
        Wh_Log(L"Failed to create virtual desktop notification stop event");
        return false;
    }

    g_notificationReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_notificationReadyEvent) {
        Wh_Log(L"Failed to create virtual desktop notification ready event");
        CloseHandle(g_notificationStopEvent);
        g_notificationStopEvent = nullptr;
        return false;
    }

    g_notificationThread = CreateThread(nullptr, 0,
                                        VirtualDesktopNotificationThreadProc,
                                        nullptr, 0, nullptr);
    if (!g_notificationThread) {
        Wh_Log(L"Failed to create virtual desktop notification thread");
        CloseHandle(g_notificationReadyEvent);
        g_notificationReadyEvent = nullptr;
        CloseHandle(g_notificationStopEvent);
        g_notificationStopEvent = nullptr;
        return false;
    }

    DWORD waitResult = WaitForSingleObject(g_notificationReadyEvent, 3000);
    if (waitResult != WAIT_OBJECT_0) {
        Wh_Log(L"Timed out waiting for virtual desktop notification thread readiness: %lu",
               waitResult);
    }

    return g_virtualDesktopNotificationsRegistered.load();
}

void StopVirtualDesktopNotificationThread() {
    if (g_notificationStopEvent) {
        SetEvent(g_notificationStopEvent);
    }

    if (g_notificationThread) {
        WaitForSingleObject(g_notificationThread, 3000);
        CloseHandle(g_notificationThread);
        g_notificationThread = nullptr;
    }

    if (g_notificationReadyEvent) {
        CloseHandle(g_notificationReadyEvent);
        g_notificationReadyEvent = nullptr;
    }

    if (g_notificationStopEvent) {
        CloseHandle(g_notificationStopEvent);
        g_notificationStopEvent = nullptr;
    }
}

FrameworkElement FindDescendantByName(const DependencyObject& parent,
                                      const winrt::hstring& name) {
    auto frameworkElement = parent.try_as<FrameworkElement>();
    if (frameworkElement && frameworkElement.Name() == name) {
        return frameworkElement;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childrenCount; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        auto result = FindDescendantByName(child, name);
        if (result) {
            return result;
        }
    }

    return nullptr;
}

bool TryGetClockTextBlocks(FrameworkElement root,
                           Controls::TextBlock* dateTextBlock,
                           Controls::TextBlock* timeTextBlock) {
    if (!dateTextBlock || !timeTextBlock) {
        return false;
    }

    *dateTextBlock = FindDescendantByName(root, L"DateInnerTextBlock")
                         .try_as<Controls::TextBlock>();
    *timeTextBlock = FindDescendantByName(root, L"TimeInnerTextBlock")
                         .try_as<Controls::TextBlock>();

    return static_cast<bool>(*dateTextBlock || *timeTextBlock);
}

ClockEntryPtr AddOrGetClockEntry(Controls::TextBlock dateTextBlock,
                                 Controls::TextBlock timeTextBlock) {
    auto sameBlock = [&](const ClockEntryPtr& entry) {
        auto existingDate = entry->dateTextBlock.get();
        auto existingTime = entry->timeTextBlock.get();
        return (existingDate && dateTextBlock && existingDate == dateTextBlock) ||
               (existingTime && timeTextBlock && existingTime == timeTextBlock);
    };

    std::lock_guard<std::mutex> lock(g_clockEntriesMutex);

    g_clockEntries.erase(
        std::remove_if(g_clockEntries.begin(), g_clockEntries.end(),
                       [](const ClockEntryPtr& entry) {
                           return !entry->dateTextBlock.get() &&
                                  !entry->timeTextBlock.get();
                       }),
        g_clockEntries.end());

    auto it = std::find_if(g_clockEntries.begin(), g_clockEntries.end(), sameBlock);
    if (it != g_clockEntries.end()) {
        (*it)->dateTextBlock = dateTextBlock;
        (*it)->timeTextBlock = timeTextBlock;
        return *it;
    }

    auto entry = std::make_shared<ClockEntry>();
    entry->dateTextBlock = dateTextBlock;
    entry->timeTextBlock = timeTextBlock;
    g_clockEntries.push_back(entry);
    return entry;
}

std::vector<ClockEntryPtr> GetClockEntriesSnapshot() {
    std::lock_guard<std::mutex> lock(g_clockEntriesMutex);
    return g_clockEntries;
}

double MeasureSingleRunTextWidth(const Controls::TextBlock& source,
                                 const std::wstring& text,
                                 FontWeight fontWeight) {
    Controls::TextBlock measureBlock;
    measureBlock.FontFamily(source.FontFamily());
    measureBlock.FontSize(source.FontSize());
    measureBlock.FontStretch(source.FontStretch());
    measureBlock.FontStyle(source.FontStyle());
    measureBlock.FontWeight(source.FontWeight());
    measureBlock.CharacterSpacing(source.CharacterSpacing());
    measureBlock.TextWrapping(TextWrapping::NoWrap);

    auto inlines = measureBlock.Inlines();
    inlines.Clear();

    Documents::Run run;
    run.Text(winrt::hstring(text));
    run.FontWeight(fontWeight);
    run.FontSize(GetConfiguredIndicatorFontSize(source.FontSize()));
    if (UseTabularNumeralsForIndicator()) {
        Documents::Typography::SetNumeralAlignment(
            run, FontNumeralAlignment::Tabular);
    }
    inlines.Append(run);

    measureBlock.Measure(winrt::Windows::Foundation::Size{
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
    });
    return measureBlock.DesiredSize().Width;
}

double MeasureIndicatorTextWidth(const Controls::TextBlock& source,
                                 const std::wstring& baseText,
                                 const std::wstring& suffix) {
    Controls::TextBlock measureBlock;
    measureBlock.FontFamily(source.FontFamily());
    measureBlock.FontSize(source.FontSize());
    measureBlock.FontStretch(source.FontStretch());
    measureBlock.FontStyle(source.FontStyle());
    measureBlock.FontWeight(source.FontWeight());
    measureBlock.CharacterSpacing(source.CharacterSpacing());
    measureBlock.TextWrapping(TextWrapping::NoWrap);

    auto inlines = measureBlock.Inlines();
    inlines.Clear();

    if (!baseText.empty()) {
        Documents::Run baseRun;
        baseRun.Text(winrt::hstring(baseText));
        inlines.Append(baseRun);
    }

    if (!suffix.empty()) {
        Documents::Run suffixRun;
        suffixRun.Text(winrt::hstring(suffix));
        suffixRun.FontWeight(GetConfiguredIndicatorFontWeight());
        suffixRun.FontSize(GetConfiguredIndicatorFontSize(source.FontSize()));
        if (UseTabularNumeralsForIndicator()) {
            Documents::Typography::SetNumeralAlignment(
                suffixRun, FontNumeralAlignment::Tabular);
        }
        inlines.Append(suffixRun);
    }

    measureBlock.Measure(winrt::Windows::Foundation::Size{
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
    });
    return measureBlock.DesiredSize().Width;
}

std::vector<IndicatorSegment> BuildMarkerSuffixSegments(bool hasBaseText,
                                                        int currentDesktopNumber,
                                                        int desktopCount) {
    std::vector<IndicatorSegment> segments;

    if (hasBaseText && g_settings.leftPadding > 0) {
        segments.push_back({BuildPadding(g_settings.leftPadding),
                            IndicatorSegmentStyle::Normal});
    }

    std::wstring gap = BuildIndicatorGap();
    std::wstring markerSymbol = GetConfiguredMarkerSymbol();
    desktopCount = std::max(desktopCount, 1);
    currentDesktopNumber = std::clamp(currentDesktopNumber, 1, desktopCount);

    for (int i = 1; i <= desktopCount; ++i) {
        if (i > 1 && !gap.empty()) {
            segments.push_back({gap, IndicatorSegmentStyle::Normal});
        }

        segments.push_back(
            {markerSymbol,
             i == currentDesktopNumber ? IndicatorSegmentStyle::Normal
                                       : IndicatorSegmentStyle::Dimmed});
    }

    if (g_settings.rightPadding > 0) {
        segments.push_back(
            {BuildTrailingPadding(g_settings.rightPadding),
             IndicatorSegmentStyle::Normal});
    }

    return segments;
}

IndicatorLayout BuildIndicatorLayout(const Controls::TextBlock& source,
                                     bool hasBaseText,
                                     int currentDesktopNumber,
                                     int desktopCount) {
    IndicatorLayout layout;

    if (g_settings.indicatorMode == IndicatorMode::Markers) {
        layout.renderedSuffix = BuildIndicatorSuffix(
            hasBaseText, BuildIndicatorCoreText(currentDesktopNumber, desktopCount));
        layout.widestSuffixWidth = MeasureSingleRunTextWidth(
            source, layout.renderedSuffix, GetConfiguredIndicatorFontWeight());
        layout.suffixSegments = BuildMarkerSuffixSegments(
            hasBaseText, currentDesktopNumber, desktopCount);
        return layout;
    }

    std::wstring visibleSuffix = BuildIndicatorSuffix(
        hasBaseText, BuildIndicatorCoreText(currentDesktopNumber, desktopCount));

    double currentWidth = MeasureSingleRunTextWidth(
        source, visibleSuffix, GetConfiguredIndicatorFontWeight());
    layout.widestSuffixWidth = currentWidth;

    for (int i = 1; i <= desktopCount; ++i) {
        std::wstring candidateSuffix =
            BuildIndicatorSuffix(hasBaseText, FormatDesktopNumber(i));
        double candidateWidth = MeasureSingleRunTextWidth(
            source, candidateSuffix, GetConfiguredIndicatorFontWeight());
        if (candidateWidth > layout.widestSuffixWidth) {
            layout.widestSuffixWidth = candidateWidth;
        }
    }

    struct SpacerCandidate {
        wchar_t ch;
        double width;
    };

    const wchar_t spacerChars[] = {
        L'\u00A0',  // no-break space
        L'\u2007',  // figure space
        L'\u2005',  // four-per-em space
        L'\u2009',  // thin space
        L'\u200A',  // hair space
    };

    auto buildInvisibleFillerText = [&](double targetWidth) {
        if (targetWidth <= 0.1) {
            return std::wstring{};
        }

        std::vector<SpacerCandidate> candidates;
        for (wchar_t ch : spacerChars) {
            std::wstring text(1, ch);
            double width = MeasureSingleRunTextWidth(
                source, text, GetConfiguredIndicatorFontWeight());
            if (width > 0.01) {
                candidates.push_back({ch, width});
            }
        }

        std::sort(candidates.begin(), candidates.end(),
                  [](const SpacerCandidate& a, const SpacerCandidate& b) {
                      return a.width > b.width;
                  });

        std::wstring result;
        double usedWidth = 0;
        for (const auto& candidate : candidates) {
            while (usedWidth + candidate.width <= targetWidth + 0.01) {
                result += candidate.ch;
                usedWidth += candidate.width;
            }
        }

        return result;
    };

    double fillerWidth = std::max(0.0, layout.widestSuffixWidth - currentWidth);
    std::wstring leftFiller = buildInvisibleFillerText(fillerWidth / 2.0);
    double leftFillerWidth = MeasureSingleRunTextWidth(
        source, leftFiller, GetConfiguredIndicatorFontWeight());
    std::wstring rightFiller =
        buildInvisibleFillerText(std::max(0.0, fillerWidth - leftFillerWidth));

    layout.renderedSuffix = leftFiller + visibleSuffix + rightFiller;
    layout.suffixSegments.push_back(
        {leftFiller, IndicatorSegmentStyle::Transparent});
    layout.suffixSegments.push_back(
        {visibleSuffix, IndicatorSegmentStyle::Normal});
    layout.suffixSegments.push_back(
        {rightFiller, IndicatorSegmentStyle::Transparent});
    return layout;
}

template <typename TInlines>
void AppendRun(const TInlines& inlines,
               const std::wstring& text,
               FontWeight fontWeight,
               Media::Brush foreground = nullptr,
               bool useIndicatorTypography = true,
               double fontSize = 0) {
    if (text.empty()) {
        return;
    }

    Documents::Run run;
    run.Text(winrt::hstring(text));
    run.FontWeight(fontWeight);
    if (fontSize > 0) {
        run.FontSize(fontSize);
    }
    if (useIndicatorTypography && UseTabularNumeralsForIndicator()) {
        Documents::Typography::SetNumeralAlignment(
            run, FontNumeralAlignment::Tabular);
    }
    if (foreground) {
        run.Foreground(foreground);
    }
    inlines.Append(run);
}

Media::Brush CreateTransparentBrush() {
    return Media::SolidColorBrush(winrt::Windows::UI::Color{0, 255, 255, 255});
}

Media::Brush CreateDimmedIndicatorBrush(Controls::TextBlock targetTextBlock) {
    BYTE opacity = static_cast<BYTE>(
        std::clamp(g_settings.inactiveMarkerOpacity, 0, 100) * 255 / 100);
    winrt::Windows::UI::Color color{opacity, 255, 255, 255};

    auto solidBrush = targetTextBlock.Foreground().try_as<Media::SolidColorBrush>();
    if (solidBrush) {
        color = solidBrush.Color();
        color.A = static_cast<BYTE>((color.A * std::clamp(
            g_settings.inactiveMarkerOpacity, 0, 100)) / 100);
    }

    return Media::SolidColorBrush(color);
}

void SetIndicatorTextBlockContent(Controls::TextBlock targetTextBlock,
                                  const std::wstring& baseText,
                                  const IndicatorLayout& layout) {
    auto inlines = targetTextBlock.Inlines();
    inlines.Clear();

    if (!baseText.empty()) {
        AppendRun(inlines, baseText, targetTextBlock.FontWeight(), nullptr, false,
                  targetTextBlock.FontSize());
    }

    Media::Brush transparentBrush = CreateTransparentBrush();
    Media::Brush dimmedBrush = CreateDimmedIndicatorBrush(targetTextBlock);
    double indicatorFontSize =
        GetConfiguredIndicatorFontSize(targetTextBlock.FontSize());

    for (const auto& segment : layout.suffixSegments) {
        Media::Brush foreground = nullptr;
        if (segment.style == IndicatorSegmentStyle::Transparent) {
            foreground = transparentBrush;
        } else if (segment.style == IndicatorSegmentStyle::Dimmed) {
            foreground = dimmedBrush;
        }

        AppendRun(inlines, segment.text, GetConfiguredIndicatorFontWeight(),
                  foreground, true, indicatorFontSize);
    }
}

void EnsureReservedWidth(const ClockEntryPtr& entry,
                         Controls::TextBlock targetTextBlock,
                         const std::wstring& baseText) {
    if (!targetTextBlock) {
        return;
    }

    int desktopCount = ReadDesktopCountFromRegistry();
    double widestWidth = 0;

    if (g_settings.indicatorMode == IndicatorMode::Markers) {
        widestWidth = MeasureIndicatorTextWidth(
            targetTextBlock, baseText,
            BuildIndicatorSuffix(!baseText.empty(), BuildMarkerSequenceText(desktopCount)));
    } else {
        widestWidth = MeasureIndicatorTextWidth(
            targetTextBlock, baseText,
            BuildIndicatorSuffix(!baseText.empty(), FormatDesktopNumber(1)));

        for (int i = 2; i <= desktopCount; ++i) {
            double candidateWidth = MeasureIndicatorTextWidth(
                targetTextBlock, baseText,
                BuildIndicatorSuffix(!baseText.empty(), FormatDesktopNumber(i)));
            if (candidateWidth > widestWidth) {
                widestWidth = candidateWidth;
            }
        }
    }

    if (targetTextBlock == entry->dateTextBlock.get()) {
        if (!entry->originalDateMinWidthCaptured) {
            entry->originalDateMinWidth = targetTextBlock.MinWidth();
            entry->originalDateMinWidthCaptured = true;
        }

        targetTextBlock.MinWidth(std::max(entry->originalDateMinWidth, widestWidth));
    } else if (targetTextBlock == entry->timeTextBlock.get()) {
        if (!entry->originalTimeMinWidthCaptured) {
            entry->originalTimeMinWidth = targetTextBlock.MinWidth();
            entry->originalTimeMinWidthCaptured = true;
        }

        targetTextBlock.MinWidth(std::max(entry->originalTimeMinWidth, widestWidth));
    }
}

void EnsureStableTextAlignment(const ClockEntryPtr& entry,
                               Controls::TextBlock targetTextBlock) {
    if (!targetTextBlock) {
        return;
    }

    if (targetTextBlock == entry->dateTextBlock.get()) {
        if (!entry->originalDateTextAlignmentCaptured) {
            entry->originalDateTextAlignment = targetTextBlock.TextAlignment();
            entry->originalDateTextAlignmentCaptured = true;
        }

        targetTextBlock.TextAlignment(TextAlignment::Start);
    } else if (targetTextBlock == entry->timeTextBlock.get()) {
        if (!entry->originalTimeTextAlignmentCaptured) {
            entry->originalTimeTextAlignment = targetTextBlock.TextAlignment();
            entry->originalTimeTextAlignmentCaptured = true;
        }

        targetTextBlock.TextAlignment(TextAlignment::Start);
    }
}

void ApplyIndicator(const ClockEntryPtr& entry) {
    if (!entry || g_unloading) {
        return;
    }

    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    if (!dateTextBlock && !timeTextBlock) {
        return;
    }

    int currentDesktopNumber = g_currentDesktopNumber.load();
    int desktopCount = ReadDesktopCountFromRegistry();

    std::lock_guard<std::mutex> lock(entry->mutex);

    bool useDateLine = dateTextBlock && !entry->baseDateText.empty();
    if (!useDateLine && dateTextBlock && !timeTextBlock) {
        useDateLine = true;
    }

    if (useDateLine) {
        IndicatorLayout layout =
            BuildIndicatorLayout(dateTextBlock, !entry->baseDateText.empty(),
                                 currentDesktopNumber, desktopCount);
        EnsureReservedWidth(entry, dateTextBlock, entry->baseDateText);
        EnsureStableTextAlignment(entry, dateTextBlock);
        SetIndicatorTextBlockContent(dateTextBlock, entry->baseDateText, layout);
        entry->lastAppliedSuffix = layout.renderedSuffix;
    } else if (timeTextBlock) {
        IndicatorLayout layout =
            BuildIndicatorLayout(timeTextBlock, !entry->baseTimeText.empty(),
                                 currentDesktopNumber, desktopCount);
        EnsureReservedWidth(entry, timeTextBlock, entry->baseTimeText);
        EnsureStableTextAlignment(entry, timeTextBlock);
        SetIndicatorTextBlockContent(timeTextBlock, entry->baseTimeText, layout);
        entry->lastAppliedSuffix = layout.renderedSuffix;
    }
}

void CaptureClockBaseText(const ClockEntryPtr& entry) {
    if (!entry) {
        return;
    }

    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    if (!dateTextBlock && !timeTextBlock) {
        return;
    }

    std::lock_guard<std::mutex> lock(entry->mutex);

    if (dateTextBlock) {
        entry->baseDateText =
            TrimAppliedSuffix(dateTextBlock.Text().c_str(),
                              entry->lastAppliedSuffix);
    }

    if (timeTextBlock) {
        entry->baseTimeText =
            TrimAppliedSuffix(timeTextBlock.Text().c_str(),
                              entry->lastAppliedSuffix);
    }
}

void RestoreClockText(const ClockEntryPtr& entry) {
    if (!entry) {
        return;
    }

    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    if (!dateTextBlock && !timeTextBlock) {
        return;
    }

    std::lock_guard<std::mutex> lock(entry->mutex);

    if (dateTextBlock &&
        (!entry->baseDateText.empty() || !entry->lastAppliedSuffix.empty())) {
        dateTextBlock.Inlines().Clear();
        dateTextBlock.Text(winrt::hstring(entry->baseDateText));
        if (entry->originalDateMinWidthCaptured) {
            dateTextBlock.MinWidth(entry->originalDateMinWidth);
        }
        if (entry->originalDateTextAlignmentCaptured) {
            dateTextBlock.TextAlignment(entry->originalDateTextAlignment);
        }
    }

    if (timeTextBlock &&
        (!entry->baseTimeText.empty() || !entry->lastAppliedSuffix.empty())) {
        timeTextBlock.Inlines().Clear();
        timeTextBlock.Text(winrt::hstring(entry->baseTimeText));
        if (entry->originalTimeMinWidthCaptured) {
            timeTextBlock.MinWidth(entry->originalTimeMinWidth);
        }
        if (entry->originalTimeTextAlignmentCaptured) {
            timeTextBlock.TextAlignment(entry->originalTimeTextAlignment);
        }
    }

    entry->lastAppliedSuffix.clear();
}

void DispatchToEntry(const ClockEntryPtr& entry, bool captureBaseText) {
    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    auto dispatchSource = dateTextBlock ? dateTextBlock : timeTextBlock;
    if (!dispatchSource) {
        return;
    }

    auto dispatcher = dispatchSource.Dispatcher();
    if (!dispatcher) {
        return;
    }

    dispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
        [entry, captureBaseText] {
            if (g_unloading) {
                return;
            }

            if (captureBaseText) {
                CaptureClockBaseText(entry);
            }

            ApplyIndicator(entry);
        });
}

void DispatchRestoreToEntry(const ClockEntryPtr& entry) {
    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    auto dispatchSource = dateTextBlock ? dateTextBlock : timeTextBlock;
    if (!dispatchSource) {
        return;
    }

    auto dispatcher = dispatchSource.Dispatcher();
    if (!dispatcher) {
        return;
    }

    dispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [entry] { RestoreClockText(entry); });
}

void UpdateAllClockEntriesCurrentThread(bool captureBaseText) {
    for (const auto& entry : GetClockEntriesSnapshot()) {
        if (captureBaseText) {
            CaptureClockBaseText(entry);
        }

        ApplyIndicator(entry);
    }
}

void UpdateAllClockEntries(bool captureBaseText) {
    for (const auto& entry : GetClockEntriesSnapshot()) {
        DispatchToEntry(entry, captureBaseText);
    }
}

void RestoreAllClockEntries() {
    for (const auto& entry : GetClockEntriesSnapshot()) {
        DispatchRestoreToEntry(entry);
    }
}

void ProcessDateTimeIconContentElement(FrameworkElement root) {
    Controls::TextBlock dateTextBlock = nullptr;
    Controls::TextBlock timeTextBlock = nullptr;
    if (!TryGetClockTextBlocks(root, &dateTextBlock, &timeTextBlock)) {
        return;
    }

    auto entry = AddOrGetClockEntry(dateTextBlock, timeTextBlock);
    CaptureClockBaseText(entry);
    ApplyIndicator(entry);
}

void ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
    LPVOID pThis,
    LPVOID param1,
    ClockSystemTrayIconDataModel_RefreshIcon_t original) {
    original(pThis, param1);
    UpdateAllClockEntriesCurrentThread(true);
}

void WINAPI ClockSystemTrayIconDataModel_RefreshIcon_Hook(LPVOID pThis,
                                                          LPVOID param1) {
    ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
        pThis, param1, ClockSystemTrayIconDataModel_RefreshIcon_Original);
}

void WINAPI ClockSystemTrayIconDataModel2_RefreshIcon_Hook(LPVOID pThis,
                                                           LPVOID param1) {
    ClockSystemTrayIconDataModel_RefreshIcon_Hook_Impl(
        pThis, param1, ClockSystemTrayIconDataModel2_RefreshIcon_Original);
}

void WINAPI DateTimeIconContent_OnApplyTemplate_Hook(LPVOID pThis) {
    DateTimeIconContent_OnApplyTemplate_Original(pThis);

    IUnknown* elementUnknown = *((IUnknown**)pThis + 1);
    if (!elementUnknown) {
        return;
    }

    FrameworkElement root = nullptr;
    elementUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                   winrt::put_abi(root));
    if (!root) {
        return;
    }

    try {
        ProcessDateTimeIconContentElement(root);
    } catch (...) {
        Wh_Log(L"Failed to process DateTimeIconContent template");
    }
}

HRESULT WINAPI BadgeIconContent_get_ViewModel_Hook(LPVOID pThis, LPVOID pArgs) {
    HRESULT hr = BadgeIconContent_get_ViewModel_Original(pThis, pArgs);

    try {
        winrt::Windows::Foundation::IInspectable inspectable = nullptr;
        winrt::check_hresult(
            ((IUnknown*)pThis)
                ->QueryInterface(
                    winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                    winrt::put_abi(inspectable)));

        if (winrt::get_class_name(inspectable) == L"SystemTray.DateTimeIconContent") {
            auto root = inspectable.try_as<FrameworkElement>();
            if (root && root.IsLoaded()) {
                ProcessDateTimeIconContentElement(root);
            }
        }
    } catch (...) {
    }

    return hr;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_RefreshIcon_Original,
            ClockSystemTrayIconDataModel_RefreshIcon_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_RefreshIcon_Original,
            ClockSystemTrayIconDataModel2_RefreshIcon_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &BadgeIconContent_get_ViewModel_Original,
            BadgeIconContent_get_ViewModel_Hook,
            true,
        },
    };

    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookExplorerExeSymbols() {
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(protected: virtual void __cdecl ClockButton::v_OnDisplayStateChange(bool))"},
            &ClockButton_v_OnDisplayStateChange_Original,
            nullptr,
            true,
        },
    };

    if (!HookSymbols(GetModuleHandle(nullptr), explorerExeHooks,
                     ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols for explorer.exe failed");
        return false;
    }

    return true;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

void RefreshClockButtonWindow(HWND hWnd) {
    if (!hWnd || !ClockButton_v_OnDisplayStateChange_Original) {
        return;
    }

    HWND clockButtonWnd = FindWindowExW(hWnd, nullptr, L"ClockButton", nullptr);
    if (!clockButtonWnd) {
        return;
    }

    LONG_PTR clockButtonLongPtr = GetWindowLongPtrW(clockButtonWnd, 0);
    if (clockButtonLongPtr) {
        ClockButton_v_OnDisplayStateChange_Original((LPVOID)clockButtonLongPtr,
                                                    true);
    }
}

void TriggerWin11ClockUpdateWatcher() {
    constexpr WCHAR kTempValueName[] =
        L"_temp_windhawk_taskbar-desktop-roman-indicator";

    HKEY hSubKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                                KEY_WRITE, &hSubKey);
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open clock update watcher key: %ld", result);
        return;
    }

    if (RegSetValueExW(hSubKey, kTempValueName, 0, REG_SZ, (const BYTE*)L"",
                       sizeof(WCHAR)) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to create temp clock update watcher value");
    } else if (RegDeleteValueW(hSubKey, kTempValueName) != ERROR_SUCCESS) {
        Wh_Log(L"Failed to remove temp clock update watcher value");
    }

    RegCloseKey(hSubKey);
}

void RefreshLiveTaskbarClock() {
    if (g_winVersion < WinVersion::Win11) {
        return;
    }

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        return;
    }

    TriggerWin11ClockUpdateWatcher();

    if (!ClockButton_v_OnDisplayStateChange_Original) {
        return;
    }

    RefreshClockButtonWindow(taskbarWnd);

    DWORD taskbarThreadId = GetWindowThreadProcessId(taskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return;
    }

    auto enumWindowsProc = [](HWND hWnd) {
        WCHAR className[32];
        if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
            return;
        }

        RefreshClockButtonWindow(hWnd);
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            proc(hWnd);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR moduleName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", moduleName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
            RefreshLiveTaskbarClock();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR libFileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(libFileName, file, flags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, libFileName);
    }

    return module;
}

DWORD WINAPI DesktopPollThreadProc(LPVOID) {
    int lastDesktopNumber = ReadCurrentDesktopNumberFromRegistry();
    g_currentDesktopNumber.store(lastDesktopNumber);

    while (WaitForSingleObject(g_stopEvent, g_pollIntervalMs.load()) == WAIT_TIMEOUT) {
        int desktopNumber = ReadCurrentDesktopNumberFromRegistry();
        if (desktopNumber != lastDesktopNumber) {
            lastDesktopNumber = desktopNumber;
            g_currentDesktopNumber.store(desktopNumber);
            UpdateAllClockEntries(false);
        }
    }

    return 0;
}

bool StartDesktopPollThread() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"Failed to create desktop poll stop event");
        return false;
    }

    g_pollThread = CreateThread(nullptr, 0, DesktopPollThreadProc, nullptr, 0, nullptr);
    if (!g_pollThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return false;
    }
    return true;
}

void StopDesktopPollThread() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_pollThread) {
        WaitForSingleObject(g_pollThread, 3000);
        CloseHandle(g_pollThread);
        g_pollThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void EnsureDesktopChangeTracking() {
    bool notificationsRegistered = EnsureVirtualDesktopNotificationThread();

    if (g_settings.pollIntervalMs <= 0) {
        if (g_pollThread || g_stopEvent) {
            StopDesktopPollThread();
        }
    } else if (!g_pollThread && !g_stopEvent) {
        if (!StartDesktopPollThread()) {
            Wh_Log(L"Failed to start desktop poll thread");
        }
    }

    if (!notificationsRegistered && g_settings.pollIntervalMs <= 0) {
        Wh_Log(L"Desktop change tracking has no active update mechanism");
    }
}

BOOL Wh_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_winVersion = GetExplorerVersion();
    if (g_winVersion < WinVersion::Win11) {
        Wh_Log(L"Only Windows 11 is supported");
        return FALSE;
    }

    g_currentDesktopNumber.store(ReadCurrentDesktopNumberFromRegistry());

    HookExplorerExeSymbols();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    }

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    if (!loadLibraryExW) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    EnsureDesktopChangeTracking();

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnsureDesktopChangeTracking();
    RefreshLiveTaskbarClock();
    UpdateAllClockEntries(true);
}

void Wh_ModAfterInit() {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true) &&
                HookTaskbarViewDllSymbols(taskbarViewModule)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    EnsureDesktopChangeTracking();
    RefreshLiveTaskbarClock();
    UpdateAllClockEntries(true);
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopVirtualDesktopNotificationThread();
    StopDesktopPollThread();
    RestoreAllClockEntries();
    Sleep(200);
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_clockEntriesMutex);
    g_clockEntries.clear();
}
