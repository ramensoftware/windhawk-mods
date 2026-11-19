// ==WindhawkMod==
// @id              change-more-fonts
// @name            Change More Fonts
// @description     Use an installed font for Windows 11 text in Explorer, Start, Settings, and other shell surfaces.
// @version         1.0.0
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
  - ReplaceMonospace: false
    $name: Include monospace fonts
    $description: Applies to monospace families on your list or when you replace all eligible fonts.
  - AdditionalFonts: ""
    $name: Also replace these fonts
    $description: Enter family names separated by commas.
  - ExcludedFonts: ""
    $name: Keep these fonts
    $description: Enter family names separated by commas. This list takes priority over the replacement lists.
  $name: Fonts to replace
*/
// ==/WindhawkModSettings==

#include <dwrite.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using Microsoft::WRL::ComPtr;

constexpr wchar_t kDefaultFamily[] = L"Tahoma";

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
    int fontWeight = 0;
    bool forceBaseWeight = false;
    bool disableMod = false;
    bool disableGdi = false;
    bool disableGdiDrawing = false;
    bool disableDirectWrite = false;
    bool replaceAllFonts = false;
    bool replaceMonospace = false;
    std::vector<std::wstring> additionalFamilies;
    std::vector<std::wstring> extraExcludedFamilies;
};

struct Runtime {
    Settings settings;
    std::wstring dwFamily;
    std::wstring gdiFaceBase;
    std::wstring gdiFaceBold;
    bool dwValid = false;
    bool gdiValid = false;
    std::vector<std::wstring> replaceFamilies;
    std::vector<std::wstring> excludedFamilies;
};

std::shared_ptr<const Runtime> g_runtime;
SRWLOCK g_runtimeLock = SRWLOCK_INIT;
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
    AcquireSRWLockShared(&g_runtimeLock);
    auto runtime = g_runtime;
    ReleaseSRWLockShared(&g_runtimeLock);
    return runtime;
}

void PublishRuntime(std::shared_ptr<const Runtime> runtime) noexcept {
    AcquireSRWLockExclusive(&g_runtimeLock);
    g_runtime.swap(runtime);
    ReleaseSRWLockExclusive(&g_runtimeLock);
}

// ---------------------------------------------------------------------------
// String helpers
// ---------------------------------------------------------------------------

std::wstring ReadStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
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
                       bool fixedPitch, const Runtime& rt) {
    return lowerName.empty() || lowerName[0] == L'@' ||
           IsUnsupportedCharset(charset) ||
           (!rt.settings.replaceMonospace &&
            (fixedPitch || IsMonospaceName(lowerName))) ||
           IsSymbolName(lowerName) || IsProtectedScriptName(lowerName) ||
           IsExcludedFamily(lowerName, rt);
}

bool IsTargetFamily(std::wstring_view name, const Runtime& rt) {
    return !name.empty() &&
           (EqualsCI(name, rt.settings.fontFamily) ||
            EqualsCI(name, rt.dwFamily) || EqualsCI(name, rt.gdiFaceBase) ||
            EqualsCI(name, rt.gdiFaceBold));
}

bool IsReplaceListed(const std::wstring& lowerName, const Runtime& rt) {
    for (const auto& family : rt.replaceFamilies) {
        if (EqualsCI(lowerName, family)) {
            return true;
        }
    }
    return false;
}

int ChooseGdiWeight(int originalWeight, const Runtime& rt) {
    if (rt.settings.fontWeight == 0 ||
        (originalWeight >= 600 && !rt.settings.forceBaseWeight)) {
        return originalWeight;
    }
    return rt.settings.fontWeight;
}

DWRITE_FONT_WEIGHT ChooseDWriteWeight(DWRITE_FONT_WEIGHT originalWeight,
                                      const Runtime& rt) {
    return static_cast<DWRITE_FONT_WEIGHT>(ChooseGdiWeight(originalWeight, rt));
}

// ---------------------------------------------------------------------------
// Logging
// ---------------------------------------------------------------------------

