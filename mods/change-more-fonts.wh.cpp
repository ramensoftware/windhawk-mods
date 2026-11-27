// ==WindhawkMod==
// @id              change-more-fonts
// @name            Change More Fonts
// @description     Use an installed font for Windows 11 text in Explorer, Start, Settings, and other shell surfaces.
// @version         1.1.0
// @author          xh1le
// @github          https://github.com/xh1le
// @license         MIT
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @include         TextInputHost.exe
// @include         SystemSettings.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Change More Fonts

Pick an installed font under **Settings > Font > Font family**. The default is
Tahoma. The mod keeps the size requested by Windows. You can keep the requested
weight or set your own. Some fonts take up more room, so labels may wrap or
cut off sooner.

## Where it applies

| Area | Text the mod can change |
| --- | --- |
| File Explorer | File names, details, navigation pane, address bar, and context menus |
| Desktop | Icon labels and menus |
| Taskbar | Labels, tooltips, and menus |
| Start and Search | Menu text, app labels, and the search box |
| Quick Settings and notification center | Labels and buttons |
| Windows Settings | Page headings, row labels, and descriptions |
| Touch keyboard and input UI | Labels drawn by the Windows input host |
| Classic controls in these processes | Menus, tooltips, status bars, and dialogs |

Coverage depends on how each control draws text. DWM title bars, web content,
and text rendered as images are outside the supported paths. The mod skips icon
fonts, emoji, and some fonts used for specific scripts. With classic controls,
pick a font that has the characters you need, since GDI fallback is limited.

## Screenshots

### Windows Settings

| Mod off | Zen Maru Gothic |
| --- | --- |
| ![Windows Settings with the mod off](https://raw.githubusercontent.com/xh1le/windhawk-mods/b44a0888305a411b432af67fb948b54ea8408cc8/screenshots/change-more-fonts/settings-before.png) | ![Windows Settings with Zen Maru Gothic](https://raw.githubusercontent.com/xh1le/windhawk-mods/b44a0888305a411b432af67fb948b54ea8408cc8/screenshots/change-more-fonts/settings-after.png) |

### Explorer details view

| Mod off | Tahoma |
| --- | --- |
| ![Explorer details with the mod off](https://raw.githubusercontent.com/xh1le/windhawk-mods/b44a0888305a411b432af67fb948b54ea8408cc8/screenshots/change-more-fonts/explorer-before.png) | ![Explorer details with Tahoma](https://raw.githubusercontent.com/xh1le/windhawk-mods/b44a0888305a411b432af67fb948b54ea8408cc8/screenshots/change-more-fonts/explorer-after.png) |

## Changing more fonts

The default list covers the usual Windows UI families. Add specific families
under **Also replace these fonts** or turn on **Replace all eligible fonts**
to include document fonts such as Arial and Calibri. Turn on **Include
monospace fonts** if you want the same rules to apply to fonts such as Consolas.
The **Keep these fonts** list takes priority.

To try a different app, add its executable under this mod's **Advanced >
Process inclusion list**. The app needs to use GDI or system DirectWrite for
its text. Use the per-mod exclusion list to skip apps.

Turn on **Replace only listed fonts** to use **Also replace these fonts** as an
allowlist alongside **Font replacement rules**. This takes priority over
**Replace all eligible fonts**. **Keep these fonts** and the built-in font
protections still apply.

Add a row to **Font replacement rules** to give a source family its own
replacement. For example, set `Segoe UI` to `Arial` and `Consolas` to
`Cascadia Mono`. A rule can target a specific monospace font while the broader
**Include monospace fonts** option is off. The first rule for each source wins.
If a renderer can't find that
row's replacement, it keeps the original font. Rules use the active process's
weight settings and apply wherever the mod is active.

A rule's destination takes priority if another rule or list includes it as
a source. This keeps the replacement family when an app creates a bold or
underlined font from it.

## Use different fonts in different apps

Add a row under **Process overrides** and enter an executable name such as
`SystemSettings.exe`. Choose **Custom font settings** or **Turn off replacement**.
A blank family and weight -1 inherit the global values. Weight 0 keeps the
original weight. If a renderer can't find the override family, it uses the
global family with the override's weight setting. The first matching row
wins. An override for `explorer.exe`
also applies to the desktop and taskbar hosted by that process. To target an app
outside the built-in list, add it to Windhawk's **Advanced > Process inclusion
list** as well.

## Compatibility

Run one font replacement mod at a time. If you use Explorer Font Changer,
disable it before enabling Change More Fonts.

Taskbar, Start menu, and notification center stylers can assign fonts to
individual controls. If a theme's font should stay, add its family to
**Keep these fonts**. To use your chosen family on that control, update the
theme's font rule or add its family to **Also replace these fonts**.

Explorer Translucent Menus and Translucent Windows also hook classic text
drawing. If menu text looks wrong with either mod enabled, try **Rendering >
Keep cached GDI fonts**. Font creation and DirectWrite changes
still work with that option on.

Windows font substitutions you set separately can still affect GDI text
with Change More Fonts paused.

## Refresh the text

Restart the affected app if it still shows the old font. Signing out and back
in refreshes Start and other shell processes. Restarting also helps if text
is clipped because the app measured it with a cached font. Turning the mod
off and restarting the affected app restores its usual font.

The mod runs in 64-bit processes on Windows 11 and changes fonts in memory.
Optional apps added through the inclusion list also need to be 64-bit.

## Font didn't change?

Copy the family name from **Windows Settings > Personalization > Fonts** and
save the mod settings again after installing a font. If only some text changes,
open the mod's logs in Windhawk. Look for the requested family and the GDI and
DirectWrite status for the affected process. If the log shows the expected
family and an active renderer, reopen the affected app to refresh existing text.
Check **Process overrides** and **Keep these fonts** if the original font persists.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
  - FontFamily: "Tahoma"
    $name: Font family
    $description: Enter the family name shown in Windows Settings under Personalization > Fonts.
  - FontWeight: 0
    $name: Font weight
    $description: Use 0 to keep the requested weight, or enter a value from 100 to 900. 400 is Regular and 500 is Medium.
  - ForceBaseWeight: false
    $name: Use the chosen weight for bold text too
    $description: Applies when you set a weight. Some controls set their own weight after choosing a font.
  - DisableMod: false
    $name: Pause font changes
    $description: Apps use their usual font the next time they create or lay out text.
  $name: Font
- Rendering:
  - DisableGdi: false
    $name: Disable GDI replacement
    $description: Use the app's original fonts in classic Windows controls.
  - DisableGdiDrawing: false
    $name: Keep cached GDI fonts
    $description: Keep the font already selected when classic text is drawn. Try this if another mod changes menu text drawing.
  - DisableDirectWrite: false
    $name: Disable DirectWrite replacement
    $description: Use the app's original fonts for DirectWrite text, including most XAML controls.
  $name: Rendering
- Exclusions:
  - ReplaceAllFonts: false
    $name: Replace all eligible fonts
    $description: Includes document fonts such as Arial and Calibri in the apps this mod targets.
  - ReplaceListedOnly: false
    $name: Replace only listed fonts
    $description: Use the list below and any font replacement rules. Takes priority over Replace all eligible fonts.
  - ReplaceMonospace: false
    $name: Include monospace fonts
    $description: Include monospace fonts in list and all-font replacements. A rule can target a specific monospace family while this is off.
  - AdditionalFonts: ""
    $name: Also replace these fonts
    $description: Enter family names separated by commas.
  - ExcludedFonts: ""
    $name: Keep these fonts
    $description: Enter family names separated by commas. This list takes priority over the replacement lists.
  $name: Fonts to replace
- FontReplacementRules:
  - - Mode: replace
      $name: Rule status
      $options:
      - replace: Replace this family
      - disabled: Skip this rule
    - SourceFamily: ""
      $name: Original font family
      $description: Enter an exact family name such as Segoe UI.
    - ReplacementFamily: ""
      $name: Replacement font family
      $description: Enter an installed family such as Arial.
  $name: Font replacement rules
  $description: Set a different replacement for a specific source family. The first matching row wins.
- ProcessOverrides:
  - - Process: ""
      $name: Executable name
      $description: Enter a name such as SystemSettings.exe. The first matching row wins.
    - Mode: inherit
      $name: Font settings for this process
      $options:
      - inherit: Use global settings
      - custom: Custom font settings
      - disabled: Turn off replacement
    - FontFamily: ""
      $name: Font family
      $description: Leave blank to use the global font family.
    - FontWeight: -1
      $name: Font weight
      $description: Use -1 for the global weight, 0 to keep the original weight, or 100 to 900 to set a weight.
  $name: Process overrides
  $description: Set a different font or turn off replacement for individual executables.
*/
// ==/WindhawkModSettings==

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using Microsoft::WRL::ComPtr;

constexpr wchar_t kDefaultFamily[] = L"Tahoma";
constexpr int kGdiSampleWeights[] = {100, 200, 300, 350, 400, 500,
                                     600, 700, 800, 900, 1000};

constexpr const wchar_t* kDefaultReplaceFamilies[] = {
    // Windows 11 / Windows 10 shell families.
    L"Segoe UI",
    L"Segoe UI Variable",
    L"Segoe UI VF",
    L"Segoe UI Variable Text",
    L"Segoe UI Variable Display",
    L"Segoe UI Variable Small",
    L"Segoe UI Variable Text Semibold",
    L"Segoe UI Variable Display Semibold",
    L"Segoe UI Variable Small Semibold",
    L"Segoe UI Variable Text Light",
    L"Segoe UI Variable Display Light",
    L"Segoe UI Variable Text Semilight",
    L"Segoe UI Variable Display Semilight",
    L"Segoe UI Semibold",
    L"Segoe UI Semilight",
    L"Segoe UI Light",
    L"Segoe UI Bold",
    // Classic Win32 UI faces.
    L"Tahoma",
    L"Microsoft Sans Serif",
    L"MS Shell Dlg",
    L"MS Shell Dlg 2",
    L"MS Sans Serif",
    // Japanese UI faces (document faces are intentionally excluded).
    L"Yu Gothic UI",
    L"Meiryo UI",
    L"MS UI Gothic",
};

constexpr const wchar_t* kProtectedScriptFamilies[] = {
    // Japanese document faces.
    L"Yu Gothic",
    L"Yu Mincho",
    L"Meiryo",
    L"MS Gothic",
    L"MS Mincho",
    L"MS PGothic",
    L"MS PMincho",
    // CJK faces with distinct glyph coverage.
    L"Microsoft YaHei",
    L"Microsoft YaHei UI",
    L"Microsoft JhengHei",
    L"SimSun",
    L"NSimSun",
    L"SimHei",
    L"KaiTi",
    L"FangSong",
    L"MingLiU",
    L"PMingLiU",
    L"Malgun Gothic",
    L"Gulim",
    L"Batang",
    L"Dotum",
    L"Leelawadee",
    L"Leelawadee UI",
    // Fonts used for particular writing systems.
    L"Nirmala UI",
    L"Mangal",
    L"Kokila",
    L"Utsaah",
    L"Aparajita",
    L"Sangam MN",
    L"Gautami",
    L"Vrinda",
    L"Shonar Bangla",
    L"Bangla UI",
    L"Iskoola Pota",
    L"Kartika",
    L"Raavi",
    L"Latha",
    L"Tunga",
    L"Shruti",
    L"Kalinga",
    L"Myanmar Text",
    L"Myanmar UI",
    L"Khmer UI",
    L"Lao UI",
    L"DokChampa",
    L"Ebrima",
    L"Gadugi",
    L"Microsoft Himalaya",
    // Arabic and Hebrew faces.
    L"Arabic Typesetting",
    L"Traditional Arabic",
    L"Simplified Arabic",
    L"Sakkal Majalla",
    L"Aldhabi",
    L"Urdu Typesetting",
    L"Andalus",
    L"Arabic Transparent",
    L"David",
    L"Narkisim",
    L"FrankRuehl",
    L"Miriam",
    L"Miriam Fixed",
    L"Rod",
    L"Arial Hebrew",
    L"Arial Unicode MS",
};

struct Settings {
    std::wstring fontFamily = kDefaultFamily;
    std::wstring processFontFamily;
    int fontWeight = 0;
    bool forceBaseWeight = false;
    bool disableMod = false;
    bool disableGdi = false;
    bool disableGdiDrawing = false;
    bool disableDirectWrite = false;
    bool replaceAllFonts = false;
    bool replaceListedOnly = false;
    bool replaceMonospace = false;
    std::vector<std::wstring> additionalFamilies;
    std::vector<std::wstring> extraExcludedFamilies;
};

struct ReplacementFont {
    std::wstring family;
    std::wstring dwFamily;
    std::array<std::wstring, std::size(kGdiSampleWeights)> gdiFaces;
};

struct FontReplacementRule {
    std::wstring source;
    ReplacementFont replacement;
};

struct Runtime {
    Settings settings;
    ReplacementFont defaultFont;
    std::vector<FontReplacementRule> fontRules;
    bool dwValid = false;
    bool gdiValid = false;
    std::vector<std::wstring> replaceFamilies;
    std::vector<std::wstring> excludedFamilies;
};

std::shared_ptr<const Runtime> g_runtime;
SRWLOCK g_runtimeLock = SRWLOCK_INIT;
std::atomic<bool> g_runtimeAvailable{false};
thread_local bool t_internal = false;

// Declared early because GDI face verification uses it before the hook
// section below.
using CreateFontIndirectW_t = decltype(&CreateFontIndirectW);
CreateFontIndirectW_t g_origCreateFontIndirectW = nullptr;

struct InternalScope {
    bool previous;
    InternalScope() : previous(t_internal) { t_internal = true; }
    ~InternalScope() { t_internal = previous; }
    InternalScope(const InternalScope&) = delete;
    InternalScope& operator=(const InternalScope&) = delete;
};

std::shared_ptr<const Runtime> GetRuntimeSnapshot() {
    if (!g_runtimeAvailable.load(std::memory_order_acquire)) {
        return nullptr;
    }
    AcquireSRWLockShared(&g_runtimeLock);
    auto runtime = g_runtime;
    ReleaseSRWLockShared(&g_runtimeLock);
    return runtime;
}

void PublishRuntime(std::shared_ptr<const Runtime> runtime) noexcept {
    AcquireSRWLockExclusive(&g_runtimeLock);
    g_runtime.swap(runtime);
    g_runtimeAvailable.store(g_runtime != nullptr, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_runtimeLock);
}

// ---------------------------------------------------------------------------
// String helpers
// ---------------------------------------------------------------------------

std::wstring ReadStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(L"%s", name).get();
}

std::wstring ReadIndexedStringSetting(PCWSTR format, int index) {
    return WindhawkUtils::StringSetting::make(format, index).get();
}

std::wstring Trim(std::wstring value) {
    size_t begin = 0;
    while (begin < value.size() && iswspace(value[begin])) {
        begin++;
    }
    size_t end = value.size();
    while (end > begin && iswspace(value[end - 1])) {
        end--;
    }
    return value.substr(begin, end - begin);
}

std::wstring ToLower(std::wstring value) {
    for (auto& ch : value) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return value;
}

bool EqualsCI(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }
    return true;
}

