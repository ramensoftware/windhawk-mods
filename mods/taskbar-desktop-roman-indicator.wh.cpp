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
* Configurable left and right padding
* Configurable spacing between indicator characters
* Optional bold indicator text
* Configurable polling interval
* Fast desktop change detection

## Notes

* Windows 11 only
* The indicator is shown on the date line when available
* The current desktop is detected by polling Explorer's virtual desktop state
* Lower poll intervals react faster but increase registry reads in Explorer;
  50-100 ms is usually fine, while going much lower is rarely worth it
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- numberingFormat: roman
  $name: Numbering format
  $description: Choose whether the desktop indicator uses Roman or Arabic numerals.
  $options:
    - roman: Roman numerals
    - arabic: Arabic numerals
- leftPadding: 6
  $name: Left padding
  $description: Number of spaces between the clock text and the desktop indicator.
- rightPadding: 0
  $name: Right padding
  $description: Number of spaces after the desktop indicator.
- indicatorCharacterSpacing: 1
  $name: Indicator character spacing
  $description: Number of spaces inserted between characters in the desktop indicator.
- indicatorWeight: bold
  $name: Indicator weight
  $description: Font weight for the desktop indicator.
  $options:
    - normal: Normal
    - bold: Bold
- pollIntervalMs: 100
  $name: Poll interval (ms)
  $description: How often Explorer checks for desktop changes. Lower values react faster but do more registry reads; 50-100 ms is usually fine, and values much lower than that are rarely worthwhile.
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