void LogInitInfo(const Runtime& rt, const std::wstring& processName) {
    Wh_Log(L"Change More Fonts in %s", processName.c_str());
    Wh_Log(L"Requested family='%s' weight=%d forceBaseWeight=%d replaceAll=%d",
           rt.settings.fontFamily.c_str(), rt.settings.fontWeight,
           rt.settings.forceBaseWeight ? 1 : 0,
           rt.settings.replaceAllFonts ? 1 : 0);
    Wh_Log(L"DirectWrite family='%s' valid=%d", rt.dwFamily.c_str(),
           rt.dwValid ? 1 : 0);
    Wh_Log(L"GDI base face='%s' bold face='%s' valid=%d",
           rt.gdiFaceBase.c_str(), rt.gdiFaceBold.c_str(), rt.gdiValid ? 1 : 0);
    Wh_Log(
        L"Known limits: DWM title bars, WebView2 content and layout ranges "
        L"with custom font collections lacking the target family.");
}

// ---------------------------------------------------------------------------
// GDI face resolution
// ---------------------------------------------------------------------------

struct GdiCandidate {
    std::wstring name;
    int weight = 400;
    bool italic = false;
};

struct GdiEnumContext {
    std::wstring familyLower;
    std::vector<GdiCandidate> candidates;
};

std::wstring StripWeightSuffix(std::wstring name);

int CALLBACK GdiEnumProc(const LOGFONTW* logFont,
                         const TEXTMETRICW*,
                         DWORD,
                         LPARAM lParam) {
    auto* context = reinterpret_cast<GdiEnumContext*>(lParam);
    if (!logFont || !context) {
        return 1;
    }
    std::wstring name(logFont->lfFaceName,
                      wcsnlen(logFont->lfFaceName, LF_FACESIZE));
    if (name.empty() || name[0] == L'@') {
        return 1;
    }
    std::wstring lower = ToLower(name);
    if (lower != context->familyLower &&
        StripWeightSuffix(lower) != context->familyLower) {
        return 1;
    }
    for (const auto& candidate : context->candidates) {
        if (EqualsCI(candidate.name, name) &&
            candidate.weight == logFont->lfWeight &&
            candidate.italic == (logFont->lfItalic != 0)) {
            return 1;
        }
    }
    context->candidates.push_back(
        {name, logFont->lfWeight, logFont->lfItalic != 0});
    return 1;
}

bool VerifyGdiFace(const std::wstring& face, int weight) {
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
        return false;
    }

    bool ok = false;
    HDC dc = CreateCompatibleDC(nullptr);
    if (dc) {
        HGDIOBJ old = SelectObject(dc, font);
        if (old && old != HGDI_ERROR) {
            wchar_t selected[LF_FACESIZE] = {};
            if (GetTextFaceW(dc, LF_FACESIZE, selected) > 0) {
                ok = EqualsCI(selected, face);
            }
            SelectObject(dc, old);
        }
        DeleteDC(dc);
    }
    DeleteObject(font);
    return ok;
}

std::wstring ChooseGdiFace(const std::vector<GdiCandidate>& candidates,
                           int targetWeight,
                           const std::wstring& family,
                           bool* found) {
    std::vector<const GdiCandidate*> ordered;
    for (const auto& candidate : candidates) {
        if (!candidate.italic) {
            ordered.push_back(&candidate);
        }
    }
    if (ordered.empty()) {
        *found = false;
        return {};
    }

    std::sort(
        ordered.begin(), ordered.end(),
        [&](const GdiCandidate* a, const GdiCandidate* b) {
            auto score = [&](const GdiCandidate* c) {
                long long value =
                    static_cast<long long>(std::abs(c->weight - targetWeight)) *
                    100;
                if (!EqualsCI(c->name, family)) {
                    value += 10;
                }
                value += static_cast<long long>(c->name.size());
                return value;
            };
            return score(a) < score(b);
        });

    for (const auto* candidate : ordered) {
        if (VerifyGdiFace(candidate->name, targetWeight)) {
            *found = true;
            return candidate->name;
        }
    }

    *found = false;
    return {};
}