bool StartsWithCI(std::wstring_view text, std::wstring_view prefix) {
    if (text.size() < prefix.size()) {
        return false;
    }
    return EqualsCI(text.substr(0, prefix.size()), prefix);
}

bool ContainsCI(std::wstring_view text, std::wstring_view needle) {
    if (needle.empty() || text.size() < needle.size()) {
        return false;
    }
    for (size_t i = 0; i + needle.size() <= text.size(); i++) {
        if (EqualsCI(text.substr(i, needle.size()), needle)) {
            return true;
        }
    }
    return false;
}

std::vector<std::wstring> SplitList(std::wstring_view text) {
    std::vector<std::wstring> result;
    std::wstring current;
    for (wchar_t ch : text) {
        if (ch == L',' || ch == L';' || ch == L'\r' || ch == L'\n') {
            auto item = Trim(current);
            if (!item.empty()) {
                result.push_back(std::move(item));
            }
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    auto item = Trim(current);
    if (!item.empty()) {
        result.push_back(std::move(item));
    }
    return result;
}

void AddUnique(std::vector<std::wstring>& list, std::wstring value) {
    value = ToLower(Trim(std::move(value)));
    if (value.empty()) {
        return;
    }
    if (std::find(list.begin(), list.end(), value) == list.end()) {
        list.push_back(std::move(value));
    }
}

std::wstring GetProcessPath() {
    std::vector<wchar_t> path(32768);
    DWORD length = GetModuleFileNameW(nullptr, path.data(),
                                      static_cast<DWORD>(path.size()));
    if (!length || length >= path.size()) {
        return {};
    }
    return std::wstring(path.data(), length);
}

std::wstring GetProcessBaseName() {
    auto path = GetProcessPath();
    size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        path = path.substr(slash + 1);
    }
    return ToLower(path);
}

// ---------------------------------------------------------------------------
// Font classification
// ---------------------------------------------------------------------------

bool IsMonospaceName(std::wstring_view lowerName) {
    static constexpr const wchar_t* kExact[] = {
        L"consolas",
        L"courier",
        L"courier new",
        L"lucida console",
        L"lucida sans typewriter",
        L"cascadia mono",
        L"cascadia code",
        L"fira code",
        L"fira mono",
        L"source code pro",
        L"inconsolata",
        L"anonymous pro",
        L"hack",
        L"iosevka",
        L"cousine",
        L"terminal",
        L"fixedsys",
        L"ms gothic",
        L"ms mincho",
        L"nsimsun",
        L"simsun",
        L"mingliu",
        L"pmingliu",
        L"batang",
        L"gulim",
        L"dotum",
    };
    for (auto exact : kExact) {
        if (EqualsCI(lowerName, exact)) {
            return true;
        }
    }
    // "Monotype" is a vendor prefix, not a monospace marker. Keep the common
    // proportional Monotype faces out of the false-positive set before the
    // broad "mono" substring check below.
    if (ContainsCI(lowerName, L"monotype") &&
        !ContainsCI(lowerName, L"mono ") && !ContainsCI(lowerName, L" mono")) {
        return false;
    }
    static constexpr const wchar_t* kParts[] = {
        L"mono", L"console", L"courier", L"fixed", L"terminal",
    };
    for (auto part : kParts) {
        if (ContainsCI(lowerName, part)) {
            return true;
        }
    }
    return false;
}

bool IsSymbolName(std::wstring_view lowerName) {
    static constexpr const wchar_t* kExact[] = {
        L"symbol",
        L"marlett",
        L"wingdings",
        L"wingdings 2",
        L"wingdings 3",
        L"webdings",
        L"segoe ui symbol",
        L"segoe ui emoji",
        L"segoe ui historic",
        L"segoe fluent icons",
        L"segoe mdl2 assets",
        L"cambria math",
    };
    for (auto exact : kExact) {
        if (EqualsCI(lowerName, exact)) {
            return true;
        }
    }
    static constexpr const wchar_t* kParts[] = {
        L"symbol",  L"emoji",     L"mdl2",     L"fluent icons",
        L"marlett", L"wingdings", L"webdings",
    };
    for (auto part : kParts) {
        if (ContainsCI(lowerName, part)) {
            return true;
        }
    }
    return false;
}

bool IsProtectedScriptName(std::wstring_view lowerName) {
    for (auto exact : kProtectedScriptFamilies) {
        if (EqualsCI(lowerName, exact)) {
            return true;
        }
    }
    static constexpr const wchar_t* kPrefixes[] = {
        L"microsoft yahei", L"microsoft jhenghei", L"malgun gothic",
        L"noto sans cjk",   L"noto serif cjk",     L"source han",
        L"dengxian",
    };
    for (auto prefix : kPrefixes) {
        if (StartsWithCI(lowerName, prefix)) {
            return true;
        }
    }
    return false;
}

bool IsUnsupportedCharset(BYTE charset) {
    switch (charset) {
        case SYMBOL_CHARSET:
        case OEM_CHARSET:
        case MAC_CHARSET:
        case THAI_CHARSET:
        case HEBREW_CHARSET:
        case ARABIC_CHARSET:
        case VIETNAMESE_CHARSET:
        case GB2312_CHARSET:
        case CHINESEBIG5_CHARSET:
        case HANGEUL_CHARSET:
        case JOHAB_CHARSET:
            return true;
        default:
            return false;
    }
}

bool IsExcludedFamily(const std::wstring& lowerName, const Runtime& rt) {
    for (const auto& excluded : rt.excludedFamilies) {
        if (EqualsCI(lowerName, excluded)) {
            return true;
        }
    }
    return false;
}

bool IsProtectedFamily(const std::wstring& lowerName, BYTE charset,
                       bool fixedPitch, const Runtime& rt, bool explicitRule) {
    return lowerName.empty() || lowerName[0] == L'@' ||
           IsUnsupportedCharset(charset) ||
           (!explicitRule && !rt.settings.replaceMonospace &&
            (fixedPitch || IsMonospaceName(lowerName))) ||
           IsSymbolName(lowerName) || IsProtectedScriptName(lowerName) ||
           IsExcludedFamily(lowerName, rt);
}

bool IsTargetFamily(std::wstring_view name, const ReplacementFont& font) {
    return !name.empty() &&
           (EqualsCI(name, font.family) ||
            EqualsCI(name, font.dwFamily) ||
            std::any_of(font.gdiFaces.begin(), font.gdiFaces.end(),
                        [name](const auto& face) { return EqualsCI(name, face); }));
}

const ReplacementFont* FindRuleReplacement(std::wstring_view family,
                                           const Runtime& rt) {
    for (const auto& rule : rt.fontRules) {
        if (EqualsCI(family, rule.source)) {
            return &rule.replacement;
        }
    }
    return nullptr;
}

bool IsListedSourceFamily(std::wstring_view family, const Runtime& rt) {
    return std::any_of(rt.replaceFamilies.begin(), rt.replaceFamilies.end(),
                       [family](const auto& name) { return EqualsCI(family, name); });
}

const ReplacementFont* SelectReplacement(std::wstring_view family,
                                         const Runtime& rt, bool latePass) {
    // Apps can create another font from a LOGFONT that already has the
    // replacement face. Keep rule destinations on creation and draw paths.
    for (const auto& rule : rt.fontRules) {
        if (IsTargetFamily(family, rule.replacement)) {
            return nullptr;
        }
    }
    bool isDefaultFont = IsTargetFamily(family, rt.defaultFont);
    if (const auto* ruleFont = FindRuleReplacement(family, rt)) {
        return latePass && isDefaultFont ? nullptr : ruleFont;
    }
    if (IsListedSourceFamily(family, rt) ||
        (!rt.settings.replaceListedOnly && (rt.settings.replaceAllFonts || isDefaultFont))) {
        return &rt.defaultFont;
    }
    return nullptr;
}

const std::wstring& GdiFaceForWeight(const ReplacementFont& font, int weight) {
    weight = weight == FW_DONTCARE ? FW_NORMAL : std::clamp(weight, 100, 1000);
    auto begin = std::begin(kGdiSampleWeights);
    auto above = std::lower_bound(begin, std::end(kGdiSampleWeights), weight);
    if (above != begin && weight - above[-1] < *above - weight) {
        above--;
    }
    return font.gdiFaces[above - begin];
}

struct FamilySuffix {
    std::wstring_view text;
    int weight;
};

const FamilySuffix* FindFamilySuffix(std::wstring_view family) {
    static constexpr FamilySuffix suffixes[] = {
        {L" extra light", 200}, {L" extralight", 200},
        {L" ultra light", 200}, {L" ultralight", 200},
        {L" semi light", 350},  {L" semilight", 350},
        {L" semi bold", 600},   {L" semibold", 600},
        {L" demi bold", 600},   {L" demibold", 600},
        {L" extra bold", 800},  {L" extrabold", 800},
        {L" ultra bold", 800},  {L" ultrabold", 800},
        {L" extra black", 950}, {L" extrablack", 950},
        {L" thin", 100},        {L" light", 300},
        {L" regular", 400},     {L" normal", 400},
        {L" book", 400},        {L" medium", 500},
        {L" bold", 700},        {L" black", 900},
        {L" heavy", 900},       {L" italic", 0},
        {L" oblique", 0},
    };
    for (const auto& suffix : suffixes) {
        if (family.size() > suffix.text.size() && family.ends_with(suffix.text)) {
            return &suffix;
        }
    }
    return nullptr;
}

int NamedFontWeight(std::wstring_view lowerFamily) {
    const auto* suffix = FindFamilySuffix(lowerFamily);
    return suffix ? suffix->weight : 0;
}

int SourceWeight(int namedWeight, int requestedWeight) {
    if (namedWeight && (requestedWeight == FW_DONTCARE || requestedWeight == FW_NORMAL)) {
        return namedWeight;
    }
    return requestedWeight;
}

int ChooseGdiWeight(int originalWeight, const Runtime& rt) {
    if (rt.settings.fontWeight == 0 ||
        (originalWeight >= 600 && !rt.settings.forceBaseWeight)) {
        return originalWeight;
    }
    return rt.settings.fontWeight;
}

DWRITE_FONT_WEIGHT ChooseDWriteWeight(DWRITE_FONT_WEIGHT originalWeight,
                                      int namedWeight,
                                      const Runtime& rt) {
    return static_cast<DWRITE_FONT_WEIGHT>(
        ChooseGdiWeight(SourceWeight(namedWeight, originalWeight), rt));
}

// ---------------------------------------------------------------------------
// Logging
// ---------------------------------------------------------------------------

void LogInitInfo(const Runtime& rt, const std::wstring& processName) {
    Wh_Log(L"Change More Fonts in %s", processName.c_str());
    Wh_Log(L"Requested family='%s' weight=%d forceBaseWeight=%d",
           rt.defaultFont.family.c_str(), rt.settings.fontWeight,
           rt.settings.forceBaseWeight ? 1 : 0);
    Wh_Log(L"DirectWrite family='%s' active=%d", rt.defaultFont.dwFamily.c_str(),
           rt.dwValid && !rt.settings.disableMod && !rt.settings.disableDirectWrite);
    Wh_Log(L"GDI regular face='%s' bold face='%s' active=%d",
           GdiFaceForWeight(rt.defaultFont, FW_NORMAL).c_str(),
           GdiFaceForWeight(rt.defaultFont, FW_BOLD).c_str(),
           rt.gdiValid && !rt.settings.disableMod && !rt.settings.disableGdi);
    for (const auto& rule : rt.fontRules) {
        Wh_Log(L"Rule '%s' -> '%s' (GDI='%s', DirectWrite='%s')",
               rule.source.c_str(), rule.replacement.family.c_str(),
               GdiFaceForWeight(rule.replacement, FW_NORMAL).c_str(),
               rule.replacement.dwFamily.c_str());
    }
    Wh_Log(
        L"Known limits: DWM title bars, WebView2 content and layout ranges "
        L"with custom font collections lacking the target family.");
}

// ---------------------------------------------------------------------------
// GDI face resolution
// ---------------------------------------------------------------------------

struct GdiCandidate {
    std::wstring name;
    std::wstring base;
    bool italic;
};

struct GdiFontCatalog {
    HDC dc;
    std::unordered_map<std::wstring, GdiCandidate> candidates;
    bool failed = false;

    explicit GdiFontCatalog(bool enabled)
        : dc(enabled ? CreateCompatibleDC(nullptr) : nullptr) {}
    ~GdiFontCatalog() {
        if (dc) {
            DeleteDC(dc);
        }
    }
    GdiFontCatalog(const GdiFontCatalog&) = delete;
    GdiFontCatalog& operator=(const GdiFontCatalog&) = delete;
};

int CALLBACK GdiEnumProc(const LOGFONTW* logFont,
                         const TEXTMETRICW*, DWORD, LPARAM lParam) {
    auto& catalog = *reinterpret_cast<GdiFontCatalog*>(lParam);
    try {
        std::wstring name(logFont->lfFaceName,
                          wcsnlen(logFont->lfFaceName, LF_FACESIZE));
        if (name.empty() || name[0] == L'@') {
            return 1;
        }
        auto lower = ToLower(name);
        auto [entry, inserted] = catalog.candidates.try_emplace(lower);
        if (inserted) {
            auto* suffix = FindFamilySuffix(lower);
            entry->second = {std::move(name),
                             suffix ? lower.substr(0, lower.size() - suffix->text.size())
                                    : std::wstring(),
                             logFont->lfItalic != 0};
        } else if (!logFont->lfItalic) {
            entry->second.italic = false;
        }
        return 1;
    } catch (...) {
        catalog.failed = true;
        return 0;
    }
}

int GetRealizedGdiWeight(HDC dc, const std::wstring& face, int weight) {
    LOGFONTW logFont = {};
    logFont.lfHeight = -16;
    logFont.lfWeight = weight;
    logFont.lfCharSet = DEFAULT_CHARSET;
    wcsncpy_s(logFont.lfFaceName, face.c_str(), _TRUNCATE);

    HFONT font = nullptr;
    if (g_origCreateFontIndirectW) {
        font = g_origCreateFontIndirectW(&logFont);
    } else {
        font = CreateFontIndirectW(&logFont);
    }
    if (!font) {
        return -1;
    }

    int realizedWeight = -1;
    HGDIOBJ old = SelectObject(dc, font);
    if (old && old != HGDI_ERROR) {
        wchar_t selected[LF_FACESIZE] = {};
        TEXTMETRICW metrics{};
        if (GetTextFaceW(dc, LF_FACESIZE, selected) > 0 &&
            EqualsCI(selected, face) && GetTextMetricsW(dc, &metrics)) {
            realizedWeight = metrics.tmWeight;
        }
        SelectObject(dc, old);
    }
    DeleteObject(font);
    return realizedWeight;
}

std::wstring ChooseGdiFace(HDC dc, const std::vector<const GdiCandidate*>& candidates,
                           int targetWeight, const std::wstring& family) {
    int weight = GetRealizedGdiWeight(dc, family, targetWeight);
    std::wstring best = weight >= 0 ? family : L"";
    int bestDistance = weight >= 0 ? std::abs(weight - targetWeight) : INT_MAX;
    for (const auto& candidate : candidates) {
        if (!bestDistance) {
            break;
        }
        if (candidate->italic || EqualsCI(candidate->name, family)) {
            continue;
        }
        weight = GetRealizedGdiWeight(dc, candidate->name, targetWeight);
        if (weight >= 0 && std::abs(weight - targetWeight) < bestDistance) {
            best = candidate->name;
            bestDistance = std::abs(weight - targetWeight);
        }
    }
    return best;
}

bool ResolveGdiFaces(ReplacementFont& font, const GdiFontCatalog& catalog) {
    if (font.family.size() >= LF_FACESIZE) {
        Wh_Log(L"Font family '%s' is too long for GDI (maximum %d characters)",
               font.family.c_str(), LF_FACESIZE - 1);
        return false;
    }
    if (!catalog.dc || catalog.failed) {
        return false;
    }

    auto lower = ToLower(font.family);
    std::vector<const GdiCandidate*> candidates;
    for (const auto& [name, candidate] : catalog.candidates) {
        if (name == lower || candidate.base == lower) {
            candidates.push_back(&candidate);
        }
    }
    if (candidates.empty()) {
        Wh_Log(L"No GDI face matching '%s' was found", font.family.c_str());
        return false;
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const auto* a, const auto* b) { return a->name < b->name; });

    auto regular = ChooseGdiFace(catalog.dc, candidates, FW_NORMAL, font.family);
    if (regular.empty()) {
        Wh_Log(L"Could not verify a GDI face for '%s'", font.family.c_str());
        return false;
    }

    for (size_t i = 0; i < font.gdiFaces.size(); i++) {
        int weight = kGdiSampleWeights[i];
        auto face = weight == FW_NORMAL
                        ? regular
                        : ChooseGdiFace(catalog.dc, candidates, weight, font.family);
        font.gdiFaces[i] = face.empty() ? regular : std::move(face);
    }
    return true;
}