struct IndicatorLayout {
    std::wstring visibleSuffix;
    std::wstring renderedSuffix;
    double widestSuffixWidth = 0;
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

struct ModSettings {
    NumberingFormat numberingFormat = NumberingFormat::Roman;
    int leftPadding = 3;
    int rightPadding = 0;
    int indicatorCharacterSpacing = 1;
    int pollIntervalMs = 100;
    IndicatorWeight indicatorWeight = IndicatorWeight::Normal;
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

WinVersion g_winVersion = WinVersion::Unsupported;
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_unloading = false;
std::atomic<int> g_currentDesktopNumber = 1;
std::atomic<int> g_pollIntervalMs = 100;

std::mutex g_clockEntriesMutex;
std::vector<ClockEntryPtr> g_clockEntries;

HANDLE g_stopEvent = nullptr;
HANDLE g_pollThread = nullptr;
ModSettings g_settings;

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
    std::wstring rightPadding = BuildPadding(g_settings.rightPadding);
    return leftPadding + desktopNumber + rightPadding;
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

bool UseTabularNumeralsForIndicator() {
    return g_settings.numberingFormat == NumberingFormat::Arabic;
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

    g_settings.leftPadding = std::max(0, Wh_GetIntSetting(L"leftPadding"));
    g_settings.rightPadding = std::max(0, Wh_GetIntSetting(L"rightPadding"));
    g_settings.indicatorCharacterSpacing =
        std::max(0, Wh_GetIntSetting(L"indicatorCharacterSpacing"));
    g_settings.pollIntervalMs = std::clamp(Wh_GetIntSetting(L"pollIntervalMs"), 25, 2000);
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

IndicatorLayout BuildIndicatorLayout(const Controls::TextBlock& source,
                                     bool hasBaseText,
                                     const std::wstring& desktopNumber) {
    IndicatorLayout layout;
    layout.visibleSuffix = BuildIndicatorSuffix(hasBaseText, desktopNumber);

    double currentWidth = MeasureSingleRunTextWidth(
        source, layout.visibleSuffix, GetConfiguredIndicatorFontWeight());
    layout.widestSuffixWidth = currentWidth;

    int desktopCount = ReadDesktopCountFromRegistry();
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

    layout.renderedSuffix = leftFiller + layout.visibleSuffix + rightFiller;
    return layout;
}

template <typename TInlines>
void AppendRun(const TInlines& inlines,
               const std::wstring& text,
               FontWeight fontWeight,
               bool transparent = false) {
    if (text.empty()) {
        return;
    }

    Documents::Run run;
    run.Text(winrt::hstring(text));
    run.FontWeight(fontWeight);
    if (UseTabularNumeralsForIndicator()) {
        Documents::Typography::SetNumeralAlignment(
            run, FontNumeralAlignment::Tabular);
    }
    if (transparent) {
        winrt::Windows::UI::Color transparentColor{};
        run.Foreground(Media::SolidColorBrush(transparentColor));
    }
    inlines.Append(run);
}

void SetIndicatorTextBlockContent(Controls::TextBlock targetTextBlock,
                                  const std::wstring& baseText,
                                  const IndicatorLayout& layout) {
    auto inlines = targetTextBlock.Inlines();
    inlines.Clear();

    if (!baseText.empty()) {
        AppendRun(inlines, baseText, targetTextBlock.FontWeight());
    }

    std::wstring leftFiller;
    std::wstring rightFiller;

    if (layout.renderedSuffix.size() >= layout.visibleSuffix.size()) {
        size_t visiblePos = layout.renderedSuffix.find(layout.visibleSuffix);
        if (visiblePos != std::wstring::npos) {
            leftFiller = layout.renderedSuffix.substr(0, visiblePos);
            rightFiller =
                layout.renderedSuffix.substr(visiblePos + layout.visibleSuffix.size());
        }
    }

    AppendRun(inlines, leftFiller, GetConfiguredIndicatorFontWeight(), true);
    AppendRun(inlines, layout.visibleSuffix, GetConfiguredIndicatorFontWeight());
    AppendRun(inlines, rightFiller, GetConfiguredIndicatorFontWeight(), true);
}

void EnsureReservedWidth(const ClockEntryPtr& entry,
                         Controls::TextBlock targetTextBlock,
                         const std::wstring& baseText) {
    if (!targetTextBlock) {
        return;
    }

    int desktopCount = ReadDesktopCountFromRegistry();
    double widestWidth = MeasureIndicatorTextWidth(
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

void ApplyRomanIndicator(const ClockEntryPtr& entry) {
    if (!entry || g_unloading) {
        return;
    }

    auto dateTextBlock = entry->dateTextBlock.get();
    auto timeTextBlock = entry->timeTextBlock.get();
    if (!dateTextBlock && !timeTextBlock) {
        return;
    }

    std::wstring desktopNumber = FormatDesktopNumber(g_currentDesktopNumber.load());

    std::lock_guard<std::mutex> lock(entry->mutex);

    bool useDateLine = dateTextBlock && !entry->baseDateText.empty();
    if (!useDateLine && dateTextBlock && !timeTextBlock) {
        useDateLine = true;
    }

    if (useDateLine) {
        IndicatorLayout layout =
            BuildIndicatorLayout(dateTextBlock, !entry->baseDateText.empty(),
                                 desktopNumber);
        EnsureReservedWidth(entry, dateTextBlock, entry->baseDateText);
        EnsureStableTextAlignment(entry, dateTextBlock);
        SetIndicatorTextBlockContent(dateTextBlock, entry->baseDateText, layout);
        entry->lastAppliedSuffix = layout.renderedSuffix;
    } else if (timeTextBlock) {
        IndicatorLayout layout =
            BuildIndicatorLayout(timeTextBlock, !entry->baseTimeText.empty(),
                                 desktopNumber);
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

            ApplyRomanIndicator(entry);
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

        ApplyRomanIndicator(entry);
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
    ApplyRomanIndicator(entry);
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

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR moduleName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", moduleName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
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

BOOL Wh_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_winVersion = GetExplorerVersion();
    if (g_winVersion < WinVersion::Win11) {
        Wh_Log(L"Only Windows 11 is supported");
        return FALSE;
    }

    g_currentDesktopNumber.store(ReadCurrentDesktopNumberFromRegistry());

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

    if (!StartDesktopPollThread()) {
        Wh_Log(L"Failed to start desktop poll thread");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
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

    UpdateAllClockEntries(true);
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    StopDesktopPollThread();
    RestoreAllClockEntries();
    Sleep(200);
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_clockEntriesMutex);
    g_clockEntries.clear();
}