void ResolveGdiFaces(Runtime& rt) {
    rt.gdiValid = false;
    rt.gdiFaceBase.clear();
    rt.gdiFaceBold.clear();

    if (rt.settings.fontFamily.size() > LF_FACESIZE - 1) {
        Wh_Log(L"Font family is too long for GDI (max %d): %s", LF_FACESIZE - 1,
               rt.settings.fontFamily.c_str());
        return;
    }

    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc) {
        Wh_Log(L"CreateCompatibleDC failed; GDI replacement disabled");
        return;
    }

    GdiEnumContext context;
    context.familyLower = ToLower(rt.settings.fontFamily);
    LOGFONTW filter = {};
    filter.lfCharSet = DEFAULT_CHARSET;
    EnumFontFamiliesExW(dc, &filter, GdiEnumProc,
                        reinterpret_cast<LPARAM>(&context), 0);

    if (context.candidates.empty()) {
        Wh_Log(L"No GDI face matching '%s' was found; GDI replacement disabled",
               rt.settings.fontFamily.c_str());
        DeleteDC(dc);
        return;
    }

    for (size_t i = 0; i < context.candidates.size() && i < 20; i++) {
        Wh_Log(L"GDI candidate: face='%s' weight=%d italic=%d",
               context.candidates[i].name.c_str(), context.candidates[i].weight,
               context.candidates[i].italic ? 1 : 0);
    }

    int baseWeight = rt.settings.fontWeight ? rt.settings.fontWeight : FW_NORMAL;
    bool baseFound = false;
    auto base = ChooseGdiFace(context.candidates, baseWeight,
                              rt.settings.fontFamily, &baseFound);
    if (!baseFound) {
        Wh_Log(L"Could not verify a GDI face for weight %d", baseWeight);
        DeleteDC(dc);
        return;
    }

    bool boldFound = false;
    auto bold = ChooseGdiFace(context.candidates, 700, rt.settings.fontFamily,
                              &boldFound);
    if (!boldFound) {
        bold = base;
    }

    rt.gdiFaceBase = std::move(base);
    rt.gdiFaceBold = std::move(bold);
    rt.gdiValid = true;
    DeleteDC(dc);
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

std::wstring StripWeightSuffix(std::wstring name) {
    static constexpr const wchar_t* kSuffixes[] = {
        L" thin",     L" extralight", L" extra light", L" light",
        L" regular",  L" normal",     L" book",        L" medium",
        L" semibold", L" semi bold",  L" semilight",   L" semi light",
        L" bold",     L" extrabold",  L" extra bold",  L" black",
        L" heavy",    L" italic",     L" oblique",
    };
    auto lower = ToLower(name);
    for (auto suffix : kSuffixes) {
        size_t suffixLength = wcslen(suffix);
        if (lower.size() > suffixLength && lower.ends_with(suffix)) {
            auto stripped = Trim(name.substr(0, name.size() - suffixLength));
            if (!stripped.empty()) {
                return stripped;
            }
        }
    }
    return name;
}

bool ResolveDWriteFamily(Runtime& rt) {
    rt.dwValid = false;
    rt.dwFamily.clear();
    if (!g_dwFactory || rt.settings.fontFamily.empty()) {
        return false;
    }

    IDWriteFontCollection* collection = nullptr;
    if (FAILED(g_dwFactory->GetSystemFontCollection(&collection, FALSE)) ||
        !collection) {
        Wh_Log(
            L"GetSystemFontCollection failed; DirectWrite replacement "
            L"disabled");
        return false;
    }

    std::wstring resolved;
    bool found =
        FindDWriteFamilyExact(collection, rt.settings.fontFamily, &resolved);

    if (!found) {
        auto stripped = StripWeightSuffix(rt.settings.fontFamily);
        if (!EqualsCI(stripped, rt.settings.fontFamily)) {
            found = FindDWriteFamilyExact(collection, stripped, &resolved);
        }
    }

    if (!found) {
        Wh_Log(
            L"DirectWrite family '%s' was not found; DirectWrite replacement "
            L"disabled",
            rt.settings.fontFamily.c_str());
        collection->Release();
        return false;
    }

    int baseWeight = rt.settings.fontWeight ? rt.settings.fontWeight : FW_NORMAL;
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
        collection->Release();
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

    rt.dwFamily = std::move(resolved);
    rt.dwValid = true;
    collection->Release();
    return true;
}