// ---------------------------------------------------------------------------
// DirectWrite helpers
// ---------------------------------------------------------------------------

IDWriteFactory* g_dwFactory = nullptr;
HMODULE g_dwModule = nullptr;
bool g_gdiHooksInstalled = false;
bool g_dwHooksInstalled = false;

std::wstring GetLocalizedFamilyName(IDWriteLocalizedStrings* names) {
    if (!names) {
        return {};
    }
    UINT32 index = 0;
    BOOL exists = FALSE;
    names->FindLocaleName(L"en-us", &index, &exists);
    if (!exists) {
        index = 0;
    }
    UINT32 length = 0;
    if (FAILED(names->GetStringLength(index, &length)) || length == 0 ||
        length > 256) {
        return {};
    }
    std::wstring result(length + 1, L'\0');
    if (FAILED(names->GetString(index, result.data(), length + 1))) {
        return {};
    }
    result.resize(length);
    return result;
}

std::wstring GetFamilyName(IDWriteFontFamily* family) {
    ComPtr<IDWriteLocalizedStrings> names;
    if (!family || FAILED(family->GetFamilyNames(names.GetAddressOf()))) {
        return {};
    }
    return GetLocalizedFamilyName(names.Get());
}

bool FindDWriteFamilyExact(IDWriteFontCollection* collection,
                           const std::wstring& name,
                           std::wstring* result) {
    if (!collection || name.empty()) {
        return false;
    }
    UINT32 index = 0;
    BOOL exists = FALSE;
    if (FAILED(collection->FindFamilyName(name.c_str(), &index, &exists)) ||
        !exists) {
        return false;
    }
    IDWriteFontFamily* family = nullptr;
    if (FAILED(collection->GetFontFamily(index, &family)) || !family) {
        return false;
    }
    auto actual = GetFamilyName(family);
    family->Release();
    if (actual.empty()) {
        actual = name;
    }
    *result = std::move(actual);
    return true;
}

std::wstring StripWeightSuffix(const std::wstring& name) {
    auto lower = ToLower(name);
    auto* suffix = FindFamilySuffix(lower);
    return suffix ? Trim(name.substr(0, name.size() - suffix->text.size())) : name;
}

bool ResolveDWriteFamily(ReplacementFont& target, IDWriteFontCollection* collection,
                         int fontWeight) {
    if (!collection) {
        return false;
    }

    std::wstring resolved;
    bool found =
        FindDWriteFamilyExact(collection, target.family, &resolved);

    if (!found) {
        auto stripped = StripWeightSuffix(target.family);
        if (!EqualsCI(stripped, target.family)) {
            found = FindDWriteFamilyExact(collection, stripped, &resolved);
        }
    }

    if (!found) {
        Wh_Log(L"DirectWrite family '%s' was not found", target.family.c_str());
        return false;
    }

    int baseWeight = fontWeight ? fontWeight : FW_NORMAL;
    IDWriteTextFormat* testFormat = nullptr;
    HRESULT hr = g_dwFactory->CreateTextFormat(
        resolved.c_str(), nullptr, static_cast<DWRITE_FONT_WEIGHT>(baseWeight),
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-us",
        &testFormat);
    if (FAILED(hr) || !testFormat) {
        hr = g_dwFactory->CreateTextFormat(
            resolved.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f,
            L"en-us", &testFormat);
    }
    if (FAILED(hr) || !testFormat) {
        Wh_Log(L"DirectWrite CreateTextFormat failed for '%s' (hr=0x%08X)",
               resolved.c_str(), static_cast<unsigned>(hr));
        return false;
    }
    testFormat->Release();

    UINT32 familyIndex = 0;
    BOOL exists = FALSE;
    if (SUCCEEDED(collection->FindFamilyName(resolved.c_str(), &familyIndex,
                                             &exists)) &&
        exists) {
        IDWriteFontFamily* family = nullptr;
        if (SUCCEEDED(collection->GetFontFamily(familyIndex, &family)) &&
            family) {
            std::vector<int> weights;
            UINT32 fontCount = family->GetFontCount();
            for (UINT32 i = 0; i < fontCount && i < 32; i++) {
                IDWriteFont* font = nullptr;
                if (SUCCEEDED(family->GetFont(i, &font)) && font) {
                    int weight = static_cast<int>(font->GetWeight());
                    if (std::find(weights.begin(), weights.end(), weight) ==
                        weights.end()) {
                        weights.push_back(weight);
                    }
                    font->Release();
                }
            }
            family->Release();
            std::wstring weightList;
            for (int weight : weights) {
                if (!weightList.empty()) {
                    weightList += L",";
                }
                weightList += std::to_wstring(weight);
            }
            Wh_Log(L"DirectWrite family '%s' weights=[%s]", resolved.c_str(),
                   weightList.c_str());
        }
    }

    target.dwFamily = std::move(resolved);
    return true;
}

// ---------------------------------------------------------------------------
// Settings and runtime construction
// ---------------------------------------------------------------------------

std::wstring FirstFontFamily(std::wstring_view value) {
    auto parts = SplitList(value);
    // If a fallback list was pasted, use its first family.
    return parts.empty() ? L"" : std::move(parts.front());
}

int NormalizeFontWeight(int weight) {
    return weight == 0 ? 0 : std::clamp(weight, 100, 900);
}

void ApplyProcessOverride(Settings& settings, std::wstring_view processName) {
    for (int i = 0; !processName.empty(); i++) {
        auto mode = ReadIndexedStringSetting(L"ProcessOverrides[%d].Mode", i);
        if (mode.empty()) {
            break;
        }
        auto process = Trim(ReadIndexedStringSetting(L"ProcessOverrides[%d].Process", i));
        if (!EqualsCI(process, processName)) {
            continue;
        }
        if (mode == L"disabled") {
            settings.disableMod = true;
        } else if (mode == L"custom") {
            settings.processFontFamily = FirstFontFamily(
                ReadIndexedStringSetting(L"ProcessOverrides[%d].FontFamily", i));
            int weight = Wh_GetIntSetting(L"ProcessOverrides[%d].FontWeight", i);
            if (weight >= 0) {
                settings.fontWeight = NormalizeFontWeight(weight);
            }
        }
        break;
    }
}

Settings LoadSettings(std::wstring_view processName) {
    Settings settings;
    auto family = FirstFontFamily(ReadStringSetting(L"General.FontFamily"));
    if (!family.empty()) {
        settings.fontFamily = std::move(family);
    }
    settings.fontWeight = NormalizeFontWeight(Wh_GetIntSetting(L"General.FontWeight"));
    settings.forceBaseWeight =
        Wh_GetIntSetting(L"General.ForceBaseWeight") != 0;
    settings.disableMod = Wh_GetIntSetting(L"General.DisableMod") != 0;

    settings.disableGdi = Wh_GetIntSetting(L"Rendering.DisableGdi") != 0;
    settings.disableGdiDrawing =
        Wh_GetIntSetting(L"Rendering.DisableGdiDrawing") != 0;
    settings.disableDirectWrite =
        Wh_GetIntSetting(L"Rendering.DisableDirectWrite") != 0;

    settings.replaceAllFonts =
        Wh_GetIntSetting(L"Exclusions.ReplaceAllFonts") != 0;
    settings.replaceListedOnly =
        Wh_GetIntSetting(L"Exclusions.ReplaceListedOnly") != 0;
    settings.replaceMonospace =
        Wh_GetIntSetting(L"Exclusions.ReplaceMonospace") != 0;

    settings.additionalFamilies =
        SplitList(ReadStringSetting(L"Exclusions.AdditionalFonts"));
    settings.extraExcludedFamilies =
        SplitList(ReadStringSetting(L"Exclusions.ExcludedFonts"));

    ApplyProcessOverride(settings, processName);
    return settings;
}

std::shared_ptr<Runtime> BuildRuntime(Settings settings, bool refreshFonts) {
    InternalScope internalScope;

    auto runtime = std::make_shared<Runtime>();
    runtime->settings = std::move(settings);

    if (!runtime->settings.replaceListedOnly) {
        for (auto family : kDefaultReplaceFamilies) {
            AddUnique(runtime->replaceFamilies, family);
        }
    }
    for (auto& family : runtime->settings.additionalFamilies) {
        AddUnique(runtime->replaceFamilies, family);
    }
    for (auto& family : runtime->settings.extraExcludedFamilies) {
        AddUnique(runtime->excludedFamilies, family);
    }

    ComPtr<IDWriteFontCollection> collection;
    if (!runtime->settings.disableDirectWrite && g_dwFactory &&
        FAILED(g_dwFactory->GetSystemFontCollection(
                           collection.GetAddressOf(), refreshFonts))) {
        Wh_Log(L"Could not read the DirectWrite system font collection");
    }
    GdiFontCatalog catalog(!runtime->settings.disableGdi);
    if (catalog.dc) {
        LOGFONTW filter{};
        filter.lfCharSet = DEFAULT_CHARSET;
        int result = EnumFontFamiliesExW(catalog.dc, &filter, GdiEnumProc,
                                         reinterpret_cast<LPARAM>(&catalog), 0);
        if (!result || catalog.failed) {
            catalog.failed = true;
            Wh_Log(L"GDI font enumeration did not complete");
        }
    } else if (!runtime->settings.disableGdi) {
        Wh_Log(L"Could not create a device context for GDI font resolution");
    }
    std::unordered_map<std::wstring, ReplacementFont> resolvedFonts;
    auto resolveFont = [&](const std::wstring& family) -> const ReplacementFont& {
        auto [entry, inserted] = resolvedFonts.try_emplace(ToLower(family));
        auto& font = entry->second;
        if (inserted) {
            font.family = family;
            if (!runtime->settings.disableGdi) {
                runtime->gdiValid |= ResolveGdiFaces(font, catalog);
            }
            runtime->dwValid |= ResolveDWriteFamily(font, collection.Get(),
                                                   runtime->settings.fontWeight);
        }
        return font;
    };
    auto& defaultFont = runtime->defaultFont;
    const auto& globalFamily = runtime->settings.fontFamily;
    const auto& processFamily = runtime->settings.processFontFamily;
    defaultFont = resolveFont(processFamily.empty() ? globalFamily : processFamily);
    if (!processFamily.empty() && !EqualsCI(processFamily, globalFamily)) {
        bool needGdi = !runtime->settings.disableGdi &&
                       GdiFaceForWeight(defaultFont, FW_NORMAL).empty();
        bool needDw = !runtime->settings.disableDirectWrite && defaultFont.dwFamily.empty();
        const auto* fallback = needGdi || needDw ? &resolveFont(globalFamily) : nullptr;
        if (needGdi && !GdiFaceForWeight(*fallback, FW_NORMAL).empty()) {
            defaultFont.gdiFaces = fallback->gdiFaces;
            Wh_Log(L"GDI uses global family '%s' because process family '%s' is unavailable",
                   globalFamily.c_str(), processFamily.c_str());
        }
        if (needDw && !fallback->dwFamily.empty()) {
            defaultFont.dwFamily = fallback->dwFamily;
            Wh_Log(L"DirectWrite uses global family '%s' because process family '%s' is unavailable",
                   globalFamily.c_str(), processFamily.c_str());
        }
    }

    for (int i = 0;; i++) {
        auto mode = ReadIndexedStringSetting(L"FontReplacementRules[%d].Mode", i);
        if (mode.empty()) {
            break;
        }
        if (mode != L"replace") {
            continue;
        }
        auto source = Trim(ReadIndexedStringSetting(L"FontReplacementRules[%d].SourceFamily", i));
        auto family = FirstFontFamily(
            ReadIndexedStringSetting(L"FontReplacementRules[%d].ReplacementFamily", i));
        if (source.empty() || family.empty() || FindRuleReplacement(source, *runtime)) {
            continue;
        }
        if (IsProtectedFamily(ToLower(source), DEFAULT_CHARSET, false, *runtime, true)) {
            Wh_Log(L"Skipped font rule for protected or excluded source '%s'", source.c_str());
            continue;
        }
        runtime->fontRules.push_back({std::move(source), resolveFont(family)});
    }
    return runtime;
}