// ---------------------------------------------------------------------------
// Settings and runtime construction
// ---------------------------------------------------------------------------

Settings LoadSettings() {
    Settings settings;

    {
        auto family = Trim(ReadStringSetting(L"General.FontFamily"));
        // The mod expects one family. If a fallback list was pasted, use its
        // first entry rather than failing to resolve the whole string.
        auto parts = SplitList(family);
        if (!parts.empty()) {
            family = std::move(parts[0]);
        }
        if (!family.empty()) {
            settings.fontFamily = std::move(family);
        }
    }

    int weight = Wh_GetIntSetting(L"General.FontWeight");
    settings.fontWeight = weight == 0 ? 0 : std::clamp(weight, 100, 900);
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
    settings.replaceMonospace =
        Wh_GetIntSetting(L"Exclusions.ReplaceMonospace") != 0;

    settings.additionalFamilies =
        SplitList(ReadStringSetting(L"Exclusions.AdditionalFonts"));
    settings.extraExcludedFamilies =
        SplitList(ReadStringSetting(L"Exclusions.ExcludedFonts"));

    return settings;
}

std::shared_ptr<Runtime> BuildRuntime() {
    InternalScope internalScope;

    auto runtime = std::make_shared<Runtime>();
    runtime->settings = LoadSettings();

    for (auto family : kDefaultReplaceFamilies) {
        AddUnique(runtime->replaceFamilies, family);
    }
    for (auto& family : runtime->settings.additionalFamilies) {
        AddUnique(runtime->replaceFamilies, family);
    }
    for (auto& family : runtime->settings.extraExcludedFamilies) {
        AddUnique(runtime->excludedFamilies, family);
    }

    ResolveGdiFaces(*runtime);
    ResolveDWriteFamily(*runtime);
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
                          bool& changed) {
    changed = false;
    std::wstring lower = ToLower(Trim(face));
    if (IsProtectedFamily(lower, charset, fixedPitch, rt)) {
        return;
    }

    if (!IsTargetFamily(lower, rt) && !rt.settings.replaceAllFonts &&
        !IsReplaceListed(lower, rt)) {
        return;
    }

    int newWeight = ChooseGdiWeight(weight, rt);
    const auto& replacement = newWeight >= 700 ? rt.gdiFaceBold : rt.gdiFaceBase;
    if (!EqualsCI(replacement, face)) {
        newFace = replacement;
        changed = true;
    }
    if (weight != newWeight) {
        weight = newWeight;
        changed = true;
    }
}