// ---------------------------------------------------------------------------
// GDI hooks
// ---------------------------------------------------------------------------

using CreateFontIndirectA_t = decltype(&CreateFontIndirectA);
using CreateFontIndirectExW_t = decltype(&CreateFontIndirectExW);
using CreateFontIndirectExA_t = decltype(&CreateFontIndirectExA);
using CreateFontW_t = decltype(&CreateFontW);
using CreateFontA_t = decltype(&CreateFontA);
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);

CreateFontIndirectA_t g_origCreateFontIndirectA = nullptr;
CreateFontIndirectExW_t g_origCreateFontIndirectExW = nullptr;
CreateFontIndirectExA_t g_origCreateFontIndirectExA = nullptr;
CreateFontW_t g_origCreateFontW = nullptr;
CreateFontA_t g_origCreateFontA = nullptr;
DrawTextW_t g_origDrawTextW = nullptr;
DrawTextExW_t g_origDrawTextExW = nullptr;

void ApplyGdiFaceDecision(const Runtime& rt,
                          const std::wstring& face,
                          BYTE charset,
                          bool fixedPitch,
                          int& weight,
                          std::wstring& newFace,
                          bool& changed,
                          bool latePass = false) {
    changed = false;
    std::wstring lower = ToLower(Trim(face));
    const auto* font = SelectReplacement(lower, rt, latePass);
    if (!font || IsProtectedFamily(lower, charset, fixedPitch, rt,
                                   font != &rt.defaultFont)) {
        return;
    }

    // Listed sources can also be faces of the chosen replacement, as with
    // Segoe UI Semibold when the user has selected the classic Segoe UI family.
    int namedWeight = !IsTargetFamily(lower, *font) || IsListedSourceFamily(lower, rt)
                          ? NamedFontWeight(lower)
                          : 0;
    int newWeight = ChooseGdiWeight(SourceWeight(namedWeight, weight), rt);
    const auto& replacement = GdiFaceForWeight(*font, newWeight);
    if (replacement.empty()) {
        return;
    }
    if (!EqualsCI(replacement, face)) {
        newFace = replacement;
        changed = true;
    }
    if (weight != newWeight) {
        weight = newWeight;
        changed = true;
    }
}

bool ApplyToLogFontW(const Runtime& rt, LOGFONTW* logFont, bool latePass = false) {
    if (!logFont) {
        return false;
    }
    std::wstring face(logFont->lfFaceName,
                      wcsnlen(logFont->lfFaceName, LF_FACESIZE));
    int weight = logFont->lfWeight;
    std::wstring newFace;
    bool changed = false;
    ApplyGdiFaceDecision(rt, face, logFont->lfCharSet,
                         (logFont->lfPitchAndFamily & FIXED_PITCH) != 0, weight,
                         newFace, changed, latePass);
    if (!changed) {
        return false;
    }
    if (!newFace.empty()) {
        wcsncpy_s(logFont->lfFaceName, newFace.c_str(), _TRUNCATE);
    }
    logFont->lfWeight = weight;
    return true;
}

// Explorer can paint with an HFONT cached before the mod was loaded or
// created inside a system component that bypassed the public creation APIs.
std::shared_ptr<const Runtime> GetGdiDrawRuntime() {
    if (t_internal) {
        return nullptr;
    }
    auto rt = GetRuntimeSnapshot();
    if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
        rt->settings.disableGdiDrawing || !rt->gdiValid) {
        return nullptr;
    }
    return rt;
}

class ScopedDrawFont {
   public:
    ScopedDrawFont(HDC dc, const Runtime& rt) : dc_(dc) {
        if (GetDeviceCaps(dc, TECHNOLOGY) != DT_RASDISPLAY) {
            return;
        }

        LOGFONTW logFont = {};
        HGDIOBJ selected = GetCurrentObject(dc, OBJ_FONT);
        if (GetObjectW(selected, sizeof(logFont), &logFont) != sizeof(logFont)) {
            return;
        }
        try {
            if (!ApplyToLogFontW(rt, &logFont, true)) {
                return;
            }
        } catch (...) {
            return;
        }
        font_ = CreateFontIndirectW(&logFont);
        if (font_) {
            original_ = SelectObject(dc, font_);
        }
    }

    ~ScopedDrawFont() {
        if (original_ && original_ != HGDI_ERROR) {
            SelectObject(dc_, original_);
        }
        if (font_) {
            DeleteObject(font_);
        }
    }

    ScopedDrawFont(const ScopedDrawFont&) = delete;
    ScopedDrawFont& operator=(const ScopedDrawFont&) = delete;

   private:
    HDC dc_;
    HFONT font_ = nullptr;
    HGDIOBJ original_ = nullptr;
};

int WINAPI DrawTextW_Hook(HDC dc, LPCWSTR text, int count, LPRECT rect,
                         UINT format) {
    auto rt = GetGdiDrawRuntime();
    if (!rt) {
        return g_origDrawTextW(dc, text, count, rect, format);
    }
    InternalScope scope;
    ScopedDrawFont drawFont(dc, *rt);
    return g_origDrawTextW(dc, text, count, rect, format);
}

int WINAPI DrawTextExW_Hook(HDC dc, LPWSTR text, int count, LPRECT rect,
                           UINT format, LPDRAWTEXTPARAMS params) {
    auto rt = GetGdiDrawRuntime();
    if (!rt) {
        return g_origDrawTextExW(dc, text, count, rect, format, params);
    }
    InternalScope scope;
    ScopedDrawFont drawFont(dc, *rt);
    return g_origDrawTextExW(dc, text, count, rect, format, params);
}

bool ConvertGdiFaceToAnsi(PCWSTR face, char (&result)[LF_FACESIZE]) {
    wchar_t roundTrip[LF_FACESIZE] = {};
    return WideCharToMultiByte(CP_ACP, 0, face, -1, result, LF_FACESIZE,
                               nullptr, nullptr) &&
           MultiByteToWideChar(CP_ACP, 0, result, -1, roundTrip, LF_FACESIZE) &&
           wcscmp(roundTrip, face) == 0;
}

void ApplyToLogFontA(const Runtime& rt, LOGFONTA* logFont) {
    if (!logFont || strnlen(logFont->lfFaceName, LF_FACESIZE) == LF_FACESIZE) {
        return;
    }
    LOGFONTW wide = {};
    wide.lfHeight = logFont->lfHeight;
    wide.lfWidth = logFont->lfWidth;
    wide.lfEscapement = logFont->lfEscapement;
    wide.lfOrientation = logFont->lfOrientation;
    wide.lfWeight = logFont->lfWeight;
    wide.lfItalic = logFont->lfItalic;
    wide.lfUnderline = logFont->lfUnderline;
    wide.lfStrikeOut = logFont->lfStrikeOut;
    wide.lfCharSet = logFont->lfCharSet;
    wide.lfOutPrecision = logFont->lfOutPrecision;
    wide.lfClipPrecision = logFont->lfClipPrecision;
    wide.lfQuality = logFont->lfQuality;
    wide.lfPitchAndFamily = logFont->lfPitchAndFamily;
    wchar_t originalWide[LF_FACESIZE] = {};
    if (!MultiByteToWideChar(CP_ACP, 0, logFont->lfFaceName, -1, originalWide,
                            LF_FACESIZE)) {
        return;
    }
    wcscpy_s(wide.lfFaceName, originalWide);
    if (!ApplyToLogFontW(rt, &wide)) {
        return;
    }
    if (!EqualsCI(wide.lfFaceName, originalWide)) {
        char face[LF_FACESIZE] = {};
        if (!ConvertGdiFaceToAnsi(wide.lfFaceName, face)) {
            return;
        }
        strncpy_s(logFont->lfFaceName, face, _TRUNCATE);
    }
    logFont->lfWeight = wide.lfWeight;
}

HFONT WINAPI CreateFontIndirectW_Hook(const LOGFONTW* logFont) {
    if (t_internal) {
        return g_origCreateFontIndirectW(logFont);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !logFont) {
            return g_origCreateFontIndirectW(logFont);
        }
        LOGFONTW copy = *logFont;
        InternalScope scope;
        ApplyToLogFontW(*rt, &copy);
        return g_origCreateFontIndirectW(&copy);
    } catch (...) {
        return g_origCreateFontIndirectW(logFont);
    }
}

HFONT WINAPI CreateFontIndirectA_Hook(const LOGFONTA* logFont) {
    if (t_internal) {
        return g_origCreateFontIndirectA(logFont);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !logFont) {
            return g_origCreateFontIndirectA(logFont);
        }
        LOGFONTA copy = *logFont;
        InternalScope scope;
        ApplyToLogFontA(*rt, &copy);
        return g_origCreateFontIndirectA(&copy);
    } catch (...) {
        return g_origCreateFontIndirectA(logFont);
    }
}

template <typename T>
bool CopyExtendedLogFont(const T& original, T& copy) {
    const DWORD axes = original.elfDesignVector.dvNumAxes;
    if (axes > MM_MAX_NUMAXES) {
        return false;
    }
    const size_t size = offsetof(T, elfDesignVector) +
                        offsetof(DESIGNVECTOR, dvValues) + axes * sizeof(LONG);
    memcpy(&copy, &original, size);
    return true;
}

HFONT WINAPI CreateFontIndirectExW_Hook(const ENUMLOGFONTEXDVW* logFont) {
    if (t_internal) {
        return g_origCreateFontIndirectExW(logFont);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !logFont) {
            return g_origCreateFontIndirectExW(logFont);
        }
        ENUMLOGFONTEXDVW copy = {};
        if (!CopyExtendedLogFont(*logFont, copy)) {
            return g_origCreateFontIndirectExW(logFont);
        }
        InternalScope scope;
        ApplyToLogFontW(*rt, &copy.elfEnumLogfontEx.elfLogFont);
        return g_origCreateFontIndirectExW(&copy);
    } catch (...) {
        return g_origCreateFontIndirectExW(logFont);
    }
}

HFONT WINAPI CreateFontIndirectExA_Hook(const ENUMLOGFONTEXDVA* logFont) {
    if (t_internal) {
        return g_origCreateFontIndirectExA(logFont);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !logFont) {
            return g_origCreateFontIndirectExA(logFont);
        }
        ENUMLOGFONTEXDVA copy = {};
        if (!CopyExtendedLogFont(*logFont, copy)) {
            return g_origCreateFontIndirectExA(logFont);
        }
        InternalScope scope;
        ApplyToLogFontA(*rt, &copy.elfEnumLogfontEx.elfLogFont);
        return g_origCreateFontIndirectExA(&copy);
    } catch (...) {
        return g_origCreateFontIndirectExA(logFont);
    }
}

HFONT WINAPI CreateFontW_Hook(int height,
                              int width,
                              int escapement,
                              int orientation,
                              int weight,
                              DWORD italic,
                              DWORD underline,
                              DWORD strikeOut,
                              DWORD charset,
                              DWORD outPrecision,
                              DWORD clipPrecision,
                              DWORD quality,
                              DWORD pitchAndFamily,
                              LPCWSTR faceName) {
    if (t_internal) {
        return g_origCreateFontW(height, width, escapement, orientation, weight,
                                 italic, underline, strikeOut, charset,
                                 outPrecision, clipPrecision, quality,
                                 pitchAndFamily, faceName);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !faceName) {
            return g_origCreateFontW(height, width, escapement, orientation,
                                     weight, italic, underline, strikeOut,
                                     charset, outPrecision, clipPrecision,
                                     quality, pitchAndFamily, faceName);
        }

        std::wstring face = faceName;
        std::wstring newFace;
        int newWeight = weight;
        bool changed = false;
        InternalScope scope;
        ApplyGdiFaceDecision(*rt, face, static_cast<BYTE>(charset),
                             (pitchAndFamily & FIXED_PITCH) != 0, newWeight,
                             newFace, changed);
        if (!changed) {
            return g_origCreateFontW(height, width, escapement, orientation,
                                     weight, italic, underline, strikeOut,
                                     charset, outPrecision, clipPrecision,
                                     quality, pitchAndFamily, faceName);
        }
        return g_origCreateFontW(
            height, width, escapement, orientation, newWeight, italic,
            underline, strikeOut, charset, outPrecision, clipPrecision, quality,
            pitchAndFamily, newFace.empty() ? faceName : newFace.c_str());
    } catch (...) {
        return g_origCreateFontW(height, width, escapement, orientation, weight,
                                 italic, underline, strikeOut, charset,
                                 outPrecision, clipPrecision, quality,
                                 pitchAndFamily, faceName);
    }
}

HFONT WINAPI CreateFontA_Hook(int height,
                              int width,
                              int escapement,
                              int orientation,
                              int weight,
                              DWORD italic,
                              DWORD underline,
                              DWORD strikeOut,
                              DWORD charset,
                              DWORD outPrecision,
                              DWORD clipPrecision,
                              DWORD quality,
                              DWORD pitchAndFamily,
                              LPCSTR faceName) {
    if (t_internal) {
        return g_origCreateFontA(height, width, escapement, orientation, weight,
                                 italic, underline, strikeOut, charset,
                                 outPrecision, clipPrecision, quality,
                                 pitchAndFamily, faceName);
    }
    try {
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableGdi ||
            !rt->gdiValid || !faceName) {
            return g_origCreateFontA(height, width, escapement, orientation,
                                     weight, italic, underline, strikeOut,
                                     charset, outPrecision, clipPrecision,
                                     quality, pitchAndFamily, faceName);
        }

        std::wstring face;
        {
            wchar_t wide[LF_FACESIZE] = {};
            if (!MultiByteToWideChar(CP_ACP, 0, faceName, -1, wide,
                                     LF_FACESIZE)) {
                return g_origCreateFontA(height, width, escapement, orientation,
                                         weight, italic, underline, strikeOut,
                                         charset, outPrecision, clipPrecision,
                                         quality, pitchAndFamily, faceName);
            }
            face = wide;
        }

        std::wstring newFace;
        int newWeight = weight;
        bool changed = false;
        InternalScope scope;
        ApplyGdiFaceDecision(*rt, face, static_cast<BYTE>(charset),
                             (pitchAndFamily & FIXED_PITCH) != 0, newWeight,
                             newFace, changed);
        char ansiFace[LF_FACESIZE] = {};
        if (!changed || (!newFace.empty() &&
                         !ConvertGdiFaceToAnsi(newFace.c_str(), ansiFace))) {
            return g_origCreateFontA(height, width, escapement, orientation,
                                     weight, italic, underline, strikeOut,
                                     charset, outPrecision, clipPrecision,
                                     quality, pitchAndFamily, faceName);
        }

        return g_origCreateFontA(
            height, width, escapement, orientation, newWeight, italic,
            underline, strikeOut, charset, outPrecision, clipPrecision, quality,
            pitchAndFamily, newFace.empty() ? faceName : ansiFace);
    } catch (...) {
        return g_origCreateFontA(height, width, escapement, orientation, weight,
                                 italic, underline, strikeOut, charset,
                                 outPrecision, clipPrecision, quality,
                                 pitchAndFamily, faceName);
    }
}

// ---------------------------------------------------------------------------
// DirectWrite hooks
// ---------------------------------------------------------------------------

using CreateTextFormat_t = HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory*,
                                                       const WCHAR*,
                                                       IDWriteFontCollection*,
                                                       DWRITE_FONT_WEIGHT,
                                                       DWRITE_FONT_STYLE,
                                                       DWRITE_FONT_STRETCH,
                                                       FLOAT,
                                                       const WCHAR*,
                                                       IDWriteTextFormat**);

using CreateTextFormat6_t =
    HRESULT(STDMETHODCALLTYPE*)(IDWriteFactory6*,
                                const WCHAR*,
                                IDWriteFontCollection*,
                                const DWRITE_FONT_AXIS_VALUE*,
                                UINT32,
                                FLOAT,
                                const WCHAR*,
                                IDWriteTextFormat3**);

using MapCharacters_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFontFallback*, IDWriteTextAnalysisSource*, UINT32, UINT32,
    IDWriteFontCollection*, const WCHAR*, DWRITE_FONT_WEIGHT,
    DWRITE_FONT_STYLE, DWRITE_FONT_STRETCH, UINT32*, IDWriteFont**, FLOAT*);

using MapCharacters1_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteFontFallback1*, IDWriteTextAnalysisSource*, UINT32, UINT32,
    IDWriteFontCollection*, const WCHAR*, const DWRITE_FONT_AXIS_VALUE*,
    UINT32, UINT32*, FLOAT*, IDWriteFontFace5**);

using SetFontFamilyName_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteTextLayout*, const WCHAR*, DWRITE_TEXT_RANGE);

CreateTextFormat_t g_origCreateTextFormat = nullptr;
CreateTextFormat6_t g_origCreateTextFormat6 = nullptr;
MapCharacters_t g_origMapCharacters = nullptr;
MapCharacters1_t g_origMapCharacters1 = nullptr;
SetFontFamilyName_t g_origSetFontFamilyName = nullptr;

struct DWriteReplacement {
    const ReplacementFont* font = nullptr;
    int namedWeight = 0;
};

DWriteReplacement SelectDWriteReplacement(std::wstring_view family,
                                           const Runtime& rt,
                                           bool latePass = false) {
    auto segments = SplitList(family);
    if (segments.empty()) {
        return {};
    }
    // Keep the whole fallback list when a later entry is protected or kept.
    for (size_t i = 1; i < segments.size(); i++) {
        const auto lower = ToLower(segments[i]);
        if (IsSymbolName(lower) || IsProtectedScriptName(lower) ||
            IsExcludedFamily(lower, rt)) {
            return {};
        }
    }

    auto lower = ToLower(segments.front());
    const auto* font = SelectReplacement(lower, rt, latePass);
    if (!font || font->dwFamily.empty() ||
        IsProtectedFamily(lower, DEFAULT_CHARSET, false, rt,
                          font != &rt.defaultFont)) {
        return {};
    }
    return {font, NamedFontWeight(lower)};
}

std::shared_ptr<const Runtime> GetDirectWriteRuntime(PCWSTR family) {
    if (t_internal || !family || !*family) {
        return nullptr;
    }
    auto rt = GetRuntimeSnapshot();
    if (!rt || rt->settings.disableMod || rt->settings.disableDirectWrite ||
        !rt->dwValid) {
        return nullptr;
    }
    return rt;
}

std::atomic<bool> g_loggedDwSuccess{false};
std::atomic<bool> g_loggedDwMapping{false};
std::atomic<bool> g_loggedDwLayout{false};

std::vector<DWRITE_FONT_AXIS_VALUE> ChooseDWriteAxes(
    const DWRITE_FONT_AXIS_VALUE* axisValues, UINT32 axisCount,
    int namedWeight, const Runtime& rt) {
    std::vector<DWRITE_FONT_AXIS_VALUE> axes;
    if (axisCount) {
        axes.assign(axisValues, axisValues + axisCount);
    }
    if (rt.settings.fontWeight == 0 && !namedWeight) {
        return axes;
    }
    auto weightAxis = std::find_if(axes.begin(), axes.end(), [](const auto& axis) {
        return axis.axisTag == DWRITE_FONT_AXIS_TAG_WEIGHT;
    });
    if (weightAxis == axes.end()) {
        axes.push_back({DWRITE_FONT_AXIS_TAG_WEIGHT, static_cast<float>(
                            ChooseGdiWeight(SourceWeight(namedWeight, FW_NORMAL), rt))});
    } else {
        if (namedWeight && (weightAxis->value == 0.0f || weightAxis->value == 400.0f)) {
            weightAxis->value = static_cast<float>(namedWeight);
        }
        if (rt.settings.fontWeight &&
            (rt.settings.forceBaseWeight || weightAxis->value < 600.0f)) {
            weightAxis->value = static_cast<float>(rt.settings.fontWeight);
        }
    }
    return axes;
}

IDWriteFontCollection* SelectDWriteCollection(IDWriteFontCollection* collection,
                                             const wchar_t* family) {
    if (collection) {
        UINT32 index = 0;
        BOOL exists = FALSE;
        if (SUCCEEDED(collection->FindFamilyName(family, &index, &exists)) &&
            exists) {
            return collection;
        }
    }
    // DirectWrite accepts a name even when the supplied collection cannot
    // resolve it. Let the factory use its system collection in that case.
    return nullptr;
}