bool ApplyToLogFontW(const Runtime& rt, LOGFONTW* logFont) {
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
                         newFace, changed);
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
            if (!ApplyToLogFontW(rt, &logFont)) {
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

bool IsSymbolSegment(const std::wstring& segment) {
    auto lower = ToLower(Trim(segment));
    return IsSymbolName(lower) || IsProtectedScriptName(lower);
}

bool ShouldReplaceDWriteFamily(const std::wstring& family, const Runtime& rt) {
    auto segments = SplitList(family);
    if (segments.empty()) {
        return false;
    }
    // If a XAML font-family fallback list contains an icon/emoji face, keep
    // the whole list. DWrite will use its normal fallback chain.
    for (size_t i = 1; i < segments.size(); i++) {
        if (IsSymbolSegment(segments[i])) {
            return false;
        }
    }

    auto lower = ToLower(segments.front());
    if (IsProtectedFamily(lower, DEFAULT_CHARSET, false, rt)) {
        return false;
    }

    return IsTargetFamily(lower, rt) || rt.settings.replaceAllFonts ||
           IsReplaceListed(lower, rt);
}

std::atomic<bool> g_loggedDwSuccess{false};
std::atomic<bool> g_loggedDwMapping{false};
std::atomic<bool> g_loggedDwLayout{false};

std::vector<DWRITE_FONT_AXIS_VALUE> ChooseDWriteAxes(
    const DWRITE_FONT_AXIS_VALUE* axisValues, UINT32 axisCount,
    const Runtime& rt) {
    std::vector<DWRITE_FONT_AXIS_VALUE> axes;
    if (axisCount) {
        axes.assign(axisValues, axisValues + axisCount);
    }
    if (rt.settings.fontWeight == 0) {
        return axes;
    }
    auto weightAxis = std::find_if(axes.begin(), axes.end(), [](const auto& axis) {
        return axis.axisTag == DWRITE_FONT_AXIS_TAG_WEIGHT;
    });
    if (weightAxis == axes.end()) {
        axes.push_back({DWRITE_FONT_AXIS_TAG_WEIGHT,
                        static_cast<float>(rt.settings.fontWeight)});
    } else if (rt.settings.forceBaseWeight || weightAxis->value < 600.0f) {
        weightAxis->value = static_cast<float>(rt.settings.fontWeight);
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
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableDirectWrite ||
            !rt->dwValid || rt->dwFamily.empty()) {
            return g_origCreateTextFormat(self, familyName, collection, weight,
                                          style, stretch, size, locale,
                                          textFormat);
        }

        std::wstring family = familyName;

        if (!ShouldReplaceDWriteFamily(family, *rt)) {
            return g_origCreateTextFormat(self, familyName, collection, weight,
                                          style, stretch, size, locale,
                                          textFormat);
        }

        DWRITE_FONT_WEIGHT newWeight = ChooseDWriteWeight(weight, *rt);
        auto* targetCollection =
            SelectDWriteCollection(collection, rt->dwFamily.c_str());
        HRESULT hr = g_origCreateTextFormat(self, rt->dwFamily.c_str(),
                                            targetCollection, newWeight, style,
                                            stretch, size, locale, textFormat);
        if (SUCCEEDED(hr)) {
            if (!g_loggedDwSuccess) {
                g_loggedDwSuccess = true;
                Wh_Log(L"DirectWrite created a format for '%s' as '%s' (weight %d -> %d)",
                       family.c_str(), rt->dwFamily.c_str(),
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
                   family.c_str(), static_cast<unsigned>(hr));
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
        auto rt = GetRuntimeSnapshot();
        if (!rt || rt->settings.disableMod || rt->settings.disableDirectWrite ||
            !rt->dwValid || rt->dwFamily.empty()) {
            return g_origCreateTextFormat6(self, familyName, collection,
                                           axisValues, axisCount, size, locale,
                                           textFormat);
        }

        std::wstring family = familyName;
        if (!ShouldReplaceDWriteFamily(family, *rt)) {
            return g_origCreateTextFormat6(self, familyName, collection,
                                           axisValues, axisCount, size, locale,
                                           textFormat);
        }

        auto axes = ChooseDWriteAxes(axisValues, axisCount, *rt);

        auto* targetCollection =
            SelectDWriteCollection(collection, rt->dwFamily.c_str());
        HRESULT hr = g_origCreateTextFormat6(
            self, rt->dwFamily.c_str(), targetCollection, axes.data(),
            static_cast<UINT32>(axes.size()), size, locale, textFormat);
        if (SUCCEEDED(hr)) {
            return hr;
        }

        HRESULT retryHr =
            g_origCreateTextFormat6(self, familyName, collection, axisValues,
                                    axisCount, size, locale, textFormat);
        if (SUCCEEDED(retryHr)) {
            Wh_Log(L"DirectWrite6 fallback to original family '%s'",
                   family.c_str());
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
std::shared_ptr<const Runtime> GetMappingRuntime(PCWSTR family) {
    if (t_internal || !family || !*family) {
        return nullptr;
    }
    auto rt = GetRuntimeSnapshot();
    if (!rt || rt->settings.disableMod || rt->settings.disableDirectWrite ||
        !rt->dwValid || !ShouldReplaceDWriteFamily(family, *rt)) {
        return nullptr;
    }
    return rt;
}

ComPtr<IDWriteFontCollection> GetMappingCollection(
    IDWriteFontCollection* collection, const Runtime& rt) {
    ComPtr<IDWriteFontCollection> result =
        SelectDWriteCollection(collection, rt.dwFamily.c_str());
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
        if (auto rt = GetMappingRuntime(family);
            rt && length && mappedLength && mappedFont && scale) {
            InternalScope scope;
            auto target = GetMappingCollection(collection, *rt);
            ComPtr<IDWriteFont> candidate;
            UINT32 candidateLength = 0;
            FLOAT candidateScale = 1.0f;
            if (target && SUCCEEDED(g_origMapCharacters(
                              self, source, position, length, target.Get(),
                              rt->dwFamily.c_str(), ChooseDWriteWeight(weight, *rt),
                              style, stretch, &candidateLength,
                              candidate.GetAddressOf(), &candidateScale)) &&
                candidate && candidateLength) {
                ComPtr<IDWriteFontFamily> actual;
                if (SUCCEEDED(candidate->GetFontFamily(actual.GetAddressOf()))) {
                    auto name = GetFamilyName(actual.Get());
                    if (IsTargetFamily(name, *rt)) {
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
        if (auto rt = GetMappingRuntime(family);
            rt && length && (!axisCount || axisValues) && mappedLength &&
            mappedFace && scale) {
            InternalScope scope;
            auto target = GetMappingCollection(collection, *rt);
            auto axes = ChooseDWriteAxes(axisValues, axisCount, *rt);
            ComPtr<IDWriteFontFace5> candidate;
            UINT32 candidateLength = 0;
            FLOAT candidateScale = 1.0f;
            if (target && SUCCEEDED(g_origMapCharacters1(
                              self, source, position, length, target.Get(),
                              rt->dwFamily.c_str(), axes.data(),
                              static_cast<UINT32>(axes.size()), &candidateLength,
                              &candidateScale, candidate.GetAddressOf())) &&
                candidate && candidateLength) {
                ComPtr<IDWriteLocalizedStrings> names;
                if (SUCCEEDED(candidate->GetFamilyNames(names.GetAddressOf()))) {
                    auto name = GetLocalizedFamilyName(names.Get());
                    if (IsTargetFamily(name, *rt)) {
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

HRESULT STDMETHODCALLTYPE SetFontFamilyName_Hook(
    IDWriteTextLayout* self, const WCHAR* family, DWRITE_TEXT_RANGE range) {
    try {
        if (auto rt = GetMappingRuntime(family); rt && range.length) {
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
                    !SelectDWriteCollection(collection.Get(), rt->dwFamily.c_str())) {
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
                HRESULT hr = g_origSetFontFamilyName(self, rt->dwFamily.c_str(), range);
                if (SUCCEEDED(hr)) {
                    if (!g_loggedDwLayout.exchange(true, std::memory_order_relaxed)) {
                        Wh_Log(L"DirectWrite set a layout range from '%s' to '%s'",
                               family, rt->dwFamily.c_str());
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
    if (g_gdiHooksInstalled) {
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
    if (g_dwHooksInstalled) {
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
    if (SUCCEEDED(g_dwFactory->CreateTextFormat(
            rt.dwFamily.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
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
    CreateDWriteFactory();

    std::shared_ptr<const Runtime> runtime;
    try {
        runtime = BuildRuntime();
    } catch (...) {
        Wh_Log(L"Exception while resolving fonts; mod will stay pass-through");
        return TRUE;
    }
    PublishRuntime(runtime);
    LogInitInfo(*runtime, processName);

    if (runtime->gdiValid || runtime->dwValid) {
        InstallGdiHooks(*runtime);
        InstallDirectWriteHooks(*runtime);
    } else {
        // Stay loaded so that installing the font and changing a setting can
        // recover without restarting the target process.
        Wh_Log(
            L"Font family '%s' is unavailable. Check the name and installation, "
            L"then change a setting to try again.", runtime->settings.fontFamily.c_str());
    }

    Wh_Log(L"Initialization complete (GDI=%d DirectWrite=%d)",
           runtime->gdiValid ? 1 : 0, runtime->dwValid ? 1 : 0);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed; rebuilding runtime");
    if (!g_dwFactory) {
        CreateDWriteFactory();
    }
    std::shared_ptr<Runtime> runtime;
    try {
        runtime = BuildRuntime();
    } catch (...) {
        Wh_Log(
            L"Exception while rebuilding settings; keeping the previous "
            L"runtime");
        return;
    }
    PublishRuntime(runtime);
    LogInitInfo(*runtime, GetProcessBaseName());
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