HRESULT STDMETHODCALLTYPE
CreateTextFormat_Hook(IDWriteFactory* self,
                      const WCHAR* familyName,
                      IDWriteFontCollection* collection,
                      DWRITE_FONT_WEIGHT weight,
                      DWRITE_FONT_STYLE style,
                      DWRITE_FONT_STRETCH stretch,
                      FLOAT size,
                      const WCHAR* locale,
                      IDWriteTextFormat** textFormat) {
    if (t_internal || !familyName) {
        return g_origCreateTextFormat(self, familyName, collection, weight,
                                      style, stretch, size, locale, textFormat);
    }
    try {
        auto rt = GetDirectWriteRuntime(familyName);
        const auto [font, namedWeight] =
            rt ? SelectDWriteReplacement(familyName, *rt) : DWriteReplacement{};
        if (!font) {
            return g_origCreateTextFormat(self, familyName, collection, weight,
                                          style, stretch, size, locale,
                                          textFormat);
        }

        InternalScope scope;
        DWRITE_FONT_WEIGHT newWeight = ChooseDWriteWeight(weight, namedWeight, *rt);
        auto* targetCollection =
            SelectDWriteCollection(collection, font->dwFamily.c_str());
        HRESULT hr = g_origCreateTextFormat(self, font->dwFamily.c_str(),
                                            targetCollection, newWeight, style,
                                            stretch, size, locale, textFormat);
        if (SUCCEEDED(hr)) {
            if (!g_loggedDwSuccess) {
                g_loggedDwSuccess = true;
                Wh_Log(L"DirectWrite created a format for '%s' as '%s' (weight %d -> %d)",
                       familyName, font->dwFamily.c_str(),
                       static_cast<int>(weight), static_cast<int>(newWeight));
            }
            return hr;
        }

        // Keep the caller's original format if creating the substitute fails.
        HRESULT retryHr =
            g_origCreateTextFormat(self, familyName, collection, weight, style,
                                   stretch, size, locale, textFormat);
        if (SUCCEEDED(retryHr)) {
            Wh_Log(L"DirectWrite fallback to original family '%s' (hr=0x%08X)",
                   familyName, static_cast<unsigned>(hr));
            return retryHr;
        }
        return hr;
    } catch (...) {
        return g_origCreateTextFormat(self, familyName, collection, weight,
                                      style, stretch, size, locale, textFormat);
    }
}

HRESULT STDMETHODCALLTYPE
CreateTextFormat6_Hook(IDWriteFactory6* self,
                       const WCHAR* familyName,
                       IDWriteFontCollection* collection,
                       const DWRITE_FONT_AXIS_VALUE* axisValues,
                       UINT32 axisCount,
                       FLOAT size,
                       const WCHAR* locale,
                       IDWriteTextFormat3** textFormat) {
    if (t_internal || !familyName || (axisCount && !axisValues)) {
        return g_origCreateTextFormat6(self, familyName, collection, axisValues,
                                       axisCount, size, locale, textFormat);
    }
    try {
        auto rt = GetDirectWriteRuntime(familyName);
        const auto [font, namedWeight] =
            rt ? SelectDWriteReplacement(familyName, *rt) : DWriteReplacement{};
        if (!font) {
            return g_origCreateTextFormat6(self, familyName, collection,
                                           axisValues, axisCount, size, locale,
                                           textFormat);
        }

        InternalScope scope;
        auto axes = ChooseDWriteAxes(axisValues, axisCount, namedWeight, *rt);

        auto* targetCollection =
            SelectDWriteCollection(collection, font->dwFamily.c_str());
        HRESULT hr = g_origCreateTextFormat6(
            self, font->dwFamily.c_str(), targetCollection, axes.data(),
            static_cast<UINT32>(axes.size()), size, locale, textFormat);
        if (SUCCEEDED(hr)) {
            return hr;
        }

        HRESULT retryHr =
            g_origCreateTextFormat6(self, familyName, collection, axisValues,
                                    axisCount, size, locale, textFormat);
        if (SUCCEEDED(retryHr)) {
            Wh_Log(L"DirectWrite6 fallback to original family '%s'",
                   familyName);
            return retryHr;
        }
        return hr;
    } catch (...) {
        return g_origCreateTextFormat6(self, familyName, collection, axisValues,
                                       axisCount, size, locale, textFormat);
    }
}

// XAML can create a default text format, then select the face for each
// actual text run through font fallback without using the format's family.
ComPtr<IDWriteFontCollection> GetMappingCollection(
    IDWriteFontCollection* collection, const ReplacementFont& font) {
    ComPtr<IDWriteFontCollection> result =
        SelectDWriteCollection(collection, font.dwFamily.c_str());
    if (!result) {
        g_dwFactory->GetSystemFontCollection(result.GetAddressOf(), FALSE);
    }
    return result;
}

void LogMappedFamily(PCWSTR family, const std::wstring& mappedFamily) {
    if (!g_loggedDwMapping.exchange(true, std::memory_order_relaxed)) {
        Wh_Log(L"DirectWrite selected '%s' to render text requested in '%s'",
               mappedFamily.c_str(), family);
    }
}

HRESULT STDMETHODCALLTYPE MapCharacters_Hook(
    IDWriteFontFallback* self, IDWriteTextAnalysisSource* source,
    UINT32 position, UINT32 length, IDWriteFontCollection* collection,
    const WCHAR* family, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style,
    DWRITE_FONT_STRETCH stretch, UINT32* mappedLength, IDWriteFont** mappedFont,
    FLOAT* scale) {
    try {
        auto rt = GetDirectWriteRuntime(family);
        const auto [font, namedWeight] =
            rt ? SelectDWriteReplacement(family, *rt, true) : DWriteReplacement{};
        if (font && length && mappedLength && mappedFont && scale) {
            InternalScope scope;
            auto target = GetMappingCollection(collection, *font);
            ComPtr<IDWriteFont> candidate;
            UINT32 candidateLength = 0;
            FLOAT candidateScale = 1.0f;
            if (target && SUCCEEDED(g_origMapCharacters(
                              self, source, position, length, target.Get(),
                              font->dwFamily.c_str(),
                              ChooseDWriteWeight(weight, namedWeight, *rt),
                              style, stretch, &candidateLength,
                              candidate.GetAddressOf(), &candidateScale)) &&
                candidate && candidateLength) {
                ComPtr<IDWriteFontFamily> actual;
                if (SUCCEEDED(candidate->GetFontFamily(actual.GetAddressOf()))) {
                    auto name = GetFamilyName(actual.Get());
                    if (IsTargetFamily(name, *font)) {
                        LogMappedFamily(family, name);
                        *mappedLength = candidateLength;
                        *scale = candidateScale;
                        *mappedFont = candidate.Detach();
                        return S_OK;
                    }
                }
            }
        }
    } catch (...) {
        // Preserve the original mapping if the substitute is unavailable.
    }
    return g_origMapCharacters(self, source, position, length, collection,
                                family, weight, style, stretch, mappedLength,
                                mappedFont, scale);
}

HRESULT STDMETHODCALLTYPE MapCharacters1_Hook(
    IDWriteFontFallback1* self, IDWriteTextAnalysisSource* source,
    UINT32 position, UINT32 length, IDWriteFontCollection* collection,
    const WCHAR* family, const DWRITE_FONT_AXIS_VALUE* axisValues,
    UINT32 axisCount, UINT32* mappedLength, FLOAT* scale,
    IDWriteFontFace5** mappedFace) {
    try {
        auto rt = GetDirectWriteRuntime(family);
        const auto [font, namedWeight] =
            rt ? SelectDWriteReplacement(family, *rt, true) : DWriteReplacement{};
        if (font && length && (!axisCount || axisValues) && mappedLength &&
            mappedFace && scale) {
            InternalScope scope;
            auto target = GetMappingCollection(collection, *font);
            auto axes = ChooseDWriteAxes(axisValues, axisCount, namedWeight, *rt);
            ComPtr<IDWriteFontFace5> candidate;
            UINT32 candidateLength = 0;
            FLOAT candidateScale = 1.0f;
            if (target && SUCCEEDED(g_origMapCharacters1(
                              self, source, position, length, target.Get(),
                              font->dwFamily.c_str(), axes.data(),
                              static_cast<UINT32>(axes.size()), &candidateLength,
                              &candidateScale, candidate.GetAddressOf())) &&
                candidate && candidateLength) {
                ComPtr<IDWriteLocalizedStrings> names;
                if (SUCCEEDED(candidate->GetFamilyNames(names.GetAddressOf()))) {
                    auto name = GetLocalizedFamilyName(names.Get());
                    if (IsTargetFamily(name, *font)) {
                        LogMappedFamily(family, name);
                        *mappedLength = candidateLength;
                        *scale = candidateScale;
                        *mappedFace = candidate.Detach();
                        return S_OK;
                    }
                }
            }
        }
    } catch (...) {
        // Preserve the original mapping if the substitute is unavailable.
    }
    return g_origMapCharacters1(self, source, position, length, collection,
                                 family, axisValues, axisCount, mappedLength,
                                 scale, mappedFace);
}

void ApplyNamedLayoutWeight(IDWriteTextLayout* layout, DWRITE_TEXT_RANGE range,
                            int namedWeight, const Runtime& rt) {
    if (!namedWeight || namedWeight == FW_NORMAL) {
        return;
    }
    const auto weight = ChooseDWriteWeight(DWRITE_FONT_WEIGHT_NORMAL,
                                           namedWeight, rt);
    if (weight == DWRITE_FONT_WEIGHT_NORMAL) {
        return;
    }
    const UINT64 end = static_cast<UINT64>(range.startPosition) + range.length;
    for (UINT64 position = range.startPosition; position < end;) {
        DWRITE_FONT_WEIGHT current{};
        DWRITE_TEXT_RANGE run{};
        const auto start = static_cast<UINT32>(position);
        if (FAILED(layout->GetFontWeight(start, &current, &run))) {
            return;
        }
        UINT64 next = std::min(end, static_cast<UINT64>(run.startPosition) + run.length);
        if (next <= position) {
            return;
        }
        if (current == DWRITE_FONT_WEIGHT_NORMAL &&
            FAILED(layout->SetFontWeight(
                weight, {start, static_cast<UINT32>(next - position)}))) {
            return;
        }
        position = next;
    }
}

HRESULT STDMETHODCALLTYPE SetFontFamilyName_Hook(
    IDWriteTextLayout* self, const WCHAR* family, DWRITE_TEXT_RANGE range) {
    try {
        auto rt = GetDirectWriteRuntime(family);
        const auto [font, namedWeight] =
            rt ? SelectDWriteReplacement(family, *rt) : DWriteReplacement{};
        if (font && range.length) {
            // Check every collection used by the affected text range before
            // replacing a family name, since a custom collection may lack it.
            UINT64 end = static_cast<UINT64>(range.startPosition) + range.length;
            UINT64 position = range.startPosition;
            while (position < end && position < UINT32_MAX) {
                ComPtr<IDWriteFontCollection> collection;
                DWRITE_TEXT_RANGE collectionRange{};
                if (FAILED(self->GetFontCollection(
                        static_cast<UINT32>(position), collection.GetAddressOf(),
                        &collectionRange)) ||
                    !SelectDWriteCollection(collection.Get(), font->dwFamily.c_str())) {
                    break;
                }
                UINT64 next = static_cast<UINT64>(collectionRange.startPosition) +
                              collectionRange.length;
                if (next <= position) {
                    break;
                }
                position = next;
            }
            if (position >= end) {
                InternalScope scope;
                HRESULT hr = g_origSetFontFamilyName(self, font->dwFamily.c_str(), range);
                if (SUCCEEDED(hr)) {
                    ApplyNamedLayoutWeight(self, range, namedWeight, *rt);
                    if (!g_loggedDwLayout.exchange(true, std::memory_order_relaxed)) {
                        Wh_Log(L"DirectWrite set a layout range from '%s' to '%s'",
                               family, font->dwFamily.c_str());
                    }
                    return hr;
                }
            }
        }
    } catch (...) {
        // Fall back to the family's original value.
    }
    return g_origSetFontFamilyName(self, family, range);
}

// ---------------------------------------------------------------------------
// Hook installation
// ---------------------------------------------------------------------------

template <typename T>
bool InstallFunctionHook(HMODULE module,
                         const char* name,
                         T hook,
                         T* original) {
    if (!module || !name || !hook || !original) {
        return false;
    }
    auto target = reinterpret_cast<T>(GetProcAddress(module, name));
    if (!target) {
        Wh_Log(L"GetProcAddress failed for %S", name);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"SetFunctionHook failed for %S", name);
        return false;
    }
    return true;
}

template <typename T>
bool InstallVtableHook(void** vtable, size_t index, T hook, T* original) {
    if (!vtable || !hook || !original) {
        return false;
    }
    auto target = reinterpret_cast<T>(vtable[index]);
    if (!target) {
        Wh_Log(L"Vtable entry %zu is null", index);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"SetFunctionHook failed for vtable entry %zu", index);
        return false;
    }
    return true;
}

void InstallGdiHooks(const Runtime& rt) {
    if (rt.settings.disableGdi || g_gdiHooksInstalled) {
        return;
    }
    if (!rt.gdiValid) {
        Wh_Log(L"GDI replacement disabled: no verified GDI face");
        return;
    }

    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    InstallFunctionHook(gdi32, "CreateFontIndirectW", CreateFontIndirectW_Hook,
                        &g_origCreateFontIndirectW);
    InstallFunctionHook(gdi32, "CreateFontIndirectA", CreateFontIndirectA_Hook,
                        &g_origCreateFontIndirectA);
    InstallFunctionHook(gdi32, "CreateFontIndirectExW",
                        CreateFontIndirectExW_Hook,
                        &g_origCreateFontIndirectExW);
    InstallFunctionHook(gdi32, "CreateFontIndirectExA",
                        CreateFontIndirectExA_Hook,
                        &g_origCreateFontIndirectExA);
    InstallFunctionHook(gdi32, "CreateFontW", CreateFontW_Hook,
                        &g_origCreateFontW);
    InstallFunctionHook(gdi32, "CreateFontA", CreateFontA_Hook,
                        &g_origCreateFontA);
    InstallFunctionHook(user32, "DrawTextW", DrawTextW_Hook, &g_origDrawTextW);
    InstallFunctionHook(user32, "DrawTextExW", DrawTextExW_Hook,
                        &g_origDrawTextExW);

    g_gdiHooksInstalled = true;
}

void InstallDirectWriteHooks(const Runtime& rt) {
    if (rt.settings.disableDirectWrite || g_dwHooksInstalled) {
        return;
    }
    if (!rt.dwValid || !g_dwFactory) {
        Wh_Log(L"DirectWrite replacement disabled: no verified family");
        return;
    }

    void** vtable = *reinterpret_cast<void***>(g_dwFactory);
    if (!InstallVtableHook(vtable, 15, CreateTextFormat_Hook,
                           &g_origCreateTextFormat)) {
        Wh_Log(L"Failed to hook IDWriteFactory::CreateTextFormat");
    }

    ComPtr<IDWriteTextFormat> testFormat;
    ComPtr<IDWriteTextLayout> testLayout;
    const auto* testFamily = &rt.defaultFont.dwFamily;
    if (testFamily->empty()) {
        for (const auto& rule : rt.fontRules) {
            if (!rule.replacement.dwFamily.empty()) {
                testFamily = &rule.replacement.dwFamily;
                break;
            }
        }
    }
    if (SUCCEEDED(g_dwFactory->CreateTextFormat(
            testFamily->c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f,
            L"en-us", testFormat.GetAddressOf())) &&
        SUCCEEDED(g_dwFactory->CreateTextLayout(
            L"x", 1, testFormat.Get(), 100.0f, 100.0f, testLayout.GetAddressOf()))) {
        auto** layoutVtable = *reinterpret_cast<void***>(testLayout.Get());
        InstallVtableHook(layoutVtable, 31, SetFontFamilyName_Hook,
                          &g_origSetFontFamilyName);
    } else {
        Wh_Log(L"Could not create a DirectWrite layout to install font hooks");
    }

    IDWriteFactory6* factory6 = nullptr;
    if (SUCCEEDED(g_dwFactory->QueryInterface(
            __uuidof(IDWriteFactory6), reinterpret_cast<void**>(&factory6))) &&
        factory6) {
        void** vtable6 = *reinterpret_cast<void***>(factory6);
        // IDWriteFactory6::CreateTextFormat (variable-axis overload).
        if (!InstallVtableHook(vtable6, 54, CreateTextFormat6_Hook,
                               &g_origCreateTextFormat6)) {
            Wh_Log(L"Failed to hook IDWriteFactory6::CreateTextFormat");
        }
        factory6->Release();
    } else {
        Wh_Log(
            L"IDWriteFactory6 is unavailable; variable-axis formats are not "
            L"hooked");
    }

    ComPtr<IDWriteFactory2> factory2;
    ComPtr<IDWriteFontFallback> fallback;
    if (SUCCEEDED(g_dwFactory->QueryInterface(IID_PPV_ARGS(factory2.GetAddressOf()))) &&
        SUCCEEDED(factory2->GetSystemFontFallback(fallback.GetAddressOf())) &&
        fallback) {
        auto** fallbackVtable = *reinterpret_cast<void***>(fallback.Get());
        InstallVtableHook(fallbackVtable, 3, MapCharacters_Hook,
                          &g_origMapCharacters);
        ComPtr<IDWriteFontFallback1> fallback1;
        if (SUCCEEDED(fallback.As(&fallback1))) {
            auto** fallback1Vtable = *reinterpret_cast<void***>(fallback1.Get());
            InstallVtableHook(fallback1Vtable, 4, MapCharacters1_Hook,
                              &g_origMapCharacters1);
        }
    } else {
        Wh_Log(L"Could not get the DirectWrite system font fallback");
    }

    g_dwHooksInstalled = true;
}

bool CreateDWriteFactory() {
    if (!g_dwModule) {
        g_dwModule =
            LoadLibraryExW(L"dwrite.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!g_dwModule) {
        Wh_Log(L"Failed to load dwrite.dll");
        return false;
    }
    using DWriteCreateFactory_t =
        HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
    auto createFactory = reinterpret_cast<DWriteCreateFactory_t>(
        GetProcAddress(g_dwModule, "DWriteCreateFactory"));
    if (!createFactory) {
        Wh_Log(L"DWriteCreateFactory was not found");
        return false;
    }
    HRESULT hr =
        createFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                      reinterpret_cast<IUnknown**>(&g_dwFactory));
    if (FAILED(hr) || !g_dwFactory) {
        if (g_dwFactory) {
            g_dwFactory->Release();
            g_dwFactory = nullptr;
        }
        Wh_Log(L"DWriteCreateFactory failed (hr=0x%08X)",
               static_cast<unsigned>(hr));
        return false;
    }
    return true;
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk callbacks
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    auto processName = GetProcessBaseName();

    std::shared_ptr<const Runtime> runtime;
    try {
        auto settings = LoadSettings(processName);
        if (settings.disableMod) {
            Wh_Log(L"Font replacement is off in '%s'", processName.c_str());
            return FALSE;
        }
        if (!settings.disableDirectWrite) {
            CreateDWriteFactory();
        }
        runtime = BuildRuntime(std::move(settings), false);
    } catch (...) {
        Wh_Log(L"Exception while resolving fonts; mod will stay pass-through");
        return TRUE;
    }
    PublishRuntime(runtime);
    LogInitInfo(*runtime, processName);

    if (runtime->gdiValid || runtime->dwValid) {
        InstallGdiHooks(*runtime);
        InstallDirectWriteHooks(*runtime);
    } else if (!runtime->settings.disableGdi || !runtime->settings.disableDirectWrite) {
        // Stay loaded so that installing the font and changing a setting can
        // recover without restarting the target process.
        Wh_Log(
            L"Font family '%s' is unavailable. Check the name and installation, "
            L"then change a setting to try again.", runtime->defaultFont.family.c_str());
    }

    Wh_Log(L"Initialization complete (GDI=%d DirectWrite=%d)",
           runtime->gdiValid ? 1 : 0, runtime->dwValid ? 1 : 0);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed; rebuilding runtime");
    auto processName = GetProcessBaseName();
    std::shared_ptr<Runtime> runtime;
    try {
        auto settings = LoadSettings(processName);
        if (settings.disableMod) {
            PublishRuntime(nullptr);
            Wh_Log(L"Font replacement is off in '%s'", processName.c_str());
            return;
        }
        if (!settings.disableDirectWrite && !g_dwFactory) {
            CreateDWriteFactory();
        }
        runtime = BuildRuntime(std::move(settings), true);
    } catch (...) {
        Wh_Log(
            L"Exception while rebuilding settings; keeping the previous "
            L"runtime");
        return;
    }
    PublishRuntime(runtime);
    LogInitInfo(*runtime, processName);
    const bool hadGdiHooks = g_gdiHooksInstalled;
    const bool hadDwHooks = g_dwHooksInstalled;
    InstallGdiHooks(*runtime);
    InstallDirectWriteHooks(*runtime);
    if ((g_gdiHooksInstalled != hadGdiHooks ||
         g_dwHooksInstalled != hadDwHooks) &&
        !Wh_ApplyHookOperations()) {
        Wh_Log(L"Failed to apply newly installed font hooks");
    }
    Wh_Log(
        L"Settings applied. Existing UI text may keep the old font until "
        L"the target process recreates it; restart Explorer for a full "
        L"refresh.");
}

void Wh_ModUninit() {
    PublishRuntime(nullptr);
    g_gdiHooksInstalled = false;
    g_dwHooksInstalled = false;
    if (g_dwFactory) {
        g_dwFactory->Release();
        g_dwFactory = nullptr;
    }
    if (g_dwModule) {
        FreeLibrary(g_dwModule);
        g_dwModule = nullptr;
    }
    Wh_Log(L"Uninitialized");
}
